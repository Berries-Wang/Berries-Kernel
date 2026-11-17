.. SPDX-License-Identifier: GPL-2.0

=========================================
ixgbe devlink support(ixgbe devlink 支持)
=========================================

This document describes the devlink features implemented by the ``ixgbe``
device driver.
(本文描述 ``ixgbe`` 设备驱动实现的 devlink 特性。)

Info versions(信息版本)
=======================

Any of the versions dealing with the security presented by ``devlink-info``
is purely informational. Devlink does not use a secure channel to communicate
with the device.
(任何涉及 ``devlink-info`` 所呈现安全性的版本都纯粹是信息性的。devlink 不使用安全通道与设备通信。)

The ``ixgbe`` driver reports the following versions
(``ixgbe`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
    :widths: 5 5 5 90

    * - Name (名称)
      - Type (类型)
      - Example (示例)
      - Description (描述)
    * - ``board.id``
      - fixed
      - H49289-000
      - The Product Board Assembly (PBA) identifier of the board.
        (板的产品板组件 (PBA) 标识符。)
    * - ``fw.undi``
      - running
      - 1.1937.0
      - Version of the Option ROM containing the UEFI driver. The version is
        reported in ``major.minor.patch`` format. The major version is
        incremented whenever a major breaking change occurs, or when the
        minor version would overflow. The minor version is incremented for
        non-breaking changes and reset to 1 when the major version is
        incremented. The patch version is normally 0 but is incremented when
        a fix is delivered as a patch against an older base Option ROM.
        (包含 UEFI 驱动的 Option ROM 的版本。版本以 ``major.minor.patch`` 格式报告。每当发生重大破坏性变更、或次版本将要溢出时，主版本递增。次版本针对非破坏性变更递增，并在主版本递增时重置为 1。补丁版本通常为 0，但当修复以针对较旧基础 Option ROM 的补丁形式交付时递增。)
    * - ``fw.undi.srev``
      - running
      - 4
      - Number indicating the security revision of the Option ROM.
        (指示 Option ROM 安全修订号的数字。)
    * - ``fw.bundle_id``
      - running
      - 0x80000d0d
      - Unique identifier of the firmware image file that was loaded onto
        the device. Also referred to as the EETRACK identifier of the NVM.
        (加载到设备上的固件镜像文件的唯一标识符。也称为 NVM 的 EETRACK 标识符。)
    * - ``fw.mgmt.api``
      - running
      - 1.5.1
      - 3-digit version number (major.minor.patch) of the API exported over
        the AdminQ by the management firmware. Used by the driver to
        identify what commands are supported. Historical versions of the
        kernel only displayed a 2-digit version number (major.minor).
        (管理固件通过 AdminQ 导出的 API 的 3 位版本号 (major.minor.patch)。驱动用它来识别支持哪些命令。历史版本的内核只显示 2 位版本号 (major.minor)。)
    * - ``fw.mgmt.build``
      - running
      - 0x305d955f
      - Unique identifier of the source for the management firmware.
        (管理固件源码的唯一标识符。)
    * - ``fw.mgmt.srev``
      - running
      - 3
      - Number indicating the security revision of the firmware.
        (指示固件安全修订号的数字。)
    * - ``fw.psid.api``
      - running
      - 0.80
      - Version defining the format of the flash contents.
        (定义闪存内容格式的版本。)
    * - ``fw.netlist``
      - running
      - 1.1.2000-6.7.0
      - The version of the netlist module. This module defines the device's
        Ethernet capabilities and default settings, and is used by the
        management firmware as part of managing link and device
        connectivity.
        (netlist 模块的版本。该模块定义设备的以太网能力和默认设置，管理固件将其作为管理链路和设备连接的一部分使用。)
    * - ``fw.netlist.build``
      - running
      - 0xee16ced7
      - The first 4 bytes of the hash of the netlist module contents.
        (netlist 模块内容哈希的前 4 个字节。)

Flash Update(闪存更新)
======================

The ``ixgbe`` driver implements support for flash update using the
``devlink-flash`` interface. It supports updating the device flash using a
combined flash image that contains the ``fw.mgmt``, ``fw.undi``, and
``fw.netlist`` components.
(``ixgbe`` 驱动通过 ``devlink-flash`` 接口实现闪存更新支持。它支持使用包含 ``fw.mgmt``、``fw.undi`` 和 ``fw.netlist`` 组件的组合闪存镜像来更新设备闪存。)

.. list-table:: List of supported overwrite modes
   :widths: 5 95

   * - Bits (位)
     - Behavior (行为)
   * - ``DEVLINK_FLASH_OVERWRITE_SETTINGS``
     - Do not preserve settings stored in the flash components being
       updated. This includes overwriting the port configuration that
       determines the number of physical functions the device will
       initialize with.
       (不保留被更新的闪存组件中存储的设置。这包括覆盖决定设备将以多少个物理功能 (physical function) 初始化的端口配置。)
   * - ``DEVLINK_FLASH_OVERWRITE_SETTINGS`` and ``DEVLINK_FLASH_OVERWRITE_IDENTIFIERS``
     - Do not preserve either settings or identifiers. Overwrite everything
       in the flash with the contents from the provided image, without
       performing any preservation. This includes overwriting device
       identifying fields such as the MAC address, Vital product Data (VPD) area,
       and device serial number. It is expected that this combination be used with an
       image customized for the specific device.
       (既不保留设置也不保留标识符。用提供的镜像内容覆盖闪存中的所有内容，不做任何保留。这包括覆盖设备标识字段，如 MAC 地址、重要产品数据 (VPD) 区和设备序列号。预期此组合应与针对特定设备定制的镜像一起使用。)

Reload(重载)
============

The ``ixgbe`` driver supports activating new firmware after a flash update
using ``DEVLINK_CMD_RELOAD`` with the ``DEVLINK_RELOAD_ACTION_FW_ACTIVATE``
action.
(``ixgbe`` 驱动支持在闪存更新后，通过 ``DEVLINK_CMD_RELOAD`` 配合 ``DEVLINK_RELOAD_ACTION_FW_ACTIVATE`` 动作来激活新固件。)

.. code:: shell

    $ devlink dev reload pci/0000:01:00.0 reload action fw_activate

The new firmware is activated by issuing a device specific Embedded
Management Processor reset which requests the device to reset and reload the
EMP firmware image.
(通过发出设备特定的嵌入式管理处理器 (Embedded Management Processor) 复位来激活新固件，该复位请求设备复位并重新加载 EMP 固件镜像。)

The driver does not currently support reloading the driver via
``DEVLINK_RELOAD_ACTION_DRIVER_REINIT``.
(驱动目前不支持通过 ``DEVLINK_RELOAD_ACTION_DRIVER_REINIT`` 重新加载驱动自身。)

Regions(区域)
=============

The ``ixgbe`` driver implements the following regions for accessing internal
device data.
(``ixgbe`` 驱动实现了以下区域 (region) 用于访问设备内部数据。)

.. list-table:: regions implemented
    :widths: 15 85

    * - Name (名称)
      - Description (描述)
    * - ``nvm-flash``
      - The contents of the entire flash chip, sometimes referred to as
        the device's Non Volatile Memory.
        (整个闪存芯片的内容，有时称为设备的非易失性存储器。)
    * - ``shadow-ram``
      - The contents of the Shadow RAM, which is loaded from the beginning
        of the flash. Although the contents are primarily from the flash,
        this area also contains data generated during device boot which is
        not stored in flash.
        (Shadow RAM 的内容，它从闪存起始处加载。虽然其内容主要来自闪存，但该区域还包含设备启动期间生成的、未存储在闪存中的数据。)
    * - ``device-caps``
      - The contents of the device firmware's capabilities buffer. Useful to
        determine the current state and configuration of the device.
        (设备固件能力缓冲区的内容。可用于确定设备的当前状态和配置。)

Both the ``nvm-flash`` and ``shadow-ram`` regions can be accessed without a
snapshot. The ``device-caps`` region requires a snapshot as the contents are
sent by firmware and can't be split into separate reads.
(``nvm-flash`` 和 ``shadow-ram`` 区域无需快照即可访问。``device-caps`` 区域需要快照，因为其内容由固件发送，无法拆分为多次读取。)

Users can request an immediate capture of a snapshot for all three regions
via the ``DEVLINK_CMD_REGION_NEW`` command.
(用户可以通过 ``DEVLINK_CMD_REGION_NEW`` 命令为所有三个区域请求立即捕获快照。)

.. code:: shell

    $ devlink region show
    pci/0000:01:00.0/nvm-flash: size 10485760 snapshot [] max 1
    pci/0000:01:00.0/device-caps: size 4096 snapshot [] max 10

    $ devlink region new pci/0000:01:00.0/nvm-flash snapshot 1

    $ devlink region dump pci/0000:01:00.0/nvm-flash snapshot 1
    0000000000000000 0014 95dc 0014 9514 0035 1670 0034 db30
    0000000000000010 0000 0000 ffff ff04 0029 8c00 0028 8cc8
    0000000000000020 0016 0bb8 0016 1720 0000 0000 c00f 3ffc
    0000000000000030 bada cce5 bada cce5 bada cce5 bada cce5

    $ devlink region read pci/0000:01:00.0/nvm-flash snapshot 1 address 0 length 16
    0000000000000000 0014 95dc 0014 9514 0035 1670 0034 db30

    $ devlink region delete pci/0000:01:00.0/device-caps snapshot 1
