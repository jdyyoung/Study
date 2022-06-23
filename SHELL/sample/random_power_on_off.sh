#!/bin/sh

power_off(){
	echo "----- power_off -----"
	echo 0 >   /sys/class/gpio/gpio58/value && echo 1 >   /sys/class/gpio/gpio58/value
}

power_on(){
	echo "----- power_on -----"
	echo 0 >   /sys/class/gpio/gpio57/value && echo 1 >   /sys/class/gpio/gpio57/value
}

#T5_g57=`cat /sys/class/gpio/gpio57/value` 
#W6_g58=`cat /sys/class/gpio/gpio58/value` 
echo 58 > /sys/class/gpio/export                              
echo out > /sys/class/gpio/gpio58/direction

while true;do
	power_on
	sleep 15
	#再随机1~15s 断电
	RANDOM_VAL=`echo $RANDOM |cksum |cut -c 1-4`
	SLEEP_S=$(( $RANDOM_VAL%15 ))
	echo "after sleep $SLEEP_S power_off"
	sleep $SLEEP_S 
	power_off
	sleep 2
	echo "xdc random power_on_off test" >> /skybell/misc/xdc_test
done