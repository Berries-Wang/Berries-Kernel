.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

===================================
FWSEC (Firmware Security)(固件安全)
===================================
This document briefly/conceptually describes the FWSEC (Firmware Security) image
and its role in the GPU boot sequence. As such, this information is subject to
change in the future and is only current as of the Ampere GPU family. However,
hopefully the concepts described will be useful for understanding the kernel code
that deals with it. All the information is derived from publicly available
sources such as public drivers and documentation.
(本文从概念上简要描述 FWSEC(固件安全)镜像及其在 GPU 启动序列中的角色。因此，这些信息日后可能发生变化，且仅以 Ampere GPU 家族为现状基准。不过，希望这里描述的概念有助于理解内核中处理它的代码。所有信息均来自公开驱动与文档等公开来源。)

The role of FWSEC is to provide a secure boot process. It runs in
'Heavy-secure' mode, and performs firmware verification after a GPU reset
before loading various ucode images onto other microcontrollers on the GPU,
such as the PMU and GSP.
(FWSEC 的角色是提供安全启动流程。它运行在 'Heavy-secure' 模式，在 GPU 复位之后、把各个 ucode 镜像加载到 GPU 上其他微控制器(如 PMU 与 GSP)之前，执行固件验证。)

FWSEC itself is an application stored in the VBIOS ROM in the FWSEC partition of
ROM (see vbios.rst for more details). It contains different commands like FRTS
(Firmware Runtime Services) and SB (Secure Booting other microcontrollers after
reset and loading them with other non-FWSEC ucode). The kernel driver only needs
to perform FRTS, since Secure Boot (SB) has already completed by the time the driver
is loaded.
(FWSEC 本身是一个应用，存储在 VBIOS ROM 的 FWSEC 分区中(更多细节见 vbios.rst)。它包含不同的命令，例如 FRTS(固件运行时服务)与 SB(在复位后安全引导其他微控制器，并为其加载其他非 FWSEC 的 ucode)。内核驱动只需执行 FRTS，因为等到驱动加载时，安全启动 (SB) 已经完成。)

The FRTS command carves out the WPR2 region (Write protected region) which contains
data required for power management. Once setup, only HS mode ucode can access it
(see falcon.rst for privilege levels).
(FRTS 命令会划分出 WPR2 区域(写保护区域)，其中包含电源管理所需的数据。一旦建立，只有 HS 模式的 ucode 才能访问它(特权级别见 falcon.rst)。)

The FWSEC image is located in the VBIOS ROM in the partition of the ROM that contains
various ucode images (also known as applications) -- one of them being FWSEC. For how
it is extracted, see vbios.rst and the vbios.rs source code.
(FWSEC 镜像位于 VBIOS ROM 中存放各种 ucode 镜像(也称为应用)的分区——FWSEC 即其中之一。关于它如何被提取，见 vbios.rst 与 vbios.rs 源代码。)

The Falcon data for each ucode images (including the FWSEC image) is a combination
of headers, data sections (DMEM) and instruction code sections (IMEM). All these
ucode images are stored in the same ROM partition and the PMU table is used to look
up the application to load it based on its application ID (see vbios.rs).
(每个 ucode 镜像(包括 FWSEC 镜像)的 Falcon 数据由头部、数据段 (DMEM) 与指令代码段 (IMEM) 组合而成。所有这些 ucode 镜像都存放在同一个 ROM 分区中，PMU 表用于按应用 ID 查找要加载的应用(见 vbios.rs)。)

For the nova-core driver, the FWSEC contains an 'application interface' called
DMEMMAPPER. This interface is used to execute the 'FWSEC-FRTS' command, among others.
For Ampere, FWSEC is running on the GSP in Heavy-secure mode and runs FRTS.
(对 nova-core 驱动而言，FWSEC 包含一个名为 DMEMMAPPER 的"应用接口"。该接口除其他用途外，用于执行 'FWSEC-FRTS' 命令。在 Ampere 上，FWSEC 以 Heavy-secure 模式运行在 GSP 上并执行 FRTS。)

FWSEC Memory Layout(FWSEC 内存布局)
-----------------------------------
The memory layout of the FWSEC image is as follows::

   +---------------------------------------------------------------+
   |                         FWSEC ROM image (type 0xE0)           |
   |                                                               |
   |  +---------------------------------+                          |
   |  |     PMU Falcon Ucode Table      |                          |
   |  |     (PmuLookupTable)            |                          |
   |  |  +-------------------------+    |                          |
   |  |  | Table Header            |    |                          |
   |  |  | - version: 0x01         |    |                          |
   |  |  | - header_size: 6        |    |                          |
   |  |  | - entry_size: 6         |    |                          |
   |  |  | - entry_count: N        |    |                          |
   |  |  | - desc_version:3(unused)|    |                          |
   |  |  +-------------------------+    |                          |
   |  |         ...                     |                          |
   |  |  +-------------------------+    |                          |
   |  |  | Entry for FWSEC (0x85)  |    |                          |
   |  |  | (PmuLookupTableEntry)   |    |                          |
   |  |  | - app_id: 0x85 (FWSEC)  |----|----+                     |
   |  |  | - target_id: 0x01 (PMU) |    |    |                     |
   |  |  | - data: offset ---------|----|----|---+ look up FWSEC   |
   |  |  +-------------------------+    |    |   |                 |
   |  +---------------------------------+    |   |                 |
   |                                         |   |                 |
   |                                         |   |                 |
   |  +---------------------------------+    |   |                 |
   |  |     FWSEC Ucode Component       |<---+   |                 |
   |  |     (aka Falcon data)           |        |                 |
   |  |  +-------------------------+    |        |                 |
   |  |  | FalconUCodeDescV3       |<---|--------+                 |
   |  |  | - hdr                   |    |                          |
   |  |  | - stored_size           |    |                          |
   |  |  | - pkc_data_offset       |    |                          |
   |  |  | - interface_offset -----|----|----------------+         |
   |  |  | - imem_phys_base        |    |                |         |
   |  |  | - imem_load_size        |    |                |         |
   |  |  | - imem_virt_base        |    |                |         |
   |  |  | - dmem_phys_base        |    |                |         |
   |  |  | - dmem_load_size        |    |                |         |
   |  |  | - engine_id_mask        |    |                |         |
   |  |  | - ucode_id              |    |                |         |
   |  |  | - signature_count       |    |    look up sig |         |
   |  |  | - signature_versions --------------+          |         |
   |  |  +-------------------------+    |     |          |         |
   |  |         (no gap)                |     |          |         |
   |  |  +-------------------------+    |     |          |         |
   |  |  | Signatures Section      |<---|-----+          |         |
   |  |  | (384 bytes per sig)     |    |                |         |
   |  |  | - RSA-3K Signature 1    |    |                |         |
   |  |  | - RSA-3K Signature 2    |    |                |         |
   |  |  |   ...                   |    |                |         |
   |  |  +-------------------------+    |                |         |
   |  |                                 |                |         |
   |  |  +-------------------------+    |                |         |
   |  |  | IMEM Section (Code)     |    |                |         |
   |  |  |                         |    |                |         |
   |  |  | Contains instruction    |    |                |         |
   |  |  | code etc.               |    |                |         |
   |  |  +-------------------------+    |                |         |
   |  |                                 |                |         |
   |  |  +-------------------------+    |                |         |
   |  |  | DMEM Section (Data)     |    |                |         |
   |  |  |                         |    |                |         |
   |  |  | +---------------------+ |    |                |         |
   |  |  | | Application         | |<---|----------------+         |
   |  |  | | Interface Table     | |    |                          |
   |  |  | | (FalconAppifHdrV1)  | |    |                          |
   |  |  | | Header:             | |    |                          |
   |  |  | | - version: 0x01     | |    |                          |
   |  |  | | - header_size: 4    | |    |                          |
   |  |  | | - entry_size: 8     | |    |                          |
   |  |  | | - entry_count: N    | |    |                          |
   |  |  | |                     | |    |                          |
   |  |  | | Entries:            | |    |                          |
   |  |  | | +-----------------+ | |    |                          |
   |  |  | | | DEVINIT (ID 1)  | | |    |                          |
   |  |  | | | - id: 0x01      | | |    |                          |
   |  |  | | | - dmemOffset X -|-|-|----+                          |
   |  |  | | +-----------------+ | |    |                          |
   |  |  | | +-----------------+ | |    |                          |
   |  |  | | | DMEMMAPPER(ID 4)| | |    |                          |
   |  |  | | | - id: 0x04      | | |    | Used only for DevInit    |
   |  |  | | |  (NVFW_FALCON_  | | |    | application (not FWSEC)  |
   |  |  | | |   APPIF_ID_DMEMMAPPER)   |                          |
   |  |  | | | - dmemOffset Y -|-|-|----|-----+                    |
   |  |  | | +-----------------+ | |    |     |                    |
   |  |  | +---------------------+ |    |     |                    |
   |  |  |                         |    |     |                    |
   |  |  | +---------------------+ |    |     |                    |
   |  |  | | DEVINIT Engine      |<|----+     | Used by FWSEC      |
   |  |  | | Interface           | |    |     |         app.       |
   |  |  | +---------------------+ |    |     |                    |
   |  |  |                         |    |     |                    |
   |  |  | +---------------------+ |    |     |                    |
   |  |  | | DMEM Mapper (ID 4)  |<|----+-----+                    |
   |  |  | | (FalconAppifDmemmapperV3)  |                          |
   |  |  | | - signature: "DMAP" | |    |                          |
   |  |  | | - version: 0x0003   | |    |                          |
   |  |  | | - Size: 64 bytes    | |    |                          |
   |  |  | | - cmd_in_buffer_off | |----|------------+             |
   |  |  | | - cmd_in_buffer_size| |    |            |             |
   |  |  | | - cmd_out_buffer_off| |----|------------|-----+       |
   |  |  | | - cmd_out_buffer_sz | |    |            |     |       |
   |  |  | | - init_cmd          | |    |            |     |       |
   |  |  | | - features          | |    |            |     |       |
   |  |  | | - cmd_mask0/1       | |    |            |     |       |
   |  |  | +---------------------+ |    |            |     |       |
   |  |  |                         |    |            |     |       |
   |  |  | +---------------------+ |    |            |     |       |
   |  |  | | Command Input Buffer|<|----|------------+     |       |
   |  |  | | - Command data      | |    |                  |       |
   |  |  | | - Arguments         | |    |                  |       |
   |  |  | +---------------------+ |    |                  |       |
   |  |  |                         |    |                  |       |
   |  |  | +---------------------+ |    |                  |       |
   |  |  | | Command Output      |<|----|------------------+       |
   |  |  | | Buffer              | |    |                          |
   |  |  | | - Results           | |    |                          |
   |  |  | | - Status            | |    |                          |
   |  |  | +---------------------+ |    |                          |
   |  |  +-------------------------+    |                          |
   |  +---------------------------------+                          |
   |                                                               |
   +---------------------------------------------------------------+

(FWSEC 镜像的内存布局如下：)

.. note::
   This is using an GA-102 Ampere GPU as an example and could vary for future GPUs.
   (此处以 GA-102 Ampere GPU 为例，未来的 GPU 可能有所不同。)

.. note::
   The FWSEC image also plays a role in memory scrubbing (ECC initialization) and VPR
   (Video Protected Region) initialization as well. Before the nova-core driver is even
   loaded, the FWSEC image is running on the GSP in heavy-secure mode. After the devinit
   sequence completes, it does VRAM memory scrubbing (ECC initialization). On consumer
   GPUs, it scrubs only part of memory and then initiates 'async scrubbing'. Before this
   async scrubbing completes, the unscrubbed VRAM cannot be used for allocation (thus DRM
   memory allocators need to wait for this scrubbing to complete).
   (FWSEC 镜像还在内存擦除 (ECC 初始化) 与 VPR(视频保护区域)初始化中发挥作用。早在 nova-core 驱动加载之前，FWSEC 镜像就已以 heavy-secure 模式运行在 GSP 上。devinit 序列完成后，它执行 VRAM 内存擦除 (ECC 初始化)。在消费级 GPU 上，它只擦除一部分内存，然后启动"异步擦除"。在该异步擦除完成之前，未擦除的 VRAM 不能用于分配(因此 DRM 内存分配器需要等待擦除完成)。)
