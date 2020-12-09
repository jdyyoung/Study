#! /bin/sh
### BEGIN INIT INFO
# File:				station_connect.sh	
# Description:      wifi station connect to AP 
# Author:			gao_wangsheng
# Email: 			gao_wangsheng@anyka.oa
# Date:				2012-8-2
### END INIT INFO

MODE=$1
# GSSID="$2"
# SSID=\'\"$GSSID\"\'
# GPSK="$3"
# PSK=\'\"$GPSK\"\'
KEY=$PSK
KEY_INDEX=$4
KEY_INDEX=${KEY_INDEX:-0}
NET_ID=
PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin

usage()
{
	echo "Usage: $0 mode(wpa|wep|open) ssid password"
	exit 3
}

check_ip_and_start()
{
	echo "check ip and start"
          
	status=
	i=0
	while [ $i -lt 2 ]
	do
		echo "using dynamic ip ..."
		killall udhcpc
		udhcpc -b -i wlan0
		sleep 1

		status=`ifconfig wlan0 | grep "inet addr:"`
		if [ "$status" != "" ];then
			break
		fi
		i=`expr $i + 1`
	done
	
	if [ $i -eq 2 ];then
		echo "[WiFi Station] fails to get ip address"
		return 1
	fi

	return 0
}

refresh_net()
{
	#### remove all connected netword
	while true
	do
		NET_ID=`wpa_cli -iwlan0 list_network\
			| awk 'NR>=2{print $1}'`

		if [ -n "$NET_ID" ];then
			wpa_cli -p/var/run/wpa_supplicant remove_network $NET_ID
		else
			break
		fi
	done
	wpa_cli -p/var/run/wpa_supplicant ap_scan 1
}

# write_mac_to_config_file(){
# 	MAC=`ifconfig |grep HWaddr|awk '{print $5}' | sed 's/://g'`
# 	old_mac=`cat /etc/jffs2/anyka_cfg.ini | grep mac | awk '{print $3}'`
# 	#echo "###########MAC=$MAC######old_mac=$old_mac###########"
# 	if [ "$MAC" = "$old_mac" ];then
# 		echo "mac=$MAC"
# 	else
# 		sed -i "s/$old_mac/$MAC/g" /etc/jffs2/anyka_cfg.ini
# 		#cat /etc/jffs2/anyka_cfg.ini | grep mac
# 	fi
# }
check_status_is_completed()
{
	echo "check net status is completed"
	status=
	i=0
	while [ $i -lt 15 ]
	do
		status=`wpa_cli -iwlan0 status | grep wpa_state`
		#echo "###i= $i ####wpa_cli -iwlan0 status:$status"
		if [ "$status" = "wpa_state=COMPLETED" ];then
			echo "#####wpa_cli -iwlan0 status######i= $i #################"
			return 0
		fi
		sleep 1

		i=`expr $i + 1`
	done
	
	if [ $i -eq 15 ];then
		echo "-------check net status not completed-----"
		return 1
	fi

	return 0
}

station_connect()
{	
	#sh -c "wpa_cli -iwlan0 set_network $1 scan_ssid 1"
	wpa_cli -iwlan0 enable_network $1
	#wpa_cli -iwlan0 select_network $1
    sleep 1
	check_status_is_completed
	if [ $? -eq 1 ];then
		#echo " station_connect   if ------------------"
        wpa_cli -p/var/run/wpa_supplicant remove_network $1
		#wpa_cli -iwlan0 reconfig
		#exit 0
		#wpa_cli -iwlan0 save_config
		return 0
    fi
	sleep 1
	wpa_cli -iwlan0 save_config
	# write_mac_to_config_file
	check_ip_and_start

	sed -i '/disabled=1/d' /etc/jffs2/wpa_supplicant.conf
}

connet_wpa()
{
	NET_ID=""
	#refresh_net
	OK=`wpa_cli -iwlan0 status | grep wpa_state`
	if [ "$OK" = "wpa_state=COMPLETED" ];then
		echo " connect_wpa   if ------------------"
		NET_ID=`wpa_cli -iwlan0 list_network | grep CURRENT | awk '{print $1}'`
		wpa_cli -iwlan0 disable_network $NET_ID
	fi

	NET_ID=`wpa_cli -iwlan0 add_network`
	OK=`sh -c "wpa_cli -iwlan0 set_network $NET_ID ssid $SSID"`
	if [ "$OK" = "FAIL" ];then
		return
	fi
	# wpa_cli -iwlan0 set_network $NET_ID key_mgmt WPA-PSK
	OK=`sh -c "wpa_cli -iwlan0 set_network $NET_ID psk $PSK"`
	if [ "$OK" = "FAIL" ];then
		return
	fi
	station_connect $NET_ID
}

connet_wep()
{
	NET_ID=""
	refresh_net
	if [ "$NET_ID" = "" ];then
	{
		NET_ID=`wpa_cli -iwlan0 add_network`
		sh -c "wpa_cli -iwlan0 set_network $NET_ID ssid $SSID"
		wpa_cli -iwlan0 set_network $NET_ID key_mgmt NONE
		keylen=$echo${#KEY}
		
		if [ $keylen != "9" ] && [ $keylen != "17" ];then
		{
			wepkey1=${KEY#*'"'}
			wepkey2=${wepkey1%'"'*};
			KEY=$wepkey2;
			echo $KEY
		}
		fi				
		sh -c "wpa_cli -iwlan0 set_network $NET_ID wep_key${KEY_INDEX} $KEY"
	}
	elif [ "$GPSK" != "" ];then
	{
		keylen=$echo${#KEY}
		if [ $keylen != "9" ] && [ $keylen != "17" ];then
		{
			wepkey1=${KEY#*'"'}
			wepkey2=${wepkey1%'"'*};
			KEY=$wepkey2;
			echo $KEY
		}
		fi	
		sh -c "wpa_cli -iwlan0 set_network $NET_ID wep_key${KEY_INDEX} $KEY"
	}
	fi

	station_connect $NET_ID
}

connet_open()
{
	NET_ID=""
	#refresh_net
	OK=`wpa_cli -iwlan0 status | grep wpa_state`
	if [ "$OK" = "wpa_state=COMPLETED" ];then
		NET_ID=`wpa_cli -iwlan0 list_network | grep CURRENT | awk '{print $1}'`
		wpa_cli -iwlan0 disable_network $NET_ID
	fi
	NET_ID=`wpa_cli -iwlan0 add_network`
	sh -c "wpa_cli -iwlan0 set_network $NET_ID ssid $SSID"
	wpa_cli -iwlan0 set_network $NET_ID key_mgmt NONE

	station_connect $NET_ID
}

connect_adhoc()
{
	NET_ID=""
	refresh_net
	if [ "$NET_ID" = "" ];then
	{
		wpa_cli ap_scan 2
		NET_ID=`wpa_cli -iwlan0 add_network`
		sh -c "wpa_cli -iwlan0 set_network $NET_ID ssid $SSID"
		wpa_cli -iwlan0 set_network $NET_ID mode 1
		wpa_cli -iwlan0 set_network $NET_ID key_mgmt NONE
	}
	fi

	station_connect $NET_ID
}

check_same_ssid_and_tempary_remove()
{
	while true
	do
		NET_ID=`wpa_cli -iwlan0 list_network | grep -w "$GSSID" | awk '{print $1}'`

		if [ -n "$NET_ID" ];then
			wpa_cli -p/var/run/wpa_supplicant remove_network $NET_ID
		else
			break
		fi
	done
}
check_ssid_ok()
{
	GSSID=`sed -n '1p' /tmp/wifi_cfg`
	#echo "============== SSID = $GSSID ========================"
	if [ "$GSSID" = "" ]
	then
		echo "Incorrect ssid!"
		usage
		exit -1
	fi
	SSID=\'\"$GSSID\"\'
	#echo "============== SSID = $SSID ========================"
}

check_password_ok()
{
	if [ "$MODE" = "open" ];then
		return
	fi
	GPSK=`sed -n '2p' /tmp/wifi_cfg`
	CNT=`echo -n "$GPSK" | wc -c`
	# echo "=============GPSK CNT=$CNT===================="
	if [ $CNT -lt 8 ]
	then
		echo "Incorrect password!"
		usage
		exit -1
	fi
	PSK=\'\"$GPSK\"\'
	#echo "============== PSK = $PSK ========================"
}


#
# main:
#
#get_system_time
check_ssid_ok
check_password_ok
WPA_PID=`ps | grep wpa_supplicant | grep -v grep | awk '{print $1}'`
echo "WPA_PID= $WPA_PID ............................"
if [ -z "$WPA_PID" ];then
	echo "wpa_supplicant running ............................"
	wpa_supplicant -B -iwlan0 -Dwext -c /etc/jffs2/wpa_supplicant.conf
	if [ $? -ne 0 ];then
		cp /usr/local/wpa_supplicant.conf /etc/jffs2/wpa_supplicant.conf
		sync
		wpa_supplicant -B -iwlan0 -Dwext -c /etc/jffs2/wpa_supplicant.conf
	fi
fi
sleep 3
check_same_ssid_and_tempary_remove

echo $0 $*
case "$MODE" in
	wpa)
		connet_wpa 
		;;
	wep)
		connet_wep 
		;;
	open)
		connet_open 
		;;
	adhoc)
		connect_adhoc
		;;
	*)
		usage
		;;
esac
#get_system_time
exit 0

