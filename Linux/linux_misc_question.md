



```
D3-Boot> pri
SKYBELL_FWVER_ENV=SB_SLIM2_0001
__A__=VERIFIED
__B__=FAILED
__SN__=AAAAA00000
__factorymode__=1
baudrate=115200
bootcmd=run setargs;nand read 0xa00000 ${nand-kernel-A};nand read 0x1400000 ${nand-rootfs-A};bootz 0xa00000
bootdelay=3
bootlimit=0
ethact=dweqos
ethaddr=00:00:00:00:00:00
factroymode=1
fdt_high=0xffffffff
gatewayip=192.168.0.1
ipaddr=0.0.0.0
modelname=Schubert
nand-kernel-A=0x180000 0x400000
nand-kernel-B=0x580000 0x400000
nand-rootfs-A=0x980000 0x2000000
nand-rootfs-B=0x2980000 0x2000000
nandflash_chip_id=0x12
netmask=255.255.0.0
phy_mode=rmii
root=/dev/ram0 ro initrd=0x1400000,32M
serverip=192.168.0.6
setargs=setenv bootargs;setenv bootargs root=${root} mem=16M@0x00000000 mem=47M@0x1100000 console=${tty_num},${baudrate} phy_mode=${phy_mode} ${mtdparts}
stderr=serial
stdin=serial
stdout=serial
tty_num=0

Environment size: 888/65532 bytes
D3-Boot>
```



```
phy_mode=rmii
root=/dev/ram0 ro initrd=0x1400000,32M
serverip=192.168.0.6
setargs=setenv bootargs;setenv bootargs root=${root} mem=16M@0x00000000 mem=111M@0x1100000 console=${tty_num},${baudrate} phy_mode=${phy_mode} ${mtdparts}
setargs=setenv bootargs;setenv bootargs root=${root} mem=16M@0x00000000 mem=47M@0x1100000 console=${tty_num},${baudrate} phy_mode=${phy_mode} ${mtdparts}
```



开机起不来，只打印到这....

```
Kernel image @ 0xa00000 [ 0x000000 - 0x39d7d8 ]
Starting kernel ...
Uncompressing Linux... done, booting the kernel.
```



```
   4.3.6.在内核的启动参数中指定mem=10M,告诉内核将物理内存的最后10M预留出来,预留给驱动单独使用,利用ioremap函数进行映射即可访问
    setenv bootargs root=/dev/nfs nfsroot=... mem=10M
    saveenv
    boot
    将来驱动利用ioremap函数将这10M的物理内存进行映射，即可访问
```





```
sudo cat /proc/iomem
cat /proc/cmdline
```

```
# cat /proc/cmdline
root=/dev/ram0 ro initrd=0x1400000,32M mem=16M@0x00000000 mem=111M@0x1100000 console=0,115200 phy_mode=rmii
# cat /proc/iomem
00000000-00ffffff : System RAM
  00008000-006c0fff : Kernel code
  006ee000-00747b87 : Kernel data
01100000-07ffffff : System RAM
08020200-08020273 : 
083b2380-083b2833 : 
083b3300-083b3aa3 : 
08949f80-0894a76f : 
0894de00-0894de73 : L
08a4c780-08a4cf6f : h;,@
0a450280-0a45045f : 
0a7a2880-0a7a28f3 : 
0afa2900-0afa2adf : 
0b2f4f00-0b2f4f73 : 
0bafa480-0bafa5a7 : 
0bafae80-0bafaef3 : 
0c479900-0c479adf : 
0c55ca80-0c55caf3 : 
0c6d2a00-0c6d31ef : 
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

《uboot环境变量：详谈bootcmd 和bootargs》 - 一个不知道干嘛的小萌新 - 博客园 - https://www.cnblogs.com/zhuangquan/p/11393128.html