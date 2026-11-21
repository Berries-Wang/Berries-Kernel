.. SPDX-License-Identifier: GPL-2.0

=====================================================
Device Initialization (devinit)(设备初始化 (devinit))
=====================================================
The devinit process is complex and subject to change. This document provides a high-level
overview using the Ampere GPU family as an example. The goal is to provide a conceptual
overview of the process to aid in understanding the corresponding kernel code.
(devinit 过程复杂且可能变化。本文档以 Ampere GPU 家族为例给出高层次的概述，
目的是提供对该过程的概念性介绍，以帮助理解相应的内核代码。)

Device initialization (devinit) is a crucial sequence of register read/write operations
that occur after a GPU reset. The devinit sequence is essential for properly configuring
the GPU hardware before it can be used.
(设备初始化 (devinit) 是 GPU 复位之后发生的一系列关键的寄存器读/写操作。
devinit 序列对于在 GPU 硬件可用之前正确配置它是必不可少的。)

The devinit engine is an interpreter program that typically runs on the PMU (Power Management
Unit) microcontroller of the GPU. This interpreter executes a "script" of initialization
commands. The devinit engine itself is part of the VBIOS ROM in the same ROM image as the
FWSEC (Firmware Security) image (see fwsec.rst and vbios.rst) and it runs before the
nova-core driver is even loaded. On an Ampere GPU, the devinit ucode is separate from the
FWSEC ucode. It is launched by FWSEC, which runs on the GSP in 'heavy-secure' mode, while
devinit runs on the PMU in 'light-secure' mode.
(devinit 引擎是一个解释器程序，通常运行在 GPU 的 PMU (电源管理单元)
微控制器上。该解释器执行一段由初始化命令组成的"脚本"。devinit 引擎本身是
VBIOS ROM 的一部分，与 FWSEC (固件安全) 镜像位于同一 ROM 镜像中 (参见
fwsec.rst 和 vbios.rst)，并且在 nova-core 驱动被加载之前就运行。在 Ampere
GPU 上，devinit 微码 (ucode) 与 FWSEC 微码是分开的。它由 FWSEC 启动——
FWSEC 以 'heavy-secure' 模式运行在 GSP 上，而 devinit 以 'light-secure'
模式运行在 PMU 上。)

Key Functions of devinit(devinit 的关键功能)
--------------------------------------------
devinit performs several critical tasks:
(devinit 执行若干关键任务：)

1. Programming VRAM memory controller timings
   (对 VRAM 内存控制器时序进行编程)
2. Power sequencing
   (电源时序控制)
3. Clock and PLL (Phase-Locked Loop) configuration
   (时钟与 PLL (锁相环) 配置)
4. Thermal management
   (热管理)

Low-level Firmware Initialization Flow(底层固件初始化流程)
--------------------------------------------------------------------------------------
Upon reset, several microcontrollers on the GPU (such as PMU, SEC2, GSP, etc.) run GPU
firmware (gfw) code to set up the GPU and its core parameters. Most of the GPU is
considered unusable until this initialization process completes.
(复位之后，GPU 上的几个微控制器 (如 PMU、SEC2、GSP 等) 运行 GPU 固件 (gfw)
代码来设置 GPU 及其核心参数。在此初始化过程完成之前，GPU 的大部分被认为
不可用。)

These low-level GPU firmware components are typically:
(这些底层 GPU 固件组件通常：)

1. Located in the VBIOS ROM in the same ROM partition (see vbios.rst and fwsec.rst).
   (位于 VBIOS ROM 中同一 ROM 分区内 (参见 vbios.rst 和 fwsec.rst)。)
2. Executed in sequence on different microcontrollers:
   (在不同微控制器上按顺序执行：)

  - The devinit engine typically but not necessarily runs on the PMU.
    (devinit 引擎通常但未必运行在 PMU 上。)
  - On an Ampere GPU, the FWSEC typically runs on the GSP (GPU System Processor) in
    heavy-secure mode.
    (在 Ampere GPU 上，FWSEC 通常以 heavy-secure 模式运行在 GSP (GPU 系统
    处理器) 上。)

Before the driver can proceed with further initialization, it must wait for a signal
indicating that core initialization is complete (known as GFW_BOOT). This signal is
asserted by the FWSEC running on the GSP in heavy-secure mode.
(在驱动继续进行后续初始化之前，它必须等待指示核心初始化完成的信号 (称为
GFW_BOOT)。该信号由运行在 GSP 上、处于 heavy-secure 模式的 FWSEC 发出。)

Runtime Considerations(运行时注意事项)
----------------------------------------------------------------------------------------
It's important to note that the devinit sequence also needs to run during suspend/resume
operations at runtime, not just during initial boot, as it is critical to power management.
(需要注意的是，devinit 序列不仅在初次启动时需要运行，在运行时的挂起/恢复
操作期间也需要运行，因为它对电源管理至关重要。)

Security and Access Control(安全与访问控制)
-------------------------------------------------------------------------------------
The initialization process involves careful privilege management. For example, before
accessing certain completion status registers, the driver must check privilege level
masks. Some registers are only accessible after secure firmware (FWSEC) lowers the
privilege level to allow CPU (LS/low-secure) access. This is the case, for example,
when receiving the GFW_BOOT signal.
(初始化过程涉及细致的特权管理。例如，在访问某些完成状态寄存器之前，驱动
必须检查特权级别掩码。有些寄存器只有在安全固件 (FWSEC) 将特权级别降低以
允许 CPU (LS/低安全) 访问之后才能访问。接收 GFW_BOOT 信号时就是这种情况。)