

```C
//返回上电到现在时间多少秒
time_t monotonic_time(void){
    struct timespec ts;
    return clock_gettime(CLOCK_MONOTONIC, &ts) == 0? ts.tv_sec : 0;
}
//返回上电到现在时间多少毫秒
i64 msec_monotonic_time(void){
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        i64 msecs = ts.tv_sec;
        return (msecs*1000) + (ts.tv_nsec/1000000);
    }
    return 0;
}
```



---

gdb调试多线程 - 程序员大本营 - https://www.pianshen.com/article/9902751244/

(4条消息) 利用GDB调试多线程core文件_shaomingliang499的博客-CSDN博客 - https://blog.csdn.net/shaomingliang499/article/details/50536796/?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_baidulandingword-0&spm=1001.2101.3001.4242

(4条消息) GDB调试多线程和多进程以及Core文件_icepeplo的博客-CSDN博客 - https://blog.csdn.net/icepeplo/article/details/100580300?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_title-0&spm=1001.2101.3001.4242

2021-06-16:

在Linux中如何利用backtrace信息解决程序崩溃的问题_海阔天空的专栏-CSDN博客_backtrace - https://blog.csdn.net/gongmin856/article/details/79192259

linux 多线程段错误,关于linux的段错误(Segmentation fault)_老少女王烦烦的博客-CSDN博客 - https://blog.csdn.net/weixin_35081955/article/details/116795427?utm_term=%E5%A4%9A%E7%BA%BF%E7%A8%8Blinux%E6%AE%B5%E9%94%99%E8%AF%AF&utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~all~sobaiduweb~default-1-116795427&spm=3001.4430

利用core文件对多线程和多进程进行调试_ZHY_xz的博客-CSDN博客 - https://blog.csdn.net/qq_35118894/article/details/72851733

利用GDB调试多线程core文件 - 程序员大本营 - https://www.pianshen.com/article/67061206730/

如何使用GDB调试多线程 - 程序员大本营 - https://www.pianshen.com/article/32211826367/

----

2021-02-26:

静态库与动态库生成与使用：

```
(2)静态库的使用步骤
   a.编写测试源代码(xxx.c)
     vi main.c文件
   b.只编译不链接生成目标文件(xxx.o)
     gcc/cc -c main.c
   c.链接测试文件和静态库文件，链接的方式有三种:
     1)直接链接
       cc main.o libadd.a
     2)使用编译选项进行链接(掌握)
       gcc/cc main.o -l 库名 -L 库文件所在的路径
       gcc/cc main.o -l add -L .
     3)配置环境变量LIBRARY_PATH
       export LIBRARY_PATH=$LIBRARY_PATH:.
       gcc/cc main.o -l add

2.3 共享库的生成和使用步骤
(1)共享库的生成步骤
   a.编写源代码(xxx.c)
     vi add.c文件
   b.只编译不链接生成目标文件(xxx.o)
     gcc/cc -c -fpic/*小模式,代码少*/ add.c
   c.生成共享库文件
     gcc/cc -shared 目标文件(xxx.o) -o lib库名.so
     gcc/cc -shared/*共享*/ add.o -o libadd.so
(2)共享库的使用步骤
   a.编写测试源代码(xxx.c)
     vi main.c文件
   b.只编译不链接生成目标文件(xxx.o)
     gcc/cc -c main.c
   c.链接测试文件和共享库文件，链接的方式有三种:
     1)直接链接
       cc main.o libadd.so
     2)使用编译选项进行链接(掌握)
       gcc/cc main.o -l 库名 -L 库文件所在的路径
       gcc/cc main.o -l add -L .
     3)配置环境变量LIBRARY_PATH
       export LIBRARY_PATH=$LIBRARY_PATH:.
       gcc/cc main.o -l add

注意：
   共享库的使用要求配置环境变量LD_LIBRARY_PATH的值，主要解决运行时找不到共享库的问题
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
```

注意：C++ 调用纯C的动态库，在-L与-l 都正确时还会出现报错：

```
undefined reference to `ex_log_init(char const*)'
```

解决方法：

C++项目中的extern "C" {} - 吴秦 - 博客园 - https://www.cnblogs.com/skynet/archive/2010/07/10/1774964.html

```C
#ifdef __cplusplus
extern "C" {
#endif
 
/*...*/
 
#ifdef __cplusplus
}
#endif
```

---



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



----



```
sem_t play_sem;
sem_init()
sem_post()
sem_wait()
sem_destroy()
```

