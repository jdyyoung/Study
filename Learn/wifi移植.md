mjpg-streamer-r63$make
gcc -O2 -DLINUX -D_GNU_SOURCE -Wall   -c -o mjpg_streamer.o mjpg_streamer.c
In file included from mjpg_streamer.c:27:0:
/usr/include/linux/videodev.h:17:29: 致命错误： linux/videodev2.h：没有那个文件或目录
编译中断。
make: *** [mjpg_streamer.o] 错误 1


/usr/include/linux/videodev2.h:64:28: 致命错误： linux/compiler.h：没有那个文
或目录
编译中断。
make: *** [mjpg_streamer.o] 错误 1



insmod: can't insert 'mtutil7601Usta.ko': invalid module format


--> RTMPAllocTxRxRingMemory
<-- RTMPAllocTxRxRingMemory, Status=0
<-- RTMPAllocAdapterBlock, Status=0
NumEndpoints=8
BULK IN MaxPacketSize = 512
EP address = 0x84
BULK IN MaxPacketSize = 512
EP address = 0x85
BULK OUT MaxPacketSize = 512
EP address = 0x 8  
BULK OUT MaxPacketSize = 512
EP address = 0x 4  
BULK OUT MaxPacketSize = 512
EP address = 0x 5  
BULK OUT MaxPacketSize = 512
EP address = 0x 6  
BULK OUT MaxPacketSize = 512
EP address = 0x 7  
BULK OUT MaxPacketSize = 512
EP address = 0x 9  
RTMP_COM_IoctlHandle():pAd->BulkOutEpAddr=0x8
RTMP_COM_IoctlHandle():pAd->BulkOutEpAddr=0x4
RTMP_COM_IoctlHandle():pAd->BulkOutEpAddr=0x5
RTMP_COM_IoctlHandle():pAd->BulkOutEpAddr=0x6
RTMP_COM_IoctlHandle():pAd->BulkOutEpAddr=0x7
RTMP_COM_IoctlHandle():pAd->BulkOutEpAddr=0x9
STA Driver version-3.0.0.3
-->MT7601_Init():
Chip specific bbpRegTbSize=0!
Chip VCO calibration mode = 0!
NVM is EFUSE
Efuse Size=0x1d [Range:1e0-1fc] 
Endpoint(8) is for In-band Command
Endpoint(4) is for WMM0 AC0
Endpoint(5) is for WMM0 AC1
Endpoint(6) is for WMM0 AC2
Endpoint(7) is for WMM0 AC3
Endpoint(9) is for WMM1 AC0
Endpoint(84) is for Data-In
Endpoint(85) is for Command Rsp
Allocate a net device with private data size=0!
Allocate net device ops success!
The name of the new ra interface is ra0...
RtmpOSNetDevAttach()--->
<---RtmpOSNetDevAttach(), ret=0
<===rt2870_probe()!


/home/drivers # usb 1-1.1: USB disconnect, address 5
rtusb_disconnect: unregister usbnet usb-s5pv210-1.1
RtmpOSNetDevDetach(): RtmpOSNetDeviceDetach(), dev->name=ra0!
netconsole: network logging stopped, interface ra0 unregistered
---> RTMPFreeTxRxRingMemory
<--- RTMPFreeTxRxRingMemory
 RTUSB disconnect successfully\
 
 
 
 MT7601驱动移植 - zqj6893的专栏 - 博客频道 - CSDN.NET
http://blog.csdn.net/zqj6893/article/details/44258439

编译时指定架构，指定芯片
make ARCH=arm CHIPSET=7601U 

出现如下错误：
/home/drivers # insmod rt5370sta.ko 
rt5370sta: module license 'unspecified' taints kernel.
Disabling lock debugging due to kernel taint
rt5370sta: Unknown symbol usb_alloc_urb (err 0)
rt5370sta: Unknown symbol usb_free_urb (err 0)
rt5370sta: Unknown symbol usb_alloc_coherent (err 0)
rt5370sta: Unknown symbol usb_register_driver (err 0)
rt5370sta: Unknown symbol usb_put_dev (err 0)
rt5370sta: Unknown symbol usb_get_dev (err 0)
rt5370sta: Unknown symbol usb_submit_urb (err 0)
rt5370sta: Unknown symbol usb_free_coherent (err 0)
rt5370sta: Unknown symbol usb_control_msg (err 0)
rt5370sta: Unknown symbol usb_deregister (err 0)
rt5370sta: Unknown symbol usb_kill_urb (err 0)
insmod: can't insert 'rt5370sta.ko': unknown symbol in module, or unknown parameter

解决网址：
rt3070sta: module license 'unspecified' taints kernel... SOLVED
http://www.linuxforums.org/forum/wireless-internet/161550-rt3070sta-module-license-unspecified-taints-kernel-solved.html

Open this file in a text editor

/RT3070_LinuxSTA_V2.3.0.1_20100208/os/linux/usb_main_dev.c

Change this:
Code:
#include "rt_config.h"


// Following information will be show when you run 'modinfo'
// *** If you have a solution for the bug in current version of driver, please mail to me.
// Otherwise post to forum in ralinktech's web site(www.ralinktech.com) and let all users help you. ***
MODULE_AUTHOR("Paul Lin <paul_lin@ralinktech.com>");
MODULE_DESCRIPTION("RT2870 Wireless Lan Linux Driver");
#ifdef CONFIG_STA_SUPPORT
#ifdef MODULE_VERSION
MODULE_VERSION(STA_DRIVER_VERSION);
#endif
#endif // CONFIG_STA_SUPPORT //
To this:
Code:
#include "rt_config.h"


// Following information will be show when you run 'modinfo'
// *** If you have a solution for the bug in current version of driver, please mail to me.
// Otherwise post to forum in ralinktech's web site(www.ralinktech.com) and let all users help you. ***
MODULE_AUTHOR("Paul Lin <paul_lin@ralinktech.com>");
MODULE_DESCRIPTION("RT2870 Wireless Lan Linux Driver");
MODULE_LICENSE("GPL");
#ifdef CONFIG_STA_SUPPORT
#ifdef MODULE_VERSION
MODULE_VERSION(STA_DRIVER_VERSION);
#endif
#endif // CONFIG_STA_SUPPORT //
You are adding the 'MODULE_LICENSE("GPL");' line, that is highlighted in red. 





/home/wifi # ./hostapd  /etc/rtl_hostapd_2G.conf -B
Configuration file: /etc/rtl_hostapd_2G.conf
drv->ifindex=8
l2_sock_recv==l2_sock_xmit=0x0x66638
===>rt_ioctl_siwmode::SIOCSIWMODE (unknown 3)
ioctl[SIOCSIWMODE]: Invalid argument
Could not set interface to mode(3)!
Could not set interface to master mode!
rtl871xdrv driver initialization failed.
rmdir[ctrl_interface]: No such file or directory
/home/wifi # 


2.3.?Libraries
http://siso.sourceforge.net/toolchain-libs.html

http://pcmcia-cs.sourceforge.net/ftp/contrib/wireless_tools.28.tar.gz

wifi 驱动移植范例-iibull-ChinaUnix博客
http://blog.chinaunix.net/uid-27875-id-3238418.html

dnsmasq: cannot open or create lease file /var/lib/misc/dnsmasq.leases: No such file or directory

libvirt的dnsmasq使用 - 爱悠闲,快乐工作,悠闲生活!
http://www.aiuxian.com/article/p-1182299.html

dnsmasq: unknown user or group: root

dnsmasq: failed to bind DHCP server socket: Address already in use


基于ARM 平台的dnsmasq工具的使用-chenzhongpeng-ChinaUnix博客
http://blog.chinaunix.net/uid-29287950-id-5581666.html

MT7601 - BananaPro/Pi
http://wiki.lemaker.org/MT7601/zh-hans


【L】小米随身WiFi，Linux下AP热点驱动（开源） - sumang_87的专栏 - 博客频道 - CSDN.NET
http://blog.csdn.net/sumang_87/article/details/38168877



