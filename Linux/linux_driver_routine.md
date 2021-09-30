

static struct input_dev *gpio_key_dev;

gpio_key_dev = input_allocate_device();





(12条消息) dts解析_wdjjwb的专栏-CSDN博客 - https://blog.csdn.net/wdjjwb/article/details/77297372

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



(12条消息) 嵌入式linux mipi摄像头v4l2,ARM平台基于嵌入式Linux使用MIPI CSI-2接口_子清视界的博客-CSDN博客 - https://blog.csdn.net/weixin_28786193/article/details/116799887

(12条消息) ㉓AW-H3 Linux驱动开发之mipi camera(CSI)驱动程序___毛豆的博客-CSDN博客 - https://blog.csdn.net/qq_23922117/article/details/85852586

CSI-MIPI学习笔记 - Hello-World3 - 博客园 - https://www.cnblogs.com/hellokitty2/p/7912734.html

linux/imx6-mipi-csi2.c at master · torvalds/linux · GitHub - https://github.com/torvalds/linux/blob/master/drivers/staging/media/imx/imx6-mipi-csi2.c

为什么将I.MX6作为ARM开发的首选 - 知乎 - https://zhuanlan.zhihu.com/p/72711575

(12条消息) linux_驱动_总线协议_MIPI_camera_协议详细解析_笔记_专一的黄先生的博客-CSDN博客 - https://blog.csdn.net/weixin_40452895/article/details/106355754

13. MIPI CSI-2 — The Linux Kernel documentation - https://www.kernel.org/doc/html/v4.10/media/kapi/csi2.html

(12条消息) 嵌入式工程师必备：MIPI CSI-2 接口协议_yanglei0385的博客-CSDN博客 - https://blog.csdn.net/yanglei0385/article/details/85258122