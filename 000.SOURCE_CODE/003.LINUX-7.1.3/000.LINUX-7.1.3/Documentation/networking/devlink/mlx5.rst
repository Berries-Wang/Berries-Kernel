.. SPDX-License-Identifier: GPL-2.0

====================
mlx5 devlink support
====================

(mlx5 devlink 支持)

This document describes the devlink features implemented by the ``mlx5``
device driver.
(本文档描述 ``mlx5`` 设备驱动所实现的 devlink 功能。)

Parameters
(参数)
==========

.. list-table:: Generic parameters implemented

   * - Name
     - Mode
     - Validation
     - Notes
   * - ``enable_roce``
     - driverinit
     - Boolean
     - If the device supports RoCE disablement, RoCE enablement state controls
       device support for RoCE capability. Otherwise, the control occurs in the
       driver stack. When RoCE is disabled at the driver level, only raw
       ethernet QPs are supported.
   * - ``io_eq_size``
     - driverinit
     - The range is between 64 and 4096.
     -
   * - ``event_eq_size``
     - driverinit
     - The range is between 64 and 4096.
     -
   * - ``max_macs``
     - driverinit
     - The range is between 1 and 2^31. Only power of 2 values are supported.
     -
   * - ``enable_sriov``
     - permanent
     - Boolean
     - Applies to each physical function (PF) independently, if the device
       supports it. Otherwise, it applies symmetrically to all PFs.
   * - ``total_vfs``
     - permanent
     - The range is between 1 and a device-specific max.
     - Applies to each physical function (PF) independently, if the device
       supports it. Otherwise, it applies symmetrically to all PFs.

Note: permanent parameters such as ``enable_sriov`` and ``total_vfs`` require FW reset to take effect
(Note：诸如 ``enable_sriov`` 和 ``total_vfs`` 这类永久 (permanent) 参数需要
重置固件 (FW reset) 才能生效。)

.. code-block:: bash

   # setup parameters
   devlink dev param set pci/0000:01:00.0 name enable_sriov value true cmode permanent
   devlink dev param set pci/0000:01:00.0 name total_vfs value 8 cmode permanent

   # Fw reset
   devlink dev reload pci/0000:01:00.0 action fw_activate

   # for PCI related config such as sriov PCI reset/rescan is required:
   echo 1 >/sys/bus/pci/devices/0000:01:00.0/remove
   echo 1 >/sys/bus/pci/rescan
   grep ^ /sys/bus/pci/devices/0000:01:00.0/sriov_*

   * - ``num_doorbells``
     - driverinit
     - This controls the number of channel doorbells used by the netdev. In all
       cases, an additional doorbell is allocated and used for non-channel
       communication (e.g. for PTP, HWS, etc.). Supported values are:

       - 0: No channel-specific doorbells, use the global one for everything.
       - [1, max_num_channels]: Spread netdev channels equally across these
         doorbells.

The ``mlx5`` driver also implements the following driver-specific
parameters.
(``mlx5`` 驱动还实现了以下驱动专属参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name
     - Type
     - Mode
     - Description
   * - ``flow_steering_mode``
     - string
     - runtime
     - Controls the flow steering mode of the driver

       * ``dmfs`` Device managed flow steering. In DMFS mode, the HW
         steering entities are created and managed through firmware.
       * ``smfs`` Software managed flow steering. In SMFS mode, the HW
         steering entities are created and manage through the driver without
         firmware intervention.
       * ``hmfs`` Hardware managed flow steering. In HMFS mode, the driver
         is configuring steering rules directly to the HW using Work Queues with
         a special new type of WQE (Work Queue Element).

       SMFS mode is faster and provides better rule insertion rate compared to
       default DMFS mode.
   * - ``fdb_large_groups``
     - u32
     - driverinit
     - Control the number of large groups (size > 1) in the FDB table.

       * The default value is 15, and the range is between 1 and 1024.
   * - ``esw_multiport``
     - Boolean
     - runtime
     - Control MultiPort E-Switch shared fdb mode.

       An experimental mode where a single E-Switch is used and all the vports
       and physical ports on the NIC are connected to it.

       An example is to send traffic from a VF that is created on PF0 to an
       uplink that is natively associated with the uplink of PF1

       Note: Future devices, ConnectX-8 and onward, will eventually have this
       as the default to allow forwarding between all NIC ports in a single
       E-switch environment and the dual E-switch mode will likely get
       deprecated.

       Default: disabled
   * - ``esw_port_metadata``
     - Boolean
     - runtime
     - When applicable, disabling eswitch metadata can increase packet rate up
       to 20% depending on the use case and packet sizes.

       Eswitch port metadata state controls whether to internally tag packets
       with metadata. Metadata tagging must be enabled for multi-port RoCE,
       failover between representors and stacked devices. By default metadata is
       enabled on the supported devices in E-switch. Metadata is applicable only
       for E-switch in switchdev mode and users may disable it when NONE of the
       below use cases will be in use:
       1. HCA is in Dual/multi-port RoCE mode.
       2. VF/SF representor bonding (Usually used for Live migration)
       3. Stacked devices

       When metadata is disabled, the above use cases will fail to initialize if
       users try to enable them.

       Note: Setting this parameter does not take effect immediately. Setting
       must happen in legacy mode and eswitch port metadata takes effect after
       enabling switchdev mode.
   * - ``hairpin_num_queues``
     - u32
     - driverinit
     - We refer to a TC NIC rule that involves forwarding as "hairpin".
       Hairpin queues are mlx5 hardware specific implementation for hardware
       forwarding of such packets.

       Control the number of hairpin queues.
   * - ``hairpin_queue_size``
     - u32
     - driverinit
     - Control the size (in packets) of the hairpin queues.
   * - ``pcie_cong_inbound_high``
     - u16
     - driverinit
     - High threshold configuration for PCIe congestion events. The firmware
       will send an event once device side inbound PCIe traffic went
       above the configured high threshold for a long enough period (at least
       200ms).

       See pci_bw_inbound_high ethtool stat.

       Units are 0.01 %. Accepted values are in range [0, 10000].
       pcie_cong_inbound_low < pcie_cong_inbound_high.
       Default value: 9000 (Corresponds to 90%).
   * - ``pcie_cong_inbound_low``
     - u16
     - driverinit
     - Low threshold configuration for PCIe congestion events. The firmware
       will send an event once device side inbound PCIe traffic went
       below the configured low threshold, only after having been previously in
       a congested state.

       See pci_bw_inbound_low ethtool stat.

       Units are 0.01 %. Accepted values are in range [0, 10000].
       pcie_cong_inbound_low < pcie_cong_inbound_high.
       Default value: 7500.
   * - ``pcie_cong_outbound_high``
     - u16
     - driverinit
     - High threshold configuration for PCIe congestion events. The firmware
       will send an event once device side outbound PCIe traffic went
       above the configured high threshold for a long enough period (at least
       200ms).

       See pci_bw_outbound_high ethtool stat.

       Units are 0.01 %. Accepted values are in range [0, 10000].
       pcie_cong_outbound_low < pcie_cong_outbound_high.
       Default value: 9000 (Corresponds to 90%).
   * - ``pcie_cong_outbound_low``
     - u16
     - driverinit
     - Low threshold configuration for PCIe congestion events. The firmware
       will send an event once device side outbound PCIe traffic went
       below the configured low threshold, only after having been previously in
       a congested state.

       See pci_bw_outbound_low ethtool stat.

       Units are 0.01 %. Accepted values are in range [0, 10000].
       pcie_cong_outbound_low < pcie_cong_outbound_high.
       Default value: 7500.

   * - ``cqe_compress_type``
     - string
     - permanent
     - Configure which mechanism/algorithm should be used by the NIC that will
       affect the rate (aggressiveness) of compressed CQEs depending on PCIe bus
       conditions and other internal NIC factors. This mode affects all queues
       that enable compression.
       * ``balanced`` : Merges fewer CQEs, resulting in a moderate compression ratio but maintaining a balance between bandwidth savings and performance
       * ``aggressive`` : Merges more CQEs into a single entry, achieving a higher compression rate and maximizing performance, particularly under high traffic loads

   * - ``swp_l4_csum_mode``
     - string
     - permanent
     - Configure how the L4 checksum is calculated by the device when using
       Software Parser (SWP) hints for header locations.

       * ``default`` : Use the device's default checksum calculation
         mode. The driver will discover during init whether or
         full_csum or l4_only is in use. Setting this value explicitly
         from userspace is not allowed, but some firmware versions may
         return this value on param read.
       * ``full_csum`` : Calculate full checksum including the pseudo-header
       * ``l4_only`` : Calculate L4-only checksum, excluding the pseudo-header

The ``mlx5`` driver supports reloading via ``DEVLINK_CMD_RELOAD``
(``mlx5`` 驱动支持通过 ``DEVLINK_CMD_RELOAD`` 进行重载。)

Info versions
(信息版本)
=============

The ``mlx5`` driver reports the following versions
(``mlx5`` 驱动报告以下版本信息。)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name
     - Type
     - Description
   * - ``fw.psid``
     - fixed
     - Used to represent the board id of the device.
   * - ``fw.version``
     - stored, running
     - Three digit major.minor.subminor firmware version number.

Health reporters
(健康报告器)
================

tx reporter
(tx 报告器)
-----------

The tx reporter is responsible for reporting and recovering of the following three error scenarios:
(tx 报告器负责报告并恢复以下三种错误场景：)

- tx timeout
  (tx 超时)

    Report on kernel tx timeout detection.
    (报告内核检测到 tx 超时。)
    Recover by searching lost interrupts.
    (通过查找丢失的中断来恢复。)
- tx error completion
  (tx 错误完成)

    Report on error tx completion.
    (报告错误的 tx 完成事件。)
    Recover by flushing the tx queue and reset it.
    (通过冲刷 tx 队列并将其复位来恢复。)
- tx PTP port timestamping CQ unhealthy
  (tx PTP 端口时间戳 CQ 不健康)

    Report too many CQEs never delivered on port ts CQ.
    (报告端口 ts CQ 上有过多 CQE 从未被送达。)
    Recover by flushing and re-creating all PTP channels.
    (通过冲刷并重建所有 PTP 通道来恢复。)

tx reporter also support on demand diagnose callback, on which it provides
real time information of its send queues status.
(tx 报告器还支持按需诊断回调，可实时提供其发送队列状态的信息。)

User commands examples:
(用户命令示例：)

- Diagnose send queues status::
  (诊断发送队列状态：)

    $ devlink health diagnose pci/0000:82:00.0 reporter tx

.. note::
   This command has valid output only when interface is up, otherwise the command has empty output.
   (注意：只有接口处于 up 状态时该命令才有有效输出，否则输出为空。)

- Show number of tx errors indicated, number of recover flows ended successfully,
  is autorecover enabled and graceful period from last recover::
  (显示已指示的 tx 错误数量、成功结束的恢复流程数量、是否启用了自动恢复，以及
  距上次恢复的宽限期：)

    $ devlink health show pci/0000:82:00.0 reporter tx

rx reporter
(rx 报告器)
-----------

The rx reporter is responsible for reporting and recovering of the following two error scenarios:
(rx 报告器负责报告并恢复以下两种错误场景：)

- rx queues' initialization (population) timeout
  (rx 队列初始化 (填充) 超时)

    Population of rx queues' descriptors on ring initialization is done
    in napi context via triggering an irq. In case of a failure to get
    the minimum amount of descriptors, a timeout would occur, and
    descriptors could be recovered by polling the EQ (Event Queue).
    (环初始化时 rx 队列描述符的填充是在 napi 上下文中通过触发一个中断完成的。
    如果未能获得最少数量的描述符，就会发生超时，此时可通过轮询 EQ (事件队列)
    来恢复描述符。)
- rx completions with errors (reported by HW on interrupt context)
  (带错误的 rx 完成 (由硬件在中断上下文中报告))

    Report on rx completion error.
    (报告 rx 完成错误。)
    Recover (if needed) by flushing the related queue and reset it.
    (如有需要，通过冲刷相关队列并将其复位来恢复。)

rx reporter also supports on demand diagnose callback, on which it
provides real time information of its receive queues' status.
(rx 报告器还支持按需诊断回调，可实时提供其接收队列状态的信息。)

- Diagnose rx queues' status and corresponding completion queue::
  (诊断 rx 队列状态及对应的完成队列：)

    $ devlink health diagnose pci/0000:82:00.0 reporter rx

.. note::
   This command has valid output only when interface is up. Otherwise, the command has empty output.
   (注意：只有接口处于 up 状态时该命令才有有效输出，否则输出为空。)

- Show number of rx errors indicated, number of recover flows ended successfully,
  is autorecover enabled, and graceful period from last recover::
  (显示已指示的 rx 错误数量、成功结束的恢复流程数量、是否启用了自动恢复，以及
  距上次恢复的宽限期：)

    $ devlink health show pci/0000:82:00.0 reporter rx

fw reporter
(fw 报告器)
-----------

The fw reporter implements `diagnose` and `dump` callbacks.
(fw 报告器实现了 `diagnose` 与 `dump` 回调。)
It follows symptoms of fw error such as fw syndrome by triggering
fw core dump and storing it into the dump buffer.
(它通过触发 fw core dump 并将其存入转储缓冲区，来跟踪 fw syndrome 等 fw 错误
症状。)
The fw reporter diagnose command can be triggered any time by the user to check
current fw status.
(用户可随时触发 fw 报告器的 diagnose 命令，以检查当前的 fw 状态。)

User commands examples:
(用户命令示例：)

- Check fw heath status::
  (检查 fw 健康状态：)

    $ devlink health diagnose pci/0000:82:00.0 reporter fw

- Read FW core dump if already stored or trigger new one::
  (读取已存储的 FW core dump (若已存在)，否则触发新的转储：)

    $ devlink health dump show pci/0000:82:00.0 reporter fw

.. note::
   This command can run only on the PF which has fw tracer ownership,
   running it on other PF or any VF will return "Operation not permitted".
   (注意：该命令只能在拥有 fw tracer 所有权的 PF 上运行；在其他 PF 或任何 VF
   上运行将返回 "Operation not permitted"。)

fw fatal reporter
(fw 致命错误报告器)
-----------------

The fw fatal reporter implements `dump` and `recover` callbacks.
(fw fatal 报告器实现了 `dump` 与 `recover` 回调。)
It follows fatal errors indications by CR-space dump and recover flow.
(它通过 CR-space 转储与恢复流程来跟踪致命错误指示。)
The CR-space dump uses vsc interface which is valid even if the FW command
interface is not functional, which is the case in most FW fatal errors.
(CR-space 转储使用 vsc 接口，即使 FW 命令接口失效 (大多数 FW 致命错误时正是
如此) 也仍然可用。)
The recover function runs recover flow which reloads the driver and triggers fw
reset if needed.
(恢复函数运行恢复流程，重新加载驱动，并在需要时触发 fw 复位。)
On firmware error, the health buffer is dumped into the dmesg. The log
level is derived from the error's severity (given in health buffer).
(发生固件错误时，health buffer 会被转储到 dmesg 中。日志级别取决于错误的严重
程度 (记录在 health buffer 中)。)

User commands examples:
(用户命令示例：)

- Run fw recover flow manually::
  (手动运行 fw 恢复流程：)

    $ devlink health recover pci/0000:82:00.0 reporter fw_fatal

- Read FW CR-space dump if already stored or trigger new one::
  (读取已存储的 FW CR-space 转储 (若已存在)，否则触发新的转储：)

    $ devlink health dump show pci/0000:82:00.1 reporter fw_fatal

.. note::
   This command can run only on PF.
   (注意：该命令只能在 PF 上运行。)

vnic reporter
(vnic 报告器)
-------------

The vnic reporter implements only the `diagnose` callback.
(vnic 报告器只实现了 `diagnose` 回调。)
It is responsible for querying the vnic diagnostic counters from fw and displaying
them in realtime.
(它负责从 fw 查询 vnic 诊断计数器并实时显示。)

Description of the vnic counters:
(vnic 计数器说明：)

- total_error_queues
        number of queues in an error state due to
        an async error or errored command.
        (因异步错误或出错命令而处于错误状态的队列数量。)
- send_queue_priority_update_flow
        number of QP/SQ priority/SL update events.
        (QP/SQ 优先级/SL 更新事件的数量。)
- cq_overrun
        number of times CQ entered an error state due to an overflow.
        (CQ 因溢出而进入错误状态的次数。)
- async_eq_overrun
        number of times an EQ mapped to async events was overrun.
        (映射到异步事件的 EQ 发生溢出的次数。)
- comp_eq_overrun
        number of times an EQ mapped to completion events was
        overrun.
        (映射到完成事件的 EQ 发生溢出的次数。)
- quota_exceeded_command
        number of commands issued and failed due to quota exceeded.
        (因超出配额而发出并失败的命令数量。)
- invalid_command
        number of commands issued and failed dues to any reason other than quota
        exceeded.
        (因配额超出以外的原因而发出并失败的命令数量。)
- nic_receive_steering_discard
        number of packets that completed RX flow
        steering but were discarded due to a mismatch in flow table.
        (完成了 RX 流 steering 但因流表中不匹配而被丢弃的数据包数量。)
- generated_pkt_steering_fail
	number of packets generated by the VNIC experiencing unexpected steering
	failure (at any point in steering flow).
        (由 VNIC 生成、遭遇意外 steering 失败 (在 steering 流程的任一环节) 的
        数据包数量。)
- handled_pkt_steering_fail
	number of packets handled by the VNIC experiencing unexpected steering
	failure (at any point in steering flow owned by the VNIC, including the FDB
	for the eswitch owner).
        (由 VNIC 处理、遭遇意外 steering 失败 (在 VNIC 拥有的 steering 流程的
        任一环节，包括 eswitch 所有者的 FDB) 的数据包数量。)
- icm_consumption
        amount of Interconnect Host Memory (ICM) consumed by the vnic in
        granularity of 4KB. ICM is host memory allocated by SW upon HCA request
        and is used for storing data structures that control HCA operation.
        (vnic 以 4KB 为粒度消耗的互连主机内存 (ICM) 数量。ICM 是 SW 应 HCA 请求
        分配的主机内存，用于存储控制 HCA 操作的数据结构。)
- bar_uar_access
        number of WRITE or READ access operations to the UAR on the PCIe BAR.
        (对 PCIe BAR 上的 UAR 进行 WRITE 或 READ 访问操作的次数。)
- odp_local_triggered_page_fault
        number of locally-triggered page-faults due to ODP.
        (由 ODP 引发的本地触发缺页次数。)
- odp_remote_triggered_page_fault
        number of remotly-triggered page-faults due to ODP.
        (由 ODP 引发的远程触发缺页次数。)

User commands examples:
(用户命令示例：)

- Diagnose PF/VF vnic counters::
  (诊断 PF/VF 的 vnic 计数器：)

        $ devlink health diagnose pci/0000:82:00.1 reporter vnic

- Diagnose representor vnic counters (performed by supplying devlink port of the
  representor, which can be obtained via devlink port command)::
  (诊断代表设备的 vnic 计数器 (需提供该代表设备的 devlink 端口，可通过
  devlink port 命令获取)：)

        $ devlink health diagnose pci/0000:82:00.1/65537 reporter vnic

.. note::
   This command can run over all interfaces such as PF/VF and representor ports.
   (注意：该命令可在所有接口上运行，例如 PF/VF 与代表设备端口。)
