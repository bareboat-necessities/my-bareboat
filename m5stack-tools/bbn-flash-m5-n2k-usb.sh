#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn-nmea200-m5atom/tree/main/bbn-nmea2000-usb-gw-m5atom

usage() {
   echo "Usage: $0 -p ttyPort"
   echo -e "\t-p ttyPort of esp32 device ex: /dev/ttyACM1"
   exit 1
}

while getopts "p:" opt
do
   case "$opt" in
      p ) parameterP="$OPTARG" ;;
      ? ) usage ;;
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$parameterP" ]
then
   echo "Option -p value is required";
   usage
fi

m_dir=$(pwd)

if [ -d /tmp/bbn-flash-m5-n2k-usb ]; then
  rm -rf /tmp/bbn-flash-m5-n2k-usb/
fi

mkdir /tmp/bbn-flash-m5-n2k-usb && cd /tmp/bbn-flash-m5-n2k-usb

wget https://github.com/bareboat-necessities/bbn-nmea200-m5atom/releases/download/main/bbn-nmea200-m5atom_bin-2024-12-07.zip
unzip bbn-nmea200-m5atom_bin-2024-12-07.zip
/srv/esphome/bin/esptool.py \
 --chip esp32 --port "$parameterP" --baud 1500000 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn-nmea2000-usb-gw-m5atom-firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn-flash-m5-n2k-usb/
