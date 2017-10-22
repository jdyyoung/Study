#!/bin/bash

#sed练习
#第一行到第三行增加字符串"drink tea"
sed '1,3a drink tea' sed_test.file  

#第一行代替为Hi
sed '1c Hi' sed_test.file

#删除第一行
sed '1d' ab
#删除最后一行
sed '$d' ab
#删除1到2行
sed '1,2d' ab
#删除2到最后一行
sed '2,$d' ab

