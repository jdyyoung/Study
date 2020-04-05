pjsib学习

ALG(应用层网关)

ICE的全称Interactive Connectivity Establishment（互动式连接建立）

ICE Session的使用步骤类似于报告/答复模式(offerer and answerer):

报告/答复模式(offerer and answerer):

---

./configure --host=arm-anykav200-linux-uclibcgnueabi-gcc

make dep&&make clean&&make

----

执行：./configure --host=arm-anykav200-linux-uclibcgnueabi-gcc

报错：

➜  pjproject-2.10 git:(master) ✗ ./configure --host=arm-anykav200-linux-uclibcgnueabi-gcc
checking build system type... x86_64-unknown-linux-gnu
checking host system type... Invalid configuration `arm-anykav200-linux-uclibcgnueabi-gcc': machine `arm-anykav200-linux-uclibcgnueabi' not recognized
aconfigure: error: /bin/bash ./config.sub arm-anykav200-linux-uclibcgnueabi-gcc failed

解决：多了-gcc所以：./configure --host=arm-anykav200-linux-uclibcgnueabi

---



**网络视频监控P2P解决方案_网络_ljh081231的专栏-CSDN博客 - https://blog.csdn.net/ljh081231/article/details/18454537**

**PJSIP开源库详解 - Ray.floyd - 博客园 - https://www.cnblogs.com/rayfloyd/p/7206815.html**

pjsip小结_Python_MR_DUAN1105的博客-CSDN博客 - https://blog.csdn.net/MR_DUAN1105/article/details/97618982

PJSIP_Java_yuanchunsi的博客-CSDN博客 - https://blog.csdn.net/yuanchunsi/article/details/78500382

**PJSIP开发指南 - 大米粥的博客 - 博客园 - https://www.cnblogs.com/damizhou/p/11706605.html**

pjsip简介 - bw_0927 - 博客园 - https://www.cnblogs.com/my_life/articles/2175462.html

开源pjsip使用介绍 - http://www.360doc.com/content/14/0909/18/7821691_408213222.shtml

目前两种流行的Sip服务器源码下载网站：
1、freeswitch
http://files.freeswitch.org/
https://wiki.freeswitch.org/wiki/Configuring_SIP官网介绍
2、opensips
http://opensips.org/pub/opensips/
http://opensips.org/官网介绍

---

pjsip 视频部分学习思路_叶序_新浪博客 - http://blog.sina.com.cn/s/blog_513f4e8401011pks.html

 pjmedia_test  视频的捕捉和播放

  pjmedia 的视频捕捉和传输vid_streamutil.c

  simpleua.c  简单的sip 和视频的协作

  pjsip_ua.c  完整的sip 和视频的协作

PJSIP 添加视频的思路和想法_移动开发_记录程序员的每一天-CSDN博客 - https://blog.csdn.net/a511244213/article/details/49613329

PJSIP 网络视频，电话-Socket-51CTO博客 - https://blog.51cto.com/6951916/1365110

twd_1991的专栏_百无求_CSDN博客-voip,c/c++,openwrt领域博主 - https://blog.csdn.net/twd_1991?t=1

SIP_ying357的专栏_jim123-CSDN博客 - https://blog.csdn.net/ying357/category_1342860.html

pjsip协议栈的一些个人总结_ge的博客-CSDN博客 - https://blog.csdn.net/wsad1201/article/details/80210439?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task

---

已阅文章：

使用pjsip传输已经编码的视频,源码在github-sxcong-ChinaUnix博客 - http://blog.chinaunix.net/uid-15063109-id-4445165.html

pjsip 如何传视频？ - 南守拥 - 博客园 - https://www.cnblogs.com/nanshouyong326/archive/2009/09/16/1567832.html

pjsip 视频部分学习思路_叶序_新浪博客 - http://blog.sina.com.cn/s/blog_513f4e8401011pks.html

关于在pjsip中添加视频的流程说明 - thread-socket-video - ITkeyowrd - http://www.itkeyword.com/doc/9697720499403882796/stream-session-querythreadsocketvideo

---

如何看pjsip:

pjsip协议栈的一些个人总结_ge的博客-CSDN博客 - https://blog.csdn.net/wsad1201/article/details/80210439?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task

pjsip代码分析计划_ying357的专栏-CSDN博客 - https://blog.csdn.net/ying357/article/details/9918459

pjsip_chrycoder的博客-CSDN博客 - https://blog.csdn.net/chrycoder/category_9010082.html

SIP_ying357的专栏_jim123-CSDN博客 - https://blog.csdn.net/ying357/category_1342860.html

tsx（管理事务）

----

源码阅读辅助：

Doxygen文档生成工具 - 简书 - https://www.jianshu.com/p/bf5afbbe183b

Python-doc rst文件打开 - 一只小小的寄居蟹 - 博客园 - https://www.cnblogs.com/xiao-apple36/p/9209338.html

RST文件_Python_grllery的博客-CSDN博客 - https://blog.csdn.net/grllery/article/details/80426875

---

安装python的sphinx模块：

pip install sphinx
pip install -i http://pypi.douban.com/simple/ sphinx_rtd_theme --trusted-host pypi.douban.com

然后到下载的源码的文档路径：/mnt/d/pjproject-2.10/pjproject-2.10/doc/pjsip-book

执行make

//注意：可能修改Makefile指定 ~/.local/bin/sphinx-build

就可以生成开源库源码的开发文档了

sudo apt install doxygen 安装doxygen工具

到各个库里面执行： doxygen docs/doxygen.cfg ，生成库的API文档

---

pjnath NAT穿越的测试：PJNATH - Open Source ICE, STUN, and TURN Library (2.10) - https://www.pjsip.org/pjnath/docs/html/

中文翻译：pjsip教程(一)之PJNATH简介 - 大雪先生 - 博客园 - https://www.cnblogs.com/ishang/p/3413232.html

1. NAT穿越库的选择，笔者推荐PJSIP，网路摄像机以及客户端都可以采用。

   网络视频监控P2P解决方案_网络_ljh081231的专栏-CSDN博客 - https://blog.csdn.net/ljh081231/article/details/18454537

   

PJNATH 测试一_网络_rui631483900的专栏-CSDN博客 - https://blog.csdn.net/rui631483900/article/details/84562271

PJNATH与UDT的整合_网络_cuishumao的专栏-CSDN博客 - https://blog.csdn.net/cuishumao/article/details/12232965

PJNATH介绍 — 开源的用于NAT穿透的ICE, STUN和TURN | 学步园 - https://www.xuebuyuan.com/1586945.html

ICE使用STUN（Session Traversal Utlities for NAT, 其最新的草案是draft-ietf-behave-rfc3489bis-06）协议来做连通性检测。对于转播，它使用TURN（Obtaining
Relay Addresses from Simple Traversal Underneath NAT，其最新的草案是draft-ietf-behave-turn-03）来做同样的检测。

ICEdemo - 程序园 - http://www.voidcn.com/article/p-zmfvdkjx-bgz.html

---

PJNATH库不支持直接处理SDP消息，SDP消息的封装与拆解还需应用程序自己实现。???

pjlib有相应sdp编解码的工具？？