#!/bin/sh
# File:				update.sh	
# Provides:         
# Description:      update zImage&rootfs under dir1/dir2/...
# Author:			xc

VAR1="uImage"
VAR2="root.sqsh4"
VAR3="usr.sqsh4"
VAR4="usr.jffs2"

ZMD5="uImage.md5"
SMD5="usr.sqsh4.md5"
JMD5="usr.jffs2.md5"
RMD5="root.sqsh4.md5"

DIR1="/tmp"
DIR2="/mnt"
UPDATE_DIR_TMP=0

update_voice_tip()
{
	echo "play update voice tips"
	ccli -t --f "/usr/share/update_tip.mp3"
	sleep 10
}

update_ispconfig()
{
	rm -rf /etc/jffs2/isp*.conf
}

update_kernel()
{
	echo "check ${VAR1}............................."

	if [ -e ${DIR1}/${VAR1} ] 
	then
		#if [ -e ${DIR1}/${ZMD5} ];then

			result=`cd ${DIR1} && md5sum -c ${ZMD5} | grep OK`
			if [ -z "$result" ];then
				echo "MD5 check uImage failed, can't updata"
				return
			else
				echo "MD5 check uImage success"
			fi
		#fi

		echo "update ${VAR1} under ${DIR1}...."
		updater local KERNEL=${DIR1}/${VAR1}
	fi	
}

update_squash()
{		
	echo "check ${VAR3}.........................."

	if [ -e ${DIR1}/${VAR3} ]
	then
		#if [ -e ${DIR1}/${SMD5} ];then

			result=`cd ${DIR1} && md5sum -c ${SMD5} | grep OK`
			if [ -z "$result" ];then
				echo "MD5 check usr.sqsh4 failed, can't updata"
				return
			else
				echo "MD5 check usr.sqsh4 success"
			fi
		#fi

		echo "update ${VAR3} under ${DIR1}...."
		updater local B=${DIR1}/${VAR3}
	fi	
}

update_jffs2()
{
	echo "check ${VAR4}........................"

	if [ -e ${DIR1}/${VAR4} ]
	then
		#if [ -e ${DIR1}/${JMD5} ];then

			result=`cd ${DIR1} && md5sum -c ${JMD5} | grep OK`
			if [ -z "$result" ];then
				echo "MD5 check usr.jffs2 failed, can't updata"
				return
			else
				echo "MD5 check usr.jffs2 success"
			fi
		#fi

		echo "update ${VAR4} under ${DIR1}...."
		updater local C=${DIR1}/${VAR4}
	fi	
}

update_rootfs_squash()
{		
	echo "check ${VAR2}.........................."

	if [ -e ${DIR1}/${VAR2} ]
	then
		#if [ -e ${DIR1}/${RMD5} ];then

			result=`cd ${DIR1} && md5sum -c ${RMD5} | grep OK`
			if [ -z "$result" ];then
				echo "MD5 check root.sqsh4 failed, can't updata"
				return
			else
				echo "MD5 check root.sqsh4 success"
			fi
		#fi

		echo "update ${VAR2} under ${DIR1}...."
		updater local A=${DIR1}/${VAR2}
	fi	
}

update_check_image()
{
	echo "check update image .........................."

	for target in ${VAR1} ${VAR2} ${VAR3} ${VAR4}
	do
		if [ -f ${DIR1}/${target} ]; then
			echo "find a target ${target}, update in /tmp"
			UPDATE_DIR_TMP=1
			break
		fi	
	done
}

#
# main:
#
echo ""
echo "### enter update.sh ###"

#check battery capacity
battery_value=`cat /sys/kernel/ain/bat`
echo "battery value is $battery_value-------------"
if [ $battery_value -lt 3700 ];then
	echo "battery value less than 3700 , refuse update"
	ccli -t --f "/usr/share/battery_low_no_update.mp3"
	#sleep 
	exit 0
fi

echo "stop system service before update....."
killall -15 syslogd
killall -15 klogd
killall -15 tcpsvd

# play update vioce tip
update_voice_tip 

# send signal to stop watchdog
killall -12 daemon 
sleep 5
# kill apps, MUST use force kill
killall -9 daemon
killall -9 anyka_ipc
#killall -9 net_manage.sh
#/usr/sbin/wifi_manage.sh stop
#killall -9 smartlink

# sleep to wait the program exit
i=5
while [ $i -gt 0 ]
do
	sleep 1

	pid=`pgrep anyka_ipc`
	if [ -z "$pid" ];then
		echo "The main app anyka_ipc has exited !!!"
		break
	fi

	i=`expr $i - 1`
done

if [ $i -eq 0 ];then
	echo "The main app anyka_ipc is still run, we don't do update, reboot now !!!"
	reboot
fi

echo "############ please wait a moment. And don't remove TFcard or power-off #############"

# cp busybox to tmp, avoid the command become no use
cp /bin/busybox /tmp/
cp /usr/bin/uart_BPS4800_test /tmp
cp /usr/bin/ak_adec_demo /tmp/
cp /usr/share/update_success_reboot.mp3 /tmp
cp /etc/jffs2/wpa_supplicant.conf /tmp
#remove  config.json
rm -rf /etc/jffs2/config.json

# mount jffs2 file-system. 
#/bin/mount -t jffs2 /dev/mtdblock6 /etc/jffs2
umount /dev/mtdblock6

update_check_image

#检查tf卡是否有固件
if [ $UPDATE_DIR_TMP -ne 1 ];then
	## copy the image file to /tmp to avoid update fail on TF-card
	for dir in ${VAR1} ${VAR2} ${VAR3} ${VAR4}
	do
		cp ${DIR2}/${dir} /tmp/ 2>/dev/null
		cp ${DIR2}/${dir}.md5 /tmp/ 2>/dev/null
	done
	umount /mnt/ -l
	echo "update use image from /mnt"
fi

# update_check_image
# if [ $UPDATE_DIR_TMP -ne 1 ];then
# 	echo "no image updatem,will exit"
# 	kill  -9 `ps | grep eyes_led_show | grep -v grep | awk '{print $1}'`
# 	#TODO:没有固件不能升级的提示音
# 	#ccli -t --f "/usr/share/.mp3"
# 	#sleep 
# 	exit 0
# fi


cd ${DIR1}

update_ispconfig

update_kernel
update_jffs2
update_squash
update_rootfs_squash


#SPK_EN,使能功放，重启才有声音
/tmp/uart_BPS4800_test -a
/tmp/busybox sleep 1

/tmp/busybox echo "############ update finished, tips #############"
#播放升级完成提示音
ak_adec_demo 16000 1 mp3 /tmp/update_success_reboot.mp3 2
/tmp/busybox sleep 2

/tmp/busybox mount -t jffs2 /dev/mtdblock6 /etc/jffs2
/tmp/busybox cp /tmp/wpa_supplicant.conf /etc/jffs2
/tmp/busybox echo "############ update finished, reboot now #############"

/tmp/busybox sleep 1
/tmp/uart_BPS4800_test -d
/tmp/uart_BPS4800_test -g
/tmp/uart_BPS4800_test -c
