.. SPDX-License-Identifier: GPL-2.0

====================================================================================
ARM Virtual Generic Interrupt Controller v2 (VGIC)(ARM 虚拟通用中断控制器 v2 (VGIC))
====================================================================================

Device types supported:
(支持的设备类型：)

  - KVM_DEV_TYPE_ARM_VGIC_V2     ARM Generic Interrupt Controller v2.0
    (KVM_DEV_TYPE_ARM_VGIC_V2     ARM 通用中断控制器 v2.0)

Only one VGIC instance may be instantiated through either this API or the
legacy KVM_CREATE_IRQCHIP API.  The created VGIC will act as the VM interrupt
controller, requiring emulated user-space devices to inject interrupts to the
VGIC instead of directly to CPUs.
(只能通过本 API 或旧式的 KVM_CREATE_IRQCHIP API 实例化一个 VGIC 实例。创建的
VGIC 将充当 VM 的中断控制器，要求模拟的用户空间设备向 VGIC 而不是直接向 CPU
注入中断。)

GICv3 implementations with hardware compatibility support allow creating a
guest GICv2 through this interface.  For information on creating a guest GICv3
device and guest ITS devices, see arm-vgic-v3.txt.  It is not possible to
create both a GICv3 and GICv2 device on the same VM.
(具有硬件兼容性支持的 GICv3 实现允许通过此接口创建客户机 GICv2。有关创建
客户机 GICv3 设备和客户机 ITS 设备的信息，参见 arm-vgic-v3.txt。无法在同一
VM 上同时创建 GICv3 与 GICv2 设备。)


Groups:
(组：)
  KVM_DEV_ARM_VGIC_GRP_ADDR
   Attributes:
   (属性：)

    KVM_VGIC_V2_ADDR_TYPE_DIST (rw, 64-bit)
      Base address in the guest physical address space of the GIC distributor
      register mappings. Only valid for KVM_DEV_TYPE_ARM_VGIC_V2.
      This address needs to be 4K aligned and the region covers 4 KByte.
      (GIC 分布器寄存器映射在客户机物理地址空间中的基地址。仅对
      KVM_DEV_TYPE_ARM_VGIC_V2 有效。该地址需要 4K 对齐，且该区域覆盖
      4 KByte。)

    KVM_VGIC_V2_ADDR_TYPE_CPU (rw, 64-bit)
      Base address in the guest physical address space of the GIC virtual cpu
      interface register mappings. Only valid for KVM_DEV_TYPE_ARM_VGIC_V2.
      This address needs to be 4K aligned and the region covers 8 KByte.
      (GIC 虚拟 CPU 接口寄存器映射在客户机物理地址空间中的基地址。仅对
      KVM_DEV_TYPE_ARM_VGIC_V2 有效。该地址需要 4K 对齐，且该区域覆盖
      8 KByte。)

  Errors:
  (错误：)

    =======  =============================================================
    -E2BIG   Address outside of addressable IPA range (地址超出可寻址 IPA 范围)
    -EINVAL  Incorrectly aligned address (地址未正确对齐)
    -EEXIST  Address already configured (地址已被配置)
    -ENXIO   The group or attribute is unknown/unsupported for this device
             or hardware support is missing. (此设备不支持未知的组或属性，或缺少硬件支持。)
    -EFAULT  Invalid user pointer for attr->addr. (attr->addr 的用户指针无效。)
    =======  =============================================================

  KVM_DEV_ARM_VGIC_GRP_DIST_REGS
   Attributes:

    The attr field of kvm_device_attr encodes two values::

      bits:     | 63   ....  40 | 39 ..  32  |  31   ....    0 |
      values:   |    reserved   | vcpu_index |      offset     |

    (kvm_device_attr 的 attr 字段编码了两个值：)

    All distributor regs are (rw, 32-bit)
    (所有分布器寄存器均为 (rw, 32-bit)。)

    The offset is relative to the "Distributor base address" as defined in the
    GICv2 specs.  Getting or setting such a register has the same effect as
    reading or writing the register on the actual hardware from the cpu whose
    index is specified with the vcpu_index field.  Note that most distributor
    fields are not banked, but return the same value regardless of the
    vcpu_index used to access the register.
    (偏移量相对于 GICv2 规范中定义的 "Distributor base address"（分布器基地址）。
    获取或设置这样的寄存器，与在以 vcpu_index 字段指定索引的 CPU 上读写实际
    硬件中的该寄存器效果相同。注意，大多数分布器字段不是按 CPU 分组存储
    (banked) 的，无论使用哪个 vcpu_index 访问该寄存器，返回的都是相同的值。)

    GICD_IIDR.Revision is updated when the KVM implementation of an emulated
    GICv2 is changed in a way directly observable by the guest or userspace.
    Userspace should read GICD_IIDR from KVM and write back the read value to
    confirm its expected behavior is aligned with the KVM implementation.
    Userspace should set GICD_IIDR before setting any other registers (both
    KVM_DEV_ARM_VGIC_GRP_DIST_REGS and KVM_DEV_ARM_VGIC_GRP_CPU_REGS) to ensure
    the expected behavior. Unless GICD_IIDR has been set from userspace, writes
    to the interrupt group registers (GICD_IGROUPR) are ignored.
    (当模拟 GICv2 的 KVM 实现以客户机或用户空间可直接观察到的方式发生变化时，
    GICD_IIDR.Revision 会更新。用户空间应从 KVM 读取 GICD_IIDR 并把读到的值
    写回，以确认其预期行为与 KVM 实现一致。为确保预期行为，用户空间应在设置
    任何其他寄存器（包括 KVM_DEV_ARM_VGIC_GRP_DIST_REGS 和
    KVM_DEV_ARM_VGIC_GRP_CPU_REGS）之前先设置 GICD_IIDR。除非 GICD_IIDR 已由
    用户空间设置，否则对中断组寄存器（GICD_IGROUPR）的写入将被忽略。)

  Errors:
  (错误：)

    =======  =====================================================
    -ENXIO   Getting or setting this register is not yet supported (尚不支持获取或设置此寄存器)
    -EBUSY   One or more VCPUs are running (一个或多个 VCPU 正在运行)
    -EINVAL  Invalid vcpu_index supplied (提供的 vcpu_index 无效)
    =======  =====================================================

  KVM_DEV_ARM_VGIC_GRP_CPU_REGS
   Attributes:
   (属性：)

    The attr field of kvm_device_attr encodes two values::

      bits:     | 63   ....  40 | 39 ..  32  |  31   ....    0 |
      values:   |    reserved   | vcpu_index |      offset     |

    (kvm_device_attr 的 attr 字段编码了两个值：)

    All CPU interface regs are (rw, 32-bit)
    (所有 CPU 接口寄存器均为 (rw, 32-bit)。)

    The offset specifies the offset from the "CPU interface base address" as
    defined in the GICv2 specs.  Getting or setting such a register has the
    same effect as reading or writing the register on the actual hardware.
    (偏移量指定相对于 GICv2 规范中定义的 "CPU interface base address"（CPU
    接口基地址）的偏移。获取或设置这样的寄存器，与在实际硬件上读写该寄存器
    效果相同。)

    The Active Priorities Registers APRn are implementation defined, so we set a
    fixed format for our implementation that fits with the model of a "GICv2
    implementation without the security extensions" which we present to the
    guest.  This interface always exposes four register APR[0-3] describing the
    maximum possible 128 preemption levels.  The semantics of the register
    indicate if any interrupts in a given preemption level are in the active
    state by setting the corresponding bit.
    (活动优先级寄存器 APRn 是实现定义的，因此我们为自己的实现设定一种固定
    格式，以契合呈现给客户机的 "无安全扩展的 GICv2 实现" 模型。此接口总是
    暴露四个寄存器 APR[0-3]，描述最大可能的 128 个抢占级别。寄存器的语义
    是通过置位相应的位来指示给定抢占级别中的任何中断是否处于活动状态。)

    Thus, preemption level X has one or more active interrupts if and only if:

      APRn[X mod 32] == 0b1,  where n = X / 32

    (因此，当且仅当抢占级别 X 拥有一个或多个活动中断时：

      APRn[X mod 32] == 0b1，其中 n = X / 32)

    Bits for undefined preemption levels are RAZ/WI.
    (未定义抢占级别的位为 RAZ/WI。)

    Note that this differs from a CPU's view of the APRs on hardware in which
    a GIC without the security extensions expose group 0 and group 1 active
    priorities in separate register groups, whereas we show a combined view
    similar to GICv2's GICH_APR.
    (注意，这与 CPU 在硬件上查看 APR 的视角不同：在没有安全扩展的 GIC 上，
    组 0 和组 1 的活动优先级位于分开的寄存器组中，而我们展示的是类似 GICv2
    GICH_APR 的合并视图。)

    For historical reasons and to provide ABI compatibility with userspace we
    export the GICC_PMR register in the format of the GICH_VMCR.VMPriMask
    field in the lower 5 bits of a word, meaning that userspace must always
    use the lower 5 bits to communicate with the KVM device and must shift the
    value left by 3 places to obtain the actual priority mask level.
    (出于历史原因并为提供与用户空间的 ABI 兼容性，我们以 GICH_VMCR.VMPriMask
    字段的格式导出 GICC_PMR 寄存器（位于一个字的低 5 位），这意味着用户空间
    必须始终使用低 5 位与 KVM 设备通信，并且必须将该值左移 3 位才能获得实际的
    优先级屏蔽级别。)

  Errors:
  (错误：)

    =======  =====================================================
    -ENXIO   Getting or setting this register is not yet supported (尚不支持获取或设置此寄存器)
    -EBUSY   One or more VCPUs are running (一个或多个 VCPU 正在运行)
    -EINVAL  Invalid vcpu_index supplied (提供的 vcpu_index 无效)
    =======  =====================================================

  KVM_DEV_ARM_VGIC_GRP_NR_IRQS
   Attributes:
   (属性：)

    A value describing the number of interrupts (SGI, PPI and SPI) for
    this GIC instance, ranging from 64 to 1024, in increments of 32.
    (描述此 GIC 实例中断数量（SGI、PPI 和 SPI）的值，范围从 64 到 1024，
    以 32 为步长递增。)

  Errors:
  (错误：)

    =======  =============================================================
    -EINVAL  Value set is out of the expected range (设置的值超出预期范围)
    -EBUSY   Value has already be set, or GIC has already been initialized
             with default values. (值已被设置，或 GIC 已使用默认值初始化。)
    =======  =============================================================

  KVM_DEV_ARM_VGIC_GRP_CTRL
   Attributes:
   (属性：)

    KVM_DEV_ARM_VGIC_CTRL_INIT
      request the initialization of the VGIC or ITS, no additional parameter
      in kvm_device_attr.addr.
      (请求初始化 VGIC 或 ITS，kvm_device_attr.addr 中没有附加参数。)

  Errors:
  (错误：)

    =======  =========================================================
    -ENXIO   VGIC not properly configured as required prior to calling
             this attribute (调用此属性之前，VGIC 未按要求正确配置)
    -ENODEV  no online VCPU (没有在线的 VCPU)
    -ENOMEM  memory shortage when allocating vgic internal data (分配 vgic 内部数据时内存不足)
    =======  =========================================================
