#! /bin/sh
### BEGIN INIT INFO
# File:				camera.sh	
# Provides:         camera service 
# Required-Start:   $
# Required-Stop:
# Default-Start:     
# Default-Stop:
# Short-Description:web service
# Author:			gao_wangsheng
# Email: 			gao_wangsheng@anyka.oa
# Date:				2012-8-8
### END INIT INFO

MODE=$1
PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin
mode=hostapd
network=
usage()
{
	echo "Usage: $0 start|stop)"
	exit 3
}

stop()
{
	killall -9 anyka_ipc
	echo "stopping ipc service......"
}

start ()
{
	echo "start ipc service......"
	if [ ! -d  "/tmp/aiwifi" ];then
		mkdir /tmp/aiwifi
	fi

	pid=`pgrep anyka_ipc`
    if [ "$pid" = "" ]
    then
	    anyka_ipc &
	fi
}

restart ()
{
	echo "restart ipc service......"
	stop
	start
}

cpu_overload ()
{
	echo "kill anyka_ipc service......"
	cmp_val=95.0
	val=`top -n1 | grep -v grep |  grep -w "anyka_ipc" | awk '{print $7}'`
	num=`echo $val | awk '{print $1}'`
	
	# echo "-------1---------num= ${num}---------------"
	if [ `expr $num \> $cmp_val` -eq 0 ];then
		echo "--------$cmp_val is bigger--------------"
	else
		echo "-------$num is bigger    ---------------"
		kill  -9 `ps | grep anyka_ipc | grep -v grep | awk '{print $1}'`
	fi
}

#
# main:
#

case "$MODE" in
	start)
		start
		;;
	stop)
		stop
		;;
	restart)
		restart
		;;
	cpu_overload)
		cpu_overload
		;;
	*)
		usage
		;;
esac
exit 0

