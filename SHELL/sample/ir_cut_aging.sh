#!/bin/sh


echo "user_start.sh start...."

omfcmd_ -c shm0_cmd -p cmd=appXdc01SysParams,operation=0,key=4,value=1
rm /mnt/diskc/ir_cut_aging.test
date > /mnt/diskc/ir_cut_aging.test
TEST_TIME=$1

while [ $TEST_TIME != 0 ];do
let TEST_TIME--
omfcmd_ -c shm0_cmd -p cmd=appXdc01SysParams,operation=0,key=5,value=1
echo "set ir_cut on " >> /mnt/diskc/ir_cut_aging.test
sleep 1
omfcmd_ -c shm0_cmd -p cmd=appXdc01SysParams,operation=0,key=5,value=0
echo "set ir_cut off " >> /mnt/diskc/ir_cut_aging.test
sleep 1
done


echo "user_start.sh ending...."
