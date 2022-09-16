#!/usr/bin/bash -e

sudo apt-get install  libfreeimage-dev

git clone https://github.com/nohal/imgkap
cd imgkap
make -j 4
sudo make install


