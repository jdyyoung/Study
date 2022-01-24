/*
测试时，发现没在/sys/devices/platform/ 生成相应的节点mytest
probe()没被执行？？？
“probe函数是match成功后调用的。”
Liunx驱动中，probe函数何时被调用_clam_zxf的博客-CSDN博客 - https://blog.csdn.net/clam_zxf/article/details/74291352
10.10、驱动开发 -- platform总线_feiffer·hu的博客-CSDN博客 - https://blog.csdn.net/qq_41110647/article/details/106733456?spm=1001.2014.3001.5501
feiffer·hu的博客_Feiffer_Hu_CSDN博客-Linux 嵌入式,C语言,驱动开发领域博主 - https://blog.csdn.net/qq_41110647
*/
/*
linxu 驱动 DEVICE_ATTR和device_create_file 学习实例 -
灰信网（软件开发博客聚合） - https://www.freesion.com/article/8774867281/
*/
#include <linux/init.h>
#include <linux/module.h>
 
/*驱动注册的头文件，包含驱动的结构体和注册和卸载的函数*/
#include <linux/platform_device.h>
 
#define DRIVER_NAME "mytest"
 
MODULE_LICENSE("Dual BSD/GPL");
 
static ssize_t mytest_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    printk("mytest_show:linux kernel test show!\n");
}
 
static ssize_t mytest_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    printk("mytest_store\n");
}
static DEVICE_ATTR(vib, S_IWUSR | S_IRUGO, mytest_show, mytest_store);

static int mytest_probe(struct platform_device *pdv){
	int err;
	printk(KERN_EMERG "\tinitialized\n");
	
	err = device_create_file(&pdv->dev, &dev_attr_vib);
	if (err)
		 printk(KERN_EMERG "\tdevice_create_file_error\n");
	else
		 printk(KERN_EMERG "\t very good device_create_file_success\n");
	return 0;
}
 
static int mytest_remove(struct platform_device *pdv){
	
	return 0;
}
 
static void mytest_shutdown(struct platform_device *pdv){	
	;
}
 
static int mytest_suspend(struct platform_device *pdv){
	return 0;
}
 
static int mytest_resume(struct platform_device *pdv){
	return 0;
}

static void mytest_release (struct device *dev) {}

//定义初始化硬件节点
static struct platform_device mytest_dev = {
    .name = DRIVER_NAME, //用于匹配
    .id = -1,
    .dev = {
        .release = mytest_release //避免内核警告
    }
};

struct platform_driver mytest_driver = {
	.probe = mytest_probe,
	.remove = mytest_remove,
	.shutdown = mytest_shutdown,
	.suspend = mytest_suspend,
	.resume = mytest_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	}
};
 
 
static int mytest_init(void)
{
	int DriverState;
	
	printk(KERN_EMERG "device_create_file test enter!\n");
	DriverState = platform_driver_register(&mytest_driver);
	
	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	DriverState = platform_device_register(&mytest_dev);
	printk(KERN_EMERG "\t---DriverState is %d\n",DriverState);
	return 0;
}
 
static void mytest_exit(void)
{
	printk(KERN_EMERG "---device_create_file test exit!\n");
	platform_device_unregister(&mytest_dev);
	printk(KERN_EMERG "device_create_file test exit!\n");
	platform_driver_unregister(&mytest_driver);	
}
 
module_init(mytest_init);
module_exit(mytest_exit);