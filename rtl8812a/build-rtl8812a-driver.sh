sudo apt-get install raspberrypi-kernel-headers

cd ~

git clone https://github.com/aircrack-ng/rtl8812au

cd rtl8812au

sed -i 's/CONFIG_PLATFORM_I386_PC = y/CONFIG_PLATFORM_I386_PC = n/g' Makefile
sed -i 's/CONFIG_PLATFORM_ARM64_RPI = n/CONFIG_PLATFORM_ARM64_RPI = y/g' Makefile

export ARCH=arm

sudo ln -s /usr/src/linux-headers-5.10.103+ /lib/modules/5.10.103-v8+/build

make

sudo make install
