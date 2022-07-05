#  Linux内核驱动学习

DAY1：

```c
  #include <linux/init.h>
  #include <linux/module.h>

  static int helloworld_init(void)
  {
  	printk("hello,world\n");
  	return 0; //执行成功返回0，执行失败返回负值
  }
  static void helloworld_exit(void)
  {
  	printk("good bye!\n");
  }
  module_init(helloworld_init);
  module_exit(helloworld_exit);
  MODULE_LICENSE("GPL");
```

```
内核程序特点：
  1.使用的头文件都位于内核源码中
  2.使用调用的函数的定义都位于内核源码中
  3.module_init修饰的函数helloworld_init就是内核程序的入口函数，类似main函数,入口函数的返回值为int,参数为void
  4.module_exit修饰的函数helloworld_exit就是内核程序的出口函数，类似return 0;返回值，形参都是void
  5.任何一个内核程序(.c),都必须添加GPL许可声明，必须添加：
    MODULE_LICENSE("GPL");
  6.内核程序的入口函数执行完毕，并不代表内核程序结束，恰恰相反，内核程序而是刚刚开始，仅仅是存在于内存中，供应用程序来访问使用；
  7.内核程序的出口函数执行完毕，这个内核程序将会从内存中剔除
```

---

```makefile
obj-m += helloworld.o
#说明：将helloworld.c最终编译成对应的二进制可执行文件helloworld.ko
all: 
 	make -C /opt/kernel SUBDIRS=$(PWD) modules
clean:
    make -C /opt/kernel SUBDIRS=$(PWD) clean
```

```
make -C /opt/kernel:到/opt/kernel目录下执行make命令
SUBDIRS=$(PWD):$(PWD)代表helloworld.c所在的路径，把路径信息给SUBDIRS(表示内核的一个子目录)
modules：告诉内核，对于helloworld.c所在目录下的文件采用模块编译，就是将.c->.ko

Makefile学习:$()
```

```
insmod
lsmod
rmmod

关于驱动还有另外命令：
modprobe
```

-----

---

DAY2:

```
linux内核命令行传递参数的方法：
  1. 明确内核程序的哪些全局变量将来需要传递参数
  2. 要利用一下宏对这个变量进行传参声明，告诉内核将来这个变量可以通过命令行传递参数
     module_param(name, type, perm);
     @name:变量名
     @type:变量的数据类型
     	   bool,invbool
     	   char,uchar
     	   short,ushort
     	   int,uint
     	   long,ulong
     	   charp(字符指针char *)
     	   切记：linux内核程序不允许处理浮点数！
     	   如果需要处理浮点数，方法：
     	   1.放大倍数：2.3*3.2=>23*32,应用程序再除以100倍
     	   2.干脆直接让应用程序来处理！
     @perm：变量的访问权限(rwx)，不能有可执行权限，
    	    例如0664
    	    特殊情况：权限可以指定为0,在/sys/module/helloworld/parameters/不会有同名的文件
     	    如果权限不为0，在/sys/module/helloworld/parameters/会有同名的文件，可以通过修改文件来间接给内核变量传递的参数
          	    
```

```
//不传递参数
     1.insmod helloworld.ko
     2.rmmod helloworld
     
     //加载模块时传递参数
     3.insmod helloworld.ko irq=100 pstring=china
     4.rmmod helloworld
     
     //模块运行期间传递参数
     5.insmod helloworld.ko irq=100 pstring=china
     6.cd /sys/module/helloworld/parameters/
     7.cat irq //读文件
     8.echo 20000 > irq //写文件
     9.rmmod helloworld
```

```
3.对于数组进行命令行的传参声明：
       module_param_array(name,type,nump,perm);
       name：数组名
       type:数组元素的数据类型
       nump:有效的数组元素的个数指针
       perm:访问权限
       
       
 //不传递参数
     1.insmod helloworld.ko
     2.rmmod helloworld
     
     //加载模块时传递参数
     3.insmod helloworld.ko fish=1,2,3,4,5
     4.rmmod helloworld
     
     //模块运行期间传递参数
     5.insmod helloworld.ko fish=1,2,3,3,4
     6.cd /sys/module/helloworld/parameters/
     7.cat fish //读文件
     8.echo 20000,30000,100,200 > fish //写文件
     9.rmmod helloworld
```



----

```
5.linux内核符号导出
  linux内核符号导出就是将内核的全局变量和函数导出让其他模块访问使用  //jdy0909:内核的跨文件调用
  导出变量或者函数的方法：
  EXPORT_SYMBOL(变量名或者函数名)
  或者
  EXPORT_SYMBOL_GPL(变量名或者函数名)
  前者导出的变量或者函数，所有内核程序都能够访问使用；
  后者导出的变量或者函数，只能给那些遵循GPL协议的内核程序访问使用（MODULE_LICENSE("GPL")
```

```
6.linux内核打印函数printk
  特点：
  1.只能运行在内核中，不能用于应用程序 
  2.能够指定打印级别，打印级别有8级(0~7)
    #define	KERN_EMERG	"<0>"
    ...
    #define	KERN_DEBUG	"<7>"
    数字越小，代表问题越严重
  3.用法：
    printk(KERN_DEBUG "this is a debug message!\n");
    或者：
    printk("<7>" "this is a debug message!\n");
  4.设置默认的打印输出级别(类似水位的告警阀值)，例如：
    int helloworld_init(void) {
    	printk("<7>" "loglevel 7!\n");
    	printk("<3>" "loglevel 3!\n");
    	printk("<6>" "loglevel 6!\n");
    	printk("<1>" "loglevel 1!\n");
    }
    问：只想在终端看3和1,不想看6和7
    答：通过设置默认的打印输出级别，即可完成信息的输出和不输出，如果printk指定的打印输出级别小于默认的打印输出级别，这类信息需要进行输出，否则不输出！
    问：如何设置默认的打印输出级别呢？
    方法1：修改配置文件
    	   cat /proc/sys/kernel/printk
    	   7(虚拟终端)       4       1       7
    	   echo 8 > /proc/sys/kernel/printk 
    方法2：
    	方法1对于内核启动时的信息无法进行配置！
    	方法2采用在内核启动的参数中设置默认的打印输出级别
```

```
案例：编写内核程序，使用printk并且指定打印输出级别
    ARM板上执行：
    1.insmod helloworld.ko
    2.rmmod helloworld
    3.cat /proc/sys/kernel/printk
    4.echo 8 > /proc/sys/kernel/printk //方法1
    
    //采用方法2设置默认的打印输出级别
    5.重启开发板，进入uboot模式
    6.设置内核的启动参数：
      setenv bootargs root=/dev/nfs nfsroot=192.168.1.8:/opt/rootfs ip=192.168.1.110:192.168.1.8:192.168.1.1:255.255.255.0::eth0:on init=/linuxrc console=ttySAC0,115200 debug
      saveevne
      boot //启动系统
    7.insmod helloworld.ko
    8.rmmod helloworld
    9.cat /proc/sys/kernel/printk //查看debug指定的默认输出级别是多少(10)
    
   10.设置内核的启动参数：
      setenv bootargs root=/dev/nfs nfsroot=192.168.1.8:/opt/rootfs ip=192.168.1.110:192.168.1.8:192.168.1.1:255.255.255.0::eth0:on init=/linuxrc console=ttySAC0,115200 quiet
      boot //启动系统
   11.insmod helloworld.ko
   12.rmmod helloworld
   13.cat /proc/sys/kernel/printk //查看quiet指定的默认输出级别是多少(4)
   
   14.设置内核的启动参数：
      setenv bootargs root=/dev/nfs nfsroot=192.168.1.8:/opt/rootfs ip=192.168.1.110:192.168.1.8:192.168.1.1:255.255.255.0::eth0:on init=/linuxrc console=ttySAC0,115200 loglevel=数字
      boot //启动系统
      
   15.insmod ...
   16.rmmod ...
   
   debug
   quiet
   loglevel=数字
```

```
 内核通过printk() 输出的信息具有日志级别，日志级别是通过在printk() 输出的字符串前加一个带尖括号的整数来控制的，如printk("<6>Hello, world!\n");。内核中共提供了八种不同的日志级别，在 linux/kernel.h 中有相应的宏对应。

#define KERN_EMERG   "<0>"   /* systemis unusable */
#define KERN_ALERT   "<1>"   /* actionmust be taken immediately */
#define KERN_CRIT    "<2>"   /*critical conditions */
#define KERN_ERR     "<3>"   /* errorconditions */
#define KERN_WARNING "<4>"   /* warning conditions */
#define KERN_NOTICE  "<5>"   /* normalbut significant */
#define KERN_INFO    "<6>"   /*informational */
#define KERN_DEBUG   "<7>"   /*debug-level messages */
```

---

```
7.linux内核GPIO操作库函数
  linux内核GPIO操作库函数：
  1.int gpio_request(int gpio, char *name)
  函数功能：由于硬件GPIO对于内核来说是一种宝贵的资源，所以驱动如果要访问某个硬件GPIO，首先向内核申请硬件GPIO的资源，类似malloc
  参数说明：
  	gpio:硬件GPIO在linux内核中都有一个唯一的软件编号(类似身份证号)：
  	例如：
  	硬件GPIO,GPC0_3对应的软件编号为：S5PV210_GPC0(3);
  	硬件GPIO,GPF1_5对应的软件编号为：S5PV210_GPF1(5)
  	
  	name:GPIO的名称，随便指定！
  	内核函数的返回值无需记忆，只需看内核源码别人怎么判断，你就怎么判断！
  2.void gpio_free(int gpio)
    释放GPIO资源
    gpio:硬件GPIO对应的软件编号
    
  3.int gpio_direction_output(int gpio, int value)
    配置GPIO为输出口，同时输出value值(1或者0)
  
  4.int gpio_direction_input(int gpio);
    配置GPIO为输入口 
    
  5.int gpio_set_value(int gpio, int value);
    设置GPIO的输出状态value(1或者0)，此函数在使用时，GPIO首先应该配置为输出口  
    
  6.int gpio_get_value(int gpio)
    获取GPIO的状态，此时GPIO是输入口还是输出口无所谓！
    返回1表明为高电平；返回0表明为低电平
```

//jdy TODO:操作GPIO的方法的统计

---

```
8.linux内核系统调用
  8.1.C标准库函数和系统调用函数
      malloc,printf...库函数
      open,close,read...系统调用函数
  8.2.linux系统包括两部分：用户空间(用户态)和内核空间(内核态)
  8.3.用户空间(用户态)
      特点：
      1.包含的软件为标准的库和应用程序
      2.用户空间的软件在运行时，对应的CPU工作模式为USR模式
      3.用户空间的软件不能直接访问内核(zImage)的代码，数据和地址，但是可以通过系统调用间接访问
      4.用户空间的软件在运行时，如果对地址进行非法访问，会造成应用程序的崩溃，而不会造成操作系统的崩溃！
        例如：*(int *)0 = 0;
      5.用户空间的软件不能直接访问硬件资源(硬件的物理地址)，可以将硬件的物理地址映射到用户空间的虚拟地址上(mmap)
      6.4G虚拟地址，用户空间占前3G(0~0xBFFFFFFF)
      7.类似论坛的普通用户
      
   8.4.内核空间(内核态)
       特点：
	1.包含的软件有：
		网络协议栈
		设备驱动
		文件系统
		平台相关
		系统调用
		进程管理
		内存管理 
	2.内核空间的软件在运行时，对应的CPU的工作模式为SVC模式（Super Visor Call）
	3.内核空间的软件在运行时，如果对内存进行非法的访问，最终造成操作系统的崩溃,*(int *)0 = 0;
	4.内核空间的软件不能直接访问硬件资源(硬件的物理地址)，需要将硬件物理地址映射到内核空间的虚拟地址	  上
	5.4G虚拟地址，内核占后1G（0xC0000000~0xFFFFFFFF）
	6.类似论坛管理员
	
    8.5.用户空间和内核空间的交互只能通过系统调用
       	用户进程如果调用系统调用函数，实现进程由用户空间切换到内核空间运行，本质上就是CPU由USR模式切换到SVC模式；
       	总结：系统调用本质上利用软中断来实现！
       	
    8.6.系统调用实现的过程
    以write系统调用函数为例：
    1.应用程序调用write系统调用函数，首先调用到C库的write函数
    2.C库的write函数将会做两件事：
      2.1.保存write函数的系统调用号到R7寄存器
      系统调用号：linux内核给每一个系统调用函数指定一个软件编号(身份证号)，write的系统调用号__NR_write=4
      2.2.调用svc指令触发软中断异常，至此CPU的工作模式由USR切换到SVC
    3.一旦触发软中断异常，CPU毫无条件的跳转异常向量表的软中断处理入口(内核来实现)，至此应用程序由用户态切换	到内核态继续运行
    4.跳转到软中断的处理入口，将会做两件事：
      1.从R7取出系统调用号
      2.以系统调用号为索引在系统调用表中找到对应的内核函数sys_write
      3.调用内核函数sys_write,调用完毕，原路返回到用户态
      
      什么是SVC模式_送人玫瑰，手有余香。-CSDN博客 - https://blog.csdn.net/jobsss/article/details/7548550
      
      一、ARM体系的CPU有以下7种工作模式：

1、用户模式（usr）：正常的程序执行状态
2、快速中断模式（fiq）: 处理快速中断，支持高速数据传送或通道处理
3、中断模式（irq）: 处理普通中断
4、管理模式（svc）：操作系统使用的保护模式
5、系统模式（sys）：运行具有特权的操作系统任务
6、数据访问终止模式（abt）：数据或指令预取终止时进入该模式
7、未定义指令终止模式（und）：未定义的指令执行时进入该模式
```

---

---

DAY3:

```
6.linux设备驱动程序开发相关内容
  6.1.设备驱动的作用
      操作和管理硬件设备
      给用户提供访问操作硬件的接口
  6.2.linux内核设备驱动的分类
      A. 字符设备驱动
       特性：设备的访问采用字节流形式
       包含的设备：串口(蓝牙,GPS,GPRS,zigbee等),按键,鼠标,触摸屏,LCD,声卡,各种传感器
      B. 块设备驱动
       特性：设备的访问采用数据块形式，比如512byte,1K,4K
       包含的设备：硬盘,U盘,SD卡,TF卡,Nand,Nor,EMMC等
      C. 网络设备驱动
       特性：设备的访问结合TCP/IP协议栈
       包含的设备：有线和无线

7.linux内核字符设备驱动开发相关内容
  7.1.unix/linux编程理念："一切皆文件"
      "一切"：就是指计算机硬件
      "文件"：看到文件就等于看到硬件
      对文件的访问本质就是对硬件的访问
      利用系统调用函数对文件进行操作
  7.2.linux系统设备文件用于表示硬件设备本身
      设备文件分类：字符设备文件和块设备文件
      字符设备文件就是字符设备在用户空间的表现形式；
      块设备文件就是块设备在用户空间的表现形式；
      字符设备文件和块设备文件的访问同样利用系统调用函数
  7.3.字符设备文件
      明确：字符设备文件本身就是字符设备硬件
      存放目录：/dev目录
      例如：S5PV210四个串口为例：
      ls /dev/s3c2410_serial* -lh

      crw-rw----  204,  64  s3c2410_serial0
      crw-rw----  204,  65  s3c2410_serial1
      crw-rw----  204,  66  s3c2410_serial2
      crw-rw----  204,  67  s3c2410_serial3
      说明：
      1.四个串口硬件都有唯一对应的字符设备文件
      2."c"表示此设备文件代表的是字符设备文件
        “b”表示此设备文件代表的是块设备文件
      3.“204”：表示设备文件的主设备号
      4."64,65,66,67"：表示设备文件的次设备号
      5.“s3c2410_serial0”:表示第一个串口的设备文件
        “s3c2410_serial1”:表示第二个串口的设备文件
        “s3c2410_serial2”:表示第三个串口的设备文件
        “s3c2410_serial3”:表示第四个串口的设备文件
      6.访问串口硬件，利用系统调用函数对对应的设别文件操作
        打开串口：
        int fd = open("/dev/s3c2410_serial0", O_RDWR);
        读串口：
        read(fd, buf, size);
        写串口：
        write(fd, buf, size);
        关闭串口：
        close(fd);
      7.手动创建设备文件
        mknod /dev/设备文件名 c 主设备号  次设备号                                                   例如：
        mknod /dev/myled c 250 0-

    7.4.设备号,主设备号,次设备号
    设备号包含主，次设备号
    设备号的数据类型：dev_t(本质上是unsigned int)
    设备号的高12位保存主设备号信息
    设备号的低20位保存次设备号信息
    设备号相关操作宏：
    1.MKDEV
      已知主，次设备号，求设备号
      设备号=MKDEV(主设备号,次设备号);
    2.MAJOR
      已知设备号，求主设备号
      主设备号=MAJOR(设备号);
    3.MINOR
      已知设备号，求次设备号
      次设备号=MINOR(设备号);
    主设备号作用：应用程序根据设备文件的主设备号在茫茫的内核驱动中找到适合自己的设备驱动
         注意：一个设备驱动仅仅根一个主设备号进行绑定关联，以串口为例，主设备号都是204，说明四个硬件串口共享一个设备驱动
    次设备号作用：如果多个硬件设备共享一个驱动，那么驱动再根据次设备号来分区具体的硬件设备个体
    设备号对于内核来说是一种宝贵的资源！
    向内核申请设备号和释放设备号的方法：
    int alloc_chrdev_region(dev_t *dev,unsigned baseminor, unsigned count,
    const char *name)
函数功能：向内核申请设备号
参数：
dev:待申请的设备号，内核将申请好的设备号信息保存在dev指向的变量中
baseminor:希望的起始次设备号，一般指定为0
count：次设备号的个数
      如果次设备号从0开始，个数为5个，次设备号分别为：0,1,2,3,4
name:设备名称，切记不是设备文件名，将来出现在cat -/proc/devices中

void unregister_chrdev_region(dev_t dev,int count);
释放设备号
dev:申请号的设备号
count:次设备号的个数

注意：一旦申请完毕，通过cat /proc/devices查看申请的设备号信息

    8.linux内核描述字符设备的数据结构
      linux字符设备驱动给用户提供的操作接口的数据结构:
      struct cdev {
          dev_t dev; //申请的设备号
          int count; //次设备号的个数(硬件设备的个数)
          struct file_operations *fops; //提供的操作方法
      ...
      };//描述字符设备

      struct file_operations {
      		struct module *owner; //驱动的引用计数
       		int (*open)(struct inode *inode,
         	struct file *file) //打开设备
      		int (*release)(struct inode *inode,
      		struct file *file) //关闭设备
      ...
      };//给字符设别提供的硬件操作方法的数据结构
      成员说明：
      open接口：用于打开设备
      和应用程序的调用关系：
      应用程序调用open->触发软中断(C库)
      ->sys_open(内核)->利用设备号找到驱动->最终调用驱动open接口
      release:用于关闭设备
      和应用程序的调用关系：
        应用程序调用close->触发软中断(C库)->sys_close(内核)->利用fd->最终调用驱动的release接口
    案例：利用字符设备驱动框架，实现LED设备驱动，用户需求应用调用open，开所有灯
      	 当应用调用close，关闭所有灯
    实施计划：
    1.定义初始化硬件操作方法对象
      static struct file_operations led_fops = {
          .owner = THIS_MODULE,
          .open = led_open,
          .release = led_close
      };
    2.定义初始化字符设备对象
      static struct cdev led_cdev;
      cdev_init(&led_cdev, &led_fops);
      //结果：led_cdev.fops = &led_fops;-
    3.向内核注册字符设备对象
      cdev_add(&led_cdev, 申请好的设备号, 硬件个数);
      “硬件个数”：2个LED可以作为一个硬件！
      //结果：内核有了一个真实的字符设备驱动

    实施步骤：
    PC机：
    1.mkdir /opt/drivers/day03/1.0 -p
    2.cd /opt/drivers/day03/1.0
    3.vim led_drv.c //驱动
    4.vim led_test.c //应用
    5.vim Makefile
    6.make-
    7.arm-linux-gcc -o led_test led_test.c
    8.cp led_test led_drv.ko /opt/rootfs

    ARM执行：
    1.insmod led_drv.ko
    2.cat /proc/devices //查看申请的设备号信息
    Character devices: //当前系统的字符设备
  	1 mem
  	2 pty
  	3 ttyp
  	4 /dev/vc/0
  	第一列：设备的主设备号
  	第二列：设备的名称
    3.mknod /dev/myled c 查看的主设备号 0
    4../led_test //运行应用程序

**********************************************************
8.linux字符设备驱动硬件操作接口之read
  回顾：应用程序read系统调用
  char buf[1024] = {0}; //用户缓冲区
  ret = read(fd, buf, 1024);-
  //从设备读数据，读到用户缓冲区buf,要读1024字节
  //ret表示实际读取的字节数
  
  对应的底层驱动的操作接口：
  struct file_operations {
  	ssize_t (*read)(struct file *file,
    	char __user *buf,
    	size_t count,
    	loff_t *ppos)
   }
  read接口：用于读设备
  参数：
  file:文件指针,对应的就是fd
  buf:buf指针用__user来修饰，表明buf指针变量保存的是用户缓冲区的首地址，所以内核代码不允许直接访问buf(*buf = 1),如果驱动要想用户缓冲区buf中写入数据，只需利用内核提供的内存拷贝函数即可完成数据的写入操作；
  内核提供的内存拷贝函数：
  int copy_to_user(void __user *to,const void *from,unsigned long n)
  函数功能：将内核缓冲区的数据拷贝到用户缓冲区
  to：目标地址，保存的是用户缓冲区的首地址
  from:源地址，保存的是内核缓冲区的首地址
  n：拷贝的字节数

count：要拷贝的字节数
ppos:记录上一次的读位置信息，代码操作如下：
     1.首先获取上一次的读位置信息
       loff_t pos = *ppos;
     2.这次读了12字节，读完以后，要记得更新读位置
       *ppos = pos + 12;
     此形参一般用于多次读！
  案例：读取灯的状态
   0:灯2,灯1都关
   1:灯2关，灯1开
   2:灯2开，灯1关
   3：灯2，灯1都开

9.linux字符设备驱动硬件操作接口之write
  回顾应用程序write
  int data = 0x55; //用户缓冲区
  write(fd, &data, 4); //向设备写入数据0x55,4字节
  对应的底层驱动的write接口：
  struct file_operations {
    ssize_t (*write)(struct file *file,
          const char __user *buf,
          size_t count,
          loff_t *ppos)
  }
  write接口：用于写设备
  调用关系：
  应用调用write->软中断(C库)->sys_write(内核)->write接口(驱动)
  参数：
  file:文件指针
  buf:此指针变量保存的用户缓冲区的首地址，在内核空间不能直接访问操作(int data = *(int *)buf,危险)，可以利用内核提供的内存拷贝函数，将用户数据拷贝到内核缓冲区中；copy_from_user

  unsigned long copy_from_user(void *to,const void __user *from,unsigned long n)
  函数功能：将用户缓冲区的数据拷贝到内核缓冲区中
  to:目的地址，保存的是内核缓冲区的首地址
  from:源地址，保存的是用户缓冲区的首地址
  n:拷贝的字节数
 

  count：要写入的字节数
  ppos:保存上一次的写位置，操作如下：
       1.首先获取上一次的写位置
         loff_t pos = *ppos;
       2.每一次写入12字节，更新写位置信息
         *ppos = pos + 12
       此参数适合多次写入操作

  总结：
  底层驱动的open,release接口可以不用指定初始化，应用程序永远返回成功；
  案例：用户写1，开所有灯，用户写0，关所有的灯
  案例：用户能够控制某个灯的开关状态
  提示：
  struct led_cmd {
      int cmd; //控制开关：1=开；0=关
      int index; //灯的编号：1=第一个灯；2=第二个灯
  };                                                                                                                                                                           
  
```

----

DAY 04

```
4.linux字符设备驱动硬件操作接口之ioctl
  4.1.应用程序ioctl系统调用函数
  ioctl系统调用函数的原型：
  int ioctl(int fd, int request, ...) //内核的一个痛点
  函数功能：
  	1.向设备发送控制命令
  	2.能够和设备完成读/写操作
  参数：
  fd:设备文件描述符
  request:向设备发送的控制命令字,命令字由程序员自己单独定义,例如：
  	#define LED_ON	0x100001 //开灯命令
  	#define LED_OFF 0x100002 //关灯命令
   将来驱动程序利用switch...case进行命令的解析判断！
  ...:如果应用和驱动要完成数据的读或者写,"..."就是传递的用户缓冲区的首地址，将来驱动可以向这个用户缓冲区中写或者读数据(切记也要用copy*)
  例如：
  	//仅仅开灯
  	ioctl(fd, LED_ON);
  	//仅仅关灯
  	ioctl(fd, LED_OFF);
  	//不仅仅要开灯，并且指定开第一个灯
  	int index = 1; //用户缓冲区,保存灯的编号
  	ioctl(fd, LED_ON, &index);
  	//不仅仅要关灯，并且指定关第一个灯
  	int index = 1; //用户缓冲区,保存灯的编号
  	ioctl(fd, LED_OFF, &index); //&index==0x40100010
  	
   4.2.对应底层驱动的ioctl接口
       struct file_operations {
       	  long (.unlocked_ioctl)(struct file *file,
       	  			unsigned int cmd,
       	  			unsigned long arg)
       	  			//arg == 0x40100010
       };
       ioctl接口：
       		1.向设备发送控制命令
  			2.能够和设备完成读/写操作
       调用关系：
       应用ioctl->软中断->sys_ioctl->.unlocked_ioctl
       参数：
       file:文件指针
       cmd:保存用户向设备发送的控制命令字
       	   switch(cmd) {
       	   	case LED_ON:
       	   		...
       	   		break;
       	   	case LED_OFF:
       	   		...
       	   		break;
       	   }
       arg:保存用户缓冲区的首地址,在内核空间照样不能直接访问操作(int index = *(int *)arg危险),需要利用内存拷贝函数：copy_from_user/copy_to_user,例如：
           int index; //内核缓冲区
           copy_from_user(&index, (int *)arg, 4);
       案例：利用ioctl实现开关某个灯 
       实施步骤：
       1.mkdir /opt/drivers/day04/1.0 -p
       2.cd /opt/drivers/day04/1.0
       3.vim led_drv.c
       4.vim Makefile
       5.vim led_test.c
       6.make
       7.arm-linux-gcc -o led_test led_test.c
       8.cp led_test led_drv.ko /opt/rootfs
       
       ARM执行：
       1.insmod led_drv.ko
       2.cat /proc/devices //查看主设备号
       3.mknod /dev/myled c 主设备号 0
       4../led_test    

**********************************************************
5.linux内核字符设备文件的自动创建
  手动创建：mknod
  自动创建设备文件的步骤：
  1.保证根文件系统支持mdev可执行程序
    mdev将来是创建设备文件的真正的人！
    which is mdev //查看mdev的路劲
  2.保证文件系统的etc目录下有fstab文件，文件内容必须有：
    proc  /proc        proc   defaults        0     0
    说明：将procfs虚拟文件系统挂接到/proc目录
    sysfs /sys         sysfs  defaults        0     0
    说明：将sysfs虚拟文件系统挂接到/sys目录
    tmpfs /dev         tmpfs  defaults        0     0
    说明：将tmpfs虚拟文件系统挂接到/dev目录
    注意：sysfs,tmpfs,procfs虚拟文件系统的内容都是存在于内存中！
  3.保证根文件系统的etc/init.d/rcS脚本中，必须有：
    /bin/mount -a #为了解析fstab文件
    echo /sbin/mdev > /proc/sys/kernel/hotplug 
    #将来内核解析hotplug文件，执行mdev可执行程序，创建设备文件
  4.字符设备驱动只需调用以下四个函数，即可完成设备文件的最终创建
    
    struct class *cls; //定义设备类指针(树枝)
    入口函数调用：
    //定义设备类(长树枝，树枝名叫tarena)
    cls = class_create(THIS_MODULE, "tarena");
    //创建设备文件(长苹果),dev表示设备号,myled表示设备文件
    device_create(cls, NULL, dev, NULL, "myled");
    
    出口函数调用：
    //删除设备文件(摘苹果)
    device_destroy(cls, dev);
    //删除设备类(砍树枝)
    class_destroy(cls);
    
    案例：基于之前的代码，添加自动创建设备文件的功能

**********************************************************
6.linux字符设备驱动之inode,file
  struct inode {
  	dev_t i_rdev; //保存设备文件的设备号
  	struct cdev *i_cdev;//指向对应的字符设备驱动对象led_cdev
  	....
  };
  作用：描述文件的物理信息,比如文件的大小,创建日期，修改日期，访问权限，用户和组等
  文件一旦创建，内核就会创建对应的inode对象；
  文件一旦销毁，内核也会销毁对应的inode对象
  
  struct file {
  	struct file_operations *f_op; //指向字符设备驱动的硬件操作对象led_fops
  	...
  };
  作用：描述文件被打开(open)以后的信息，例如
        open("/dev/myled", O_RDWR);
  文件被打开，内核创建对应的file对象
  文件关闭，内核销毁对应的file对象
  注意：一个文件只能有一个inode对象，但是可以多有个file对象
  
  inode对象和file对象的关系：
  struct inode *inode = file->f_path.dentry->d_inode;
  
  案例：利用次设备号来分区2个LED硬件设备
  分析：
  1.一个驱动管理2个硬件，LED1，LED2
  2.对应的设备文件分别：/dev/myled1,/dev/myled2
  3.驱动只需一个，主设备号只需一个，并且共享
  4.次设备号分别定义为：0,1
  5.字符设备对象led_cdev共享
  6.字符设备操作对象led_fops共享
  7.操作函数共享
  8.操作函数通过次设备号区分
  
**********************************************************
7.linux内核混杂设备驱动开发相关内容
  特点：
  	本质上还是字符设备
  	只是主设备号由内核定义好为10
  	各个混杂设备通过次设备号来区分
  数据结构：
  	struct miscdevice {
  		int minor;
  		char *name;
  		const struct file_operations *fops;
  		...
  	};
  	minor:混杂设备对应的次设备号，一般指定为MISC_DYNAMIC_MINOR宏，表明让内核帮你分配一个次设备号
  	name:设备文件名，混杂设备的设备文件由内核自动创建
  	fops:混杂设备具有的硬件操作方法
  	
   实现混杂设备驱动的步骤：
   	1.定义初始化混杂设备对象
   	2.注册混杂设备对象到内核
   	  misc_register
   	3.卸载混杂设备对象
      misc_deregister
          
   案例：采用混杂设备编程框架，来实现LED设备驱动
```



查看输入设备信息：

```
cat /proc/bus/input/devices 
I: Bus=0019 Vendor=0000 Product=0003 Version=0000
N: Name="Sleep Button"
P: Phys=PNP0C0E/button/input0
S: Sysfs=/devices/LNXSYSTM:00/LNXSYBUS:00/PNP0C0E:00/input/input0
U: Uniq=
H: Handlers=kbd event0 
B: PROP=0
B: EV=3
B: KEY=4000 0 0

I: Bus=0019 Vendor=0000 Product=0001 Version=0000
N: Name="Power Button"
P: Phys=PNP0C0C/button/input0
S: Sysfs=/devices/LNXSYSTM:00/LNXSYBUS:00/PNP0C0C:00/input/input1
U: Uniq=
H: Handlers=kbd event1 
B: PROP=0
B: EV=3
B: KEY=10000000000000 0

I: Bus=0019 Vendor=0000 Product=0001 Version=0000
N: Name="Power Button"
P: Phys=LNXPWRBN/button/input0
S: Sysfs=/devices/LNXSYSTM:00/LNXPWRBN:00/input/input2
U: Uniq=
H: Handlers=kbd event2 
B: PROP=0
B: EV=3
B: KEY=10000000000000 0

I: Bus=0019 Vendor=0000 Product=0006 Version=0000
N: Name="Video Bus"
P: Phys=LNXVIDEO/video/input0
S: Sysfs=/devices/LNXSYSTM:00/LNXSYBUS:00/PNP0A08:00/LNXVIDEO:00/input/input3
U: Uniq=
H: Handlers=kbd event3 
B: PROP=0
B: EV=3
B: KEY=3e000b00000000 0 0 0

I: Bus=0003 Vendor=093a Product=2510 Version=0111
N: Name="PixArt USB Optical Mouse"
P: Phys=usb-0000:00:14.0-4.1/input0
S: Sysfs=/devices/pci0000:00/0000:00:14.0/usb1/1-4/1-4.1/1-4.1:1.0/0003:093A:2510.0001/input/input4
U: Uniq=
H: Handlers=mouse0 event4 
B: PROP=0
B: EV=17
B: KEY=ff0000 0 0 0 0
B: REL=103
B: MSC=10

I: Bus=0003 Vendor=258a Product=1006 Version=0110
N: Name="SINO WEALTH USB KEYBOARD"
P: Phys=usb-0000:00:14.0-4.3/input0
S: Sysfs=/devices/pci0000:00/0000:00:14.0/usb1/1-4/1-4.3/1-4.3:1.0/0003:258A:1006.0002/input/input5
U: Uniq=
H: Handlers=sysrq kbd event5 leds 
B: PROP=0
B: EV=120013
B: KEY=1000000000007 ff9f207ac14057ff febeffdfffefffff fffffffffffffffe
B: MSC=10
B: LED=7

I: Bus=0003 Vendor=258a Product=1006 Version=0110
N: Name="SINO WEALTH USB KEYBOARD"
P: Phys=usb-0000:00:14.0-4.3/input1
S: Sysfs=/devices/pci0000:00/0000:00:14.0/usb1/1-4/1-4.3/1-4.3:1.1/0003:258A:1006.0003/input/input6
U: Uniq=
H: Handlers=sysrq kbd event6 
B: PROP=0
B: EV=10001f
B: KEY=3007f 0 0 483ffff17aff32d bf54444600000000 1 130c130b17c007 ffa67bfad941dfff febeffdfffefffff fffffffffffffffe
B: REL=40
B: ABS=100000000
B: MSC=10


```



-----

```
**********************************************************
8.linux内核中断编程：
  面试题：谈谈对中断的理解
  1.为什么有中断机制?
    外设的处理速度要远远慢于CPU的处理速度；
    如果采用轮询方式(CPU一直等待)处理外设,会降低CPU的利用率；
    如果采用中断方式处理外设,会大大提高CPU的利用率
    以CPU读取串口数据为例：
    为了方式串口数据的丢失,不采用中断，采用轮询，CPU只能做一件不停读取串口的事情；
    如果采用中断，CPU当发现串口数据不可读，CPU可以做其他事情，一旦串口数据准备就绪，串口会给CPU发送一个中断信号，告诉CPU，数据准备就绪，请来处理，CPU处理完毕以后，再接着执行原先被打断的任务！
  2.中断的硬件触发和连接
    明确：外设的中断信号并不是直接输送给CPU，而是先给中断控制器(一般都是集成在CPU内部)，每当外设产生一个中断信号，此中断信号首先输送给中断控制器，经过中断控制器的判断以后，再决定是否给CPU发送
    明确：中断控制器的作用
    	能够屏蔽或者使能某个外设中断
    	能够设置外设中断的优先级
    	能够设置CPU外部的外设的中断信号的有效触发方式
    	如果是多核,能够指定中断信号给哪个CPU发送  
    	
    一旦CPU接收到了外设的中断信号以后，CPU开始进行中断的处理
   
   3.中断的处理流程
     参见interrupt.bmp  
     
   4.中断的软件编程
     明确：不管是裸板还是带操作系统，中断编程的步骤一致:
     1.编写异常向量表的代码
     2.编写保护现场的代码
     3.编写中断处理函数
     4.编写恢复现场的代码
     
     不管是在裸板上(keil)还是在linux内核中，进行中断编程，1,2,4三步骤已经由内核实现，驱动开发者只需要向内核注册中断处理函数即可！
     
    5.linux内核中断编程
     明确：内核中断编程，只需做将中断处理函数注册到内核中即可，一旦中断触发，内核执行对应的中断处理函数！
     向内核注册和卸载中断处理函数的方法为：
     //irq:interrupt request
     request_irq/free_irq(大名鼎鼎)
     函数原型：
     request_irq(unsigned int irq,
     		 irq_handler_t handler, 
     		 unsigned long flags,
	         const char *name, 
	         void *dev)
     函数功能：
     	1.向内核申请硬件中断资源   
     	2.然后注册硬件中断的中断处理函数到内核   
     参数：
     irq:硬件中断对应的软件编号，又称中断号(类似硬件中断的身份证号)，从32开始，0~31内核保留！中断号由芯片厂家在内核源码定义！例如：
        第一个硬件中断XEINT0，对应的软件编号为IRQ_EINT(0)
        第十个硬件中断XEINT10,对应的软件编号为IRQ_EINT(10)
     handler:中断处理函数
     flags：中断标志
     	    对于外部中断，flags要指定有效的中断触发方式：
     	    IRQF_TRIGGER_FALLING:
     	    IRQF_TRIGGER_RISING:
     	    IRQF_TRIGGER_HIGH:
     	    IRQF_TRIGGER_LOW: 
     	    可以做位或运算！
     	    对于内部中断，flags给0 
    name：中断名称，将来出现在cat /proc/interrupts
    dev:给中断处理函数传递的参数，不传递给NULL 
    
    中断处理函数说明：
    typedef irqreturn_t (*irq_handler_t)(int, void *); 
    typedef enum irqreturn irqreturn_t;
    例如：
    //中断处理函数定义
    static irqreturn_t xxx_irq_handler(int irq, void *dev)
    {
    	//irq:对应的中断号
    	//dev：保存注册中断时传递的参数	
    	return IRQ_NONE; //中断不做处理，异常
    	或者
    	return IRQ_HANDLED; //中断处理，正常
    	或者
    	return IRQ_WAKE_THREAD; //中断处理完毕，再启动一个内核线程
    }  
    
    //释放中断资源和删除中断处理函数
    void free_irq(int irq, void *dev)
    irq:释放的中断号资源
    dev:给中断处理函数传递的参数，切记一定要和注册时传递的参数要一样！ 
    
    案例：编写KEY_UP按键的中断处理函数
    提示：标准按键值在include/linux/input.h定义
    实施步骤：
    1.去除内核的官方按键驱动
      cd /opt/kernel
      make menuconfig
      Device Drivers->
      	Input device support->
      		Keyboards->
      			<*> s3c gpio keypad support //去掉保存退出
      make zImage
      cp arch/arm/boot/zImage /tftpboot
      重启开发板
    
    开发板执行：
    1.insmod btn_drv.ko
    2.操作按键，看打印信息   
    
```

DAY05:

```
3.6.linux内核对于中断处理函数的要求
    明确：
    	在linux内核，"任务"包括进程,硬件中断,软中断
        在linux内核，优先级的划分：
        	硬件中断大于软中断
        	软中断大于进程
        	硬件中断无优先级
        	软中断有优先级
        	进程有优先级
        中断不隶属于任何进程，不参与进程的调度
        如果中断的处理函数长时间占用CPU资源，导致系统的并发和响应能力受到影响
        
   结论：内核要求中断处理函数执行的速度要快，更不能做休眠操作
   问：有些场合无法满足这种要求，内核如何处理呢？
   
3.7.linux内核对于以上问题，提出了顶半部和底半部机制加以优化
  顶半部的特点：
      1.本质还是中断处理函数
      2.执行原先中断处理函数中紧急，耗时较短的任务
      3.不可被中断
      4.登记底半部的内容，以便将来CPU去执行
  
  底半部的特点：
      1.本质就是延后执行的一种手段，不一定非要和顶半部配合使用
        例如将来让某个事情延后执行，可以单独采用底半部机制
      2.如果有中断，执行原先中断处理函数中不紧急，耗时较长的任务
      3.可被别的任务中断
      4.CPU在适当的时候去执行底半部
      5.至于在何时何地去登记，随便你！一旦被登记，CPU资源也能够获取，底半部的内容立马得到执行！  
      6.底半部的实现方法三种：tasklet、工作队列、软中断
3.8.底半部机制之tasklet
  明确：tasklet的本质仅仅是延后执行
  特点：
      1.tasklet本身基于软中断实现的；
      2.延后执行的内容都会放在tasklet的一个函数中，此函数又称延后处理函数；里面可以放不紧急，耗时较长的内容
      3.tasklet的延后处理函数工作在中断上下文中，所以不能进行休眠操作
  数据结构：
  struct tasklet_struct {
  	void  (*function)(unsigned long data);
  	unsigned long data;
  	...
  };
  function:延后处理函数
  data:给延后处理函数传递的参数，一般传递指针
  
  使用步骤：
  1.定义初始化tasklet对象
    int g_data = 0x55;
    DECLARE_TASKLET(btn_tasklet,btn_tasklet_func, (unsigned long)&g_data);
    //定义tasklet对象，名称为btn_tasklet
    //延后处理函数为btn_tasklet_func
    //给延后处理函数传递的参数为g_data
    
    或者：
    struct tasklet_strcut btn_tasklet;
    tasklet_init(&btn_tasklet, btn_tasklet_func,(unsigned long)&g_data);
    
  2.编写延后处理函数
    切记：不要进行休眠操作
  3.在适当的地方登记延后处理函数
     tasklet_schedule(&btn_tasklet)
     
   案例：将原先的按键中断处理函数添加底半部机制
   //jdy:顶半部的中断处理函数执行完后，再执行底半部；当有其他优先的任务时，执行优先任务后执行底半部。
 
3.9.底半部机制之工作队列
   特点：
   1.工作队列 = 工作 + 队列 = 工作的队列 = 队列中每一个节点就是一个工作 
     “工作”：延后执行的任务
   2.工作队列也是延后执行的一种手段
   3.工作队列中每一个节点代表着延后执行的某个事，这个事要放在一个函数中，此函数又称工作节点的延后处理函数
   4.工作队列中每一个节点对应的延后处理函数工作在进程上下文中，可以进行休眠操作
   5.工作队列就是为了解决tasklet的延后处理函数不能休眠的问题,将来如果发现延后处理函数中需要做休眠操作，只能采用工作队列，而不能采用tasklet
   6.工作队列的延后处理函数的效率不如tasklet的延后处理函数
   7.工作队列可以使用内核默认的！
     
   工作队列中每一个节点的数据结构
   struct work_struct {
   	work_func_t func;//延后处理函数
   	...
   }
   typedef void (*work_func_t)(struct work_struct *work);
   
   使用步骤：
   1.定义初始化工作队列中某个节点
     struct work_struct btn_work;
     INIT_WORK(&btn_work, btn_work_function);
     //给节点指定一个延后处理函数
   2.编写延后处理函数
     一般做不紧急，耗时较长的内容
     切记：咱可以进行休眠操作！
   3.在适当的地方进行登记延后处理函数，一般在顶半部(中断处理函数)
     schedule_work(&btn_work); //一旦登记完毕，适当的时候，内核调用对应的延后处理函数
     结果是将这个节点添加到内核默认的工作队列中去！
     
   案例：编写按键驱动，添加底半部机制，采用工作队列

3.9.底半部机制之软中断
  	特点：
  	1.软中断本质也是延后执行一种手段
  	2.软中断的延后处理函数可以运行在多个CPU上
  	3.由于第2点,内核要求软中断的延后处理函数要具备可重入性
  	  函数可重入性：
  	  1.尽量避免访问全局变量；
  	  2.如果要访问全局变量，记得要进行互斥访问，缺点是大大降低了代码的执行效率
      4.tasklet虽然是基于软中断实现，但是其延后处理函数只能运行在一个CPU上
      5.通过第2和4这两个特点对比，软中断的效率要比tasklet要高
      6.软中断的软件实现，不能采用insmod和rmmod形式的动态加载和卸载，
      只能静态编译到zImage，无形加大了软中断代码实现的复杂度！
      就是因为麻烦，所以给你提供了tasklet机制
```

---

```
**********************************************************
4.linux内核软件定时器
  4.1.硬件定时器
  特点：
  	1.能够周期性的给CPU产生定时器中断信号
  	2.周期或者频率可以通过软件编程来设置
  4.2.硬件定时器在linux内核中的中断处理函数
      vim arch/arm/mach-s5pv210/mach-cw210.c找到
      .timer = &s5p_systimer //即可找到中断处理函数
      定时器中断处理函数将做：
      1.更新系统的运行时间
      2.更新实际时间(墙上时间)
      3.检查进程时间片是否用完，决定是否实施调度
      4.检查是否有超时的软件定时器，如果有超时的软件定时器，内核执行对应的超时处理函数
      5.统计系统的资源信息等
  
  4.3.内核时间相关的概念
      HZ:
      1.内核常量
      2.ARM架构,HZ=100
        x86架构,HZ=1000
      3.HZ=100,表示硬件定时器1秒钟将会给CPU发送100次定时器中断信号,每发生一次定时器中断的时间间隔为10ms
      
      jiffies/jiffies_64:
      1.前者为内核全局变量,数据类型unsigned long(32)
      2.后者为内核全局变量,数据类型unsigned long long(64)
      3.jiffies的值取的是jiffies_64的低32位
      4.jiffies_64用来记录自系统开机依赖,硬件定时器发生了多少次的定时器中断,每发生一次,加1；
        jiffies如果用来记录系统的定时器中断,497天后会溢出
        jiifies_64多少天溢出呢?
      5.一般内核代码使用jiffies居多，一般用来记录流失时间
      例如：
      unsigned long timeout = jiffies + 2*HZ;
      说明：
      jiffies:表示当前时间
      2*HZ:时间间隔为2秒
      timeout:表示2秒以后的时间
      
      //代码纠错
      unsigned long timeout = jiffies + 2*HZ;
      ... //执行一些代码，当然需要一些时间
      //执行完毕，进行判断
      if (jiffies > timeout) {
      	  //超时
      } else {
      	  //没有超时
      }
      
      为了避免溢出:内核提供了相关的宏：
      time_before/time_after
      例如：
      unsigned long timeout = jiffies + 2*HZ;
      ... //执行一些代码，当然需要一些时间
      //执行完毕，进行判断
      if (time_before(jiffies, timeout)) {
      	  //没超时
      } else {
      	  //超时
      }
      
      或者：
      unsigned long timeout = jiffies + 2*HZ;
      ... //执行一些代码，当然需要一些时间
      //执行完毕，进行判断
      if (time_after(jiffies, timeout)) {
      	  //超时
      } else {
      	  //没超时
      }
      
      //研究推敲：
    #define time_after(a,b)  ((long)(b) - (long)(a) < 0))
```

DAY06:

```
软件定时器
  	特点：
  	1.定时器到期能够执行某个任务
  	2.定时器的超时处理函数将会执行某个任务
  	3.定时器一旦到期,内核会删除定时器
  	4.超时处理函数只执行一次
  	5.定时器基于软中断实现,超时处理函数中不能
  	休眠
  	
  	数据结构：
  	struct timer_list {
  		unsigned long expires;
  		void (*function)(unsigned long 	data);
  		unsigned long data;
  		...
  	}
  	expires:超时时候的时间,例如超时时间间隔为10秒,expires = jiffies + 10*HZ
  	function:超时处理函数,不能休眠
  	data:给超时处理函数传递的参数,一般data保存的是指针
  	
  	编程使用：
  	1.定义初始化定时器对象
  	  struct timer_list mytimer;
  	  init_timer(&mytimer);
  	  //超时时间,处理函数,传递的参数需要额外初始化
  	  mytimer.expires = jiffies + 10*HZ;
  	  mytimer.function = mytimer_function;
  	  mytimer.data = 变量的指针;
  	2.编写超时处理函数mytimer_function
  	  切记不能有休眠的代码
  	3.添加注册定时器对象到内核,并且启动定时器
  	  add_timer(&mytimer); //开始倒计时
  	  一旦定时器到期,内核调用超时处理函数，内核会自动删除定时器
  	4.删除定时器
  	  del_timer(&mytimer);
  	5.修改定时器的超时时间
  	  mod_timer(&mytimer, jiffies + 20*HZ);
  	  等价于：
  	  1.先删除定时器del_timer
  	  2.再修改超时时间
  	    mytimer.expires = jiffies + 20*HZ
  	  3.最后添加启动定时器
  	    add_timer
  	    
	案例：利用定时器,每隔2000ms打印一句话
	案例: 利用定时器,每个2000ms开关灯
	案例：利用定时器和内核命令行传参的知识,驱动加载以后,能够动态修改灯的闪烁频率		:100ms,500ms,1000ms,2000ms
	
	ARM:
	1.insmod led_drv.ko
	
3.linux内核延时的方法
  延时分：忙延时和休眠延时
  忙延时：CPU原地空转等待条件满足,适用于延时时间	  比较短的场合
  忙延时的函数：
  1.ndelay(纳秒数);
    ndelay(100); //延时100纳秒
  2.udelay(微秒数)
    udelay(100); //延时100微秒
  3.mdelay(毫秒数);
    mdelay(5); //延时5毫秒
    如果延时时间大于10毫秒,可以采用休眠延时
    
  休眠延时：
  "休眠"：仅仅存于在进程的世界中
  休眠延时：指示进程休眠时会释放它所占用的CPU资源,将CPU资源给其他任务使用
  
  休眠延时的函数：
  1.msleep(微秒数);
    msleep(500); //延时500毫秒
  2.ssleep(秒数);
    ssleep(1000); //延时1000秒
  3.schedule(); //永久性休眠
  4.schedule_timeout(5*HZ); //休眠延时5秒
```

----

```
4.linux内核并发和竞态
  面试题：谈谈进程间通信的机制
  4.1.案例：要求LED设备,同一时刻只能被一个应用程序打开访问操作
  实施方案：
  1.在用户空间实现
    A,B,C多进程可以采用进程间通信来决定由哪个进程打开硬件设备
  2.在底层驱动实现
    不管有多少个进程访问设备，永远先open,它们最终调用同一个底层驱动的open,只需在底层驱动的open做相关的代码限定即可
  参考代码：
  static int open_cnt = 1; 
  static int led_open(struct inode *inode,
  			struct file *file){
  	if (--open_cnt != 0) {
  		printk("设备已被打开!\n");
  		open_cnt++;
  		return -EBUSY;//返回设备忙错误
  	}
  	printk("设备打开成功!\n");
  	return 0;
  }
   static int led_close(struct inode *inode,
  			struct file *file)
  {
  	open_cnt++;
  	return 0;
  }
  回顾：
  gcc编译过程：
  预处理->汇编->编译->连接
  正常情况：
  以--open_cnt语句为了,要经过：
  1.ldr ... //先加载
  2.--  ... //再自减
  3.str ... //最后存储
  
  先A进程来打开设备：
  	1.先加载 open_cnt = 1;
  	2.修改,写回：open_cnt = 0;
  	3.结果：A打开设备成功
  	
  再B进程打开设备：
        1.先加载 open_cnt = 0
        2.修改，写回：open_cnt = -1
        3.结果：B打开设备失败
        
  异常情况：
  先A进程来打开设备：
  	1.先加载 open_cnt = 1
  	由于linux内核支持单CPU进程与进程之前的抢占(高优先级的进程抢占低优先级进程的CPU资	源)，此时此刻，B进程抢占A进程的CPU资源
  B进程接着执行：
  	1.先加载 open_cnt = 1
  	2.修改,写回：open_cnt = 0;
  	3.结果：B打开设备成功
        4.B执行完毕，将CPU资源让给A进程
  A继续执行：
  	2.修改，写回:open_cnt = 0;
  	3.结果：A打开设备成功
  
  总结：产生问题的原因是进程与进程之前的抢占引起的竞争状态；
  	产生这种竞争状态的根本原因是加载,修改,存储这三个过程不具备原子性
  	只要加载,修改,存储这三条语句在执行的时候,不会发生CPU资源的切换，也就是让这三条语句	的执行路径具有原子性，即可避免这种竞争问题
  	
  4.2.linux内核产生竞争状态的几种情形：
    1.单CPU进程与进程的抢占
    2.中断和进程
    3.中断和中断
    4.多核(共享内存，闪存，IO等)
    
  4.3.概念
    1.并发：多个执行单元同时发生
    2.竞态：多个执行单元同时对共享资源的访问，形参竞争状态
    3.共享资源：软件上的全局变量或者硬件寄存器
    4.互斥访问：当一个执行单元在访问共享资源的时候，其他执行单元被禁止访问
    5.临界区：访问共享资源的代码区域
    6.执行路径具有原子性：代码的执行过程不允许发生CPU资源的切换
    
  4.4.linux内核解决竞态问题的方法
  中断屏蔽
  自旋锁
  信号量
  原子操作
  
5.linux内核解决竞态方法之中断屏蔽
  特点：
  1.能够解决进程与进程抢占引起的竞态问题
  2.能够解决中断和进程引起的竞态问题
  3.能够解决中断和中断引起的竞态问题
  4.多核的竞态问题无法解决
  5.中断屏蔽所保护的临界区执行速度要快,更不能做休眠操作
  
  编程步骤：
  1.明确驱动代码中哪些是共享资源
  2.明确驱动代码中哪些是临界区
  3.明确临界区中是否有休眠
  4.访问临界区之前屏蔽中断
    unsigned long flags;
    local_irq_save(flags);//屏蔽中断,保存中断状态到flags
    “local”:指的是当前临界区所在的CPU，把对应的中断信号进行屏蔽，其他CPU上的中断信号不做屏蔽
  5.访问临界区
    切记不能休眠
    执行速度要快
    
  6.访问临界区之后恢复中断
    local_irq_restore(flags);//恢复中断,恢复之前保存的中断状态
    参考例子代码：
    static int open_cnt = 1; //共享资源
    static int led_open(struct inode *inode,
  			struct file *file){
  	unsigned long flags;
  	local_irq_save(flags);//关中断
  	//临界区
  	if (--open_cnt != 0) {
  		printk("设备已被打开!\n");
  		open_cnt++;
  		local_irq_restore(flags);//开中断
  		return -EBUSY;//返回设备忙错误
  	}
  	local_irq_restore(flags);//开中断
  	printk("设备打开成功!\n");
  	return 0;
   }

5.linux内核解决竞态方法之自旋锁
  特点：
  1.能够解决多核引起的竞态问题
  2.能够解决进程与进程的抢占引起的竞态问题
  3.如果要解决中断的引起的问题,必须使用衍生自旋锁
  4.自旋锁=自旋 + 锁
  5.持有自旋锁的任务在访问临界区时，临界区的执行速度要快，更不能做休眠操作
  6.如果没有获取自旋锁的任务,将会原地忙等待自旋锁的释放
  
  数据类型：spinlock_t
  
  编程步骤：
  1.明确共享资源
  2.明确临界区
  3.明确是否有休眠
  4.定义初始化自旋锁
    spinlock_t lock;
    spin_lock_init(&lock); 
  5.访问临界区之前获取自旋锁
    spin_lock(&lock);
    说明：
    1.如果获取自旋锁,立即返回
    2.如果没有获取自旋锁,原地忙等待,直到持有自旋锁
    3.中断引起的竞态问题无法解决
    
    或者：
    
    spin_trylock(&lock);
    说明：
    1.如果获取自旋锁,返回true
    2.如果没有获取自旋锁,也返回,不进行忙等待,返回false
    3.中断引起的竞态问题无法解决
    
    或者：
    
    unsigned long flags;
    spin_lock_irqsave(&lock,flags);
    说明：
    1.获取自旋锁,立即返回
    2.没有获取自旋锁,忙等待
    3.也能够解决中断引起的竞态问题 
    
  6.访问临界区
    切记不能做休眠操作
    
  7.访问临界区之后,释放自旋锁
    spin_unlock(&lock);
    说明：
    此方法跟spin_lock/spin_trylock配对
    
    或者
    spin_unlock_irqrestore(&lock,flags);
    说明：
    此方法跟spin_lock_irqsave配对 
 
    参考代码：
    static int open_cnt = 1; //共享资源
    static int led_open(struct inode *inode,
  			struct file *file){
  	unsigned long flags;
  	spin_lock_irqsave(&lock, flags);
  	//关中断,获取锁
  	//临界区
  	if (--open_cnt != 0) {
  		printk("设备已被打开!\n");
  		open_cnt++;
  	 spin_unlock_irqrestore(&lock,flags);
  	        //释放锁,开中断
  		return -EBUSY;//返回设备忙错误
  	}
  	spin_unlock_irqrestore(&lock,flags);
  	//释放锁,开中断
  	printk("设备打开成功!\n");
  	return 0;
   }
   
   
   案例：利用自旋锁实现一个设备只能被打开一次
   实验步骤：
   ARM执行：
   1.insmod led_drv.ko
   2.ls /dev/myled -lh
   3../led_test & //启动A进程，后台运行
   4.ps  //查看A进程的PID
   5.top //查看A进程的状态,按Q键退出
      PID      休眠状态		  进程名称
      86   ...  S(休眠)  .... ./led_test
   6../led_test  //启动B进程
   7.kill A进程的PID
 
6.linux内核解决竞态方法之信号量
  特点：
  1.信号量又称睡眠锁
  2.信号量的本质就是解决自旋锁保护的临界区中不能进行休眠的问题,如果将来临界区有休眠动作,只能考虑使用信号量,而不是自旋锁
  3.没有获取信号量的任务将会进入休眠等待状态 
  
  数据类型：struct semaphore
  
  编程步骤：
  1.明确共享资源
  2.明确临界区
  3.明确是否有休眠
  4.定义初始化信号量
    struct semaphore sema;
    sema_init(&sema, 1); //初始化为互斥信号量
  5.访问临界区之前获取信号量
    down(&sema)
    说明：
    1.如果获取信号量,立即返回
    2.如果没有获取信号,进程将进入不可中断的休眠状态,直到获取信号量立即返回
    3.不可中断的休眠状态指示休眠的进程在休眠期间不会立即处理接收到的信号,而是休眠的进程被唤醒以后才会处理之前接收到的信号;这种休眠状态的进程只能由驱动主动唤醒(持有信号量的任务来唤醒)
    4.不能用于中断
    
    或者：
    down_interruptible(&sema);
    1.如果获取信号量,立即返回false
    2.如果没有获取信号,进程将进入可中断的休眠状态,直到获取信号量立即返回false,如果返回true表示是由于信号引起的唤醒
    3.可中断的休眠状态指示休眠的进程在休眠期间会立即处理接收到的信号
    4.不能用于中断
    5.可中断的休眠进程被唤醒的方法有两种：
      1.一种接收到了信号
      2.一种接收到了信号量
    
    或者：
    down_trylock(&sema);  
    1.获取信号量返回false
    2.没有获取信号量,任务不休眠,立即返回true
    3.可以用于中断上下文
    
  6.访问临界区
    可以进行休眠操作
  7.访问临界区之后释放信号
    up(&sema);
    1.先唤醒休眠的进程
    2.再释放信号量给之前休眠的进程
  
  案例：利用信号量,实现一个设备只被打开一次
  实验步骤：
  down方法：
  1.insmod led_drv.ko
  A->B->kill A->kill B
  2../led_test & //启动A进程
    A休眠是因为调用sleep
  3.ps //查看A进程的PID
  4../led_test & //启动B进程
    B休眠是因为没有获取信号量
  5.ps //查看B进程的PID
  6.top //查看A,B进程的状态,按Q键退出
     106    S    ./led_test
     109    D    ./led_test
     第一列：进程的PID
     第二列：进程的状态
            "S":可中断的休眠状态
            “D”:不可中断的休眠状态
            
  7.kill A进程的PID
  8.top //查看B进的状态
    B休眠是因为调用sleep
  9.kill B进程的PID
  
  A->B->kill B->kill A
  10../led_test & //启动A
  11../led_test & //启动B
  12.ps 
  13.top 
  14.kill B进程的PID
  15.kill A进程的PID
  
  down_interruptible方法：
  1.insmod led_drv.ko
  A->B->kill A->kill B
  2../led_test & //启动A进程
    A休眠是因为调用sleep
  3.ps //查看A进程的PID
  4../led_test & //启动B进程
    B休眠是因为没有获取信号量
  5.ps //查看B进程的PID
  6.top //查看A,B进程的状态,按Q键退出
     106    S    ./led_test
     109    S    ./led_test
     第一列：进程的PID
     第二列：进程的状态
            "S":可中断的休眠状态
            “D”:不可中断的休眠状态
            
  7.kill A进程的PID
  8.top //查看B进的状态
    B休眠是因为调用sleep
  9.kill B进程的PID
  
  A->B->kill B->kill A
  10../led_test & //启动A
  11../led_test & //启动B
  12.ps 
  13.top 
  14.kill B进程的PID
  15.kill A进程的PID
```



---

DAY07:

```
3.linux内核等待队列机制
	特点：
	1.等待分忙等待和休眠等待,等待队列机制研究的等待是休眠等
	2.休眠等待是指当前进程释放CPU资源,将CPU资源给其他任务使用
	3.为何等待,等待某个事件到来,等待某个内容得到满足；
	4.linux系统下休眠等待的方法：
	  在用户空间调用usleep,sleep等进行休眠
	  在内核空间调用msleep,ssleep,schedule,schedule_timeout等
	  这些函数调用时一般都需要指明具体的休眠时间
	  缺点在于必须指定时间，并且要休眠够指定的时间,如果事件提前得到满足,无法唤醒,除非发送信号
	  
	  对于休眠时间不确定,事件何时到来不确定，以上休眠方法显然不够用
	5.进程的状态：
	  进程的运行状态：TASK_RUNNING
	  进程的准备就绪状态：TASK_READY
	  进程的休眠状态：
	  	不可中断的休眠：TASK_UNINTERRUPTIBLE
	  	可中断的休眠：TASK_INTERRUPTIBLE
	6.等待队列 = 等待 + 队列 = 要休眠的进程在等待 + 队列 = 有多个休眠的进程放在一组形成队列
	jdy:进程等待事件满足
          例如：
          A,B,C三个进程要读串口,但是串口没有数据,A,B,C进入休眠状态等待串口数据的到来，A,B,C它们在同一个队列中
	7.同一个等待对列中的每一个节点,每一个休眠的进程都等待同一个事件，如果等待不同的事件，将进程放在不同的队列中去！直到事件到来唤醒休眠的进程
	总结：linux内核等待队列机制的本质目的就是让进程在内核空间随时随地进行休眠,随时随地进行唤醒
	
等待队列实现进程休眠的编程步骤：
	要休眠的进程类似小鸡(由驱动开发者来实现)
	等待队列头类似鸡妈妈(由驱动开发者来实现) //不同队列，不同等待队列头
	进程的处理者调度器类似老鹰(由内核来实现)
	
```



1.定义初始化等待队列头(构造一个鸡妈妈)
	  wait_queue_head_t wq; //定义一个等待队列头
	  init_waitqueue_head(&wq); //初始化等待队列头
	  注意：等待队列头的个数要依赖事件的类型和种类
	  例如：
	  A,B,C三个读进程对应一个读等待队列头
	  D,E,F三个写进程对应一个写等待队列头

2.定义初始化装载休眠进程的容器(构造一个小鸡)
	  wait_queue_t wait; //定义一个装载进程的容器
	  init_waitqueue_entry(&wait, current);//将当前进程添加到容器中
	  说明：
	  current:
	  	1.内核全局指针变量
	  	2.数据类型：struct task_struct
	  	3.struct task_struct用来描述linux系统每一个进程
	  	  每一个进程，内核会给这个进程创建一个task_struct对象
	  	  struct task_struct {
	  	  	pid_t pid;//进程的PID
	  	  	char comm[TASK_COMM_LEN];//进程的名称
	  	  	...
	  	  };
	  	4.current指针指向当前进程
	  	5."当前进程"：是指正在获取使用CPU资源的进程
	  	
3.将休眠的进程添加到等待队列中去(将小鸡添加到鸡妈妈的后面)
	   add_wait_queue(&wq, &wait);
	   注意：此时此刻当前进程还没有真正的休眠

4.设置当前进程的休眠状态为可中断或者不可中断的休眠状态
	   set_current_state(TASK_INTERRUPTIBLE); //可
	   或者
	   set_current_state(TASK_UNINTERRUPTIBLE); //不可
	 
5.让当前进程进入真正的休眠状态
	   schedule(); //永久性休眠
	   注意：
	   此时此刻代码执行到这里不再继续执行，等待事件得到满足被唤醒，或者接收到了信号被唤醒(前提是可中断的休眠状态)
	
6.一旦进程被唤醒,将从schedule函数返回，代码继续执行，然后要设置进程的状态为运行，并且将当前进程从休眠队列中移出
	   set_current_state(TASK_RUNNING);
	   remove_wait_queue(&wq, &wait);   

7.要判断进程被唤醒的原因，是驱动主动唤醒(事件得到满足)还是信号引起的唤醒的？
	   if (signal_pending(current)) {
	   	printk("由于信号引起的唤醒!\n");
	   	return -ERESTARTSYS;
	   } else {
	   	printk("事件得到满足,驱动主动唤醒!\n");
	   	... //处理事件
	   }

8.事件得到满足,驱动主动唤醒休眠进程的方法：
	   wake_up(&wq); //唤醒wq等待队列中所有的休眠进程
	   或者
	   wake_up_interruptible(&wq); //唤醒wq等待队列中所有的的睡眠类型为可中断的休眠进程
	 

	 案例：写进程唤醒读进程
	 insmod btn_drv.ko
	 ./btn_test r & //启动读进程
	 ./btn_test w //启动写进程
	 ./btn_test r & //启动读进程
	 kill 读进程PID
	 
	 案例：编写按键驱动,上报按键的状态和按键值
	 1.应用程序调用read读取按键的信息
	 2.应用程序发现按键无操作,进程在内核空间进行休眠，
	 等待按键有操作
	 3.按键有操作，唤醒休眠的进程，按键有操作，必然产生中断，只需在中断处理函数中唤醒休眠的进程，还要保存将来上报的按键信息
	 4.进程被唤醒，上报按键信息
	 
	 1.insmod btn_drv.ko
	 2.ls /dev/mybtn -lh
	 3.cat /proc/interrupts
	 4../btn_test 启动读按键进程
	 5.操作按键

