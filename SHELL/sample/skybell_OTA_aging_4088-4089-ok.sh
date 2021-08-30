#!/bin/bash

#DEVICE_ID="610ce337120a240007240398"
#node  ${JS_CONF} -d 610ce337120a240007240398 -c 'systemlinux' -p 'fw_printenv > /tmp/cmd.log' -m 'post'
#i=0
DEVICE_ID=$1
JS_CONF=$2
UPDATE_VERSION=$3

V4076_FW_ID="5faca4691fda5a2d8f07ea9d"
V4083_FW_ID="601c04c8cccd406fceb18643"
V4087_FW_ID="60a528b1451690960371839b"
V4088_FW_ID="60fab97f8fb2e7919bbf697b"
V4089_FW_ID="61140a9586745707a1ca55ad"
V4076='v4.076'
V4083='v4.083'
V4087='v4.087'
V4088='v4.088'
V4089='v4.089'

#Shell脚本8种字符串截取方法总结 - 杨哥哥 - 博客园 - https://www.cnblogs.com/youngerger/p/8433118.html
CMD_LOG=`echo ${DEVICE_ID:0-4:4}`
CMD_LOG="./cmd_${CMD_LOG}.log"

START_TIME=`date +%m%d%H%M`
LOG_FILE="./$START_TIME-$DEVICE_ID.log"
echo "$START_TIME-$DEVICE_ID " > $LOG_FILE
#查询当前FW 的版本，升级到另外版本
while true;do
	CUR_TIME=`date "+%Y%m%d-%H:%M:%S"`
	echo "[$CUR_TIME]:start test!" >> $LOG_FILE
	timeout 10 node ${JS_CONF} -d ${DEVICE_ID} -c 'systemfirmware' GET device firmware > /tmp/buf.log
	CUR_VER=`cat /tmp/buf.log | grep Payload | awk -F':  '  '{print $2}' | grep -Po 'rootfs-[A,B][" :]+\K[^"]+'`
	printf "Current FW is $CUR_VER ! \n"
	#echo "Current FW is $CUR_VER!" >> /tmp/test.log
	#根据不同版本，执行对立的版本升级
	if [[ "$CUR_VER" == "$V4076" ]] || [[ "$CUR_VER" == "$V4083" ]] || [[ "$CUR_VER" == "v4.089" ]] || [[ "$CUR_VER" == "v4.086" ]] || [[ "$CUR_VER" == "v4.087" ]];then
		node ${JS_CONF} -d ${DEVICE_ID} -c 'firmwares/install' -m 'post' -p {\"id\":\"$V4088_FW_ID\"}
	elif [ "$CUR_VER" == "$V4088" ];then
		node ${JS_CONF} -d ${DEVICE_ID} -c 'firmwares/install' -m 'post' -p {\"id\":\"$V4089_FW_ID\"}
	else
		echo "Get FW version error or invalid FW,next!!!"
		sleep 2
		continue
	fi

	#初步测试，大约8min 能升级完成一次
	sleep 540 #600

	while true;do
		timeout 10 node ${JS_CONF} -d ${DEVICE_ID} -c 'systemfirmware' GET device firmware > /tmp/buf.log
		CUR_VER_2=`cat /tmp/buf.log | grep Payload | awk -F':  '  '{print $2}' | grep -Po 'rootfs-[A,B][" :]+\K[^"]+'`
		if [ "$CUR_VER" == "$CUR_VER_2" ];then
			echo "net so slow!!!!" >> $LOG_FILE
			sleep 10
			continue
		fi
		printf "Current FW is $CUR_VER ! \n"
		#升级网速不可控
		timeout 10 node  ${JS_CONF} -d ${DEVICE_ID} -c 'systemlinux' -p 'fw_printenv > /tmp/cmd.log' -m 'post'
		timeout 15 node ${JS_CONF} -d ${DEVICE_ID} -c 'systemlinux' -p 'curl --cacert /firmware/ca-bundle.crt -vvvv -T /tmp/cmd.log https://s3.amazonaws.com/skybell-device-logs-public/cmd.log' -m 'post'

		rm ${CMD_LOG} && sync
		wget --no-check-certificate  https://s3.amazonaws.com/skybell-device-logs-public/cmd.log -O ${CMD_LOG}
		RET=`echo $?`
		if [ "$RET" != "0" ];then
			rm ${CMD_LOG} && sync
			wget --no-check-certificate  https://s3.amazonaws.com/skybell-device-logs-public/cmd.log -O ${CMD_LOG}
		fi

		#两个都是认证才继续下个升级，30min 超时重新再来一次
		A_RESULT=`cat ${CMD_LOG} | grep __A__`
		B_RESULT=`cat ${CMD_LOG} | grep __B__`

		if [[ "$A_RESULT" == "__A__=VERIFIED" ]] && [[ "$B_RESULT" == "__B__=VERIFIED" ]];then
			#进行下一次测试
			CUR_TIME=`date "+%Y%m%d-%H:%M:%S"`
			echo "[$CUR_TIME]__A__=VERIFIED and __B__=VERIFIED,go to next test!" >> $LOG_FILE
			break
		else
			#继续查询
			sleep 5
			continue
		fi
	done
done
