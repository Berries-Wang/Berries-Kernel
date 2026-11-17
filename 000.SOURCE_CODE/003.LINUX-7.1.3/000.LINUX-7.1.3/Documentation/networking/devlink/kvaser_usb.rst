.. SPDX-License-Identifier: GPL-2.0

===================================================
kvaser_usb devlink support(kvaser_usb devlink 支持)
===================================================

This document describes the devlink features implemented by the
``kvaser_usb`` device driver.
(本文档描述 ``kvaser_usb`` 设备驱动所实现的 devlink 功能。)

Info versions(Info 版本)
========================

The ``kvaser_usb`` driver reports the following versions
(``kvaser_usb`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw``
     - running
     - Version of the firmware running on the device. Also available
       through ``ethtool -i`` as ``firmware-version``.
       (设备上正在运行的固件的版本。也可以通过 ``ethtool -i`` 查看，即 ``firmware-version``。)
   * - ``board.rev``
     - fixed
     - The device hardware revision.
       (设备的硬件修订版本。)
   * - ``board.id``
     - fixed
     - The device EAN (product number).
       (设备的 EAN(产品编号)。)
   * - ``serial_number``
     - fixed
     - The device serial number.
       (设备的序列号。)
