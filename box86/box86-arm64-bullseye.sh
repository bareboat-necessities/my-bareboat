#!/usr/bin/bash -e

# See: https://medium.com/@safiuddinkhan/how-to-install-box86-box64-and-wine-on-raspberry-pi-os-bullseye-64-bit-c88028052f8c

sudo apt-get -y install build-essential mono-runtime git cmake

sudo dpkg --add-architecture armhf
sudo apt-get -y install gcc-arm-linux-gnueabihf

cd ~
git clone https://github.com/ptitSeb/box86 && cd box86
mkdir build; cd build; cmake .. -DRPI4ARM64=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo; make -j4
sudo make install
sudo systemctl restart systemd-binfmt

sudo apt-get -y install libegl-mesa0:armhf libgdm1:armhf libgl1-mesa-dri:armhf libglapi-mesa:armhf \
  libgles2-mesa:armhf libglu1-mesa:armhf libglx-mesa0:armhf mesa-va-drivers:armhf mesa-vdpau-drivers:armhf \
  mesa-vulkan-drivers:armhf libsdl1.2debian:armhf libsdl2-2.0-0:armhf libudev1:armhf

# Wine
sudo apt-get -y install libasound2:armhf libc6:armhf libglib2.0-0:armhf libgphoto2-6:armhf libgphoto2-port12:armhf libgstreamer-plugins-base1.0-0:armhf \
 libgstreamer1.0-0:armhf libldap-2.4-2:armhf libopenal1:armhf libpcap0.8:armhf libpulse0:armhf libsane1:armhf libudev1:armhf libunwind8:armhf libusb-1.0-0:armhf \
 libvkd3d1:armhf libx11-6:armhf libxext6:armhf ocl-icd-libopencl1:armhf libasound2-plugins:armhf libncurses6:armhf libtinfo5:armhf libasound2:armhf \
 libc6:armhf libglib2.0-0:armhf libgphoto2-6:armhf libgphoto2-port12:armhf libgstreamer-plugins-base1.0-0:armhf \
 libgstreamer1.0-0:armhf libldap-2.4-2:armhf libopenal1:armhf libpcap0.8:armhf libpulse0:armhf libsane1:armhf \
 libudev1:armhf libunwind8:armhf libusb-1.0-0:armhf libvkd3d1:armhf libx11-6:armhf libxext6:armhf ocl-icd-libopencl1:armhf \
 libasound2-plugins:armhf libncurses6:armhf libmpeg2-4:armhf libmpeg2encpp-2.1-0:armhf libtinfo6:armhf \
 libmpg123-0:armhf libtinfo5:armhf cabextract:armhf libxslt1.1:armhf zenity:armhf

cd
cd ~/Downloads
wget https://dl.winehq.org/wine-builds/debian/dists/bullseye/main/binary-i386/wine-stable-i386_6.0.4~bullseye-1_i386.deb
wget https://dl.winehq.org/wine-builds/debian/dists/bullseye/main/binary-i386/wine-stable_6.0.4~bullseye-1_i386.deb
dpkg-deb -xv wine-stable-i386_6.0.4~bullseye-1_i386.deb wine-installer
dpkg-deb -xv wine-stable_6.0.4~bullseye-1_i386.deb wine-installer
mv ~/Downloads/wine-installer/opt/wine* ~/wine
rm -rf wine-installer
cd


sudo ln -s ~/wine/bin/wine /usr/local/bin/wine
sudo ln -s ~/wine/bin/wineboot /usr/local/bin/wineboot
sudo ln -s ~/wine/bin/winecfg /usr/local/bin/winecfg
sudo ln -s ~/wine/bin/wineserver /usr/local/bin/wineserver
sudo chmod +x /usr/local/bin/wine /usr/local/bin/wineboot /usr/local/bin/winecfg /usr/local/bin/wineserver

wget https://raw.githubusercontent.com/Winetricks/winetricks/master/src/winetricks
sudo mv winetricks /usr/local/bin/
sudo chmod +x /usr/local/bin/winetricks

