#!/bin/bash -e

myArch=$(dpkg --print-architecture)

FID=804
if [ "armhf" != "$myArch" ] ; then
    FID=556
    sudo apt-get -y install libsystemd0
fi

# See: https://www.meltemus.com

cd /home/user
echo "Downloading..."

curl -q -O - "https://www.meltemus.com/index.php/en/download?task=download.send&id=${FID}&catid=9&m=0" \
 -H 'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:127.0) Gecko/20100101 Firefox/127.0' \
 -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8' \
 -H 'Accept-Language: en-US,en;q=0.5' -H 'Accept-Encoding: gzip, deflate, br, zstd' \
 -H 'DNT: 1' -H 'Sec-GPC: 1' -H 'Connection: keep-alive' \
 -H 'Upgrade-Insecure-Requests: 1' -H 'Sec-Fetch-Dest: document' -H 'Sec-Fetch-Mode: navigate' \
 -H 'Sec-Fetch-Site: same-origin' -H 'Sec-Fetch-User: ?1' -H 'Priority: u=1' > qtVlm-rpi.tar.gz

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
