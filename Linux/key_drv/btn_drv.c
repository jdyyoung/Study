#include <linux/init.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
//include/linux/input.h
#include <linux/input.h> //标准按键值
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
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
        .gpio = S5PV210_GPH0(0),
        .irq = IRQ_EINT(0),
        .name = "KEY_UP",
        .code = KEY_UP
    },
    [1] = {
        .gpio = S5PV210_GPH0(1),
        .irq = IRQ_EINT(1),
        .name = "KEY_DOWN",
        .code = KEY_DOWN
    }
};

//中断处理函数
static irqreturn_t button_isr_handler(int irq, 
                                    void *dev)
{
    int state = 0;
    //1.获取按键对应的硬件信息
    struct btn_resource *pdata = dev;
    //2.获取按键的状态
    state = gpio_get_value(pdata->gpio);
    
    //3.打印按键的信息
    printk("按键%s的状态为%s,按键值为%d\n",
            pdata->name, 
            state?"松开":"按下",
            pdata->code);
    return 0;
}

static int btn_init(void)
{
    int i;
    //1.申请中断资源和注册中断处理函数
    for (i = 0; i < ARRAY_SIZE(btn_info); i++) {
        gpio_request(btn_info[i].gpio,
                        btn_info[i].name);
        request_irq(btn_info[i].irq,
                button_isr_handler,
        IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
                btn_info[i].name,
                &btn_info[i]);
    }
    return 0;
}

static void btn_exit(void)
{
    int i;
    //1.释放资源
    for (i = 0; i < ARRAY_SIZE(btn_info); i++) {
        gpio_free(btn_info[i].gpio);
        free_irq(btn_info[i].irq, &btn_info[i]);
    }
}

module_init(btn_init);
module_exit(btn_exit);
MODULE_LICENSE("GPL");
