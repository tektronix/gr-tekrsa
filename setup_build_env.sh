#!/bin/bash
set -e

# This script installs gnuradio-companion and its dependencies, as well as the new RSA API

script_dir="$(CDPATH= cd -- $(dirname -- $0) && pwd)"
script_user="$(pstree -lu -s $$ | sed -n 's/[^(]*(\([^)]*\).*/\1/p')"

# Install gnuradio-companion and its dependencies
echo "Installing project dependencies"
apt-get update && \
apt-get install -y \
    build-essential \
    cmake \
    doxygen \
    gnuradio \
    gnuradio-dev \
    libboost-all-dev \
    libudev-dev \
    libudev1 \
    libusb-1.0-0 \
    libusb-1.0-0-dev \
    libusb-dev \
    swig3.0

# Setup the RSA API
echo "Setting up RSA API"

wget https://github.com/tektronix/USB-RSA-gnuradio-module-WIP/releases/download/v0.1.2-alpha/RSA_API-0.13.211.tar.gz
mkdir -p "${script_dir}/RSA_API-0.13.211"
tar xvzf "${script_dir}/RSA_API-0.13.211.tar.gz" -C "${script_dir}/RSA_API-0.13.211"

mkdir -p /opt/include
cp "${script_dir}/RSA_API-0.13.211/RSA_API.h" /opt/include
chmod -R 777 /opt/include

mkdir -p /opt/lib
cp "${script_dir}/RSA_API-0.13.211/libRSA_API.so" /opt/lib
chmod -R 777 /opt/lib

cp "${script_dir}/RSA_API-0.13.211/cyusb.conf" /etc

# Setup the plugdev group
if ! getent group plugdev > /dev/null 2>&1
then
    groupadd plugdev
fi
usermod -a -G plugdev "${script_user}"
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0699", MODE="0666", GROUP="plugdev", ATTR{power/autosuspend}="-1", ATTR{power/control}="on", ATTR{bMaxPower}="500mA"' > /etc/udev/rules.d/Tektronix.rules
chmod a+r /etc/udev/rules.d/Tektronix.rules

# Re-create /dev entries with correct group and mode
udevadm trigger --subsystem-match="usb"

# Build the RSA Blocks
echo "Building the RSA Blocks"
OLDPWD="$(pwd)"
mkdir "${script_dir}/build"
cd "${script_dir}/build"
cmake ..
make
make install
cd "${OLDPWD}"

echo "Success: Setup complete"
