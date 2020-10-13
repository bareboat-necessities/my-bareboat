#!/bin/bash

sudo apt-get -y install \
 git-core \
 build-essential \
 cmake \
 npm \
 libuv1-dev \
 libglfw3-dev \
 libopengl-dev \
 ibjpeg-dev \
 libpng-dev \
 libgl1-mesa-dev \
 libglu1-mesa-dev \
 libcurl4-openssl-dev \
 zlib1g-dev

git clone --recurse-submodules https://github.com/mapbox/mapbox-gl-native.git
cd mapbox-gl-native || exit 255

npm update

mkdir build-rpi
cd build-rpi || exit 255

cmake .. -B build
cmake --build build

# MAPBOX_ACCESS_TOKEN=<public token> build/platform/glfw/mbgl-glfw
