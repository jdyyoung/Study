

Linux内核驱动之GPIO子系统API接口概述 - 走看看 - http://t.zoukankan.com/Cqlismy-p-11891789.html

----

GPIO:

上拉，开漏

gpio 的相关编程：

电气原理图 --> 芯片datasheet (芯片原厂的文档) --> 查表计算，找出gpio序号

可能涉及内核的相关gpio的头文件？修改dts

应用层echo 与 cat的操作

---

```
SPK_SHDN --> 
```

![](..\md_att\1631786411144.png)

M5S_SiP_Pin_Info.xlsx

![](..\md_att\微信截图_20210916185436.png)

GPIO_1_IO_D[9]:1*32 + 9 = 41

 gpio引脚编号计算：

引脚编号 = 控制引脚的寄存器基数 + 控制引脚寄存器位数

```
前面的數字代表group, 每個group都含32個pin.
對系統來說是從group0的pin0開始算起
gpio pin為GPIO_2_IO_D[12], 在系統對應的pin是gpio76 (即 2*32 + 12)
```



```
gpio:硬件GPIO在linux内核中都有一个唯一的软件编号(类似身份证号)：
  	例如：
  	硬件GPIO,GPC0_3对应的软件编号为：S5PV210_GPC0(3);
  	硬件GPIO,GPF1_5对应的软件编号为：S5PV210_GPF1(5)
  	
linux的GPIO驱动的使用（s5pv210）_kunkliu的博客-
CSDN博客 - https://blog.csdn.net/kunkliu/article/details/78036658


计算每个组的start编号，可以参考内核源码的两个文件：\arch\arm\mach-s5pv210\include\mach\Gpio.h    
和arch\arm\mach-s5pv210\Gpiolib.c
```



---



```
以前在sugr:
ls /sys/kernel/debug/aml_reg
echo addr > paddr
cat paddr
echo addr val > paddr
cat paddr

下面两种方法，
1写驱动，给应用提供控制接口。驱动也不要用地址映射的方法去控制，要用标准的gpio接口
2、使用gpio的 sysfs interface。参考：
https://blog.csdn.net/mirkerson/article/details/8464231
https://blog.csdn.net/q1070292175/article/details/17307381

uboot下打印出dts:
fdt addr 0x1000000
fdt print
```

https://blog.csdn.net/mirkerson/article/details/8464231
https://blog.csdn.net/q1070292175/article/details/17307381