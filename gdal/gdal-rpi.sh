#!/bin/bash

sudo apt-get -y install build-essential cmake \
 dh-python \
 doxygen \
 graphviz \
 chrpath \
 bash-completion \
 libarmadillo-dev \
 libcfitsio-dev \
 libcharls-dev \
 libcurl4-gnutls-dev \
 libdap-dev \
 libdoxygen-filter-perl \
 libepsilon-dev \
 libexpat1-dev \
 libfreexl-dev \
 libfyba-dev \
 libgeos-dev \
 libgeotiff-dev \
 libgif-dev \
 libhdf4-alt-dev \
 libhdf5-dev \
 libjpeg-dev \
 libjson-c-dev \
 libkml-dev \
 liblzma-dev \
 libmodern-perl-perl \
 default-libmysqlclient-dev \
 libnetcdf-dev \
 libogdi3.2-dev \
 libopenjp2-7-dev \
 libpcre3-dev \
 libpng-dev \
 libpoppler-private-dev \
 libpq-dev \
 libproj-dev \
 libqhull-dev \
 libspatialite-dev \
 libsqlite3-dev \
 libtiff-dev \
 liburiparser-dev \
 libwebp-dev \
 libxerces-c-dev \
 libxml2-dev \
 libzstd-dev \
 lsb-release \
 netcdf-bin \
 patch \
 python3-all-dev \
 python3-numpy \
 python3-distutils \
 swig \
 unixodbc-dev \
 zlib1g-dev

git clone -b "6.3.2" https://github.com/OSGeo/PROJ.git

cd PROJ || exit 255

sudo apt-get -y install sqlite3

./autogen.sh
./configure
make -sj4

sudo make install

cd ..

git clone -b "debian/3.1.3+dfsg-1" https://salsa.debian.org/debian-gis-team/gdal.git

cd gdal || exit 255

dpkg-buildpackage -rfakeroot -b -uc -us -d

