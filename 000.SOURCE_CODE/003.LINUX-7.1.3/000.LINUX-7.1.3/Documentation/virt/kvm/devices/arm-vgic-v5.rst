.. SPDX-License-Identifier: GPL-2.0

========================================================================================
ARM Virtual Generic Interrupt Controller v5 (VGICv5)(ARM 虚拟通用中断控制器 v5 (VGICv5))
========================================================================================


Device types supported:
(支持的设备类型：)
  - KVM_DEV_TYPE_ARM_VGIC_V5     ARM Generic Interrupt Controller v5.0(ARM 通用中断控制器 v5.0)

Only one VGIC instance may be instantiated through this API.  The created VGIC
will act as the VM interrupt controller, requiring emulated user-space devices
to inject interrupts to the VGIC instead of directly to CPUs.
(通过此 API 只能实例化一个 VGIC 实例。所创建的 VGIC 将充当 VM 的中断控制
器，要求模拟的用户态设备把中断注入 VGIC，而不是直接注入 CPU。)

Creating a guest GICv5 device requires a host GICv5 host.  The current VGICv5
device only supports PPI interrupts.  These can either be injected from emulated
in-kernel devices (such as the Arch Timer, or PMU), or via the KVM_IRQ_LINE
ioctl.
(创建客户机 GICv5 设备需要宿主机具有 GICv5。当前的 VGICv5 设备只支持 PPI
中断。这些中断既可以从内核中模拟的设备 (如 Arch Timer 或 PMU) 注入，也可以
通过 KVM_IRQ_LINE ioctl 注入。)

Groups:
(组：)
  KVM_DEV_ARM_VGIC_GRP_CTRL
   Attributes:
   (属性：)

    KVM_DEV_ARM_VGIC_CTRL_INIT
      request the initialization of the VGIC, no additional parameter in
      kvm_device_attr.addr. Must be called after all VCPUs have been created.
      (请求对 VGIC 进行初始化，kvm_device_attr.addr 中没有额外参数。必须在
      所有 VCPU 创建之后调用。)

   KVM_DEV_ARM_VGIC_USERPSPACE_PPIs
      request the mask of userspace-drivable PPIs. Only a subset of the PPIs can
      be directly driven from userspace with GICv5, and the returned mask
      informs userspace of which it is allowed to drive via KVM_IRQ_LINE.
      (请求可由用户态驱动的 PPI 掩码。在 GICv5 上只有一部分 PPI 可以由
      用户态直接驱动，返回的掩码告知用户态允许通过 KVM_IRQ_LINE 驱动哪些
      PPI。)

      Userspace must allocate and point to __u64[2] of data in
      kvm_device_attr.addr. When this call returns, the provided memory will be
      populated with the userspace PPI mask. The lower __u64 contains the mask
      for the lower 64 PPIS, with the remaining 64 being in the second __u64.
      (用户态必须分配 __u64[2] 数据并让 kvm_device_attr.addr 指向它。当此
      调用返回时，所提供的内存中将填充用户态 PPI 掩码。低位的 __u64 包含
      低 64 个 PPI 的掩码，其余 64 个在第二个 __u64 中。)

      This is a read-only attribute, and cannot be set. Attempts to set it are
      rejected.
      (这是一个只读属性，不能设置。设置它的尝试会被拒绝。)

  Errors:
  (错误：)

    =======  ========================================================
    -ENXIO   VGIC not properly configured as required prior to calling
             this attribute(调用此属性之前 VGIC 未按要求正确配置)
    -ENODEV  no online VCPU(没有在线的 VCPU)
    -ENOMEM  memory shortage when allocating vgic internal data(分配 vgic 内部数据时内存不足)
    -EFAULT  Invalid guest ram access(无效的客户机内存访问)
    -EBUSY   One or more VCPUS are running(有一个或多个 VCPU 正在运行)
    =======  ========================================================
