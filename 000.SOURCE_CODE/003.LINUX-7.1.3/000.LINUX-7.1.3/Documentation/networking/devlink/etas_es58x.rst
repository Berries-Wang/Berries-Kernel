.. SPDX-License-Identifier: GPL-2.0

===================================================
etas_es58x devlink support(etas_es58x devlink 支持)
===================================================

This document describes the devlink features implemented by the
``etas_es58x`` device driver.
(本文档描述 ``etas_es58x`` 设备驱动所实现的 devlink 功能。)

Info versions(Info 版本)
========================

The ``etas_es58x`` driver reports the following versions
(``etas_es58x`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw``
     - running
     - Version of the firmware running on the device. Also available
       through ``ethtool -i`` as the first member of the
       ``firmware-version``.
       (设备上正在运行的固件的版本。也可以通过 ``ethtool -i`` 查看，是 ``firmware-version`` 的第一个成员。)
   * - ``fw.bootloader``
     - running
     - Version of the bootloader running on the device. Also available
       through ``ethtool -i`` as the second member of the
       ``firmware-version``.
       (设备上正在运行的引导加载程序 (bootloader) 的版本。也可以通过 ``ethtool -i`` 查看，是 ``firmware-version`` 的第二个成员。)
   * - ``board.rev``
     - fixed
     - The hardware revision of the device.
       (设备的硬件修订版本。)
   * - ``serial_number``
     - fixed
     - The USB serial number. Also available through ``lsusb -v``.
       (USB 序列号。也可以通过 ``lsusb -v`` 查看。)
