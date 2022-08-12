#!/usr/bin/bash -e

# See: https://forum.armbian.com/topic/19526-how-to-install-box86-box64-wine32-wine64-winetricks-on-arm64/

sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install -y git cmake cabextract gcc-arm-linux-gnueabihf libc6-dev-armhf-cross

{
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8580&key=61d35a8d10b75dcd080f73ff061a9d2a" 
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8581&key=03b1d32e8620bbe0620cb99cb547746f" 
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8582&key=dca9fbfc0a49a7e31d09783902e20204" 
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8583&key=f61a84ed20203e142f5a63667c7625a5" 
} > arm-linux-gnueabihf.7z

sudo 7z x arm-linux-gnueabihf.7z

sudo mv arm-linux-gnueabihf/* /lib/arm-linux-gnueabihf/

cd
git clone https://github.com/ptitSeb/box86
cd box86
mkdir build
cd build
cmake .. -DARM_DYNAREC=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j4
sudo make install
sudo systemctl restart systemd-binfmt
cd


#cd
#git clone https://github.com/ptitSeb/box64
#cd box64
#mkdir build
#cd build 
#cmake .. -DARM_DYNAREC=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
#make -j4
#sudo make install
#sudo systemctl restart systemd-binfmt
#cd



cd
cd ~/Downloads
wget https://dl.winehq.org/wine-builds/debian/dists/buster/main/binary-i386/wine-stable-i386_6.0.2~buster-1_i386.deb
wget https://dl.winehq.org/wine-builds/debian/dists/buster/main/binary-i386/wine-stable_6.0.2~buster-1_i386.deb
#wget https://dl.winehq.org/wine-builds/debian/dists/buster/main/binary-amd64/wine-stable-amd64_6.0.2~buster-1_amd64.deb
#wget https://dl.winehq.org/wine-builds/debian/dists/buster/main/binary-amd64/wine-stable_6.0.2~buster-1_amd64.deb
dpkg-deb -xv wine-stable-i386_6.0.2~buster-1_i386.deb wine-installer
dpkg-deb -xv wine-stable_6.0.2~buster-1_i386.deb wine-installer
#dpkg-deb -xv wine-stable-amd64_6.0.2~buster-1_amd64.deb wine-installer
#dpkg-deb -xv wine-stable_6.0.2~buster-1_amd64.deb wine-installer
mv ~/Downloads/wine-installer/opt/wine* ~/wine
rm -rf wine-installer
cd


sudo ln -s ~/wine/bin/wine /usr/local/bin/wine
sudo ln -s ~/wine/bin/wineboot /usr/local/bin/wineboot
sudo ln -s ~/wine/bin/winecfg /usr/local/bin/winecfg
sudo ln -s ~/wine/bin/wineserver /usr/local/bin/wineserver
sudo chmod +x /usr/local/bin/wine /usr/local/bin/wineboot /usr/local/bin/winecfg /usr/local/bin/wineserver


WINEPREFIX=~/.wine WINEARCH=win32 wine winecfg
WINEPREFIX=~/.wine64 WINEARCH=win64 wine winecfg

