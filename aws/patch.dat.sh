#!/bin/bash
conf=$1
# 2
x=$2
# 2
y=$3

name="${conf}_${x}_${y}"

for f in ${name}/*.v ; do
  echo $f
  sed -i 's/readmemh("\./readmemh("\/opt\/data/g' $f
done
