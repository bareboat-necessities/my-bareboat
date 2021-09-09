#!/bin/bash -e

# See: https://www.raspberrypi.org/forums/viewtopic.php?p=1880314

sudo apt install build-essential cmake cmake-data debhelper dbus google-mock \
    libboost-dev libboost-filesystem-dev libboost-log-dev libboost-iostreams-dev \
    libboost-program-options-dev libboost-system-dev libboost-test-dev \
    libboost-thread-dev libcap-dev libexpat1-dev libsystemd-dev libegl1-mesa-dev \
    libgles2-mesa-dev libglm-dev libgtest-dev liblxc1 \
    libproperties-cpp-dev libprotobuf-dev libsdl2-dev libsdl2-image-dev lxc-dev \
    pkg-config protobuf-compiler python3-minimal
    
git clone https://github.com/anbox/anbox.git
cd anbox
git reset --hard 3ed2e6d5c360d57b6aa61386e279adf3ff155ded
git submodule init
wget -O anbox-patch.zip "https://www.raspberrypi.org/forums/download/file.php?id=47177&sid=6efa5aaef4401f0bc981550e9a3dd499"
unzip anbox-patch.zip
git apply anbox-patch.diff

mkdir build
cd build
cmake ..
make

sudo make install

sudo mkdir /var/lib/anbox
wget https://anbox.postmarketos.org/android-7.1.2_r39.1-anbox_armv7a_neon-userdebug.img
sudo cp android-7.1.2_r39.1-anbox_armv7a_neon-userdebug.img /var/lib/anbox/android.img

sudo mkdir /dev/binderfs
sudo mount -t binder binder /dev/binderfs
sudo ./scripts/anbox-bridge.sh start
sudo ANBOX_LOG_LEVEL=debug anbox container-manager --daemon --privileged --data-path=/var/lib/anbox
anbox session-manager --single-window



