#!/bin/bash

# ============================================================
# Linux 7.1.3 Kernel Build Script (aarch64)
# ============================================================
# 交叉编译工具链路径（ARM64 GCC 14.3）
TOOL_CHAIN_BIN=/home/wei/apps/001.Arm-ToolChain/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu

# 目标架构
export ARCH=arm64
# 交叉编译前缀
export CROSS_COMPILE=aarch64-none-linux-gnu-
# 将工具链加入 PATH
export PATH=${PATH}:${TOOL_CHAIN_BIN}/bin

# 清理之前的编译产物
make clean

# 生成默认配置（基于 arm64 架构）
make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu-  defconfig
# 交互式菜单配置（可在此勾选/取消内核功能模块）
make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu-  menuconfig

# 编译内核（-j8 表示 8 线程并行编译）
# 如需保存预编译中间文件（.i 宏展开文件、.S 汇编文件），可取消下一行注释：
#make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu-  KCFLAGS="-save-temps" -j8
make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -j8

# ============================================================
# 调试与中间文件说明：
# ============================================================
# 1. 在 Makefile 中添加 KBUILD_CFLAGS += -save-temps，编译后会保留：
#      - .i 文件：预处理后的 C 源码（宏已展开）
#      - .S 文件：编译后的汇编代码
#    注意：.h 头文件不会单独生成中间文件。
#
# 2. 或者在命令行传参：make KCFLAGS="-save-temps" -j8
#
# 3. 查看宏展开内容：
#      aarch64-none-linux-gnu-objdump -S head.o    # 反汇编查看
#      或直接查看 *.i 文件（预编译后的完整展开结果）
# ============================================================