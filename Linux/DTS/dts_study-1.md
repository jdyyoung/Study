

2021-11-15：

```dts
csi0:csi@0 {
	device_type = "csi0";
	compatible = "allwinner,sunxi-csi";
	device_id = <22>;
	status = "okay";
};
csi1:csi@1 {
	device_type = "csi1";
	compatible = "allwinner,sunxi-csi";
	device_id = <11>;
	status = "okay";
};
```

同级节点，相同compatible 属性值，且status 属性值是“okay”,则都会遍历读取节点的各个属性值。

```
# insmod /skybell/misc/dts_drv_test.ko 
DTS TEST:test_device_id = 22
DTS TEST:test_device_id = 11
```

---

Linux Platform驱动模型(二) _驱动方法 - Abnor - 博客园 - https://www.cnblogs.com/xiaojiang1025/archive/2017/02/06/6367910.html

(11条消息) Linux设备驱动模型之platform总线深入浅出(加入设备树)_小白要学arm-CSDN博客_platform_get_irq - https://blog.csdn.net/qq_31505483/article/details/73718954

(11条消息) Linux中断申请流程_pandy_gao的博客-CSDN博客_platform_get_irq - https://blog.csdn.net/pandy_gao/article/details/79309712

---

节点，子节点，单元地址

属性名，属性值

标签，引用，phandle

节点名规则：

属性名规则：

```
关键字节点：
关键字属性：
compatible, address, interrupt
"mac_addr"，"gpio"，"clock"，"power"。"regulator"
```

- **#address-cells**，用来描述子节点**"reg"**属性的地址表中用来描述首地址的cell的数量，
- **#size-cells**，用来描述子节点**"reg"**属性的地址表中用来描述地址长度的cell的数量。



按key可以表述的数据类型，可以分为：

字符串：string-prop = "a string";
字符串列表：string-list = "hi","str","list";
整型数据：cell-prop = <0xaa>;
二进制数据：binary-prop = [aa bb f8];
混合数据：mixed-prop = "str",<0x123>,[ff dd];
布尔类型：bool-porp;
引用类型：ref-prop = <&other-node>

---

Linux设备树语法详解 - Abnor - 博客园 - https://www.cnblogs.com/xiaojiang1025/p/6131381.html

linux设备驱动(24)dts补充 - Action_er - 博客园 - https://www.cnblogs.com/xinghuo123/p/13238266.html

Linux驱动篇(六)——设备树之DTS规则（一） - 知乎 - https://zhuanlan.zhihu.com/p/143167176

DTS机制_随风的博客-CSDN博客 - https://blog.csdn.net/magicarm_lw/article/details/87722249

Linux内核根据DTS创建设备过程分析（Android 5.1）_随风的博客-CSDN博客 - https://blog.csdn.net/magicarm_lw/article/details/88020798

浅析Linux DeviceTree - ricks - 博客园 - https://www.cnblogs.com/ricks/p/9993245.html

设备树详解 (借点引用, &... , 结构, 节点属性设置如gpio的上拉,下拉,io中断设置等 ) - Red_Point - 博客园 - https://www.cnblogs.com/tureno/articles/6603735.html

Linux dts 设备树详解(一) 基础知识_GREYWALL-CSDN博客 - https://blog.csdn.net/u010632165/article/details/89847843

Linux dts 设备树详解(二) 动手编写设备树dts_GREYWALL-CSDN博客 - https://blog.csdn.net/u010632165/article/details/91488811

Linux设备树解析_smcdef的博客-CSDN博客_linux 设备树解析 - https://blog.csdn.net/smcdef/article/details/77387975

---

(11条消息) Linux设备树解析_smcdef的博客-CSDN博客_linux 设备树解析 - https://blog.csdn.net/smcdef/article/details/77387975

Linux驱动篇(六)——设备树之DTS规则（一） - 知乎 - https://zhuanlan.zhihu.com/p/143167176

linux设备驱动(24)dts补充 - Action_er - 博客园 - https://www.cnblogs.com/xinghuo123/p/13238266.html

，添加了bootargs，memory，clock，interrupt等非platform_device的节点，这样就不能对所有的device_node转换成platform_device。memory，interrupt等虽然是硬件，但是不是platform_device。

---

Linux设备树语法详解 - Abnor - 博客园 - https://www.cnblogs.com/xiaojiang1025/p/6131381.html



编译设备树的时候，**相同的节点的不同属性信息都会被合并，相同节点的相同的属性会被重写**，使用引用可以避免移植者四处找节点，直接在板级.dts增改即可。

---

设备树详解 (借点引用, &... , 结构, 节点属性设置如gpio的上拉,下拉,io中断设置等 ) - Red_Point - 博客园 - https://www.cnblogs.com/tureno/articles/6603735.html

该设备节点中设置了reset-gpios = <&gpio1 15 1>;这格式是什么意思呢？&gpio1 15引用了gpio1节点，故此处含义为gpio1_15这个引脚；最后一个参数1则代表低电平有效，0则为高电平有效。至于gpio1_15具体对应哪个引脚，在imx6的手册上都有详细描述
其实最后一个参数（高低电平有效）不是必须的，因为gpio1节点中设置了#gpio-cells = <2>;，所以才有两个参数；某些soc的gpio节点中会设置为#gpio-cells = <1>;，那么可以不写最后一个参数

---

浅析Linux DeviceTree - ricks - 博客园 - https://www.cnblogs.com/ricks/p/9993245.html

标签和引用是为了方便编写dts文件，当标签用于节点时开发人员可在任意地方引用该标签而不用关注该标签的全路径；当标签应用于属性时，开发人员可以在其他属性中引用该标签，避免重复的工作。例如Label一个字符串，避免在每个需要的地方重复写同一个字符串。

，也可以使用alias为标签定义别名，如：`alias { i2c_0 = &i2c0};`，这样在引用的地方就不用再写&号了，alias+label+reference是非常常用的做法。

Skeleton DTSI
Skeleton的作用是定义设备启动所需要的最小的组件，它定义了root-node下的最基本且必要的child-node类型，通常对应SoC上的基础设施如CPU,Memory等。

/*
 * Skeleton device tree in the 64 bits version; the bare minimum
 * needed to boot; just include and add a compatible value.  The
 * bootloader will typically populate the memory node.
 */
/ {
    #address-cells = <2>;
    #size-cells = <2>;
    cpus { };
    soc { };
    chosen { };
    aliases { };
    memory { device_type = "memory"; reg = <0 0 0 0>; };
};
如上Code来源于skeleton64.dtsi，在root-node下一共定义了5个基本的child-node，cpus代表SoC上所有的cpu，具体cpu的个数以及参数定义在cpus的child-node下；soc代表平台上的所有片上外设以及片外外设，例如uart/clock/spi/i2c/display/...具体这些外设的定义在其他dts/dtsi文件中；chosen用于定义runtime configuration；aliases用于定义node的别名；memory用于定义设备上的物理内存。

address-cells定义了根节点的所有子节点的地址空间属性（reg属性）中使用2个cell来表示启示地址

size-cells定义了根节点的所有子节点的地址空间属性（reg属性）中使用2个cell来表示地址空间的长度

---



2021-0917：

```
sudo apt-get install device-tree-compiler
#DTB转DTS：
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
# cat /proc/interrupts 
           CPU0       
 16:    3257379     GIC-0  27 Edge      gt
 18:          0     GIC-0  30 Level     mpcore_wdt
 20:      10005     GIC-0  87 Level     serial
 22:        383     GIC-0  89 Level   
 23:        707     GIC-0  65 Level     b2d00000.i2c
 24:        424     GIC-0  66 Level     b2e00000.i2c
 25:          0     GIC-0  67 Level     b2f00000.i2c
 26:          0  VPL_MBC_INTC  15 Level     dw-mci
 27:     161539  VPL_MBC_INTC  17 Level     dw-mci
 33:      79358     GIC-0  37 Level     apb_dmac
 34:       4013  VPL_MBC_INTC  19 Level     b8300000.dwc2, b8300000.dwc2, dwc2_hsotg:usb1
 37:          0     GIC-0  39 Level     vpl_dmac0
 38:          5     GIC-0  40 Level     vpl_dmac1
 39:      12382     GIC-0  41 Level     vpl_vic
 45:          3     GIC-0  54 Level   
 46:      12381     GIC-0  55 Level     vma_ifpe
 47:      12379     GIC-0  56 Level     vma_ispe
 48:          0     GIC-0  57 Level     vma_rs
 50:         12     GIC-0  59 Level     vma_jebe
 52:       1398     GIC-0  52 Level     vma_h4ee
 53:          0     GIC-0  53 Level     VPU_CODEC_IRQ
117:          0  VPL_GPIO  29 Edge      power-key
Err:          0
```



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

