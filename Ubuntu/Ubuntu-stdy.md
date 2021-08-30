

20210827：

ubuntu crc32 安装与使用：

注意大小字节序！

```
$ sudo apt-get install libarchive-zip-perl
rck@rck:/vtcs/code/uboot_env_crc32$ crc32 uboot-slim2-B1BB700084.env.ok
fdd6526b
```

hexdump的使用：

```
rck@rck:/vtcs/code/uboot_env_crc32$ hexdump -C uboot-slim2-B1BB700084.env
00000000  b5 7a 37 b6 53 4b 59 42  45 4c 4c 5f 46 57 56 45  |.z7.SKYBELL_FWVE|
00000010  52 5f 45 4e 56 3d 53 42  5f 53 4c 49 4d 32 5f 30  |R_ENV=SB_SLIM2_0|
00000020  30 30 31 00 5f 5f 41 5f  5f 3d 56 45 52 49 46 49  |001.__A__=VERIFI|
00000030  45 44 00 5f 5f 42 5f 5f  3d 46 41 49 4c 45 44 00  |ED.__B__=FAILED.|
00000040  62 61 75 64 72 61 74 65  3d 31 31 35 32 30 30 00  |baudrate=115200.|
00000050  62 6f 6f 74 63 6d 64 3d  72 75 6e 20 73 65 74 61  |bootcmd=run seta|
00000060  72 67 73 3b 6e 61 6e 64  20 72 65 61 64 20 30 78  |rgs;nand read 0x|
00000070  61 30 30 30 30 30 20 24  7b 6e 61 6e 64 2d 6b 65  |a00000 ${nand-ke|
00000080  72 6e 65 6c 2d 41 7d 3b  6e 61 6e 64 20 72 65 61  |rnel-A};nand rea|
00000090  64 20 30 78 31 34 30 30  30 30 30 20 24 7b 6e 61  |d 0x1400000 ${na|
000000a0  6e 64 2d 72 6f 6f 74 66  73 2d 41 7d 3b 62 6f 6f  |nd-rootfs-A};boo|
000000b0  74 7a 20 30 78 61 30 30  30 30 30 00 62 6f 6f 74  |tz 0xa00000.boot|
000000c0  64 65 6c 61 79 3d 33 00  62 6f 6f 74 6c 69 6d 69  |delay=3.bootlimi|
000000d0  74 3d 30 00 65 74 68 61  63 74 3d 64 77 65 71 6f  |t=0.ethact=dweqo|
000000e0  73 00 65 74 68 61 64 64  72 3d 30 30 3a 30 30 3a  |s.ethaddr=00:00:|
000000f0  30 30 3a 30 30 3a 30 30  3a 30 30 00 66 61 63 74  |00:00:00:00.fact|
00000100  72 6f 79 6d 6f 64 65 3d  31 00 66 64 74 5f 68 69  |roymode=1.fdt_hi|
00000110  67 68 3d 30 78 66 66 66  66 66 66 66 66 00 67 61  |gh=0xffffffff.ga|
00000120  74 65 77 61 79 69 70 3d  31 39 32 2e 31 36 38 2e  |tewayip=192.168.|
00000130  30 2e 31 00 69 70 61 64  64 72 3d 30 2e 30 2e 30  |0.1.ipaddr=0.0.0|
00000140  2e 30 00 6d 6f 64 65 6c  6e 61 6d 65 3d 53 63 68  |.0.modelname=Sch|
00000150  75 62 65 72 74 00 6e 61  6e 64 2d 6b 65 72 6e 65  |ubert.nand-kerne|
00000160  6c 2d 41 3d 30 78 31 38  30 30 30 30 20 30 78 34  |l-A=0x180000 0x4|
00000170  30 30 30 30 30 00 6e 61  6e 64 2d 6b 65 72 6e 65  |00000.nand-kerne|
00000180  6c 2d 42 3d 30 78 35 38  30 30 30 30 20 30 78 34  |l-B=0x580000 0x4|
00000190  30 30 30 30 30 00 6e 61  6e 64 2d 72 6f 6f 74 66  |00000.nand-rootf|
000001a0  73 2d 41 3d 30 78 39 38  30 30 30 30 20 30 78 32  |s-A=0x980000 0x2|
000001b0  30 30 30 30 30 30 00 6e  61 6e 64 2d 72 6f 6f 74  |000000.nand-root|
000001c0  66 73 2d 42 3d 30 78 32  39 38 30 30 30 30 20 30  |fs-B=0x2980000 0|
000001d0  78 32 30 30 30 30 30 30  00 6e 65 74 6d 61 73 6b  |x2000000.netmask|
000001e0  3d 32 35 35 2e 32 35 35  2e 30 2e 30 00 70 68 79  |=255.255.0.0.phy|
000001f0  5f 6d 6f 64 65 3d 72 6d  69 69 00 72 6f 6f 74 3d  |_mode=rmii.root=|
00000200  2f 64 65 76 2f 72 61 6d  30 20 72 6f 20 69 6e 69  |/dev/ram0 ro ini|
00000210  74 72 64 3d 30 78 31 34  30 30 30 30 30 2c 33 32  |trd=0x1400000,32|
00000220  4d 00 73 65 72 76 65 72  69 70 3d 31 39 32 2e 31  |M.serverip=192.1|
00000230  36 38 2e 30 2e 36 00 73  65 74 61 72 67 73 3d 73  |68.0.6.setargs=s|
00000240  65 74 65 6e 76 20 62 6f  6f 74 61 72 67 73 3b 73  |etenv bootargs;s|
00000250  65 74 65 6e 76 20 62 6f  6f 74 61 72 67 73 20 72  |etenv bootargs r|
00000260  6f 6f 74 3d 24 7b 72 6f  6f 74 7d 20 6d 65 6d 3d  |oot=${root} mem=|
00000270  31 36 4d 40 30 78 30 30  30 30 30 30 30 30 20 6d  |16M@0x00000000 m|
00000280  65 6d 3d 31 31 31 4d 40  30 78 31 31 30 30 30 30  |em=111M@0x110000|
00000290  30 20 63 6f 6e 73 6f 6c  65 3d 24 7b 74 74 79 5f  |0 console=${tty_|
000002a0  6e 75 6d 7d 2c 24 7b 62  61 75 64 72 61 74 65 7d  |num},${baudrate}|
000002b0  20 70 68 79 5f 6d 6f 64  65 3d 24 7b 70 68 79 5f  | phy_mode=${phy_|
000002c0  6d 6f 64 65 7d 20 24 7b  6d 74 64 70 61 72 74 73  |mode} ${mtdparts|
000002d0  7d 00 73 74 64 65 72 72  3d 73 65 72 69 61 6c 00  |}.stderr=serial.|
000002e0  73 74 64 69 6e 3d 73 65  72 69 61 6c 00 73 74 64  |stdin=serial.std|
000002f0  6f 75 74 3d 73 65 72 69  61 6c 00 6e 61 6e 64 66  |out=serial.nandf|
00000300  6c 61 73 68 5f 63 68 69  70 5f 69 64 3d 30 78 31  |lash_chip_id=0x1|
00000310  32 00 5f 5f 53 4e 5f 5f  3d 42 31 42 42 37 30 30  |2.__SN__=B1BB700|
00000320  30 38 34 00 74 74 79 5f  6e 75 6d 3d 33 00 5f 5f  |084.tty_num=3.__|
00000330  66 61 63 74 6f 72 79 6d  6f 64 65 5f 5f 3d 30 00  |factorymode__=0.|
00000340  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00005000  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
*
00010000
rck@rck:/vtcs/code/uboot_env_crc32$
```

20210811：

wget下载到指定目录并指定文件名：-O

```
#下载cmd.log 到本地local_cmd.log
wget --no-check-certificate  https://s3.amazonaws.com/skybell-device-logs-public/cmd.log -O local_cmd.log
```

VMware虚拟机中安装Ubuntu 12.04.5 LTS图解_Lemonsany_的博客-CSDN博客 - https://blog.csdn.net/Lemonsany_/article/details/106826678

---

2021-05-13：

Ubuntu14.04. 升级cmake 2.8 => 3.16

```
sudo apt remove cmake
cd /tmp
wget https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0.tar.gz
tar -zxvf cmake-3.16.0.tar.gz
cd cmake-3.16.0
./configure
make
```

执行make 出现以下错误：

```
In file included from /usr/include/openssl/ssl.h:18:0,
                 from /tmp/cmake-3.16.0/Utilities/cmcurl/lib/curl_ntlm_core.h:41,
                 from /tmp/cmake-3.16.0/Utilities/cmcurl/lib/http_ntlm.c:40:
/usr/include/openssl/bio.h:686:1: error: old-style parameter declarations in prototyped function definition
 DEPRECATEDIN_1_1_0(struct hostent *BIO_gethostbyname(const char *name))
 ^
/tmp/cmake-3.16.0/Utilities/cmcurl/lib/http_ntlm.c:257:1: error: expected ‘{’ at end of input
 }
 ^
make[2]: *** [Utilities/cmcurl/lib/CMakeFiles/cmcurl.dir/http_ntlm.c.o] Error 1
make[1]: *** [Utilities/cmcurl/lib/CMakeFiles/cmcurl.dir/all] Error 2
make: *** [all] Error 2
```



```
#解决方法
sudo apt-get remove libssl-dev
sudo apt-get install libssl-dev

#然后再重新编译安装
make
sudo make install

#可能需要：添加PATH环境变量 /usr/local/bin
#验证
cmake --version
cmake version 3.16.0

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```



------

2021-05-11

```
young@young-vm:~/AWS_WebRTC/20200227/amazon-kinesis-video-streams-webrtc-sdk-c$ sudo apt install tree
[sudo] young 的密码： 
E: 无法获得锁 /var/lib/dpkg/lock-frontend - open (11: 资源暂时不可用)
E: 无法获取 dpkg 前端锁 (/var/lib/dpkg/lock-frontend)，是否有其他进程正占用它？
young@young-vm:~/AWS_WebRTC/20200227/amazon-kinesis-video-streams-webrtc-sdk-c$ sudo rm -r -f   /var/lib/dpkg/lock-frontend
young@young-vm:~/AWS_WebRTC/20200227/amazon-kinesis-video-streams-webrtc-sdk-c$ sudo apt install tree
E: 无法获得锁 /var/lib/dpkg/lock - open (11: 资源暂时不可用)
E: 无法锁定管理目录(/var/lib/dpkg/)，是否有其他进程正占用它？
young@young-vm:~/AWS_WebRTC/20200227/amazon-kinesis-video-streams-webrtc-sdk-c$ sudo rm /var/lib/dpkg/lock
young@young-vm:~/AWS_WebRTC/20200227/amazon-kinesis-video-streams-webrtc-sdk-c$ sudo apt install tree
正在读取软件包列表... 完成
正在分析软件包的依赖关系树       
正在读取状态信息... 完成       
下列软件包是自动安装的并且现在不需要了：
.....
```

解决方法：

```
sudo rm -r -f   /var/lib/dpkg/lock-frontend
 sudo rm /var/lib/dpkg/lock
```

---

---

安装完Ubuntu 必做的事：

```
sudo apt install tree
sudo apt install git
sudo apt install gitk
sudo apt install vim
sudo apt install dos2unix
```

