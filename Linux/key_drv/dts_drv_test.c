#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>
/*
test_k: test_keys@0 {
			status = "okay";
			compatible = "vatics,test-button";
			interrupt-parent = <&gpio1>;
			interrupts = <25 IRQ_TYPE_LEVEL_LOW>;
		};
*/


static int dts_test_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	if (np == NULL) {
		printk(" DTS TEST failed to get of node\n");
		return -1;
	}

	of_property_read_u32(np, "test_device_id", &pdev->id);
	if (pdev->id < 0) {
		printk("DTS _TEST failed to get device id\n");
		return -EINVAL;
	}

	printk("DTS TEST:test_device_id = %d\n",pdev->id);
	return 0;
}

static int dts_test_remove(struct platform_device *pdev){
	printk("%s goto here---------------------------\n",__func__);
	return 0;
}

static const struct of_device_id irq_test_of_match[] = {
    { .compatible = "allwinner,sunxi-csi", },
    { },
};

MODULE_DEVICE_TABLE(of, irq_test_of_match);

static struct platform_driver irq_test_device_driver = {
    .probe      = dts_test_probe,
	.remove     = dts_test_remove,
    .driver     = {
        .name   = "test-button",
        .of_match_table = irq_test_of_match,
    }
};

static int __init irq_test_init(void)
{
	return platform_driver_register(&irq_test_device_driver);
}

static void __exit irq_test_exit(void)
{
    platform_driver_unregister(&irq_test_device_driver);
}

module_init(irq_test_init);
module_exit(irq_test_exit);
MODULE_LICENSE("GPL");
