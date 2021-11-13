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

echo "Reboot for changes to take effect"
