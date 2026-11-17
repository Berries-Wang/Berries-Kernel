.. SPDX-License-Identifier: GPL-2.0+

=========================================================================================================
RDMA Driver for the AMD Pensando(R) Ethernet adapter family(AMD Pensando(R) 以太网适配器系列的 RDMA 驱动)
=========================================================================================================

AMD Pensando RDMA driver.
(AMD Pensando RDMA 驱动。)
Copyright (C) 2018-2025, Advanced Micro Devices, Inc.

Overview(概述)
==============

The ionic_rdma driver provides Remote Direct Memory Access functionality
for AMD Pensando DSC (Distributed Services Card) devices. This driver
implements RDMA capabilities as an auxiliary driver that operates in
conjunction with the ionic ethernet driver.
(ionic_rdma 驱动为 AMD Pensando DSC (Distributed Services Card,分布式服务卡)设备提供远程直接内存访问 (RDMA) 功能。该驱动以辅助驱动的形式实现 RDMA 能力，与 ionic 以太网驱动协同工作。)

The ionic ethernet driver detects RDMA capability during device
initialization and creates auxiliary devices that the ionic_rdma driver
binds to, establishing the RDMA data path and control interfaces.
(ionic 以太网驱动在设备初始化期间检测 RDMA 能力，并创建供 ionic_rdma 驱动绑定的辅助设备，从而建立 RDMA 数据通路和控制接口。)

Identifying the Adapter(识别适配器)
===================================

See Documentation/networking/device_drivers/ethernet/pensando/ionic.rst
for more information on identifying the adapter.
(有关识别适配器的更多信息，参见 Documentation/networking/device_drivers/ethernet/pensando/ionic.rst)

Enabling the driver(启用驱动)
=============================

The ionic_rdma driver depends on the ionic ethernet driver.
See Documentation/networking/device_drivers/ethernet/pensando/ionic.rst
for detailed information on enabling and configuring the ionic driver.
(ionic_rdma 驱动依赖 ionic 以太网驱动。有关启用和配置 ionic 驱动的详细信息，参见 Documentation/networking/device_drivers/ethernet/pensando/ionic.rst)

The ionic_rdma driver is enabled via the standard kernel configuration system,
using the make command::
(ionic_rdma 驱动通过标准内核配置系统启用，使用 make 命令：)

  make oldconfig/menuconfig/etc.

The driver is located in the menu structure at:
(该驱动位于菜单结构中的以下位置：)

  -> Device Drivers
    -> InfiniBand support
      -> AMD Pensando DSC RDMA/RoCE Support

Support(支持)
=============

For general Linux RDMA support, please use the RDMA mailing
list, which is monitored by AMD Pensando personnel::
(如需一般的 Linux RDMA 支持，请使用由 AMD Pensando 人员关注的 RDMA 邮件列表：)

  linux-rdma@vger.kernel.org
