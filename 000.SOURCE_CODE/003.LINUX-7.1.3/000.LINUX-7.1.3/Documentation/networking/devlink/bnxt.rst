.. SPDX-License-Identifier: GPL-2.0

=======================================
bnxt devlink support(bnxt devlink 支持)
=======================================

This document describes the devlink features implemented by the ``bnxt``
device driver.
(本文档描述 ``bnxt`` 设备驱动所实现的 devlink 功能。)

Parameters(参数)
================

.. list-table:: Generic parameters implemented

   * - Name (名称)
     - Mode (模式)
   * - ``enable_sriov``
     - Permanent
   * - ``ignore_ari``
     - Permanent
   * - ``msix_vec_per_pf_max``
     - Permanent
   * - ``msix_vec_per_pf_min``
     - Permanent
   * - ``enable_remote_dev_reset``
     - Runtime
   * - ``enable_roce``
     - Permanent

The ``bnxt`` driver also implements the following driver-specific
parameters.
(``bnxt`` 驱动还实现了以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``gre_ver_check``
     - Boolean
     - Permanent
     - Generic Routing Encapsulation (GRE) version check will be enabled in
       the device. If disabled, the device will skip the version check for
       incoming packets.
       (设备中将启用通用路由封装 (GRE) 版本检查。若禁用，设备将跳过对传入数据包的版本检查。)

Info versions(Info 版本)
========================

The ``bnxt_en`` driver reports the following versions
(``bnxt_en`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
      :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``board.id``
     - fixed
     - Part number identifying the board design (标识板卡设计的部件号)
   * - ``asic.id``
     - fixed
     - ASIC design identifier (ASIC 设计标识符)
   * - ``asic.rev``
     - fixed
     - ASIC design revision (ASIC 设计修订版本)
   * - ``fw.psid``
     - stored, running
     - Firmware parameter set version of the board (板卡的固件参数集版本)
   * - ``fw``
     - stored, running
     - Overall board firmware version (板卡的整体固件版本)
   * - ``fw.mgmt``
     - stored, running
     - NIC hardware resource management firmware version (NIC 硬件资源管理固件版本)
   * - ``fw.mgmt.api``
     - running
     - Minimum firmware interface spec version supported between driver and firmware (驱动与固件之间支持的最低固件接口规范版本)
   * - ``fw.nsci``
     - stored, running
     - General platform management firmware version (通用平台管理固件版本)
   * - ``fw.roce``
     - stored, running
     - RoCE management firmware version (RoCE 管理固件版本)
