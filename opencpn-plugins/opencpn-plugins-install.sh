#!/bin/bash -e

xmlstarlet sel -t \
 -m "/plugins/plugin[normalize-space(target-arch)='armhf' and normalize-space(target)!='android-armhf' and normalize-space(target-version)=10]" \
 -v "normalize-space(version)" -o ' ' \
 -v "normalize-space(api-version)" -o ' ' \
 -v "normalize-space(target)" -o ' ' \
 -v "normalize-space(target-version)" -o ' ' \
 -v "normalize-space(name)" -o ' ' \
 -v "normalize-space(tarball-url)" \
 -n  ~/.opencpn/ocpn-plugins.xml | sort 

echo ==================================================

xmlstarlet sel -t \
 -m "/plugins/plugin" \
 -v "normalize-space(version)" -o ' ' \
 -v "normalize-space(api-version)" -o ' ' \
 -v "normalize-space(target)" -o ' ' \
 -v "normalize-space(target-version)" -o ' ' \
 -v "normalize-space(name)" -o ' ' \
 -v "normalize-space(tarball-url)" \
 -n  ~/.opencpn/ocpn-plugins.xml | sort | grep armhf | grep -v stretch | grep rasp 

echo ==================================================

xmlstarlet sel -t \
 -m "/plugins/plugin[normalize-space(target-arch)='armhf' and normalize-space(target)!='android-armhf' and normalize-space(target-version)=10]" \
 -v "normalize-space(name)" -o ' ' \
 -n  ~/.opencpn/ocpn-plugins.xml | sort 
