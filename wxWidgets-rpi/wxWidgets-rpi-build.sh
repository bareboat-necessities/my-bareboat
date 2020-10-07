#!/bin/bash

sudo apt-get -y install build-essential cmake gettext git-core libgtk-3-dev libgtk-3-0 libgl1-mesa-dev libglu1-mesa-dev \
 zlib1g-dev libjpeg-dev libpng-dev libtiff5-dev libsm-dev autotools-dev autoconf libexpat1-dev libxt-dev \
 libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libwebkit2gtk-4.0-dev libnotify-dev

git clone -b v3.1.4 --recurse-submodules https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets || exit 255

mkdir build-gtk
cd build-gtk || exit 255

../configure --with-gtk=3 --with-libpng=sys --with-libjpeg=sys --with-libtiff=sys --with-regex=builtin \
 --enable-display --enable-geometry --enable-graphics_ctx --enable-mediactrl --enable-sound \
 --with-zlib=builtin --with-expat=builtin --enable-unicode --enable-aui --with-opengl \
 --enable-debug --enable-exceptions --enable-webview LDFLAGS="-static-libgcc -static-libstdc++"
make -sj5
sudo make install
sudo ldconfig
