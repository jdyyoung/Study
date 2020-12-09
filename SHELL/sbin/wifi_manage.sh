#! /bin/sh
### BEGIN INIT INFO
# File:				wifi_manage.sh	
# Provides:         start wifi station and ap
# Required-Start:   $
# Required-Stop:
# Default-Start:     
# Default-Stop:
# Short-Description:start or stop wifi  
# Author:			
# Email: 			
# Date:				2015-3-5
### END INIT INFO

PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin
MODE=$1
cfgfile="/etc/jffs2/anyka_cfg.ini"

usage()
{
	echo "Usage: $0 start | stop | reset"
}

wifi_stop()
{
	killall -15 wifi_run.sh
	ifconfig wlan0 down
	/usr/sbin/wifi_station.sh stop
	/usr/sbin/wifi_driver.sh uninstall
	killall -15 wifi_station.sh
}

wifi_start(){
	/usr/sbin/wifi_run.sh
}

wifi_reset(){
	/usr/sbin/wifi_station.sh stop
	/usr/sbin/wifi_station.sh start
}

#
#main
#

case "$MODE" in
	start)
		wifi_start &
		;;
	stop)
		wifi_stop
		;;
	reset)
		wifi_reset
		;;
	*)
		usage
		;;
esac
exit 0

