#!/bin/bash

# Debian source repository https://gitlab.com/leamas/opencpn.git

# Build wxWidgets before
# See ../wxWidgets-rpi/

sudo apt-get -y install build-essential cmake gettext git-core gpsd gpsd-clients libgps-dev \
 libglu1-mesa-dev libgtk-3-dev libbz2-dev libtinyxml-dev libportaudio2 portaudio19-dev libcurl4-openssl-dev \
 libexpat1-dev libcairo2-dev libarchive-dev liblzma-dev libexif-dev libelf-dev libsqlite3-dev \
 bc bison flex libssl-dev python3 ddd htop

git clone -b PPA-5.2.0.8 https://github.com/opencpn/opencpn.git

cd opencpn && mkdir build

# in CMakeLists.txt replace references to 3.0 wxWidgets to 3.1 wxWidgets

cd build || exit 255

cmake -DOCPN_BUNDLE_TCDATA=ON -DOCPN_BUNDLE_GSHHS=ON -DOCPN_USE_GL=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo \
 -DOCPN_USE_CRASHREPORT=ON -DOCPN_ENABLE_PORTAUDIO=OFF -DOCPN_ENABLE_SYSTEM_CMD_SOUND=ON -DOCPN_FORCE_GTK3=ON ..

# in ../src/glChartCanvas.cpp add #include <GL/glx.h>

make -sj5

sudo make install
