#!/bin/bash

# See: https://www.meltemus.com

wget https://www.meltemus.com/index.php/en/download/send/9-raspberrypi/197-qtvlm-5-9-7-6
mv 197-qtvlm-5-9-7-6 qtVlm-5.9.7-p2-rpi.tar.gz

gzip -cd < qtVlm-5.9.7-p2-rpi.tar.gz | tar xvf -

# cd qtVlm
# ./qtVlm_touchscreen -platform xcb
