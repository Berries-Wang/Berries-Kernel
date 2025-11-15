#!/bin/bash
TOOL_CHAIN_BIN=/home/wei/apps/001.Arm-ToolChain/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu

export ARCH=arm64
export CROSS_COMPILE=aarch64-none-linux-gnu-
export PATH=${PATH}:${TOOL_CHAIN_BIN}/bin

make clean

make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu-  defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu-  menuconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu-  -j8

# 在Makefile文件中添加 KBUILD_CFLAGS += -save-temps ， 就可以将 .i (宏展开文件) .S 文件保存 ， 
# 注意，.h 不会生成。
#
#
# 或者 make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- KCFLAGS="-save-temps" -j8
#