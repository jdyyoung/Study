#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #include "log.h"
#include "recovery.h"

#ifndef LOG_SAVE_DIR
#define LOG_SAVE_DIR "/tmp"
#endif

int main(){
	log_init_ext(LOG_SAVE_DIR);
	
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
