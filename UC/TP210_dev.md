重启tftp服务器软件：

```
sudo /etc/init.d/tftpd-hpa restart
```

重启nfs服务器软件：

```
sudo /etc/init.d/nfs-kernel-server restart
```



---

```
#ifdef CONFIG_MTD_NAND
static struct mtd_partition tq210_partition_info[] = {
	[0] = {
		.name		= "Bootloader",
		.offset		= 0,
		.size		= 0x200000,
		//.mask_flags	= MTD_CAP_NANDFLASH,
		.mask_flags	= 0,/*  jdy 修改于170725*/

	},
/*	[1] = {
		.name		= "Param",
		.offset		= 0x80000,
		.size		= 0x80000,
		.mask_flags	= MTD_CAP_NANDFLASH,
	},*/
	[1] = {
		.name		= "LOGO",
		.offset		= 0x200000,
		.size		= 0x300000,
		//.mask_flags	= MTD_CAP_NANDFLASH,
		.mask_flags	= 0,/*  jdy 修改于170725*/
	},
	[2] = {
		.name		= "Kernel",
		.offset		= 0x500000,
		.size		= 0x500000,
		//.mask_flags	= MTD_CAP_NANDFLASH,
		.mask_flags	= 0,/*  jdy 修改于170725*/
	},
	[3] = {
		.name		= "rootfs",
		.offset		= 0xA00000,
		.size		= 0xA00000,
	},
    /*jdy 20170924 add [4] */
    [4] = {
        .name       = "userdata",
        .offset     = MTDPART_OFS_APPEND,
        .size       = MTDPART_SIZ_FULL,
    
    },
};

struct s3c_nand_mtd_info tq210_nand_info = {
	.chip_nr = 1,
	.mtd_part_nr = ARRAY_SIZE(tq210_partition_info),
	.partition = tq210_partition_info
};
#endif /* CONFIG_MTD_NAND */
```





---

切换网络：

```
cp /etc/network/interfaces.eth /etc/network/interfaces
||
cp /etc/network/interfaces.wifi /etc/network/interfaces

sudo /etc/init.d/networking restart
sudo service network-manager restart
```



```
sudo route add -net 192.168.1.0 netmask 255.255.255.0 eno1

sudo route add -net 192.168.1.0 netmask 255.255.255.0 gw 192.168.1.1
```



---

启动ssh:

```
sudo /etc/init.d/ssh start
```



---

tq210 的uboot -环境变量：

```
mtddevname=bios

Environment size: 670/16380 bytes
tarena # 
bootdelay=3
baudrate=115200
ethaddr=00:40:5c:26:0a:5b
lcdtype=X800Y480
tftp=20008000 zImage_Kernel_3.0.8_TQ210_for_Linux_v2.2_CoreB_20160528.bin;bootm 20008000
filesize=3E640
fileaddr=50008000
gatewayip=192.168.1.2
netmask=255.255.255.0
ipaddr=192.168.1.6
serverip=192.168.1.8
bootcmd=tftp 20008000 zImage ;bootm 20008000
bootargs=root=/dev/nfs nfsroot=192.168.1.8:/opt/rootfs ip=192.168.1.6:192.168.1.8:192.168.1.1:255.255.255.0::eth0:on init=/linuxrc console=ttySAC0,115200
stdin=serial
stdout=serial
stderr=serial
mtdids=nand0=s5pv210-nand
mtdparts=mtdparts=s5pv210-nand:1m@0(bios),1m(params),3m(logo),5m(kernel),-(root)
partition=nand0,0
mtddevnum=0
mtddevname=bios

Environment size: 670/16380 bytes
```

skybell 的环境变量：

```
D3-Boot> pri
__B__=VERIFIED
__factorymode__=0
baudrate=115200
bootcmd=run setargs;nand read 0xa00000 0x1e0000 0x400000;nand read 0x1400000 0x180000 0x60000;bootz 0xa00000 - 0x1400000
bootdelay=3
bootlimit=0
ethact=dweqos
ethaddr=00:00:00:00:00:00
factroymode=1
fdt_high=0xffffffff
gatewayip=192.168.0.1
ipaddr=0.0.0.0
modelname=Schubert
netmask=255.255.0.0
phy_mode=rmii
root=/dev/mtdblock4
serverip=192.168.0.6
setargs=setenv bootargs;setenv bootargs root=${root} mem=16M@0x00000000 mem=111M@0x1100000 console=0,${baudrate} phy_mode=${phy_mode} ${mtdparts}
stderr=serial
stdin=serial
stdout=serial

Environment size: 629/65532 bytes
```

---

```
-setenv bootargs 'root=/dev/nfs nfsroot=192.168.1.8:/opt/rootfs ip=192.168.1.6:192.168.1.8:192.168.1.1:255.255.255.0::eno1:on init=/linuxrc console=ttySAC0,115200'
```

```
setenv bootargs 'root=/dev/mtdblock3 rootfstype=cramfs  init=/linuxrc console=ttySAC0,115200'
```

```

```

---

内核启动卡在 Starting kernel ...

内核启动卡在 Starting kernel ... - Hello-World3 - 博客园 - https://www.cnblogs.com/hellokitty2/p/10092990.html

嵌入式Linux编译内核步骤 / 重点解决机器码问题 / 三星2451 - Carlos·Wei - 博客园 - https://www.cnblogs.com/sigma0/p/7998732.html

(3条消息)S5PV210的kernel移植_Hard struggle-CSDN博客 - https://blog.csdn.net/zjshui/article/details/49203699

Linux内核分析：uboot与Linux内核机器码分析 - LinFeng-Learning - 博客园 - https://www.cnblogs.com/linfeng-learning/p/9285546.html

---

(3条消息)编译内核提示mkimage command not found – U-Boot images will not be built_wiwa@tech-CSDN博客 - https://blog.csdn.net/eibo51/article/details/51901480