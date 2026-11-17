.. SPDX-License-Identifier: GPL-2.0

====================
hns3 devlink support
====================

(hns3 devlink 支持)

This document describes the devlink features implemented by the ``hns3``
device driver.
(本文档描述 ``hns3`` 设备驱动所实现的 devlink 功能。)

The ``hns3`` driver supports reloading via ``DEVLINK_CMD_RELOAD``.
(``hns3`` 驱动支持通过 ``DEVLINK_CMD_RELOAD`` 进行重载。)

Info versions
=============
(Info 版本)

The ``hns3`` driver reports the following versions
(``hns3`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 10 10 80

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw``
     - running
     - Used to represent the firmware version.
       (用于表示固件版本。)
   * - ``fw.scc``
     - running
     - Used to represent the Soft Congestion Control (SSC) firmware version.
       SCC is a firmware component which provides multiple RDMA congestion
       control algorithms, including DCQCN.
       (用于表示软拥塞控制 (SSC) 固件版本。SCC 是一个固件组件，提供包括 DCQCN 在内的多种 RDMA 拥塞控制算法。)
