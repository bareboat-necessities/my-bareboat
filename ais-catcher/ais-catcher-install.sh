#!/bin/bash -e

# See: https://github.com/jvde-github/AIS-catcher

sudo apt-get install git make gcc g++ cmake pkg-config -y librtlsdr-dev libairspy-dev libairspyhf-dev \
  libhackrf-dev libsoapysdr-dev libzmq3-dev libcurl4-openssl-dev zlib1g-dev
git clone https://github.com/jvde-github/AIS-catcher.git
cd AIS-catcher
mkdir build
cd build
cmake ..
make -j 4
sudo make install

# For runtime
sudo apt-get install -y librtlsdr libairspy libairspyhf \
  libhackrf libsoapysdr libzmq3 libcurl4-openssl zlib1g

