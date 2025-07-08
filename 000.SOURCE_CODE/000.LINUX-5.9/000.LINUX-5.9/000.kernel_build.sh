#!/bin/bash
make clean
export ARCH=arm
export CROSS_COMPILE=aarch64-linux-gnu-
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  menuconfig -j4
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j4
