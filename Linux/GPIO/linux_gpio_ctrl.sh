#!/bin/sh

:<<comment
1. 导出   /sys/class/gpio# echo 203 > export
2. 设置方向      /sys/class/gpio/gpio203# echo out > direction
3. 查看方向      /sys/class/gpio/gpio203# cat direction
4. 设置输出      /sys/class/gpio/gpio203# echo 1 > value
5. 查看输出值  /sys/class/gpio/gpio203# cat value  
6. 取消导出     /sys/class/gpio# echo 203 > unexport
comment

echo 12 > /sys/class/gpio/export

#direction接受的参数：in, out, high, low。
#high/low同时设置方向为输出，并将value设置为相应的1/0。
echo out > /sys/class/gpio/gpio12/direction

# value文件是端口的数值，为1或0.
echo 1 >/sys/class/gpio/gpio12/value

echo in > /sys/class/gpio/gpio12/direction
cat /sys/class/gpio/gpio12/value

echo 12 > /sys/class/gpio/unexport