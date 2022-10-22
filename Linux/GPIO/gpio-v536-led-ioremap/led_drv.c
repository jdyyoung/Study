#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/io.h> //ioremap
#include <linux/uaccess.h>

//定义保存配置和数据寄存器的内核虚拟地址
static unsigned long *gpiocon, *gpiodata;

#define LED_ON  0x100001
#define LED_OFF 0x100002
/*
 *  int index = 1;
 *  ioctl(fd, LED_ON, &index);
 * */
static long led_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    //1.拷贝用户数据到内核空间
    int index;
    copy_from_user(&index, (int *)arg, sizeof(index));

    //2.解析命令
    switch(cmd) {
        case LED_ON:
            if (index == 1)
				*gpiodata &= ~(1 << 2);
            break;
        case LED_OFF:
            if (index == 1)
                *gpiodata |= (1 << 2);
            break;
    }
    //调试信息
    printk("配置寄存器=%#x, 数据寄存器=%#x\n", *gpiocon, *gpiodata);
    return 0;
}

//定义初始化硬件操作方法
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = led_ioctl
};

//定义初始化混杂设备对象
static struct miscdevice led_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "myled",
    .fops = &led_fops
};

static int led_init(void)
{
    misc_register(&led_misc); //注册
    //将物理地址映射到内核的虚拟地址上
    gpiocon = ioremap(0x0300B0FC, 4);
    gpiodata = ioremap(0x0300B10C, 4);

    //配置GPIO为输出口
    *gpiocon &= ~(0xf << 8);
    *gpiocon |= (1 << 8);
	
	//输出1，加载驱动默认灯灭的状态
    *gpiodata |=(1 << 2);

    return 0;
}

static void led_exit(void)
{
    misc_deregister(&led_misc); //卸载
    //输出1
    //解除地址映射
    *gpiodata |=(1 << 2);
    iounmap(gpiocon);
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
