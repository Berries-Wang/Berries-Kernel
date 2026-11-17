.. SPDX-License-Identifier: GPL-2.0

=================================================
mv88e6xxx devlink support(mv88e6xxx devlink 支持)
=================================================

This document describes the devlink features implemented by the ``mv88e6xxx``
device driver.
(本文档描述 ``mv88e6xxx`` 设备驱动所实现的 devlink 功能。)

Parameters(参数)
================

The ``mv88e6xxx`` driver implements the following driver-specific parameters.
(``mv88e6xxx`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``ATU_hash``
     - u8
     - runtime
     - Select one of four possible hashing algorithms for MAC addresses in
       the Address Translation Unit. A value of 3 may work better than the
       default of 1 when many MAC addresses have the same OUI. Only the
       values 0 to 3 are valid for this parameter.
       (为地址转换单元 (Address Translation Unit) 中的 MAC 地址从四种可能的哈希算法中选择一种。当许多 MAC 地址具有相同 OUI 时，取值 3 可能比默认值 1 效果更好。该参数的有效取值只有 0 到 3。)
