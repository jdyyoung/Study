#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

static int open_cnt = 1; //共享资源
static spinlock_t lock; //定义自旋锁

static int led_open(struct inode *inode,
                        struct file *file)
{
    unsigned long flags;
    
    //关中断,获取锁
    spin_lock_irqsave(&lock, flags);
    
    //临界区
    if (--open_cnt != 0) {
        printk("设备已被打开!\n");
        open_cnt++;
        //释放锁,开中断
        spin_unlock_irqrestore(&lock,flags);
        return -EBUSY;//返回设备忙错误
    }
    
    //释放锁,开中断
    spin_unlock_irqrestore(&lock,flags);
    
    printk("设备打开成功!\n");
    return 0;
}

static int led_close(struct inode *inode,
                        struct file *file)
{
    unsigned long flags;
    spin_lock_irqsave(&lock, flags);
    open_cnt++; //临界区
    spin_unlock_irqrestore(&lock, flags);
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
    spin_lock_init(&lock); //初始化
    return 0;
}

static void led_exit(void)
{
    misc_deregister(&led_misc); //卸载
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
