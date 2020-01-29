#!/bin/bash

# enable aux audio
amixer cset numid=3 1

# Additional marine related utilities
sudo apt install predict gpsd rtl-ais klex

# Additional utilities
sudo apt install nmap git xmlstarlet

# Install command line utility to show current location
sudo apt install jq
sudo bash -c 'cat << EOF > /usr/bin/gps-loc
#!/bin/bash
curl -s http://localhost:3000/signalk/v1/api/vessels/self/navigation/position/ | jq -M -jr '\''.value.latitude," ",.value.longitude','" ",.timestamp'\''
EOF'
sudo chmod +x /usr/bin/gps-loc

# TODO: Does it help with reboots???
sudo apt install uhubctl
sudo bash -c 'cat << EOF > /lib/systemd/system-shutdown/hub-off.sh
#!/bin/bash
sudo /sbin/uhubctl -l 2 -p 4 -a 0
echo "1-1" | sudo tee /sys/bus/usb/drivers/usb/unbind
EOF'
sudo chmod +x /lib/systemd/system-shutdown/hub-off.sh
