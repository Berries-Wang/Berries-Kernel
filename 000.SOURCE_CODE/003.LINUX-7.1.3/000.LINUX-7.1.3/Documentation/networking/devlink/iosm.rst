.. SPDX-License-Identifier: GPL-2.0

====================
iosm devlink support
====================

(iosm devlink 支持)

This document describes the devlink features implemented by the ``iosm``
device driver.
(本文档描述 ``iosm`` 设备驱动程序所实现的 devlink 功能。)

Parameters
(参数)
==========

The ``iosm`` driver implements the following driver-specific parameters.
(``iosm`` 驱动程序实现了以下驱动特定的参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``erase_full_flash``
     - u8
     - runtime
     - erase_full_flash parameter is used to check if full erase is required for
       the device during firmware flashing.
       If set, Full nand erase command will be sent to the device. By default,
       only conditional erase support is enabled.
       (erase_full_flash 参数用于检查在固件烧写期间是否需要对设备进行完全擦除。
       如果设置了该参数，将向设备发送完整的 nand 擦除命令。默认情况下，仅启用条件
       擦除支持。)


Flash Update
(闪存更新)
============

The ``iosm`` driver implements support for flash update using the
``devlink-flash`` interface.
(``iosm`` 驱动程序通过 ``devlink-flash`` 接口实现了闪存更新支持。)

It supports updating the device flash using a combined flash image which contains
the Bootloader images and other modem software images.
(它支持使用一个包含引导加载程序镜像和其他调制解调器软件镜像的组合闪存镜像来更新设备
闪存。)

The driver uses DEVLINK_SUPPORT_FLASH_UPDATE_COMPONENT to identify type of
firmware image that need to be flashed as requested by user space application.
Supported firmware image types.
(驱动程序使用 DEVLINK_SUPPORT_FLASH_UPDATE_COMPONENT 来识别用户空间应用程序所请求的
需要烧写的固件镜像类型。支持的固件镜像类型如下。)

.. list-table:: Firmware Image types
    :widths: 15 85

    * - Name (名称)
      - Description (描述)
    * - ``PSI RAM``
      - Primary Signed Image (主签名镜像)
    * - ``EBL``
      - External Bootloader (外部引导加载程序)
    * - ``FLS``
      - Modem Software Image (调制解调器软件镜像)

PSI RAM and EBL are the RAM images which are injected to the device when the
device is in BOOT ROM stage. Once this is successful, the actual modem firmware
image is flashed to the device. The modem software image contains multiple files
each having one secure bin file and at least one Loadmap/Region file. For flashing
these files, appropriate commands are sent to the modem device along with the
data required for flashing. The data like region count and address of each region
has to be passed to the driver using the devlink param command.
(PSI RAM 和 EBL 是在设备处于 BOOT ROM 阶段时注入到设备中的 RAM 镜像。一旦成功，实际的
调制解调器固件镜像就会被烧写到设备中。调制解调器软件镜像包含多个文件，每个文件有一个
secure bin 文件和至少一个 Loadmap/Region 文件。烧写这些文件时，会向调制解调器设备发送
适当的命令以及烧写所需的数据。诸如 region 数量和每个 region 的地址之类的数据必须通过
devlink param 命令传递给驱动程序。)

If the device has to be fully erased before firmware flashing, user application
need to set the erase_full_flash parameter using devlink param command.
By default, conditional erase feature is supported.
(如果在进行固件烧写之前必须完全擦除设备，用户应用程序需要使用 devlink param 命令设置
erase_full_flash 参数。默认情况下支持条件擦除特性。)

Flash Commands:
(Flash 命令：)
===============
1) When modem is in Boot ROM stage, user can use below command to inject PSI RAM
image using devlink flash command.
(1) 当调制解调器处于 Boot ROM 阶段时，用户可以使用下面的命令通过 devlink flash 命令
注入 PSI RAM 镜像。)

$ devlink dev flash pci/0000:02:00.0 file <PSI_RAM_File_name>

2) If user want to do a full erase, below command need to be issued to set the
erase full flash param (To be set only if full erase required).
(2) 如果用户想要执行完全擦除，需要执行下面的命令来设置 erase full flash 参数
(仅在需要完全擦除时才设置)。)

$ devlink dev param set pci/0000:02:00.0 name erase_full_flash value true cmode runtime

3) Inject EBL after the modem is in PSI stage.
(3) 在调制解调器进入 PSI 阶段之后注入 EBL。)

$ devlink dev flash pci/0000:02:00.0 file <EBL_File_name>

4) Once EBL is injected successfully, then the actual firmware flashing takes
place. Below is the sequence of commands used for each of the firmware images.
(4) 一旦 EBL 注入成功，就会开始实际的固件烧写。下面是针对每个固件镜像所使用的命令
序列。)

a) Flash secure bin file.
(a) 烧写 secure bin 文件。)

$ devlink dev flash pci/0000:02:00.0 file <Secure_bin_file_name>

b) Flashing the Loadmap/Region file
(b) 烧写 Loadmap/Region 文件。)

$ devlink dev flash pci/0000:02:00.0 file <Load_map_file_name>

Regions
(区域)
=======

The ``iosm`` driver supports dumping the coredump logs.
(``iosm`` 驱动程序支持转储 coredump 日志。)

In case a firmware encounters an exception, a snapshot will be taken by the
driver. Following regions are accessed for device internal data.
(当固件遇到异常时，驱动程序会拍摄一个快照。以下 region 用于访问设备内部数据。)

.. list-table:: Regions implemented
    :widths: 15 85

    * - Name (名称)
      - Description (描述)
    * - ``report.json``
      - The summary of exception details logged as part of this region.
        (作为该 region 的一部分所记录的异常详情摘要。)
    * - ``coredump.fcd``
      - This region contains the details related to the exception occurred in the
        device (RAM dump).
        (该 region 包含与设备中发生的异常相关的详细信息(RAM 转储)。)
    * - ``cdd.log``
      - This region contains the logs related to the modem CDD driver.
        (该 region 包含与调制解调器 CDD 驱动程序相关的日志。)
    * - ``eeprom.bin``
      - This region contains the eeprom logs.
        (该 region 包含 eeprom 日志。)
    * - ``bootcore_trace.bin``
      -  This region contains the current instance of bootloader logs.
         (该 region 包含引导加载程序当前实例的日志。)
    * - ``bootcore_prev_trace.bin``
      - This region contains the previous instance of bootloader logs.
        (该 region 包含引导加载程序前一个实例的日志。)


Region commands
(region 命令)
===============

$ devlink region show

$ devlink region new pci/0000:02:00.0/report.json

$ devlink region dump pci/0000:02:00.0/report.json snapshot 0

$ devlink region del pci/0000:02:00.0/report.json snapshot 0

$ devlink region new pci/0000:02:00.0/coredump.fcd

$ devlink region dump pci/0000:02:00.0/coredump.fcd snapshot 1

$ devlink region del pci/0000:02:00.0/coredump.fcd snapshot 1

$ devlink region new pci/0000:02:00.0/cdd.log

$ devlink region dump pci/0000:02:00.0/cdd.log snapshot 2

$ devlink region del pci/0000:02:00.0/cdd.log snapshot 2

$ devlink region new pci/0000:02:00.0/eeprom.bin

$ devlink region dump pci/0000:02:00.0/eeprom.bin snapshot 3

$ devlink region del pci/0000:02:00.0/eeprom.bin snapshot 3

$ devlink region new pci/0000:02:00.0/bootcore_trace.bin

$ devlink region dump pci/0000:02:00.0/bootcore_trace.bin snapshot 4

$ devlink region del pci/0000:02:00.0/bootcore_trace.bin snapshot 4

$ devlink region new pci/0000:02:00.0/bootcore_prev_trace.bin

$ devlink region dump pci/0000:02:00.0/bootcore_prev_trace.bin snapshot 5

$ devlink region del pci/0000:02:00.0/bootcore_prev_trace.bin snapshot 5
