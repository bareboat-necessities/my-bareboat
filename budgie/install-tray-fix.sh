#!/bin/bash

packages="
  libbudgietheme0
  budgie-core-dev
  budgie-desktop-doc
  libbudgie-private0
  budgie-desktop
  gir1.2-budgie-1.0
  budgie-core
  libraven0"

for value in $packages
do
    wget $value
done

curl -1sLf \
  'https://dl.cloudsmith.io/public/bbn-projects/bbn-budgie/setup.deb.sh' \
  | sudo -E distro=debian codename=buster bash

sudo apt update
sudo apt install budgie-core libbudgie-private0
