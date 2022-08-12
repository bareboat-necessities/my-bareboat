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
root-groups=root
users=user
EOF'

sudo bash -c 'cat << EOF > /etc/schroot/desktop/nssdatabases
passwd
shadow
group
gshadow
services
protocols
EOF'

sudo bash -c 'cat << EOF > /srv/chroot/debian-armhf/var/lib/dpkg/statoverride
root root 2755 /usr/bin/crontab
EOF'

{
sudo apt-get update
sudo apt-get upgrade
cat << EOF >> ~/.bashrc
export LANGUAGE="C"
export LC_ALL="C"
export DISPLAY=:0
EOF
} | sudo schroot -c arm32


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



