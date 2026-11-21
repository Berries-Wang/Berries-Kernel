.. SPDX-License-Identifier: GPL-2.0

==========================================
XICS interrupt controller(XICS 中断控制器)
==========================================

Device type supported: KVM_DEV_TYPE_XICS
(支持的设备类型：KVM_DEV_TYPE_XICS。)

Groups:
(组：)
  1. KVM_DEV_XICS_GRP_SOURCES
       Attributes:

         One per interrupt source, indexed by the source number.
         (每个中断源一个，以源编号为索引。)
  2. KVM_DEV_XICS_GRP_CTRL
       Attributes:

         2.1 KVM_DEV_XICS_NR_SERVERS (write only)

  The kvm_device_attr.addr points to a __u32 value which is the number of
  interrupt server numbers (ie, highest possible vcpu id plus one).
  (kvm_device_attr.addr 指向一个 __u32 值，即中断服务器编号的数量
  (即最大可能的 vcpu id 加 1)。)

  Errors:
  (错误：)

    =======  ==========================================
    -EINVAL  Value greater than KVM_MAX_VCPU_IDS.
    -EFAULT  Invalid user pointer for attr->addr.
    -EBUSY   A vcpu is already connected to the device.
    =======  ==========================================

This device emulates the XICS (eXternal Interrupt Controller
Specification) defined in PAPR.  The XICS has a set of interrupt
sources, each identified by a 20-bit source number, and a set of
Interrupt Control Presentation (ICP) entities, also called "servers",
each associated with a virtual CPU.
(该设备模拟 PAPR 中定义的 XICS(eXternal Interrupt Controller
Specification)。XICS 有一组中断源，每个中断源由一个 20 位的源编号
标识；还有一组中断控制呈现(Interrupt Control Presentation, ICP)
实体，也称为 "servers"，每个实体与一个虚拟 CPU 关联。)

The ICP entities are created by enabling the KVM_CAP_IRQ_ARCH
capability for each vcpu, specifying KVM_CAP_IRQ_XICS in args[0] and
the interrupt server number (i.e. the vcpu number from the XICS's
point of view) in args[1] of the kvm_enable_cap struct.  Each ICP has
64 bits of state which can be read and written using the
KVM_GET_ONE_REG and KVM_SET_ONE_REG ioctls on the vcpu.  The 64 bit
state word has the following bitfields, starting at the
least-significant end of the word:
(ICP 实体通过为每个 vcpu 启用 KVM_CAP_IRQ_ARCH 能力来创建：在
kvm_enable_cap 结构的 args[0] 中指定 KVM_CAP_IRQ_XICS，并在 args[1]
中指定中断服务器编号(即从 XICS 角度看的 vcpu 编号)。每个 ICP 有
64 位状态，可以在该 vcpu 上使用 KVM_GET_ONE_REG 和 KVM_SET_ONE_REG
ioctl 读写。64 位状态字包含以下位域，从字的最低有效端开始：)

* Unused, 16 bits
  (未使用，16 位)

* Pending interrupt priority, 8 bits
  Zero is the highest priority, 255 means no interrupt is pending.
  (待决中断优先级，8 位。零是最高优先级，255 表示没有中断待决。)

* Pending IPI (inter-processor interrupt) priority, 8 bits
  Zero is the highest priority, 255 means no IPI is pending.
  (待决 IPI(处理器间中断)优先级，8 位。零是最高优先级，255 表示
  没有 IPI 待决。)

* Pending interrupt source number, 24 bits
  Zero means no interrupt pending, 2 means an IPI is pending
  (待决中断源编号，24 位。零表示没有中断待决，2 表示有 IPI 待决)

* Current processor priority, 8 bits
  Zero is the highest priority, meaning no interrupts can be
  delivered, and 255 is the lowest priority.
  (当前处理器优先级，8 位。零是最高优先级，意味着不能递交任何
  中断；255 是最低优先级。)

Each source has 64 bits of state that can be read and written using
the KVM_GET_DEVICE_ATTR and KVM_SET_DEVICE_ATTR ioctls, specifying the
KVM_DEV_XICS_GRP_SOURCES attribute group, with the attribute number being
the interrupt source number.  The 64 bit state word has the following
bitfields, starting from the least-significant end of the word:
(每个源有 64 位状态，可以使用 KVM_GET_DEVICE_ATTR 和
KVM_SET_DEVICE_ATTR ioctl 读写，需指定 KVM_DEV_XICS_GRP_SOURCES 属性
组，属性编号即中断源编号。64 位状态字包含以下位域，从字的最低有效端
开始：)

* Destination (server number), 32 bits

  This specifies where the interrupt should be sent, and is the
  interrupt server number specified for the destination vcpu.
  (目标(服务器编号)，32 位。它指定中断应发送到哪里，即为目标 vcpu
  指定的中断服务器编号。)

* Priority, 8 bits

  This is the priority specified for this interrupt source, where 0 is
  the highest priority and 255 is the lowest.  An interrupt with a
  priority of 255 will never be delivered.
  (优先级，8 位。这是为此中断源指定的优先级，0 是最高优先级，255 是
  最低。优先级为 255 的中断永远不会被递交。)

* Level sensitive flag, 1 bit

  This bit is 1 for a level-sensitive interrupt source, or 0 for
  edge-sensitive (or MSI).
  (电平敏感标志，1 位。电平敏感中断源此位为 1，边沿敏感(或 MSI)
  则为 0。)

* Masked flag, 1 bit

  This bit is set to 1 if the interrupt is masked (cannot be delivered
  regardless of its priority), for example by the ibm,int-off RTAS
  call, or 0 if it is not masked.
  (屏蔽标志，1 位。如果中断被屏蔽(无论其优先级如何都无法递交)，
  例如通过 ibm,int-off RTAS 调用，此位设为 1；未屏蔽则为 0。)

* Pending flag, 1 bit

  This bit is 1 if the source has a pending interrupt, otherwise 0.
  (待决标志，1 位。如果该源有待决中断，此位为 1，否则为 0。)

Only one XICS instance may be created per VM.
(每个 VM 只能创建一个 XICS 实例。)
