.. SPDX-License-Identifier: GPL-2.0

=====================================
sfc devlink support(sfc devlink 支持)
=====================================

This document describes the devlink features implemented by the ``sfc``
device driver for the ef10 and ef100 devices.
(本文档描述 ``sfc`` 设备驱动针对 ef10 和 ef100 设备所实现的 devlink 功能。)

Info versions(Info 版本)
========================

The ``sfc`` driver reports the following versions
(``sfc`` 驱动报告以下版本)

.. list-table:: devlink info versions implemented
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``fw.bundle_id``
     - stored
     - Version of the firmware "bundle" image that was last used to update
       multiple components.
       (最近一次用于更新多个组件的固件 "bundle" 镜像的版本。)
   * - ``fw.mgmt.suc``
     - running
     - For boards where the management function is split between multiple
       control units, this is the SUC control unit's firmware version.
       (对于管理功能被拆分到多个控制单元的板卡，这是 SUC 控制单元的固件版本。)
   * - ``fw.mgmt.cmc``
     - running
     - For boards where the management function is split between multiple
       control units, this is the CMC control unit's firmware version.
       (对于管理功能被拆分到多个控制单元的板卡，这是 CMC 控制单元的固件版本。)
   * - ``fpga.rev``
     - running
     - FPGA design revision.
       (FPGA 设计修订版本。)
   * - ``fpga.app``
     - running
     - Datapath programmable logic version.
       (数据通路可编程逻辑版本。)
   * - ``fw.app``
     - running
     - Datapath software/microcode/firmware version.
       (数据通路软件/微码/固件版本。)
   * - ``coproc.boot``
     - running
     - SmartNIC application co-processor (APU) first stage boot loader version.
       (SmartNIC 应用协处理器 (APU) 第一阶段引导加载程序版本。)
   * - ``coproc.uboot``
     - running
     - SmartNIC application co-processor (APU) co-operating system loader version.
       (SmartNIC 应用协处理器 (APU) 协操作系统加载程序版本。)
   * - ``coproc.main``
     - running
     - SmartNIC application co-processor (APU) main operating system version.
       (SmartNIC 应用协处理器 (APU) 主操作系统版本。)
   * - ``coproc.recovery``
     - running
     - SmartNIC application co-processor (APU) recovery operating system version.
       (SmartNIC 应用协处理器 (APU) 恢复操作系统版本。)
   * - ``fw.exprom``
     - running
     - Expansion ROM version. For boards where the expansion ROM is split between
       multiple images (e.g. PXE and UEFI), this is the specifically the PXE boot
       ROM version.
       (扩展 ROM 版本。对于扩展 ROM 被拆分为多个镜像(例如 PXE 和 UEFI)的板卡，这里特指 PXE 引导 ROM 版本。)
   * - ``fw.uefi``
     - running
     - UEFI driver version (No UNDI support).
       (UEFI 驱动版本(不支持 UNDI)。)

Flash Update(Flash 更新)
========================

The ``sfc`` driver implements support for flash update using the
``devlink-flash`` interface. It supports updating the device flash using a
combined flash image ("bundle") that contains multiple components (on ef10,
typically ``fw.mgmt``, ``fw.app``, ``fw.exprom`` and ``fw.uefi``).
(``sfc`` 驱动通过 ``devlink-flash`` 接口实现了对 flash 更新的支持。它支持使用包含多个组件的组合 flash 镜像 ("bundle") 来更新设备 flash(在 ef10 上通常是 ``fw.mgmt`` 、 ``fw.app`` 、 ``fw.exprom`` 和 ``fw.uefi``)。)

The driver does not support any overwrite mask flags.
(该驱动不支持任何 overwrite 掩码标志。)
