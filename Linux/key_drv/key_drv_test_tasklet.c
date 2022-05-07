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

//声明描述按键的硬件数据结构
struct btn_resource {
    int gpio; //对应的GPIO编号
    int irq; //对应的中断号
    char *name; //按键名称
    int code; //按键值
};

//定义初始化按键的信息
static struct btn_resource btn_info[] = {
    [0] = {
        .name = "KEY_UP",
        .code = KEY_UP
    },
};

//延后处理函数
//data == 0xC0001000
static void btn_tsk_function(unsigned long data)
{
    int *pdata = (int *)data;
    printk("%s:g_data = %#x\n",__func__,*pdata); 
}

static int g_data = 0x55;

//定义初始化tasklet对象
//&g_data == 0xC0001000
static DECLARE_TASKLET(btn_tsk, btn_tsk_function,(unsigned long)&g_data);

//中断处理函数
static irqreturn_t button_isr_handler(int irq, 
                                    void *dev)
{
    int state = 0;
    //1.获取按键对应的硬件信息
    struct btn_resource *pdata = dev;
    //2.获取按键的状态
    state = gpio_get_value(pdata->gpio);

    //登记底半部tasklet延后处理函数
    tasklet_schedule(&btn_tsk);
    
    //3.打印按键的信息
    printk("按键%s的状态为%s,按键值为%d\n",
            pdata->name, 
            state?"按下":"松开",
            pdata->code);
    return 0;
}

static int irq_test_probe(struct platform_device *device)
{
    btn_info[0].gpio = 57;//32 * 1 + 25;
	btn_info[0].irq = platform_get_irq(device, 0);
	gpio_request(btn_info[0].gpio,btn_info[0].name);
        request_irq(btn_info[0].irq,
                button_isr_handler,
        IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
                btn_info[0].name,
                &btn_info[0]);

	printk(KERN_INFO "get virq %d for key.\n", btn_info[0].irq);
	return 0;
}

static const struct of_device_id irq_test_of_match[] = {
    { .compatible = "vatics,test-button", },
    { },
};

MODULE_DEVICE_TABLE(of, irq_test_of_match);

static struct platform_driver irq_test_device_driver = {
    .probe      = irq_test_probe,
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
    gpio_free(btn_info[0].gpio);
    free_irq(btn_info[0].irq, &btn_info[0]);
    platform_driver_unregister(&irq_test_device_driver);
}

module_init(irq_test_init);
module_exit(irq_test_exit);

MODULE_LICENSE("GPL");
