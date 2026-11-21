.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

================
VBIOS(视频 BIOS)
================
This document describes the layout of the VBIOS image which is a series of concatenated
images in the ROM of the GPU. The VBIOS is mirrored onto the BAR 0 space and is read
by both Boot ROM firmware (also known as IFR or init-from-rom firmware) on the GPU to
bootstrap various microcontrollers (PMU, SEC, GSP) with critical initialization before
the driver loads, as well as by the nova-core driver in the kernel to boot the GSP.
(本文描述 VBIOS 镜像的布局，它是一系列在 GPU ROM 中前后相连的镜像。VBIOS 被映射到 BAR 0 空间，既由 GPU 上的 Boot ROM 固件(也称 IFR 或 init-from-rom 固件)读取——用于在驱动加载之前，对各种微控制器(PMU、SEC、GSP)完成关键初始化引导——也由内核中的 nova-core 驱动读取，用于引导 GSP。)

The format of the images in the ROM follow the "BIOS Specification" part of the
PCI specification, with Nvidia-specific extensions. The ROM images of type FwSec
are the ones that contain Falcon ucode and what we are mainly looking for.
(ROM 中镜像的格式遵循 PCI 规范的 "BIOS Specification" 部分，外加 Nvidia 专有扩展。类型为 FwSec 的 ROM 镜像包含 Falcon ucode，是我们主要关注的目标。)

As an example, the following are the different image types that can be found in the
VBIOS of an Ampere GA102 GPU which is supported by the nova-core driver.
(以 nova-core 驱动支持的 Ampere GA102 GPU 为例，其 VBIOS 中可以找到以下不同类型的镜像。)

- PciAt Image (Type 0x00) - This is the standard PCI BIOS image, whose name
  likely comes from the "IBM PC/AT" architecture.
  (PciAt Image(类型 0x00)- 标准 PCI BIOS 镜像，其名称很可能来自 "IBM PC/AT" 架构。)

- EFI Image (Type 0x03) - This is the EFI BIOS image. It contains the UEFI GOP
  driver that is used to display UEFI graphics output.
  (EFI Image(类型 0x03)- EFI BIOS 镜像。它包含用于显示 UEFI 图形输出的 UEFI GOP 驱动。)

- First FwSec Image (Type 0xE0) - The first FwSec image (Secure Firmware)
  (First FwSec Image(类型 0xE0)- 第一个 FwSec 镜像(安全固件))

- Second FwSec Image (Type 0xE0) - The second FwSec image (Secure Firmware)
  contains various  microcodes (also known as an applications) that do a range
  of different functions. The FWSEC ucode is run in heavy-secure mode and
  typically runs directly on the GSP (it could be running on a different
  designated processor in future generations but as of Ampere, it is the GSP).
  This firmware then loads other firmware ucodes onto the PMU and SEC2
  microcontrollers for gfw initialization after GPU reset and before the driver
  loads (see devinit.rst). The DEVINIT ucode is itself another ucode that is
  stored in this ROM partition.
  (Second FwSec Image(类型 0xE0)- 第二个 FwSec 镜像(安全固件)，包含执行多种不同功能的各类微码(也称为应用)。FWSEC ucode 以 heavy-secure 模式运行，通常直接运行在 GSP 上(未来各代可能运行在其他指定处理器上，但截至 Ampere 世代，它是 GSP)。该固件随后把其他固件 ucode 加载到 PMU 与 SEC2 微控制器上，用于 GPU 复位之后、驱动加载之前的 gfw 初始化(见 devinit.rst)。DEVINIT ucode 本身是另一个 ucode，也存储在这个 ROM 分区中。)

Once located, the Falcon ucodes have "Application Interfaces" in their data
memory (DMEM). For FWSEC, the application interface we use for FWSEC is the
"DMEM mapper" interface which is configured to run the "FRTS" command. This
command carves out the WPR2 (Write-Protected Region) in VRAM. It then places
important power-management data, called 'FRTS', into this region. The WPR2
region is only accessible to heavy-secure ucode.
(定位之后，Falcon ucode 在其数据内存 (DMEM) 中带有"应用接口"。对 FWSEC 而言，我们使用的应用接口是"DMEM 映射器"接口，它被配置为运行 "FRTS" 命令。该命令在 VRAM 中划分出 WPR2(写保护区域)，随后把名为 'FRTS' 的重要电源管理数据放入该区域。WPR2 区域只有 heavy-secure ucode 可以访问。)

.. note::
   It is not clear why FwSec has 2 different partitions in the ROM, but they both
   are of type 0xE0 and can be identified as such. This could be subject to change
   in future generations.
   (尚不清楚 FwSec 为何在 ROM 中有两个不同分区，但两者类型都是 0xE0，可据此识别。这在未来的世代中可能发生变化。)

VBIOS ROM Layout(VBIOS ROM 布局)
--------------------------------
The VBIOS layout is roughly a series of concatenated images laid out as follows::

    +----------------------------------------------------------------------------+
    | VBIOS (Starting at ROM_OFFSET: 0x300000)                                   |
    +----------------------------------------------------------------------------+
    | +-----------------------------------------------+                          |
    | | PciAt Image (Type 0x00)                       |                          |
    | +-----------------------------------------------+                          |
    | | +-------------------+                         |                          |
    | | | ROM Header        |                         |                          |
    | | | (Signature 0xAA55)|                         |                          |
    | | +-------------------+                         |                          |
    | |         | rom header's pci_data_struct_offset |                          |
    | |         | points to the PCIR structure        |                          |
    | |         V                                     |                          |
    | | +-------------------+                         |                          |
    | | | PCIR Structure    |                         |                          |
    | | | (Signature "PCIR")|                         |                          |
    | | | last_image: 0x80  |                         |                          |
    | | | image_len: size   |                         |                          |
    | | | in 512-byte units |                         |                          |
    | | +-------------------+                         |                          |
    | |         |                                     |                          |
    | |         | NPDE immediately follows PCIR       |                          |
    | |         V                                     |                          |
    | | +-------------------+                         |                          |
    | | | NPDE Structure    |                         |                          |
    | | | (Signature "NPDE")|                         |                          |
    | | | last_image: 0x00  |                         |                          |
    | | +-------------------+                         |                          |
    | |                                               |                          |
    | | +-------------------+                         |                          |
    | | | BIT Header        | (Signature scanning     |                          |
    | | | (Signature "BIT") |  provides the location  |                          |
    | | +-------------------+  of the BIT table)      |                          |
    | |         | header is                           |                          |
    | |         | followed by a table of tokens       |                          |
    | |         V one of which is for falcon data.    |                          |
    | | +-------------------+                         |                          |
    | | | BIT Tokens        |                         |                          |
    | | |  ______________   |                         |                          |
    | | | | Falcon Data |   |                         |                          |
    | | | | Token (0x70)|---+------------>------------+--+                       |
    | | | +-------------+   |  falcon_data_ptr()      |  |                       |
    | | +-------------------+                         |  V                       |
    | +-----------------------------------------------+  |                       |
    |              (no gap between images)               |                       |
    | +-----------------------------------------------+  |                       |
    | | EFI Image (Type 0x03)                         |  |                       |
    | +-----------------------------------------------+  |                       |
    | | Contains the UEFI GOP driver (Graphics Output)|  |                       |
    | | +-------------------+                         |  |                       |
    | | | ROM Header        |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | PCIR Structure    |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | NPDE Structure    |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | Image data        |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | +-----------------------------------------------+  |                       |
    |              (no gap between images)               |                       |
    | +-----------------------------------------------+  |                       |
    | | First FwSec Image (Type 0xE0)                 |  |                       |
    | +-----------------------------------------------+  |                       |
    | | +-------------------+                         |  |                       |
    | | | ROM Header        |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | PCIR Structure    |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | NPDE Structure    |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | Image data        |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | +-----------------------------------------------+  |                       |
    |              (no gap between images)               |                       |
    | +-----------------------------------------------+  |                       |
    | | Second FwSec Image (Type 0xE0)                |  |                       |
    | +-----------------------------------------------+  |                       |
    | | +-------------------+                         |  |                       |
    | | | ROM Header        |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | PCIR Structure    |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | | | NPDE Structure    |                         |  |                       |
    | | +-------------------+                         |  |                       |
    | |                                               |  |                       |
    | | +-------------------+                         |  |                       |
    | | | PMU Lookup Table  | <- falcon_data_offset <----+                       |
    | | | +-------------+   |    pmu_lookup_table     |                          |
    | | | | Entry 0x85  |   |                         |                          |
    | | | | FWSEC_PROD  |   |                         |                          |
    | | | +-------------+   |                         |                          |
    | | +-------------------+                         |                          |
    | |         |                                     |                          |
    | |         | points to                           |                          |
    | |         V                                     |                          |
    | | +-------------------+                         |                          |
    | | | FalconUCodeDescV3 | <- falcon_ucode_offset  |                          |
    | | | (FWSEC Firmware)  |    fwsec_header()       |                          |
    | | +-------------------+                         |                          |
    | |         |   immediately followed  by...       |                          |
    | |         V                                     |                          |
    | | +----------------------------+                |                          |
    | | | Signatures + FWSEC Ucode   |                |                          |
    | | | fwsec_sigs(), fwsec_ucode()|                |                          |
    | | +----------------------------+                |                          |
    | +-----------------------------------------------+                          |
    |                                                                            |
    +----------------------------------------------------------------------------+

(VBIOS 布局大致是一系列按如下方式前后相连的镜像：)

.. note::
   This diagram is created based on an GA-102 Ampere GPU as an example and could
   vary for future or other GPUs.
   (本图以 GA-102 Ampere GPU 为例绘制，未来的或其他 GPU 可能有所不同。)

.. note::
   For more explanations of acronyms, see the detailed descriptions in `vbios.rs`.
   (更多缩略语解释见 `vbios.rs` 中的详细描述。)

Falcon data Lookup(Falcon 数据查找)
-----------------------------------
A key part of the VBIOS extraction code (vbios.rs) is to find the location of the
Falcon data in the VBIOS which contains the PMU lookup table. This lookup table is
used to find the required Falcon ucode based on an application ID.
(VBIOS 提取代码 (vbios.rs) 的一个关键部分，是在 VBIOS 中找到包含 PMU 查找表的 Falcon 数据的位置。该查找表用于根据应用 ID 找到所需的 Falcon ucode。)

The location of the PMU lookup table is found by scanning the BIT (`BIOS Information Table`_)
tokens for a token with the id `BIT_TOKEN_ID_FALCON_DATA` (0x70) which indicates the
offset of the same from the start of the VBIOS image. Unfortunately, the offset
does not account for the EFI image located between the PciAt and FwSec images.
The `vbios.rs` code compensates for this with appropriate arithmetic.
(PMU 查找表的位置通过扫描 BIT(`BIOS Information Table`_)令牌找到：寻找 id 为 `BIT_TOKEN_ID_FALCON_DATA` (0x70) 的令牌，它指示该表相对于 VBIOS 镜像起始处的偏移。遗憾的是，该偏移没有把位于 PciAt 镜像与 FwSec 镜像之间的 EFI 镜像计入。`vbios.rs` 代码用适当的算术补偿了这一点。)

.. _`BIOS Information Table`: https://download.nvidia.com/open-gpu-doc/BIOS-Information-Table/1/BIOS-Information-Table.html
