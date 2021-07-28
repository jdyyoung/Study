2021-07-28:

RFC3489/STUN （Simple Traversal of User Datagram Protocol Through Network Address Translators），即简单的用UDP穿透NAT，是个轻量级的协
议，是基于UDP的完整的穿透NAT的解决方案。它允许应用程序发现它们与公共互联网之间存在的NAT和防火墙及其他类型。它也可以让应用
程序确定NAT分配给它们的公网IP地址和端口号。STUN是一种Client/Server的协议，也是一种Request/Response的协议，默认端口号是
3478。（IETF官方文档RFC3489/STUN点此进入） 



STUN协议在RFC5389中被重新命名为Session Traversal Utilities for NAT，即NAT会话穿透效用。在这里，NAT会话穿透效用被定位为一个用
于其他解决NAT穿透问题协议的协议。它可以用于终端设备检查由NAT分配给终端的IP地址和端口号。同时，它也被用来检查两个终端之间的连
接性，好比是一种维持NAT绑定表项的保活协议。STUN可以用于多种NAT类型，并不需要它们提供特殊的行为。 



TURN，在RFC5766中定义，英文全称Traversal Using Relays around NAT（TURN）：Relay Extensions to Session Traversal Utilities for
NAT（STUN），即使用中继穿透NAT：STUN的中继扩展。简单的说，TURN与STUN的共同点都是通过修改应用层中的私网地址达到NAT穿透
的效果，异同点是TURN是通过两方通讯的“中间人”方式实现穿透。 

ICE的全称Interactive Connectivity Establishment（互动式连接建立），由IETF的MMUSIC工作组开发出来的，它所提供的是一种框架，使各种
NAT穿透技术可以实现统一 

2021-07-27：

```
#国内免费可用的stun服务器：
stun.xten.com   
stun.voipbuster.com  
stun.sipgate.net  
stun.ekiga.net
stun.ideasip.com
stun.schlund.de
stun.voiparound.com
stun.voipbuster.com
stun.voipstunt.com
stun.counterpath.com
stun.1und1.de
stun.gmx.net
stun.callwithus.com
stun.counterpath.net
stun.internetcalls.com
numb.viagenie.ca 

转自：https://gist.github.com/zziuni/3741933
  # source : http://code.google.com/p/natvpn/source/browse/trunk/stun_server_list
  # A list of available STUN server.
  stun.l.google.com:19302
  stun1.l.google.com:19302
  stun2.l.google.com:19302
  stun3.l.google.com:19302
  stun4.l.google.com:19302
  stun01.sipphone.com
  stun.ekiga.net
  stun.fwdnet.net
  stun.ideasip.com
  stun.iptel.org
  stun.rixtelecom.se
  stun.schlund.de
  stunserver.org
  stun.softjoys.com
  stun.voiparound.com
  stun.voipbuster.com
  stun.voipstunt.com
  stun.voxgratia.org
  stun.xten.com
```

TUTK：

基于IOTC平台的服务器搭建,台湾TUTK物联网平台

---

物联云连接平台(IOTC Platform)为全方位的物联网解决方案，以智慧型的云端连线架构为基础，加上先进云端运算技术，透过适用于各种不同操作系统的应用程序编程接口(API)，建构了一个全新概念的连接平台。
在物联云连接平台(IOTC Platform)，设备开发者无需自行处理复杂的云端连接问题，只要整合能与物联智慧对接的，可通用于各种不同操作系统的API， 就能与物联云连接平台(IOTC Platform)中任意设备建立跨平台连接。[1]
长久以来，因为大部分的网络设备使用浮动IP的缘故，通过连接互联网时都需要使用DDNS功能加上路由器上的Port Mapping 设定，其复杂程度是非专业人员所能完成的，因此网络设备连接的发展及应用一直受到很大的限制，尤其是在消费型市场一直无法普及。
如今物联云连接平台(IOTC Platform)能够让所有网络设备都可以轻易地互相"通话"，所有的智能终端(手机/平板/电脑/笔记本)、家用电器(电视/空调/洗衣机)、网络设备(路由器/网关)、智慧家庭设备（智能插座/智能门锁/智能窗帘/智能灯泡）、监控设备(IPCam/DVR/NVR/NAS)都能无需设定 IP即可互相连接。

---

5:让摄像头支持P2P访问

无须搭建云服务器，在任何地方都可以随时视频摄像头。

目前这块技术已经有第三方的公司做的比较久了，建议直接整合，没必要自己研发，主要是成本高，我采用的是 TUTK的P2P平台，只需要购买他们的ID号，配置到摄像头里就可以让摄像头在任何地方随时视频，整合也比较简单.

---

TUTK P2P：Kalay云端平台整合设备智慧化过程中所需元素，并提供适用于各种操作系统的应用程序编程接口(API)，让装置得以跨越品牌及通讯协议相互沟通。



RLY/relay:中继服务器

自己动手研发P2P功能的IPC摄像头_嵌入式_imlsq的博客-CSDN博客 - https://blog.csdn.net/imlsq/article/details/51319187

WebRTC和TUTK的P2P_嘉年华_新浪博客 - http://blog.sina.com.cn/s/blog_c636d10e0102y1xq.html

音视频开发——概述（含TUTK demo iOS）_移动开发_蔡兵的专栏-CSDN博客 - https://blog.csdn.net/a411358606/article/details/52168452

TUTK p2p穿透的使用介绍_网络_孤独行者的专栏-CSDN博客 - https://blog.csdn.net/jakejohn/article/details/87445555

Tutk P2P的原理和常见的实现方式_网络_书弋江山的博客-CSDN博客 - https://blog.csdn.net/bobbob32/article/details/78778762?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task

Kalay物联网平台 – 实时影像传输 | 物联智慧 - http://www.throughtek.cn/solution_streaming.html

---

P2P Net 穿越：

Tutk P2P的原理和常见的实现方式_网络_书弋江山的博客-CSDN博客 - https://blog.csdn.net/bobbob32/article/details/78778762?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task

NAT穿透技术详解（udp打洞精髓附代码）_网络_lyztyycode的博客-CSDN博客 - https://blog.csdn.net/lyztyycode/article/details/80865617

[转]P2P原理和常见实现方法_网络_aodiyu3146的博客-CSDN博客 - https://blog.csdn.net/aodiyu3146/article/details/101244750

nat 穿透原理_网络_javascript-CSDN博客 - https://blog.csdn.net/a723811549/article/details/83202775

P2P穿透及TURN转发简要说明_网络_yuanchunsi的博客-CSDN博客 - https://blog.csdn.net/yuanchunsi/article/details/53302694



NAT穿透（UDP打洞）_网络_Barry的博客-CSDN博客 - https://blog.csdn.net/rankun1/article/details/78027027

简单解释 P2P 技术之 UDP 打洞 - 知乎 - https://zhuanlan.zhihu.com/p/40816201

【原创】IP摄像头技术纵览（七）---P2P技术—UDP打洞实现内网NAT穿透_网络_池上好风---码农改变世界-CSDN博客 - https://blog.csdn.net/linczone/article/details/46042639

---

请教UDP 打洞是个什么过程，有成功过的请进。-CSDN论坛 - https://bbs.csdn.net/topics/80304372

楼主可以看看 **stun**协议

---

可以接着参考**STUN/TURN以及ICE**等协议。

STUN/TURN/ICE协议在P2P SIP中的应用_嵌入式_shun35的博客-CSDN博客 - https://blog.csdn.net/shun35/article/details/94642925

P2P技术详解(三)：P2P技术之STUN、TURN、ICE详解 - 哦摩西罗伊 - 博客园 - https://www.cnblogs.com/mlgjb/p/8243690.html

UDP hole punching

从这段代码中可以看出所谓的“打洞”过程就是从服务器上得到目标主机的外网地址和端口。
再向这个地址和端口发送一个UDP数据包。
可是我们知道对于大部分的Cone NAT，只会对服务器发过来的数据包进行转发。对别的IP发过来的数据包是直接丢弃的。

---

ICE协议下NAT穿越的实现（STUN&TURN） - 简书 - https://www.jianshu.com/p/84e8c78ca61d

三. 四种NAT类型：
RFC3489 中将 NAT 的实现分为四大类：

Full Cone NAT （完全锥形 NAT）

Restricted Cone NAT （限制锥形 NAT ，可以理解为 IP 限制，Port不限制）

Port Restricted Cone NAT （端口限制锥形 NAT，IP+Port 限制）

Symmetric NAT （对称 NAT）

---