#!/usr/bin/bash -e


sudo apt-get -y install libaudiofile-dev


git clone https://github.com/cropinghigh/inmarsatc

cd inmarsatc
mkdir build && cd build
cmake ..
make
sudo make install
cd ../../


git clone https://github.com/cropinghigh/stdcdec

cd stdcdec
mkdir build && cd build
cmake ..
make
sudo make install
cd ../../
