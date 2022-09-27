



```
问题一：ifconfig之后只显示lo,没有看到eth0 ?
eth0设置不正确，导致无法正常启动，修改eth0配置文件就好 
ubuntu 12.04的网络设置文件是/etc/network/interfaces，打开文件，会看到 
auto lo 
iface lo inet loopback 
这边的设置是本地回路。在后面加上 
auto eth0 
iface eth0 inet static 
address 192.168.1.230 //（ip地址） 
netmask 255.255.255.0 //（子网掩码） 
gateway 192.168.1.1 //（网关） 
其中eth0就是电脑的网卡，如果电脑有多块网卡，比如还会有eth1，都可以在这里进行设置。iface eth0 inet 设置为dhcp是动态获取IP，设置为static则用自定义的IP。这边要自定义IP地址，所以选择static选项。

ubuntu ifconfig 不显示IP地址 - 走看看 - http://t.zoukankan.com/Jesse-Li-p-8944669.html

auto eth0 
iface eth0 inet dhcp
```



```
unzip把压缩文件解压到指定目录下 -d
例如：
unzip xxxx.zip  -d xxxxx2目录
unzip -o 是解压时覆盖文件
```




```
zip 压缩方法：
 
压缩当前的文件夹 zip -r ./xahot.zip ./* -r表示递归
zip [参数] [打包后的文件名] [打包的目录路径]
解压 unzip xahot.zip 不解释
Linux下压缩某个文件夹命令 - Alan·Jones - 博客园 - https://www.cnblogs.com/Alan-Jones/p/7488179.html
```

2021-0516：

使用linux 中的 tree 指定显示深度，显示文件目录结构：

```
#显示深度为3级
tree -L 3
#只显示目录，不显示文件
tree -d -L 3
```



---

```
#使用patch指令将文件"testfile1"升级，其升级补丁文件为"testfile.patch"，输入如下命令：
patch -p0 testfile1 testfile.patch    #使用补丁程序升级文件 
```

---

安装samba在设置环境时报的错：

```
lindenis@ubuntu:~$ sudo system-config-samba

(system-config-samba:21365): IBUS-WARNING **: The owner of /home/lindenis/.config/ibus/bus is not root!
Traceback (most recent call last):
  File "/usr/sbin/system-config-samba", line 45, in <module>
    mainWindow.MainWindow(debug_flag)
  File "/usr/share/system-config-samba/mainWindow.py", line 116, in __init__
    self.samba_user_data = sambaUserData.SambaUserData(self)
  File "/usr/share/system-config-samba/sambaUserData.py", line 46, in __init__
    self.readSmbPasswords()
  File "/usr/share/system-config-samba/sambaUserData.py", line 63, in readSmbPasswords
    raise RuntimeError, (_("You do not have permission to execute %s." % pdbeditcmd))
RuntimeError: You do not have permission to execute /usr/bin/pdbedit.
lindenis@ubuntu:~$ 
```

解决方法：

缺少/etc/samba/smb.conf，网上下载一个或者拷贝现成已有的配置文件过去就可以。以下是其中自己的smb.conf 。

```
#
# Sample configuration file for the Samba suite for Debian GNU/Linux.
#
#
# This is the main Samba configuration file. You should read the
# smb.conf(5) manual page in order to understand the options listed
# here. Samba has a huge number of configurable options most of which 
# are not shown in this example
#
# Some options that are often worth tuning have been included as
# commented-out examples in this file.
#  - When such options are commented with ";", the proposed setting
#    differs from the default Samba behaviour
#  - When commented with "#", the proposed setting is the default
#    behaviour of Samba but the option is considered important
#    enough to be mentioned here
#
# NOTE: Whenever you modify this file you should run the command
# "testparm" to check that you have not made any basic syntactic 
# errors. 

#======================= Global Settings =======================

[global]

## Browsing/Identification ###

# Change this to the workgroup/NT-domain name your Samba server will part of
	workgroup = WORKGROUP

# server string is the equivalent of the NT Description field
	server string = %h server (Samba, Ubuntu)

# Windows Internet Name Serving Support Section:
# WINS Support - Tells the NMBD component of Samba to enable its WINS Server
#   wins support = no

# WINS Server - Tells the NMBD components of Samba to be a WINS Client
# Note: Samba can be either a WINS Server, or a WINS Client, but NOT both
;   wins server = w.x.y.z

# This will prevent nmbd to search for NetBIOS names through DNS.
	dns proxy = no

#### Networking ####

# The specific set of interfaces / networks to bind to
# This can be either the interface name or an IP address/netmask;
# interface names are normally preferred
;   interfaces = 127.0.0.0/8 eth0

# Only bind to the named interfaces and/or networks; you must use the
# 'interfaces' option above to use this.
# It is recommended that you enable this feature if your Samba machine is
# not protected by a firewall or is a firewall itself.  However, this
# option cannot handle dynamic or non-broadcast interfaces correctly.
;   bind interfaces only = yes



#### Debugging/Accounting ####

# This tells Samba to use a separate log file for each machine
# that connects
	log file = /var/log/samba/log.%m

# Cap the size of the individual log files (in KiB).
	max log size = 1000

# If you want Samba to only log through syslog then set the following
# parameter to 'yes'.
#   syslog only = no

# We want Samba to log a minimum amount of information to syslog. Everything
# should go to /var/log/samba/log.{smbd,nmbd} instead. If you want to log
# through syslog you should set the following parameter to something higher.
	syslog = 0

# Do something sensible when Samba crashes: mail the admin a backtrace
	panic action = /usr/share/samba/panic-action %d


####### Authentication #######

# Server role. Defines in which mode Samba will operate. Possible
# values are "standalone server", "member server", "classic primary
# domain controller", "classic backup domain controller", "active
# directory domain controller". 
#
# Most people will want "standalone sever" or "member server".
# Running as "active directory domain controller" will require first
# running "samba-tool domain provision" to wipe databases and create a
# new domain.
	server role = standalone server

# If you are using encrypted passwords, Samba will need to know what
# password database type you are using.  
;	passdb backend = tdbsam

	obey pam restrictions = yes

# This boolean parameter controls whether Samba attempts to sync the Unix
# password with the SMB password when the encrypted SMB password in the
# passdb is changed.
	unix password sync = yes

# For Unix password sync to work on a Debian GNU/Linux system, the following
# parameters must be set (thanks to Ian Kahan <<kahan@informatik.tu-muenchen.de> for
# sending the correct chat script for the passwd program in Debian Sarge).
	passwd program = /usr/bin/passwd %u
	passwd chat = *Enter\snew\s*\spassword:* %n\n *Retype\snew\s*\spassword:* %n\n *password\supdated\ssuccessfully* .

# This boolean controls whether PAM will be used for password changes
# when requested by an SMB client instead of the program listed in
# 'passwd program'. The default is 'no'.
	pam password change = yes

# This option controls how unsuccessful authentication attempts are mapped
# to anonymous connections
	map to guest = bad user

########## Domains ###########

#
# The following settings only takes effect if 'server role = primary
# classic domain controller', 'server role = backup domain controller'
# or 'domain logons' is set 
#

# It specifies the location of the user's
# profile directory from the client point of view) The following
# required a [profiles] share to be setup on the samba server (see
# below)
;   logon path = \\%N\profiles\%U
# Another common choice is storing the profile in the user's home directory
# (this is Samba's default)
#   logon path = \\%N\%U\profile

# The following setting only takes effect if 'domain logons' is set
# It specifies the location of a user's home directory (from the client
# point of view)
;   logon drive = H:
#   logon home = \\%N\%U

# The following setting only takes effect if 'domain logons' is set
# It specifies the script to run during logon. The script must be stored
# in the [netlogon] share
# NOTE: Must be store in 'DOS' file format convention
;   logon script = logon.cmd

# This allows Unix users to be created on the domain controller via the SAMR
# RPC pipe.  The example command creates a user account with a disabled Unix
# password; please adapt to your needs
; add user script = /usr/sbin/adduser --quiet --disabled-password --gecos "" %u

# This allows machine accounts to be created on the domain controller via the 
# SAMR RPC pipe.  
# The following assumes a "machines" group exists on the system
; add machine script  = /usr/sbin/useradd -g machines -c "%u machine account" -d /var/lib/samba -s /bin/false %u

# This allows Unix groups to be created on the domain controller via the SAMR
# RPC pipe.  
; add group script = /usr/sbin/addgroup --force-badname %g

############ Misc ############

# Using the following line enables you to customise your configuration
# on a per machine basis. The %m gets replaced with the netbios name
# of the machine that is connecting
;   include = /home/samba/etc/smb.conf.%m

# Some defaults for winbind (make sure you're not using the ranges
# for something else.)
;   idmap uid = 10000-20000
;   idmap gid = 10000-20000
;   template shell = /bin/bash

# Setup usershare options to enable non-root users to share folders
# with the net usershare command.

# Maximum number of usershare. 0 (default) means that usershare is disabled.
;	usershare max shares = 100

# Allow users who've been granted usershare privileges to create
# public shares, not just authenticated ones
	usershare allow guests = yes

#======================= Share Definitions =======================

# Un-comment the following (and tweak the other settings below to suit)
# to enable the default home directory shares. This will share each
# user's home directory as \\server\username
;[homes]
;   comment = Home Directories
;   browseable = no

# By default, the home directories are exported read-only. Change the
# next parameter to 'no' if you want to be able to write to them.
;   read only = yes

# File creation mask is set to 0700 for security reasons. If you want to
# create files with group=rw permissions, set next parameter to 0775.
;   create mask = 0700

# Directory creation mask is set to 0700 for security reasons. If you want to
# create dirs. with group=rw permissions, set next parameter to 0775.
;   directory mask = 0700

# By default, \\server\username shares can be connected to by anyone
# with access to the samba server.
# Un-comment the following parameter to make sure that only "username"
# can connect to \\server\username
# This might need tweaking when using external authentication schemes
;   valid users = %S

# Un-comment the following and create the netlogon directory for Domain Logons
# (you need to configure Samba to act as a domain controller too.)
;[netlogon]
;   comment = Network Logon Service
;   path = /home/samba/netlogon
;   guest ok = yes
;   read only = yes

# Un-comment the following and create the profiles directory to store
# users profiles (see the "logon path" option above)
# (you need to configure Samba to act as a domain controller too.)
# The path below should be writable by all users so that their
# profile directory may be created the first time they log on
;[profiles]
;   comment = Users profiles
;   path = /home/samba/profiles
;   guest ok = no
;   browseable = no
;   create mask = 0600
;   directory mask = 0700

[printers]
	comment = All Printers
	browseable = no
	path = /var/spool/samba
	printable = yes
;	guest ok = no
;	read only = yes
	create mask = 0700

# Windows clients look for this share name as a source of downloadable
# printer drivers
[print$]
	comment = Printer Drivers
	path = /var/lib/samba/printers
;	browseable = yes
;	read only = yes
;	guest ok = no
# Uncomment to allow remote administration of Windows print drivers.
# You may need to replace 'lpadmin' with the name of the group your
# admin users are members of.
# Please note that you also need to set appropriate Unix permissions
# to the drivers directory for these users to have write rights in it
;   write list = root, @lpadmin

[v536]
	comment = rck_samba
	path = /home/lindenis
	writeable = yes
;	browseable = yes
	valid users = lindenis

```

启示链接：Bug #964681 “system-config-samba.py crashed with RuntimeError in...” : Bugs : system-config-samba package : Ubuntu - https://bugs.launchpad.net/ubuntu/+source/system-config-samba/+bug/964681/





---

在Linux Shell脚本中使用jq工具解析json数据_杨-CSDN博客 - https://blog.csdn.net/qq_36956154/article/details/103874998

命令行 JSON 处理工具 jq 的使用介绍_Linux编程_Linux公社-Linux系统门户网站 - https://www.linuxidc.com/Linux/2017-10/148037.htm

```
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$ cat /tmp/get_device_info.log | jq .
{
  "data": {
    "activateStatus": 1,
    "otaStatus": 0,
    "heartbeatTime": 300,
    "timeZone": "UTC+08:00",
    "internalIp": "192.168.98.105",
    "modelName": "Doorbell",
    "cameraImage": "http://ecosystem.redbeecloud.com/RedbeeBackend/v1/file/download/camera/59537f2fcecd46dda5bbf4173aa979fc.jpg?time=1635321828357",
    "cameraOnline": true,
    "region": "us-east-1",
    "kvsStreamArn": "arn:aws:kinesisvideo:us-east-1:216765166536:stream/1200000008021401/1633680931818",
    "emailNotification": false,
    "pushNotification": false,
    "cameraPassword": "5vmbrD2AFHZxomKC",
    "cameraUsername": "admin",
    "cameraMac": "c0:e7:bf:83:c8:42",
    "cameraUid": "6JK623EXZ48BFJ8T111A",
    "cameraSn": "1200000008021401",
    "imei": "166f0640f5ac",
    "userId": "RB1628040109965",
    "deviceId": "59537f2fcecd46dda5bbf4173aa979fc",
    "cameraName": "New 080",
    "cameraType": 3,
    "hwVersion": "V06",
    "fwVersion": "V1035",
    "mcuVersion": "0.23",
    "wifiSsid": "Matrix",
    "wifiPwd": "Rd123789",
    "onlineUpdateTime": "Oct 27, 2021 8:18:26 AM"
  },
  "error": "",
  "result": true
}
```



---

2021-10-21:

```
在linux中使用matlab的时候，常常用ctrl+z将matlab挂起，一开始并不知道怎么处理，也关不掉
后来发现用fg再回车就可以将后台挂起程序切换的前台来。
```

```
vim撤消和取消撤消
撤销：u
取消撤销：Ctrl+r
```

```
rck@rck:/vtcs/nn_test$ repo
The program 'repo' is currently not installed. You can install it by typing:
sudo apt-get install phablet-tools
rck@rck:/vtcs/nn_test$
```



----

Ubuntu中使用motion制作监控_weixin_44511361的博客-CSDN博客 - https://blog.csdn.net/weixin_44511361/article/details/103211798

motion

虚拟机Linux如何使用笔记本电脑的前置摄像头_fendoubasaonian的专栏-CSDN博客_虚拟机调用笔记本摄像头 - https://blog.csdn.net/fendoubasaonian/article/details/50180025

cheese

---

20210901：

rsync:两台不同虚拟机同步内容：

Linux rsync 命令学习 - Michael翔 - 博客园 - https://www.cnblogs.com/michael-xiang/p/10466887.html

rsync配置两台服务器之间的文件备份(同步)_qinshengfei的专栏-CSDN博客 - https://blog.csdn.net/qinshengfei/article/details/103773438?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_title~default-0.control&spm=1001.2101.3001.4242

rsync 服务端安装完成之后是没有生成rsync.conf文件的，需要手动创建rsyncd.conf

```
vim /etc/rsyncd.conf
//----------------------------------------------------------------
#先定义整体变量
secrets file = /etc/rsyncd.secrets          #配置同步用户名和密码
motd file = /etc/rsyncd.motd
read only = yes
list = yes
#uid = nobody
#gid = nobody
uid = root
gid = root
hosts allow = *    #哪些电脑可以访问rsync服务
hosts deny = 0.0.0.0/32    #哪些电脑不可以访问rsync服务
max connections = 2
log file = /var/log/rsyncd.log
pid file = /var/run/rsyncd.pid
lock file = /var/run/rsync.lock
　　
#再定义要rsync的目录
[backup]
path = /data/app/files
list=yes
ignore errors
auth users = root
comment = welcome
exclude = file1/  file2/ 

```

测试：

```
rsync  -avlK ./customization/ rck@192.168.37.152:/vtcs/0002_iCatch/svn2git/vi37/iCatOS/sphost/customization
```



```
rsync -arzvtopg --delete rck@192.168.37.144:/vtcs/0002_iCatch/svn2git/vi37/iCatOS ~/iCatOS
```

---

2022-06-22

rsync免密的设置：ssh免密，rsync也就免密！

添加公钥给客户端的.ssh下

```
[azuo1228@dest-server ~]$ mkdir .ssh
[azuo1228@dest-server ~]$ cd .ssh/
[azuo1228@dest-server .ssh]$ cat ../id_rsa.pub | tee -a authorized_keys
```

Rsync设置免输入密码定期同步文件文件夹 - 多客博图网 - http://www.doocr.com/articles/58c3aa9d827a1a6753add53b

SSH配置key免密码登录 - 多客博图网 - http://www.doocr.com/articles/58c3a904827a1a6753add53a

---

安装scp,ssh服务：

```
sudo apt-get install openssh-server
```

验证安装成功：

```
rck@rck:/vtcs/d3tech/skybell_m5s_app$ sshd -v
unknown option -- v
OpenSSH_6.6.1p1 Ubuntu-2ubuntu2.13, OpenSSL 1.0.1f 6 Jan 2014
usage: sshd [-46DdeiqTt] [-b bits] [-C connection_spec] [-c host_cert_file]
            [-E log_file] [-f config_file] [-g login_grace_time]
            [-h host_key_file] [-k key_gen_time] [-o option] [-p port]
            [-u len]
```

Linux scp命令 | 菜鸟教程 - https://www.runoob.com/linux/linux-comm-scp.html

----

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

