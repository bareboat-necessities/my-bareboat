#!/bin/bash -e

echo "Installing DRM"

# DRM management
# https://www.widevine.com/

# DRM
sudo apt-get install -y -q libwidevinecdm0

echo "Checking touchscreen fixes"
if [ -L /dev/twofingtouch ]
then
  MATCH_PRODUCT=$(udevadm info -a -n /dev/twofingtouch | grep "ATTRS{name}" | sed -e 's#.*=="##' -e 's#"$##')
  sudo bash -c 'cat << EOF > /usr/share/X11/xorg.conf.d/90-touchinput.conf
Section "InputClass"
    Identifier "calibration"
    Driver "evdev"
    MatchProduct "'"${MATCH_PRODUCT}"'"
    MatchDevicePath "/dev/input/event*"
    Option "EmulateThirdButton" "1"
    Option "EmulateThirdButtonTimeout" "750"
    Option "EmulateThirdButtonMoveThreshold" "30"
EndSection
EOF'
fi

if [ -f /usr/lib/opencpn/liblogbookkonni_pi.so ]
then
  sudo rm -f /usr/lib/opencpn/libLogbookKonni_pi.so
fi

# rpi-clone
git clone https://github.com/bareboat-necessities/rpi-clone.git
cd rpi-clone
sudo cp rpi-clone rpi-clone-setup /usr/local/sbin
cd ..
sudo chmod +x /usr/local/sbin/rpi-clone*
rm -rf rpi-clone

#/home/user/add-ons/scytalec-inmarsat-install.sh

echo "Reboot for changes to take effect"
