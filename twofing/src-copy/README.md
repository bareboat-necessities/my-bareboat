# twogfing

twofing is a daemon which runs in the background and recognizes two-finger gestures performed on a touchscreen and converts them into mouse and keyboard events. This way, such gestures can be used in almost all existing applications (even ones where you wouldn’t expect it, like Wine applications) without having to modify them.

# Installation

```
sudo apt-get -y install \
  build-essential \
  libx11-dev \
  libxtst-dev \
  libxi-dev \
  x11proto-randr-dev \
  libxrandr-dev \
  xserver-xorg-input-evdev \
  xserver-xorg-input-evdev-dev
make
sudo make install
```

Create a x11 conf file and att the following section to it

```
Section "InputClass"
  Identifier "calibration"
  Driver "evdev"
  MatchProduct "{{ put your device name here, get it with xinput list }}"

  Option "EmulateThirdButton" "1"
  Option "EmulateThirdButtonTimeout" "750"
  Option "EmulateThirdButtonMoveThreshold" "30"
EndSection
```
