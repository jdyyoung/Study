#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #include "log.h"
#include "recovery.h"

#ifndef LOG_SAVE_DIR
#define LOG_SAVE_DIR "/tmp"
#endif

static void d3_log_init(){
	LOG_DEVICE_INFO d3_param = {0};
	// d3_param.fw_ver = D3_FWVER;
	// d3_param.hw_ver = d3_camera_config.McuAppV;  
	// d3_param.mcu_ver = d3_camera_config.HwVersion;
	// strcpy(d3_param.device_sn,d3_camera.device_sn);
	// strcpy(d3_param.device_mac,d3_camera.device_mac);

	d3_param.fw_ver = 1001;
	d3_param.hw_ver = 101;  
	d3_param.mcu_ver = 11;
	strcpy(d3_param.device_sn,"0123456789");
	strcpy(d3_param.device_mac,"00:11:22:33:44:55");
	d3_param.bytes_per_file = 0x80000ul;
	d3_param.max_files = 10;
	DBUG(0,"fw_ver =%d,hw_ver = %d, mcu_ver = %d,device_sn =%s,device_mac=%s",d3_param.fw_ver,d3_param.hw_ver,d3_param.mcu_ver,d3_param.device_sn,d3_param.device_mac);
	log_init_ext(LOG_SAVE_DIR,&d3_param);
	printf("%s:%d-------------------------------------------------------\n",__func__,__LINE__);
}

int main(){

	// LOG_DEVICE_INFO d3_param = {0};
	// d3_param.fw_ver = 1001;
	// d3_param.hw_ver = 101;  
	// d3_param.mcu_ver = 11;
	// strcpy(d3_param.device_sn,"0123456789");
	// strcpy(d3_param.device_mac,"00:11:22:33:44:55");

	// log_init_ext(LOG_SAVE_DIR,&d3_param);
	d3_log_init();
	system_log_level_update(XLOG_VERBOSE);
	system_console_level_update(XLOG_VERBOSE);
	int i = 0;
	while(1){
		DBUG(0,"log system test:i=%d\n",i);
		DBUG(1,"1111-log system test:i=%d\n",i);
		sleep(1);
		ALERT(0,"log system test:i=%d\n",i);
		sleep(1);
		WARN(0,"log system test:i=%d\n",i);
		sleep(1);
		ERROR(0,"log system test:i=%d\n",i);
		sleep(1);
		i++;
		switch (i / 10)
		{
		case 1:
			system_log_level_update(XLOG_DEBUG);
			system_console_level_update(XLOG_DEBUG);	
			DBUG(0,"---------set level is DEBUG------\n");
			DBUG(11111,"11111---------set level is DEBUG------\n");
			ALERT(0,"---------set level is DEBUG------\n");
			WARN(0,"---------set level is DEBUG------\n");
			ERROR(0,"---------set level is DEBUG------\n");
			printf("---------set level is DEBUG------\n");
			break;
		case 2:
			system_log_level_update(XLOG_INFO);
			system_console_level_update(XLOG_INFO);
			DBUG(0,"---------set level is INFO------\n");
			DBUG(1,"11111---------set level is INFO------\n");
			ALERT(0,"---------set level is INFO------\n");
			WARN(0,"---------set level is INFO------\n");
			ERROR(0,"---------set level is INFO------\n");
			printf("---------set level is INFO------\n");
			break;
		case 3:
			system_log_level_update(XLOG_WARN);
			system_console_level_update(XLOG_WARN);
			DBUG(0,"---------set level is XLOG_WARN------\n");
			DBUG(1,"11111---------set level is XLOG_WARN------\n");
			ALERT(0,"---------set level is XLOG_WARN------\n");
			WARN(0,"---------set level is XLOG_WARN------\n");
			ERROR(0,"---------set level is XLOG_WARN------\n");
			printf("---------set level is XLOG_WARN------\n");
			break;
		case 4:
			system_log_level_update(XLOG_ERROR);
			system_console_level_update(XLOG_ERROR);
			DBUG(0,"---------set level is XLOG_ERROR------\n");
			DBUG(1,"11111---------set level is XLOG_ERROR------\n");
			ALERT(0,"---------set level is XLOG_ERROR------\n");
			WARN(0,"---------set level is XLOG_ERROR------\n");
			ERROR(0,"---------set level is XLOG_ERROR------\n");
			printf("---------set level is XLOG_ERROR------\n");
			break;
		case 5:
			system_log_level_update(XLOG_NONE);
			system_console_level_update(XLOG_NONE);
			DBUG(0,"---------set level is XLOG_NONE------\n");
			ALERT(0,"---------set level is XLOG_NONE------\n");
			WARN(0,"---------set level is XLOG_NONE------\n");
			ERROR(0,"---------set level is XLOG_NONE------\n");
			printf("---------set level is XLOG_NONE------\n");
			return 0;
		default:
			break;
		}
	}
	return 0;
}
