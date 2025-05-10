#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn-wave-period-esp32

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

if [ -d /tmp/bbn-flash-m5-wave ]; then
  rm -rf /tmp/bbn-flash-m5-wave/
fi

mkdir /tmp/bbn-flash-m5-wave && cd /tmp/bbn-flash-m5-wave

wget https://github.com/bareboat-necessities/bbn-wave-period-esp32/releases/download/v1.7.2/bbn_wave_freq_m5atomS3_bin-2025-05-09.zip
unzip bbn_wave_freq_m5atomS3_bin-2025-05-09.zip
/srv/esphome/bin/esptool.py --chip esp32s3 \
 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn_wave_freq_m5atomS3_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn-flash-m5-wave/
