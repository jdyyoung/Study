#!/bin/bash

#${^^}将值都转换成大写
#${,,}将值都转换成小写

SN=$1
echo "===========raw SN=$SN"
SN=${SN^^}
echo "===========^^ change SN=$SN"
SN=${SN,,}
echo "===========,, change SN=$SN"
