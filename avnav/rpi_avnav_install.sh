#!/bin/bash

# See https://github.com/wellenvogel/avnav
# and https://github.com/free-x/avnav/wiki
# and https://github.com/free-x

sudo wget -q -O - https://open-mind.space/repo/open-mind.space.gpg.key |  sudo apt-key add -

sudo bash -c 'cat << EOF > /etc/apt/sources.list.d/extra.list
deb https://open-mind.space/repo/ buster-stable avnav
EOF'

sudo apt update
sudo apt install avnav

# sudo nano /var/lib/avnav/avnav_server.xml
#
# <AVNSocketReader host="localhost" port="10110"/>
#
#
# <AVNHttpServer navurl="/viewer/avnav_navi.php" httpPort="8099" upzoom="0"
#
# sudo systemctl start avnav
#
# Open browser with URL: http://localhost:8099/
#
