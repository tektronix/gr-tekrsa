#!/bin/bash
apt-get install -y build-essential gnuradio libusb-1.0-0 libusb-1.0-0-dev libusb-dev libudev1 libudev-dev libboost-all-dev cmake gnuradio-dev doxygen

unzip -o "MAR 28 Ubuntu 15 Bob_v1.0 (1).zip"

mkdir -p /opt/include
cp "MAR 28 Ubuntu 15 Bob_v1.0/include"/* /opt/include
chmod -R 777 /opt/include

mkdir -p /opt/lib
cp "MAR 28 Ubuntu 15 Bob_v1.0/lib/"/* /opt/lib
cp libBaseDSPL.a /opt/lib
chmod -R 777 /opt/include

