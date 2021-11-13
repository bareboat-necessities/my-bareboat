#!/bin/bash

packages="
  libbudgietheme0_10.5-2_armhf.deb
  budgie-core-dev_10.5-2_armhf.deb
  budgie-desktop-doc_10.5-2_all.deb
  libbudgie-private0_10.5-2_armhf.deb
  budgie-desktop_10.5-2_all.deb
  gir1.2-budgie-1.0_10.5-2_armhf.deb
  budgie-core_10.5-2_armhf.deb
  libraven0_10.5-2_armhf.deb"

for value in $packages
do
    wget $value
done

curl -1sLf \
  'https://dl.cloudsmith.io/public/bbn-projects/bbn-budgie/setup.deb.sh' \
  | sudo -E distro=debian codename=buster bash

sudo apt update
sudo apt install budgie-desktop
