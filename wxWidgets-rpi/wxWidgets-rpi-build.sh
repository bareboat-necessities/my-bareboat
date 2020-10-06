#!/bin/bash

sudo apt-get -y install build-essential cmake gettext git-core libgtk-3-dev libgtk-3-0

git clone -b v3.1.4 --recurse-submodules https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets || exit

mkdir build-gtk
cd build-gtk || exit

../configure --with-gtk=3 --with-libpng=builtin --with-libjpeg=builtin --with-libtiff=builtin --with-regex=builtin \
 --with-zlib=builtin --with-expat=builtin --enable-unicode --enable-aui --with-opengl \
 LDFLAGS="-static-libgcc -static-libstdc++"
make -sj5
sudo make install
sudo ldconfig
