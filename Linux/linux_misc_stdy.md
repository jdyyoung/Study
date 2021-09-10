20210910:

```
#查看内核调试log
cat /pro/kmsg
dmesg

//查看映射内存，uboot-env:mem
# cat /proc/iomem 
00000000-00ffffff : System RAM
  00008000-006c0fff : Kernel code
  006ee000-00747b87 : Kernel data
01100000-07ffffff : System RAM
b0100000-b01001df : VMA_H4EE
b2100000-b2100fff : /soc/spi@b2100000
b2700004-b2701003 : serial
b2800004-b2801003 : serial
b2d00000-b2d00fff : /soc/i2c@b2d00000
b2e00000-b2e00fff : /soc/i2c@b2e00000
b2f00000-b2f00fff : /soc/i2c@b2f00000
b3100000-b310000f : /soc/usbphy@b3100000
b3500000-b3500fff : /soc/vpl-gpio@b3500000
b3600000-b3600fff : /soc/vpl-gpio@b3600000
b3700000-b3700fff : /soc/vpl-gpio@b3700000
b3b00004-b3b01003 : serial
b3f00000-b3f000ff : SYSC-MMC
b4100000-b4100fff : /soc/vpl_apbcdma@0xb4100000
b4200000-b4200073 : VPL_DMAC_0
b4300000-b4300073 : VPL_DMAC_1
b4500000-b450063f : VPL_VIC
b4700000-b470004f : VMA_DCE
b4800000-b48007a3 : VMA_IFPE
b4900000-b49007ef : VMA_ISPE
b4a00000-b4a0004b : VMA_JDBE
b4b00000-b4b00127 : VMA_JEBE
b4c00000-b4c00023 : VMA_MEAE
b8100000-b8100fff : /soc/dwmmc0@b8100000
b8200000-b8200fff : /soc/dwmmc1@b8200000
b8300000-b8300fff : /soc/dwc2@b8300000
# 
```



```
Kernel command line: root=/dev/ram0 ro initrd=0x1400000,32M mem=16M@0x00000000 mem=111M@0x1100000 console=0,115200 phy_mode=rmii
PID hash table entries: 512 (order: -1, 2048 bytes)
Dentry cache hash table entries: 16384 (order: 4, 65536 bytes)
Inode-cache hash table entries: 8192 (order: 3, 32768 bytes)
Memory: 88532K/130048K available (5387K kernel code, 211K rwdata, 1320K rodata, 180K init, 147K bss, 41516K reserved, 0K cma-reserved)
Virtual kernel memory layout:
    vector  : 0xffff0000 - 0xffff1000   (   4 kB)
    fixmap  : 0xffc00000 - 0xfff00000   (3072 kB)
    vmalloc : 0xc8800000 - 0xff800000   ( 880 MB)
    lowmem  : 0xc0000000 - 0xc8000000   ( 128 MB)
    modules : 0xbf000000 - 0xc0000000   (  16 MB)
      .text : 0xc0008000 - 0xc054b1f0   (5389 kB)
      .init : 0xc06c1000 - 0xc06ee000   ( 180 kB)
      .data : 0xc06ee000 - 0xc0722f48   ( 212 kB)
       .bss : 0xc0722f48 - 0xc0747b88   ( 148 kB)
```

----

OpenWrt Project: 开发者指南 - https://openwrt.org/zh/docs/guide-developer/start

buildroot教程 - fuzidage - 博客园 - https://www.cnblogs.com/fuzidage/p/12049442.html

---

只打印到这句，可能内核打印串口没打开或者搞错端口，或者mem设置不对

```
phy_mode=rmii
root=/dev/ram0 ro initrd=0x1400000,32M
serverip=192.168.0.6
//111M起得来
setargs=setenv bootargs;setenv bootargs root=${root} mem=16M@0x00000000 mem=111M@0x1100000 console=${tty_num},${baudrate} phy_mode=${phy_mode} ${mtdparts}
//47M 起不来
setargs=setenv bootargs;setenv bootargs root=${root} mem=16M@0x00000000 mem=47M@0x1100000 console=${tty_num},${baudrate} phy_mode=${phy_mode} ${mtdparts}
```



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

