#!/usr/bin/bash -e

# See: https://forum.armbian.com/topic/19526-how-to-install-box86-box64-wine32-wine64-winetricks-on-arm64/

sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install git cmake cabextract gcc-arm-linux-gnueabihf libc6-dev-armhf-cross

{
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8580&key=61d35a8d10b75dcd080f73ff061a9d2a" 
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8581&key=03b1d32e8620bbe0620cb99cb547746f" 
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8582&key=dca9fbfc0a49a7e31d09783902e20204" 
  wget -O - "https://forum.armbian.com/applications/core/interface/file/attachment.php?id=8583&key=f61a84ed20203e142f5a63667c7625a5" 
} > arm-linux-gnueabihf.7z


