.. SPDX-License-Identifier: GPL-2.0

=========================================
ionic devlink support(ionic devlink 支持)
=========================================

This document describes the devlink features implemented by the ``ionic``
device driver.
(本文档描述 ``ionic`` 设备驱动所实现的 devlink 功能。)

Info versions(Info 版本)
========================

The ``ionic`` driver reports the following versions
(``ionic`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw``
     - running
     - Version of firmware running on the device
       (设备上正在运行的固件的版本)
   * - ``asic.id``
     - fixed
     - The ASIC type for this device
       (该设备的 ASIC 类型)
   * - ``asic.rev``
     - fixed
     - The revision of the ASIC for this device
       (该设备 ASIC 的修订版本)
