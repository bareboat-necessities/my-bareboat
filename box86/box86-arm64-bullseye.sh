#!/usr/bin/bash -e

# See: https://medium.com/@safiuddinkhan/how-to-install-box86-box64-and-wine-on-raspberry-pi-os-bullseye-64-bit-c88028052f8c

sudo apt-get -y install build-essential mono-runtime git cmake cabextract gcc-arm-linux-gnueabihf libc6-dev-armhf-cross zenity

sudo dpkg --add-architecture armhf
sudo apt-get -y install gcc-arm-linux-gnueabihf

sudo apt update 

sudo apt-get -y install libegl-mesa0:armhf libgdm1:armhf libgl1-mesa-dri:armhf libglapi-mesa:armhf \
  libgles2-mesa:armhf libglu1-mesa:armhf libglx-mesa0:armhf mesa-va-drivers:armhf mesa-vdpau-drivers:armhf \
  mesa-vulkan-drivers:armhf libsdl1.2debian:armhf libsdl2-2.0-0:armhf libudev1:armhf libllvm11:armhf \
  linux-libc-dev:armhf libncurses6:armhf libc6:armhf  libx11-6:armhf libgdk-pixbuf2.0-0:armhf libgtk2.0-0:armhf \
  libstdc++6:armhf libsdl2-2.0-0:armhf mesa-va-drivers:armhf libsdl1.2-dev:armhf libsdl-mixer1.2:armhf \
  libpng16-16:armhf libcal3d12v5:armhf libsdl2-net-2.0-0:armhf libopenal1:armhf libsdl2-image-2.0-0:armhf \
  libvorbis-dev:armhf libcurl4:armhf libjpeg62:armhf  libudev1:armhf libgl1-mesa-dev:armhf  libx11-dev:armhf \
  libsmpeg0:armhf libavcodec58:armhf libavformat58:armhf libswscale5:armhf libsdl2-image-2.0-0:armhf \
  libsdl2-mixer-2.0-0:armhf gcc-arm-linux-gnueabihf

sudo wget https://ryanfortner.github.io/box86-debs/box86.list -O /etc/apt/sources.list.d/box86.list
wget -qO- https://ryanfortner.github.io/box86-debs/KEY.gpg | gpg --dearmor | sudo tee /usr/share/keyrings/box86-debs-archive-keyring.gpg
sudo apt update && sudo apt-get -y install box86:armhf=0.2.7+20220831.a25c32a-1


# Wine
sudo apt-get -y install libasound2:armhf libc6:armhf libglib2.0-0:armhf libgphoto2-6:armhf libgphoto2-port12:armhf libgstreamer-plugins-base1.0-0:armhf \
 libgstreamer1.0-0:armhf libldap-2.4-2:armhf libopenal1:armhf libpcap0.8:armhf libpulse0:armhf libsane1:armhf libudev1:armhf libunwind8:armhf libusb-1.0-0:armhf \
 libvkd3d1:armhf libx11-6:armhf libxext6:armhf ocl-icd-libopencl1:armhf libasound2-plugins:armhf libncurses6:armhf libtinfo5:armhf libasound2:armhf \
 libc6:armhf libglib2.0-0:armhf libgphoto2-6:armhf libgphoto2-port12:armhf libgstreamer-plugins-base1.0-0:armhf \
 libgstreamer1.0-0:armhf libldap-2.4-2:armhf libopenal1:armhf libpcap0.8:armhf libpulse0:armhf libsane1:armhf \
 libudev1:armhf libunwind8:armhf libusb-1.0-0:armhf libvkd3d1:armhf libx11-6:armhf libxext6:armhf ocl-icd-libopencl1:armhf \
 libasound2-plugins:armhf libncurses6:armhf libmpeg2-4:armhf libmpeg2encpp-2.1-0:armhf libtinfo6:armhf \
 libmpg123-0:armhf libtinfo5:armhf libxslt1.1:armhf

cd ~/Downloads

wget https://dl.winehq.org/wine-builds/debian/dists/bullseye/main/binary-i386/wine-stable-i386_5.0.4~bullseye-1_i386.deb
wget https://dl.winehq.org/wine-builds/debian/dists/bullseye/main/binary-i386/wine-stable_5.0.4~bullseye-1_i386.deb
dpkg-deb -xv wine-stable-i386_5.0.4~bullseye-1_i386.deb wine-installer
dpkg-deb -xv wine-stable_5.0.4~bullseye-1_i386.deb wine-installer

#wget https://dl.winehq.org/wine-builds/debian/dists/bullseye/main/binary-i386/wine-staging-i386_7.15~bullseye-1_i386.deb
#wget https://dl.winehq.org/wine-builds/debian/dists/bullseye/main/binary-i386/wine-staging_7.15~bullseye-1_i386.deb
#dpkg-deb -xv wine-staging-i386_7.15~bullseye-1_i386.deb wine-installer
#dpkg-deb -xv wine-staging_7.15~bullseye-1_i386.deb wine-installer

rm -rf ~/wine
mv ~/Downloads/wine-installer/opt/wine* ~/wine
rm -rf wine-installer
cd ~

sudo ln -s ~/wine/bin/wine /usr/local/bin/wine
sudo ln -s ~/wine/bin/wineboot /usr/local/bin/wineboot
sudo ln -s ~/wine/bin/winecfg /usr/local/bin/winecfg
sudo ln -s ~/wine/bin/wineserver /usr/local/bin/wineserver
sudo chmod +x /usr/local/bin/wine /usr/local/bin/wineboot /usr/local/bin/winecfg /usr/local/bin/wineserver

wget https://raw.githubusercontent.com/Winetricks/winetricks/master/src/winetricks
sudo mv winetricks /usr/local/bin/
sudo chmod +x /usr/local/bin/winetricks


sudo apt-get -y install winbind

# Run Wine Config
wineboot --init
winecfg

########################################   AirMail   ########################################

wget http://siriuscyber.net/sailmail/amsm35054b.exe

# Install AirMail
wine amsm35054b.exe                                 

# To run AirMail
cd ~/.wine/drive_c/Program\ Files/Airmail
wine Airmail3.exe



