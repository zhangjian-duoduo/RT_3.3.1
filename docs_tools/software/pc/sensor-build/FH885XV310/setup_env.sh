#!/bin/bash

CUR_DIR=$(pwd)

if [ -z "$SDK_DIR" ]; then
    echo "Error: SDK_DIR is empty. You must export SDK_DIR=/path/to/SDK folder name"
    exit 1
fi

echo "### env ###
export LIB_ROOT=$CUR_DIR/../../../../../rt-thread/lib/fh885xv310/inc
export LIB_ROOT_EN=1
export BUILD_MODE=1

export RTT_ROOT=$SDK_DIR/rt-thread
export RTT_KERNEL=\$(RTT_ROOT)
export RTT_INSTALL_DIR=$CUR_DIR/_output
export ARCH=arm
export CHIP_ID=CH2
export CROSS_COMPILE=arm-fullhanv2-eabi-
export A7=1
export VFP=1
export RTT_CROSS_COMPILE=arm-fullhanv2-eabi-
export RTT=1
export RTT3=1
export RTOSV3=1
" > personal.make

