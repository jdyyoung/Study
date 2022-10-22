#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/device.h>

#define COUNT 1

#define LED_IO GPIOH(2)         // 要点亮的 led 

#define dev_name "V536_LED"

dev_t device_id;                 // 存放设备 ID
struct cdev led_cdev;            // led cdev 结构
static struct class *led_class;  // led 类操作

// 定义 Open 函数
static int led_open(struct inode *inode_, struct file *file_)
{
    int ret;
    ret = gpio_request(LED_IO, "led_0");
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        gpio_direction_output(LED_IO, 0);
        return 0;
    }
}

// 定义写操作
static ssize_t led_write(struct file *file_, const char *__user buf_, size_t len_, loff_t *loff_)
{
    char stat; int ret;

    ret = copy_from_user(&stat, buf_, sizeof(char));  // 读取用户写入的信息
    if(ret < 0)
        return ret;

    if (!stat)                                        // 因为 LED 是下拉点亮，所以反选，如果是上拉点亮那就删除 !
    {
        gpio_set_value(LED_IO, 0);                    // 点亮一颗 LED
    } 
    else
    {
        gpio_set_value(LED_IO, 1);                    // 熄灭 LED
    }
    return 0;
}

// 关闭 GPIO 操作
static int led_close(struct inode *inode_, struct file *file_)
{
    gpio_set_value(LED_IO, 0);                       // 恢复 IO 状态
    gpio_free(LED_IO);                               // 清除绑定
    return 0;
}

// 封包为文件系统操作接口
static struct file_operations led_file_operations = {
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
    .release = led_close,
};

// 初始化设备使用的操作
static int __init led_init(void)
{
    int ret;

    device_id = MKDEV(3242, 3432);   // 随便定义的 设备id
    ret = register_chrdev_region(device_id, COUNT, dev_name);   // 注册设备
    if (ret < 0)
    {
        return ret;
    }

    cdev_init(&led_cdev, &led_file_operations);
    led_cdev.owner = THIS_MODULE;
    ret = cdev_add(&led_cdev, device_id, COUNT);              // 加入到 cdev

    if (ret < 0)
    {
        unregister_chrdev_region(device_id, COUNT);
    }

    led_class = class_create(THIS_MODULE, "led_class");         // 创建类
    if (led_class == NULL)
    {
        cdev_del(&led_cdev);
    }

    device_create(led_class, NULL, device_id, NULL, dev_name);   // 创建设备

    return 0;
}

// 删除设备的操作
static void __exit led_exit(void)
{
    // 刚才怎么创建的就怎么删除
    device_destroy(led_class, device_id);
    class_destroy(led_class);
    unregister_chrdev_region(device_id, COUNT);
    cdev_del(&led_cdev);
    gpio_free(LED_IO);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("WTFPL");
MODULE_AUTHOR("AWOL");
MODULE_DESCRIPTION("light up v536 vision led");
MODULE_VERSION("1.0");
