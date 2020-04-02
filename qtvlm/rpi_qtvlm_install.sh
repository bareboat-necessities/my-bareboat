#!/bin/bash

# See: https://www.meltemus.com

curl https://www.meltemus.com/index.php/en/download/send/9-raspberrypi/197-qtvlm-5-9-7-6 > qtVlm-5.9.7-p2-rpi.tar.gz
gzip -cd < qtVlm-5.9.7-p2-rpi.tar.gz | tar xvf -
curl https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/master/qtvlm-conf/qtVlm.ini > /home/pi/.qtVlm/qtVlm.ini

# cd qtVlm
# ./qtVlm -platform xcb
