.. SPDX-License-Identifier: GPL-2.0+

====================================================================================================
Linux Driver for the AMD/Pensando(R) DSC adapter family(AMD/Pensando(R) DSC 适配器家族的 Linux 驱动)
====================================================================================================

Copyright(c) 2023 Advanced Micro Devices, Inc
(版权所有 (c) 2023 Advanced Micro Devices, Inc)

Identifying the Adapter(识别适配器)
===================================

To find if one or more AMD/Pensando PCI Core devices are installed on the
host, check for the PCI devices::
(要确认主机上是否安装了一个或多个 AMD/Pensando PCI Core 设备，可用以下
命令检查 PCI 设备：)

  # lspci -d 1dd8:100c
  b5:00.0 Processing accelerators: Pensando Systems Device 100c
  b6:00.0 Processing accelerators: Pensando Systems Device 100c

If such devices are listed as above, then the pds_core.ko driver should find
and configure them for use.  There should be log entries in the kernel
messages such as these::
(如果列出了如上所示的设备，pds_core.ko 驱动就应当能找到它们并完成配置
以供使用。内核消息中应出现类似如下的日志条目：)

  $ dmesg | grep pds_core
  pds_core 0000:b5:00.0: 252.048 Gb/s available PCIe bandwidth (16.0 GT/s PCIe x16 link)
  pds_core 0000:b5:00.0: FW: 1.60.0-73
  pds_core 0000:b6:00.0: 252.048 Gb/s available PCIe bandwidth (16.0 GT/s PCIe x16 link)
  pds_core 0000:b6:00.0: FW: 1.60.0-73

Driver and firmware version information can be gathered with devlink::
(可以用 devlink 收集驱动与固件版本信息：)

  $ devlink dev info pci/0000:b5:00.0
  pci/0000:b5:00.0:
    driver pds_core
    serial_number FLM18420073
    versions:
        fixed:
          asic.id 0x0
          asic.rev 0x0
        running:
          fw 1.51.0-73
        stored:
          fw.goldfw 1.15.9-C-22
          fw.mainfwa 1.60.0-73
          fw.mainfwb 1.60.0-57

Info versions(信息版本)
=======================

The ``pds_core`` driver reports the following versions
(``pds_core`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw``
     - running
     - Version of firmware running on the device
       (设备上正在运行的固件版本)
   * - ``fw.goldfw``
     - stored
     - Version of firmware stored in the goldfw slot
       (存储在 goldfw 槽位中的固件版本)
   * - ``fw.mainfwa``
     - stored
     - Version of firmware stored in the mainfwa slot
       (存储在 mainfwa 槽位中的固件版本)
   * - ``fw.mainfwb``
     - stored
     - Version of firmware stored in the mainfwb slot
       (存储在 mainfwb 槽位中的固件版本)
   * - ``asic.id``
     - fixed
     - The ASIC type for this device
       (此设备的 ASIC 类型)
   * - ``asic.rev``
     - fixed
     - The revision of the ASIC for this device
       (此设备 ASIC 的版本号)

Parameters(参数)
================

The ``pds_core`` driver implements the following generic
parameters for controlling the functionality to be made available
as auxiliary_bus devices.
(``pds_core`` 驱动实现了以下通用参数，用于控制要以 auxiliary_bus 设备
形式提供的功能。)

.. list-table:: Generic parameters implemented
   :widths: 5 5 8 82

   * - Name (名称)
     - Mode (模式)
     - Type (类型)
     - Description (描述)
   * - ``enable_vnet``
     - runtime
     - Boolean
     - Enables vDPA functionality through an auxiliary_bus device
       (通过 auxiliary_bus 设备启用 vDPA 功能)

Firmware Management(固件管理)
=============================

The ``flash`` command can update a the DSC firmware.  The downloaded firmware
will be saved into either of firmware bank 1 or bank 2, whichever is not
currently in use, and that bank will used for the next boot::
(``flash`` 命令可以更新 DSC 固件。下载的固件将保存到固件 bank 1 或 bank 2
中当前未使用的那一个，并使用该 bank 进行下次启动：)

  # devlink dev flash pci/0000:b5:00.0 \
            file pensando/dsc_fw_1.63.0-22.tar

Health Reporters(健康报告器)
============================

The driver supports a devlink health reporter for FW status::
(驱动支持用于固件状态的 devlink health reporter：)

  # devlink health show pci/0000:2b:00.0 reporter fw
  pci/0000:2b:00.0:
    reporter fw
      state healthy error 0 recover 0
  # devlink health diagnose pci/0000:2b:00.0 reporter fw
   Status: healthy State: 1 Generation: 0 Recoveries: 0

Enabling the driver(启用驱动)
=============================

The driver is enabled via the standard kernel configuration system,
using the make command::
(驱动通过标准内核配置系统启用，使用 make 命令：)

  make oldconfig/menuconfig/etc.

The driver is located in the menu structure at:
(驱动在菜单结构中的位置如下：)

  -> Device Drivers
    -> Network device support (NETDEVICES [=y])
      -> Ethernet driver support
        -> AMD devices
          -> AMD/Pensando Ethernet PDS_CORE Support

Support(支持)
=============

For general Linux networking support, please use the netdev mailing
list, which is monitored by AMD/Pensando personnel::
(如需一般性的 Linux 网络支持，请使用 netdev 邮件列表，AMD/Pensando 的
人员会关注该列表：)

  netdev@vger.kernel.org
