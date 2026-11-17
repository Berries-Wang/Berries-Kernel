.. SPDX-License-Identifier: GPL-2.0

=========================================================
kvaser_pciefd devlink support(kvaser_pciefd devlink 支持)
=========================================================

This document describes the devlink features implemented by the
``kvaser_pciefd`` device driver.
(本文档描述 ``kvaser_pciefd`` 设备驱动所实现的 devlink 特性。)

Info versions(信息版本)
=======================

The ``kvaser_pciefd`` driver reports the following versions
(``kvaser_pciefd`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw``
     - running
     - Version of the firmware running on the device. Also available
       through ``ethtool -i`` as ``firmware-version``.
       (设备上运行的固件版本。也可通过 ``ethtool -i`` 以 ``firmware-version`` 查看。)
