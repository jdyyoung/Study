

图像格式与视频数字化 && sensor的类型（YUV sensor ，Raw sensor）&& RAW sensor预览碰到的颜色问题_zhandoushi的专栏-CSDN博客 - https://blog.csdn.net/zhandoushi1982/article/details/5011321

YCbCr_百度百科 - https://baike.baidu.com/item/YCbCr/10012133?fr=aladdin

YUV是编译true-color颜色空间（color space）的种类，Y'UV, YUV, YCbCr，YPbPr等专有名词都可以称为YUV，彼此有重叠。“Y”表示明亮度（Luminance或Luma），也就是灰阶值，“U”和“V”表示的则是色度（Chrominance或Chroma），作用是描述影像色彩及饱和度，用于指定像素的颜色。

颜色空间---RGB、YUV和YCbCr的理解 - Dec-Fth - 博客园 - https://www.cnblogs.com/faith0217/articles/4262913.html

颜色空间系列3: RGB和YCbCr颜色空间的转换及优化算法 - Imageshop - 博客园 - https://www.cnblogs.com/Imageshop/archive/2013/02/14/2911309.html

(11条消息) 非常难得的 CMOS sensor 工作原理的深入技术科普_sadamoo的专栏-CSDN博客_cmos sensor - https://blog.csdn.net/sadamoo/article/details/86307841

(Y:明亮度、灰度，UV:色度、饱和度):

YUV422P

YUV420

YVU420

NV16

NV12

NV61

NV21

YUYV

YVYU

UYVY

VYUY

(11条消息) YUV_NV21图像数据到RGB颜色空间的转换_CG-SpatialAI-CSDN博客_nv21 - https://blog.csdn.net/u011178262/article/details/84201597

Datahammer 7yuv YUV Viewer, Raw Graphics Editor and Hex Editor - http://datahammer.de/

NV21与I420 - 简书 - https://www.jianshu.com/p/9ad01d4f824c

图像格式与视频数字化 && sensor的类型（YUV sensor ，Raw sensor）&& RAW sensor预览碰到的颜色问题_zhandoushi的专栏-CSDN博客 - https://blog.csdn.net/zhandoushi1982/article/details/5011321

 Sensor信号输出YUV、RGB、RAW DATA、JPEG 4种方式区别_southcamel的专栏-CSDN博客 - https://blog.csdn.net/southcamel/article/details/8305873

----

Bitmap（位图）

*音圈马达* （Voice Coil Actuator/ Voice Coil Motor）

相机上的音圈马达（vcm）是什么意思？_百度知道 - https://zhidao.baidu.com/question/167682716.html

```

```

MIPI-DSI属于MIPI子协议，为 Display工作组制定的关于显示模组接口的规范标准。MIPI-DSI使用D-PHY作为物理层传输。
D-PHY采用1对源同步的差分时钟和1～4对差分数据线来进行数据传输。数据传输采用DDR方式，即在时钟的上下边沿都有数据传输。
（1）D-PHY传输状态：低功耗LP和高速HS。
LP（单信号0V~1.2V）：低功耗模式，10Mbps传输速度、异步传输，主要用于传输控制命令。
HS（差分信号100mv~300mv）：高速模式，80M~1Gbps传输速度、同步传送，用于传输高速图像数据。
HS差分和LP单信号如图3所示。
![](.\md_att\825709-1F323110131491.png)

**（2）D-PHY三种模式：**控制模式（Control Mode）、Escape Mode、和高速模式（HS Mode）

其中前两种模式都属于LP状态下的两种模式，高速模式属于HS状态， Escape Mode被定义为LP状态下的一种比较特殊的操作模式。

![](.\md_att\825709-1F323110144259.png)

MIPI-DSI利用单端和差分信号线进行数据传输，LP模式下进行单端数据传输，HS模式下进行差分数据传输，使用可以双向传输的Data0+/Data0-数据Lane进行配置。

 ***3、MIPI-DSI数据传输格式***

MIPI-DSI的数据传输格式以数据包为基本单元传输，数据包的类型分为 **短数据包和长数据包。**

**（1）、短数据包：4 bytes（固定长度）**主要用于传输命令、读写寄存器；

数据包包头：

**· 数据标识符（DI）\*1byte：**含虚拟数据通道【7:6】和数据类型【5:0】。

**· 数据包\*2 byte：**要传送的数据，长度固定两个字节。

**· 误差校正码（ECC）\*1byte：**可以把一个位的错误纠正。

![](.\md_att\825709-1F32311015K92.png)

Mobile industry process interface

MIPI CSI2:camera serial interface 2 代

差分信号P高N低表1，P低N高表0.

**（2）、长数据包：6~65541 bytes（动态长度）**主要用于传输大量图像数据或者部分控制命令。

数据包报头（4 bytes）：

**· 数据标识符（DI）\*1byte：**含虚拟数据通道【7:6】和数据类型【5:0】。

**· 字节数（WC）\*2 byte：**要传送的数据，长度固定两个字节。

**· 误差校正码（ECC）\*1byte：**可以把一个位的错误纠正。

**· 有效传送数据（6~65541 bytes）：**最大字节=2^16。

**· 数据包页脚（2 byte）：**校验。

![](.\md_att\825709-1F323110210521.png)

---

D-phy包含：
• 一对时钟lane
• 1-4对数据lane 



短包一般是用来发送同步控制信号的，一般不建议使用短包来发送用户数据：帧开始与帧结束。

---

如何使用示波器分析手机中的MIPI-DSI协议？ - 网络/协议 - 电子发烧友网 - http://www.elecfans.com/news/wangluo/20170323499614.html

(1条消息) MIPI CSI 正确图像信号参考图-----用于调试参考对照使用_jzwjzw19900922的博客-CSDN博客 - https://blog.csdn.net/jzwjzw19900922/article/details/103628800

入门视频采集与处理（BT656简介） 转 - Finley - 博客园 - https://www.cnblogs.com/s_agapo/archive/2012/04/08/2437775.html



嵌入式开发之mipi干货_CNMhukaun的博客-CSDN博客 - https://blog.csdn.net/CNMhukaun/article/details/47811365?locationNum=4&fps=1

MIPI入门——CSI-2介绍（三）_简单同学-CSDN博客_mipi长包短包 - https://blog.csdn.net/l471094842/article/details/95619452

mipi 调试经验 - Magnum Programm Life - 博客园 - https://www.cnblogs.com/biglucky/p/4154871.html

MIPI CSI2学习（一）：说一说MIPI CSI2_会飞的胖子的博客-CSDN博客 - https://blog.csdn.net/u011652362/article/details/81741134

mipi-rx之软件篇 - 走看看 - http://t.zoukankan.com/fuzidage-p-14107768.html

MIPI入门——CSI-2介绍（三）_简单同学-CSDN博客_mipi长包短包 - https://blog.csdn.net/l471094842/article/details/95619452

---

(12条消息) 嵌入式linux mipi摄像头v4l2,ARM平台基于嵌入式Linux使用MIPI CSI-2接口_子清视界的博客-CSDN博客 - https://blog.csdn.net/weixin_28786193/article/details/116799887

(12条消息) ㉓AW-H3 Linux驱动开发之mipi camera(CSI)驱动程序___毛豆的博客-CSDN博客 - https://blog.csdn.net/qq_23922117/article/details/85852586

CSI-MIPI学习笔记 - Hello-World3 - 博客园 - https://www.cnblogs.com/hellokitty2/p/7912734.html

linux/imx6-mipi-csi2.c at master · torvalds/linux · GitHub - https://github.com/torvalds/linux/blob/master/drivers/staging/media/imx/imx6-mipi-csi2.c

为什么将I.MX6作为ARM开发的首选 - 知乎 - https://zhuanlan.zhihu.com/p/72711575

(12条消息) linux_驱动_总线协议_MIPI_camera_协议详细解析_笔记_专一的黄先生的博客-CSDN博客 - https://blog.csdn.net/weixin_40452895/article/details/106355754

13. MIPI CSI-2 — The Linux Kernel documentation - https://www.kernel.org/doc/html/v4.10/media/kapi/csi2.html

(12条消息) 嵌入式工程师必备：MIPI CSI-2 接口协议_yanglei0385的博客-CSDN博客 - https://blog.csdn.net/yanglei0385/article/details/85258122



MIPI CSI-2简介 - liuwanpeng - 博客园 - https://www.cnblogs.com/liuwanpeng/p/14388638.html

---

拜耳阵列（Bayer Pattern）简介_兔角与禅-CSDN博客_bayer pattern - https://blog.csdn.net/edogawachia/article/details/84963652





---

```
luma
chroma

```

“Y”表示明亮度（Luminance或Luma），也就是灰阶值

“U”和“V”表示的则是色度（Chrominance或Chroma），作用是描述影像色彩及饱和度，用于指定像素的颜色。

