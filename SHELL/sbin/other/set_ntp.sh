#!/bin/sh

var=`connmanctl state | grep "State" | awk -F= '{print $2}'`

netState=`echo $var`

ntpFlag=`cat /etc/syncTimeMode`

#echo "#######ntpFlag=$ntpFlag##########"

if [ $netState = "online" -a $ntpFlag = 0 ];then
	/etc/init.d/S49ntp start
	echo 1 > /etc/syncTimeMode
else
	echo "####netState is idle  #######"
fi
