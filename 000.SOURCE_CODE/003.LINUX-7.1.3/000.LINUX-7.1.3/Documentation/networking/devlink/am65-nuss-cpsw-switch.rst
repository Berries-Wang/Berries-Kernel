.. SPDX-License-Identifier: GPL-2.0

==============================
am65-cpsw-nuss devlink support
==============================

(am65-cpsw-nuss devlink 支持)

This document describes the devlink features implemented by the ``am65-cpsw-nuss``
device driver.
(本文档描述 ``am65-cpsw-nuss`` 设备驱动所实现的 devlink 功能。)

Parameters
==========
(参数)

The ``am65-cpsw-nuss`` driver implements the following driver-specific
parameters.
(``am65-cpsw-nuss`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``switch_mode``
     - Boolean
     - runtime
     - Enable switch mode
       (启用交换机模式)
