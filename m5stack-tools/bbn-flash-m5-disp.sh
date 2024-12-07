#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn-m5stack-tough

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

if [ -d /tmp/bbn-flash-m5-disp ]; then
  rm -rf /tmp/bbn-flash-m5-disp/
fi

mkdir /tmp/bbn-flash-m5-disp && cd /tmp/bbn-flash-m5-disp

wget https://github.com/bareboat-necessities/bbn-m5stack-tough/releases/download/main/bbn_m5tough_active_boat_bin-2024-12-03.zip
unzip bbn_m5tough_active_boat_bin-2024-12-03.zip
/srv/esphome/bin/esptool.py \
 --chip esp32 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_m5tough_active_boat_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn-flash-m5-disp/
