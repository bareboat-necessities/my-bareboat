#!/usr/bin/bash -e

sudo apt-get -y install schroot debootstrap

sudo mkdir /srv/chroot
sudo mkdir /srv/chroot/debian-armhf

sudo debootstrap --arch armhf --foreign bullseye /srv/chroot/debian-armhf http://debian.xtdv.net/debian

sudo chroot "/srv/chroot/debian-armhf" /debootstrap/debootstrap --second-stage

sudo bash -c 'cat << EOF > /etc/schroot/chroot.d/debian-armhf.conf
[debian-armhf]
description=Debian Armhf chroot
aliases=debian-armhf
type=directory
directory=/srv/chroot/debian-armhf
profile=desktop
personality=linux
preserve-environment=true
root-users=user
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

