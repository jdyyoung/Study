

OpenWrt Project: 开发者指南 - https://openwrt.org/zh/docs/guide-developer/start

buildroot教程 - fuzidage - 博客园 - https://www.cnblogs.com/fuzidage/p/12049442.html

---

只打印到这句，可能内核打印串口没打开或者搞错端口

```

Starting kernel ...

Uncompressing Linux... done, booting the kernel.
```

Netcat详解_发哥微课堂-CSDN博客_netcat - https://blog.csdn.net/fageweiketang/article/details/82833193

 tcpdump参数解析及使用详解（转载）_lgh1117的专栏-CSDN博客 - https://blog.csdn.net/lgh1117/article/details/80213397

```
./tcpdump -i wlan0 -w /skybell/misc/logs/dns.pcap port 53 &
```

渗透测试_含笑_侠客岛的含笑-CSDN博客 - https://blog.csdn.net/qq_33936481/category_6127545.html

C语言解析WAV音频文件_weixin_30916125的博客-CSDN博客 - https://blog.csdn.net/weixin_30916125/article/details/98033409

ALSA音频工具amixer,aplay,arecord - CSlunatic - 博客园 - https://www.cnblogs.com/cslunatic/p/3227655.html

(3条消息)使用arecord、aplay 实现录音和播放_xiongtiancheng的博客-CSDN博客 - https://blog.csdn.net/xiongtiancheng/article/details/80577478

```
arecord -Dhw:0,0 -r8000 -f S16_LE -c 2  -d 11 /tmp/1.wmv
```



```
/etc/inittab::respawn
```

```
/proc/modules
```

```
/etc/profile
```



uboot学习笔记之uboot版本选择（一） - 程序员大本营 - https://www.pianshen.com/article/5738473092/

新版本uboot和旧版本的区别 - JustRelax - 博客园 - https://www.cnblogs.com/awsqsh/articles/4508726.html



```
D3-Boot> help
?       - alias for 'help'
base    - print or set address offset
boot    - boot default, i.e., run 'bootcmd'
bootd   - boot default, i.e., run 'bootcmd'
booti   - boot arm64 Linux Image image from memory
bootm   - boot application image from memory
bootp   - boot image via network using BOOTP/TFTP protocol
bootz   - boot Linux zImage image from memory
chpart  - change active partition
cmp     - memory compare
coninfo - print console devices and information
cp      - memory copy
crc32   - checksum calculation
dcache  - enable or disable data cache
dce     - DCE
echo    - echo args to console
editenv - edit environment variable
enter_ns- switch to non secure
env     - environment handling commands
fatinfo - print information about filesystem
fatload - load binary file from a dos filesystem
fatls   - list files in a directory (default /)
fatsize - determine a file's size
fatwrite- write file into a dos filesystem
fdt     - flattened device tree utility commands
giu     - Giuliani control
go      - start application at address 'addr'
help    - print command description/usage
i2c     - I2C sub-system
icache  - enable or disable instruction cache
ini     - parse an ini file in memory and merge the specified section into the env
loop    - infinite loop on address range
md      - memory display
mm      - memory modify (auto-incrementing address)
mmc     - MMC sub system
mmcinfo - display MMC info
mtdparts- define flash/nand partitions
mtest   - simple RAM read/write test
mw      - memory write (fill)
nand    - NAND sub-system
nboot   - boot from NAND device
nm      - memory modify (constant address)
part    - disk partition related commands
printenv- print environment variables
reset   - Perform RESET of the CPU
run     - run commands in an environment variable
saveenv - save environment variables to persistent storage
setenv  - set environment variables
sf      - SPI flash sub-system
sleep   - delay execution for some time
source  - run script from memory
sys     - System configuration 
tftpboot- boot image via network using TFTP protocol
usb     - USB sub-system
usbboot - boot from USB device
version - print monitor, compiler and linker version
D3-Boot> 
```

