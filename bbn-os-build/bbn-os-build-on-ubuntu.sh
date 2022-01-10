#!/bin/bash -e

# For Ubuntu 16.04.7

export DOCKER_IMAGE=arm32v7/debian:buster
export CONTAINER_DISTRO=debian:buster
export PKG_RELEASE=raspbian
export PKG_DISTRO=buster
export PKG_ARCH=armhf
export EMU=on

WORK_DIR=$(pwd)/ci-source

apt-get install curl git docker.io zerofree

rm -rf $WORK_DIR
mkdir $WORK_DIR && cd $WORK_DIR

git clone --branch buster https://github.com/bareboat-necessities/lysmarine_gen .
#git clone --branch incremental https://github.com/bareboat-necessities/lysmarine_gen .

mv *install-scripts* cross-build-release/
chmod a+x .circleci/*.sh

.circleci/build-ci.sh 2>&1 | tee build.log

IMG=$(ls cross-build-release/release/*/*.img)

# Mount the image and make the binds required to chroot.
losetup -f
partitions=$(kpartx -sav $IMG | cut -d' ' -f3)
partQty=$(echo $partitions | wc -w)
echo $partQty partitions detected.

# mount partition table in /dev/loop
loopId=$(echo $partitions | grep -oh '[0-9]*' | head -n 1)

rootfs=./rootfs
mkdir -p $rootfs/boot

mountOpt="-o ro"

if [ $partQty == 2 ]; then
  mount $mountOpt -v /dev/mapper/loop${loopId}p2 $rootfs/
  if [ ! -d $rootfs/boot ]; then mkdir $rootfs/boot; fi
  mount $mountOpt -v /dev/mapper/loop${loopId}p1 $rootfs/boot/
elif [ $partQty == 1 ]; then
  mount $mountOpt -v /dev/mapper/loop${loopId}p1 $rootfs/
else
  log "ERROR: unsuported amount of partitions."
  exit 1
fi

zerofree $rootfs/

umount $rootfs/boot
umount $rootfs

kpartx -d $IMG
losetup -d $IMG

xz -z -c -v -6 --threads=8 $IMG > $IMG.xz
