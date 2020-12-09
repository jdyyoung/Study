#! /bin/sh
### BEGIN INIT INFO
# File:				service.sh
# Provides:         init service
# Required-Start:   $
# Required-Stop:
# Default-Start:
# Default-Stop:
# Short-Description:web service
# Author:			gao_wangsheng
# Email: 			gao_wangsheng@anyka.oa
# Date:				2012-12-27
### END INIT INFO

MODE=$1
TEST_MODE=0
PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin
cfgfile="/etc/jffs2/anyka_cfg.ini"

usage()
{
	echo "Usage: $0 start|stop)"
	exit 3
}

stop_service()
{
	killall -12 daemon
	echo "watch dog closed"
	sleep 5
	killall daemon
	killall cmd_serverd

	/usr/sbin/anyka_ipc.sh stop

	echo "stop network service......"
}

start_service ()
{
	cmd_serverd &
	if [ -e "/etc/jffs2/yc_product_test" ];then
		echo "####################will into YC product test###############################"
		insmod /usr/modules/udc.ko
		insmod /usr/modules/g_ether.ko
		sleep 1
		/usr/sbin/eth_manage.sh start
		/usr/sbin/wifi_driver.sh station
		sleep 1
		ifconfig wlan0 up
		/etc/jffs2/yc_product_test &
		# /usr/sbin/product_test &
		return 
	fi

	daemon 
	
	/usr/sbin/anyka_ipc.sh start 
	/usr/sbin/wifi_station.sh start &
	uart_BPS4800_test -k 

	is_first_startup=`grep -w network /etc/jffs2/wpa_supplicant.conf | wc -l`
	echo "##############is_first_startup = $is_first_startup#############"
	if [ "$is_first_startup" != "0" ];then
		volume_value=`get_volume`
		echo "##############no is_first_startup:volume_value=$volume_value#############"
		ak_adec_demo 16000 1 mp3 /usr/share/startup_net_connecting.mp3 $volume_value
	else
		echo "$0 :this is first_startup ========================="
	fi
	#/usr/sbin/save_log.sh &

	#echo "start net service......"
	#/usr/sbin/net_manage.sh &
	#/usr/sbin/wifi_manage.sh start
}

restart_service ()
{
	echo "restart service......"
	stop_service
	start_service
}

#
# main:
#

case "$MODE" in
	start)
		start_service
		;;
	stop)
		stop_service
		;;
	restart)
		restart_service
		;;
	*)
		usage
		;;
esac
exit 0

