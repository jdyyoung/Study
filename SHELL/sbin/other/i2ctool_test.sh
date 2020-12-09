#!/bin/sh

i=0
while [ $i -le 50 ];do
	echo $i
	#i2cset -f -y 1 0x50 0x00
	i2cget -f -y 1 0x08 0x66
	i=$(($i+1))
	sleep 1
done
echo "$0 finished"
