#!/bin/bash

# See https://github.com/wellenvogel/avnav
# and https://github.com/free-x/avnav/wiki
# and https://github.com/free-x

sudo wget -q -O - https://open-mind.space/repo/open-mind.space.gpg.key |  sudo apt-key add -

sudo bash -c 'cat << EOF > /etc/apt/sources.list.d/extra.list
deb https://open-mind.space/repo/ buster-stable avnav
EOF'

sudo apt update
sudo apt-get -y install avnav
sudo apt-get -y install xterm
sudo apt-get -y install mpg123

mkdir -p /home/pi/AvNavCharts/out

sudo adduser avnav audio

sudo systemctl enable avnav


# sudo nano /var/lib/avnav/avnav_server.xml
#
# <AVNSocketReader host="localhost" port="10110"/>
#
# <AVNHttpServer navurl="/viewer/avnav_navi.php" httpPort="8099" upzoom="0" chartbase="maps" empty="nonexistent">
#
#     <Directory urlpath="maps" path="/home/pi/AvNavCharts/out"></Directory>
#
#
# sudo systemctl start avnav

sudo apt-get -y install xdotool

#
# Open browser with URL: http://localhost:8099/
#
# chromium-browser -new-window http://localhost:8099; sleep 2; xdotool key F11
#
