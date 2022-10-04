#!/bin/bash -e

cd ~
sudo apt-get -y install build-essential git cmake flex bison libelf-dev libusb-dev libhidapi-dev libftdi1-dev
git clone https://github.com/avrdudes/avrdude
cd avrdude
cmake -D CMAKE_BUILD_TYPE=RelWithDebInfo -D HAVE_LINUXGPIO=1 -D HAVE_LINUXSPI=1 -B build_linux
cd build_linux/
make -j 4
sudo make install
cd ~
