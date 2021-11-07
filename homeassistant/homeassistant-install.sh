#!/bin/bash -e

sudo apt-get install -y python3 python3-dev python3-venv python3-pip libffi-dev libssl-dev libjpeg-dev \
   zlib1g-dev autoconf build-essential libopenjp2-7 libtiff5 libturbojpeg0 tzdata

sudo useradd -rm homeassistant -G dialout,gpio,i2c

sudo mkdir -p /srv/homeassistant
sudo chown homeassistant:homeassistant /srv/homeassistant

sudo -u homeassistant -H -s
cd /srv/homeassistant
python3 -m venv .
source bin/activate

python3 -m pip install wheel

pip3 install homeassistant

hass

