//#include "private.h"
/*
/{
	demo{
		compatible = "4412,demo0";
		reg = <0x5000000 0x2 0x5000008 0x2>;
		interrupt-parent = <&gic>;
		interrupts = <0 25 0>, <0 26 0>;
		intpriv = <0x12345678>;
		strpriv = "hello world";
	};
};
*/

struct privatedata {
	int val;
	char str[36];
};
static void getprivdata(struct device_node *np)
{
	struct property *prop;
	prop = of_find_property(np, "intpriv", NULL);
	if(prop)
		printk("private val: %x\n", *((int *)(prop->value)));

	prop = of_find_property(np, "strpriv", NULL);
	if(prop)
		printk("private str: %s\n", (char *)(prop->value) );
}

static int demo_probe(struct platform_device *pdev)
{
	int irq;
	struct resource *addr;
	struct privatedata *priv;
	
	printk(KERN_INFO "%s : %s : %d - entry.\n", __FILE__, __func__, __LINE__);

	priv = dev_get_platdata(&pdev->dev);
	if(priv){
		printk(KERN_INFO "%x : %s \n", priv->val, priv->str);		
	}else{
		getprivdata(pdev->dev.of_node);
	}

	addr = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(addr){
		printk(KERN_INFO "0: %x : %d \n", addr->start, resource_size(addr));		
	}
	addr = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if(addr){
		printk(KERN_INFO "1: %x : %d \n", addr->start, resource_size(addr));		
	}
	addr = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	if(!addr){
		printk(KERN_INFO "No 2 resource\n");		
	}

	irq = platform_get_irq(pdev, 0);
	if(0 > irq){
		return irq;
	}else{
		printk(KERN_INFO "irq 0: %d \n", irq);		
	}
	irq = platform_get_irq(pdev, 1);
	if(0 > irq){
		return irq;
	}else{
		printk(KERN_INFO "irq 0: %d \n", irq);		
	}

	irq = platform_get_irq(pdev, 2);
	if(0 > irq){
			printk(KERN_INFO "No 2 irq\n");		
	}

	return 0;
}

static int demo_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_device_id tbl[] = {
	{"demo0"},
	{"demo1"},
	{},
};
MODULE_DEVICE_TABLE(platform, tbl);

#ifdef CONFIG_OF
struct of_device_id of_tbl[] = {
	{.compatible = "4412,demo0",},
	{.compatible = "4412,demo1",},
	{},
};
#endif

//1. alloc obj
static struct platform_driver drv = {
	.probe	= demo_probe,
	.remove	= demo_remove,

	.driver = {
		.name = "demo",
#ifdef CONFIG_OF
		.of_match_table = of_tbl,
#endif
	},

	.id_table = tbl,
};


static int __init drv_init(void)
{
	//get command and pid
	printk(KERN_INFO "(%s:pid=%d), %s : %s : %d - entry.\n",current->comm, current->pid, __FILE__, __func__, __LINE__);
	return platform_driver_register(&drv);
}
static void __exit drv_exit(void)
{
	//get command and pid
	printk(KERN_INFO "(%s:pid=%d), %s : %s : %d - leave.\n",current->comm, current->pid, __FILE__, __func__, __LINE__);

	platform_driver_unregister(&drv);
}
module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");