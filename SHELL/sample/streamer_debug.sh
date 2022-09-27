#!/bin/sh
：<<doc

shell中&&和||的用法 要用[[ ]]括起来判断
判断字符串相等，==两边都要有""
Linux 中shell 脚本if判断多个条件_IChen.的博客-CSDN博客_shellif判断多个条件 - 
https://blog.csdn.net/ichen820/article/details/113769268
doc


while true;do
	skybell_pid=`pidof skybell`
	mcu_ping_pid=`pidof mcu_ping.sh`
	if [[ "$skybell_pid" == "" && "$mcu_ping_pid" == "" ]];then
		echo "skybell exit,red led show-------------------"
		/skybell/bin/mcu_ping.sh >> /tmp/mcu_ping &
		/skybell/bin/mcu_uart_test -c
	fi
	sleep 10
done
