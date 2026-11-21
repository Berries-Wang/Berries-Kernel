.. SPDX-License-Identifier: GPL-2.0

=========================================================
FLIC (floating interrupt controller)(FLIC 浮动中断控制器)
=========================================================

FLIC handles floating (non per-cpu) interrupts, i.e. I/O, service and some
machine check interruptions. All interrupts are stored in a per-vm list of
pending interrupts. FLIC performs operations on this list.
(FLIC 处理浮动(非 per-cpu)中断，即 I/O、服务中断和某些机器检查中断。所有中断都存储在一个 per-vm 的挂起中断列表中。FLIC 对该列表执行操作。)

Only one FLIC instance may be instantiated.
(只能实例化一个 FLIC 实例。)

FLIC provides support to
(FLIC 提供以下支持：)
- add interrupts (KVM_DEV_FLIC_ENQUEUE)
  (添加中断 (KVM_DEV_FLIC_ENQUEUE))
- inspect currently pending interrupts (KVM_FLIC_GET_ALL_IRQS)
  (检查当前挂起的中断 (KVM_FLIC_GET_ALL_IRQS))
- purge all pending floating interrupts (KVM_DEV_FLIC_CLEAR_IRQS)
  (清除所有挂起的浮动中断 (KVM_DEV_FLIC_CLEAR_IRQS))
- purge one pending floating I/O interrupt (KVM_DEV_FLIC_CLEAR_IO_IRQ)
  (清除一条挂起的浮动 I/O 中断 (KVM_DEV_FLIC_CLEAR_IO_IRQ))
- enable/disable for the guest transparent async page faults
  (为客户机启用/禁用透明的异步页错误 (async page faults))
- register and modify adapter interrupt sources (KVM_DEV_FLIC_ADAPTER_*)
  (注册和修改适配器中断源 (KVM_DEV_FLIC_ADAPTER_*))
- modify AIS (adapter-interruption-suppression) mode state (KVM_DEV_FLIC_AISM)
  (修改 AIS (adapter-interruption-suppression，适配器中断抑制) 模式状态 (KVM_DEV_FLIC_AISM))
- inject adapter interrupts on a specified adapter (KVM_DEV_FLIC_AIRQ_INJECT)
  (在指定的适配器上注入适配器中断 (KVM_DEV_FLIC_AIRQ_INJECT))
- get/set all AIS mode states (KVM_DEV_FLIC_AISM_ALL)
  (获取/设置所有 AIS 模式状态 (KVM_DEV_FLIC_AISM_ALL))

Groups:
(组 (Groups)：)
  KVM_DEV_FLIC_ENQUEUE
    Passes a buffer and length into the kernel which are then injected into
    the list of pending interrupts.
    attr->addr contains the pointer to the buffer and attr->attr contains
    the length of the buffer.
    The format of the data structure kvm_s390_irq as it is copied from userspace
    is defined in usr/include/linux/kvm.h.
    (向内核传递一个缓冲区和长度，然后将其注入挂起中断列表。
    attr->addr 包含指向缓冲区的指针，attr->attr 包含缓冲区的长度。
    从用户空间复制而来的数据结构 kvm_s390_irq 的格式定义在
    usr/include/linux/kvm.h 中。)

  KVM_DEV_FLIC_GET_ALL_IRQS
    Copies all floating interrupts into a buffer provided by userspace.
    When the buffer is too small it returns -ENOMEM, which is the indication
    for userspace to try again with a bigger buffer.
    (将所有浮动中断复制到用户空间提供的缓冲区中。
    当缓冲区太小时，它返回 -ENOMEM，这提示用户空间用更大的缓冲区重试。)

    -ENOBUFS is returned when the allocation of a kernelspace buffer has
    failed.
    (当内核空间缓冲区的分配失败时返回 -ENOBUFS。)

    -EFAULT is returned when copying data to userspace failed.
    All interrupts remain pending, i.e. are not deleted from the list of
    currently pending interrupts.
    attr->addr contains the userspace address of the buffer into which all
    interrupt data will be copied.
    attr->attr contains the size of the buffer in bytes.
    (当向用户空间复制数据失败时返回 -EFAULT。
    所有中断保持挂起状态，即不会从当前挂起中断列表中删除。
    attr->addr 包含缓冲区的用户空间地址，所有中断数据将被复制到该缓冲区。
    attr->attr 包含缓冲区的大小(以字节为单位)。)

  KVM_DEV_FLIC_CLEAR_IRQS
    Simply deletes all elements from the list of currently pending floating
    interrupts.  No interrupts are injected into the guest.
    (只是从当前挂起的浮动中断列表中删除所有元素。不会有中断被注入到客户机。)

  KVM_DEV_FLIC_CLEAR_IO_IRQ
    Deletes one (if any) I/O interrupt for a subchannel identified by the
    subsystem identification word passed via the buffer specified by
    attr->addr (address) and attr->attr (length).
    (删除一条(如果有的话)I/O 中断，该中断所属子通道由通过 attr->addr
    (地址) 和 attr->attr (长度) 指定的缓冲区传递的子系统标识字标识。)

  KVM_DEV_FLIC_APF_ENABLE
    Enables async page faults for the guest. So in case of a major page fault
    the host is allowed to handle this async and continues the guest.
    (为客户机启用异步页错误。这样，在发生严重缺页 (major page fault) 时，
    允许主机异步处理并继续运行客户机。)

    -EINVAL is returned when called on the FLIC of a ucontrol VM.
    (在 ucontrol VM 的 FLIC 上调用时返回 -EINVAL。)

  KVM_DEV_FLIC_APF_DISABLE_WAIT
    Disables async page faults for the guest and waits until already pending
    async page faults are done. This is necessary to trigger a completion interrupt
    for every init interrupt before migrating the interrupt list.
    (为客户机禁用异步页错误，并等待已经挂起的异步页错误处理完成。
    在迁移中断列表之前，需要这样做来为每个 init 中断触发一个完成中断。)

    -EINVAL is returned when called on the FLIC of a ucontrol VM.
    (在 ucontrol VM 的 FLIC 上调用时返回 -EINVAL。)

  KVM_DEV_FLIC_ADAPTER_REGISTER
    Register an I/O adapter interrupt source. Takes a kvm_s390_io_adapter
    describing the adapter to register::

	struct kvm_s390_io_adapter {
		__u32 id;
		__u8 isc;
		__u8 maskable;
		__u8 swap;
		__u8 flags;
	};

   (注册一个 I/O 适配器中断源。接受一个 kvm_s390_io_adapter，描述要注册的适配器：)

   id contains the unique id for the adapter, isc the I/O interruption subclass
   to use, maskable whether this adapter may be masked (interrupts turned off),
   swap whether the indicators need to be byte swapped, and flags contains
   further characteristics of the adapter.
   (id 包含适配器的唯一 id，isc 表示要使用的 I/O 中断子类，maskable 表示
   该适配器是否可以被屏蔽(中断被关闭)，swap 表示指示器是否需要进行
   字节交换，flags 包含适配器的其他特性。)

   Currently defined values for 'flags' are:
   (目前为 'flags' 定义的值有：)

   - KVM_S390_ADAPTER_SUPPRESSIBLE: adapter is subject to AIS
     (adapter-interrupt-suppression) facility. This flag only has an effect if
     the AIS capability is enabled.
     (KVM_S390_ADAPTER_SUPPRESSIBLE: 适配器受 AIS
     (adapter-interrupt-suppression，适配器中断抑制) 功能的约束。仅当 AIS
     能力启用时，此标志才有效。)

   Unknown flag values are ignored.
   (未知的标志值将被忽略。)


  KVM_DEV_FLIC_ADAPTER_MODIFY
    Modifies attributes of an existing I/O adapter interrupt source. Takes
    a kvm_s390_io_adapter_req specifying the adapter and the operation::

	struct kvm_s390_io_adapter_req {
		__u32 id;
		__u8 type;
		__u8 mask;
		__u16 pad0;
		__u64 addr;
	};

    (修改现有 I/O 适配器中断源的属性。接受一个 kvm_s390_io_adapter_req，
    指定适配器和操作：)

    id specifies the adapter and type the operation. The supported operations
    are:
    (id 指定适配器，type 指定操作。支持的操作有：)

    KVM_S390_IO_ADAPTER_MASK
      mask or unmask the adapter, as specified in mask
      (按 mask 的指定屏蔽或取消屏蔽适配器)

    KVM_S390_IO_ADAPTER_MAP
      This is now a no-op. The mapping is purely done by the irq route.
      (这现在是一个空操作 (no-op)。映射完全由 irq 路由完成。)
    KVM_S390_IO_ADAPTER_UNMAP
      This is now a no-op. The mapping is purely done by the irq route.
      (这现在是一个空操作 (no-op)。映射完全由 irq 路由完成。)

  KVM_DEV_FLIC_AISM
    modify the adapter-interruption-suppression mode for a given isc if the
    AIS capability is enabled. Takes a kvm_s390_ais_req describing::

	struct kvm_s390_ais_req {
		__u8 isc;
		__u16 mode;
	};

    (如果 AIS 能力已启用，则修改给定 isc 的适配器中断抑制模式。
    接受一个 kvm_s390_ais_req，描述如下：)

    isc contains the target I/O interruption subclass, mode the target
    adapter-interruption-suppression mode. The following modes are
    currently supported:
    (isc 包含目标 I/O 中断子类，mode 表示目标适配器中断抑制模式。
    目前支持以下模式：)

    - KVM_S390_AIS_MODE_ALL: ALL-Interruptions Mode, i.e. airq injection
      is always allowed;
      (KVM_S390_AIS_MODE_ALL: ALL-Interruptions Mode (全部中断模式)，
      即始终允许注入 airq；)
    - KVM_S390_AIS_MODE_SINGLE: SINGLE-Interruption Mode, i.e. airq
      injection is only allowed once and the following adapter interrupts
      will be suppressed until the mode is set again to ALL-Interruptions
      or SINGLE-Interruption mode.
      (KVM_S390_AIS_MODE_SINGLE: SINGLE-Interruption Mode (单中断模式)，
      即 airq 注入只允许一次，随后的适配器中断将被抑制，直到模式再次
      被设置为 ALL-Interruptions 或 SINGLE-Interruption 模式。)

  KVM_DEV_FLIC_AIRQ_INJECT
    Inject adapter interrupts on a specified adapter.
    attr->attr contains the unique id for the adapter, which allows for
    adapter-specific checks and actions.
    For adapters subject to AIS, handle the airq injection suppression for
    an isc according to the adapter-interruption-suppression mode on condition
    that the AIS capability is enabled.
    (在指定的适配器上注入适配器中断。
    attr->attr 包含适配器的唯一 id，从而允许进行适配器特定的检查和操作。
    对于受 AIS 约束的适配器，在 AIS 能力启用的条件下，根据适配器中断
    抑制模式处理某个 isc 的 airq 注入抑制。)

  KVM_DEV_FLIC_AISM_ALL
    Gets or sets the adapter-interruption-suppression mode for all ISCs. Takes
    a kvm_s390_ais_all describing::

	struct kvm_s390_ais_all {
	       __u8 simm; /* Single-Interruption-Mode mask */
	       __u8 nimm; /* No-Interruption-Mode mask *
	};

    (获取或设置所有 ISC 的适配器中断抑制模式。接受一个 kvm_s390_ais_all，
    描述如下：)

    simm contains Single-Interruption-Mode mask for all ISCs, nimm contains
    No-Interruption-Mode mask for all ISCs. Each bit in simm and nimm corresponds
    to an ISC (MSB0 bit 0 to ISC 0 and so on). The combination of simm bit and
    nimm bit presents AIS mode for a ISC.
    (simm 包含所有 ISC 的 Single-Interruption-Mode 掩码，nimm 包含所有 ISC
    的 No-Interruption-Mode 掩码。 simm 和 nimm 中的每一位对应一个 ISC
    (MSB0 位 0 对应 ISC 0，依此类推)。 simm 位与 nimm 位的组合表示一个
    ISC 的 AIS 模式。)

    KVM_DEV_FLIC_AISM_ALL is indicated by KVM_CAP_S390_AIS_MIGRATION.
    (KVM_DEV_FLIC_AISM_ALL 由 KVM_CAP_S390_AIS_MIGRATION 标识。)

Note: The KVM_SET_DEVICE_ATTR/KVM_GET_DEVICE_ATTR device ioctls executed on
FLIC with an unknown group or attribute gives the error code EINVAL (instead of
ENXIO, as specified in the API documentation). It is not possible to conclude
that a FLIC operation is unavailable based on the error code resulting from a
usage attempt.
(注意：在 FLIC 上执行的 KVM_SET_DEVICE_ATTR/KVM_GET_DEVICE_ATTR 设备 ioctl，
如果组或属性未知，会给出错误码 EINVAL(而不是 API 文档中规定的 ENXIO)。
无法根据使用尝试产生的错误码断定某个 FLIC 操作不可用。)

.. note:: The KVM_DEV_FLIC_CLEAR_IO_IRQ ioctl will return EINVAL in case a
	  zero schid is specified.
	  (注意：如果指定了零 schid，KVM_DEV_FLIC_CLEAR_IO_IRQ ioctl 将
	  返回 EINVAL。)
