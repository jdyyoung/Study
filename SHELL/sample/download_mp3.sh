#!/bin/bash

TEST_PWD=`pwd .`
#/vtcs/code/a_mintue_com/A_doc/Z_doc
for doc in {C..Z};do
	DOWNLOAD_DIR=${TEST_PWD}/${doc}_doc
	cd ${DOWNLOAD_DIR}
	dos2unix ${DOWNLOAD_DIR}/${doc}.txt
	sed -i 's/ /_/g' ${doc}.txt
	for line in `cat ${doc}.txt`;do
		#shell中的字母大小写转换_zzxuu的博客-CSDN博客_shell 大写转小写 - https://blog.csdn.net/enenand/article/details/78550678
		#wget --no-check-certificate https://listenaminute.com/w/walking.html
		echo "wget https://listenaminute.com/${doc,,}/${line,,}.mp3"
		wget --no-check-certificate https://listenaminute.com/${doc,,}/${line,,}.mp3
	done
done
