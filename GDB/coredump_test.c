#include<stdio.h>
#include<stdlib.h>
#include <sys/resource.h>

#define SKYBELL_CORE  "/skybell/misc/core"
typedef  unsigned int u32;

int set_core_limits(u32 dump_limit)
{
    struct rlimit core_limits;
    
    if (getrlimit(RLIMIT_CORE, &core_limits) < 0) {
        printf("getrlimit(RLIMIT_CORE)");
        return -1;
    }

    if ((u32)core_limits.rlim_cur < dump_limit) {
        core_limits.rlim_cur = dump_limit;
        if ((u32)core_limits.rlim_max < dump_limit)
            core_limits.rlim_max = dump_limit;
        if (setrlimit(RLIMIT_CORE, &core_limits) < 0) {
            printf("setrlimit(RLIMIT_CORE)");
            return -1;
        }
    } else {
        printf("core_limits.rlim_cur(0x%x) > dump_limit(0x%x)\n",
              (unsigned int)core_limits.rlim_cur, (unsigned int)dump_limit);
    }

    return 0;
}

void enable_core_dumps(u32 dump_limit, char* pathname)
{
    // char core_pattern[32];

    if (set_core_limits(dump_limit))
        return;

    // the following magic will direct all core file dumps to the 'pathname'
    // directory, (probably /skybell/core) where "%e-%t-%s-%p" signifies:
    //     %e -- executable filename (e.g. skybell)
    //     %t -- time of dump (epoch seconds)
    //     %s -- number of signal causing dump
    //     %p -- PID of dumped process
    // 
    // Note: the core file dump process will not create directories, and the
    // existing directories must be writable (and obviously there must be
    // sufficient space in the filesystem).  The one problem with the current
    // approach is that it can potentially fill up the file system because each
    // core file has a unique name -- in the production system we may want to
    // just specify the name of the executable and maybe the signal number so
    // multiple dumps will overwrite the previous core file -- this could be
    // qualified with the 'development' boolean to cause the two distinct
    // behaviors.
    // 
    unlink(pathname);
    // snprintf(core_pattern, sizeof(core_pattern), "%%e-v%s-%%s-%%t", PROG_VERSION);
    system("echo /skybell/misc/core/%e-v20201025-%s-%t > /proc/sys/kernel/core_pattern");
}

static void CoreTest()
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
int main(){
	enable_core_dumps(0x580000, SKYBELL_CORE);
    CoreTest();
	return 0;
}