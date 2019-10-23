#!/bin/sh

# if [ $# != 1 ];then
# 	echo "usage:$0 ip 
# 		e.g.:$0 192.168.0.11"
# 	exit -1
# fi
# echo "ftp is IP is $1"
make clean
make all
make install
make image
#cd rootfs
#ls -l

# echo "#####################################################################"
# echo "now starting ftp transfer............................................"
# echo "#####################################################################"
# ftp -i -n $1 <<EOF
# user root cloud39ev200
# cd /tmp
# ls
# put usr.sqsh4
# EOF

# echo "########################$0 exit######################################"

#yys 20190312 test upload
