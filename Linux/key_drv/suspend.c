/*
 * arch/arm/mach-vienna/suspend.c
 *
 * Copyright (C) 2018 VATICS Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/cpu_pm.h>
#include <linux/io.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#include <asm/cacheflush.h>
#include <asm/mcpm.h>
#include <asm/suspend.h>
#include <mach/maps.h>
#include <linux/console.h>

#include "suspend.h"

#define LP_STAT_OFS		0x4
#define PUB_ZQ0SR0_OFS		0x188
#define SYSC_CLK1_OFS		0x11c
#define MCU_SUS_TIME_IDX	0x10
#define GIU_META		0x83
#define LP_STAT_VAL		0x3
#define I2C_SLAVE_ADDR          0x1f
#define POWER_KEYPAD_SUPPORT

static struct vatics_reserved_block_s info;
static void *lp_ctrl;
struct i2c_regs *i2c_base;
static void *pub_zqsr_mmr;
static void *sysc_clk1_reg;
static unsigned int zqsr_val;
static unsigned int zqsr_val1;
static unsigned int i2c_clk_bit;
static int suspend_time;/*seconds*/

/*
 * if test_mode = 1, don't clear suspend_time
 */
static int test_mode;

module_param(suspend_time, int, 0644);
module_param(test_mode, int, 0644);
#ifdef POWER_KEYPAD_SUPPORT
static struct input_dev *gpio_key_dev;
#if !defined(CONFIG_OF)
static struct platform_device *power_keys_device;
#endif
#endif

static int i2c_wait_for_bb(void)
{
	while ((readl(&i2c_base->ic_status) & IC_STATUS_MA) ||
	       !(readl(&i2c_base->ic_status) & IC_STATUS_TFE))
		;

	return 0;
}

static void i2c_setaddress(int i2c_addr)
{
	unsigned int enbl;

	/* Disable i2c */
	enbl = __raw_readl(&i2c_base->ic_enable);
	enbl &= ~IC_ENABLE_0B;
	__raw_writel(enbl, &i2c_base->ic_enable);

	__raw_writel(i2c_addr, &i2c_base->ic_tar);

	/* Enable i2c */
	enbl = readl(&i2c_base->ic_enable);
	enbl |= IC_ENABLE_0B;
	writel(enbl, &i2c_base->ic_enable);
}

static void i2c_flush_rxfifo(void)
{
	while (__raw_readl(&i2c_base->ic_status) & IC_STATUS_RFNE)
		__raw_readl(&i2c_base->ic_cmd_data);
}
static int i2c_xfer_init(unsigned char chip, unsigned int addr,
			 int alen)
{
	int ret = i2c_wait_for_bb();

	if (ret)
		return ret;

	i2c_setaddress(chip);
	while (alen) {
		alen--;
		/* high byte address going out first */
		__raw_writel((addr >> (alen * 8)) & 0xff,
		       &i2c_base->ic_cmd_data);
	}
	while (!(__raw_readl(&i2c_base->ic_raw_intr_stat) & IC_TX_EMPTY))
		;
	return 0;
}
static int i2c_xfer_finish(void)
{
	while (1) {
		if ((__raw_readl(&i2c_base->ic_raw_intr_stat) & IC_STOP_DET)) {
			readl(&i2c_base->ic_clr_stop_det);
			break;
		}
	}

	if (i2c_wait_for_bb())
		return 5;

	i2c_flush_rxfifo();

	return 0;
}
static unsigned int i2c_read(unsigned int reg)
{
	int len = 4;
	int alen = 1;
	unsigned char buf[4];
	unsigned char *buffer;
	unsigned char dev_addr = I2C_SLAVE_ADDR;
	unsigned int val = 0;

	int ret = i2c_xfer_init(dev_addr, reg, alen);

	if (ret)
		return ret;

	buffer = buf;

	if (len == 1)
		__raw_writel(IC_CMD | IC_STOP, &i2c_base->ic_cmd_data);
	else if (len > 0)
		__raw_writel(IC_CMD, &i2c_base->ic_cmd_data);

	while (len) {
		if ((__raw_readl(&i2c_base->ic_status) & IC_STATUS_RFNE) != 0) {
			*buffer++ = (unsigned char)
				__raw_readl(&i2c_base->ic_cmd_data);
			len--;
			if (len == 1)
				__raw_writel(IC_CMD | IC_STOP,
					&i2c_base->ic_cmd_data);
			else if (len > 0)
				__raw_writel(IC_CMD,
					&i2c_base->ic_cmd_data);
		}
	}

	i2c_xfer_finish();
	val = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	return val;
}

static int i2c_write(unsigned int reg, unsigned int value)
{
	int len = 4;
	int alen = 1;
	unsigned char buf[4];
	unsigned char *buffer;
	unsigned char dev_addr = I2C_SLAVE_ADDR;
	int ret;

	buf[0] = (value & 0xFF << 24) >> 24;
	buf[1] = (value & 0xFF << 16) >> 16;
	buf[2] = (value & 0xFF << 8) >> 8;
	buf[3] = (value & 0xFF << 0) >> 0;
	buffer = buf;

	ret = i2c_xfer_init(dev_addr, reg, alen);
	if (ret)
		return ret;

	while (len) {
		if (__raw_readl(&i2c_base->ic_status) & IC_STATUS_TFNF) {
			if (--len == 0)
				__raw_writel(*buffer | IC_STOP,
				       &i2c_base->ic_cmd_data);
			else
				__raw_writel(*buffer, &i2c_base->ic_cmd_data);
			buffer++;
		}
	}

	return i2c_xfer_finish();
}

static int __maybe_unused set_suspend_status(void)
{
	int ret = 0;
	return ret;
}

static void enter_lp_state(void)
{
	__raw_writel(LP_STAT_VAL, lp_ctrl);
}

static void vatics_flush_cache_all(void)
{
	flush_cache_all();
	outer_flush_all();
}

static void set_suspend_time(void)
{
	i2c_write(MCU_SUS_TIME_IDX, suspend_time);
}

static void set_suspend_state(void)
{
	unsigned int val;

	val = __raw_readl(pub_zqsr_mmr);
	/*enable i2c 0 clock*/
	__raw_writel(0x1 << i2c_clk_bit, sysc_clk1_reg);

	if (suspend_time != 0) {
		set_suspend_time();
		if (test_mode != 1)
			suspend_time = 0;
	}
	i2c_write(GIU_META, val);
	zqsr_val = i2c_read(GIU_META);
	zqsr_val1 = val;
	if (zqsr_val != zqsr_val1) {
		asm volatile("1: b 1b\n\t");
		asm volatile(".word 0x00aa0001\n\t");
	}
}


static int vatics_cpu_suspend(unsigned long arg)
{
	set_suspend_state();
	vatics_flush_cache_all();
	enter_lp_state();
	cpu_do_idle();
	asm volatile("1: b 1b\n\t");
	return -1;
}

static void vatics_pm_prepare(void)
{
	__raw_writel(virt_to_phys(cpu_resume),
		(void *)(info.misc + vatics_resume_addr));
}
/*
 * Suspend Ops
 */

static int vatics_suspend_enter(suspend_state_t state)
{
	int ret;
	static unsigned int suspend_times;

	flush_cache_all();

	ret = cpu_suspend(0, vatics_cpu_suspend);
	if (ret)
		return ret;

	if (console_suspend_enabled) {
		pr_notice("suspend times = 0x%x\n", suspend_times);
		pr_notice("ZQ0SR0 = 0x%x\n", zqsr_val);
	}
	++suspend_times;
	return 0;
}

static int vatics_suspend_prepare(void)
{
	vatics_pm_prepare();
	return 0;
}

static void vatics_suspend_finish(void)
{
}

static const struct platform_suspend_ops vatics_suspend_ops = {
	.enter		= vatics_suspend_enter,
	.prepare	= vatics_suspend_prepare,
	.finish		= vatics_suspend_finish,
	.valid		= suspend_valid_only_mem,
};

static const struct of_device_id vatics_pmu_of_device_ids[] __initconst = {
	{
		.compatible = "vatics,pmu",
	},
};
void __init vatics_pm_init(void)
{
	const struct of_device_id *match;
	struct device_node *np;
	void *addr;
	unsigned int sysc_addr;
	unsigned int i2c_addr;
	unsigned int pub_addr;
	unsigned int pctl_addr;

	np = of_find_matching_node_and_match(
		NULL, vatics_pmu_of_device_ids, &match);
	if (!np) {
		pr_err("failed to find pmu node\n");
		return;
	}

	if (of_property_read_u32(np, "sysc_reg", &sysc_addr)) {
		pr_err("failed to find sysc address\n");
		return;
	}
	if (of_property_read_u32(np, "pub_reg", &pub_addr)) {
		pr_err("failed to find pub address\n");
		return;
	}
	if (of_property_read_u32(np, "pctl_reg", &pctl_addr)) {
		pr_err("failed to find pctl address\n");
		return;
	}
	if (of_property_read_u32(np, "i2c_reg", &i2c_addr)) {
		pr_err("failed to find pub address\n");
		return;
	}
	if (of_property_read_u32(np, "i2c_clk_bit", &i2c_clk_bit)) {
		pr_err("failed to find i2c clk bit\n");
		return;
	}
	addr = ioremap(reserved_block_addr, reserved_block_size);
	pr_notice("remap 16M to 0x%x\n", (unsigned int)addr);
	info.loader_addr = (unsigned int *)addr;
	info.dram_training_addr =
		(unsigned int *)(addr + vatics_dram_training_ofs);
	info.misc = (unsigned int *)(addr + vatics_misc_ofs);

	/*
	 *printk("loader_addr = %x, traininig_addr = %x, misc = %x",
	 *	info.loader_addr, info.dram_training_addr, info.misc);
	 *printk("resume addr = %x", info.misc + vatics_resume_addr);
	 */

	lp_ctrl = ioremap(pctl_addr + LP_STAT_OFS, 0x4);
	i2c_base = (struct i2c_regs *)ioremap(i2c_addr, 0x100);
	pub_zqsr_mmr = ioremap(pub_addr + PUB_ZQ0SR0_OFS, 0x4);
	sysc_clk1_reg = ioremap(sysc_addr + SYSC_CLK1_OFS, 0x4);

	suspend_set_ops(&vatics_suspend_ops);
}

#ifdef POWER_KEYPAD_SUPPORT
static irqreturn_t gpio_isr(int irq, void *dev_id)
{
	input_report_key(gpio_key_dev, KEY_POWER, 1);
	input_report_key(gpio_key_dev, KEY_POWER, 0);
	input_sync(gpio_key_dev);
	return IRQ_HANDLED;
}
static int power_keys_suspend(struct device *dev)
{
	return 0;
}
static int power_keys_resume(struct device *dev)
{
	return 0;
}

static SIMPLE_DEV_PM_OPS(power_keys_pm_ops,
		power_keys_suspend, power_keys_resume);

static int power_keys_probe(struct platform_device *device)
{
	struct device *dev = &device->dev;
	int err = 0;
	int virq;

	virq = platform_get_irq(device, 0);
	gpio_key_dev = input_allocate_device();
	gpio_key_dev->name = "power_gpio";
	gpio_key_dev->phys = "gpio_pin";
	input_set_capability(gpio_key_dev, EV_KEY, KEY_POWER);
	err = input_register_device(gpio_key_dev);
	if (err) {
		dev_err(dev, "Unable to register input device, error: %d\n",
			err);
		goto fail1;
	}

	err = request_irq(virq, &gpio_isr,
			IRQF_TRIGGER_NONE, "power-key", NULL);

	return 0;
fail1:
	input_free_device(gpio_key_dev);
	return err;
}
#if defined(CONFIG_OF)
static const struct of_device_id power_button_of_match[] = {
	{ .compatible = "vatics,power-button", },
	{},
};
#endif
static struct platform_driver power_keys_device_driver = {
	.probe		= power_keys_probe,
	.driver		= {
		.name	= "power-key",
		.owner	= THIS_MODULE,
		.pm	= &power_keys_pm_ops,
#if defined(CONFIG_OF)
		.of_match_table = of_match_ptr(power_button_of_match),
#endif
	}
};
static int __init power_keys_init(void)
{
#if !defined(CONFIG_OF)
	power_keys_device = platform_device_alloc("power-key", -1);
	platform_device_add(power_keys_device);
#endif
	return platform_driver_register(&power_keys_device_driver);
}
late_initcall(power_keys_init);
#endif
