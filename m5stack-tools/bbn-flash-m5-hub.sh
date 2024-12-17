#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn_esp32_sensors_hub

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

if [ -d /tmp/bbn_esp32_sensors_hub ]; then
  rm -rf /tmp/bbn_esp32_sensors_hub/
fi

mkdir /tmp/bbn_esp32_sensors_hub && cd /tmp/bbn_esp32_sensors_hub

wget https://github.com/bareboat-necessities/bbn_esp32_sensors_hub/releases/download/v0.1.2/bbn_esp32_sensors_hub_bin-2024-12-17.zip
unzip bbn_esp32_sensors_hub_bin-2024-12-17.zip
/srv/esphome/bin/esptool.py --chip esp32s3 \
 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_esp32_sensors_hub_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn_esp32_sensors_hub/
