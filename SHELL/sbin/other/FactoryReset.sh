#!/bin/sh

rm /skybell/config/net/wpa_supplicant.conf
rm /skybell/config/net/wpa_supplicant.conf.enc

rm /skybell/misc/factory-ssid
rm /skybell/misc/reboot-count.bin
rm /skybell/misc/restart-count.bin

rm /skybell/config/settings.json
rm /skybell/config/settings.json.crc
rm /skybell/config/backup.json
rm /skybell/config/backup.json.crc

rm /skybell/config/resource.json
rm /skybell/config/access_token.json
rm /skybell/config/invite_token.json
rm /skybell/config/provision.json
rm /skybell/config/device_id.json
rm /skybell/config/device_name.txt
rm /skybell/config/hmac_key.txt

rm /skybell/config/resource.json.enc
rm /skybell/config/access_token.json.enc
rm /skybell/config/invite_token.json.enc
rm /skybell/config/provision.json.enc
rm /skybell/config/device_id.json.enc

rm -rf /skybell/misc/ota/*
rm -rf /skybell/misc/core/*
rm -rf /skybell/misc/oops/*
rm -f /skybell/config/net/wifi_only_2_4G

/skybell/bin/factorymode.arm 0

cp /etc/shadow.slim2 /skybell/config/shadow
sync

# _SN_=`fw_printenv __SN__ | awk -F= '{print $2}'`
# if [ "$_SN_" = "AAAAA00000" ];then
	# _SN_=`/skybell/bin/sn.arm | grep "sn =" | awk -F' = ' '{print $2}'`
# fi
# passwd=`openssl passwd -1 -salt 'skybell' $_SN_`
# echo "----------------passwd = ${passwd}--------------"
# last_passwd=`cat /skybell/config/shadow | grep "root" | awk -F: '{print $2}'`
# sed -i "s@$last_passwd@$passwd@g" /skybell/config/shadow




