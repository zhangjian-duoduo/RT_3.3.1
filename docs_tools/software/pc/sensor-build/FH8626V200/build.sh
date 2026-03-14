#!/bin/bash

source personal.make &> /dev/null
set -e

pushd $SDK_DIR/rt-thread
make fh8626v200_defconfig
popd

cd isp
mkdir -p $RTT_INSTALL_DIR

make clean
make lib RTT=1 MS=0 STATIC=1

make clean
make lib RTT=1 MS=1 STATIC=1
