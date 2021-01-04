Linux C中的open函数_boyemachao的专栏-CSDN博客_linux open() - https://blog.csdn.net/boyemachao/article/details/91820647

```
flags分为两类：主类，副类
主类：O_RDONLY 以只读方式打开/O_WRONLY 以只写方式打开/O_RDWR 以可读可写方式打开
三这是互斥的
副类:
O_CREAT 如果文件不存在则创建该文件
O_EXCL 如果使用O_CREAT选项且文件存在，则返回错误消息
O_NOCTTY 如果文件为终端，那么终端不可以调用open系统调用的那个进程的控制终端
O_TRUNC 如果文件已经存在泽删除文件中原有数据
O_APPEND 以追加的方式打开
主副可以配合使用，例如:O_RDWR|O_CREAT|O_TRUNC
```

open函数中O_CLOEXEC标志的开关_cjsycyl的专栏-CSDN博客_o_cloexec - https://blog.csdn.net/cjsycyl/article/details/20920307

```
Linux中，文件描述符有一个属性：CLOEXEC，即当调用exec（）函数成功后，文件描述符会自动关闭。在以往的内核版本（2.6.23以前）中，需要调用 fcntl(fd, F_SETFD, FD_CLOEXEC) 来设置这个属性。而新版本（2.6.23开始）中，可以在调用open函数的时候，通过 flags 参数设置 CLOEXEC 功能，如 open(filename, O_CLOEXEC)。

      虽然新版本支持在open时设置CLOEXEC，但是在编译的时候还是会提示错误 - error: ‘O_CLOEXEC’ undeclared (first use in this function)。原来这个新功能要求我们手动去打开，需要设置一个宏(_GNU_SOURCE)。可通过以下两种方法来设置这个宏以打开新功能：

1. 在源代码中加入 #define _GNU_SOURCE
2. 在编译参数中加入 -D_GNU_SOURCE
```



---

【linux下整型原子操作】-博文推荐-CSDN博客 - https://www.csdn.net/gather_24/MtjaQg4sMTQwNi1ibG9n.html



getdtablesize()函数 Unix/Linux - Unix/Linux系统调用!(https://g.alicdn.com/dingding/desktop-assets/1.5.2/img/emoji/36x36/2122.png) - https://www.yiibai.com/unix_system_calls/getdtablesize.html

linux系统编程之进程（五）：exec系列函数（execl,execlp,execle,execv,execvp)使用 - mickole - 博客园 - https://www.cnblogs.com/mickole/p/3187409.html

stat函数详解_星星紫炎-CSDN博客 - https://blog.csdn.net/qq_40839779/article/details/82789217

Linux stat函数和stat命令 - 小石王 - 博客园 - https://www.cnblogs.com/xiaoshiwang/p/10764243.html

linux 下的clock_gettime() 获取时间函数 - 莘莘学子 - 博客园 - https://www.cnblogs.com/wangliangblog/p/9336276.html

Linux 下的KILL函数的用法 - 拂 晓 - 博客园 - https://www.cnblogs.com/leeming0222/articles/3994125.html

---

iniparser库使用_stone8761的专栏-CSDN博客 - https://blog.csdn.net/stone8761/article/details/81100747

解析配置文件.ini 的库

---

关于__GNU_SOURCE 这个宏 - 颠覆者 - 博客园 - https://www.cnblogs.com/raozhiyi/articles/9509600.html

(3条消息)关于waitpid()中的WNOHANG_searchwang的专栏-CSDN博客 - https://blog.csdn.net/searchwang/article/details/30803767

Linux串口—struct termios结构体【转】 - sky-heaven - 博客园 - https://www.cnblogs.com/sky-heaven/p/9675253.html

Linux下signal信号汇总 - 撒欢 - 博客园 - https://www.cnblogs.com/frisk/p/11602973.html

linux下core file size设置笔记 - 散尽浮华 - 博客园 - https://www.cnblogs.com/kevingrace/p/8243938.html

```
ulimit -c unlimited; ulimit -c 1024; 
echo "/skybell/misc/corefile-%e-%p-%t" > /proc/sys/kernel/core_pattern 
```



Linux下用文件IO的方式操作GPIO（/sys/class/gpio） - 嵌入式操作系统 - 博客园 - https://www.cnblogs.com/zym0805/p/5814351.html