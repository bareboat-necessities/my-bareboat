#!/usr/bin/bash -e


# See https://github.com/allinurl/gwsocket

wget http://tar.gwsocket.io/gwsocket-0.3.tar.gz
tar -xzvf gwsocket-0.3.tar.gz
cd gwsocket-0.3/
./configure
make
sudo make install


# Usage example:

gwsocket -p 7474 --pipein=out  &
socat - TCP4:localhost:10110 >> out  &
wsdump ws://localhost:7474/





