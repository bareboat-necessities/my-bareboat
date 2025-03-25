#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn_sensors_hub_C

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

if [ -d /tmp/bbn_sensors_hub_C ]; then
  rm -rf /tmp/bbn_sensors_hub_C/
fi

mkdir /tmp/bbn_sensors_hub_C && cd /tmp/bbn_sensors_hub_C

wget https://github.com/bareboat-necessities/bbn_sensors_hub_C/releases/download/v0.1.0/bbn_sensors_hub_C_bin-2025-03-25.zip
unzip bbn_sensors_hub_C_bin-2025-03-25.zip
/srv/esphome/bin/esptool.py --chip esp32s3 \
 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_sensors_hub_C_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn_sensors_hub_C/
