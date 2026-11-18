.. SPDX-License-Identifier: GPL-2.0

======================================================================================================
The Definitive KVM (Kernel-based Virtual Machine) API Documentation(KVM(基于内核的虚拟机)API 权威文档)
======================================================================================================

1. General description(1. 总体描述)
===================================

The kvm API is centered around different kinds of file descriptors
and ioctls that can be issued to these file descriptors.  An initial
open("/dev/kvm") obtains a handle to the kvm subsystem; this handle
can be used to issue system ioctls.  A KVM_CREATE_VM ioctl on this
handle will create a VM file descriptor which can be used to issue VM
ioctls.  A KVM_CREATE_VCPU or KVM_CREATE_DEVICE ioctl on a VM fd will
create a virtual cpu or device and return a file descriptor pointing to
the new resource.
(kvm API 以不同种类的文件描述符以及可向这些文件描述符发出的各种 ioctl 为核心。初次调用 open("/dev/kvm") 会取得 kvm 子系统的一个句柄；该句柄可用于发出系统级 ioctl。对该句柄发出 KVM_CREATE_VM ioctl 会创建一个 VM 文件描述符，可用于发出 VM 级 ioctl。对 VM fd 发出 KVM_CREATE_VCPU 或 KVM_CREATE_DEVICE ioctl 会创建一个虚拟 cpu 或设备，并返回指向新资源的文件描述符。)

In other words, the kvm API is a set of ioctls that are issued to
different kinds of file descriptor in order to control various aspects of
a virtual machine.  Depending on the file descriptor that accepts them,
ioctls belong to the following classes:
(换言之，kvm API 是一组发往不同种类文件描述符的 ioctl，用于控制虚拟机的各个方面。根据接受它们的文件描述符的不同，ioctl 分为以下几个类别：)

 - System ioctls: These query and set global attributes which affect the
   whole kvm subsystem.  In addition a system ioctl is used to create
   virtual machines.
   (系统级 ioctl：查询并设置影响整个 kvm 子系统的全局属性。此外，系统级 ioctl 还用于创建虚拟机。)

 - VM ioctls: These query and set attributes that affect an entire virtual
   machine, for example memory layout.  In addition a VM ioctl is used to
   create virtual cpus (vcpus) and devices.
   (VM 级 ioctl：查询并设置影响整个虚拟机的属性，例如内存布局。此外，VM 级 ioctl 还用于创建虚拟 cpu (vcpu) 和设备。)

   VM ioctls must be issued from the same process (address space) that was
   used to create the VM.
   (VM 级 ioctl 必须由创建该 VM 的同一进程(地址空间)发出。)

 - vcpu ioctls: These query and set attributes that control the operation
   of a single virtual cpu.
   (vcpu 级 ioctl：查询并设置控制单个虚拟 cpu 操作的属性。)

   vcpu ioctls should be issued from the same thread that was used to create
   the vcpu, except for asynchronous vcpu ioctl that are marked as such in
   the documentation.  Otherwise, the first ioctl after switching threads
   could see a performance impact.
   (vcpu 级 ioctl 应当由创建该 vcpu 的同一线程发出，文档中标注为异步的 vcpu ioctl 除外。否则，切换线程之后的第一个 ioctl 可能出现性能损失。)

 - device ioctls: These query and set attributes that control the operation
   of a single device.
   (device 级 ioctl：查询并设置控制单个设备操作的属性。)

   device ioctls must be issued from the same process (address space) that
   was used to create the VM.
   (device 级 ioctl 必须由创建该 VM 的同一进程(地址空间)发出。)

While most ioctls are specific to one kind of file descriptor, in some
cases the same ioctl can belong to more than one class.
(虽然大多数 ioctl 只属于某一种文件描述符，但在某些情况下，同一个 ioctl 可以属于多个类别。)

The KVM API grew over time.  For this reason, KVM defines many constants
of the form ``KVM_CAP_*``, each corresponding to a set of functionality
provided by one or more ioctls.  Availability of these "capabilities" can
be checked with :ref:`KVM_CHECK_EXTENSION <KVM_CHECK_EXTENSION>`.  Some
capabilities also need to be enabled for VMs or VCPUs where their
functionality is desired (see :ref:`cap_enable` and :ref:`cap_enable_vm`).
(KVM API 随时间不断演进。为此，KVM 定义了许多形如 KVM_CAP_* 的常量，每个常量对应由一个或多个 ioctl 提供的一组功能。这些 "capabilities"(能力)是否可用，可通过 KVM_CHECK_EXTENSION 查询。某些能力还需要在希望使用其功能的 VM 或 VCPU 上显式启用(参见 cap_enable 与 cap_enable_vm 两节)。)


2. Restrictions(2. 限制)
========================

In general file descriptors can be migrated among processes by means
of fork() and the SCM_RIGHTS facility of unix domain socket.  These
kinds of tricks are explicitly not supported by kvm.  While they will
not cause harm to the host, their actual behavior is not guaranteed by
the API.  See "General description" for details on the ioctl usage
model that is supported by KVM.
(一般来说，文件描述符可以通过 fork() 和 unix 域套接字的 SCM_RIGHTS 机制在进程之间迁移。kvm 明确不支持这类技巧。虽然它们不会对宿主机造成危害，但其实际行为不受 API 保证。有关 KVM 所支持的 ioctl 使用模型的详细信息，请参见 "General description"(总体描述)一节。)

It is important to note that although VM ioctls may only be issued from
the process that created the VM, a VM's lifecycle is associated with its
file descriptor, not its creator (process).  In other words, the VM and
its resources, *including the associated address space*, are not freed
until the last reference to the VM's file descriptor has been released.
For example, if fork() is issued after ioctl(KVM_CREATE_VM), the VM will
not be freed until both the parent (original) process and its child have
put their references to the VM's file descriptor.
(需要注意的是，虽然 VM 级 ioctl 只能由创建 VM 的进程发出，但 VM 的生命周期是与它的文件描述符而非其创建者(进程)绑定的。换言之，在对 VM 文件描述符的最后一个引用被释放之前，VM 及其资源(*包括相关联的地址空间*)都不会被释放。例如，如果在 ioctl(KVM_CREATE_VM) 之后执行了 fork()，那么要等到父(原始)进程及其子进程都释放了对 VM 文件描述符的引用之后，VM 才会被释放。)

Because a VM's resources are not freed until the last reference to its
file descriptor is released, creating additional references to a VM
via fork(), dup(), etc... without careful consideration is strongly
discouraged and may have unwanted side effects, e.g. memory allocated
by and on behalf of the VM's process may not be freed/unaccounted when
the VM is shut down.
(由于 VM 的资源要等到其文件描述符的最后一个引用被释放时才会释放，因此强烈不建议不加考虑地通过 fork()、dup() 等方式创建对 VM 的额外引用，否则可能产生不期望的副作用，例如由 VM 进程分配并代表其分配的内存，在 VM 关闭时可能不会被释放或计入统计。)


3. Extensions(3. 扩展)
======================

As of Linux 2.6.22, the KVM ABI has been stabilized: no backward
incompatible change are allowed.  However, there is an extension
facility that allows backward-compatible extensions to the API to be
queried and used.
(自 Linux 2.6.22 起，KVM ABI 已经稳定：不允许再出现向后不兼容的变更。不过，KVM 提供了一种扩展机制，允许以向后兼容的方式对 API 进行扩展，并可供查询和使用。)

The extension mechanism is not based on the Linux version number.
Instead, kvm defines extension identifiers and a facility to query
whether a particular extension identifier is available.  If it is, a
set of ioctls is available for application use.
(扩展机制不基于 Linux 版本号。相反，kvm 定义了扩展标识符，并提供查询某个特定扩展标识符是否可用的机制。如果可用，则有一组 ioctl 供应用程序使用。)


4. API description(4. API 描述)
===============================

This section describes ioctls that can be used to control kvm guests.
For each ioctl, the following information is provided along with a
description:
(本节描述可用于控制 kvm 客户机(guest)的各个 ioctl。除描述之外，每个 ioctl 还提供以下信息：)

  Capability:
      which KVM extension provides this ioctl.  Can be 'basic',
      which means that is will be provided by any kernel that supports
      API version 12 (see :ref:`KVM_GET_API_VERSION <KVM_GET_API_VERSION>`),
      or a KVM_CAP_xyz constant that can be checked with
      :ref:`KVM_CHECK_EXTENSION <KVM_CHECK_EXTENSION>`.
      (能力：提供该 ioctl 的 KVM 扩展。可以是 'basic'，即任何支持 API 版本 12 的内核都会提供(参见 KVM_GET_API_VERSION 一节)；也可以是 KVM_CAP_xyz 常量，可通过 KVM_CHECK_EXTENSION 查询。)

  Architectures:
      which instruction set architectures provide this ioctl.
      x86 includes both i386 and x86_64.
      (架构：提供该 ioctl 的指令集架构。x86 同时包括 i386 和 x86_64。)

  Type:
      system, vm, or vcpu.
      (类型：system、vm 或 vcpu。)

  Parameters:
      what parameters are accepted by the ioctl.
      (参数：该 ioctl 接受哪些参数。)

  Returns:
      the return value.  General error numbers (EBADF, ENOMEM, EINVAL)
      are not detailed, but errors with specific meanings are.
      (返回值：一般性错误码(EBADF、ENOMEM、EINVAL)不再赘述，但具有特定含义的错误会加以说明。)


.. _KVM_GET_API_VERSION:

4.1 KVM_GET_API_VERSION(4.1 KVM_GET_API_VERSION)
------------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: none
  (参数：无)
:Returns: the constant KVM_API_VERSION (=12)
  (返回值：常量 KVM_API_VERSION (=12))

This identifies the API version as the stable kvm API. It is not
expected that this number will change.  However, Linux 2.6.20 and
2.6.21 report earlier versions; these are not documented and not
supported.  Applications should refuse to run if KVM_GET_API_VERSION
returns a value other than 12.  If this check passes, all ioctls
described as 'basic' will be available.
(本调用将 API 版本标识为稳定的 kvm API。预计该数字不会变化。不过，Linux 2.6.20 和 2.6.21 报告的是更早的版本；这些版本未在文档中说明，也不受支持。如果 KVM_GET_API_VERSION 返回的值不是 12，应用程序应当拒绝运行。若此检查通过，则所有描述为 'basic' 的 ioctl 都可用。)


4.2 KVM_CREATE_VM(4.2 KVM_CREATE_VM)
------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: machine type identifier (KVM_VM_*)
  (参数：机器类型标识符 (KVM_VM_*))
:Returns: a VM fd that can be used to control the new virtual machine.
  (返回值：一个可用于控制新虚拟机的 VM fd。)

The new VM has no virtual cpus and no memory.
You probably want to use 0 as machine type.
(新建的 VM 没有虚拟 cpu，也没有内存。机器类型很可能应该用 0。)

X86:
^^^^

Supported X86 VM types can be queried via KVM_CAP_VM_TYPES.
(支持的 x86 VM 类型可以通过 KVM_CAP_VM_TYPES 查询。)

S390:
^^^^^

In order to create user controlled virtual machines on S390, check
KVM_CAP_S390_UCONTROL and use the flag KVM_VM_S390_UCONTROL as
privileged user (CAP_SYS_ADMIN).
(要在 S390 上创建用户可控的虚拟机，请以特权用户(具备 CAP_SYS_ADMIN)身份检查 KVM_CAP_S390_UCONTROL 并使用标志 KVM_VM_S390_UCONTROL。)

MIPS:
^^^^^

To use hardware assisted virtualization on MIPS (VZ ASE) rather than
the default trap & emulate implementation (which changes the virtual
memory layout to fit in user mode), check KVM_CAP_MIPS_VZ and use the
flag KVM_VM_MIPS_VZ.
(要在 MIPS 上使用硬件辅助虚拟化(VZ ASE)而不是默认的陷入与模拟(trap & emulate)实现(后者会改变虚拟内存布局以适应用户态)，请检查 KVM_CAP_MIPS_VZ 并使用标志 KVM_VM_MIPS_VZ。)

ARM64:
^^^^^^

On arm64, the physical address size for a VM (IPA Size limit) is limited
to 40bits by default. The limit can be configured if the host supports the
extension KVM_CAP_ARM_VM_IPA_SIZE. When supported, use
KVM_VM_TYPE_ARM_IPA_SIZE(IPA_Bits) to set the size in the machine type
identifier, where IPA_Bits is the maximum width of any physical
address used by the VM. The IPA_Bits is encoded in bits[7-0] of the
machine type identifier.
(在 arm64 上，VM 的物理地址位宽(IPA Size limit)默认限制为 40 位。如果宿主机支持扩展 KVM_CAP_ARM_VM_IPA_SIZE，则可以配置该限制。在支持的情况下，使用 KVM_VM_TYPE_ARM_IPA_SIZE(IPA_Bits) 将位宽设置到机器类型标识符中，其中 IPA_Bits 是 VM 所用任何物理地址的最大位宽。IPA_Bits 编码在机器类型标识符的 bits[7-0] 中。)

e.g, to configure a guest to use 48bit physical address size::

    vm_fd = ioctl(dev_fd, KVM_CREATE_VM, KVM_VM_TYPE_ARM_IPA_SIZE(48));

(例如，要将客户机配置为使用 48 位物理地址位宽：)

The requested size (IPA_Bits) must be:
(请求的位宽 (IPA_Bits) 必须是：)

 ==   =========================================================
  0   Implies default size, 40bits (for backward compatibility)
      (表示默认位宽，40 位(为了向后兼容))
  N   Implies N bits, where N is a positive integer such that,
      32 <= N <= Host_IPA_Limit
      (表示 N 位，其中 N 是满足 32 <= N <= Host_IPA_Limit 的正整数)
 ==   =========================================================

Host_IPA_Limit is the maximum possible value for IPA_Bits on the host and
is dependent on the CPU capability and the kernel configuration. The limit can
be retrieved using KVM_CAP_ARM_VM_IPA_SIZE of the KVM_CHECK_EXTENSION
ioctl() at run-time.
(Host_IPA_Limit 是宿主机上 IPA_Bits 可能的最大值，取决于 CPU 能力和内核配置。该限制可在运行时通过 KVM_CHECK_EXTENSION ioctl() 的 KVM_CAP_ARM_VM_IPA_SIZE 获取。)

Creation of the VM will fail if the requested IPA size (whether it is
implicit or explicit) is unsupported on the host.
(如果请求的 IPA 位宽(无论是隐式还是显式指定)在宿主机上不受支持，VM 的创建将失败。)

Please note that configuring the IPA size does not affect the capability
exposed by the guest CPUs in ID_AA64MMFR0_EL1[PARange]. It only affects
size of the address translated by the stage2 level (guest physical to
host physical address translations).
(请注意，配置 IPA 位宽不会影响客户机 CPU 在 ID_AA64MMFR0_EL1[PARange] 中暴露的能力。它只影响 stage2 层级转换的地址大小(客户机物理地址到宿主机物理地址的转换)。)


4.3 KVM_GET_MSR_INDEX_LIST, KVM_GET_MSR_FEATURE_INDEX_LIST(4.3 KVM_GET_MSR_INDEX_LIST, KVM_GET_MSR_FEATURE_INDEX_LIST)
----------------------------------------------------------------------------------------------------------------------

:Capability: basic, KVM_CAP_GET_MSR_FEATURES for KVM_GET_MSR_FEATURE_INDEX_LIST
  (能力：basic；KVM_GET_MSR_FEATURE_INDEX_LIST 需要 KVM_CAP_GET_MSR_FEATURES。)
:Architectures: x86
  (架构：x86)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: struct kvm_msr_list (in/out)
  (参数：struct kvm_msr_list (in/out))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

Errors:
(错误：)

  ======     ============================================================
  EFAULT     the msr index list cannot be read from or written to
             (无法读取或写入 MSR 索引列表)
  E2BIG      the msr index list is too big to fit in the array specified by
             the user.
             (MSR 索引列表过大，无法放入用户指定的数组。)
  ======     ============================================================

::

  struct kvm_msr_list {
	__u32 nmsrs; /* number of msrs in entries */
	__u32 indices[0];
  };

The user fills in the size of the indices array in nmsrs, and in return
kvm adjusts nmsrs to reflect the actual number of msrs and fills in the
indices array with their numbers.
(用户在 nmsrs 中填入 indices 数组的大小；作为返回，kvm 会把 nmsrs 调整为 MSR 的实际数量，并把它们的编号填入 indices 数组。)

KVM_GET_MSR_INDEX_LIST returns the guest msrs that are supported.  The list
varies by kvm version and host processor, but does not change otherwise.
(KVM_GET_MSR_INDEX_LIST 返回受支持的客户机 MSR。该列表随 kvm 版本和宿主处理器不同而不同，除此之外不会改变。)

Note: if kvm indicates supports MCE (KVM_CAP_MCE), then the MCE bank MSRs are
not returned in the MSR list, as different vcpus can have a different number
of banks, as set via the KVM_X86_SETUP_MCE ioctl.
(注意：如果 kvm 声明支持 MCE (KVM_CAP_MCE)，MSR 列表中不会返回 MCE bank MSR，因为不同 vcpu 的 bank 数量可以不同，bank 数量通过 KVM_X86_SETUP_MCE ioctl 设置。)

KVM_GET_MSR_FEATURE_INDEX_LIST returns the list of MSRs that can be passed
to the KVM_GET_MSRS system ioctl.  This lets userspace probe host capabilities
and processor features that are exposed via MSRs (e.g., VMX capabilities).
This list also varies by kvm version and host processor, but does not change
otherwise.
(KVM_GET_MSR_FEATURE_INDEX_LIST 返回可以传给 KVM_GET_MSRS 系统 ioctl 的 MSR 列表。这让用户态能够探测通过 MSR 暴露出来的宿主机能力与处理器特性(如 VMX 能力)。该列表同样随 kvm 版本和宿主处理器不同而不同，除此之外不会改变。)


.. _KVM_CHECK_EXTENSION:

4.4 KVM_CHECK_EXTENSION(4.4 KVM_CHECK_EXTENSION)
------------------------------------------------

:Capability: basic, KVM_CAP_CHECK_EXTENSION_VM for vm ioctl
  (能力：basic；vm ioctl 需要 KVM_CAP_CHECK_EXTENSION_VM。)
:Architectures: all
  (架构：全部)
:Type: system ioctl, vm ioctl
  (类型：system ioctl、vm ioctl)
:Parameters: extension identifier (KVM_CAP_*)
  (参数：扩展标识符 (KVM_CAP_*))
:Returns: 0 if unsupported; 1 (or some other positive integer) if supported
  (返回值：不支持时为 0；支持时为 1(或其他某个正整数))

The API allows the application to query about extensions to the core
kvm API.  Userspace passes an extension identifier (an integer) and
receives an integer that describes the extension availability.
Generally 0 means no and 1 means yes, but some extensions may report
additional information in the integer return value.
(该 API 允许应用程序查询核心 kvm API 的扩展情况。用户态传入一个扩展标识符(整数)，并收到一个描述该扩展可用性的整数。通常 0 表示不支持、1 表示支持，但某些扩展可能在整数返回值中报告额外信息。)

Based on their initialization different VMs may have different capabilities.
It is thus encouraged to use the vm ioctl to query for capabilities (available
with KVM_CAP_CHECK_EXTENSION_VM on the vm fd)
(由于初始化方式不同，不同 VM 可能拥有不同的能力。因此建议使用 vm ioctl 来查询能力(在 vm fd 上可通过 KVM_CAP_CHECK_EXTENSION_VM 使用)。)

4.5 KVM_GET_VCPU_MMAP_SIZE(4.5 KVM_GET_VCPU_MMAP_SIZE)
------------------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: none
  (参数：无)
:Returns: size of vcpu mmap area, in bytes
  (返回值：vcpu mmap 区域的大小，以字节为单位)

The KVM_RUN ioctl (cf.) communicates with userspace via a shared
memory region.  This ioctl returns the size of that region.  See the
KVM_RUN documentation for details.
(KVM_RUN ioctl(参见 KVM_RUN 一节)通过一块共享内存区域与用户态通信。此 ioctl 返回该区域的大小。详见 KVM_RUN 的文档。)

Besides the size of the KVM_RUN communication region, other areas of
the VCPU file descriptor can be mmap-ed, including:
(除 KVM_RUN 通信区域之外，VCPU 文件描述符的其他区域也可以被 mmap，包括：)

- if KVM_CAP_COALESCED_MMIO is available, a page at
  KVM_COALESCED_MMIO_PAGE_OFFSET * PAGE_SIZE; for historical reasons,
  this page is included in the result of KVM_GET_VCPU_MMAP_SIZE.
  KVM_CAP_COALESCED_MMIO is not documented yet.
  (如果 KVM_CAP_COALESCED_MMIO 可用，则在 KVM_COALESCED_MMIO_PAGE_OFFSET * PAGE_SIZE 处有一页；出于历史原因，这一页包含在 KVM_GET_VCPU_MMAP_SIZE 的结果中。KVM_CAP_COALESCED_MMIO 尚无文档。)

- if KVM_CAP_DIRTY_LOG_RING is available, a number of pages at
  KVM_DIRTY_LOG_PAGE_OFFSET * PAGE_SIZE.  For more information on
  KVM_CAP_DIRTY_LOG_RING, see :ref:`KVM_CAP_DIRTY_LOG_RING`.
  (如果 KVM_CAP_DIRTY_LOG_RING 可用，则在 KVM_DIRTY_LOG_PAGE_OFFSET * PAGE_SIZE 处有若干页。关于 KVM_CAP_DIRTY_LOG_RING 的更多信息，参见 KVM_CAP_DIRTY_LOG_RING 一节。)


4.7 KVM_CREATE_VCPU(4.7 KVM_CREATE_VCPU)
----------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: vcpu id (apic id on x86)
  (参数：vcpu id(x86 上为 apic id))
:Returns: vcpu fd on success, -1 on error
  (返回值：成功时为 vcpu fd，出错时为 -1)

This API adds a vcpu to a virtual machine. No more than max_vcpus may be added.
The vcpu id is an integer in the range [0, max_vcpu_id).
(该 API 向虚拟机添加一个 vcpu。添加的 vcpu 数量不能超过 max_vcpus。vcpu id 是 [0, max_vcpu_id) 范围内的整数。)

The recommended max_vcpus value can be retrieved using the KVM_CAP_NR_VCPUS of
the KVM_CHECK_EXTENSION ioctl() at run-time.
The maximum possible value for max_vcpus can be retrieved using the
KVM_CAP_MAX_VCPUS of the KVM_CHECK_EXTENSION ioctl() at run-time.
(推荐的 max_vcpus 值可在运行时通过 KVM_CHECK_EXTENSION ioctl() 的 KVM_CAP_NR_VCPUS 获取。max_vcpus 的最大可能值可在运行时通过 KVM_CHECK_EXTENSION ioctl() 的 KVM_CAP_MAX_VCPUS 获取。)

If the KVM_CAP_NR_VCPUS does not exist, you should assume that max_vcpus is 4
cpus max.
If the KVM_CAP_MAX_VCPUS does not exist, you should assume that max_vcpus is
same as the value returned from KVM_CAP_NR_VCPUS.
(如果 KVM_CAP_NR_VCPUS 不存在，应假定 max_vcpus 最多为 4 个 cpu。如果 KVM_CAP_MAX_VCPUS 不存在，应假定 max_vcpus 与 KVM_CAP_NR_VCPUS 返回的值相同。)

The maximum possible value for max_vcpu_id can be retrieved using the
KVM_CAP_MAX_VCPU_ID of the KVM_CHECK_EXTENSION ioctl() at run-time.
(max_vcpu_id 的最大可能值可在运行时通过 KVM_CHECK_EXTENSION ioctl() 的 KVM_CAP_MAX_VCPU_ID 获取。)

If the KVM_CAP_MAX_VCPU_ID does not exist, you should assume that max_vcpu_id
is the same as the value returned from KVM_CAP_MAX_VCPUS.
(如果 KVM_CAP_MAX_VCPU_ID 不存在，应假定 max_vcpu_id 与 KVM_CAP_MAX_VCPUS 返回的值相同。)

On powerpc using book3s_hv mode, the vcpus are mapped onto virtual
threads in one or more virtual CPU cores.  (This is because the
hardware requires all the hardware threads in a CPU core to be in the
same partition.)  The KVM_CAP_PPC_SMT capability indicates the number
of vcpus per virtual core (vcore).  The vcore id is obtained by
dividing the vcpu id by the number of vcpus per vcore.  The vcpus in a
given vcore will always be in the same physical core as each other
(though that might be a different physical core from time to time).
Userspace can control the threading (SMT) mode of the guest by its
allocation of vcpu ids.  For example, if userspace wants
single-threaded guest vcpus, it should make all vcpu ids be a multiple
of the number of vcpus per vcore.
(在使用 book3s_hv 模式的 powerpc 上，vcpu 被映射到一个或多个虚拟 CPU 核心中的虚拟线程上。(这是因为硬件要求 CPU 核心中的所有硬件线程都处于同一个分区。)KVM_CAP_PPC_SMT 能力表示每个虚拟核心 (vcore) 的 vcpu 数量。vcore id 由 vcpu id 除以每个 vcore 的 vcpu 数量得到。给定 vcore 中的各个 vcpu 始终位于彼此相同的物理核心上(不过该物理核心可能不时变化)。用户态可以通过分配 vcpu id 来控制客户机的线程 (SMT) 模式。例如，如果用户态想要单线程的客户机 vcpu，就应让所有 vcpu id 都是每 vcore vcpu 数量的倍数。)

For virtual cpus that have been created with S390 user controlled virtual
machines, the resulting vcpu fd can be memory mapped at page offset
KVM_S390_SIE_PAGE_OFFSET in order to obtain a memory map of the virtual
cpu's hardware control block.
(对于通过 S390 用户可控虚拟机创建的虚拟 cpu，可以在页偏移 KVM_S390_SIE_PAGE_OFFSET 处对得到的 vcpu fd 进行内存映射，以获得该虚拟 cpu 硬件控制块的内存映射。)


4.8 KVM_GET_DIRTY_LOG(4.8 KVM_GET_DIRTY_LOG)
--------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_dirty_log (in/out)
  (参数：struct kvm_dirty_log (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  /* for KVM_GET_DIRTY_LOG */
  struct kvm_dirty_log {
	__u32 slot;
	__u32 padding;
	union {
		void __user *dirty_bitmap; /* one bit per page */
		__u64 padding;
	};
  };

Given a memory slot, return a bitmap containing any pages dirtied
since the last call to this ioctl.  Bit 0 is the first page in the
memory slot.  Ensure the entire structure is cleared to avoid padding
issues.
(给定一个内存 slot，返回一个位图，其中包含自上次调用此 ioctl 以来被弄脏的所有页。位 0 对应内存 slot 中的第一页。请确保整个结构体被清零，以避免填充 (padding) 问题。)

If KVM_CAP_MULTI_ADDRESS_SPACE is available, bits 16-31 of slot field specifies
the address space for which you want to return the dirty bitmap.  See
KVM_SET_USER_MEMORY_REGION for details on the usage of slot field.
(如果 KVM_CAP_MULTI_ADDRESS_SPACE 可用，slot 字段的 bits 16-31 指定要返回其脏位图的地址空间。slot 字段用法的详细信息参见 KVM_SET_USER_MEMORY_REGION。)

The bits in the dirty bitmap are cleared before the ioctl returns, unless
KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 is enabled.  For more information,
see the description of the capability.
(脏位图中的各位会在 ioctl 返回之前被清除，除非已启用 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2。更多信息参见该能力的说明。)

Note that the Xen shared_info page, if configured, shall always be assumed
to be dirty. KVM will not explicitly mark it such.
(注意：如果配置了 Xen shared_info 页，应始终假定它是脏的。KVM 不会显式地将其标记为脏。)


4.10 KVM_RUN(4.10 KVM_RUN)
--------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: none
  (参数：无)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Errors:
(错误：)

  =======    ==============================================================
  EINTR      an unmasked signal is pending
             (有一个未被屏蔽的信号处于待决状态)
  ENOEXEC    the vcpu hasn't been initialized or the guest tried to execute
             instructions from device memory (arm64)
             (vcpu 尚未初始化，或客户机试图从设备内存执行指令 (arm64))
  ENOSYS     data abort outside memslots with no syndrome info and
             KVM_CAP_ARM_NISV_TO_USER not enabled (arm64)
             (在 memslot 之外发生数据中止且没有综合症信息，并且未启用 KVM_CAP_ARM_NISV_TO_USER (arm64))
  EPERM      SVE feature set but not finalized (arm64)
             (设置了 SVE 特性但尚未最终确定 (arm64))
  =======    ==============================================================

This ioctl is used to run a guest virtual cpu.  While there are no
explicit parameters, there is an implicit parameter block that can be
obtained by mmap()ing the vcpu fd at offset 0, with the size given by
KVM_GET_VCPU_MMAP_SIZE.  The parameter block is formatted as a 'struct
kvm_run' (see below).
(此 ioctl 用于运行客户机虚拟 cpu。虽然没有显式参数，但存在一个隐式参数块：将 vcpu fd 在偏移 0 处 mmap()，大小由 KVM_GET_VCPU_MMAP_SIZE 给出，即可获得该参数块。参数块的格式是 'struct kvm_run'(见下文)。)


4.11 KVM_GET_REGS(4.11 KVM_GET_REGS)
------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all except arm64
  (架构：除 arm64 外的全部)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_regs (out)
  (参数：struct kvm_regs (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Reads the general purpose registers from the vcpu.
(从 vcpu 读取通用寄存器。)

::

  /* x86 */
  struct kvm_regs {
	/* out (KVM_GET_REGS) / in (KVM_SET_REGS) */
	__u64 rax, rbx, rcx, rdx;
	__u64 rsi, rdi, rsp, rbp;
	__u64 r8,  r9,  r10, r11;
	__u64 r12, r13, r14, r15;
	__u64 rip, rflags;
  };

  /* mips */
  struct kvm_regs {
	/* out (KVM_GET_REGS) / in (KVM_SET_REGS) */
	__u64 gpr[32];
	__u64 hi;
	__u64 lo;
	__u64 pc;
  };

  /* LoongArch */
  struct kvm_regs {
	/* out (KVM_GET_REGS) / in (KVM_SET_REGS) */
	unsigned long gpr[32];
	unsigned long pc;
  };


4.12 KVM_SET_REGS(4.12 KVM_SET_REGS)
------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all except arm64
  (架构：除 arm64 外的全部)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_regs (in)
  (参数：struct kvm_regs (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Writes the general purpose registers into the vcpu.
(向 vcpu 写入通用寄存器。)

See KVM_GET_REGS for the data structure.
(数据结构参见 KVM_GET_REGS。)


4.13 KVM_GET_SREGS(4.13 KVM_GET_SREGS)
--------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86, ppc
  (架构：x86、ppc)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_sregs (out)
  (参数：struct kvm_sregs (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Reads special registers from the vcpu.
(从 vcpu 读取特殊寄存器。)

::

  /* x86 */
  struct kvm_sregs {
	struct kvm_segment cs, ds, es, fs, gs, ss;
	struct kvm_segment tr, ldt;
	struct kvm_dtable gdt, idt;
	__u64 cr0, cr2, cr3, cr4, cr8;
	__u64 efer;
	__u64 apic_base;
	__u64 interrupt_bitmap[(KVM_NR_INTERRUPTS + 63) / 64];
  };

  /* ppc -- see arch/powerpc/include/uapi/asm/kvm.h */

interrupt_bitmap is a bitmap of pending external interrupts.  At most
one bit may be set.  This interrupt has been acknowledged by the APIC
but not yet injected into the cpu core.
(interrupt_bitmap 是待决外部中断的位图。最多只能有一个位被置位。该中断已被 APIC 应答，但尚未注入 cpu 核心。)


4.14 KVM_SET_SREGS(4.14 KVM_SET_SREGS)
--------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86, ppc
  (架构：x86、ppc)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_sregs (in)
  (参数：struct kvm_sregs (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Writes special registers into the vcpu.  See KVM_GET_SREGS for the
data structures.
(向 vcpu 写入特殊寄存器。数据结构参见 KVM_GET_SREGS。)


4.15 KVM_TRANSLATE(4.15 KVM_TRANSLATE)
--------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_translation (in/out)
  (参数：struct kvm_translation (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Translates a virtual address according to the vcpu's current address
translation mode.
(按照 vcpu 当前的地址转换模式转换一个虚拟地址。)

::

  struct kvm_translation {
	/* in */
	__u64 linear_address;

	/* out */
	__u64 physical_address;
	__u8  valid;
	__u8  writeable;
	__u8  usermode;
	__u8  pad[5];
  };


4.16 KVM_INTERRUPT(4.16 KVM_INTERRUPT)
--------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86, ppc, mips, riscv, loongarch
  (架构：x86、ppc、mips、riscv、loongarch)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_interrupt (in)
  (参数：struct kvm_interrupt (in))
:Returns: 0 on success, negative on failure.
  (返回值：成功时为 0，失败时为负值。)

Queues a hardware interrupt vector to be injected.
(将一个待注入的硬件中断向量排队。)

::

  /* for KVM_INTERRUPT */
  struct kvm_interrupt {
	/* in */
	__u32 irq;
  };

X86:
^^^^

:Returns:
  (返回值：)

	========= ===================================
	  0       on success, (成功时)
	 -EEXIST  if an interrupt is already enqueued (已有一个中断在排队中)
	 -EINVAL  the irq number is invalid (irq 编号无效)
	 -ENXIO   if the PIC is in the kernel (PIC 位于内核中时)
	 -EFAULT  if the pointer is invalid (指针无效时)
	========= ===================================

Note 'irq' is an interrupt vector, not an interrupt pin or line. This
ioctl is useful if the in-kernel PIC is not used.
(注意 'irq' 是中断向量，不是中断引脚或中断线。在不使用内核内 PIC 的情况下，此 ioctl 很有用。)

PPC:
^^^^

Queues an external interrupt to be injected. This ioctl is overloaded
with 3 different irq values:
(将一个待注入的外部中断排队。此 ioctl 以 3 种不同的 irq 取值复用：)

a) KVM_INTERRUPT_SET

   This injects an edge type external interrupt into the guest once it's ready
   to receive interrupts. When injected, the interrupt is done.
   (向客户机注入一个边沿触发的外部中断，前提是客户机已准备好接收中断。注入之后，该中断即告结束。)

b) KVM_INTERRUPT_UNSET

   This unsets any pending interrupt.
   (取消任何待决的中断。)

   Only available with KVM_CAP_PPC_UNSET_IRQ.
   (仅在 KVM_CAP_PPC_UNSET_IRQ 下可用。)

c) KVM_INTERRUPT_SET_LEVEL

   This injects a level type external interrupt into the guest context. The
   interrupt stays pending until a specific ioctl with KVM_INTERRUPT_UNSET
   is triggered.
   (向客户机上下文注入一个电平触发的外部中断。该中断保持待决状态，直到触发一个带 KVM_INTERRUPT_UNSET 的特定 ioctl 为止。)

   Only available with KVM_CAP_PPC_IRQ_LEVEL.
   (仅在 KVM_CAP_PPC_IRQ_LEVEL 下可用。)

Note that any value for 'irq' other than the ones stated above is invalid
and incurs unexpected behavior.
(注意：'irq' 取上述取值之外的任何值都是无效的，会导致意外行为。)

This is an asynchronous vcpu ioctl and can be invoked from any thread.
(这是异步 vcpu ioctl，可以从任意线程调用。)

MIPS:
^^^^^

Queues an external interrupt to be injected into the virtual CPU. A negative
interrupt number dequeues the interrupt.
(将一个待注入虚拟 CPU 的外部中断排队。负的中断编号表示将该中断出队。)

This is an asynchronous vcpu ioctl and can be invoked from any thread.
(这是异步 vcpu ioctl，可以从任意线程调用。)

RISC-V:
^^^^^^^

Queues an external interrupt to be injected into the virtual CPU. This ioctl
is overloaded with 2 different irq values:
(将一个待注入虚拟 CPU 的外部中断排队。此 ioctl 以 2 种不同的 irq 取值复用：)

a) KVM_INTERRUPT_SET

   This sets external interrupt for a virtual CPU and it will receive
   once it is ready.
   (为虚拟 CPU 设置外部中断，虚拟 CPU 就绪后即会接收。)

b) KVM_INTERRUPT_UNSET

   This clears pending external interrupt for a virtual CPU.
   (清除虚拟 CPU 上待决的外部中断。)

This is an asynchronous vcpu ioctl and can be invoked from any thread.
(这是异步 vcpu ioctl，可以从任意线程调用。)

LOONGARCH:
^^^^^^^^^^

Queues an external interrupt to be injected into the virtual CPU. A negative
interrupt number dequeues the interrupt.
(将一个待注入虚拟 CPU 的外部中断排队。负的中断编号表示将该中断出队。)

This is an asynchronous vcpu ioctl and can be invoked from any thread.
(这是异步 vcpu ioctl，可以从任意线程调用。)


4.18 KVM_GET_MSRS(4.18 KVM_GET_MSRS)
------------------------------------

:Capability: basic (vcpu), KVM_CAP_GET_MSR_FEATURES (system)
  (能力：basic (vcpu)、KVM_CAP_GET_MSR_FEATURES (system))
:Architectures: x86
  (架构：x86)
:Type: system ioctl, vcpu ioctl
  (类型：system ioctl、vcpu ioctl)
:Parameters: struct kvm_msrs (in/out)
  (参数：struct kvm_msrs (in/out))
:Returns: number of msrs successfully returned;
          -1 on error
  (返回值：成功返回的 MSR 数量；出错时为 -1)

When used as a system ioctl:
Reads the values of MSR-based features that are available for the VM.  This
is similar to KVM_GET_SUPPORTED_CPUID, but it returns MSR indices and values.
The list of msr-based features can be obtained using KVM_GET_MSR_FEATURE_INDEX_LIST
in a system ioctl.
(作为系统 ioctl 使用时：读取 VM 可用的基于 MSR 的特性的值。这与 KVM_GET_SUPPORTED_CPUID 类似，但返回的是 MSR 索引和值。基于 MSR 的特性列表可通过系统 ioctl 使用 KVM_GET_MSR_FEATURE_INDEX_LIST 获取。)

When used as a vcpu ioctl:
Reads model-specific registers from the vcpu.  Supported msr indices can
be obtained using KVM_GET_MSR_INDEX_LIST in a system ioctl.
(作为 vcpu ioctl 使用时：从 vcpu 读取模型特定寄存器 (MSR)。支持的 MSR 索引可通过系统 ioctl 使用 KVM_GET_MSR_INDEX_LIST 获取。)

::

  struct kvm_msrs {
	__u32 nmsrs; /* number of msrs in entries */
	__u32 pad;

	struct kvm_msr_entry entries[0];
  };

  struct kvm_msr_entry {
	__u32 index;
	__u32 reserved;
	__u64 data;
  };

Application code should set the 'nmsrs' member (which indicates the
size of the entries array) and the 'index' member of each array entry.
kvm will fill in the 'data' member.
(应用程序应设置 'nmsrs' 成员(它指明 entries 数组的大小)以及每个数组条目的 'index' 成员。kvm 会填入 'data' 成员。)


4.19 KVM_SET_MSRS(4.19 KVM_SET_MSRS)
------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_msrs (in)
  (参数：struct kvm_msrs (in))
:Returns: number of msrs successfully set (see below), -1 on error
  (返回值：成功设置的 MSR 数量(见下文)，出错时为 -1)

Writes model-specific registers to the vcpu.  See KVM_GET_MSRS for the
data structures.
(向 vcpu 写入模型特定寄存器 (MSR)。数据结构参见 KVM_GET_MSRS。)

Application code should set the 'nmsrs' member (which indicates the
size of the entries array), and the 'index' and 'data' members of each
array entry.
(应用程序应设置 'nmsrs' 成员(它指明 entries 数组的大小)，以及每个数组条目的 'index' 和 'data' 成员。)

It tries to set the MSRs in array entries[] one by one. If setting an MSR
fails, e.g., due to setting reserved bits, the MSR isn't supported/emulated
by KVM, etc..., it stops processing the MSR list and returns the number of
MSRs that have been set successfully.
(它逐个尝试设置数组 entries[] 中的 MSR。如果设置某个 MSR 失败(例如由于设置了保留位、KVM 不支持/不模拟该 MSR 等)，它会停止处理 MSR 列表，并返回已成功设置的 MSR 数量。)


4.20 KVM_SET_CPUID(4.20 KVM_SET_CPUID)
--------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_cpuid (in)
  (参数：struct kvm_cpuid (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Defines the vcpu responses to the cpuid instruction.  Applications
should use the KVM_SET_CPUID2 ioctl if available.
(定义 vcpu 对 cpuid 指令的响应。如果可用，应用程序应改用 KVM_SET_CPUID2 ioctl。)

Caveat emptor:
(后果自负(买者当心)：)
  - If this IOCTL fails, KVM gives no guarantees that previous valid CPUID
    configuration (if there is) is not corrupted. Userspace can get a copy
    of the resulting CPUID configuration through KVM_GET_CPUID2 in case.
    (如果此 IOCTL 失败，KVM 不保证之前有效的 CPUID 配置(如果存在)没有被打乱。万一发生这种情况，用户态可以通过 KVM_GET_CPUID2 获取最终 CPUID 配置的副本。)
  - Using KVM_SET_CPUID{,2} after KVM_RUN, i.e. changing the guest vCPU model
    after running the guest, may cause guest instability.
    (在 KVM_RUN 之后使用 KVM_SET_CPUID{,2}，也就是在运行客户机之后更改客户机 vCPU 模型，可能导致客户机不稳定。)
  - Using heterogeneous CPUID configurations, modulo APIC IDs, topology, etc...
    may cause guest instability.
    (使用异构的 CPUID 配置(APIC ID、拓扑等方面的差异除外)可能导致客户机不稳定。)

::

  struct kvm_cpuid_entry {
	__u32 function;
	__u32 eax;
	__u32 ebx;
	__u32 ecx;
	__u32 edx;
	__u32 padding;
  };

  /* for KVM_SET_CPUID */
  struct kvm_cpuid {
	__u32 nent;
	__u32 padding;
	struct kvm_cpuid_entry entries[0];
  };


4.21 KVM_SET_SIGNAL_MASK(4.21 KVM_SET_SIGNAL_MASK)
--------------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: all
  (架构：全部)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_signal_mask (in)
  (参数：struct kvm_signal_mask (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Defines which signals are blocked during execution of KVM_RUN.  This
signal mask temporarily overrides the threads signal mask.  Any
unblocked signal received (except SIGKILL and SIGSTOP, which retain
their traditional behaviour) will cause KVM_RUN to return with -EINTR.
(定义在 KVM_RUN 执行期间屏蔽哪些信号。该信号掩码会临时覆盖线程的信号掩码。收到的任何未被屏蔽的信号(保留传统行为的 SIGKILL 和 SIGSTOP 除外)都会使 KVM_RUN 以 -EINTR 返回。)

Note the signal will only be delivered if not blocked by the original
signal mask.
(注意：只有未被原始信号掩码屏蔽的信号才会被送达。)

::

  /* for KVM_SET_SIGNAL_MASK */
  struct kvm_signal_mask {
	__u32 len;
	__u8  sigset[0];
  };


4.22 KVM_GET_FPU(4.22 KVM_GET_FPU)
----------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86, loongarch
  (架构：x86、loongarch)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_fpu (out)
  (参数：struct kvm_fpu (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Reads the floating point state from the vcpu.
(从 vcpu 读取浮点状态。)

::

  /* x86: for KVM_GET_FPU and KVM_SET_FPU */
  struct kvm_fpu {
	__u8  fpr[8][16];
	__u16 fcw;
	__u16 fsw;
	__u8  ftwx;  /* in fxsave format */
	__u8  pad1;
	__u16 last_opcode;
	__u64 last_ip;
	__u64 last_dp;
	__u8  xmm[16][16];
	__u32 mxcsr;
	__u32 pad2;
  };

  /* LoongArch: for KVM_GET_FPU and KVM_SET_FPU */
  struct kvm_fpu {
	__u32 fcsr;
	__u64 fcc;
	struct kvm_fpureg {
		__u64 val64[4];
	}fpr[32];
  };


4.23 KVM_SET_FPU(4.23 KVM_SET_FPU)
----------------------------------

:Capability: basic
  (能力：basic)
:Architectures: x86, loongarch
  (架构：x86、loongarch)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_fpu (in)
  (参数：struct kvm_fpu (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Writes the floating point state to the vcpu.
(向 vcpu 写入浮点状态。)

::

  /* x86: for KVM_GET_FPU and KVM_SET_FPU */
  struct kvm_fpu {
	__u8  fpr[8][16];
	__u16 fcw;
	__u16 fsw;
	__u8  ftwx;  /* in fxsave format */
	__u8  pad1;
	__u16 last_opcode;
	__u64 last_ip;
	__u64 last_dp;
	__u8  xmm[16][16];
	__u32 mxcsr;
	__u32 pad2;
  };

  /* LoongArch: for KVM_GET_FPU and KVM_SET_FPU */
  struct kvm_fpu {
	__u32 fcsr;
	__u64 fcc;
	struct kvm_fpureg {
		__u64 val64[4];
	}fpr[32];
  };


4.24 KVM_CREATE_IRQCHIP(4.24 KVM_CREATE_IRQCHIP)
------------------------------------------------

:Capability: KVM_CAP_IRQCHIP, KVM_CAP_S390_IRQCHIP (s390)
  (能力：KVM_CAP_IRQCHIP、KVM_CAP_S390_IRQCHIP (s390))
:Architectures: x86, arm64, s390
  (架构：x86、arm64、s390)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: none
  (参数：无)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Creates an interrupt controller model in the kernel.
On x86, creates a virtual ioapic, a virtual PIC (two PICs, nested), and sets up
future vcpus to have a local APIC.  IRQ routing for GSIs 0-15 is set to both
PIC and IOAPIC; GSI 16-23 only go to the IOAPIC.
On arm64, a GICv2 is created. Any other GIC versions require the usage of
KVM_CREATE_DEVICE, which also supports creating a GICv2.  Using
KVM_CREATE_DEVICE is preferred over KVM_CREATE_IRQCHIP for GICv2.
On s390, a dummy irq routing table is created.
(在内核中创建中断控制器模型。在 x86 上，创建一个虚拟 IOAPIC、一个虚拟 PIC(两个嵌套的 PIC)，并设置后续创建的 vcpu 拥有本地 APIC。GSI 0-15 的 IRQ 路由同时指向 PIC 和 IOAPIC；GSI 16-23 只指向 IOAPIC。在 arm64 上会创建 GICv2。其他 GIC 版本需要使用 KVM_CREATE_DEVICE,它同样支持创建 GICv2。对于 GICv2,优先使用 KVM_CREATE_DEVICE 而不是 KVM_CREATE_IRQCHIP。在 s390 上会创建一个占位 (dummy) 的 IRQ 路由表。)

Note that on s390 the KVM_CAP_S390_IRQCHIP vm capability needs to be enabled
before KVM_CREATE_IRQCHIP can be used.
(注意：在 s390 上，需要先启用 KVM_CAP_S390_IRQCHIP vm 能力，之后才能使用 KVM_CREATE_IRQCHIP。)


4.25 KVM_IRQ_LINE(4.25 KVM_IRQ_LINE)
------------------------------------

:Capability: KVM_CAP_IRQCHIP
  (能力：KVM_CAP_IRQCHIP)
:Architectures: x86, arm64
  (架构：x86、arm64)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_irq_level
  (参数：struct kvm_irq_level)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Sets the level of a GSI input to the interrupt controller model in the kernel.
On some architectures it is required that an interrupt controller model has
been previously created with KVM_CREATE_IRQCHIP.  Note that edge-triggered
interrupts require the level to be set to 1 and then back to 0.
(设置内核中中断控制器模型某个 GSI 输入的电平。在某些架构上，必须先用 KVM_CREATE_IRQCHIP 创建中断控制器模型。注意边沿触发中断要求把电平先置 1 再置回 0。)

On real hardware, interrupt pins can be active-low or active-high.  This
does not matter for the level field of struct kvm_irq_level: 1 always
means active (asserted), 0 means inactive (deasserted).
(在真实硬件上，中断引脚可以是低电平有效或高电平有效。这对 struct kvm_irq_level 的 level 字段没有影响：1 始终表示有效(断言)，0 表示无效(去断言)。)

x86 allows the operating system to program the interrupt polarity
(active-low/active-high) for level-triggered interrupts, and KVM used
to consider the polarity.  However, due to bitrot in the handling of
active-low interrupts, the above convention is now valid on x86 too.
This is signaled by KVM_CAP_X86_IOAPIC_POLARITY_IGNORED.  Userspace
should not present interrupts to the guest as active-low unless this
capability is present (or unless it is not using the in-kernel irqchip,
of course).
(x86 允许操作系统为电平触发中断设置中断极性(低电平有效/高电平有效)，KVM 过去会考虑该极性。然而，由于处理低电平有效中断的代码年久失修 (bitrot)，上述约定现在在 x86 上同样有效。这一点由 KVM_CAP_X86_IOAPIC_POLARITY_IGNORED 表明。除非存在该能力(当然，或者根本不使用内核内 irqchip)，用户态不应以低电平有效的方式向客户机呈现中断。)


arm64 can signal an interrupt either at the CPU level, or at the
in-kernel irqchip (GIC), and for in-kernel irqchip can tell the GIC to
use PPIs designated for specific cpus.  The irq field is interpreted
like this::

  bits:  |  31 ... 28  | 27 ... 24 | 23  ... 16 | 15 ... 0 |
  field: | vcpu2_index | irq_type  | vcpu_index |  irq_id  |

(arm64 既可以在 CPU 层级发出中断，也可以在内核内 irqchip (GIC) 处发出中断；对于内核内 irqchip，还可以让 GIC 使用专供特定 cpu 的 PPI。irq 字段的解释如下：)

The irq_type field has the following values:

- KVM_ARM_IRQ_TYPE_CPU:
	       out-of-kernel GIC: irq_id 0 is IRQ, irq_id 1 is FIQ
	       (内核外 GIC：irq_id 0 为 IRQ，irq_id 1 为 FIQ)
- KVM_ARM_IRQ_TYPE_SPI:
	       in-kernel GICv2/GICv3: SPI, irq_id between 32 and 1019 (incl.)
               (the vcpu_index field is ignored)
	       in-kernel GICv5: SPI, irq_id between 0 and 65535 (incl.)
	       (内核内 GICv2/GICv3：SPI，irq_id 介于 32 与 1019 之间(含)；(忽略 vcpu_index 字段)；内核内 GICv5：SPI，irq_id 介于 0 与 65535 之间(含))
- KVM_ARM_IRQ_TYPE_PPI:
	       in-kernel GICv2/GICv3: PPI, irq_id between 16 and 31 (incl.)
	       in-kernel GICv5: PPI, irq_id between 0 and 127 (incl.)
	       (内核内 GICv2/GICv3：PPI，irq_id 介于 16 与 31 之间(含)；内核内 GICv5：PPI，irq_id 介于 0 与 127 之间(含))

(The irq_id field thus corresponds nicely to the IRQ ID in the ARM GIC specs)
(irq_id 字段因此与 ARM GIC 规范中的 IRQ ID 恰好对应)

In both cases, level is used to assert/deassert the line.
(两种情况下，level 都用于断言/去断言该线路。)

When KVM_CAP_ARM_IRQ_LINE_LAYOUT_2 is supported, the target vcpu is
identified as (256 * vcpu2_index + vcpu_index). Otherwise, vcpu2_index
must be zero.
(当支持 KVM_CAP_ARM_IRQ_LINE_LAYOUT_2 时，目标 vcpu 由 (256 * vcpu2_index + vcpu_index) 标识。否则，vcpu2_index 必须为 0。)

Note that on arm64, the KVM_CAP_IRQCHIP capability only conditions
injection of interrupts for the in-kernel irqchip. KVM_IRQ_LINE can always
be used for a userspace interrupt controller.
(注意：在 arm64 上，KVM_CAP_IRQCHIP 能力只约束内核内 irqchip 的中断注入。KVM_IRQ_LINE 始终可用于用户态中断控制器。)

::

  struct kvm_irq_level {
	union {
		__u32 irq;     /* GSI */
		__s32 status;  /* not used for KVM_IRQ_LEVEL */
	};
	__u32 level;           /* 0 or 1 */
  };


4.26 KVM_GET_IRQCHIP(4.26 KVM_GET_IRQCHIP)
------------------------------------------

:Capability: KVM_CAP_IRQCHIP
  (能力：KVM_CAP_IRQCHIP)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_irqchip (in/out)
  (参数：struct kvm_irqchip (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Reads the state of a kernel interrupt controller created with
KVM_CREATE_IRQCHIP into a buffer provided by the caller.
(把用 KVM_CREATE_IRQCHIP 创建的内核中断控制器的状态读入调用者提供的缓冲区。)

::

  struct kvm_irqchip {
	__u32 chip_id;  /* 0 = PIC1, 1 = PIC2, 2 = IOAPIC */
	__u32 pad;
        union {
		char dummy[512];  /* reserving space */
		struct kvm_pic_state pic;
		struct kvm_ioapic_state ioapic;
	} chip;
  };


4.27 KVM_SET_IRQCHIP(4.27 KVM_SET_IRQCHIP)
------------------------------------------

:Capability: KVM_CAP_IRQCHIP
  (能力：KVM_CAP_IRQCHIP)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_irqchip (in)
  (参数：struct kvm_irqchip (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Sets the state of a kernel interrupt controller created with
KVM_CREATE_IRQCHIP from a buffer provided by the caller.
(用调用者提供的缓冲区设置用 KVM_CREATE_IRQCHIP 创建的内核中断控制器的状态。)

::

  struct kvm_irqchip {
	__u32 chip_id;  /* 0 = PIC1, 1 = PIC2, 2 = IOAPIC */
	__u32 pad;
        union {
		char dummy[512];  /* reserving space */
		struct kvm_pic_state pic;
		struct kvm_ioapic_state ioapic;
	} chip;
  };


4.28 KVM_XEN_HVM_CONFIG(4.28 KVM_XEN_HVM_CONFIG)
------------------------------------------------

:Capability: KVM_CAP_XEN_HVM
  (能力：KVM_CAP_XEN_HVM)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_xen_hvm_config (in)
  (参数：struct kvm_xen_hvm_config (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Sets the MSR that the Xen HVM guest uses to initialize its hypercall
page, and provides the starting address and size of the hypercall
blobs in userspace.  When the guest writes the MSR, kvm copies one
page of a blob (32- or 64-bit, depending on the vcpu mode) to guest
memory.
(设置 Xen HVM 客户机用于初始化其超调用 (hypercall) 页的 MSR，并提供用户态中超调用 blob 的起始地址和大小。当客户机写该 MSR 时，kvm 会把 blob 的一页(32 位或 64 位，取决于 vcpu 模式)复制到客户机内存。)

The MSR index must be in the range [0x40000000, 0x4fffffff], i.e. must reside
in the range that is unofficially reserved for use by hypervisors.  The min/max
values are enumerated via KVM_XEN_MSR_MIN_INDEX and KVM_XEN_MSR_MAX_INDEX.
(MSR 索引必须在 [0x40000000, 0x4fffffff] 范围内，即必须落在非官方保留给 hypervisor 使用的范围内。最小/最大值通过 KVM_XEN_MSR_MIN_INDEX 与 KVM_XEN_MSR_MAX_INDEX 枚举。)

::

  struct kvm_xen_hvm_config {
	__u32 flags;
	__u32 msr;
	__u64 blob_addr_32;
	__u64 blob_addr_64;
	__u8 blob_size_32;
	__u8 blob_size_64;
	__u8 pad2[30];
  };

If certain flags are returned from the KVM_CAP_XEN_HVM check, they may
be set in the flags field of this ioctl:
(如果 KVM_CAP_XEN_HVM 检查返回了某些标志，它们可以在此 ioctl 的 flags 字段中设置：)

The KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL flag requests KVM to generate
the contents of the hypercall page automatically; hypercalls will be
intercepted and passed to userspace through KVM_EXIT_XEN.  In this
case, all of the blob size and address fields must be zero.
(KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL 标志请求 KVM 自动生成超调用页的内容；超调用会被拦截，并通过 KVM_EXIT_XEN 传给用户态。在这种情况下，所有 blob 大小和地址字段都必须为 0。)

The KVM_XEN_HVM_CONFIG_EVTCHN_SEND flag indicates to KVM that userspace
will always use the KVM_XEN_HVM_EVTCHN_SEND ioctl to deliver event
channel interrupts rather than manipulating the guest's shared_info
structures directly. This, in turn, may allow KVM to enable features
such as intercepting the SCHEDOP_poll hypercall to accelerate PV
spinlock operation for the guest. Userspace may still use the ioctl
to deliver events if it was advertised, even if userspace does not
send this indication that it will always do so
(KVM_XEN_HVM_CONFIG_EVTCHN_SEND 标志向 KVM 表明，用户态将始终使用 KVM_XEN_HVM_EVTCHN_SEND ioctl 来投递事件通道中断，而不是直接操作客户机的 shared_info 结构。这进而可以让 KVM 启用一些特性，例如拦截 SCHEDOP_poll 超调用，以加速客户机的 PV 自旋锁操作。只要该能力已通告，即使用户态没有发送这种"将始终如此"的声明，它仍然可以使用该 ioctl 来投递事件。)

No other flags are currently valid in the struct kvm_xen_hvm_config.
(目前在 struct kvm_xen_hvm_config 中没有其他有效标志。)

4.29 KVM_GET_CLOCK(4.29 KVM_GET_CLOCK)
--------------------------------------

:Capability: KVM_CAP_ADJUST_CLOCK
  (能力：KVM_CAP_ADJUST_CLOCK)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_clock_data (out)
  (参数：struct kvm_clock_data (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Gets the current timestamp of kvmclock as seen by the current guest. In
conjunction with KVM_SET_CLOCK, it is used to ensure monotonicity on scenarios
such as migration.
(获取当前客户机看到的 kvmclock 时间戳。与 KVM_SET_CLOCK 配合使用，用于在迁移等场景下确保单调性。)

When KVM_CAP_ADJUST_CLOCK is passed to KVM_CHECK_EXTENSION, it returns the
set of bits that KVM can return in struct kvm_clock_data's flag member.
(当把 KVM_CAP_ADJUST_CLOCK 传给 KVM_CHECK_EXTENSION 时，它返回 KVM 可以在 struct kvm_clock_data 的 flag 成员中返回的位集合。)

The following flags are defined:
(定义了以下标志：)

KVM_CLOCK_TSC_STABLE
  If set, the returned value is the exact kvmclock
  value seen by all VCPUs at the instant when KVM_GET_CLOCK was called.
  If clear, the returned value is simply CLOCK_MONOTONIC plus a constant
  offset; the offset can be modified with KVM_SET_CLOCK.  KVM will try
  to make all VCPUs follow this clock, but the exact value read by each
  VCPU could differ, because the host TSC is not stable.
  (如果置位，返回的值就是调用 KVM_GET_CLOCK 的那一刻所有 VCPU 看到的准确 kvmclock 值。如果清零，返回的值只是 CLOCK_MONOTONIC 加上一个常量偏移；该偏移可用 KVM_SET_CLOCK 修改。KVM 会尽量让所有 VCPU 跟随此时钟，但由于宿主机 TSC 不稳定，每个 VCPU 读到的准确值可能不同。)

KVM_CLOCK_REALTIME
  If set, the `realtime` field in the kvm_clock_data
  structure is populated with the value of the host's real time
  clocksource at the instant when KVM_GET_CLOCK was called. If clear,
  the `realtime` field does not contain a value.
  (如果置位，kvm_clock_data 结构的 realtime 字段会填充为调用 KVM_GET_CLOCK 那一刻宿主机实时时钟源的值。如果清零，realtime 字段不包含值。)

KVM_CLOCK_HOST_TSC
  If set, the `host_tsc` field in the kvm_clock_data
  structure is populated with the value of the host's timestamp counter (TSC)
  at the instant when KVM_GET_CLOCK was called. If clear, the `host_tsc` field
  does not contain a value.
  (如果置位，kvm_clock_data 结构的 host_tsc 字段会填充为调用 KVM_GET_CLOCK 那一刻宿主机时间戳计数器 (TSC) 的值。如果清零，host_tsc 字段不包含值。)

::

  struct kvm_clock_data {
	__u64 clock;  /* kvmclock current value */
	__u32 flags;
	__u32 pad0;
	__u64 realtime;
	__u64 host_tsc;
	__u32 pad[4];
  };


4.30 KVM_SET_CLOCK(4.30 KVM_SET_CLOCK)
--------------------------------------

:Capability: KVM_CAP_ADJUST_CLOCK
  (能力：KVM_CAP_ADJUST_CLOCK)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_clock_data (in)
  (参数：struct kvm_clock_data (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Sets the current timestamp of kvmclock to the value specified in its parameter.
In conjunction with KVM_GET_CLOCK, it is used to ensure monotonicity on scenarios
such as migration.
(把 kvmclock 的当前时间戳设置为其参数中指定的值。与 KVM_GET_CLOCK 配合使用，用于在迁移等场景下确保单调性。)

The following flags can be passed:
(可以传入以下标志：)

KVM_CLOCK_REALTIME
  If set, KVM will compare the value of the `realtime` field
  with the value of the host's real time clocksource at the instant when
  KVM_SET_CLOCK was called. The difference in elapsed time is added to the final
  kvmclock value that will be provided to guests.
  (如果置位，KVM 会把 realtime 字段的值与调用 KVM_SET_CLOCK 那一刻宿主机实时时钟源的值进行比较，并把流逝时间之差加到最终提供给客户机的 kvmclock 值中。)

Other flags returned by ``KVM_GET_CLOCK`` are accepted but ignored.
(KVM_GET_CLOCK 返回的其他标志会被接受但忽略。)

::

  struct kvm_clock_data {
	__u64 clock;  /* kvmclock current value */
	__u32 flags;
	__u32 pad0;
	__u64 realtime;
	__u64 host_tsc;
	__u32 pad[4];
  };


4.31 KVM_GET_VCPU_EVENTS(4.31 KVM_GET_VCPU_EVENTS)
--------------------------------------------------

:Capability: KVM_CAP_VCPU_EVENTS
  (能力：KVM_CAP_VCPU_EVENTS)
:Extended by: KVM_CAP_INTR_SHADOW
  (扩展：KVM_CAP_INTR_SHADOW)
:Architectures: x86, arm64
  (架构：x86、arm64)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_vcpu_events (out)
  (参数：struct kvm_vcpu_events (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

X86:
^^^^

Gets currently pending exceptions, interrupts, and NMIs as well as related
states of the vcpu.
(获取 vcpu 当前待决的异常、中断和 NMI 以及相关状态。)

::

  struct kvm_vcpu_events {
	struct {
		__u8 injected;
		__u8 nr;
		__u8 has_error_code;
		__u8 pending;
		__u32 error_code;
	} exception;
	struct {
		__u8 injected;
		__u8 nr;
		__u8 soft;
		__u8 shadow;
	} interrupt;
	struct {
		__u8 injected;
		__u8 pending;
		__u8 masked;
		__u8 pad;
	} nmi;
	__u32 sipi_vector;
	__u32 flags;
	struct {
		__u8 smm;
		__u8 pending;
		__u8 smm_inside_nmi;
		__u8 latched_init;
	} smi;
	__u8 reserved[27];
	__u8 exception_has_payload;
	__u64 exception_payload;
  };

The following bits are defined in the flags field:
(flags 字段中定义了以下位：)

- KVM_VCPUEVENT_VALID_SHADOW may be set to signal that
  interrupt.shadow contains a valid state.
  (KVM_VCPUEVENT_VALID_SHADOW 可以置位，表示 interrupt.shadow 包含有效状态。)

- KVM_VCPUEVENT_VALID_SMM may be set to signal that smi contains a
  valid state.
  (KVM_VCPUEVENT_VALID_SMM 可以置位，表示 smi 包含有效状态。)

- KVM_VCPUEVENT_VALID_PAYLOAD may be set to signal that the
  exception_has_payload, exception_payload, and exception.pending
  fields contain a valid state. This bit will be set whenever
  KVM_CAP_EXCEPTION_PAYLOAD is enabled.
  (KVM_VCPUEVENT_VALID_PAYLOAD 可以置位，表示 exception_has_payload、exception_payload 和 exception.pending 字段包含有效状态。只要启用了 KVM_CAP_EXCEPTION_PAYLOAD，该位就会置位。)

- KVM_VCPUEVENT_VALID_TRIPLE_FAULT may be set to signal that the
  triple_fault_pending field contains a valid state. This bit will
  be set whenever KVM_CAP_X86_TRIPLE_FAULT_EVENT is enabled.
  (KVM_VCPUEVENT_VALID_TRIPLE_FAULT 可以置位，表示 triple_fault_pending 字段包含有效状态。只要启用了 KVM_CAP_X86_TRIPLE_FAULT_EVENT，该位就会置位。)

ARM64:
^^^^^^

If the guest accesses a device that is being emulated by the host kernel in
such a way that a real device would generate a physical SError, KVM may make
a virtual SError pending for that VCPU. This system error interrupt remains
pending until the guest takes the exception by unmasking PSTATE.A.
(如果客户机访问一个由宿主内核模拟的设备，且真实设备会因此产生物理 SError，KVM 可以为该 VCPU 挂起一个虚拟 SError。这一系统错误中断会一直保持待决，直到客户机通过解除 PSTATE.A 的屏蔽而获取该异常。)

Running the VCPU may cause it to take a pending SError, or make an access that
causes an SError to become pending. The event's description is only valid while
the VPCU is not running.
(运行 VCPU 可能使它接收一个待决的 SError，或执行某个导致 SError 挂起的访问。只有在 VCPU 未运行时，该事件的描述才有效。)

This API provides a way to read and write the pending 'event' state that is not
visible to the guest. To save, restore or migrate a VCPU the struct representing
the state can be read then written using this GET/SET API, along with the other
guest-visible registers. It is not possible to 'cancel' an SError that has been
made pending.
(此 API 提供了对客户机不可见的待决 'event' 状态的读写途径。要保存、恢复或迁移 VCPU，可以用这组 GET/SET API 连同其他客户机可见寄存器一起，先读取再写入表示该状态的结构体。已挂起的 SError 无法被'取消'。)

A device being emulated in user-space may also wish to generate an SError. To do
this the events structure can be populated by user-space. The current state
should be read first, to ensure no existing SError is pending. If an existing
SError is pending, the architecture's 'Multiple SError interrupts' rules should
be followed. (2.5.3 of DDI0587.a "ARM Reliability, Availability, and
Serviceability (RAS) Specification").
(在用户态模拟的设备也可能希望产生 SError。为此，用户态可以填充 events 结构体。应先读取当前状态，以确保没有已挂起的 SError。如果已有 SError 挂起，应遵循架构的 'Multiple SError interrupts' 规则(DDI0587.a "ARM Reliability, Availability, and Serviceability (RAS) Specification" 的 2.5.3 节)。)

SError exceptions always have an ESR value. Some CPUs have the ability to
specify what the virtual SError's ESR value should be. These systems will
advertise KVM_CAP_ARM_INJECT_SERROR_ESR. In this case exception.has_esr will
always have a non-zero value when read, and the agent making an SError pending
should specify the ISS field in the lower 24 bits of exception.serror_esr. If
the system supports KVM_CAP_ARM_INJECT_SERROR_ESR, but user-space sets the events
with exception.has_esr as zero, KVM will choose an ESR.
(SError 异常总是带有 ESR 值。某些 CPU 能够指定虚拟 SError 的 ESR 值。这类系统会通告 KVM_CAP_ARM_INJECT_SERROR_ESR。在这种情况下，读取时 exception.has_esr 总是非零，而使 SError 挂起的一方应在 exception.serror_esr 的低 24 位中指定 ISS 字段。如果系统支持 KVM_CAP_ARM_INJECT_SERROR_ESR，但用户态在设置 events 时把 exception.has_esr 置为 0，KVM 会自行选择一个 ESR。)

Specifying exception.has_esr on a system that does not support it will return
-EINVAL. Setting anything other than the lower 24bits of exception.serror_esr
will return -EINVAL.
(在不支持该特性的系统上指定 exception.has_esr 会返回 -EINVAL。设置 exception.serror_esr 低 24 位以外的任何内容都会返回 -EINVAL。)

It is not possible to read back a pending external abort (injected via
KVM_SET_VCPU_EVENTS or otherwise) because such an exception is always delivered
directly to the virtual CPU).
(无法读回一个待决的外部中止(通过 KVM_SET_VCPU_EVENTS 或其他方式注入)，因为这类异常总是直接投递给虚拟 CPU。)

Calling this ioctl on a vCPU that hasn't been initialized will return
-ENOEXEC.
(对尚未初始化的 vCPU 调用此 ioctl 会返回 -ENOEXEC。)

::

  struct kvm_vcpu_events {
	struct {
		__u8 serror_pending;
		__u8 serror_has_esr;
		__u8 ext_dabt_pending;
		/* Align it to 8 bytes */
		__u8 pad[5];
		__u64 serror_esr;
	} exception;
	__u32 reserved[12];
  };

4.32 KVM_SET_VCPU_EVENTS(4.32 KVM_SET_VCPU_EVENTS)
--------------------------------------------------

:Capability: KVM_CAP_VCPU_EVENTS
  (能力：KVM_CAP_VCPU_EVENTS)
:Extended by: KVM_CAP_INTR_SHADOW
  (扩展：KVM_CAP_INTR_SHADOW)
:Architectures: x86, arm64
  (架构：x86、arm64)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_vcpu_events (in)
  (参数：struct kvm_vcpu_events (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

X86:
^^^^

Set pending exceptions, interrupts, and NMIs as well as related states of the
vcpu.
(设置 vcpu 待决的异常、中断和 NMI 以及相关状态。)

See KVM_GET_VCPU_EVENTS for the data structure.
(数据结构参见 KVM_GET_VCPU_EVENTS。)

Fields that may be modified asynchronously by running VCPUs can be excluded
from the update. These fields are nmi.pending, sipi_vector, smi.smm,
smi.pending. Keep the corresponding bits in the flags field cleared to
suppress overwriting the current in-kernel state. The bits are:
(可以从更新中排除那些可能被正在运行的 VCPU 异步修改的字段。这些字段是 nmi.pending、sipi_vector、smi.smm、smi.pending。保持 flags 字段中相应位为 0，即可避免覆盖内核中的当前状态。这些位是：)

===============================  ==================================
KVM_VCPUEVENT_VALID_NMI_PENDING  transfer nmi.pending to the kernel (把 nmi.pending 传给内核)
KVM_VCPUEVENT_VALID_SIPI_VECTOR  transfer sipi_vector (传给内核 sipi_vector)
KVM_VCPUEVENT_VALID_SMM          transfer the smi sub-struct. (传给内核 smi 子结构体)
===============================  ==================================

If KVM_CAP_INTR_SHADOW is available, KVM_VCPUEVENT_VALID_SHADOW can be set in
the flags field to signal that interrupt.shadow contains a valid state and
shall be written into the VCPU.
(如果 KVM_CAP_INTR_SHADOW 可用，可以在 flags 字段中设置 KVM_VCPUEVENT_VALID_SHADOW，表示 interrupt.shadow 包含有效状态并应写入 VCPU。)

KVM_VCPUEVENT_VALID_SMM can only be set if KVM_CAP_X86_SMM is available.
(只有 KVM_CAP_X86_SMM 可用时才能设置 KVM_VCPUEVENT_VALID_SMM。)

If KVM_CAP_EXCEPTION_PAYLOAD is enabled, KVM_VCPUEVENT_VALID_PAYLOAD
can be set in the flags field to signal that the
exception_has_payload, exception_payload, and exception.pending fields
contain a valid state and shall be written into the VCPU.
(如果启用了 KVM_CAP_EXCEPTION_PAYLOAD，可以在 flags 字段中设置 KVM_VCPUEVENT_VALID_PAYLOAD，表示 exception_has_payload、exception_payload 和 exception.pending 字段包含有效状态并应写入 VCPU。)

If KVM_CAP_X86_TRIPLE_FAULT_EVENT is enabled, KVM_VCPUEVENT_VALID_TRIPLE_FAULT
can be set in flags field to signal that the triple_fault field contains
a valid state and shall be written into the VCPU.
(如果启用了 KVM_CAP_X86_TRIPLE_FAULT_EVENT，可以在 flags 字段中设置 KVM_VCPUEVENT_VALID_TRIPLE_FAULT，表示 triple_fault 字段包含有效状态并应写入 VCPU。)

ARM64:
^^^^^^

User space may need to inject several types of events to the guest.
(用户态可能需要向客户机注入多种类型的事件。)

Set the pending SError exception state for this VCPU. It is not possible to
'cancel' an Serror that has been made pending.
(设置此 VCPU 的待决 SError 异常状态。已挂起的 SError 无法被'取消'。)

If the guest performed an access to I/O memory which could not be handled by
userspace, for example because of missing instruction syndrome decode
information or because there is no device mapped at the accessed IPA, then
userspace can ask the kernel to inject an external abort using the address
from the exiting fault on the VCPU. It is a programming error to set
ext_dabt_pending after an exit which was not either KVM_EXIT_MMIO,
KVM_EXIT_ARM_NISV, or KVM_EXIT_ARM_LDST64B. This feature is only available if
the system supports KVM_CAP_ARM_INJECT_EXT_DABT. This is a helper which
provides commonality in how userspace reports accesses for the above cases to
guests, across different userspace implementations. Nevertheless, userspace
can still emulate all Arm exceptions by manipulating individual registers
using the KVM_SET_ONE_REG API.
(如果客户机对 I/O 内存执行的访问无法由用户态处理——例如缺少指令综合症解码信息，或被访问的 IPA 上没有映射设备——用户态可以请求内核使用 VCPU 上这次退出故障的地址注入一个外部中止。如果退出原因不是 KVM_EXIT_MMIO、KVM_EXIT_ARM_NISV 或 KVM_EXIT_ARM_LDST64B，却设置了 ext_dabt_pending，则属于编程错误。只有系统支持 KVM_CAP_ARM_INJECT_EXT_DABT 时此特性才可用。它是一个辅助机制，让不同用户态实现在就上述情况向客户机报告访问时保持一致。尽管如此，用户态仍然可以通过 KVM_SET_ONE_REG API 操作单个寄存器来模拟所有 Arm 异常。)

See KVM_GET_VCPU_EVENTS for the data structure.
(数据结构参见 KVM_GET_VCPU_EVENTS。)

Calling this ioctl on a vCPU that hasn't been initialized will return
-ENOEXEC.
(对尚未初始化的 vCPU 调用此 ioctl 会返回 -ENOEXEC。)

4.33 KVM_GET_DEBUGREGS(4.33 KVM_GET_DEBUGREGS)
----------------------------------------------

:Capability: KVM_CAP_DEBUGREGS
  (能力：KVM_CAP_DEBUGREGS)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_debugregs (out)
  (参数：struct kvm_debugregs (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Reads debug registers from the vcpu.
(从 vcpu 读取调试寄存器。)

::

  struct kvm_debugregs {
	__u64 db[4];
	__u64 dr6;
	__u64 dr7;
	__u64 flags;
	__u64 reserved[9];
  };


4.34 KVM_SET_DEBUGREGS(4.34 KVM_SET_DEBUGREGS)
----------------------------------------------

:Capability: KVM_CAP_DEBUGREGS
  (能力：KVM_CAP_DEBUGREGS)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_debugregs (in)
  (参数：struct kvm_debugregs (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Writes debug registers into the vcpu.
(向 vcpu 写入调试寄存器。)

See KVM_GET_DEBUGREGS for the data structure. The flags field is unused
yet and must be cleared on entry.
(数据结构参见 KVM_GET_DEBUGREGS。flags 字段尚未使用，进入时必须清零。)


4.35 KVM_SET_USER_MEMORY_REGION(4.35 KVM_SET_USER_MEMORY_REGION)
----------------------------------------------------------------

:Capability: KVM_CAP_USER_MEMORY
  (能力：KVM_CAP_USER_MEMORY)
:Architectures: all
  (架构：全部)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_userspace_memory_region (in)
  (参数：struct kvm_userspace_memory_region (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  struct kvm_userspace_memory_region {
	__u32 slot;
	__u32 flags;
	__u64 guest_phys_addr;
	__u64 memory_size; /* bytes */
	__u64 userspace_addr; /* start of the userspace allocated memory */
  };

  /* for kvm_userspace_memory_region::flags */
  #define KVM_MEM_LOG_DIRTY_PAGES	(1UL << 0)
  #define KVM_MEM_READONLY	(1UL << 1)

This ioctl allows the user to create, modify or delete a guest physical
memory slot.  Bits 0-15 of "slot" specify the slot id and this value
should be less than the maximum number of user memory slots supported per
VM.  The maximum allowed slots can be queried using KVM_CAP_NR_MEMSLOTS.
Slots may not overlap in guest physical address space.
(此 ioctl 允许用户创建、修改或删除一个客户机物理内存 slot。"slot" 的 bits 0-15 指定 slot id，该值应小于每个 VM 支持的用户内存 slot 最大数量。允许的最大 slot 数可通过 KVM_CAP_NR_MEMSLOTS 查询。各 slot 在客户机物理地址空间中不得重叠。)

If KVM_CAP_MULTI_ADDRESS_SPACE is available, bits 16-31 of "slot"
specifies the address space which is being modified.  They must be
less than the value that KVM_CHECK_EXTENSION returns for the
KVM_CAP_MULTI_ADDRESS_SPACE capability.  Slots in separate address spaces
are unrelated; the restriction on overlapping slots only applies within
each address space.
(如果 KVM_CAP_MULTI_ADDRESS_SPACE 可用，"slot" 的 bits 16-31 指定正在修改的地址空间。它们必须小于 KVM_CHECK_EXTENSION 针对 KVM_CAP_MULTI_ADDRESS_SPACE 能力返回的值。不同地址空间中的 slot 互不相关；slot 不得重叠的限制只在每个地址空间内部适用。)

Deleting a slot is done by passing zero for memory_size.  When changing
an existing slot, it may be moved in the guest physical memory space,
or its flags may be modified, but it may not be resized.
(把 memory_size 传 0 即删除一个 slot。修改已有 slot 时，可以在客户机物理内存空间中移动它，或修改它的 flags，但不能改变其大小。)

Memory for the region is taken starting at the address denoted by the
field userspace_addr, which must point at user addressable memory for
the entire memory slot size.  Any object may back this memory, including
anonymous memory, ordinary files, and hugetlbfs.  Changes in the backing
of the memory region are automatically reflected into the guest.
For example, an mmap() that affects the region will be made visible
immediately.  Another example is madvise(MADV_DROP).
(该区域的内存从 userspace_addr 字段所表示的地址开始取用，该地址必须指向覆盖整个内存 slot 大小的用户可寻址内存。任何对象都可以作为这块内存的后备，包括匿名内存、普通文件和 hugetlbfs。内存区域后备的变更会自动反映到客户机中。例如，影响该区域的 mmap() 会立即变为可见。另一个例子是 madvise(MADV_DROP)。)

On architectures that support a form of address tagging, userspace_addr must
be an untagged address.
(在支持某种地址标签 (address tagging) 的架构上，userspace_addr 必须是未加标签的地址。)

It is recommended that the lower 21 bits of guest_phys_addr and userspace_addr
be identical.  This allows large pages in the guest to be backed by large
pages in the host.
(建议 guest_phys_addr 与 userspace_addr 的低 21 位相同。这样客户机的大页就能由宿主机的大页支撑。)

The flags field supports two flags: KVM_MEM_LOG_DIRTY_PAGES and
KVM_MEM_READONLY.  The former can be set to instruct KVM to keep track of
writes to memory within the slot.  See KVM_GET_DIRTY_LOG ioctl to know how to
use it.  The latter can be set, if KVM_CAP_READONLY_MEM capability allows it,
to make a new slot read-only.  In this case, writes to this memory will be
posted to userspace as KVM_EXIT_MMIO exits.
(flags 字段支持两个标志：KVM_MEM_LOG_DIRTY_PAGES 和 KVM_MEM_READONLY。前者可置位以指示 KVM 跟踪对该 slot 内内存的写入，其用法参见 KVM_GET_DIRTY_LOG ioctl。后者在 KVM_CAP_READONLY_MEM 能力允许时可置位，使新 slot 变为只读。这种情况下，对该内存的写入会以 KVM_EXIT_MMIO 退出的形式上报给用户态。)

For TDX guest, deleting/moving memory region loses guest memory contents.
Read only region isn't supported.  Only as-id 0 is supported.
(对 TDX 客户机，删除/移动内存区域会丢失客户机内存内容。不支持只读区域。仅支持 as-id 0。)

Note: On arm64, a write generated by the page-table walker (to update
the Access and Dirty flags, for example) never results in a
KVM_EXIT_MMIO exit when the slot has the KVM_MEM_READONLY flag. This
is because KVM cannot provide the data that would be written by the
page-table walker, making it impossible to emulate the access.
Instead, an abort (data abort if the cause of the page-table update
was a load or a store, instruction abort if it was an instruction
fetch) is injected in the guest.
(注意：在 arm64 上，当 slot 带有 KVM_MEM_READONLY 标志时，由页表遍历器产生的写(例如更新 Access 和 Dirty 标志)绝不会导致 KVM_EXIT_MMIO 退出。这是因为 KVM 无法提供页表遍历器将要写的数据，因而无法模拟该访问。取而代之的是向客户机注入一个 abort(如果页表更新的起因是 load 或 store 则为 data abort，如果是取指则为 instruction abort)。)

S390:
^^^^^

Returns -EINVAL or -EEXIST if the VM has the KVM_VM_S390_UCONTROL flag set.
Returns -EINVAL if called on a protected VM.
(如果 VM 设置了 KVM_VM_S390_UCONTROL 标志，返回 -EINVAL 或 -EEXIST。对受保护 VM 调用时返回 -EINVAL。)

4.36 KVM_SET_TSS_ADDR(4.36 KVM_SET_TSS_ADDR)
--------------------------------------------

:Capability: KVM_CAP_SET_TSS_ADDR
  (能力：KVM_CAP_SET_TSS_ADDR)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: unsigned long tss_address (in)
  (参数：unsigned long tss_address (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

This ioctl defines the physical address of a three-page region in the guest
physical address space.  The region must be within the first 4GB of the
guest physical address space and must not conflict with any memory slot
or any mmio address.  The guest may malfunction if it accesses this memory
region.
(此 ioctl 在客户机物理地址空间中定义一个三页区域的物理地址。该区域必须位于客户机物理地址空间的前 4GB 内，且不得与任何内存 slot 或任何 mmio 地址冲突。如果客户机访问该内存区域，可能会出现故障。)

This ioctl is required on Intel-based hosts.  This is needed on Intel hardware
because of a quirk in the virtualization implementation (see the internals
documentation when it pops into existence).
(基于 Intel 的宿主机必须有此 ioctl。这是 Intel 硬件上虚拟化实现的一个怪癖所致(待相应的内部文档出现后参见)。)


.. _KVM_ENABLE_CAP:

4.37 KVM_ENABLE_CAP(4.37 KVM_ENABLE_CAP)
----------------------------------------

:Capability: KVM_CAP_ENABLE_CAP
  (能力：KVM_CAP_ENABLE_CAP)
:Architectures: mips, ppc, s390, x86, loongarch
  (架构：mips、ppc、s390、x86、loongarch)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_enable_cap (in)
  (参数：struct kvm_enable_cap (in))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

:Capability: KVM_CAP_ENABLE_CAP_VM
  (能力：KVM_CAP_ENABLE_CAP_VM)
:Architectures: all
  (架构：全部)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_enable_cap (in)
  (参数：struct kvm_enable_cap (in))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

.. note::

   Not all extensions are enabled by default. Using this ioctl the application
   can enable an extension, making it available to the guest.
   (注意：并非所有扩展都默认启用。应用程序可以使用此 ioctl 启用某个扩展，使其对客户机可用。)

On systems that do not support this ioctl, it always fails. On systems that
do support it, it only works for extensions that are supported for enablement.
(在不支持此 ioctl 的系统上，它总是失败。在支持的系统上，它只对支持启用的扩展有效。)

To check if a capability can be enabled, the KVM_CHECK_EXTENSION ioctl should
be used.
(要检查某个能力能否启用，应使用 KVM_CHECK_EXTENSION ioctl。)

::

  struct kvm_enable_cap {
       /* in */
       __u32 cap;

The capability that is supposed to get enabled.
(要启用的能力。)

::

       __u32 flags;

A bitfield indicating future enhancements. Has to be 0 for now.
(指示未来增强功能的位域。目前必须为 0。)

::

       __u64 args[4];

Arguments for enabling a feature. If a feature needs initial values to
function properly, this is the place to put them.
(启用某特性所需的参数。如果某特性需要初始值才能正常工作，就放在这里。)

::

       __u8  pad[64];
  };

The vcpu ioctl should be used for vcpu-specific capabilities, the vm ioctl
for vm-wide capabilities.
(vcpu 特定的能力应使用 vcpu ioctl，VM 级能力应使用 vm ioctl。)

4.38 KVM_GET_MP_STATE(4.38 KVM_GET_MP_STATE)
--------------------------------------------

:Capability: KVM_CAP_MP_STATE
  (能力：KVM_CAP_MP_STATE)
:Architectures: x86, s390, arm64, riscv, loongarch
  (架构：x86、s390、arm64、riscv、loongarch)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_mp_state (out)
  (参数：struct kvm_mp_state (out))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

::

  struct kvm_mp_state {
	__u32 mp_state;
  };

Returns the vcpu's current "multiprocessing state" (though also valid on
uniprocessor guests).
(返回 vcpu 当前的"多处理状态"("multiprocessing state"，在单处理器客户机上同样有效)。)

Possible values are:
(可能的取值如下：)

   ==========================    ===============================================
   KVM_MP_STATE_RUNNABLE         the vcpu is currently running
                                 [x86,arm64,riscv,loongarch]
                                 (vcpu 当前正在运行)
   KVM_MP_STATE_UNINITIALIZED    the vcpu is an application processor (AP)
                                 which has not yet received an INIT signal [x86]
                                 (vcpu 是尚未收到 INIT 信号的应用处理器 (AP))
   KVM_MP_STATE_INIT_RECEIVED    the vcpu has received an INIT signal, and is
                                 now ready for a SIPI [x86]
                                 (vcpu 已收到 INIT 信号，现在正等待 SIPI)
   KVM_MP_STATE_HALTED           the vcpu has executed a HLT instruction and
                                 is waiting for an interrupt [x86]
                                 (vcpu 已执行 HLT 指令，正在等待中断)
   KVM_MP_STATE_SIPI_RECEIVED    the vcpu has just received a SIPI (vector
                                 accessible via KVM_GET_VCPU_EVENTS) [x86]
                                 (vcpu 刚刚收到一个 SIPI，向量可通过 KVM_GET_VCPU_EVENTS 获取)
   KVM_MP_STATE_STOPPED          the vcpu is stopped [s390,arm64,riscv]
                                 (vcpu 已停止)
   KVM_MP_STATE_CHECK_STOP       the vcpu is in a special error state [s390]
                                 (vcpu 处于特殊错误状态)
   KVM_MP_STATE_OPERATING        the vcpu is operating (running or halted)
                                 [s390]
                                 (vcpu 正在运转(运行或停机))
   KVM_MP_STATE_LOAD             the vcpu is in a special load/startup state
                                 [s390]
                                 (vcpu 处于特殊的加载/启动状态)
   KVM_MP_STATE_SUSPENDED        the vcpu is in a suspend state and is waiting
                                 for a wakeup event [arm64]
                                 (vcpu 处于挂起状态，正在等待唤醒事件)
   ==========================    ===============================================

On x86, this ioctl is only useful after KVM_CREATE_IRQCHIP. Without an
in-kernel irqchip, the multiprocessing state must be maintained by userspace on
these architectures.
(在 x86 上，此 ioctl 只在 KVM_CREATE_IRQCHIP 之后才有用。在没有内核内 irqchip 的情况下，这些架构上的多处理状态必须由用户态维护。)

For arm64:
^^^^^^^^^^

If a vCPU is in the KVM_MP_STATE_SUSPENDED state, KVM will emulate the
architectural execution of a WFI instruction.
(如果 vCPU 处于 KVM_MP_STATE_SUSPENDED 状态，KVM 会按架构语义模拟一条 WFI 指令的执行。)

If a wakeup event is recognized, KVM will exit to userspace with a
KVM_SYSTEM_EVENT exit, where the event type is KVM_SYSTEM_EVENT_WAKEUP. If
userspace wants to honor the wakeup, it must set the vCPU's MP state to
KVM_MP_STATE_RUNNABLE. If it does not, KVM will continue to await a wakeup
event in subsequent calls to KVM_RUN.
(如果识别到唤醒事件，KVM 会以 KVM_SYSTEM_EVENT 退出返回用户态，事件类型为 KVM_SYSTEM_EVENT_WAKEUP。如果用户态想要响应唤醒，必须把 vCPU 的 MP 状态设置为 KVM_MP_STATE_RUNNABLE。如果不这样做，KVM 会在后续的 KVM_RUN 调用中继续等待唤醒事件。)

.. warning::

     If userspace intends to keep the vCPU in a SUSPENDED state, it is
     strongly recommended that userspace take action to suppress the
     wakeup event (such as masking an interrupt). Otherwise, subsequent
     calls to KVM_RUN will immediately exit with a KVM_SYSTEM_EVENT_WAKEUP
     event and inadvertently waste CPU cycles.
     (警告：如果用户态打算让 vCPU 保持在 SUSPENDED 状态，强烈建议用户态采取行动抑制唤醒事件(例如屏蔽某个中断)。否则，后续对 KVM_RUN 的调用会立即以 KVM_SYSTEM_EVENT_WAKEUP 事件退出，并无谓地浪费 CPU 周期。)

     Additionally, if userspace takes action to suppress a wakeup event,
     it is strongly recommended that it also restores the vCPU to its
     original state when the vCPU is made RUNNABLE again. For example,
     if userspace masked a pending interrupt to suppress the wakeup,
     the interrupt should be unmasked before returning control to the
     guest.
     (另外，如果用户态采取了抑制唤醒事件的行动，强烈建议在 vCPU 重新变为 RUNNABLE 时把 vCPU 恢复到其原状态。例如，如果用户态为抑制唤醒而屏蔽了一个待决中断，应在把控制权交还客户机之前解除对该中断的屏蔽。)

For riscv:
^^^^^^^^^^

The only states that are valid are KVM_MP_STATE_STOPPED and
KVM_MP_STATE_RUNNABLE which reflect if the vcpu is paused or not.
(唯一有效的状态是 KVM_MP_STATE_STOPPED 和 KVM_MP_STATE_RUNNABLE，它们反映 vcpu 是否被暂停。)

On LoongArch, only the KVM_MP_STATE_RUNNABLE state is used to reflect
whether the vcpu is runnable.
(在 LoongArch 上，只使用 KVM_MP_STATE_RUNNABLE 状态来反映 vcpu 是否可运行。)

4.39 KVM_SET_MP_STATE(4.39 KVM_SET_MP_STATE)
--------------------------------------------

:Capability: KVM_CAP_MP_STATE
  (能力：KVM_CAP_MP_STATE)
:Architectures: x86, s390, arm64, riscv, loongarch
  (架构：x86、s390、arm64、riscv、loongarch)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_mp_state (in)
  (参数：struct kvm_mp_state (in))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

Sets the vcpu's current "multiprocessing state"; see KVM_GET_MP_STATE for
arguments.
(设置 vcpu 当前的"多处理状态"；参数参见 KVM_GET_MP_STATE。)

On x86, this ioctl is only useful after KVM_CREATE_IRQCHIP. Without an
in-kernel irqchip, the multiprocessing state must be maintained by userspace on
these architectures.
(在 x86 上，此 ioctl 只在 KVM_CREATE_IRQCHIP 之后才有用。在没有内核内 irqchip 的情况下，这些架构上的多处理状态必须由用户态维护。)

For arm64/riscv:
^^^^^^^^^^^^^^^^

The only states that are valid are KVM_MP_STATE_STOPPED and
KVM_MP_STATE_RUNNABLE which reflect if the vcpu should be paused or not.
(唯一有效的状态是 KVM_MP_STATE_STOPPED 和 KVM_MP_STATE_RUNNABLE，它们反映 vcpu 是否应当被暂停。)

On LoongArch, only the KVM_MP_STATE_RUNNABLE state is used to reflect
whether the vcpu is runnable.
(在 LoongArch 上，只使用 KVM_MP_STATE_RUNNABLE 状态来反映 vcpu 是否可运行。)

4.40 KVM_SET_IDENTITY_MAP_ADDR(4.40 KVM_SET_IDENTITY_MAP_ADDR)
--------------------------------------------------------------

:Capability: KVM_CAP_SET_IDENTITY_MAP_ADDR
  (能力：KVM_CAP_SET_IDENTITY_MAP_ADDR)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: unsigned long identity (in)
  (参数：unsigned long identity (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

This ioctl defines the physical address of a one-page region in the guest
physical address space.  The region must be within the first 4GB of the
guest physical address space and must not conflict with any memory slot
or any mmio address.  The guest may malfunction if it accesses this memory
region.
(此 ioctl 在客户机物理地址空间中定义一个单页区域的物理地址。该区域必须位于客户机物理地址空间的前 4GB 内，且不得与任何内存 slot 或任何 mmio 地址冲突。如果客户机访问该内存区域，可能会出现故障。)

Setting the address to 0 will result in resetting the address to its default
(0xfffbc000).
(把地址设为 0 会把该地址重置为其默认值 (0xfffbc000)。)

This ioctl is required on Intel-based hosts.  This is needed on Intel hardware
because of a quirk in the virtualization implementation (see the internals
documentation when it pops into existence).
(基于 Intel 的宿主机必须有此 ioctl。这是 Intel 硬件上虚拟化实现的一个怪癖所致(待相应的内部文档出现后参见)。)

Fails if any VCPU has already been created.
(如果已经创建了任何 VCPU，则会失败。)

4.41 KVM_SET_BOOT_CPU_ID(4.41 KVM_SET_BOOT_CPU_ID)
--------------------------------------------------

:Capability: KVM_CAP_SET_BOOT_CPU_ID
  (能力：KVM_CAP_SET_BOOT_CPU_ID)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: unsigned long vcpu_id
  (参数：unsigned long vcpu_id)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Define which vcpu is the Bootstrap Processor (BSP).  Values are the same
as the vcpu id in KVM_CREATE_VCPU.  If this ioctl is not called, the default
is vcpu 0. This ioctl has to be called before vcpu creation,
otherwise it will return EBUSY error.
(定义哪个 vcpu 是引导处理器 (Bootstrap Processor, BSP)。取值与 KVM_CREATE_VCPU 中的 vcpu id 相同。如果不调用此 ioctl，默认为 vcpu 0。此 ioctl 必须在创建 vcpu 之前调用，否则会返回 EBUSY 错误。)


4.42 KVM_GET_XSAVE(4.42 KVM_GET_XSAVE)
--------------------------------------

:Capability: KVM_CAP_XSAVE
  (能力：KVM_CAP_XSAVE)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_xsave (out)
  (参数：struct kvm_xsave (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)


::

  struct kvm_xsave {
	__u32 region[1024];
	__u32 extra[0];
  };

This ioctl would copy current vcpu's xsave struct to the userspace.
(此 ioctl 会把 vcpu 当前的 xsave 结构体复制到用户态。)

4.43 KVM_SET_XSAVE(4.43 KVM_SET_XSAVE)
--------------------------------------

:Capability: KVM_CAP_XSAVE and KVM_CAP_XSAVE2
  (能力：KVM_CAP_XSAVE 与 KVM_CAP_XSAVE2)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_xsave (in)
  (参数：struct kvm_xsave (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::


  struct kvm_xsave {
	__u32 region[1024];
	__u32 extra[0];
  };

This ioctl would copy userspace's xsave struct to the kernel. It copies
as many bytes as are returned by KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2),
when invoked on the vm file descriptor. The size value returned by
KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2) will always be at least 4096.
Currently, it is only greater than 4096 if a dynamic feature has been
enabled with ``arch_prctl()``, but this may change in the future.
(此 ioctl 会把用户态的 xsave 结构体复制到内核。在 vm 文件描述符上调用时，它复制的字节数与 KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2) 返回的值相同。KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2) 返回的大小值始终至少为 4096。目前，只有在用 ``arch_prctl()`` 启用了动态特性时它才会大于 4096，但这一点将来可能改变。)

The offsets of the state save areas in struct kvm_xsave follow the
contents of CPUID leaf 0xD on the host.
(struct kvm_xsave 中各状态保存区域的偏移遵循宿主机 CPUID leaf 0xD 的内容。)


4.44 KVM_GET_XCRS(4.44 KVM_GET_XCRS)
------------------------------------

:Capability: KVM_CAP_XCRS
  (能力：KVM_CAP_XCRS)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_xcrs (out)
  (参数：struct kvm_xcrs (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  struct kvm_xcr {
	__u32 xcr;
	__u32 reserved;
	__u64 value;
  };

  struct kvm_xcrs {
	__u32 nr_xcrs;
	__u32 flags;
	struct kvm_xcr xcrs[KVM_MAX_XCRS];
	__u64 padding[16];
  };

This ioctl would copy current vcpu's xcrs to the userspace.
(此 ioctl 会把 vcpu 当前的 xcrs 复制到用户态。)


4.45 KVM_SET_XCRS(4.45 KVM_SET_XCRS)
------------------------------------

:Capability: KVM_CAP_XCRS
  (能力：KVM_CAP_XCRS)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_xcrs (in)
  (参数：struct kvm_xcrs (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  struct kvm_xcr {
	__u32 xcr;
	__u32 reserved;
	__u64 value;
  };

  struct kvm_xcrs {
	__u32 nr_xcrs;
	__u32 flags;
	struct kvm_xcr xcrs[KVM_MAX_XCRS];
	__u64 padding[16];
  };

This ioctl would set vcpu's xcr to the value userspace specified.
(此 ioctl 会把 vcpu 的 xcr 设置为用户态指定的值。)


4.46 KVM_GET_SUPPORTED_CPUID(4.46 KVM_GET_SUPPORTED_CPUID)
----------------------------------------------------------

:Capability: KVM_CAP_EXT_CPUID
  (能力：KVM_CAP_EXT_CPUID)
:Architectures: x86
  (架构：x86)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: struct kvm_cpuid2 (in/out)
  (参数：struct kvm_cpuid2 (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  struct kvm_cpuid2 {
	__u32 nent;
	__u32 padding;
	struct kvm_cpuid_entry2 entries[0];
  };

  #define KVM_CPUID_FLAG_SIGNIFCANT_INDEX		BIT(0)
  #define KVM_CPUID_FLAG_STATEFUL_FUNC		BIT(1) /* deprecated */
  #define KVM_CPUID_FLAG_STATE_READ_NEXT		BIT(2) /* deprecated */

  struct kvm_cpuid_entry2 {
	__u32 function;
	__u32 index;
	__u32 flags;
	__u32 eax;
	__u32 ebx;
	__u32 ecx;
	__u32 edx;
	__u32 padding[3];
  };

This ioctl returns x86 cpuid features which are supported by both the
hardware and kvm in its default configuration.  Userspace can use the
information returned by this ioctl to construct cpuid information (for
KVM_SET_CPUID2) that is consistent with hardware, kernel, and
userspace capabilities, and with user requirements (for example, the
user may wish to constrain cpuid to emulate older hardware, or for
feature consistency across a cluster).
(此 ioctl 返回在默认配置下硬件和 kvm 都支持的 x86 cpuid 特性。用户态可以利用此 ioctl 返回的信息来构造与硬件、内核和用户态能力以及用户需求相一致的 cpuid 信息(供 KVM_SET_CPUID2 使用)(例如，用户可能希望约束 cpuid 以模拟较旧的硬件，或者为了让集群内特性保持一致)。)

Dynamically-enabled feature bits need to be requested with
``arch_prctl()`` before calling this ioctl. Feature bits that have not
been requested are excluded from the result.
(动态启用的特性位需要在调用此 ioctl 之前通过 ``arch_prctl()`` 请求。未请求的特性位不会出现在结果中。)

Note that certain capabilities, such as KVM_CAP_X86_DISABLE_EXITS, may
expose cpuid features (e.g. MONITOR) which are not supported by kvm in
its default configuration. If userspace enables such capabilities, it
is responsible for modifying the results of this ioctl appropriately.
(注意，某些能力(如 KVM_CAP_X86_DISABLE_EXITS)可能会暴露 kvm 默认配置并不支持的 cpuid 特性(例如 MONITOR)。如果用户态启用这类能力，就有责任相应地修改此 ioctl 的结果。)

Userspace invokes KVM_GET_SUPPORTED_CPUID by passing a kvm_cpuid2 structure
with the 'nent' field indicating the number of entries in the variable-size
array 'entries'.  If the number of entries is too low to describe the cpu
capabilities, an error (E2BIG) is returned.  If the number is too high,
the 'nent' field is adjusted and an error (ENOMEM) is returned.  If the
number is just right, the 'nent' field is adjusted to the number of valid
entries in the 'entries' array, which is then filled.
(用户态通过传入 kvm_cpuid2 结构体来调用 KVM_GET_SUPPORTED_CPUID，其中 'nent' 字段指明变长数组 'entries' 中的条目数量。如果条目数量太少而不足以描述 cpu 能力，则返回错误 (E2BIG)。如果数量太多，会调整 'nent' 字段并返回错误 (ENOMEM)。如果数量恰好，'nent' 字段会被调整为 'entries' 数组中有效条目的数量，然后填充该数组。)

The entries returned are the host cpuid as returned by the cpuid instruction,
with unknown or unsupported features masked out.  Some features (for example,
x2apic), may not be present in the host cpu, but are exposed by kvm if it can
emulate them efficiently. The fields in each entry are defined as follows:
(返回的条目就是 cpuid 指令返回的宿主机 cpuid，其中未知或不支持的特性被屏蔽掉。某些特性(例如 x2apic)可能在宿主机 cpu 上并不存在，但如果 kvm 能高效地模拟它们，就会暴露出来。每个条目中的字段定义如下：)

  function:
         the eax value used to obtain the entry
         (用于取得该条目的 eax 值)

  index:
         the ecx value used to obtain the entry (for entries that are
         affected by ecx)
         (用于取得该条目的 ecx 值(对受 ecx 影响的条目))

  flags:
     an OR of zero or more of the following:
     (以下零个或多个标志的按位或：)

        KVM_CPUID_FLAG_SIGNIFCANT_INDEX:
           if the index field is valid
           (如果 index 字段有效)

   eax, ebx, ecx, edx:
         the values returned by the cpuid instruction for
         this function/index combination
         (cpuid 指令针对该 function/index 组合返回的值)

x2APIC (CPUID leaf 1, ecx[21) and TSC deadline timer (CPUID leaf 1, ecx[24])
may be returned as true, but they depend on KVM_CREATE_IRQCHIP for in-kernel
emulation of the local APIC.  TSC deadline timer support is also reported via::

  ioctl(KVM_CHECK_EXTENSION, KVM_CAP_TSC_DEADLINE_TIMER)

(x2APIC (CPUID leaf 1, ecx[21) 与 TSC deadline timer (CPUID leaf 1, ecx[24]) 可能返回为 true，但它们依赖 KVM_CREATE_IRQCHIP 在内核内模拟本地 APIC。TSC deadline timer 支持还会通过以下调用报告：)

if that returns true and you use KVM_CREATE_IRQCHIP, or if you emulate the
feature in userspace, then you can enable the feature for KVM_SET_CPUID2.
(如果该调用返回 true 并且你使用 KVM_CREATE_IRQCHIP，或者你在用户态模拟该特性，那么就可以为 KVM_SET_CPUID2 启用该特性。)

Enabling x2APIC in KVM_SET_CPUID2 requires KVM_CREATE_IRQCHIP as KVM doesn't
support forwarding x2APIC MSR accesses to userspace, i.e. KVM does not support
emulating x2APIC in userspace.
(在 KVM_SET_CPUID2 中启用 x2APIC 需要 KVM_CREATE_IRQCHIP，因为 KVM 不支持把 x2APIC MSR 访问转发给用户态，即 KVM 不支持在用户态模拟 x2APIC。)

4.47 KVM_PPC_GET_PVINFO(4.47 KVM_PPC_GET_PVINFO)
------------------------------------------------

:Capability: KVM_CAP_PPC_GET_PVINFO
  (能力：KVM_CAP_PPC_GET_PVINFO)
:Architectures: ppc
  (架构：ppc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ppc_pvinfo (out)
  (参数：struct kvm_ppc_pvinfo (out))
:Returns: 0 on success, !0 on error
  (返回值：成功时为 0，出错时为 !0)

::

  struct kvm_ppc_pvinfo {
	__u32 flags;
	__u32 hcall[4];
	__u8  pad[108];
  };

This ioctl fetches PV specific information that need to be passed to the guest
using the device tree or other means from vm context.
(此 ioctl 从 vm 上下文获取需要通过设备树或其他方式传递给客户机的 PV 特定信息。)

The hcall array defines 4 instructions that make up a hypercall.
(hcall 数组定义了构成一个超调用的 4 条指令。)

If any additional field gets added to this structure later on, a bit for that
additional piece of information will be set in the flags bitmap.
(如果以后在此结构体中添加了任何额外字段，flags 位图中会为该额外信息设置相应的位。)

The flags bitmap is defined as::

   /* the host supports the ePAPR idle hcall
   #define KVM_PPC_PVINFO_FLAGS_EV_IDLE   (1<<0)

(flags 位图定义如下：)

4.52 KVM_SET_GSI_ROUTING(4.52 KVM_SET_GSI_ROUTING)
--------------------------------------------------

:Capability: KVM_CAP_IRQ_ROUTING
  (能力：KVM_CAP_IRQ_ROUTING)
:Architectures: x86 s390 arm64
  (架构：x86、s390、arm64)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_irq_routing (in)
  (参数：struct kvm_irq_routing (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Sets the GSI routing table entries, overwriting any previously set entries.
(设置 GSI 路由表条目，覆盖之前设置的任何条目。)

On arm64, GSI routing has the following limitation:
(在 arm64 上，GSI 路由有以下限制：)

- GSI routing does not apply to KVM_IRQ_LINE but only to KVM_IRQFD.
  (GSI 路由不适用于 KVM_IRQ_LINE，只适用于 KVM_IRQFD。)

::

  struct kvm_irq_routing {
	__u32 nr;
	__u32 flags;
	struct kvm_irq_routing_entry entries[0];
  };

No flags are specified so far, the corresponding field must be set to zero.
(到目前为止没有定义任何标志，相应字段必须置 0。)

::

  struct kvm_irq_routing_entry {
	__u32 gsi;
	__u32 type;
	__u32 flags;
	__u32 pad;
	union {
		struct kvm_irq_routing_irqchip irqchip;
		struct kvm_irq_routing_msi msi;
		struct kvm_irq_routing_s390_adapter adapter;
		struct kvm_irq_routing_hv_sint hv_sint;
		struct kvm_irq_routing_xen_evtchn xen_evtchn;
		__u32 pad[8];
	} u;
  };

  /* gsi routing entry types */
  #define KVM_IRQ_ROUTING_IRQCHIP 1
  #define KVM_IRQ_ROUTING_MSI 2
  #define KVM_IRQ_ROUTING_S390_ADAPTER 3
  #define KVM_IRQ_ROUTING_HV_SINT 4
  #define KVM_IRQ_ROUTING_XEN_EVTCHN 5

On s390, adding a KVM_IRQ_ROUTING_S390_ADAPTER is rejected on ucontrol VMs with
error -EINVAL.
(在 s390 上，在 ucontrol VM 上添加 KVM_IRQ_ROUTING_S390_ADAPTER 会被拒绝，并返回错误 -EINVAL。)

flags:
(标志：)

- KVM_MSI_VALID_DEVID: used along with KVM_IRQ_ROUTING_MSI routing entry
  type, specifies that the devid field contains a valid value.  The per-VM
  KVM_CAP_MSI_DEVID capability advertises the requirement to provide
  the device ID.  If this capability is not available, userspace should
  never set the KVM_MSI_VALID_DEVID flag as the ioctl might fail.
  (与 KVM_IRQ_ROUTING_MSI 路由条目类型一起使用，指定 devid 字段包含有效值。每 VM 的 KVM_CAP_MSI_DEVID 能力通告了提供设备 ID 的要求。如果该能力不可用，用户态绝不应设置 KVM_MSI_VALID_DEVID 标志，否则 ioctl 可能失败。)
- zero otherwise
  (否则为 0)

::

  struct kvm_irq_routing_irqchip {
	__u32 irqchip;
	__u32 pin;
  };

  struct kvm_irq_routing_msi {
	__u32 address_lo;
	__u32 address_hi;
	__u32 data;
	union {
		__u32 pad;
		__u32 devid;
	};
  };

If KVM_MSI_VALID_DEVID is set, devid contains a unique device identifier
for the device that wrote the MSI message.  For PCI, this is usually a
BDF identifier in the lower 16 bits.
(如果设置了 KVM_MSI_VALID_DEVID，devid 包含写入该 MSI 消息的设备的唯一设备标识符。对 PCI 来说，这通常是低 16 位中的 BDF 标识符。)

On x86, address_hi is ignored unless the KVM_X2APIC_API_USE_32BIT_IDS
feature of KVM_CAP_X2APIC_API capability is enabled.  If it is enabled,
address_hi bits 31-8 provide bits 31-8 of the destination id.  Bits 7-0 of
address_hi must be zero.
(在 x86 上，除非启用了 KVM_CAP_X2APIC_API 能力的 KVM_X2APIC_API_USE_32BIT_IDS 特性，否则 address_hi 会被忽略。如果已启用，address_hi 的 bits 31-8 提供目标 id 的 bits 31-8。address_hi 的 bits 7-0 必须为 0。)

::

  struct kvm_irq_routing_s390_adapter {
	__u64 ind_addr;
	__u64 summary_addr;
	__u64 ind_offset;
	__u32 summary_offset;
	__u32 adapter_id;
  };

  struct kvm_irq_routing_hv_sint {
	__u32 vcpu;
	__u32 sint;
  };

  struct kvm_irq_routing_xen_evtchn {
	__u32 port;
	__u32 vcpu;
	__u32 priority;
  };


When KVM_CAP_XEN_HVM includes the KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL bit
in its indication of supported features, routing to Xen event channels
is supported. Although the priority field is present, only the value
KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL is supported, which means delivery by
2 level event channels. FIFO event channel support may be added in
the future.
(当 KVM_CAP_XEN_HVM 在其支持特性指示中包含 KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL 位时，支持路由到 Xen 事件通道。虽然存在 priority 字段，但只支持值 KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL，即通过二级 (2 level) 事件通道投递。FIFO 事件通道支持将来可能会添加。)


4.55 KVM_SET_TSC_KHZ(4.55 KVM_SET_TSC_KHZ)
------------------------------------------

:Capability: KVM_CAP_TSC_CONTROL / KVM_CAP_VM_TSC_CONTROL
  (能力：KVM_CAP_TSC_CONTROL / KVM_CAP_VM_TSC_CONTROL)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl / vm ioctl
  (类型：vcpu ioctl / vm ioctl)
:Parameters: virtual tsc_khz
  (参数：virtual tsc_khz)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Specifies the tsc frequency for the virtual machine. The unit of the
frequency is KHz.
(指定虚拟机的 tsc 频率。频率单位是 KHz。)

If the KVM_CAP_VM_TSC_CONTROL capability is advertised, this can also
be used as a vm ioctl to set the initial tsc frequency of subsequently
created vCPUs.  Note, the vm ioctl is only allowed prior to creating vCPUs.
(如果通告了 KVM_CAP_VM_TSC_CONTROL 能力，它还可以作为 vm ioctl 使用，为后续创建的 vCPU 设置初始 tsc 频率。注意，vm ioctl 只允许在创建 vCPU 之前使用。)

For TSC protected Confidential Computing (CoCo) VMs where TSC frequency
is configured once at VM scope and remains unchanged during VM's
lifetime, the vm ioctl should be used to configure the TSC frequency
and the vcpu ioctl is not supported.
(对于 TSC 受保护的机密计算 (CoCo) VM——其 TSC 频率在 VM 范围内配置一次并在 VM 生命周期内保持不变——应使用 vm ioctl 配置 TSC 频率，不支持 vcpu ioctl。)

Example of such CoCo VMs: TDX guests.
(此类 CoCo VM 的例子：TDX 客户机。)

4.56 KVM_GET_TSC_KHZ(4.56 KVM_GET_TSC_KHZ)
------------------------------------------

:Capability: KVM_CAP_GET_TSC_KHZ / KVM_CAP_VM_TSC_CONTROL
  (能力：KVM_CAP_GET_TSC_KHZ / KVM_CAP_VM_TSC_CONTROL)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl / vm ioctl
  (类型：vcpu ioctl / vm ioctl)
:Parameters: none
  (参数：无)
:Returns: virtual tsc-khz on success, negative value on error
  (返回值：成功时为虚拟 tsc-khz，出错时为负值)

Returns the tsc frequency of the guest. The unit of the return value is
KHz. If the host has unstable tsc this ioctl returns -EIO instead as an
error.
(返回客户机的 tsc 频率。返回值单位是 KHz。如果宿主机的 tsc 不稳定，此 ioctl 会返回 -EIO 作为错误。)


4.57 KVM_GET_LAPIC(4.57 KVM_GET_LAPIC)
--------------------------------------

:Capability: KVM_CAP_IRQCHIP
  (能力：KVM_CAP_IRQCHIP)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_lapic_state (out)
  (参数：struct kvm_lapic_state (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  #define KVM_APIC_REG_SIZE 0x400
  struct kvm_lapic_state {
	char regs[KVM_APIC_REG_SIZE];
  };

Reads the Local APIC registers and copies them into the input argument.  The
data format and layout are the same as documented in the architecture manual.
(读取本地 APIC 寄存器并把它们复制到输入参数中。数据格式与布局与架构手册中的记载相同。)

If KVM_X2APIC_API_USE_32BIT_IDS feature of KVM_CAP_X2APIC_API is
enabled, then the format of APIC_ID register depends on the APIC mode
(reported by MSR_IA32_APICBASE) of its VCPU.  x2APIC stores APIC ID in
the APIC_ID register (bytes 32-35).  xAPIC only allows an 8-bit APIC ID
which is stored in bits 31-24 of the APIC register, or equivalently in
byte 35 of struct kvm_lapic_state's regs field.  KVM_GET_LAPIC must then
be called after MSR_IA32_APICBASE has been set with KVM_SET_MSR.
(如果启用了 KVM_CAP_X2APIC_API 的 KVM_X2APIC_API_USE_32BIT_IDS 特性，则 APIC_ID 寄存器的格式取决于其 VCPU 的 APIC 模式(由 MSR_IA32_APICBASE 报告)。x2APIC 把 APIC ID 存储在 APIC_ID 寄存器中(字节 32-35)。xAPIC 只允许 8 位 APIC ID，它存储在 APIC 寄存器的 bits 31-24 中，等价地存储在 struct kvm_lapic_state 的 regs 字段的字节 35 中。此时必须在用 KVM_SET_MSR 设置 MSR_IA32_APICBASE 之后调用 KVM_GET_LAPIC。)

If KVM_X2APIC_API_USE_32BIT_IDS feature is disabled, struct kvm_lapic_state
always uses xAPIC format.
(如果禁用了 KVM_X2APIC_API_USE_32BIT_IDS 特性，struct kvm_lapic_state 始终使用 xAPIC 格式。)


4.58 KVM_SET_LAPIC(4.58 KVM_SET_LAPIC)
--------------------------------------

:Capability: KVM_CAP_IRQCHIP
  (能力：KVM_CAP_IRQCHIP)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_lapic_state (in)
  (参数：struct kvm_lapic_state (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  #define KVM_APIC_REG_SIZE 0x400
  struct kvm_lapic_state {
	char regs[KVM_APIC_REG_SIZE];
  };

Copies the input argument into the Local APIC registers.  The data format
and layout are the same as documented in the architecture manual.
(把输入参数复制到本地 APIC 寄存器中。数据格式与布局与架构手册中的记载相同。)

The format of the APIC ID register (bytes 32-35 of struct kvm_lapic_state's
regs field) depends on the state of the KVM_CAP_X2APIC_API capability.
See the note in KVM_GET_LAPIC.
(APIC ID 寄存器(struct kvm_lapic_state 的 regs 字段的字节 32-35)的格式取决于 KVM_CAP_X2APIC_API 能力的状态。参见 KVM_GET_LAPIC 中的说明。)


4.59 KVM_IOEVENTFD(4.59 KVM_IOEVENTFD)
--------------------------------------

:Capability: KVM_CAP_IOEVENTFD
  (能力：KVM_CAP_IOEVENTFD)
:Architectures: all
  (架构：全部)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ioeventfd (in)
  (参数：struct kvm_ioeventfd (in))
:Returns: 0 on success, !0 on error
  (返回值：成功时为 0，出错时为 !0)

This ioctl attaches or detaches an ioeventfd to a legal pio/mmio address
within the guest.  A guest write in the registered address will signal the
provided event instead of triggering an exit.
(此 ioctl 把一个 ioeventfd 附着到客户机内合法的 pio/mmio 地址，或从中分离。客户机对已注册地址的写操作会触发所提供的 eventfd 信号，而不是引发一次退出。)

::

  struct kvm_ioeventfd {
	__u64 datamatch;
	__u64 addr;        /* legal pio/mmio address */
	__u32 len;         /* 0, 1, 2, 4, or 8 bytes    */
	__s32 fd;
	__u32 flags;
	__u8  pad[36];
  };

For the special case of virtio-ccw devices on s390, the ioevent is matched
to a subchannel/virtqueue tuple instead.
(对于 s390 上 virtio-ccw 设备的特殊情况，ioevent 改为匹配到子通道/virtqueue 元组。)

The following flags are defined::

  #define KVM_IOEVENTFD_FLAG_DATAMATCH (1 << kvm_ioeventfd_flag_nr_datamatch)
  #define KVM_IOEVENTFD_FLAG_PIO       (1 << kvm_ioeventfd_flag_nr_pio)
  #define KVM_IOEVENTFD_FLAG_DEASSIGN  (1 << kvm_ioeventfd_flag_nr_deassign)
  #define KVM_IOEVENTFD_FLAG_VIRTIO_CCW_NOTIFY \
	(1 << kvm_ioeventfd_flag_nr_virtio_ccw_notify)

(定义了以下标志：)

If datamatch flag is set, the event will be signaled only if the written value
to the registered address is equal to datamatch in struct kvm_ioeventfd.
(如果设置了 datamatch 标志，只有写入已注册地址的值等于 struct kvm_ioeventfd 中的 datamatch 时，事件才会被触发。)

For virtio-ccw devices, addr contains the subchannel id and datamatch the
virtqueue index.
(对 virtio-ccw 设备，addr 包含子通道 id，datamatch 包含 virtqueue 索引。)

With KVM_CAP_IOEVENTFD_ANY_LENGTH, a zero length ioeventfd is allowed, and
the kernel will ignore the length of guest write and may get a faster vmexit.
The speedup may only apply to specific architectures, but the ioeventfd will
work anyway.
(有了 KVM_CAP_IOEVENTFD_ANY_LENGTH，允许零长度的 ioeventfd，内核会忽略客户机写入的长度，并可能获得更快的 vmexit。加速可能只适用于特定架构，但 ioeventfd 无论如何都能正常工作。)

4.60 KVM_DIRTY_TLB(4.60 KVM_DIRTY_TLB)
--------------------------------------

:Capability: KVM_CAP_SW_TLB
  (能力：KVM_CAP_SW_TLB)
:Architectures: ppc
  (架构：ppc)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_dirty_tlb (in)
  (参数：struct kvm_dirty_tlb (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  struct kvm_dirty_tlb {
	__u64 bitmap;
	__u32 num_dirty;
  };

This must be called whenever userspace has changed an entry in the shared
TLB, prior to calling KVM_RUN on the associated vcpu.
(每当用户态修改了共享 TLB 中的条目时都必须调用此 ioctl，并且要在对关联 vcpu 调用 KVM_RUN 之前进行。)

The "bitmap" field is the userspace address of an array.  This array
consists of a number of bits, equal to the total number of TLB entries as
determined by the last successful call to ``KVM_ENABLE_CAP(KVM_CAP_SW_TLB)``,
rounded up to the nearest multiple of 64.
("bitmap" 字段是一个数组的用户态地址。该数组由若干位组成，位数等于由最近一次成功的 ``KVM_ENABLE_CAP(KVM_CAP_SW_TLB)`` 调用确定的 TLB 条目总数，并向上取整到 64 的最近倍数。)

Each bit corresponds to one TLB entry, ordered the same as in the shared TLB
array.
(每一位对应一个 TLB 条目，顺序与共享 TLB 数组中相同。)

The array is little-endian: the bit 0 is the least significant bit of the
first byte, bit 8 is the least significant bit of the second byte, etc.
This avoids any complications with differing word sizes.
(该数组为小端序：位 0 是第一个字节的最低有效位，位 8 是第二个字节的最低有效位，依此类推。这避免了字长不同带来的任何复杂问题。)

The "num_dirty" field is a performance hint for KVM to determine whether it
should skip processing the bitmap and just invalidate everything.  It must
be set to the number of set bits in the bitmap.
("num_dirty" 字段是给 KVM 的性能提示，用于判断是否应跳过位图处理而直接使全部内容失效。它必须设置为位图中已置位的数量。)


4.62 KVM_CREATE_SPAPR_TCE(4.62 KVM_CREATE_SPAPR_TCE)
----------------------------------------------------

:Capability: KVM_CAP_SPAPR_TCE
  (能力：KVM_CAP_SPAPR_TCE)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_create_spapr_tce (in)
  (参数：struct kvm_create_spapr_tce (in))
:Returns: file descriptor for manipulating the created TCE table
  (返回值：用于操作所创建 TCE 表的文件描述符)

This creates a virtual TCE (translation control entry) table, which
is an IOMMU for PAPR-style virtual I/O.  It is used to translate
logical addresses used in virtual I/O into guest physical addresses,
and provides a scatter/gather capability for PAPR virtual I/O.
(这会创建一个虚拟 TCE (translation control entry) 表，它是面向 PAPR 风格虚拟 I/O 的 IOMMU。它用于把虚拟 I/O 中使用的逻辑地址转换为客户机物理地址，并为 PAPR 虚拟 I/O 提供分散/聚集 (scatter/gather) 能力。)

::

  /* for KVM_CAP_SPAPR_TCE */
  struct kvm_create_spapr_tce {
	__u64 liobn;
	__u32 window_size;
  };

The liobn field gives the logical IO bus number for which to create a
TCE table.  The window_size field specifies the size of the DMA window
which this TCE table will translate - the table will contain one 64
bit TCE entry for every 4kiB of the DMA window.
(liobn 字段给出要为其创建 TCE 表的逻辑 IO 总线号。window_size 字段指定此 TCE 表要转换的 DMA 窗口大小——DMA 窗口每 4kiB 对应表中一个 64 位 TCE 条目。)

When the guest issues an H_PUT_TCE hcall on a liobn for which a TCE
table has been created using this ioctl(), the kernel will handle it
in real mode, updating the TCE table.  H_PUT_TCE calls for other
liobns will cause a vm exit and must be handled by userspace.
(当客户机在已用此 ioctl() 创建了 TCE 表的 liobn 上发出 H_PUT_TCE 超调用时，内核会以 real mode 处理它并更新 TCE 表。对其他 liobn 的 H_PUT_TCE 调用会引发 vm exit，必须由用户态处理。)

The return value is a file descriptor which can be passed to mmap(2)
to map the created TCE table into userspace.  This lets userspace read
the entries written by kernel-handled H_PUT_TCE calls, and also lets
userspace update the TCE table directly which is useful in some
circumstances.
(返回值是一个文件描述符，可以把它传给 mmap(2) 将创建的 TCE 表映射进用户态。这让用户态能读取由内核处理的 H_PUT_TCE 调用写入的条目，也让用户态能直接更新 TCE 表——这在某些情况下很有用。)


4.64 KVM_NMI(4.64 KVM_NMI)
--------------------------

:Capability: KVM_CAP_USER_NMI
  (能力：KVM_CAP_USER_NMI)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: none
  (参数：无)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Queues an NMI on the thread's vcpu.  Note this is well defined only
when KVM_CREATE_IRQCHIP has not been called, since this is an interface
between the virtual cpu core and virtual local APIC.  After KVM_CREATE_IRQCHIP
has been called, this interface is completely emulated within the kernel.
(在该线程的 vcpu 上把一个 NMI 排队。注意，只有在尚未调用 KVM_CREATE_IRQCHIP 时其行为才有明确界定，因为它是虚拟 cpu 核心与虚拟本地 APIC 之间的接口。调用 KVM_CREATE_IRQCHIP 之后，该接口完全在内核内模拟。)

To use this to emulate the LINT1 input with KVM_CREATE_IRQCHIP, use the
following algorithm:
(要在使用 KVM_CREATE_IRQCHIP 的情况下用它模拟 LINT1 输入，可使用以下算法：)

  - pause the vcpu
    (暂停 vcpu)
  - read the local APIC's state (KVM_GET_LAPIC)
    (读取本地 APIC 的状态 (KVM_GET_LAPIC))
  - check whether changing LINT1 will queue an NMI (see the LVT entry for LINT1)
    (检查改变 LINT1 是否会使一个 NMI 入队(参见 LINT1 的 LVT 条目))
  - if so, issue KVM_NMI
    (如果是，则发出 KVM_NMI)
  - resume the vcpu
    (恢复 vcpu 运行)

Some guests configure the LINT1 NMI input to cause a panic, aiding in
debugging.
(某些客户机会把 LINT1 NMI 输入配置为引发 panic，以便于调试。)


4.65 KVM_S390_UCAS_MAP(4.65 KVM_S390_UCAS_MAP)
----------------------------------------------

:Capability: KVM_CAP_S390_UCONTROL
  (能力：KVM_CAP_S390_UCONTROL)
:Architectures: s390
  (架构：s390)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_s390_ucas_mapping (in)
  (参数：struct kvm_s390_ucas_mapping (in))
:Returns: 0 in case of success
  (返回值：成功时为 0)

The parameter is defined like this::

	struct kvm_s390_ucas_mapping {
		__u64 user_addr;
		__u64 vcpu_addr;
		__u64 length;
	};

(参数定义如下：)

This ioctl maps the memory at "user_addr" with the length "length" to
the vcpu's address space starting at "vcpu_addr". All parameters need to
be aligned by 1 megabyte.
(此 ioctl 把 "user_addr" 处长度为 "length" 的内存映射到 vcpu 地址空间中从 "vcpu_addr" 开始的位置。所有参数都需要按 1 MB 对齐。)


4.66 KVM_S390_UCAS_UNMAP(4.66 KVM_S390_UCAS_UNMAP)
--------------------------------------------------

:Capability: KVM_CAP_S390_UCONTROL
  (能力：KVM_CAP_S390_UCONTROL)
:Architectures: s390
  (架构：s390)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_s390_ucas_mapping (in)
  (参数：struct kvm_s390_ucas_mapping (in))
:Returns: 0 in case of success
  (返回值：成功时为 0)

The parameter is defined like this::

	struct kvm_s390_ucas_mapping {
		__u64 user_addr;
		__u64 vcpu_addr;
		__u64 length;
	};

(参数定义如下：)

This ioctl unmaps the memory in the vcpu's address space starting at
"vcpu_addr" with the length "length". The field "user_addr" is ignored.
All parameters need to be aligned by 1 megabyte.
(此 ioctl 从 vcpu 地址空间中解除从 "vcpu_addr" 开始、长度为 "length" 的内存的映射。字段 "user_addr" 被忽略。所有参数都需要按 1 MB 对齐。)


4.67 KVM_S390_VCPU_FAULT(4.67 KVM_S390_VCPU_FAULT)
--------------------------------------------------

:Capability: KVM_CAP_S390_UCONTROL
  (能力：KVM_CAP_S390_UCONTROL)
:Architectures: s390
  (架构：s390)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: vcpu absolute address (in)
  (参数：vcpu absolute address (in))
:Returns: 0 in case of success
  (返回值：成功时为 0)

This call creates a page table entry on the virtual cpu's address space
(for user controlled virtual machines) or the virtual machine's address
space (for regular virtual machines). This only works for minor faults,
thus it's recommended to access subject memory page via the user page
table upfront. This is useful to handle validity intercepts for user
controlled virtual machines to fault in the virtual cpu's lowcore pages
prior to calling the KVM_RUN ioctl.
(此调用在虚拟 cpu 的地址空间(用户可控虚拟机)或虚拟机的地址空间(常规虚拟机)上创建一个页表项。它只对次缺页 (minor fault) 有效，因此建议先通过用户页表访问相关内存页。它可用于处理用户可控虚拟机的 validity 截获，在调用 KVM_RUN ioctl 之前把虚拟 cpu 的 lowcore 页调入。)


4.68 KVM_SET_ONE_REG(4.68 KVM_SET_ONE_REG)
------------------------------------------

:Capability: KVM_CAP_ONE_REG
  (能力：KVM_CAP_ONE_REG)
:Architectures: all
  (架构：全部)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_one_reg (in)
  (参数：struct kvm_one_reg (in))
:Returns: 0 on success, negative value on failure
  (返回值：成功时为 0，失败时为负值)

Errors:
(错误：)

  ======   ============================================================
  ENOENT   no such register
           (不存在这样的寄存器)
  EINVAL   invalid register ID, or no such register or used with VMs in
           protected virtualization mode on s390
           (寄存器 ID 无效，或不存在这样的寄存器，或在 s390 上用于受保护虚拟化模式的 VM)
  EPERM    (arm64) register access not allowed before vcpu finalization
           ((arm64) 在 vcpu 最终确定之前不允许访问寄存器)
  EBUSY    (riscv) changing register value not allowed after the vcpu
           has run at least once
           ((riscv) 在 vcpu 至少运行过一次之后不允许更改寄存器值)
  ======   ============================================================

(These error codes are indicative only: do not rely on a specific error
code being returned in a specific situation.)
(这些错误码仅供参考：不要依赖在特定情况下一定返回某个特定错误码。)

::

  struct kvm_one_reg {
       __u64 id;
       __u64 addr;
 };

Using this ioctl, a single vcpu register can be set to a specific value
defined by user space with the passed in struct kvm_one_reg, where id
refers to the register identifier as described below and addr is a pointer
to a variable with the respective size. There can be architecture agnostic
and architecture specific registers. Each have their own range of operation
and their own constants and width. To keep track of the implemented
registers, find a list below:
(使用此 ioctl，可以用传入的 struct kvm_one_reg 把单个 vcpu 寄存器设置为用户空间定义的特定值，其中 id 指下文描述的寄存器标识符，addr 是指向相应大小变量的指针。可以存在架构无关和架构特定的寄存器。它们各有自己的作用范围、常量和位宽。要了解已实现的寄存器，请看下面的列表：)

  ======= =============================== ============
  Arch              Register              Width (bits)
  ======= =============================== ============
  PPC     KVM_REG_PPC_HIOR                64
  PPC     KVM_REG_PPC_IAC1                64
  PPC     KVM_REG_PPC_IAC2                64
  PPC     KVM_REG_PPC_IAC3                64
  PPC     KVM_REG_PPC_IAC4                64
  PPC     KVM_REG_PPC_DAC1                64
  PPC     KVM_REG_PPC_DAC2                64
  PPC     KVM_REG_PPC_DABR                64
  PPC     KVM_REG_PPC_DSCR                64
  PPC     KVM_REG_PPC_PURR                64
  PPC     KVM_REG_PPC_SPURR               64
  PPC     KVM_REG_PPC_DAR                 64
  PPC     KVM_REG_PPC_DSISR               32
  PPC     KVM_REG_PPC_AMR                 64
  PPC     KVM_REG_PPC_UAMOR               64
  PPC     KVM_REG_PPC_MMCR0               64
  PPC     KVM_REG_PPC_MMCR1               64
  PPC     KVM_REG_PPC_MMCRA               64
  PPC     KVM_REG_PPC_MMCR2               64
  PPC     KVM_REG_PPC_MMCRS               64
  PPC     KVM_REG_PPC_MMCR3               64
  PPC     KVM_REG_PPC_SIAR                64
  PPC     KVM_REG_PPC_SDAR                64
  PPC     KVM_REG_PPC_SIER                64
  PPC     KVM_REG_PPC_SIER2               64
  PPC     KVM_REG_PPC_SIER3               64
  PPC     KVM_REG_PPC_PMC1                32
  PPC     KVM_REG_PPC_PMC2                32
  PPC     KVM_REG_PPC_PMC3                32
  PPC     KVM_REG_PPC_PMC4                32
  PPC     KVM_REG_PPC_PMC5                32
  PPC     KVM_REG_PPC_PMC6                32
  PPC     KVM_REG_PPC_PMC7                32
  PPC     KVM_REG_PPC_PMC8                32
  PPC     KVM_REG_PPC_FPR0                64
  ...
  PPC     KVM_REG_PPC_FPR31               64
  PPC     KVM_REG_PPC_VR0                 128
  ...
  PPC     KVM_REG_PPC_VR31                128
  PPC     KVM_REG_PPC_VSR0                128
  ...
  PPC     KVM_REG_PPC_VSR31               128
  PPC     KVM_REG_PPC_FPSCR               64
  PPC     KVM_REG_PPC_VSCR                32
  PPC     KVM_REG_PPC_VPA_ADDR            64
  PPC     KVM_REG_PPC_VPA_SLB             128
  PPC     KVM_REG_PPC_VPA_DTL             128
  PPC     KVM_REG_PPC_EPCR                32
  PPC     KVM_REG_PPC_EPR                 32
  PPC     KVM_REG_PPC_TCR                 32
  PPC     KVM_REG_PPC_TSR                 32
  PPC     KVM_REG_PPC_OR_TSR              32
  PPC     KVM_REG_PPC_CLEAR_TSR           32
  PPC     KVM_REG_PPC_MAS0                32
  PPC     KVM_REG_PPC_MAS1                32
  PPC     KVM_REG_PPC_MAS2                64
  PPC     KVM_REG_PPC_MAS7_3              64
  PPC     KVM_REG_PPC_MAS4                32
  PPC     KVM_REG_PPC_MAS6                32
  PPC     KVM_REG_PPC_MMUCFG              32
  PPC     KVM_REG_PPC_TLB0CFG             32
  PPC     KVM_REG_PPC_TLB1CFG             32
  PPC     KVM_REG_PPC_TLB2CFG             32
  PPC     KVM_REG_PPC_TLB3CFG             32
  PPC     KVM_REG_PPC_TLB0PS              32
  PPC     KVM_REG_PPC_TLB1PS              32
  PPC     KVM_REG_PPC_TLB2PS              32
  PPC     KVM_REG_PPC_TLB3PS              32
  PPC     KVM_REG_PPC_EPTCFG              32
  PPC     KVM_REG_PPC_ICP_STATE           64
  PPC     KVM_REG_PPC_VP_STATE            128
  PPC     KVM_REG_PPC_TB_OFFSET           64
  PPC     KVM_REG_PPC_SPMC1               32
  PPC     KVM_REG_PPC_SPMC2               32
  PPC     KVM_REG_PPC_IAMR                64
  PPC     KVM_REG_PPC_TFHAR               64
  PPC     KVM_REG_PPC_TFIAR               64
  PPC     KVM_REG_PPC_TEXASR              64
  PPC     KVM_REG_PPC_FSCR                64
  PPC     KVM_REG_PPC_PSPB                32
  PPC     KVM_REG_PPC_EBBHR               64
  PPC     KVM_REG_PPC_EBBRR               64
  PPC     KVM_REG_PPC_BESCR               64
  PPC     KVM_REG_PPC_TAR                 64
  PPC     KVM_REG_PPC_DPDES               64
  PPC     KVM_REG_PPC_DAWR                64
  PPC     KVM_REG_PPC_DAWRX               64
  PPC     KVM_REG_PPC_CIABR               64
  PPC     KVM_REG_PPC_IC                  64
  PPC     KVM_REG_PPC_VTB                 64
  PPC     KVM_REG_PPC_CSIGR               64
  PPC     KVM_REG_PPC_TACR                64
  PPC     KVM_REG_PPC_TCSCR               64
  PPC     KVM_REG_PPC_PID                 64
  PPC     KVM_REG_PPC_ACOP                64
  PPC     KVM_REG_PPC_VRSAVE              32
  PPC     KVM_REG_PPC_LPCR                32
  PPC     KVM_REG_PPC_LPCR_64             64
  PPC     KVM_REG_PPC_PPR                 64
  PPC     KVM_REG_PPC_ARCH_COMPAT         32
  PPC     KVM_REG_PPC_DABRX               32
  PPC     KVM_REG_PPC_WORT                64
  PPC	  KVM_REG_PPC_SPRG9               64
  PPC	  KVM_REG_PPC_DBSR                32
  PPC     KVM_REG_PPC_TIDR                64
  PPC     KVM_REG_PPC_PSSCR               64
  PPC     KVM_REG_PPC_DEC_EXPIRY          64
  PPC     KVM_REG_PPC_PTCR                64
  PPC     KVM_REG_PPC_HASHKEYR            64
  PPC     KVM_REG_PPC_HASHPKEYR           64
  PPC     KVM_REG_PPC_DAWR1               64
  PPC     KVM_REG_PPC_DAWRX1              64
  PPC     KVM_REG_PPC_DEXCR               64
  PPC     KVM_REG_PPC_TM_GPR0             64
  ...
  PPC     KVM_REG_PPC_TM_GPR31            64
  PPC     KVM_REG_PPC_TM_VSR0             128
  ...
  PPC     KVM_REG_PPC_TM_VSR63            128
  PPC     KVM_REG_PPC_TM_CR               64
  PPC     KVM_REG_PPC_TM_LR               64
  PPC     KVM_REG_PPC_TM_CTR              64
  PPC     KVM_REG_PPC_TM_FPSCR            64
  PPC     KVM_REG_PPC_TM_AMR              64
  PPC     KVM_REG_PPC_TM_PPR              64
  PPC     KVM_REG_PPC_TM_VRSAVE           64
  PPC     KVM_REG_PPC_TM_VSCR             32
  PPC     KVM_REG_PPC_TM_DSCR             64
  PPC     KVM_REG_PPC_TM_TAR              64
  PPC     KVM_REG_PPC_TM_XER              64

  MIPS    KVM_REG_MIPS_R0                 64
  ...
  MIPS    KVM_REG_MIPS_R31                64
  MIPS    KVM_REG_MIPS_HI                 64
  MIPS    KVM_REG_MIPS_LO                 64
  MIPS    KVM_REG_MIPS_PC                 64
  MIPS    KVM_REG_MIPS_CP0_INDEX          32
  MIPS    KVM_REG_MIPS_CP0_ENTRYLO0       64
  MIPS    KVM_REG_MIPS_CP0_ENTRYLO1       64
  MIPS    KVM_REG_MIPS_CP0_CONTEXT        64
  MIPS    KVM_REG_MIPS_CP0_CONTEXTCONFIG  32
  MIPS    KVM_REG_MIPS_CP0_USERLOCAL      64
  MIPS    KVM_REG_MIPS_CP0_XCONTEXTCONFIG 64
  MIPS    KVM_REG_MIPS_CP0_PAGEMASK       32
  MIPS    KVM_REG_MIPS_CP0_PAGEGRAIN      32
  MIPS    KVM_REG_MIPS_CP0_SEGCTL0        64
  MIPS    KVM_REG_MIPS_CP0_SEGCTL1        64
  MIPS    KVM_REG_MIPS_CP0_SEGCTL2        64
  MIPS    KVM_REG_MIPS_CP0_PWBASE         64
  MIPS    KVM_REG_MIPS_CP0_PWFIELD        64
  MIPS    KVM_REG_MIPS_CP0_PWSIZE         64
  MIPS    KVM_REG_MIPS_CP0_WIRED          32
  MIPS    KVM_REG_MIPS_CP0_PWCTL          32
  MIPS    KVM_REG_MIPS_CP0_HWRENA         32
  MIPS    KVM_REG_MIPS_CP0_BADVADDR       64
  MIPS    KVM_REG_MIPS_CP0_BADINSTR       32
  MIPS    KVM_REG_MIPS_CP0_BADINSTRP      32
  MIPS    KVM_REG_MIPS_CP0_COUNT          32
  MIPS    KVM_REG_MIPS_CP0_ENTRYHI        64
  MIPS    KVM_REG_MIPS_CP0_COMPARE        32
  MIPS    KVM_REG_MIPS_CP0_STATUS         32
  MIPS    KVM_REG_MIPS_CP0_INTCTL         32
  MIPS    KVM_REG_MIPS_CP0_CAUSE          32
  MIPS    KVM_REG_MIPS_CP0_EPC            64
  MIPS    KVM_REG_MIPS_CP0_PRID           32
  MIPS    KVM_REG_MIPS_CP0_EBASE          64
  MIPS    KVM_REG_MIPS_CP0_CONFIG         32
  MIPS    KVM_REG_MIPS_CP0_CONFIG1        32
  MIPS    KVM_REG_MIPS_CP0_CONFIG2        32
  MIPS    KVM_REG_MIPS_CP0_CONFIG3        32
  MIPS    KVM_REG_MIPS_CP0_CONFIG4        32
  MIPS    KVM_REG_MIPS_CP0_CONFIG5        32
  MIPS    KVM_REG_MIPS_CP0_CONFIG7        32
  MIPS    KVM_REG_MIPS_CP0_XCONTEXT       64
  MIPS    KVM_REG_MIPS_CP0_ERROREPC       64
  MIPS    KVM_REG_MIPS_CP0_KSCRATCH1      64
  MIPS    KVM_REG_MIPS_CP0_KSCRATCH2      64
  MIPS    KVM_REG_MIPS_CP0_KSCRATCH3      64
  MIPS    KVM_REG_MIPS_CP0_KSCRATCH4      64
  MIPS    KVM_REG_MIPS_CP0_KSCRATCH5      64
  MIPS    KVM_REG_MIPS_CP0_KSCRATCH6      64
  MIPS    KVM_REG_MIPS_CP0_MAAR(0..63)    64
  MIPS    KVM_REG_MIPS_COUNT_CTL          64
  MIPS    KVM_REG_MIPS_COUNT_RESUME       64
  MIPS    KVM_REG_MIPS_COUNT_HZ           64
  MIPS    KVM_REG_MIPS_FPR_32(0..31)      32
  MIPS    KVM_REG_MIPS_FPR_64(0..31)      64
  MIPS    KVM_REG_MIPS_VEC_128(0..31)     128
  MIPS    KVM_REG_MIPS_FCR_IR             32
  MIPS    KVM_REG_MIPS_FCR_CSR            32
  MIPS    KVM_REG_MIPS_MSA_IR             32
  MIPS    KVM_REG_MIPS_MSA_CSR            32
  ======= =============================== ============

ARM registers are mapped using the lower 32 bits.  The upper 16 of that
is the register group type, or coprocessor number:
(ARM 寄存器使用低 32 位进行映射。其中高 16 位是寄存器组类型，即协处理器编号：)

ARM core registers have the following id bit patterns::

  0x4020 0000 0010 <index into the kvm_regs struct:16>

(ARM 核心寄存器有以下 id 位模式：)

ARM 32-bit CP15 registers have the following id bit patterns::

  0x4020 0000 000F <zero:1> <crn:4> <crm:4> <opc1:4> <opc2:3>

(ARM 32 位 CP15 寄存器有以下 id 位模式：)

ARM 64-bit CP15 registers have the following id bit patterns::

  0x4030 0000 000F <zero:1> <zero:4> <crm:4> <opc1:4> <zero:3>

(ARM 64 位 CP15 寄存器有以下 id 位模式：)

ARM CCSIDR registers are demultiplexed by CSSELR value::

  0x4020 0000 0011 00 <csselr:8>

(ARM CCSIDR 寄存器按 CSSELR 值解复用：)

ARM 32-bit VFP control registers have the following id bit patterns::

  0x4020 0000 0012 1 <regno:12>

(ARM 32 位 VFP 控制寄存器有以下 id 位模式：)

ARM 64-bit FP registers have the following id bit patterns::

  0x4030 0000 0012 0 <regno:12>

(ARM 64 位 FP 寄存器有以下 id 位模式：)

ARM firmware pseudo-registers have the following bit pattern::

  0x4030 0000 0014 <regno:16>

(ARM 固件伪寄存器有以下位模式：)

arm64 registers are mapped using the lower 32 bits. The upper 16 of
that is the register group type, or coprocessor number:
(arm64 寄存器使用低 32 位进行映射。其中高 16 位是寄存器组类型，即协处理器编号：)

arm64 core/FP-SIMD registers have the following id bit patterns. Note
that the size of the access is variable, as the kvm_regs structure
contains elements ranging from 32 to 128 bits. The index is a 32bit
value in the kvm_regs structure seen as a 32bit array::

  0x60x0 0000 0010 <index into the kvm_regs struct:16>

(arm64 核心/FP-SIMD 寄存器有以下 id 位模式。注意，访问的大小是可变的，因为 kvm_regs 结构体包含从 32 位到 128 位的元素。索引是 kvm_regs 结构体按 32 位数组看待时的 32 位值：)

Specifically:
(具体如下：)

======================= ========= ===== =======================================
    Encoding            Register  Bits  kvm_regs member
======================= ========= ===== =======================================
  0x6030 0000 0010 0000 X0          64  regs.regs[0]
  0x6030 0000 0010 0002 X1          64  regs.regs[1]
  ...
  0x6030 0000 0010 003c X30         64  regs.regs[30]
  0x6030 0000 0010 003e SP          64  regs.sp
  0x6030 0000 0010 0040 PC          64  regs.pc
  0x6030 0000 0010 0042 PSTATE      64  regs.pstate
  0x6030 0000 0010 0044 SP_EL1      64  sp_el1
  0x6030 0000 0010 0046 ELR_EL1     64  elr_el1
  0x6030 0000 0010 0048 SPSR_EL1    64  spsr[KVM_SPSR_EL1] (alias SPSR_SVC)
  0x6030 0000 0010 004a SPSR_ABT    64  spsr[KVM_SPSR_ABT]
  0x6030 0000 0010 004c SPSR_UND    64  spsr[KVM_SPSR_UND]
  0x6030 0000 0010 004e SPSR_IRQ    64  spsr[KVM_SPSR_IRQ]
  0x6030 0000 0010 0050 SPSR_FIQ    64  spsr[KVM_SPSR_FIQ]
  0x6040 0000 0010 0054 V0         128  fp_regs.vregs[0]    [1]_
  0x6040 0000 0010 0058 V1         128  fp_regs.vregs[1]    [1]_
  ...
  0x6040 0000 0010 00d0 V31        128  fp_regs.vregs[31]   [1]_
  0x6020 0000 0010 00d4 FPSR        32  fp_regs.fpsr
  0x6020 0000 0010 00d5 FPCR        32  fp_regs.fpcr
======================= ========= ===== =======================================

.. [1] These encodings are not accepted for SVE-enabled vcpus.  See
       :ref:`KVM_ARM_VCPU_INIT`.
       (这些编码不会被启用了 SVE 的 vcpu 接受。参见 KVM_ARM_VCPU_INIT 一节。)

       The equivalent register content can be accessed via bits [127:0] of
       the corresponding SVE Zn registers instead for vcpus that have SVE
       enabled (see below).
       (对于启用了 SVE 的 vcpu，可以改为通过对应 SVE Zn 寄存器的 bits [127:0] 访问等效的寄存器内容(见下文)。)

arm64 CCSIDR registers are demultiplexed by CSSELR value::

  0x6020 0000 0011 00 <csselr:8>

(arm64 CCSIDR 寄存器按 CSSELR 值解复用：)

arm64 system registers have the following id bit patterns::

  0x6030 0000 0013 <op0:2> <op1:3> <crn:4> <crm:4> <op2:3>

(arm64 系统寄存器有以下 id 位模式：)

.. warning::

     Two system register IDs do not follow the specified pattern.  These
     are KVM_REG_ARM_TIMER_CVAL and KVM_REG_ARM_TIMER_CNT, which map to
     system registers CNTV_CVAL_EL0 and CNTVCT_EL0 respectively.  These
     two had their values accidentally swapped, which means TIMER_CVAL is
     derived from the register encoding for CNTVCT_EL0 and TIMER_CNT is
     derived from the register encoding for CNTV_CVAL_EL0.  As this is
     API, it must remain this way.
     (警告：有两个系统寄存器 ID 不遵循规定的模式。它们是 KVM_REG_ARM_TIMER_CVAL 和 KVM_REG_ARM_TIMER_CNT，分别映射到系统寄存器 CNTV_CVAL_EL0 和 CNTVCT_EL0。这两个寄存器的值被意外地对调了，即 TIMER_CVAL 由 CNTVCT_EL0 的寄存器编码派生，而 TIMER_CNT 由 CNTV_CVAL_EL0 的寄存器编码派生。由于这已成 API，必须保持原样。)

arm64 firmware pseudo-registers have the following bit pattern::

  0x6030 0000 0014 <regno:16>

(arm64 固件伪寄存器有以下位模式：)

arm64 SVE registers have the following bit patterns::

  0x6080 0000 0015 00 <n:5> <slice:5>   Zn bits[2048*slice + 2047 : 2048*slice]
  0x6050 0000 0015 04 <n:4> <slice:5>   Pn bits[256*slice + 255 : 256*slice]
  0x6050 0000 0015 060 <slice:5>        FFR bits[256*slice + 255 : 256*slice]
  0x6060 0000 0015 ffff                 KVM_REG_ARM64_SVE_VLS pseudo-register

(arm64 SVE 寄存器有以下位模式：)

Access to register IDs where 2048 * slice >= 128 * max_vq will fail with
ENOENT.  max_vq is the vcpu's maximum supported vector length in 128-bit
quadwords: see [2]_ below.
(对满足 2048 * slice >= 128 * max_vq 的寄存器 ID 的访问会以 ENOENT 失败。max_vq 是该 vcpu 支持的最大向量长度(以 128 位四字为单位)：参见下文脚注 2。)

These registers are only accessible on vcpus for which SVE is enabled.
See KVM_ARM_VCPU_INIT for details.
(这些寄存器只在启用了 SVE 的 vcpu 上可访问。详细信息参见 KVM_ARM_VCPU_INIT。)

In addition, except for KVM_REG_ARM64_SVE_VLS, these registers are not
accessible until the vcpu's SVE configuration has been finalized
using KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE).  See KVM_ARM_VCPU_INIT
and KVM_ARM_VCPU_FINALIZE for more information about this procedure.
(另外，除 KVM_REG_ARM64_SVE_VLS 之外，在用 KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE) 最终确定 vcpu 的 SVE 配置之前，这些寄存器不可访问。关于此过程的更多信息参见 KVM_ARM_VCPU_INIT 与 KVM_ARM_VCPU_FINALIZE。)

KVM_REG_ARM64_SVE_VLS is a pseudo-register that allows the set of vector
lengths supported by the vcpu to be discovered and configured by
userspace.  When transferred to or from user memory via KVM_GET_ONE_REG
or KVM_SET_ONE_REG, the value of this register is of type
__u64[KVM_ARM64_SVE_VLS_WORDS], and encodes the set of vector lengths as
follows::

  __u64 vector_lengths[KVM_ARM64_SVE_VLS_WORDS];

  if (vq >= SVE_VQ_MIN && vq <= SVE_VQ_MAX &&
      ((vector_lengths[(vq - KVM_ARM64_SVE_VQ_MIN) / 64] >>
		((vq - KVM_ARM64_SVE_VQ_MIN) % 64)) & 1))
	/* Vector length vq * 16 bytes supported */
  else
	/* Vector length vq * 16 bytes not supported */

(KVM_REG_ARM64_SVE_VLS 是一个伪寄存器，允许用户空间发现并配置该 vcpu 支持的向量长度集合。通过 KVM_GET_ONE_REG 或 KVM_SET_ONE_REG 与用户内存之间传输时，此寄存器的值类型为 __u64[KVM_ARM64_SVE_VLS_WORDS]，并按如下方式对向量长度集合进行编码：)

.. [2] The maximum value vq for which the above condition is true is
       max_vq.  This is the maximum vector length available to the guest on
       this vcpu, and determines which register slices are visible through
       this ioctl interface.
       (使上述条件成立的 vq 最大值即 max_vq。它是该 vcpu 上客户机可用的最大向量长度，并决定了通过此 ioctl 接口可见的寄存器分片。)

(See Documentation/arch/arm64/sve.rst for an explanation of the "vq"
nomenclature.)
(参见 Documentation/arch/arm64/sve.rst 了解 "vq" 命名法的解释。)

KVM_REG_ARM64_SVE_VLS is only accessible after KVM_ARM_VCPU_INIT.
KVM_ARM_VCPU_INIT initialises it to the best set of vector lengths that
the host supports.
(KVM_REG_ARM64_SVE_VLS 只能在 KVM_ARM_VCPU_INIT 之后访问。KVM_ARM_VCPU_INIT 会把它初始化为宿主机支持的最佳向量长度集合。)

Userspace may subsequently modify it if desired until the vcpu's SVE
configuration is finalized using KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE).
(此后用户态可以在需要时修改它，直到使用 KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE) 最终确定 vcpu 的 SVE 配置为止。)

Apart from simply removing all vector lengths from the host set that
exceed some value, support for arbitrarily chosen sets of vector lengths
is hardware-dependent and may not be available.  Attempting to configure
an invalid set of vector lengths via KVM_SET_ONE_REG will fail with
EINVAL.
(除了简单地从宿主机集合中去掉超过某个值的所有向量长度之外，对任意选定向量长度集合的支持取决于硬件，可能不可用。尝试通过 KVM_SET_ONE_REG 配置无效的向量长度集合会以 EINVAL 失败。)

After the vcpu's SVE configuration is finalized, further attempts to
write this register will fail with EPERM.
(在 vcpu 的 SVE 配置最终确定之后，再尝试写此寄存器会以 EPERM 失败。)

arm64 bitmap feature firmware pseudo-registers have the following bit pattern::

  0x6030 0000 0016 <regno:16>

(arm64 位图特性固件伪寄存器有以下位模式：)

The bitmap feature firmware registers exposes the hypercall services that
are available for userspace to configure. The set bits corresponds to the
services that are available for the guests to access. By default, KVM
sets all the supported bits during VM initialization. The userspace can
discover the available services via KVM_GET_ONE_REG, and write back the
bitmap corresponding to the features that it wishes guests to see via
KVM_SET_ONE_REG.
(位图特性固件寄存器暴露了可供用户态配置的超调用服务。置位的位对应客户机可以访问的服务。默认情况下，KVM 会在 VM 初始化期间设置所有受支持的位。用户态可以通过 KVM_GET_ONE_REG 发现可用服务，并通过 KVM_SET_ONE_REG 写回与其希望客户机看到的特性相对应的位图。)

Note: These registers are immutable once any of the vCPUs of the VM has
run at least once. A KVM_SET_ONE_REG in such a scenario will return
a -EBUSY to userspace.
(注意：一旦 VM 的任何 vCPU 至少运行过一次，这些寄存器就不可再变。这种情况下调用 KVM_SET_ONE_REG 会向用户态返回 -EBUSY。)

(See Documentation/virt/kvm/arm/hypercalls.rst for more details.)
(更多细节参见 Documentation/virt/kvm/arm/hypercalls.rst。)


MIPS registers are mapped using the lower 32 bits.  The upper 16 of that is
the register group type:
(MIPS 寄存器使用低 32 位进行映射。其中高 16 位是寄存器组类型：)

MIPS core registers (see above) have the following id bit patterns::

  0x7030 0000 0000 <reg:16>

(MIPS 核心寄存器(见上文)有以下 id 位模式：)

MIPS CP0 registers (see KVM_REG_MIPS_CP0_* above) have the following id bit
patterns depending on whether they're 32-bit or 64-bit registers::

  0x7020 0000 0001 00 <reg:5> <sel:3>   (32-bit)
  0x7030 0000 0001 00 <reg:5> <sel:3>   (64-bit)

(MIPS CP0 寄存器(见上文 KVM_REG_MIPS_CP0_*)根据其是 32 位还是 64 位寄存器而有以下 id 位模式：)

Note: KVM_REG_MIPS_CP0_ENTRYLO0 and KVM_REG_MIPS_CP0_ENTRYLO1 are the MIPS64
versions of the EntryLo registers regardless of the word size of the host
hardware, host kernel, guest, and whether XPA is present in the guest, i.e.
with the RI and XI bits (if they exist) in bits 63 and 62 respectively, and
the PFNX field starting at bit 30.
(注意：无论宿主机硬件、宿主内核和客户机的字长如何，也不管客户机是否存在 XPA，KVM_REG_MIPS_CP0_ENTRYLO0 与 KVM_REG_MIPS_CP0_ENTRYLO1 都是 EntryLo 寄存器的 MIPS64 版本，即 RI 和 XI 位(如果存在)分别位于 bits 63 和 62，PFNX 字段从位 30 开始。)

MIPS MAARs (see KVM_REG_MIPS_CP0_MAAR(*) above) have the following id bit
patterns::

  0x7030 0000 0001 01 <reg:8>

(MIPS MAAR(见上文 KVM_REG_MIPS_CP0_MAAR(*))有以下 id 位模式：)

MIPS KVM control registers (see above) have the following id bit patterns::

  0x7030 0000 0002 <reg:16>

(MIPS KVM 控制寄存器(见上文)有以下 id 位模式：)

MIPS FPU registers (see KVM_REG_MIPS_FPR_{32,64}() above) have the following
id bit patterns depending on the size of the register being accessed. They are
always accessed according to the current guest FPU mode (Status.FR and
Config5.FRE), i.e. as the guest would see them, and they become unpredictable
if the guest FPU mode is changed. MIPS SIMD Architecture (MSA) vector
registers (see KVM_REG_MIPS_VEC_128() above) have similar patterns as they
overlap the FPU registers::

  0x7020 0000 0003 00 <0:3> <reg:5> (32-bit FPU registers)
  0x7030 0000 0003 00 <0:3> <reg:5> (64-bit FPU registers)
  0x7040 0000 0003 00 <0:3> <reg:5> (128-bit MSA vector registers)

(MIPS FPU 寄存器(见上文 KVM_REG_MIPS_FPR_{32,64}())根据所访问寄存器的大小而有以下 id 位模式。它们始终按当前客户机 FPU 模式 (Status.FR 和 Config5.FRE) 访问，即按客户机看到的方式访问；如果客户机 FPU 模式改变，它们将变得不可预测。MIPS SIMD 架构 (MSA) 向量寄存器(见上文 KVM_REG_MIPS_VEC_128())与 FPU 寄存器重叠，因此具有类似的模式：)

MIPS FPU control registers (see KVM_REG_MIPS_FCR_{IR,CSR} above) have the
following id bit patterns::

  0x7020 0000 0003 01 <0:3> <reg:5>

(MIPS FPU 控制寄存器(见上文 KVM_REG_MIPS_FCR_{IR,CSR})有以下 id 位模式：)

MIPS MSA control registers (see KVM_REG_MIPS_MSA_{IR,CSR} above) have the
following id bit patterns::

  0x7020 0000 0003 02 <0:3> <reg:5>

(MIPS MSA 控制寄存器(见上文 KVM_REG_MIPS_MSA_{IR,CSR})有以下 id 位模式：)

RISC-V registers are mapped using the lower 32 bits. The upper 8 bits of
that is the register group type.
(RISC-V 寄存器使用低 32 位进行映射。其中高 8 位是寄存器组类型。)

RISC-V config registers are meant for configuring a Guest VCPU and it has
the following id bit patterns::

  0x8020 0000 01 <index into the kvm_riscv_config struct:24> (32bit Host)
  0x8030 0000 01 <index into the kvm_riscv_config struct:24> (64bit Host)

(RISC-V config 寄存器用于配置客户机 VCPU，它有以下 id 位模式：)

Following are the RISC-V config registers:
(RISC-V config 寄存器如下：)

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x80x0 0000 0100 0000 isa       ISA feature bitmap of Guest VCPU
======================= ========= =============================================

The isa config register can be read anytime but can only be written before
a Guest VCPU runs. It will have ISA feature bits matching underlying host
set by default.
(isa config 寄存器可以随时读取，但只能在客户机 VCPU 运行之前写入。默认情况下，它会设置与底层宿主机相匹配的 ISA 特性位。)

RISC-V core registers represent the general execution state of a Guest VCPU
and it has the following id bit patterns::

  0x8020 0000 02 <index into the kvm_riscv_core struct:24> (32bit Host)
  0x8030 0000 02 <index into the kvm_riscv_core struct:24> (64bit Host)

(RISC-V core 寄存器表示客户机 VCPU 的通用执行状态，它有以下 id 位模式：)

Following are the RISC-V core registers:
(RISC-V core 寄存器如下：)

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x80x0 0000 0200 0000 regs.pc   Program counter
  0x80x0 0000 0200 0001 regs.ra   Return address
  0x80x0 0000 0200 0002 regs.sp   Stack pointer
  0x80x0 0000 0200 0003 regs.gp   Global pointer
  0x80x0 0000 0200 0004 regs.tp   Task pointer
  0x80x0 0000 0200 0005 regs.t0   Caller saved register 0
  0x80x0 0000 0200 0006 regs.t1   Caller saved register 1
  0x80x0 0000 0200 0007 regs.t2   Caller saved register 2
  0x80x0 0000 0200 0008 regs.s0   Callee saved register 0
  0x80x0 0000 0200 0009 regs.s1   Callee saved register 1
  0x80x0 0000 0200 000a regs.a0   Function argument (or return value) 0
  0x80x0 0000 0200 000b regs.a1   Function argument (or return value) 1
  0x80x0 0000 0200 000c regs.a2   Function argument 2
  0x80x0 0000 0200 000d regs.a3   Function argument 3
  0x80x0 0000 0200 000e regs.a4   Function argument 4
  0x80x0 0000 0200 000f regs.a5   Function argument 5
  0x80x0 0000 0200 0010 regs.a6   Function argument 6
  0x80x0 0000 0200 0011 regs.a7   Function argument 7
  0x80x0 0000 0200 0012 regs.s2   Callee saved register 2
  0x80x0 0000 0200 0013 regs.s3   Callee saved register 3
  0x80x0 0000 0200 0014 regs.s4   Callee saved register 4
  0x80x0 0000 0200 0015 regs.s5   Callee saved register 5
  0x80x0 0000 0200 0016 regs.s6   Callee saved register 6
  0x80x0 0000 0200 0017 regs.s7   Callee saved register 7
  0x80x0 0000 0200 0018 regs.s8   Callee saved register 8
  0x80x0 0000 0200 0019 regs.s9   Callee saved register 9
  0x80x0 0000 0200 001a regs.s10  Callee saved register 10
  0x80x0 0000 0200 001b regs.s11  Callee saved register 11
  0x80x0 0000 0200 001c regs.t3   Caller saved register 3
  0x80x0 0000 0200 001d regs.t4   Caller saved register 4
  0x80x0 0000 0200 001e regs.t5   Caller saved register 5
  0x80x0 0000 0200 001f regs.t6   Caller saved register 6
  0x80x0 0000 0200 0020 mode      Privilege mode (1 = S-mode or 0 = U-mode)
======================= ========= =============================================

RISC-V csr registers represent the supervisor mode control/status registers
of a Guest VCPU and it has the following id bit patterns::

  0x8020 0000 03 <index into the kvm_riscv_csr struct:24> (32bit Host)
  0x8030 0000 03 <index into the kvm_riscv_csr struct:24> (64bit Host)

Following are the RISC-V csr registers:

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x80x0 0000 0300 0000 sstatus   Supervisor status
  0x80x0 0000 0300 0001 sie       Supervisor interrupt enable
  0x80x0 0000 0300 0002 stvec     Supervisor trap vector base
  0x80x0 0000 0300 0003 sscratch  Supervisor scratch register
  0x80x0 0000 0300 0004 sepc      Supervisor exception program counter
  0x80x0 0000 0300 0005 scause    Supervisor trap cause
  0x80x0 0000 0300 0006 stval     Supervisor bad address or instruction
  0x80x0 0000 0300 0007 sip       Supervisor interrupt pending
  0x80x0 0000 0300 0008 satp      Supervisor address translation and protection
======================= ========= =============================================

RISC-V timer registers represent the timer state of a Guest VCPU and it has
the following id bit patterns::

  0x8030 0000 04 <index into the kvm_riscv_timer struct:24>

Following are the RISC-V timer registers:

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x8030 0000 0400 0000 frequency Time base frequency (read-only)
  0x8030 0000 0400 0001 time      Time value visible to Guest
  0x8030 0000 0400 0002 compare   Time compare programmed by Guest
  0x8030 0000 0400 0003 state     Time compare state (1 = ON or 0 = OFF)
======================= ========= =============================================

RISC-V F-extension registers represent the single precision floating point
state of a Guest VCPU and it has the following id bit patterns::

  0x8020 0000 05 <index into the __riscv_f_ext_state struct:24>

Following are the RISC-V F-extension registers:

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x8020 0000 0500 0000 f[0]      Floating point register 0
  ...
  0x8020 0000 0500 001f f[31]     Floating point register 31
  0x8020 0000 0500 0020 fcsr      Floating point control and status register
======================= ========= =============================================

RISC-V D-extension registers represent the double precision floating point
state of a Guest VCPU and it has the following id bit patterns::

  0x8020 0000 06 <index into the __riscv_d_ext_state struct:24> (fcsr)
  0x8030 0000 06 <index into the __riscv_d_ext_state struct:24> (non-fcsr)

Following are the RISC-V D-extension registers:

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x8030 0000 0600 0000 f[0]      Floating point register 0
  ...
  0x8030 0000 0600 001f f[31]     Floating point register 31
  0x8020 0000 0600 0020 fcsr      Floating point control and status register
======================= ========= =============================================

LoongArch registers are mapped using the lower 32 bits. The upper 16 bits of
that is the register group type.
(LoongArch 寄存器使用低 32 位进行映射。其中高 16 位是寄存器组类型。)

LoongArch csr registers are used to control guest cpu or get status of guest
cpu, and they have the following id bit patterns::

  0x9030 0000 0001 00 <reg:5> <sel:3>   (64-bit)

(LoongArch csr 寄存器用于控制客户机 cpu 或获取客户机 cpu 的状态，它们有以下 id 位模式：)

LoongArch KVM control registers are used to implement some new defined functions
such as set vcpu counter or reset vcpu, and they have the following id bit patterns::

  0x9030 0000 0002 <reg:16>

(LoongArch KVM 控制寄存器用于实现一些新定义的功能，例如设置 vcpu 计数器或重置 vcpu，它们有以下 id 位模式：)

x86 MSR registers have the following id bit patterns::
  0x2030 0002 <msr number:32>

(x86 MSR 寄存器有以下 id 位模式：)

Following are the KVM-defined registers for x86:
(KVM 为 x86 定义的寄存器如下：)

======================= ========= =============================================
    Encoding            Register  Description
======================= ========= =============================================
  0x2030 0003 0000 0000 SSP       Shadow Stack Pointer
======================= ========= =============================================

4.69 KVM_GET_ONE_REG(4.69 KVM_GET_ONE_REG)
------------------------------------------

:Capability: KVM_CAP_ONE_REG
  (能力：KVM_CAP_ONE_REG)
:Architectures: all
  (架构：全部)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_one_reg (in and out)
  (参数：struct kvm_one_reg (in and out))
:Returns: 0 on success, negative value on failure
  (返回值：成功时为 0，失败时为负值)

Errors include:
(可能的错误包括：)

  ======== ============================================================
  ENOENT   no such register
           (不存在这样的寄存器)
  EINVAL   invalid register ID, or no such register or used with VMs in
           protected virtualization mode on s390
           (寄存器 ID 无效，或不存在这样的寄存器，或在 s390 上用于受保护虚拟化模式的 VM)
  EPERM    (arm64) register access not allowed before vcpu finalization
           ((arm64) 在 vcpu 最终确定之前不允许访问寄存器)
  ======== ============================================================

(These error codes are indicative only: do not rely on a specific error
code being returned in a specific situation.)
(这些错误码仅供参考：不要依赖在特定情况下一定返回某个特定错误码。)

This ioctl allows to receive the value of a single register implemented
in a vcpu. The register to read is indicated by the "id" field of the
kvm_one_reg struct passed in. On success, the register value can be found
at the memory location pointed to by "addr".
(此 ioctl 用于获取 vcpu 中实现的单个寄存器的值。要读取的寄存器由传入的 kvm_one_reg 结构体的 "id" 字段指明。成功时，可以在 "addr" 所指向的内存位置找到寄存器值。)

The list of registers accessible using this interface is identical to the
list in 4.68.
(可通过此接口访问的寄存器列表与 4.68 节中的列表相同。)


4.70 KVM_KVMCLOCK_CTRL(4.70 KVM_KVMCLOCK_CTRL)
----------------------------------------------

:Capability: KVM_CAP_KVMCLOCK_CTRL
  (能力：KVM_CAP_KVMCLOCK_CTRL)
:Architectures: Any that implement pvclocks (currently x86 only)
  (架构：任何实现了 pvclocks 的架构(目前仅 x86))
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: None
  (参数：无)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

This ioctl sets a flag accessible to the guest indicating that the specified
vCPU has been paused by the host userspace.
(此 ioctl 设置一个对客户机可见的标志，表明指定的 vCPU 已被宿主用户态暂停。)

The host will set a flag in the pvclock structure that is checked from the
soft lockup watchdog.  The flag is part of the pvclock structure that is
shared between guest and host, specifically the second bit of the flags
field of the pvclock_vcpu_time_info structure.  It will be set exclusively by
the host and read/cleared exclusively by the guest.  The guest operation of
checking and clearing the flag must be an atomic operation so
load-link/store-conditional, or equivalent must be used.  There are two cases
where the guest will clear the flag: when the soft lockup watchdog timer resets
itself or when a soft lockup is detected.  This ioctl can be called any time
after pausing the vcpu, but before it is resumed.
(宿主机会在 pvclock 结构中设置一个由软锁定 (soft lockup) watchdog 检查的标志。该标志是客户机与宿主机共享的 pvclock 结构的一部分，具体是 pvclock_vcpu_time_info 结构 flags 字段的第二位。它只由宿主机设置，只由客户机读取/清除。客户机检查并清除该标志的操作必须是原子操作，因此必须使用 load-link/store-conditional 或等效机制。客户机会在两种情况下清除该标志：软锁定 watchdog 定时器自行复位时，或检测到软锁定时。此 ioctl 可以在暂停 vcpu 之后、恢复 vcpu 之前的任何时刻调用。)


4.71 KVM_SIGNAL_MSI(4.71 KVM_SIGNAL_MSI)
----------------------------------------

:Capability: KVM_CAP_SIGNAL_MSI
  (能力：KVM_CAP_SIGNAL_MSI)
:Architectures: x86 arm64
  (架构：x86、arm64)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_msi (in)
  (参数：struct kvm_msi (in))
:Returns: >0 on delivery, 0 if guest blocked the MSI, and -1 on error
  (返回值：投递时为 >0，客户机阻塞该 MSI 时为 0，出错时为 -1)

Directly inject a MSI message. Only valid with in-kernel irqchip that handles
MSI messages.
(直接注入一个 MSI 消息。只在能处理 MSI 消息的内核内 irqchip 下有效。)

::

  struct kvm_msi {
	__u32 address_lo;
	__u32 address_hi;
	__u32 data;
	__u32 flags;
	__u32 devid;
	__u8  pad[12];
  };

flags:
(标志：)
  KVM_MSI_VALID_DEVID: devid contains a valid value.  The per-VM
  KVM_CAP_MSI_DEVID capability advertises the requirement to provide
  the device ID.  If this capability is not available, userspace
  should never set the KVM_MSI_VALID_DEVID flag as the ioctl might fail.
  (KVM_MSI_VALID_DEVID：devid 包含有效值。每 VM 的 KVM_CAP_MSI_DEVID 能力通告了提供设备 ID 的要求。如果该能力不可用，用户态绝不应设置 KVM_MSI_VALID_DEVID 标志，否则 ioctl 可能失败。)

If KVM_MSI_VALID_DEVID is set, devid contains a unique device identifier
for the device that wrote the MSI message.  For PCI, this is usually a
BDF identifier in the lower 16 bits.
(如果设置了 KVM_MSI_VALID_DEVID，devid 包含写入该 MSI 消息的设备的唯一设备标识符。对 PCI 来说，这通常是低 16 位中的 BDF 标识符。)

On x86, address_hi is ignored unless the KVM_X2APIC_API_USE_32BIT_IDS
feature of KVM_CAP_X2APIC_API capability is enabled.  If it is enabled,
address_hi bits 31-8 provide bits 31-8 of the destination id.  Bits 7-0 of
address_hi must be zero.
(在 x86 上，除非启用了 KVM_CAP_X2APIC_API 能力的 KVM_X2APIC_API_USE_32BIT_IDS 特性，否则 address_hi 会被忽略。如果已启用，address_hi 的 bits 31-8 提供目标 id 的 bits 31-8。address_hi 的 bits 7-0 必须为 0。)


4.71 KVM_CREATE_PIT2(4.71 KVM_CREATE_PIT2)
------------------------------------------

:Capability: KVM_CAP_PIT2
  (能力：KVM_CAP_PIT2)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_pit_config (in)
  (参数：struct kvm_pit_config (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Creates an in-kernel device model for the i8254 PIT. This call is only valid
after enabling in-kernel irqchip support via KVM_CREATE_IRQCHIP. The following
parameters have to be passed::

  struct kvm_pit_config {
	__u32 flags;
	__u32 pad[15];
  };

(为 i8254 PIT 创建内核内设备模型。此调用只有在通过 KVM_CREATE_IRQCHIP 启用内核内 irqchip 支持之后才有效。必须传入以下参数：)

Valid flags are::

  #define KVM_PIT_SPEAKER_DUMMY     1 /* emulate speaker port stub */

(有效标志为：)

PIT timer interrupts may use a per-VM kernel thread for injection. If it
exists, this thread will have a name of the following pattern::

  kvm-pit/<owner-process-pid>

(PIT 定时器中断可能使用一个每 VM 内核线程进行注入。如果该线程存在，其名称将遵循以下模式：)

When running a guest with elevated priorities, the scheduling parameters of
this thread may have to be adjusted accordingly.
(在以提升的优先级运行客户机时，可能需要相应调整该线程的调度参数。)

This IOCTL replaces the obsolete KVM_CREATE_PIT.
(此 IOCTL 取代已废弃的 KVM_CREATE_PIT。)


4.72 KVM_GET_PIT2(4.72 KVM_GET_PIT2)
------------------------------------

:Capability: KVM_CAP_PIT_STATE2
  (能力：KVM_CAP_PIT_STATE2)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_pit_state2 (out)
  (参数：struct kvm_pit_state2 (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Retrieves the state of the in-kernel PIT model. Only valid after
KVM_CREATE_PIT2. The state is returned in the following structure::

  struct kvm_pit_state2 {
	struct kvm_pit_channel_state channels[3];
	__u32 flags;
	__u32 reserved[9];
  };

(获取内核内 PIT 模型的状态。只在 KVM_CREATE_PIT2 之后有效。状态通过以下结构返回：)

Valid flags are::

  /* disable PIT in HPET legacy mode */
  #define KVM_PIT_FLAGS_HPET_LEGACY     0x00000001
  /* speaker port data bit enabled */
  #define KVM_PIT_FLAGS_SPEAKER_DATA_ON 0x00000002

(有效标志为：)

This IOCTL replaces the obsolete KVM_GET_PIT.
(此 IOCTL 取代已废弃的 KVM_GET_PIT。)


4.73 KVM_SET_PIT2(4.73 KVM_SET_PIT2)
------------------------------------

:Capability: KVM_CAP_PIT_STATE2
  (能力：KVM_CAP_PIT_STATE2)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_pit_state2 (in)
  (参数：struct kvm_pit_state2 (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Sets the state of the in-kernel PIT model. Only valid after KVM_CREATE_PIT2.
See KVM_GET_PIT2 for details on struct kvm_pit_state2.
(设置内核内 PIT 模型的状态。只在 KVM_CREATE_PIT2 之后有效。struct kvm_pit_state2 的细节参见 KVM_GET_PIT2。)

.. Tip::
  ``KVM_SET_PIT2`` strictly adheres to the spec of Intel 8254 PIT.  For example,
  a ``count`` value of 0 in ``struct kvm_pit_channel_state`` is interpreted as
  65536, which is the maximum count value. Refer to `Intel 8254 programmable
  interval timer <https://www.scs.stanford.edu/10wi-cs140/pintos/specs/8254.pdf>`_.
  (提示：``KVM_SET_PIT2`` 严格遵守 Intel 8254 PIT 的规范。例如，``struct kvm_pit_channel_state`` 中为 0 的 ``count`` 值会被解释为 65536，即最大计数值。参见 Intel 8254 可编程间隔定时器规范 (https://www.scs.stanford.edu/10wi-cs140/pintos/specs/8254.pdf)。)

This IOCTL replaces the obsolete KVM_SET_PIT.
(此 IOCTL 取代已废弃的 KVM_SET_PIT。)


4.74 KVM_PPC_GET_SMMU_INFO(4.74 KVM_PPC_GET_SMMU_INFO)
------------------------------------------------------

:Capability: KVM_CAP_PPC_GET_SMMU_INFO
  (能力：KVM_CAP_PPC_GET_SMMU_INFO)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: None
  (参数：无)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

This populates and returns a structure describing the features of
the "Server" class MMU emulation supported by KVM.
This can in turn be used by userspace to generate the appropriate
device-tree properties for the guest operating system.
(它填充并返回一个结构，描述 KVM 支持的 "Server" 类 MMU 模拟的特性。用户态可以进而用它为客户机操作系统生成适当的设备树属性。)

The structure contains some global information, followed by an
array of supported segment page sizes::

      struct kvm_ppc_smmu_info {
	     __u64 flags;
	     __u32 slb_size;
	     __u32 pad;
	     struct kvm_ppc_one_seg_page_size sps[KVM_PPC_PAGE_SIZES_MAX_SZ];
      };

(该结构包含一些全局信息，后跟一个受支持的段页大小数组：)

The supported flags are:
(支持的标志有：)

    - KVM_PPC_PAGE_SIZES_REAL:
        When that flag is set, guest page sizes must "fit" the backing
        store page sizes. When not set, any page size in the list can
        be used regardless of how they are backed by userspace.
        (设置该标志时，客户机页大小必须与后备存储页大小"契合"。未设置时，列表中的任何页大小都可以使用，无论用户态如何为其提供后备。)

    - KVM_PPC_1T_SEGMENTS
        The emulated MMU supports 1T segments in addition to the
        standard 256M ones.
        (模拟的 MMU 除标准 256M 段外还支持 1T 段。)

    - KVM_PPC_NO_HASH
	This flag indicates that HPT guests are not supported by KVM,
	thus all guests must use radix MMU mode.
	(该标志表示 KVM 不支持 HPT 客户机，因此所有客户机都必须使用 radix MMU 模式。)

The "slb_size" field indicates how many SLB entries are supported
("slb_size" 字段指示支持多少个 SLB 条目)

The "sps" array contains 8 entries indicating the supported base
page sizes for a segment in increasing order. Each entry is defined
as follow::

   struct kvm_ppc_one_seg_page_size {
	__u32 page_shift;	/* Base page shift of segment (or 0) */
	__u32 slb_enc;		/* SLB encoding for BookS */
	struct kvm_ppc_one_page_size enc[KVM_PPC_PAGE_SIZES_MAX_SZ];
   };

("sps" 数组包含 8 个条目，按递增顺序给出一个段支持的各基本页大小。每个条目定义如下：)

An entry with a "page_shift" of 0 is unused. Because the array is
organized in increasing order, a lookup can stop when encountering
such an entry.
("page_shift" 为 0 的条目未被使用。由于数组按递增顺序组织，遇到这样的条目时查找即可停止。)

The "slb_enc" field provides the encoding to use in the SLB for the
page size. The bits are in positions such as the value can directly
be OR'ed into the "vsid" argument of the slbmte instruction.
("slb_enc" 字段提供要在 SLB 中为该页大小使用的编码。这些位的位置安排使得该值可以直接 OR 进 slbmte 指令的 "vsid" 参数。)

The "enc" array is a list which for each of those segment base page
size provides the list of supported actual page sizes (which can be
only larger or equal to the base page size), along with the
corresponding encoding in the hash PTE. Similarly, the array is
8 entries sorted by increasing sizes and an entry with a "0" shift
is an empty entry and a terminator::

   struct kvm_ppc_one_page_size {
	__u32 page_shift;	/* Page shift (or 0) */
	__u32 pte_enc;		/* Encoding in the HPTE (>>12) */
   };

("enc" 数组是一个列表，为每个这样的段基本页大小给出受支持的实际页大小列表(只能大于或等于基本页大小)，以及 hash PTE 中的相应编码。类似地，该数组为 8 个条目并按大小递增排序，"0" shift 的条目是空条目，也是终止符：)

The "pte_enc" field provides a value that can OR'ed into the hash
PTE's RPN field (ie, it needs to be shifted left by 12 to OR it
into the hash PTE second double word).
("pte_enc" 字段提供一个可以 OR 进 hash PTE 的 RPN 字段的值(即需要左移 12 位后再 OR 进 hash PTE 的第二个双字)。)

4.75 KVM_IRQFD(4.75 KVM_IRQFD)
------------------------------

:Capability: KVM_CAP_IRQFD
  (能力：KVM_CAP_IRQFD)
:Architectures: x86 s390 arm64
  (架构：x86、s390、arm64)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_irqfd (in)
  (参数：struct kvm_irqfd (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Allows setting an eventfd to directly trigger a guest interrupt.
kvm_irqfd.fd specifies the file descriptor to use as the eventfd and
kvm_irqfd.gsi specifies the irqchip pin toggled by this event.  When
an event is triggered on the eventfd, an interrupt is injected into
the guest using the specified gsi pin.  The irqfd is removed using
the KVM_IRQFD_FLAG_DEASSIGN flag, specifying both kvm_irqfd.fd
and kvm_irqfd.gsi.
(允许设置一个 eventfd 来直接触发客户机中断。kvm_irqfd.fd 指定用作 eventfd 的文件描述符，kvm_irqfd.gsi 指定由该事件翻转的 irqchip 引脚。当 eventfd 上触发事件时，会使用指定的 gsi 引脚向客户机注入一个中断。irqfd 用 KVM_IRQFD_FLAG_DEASSIGN 标志移除，同时指定 kvm_irqfd.fd 和 kvm_irqfd.gsi。)

With KVM_CAP_IRQFD_RESAMPLE, KVM_IRQFD supports a de-assert and notify
mechanism allowing emulation of level-triggered, irqfd-based
interrupts.  When KVM_IRQFD_FLAG_RESAMPLE is set the user must pass an
additional eventfd in the kvm_irqfd.resamplefd field.  When operating
in resample mode, posting of an interrupt through kvm_irq.fd asserts
the specified gsi in the irqchip.  When the irqchip is resampled, such
as from an EOI, the gsi is de-asserted and the user is notified via
kvm_irqfd.resamplefd.  It is the user's responsibility to re-queue
the interrupt if the device making use of it still requires service.
Note that closing the resamplefd is not sufficient to disable the
irqfd.  The KVM_IRQFD_FLAG_RESAMPLE is only necessary on assignment
and need not be specified with KVM_IRQFD_FLAG_DEASSIGN.
(有了 KVM_CAP_IRQFD_RESAMPLE，KVM_IRQFD 支持去断言并通知 (de-assert and notify) 机制，可以模拟基于 irqfd 的电平触发中断。设置 KVM_IRQFD_FLAG_RESAMPLE 时，用户必须在 kvm_irqfd.resamplefd 字段中传入一个额外的 eventfd。在 resample 模式下工作时，通过 kvm_irq.fd 投递中断会断言 irqchip 中指定的 gsi。当 irqchip 被重新采样(例如来自 EOI)时，gsi 被去断言，并通过 kvm_irqfd.resamplefd 通知用户。如果使用该中断的设备仍需要服务，重新排队该中断是用户的责任。注意，关闭 resamplefd 并不足以禁用 irqfd。KVM_IRQFD_FLAG_RESAMPLE 只在分配时需要，与 KVM_IRQFD_FLAG_DEASSIGN 一起使用时无需再指定。)

On arm64, gsi routing being supported, the following can happen:
(在 arm64 上，由于支持 gsi 路由，可能发生以下情况：)

- in case no routing entry is associated to this gsi, injection fails
  (如果没有与该 gsi 关联的路由条目，注入失败)
- in case the gsi is associated to an irqchip routing entry,
  irqchip.pin + 32 corresponds to the injected SPI ID.
  (如果该 gsi 关联到 irqchip 路由条目，irqchip.pin + 32 对应注入的 SPI ID。)
- in case the gsi is associated to an MSI routing entry, the MSI
  message and device ID are translated into an LPI (support restricted
  to GICv3 ITS in-kernel emulation).
  (如果该 gsi 关联到 MSI 路由条目，MSI 消息和设备 ID 会被转换为一个 LPI(仅支持 GICv3 ITS 的内核内模拟)。)

4.76 KVM_PPC_ALLOCATE_HTAB(4.76 KVM_PPC_ALLOCATE_HTAB)
------------------------------------------------------

:Capability: KVM_CAP_PPC_ALLOC_HTAB
  (能力：KVM_CAP_PPC_ALLOC_HTAB)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: Pointer to u32 containing hash table order (in/out)
  (参数：指向包含哈希表阶数 (order) 的 u32 的指针 (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

This requests the host kernel to allocate an MMU hash table for a
guest using the PAPR paravirtualization interface.  This only does
anything if the kernel is configured to use the Book 3S HV style of
virtualization.  Otherwise the capability doesn't exist and the ioctl
returns an ENOTTY error.  The rest of this description assumes Book 3S
HV.
(这请求宿主内核使用 PAPR 半虚拟化接口为客户机分配一个 MMU 哈希表。只有当内核配置为使用 Book 3S HV 风格的虚拟化时它才起作用。否则该能力不存在，ioctl 返回 ENOTTY 错误。本节其余描述均假定使用 Book 3S HV。)

There must be no vcpus running when this ioctl is called; if there
are, it will do nothing and return an EBUSY error.
(调用此 ioctl 时不得有任何 vcpu 正在运行；如果有，它将什么也不做并返回 EBUSY 错误。)

The parameter is a pointer to a 32-bit unsigned integer variable
containing the order (log base 2) of the desired size of the hash
table, which must be between 18 and 46.  On successful return from the
ioctl, the value will not be changed by the kernel.
(参数是一个指向 32 位无符号整数变量的指针，该变量包含所需哈希表大小的阶 (order，以 2 为底的对数)，必须在 18 到 46 之间。从 ioctl 成功返回后，内核不会改变该值。)

If no hash table has been allocated when any vcpu is asked to run
(with the KVM_RUN ioctl), the host kernel will allocate a
default-sized hash table (16 MB).
(如果在请求运行任何 vcpu 时(通过 KVM_RUN ioctl)尚未分配哈希表，宿主内核会分配一个默认大小的哈希表 (16 MB)。)

If this ioctl is called when a hash table has already been allocated,
with a different order from the existing hash table, the existing hash
table will be freed and a new one allocated.  If this is ioctl is
called when a hash table has already been allocated of the same order
as specified, the kernel will clear out the existing hash table (zero
all HPTEs).  In either case, if the guest is using the virtualized
real-mode area (VRMA) facility, the kernel will re-create the VMRA
HPTEs on the next KVM_RUN of any vcpu.
(如果已分配了哈希表而此 ioctl 以不同于现有哈希表的阶被调用，现有哈希表会被释放并分配一个新的。如果已分配的哈希表的阶与指定的相同而调用此 ioctl，内核会清空现有哈希表(把所有 HPTE 置 0)。无论哪种情况，如果客户机正在使用虚拟化 real-mode 区域 (VRMA) 设施，内核会在任何 vcpu 的下一次 KVM_RUN 时重建 VMRA HPTE。)

4.77 KVM_S390_INTERRUPT(4.77 KVM_S390_INTERRUPT)
------------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: s390
  (架构：s390)
:Type: vm ioctl, vcpu ioctl
  (类型：vm ioctl、vcpu ioctl)
:Parameters: struct kvm_s390_interrupt (in)
  (参数：struct kvm_s390_interrupt (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Allows to inject an interrupt to the guest. Interrupts can be floating
(vm ioctl) or per cpu (vcpu ioctl), depending on the interrupt type.
(允许向客户机注入一个中断。根据中断类型的不同，中断可以是浮动的 (floating，vm ioctl) 或每 cpu 的 (vcpu ioctl)。)

Interrupt parameters are passed via kvm_s390_interrupt::

  struct kvm_s390_interrupt {
	__u32 type;
	__u32 parm;
	__u64 parm64;
  };

(中断参数通过 kvm_s390_interrupt 传递：)

type can be one of the following:
(type 可以是以下之一：)

KVM_S390_SIGP_STOP (vcpu)
    - sigp stop; optional flags in parm
    (sigp stop；parm 中为可选标志)
KVM_S390_PROGRAM_INT (vcpu)
    - program check; code in parm
    (程序检查；代码在 parm 中)
KVM_S390_SIGP_SET_PREFIX (vcpu)
    - sigp set prefix; prefix address in parm
    (sigp set prefix；前缀地址在 parm 中)
KVM_S390_RESTART (vcpu)
    - restart
    (重启)
KVM_S390_INT_CLOCK_COMP (vcpu)
    - clock comparator interrupt
    (时钟比较器中断)
KVM_S390_INT_CPU_TIMER (vcpu)
    - CPU timer interrupt
    (CPU 定时器中断)
KVM_S390_INT_VIRTIO (vm)
    - virtio external interrupt; external interrupt
      parameters in parm and parm64
    (virtio 外部中断；外部中断参数在 parm 和 parm64 中)
KVM_S390_INT_SERVICE (vm)
    - sclp external interrupt; sclp parameter in parm
    (sclp 外部中断；sclp 参数在 parm 中)
KVM_S390_INT_EMERGENCY (vcpu)
    - sigp emergency; source cpu in parm
    (sigp emergency；源 cpu 在 parm 中)
KVM_S390_INT_EXTERNAL_CALL (vcpu)
    - sigp external call; source cpu in parm
    (sigp external call；源 cpu 在 parm 中)
KVM_S390_INT_IO(ai,cssid,ssid,schid) (vm)
    - compound value to indicate an
      I/O interrupt (ai - adapter interrupt; cssid,ssid,schid - subchannel);
      I/O interruption parameters in parm (subchannel) and parm64 (intparm,
      interruption subclass)
    (指示一次 I/O 中断的复合值(ai - 适配器中断；cssid,ssid,schid - 子通道)；I/O 中断参数在 parm(子通道)和 parm64(intparm、中断子类)中)
KVM_S390_MCHK (vm, vcpu)
    - machine check interrupt; cr 14 bits in parm, machine check interrupt
      code in parm64 (note that machine checks needing further payload are not
      supported by this ioctl)
    (机器检查中断；cr 14 位在 parm 中，机器检查中断代码在 parm64 中(注意，需要额外载荷的机器检查不受此 ioctl 支持))

This is an asynchronous vcpu ioctl and can be invoked from any thread.
(这是异步 vcpu ioctl，可以从任意线程调用。)

4.78 KVM_PPC_GET_HTAB_FD(4.78 KVM_PPC_GET_HTAB_FD)
--------------------------------------------------

:Capability: KVM_CAP_PPC_HTAB_FD
  (能力：KVM_CAP_PPC_HTAB_FD)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: Pointer to struct kvm_get_htab_fd (in)
  (参数：指向 struct kvm_get_htab_fd 的指针 (in))
:Returns: file descriptor number (>= 0) on success, -1 on error
  (返回值：成功时为文件描述符编号 (>= 0)，出错时为 -1)

This returns a file descriptor that can be used either to read out the
entries in the guest's hashed page table (HPT), or to write entries to
initialize the HPT.  The returned fd can only be written to if the
KVM_GET_HTAB_WRITE bit is set in the flags field of the argument, and
can only be read if that bit is clear.  The argument struct looks like
this::

  /* For KVM_PPC_GET_HTAB_FD */
  struct kvm_get_htab_fd {
	__u64	flags;
	__u64	start_index;
	__u64	reserved[2];
  };

  /* Values for kvm_get_htab_fd.flags */
  #define KVM_GET_HTAB_BOLTED_ONLY	((__u64)0x1)
  #define KVM_GET_HTAB_WRITE		((__u64)0x2)

(这返回一个文件描述符，既可用于读出客户机哈希页表 (HPT) 中的条目，也可用于写入条目以初始化 HPT。只有在参数的 flags 字段中设置了 KVM_GET_HTAB_WRITE 位时，返回的 fd 才能被写入；只有该位为 0 时才能被读取。参数结构如下：)

The 'start_index' field gives the index in the HPT of the entry at
which to start reading.  It is ignored when writing.
('start_index' 字段给出 HPT 中开始读取的条目索引。写入时它被忽略。)

Reads on the fd will initially supply information about all
"interesting" HPT entries.  Interesting entries are those with the
bolted bit set, if the KVM_GET_HTAB_BOLTED_ONLY bit is set, otherwise
all entries.  When the end of the HPT is reached, the read() will
return.  If read() is called again on the fd, it will start again from
the beginning of the HPT, but will only return HPT entries that have
changed since they were last read.
(对 fd 的读取最初会提供所有"感兴趣的"HPT 条目的信息。如果设置了 KVM_GET_HTAB_BOLTED_ONLY 位，感兴趣的条目是设置了 bolted 位的那些条目；否则为全部条目。到达 HPT 末尾时 read() 会返回。如果在 fd 上再次调用 read()，它会从头开始，但只返回自上次读取以来发生变化的 HPT 条目。)

Data read or written is structured as a header (8 bytes) followed by a
series of valid HPT entries (16 bytes) each.  The header indicates how
many valid HPT entries there are and how many invalid entries follow
the valid entries.  The invalid entries are not represented explicitly
in the stream.  The header format is::

  struct kvm_get_htab_header {
	__u32	index;
	__u16	n_valid;
	__u16	n_invalid;
  };

(读取或写入的数据结构为：一个头部 (8 字节)，后跟一系列各 16 字节的有效 HPT 条目。头部指出有多少个有效 HPT 条目，以及有效条目之后跟随多少个无效条目。无效条目不会在流中显式表示。头部格式如下：)

Writes to the fd create HPT entries starting at the index given in the
header; first 'n_valid' valid entries with contents from the data
written, then 'n_invalid' invalid entries, invalidating any previously
valid entries found.
(对 fd 的写入会从头部给定的索引开始创建 HPT 条目：先用写入的数据创建 'n_valid' 个有效条目，然后是 'n_invalid' 个无效条目，使之前发现的有效条目失效。)

4.79 KVM_CREATE_DEVICE(4.79 KVM_CREATE_DEVICE)
----------------------------------------------

:Capability: KVM_CAP_DEVICE_CTRL
  (能力：KVM_CAP_DEVICE_CTRL)
:Architectures: all
  (架构：全部)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_create_device (in/out)
  (参数：struct kvm_create_device (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Errors:
(错误：)

  ======  =======================================================
  ENODEV  The device type is unknown or unsupported
          (设备类型未知或不受支持)
  EEXIST  Device already created, and this type of device may not
          be instantiated multiple times
          (设备已创建，且该类型的设备不允许被多次实例化)
  ======  =======================================================

  Other error conditions may be defined by individual device types or
  have their standard meanings.
  (其他错误情况可由个别设备类型定义，或具有其标准含义。)

Creates an emulated device in the kernel.  The file descriptor returned
in fd can be used with KVM_SET/GET/HAS_DEVICE_ATTR.
(在内核中创建一个模拟设备。fd 中返回的文件描述符可与 KVM_SET/GET/HAS_DEVICE_ATTR 一起使用。)

If the KVM_CREATE_DEVICE_TEST flag is set, only test whether the
device type is supported (not necessarily whether it can be created
in the current vm).
(如果设置了 KVM_CREATE_DEVICE_TEST 标志，则只测试设备类型是否受支持(不一定是它能否在当前 vm 中创建)。)

Individual devices should not define flags.  Attributes should be used
for specifying any behavior that is not implied by the device type
number.
(个别设备不应定义标志。设备类型编号未隐含的任何行为都应改用属性 (attribute) 来指定。)

::

  struct kvm_create_device {
	__u32	type;	/* in: KVM_DEV_TYPE_xxx */
	__u32	fd;	/* out: device handle */
	__u32	flags;	/* in: KVM_CREATE_DEVICE_xxx */
  };

4.80 KVM_SET_DEVICE_ATTR/KVM_GET_DEVICE_ATTR(4.80 KVM_SET_DEVICE_ATTR/KVM_GET_DEVICE_ATTR)
------------------------------------------------------------------------------------------

:Capability: KVM_CAP_DEVICE_CTRL, KVM_CAP_VM_ATTRIBUTES for vm device,
             KVM_CAP_VCPU_ATTRIBUTES for vcpu device
             KVM_CAP_SYS_ATTRIBUTES for system (/dev/kvm) device (no set)
  (能力：KVM_CAP_DEVICE_CTRL；vm 设备需要 KVM_CAP_VM_ATTRIBUTES；vcpu 设备需要 KVM_CAP_VCPU_ATTRIBUTES；system (/dev/kvm) 设备需要 KVM_CAP_SYS_ATTRIBUTES(无 set))
:Architectures: x86, arm64, s390
  (架构：x86、arm64、s390)
:Type: device ioctl, vm ioctl, vcpu ioctl
  (类型：device ioctl、vm ioctl、vcpu ioctl)
:Parameters: struct kvm_device_attr
  (参数：struct kvm_device_attr)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Errors:
(错误：)

  =====   =============================================================
  ENXIO   The group or attribute is unknown/unsupported for this device
          or hardware support is missing.
          (该设备不认识/不支持该组或属性，或缺少硬件支持。)
  EPERM   The attribute cannot (currently) be accessed this way
          (e.g. read-only attribute, or attribute that only makes
          sense when the device is in a different state)
          (该属性(目前)不能以这种方式访问(例如只读属性，或只在设备处于不同状态时才有意义的属性))
  =====   =============================================================

  Other error conditions may be defined by individual device types.
  (其他错误情况可由个别设备类型定义。)

Gets/sets a specified piece of device configuration and/or state.  The
semantics are device-specific.  See individual device documentation in
the "devices" directory.  As with ONE_REG, the size of the data
transferred is defined by the particular attribute.
(获取/设置设备配置和/或状态的指定部分。语义由具体设备决定。参见 "devices" 目录中各设备的文档。与 ONE_REG 一样，传输数据的大小由具体属性定义。)

::

  struct kvm_device_attr {
	__u32	flags;		/* no flags currently defined */
	__u32	group;		/* device-defined */
	__u64	attr;		/* group-defined */
	__u64	addr;		/* userspace address of attr data */
  };

4.81 KVM_HAS_DEVICE_ATTR(4.81 KVM_HAS_DEVICE_ATTR)
--------------------------------------------------

:Capability: KVM_CAP_DEVICE_CTRL, KVM_CAP_VM_ATTRIBUTES for vm device,
             KVM_CAP_VCPU_ATTRIBUTES for vcpu device
             KVM_CAP_SYS_ATTRIBUTES for system (/dev/kvm) device
  (能力：KVM_CAP_DEVICE_CTRL；vm 设备需要 KVM_CAP_VM_ATTRIBUTES；vcpu 设备需要 KVM_CAP_VCPU_ATTRIBUTES；system (/dev/kvm) 设备需要 KVM_CAP_SYS_ATTRIBUTES)
:Type: device ioctl, vm ioctl, vcpu ioctl
  (类型：device ioctl、vm ioctl、vcpu ioctl)
:Parameters: struct kvm_device_attr
  (参数：struct kvm_device_attr)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Errors:
(错误：)

  =====   =============================================================
  ENXIO   The group or attribute is unknown/unsupported for this device
          or hardware support is missing.
          (该设备不认识/不支持该组或属性，或缺少硬件支持。)
  =====   =============================================================

Tests whether a device supports a particular attribute.  A successful
return indicates the attribute is implemented.  It does not necessarily
indicate that the attribute can be read or written in the device's
current state.  "addr" is ignored.
(测试设备是否支持某个特定属性。成功返回表明该属性已实现。它不一定表明在该设备当前状态下该属性可读或可写。"addr" 被忽略。)

.. _KVM_ARM_VCPU_INIT:

4.82 KVM_ARM_VCPU_INIT(4.82 KVM_ARM_VCPU_INIT)
----------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: arm64
  (架构：arm64)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_vcpu_init (in)
  (参数：struct kvm_vcpu_init (in))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

Errors:
(错误：)

  ======     =================================================================
  EINVAL     the target is unknown, or the combination of features is invalid.
             (target 未知，或特性组合无效。)
  ENOENT     a features bit specified is unknown.
             (指定的某个特性位未知。)
  ======     =================================================================

This tells KVM what type of CPU to present to the guest, and what
optional features it should have.  This will cause a reset of the cpu
registers to their initial values.  If this is not called, KVM_RUN will
return ENOEXEC for that vcpu.
(此调用告诉 KVM 要向客户机呈现哪种类型的 CPU，以及它应具备哪些可选特性。这会使 cpu 寄存器复位到其初始值。如果不调用它，KVM_RUN 会为该 vcpu 返回 ENOEXEC。)

The initial values are defined as:
(初始值定义如下：)
	- Processor state:
	  (处理器状态：)
		* AArch64: EL1h, D, A, I and F bits set. All other bits
		  are cleared.
		  (AArch64：置位 EL1h、D、A、I 与 F 位，所有其他位清零。)
		* AArch32: SVC, A, I and F bits set. All other bits are
		  cleared.
		  (AArch32：置位 SVC、A、I 与 F 位，所有其他位清零。)
	- General Purpose registers, including PC and SP: set to 0
	  (通用寄存器(包括 PC 和 SP)：置 0)
	- FPSIMD/NEON registers: set to 0
	  (FPSIMD/NEON 寄存器：置 0)
	- SVE registers: set to 0
	  (SVE 寄存器：置 0)
	- System registers: Reset to their architecturally defined
	  values as for a warm reset to EL1 (resp. SVC) or EL2 (in the
	  case of EL2 being enabled).
	  (系统寄存器：复位到其架构定义的值，如同热复位到 EL1(或 SVC)或 EL2(在启用 EL2 的情况下)。)

Note that because some registers reflect machine topology, all vcpus
should be created before this ioctl is invoked.
(注意，由于某些寄存器反映机器拓扑，应在调用此 ioctl 之前创建所有 vcpu。)

Userspace can call this function multiple times for a given vcpu, including
after the vcpu has been run. This will reset the vcpu to its initial
state. All calls to this function after the initial call must use the same
target and same set of feature flags, otherwise EINVAL will be returned.
(对给定的 vcpu，用户态可以多次调用此函数，包括在 vcpu 已经运行之后。这会把 vcpu 复位到其初始状态。首次调用之后的每次调用都必须使用相同的 target 和相同的特性标志集合，否则会返回 EINVAL。)

Possible features:
(可能的特性：)

	- KVM_ARM_VCPU_POWER_OFF: Starts the CPU in a power-off state.
	  Depends on KVM_CAP_ARM_PSCI.  If not set, the CPU will be powered on
	  and execute guest code when KVM_RUN is called.
	  (使 CPU 以断电状态启动。依赖 KVM_CAP_ARM_PSCI。如果未设置，调用 KVM_RUN 时 CPU 会通电并执行客户机代码。)
	- KVM_ARM_VCPU_EL1_32BIT: Starts the CPU in a 32bit mode.
	  Depends on KVM_CAP_ARM_EL1_32BIT (arm64 only).
	  (使 CPU 以 32 位模式启动。依赖 KVM_CAP_ARM_EL1_32BIT(仅限 arm64)。)
	- KVM_ARM_VCPU_PSCI_0_2: Emulate PSCI v0.2 (or a future revision
          backward compatible with v0.2) for the CPU.
	  Depends on KVM_CAP_ARM_PSCI_0_2.
	  (为该 CPU 模拟 PSCI v0.2(或与 v0.2 向后兼容的未来版本)。依赖 KVM_CAP_ARM_PSCI_0_2。)
	- KVM_ARM_VCPU_PMU_V3: Emulate PMUv3 for the CPU.
	  Depends on KVM_CAP_ARM_PMU_V3.
	  (为该 CPU 模拟 PMUv3。依赖 KVM_CAP_ARM_PMU_V3。)

	- KVM_ARM_VCPU_PTRAUTH_ADDRESS: Enables Address Pointer authentication
	  for arm64 only.
	  Depends on KVM_CAP_ARM_PTRAUTH_ADDRESS.
	  If KVM_CAP_ARM_PTRAUTH_ADDRESS and KVM_CAP_ARM_PTRAUTH_GENERIC are
	  both present, then both KVM_ARM_VCPU_PTRAUTH_ADDRESS and
	  KVM_ARM_VCPU_PTRAUTH_GENERIC must be requested or neither must be
	  requested.
	  (启用地址指针认证，仅限 arm64。依赖 KVM_CAP_ARM_PTRAUTH_ADDRESS。如果 KVM_CAP_ARM_PTRAUTH_ADDRESS 与 KVM_CAP_ARM_PTRAUTH_GENERIC 同时存在，则 KVM_ARM_VCPU_PTRAUTH_ADDRESS 与 KVM_ARM_VCPU_PTRAUTH_GENERIC 必须同时请求，或都不请求。)

	- KVM_ARM_VCPU_PTRAUTH_GENERIC: Enables Generic Pointer authentication
	  for arm64 only.
	  Depends on KVM_CAP_ARM_PTRAUTH_GENERIC.
	  If KVM_CAP_ARM_PTRAUTH_ADDRESS and KVM_CAP_ARM_PTRAUTH_GENERIC are
	  both present, then both KVM_ARM_VCPU_PTRAUTH_ADDRESS and
	  KVM_ARM_VCPU_PTRAUTH_GENERIC must be requested or neither must be
	  requested.
	  (启用通用指针认证，仅限 arm64。依赖 KVM_CAP_ARM_PTRAUTH_GENERIC。如果 KVM_CAP_ARM_PTRAUTH_ADDRESS 与 KVM_CAP_ARM_PTRAUTH_GENERIC 同时存在，则 KVM_ARM_VCPU_PTRAUTH_ADDRESS 与 KVM_ARM_VCPU_PTRAUTH_GENERIC 必须同时请求，或都不请求。)

	- KVM_ARM_VCPU_SVE: Enables SVE for the CPU (arm64 only).
	  Depends on KVM_CAP_ARM_SVE.
	  Requires KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE):
	  (为该 CPU 启用 SVE(仅限 arm64)。依赖 KVM_CAP_ARM_SVE。需要 KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE)：)

	   * After KVM_ARM_VCPU_INIT:
	     (在 KVM_ARM_VCPU_INIT 之后：)

	      - KVM_REG_ARM64_SVE_VLS may be read using KVM_GET_ONE_REG: the
	        initial value of this pseudo-register indicates the best set of
	        vector lengths possible for a vcpu on this host.
	        (可以用 KVM_GET_ONE_REG 读取 KVM_REG_ARM64_SVE_VLS：该伪寄存器的初始值给出此宿主机上一个 vcpu 可能的最佳向量长度集合。)

	   * Before KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE):
	     (在 KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE) 之前：)

	      - KVM_RUN and KVM_GET_REG_LIST are not available;
	        (KVM_RUN 和 KVM_GET_REG_LIST 不可用；)

	      - KVM_GET_ONE_REG and KVM_SET_ONE_REG cannot be used to access
	        the scalable architectural SVE registers
	        KVM_REG_ARM64_SVE_ZREG(), KVM_REG_ARM64_SVE_PREG() or
	        KVM_REG_ARM64_SVE_FFR;
	        (KVM_GET_ONE_REG 和 KVM_SET_ONE_REG 不能用于访问可伸缩架构 SVE 寄存器 KVM_REG_ARM64_SVE_ZREG()、KVM_REG_ARM64_SVE_PREG() 或 KVM_REG_ARM64_SVE_FFR；)

	      - KVM_REG_ARM64_SVE_VLS may optionally be written using
	        KVM_SET_ONE_REG, to modify the set of vector lengths available
	        for the vcpu.
	        (可以选择用 KVM_SET_ONE_REG 写 KVM_REG_ARM64_SVE_VLS，以修改该 vcpu 可用的向量长度集合。)

	   * After KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE):
	     (在 KVM_ARM_VCPU_FINALIZE(KVM_ARM_VCPU_SVE) 之后：)

	      - the KVM_REG_ARM64_SVE_VLS pseudo-register is immutable, and can
	        no longer be written using KVM_SET_ONE_REG.
	        (KVM_REG_ARM64_SVE_VLS 伪寄存器不可变，不能再通过 KVM_SET_ONE_REG 写入。)

	- KVM_ARM_VCPU_HAS_EL2: Enable Nested Virtualisation support,
	  booting the guest from EL2 instead of EL1.
	  Depends on KVM_CAP_ARM_EL2.
	  The VM is running with HCR_EL2.E2H being RES1 (VHE) unless
	  KVM_ARM_VCPU_HAS_EL2_E2H0 is also set.
	  (启用嵌套虚拟化支持，让客户机从 EL2 而不是 EL1 启动。依赖 KVM_CAP_ARM_EL2。除非还设置了 KVM_ARM_VCPU_HAS_EL2_E2H0，否则 VM 将以 HCR_EL2.E2H 为 RES1 (VHE) 运行。)

	- KVM_ARM_VCPU_HAS_EL2_E2H0: Restrict Nested Virtualisation
	  support to HCR_EL2.E2H being RES0 (non-VHE).
	  Depends on KVM_CAP_ARM_EL2_E2H0.
	  KVM_ARM_VCPU_HAS_EL2 must also be set.
	  (把嵌套虚拟化支持限制为 HCR_EL2.E2H 为 RES0 (non-VHE)。依赖 KVM_CAP_ARM_EL2_E2H0。还必须设置 KVM_ARM_VCPU_HAS_EL2。)

4.83 KVM_ARM_PREFERRED_TARGET(4.83 KVM_ARM_PREFERRED_TARGET)
------------------------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: arm64
  (架构：arm64)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_vcpu_init (out)
  (参数：struct kvm_vcpu_init (out))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

Errors:
(错误：)

  ======     ==========================================
  ENODEV     no preferred target available for the host
             (宿主机没有可用的首选 target)
  ======     ==========================================

This queries KVM for preferred CPU target type which can be emulated
by KVM on underlying host.
(此调用向 KVM 查询可在底层宿主机上由 KVM 模拟的首选 CPU target 类型。)

The ioctl returns struct kvm_vcpu_init instance containing information
about preferred CPU target type and recommended features for it.  The
kvm_vcpu_init->features bitmap returned will have feature bits set if
the preferred target recommends setting these features, but this is
not mandatory.
(该 ioctl 返回一个 struct kvm_vcpu_init 实例，其中包含首选 CPU target 类型的信息及其推荐特性。返回的 kvm_vcpu_init->features 位图会在首选 target 建议设置这些特性时置位相应特性位，但这不是强制的。)

The information returned by this ioctl can be used to prepare an instance
of struct kvm_vcpu_init for KVM_ARM_VCPU_INIT ioctl which will result in
VCPU matching underlying host.
(此 ioctl 返回的信息可用于为 KVM_ARM_VCPU_INIT ioctl 准备一个 struct kvm_vcpu_init 实例，这样得到的 VCPU 将与底层宿主机匹配。)


4.84 KVM_GET_REG_LIST(4.84 KVM_GET_REG_LIST)
--------------------------------------------

:Capability: basic
  (能力：basic)
:Architectures: arm64, mips, riscv, x86 (if KVM_CAP_ONE_REG)
  (架构：arm64、mips、riscv、x86(如果有 KVM_CAP_ONE_REG))
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_reg_list (in/out)
  (参数：struct kvm_reg_list (in/out))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

Errors:
(错误：)

  =====      ==============================================================
  E2BIG      the reg index list is too big to fit in the array specified by
             the user (the number required will be written into n).
             (寄存器索引列表过大，无法放入用户指定的数组(所需数量会写入 n)。)
  =====      ==============================================================

::

  struct kvm_reg_list {
	__u64 n; /* number of registers in reg[] */
	__u64 reg[0];
  };

This ioctl returns the guest registers that are supported for the
KVM_GET_ONE_REG/KVM_SET_ONE_REG calls.
(此 ioctl 返回 KVM_GET_ONE_REG/KVM_SET_ONE_REG 调用所支持的客户机寄存器。)

Note that s390 does not support KVM_GET_REG_LIST for historical reasons
(read: nobody cared).  The set of registers in kernels 4.x and newer is:
(注意，出于历史原因(说白了：没人关心)，s390 不支持 KVM_GET_REG_LIST。内核 4.x 及更新版本中的寄存器集合为：)

- KVM_REG_S390_TODPR

- KVM_REG_S390_EPOCHDIFF

- KVM_REG_S390_CPU_TIMER

- KVM_REG_S390_CLOCK_COMP

- KVM_REG_S390_PFTOKEN

- KVM_REG_S390_PFCOMPARE

- KVM_REG_S390_PFSELECT

- KVM_REG_S390_PP

- KVM_REG_S390_GBEA

Note, for x86, all MSRs enumerated by KVM_GET_MSR_INDEX_LIST are supported as
type KVM_X86_REG_TYPE_MSR, but are NOT enumerated via KVM_GET_REG_LIST.
(注意，对 x86 来说，KVM_GET_MSR_INDEX_LIST 枚举出的所有 MSR 都作为类型 KVM_X86_REG_TYPE_MSR 受支持，但不会通过 KVM_GET_REG_LIST 枚举。)

4.85 KVM_ARM_SET_DEVICE_ADDR (deprecated)(4.85 KVM_ARM_SET_DEVICE_ADDR (deprecated))
------------------------------------------------------------------------------------

:Capability: KVM_CAP_ARM_SET_DEVICE_ADDR
  (能力：KVM_CAP_ARM_SET_DEVICE_ADDR)
:Architectures: arm64
  (架构：arm64)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_arm_device_address (in)
  (参数：struct kvm_arm_device_address (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Errors:
(错误：)

  ======  ============================================
  ENODEV  The device id is unknown
          (设备 id 未知)
  ENXIO   Device not supported on current system
          (当前系统不支持该设备)
  EEXIST  Address already set
          (地址已被设置)
  E2BIG   Address outside guest physical address space
          (地址在客户机物理地址空间之外)
  EBUSY   Address overlaps with other device range
          (地址与其他设备范围重叠)
  ======  ============================================

::

  struct kvm_arm_device_addr {
	__u64 id;
	__u64 addr;
  };

Specify a device address in the guest's physical address space where guests
can access emulated or directly exposed devices, which the host kernel needs
to know about. The id field is an architecture specific identifier for a
specific device.
(在客户机物理地址空间中指定一个设备地址，客户机可以从该地址访问模拟的或直接暴露的设备，宿主内核需要知道这些地址。id 字段是特定设备的架构特定标识符。)

arm64 divides the id field into two parts, a device id and an
address type id specific to the individual device::

  bits:  | 63        ...       32 | 31    ...    16 | 15    ...    0 |
  field: |        0x00000000      |     device id   |  addr type id  |

(arm64 把 id 字段分成两部分：设备 id 和特定于该设备的地址类型 id：)

arm64 currently only require this when using the in-kernel GIC
support for the hardware VGIC features, using KVM_ARM_DEVICE_VGIC_V2
as the device id.  When setting the base address for the guest's
mapping of the VGIC virtual CPU and distributor interface, the ioctl
must be called after calling KVM_CREATE_IRQCHIP, but before calling
KVM_RUN on any of the VCPUs.  Calling this ioctl twice for any of the
base addresses will return -EEXIST.
(在 arm64 上，目前只有在使用内核内 GIC 支持以获得硬件 VGIC 特性时才需要此调用，设备 id 使用 KVM_ARM_DEVICE_VGIC_V2。设置客户机映射 VGIC 虚拟 CPU 和分发器 (distributor) 接口的基地址时，必须在调用 KVM_CREATE_IRQCHIP 之后、对任何 VCPU 调用 KVM_RUN 之前调用此 ioctl。对任一基地址两次调用此 ioctl 会返回 -EEXIST。)

Note, this IOCTL is deprecated and the more flexible SET/GET_DEVICE_ATTR API
should be used instead.
(注意，此 IOCTL 已废弃，应改用更灵活的 SET/GET_DEVICE_ATTR API。)


4.86 KVM_PPC_RTAS_DEFINE_TOKEN(4.86 KVM_PPC_RTAS_DEFINE_TOKEN)
--------------------------------------------------------------

:Capability: KVM_CAP_PPC_RTAS
  (能力：KVM_CAP_PPC_RTAS)
:Architectures: ppc
  (架构：ppc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_rtas_token_args
  (参数：struct kvm_rtas_token_args)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Defines a token value for a RTAS (Run Time Abstraction Services)
service in order to allow it to be handled in the kernel.  The
argument struct gives the name of the service, which must be the name
of a service that has a kernel-side implementation.  If the token
value is non-zero, it will be associated with that service, and
subsequent RTAS calls by the guest specifying that token will be
handled by the kernel.  If the token value is 0, then any token
associated with the service will be forgotten, and subsequent RTAS
calls by the guest for that service will be passed to userspace to be
handled.
(为某个 RTAS (Run Time Abstraction Services) 服务定义一个令牌 (token) 值，以便让它能在内核中处理。参数结构给出服务的名称，该名称必须是已有内核侧实现的服务。如果令牌值非 0，它将与该服务关联，客户机后续指定该令牌的 RTAS 调用将由内核处理。如果令牌值为 0，与该服务关联的任何令牌都会被遗忘，客户机后续针对该服务的 RTAS 调用会被传给用户态处理。)

4.87 KVM_SET_GUEST_DEBUG(4.87 KVM_SET_GUEST_DEBUG)
--------------------------------------------------

:Capability: KVM_CAP_SET_GUEST_DEBUG
  (能力：KVM_CAP_SET_GUEST_DEBUG)
:Architectures: x86, s390, ppc, arm64
  (架构：x86、s390、ppc、arm64)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_guest_debug (in)
  (参数：struct kvm_guest_debug (in))
:Returns: 0 on success; -1 on error
  (返回值：成功时为 0；出错时为 -1)

::

  struct kvm_guest_debug {
       __u32 control;
       __u32 pad;
       struct kvm_guest_debug_arch arch;
  };

Set up the processor specific debug registers and configure vcpu for
handling guest debug events. There are two parts to the structure, the
first a control bitfield indicates the type of debug events to handle
when running. Common control bits are:
(设置处理器特定的调试寄存器并配置 vcpu 以处理客户机调试事件。结构有两部分：第一部分是控制位域，指明运行时要处理的调试事件类型。常用控制位有：)

  - KVM_GUESTDBG_ENABLE:        guest debugging is enabled
    (启用客户机调试)
  - KVM_GUESTDBG_SINGLESTEP:    the next run should single-step
    (下一次运行应单步执行)

The top 16 bits of the control field are architecture specific control
flags which can include the following:
(control 字段的高 16 位是架构特定的控制标志，可以包括以下各项：)

  - KVM_GUESTDBG_USE_SW_BP:     using software breakpoints [x86, arm64]
    (使用软件断点 [x86, arm64])
  - KVM_GUESTDBG_USE_HW_BP:     using hardware breakpoints [x86, s390]
    (使用硬件断点 [x86, s390])
  - KVM_GUESTDBG_USE_HW:        using hardware debug events [arm64]
    (使用硬件调试事件 [arm64])
  - KVM_GUESTDBG_INJECT_DB:     inject DB type exception [x86]
    (注入 DB 类型异常 [x86])
  - KVM_GUESTDBG_INJECT_BP:     inject BP type exception [x86]
    (注入 BP 类型异常 [x86])
  - KVM_GUESTDBG_EXIT_PENDING:  trigger an immediate guest exit [s390]
    (触发一次立即的客户机退出 [s390])
  - KVM_GUESTDBG_BLOCKIRQ:      avoid injecting interrupts/NMI/SMI [x86]
    (避免注入中断/NMI/SMI [x86])

For example KVM_GUESTDBG_USE_SW_BP indicates that software breakpoints
are enabled in memory so we need to ensure breakpoint exceptions are
correctly trapped and the KVM run loop exits at the breakpoint and not
running off into the normal guest vector. For KVM_GUESTDBG_USE_HW_BP
we need to ensure the guest vCPUs architecture specific registers are
updated to the correct (supplied) values.
(例如 KVM_GUESTDBG_USE_SW_BP 表示内存中启用了软件断点，因此我们需要确保断点异常被正确捕获，并且 KVM 运行循环在断点处退出，而不是跑到普通的客户机向量。对 KVM_GUESTDBG_USE_HW_BP，我们需要确保把客户机 vCPU 的架构特定寄存器更新为正确(所提供)的值。)

The second part of the structure is architecture specific and
typically contains a set of debug registers.
(结构的第二部分是架构特定的，通常包含一组调试寄存器。)

For arm64 the number of debug registers is implementation defined and
can be determined by querying the KVM_CAP_GUEST_DEBUG_HW_BPS and
KVM_CAP_GUEST_DEBUG_HW_WPS capabilities which return a positive number
indicating the number of supported registers.
(对 arm64，调试寄存器的数量由实现定义，可以通过查询 KVM_CAP_GUEST_DEBUG_HW_BPS 和 KVM_CAP_GUEST_DEBUG_HW_WPS 能力来确定，它们返回一个正数，表示受支持的寄存器数量。)

For ppc, the KVM_CAP_PPC_GUEST_DEBUG_SSTEP capability indicates whether
the single-step debug event (KVM_GUESTDBG_SINGLESTEP) is supported.
(对 ppc，KVM_CAP_PPC_GUEST_DEBUG_SSTEP 能力表明是否支持单步调试事件 (KVM_GUESTDBG_SINGLESTEP)。)

Also when supported, KVM_CAP_SET_GUEST_DEBUG2 capability indicates the
supported KVM_GUESTDBG_* bits in the control field.
(另外，在受支持的情况下，KVM_CAP_SET_GUEST_DEBUG2 能力指明 control 字段中受支持的 KVM_GUESTDBG_* 位。)

When debug events exit the main run loop with the reason
KVM_EXIT_DEBUG with the kvm_debug_exit_arch part of the kvm_run
structure containing architecture specific debug information.
(当调试事件以 KVM_EXIT_DEBUG 原因退出主运行循环时，kvm_run 结构的 kvm_debug_exit_arch 部分包含架构特定的调试信息。)

4.88 KVM_GET_EMULATED_CPUID(4.88 KVM_GET_EMULATED_CPUID)
--------------------------------------------------------

:Capability: KVM_CAP_EXT_EMUL_CPUID
  (能力：KVM_CAP_EXT_EMUL_CPUID)
:Architectures: x86
  (架构：x86)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: struct kvm_cpuid2 (in/out)
  (参数：struct kvm_cpuid2 (in/out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

::

  struct kvm_cpuid2 {
	__u32 nent;
	__u32 flags;
	struct kvm_cpuid_entry2 entries[0];
  };

The member 'flags' is used for passing flags from userspace.
('flags' 成员用于从用户态传入标志。)

::

  #define KVM_CPUID_FLAG_SIGNIFCANT_INDEX		BIT(0)
  #define KVM_CPUID_FLAG_STATEFUL_FUNC		BIT(1) /* deprecated */
  #define KVM_CPUID_FLAG_STATE_READ_NEXT		BIT(2) /* deprecated */

  struct kvm_cpuid_entry2 {
	__u32 function;
	__u32 index;
	__u32 flags;
	__u32 eax;
	__u32 ebx;
	__u32 ecx;
	__u32 edx;
	__u32 padding[3];
  };

This ioctl returns x86 cpuid features which are emulated by
kvm.Userspace can use the information returned by this ioctl to query
which features are emulated by kvm instead of being present natively.
(此 ioctl 返回由 kvm 模拟的 x86 cpuid 特性。用户态可以用此 ioctl 返回的信息查询哪些特性是由 kvm 模拟的而非原生存在。)

Userspace invokes KVM_GET_EMULATED_CPUID by passing a kvm_cpuid2
structure with the 'nent' field indicating the number of entries in
the variable-size array 'entries'. If the number of entries is too low
to describe the cpu capabilities, an error (E2BIG) is returned. If the
number is too high, the 'nent' field is adjusted and an error (ENOMEM)
is returned. If the number is just right, the 'nent' field is adjusted
to the number of valid entries in the 'entries' array, which is then
filled.
(用户态通过传入 kvm_cpuid2 结构体来调用 KVM_GET_EMULATED_CPUID，其中 'nent' 字段指明变长数组 'entries' 中的条目数量。如果条目数量太少而不足以描述 cpu 能力，则返回错误 (E2BIG)。如果数量太多，会调整 'nent' 字段并返回错误 (ENOMEM)。如果数量恰好，'nent' 字段会被调整为 'entries' 数组中有效条目的数量，然后填充该数组。)

The entries returned are the set CPUID bits of the respective features
which kvm emulates, as returned by the CPUID instruction, with unknown
or unsupported feature bits cleared.
(返回的条目是 kvm 所模拟的各特性的已置位 CPUID 位，与 CPUID 指令返回的一致，其中未知或不受支持的特性位被清零。)

Features like x2apic, for example, may not be present in the host cpu
but are exposed by kvm in KVM_GET_SUPPORTED_CPUID because they can be
emulated efficiently and thus not included here.
(例如 x2apic 之类的特性可能并不存在于宿主机 cpu 上，但因为可以被高效模拟而由 kvm 在 KVM_GET_SUPPORTED_CPUID 中暴露，因此不包含在这里。)

The fields in each entry are defined as follows:
(每个条目中的字段定义如下：)

  function:
	 the eax value used to obtain the entry
	 (用于取得该条目的 eax 值)
  index:
	 the ecx value used to obtain the entry (for entries that are
         affected by ecx)
	 (用于取得该条目的 ecx 值(对受 ecx 影响的条目))
  flags:
    an OR of zero or more of the following:
    (以下零个或多个标志的按位或：)

        KVM_CPUID_FLAG_SIGNIFCANT_INDEX:
           if the index field is valid
           (如果 index 字段有效)

   eax, ebx, ecx, edx:

         the values returned by the cpuid instruction for
         this function/index combination
         (cpuid 指令针对该 function/index 组合返回的值)

4.89 KVM_S390_MEM_OP(4.89 KVM_S390_MEM_OP)
------------------------------------------

:Capability: KVM_CAP_S390_MEM_OP, KVM_CAP_S390_PROTECTED, KVM_CAP_S390_MEM_OP_EXTENSION
  (能力：KVM_CAP_S390_MEM_OP、KVM_CAP_S390_PROTECTED、KVM_CAP_S390_MEM_OP_EXTENSION)
:Architectures: s390
  (架构：s390)
:Type: vm ioctl, vcpu ioctl
  (类型：vm ioctl、vcpu ioctl)
:Parameters: struct kvm_s390_mem_op (in)
  (参数：struct kvm_s390_mem_op (in))
:Returns: = 0 on success,
          < 0 on generic error (e.g. -EFAULT or -ENOMEM),
          16 bit program exception code if the access causes such an exception
  (返回值：成功时为 = 0；一般性错误时 < 0(例如 -EFAULT 或 -ENOMEM)；如果该访问引发此类异常，则返回 16 位程序异常码)

Read or write data from/to the VM's memory.
The KVM_CAP_S390_MEM_OP_EXTENSION capability specifies what functionality is
supported.
(从 VM 的内存读取数据或向其写入数据。KVM_CAP_S390_MEM_OP_EXTENSION 能力指明支持哪些功能。)

Parameters are specified via the following structure::

  struct kvm_s390_mem_op {
	__u64 gaddr;		/* the guest address */
	__u64 flags;		/* flags */
	__u32 size;		/* amount of bytes */
	__u32 op;		/* type of operation */
	__u64 buf;		/* buffer in userspace */
	union {
		struct {
			__u8 ar;	/* the access register number */
			__u8 key;	/* access key, ignored if flag unset */
			__u8 pad1[6];	/* ignored */
			__u64 old_addr;	/* ignored if flag unset */
		};
		__u32 sida_offset; /* offset into the sida */
		__u8 reserved[32]; /* ignored */
	};
  };

(参数通过以下结构指定：)

The start address of the memory region has to be specified in the "gaddr"
field, and the length of the region in the "size" field (which must not
be 0). The maximum value for "size" can be obtained by checking the
KVM_CAP_S390_MEM_OP capability. "buf" is the buffer supplied by the
userspace application where the read data should be written to for
a read access, or where the data that should be written is stored for
a write access.  The "reserved" field is meant for future extensions.
Reserved and unused values are ignored. Future extension that add members must
introduce new flags.
(内存区域的起始地址必须在 "gaddr" 字段中指定，区域长度在 "size" 字段中指定(不得为 0)。"size" 的最大值可以通过查询 KVM_CAP_S390_MEM_OP 能力获得。"buf" 是用户态应用程序提供的缓冲区：读访问时读取的数据应写入此处，写访问时待写的数据存放在此处。"reserved" 字段留给未来扩展使用。保留和未使用的值会被忽略。将来增加成员的扩展必须引入新的标志。)

The type of operation is specified in the "op" field. Flags modifying
their behavior can be set in the "flags" field. Undefined flag bits must
be set to 0.
(操作的类型在 "op" 字段中指定。可以在 "flags" 字段中设置修改其行为的标志。未定义的标志位必须置 0。)

Possible operations are:
  * ``KVM_S390_MEMOP_LOGICAL_READ``
  * ``KVM_S390_MEMOP_LOGICAL_WRITE``
  * ``KVM_S390_MEMOP_ABSOLUTE_READ``
  * ``KVM_S390_MEMOP_ABSOLUTE_WRITE``
  * ``KVM_S390_MEMOP_SIDA_READ``
  * ``KVM_S390_MEMOP_SIDA_WRITE``
  * ``KVM_S390_MEMOP_ABSOLUTE_CMPXCHG``

Logical read/write:
^^^^^^^^^^^^^^^^^^^

Access logical memory, i.e. translate the given guest address to an absolute
address given the state of the VCPU and use the absolute address as target of
the access. "ar" designates the access register number to be used; the valid
range is 0..15.
Logical accesses are permitted for the VCPU ioctl only.
Logical accesses are permitted for non-protected guests only.
(访问逻辑内存，即根据 VCPU 的状态把给定的客户机地址转换为绝对地址，并把该绝对地址用作访问目标。"ar" 指定要使用的访问寄存器号；有效范围为 0..15。逻辑访问只允许用于 VCPU ioctl。逻辑访问只允许用于非受保护客户机。)

Supported flags:
(支持的标志：)
  * ``KVM_S390_MEMOP_F_CHECK_ONLY``
  * ``KVM_S390_MEMOP_F_INJECT_EXCEPTION``
  * ``KVM_S390_MEMOP_F_SKEY_PROTECTION``

The KVM_S390_MEMOP_F_CHECK_ONLY flag can be set to check whether the
corresponding memory access would cause an access exception; however,
no actual access to the data in memory at the destination is performed.
In this case, "buf" is unused and can be NULL.
(可以设置 KVM_S390_MEMOP_F_CHECK_ONLY 标志来检查相应的内存访问是否会引发访问异常；但不会真正访问目标处的内存数据。这种情况下 "buf" 不被使用，可以为 NULL。)

In case an access exception occurred during the access (or would occur
in case of KVM_S390_MEMOP_F_CHECK_ONLY), the ioctl returns a positive
error number indicating the type of exception. This exception is also
raised directly at the corresponding VCPU if the flag
KVM_S390_MEMOP_F_INJECT_EXCEPTION is set.
On protection exceptions, unless specified otherwise, the injected
translation-exception identifier (TEID) indicates suppression.
(如果在访问期间发生了访问异常(或在 KVM_S390_MEMOP_F_CHECK_ONLY 情况下本会发生异常)，ioctl 返回一个正的错误编号，指示异常类型。如果设置了 KVM_S390_MEMOP_F_INJECT_EXCEPTION 标志，该异常也会直接在相应 VCPU 上引发。对保护异常，除非另有说明，注入的转换异常标识符 (TEID) 指示抑制 (suppression)。)

If the KVM_S390_MEMOP_F_SKEY_PROTECTION flag is set, storage key
protection is also in effect and may cause exceptions if accesses are
prohibited given the access key designated by "key"; the valid range is 0..15.
KVM_S390_MEMOP_F_SKEY_PROTECTION is available if KVM_CAP_S390_MEM_OP_EXTENSION
is > 0.
Since the accessed memory may span multiple pages and those pages might have
different storage keys, it is possible that a protection exception occurs
after memory has been modified. In this case, if the exception is injected,
the TEID does not indicate suppression.
(如果设置了 KVM_S390_MEMOP_F_SKEY_PROTECTION 标志，存储键 (storage key) 保护也一并生效；在按 "key" 指定的访问键禁止访问时可能引发异常；有效范围为 0..15。当 KVM_CAP_S390_MEM_OP_EXTENSION > 0 时，KVM_S390_MEMOP_F_SKEY_PROTECTION 可用。由于被访问的内存可能跨越多页且各页可能具有不同的存储键，保护异常有可能发生在内存已被修改之后。这种情况下，如果异常被注入，TEID 不指示抑制。)

Absolute read/write:
^^^^^^^^^^^^^^^^^^^^

Access absolute memory. This operation is intended to be used with the
KVM_S390_MEMOP_F_SKEY_PROTECTION flag, to allow accessing memory and performing
the checks required for storage key protection as one operation (as opposed to
user space getting the storage keys, performing the checks, and accessing
memory thereafter, which could lead to a delay between check and access).
Absolute accesses are permitted for the VM ioctl if KVM_CAP_S390_MEM_OP_EXTENSION
has the KVM_S390_MEMOP_EXTENSION_CAP_BASE bit set.
Currently absolute accesses are not permitted for VCPU ioctls.
Absolute accesses are permitted for non-protected guests only.
(访问绝对内存。此操作旨在与 KVM_S390_MEMOP_F_SKEY_PROTECTION 标志配合使用，以便把访问内存与执行存储键保护所需的检查合并为一次操作(而不是由用户态先取存储键、执行检查、然后再访问内存——那样会在检查与访问之间产生延迟)。如果 KVM_CAP_S390_MEM_OP_EXTENSION 设置了 KVM_S390_MEMOP_EXTENSION_CAP_BASE 位，则 VM ioctl 允许绝对访问。目前 VCPU ioctl 不允许绝对访问。绝对访问只允许用于非受保护客户机。)

Supported flags:
(支持的标志：)
  * ``KVM_S390_MEMOP_F_CHECK_ONLY``
  * ``KVM_S390_MEMOP_F_SKEY_PROTECTION``

The semantics of the flags common with logical accesses are as for logical
accesses.
(与逻辑访问共有的那些标志，其语义与逻辑访问时相同。)

Absolute cmpxchg:
^^^^^^^^^^^^^^^^^

Perform cmpxchg on absolute guest memory. Intended for use with the
KVM_S390_MEMOP_F_SKEY_PROTECTION flag.
Instead of doing an unconditional write, the access occurs only if the target
location contains the value pointed to by "old_addr".
This is performed as an atomic cmpxchg with the length specified by the "size"
parameter. "size" must be a power of two up to and including 16.
If the exchange did not take place because the target value doesn't match the
old value, the value "old_addr" points to is replaced by the target value.
User space can tell if an exchange took place by checking if this replacement
occurred. The cmpxchg op is permitted for the VM ioctl if
KVM_CAP_S390_MEM_OP_EXTENSION has flag KVM_S390_MEMOP_EXTENSION_CAP_CMPXCHG set.
(对绝对客户机内存执行 cmpxchg。旨在与 KVM_S390_MEMOP_F_SKEY_PROTECTION 标志配合使用。它不是无条件写入，而是只有当目标位置包含 "old_addr" 所指向的值时才执行访问。它以 "size" 参数指定的长度作为原子 cmpxchg 执行。"size" 必须是 2 的幂，最大到 16(含)。如果因为目标值与旧值不匹配而未发生交换，"old_addr" 所指向的值会被替换为目标值。用户态可以通过检查是否发生了这种替换来判断是否进行了交换。如果 KVM_CAP_S390_MEM_OP_EXTENSION 设置了标志 KVM_S390_MEMOP_EXTENSION_CAP_CMPXCHG，则 cmpxchg 操作允许用于 VM ioctl。)

Supported flags:
(支持的标志：)
  * ``KVM_S390_MEMOP_F_SKEY_PROTECTION``

SIDA read/write:
^^^^^^^^^^^^^^^^

Access the secure instruction data area which contains memory operands necessary
for instruction emulation for protected guests.
SIDA accesses are available if the KVM_CAP_S390_PROTECTED capability is available.
SIDA accesses are permitted for the VCPU ioctl only.
SIDA accesses are permitted for protected guests only.
(访问安全指令数据区 (secure instruction data area)，其中包含为受保护客户机进行指令模拟所需的内存操作数。只有 KVM_CAP_S390_PROTECTED 能力可用时才能使用 SIDA 访问。SIDA 访问只允许用于 VCPU ioctl。SIDA 访问只允许用于受保护客户机。)

No flags are supported.
(不支持任何标志。)

4.90 KVM_S390_GET_SKEYS(4.90 KVM_S390_GET_SKEYS)
------------------------------------------------

:Capability: KVM_CAP_S390_SKEYS
  (能力：KVM_CAP_S390_SKEYS)
:Architectures: s390
  (架构：s390)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_s390_skeys
  (参数：struct kvm_s390_skeys)
:Returns: 0 on success, KVM_S390_GET_SKEYS_NONE if guest is not using storage
          keys, negative value on error
  (返回值：成功时为 0；客户机未使用存储键时为 KVM_S390_GET_SKEYS_NONE；出错时为负值)

This ioctl is used to get guest storage key values on the s390
architecture. The ioctl takes parameters via the kvm_s390_skeys struct::

  struct kvm_s390_skeys {
	__u64 start_gfn;
	__u64 count;
	__u64 skeydata_addr;
	__u32 flags;
	__u32 reserved[9];
  };

(此 ioctl 用于在 s390 架构上获取客户机存储键值。ioctl 通过 kvm_s390_skeys 结构取得参数：)

The start_gfn field is the number of the first guest frame whose storage keys
you want to get.
(start_gfn 字段是要获取其存储键的第一个客户机帧号。)

The count field is the number of consecutive frames (starting from start_gfn)
whose storage keys to get. The count field must be at least 1 and the maximum
allowed value is defined as KVM_S390_SKEYS_MAX. Values outside this range
will cause the ioctl to return -EINVAL.
(count 字段是要获取其存储键的连续帧数量(从 start_gfn 开始)。count 字段必须至少为 1，允许的最大值定义为 KVM_S390_SKEYS_MAX。超出此范围的值会导致 ioctl 返回 -EINVAL。)

The skeydata_addr field is the address to a buffer large enough to hold count
bytes. This buffer will be filled with storage key data by the ioctl.
(skeydata_addr 字段是指向一个足以容纳 count 字节的缓冲区的地址。ioctl 会用存储键数据填充该缓冲区。)

4.91 KVM_S390_SET_SKEYS(4.91 KVM_S390_SET_SKEYS)
------------------------------------------------

:Capability: KVM_CAP_S390_SKEYS
  (能力：KVM_CAP_S390_SKEYS)
:Architectures: s390
  (架构：s390)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_s390_skeys
  (参数：struct kvm_s390_skeys)
:Returns: 0 on success, negative value on error
  (返回值：成功时为 0，出错时为负值)

This ioctl is used to set guest storage key values on the s390
architecture. The ioctl takes parameters via the kvm_s390_skeys struct.
See section on KVM_S390_GET_SKEYS for struct definition.
(此 ioctl 用于在 s390 架构上设置客户机存储键值。ioctl 通过 kvm_s390_skeys 结构取得参数。结构定义参见 KVM_S390_GET_SKEYS 一节。)

The start_gfn field is the number of the first guest frame whose storage keys
you want to set.
(start_gfn 字段是要设置其存储键的第一个客户机帧号。)

The count field is the number of consecutive frames (starting from start_gfn)
whose storage keys to get. The count field must be at least 1 and the maximum
allowed value is defined as KVM_S390_SKEYS_MAX. Values outside this range
will cause the ioctl to return -EINVAL.
(count 字段是要获取其存储键的连续帧数量(从 start_gfn 开始)。count 字段必须至少为 1，允许的最大值定义为 KVM_S390_SKEYS_MAX。超出此范围的值会导致 ioctl 返回 -EINVAL。)

The skeydata_addr field is the address to a buffer containing count bytes of
storage keys. Each byte in the buffer will be set as the storage key for a
single frame starting at start_gfn for count frames.
(skeydata_addr 字段是指向一个包含 count 字节存储键的缓冲区的地址。缓冲区中的每个字节将作为单个帧的存储键被设置，从 start_gfn 开始共 count 帧。)

Note: If any architecturally invalid key value is found in the given data then
the ioctl will return -EINVAL.
(注意：如果在给定数据中发现任何架构上无效的键值，ioctl 会返回 -EINVAL。)

4.92 KVM_S390_IRQ(4.92 KVM_S390_IRQ)
------------------------------------

:Capability: KVM_CAP_S390_INJECT_IRQ
  (能力：KVM_CAP_S390_INJECT_IRQ)
:Architectures: s390
  (架构：s390)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_s390_irq (in)
  (参数：struct kvm_s390_irq (in))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Errors:
(错误：)


  ======  =================================================================
  EINVAL  interrupt type is invalid
          type is KVM_S390_SIGP_STOP and flag parameter is invalid value,
          type is KVM_S390_INT_EXTERNAL_CALL and code is bigger
          than the maximum of VCPUs
          (中断类型无效；类型为 KVM_S390_SIGP_STOP 且 flag 参数是无效值；类型为 KVM_S390_INT_EXTERNAL_CALL 且代码大于 VCPU 最大数量)
  EBUSY   type is KVM_S390_SIGP_SET_PREFIX and vcpu is not stopped,
          type is KVM_S390_SIGP_STOP and a stop irq is already pending,
          type is KVM_S390_INT_EXTERNAL_CALL and an external call interrupt
          is already pending
          (类型为 KVM_S390_SIGP_SET_PREFIX 且 vcpu 未停止；类型为 KVM_S390_SIGP_STOP 且已有一个 stop irq 待决；类型为 KVM_S390_INT_EXTERNAL_CALL 且已有一个 external call 中断待决)
  ======  =================================================================

Allows to inject an interrupt to the guest.
(允许向客户机注入一个中断。)

Using struct kvm_s390_irq as a parameter allows
to inject additional payload which is not
possible via KVM_S390_INTERRUPT.
(使用 struct kvm_s390_irq 作为参数可以注入额外载荷，这是通过 KVM_S390_INTERRUPT 无法做到的。)

Interrupt parameters are passed via kvm_s390_irq::

  struct kvm_s390_irq {
	__u64 type;
	union {
		struct kvm_s390_io_info io;
		struct kvm_s390_ext_info ext;
		struct kvm_s390_pgm_info pgm;
		struct kvm_s390_emerg_info emerg;
		struct kvm_s390_extcall_info extcall;
		struct kvm_s390_prefix_info prefix;
		struct kvm_s390_stop_info stop;
		struct kvm_s390_mchk_info mchk;
		char reserved[64];
	} u;
  };

(中断参数通过 kvm_s390_irq 传递：)

type can be one of the following:
(type 可以是以下之一：)

- KVM_S390_SIGP_STOP - sigp stop; parameter in .stop
  (sigp stop；参数在 .stop 中)
- KVM_S390_PROGRAM_INT - program check; parameters in .pgm
  (程序检查；参数在 .pgm 中)
- KVM_S390_SIGP_SET_PREFIX - sigp set prefix; parameters in .prefix
  (sigp set prefix；参数在 .prefix 中)
- KVM_S390_RESTART - restart; no parameters
  (重启；无参数)
- KVM_S390_INT_CLOCK_COMP - clock comparator interrupt; no parameters
  (时钟比较器中断；无参数)
- KVM_S390_INT_CPU_TIMER - CPU timer interrupt; no parameters
  (CPU 定时器中断；无参数)
- KVM_S390_INT_EMERGENCY - sigp emergency; parameters in .emerg
  (sigp emergency；参数在 .emerg 中)
- KVM_S390_INT_EXTERNAL_CALL - sigp external call; parameters in .extcall
  (sigp external call；参数在 .extcall 中)
- KVM_S390_MCHK - machine check interrupt; parameters in .mchk
  (机器检查中断；参数在 .mchk 中)

This is an asynchronous vcpu ioctl and can be invoked from any thread.
(这是异步 vcpu ioctl，可以从任意线程调用。)

4.94 KVM_S390_GET_IRQ_STATE(4.94 KVM_S390_GET_IRQ_STATE)
--------------------------------------------------------

:Capability: KVM_CAP_S390_IRQ_STATE
  (能力：KVM_CAP_S390_IRQ_STATE)
:Architectures: s390
  (架构：s390)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_s390_irq_state (out)
  (参数：struct kvm_s390_irq_state (out))
:Returns: >= number of bytes copied into buffer,
          -EINVAL if buffer size is 0,
          -ENOBUFS if buffer size is too small to fit all pending interrupts,
          -EFAULT if the buffer address was invalid
  (返回值：>= 复制到缓冲区中的字节数；缓冲区大小为 0 时为 -EINVAL；缓冲区太小放不下所有待决中断时为 -ENOBUFS；缓冲区地址无效时为 -EFAULT)

This ioctl allows userspace to retrieve the complete state of all currently
pending interrupts in a single buffer. Use cases include migration
and introspection. The parameter structure contains the address of a
userspace buffer and its length::

  struct kvm_s390_irq_state {
	__u64 buf;
	__u32 flags;        /* will stay unused for compatibility reasons */
	__u32 len;
	__u32 reserved[4];  /* will stay unused for compatibility reasons */
  };

(此 ioctl 允许用户态在单个缓冲区中获取当前所有待决中断的完整状态。用例包括迁移和内省 (introspection)。参数结构包含用户态缓冲区的地址及其长度：)

Userspace passes in the above struct and for each pending interrupt a
struct kvm_s390_irq is copied to the provided buffer.
(用户态传入上述结构，然后每个待决中断会有一个 struct kvm_s390_irq 被复制到提供的缓冲区。)

The structure contains a flags and a reserved field for future extensions. As
the kernel never checked for flags == 0 and QEMU never pre-zeroed flags and
reserved, these fields can not be used in the future without breaking
compatibility.
(该结构包含用于未来扩展的 flags 和 reserved 字段。由于内核从未检查 flags == 0，且 QEMU 从未预先把 flags 和 reserved 清零，这些字段将来若不破坏兼容性就无法使用。)

If -ENOBUFS is returned the buffer provided was too small and userspace
may retry with a bigger buffer.
(如果返回 -ENOBUFS，说明提供的缓冲区太小，用户态可以用更大的缓冲区重试。)

4.95 KVM_S390_SET_IRQ_STATE(4.95 KVM_S390_SET_IRQ_STATE)
--------------------------------------------------------

:Capability: KVM_CAP_S390_IRQ_STATE
  (能力：KVM_CAP_S390_IRQ_STATE)
:Architectures: s390
  (架构：s390)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_s390_irq_state (in)
  (参数：struct kvm_s390_irq_state (in))
:Returns: 0 on success,
          -EFAULT if the buffer address was invalid,
          -EINVAL for an invalid buffer length (see below),
          -EBUSY if there were already interrupts pending,
          errors occurring when actually injecting the
          interrupt. See KVM_S390_IRQ.
  (返回值：成功时为 0；缓冲区地址无效时为 -EFAULT；缓冲区长度无效时为 -EINVAL(见下文)；已有中断待决时为 -EBUSY；以及实际注入中断时发生的错误。参见 KVM_S390_IRQ。)

This ioctl allows userspace to set the complete state of all cpu-local
interrupts currently pending for the vcpu. It is intended for restoring
interrupt state after a migration. The input parameter is a userspace buffer
containing a struct kvm_s390_irq_state::

  struct kvm_s390_irq_state {
	__u64 buf;
	__u32 flags;        /* will stay unused for compatibility reasons */
	__u32 len;
	__u32 reserved[4];  /* will stay unused for compatibility reasons */
  };

(此 ioctl 允许用户态设置该 vcpu 当前待决的所有 cpu 本地中断的完整状态。它用于在迁移之后恢复中断状态。输入参数是包含 struct kvm_s390_irq_state 的用户态缓冲区：)

The restrictions for flags and reserved apply as well.
(see KVM_S390_GET_IRQ_STATE)
(flags 和 reserved 的限制同样适用(参见 KVM_S390_GET_IRQ_STATE)。)

The userspace memory referenced by buf contains a struct kvm_s390_irq
for each interrupt to be injected into the guest.
If one of the interrupts could not be injected for some reason the
ioctl aborts.
(buf 所引用的用户态内存中，对每个要注入客户机的中断各包含一个 struct kvm_s390_irq。如果由于某种原因某个中断无法注入，ioctl 会中止。)

len must be a multiple of sizeof(struct kvm_s390_irq). It must be > 0
and it must not exceed (max_vcpus + 32) * sizeof(struct kvm_s390_irq),
which is the maximum number of possibly pending cpu-local interrupts.
(len 必须是 sizeof(struct kvm_s390_irq) 的倍数。它必须 > 0，且不得超过 (max_vcpus + 32) * sizeof(struct kvm_s390_irq)，即可能待决的 cpu 本地中断的最大数量。)

4.96 KVM_SMI(4.96 KVM_SMI)
--------------------------

:Capability: KVM_CAP_X86_SMM
  (能力：KVM_CAP_X86_SMM)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: none
  (参数：无)
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Queues an SMI on the thread's vcpu.
(在该线程的 vcpu 上把一个 SMI 排队。)

4.97 KVM_X86_SET_MSR_FILTER(4.97 KVM_X86_SET_MSR_FILTER)
---------------------------------------------------------

:Capability: KVM_CAP_X86_MSR_FILTER
  (能力：KVM_CAP_X86_MSR_FILTER)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_msr_filter
  (参数：struct kvm_msr_filter)
:Returns: 0 on success, < 0 on error
  (返回值：成功时为 0，出错时 < 0)

::

  struct kvm_msr_filter_range {
  #define KVM_MSR_FILTER_READ  (1 << 0)
  #define KVM_MSR_FILTER_WRITE (1 << 1)
	__u32 flags;
	__u32 nmsrs; /* number of msrs in bitmap */
	__u32 base;  /* MSR index the bitmap starts at */
	__u8 *bitmap; /* a 1 bit allows the operations in flags, 0 denies */
  };

  #define KVM_MSR_FILTER_MAX_RANGES 16
  struct kvm_msr_filter {
  #define KVM_MSR_FILTER_DEFAULT_ALLOW (0 << 0)
  #define KVM_MSR_FILTER_DEFAULT_DENY  (1 << 0)
	__u32 flags;
	struct kvm_msr_filter_range ranges[KVM_MSR_FILTER_MAX_RANGES];
  };

flags values for ``struct kvm_msr_filter_range``:
(struct kvm_msr_filter_range 的 flags 取值：)

``KVM_MSR_FILTER_READ``

  Filter read accesses to MSRs using the given bitmap. A 0 in the bitmap
  indicates that read accesses should be denied, while a 1 indicates that
  a read for a particular MSR should be allowed regardless of the default
  filter action.
  (使用给定的位图过滤对 MSR 的读访问。位图中的 0 表示应拒绝读访问，而 1 表示对特定 MSR 的读应被允许，不论默认的过滤行为如何。)

``KVM_MSR_FILTER_WRITE``

  Filter write accesses to MSRs using the given bitmap. A 0 in the bitmap
  indicates that write accesses should be denied, while a 1 indicates that
  a write for a particular MSR should be allowed regardless of the default
  filter action.
  (使用给定的位图过滤对 MSR 的写访问。位图中的 0 表示应拒绝写访问，而 1 表示对特定 MSR 的写应被允许，不论默认的过滤行为如何。)

flags values for ``struct kvm_msr_filter``:
(struct kvm_msr_filter 的 flags 取值：)

``KVM_MSR_FILTER_DEFAULT_ALLOW``

  If no filter range matches an MSR index that is getting accessed, KVM will
  allow accesses to all MSRs by default.
  (如果没有过滤器范围与正在被访问的 MSR 索引匹配，KVM 默认允许对所有 MSR 的访问。)

``KVM_MSR_FILTER_DEFAULT_DENY``

  If no filter range matches an MSR index that is getting accessed, KVM will
  deny accesses to all MSRs by default.
  (如果没有过滤器范围与正在被访问的 MSR 索引匹配，KVM 默认拒绝对所有 MSR 的访问。)

This ioctl allows userspace to define up to 16 bitmaps of MSR ranges to deny
guest MSR accesses that would normally be allowed by KVM.  If an MSR is not
covered by a specific range, the "default" filtering behavior applies.  Each
bitmap range covers MSRs from [base .. base+nmsrs).
(此 ioctl 允许用户态定义最多 16 个 MSR 范围位图，用来拒绝通常会被 KVM 允许的客户机 MSR 访问。如果某个 MSR 不被任何特定范围覆盖，则应用 "default" 过滤行为。每个位图范围覆盖 [base .. base+nmsrs) 内的 MSR。)

If an MSR access is denied by userspace, the resulting KVM behavior depends on
whether or not KVM_CAP_X86_USER_SPACE_MSR's KVM_MSR_EXIT_REASON_FILTER is
enabled.  If KVM_MSR_EXIT_REASON_FILTER is enabled, KVM will exit to userspace
on denied accesses, i.e. userspace effectively intercepts the MSR access.  If
KVM_MSR_EXIT_REASON_FILTER is not enabled, KVM will inject a #GP into the guest
on denied accesses.  Note, if an MSR access is denied during emulation of MSR
load/stores during VMX transitions, KVM ignores KVM_MSR_EXIT_REASON_FILTER.
See the below warning for full details.
(如果用户态拒绝了某个 MSR 访问，KVM 的最终行为取决于 KVM_CAP_X86_USER_SPACE_MSR 的 KVM_MSR_EXIT_REASON_FILTER 是否启用。如果启用了 KVM_MSR_EXIT_REASON_FILTER，KVM 会在被拒绝的访问上退到用户态，即用户态实际拦截了该 MSR 访问。如果未启用 KVM_MSR_EXIT_REASON_FILTER，KVM 会在被拒绝的访问上向客户机注入 #GP。注意，如果 MSR 访问是在 VMX 转换期间模拟 MSR 装载/保存时被拒绝的，KVM 会忽略 KVM_MSR_EXIT_REASON_FILTER。完整细节参见下面的 warning。)

If an MSR access is allowed by userspace, KVM will emulate and/or virtualize
the access in accordance with the vCPU model.  Note, KVM may still ultimately
inject a #GP if an access is allowed by userspace, e.g. if KVM doesn't support
the MSR, or to follow architectural behavior for the MSR.
(如果用户态允许了某个 MSR 访问，KVM 会按照 vCPU 模型对访问进行模拟和/或虚拟化。注意，即使访问被用户态允许，KVM 最终仍可能注入 #GP，例如 KVM 不支持该 MSR，或为了遵循该 MSR 的架构行为。)

By default, KVM operates in KVM_MSR_FILTER_DEFAULT_ALLOW mode with no MSR range
filters.
(默认情况下，KVM 以 KVM_MSR_FILTER_DEFAULT_ALLOW 模式运行，且没有任何 MSR 范围过滤器。)

Calling this ioctl with an empty set of ranges (all nmsrs == 0) disables MSR
filtering. In that mode, ``KVM_MSR_FILTER_DEFAULT_DENY`` is invalid and causes
an error.
(以空的 ranges 集合(所有 nmsrs == 0)调用此 ioctl 会禁用 MSR 过滤。在该模式下，KVM_MSR_FILTER_DEFAULT_DENY 无效并会导致错误。)

.. warning::
   MSR accesses that are side effects of instruction execution (emulated or
   native) are not filtered as hardware does not honor MSR bitmaps outside of
   RDMSR and WRMSR, and KVM mimics that behavior when emulating instructions
   to avoid pointless divergence from hardware.  E.g. RDPID reads MSR_TSC_AUX,
   SYSENTER reads the SYSENTER MSRs, etc.
   (警告：作为指令执行(模拟的或原生的)副作用的 MSR 访问不会被过滤，因为硬件在 RDMSR 和 WRMSR 之外并不遵守 MSR 位图，而 KVM 在模拟指令时会模仿这一行为，以避免与硬件产生无谓的差异。例如 RDPID 读取 MSR_TSC_AUX，SYSENTER 读取 SYSENTER MSR，等等。)

   MSRs that are loaded/stored via dedicated VMCS fields are not filtered as
   part of VM-Enter/VM-Exit emulation.
   (通过专用 VMCS 字段装载/保存的 MSR，在 VM-Enter/VM-Exit 模拟中不会被过滤。)

   MSRs that are loaded/store via VMX's load/store lists _are_ filtered as part
   of VM-Enter/VM-Exit emulation.  If an MSR access is denied on VM-Enter, KVM
   synthesizes a consistency check VM-Exit(EXIT_REASON_MSR_LOAD_FAIL).  If an
   MSR access is denied on VM-Exit, KVM synthesizes a VM-Abort.  In short, KVM
   extends Intel's architectural list of MSRs that cannot be loaded/saved via
   the VM-Enter/VM-Exit MSR list.  It is platform owner's responsibility to
   to communicate any such restrictions to their end users.
   (通过 VMX 装载/保存列表装载/保存的 MSR，在 VM-Enter/VM-Exit 模拟中则会被过滤。如果 VM-Enter 时有 MSR 访问被拒绝，KVM 会合成一个一致性检查 VM-Exit (EXIT_REASON_MSR_LOAD_FAIL)。如果 VM-Exit 时有 MSR 访问被拒绝，KVM 会合成一个 VM-Abort。简言之，KVM 扩展了 Intel 架构上"不能通过 VM-Enter/VM-Exit MSR 列表装载/保存"的 MSR 列表。平台所有者有责任把这些限制告知其最终用户。)

   x2APIC MSR accesses cannot be filtered (KVM silently ignores filters that
   cover any x2APIC MSRs).
   (x2APIC MSR 访问无法被过滤(KVM 会静默忽略覆盖任何 x2APIC MSR 的过滤器)。)

Note, invoking this ioctl while a vCPU is running is inherently racy.  However,
KVM does guarantee that vCPUs will see either the previous filter or the new
filter, e.g. MSRs with identical settings in both the old and new filter will
have deterministic behavior.
(注意，在 vCPU 正在运行时调用此 ioctl 本质上是有竞态的。不过，KVM 确实保证 vCPU 会看到旧过滤器或新过滤器之一，例如在旧、新过滤器中设置相同的 MSR 将具有确定的行为。)

Similarly, if userspace wishes to intercept on denied accesses,
KVM_MSR_EXIT_REASON_FILTER must be enabled before activating any filters, and
left enabled until after all filters are deactivated.  Failure to do so may
result in KVM injecting a #GP instead of exiting to userspace.
(类似地，如果用户态希望在被拒绝的访问上进行拦截，必须在激活任何过滤器之前启用 KVM_MSR_EXIT_REASON_FILTER，并保持启用直到所有过滤器都停用之后。否则可能导致 KVM 注入 #GP 而不是退到用户态。)

4.98 KVM_CREATE_SPAPR_TCE_64(4.98 KVM_CREATE_SPAPR_TCE_64)
----------------------------------------------------------

:Capability: KVM_CAP_SPAPR_TCE_64
  (能力：KVM_CAP_SPAPR_TCE_64)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_create_spapr_tce_64 (in)
  (参数：struct kvm_create_spapr_tce_64 (in))
:Returns: file descriptor for manipulating the created TCE table
  (返回值：用于操作所创建 TCE 表的文件描述符)

This is an extension for KVM_CAP_SPAPR_TCE which only supports 32bit
windows, described in 4.62 KVM_CREATE_SPAPR_TCE
(这是对 KVM_CAP_SPAPR_TCE 的扩展，后者只支持 32 位窗口，见 4.62 KVM_CREATE_SPAPR_TCE 一节)

This capability uses extended struct in ioctl interface::

  /* for KVM_CAP_SPAPR_TCE_64 */
  struct kvm_create_spapr_tce_64 {
	__u64 liobn;
	__u32 page_shift;
	__u32 flags;
	__u64 offset;	/* in pages */
	__u64 size; 	/* in pages */
  };

The aim of extension is to support an additional bigger DMA window with
a variable page size.
KVM_CREATE_SPAPR_TCE_64 receives a 64bit window size, an IOMMU page shift and
a bus offset of the corresponding DMA window, @size and @offset are numbers
of IOMMU pages.
(扩展的目标是支持一个额外的、更大的、页大小可变的 DMA 窗口。KVM_CREATE_SPAPR_TCE_64 接收一个 64 位窗口大小、一个 IOMMU 页移位以及相应 DMA 窗口的总线偏移，@size 和 @offset 是 IOMMU 页数。)

@flags are not used at the moment.
(@flags 目前未使用。)

The rest of functionality is identical to KVM_CREATE_SPAPR_TCE.
(其余功能与 KVM_CREATE_SPAPR_TCE 相同。)

4.99 KVM_REINJECT_CONTROL(4.99 KVM_REINJECT_CONTROL)
----------------------------------------------------

:Capability: KVM_CAP_REINJECT_CONTROL
  (能力：KVM_CAP_REINJECT_CONTROL)
:Architectures: x86
  (架构：x86)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_reinject_control (in)
  (参数：struct kvm_reinject_control (in))
:Returns: 0 on success,
         -EFAULT if struct kvm_reinject_control cannot be read,
         -ENXIO if KVM_CREATE_PIT or KVM_CREATE_PIT2 didn't succeed earlier.
  (返回值：成功时为 0；无法读取 struct kvm_reinject_control 时为 -EFAULT；如果之前 KVM_CREATE_PIT 或 KVM_CREATE_PIT2 未成功，则为 -ENXIO)

i8254 (PIT) has two modes, reinject and !reinject.  The default is reinject,
where KVM queues elapsed i8254 ticks and monitors completion of interrupt from
vector(s) that i8254 injects.  Reinject mode dequeues a tick and injects its
interrupt whenever there isn't a pending interrupt from i8254.
!reinject mode injects an interrupt as soon as a tick arrives.
(i8254 (PIT) 有两种模式：reinject 与 !reinject。默认为 reinject，此时 KVM 会把已流逝的 i8254 tick 排队，并监视来自 i8254 所注入向量的中断的完成情况。reinject 模式下，只要没有来自 i8254 的待决中断，就会让一个 tick 出队并注入其中断。!reinject 模式则在一个 tick 到达时立即注入一个中断。)

::

  struct kvm_reinject_control {
	__u8 pit_reinject;
	__u8 reserved[31];
  };

pit_reinject = 0 (!reinject mode) is recommended, unless running an old
operating system that uses the PIT for timing (e.g. Linux 2.4.x).
(推荐 pit_reinject = 0 (!reinject 模式)，除非运行的是使用 PIT 计时的旧操作系统(例如 Linux 2.4.x)。)

4.100 KVM_PPC_CONFIGURE_V3_MMU(4.100 KVM_PPC_CONFIGURE_V3_MMU)
--------------------------------------------------------------

:Capability: KVM_CAP_PPC_MMU_RADIX or KVM_CAP_PPC_MMU_HASH_V3
  (能力：KVM_CAP_PPC_MMU_RADIX 或 KVM_CAP_PPC_MMU_HASH_V3)
:Architectures: ppc
  (架构：ppc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ppc_mmuv3_cfg (in)
  (参数：struct kvm_ppc_mmuv3_cfg (in))
:Returns: 0 on success,
         -EFAULT if struct kvm_ppc_mmuv3_cfg cannot be read,
         -EINVAL if the configuration is invalid
  (返回值：成功时为 0；无法读取 struct kvm_ppc_mmuv3_cfg 时为 -EFAULT；配置无效时为 -EINVAL)

This ioctl controls whether the guest will use radix or HPT (hashed
page table) translation, and sets the pointer to the process table for
the guest.
(此 ioctl 控制客户机将使用 radix 还是 HPT (hashed page table) 转换，并设置客户机的进程表指针。)

::

  struct kvm_ppc_mmuv3_cfg {
	__u64	flags;
	__u64	process_table;
  };

There are two bits that can be set in flags; KVM_PPC_MMUV3_RADIX and
KVM_PPC_MMUV3_GTSE.  KVM_PPC_MMUV3_RADIX, if set, configures the guest
to use radix tree translation, and if clear, to use HPT translation.
KVM_PPC_MMUV3_GTSE, if set and if KVM permits it, configures the guest
to be able to use the global TLB and SLB invalidation instructions;
if clear, the guest may not use these instructions.
(flags 中可以设置两个位：KVM_PPC_MMUV3_RADIX 和 KVM_PPC_MMUV3_GTSE。KVM_PPC_MMUV3_RADIX 置位时把客户机配置为使用基数树 (radix tree) 转换，清零时使用 HPT 转换。KVM_PPC_MMUV3_GTSE 置位且 KVM 允许时，把客户机配置为能够使用全局 TLB 与 SLB 失效指令；清零时客户机不得使用这些指令。)

The process_table field specifies the address and size of the guest
process table, which is in the guest's space.  This field is formatted
as the second doubleword of the partition table entry, as defined in
the Power ISA V3.00, Book III section 5.7.6.1.
(process_table 字段指定客户机进程表的地址和大小，它位于客户机地址空间中。该字段的格式为分区表条目的第二个双字，定义见 Power ISA V3.00 Book III 第 5.7.6.1 节。)

4.101 KVM_PPC_GET_RMMU_INFO(4.101 KVM_PPC_GET_RMMU_INFO)
--------------------------------------------------------

:Capability: KVM_CAP_PPC_MMU_RADIX
  (能力：KVM_CAP_PPC_MMU_RADIX)
:Architectures: ppc
  (架构：ppc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ppc_rmmu_info (out)
  (参数：struct kvm_ppc_rmmu_info (out))
:Returns: 0 on success,
	 -EFAULT if struct kvm_ppc_rmmu_info cannot be written,
	 -EINVAL if no useful information can be returned
  (返回值：成功时为 0；无法写入 struct kvm_ppc_rmmu_info 时为 -EFAULT；无法返回有用信息时为 -EINVAL)

This ioctl returns a structure containing two things: (a) a list
containing supported radix tree geometries, and (b) a list that maps
page sizes to put in the "AP" (actual page size) field for the tlbie
(TLB invalidate entry) instruction.
(此 ioctl 返回一个包含两件事的结构：(a) 一个包含受支持基数树几何结构的列表；(b) 一个把页大小映射到 tlbie (TLB invalidate entry) 指令 "AP"(实际页大小)字段的列表。)

::

  struct kvm_ppc_rmmu_info {
	struct kvm_ppc_radix_geom {
		__u8	page_shift;
		__u8	level_bits[4];
		__u8	pad[3];
	}	geometries[8];
	__u32	ap_encodings[8];
  };

The geometries[] field gives up to 8 supported geometries for the
radix page table, in terms of the log base 2 of the smallest page
size, and the number of bits indexed at each level of the tree, from
the PTE level up to the PGD level in that order.  Any unused entries
will have 0 in the page_shift field.
(geometries[] 字段给出 radix 页表最多 8 种受支持的几何结构，以最小页大小以 2 为底的对数以及树的每一层索引的位数表示，从 PTE 层到 PGD 层依次排列。未使用的条目在 page_shift 字段中为 0。)

The ap_encodings gives the supported page sizes and their AP field
encodings, encoded with the AP value in the top 3 bits and the log
base 2 of the page size in the bottom 6 bits.
(ap_encodings 给出受支持的页大小及其 AP 字段编码，编码方式为 AP 值在最高 3 位、页大小以 2 为底的对数在最低 6 位。)

4.102 KVM_PPC_RESIZE_HPT_PREPARE(4.102 KVM_PPC_RESIZE_HPT_PREPARE)
------------------------------------------------------------------

:Capability: KVM_CAP_SPAPR_RESIZE_HPT
  (能力：KVM_CAP_SPAPR_RESIZE_HPT)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ppc_resize_hpt (in)
  (参数：struct kvm_ppc_resize_hpt (in))
:Returns: 0 on successful completion,
	 >0 if a new HPT is being prepared, the value is an estimated
         number of milliseconds until preparation is complete,
         -EFAULT if struct kvm_reinject_control cannot be read,
	 -EINVAL if the supplied shift or flags are invalid,
	 -ENOMEM if unable to allocate the new HPT,
  (返回值：成功完成时为 0；正在准备新 HPT 时 >0，其值为距离准备完成预计的毫秒数；无法读取 struct kvm_reinject_control 时为 -EFAULT；提供的 shift 或 flags 无效时为 -EINVAL；无法分配新 HPT 时为 -ENOMEM)

Used to implement the PAPR extension for runtime resizing of a guest's
Hashed Page Table (HPT).  Specifically this starts, stops or monitors
the preparation of a new potential HPT for the guest, essentially
implementing the H_RESIZE_HPT_PREPARE hypercall.
(用于实现运行时调整客户机哈希页表 (HPT) 大小的 PAPR 扩展。具体而言，它启动、停止或监视为客户机准备一个新潜在 HPT 的过程，本质上实现 H_RESIZE_HPT_PREPARE 超调用。)

::

  struct kvm_ppc_resize_hpt {
	__u64 flags;
	__u32 shift;
	__u32 pad;
  };

If called with shift > 0 when there is no pending HPT for the guest,
this begins preparation of a new pending HPT of size 2^(shift) bytes.
It then returns a positive integer with the estimated number of
milliseconds until preparation is complete.
(在客户机没有待决 HPT 时以 shift > 0 调用，会开始准备一个大小为 2^(shift) 字节的新待决 HPT，然后返回一个正整数，即距离准备完成预计的毫秒数。)

If called when there is a pending HPT whose size does not match that
requested in the parameters, discards the existing pending HPT and
creates a new one as above.
(如果存在大小与参数所请求不一致的待决 HPT，则丢弃现有待决 HPT，并按上述方式创建新的。)

If called when there is a pending HPT of the size requested, will:
(如果存在大小符合所请求的待决 HPT，则会：)

  * If preparation of the pending HPT is already complete, return 0
    (如果待决 HPT 的准备已经完成，返回 0)
  * If preparation of the pending HPT has failed, return an error
    code, then discard the pending HPT.
    (如果待决 HPT 的准备已失败，返回一个错误码，然后丢弃该待决 HPT。)
  * If preparation of the pending HPT is still in progress, return an
    estimated number of milliseconds until preparation is complete.
    (如果待决 HPT 的准备仍在进行中，返回距离准备完成预计的毫秒数。)

If called with shift == 0, discards any currently pending HPT and
returns 0 (i.e. cancels any in-progress preparation).
(以 shift == 0 调用时，丢弃任何当前待决的 HPT 并返回 0(即取消任何进行中的准备)。)

flags is reserved for future expansion, currently setting any bits in
flags will result in an -EINVAL.
(flags 保留供将来扩展；目前在 flags 中设置任何位都会导致 -EINVAL。)

Normally this will be called repeatedly with the same parameters until
it returns <= 0.  The first call will initiate preparation, subsequent
ones will monitor preparation until it completes or fails.
(通常会用相同参数反复调用它，直到返回 <= 0。第一次调用会启动准备，后续调用会监视准备直到完成或失败。)

4.103 KVM_PPC_RESIZE_HPT_COMMIT(4.103 KVM_PPC_RESIZE_HPT_COMMIT)
----------------------------------------------------------------

:Capability: KVM_CAP_SPAPR_RESIZE_HPT
  (能力：KVM_CAP_SPAPR_RESIZE_HPT)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ppc_resize_hpt (in)
  (参数：struct kvm_ppc_resize_hpt (in))
:Returns: 0 on successful completion,
         -EFAULT if struct kvm_reinject_control cannot be read,
	 -EINVAL if the supplied shift or flags are invalid,
	 -ENXIO is there is no pending HPT, or the pending HPT doesn't
         have the requested size,
	 -EBUSY if the pending HPT is not fully prepared,
	 -ENOSPC if there was a hash collision when moving existing
         HPT entries to the new HPT,
	 -EIO on other error conditions
  (返回值：成功完成时为 0；无法读取 struct kvm_reinject_control 时为 -EFAULT；提供的 shift 或 flags 无效时为 -EINVAL；没有待决 HPT 或待决 HPT 大小不符合要求时为 -ENXIO；待决 HPT 尚未完全准备好时为 -EBUSY；把现有 HPT 条目移到新 HPT 时发生哈希冲突时为 -ENOSPC；其他错误情况时为 -EIO)

Used to implement the PAPR extension for runtime resizing of a guest's
Hashed Page Table (HPT).  Specifically this requests that the guest be
transferred to working with the new HPT, essentially implementing the
H_RESIZE_HPT_COMMIT hypercall.
(用于实现运行时调整客户机哈希页表 (HPT) 大小的 PAPR 扩展。具体而言，它请求把客户机转移到使用新 HPT，本质上实现 H_RESIZE_HPT_COMMIT 超调用。)

::

  struct kvm_ppc_resize_hpt {
	__u64 flags;
	__u32 shift;
	__u32 pad;
  };

This should only be called after KVM_PPC_RESIZE_HPT_PREPARE has
returned 0 with the same parameters.  In other cases
KVM_PPC_RESIZE_HPT_COMMIT will return an error (usually -ENXIO or
-EBUSY, though others may be possible if the preparation was started,
but failed).
(只应在 KVM_PPC_RESIZE_HPT_PREPARE 以相同参数返回 0 之后调用。其他情况下 KVM_PPC_RESIZE_HPT_COMMIT 会返回错误(通常是 -ENXIO 或 -EBUSY，但如果准备已启动却失败了，也可能出现其他错误)。)

This will have undefined effects on the guest if it has not already
placed itself in a quiescent state where no vcpu will make MMU enabled
memory accesses.
(如果客户机尚未让自己进入静止状态——即任何 vcpu 都不会进行启用 MMU 的内存访问——此调用对客户机的影响是未定义的。)

On successful completion, the pending HPT will become the guest's active
HPT and the previous HPT will be discarded.
(成功完成后，待决 HPT 将成为客户机的活动 HPT，先前的 HPT 会被丢弃。)

On failure, the guest will still be operating on its previous HPT.
(失败时，客户机仍将使用其先前的 HPT 运行。)

4.104 KVM_X86_GET_MCE_CAP_SUPPORTED(4.104 KVM_X86_GET_MCE_CAP_SUPPORTED)
------------------------------------------------------------------------

:Capability: KVM_CAP_MCE
  (能力：KVM_CAP_MCE)
:Architectures: x86
  (架构：x86)
:Type: system ioctl
  (类型：system ioctl)
:Parameters: u64 mce_cap (out)
  (参数：u64 mce_cap (out))
:Returns: 0 on success, -1 on error
  (返回值：成功时为 0，出错时为 -1)

Returns supported MCE capabilities. The u64 mce_cap parameter
has the same format as the MSR_IA32_MCG_CAP register. Supported
capabilities will have the corresponding bits set.
(返回受支持的 MCE 能力。u64 mce_cap 参数的格式与 MSR_IA32_MCG_CAP 寄存器相同。受支持的能力会置位相应的位。)

4.105 KVM_X86_SETUP_MCE(4.105 KVM_X86_SETUP_MCE)
------------------------------------------------

:Capability: KVM_CAP_MCE
  (能力：KVM_CAP_MCE)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: u64 mcg_cap (in)
  (参数：u64 mcg_cap (in))
:Returns: 0 on success,
         -EFAULT if u64 mcg_cap cannot be read,
         -EINVAL if the requested number of banks is invalid,
         -EINVAL if requested MCE capability is not supported.
  (返回值：成功时为 0；无法读取 u64 mcg_cap 时为 -EFAULT；请求的 bank 数量无效时为 -EINVAL；请求的 MCE 能力不受支持时为 -EINVAL)

Initializes MCE support for use. The u64 mcg_cap parameter
has the same format as the MSR_IA32_MCG_CAP register and
specifies which capabilities should be enabled. The maximum
supported number of error-reporting banks can be retrieved when
checking for KVM_CAP_MCE. The supported capabilities can be
retrieved with KVM_X86_GET_MCE_CAP_SUPPORTED.
(初始化要使用的 MCE 支持。u64 mcg_cap 参数的格式与 MSR_IA32_MCG_CAP 寄存器相同，并指明应启用哪些能力。查询 KVM_CAP_MCE 时可以获得受支持的最大错误报告 bank 数量。受支持的能力可通过 KVM_X86_GET_MCE_CAP_SUPPORTED 获得。)

4.106 KVM_X86_SET_MCE(4.106 KVM_X86_SET_MCE)
--------------------------------------------

:Capability: KVM_CAP_MCE
  (能力：KVM_CAP_MCE)
:Architectures: x86
  (架构：x86)
:Type: vcpu ioctl
  (类型：vcpu ioctl)
:Parameters: struct kvm_x86_mce (in)
  (参数：struct kvm_x86_mce (in))
:Returns: 0 on success,
         -EFAULT if struct kvm_x86_mce cannot be read,
         -EINVAL if the bank number is invalid,
         -EINVAL if VAL bit is not set in status field.
  (返回值：成功时为 0；无法读取 struct kvm_x86_mce 时为 -EFAULT；bank 编号无效时为 -EINVAL；status 字段中未设置 VAL 位时为 -EINVAL)

Inject a machine check error (MCE) into the guest. The input
parameter is::

  struct kvm_x86_mce {
	__u64 status;
	__u64 addr;
	__u64 misc;
	__u64 mcg_status;
	__u8 bank;
	__u8 pad1[7];
	__u64 pad2[3];
  };

If the MCE being reported is an uncorrected error, KVM will
inject it as an MCE exception into the guest. If the guest
MCG_STATUS register reports that an MCE is in progress, KVM
causes an KVM_EXIT_SHUTDOWN vmexit.
(如果报告的 MCE 是未纠正的错误，KVM 会把它作为 MCE 异常注入客户机。如果客户机 MCG_STATUS 寄存器报告 MCE 正在进行中，KVM 会引发一个 KVM_EXIT_SHUTDOWN vmexit。)

Otherwise, if the MCE is a corrected error, KVM will just
store it in the corresponding bank (provided this bank is
not holding a previously reported uncorrected error).
(否则，如果该 MCE 是已纠正的错误，KVM 只会把它存入相应的 bank(前提是该 bank 没有保存先前报告的未纠正错误)。)

4.107 KVM_S390_GET_CMMA_BITS(4.107 KVM_S390_GET_CMMA_BITS)
----------------------------------------------------------

:Capability: KVM_CAP_S390_CMMA_MIGRATION
  (能力：KVM_CAP_S390_CMMA_MIGRATION)
:Architectures: s390
  (架构：s390)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_s390_cmma_log (in, out)
  (参数：struct kvm_s390_cmma_log (in, out))
:Returns: 0 on success, a negative value on error
  (返回值：成功时为 0，出错时为负值)

Errors:
(错误：)

  ======     =============================================================
  ENOMEM     not enough memory can be allocated to complete the task
             (无法分配足够的内存来完成任务)
  ENXIO      if CMMA is not enabled
             (如果 CMMA 未启用)
  EINVAL     if KVM_S390_CMMA_PEEK is not set but migration mode was not enabled
             (如果未设置 KVM_S390_CMMA_PEEK 但未启用迁移模式)
  EINVAL     if KVM_S390_CMMA_PEEK is not set but dirty tracking has been
             disabled (and thus migration mode was automatically disabled)
             (如果未设置 KVM_S390_CMMA_PEEK 但脏页跟踪已被禁用(因此迁移模式被自动禁用))
  EFAULT     if the userspace address is invalid or if no page table is
             present for the addresses (e.g. when using hugepages).
             (如果用户态地址无效，或这些地址没有页表(例如使用大页时)。)
  ======     =============================================================

This ioctl is used to get the values of the CMMA bits on the s390
architecture. It is meant to be used in two scenarios:
(此 ioctl 用于在 s390 架构上获取 CMMA 位的值。它用于两种场景：)

- During live migration to save the CMMA values. Live migration needs
  to be enabled via the KVM_REQ_START_MIGRATION VM property.
  (在实时迁移期间保存 CMMA 值。实时迁移需要通过 KVM_REQ_START_MIGRATION VM 属性启用。)
- To non-destructively peek at the CMMA values, with the flag
  KVM_S390_CMMA_PEEK set.
  (以非破坏性方式窥视 CMMA 值，需设置标志 KVM_S390_CMMA_PEEK。)

The ioctl takes parameters via the kvm_s390_cmma_log struct. The desired
values are written to a buffer whose location is indicated via the "values"
member in the kvm_s390_cmma_log struct.  The values in the input struct are
also updated as needed.
(ioctl 通过 kvm_s390_cmma_log 结构取得参数。所需的值会被写入一个缓冲区，缓冲区位置由 kvm_s390_cmma_log 结构的 "values" 成员指明。输入结构中的值也会按需更新。)

Each CMMA value takes up one byte.
(每个 CMMA 值占一个字节。)

::

  struct kvm_s390_cmma_log {
	__u64 start_gfn;
	__u32 count;
	__u32 flags;
	union {
		__u64 remaining;
		__u64 mask;
	};
	__u64 values;
  };

start_gfn is the number of the first guest frame whose CMMA values are
to be retrieved,
(start_gfn 是要检索其 CMMA 值的第一个客户机帧号，)

count is the length of the buffer in bytes,
(count 是以字节为单位的缓冲区长度，)

values points to the buffer where the result will be written to.
(values 指向结果将要写入的缓冲区。)

If count is greater than KVM_S390_SKEYS_MAX, then it is considered to be
KVM_S390_SKEYS_MAX. KVM_S390_SKEYS_MAX is re-used for consistency with
other ioctls.
(如果 count 大于 KVM_S390_SKEYS_MAX，则按 KVM_S390_SKEYS_MAX 处理。为与其他 ioctl 保持一致而复用 KVM_S390_SKEYS_MAX。)

The result is written in the buffer pointed to by the field values, and
the values of the input parameter are updated as follows.
(结果写入 values 字段所指向的缓冲区，输入参数的值按如下方式更新。)

Depending on the flags, different actions are performed. The only
supported flag so far is KVM_S390_CMMA_PEEK.
(根据 flags 的不同会执行不同的操作。到目前为止唯一受支持的标志是 KVM_S390_CMMA_PEEK。)

The default behaviour if KVM_S390_CMMA_PEEK is not set is:
start_gfn will indicate the first page frame whose CMMA bits were dirty.
It is not necessarily the same as the one passed as input, as clean pages
are skipped.
(未设置 KVM_S390_CMMA_PEEK 时的默认行为是：start_gfn 将指示 CMMA 位为脏的第一个页帧。它不一定与作为输入传入的那个相同，因为干净的页会被跳过。)

count will indicate the number of bytes actually written in the buffer.
It can (and very often will) be smaller than the input value, since the
buffer is only filled until 16 bytes of clean values are found (which
are then not copied in the buffer). Since a CMMA migration block needs
the base address and the length, for a total of 16 bytes, we will send
back some clean data if there is some dirty data afterwards, as long as
the size of the clean data does not exceed the size of the header. This
allows to minimize the amount of data to be saved or transferred over
the network at the expense of more roundtrips to userspace. The next
invocation of the ioctl will skip over all the clean values, saving
potentially more than just the 16 bytes we found.
(count 将指示实际写入缓冲区的字节数。它可能(而且经常会)小于输入值，因为缓冲区只会填充到发现 16 字节干净值为止(这些干净值不会复制进缓冲区)。由于一个 CMMA 迁移块需要基地址和长度共 16 字节，只要干净数据的大小不超过头部大小，若其后还有脏数据，我们会回传一些干净数据。这样可以以更多次与用户态的往返为代价，把需要保存或经网络传输的数据量降到最少。ioctl 的下一次调用会跳过所有干净值，从而可能节省的不止我们找到的那 16 字节。)

If KVM_S390_CMMA_PEEK is set:
the existing storage attributes are read even when not in migration
mode, and no other action is performed;
(如果设置了 KVM_S390_CMMA_PEEK：即使在迁移模式之外也会读取现有存储属性，且不执行任何其他操作；)

the output start_gfn will be equal to the input start_gfn,
(输出的 start_gfn 将等于输入的 start_gfn，)

the output count will be equal to the input count, except if the end of
memory has been reached.
(输出的 count 将等于输入的 count，除非已到达内存末尾。)

In both cases:
the field "remaining" will indicate the total number of dirty CMMA values
still remaining, or 0 if KVM_S390_CMMA_PEEK is set and migration mode is
not enabled.
(两种情况下：字段 "remaining" 将指示仍然剩余的脏 CMMA 值总数；如果设置了 KVM_S390_CMMA_PEEK 且未启用迁移模式，则为 0。)

mask is unused.
(mask 未使用。)

values points to the userspace buffer where the result will be stored.
(values 指向将存储结果的用户态缓冲区。)

4.108 KVM_S390_SET_CMMA_BITS(4.108 KVM_S390_SET_CMMA_BITS)
----------------------------------------------------------

:Capability: KVM_CAP_S390_CMMA_MIGRATION
  (能力：KVM_CAP_S390_CMMA_MIGRATION)
:Architectures: s390
  (架构：s390)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_s390_cmma_log (in)
  (参数：struct kvm_s390_cmma_log (in))
:Returns: 0 on success, a negative value on error
  (返回值：成功时为 0，出错时为负值)

This ioctl is used to set the values of the CMMA bits on the s390
architecture. It is meant to be used during live migration to restore
the CMMA values, but there are no restrictions on its use.
The ioctl takes parameters via the kvm_s390_cmma_values struct.
Each CMMA value takes up one byte.
(此 ioctl 用于在 s390 架构上设置 CMMA 位的值。它应在实时迁移期间用于恢复 CMMA 值，但其使用没有任何限制。ioctl 通过 kvm_s390_cmma_values 结构取得参数。每个 CMMA 值占一个字节。)

::

  struct kvm_s390_cmma_log {
	__u64 start_gfn;
	__u32 count;
	__u32 flags;
	union {
		__u64 remaining;
		__u64 mask;
 	};
	__u64 values;
  };

start_gfn indicates the starting guest frame number,
(start_gfn 指示起始客户机帧号，)

count indicates how many values are to be considered in the buffer,
(count 指示要考虑缓冲区中的多少个值，)

flags is not used and must be 0.
(flags 未使用，必须为 0。)

mask indicates which PGSTE bits are to be considered.
(mask 指示要考虑哪些 PGSTE 位。)

remaining is not used.
(remaining 未使用。)

values points to the buffer in userspace where to store the values.
(values 指向用户态中存储这些值的缓冲区。)

This ioctl can fail with -ENOMEM if not enough memory can be allocated to
complete the task, with -ENXIO if CMMA is not enabled, with -EINVAL if
the count field is too large (e.g. more than KVM_S390_CMMA_SIZE_MAX) or
if the flags field was not 0, with -EFAULT if the userspace address is
invalid, if invalid pages are written to (e.g. after the end of memory)
or if no page table is present for the addresses (e.g. when using
hugepages).
(如果无法分配足够的内存来完成任务，此 ioctl 会以 -ENOMEM 失败；如果 CMMA 未启用，会以 -ENXIO 失败；如果 count 字段过大(例如超过 KVM_S390_CMMA_SIZE_MAX)或 flags 字段不为 0，会以 -EINVAL 失败；如果用户态地址无效、写入了无效页(例如内存末尾之后)或这些地址没有页表(例如使用大页时)，会以 -EFAULT 失败。)

4.109 KVM_PPC_GET_CPU_CHAR(4.109 KVM_PPC_GET_CPU_CHAR)
------------------------------------------------------

:Capability: KVM_CAP_PPC_GET_CPU_CHAR
  (能力：KVM_CAP_PPC_GET_CPU_CHAR)
:Architectures: powerpc
  (架构：powerpc)
:Type: vm ioctl
  (类型：vm ioctl)
:Parameters: struct kvm_ppc_cpu_char (out)
  (参数：struct kvm_ppc_cpu_char (out))
:Returns: 0 on successful completion,
	 -EFAULT if struct kvm_ppc_cpu_char cannot be written
  (返回值：成功完成时为 0；无法写入 struct kvm_ppc_cpu_char 时为 -EFAULT)

This ioctl gives userspace information about certain characteristics
of the CPU relating to speculative execution of instructions and
possible information leakage resulting from speculative execution (see
CVE-2017-5715, CVE-2017-5753 and CVE-2017-5754).  The information is
returned in struct kvm_ppc_cpu_char, which looks like this::

  struct kvm_ppc_cpu_char {
	__u64	character;		/* characteristics of the CPU */
	__u64	behaviour;		/* recommended software behaviour */
	__u64	character_mask;		/* valid bits in character */
	__u64	behaviour_mask;		/* valid bits in behaviour */
  };

For extensibility, the character_mask and behaviour_mask fields
indicate which bits of character and behaviour have been filled in by
the kernel.  If the set of defined bits is extended in future then
userspace will be able to tell whether it is running on a kernel that
knows about the new bits.
(为了可扩展性，character_mask 和 behaviour_mask 字段指明 character 和 behaviour 中哪些位已由内核填充。如果将来定义的位集合被扩展，用户态就能判断自己是否运行在认识这些新位的内核上。)

The character field describes attributes of the CPU which can help
with preventing inadvertent information disclosure - specifically,
whether there is an instruction to flash-invalidate the L1 data cache
(ori 30,30,0 or mtspr SPRN_TRIG2,rN), whether the L1 data cache is set
to a mode where entries can only be used by the thread that created
them, whether the bcctr[l] instruction prevents speculation, and
whether a speculation barrier instruction (ori 31,31,0) is provided.
(character 字段描述有助于防止无意信息泄露的 CPU 属性——具体包括：是否存在闪式失效 L1 数据缓存的指令 (ori 30,30,0 或 mtspr SPRN_TRIG2,rN)；L1 数据缓存是否被设置为条目只能由创建它们的线程使用的模式；bcctr[l] 指令是否阻止推测执行；以及是否提供推测屏障指令 (ori 31,31,0)。)

The behaviour field describes actions that software should take to
prevent inadvertent information disclosure, and thus describes which
vulnerabilities the hardware is subject to; specifically whether the
L1 data cache should be flushed when returning to user mode from the
kernel, and whether a speculation barrier should be placed between an
array bounds check and the array access.
(behaviour 字段描述软件应采取的防止无意信息泄露的措施，从而描述硬件受哪些漏洞影响；具体包括：从内核返回用户态时是否应刷新 L1 数据缓存，以及是否应在数组边界检查与数组访问之间放置推测屏障。)

These fields use the same bit definitions as the new
H_GET_CPU_CHARACTERISTICS hypercall.
(这些字段使用与新 H_GET_CPU_CHARACTERISTICS 超调用相同的位定义。)

4.110 KVM_MEMORY_ENCRYPT_OP(4.110 KVM_MEMORY_ENCRYPT_OP)
--------------------------------------------------------

:Capability: basic
:Architectures: x86
:Type: vm ioctl, vcpu ioctl
:Parameters: an opaque platform specific structure (in/out)
:Returns: 0 on success; -1 on error

(能力：basic；架构：x86；类型：vm ioctl, vcpu ioctl；参数：一个不透明的平台相关结构
(in/out)；返回值：成功时为 0；出错时为 -1。)

If the platform supports creating encrypted VMs then this ioctl can be used
for issuing platform-specific memory encryption commands to manage those
encrypted VMs.
(若平台支持创建加密 VM，则此 ioctl 可用于下发平台特定的内存加密命令，以管理这些加密
VM。)

Currently, this ioctl is used for issuing both Secure Encrypted Virtualization
(SEV) commands on AMD Processors and Trusted Domain Extensions (TDX) commands
on Intel Processors.  The detailed commands are defined in
Documentation/virt/kvm/x86/amd-memory-encryption.rst and
Documentation/virt/kvm/x86/intel-tdx.rst.
(目前，此 ioctl 既用于在 AMD 处理器上下发安全加密虚拟化 (SEV) 命令，也用于在 Intel
处理器上下发可信域扩展 (TDX) 命令。详细命令定义于
Documentation/virt/kvm/x86/amd-memory-encryption.rst 与
Documentation/virt/kvm/x86/intel-tdx.rst。)

4.111 KVM_MEMORY_ENCRYPT_REG_REGION
-----------------------------------

:Capability: basic
:Architectures: x86
:Type: system
:Parameters: struct kvm_enc_region (in)
:Returns: 0 on success; -1 on error

(能力：basic；架构：x86；类型：system；参数：struct kvm_enc_region (in)；返回值：
成功时为 0；出错时为 -1。)

This ioctl can be used to register a guest memory region which may
contain encrypted data (e.g. guest RAM, SMRAM etc).
(此 ioctl 可用于注册一个可能包含加密数据的客户机内存区域（例如客户机 RAM、SMRAM 等）。)

It is used in the SEV-enabled guest. When encryption is enabled, a guest
memory region may contain encrypted data. The SEV memory encryption
engine uses a tweak such that two identical plaintext pages, each at
different locations will have differing ciphertexts. So swapping or
moving ciphertext of those pages will not result in plaintext being
swapped. So relocating (or migrating) physical backing pages for the SEV
guest will require some additional steps.
(它用于启用了 SEV 的客户机。启用加密后，客户机内存区域可能包含加密数据。SEV 内存加
密引擎使用一个 tweak（微调值），使得位于不同位置的两个相同明文页会得到不同的密文。
因此，交换或移动这些页的密文并不会导致明文被交换。所以，为 SEV 客户机重定位（或迁
移）物理后备页需要一些额外的步骤。)

Note: The current SEV key management spec does not provide commands to
swap or migrate (move) ciphertext pages. Hence, for now we pin the guest
memory region registered with the ioctl.
(注意：当前的 SEV 密钥管理规范没有提供交换或迁移（移动）密文页的命令。因此，目前我
们会钉住（pin）通过该 ioctl 注册的客户机内存区域。)

4.112 KVM_MEMORY_ENCRYPT_UNREG_REGION
-------------------------------------

:Capability: basic
:Architectures: x86
:Type: system
:Parameters: struct kvm_enc_region (in)
:Returns: 0 on success; -1 on error

(能力：basic；架构：x86；类型：system；参数：struct kvm_enc_region (in)；返回值：
成功时为 0；出错时为 -1。)

This ioctl can be used to unregister the guest memory region registered
with KVM_MEMORY_ENCRYPT_REG_REGION ioctl above.
(此 ioctl 可用于注销前面通过 KVM_MEMORY_ENCRYPT_REG_REGION ioctl 注册的客户机内存
区域。)

4.113 KVM_HYPERV_EVENTFD
------------------------

:Capability: KVM_CAP_HYPERV_EVENTFD
:Architectures: x86
:Type: vm ioctl
:Parameters: struct kvm_hyperv_eventfd (in)

(能力：KVM_CAP_HYPERV_EVENTFD；架构：x86；类型：vm ioctl；参数：struct
kvm_hyperv_eventfd (in)。)

This ioctl (un)registers an eventfd to receive notifications from the guest on
the specified Hyper-V connection id through the SIGNAL_EVENT hypercall, without
causing a user exit.  SIGNAL_EVENT hypercall with non-zero event flag number
(bits 24-31) still triggers a KVM_EXIT_HYPERV_HCALL user exit.
(此 ioctl 注册/注销一个 eventfd，用于通过 SIGNAL_EVENT 超级调用接收来自客户机在指定
Hyper-V 连接 id 上的通知，而不会引发用户态退出。带非零事件标志编号（位 24-31）的
SIGNAL_EVENT 超级调用仍会触发 KVM_EXIT_HYPERV_HCALL 用户态退出。)

::

  struct kvm_hyperv_eventfd {
	__u32 conn_id;
	__s32 fd;
	__u32 flags;
	__u32 padding[3];
  };

The conn_id field should fit within 24 bits::
(conn_id 字段应当能容纳在 24 位之内：)

  #define KVM_HYPERV_CONN_ID_MASK		0x00ffffff

The acceptable values for the flags field are::
(flags 字段可接受的取值：)

  #define KVM_HYPERV_EVENTFD_DEASSIGN	(1 << 0)

:Returns: 0 on success,
 	  -EINVAL if conn_id or flags is outside the allowed range,
	  -ENOENT on deassign if the conn_id isn't registered,
	  -EEXIST on assign if the conn_id is already registered

(返回值：成功时为 0；conn_id 或 flags 超出允许范围时为 -EINVAL；解除绑定时若 conn_id
未注册为 -ENOENT；绑定时若 conn_id 已注册为 -EEXIST。)

4.114 KVM_GET_NESTED_STATE
--------------------------

:Capability: KVM_CAP_NESTED_STATE
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_nested_state (in/out)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_NESTED_STATE；架构：x86；类型：vcpu ioctl；参数：struct
kvm_nested_state (in/out)；返回值：成功时为 0，出错时为 -1。)

Errors:
(错误：)

  =====      =============================================================
  E2BIG      the total state size exceeds the value of 'size' specified by
             the user; the size required will be written into size.
             (状态总大小超过了用户指定的 'size' 值；所需大小会写入 size。)
  =====      =============================================================

::

  struct kvm_nested_state {
	__u16 flags;
	__u16 format;
	__u32 size;

	union {
		struct kvm_vmx_nested_state_hdr vmx;
		struct kvm_svm_nested_state_hdr svm;

		/* Pad the header to 128 bytes.  */
		__u8 pad[120];
	} hdr;

	union {
		struct kvm_vmx_nested_state_data vmx[0];
		struct kvm_svm_nested_state_data svm[0];
	} data;
  };

  #define KVM_STATE_NESTED_GUEST_MODE		0x00000001
  #define KVM_STATE_NESTED_RUN_PENDING		0x00000002
  #define KVM_STATE_NESTED_EVMCS		0x00000004

  #define KVM_STATE_NESTED_FORMAT_VMX		0
  #define KVM_STATE_NESTED_FORMAT_SVM		1

  #define KVM_STATE_NESTED_VMX_VMCS_SIZE	0x1000

  #define KVM_STATE_NESTED_VMX_SMM_GUEST_MODE	0x00000001
  #define KVM_STATE_NESTED_VMX_SMM_VMXON	0x00000002

  #define KVM_STATE_VMX_PREEMPTION_TIMER_DEADLINE 0x00000001

  struct kvm_vmx_nested_state_hdr {
	__u64 vmxon_pa;
	__u64 vmcs12_pa;

	struct {
		__u16 flags;
	} smm;

	__u32 flags;
	__u64 preemption_timer_deadline;
  };

  struct kvm_vmx_nested_state_data {
	__u8 vmcs12[KVM_STATE_NESTED_VMX_VMCS_SIZE];
	__u8 shadow_vmcs12[KVM_STATE_NESTED_VMX_VMCS_SIZE];
  };

This ioctl copies the vcpu's nested virtualization state from the kernel to
userspace.
(此 ioctl 将 vCPU 的嵌套虚拟化状态从内核复制到用户态。)

The maximum size of the state can be retrieved by passing KVM_CAP_NESTED_STATE
to the KVM_CHECK_EXTENSION ioctl().
(向 KVM_CHECK_EXTENSION ioctl() 传入 KVM_CAP_NESTED_STATE 即可获取该状态的最大
大小。)

4.115 KVM_SET_NESTED_STATE
--------------------------

:Capability: KVM_CAP_NESTED_STATE
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_nested_state (in)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_NESTED_STATE；架构：x86；类型：vcpu ioctl；参数：struct
kvm_nested_state (in)；返回值：成功时为 0，出错时为 -1。)

This copies the vcpu's kvm_nested_state struct from userspace to the kernel.
For the definition of struct kvm_nested_state, see KVM_GET_NESTED_STATE.
(它将 vCPU 的 kvm_nested_state 结构体从用户态复制到内核。struct kvm_nested_state
的定义参见 KVM_GET_NESTED_STATE。)

4.116 KVM_(UN)REGISTER_COALESCED_MMIO
-------------------------------------

:Capability: KVM_CAP_COALESCED_MMIO (for coalesced mmio)
	     KVM_CAP_COALESCED_PIO (for coalesced pio)
:Architectures: all
:Type: vm ioctl
:Parameters: struct kvm_coalesced_mmio_zone
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_COALESCED_MMIO（用于合并 mmio）、KVM_CAP_COALESCED_PIO（用于合并
pio）；架构：all；类型：vm ioctl；参数：struct kvm_coalesced_mmio_zone；返回值：
成功时为 0，出错时小于 0。)

Coalesced I/O is a performance optimization that defers hardware
register write emulation so that userspace exits are avoided.  It is
typically used to reduce the overhead of emulating frequently accessed
hardware registers.
(合并 I/O（coalesced I/O）是一种性能优化，它推迟硬件寄存器写操作的模拟，从而避免
退出到用户态。它通常用于降低模拟频繁访问的硬件寄存器的开销。)

When a hardware register is configured for coalesced I/O, write accesses
do not exit to userspace and their value is recorded in a ring buffer
that is shared between kernel and userspace.
(当硬件寄存器被配置为合并 I/O 后，写访问不再退出到用户态，其值被记录在一个内核与
用户态共享的环形缓冲区中。)

Coalesced I/O is used if one or more write accesses to a hardware
register can be deferred until a read or a write to another hardware
register on the same device.  This last access will cause a vmexit and
userspace will process accesses from the ring buffer before emulating
it. That will avoid exiting to userspace on repeated writes.
(当对某个硬件寄存器的一次或多次写访问可以推迟到对同一设备上另一个硬件寄存器的读或
写时，就会使用合并 I/O。最后这一次访问会引发 vmexit，用户态在模拟它之前会先处理来
自环形缓冲区的各次访问。这样可以避免在重复写时多次退出到用户态。)

Coalesced pio is based on coalesced mmio. There is little difference
between coalesced mmio and pio except that coalesced pio records accesses
to I/O ports.
(合并 pio 基于合并 mmio。两者几乎没有区别，只是合并 pio 记录的是对 I/O 端口的
访问。)

4.117 KVM_CLEAR_DIRTY_LOG
-------------------------

:Capability: KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2
:Architectures: x86, arm64, mips
:Type: vm ioctl
:Parameters: struct kvm_clear_dirty_log (in)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2；架构：x86、arm64、mips；类型：vm ioctl；
参数：struct kvm_clear_dirty_log (in)；返回值：成功时为 0，出错时为 -1。)

::

  /* for KVM_CLEAR_DIRTY_LOG */
  struct kvm_clear_dirty_log {
	__u32 slot;
	__u32 num_pages;
	__u64 first_page;
	union {
		void __user *dirty_bitmap; /* one bit per page */
		__u64 padding;
	};
  };

The ioctl clears the dirty status of pages in a memory slot, according to
the bitmap that is passed in struct kvm_clear_dirty_log's dirty_bitmap
field.  Bit 0 of the bitmap corresponds to page "first_page" in the
memory slot, and num_pages is the size in bits of the input bitmap.
first_page must be a multiple of 64; num_pages must also be a multiple of
64 unless first_page + num_pages is the size of the memory slot.  For each
bit that is set in the input bitmap, the corresponding page is marked "clean"
in KVM's dirty bitmap, and dirty tracking is re-enabled for that page
(for example via write-protection, or by clearing the dirty bit in
a page table entry).
(该 ioctl 依据传入 struct kvm_clear_dirty_log 之 dirty_bitmap 字段的位图，清除内
存槽中页的脏状态。位图的第 0 位对应内存槽中的页 "first_page"，num_pages 是输入位图
的位数大小。first_page 必须是 64 的倍数；除非 first_page + num_pages 等于内存槽的
大小，否则 num_pages 也必须是 64 的倍数。对于输入位图中每个置位的位，KVM 会在其脏
位图中将对应页标记为 "clean"，并为该页重新启用脏跟踪（例如通过写保护，或清除页表
项中的脏位）。)

If KVM_CAP_MULTI_ADDRESS_SPACE is available, bits 16-31 of slot field specifies
the address space for which you want to clear the dirty status.  See
KVM_SET_USER_MEMORY_REGION for details on the usage of slot field.
(如果 KVM_CAP_MULTI_ADDRESS_SPACE 可用，则 slot 字段的位 16-31 指定要清除脏状态的
地址空间。slot 字段用法的细节参见 KVM_SET_USER_MEMORY_REGION。)

This ioctl is mostly useful when KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2
is enabled; for more information, see the description of the capability.
However, it can always be used as long as KVM_CHECK_EXTENSION confirms
that KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 is present.
(此 ioctl 主要在启用 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 时有用；更多信息参见该能力
的说明。不过，只要 KVM_CHECK_EXTENSION 确认 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2
存在，它就总是可以使用。)

4.118 KVM_GET_SUPPORTED_HV_CPUID
--------------------------------

:Capability: KVM_CAP_HYPERV_CPUID (vcpu), KVM_CAP_SYS_HYPERV_CPUID (system)
:Architectures: x86
:Type: system ioctl, vcpu ioctl
:Parameters: struct kvm_cpuid2 (in/out)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_HYPERV_CPUID（vcpu）、KVM_CAP_SYS_HYPERV_CPUID（system）；架构：
x86；类型：system ioctl 与 vcpu ioctl；参数：struct kvm_cpuid2 (in/out)；返回值：
成功时为 0，出错时为 -1。)

::

  struct kvm_cpuid2 {
	__u32 nent;
	__u32 padding;
	struct kvm_cpuid_entry2 entries[0];
  };

  struct kvm_cpuid_entry2 {
	__u32 function;
	__u32 index;
	__u32 flags;
	__u32 eax;
	__u32 ebx;
	__u32 ecx;
	__u32 edx;
	__u32 padding[3];
  };

This ioctl returns x86 cpuid features leaves related to Hyper-V emulation in
KVM.  Userspace can use the information returned by this ioctl to construct
cpuid information presented to guests consuming Hyper-V enlightenments (e.g.
Windows or Hyper-V guests).
(此 ioctl 返回 KVM 中与 Hyper-V 模拟相关的 x86 CPUID 特性叶。用户态可以利用此 ioctl
返回的信息，构造呈现给使用 Hyper-V 半虚拟化加速（enlightenment）的客户机（例如
Windows 或 Hyper-V 客户机）的 CPUID 信息。)

CPUID feature leaves returned by this ioctl are defined by Hyper-V Top Level
Functional Specification (TLFS). These leaves can't be obtained with
KVM_GET_SUPPORTED_CPUID ioctl because some of them intersect with KVM feature
leaves (0x40000000, 0x40000001).
(此 ioctl 返回的 CPUID 特性叶由 Hyper-V 顶层功能规范 (TLFS) 定义。这些叶无法通过
KVM_GET_SUPPORTED_CPUID ioctl 获得，因为其中一些与 KVM 特性叶 (0x40000000、
0x40000001) 相交。)

Currently, the following list of CPUID leaves are returned:
(目前返回以下 CPUID 叶列表：)

 - HYPERV_CPUID_VENDOR_AND_MAX_FUNCTIONS
 - HYPERV_CPUID_INTERFACE
 - HYPERV_CPUID_VERSION
 - HYPERV_CPUID_FEATURES
 - HYPERV_CPUID_ENLIGHTMENT_INFO
 - HYPERV_CPUID_IMPLEMENT_LIMITS
 - HYPERV_CPUID_NESTED_FEATURES
 - HYPERV_CPUID_SYNDBG_VENDOR_AND_MAX_FUNCTIONS
 - HYPERV_CPUID_SYNDBG_INTERFACE
 - HYPERV_CPUID_SYNDBG_PLATFORM_CAPABILITIES

Userspace invokes KVM_GET_SUPPORTED_HV_CPUID by passing a kvm_cpuid2 structure
with the 'nent' field indicating the number of entries in the variable-size
array 'entries'.  If the number of entries is too low to describe all Hyper-V
feature leaves, an error (E2BIG) is returned. If the number is more or equal
to the number of Hyper-V feature leaves, the 'nent' field is adjusted to the
number of valid entries in the 'entries' array, which is then filled.
(用户态调用 KVM_GET_SUPPORTED_HV_CPUID 时传入一个 kvm_cpuid2 结构体，其 'nent'
字段指明变长数组 'entries' 中的条目数。如果条目数过少而无法描述全部 Hyper-V 特性
叶，则返回错误 (E2BIG)。如果该数目大于或等于 Hyper-V 特性叶的数量，则 'nent' 字段
会被调整为 'entries' 数组中有效条目的数量，随后填充该数组。)

'index' and 'flags' fields in 'struct kvm_cpuid_entry2' are currently reserved,
userspace should not expect to get any particular value there.
('struct kvm_cpuid_entry2' 中的 'index' 与 'flags' 字段目前是保留的，用户态不应期
望从中得到任何特定值。)

Note, vcpu version of KVM_GET_SUPPORTED_HV_CPUID is currently deprecated. Unlike
system ioctl which exposes all supported feature bits unconditionally, vcpu
version has the following quirks:
(注意：KVM_GET_SUPPORTED_HV_CPUID 的 vcpu 版本目前已废弃。与无条件暴露所有受支持特
性位的 system ioctl 不同，vcpu 版本有以下怪癖：)

- HYPERV_CPUID_NESTED_FEATURES leaf and HV_X64_ENLIGHTENED_VMCS_RECOMMENDED
  feature bit are only exposed when Enlightened VMCS was previously enabled
  on the corresponding vCPU (KVM_CAP_HYPERV_ENLIGHTENED_VMCS).
  (仅当先前在相应 vCPU 上启用了 Enlightened VMCS (KVM_CAP_HYPERV_ENLIGHTENED_VMCS)
  时，才会暴露 HYPERV_CPUID_NESTED_FEATURES 叶与 HV_X64_ENLIGHTENED_VMCS_RECOMMENDED
  特性位。)
- HV_STIMER_DIRECT_MODE_AVAILABLE bit is only exposed with in-kernel LAPIC.
  (presumes KVM_CREATE_IRQCHIP has already been called).
  (仅在使用内核内 LAPIC 时才暴露 HV_STIMER_DIRECT_MODE_AVAILABLE 位（前提是已经调
  用过 KVM_CREATE_IRQCHIP）。)

4.119 KVM_ARM_VCPU_FINALIZE
---------------------------

:Architectures: arm64
:Type: vcpu ioctl
:Parameters: int feature (in)
:Returns: 0 on success, -1 on error

(架构：arm64；类型：vcpu ioctl；参数：int feature (in)；返回值：成功时为 0，出错时
为 -1。)

Errors:
(错误：)

  ======     ==============================================================
  EPERM      feature not enabled, needs configuration, or already finalized
             (特性未启用、需要配置，或已经终结)
  EINVAL     feature unknown or not present
             (特性未知或不存在)
  ======     ==============================================================

Recognised values for feature:
(feature 的可识别取值：)

  =====      ===========================================
  arm64      KVM_ARM_VCPU_SVE (requires KVM_CAP_ARM_SVE)
             (KVM_ARM_VCPU_SVE（需要 KVM_CAP_ARM_SVE）)
  =====      ===========================================

Finalizes the configuration of the specified vcpu feature.
(终结指定 vCPU 特性的配置。)

The vcpu must already have been initialised, enabling the affected feature, by
means of a successful :ref:`KVM_ARM_VCPU_INIT <KVM_ARM_VCPU_INIT>` call with the
appropriate flag set in features[].
(该 vCPU 必须已经通过一次成功的 :ref:`KVM_ARM_VCPU_INIT <KVM_ARM_VCPU_INIT>` 调用
完成初始化，并在 features[] 中设置了相应标志以启用受影响的特性。)

For affected vcpu features, this is a mandatory step that must be performed
before the vcpu is fully usable.
(对于受影响的 vCPU 特性而言，这是在 vCPU 完全可用之前必须执行的一个强制步骤。)

Between KVM_ARM_VCPU_INIT and KVM_ARM_VCPU_FINALIZE, the feature may be
configured by use of ioctls such as KVM_SET_ONE_REG.  The exact configuration
that should be performed and how to do it are feature-dependent.
(在 KVM_ARM_VCPU_INIT 与 KVM_ARM_VCPU_FINALIZE 之间，可以使用诸如 KVM_SET_ONE_REG
之类的 ioctl 来配置该特性。应当执行的确切配置及其方式取决于具体特性。)

Other calls that depend on a particular feature being finalized, such as
KVM_RUN, KVM_GET_REG_LIST, KVM_GET_ONE_REG and KVM_SET_ONE_REG, will fail with
-EPERM unless the feature has already been finalized by means of a
KVM_ARM_VCPU_FINALIZE call.
(依赖于某个特性已终结的其他调用，例如 KVM_RUN、KVM_GET_REG_LIST、KVM_GET_ONE_REG
与 KVM_SET_ONE_REG，若该特性尚未通过 KVM_ARM_VCPU_FINALIZE 调用终结，则会以 -EPERM
失败。)

See KVM_ARM_VCPU_INIT for details of vcpu features that require finalization
using this ioctl.
(需要使用此 ioctl 终结的 vCPU 特性的详情参见 KVM_ARM_VCPU_INIT。)

4.120 KVM_SET_PMU_EVENT_FILTER
------------------------------

:Capability: KVM_CAP_PMU_EVENT_FILTER
:Architectures: x86
:Type: vm ioctl
:Parameters: struct kvm_pmu_event_filter (in)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_PMU_EVENT_FILTER；架构：x86；类型：vm ioctl；参数：struct
kvm_pmu_event_filter (in)；返回值：成功时为 0，出错时为 -1。)

Errors:
(错误：)

  ======     ============================================================
  EFAULT     args[0] cannot be accessed
             (无法访问 args[0])
  EINVAL     args[0] contains invalid data in the filter or filter events
             (args[0] 中的 filter 或过滤事件包含无效数据)
  E2BIG      nevents is too large
             (nevents 过大)
  EBUSY      not enough memory to allocate the filter
             (内存不足以分配过滤器)
  ======     ============================================================

::

  struct kvm_pmu_event_filter {
	__u32 action;
	__u32 nevents;
	__u32 fixed_counter_bitmap;
	__u32 flags;
	__u32 pad[4];
	__u64 events[0];
  };

This ioctl restricts the set of PMU events the guest can program by limiting
which event select and unit mask combinations are permitted.
(此 ioctl 通过限制允许的事件选择 (event select) 与单元掩码 (unit mask) 组合，来约
束客户机可以编程的 PMU 事件集合。)

The argument holds a list of filter events which will be allowed or denied.
(参数持有一个将被允许或拒绝的过滤事件列表。)

Filter events only control general purpose counters; fixed purpose counters
are controlled by the fixed_counter_bitmap.
(过滤事件只控制通用计数器；固定用途计数器则由 fixed_counter_bitmap 控制。)

Valid values for 'flags'::

(flags 的有效取值：)

``0``

To use this mode, clear the 'flags' field.
(要使用此模式，请将 'flags' 字段清零。)

In this mode each event will contain an event select + unit mask.
(在此模式下，每个事件将包含一个事件选择 + 单元掩码。)

When the guest attempts to program the PMU the guest's event select +
unit mask is compared against the filter events to determine whether the
guest should have access.
(当客户机尝试对 PMU 编程时，会将客户机的事件选择 + 单元掩码与过滤事件进行比较，
以决定客户机是否拥有访问权限。)

``KVM_PMU_EVENT_FLAG_MASKED_EVENTS``
(KVM_PMU_EVENT_FLAG_MASKED_EVENTS(掩码事件))

:Capability: KVM_CAP_PMU_EVENT_MASKED_EVENTS

In this mode each filter event will contain an event select, mask, match, and
exclude value.  To encode a masked event use::
(在此模式下，每个过滤事件将包含一个事件选择、掩码 (mask)、匹配值 (match) 与排除
位 (exclude)。要编码一个掩码事件，使用：)

  KVM_PMU_ENCODE_MASKED_ENTRY()

An encoded event will follow this layout::
(编码后的事件遵循如下布局：)

  Bits   Description
  ----   -----------
  7:0    event select (low bits)
  15:8   umask match
  31:16  unused
  35:32  event select (high bits)
  36:54  unused
  55     exclude bit
  63:56  umask mask

When the guest attempts to program the PMU, these steps are followed in
determining if the guest should have access:
(当客户机尝试对 PMU 编程时，按以下步骤判断客户机是否拥有访问权限：)

 1. Match the event select from the guest against the filter events.
    (将来自客户机的事件选择与过滤事件进行匹配。)
 2. If a match is found, match the guest's unit mask to the mask and match
    values of the included filter events.
    I.e. (unit mask & mask) == match && !exclude.
    (如果找到匹配，则将客户机的单元掩码与所含 (included) 过滤事件的 mask 与 match
    值进行匹配。即 (unit mask & mask) == match && !exclude。)
 3. If a match is found, match the guest's unit mask to the mask and match
    values of the excluded filter events.
    I.e. (unit mask & mask) == match && exclude.
    (如果找到匹配，则将客户机的单元掩码与被排除 (excluded) 过滤事件的 mask 与
    match 值进行匹配。即 (unit mask & mask) == match && exclude。)
 4.
   a. If an included match is found and an excluded match is not found, filter
      the event.
      (如果找到了 included 匹配且没有找到 excluded 匹配，则过滤该事件。)
   b. For everything else, do not filter the event.
      (其余所有情况均不过滤该事件。)
 5.
   a. If the event is filtered and it's an allow list, allow the guest to
      program the event.
      (如果事件被过滤且这是允许列表 (allow list)，则允许客户机对该事件编程。)
   b. If the event is filtered and it's a deny list, do not allow the guest to
      program the event.
      (如果事件被过滤且这是拒绝列表 (deny list)，则不允许客户机对该事件编程。)

When setting a new pmu event filter, -EINVAL will be returned if any of the
unused fields are set or if any of the high bits (35:32) in the event
select are set when called on Intel.
(设置新的 PMU 事件过滤器时，如果任何未使用字段被置位，或在 Intel 上调用时事件选择
的高位 (35:32) 中有任何一位被置位，则返回 -EINVAL。)

Valid values for 'action'::
(action 的有效取值：)

  #define KVM_PMU_EVENT_ALLOW 0
  #define KVM_PMU_EVENT_DENY 1

Via this API, KVM userspace can also control the behavior of the VM's fixed
counters (if any) by configuring the "action" and "fixed_counter_bitmap" fields.
(通过此 API，KVM 用户态还可以通过配置 "action" 与 "fixed_counter_bitmap" 字段来控
制 VM 的固定计数器（如果存在的话）的行为。)

Specifically, KVM follows the following pseudo-code when determining whether to
allow the guest FixCtr[i] to count its pre-defined fixed event::

  FixCtr[i]_is_allowed = (action == ALLOW) && (bitmap & BIT(i)) ||
    (action == DENY) && !(bitmap & BIT(i));
  FixCtr[i]_is_denied = !FixCtr[i]_is_allowed;

(具体而言，在决定是否允许客户机 FixCtr[i] 对其预定义的固定事件计数时，KVM 遵循以下
伪代码：)

KVM always consumes fixed_counter_bitmap, it's userspace's responsibility to
ensure fixed_counter_bitmap is set correctly, e.g. if userspace wants to define
a filter that only affects general purpose counters.
(KVM 总是会消费 fixed_counter_bitmap；确保 fixed_counter_bitmap 被正确设置是用户态
的责任，例如当用户态想定义一个只影响通用计数器的过滤器时。)

Note, the "events" field also applies to fixed counters' hardcoded event_select
and unit_mask values.  "fixed_counter_bitmap" has higher priority than "events"
if there is a contradiction between the two.
(注意："events" 字段同样适用于固定计数器硬编码的 event_select 与 unit_mask 值。
若两者存在矛盾，"fixed_counter_bitmap" 的优先级高于 "events"。)

4.121 KVM_PPC_SVM_OFF(4.121 KVM_PPC_SVM_OFF)
--------------------------------------------

:Capability: basic
:Architectures: powerpc
:Type: vm ioctl
:Parameters: none
:Returns: 0 on successful completion,

(能力：basic；架构：powerpc；类型：vm ioctl；参数：无；返回值：成功完成时为 0。)

Errors:

  ======     ================================================================
  EINVAL     if ultravisor failed to terminate the secure guest
             (ultravisor 未能终止安全客户机)
  ENOMEM     if hypervisor failed to allocate new radix page tables for guest
             (hypervisor 未能为客户机分配新的 radix 页表)
  ======     ================================================================

This ioctl is used to turn off the secure mode of the guest or transition
the guest from secure mode to normal mode. This is invoked when the guest
is reset. This has no effect if called for a normal guest.
(此 ioctl 用于关闭客户机的安全模式，或将客户机从安全模式切换回普通模式。该调用在
客户机复位时被触发。对普通客户机调用此 ioctl 没有任何效果。)

This ioctl issues an ultravisor call to terminate the secure guest,
unpins the VPA pages and releases all the device pages that are used to
track the secure pages by hypervisor.
(此 ioctl 会发出一次 ultravisor 调用以终止安全客户机，解除 VPA 页的钉住状态，并
释放 hypervisor 用于跟踪安全页的所有设备页。)

4.122 KVM_S390_NORMAL_RESET(4.122 KVM_S390_NORMAL_RESET)
--------------------------------------------------------

:Capability: KVM_CAP_S390_VCPU_RESETS
:Architectures: s390
:Type: vcpu ioctl
:Parameters: none
:Returns: 0

(能力：KVM_CAP_S390_VCPU_RESETS；架构：s390；类型：vcpu ioctl；参数：无；返回值：0。)

This ioctl resets VCPU registers and control structures according to
the cpu reset definition in the POP (Principles Of Operation).
(此 ioctl 按照 POP(操作原理，Principles Of Operation)中的 cpu reset(普通 CPU 复
位)定义，复位 VCPU 寄存器与控制结构。)

4.123 KVM_S390_INITIAL_RESET(4.123 KVM_S390_INITIAL_RESET)
----------------------------------------------------------

:Capability: basic
:Architectures: s390
:Type: vcpu ioctl
:Parameters: none
:Returns: 0

(能力：basic；架构：s390；类型：vcpu ioctl；参数：无；返回值：0。)

This ioctl resets VCPU registers and control structures according to
the initial cpu reset definition in the POP. However, the cpu is not
put into ESA mode. This reset is a superset of the normal reset.
(此 ioctl 按照 POP 中的 initial cpu reset(初始 CPU 复位)定义，复位 VCPU 寄存器与
控制结构。不过，cpu 不会进入 ESA 模式。该复位是普通复位的超集。)

4.124 KVM_S390_CLEAR_RESET(4.124 KVM_S390_CLEAR_RESET)
------------------------------------------------------

:Capability: KVM_CAP_S390_VCPU_RESETS
:Architectures: s390
:Type: vcpu ioctl
:Parameters: none
:Returns: 0

(能力：KVM_CAP_S390_VCPU_RESETS；架构：s390；类型：vcpu ioctl；参数：无；返回值：0。)

This ioctl resets VCPU registers and control structures according to
the clear cpu reset definition in the POP. However, the cpu is not put
into ESA mode. This reset is a superset of the initial reset.
(此 ioctl 按照 POP 中的 clear cpu reset(清除 CPU 复位)定义，复位 VCPU 寄存器与控
制结构。不过，cpu 不会进入 ESA 模式。该复位是初始复位的超集。)


4.125 KVM_S390_PV_COMMAND(4.125 KVM_S390_PV_COMMAND)
----------------------------------------------------

:Capability: KVM_CAP_S390_PROTECTED
:Architectures: s390
:Type: vm ioctl
:Parameters: struct kvm_pv_cmd
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_S390_PROTECTED；架构：s390；类型：vm ioctl；参数：struct
kvm_pv_cmd；返回值：成功时为 0，出错时小于 0。)

::

  struct kvm_pv_cmd {
	__u32 cmd;	/* Command to be executed */
	__u16 rc;	/* Ultravisor return code */
	__u16 rrc;	/* Ultravisor return reason code */
	__u64 data;	/* Data or address */
	__u32 flags;    /* flags for future extensions. Must be 0 for now */
	__u32 reserved[3];
  };

**Ultravisor return codes**
The Ultravisor return (reason) codes are provided by the kernel if a
Ultravisor call has been executed to achieve the results expected by
the command. Therefore they are independent of the IOCTL return
code. If KVM changes `rc`, its value will always be greater than 0
hence setting it to 0 before issuing a PV command is advised to be
able to detect a change of `rc`.
(Ultravisor 返回码：Ultravisor 返回(原因)码由内核在执行过 Ultravisor 调用、达成了
命令所期望的结果之后提供，因此它们与 ioctl 的返回码相互独立。如果 KVM 修改了 rc，
其值总是大于 0，因此建议在发出 PV 命令之前先将 rc 置 0，以便检测 rc 是否被修改。)

**cmd values:**
(cmd 取值：)

KVM_PV_ENABLE
  Allocate memory and register the VM with the Ultravisor, thereby
  donating memory to the Ultravisor that will become inaccessible to
  KVM. All existing CPUs are converted to protected ones. After this
  command has succeeded, any CPU added via hotplug will become
  protected during its creation as well.
  (分配内存并将 VM 注册到 Ultravisor，从而把内存捐赠给 Ultravisor，使其对 KVM 不
  可访问。所有现有 CPU 都会被转换为受保护状态。此命令成功之后，通过热插拔新增的
  CPU 在创建时同样会被保护起来。)

  Errors:

  =====      =============================
  EINTR      an unmasked signal is pending
             (有未被屏蔽的信号处于挂起状态)
  =====      =============================

KVM_PV_DISABLE
  Deregister the VM from the Ultravisor and reclaim the memory that had
  been donated to the Ultravisor, making it usable by the kernel again.
  All registered VCPUs are converted back to non-protected ones. If a
  previous protected VM had been prepared for asynchronous teardown with
  KVM_PV_ASYNC_CLEANUP_PREPARE and not subsequently torn down with
  KVM_PV_ASYNC_CLEANUP_PERFORM, it will be torn down in this call
  together with the current protected VM.
  (从 Ultravisor 注销该 VM，并回收之前捐赠给 Ultravisor 的内存，使其重新可供内核
  使用。所有已注册的 VCPU 都会被转换回非受保护状态。如果之前某个受保护 VM 已经通过
  KVM_PV_ASYNC_CLEANUP_PREPARE 准备好异步销毁、且随后尚未通过
  KVM_PV_ASYNC_CLEANUP_PERFORM 完成销毁，那么该 VM 将在本次调用中与当前的受保护 VM
  一起被销毁。)

KVM_PV_VM_SET_SEC_PARMS
  Pass the image header from VM memory to the Ultravisor in
  preparation of image unpacking and verification.
  (把 VM 内存中的镜像头传递给 Ultravisor，为镜像解包与校验做准备。)

KVM_PV_VM_UNPACK
  Unpack (protect and decrypt) a page of the encrypted boot image.
  (对加密启动镜像的一个页进行解包(加以保护并解密)。)

KVM_PV_VM_VERIFY
  Verify the integrity of the unpacked image. Only if this succeeds,
  KVM is allowed to start protected VCPUs.
  (校验已解包镜像的完整性。只有此步骤成功之后，KVM 才允许启动受保护的 VCPU。)

KVM_PV_INFO
  :Capability: KVM_CAP_S390_PROTECTED_DUMP

  Presents an API that provides Ultravisor related data to userspace
  via subcommands. len_max is the size of the user space buffer,
  len_written is KVM's indication of how much bytes of that buffer
  were actually written to. len_written can be used to determine the
  valid fields if more response fields are added in the future.

  ::

     enum pv_cmd_info_id {
	KVM_PV_INFO_VM,
	KVM_PV_INFO_DUMP,
     };

     struct kvm_s390_pv_info_header {
	__u32 id;
	__u32 len_max;
	__u32 len_written;
	__u32 reserved;
     };

     struct kvm_s390_pv_info {
	struct kvm_s390_pv_info_header header;
	struct kvm_s390_pv_info_dump dump;
	struct kvm_s390_pv_info_vm vm;
     };

  (该命令呈现一组通过子命令向用户态提供 Ultravisor 相关数据的 API。len_max 是用户
  态缓冲区的大小；len_written 是 KVM 对该缓冲区实际写入了多少字节的说明。如果将来
  增加了更多响应字段，可以利用 len_written 来判定哪些字段有效。)

**subcommands:**
(子命令：)

  KVM_PV_INFO_VM
    This subcommand provides basic Ultravisor information for PV
    hosts. These values are likely also exported as files in the sysfs
    firmware UV query interface but they are more easily available to
    programs in this API.
    (此子命令为 PV 主机提供基本的 Ultravisor 信息。这些值很可能也会以文件形式导出
    到 sysfs 固件 UV 查询接口，但通过本 API 程序更容易获取。)

    The installed calls and feature_indication members provide the
    installed UV calls and the UV's other feature indications.
    (installed calls 与 feature_indication 成员提供已安装的 UV 调用以及 UV 的其他
    特性指示。)

    The max_* members provide information about the maximum number of PV
    vcpus, PV guests and PV guest memory size.

    ::

      struct kvm_s390_pv_info_vm {
	__u64 inst_calls_list[4];
	__u64 max_cpus;
	__u64 max_guests;
	__u64 max_guest_addr;
	__u64 feature_indication;
      };

    (max_* 成员提供 PV vcpu 最大数量、PV 客户机最大数量以及 PV 客户机内存大小上限
    的相关信息。)


  KVM_PV_INFO_DUMP
    This subcommand provides information related to dumping PV guests.

    ::

      struct kvm_s390_pv_info_dump {
	__u64 dump_cpu_buffer_len;
	__u64 dump_config_mem_buffer_per_1m;
	__u64 dump_config_finalize_len;
      };

    (此子命令提供与转储 PV 客户机相关的信息。)

KVM_PV_DUMP
  :Capability: KVM_CAP_S390_PROTECTED_DUMP

  Presents an API that provides calls which facilitate dumping a
  protected VM.

  ::

    struct kvm_s390_pv_dmp {
      __u64 subcmd;
      __u64 buff_addr;
      __u64 buff_len;
      __u64 gaddr;		/* For dump storage state */
    };

  (该命令呈现一组便于转储受保护 VM 的调用所组成的 API。)

  **subcommands:**
  (子命令：)

  KVM_PV_DUMP_INIT
    Initializes the dump process of a protected VM. If this call does
    not succeed all other subcommands will fail with -EINVAL. This
    subcommand will return -EINVAL if a dump process has not yet been
    completed.

    Not all PV vms can be dumped, the owner needs to set `dump
    allowed` PCF bit 34 in the SE header to allow dumping.
    (初始化受保护 VM 的转储过程。如果此调用不成功，所有其他子命令都会以 -EINVAL 失
    败。如果某个转储过程尚未完成，此子命令会返回 -EINVAL。

    并非所有 PV VM 都可以转储：所有者需要在 SE 头中设置 "dump allowed" PCF 位 34，
    才允许转储。)

  KVM_PV_DUMP_CONFIG_STOR_STATE
     Stores `buff_len` bytes of tweak component values starting with
     the 1MB block specified by the absolute guest address
     (`gaddr`). `buff_len` needs to be `conf_dump_storage_state_len`
     aligned and at least >= the `conf_dump_storage_state_len` value
     provided by the dump uv_info data. buff_user might be written to
     even if an error rc is returned. For instance if we encounter a
     fault after writing the first page of data.
     (从绝对客户机地址 gaddr 所指定的 1MB 块开始，存储 buff_len 字节的 tweak 分量
     值。buff_len 必须按 conf_dump_storage_state_len 对齐，且至少不低于转储 uv_info
     数据所提供的 conf_dump_storage_state_len 值。即使返回了错误的 rc，buff_user 也
     可能已被写入，例如在写出第一页数据之后遇到访问错误的情况。)

  KVM_PV_DUMP_COMPLETE
    If the subcommand succeeds it completes the dump process and lets
    KVM_PV_DUMP_INIT be called again.

    On success `conf_dump_finalize_len` bytes of completion data will be
    stored to the `buff_addr`. The completion data contains a key
    derivation seed, IV, tweak nonce and encryption keys as well as an
    authentication tag all of which are needed to decrypt the dump at a
    later time.
    (如果该子命令成功，它会完成整个转储过程，并允许再次调用 KVM_PV_DUMP_INIT。

    成功时，conf_dump_finalize_len 字节的完成数据会被存储到 buff_addr 中。完成数据
    包含一个密钥派生种子、IV、tweak nonce 和加密密钥，以及一个认证标签——之后解密该
    转储时全部需要用到它们。)

KVM_PV_ASYNC_CLEANUP_PREPARE
  :Capability: KVM_CAP_S390_PROTECTED_ASYNC_DISABLE

  Prepare the current protected VM for asynchronous teardown. Most
  resources used by the current protected VM will be set aside for a
  subsequent asynchronous teardown. The current protected VM will then
  resume execution immediately as non-protected. There can be at most
  one protected VM prepared for asynchronous teardown at any time. If
  a protected VM had already been prepared for teardown without
  subsequently calling KVM_PV_ASYNC_CLEANUP_PERFORM, this call will
  fail. In that case, the userspace process should issue a normal
  KVM_PV_DISABLE. The resources set aside with this call will need to
  be cleaned up with a subsequent call to KVM_PV_ASYNC_CLEANUP_PERFORM
  or KVM_PV_DISABLE, otherwise they will be cleaned up when KVM
  terminates. KVM_PV_ASYNC_CLEANUP_PREPARE can be called again as soon
  as cleanup starts, i.e. before KVM_PV_ASYNC_CLEANUP_PERFORM finishes.
  (让当前受保护 VM 做好异步销毁的准备。当前受保护 VM 使用的大多数资源会被预留出来，
  供后续的异步销毁使用。随后，当前受保护 VM 会立即以非受保护状态恢复执行。任何时刻
  最多只能有一个受保护 VM 处于已准备异步销毁的状态。如果某个受保护 VM 已经准备好销
  毁、但随后没有调用 KVM_PV_ASYNC_CLEANUP_PERFORM，此调用会失败。此时，用户态进程应
  当发出一次普通的 KVM_PV_DISABLE。此调用预留的资源需要通过随后调用
  KVM_PV_ASYNC_CLEANUP_PERFORM 或 KVM_PV_DISABLE 来清理，否则它们会在 KVM 终止时被
  清理。一旦清理开始(即在 KVM_PV_ASYNC_CLEANUP_PERFORM 结束之前)，就可以再次调用
  KVM_PV_ASYNC_CLEANUP_PREPARE。)

KVM_PV_ASYNC_CLEANUP_PERFORM
  :Capability: KVM_CAP_S390_PROTECTED_ASYNC_DISABLE

  Tear down the protected VM previously prepared for teardown with
  KVM_PV_ASYNC_CLEANUP_PREPARE. The resources that had been set aside
  will be freed during the execution of this command. This PV command
  should ideally be issued by userspace from a separate thread. If a
  fatal signal is received (or the process terminates naturally), the
  command will terminate immediately without completing, and the normal
  KVM shutdown procedure will take care of cleaning up all remaining
  protected VMs, including the ones whose teardown was interrupted by
  process termination.
  (销毁之前通过 KVM_PV_ASYNC_CLEANUP_PREPARE 准备好销毁的受保护 VM。先前预留的资源
  会在本命令执行期间被释放。理想情况下，用户态应当在单独的线程中发出此 PV 命令。如
  果收到致命信号(或进程自然终止)，命令会立即终止而不会完成，由正常的 KVM 关闭流程
  负责清理所有剩余的受保护 VM，包括那些销毁过程被进程终止所打断的 VM。)

4.126 KVM_XEN_HVM_SET_ATTR(4.126 KVM_XEN_HVM_SET_ATTR)
------------------------------------------------------

:Capability: KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO
:Architectures: x86
:Type: vm ioctl
:Parameters: struct kvm_xen_hvm_attr
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO；架构：x86；类型：vm ioctl；
参数：struct kvm_xen_hvm_attr；返回值：成功时为 0，出错时小于 0。)

::

  struct kvm_xen_hvm_attr {
	__u16 type;
	__u16 pad[3];
	union {
		__u8 long_mode;
		__u8 vector;
		__u8 runstate_update_flag;
		union {
			__u64 gfn;
			__u64 hva;
		} shared_info;
		struct {
			__u32 send_port;
			__u32 type; /* EVTCHNSTAT_ipi / EVTCHNSTAT_interdomain */
			__u32 flags;
			union {
				struct {
					__u32 port;
					__u32 vcpu;
					__u32 priority;
				} port;
				struct {
					__u32 port; /* Zero for eventfd */
					__s32 fd;
				} eventfd;
				__u32 padding[4];
			} deliver;
		} evtchn;
		__u32 xen_version;
		__u64 pad[8];
	} u;
  };

type values:
(type 取值：)

KVM_XEN_ATTR_TYPE_LONG_MODE
  Sets the ABI mode of the VM to 32-bit or 64-bit (long mode). This
  determines the layout of the shared_info page exposed to the VM.
  (将 VM 的 ABI 模式设置为 32 位或 64 位(long mode)。这决定了呈现给 VM 的
  shared_info 页的布局。)

KVM_XEN_ATTR_TYPE_SHARED_INFO
  Sets the guest physical frame number at which the Xen shared_info
  page resides. Note that although Xen places vcpu_info for the first
  32 vCPUs in the shared_info page, KVM does not automatically do so
  and instead requires that KVM_XEN_VCPU_ATTR_TYPE_VCPU_INFO or
  KVM_XEN_VCPU_ATTR_TYPE_VCPU_INFO_HVA be used explicitly even when
  the vcpu_info for a given vCPU resides at the "default" location
  in the shared_info page. This is because KVM may not be aware of
  the Xen CPU id which is used as the index into the vcpu_info[]
  array, so may know the correct default location.
  (设置 Xen shared_info 页所在的客户机物理帧号。注意，尽管 Xen 会把前 32 个 vCPU
  的 vcpu_info 放在 shared_info 页中，KVM 并不会自动这样做，而是要求显式使用
  KVM_XEN_VCPU_ATTR_TYPE_VCPU_INFO 或 KVM_XEN_VCPU_ATTR_TYPE_VCPU_INFO_HVA，即使
  给定 vCPU 的 vcpu_info 位于 shared_info 页中的 "default"(默认)位置也是如此。这
  是因为 KVM 可能不知道用作 vcpu_info[] 数组索引的 Xen CPU id，因而可能不知道正确
  的默认位置。)

  Note that the shared_info page may be constantly written to by KVM;
  it contains the event channel bitmap used to deliver interrupts to
  a Xen guest, amongst other things. It is exempt from dirty tracking
  mechanisms — KVM will not explicitly mark the page as dirty each
  time an event channel interrupt is delivered to the guest! Thus,
  userspace should always assume that the designated GFN is dirty if
  any vCPU has been running or any event channel interrupts can be
  routed to the guest.
  (注意，KVM 可能会持续地写 shared_info 页；其中除其他内容外，还包含用于向 Xen
  客户机投递中断的事件通道位图。它不受脏页跟踪机制的约束——每当有事件通道中断投递
  给客户机时，KVM 都不会显式地把该页标记为脏页！因此，只要有任何 vCPU 运行过、或
  有任何事件通道中断可能被路由到客户机，用户态就应当始终假定指定的 GFN 是脏的。)

  Setting the gfn to KVM_XEN_INVALID_GFN will disable the shared_info
  page.
  (把 gfn 设置为 KVM_XEN_INVALID_GFN 将禁用 shared_info 页。)

KVM_XEN_ATTR_TYPE_SHARED_INFO_HVA
  If the KVM_XEN_HVM_CONFIG_SHARED_INFO_HVA flag is also set in the
  Xen capabilities, then this attribute may be used to set the
  userspace address at which the shared_info page resides, which
  will always be fixed in the VMM regardless of where it is mapped
  in guest physical address space. This attribute should be used in
  preference to KVM_XEN_ATTR_TYPE_SHARED_INFO as it avoids
  unnecessary invalidation of an internal cache when the page is
  re-mapped in guest physical address space.
  (如果 Xen 能力中还设置了 KVM_XEN_HVM_CONFIG_SHARED_INFO_HVA 标志，则此属性可用
  于设置 shared_info 页所在的用户态地址；无论该页被映射到客户机物理地址空间的何处，
  它在 VMM 中始终保持固定。此属性应优先于 KVM_XEN_ATTR_TYPE_SHARED_INFO 使用，因为
  它避免了该页在客户机物理地址空间中重新映射时不必要的内部缓存失效。)

  Setting the hva to zero will disable the shared_info page.
  (把 hva 设置为零将禁用 shared_info 页。)

KVM_XEN_ATTR_TYPE_UPCALL_VECTOR
  Sets the exception vector used to deliver Xen event channel upcalls.
  This is the HVM-wide vector injected directly by the hypervisor
  (not through the local APIC), typically configured by a guest via
  HVM_PARAM_CALLBACK_IRQ. This can be disabled again (e.g. for guest
  SHUTDOWN_soft_reset) by setting it to zero.
  (设置用于投递 Xen 事件通道 upcall 的异常向量。这是由 hypervisor 直接注入(不经过
  本地 APIC)的 HVM 范围向量，通常由客户机通过 HVM_PARAM_CALLBACK_IRQ 配置。把它重
  新设置为零即可将其禁用(例如在客户机 SHUTDOWN_soft_reset 时)。)

KVM_XEN_ATTR_TYPE_EVTCHN
  This attribute is available when the KVM_CAP_XEN_HVM ioctl indicates
  support for KVM_XEN_HVM_CONFIG_EVTCHN_SEND features. It configures
  an outbound port number for interception of EVTCHNOP_send requests
  from the guest. A given sending port number may be directed back to
  a specified vCPU (by APIC ID) / port / priority on the guest, or to
  trigger events on an eventfd. The vCPU and priority can be changed
  by setting KVM_XEN_EVTCHN_UPDATE in a subsequent call, but other
  fields cannot change for a given sending port. A port mapping is
  removed by using KVM_XEN_EVTCHN_DEASSIGN in the flags field. Passing
  KVM_XEN_EVTCHN_RESET in the flags field removes all interception of
  outbound event channels. The values of the flags field are mutually
  exclusive and cannot be combined as a bitmask.
  (当 KVM_CAP_XEN_HVM ioctl 表明支持 KVM_XEN_HVM_CONFIG_EVTCHN_SEND 特性时，此属
  性可用。它配置一个出站端口号，用于拦截来自客户机的 EVTCHNOP_send 请求。给定的发
  送端口号可以被引导回客户机上指定的 vCPU(按 APIC ID)/端口/优先级，或用于触发
  eventfd 上的事件。通过在后续调用中设置 KVM_XEN_EVTCHN_UPDATE 可以更改 vCPU 与优
  先级，但对给定的发送端口而言，其他字段不能更改。在 flags 字段中使用
  KVM_XEN_EVTCHN_DEASSIGN 可移除端口映射。在 flags 字段中传入 KVM_XEN_EVTCHN_RESET
  将移除对所有出站事件通道的拦截。flags 字段的这些取值互斥，不能按位掩码组合使用。)

KVM_XEN_ATTR_TYPE_XEN_VERSION
  This attribute is available when the KVM_CAP_XEN_HVM ioctl indicates
  support for KVM_XEN_HVM_CONFIG_EVTCHN_SEND features. It configures
  the 32-bit version code returned to the guest when it invokes the
  XENVER_version call; typically (XEN_MAJOR << 16 | XEN_MINOR). PV
  Xen guests will often use this to as a dummy hypercall to trigger
  event channel delivery, so responding within the kernel without
  exiting to userspace is beneficial.
  (当 KVM_CAP_XEN_HVM ioctl 表明支持 KVM_XEN_HVM_CONFIG_EVTCHN_SEND 特性时，此属
  性可用。它配置客户机调用 XENVER_version 时返回给它的 32 位版本码，通常为
  (XEN_MAJOR << 16 | XEN_MINOR)。PV Xen 客户机经常把它用作触发事件通道投递的哑超
  级调用(hypercall)，因此在内核内直接应答而不退出到用户态是有益的。)

KVM_XEN_ATTR_TYPE_RUNSTATE_UPDATE_FLAG
  This attribute is available when the KVM_CAP_XEN_HVM ioctl indicates
  support for KVM_XEN_HVM_CONFIG_RUNSTATE_UPDATE_FLAG. It enables the
  XEN_RUNSTATE_UPDATE flag which allows guest vCPUs to safely read
  other vCPUs' vcpu_runstate_info. Xen guests enable this feature via
  the VMASST_TYPE_runstate_update_flag of the HYPERVISOR_vm_assist
  hypercall.
  (当 KVM_CAP_XEN_HVM ioctl 表明支持 KVM_XEN_HVM_CONFIG_RUNSTATE_UPDATE_FLAG 时，
  此属性可用。它启用 XEN_RUNSTATE_UPDATE 标志，允许客户机 vCPU 安全地读取其他
  vCPU 的 vcpu_runstate_info。Xen 客户机通过 HYPERVISOR_vm_assist 超级调用的
  VMASST_TYPE_runstate_update_flag 启用该特性。)

4.127 KVM_XEN_HVM_GET_ATTR(4.127 KVM_XEN_HVM_GET_ATTR)
------------------------------------------------------

:Capability: KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO
:Architectures: x86
:Type: vm ioctl
:Parameters: struct kvm_xen_hvm_attr
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO；架构：x86；类型：vm ioctl；
参数：struct kvm_xen_hvm_attr；返回值：成功时为 0，出错时小于 0。)

Allows Xen VM attributes to be read. For the structure and types,
see KVM_XEN_HVM_SET_ATTR above. The KVM_XEN_ATTR_TYPE_EVTCHN
attribute cannot be read.
(允许读取 Xen VM 属性。结构与类型参见上文 KVM_XEN_HVM_SET_ATTR 一节。
KVM_XEN_ATTR_TYPE_EVTCHN 属性不可读取。)

4.128 KVM_XEN_VCPU_SET_ATTR(4.128 KVM_XEN_VCPU_SET_ATTR)
--------------------------------------------------------

:Capability: KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_xen_vcpu_attr
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO；架构：x86；类型：vcpu
ioctl；参数：struct kvm_xen_vcpu_attr；返回值：成功时为 0，出错时小于 0。)

::

  struct kvm_xen_vcpu_attr {
	__u16 type;
	__u16 pad[3];
	union {
		__u64 gpa;
		__u64 pad[4];
		struct {
			__u64 state;
			__u64 state_entry_time;
			__u64 time_running;
			__u64 time_runnable;
			__u64 time_blocked;
			__u64 time_offline;
		} runstate;
		__u32 vcpu_id;
		struct {
			__u32 port;
			__u32 priority;
			__u64 expires_ns;
		} timer;
		__u8 vector;
	} u;
  };

type values:
(type 取值：)

KVM_XEN_VCPU_ATTR_TYPE_VCPU_INFO
  Sets the guest physical address of the vcpu_info for a given vCPU.
  As with the shared_info page for the VM, the corresponding page may be
  dirtied at any time if event channel interrupt delivery is enabled, so
  userspace should always assume that the page is dirty without relying
  on dirty logging. Setting the gpa to KVM_XEN_INVALID_GPA will disable
  the vcpu_info.
  (设置给定 vCPU 的 vcpu_info 的客户机物理地址。与 VM 的 shared_info 页一样，只要
  启用了事件通道中断投递，相应页就可能在任何时刻变脏，因此用户态应当始终假定该页是
  脏的，而不应依赖脏页日志。把 gpa 设置为 KVM_XEN_INVALID_GPA 将禁用 vcpu_info。)

KVM_XEN_VCPU_ATTR_TYPE_VCPU_INFO_HVA
  If the KVM_XEN_HVM_CONFIG_SHARED_INFO_HVA flag is also set in the
  Xen capabilities, then this attribute may be used to set the
  userspace address of the vcpu_info for a given vCPU. It should
  only be used when the vcpu_info resides at the "default" location
  in the shared_info page. In this case it is safe to assume the
  userspace address will not change, because the shared_info page is
  an overlay on guest memory and remains at a fixed host address
  regardless of where it is mapped in guest physical address space
  and hence unnecessary invalidation of an internal cache may be
  avoided if the guest memory layout is modified.
  If the vcpu_info does not reside at the "default" location then
  it is not guaranteed to remain at the same host address and
  hence the aforementioned cache invalidation is required.
  (如果 Xen 能力中还设置了 KVM_XEN_HVM_CONFIG_SHARED_INFO_HVA 标志，则此属性可用
  于设置给定 vCPU 的 vcpu_info 的用户态地址。它只应在 vcpu_info 位于 shared_info
  页中 "default"(默认)位置时使用。在这种情况下，可以放心地假定用户态地址不会改变，
  因为 shared_info 页是叠加在客户机内存之上的，无论它被映射到客户机物理地址空间的
  何处，都保持在固定的宿主机地址上；因此，即使客户机内存布局被修改，也可以避免不必
  要的内部缓存失效。
  如果 vcpu_info 不位于 "default" 位置，则无法保证它停留在同一宿主机地址上，因此
  需要前述的缓存失效处理。)

KVM_XEN_VCPU_ATTR_TYPE_VCPU_TIME_INFO
  Sets the guest physical address of an additional pvclock structure
  for a given vCPU. This is typically used for guest vsyscall support.
  Setting the gpa to KVM_XEN_INVALID_GPA will disable the structure.
  (为给定 vCPU 设置一个额外 pvclock 结构的客户机物理地址。它通常用于客户机的
  vsyscall 支持。把 gpa 设置为 KVM_XEN_INVALID_GPA 将禁用该结构。)

KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_ADDR
  Sets the guest physical address of the vcpu_runstate_info for a given
  vCPU. This is how a Xen guest tracks CPU state such as steal time.
  Setting the gpa to KVM_XEN_INVALID_GPA will disable the runstate area.
  (为给定 vCPU 设置 vcpu_runstate_info 的客户机物理地址。Xen 客户机就是这样跟踪
  CPU 状态(如 steal time)的。把 gpa 设置为 KVM_XEN_INVALID_GPA 将禁用 runstate
  区域。)

KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_CURRENT
  Sets the runstate (RUNSTATE_running/_runnable/_blocked/_offline) of
  the given vCPU from the .u.runstate.state member of the structure.
  KVM automatically accounts running and runnable time but blocked
  and offline states are only entered explicitly.
  (依据结构体的 .u.runstate.state 成员，设置给定 vCPU 的运行状态
  (RUNSTATE_running/_runnable/_blocked/_offline)。KVM 会自动统计 running 与
  runnable 时间，但 blocked 与 offline 状态只能通过显式设置进入。)

KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_DATA
  Sets all fields of the vCPU runstate data from the .u.runstate member
  of the structure, including the current runstate. The state_entry_time
  must equal the sum of the other four times.
  (依据结构体的 .u.runstate 成员设置 vCPU 运行状态数据的所有字段，包括当前运行状
  态。state_entry_time 必须等于其他四项时间之和。)

KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_ADJUST
  This *adds* the contents of the .u.runstate members of the structure
  to the corresponding members of the given vCPU's runstate data, thus
  permitting atomic adjustments to the runstate times. The adjustment
  to the state_entry_time must equal the sum of the adjustments to the
  other four times. The state field must be set to -1, or to a valid
  runstate value (RUNSTATE_running, RUNSTATE_runnable, RUNSTATE_blocked
  or RUNSTATE_offline) to set the current accounted state as of the
  adjusted state_entry_time.
  (此属性把结构体 .u.runstate 各成员的内容*累加*到给定 vCPU 运行状态数据的对应成
  员上，从而允许以原子方式调整运行状态时间。对 state_entry_time 的调整量必须等于对
  其他四项时间调整量之和。state 字段必须设置为 -1，或设置为有效的运行状态值
  (RUNSTATE_running、RUNSTATE_runnable、RUNSTATE_blocked 或 RUNSTATE_offline)，
  以设定调整后 state_entry_time 时刻的当前记账状态。)

KVM_XEN_VCPU_ATTR_TYPE_VCPU_ID
  This attribute is available when the KVM_CAP_XEN_HVM ioctl indicates
  support for KVM_XEN_HVM_CONFIG_EVTCHN_SEND features. It sets the Xen
  vCPU ID of the given vCPU, to allow timer-related VCPU operations to
  be intercepted by KVM.
  (当 KVM_CAP_XEN_HVM ioctl 表明支持 KVM_XEN_HVM_CONFIG_EVTCHN_SEND 特性时，此属
  性可用。它设置给定 vCPU 的 Xen vCPU ID，使 KVM 能够拦截与定时器相关的 VCPU 操作。)

KVM_XEN_VCPU_ATTR_TYPE_TIMER
  This attribute is available when the KVM_CAP_XEN_HVM ioctl indicates
  support for KVM_XEN_HVM_CONFIG_EVTCHN_SEND features. It sets the
  event channel port/priority for the VIRQ_TIMER of the vCPU, as well
  as allowing a pending timer to be saved/restored. Setting the timer
  port to zero disables kernel handling of the singleshot timer.
  (当 KVM_CAP_XEN_HVM ioctl 表明支持 KVM_XEN_HVM_CONFIG_EVTCHN_SEND 特性时，此属
  性可用。它设置 vCPU 的 VIRQ_TIMER 的事件通道端口/优先级，同时允许保存/恢复处于
  挂起状态的定时器。把定时器端口设置为零将禁用内核对 singleshot 定时器的处理。)

KVM_XEN_VCPU_ATTR_TYPE_UPCALL_VECTOR
  This attribute is available when the KVM_CAP_XEN_HVM ioctl indicates
  support for KVM_XEN_HVM_CONFIG_EVTCHN_SEND features. It sets the
  per-vCPU local APIC upcall vector, configured by a Xen guest with
  the HVMOP_set_evtchn_upcall_vector hypercall. This is typically
  used by Windows guests, and is distinct from the HVM-wide upcall
  vector configured with HVM_PARAM_CALLBACK_IRQ. It is disabled by
  setting the vector to zero.
  (当 KVM_CAP_XEN_HVM ioctl 表明支持 KVM_XEN_HVM_CONFIG_EVTCHN_SEND 特性时，此属
  性可用。它设置每个 vCPU 的本地 APIC upcall 向量，Xen 客户机通过
  HVMOP_set_evtchn_upcall_vector 超级调用对其进行配置。它通常被 Windows 客户机使
  用，与通过 HVM_PARAM_CALLBACK_IRQ 配置的 HVM 范围 upcall 向量不同。把向量设置为
  零即可将其禁用。)


4.129 KVM_XEN_VCPU_GET_ATTR(4.129 KVM_XEN_VCPU_GET_ATTR)
--------------------------------------------------------

:Capability: KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_xen_vcpu_attr
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_SHARED_INFO；架构：x86；类型：vcpu
ioctl；参数：struct kvm_xen_vcpu_attr；返回值：成功时为 0，出错时小于 0。)

Allows Xen vCPU attributes to be read. For the structure and types,
see KVM_XEN_VCPU_SET_ATTR above.
(允许读取 Xen vCPU 属性。结构与类型参见上文 KVM_XEN_VCPU_SET_ATTR 一节。)

The KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_ADJUST type may not be used
with the KVM_XEN_VCPU_GET_ATTR ioctl.
(KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_ADJUST 类型不能与 KVM_XEN_VCPU_GET_ATTR ioctl
一起使用。)

4.130 KVM_ARM_MTE_COPY_TAGS(4.130 KVM_ARM_MTE_COPY_TAGS)
--------------------------------------------------------

:Capability: KVM_CAP_ARM_MTE
:Architectures: arm64
:Type: vm ioctl
:Parameters: struct kvm_arm_copy_mte_tags
:Returns: number of bytes copied, < 0 on error (-EINVAL for incorrect
          arguments, -EFAULT if memory cannot be accessed).

(能力：KVM_CAP_ARM_MTE；架构：arm64；类型：vm ioctl；参数：struct
kvm_arm_copy_mte_tags；返回值：复制的字节数，出错时小于 0(参数不正确时为
-EINVAL，内存无法访问时为 -EFAULT)。)

::

  struct kvm_arm_copy_mte_tags {
	__u64 guest_ipa;
	__u64 length;
	void __user *addr;
	__u64 flags;
	__u64 reserved[2];
  };

Copies Memory Tagging Extension (MTE) tags to/from guest tag memory. The
``guest_ipa`` and ``length`` fields must be ``PAGE_SIZE`` aligned.
``length`` must not be bigger than 2^31 - PAGE_SIZE bytes. The ``addr``
field must point to a buffer which the tags will be copied to or from.
(在客户机标签内存与用户态缓冲区之间复制内存标记扩展 (MTE) 的标签。``guest_ipa``
与 ``length`` 字段必须按 ``PAGE_SIZE`` 对齐。``length`` 不得大于 2^31 - PAGE_SIZE
字节。``addr`` 字段必须指向一个缓冲区，标签将被复制到该缓冲区或从中复制出来。)

``flags`` specifies the direction of copy, either ``KVM_ARM_TAGS_TO_GUEST`` or
``KVM_ARM_TAGS_FROM_GUEST``.
(``flags`` 指定复制方向，为 ``KVM_ARM_TAGS_TO_GUEST`` 或
``KVM_ARM_TAGS_FROM_GUEST`` 之一。)

The size of the buffer to store the tags is ``(length / 16)`` bytes
(granules in MTE are 16 bytes long). Each byte contains a single tag
value. This matches the format of ``PTRACE_PEEKMTETAGS`` and
``PTRACE_POKEMTETAGS``.
(存储标签的缓冲区大小为 ``(length / 16)`` 字节(MTE 中的 granule 长度为 16 字节)。
每个字节包含一个标签值。这与 ``PTRACE_PEEKMTETAGS`` 和 ``PTRACE_POKEMTETAGS`` 的
格式一致。)

If an error occurs before any data is copied then a negative error code is
returned. If some tags have been copied before an error occurs then the number
of bytes successfully copied is returned. If the call completes successfully
then ``length`` is returned.
(如果在复制任何数据之前发生错误，则返回负的错误码。如果在发生错误之前已经复制了
部分标签，则返回成功复制的字节数。如果调用成功完成，则返回 ``length``。)

4.131 KVM_GET_SREGS2(4.131 KVM_GET_SREGS2)
------------------------------------------

:Capability: KVM_CAP_SREGS2
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_sregs2 (out)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_SREGS2；架构：x86；类型：vcpu ioctl；参数：struct kvm_sregs2
(out)；返回值：成功时为 0，出错时为 -1。)

Reads special registers from the vcpu.
This ioctl (when supported) replaces the KVM_GET_SREGS.
(从 vcpu 读取特殊寄存器。此 ioctl(在受支持时)取代 KVM_GET_SREGS。)

::

        struct kvm_sregs2 {
                /* out (KVM_GET_SREGS2) / in (KVM_SET_SREGS2) */
                struct kvm_segment cs, ds, es, fs, gs, ss;
                struct kvm_segment tr, ldt;
                struct kvm_dtable gdt, idt;
                __u64 cr0, cr2, cr3, cr4, cr8;
                __u64 efer;
                __u64 apic_base;
                __u64 flags;
                __u64 pdptrs[4];
        };

flags values for ``kvm_sregs2``:
(kvm_sregs2 的 flags 取值：)

``KVM_SREGS2_FLAGS_PDPTRS_VALID``

  Indicates that the struct contains valid PDPTR values.
  (表明该结构体中包含有效的 PDPTR 值。)


4.132 KVM_SET_SREGS2(4.132 KVM_SET_SREGS2)
------------------------------------------

:Capability: KVM_CAP_SREGS2
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_sregs2 (in)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_SREGS2；架构：x86；类型：vcpu ioctl；参数：struct kvm_sregs2
(in)；返回值：成功时为 0，出错时为 -1。)

Writes special registers into the vcpu.
See KVM_GET_SREGS2 for the data structures.
This ioctl (when supported) replaces the KVM_SET_SREGS.
(向 vcpu 写入特殊寄存器。数据结构参见 KVM_GET_SREGS2。此 ioctl(在受支持时)取代
KVM_SET_SREGS。)

4.133 KVM_GET_STATS_FD(4.133 KVM_GET_STATS_FD)
----------------------------------------------

:Capability: KVM_CAP_STATS_BINARY_FD
:Architectures: all
:Type: vm ioctl, vcpu ioctl
:Parameters: none
:Returns: statistics file descriptor on success, < 0 on error

(能力：KVM_CAP_STATS_BINARY_FD；架构：全部；类型：vm ioctl, vcpu ioctl；参数：无；
返回值：成功时为统计信息文件描述符，出错时小于 0。)

Errors:

  ======     ======================================================
  ENOMEM     if the fd could not be created due to lack of memory
             (因内存不足而无法创建 fd)
  EMFILE     if the number of opened files exceeds the limit
             (打开文件的数量超过限制)
  ======     ======================================================

The returned file descriptor can be used to read VM/vCPU statistics data in
binary format. The data in the file descriptor consists of four blocks
organized as follows:
(返回的文件描述符可用于以二进制格式读取 VM/vCPU 统计数据。文件描述符中的数据由
按如下方式组织的四个块组成：)

+-------------+
|   Header    |
+-------------+
|  id string  |
+-------------+
| Descriptors |
+-------------+
| Stats Data  |
+-------------+

Apart from the header starting at offset 0, please be aware that it is
not guaranteed that the four blocks are adjacent or in the above order;
the offsets of the id, descriptors and data blocks are found in the
header.  However, all four blocks are aligned to 64 bit offsets in the
file and they do not overlap.
(除了从偏移 0 开始的头部之外，请注意不保证这四个块彼此相邻或按上述顺序排列；id
块、描述符块与数据块的偏移量记录在头部中。不过，这四个块都按 64 位对齐的偏移量存
放，且互不重叠。)

All blocks except the data block are immutable.  Userspace can read them
only one time after retrieving the file descriptor, and then use ``pread`` or
``lseek`` to read the statistics repeatedly.
(除数据块之外的所有块都是不可变的。用户态在取得文件描述符之后只需读取它们一次，
随后即可使用 ``pread`` 或 ``lseek`` 反复读取统计数据。)

All data is in system endianness.
(所有数据均采用系统字节序。)

The format of the header is as follows::

	struct kvm_stats_header {
		__u32 flags;
		__u32 name_size;
		__u32 num_desc;
		__u32 id_offset;
		__u32 desc_offset;
		__u32 data_offset;
	};

(头部格式如下：)

The ``flags`` field is not used at the moment. It is always read as 0.
(``flags`` 字段目前未使用。它总是被读出为 0。)

The ``name_size`` field is the size (in byte) of the statistics name string
(including trailing '\0') which is contained in the "id string" block and
appended at the end of every descriptor.
(``name_size`` 字段是统计名称字符串的大小(以字节为单位，包含结尾的 '\0')，该字
符串包含在 "id string" 块中，并附加在每个描述符的末尾。)

The ``num_desc`` field is the number of descriptors that are included in the
descriptor block.  (The actual number of values in the data block may be
larger, since each descriptor may comprise more than one value).
(``num_desc`` 字段是描述符块中所包含描述符的数量。(数据块中实际的数值数量可能更
大，因为每个描述符可能包含多于一个数值)。)

The ``id_offset`` field is the offset of the id string from the start of the
file indicated by the file descriptor. It is a multiple of 8.
(``id_offset`` 字段是 id 字符串相对于文件描述符所指文件起始处的偏移量。它是 8 的
倍数。)

The ``desc_offset`` field is the offset of the Descriptors block from the start
of the file indicated by the file descriptor. It is a multiple of 8.
(``desc_offset`` 字段是 Descriptors 块相对于文件描述符所指文件起始处的偏移量。它
是 8 的倍数。)

The ``data_offset`` field is the offset of the Stats Data block from the start
of the file indicated by the file descriptor. It is a multiple of 8.
(``data_offset`` 字段是 Stats Data 块相对于文件描述符所指文件起始处的偏移量。它是
8 的倍数。)

The id string block contains a string which identifies the file descriptor on
which KVM_GET_STATS_FD was invoked.  The size of the block, including the
trailing ``'\0'``, is indicated by the ``name_size`` field in the header.
(id 字符串块包含一个字符串，用于标识在其上调用 KVM_GET_STATS_FD 的文件描述符。该
块的大小(包含结尾的 '\0')由头部中的 ``name_size`` 字段指示。)

The descriptors block is only needed to be read once for the lifetime of the
file descriptor contains a sequence of ``struct kvm_stats_desc``, each followed
by a string of size ``name_size``.
::

	#define KVM_STATS_TYPE_SHIFT		0
	#define KVM_STATS_TYPE_MASK		(0xF << KVM_STATS_TYPE_SHIFT)
	#define KVM_STATS_TYPE_CUMULATIVE	(0x0 << KVM_STATS_TYPE_SHIFT)
	#define KVM_STATS_TYPE_INSTANT		(0x1 << KVM_STATS_TYPE_SHIFT)
	#define KVM_STATS_TYPE_PEAK		(0x2 << KVM_STATS_TYPE_SHIFT)
	#define KVM_STATS_TYPE_LINEAR_HIST	(0x3 << KVM_STATS_TYPE_SHIFT)
	#define KVM_STATS_TYPE_LOG_HIST		(0x4 << KVM_STATS_TYPE_SHIFT)
	#define KVM_STATS_TYPE_MAX		KVM_STATS_TYPE_LOG_HIST

	#define KVM_STATS_UNIT_SHIFT		4
	#define KVM_STATS_UNIT_MASK		(0xF << KVM_STATS_UNIT_SHIFT)
	#define KVM_STATS_UNIT_NONE		(0x0 << KVM_STATS_UNIT_SHIFT)
	#define KVM_STATS_UNIT_BYTES		(0x1 << KVM_STATS_UNIT_SHIFT)
	#define KVM_STATS_UNIT_SECONDS		(0x2 << KVM_STATS_UNIT_SHIFT)
	#define KVM_STATS_UNIT_CYCLES		(0x3 << KVM_STATS_UNIT_SHIFT)
	#define KVM_STATS_UNIT_BOOLEAN		(0x4 << KVM_STATS_UNIT_SHIFT)
	#define KVM_STATS_UNIT_MAX		KVM_STATS_UNIT_BOOLEAN

	#define KVM_STATS_BASE_SHIFT		8
	#define KVM_STATS_BASE_MASK		(0xF << KVM_STATS_BASE_SHIFT)
	#define KVM_STATS_BASE_POW10		(0x0 << KVM_STATS_BASE_SHIFT)
	#define KVM_STATS_BASE_POW2		(0x1 << KVM_STATS_BASE_SHIFT)
	#define KVM_STATS_BASE_MAX		KVM_STATS_BASE_POW2

	struct kvm_stats_desc {
		__u32 flags;
		__s16 exponent;
		__u16 size;
		__u32 offset;
		__u32 bucket_size;
		char name[];
	};

(描述符块在文件描述符的生命周期内只需读取一次，其中包含一系列 kvm_stats_desc
结构体，每个结构体后面跟着一个大小为 name_size 的字符串。)

The ``flags`` field contains the type and unit of the statistics data described
by this descriptor. Its endianness is CPU native.
The following flags are supported:
(``flags`` 字段包含此描述符所描述统计数据的类型与单位。其字节序为 CPU 原生序。
支持以下标志：)

Bits 0-3 of ``flags`` encode the type:
(``flags`` 的位 0-3 编码类型：)

  * ``KVM_STATS_TYPE_CUMULATIVE``
    The statistics reports a cumulative count. The value of data can only be increased.
    Most of the counters used in KVM are of this type.
    The corresponding ``size`` field for this type is always 1.
    All cumulative statistics data are read/write.
    (该统计报告一个累计计数。数据的值只能增加。KVM 中使用的大多数计数器都属于此
    类型。此类型对应的 ``size`` 字段总是 1。所有累计型统计数据均可读可写。)
  * ``KVM_STATS_TYPE_INSTANT``
    The statistics reports an instantaneous value. Its value can be increased or
    decreased. This type is usually used as a measurement of some resources,
    like the number of dirty pages, the number of large pages, etc.
    All instant statistics are read only.
    The corresponding ``size`` field for this type is always 1.
    (该统计报告一个瞬时值。其值可以增加或减少。此类型通常用于度量某些资源，例如
    脏页的数量、大页的数量等。所有瞬时型统计均为只读。此类型对应的 ``size`` 字段
    总是 1。)
  * ``KVM_STATS_TYPE_PEAK``
    The statistics data reports a peak value, for example the maximum number
    of items in a hash table bucket, the longest time waited and so on.
    The value of data can only be increased.
    The corresponding ``size`` field for this type is always 1.
    (该统计数据报告一个峰值，例如哈希表桶中条目的最大数量、最长等待时间等。数据的
    值只能增加。此类型对应的 ``size`` 字段总是 1。)
  * ``KVM_STATS_TYPE_LINEAR_HIST``
    The statistic is reported as a linear histogram. The number of
    buckets is specified by the ``size`` field. The size of buckets is specified
    by the ``hist_param`` field. The range of the Nth bucket (1 <= N < ``size``)
    is [``hist_param``*(N-1), ``hist_param``*N), while the range of the last
    bucket is [``hist_param``*(``size``-1), +INF). (+INF means positive infinity
    value.)
    (该统计以线性直方图形式报告。桶的数量由 ``size`` 字段指定，桶的大小由
    ``hist_param`` 字段指定。第 N 个桶 (1 <= N < size) 的范围是
    [hist_param*(N-1), hist_param*N)，而最后一个桶的范围是
    [hist_param*(size-1), +INF)。(＋INF 表示正无穷大值。)
  * ``KVM_STATS_TYPE_LOG_HIST``
    The statistic is reported as a logarithmic histogram. The number of
    buckets is specified by the ``size`` field. The range of the first bucket is
    [0, 1), while the range of the last bucket is [pow(2, ``size``-2), +INF).
    Otherwise, The Nth bucket (1 < N < ``size``) covers
    [pow(2, N-2), pow(2, N-1)).
    (该统计以对数直方图形式报告。桶的数量由 ``size`` 字段指定。第一个桶的范围是
    [0, 1)，最后一个桶的范围是 [pow(2, size-2), +INF)。此外，第 N 个桶
    (1 < N < size) 覆盖 [pow(2, N-2), pow(2, N-1))。)

Bits 4-7 of ``flags`` encode the unit:
(``flags`` 的位 4-7 编码单位：)

  * ``KVM_STATS_UNIT_NONE``
    There is no unit for the value of statistics data. This usually means that
    the value is a simple counter of an event.
    (统计数据的值没有单位。这通常意味着该值是某个事件的简单计数器。)
  * ``KVM_STATS_UNIT_BYTES``
    It indicates that the statistics data is used to measure memory size, in the
    unit of Byte, KiByte, MiByte, GiByte, etc. The unit of the data is
    determined by the ``exponent`` field in the descriptor.
    (表示统计数据用于度量内存大小，单位为 Byte、KiByte、MiByte、GiByte 等。数据的
    单位由描述符中的 ``exponent`` 字段决定。)
  * ``KVM_STATS_UNIT_SECONDS``
    It indicates that the statistics data is used to measure time or latency.
    (表示统计数据用于度量时间或延迟。)
  * ``KVM_STATS_UNIT_CYCLES``
    It indicates that the statistics data is used to measure CPU clock cycles.
    (表示统计数据用于度量 CPU 时钟周期。)
  * ``KVM_STATS_UNIT_BOOLEAN``
    It indicates that the statistic will always be either 0 or 1.  Boolean
    statistics of "peak" type will never go back from 1 to 0.  Boolean
    statistics can be linear histograms (with two buckets) but not logarithmic
    histograms.
    (表示该统计的值始终为 0 或 1。"peak" 类型的布尔统计永远不会从 1 回落到 0。
    布尔统计可以是线性直方图(含两个桶)，但不能是对数直方图。)

Note that, in the case of histograms, the unit applies to the bucket
ranges, while the bucket value indicates how many samples fell in the
bucket's range.
(注意，对于直方图而言，单位适用于桶的范围，而桶的值表示落入该桶范围内的样本数量。)

Bits 8-11 of ``flags``, together with ``exponent``, encode the scale of the
unit:
(``flags`` 的位 8-11 与 ``exponent`` 一起编码单位的刻度：)

  * ``KVM_STATS_BASE_POW10``
    The scale is based on power of 10. It is used for measurement of time and
    CPU clock cycles.  For example, an exponent of -9 can be used with
    ``KVM_STATS_UNIT_SECONDS`` to express that the unit is nanoseconds.
    (刻度基于 10 的幂。它用于时间与 CPU 时钟周期的度量。例如，指数 -9 可与
    ``KVM_STATS_UNIT_SECONDS`` 搭配使用，表示单位为纳秒。)
  * ``KVM_STATS_BASE_POW2``
    The scale is based on power of 2. It is used for measurement of memory size.
    For example, an exponent of 20 can be used with ``KVM_STATS_UNIT_BYTES`` to
    express that the unit is MiB.
    (刻度基于 2 的幂。它用于内存大小的度量。例如，指数 20 可与
    ``KVM_STATS_UNIT_BYTES`` 搭配使用，表示单位为 MiB。)

The ``size`` field is the number of values of this statistics data. Its
value is usually 1 for most of simple statistics. 1 means it contains an
unsigned 64bit data.
(``size`` 字段是该统计数据包含的数值个数。对于大多数简单统计而言，其值通常为 1。
1 表示它包含一个无符号 64 位数据。)

The ``offset`` field is the offset from the start of Data Block to the start of
the corresponding statistics data.
(``offset`` 字段是从 Data Block 起始处到相应统计数据起始处的偏移量。)

The ``bucket_size`` field is used as a parameter for histogram statistics data.
It is only used by linear histogram statistics data, specifying the size of a
bucket in the unit expressed by bits 4-11 of ``flags`` together with ``exponent``.
(``bucket_size`` 字段用作直方图统计数据的参数。它只被线性直方图统计数据使用，指定
一个桶的大小，其单位由 ``flags`` 的位 4-11 与 ``exponent`` 共同表达。)

The ``name`` field is the name string of the statistics data. The name string
starts at the end of ``struct kvm_stats_desc``.  The maximum length including
the trailing ``'\0'``, is indicated by ``name_size`` in the header.
(``name`` 字段是统计数据的名称字符串。名称字符串从 ``struct kvm_stats_desc`` 的
末尾开始。最大长度(包含结尾的 '\0')由头部中的 ``name_size`` 指示。)

The Stats Data block contains an array of 64-bit values in the same order
as the descriptors in Descriptors block.
(Stats Data 块包含一个 64 位值数组，其排列顺序与 Descriptors 块中描述符的顺序
相同。)

4.134 KVM_GET_XSAVE2(4.134 KVM_GET_XSAVE2)
------------------------------------------

:Capability: KVM_CAP_XSAVE2
:Architectures: x86
:Type: vcpu ioctl
:Parameters: struct kvm_xsave (out)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_XSAVE2；架构：x86；类型：vcpu ioctl；参数：struct kvm_xsave
(out)；返回值：成功时为 0，出错时为 -1。)

::

  struct kvm_xsave {
	__u32 region[1024];
	__u32 extra[0];
  };

This ioctl would copy current vcpu's xsave struct to the userspace. It
copies as many bytes as are returned by KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2)
when invoked on the vm file descriptor. The size value returned by
KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2) will always be at least 4096.
Currently, it is only greater than 4096 if a dynamic feature has been
enabled with ``arch_prctl()``, but this may change in the future.
(此 ioctl 会把当前 vcpu 的 xsave 结构复制到用户态。它复制的字节数等于在 vm 文件描
述符上调用 KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2) 时返回的大小。
KVM_CHECK_EXTENSION(KVM_CAP_XSAVE2) 返回的大小值总是至少为 4096。目前，只有在通
过 arch_prctl() 启用了动态特性时它才会大于 4096，但将来这一点可能会改变。)

The offsets of the state save areas in struct kvm_xsave follow the contents
of CPUID leaf 0xD on the host.
(struct kvm_xsave 中各状态保存区域的偏移量遵循宿主机上 CPUID leaf 0xD 的内容。)

4.135 KVM_XEN_HVM_EVTCHN_SEND(4.135 KVM_XEN_HVM_EVTCHN_SEND)
------------------------------------------------------------

:Capability: KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_EVTCHN_SEND
:Architectures: x86
:Type: vm ioctl
:Parameters: struct kvm_irq_routing_xen_evtchn
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_XEN_HVM / KVM_XEN_HVM_CONFIG_EVTCHN_SEND；架构：x86；类型：vm
ioctl；参数：struct kvm_irq_routing_xen_evtchn；返回值：成功时为 0，出错时小于 0。)

::

   struct kvm_irq_routing_xen_evtchn {
	__u32 port;
	__u32 vcpu;
	__u32 priority;
   };

This ioctl injects an event channel interrupt directly to the guest vCPU.
(此 ioctl 直接向客户机 vCPU 注入一个事件通道中断。)

4.136 KVM_S390_PV_CPU_COMMAND(4.136 KVM_S390_PV_CPU_COMMAND)
------------------------------------------------------------

:Capability: KVM_CAP_S390_PROTECTED_DUMP
:Architectures: s390
:Type: vcpu ioctl
:Parameters: none
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_S390_PROTECTED_DUMP；架构：s390；类型：vcpu ioctl；参数：无；返回
值：成功时为 0，出错时小于 0。)

This ioctl closely mirrors `KVM_S390_PV_COMMAND` but handles requests
for vcpus. It re-uses the kvm_s390_pv_dmp struct and hence also shares
the command ids.
(此 ioctl 与 KVM_S390_PV_COMMAND 密切对应，但处理针对 vcpu 的请求。它复用
kvm_s390_pv_dmp 结构体，因此也共享相同的命令 id。)

**command:**
(命令：)

KVM_PV_DUMP
  Presents an API that provides calls which facilitate dumping a vcpu
  of a protected VM.
  (呈现一组便于转储受保护 VM 的某个 vcpu 的调用所组成的 API。)

**subcommand:**
(子命令：)

KVM_PV_DUMP_CPU
  Provides encrypted dump data like register values.
  The length of the returned data is provided by uv_info.guest_cpu_stor_len.
  (提供加密的转储数据，例如寄存器值。返回数据的长度由 uv_info.guest_cpu_stor_len
  给出。)

4.137 KVM_S390_ZPCI_OP(4.137 KVM_S390_ZPCI_OP)
----------------------------------------------

:Capability: KVM_CAP_S390_ZPCI_OP
:Architectures: s390
:Type: vm ioctl
:Parameters: struct kvm_s390_zpci_op (in)
:Returns: 0 on success, <0 on error

(能力：KVM_CAP_S390_ZPCI_OP；架构：s390；类型：vm ioctl；参数：struct
kvm_s390_zpci_op (in)；返回值：成功时为 0，出错时小于 0。)

Used to manage hardware-assisted virtualization features for zPCI devices.
(用于管理 zPCI 设备的硬件辅助虚拟化特性。)

Parameters are specified via the following structure::

  struct kvm_s390_zpci_op {
	/* in */
	__u32 fh;		/* target device */
	__u8  op;		/* operation to perform */
	__u8  pad[3];
	union {
		/* for KVM_S390_ZPCIOP_REG_AEN */
		struct {
			__u64 ibv;	/* Guest addr of interrupt bit vector */
			__u64 sb;	/* Guest addr of summary bit */
			__u32 flags;
			__u32 noi;	/* Number of interrupts */
			__u8 isc;	/* Guest interrupt subclass */
			__u8 sbo;	/* Offset of guest summary bit vector */
			__u16 pad;
		} reg_aen;
		__u64 reserved[8];
	} u;
  };

(参数通过以下结构指定：)

The type of operation is specified in the "op" field.
KVM_S390_ZPCIOP_REG_AEN is used to register the VM for adapter event
notification interpretation, which will allow firmware delivery of adapter
events directly to the vm, with KVM providing a backup delivery mechanism;
KVM_S390_ZPCIOP_DEREG_AEN is used to subsequently disable interpretation of
adapter event notifications.
(操作的类型在 "op" 字段中指定。KVM_S390_ZPCIOP_REG_AEN 用于把 VM 注册到适配器事
件通知解释 (adapter event notification interpretation)，这将允许固件把适配器事件
直接投递给 vm，而由 KVM 提供备份投递机制；KVM_S390_ZPCIOP_DEREG_AEN 用于随后禁用
适配器事件通知的解释。)

The target zPCI function must also be specified via the "fh" field.  For the
KVM_S390_ZPCIOP_REG_AEN operation, additional information to establish firmware
delivery must be provided via the "reg_aen" struct.
(还必须通过 "fh" 字段指定目标 zPCI 函数。对于 KVM_S390_ZPCIOP_REG_AEN 操作，必须
通过 "reg_aen" 结构体提供用于建立固件投递的附加信息。)

The "pad" and "reserved" fields may be used for future extensions and should be
set to 0s by userspace.
("pad" 与 "reserved" 字段可用于将来的扩展，用户态应把它们设置为 0。)

4.138 KVM_ARM_SET_COUNTER_OFFSET(4.138 KVM_ARM_SET_COUNTER_OFFSET)
------------------------------------------------------------------

:Capability: KVM_CAP_COUNTER_OFFSET
:Architectures: arm64
:Type: vm ioctl
:Parameters: struct kvm_arm_counter_offset (in)
:Returns: 0 on success, < 0 on error

(能力：KVM_CAP_COUNTER_OFFSET；架构：arm64；类型：vm ioctl；参数：struct
kvm_arm_counter_offset (in)；返回值：成功时为 0，出错时小于 0。)

This capability indicates that userspace is able to apply a single VM-wide
offset to both the virtual and physical counters as viewed by the guest
using the KVM_ARM_SET_CNT_OFFSET ioctl and the following data structure:
(此能力表明，用户态能够使用 KVM_ARM_SET_CNT_OFFSET ioctl 及以下数据结构，对客户
机所见的虚拟与物理计数器施加一个 VM 范围的统一偏移量：)

::

	struct kvm_arm_counter_offset {
		__u64 counter_offset;
		__u64 reserved;
	};

The offset describes a number of counter cycles that are subtracted from
both virtual and physical counter views (similar to the effects of the
CNTVOFF_EL2 and CNTPOFF_EL2 system registers, but only global). The offset
always applies to all vcpus (already created or created after this ioctl)
for this VM.
(该偏移量描述了将从虚拟与物理计数器视图中减去的计数器周期数(类似于 CNTVOFF_EL2
与 CNTPOFF_EL2 系统寄存器的效果，但只是全局性的)。该偏移量始终作用于该 VM 的所有
vcpu(包括已创建的以及在此 ioctl 之后创建的)。)

It is userspace's responsibility to compute the offset based, for example,
on previous values of the guest counters.
(用户态负责计算该偏移量，例如基于客户机计数器此前的取值来计算。)

Any value other than 0 for the "reserved" field may result in an error
(-EINVAL) being returned. This ioctl can also return -EBUSY if any vcpu
ioctl is issued concurrently.
("reserved" 字段为 0 以外的任何值都可能导致返回错误 (-EINVAL)。如果并发地发出了
任何 vcpu ioctl，此 ioctl 也可能返回 -EBUSY。)

Note that using this ioctl results in KVM ignoring subsequent userspace
writes to the CNTVCT_EL0 and CNTPCT_EL0 registers using the SET_ONE_REG
interface. No error will be returned, but the resulting offset will not be
applied.
(注意，使用此 ioctl 会导致 KVM 忽略用户态随后通过 SET_ONE_REG 接口对 CNTVCT_EL0
与 CNTPCT_EL0 寄存器的写入。不会返回任何错误，但得到的偏移量不会被应用。)

.. _KVM_ARM_GET_REG_WRITABLE_MASKS:

4.139 KVM_ARM_GET_REG_WRITABLE_MASKS
------------------------------------

:Capability: KVM_CAP_ARM_SUPPORTED_REG_MASK_RANGES
:Architectures: arm64
:Type: vm ioctl
:Parameters: struct reg_mask_range (in/out)
:Returns: 0 on success, < 0 on error


::

        #define KVM_ARM_FEATURE_ID_RANGE	0
        #define KVM_ARM_FEATURE_ID_RANGE_SIZE	(3 * 8 * 8)

        struct reg_mask_range {
                __u64 addr;             /* Pointer to mask array */
                __u32 range;            /* Requested range */
                __u32 reserved[13];
        };

This ioctl copies the writable masks for a selected range of registers to
userspace.

The ``addr`` field is a pointer to the destination array where KVM copies
the writable masks.

The ``range`` field indicates the requested range of registers.
``KVM_CHECK_EXTENSION`` for the ``KVM_CAP_ARM_SUPPORTED_REG_MASK_RANGES``
capability returns the supported ranges, expressed as a set of flags. Each
flag's bit index represents a possible value for the ``range`` field.
All other values are reserved for future use and KVM may return an error.
(``range`` 字段指示所请求的寄存器范围。对 ``KVM_CAP_ARM_SUPPORTED_REG_MASK_RANGES``
能力执行 ``KVM_CHECK_EXTENSION`` 会以一组标志的形式返回受支持的范围。每个标志的位
索引代表 ``range`` 字段的一个可能取值。所有其他取值保留供将来使用，KVM 可能返回错
误。)

The ``reserved[13]`` array is reserved for future use and should be 0, or
KVM may return an error.
(``reserved[13]`` 数组保留供将来使用，应当为 0，否则 KVM 可能返回错误。)

KVM_ARM_FEATURE_ID_RANGE (0)(KVM_ARM_FEATURE_ID_RANGE (0))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Feature ID range is defined as the AArch64 System register space with
op0==3, op1=={0, 1, 3}, CRn==0, CRm=={0-7}, op2=={0-7}.
(Feature ID 范围定义为满足 op0==3、op1=={0, 1, 3}、CRn==0、CRm=={0-7}、
op2=={0-7} 的 AArch64 系统寄存器空间。)

The mask returned array pointed to by ``addr`` is indexed by the macro
``ARM64_FEATURE_ID_RANGE_IDX(op0, op1, crn, crm, op2)``, allowing userspace
to know what fields can be changed for the system register described by
``op0, op1, crn, crm, op2``. KVM rejects ID register values that describe a
superset of the features supported by the system.
(``addr`` 所指向的返回掩码数组以宏
``ARM64_FEATURE_ID_RANGE_IDX(op0, op1, crn, crm, op2)`` 为索引，使用户态能够知道
对于由 ``op0, op1, crn, crm, op2`` 所描述的系统寄存器，哪些字段可以被修改。KVM 会
拒绝那些描述了系统所支持特性之超集的 ID 寄存器值。)

4.140 KVM_SET_USER_MEMORY_REGION2(4.140 KVM_SET_USER_MEMORY_REGION2)
--------------------------------------------------------------------

:Capability: KVM_CAP_USER_MEMORY2
:Architectures: all
:Type: vm ioctl
:Parameters: struct kvm_userspace_memory_region2 (in)
:Returns: 0 on success, -1 on error

(能力：KVM_CAP_USER_MEMORY2；架构：全部；类型：vm ioctl；参数：struct
kvm_userspace_memory_region2 (in)；返回值：成功时为 0，出错时为 -1。)

KVM_SET_USER_MEMORY_REGION2 is an extension to KVM_SET_USER_MEMORY_REGION that
allows mapping guest_memfd memory into a guest.  All fields shared with
KVM_SET_USER_MEMORY_REGION identically.  Userspace can set KVM_MEM_GUEST_MEMFD
in flags to have KVM bind the memory region to a given guest_memfd range of
[guest_memfd_offset, guest_memfd_offset + memory_size].  The target guest_memfd
must point at a file created via KVM_CREATE_GUEST_MEMFD on the current VM, and
the target range must not be bound to any other memory region.  All standard
bounds checks apply (use common sense).
(KVM_SET_USER_MEMORY_REGION2 是 KVM_SET_USER_MEMORY_REGION 的扩展，允许把
guest_memfd 内存映射进客户机。与 KVM_SET_USER_MEMORY_REGION 共有的字段含义完全相
同。用户态可以在 flags 中设置 KVM_MEM_GUEST_MEMFD，让 KVM 把内存区域绑定到给定的
guest_memfd 范围 [guest_memfd_offset, guest_memfd_offset + memory_size]。目标
guest_memfd 必须指向在当前 VM 上通过 KVM_CREATE_GUEST_MEMFD 创建的文件，且目标范
围不得绑定到任何其他内存区域。所有标准的边界检查都适用(请运用常识)。)

::

  struct kvm_userspace_memory_region2 {
	__u32 slot;
	__u32 flags;
	__u64 guest_phys_addr;
	__u64 memory_size; /* bytes */
	__u64 userspace_addr; /* start of the userspace allocated memory */
	__u64 guest_memfd_offset;
	__u32 guest_memfd;
	__u32 pad1;
	__u64 pad2[14];
  };

A KVM_MEM_GUEST_MEMFD region _must_ have a valid guest_memfd (private memory) and
userspace_addr (shared memory).  However, "valid" for userspace_addr simply
means that the address itself must be a legal userspace address.  The backing
mapping for userspace_addr is not required to be valid/populated at the time of
KVM_SET_USER_MEMORY_REGION2, e.g. shared memory can be lazily mapped/allocated
on-demand.
(KVM_MEM_GUEST_MEMFD 区域必须具有有效的 guest_memfd(私有内存)与
userspace_addr(共享内存)。不过，对 userspace_addr 而言 "valid" 只意味着地址本身必
须是合法的用户态地址。在执行 KVM_SET_USER_MEMORY_REGION2 时，userspace_addr 的后
备映射并不要求有效/已填充，例如共享内存可以按需懒式映射/分配。)

When mapping a gfn into the guest, KVM selects shared vs. private, i.e consumes
userspace_addr vs. guest_memfd, based on the gfn's KVM_MEMORY_ATTRIBUTE_PRIVATE
state.  At VM creation time, all memory is shared, i.e. the PRIVATE attribute
is '0' for all gfns.  Userspace can control whether memory is shared/private by
toggling KVM_MEMORY_ATTRIBUTE_PRIVATE via KVM_SET_MEMORY_ATTRIBUTES as needed.
(把某个 gfn 映射进客户机时，KVM 会依据该 gfn 的 KVM_MEMORY_ATTRIBUTE_PRIVATE 状态
选择共享还是私有，即使用 userspace_addr 还是 guest_memfd。VM 创建时，所有内存都是
共享的，即所有 gfn 的 PRIVATE 属性都为 '0'。用户态可以按需通过
KVM_SET_MEMORY_ATTRIBUTES 切换 KVM_MEMORY_ATTRIBUTE_PRIVATE，以控制内存是共享还
是私有。)

S390:(S390)
^^^^^^^^^^^

Returns -EINVAL if the VM has the KVM_VM_S390_UCONTROL flag set.
Returns -EINVAL if called on a protected VM.
(如果 VM 设置了 KVM_VM_S390_UCONTROL 标志，则返回 -EINVAL。对受保护 VM 调用时，
返回 -EINVAL。)

4.141 KVM_SET_MEMORY_ATTRIBUTES(4.141 KVM_SET_MEMORY_ATTRIBUTES)
----------------------------------------------------------------

:Capability: KVM_CAP_MEMORY_ATTRIBUTES
:Architectures: x86
:Type: vm ioctl
:Parameters: struct kvm_memory_attributes (in)
:Returns: 0 on success, <0 on error

(能力：KVM_CAP_MEMORY_ATTRIBUTES；架构：x86；类型：vm ioctl；参数：struct
kvm_memory_attributes (in)；返回值：成功时为 0，出错时小于 0。)

KVM_SET_MEMORY_ATTRIBUTES allows userspace to set memory attributes for a range
of guest physical memory.
(KVM_SET_MEMORY_ATTRIBUTES 允许用户态为一段客户机物理内存设置内存属性。)

::

  struct kvm_memory_attributes {
	__u64 address;
	__u64 size;
	__u64 attributes;
	__u64 flags;
  };

  #define KVM_MEMORY_ATTRIBUTE_PRIVATE           (1ULL << 3)

The address and size must be page aligned.  The supported attributes can be
retrieved via ioctl(KVM_CHECK_EXTENSION) on KVM_CAP_MEMORY_ATTRIBUTES.  If
executed on a VM, KVM_CAP_MEMORY_ATTRIBUTES precisely returns the attributes
supported by that VM.  If executed at system scope, KVM_CAP_MEMORY_ATTRIBUTES
returns all attributes supported by KVM.  The only attribute defined at this
time is KVM_MEMORY_ATTRIBUTE_PRIVATE, which marks the associated gfn as being
guest private memory.
(address 与 size 必须按页对齐。可以通过在 KVM_CAP_MEMORY_ATTRIBUTES 上执行
ioctl(KVM_CHECK_EXTENSION) 来获取受支持的属性。如果在 VM 上执行，
KVM_CAP_MEMORY_ATTRIBUTES 精确返回该 VM 所支持的属性；如果在系统范围执行，则返回
KVM 支持的全部属性。目前定义的唯一属性是 KVM_MEMORY_ATTRIBUTE_PRIVATE，它把相应
的 gfn 标记为客户机私有内存。)

Note, there is no "get" API.  Userspace is responsible for explicitly tracking
the state of a gfn/page as needed.
(注意，不存在 "get" API。用户态负责按需显式跟踪 gfn/页的状态。)

The "flags" field is reserved for future extensions and must be '0'.
("flags" 字段保留供将来扩展使用，必须为 '0'。)

4.142 KVM_CREATE_GUEST_MEMFD(4.142 KVM_CREATE_GUEST_MEMFD)
----------------------------------------------------------

:Capability: KVM_CAP_GUEST_MEMFD
:Architectures: none
:Type: vm ioctl
:Parameters: struct kvm_create_guest_memfd(in)
:Returns: A file descriptor on success, <0 on error

(能力：KVM_CAP_GUEST_MEMFD；架构：无；类型：vm ioctl；参数：struct
kvm_create_guest_memfd(in)；返回值：成功时为一个文件描述符，出错时小于 0。)

KVM_CREATE_GUEST_MEMFD creates an anonymous file and returns a file descriptor
that refers to it.  guest_memfd files are roughly analogous to files created
via memfd_create(), e.g. guest_memfd files live in RAM, have volatile storage,
and are automatically released when the last reference is dropped.  Unlike
"regular" memfd_create() files, guest_memfd files are bound to their owning
virtual machine (see below), cannot be mapped, read, or written by userspace,
and cannot be resized  (guest_memfd files do however support PUNCH_HOLE).
(KVM_CREATE_GUEST_MEMFD 创建一个匿名文件，并返回指向它的文件描述符。guest_memfd
文件大致类似于通过 memfd_create() 创建的文件，例如 guest_memfd 文件驻留在 RAM 中、
其存储是易失的，并且当最后一个引用被释放时会自动释放。与 "常规" 的 memfd_create()
文件不同，guest_memfd 文件绑定到其所属的虚拟机(见下文)，不能被用户态映射、读取或
写入，也不能调整大小(不过 guest_memfd 文件支持 PUNCH_HOLE)。)

::

  struct kvm_create_guest_memfd {
	__u64 size;
	__u64 flags;
	__u64 reserved[6];
  };

Conceptually, the inode backing a guest_memfd file represents physical memory,
i.e. is coupled to the virtual machine as a thing, not to a "struct kvm".  The
file itself, which is bound to a "struct kvm", is that instance's view of the
underlying memory, e.g. effectively provides the translation of guest addresses
to host memory.  This allows for use cases where multiple KVM structures are
used to manage a single virtual machine, e.g. when performing intrahost
migration of a virtual machine.
(从概念上讲，guest_memfd 文件背后的 inode 代表物理内存，即它与虚拟机这一实体耦合，
而不与 "struct kvm" 耦合。文件本身则绑定到一个 "struct kvm"，是该实例对底层内存的
视图，例如有效地提供从客户机地址到宿主机内存的翻译。这使得由多个 KVM 结构管理同一
台虚拟机的用例成为可能，例如在宿主机内部迁移虚拟机时。)

KVM currently only supports mapping guest_memfd via KVM_SET_USER_MEMORY_REGION2,
and more specifically via the guest_memfd and guest_memfd_offset fields in
"struct kvm_userspace_memory_region2", where guest_memfd_offset is the offset
into the guest_memfd instance.  For a given guest_memfd file, there can be at
most one mapping per page, i.e. binding multiple memory regions to a single
guest_memfd range is not allowed (any number of memory regions can be bound to
a single guest_memfd file, but the bound ranges must not overlap).
(KVM 目前只支持通过 KVM_SET_USER_MEMORY_REGION2 映射 guest_memfd，更具体地说，是
通过 "struct kvm_userspace_memory_region2" 中的 guest_memfd 与 guest_memfd_offset
字段来映射，其中 guest_memfd_offset 是 guest_memfd 实例内的偏移量。对给定的
guest_memfd 文件，每页最多只能有一个映射，即不允许把多个内存区域绑定到同一个
guest_memfd 范围(可以把任意数量的内存区域绑定到一个 guest_memfd 文件，但绑定的范
围不得重叠)。)

The capability KVM_CAP_GUEST_MEMFD_FLAGS enumerates the `flags` that can be
specified via KVM_CREATE_GUEST_MEMFD.  Currently defined flags:
(能力 KVM_CAP_GUEST_MEMFD_FLAGS 枚举了可以通过 KVM_CREATE_GUEST_MEMFD 指定的
flags。当前定义的标志：)

  ============================ ================================================
  GUEST_MEMFD_FLAG_MMAP        Enable using mmap() on the guest_memfd file
                               descriptor.
                               (允许对 guest_memfd 文件描述符使用 mmap()。)
  GUEST_MEMFD_FLAG_INIT_SHARED Make all memory in the file shared during
                               KVM_CREATE_GUEST_MEMFD (memory files created
                               without INIT_SHARED will be marked private).
                               Shared memory can be faulted into host userspace
                               page tables. Private memory cannot.
                               (使文件中的所有内存在 KVM_CREATE_GUEST_MEMFD
                               期间即为共享(未带 INIT_SHARED 创建的内存文件将
                               被标记为私有)。共享内存可以被缺页调入宿主用户态
                               页表，私有内存则不能。)
  ============================ ================================================

When the KVM MMU performs a PFN lookup to service a guest fault and the backing
guest_memfd has the GUEST_MEMFD_FLAG_MMAP set, then the fault will always be
consumed from guest_memfd, regardless of whether it is a shared or a private
fault.
(当 KVM MMU 为处理客户机缺页而执行 PFN 查找、且后备 guest_memfd 设置了
GUEST_MEMFD_FLAG_MMAP 时，该缺页将始终从 guest_memfd 消费，无论它是共享缺页还是
私有缺页。)

See KVM_SET_USER_MEMORY_REGION2 for additional details.
(更多细节参见 KVM_SET_USER_MEMORY_REGION2。)

4.143 KVM_PRE_FAULT_MEMORY(4.143 KVM_PRE_FAULT_MEMORY)
------------------------------------------------------

:Capability: KVM_CAP_PRE_FAULT_MEMORY
:Architectures: none
:Type: vcpu ioctl
:Parameters: struct kvm_pre_fault_memory (in/out)
:Returns: 0 if at least one page is processed, < 0 on error

(能力：KVM_CAP_PRE_FAULT_MEMORY；架构：无；类型：vcpu ioctl；参数：struct
kvm_pre_fault_memory (in/out)；返回值：至少处理了一个页时为 0，出错时小于 0。)

Errors:

  ========== ===============================================================
  EINVAL     The specified `gpa` and `size` were invalid (e.g. not
             page aligned, causes an overflow, or size is zero).
             (指定的 gpa 与 size 无效(例如未按页对齐、导致溢出，或 size 为
             零)。)
  ENOENT     The specified `gpa` is outside defined memslots.
             (指定的 gpa 位于已定义 memslot 之外。)
  EINTR      An unmasked signal is pending and no page was processed.
             (有未被屏蔽的信号处于挂起状态，且没有处理任何页。)
  EFAULT     The parameter address was invalid.
             (参数地址无效。)
  EOPNOTSUPP Mapping memory for a GPA is unsupported by the
             hypervisor, and/or for the current vCPU state/mode.
             (hypervisor 不支持为某个 GPA 预映射内存，和/或当前 vCPU 状态/模
             式下不支持。)
  EIO        unexpected error conditions (also causes a WARN)
             (出现意料之外的错误情形(同时会触发一次 WARN)。)
  ========== ===============================================================

::

  struct kvm_pre_fault_memory {
	/* in/out */
	__u64 gpa;
	__u64 size;
	/* in */
	__u64 flags;
	__u64 padding[5];
  };

KVM_PRE_FAULT_MEMORY populates KVM's stage-2 page tables used to map memory
for the current vCPU state.  KVM maps memory as if the vCPU generated a
stage-2 read page fault, e.g. faults in memory as needed, but doesn't break
CoW.  However, KVM does not mark any newly created stage-2 PTE as Accessed.
(KVM_PRE_FAULT_MEMORY 填充 KVM 用于为当前 vCPU 状态映射内存的 stage-2 页表。KVM
映射内存的方式如同 vCPU 产生了 stage-2 读缺页，例如按需调入内存，但不破坏 CoW。
不过，KVM 不会把任何新建的 stage-2 PTE 标记为已访问 (Accessed)。)

In the case of confidential VM types where there is an initial set up of
private guest memory before the guest is 'finalized'/measured, this ioctl
should only be issued after completing all the necessary setup to put the
guest into a 'finalized' state so that the above semantics can be reliably
ensured.
(对于在客户机被 "定稿"(finalized)/度量之前需要先行设置私有客户机内存的机密 VM
类型，只有在完成把客户机置入 "定稿" 状态所需的全部必要设置之后，才应当发出此
ioctl，以便可靠地确保上述语义。)

In some cases, multiple vCPUs might share the page tables.  In this
case, the ioctl can be called in parallel.
(某些情况下，多个 vCPU 可能共享页表。此时可以并行调用此 ioctl。)

When the ioctl returns, the input values are updated to point to the
remaining range.  If `size` > 0 on return, the caller can just issue
the ioctl again with the same `struct kvm_map_memory` argument.
(ioctl 返回时，输入值会被更新为指向剩余范围。如果返回时 size > 0，调用者只需用
相同的 struct kvm_map_memory 参数再次发出该 ioctl 即可。)

Shadow page tables cannot support this ioctl because they
are indexed by virtual address or nested guest physical address.
Calling this ioctl when the guest is using shadow page tables (for
example because it is running a nested guest with nested page tables)
will fail with `EOPNOTSUPP` even if `KVM_CHECK_EXTENSION` reports
the capability to be present.
(影子页表无法支持此 ioctl，因为它们是按虚拟地址或嵌套客户机物理地址索引的。当客
户机正在使用影子页表时(例如因为它正在运行使用嵌套页表的嵌套客户机)调用此 ioctl
会以 EOPNOTSUPP 失败，即使 KVM_CHECK_EXTENSION 报告该能力存在也是如此。)

`flags` must currently be zero.
(目前 flags 必须为零。)

4.144 KVM_S390_KEYOP(4.144 KVM_S390_KEYOP)
------------------------------------------

:Capability: KVM_CAP_S390_KEYOP
:Architectures: s390
:Type: vm ioctl
:Parameters: struct kvm_s390_keyop (in/out)
:Returns: 0 in case of success, < 0 on error

(能力：KVM_CAP_S390_KEYOP；架构：s390；类型：vm ioctl；参数：struct
kvm_s390_keyop (in/out)；返回值：成功时为 0，出错时小于 0。)

The specified key operation is performed on the given guest address. The
previous storage key (or the relevant part thereof) will be returned in
`key`.
(在给定的客户机地址上执行指定的键操作。先前的存储键(或其相关部分)将通过 key 返
回。)

::

  struct kvm_s390_keyop {
	__u64 guest_addr;
	__u8  key;
	__u8  operation;
  };

Currently supported values for ``operation``:
(当前支持的 ``operation`` 取值：)

KVM_S390_KEYOP_ISKE
  Returns the storage key for the guest address ``guest_addr`` in ``key``.
  (在 ``key`` 中返回客户机地址 ``guest_addr`` 的存储键。)

KVM_S390_KEYOP_RRBE
  Resets the reference bit for the guest address ``guest_addr``, returning the
  R and C bits of the old storage key in ``key``; the remaining fields of
  the storage key will be set to 0.
  (复位客户机地址 ``guest_addr`` 的引用位，在 ``key`` 中返回旧存储键的 R 与 C 位；
  存储键的其余字段将被置为 0。)

KVM_S390_KEYOP_SSKE
  Sets the storage key for the guest address ``guest_addr`` to the key
  specified in ``key``, returning the previous value in ``key``.
  (把客户机地址 ``guest_addr`` 的存储键设置为 ``key`` 中指定的键，并在 ``key`` 中
  返回先前的值。)

.. _kvm_run:

5. The kvm_run structure(5. kvm_run 结构)
=========================================

Application code obtains a pointer to the kvm_run structure by
mmap()ing a vcpu fd.  From that point, application code can control
execution by changing fields in kvm_run prior to calling the KVM_RUN
ioctl, and obtain information about the reason KVM_RUN returned by
looking up structure members.
(应用程序代码通过对 vcpu fd 执行 mmap() 来获得指向 kvm_run 结构的指针。从那一刻
起，应用程序代码可以在调用 KVM_RUN ioctl 之前修改 kvm_run 中的字段来控制执行，并
通过查询结构成员来获得 KVM_RUN 返回原因的相关信息。)

::

  struct kvm_run {
	/* in */
	__u8 request_interrupt_window;

Request that KVM_RUN return when it becomes possible to inject external
interrupts into the guest.  Useful in conjunction with KVM_INTERRUPT.
(请求 KVM_RUN 在可以向客户机注入外部中断时返回。与 KVM_INTERRUPT 配合使用很有
用。)

::

	__u8 immediate_exit;

This field is polled once when KVM_RUN starts; if non-zero, KVM_RUN
exits immediately, returning -EINTR.  In the common scenario where a
signal is used to "kick" a VCPU out of KVM_RUN, this field can be used
to avoid usage of KVM_SET_SIGNAL_MASK, which has worse scalability.
Rather than blocking the signal outside KVM_RUN, userspace can set up
a signal handler that sets run->immediate_exit to a non-zero value.
(KVM_RUN 启动时会轮询此字段一次；如果非零，KVM_RUN 会立即退出并返回 -EINTR。在常
见场景中，信号被用来把 VCPU 从 KVM_RUN 中 "踢" 出来，此时可以使用此字段来避免使
用扩展性较差的 KVM_SET_SIGNAL_MASK。与其在 KVM_RUN 之外阻塞信号，用户态可以设置
一个信号处理函数，把 run->immediate_exit 置为非零值。)

This field is ignored if KVM_CAP_IMMEDIATE_EXIT is not available.
(如果 KVM_CAP_IMMEDIATE_EXIT 不可用，此字段将被忽略。)

::

	__u8 padding1[6];

	/* out */
	__u32 exit_reason;

When KVM_RUN has returned successfully (return value 0), this informs
application code why KVM_RUN has returned.  Allowable values for this
field are detailed below.
(当 KVM_RUN 成功返回(返回值 0)时，此字段告知应用程序代码 KVM_RUN 返回的原因。此
字段允许的取值详见下文。)

::

	__u8 ready_for_interrupt_injection;

If request_interrupt_window has been specified, this field indicates
an interrupt can be injected now with KVM_INTERRUPT.
(如果已指定 request_interrupt_window，此字段表示现在可以用 KVM_INTERRUPT 注入中
断。)

::

	__u8 if_flag;

The value of the current interrupt flag.  Only valid if in-kernel
local APIC is not used.
(当前中断标志的值。仅在不使用内核内本地 APIC 时有效。)

::

	__u16 flags;

More architecture-specific flags detailing state of the VCPU that may
affect the device's behavior. Current defined flags::

  /* x86, set if the VCPU is in system management mode */
  #define KVM_RUN_X86_SMM          (1 << 0)
  /* x86, set if bus lock detected in VM */
  #define KVM_RUN_X86_BUS_LOCK     (1 << 1)
  /* x86, set if the VCPU is executing a nested (L2) guest */
  #define KVM_RUN_X86_GUEST_MODE   (1 << 2)

  /* arm64, set for KVM_EXIT_DEBUG */
  #define KVM_DEBUG_ARCH_HSR_HIGH_VALID  (1 << 0)

(更多描述 VCPU 状态、可能影响设备行为的架构特定标志。当前定义的标志如下：)

::

	/* in (pre_kvm_run), out (post_kvm_run) */
	__u64 cr8;

The value of the cr8 register.  Only valid if in-kernel local APIC is
not used.  Both input and output.
(cr8 寄存器的值。仅在不使用内核内本地 APIC 时有效。既是输入也是输出。)

::

	__u64 apic_base;

The value of the APIC BASE msr.  Only valid if in-kernel local
APIC is not used.  Both input and output.
(APIC BASE MSR 的值。仅在不使用内核内本地 APIC 时有效。既是输入也是输出。)

::

	union {
		/* KVM_EXIT_UNKNOWN */
		struct {
			__u64 hardware_exit_reason;
		} hw;

If exit_reason is KVM_EXIT_UNKNOWN, the vcpu has exited due to unknown
reasons.  Further architecture-specific information is available in
hardware_exit_reason.
(如果 exit_reason 为 KVM_EXIT_UNKNOWN，则 vcpu 因未知原因退出。更多架构相关信息
可从 hardware_exit_reason 获得。)

::

		/* KVM_EXIT_FAIL_ENTRY */
		struct {
			__u64 hardware_entry_failure_reason;
			__u32 cpu; /* if KVM_LAST_CPU */
		} fail_entry;

If exit_reason is KVM_EXIT_FAIL_ENTRY, the vcpu could not be run due
to unknown reasons.  Further architecture-specific information is
available in hardware_entry_failure_reason.
(如果 exit_reason 为 KVM_EXIT_FAIL_ENTRY，则 vcpu 因未知原因而无法运行。更多架构
相关信息可从 hardware_entry_failure_reason 获得。)

::

		/* KVM_EXIT_EXCEPTION */
		struct {
			__u32 exception;
			__u32 error_code;
		} ex;

Unused.
(未使用。)

::

		/* KVM_EXIT_IO */
		struct {
  #define KVM_EXIT_IO_IN  0
  #define KVM_EXIT_IO_OUT 1
			__u8 direction;
			__u8 size; /* bytes */
			__u16 port;
			__u32 count;
			__u64 data_offset; /* relative to kvm_run start */
		} io;

If exit_reason is KVM_EXIT_IO, then the vcpu has
executed a port I/O instruction which could not be satisfied by kvm.
data_offset describes where the data is located (KVM_EXIT_IO_OUT) or
where kvm expects application code to place the data for the next
KVM_RUN invocation (KVM_EXIT_IO_IN).  Data format is a packed array.
(如果 exit_reason 为 KVM_EXIT_IO，则 vcpu 执行了一条 kvm 无法满足的端口 I/O 指令。
data_offset 描述数据所在的位置 (KVM_EXIT_IO_OUT)，或 kvm 期望应用程序代码为下一
次 KVM_RUN 调用放置数据的位置 (KVM_EXIT_IO_IN)。数据格式为紧凑数组。)

::

		/* KVM_EXIT_DEBUG */
		struct {
			struct kvm_debug_exit_arch arch;
		} debug;

If the exit_reason is KVM_EXIT_DEBUG, then a vcpu is processing a debug event
for which architecture specific information is returned.
(如果 exit_reason 为 KVM_EXIT_DEBUG，则 vcpu 正在处理一个调试事件，并返回架构特
定的信息。)

::

		/* KVM_EXIT_MMIO */
		struct {
			__u64 phys_addr;
			__u8  data[8];
			__u32 len;
			__u8  is_write;
		} mmio;

If exit_reason is KVM_EXIT_MMIO, then the vcpu has
executed a memory-mapped I/O instruction which could not be satisfied
by kvm.  The 'data' member contains the written data if 'is_write' is
true, and should be filled by application code otherwise.
(如果 exit_reason 为 KVM_EXIT_MMIO，则 vcpu 执行了一条 kvm 无法满足的内存映射
I/O 指令。'is_write' 为真时，'data' 成员包含已写入的数据；否则应由应用程序代码
填充。)

The 'data' member contains, in its first 'len' bytes, the value as it would
appear if the VCPU performed a load or store of the appropriate width directly
to the byte array.
('data' 成员的前 'len' 个字节包含这样的值：如果 VCPU 直接对该字节数组执行相应宽
度的加载或存储，它看起来就是这个值。)

.. note::

      For KVM_EXIT_IO, KVM_EXIT_MMIO, KVM_EXIT_OSI, KVM_EXIT_PAPR, KVM_EXIT_XEN,
      KVM_EXIT_EPR, KVM_EXIT_HYPERCALL, KVM_EXIT_TDX,
      KVM_EXIT_X86_RDMSR and KVM_EXIT_X86_WRMSR the corresponding
      operations are complete (and guest state is consistent) only after userspace
      has re-entered the kernel with KVM_RUN.  The kernel side will first finish
      incomplete operations and then check for pending signals.
      (注意：对于 KVM_EXIT_IO、KVM_EXIT_MMIO、KVM_EXIT_OSI、KVM_EXIT_PAPR、
      KVM_EXIT_XEN、KVM_EXIT_EPR、KVM_EXIT_HYPERCALL、KVM_EXIT_TDX、
      KVM_EXIT_X86_RDMSR 与 KVM_EXIT_X86_WRMSR，只有当用户态通过 KVM_RUN 重新进入
      内核之后，相应操作才算完成(且客户机状态保持一致)。内核侧会先完成未完成的操
      作，然后检查是否有挂起的信号。)

      The pending state of the operation is not preserved in state which is
      visible to userspace, thus userspace should ensure that the operation is
      completed before performing a live migration.  Userspace can re-enter the
      guest with an unmasked signal pending or with the immediate_exit field set
      to complete pending operations without allowing any further instructions
      to be executed.
      (操作的挂起状态不会保存在用户态可见的状态中，因此用户态应当确保在执行热迁移
      之前完成该操作。用户态可以在有未屏蔽信号挂起、或 immediate_exit 字段已置位的
      情况下重新进入客户机，以完成挂起的操作而不让任何后续指令被执行。)

::

		/* KVM_EXIT_HYPERCALL */
		struct {
			__u64 nr;
			__u64 args[6];
			__u64 ret;
			__u64 flags;
		} hypercall;


It is strongly recommended that userspace use ``KVM_EXIT_IO`` (x86) or
``KVM_EXIT_MMIO`` (all except s390) to implement functionality that
requires a guest to interact with host userspace.
(强烈建议用户态使用 ``KVM_EXIT_IO`` (x86) 或 ``KVM_EXIT_MMIO`` (除 s390 之外的所
有架构) 来实现需要客户机与宿主用户态交互的功能。)

.. note:: KVM_EXIT_IO is significantly faster than KVM_EXIT_MMIO.
        (注意：KVM_EXIT_IO 比 KVM_EXIT_MMIO 快得多。)

For arm64:(针对 arm64)
----------------------

SMCCC exits can be enabled depending on the configuration of the SMCCC
filter. See the Documentation/virt/kvm/devices/vm.rst
``KVM_ARM_SMCCC_FILTER`` for more details.
(根据 SMCCC 过滤器的配置，可以启用 SMCCC 退出。更多细节参见
Documentation/virt/kvm/devices/vm.rst 中的 ``KVM_ARM_SMCCC_FILTER``。)

``nr`` contains the function ID of the guest's SMCCC call. Userspace is
expected to use the ``KVM_GET_ONE_REG`` ioctl to retrieve the call
parameters from the vCPU's GPRs.
(``nr`` 包含客户机 SMCCC 调用的函数 ID。用户态应当使用 ``KVM_GET_ONE_REG`` ioctl
从 vCPU 的 GPR 中取回调用参数。)

Definition of ``flags``:
(``flags`` 的定义：)

 - ``KVM_HYPERCALL_EXIT_SMC``: Indicates that the guest used the SMC
   conduit to initiate the SMCCC call. If this bit is 0 then the guest
   used the HVC conduit for the SMCCC call.
   (``KVM_HYPERCALL_EXIT_SMC``：表示客户机使用 SMC 通道发起 SMCCC 调用。如果此位
   为 0，则客户机使用 HVC 通道发起该 SMCCC 调用。)

 - ``KVM_HYPERCALL_EXIT_16BIT``: Indicates that the guest used a 16bit
   instruction to initiate the SMCCC call. If this bit is 0 then the
   guest used a 32bit instruction. An AArch64 guest always has this
   bit set to 0.
   (``KVM_HYPERCALL_EXIT_16BIT``：表示客户机使用 16 位指令发起 SMCCC 调用。如果
   此位为 0，则客户机使用 32 位指令。AArch64 客户机的该位始终为 0。)

At the point of exit, PC points to the instruction immediately following
the trapping instruction.
(退出时，PC 指向陷入 (trapping) 指令之后紧邻的指令。)

::

		/* KVM_EXIT_TPR_ACCESS */
		struct {
			__u64 rip;
			__u32 is_write;
			__u32 pad;
		} tpr_access;

To be documented (KVM_TPR_ACCESS_REPORTING).
(待补充文档 (KVM_TPR_ACCESS_REPORTING)。)

::

		/* KVM_EXIT_S390_SIEIC */
		struct {
			__u8 icptcode;
			__u64 mask; /* psw upper half */
			__u64 addr; /* psw lower half */
			__u16 ipa;
			__u32 ipb;
		} s390_sieic;

s390 specific.
(s390 专用。)

::

		/* KVM_EXIT_S390_RESET */
  #define KVM_S390_RESET_POR       1
  #define KVM_S390_RESET_CLEAR     2
  #define KVM_S390_RESET_SUBSYSTEM 4
  #define KVM_S390_RESET_CPU_INIT  8
  #define KVM_S390_RESET_IPL       16
		__u64 s390_reset_flags;

s390 specific.
(s390 专用。)

::

		/* KVM_EXIT_S390_UCONTROL */
		struct {
			__u64 trans_exc_code;
			__u32 pgm_code;
		} s390_ucontrol;

s390 specific. A page fault has occurred for a user controlled virtual
machine (KVM_VM_S390_UNCONTROL) on its host page table that cannot be
resolved by the kernel.
The program code and the translation exception code that were placed
in the cpu's lowcore are presented here as defined by the z Architecture
Principles of Operation Book in the Chapter for Dynamic Address Translation
(DAT)
(s390 专用。用户控制的虚拟机 (KVM_VM_S390_UNCONTROL) 在其宿主页表上发生了一次内
核无法解决的缺页。这里呈现的程序代码 (program code) 与翻译异常代码 (translation
exception code)，按照 z 架构《操作原理》(Principles of Operation) 一书中 "动态地
址翻译 (DAT)" 一章的定义，放置在 cpu 的 lowcore 中。)

::

		/* KVM_EXIT_DCR */
		struct {
			__u32 dcrn;
			__u32 data;
			__u8  is_write;
		} dcr;

Deprecated - was used for 440 KVM.
(已废弃——曾被用于 440 KVM。)

::

		/* KVM_EXIT_OSI */
		struct {
			__u64 gprs[32];
		} osi;

MOL uses a special hypercall interface it calls 'OSI'. To enable it, we catch
hypercalls and exit with this exit struct that contains all the guest gprs.
(MOL 使用一种它称之为 'OSI' 的特殊超级调用接口。为了启用它，我们会拦截超级调用，
并以这个包含所有客户机 gpr 的退出结构退出。)

If exit_reason is KVM_EXIT_OSI, then the vcpu has triggered such a hypercall.
Userspace can now handle the hypercall and when it's done modify the gprs as
necessary. Upon guest entry all guest GPRs will then be replaced by the values
in this struct.
(如果 exit_reason 为 KVM_EXIT_OSI，则 vcpu 触发了这样一个超级调用。用户态此时可
以处理该超级调用，完成后按需修改 gprs。再次进入客户机时，所有客户机 GPR 都将被
此结构中的值替换。)

::

		/* KVM_EXIT_PAPR_HCALL */
		struct {
			__u64 nr;
			__u64 ret;
			__u64 args[9];
		} papr_hcall;

This is used on 64-bit PowerPC when emulating a pSeries partition,
e.g. with the 'pseries' machine type in qemu.  It occurs when the
guest does a hypercall using the 'sc 1' instruction.  The 'nr' field
contains the hypercall number (from the guest R3), and 'args' contains
the arguments (from the guest R4 - R12).  Userspace should put the
return code in 'ret' and any extra returned values in args[].
The possible hypercalls are defined in the Power Architecture Platform
Requirements (PAPR) document available from www.power.org (free
developer registration required to access it).
(在 64 位 PowerPC 上模拟 pSeries 分区时使用，例如在 qemu 中使用 'pseries' 机型时。
当客户机使用 'sc 1' 指令发出超级调用时会发生此退出。'nr' 字段包含超级调用编号(来
自客户机 R3)，'args' 包含参数(来自客户机 R4 - R12)。用户态应把返回码放入 'ret'，
把其他额外返回值放入 args[]。可能的超级调用定义于可从 www.power.org 获取的 Power
Architecture Platform Requirements (PAPR) 文档中(访问该文档需要免费注册开发者账
号)。)

::

		/* KVM_EXIT_S390_TSCH */
		struct {
			__u16 subchannel_id;
			__u16 subchannel_nr;
			__u32 io_int_parm;
			__u32 io_int_word;
			__u32 ipb;
			__u8 dequeued;
		} s390_tsch;

s390 specific. This exit occurs when KVM_CAP_S390_CSS_SUPPORT has been enabled
and TEST SUBCHANNEL was intercepted. If dequeued is set, a pending I/O
interrupt for the target subchannel has been dequeued and subchannel_id,
subchannel_nr, io_int_parm and io_int_word contain the parameters for that
interrupt. ipb is needed for instruction parameter decoding.
(s390 专用。当 KVM_CAP_S390_CSS_SUPPORT 已启用且 TEST SUBCHANNEL 被拦截时会发生
此退出。如果设置了 dequeued，则表示目标子通道的一个挂起 I/O 中断已被出队，
subchannel_id、subchannel_nr、io_int_parm 与 io_int_word 包含该中断的参数。ipb 用
于指令参数解码。)

::

		/* KVM_EXIT_EPR */
		struct {
			__u32 epr;
		} epr;

On FSL BookE PowerPC chips, the interrupt controller has a fast patch
interrupt acknowledge path to the core. When the core successfully
delivers an interrupt, it automatically populates the EPR register with
the interrupt vector number and acknowledges the interrupt inside
the interrupt controller.
(在 FSL BookE PowerPC 芯片上，中断控制器有一条通往核心的快速中断确认路径。当核心
成功投递一个中断时，它会自动把中断向量号填入 EPR 寄存器，并在中断控制器内部确认
该中断。)

In case the interrupt controller lives in user space, we need to do
the interrupt acknowledge cycle through it to fetch the next to be
delivered interrupt vector using this exit.
(如果中断控制器位于用户态，我们就需要通过它完成中断确认周期，并使用此退出来取出
下一个待投递的中断向量。)

It gets triggered whenever both KVM_CAP_PPC_EPR are enabled and an
external interrupt has just been delivered into the guest. User space
should put the acknowledged interrupt vector into the 'epr' field.
(只要 KVM_CAP_PPC_EPR 已启用且一个外部中断刚刚被投递给客户机，就会触发此退出。用
户态应把已确认的中断向量放入 'epr' 字段。)

::

		/* KVM_EXIT_SYSTEM_EVENT */
		struct {
  #define KVM_SYSTEM_EVENT_SHUTDOWN       1
  #define KVM_SYSTEM_EVENT_RESET          2
  #define KVM_SYSTEM_EVENT_CRASH          3
  #define KVM_SYSTEM_EVENT_WAKEUP         4
  #define KVM_SYSTEM_EVENT_SUSPEND        5
  #define KVM_SYSTEM_EVENT_SEV_TERM       6
  #define KVM_SYSTEM_EVENT_TDX_FATAL      7
			__u32 type;
                        __u32 ndata;
                        __u64 data[16];
		} system_event;

If exit_reason is KVM_EXIT_SYSTEM_EVENT then the vcpu has triggered
a system-level event using some architecture specific mechanism (hypercall
or some special instruction). In case of ARM64, this is triggered using
HVC instruction based PSCI call from the vcpu.
(如果 exit_reason 为 KVM_EXIT_SYSTEM_EVENT，则 vcpu 使用某种架构特定的机制(超级调
用或某些特殊指令)触发了一个系统级事件。在 ARM64 上，这通过 vcpu 发出的基于 HVC
指令的 PSCI 调用触发。)

The 'type' field describes the system-level event type.
Valid values for 'type' are:
('type' 字段描述系统级事件的类型。'type' 的有效取值为：)

 - KVM_SYSTEM_EVENT_SHUTDOWN -- the guest has requested a shutdown of the
   VM. Userspace is not obliged to honour this, and if it does honour
   this does not need to destroy the VM synchronously (ie it may call
   KVM_RUN again before shutdown finally occurs).
   (KVM_SYSTEM_EVENT_SHUTDOWN -- 客户机请求关闭 VM。用户态没有义务照办；即使照
   办，也不需要同步销毁 VM(即可以在关闭最终发生之前再次调用 KVM_RUN)。)
 - KVM_SYSTEM_EVENT_RESET -- the guest has requested a reset of the VM.
   As with SHUTDOWN, userspace can choose to ignore the request, or
   to schedule the reset to occur in the future and may call KVM_RUN again.
   (KVM_SYSTEM_EVENT_RESET -- 客户机请求复位 VM。与 SHUTDOWN 一样，用户态可以选
   择忽略该请求，或安排复位在未来发生，并可以再次调用 KVM_RUN。)
 - KVM_SYSTEM_EVENT_CRASH -- the guest crash occurred and the guest
   has requested a crash condition maintenance. Userspace can choose
   to ignore the request, or to gather VM memory core dump and/or
   reset/shutdown of the VM.
   (KVM_SYSTEM_EVENT_CRASH -- 发生了客户机崩溃，客户机请求进行崩溃状态维护。用户
   态可以选择忽略该请求，或者收集 VM 内存核心转储和/或复位/关闭 VM。)
 - KVM_SYSTEM_EVENT_SEV_TERM -- an AMD SEV guest requested termination.
   The guest physical address of the guest's GHCB is stored in `data[0]`.
   (KVM_SYSTEM_EVENT_SEV_TERM -- AMD SEV 客户机请求终止。客户机 GHCB 的客户机物
   理地址存储在 data[0] 中。)
 - KVM_SYSTEM_EVENT_TDX_FATAL -- a TDX guest reported a fatal error state.
   KVM doesn't do any parsing or conversion, it just dumps 16 general-purpose
   registers to userspace, in ascending order of the 4-bit indices for x86-64
   general-purpose registers in instruction encoding, as defined in the Intel
   SDM.
   (KVM_SYSTEM_EVENT_TDX_FATAL -- TDX 客户机报告了一个致命错误状态。KVM 不做任何
   解析或转换，只是按照 Intel SDM 中指令编码里 x86-64 通用寄存器 4 位索引的升序，
   把 16 个通用寄存器转储给用户态。)
 - KVM_SYSTEM_EVENT_WAKEUP -- the exiting vCPU is in a suspended state and
   KVM has recognized a wakeup event. Userspace may honor this event by
   marking the exiting vCPU as runnable, or deny it and call KVM_RUN again.
   (KVM_SYSTEM_EVENT_WAKEUP -- 退出的 vCPU 处于挂起状态，KVM 识别到了一个唤醒事
   件。用户态可以通过把退出的 vCPU 标记为可运行来响应此事件，也可以拒绝它并再次
   调用 KVM_RUN。)
 - KVM_SYSTEM_EVENT_SUSPEND -- the guest has requested a suspension of
   the VM.
   (KVM_SYSTEM_EVENT_SUSPEND -- 客户机请求挂起 VM。)

If KVM_CAP_SYSTEM_EVENT_DATA is present, the 'data' field can contain
architecture specific information for the system-level event.  Only
the first `ndata` items (possibly zero) of the data array are valid.
(如果存在 KVM_CAP_SYSTEM_EVENT_DATA，'data' 字段可以包含系统级事件的架构特定信
息。数据数组中只有前 ndata 项(可能为零)是有效的。)

 - for arm64, data[0] is set to KVM_SYSTEM_EVENT_RESET_FLAG_PSCI_RESET2 if
   the guest issued a SYSTEM_RESET2 call according to v1.1 of the PSCI
   specification.
   (对于 arm64，如果客户机按照 PSCI 规范 v1.1 发出了 SYSTEM_RESET2 调用，
   data[0] 会被设置为 KVM_SYSTEM_EVENT_RESET_FLAG_PSCI_RESET2。)

 - for arm64, data[0] is set to KVM_SYSTEM_EVENT_SHUTDOWN_FLAG_PSCI_OFF2
   if the guest issued a SYSTEM_OFF2 call according to v1.3 of the PSCI
   specification.
   (对于 arm64，如果客户机按照 PSCI 规范 v1.3 发出了 SYSTEM_OFF2 调用，data[0]
   会被设置为 KVM_SYSTEM_EVENT_SHUTDOWN_FLAG_PSCI_OFF2。)

 - for RISC-V, data[0] is set to the value of the second argument of the
   ``sbi_system_reset`` call.
   (对于 RISC-V，data[0] 被设置为 ``sbi_system_reset`` 调用第二个参数的值。)

Previous versions of Linux defined a `flags` member in this struct.  The
field is now aliased to `data[0]`.  Userspace can assume that it is only
written if ndata is greater than 0.
(Linux 的先前版本在此结构中定义了一个 flags 成员。该字段现在是 data[0] 的别名。
用户态可以假定只有当 ndata 大于 0 时它才会被写入。)

For arm/arm64:(针对 arm/arm64)
------------------------------

KVM_SYSTEM_EVENT_SUSPEND exits are enabled with the
KVM_CAP_ARM_SYSTEM_SUSPEND VM capability. If a guest invokes the PSCI
SYSTEM_SUSPEND function, KVM will exit to userspace with this event
type.
(KVM_SYSTEM_EVENT_SUSPEND 退出通过 KVM_CAP_ARM_SYSTEM_SUSPEND VM 能力启用。如果
客户机调用 PSCI SYSTEM_SUSPEND 函数，KVM 将以此事件类型退出到用户态。)

It is the sole responsibility of userspace to implement the PSCI
SYSTEM_SUSPEND call according to ARM DEN0022D.b 5.19 "SYSTEM_SUSPEND".
KVM does not change the vCPU's state before exiting to userspace, so
the call parameters are left in-place in the vCPU registers.
(按照 ARM DEN0022D.b 5.19 "SYSTEM_SUSPEND" 实现 PSCI SYSTEM_SUSPEND 调用完全是
用户态的职责。KVM 在退出到用户态之前不会改变 vCPU 的状态，因此调用参数原样保留在
vCPU 寄存器中。)

Userspace is _required_ to take action for such an exit. It must
either:
(用户态必须对这类退出采取行动。它必须二选一：)

 - Honor the guest request to suspend the VM. Userspace can request
   in-kernel emulation of suspension by setting the calling vCPU's
   state to KVM_MP_STATE_SUSPENDED. Userspace must configure the vCPU's
   state according to the parameters passed to the PSCI function when
   the calling vCPU is resumed. See ARM DEN0022D.b 5.19.1 "Intended use"
   for details on the function parameters.
   (满足客户机挂起 VM 的请求。用户态可以通过把发起调用的 vCPU 状态设置为
   KVM_MP_STATE_SUSPENDED 来请求内核内模拟挂起。当发起调用的 vCPU 恢复运行时，用
   户态必须根据传递给 PSCI 函数的参数来配置 vCPU 状态。函数参数的细节参见 ARM
   DEN0022D.b 5.19.1 "Intended use"。)

 - Deny the guest request to suspend the VM. See ARM DEN0022D.b 5.19.2
   "Caller responsibilities" for possible return values.
   (拒绝客户机挂起 VM 的请求。可能的返回值参见 ARM DEN0022D.b 5.19.2 "Caller
   responsibilities"。)

Hibernation using the PSCI SYSTEM_OFF2 call is enabled when PSCI v1.3
is enabled. If a guest invokes the PSCI SYSTEM_OFF2 function, KVM will
exit to userspace with the KVM_SYSTEM_EVENT_SHUTDOWN event type and with
data[0] set to KVM_SYSTEM_EVENT_SHUTDOWN_FLAG_PSCI_OFF2. The only
supported hibernate type for the SYSTEM_OFF2 function is HIBERNATE_OFF.
(当启用 PSCI v1.3 时，即启用了使用 PSCI SYSTEM_OFF2 调用的休眠。如果客户机调用
PSCI SYSTEM_OFF2 函数，KVM 将以 KVM_SYSTEM_EVENT_SHUTDOWN 事件类型退出到用户态，
且 data[0] 被设置为 KVM_SYSTEM_EVENT_SHUTDOWN_FLAG_PSCI_OFF2。SYSTEM_OFF2 函数支
持的唯一休眠类型是 HIBERNATE_OFF。)

::

		/* KVM_EXIT_IOAPIC_EOI */
		struct {
			__u8 vector;
		} eoi;

Indicates that the VCPU's in-kernel local APIC received an EOI for a
level-triggered IOAPIC interrupt.  This exit only triggers when the
IOAPIC is implemented in userspace (i.e. KVM_CAP_SPLIT_IRQCHIP is enabled);
the userspace IOAPIC should process the EOI and retrigger the interrupt if
it is still asserted.  Vector is the LAPIC interrupt vector for which the
EOI was received.
(表示 VCPU 的内核内本地 APIC 收到了针对某个电平触发 IOAPIC 中断的 EOI。此退出只
在 IOAPIC 由用户态实现(即启用了 KVM_CAP_SPLIT_IRQCHIP)时触发；用户态 IOAPIC 应处
理该 EOI，若中断仍处于有效状态则重新触发它。vector 是收到 EOI 的 LAPIC 中断向
量。)

::

		struct kvm_hyperv_exit {
  #define KVM_EXIT_HYPERV_SYNIC          1
  #define KVM_EXIT_HYPERV_HCALL          2
  #define KVM_EXIT_HYPERV_SYNDBG         3
			__u32 type;
			__u32 pad1;
			union {
				struct {
					__u32 msr;
					__u32 pad2;
					__u64 control;
					__u64 evt_page;
					__u64 msg_page;
				} synic;
				struct {
					__u64 input;
					__u64 result;
					__u64 params[2];
				} hcall;
				struct {
					__u32 msr;
					__u32 pad2;
					__u64 control;
					__u64 status;
					__u64 send_page;
					__u64 recv_page;
					__u64 pending_page;
				} syndbg;
			} u;
		};
		/* KVM_EXIT_HYPERV */
                struct kvm_hyperv_exit hyperv;

Indicates that the VCPU exits into userspace to process some tasks
related to Hyper-V emulation.
(表示 VCPU 退出到用户态，以处理与 Hyper-V 模拟相关的一些任务。)

Valid values for 'type' are:
('type' 的有效取值为：)

	- KVM_EXIT_HYPERV_SYNIC -- synchronously notify user-space about

Hyper-V SynIC state change. Notification is used to remap SynIC
event/message pages and to enable/disable SynIC messages/events processing
in userspace.
(Hyper-V SynIC 状态变更的同步通知。该通知用于重新映射 SynIC 事件/消息页，以及在
用户态启用/禁用 SynIC 消息/事件的处理。)

	- KVM_EXIT_HYPERV_SYNDBG -- synchronously notify user-space about

Hyper-V Synthetic debugger state change. Notification is used to either update
the pending_page location or to send a control command (send the buffer located
in send_page or recv a buffer to recv_page).
(Hyper-V 合成调试器 (Synthetic debugger) 状态变更的同步通知。该通知用于更新
pending_page 的位置，或发送控制命令(把 send_page 中的缓冲区发送出去，或接收缓冲
区到 recv_page)。)

::

		/* KVM_EXIT_ARM_NISV / KVM_EXIT_ARM_LDST64B */
		struct {
			__u64 esr_iss;
			__u64 fault_ipa;
		} arm_nisv;

- KVM_EXIT_ARM_NISV:
  (KVM_EXIT_ARM_NISV：)

Used on arm64 systems. If a guest accesses memory not in a memslot,
KVM will typically return to userspace and ask it to do MMIO emulation on its
behalf. However, for certain classes of instructions, no instruction decode
(direction, length of memory access) is provided, and fetching and decoding
the instruction from the VM is overly complicated to live in the kernel.
(用于 arm64 系统。如果客户机访问了不在 memslot 中的内存，KVM 通常会返回用户态，
请它代为进行 MMIO 模拟。然而，对某几类指令而言，内核不会提供指令解码信息(方向、
内存访问长度)，而从 VM 中取指并解码又过于复杂，不适合放在内核里。)

Historically, when this situation occurred, KVM would print a warning and kill
the VM. KVM assumed that if the guest accessed non-memslot memory, it was
trying to do I/O, which just couldn't be emulated, and the warning message was
phrased accordingly. However, what happened more often was that a guest bug
caused access outside the guest memory areas which should lead to a more
meaningful warning message and an external abort in the guest, if the access
did not fall within an I/O window.
(在过去，发生这种情况时 KVM 会打印一条警告并杀死 VM。KVM 假定：如果客户机访问了
非 memslot 内存，它就是在尝试做 I/O，只是无法模拟，警告消息也据此措辞。但更常发
生的情况是：客户机 bug 导致访问越出了客户机内存区域——如果该访问没有落在 I/O 窗
口内，这应当产生一条更有意义的警告消息，并在客户机内触发外部中止 (external
abort)。)

Userspace implementations can query for KVM_CAP_ARM_NISV_TO_USER, and enable
this capability at VM creation. Once this is done, these types of errors will
instead return to userspace with KVM_EXIT_ARM_NISV, with the valid bits from
the ESR_EL2 in the esr_iss field, and the faulting IPA in the fault_ipa field.
Userspace can either fix up the access if it's actually an I/O access by
decoding the instruction from guest memory (if it's very brave) and continue
executing the guest, or it can decide to suspend, dump, or restart the guest.
(用户态实现可以查询 KVM_CAP_ARM_NISV_TO_USER，并在创建 VM 时启用该能力。启用之
后，这类错误将改为以 KVM_EXIT_ARM_NISV 返回用户态：esr_iss 字段携带 ESR_EL2 的有
效位，fault_ipa 字段携带出错的 IPA。如果这实际上是一次 I/O 访问，用户态可以通过
从客户机内存解码该指令来修复这次访问(如果它足够大胆)并继续执行客户机；也可以决
定挂起、转储或重启客户机。)

Note that KVM does not skip the faulting instruction as it does for
KVM_EXIT_MMIO, but userspace has to emulate any change to the processing state
if it decides to decode and emulate the instruction.
(注意，KVM 不会像对 KVM_EXIT_MMIO 那样跳过出错的指令；如果用户态决定解码并模拟该
指令，则处理状态 (processing state) 的任何改变都必须由用户态来模拟。)

This feature isn't available to protected VMs, as userspace does not
have access to the state that is required to perform the emulation.
Instead, a data abort exception is directly injected in the guest.
Note that although KVM_CAP_ARM_NISV_TO_USER will be reported if
queried outside of a protected VM context, the feature will not be
exposed if queried on a protected VM file descriptor.
(受保护 VM 无法使用此特性，因为用户态无法访问执行模拟所需的状态。此时会直接向客
户机注入一个数据中止 (data abort) 异常。注意，尽管在受保护 VM 上下文之外查询时会
报告 KVM_CAP_ARM_NISV_TO_USER，但在受保护 VM 文件描述符上查询时该特性不会暴露。)

- KVM_EXIT_ARM_LDST64B:
  (KVM_EXIT_ARM_LDST64B：)

Used on arm64 systems. When a guest using a LD64B, ST64B, ST64BV, ST64BV0,
outside of a memslot, KVM will return to userspace with KVM_EXIT_ARM_LDST64B,
exposing the relevant ESR_EL2 information and faulting IPA, similarly to
KVM_EXIT_ARM_NISV.
(用于 arm64 系统。当使用 LD64B、ST64B、ST64BV、ST64BV0 的客户机在 memslot 之外
访问时，KVM 会以 KVM_EXIT_ARM_LDST64B 返回用户态，暴露相关的 ESR_EL2 信息与出错
的 IPA，与 KVM_EXIT_ARM_NISV 类似。)

Userspace is supposed to fully emulate the instructions, which includes:
(用户态应当完整模拟这些指令，包括：)

	- fetch of the operands for a store, including ACCDATA_EL1 in the case
	  of a ST64BV0 instruction
	  (为 store 取得操作数，ST64BV0 指令的情况还包括取得 ACCDATA_EL1)
	- deal with the endianness if the guest is big-endian
	  (在客户机为大端序时处理字节序)
	- emulate the access, including the delivery of an exception if the
	  access didn't succeed
	  (模拟这次访问，包括在访问未成功时投递异常)
	- provide a return value in the case of ST64BV/ST64BV0
	  (在 ST64BV/ST64BV0 的情况下提供返回值)
	- return the data in the case of a load
	  (在 load 的情况下返回数据)
	- increment PC if the instruction was successfully executed
	  (如果指令成功执行则递增 PC)

Note that there is no expectation of performance for this emulation, as it
involves a large number of interaction with the guest state. It is, however,
expected that the instruction's semantics are preserved, specially the
single-copy atomicity property of the 64 byte access.
(对这种模拟没有性能上的期望，因为它涉及与客户机状态的大量交互。不过，指令的语义
应当得到保留，特别是 64 字节访问的单拷贝原子性 (single-copy atomicity)。)

This exit reason must be handled if userspace sets ID_AA64ISAR1_EL1.LS64 to a
non-zero value, indicating that FEAT_LS64* is enabled.
(如果用户态把 ID_AA64ISAR1_EL1.LS64 设置为非零值(表示 FEAT_LS64* 已启用)，则必
须处理此退出原因。)

::

		/* KVM_EXIT_X86_RDMSR / KVM_EXIT_X86_WRMSR */
		struct {
			__u8 error; /* user -> kernel */
			__u8 pad[7];
			__u32 reason; /* kernel -> user */
			__u32 index; /* kernel -> user */
			__u64 data; /* kernel <-> user */
		} msr;

Used on x86 systems. When the VM capability KVM_CAP_X86_USER_SPACE_MSR is
enabled, MSR accesses to registers that would invoke a #GP by KVM kernel code
may instead trigger a KVM_EXIT_X86_RDMSR exit for reads and KVM_EXIT_X86_WRMSR
exit for writes.
(用于 x86 系统。当启用 VM 能力 KVM_CAP_X86_USER_SPACE_MSR 后，原本会由 KVM 内核代
码引发 #GP 的 MSR 访问，读操作可能改为触发 KVM_EXIT_X86_RDMSR 退出，写操作可能改
为触发 KVM_EXIT_X86_WRMSR 退出。)

The "reason" field specifies why the MSR interception occurred. Userspace will
only receive MSR exits when a particular reason was requested during through
ENABLE_CAP. Currently valid exit reasons are:
("reason" 字段说明发生 MSR 拦截的原因。只有当通过 ENABLE_CAP 请求了特定原因时，
用户态才会收到 MSR 退出。当前有效的退出原因如下：)

============================ ========================================
 KVM_MSR_EXIT_REASON_UNKNOWN access to MSR that is unknown to KVM
                             (访问 KVM 未知 (unknown) 的 MSR)
 KVM_MSR_EXIT_REASON_INVAL   access to invalid MSRs or reserved bits
                             (访问无效的 MSR 或保留位)
 KVM_MSR_EXIT_REASON_FILTER  access blocked by KVM_X86_SET_MSR_FILTER
                             (访问被 KVM_X86_SET_MSR_FILTER 阻止)
============================ ========================================

For KVM_EXIT_X86_RDMSR, the "index" field tells userspace which MSR the guest
wants to read. To respond to this request with a successful read, userspace
writes the respective data into the "data" field and must continue guest
execution to ensure the read data is transferred into guest register state.
(对于 KVM_EXIT_X86_RDMSR，"index" 字段告知用户态客户机想读取哪个 MSR。要以一次成
功的读取来响应该请求，用户态把相应数据写入 "data" 字段，并且必须继续执行客户机，
以确保读到的数据被传输进客户机寄存器状态。)

If the RDMSR request was unsuccessful, userspace indicates that with a "1" in
the "error" field. This will inject a #GP into the guest when the VCPU is
executed again.
(如果 RDMSR 请求失败，用户态在 "error" 字段中用 "1" 表示。VCPU 再次执行时，这会
向客户机注入一个 #GP。)

For KVM_EXIT_X86_WRMSR, the "index" field tells userspace which MSR the guest
wants to write. Once finished processing the event, userspace must continue
vCPU execution. If the MSR write was unsuccessful, userspace also sets the
"error" field to "1".
(对于 KVM_EXIT_X86_WRMSR，"index" 字段告知用户态客户机想写入哪个 MSR。处理完该事
件后，用户态必须继续 vCPU 执行。如果 MSR 写入失败，用户态还要把 "error" 字段设置
为 "1"。)

See KVM_X86_SET_MSR_FILTER for details on the interaction with MSR filtering.
(与 MSR 过滤交互的细节参见 KVM_X86_SET_MSR_FILTER。)

::


		struct kvm_xen_exit {
  #define KVM_EXIT_XEN_HCALL          1
			__u32 type;
			union {
				struct {
					__u32 longmode;
					__u32 cpl;
					__u64 input;
					__u64 result;
					__u64 params[6];
				} hcall;
			} u;
		};
		/* KVM_EXIT_XEN */
                struct kvm_hyperv_exit xen;

Indicates that the VCPU exits into userspace to process some tasks
related to Xen emulation.
(表示 VCPU 退出到用户态，以处理与 Xen 模拟相关的一些任务。)

Valid values for 'type' are:
('type' 的有效取值为：)

  - KVM_EXIT_XEN_HCALL -- synchronously notify user-space about Xen hypercall.
    Userspace is expected to place the hypercall result into the appropriate
    field before invoking KVM_RUN again.
    (KVM_EXIT_XEN_HCALL -- 同步通知用户态发生了 Xen 超级调用。用户态应在再次调用
    KVM_RUN 之前，把超级调用的结果放入相应字段。)

::

		/* KVM_EXIT_RISCV_SBI */
		struct {
			unsigned long extension_id;
			unsigned long function_id;
			unsigned long args[6];
			unsigned long ret[2];
		} riscv_sbi;

If exit reason is KVM_EXIT_RISCV_SBI then it indicates that the VCPU has
done a SBI call which is not handled by KVM RISC-V kernel module. The details
of the SBI call are available in 'riscv_sbi' member of kvm_run structure. The
'extension_id' field of 'riscv_sbi' represents SBI extension ID whereas the
'function_id' field represents function ID of given SBI extension. The 'args'
array field of 'riscv_sbi' represents parameters for the SBI call and 'ret'
array field represents return values. The userspace should update the return
values of SBI call before resuming the VCPU. For more details on RISC-V SBI
spec refer, https://github.com/riscv/riscv-sbi-doc.
(如果退出原因为 KVM_EXIT_RISCV_SBI，则表示 VCPU 执行了一次未被 KVM RISC-V 内核模
块处理的 SBI 调用。SBI 调用的细节可从 kvm_run 结构的 'riscv_sbi' 成员中获得。
'riscv_sbi' 的 'extension_id' 字段代表 SBI 扩展 ID，'function_id' 字段代表给定
SBI 扩展的函数 ID。'riscv_sbi' 的 'args' 数组字段代表 SBI 调用的参数，'ret' 数组
字段代表返回值。用户态应在恢复 VCPU 之前更新 SBI 调用的返回值。RISC-V SBI 规范的
更多细节参见 https://github.com/riscv/riscv-sbi-doc。)

::

		/* KVM_EXIT_MEMORY_FAULT */
		struct {
  #define KVM_MEMORY_EXIT_FLAG_PRIVATE	(1ULL << 3)
			__u64 flags;
			__u64 gpa;
			__u64 size;
		} memory_fault;

KVM_EXIT_MEMORY_FAULT indicates the vCPU has encountered a memory fault that
could not be resolved by KVM.  The 'gpa' and 'size' (in bytes) describe the
guest physical address range [gpa, gpa + size) of the fault.  The 'flags' field
describes properties of the faulting access that are likely pertinent:
(KVM_EXIT_MEMORY_FAULT 表示 vCPU 遇到了一个 KVM 无法解决的内存故障。'gpa' 与
'size'(以字节为单位)描述该故障的客户机物理地址范围 [gpa, gpa + size)。'flags'
字段描述此次出错访问可能相关的一些属性：)

 - KVM_MEMORY_EXIT_FLAG_PRIVATE - When set, indicates the memory fault occurred
   on a private memory access.  When clear, indicates the fault occurred on a
   shared access.
   (KVM_MEMORY_EXIT_FLAG_PRIVATE - 置位时表示该内存故障发生在私有内存访问上；清
   零时表示故障发生在共享访问上。)

Note!  KVM_EXIT_MEMORY_FAULT is unique among all KVM exit reasons in that it
accompanies a return code of '-1', not '0'!  errno will always be set to EFAULT
or EHWPOISON when KVM exits with KVM_EXIT_MEMORY_FAULT, userspace should assume
kvm_run.exit_reason is stale/undefined for all other error numbers.
(注意！KVM_EXIT_MEMORY_FAULT 在所有 KVM 退出原因中是独一无二的：它伴随的返回码是
'-1' 而不是 '0'！当 KVM 以 KVM_EXIT_MEMORY_FAULT 退出时，errno 总是被设置为
EFAULT 或 EHWPOISON；对于所有其他错误码，用户态应假定 kvm_run.exit_reason 是过时
/未定义的。)

::

    /* KVM_EXIT_NOTIFY */
    struct {
  #define KVM_NOTIFY_CONTEXT_INVALID	(1 << 0)
      __u32 flags;
    } notify;

Used on x86 systems. When the VM capability KVM_CAP_X86_NOTIFY_VMEXIT is
enabled, a VM exit generated if no event window occurs in VM non-root mode
for a specified amount of time. Once KVM_X86_NOTIFY_VMEXIT_USER is set when
enabling the cap, it would exit to userspace with the exit reason
KVM_EXIT_NOTIFY for further handling. The "flags" field contains more
detailed info.
(用于 x86 系统。当启用 VM 能力 KVM_CAP_X86_NOTIFY_VMEXIT 后，如果 VM 非 root 模式
在指定的一段时间内没有出现事件窗口，则会产生一次 VM exit。启用该能力时若设置了
KVM_X86_NOTIFY_VMEXIT_USER，则会以退出原因 KVM_EXIT_NOTIFY 退出到用户态做进一步
处理。"flags" 字段包含更详细的信息。)

The valid value for 'flags' is:
('flags' 的有效取值为：)

  - KVM_NOTIFY_CONTEXT_INVALID -- the VM context is corrupted and not valid
    in VMCS. It would run into unknown result if resume the target VM.
    (KVM_NOTIFY_CONTEXT_INVALID -- VM 上下文已损坏，在 VMCS 中无效。如果恢复目标
    VM，结果将不可预知。)

::

		/* KVM_EXIT_TDX */
		struct {
			__u64 flags;
			__u64 nr;
			union {
				struct {
					u64 ret;
					u64 data[5];
				} unknown;
				struct {
					u64 ret;
					u64 gpa;
					u64 size;
				} get_quote;
				struct {
					u64 ret;
					u64 leaf;
					u64 r11, r12, r13, r14;
				} get_tdvmcall_info;
				struct {
					u64 ret;
					u64 vector;
				} setup_event_notify;
			};
		} tdx;

Process a TDVMCALL from the guest.  KVM forwards select TDVMCALL based
on the Guest-Hypervisor Communication Interface (GHCI) specification;
KVM bridges these requests to the userspace VMM with minimal changes,
placing the inputs in the union and copying them back to the guest
on re-entry.
(处理来自客户机的 TDVMCALL。KVM 依据 Guest-Hypervisor Communication Interface
(GHCI) 规范转发选定的 TDVMCALL；KVM 以最小的改动把这些请求桥接给用户态 VMM，把输
入放入联合体中，并在重新进入时把它们复制回客户机。)

Flags are currently always zero, whereas ``nr`` contains the TDVMCALL
number from register R11.  The remaining field of the union provide the
inputs and outputs of the TDVMCALL.  Currently the following values of
``nr`` are defined:
(Flags 目前总是为零，而 ``nr`` 包含来自寄存器 R11 的 TDVMCALL 编号。联合体的其余
字段提供 TDVMCALL 的输入与输出。当前定义了以下 ``nr`` 取值：)

 * ``TDVMCALL_GET_QUOTE``: the guest has requested to generate a TD-Quote
   signed by a service hosting TD-Quoting Enclave operating on the host.
   Parameters and return value are in the ``get_quote`` field of the union.
   The ``gpa`` field and ``size`` specify the guest physical address
   (without the shared bit set) and the size of a shared-memory buffer, in
   which the TDX guest passes a TD Report.  The ``ret`` field represents
   the return value of the GetQuote request.  When the request has been
   queued successfully, the TDX guest can poll the status field in the
   shared-memory area to check whether the Quote generation is completed or
   not. When completed, the generated Quote is returned via the same buffer.
   (``TDVMCALL_GET_QUOTE``：客户机请求生成由宿主机上托管 TD-Quoting Enclave 的服
   务签名的 TD-Quote。参数与返回值位于联合体的 ``get_quote`` 字段。``gpa`` 与
   ``size`` 字段指定一个共享内存缓冲区的客户机物理地址(不设置共享位)与大小，TDX
   客户机通过该缓冲区传递 TD Report。``ret`` 字段代表 GetQuote 请求的返回值。请求
   成功入队后，TDX 客户机可以轮询共享内存区域中的 status 字段，检查 Quote 生成是
   否完成。完成后，生成的 Quote 通过同一缓冲区返回。)

 * ``TDVMCALL_GET_TD_VM_CALL_INFO``: the guest has requested the support
   status of TDVMCALLs.  The output values for the given leaf should be
   placed in fields from ``r11`` to ``r14`` of the ``get_tdvmcall_info``
   field of the union.
   (``TDVMCALL_GET_TD_VM_CALL_INFO``：客户机请求查询各 TDVMCALL 的支持状态。给定
   leaf 的输出值应放入联合体 ``get_tdvmcall_info`` 字段中从 ``r11`` 到 ``r14`` 的
   各字段。)

 * ``TDVMCALL_SETUP_EVENT_NOTIFY_INTERRUPT``: the guest has requested to
   set up a notification interrupt for vector ``vector``.
   (``TDVMCALL_SETUP_EVENT_NOTIFY_INTERRUPT``：客户机请求为向量 ``vector`` 设置
   一个通知中断。)

KVM may add support for more values in the future that may cause a userspace
exit, even without calls to ``KVM_ENABLE_CAP`` or similar.  In this case,
it will enter with output fields already valid; in the common case, the
``unknown.ret`` field of the union will be ``TDVMCALL_STATUS_SUBFUNC_UNSUPPORTED``.
Userspace need not do anything if it does not wish to support a TDVMCALL.
(KVM 将来可能会增加对更多取值的支持，即使没有调用 ``KVM_ENABLE_CAP`` 或类似接口，
这些取值也可能导致退出到用户态。在这种情况下，重新进入时输出字段已经有效；常见
情形下，联合体的 ``unknown.ret`` 字段会是 ``TDVMCALL_STATUS_SUBFUNC_UNSUPPORTED``。
如果用户态不想支持某个 TDVMCALL，它无需做任何事情。)

::

		/* KVM_EXIT_ARM_SEA */
		struct {
  #define KVM_EXIT_ARM_SEA_FLAG_GPA_VALID   (1ULL << 0)
			__u64 flags;
			__u64 esr;
			__u64 gva;
			__u64 gpa;
		} arm_sea;

Used on arm64 systems. When the VM capability ``KVM_CAP_ARM_SEA_TO_USER`` is
enabled, a KVM exits to userspace if a guest access causes a synchronous
external abort (SEA) and the host APEI fails to handle the SEA.
(用于 arm64 系统。当启用 VM 能力 ``KVM_CAP_ARM_SEA_TO_USER`` 后，如果客户机访问引
发同步外部中止 (SEA)、且宿主 APEI 未能处理该 SEA，KVM 会退出到用户态。)

``esr`` is set to a sanitized value of ESR_EL2 from the exception taken to KVM,
consisting of the following fields:
(``esr`` 被设置为进入 KVM 的异常所对应的 ESR_EL2 净化值 (sanitized)，由以下字段
组成：)

 - ``ESR_EL2.EC``
 - ``ESR_EL2.IL``
 - ``ESR_EL2.FnV``
 - ``ESR_EL2.EA``
 - ``ESR_EL2.CM``
 - ``ESR_EL2.WNR``
 - ``ESR_EL2.FSC``
 - ``ESR_EL2.SET`` (when FEAT_RAS is implemented for the VM)
   (``ESR_EL2.SET``，当为该 VM 实现了 FEAT_RAS 时)

``gva`` is set to the value of FAR_EL2 from the exception taken to KVM when
``ESR_EL2.FnV == 0``. Otherwise, the value of ``gva`` is unknown.
(当 ``ESR_EL2.FnV == 0`` 时，``gva`` 被设置为进入 KVM 的异常所对应的 FAR_EL2 值。
否则，``gva`` 的值未知。)

``gpa`` is set to the faulting IPA from the exception taken to KVM when
the ``KVM_EXIT_ARM_SEA_FLAG_GPA_VALID`` flag is set. Otherwise, the value of
``gpa`` is unknown.
(当设置了 ``KVM_EXIT_ARM_SEA_FLAG_GPA_VALID`` 标志时，``gpa`` 被设置为进入 KVM 的
异常所对应的出错 IPA。否则，``gpa`` 的值未知。)

::

		/* Fix the size of the union. */
		char padding[256];
	};

	/*
	 * shared registers between kvm and userspace.
	 * kvm_valid_regs specifies the register classes set by the host
	 * kvm_dirty_regs specified the register classes dirtied by userspace
	 * struct kvm_sync_regs is architecture specific, as well as the
	 * bits for kvm_valid_regs and kvm_dirty_regs
	 */
	__u64 kvm_valid_regs;
	__u64 kvm_dirty_regs;
	union {
		struct kvm_sync_regs regs;
		char padding[SYNC_REGS_SIZE_BYTES];
	} s;

If KVM_CAP_SYNC_REGS is defined, these fields allow userspace to access
certain guest registers without having to call SET/GET_*REGS. Thus we can
avoid some system call overhead if userspace has to handle the exit.
Userspace can query the validity of the structure by checking
kvm_valid_regs for specific bits. These bits are architecture specific
and usually define the validity of a groups of registers. (e.g. one bit
for general purpose registers)
(如果定义了 KVM_CAP_SYNC_REGS，这些字段允许用户态访问某些客户机寄存器而无需调用
SET/GET_*REGS。这样，当用户态必须处理退出时，可以省去一些系统调用开销。用户态可
以通过检查 kvm_valid_regs 中的特定位来查询该结构的有效性。这些位是架构特定的，
通常定义一组寄存器的有效性(例如通用寄存器对应一个位)。)

Please note that the kernel is allowed to use the kvm_run structure as the
primary storage for certain register types. Therefore, the kernel may use the
values in kvm_run even if the corresponding bit in kvm_dirty_regs is not set.
(请注意，内核允许把 kvm_run 结构用作某些寄存器类型的主存储。因此，即使
kvm_dirty_regs 中的对应位未置位，内核也可能使用 kvm_run 中的值。)

::

		/* KVM_EXIT_SNP_REQ_CERTS */
		struct kvm_exit_snp_req_certs {
			__u64 gpa;
			__u64 npages;
			__u64 ret;
		};

KVM_EXIT_SNP_REQ_CERTS indicates an SEV-SNP guest with certificate-fetching
enabled (see KVM_SEV_SNP_ENABLE_REQ_CERTS) has generated an Extended Guest
Request NAE #VMGEXIT (SNP_GUEST_REQUEST) with message type MSG_REPORT_REQ,
i.e. has requested an attestation report from firmware, and would like the
certificate data corresponding to the attestation report signature to be
provided by the hypervisor as part of the request.
(KVM_EXIT_SNP_REQ_CERTS 表示启用了证书获取的 SEV-SNP 客户机(参见
KVM_SEV_SNP_ENABLE_REQ_CERTS)产生了一个消息类型为 MSG_REPORT_REQ 的扩展客户机请
求 NAE #VMGEXIT (SNP_GUEST_REQUEST)，即向固件请求了一份 attestation report，并希
望 hypervisor 把与该报告签名对应的证书数据作为请求的一部分提供。)

To allow for userspace to provide the certificate, the 'gpa' and 'npages'
are forwarded verbatim from the guest request (the RAX and RBX GHCB fields
respectively).  'ret' is not an "output" from KVM, and is always '0' on
exit.  KVM verifies the 'gpa' is 4KiB aligned prior to exiting to userspace,
but otherwise the information from the guest isn't validated.
(为了让用户态能够提供证书，'gpa' 与 'npages' 会从客户机请求中原样转发(分别对应
GHCB 的 RAX 与 RBX 字段)。'ret' 不是来自 KVM 的 "输出"，退出时总是 '0'。在退出到
用户态之前，KVM 会校验 'gpa' 是否 4KiB 对齐，除此之外不会校验来自客户机的信息。)

Upon the next KVM_RUN, e.g. after userspace has serviced the request (or not),
KVM will complete the #VMGEXIT, using the 'ret' field to determine whether to
signal success or failure to the guest, and on failure, what reason code will
be communicated via SW_EXITINFO2.  If 'ret' is set to an unsupported value (see
the table below), KVM_RUN will fail with -EINVAL.  For a 'ret' of 'ENOSPC', KVM
also consumes the 'npages' field, i.e. userspace can use the field to inform
the guest of the number of pages needed to hold all the certificate data.
(在下一次 KVM_RUN 时，例如在用户态处理(或未处理)完该请求之后，KVM 会完成该
#VMGEXIT，用 'ret' 字段决定向客户机报告成功还是失败；失败时，再通过 SW_EXITINFO2
传递原因码。如果 'ret' 被设置为不支持的值(见下表)，KVM_RUN 会以 -EINVAL 失败。
当 'ret' 为 'ENOSPC' 时，KVM 还会消费 'npages' 字段，即用户态可以用该字段告知客
户机容纳全部证书数据所需的页数。)

The supported 'ret' values and their respective SW_EXITINFO2 encodings:
(受支持的 'ret' 取值及其对应的 SW_EXITINFO2 编码如下：)

  ======     =============================================================
  0          0x0, i.e. success.  KVM will emit an SNP_GUEST_REQUEST command
             to SNP firmware.
             (0x0，即成功。KVM 会向 SNP 固件发出一条 SNP_GUEST_REQUEST 命令。)
  ENOSPC     0x0000000100000000, i.e. not enough guest pages to hold the
             certificate table and certificate data.  KVM will also set the
             RBX field in the GHBC to 'npages'.
             (0x0000000100000000，即没有足够的客户机页来容纳证书表与证书数据。
             KVM 还会把 GHCB 中的 RBX 字段设置为 'npages'。)
  EAGAIN     0x0000000200000000, i.e. the host is busy and the guest should
             retry the request.
             (0x0000000200000000，即宿主机正忙，客户机应当重试该请求。)
  EIO        0xffffffff00000000, for all other errors (this return code is
             a KVM-defined hypervisor value, as allowed by the GHCB)
             (0xffffffff00000000，用于所有其他错误(此返回码是 GHCB 允许的
             KVM 定义 hypervisor 值)。)
  ======     =============================================================


.. _cap_enable:

6. Capabilities that can be enabled on vCPUs(6. 可在 vCPU 上启用的能力)
========================================================================

There are certain capabilities that change the behavior of the virtual CPU or
the virtual machine when enabled. To enable them, please see
:ref:`KVM_ENABLE_CAP`.
(某些能力在启用后会改变虚拟 CPU 或虚拟机的行为。要启用它们，请参见
:ref:`KVM_ENABLE_CAP`。)

Below you can find a list of capabilities and what their effect on the vCPU or
the virtual machine is when enabling them.
(下面列出了各能力，以及启用它们时对 vCPU 或虚拟机的影响。)

The following information is provided along with the description:
(描述中还提供以下信息：)

  Architectures:
      which instruction set architectures provide this ioctl.
      x86 includes both i386 and x86_64.
      (提供此 ioctl 的指令集架构。x86 同时包括 i386 与 x86_64。)

  Target:
      whether this is a per-vcpu or per-vm capability.
      (该能力是针对每个 vcpu 还是每个 vm。)

  Parameters:
      what parameters are accepted by the capability.
      (该能力接受哪些参数。)

  Returns:
      the return value.  General error numbers (EBADF, ENOMEM, EINVAL)
      are not detailed, but errors with specific meanings are.
      (返回值。一般性错误码 (EBADF、ENOMEM、EINVAL) 不再赘述，但具有特定含义的错
      误会加以说明。)


6.1 KVM_CAP_PPC_OSI(6.1 KVM_CAP_PPC_OSI)
----------------------------------------

:Architectures: ppc
:Target: vcpu
:Parameters: none
:Returns: 0 on success; -1 on error

(架构：ppc；目标：vcpu；参数：无；返回值：成功时为 0，出错时为 -1。)

This capability enables interception of OSI hypercalls that otherwise would
be treated as normal system calls to be injected into the guest. OSI hypercalls
were invented by Mac-on-Linux to have a standardized communication mechanism
between the guest and the host.
(此能力启用对 OSI 超级调用的拦截；否则这些超级调用会被当作普通系统调用注入客户
机。OSI 超级调用由 Mac-on-Linux 发明，用于在客户机与宿主机之间建立标准化的通信机
制。)

When this capability is enabled, KVM_EXIT_OSI can occur.
(启用此能力后，可能发生 KVM_EXIT_OSI。)


6.2 KVM_CAP_PPC_PAPR(6.2 KVM_CAP_PPC_PAPR)
------------------------------------------

:Architectures: ppc
:Target: vcpu
:Parameters: none
:Returns: 0 on success; -1 on error

(架构：ppc；目标：vcpu；参数：无；返回值：成功时为 0，出错时为 -1。)

This capability enables interception of PAPR hypercalls. PAPR hypercalls are
done using the hypercall instruction "sc 1".
(此能力启用对 PAPR 超级调用的拦截。PAPR 超级调用通过超级调用指令 "sc 1" 完成。)

It also sets the guest privilege level to "supervisor" mode. Usually the guest
runs in "hypervisor" privilege mode with a few missing features.
(它还会把客户机特权级设置为 "supervisor"(管理者)模式。通常客户机运行在缺少若干
特性的 "hypervisor"(管理程序)特权模式下。)

In addition to the above, it changes the semantics of SDR1. In this mode, the
HTAB address part of SDR1 contains an HVA instead of a GPA, as PAPR keeps the
HTAB invisible to the guest.
(除此之外，它还改变了 SDR1 的语义。在这种模式下，SDR1 的 HTAB 地址部分包含 HVA
而非 GPA，因为 PAPR 对客户机隐藏了 HTAB。)

When this capability is enabled, KVM_EXIT_PAPR_HCALL can occur.
(启用此能力后，可能发生 KVM_EXIT_PAPR_HCALL。)


6.3 KVM_CAP_SW_TLB(6.3 KVM_CAP_SW_TLB)
--------------------------------------

:Architectures: ppc
:Target: vcpu
:Parameters: args[0] is the address of a struct kvm_config_tlb
:Returns: 0 on success; -1 on error

(架构：ppc；目标：vcpu；参数：args[0] 是 struct kvm_config_tlb 的地址；返回值：成
功时为 0，出错时为 -1。)

::

  struct kvm_config_tlb {
	__u64 params;
	__u64 array;
	__u32 mmu_type;
	__u32 array_len;
  };

Configures the virtual CPU's TLB array, establishing a shared memory area
between userspace and KVM.  The "params" and "array" fields are userspace
addresses of mmu-type-specific data structures.  The "array_len" field is an
safety mechanism, and should be set to the size in bytes of the memory that
userspace has reserved for the array.  It must be at least the size dictated
by "mmu_type" and "params".
(配置虚拟 CPU 的 TLB 数组，在用户态与 KVM 之间建立一个共享内存区域。"params" 与
"array" 字段是特定 mmu 类型数据结构的用户态地址。"array_len" 字段是一种安全机
制，应设置为用户态为数组保留的内存大小(以字节为单位)。它必须不小于由 "mmu_type"
与 "params" 规定的大小。)

While KVM_RUN is active, the shared region is under control of KVM.  Its
contents are undefined, and any modification by userspace results in
boundedly undefined behavior.
(在 KVM_RUN 执行期间，共享区域由 KVM 控制。其内容是未定义的，用户态的任何修改都
会导致有界的未定义行为。)

On return from KVM_RUN, the shared region will reflect the current state of
the guest's TLB.  If userspace makes any changes, it must call KVM_DIRTY_TLB
to tell KVM which entries have been changed, prior to calling KVM_RUN again
on this vcpu.
(从 KVM_RUN 返回时，共享区域将反映客户机 TLB 的当前状态。如果用户态做了任何修
改，在此 vcpu 上再次调用 KVM_RUN 之前，必须先调用 KVM_DIRTY_TLB 告诉 KVM 哪些条
目已被修改。)

For mmu types KVM_MMU_FSL_BOOKE_NOHV and KVM_MMU_FSL_BOOKE_HV:
(对于 mmu 类型 KVM_MMU_FSL_BOOKE_NOHV 与 KVM_MMU_FSL_BOOKE_HV：)

 - The "params" field is of type "struct kvm_book3e_206_tlb_params".
   ("params" 字段的类型为 "struct kvm_book3e_206_tlb_params"。)
 - The "array" field points to an array of type "struct
   kvm_book3e_206_tlb_entry".
   ("array" 字段指向一个类型为 "struct kvm_book3e_206_tlb_entry" 的数组。)
 - The array consists of all entries in the first TLB, followed by all
   entries in the second TLB.
   (数组由第一个 TLB 的全部条目，以及随后第二个 TLB 的全部条目组成。)
 - Within a TLB, entries are ordered first by increasing set number.  Within a
   set, entries are ordered by way (increasing ESEL).
   (在一个 TLB 内，条目先按递增的 set 编号排序；在一个 set 内，条目按 way(递增的
   ESEL)排序。)
 - The hash for determining set number in TLB0 is: (MAS2 >> 12) & (num_sets - 1)
   where "num_sets" is the tlb_sizes[] value divided by the tlb_ways[] value.
   (在 TLB0 中确定 set 编号的哈希为： (MAS2 >> 12) & (num_sets - 1)，其中
   "num_sets" 是 tlb_sizes[] 值除以 tlb_ways[] 值。)
 - The tsize field of mas1 shall be set to 4K on TLB0, even though the
   hardware ignores this value for TLB0.
   (在 TLB0 上，mas1 的 tsize 字段应设置为 4K，即使硬件对 TLB0 会忽略此值。)

6.4 KVM_CAP_S390_CSS_SUPPORT(6.4 KVM_CAP_S390_CSS_SUPPORT)
----------------------------------------------------------

:Architectures: s390
:Target: vcpu
:Parameters: none
:Returns: 0 on success; -1 on error

(架构：s390；目标：vcpu；参数：无；返回值：成功时为 0，出错时为 -1。)

This capability enables support for handling of channel I/O instructions.
(此能力启用对通道 I/O 指令处理的支持。)

TEST PENDING INTERRUPTION and the interrupt portion of TEST SUBCHANNEL are
handled in-kernel, while the other I/O instructions are passed to userspace.
(TEST PENDING INTERRUPTION 与 TEST SUBCHANNEL 的中断部分在内核内处理，其他 I/O 指
令则传递给用户态。)

When this capability is enabled, KVM_EXIT_S390_TSCH will occur on TEST
SUBCHANNEL intercepts.
(启用此能力后，TEST SUBCHANNEL 被拦截时将发生 KVM_EXIT_S390_TSCH。)

Note that even though this capability is enabled per-vcpu, the complete
virtual machine is affected.
(注意，尽管此能力按 vcpu 启用，但整个虚拟机都会受到影响。)

6.5 KVM_CAP_PPC_EPR(6.5 KVM_CAP_PPC_EPR)
----------------------------------------

:Architectures: ppc
:Target: vcpu
:Parameters: args[0] defines whether the proxy facility is active
:Returns: 0 on success; -1 on error

(架构：ppc；目标：vcpu；参数：args[0] 定义代理 (proxy) 设施是否处于活动状态；返
回值：成功时为 0，出错时为 -1。)

This capability enables or disables the delivery of interrupts through the
external proxy facility.
(此能力启用或禁用通过外部代理 (external proxy) 设施投递中断。)

When enabled (args[0] != 0), every time the guest gets an external interrupt
delivered, it automatically exits into user space with a KVM_EXIT_EPR exit
to receive the topmost interrupt vector.
(启用时 (args[0] != 0)，每当客户机被投递一个外部中断，它都会自动以 KVM_EXIT_EPR
退出到用户空间，以接收最优先的中断向量。)

When disabled (args[0] == 0), behavior is as if this facility is unsupported.
(禁用时 (args[0] == 0)，行为如同不支持该设施。)

When this capability is enabled, KVM_EXIT_EPR can occur.
(启用此能力后，可能发生 KVM_EXIT_EPR。)

6.6 KVM_CAP_IRQ_MPIC(6.6 KVM_CAP_IRQ_MPIC)
------------------------------------------

:Architectures: ppc
:Parameters: args[0] is the MPIC device fd;
             args[1] is the MPIC CPU number for this vcpu

(架构：ppc；参数：args[0] 是 MPIC 设备 fd；args[1] 是此 vcpu 的 MPIC CPU 编号。)

This capability connects the vcpu to an in-kernel MPIC device.
(此能力把 vcpu 连接到内核内的 MPIC 设备。)

6.7 KVM_CAP_IRQ_XICS(6.7 KVM_CAP_IRQ_XICS)
------------------------------------------

:Architectures: ppc
:Target: vcpu
:Parameters: args[0] is the XICS device fd;
             args[1] is the XICS CPU number (server ID) for this vcpu

(架构：ppc；目标：vcpu；参数：args[0] 是 XICS 设备 fd；args[1] 是此 vcpu 的 XICS
CPU 编号(服务器 ID)。)

This capability connects the vcpu to an in-kernel XICS device.
(此能力把 vcpu 连接到内核内的 XICS 设备。)

6.8 KVM_CAP_S390_IRQCHIP(6.8 KVM_CAP_S390_IRQCHIP)
--------------------------------------------------

:Architectures: s390
:Target: vm
:Parameters: none

(架构：s390；目标：vm；参数：无。)

This capability enables the in-kernel irqchip for s390. Please refer to
"4.24 KVM_CREATE_IRQCHIP" for details.
(此能力为 s390 启用内核内 irqchip。细节请参见 "4.24 KVM_CREATE_IRQCHIP"。)

6.9 KVM_CAP_MIPS_FPU(6.9 KVM_CAP_MIPS_FPU)
------------------------------------------

:Architectures: mips
:Target: vcpu
:Parameters: args[0] is reserved for future use (should be 0).

(架构：mips；目标：vcpu；参数：args[0] 保留供将来使用(应为 0)。)

This capability allows the use of the host Floating Point Unit by the guest. It
allows the Config1.FP bit to be set to enable the FPU in the guest. Once this is
done the ``KVM_REG_MIPS_FPR_*`` and ``KVM_REG_MIPS_FCR_*`` registers can be
accessed (depending on the current guest FPU register mode), and the Status.FR,
Config5.FRE bits are accessible via the KVM API and also from the guest,
depending on them being supported by the FPU.
(此能力允许客户机使用宿主机的浮点单元 (FPU)。它允许设置 Config1.FP 位以在客户机
中启用 FPU。完成后即可访问 ``KVM_REG_MIPS_FPR_*`` 与 ``KVM_REG_MIPS_FCR_*`` 寄存
器(取决于当前客户机的 FPU 寄存器模式)，并且 Status.FR、Config5.FRE 位可通过
KVM API 访问，也可以从客户机访问，取决于 FPU 是否支持它们。)

6.10 KVM_CAP_MIPS_MSA(6.10 KVM_CAP_MIPS_MSA)
--------------------------------------------

:Architectures: mips
:Target: vcpu
:Parameters: args[0] is reserved for future use (should be 0).

(架构：mips；目标：vcpu；参数：args[0] 保留供将来使用(应为 0)。)

This capability allows the use of the MIPS SIMD Architecture (MSA) by the guest.
It allows the Config3.MSAP bit to be set to enable the use of MSA by the guest.
Once this is done the ``KVM_REG_MIPS_VEC_*`` and ``KVM_REG_MIPS_MSA_*``
registers can be accessed, and the Config5.MSAEn bit is accessible via the
KVM API and also from the guest.
(此能力允许客户机使用 MIPS SIMD 架构 (MSA)。它允许设置 Config3.MSAP 位以使客户机
能够使用 MSA。完成后即可访问 ``KVM_REG_MIPS_VEC_*`` 与 ``KVM_REG_MIPS_MSA_*`` 寄
存器，Config5.MSAEn 位可通过 KVM API 访问，也可以从客户机访问。)

6.74 KVM_CAP_SYNC_REGS(6.74 KVM_CAP_SYNC_REGS)
----------------------------------------------

:Architectures: s390, x86
:Target: s390: always enabled, x86: vcpu
:Parameters: none
:Returns: x86: KVM_CHECK_EXTENSION returns a bit-array indicating which register
          sets are supported
          (bitfields defined in arch/x86/include/uapi/asm/kvm.h).

(架构：s390, x86；目标：s390 为始终启用，x86 为 vcpu；参数：无；返回值：x86 上
KVM_CHECK_EXTENSION 返回一个位数组，指示支持哪些寄存器集(位域定义于
arch/x86/include/uapi/asm/kvm.h)。)

As described above in the kvm_sync_regs struct info in section :ref:`kvm_run`,
KVM_CAP_SYNC_REGS "allow[s] userspace to access certain guest registers
without having to call SET/GET_*REGS". This reduces overhead by eliminating
repeated ioctl calls for setting and/or getting register values. This is
particularly important when userspace is making synchronous guest state
modifications, e.g. when emulating and/or intercepting instructions in
userspace.
(如上文 :ref:`kvm_run` 一节中 kvm_sync_regs 结构信息所述，KVM_CAP_SYNC_REGS "允许
用户态访问某些客户机寄存器而无需调用 SET/GET_*REGS"。它省去了反复设置和/或读取寄
存器值的 ioctl 调用，从而降低开销。当用户态需要对客户机状态做同步修改时——例如
在用户态模拟和/或拦截指令时——这一点尤为重要。)

For s390 specifics, please refer to the source code.
(s390 的细节请参见源代码。)

For x86:
(对于 x86：)

- the register sets to be copied out to kvm_run are selectable
  by userspace (rather that all sets being copied out for every exit).
  (复制到 kvm_run 的寄存器集可由用户态选择(而不是每次退出都复制出全部集合)。)
- vcpu_events are available in addition to regs and sregs.
  (除 regs 与 sregs 之外，还提供 vcpu_events。)

For x86, the 'kvm_valid_regs' field of struct kvm_run is overloaded to
function as an input bit-array field set by userspace to indicate the
specific register sets to be copied out on the next exit.
(在 x86 上，struct kvm_run 的 'kvm_valid_regs' 字段被复用为一个输入位数组字段，
由用户态设置，用于指示下次退出时要复制出的具体寄存器集。)

To indicate when userspace has modified values that should be copied into
the vCPU, the all architecture bitarray field, 'kvm_dirty_regs' must be set.
This is done using the same bitflags as for the 'kvm_valid_regs' field.
If the dirty bit is not set, then the register set values will not be copied
into the vCPU even if they've been modified.
(为了表明用户态已修改了应当复制进 vCPU 的值，必须设置全架构位数组字段
'kvm_dirty_regs'。做法与 'kvm_valid_regs' 字段使用相同的位标志。如果没有设置
dirty 位，那么即使寄存器集的值已被修改，也不会被复制进 vCPU。)

Unused bitfields in the bitarrays must be set to zero.
(位数组中未使用的位域必须置零。)

::

  struct kvm_sync_regs {
        struct kvm_regs regs;
        struct kvm_sregs sregs;
        struct kvm_vcpu_events events;
  };

6.75 KVM_CAP_PPC_IRQ_XIVE(6.75 KVM_CAP_PPC_IRQ_XIVE)
----------------------------------------------------

:Architectures: ppc
:Target: vcpu
:Parameters: args[0] is the XIVE device fd;
             args[1] is the XIVE CPU number (server ID) for this vcpu

(架构：ppc；目标：vcpu；参数：args[0] 是 XIVE 设备 fd；args[1] 是此 vcpu 的 XIVE
CPU 编号(服务器 ID)。)

This capability connects the vcpu to an in-kernel XIVE device.
(此能力把 vcpu 连接到内核内的 XIVE 设备。)

6.76 KVM_CAP_HYPERV_SYNIC(6.76 KVM_CAP_HYPERV_SYNIC)
----------------------------------------------------

:Architectures: x86
:Target: vcpu

(架构：x86；目标：vcpu。)

This capability, if KVM_CHECK_EXTENSION indicates that it is
available, means that the kernel has an implementation of the
Hyper-V Synthetic interrupt controller(SynIC). Hyper-V SynIC is
used to support Windows Hyper-V based guest paravirt drivers(VMBus).
(此能力(如果 KVM_CHECK_EXTENSION 表明其可用)意味着内核实现了 Hyper-V 合成中断控
制器 (SynIC)。Hyper-V SynIC 用于支持基于 Windows Hyper-V 的客户机半虚拟化驱动
(VMBus)。)

In order to use SynIC, it has to be activated by setting this
capability via KVM_ENABLE_CAP ioctl on the vcpu fd. Note that this
will disable the use of APIC hardware virtualization even if supported
by the CPU, as it's incompatible with SynIC auto-EOI behavior.
(要使用 SynIC，必须在 vcpu fd 上通过 KVM_ENABLE_CAP ioctl 设置此能力来激活它。
注意，即使 CPU 支持 APIC 硬件虚拟化，这也会禁用它，因为它与 SynIC 的 auto-EOI
行为不兼容。)

6.77 KVM_CAP_HYPERV_SYNIC2(6.77 KVM_CAP_HYPERV_SYNIC2)
------------------------------------------------------

:Architectures: x86
:Target: vcpu

(架构：x86；目标：vcpu。)

This capability enables a newer version of Hyper-V Synthetic interrupt
controller (SynIC).  The only difference with KVM_CAP_HYPERV_SYNIC is that KVM
doesn't clear SynIC message and event flags pages when they are enabled by
writing to the respective MSRs.
(此能力启用较新版本的 Hyper-V 合成中断控制器 (SynIC)。它与 KVM_CAP_HYPERV_SYNIC
的唯一区别是：当通过写入相应 MSR 启用 SynIC 消息与事件标志页时，KVM 不会清除这些
页。)

6.78 KVM_CAP_HYPERV_DIRECT_TLBFLUSH(6.78 KVM_CAP_HYPERV_DIRECT_TLBFLUSH)
------------------------------------------------------------------------

:Architectures: x86
:Target: vcpu

(架构：x86；目标：vcpu。)

This capability indicates that KVM running on top of Hyper-V hypervisor
enables Direct TLB flush for its guests meaning that TLB flush
hypercalls are handled by Level 0 hypervisor (Hyper-V) bypassing KVM.
Due to the different ABI for hypercall parameters between Hyper-V and
KVM, enabling this capability effectively disables all hypercall
handling by KVM (as some KVM hypercall may be mistakenly treated as TLB
flush hypercalls by Hyper-V) so userspace should disable KVM identification
in CPUID and only exposes Hyper-V identification. In this case, guest
thinks it's running on Hyper-V and only use Hyper-V hypercalls.
(此能力表明，运行在 Hyper-V hypervisor 之上的 KVM 为其客户机启用了 Direct TLB
flush，即 TLB flush 超级调用由第 0 层 hypervisor (Hyper-V) 处理，绕过 KVM。由于
Hyper-V 与 KVM 的超级调用参数 ABI 不同，启用此能力实际上会禁用 KVM 的所有超级调
用处理(因为某些 KVM 超级调用可能被 Hyper-V 误判为 TLB flush 超级调用)，因此用户
态应在 CPUID 中禁用 KVM 标识，只暴露 Hyper-V 标识。这样，客户机会认为自己运行在
Hyper-V 上，只使用 Hyper-V 超级调用。)

6.79 KVM_CAP_HYPERV_ENFORCE_CPUID(6.79 KVM_CAP_HYPERV_ENFORCE_CPUID)
--------------------------------------------------------------------

:Architectures: x86
:Target: vcpu

(架构：x86；目标：vcpu。)

When enabled, KVM will disable emulated Hyper-V features provided to the
guest according to the bits Hyper-V CPUID feature leaves. Otherwise, all
currently implemented Hyper-V features are provided unconditionally when
Hyper-V identification is set in the HYPERV_CPUID_INTERFACE (0x40000001)
leaf.
(启用后，KVM 将依据 Hyper-V CPUID 特性叶中的位，禁用提供给客户机的各模拟 Hyper-V
特性。否则，只要在 HYPERV_CPUID_INTERFACE (0x40000001) 叶中设置了 Hyper-V 标识，
当前已实现的所有 Hyper-V 特性都会无条件提供。)

6.80 KVM_CAP_ENFORCE_PV_FEATURE_CPUID(6.80 KVM_CAP_ENFORCE_PV_FEATURE_CPUID)
----------------------------------------------------------------------------

:Architectures: x86
:Target: vcpu

(架构：x86；目标：vcpu。)

When enabled, KVM will disable paravirtual features provided to the
guest according to the bits in the KVM_CPUID_FEATURES CPUID leaf
(0x40000001). Otherwise, a guest may use the paravirtual features
regardless of what has actually been exposed through the CPUID leaf.
(启用后，KVM 将依据 KVM_CPUID_FEATURES CPUID 叶 (0x40000001) 中的位，禁用提供给
客户机的半虚拟化特性。否则，无论 CPUID 叶中实际暴露了什么，客户机都可能使用这些
半虚拟化特性。)

.. _KVM_CAP_DIRTY_LOG_RING:


.. _cap_enable_vm:

7. Capabilities that can be enabled on VMs(7. 可在 VM 上启用的能力)
======================================================================

There are certain capabilities that change the behavior of the virtual
machine when enabled. To enable them, please see section
:ref:`KVM_ENABLE_CAP`. Below you can find a list of capabilities and
what their effect on the VM is when enabling them.
(某些能力在启用后会改变虚拟机的行为。要启用它们，请参见 :ref:`KVM_ENABLE_CAP`
一节。下面列出了各能力，以及启用它们时对 VM 的影响。)

The following information is provided along with the description:
(描述中还提供以下信息：)

  Architectures:
      which instruction set architectures provide this ioctl.
      x86 includes both i386 and x86_64.
      (提供此 ioctl 的指令集架构。x86 同时包括 i386 与 x86_64。)

  Parameters:
      what parameters are accepted by the capability.
      (该能力接受哪些参数。)

  Returns:
      the return value.  General error numbers (EBADF, ENOMEM, EINVAL)
      are not detailed, but errors with specific meanings are.
      (返回值。一般性错误码 (EBADF、ENOMEM、EINVAL) 不再赘述，但具有特定含义的错
      误会加以说明。)


7.1 KVM_CAP_PPC_ENABLE_HCALL(7.1 KVM_CAP_PPC_ENABLE_HCALL)
----------------------------------------------------------

:Architectures: ppc
:Parameters: args[0] is the sPAPR hcall number;
	     args[1] is 0 to disable, 1 to enable in-kernel handling

(架构：ppc；参数：args[0] 是 sPAPR hcall 编号；args[1] 为 0 表示禁用内核内处理，
为 1 表示启用内核内处理。)

This capability controls whether individual sPAPR hypercalls (hcalls)
get handled by the kernel or not.  Enabling or disabling in-kernel
handling of an hcall is effective across the VM.  On creation, an
initial set of hcalls are enabled for in-kernel handling, which
consists of those hcalls for which in-kernel handlers were implemented
before this capability was implemented.  If disabled, the kernel will
not to attempt to handle the hcall, but will always exit to userspace
to handle it.  Note that it may not make sense to enable some and
disable others of a group of related hcalls, but KVM does not prevent
userspace from doing that.
(此能力控制各个 sPAPR 超级调用 (hcall) 是否由内核处理。启用或禁用某个 hcall 的内
核内处理对整个 VM 生效。VM 创建时，会默认启用一组 hcall 的内核内处理，即在此能力
实现之前就已有内核内处理程序的那些 hcall。如果被禁用，内核不会尝试处理该 hcall，
而总是退出到用户态来处理。注意，把一组相关的 hcall 中的一些启用、另一些禁用可能
并不合理，但 KVM 并不阻止用户态这样做。)

If the hcall number specified is not one that has an in-kernel
implementation, the KVM_ENABLE_CAP ioctl will fail with an EINVAL
error.
(如果指定的 hcall 编号没有内核内实现，KVM_ENABLE_CAP ioctl 会以 EINVAL 错误失
败。)

7.2 KVM_CAP_S390_USER_SIGP(7.2 KVM_CAP_S390_USER_SIGP)
------------------------------------------------------

:Architectures: s390
:Parameters: none

(架构：s390；参数：无。)

This capability controls which SIGP orders will be handled completely in user
space. With this capability enabled, all fast orders will be handled completely
in the kernel:
(此能力控制哪些 SIGP 命令 (order) 将完全在用户空间处理。启用此能力后，所有快速命
令 (fast orders) 将完全在内核中处理：)

- SENSE
- SENSE RUNNING
- EXTERNAL CALL
- EMERGENCY SIGNAL
- CONDITIONAL EMERGENCY SIGNAL

All other orders will be handled completely in user space.
(所有其他命令将完全在用户空间处理。)

Only privileged operation exceptions will be checked for in the kernel (or even
in the hardware prior to interception). If this capability is not enabled, the
old way of handling SIGP orders is used (partially in kernel and user space).
(内核中只会检查特权操作异常(甚至在拦截之前的硬件中也只检查它)。如果未启用此能
力，则使用旧的 SIGP 命令处理方式(部分在内核、部分在用户空间)。)

7.3 KVM_CAP_S390_VECTOR_REGISTERS(7.3 KVM_CAP_S390_VECTOR_REGISTERS)
--------------------------------------------------------------------

:Architectures: s390
:Parameters: none
:Returns: 0 on success, negative value on error

(架构：s390；参数：无；返回值：成功时为 0，出错时为负值。)

Allows use of the vector registers introduced with z13 processor, and
provides for the synchronization between host and user space.  Will
return -EINVAL if the machine does not support vectors.
(允许使用 z13 处理器引入的向量寄存器，并提供宿主机与用户空间之间的同步。如果机
器不支持向量，将返回 -EINVAL。)

7.4 KVM_CAP_S390_USER_STSI(7.4 KVM_CAP_S390_USER_STSI)
------------------------------------------------------

:Architectures: s390
:Parameters: none

(架构：s390；参数：无。)

This capability allows post-handlers for the STSI instruction. After
initial handling in the kernel, KVM exits to user space with
KVM_EXIT_S390_STSI to allow user space to insert further data.
(此能力允许为 STSI 指令提供后置处理器 (post-handler)。在内核中完成初始处理之后，
KVM 以 KVM_EXIT_S390_STSI 退出到用户空间，允许用户空间插入更多数据。)

Before exiting to userspace, kvm handlers should fill in s390_stsi field of
vcpu->run::

  struct {
	__u64 addr;
	__u8 ar;
	__u8 reserved;
	__u8 fc;
	__u8 sel1;
	__u16 sel2;
  } s390_stsi;

  @addr - guest address of STSI SYSIB
  @fc   - function code
  @sel1 - selector 1
  @sel2 - selector 2
  @ar   - access register number

(在退出到用户空间之前，kvm 处理程序应当填写 vcpu->run 的 s390_stsi 字段，其内容
如下：)

KVM handlers should exit to userspace with rc = -EREMOTE.
(KVM 处理程序应以 rc = -EREMOTE 退出到用户空间。)

7.5 KVM_CAP_SPLIT_IRQCHIP(7.5 KVM_CAP_SPLIT_IRQCHIP)
----------------------------------------------------

:Architectures: x86
:Parameters: args[0] - number of routes reserved for userspace IOAPICs
:Returns: 0 on success, -1 on error

(架构：x86；参数：args[0] - 为用户态 IOAPIC 保留的路由数量；返回值：成功时为 0，
出错时为 -1。)

Create a local apic for each processor in the kernel. This can be used
instead of KVM_CREATE_IRQCHIP if the userspace VMM wishes to emulate the
IOAPIC and PIC (and also the PIT, even though this has to be enabled
separately).
(在内核中为每个处理器创建一个本地 APIC。如果用户态 VMM 希望自己模拟 IOAPIC 与
PIC(还有 PIT，尽管它需要单独启用)，可以用它代替 KVM_CREATE_IRQCHIP。)

This capability also enables in kernel routing of interrupt requests;
when KVM_CAP_SPLIT_IRQCHIP only routes of KVM_IRQ_ROUTING_MSI type are
used in the IRQ routing table.  The first args[0] MSI routes are reserved
for the IOAPIC pins.  Whenever the LAPIC receives an EOI for these routes,
a KVM_EXIT_IOAPIC_EOI vmexit will be reported to userspace.
(此能力还会启用内核内的中断请求路由；在 KVM_CAP_SPLIT_IRQCHIP 模式下，IRQ 路由表
中只使用 KVM_IRQ_ROUTING_MSI 类型的路由。前 args[0] 条 MSI 路由保留给 IOAPIC 引
脚。每当 LAPIC 收到这些路由的 EOI 时，都会向用户态报告一次 KVM_EXIT_IOAPIC_EOI
vmexit。)

Fails if VCPU has already been created, or if the irqchip is already in the
kernel (i.e. KVM_CREATE_IRQCHIP has already been called).
(如果 VCPU 已经创建，或 irqchip 已经在内核中(即已经调用过 KVM_CREATE_IRQCHIP)，
则失败。)

7.6 KVM_CAP_S390_RI(7.6 KVM_CAP_S390_RI)
----------------------------------------

:Architectures: s390
:Parameters: none

(架构：s390；参数：无。)

Allows use of runtime-instrumentation introduced with zEC12 processor.
Will return -EINVAL if the machine does not support runtime-instrumentation.
Will return -EBUSY if a VCPU has already been created.
(允许使用 zEC12 处理器引入的运行时插桩 (runtime-instrumentation)。如果机器不支持
运行时插桩，将返回 -EINVAL。如果已经创建了 VCPU，将返回 -EBUSY。)

7.7 KVM_CAP_X2APIC_API(7.7 KVM_CAP_X2APIC_API)
----------------------------------------------

:Architectures: x86
:Parameters: args[0] - features that should be enabled
:Returns: 0 on success, -EINVAL when args[0] contains invalid features

(架构：x86；参数：args[0] - 应启用的特性；返回值：成功时为 0，args[0] 包含无效特
性时为 -EINVAL。)

Valid feature flags in args[0] are::

  #define KVM_X2APIC_API_USE_32BIT_IDS                          (1ULL << 0)
  #define KVM_X2APIC_API_DISABLE_BROADCAST_QUIRK                (1ULL << 1)
  #define KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST              (1ULL << 2)
  #define KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST             (1ULL << 3)

(args[0] 中有效的特性标志如下：)

Enabling KVM_X2APIC_API_USE_32BIT_IDS changes the behavior of
KVM_SET_GSI_ROUTING, KVM_SIGNAL_MSI, KVM_SET_LAPIC, and KVM_GET_LAPIC,
allowing the use of 32-bit APIC IDs.  See KVM_CAP_X2APIC_API in their
respective sections.
(启用 KVM_X2APIC_API_USE_32BIT_IDS 会改变 KVM_SET_GSI_ROUTING、KVM_SIGNAL_MSI、
KVM_SET_LAPIC 与 KVM_GET_LAPIC 的行为，允许使用 32 位 APIC ID。参见各自小节中的
KVM_CAP_X2APIC_API。)

KVM_X2APIC_API_DISABLE_BROADCAST_QUIRK must be enabled for x2APIC to work
in logical mode or with more than 255 VCPUs.  Otherwise, KVM treats 0xff
as a broadcast even in x2APIC mode in order to support physical x2APIC
without interrupt remapping.  This is undesirable in logical mode,
where 0xff represents CPUs 0-7 in cluster 0.
(要让 x2APIC 在逻辑模式下或超过 255 个 VCPU 的情况下工作，必须启用
KVM_X2APIC_API_DISABLE_BROADCAST_QUIRK。否则，即使在 x2APIC 模式下，KVM 也会把
0xff 当作广播，以支持不带中断重映射的物理 x2APIC。这在逻辑模式中是不可取的，因为
0xff 在逻辑模式中代表 cluster 0 的 CPU 0-7。)

Setting KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST instructs KVM to enable
Suppress EOI Broadcasts.  KVM will advertise support for Suppress EOI
Broadcast to the guest and suppress LAPIC EOI broadcasts when the guest
sets the Suppress EOI Broadcast bit in the SPIV register.  This flag is
supported only when using a split IRQCHIP.
(设置 KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST 指示 KVM 启用 Suppress EOI
Broadcasts(抑制 EOI 广播)。KVM 会向客户机通告对 Suppress EOI Broadcast 的支持，
并在客户机在 SPIV 寄存器中设置 Suppress EOI Broadcast 位时抑制 LAPIC EOI 广播。
此标志仅在使用 split IRQCHIP 时受支持。)

Setting KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST disables support for
Suppress EOI Broadcasts entirely, i.e. instructs KVM to NOT advertise
support to the guest.
(设置 KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST 会完全禁用对 Suppress EOI
Broadcasts 的支持，即指示 KVM 不要向客户机通告支持。)

Modern VMMs should either enable KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST
or KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST.  If not, legacy quirky
behavior will be used by KVM: in split IRQCHIP mode, KVM will advertise
support for Suppress EOI Broadcasts but not actually suppress EOI
broadcasts; for in-kernel IRQCHIP mode, KVM will not advertise support for
Suppress EOI Broadcasts.
(现代 VMM 应当在 KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST 与
KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST 之间二选一。否则，KVM 会沿用旧有的怪异
行为：在 split IRQCHIP 模式下，KVM 会通告对 Suppress EOI Broadcasts 的支持，但并
不真正抑制 EOI 广播；在内核内 IRQCHIP 模式下，KVM 不会通告对 Suppress EOI
Broadcasts 的支持。)

Setting both KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST and
KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST will fail with an EINVAL error,
as will setting KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST without a split
IRCHIP.
(同时设置 KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST 与
KVM_X2APIC_DISABLE_SUPPRESS_EOI_BROADCAST 会以 EINVAL 错误失败；在没有 split
IRQCHIP 的情况下设置 KVM_X2APIC_ENABLE_SUPPRESS_EOI_BROADCAST 同样会失败。)

7.8 KVM_CAP_S390_USER_INSTR0(7.8 KVM_CAP_S390_USER_INSTR0)
----------------------------------------------------------

:Architectures: s390
:Parameters: none

(架构：s390；参数：无。)

With this capability enabled, the illegal instruction 0x0000 (2 bytes) will
be intercepted and forwarded to user space. User space can use this
mechanism e.g. to realize 2-byte software breakpoints. The kernel will
not inject an operating exception for these instructions, user space has
to take care of that.
(启用此能力后，非法指令 0x0000(2 字节)会被拦截并转发给用户空间。用户空间可以利
用这一机制实现 2 字节软件断点等。内核不会为这些指令注入操作异常 (operating
exception)，用户空间必须自行处理。)

This capability can be enabled dynamically even if VCPUs were already
created and are running.
(即使 VCPU 已经创建并正在运行，此能力也可以动态启用。)

7.9 KVM_CAP_S390_GS(7.9 KVM_CAP_S390_GS)
----------------------------------------

:Architectures: s390
:Parameters: none
:Returns: 0 on success; -EINVAL if the machine does not support
          guarded storage; -EBUSY if a VCPU has already been created.

(架构：s390；参数：无；返回值：成功时为 0；机器不支持受保护存储 (guarded
storage) 时为 -EINVAL；已创建 VCPU 时为 -EBUSY。)

Allows use of guarded storage for the KVM guest.
(允许 KVM 客户机使用受保护存储 (guarded storage)。)

7.10 KVM_CAP_S390_AIS(7.10 KVM_CAP_S390_AIS)
--------------------------------------------

:Architectures: s390
:Parameters: none

Allow use of adapter-interruption suppression.
:Returns: 0 on success; -EBUSY if a VCPU has already been created.
(架构：s390；参数：无。允许使用适配器中断抑制 (adapter-interruption
suppression)。返回值：成功时为 0；已创建 VCPU 时为 -EBUSY。)

7.11 KVM_CAP_PPC_SMT(7.11 KVM_CAP_PPC_SMT)
------------------------------------------

:Architectures: ppc
:Parameters: vsmt_mode, flags

(架构：ppc；参数：vsmt_mode, flags。)

Enabling this capability on a VM provides userspace with a way to set
the desired virtual SMT mode (i.e. the number of virtual CPUs per
virtual core).  The virtual SMT mode, vsmt_mode, must be a power of 2
between 1 and 8.  On POWER8, vsmt_mode must also be no greater than
the number of threads per subcore for the host.  Currently flags must
be 0.  A successful call to enable this capability will result in
vsmt_mode being returned when the KVM_CAP_PPC_SMT capability is
subsequently queried for the VM.  This capability is only supported by
HV KVM, and can only be set before any VCPUs have been created.
The KVM_CAP_PPC_SMT_POSSIBLE capability indicates which virtual SMT
modes are available.
(在 VM 上启用此能力，为用户态提供了设置所需虚拟 SMT 模式(即每个虚拟核心的虚拟
CPU 数)的途径。虚拟 SMT 模式 vsmt_mode 必须是 1 到 8 之间 (含) 的 2 的幂。在
POWER8 上，vsmt_mode 还不得超过宿主机每个 subcore 的线程数。目前 flags 必须为 0。
成功启用此能力后，之后再对 VM 查询 KVM_CAP_PPC_SMT 能力时将返回 vsmt_mode。此能力
只受 HV KVM 支持，且只能在创建任何 VCPU 之前设置。KVM_CAP_PPC_SMT_POSSIBLE 能力指
示哪些虚拟 SMT 模式可用。)

7.12 KVM_CAP_PPC_FWNMI(7.12 KVM_CAP_PPC_FWNMI)
----------------------------------------------

:Architectures: ppc
:Parameters: none

(架构：ppc；参数：无。)

With this capability a machine check exception in the guest address
space will cause KVM to exit the guest with NMI exit reason. This
enables QEMU to build error log and branch to guest kernel registered
machine check handling routine. Without this capability KVM will
branch to guests' 0x200 interrupt vector.
(启用此能力后，客户机地址空间中的机器检查异常会使 KVM 以 NMI 退出原因退出客户
机。这使 QEMU 能够构建错误日志，并跳转到客户机内核注册的机器检查处理例程。如果没
有此能力，KVM 会跳转到客户机的 0x200 中断向量。)

7.13 KVM_CAP_X86_DISABLE_EXITS(7.13 KVM_CAP_X86_DISABLE_EXITS)
--------------------------------------------------------------

:Architectures: x86
:Parameters: args[0] defines which exits are disabled
:Returns: 0 on success, -EINVAL when args[0] contains invalid exits
          or if any vCPUs have already been created

(架构：x86；参数：args[0] 定义要禁用哪些退出；返回值：成功时为 0；args[0] 包含无
效退出、或已经创建了任何 vCPU 时为 -EINVAL。)

Valid bits in args[0] are::

  #define KVM_X86_DISABLE_EXITS_MWAIT            (1 << 0)
  #define KVM_X86_DISABLE_EXITS_HLT              (1 << 1)
  #define KVM_X86_DISABLE_EXITS_PAUSE            (1 << 2)
  #define KVM_X86_DISABLE_EXITS_CSTATE           (1 << 3)
  #define KVM_X86_DISABLE_EXITS_APERFMPERF       (1 << 4)

(args[0] 中的有效位如下：)

Enabling this capability on a VM provides userspace with a way to no
longer intercept some instructions for improved latency in some
workloads, and is suggested when vCPUs are associated to dedicated
physical CPUs.  More bits can be added in the future; userspace can
just pass the KVM_CHECK_EXTENSION result to KVM_ENABLE_CAP to disable
all such vmexits.
(在 VM 上启用此能力，为用户态提供了不再拦截某些指令的途径，从而改善某些工作负载
的延迟；当 vCPU 绑定到专用物理 CPU 时建议使用。将来可能会增加更多位；用户态只需
把 KVM_CHECK_EXTENSION 的结果传给 KVM_ENABLE_CAP，即可禁用所有这类 vmexit。)

Do not enable KVM_FEATURE_PV_UNHALT if you disable HLT exits.
(如果禁用了 HLT 退出，请不要启用 KVM_FEATURE_PV_UNHALT。)

Virtualizing the ``IA32_APERF`` and ``IA32_MPERF`` MSRs requires more
than just disabling APERF/MPERF exits. While both Intel and AMD
document strict usage conditions for these MSRs--emphasizing that only
the ratio of their deltas over a time interval (T0 to T1) is
architecturally defined--simply passing through the MSRs can still
produce an incorrect ratio.
(虚拟化 ``IA32_APERF`` 与 ``IA32_MPERF`` 这两个 MSR，可不只是禁用 APERF/MPERF 退
出就行。虽然 Intel 与 AMD 都为这些 MSR 规定了严格的使用条件——强调在一个时间区
间 (T0 到 T1) 上，只有它们增量的比值在架构上有定义——但简单地直通这些 MSR 仍可
能产生错误的比值。)

This erroneous ratio can occur if, between T0 and T1:
(如果在 T0 与 T1 之间发生以下情况，就可能得到错误的比值：)

1. The vCPU thread migrates between logical processors.
   (1. vCPU 线程在逻辑处理器之间发生了迁移。)
2. Live migration or suspend/resume operations take place.
   (2. 进行了热迁移或挂起/恢复操作。)
3. Another task shares the vCPU's logical processor.
   (3. 有其他任务与该 vCPU 的逻辑处理器共享。)
4. C-states lower than C0 are emulated (e.g., via HLT interception).
   (4. 模拟了低于 C0 的 C-state(例如通过 HLT 拦截)。)
5. The guest TSC frequency doesn't match the host TSC frequency.
   (5. 客户机 TSC 频率与宿主机 TSC 频率不一致。)

Due to these complexities, KVM does not automatically associate this
passthrough capability with the guest CPUID bit,
``CPUID.6:ECX.APERFMPERF[bit 0]``. Userspace VMMs that deem this
mechanism adequate for virtualizing the ``IA32_APERF`` and
``IA32_MPERF`` MSRs must set the guest CPUID bit explicitly.
(由于这些复杂性，KVM 不会自动把这种直通能力与客户机 CPUID 位
``CPUID.6:ECX.APERFMPERF[bit 0]`` 关联起来。认为该机制足以虚拟化
``IA32_APERF`` 与 ``IA32_MPERF`` MSR 的用户态 VMM，必须显式设置客户机 CPUID 位。)


7.14 KVM_CAP_S390_HPAGE_1M(7.14 KVM_CAP_S390_HPAGE_1M)
------------------------------------------------------

:Architectures: s390
:Parameters: none
:Returns: 0 on success, -EINVAL if hpage module parameter was not set
	  or cmma is enabled, or the VM has the KVM_VM_S390_UCONTROL
	  flag set

(架构：s390；参数：无；返回值：成功时为 0；未设置 hpage 模块参数、cmma 已启用、或
VM 设置了 KVM_VM_S390_UCONTROL 标志时为 -EINVAL。)

With this capability the KVM support for memory backing with 1m pages
through hugetlbfs can be enabled for a VM. After the capability is
enabled, cmma can't be enabled anymore and pfmfi and the storage key
interpretation are disabled. If cmma has already been enabled or the
hpage module parameter is not set to 1, -EINVAL is returned.
(通过此能力，可以为 VM 启用 KVM 对经 hugetlbfs 使用 1m 页作为内存后备的支持。能力
启用之后，cmma 不能再启用，pfmfi 与存储键解释将被禁用。如果 cmma 已经启用、或
hpage 模块参数未设置为 1，则返回 -EINVAL。)

While it is generally possible to create a huge page backed VM without
this capability, the VM will not be able to run.
(虽然一般情况下即使没有此能力也可以创建巨页后备的 VM，但该 VM 将无法运行。)

7.15 KVM_CAP_MSR_PLATFORM_INFO(7.15 KVM_CAP_MSR_PLATFORM_INFO)
--------------------------------------------------------------

:Architectures: x86
:Parameters: args[0] whether feature should be enabled or not

(架构：x86；参数：args[0] 表示是否应启用该特性。)

With this capability, a guest may read the MSR_PLATFORM_INFO MSR. Otherwise,
a #GP would be raised when the guest tries to access. Currently, this
capability does not enable write permissions of this MSR for the guest.
(启用此能力后，客户机可以读取 MSR_PLATFORM_INFO MSR。否则，客户机尝试访问时会引
发 #GP。目前，此能力不会为客户机启用对该 MSR 的写权限。)

7.16 KVM_CAP_PPC_NESTED_HV(7.16 KVM_CAP_PPC_NESTED_HV)
------------------------------------------------------

:Architectures: ppc
:Parameters: none
:Returns: 0 on success, -EINVAL when the implementation doesn't support
	  nested-HV virtualization.

(架构：ppc；参数：无；返回值：成功时为 0；实现不支持 nested-HV 虚拟化时为
-EINVAL。)

HV-KVM on POWER9 and later systems allows for "nested-HV"
virtualization, which provides a way for a guest VM to run guests that
can run using the CPU's supervisor mode (privileged non-hypervisor
state).  Enabling this capability on a VM depends on the CPU having
the necessary functionality and on the facility being enabled with a
kvm-hv module parameter.
(POWER9 及之后系统上的 HV-KVM 支持 "nested-HV" 虚拟化，它提供了一种方式，让客户
机 VM 能够运行可以使用 CPU 管理者模式 (supervisor mode，特权非 hypervisor 状态)
的嵌套客户机。在 VM 上启用此能力，取决于 CPU 具备必要的功能，以及该设施已通过
kvm-hv 模块参数启用。)

7.17 KVM_CAP_EXCEPTION_PAYLOAD(7.17 KVM_CAP_EXCEPTION_PAYLOAD)
--------------------------------------------------------------

:Architectures: x86
:Parameters: args[0] whether feature should be enabled or not

(架构：x86；参数：args[0] 表示是否应启用该特性。)

With this capability enabled, CR2 will not be modified prior to the
emulated VM-exit when L1 intercepts a #PF exception that occurs in
L2. Similarly, for kvm-intel only, DR6 will not be modified prior to
the emulated VM-exit when L1 intercepts a #DB exception that occurs in
L2. As a result, when KVM_GET_VCPU_EVENTS reports a pending #PF (or
#DB) exception for L2, exception.has_payload will be set and the
faulting address (or the new DR6 bits*) will be reported in the
exception_payload field. Similarly, when userspace injects a #PF (or
#DB) into L2 using KVM_SET_VCPU_EVENTS, it is expected to set
exception.has_payload and to put the faulting address - or the new DR6
bits\ [#]_ - in the exception_payload field.
(启用此能力后，当 L1 拦截发生在 L2 的 #PF 异常时，在模拟 VM-exit 之前不会修改
CR2。类似地(仅对 kvm-intel)，当 L1 拦截发生在 L2 的 #DB 异常时，在模拟 VM-exit
之前不会修改 DR6。因此，当 KVM_GET_VCPU_EVENTS 为 L2 报告挂起的 #PF(或 #DB)异常
时，exception.has_payload 会被置位，出错地址(或新的 DR6 位*)会通过
exception_payload 字段报告。类似地，当用户态使用 KVM_SET_VCPU_EVENTS 向 L2 注入
#PF(或 #DB)时，预期它会设置 exception.has_payload，并把出错地址(或新的 DR6 位，
见脚注)放入 exception_payload 字段。)

This capability also enables exception.pending in struct
kvm_vcpu_events, which allows userspace to distinguish between pending
and injected exceptions.
(此能力还会启用 struct kvm_vcpu_events 中的 exception.pending，使用户态能够区分
挂起的与已注入的异常。)


.. [#] For the new DR6 bits, note that bit 16 is set iff the #DB exception
       will clear DR6.RTM.
       (对于新的 DR6 位：注意，当且仅当 #DB 异常会清除 DR6.RTM 时，位 16 才被置
       位。)

7.18 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2(7.18 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2)
------------------------------------------------------------------------------

:Architectures: x86, arm64, mips
:Parameters: args[0] whether feature should be enabled or not

(架构：x86, arm64, mips；参数：args[0] 表示是否应启用该特性。)

Valid flags are::

  #define KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE   (1 << 0)
  #define KVM_DIRTY_LOG_INITIALLY_SET           (1 << 1)

(有效的标志如下：)

With KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE is set, KVM_GET_DIRTY_LOG will not
automatically clear and write-protect all pages that are returned as dirty.
Rather, userspace will have to do this operation separately using
KVM_CLEAR_DIRTY_LOG.
(设置了 KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE 后，KVM_GET_DIRTY_LOG 不会自动清除并
写保护所有被报告为脏的页。相反，用户态必须使用 KVM_CLEAR_DIRTY_LOG 单独执行这一
操作。)

At the cost of a slightly more complicated operation, this provides better
scalability and responsiveness for two reasons.  First,
KVM_CLEAR_DIRTY_LOG ioctl can operate on a 64-page granularity rather
than requiring to sync a full memslot; this ensures that KVM does not
take spinlocks for an extended period of time.  Second, in some cases a
large amount of time can pass between a call to KVM_GET_DIRTY_LOG and
userspace actually using the data in the page.  Pages can be modified
during this time, which is inefficient for both the guest and userspace:
the guest will incur a higher penalty due to write protection faults,
while userspace can see false reports of dirty pages.  Manual reprotection
helps reducing this time, improving guest performance and reducing the
number of dirty log false positives.
(以略微复杂的操作为代价，这带来了更好的可扩展性与响应性，原因有二。首先，
KVM_CLEAR_DIRTY_LOG ioctl 可以按 64 页的粒度操作，而不必同步整个 memslot；这确保
KVM 不会长时间持有自旋锁。其次，某些情况下，从调用 KVM_GET_DIRTY_LOG 到用户态实
际使用页中数据之间，可能经过很长时间。期间页可能被修改，这对客户机与用户态都是
低效的：客户机会因写保护缺页而付出更高代价，而用户态可能看到脏页的错误报告。手
动重新保护有助于缩短这段时间，提升客户机性能并减少脏日志的误报数量。)

With KVM_DIRTY_LOG_INITIALLY_SET set, all the bits of the dirty bitmap
will be initialized to 1 when created.  This also improves performance because
dirty logging can be enabled gradually in small chunks on the first call
to KVM_CLEAR_DIRTY_LOG.  KVM_DIRTY_LOG_INITIALLY_SET depends on
KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE (it is also only available on
x86, arm64 and riscv for now).
(设置了 KVM_DIRTY_LOG_INITIALLY_SET 后，脏位图的所有位在创建时都会被初始化为 1。
这同样能提升性能，因为脏日志可以在第一次调用 KVM_CLEAR_DIRTY_LOG 时分小块逐步启
用。KVM_DIRTY_LOG_INITIALLY_SET 依赖于 KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE(目前
也只在 x86、arm64 与 riscv 上可用)。)

KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 was previously available under the name
KVM_CAP_MANUAL_DIRTY_LOG_PROTECT, but the implementation had bugs that make
it hard or impossible to use it correctly.  The availability of
KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 signals that those bugs are fixed.
Userspace should not try to use KVM_CAP_MANUAL_DIRTY_LOG_PROTECT.
(KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 之前以 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT 的名
称提供，但当时的实现存在一些 bug，使其难以或无法正确使用。
KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2 的可用性表明这些 bug 已经修复。用户态不应再尝
试使用 KVM_CAP_MANUAL_DIRTY_LOG_PROTECT。)

7.19 KVM_CAP_PPC_SECURE_GUEST(7.19 KVM_CAP_PPC_SECURE_GUEST)
------------------------------------------------------------

:Architectures: ppc

(架构：ppc。)

This capability indicates that KVM is running on a host that has
ultravisor firmware and thus can support a secure guest.  On such a
system, a guest can ask the ultravisor to make it a secure guest,
one whose memory is inaccessible to the host except for pages which
are explicitly requested to be shared with the host.  The ultravisor
notifies KVM when a guest requests to become a secure guest, and KVM
has the opportunity to veto the transition.
(此能力表明 KVM 运行在具有 ultravisor 固件的宿主机上，因而可以支持安全客户机。
在这样的系统上，客户机可以请求 ultravisor 把自己变成安全客户机——除了明确请求与
宿主机共享的页之外，其内存对宿主机不可访问。当客户机请求成为安全客户机时，
ultravisor 会通知 KVM，KVM 有机会否决这一转换。)

If present, this capability can be enabled for a VM, meaning that KVM
will allow the transition to secure guest mode.  Otherwise KVM will
veto the transition.
(如果该能力存在，则可以为 VM 启用它，意味着 KVM 将允许向安全客户机模式的转换。
否则 KVM 会否决该转换。)

7.20 KVM_CAP_HALT_POLL(7.20 KVM_CAP_HALT_POLL)
----------------------------------------------

:Architectures: all
:Target: VM
:Parameters: args[0] is the maximum poll time in nanoseconds
:Returns: 0 on success; -1 on error

(架构：全部；目标：VM；参数：args[0] 是以纳秒为单位的最大轮询时间；返回值：成功
时为 0，出错时为 -1。)

KVM_CAP_HALT_POLL overrides the kvm.halt_poll_ns module parameter to set the
maximum halt-polling time for all vCPUs in the target VM. This capability can
be invoked at any time and any number of times to dynamically change the
maximum halt-polling time.
(KVM_CAP_HALT_POLL 覆盖 kvm.halt_poll_ns 模块参数，为目标 VM 中的所有 vCPU 设置
最大的 halt-polling 时间。此能力可以在任何时间、以任意次数调用，以动态更改最大
halt-polling 时间。)

See Documentation/virt/kvm/halt-polling.rst for more information on halt
polling.
(关于 halt polling 的更多信息参见 Documentation/virt/kvm/halt-polling.rst。)

7.21 KVM_CAP_X86_USER_SPACE_MSR(7.21 KVM_CAP_X86_USER_SPACE_MSR)
----------------------------------------------------------------

:Architectures: x86
:Target: VM
:Parameters: args[0] contains the mask of KVM_MSR_EXIT_REASON_* events to report
:Returns: 0 on success; -1 on error

(架构：x86；目标：VM；参数：args[0] 包含要报告的 KVM_MSR_EXIT_REASON_* 事件掩码；
返回值：成功时为 0，出错时为 -1。)

This capability allows userspace to intercept RDMSR and WRMSR instructions if
access to an MSR is denied.  By default, KVM injects #GP on denied accesses.
(此能力允许用户态在 MSR 访问被拒绝时拦截 RDMSR 与 WRMSR 指令。默认情况下，KVM 会
对被拒绝的访问注入 #GP。)

When a guest requests to read or write an MSR, KVM may not implement all MSRs
that are relevant to a respective system. It also does not differentiate by
CPU type.
(当客户机请求读取或写入某个 MSR 时，KVM 可能没有实现与相应系统相关的所有 MSR。它
也不按 CPU 类型加以区分。)

To allow more fine grained control over MSR handling, userspace may enable
this capability. With it enabled, MSR accesses that match the mask specified in
args[0] and would trigger a #GP inside the guest will instead trigger
KVM_EXIT_X86_RDMSR and KVM_EXIT_X86_WRMSR exit notifications.  Userspace
can then implement model specific MSR handling and/or user notifications
to inform a user that an MSR was not emulated/virtualized by KVM.
(为了对 MSR 处理实现更细粒度的控制，用户态可以启用此能力。启用后，匹配 args[0]
所指定掩码、原本会在客户机内触发 #GP 的 MSR 访问，将改为触发 KVM_EXIT_X86_RDMSR
与 KVM_EXIT_X86_WRMSR 退出通知。用户态随后可以实现型号相关的 MSR 处理，和/或通过
用户通知告知用户：某个 MSR 未被 KVM 模拟/虚拟化。)

The valid mask flags are:
(有效的掩码标志如下：)

============================ ===============================================
 KVM_MSR_EXIT_REASON_UNKNOWN intercept accesses to unknown (to KVM) MSRs
                             (拦截对 KVM 未知 (unknown) 的 MSR 的访问)
 KVM_MSR_EXIT_REASON_INVAL   intercept accesses that are architecturally
                             invalid according to the vCPU model and/or mode
                             (拦截依据 vCPU 型号和/或模式在架构上无效的访
                             问)
 KVM_MSR_EXIT_REASON_FILTER  intercept accesses that are denied by userspace
                             via KVM_X86_SET_MSR_FILTER
                             (拦截被用户态通过 KVM_X86_SET_MSR_FILTER 拒绝
                             的访问)
============================ ===============================================

7.22 KVM_CAP_X86_BUS_LOCK_EXIT(7.22 KVM_CAP_X86_BUS_LOCK_EXIT)
---------------------------------------------------------------

:Architectures: x86
:Target: VM
:Parameters: args[0] defines the policy used when bus locks detected in guest
:Returns: 0 on success, -EINVAL when args[0] contains invalid bits

(架构：x86；目标：VM；参数：args[0] 定义在客户机中检测到总线锁时使用的策略；返回
值：成功时为 0，args[0] 包含无效位时为 -EINVAL。)

Valid bits in args[0] are::

  #define KVM_BUS_LOCK_DETECTION_OFF      (1 << 0)
  #define KVM_BUS_LOCK_DETECTION_EXIT     (1 << 1)

(args[0] 中的有效位如下：)

Enabling this capability on a VM provides userspace with a way to select a
policy to handle the bus locks detected in guest. Userspace can obtain the
supported modes from the result of KVM_CHECK_EXTENSION and define it through
the KVM_ENABLE_CAP. The supported modes are mutually-exclusive.
(在 VM 上启用此能力，为用户态提供了选择策略来处理客户机中检测到的总线锁的途径。
用户态可以从 KVM_CHECK_EXTENSION 的结果获得受支持的模式，并通过 KVM_ENABLE_CAP 加
以定义。受支持的模式互斥。)

This capability allows userspace to force VM exits on bus locks detected in the
guest, irrespective whether or not the host has enabled split-lock detection
(which triggers an #AC exception that KVM intercepts). This capability is
intended to mitigate attacks where a malicious/buggy guest can exploit bus
locks to degrade the performance of the whole system.
(此能力允许用户态强制对客户机中检测到的总线锁产生 VM exit，无论宿主机是否启用了
split-lock 检测(它会触发一个被 KVM 拦截的 #AC 异常)。此能力旨在缓解恶意/有缺陷的
客户机利用总线锁拖垮整个系统性能的攻击。)

If KVM_BUS_LOCK_DETECTION_OFF is set, KVM doesn't force guest bus locks to VM
exit, although the host kernel's split-lock #AC detection still applies, if
enabled.
(如果设置了 KVM_BUS_LOCK_DETECTION_OFF，KVM 不会强制客户机总线锁引发 VM exit，不
过(如果启用了)宿主机内核的 split-lock #AC 检测仍然适用。)

If KVM_BUS_LOCK_DETECTION_EXIT is set, KVM enables a CPU feature that ensures
bus locks in the guest trigger a VM exit, and KVM exits to userspace for all
such VM exits, e.g. to allow userspace to throttle the offending guest and/or
apply some other policy-based mitigation. When exiting to userspace, KVM sets
KVM_RUN_X86_BUS_LOCK in vcpu-run->flags, and conditionally sets the exit_reason
to KVM_EXIT_X86_BUS_LOCK.
(如果设置了 KVM_BUS_LOCK_DETECTION_EXIT，KVM 会启用一个 CPU 特性，确保客户机中的
总线锁触发 VM exit，并且 KVM 会在所有这类 VM exit 时退出到用户态，例如允许用户态
对违规的客户机进行限流和/或应用其他基于策略的缓解。退出到用户态时，KVM 会在
vcpu-run->flags 中设置 KVM_RUN_X86_BUS_LOCK，并视情况把 exit_reason 设置为
KVM_EXIT_X86_BUS_LOCK。)

Due to differences in the underlying hardware implementation, the vCPU's RIP at
the time of exit diverges between Intel and AMD.  On Intel hosts, RIP points at
the next instruction, i.e. the exit is trap-like.  On AMD hosts, RIP points at
the offending instruction, i.e. the exit is fault-like.
(由于底层硬件实现的差异，退出时 vCPU 的 RIP 在 Intel 与 AMD 上有所不同。在 Intel
宿主机上，RIP 指向下一条指令，即该退出类似陷阱 (trap-like)；在 AMD 宿主机上，RIP
指向违规指令，即该退出类似故障 (fault-like)。)

Note! Detected bus locks may be coincident with other exits to userspace, i.e.
KVM_RUN_X86_BUS_LOCK should be checked regardless of the primary exit reason if
userspace wants to take action on all detected bus locks.
(注意！检测到的总线锁可能与其他退出到用户态的情形同时发生，即如果用户态想对所有
检测到的总线锁采取行动，无论主要退出原因是什么，都应检查 KVM_RUN_X86_BUS_LOCK。)

7.23 KVM_CAP_PPC_DAWR1(7.23 KVM_CAP_PPC_DAWR1)
-----------------------------------------------

:Architectures: ppc
:Parameters: none
:Returns: 0 on success, -EINVAL when CPU doesn't support 2nd DAWR

(架构：ppc；参数：无；返回值：成功时为 0，CPU 不支持第 2 个 DAWR 时为 -EINVAL。)

This capability can be used to check / enable 2nd DAWR feature provided
by POWER10 processor.
(此能力可用于检查/启用 POWER10 处理器提供的第 2 个 DAWR 特性。)


7.24 KVM_CAP_VM_COPY_ENC_CONTEXT_FROM(7.24 KVM_CAP_VM_COPY_ENC_CONTEXT_FROM)
----------------------------------------------------------------------------

:Architectures: x86 SEV enabled
:Type: vm
:Parameters: args[0] is the fd of the source vm
:Returns: 0 on success; ENOTTY on error

(架构：x86 SEV enabled；类型：vm；参数：args[0] 是源 vm 的 fd；返回值：成功时为
0，出错时为 ENOTTY。)

This capability enables userspace to copy encryption context from the vm
indicated by the fd to the vm this is called on.
(此能力使用户态能够把加密上下文从 fd 所指的 vm 复制到调用此能力的 vm。)

This is intended to support in-guest workloads scheduled by the host. This
allows the in-guest workload to maintain its own NPTs and keeps the two vms
from accidentally clobbering each other with interrupts and the like (separate
APIC/MSRs/etc).
(其目的是支持由宿主机调度的客户机内工作负载。这允许客户机内工作负载维护自己的
NPT，并避免两个 vm 因中断之类的事情而意外地相互破坏(各自独立的 APIC/MSR 等)。)

7.25 KVM_CAP_SGX_ATTRIBUTE(7.25 KVM_CAP_SGX_ATTRIBUTE)
------------------------------------------------------

:Architectures: x86
:Target: VM
:Parameters: args[0] is a file handle of a SGX attribute file in securityfs
:Returns: 0 on success, -EINVAL if the file handle is invalid or if a requested
          attribute is not supported by KVM.

(架构：x86；目标：VM；参数：args[0] 是 securityfs 中某个 SGX 属性文件的文件句
柄；返回值：成功时为 0；文件句柄无效、或所请求的属性不受 KVM 支持时为 -EINVAL。)

KVM_CAP_SGX_ATTRIBUTE enables a userspace VMM to grant a VM access to one or
more privileged enclave attributes.  args[0] must hold a file handle to a valid
SGX attribute file corresponding to an attribute that is supported/restricted
by KVM (currently only PROVISIONKEY).
(KVM_CAP_SGX_ATTRIBUTE 允许用户态 VMM 授予 VM 访问一个或多个特权 enclave 属性的
权限。args[0] 必须持有指向有效 SGX 属性文件的文件句柄，且该文件对应的属性受 KVM
支持/限制(目前只有 PROVISIONKEY)。)

The SGX subsystem restricts access to a subset of enclave attributes to provide
additional security for an uncompromised kernel, e.g. use of the PROVISIONKEY
is restricted to deter malware from using the PROVISIONKEY to obtain a stable
system fingerprint.  To prevent userspace from circumventing such restrictions
by running an enclave in a VM, KVM prevents access to privileged attributes by
default.
(SGX 子系统限制对一部分 enclave 属性的访问，为未被攻破的内核提供额外的安全性，例
如 PROVISIONKEY 的使用受到限制，以阻止恶意软件利用 PROVISIONKEY 获得稳定的系统指
纹。为了防止用户态通过在 VM 中运行 enclave 来绕过这类限制，KVM 默认禁止访问特权
属性。)

See Documentation/arch/x86/sgx.rst for more details.
(更多细节参见 Documentation/arch/x86/sgx.rst。)

7.27 KVM_CAP_EXIT_ON_EMULATION_FAILURE(7.27 KVM_CAP_EXIT_ON_EMULATION_FAILURE)
------------------------------------------------------------------------------

:Architectures: x86
:Parameters: args[0] whether the feature should be enabled or not

(架构：x86；参数：args[0] 表示该特性是否应启用。)

When this capability is enabled, an emulation failure will result in an exit
to userspace with KVM_INTERNAL_ERROR (except when the emulator was invoked
to handle a VMware backdoor instruction). Furthermore, KVM will now provide up
to 15 instruction bytes for any exit to userspace resulting from an emulation
failure.  When these exits to userspace occur use the emulation_failure struct
instead of the internal struct.  They both have the same layout, but the
emulation_failure struct matches the content better.  It also explicitly
defines the 'flags' field which is used to describe the fields in the struct
that are valid (ie: if KVM_INTERNAL_ERROR_EMULATION_FLAG_INSTRUCTION_BYTES is
set in the 'flags' field then both 'insn_size' and 'insn_bytes' have valid data
in them.)
(启用此能力后，模拟失败会导致以 KVM_INTERNAL_ERROR 退出到用户态(除非调用模拟器
是为了处理 VMware 后门指令)。此外，KVM 现在会为任何因模拟失败而产生的退出提供最
多 15 字节的指令字节。发生这类退出到用户态时，应使用 emulation_failure 结构而不
是 internal 结构。两者布局相同，但 emulation_failure 结构与内容更匹配。它还显式
定义了 'flags' 字段，用于描述结构中哪些字段是有效的(即：如果 'flags' 字段中设置
了 KVM_INTERNAL_ERROR_EMULATION_FLAG_INSTRUCTION_BYTES，则 'insn_size' 与
'insn_bytes' 都包含有效数据)。)

7.28 KVM_CAP_ARM_MTE(7.28 KVM_CAP_ARM_MTE)
------------------------------------------

:Architectures: arm64
:Parameters: none

(架构：arm64；参数：无。)

This capability indicates that KVM (and the hardware) supports exposing the
Memory Tagging Extensions (MTE) to the guest. It must also be enabled by the
VMM before creating any VCPUs to allow the guest access. Note that MTE is only
available to a guest running in AArch64 mode and enabling this capability will
cause attempts to create AArch32 VCPUs to fail.
(此能力表明 KVM(及硬件)支持把内存标记扩展 (MTE) 暴露给客户机。VMM 还必须在创建
任何 VCPU 之前启用它，才能允许客户机访问。注意，MTE 只对以 AArch64 模式运行的客
户机可用，启用此能力会导致创建 AArch32 VCPU 的尝试失败。)

When enabled the guest is able to access tags associated with any memory given
to the guest. KVM will ensure that the tags are maintained during swap or
hibernation of the host; however the VMM needs to manually save/restore the
tags as appropriate if the VM is migrated.
(启用后，客户机能够访问与分配给它的任何内存相关联的标签。KVM 会确保在宿主机发生
交换 (swap) 或休眠时维护这些标签；但如果 VM 发生迁移，VMM 需要酌情手动保存/恢复
标签。)

When this capability is enabled all memory in memslots must be mapped as
``MAP_ANONYMOUS`` or with a RAM-based file mapping (``tmpfs``, ``memfd``),
attempts to create a memslot with an invalid mmap will result in an
-EINVAL return.
(启用此能力后，memslot 中的所有内存都必须以 ``MAP_ANONYMOUS`` 或基于 RAM 的文件
映射(``tmpfs``、``memfd``)方式映射；用无效 mmap 创建 memslot 的尝试会导致返回
-EINVAL。)

When enabled the VMM may make use of the ``KVM_ARM_MTE_COPY_TAGS`` ioctl to
perform a bulk copy of tags to/from the guest.
(启用后，VMM 可以使用 ``KVM_ARM_MTE_COPY_TAGS`` ioctl 在客户机与用户态之间批量复
制标签。)

7.29 KVM_CAP_VM_MOVE_ENC_CONTEXT_FROM(7.29 KVM_CAP_VM_MOVE_ENC_CONTEXT_FROM)
----------------------------------------------------------------------------

:Architectures: x86 SEV enabled
:Type: vm
:Parameters: args[0] is the fd of the source vm
:Returns: 0 on success

(架构：x86 SEV enabled；类型：vm；参数：args[0] 是源 vm 的 fd；返回值：成功时为
0。)

This capability enables userspace to migrate the encryption context from the VM
indicated by the fd to the VM this is called on.
(此能力使用户态能够把加密上下文从 fd 所指的 VM 迁移到调用此能力的 VM。)

This is intended to support intra-host migration of VMs between userspace VMMs,
upgrading the VMM process without interrupting the guest.
(其目的是支持在用户态 VMM 之间进行 VM 的宿主机内迁移，在不中断客户机的情况下升
级 VMM 进程。)

7.31 KVM_CAP_DISABLE_QUIRKS2(7.31 KVM_CAP_DISABLE_QUIRKS2)
----------------------------------------------------------

:Parameters: args[0] - set of KVM quirks to disable
:Architectures: x86
:Type: vm

(参数：args[0] - 要禁用的 KVM quirk 集合；架构：x86；类型：vm。)

This capability, if enabled, will cause KVM to disable some behavior
quirks.
(此能力(如果启用)会使 KVM 禁用一些行为怪癖 (quirk)。)

Calling KVM_CHECK_EXTENSION for this capability returns a bitmask of
quirks that can be disabled in KVM.
(对此能力调用 KVM_CHECK_EXTENSION 会返回一个位掩码，表示 KVM 中可以禁用的
quirks。)

The argument to KVM_ENABLE_CAP for this capability is a bitmask of
quirks to disable, and must be a subset of the bitmask returned by
KVM_CHECK_EXTENSION.
(此能力传给 KVM_ENABLE_CAP 的参数是要禁用的 quirks 位掩码，必须是
KVM_CHECK_EXTENSION 所返回位掩码的子集。)

The valid bits in cap.args[0] are:
(cap.args[0] 中的有效位如下：)

========================================   ================================================
KVM_X86_QUIRK_LINT0_REENABLED              By default, the reset value for the LVT
                                           LINT0 register is 0x700 (APIC_MODE_EXTINT).
                                           When this quirk is disabled, the reset value
                                           is 0x10000 (APIC_LVT_MASKED).
                                           (默认情况下，LVT LINT0 寄存器的复位值为
                                           0x700 (APIC_MODE_EXTINT)。禁用此 quirk
                                           后，复位值为 0x10000 (APIC_LVT_MASKED)。)

KVM_X86_QUIRK_CD_NW_CLEARED                By default, KVM clears CR0.CD and CR0.NW on
                                           AMD CPUs to workaround buggy guest firmware
                                           that runs in perpetuity with CR0.CD, i.e.
                                           with caches in "no fill" mode.
                                           (默认情况下，KVM 会在 AMD CPU 上清除
                                           CR0.CD 与 CR0.NW，以规避一直以 CR0.CD 运
                                           行(即缓存处于 "no fill" 模式)的有缺陷客
                                           户机固件。)

                                           When this quirk is disabled, KVM does not
                                           change the value of CR0.CD and CR0.NW.
                                           (禁用此 quirk 后，KVM 不再改变 CR0.CD 与
                                           CR0.NW 的值。)

KVM_X86_QUIRK_LAPIC_MMIO_HOLE              By default, the MMIO LAPIC interface is
                                           available even when configured for x2APIC
                                           mode. When this quirk is disabled, KVM
                                           disables the MMIO LAPIC interface if the
                                           LAPIC is in x2APIC mode.
                                           (默认情况下，即使配置为 x2APIC 模式，
                                           MMIO LAPIC 接口仍然可用。禁用此 quirk
                                           后，如果 LAPIC 处于 x2APIC 模式，KVM 会
                                           禁用 MMIO LAPIC 接口。)

KVM_X86_QUIRK_OUT_7E_INC_RIP               By default, KVM pre-increments %rip before
                                           exiting to userspace for an OUT instruction
                                           to port 0x7e. When this quirk is disabled,
                                           KVM does not pre-increment %rip before
                                           exiting to userspace.
                                           (默认情况下，对于发往端口 0x7e 的 OUT 指
                                           令，KVM 在退出到用户态之前会预先递增
                                           %rip。禁用此 quirk 后，KVM 在退出到用户态
                                           之前不再预递增 %rip。)

KVM_X86_QUIRK_MISC_ENABLE_NO_MWAIT         When this quirk is disabled, KVM sets
                                           CPUID.01H:ECX[bit 3] (MONITOR/MWAIT) if
                                           IA32_MISC_ENABLE[bit 18] (MWAIT) is set.
                                           Additionally, when this quirk is disabled,
                                           KVM clears CPUID.01H:ECX[bit 3] if
                                           IA32_MISC_ENABLE[bit 18] is cleared.
                                           (禁用此 quirk 后，如果
                                           IA32_MISC_ENABLE[bit 18] (MWAIT) 置位，
                                           KVM 会设置 CPUID.01H:ECX[bit 3]
                                           (MONITOR/MWAIT)。此外，如果
                                           IA32_MISC_ENABLE[bit 18] 被清除，KVM 会清
                                           除 CPUID.01H:ECX[bit 3]。)

KVM_X86_QUIRK_FIX_HYPERCALL_INSN           By default, KVM rewrites guest
                                           VMMCALL/VMCALL instructions to match the
                                           vendor's hypercall instruction for the
                                           system. When this quirk is disabled, KVM
                                           will no longer rewrite invalid guest
                                           hypercall instructions. Executing the
                                           incorrect hypercall instruction will
                                           generate a #UD within the guest.
                                           (默认情况下，KVM 会改写客户机的
                                           VMMCALL/VMCALL 指令，使其匹配本系统所用厂
                                           商的超级调用指令。禁用此 quirk 后，KVM 不
                                           再改写无效的客户机超级调用指令。执行错误
                                           的超级调用指令会在客户机内产生 #UD。)

KVM_X86_QUIRK_MWAIT_NEVER_UD_FAULTS        By default, KVM emulates MONITOR/MWAIT (if
                                           they are intercepted) as NOPs regardless of
                                           whether or not MONITOR/MWAIT are supported
                                           according to guest CPUID.  When this quirk
                                           is disabled and KVM_X86_DISABLE_EXITS_MWAIT
                                           is not set (MONITOR/MWAIT are intercepted),
                                           KVM will inject a #UD on MONITOR/MWAIT if
                                           they're unsupported per guest CPUID.  Note,
                                           KVM will modify MONITOR/MWAIT support in
                                           guest CPUID on writes to MISC_ENABLE if
                                           KVM_X86_QUIRK_MISC_ENABLE_NO_MWAIT is
                                           disabled.
                                           (默认情况下，无论客户机 CPUID 是否支持
                                           MONITOR/MWAIT，KVM 都会把 MONITOR/MWAIT
                                           (若被拦截)模拟为 NOP。禁用此 quirk 且未设
                                           置 KVM_X86_DISABLE_EXITS_MWAIT(MONITOR/
                                           MWAIT 被拦截)时，若客户机 CPUID 不支持
                                           MONITOR/MWAIT，KVM 会在 MONITOR/MWAIT 上
                                           注入 #UD。注意，如果
                                           KVM_X86_QUIRK_MISC_ENABLE_NO_MWAIT 被禁
                                           用，KVM 会在写入 MISC_ENABLE 时修改客户机
                                           CPUID 中的 MONITOR/MWAIT 支持。)

KVM_X86_QUIRK_SLOT_ZAP_ALL                 By default, for KVM_X86_DEFAULT_VM VMs, KVM
                                           invalidates all SPTEs in all memslots and
                                           address spaces when a memslot is deleted or
                                           moved.  When this quirk is disabled (or the
                                           VM type isn't KVM_X86_DEFAULT_VM), KVM only
                                           ensures the backing memory of the deleted
                                           or moved memslot isn't reachable, i.e KVM
                                           _may_ invalidate only SPTEs related to the
                                           memslot.
                                           (默认情况下，对于 KVM_X86_DEFAULT_VM 类型
                                           的 VM，当某个 memslot 被删除或移动时，
                                           KVM 会使所有 memslot 与地址空间中的所有
                                           SPTE 失效。禁用此 quirk(或 VM 类型不是
                                           KVM_X86_DEFAULT_VM)后，KVM 只确保被删除
                                           或移动的 memslot 的后备内存不可达，即
                                           KVM _may_ 只使与该 memslot 相关的 SPTE 失
                                           效。)

KVM_X86_QUIRK_STUFF_FEATURE_MSRS           By default, at vCPU creation, KVM sets the
                                           vCPU's MSR_IA32_PERF_CAPABILITIES (0x345),
                                           MSR_IA32_ARCH_CAPABILITIES (0x10a),
                                           MSR_PLATFORM_INFO (0xce), and all VMX MSRs
                                           (0x480..0x492) to the maximal capabilities
                                           supported by KVM.  KVM also sets
                                           MSR_IA32_UCODE_REV (0x8b) to an arbitrary
                                           value (which is different for Intel vs.
                                           AMD).  Lastly, when guest CPUID is set (by
                                           userspace), KVM modifies select VMX MSR
                                           fields to force consistency between guest
                                           CPUID and L2's effective ISA.  When this
                                           quirk is disabled, KVM zeroes the vCPU's MSR
                                           values (with two exceptions, see below),
                                           i.e. treats the feature MSRs like CPUID
                                           leaves and gives userspace full control of
                                           the vCPU model definition.  This quirk does
                                           not affect VMX MSRs CR0/CR4_FIXED1 (0x487
                                           and 0x489), as KVM does now allow them to
                                           be set by userspace (KVM sets them based on
                                           guest CPUID, for safety purposes).
                                           (默认情况下，在 vCPU 创建时，KVM 会把
                                           vCPU 的 MSR_IA32_PERF_CAPABILITIES
                                           (0x345)、MSR_IA32_ARCH_CAPABILITIES
                                           (0x10a)、MSR_PLATFORM_INFO (0xce) 以及所
                                           有 VMX MSR (0x480..0x492) 设置为 KVM 支持
                                           的最大能力。KVM 还会把 MSR_IA32_UCODE_REV
                                           (0x8b) 设置为一个任意值(Intel 与 AMD 上该
                                           值不同)。最后，当(由用户态)设置客户机
                                           CPUID 时，KVM 会修改选定的 VMX MSR 字段，
                                           以强制客户机 CPUID 与 L2 的有效 ISA 保持一
                                           致。禁用此 quirk 后，KVM 会把 vCPU 的 MSR
                                           值清零(有两个例外，见下文)，即像对待
                                           CPUID 叶一样对待这些特性 MSR，把 vCPU 型号
                                           的定义完全交给用户态控制。此 quirk 不影响
                                           VMX MSR CR0/CR4_FIXED1 (0x487 与
                                           0x489)，因为 KVM 不允许用户态设置它们
                                           (出于安全考虑，KVM 根据客户机 CPUID 来设
                                           置它们)。)

KVM_X86_QUIRK_IGNORE_GUEST_PAT             By default, on Intel platforms, KVM ignores
                                           guest PAT and forces the effective memory
                                           type to WB in EPT.  The quirk is not available
                                           on Intel platforms which are incapable of
                                           safely honoring guest PAT (i.e., without CPU
                                           self-snoop, KVM always ignores guest PAT and
                                           forces effective memory type to WB).  It is
                                           also ignored on AMD platforms or, on Intel,
                                           when a VM has non-coherent DMA devices
                                           assigned; KVM always honors guest PAT in
                                           such case. The quirk is needed to avoid
                                           slowdowns on certain Intel Xeon platforms
                                           (e.g. ICX, SPR) where self-snoop feature is
                                           supported but UC is slow enough to cause
                                           issues with some older guests that use
                                           UC instead of WC to map the video RAM.
                                           Userspace can disable the quirk to honor
                                           guest PAT if it knows that there is no such
                                           guest software, for example if it does not
                                           expose a bochs graphics device (which is
                                           known to have had a buggy driver).
                                           (默认情况下，在 Intel 平台上，KVM 忽略客户
                                           机 PAT，并在 EPT 中强制有效内存类型为
                                           WB。在无法安全遵循客户机 PAT 的 Intel 平
                                           台上，此 quirk 不可用(即没有 CPU
                                           self-snoop 时，KVM 总是忽略客户机 PAT 并强
                                           制有效内存类型为 WB)。在 AMD 平台上，或在
                                           Intel 上当 VM 分配了非一致性 DMA 设备时，
                                           该 quirk 也会被忽略；这类情况下 KVM 总是遵
                                           循客户机 PAT。某些 Intel Xeon 平台(例如
                                           ICX、SPR)支持 self-snoop 特性，但 UC 足够
                                           慢，会导致一些用 UC 而非 WC 映射显存的老客
                                           户机出现问题；此 quirk 正是为了避免这类平
                                           台上的性能下降而需要的。如果用户态确定不
                                           存在这类客户机软件——例如它不暴露 bochs
                                           显卡设备(已知其驱动有 bug)——可以禁用此
                                           quirk 以遵循客户机 PAT。)

KVM_X86_QUIRK_VMCS12_ALLOW_FREEZE_IN_SMM   By default, KVM relaxes the consistency
                                           check for GUEST_IA32_DEBUGCTL in vmcs12
                                           to allow FREEZE_IN_SMM to be set.  When
                                           this quirk is disabled, KVM requires this
                                           bit to be cleared.  Note that the vmcs02
                                           bit is still completely controlled by the
                                           host, regardless of the quirk setting.
                                           (默认情况下，KVM 放宽 vmcs12 中
                                           GUEST_IA32_DEBUGCTL 的一致性检查，以允许
                                           设置 FREEZE_IN_SMM。禁用此 quirk 后，
                                           KVM 要求清除此位。注意，无论 quirk 设置如
                                           何，vmcs02 的该位仍完全由宿主机控制。)
========================================   ================================================

7.32 KVM_CAP_MAX_VCPU_ID(7.32 KVM_CAP_MAX_VCPU_ID)
--------------------------------------------------

:Architectures: x86
:Target: VM
:Parameters: args[0] - maximum APIC ID value set for current VM
:Returns: 0 on success, -EINVAL if args[0] is beyond KVM_MAX_VCPU_IDS
          supported in KVM or if it has been set.

(架构：x86；目标：VM；参数：args[0] - 为当前 VM 设置的最大 APIC ID 值；返回值：成
功时为 0；args[0] 超出 KVM 所支持的 KVM_MAX_VCPU_IDS、或该值已被设置时为
-EINVAL。)

This capability allows userspace to specify maximum possible APIC ID
assigned for current VM session prior to the creation of vCPUs, saving
memory for data structures indexed by the APIC ID.  Userspace is able
to calculate the limit to APIC ID values from designated
CPU topology.
(此能力允许用户态在创建 vCPU 之前，为当前 VM 会话指定可能的最大 APIC ID，从而为
按 APIC ID 索引的数据结构节省内存。用户态能够根据给定的 CPU 拓扑计算出 APIC ID 取
值的上限。)

The value can be changed only until KVM_ENABLE_CAP is set to a nonzero
value or until a vCPU is created.  Upon creation of the first vCPU,
if the value was set to zero or KVM_ENABLE_CAP was not invoked, KVM
uses the return value of KVM_CHECK_EXTENSION(KVM_CAP_MAX_VCPU_ID) as
the maximum APIC ID.
(该值只能修改到 KVM_ENABLE_CAP 被设置为非零值、或创建了一个 vCPU 为止。创建第一
个 vCPU 时，如果该值被设为 0 或未调用过 KVM_ENABLE_CAP，KVM 会使用
KVM_CHECK_EXTENSION(KVM_CAP_MAX_VCPU_ID) 的返回值作为最大 APIC ID。)

7.33 KVM_CAP_X86_NOTIFY_VMEXIT(7.33 KVM_CAP_X86_NOTIFY_VMEXIT)
--------------------------------------------------------------

:Architectures: x86
:Target: VM
:Parameters: args[0] is the value of notify window as well as some flags
:Returns: 0 on success, -EINVAL if args[0] contains invalid flags or notify
          VM exit is unsupported.

(架构：x86；目标：VM；参数：args[0] 是 notify window 的值以及一些标志；返回值：成
功时为 0；args[0] 包含无效标志、或 notify VM exit 不受支持时为 -EINVAL。)

Bits 63:32 of args[0] are used for notify window.
Bits 31:0 of args[0] are for some flags. Valid bits are::

  #define KVM_X86_NOTIFY_VMEXIT_ENABLED    (1 << 0)
  #define KVM_X86_NOTIFY_VMEXIT_USER       (1 << 1)

(args[0] 的位 63:32 用于 notify window。位 31:0 用于一些标志。有效位如下：)

This capability allows userspace to configure the notify VM exit on/off
in per-VM scope during VM creation. Notify VM exit is disabled by default.
When userspace sets KVM_X86_NOTIFY_VMEXIT_ENABLED bit in args[0], VMM will
enable this feature with the notify window provided, which will generate
a VM exit if no event window occurs in VM non-root mode for a specified of
time (notify window).
(此能力允许用户态在创建 VM 时按 VM 范围配置 notify VM exit 的开/关。notify VM
exit 默认禁用。当用户态在 args[0] 中设置 KVM_X86_NOTIFY_VMEXIT_ENABLED 位时，
VMM 将以所提供的 notify window 启用该特性：如果 VM 非 root 模式在指定的一段时间
(notify window) 内没有出现事件窗口，就会产生一次 VM exit。)

If KVM_X86_NOTIFY_VMEXIT_USER is set in args[0], upon notify VM exits happen,
KVM would exit to userspace for handling.
(如果在 args[0] 中设置了 KVM_X86_NOTIFY_VMEXIT_USER，则发生 notify VM exit 时，
KVM 会退出到用户态进行处理。)

This capability is aimed to mitigate the threat that malicious VMs can
cause CPU stuck (due to event windows don't open up) and make the CPU
unavailable to host or other VMs.
(此能力旨在缓解恶意 VM 造成 CPU 卡死(由于事件窗口无法打开)、使宿主机或其他 VM
无法使用该 CPU 的威胁。)

7.35 KVM_CAP_X86_APIC_BUS_CYCLES_NS(7.35 KVM_CAP_X86_APIC_BUS_CYCLES_NS)
------------------------------------------------------------------------

:Architectures: x86
:Target: VM
:Parameters: args[0] is the desired APIC bus clock rate, in nanoseconds
:Returns: 0 on success, -EINVAL if args[0] contains an invalid value for the
          frequency or if any vCPUs have been created, -ENXIO if a virtual
          local APIC has not been created using KVM_CREATE_IRQCHIP.

(架构：x86；目标：VM；参数：args[0] 是期望的 APIC 总线时钟频率，单位为纳秒；返回
值：成功时为 0；args[0] 中的频率值无效、或已创建任何 vCPU 时为 -EINVAL；未使用
KVM_CREATE_IRQCHIP 创建虚拟本地 APIC 时为 -ENXIO。)

This capability sets the VM's APIC bus clock frequency, used by KVM's in-kernel
virtual APIC when emulating APIC timers.  KVM's default value can be retrieved
by KVM_CHECK_EXTENSION.
(此能力设置 VM 的 APIC 总线时钟频率，KVM 的内核内虚拟 APIC 在模拟 APIC 定时器时会
使用它。KVM 的默认值可以通过 KVM_CHECK_EXTENSION 获取。)

Note: Userspace is responsible for correctly configuring CPUID 0x15, a.k.a. the
core crystal clock frequency, if a non-zero CPUID 0x15 is exposed to the guest.
(注意：如果向客户机暴露了非零的 CPUID 0x15(即核心晶振时钟频率)，用户态负责正确
配置它。)

7.36 KVM_CAP_DIRTY_LOG_RING/KVM_CAP_DIRTY_LOG_RING_ACQ_REL(7.36 KVM_CAP_DIRTY_LOG_RING/KVM_CAP_DIRTY_LOG_RING_ACQ_REL)
----------------------------------------------------------------------------------------------------------------------

:Architectures: x86, arm64, riscv
:Type: vm
:Parameters: args[0] - size of the dirty log ring

(架构：x86, arm64, riscv；类型：vm；参数：args[0] - 脏日志环形缓冲区的大小。)

KVM is capable of tracking dirty memory using ring buffers that are
mmapped into userspace; there is one dirty ring per vcpu.
(KVM 能够使用 mmap 到用户态的环形缓冲区来跟踪脏内存；每个 vcpu 有一个脏页环
(dirty ring)。)

The dirty ring is available to userspace as an array of
``struct kvm_dirty_gfn``.  Each dirty entry is defined as::

  struct kvm_dirty_gfn {
          __u32 flags;
          __u32 slot; /* as_id | slot_id */
          __u64 offset;
  };

(脏页环以 ``struct kvm_dirty_gfn`` 数组的形式提供给用户态。每个脏条目定义如下：)

The following values are defined for the flags field to define the
current state of the entry::

  #define KVM_DIRTY_GFN_F_DIRTY           BIT(0)
  #define KVM_DIRTY_GFN_F_RESET           BIT(1)
  #define KVM_DIRTY_GFN_F_MASK            0x3

(为描述条目的当前状态，flags 字段定义了以下取值：)

Userspace should call KVM_ENABLE_CAP ioctl right after KVM_CREATE_VM
ioctl to enable this capability for the new guest and set the size of
the rings.  Enabling the capability is only allowed before creating any
vCPU, and the size of the ring must be a power of two.  The larger the
ring buffer, the less likely the ring is full and the VM is forced to
exit to userspace. The optimal size depends on the workload, but it is
recommended that it be at least 64 KiB (4096 entries).
(用户态应在 KVM_CREATE_VM ioctl 之后立即调用 KVM_ENABLE_CAP ioctl，为新客户机启
用此能力并设置环形缓冲区的大小。只有在创建任何 vCPU 之前才允许启用该能力，且环形
缓冲区的大小必须是 2 的幂。环形缓冲区越大，环被填满并迫使 VM 退出到用户态的可能
性就越小。最佳大小取决于工作负载，但建议至少为 64 KiB(4096 个条目)。)

Just like for dirty page bitmaps, the buffer tracks writes to
all user memory regions for which the KVM_MEM_LOG_DIRTY_PAGES flag was
set in KVM_SET_USER_MEMORY_REGION.  Once a memory region is registered
with the flag set, userspace can start harvesting dirty pages from the
ring buffer.
(与脏页位图一样，该缓冲区跟踪对 KVM_SET_USER_MEMORY_REGION 中设置了
KVM_MEM_LOG_DIRTY_PAGES 标志的所有用户内存区域的写操作。一旦某个内存区域在设置了
该标志的情况下完成注册，用户态就可以开始从环形缓冲区收集脏页。)

An entry in the ring buffer can be unused (flag bits ``00``),
dirty (flag bits ``01``) or harvested (flag bits ``1X``).  The
state machine for the entry is as follows::

          dirtied         harvested        reset
     00 -----------> 01 -------------> 1X -------+
      ^                                          |
      |                                          |
      +------------------------------------------+

(环形缓冲区中的条目可以是未使用(flag 位 ``00``)、脏(flag 位 ``01``)或已收集
(flag 位 ``1X``)状态。条目的状态机如下：)

To harvest the dirty pages, userspace accesses the mmapped ring buffer
to read the dirty GFNs.  If the flags has the DIRTY bit set (at this stage
the RESET bit must be cleared), then it means this GFN is a dirty GFN.
The userspace should harvest this GFN and mark the flags from state
``01b`` to ``1Xb`` (bit 0 will be ignored by KVM, but bit 1 must be set
to show that this GFN is harvested and waiting for a reset), and move
on to the next GFN.  The userspace should continue to do this until the
flags of a GFN have the DIRTY bit cleared, meaning that it has harvested
all the dirty GFNs that were available.
(要收集脏页，用户态访问 mmap 的环形缓冲区来读取脏 GFN。如果 flags 置了 DIRTY 位
(此时 RESET 位必须已被清除)，则表示该 GFN 是一个脏 GFN。用户态应收集该 GFN，并
把 flags 从状态 ``01b`` 标记为 ``1Xb`` (位 0 会被 KVM 忽略，但位 1 必须置位，以表
明该 GFN 已被收集、正在等待复位)，然后继续处理下一个 GFN。用户态应持续这一过程，
直到某个 GFN 的 flags 清除了 DIRTY 位，即表示所有可用的脏 GFN 都已被收集。)

Note that on weakly ordered architectures, userspace accesses to the
ring buffer (and more specifically the 'flags' field) must be ordered,
using load-acquire/store-release accessors when available, or any
other memory barrier that will ensure this ordering.
(注意，在弱序 (weakly ordered) 架构上，用户态对环形缓冲区(更具体地说是 'flags'
字段)的访问必须保持顺序：在可用时使用 load-acquire/store-release 访问器，或使用
任何其他能确保这种顺序的内存屏障。)

It's not necessary for userspace to harvest the all dirty GFNs at once.
However it must collect the dirty GFNs in sequence, i.e., the userspace
program cannot skip one dirty GFN to collect the one next to it.
(用户态不必一次性收集所有脏 GFN。但必须按顺序收集脏 GFN，即用户态程序不能跳过某
个脏 GFN 去收集它旁边的那个。)

After processing one or more entries in the ring buffer, userspace
calls the VM ioctl KVM_RESET_DIRTY_RINGS to notify the kernel about
it, so that the kernel will reprotect those collected GFNs.
Therefore, the ioctl must be called *before* reading the content of
the dirty pages.
(处理完环形缓冲区中的一个或多个条目后，用户态调用 VM ioctl
KVM_RESET_DIRTY_RINGS 把这一点通知内核，使内核对这些已收集的 GFN 重新施加保护。
因此，该 ioctl 必须在读取脏页内容*之前*调用。)

The dirty ring can get full.  When it happens, the KVM_RUN of the
vcpu will return with exit reason KVM_EXIT_DIRTY_RING_FULL.
(脏页环可能被填满。发生这种情况时，该 vcpu 的 KVM_RUN 会以退出原因
KVM_EXIT_DIRTY_RING_FULL 返回。)

The dirty ring interface has a major difference comparing to the
KVM_GET_DIRTY_LOG interface in that, when reading the dirty ring from
userspace, it's still possible that the kernel has not yet flushed the
processor's dirty page buffers into the kernel buffer (with dirty bitmaps, the
flushing is done by the KVM_GET_DIRTY_LOG ioctl).  To achieve that, one
needs to kick the vcpu out of KVM_RUN using a signal.  The resulting
vmexit ensures that all dirty GFNs are flushed to the dirty rings.
(脏页环接口与 KVM_GET_DIRTY_LOG 接口有一个重大区别：当从用户态读取脏页环时，内核
可能尚未把处理器的脏页缓冲区刷入内核缓冲区(使用脏位图时，刷新由 KVM_GET_DIRTY_LOG
ioctl 完成)。要达到这个目的，需要用信号把 vcpu 从 KVM_RUN 中踢出来。由此产生的
vmexit 确保所有脏 GFN 都被刷入脏页环。)

NOTE: KVM_CAP_DIRTY_LOG_RING_ACQ_REL is the only capability that
should be exposed by weakly ordered architecture, in order to indicate
the additional memory ordering requirements imposed on userspace when
reading the state of an entry and mutating it from DIRTY to HARVESTED.
Architecture with TSO-like ordering (such as x86) are allowed to
expose both KVM_CAP_DIRTY_LOG_RING and KVM_CAP_DIRTY_LOG_RING_ACQ_REL
to userspace.
(注意：KVM_CAP_DIRTY_LOG_RING_ACQ_REL 是弱序架构应当唯一暴露的能力，用于表明在
读取条目状态并把该条目从 DIRTY 修改为 HARVESTED 时，对用户态施加的额外内存顺序要
求。具有 TSO 类顺序的架构(例如 x86)允许同时向用户态暴露 KVM_CAP_DIRTY_LOG_RING
与 KVM_CAP_DIRTY_LOG_RING_ACQ_REL。)

After enabling the dirty rings, the userspace needs to detect the
capability of KVM_CAP_DIRTY_LOG_RING_WITH_BITMAP to see whether the
ring structures can be backed by per-slot bitmaps. With this capability
advertised, it means the architecture can dirty guest pages without
vcpu/ring context, so that some of the dirty information will still be
maintained in the bitmap structure. KVM_CAP_DIRTY_LOG_RING_WITH_BITMAP
can't be enabled if the capability of KVM_CAP_DIRTY_LOG_RING_ACQ_REL
hasn't been enabled, or any memslot has been existing.
(启用脏页环之后，用户态需要探测 KVM_CAP_DIRTY_LOG_RING_WITH_BITMAP 能力，以确定环
形结构是否可以用每 slot 的位图作后备。通告此能力意味着该架构可以在没有 vcpu/环上
下文的情况下弄脏客户机页，因此部分脏信息仍会维护在位图结构中。如果尚未启用
KVM_CAP_DIRTY_LOG_RING_ACQ_REL 能力、或已存在任何 memslot，则不能启用
KVM_CAP_DIRTY_LOG_RING_WITH_BITMAP。)

Note that the bitmap here is only a backup of the ring structure. The
use of the ring and bitmap combination is only beneficial if there is
only a very small amount of memory that is dirtied out of vcpu/ring
context. Otherwise, the stand-alone per-slot bitmap mechanism needs to
be considered.
(注意，这里的位图只是环形结构的后备。只有在 vcpu/环上下文之外被弄脏的内存非常少
时，环与位图的组合才有收益；否则，需要考虑独立的每 slot 位图机制。)

To collect dirty bits in the backup bitmap, userspace can use the same
KVM_GET_DIRTY_LOG ioctl. KVM_CLEAR_DIRTY_LOG isn't needed as long as all
the generation of the dirty bits is done in a single pass. Collecting
the dirty bitmap should be the very last thing that the VMM does before
considering the state as complete. VMM needs to ensure that the dirty
state is final and avoid missing dirty pages from another ioctl ordered
after the bitmap collection.
(要收集后备位图中的脏位，用户态可以使用同一个 KVM_GET_DIRTY_LOG ioctl。只要所有
脏位的生成都在一次遍历中完成，就不需要 KVM_CLEAR_DIRTY_LOG。收集脏位图应当是
VMM 在认定状态完成之前做的最后一件事。VMM 需要确保脏状态是最终的，并避免遗漏在
位图收集之后按顺序执行的另一个 ioctl 所产生的脏页。)

NOTE: Multiple examples of using the backup bitmap: (1) save vgic/its
tables through command KVM_DEV_ARM_{VGIC_GRP_CTRL, ITS_SAVE_TABLES} on
KVM device "kvm-arm-vgic-its". (2) restore vgic/its tables through
command KVM_DEV_ARM_{VGIC_GRP_CTRL, ITS_RESTORE_TABLES} on KVM device
"kvm-arm-vgic-its". VGICv3 LPI pending status is restored. (3) save
vgic3 pending table through KVM_DEV_ARM_VGIC_{GRP_CTRL, SAVE_PENDING_TABLES}
command on KVM device "kvm-arm-vgic-v3".
(注意：使用后备位图的几个例子：(1) 通过 KVM 设备 "kvm-arm-vgic-its" 上的命令
KVM_DEV_ARM_{VGIC_GRP_CTRL, ITS_SAVE_TABLES} 保存 vgic/its 表。(2) 通过 KVM 设备
"kvm-arm-vgic-its" 上的命令 KVM_DEV_ARM_{VGIC_GRP_CTRL, ITS_RESTORE_TABLES} 恢复
vgic/its 表，VGICv3 LPI 挂起状态会被恢复。(3) 通过 KVM 设备 "kvm-arm-vgic-v3" 上
的 KVM_DEV_ARM_VGIC_{GRP_CTRL, SAVE_PENDING_TABLES} 命令保存 vgic3 挂起表。)

7.37 KVM_CAP_PMU_CAPABILITY(7.37 KVM_CAP_PMU_CAPABILITY)
--------------------------------------------------------

:Architectures: x86
:Type: vm
:Parameters: arg[0] is bitmask of PMU virtualization capabilities.
:Returns: 0 on success, -EINVAL when arg[0] contains invalid bits

(架构：x86；类型：vm；参数：arg[0] 是 PMU 虚拟化能力的位掩码；返回值：成功时为
0，arg[0] 包含无效位时为 -EINVAL。)

This capability alters PMU virtualization in KVM.
(此能力会改变 KVM 中的 PMU 虚拟化。)

Calling KVM_CHECK_EXTENSION for this capability returns a bitmask of
PMU virtualization capabilities that can be adjusted on a VM.
(对此能力调用 KVM_CHECK_EXTENSION 会返回一个位掩码，表示可以在 VM 上调整的 PMU 虚
拟化能力。)

The argument to KVM_ENABLE_CAP is also a bitmask and selects specific
PMU virtualization capabilities to be applied to the VM.  This can
only be invoked on a VM prior to the creation of VCPUs.
(传给 KVM_ENABLE_CAP 的参数同样是位掩码，用于选择要应用到该 VM 的具体 PMU 虚拟化
能力。它只能在创建 VCPU 之前在 VM 上调用。)

At this time, KVM_PMU_CAP_DISABLE is the only capability.  Setting
this capability will disable PMU virtualization for that VM.  Usermode
should adjust CPUID leaf 0xA to reflect that the PMU is disabled.
(目前，KVM_PMU_CAP_DISABLE 是唯一的能力。设置它将禁用该 VM 的 PMU 虚拟化。用户态
程序应调整 CPUID 叶 0xA 以反映 PMU 已被禁用。)

7.38 KVM_CAP_VM_DISABLE_NX_HUGE_PAGES(7.38 KVM_CAP_VM_DISABLE_NX_HUGE_PAGES)
----------------------------------------------------------------------------

:Architectures: x86
:Type: vm
:Parameters: arg[0] must be 0.
:Returns: 0 on success, -EPERM if the userspace process does not
          have CAP_SYS_BOOT, -EINVAL if args[0] is not 0 or any vCPUs have been
          created.

(架构：x86；类型：vm；参数：arg[0] 必须为 0；返回值：成功时为 0；用户态进程没有
CAP_SYS_BOOT 时为 -EPERM；args[0] 不为 0 或已创建任何 vCPU 时为 -EINVAL。)

This capability disables the NX huge pages mitigation for iTLB MULTIHIT.
(此能力禁用针对 iTLB MULTIHIT 的 NX 巨页缓解措施。)

The capability has no effect if the nx_huge_pages module parameter is not set.
(如果未设置 nx_huge_pages 模块参数，此能力没有效果。)

This capability may only be set before any vCPUs are created.
(此能力只能在创建任何 vCPU 之前设置。)

7.39 KVM_CAP_ARM_EAGER_SPLIT_CHUNK_SIZE(7.39 KVM_CAP_ARM_EAGER_SPLIT_CHUNK_SIZE)
--------------------------------------------------------------------------------

:Architectures: arm64
:Type: vm
:Parameters: arg[0] is the new split chunk size.
:Returns: 0 on success, -EINVAL if any memslot was already created.

(架构：arm64；类型：vm；参数：arg[0] 是新的拆分块 (split chunk) 大小；返回值：成
功时为 0，已创建任何 memslot 时为 -EINVAL。)

This capability sets the chunk size used in Eager Page Splitting.
(此能力设置 Eager Page Splitting(急切页拆分)所使用的块大小。)

Eager Page Splitting improves the performance of dirty-logging (used
in live migrations) when guest memory is backed by huge-pages.  It
avoids splitting huge-pages (into PAGE_SIZE pages) on fault, by doing
it eagerly when enabling dirty logging (with the
KVM_MEM_LOG_DIRTY_PAGES flag for a memory region), or when using
KVM_CLEAR_DIRTY_LOG.
(当客户机内存由巨页作后备时，Eager Page Splitting 能提升脏日志记录(用于热迁移)的
性能。它避免在缺页时才拆分巨页(拆成 PAGE_SIZE 页)，而是在启用脏日志(为内存区域设
置 KVM_MEM_LOG_DIRTY_PAGES 标志)或使用 KVM_CLEAR_DIRTY_LOG 时急切地完成拆分。)

The chunk size specifies how many pages to break at a time, using a
single allocation for each chunk. Bigger the chunk size, more pages
need to be allocated ahead of time.
(块大小指定一次拆分多少页，每个块使用一次分配。块大小越大，需要提前分配的页就越
多。)

The chunk size needs to be a valid block size. The list of acceptable
block sizes is exposed in KVM_CAP_ARM_SUPPORTED_BLOCK_SIZES as a
64-bit bitmap (each bit describing a block size). The default value is
0, to disable the eager page splitting.
(块大小必须是有效的块尺寸。可接受的块尺寸列表通过
KVM_CAP_ARM_SUPPORTED_BLOCK_SIZES 以 64 位位图的形式暴露(每一位描述一种块尺寸)。
默认值为 0，表示禁用急切页拆分。)

7.40 KVM_CAP_EXIT_HYPERCALL(7.40 KVM_CAP_EXIT_HYPERCALL)
--------------------------------------------------------

:Architectures: x86
:Type: vm

(架构：x86；类型：vm。)

This capability, if enabled, will cause KVM to exit to userspace
with KVM_EXIT_HYPERCALL exit reason to process some hypercalls.
(启用此能力后，KVM 会以 KVM_EXIT_HYPERCALL 退出原因退出到用户态，以处理某些超级
调用。)

Calling KVM_CHECK_EXTENSION for this capability will return a bitmask
of hypercalls that can be configured to exit to userspace.
Right now, the only such hypercall is KVM_HC_MAP_GPA_RANGE.
(对此能力调用 KVM_CHECK_EXTENSION 会返回一个位掩码，表示可以配置为退出到用户态的
超级调用。目前，唯一的这类超级调用是 KVM_HC_MAP_GPA_RANGE。)

The argument to KVM_ENABLE_CAP is also a bitmask, and must be a subset
of the result of KVM_CHECK_EXTENSION.  KVM will forward to userspace
the hypercalls whose corresponding bit is in the argument, and return
ENOSYS for the others.
(传给 KVM_ENABLE_CAP 的参数同样是位掩码，并且必须是 KVM_CHECK_EXTENSION 结果的子
集。KVM 会把参数中对应位被置位的那些超级调用转发给用户态，对其余的返回 ENOSYS。)

7.41 KVM_CAP_ARM_SYSTEM_SUSPEND(7.41 KVM_CAP_ARM_SYSTEM_SUSPEND)
----------------------------------------------------------------

:Architectures: arm64
:Type: vm

(架构：arm64；类型：vm。)

When enabled, KVM will exit to userspace with KVM_EXIT_SYSTEM_EVENT of
type KVM_SYSTEM_EVENT_SUSPEND to process the guest suspend request.
(启用后，KVM 会以 KVM_SYSTEM_EVENT_SUSPEND 类型的 KVM_EXIT_SYSTEM_EVENT 退出到用
户态，以处理客户机的挂起请求。)

7.42 KVM_CAP_ARM_WRITABLE_IMP_ID_REGS(7.42 KVM_CAP_ARM_WRITABLE_IMP_ID_REGS)
----------------------------------------------------------------------------

:Architectures: arm64
:Target: VM
:Parameters: None
:Returns: 0 on success, -EINVAL if vCPUs have been created before enabling this
          capability.

(架构：arm64；目标：VM；参数：无；返回值：成功时为 0，在启用此能力之前已创建
vCPU 时为 -EINVAL。)

This capability changes the behavior of the registers that identify a PE
implementation of the Arm architecture: MIDR_EL1, REVIDR_EL1, and AIDR_EL1.
By default, these registers are visible to userspace but treated as invariant.
(此能力改变用于标识 Arm 架构 PE 实现的寄存器的行为：MIDR_EL1、REVIDR_EL1 与
AIDR_EL1。默认情况下，这些寄存器对用户态可见，但被视为不变 (invariant)。)

When this capability is enabled, KVM allows userspace to change the
aforementioned registers before the first KVM_RUN. These registers are VM
scoped, meaning that the same set of values are presented on all vCPUs in a
given VM.
(启用此能力后，KVM 允许用户态在第一次 KVM_RUN 之前修改上述寄存器。这些寄存器是
VM 范围的，意味着给定 VM 中的所有 vCPU 都呈现同一组值。)

7.43 KVM_CAP_RISCV_MP_STATE_RESET(7.43 KVM_CAP_RISCV_MP_STATE_RESET)
--------------------------------------------------------------------

:Architectures: riscv
:Type: VM
:Parameters: None
:Returns: 0 on success, -EINVAL if arg[0] is not zero

(架构：riscv；类型：VM；参数：无；返回值：成功时为 0，arg[0] 不为零时为
-EINVAL。)

When this capability is enabled, KVM resets the VCPU when setting
MP_STATE_INIT_RECEIVED through IOCTL.  The original MP_STATE is preserved.
(启用此能力后，当通过 IOCTL 设置 MP_STATE_INIT_RECEIVED 时，KVM 会复位该 VCPU。原
有的 MP_STATE 会被保留。)

7.44 KVM_CAP_ARM_CACHEABLE_PFNMAP_SUPPORTED(7.44 KVM_CAP_ARM_CACHEABLE_PFNMAP_SUPPORTED)
-----------------------------------------------------------------------------------------

:Architectures: arm64
:Target: VM
:Parameters: None

(架构：arm64；目标：VM；参数：无。)

This capability indicate to the userspace whether a PFNMAP memory region
can be safely mapped as cacheable. This relies on the presence of
force write back (FWB) feature support on the hardware.
(此能力告知用户态：某个 PFNMAP 内存区域是否可以安全地按可缓存 (cacheable) 方式映
射。这依赖硬件支持 force write back (FWB) 特性。)

7.45 KVM_CAP_ARM_SEA_TO_USER(7.45 KVM_CAP_ARM_SEA_TO_USER)
----------------------------------------------------------

:Architecture: arm64
:Target: VM
:Parameters: none
:Returns: 0 on success, -EINVAL if unsupported.

(架构：arm64；目标：VM；参数：无；返回值：成功时为 0，不支持时为 -EINVAL。)

When this capability is enabled, KVM may exit to userspace for SEAs taken to
EL2 resulting from a guest access. See ``KVM_EXIT_ARM_SEA`` for more
information.
(启用此能力后，对于由客户机访问引发的、进入 EL2 的 SEA，KVM 可能会退出到用户态。
更多信息参见 ``KVM_EXIT_ARM_SEA``。)

7.46 KVM_CAP_S390_USER_OPEREXEC(7.46 KVM_CAP_S390_USER_OPEREXEC)
----------------------------------------------------------------

:Architectures: s390
:Parameters: none

(架构：s390；参数：无。)

When this capability is enabled KVM forwards all operation exceptions
that it doesn't handle itself to user space. This also includes the
0x0000 instructions managed by KVM_CAP_S390_USER_INSTR0. This is
helpful if user space wants to emulate instructions which are not
(yet) implemented in hardware.
(启用此能力后，KVM 会把它自身不处理的所有操作异常 (operation exception) 转发给用
户空间。这也包括由 KVM_CAP_S390_USER_INSTR0 管理的 0x0000 指令。如果用户空间想模
拟尚未在硬件中实现的指令，这会很有帮助。)

This capability can be enabled dynamically even if VCPUs were already
created and are running.
(即使 VCPU 已经创建并正在运行，此能力也可以动态启用。)

8. Other capabilities.(8. 其他能力)
===================================

This section lists capabilities that give information about other
features of the KVM implementation.
(本节列出提供 KVM 实现其他特性相关信息的各能力。)

8.1 KVM_CAP_PPC_HWRNG(8.1 KVM_CAP_PPC_HWRNG)
--------------------------------------------

:Architectures: ppc

(架构：ppc。)

This capability, if KVM_CHECK_EXTENSION indicates that it is
available, means that the kernel has an implementation of the
H_RANDOM hypercall backed by a hardware random-number generator.
If present, the kernel H_RANDOM handler can be enabled for guest use
with the KVM_CAP_PPC_ENABLE_HCALL capability.
(此能力(如果 KVM_CHECK_EXTENSION 表明其可用)意味着内核实现了由硬件随机数生成器
支撑的 H_RANDOM 超级调用。如果存在，可以通过 KVM_CAP_PPC_ENABLE_HCALL 能力启用内
核的 H_RANDOM 处理程序供客户机使用。)

8.3 KVM_CAP_PPC_MMU_RADIX(8.3 KVM_CAP_PPC_MMU_RADIX)
----------------------------------------------------

:Architectures: ppc

(架构：ppc。)

This capability, if KVM_CHECK_EXTENSION indicates that it is
available, means that the kernel can support guests using the
radix MMU defined in Power ISA V3.00 (as implemented in the POWER9
processor).
(此能力(如果 KVM_CHECK_EXTENSION 表明其可用)意味着内核可以支持使用 Power ISA
V3.00 中定义的 radix MMU 的客户机(如在 POWER9 处理器中实现的那样)。)

8.4 KVM_CAP_PPC_MMU_HASH_V3(8.4 KVM_CAP_PPC_MMU_HASH_V3)
--------------------------------------------------------

:Architectures: ppc

(架构：ppc。)

This capability, if KVM_CHECK_EXTENSION indicates that it is
available, means that the kernel can support guests using the
hashed page table MMU defined in Power ISA V3.00 (as implemented in
the POWER9 processor), including in-memory segment tables.
(此能力(如果 KVM_CHECK_EXTENSION 表明其可用)意味着内核可以支持使用 Power ISA
V3.00 中定义的哈希页表 MMU 的客户机(如在 POWER9 处理器中实现的那样)，包括内存中的
段表。)

8.5 KVM_CAP_MIPS_VZ(8.5 KVM_CAP_MIPS_VZ)
----------------------------------------

:Architectures: mips

(架构：mips。)

This capability, if KVM_CHECK_EXTENSION on the main kvm handle indicates that
it is available, means that full hardware assisted virtualization capabilities
of the hardware are available for use through KVM. An appropriate
KVM_VM_MIPS_* type must be passed to KVM_CREATE_VM to create a VM which
utilises it.
(此能力(如果在主 kvm 句柄上调用 KVM_CHECK_EXTENSION 表明其可用)意味着硬件的完整
硬件辅助虚拟化能力可以通过 KVM 使用。必须把适当的 KVM_VM_MIPS_* 类型传给
KVM_CREATE_VM，以创建使用它的 VM。)

If KVM_CHECK_EXTENSION on a kvm VM handle indicates that this capability is
available, it means that the VM is using full hardware assisted virtualization
capabilities of the hardware. This is useful to check after creating a VM with
KVM_VM_MIPS_DEFAULT.
(如果在 kvm VM 句柄上调用 KVM_CHECK_EXTENSION 表明此能力可用，则表示该 VM 正在使
用硬件的完整硬件辅助虚拟化能力。在用 KVM_VM_MIPS_DEFAULT 创建 VM 之后进行检查很有
用。)

The value returned by KVM_CHECK_EXTENSION should be compared against known
values (see below). All other values are reserved. This is to allow for the
possibility of other hardware assisted virtualization implementations which
may be incompatible with the MIPS VZ ASE.
(KVM_CHECK_EXTENSION 返回的值应当与已知取值(见下)进行比较。所有其他取值都保留。
这是为了兼容可能与 MIPS VZ ASE 不兼容的其他硬件辅助虚拟化实现。)

==  ==========================================================================
 0  The trap & emulate implementation is in use to run guest code in user
    mode. Guest virtual memory segments are rearranged to fit the guest in the
    user mode address space.
    (正在使用 trap & emulate 实现，在用户态运行客户机代码。客户机虚拟内存段被重
    新排列，以把客户机放入用户态地址空间。)

 1  The MIPS VZ ASE is in use, providing full hardware assisted
    virtualization, including standard guest virtual memory segments.
    (正在使用 MIPS VZ ASE，提供完整的硬件辅助虚拟化，包括标准的客户机虚拟内存
    段。)
==  ==========================================================================


8.7 KVM_CAP_MIPS_64BIT(8.7 KVM_CAP_MIPS_64BIT)
----------------------------------------------

:Architectures: mips

(架构：mips。)

This capability indicates the supported architecture type of the guest, i.e. the
supported register and address width.
(此能力表明客户机支持的架构类型，即所支持的寄存器与地址宽度。)

The values returned when this capability is checked by KVM_CHECK_EXTENSION on a
kvm VM handle correspond roughly to the CP0_Config.AT register field, and should
be checked specifically against known values (see below). All other values are
reserved.
(在 kvm VM 句柄上通过 KVM_CHECK_EXTENSION 检查此能力时返回的值，大致对应
CP0_Config.AT 寄存器字段，应当具体地与已知取值(见下)进行核对。所有其他取值都保
留。)

==  ========================================================================
 0  MIPS32 or microMIPS32.
    Both registers and addresses are 32-bits wide.
    It will only be possible to run 32-bit guest code.
    (MIPS32 或 microMIPS32。寄存器与地址都是 32 位宽。只能运行 32 位客户机代
    码。)

 1  MIPS64 or microMIPS64 with access only to 32-bit compatibility segments.
    Registers are 64-bits wide, but addresses are 32-bits wide.
    64-bit guest code may run but cannot access MIPS64 memory segments.
    It will also be possible to run 32-bit guest code.
    (MIPS64 或 microMIPS64，但只能访问 32 位兼容段。寄存器是 64 位宽，而地址是
    32 位宽。可以运行 64 位客户机代码，但不能访问 MIPS64 内存段。也可以运行 32
    位客户机代码。)

 2  MIPS64 or microMIPS64 with access to all address segments.
    Both registers and addresses are 64-bits wide.
    It will be possible to run 64-bit or 32-bit guest code.
    (MIPS64 或 microMIPS64，可访问所有地址段。寄存器与地址都是 64 位宽。可以运
    行 64 位或 32 位客户机代码。)
==  ========================================================================

8.9 KVM_CAP_ARM_USER_IRQ(8.9 KVM_CAP_ARM_USER_IRQ)
--------------------------------------------------

:Architectures: arm64

(架构：arm64。)

This capability, if KVM_CHECK_EXTENSION indicates that it is available, means
that if userspace creates a VM without an in-kernel interrupt controller, it
will be notified of changes to the output level of in-kernel emulated devices,
which can generate virtual interrupts, presented to the VM.
For such VMs, on every return to userspace, the kernel
updates the vcpu's run->s.regs.device_irq_level field to represent the actual
output level of the device.
(此能力(如果 KVM_CHECK_EXTENSION 表明其可用)意味着：如果用户态创建了一个没有内
核内中断控制器的 VM，那么当可以向 VM 提供虚拟中断的内核内模拟设备的输出电平发生
变化时，用户态会收到通知。对于这类 VM，每次返回用户态时，内核都会更新 vcpu 的
run->s.regs.device_irq_level 字段，以表示设备的实际输出电平。)

Whenever kvm detects a change in the device output level, kvm guarantees at
least one return to userspace before running the VM.  This exit could either
be a KVM_EXIT_INTR or any other exit event, like KVM_EXIT_MMIO. This way,
userspace can always sample the device output level and re-compute the state of
the userspace interrupt controller.  Userspace should always check the state
of run->s.regs.device_irq_level on every kvm exit.
The value in run->s.regs.device_irq_level can represent both level and edge
triggered interrupt signals, depending on the device.  Edge triggered interrupt
signals will exit to userspace with the bit in run->s.regs.device_irq_level
set exactly once per edge signal.
(每当 kvm 检测到设备输出电平发生变化时，kvm 都保证在运行 VM 之前至少返回一次用
户态。该退出可能是 KVM_EXIT_INTR，也可能是任何其他退出事件，例如 KVM_EXIT_MMIO。
这样，用户态总能采样设备输出电平并重新计算用户态中断控制器的状态。用户态应当在
每次 kvm 退出时都检查 run->s.regs.device_irq_level 的状态。run->
s.regs.device_irq_level 中的值既可以表示电平触发的中断信号，也可以表示边沿触发的
中断信号，取决于设备。对于边沿触发的中断信号，每个边沿信号都会恰好一次以
run->s.regs.device_irq_level 中置位的位退出到用户态。)

The field run->s.regs.device_irq_level is available independent of
run->kvm_valid_regs or run->kvm_dirty_regs bits.
(run->s.regs.device_irq_level 字段的可用性与 run->kvm_valid_regs 或
run->kvm_dirty_regs 位无关。)

If KVM_CAP_ARM_USER_IRQ is supported, the KVM_CHECK_EXTENSION ioctl returns a
number larger than 0 indicating the version of this capability is implemented
and thereby which bits in run->s.regs.device_irq_level can signal values.
(如果支持 KVM_CAP_ARM_USER_IRQ，KVM_CHECK_EXTENSION ioctl 会返回一个大于 0 的数
字，表示实现了该能力的哪个版本，从而表明 run->s.regs.device_irq_level 中哪些位可
以用来传递值。)

Currently the following bits are defined for the device_irq_level bitmap::

  KVM_CAP_ARM_USER_IRQ >= 1:

    KVM_ARM_DEV_EL1_VTIMER -  EL1 virtual timer
    KVM_ARM_DEV_EL1_PTIMER -  EL1 physical timer
    KVM_ARM_DEV_PMU        -  ARM PMU overflow interrupt signal

(目前 device_irq_level 位图中定义了以下各位：)

Future versions of kvm may implement additional events. These will get
indicated by returning a higher number from KVM_CHECK_EXTENSION and will be
listed above.
(kvm 的未来版本可能会实现更多事件。它们将通过 KVM_CHECK_EXTENSION 返回更大的数
字来指示，并被列在上文。)

8.10 KVM_CAP_PPC_SMT_POSSIBLE(8.10 KVM_CAP_PPC_SMT_POSSIBLE)
------------------------------------------------------------

:Architectures: ppc

(架构：ppc。)

Querying this capability returns a bitmap indicating the possible
virtual SMT modes that can be set using KVM_CAP_PPC_SMT.  If bit N
(counting from the right) is set, then a virtual SMT mode of 2^N is
available.
(查询此能力会返回一个位图，指示可以使用 KVM_CAP_PPC_SMT 设置的可能的虚拟 SMT 模
式。如果位 N(从右往左数)被置位，则虚拟 SMT 模式 2^N 可用。)

8.12 KVM_CAP_HYPERV_VP_INDEX(8.12 KVM_CAP_HYPERV_VP_INDEX)
----------------------------------------------------------

:Architectures: x86

(架构：x86。)

This capability indicates that userspace can load HV_X64_MSR_VP_INDEX msr.  Its
value is used to denote the target vcpu for a SynIC interrupt.  For
compatibility, KVM initializes this msr to KVM's internal vcpu index.  When this
capability is absent, userspace can still query this msr's value.
(此能力表明用户态可以装载 HV_X64_MSR_VP_INDEX MSR。其值用于表示 SynIC 中断的目标
vcpu。为了兼容性，KVM 会把该 MSR 初始化为 KVM 的内部 vcpu 索引。即使没有此能力，
用户态仍可以查询该 MSR 的值。)

8.13 KVM_CAP_S390_AIS_MIGRATION(8.13 KVM_CAP_S390_AIS_MIGRATION)
----------------------------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates if the flic device will be able to get/set the
AIS states for migration via the KVM_DEV_FLIC_AISM_ALL attribute and allows
to discover this without having to create a flic device.
(此能力表明 flic 设备是否能够通过 KVM_DEV_FLIC_AISM_ALL 属性为迁移获取/设置 AIS
状态，并使得无需创建 flic 设备就能探知这一点。)

8.14 KVM_CAP_S390_PSW(8.14 KVM_CAP_S390_PSW)
--------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates that the PSW is exposed via the kvm_run structure.
(此能力表明 PSW 通过 kvm_run 结构暴露。)

8.15 KVM_CAP_S390_GMAP(8.15 KVM_CAP_S390_GMAP)
----------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates that the user space memory used as guest mapping can
be anywhere in the user memory address space, as long as the memory slots are
aligned and sized to a segment (1MB) boundary.
(此能力表明用作客户机映射的用户空间内存可以位于用户内存地址空间的任何位置，只要
内存槽按段 (1MB) 边界对齐并按段大小设置即可。)

8.16 KVM_CAP_S390_COW(8.16 KVM_CAP_S390_COW)
--------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates that the user space memory used as guest mapping can
use copy-on-write semantics as well as dirty pages tracking via read-only page
tables.
(此能力表明用作客户机映射的用户空间内存可以使用写时复制 (copy-on-write) 语义，以
及经由只读页表进行的脏页跟踪。)

8.17 KVM_CAP_S390_BPB(8.17 KVM_CAP_S390_BPB)
--------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates that kvm will implement the interfaces to handle
reset, migration and nested KVM for branch prediction blocking. The stfle
facility 82 should not be provided to the guest without this capability.
(此能力表明 kvm 将实现用于处理分支预测阻塞 (branch prediction blocking) 的复位、
迁移与嵌套 KVM 接口。没有此能力时，不应向客户机提供 stfle facility 82。)

8.18 KVM_CAP_HYPERV_TLBFLUSH(8.18 KVM_CAP_HYPERV_TLBFLUSH)
----------------------------------------------------------

:Architectures: x86

(架构：x86。)

This capability indicates that KVM supports paravirtualized Hyper-V TLB Flush
hypercalls:
HvFlushVirtualAddressSpace, HvFlushVirtualAddressSpaceEx,
HvFlushVirtualAddressList, HvFlushVirtualAddressListEx.
(此能力表明 KVM 支持半虚拟化的 Hyper-V TLB flush 超级调用：HvFlushVirtualAddress
Space、HvFlushVirtualAddressSpaceEx、HvFlushVirtualAddressList、
HvFlushVirtualAddressListEx。)

8.19 KVM_CAP_ARM_INJECT_SERROR_ESR(8.19 KVM_CAP_ARM_INJECT_SERROR_ESR)
----------------------------------------------------------------------

:Architectures: arm64

(架构：arm64。)

This capability indicates that userspace can specify (via the
KVM_SET_VCPU_EVENTS ioctl) the syndrome value reported to the guest when it
takes a virtual SError interrupt exception.
If KVM advertises this capability, userspace can only specify the ISS field for
the ESR syndrome. Other parts of the ESR, such as the EC are generated by the
CPU when the exception is taken. If this virtual SError is taken to EL1 using
AArch64, this value will be reported in the ISS field of ESR_ELx.
(此能力表明用户态可以通过 KVM_SET_VCPU_EVENTS ioctl，指定客户机发生虚拟 SError
中断异常时向其报告的 syndrome 值。如果 KVM 通告了此能力，用户态只能为 ESR syndrome
指定 ISS 字段。ESR 的其他部分(例如 EC)由 CPU 在发生异常时生成。如果该虚拟 SError
以 AArch64 方式进入 EL1，该值将报告在 ESR_ELx 的 ISS 字段中。)

See KVM_CAP_VCPU_EVENTS for more details.
(更多细节参见 KVM_CAP_VCPU_EVENTS。)

8.20 KVM_CAP_HYPERV_SEND_IPI(8.20 KVM_CAP_HYPERV_SEND_IPI)
----------------------------------------------------------

:Architectures: x86

(架构：x86。)

This capability indicates that KVM supports paravirtualized Hyper-V IPI send
hypercalls:
HvCallSendSyntheticClusterIpi, HvCallSendSyntheticClusterIpiEx.
(此能力表明 KVM 支持半虚拟化的 Hyper-V IPI 发送超级调用：HvCallSendSyntheticClus
terIpi、HvCallSendSyntheticClusterIpiEx。)

8.22 KVM_CAP_S390_VCPU_RESETS(8.22 KVM_CAP_S390_VCPU_RESETS)
------------------------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates that the KVM_S390_NORMAL_RESET and
KVM_S390_CLEAR_RESET ioctls are available.
(此能力表明 KVM_S390_NORMAL_RESET 与 KVM_S390_CLEAR_RESET ioctl 可用。)

8.23 KVM_CAP_S390_PROTECTED(8.23 KVM_CAP_S390_PROTECTED)
--------------------------------------------------------

:Architectures: s390

(架构：s390。)

This capability indicates that the Ultravisor has been initialized and
KVM can therefore start protected VMs.
This capability governs the KVM_S390_PV_COMMAND ioctl and the
KVM_MP_STATE_LOAD MP_STATE. KVM_SET_MP_STATE can fail for protected
guests when the state change is invalid.
(此能力表明 Ultravisor 已经初始化，因此 KVM 可以启动受保护的 VM。此能力管辖
KVM_S390_PV_COMMAND ioctl 与 KVM_MP_STATE_LOAD MP_STATE。对于受保护的客户机，当状
态变更无效时，KVM_SET_MP_STATE 可能失败。)

8.24 KVM_CAP_STEAL_TIME(8.24 KVM_CAP_STEAL_TIME)
------------------------------------------------

:Architectures: arm64, x86

(架构：arm64, x86。)

This capability indicates that KVM supports steal time accounting.
When steal time accounting is supported it may be enabled with
architecture-specific interfaces.  This capability and the architecture-
specific interfaces must be consistent, i.e. if one says the feature
is supported, than the other should as well and vice versa.  For arm64
see Documentation/virt/kvm/devices/vcpu.rst "KVM_ARM_VCPU_PVTIME_CTRL".
For x86 see Documentation/virt/kvm/x86/msr.rst "MSR_KVM_STEAL_TIME".
(此能力表明 KVM 支持 steal time 记账。在支持 steal time 记账时，可以通过架构特定
的接口启用它。此能力与架构特定的接口必须保持一致，即如果一方说支持该特性，另一
方也应如此，反之亦然。arm64 参见 Documentation/virt/kvm/devices/vcpu.rst 中的
"KVM_ARM_VCPU_PVTIME_CTRL"；x86 参见 Documentation/virt/kvm/x86/msr.rst 中的
"MSR_KVM_STEAL_TIME"。)

8.25 KVM_CAP_S390_DIAG318(8.25 KVM_CAP_S390_DIAG318)
----------------------------------------------------

:Architectures: s390

(架构：s390。)

This capability enables a guest to set information about its control program
(i.e. guest kernel type and version). The information is helpful during
system/firmware service events, providing additional data about the guest
environments running on the machine.
(此能力使客户机能够设置关于其控制程序 (control program) 的信息(即客户机内核类型
与版本)。这些信息在系统/固件服务事件期间很有用，可以提供关于机器上运行的客户机
环境的额外数据。)

The information is associated with the DIAGNOSE 0x318 instruction, which sets
an 8-byte value consisting of a one-byte Control Program Name Code (CPNC) and
a 7-byte Control Program Version Code (CPVC). The CPNC determines what
environment the control program is running in (e.g. Linux, z/VM...), and the
CPVC is used for information specific to OS (e.g. Linux version, Linux
distribution...)
(该信息与 DIAGNOSE 0x318 指令相关联，该指令设置一个 8 字节的值，由一个字节的控制
程序名称代码 (CPNC) 与 7 个字节的控制程序版本代码 (CPVC) 组成。CPNC 决定控制程序
运行在什么环境中(例如 Linux、z/VM 等)，CPVC 用于 OS 特定的信息(例如 Linux 版
本、Linux 发行版等)。)

If this capability is available, then the CPNC and CPVC can be synchronized
between KVM and userspace via the sync regs mechanism (KVM_SYNC_DIAG318).
(如果此能力可用，则 CPNC 与 CPVC 可以通过 sync regs 机制 (KVM_SYNC_DIAG318) 在
KVM 与用户态之间同步。)

8.26 KVM_CAP_X86_USER_SPACE_MSR(8.26 KVM_CAP_X86_USER_SPACE_MSR)
----------------------------------------------------------------

:Architectures: x86

(架构：x86。)

This capability indicates that KVM supports deflection of MSR reads and
writes to user space. It can be enabled on a VM level. If enabled, MSR
accesses that would usually trigger a #GP by KVM into the guest will
instead get bounced to user space through the KVM_EXIT_X86_RDMSR and
KVM_EXIT_X86_WRMSR exit notifications.
(此能力表明 KVM 支持把 MSR 读取与写入转向 (deflect) 到用户空间。它可以在 VM 级
别启用。启用后，通常会由 KVM 在客户机内触发 #GP 的 MSR 访问，将改为通过
KVM_EXIT_X86_RDMSR 与 KVM_EXIT_X86_WRMSR 退出通知转向用户空间。)

8.27 KVM_CAP_X86_MSR_FILTER(8.27 KVM_CAP_X86_MSR_FILTER)
--------------------------------------------------------

:Architectures: x86

(架构：x86。)

This capability indicates that KVM supports that accesses to user defined MSRs
may be rejected. With this capability exposed, KVM exports new VM ioctl
KVM_X86_SET_MSR_FILTER which user space can call to specify bitmaps of MSR
ranges that KVM should deny access to.
(此能力表明 KVM 支持拒绝对用户自定义 MSR 的访问。在暴露此能力后，KVM 导出新的
VM ioctl KVM_X86_SET_MSR_FILTER，用户空间可以调用它来指定 KVM 应拒绝访问的 MSR 范
围位图。)

In combination with KVM_CAP_X86_USER_SPACE_MSR, this allows user space to
trap and emulate MSRs that are outside of the scope of KVM as well as
limit the attack surface on KVM's MSR emulation code.
(与 KVM_CAP_X86_USER_SPACE_MSR 相结合，这允许用户空间陷阱并模拟 KVM 范围之外的
MSR，同时限制 KVM MSR 模拟代码的攻击面。)

8.30 KVM_CAP_XEN_HVM(8.30 KVM_CAP_XEN_HVM)
------------------------------------------

:Architectures: x86

(架构：x86。)

This capability indicates the features that Xen supports for hosting Xen
PVHVM guests. Valid flags are::

  #define KVM_XEN_HVM_CONFIG_HYPERCALL_MSR		(1 << 0)
  #define KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL		(1 << 1)
  #define KVM_XEN_HVM_CONFIG_SHARED_INFO		(1 << 2)
  #define KVM_XEN_HVM_CONFIG_RUNSTATE			(1 << 3)
  #define KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL		(1 << 4)
  #define KVM_XEN_HVM_CONFIG_EVTCHN_SEND		(1 << 5)
  #define KVM_XEN_HVM_CONFIG_RUNSTATE_UPDATE_FLAG	(1 << 6)
  #define KVM_XEN_HVM_CONFIG_PVCLOCK_TSC_UNSTABLE	(1 << 7)

(此能力表明 Xen 为承载 Xen PVHVM 客户机所支持的特性。有效标志如下：)

The KVM_XEN_HVM_CONFIG_HYPERCALL_MSR flag indicates that the KVM_XEN_HVM_CONFIG
ioctl is available, for the guest to set its hypercall page.
(KVM_XEN_HVM_CONFIG_HYPERCALL_MSR 标志表明 KVM_XEN_HVM_CONFIG ioctl 可用，客户机
可以用它设置自己的超级调用页。)

If KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL is also set, the same flag may also be
provided in the flags to KVM_XEN_HVM_CONFIG, without providing hypercall page
contents, to request that KVM generate hypercall page content automatically
and also enable interception of guest hypercalls with KVM_EXIT_XEN.
(如果还设置了 KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL，则也可以在传给 KVM_XEN_HVM_CONFIG
的 flags 中提供同一标志而不提供超级调用页内容，以请求 KVM 自动生成超级调用页内容，
并同时启用对客户机超级调用的拦截(以 KVM_EXIT_XEN 退出)。)

The KVM_XEN_HVM_CONFIG_SHARED_INFO flag indicates the availability of the
KVM_XEN_HVM_SET_ATTR, KVM_XEN_HVM_GET_ATTR, KVM_XEN_VCPU_SET_ATTR and
KVM_XEN_VCPU_GET_ATTR ioctls, as well as the delivery of exception vectors
for event channel upcalls when the evtchn_upcall_pending field of a vcpu's
vcpu_info is set.
(KVM_XEN_HVM_CONFIG_SHARED_INFO 标志表明 KVM_XEN_HVM_SET_ATTR、
KVM_XEN_HVM_GET_ATTR、KVM_XEN_VCPU_SET_ATTR 与 KVM_XEN_VCPU_GET_ATTR ioctl 可用，
并且当 vcpu 的 vcpu_info 的 evtchn_upcall_pending 字段被置位时，会为事件通道
upcall 投递异常向量。)

The KVM_XEN_HVM_CONFIG_RUNSTATE flag indicates that the runstate-related
features KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_ADDR/_CURRENT/_DATA/_ADJUST are
supported by the KVM_XEN_VCPU_SET_ATTR/KVM_XEN_VCPU_GET_ATTR ioctls.
(KVM_XEN_HVM_CONFIG_RUNSTATE 标志表明运行状态 (runstate) 相关特性
KVM_XEN_VCPU_ATTR_TYPE_RUNSTATE_ADDR/_CURRENT/_DATA/_ADJUST 受
KVM_XEN_VCPU_SET_ATTR/KVM_XEN_VCPU_GET_ATTR ioctl 支持。)

The KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL flag indicates that IRQ routing entries
of the type KVM_IRQ_ROUTING_XEN_EVTCHN are supported, with the priority
field set to indicate 2 level event channel delivery.
(KVM_XEN_HVM_CONFIG_EVTCHN_2LEVEL 标志表明支持 KVM_IRQ_ROUTING_XEN_EVTCHN 类型的
IRQ 路由表项，其中优先级字段被设置为表示 2 级事件通道投递。)

The KVM_XEN_HVM_CONFIG_EVTCHN_SEND flag indicates that KVM supports
injecting event channel events directly into the guest with the
KVM_XEN_HVM_EVTCHN_SEND ioctl. It also indicates support for the
KVM_XEN_ATTR_TYPE_EVTCHN/XEN_VERSION HVM attributes and the
KVM_XEN_VCPU_ATTR_TYPE_VCPU_ID/TIMER/UPCALL_VECTOR vCPU attributes.
related to event channel delivery, timers, and the XENVER_version
interception.
(KVM_XEN_HVM_CONFIG_EVTCHN_SEND 标志表明 KVM 支持使用 KVM_XEN_HVM_EVTCHN_SEND
ioctl 直接向客户机注入事件通道事件。它还表明支持与事件通道投递、定时器以及
XENVER_version 拦截相关的 KVM_XEN_ATTR_TYPE_EVTCHN/XEN_VERSION HVM 属性与
KVM_XEN_VCPU_ATTR_TYPE_VCPU_ID/TIMER/UPCALL_VECTOR vCPU 属性。)

The KVM_XEN_HVM_CONFIG_RUNSTATE_UPDATE_FLAG flag indicates that KVM supports
the KVM_XEN_ATTR_TYPE_RUNSTATE_UPDATE_FLAG attribute in the KVM_XEN_SET_ATTR
and KVM_XEN_GET_ATTR ioctls. This controls whether KVM will set the
XEN_RUNSTATE_UPDATE flag in guest memory mapped vcpu_runstate_info during
updates of the runstate information. Note that versions of KVM which support
the RUNSTATE feature above, but not the RUNSTATE_UPDATE_FLAG feature, will
always set the XEN_RUNSTATE_UPDATE flag when updating the guest structure,
which is perhaps counterintuitive. When this flag is advertised, KVM will
behave more correctly, not using the XEN_RUNSTATE_UPDATE flag until/unless
specifically enabled (by the guest making the hypercall, causing the VMM
to enable the KVM_XEN_ATTR_TYPE_RUNSTATE_UPDATE_FLAG attribute).
(KVM_XEN_HVM_CONFIG_RUNSTATE_UPDATE_FLAG 标志表明 KVM 在 KVM_XEN_SET_ATTR 与
KVM_XEN_GET_ATTR ioctl 中支持 KVM_XEN_ATTR_TYPE_RUNSTATE_UPDATE_FLAG 属性。它控
制 KVM 在更新运行状态信息时，是否会在映射到客户机内存的 vcpu_runstate_info 中设
置 XEN_RUNSTATE_UPDATE 标志。注意，支持上述 RUNSTATE 特性但不支持
RUNSTATE_UPDATE_FLAG 特性的 KVM 版本，在更新客户机结构时总会设置
XEN_RUNSTATE_UPDATE 标志，这可能有些反直觉。当通告此标志时，KVM 的行为会更正
确：除非被明确启用(由客户机发出超级调用，使 VMM 启用
KVM_XEN_ATTR_TYPE_RUNSTATE_UPDATE_FLAG 属性)，否则不会使用 XEN_RUNSTATE_UPDATE
标志。)

The KVM_XEN_HVM_CONFIG_PVCLOCK_TSC_UNSTABLE flag indicates that KVM supports
clearing the PVCLOCK_TSC_STABLE_BIT flag in Xen pvclock sources. This will be
done when the KVM_CAP_XEN_HVM ioctl sets the
KVM_XEN_HVM_CONFIG_PVCLOCK_TSC_UNSTABLE flag.
(KVM_XEN_HVM_CONFIG_PVCLOCK_TSC_UNSTABLE 标志表明 KVM 支持清除 Xen pvclock 时钟源
中的 PVCLOCK_TSC_STABLE_BIT 标志。当 KVM_CAP_XEN_HVM ioctl 设置
KVM_XEN_HVM_CONFIG_PVCLOCK_TSC_UNSTABLE 标志时会这样做。)

8.31 KVM_CAP_SPAPR_MULTITCE(8.31 KVM_CAP_SPAPR_MULTITCE)
--------------------------------------------------------

:Architectures: ppc
:Type: vm

(架构：ppc；类型：vm。)

This capability means the kernel is capable of handling hypercalls
H_PUT_TCE_INDIRECT and H_STUFF_TCE without passing those into the user
space. This significantly accelerates DMA operations for PPC KVM guests.
User space should expect that its handlers for these hypercalls
are not going to be called if user space previously registered LIOBN
in KVM (via KVM_CREATE_SPAPR_TCE or similar calls).
(此能力意味着内核能够处理超级调用 H_PUT_TCE_INDIRECT 与 H_STUFF_TCE 而无需把它们
传递给用户空间。这会显著加速 PPC KVM 客户机的 DMA 操作。如果用户空间之前已在 KVM
中注册了 LIOBN(通过 KVM_CREATE_SPAPR_TCE 或类似调用)，用户空间应当预期它为这些超
级调用编写的处理程序不会被调用。)

In order to enable H_PUT_TCE_INDIRECT and H_STUFF_TCE use in the guest,
user space might have to advertise it for the guest. For example,
IBM pSeries (sPAPR) guest starts using them if "hcall-multi-tce" is
present in the "ibm,hypertas-functions" device-tree property.
(为了在客户机中启用 H_PUT_TCE_INDIRECT 与 H_STUFF_TCE 的使用，用户空间可能必须向
客户机通告它们。例如，IBM pSeries (sPAPR) 客户机在 "ibm,hypertas-functions" 设备
树属性中存在 "hcall-multi-tce" 时就开始使用它们。)

The hypercalls mentioned above may or may not be processed successfully
in the kernel based fast path. If they can not be handled by the kernel,
they will get passed on to user space. So user space still has to have
an implementation for these despite the in kernel acceleration.
(上述超级调用可能在基于内核的快速路径中处理成功，也可能不成功。如果内核无法处
理，它们会被传递给用户空间。因此，尽管有内核内加速，用户空间仍必须为它们提供实
现。)

This capability is always enabled.
(此能力始终启用。)

8.32 KVM_CAP_PTP_KVM(8.32 KVM_CAP_PTP_KVM)
------------------------------------------

:Architectures: arm64

(架构：arm64。)

This capability indicates that the KVM virtual PTP service is
supported in the host. A VMM can check whether the service is
available to the guest on migration.
(此能力表明宿主机支持 KVM 虚拟 PTP 服务。VMM 可以据此检查迁移之后该服务对客户机
是否可用。)

8.37 KVM_CAP_S390_PROTECTED_DUMP(8.37 KVM_CAP_S390_PROTECTED_DUMP)
------------------------------------------------------------------

:Architectures: s390
:Type: vm

(架构：s390；类型：vm。)

This capability indicates that KVM and the Ultravisor support dumping
PV guests. The `KVM_PV_DUMP` command is available for the
`KVM_S390_PV_COMMAND` ioctl and the `KVM_PV_INFO` command provides
dump related UV data. Also the vcpu ioctl `KVM_S390_PV_CPU_COMMAND` is
available and supports the `KVM_PV_DUMP_CPU` subcommand.
(此能力表明 KVM 与 Ultravisor 支持转储 PV 客户机。KVM_PV_DUMP 命令可用于
KVM_S390_PV_COMMAND ioctl，KVM_PV_INFO 命令提供与转储相关的 UV 数据。此外，vcpu
ioctl KVM_S390_PV_CPU_COMMAND 也可用，并支持 KVM_PV_DUMP_CPU 子命令。)

8.39 KVM_CAP_S390_CPU_TOPOLOGY(8.39 KVM_CAP_S390_CPU_TOPOLOGY)
--------------------------------------------------------------

:Architectures: s390
:Type: vm

(架构：s390；类型：vm。)

This capability indicates that KVM will provide the S390 CPU Topology
facility which consist of the interpretation of the PTF instruction for
the function code 2 along with interception and forwarding of both the
PTF instruction with function codes 0 or 1 and the STSI(15,1,x)
instruction to the userland hypervisor.
(此能力表明 KVM 将提供 S390 CPU 拓扑 (CPU Topology) 设施，它包括对功能代码为 2
的 PTF 指令的解释，以及对功能代码为 0 或 1 的 PTF 指令与 STSI(15,1,x) 指令的拦
截，并把它们转发给用户态 hypervisor。)

The stfle facility 11, CPU Topology facility, should not be indicated
to the guest without this capability.
(没有此能力时，不应向客户机指示 stfle facility 11(CPU Topology 设施)。)

When this capability is present, KVM provides a new attribute group
on vm fd, KVM_S390_VM_CPU_TOPOLOGY.
This new attribute allows to get, set or clear the Modified Change
Topology Report (MTCR) bit of the SCA through the kvm_device_attr
structure.
(此能力存在时，KVM 会在 vm fd 上提供一个新的属性组 KVM_S390_VM_CPU_TOPOLOGY。该
新属性允许通过 kvm_device_attr 结构获取、设置或清除 SCA 的修改拓扑报告 (Modified
Change Topology Report, MTCR) 位。)

When getting the Modified Change Topology Report value, the attr->addr
must point to a byte where the value will be stored or retrieved from.
(获取 Modified Change Topology Report 值时，attr->addr 必须指向一个字节，值将
被存储到该字节或从该字节取出。)

8.41 KVM_CAP_VM_TYPES(8.41 KVM_CAP_VM_TYPES)
--------------------------------------------

:Architectures: x86
:Type: system ioctl

(架构：x86；类型：system ioctl。)

This capability returns a bitmap of support VM types.  The 1-setting of bit @n
means the VM type with value @n is supported.  Possible values of @n are::

  #define KVM_X86_DEFAULT_VM	0
  #define KVM_X86_SW_PROTECTED_VM	1
  #define KVM_X86_SEV_VM	2
  #define KVM_X86_SEV_ES_VM	3

(此能力返回受支持 VM 类型的位图。位 @n 置 1 表示取值为 @n 的 VM 类型受支持。@n
的可能取值如下：)

Note, KVM_X86_SW_PROTECTED_VM is currently only for development and testing.
Do not use KVM_X86_SW_PROTECTED_VM for "real" VMs, and especially not in
production.  The behavior and effective ABI for software-protected VMs is
unstable.
(注意，KVM_X86_SW_PROTECTED_VM 目前仅用于开发与测试。不要把
KVM_X86_SW_PROTECTED_VM 用于 "真实" 的 VM，尤其不要用于生产环境。软件保护 VM 的
行为与实际 ABI 是不稳定的。)

8.42 KVM_CAP_PPC_RPT_INVALIDATE(8.42 KVM_CAP_PPC_RPT_INVALIDATE)
----------------------------------------------------------------

:Architectures: ppc

(架构：ppc。)

This capability indicates that the kernel is capable of handling
H_RPT_INVALIDATE hcall.
(此能力表明内核能够处理 H_RPT_INVALIDATE 超级调用。)

In order to enable the use of H_RPT_INVALIDATE in the guest,
user space might have to advertise it for the guest. For example,
IBM pSeries (sPAPR) guest starts using it if "hcall-rpt-invalidate" is
present in the "ibm,hypertas-functions" device-tree property.
(为了在客户机中启用 H_RPT_INVALIDATE 的使用，用户空间可能必须向客户机通告它。
例如，IBM pSeries (sPAPR) 客户机在 "ibm,hypertas-functions" 设备树属性中存在
"hcall-rpt-invalidate" 时就开始使用它。)

This capability is enabled for hypervisors on platforms like POWER9
that support radix MMU.
(在支持 radix MMU 的平台(例如 POWER9)上，hypervisor 会启用此能力。)

8.43 KVM_CAP_PPC_AIL_MODE_3(8.43 KVM_CAP_PPC_AIL_MODE_3)
--------------------------------------------------------

:Architectures: ppc

(架构：ppc。)

This capability indicates that the kernel supports the mode 3 setting for the
"Address Translation Mode on Interrupt" aka "Alternate Interrupt Location"
resource that is controlled with the H_SET_MODE hypercall.
(此能力表明内核支持通过 H_SET_MODE 超级调用控制的 "Address Translation Mode on
Interrupt"(中断时的地址翻译模式)，又称 "Alternate Interrupt Location" 资源的模式
3 设置。)

This capability allows a guest kernel to use a better-performance mode for
handling interrupts and system calls.
(此能力允许客户机内核使用性能更好的模式来处理中断与系统调用。)

8.44 KVM_CAP_MEMORY_FAULT_INFO(8.44 KVM_CAP_MEMORY_FAULT_INFO)
--------------------------------------------------------------

:Architectures: x86

(架构：x86。)

The presence of this capability indicates that KVM_RUN will fill
kvm_run.memory_fault if KVM cannot resolve a guest page fault VM-Exit, e.g. if
there is a valid memslot but no backing VMA for the corresponding host virtual
address.
(此能力的存在表明：当 KVM 无法解决一次客户机缺页 VM-Exit 时(例如存在有效 memslot
但对应的宿主机虚拟地址没有后备 VMA)，KVM_RUN 会填充 kvm_run.memory_fault。)

The information in kvm_run.memory_fault is valid if and only if KVM_RUN returns
an error with errno=EFAULT or errno=EHWPOISON *and* kvm_run.exit_reason is set
to KVM_EXIT_MEMORY_FAULT.
(当且仅当 KVM_RUN 以 errno=EFAULT 或 errno=EHWPOISON 返回错误*且*
kvm_run.exit_reason 被设置为 KVM_EXIT_MEMORY_FAULT 时，kvm_run.memory_fault 中的
信息才是有效的。)

Note: Userspaces which attempt to resolve memory faults so that they can retry
KVM_RUN are encouraged to guard against repeatedly receiving the same
error/annotated fault.
(注意：对于尝试解决内存故障以便重试 KVM_RUN 的用户态，建议防范反复收到相同错误
/带注解故障的情况。)

See KVM_EXIT_MEMORY_FAULT for more information.
(更多信息参见 KVM_EXIT_MEMORY_FAULT。)

8.45 KVM_CAP_X86_GUEST_MODE(8.45 KVM_CAP_X86_GUEST_MODE)
--------------------------------------------------------

:Architectures: x86

(架构：x86。)

The presence of this capability indicates that KVM_RUN will update the
KVM_RUN_X86_GUEST_MODE bit in kvm_run.flags to indicate whether the
vCPU was executing nested guest code when it exited.
(此能力的存在表明，KVM_RUN 会更新 kvm_run.flags 中的 KVM_RUN_X86_GUEST_MODE 位，
以指示 vCPU 退出时是否正在执行嵌套客户机代码。)

8.46 KVM_CAP_S390_KEYOP(8.46 KVM_CAP_S390_KEYOP)
------------------------------------------------

:Architectures: s390

(架构：s390。)

The presence of this capability indicates that the KVM_S390_KEYOP ioctl is
available.
(此能力的存在表明 KVM_S390_KEYOP ioctl 可用。)

KVM exits with the register state of either the L1 or L2 guest
depending on which executed at the time of an exit. Userspace must
take care to differentiate between these cases.
(KVM 退出时会携带 L1 或 L2 客户机的寄存器状态，取决于退出时是哪一方在执行。用户
态必须注意区分这两种情况。)

8.47 KVM_CAP_S390_VSIE_ESAMODE(8.47 KVM_CAP_S390_VSIE_ESAMODE)
--------------------------------------------------------------

:Architectures: s390

(架构：s390。)

The presence of this capability indicates that the nested KVM guest can
start in ESA mode.
(此能力的存在表明嵌套 KVM 客户机可以以 ESA 模式启动。)

9. Known KVM API problems(9. KVM API 的已知问题)
================================================

In some cases, KVM's API has some inconsistencies or common pitfalls
that userspace need to be aware of.  This section details some of
these issues.
(某些情况下，KVM 的 API 存在一些不一致或常见陷阱，用户态需要了解它们。本节详细
说明其中一部分问题。)

Most of them are architecture specific, so the section is split by
architecture.
(其中大多数是架构特定的，因此本节按架构划分。)

9.1. x86(9.1. x86)
------------------

``KVM_GET_SUPPORTED_CPUID`` issues(``KVM_GET_SUPPORTED_CPUID`` 相关问题)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In general, ``KVM_GET_SUPPORTED_CPUID`` is designed so that it is possible
to take its result and pass it directly to ``KVM_SET_CPUID2``.  This section
documents some cases in which that requires some care.
(总体而言，``KVM_GET_SUPPORTED_CPUID`` 的设计使得可以把它返回的结果直接传给
``KVM_SET_CPUID2``。本节记录了一些需要小心处理的情况。)

Local APIC features(本地 APIC 特性)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CPU[EAX=1]:ECX[21] (X2APIC) is reported by ``KVM_GET_SUPPORTED_CPUID``,
but it can only be enabled if ``KVM_CREATE_IRQCHIP`` or
``KVM_ENABLE_CAP(KVM_CAP_IRQCHIP_SPLIT)`` are used to enable in-kernel emulation of
the local APIC.
(``KVM_GET_SUPPORTED_CPUID`` 会报告 CPU[EAX=1]:ECX[21] (X2APIC)，但只有在使用
``KVM_CREATE_IRQCHIP`` 或 ``KVM_ENABLE_CAP(KVM_CAP_IRQCHIP_SPLIT)`` 启用本地 APIC
的内核内模拟时，它才能被启用。)

The same is true for the ``KVM_FEATURE_PV_UNHALT`` paravirtualized feature.
(对于 ``KVM_FEATURE_PV_UNHALT`` 半虚拟化特性也是如此。)

On older versions of Linux, CPU[EAX=1]:ECX[24] (TSC_DEADLINE) is not reported by
``KVM_GET_SUPPORTED_CPUID``, but it can be enabled if ``KVM_CAP_TSC_DEADLINE_TIMER``
is present and the kernel has enabled in-kernel emulation of the local APIC.
On newer versions, ``KVM_GET_SUPPORTED_CPUID`` does report the bit as available.
(在较旧版本的 Linux 上，``KVM_GET_SUPPORTED_CPUID`` 不报告 CPU[EAX=1]:ECX[24]
(TSC_DEADLINE)，但如果 ``KVM_CAP_TSC_DEADLINE_TIMER`` 存在且内核已启用本地 APIC 的
内核内模拟，则它可以被启用。在较新版本上，``KVM_GET_SUPPORTED_CPUID`` 会报告该位
可用。)

CPU topology(CPU 拓扑)
~~~~~~~~~~~~~~~~~~~~~~

Several CPUID values include topology information for the host CPU:
0x0b and 0x1f for Intel systems, 0x8000001e for AMD systems.  Different
versions of KVM return different values for this information and userspace
should not rely on it.  Currently they return all zeroes.
(几个 CPUID 值包含宿主机 CPU 的拓扑信息：Intel 系统为 0x0b 与 0x1f，AMD 系统为
0x8000001e。不同版本的 KVM 为这些信息返回不同的值，用户态不应依赖它们。目前它们
返回全零。)

If userspace wishes to set up a guest topology, it should be careful that
the values of these three leaves differ for each CPU.  In particular,
the APIC ID is found in EDX for all subleaves of 0x0b and 0x1f, and in EAX
for 0x8000001e; the latter also encodes the core id and node id in bits
7:0 of EBX and ECX respectively.
(如果用户态想设置客户机拓扑，需要注意这三个叶的值对每个 CPU 都要有所不同。特别
地，0x0b 与 0x1f 的所有子叶中 APIC ID 位于 EDX，而 0x8000001e 中位于 EAX；后者还
在 EBX 与 ECX 的位 7:0 中分别编码 core id 与 node id。)

Obsolete ioctls and capabilities(已废弃的 ioctl 与能力)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

KVM_CAP_DISABLE_QUIRKS does not let userspace know which quirks are actually
available.  Use ``KVM_CHECK_EXTENSION(KVM_CAP_DISABLE_QUIRKS2)`` instead if
available.
(KVM_CAP_DISABLE_QUIRKS 不会让用户态知道哪些 quirks 实际可用。如果可用，请改用
``KVM_CHECK_EXTENSION(KVM_CAP_DISABLE_QUIRKS2)``。)

Ordering of KVM_GET_*/KVM_SET_* ioctls(KVM_GET_*/KVM_SET_* ioctl 的执行顺序)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TBD
