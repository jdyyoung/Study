#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

static int hello_value;

static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
   return sprintf(buf, "%d\n", hello_value);
}

static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
   sscanf(buf, "%du", &hello_value);
   return count;
}

//   cat /sys/kernel/helloworld/attr_value 
// static struct kobj_attribute hello_value_attribute = __ATTR(attr_value, 0666, hello_show, hello_store);
static struct kobj_attribute hello_value_attribute = __ATTR(attr_value, 0444, hello_show, hello_store);

static struct kobject *helloworld_kobj;

static int __init helloworld_init(void){
	int retval;
	helloworld_kobj = kobject_create_and_add("helloworld", kernel_kobj);
	if (!helloworld_kobj)
		return -ENOMEM;

	retval = sysfs_create_file(helloworld_kobj, &hello_value_attribute);

	if (retval)
		kobject_put(helloworld_kobj);

	return retval;
}

static void __exit helloworld_exit(void){
	kobject_put(helloworld_kobj);
}

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_LICENSE("GPL");

/*
sysfs_create_file示例一_xmzzy2012的专栏-CSDN博客 - https://blog.csdn.net/xmzzy2012/article/details/48245803
一、运行结果：
root@thiz:/sys/kernel/hellowrold# ls
hello_value
root@thiz:/sys/kernel/helloworld# echo 1 > hello_value
root@thiz:/sys/kernel/hellowrold# cat hello_value
1

二、分析
1、目录项(/sys/kernel/helloworld)：通过函数kobject_create_and_add("helloworld", kernel_kobj)可以在/sys/kernel下建立一个helloworld目录项。
2、属性文件(hello_value):通过函数sysfs_create_file(helloworld_kobj, &hello_value_attribute)建立。这个也同时建立了文件与操作之间的联系和对应。
3、操作(hello_show、hello_store):在sys系统中对文件的操作有2个函数，一个是show，一个是store，这两个函数和普通文件的read和write函数有点类似，是他们的精简版。
对于sprintf和sscanf是对copy_from_user和copy_to_user函数的封装。
4、如果一组属性则可以如下：
static struct attribute *attrs [] = {
   &foo_attribute.attr,
   &baz_attribuet.attr,
   &bar_attribute.attr,
   NULL,      //need to NULL terminate the list of attributes 
};

static struct attribute_group attr_group = {
   .attrs = attrs,
};
retval = sysfs_create_group(example_kobj, &attr_group);
*/