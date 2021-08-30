#!/bin/bash

:<< comment
rck@rck:/tmp$ ./if_test_error.sh 
./if_test_error.sh: line 14: syntax error: unexpected end of file

因为：elif 写成C语言else if
comment


VAR=$1

if [ "$VAR" == "0" ];then
	echo "+++++++++++++VAR=$VAR================"
else if [ "$VAR" == "1"];then
	echo "=============VAR=$VAR================"
else
	echo "111111111"
fi
