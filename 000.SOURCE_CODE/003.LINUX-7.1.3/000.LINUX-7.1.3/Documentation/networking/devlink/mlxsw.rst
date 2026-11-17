.. SPDX-License-Identifier: GPL-2.0

=====================
mlxsw devlink support
=====================

(mlxsw devlink 支持)

This document describes the devlink features implemented by the ``mlxsw``
device driver.
(本文档描述 ``mlxsw`` 设备驱动所实现的 devlink 特性。)

Parameters
(参数)
==========

.. list-table:: Generic parameters implemented

   * - Name (名称)
     - Mode (模式)
   * - ``fw_load_policy``
     - driverinit

The ``mlxsw`` driver also implements the following driver-specific
parameters.
(``mlxsw`` 驱动还实现了以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``acl_region_rehash_interval``
     - u32
     - runtime
     - Sets an interval for periodic ACL region rehashes. The value is
       specified in milliseconds, with a minimum of ``3000``. The value of
       ``0`` disables periodic work entirely. The first rehash will be run
       immediately after the value is set.
       (为周期性的 ACL 区域重哈希 (rehash) 设置间隔。该值以毫秒为单位
       指定，最小值为 ``3000``。值 ``0`` 会完全禁用周期性工作。在该值
       设置之后会立即运行第一次重哈希。)

The ``mlxsw`` driver supports reloading via ``DEVLINK_CMD_RELOAD``
(``mlxsw`` 驱动支持通过 ``DEVLINK_CMD_RELOAD`` 进行重载)

Info versions
(信息版本)
=============

The ``mlxsw`` driver reports the following versions
(``mlxsw`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``hw.revision``
     - fixed
     - The hardware revision for this board
       (此板卡的硬件版本)
   * - ``fw.psid``
     - fixed
     - Firmware PSID
       (固件 PSID)
   * - ``fw.version``
     - running
     - Three digit firmware version
       (三位数字的固件版本)

Line card auxiliary device info versions
(线卡辅助设备信息版本)
========================================

The ``mlxsw`` driver reports the following versions for line card auxiliary device
(``mlxsw`` 驱动为线卡辅助设备报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``hw.revision``
     - fixed
     - The hardware revision for this line card
       (此线卡的硬件版本)
   * - ``ini.version``
     - running
     - Version of line card INI loaded
       (已加载的线卡 INI 版本)
   * - ``fw.psid``
     - fixed
     - Line card device PSID
       (线卡设备 PSID)
   * - ``fw.version``
     - running
     - Three digit firmware version of line card device
       (线卡设备的三位数字固件版本)

Driver-specific Traps
(驱动特定陷阱)
=====================

.. list-table:: List of Driver-specific Traps Registered by ``mlxsw``
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``irif_disabled``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed from a disabled router interface (RIF). This can happen during
       RIF dismantle, when the RIF is first disabled before being removed
       completely
       (捕获那些设备决定丢弃的数据包，因为它们需要从一个已禁用的路由接口
       (RIF) 路由。这可能发生在 RIF 拆卸期间，即 RIF 在被完全移除之前
       先被禁用时)
   * - ``erif_disabled``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed through a disabled router interface (RIF). This can happen during
       RIF dismantle, when the RIF is first disabled before being removed
       completely
       (捕获那些设备决定丢弃的数据包，因为它们需要通过一个已禁用的路由接口
       (RIF) 路由。这可能发生在 RIF 拆卸期间，即 RIF 在被完全移除之前
       先被禁用时)
