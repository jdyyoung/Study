/*
Linux驱动学习 —— 在/sys下面创建目录示例 - 摩斯电码 - 博客园 
- https://www.cnblogs.com/pengdonglin137/p/6217419.html
[root@tiny4412 mnt]# ls -R /sys/sysfs_demo/
/sys/sysfs_demo/:
node_one      node_two      sysfs_demo_2

/sys/sysfs_demo/sysfs_demo_2:
node_four   node_three

用到的两个函数分别是： kobject_create_and_add 和 sysfs_create_group。
前一个函数用于在/sys下面创建目录， 后一个函数用于创建文件。
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/device.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

static struct kobject *k_obj = NULL;
static struct kobject *k_obj_2 = NULL;

static char node_one_buf[20] = {0};

static ssize_t sysfs_demo_show_node_one(struct kobject *kobj, struct kobj_attribute *attr, char * buf)
{
    pr_info("enter, node: %s\n", attr->attr.name);
    return sprintf(buf, "%s: %s\n", attr->attr.name, node_one_buf);
}

static ssize_t sysfs_demo_store_node_one(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t n)
{
    pr_info("enter, node: %s\n", attr->attr.name);

    sprintf(node_one_buf, "%s", buf);

    return n;
}

static struct kobj_attribute node_one_attribute =
    __ATTR(node_one, S_IWUSR|S_IRUGO, sysfs_demo_show_node_one, sysfs_demo_store_node_one);

static ssize_t sysfs_demo_show_node_two(struct kobject *kobj, struct kobj_attribute *attr, char * buf)
{

    return sprintf(buf, "%s\n", attr->attr.name);
}

static struct kobj_attribute node_two_attribute =
    __ATTR(node_two, S_IWUSR|S_IRUGO, sysfs_demo_show_node_two, NULL);

static struct attribute *sysfs_demo_attributes[] = {
    &node_one_attribute.attr,
    &node_two_attribute.attr,
    NULL
};

static const struct attribute_group sysfs_demo_attr_group = {
    .attrs = sysfs_demo_attributes,
};

static char node_three_buf[20] = {0};

static ssize_t sysfs_demo_show_node_three(struct kobject *kobj, struct kobj_attribute *attr, char * buf)
{
    pr_info("enter, node: %s\n", attr->attr.name);
    return sprintf(buf, "%s: %s\n", attr->attr.name, node_three_buf);
}

static ssize_t sysfs_demo_store_node_three(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t n)
{
    pr_info("enter, node: %s\n", attr->attr.name);

    sprintf(node_three_buf, "%s", buf);

    return n;
}

static struct kobj_attribute node_three_attribute =
    __ATTR(node_three, S_IWUSR|S_IRUGO, sysfs_demo_show_node_three, sysfs_demo_store_node_three);

static ssize_t sysfs_demo_show_node_four(struct kobject *kobj, struct kobj_attribute *attr, char * buf)
{

    return sprintf(buf, "%s\n", attr->attr.name);
}

static struct kobj_attribute node_four_attribute =
    __ATTR(node_four, S_IWUSR|S_IRUGO, sysfs_demo_show_node_four, NULL);

static struct attribute *sysfs_demo2_attributes[] = {
    &node_three_attribute.attr,
    &node_four_attribute.attr,
    NULL
};

static const struct attribute_group sysfs_demo2_attr_group = {
    .attrs = sysfs_demo2_attributes,
};

static int __init sysfs_demo_init(void)
{
    if ((k_obj = kobject_create_and_add("sysfs_demo", NULL)) == NULL ) {
        pr_err("sysfs_demo sys node create error \n");
        goto out;
    }

    if(sysfs_create_group(k_obj, &sysfs_demo_attr_group) ) {
        pr_err("sysfs_create_group failed\n");
        goto out2;
    }

    if ((k_obj_2 = kobject_create_and_add("sysfs_demo_2", k_obj)) == NULL ) {
        pr_err("hwinfo sys node create error \n");
        goto out3;
    }

    if(sysfs_create_group(k_obj_2, &sysfs_demo2_attr_group) ) {
        pr_err("sysfs_create_group failed\n");
        goto out4;
    }

    pr_info("enter.\n");
    return 0;
out4:
    kobject_put(k_obj_2);
out3:
    sysfs_remove_group(k_obj, &sysfs_demo_attr_group);
out2:
    kobject_put(k_obj);
out:
    return -1;
}
module_init(sysfs_demo_init);

static void __exit sysfs_demo_exit(void)
{
    pr_info("enter.\n");

    if (k_obj) {
        sysfs_remove_group(k_obj, &sysfs_demo_attr_group);
        if (k_obj_2) {
            sysfs_remove_group(k_obj_2, &sysfs_demo2_attr_group);
            kobject_put(k_obj_2);
        }
        kobject_put(k_obj);
    }

}
module_exit(sysfs_demo_exit);

MODULE_LICENSE("GPL");