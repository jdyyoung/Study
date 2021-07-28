//设计多个子线程，测试产生coredump文件
//多线程的core文件大小与什么有关？如何保存？如何用GDB 分析 core文件？
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