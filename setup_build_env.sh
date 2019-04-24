#!/bin/bash

#this script installs gnuradio companion and its dependencies, as well as the new RSA API

#install gnuradio and its dependencies:
apt-get install -y build-essential gnuradio libusb-1.0-0 libusb-1.0-0-dev libusb-dev libudev1 libudev-dev libboost-all-dev cmake gnuradio-dev doxygen swig3.0

#setup the RSA API
mkdir RSA_API-0.13.211
tar xvzf "RSA_API-0.13.211.tar.gz" -C ./RSA_API-0.13.211

mkdir -p /opt/include
cp RSA_API-0.13.211/RSA_API.h /opt/include
chmod -R 777 /opt/include

mkdir -p /opt/lib
cp RSA_API-0.13.211/libRSA_API.so /opt/lib
chmod -R 777 /opt/include

cp RSA_API-0.13.211/cyusb.conf /etc/

# Setup the plugdev group
groupadd plugdev
usermod -a -G plugdev $(whoami)
touch /etc/udev/rules.d/Tektronix.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0699", MODE="0666", GROUP="plugdev", ATTR{power/autosuspend}="-1", ATTR{power/control}="on", ATTR{bMaxPower}="500mA"' >> /etc/udev/rules.d/Tektronix.rules
chmod a+r /etc/udev/rules.d/Tektronix.rules

# Build the RSA Blocks
mkdir oot/gr-tekRSA/build
cd oot/gr-tekRSA/build
cmake ..
make
make install
cd ../../..


