#!/bin/bash

# enable aux audio
amixer cset numid=3 1

sudo apt install nmap predict git gpsd xmlstarlet

sudo apt install jq
sudo bash -c 'cat << EOF > /usr/bin/gps-loc
#!/bin/bash
curl -s http://localhost:3000/signalk/v1/api/vessels/self/navigation/position/ | jq -M -jr '\''.value.latitude," ",.value.longitude','" ",.timestamp'\''
EOF'
sudo chmod +x /usr/bin/gps-loc

sudo apt install uhubctl
sudo bash -c 'cat << EOF > /lib/systemd/system-shutdown/hub-off.sh
#!/bin/bash
sudo /sbin/uhubctl -l 2 -p 4 -a 0
echo ''1-1'' | sudo tee /sys/bus/usb/drivers/usb/unbind
EOF'
sudo chmod +x /lib/systemd/system-shutdown/hub-off.sh
