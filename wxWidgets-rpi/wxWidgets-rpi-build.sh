#!/bin/bash

sudo apt-get -y install build-essential cmake gettext git-core libgtk-3-dev libgl1-mesa-dev libglu1-mesa-dev \
 libgtk2.0-dev \
 zlib1g-dev libjpeg-dev libpng-dev libtiff5-dev libsm-dev autotools-dev autoconf libexpat1-dev libxt-dev \
 libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libwebkit2gtk-4.0-dev libnotify-dev

git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets || exit 255

#git checkout 301e5ec9fe3719fac8a2786e8675b19e2242183e

mkdir build-gtk2
cd build-gtk2 || exit 255

../configure \
 --with-gtk=2 \
 --with-libpng=sys \
 --with-libjpeg=sys \
 --with-libtiff=sys \
 --with-regex=sys \
 --with-zlib=sys \
 --with-expat=sys \
 --enable-display \
 --enable-geometry \
 --enable-graphics_ctx \
 --enable-mediactrl \
 --enable-sound \
 --with-opengl \
 --enable-webview
make -sj5
sudo make install
sudo ldconfig
