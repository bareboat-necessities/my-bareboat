#!/bin/bash -e

myArch=$(dpkg --print-architecture)

# See: https://www.meltemus.com

cd /home/user
echo "Downloading..."

AGENT="Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36"
wget --user-agent="$AGENT" -O qtVlm-rpi.tar.gz "https://www.meltemus.com/index.php/en/download?task=download.send&id=804&catid=9&m=0"

gzip -cd < qtVlm-rpi.tar.gz | tar xvf -
mkdir /home/user/.qtVlm 
wget -q -O - https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/master/qtvlm-conf/qtVlm.ini > /home/user/.qtVlm/qtVlm.ini

sudo bash -c 'cat << EOF > /usr/local/share/applications/qtvlm.desktop
[Desktop Entry]
Type=Application
Name=QtVlm
GenericName=QtVlm
Comment=QtVlm ChartPlotter
Exec=sh -c "cd /home/user/qtVlm; ./qtVlm -platform xcb"
Terminal=false
Icon=/home/user/qtVlm/icon/qtVlm_48x48.png
Categories=Navigation;ChartPlotter
Keywords=Navigation;ChartPlotter
EOF'

wget -q -O - http://download.meltemus.com/qtvlm/qtVlm_documentation_en.pdf > /home/user/Documents/qtVlm_documentation_en.pdf

# cd qtVlm
# ./qtVlm -platform xcb
