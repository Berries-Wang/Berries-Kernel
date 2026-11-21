.. SPDX-License-Identifier: GPL-2.0

=========================================
Protected KVM (pKVM)(受保护的 KVM (pKVM))
=========================================

**NOTE**: pKVM is currently an experimental, development feature and
subject to breaking changes as new isolation features are implemented.
Please reach out to the developers at kvmarm@lists.linux.dev if you have
any questions.
(注意：pKVM 目前是一项实验性的开发中功能，随着新隔离特性的实现，
可能会有破坏性变更。如有任何疑问，请联系 kvmarm@lists.linux.dev 的
开发者。)

Overview(概述)
==============

Booting a host kernel with '``kvm-arm.mode=protected``' enables
"Protected KVM" (pKVM). During boot, pKVM installs a stage-2 identity
map page-table for the host and uses it to isolate the hypervisor
running at EL2 from the rest of the host running at EL1/0.
(以 '``kvm-arm.mode=protected``' 启动主机内核会启用 "Protected KVM"
(受保护的 KVM，pKVM)。在启动过程中，pKVM 为主机安装一个 stage-2
恒等映射页表，并用它把运行在 EL2 的 hypervisor 与运行在 EL1/0 的
主机其余部分隔离开。)

pKVM permits creation of protected virtual machines (pVMs) by passing
the ``KVM_VM_TYPE_ARM_PROTECTED`` machine type identifier to the
``KVM_CREATE_VM`` ioctl(). The hypervisor isolates pVMs from the host by
unmapping pages from the stage-2 identity map as they are accessed by a
pVM. Hypercalls are provided for a pVM to share specific regions of its
IPA space back with the host, allowing for communication with the VMM.
A Linux guest must be configured with ``CONFIG_ARM_PKVM_GUEST=y`` in
order to issue these hypercalls.
(pKVM 通过把 ``KVM_VM_TYPE_ARM_PROTECTED`` 机器类型标识符传给
``KVM_CREATE_VM`` ioctl() 来允许创建受保护的虚拟机(pVM)。当某个 pVM
访问页面时，hypervisor 会把这些页面从 stage-2 恒等映射中解除映射，
从而使 pVM 与主机隔离。hypervisor 提供了一些 hypercall，让 pVM 可以
把其 IPA 空间的特定区域共享回主机，以便与 VMM 通信。Linux guest 必须
配置 ``CONFIG_ARM_PKVM_GUEST=y`` 才能发出这些 hypercall。)

See hypercalls.rst for more details.
(详情参见 hypercalls.rst。)

Isolation mechanisms(隔离机制)
==============================

pKVM relies on a number of mechanisms to isolate PVMs from the host:
(pKVM 依靠多种机制把 PVM 与主机隔离：)

CPU memory isolation(CPU 内存隔离)
----------------------------------

Status: Isolation of anonymous memory and metadata pages.
(状态：匿名内存与元数据页面的隔离。)

Metadata pages (e.g. page-table pages and '``struct kvm_vcpu``' pages)
are donated from the host to the hypervisor during pVM creation and
are consequently unmapped from the stage-2 identity map until the pVM is
destroyed.
(元数据页面(例如页表页面和 '``struct kvm_vcpu``' 页面)在 pVM 创建
期间从主机捐赠给 hypervisor，因而在 pVM 销毁之前会一直从 stage-2
恒等映射中解除映射。)

Similarly to regular KVM, pages are lazily mapped into the guest in
response to stage-2 page faults handled by the host. However, when
running a pVM, these pages are first pinned and then unmapped from the
stage-2 identity map as part of the donation procedure. This gives rise
to some user-visible differences when compared to non-protected VMs,
largely due to the lack of MMU notifiers:
(与普通 KVM 类似，页面是在主机处理 stage-2 缺页中断时惰性映射到
guest 中的。然而，在运行 pVM 时，这些页面会先被固定(pin)，然后作为
捐赠过程的一部分从 stage-2 恒等映射中解除映射。与非受保护的 VM 相比，
这带来了一些用户可见的差异，主要原因是缺少 MMU 通知器：)

* Memslots cannot be moved or deleted once the pVM has started running.
  (pVM 开始运行后，memslot 不能再移动或删除。)
* Read-only memslots and dirty logging are not supported.
  (不支持只读 memslot 和脏页日志。)
* With the exception of swap, file-backed pages cannot be mapped into a
  pVM.
  (除 swap 外，文件支撑的页面不能映射进 pVM。)
* Donated pages are accounted against ``RLIMIT_MLOCK`` and so the VMM
  must have a sufficient resource limit or be granted ``CAP_IPC_LOCK``.
  The lack of a runtime reclaim mechanism means that memory locked for
  a pVM will remain locked until the pVM is destroyed.
  (捐赠的页面计入 ``RLIMIT_MLOCK``，因此 VMM 必须有足够的资源限制，
  或被授予 ``CAP_IPC_LOCK``。由于缺少运行时回收机制，为 pVM 锁定的
  内存将保持锁定，直到 pVM 被销毁。)
* Changes to the VMM address space (e.g. a ``MAP_FIXED`` mmap() over a
  mapping associated with a memslot) are not reflected in the guest and
  may lead to loss of coherency.
  (VMM 地址空间的变更(例如对与 memslot 关联的映射执行 ``MAP_FIXED``
  mmap())不会反映到 guest 中，并可能导致一致性丢失。)
* Accessing pVM memory that has not been shared back will result in the
  delivery of a SIGSEGV.
  (访问尚未共享回主机的 pVM 内存会导致收到 SIGSEGV。)
* If a system call accesses pVM memory that has not been shared back
  then it will either return ``-EFAULT`` or forcefully reclaim the
  memory pages. Reclaimed memory is zeroed by the hypervisor and a
  subsequent attempt to access it in the pVM will return ``-EFAULT``
  from the ``VCPU_RUN`` ioctl().
  (如果系统调用访问尚未共享回主机的 pVM 内存，它要么返回
  ``-EFAULT``，要么强制回收这些内存页。回收的内存由 hypervisor 置零，
  之后在 pVM 中再次访问它的尝试会从 ``VCPU_RUN`` ioctl() 返回
  ``-EFAULT``。)

CPU state isolation(CPU 状态隔离)
---------------------------------

Status: **Unimplemented.**
(状态：未实现。)

DMA isolation using an IOMMU(使用 IOMMU 的 DMA 隔离)
----------------------------------------------------

Status: **Unimplemented.**
(状态：未实现。)

Proxying of Trustzone services(Trustzone 服务的代理)
----------------------------------------------------

Status: FF-A and PSCI calls from the host are proxied by the pKVM
hypervisor.
(状态：来自主机的 FF-A 和 PSCI 调用由 pKVM hypervisor 代理。)

The FF-A proxy ensures that the host cannot share pVM or hypervisor
memory with Trustzone as part of a "confused deputy" attack.
(FF-A 代理确保主机无法在 "confused deputy" (受骗代理人)攻击中把
pVM 或 hypervisor 内存与 Trustzone 共享。)

The PSCI proxy ensures that CPUs always have the stage-2 identity map
installed when they are executing in the host.
(PSCI 代理确保 CPU 在主机中执行时始终安装有 stage-2 恒等映射。)

Protected VM firmware (pvmfw)(受保护的 VM 固件 (pvmfw))
-------------------------------------------------------

Status: **Unimplemented.**
(状态：未实现。)

Resources(参考资料)
===================

Quentin Perret's KVM Forum 2022 talk entitled "Protected KVM on arm64: A
technical deep dive" remains a good resource for learning more about
pKVM, despite some of the details having changed in the meantime:
(Quentin Perret 在 2022 年 KVM Forum 上的演讲 "Protected KVM on
arm64: A technical deep dive" 仍然是深入了解 pKVM 的好资料，尽管其中
一些细节在此期间已经改变：)

https://www.youtube.com/watch?v=9npebeVFbFw
