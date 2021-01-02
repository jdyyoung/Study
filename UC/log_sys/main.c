#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
// #include "recovery.h"



static void log_restore(void) 
{
    system_log = log_restart("system", 0x20000ul, 10, TRUE);
    debug_log = log_restart("debug", 0x80000ul, 10, TRUE);
}

int main(){
	log_init(SKYBELL_MISC, log_restore);
	
	while(1){
		DBUG(0,"log system test:%s-%d\n",__func__,__LINE__);
		sleep(1);
		ALERT(0,"log system test:%s-%d\n",__func__,__LINE__);
		sleep(1);
		WARN(0,"log system test:%s-%d\n",__func__,__LINE__);
		sleep(1);
		ERROR(0,"log system test:%s-%d\n",__func__,__LINE__);
		sleep(1);
	}
	return 0;
}