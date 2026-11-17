.. SPDX-License-Identifier: GPL-2.0

===================
ice devlink support
===================

(ice 驱动的 devlink 支持)

This document describes the devlink features implemented by the ``ice``
device driver.
(本文档描述 ``ice`` 设备驱动所实现的 devlink 功能。)

Parameters
(参数)
==========

.. list-table:: Generic parameters implemented
   :widths: 5 5 90

   * - Name (名称)
     - Mode (模式)
     - Notes (说明)
   * - ``enable_roce``
     - runtime
     - mutually exclusive with ``enable_iwarp``
       (与 ``enable_iwarp`` 互斥)
   * - ``enable_iwarp``
     - runtime
     - mutually exclusive with ``enable_roce``
       (与 ``enable_roce`` 互斥)
   * - ``tx_scheduling_layers``
     - permanent
     - The ice hardware uses hierarchical scheduling for Tx with a fixed
       number of layers in the scheduling tree. Each of them are decision
       points. Root node represents a port, while all the leaves represent
       the queues. This way of configuring the Tx scheduler allows features
       like DCB or devlink-rate (documented below) to configure how much
       bandwidth is given to any given queue or group of queues, enabling
       fine-grained control because scheduling parameters can be configured
       at any given layer of the tree.
       (ice 硬件对发送（Tx）采用分层调度，调度树中的层数是固定的。每一层都是一个
       决策点。根节点代表一个端口，而所有叶子节点代表各个队列。这种配置 Tx 调度器的
       方式使得 DCB 或 devlink-rate（见下文说明）等功能可以为任意队列或队列组配置
       可获得的带宽，从而实现细粒度的控制，因为调度参数可以在树的任意一层进行配置。)

       The default 9-layer tree topology was deemed best for most workloads,
       as it gives an optimal ratio of performance to configurability. However,
       for some specific cases, this 9-layer topology might not be desired.
       One example would be sending traffic to queues that are not a multiple
       of 8. Because the maximum radix is limited to 8 in 9-layer topology,
       the 9th queue has a different parent than the rest, and it's given
       more bandwidth credits. This causes a problem when the system is
       sending traffic to 9 queues:
       (默认的 9 层树拓扑结构被认为对大多数工作负载最为合适，因为它在性能与可配置性
       之间达到了最佳平衡。但在某些特定情况下，这种 9 层拓扑可能并不理想。一个例子是
       向数量不是 8 的倍数的队列发送流量。由于 9 层拓扑的最大扇出被限制为 8，第 9 个
       队列的父节点与其他队列不同，因而被分配了更多的带宽份额。当系统向 9 个队列发送
       流量时就会产生如下问题：)

       | tx_queue_0_packets: 24163396
       | tx_queue_1_packets: 24164623
       | tx_queue_2_packets: 24163188
       | tx_queue_3_packets: 24163701
       | tx_queue_4_packets: 24163683
       | tx_queue_5_packets: 24164668
       | tx_queue_6_packets: 23327200
       | tx_queue_7_packets: 24163853
       | tx_queue_8_packets: 91101417 < Too much traffic is sent from 9th
       | （第 9 个队列发送了过多流量）

       To address this need, you can switch to a 5-layer topology, which
       changes the maximum topology radix to 512. With this enhancement,
       the performance characteristic is equal as all queues can be assigned
       to the same parent in the tree. The obvious drawback of this solution
       is a lower configuration depth of the tree.
       (为满足这一需求，可以切换到 5 层拓扑，它将最大拓扑扇出提高到 512。经过这一
       改进，各队列的性能表现将完全一致，因为所有队列都可以挂在树中同一个父节点之下。
       这种方案显而易见的缺点是树的配置深度变浅了。)

       Use the ``tx_scheduling_layer`` parameter with the devlink command
       to change the transmit scheduler topology. To use 5-layer topology,
       use a value of 5. For example:
       (使用 ``tx_scheduling_layer`` 参数和 devlink 命令来更改发送调度器拓扑。
       要使用 5 层拓扑，将值设为 5。例如：)
       $ devlink dev param set pci/0000:16:00.0 name tx_scheduling_layers
       value 5 cmode permanent
       Use a value of 9 to set it back to the default value.
       (将值设为 9 即可恢复为默认值。)

       You must do PCI slot powercycle for the selected topology to take effect.
       (必须对 PCI 插槽进行断电重启（powercycle），所选拓扑才能生效。)

       To verify that value has been set:
       (要验证该值是否已设置：)
       $ devlink dev param show pci/0000:16:00.0 name tx_scheduling_layers
   * - ``msix_vec_per_pf_max``
     - driverinit
     - Set the max MSI-X that can be used by the PF, rest can be utilized for
       SRIOV. The range is from min value set in msix_vec_per_pf_min to
       2k/number of ports.
       (设置 PF 可使用的最大 MSI-X 数量，其余的可供 SRIOV 使用。取值范围是从
       msix_vec_per_pf_min 设置的最小值到 2k/端口数。)
   * - ``msix_vec_per_pf_min``
     - driverinit
     - Set the min MSI-X that will be used by the PF. This value inform how many
       MSI-X will be allocated statically. The range is from 2 to value set
       in msix_vec_per_pf_max.
       (设置 PF 将使用的最小 MSI-X 数量。该值指示将静态分配多少个 MSI-X。取值
       范围是从 2 到 msix_vec_per_pf_max 中设置的值。)

.. list-table:: Driver specific parameters implemented
    :widths: 5 5 90

    * - Name (名称)
      - Mode (模式)
      - Description (描述)
    * - ``local_forwarding``
      - runtime
      - Controls loopback behavior by tuning scheduler bandwidth.
        It impacts all kinds of functions: physical, virtual and
        subfunctions.
        (通过调整调度器带宽来控制环回行为。它影响所有类型的
        功能：物理功能、虚拟功能和子功能。)
        Supported values are:
        (支持的取值有：)

        ``enabled`` - loopback traffic is allowed on port
        (``enabled`` - 允许端口上的环回流量)

        ``disabled`` - loopback traffic is not allowed on this port
        (``disabled`` - 不允许此端口上的环回流量)

        ``prioritized`` - loopback traffic is prioritized on this port
        (``prioritized`` - 此端口上的环回流量被优先处理)

        Default value of ``local_forwarding`` parameter is ``enabled``.
        ``prioritized`` provides ability to adjust loopback traffic rate to increase
        one port capacity at cost of the another. User needs to disable
        local forwarding on one of the ports in order have increased capacity
        on the ``prioritized`` port.
        (``local_forwarding`` 参数的默认值为 ``enabled`` 。 ``prioritized`` 允许
        调整环回流量速率，以牺牲一个端口的容量为代价来提升另一个端口的容量。用户需要
        在其中一个端口上禁用本地转发，才能使 ``prioritized`` 端口获得更高的容量。)

Info versions
(信息版本)
=============

The ``ice`` driver reports the following versions
(``ice`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
    :widths: 5 5 5 90

    * - Name (名称)
      - Type (类型)
      - Example (示例)
      - Description (描述)
    * - ``board.id``
      - fixed
      - K65390-000
      - The Product Board Assembly (PBA) identifier of the board.
        (电路板的产品板卡组件 (PBA) 标识符。)
    * - ``cgu.id``
      - fixed
      - 36
      - The Clock Generation Unit (CGU) hardware revision identifier.
        (时钟生成单元 (CGU) 的硬件修订版本标识符。)
    * - ``fw.mgmt``
      - running
      - 2.1.7
      - 3-digit version number of the management firmware running on the
        Embedded Management Processor of the device. It controls the PHY,
        link, access to device resources, etc. Intel documentation refers to
        this as the EMP firmware.
        (设备嵌入式管理处理器上运行的管理固件的 3 位版本号。它控制 PHY、
        链路、对设备资源的访问等。Intel 文档将其称为 EMP 固件。)
    * - ``fw.mgmt.api``
      - running
      - 1.5.1
      - 3-digit version number (major.minor.patch) of the API exported over
        the AdminQ by the management firmware. Used by the driver to
        identify what commands are supported. Historical versions of the
        kernel only displayed a 2-digit version number (major.minor).
        (管理固件通过 AdminQ 导出的 API 的 3 位版本号
        (major.minor.patch)。驱动程序用它来判断支持哪些命令。历史版本的内核
        只显示 2 位版本号 (major.minor)。)
    * - ``fw.mgmt.build``
      - running
      - 0x305d955f
      - Unique identifier of the source for the management firmware.
        (管理固件源码的唯一标识符。)
    * - ``fw.undi``
      - running
      - 1.2581.0
      - Version of the Option ROM containing the UEFI driver. The version is
        reported in ``major.minor.patch`` format. The major version is
        incremented whenever a major breaking change occurs, or when the
        minor version would overflow. The minor version is incremented for
        non-breaking changes and reset to 1 when the major version is
        incremented. The patch version is normally 0 but is incremented when
        a fix is delivered as a patch against an older base Option ROM.
        (包含 UEFI 驱动的 Option ROM 的版本。版本以 ``major.minor.patch`` 格式
        报告。每当发生重大破坏性变更，或者次版本将要溢出时，主版本号就会递增。
        次版本号在非破坏性变更时递增，并在主版本号递增时重置为 1。补丁版本号通常
        为 0，但当修复以针对较旧基础 Option ROM 的补丁形式交付时，补丁版本号会
        递增。)
    * - ``fw.psid.api``
      - running
      - 0.80
      - Version defining the format of the flash contents.
        (定义闪存内容格式的版本。)
    * - ``fw.bundle_id``
      - running
      - 0x80002ec0
      - Unique identifier of the firmware image file that was loaded onto
        the device. Also referred to as the EETRACK identifier of the NVM.
        (加载到设备上的固件镜像文件的唯一标识符。也称为 NVM 的
        EETRACK 标识符。)
    * - ``fw.app.name``
      - running
      - ICE OS Default Package
      - The name of the DDP package that is active in the device. The DDP
        package is loaded by the driver during initialization. Each
        variation of the DDP package has a unique name.
        (设备中当前生效的 DDP 包的名称。DDP 包由驱动程序在初始化期间加载。
        每个 DDP 包变体都有一个唯一的名称。)
    * - ``fw.app``
      - running
      - 1.3.1.0
      - The version of the DDP package that is active in the device. Note
        that both the name (as reported by ``fw.app.name``) and version are
        required to uniquely identify the package.
        (设备中当前生效的 DDP 包的版本。注意，要唯一地标识该包，需要同时提供
        名称 (由 ``fw.app.name`` 报告) 和版本号。)
    * - ``fw.app.bundle_id``
      - running
      - 0xc0000001
      - Unique identifier for the DDP package loaded in the device. Also
        referred to as the DDP Track ID. Can be used to uniquely identify
        the specific DDP package.
        (设备中加载的 DDP 包的唯一标识符。也称为 DDP Track ID。可用于唯一地
        标识特定的 DDP 包。)
    * - ``fw.netlist``
      - running
      - 1.1.2000-6.7.0
      - The version of the netlist module. This module defines the device's
        Ethernet capabilities and default settings, and is used by the
        management firmware as part of managing link and device
        connectivity.
        (netlist 模块的版本。该模块定义设备的以太网能力和默认设置，管理固件
        在管理链路与设备连接时会使用它。)
    * - ``fw.netlist.build``
      - running
      - 0xee16ced7
      - The first 4 bytes of the hash of the netlist module contents.
        (netlist 模块内容哈希值的前 4 个字节。)
    * - ``fw.cgu``
      - running
      - 8032.16973825.6021
      - The version of Clock Generation Unit (CGU). Format:
        <CGU type>.<configuration version>.<firmware version>.
        (时钟生成单元 (CGU) 的版本。格式：
        <CGU type>.<configuration version>.<firmware version>。)

Flash Update
(固件更新)
============

The ``ice`` driver implements support for flash update using the
``devlink-flash`` interface. It supports updating the device flash using a
combined flash image that contains the ``fw.mgmt``, ``fw.undi``, and
``fw.netlist`` components.
(``ice`` 驱动通过 ``devlink-flash`` 接口实现对固件更新的支持。它支持使用
包含 ``fw.mgmt`` 、 ``fw.undi`` 和 ``fw.netlist`` 组件的合并固件镜像来更新
设备闪存。)

.. list-table:: List of supported overwrite modes
   :widths: 5 95

   * - Bits (位)
     - Behavior (行为)
   * - ``DEVLINK_FLASH_OVERWRITE_SETTINGS``
     - Do not preserve settings stored in the flash components being
       updated. This includes overwriting the port configuration that
       determines the number of physical functions the device will
       initialize with.
       (不保留正在更新的闪存组件中存储的设置。这包括覆盖用于决定设备将以多少个
       物理功能进行初始化的端口配置。)
   * - ``DEVLINK_FLASH_OVERWRITE_SETTINGS`` and ``DEVLINK_FLASH_OVERWRITE_IDENTIFIERS``
     - Do not preserve either settings or identifiers. Overwrite everything
       in the flash with the contents from the provided image, without
       performing any preservation. This includes overwriting device
       identifying fields such as the MAC address, VPD area, and device
       serial number. It is expected that this combination be used with an
       image customized for the specific device.
       (既不保留设置，也不保留标识符。使用所提供镜像的内容覆盖闪存中的所有内容，
       不做任何保留。这包括覆盖设备的标识字段，如 MAC 地址、VPD 区域和设备
       序列号。预期这一组合应与针对特定设备定制的镜像一起使用。)

The ice hardware does not support overwriting only identifiers while
preserving settings, and thus ``DEVLINK_FLASH_OVERWRITE_IDENTIFIERS`` on its
own will be rejected. If no overwrite mask is provided, the firmware will be
instructed to preserve all settings and identifying fields when updating.
(ice 硬件不支持在保留设置的同时仅覆盖标识符，因此单独使用
``DEVLINK_FLASH_OVERWRITE_IDENTIFIERS`` 将被拒绝。如果未提供覆盖掩码，更新时
将指示固件保留所有设置和标识字段。)

Reload
(重新加载)
======

The ``ice`` driver supports activating new firmware after a flash update
using ``DEVLINK_CMD_RELOAD`` with the ``DEVLINK_RELOAD_ACTION_FW_ACTIVATE``
action.
(``ice`` 驱动支持在固件更新后，使用带有 ``DEVLINK_RELOAD_ACTION_FW_ACTIVATE``
动作的 ``DEVLINK_CMD_RELOAD`` 来激活新固件。)

.. code:: shell

    $ devlink dev reload pci/0000:01:00.0 reload action fw_activate

The new firmware is activated by issuing a device specific Embedded
Management Processor reset which requests the device to reset and reload the
EMP firmware image.
(新固件的激活方式是发出设备特定的嵌入式管理处理器复位请求，让设备复位并重新
加载 EMP 固件镜像。)

The driver does not currently support reloading the driver via
``DEVLINK_RELOAD_ACTION_DRIVER_REINIT``.
(驱动程序目前不支持通过 ``DEVLINK_RELOAD_ACTION_DRIVER_REINIT`` 重新加载
驱动。)

Port split
(端口拆分)
==========

The ``ice`` driver supports port splitting only for port 0, as the FW has
a predefined set of available port split options for the whole device.
(``ice`` 驱动仅支持对端口 0 进行端口拆分，因为固件为整个设备预定义了一组
可用的端口拆分选项。)

A system reboot is required for port split to be applied.
(端口拆分需要重启系统才能生效。)

The following command will select the port split option with 4 ports:
(以下命令将选择具有 4 个端口的端口拆分选项：)

.. code:: shell

    $ devlink port split pci/0000:16:00.0/0 count 4

The list of all available port options will be printed to dynamic debug after
each ``split`` and ``unsplit`` command. The first option is the default.
(每次执行 ``split`` 和 ``unsplit`` 命令后，所有可用端口选项的列表都会打印到
动态调试中。第一个选项是默认选项。)

.. code:: shell

    ice 0000:16:00.0: Available port split options and max port speeds (Gbps):
    ice 0000:16:00.0: Status  Split      Quad 0          Quad 1
    ice 0000:16:00.0:         count  L0  L1  L2  L3  L4  L5  L6  L7
    ice 0000:16:00.0: Active  2     100   -   -   - 100   -   -   -
    ice 0000:16:00.0:         2      50   -  50   -   -   -   -   -
    ice 0000:16:00.0: Pending 4      25  25  25  25   -   -   -   -
    ice 0000:16:00.0:         4      25  25   -   -  25  25   -   -
    ice 0000:16:00.0:         8      10  10  10  10  10  10  10  10
    ice 0000:16:00.0:         1     100   -   -   -   -   -   -   -

There could be multiple FW port options with the same port split count. When
the same port split count request is issued again, the next FW port option with
the same port split count will be selected.
(可能存在多个具有相同端口拆分数量的固件端口选项。当再次发出相同的端口拆分数量
请求时，将选择具有相同端口拆分数量的下一个固件端口选项。)

``devlink port unsplit`` will select the option with a split count of 1. If
there is no FW option available with split count 1, you will receive an error.
(``devlink port unsplit`` 将选择拆分数量为 1 的选项。如果没有拆分数量为 1 的
固件选项可用，将会收到一个错误。)

Regions
(区域)
=======

The ``ice`` driver implements the following regions for accessing internal
device data.
(``ice`` 驱动实现了以下区域，用于访问设备内部数据。)

.. list-table:: regions implemented
    :widths: 15 85

    * - Name (名称)
      - Description (描述)
    * - ``nvm-flash``
      - The contents of the entire flash chip, sometimes referred to as
        the device's Non Volatile Memory.
        (整个闪存芯片的内容，有时也称为设备的非易失性存储器 (NVM)。)
    * - ``shadow-ram``
      - The contents of the Shadow RAM, which is loaded from the beginning
        of the flash. Although the contents are primarily from the flash,
        this area also contains data generated during device boot which is
        not stored in flash.
        (影子 RAM (Shadow RAM) 的内容，它从闪存的起始处加载。虽然其内容主要
        来自闪存，但该区域还包含设备启动期间生成的、未存储在闪存中的数据。)
    * - ``device-caps``
      - The contents of the device firmware's capabilities buffer. Useful to
        determine the current state and configuration of the device.
        (设备固件能力缓冲区的内容。可用于确定设备的当前状态和配置。)

Both the ``nvm-flash`` and ``shadow-ram`` regions can be accessed without a
snapshot. The ``device-caps`` region requires a snapshot as the contents are
sent by firmware and can't be split into separate reads.
(``nvm-flash`` 和 ``shadow-ram`` 区域都可以在不带快照的情况下访问。
``device-caps`` 区域则需要快照，因为其内容由固件发送，无法拆分为多次读取。)

Users can request an immediate capture of a snapshot for all three regions
via the ``DEVLINK_CMD_REGION_NEW`` command.
(用户可以通过 ``DEVLINK_CMD_REGION_NEW`` 命令为所有这三个区域请求立即捕获
快照。)

.. code:: shell

    $ devlink region show
    pci/0000:01:00.0/nvm-flash: size 10485760 snapshot [] max 1
    pci/0000:01:00.0/device-caps: size 4096 snapshot [] max 10

    $ devlink region new pci/0000:01:00.0/nvm-flash snapshot 1
    $ devlink region dump pci/0000:01:00.0/nvm-flash snapshot 1

    $ devlink region dump pci/0000:01:00.0/nvm-flash snapshot 1
    0000000000000000 0014 95dc 0014 9514 0035 1670 0034 db30
    0000000000000010 0000 0000 ffff ff04 0029 8c00 0028 8cc8
    0000000000000020 0016 0bb8 0016 1720 0000 0000 c00f 3ffc
    0000000000000030 bada cce5 bada cce5 bada cce5 bada cce5

    $ devlink region read pci/0000:01:00.0/nvm-flash snapshot 1 address 0 length 16
    0000000000000000 0014 95dc 0014 9514 0035 1670 0034 db30

    $ devlink region delete pci/0000:01:00.0/nvm-flash snapshot 1

    $ devlink region new pci/0000:01:00.0/device-caps snapshot 1
    $ devlink region dump pci/0000:01:00.0/device-caps snapshot 1
    0000000000000000 01 00 01 00 00 00 00 00 01 00 00 00 00 00 00 00
    0000000000000010 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000020 02 00 02 01 32 03 00 00 0a 00 00 00 25 00 00 00
    0000000000000030 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000040 04 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00
    0000000000000050 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000060 05 00 01 00 03 00 00 00 00 00 00 00 00 00 00 00
    0000000000000070 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000080 06 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00
    0000000000000090 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000000a0 08 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000000b0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000000c0 12 00 01 00 01 00 00 00 01 00 01 00 00 00 00 00
    00000000000000d0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000000e0 13 00 01 00 00 01 00 00 00 00 00 00 00 00 00 00
    00000000000000f0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000100 14 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00
    0000000000000110 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000120 15 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00
    0000000000000130 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000140 16 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00
    0000000000000150 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000160 17 00 01 00 06 00 00 00 00 00 00 00 00 00 00 00
    0000000000000170 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000180 18 00 01 00 01 00 00 00 01 00 00 00 08 00 00 00
    0000000000000190 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000001a0 22 00 01 00 01 00 00 00 00 00 00 00 00 00 00 00
    00000000000001b0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000001c0 40 00 01 00 00 08 00 00 08 00 00 00 00 00 00 00
    00000000000001d0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000000000001e0 41 00 01 00 00 08 00 00 00 00 00 00 00 00 00 00
    00000000000001f0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    0000000000000200 42 00 01 00 00 08 00 00 00 00 00 00 00 00 00 00
    0000000000000210 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

    $ devlink region delete pci/0000:01:00.0/device-caps snapshot 1

Devlink Rate
(Devlink 速率)
============

The ``ice`` driver implements devlink-rate API. It allows for offload of
the Hierarchical QoS to the hardware. It enables user to group Virtual
Functions in a tree structure and assign supported parameters: tx_share,
tx_max, tx_priority and tx_weight to each node in a tree. So effectively
user gains an ability to control how much bandwidth is allocated for each
VF group. This is later enforced by the HW.
(``ice`` 驱动实现了 devlink-rate API。它允许将分层 QoS 卸载到硬件。它使用户
能够以树形结构对虚拟功能进行分组，并为树中的每个节点分配受支持的参数：
tx_share、tx_max、tx_priority 和 tx_weight。这样，用户实际上就获得了控制为
每个 VF 组分配多少带宽的能力。之后由硬件强制执行这些设置。)

It is assumed that this feature is mutually exclusive with DCB performed
in FW and ADQ, or any driver feature that would trigger changes in QoS,
for example creation of the new traffic class. The driver will prevent DCB
or ADQ configuration if user started making any changes to the nodes using
devlink-rate API. To configure those features a driver reload is necessary.
Correspondingly if ADQ or DCB will get configured the driver won't export
hierarchy at all, or will remove the untouched hierarchy if those
features are enabled after the hierarchy is exported, but before any
changes are made.
(假定该功能与固件中执行的 DCB、ADQ，或任何会触发 QoS 变更的驱动特性（例如
创建新流量类别）互斥。如果用户已开始使用 devlink-rate API 对节点做任何更改，
驱动将阻止 DCB 或 ADQ 的配置。要配置这些功能，必须重新加载驱动。相应地，如果
配置了 ADQ 或 DCB，驱动将完全不导出层级结构；或者，如果这些功能在层级结构
导出之后、但尚未做任何更改之前被启用，驱动会移除未被改动过的层级结构。)

This feature is also dependent on switchdev being enabled in the system.
It's required because devlink-rate requires devlink-port objects to be
present, and those objects are only created in switchdev mode.
(该功能还依赖于系统中启用了 switchdev。这是必需的，因为 devlink-rate 需要
存在 devlink-port 对象，而这些对象只在 switchdev 模式下才会创建。)

If the driver is set to the switchdev mode, it will export internal
hierarchy the moment VF's are created. Root of the tree is always
represented by the node_0. This node can't be deleted by the user. Leaf
nodes and nodes with children also can't be deleted.
(如果驱动被设置为 switchdev 模式，那么在创建 VF 的那一刻，它就会导出内部
层级结构。树的根节点始终由 node_0 表示。该节点不能被用户删除。叶子节点和
拥有子节点的节点也不能被删除。)

.. list-table:: Attributes supported
    :widths: 15 85

    * - Name (名称)
      - Description (描述)
    * - ``tx_max``
      - maximum bandwidth to be consumed by the tree Node. Rate Limit is
        an absolute number specifying a maximum amount of bytes a Node may
        consume during the course of one second. Rate limit guarantees
        that a link will not oversaturate the receiver on the remote end
        and also enforces an SLA between the subscriber and network
        provider.
        (树节点可消耗的最大带宽。速率限制（Rate Limit）是一个绝对数值，
        指定节点在一秒内最多可消耗的字节数。速率限制保证链路不会使远端的
        接收端过饱和，同时也在用户与网络提供商之间强制执行 SLA。)
    * - ``tx_share``
      - minimum bandwidth allocated to a tree node when it is not blocked.
        It specifies an absolute BW. While tx_max defines the maximum
        bandwidth the node may consume, the tx_share marks committed BW
        for the Node.
        (树节点在未被阻塞时分配到的最小带宽。它指定一个绝对带宽值。tx_max
        定义节点可消耗的最大带宽，而 tx_share 则标记节点的承诺带宽。)
    * - ``tx_priority``
      - allows for usage of strict priority arbiter among siblings. This
        arbitration scheme attempts to schedule nodes based on their
        priority as long as the nodes remain within their bandwidth limit.
        Range 0-7. Nodes with priority 7 have the highest priority and are
        selected first, while nodes with priority 0 have the lowest
        priority. Nodes that have the same priority are treated equally.
        (允许在兄弟节点之间使用严格优先级仲裁器。只要节点保持在各自的带宽
        限制之内，这种仲裁方案就会根据节点的优先级进行调度。取值范围
        0-7。优先级为 7 的节点拥有最高优先级，会被优先选择；优先级为 0 的
        节点优先级最低。具有相同优先级的节点被同等对待。)
    * - ``tx_weight``
      - allows for usage of Weighted Fair Queuing arbitration scheme among
        siblings. This arbitration scheme can be used simultaneously with
        the strict priority. Range 1-200. Only relative values matter for
        arbitration.
        (允许在兄弟节点之间使用加权公平排队（WFQ）仲裁方案。这种仲裁方案
        可以与严格优先级同时使用。取值范围 1-200。对仲裁而言，只有相对值
        才有意义。)

``tx_priority`` and ``tx_weight`` can be used simultaneously. In that case
nodes with the same priority form a WFQ subgroup in the sibling group
and arbitration among them is based on assigned weights.
(``tx_priority`` 和 ``tx_weight`` 可以同时使用。在这种情况下，具有相同
优先级的节点会在兄弟节点组内构成一个 WFQ 子组，它们之间的仲裁基于所分配的
权重。)

.. code:: shell

    # enable switchdev
    $ devlink dev eswitch set pci/0000:4b:00.0 mode switchdev

    # at this point driver should export internal hierarchy
    $ echo 2 > /sys/class/net/ens785np0/device/sriov_numvfs

    $ devlink port function rate show
    pci/0000:4b:00.0/node_25: type node parent node_24
    pci/0000:4b:00.0/node_24: type node parent node_0
    pci/0000:4b:00.0/node_32: type node parent node_31
    pci/0000:4b:00.0/node_31: type node parent node_30
    pci/0000:4b:00.0/node_30: type node parent node_16
    pci/0000:4b:00.0/node_19: type node parent node_18
    pci/0000:4b:00.0/node_18: type node parent node_17
    pci/0000:4b:00.0/node_17: type node parent node_16
    pci/0000:4b:00.0/node_14: type node parent node_5
    pci/0000:4b:00.0/node_5: type node parent node_3
    pci/0000:4b:00.0/node_13: type node parent node_4
    pci/0000:4b:00.0/node_12: type node parent node_4
    pci/0000:4b:00.0/node_11: type node parent node_4
    pci/0000:4b:00.0/node_10: type node parent node_4
    pci/0000:4b:00.0/node_9: type node parent node_4
    pci/0000:4b:00.0/node_8: type node parent node_4
    pci/0000:4b:00.0/node_7: type node parent node_4
    pci/0000:4b:00.0/node_6: type node parent node_4
    pci/0000:4b:00.0/node_4: type node parent node_3
    pci/0000:4b:00.0/node_3: type node parent node_16
    pci/0000:4b:00.0/node_16: type node parent node_15
    pci/0000:4b:00.0/node_15: type node parent node_0
    pci/0000:4b:00.0/node_2: type node parent node_1
    pci/0000:4b:00.0/node_1: type node parent node_0
    pci/0000:4b:00.0/node_0: type node
    pci/0000:4b:00.0/1: type leaf parent node_25
    pci/0000:4b:00.0/2: type leaf parent node_25

    # let's create some custom node
    $ devlink port function rate add pci/0000:4b:00.0/node_custom parent node_0

    # second custom node
    $ devlink port function rate add pci/0000:4b:00.0/node_custom_1 parent node_custom

    # reassign second VF to newly created branch
    $ devlink port function rate set pci/0000:4b:00.0/2 parent node_custom_1

    # assign tx_weight to the VF
    $ devlink port function rate set pci/0000:4b:00.0/2 tx_weight 5

    # assign tx_share to the VF
    $ devlink port function rate set pci/0000:4b:00.0/2 tx_share 500Mbps
