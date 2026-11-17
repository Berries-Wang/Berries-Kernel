.. SPDX-License-Identifier: GPL-2.0

===================
nfp devlink support
===================

(nfp devlink 支持)

This document describes the devlink features implemented by the ``nfp``
device driver.
(本文档描述 ``nfp`` 设备驱动所实现的 devlink 功能。)

Parameters
==========
(参数)

.. list-table:: Generic parameters implemented

   * - Name (名称)
     - Mode (模式)
   * - ``fw_load_policy``
     - permanent
   * - ``reset_dev_on_drv_probe``
     - permanent

Info versions
=============
(Info 版本)

The ``nfp`` driver reports the following versions
(``nfp`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``board.id``
     - fixed
     - Identifier of the board design (板卡设计的标识符)
   * - ``board.rev``
     - fixed
     - Revision of the board design (板卡设计的修订版本)
   * - ``board.manufacture``
     - fixed
     - Vendor of the board design (板卡设计的厂商)
   * - ``board.model``
     - fixed
     - Model name of the board design (板卡设计的型号名称)
   * - ``board.part_number``
     - fixed
     - Part number of the board and its components (板卡及其组件的部件号)
   * - ``fw.bundle_id``
     - stored, running
     - Firmware bundle id (固件 bundle 标识)
   * - ``fw.mgmt``
     - stored, running
     - Version of the management firmware (管理固件的版本)
   * - ``fw.cpld``
     - stored, running
     - The CPLD firmware component version (CPLD 固件组件版本)
   * - ``fw.app``
     - stored, running
     - The APP firmware component version (APP 固件组件版本)
   * - ``fw.undi``
     - stored, running
     - The UNDI firmware component version (UNDI 固件组件版本)
   * - ``fw.ncsi``
     - stored, running
     - The NSCI firmware component version (NSCI 固件组件版本)
   * - ``chip.init``
     - stored, running
     - The CFGR firmware component version (CFGR 固件组件版本)
