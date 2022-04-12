Linux内核部件分析 - 连通世界的list_Linux教程_Linux公社-Linux系统门户网站 - https://www.linuxidc.com/Linux/2011-10/44627.htm

readl()和writel() - 醉依危栏听松涛 - 博客园 - https://www.cnblogs.com/roger0212/p/4436722.html

---

static struct input_dev *gpio_key_dev;

gpio_key_dev = input_allocate_device();



----

dts解析_wdjjwb的专栏-CSDN博客 - https://blog.csdn.net/wdjjwb/article/details/77297372

DTS即Device Tree Source 设备树源码, Device Tree是一种描述硬件的数据结构，它起源于 OpenFirmware (OF)。

device tree的基本单元是node。这些node被组织成树状结构，除了root node，每个node都只有一个parent。一个device tree文件中只能有一个root node。每个node中包含了若干的property/value来描述该node的一些特性。每个node用节点名字（node name）标识，节点名字的格式是node-name@unit-address.

如果该node没有reg属性（后面会描述这个property），那么该节点名字中必须不能包括@和unit-address。unit-address的具体格式是和设备挂在那个bus上相关。例如对于cpu，其unit-address就是从0开始编址，以此加一。而具体的设备，例如以太网控制器，其unit-address就是寄存器地址。



Linux内核中的GPIO系统之（3）：pin controller driver代码分析 - http://www.wowotech.net/linux_kenrel/pin-controller-driver.html

《内核对设备树的处理》 - 一个不知道干嘛的小萌新 - 博客园 - https://www.cnblogs.com/zhuangquan/p/12502174.html

(12条消息) DTS那些事_Huskar_Liu的博客-CSDN博客 - https://blog.csdn.net/weixin_42418557/article/details/89103736?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0.no_search_link&spm=1001.2101.3001.4242

(12条消息) Linux phy驱动开发总结_gaoyang3513的博客-CSDN博客 - https://blog.csdn.net/gaoyang3513/article/details/115363109

Linux Platform驱动模型(二) _驱动方法 - Abnor - 博客园 - https://www.cnblogs.com/xiaojiang1025/archive/2017/02/06/6367910.html

(12条消息) Linux设备模型_在折腾的年纪里的瞎折腾-CSDN博客 - https://blog.csdn.net/jinxin_cuit/category_7090934.html

(12条消息) Linux设备模型(5)_device和device driver_在折腾的年纪里的瞎折腾-CSDN博客 - https://blog.csdn.net/jinxin_cuit/article/details/77059585



```
struct platform_drive
struct device_driver

device_driver结构体 - arm-linux 
- 博客园 - https://www.cnblogs.com/armlinux/archive/2010/09/20/2396909.html
```



```
rtk_gpio_ctl_mlk这个是node的名字，自己可以随便定义，当然最好是见名知意，可以通过驱动程序打印当前使用的设备树节点
        printk(“now dts node name is %s\n",pdev->dev.of_node->name);
```

of_find_node_by_phandle()

dev_get_platdata()



```
linux 解析phanle:
怎么知道是GPIO1 ：interrupt-parent = <&gpio1>;
		power: power_keys@0 {
			status = "okay";
			compatible = "vatics,power-button";
			interrupt-parent = <&gpio1>;
			interrupts = <29 IRQ_TYPE_EDGE_RISING>;
		};

最终获取到GPIO口软件编号：32*1 + 25 =57;
可通过：interrupt-parent = <&gpio1>; --> 接下来呢??
```



---

input子系统学习笔记五 按键驱动实例分析上-fychit-ChinaUnix博客 - http://blog.chinaunix.net/uid-20776117-id-3212088.html



```
input_allocate_device()
(12条消息) input_allocate_device浅析_u012385733的专栏
-CSDN博客 - https://blog.csdn.net/u012385733/article/details/23450635
```

```
devm_kzalloc()
(12条消息) devm_kzalloc()_u010388659的博客
-CSDN博客_devm_kzalloc - https://blog.csdn.net/u010388659/article/details/81265148
```

```
const struct dev_pm_ops *pm;
(12条消息) linux dev pm ops,需要了解的Linux中电源管理接口_我为歌狂有崽了的博客
-CSDN博客 - https://blog.csdn.net/weixin_32421193/article/details/117006146
SIMPLE_DEV_PM_OPS宏 - 王亮1 - 博客园 - https://www.cnblogs.com/liang123/p/6325187.html
(12条消息) Linux kernel pm之我理解_KevinXu101的专栏-CSDN博客_linux pm - https://blog.csdn.net/KevinXu101/article/details/7614294
```

```
linux late_initcall()
(12条消息) Linux驱动late_initcall和module_init相关分析_曾琪玮的博客-CSDN博客_late_initcall module_init - https://blog.csdn.net/qq_21547927/article/details/83002315
(12条消息) Linux驱动late_initcall和module_init相关分析_曾琪玮的博客-CSDN博客_late_initcall module_init - https://blog.csdn.net/qq_21547927/article/details/83002315

```



```
platform_set_drvdata():
platform_set_drvdata() platform_get_drvdata()_烟台 嵌入式产品开发-CSDN博客_platform_set_drvdata - https://blog.csdn.net/shell_albert/article/details/45893373
```

```
regulator_get()
(2条消息) regulator_get 调用过程_☆ 默默地牛逼着 -CSDN博客_regulator_get - https://blog.csdn.net/u012719256/article/details/52083961
Linux regulator系统（1） - Hello-World3 - 博客园 - https://www.cnblogs.com/hellokitty2/p/9975711.html
(2条消息) regulator(转）_lishuiwang的专栏-CSDN博客 - https://blog.csdn.net/lishuiwang/article/details/6130299
```

```
i2c_get_adapter()
i2c_new_probed_device()
i2c_put_adapter()
```



```
request_threaded_irq()
init_waitqueue_head()
wake_up_interruptible()

linux中断申请之request_threaded_irq - 随风飘落的雨滴 - 博客园 - https://www.cnblogs.com/dirt2/p/5632061.html
在驱动程序中使用request_threaded_irq()为什么不使用request_irq()?两者之间有什么区别? - IT屋-程序员软件开发技术分享社区 - https://www.it1352.com/1526844.html
```

