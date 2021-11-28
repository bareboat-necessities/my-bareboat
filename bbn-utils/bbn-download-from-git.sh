#!/bin/bash -e

cpuArch="armhf"
zipName="lysmarine-bbn_2021-11-15-raspios-${cpuArch}.img.xz"
imageSource="https://github.com/bareboat-necessities/lysmarine_gen/releases/download/v2021-11-15/${zipName}"

# Download the official image
log "Downloading official image from internet."
myCache=.

prefix=$myCache/$zipName
{
  echo curl -k -L -o $prefix.part1 $imageSource.part1
  echo curl -k -L -o $prefix.part2 $imageSource.part2
  echo curl -k -L -o $prefix.part3 $imageSource.part3
  echo curl -k -L -o $prefix.part4 $imageSource.part4
  echo curl -k -L -o $prefix.part5 $imageSource.part5
  echo curl -k -L -o $prefix.part6 $imageSource.part6
} | xargs -L 1 -I CMD -P 6 bash -c CMD

cat $prefix.part? > $myCache/$zipName

