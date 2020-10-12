#!/bin/bash

sudo apt-get -y install build-essential cmake bc bison flex \
 at-spi2-core \
 dh-sequence-gir \
 fonts-cantarell \
 gnome-pkg-tools \
 gobject-introspection \
 gtk-doc-tools \
 libcolord-dev \
 libcups2-dev \
 libgdk-pixbuf2.0-dev \
 libgirepository1.0-dev \
 libjson-glib-dev \
 librest-dev \
 libxkbfile-dev \
 sassc \
 xvfb \
 libcairo2-doc \
 xsltproc

git clone -b "debian/2.40.0+dfsg-5" https://salsa.debian.org/gnome-team/gdk-pixbuf.git

cd gdk-pixbuf || exit 255

sudo apt-get -y install dh-exec meson \
    libglib2.0-doc                    \
    libatk-bridge2.0-dev              \
    libatk1.0-dev                     \
    libcairo2-dev                     \
    libegl1-mesa-dev                  \
    libepoxy-dev                      \
    libfontconfig1-dev                \
    libfribidi-dev                    \
    libharfbuzz-dev                   \
    libpango1.0-dev                   \
    libwayland-dev                    \
    libxcomposite-dev                 \
    ibxcursor-dev                     \
    libxdamage-dev                    \
    libxext-dev l                     \
    ibxfixes-dev                      \
    libxi-dev                         \
    libxinerama-dev                   \
    libxkbcommon-dev                  \
    libxml2-utils                     \
    libxrandr-dev                     \
    wayland-protocols                 \
    libatk1.0-doc                     \
    libpango1.0-doc

dpkg-buildpackage -rfakeroot -b -uc -us

sudo dpkg -i ../libgdk-pixbuf2.0-bin_2.40.0+dfsg-5_*.deb
sudo dpkg -i ../libgdk-pixbuf2.0-common_2.40.0+dfsg-5_all.deb
sudo dpkg -i ../libgdk-pixbuf2.0-0_2.40.0+dfsg-5_*.deb
sudo dpkg -i ../gir1.2-gdkpixbuf-2.0_2.40.0+dfsg-5_*.deb
sudo dpkg -i ../libgdk-pixbuf2.0-dev_2.40.0+dfsg-5_*.deb

cd ..

git clone -b "debian/3.24.23-2" https://salsa.debian.org/gnome-team/gtk3.git

cd gtk3 || exit 255

dpkg-buildpackage -rfakeroot -b -uc -us

sudo dpkg -i ../libgtk-3-common_3.24.23-2_all.deb
sudo dpkg -i ../libgtk-3-0_3.24.23-2_*.deb
sudo dpkg -i ../gir1.2-gtk-3.0_3.24.23-2_*.deb
sudo dpkg -i ../libgtk-3-dev_3.24.23-2_*.deb
