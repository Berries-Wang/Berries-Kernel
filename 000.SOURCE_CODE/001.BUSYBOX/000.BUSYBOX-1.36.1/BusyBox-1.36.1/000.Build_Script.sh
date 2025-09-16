#!/bin/bash
TOOL_CHAIN_BIN=/home/wei/apps/001.Arm-ToolChain/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu

export ARCH=arm64
export CROSS_COMPILE=aarch64-none-linux-gnu-
export PATH=${PATH}:${TOOL_CHAIN_BIN}/bin

make menuconfig
make install