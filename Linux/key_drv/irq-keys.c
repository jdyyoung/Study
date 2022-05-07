/*
基于设备树编写按键中断驱动程序 - 小小的番茄 - 
博客园 - https://www.cnblogs.com/lztutumo/p/13340411.html
Linux DTS中和中断相关属性的解释和用法_月出皎兮。 佼人僚兮。 舒窈纠兮。 劳心悄兮。-CSDN博客 
- https://blog.csdn.net/rockrockwu/article/details/96461563
*/
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

/* 设备树节点 */
#if 0
irqtest_keys {
    compatible = "irq-keys";
    key-home {
        label = "GPIO key-home";
        gpios = <&gpx1 1 GPIO_ACTIVE_LOW>;
              pinctrl-0 = <&my_irq_key>;
              pinctrl-names = "default";
    };
};

my_irq_key: my-irq-key {
    samsung,pins = "gpx1-1";
    samsung,pin-pud = <EXYNOS_PIN_PULL_NONE>;
//    samsung,pin-drv = <EXYNOS4_PIN_DRV_LV4>;
};
#endif

struct my_gpio_keys_button {
    unsigned int code;
    int gpio;
    int active_low;
    const char *desc;
    unsigned int type;
    int wakeup;
    int debounce_interval;
    bool can_disable;
    int value;
    unsigned int irq;
    struct gpio_desc *gpiod;
};

static char *label[2];
static struct device *dev;
static struct my_gpio_keys_button *button;

static irqreturn_t irq_test_irq_isr(int irq, void *dev_id)
{
    printk(KERN_INFO "get irq --> irq_test_irq_isr.\n");

    return IRQ_HANDLED;
}

static int irq_test_probe(struct platform_device *pdev)
{
    /* 获取节点信息，注册中断 */
    dev = &pdev->dev;
    struct fwnode_handle *child = NULL;
    int nbuttons;
    int irq, error;
    irq_handler_t isr;
    unsigned long irqflags;

    nbuttons = device_get_child_node_count(dev);
    if (nbuttons == 0) {
        printk(KERN_INFO "no child exist, return\n");
        return ERR_PTR(-ENODEV);
    }

    printk(KERN_INFO "child num is %d.\n", nbuttons);
    button = devm_kzalloc(dev, sizeof(struct my_gpio_keys_button) * nbuttons, GFP_KERNEL);

    /* 获取lable参数，父节点没有lable属性 */
    device_property_read_string(dev, "label", label[0]);
    printk(KERN_INFO "parent lable %s\n", label[0]);

    /* 扫描处理每个子节点 */
    device_for_each_child_node(dev, child) {
        /* 获取虚拟中断号virq */
        if (is_of_node(child)) {
            button->irq = irq_of_parse_and_map(to_of_node(child), 0);
        }

        fwnode_property_read_string(child, "label", &button->desc);
        /* 获取gpio描述符gpiod */
        button->gpiod = devm_fwnode_get_gpiod_from_child(dev, NULL,
                                child,
                                GPIOD_IN,
                                button->desc);
        if (IS_ERR(button->gpiod)) {
            printk(KERN_INFO "get gpiod failed, return.\n");
            return -ENOENT;
        }

        /* 检查虚拟中断号，可不使用 */
        if (!button->irq) {
            irq = gpiod_to_irq(button->gpiod);
            if (irq < 0) {
                error = irq;
                dev_err(dev,
                    "Unable to get irq number for GPIO %d, error %d\n",
                    button->gpio, error);
                return error;
            }
            button->irq = irq;
        }

        printk(KERN_INFO "get virq %d for key.\n", button->irq);
        isr = irq_test_irq_isr;
        irqflags = 0;
        irqflags |= IRQF_SHARED;
//devm_request_any_context_irq(class_dev, data->irq,int26_irq, IRQF_TRIGGER_FALLING, data->name, data);

        /* 设置引脚为输入模式 */
        gpiod_set_value(button->gpiod, 1);
        gpiod_direction_input(button->gpiod);

        /* 注册中断 */
        /* 最后一个参数是传给中断函数的参数 */
        error = devm_request_any_context_irq(dev, button->irq, isr, IRQF_TRIGGER_FALLING, button->desc, NULL);
        if (error < 0) {
            dev_err(dev, "Unable to claim irq %d; error %d\n", button->irq, error);
            return error;
        }
    }

    return 0;
}

static const struct of_device_id irq_test_of_match[] = {
    { .compatible = "irq-keys", },
    { },
};

MODULE_DEVICE_TABLE(of, irq_test_of_match);

static struct platform_driver irq_test_device_driver = {
    .probe      = irq_test_probe,
    .driver     = {
        .name   = "irqtest_keys",
        .of_match_table = irq_test_of_match,
    }
};

static int __init irq_test_init(void)
{
    return platform_driver_register(&irq_test_device_driver);
}

static void __exit irq_test_exit(void)
{
    devm_free_irq(dev, button->irq, NULL);
    platform_driver_unregister(&irq_test_device_driver);
}

module_init(irq_test_init);
module_exit(irq_test_exit);

MODULE_LICENSE("GPL");