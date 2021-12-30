#!/bin/bash -e

export DOCKER_IMAGE=arm32v7/debian:buster
export CONTAINER_DISTRO=debian:buster
export PKG_RELEASE=raspbian
export PKG_DISTRO=buster
export PKG_ARCH=armhf
export EMU=on

WORK_DIR=$(pwd):/ci-source

rm -rf $WORK_DIR
mkdir $WORK_DIR && cd $WORK_DIR

git clone https://github.com/bareboat-necessities/lysmarine_gen .

mv install-scripts cross-build-release/
chmod a+x .circleci/*.sh

.circleci/build-ci.sh 2>&1 | tee build.log

