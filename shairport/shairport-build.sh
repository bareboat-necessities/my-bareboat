#!/bin/bash

apt-get install -y --no-install-recommends build-essential git autoconf automake libtool \
  libpopt-dev libconfig-dev \
  libasound2-dev avahi-daemon libavahi-client-dev libssl-dev libsoxr-dev libjack-dev \
  libsndio-dev libao-dev libsoundio-dev libpipewire-0.3-dev

git clone -b 4.3.2 https://github.com/mikebrady/shairport-sync.git

cd shairport-sync

autoreconf -fi

./configure --sysconfdir=/etc --with-soxr --with-avahi --with-ssl=openssl --with-systemd \
  --with-alsa --with-sndio --with-pa --with-pw --with-ao --with-jack --with-soundio --with-stdout --with-pipe

make
