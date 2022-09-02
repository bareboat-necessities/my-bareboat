#!/bin/bash -e

mkdir  opencpn-5.6.2
cd  opencpn-5.6.2
sudo apt update
sudo apt install devscripts equivs
dget -u -x http://deb.debian.org/debian/pool/main/o/opencpn/opencpn_5.6.2+dfsg-2.dsc
cd opencpn-5.6.2+dfsg
sudo mk-build-deps -i -r
sudo rm opencpn-build-deps_5.6.2+dfsg-2_arm64.buildinfo opencpn-build-deps_5.6.2+dfsg-2_arm64.changes
debuild -us -uc

