#! /bin/sh
### BEGIN INIT INFO
# File:				wifi_run.sh	
# Provides:         manage wifi station and smartlink
# Required-Start:   $
# Required-Stop:
# Default-Start:     
# Default-Stop:
# Short-Description:start wifi run at station or smartlink
# Author:			
# Email: 			
# Date:				2012-8-8
### END INIT INFO

PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin
MODE=$1
cfgfile="/etc/jffs2/anyka_cfg.ini"
notify_connect_wifi_file="/etc/jffs2/notify_connect_wifi.cfg"

#play_please_config_net()
#{
#	echo "play please config wifi tone"
#	ccli -t --f "/usr/share/anyka_please_config_net.mp3"
#	sleep 3
#}
#
#play_get_config_info()
#{
#	echo "play get wifi config tone"
#	ccli -t --f "/usr/share/anyka_camera_get_config.mp3"
#        sleep 3
#
#}
#
#play_afresh_net_config()
#{
#	echo "play please afresh config net tone"
#	ccli -t --f "/usr/share/anyka_afresh_net_config.mp3"
#}
#
#using_static_ip()
#{
#	ipaddress=`awk 'BEGIN {FS="="}/\[ethernet\]/{a=1} a==1&&$1~/^ipaddr/{gsub(/\"/,"",$2);gsub(/\;.*/, "", $2);gsub(/^[[:blank:]]*/,"",$2);print $2}' $cfgfile`
#	
#	netmask=`awk 'BEGIN {FS="="}/\[ethernet\]/{a=1} a==1&&$1~/^netmask/{gsub(/\"/,"",$2);gsub(/\;.*/, "", $2);gsub(/^[[:blank:]]*/,"",$2);print $2}' $cfgfile`
#	gateway=`awk 'BEGIN {FS="="}/\[ethernet\]/{a=1} a==1&&$1~/^gateway/{gsub(/\"/,"",$2);gsub(/\;.*/, "", $2);gsub(/^[[:blank:]]*/,"",$2);print $2}' $cfgfile`
#
#	ifconfig wlan0 $ipaddress netmask $netmask
#	route add default gw $gateway
#	sleep 1
#}
#
#
#write_mac_to_config_file(){
#	MAC=`ifconfig |grep HWaddr|awk '{print $5}' | sed 's/://g'`
#	old_mac=`cat /etc/jffs2/anyka_cfg.ini | grep mac | awk '{print $3}'`
#	if [ "$MAC" = "$old_mac" ];then
#		echo "mac=$MAC"
#	else
#		sed -i "s/mac = $old_mac/mac = $MAC/g" /etc/jffs2/anyka_cfg.ini
#	fi
#}
#
#station_install()
#{
#	### remove all wifi driver
#	/usr/sbin/wifi_driver.sh uninstall
#
#	## stop smartlink app
#
#	## install station driver
#	/usr/sbin/wifi_driver.sh station
#	i=0
#	###### wait until the wifi driver insmod finished.
#	while [ $i -lt 3 ]
#	do
#		if [ -d "/sys/class/net/wlan0" ];then
#			ifconfig wlan0 up
#			write_mac_to_config_file
#			break
#		else
#			sleep 1
#			i=`expr $i + 1`
#		fi
#	done
#	
#	if [ $i -eq 3 ];then
#		echo "wifi driver install error, exit"
#		return 1
#	fi
#
#	echo "wifi driver install OK"
#	return 0
#}
#
#station_connect()
#{
#	/usr/sbin/wifi_station.sh start
#	
#	pid=`pgrep wpa_supplicant`
#	if [ -z "$pid" ];then
#		echo "the wpa_supplicant init failed, exit start wifi"
#		return 1
#	fi
#
#	/usr/sbin/wifi_station.sh connect
#	ret=$?
#	echo "wifi connect return val: $ret"
#	if [ $ret -eq 0 ];then
#		if [ -d "/sys/class/net/eth0" ]
#		then
#			ifconfig eth0 down
#			ifconfig eth0 up
#		fi
#		/usr/sbin/led.sh blink 4000 200
#		echo "wifi connected!"
#		return 0
#	else
#		echo "[station start] wifi station connect failed"
#	fi
#
#	return $ret
#}
#
#
#station_start()
#{
#	if [ -d "/sys/class/net/wlan0" ];then
#		ifconfig wlan0 down
#		iwconfig wlan0 mode managed
#		ifconfig wlan0 up
#		write_mac_to_config_file
#	else
#		### remove all wifi driver
#		#/usr/sbin/wifi_driver.sh uninstall
#
#		## stop smartlink app
#
#		## install station driver
#		#/usr/sbin/wifi_driver.sh station
#		i=0
#		###### wait until the wifi driver insmod finished.
#		while [ $i -lt 5 ]
#		do
#			if [ -d "/sys/class/net/wlan0" ];then
#				ifconfig wlan0 up
#				write_mac_to_config_file &
#				break
#			else
#				sleep 1
#				i=`expr $i + 1`
#			fi
#		done
#	
#		if [ $i -eq 5 ];then
#			echo "wifi driver install error, exit"
#			return 1
#		fi
#
#		echo "wifi driver install OK"
#	fi
#
#	/usr/sbin/wifi_station.sh start
#	
#	pid=`pgrep wpa_supplicant`
#	if [ -z "$pid" ];then
#		echo "the wpa_supplicant init failed, exit start wifi"
#		return 1
#	fi
#
#	/usr/sbin/wifi_station.sh connect
#	ret=$?
#	echo "wifi connect return val: $ret"
#	if [ $ret -eq 0 ];then
#		echo "wifi connected!"
#		return 0
#	else
#		echo "[station start] wifi station connect failed"
#	fi
#
#	return $ret
#}
#
#smartlink_start()
#{
#	/usr/sbin/wifi_driver.sh uninstall
#	### start smartlink status led
#	#/usr/sbin/led.sh blink 1000 200
#	/usr/sbin/wifi_driver.sh smartlink
#}
#
#check_station_run()
#{
#	while true 
#	do
#		stat=`wpa_cli -iwlan0 status |grep wpa_state`
#		if [ "$stat" != "wpa_state=COMPLETED" ]
#		then	
#			echo station is unconnected	
#			#play_wifi_offline
#			return 0
#		fi
#	
#		sleep 1
#	done
#	return 1
#}
#
##main
#
#online=0
#ssid=`awk 'BEGIN {FS="="}/\[wireless\]/{a=1} a==1 && $1~/^ssid/{gsub(/\"/,"",$2);
#	gsub(/\;.*/, "", $2);gsub(/^[[:blank:]]*/,"",$2);print $2}' $cfgfile`
#
#if [ "$ssid" = "" ]
#then
#	#usb_serial & #start usb_serial here to burn tencent.conf
#
#	#sleep 3 #### sleep, wait the anyka_ipc load to the memery
#	#play_voice_flag=0
#	while true
#	do
#		#wait anyka_ipc play the voice
#		#smartlink_start
#		station_install
#
#		while true
#		do
#			if [ -e "/tmp/wireless/gbk_ssid" ];then		
#				play_get_config_info
#				station_start
#				if [ "$?" = 0 ];then
#					### start station status led
#					#/usr/sbin/led.sh blink 4000 200
#					online=1
#					break
#				else
#					echo "connect failed, ret: $?, please check your ssid and password !!!"
#					play_afresh_net_config
#					#### clean config file and re-config
#					cp /usr/local/factory_cfg.ini /etc/jffs2/anyka_cfg.ini	
#					rm -rf /tmp/wireless/
#					/usr/sbin/wifi_station.sh stop
#					break
#				fi
#			fi
#			if [ -e "/tmp/wifi_info" ];then
#				if [ -e "/tmp/test_mode" ];then
#					station_connect
#				else
#					station_start
#				fi
#				if [ "$?" = 0 ];then
#					### start station status led
#					#/usr/sbin/led.sh blink 4000 200
#					online=1
#					break
#				else
#					echo "connect failed, ret: $?, please check your ssid and password !!!"
#					play_afresh_net_config
#					#### clean config file and re-config
#					/usr/sbin/wifi_station.sh stop
#					break
#				fi
#			fi
#			sleep 1
#		done
#
#		if [ "$online" = "1" ];then
#			### station online
#			break
#		fi
#	done
#
#	#killall usb_serial
#	#rmmod g_file_storage
#else
#	station_start
#fi
#
#while true
#do
#	check_station_run
#
#	ifconfig wlan0 down
#	/usr/sbin/wifi_station.sh stop
#	killall -15 wifi_station.sh
#
#	if [ -e "/tmp/test_mode" ];then
#		station_connect
#	else
#		station_start
#	fi
#done

#==================================================20190325:young@mocn.cc============================
connect_wifi_tip=1
startup_flag=TRUE
notify_connect_wifi=FALSE

write_mac_to_config_file(){
	MAC=`ifconfig |grep HWaddr|awk '{print $5}' | sed 's/://g'`
	old_mac=`cat /etc/jffs2/anyka_cfg.ini | grep mac | awk '{print $3}'`
	#echo "###########MAC=$MAC######old_mac=$old_mac###########"
	if [ "$MAC" = "$old_mac" ];then
		echo "mac=$MAC"
	else
		sed -i "s/$old_mac/$MAC/g" /etc/jffs2/anyka_cfg.ini
		#cat /etc/jffs2/anyka_cfg.ini | grep mac
	fi
}

station_start()
{
	if [ -d "/sys/class/net/wlan0" ];then
		ifconfig wlan0 down
		iwconfig wlan0 mode managed
		ifconfig wlan0 up
		write_mac_to_config_file
	else
		i=0
		###### wait until the wifi driver insmod finished.
		while [ $i -lt 5 ]
		do
			if [ -d "/sys/class/net/wlan0" ];then
				ifconfig wlan0 up
				write_mac_to_config_file &
				break
			else
				sleep 1
				i=`expr $i + 1`
			fi
		done
	
		if [ $i -eq 5 ];then
			echo "wifi driver install error, exit"
			return 1
		fi

		echo "wifi driver install OK"
	fi

	/usr/sbin/wifi_station.sh start
	if [ $connect_wifi_tip -eq 1 ];then
		connect_wifi_tip=0
		#usleep 1500000
		#ccli -t --f "/usr/share/startup_net_connecting.mp3" &
	fi

	pid=`pgrep wpa_supplicant`
	if [ -z "$pid" ];then
		echo "the wpa_supplicant init failed, exit start wifi"
		return 1
	fi

	/usr/sbin/wifi_station.sh connect

	ret=$?
	echo "wifi connect return val: $ret"
	if [ $ret -eq 0 ];then
		echo "wifi connected!"
		return 0
	else
		echo "[station start] wifi station connect failed"
	fi

	return $ret
}

check_station_run()
{
	stat=`wpa_cli -iwlan0 status |grep wpa_state`
	if [ "$stat" != "wpa_state=COMPLETED" ]
	then	
		echo station is unconnected	
		return 0
	fi
	return 1
}

wifi_run_connect()
{
	cnt=0
	while [ $cnt -lt 3 ];do
		station_start
		check_station_run
		if [ $? -eq 1 ];then
			break
		fi
		
		ifconfig wlan0 down
		/usr/sbin/wifi_station.sh stop
		killall -15 wifi_station.sh

		cnt=`expr $cnt + 1`
	done
	if [ $cnt -eq 3 ];then
			echo "wifi connect fail, exit"
			return 1
	fi
	return 0
}

#main
ssid=`awk 'BEGIN {FS="="}/\[wireless\]/{a=1} a==1 && $1~/^ssid/{gsub(/\"/,"",$2);
	gsub(/\;.*/, "", $2);gsub(/^[[:blank:]]*/,"",$2);print $2}' $cfgfile`

is_first_startup=`awk 'BEGIN {FS="="}/\[global\]/{a=1} a==1 && $1~/^is_first_startup/{gsub(/\"/,"",$2);
	gsub(/\;.*/, "", $2);gsub(/^[[:blank:]]*/,"",$2);print $2}' $cfgfile`
#echo "####################is_first_startup=$is_first_startup#######################"
while true
do

	notify_connect_wifi=`cat  $notify_connect_wifi_file`

	#如果是第一次开机, 不用去联网
	if [ "$is_first_startup" = "FIRST_YES" ];then
		startup_flag=FALSE
		is_first_startup=FIRST_NO
		#开机联网结束发送199 或 220：STARTUP_NOTIFY_END
		# ccli --key 199
		continue
	fi

	#echo "notify_connect_wifi=$notify_connect_wifi ======================="

	if [[ "$startup_flag" = "TRUE" || "$notify_connect_wifi" = "WIFI_TRUE" ]];then
		echo "######startup_flag=$startup_flag########notify_connect_wifi=$notify_connect_wifi############"
		usleep 10000
		sed -i "s/WIFI_TRUE/WIFI_FALSE/g" $notify_connect_wifi_file
		wifi_run_connect
		if [ $? -eq 0 ];then
			#wifi conncet OK
			echo "#######################wifi conncet OK#######################"
			# ccli -t --f "/usr/share/net_connect_ok.mp3" &
			# if [ "$startup_flag" = "TRUE" ];then
			# 	#开机联网结束发送199 或 220：STARTUP_NOTIFY_END
			# 	ccli --key 199
			# fi
		else
			#wifi connect fail,TODO：如果是配置网络失败，不用播放一下提示音
			if [ "$startup_flag" = "TRUE" ];then
				#开机没网发送220：STARTUP_NOTIFY_STORY,如果没有配网就播故事机，否则不能播故事机
				ccli --key 220
			fi
		fi
		startup_flag=FALSE
	fi
	usleep 10000
done

