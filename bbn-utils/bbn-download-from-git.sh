#!/bin/bash -e

cpuArch="armhf"
zipName="lysmarine-bbn_2021-11-15-raspios-${cpuArch}.img.xz"
imageSource="https://archive.org/download/lysmarine-bbn-linux-LTS4/${zipName}"

# Download the official image
log "Downloading official image from internet."
myCache=.

prefix=$myCache/$zipName
{
  echo curl -k -L --range 0-499999999           -o $prefix.part1 $imageSource
  echo curl -k -L --range 500000000-999999999   -o $prefix.part2 $imageSource
  echo curl -k -L --range 1000000000-1499999999 -o $prefix.part3 $imageSource
  echo curl -k -L --range 1500000000-1999999999 -o $prefix.part4 $imageSource
  echo curl -k -L --range 2000000000-2499999999 -o $prefix.part5 $imageSource
  echo curl -k -L --range 2500000000-           -o $prefix.part6 $imageSource
} | xargs -L 1 -I CMD -P 6 bash -c CMD

cat $prefix.part? > $myCache/$zipName
#rm $prefix.part?

#7z e -o$myCache/ $myCache/$zipName
#rm $myCache/$zipName
