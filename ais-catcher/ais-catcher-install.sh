#!/bin/bash -e

sudo apt-get install git make gcc g++ cmake pkg-config -y librtlsdr-dev libairspy-dev libairspyhf-dev \
  libhackrf-dev libsoapysdr-dev libzmq3-dev
git clone https://github.com/jvde-github/AIS-catcher.git
cd AIS-catcher
mkdir build
cd build
cmake ..
make -j 4
sudo make install

