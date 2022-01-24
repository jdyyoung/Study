#!/bin/bash
#清空文件
echo > time.log
for(( i=1;i<=10;i++))
do
date >> time.log
echo "\n" >> time.log
sleep 2
done

cat time.log
