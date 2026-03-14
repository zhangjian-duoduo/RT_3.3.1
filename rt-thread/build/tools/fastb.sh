#!/bin/bash

bootmode=`awk -F= '/CONFIG_FH_FAST_BOOT/{print $2}' .config`
cp build/partload.ld build/baseos.ld
if [ "x$bootmode" == "xy" ];then
    # fastboot
    sed -i '/text patch for fastboot/a\\t*libsensor.a:(.text .text.*)' build/baseos.ld
    sed -i '/text patch for fastboot/a\\t*sensors/lib*.a:(.text .text.*)' build/baseos.ld
    sed -i '/text patch for fastboot/a\\t*.a:*.?sp_init.o(.text .text.*)' build/baseos.ld
    sed -i '/text patch for fastboot/a\\t*.a:*.?sp_app.o(.text .text.*)' build/baseos.ld
    sed -i '/rodata patch for fastboot/a\\t*libsensor.a:(.rodata .rodata.*)' build/baseos.ld
    sed -i '/rodata patch for fastboot/a\\t*sensors/lib*.a:(.rodata .rodata.*)' build/baseos.ld
    sed -i '/rodata patch for fastboot/a\\t*.a:*.?sp_init.o(.rodata .rodata.*)' build/baseos.ld
    sed -i '/rodata patch for fastboot/a\\t*.a:*.?sp_app.o(.rodata .rodata.*)' build/baseos.ld
    sed -i '/rwdata patch for fastboot/a\\t*(sensor_hex)' build/baseos.ld
    sed -i '/rwdata patch for fastboot/a\\t*libsensor.a:(.data .data.*)' build/baseos.ld
    sed -i '/rwdata patch for fastboot/a\\t*sensors/lib*.a:(.data .data.*)' build/baseos.ld
    sed -i '/rwdata patch for fastboot/a\\t*.a:*.?sp_init.o(.data .data.*)' build/baseos.ld
    sed -i '/rwdata patch for fastboot/a\\t*.a:*.?sp_app.o(.data .data.*)' build/baseos.ld
else
    # part load
    sed -i '/text patch for fastboot/a\\t*libvmm_rtt.a:(.text .text.*)' build/baseos.ld
    sed -i '/rodata patch for fastboot/a\\t*libvmm_rtt.a:(.rodata .rodata.*)' build/baseos.ld
    sed -i '/rwdata patch for fastboot/a\\t*libvmm_rtt.a:(.data .data.*)' build/baseos.ld
fi
