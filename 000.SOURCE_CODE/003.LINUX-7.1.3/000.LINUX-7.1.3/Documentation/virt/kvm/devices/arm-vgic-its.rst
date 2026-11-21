.. SPDX-License-Identifier: GPL-2.0

==========================================================================
ARM Virtual Interrupt Translation Service (ITS)(ARM 虚拟中断转换服务(ITS))
==========================================================================

Device types supported:
(支持的设备类型：)

  KVM_DEV_TYPE_ARM_VGIC_ITS    ARM Interrupt Translation Service Controller
  (KVM_DEV_TYPE_ARM_VGIC_ITS    ARM 中断转换服务控制器)

The ITS allows MSI(-X) interrupts to be injected into guests. This extension is
optional.  Creating a virtual ITS controller also requires a host GICv3 (see
arm-vgic-v3.txt), but does not depend on having physical ITS controllers.
(ITS 允许向客户机注入 MSI(-X) 中断。此扩展是可选的。创建虚拟 ITS 控制器还需要宿主机具有 GICv3(参见 arm-vgic-v3.txt)，但不依赖于是否拥有物理 ITS 控制器。)

There can be multiple ITS controllers per guest, each of them has to have
a separate, non-overlapping MMIO region.
(每个客户机可以有多个 ITS 控制器，每个控制器都必须拥有一个独立的、互不重叠的 MMIO 区域。)


Groups(组)
==========

KVM_DEV_ARM_VGIC_GRP_ADDR(原名)
-------------------------------

  Attributes:
  (属性：)

    KVM_VGIC_ITS_ADDR_TYPE (rw, 64-bit)
      Base address in the guest physical address space of the GICv3 ITS
      control register frame.
      This address needs to be 64K aligned and the region covers 128K.
      (GICv3 ITS 控制寄存器帧在客户机物理地址空间中的基地址。该地址需要 64K 对齐，且该区域覆盖 128K。)

  Errors:
  (错误：)

    =======  =================================================
    -E2BIG   Address outside of addressable IPA range (地址超出可寻址的 IPA 范围)
    -EINVAL  Incorrectly aligned address (地址未正确对齐)
    -EEXIST  Address already configured (地址已被配置)
    -EFAULT  Invalid user pointer for attr->addr. (attr->addr 的用户指针无效。)
    -ENODEV  Incorrect attribute or the ITS is not supported. (属性不正确，或不支持该 ITS。)
    =======  =================================================


KVM_DEV_ARM_VGIC_GRP_CTRL(原名)
-------------------------------

  Attributes:
  (属性：)

    KVM_DEV_ARM_VGIC_CTRL_INIT
      request the initialization of the ITS, no additional parameter in
      kvm_device_attr.addr.
      (请求初始化 ITS，kvm_device_attr.addr 中没有附加参数。)

    KVM_DEV_ARM_ITS_CTRL_RESET
      reset the ITS, no additional parameter in kvm_device_attr.addr.
      See "ITS Reset State" section.
      (重置 ITS，kvm_device_attr.addr 中没有附加参数。参见 "ITS Reset State" 一节。)

    KVM_DEV_ARM_ITS_SAVE_TABLES
      save the ITS table data into guest RAM, at the location provisioned
      by the guest in corresponding registers/table entries. Should userspace
      require a form of dirty tracking to identify which pages are modified
      by the saving process, it should use a bitmap even if using another
      mechanism to track the memory dirtied by the vCPUs.
      (把 ITS 表数据保存到客户机 RAM 中，保存位置由客户机在相应寄存器/表条目中预先指定。如果用户空间需要某种脏页跟踪来识别保存过程修改了哪些页，即使已经用其他机制来跟踪被 vCPU 弄脏的内存，它也应使用位图(bitmap)。)

      The layout of the tables in guest memory defines an ABI. The entries
      are laid out in little endian format as described in the last paragraph.
      (表在客户机内存中的布局定义了一个 ABI。表条目按最后一段所述以小端格式布局。)

    KVM_DEV_ARM_ITS_RESTORE_TABLES
      restore the ITS tables from guest RAM to ITS internal structures.
      (把 ITS 表从客户机 RAM 恢复到 ITS 内部结构中。)

      The GICV3 must be restored before the ITS and all ITS registers but
      the GITS_CTLR must be restored before restoring the ITS tables.
      (必须先恢复 GICv3 再恢复 ITS，并且除 GITS_CTLR 之外的所有 ITS 寄存器都必须在恢复 ITS 表之前恢复。)

      The GITS_IIDR read-only register must also be restored before
      calling KVM_DEV_ARM_ITS_RESTORE_TABLES as the IIDR revision field
      encodes the ABI revision.
      (在调用 KVM_DEV_ARM_ITS_RESTORE_TABLES 之前，还必须先恢复只读寄存器 GITS_IIDR，因为 IIDR 的 revision 字段编码了 ABI 版本。)

      The expected ordering when restoring the GICv3/ITS is described in section
      "ITS Restore Sequence".
      (恢复 GICv3/ITS 时的预期顺序在 "ITS Restore Sequence" 一节中描述。)

  Errors:
  (错误：)

    =======  ==========================================================
     -ENXIO  ITS not properly configured as required prior to setting
             this attribute (在设置此属性之前，ITS 未按要求正确配置)
    -ENOMEM  Memory shortage when allocating ITS internal data (分配 ITS 内部数据时内存不足)
    -EINVAL  Inconsistent restored data (恢复的数据不一致)
    -EFAULT  Invalid guest ram access (无效的客户机内存访问)
    -EBUSY   One or more VCPUS are running (一个或多个 vCPU 正在运行)
    -EACCES  The virtual ITS is backed by a physical GICv4 ITS, and the
	     state is not available without GICv4.1 (该虚拟 ITS 由物理 GICv4 ITS 支撑，而在没有 GICv4.1 的情况下该状态不可用)
    =======  ==========================================================

KVM_DEV_ARM_VGIC_GRP_ITS_REGS(原名)
-----------------------------------

  Attributes:
  (属性：)

      The attr field of kvm_device_attr encodes the offset of the
      ITS register, relative to the ITS control frame base address
      (ITS_base).
      (kvm_device_attr 的 attr 字段编码 ITS 寄存器相对于 ITS 控制帧基地址(ITS_base)的偏移量。)

      kvm_device_attr.addr points to a __u64 value whatever the width
      of the addressed register (32/64 bits). 64 bit registers can only
      be accessed with full length.
      (无论所寻址寄存器的宽度如何(32/64 位)，kvm_device_attr.addr 都指向一个 __u64 值。64 位寄存器只能以完整长度访问。)

      Writes to read-only registers are ignored by the kernel except for:
      (对只读寄存器的写操作会被内核忽略，以下情况除外：)

      - GITS_CREADR. It must be restored otherwise commands in the queue
        will be re-executed after restoring CWRITER. GITS_CREADR must be
        restored before restoring the GITS_CTLR which is likely to enable the
        ITS. Also it must be restored after GITS_CBASER since a write to
        GITS_CBASER resets GITS_CREADR.
        (GITS_CREADR。必须恢复它，否则在恢复 CWRITER 之后队列中的命令会被重新执行。GITS_CREADR 必须在恢复 GITS_CTLR(它很可能使能 ITS)之前恢复。另外，它必须在 GITS_CBASER 之后恢复，因为对 GITS_CBASER 的写入会复位 GITS_CREADR。)
      - GITS_IIDR. The Revision field encodes the table layout ABI revision.
        In the future we might implement direct injection of virtual LPIs.
        This will require an upgrade of the table layout and an evolution of
        the ABI. GITS_IIDR must be restored before calling
        KVM_DEV_ARM_ITS_RESTORE_TABLES.
        (GITS_IIDR。Revision 字段编码表布局的 ABI 版本。将来我们可能实现虚拟 LPI 的直接注入，这将需要升级表布局并演进 ABI。在调用 KVM_DEV_ARM_ITS_RESTORE_TABLES 之前必须恢复 GITS_IIDR。)

      For other registers, getting or setting a register has the same
      effect as reading/writing the register on real hardware.
      (对于其他寄存器，读取或设置一个寄存器的效果与在真实硬件上读写该寄存器相同。)

  Errors:
  (错误：)

    =======  ====================================================
    -ENXIO   Offset does not correspond to any supported register (偏移量不对应任何受支持的寄存器)
    -EFAULT  Invalid user pointer for attr->addr (attr->addr 的用户指针无效)
    -EINVAL  Offset is not 64-bit aligned (偏移量未按 64 位对齐)
    -EBUSY   one or more VCPUS are running (一个或多个 vCPU 正在运行)
    =======  ====================================================

ITS Restore Sequence:(ITS 恢复顺序)
------------------------------------

The following ordering must be followed when restoring the GIC, ITS, and
KVM_IRQFD assignments:
(恢复 GIC、ITS 和 KVM_IRQFD 分配时，必须遵循以下顺序：)

a) restore all guest memory and create vcpus
   (恢复所有客户机内存并创建 vCPU。)
b) restore all redistributors
   (恢复所有 redistributor。)
c) provide the ITS base address
   (KVM_DEV_ARM_VGIC_GRP_ADDR)
   (提供 ITS 基地址(KVM_DEV_ARM_VGIC_GRP_ADDR)。)
d) restore the ITS in the following order:
   (按以下顺序恢复 ITS：)

     1. Restore GITS_CBASER
        (恢复 GITS_CBASER)
     2. Restore all other ``GITS_`` registers, except GITS_CTLR!
        (恢复除 ``GITS_`` 之外的所有其他寄存器，GITS_CTLR 除外！)
     3. Load the ITS table data (KVM_DEV_ARM_ITS_RESTORE_TABLES)
        (载入 ITS 表数据(KVM_DEV_ARM_ITS_RESTORE_TABLES)。)
     4. Restore GITS_CTLR
        (恢复 GITS_CTLR)

e) restore KVM_IRQFD assignments for MSIs
   (恢复 MSI 的 KVM_IRQFD 分配。)

Then vcpus can be started.
(之后就可以启动 vCPU 了。)

ITS Table ABI REV0:(ITS 表 ABI REV0)
-------------------------------------

 Revision 0 of the ABI only supports the features of a virtual GICv3, and does
 not support a virtual GICv4 with support for direct injection of virtual
 interrupts for nested hypervisors.
 (ABI 的修订版 0 只支持虚拟 GICv3 的特性，不支持为嵌套 hypervisor 提供虚拟中断直接注入的虚拟 GICv4。)

 The device table and ITT are indexed by the DeviceID and EventID,
 respectively. The collection table is not indexed by CollectionID, and the
 entries in the collection are listed in no particular order.
 All entries are 8 bytes.
 (设备表(device table)与 ITT 分别以 DeviceID 和 EventID 为索引。集合表(collection table)不以 CollectionID 为索引，集合中的条目按任意顺序排列。所有条目均为 8 字节。)

 Device Table Entry (DTE)::

   bits:     | 63| 62 ... 49 | 48 ... 5 | 4 ... 0 |
   values:   | V |   next    | ITT_addr |  Size   |

 (设备表条目(DTE)：)

 where:
 (其中：)

 - V indicates whether the entry is valid. If not, other fields
   are not meaningful.
   (V 表示该条目是否有效。若无效，其他字段均无意义。)
 - next: equals to 0 if this entry is the last one; otherwise it
   corresponds to the DeviceID offset to the next DTE, capped by
   2^14 -1.
   (next：如果此条目是最后一个条目则为 0；否则它对应于到下一个 DTE 的 DeviceID 偏移量，上限为 2^14 -1。)
 - ITT_addr matches bits [51:8] of the ITT address (256 Byte aligned).
   (ITT_addr 对应 ITT 地址的第 [51:8] 位(256 字节对齐)。)
 - Size specifies the supported number of bits for the EventID,
   minus one
   (Size 指定 EventID 支持的位数减一)

 Collection Table Entry (CTE)::

   bits:     | 63| 62 ..  52  | 51 ... 16 | 15  ...   0 |
   values:   | V |    RES0    |  RDBase   |    ICID     |

 (集合表条目(CTE)：)

 where:
 (其中：)

 - V indicates whether the entry is valid. If not, other fields are
   not meaningful.
   (V 表示该条目是否有效。若无效，其他字段均无意义。)
 - RES0: reserved field with Should-Be-Zero-or-Preserved behavior.
   (RES0：保留字段，行为为 Should-Be-Zero-or-Preserved。)
 - RDBase is the PE number (GICR_TYPER.Processor_Number semantic),
   (RDBase 是 PE 编号(GICR_TYPER.Processor_Number 语义)，)
 - ICID is the collection ID
   (ICID 是集合 ID)

 Interrupt Translation Entry (ITE)::

   bits:     | 63 ... 48 | 47 ... 16 | 15 ... 0 |
   values:   |    next   |   pINTID  |  ICID    |

 (中断转换条目(ITE)：)

 where:
 (其中：)

 - next: equals to 0 if this entry is the last one; otherwise it corresponds
   to the EventID offset to the next ITE capped by 2^16 -1.
   (next：如果此条目是最后一个条目则为 0；否则它对应于到下一个 ITE 的 EventID 偏移量，上限为 2^16 -1。)
 - pINTID is the physical LPI ID; if zero, it means the entry is not valid
   and other fields are not meaningful.
   (pINTID 是物理 LPI ID；若为零，表示该条目无效，其他字段均无意义。)
 - ICID is the collection ID
   (ICID 是集合 ID)

ITS Reset State:(ITS 重置状态)
-------------------------------

RESET returns the ITS to the same state that it was when first created and
initialized. When the RESET command returns, the following things are
guaranteed:
(RESET 把 ITS 恢复到它最初创建并初始化时的状态。当 RESET 命令返回时，可以保证以下几点：)

- The ITS is not enabled and quiescent
  GITS_CTLR.Enabled = 0 .Quiescent=1
  (ITS 未使能且处于静默(quiescent)状态：GITS_CTLR.Enabled = 0、Quiescent = 1)
- There is no internally cached state
  (没有内部缓存的状态)
- No collection or device table are used
  GITS_BASER<n>.Valid = 0
  (没有使用任何集合表或设备表：GITS_BASER<n>.Valid = 0)
- GITS_CBASER = 0, GITS_CREADR = 0, GITS_CWRITER = 0
  (GITS_CBASER = 0、GITS_CREADR = 0、GITS_CWRITER = 0)
- The ABI version is unchanged and remains the one set when the ITS
  device was first created.
  (ABI 版本不变，仍保持 ITS 设备最初创建时设定的版本。)
