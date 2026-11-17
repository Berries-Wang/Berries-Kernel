.. SPDX-License-Identifier: GPL-2.0

=========================
octeontx2 devlink support
=========================

(octeontx2 devlink 支持)

This document describes the devlink features implemented by the ``octeontx2 AF, PF and VF``
device drivers.
(本文档描述 ``octeontx2 AF, PF and VF`` 设备驱动所实现的 devlink 功能。)

Parameters
==========
(参数)

The ``octeontx2 PF and VF`` drivers implement the following driver-specific parameters.
(``octeontx2 PF and VF`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``mcam_count``
     - u16
     - runtime
     - Select number of match CAM entries to be allocated for an interface.
       The same is used for ntuple filters of the interface. Supported by
       PF and VF drivers.
       (选择要为一个接口分配的匹配 CAM 条目数量。它也用于该接口的 ntuple 过滤器。PF 和 VF 驱动均支持。)

The ``octeontx2 AF`` driver implements the following driver-specific parameters.
(``octeontx2 AF`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``dwrr_mtu``
     - u32
     - runtime
     - Use to set the quantum which hardware uses for scheduling among transmit queues.
       Hardware uses weighted DWRR algorithm to schedule among all transmit queues.
       (用于设置硬件在发送队列之间调度时所使用的量子 (quantum)。硬件使用加权 DWRR 算法在所有发送队列之间进行调度。)
   * - ``npc_mcam_high_zone_percent``
     - u8
     - runtime
     - Use to set the number of high priority zone entries in NPC MCAM that can be allocated
       by a user, out of the three priority zone categories high, mid and low.
       (用于在 NPC MCAM 的高、中、低三个优先级区域类别中，设置用户可分配的高优先级区域条目数量。)
   * - ``npc_def_rule_cntr``
     - bool
     - runtime
     - Use to enable or disable hit counters for the default rules in NPC MCAM.
       Its not guaranteed that counters gets enabled and mapped to all the default rules,
       since the counters are scarce and driver follows a best effort approach.
       The default rule serves as the primary packet steering rule for a specific PF or VF,
       based on its DMAC address which is installed by AF driver as part of its initialization.
       Sample command to read hit counters for default rule from debugfs is as follows,
       cat /sys/kernel/debug/cn10k/npc/mcam_rules
       (用于启用或禁用 NPC MCAM 中默认规则的命中计数器。由于计数器稀缺且驱动采用尽力而为的方式，不保证计数器能被启用并映射到所有默认规则。默认规则作为特定 PF 或 VF 的主要数据包引流规则，基于其 DMAC 地址，由 AF 驱动在初始化时安装。从 debugfs 读取默认规则命中计数器的示例命令如下，
       cat /sys/kernel/debug/cn10k/npc/mcam_rules)
   * - ``nix_maxlf``
     - u16
     - runtime
     - Use to set the maximum number of LFs in NIX hardware block. This would be useful
       to increase the availability of default resources allocated to enabled LFs like
       MCAM entries for example.
       (用于设置 NIX 硬件块中 LF 的最大数量。这有助于提高分配给已启用 LF 的默认资源的可用性，例如 MCAM 条目。)

The ``octeontx2 PF`` driver implements the following driver-specific parameters.
(``octeontx2 PF`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``unicast_filter_count``
     - u8
     - runtime
     - Set the maximum number of unicast filters that can be programmed for
       the device. This can be used to achieve better device resource
       utilization, avoiding over consumption of unused MCAM table entries.
       (设置可为设备编程的单播过滤器最大数量。这可用于实现更好的设备资源利用，避免过度消耗未使用的 MCAM 表条目。)
