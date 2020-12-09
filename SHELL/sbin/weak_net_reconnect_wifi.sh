#!/bin/sh

#检测wpa_supplicant.conf里的ssid

check_ap_signal_leval()
{
	iwlist wlan0 scan > /tmp/iwlist_scan_result

	LINE_1=`grep -nw "$1" /tmp/iwlist_scan_result | awk -F: '{print $1}'`
	
	if [ "$LINE_1" = "" ];then
		return 0
	fi

	sed -n "${LINE_1},$ p" /tmp/iwlist_scan_result > /tmp/temp_file_1

	LINE_1=`cat /tmp/temp_file_1 | sed -n "/Extra:fm/=" | head -1`

	sed -n "1,${LINE_1} p" /tmp/temp_file_1 > /tmp/temp_file_ok

	SIGNAL_LEVEL=`grep "Signal level" /tmp/temp_file_ok | awk -F= '{print $3}' | awk -F / '{printf $1}'`

	echo "---------------------Signal level = $SIGNAL_LEVEL ---------------------"
	return 1
}

CURRENT_SSID=`wpa_cli list_network | grep "CURRENT" | awk '{print $2}'`
wpa_cli reconfig
#while true;do
SSID=`wpa_cli list_network | awk 'NR>2{print $2}'`
	for i in ${SSID};do
		echo "-----------SSID = $i -------------"
		if [ "$i" = "$CURRENT_SSID" ];then
			echo "---------- continue -------------"
			continue
		fi
		check_ap_signal_leval $i
		if [ $? = 0 ];then
			continue
		fi

		if [ $SIGNAL_LEVEL -gt "80" ];then
			#重新尝试连接此ssid
			NET_ID=`wpa_cli list_network | grep -w $i | awk '{print $1}'`
			wpa_cli -iwlan0 select_network $NET_ID
			wpa_cli -iwlan0 enable_network $NET_ID
			break
		else
			echo "---------------if------Signal level = $SIGNAL_LEVEL ---------------------"
		fi
	done





