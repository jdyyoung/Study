

### **LINUX 多线程core文件设置与分析的学习**

```
//设计多个子线程，测试产生coredump文件
//多线程的core文件大小与什么有关？如何保存？如何用GDB 分析 core文件？
多线程的core文件大小与线程数及core_limits参数有关；
因此，coredump 文件可能会非常大，可保存在TF中；
分析 core文件：
利用GDB调试多线程core文件 - 程序员大本营 - https://www.pianshen.com/article/67061206730/
```

测试代码：

```C
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<time.h>
#include <pthread.h>
#include<unistd.h>
#include<errno.h> //ENOMEM
#include <sys/resource.h> //getrlimit(),setrlimit()

#define MAX_PATHNAME     512
#define PROG_VERSION     "1.0.0"
#define CORE_SAVE_PATH   "/tmp"
#define THREAD_NUM       5

typedef unsigned int u32;

int do_system(char* fmt, ...)
{
    va_list vargs;
    char cmd[MAX_PATHNAME*2];
    char* cmdp = cmd;
    int rc, error;

    va_start(vargs, fmt);
    rc = vsnprintf(cmd, sizeof(cmd), fmt, vargs);
    va_end(vargs);

    if (rc > sizeof(cmd)) {
        fprintf(stderr, "Using heap because cmdlen=%d and max=%ld!\n", rc, sizeof(cmd));
        cmdp = malloc(rc+8);
        if (cmdp == NULL) {
            return -ENOMEM;
        }

        va_start(vargs, fmt);
        vsnprintf(cmdp, rc+8, fmt, vargs);
        va_end(vargs);
    }

    errno = 0; 
    rc = system(cmdp);
    error = errno;

    if (rc == -1) {
        errno = error;
        fprintf(stderr, "do_system(%s): rc=%d, WIFEXITED(rc)=%d, WEXITSTATUS(rc)=%d", 
             cmdp, rc, error, WIFEXITED(rc), WEXITSTATUS(rc));
        if (cmd != cmdp) free(cmdp);
        return -error; 
    }

    // note: this is the "exit(#);" or "main(): return(#);" lower 8 bits of
    // the return code (>=0)
    if (WIFEXITED(rc) && WEXITSTATUS(rc)) {
        fprintf(stderr, "do_system(%s): WEXITSTATUS(%d)=%d", cmd, rc, WEXITSTATUS(rc));
        if (cmd != cmdp) free(cmdp);
        return WEXITSTATUS(rc);
    }

    if (cmd != cmdp) free(cmdp);
    return 0;
}

void CoreTest()
{
	char a[5]={'a','b','c','d','\0'};
	char* p =a;
	int i =0;
	for(i=0;i<5;i++)
	{
		printf("%p = [%c]\n", p+i, *(p+i));
		if(i>=2)
		{
			p=NULL;
		}
	}
}

static int set_core_limits(u32 dump_limit)
{
    struct rlimit core_limits;
    
    if (getrlimit(RLIMIT_CORE, &core_limits) < 0) {
        fprintf(stderr, "getrlimit(RLIMIT_CORE)");
        return -1;
    }

    if ((u32)core_limits.rlim_cur < dump_limit) {
        core_limits.rlim_cur = dump_limit;
        if ((u32)core_limits.rlim_max < dump_limit)
            core_limits.rlim_max = dump_limit;
        if (setrlimit(RLIMIT_CORE, &core_limits) < 0) {
            fprintf(stderr, "setrlimit(RLIMIT_CORE)");
            return -1;
        }
    } else {
        fprintf(stdout, "core_limits.rlim_cur(0x%x) > dump_limit(0x%x)\n",(u32)core_limits.rlim_cur, dump_limit);
    }

    return 0;
}
static void enable_core_dumps(u32 dump_limit, char* pathname)
{
    char core_pattern[32];

    if (set_core_limits(dump_limit))
        return;

    // the following magic will direct all core file dumps to the 'pathname'
    // directory, (probably /skybell/core) where "%e-%t-%s-%p" signifies:
    //     %e -- executable filename (e.g. skybell)
    //     %t -- time of dump (epoch seconds)
    //     %s -- number of signal causing dump
    //     %p -- PID of dumped process
 
    unlink(pathname);
    snprintf(core_pattern, sizeof(core_pattern), "%%e-v%s-%%s-%%t", PROG_VERSION);
    do_system("echo %s/%s > /proc/sys/kernel/core_pattern", pathname, core_pattern);
}

void* test_task(void* parm){
	int num = *(int*) parm;
	printf("thread num = %d,thread ID = %d\n",num,pthread_self());
	pthread_detach(pthread_self());
	while(1){
		sleep(2);
		int r = rand()%1000;
		r = r + num;
		if(r < 20){
			printf("----------num = %d,r = %d\n",num,r);
			CoreTest();
		}
	}
	
	return NULL;
}
//0x280000 = 2,621,440 = 2560k = 2.5M
int main(){
	srand(time(NULL));
	pthread_t test_Thread_ID[THREAD_NUM];

	enable_core_dumps(0x280000, CORE_SAVE_PATH);
	int i = 0;
	for(i=0;i<THREAD_NUM;i++){
		pthread_create(&test_Thread_ID[i], NULL, test_task, (void*)&i);
		sleep(1);
		printf("test_Thread_ID[%d] create OK!\n",i);
	}
	
	while(1){
		sleep(1);
	}
	return 0;
}
```
交叉编译器版本：

```
rck@rck:/vtcs/code/core_test$ arm-linux-gcc -v
Using built-in specs.
COLLECT_GCC=/opt/vtcs_toolchain/vienna/usr/bin/arm-linux-gcc.br_real
COLLECT_LTO_WRAPPER=/opt/vtcs_toolchain/vienna/usr/bin/../libexec/gcc/arm-buildroot-linux-uclibcgnueabihf/4.9.4/lto-wrapper
Target: arm-buildroot-linux-uclibcgnueabihf
Configured with: ./configure --prefix=/opt/vtcs_toolchain/vienna//usr --sysconfdir=/opt/vtcs_toolchain/vienna//etc --enable-static --target=arm-buildroot-linux-uclibcgnueabihf --with-sysroot=/opt/vtcs_toolchain/vienna//usr/arm-buildroot-linux-uclibcgnueabihf/sysroot --disable-__cxa_atexit --with-gnu-ld --disable-libssp --disable-multilib --with-gmp=/opt/vtcs_toolchain/vienna//usr --with-mpc=/opt/vtcs_toolchain/vienna//usr --with-mpfr=/opt/vtcs_toolchain/vienna//usr --with-pkgversion='Buildroot 2017.02.7' --with-bugurl=http://bugs.buildroot.net/ --disable-libquadmath --disable-libsanitizer --enable-tls --enable-plugins --enable-lto --disable-libmudflap --enable-threads --without-isl --without-cloog --disable-decimal-float --with-abi=aapcs-linux --with-cpu=cortex-a5 --with-fpu=neon-vfpv4 --with-float=hard --with-mode=arm --enable-languages=c,c++ --with-build-time-tools=/opt/vtcs_toolchain/vienna//usr/arm-buildroot-linux-uclibcgnueabihf/bin --enable-shared --disable-libgomp
Thread model: posix
gcc version 4.9.4 (Buildroot 2017.02.7) 
rck@rck:/vtcs/code/core_test$
```
编译：

```
#gcc core_test.c -lpthread -o core_test.arm
arm-linux-gcc core_test.c -lpthread -o core_test.arm
```

测试：

```
# /skybell/misc/core_test.arm 
thread num = 0,thread ID = -1225964336
test_Thread_ID[0] create OK!
thread num = 1,thread ID = -1228061488
test_Thread_ID[1] create OK!
thread num = 2,thread ID = -1230158640
test_Thread_ID[2] create OK!
thread num = 3,thread ID = -1232255792
test_Thread_ID[3] create OK!
thread num = 4,thread ID = -1234352944
test_Thread_ID[4] create OK!
MCU VERSION:v1.16
----------num = 4,r = 13
0xb66d3db0 = [a]
0xb66d3db1 = [b]
0xb66d3db2 = [c]
Segmentation fault (core dumped)
# 
# ls -lh /tmp/
total 588K   
drwxr-xr-x    3 root     root          60 Jan  1  1970 aenc
-rw-r--r--    1 root     root       90.5K Jun 25 17:30 aplist-result.txt
-rw-------    1 root     root       10.3M Jul 12 09:08 core_test.arm-v1.0.0-11-1626080880
```



----

分析：

利用GDB调试多线程core文件 - 程序员大本营 - https://www.pianshen.com/article/67061206730/

```
rck@rck:/vtcs/core$ arm-linux-gdb core_test.arm core_test.arm-v1.0.0-11-1626080880
GNU gdb (GDB) 7.11.1
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=arm-buildroot-linux-uclibcgnueabihf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from core_test.arm...done.

warning: exec file is newer than core file.
[New LWP 2322]
[New LWP 2316]
[New LWP 2318]
[New LWP 2319]
[New LWP 2320]
[New LWP 2321]

warning: Could not load shared library symbols for 2 libraries, e.g. /lib/libc.so.0.
Use the "info sharedlibrary" command to see the complete listing.
Do you need "set solib-search-path" or "set sysroot"?
Core was generated by `/skybell/misc/core_test.arm'.
Program terminated with signal SIGSEGV, Segmentation fault.
#0  0x00010b50 in CoreTest ()
[Current thread is 1 (LWP 2322)]
(gdb) bt
#0  0x00010b50 in CoreTest ()
#1  0x00010dbc in test_task ()
#2  0xb6fa862c in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) info threads
  Id   Target Id         Frame 
* 1    LWP 2322          0x00010b50 in CoreTest ()
  2    LWP 2316          0xb6eee158 in ?? ()
  3    LWP 2318          0xb6eee158 in ?? ()
  4    LWP 2319          0xb6eee158 in ?? ()
  5    LWP 2320          0xb6eee158 in ?? ()
  6    LWP 2321          0xb6eee158 in ?? ()
(gdb) thread apply all bt 

Thread 6 (LWP 2321):
#0  0xb6eee158 in ?? ()
#1  0xb6eee228 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 5 (LWP 2320):
#0  0xb6eee158 in ?? ()
#1  0xb6eee228 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 4 (LWP 2319):
#0  0xb6eee158 in ?? ()
#1  0xb6eee228 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 3 (LWP 2318):
#0  0xb6eee158 in ?? ()
#1  0xb6eee228 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 2 (LWP 2316):
#0  0xb6eee158 in ?? ()
#1  0xb6eee228 in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 1 (LWP 2322):
#0  0x00010b50 in CoreTest ()
#1  0x00010dbc in test_task ()
#2  0xb6fa862c in ?? ()
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) t 1
[Switching to thread 1 (LWP 2322)]
#0  0x00010b50 in CoreTest ()
(gdb) q
rck@rck:/vtcs/core$
```

