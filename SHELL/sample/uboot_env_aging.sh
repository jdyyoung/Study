#!/bin/bash

#while true;do
	for i in $(seq 1 10)
	do
		random_value=`echo $RANDOM |cksum |cut -c 1-8`
		#fw_setenv test_${i} ${random_value}
		let test_${i}=$random_value
		echo test_${i}=$[test_$i]
	done
#done

for (( i=1;i<5;i++));do
	let test$i=1000
	echo test$i=$[test$i]
done








