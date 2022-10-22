#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h> //struct file_operations
#include <linux/miscdevice.h> //struct miscdevice
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/uaccess.h> //copy*

//声明描述LED的硬件相关的数据结构
struct led_resource {
    int gpio; //GPIO编号
    char *name; //GPIO名称
};

//定义初始化LED的硬件信息
static struct led_resource led_info[] = {
    [0] = {
        .gpio = GPIOH(2),
        .name = "GPIOH_2"
    },
};

#define LED_ON  0x100001 //开灯
#define LED_OFF 0x100002 //关灯

//应用ioctl->软中断->sys_ioctl->led_ioctl
static long led_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    //1.先处理arg,定义内核缓冲区，将用户缓冲区的数据拷贝到内核中
    int index;
    copy_from_user(&index, (int *)arg, sizeof(index));
   
    //2.再处理cmd,解析cmd命令
    switch(cmd) {
        case LED_ON:
            gpio_set_value(led_info[index-1].gpio, 1);
            printk("%s:第%d个灯开!\n", __func__, index);
            break;
        case LED_OFF:
            gpio_set_value(led_info[index-1].gpio, 0);
            printk("%s:第%d个灯关!\n", __func__, index);
            break;
        default:
            return -1;
    }
    return 0; //执行成功返回0，失败返回负值
}

//定义初始化LED的硬件操作对象
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = led_ioctl
};

//定义初始化混杂设备对象
static struct miscdevice led_misc = {
    .minor = MISC_DYNAMIC_MINOR, //内核帮你分配一个次设备号
    .name = "myled", //自动创建设备文件名/dev/myled
    .fops = &led_fops
};

static int led_init(void)
{
    int i;
   
    //注册混杂设备对象到内核
    misc_register(&led_misc);

    //申请GPIO资源，配置为输出，输出0(省电)
    for (i = 0; i < ARRAY_SIZE(led_info); i++) {
        gpio_request(led_info[i].gpio,
                      led_info[i].name);
        gpio_direction_output(led_info[i].gpio, 0);
    }
   
    return 0;
}

static void led_exit(void)
{
    int i;
    //卸载混杂设备
    misc_deregister(&led_misc);

    //输出0，释放GPIO资源
    for (i = 0; i < ARRAY_SIZE(led_info); i++) {
        gpio_set_value(led_info[i].gpio, 0);
        gpio_free(led_info[i].gpio);
    }
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
