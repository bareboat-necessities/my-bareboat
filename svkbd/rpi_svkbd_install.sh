#!/bin/bash

# See https://tools.suckless.org/x/svkbd/
# and https://slackbuilds.org/repository/14.2/accessibility/svkbd/

sudo apt install build-essential libx11-dev libxtst-dev libxi-dev
wget http://ponce.cc/slackware/sources/repo/svkbd-20140130.tar.gz
gzip -cd svkbd-20140130.tar.gz | tar xvf -
cd svkbd-20140130/
export LAYOUT=en; make
export LAYOUT=ru; make
export LAYOUT=de; make
export LAYOUT=arrows; make
sudo cp svkbd-en /usr/bin/
sudo cp svkbd-ru /usr/bin/
sudo cp svkbd-de /usr/bin/
sudo cp svkbd-arrows /usr/bin/
sudo bash -c 'cat << EOF > /usr/bin/toggle-keyboard.sh
#!/bin/bash
PID=\`pidof svkbd-en\`
if [ ! -e \$PID ]; then
  kill \$PID
else
 svkbd-en -g 600x200+1+1 &
fi
EOF'
sudo chmod +x /usr/bin/toggle-keyboard.sh
sudo bash -c 'cat << EOF > /usr/share/raspi-ui-overrides/applications/toggle-keyboard.desktop
[Desktop Entry]
Name=Virtual Keyboard
Comment=Virtual Keyboard
Exec=/usr/bin/toggle-keyboard.sh
Type=Application
Icon=keyboard.png
Categories=Panel;Utility
EOF'
sudo bash -c 'cat << EOF >> /home/pi/.config/lxpanel/LXDE-pi/panels/panel
Plugin {
  type=launchbar
  Config {
    Button {
      id=toggle-keyboard.desktop
    }
  }
}
EOF'
#sudo reboot
