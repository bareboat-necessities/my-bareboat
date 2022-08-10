#!/bin/bash -e

# For Ubuntu 16.04.7

export DOCKER_IMAGE=arm64v8/debian:bullseye
export CONTAINER_DISTRO=debian:bullseye
export PKG_RELEASE=raspbian
export PKG_DISTRO=bullseye
export PKG_ARCH=arm64
export EMU=on

WORK_DIR=$(pwd)/ci-source

apt-get install curl git docker.io zerofree kpartx

rm -rf $WORK_DIR
mkdir $WORK_DIR && cd $WORK_DIR

git clone --branch bullseye https://github.com/bareboat-necessities/lysmarine_gen .
#git clone --branch buster https://github.com/bareboat-necessities/lysmarine_gen .
#git clone --branch incremental https://github.com/bareboat-necessities/lysmarine_gen .

mv *install-scripts* cross-build-release/
chmod a+x .circleci/*.sh

.circleci/build-ci.sh 2>&1 | tee build.log

IMG=$(ls cross-build-release/release/*/*.img)
xz -z -c -v -6 --threads=8 $IMG > $IMG.xz
