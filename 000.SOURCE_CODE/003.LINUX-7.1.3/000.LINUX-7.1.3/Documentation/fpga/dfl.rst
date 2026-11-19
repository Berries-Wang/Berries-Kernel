===================================================================================
FPGA Device Feature List (DFL) Framework Overview(FPGA 设备特性列表 (DFL) 框架概览)
===================================================================================

Authors:
(作者：)

- Enno Luebbers <enno.luebbers@intel.com>
- Xiao Guangrong <guangrong.xiao@linux.intel.com>
- Wu Hao <hao.wu@intel.com>
- Xu Yilun <yilun.xu@intel.com>

The Device Feature List (DFL) FPGA framework (and drivers according to
this framework) hides the very details of low layer hardware and provides
unified interfaces to userspace. Applications could use these interfaces to
configure, enumerate, open and access FPGA accelerators on platforms which
implement the DFL in the device memory. Besides this, the DFL framework
enables system level management functions such as FPGA reconfiguration.
(设备特性列表 (DFL) FPGA 框架（以及依照该框架编写的驱动）隐藏了底层硬件的诸多
细节，并向用户空间提供统一接口。应用程序可以使用这些接口，在设备内存中实现
了 DFL 的平台上配置、枚举、打开和访问 FPGA 加速器。除此之外，DFL 框架还支持
FPGA 重配置等系统级管理功能。)


Device Feature List (DFL) Overview(设备特性列表 (DFL) 概览)
===========================================================
Device Feature List (DFL) defines a linked list of feature headers within the
device MMIO space to provide an extensible way of adding features. Software can
walk through these predefined data structures to enumerate FPGA features:
FPGA Interface Unit (FIU), Accelerated Function Unit (AFU) and Private Features,
as illustrated below::

    Header            Header            Header            Header
 +----------+  +-->+----------+  +-->+----------+  +-->+----------+
 |   Type   |  |   |  Type    |  |   |  Type    |  |   |  Type    |
 |   FIU    |  |   | Private  |  |   | Private  |  |   | Private  |
 +----------+  |   | Feature  |  |   | Feature  |  |   | Feature  |
 | Next_DFH |--+   +----------+  |   +----------+  |   +----------+
 +----------+      | Next_DFH |--+   | Next_DFH |--+   | Next_DFH |--> NULL
 |    ID    |      +----------+      +----------+      +----------+
 +----------+      |    ID    |      |    ID    |      |    ID    |
 | Next_AFU |--+   +----------+      +----------+      +----------+
 +----------+  |   | Feature  |      | Feature  |      | Feature  |
 |  Header  |  |   | Register |      | Register |      | Register |
 | Register |  |   |   Set    |      |   Set    |      |   Set    |
 |   Set    |  |   +----------+      +----------+      +----------+
 +----------+  |      Header
               +-->+----------+
                   |   Type   |
                   |   AFU    |
                   +----------+
                   | Next_DFH |--> NULL
                   +----------+
                   |   GUID   |
                   +----------+
                   |  Header  |
                   | Register |
                   |   Set    |
                   +----------+

(设备特性列表 (DFL) 在设备 MMIO 空间中定义了一个特性头链表，从而提供一种可
扩展的添加特性的方式。软件可以遍历这些预定义的数据结构来枚举 FPGA 特性：
FPGA 接口单元 (FIU)、加速函数单元 (AFU) 和私有特性，如下图所示：)

FPGA Interface Unit (FIU) represents a standalone functional unit for the
interface to FPGA, e.g. the FPGA Management Engine (FME) and Port (more
descriptions on FME and Port in later sections).
(FPGA 接口单元 (FIU) 代表通往 FPGA 的接口上一个独立的功能单元，例如 FPGA 管理
引擎 (FME) 和 Port（FME 与 Port 的更多说明见后续章节）。)

Accelerated Function Unit (AFU) represents an FPGA programmable region and
always connects to a FIU (e.g. a Port) as its child as illustrated above.
(加速函数单元 (AFU) 代表一个 FPGA 可编程区域，如上图所示，它总是作为子节点
连接到某个 FIU（例如 Port）。)

Private Features represent sub features of the FIU and AFU. They could be
various function blocks with different IDs, but all private features which
belong to the same FIU or AFU, must be linked to one list via the Next Device
Feature Header (Next_DFH) pointer.
(私有特性 (Private Features) 代表 FIU 和 AFU 的子特性。它们可以是具有不同 ID
的各种功能块，但属于同一 FIU 或 AFU 的所有私有特性，都必须通过下一个设备特性
头 (Next Device Feature Header, Next_DFH) 指针链接到同一个列表上。)

Each FIU, AFU and Private Feature could implement its own functional registers.
The functional register set for FIU and AFU, is named as Header Register Set,
e.g. FME Header Register Set, and the one for Private Feature, is named as
Feature Register Set, e.g. FME Partial Reconfiguration Feature Register Set.
(每个 FIU、AFU 和私有特性都可以实现自己的功能寄存器。FIU 和 AFU 的功能寄存器
集称为头寄存器集 (Header Register Set)，例如 FME 头寄存器集；私有特性的功能
寄存器集称为特性寄存器集 (Feature Register Set)，例如 FME 部分重配置特性
寄存器集。)

This Device Feature List provides a way of linking features together, it's
convenient for software to locate each feature by walking through this list,
and can be implemented in register regions of any FPGA device.
(这个设备特性列表提供了一种把特性链接在一起的方式，便于软件通过遍历该列表来
定位每个特性，并且可以实现在任何 FPGA 设备的寄存器区域中。)


Device Feature Header - Version 0(设备特性头 - 版本 0)
======================================================
Version 0 (DFHv0) is the original version of the Device Feature Header.
All multi-byte quantities in DFHv0 are little-endian.
The format of DFHv0 is shown below::

    +-----------------------------------------------------------------------+
    |63 Type 60|59 DFH VER 52|51 Rsvd 41|40 EOL|39 Next 16|15 REV 12|11 ID 0| 0x00
    +-----------------------------------------------------------------------+
    |63                                 GUID_L                             0| 0x08
    +-----------------------------------------------------------------------+
    |63                                 GUID_H                             0| 0x10
    +-----------------------------------------------------------------------+

(版本 0 (DFHv0) 是设备特性头的最初版本。DFHv0 中所有多字节量均为小端序。
DFHv0 的格式如下所示：)

- Offset 0x00

  * Type - The type of DFH (e.g. FME, AFU, or private feature).
    (Type - DFH 的类型（例如 FME、AFU 或私有特性）。)
  * DFH VER - The version of the DFH.
    (DFH VER - DFH 的版本。)
  * Rsvd - Currently unused.
    (Rsvd - 目前未使用。)
  * EOL - Set if the DFH is the end of the Device Feature List (DFL).
    (EOL - 如果该 DFH 是设备特性列表 (DFL) 的结尾则置位。)
  * Next - The offset in bytes of the next DFH in the DFL from the DFH start,
    and the start of a DFH must be aligned to an 8 byte boundary.
    If EOL is set, Next is the size of MMIO of the last feature in the list.
    (Next - DFL 中下一个 DFH 相对于 DFH 起始处的字节偏移，且 DFH 的起始必须
    按 8 字节边界对齐。如果设置了 EOL，则 Next 是列表中最后一个特性的 MMIO
    大小。)
  * REV - The revision of the feature associated with this header.
    (REV - 与该头关联的特性的修订版本。)
  * ID - The feature ID if Type is private feature.
    (ID - 如果 Type 为私有特性，则为特性 ID。)

- Offset 0x08

  * GUID_L - Least significant 64 bits of a 128-bit Globally Unique Identifier
    (present only if Type is FME or AFU).
    (GUID_L - 128 位全局唯一标识符的低 64 位（仅当 Type 为 FME 或 AFU 时
    存在）。)

- Offset 0x10

  * GUID_H - Most significant 64 bits of a 128-bit Globally Unique Identifier
    (present only if Type is FME or AFU).
    (GUID_H - 128 位全局唯一标识符的高 64 位（仅当 Type 为 FME 或 AFU 时
    存在）。)


Device Feature Header - Version 1(设备特性头 - 版本 1)
======================================================
Version 1 (DFHv1) of the Device Feature Header adds the following functionality:
(设备特性头的版本 1 (DFHv1) 增加了以下功能：)

* Provides a standardized mechanism for features to describe
  parameters/capabilities to software.
  (提供一种标准化机制，让特性向软件描述参数/能力。)
* Standardize the use of a GUID for all DFHv1 types.
  (为所有 DFHv1 类型规范 GUID 的使用。)
* Decouples the DFH location from the register space of the feature itself.
  (把 DFH 位置与特性自身的寄存器空间解耦。)

All multi-byte quantities in DFHv1 are little-endian.
The format of Version 1 of the Device Feature Header (DFH) is shown below::

    +-----------------------------------------------------------------------+
    |63 Type 60|59 DFH VER 52|51 Rsvd 41|40 EOL|39 Next 16|15 REV 12|11 ID 0| 0x00
    +-----------------------------------------------------------------------+
    |63                                 GUID_L                             0| 0x08
    +-----------------------------------------------------------------------+
    |63                                 GUID_H                             0| 0x10
    +-----------------------------------------------------------------------+
    |63                   Reg Address/Offset                      1|  Rel  0| 0x18
    +-----------------------------------------------------------------------+
    |63        Reg Size       32|Params 31|30 Group    16|15 Instance      0| 0x20
    +-----------------------------------------------------------------------+
    |63 Next    35|34RSV33|EOP32|31 Param Version 16|15 Param ID           0| 0x28
    +-----------------------------------------------------------------------+
    |63                 Parameter Data                                     0| 0x30
    +-----------------------------------------------------------------------+

                                  ...

    +-----------------------------------------------------------------------+
    |63 Next    35|34RSV33|EOP32|31 Param Version 16|15 Param ID           0|
    +-----------------------------------------------------------------------+
    |63                 Parameter Data                                     0|
    +-----------------------------------------------------------------------+

(DFHv1 中所有多字节量均为小端序。设备特性头 (DFH) 版本 1 的格式如下所示：)

- Offset 0x00

  * Type - The type of DFH (e.g. FME, AFU, or private feature).
    (Type - DFH 的类型（例如 FME、AFU 或私有特性）。)
  * DFH VER - The version of the DFH.
    (DFH VER - DFH 的版本。)
  * Rsvd - Currently unused.
    (Rsvd - 目前未使用。)
  * EOL - Set if the DFH is the end of the Device Feature List (DFL).
    (EOL - 如果该 DFH 是设备特性列表 (DFL) 的结尾则置位。)
  * Next - The offset in bytes of the next DFH in the DFL from the DFH start,
    and the start of a DFH must be aligned to an 8 byte boundary.
    If EOL is set, Next is the size of MMIO of the last feature in the list.
    (Next - DFL 中下一个 DFH 相对于 DFH 起始处的字节偏移，且 DFH 的起始必须
    按 8 字节边界对齐。如果设置了 EOL，则 Next 是列表中最后一个特性的 MMIO
    大小。)
  * REV - The revision of the feature associated with this header.
    (REV - 与该头关联的特性的修订版本。)
  * ID - The feature ID if Type is private feature.
    (ID - 如果 Type 为私有特性，则为特性 ID。)

- Offset 0x08

  * GUID_L - Least significant 64 bits of a 128-bit Globally Unique Identifier.
    (GUID_L - 128 位全局唯一标识符的低 64 位。)

- Offset 0x10

  * GUID_H - Most significant 64 bits of a 128-bit Globally Unique Identifier.
    (GUID_H - 128 位全局唯一标识符的高 64 位。)

- Offset 0x18

  * Reg Address/Offset - If Rel bit is set, then the value is the high 63 bits
    of a 16-bit aligned absolute address of the feature's registers. Otherwise
    the value is the offset from the start of the DFH of the feature's registers.
    (Reg Address/Offset - 如果设置了 Rel 位，则该值是特性寄存器的 16 位对齐
    绝对地址的高 63 位；否则该值是特性寄存器相对于 DFH 起始处的偏移。)

- Offset 0x20

  * Reg Size - Size of feature's register set in bytes.
    (Reg Size - 特性寄存器集的大小（字节）。)
  * Params - Set if DFH has a list of parameter blocks.
    (Params - 如果 DFH 带有参数块列表则置位。)
  * Group - Id of group if feature is part of a group.
    (Group - 如果特性属于某个组，则为该组的 ID。)
  * Instance - Id of feature instance within a group.
    (Instance - 组内特性实例的 ID。)

- Offset 0x28 if feature has parameters

  * Next - Offset to the next parameter block in 8 byte words. If EOP set,
    size in 8 byte words of last parameter.
    (Next - 以 8 字节字为单位的下一个参数块偏移。如果设置了 EOP，则为最后一个
    参数以 8 字节字为单位的大小。)
  * Param Version - Version of Param ID.
    (Param Version - Param ID 的版本。)
  * Param ID - ID of parameter.
    (Param ID - 参数的 ID。)

- Offset 0x30

  * Parameter Data - Parameter data whose size and format is defined by
    version and ID of the parameter.
    (Parameter Data - 参数数据，其大小与格式由该参数的版本和 ID 定义。)


FIU - FME (FPGA Management Engine)(FIU - FME（FPGA 管理引擎）)
==============================================================
The FPGA Management Engine performs reconfiguration and other infrastructure
functions. Each FPGA device only has one FME.
(FPGA 管理引擎执行重配置和其他基础设施功能。每个 FPGA 设备只有一个 FME。)

User-space applications can acquire exclusive access to the FME using open(),
and release it using close().
(用户空间应用程序可以使用 open() 获得对 FME 的独占访问，并使用 close() 释放
它。)

The following functions are exposed through ioctls:
(以下功能通过 ioctl 暴露：)

- Get driver API version (DFL_FPGA_GET_API_VERSION)(获取驱动 API 版本)
- Check for extensions (DFL_FPGA_CHECK_EXTENSION)(检查扩展)
- Program bitstream (DFL_FPGA_FME_PORT_PR)(烧写比特流)
- Assign port to PF (DFL_FPGA_FME_PORT_ASSIGN)(把 port 分配给 PF)
- Release port from PF (DFL_FPGA_FME_PORT_RELEASE)(把 port 从 PF 释放)
- Get number of irqs of FME global error (DFL_FPGA_FME_ERR_GET_IRQ_NUM)(获取 FME 全局错误的中断数)
- Set interrupt trigger for FME error (DFL_FPGA_FME_ERR_SET_IRQ)(设置 FME 错误的中断触发)

More functions are exposed through sysfs
(/sys/class/fpga_region/regionX/dfl-fme.n/):
(更多功能通过 sysfs (/sys/class/fpga_region/regionX/dfl-fme.n/) 暴露：)

 Read bitstream ID (bitstream_id)
     bitstream_id indicates version of the static FPGA region.
     (读取比特流 ID (bitstream_id)：bitstream_id 指示静态 FPGA 区域的版本。)

 Read bitstream metadata (bitstream_metadata)
     bitstream_metadata includes detailed information of static FPGA region,
     e.g. synthesis date and seed.
     (读取比特流元数据 (bitstream_metadata)：bitstream_metadata 包含静态 FPGA
     区域的详细信息，例如综合日期和种子。)

 Read number of ports (ports_num)
     one FPGA device may have more than one port, this sysfs interface indicates
     how many ports the FPGA device has.
     (读取 port 数量 (ports_num)：一个 FPGA 设备可能有多个 port，此 sysfs
     接口指示该 FPGA 设备有多少个 port。)

 Global error reporting management (errors/)
     error reporting sysfs interfaces allow user to read errors detected by the
     hardware, and clear the logged errors.
     (全局错误报告管理 (errors/)：错误报告 sysfs 接口允许用户读取硬件检测到的
     错误，并清除已记录的错误。)

 Power management (dfl_fme_power hwmon)
     power management hwmon sysfs interfaces allow user to read power management
     information (power consumption, thresholds, threshold status, limits, etc.)
     and configure power thresholds for different throttling levels.
     (电源管理 (dfl_fme_power hwmon)：电源管理 hwmon sysfs 接口允许用户读取
     电源管理信息（功耗、阈值、阈值状态、限制等），并为不同的节流级别配置
     电源阈值。)

 Thermal management (dfl_fme_thermal hwmon)
     thermal management hwmon sysfs interfaces allow user to read thermal
     management information (current temperature, thresholds, threshold status,
     etc.).
     (热管理 (dfl_fme_thermal hwmon)：热管理 hwmon sysfs 接口允许用户读取热
     管理信息（当前温度、阈值、阈值状态等）。)

 Performance reporting
     performance counters are exposed through perf PMU APIs. Standard perf tool
     can be used to monitor all available perf events. Please see performance
     counter section below for more detailed information.
     (性能报告：性能计数器通过 perf PMU API 暴露。可以使用标准 perf 工具监视
     所有可用的 perf 事件。更多详细信息请参见下文的性能计数器一节。)


FIU - PORT(端口)
================
A port represents the interface between the static FPGA fabric and a partially
reconfigurable region containing an AFU. It controls the communication from SW
to the accelerator and exposes features such as reset and debug. Each FPGA
device may have more than one port, but always one AFU per port.
(port 代表静态 FPGA 布线与包含 AFU 的部分可重配置区域之间的接口。它控制软件
与加速器之间的通信，并暴露诸如复位与调试之类的特性。每个 FPGA 设备可以有多个
port，但每个 port 总是只有一个 AFU。)


AFU(加速函数单元)
=================
An AFU is attached to a port FIU and exposes a fixed length MMIO region to be
used for accelerator-specific control registers.
(AFU 附着在 port FIU 上，并暴露一个固定长度的 MMIO 区域，用作加速器专用的
控制寄存器。)

User-space applications can acquire exclusive access to an AFU attached to a
port by using open() on the port device node and release it using close().
(用户空间应用程序可以通过在 port 设备节点上使用 open() 获得对附着于该 port 的
AFU 的独占访问，并使用 close() 释放它。)

The following functions are exposed through ioctls:
(以下功能通过 ioctl 暴露：)

- Get driver API version (DFL_FPGA_GET_API_VERSION)(获取驱动 API 版本)
- Check for extensions (DFL_FPGA_CHECK_EXTENSION)(检查扩展)
- Get port info (DFL_FPGA_PORT_GET_INFO)(获取 port 信息)
- Get MMIO region info (DFL_FPGA_PORT_GET_REGION_INFO)(获取 MMIO 区域信息)
- Map DMA buffer (DFL_FPGA_PORT_DMA_MAP)(映射 DMA 缓冲区)
- Unmap DMA buffer (DFL_FPGA_PORT_DMA_UNMAP)(解除 DMA 缓冲区映射)
- Reset AFU (DFL_FPGA_PORT_RESET)(复位 AFU)
- Get number of irqs of port error (DFL_FPGA_PORT_ERR_GET_IRQ_NUM)(获取 port 错误的中断数)
- Set interrupt trigger for port error (DFL_FPGA_PORT_ERR_SET_IRQ)(设置 port 错误的中断触发)
- Get number of irqs of UINT (DFL_FPGA_PORT_UINT_GET_IRQ_NUM)(获取 UINT 的中断数)
- Set interrupt trigger for UINT (DFL_FPGA_PORT_UINT_SET_IRQ)(设置 UINT 的中断触发)

DFL_FPGA_PORT_RESET:
  reset the FPGA Port and its AFU. Userspace can do Port
  reset at any time, e.g. during DMA or Partial Reconfiguration. But it should
  never cause any system level issue, only functional failure (e.g. DMA or PR
  operation failure) and be recoverable from the failure.
  (DFL_FPGA_PORT_RESET：复位 FPGA Port 及其 AFU。用户空间可以在任何时候执行
  port 复位，例如在 DMA 或部分重配置期间。但它绝不应造成任何系统级问题，只应
  产生功能性失败（例如 DMA 或 PR 操作失败），并且可以从失败中恢复。)

User-space applications can also mmap() accelerator MMIO regions.
(用户空间应用程序还可以 mmap() 加速器的 MMIO 区域。)

More functions are exposed through sysfs:
(/sys/class/fpga_region/<regionX>/<dfl-port.m>/):
(更多功能通过 sysfs (/sys/class/fpga_region/<regionX>/<dfl-port.m>/) 暴露：)

 Read Accelerator GUID (afu_id)
     afu_id indicates which PR bitstream is programmed to this AFU.
     (读取加速器 GUID (afu_id)：afu_id 指示哪个 PR 比特流被烧写到了此 AFU。)

 Error reporting (errors/)
     error reporting sysfs interfaces allow user to read port/afu errors
     detected by the hardware, and clear the logged errors.
     (错误报告 (errors/)：错误报告 sysfs 接口允许用户读取硬件检测到的
     port/afu 错误，并清除已记录的错误。)


DFL Framework Overview(DFL 框架概览)
====================================

::

         +----------+    +--------+ +--------+ +--------+
         |   FME    |    |  AFU   | |  AFU   | |  AFU   |
         |  Module  |    | Module | | Module | | Module |
         +----------+    +--------+ +--------+ +--------+
                 +-----------------------+
                 | FPGA Container Device |    Device Feature List
                 |  (FPGA Base Region)   |         Framework
                 +-----------------------+
  ------------------------------------------------------------------
               +----------------------------+
               |   FPGA DFL Device Module   |
               | (e.g. PCIE/Platform Device)|
               +----------------------------+
                 +------------------------+
                   |  FPGA Hardware Device  |
                 +------------------------+

DFL framework in kernel provides common interfaces to create container device
(FPGA base region), discover feature devices and their private features from the
given Device Feature Lists and create platform devices for feature devices
(e.g. FME, Port and AFU) with related resources under the container device. It
also abstracts operations for the private features and exposes common ops to
feature device drivers.
(内核中的 DFL 框架提供通用接口，用于创建容器设备（FPGA base region）、从给定
的设备特性列表中发现特性设备及其私有特性，并在容器设备下为特性设备（例如
FME、Port 和 AFU）创建带有相关资源的平台设备。它还为私有特性抽象出操作，并
向特性设备驱动暴露通用操作。)

The FPGA DFL Device could be different hardware, e.g. PCIe device, platform
device and etc. Its driver module is always loaded first once the device is
created by the system. This driver plays an infrastructural role in the
driver architecture. It locates the DFLs in the device memory, handles them
and related resources to common interfaces from DFL framework for enumeration.
(Please refer to drivers/fpga/dfl.c for detailed enumeration APIs).
(FPGA DFL 设备可以是不同的硬件，例如 PCIe 设备、平台设备等。一旦系统创建了
设备，其驱动模块总是最先被加载。该驱动在驱动架构中扮演基础设施的角色。它在
设备内存中定位 DFL，并把 DFL 及相关资源交给 DFL 框架的通用接口进行枚举。
（详细的枚举 API 请参阅 drivers/fpga/dfl.c）。)

The FPGA Management Engine (FME) driver is a platform driver which is loaded
automatically after FME platform device creation from the DFL device module. It
provides the key features for FPGA management, including:
(FPGA 管理引擎 (FME) 驱动是一个平台驱动，在 DFL 设备模块创建 FME 平台设备之后
自动加载。它提供 FPGA 管理的关键特性，包括：)

	a) Expose static FPGA region information, e.g. version and metadata.
	   Users can read related information via sysfs interfaces exposed
	   by FME driver.
	   (a) 暴露静态 FPGA 区域信息，例如版本与元数据。用户可以通过 FME 驱动
	   暴露的 sysfs 接口读取相关信息。)

	b) Partial Reconfiguration. The FME driver creates FPGA manager, FPGA
	   bridges and FPGA regions during PR sub feature initialization. Once
	   it receives a DFL_FPGA_FME_PORT_PR ioctl from user, it invokes the
	   common interface function from FPGA Region to complete the partial
	   reconfiguration of the PR bitstream to the given port.
	   (b) 部分重配置 (Partial Reconfiguration)。FME 驱动在 PR 子特性初始化
	   期间创建 FPGA manager、FPGA bridge 和 FPGA region。一旦它从用户收到
	   DFL_FPGA_FME_PORT_PR ioctl，就会调用 FPGA Region 的通用接口函数，完成
	   PR 比特流向给定 port 的部分重配置。)

Similar to the FME driver, the FPGA Accelerated Function Unit (AFU) driver is
probed once the AFU platform device is created. The main function of this module
is to provide an interface for userspace applications to access the individual
accelerators, including basic reset control on port, AFU MMIO region export, dma
buffer mapping service functions.
(与 FME 驱动类似，FPGA 加速函数单元 (AFU) 驱动在 AFU 平台设备创建后被探测。
该模块的主要功能是提供用户空间应用程序访问各个加速器的接口，包括 port 上的
基本复位控制、AFU MMIO 区域导出、DMA 缓冲区映射服务函数等。)

After feature platform devices creation, matched platform drivers will be loaded
automatically to handle different functionalities. Please refer to next sections
for detailed information on functional units which have been already implemented
under this DFL framework.
(特性平台设备创建后，匹配的平台驱动会被自动加载来处理不同的功能。关于此 DFL
框架下已实现的功能单元的详细信息，请参阅后续章节。)


Partial Reconfiguration(部分重配置)
===================================
As mentioned above, accelerators can be reconfigured through partial
reconfiguration of a PR bitstream file. The PR bitstream file must have been
generated for the exact static FPGA region and targeted reconfigurable region
(port) of the FPGA, otherwise, the reconfiguration operation will fail and
possibly cause system instability. This compatibility can be checked by
comparing the compatibility ID noted in the header of PR bitstream file against
the compat_id exposed by the target FPGA region. This check is usually done by
userspace before calling the reconfiguration IOCTL.
(如上所述，加速器可以通过对 PR 比特流文件进行部分重配置来重新配置。PR 比特流
文件必须是为该 FPGA 精确的静态 FPGA 区域和目标可重配置区域 (port) 生成的，
否则重配置操作会失败，并可能造成系统不稳定。这种兼容性可以通过比较 PR 比特流
文件头中注明的兼容性 ID 与目标 FPGA 区域暴露的 compat_id 来检查。此检查通常
由用户空间在调用重配置 IOCTL 之前完成。)


FPGA virtualization - PCIe SRIOV(FPGA 虚拟化 - PCIe SRIOV)
==========================================================
This section describes the virtualization support on DFL based FPGA device to
enable accessing an accelerator from applications running in a virtual machine
(VM). This section only describes the PCIe based FPGA device with SRIOV support.
(本节描述基于 DFL 的 FPGA 设备上的虚拟化支持，使运行在虚拟机 (VM) 中的应用
程序能够访问加速器。本节只描述支持 SRIOV 的基于 PCIe 的 FPGA 设备。)

Features supported by the particular FPGA device are exposed through Device
Feature Lists, as illustrated below:

::

    +-------------------------------+  +-------------+
    |              PF               |  |     VF      |
    +-------------------------------+  +-------------+
        ^            ^         ^              ^
        |            |         |              |
  +-----|------------|---------|--------------|-------+
  |     |            |         |              |       |
  |  +-----+     +-------+ +-------+      +-------+   |
  |  | FME |     | Port0 | | Port1 |      | Port2 |   |
  |  +-----+     +-------+ +-------+      +-------+   |
  |                  ^         ^              ^       |
  |                  |         |              |       |
  |              +-------+ +------+       +-------+   |
  |              |  AFU  | |  AFU |       |  AFU  |   |
  |              +-------+ +------+       +-------+   |
  |                                                   |
  |            DFL based FPGA PCIe Device             |
  +---------------------------------------------------+

(特定 FPGA 设备支持的特性通过设备特性列表暴露，如下图所示：)

FME is always accessed through the physical function (PF).
(FME 始终通过物理功能 (PF) 访问。)

Ports (and related AFUs) are accessed via PF by default, but could be exposed
through virtual function (VF) devices via PCIe SRIOV. Each VF only contains
1 Port and 1 AFU for isolation. Users could assign individual VFs (accelerators)
created via PCIe SRIOV interface, to virtual machines.
(port（及相关 AFU）默认经由 PF 访问，但可以通过 PCIe SRIOV 经由虚拟功能 (VF)
设备暴露。为了隔离，每个 VF 只包含 1 个 Port 和 1 个 AFU。用户可以把通过
PCIe SRIOV 接口创建的各个 VF（加速器）分配给虚拟机。)

The driver organization in virtualization case is illustrated below:
::

    +-------++------++------+             |
    | FME   || FME  || FME  |             |
    | FPGA  || FPGA || FPGA |             |
    |Manager||Bridge||Region|             |
    +-------++------++------+             |
    +-----------------------+  +--------+ |             +--------+
    |          FME          |  |  AFU   | |             |  AFU   |
    |         Module        |  | Module | |             | Module |
    +-----------------------+  +--------+ |             +--------+
          +-----------------------+       |       +-----------------------+
          | FPGA Container Device |       |       | FPGA Container Device |
          |  (FPGA Base Region)   |       |       |  (FPGA Base Region)   |
          +-----------------------+       |       +-----------------------+
            +------------------+          |         +------------------+
            | FPGA PCIE Module |          | Virtual | FPGA PCIE Module |
            +------------------+   Host   | Machine +------------------+
   -------------------------------------- | ------------------------------
             +---------------+            |          +---------------+
             | PCI PF Device |            |          | PCI VF Device |
             +---------------+            |          +---------------+

(虚拟化场景下的驱动组织结构如下图所示：)

FPGA PCIe device driver is always loaded first once an FPGA PCIe PF or VF device
is detected. It:
(一旦检测到 FPGA PCIe PF 或 VF 设备，FPGA PCIe 设备驱动总是最先被加载。它：)

* Finishes enumeration on both FPGA PCIe PF and VF device using common
  interfaces from DFL framework.
  (使用 DFL 框架的通用接口完成对 FPGA PCIe PF 和 VF 设备的枚举。)
* Supports SRIOV.
  (支持 SRIOV。)

The FME device driver plays a management role in this driver architecture, it
provides ioctls to release Port from PF and assign Port to PF. After release
a port from PF, then it's safe to expose this port through a VF via PCIe SRIOV
sysfs interface.
(FME 设备驱动在此驱动架构中扮演管理角色，它提供把 Port 从 PF 释放以及把 Port
分配给 PF 的 ioctl。把 port 从 PF 释放之后，通过 PCIe SRIOV sysfs 接口把这个
port 经由 VF 暴露出去才是安全的。)

To enable accessing an accelerator from applications running in a VM, the
respective AFU's port needs to be assigned to a VF using the following steps:
(要让运行在 VM 中的应用程序能够访问加速器，需要通过以下步骤把相应 AFU 的
port 分配给 VF：)

#. The PF owns all AFU ports by default. Any port that needs to be
   reassigned to a VF must first be released through the
   DFL_FPGA_FME_PORT_RELEASE ioctl on the FME device.
   (默认情况下 PF 拥有所有 AFU port。任何需要重新分配给 VF 的 port，必须先
   通过 FME 设备上的 DFL_FPGA_FME_PORT_RELEASE ioctl 释放。)

#. Once N ports are released from PF, then user can use command below
   to enable SRIOV and VFs. Each VF owns only one Port with AFU.

   ::

      echo N > $PCI_DEVICE_PATH/sriov_numvfs

   (一旦 N 个 port 从 PF 释放，用户就可以使用以下命令启用 SRIOV 和 VF。每个
   VF 只拥有一个带 AFU 的 Port。)

#. Pass through the VFs to VMs
   (把 VF 直通 (pass through) 给 VM)

#. The AFU under VF is accessible from applications in VM (using the
   same driver inside the VF).
   (VF 下的 AFU 可以从 VM 中的应用程序访问（使用 VF 内的同一驱动）。)

Note that an FME can't be assigned to a VF, thus PR and other management
functions are only available via the PF.
(注意，FME 不能被分配给 VF，因此 PR 和其他管理功能只能通过 PF 使用。)

Device enumeration(设备枚举)
============================
This section introduces how applications enumerate the fpga device from
the sysfs hierarchy under /sys/class/fpga_region.
(本节介绍应用程序如何从 /sys/class/fpga_region 下的 sysfs 层级枚举 fpga
设备。)

In the example below, two DFL based FPGA devices are installed in the host. Each
fpga device has one FME and two ports (AFUs).
(在下面的例子中，主机上安装了两个基于 DFL 的 FPGA 设备。每个 fpga 设备有一个
FME 和两个 port（AFU）。)

FPGA regions are created under /sys/class/fpga_region/::

	/sys/class/fpga_region/region0
	/sys/class/fpga_region/region1
	/sys/class/fpga_region/region2
	...

(FPGA region 创建在 /sys/class/fpga_region/ 之下：)

Application needs to search each regionX folder, if feature device is found,
(e.g. "dfl-port.n" or "dfl-fme.m" is found), then it's the base
fpga region which represents the FPGA device.
(应用程序需要搜索每个 regionX 文件夹，如果找到了特性设备（例如发现
"dfl-port.n" 或 "dfl-fme.m"），那么它就是代表该 FPGA 设备的 base fpga
region。)

	/sys/class/fpga_region/region0
	/sys/class/fpga_region/region1
	/sys/class/fpga_region/region2
	...

Application needs to search each regionX folder, if feature device is found,
(e.g. "dfl-port.n" or "dfl-fme.m" is found), then it's the base
fpga region which represents the FPGA device.

Each base region has one FME and two ports (AFUs) as child devices::

	/sys/class/fpga_region/region0/dfl-fme.0
	/sys/class/fpga_region/region0/dfl-port.0
	/sys/class/fpga_region/region0/dfl-port.1
	...

	/sys/class/fpga_region/region3/dfl-fme.1
	/sys/class/fpga_region/region3/dfl-port.2
	/sys/class/fpga_region/region3/dfl-port.3
	...

(每个 base region 都有一个 FME 和两个 port（AFU）作为子设备：)

In general, the FME/AFU sysfs interfaces are named as follows::

	/sys/class/fpga_region/<regionX>/<dfl-fme.n>/
	/sys/class/fpga_region/<regionX>/<dfl-port.m>/

(一般而言，FME/AFU 的 sysfs 接口命名如下：)

with 'n' consecutively numbering all FMEs and 'm' consecutively numbering all
ports.
(其中 'n' 连续编号所有 FME，'m' 连续编号所有 port。)

The device nodes used for ioctl() or mmap() can be referenced through::

	/sys/class/fpga_region/<regionX>/<dfl-fme.n>/dev
	/sys/class/fpga_region/<regionX>/<dfl-port.n>/dev

(用于 ioctl() 或 mmap() 的设备节点可以通过以下路径引用：)


Performance Counters(性能计数器)
================================
Performance reporting is one private feature implemented in FME. It could
supports several independent, system-wide, device counter sets in hardware to
monitor and count for performance events, including "basic", "cache", "fabric",
"vtd" and "vtd_sip" counters. Users could use standard perf tool to monitor
FPGA cache hit/miss rate, transaction number, interface clock counter of AFU
and other FPGA performance events.
(性能报告是 FME 中实现的一个私有特性。它可以在硬件中支持多个独立的、系统级的
设备计数器集合，用于监视和统计性能事件，包括 "basic"、"cache"、"fabric"、
"vtd" 和 "vtd_sip" 计数器。用户可以使用标准 perf 工具监视 FPGA 缓存命中/未
命中率、事务数量、AFU 接口时钟计数器以及其他 FPGA 性能事件。)

Different FPGA devices may have different counter sets, depending on hardware
implementation. E.g., some discrete FPGA cards don't have any cache. User could
use "perf list" to check which perf events are supported by target hardware.
(不同的 FPGA 设备可能拥有不同的计数器集合，取决于硬件实现。例如，一些独立
FPGA 卡没有任何缓存。用户可以使用 "perf list" 检查目标硬件支持哪些 perf
事件。)

In order to allow user to use standard perf API to access these performance
counters, driver creates a perf PMU, and related sysfs interfaces in
/sys/bus/event_source/devices/dfl_fme* to describe available perf events and
configuration options.
(为了让用户能够使用标准 perf API 访问这些性能计数器，驱动创建了一个 perf
PMU，并在 /sys/bus/event_source/devices/dfl_fme* 下创建相关 sysfs 接口，以
描述可用的 perf 事件和配置选项。)

The "format" directory describes the format of the config field of struct
perf_event_attr. There are 3 bitfields for config: "evtype" defines which type
the perf event belongs to; "event" is the identity of the event within its
category; "portid" is introduced to decide counters set to monitor on FPGA
overall data or a specific port.
("format" 目录描述 struct perf_event_attr 的 config 字段的格式。config 有 3
个位域："evtype" 定义 perf 事件属于哪种类型；"event" 是事件在其类别中的标识；
"portid" 用于决定计数器集合监视 FPGA 整体数据还是特定 port。)

The "events" directory describes the configuration templates for all available
events which can be used with perf tool directly. For example, fab_mmio_read
has the configuration "event=0x06,evtype=0x02,portid=0xff", which shows this
event belongs to fabric type (0x02), the local event id is 0x06 and it is for
overall monitoring (portid=0xff).
("events" 目录描述所有可用事件的配置模板，可以直接与 perf 工具一起使用。例如，
fab_mmio_read 的配置为 "event=0x06,evtype=0x02,portid=0xff"，表示该事件属于
fabric 类型 (0x02)，本地事件 ID 为 0x06，且用于整体监视 (portid=0xff)。)

Example usage of perf::

  $# perf list |grep dfl_fme

  dfl_fme0/fab_mmio_read/                              [Kernel PMU event]
  <...>
  dfl_fme0/fab_port_mmio_read,portid=?/                [Kernel PMU event]
  <...>

  $# perf stat -a -e dfl_fme0/fab_mmio_read/ <command>
  or
  $# perf stat -a -e dfl_fme0/event=0x06,evtype=0x02,portid=0xff/ <command>
  or
  $# perf stat -a -e dfl_fme0/config=0xff2006/ <command>

(perf 用法示例：)

Another example, fab_port_mmio_read monitors mmio read of a specific port. So
its configuration template is "event=0x06,evtype=0x01,portid=?". The portid
should be explicitly set.
(另一个例子：fab_port_mmio_read 监视特定 port 的 MMIO 读取。因此它的配置模板
是 "event=0x06,evtype=0x01,portid=?"。portid 应当被显式设置。)

Its usage of perf::

  $# perf stat -a -e dfl_fme0/fab_port_mmio_read,portid=0x0/ <command>
  or
  $# perf stat -a -e dfl_fme0/event=0x06,evtype=0x02,portid=0x0/ <command>
  or
  $# perf stat -a -e dfl_fme0/config=0x2006/ <command>

(其 perf 用法：)

Please note for fabric counters, overall perf events (fab_*) and port perf
events (fab_port_*) actually share one set of counters in hardware, so it can't
monitor both at the same time. If this set of counters is configured to monitor
overall data, then per port perf data is not supported. See below example::

  $# perf stat -e dfl_fme0/fab_mmio_read/,dfl_fme0/fab_port_mmio_write,\
                                                    portid=0/ sleep 1

  Performance counter stats for 'system wide':

                 3      dfl_fme0/fab_mmio_read/
   <not supported>      dfl_fme0/fab_port_mmio_write,portid=0x0/

       1.001750904 seconds time elapsed

(请注意，对于 fabric 计数器，整体 perf 事件 (fab_*) 与 port perf 事件
(fab_port_*) 实际上在硬件中共享同一组计数器，因此无法同时监视两者。如果这组
计数器被配置为监视整体数据，则不支持按 port 的 perf 数据。参见以下示例：)

The driver also provides a "cpumask" sysfs attribute, which contains only one
CPU id used to access these perf events. Counting on multiple CPU is not allowed
since they are system-wide counters on FPGA device.
(驱动还提供一个 "cpumask" sysfs 属性，其中只包含一个用于访问这些 perf 事件的
CPU ID。不允许在多个 CPU 上计数，因为它们是 FPGA 设备上的系统级计数器。)

The current driver does not support sampling. So "perf record" is unsupported.
(当前驱动不支持采样，因此 "perf record" 不受支持。)


Interrupt support(中断支持)
===========================
Some FME and AFU private features are able to generate interrupts. As mentioned
above, users could call ioctl (DFL_FPGA_*_GET_IRQ_NUM) to know whether or how
many interrupts are supported for this private feature. Drivers also implement
an eventfd based interrupt handling mechanism for users to get notified when
interrupt happens. Users could set eventfds to driver via
ioctl (DFL_FPGA_*_SET_IRQ), and then poll/select on these eventfds waiting for
notification.
In Current DFL, 3 sub features (Port error, FME global error and AFU interrupt)
support interrupts.
(一些 FME 和 AFU 私有特性能够产生中断。如上所述，用户可以调用 ioctl
(DFL_FPGA_*_GET_IRQ_NUM) 来了解此私有特性是否支持中断以及支持多少个中断。
驱动还实现了基于 eventfd 的中断处理机制，让用户在中断发生时得到通知。用户
可以通过 ioctl (DFL_FPGA_*_SET_IRQ) 把 eventfd 设置给驱动，然后在这些
eventfd 上 poll/select 等待通知。
在当前 DFL 中，有 3 个子特性（Port 错误、FME 全局错误和 AFU 中断）支持
中断。)


Add new FIUs support(添加新 FIU 支持)
=====================================
It's possible that developers made some new function blocks (FIUs) under this
DFL framework, then new platform device driver needs to be developed for the
new feature dev (FIU) following the same way as existing feature dev drivers
(e.g. FME and Port/AFU platform device driver). Besides that, it requires
modification on DFL framework enumeration code too, for new FIU type detection
and related platform devices creation.
(开发者可能在此 DFL 框架下做出一些新的功能块 (FIU)，此时需要按照与现有特性
设备驱动相同的方式（例如 FME 和 Port/AFU 平台设备驱动），为新特性设备 (FIU)
开发新的平台设备驱动。除此之外，还需要修改 DFL 框架的枚举代码，以支持新 FIU
类型的检测和相关平台设备的创建。)


Add new private features support(添加新私有特性支持)
====================================================
In some cases, we may need to add some new private features to existing FIUs
(e.g. FME or Port). Developers don't need to touch enumeration code in DFL
framework, as each private feature will be parsed automatically and related
mmio resources can be found under FIU platform device created by DFL framework.
Developer only needs to provide a sub feature driver with matched feature id.
FME Partial Reconfiguration Sub Feature driver (see drivers/fpga/dfl-fme-pr.c)
could be a reference.
(某些情况下，我们可能需要向现有 FIU（例如 FME 或 Port）添加新的私有特性。
开发者不需要改动 DFL 框架中的枚举代码，因为每个私有特性都会被自动解析，相关
mmio 资源可以在 DFL 框架创建的 FIU 平台设备下找到。开发者只需要提供具有匹配
特性 ID 的子特性驱动。FME 部分重配置子特性驱动（参见
drivers/fpga/dfl-fme-pr.c）可以作为参考。)

Please refer to below link to existing feature id table and guide for new feature
ids application.
(现有特性 ID 表以及申请新特性 ID 的指南请参阅以下链接。)
https://github.com/OPAE/dfl-feature-id


Location of DFLs on a PCI Device(PCI 设备上 DFL 的位置)
=======================================================
The original method for finding a DFL on a PCI device assumed the start of the
first DFL to offset 0 of bar 0.  If the first node of the DFL is an FME,
then further DFLs in the port(s) are specified in FME header registers.
Alternatively, a PCIe vendor specific capability structure can be used to
specify the location of all the DFLs on the device, providing flexibility
for the type of starting node in the DFL.  Intel has reserved the
VSEC ID of 0x43 for this purpose.  The vendor specific
data begins with a 4 byte vendor specific register for the number of DFLs followed 4 byte
Offset/BIR vendor specific registers for each DFL. Bits 2:0 of Offset/BIR register
indicates the BAR, and bits 31:3 form the 8 byte aligned offset where bits 2:0 are
zero.
(在 PCI 设备上查找 DFL 的原始方法假定第一个 DFL 从 bar 0 的偏移 0 开始。如果
DFL 的第一个节点是 FME，那么 port 中后续的 DFL 在 FME 头寄存器中指定。
另外，也可以使用 PCIe 厂商特定能力 (VSEC) 结构来指定设备上所有 DFL 的位置，
为 DFL 起始节点的类型提供了灵活性。Intel 为此保留了 VSEC ID 0x43。厂商特定
数据以一个 4 字节的厂商特定寄存器开始（表示 DFL 数量），随后是每个 DFL 一个
4 字节的 Offset/BIR 厂商特定寄存器。Offset/BIR 寄存器的位 2:0 指示 BAR，位
31:3 构成 8 字节对齐的偏移（其中位 2:0 为零）。)
::

        +----------------------------+
        |31     Number of DFLS      0|
        +----------------------------+
        |31     Offset     3|2 BIR  0|
        +----------------------------+
                      . . .
        +----------------------------+
        |31     Offset     3|2 BIR  0|
        +----------------------------+

(寄存器布局示意图：)

Being able to specify more than one DFL per BAR has been considered, but it
was determined the use case did not provide value.  Specifying a single DFL
per BAR simplifies the implementation and allows for extra error checking.
(曾经考虑过允许每个 BAR 指定多个 DFL，但最终认定该使用场景没有价值。每个 BAR
只指定一个 DFL 简化了实现，并允许额外的错误检查。)


Userspace driver support for DFL devices(DFL 设备的用户空间驱动支持)
====================================================================
The purpose of an FPGA is to be reprogrammed with newly developed hardware
components. New hardware can instantiate a new private feature in the DFL, and
then present a DFL device in the system. In some cases users may need a
userspace driver for the DFL device:
(FPGA 的目的就是用新开发的硬件组件重新编程。新硬件可以在 DFL 中实例化一个新的
私有特性，然后在系统中呈现一个 DFL 设备。某些情况下，用户可能需要为该 DFL
设备提供用户空间驱动：)

* Users may need to run some diagnostic test for their hardware.
  (用户可能需要为自己的硬件运行一些诊断测试。)
* Users may prototype the kernel driver in user space.
  (用户可能在用户空间对内核驱动做原型验证。)
* Some hardware is designed for specific purposes and does not fit into one of
  the standard kernel subsystems.
  (一些硬件为特定目的设计，不适合归入任何一个标准内核子系统。)

This requires direct access to MMIO space and interrupt handling from
userspace. The uio_dfl module exposes the UIO device interfaces for this
purpose.
(这要求从用户空间直接访问 MMIO 空间并进行中断处理。uio_dfl 模块为此目的暴露
UIO 设备接口。)

Currently the uio_dfl driver only supports the Ether Group sub feature, which
has no irq in hardware. So the interrupt handling is not added in this driver.
(目前 uio_dfl 驱动只支持 Ether Group 子特性，该特性在硬件中没有中断。因此此
驱动中未加入中断处理。)

UIO_DFL should be selected to enable the uio_dfl module driver. To support a
new DFL feature via UIO direct access, its feature id should be added to the
driver's id_table.
(应选择 UIO_DFL 以启用 uio_dfl 模块驱动。要通过 UIO 直接访问支持新的 DFL
特性，应把其特性 ID 加入驱动的 id_table。)


Open discussion(开放问题)
=========================
FME driver exports one ioctl (DFL_FPGA_FME_PORT_PR) for partial reconfiguration
to user now. In the future, if unified user interfaces for reconfiguration are
added, FME driver should switch to them from ioctl interface.
(FME 驱动目前向用户导出一个用于部分重配置的 ioctl
(DFL_FPGA_FME_PORT_PR)。将来，如果加入了统一的重配置用户接口，FME 驱动应从
ioctl 接口切换到它们。)
