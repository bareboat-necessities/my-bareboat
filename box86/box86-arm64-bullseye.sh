#!/usr/bin/bash -e

# See: https://forum.armbian.com/topic/16584-install-box86-on-arm64/

sudo apt-get -y install schroot debootstrap

sudo mkdir -p /srv/chroot
sudo mkdir -p /srv/chroot/debian-armhf

sudo debootstrap --arch armhf --foreign bullseye /srv/chroot/debian-armhf 

sudo chroot "/srv/chroot/debian-armhf" /debootstrap/debootstrap --second-stage

sudo bash -c 'cat << EOF > /etc/schroot/chroot.d/debian-armhf-on-arm64
[debian-armhf-on-arm64]
description=Debian Armhf chroot
aliases=arm32
type=directory
directory=/srv/chroot/debian-armhf
profile=desktop
personality=linux
preserve-environment=true
root-users=root
users=user
EOF'

sudo bash -c 'cat << EOF > /etc/schroot/desktop/nssdatabases
shadow
gshadow
services
protocols
EOF'

sudo bash -c 'cat << EOF > /srv/chroot/debian-armhf/var/lib/dpkg/statoverride
root root 2755 /usr/bin/crontab
EOF'

{
cat << EOF >> ~/.bashrc
export LANGUAGE="C"
export LC_ALL="C"
export DISPLAY=:0
EOF
} | sudo schroot -c arm32 -u root


sudo schroot -c arm32 -u root -- bash -c "apt-get -y install git wget cmake build-essential python3 gcc-arm-linux-gnueabihf zenity"

sudo schroot -c arm32 -u root -- bash -c "git clone https://github.com/ptitSeb/box86 && cd box86 && mkdir build; cd build; cmake .. -DARM_DYNAREC=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo; make"

sudo schroot -c arm32 -u root -- bash -c "wget https://twisteros.com/wine.tgz && tar zxvf wine.tgz"


exit 0


sudo schroot -c arm32 -u root -- bash -c "ls"



{
 apt-get install sudo
 sudo apt-get update
 sudo apt-get upgrade
} | sudo schroot -c arm32

{
cat << EOF >> ~/.bashrc
export LANGUAGE="C"
export LC_ALL="C"
export DISPLAY=:0
EOF
} | sudo schroot -c arm32 -u root


{
adduser user --quiet --disabled-password 
echo user:changeme | chpasswd
} | sudo schroot -c arm32

{
sudo -u user bash -c 'cat << EOF >> ~/.bashrc
export LANGUAGE="C"
export LC_ALL="C"
export DISPLAY=:0
EOF'  
} | sudo schroot -c arm32



