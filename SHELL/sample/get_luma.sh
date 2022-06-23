#!/bin/sh

START_TIME=`date +%m%d%H%M`
LOG_FILE="/mnt/diskc/LV$1_get_luma-$START_TIME.log"
SLEEP_TIME=$2

date > $LOG_FILE
echo "---- LV$1 get luma value start ----" >> $LOG_FILE

#close IR-LED
#omfcmd_ -c shm0_cmd -p cmd=appXdc01SysParams,operation=0,key=1,value=1
#close RGB-LED
omfcmd_ -c shm0_cmd -p cmd=appXdc01SysParams,operation=0,key=1,value=6
times=0
while [ $times -lt $SLEEP_TIME ];do
	let times++;
	date >> $LOG_FILE
	omfcmd_ -c shm0_cmd -p cmd=appXdc01SysParams,operation=1,key=34 | grep data= | awk -F',' '{print $4}' >> $LOG_FILE
	sleep 2
done
echo "---- LV$1 get luma value end ----" >> $LOG_FILE