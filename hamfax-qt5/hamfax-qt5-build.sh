#!/bin/bash -e

sudo apt-get -y install qtbase5-dev qtbase5-dev-tools qttools5-dev-tools

sudo cp /usr/bin/lrelease /usr/lib/qt5/bin/

git clone https://github.com/sergioisidoro/hamfax-qt5
cd hamfax-qt5/

autoreconf -f -i
./configure
make -j 4

