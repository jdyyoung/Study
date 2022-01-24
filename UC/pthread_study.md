

20210906：

```c
void thread_default_priority(THREAD* thread)
{
    // sets the thread priority to round-robin, non realtime
    struct sched_param sched = {.sched_priority = 0};
    int rc = pthread_setschedparam(thread->thread, SCHED_OTHER, &sched);
    if (rc != 0) {
        ERROR(errno, "pthread_setschedparam() returned %d\n", rc);
    }
}


int thread_setpriority(THREAD* thread, int priority)
{
    struct sched_param sched;
    int policy, rc;

    // sets the thread priority to round-robin, realtime
    rc = pthread_getschedparam(thread->thread, &policy, &sched);
    if (rc != 0) {
        ERROR(errno, "pthread_getschedparam() returned %d\n", rc);
        return -1;
    }

    ALERT(0, "thread-%s: policy=%d, priority=%d --> %d\n", thread->name, policy, sched.sched_priority, priority);
    sched.sched_priority = priority;
    rc = pthread_setschedparam(thread->thread, SCHED_RR, &sched);
    if (rc != 0) {
        ERROR(errno, "pthread_setschedparam() returned %d\n", rc);
        return -1;
    }

    return 0;
}
```



//获取或设置线程优先级

```
#include <pthread.h>
struct sched_param {
       int sched_priority;     /* Scheduling priority 调度优先级*/
};

pthread_setschedparam(pthread_t thread, int policy,const struct sched_param *param);
pthread_getschedparam(pthread_t thread, int *policy,struct sched_param *param);

SCHED_OTHER   the standard round-robin time-sharing policy;
SCHED_RR      a round-robin policy.//轮询策略
```



```
void thread_dump(void)
{
    int i;

    MUTEX_LOCK(&thread_pool_mutex);
    for (i = 0; i < thread_count; ++i) {
        THREAD* thread = thread_pool + i;
        if (thread->handler && thread->pid && thread->name[0]) {
            struct rusage usage;
            int rc = getrusage(/*RUSAGE_THREAD*/1, &usage);
            DBUG(0,"%d:%s utime(%ld.%ld) stime(%ld.%ld) [rc=%d]\n",
                   thread->pid, thread->name, 
                   usage.ru_utime.tv_sec, usage.ru_utime.tv_usec, 
                   usage.ru_stime.tv_sec, usage.ru_stime.tv_usec, rc);
        }
    }
    MUTEX_UNLOCK(&thread_pool_mutex);
}
```

```
SYNOPSIS
       #include <sys/time.h>
       #include <sys/resource.h>

       int getrusage(int who, struct rusage *usage);

DESCRIPTION
       getrusage() returns resource usage measures for who, which can be one of the following:

       RUSAGE_SELF
              Return resource usage statistics for the calling process, which is the sum of resources used by all threads in the process.

       RUSAGE_CHILDREN
              Return  resource  usage  statistics for all children of the calling process that have terminated and been waited for.  These statistics will include the resources used by grandchildren,
              and further removed descendants, if all of the intervening descendants waited on their terminated children.

       RUSAGE_THREAD (since Linux 2.6.26)
              Return resource usage statistics for the calling thread.

       The resource usages are returned in the structure pointed to by usage, which has the following form:

           struct rusage {
               struct timeval ru_utime; /* user CPU time used */
               struct timeval ru_stime; /* system CPU time used */
               long   ru_maxrss;        /* maximum resident set size */
               long   ru_ixrss;         /* integral shared memory size */
               long   ru_idrss;         /* integral unshared data size */
               long   ru_isrss;         /* integral unshared stack size */
               long   ru_minflt;        /* page reclaims (soft page faults) */
               long   ru_majflt;        /* page faults (hard page faults) */
               long   ru_nswap;         /* swaps */
               long   ru_inblock;       /* block input operations */
               long   ru_oublock;       /* block output operations */
               long   ru_msgsnd;        /* IPC messages sent */
               long   ru_msgrcv;        /* IPC messages received */
               long   ru_nsignals;      /* signals received */
               long   ru_nvcsw;         /* voluntary context switches */
               long   ru_nivcsw;        /* involuntary context switches */
           };

```



```
static void create_thread(THREAD* thread)
{
    if (pthread_cond_init(&thread->ready, NULL) != 0)
        ERROR(errno, "problem trying to initialize thread ready cond, error '%m'\n");

    if (pthread_create(&thread->thread, &join_attr, thread_loop, thread) != 0)
        ERROR(errno, "pthread_create(): '%m'\n");
}

pthread_kill(thread->thread, SIGUSR1);
pthread_sigmask(SIG_UNBLOCK, &signal_set, NULL);
pthread_create(&thread->thread, &join_attr, thread_loop, thread)
pthread_attr_init(&join_attr);
pthread_attr_setdetachstate(&join_attr, PTHREAD_CREATE_JOINABLE);
pthread_join(thread->thread, (void **)&status);
pthread_attr_destroy(&join_attr);


pthread_cond_init(&thread->ready, NULL)
pthread_cond_signal(&thread->ready);
pthread_cond_wait(&thread->ready, &thread->mutex.mutex);
```

```
pthread_cond_init(&q->cond, NULL);
pthread_cond_destroy(&q->cond);
pthread_cond_broadcast(&q->cond);
pthread_cond_wait(&q->cond, &q->lock.mutex);
pthread_cond_timedwait(&q->cond, &q->lock.mutex, timeout);
```

```
信号量
介绍一下POSIX(POSIX标准定义了操作系统应该为应用程序提供的接口标准，换句话说，为一个POSIX兼容的操作系统编写的程序，应该可以在任何其它的POSIX操作系统（即使是来自另一个厂商）上编译执行。)的信号量机制，定义在头文件/usr/include/semaphore.h
1）初始化一个信号量:sem_init()
int sem_init(sem_t* sem,int pshared,unsigned int value);
pshared为0时表示该信号量只能在当前进程的线程间共享，否则可以进程间共享，value给出了信号量的初始值。
2)阻塞线程
sem_wait(sem_t* sem)直到信号量sem的值大于0，解除阻塞后将sem的值减一，表明公共资源经使用后减少;sem_trywait(sem_t* sem)是wait的非阻塞版本，它直接将sem的值减一，相当于P操作。
3)增加信号量的值，唤醒线程
sem_post(sem_t* sem)会使已经被阻塞的线程其中的一个线程不再阻塞，选择机制同样是由线程的调度策略决定的。相当于V操作。
4)释放信号量资源
sem_destroy(sem_t* sem)用来释放信号量sem所占有的资源
```



----

Linux下C的线程同步机制 - 0giant - 博客园
https://www.cnblogs.com/LUO77/p/5754633.html

简单Linux C线程池 - venow - 博客园
https://www.cnblogs.com/venow/archive/2012/11/22/2779667.html

Linux 线程挂起与唤醒功能 实例 - 吴英强的技术博客 - CSDN博客
https://blog.csdn.net/waldmer/article/details/23422943

//信号量：
pthread_create()
pthread_self()
pthread_join()
pthread_detach()

//---------------------------2019-04-07-------------------------------------------
void pthread_cleanup_push(void(*rtn)(void*), void *arg);
void pthread_cleanup_pop(int execute); //调用删除上次push的清理程序
int pthread_cancel(pthread_t tid);
int pthread_kill(pthread_t thread, int sig);

在linux操作系统中可以使用ps -eLf命令来查看线程信息


Linux编程——终止线程的正常方式及取消点 - 落尘纷扰的专栏 - CSDN博客 - https://blog.csdn.net/jasonchen_gbd/article/details/78674716

Linux 多线程环境下 进程线程终止函数小结 - as_ - 博客园 - https://www.cnblogs.com/biyeymyhjob/archive/2012/10/11/2720377.html

Linux线程基础函数 - 某精神病 - 博客园 - https://www.cnblogs.com/wanpengcoder/p/5331010.html

//------------------------------2019-04-12------------------------------------
pthread_cancel,pthread_killall 段错误 - wwliu的个人页面 - 开源中国 - https://my.oschina.net/lwwklys/blog/79013?p={{currentPage-1}}

Linux多线程编程时如何查看一个进程中的某个线程是否存活 - fei的专栏 - CSDN博客 - https://blog.csdn.net/weiyuefei/article/details/54092268

pthread_kill引发的争论 - 简书 - https://www.jianshu.com/p/756240e837dd

Linux查看某个进程的线程 - EasonJim - 博客园 - https://www.cnblogs.com/EasonJim/p/8098217.html
ps -eL //查看线程

---