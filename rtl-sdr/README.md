rtl-sdr
turns your Realtek RTL2832 based DVB dongle into a SDR receiver
======================================================================

For more information see:
http://sdr.osmocom.org/trac/wiki/rtl-sdr

---

# rtl-sdr

This is a version of rtl-sdr driver which support rtl_tcp direct sampling from I/Q branch.

Modified by Jakub/SP5TOF on 07.02.2017 for direct sampling support.

Original from this post: [Direct Sampling Mode in 820T2 DVB-T on Linux os &#8211; SP5TOF &#8211; amateur radio station](http://inteligentny-dom.vxm.pl/sp5tof/?page_id=404), thanks again!

# Usage ( Debian/Ubuntu )

Run as `root` user:

```
root@pc:/home# apt install build-essential git cmake -y
root@pc:/home# git clone https://github.com/bclswl0827/rtl-sdr.git
root@pc:/home# cd rtl-sdr
root@pc:/home/rtl-sdr# mkdir build
root@pc:/home/rtl-sdr/build# cd build
root@pc:/home/rtl-sdr/build# cmake ..
root@pc:/home/rtl-sdr/build# make
root@pc:/home/rtl-sdr/build# make install
root@pc:/home/rtl-sdr/build# ldconfig
root@pc:/home/rtl-sdr/build# bash -c 'echo -e "\n# for RTL-SDR:\nblacklist dvb_usb_rtl28xxu\n" >> /etc/modprobe.d/blacklist.conf'
root@pc:/home/rtl-sdr/build# update-initramfs -u
root@pc:/home/rtl-sdr/build# rmmod dvb_usb_rtl28xxu
root@pc:/home/rtl-sdr/build# reboot
```

```
root@pc:~# rtl_tcp --help
rtl_tcp, an I/Q spectrum server for RTL2832 based DVB-T receivers
Modified by Jakub/SP5TOF on 07.02.2017 for direct sampling support

Usage: [-a listen address]
 [-p listen port (default: 1234)]
 [-f frequency to tune to [Hz]]
 [-g gain (default: 0 for auto)]
 [-s samplerate in Hz (default: 2048000 Hz)]
 [-b number of buffers (default: 32, set by library)]
 [-n max number of linked list buffers to keep (default: 500)]
 [-d device index (default: 0)]
 [-P ppm_error (default: 0)]
 [-i direct sampling(1: I-ADC input enabled), 2: Q-ADC input enabled)]
 [-c AGC Mode(1: ON, 0: OFF), default: 1(ON)
```

Sampling mode can be set in rtl_tcp now...

For example:

```
root@pc:~# rtl_tcp -a 0.0.0.0 -i 2
```

# License

MIT
