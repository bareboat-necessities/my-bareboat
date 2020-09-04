#!/bin/bash

# See https://fmirkes.github.io/articles/20190827.html

sudo apt install build-essential libevdev2 libevdev-dev
#git clone 'https://github.com/PeterCxy/evdev-right-click-emulation.git'
#git clone 'https://github.com/ixnewton/evdev-right-click-emulation.git'
git clone -b 'hold-right-click' 'https://github.com/pfpacket/evdev-right-click-emulation.git'
cd 'evdev-right-click-emulation' || exit 255
make all
sudo cp 'out/evdev-rce' '/usr/local/bin/'
chmod +x '/usr/local/bin/evdev-rce'

sudo usermod -G 'input' -a pi

echo 'uinput' | sudo tee -a /etc/modules

# /etc/udev/rules.d/99-uinput.rules
sudo bash -c 'cat << EOF > /etc/udev/rules.d/99-uinput.rules
KERNEL=="uinput", MODE="0660", GROUP="input"
EOF'

sudo udevadm control --reload-rules
sudo udevadm trigger

# $HOME/.config/autostart/evdev-rce.desktop

cat << EOF > ~/.config/autostart/evdev-rce.desktop
[Desktop Entry]
Version=1.0
Type=Application
Name=evdev-rce
GenericName=Enable long-press-to-right-click gesture
Exec=env LONG_CLICK_INTERVAL=2000 LONG_CLICK_FUZZ=50 /usr/local/bin/evdev-rce
Terminal=true
StartupNotify=false
EOF