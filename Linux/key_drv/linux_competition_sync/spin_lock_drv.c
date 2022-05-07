#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

//定义信号量
static struct semaphore sema;

static int led_open(struct inode *inode,
                        struct file *file)
{
    printk("获取信号量...!\n");
    down(&sema); 
    printk("设备打开成功!\n");
    return 0;
}

static int led_close(struct inode *inode,
                        struct file *file)
{
    printk("主动唤醒休眠的进程，并且释放信号量!\n");
    up(&sema);
    return 0;
}
//定义初始化硬件操作方法
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_close
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
    sema_init(&sema, 1); //初始化为互斥信号量
    return 0;
}

static void led_exit(void)
{
    misc_deregister(&led_misc); //卸载
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
