#!/bin/bash

# See http://plippo.de/dev_twofing
# and https://github.com/Plippo/twofing
# and https://github.com/sjlongland/twofing

sudo apt install build-essential libx11-dev libxtst-dev libxi-dev x11proto-randr-dev libxrandr-dev
wget http://plippo.de/dwl/twofing/twofing-0.1.2.tar.gz
tar -xvzf twofing-0.1.2.tar.gz
cd twofing-0.1.2
make
sudo cp twofing /usr/bin/
sudo cat /proc/bus/input/devices # Find the device name to use as matching string, find Vendor and Product IDs
sudo bash -c 'cat << EOF > /etc/udev/rules.d/70-touchscreen-argonautM7.rules
SUBSYSTEMS=="usb",ACTION=="add",KERNEL=="event*",ATTRS{idVendor}=="0000",ATTRS{idProduct}=="0009",SYMLINK+="twofingtouch",RUN+="/bin/chmod a+r /dev/twofingtouch"
KERNEL=="event*",ATTRS{name}=="Argonaut. Touchscreen",SYMLINK+="twofingtouch",RUN+="/bin/chmod a+r /dev/twofingtouch"
EOF'
sudo apt install xserver-xorg-input-evdev xinput-calibrator
sudo bash -c 'cat << EOF > /usr/share/X11/xorg.conf.d/90-touchinput.conf
Section "InputClass"
    Identifier "calibration"
    Driver "evdev"
    MatchProduct "Argonaut. Touchscreen"
    MatchDevicePath "/dev/input/event*"
    Option "Calibration" "272 9847 186 10033"
    Option "Emulate3Buttons" "True"
    Option "EmulateThirdButton" "1"
    Option "EmulateThirdButtonTimeout" "750"
    Option "EmulateThirdButtonMoveThreshold" "30"
EndSection
EOF'
sudo bash -c 'cat << EOF > /etc/udev/rules.d/99-input-tagging.rules
ACTION=="add", KERNEL=="event*", SUBSYSTEM=="input", TAG+="systemd", , ENV{SYSTEMD_ALIAS}+="/sys/subsystem/input/devices/$env{ID_SERIAL}"
EOF'
cat << EOF >> ~/.config/autostart/twofing-startup.desktop
[Desktop Entry]
Type=Application
Name=Twofing
Exec=twofing
StartupNotify=false
EOF
reboot