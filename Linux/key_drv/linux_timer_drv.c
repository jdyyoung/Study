#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <asm/gpio.h>
// #include <plat/gpio-cfg.h>

//声明LED的硬件信息的数据结构
//定义初始化2个LED的硬件信息

//定义初始化定时器对象
static struct timer_list mytimer;
static int g_data = 0x55;
static int time = 2000;
module_param(time, int, 0664);

//超时处理函数
static void mytimer_function(unsigned long data)
{
    //获取参数信息
    int *pdata = (int *)data;
    
    //判断灯的开关状态，如果是开，那就关，如果是关，那就开
    //打印
    printk("%s : g_data = %#x\n", __func__, *pdata);
    
    //重新添加定时器
    /*
        mytimer.expires = jiffies + msecs_to_jiffies(2000);
        add_timer(&mytimer);
        危险：这两条语句没有进行互斥保护,容易发生CPU资源的切换
    */
    mod_timer(&mytimer, 
            jiffies + msecs_to_jiffies(time));
}

static int led_init(void)
{
    //申请GPIO资源，配置GPIO为输出口，输出0
    //初始化定时器
    init_timer(&mytimer);
    
    //指定定时器的超时时候的时间
    mytimer.expires = 
                jiffies + msecs_to_jiffies(time);
    //指定定时器的超时处理函数
    mytimer.function = mytimer_function;
    //给超时处理函数传递参数
    mytimer.data = (unsigned long)&g_data;
    
    //注册，启动定时器
    add_timer(&mytimer);
    printk("启动定时器!\n");
    return 0;
}

static void led_exit(void)
{
    //删除定时器
    del_timer(&mytimer);
    //输出0，释放GPIO资源
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
