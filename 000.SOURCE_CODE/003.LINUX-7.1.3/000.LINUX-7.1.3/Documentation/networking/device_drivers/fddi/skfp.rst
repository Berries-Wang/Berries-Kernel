.. SPDX-License-Identifier: GPL-2.0

.. include:: <isonum.txt>

=================================================
SysKonnect driver - SKFP(SysKonnect 驱动 —— SKFP)
=================================================

|copy| Copyright 1998-2000 SysKonnect,
(|copy| 版权所有 1998-2000 SysKonnect，)

skfp.txt created 11-May-2000
(skfp.txt 创建于 2000 年 5 月 11 日)

Readme File for skfp.o v2.06
(skfp.o v2.06 的自述文件)


.. This file contains

   (1) OVERVIEW
   (2) SUPPORTED ADAPTERS
   (3) GENERAL INFORMATION
   (4) INSTALLATION
   (5) INCLUSION OF THE ADAPTER IN SYSTEM START
   (6) TROUBLESHOOTING
   (7) FUNCTION OF THE ADAPTER LEDS
   (8) HISTORY


1. Overview(1. 概述)
====================

This README explains how to use the driver 'skfp' for Linux with your
network adapter.
(本自述文件解释如何在 Linux 中配合你的网络适配器使用 'skfp' 驱动。)

Chapter 2: Contains a list of all network adapters that are supported by
this driver.
(第 2 章：列出本驱动所支持的所有网络适配器。)

Chapter 3:
(第 3 章：)

	   Gives some general information.
	   (提供一些一般性信息。)

Chapter 4: Describes common problems and solutions.
(第 4 章：描述常见问题与解决方法。)

Chapter 5: Shows the changed functionality of the adapter LEDs.
(第 5 章：展示适配器 LED 功能的变化。)

Chapter 6: History of development.
(第 6 章：开发历史。)


2. Supported adapters(2. 支持的适配器)
======================================

The network driver 'skfp' supports the following network adapters:
SysKonnect adapters:
('skfp' 网络驱动支持以下网络适配器：
SysKonnect 适配器：)

  - SK-5521 (SK-NET FDDI-UP)
  - SK-5522 (SK-NET FDDI-UP DAS)
  - SK-5541 (SK-NET FDDI-FP)
  - SK-5543 (SK-NET FDDI-LP)
  - SK-5544 (SK-NET FDDI-LP DAS)
  - SK-5821 (SK-NET FDDI-UP64)
  - SK-5822 (SK-NET FDDI-UP64 DAS)
  - SK-5841 (SK-NET FDDI-FP64)
  - SK-5843 (SK-NET FDDI-LP64)
  - SK-5844 (SK-NET FDDI-LP64 DAS)

Compaq adapters (not tested):
(Compaq 适配器（未经测试）：)

  - Netelligent 100 FDDI DAS Fibre SC
  - Netelligent 100 FDDI SAS Fibre SC
  - Netelligent 100 FDDI DAS UTP
  - Netelligent 100 FDDI SAS UTP
  - Netelligent 100 FDDI SAS Fibre MIC


3. General Information(3. 一般信息)
===================================

From v2.01 on, the driver is integrated in the linux kernel sources.
Therefore, the installation is the same as for any other adapter
supported by the kernel.
(从 v2.01 起，该驱动已集成到 Linux 内核源码中。因此，其安装方式与内核支持的任何其他适配器相同。)

Refer to the manual of your distribution about the installation
of network adapters.
(网络适配器的安装请参阅你的发行版手册。)

Makes my life much easier :-)
(这让我的生活轻松多了 :-))

4. Troubleshooting(4. 故障排除)
===============================

If you run into problems during installation, check those items:
(如果在安装过程中遇到问题，请检查以下事项：)

Problem:
(问题：)

	  The FDDI adapter cannot be found by the driver.
	  (驱动找不到 FDDI 适配器。)

Reason:
(原因：)

	  Look in /proc/pci for the following entry:
	  (在 /proc/pci 中查找以下条目：)

	     'FDDI network controller: SysKonnect SK-FDDI-PCI ...'

	  If this entry exists, then the FDDI adapter has been
	  found by the system and should be able to be used.
	  (如果该条目存在，说明系统已经找到该 FDDI 适配器，应当可以使用。)

	  If this entry does not exist or if the file '/proc/pci'
	  is not there, then you may have a hardware problem or PCI
	  support may not be enabled in your kernel.
	  (如果该条目不存在，或文件 '/proc/pci' 不存在，那么你可能遇到了硬件问题，或者内核中未启用 PCI 支持。)

	  The adapter can be checked using the diagnostic program
	  which is available from the SysKonnect web site:
	  (可以使用 SysKonnect 网站上提供的诊断程序来检查该适配器：)

	      www.syskonnect.de

	  Some COMPAQ machines have a problem with PCI under
	  Linux. This is described in the 'PCI howto' document
	  (included in some distributions or available from the
	  www, e.g. at 'www.linux.org') and no workaround is available.
	  (一些 COMPAQ 机器在 Linux 下存在 PCI 问题。这在 'PCI howto' 文档（包含在某些发行版中，或可从网上获得，例如 'www.linux.org'）中有描述，且没有可用的变通方法。)

Problem:
(问题：)

	  You want to use your computer as a router between
	  multiple IP subnetworks (using multiple adapters), but
	  you cannot reach computers in other subnetworks.
	  (你想把计算机用作多个 IP 子网之间的路由器（使用多个适配器），但无法访问其他子网中的计算机。)

Reason:
(原因：)

	  Either the router's kernel is not configured for IP
	  forwarding or there is a problem with the routing table
	  and gateway configuration in at least one of the
	  computers.
	  (或者是路由器的内核没有配置 IP 转发，或者是至少一台计算机上的路由表和网关配置有问题。)

If your problem is not listed here, please contact our
technical support for help.
(如果你遇到的问题未在此列出，请联系我们的技术支持寻求帮助。)

You can send email to: linux@syskonnect.de
(你可以发送邮件至：linux@syskonnect.de)

When contacting our technical support,
please ensure that the following information is available:
(联系我们的技术支持时，请确保提供以下信息：)

- System Manufacturer and Model
  (系统制造商和型号)
- Boards in your system
  (系统中的板卡)
- Distribution
  (发行版)
- Kernel version
  (内核版本)


5. Function of the Adapter LEDs(5. 适配器 LED 的功能)
=====================================================

	The functionality of the LED's on the FDDI network adapters was
	changed in SMT version v2.82. With this new SMT version, the yellow
	LED works as a ring operational indicator. An active yellow LED
	indicates that the ring is down. The green LED on the adapter now
	works as a link indicator where an active GREEN LED indicates that
	the respective port has a physical connection.
	(FDDI 网络适配器上 LED 的功能在 SMT 版本 v2.82 中发生了变化。在这个新的 SMT 版本中，黄色 LED 用作环运行指示器。黄色 LED 亮起表示环已停机。适配器上的绿色 LED 现在用作链路指示器，绿色 LED 亮起表示相应端口具有物理连接。)

	With versions of SMT prior to v2.82 a ring up was indicated if the
	yellow LED was off while the green LED(s) showed the connection
	status of the adapter. During a ring down the green LED was off and
	the yellow LED was on.
	(在 v2.82 之前的 SMT 版本中，如果黄色 LED 熄灭而绿色 LED 显示适配器的连接状态，则表示环已建立。环停机期间，绿色 LED 熄灭，黄色 LED 亮起。)

	All implementations indicate that a driver is not loaded if
	all LEDs are off.
	(在所有实现中，如果所有 LED 都熄灭，则表示驱动未加载。)


6. History(6. 历史)
===================

v2.06 (20000511) (In-Kernel version)
    New features:
    (新特性：)

	- 64 bit support
	  (64 位支持)
	- new pci dma interface
	  (新的 PCI DMA 接口)
	- in kernel 2.3.99
	  (包含于内核 2.3.99)

v2.05 (20000217) (In-Kernel version)
    New features:
    (新特性：)

	- Changes for 2.3.45 kernel
	  (针对 2.3.45 内核的修改)

v2.04 (20000207) (Standalone version)
    New features:
    (新特性：)

	- Added rx/tx byte counter
	  (新增收/发字节计数器)

v2.03 (20000111) (Standalone version)
    Problems fixed:
    (修复的问题：)

	- Fixed printk statements from v2.02
	  (修正了 v2.02 中的 printk 语句)

v2.02 (991215) (Standalone version)
    Problems fixed:
    (修复的问题：)

	- Removed unnecessary output
	  (移除了不必要的输出)
	- Fixed path for "printver.sh" in makefile
	  (修正了 makefile 中 "printver.sh" 的路径)

v2.01 (991122) (In-Kernel version)
    New features:
    (新特性：)

	- Integration in Linux kernel sources
	  (集成到 Linux 内核源码中)
	- Support for memory mapped I/O.
	  (支持内存映射 I/O。)

v2.00 (991112)
    New features:
    (新特性：)

	- Full source released under GPL
	  (完整源码以 GPL 发布)

v1.05 (991023)
    Problems fixed:
    (修复的问题：)

	- Compilation with kernel version 2.2.13 failed
	  (修复了使用内核版本 2.2.13 编译失败的问题)

v1.04 (990427)
    Changes:
    (变更：)

	- New SMT module included, changing LED functionality
	  (包含新的 SMT 模块，改变了 LED 功能)

    Problems fixed:
    (修复的问题：)

	- Synchronization on SMP machines was buggy
	  (修复了 SMP 机器上同步存在缺陷的问题)

v1.03 (990325)
    Problems fixed:
    (修复的问题：)

	- Interrupt routing on SMP machines could be incorrect
	  (修复了 SMP 机器上中断路由可能不正确的问题)

v1.02 (990310)
    New features:
    (新特性：)

	- Support for kernel versions 2.2.x added
	  (新增对内核版本 2.2.x 的支持)
	- Kernel patch instead of private duplicate of kernel functions
	  (改用内核补丁，而不是私自复制内核函数)

v1.01 (980812)
    Problems fixed:
    (修复的问题：)

	Connection hangup with telnet
	(telnet 连接挂断)
	Slow telnet connection
	(telnet 连接缓慢)

v1.00 beta 01 (980507)
    New features:
    (新特性：)

	None.
	(无。)

    Problems fixed:
    (修复的问题：)

	None.
	(无。)

    Known limitations:
    (已知限制：)

	- tar archive instead of standard package format (rpm).
	  (使用 tar 归档而不是标准软件包格式 (rpm)。)
	- FDDI statistic is empty.
	  (FDDI 统计信息为空。)
	- not tested with 2.1.xx kernels
	  (未在 2.1.xx 内核上测试)
	- integration in kernel not tested
	  (未测试内核集成)
	- not tested simultaneously with FDDI adapters from other vendors.
	  (未与其他厂商的 FDDI 适配器同时进行测试)
	- only X86 processors supported.
	  (仅支持 X86 处理器。)
	- SBA (Synchronous Bandwidth Allocator) parameters can
	  not be configured.
	  (无法配置 SBA (同步带宽分配器) 参数。)
	- does not work on some COMPAQ machines. See the PCI howto
	  document for details about this problem.
	  (在某些 COMPAQ 机器上无法工作。有关此问题的详情请参阅 PCI howto 文档。)
	- data corruption with kernel versions below 2.0.33.
	  (在低于 2.0.33 的内核版本上会出现数据损坏。)
