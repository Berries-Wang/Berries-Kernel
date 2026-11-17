.. SPDX-License-Identifier: GPL-2.0

=====================================
qed devlink support(qed devlink 支持)
=====================================

This document describes the devlink features implemented by the ``qed`` core
device driver.
(本文档描述 ``qed`` 核心设备驱动所实现的 devlink 特性。)

Parameters(参数)
================

The ``qed`` driver implements the following driver-specific parameters.
(``qed`` 驱动实现了以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``iwarp_cmt``
     - Boolean
     - runtime
     - Enable iWARP functionality for 100g devices. Note that this impacts
       L2 performance, and is therefore not enabled by default.
       (为 100G 设备启用 iWARP 功能。注意这会影响 L2 性能，因此默认情况下不启用。)
