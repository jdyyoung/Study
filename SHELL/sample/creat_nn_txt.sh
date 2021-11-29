#!/bin/bash

:<< comment
$fullfile=/the/path/foo.txt
$fullname=$(basename $fullfile)
$dir=$(dirname $fullfile)
$filename=$(echo $fullname | cut -d . -f1)
$extension=$(echo $fullname | cut -d . -f2)
$ echo $dir , $fullname , $filename , $extension
/the/path , foo.txt , foo , txt
comment

MP4_FILE=$1

usage(){
	echo "For example:$0 REC_2021-10-20_03-29-50.mp4"
}

if [ $# != 1 ];then
	usage
	exit -1
fi

OUT_DIR=$(echo $MP4_FILE | cut -d . -f1)
echo "OUT_DIR=$OUT_DIR"
rm -rf $OUT_DIR
mkdir $OUT_DIR
ffmpeg -i $MP4_FILE -r 1 -f image2 $OUT_DIR/image%3d.jpeg

ls $OUT_DIR > ${OUT_DIR}.txt
mv ${OUT_DIR}.txt ${OUT_DIR}/${OUT_DIR}.txt
sed -i "s#^#${OUT_DIR}\\\#g" ${OUT_DIR}/${OUT_DIR}.txt


_CNT=`cat ${OUT_DIR}/${OUT_DIR}.txt | wc -l`

printf "\n\n"
echo "appnn yolo -v 14 -filelist ${OUT_DIR}\\${OUT_DIR}.txt -filelistcnt ${_CNT}"
printf "\n\n"
