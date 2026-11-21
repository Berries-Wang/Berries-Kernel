.. SPDX-License-Identifier: GPL-2.0

=============================================================
ARM firmware pseudo-registers interface(ARM 固件伪寄存器接口)
=============================================================

KVM handles the hypercall services as requested by the guests. New hypercall
services are regularly made available by the ARM specification or by KVM (as
vendor services) if they make sense from a virtualization point of view.
(KVM 按客户机的请求处理超调用 (hypercall) 服务。新的超调用服务会定期由 ARM
规范或 KVM（作为厂商服务）提供，只要它们从虚拟化的角度来看是有意义的。)

This means that a guest booted on two different versions of KVM can observe
two different "firmware" revisions. This could cause issues if a given guest
is tied to a particular version of a hypercall service, or if a migration
causes a different version to be exposed out of the blue to an unsuspecting
guest.
(这意味着，引导在两个不同版本 KVM 上的客户机可能观察到两个不同的 "固件"
版本。如果某个客户机绑定于特定版本的超调用服务，或者迁移导致突然向毫无
防备的客户机暴露出不同的版本，这可能会引发问题。)

In order to remedy this situation, KVM exposes a set of "firmware
pseudo-registers" that can be manipulated using the GET/SET_ONE_REG
interface. These registers can be saved/restored by userspace, and set
to a convenient value as required.
(为了补救这种情况，KVM 暴露了一组可以使用 GET/SET_ONE_REG 接口操纵的
"固件伪寄存器"。用户空间可以保存/恢复这些寄存器，并按需把它们设置为
合适的值。)

The following registers are defined:
(定义了以下寄存器：)

* KVM_REG_ARM_PSCI_VERSION:

  KVM implements the PSCI (Power State Coordination Interface)
  specification in order to provide services such as CPU on/off, reset
  and power-off to the guest.
  (KVM 实现 PSCI（Power State Coordination Interface，电源状态协调接口）
  规范，以便向客户机提供 CPU 上/下电、复位和关机等服务。)

  - Only valid if the vcpu has the KVM_ARM_VCPU_PSCI_0_2 feature set
    (and thus has already been initialized)
    (仅当 vcpu 设置了 KVM_ARM_VCPU_PSCI_0_2 特性时才有效（因此已经完成
    初始化）)
  - Returns the current PSCI version on GET_ONE_REG (defaulting to the
    highest PSCI version implemented by KVM and compatible with v0.2)
    (GET_ONE_REG 返回当前 PSCI 版本（默认为 KVM 实现的、与 v0.2 兼容的
    最高 PSCI 版本）)
  - Allows any PSCI version implemented by KVM and compatible with
    v0.2 to be set with SET_ONE_REG
    (允许用 SET_ONE_REG 设置 KVM 实现的、与 v0.2 兼容的任何 PSCI 版本)
  - Affects the whole VM (even if the register view is per-vcpu)
    (影响整个 VM（即使寄存器视图是 per-vcpu 的）)

* KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_1:
    Holds the state of the firmware support to mitigate CVE-2017-5715, as
    offered by KVM to the guest via a HVC call. The workaround is described
    under SMCCC_ARCH_WORKAROUND_1 in [1].
    (保存 KVM 通过 HVC 调用向客户机提供的、用于缓解 CVE-2017-5715 的固件
    支持状态。该规避方法在 [1] 的 SMCCC_ARCH_WORKAROUND_1 项下描述。)

  Accepted values are:
  (接受的值：)

    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_1_NOT_AVAIL:
      KVM does not offer
      firmware support for the workaround. The mitigation status for the
      guest is unknown.
      (KVM 不提供该规避方法的固件支持。客户机的缓解状态未知。)
    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_1_AVAIL:
      The workaround HVC call is
      available to the guest and required for the mitigation.
      (规避 HVC 调用对客户机可用，且为缓解所必需。)
    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_1_NOT_REQUIRED:
      The workaround HVC call
      is available to the guest, but it is not needed on this VCPU.
      (规避 HVC 调用对客户机可用，但在此 VCPU 上不需要。)

* KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2:
    Holds the state of the firmware support to mitigate CVE-2018-3639, as
    offered by KVM to the guest via a HVC call. The workaround is described
    under SMCCC_ARCH_WORKAROUND_2 in [1]_.
    (保存 KVM 通过 HVC 调用向客户机提供的、用于缓解 CVE-2018-3639 的固件
    支持状态。该规避方法在 SMCCC_ARCH_WORKAROUND_2 项下描述。)

  Accepted values are:
  (接受的值：)

    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2_NOT_AVAIL:
      A workaround is not
      available. KVM does not offer firmware support for the workaround.
      (规避方法不可用。KVM 不提供该规避方法的固件支持。)
    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2_UNKNOWN:
      The workaround state is
      unknown. KVM does not offer firmware support for the workaround.
      (规避状态未知。KVM 不提供该规避方法的固件支持。)
    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2_AVAIL:
      The workaround is available,
      and can be disabled by a vCPU. If
      KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2_ENABLED is set, it is active for
      this vCPU.
      (规避方法可用，并且可以被 vCPU 禁用。如果设置了
      KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2_ENABLED，则它在此 vCPU 上处于
      活动状态。)
    KVM_REG_ARM_SMCCC_ARCH_WORKAROUND_2_NOT_REQUIRED:
      The workaround is always active on this vCPU or it is not needed.
      (该规避方法在此 vCPU 上总是活动，或者不需要它。)


Bitmap Feature Firmware Registers(位图特性固件寄存器)
-----------------------------------------------------

Contrary to the above registers, the following registers exposes the
hypercall services in the form of a feature-bitmap to the userspace. This
bitmap is translated to the services that are available to the guest.
There is a register defined per service call owner and can be accessed via
GET/SET_ONE_REG interface.
(与上述寄存器相反，以下寄存器以特性位图的形式向用户空间暴露超调用服务。
该位图被转换为客户机可用的服务。每个服务调用拥有者定义一个寄存器，可
通过 GET/SET_ONE_REG 接口访问。)

By default, these registers are set with the upper limit of the features
that are supported. This way userspace can discover all the usable
hypercall services via GET_ONE_REG. The user-space can write-back the
desired bitmap back via SET_ONE_REG. The features for the registers that
are untouched, probably because userspace isn't aware of them, will be
exposed as is to the guest.
(默认情况下，这些寄存器被设置为所支持特性的上限。这样用户空间可以通过
GET_ONE_REG 发现所有可用的超调用服务。用户空间可以通过 SET_ONE_REG 写回
所需的位图。未被触及的寄存器（可能是因为用户空间不知道它们）的特性将
原样暴露给客户机。)

Note that KVM will not allow the userspace to configure the registers
anymore once any of the vCPUs has run at least once. Instead, it will
return a -EBUSY.
(注意，一旦任何 vCPU 至少运行过一次，KVM 将不再允许用户空间配置这些
寄存器。此时它会返回 -EBUSY。)

The pseudo-firmware bitmap register are as follows:
(伪固件位图寄存器如下：)

* KVM_REG_ARM_STD_BMAP:
    Controls the bitmap of the ARM Standard Secure Service Calls.
    (控制 ARM 标准安全服务调用 (Standard Secure Service Calls) 的
    位图。)

  The following bits are accepted:
  (接受以下位：)

    Bit-0: KVM_REG_ARM_STD_BIT_TRNG_V1_0:
      The bit represents the services offered under v1.0 of ARM True Random
      Number Generator (TRNG) specification, ARM DEN0098.
      (该位表示 ARM 真随机数生成器 (TRNG) 规范 ARM DEN0098 v1.0 下提供的
      服务。)

* KVM_REG_ARM_STD_HYP_BMAP:
    Controls the bitmap of the ARM Standard Hypervisor Service Calls.
    (控制 ARM 标准 hypervisor 服务调用 (Standard Hypervisor Service
    Calls) 的位图。)

  The following bits are accepted:
  (接受以下位：)

    Bit-0: KVM_REG_ARM_STD_HYP_BIT_PV_TIME:
      The bit represents the Paravirtualized Time service as represented by
      ARM DEN0057A.
      (该位表示 ARM DEN0057A 所描述的半虚拟化时间 (Paravirtualized Time)
      服务。)

* KVM_REG_ARM_VENDOR_HYP_BMAP:
    Controls the bitmap of the Vendor specific Hypervisor Service Calls[0-63].
    (控制厂商特定 hypervisor 服务调用 [0-63] 的位图。)

  The following bits are accepted:
  (接受以下位：)

    Bit-0: KVM_REG_ARM_VENDOR_HYP_BIT_FUNC_FEAT
      The bit represents the ARM_SMCCC_VENDOR_HYP_KVM_FEATURES_FUNC_ID
      and ARM_SMCCC_VENDOR_HYP_CALL_UID_FUNC_ID function-ids.
      (该位表示 ARM_SMCCC_VENDOR_HYP_KVM_FEATURES_FUNC_ID 和
      ARM_SMCCC_VENDOR_HYP_CALL_UID_FUNC_ID 功能 ID。)

    Bit-1: KVM_REG_ARM_VENDOR_HYP_BIT_PTP:
      The bit represents the Precision Time Protocol KVM service.
      (该位表示精确时间协议 (Precision Time Protocol) KVM 服务。)

* KVM_REG_ARM_VENDOR_HYP_BMAP_2:
    Controls the bitmap of the Vendor specific Hypervisor Service Calls[64-127].
    (控制厂商特定 hypervisor 服务调用 [64-127] 的位图。)

  The following bits are accepted:
  (接受以下位：)

    Bit-0: KVM_REG_ARM_VENDOR_HYP_BIT_DISCOVER_IMPL_VER
      This represents the ARM_SMCCC_VENDOR_HYP_KVM_DISCOVER_IMPL_VER_FUNC_ID
      function-id. This is reset to 0.
      (表示 ARM_SMCCC_VENDOR_HYP_KVM_DISCOVER_IMPL_VER_FUNC_ID 功能 ID。
      它被复位为 0。)

    Bit-1: KVM_REG_ARM_VENDOR_HYP_BIT_DISCOVER_IMPL_CPUS
      This represents the ARM_SMCCC_VENDOR_HYP_KVM_DISCOVER_IMPL_CPUS_FUNC_ID
      function-id. This is reset to 0.
      (表示 ARM_SMCCC_VENDOR_HYP_KVM_DISCOVER_IMPL_CPUS_FUNC_ID 功能 ID。
      它被复位为 0。)

Errors:
(错误：)

    =======  =============================================================
    -ENOENT   Unknown register accessed. (访问了未知的寄存器。)
    -EBUSY    Attempt a 'write' to the register after the VM has started. (VM 启动之后尝试对寄存器执行 'write'。)
    -EINVAL   Invalid bitmap written to the register. (向寄存器写入了无效的位图。)
    =======  =============================================================

.. [1] https://developer.arm.com/-/media/developer/pdf/ARM_DEN_0070A_Firmware_interfaces_for_mitigating_CVE-2017-5715.pdf
