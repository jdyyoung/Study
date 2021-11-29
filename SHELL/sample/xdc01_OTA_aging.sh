#!/bin/bash

CAMERA_SN=$1
AGING_VERSION_IS=$2
UPDATE_VER_1=`echo ${AGING_VERSION_IS} | awk -F- '{print $1}'`
UPDATE_VER_2=`echo ${AGING_VERSION_IS} | awk -F- '{print $2}'`

#600 seconds
SLEEP_TIME=60

HOST="http://ecosystem.redbeecloud.com/RedbeeBackend"
AUTO_OTA_URL="v1/camera/otaByCameraSn"
GET_DEVICE_INFO_URL="v1/camera/findCameraInfoBySn"
GET_HEADER="--location --request GET"
POST_HEADER="--location --request POST"
CURL_HEADER="Content-Type:application/json"

echo "UPDATE_VER_1= $UPDATE_VER_1   UPDATE_VER_2=$UPDATE_VER_2"

:<< doc
curl --location --request POST "http://ecosystem.redbeecloud.com/RedbeeBackend/v1/camera/otaByCameraSn"  --header "Content-Type: application/json" --data "{\"version\":\"v1034\",\"cameraSn\":\"1200000008021401\"}"

curl --location --request GET "http://ecosystem.redbeecloud.com/RedbeeBackend/v1/camera/findCameraInfoBySn/1200000008021401" > /tmp/get_device_info.log
cat /tmp/get_device_info.log | jq ".data" | grep fwVersion
doc

START_TIME=`date +%m%d%H%M`
LOG_FILE="./$START_TIME-$CAMERA_SN.log"
echo "$START_TIME-$CAMERA_SN " > $LOG_FILE

while true;do

echo "-------------xdc01-auto-OTA-start------------"
echo "-------------xdc01-auto-OTA-start------------" >> $LOG_FILE
HOST_URL="${HOST}/$GET_DEVICE_INFO_URL/${CAMERA_SN}"
#echo "GET_HEADER= ${GET_HEADER}"
#echo "HOST_URL= ${HOST_URL}"
#echo "CURL_HEADER= ${CURL_HEADER}"
curl ${GET_HEADER} ${HOST_URL}  > /tmp/get_device_info.log
#cat /tmp/get_device_info.log
#cat /tmp/get_device_info.log | jq ".data" | grep fwVersion

CUR_VERSION=`cat /tmp/get_device_info.log | jq ".data" | grep fwVersion | awk -F": "  '{print $2}' | tr -d "[\",]"`
if [ "$CUR_VERSION" == "${UPDATE_VER_1}" ];then
	WILL_VERSION=$UPDATE_VER_2
else
	WILL_VERSION=$UPDATE_VER_1
fi

echo "-----CUR_VERSION=$CUR_VERSION  ---->  $WILL_VERSION"
echo "-----CUR_VERSION=$CUR_VERSION  ---->  $WILL_VERSION" >> $LOG_FILE
POST_DATA="{\"version\":\"$WILL_VERSION\",\"cameraSn\":\"$CAMERA_SN\"}"
#echo "POST_DATA=$POST_DATA"
HOST_URL="${HOST}/$AUTO_OTA_URL"
#echo "HOST_URL=${HOST_URL}"
curl ${POST_HEADER} ${HOST_URL} --header ${CURL_HEADER} --data ${POST_DATA}
echo ""

#sleep 600
times=0
while [ $times -lt $SLEEP_TIME ];do
	let times++;
	#echo "******************************************************"
	echo -e "OTA procesing.... \033[44;37;5m $times \033[0m * 10s"
	#echo "******************************************************"
	sleep 10;
done

cnt=0
while [ $cnt -lt 6 ];do

let cnt++
HOST_URL="${HOST}/$GET_DEVICE_INFO_URL/${CAMERA_SN}"
curl ${GET_HEADER} ${HOST_URL}  > /tmp/get_device_info.log
UPDATE_AFTER_VERSION=`cat /tmp/get_device_info.log | jq ".data" | grep fwVersion | awk -F": "  '{print $2}' | tr -d "[\",]"`

if [ "$UPDATE_AFTER_VERSION" == "${WILL_VERSION}" ];then
	echo "xdc01-auto-OTA-end:update-ok!" >> $LOG_FILE
	break
fi

sleep 30

done
if [ $cnt -eq 6 ];then
	echo "xdc01-auto-OTA-end:update-fail!" >> $LOG_FILE
fi
echo "-------------xdc01 auto OTA  end!!!!------------"

done


