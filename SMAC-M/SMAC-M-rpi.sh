#!/bin/bash

# See https://github.com/LarsSchy/SMAC-M

sudo apt-get y install python3 python3-pip python3-venv gdal-bin \
 python3-gdal build-essential libgdal-dev imagemagick xmlstarlet
echo 'PATH="$HOME/.local/bin/:$PATH"' >>~/.bashrc
pip3 install --user pipenv
bash
pipenv install
git clone https://github.com/LarsSchy/SMAC-M
cd ./SMAC-M || exit 255
pipenv install
pipenv run pip install "GDAL<=$(gdal-config --version)"
pipenv shell
ogrinfo --version
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57objectclasses.csv
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57attributes.csv
sudo cp s57objectclasses.csv /usr/share/gdal/s57objectclasses_iw.csv
sudo cp s57attributes.csv /usr/share/gdal/s57attributes_iw.csv
export S57_PROFILE=iw
export OGR_S57_OPTIONS=SPLIT_MULTIPOINT=ON,ADD_SOUNDG_DEPTH=ON

#test
ogrinfo --config S57_PROFILE iw -ro ./datatest/US5OH10M/US5OH10M.000 | grep "SLOTOP"
