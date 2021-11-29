#!/bin/bash

oldext="jpg"
newext="jpeg"

dir=$(eval pwd)
for file in $(ls $dir | grep .$oldext)
do
        name=$(ls $file | cut -d. -f1)
        #echo "file = $file"
        mv      $file ${name}.$newext
done

echo "change JPG=====>jpg done!"