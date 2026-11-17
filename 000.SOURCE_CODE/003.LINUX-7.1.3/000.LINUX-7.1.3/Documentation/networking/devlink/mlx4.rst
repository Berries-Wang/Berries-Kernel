.. SPDX-License-Identifier: GPL-2.0

=======================================
mlx4 devlink support(mlx4 devlink 支持)
=======================================

This document describes the devlink features implemented by the ``mlx4``
device driver.
(本文档描述 ``mlx4`` 设备驱动所实现的 devlink 功能。)

Parameters(参数)
================

.. list-table:: Generic parameters implemented

   * - Name (名称)
     - Mode (模式)
   * - ``internal_err_reset``
     - driverinit, runtime
   * - ``max_macs``
     - driverinit
   * - ``region_snapshot_enable``
     - driverinit, runtime

The ``mlx4`` driver also implements the following driver-specific
parameters.
(``mlx4`` 驱动还实现了以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``enable_64b_cqe_eqe``
     - Boolean
     - driverinit
     - Enable 64 byte CQEs/EQEs, if the FW supports it.
       (如果固件支持，则启用 64 字节 CQE/EQE。)
   * - ``enable_4k_uar``
     - Boolean
     - driverinit
     - Enable using the 4k UAR.
       (启用使用 4k UAR。)

The ``mlx4`` driver supports reloading via ``DEVLINK_CMD_RELOAD``
(``mlx4`` 驱动支持通过 ``DEVLINK_CMD_RELOAD`` 进行重载)

Regions(区域)
=============

The ``mlx4`` driver supports dumping the firmware PCI crspace and health
buffer during a critical firmware issue.
(``mlx4`` 驱动支持在发生严重固件问题时转储固件 PCI crspace 和健康缓冲区。)

In case a firmware command times out, firmware getting stuck, or a non zero
value on the catastrophic buffer, a snapshot will be taken by the driver.
(如果固件命令超时、固件卡死，或灾难性缓冲区中出现非零值，驱动将会拍摄一个快照。)

The ``cr-space`` region will contain the firmware PCI crspace contents. The
``fw-health`` region will contain the device firmware's health buffer.
Snapshots for both of these regions are taken on the same event triggers.
(``cr-space`` 区域将包含固件 PCI crspace 的内容。 ``fw-health`` 区域将包含设备固件的健康缓冲区。这两个区域的快照都在相同的事件触发条件下拍摄。)
