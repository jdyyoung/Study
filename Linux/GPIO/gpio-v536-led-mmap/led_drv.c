#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h> //mmap
#include <linux/fs.h>
#include <linux/miscdevice.h>

//mmap:只做将物理地址映射到用户虚拟地址上
static int led_mmap(struct file *file,
                        struct vm_area_struct *vma)
{
    //关闭cache功能
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    
    //将物理地址映射到用户虚拟地址上
    remap_pfn_range(vma, vma->vm_start,
                    0x0300B000  >> 12,
                    vma->vm_end - vma->vm_start,
                    vma->vm_page_prot);
    return 0;
}

//定义初始化硬件操作方法
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .mmap = led_mmap
};

//定义初始化混杂设备对象
static struct miscdevice led_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "myled",
    .fops = &led_fops
};

static int led_init(void)
{
    misc_register(&led_misc);
    return 0;
}

static void led_exit(void)
{
    misc_deregister(&led_misc);
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
