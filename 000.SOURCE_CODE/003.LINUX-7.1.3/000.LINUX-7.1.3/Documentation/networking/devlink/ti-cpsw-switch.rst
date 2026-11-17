.. SPDX-License-Identifier: GPL-2.0

==============================
ti-cpsw-switch devlink support
==============================

(ti-cpsw-switch devlink 支持)

This document describes the devlink features implemented by the ``ti-cpsw-switch``
device driver.
(本文档描述 ``ti-cpsw-switch`` 设备驱动所实现的 devlink 功能。)

Parameters
==========
(参数)

The ``ti-cpsw-switch`` driver implements the following driver-specific
parameters.
(``ti-cpsw-switch`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``ale_bypass``
     - Boolean
     - runtime
     - Enables ALE_CONTROL(4).BYPASS mode for debugging purposes. In this
       mode, all packets will be sent to the host port only.
       (为调试目的启用 ALE_CONTROL(4).BYPASS 模式。在该模式下，所有数据包都只会被发送到主机端口。)
   * - ``switch_mode``
     - Boolean
     - runtime
     - Enable switch mode
       (启用交换机模式)
