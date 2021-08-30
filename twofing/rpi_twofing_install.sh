#!/bin/bash

# See http://plippo.de/dev_twofing
# and https://github.com/Plippo/twofing
# and https://github.com/sjlongland/twofing

sudo apt-get -y install build-essential libx11-dev libxtst-dev libxi-dev x11proto-randr-dev libxrandr-dev git
git clone https://github.com/Plippo/twofing.git

cd twofing || exit 255
make
sudo cp twofing /usr/bin/
sudo cat /proc/bus/input/devices # Find the device name to use as matching string, find Vendor and Product IDs
sudo bash -c 'cat << EOF > /etc/udev/rules.d/70-touchscreen.rules
SUBSYSTEMS=="input", KERNEL=="event[0-9]*", ENV{ID_INPUT_TOUCHSCREEN}=="1", SYMLINK+="touchscreen0"
SUBSYSTEMS=="input", KERNEL=="event[0-9]*", ENV{ID_INPUT_TOUCHSCREEN}=="1", SYMLINK+="twofingtouch", RUN+="/bin/chmod a+r /dev/twofingtouch"
EOF'
sudo apt-get -y install xserver-xorg-input-evdev xinput-calibrator

# To find name for MatchProduct:
#
# udevadm info -a -n /dev/touchscreen0 | grep "ATTRS{name}" | sed -e 's#.*=="##' -e 's#"$##'
#
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
mkdir -p ~/.config/autostart
cat << EOF > ~/.config/autostart/twofing-startup.desktop
[Desktop Entry]
Type=Application
Name=Twofing
Exec=twofing
StartupNotify=false
EOF

rm -rf twofing
#reboot