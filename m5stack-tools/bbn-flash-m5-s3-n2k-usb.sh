#!/bin/bash -e

# See: https://github.com/bareboat-necessities/bbn-m5-s3-n2k-usb

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

if [ -d /tmp/bbn-flash-m5-s3-n2k-usb ]; then
  rm -rf /tmp/bbn-flash-m5-s3-n2k-usb/
fi

mkdir /tmp/bbn-flash-m5-s3-n2k-usb && cd /tmp/bbn-flash-m5-s3-n2k-usb

wget https://github.com/bareboat-necessities/bbn-m5-s3-n2k-usb/releases/download/v1.0.0/bbn-m5-s3-n2k-usb_bin-2025-02-14.zip
unzip bbn-m5-s3-n2k-usb_bin-2025-02-14.zip

/srv/esphome/bin/esptool.py --chip esp32s3 \
 --port "$parameterP" --baud 921600 \
 --before default_reset --after hard_reset write_flash \
 0x0 bbn-m5-s3-n2k-usb_firmware.bin

cd "$m_dir"
rm -rf /tmp/bbn-flash-m5-s3-n2k-usb/
