#!/bin/sh
# File:				update.sh	
# Provides:         
# Description:      recover system configuration
# Author:			aj

play_recover_tip()
{
	ccli -t --f "/usr/share/recover_cfg.mp3"
	sleep 6
}
play_recover_tip

#recover factory config ini
cp /usr/local/factory_cfg.ini /etc/jffs2/anyka_cfg.ini
cp /usr/local/wpa_supplicant.conf /etc/jffs2/wpa_supplicant.conf
rm -rf /etc/jffs2/yc_product_test
rm -f /etc/jffs2/wechat_messge                                                                            
#recover isp config ini
rm -rf /etc/jffs2/isp*.conf

#remove  config.json
rm -rf /etc/jffs2/config.json

sync 

#使能功放，使开机有提示音
uart_BPS4800_test -a
usleep 1000
uart_BPS4800_test -d
uart_BPS4800_test -g
reboot

# #断电关机
# uart_BPS4800_test -c