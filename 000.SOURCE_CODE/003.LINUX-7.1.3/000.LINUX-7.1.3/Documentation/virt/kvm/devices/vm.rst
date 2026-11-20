.. SPDX-License-Identifier: GPL-2.0

==================================
Generic vm interface(通用 vm 接口)
==================================

The virtual machine "device" also accepts the ioctls KVM_SET_DEVICE_ATTR,
KVM_GET_DEVICE_ATTR, and KVM_HAS_DEVICE_ATTR. The interface uses the same
struct kvm_device_attr as other devices, but targets VM-wide settings
and controls.
(虚拟机 "设备" 还接受 ioctl KVM_SET_DEVICE_ATTR、KVM_GET_DEVICE_ATTR 与 KVM_HAS_DEVICE_ATTR。该接口使用与其他设备相同的 struct kvm_device_attr，但面向的是 VM 范围的设置与控制。)

The groups and attributes per virtual machine, if any, are architecture
specific.
(每个虚拟机可用的组与属性(如果有)取决于具体体系结构。)

1. GROUP: KVM_S390_VM_MEM_CTRL(第 1 组：KVM_S390_VM_MEM_CTRL)
=============================================================

:Architectures: s390

1.1. ATTRIBUTE: KVM_S390_VM_MEM_ENABLE_CMMA(1.1. 属性：KVM_S390_VM_MEM_ENABLE_CMMA)
-----------------------------------------------------------------------------------

:Parameters: none(无)
:Returns: -EBUSY if a vcpu is already defined, otherwise 0
	  (如果已经定义了某个 vcpu 则为 -EBUSY，否则为 0)

Enables Collaborative Memory Management Assist (CMMA) for the virtual machine.
(为虚拟机启用协作式内存管理辅助 (Collaborative Memory Management Assist，CMMA)。)

1.2. ATTRIBUTE: KVM_S390_VM_MEM_CLR_CMMA(1.2. 属性：KVM_S390_VM_MEM_CLR_CMMA)
-----------------------------------------------------------------------------

:Parameters: none(无)
:Returns: -EINVAL if CMMA was not enabled;
	  0 otherwise
	  (如果 CMMA 未启用则为 -EINVAL；否则为 0)

Clear the CMMA status for all guest pages, so any pages the guest marked
as unused are again used any may not be reclaimed by the host.
(清除所有客户机页面的 CMMA 状态，使客户机标记为未使用的页面重新变为"在使用"，从而可能不再被宿主机回收。)

1.3. ATTRIBUTE KVM_S390_VM_MEM_LIMIT_SIZE(1.3. 属性 KVM_S390_VM_MEM_LIMIT_SIZE)
-------------------------------------------------------------------------------

:Parameters: in attr->addr the address for the new limit of guest memory
	    (attr->addr 中是客户机内存新上限的地址)
:Returns: -EFAULT if the given address is not accessible;
	  -EINVAL if the virtual machine is of type UCONTROL;
	  -E2BIG if the given guest memory is to big for that machine;
	  -EBUSY if a vcpu is already defined;
	  -ENOMEM if not enough memory is available for a new shadow guest mapping;
	  0 otherwise.
	  (如果给定地址不可访问则为 -EFAULT；如果虚拟机类型为 UCONTROL 则为 -EINVAL；如果给定的客户机内存对该机器而言过大则为 -E2BIG；如果已定义了某个 vcpu 则为 -EBUSY；如果没有足够的内存用于新的影子客户机映射则为 -ENOMEM；否则为 0。)

Allows userspace to query the actual limit and set a new limit for
the maximum guest memory size. The limit will be rounded up to
2048 MB, 4096 GB, 8192 TB respectively, as this limit is governed by
the number of page table levels. In the case that there is no limit we will set
the limit to KVM_S390_NO_MEM_LIMIT (U64_MAX).
(允许用户空间查询实际上限并为最大客户机内存大小设置新上限。上限会分别向上取整到 2048 MB、4096 GB、8192 TB，因为该上限由页表级数决定。在没有任何限制的情况下，我们会把上限设为 KVM_S390_NO_MEM_LIMIT (U64_MAX)。)

2. GROUP: KVM_S390_VM_CPU_MODEL(第 2 组：KVM_S390_VM_CPU_MODEL)
===============================================================

:Architectures: s390

2.1. ATTRIBUTE: KVM_S390_VM_CPU_MACHINE (r/o)(2.1. 属性：KVM_S390_VM_CPU_MACHINE (r/o))
----------------------------------------------------------------------------------------

Allows user space to retrieve machine and kvm specific cpu related information::

  struct kvm_s390_vm_cpu_machine {
       __u64 cpuid;           # CPUID of host
       __u32 ibc;             # IBC level range offered by host
       __u8  pad[4];
       __u64 fac_mask[256];   # set of cpu facilities enabled by KVM
       __u64 fac_list[256];   # set of cpu facilities offered by host
  }

(允许用户空间获取与机器及 KVM 相关的 CPU 信息：)

:Parameters: address of buffer to store the machine related cpu data
	     of type struct kvm_s390_vm_cpu_machine*
	    (用于存放机器相关 CPU 数据的缓冲区地址，类型为 struct kvm_s390_vm_cpu_machine*)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    -ENOMEM if not enough memory is available to process the ioctl;
	    0 in case of success.
	    (如果给定地址无法从内核空间访问则为 -EFAULT；如果没有足够的内存处理该 ioctl 则为 -ENOMEM；成功时为 0。)

2.2. ATTRIBUTE: KVM_S390_VM_CPU_PROCESSOR (r/w)(2.2. 属性：KVM_S390_VM_CPU_PROCESSOR (r/w))
===========================================================================================

Allows user space to retrieve or request to change cpu related information for a vcpu::

  struct kvm_s390_vm_cpu_processor {
       __u64 cpuid;           # CPUID currently (to be) used by this vcpu
       __u16 ibc;             # IBC level currently (to be) used by this vcpu
       __u8  pad[6];
       __u64 fac_list[256];   # set of cpu facilities currently (to be) used
			      # by this vcpu
  }

(允许用户空间获取或请求修改某个 vcpu 的 CPU 相关信息：)

KVM does not enforce or limit the cpu model data in any form. Take the information
retrieved by means of KVM_S390_VM_CPU_MACHINE as hint for reasonable configuration
setups. Instruction interceptions triggered by additionally set facility bits that
are not handled by KVM need to by implemented in the VM driver code.
(KVM 不以任何形式强制或限制 CPU 模型数据。请把通过 KVM_S390_VM_CPU_MACHINE 获取的信息作为合理配置的参考。由额外置位的、KVM 未处理的 facility 位触发的指令拦截，需要在 VM 驱动代码中实现。)

:Parameters: address of buffer to store/set the processor related cpu
	     data of type struct kvm_s390_vm_cpu_processor*.
	    (用于存放/设置处理器相关 CPU 数据的缓冲区地址，类型为 struct kvm_s390_vm_cpu_processor*。)
:Returns:  -EBUSY in case 1 or more vcpus are already activated (only in write case);
	   -EFAULT if the given address is not accessible from kernel space;
	   -ENOMEM if not enough memory is available to process the ioctl;
	   0 in case of success.
	   (如果已激活 1 个或多个 vcpu 则为 -EBUSY(仅写操作时)；如果给定地址无法从内核空间访问则为 -EFAULT；如果没有足够的内存处理该 ioctl 则为 -ENOMEM；成功时为 0。)

.. _KVM_S390_VM_CPU_MACHINE_FEAT:

2.3. ATTRIBUTE: KVM_S390_VM_CPU_MACHINE_FEAT (r/o)(2.3. 属性：KVM_S390_VM_CPU_MACHINE_FEAT (r/o))
-------------------------------------------------------------------------------------------------

Allows user space to retrieve available cpu features. A feature is available if
provided by the hardware and supported by kvm. In theory, cpu features could
even be completely emulated by kvm.

::

  struct kvm_s390_vm_cpu_feat {
	__u64 feat[16]; # Bitmap (1 = feature available), MSB 0 bit numbering
  };

(允许用户空间获取可用的 CPU 特性。当某个特性由硬件提供且被 KVM 支持时，它就是可用的。理论上，CPU 特性甚至可以完全由 KVM 仿真。)

:Parameters: address of a buffer to load the feature list from.
	    (用于读取特性列表的缓冲区地址。)
:Returns:  -EFAULT if the given address is not accessible from kernel space;
	   0 in case of success.
	   (如果给定地址无法从内核空间访问则为 -EFAULT；成功时为 0。)

2.4. ATTRIBUTE: KVM_S390_VM_CPU_PROCESSOR_FEAT (r/w)(2.4. 属性：KVM_S390_VM_CPU_PROCESSOR_FEAT (r/w))
-----------------------------------------------------------------------------------------------------

Allows user space to retrieve or change enabled cpu features for all VCPUs of a
VM. Features that are not available cannot be enabled.
(允许用户空间获取或修改 VM 所有 VCPU 启用的 CPU 特性。不可用的特性无法被启用。)

See :ref:`KVM_S390_VM_CPU_MACHINE_FEAT` for
a description of the parameter struct.

(参数结构的说明参见 :ref:`KVM_S390_VM_CPU_MACHINE_FEAT`。)

:Parameters: address of a buffer to store/load the feature list from.
	    (用于存取特性列表的缓冲区地址。)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    -EINVAL if a cpu feature that is not available is to be enabled;
	    -EBUSY if at least one VCPU has already been defined;
	    0 in case of success.
	    (如果给定地址无法从内核空间访问则为 -EFAULT；如果试图启用一个不可用的 CPU 特性则为 -EINVAL；如果至少已定义了一个 VCPU 则为 -EBUSY；成功时为 0。)

.. _KVM_S390_VM_CPU_MACHINE_SUBFUNC:

2.5. ATTRIBUTE: KVM_S390_VM_CPU_MACHINE_SUBFUNC (r/o)(2.5. 属性：KVM_S390_VM_CPU_MACHINE_SUBFUNC (r/o))
-------------------------------------------------------------------------------------------------------

Allows user space to retrieve available cpu subfunctions without any filtering
done by a set IBC. These subfunctions are indicated to the guest VCPU via
query or "test bit" subfunctions and used e.g. by cpacf functions, plo and ptff.
(允许用户空间获取可用的 CPU 子功能，而不经过已设置 IBC 的任何过滤。这些子功能通过 query 或"测试位"子功能指示给客户机 VCPU，例如 cpacf 函数、plo 与 ptff 会用到它们。)

A subfunction block is only valid if KVM_S390_VM_CPU_MACHINE contains the
STFL(E) bit introducing the affected instruction. If the affected instruction
indicates subfunctions via a "query subfunction", the response block is
contained in the returned struct. If the affected instruction
indicates subfunctions via a "test bit" mechanism, the subfunction codes are
contained in the returned struct in MSB 0 bit numbering.
(只有当 KVM_S390_VM_CPU_MACHINE 包含引入受影响指令的 STFL(E) 位时，子功能块才有效。如果受影响的指令通过"query 子功能"指示子功能，则应答块包含在返回的结构体中。如果受影响的指令通过"测试位"机制指示子功能，则子功能代码以 MSB 0 位编号包含在返回的结构体中。)

::

  struct kvm_s390_vm_cpu_subfunc {
       u8 plo[32];           # always valid (ESA/390 feature)
       u8 ptff[16];          # valid with TOD-clock steering
       u8 kmac[16];          # valid with Message-Security-Assist
       u8 kmc[16];           # valid with Message-Security-Assist
       u8 km[16];            # valid with Message-Security-Assist
       u8 kimd[16];          # valid with Message-Security-Assist
       u8 klmd[16];          # valid with Message-Security-Assist
       u8 pckmo[16];         # valid with Message-Security-Assist-Extension 3
       u8 kmctr[16];         # valid with Message-Security-Assist-Extension 4
       u8 kmf[16];           # valid with Message-Security-Assist-Extension 4
       u8 kmo[16];           # valid with Message-Security-Assist-Extension 4
       u8 pcc[16];           # valid with Message-Security-Assist-Extension 4
       u8 ppno[16];          # valid with Message-Security-Assist-Extension 5
       u8 kma[16];           # valid with Message-Security-Assist-Extension 8
       u8 kdsa[16];          # valid with Message-Security-Assist-Extension 9
       u8 reserved[1792];    # reserved for future instructions
  };

:Parameters: address of a buffer to load the subfunction blocks from.
	    (用于读取子功能块的缓冲区地址。)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    0 in case of success.
	    (如果给定地址无法从内核空间访问则为 -EFAULT；成功时为 0。)

2.6. ATTRIBUTE: KVM_S390_VM_CPU_PROCESSOR_SUBFUNC (r/w)(2.6. 属性：KVM_S390_VM_CPU_PROCESSOR_SUBFUNC (r/w))
-----------------------------------------------------------------------------------------------------------

Allows user space to retrieve or change cpu subfunctions to be indicated for
all VCPUs of a VM. This attribute will only be available if kernel and
hardware support are in place.
(允许用户空间获取或修改将向 VM 所有 VCPU 指示的 CPU 子功能。只有当内核与硬件支持齐备时，该属性才可用。)

The kernel uses the configured subfunction blocks for indication to
the guest. A subfunction block will only be used if the associated STFL(E) bit
has not been disabled by user space (so the instruction to be queried is
actually available for the guest).
(内核使用已配置的子功能块向客户机做指示。只有当关联的 STFL(E) 位未被用户空间禁用时，子功能块才会被使用(即要查询的指令对客户机确实可用)。)

As long as no data has been written, a read will fail. The IBC will be used
to determine available subfunctions in this case, this will guarantee backward
compatibility.
(只要尚未写入任何数据，读取就会失败。此时将使用 IBC 来确定可用的子功能，以此保证向后兼容。)

See :ref:`KVM_S390_VM_CPU_MACHINE_SUBFUNC` for a
description of the parameter struct.
(参数结构的说明参见 :ref:`KVM_S390_VM_CPU_MACHINE_SUBFUNC`。)

:Parameters: address of a buffer to store/load the subfunction blocks from.
	    (用于存取子功能块的缓冲区地址。)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    -EINVAL when reading, if there was no write yet;
	    -EBUSY if at least one VCPU has already been defined;
	    0 in case of success.
	    (如果给定地址无法从内核空间访问则为 -EFAULT；读取时如果尚未有过写入则为 -EINVAL；如果至少已定义了一个 VCPU 则为 -EBUSY；成功时为 0。)

3. GROUP: KVM_S390_VM_TOD(第 3 组：KVM_S390_VM_TOD)
====================================================

:Architectures: s390

3.1. ATTRIBUTE: KVM_S390_VM_TOD_HIGH(3.1. 属性：KVM_S390_VM_TOD_HIGH)
---------------------------------------------------------------------

Allows user space to set/get the TOD clock extension (u8) (superseded by
KVM_S390_VM_TOD_EXT).
(允许用户空间设置/获取 TOD 时钟扩展 (u8)(已被 KVM_S390_VM_TOD_EXT 取代)。)

:Parameters: address of a buffer in user space to store the data (u8) to
	    (用户空间中存放数据 (u8) 的缓冲区地址)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    -EINVAL if setting the TOD clock extension to != 0 is not supported
	    -EOPNOTSUPP for a PV guest (TOD managed by the ultravisor)
	    (如果给定地址无法从内核空间访问则为 -EFAULT；如果不支持把 TOD 时钟扩展设为非 0 值则为 -EINVAL；对 PV 客户机为 -EOPNOTSUPP(TOD 由 ultravisor 管理))

3.2. ATTRIBUTE: KVM_S390_VM_TOD_LOW(3.2. 属性：KVM_S390_VM_TOD_LOW)
-------------------------------------------------------------------

Allows user space to set/get bits 0-63 of the TOD clock register as defined in
the POP (u64).
(允许用户空间设置/获取 POP 中定义的 TOD 时钟寄存器的 0-63 位 (u64)。)

:Parameters: address of a buffer in user space to store the data (u64) to
	    (用户空间中存放数据 (u64) 的缓冲区地址)
:Returns:    -EFAULT if the given address is not accessible from kernel space
	     -EOPNOTSUPP for a PV guest (TOD managed by the ultravisor)
	     (如果给定地址无法从内核空间访问则为 -EFAULT；对 PV 客户机为 -EOPNOTSUPP(TOD 由 ultravisor 管理))

3.3. ATTRIBUTE: KVM_S390_VM_TOD_EXT(3.3. 属性：KVM_S390_VM_TOD_EXT)
-------------------------------------------------------------------

Allows user space to set/get bits 0-63 of the TOD clock register as defined in
the POP (u64). If the guest CPU model supports the TOD clock extension (u8), it
also allows user space to get/set it. If the guest CPU model does not support
it, it is stored as 0 and not allowed to be set to a value != 0.
(允许用户空间设置/获取 POP 中定义的 TOD 时钟寄存器的 0-63 位 (u64)。如果客户机 CPU 模型支持 TOD 时钟扩展 (u8)，它还允许用户空间获取/设置该扩展。如果客户机 CPU 模型不支持，该扩展将被存为 0，且不允许设置为非 0 值。)

:Parameters: address of a buffer in user space to store the data
	     (kvm_s390_vm_tod_clock) to
	    (用户空间中存放数据 (kvm_s390_vm_tod_clock) 的缓冲区地址)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    -EINVAL if setting the TOD clock extension to != 0 is not supported
	    -EOPNOTSUPP for a PV guest (TOD managed by the ultravisor)
	    (如果给定地址无法从内核空间访问则为 -EFAULT；如果不支持把 TOD 时钟扩展设为非 0 值则为 -EINVAL；对 PV 客户机为 -EOPNOTSUPP(TOD 由 ultravisor 管理))

4. GROUP: KVM_S390_VM_CRYPTO(第 4 组：KVM_S390_VM_CRYPTO)
=========================================================

:Architectures: s390

4.1. ATTRIBUTE: KVM_S390_VM_CRYPTO_ENABLE_AES_KW (w/o)(4.1. 属性：KVM_S390_VM_CRYPTO_ENABLE_AES_KW (w/o))
---------------------------------------------------------------------------------------------------------

Allows user space to enable aes key wrapping, including generating a new
wrapping key.
(允许用户空间启用 AES 密钥包裹 (key wrapping)，包括生成新的包裹密钥。)

:Parameters: none(无)
:Returns:    0

4.2. ATTRIBUTE: KVM_S390_VM_CRYPTO_ENABLE_DEA_KW (w/o)(4.2. 属性：KVM_S390_VM_CRYPTO_ENABLE_DEA_KW (w/o))
---------------------------------------------------------------------------------------------------------

Allows user space to enable dea key wrapping, including generating a new
wrapping key.
(允许用户空间启用 DEA 密钥包裹，包括生成新的包裹密钥。)

:Parameters: none(无)
:Returns:    0

4.3. ATTRIBUTE: KVM_S390_VM_CRYPTO_DISABLE_AES_KW (w/o)(4.3. 属性：KVM_S390_VM_CRYPTO_DISABLE_AES_KW (w/o))
-----------------------------------------------------------------------------------------------------------

Allows user space to disable aes key wrapping, clearing the wrapping key.
(允许用户空间禁用 AES 密钥包裹，并清除包裹密钥。)

:Parameters: none(无)
:Returns:    0

4.4. ATTRIBUTE: KVM_S390_VM_CRYPTO_DISABLE_DEA_KW (w/o)(4.4. 属性：KVM_S390_VM_CRYPTO_DISABLE_DEA_KW (w/o))
-----------------------------------------------------------------------------------------------------------

Allows user space to disable dea key wrapping, clearing the wrapping key.
(允许用户空间禁用 DEA 密钥包裹，并清除包裹密钥。)

:Parameters: none(无)
:Returns:    0

5. GROUP: KVM_S390_VM_MIGRATION(第 5 组：KVM_S390_VM_MIGRATION)
===============================================================

:Architectures: s390

5.1. ATTRIBUTE: KVM_S390_VM_MIGRATION_STOP (w/o)(5.1. 属性：KVM_S390_VM_MIGRATION_STOP (w/o))
---------------------------------------------------------------------------------------------

Allows userspace to stop migration mode, needed for PGSTE migration.
Setting this attribute when migration mode is not active will have no
effects.
(允许用户空间停止迁移模式，PGSTE 迁移需要该模式。在迁移模式未激活时设置该属性不会有任何效果。)

:Parameters: none(无)
:Returns:    0

5.2. ATTRIBUTE: KVM_S390_VM_MIGRATION_START (w/o)(5.2. 属性：KVM_S390_VM_MIGRATION_START (w/o))
-----------------------------------------------------------------------------------------------

Allows userspace to start migration mode, needed for PGSTE migration.
Setting this attribute when migration mode is already active will have
no effects.
(允许用户空间启动迁移模式，PGSTE 迁移需要该模式。在迁移模式已激活时设置该属性不会有任何效果。)

Dirty tracking must be enabled on all memslots, else -EINVAL is returned. When
dirty tracking is disabled on any memslot, migration mode is automatically
stopped.
(必须在所有 memslot 上启用脏页跟踪，否则返回 -EINVAL。当任何一个 memslot 禁用脏页跟踪时，迁移模式会自动停止。)

:Parameters: none(无)
:Returns:   -ENOMEM if there is not enough free memory to start migration mode;
	    -EINVAL if the state of the VM is invalid (e.g. no memory defined);
	    0 in case of success.
	    (如果没有足够的空闲内存启动迁移模式则为 -ENOMEM；如果 VM 状态无效(例如未定义内存)则为 -EINVAL；成功时为 0。)

5.3. ATTRIBUTE: KVM_S390_VM_MIGRATION_STATUS (r/o)(5.3. 属性：KVM_S390_VM_MIGRATION_STATUS (r/o))
-------------------------------------------------------------------------------------------------

Allows userspace to query the status of migration mode.
(允许用户空间查询迁移模式的状态。)

:Parameters: address of a buffer in user space to store the data (u64) to;
	     the data itself is either 0 if migration mode is disabled or 1
	     if it is enabled
	    (用户空间中存放数据 (u64) 的缓冲区地址；数据本身在迁移模式禁用时为 0，启用时为 1)
:Returns:   -EFAULT if the given address is not accessible from kernel space;
	    0 in case of success.
	    (如果给定地址无法从内核空间访问则为 -EFAULT；成功时为 0。)

6. GROUP: KVM_ARM_VM_SMCCC_CTRL(第 6 组：KVM_ARM_VM_SMCCC_CTRL)
===============================================================

:Architectures: arm64

6.1. ATTRIBUTE: KVM_ARM_VM_SMCCC_FILTER (w/o)(6.1. 属性：KVM_ARM_VM_SMCCC_FILTER (w/o))
---------------------------------------------------------------------------------------

:Parameters: Pointer to a ``struct kvm_smccc_filter``
	    (指向 ``struct kvm_smccc_filter`` 的指针)

:Returns:

        ======  ===========================================
        EEXIST  Range intersects with a previously inserted
                or reserved range (与先前插入或保留的范围相交)
        EBUSY   A vCPU in the VM has already run (VM 中已有 vCPU 运行过)
        EINVAL  Invalid filter configuration (无效的过滤器配置)
        ENOMEM  Failed to allocate memory for the in-kernel
                representation of the SMCCC filter (为内核内的 SMCCC 过滤器表示分配内存失败)
        ======  ===========================================

(返回值说明：EEXIST——范围与先前插入或保留的范围相交；EBUSY——VM 中已有 vCPU 运行过；EINVAL——无效的过滤器配置；ENOMEM——为内核内的 SMCCC 过滤器表示分配内存失败。)

Requests the installation of an SMCCC call filter described as follows::

    enum kvm_smccc_filter_action {
            KVM_SMCCC_FILTER_HANDLE = 0,
            KVM_SMCCC_FILTER_DENY,
            KVM_SMCCC_FILTER_FWD_TO_USER,
    };

    struct kvm_smccc_filter {
            __u32 base;
            __u32 nr_functions;
            __u8 action;
            __u8 pad[15];
    };

(请求安装一个按如下方式描述的 SMCCC 调用过滤器：)

The filter is defined as a set of non-overlapping ranges. Each
range defines an action to be applied to SMCCC calls within the range.
Userspace can insert multiple ranges into the filter by using
successive calls to this attribute.
(过滤器被定义为一组互不重叠的范围。每个范围定义一个要应用于该范围内 SMCCC 调用的动作。用户空间可以通过对这一属性的多次调用，向过滤器插入多个范围。)

The default configuration of KVM is such that all implemented SMCCC
calls are allowed. Thus, the SMCCC filter can be defined sparsely
by userspace, only describing ranges that modify the default behavior.
(KVM 的默认配置是允许所有已实现的 SMCCC 调用。因此，用户空间可以稀疏地定义 SMCCC 过滤器，只描述那些会修改默认行为的范围。)

The range expressed by ``struct kvm_smccc_filter`` is
[``base``, ``base + nr_functions``). The range is not allowed to wrap,
i.e. userspace cannot rely on ``base + nr_functions`` overflowing.
(``struct kvm_smccc_filter`` 表示的范围是 [``base``, ``base + nr_functions``)。该范围不允许回绕，即用户空间不能依赖 ``base + nr_functions`` 发生溢出。)

The SMCCC filter applies to both SMC and HVC calls initiated by the
guest. The SMCCC filter gates the in-kernel emulation of SMCCC calls
and as such takes effect before other interfaces that interact with
SMCCC calls (e.g. hypercall bitmap registers).
(SMCCC 过滤器同时作用于由客户机发起的 SMC 与 HVC 调用。SMCCC 过滤器把守着 SMCCC 调用的内核内仿真，因此它在其他与 SMCCC 调用交互的接口(例如 hypercall 位图寄存器)之前生效。)

Actions:
(动作：)

 - ``KVM_SMCCC_FILTER_HANDLE``: Allows the guest SMCCC call to be
   handled in-kernel. It is strongly recommended that userspace *not*
   explicitly describe the allowed SMCCC call ranges.
   (``KVM_SMCCC_FILTER_HANDLE`` ：允许客户机的 SMCCC 调用在内核内处理。强烈建议用户空间 *不要* 显式描述允许的 SMCCC 调用范围。)

 - ``KVM_SMCCC_FILTER_DENY``: Rejects the guest SMCCC call in-kernel
   and returns to the guest.
   (``KVM_SMCCC_FILTER_DENY`` ：在内核内拒绝客户机的 SMCCC 调用，然后返回客户机。)

 - ``KVM_SMCCC_FILTER_FWD_TO_USER``: The guest SMCCC call is forwarded
   to userspace with an exit reason of ``KVM_EXIT_HYPERCALL``.
   (``KVM_SMCCC_FILTER_FWD_TO_USER`` ：客户机的 SMCCC 调用被转发到用户空间，退出原因为 ``KVM_EXIT_HYPERCALL``。)

The ``pad`` field is reserved for future use and must be zero. KVM may
return ``-EINVAL`` if the field is nonzero.
(``pad`` 字段留作未来使用，必须为零。如果该字段非零，KVM 可能返回 ``-EINVAL``。)

KVM reserves the 'Arm Architecture Calls' range of function IDs and
will reject attempts to define a filter for any portion of these ranges:
(KVM 保留 'Arm Architecture Calls' 功能 ID 范围，并会拒绝为这些范围的任何部分定义过滤器的企图：)

        =========== ===============
        Start(起始) End (inclusive)(结束(含))
        =========== ===============
        0x8000_0000 0x8000_FFFF
        0xC000_0000 0xC000_FFFF
        =========== ===============
