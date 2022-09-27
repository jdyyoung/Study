linux设备驱动程序--sysfs用户接口的使用 - yangjunhe - 博客园 - https://www.cnblogs.com/yangjunhe460/p/11696626.html

Linux驱动学习 —— 在/sys下面创建目录示例 - 摩斯电码 - 博客园 - https://www.cnblogs.com/pengdonglin137/p/6217419.html

Linux下/sys目录介绍_何妨一下楼的博客-CSDN博客_linux sys目录 - https://blog.csdn.net/wade_510/article/details/72084006

---

Linux /proc和/sys目录详解_奔跑的胖小孩的博客-CSDN博客_linux sys目录 - https://blog.csdn.net/qq_38814358/article/details/110530398?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1.no_search_link&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1.no_search_link

/proc 目录
/proc 正在运行的内核信息映射，主要输出的信息为：进程信息、内存资源信息、磁盘分区信息等。

```
/proc下文件基本都是只读的，除了/proc/sys目录，它是可写的（查看和修改内核的运行参数）

/proc下数字命令的目录就是对于PID的进程目录

/proc/cmdline 启动时传递给kernel的参数信息（就是bootargs信息）
/proc/cpuinfo cpu的信息
/proc/crypto 内核使用的所有已安装的加密密码及细节
/proc/devices 已经加载的设备并分类
/proc/dma 已注册使用的ISA DMA频道列表
/proc/execdomains Linux 内核当前支持的execution domains
/proc/fb 帧缓冲设备列表，包括数量和控制它的驱动
/proc/filesystems 内核当前支持的文件系统类型
/proc/interrupts x86架构中的每个IRQ中断数
/proc/iomem 每个物理设备当前在系统内存中的映射
/proc/ioports 一个设备的输入输出所使用的注册端口范围
/proc/kcore 代表系统的物理内存，存储为核心文件格式，里边显示的是字节数，等于RAM大小加上4kb
/proc/kmsg 记录内核生成的信息，可以通过/sbin/klogd或/bin/dmesg来处理
/proc/loadavg 根据过去一段时间内CPU和IO的状态得出的负载状态，与uptime命令有关
/proc/locks 内核锁住的文件列表
/proc/mdstat 多硬盘，RAID配置信息(md=multiple disks)
/proc/meminfo RAM使用的相关信息
/proc/misc 其他的主要设备(设备号为10)上注册的驱动
/proc/modules 所有加载到内核的模块列表
/proc/mounts 系统中使用的所有挂载
/proc/partitions 分区中的块分配信息
/proc/pci 系统中的PCI设备列表
/proc/slabinfo 系统中所有活动的 slab 缓存信息
/proc/stat 所有的CPU活动信息
/proc/uptime 系统已经运行了多久
/proc/swaps 交换空间的使用情况
/proc/version Linux内核版本和gcc版本
/proc/bus 系统总线(Bus)信息，例如pci/usb等
/proc/driver 驱动信息
/proc/fs 文件系统信息
/proc/ide ide设备信息
/proc/irq 中断请求设备信息
/proc/net 网卡设备信息
/proc/scsi scsi设备信息
/proc/tty tty设备信息
/proc/net/dev 显示网络适配器及统计信息
/proc/vmstat 虚拟内存统计信息
/proc/vmcore 内核panic时的内存映像
/proc/diskstats 取得磁盘信息
/proc/schedstat kernel调度器的统计信息
/proc/zoneinfo 显示内存空间的统计信息，对分析虚拟内存行为很有用

以下是/proc目录中进程N的信息
/proc/N/cmdline 进程启动命令
/proc/N/cwd 链接到进程当前工作目录
/proc/N/environ 进程环境变量列表
/proc/N/exe 链接到进程的执行命令文件
/proc/N/fd 包含进程相关的所有的文件描述符 （ls /proc//fd | wc -l 查看某个进程打开多少FD）
/proc/N/maps 与进程相关的内存映射信息
/proc/N/mem 指代进程持有的内存，不可读
/proc/N/root 链接到进程的根目录
/proc/N/stat 进程的状态
/proc/N/statm 进程使用的内存的状态
/proc/N/status 进程状态信息，比stat/statm更具可读性
/proc/self 链接到当前正在运行的进程
```

/sys 目录
/sys 硬件设备的驱动程序信息

```
block bus class dev devices firmware fs kernel module power

/sys/devices （/sys文件系统最重要的目录结构）
该目录下是全局设备结构体系，包含所有被发现的注册在各种总线上的各种物理设备。
一般来说，所有的物理设备都按其在总线上的拓扑结构来显示，但有两个例外，
即platform devices和system devices。platform devices一般是
挂在芯片内部的高速或者低速总线上的各种控制器和外设，它们能被CPU直接寻址；
system devices不是外设，而是芯片内部的核心结构，比如CPU，timer等，
它们一般没有相关的驱动，但是会有一些体系结构相关的代码来配置它们。

/sys/dev
该目录下有字符设备(block)和块设备(char)两个子目录，里面全是以主次设备号(major:minor)命名
的链接文件，链接到/sys/devices。

/sys/class （按功能分类设备）
该目录下包含所有注册在kernel里面的设备类型，每个设备类型表达具有一种功能的设备。
每个设备类型子目录下是具体设备的符号链接，这些链接指向/sys/devices/…下的具体设备。
设备类型和设备并没有一一对应的关系，一个物理设备可能具备多种设备类型；
一个设备类型只表达具有一种功能的设备，比如：系统所有输入设备都会出现在/sys/class/input之下，
而不论它们是以何种总线连接到系统的。(/sys/class也是构成linux统一设备模型的一部分)

/sys/block （从linux2.6.26版本开始已经移到了/sys/class/block）
代表着系统中当前被发现的所有块设备。按照功能来说防止在/sys/class下会更合适，
但由于历史遗留因素而一直存在于/sys/block，但从linux2.6.22内核开始这部分就已经标记为过去时，
只有打开了CONFIG_SYSFS_DEPRECATED配置编译才会有这个目录存在，
并且其中的内容在从linux2.6.26版本开始已经正式移到了/sys/class/block，
旧的接口/sys/block为了向后兼容而保留存在，但其中的内容已经变为了指向它们在/sys/devices/中
真实设备的符号链接文件。

/sys/bus （按总线类型分类设备）
一般来说每个子目录(总线类型)下包含两个子目录，一个是devices，另一个是drivers；
其中devices下是这个总线类型下的所有设备，这些设备都是符号链接，
它们分别指向真正的设备(/sys/devices/…下)；而drivers下是所有注册在这个总线上的驱动，
每个driver子目录下 是一些可以观察和修改的driver参数。 (它也是构成linux统一设备模型的一部分)

/sys/module
该目录包含所有被载入Kernel的模块，无论这些模块是以内联(inlined)方式编译到内核映像文件中
还是编译为外模块(.ko文件)

/sys/fs
该目录用来描述系统中所有的文件系统，包括文件系统本身和按照文件系统分类存放的已挂载点。

/sys/kernel
该目录下存放的是内核中所有可调整的参数

/sys/firmware
该目录下包含对固件对象(firmware object)和属性进行操作和观察的接口，
即这里是系统加载固件机制的对用户空间的接口.(关于固件有专用于固件加载的一套API)

/sys/power
该目录下有几个属性文件可以用于控制整个机器的电源状态，如向其中写入控制命令让机器关机/重启等等。
```

---

