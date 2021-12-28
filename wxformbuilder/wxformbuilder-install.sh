#!/usr/bin/bash -e

sudo apt install libwxgtk3.0-gtk3-dev libwxgtk-media3.0-gtk3-dev libboost-dev meson cmake make git

git clone --recursive https://github.com/wxFormBuilder/wxFormBuilder
cd wxFormBuilder

meson _build --prefix $PWD/_install --buildtype=release
ninja -C _build install

cd _install

sudo cp -r bin/* /usr/local/bin/
sudo cp -r share/* /usr/local/share/
sudo cp -r lib/arm-linux-gnueabihf/* /usr/local/lib/

