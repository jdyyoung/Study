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

