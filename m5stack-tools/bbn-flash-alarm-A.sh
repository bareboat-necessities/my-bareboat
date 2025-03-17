#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn_alarm_A

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

if [ -d /tmp/bbn_alarms_A ]; then
  rm -rf /tmp/bbn_alarms_A/
fi

mkdir /tmp/bbn_alarms_A && cd /tmp/bbn_alarms_A

wget https://github.com/bareboat-necessities/bbn_alarms_A/releases/download/vTest/bbn_alarms_A_bin_firmware-2025-03-17.zip
unzip bbn_alarms_A_bin_firmware-2025-03-17.zip
/srv/esphome/bin/esptool.py --chip esp32s3 \
 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_alarms_A.merged.bin

cd "$m_dir"
rm -rf /tmp/bbn_alarms_A/
