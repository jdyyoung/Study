#!/bin/bash

NULL_FILE=ab
NO_NULL_FILE=b
NO_EXIST_FILE=abc

if [ ! -s $NO_EXIST_FILE ];then
	echo $NO_EXIST_FILE
	echo "test ok!!!"
fi
if [ ! -s $NULL_FILE ];then
	echo $NULL_FILE
	echo "test ok!!!"
fi
