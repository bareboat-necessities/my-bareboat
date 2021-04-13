#!/bin/bash -e

##
## Put commands here to be executed on first system boot
## (Useful for advanced OS image customization)
## See: https://bareboat-necessities.github.io/my-bareboat/bareboat-os.html
##

# Locale
perl -pi -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/g' /etc/locale.gen
locale-gen en_US.UTF-8
update-locale en_US.UTF-8

# WiFi Country
perl -pi -e 's/REGDOMAIN=/REGDOMAIN=US/g' /etc/default/crda
sed  -i '1i country=US' /etc/wpa_supplicant/wpa_supplicant.conf

# TimeZone
ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
dpkg-reconfigure -f noninteractive tzdata
