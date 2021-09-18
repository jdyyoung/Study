

2021-0917：

```
sudo apt-get install device-tree-compiler
#DCB转DTS：
dtc -I dtb -O dts -o *.dts *.dtb

#DTS转DTB：
./dtc -I dts -O dtb -o test.dtb test.dts

```



基于设备树编写按键中断驱动程序 - 小小的番茄 - 
博客园 - https://www.cnblogs.com/lztutumo/p/13340411.html
Linux DTS中和中断相关属性的解释和用法_月出皎兮。 佼人僚兮。 舒窈纠兮。 劳心悄兮。

CSDN博客 https://blog.csdn.net/rockrockwu/article/details/96461563

> ./simple/linux_irq.c



```
节点：用{}；
	根节点：\
	设备节点：nodex
		节点名称：图中node；
		节点地址：node@0就是节点的地址；
		子节点：childnode
```

Linux内核还约定设备名应写成形如<name>[@<unit_address>]的形式，其中name就是设备名，最长可以是31个字符长度。unit_address一般是设备地址，用来唯一标识一个节点。

Linux设备树语法中定义了一些具有规范意义的属性，包括：compatible, address, interrupt等，这些信息能够在内核初始化找到节点的时候，自动解析生成相应的设备信息。此外，还有一些Linux内核定义好的，一类设备通用的有默认意义的属性，这些属性一般不能被内核自动解析生成相应的设备信息，但是内核已经编写的相应的解析提取函数，常见的有 "mac_addr"，"gpio"，"clock"，"power"。"regulator" 等等。

```
属性：属性名称（Property name）和属性值(Property value)
常见属性：
	compatitable：Linux驱动中可以通过设备节点中的"compatible"这个属性查找设备节点。
	reg:
	
	interrupt-controller：一个空属性用来声明这个node接收中断信号，即这个node是一个中断控制器。
	#interrupt-cells：
	interrupt-parent:
	interrupts:
	
	gpio-controller:，#gpio-cells:
	ranges:
	带#的属性：#address-cells，#size-cells，#interrupt-cells，#gpio-cells
属性值(value)：
    字符串信息
    32bit无符号整型数组信息
    二进制数数组
    字符串哈希表
    以上的混合形式
```



```
标签(label)，通过&可以获取它的值，这里可以简单的理解成一个变量
```

​	

```
#XXX_cells:修饰相应属性的cells
例如： 
	#address-cells，用来描述子节点"reg"属性的地址表中用来描述首地址的cell的数量，
	#size-cells，用来描述子节点"reg"属性的地址表中用来描述地址长度的cell的数量。
```



(5条消息)设备树语法讲解_idea的博客-CSDN博客_设备树 - https://blog.csdn.net/dzw19911024/article/details/82115101

Linux dts 设备树详解(一) 基础知识_GREYWALL-CSDN博客_linux dts设备树详解 - https://blog.csdn.net/u010632165/article/details/89847843

(5条消息)Linux dts 设备树详解(二) 动手编写设备树dts_GREYWALL-CSDN博客_linux dts设备树详解 - https://blog.csdn.net/u010632165/article/details/91488811

phandle： point hander



----

作为uboot到kernel中DTS DTSI DTB等关系_su1041168096的博客-CSDN博客_dts dtb - https://blog.csdn.net/su1041168096/article/details/79283414?utm_source=blogxgwz6

Linux内核DTB文件启动的几种方式 - 凌云物网智科实验室 - 博客园 - https://www.cnblogs.com/iot-yun/p/11403498.html

内核获取dtb:

Bootloader需要将设备树在内存中的地址传给内核。在ARM中通过bootm或bootz命令来进行传递。
bootm [kernel_addr] [initrd_address][dtb_address],其中kernel_addr为内核镜像的地址，init.rd为initrd的地址，dtb_address为dtb所在的地址。若initrd_address为空，则用“-”来代替。

----

```
tarena # 
bootdelay=3
baudrate=115200
ethaddr=00:40:5c:26:0a:5b
lcdtype=X800Y480
mtdids=nand0=s5pv210-nand
mtdparts=mtdparts=s5pv210-nand:1m@0(bios),1m(params),3m(logo),5m(kernel),-(root)
tftp=20008000 zImage_Kernel_3.0.8_TQ210_for_Linux_v2.2_CoreB_20160528.bin;bootm 20008000
filesize=3E640
fileaddr=50008000
gatewayip=192.168.1.2
netmask=255.255.255.0
ipaddr=192.168.1.6
serverip=192.168.1.8
bootcmd=tftp 20008000 zImage ;bootm 20008000
bootargs=root=/dev/nfs nfsroot=192.168.1.8:/opt/rootfs ip=192.168.1.6:192.168.1.8:192.168.1.1:255.255.255.0::eth0:on init=/linuxrc console=ttySAC0,115200
stdin=serial
stdout=serial
stderr=serial
partition=nand0,0
mtddevnum=0
mtddevname=bios

Environment size: 670/16380 bytes
```

```
uboot 执行bootcmd
bootargs 传给kernel,kernel根据bootargs初始化rootfs
```

-----

2021-01-03：

内核添加dts后，device和device_driver的match匹配的变动：通过compatible属性进行匹配_one shot,one kill.-CSDN博客 - https://blog.csdn.net/ruanjianruanjianruan/article/details/61622053

Linux Platform驱动模型(二) _驱动方法 - Abnor - 博客园 - https://www.cnblogs.com/xiaojiang1025/archive/2017/02/06/6367910.html

设备树DTS 学习：2-设备树语法 - 黄树超 - 博客园 - https://www.cnblogs.com/schips/archive/2004/01/13/12208681.html

【Linux笔记】设备树基础知识 - https://mp.weixin.qq.com/s/p3ugT59JHWaDTdZvGsj83g

linux 设备树及节点引用_KjfureOne的专栏-CSDN博客 - https://blog.csdn.net/KjfureOne/article/details/51972854

```C
static const struct of_device_id PS5268_of_match[] = {
	{ .compatible = "vatics,ps5268", },
	{}
};
MODULE_DEVICE_TABLE(of, PS5268_of_match);

static struct platform_driver PS5268_driver = {
	.probe = PS5268_probe,
	.remove = PS5268_remove,
	.driver = {
		.name = "ps5268",
		.owner = THIS_MODULE,
		.of_match_table = PS5268_of_match,
	},
};
module_platform_driver(PS5268_driver);
```

---

哪几个函数：

哪几个结构体：

---

一些概念：

根节点 父节点 子节点 设备节点

节点名字



属性 值

值的类型

大括号{}

尖括号<>

方括号[]

分号； //C语言语法

逗号，

冒号： //标签后面加：

@: 后加单元地址

&:

#:在几个特殊属性上，#address-cells 和 #size-cells

标签 引用

@

&

```
声明别名： 别名 : 节点名
访问 ： &别名
```

常见特殊属性

```
compatible
model
status
#address-cells 和 #size-cells
reg
name
device_type
```

常用节点：

```
根节点
CPU节点
memory节点
chosen节点  //虚拟节点，通过设备树文件给内核传入一些参数

```

IO地址，单元地址

---

操作设备树的函数：OF函数

Linux 内核给我们提供了一系列的函数来获取设备树中的节点或者属性信息，这一系列的函数都有一个统一的前缀“of_”（“open firmware”即开放固件。）

### 1、节点相关操作函数

Linux 内核使用 device_node 结构体来描述一个节点，此结构体定义在文件 include/linux/of.h 中，定义如下：

```C
struct device_node {
	const char *name;
	const char *type;
	phandle phandle;
	const char *full_name;
	struct fwnode_handle fwnode;

	struct	property *properties;
	struct	property *deadprops;	/* removed properties */
	struct	device_node *parent;
	struct	device_node *child;
	struct	device_node *sibling;
	struct	kobject kobj;
	unsigned long _flags;
	void	*data;
#if defined(CONFIG_SPARC)
	const char *path_component_name;
	unsigned int unique_id;
	struct of_irq_controller *irq_trans;
#endif
};
```

查找节点有关的OF函数5个：

#### （1） of_find_node_by_name 函数C

of_find_node_by_name 函数通过节点名字查找指定的节点，函数原型如下：

```C
struct device_node *of_find_node_by_name(struct device_node *from,const char *name);
```

#### （2） of_find_node_by_type 函数

of_find_node_by_type 函数通过 device_type 属性查找指定的节点，函数原型如下：

```C
struct device_node *of_find_node_by_type(struct device_node *from, const char *type);
```

#### （3） of_find_compatible_node 函数

of_find_compatible_node 函数根据 device_type 和 compatible 这两个属性查找指定的节点，函数原型如下：

```C
struct device_node *of_find_compatible_node(struct device_node *from,const char*type,const char *compatible);
```

#### （4）of_find_matching_node_and_match 函数

of_find_matching_node_and_match 函数通过 of_device_id 匹配表来查找指定的节点，函数原型如下：

```C
struct device_node *of_find_matching_node_and_match(struct device_node *from,const struct of_device_id *matches,const struct of_device_id **match);
```

#### （5）of_find_node_by_path 函数

of_find_node_by_path 函数通过路径来查找指定的节点，函数原型如下：

```
inline struct device_node *of_find_node_by_path(const char *path);
```

---

### 2、提取属性值的 OF 函数

Linux 内核中使用结构体 property 表示属性，此结构体同样定义在文件 include/linux/of.h 中，内容如下：

```C
struct property {
	char	*name; 				/*属性名字*/
	int	length;    				/*属性长度*/
	void	*value; 			/*属性值*/
	struct property *next;      /*下一个属性*/
	unsigned long _flags;
	unsigned int unique_id;
	struct bin_attribute attr;
};
```

Linux 内核也提供了提取属性值的 OF 函数 ：6个

#### （1） of_find_property 函数C

of_find_property 函数用于查找指定的属性，函数原型如下：

```C
property *of_find_property(const struct device_node *np,const char *name,int *lenp);
```

#### （2）of_property_count_elems_of_size 函数

of_property_count_elems_of_size 函数用于获取属性中元素的数量，比如 reg 属性值是一个数组，那么使用此函数可以获取到这个数组的大小，此函数原型如下：

```
int of_property_count_elems_of_size(const struct device_node *np,const char *propname,int elem_size);
```

#### （3）读取 u8、 u16、 u32 和 u64 类型的数组数据

```C
int of_property_read_u8_array(const struct device_node *np,
					    	  const char               *propname,
					    	  u8                       *out_values, 
					    	  size_t                   sz)
int of_property_read_u16_array(const struct device_node *np,
					    	  const char               *propname,
					    	  u16                       *out_values, 
					    	  size_t                   sz)					 int of_property_read_u32_array(const struct device_node *np,
					    	  const char               *propname,
					    	  u32                       *out_values, 
					    	  size_t                   sz)   
int of_property_read_u64_array(const struct device_node *np,
					    	  const char               *propname,
					    	  u64                       *out_values, 
					    	  size_t                   sz)                   
```

#### （4）读取 u8、 u16、 u32 和 u64 类型属性值

```
int of_property_read_u8(const struct device_node *np,
					    	  const char               *propname,
					    	  u8                       *out_values)
int of_property_read_u16(const struct device_node *np,
					    	  const char               *propname,
					    	  u16                       *out_values)		int of_property_read_u32(const struct device_node *np,
					    	  const char               *propname,
					    	  u32                       *out_values)
int of_property_read_u64(const struct device_node *np,
					    	  const char               *propname,
					    	  u64                       *out_values)
```

#### （5）of_property_read_string 函数

of_property_read_string 函数用于读取属性中字符串值，函数原型如下：

```C
int of_property_read_string(struct device_node *np,const char *propname,const char **out_string)
```

