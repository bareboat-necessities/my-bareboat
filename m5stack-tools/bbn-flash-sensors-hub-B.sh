#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn_sensors_hub_B

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

if [ -d /tmp/bbn_sensors_hub_B ]; then
  rm -rf /tmp/bbn_sensors_hub_B/
fi

mkdir /tmp/bbn_sensors_hub_B && cd /tmp/bbn_sensors_hub_B

wget https://github.com/bareboat-necessities/bbn_sensors_hub_B/releases/download/vTest/bbn_sensors_hub_B_bin-2025-03-03.zip
unzip bbn_sensors_hub_B_bin-2025-03-03.zip
/srv/esphome/bin/esptool.py --chip esp32s3 \
 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_sensors_hub_B_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn_sensors_hub_B/
