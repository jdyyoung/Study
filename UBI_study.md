UBI（拉丁语的意思：哪？）代表UnsortedBlock Images(未分类块镜像)




```shell
#!/bin/sh
INPUT_DIR=skybell-v5app-sdk-v2.3.6
UBI_IMG=skyv5app-ubifs-none.img
CFG_FILE=ubinize.cfg

TODAY=`date +"%Y%m%d"`
OUTPUT_IMG=${INPUT_DIR}-${TODAY}.img; 

echo "Input dir: $INPUT_DIR"
echo "Onput image file: $OUTPUT_IMG"

echo "Start make UBIFS image..."
#mkfs.ubifs -x lzo -m 2048 -e 129024 -c 300 -r ./app/ -o app-ubifs.img
#mkfs.ubifs -x none -m 2048 -e 124KiB -c 512 -r ./skyv5app-20200107/ -o skyv5app-ubifs-none.img
mkfs.ubifs -x lzo -m 2048 -e 124KiB -c 512 -r ${INPUT_DIR} -o ${UBI_IMG}

if [ $? -ne 0 ]; then
    echo "[ERROR] make UBIFS image file"
    exit 0
fi
echo "Generate UBIFS image[${UBI_IMG}] [OK]..."

sleep 2;

echo "Start make ubinize image..."
#ubinize -o skyv5_ubi.img -m 2048 -p 128KiB -s 512 -O 2048 ubinize.cfg
ubinize -o ${OUTPUT_IMG} -m 2048 -p 128KiB -s 512 -O 2048 ${CFG_FILE}
#echo "Generate ubinize image [OK]..."
if [ $? -ne 0 ]; then
    echo "[ERROR] make ubinize image file"
    exit 0
fi
echo "Make ubinize image file [${OUTPUT_IMG}] [OK]!"

```

```
[ubifs]
mode=ubi
image=skyv5app-ubifs-none.img // mkfs.ubi生成的源镜像
vol_id=0                      //卷序号
vol_size=47MiB                //卷大小
vol_type=dynamic              //动态卷
vol_name=ubi0_0               //卷名 
vol_flags=autoresize
```

制作ubi文件系统脚本makeubifsimg.sh和相关的配置文件ubinize.cfg

用mkfs.ubifs和ubinize两个工具制作UBIFS镜像，具体步骤如下：

1.使用mkfs.ubifs命令将某个文件夹制作为UBIFS镜像，通过此命令制作的出的UBIFS文件系统镜像可在u-boot下使用ubi write命令烧写到NAND FLASH上

mkfs.ubifs制作镜像命令为：

```shell
mkfs.ubifs -x lzo -m 2048 -e 124KiB -c 512 -r ${INPUT_DIR} -o ${UBI_IMG}
```

​	-x lzo :指定压缩类型lzo

​	//flash datasheet，1页=2KiB = 2048Byte,1block=64page= 128KiB =131072 Byte = 0x20000 Byte

​	-m 2048:最小输入输出单元大小为2KiB(2048bytes)，一般为页大小 

​	-e 124KiB：逻辑可擦除块大小为124KiB=(每块的页数-2)\*页大小=（64-2）\* 2KiB=124KiB

​	-c最多逻辑可擦除块数目为512(512*128KiB=64MiB),这个可根据ubi volume来设置，实际上是设置此卷的最大容量

​	-r :指定把哪个路径下的文件用来构建文件系统

​	-o:输出的文件名

2. 使用ubinize命令可将使用mkfs.ubifs命令制作的UBIFS文件系统镜像转换成可直接在FLASH上烧写的格式（带有UBI文件系统镜像卷标）：

```shell
ubinize -o ${OUTPUT_IMG} -m 2048 -p 128KiB -s 512 -O 2048 ${CFG_FILE}
```

-m:最小输入输出大小为2KiB(2048bytes)，一般为页大小 
-p:物理可擦出块大小为128KiB=每块的页数\*页大小 =  64\*2KiB = 128KiB 
-s:用于UBI头部信息的最小输入输出单元，一般与最小输入输出单元(-m参数)大小一样。 

-O:VID头部(UBI专业名词)偏移量,默认是512 

通过此命令生成的ubi.img可直接使用NAND FLASH的烧写命令烧写到FLASH上。

既然有了mkfs.ubifs,为什么又出现了ubinize这个工具，这是笔者当时的思考,后来发现，经ubinize处理过的镜像文件烧写到flash中后,ubi工具才能正确处理这些存储在flash中的数据。

---

mkfs.ubifs的用法 
Usage: mkfs.ubifs [OPTIONS] target 
Make a UBIFS file system image from an existing directory tree 
Examples: 
Build file system from directory /opt/img, writting the result in the ubifs.img file 
​        mkfs.ubifs -m 512 -e 128KiB -c 100 -r /opt/img ubifs.img 
The same, but writting directly to an UBI volume 
​        mkfs.ubifs -r /opt/img /dev/ubi0_0 
Creating an empty UBIFS filesystem on an UBI volume 
​        mkfs.ubifs /dev/ubi0_0 
Options: 
-r, -d, --root=DIR               build file system from directory DIR 
-m, --min-io-size=SIZE       minimum I/O unit size，最小输入输出大小 
-e, --leb-size=SIZE        logical erase block size逻辑可擦出块大小 
-c, --max-leb-cnt=COUNT    maximum logical erase block count最大逻辑可擦出块数目 
-o, --output=FILE        output to FILE输出文件名 
-j, --jrn-size=SIZE      journal size 
-R, --reserved=SIZE      how much space should be reserved for the super-user 
-x, --compr=TYPE        compression type - "lzo", "favor_lzo", "zlib" or 
​                                  "none" (default: "lzo") 
-X, --favor-percent       may only be used with favor LZO compression and defines 
​                                how many percent better zlib should compress to make 
​                                mkfs.ubifs use zlib instead of LZO (default 20%) 
-f, --fanout=NUM         fanout NUM (default: 8) 
-F, --space-fixup         file-system free space has to be fixed up on first moun
​                          (requires kernel version 3.0 or greater) 
-k, --keyhash=TYPE       key hash type - "r5" or "test" (default: "r5") 
-p, --orph-lebs=COUNT     count of erase blocks for orphans (default: 1) 
-D, --devtable=FILE       use device table FILE 
-U, --squash-uids         squash owners making all files owned by root 
-l, --log-lebs=COUNT     count of erase blocks for the log (used only for debugging) 
-v, --verbose             verbose operation 
-V, --version             display version information 
-g, --debug=LEVEL         display debug information (0 - none, 1 - statistics, 2 - files, 3 - more details) 
-h, --help               display this help text 
例： 
mkfs.ubifs -x lzo -m 2KiB -e 124KiB -c 720 -o system_ubifs.img -d $path_to_system 
压缩格式为lzo 
-m最小输入输出大小为2KiB(2048bytes)，一般为页大小 
-e逻辑可擦除块大小为124KiB=(每块的页数-2)*页大小=（64-2）*2KiB=124KiB 
-c最多逻辑可擦除块数目为720(720*128KiB=90MiB),这个可根据ubi volume来设置，实际上是设置此卷的最大容量。 

---

## ubinize的用法

Usage: ubinize [-o filename] [-p <bytes>] [-m <bytes>] [-s <bytes>] [-O <num>] [-e 
<num>][-x  <num>]  [-Q  <num>]  [-v]  [-h]  [-V]  [--output=<filename>] 
[--peb-size=<bytes>]  [--min-io-size=<bytes>]  [--sub-page-size=<bytes>] 
[--vid-hdr-offset=<num>]  [--erase-counter=<num>]  [--ubi-ver=<num>] 
[--image-seq=<num>] [--verbose] [--help] [--version] ini-file 
Example: ubinize -o ubi.img -p 16KiB -m 512 -s 256 cfg.ini - create UBI image 
​         'ubi.img' as described by configuration file 'cfg.ini' 

-o, --output=<file name>     output file name 
-p, --peb-size=<bytes>       size of the physical eraseblock of the flash 
​                             this UBI image is created for in bytes, 
​                              kilobytes (KiB), or megabytes (MiB) 
​                              (mandatory parameter)物理可擦出块大小 
-m, --min-io-size=<bytes>    minimum input/output unit size of the flash 
​                             in bytes 
-s, --sub-page-size=<bytes>  minimum input/output unit used for UBI 
​                             headers, e.g. sub-page size in case of NAND 
​                             flash (equivalent to the minimum input/output 
​                             unit size by default)子页大小 

-O, --vid-hdr-offset=<num>   offset if the VID header from start of the 
​                              physical eraseblock (default is the next 
​                             minimum I/O unit or sub-page after the EC 
​                             header)VID头部偏移量,默认是512 
-e, --erase-counter=<num>    the erase counter value to put to EC headers (default is 0) 
-x, --ubi-ver=<num>           UBI version number to put to EC headers  (default is 1) 
-Q, --image-seq=<num>        32-bit UBI image sequence number to use 
​                             (by default a random number is picked) 
-v, --verbose                 be verbose 
-h, --help                   print help message 
-V, --version                 print program version 

例： 
**ubinize –o ubi.img -m 2KiB -p 128KiB -s 2048 $system_cfg_file –v** 
-m最小输入输出大小为2KiB(2048bytes)，一般为页大小 
-p物理可擦出块大小为128KiB=每块的页数*页大小=64*2KiB=128KiB 
-s用于UBI头部信息的最小输入输出单元，一般与最小输入输出单元(-m参数)大小一样。 
ubinize需要指定一个配置文件$system_cfg_file，内容如下： 
[rootfs-volume]  
mode=ubi  
image=system_ubifs.img  
vol_id=0  
vol_size=90MiB  
vol_type=dynamic 
vol_name=system 



UBI文件系统_钱塘六和的博客-CSDN博客 - https://blog.csdn.net/bang417/article/details/50833583

UBIFS文件系统_yedushu的博客-CSDN博客 - https://blog.csdn.net/yedushu/article/details/79767099

lzo压缩格式文件查看_园荐_博客园 - https://recomm.cnblogs.com/blogpost/6074314

---

Ubifs文件系统和mkfs.ubifs以及ubinize工具的用法_wjjontheway的专栏-CSDN博客 - https://blog.csdn.net/wjjontheway/article/details/8977871

利用mkfs.ubifs和ubinize两个工具制作UBI镜像 - hi!画家 - 博客园 - https://www.cnblogs.com/xiansong1005/p/7210795.html

ubinize的用法 - Jello - 博客园 - https://www.cnblogs.com/dakewei/p/10339414.html

//VID头部

UBI系统原理-中-alloysystem-ChinaUnix博客 - http://blog.chinaunix.net/uid-28236237-id-4217118.html

ubifs常见命令 - 断网用户 - 博客园 - https://www.cnblogs.com/under/p/10578533.html