2016-12-16
1.定义声明描述硬件相关的数据结构
2.初始化硬件信息

static int led_init(void)	
static void led_exit(void)
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

int gpio_request(int gpio,char *name)
void gpio_free(int gpio)

int gpio_direction_output(int gpio,int value)

int gpio_direction_input(int gpio)

int gpio_set_value(int gpio,int value)

int gpio_get_value(int gpio)


设备文件存放目录：
/dev

设备号：主设备号  次设备号
数据类型：dev_t    unsigned int 
高12bit：主
低20bit：次

设备号=MKDEV(主设备号，次设备号)
主设备号=MAJOR（设备号）
次设备号=MINOR（次备号）

申请设备号：
int  alloc_chrdev_region(dev_t *dev,unsigned baseminor,unsigned count,const char *name)
释放设备号
void unregister_chrdev_region(dev_t dev,int count)

1.定义初始化硬件操作方法对象
2.定义初始化字符设备对象
static struct cdev led_cdev;
cdev_init(&led_cdev,&led_fops)
3.向内核注册字符设备
cdev_add(&led_cdev,设备号，硬件个数);
4.卸载字符设备
cdev_del(&led_cdev)

2016-12-17
int copy_to_user(void __user *to,const void *from,unsigned long n)
unsigned long copy_from_user(void *to,const void __user *from,unsigned long n)

int  ioctl(int fd,int request,...)
long  (.unlocked_ioctl)(struct file *file,unsigned int cmd,unsigned long arg)
记住：copy_from_user(&index,(int*)arg,sizeof(arg))

struct class *cls;//定义设备类指针
//定义设备类
cls=class_create(THIS_MODULE,"tarena");
//创建设备文件
device_create(cls,NULL,dev,NULL,''myled'');

//删除设备文件
device_destroy(cls,dev);
//删除设备类
class_destroy(cls);

static struct miscdevice led_misc={
	.minor=MISC_DYNAMIC_MINOR,
	.name="myled",
	.fops=&led_fops
	
};

misc_register(&led_misc);
misc_deregster(&led_misc);

12-25
通过直接控制GPIO管脚实现字符设备驱动的一种基本框架流程
1.主框架：
static int led_init(void)	
static void led_exit(void)
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

2.声明变量
static dev_t  dev;//声明一个静态的设备号
static struct cdev beep_cdev;//声明一个字符设备

static struct class *cls;//声明设备类的指针，用于自动创建字符设备文件

//gpio口的相关操作地址的声明
static void *gpio_base; //gpio寄存器的虚拟起始地址
static unsigned long *gpiocon; //配置寄存器的虚拟地址，
static unsigned long *gpiodata;//数据寄存器的虚拟地址

3.在入口函数中调用初始化、注册等函数进行操作
//1.申请设备号
    alloc_chrdev_region(&dev, 0, 1, "beeps");
 //2.初始化注册字符设备cdev
    cdev_init(&beep_cdev, &beep_fops);
    cdev_add(&beep_cdev, dev, 1);
 //3.定义初始化一个设备类并自动创建设备节点
    cls = class_create(THIS_MODULE, "beeps");
    device_create(cls, NULL, dev, NULL, "mybeep");  
//4.地址映射，映射到内核地址
    gpio_base = ioremap(0xe02000A0, 8); 
    //4.1获取配置寄存器和数据寄存器对应的虚拟地址
    gpiocon = (unsigned long *)gpio_base;
    gpiodata = (unsigned long *)(gpio_base + 0x04);
//5.配置GPIO实现对相应的GPIO初始化工作
    *gpiocon &= ~(0xf << 4);
    *gpiocon |= (1 << 4);
    *gpiodata &= ~(1 << 1);
    
4.在出口函数调用卸载、注销等函数进行操作，顺序正好相反
//1.配置GPIO输出为0
    *gpiodata &= ~(1 << 1);
 //2.解除地址映射
    iounmap(gpio_base);
//3.删除设备节点
    device_destroy(cls, dev);
    class_destroy(cls);
//4.删除cdev
    cdev_del(&beep_cdev);
//5.释放设备号
    unregister_chrdev_region(dev, 1);
    
6.对硬件操作方法初始化
static struct file_operations beep_fops = {
    .owner = THIS_MODULE,
    .ioctl = beep_ioctl
};

7.对操作方法所使用的函数实现
static int beep_ioctl(struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
{   
	//实现  
}
其中注意需要时必须使用两个方向的拷贝函数：
int copy_to_user(void __user *to,const void *from,unsigned long n);
unsigned long copy_from_user(void *to,const void __user *from,unsigned long n);


12-26
DS18B20
ds18b20_drv.c
1.头文件、相关宏命令的定义
#define SKIP_ROM        0xcc
#define COVERTT         0x44
#define READ_MEM        0xbe
2.函数入口
//注册GPIO资源
gpio_request(S5PV210_GPH1(0),"GPH1_0");
//注册混杂设备
misc_register(&ds18b20_miscdev);
3.函数出口
//卸载混杂设备
misc_deregister(&ds18b20_miscdev);
//卸载GPIO
gpio_free(S5PV210_GPH1(0));


4.混杂设备的定义初始化
static struct miscdevice ds18b20_miscdev ={
        .minor=MISC_DYNAMIC_MINOR,
        .name = "ds18b20",
        .fops = &ds18b20_fops
};

5.硬件操作方法的初始化
static struct file_operations ds18b20_fops={
        .owner = THIS_MODULE,
        .read =ds18b20_read
    };
    
6.对操作方法的相关函数实现
    static ssize_t ds18b20_read(struct file *file, char *buf, size_t count, loff_t *pos)
    {}

7.根据数据手册，剥离出相关函数，并实现
    //初始化=复位脉冲+从机应答脉冲
static void ds18b20_reset(void)
{}
static unsigned char ds18b20_read8(void)
{}
static void ds18b20_write8(unsigned char data)
{}