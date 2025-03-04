#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn_sensors_hub_B0

usage() {
   echo "Usage: $0 -p ttyPort"
   echo -e "\t-p ttyPort of esp32 device ex: /dev/ttyUSB0"
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

if [ -d /tmp/bbn_sensors_hub_B0 ]; then
  rm -rf /tmp/bbn_sensors_hub_B0/
fi

mkdir /tmp/bbn_sensors_hub_B0 && cd /tmp/bbn_sensors_hub_B0

wget https://github.com/bareboat-necessities/bbn_sensors_hub_B0/releases/download/v1.0.0/bbn_sensors_hub_B0_bin-2025-03-04.zip
unzip bbn_sensors_hub_B0_bin-2025-03-04.zip
/srv/esphome/bin/esptool.py --chip esp32 \
 --port "$parameterP" --baud 1500000 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_sensors_hub_B0_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn_sensors_hub_B0/
