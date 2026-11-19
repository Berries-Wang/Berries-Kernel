.. SPDX-License-Identifier: GPL-2.0

=============================================================================================================
ARM Virtual Generic Interrupt Controller v3 and later (VGICv3)(ARM 虚拟通用中断控制器 v3 及更高版本 (VGICv3))
=============================================================================================================


Device types supported:
(支持的设备类型：)
  - KVM_DEV_TYPE_ARM_VGIC_V3     ARM Generic Interrupt Controller v3.0
    (KVM_DEV_TYPE_ARM_VGIC_V3: ARM 通用中断控制器 v3.0)

Only one VGIC instance may be instantiated through this API.  The created VGIC
will act as the VM interrupt controller, requiring emulated user-space devices
to inject interrupts to the VGIC instead of directly to CPUs.  It is not
possible to create both a GICv3 and GICv2 on the same VM.
(通过此 API 只能实例化一个 VGIC 实例。创建的 VGIC 将充当 VM 的中断控制
器，需要模拟的用户空间设备把中断注入到 VGIC 而不是直接注入到 CPU。不能
在同一个 VM 上同时创建 GICv3 和 GICv2。)

Creating a guest GICv3 device requires a host GICv3 host, or a GICv5 host with
support for FEAT_GCIE_LEGACY.
(创建客户 GICv3 设备需要主机具有 GICv3，或主机为支持 FEAT_GCIE_LEGACY
的 GICv5。)


Groups:
(组别：)
  KVM_DEV_ARM_VGIC_GRP_ADDR
   Attributes:
   (属性：)

    KVM_VGIC_V3_ADDR_TYPE_DIST (rw, 64-bit)
      Base address in the guest physical address space of the GICv3 distributor
      register mappings. Only valid for KVM_DEV_TYPE_ARM_VGIC_V3.
      This address needs to be 64K aligned and the region covers 64 KByte.
      (GICv3 distributor 寄存器映射在客户物理地址空间中的基地址。仅对
      KVM_DEV_TYPE_ARM_VGIC_V3 有效。该地址需要 64K 对齐，且区域覆盖
      64 KByte。)

    KVM_VGIC_V3_ADDR_TYPE_REDIST (rw, 64-bit)
      Base address in the guest physical address space of the GICv3
      redistributor register mappings. There are two 64K pages for each
      VCPU and all of the redistributor pages are contiguous.
      Only valid for KVM_DEV_TYPE_ARM_VGIC_V3.
      This address needs to be 64K aligned.
      (GICv3 redistributor 寄存器映射在客户物理地址空间中的基地址。每个
      VCPU 有两个 64K 页面，所有 redistributor 页面连续排列。仅对
      KVM_DEV_TYPE_ARM_VGIC_V3 有效。该地址需要 64K 对齐。)

    KVM_VGIC_V3_ADDR_TYPE_REDIST_REGION (rw, 64-bit)
      The attribute data pointed to by kvm_device_attr.addr is a __u64 value::

        bits:     | 63   ....  52  |  51   ....   16 | 15 - 12  |11 - 0
        values:   |     count      |       base      |  flags   | index

      (kvm_device_attr.addr 所指向的属性数据是一个 __u64 值，各字段为：)

      - index encodes the unique redistributor region index (index 编码
        redistributor 区域的唯一索引)
      - flags: reserved for future use, currently 0 (flags: 保留供将来使用，
        当前为 0)
      - base field encodes bits [51:16] of the guest physical base address
        of the first redistributor in the region.
        (base 字段编码该区域中第一个 redistributor 的客户物理基地址的位
        [51:16]。)
      - count encodes the number of redistributors in the region. Must be
        greater than 0.
        (count 编码该区域中 redistributor 的数量。必须大于 0。)

      There are two 64K pages for each redistributor in the region and
      redistributors are laid out contiguously within the region. Regions
      are filled with redistributors in the index order. The sum of all
      region count fields must be greater than or equal to the number of
      VCPUs. Redistributor regions must be registered in the incremental
      index order, starting from index 0.
      (区域中每个 redistributor 有两个 64K 页面，redistributor 在区域内
      连续排列。区域按索引顺序用 redistributor 填充。所有区域的 count
      字段之和必须大于或等于 VCPU 的数量。Redistributor 区域必须按递增
      的索引顺序注册，从索引 0 开始。)

      The characteristics of a specific redistributor region can be read
      by presetting the index field in the attr data.
      Only valid for KVM_DEV_TYPE_ARM_VGIC_V3.
      (通过预先在 attr 数据中设置 index 字段，可以读取特定 redistributor
      区域的特征。仅对 KVM_DEV_TYPE_ARM_VGIC_V3 有效。)

  It is invalid to mix calls with KVM_VGIC_V3_ADDR_TYPE_REDIST and
  KVM_VGIC_V3_ADDR_TYPE_REDIST_REGION attributes.
  (把 KVM_VGIC_V3_ADDR_TYPE_REDIST 与 KVM_VGIC_V3_ADDR_TYPE_REDIST_REGION
  属性的调用混用是无效的。)

  Note that to obtain reproducible results (the same VCPU being associated
  with the same redistributor across a save/restore operation), VCPU creation
  order, redistributor region creation order as well as the respective
  interleaves of VCPU and region creation MUST be preserved.  Any change in
  either ordering may result in a different vcpu_id/redistributor association,
  resulting in a VM that will fail to run at restore time.
  (注意，要获得可复现的结果 (在保存/恢复操作中同一个 VCPU 关联同一个
  redistributor)，必须保持 VCPU 创建顺序、redistributor 区域创建顺序以及
  VCPU 与区域创建的相应交错顺序。任何顺序上的改变都可能导致不同的
  vcpu_id/redistributor 关联，产生恢复时无法运行的 VM。)

  Errors:
  (错误：)

    =======  =============================================================
    -E2BIG   Address outside of addressable IPA range (地址超出可寻址 IPA 范围)
    -EINVAL  Incorrectly aligned address, bad redistributor region
             count/index, mixed redistributor region attribute usage (地址未正确对齐、redistributor 区域 count/index 错误、混用 redistributor 区域属性)
    -EEXIST  Address already configured (地址已被配置)
    -ENOENT  Attempt to read the characteristics of a non existing
             redistributor region (试图读取不存在的 redistributor 区域的特征)
    -ENXIO   The group or attribute is unknown/unsupported for this device
             or hardware support is missing. (此设备未知/不支持该组或属性，或缺少硬件支持。)
    -EFAULT  Invalid user pointer for attr->addr. (attr->addr 的用户指针无效。)
    -EBUSY   Attempt to write a register that is read-only after
             initialization (试图写入初始化后只读的寄存器)
    =======  =============================================================


  KVM_DEV_ARM_VGIC_GRP_DIST_REGS, KVM_DEV_ARM_VGIC_GRP_REDIST_REGS
   Attributes:
   (属性：)

    The attr field of kvm_device_attr encodes two values::

      bits:     | 63   ....  32  |  31   ....    0 |
      values:   |      mpidr     |      offset     |

    (kvm_device_attr 的 attr 字段编码了两个值：)

    All distributor regs are (rw, 32-bit) and kvm_device_attr.addr points to a
    __u32 value.  64-bit registers must be accessed by separately accessing the
    lower and higher word.
    (所有 distributor 寄存器都是 (rw, 32-bit)，kvm_device_attr.addr 指向一个
    __u32 值。64 位寄存器必须通过分别访问低字和高字来存取。)

    Writes to read-only registers are ignored by the kernel.
    (对只读寄存器的写入会被内核忽略。)

    KVM_DEV_ARM_VGIC_GRP_DIST_REGS accesses the main distributor registers.
    KVM_DEV_ARM_VGIC_GRP_REDIST_REGS accesses the redistributor of the CPU
    specified by the mpidr.
    (KVM_DEV_ARM_VGIC_GRP_DIST_REGS 访问主 distributor 寄存器。
    KVM_DEV_ARM_VGIC_GRP_REDIST_REGS 访问 mpidr 所指定 CPU 的
    redistributor。)

    The offset is relative to the "[Re]Distributor base address" as defined
    in the GICv3/4 specs.  Getting or setting such a register has the same
    effect as reading or writing the register on real hardware, except for the
    following registers: GICD_STATUSR, GICR_STATUSR, GICD_ISPENDR,
    GICR_ISPENDR0, GICD_ICPENDR, and GICR_ICPENDR0.  These registers behave
    differently when accessed via this interface compared to their
    architecturally defined behavior to allow software a full view of the
    VGIC's internal state.
    (该偏移相对于 GICv3/4 规范中定义的 "[Re]Distributor base address"。读取
    或设置这样的寄存器与在真实硬件上读/写该寄存器具有相同的效果，但以下
    寄存器除外：GICD_STATUSR、GICR_STATUSR、GICD_ISPENDR、GICR_ISPENDR0、
    GICD_ICPENDR 和 GICR_ICPENDR0。与架构定义的行为相比，这些寄存器经由
    此接口访问时表现不同，以便软件能完整查看 VGIC 的内部状态。)

    The mpidr field is used to specify which
    redistributor is accessed.  The mpidr is ignored for the distributor.
    (mpidr 字段用于指定访问哪个 redistributor。对 distributor 而言 mpidr
    被忽略。)

    The mpidr encoding is based on the affinity information in the
    architecture defined MPIDR, and the field is encoded as follows::

      | 63 .... 56 | 55 .... 48 | 47 .... 40 | 39 .... 32 |
      |    Aff3    |    Aff2    |    Aff1    |    Aff0    |

    (mpidr 编码基于架构定义的 MPIDR 中的亲和性信息，字段编码如下：)

    Note that distributor fields are not banked, but return the same value
    regardless of the mpidr used to access the register.
    (注意 distributor 字段不是按 CPU 分组 (banked) 的，无论使用哪个 mpidr
    访问该寄存器都返回相同的值。)

    Userspace is allowed to write the following register fields prior to
    initialization of the VGIC:
    (允许用户空间在 VGIC 初始化之前写入以下寄存器字段：)

      * GICD_IIDR.Revision
      * GICD_TYPER2.nASSGIcap

    GICD_IIDR.Revision is updated when the KVM implementation is changed in a
    way directly observable by the guest or userspace.  Userspace should read
    GICD_IIDR from KVM and write back the read value to confirm its expected
    behavior is aligned with the KVM implementation.  Userspace should set
    GICD_IIDR before setting any other registers to ensure the expected
    behavior.
    (当 KVM 实现发生客户或用户空间可直接观察到的变化时，GICD_IIDR.Revision
    会更新。用户空间应从 KVM 读取 GICD_IIDR 并把读到的值写回，以确认其
    预期行为与 KVM 实现一致。用户空间应在设置任何其他寄存器之前先设置
    GICD_IIDR，以确保预期行为。)


    GICD_TYPER2.nASSGIcap allows userspace to control the support of SGIs
    without an active state. At VGIC creation the field resets to the
    maximum capability of the system. Userspace is expected to read the field
    to determine the supported value(s) before writing to the field.
    (GICD_TYPER2.nASSGIcap 允许用户空间控制对无活跃状态 SGI 的支持。在
    VGIC 创建时该字段复位为系统的最大能力。用户空间在写入该字段之前应
    先读取它，以确定支持的取值。)


    The GICD_STATUSR and GICR_STATUSR registers are architecturally defined such
    that a write of a clear bit has no effect, whereas a write with a set bit
    clears that value.  To allow userspace to freely set the values of these two
    registers, setting the attributes with the register offsets for these two
    registers simply sets the non-reserved bits to the value written.
    (GICD_STATUSR 和 GICR_STATUSR 寄存器在架构上的定义是：写入清零的位
    无效果，而写入置位的位会清除该值。为了让用户空间能自由设置这两个
    寄存器的值，使用这两个寄存器的偏移来设置属性时，只是把非保留位设置
    成写入的值。)


    Accesses (reads and writes) to the GICD_ISPENDR register region and
    GICR_ISPENDR0 registers get/set the value of the latched pending state for
    the interrupts.
    (对 GICD_ISPENDR 寄存器区域和 GICR_ISPENDR0 寄存器的访问 (读和写) 会
    获取/设置中断的锁存挂起状态 (latched pending state) 的值。)

    This is identical to the value returned by a guest read from ISPENDR for an
    edge triggered interrupt, but may differ for level triggered interrupts.
    For edge triggered interrupts, once an interrupt becomes pending (whether
    because of an edge detected on the input line or because of a guest write
    to ISPENDR) this state is "latched", and only cleared when either the
    interrupt is activated or when the guest writes to ICPENDR. A level
    triggered interrupt may be pending either because the level input is held
    high by a device, or because of a guest write to the ISPENDR register. Only
    ISPENDR writes are latched; if the device lowers the line level then the
    interrupt is no longer pending unless the guest also wrote to ISPENDR, and
    conversely writes to ICPENDR or activations of the interrupt do not clear
    the pending status if the line level is still being held high.  (These
    rules are documented in the GICv3 specification descriptions of the ICPENDR
    and ISPENDR registers.) For a level triggered interrupt the value accessed
    here is that of the latch which is set by ISPENDR and cleared by ICPENDR or
    interrupt activation, whereas the value returned by a guest read from
    ISPENDR is the logical OR of the latch value and the input line level.
    (这与客户从 ISPENDR 读取边沿触发中断所返回的值相同，但对于电平触发
    中断可能不同。对于边沿触发中断，一旦中断变为挂起 (无论是因为在输入
    线上检测到边沿，还是因为客户写入 ISPENDR)，该状态就被"锁存"，只有在
    中断被激活或客户写入 ICPENDR 时才清除。电平触发中断可能因设备保持
    输入线为高电平而挂起，也可能因客户写入 ISPENDR 寄存器而挂起。只有
    ISPENDR 写入被锁存；如果设备拉低线电平，中断就不再挂起，除非客户也
    写入了 ISPENDR；反过来，如果线电平仍被保持为高，对 ICPENDR 的写入或
    中断的激活也不会清除挂起状态。(这些规则记录在 GICv3 规范对 ICPENDR
    和 ISPENDR 寄存器的描述中。) 对于电平触发中断，此处访问的值是由
    ISPENDR 置位、由 ICPENDR 或中断激活清除的锁存器的值，而客户从
    ISPENDR 读取的值是锁存器值与输入线电平的逻辑或。)

    Raw access to the latch state is provided to userspace so that it can save
    and restore the entire GIC internal state (which is defined by the
    combination of the current input line level and the latch state, and cannot
    be deduced from purely the line level and the value of the ISPENDR
    registers).
    (向用户空间提供对锁存状态的原始访问，是为了让它能保存和恢复完整的
    GIC 内部状态 (该状态由当前输入线电平与锁存状态的组合定义，无法仅凭
    线电平和 ISPENDR 寄存器的值推断出来)。)

    Accesses to GICD_ICPENDR register region and GICR_ICPENDR0 registers have
    RAZ/WI semantics, meaning that reads always return 0 and writes are always
    ignored.
    (对 GICD_ICPENDR 寄存器区域和 GICR_ICPENDR0 寄存器的访问具有 RAZ/WI
    语义，即读取总是返回 0，写入总是被忽略。)

  Errors:
  (错误：)

    ======  =====================================================
    -ENXIO  Getting or setting this register is not yet supported (尚不支持读取或设置该寄存器)
    -EBUSY  One or more VCPUs are running (一个或多个 VCPU 正在运行)
    ======  =====================================================


  KVM_DEV_ARM_VGIC_GRP_CPU_SYSREGS
   Attributes:
   (属性：)

    The attr field of kvm_device_attr encodes two values::

      bits:     | 63      ....       32 | 31  ....  16 | 15  ....  0 |
      values:   |         mpidr         |      RES     |    instr    |

    (kvm_device_attr 的 attr 字段编码了两个值：)

    The mpidr field encodes the CPU ID based on the affinity information in the
    architecture defined MPIDR, and the field is encoded as follows::

      | 63 .... 56 | 55 .... 48 | 47 .... 40 | 39 .... 32 |
      |    Aff3    |    Aff2    |    Aff1    |    Aff0    |

    (mpidr 字段基于架构定义的 MPIDR 中的亲和性信息编码 CPU ID，字段编码
    如下：)

    The instr field encodes the system register to access based on the fields
    defined in the A64 instruction set encoding for system register access
    (RES means the bits are reserved for future use and should be zero)::

      | 15 ... 14 | 13 ... 11 | 10 ... 7 | 6 ... 3 | 2 ... 0 |
      |   Op 0    |    Op1    |    CRn   |   CRm   |   Op2   |

    (instr 字段基于 A64 指令集编码中为系统寄存器访问定义的字段来编码要
    访问的系统寄存器 (RES 表示这些位保留供将来使用，应为零)：)

    All system regs accessed through this API are (rw, 64-bit) and
    kvm_device_attr.addr points to a __u64 value.
    (通过此 API 访问的所有系统寄存器都是 (rw, 64-bit)，kvm_device_attr.addr
    指向一个 __u64 值。)

    KVM_DEV_ARM_VGIC_GRP_CPU_SYSREGS accesses the CPU interface registers for the
    CPU specified by the mpidr field.
    (KVM_DEV_ARM_VGIC_GRP_CPU_SYSREGS 访问 mpidr 字段所指定 CPU 的 CPU
    接口寄存器。)

    The available registers are:
    (可用寄存器如下：)

    ===============  ====================================================
    ICC_PMR_EL1
    ICC_BPR0_EL1
    ICC_AP0R0_EL1
    ICC_AP0R1_EL1    when the host implements at least 6 bits of priority (当主机实现至少 6 位优先级时)
    ICC_AP0R2_EL1    when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICC_AP0R3_EL1    when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICC_AP1R0_EL1
    ICC_AP1R1_EL1    when the host implements at least 6 bits of priority (当主机实现至少 6 位优先级时)
    ICC_AP1R2_EL1    when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICC_AP1R3_EL1    when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICC_BPR1_EL1
    ICC_CTLR_EL1
    ICC_SRE_EL1
    ICC_IGRPEN0_EL1
    ICC_IGRPEN1_EL1
    ===============  ====================================================

    When EL2 is available for the guest, these registers are also available:
    (当客户可以使用 EL2 时，以下寄存器也可用：)

    =============  ====================================================
    ICH_AP0R0_EL2
    ICH_AP0R1_EL2  when the host implements at least 6 bits of priority (当主机实现至少 6 位优先级时)
    ICH_AP0R2_EL2  when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICH_AP0R3_EL2  when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICH_AP1R0_EL2
    ICH_AP1R1_EL2  when the host implements at least 6 bits of priority (当主机实现至少 6 位优先级时)
    ICH_AP1R2_EL2  when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICH_AP1R3_EL2  when the host implements 7 bits of priority (当主机实现 7 位优先级时)
    ICH_HCR_EL2
    ICC_SRE_EL2
    ICH_VTR_EL2
    ICH_VMCR_EL2
    ICH_LR0_EL2
    ICH_LR1_EL2
    ICH_LR2_EL2
    ICH_LR3_EL2
    ICH_LR4_EL2
    ICH_LR5_EL2
    ICH_LR6_EL2
    ICH_LR7_EL2
    ICH_LR8_EL2
    ICH_LR9_EL2
    ICH_LR10_EL2
    ICH_LR11_EL2
    ICH_LR12_EL2
    ICH_LR13_EL2
    ICH_LR14_EL2
    ICH_LR15_EL2
    =============  ====================================================

    CPU interface registers are only described using the AArch64
    encoding.
    (CPU 接口寄存器仅以 AArch64 编码描述。)

  Errors:
  (错误：)

    =======  =================================================
    -ENXIO   Getting or setting this register is not supported (不支持读取或设置该寄存器)
    -EBUSY   VCPU is running (VCPU 正在运行)
    -EINVAL  Invalid mpidr or register value supplied (提供的 mpidr 或寄存器值无效)
    =======  =================================================


  KVM_DEV_ARM_VGIC_GRP_NR_IRQS
   Attributes:
   (属性：)

    A value describing the number of interrupts (SGI, PPI and SPI) for
    this GIC instance, ranging from 64 to 1024, in increments of 32.
    (一个描述此 GIC 实例中断数量 (SGI、PPI 和 SPI) 的值，范围从 64 到
    1024，以 32 为步长递增。)

    kvm_device_attr.addr points to a __u32 value.
    (kvm_device_attr.addr 指向一个 __u32 值。)

  Errors:
  (错误：)

    =======  ======================================
    -EINVAL  Value set is out of the expected range (设置的值超出预期范围)
    -EBUSY   Value has already be set. (值已被设置。)
    =======  ======================================


  KVM_DEV_ARM_VGIC_GRP_CTRL
   Attributes:
   (属性：)

    KVM_DEV_ARM_VGIC_CTRL_INIT
      request the initialization of the VGIC, no additional parameter in
      kvm_device_attr.addr. Must be called after all VCPUs have been created.
      (请求 VGIC 的初始化，kvm_device_attr.addr 中没有额外参数。必须在
      所有 VCPU 都已创建之后调用。)
    KVM_DEV_ARM_VGIC_SAVE_PENDING_TABLES
      save all LPI pending bits into guest RAM pending tables.
      (把所有 LPI 挂起位保存到客户 RAM 的挂起表中。)

      The first kB of the pending table is not altered by this operation.
      (挂起表的第一个 kB 不会被此操作改动。)

  Errors:
  (错误：)

    =======  ========================================================
    -ENXIO   VGIC not properly configured as required prior to calling
             this attribute (调用此属性前 VGIC 未按要求正确配置)
    -ENODEV  no online VCPU (没有在线 VCPU)
    -ENOMEM  memory shortage when allocating vgic internal data (分配 vgic 内部数据时内存不足)
    -EFAULT  Invalid guest ram access (无效的客户 RAM 访问)
    -EBUSY   One or more VCPUS are running (一个或多个 VCPU 正在运行)
    =======  ========================================================


  KVM_DEV_ARM_VGIC_GRP_LEVEL_INFO
   Attributes:
   (属性：)

    The attr field of kvm_device_attr encodes the following values::

      bits:     | 63      ....       32 | 31   ....    10 | 9  ....  0 |
      values:   |         mpidr         |      info       |   vINTID   |

    (kvm_device_attr 的 attr 字段编码了以下值：)

    The vINTID specifies which set of IRQs is reported on.
    (vINTID 指定报告的是哪一组 IRQ。)

    The info field specifies which information userspace wants to get or set
    using this interface.  Currently we support the following info values:
    (info 字段指定用户空间想通过此接口获取或设置的信息。目前我们支持
    以下 info 取值：)

      VGIC_LEVEL_INFO_LINE_LEVEL:
	Get/Set the input level of the IRQ line for a set of 32 contiguously
	numbered interrupts.
	(获取/设置一组 32 个连续编号中断的 IRQ 线输入电平。)

	vINTID must be a multiple of 32.
	(vINTID 必须是 32 的倍数。)

	kvm_device_attr.addr points to a __u32 value which will contain a
	bitmap where a set bit means the interrupt level is asserted.
	(kvm_device_attr.addr 指向一个 __u32 值，其中包含一个位图，置位的
	位表示中断电平已声明 (asserted)。)

	Bit[n] indicates the status for interrupt vINTID + n.
	(Bit[n] 表示中断 vINTID + n 的状态。)

    SGIs and any interrupt with a higher ID than the number of interrupts
    supported, will be RAZ/WI.  LPIs are always edge-triggered and are
    therefore not supported by this interface.
    (SGI 以及 ID 高于所支持中断数的任何中断都是 RAZ/WI。LPI 总是边沿
    触发的，因此不受此接口支持。)

    PPIs are reported per VCPU as specified in the mpidr field, and SPIs are
    reported with the same value regardless of the mpidr specified.
    (PPI 按 mpidr 字段的指定按 VCPU 报告，而 SPI 无论指定哪个 mpidr 都
    以相同值报告。)

    The mpidr field encodes the CPU ID based on the affinity information in the
    architecture defined MPIDR, and the field is encoded as follows::

      | 63 .... 56 | 55 .... 48 | 47 .... 40 | 39 .... 32 |
      |    Aff3    |    Aff2    |    Aff1    |    Aff0    |

    (mpidr 字段基于架构定义的 MPIDR 中的亲和性信息编码 CPU ID，字段编码
    如下：)

  Errors:
  (错误：)
    =======  =============================================
    -EINVAL  vINTID is not multiple of 32 or info field is
	     not VGIC_LEVEL_INFO_LINE_LEVEL (vINTID 不是 32 的倍数，或 info 字段不是 VGIC_LEVEL_INFO_LINE_LEVEL)
    =======  =============================================

  KVM_DEV_ARM_VGIC_GRP_MAINT_IRQ
   Attributes:
   (属性：)

    The attr field of kvm_device_attr encodes the following values:
    (kvm_device_attr 的 attr 字段编码了以下值：)

      bits:     | 31   ....    5 | 4  ....  0 |
      values:   |      RES0      |   vINTID   |

    The vINTID specifies which interrupt is generated when the vGIC
    must generate a maintenance interrupt. This must be a PPI.
    (vINTID 指定当 vGIC 必须产生维护中断 (maintenance interrupt) 时生成的
    是哪个中断。这必须是 PPI。)
