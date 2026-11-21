.. SPDX-License-Identifier: GPL-2.0

Overview(概述)
==============
The Linux kernel contains a variety of code for running as a fully
enlightened guest on Microsoft's Hyper-V hypervisor.  Hyper-V
consists primarily of a bare-metal hypervisor plus a virtual machine
management service running in the parent partition (roughly
equivalent to KVM and QEMU, for example).  Guest VMs run in child
partitions.  In this documentation, references to Hyper-V usually
encompass both the hypervisor and the VMM service without making a
distinction about which functionality is provided by which
component.
(Linux 内核包含多种代码，用于在 Microsoft 的 Hyper-V hypervisor 上作为完全受启发的(fully enlightened)客户机运行。Hyper-V 主要由一个裸金属 hypervisor 加上一个运行在父分区中的虚拟机管理服务组成(大致相当于 KVM 与 QEMU 的组合)。客户机 VM 运行在子分区中。在本文档中，提及 Hyper-V 时通常同时涵盖 hypervisor 与 VMM 服务，而不区分哪个功能由哪个组件提供。)

Hyper-V runs on x86/x64 and arm64 architectures, and Linux guests
are supported on both.  The functionality and behavior of Hyper-V is
generally the same on both architectures unless noted otherwise.
(Hyper-V 运行在 x86/x64 与 arm64 架构上，两者都支持 Linux 客户机。除非另有说明，Hyper-V 在两种架构上的功能与行为大体相同。)

Linux Guest Communication with Hyper-V(Linux 客户机与 Hyper-V 的通信)
---------------------------------------------------------------------
Linux guests communicate with Hyper-V in four different ways:
(Linux 客户机通过四种不同的方式与 Hyper-V 通信：)

* Implicit traps: As defined by the x86/x64 or arm64 architecture,
  some guest actions trap to Hyper-V.  Hyper-V emulates the action and
  returns control to the guest.  This behavior is generally invisible
  to the Linux kernel.
  (隐式陷阱(implicit traps)：按照 x86/x64 或 arm64 架构的定义，某些客户机动作会陷入(trap)到 Hyper-V。Hyper-V 模拟该动作并把控制权返回给客户机。这种行为对 Linux 内核来说通常是不可见的。)

* Explicit hypercalls: Linux makes an explicit function call to
  Hyper-V, passing parameters.  Hyper-V performs the requested action
  and returns control to the caller.  Parameters are passed in
  processor registers or in memory shared between the Linux guest and
  Hyper-V.   On x86/x64, hypercalls use a Hyper-V specific calling
  sequence.  On arm64, hypercalls use the ARM standard SMCCC calling
  sequence.
  (显式超调用(explicit hypercalls)：Linux 显式调用 Hyper-V 并传递参数。Hyper-V 执行所请求的动作并把控制权返回给调用者。参数通过处理器寄存器或 Linux 客户机与 Hyper-V 之间共享的内存传递。在 x86/x64 上，超调用使用 Hyper-V 特有的调用序列。在 arm64 上，超调用使用 ARM 标准 SMCCC 调用序列。)

* Synthetic register access: Hyper-V implements a variety of
  synthetic registers.  On x86/x64 these registers appear as MSRs in
  the guest, and the Linux kernel can read or write these MSRs using
  the normal mechanisms defined by the x86/x64 architecture.  On
  arm64, these synthetic registers must be accessed using explicit
  hypercalls.
  (合成寄存器访问(synthetic register access)：Hyper-V 实现了多种合成寄存器。在 x86/x64 上，这些寄存器以 MSR 的形式出现在客户机中，Linux 内核可以使用 x86/x64 架构定义的常规机制读写这些 MSR。在 arm64 上，这些合成寄存器必须通过显式超调用来访问。)

* VMBus: VMBus is a higher-level software construct that is built on
  the other 3 mechanisms.  It is a message passing interface between
  the Hyper-V host and the Linux guest.  It uses memory that is shared
  between Hyper-V and the guest, along with various signaling
  mechanisms.
  (VMBus：VMBus 是构建在其余 3 种机制之上的更高层软件结构。它是 Hyper-V 主机与 Linux 客户机之间的消息传递接口。它使用 Hyper-V 与客户机之间共享的内存，以及各种信令机制。)

The first three communication mechanisms are documented in the
`Hyper-V Top Level Functional Spec (TLFS)`_.  The TLFS describes
general Hyper-V functionality and provides details on the hypercalls
and synthetic registers.  The TLFS is currently written for the
x86/x64 architecture only.
(前三种通信机制记述在 Hyper-V 顶层功能规范(TLFS)中。TLFS 描述了 Hyper-V 的一般功能，并提供关于超调用与合成寄存器的细节。TLFS 目前仅针对 x86/x64 架构编写。)

.. _Hyper-V Top Level Functional Spec (TLFS): https://docs.microsoft.com/en-us/virtualization/hyper-v-on-windows/tlfs/tlfs

VMBus is not documented.  This documentation provides a high-level
overview of VMBus and how it works, but the details can be discerned
only from the code.
(VMBus 没有公开文档。本文档提供 VMBus 及其工作方式的高层概述，但细节只能从代码中获知。)

Sharing Memory(共享内存)
------------------------
Many aspects are communication between Hyper-V and Linux are based
on sharing memory.  Such sharing is generally accomplished as
follows:
(Hyper-V 与 Linux 之间的许多通信方面都基于内存共享。这种共享通常按以下方式实现：)

* Linux allocates memory from its physical address space using
  standard Linux mechanisms.
  (Linux 使用标准 Linux 机制从其物理地址空间分配内存。)

* Linux tells Hyper-V the guest physical address (GPA) of the
  allocated memory.  Many shared areas are kept to 1 page so that a
  single GPA is sufficient.   Larger shared areas require a list of
  GPAs, which usually do not need to be contiguous in the guest
  physical address space.  How Hyper-V is told about the GPA or list
  of GPAs varies.  In some cases, a single GPA is written to a
  synthetic register.  In other cases, a GPA or list of GPAs is sent
  in a VMBus message.
  (Linux 把所分配内存的客户机物理地址(GPA)告知 Hyper-V。许多共享区域保持为 1 页，因此单个 GPA 就足够了。更大的共享区域需要一份 GPA 列表，这些 GPA 在客户机物理地址空间中通常无需连续。把 GPA 或 GPA 列表告知 Hyper-V 的方式各不相同：有时把单个 GPA 写入合成寄存器；有时通过 VMBus 消息发送 GPA 或 GPA 列表。)

* Hyper-V translates the GPAs into "real" physical memory addresses,
  and creates a virtual mapping that it can use to access the memory.
  (Hyper-V 把这些 GPA 转换为"真实"的物理内存地址，并创建一个可供自己访问该内存的虚拟映射。)

* Linux can later revoke sharing it has previously established by
  telling Hyper-V to set the shared GPA to zero.
  (Linux 之后可以通过让 Hyper-V 把共享的 GPA 置为零，来撤销先前建立的共享。)

Hyper-V operates with a page size of 4 Kbytes. GPAs communicated to
Hyper-V may be in the form of page numbers, and always describe a
range of 4 Kbytes.  Since the Linux guest page size on x86/x64 is
also 4 Kbytes, the mapping from guest page to Hyper-V page is 1-to-1.
On arm64, Hyper-V supports guests with 4/16/64 Kbyte pages as
defined by the arm64 architecture.   If Linux is using 16 or 64
Kbyte pages, Linux code must be careful to communicate with Hyper-V
only in terms of 4 Kbyte pages.  HV_HYP_PAGE_SIZE and related macros
are used in code that communicates with Hyper-V so that it works
correctly in all configurations.
(Hyper-V 使用 4 KB 的页大小。传递给 Hyper-V 的 GPA 可以是页号的形式，并且总是描述 4 KB 的范围。由于 x86/x64 上 Linux 客户机的页大小也是 4 KB，客户机页到 Hyper-V 页的映射是一对一的。在 arm64 上，Hyper-V 支持 arm64 架构定义的 4/16/64 KB 页的客户机。如果 Linux 使用 16 KB 或 64 KB 页，Linux 代码必须注意只以 4 KB 页为单位与 Hyper-V 通信。与 Hyper-V 通信的代码中使用 HV_HYP_PAGE_SIZE 及相关宏，以确保在所有配置下都能正确工作。)

As described in the TLFS, a few memory pages shared between Hyper-V
and the Linux guest are "overlay" pages.  With overlay pages, Linux
uses the usual approach of allocating guest memory and telling
Hyper-V the GPA of the allocated memory.  But Hyper-V then replaces
that physical memory page with a page it has allocated, and the
original physical memory page is no longer accessible in the guest
VM.  Linux may access the memory normally as if it were the memory
that it originally allocated.  The "overlay" behavior is visible
only because the contents of the page (as seen by Linux) change at
the time that Linux originally establishes the sharing and the
overlay page is inserted.  Similarly, the contents change if Linux
revokes the sharing, in which case Hyper-V removes the overlay page,
and the guest page originally allocated by Linux becomes visible
again.
(如 TLFS 所述，Hyper-V 与 Linux 客户机之间共享的少数内存页是"覆盖"(overlay)页。对于覆盖页，Linux 仍采用常规做法：分配客户机内存并把所分配内存的 GPA 告知 Hyper-V。但随后 Hyper-V 会用它自己分配的一个页替换那个物理内存页，原始物理内存页在客户机 VM 中不再可访问。Linux 仍可以像访问它最初分配的内存一样正常访问该内存。之所以能察觉到"覆盖"行为，只是因为(在 Linux 看来)该页的内容在 Linux 最初建立共享、插入覆盖页的时刻发生了变化。类似地，如果 Linux 撤销共享，内容也会变化——此时 Hyper-V 移除覆盖页，Linux 最初分配的客户机页重新变得可见。)

Before Linux does a kexec to a kdump kernel or any other kernel,
memory shared with Hyper-V should be revoked.  Hyper-V could modify
a shared page or remove an overlay page after the new kernel is
using the page for a different purpose, corrupting the new kernel.
Hyper-V does not provide a single "set everything" operation to
guest VMs, so Linux code must individually revoke all sharing before
doing kexec.   See hv_kexec_handler() and hv_crash_handler().  But
the crash/panic path still has holes in cleanup because some shared
pages are set using per-CPU synthetic registers and there's no
mechanism to revoke the shared pages for CPUs other than the CPU
running the panic path.
(在 Linux 通过 kexec 进入 kdump 内核或任何其他内核之前，应当撤销与 Hyper-V 共享的内存。在新内核把某个页用于其他用途之后，Hyper-V 仍可能修改该共享页或移除覆盖页，从而破坏新内核。Hyper-V 没有为客户机 VM 提供单一的"全部设置"操作，因此 Linux 代码必须在执行 kexec 之前逐项撤销所有共享。参见 hv_kexec_handler() 与 hv_crash_handler()。但崩溃/panic 路径的清理仍有漏洞，因为某些共享页是使用每 CPU 的合成寄存器设置的，而目前没有机制能为运行 panic 路径的 CPU 之外的 CPU 撤销共享页。)

CPU Management(CPU 管理)
------------------------
Hyper-V does not have a ability to hot-add or hot-remove a CPU
from a running VM.  However, Windows Server 2019 Hyper-V and
earlier versions may provide guests with ACPI tables that indicate
more CPUs than are actually present in the VM.  As is normal, Linux
treats these additional CPUs as potential hot-add CPUs, and reports
them as such even though Hyper-V will never actually hot-add them.
Starting in Windows Server 2022 Hyper-V, the ACPI tables reflect
only the CPUs actually present in the VM, so Linux does not report
any hot-add CPUs.
(Hyper-V 不具备向运行中的 VM 热添加或热移除 CPU 的能力。不过，Windows Server 2019 Hyper-V 及更早版本可能向客户机提供的 ACPI 表中列出的 CPU 数量多于 VM 中实际存在的数量。按照惯例，Linux 把这些额外的 CPU 视为潜在的热添加 CPU 并如此报告，即使 Hyper-V 实际上永远不会热添加它们。从 Windows Server 2022 Hyper-V 开始，ACPI 表只反映 VM 中实际存在的 CPU，因此 Linux 不再报告任何热添加 CPU。)

A Linux guest CPU may be taken offline using the normal Linux
mechanisms, provided no VMBus channel interrupts are assigned to
the CPU.  See the section on VMBus Interrupts for more details
on how VMBus channel interrupts can be re-assigned to permit
taking a CPU offline.
(只要没有 VMBus 通道中断被指派给某个 Linux 客户机 CPU，就可以用常规的 Linux 机制把该 CPU 下线。关于如何重新指派 VMBus 通道中断以允许 CPU 下线的更多细节，参见 VMBus 中断一节。)

32-bit and 64-bit(32 位与 64 位)
--------------------------------
On x86/x64, Hyper-V supports 32-bit and 64-bit guests, and Linux
will build and run in either version. While the 32-bit version is
expected to work, it is used rarely and may suffer from undetected
regressions.
(在 x86/x64 上，Hyper-V 支持 32 位与 64 位客户机，Linux 可以以任一版本构建并运行。虽然 32 位版本预计可以工作，但很少被使用，可能存在未被发现的性能退化。)

On arm64, Hyper-V supports only 64-bit guests.
(在 arm64 上，Hyper-V 只支持 64 位客户机。)

Endian-ness(字节序)
-------------------
All communication between Hyper-V and guest VMs uses Little-Endian
format on both x86/x64 and arm64.  Big-endian format on arm64 is not
supported by Hyper-V, and Linux code does not use endian-ness macros
when accessing data shared with Hyper-V.
(Hyper-V 与客户机 VM 之间的所有通信在 x86/x64 和 arm64 上都使用小端(Little-Endian)格式。Hyper-V 不支持 arm64 的大端格式，Linux 代码在访问与 Hyper-V 共享的数据时也不使用字节序宏。)

Versioning(版本)
----------------
Current Linux kernels operate correctly with older versions of
Hyper-V back to Windows Server 2012 Hyper-V. Support for running
on the original Hyper-V release in Windows Server 2008/2008 R2
has been removed.
(当前的 Linux 内核可以与回溯到 Windows Server 2012 Hyper-V 的旧版本 Hyper-V 正确协作。对在 Windows Server 2008/2008 R2 中最初版 Hyper-V 上运行的支持已被移除。)

A Linux guest on Hyper-V outputs in dmesg the version of Hyper-V
it is running on.  This version is in the form of a Windows build
number and is for display purposes only. Linux code does not
test this version number at runtime to determine available features
and functionality. Hyper-V indicates feature/function availability
via flags in synthetic MSRs that Hyper-V provides to the guest,
and the guest code tests these flags.
(运行在 Hyper-V 上的 Linux 客户机会在 dmesg 中输出它所运行的 Hyper-V 版本。该版本以 Windows 内部版本号的形式表示，仅用于显示。Linux 代码不会在运行时检测这个版本号来判断可用的特性与功能。Hyper-V 通过提供给客户机的合成 MSR 中的标志来指示特性/功能的可用性，由客户机代码检测这些标志。)

VMBus has its own protocol version that is negotiated during the
initial VMBus connection from the guest to Hyper-V. This version
number is also output to dmesg during boot.  This version number
is checked in a few places in the code to determine if specific
functionality is present.
(VMBus 有自己的协议版本，在客户机初次连接 Hyper-V 时协商。该版本号也会在启动期间输出到 dmesg。代码中有少数几处会检查该版本号，以判断是否存在特定功能。)

Furthermore, each synthetic device on VMBus also has a protocol
version that is separate from the VMBus protocol version. Device
drivers for these synthetic devices typically negotiate the device
protocol version, and may test that protocol version to determine
if specific device functionality is present.
(此外，VMBus 上的每个合成设备还拥有独立于 VMBus 协议版本的设备协议版本。这些合成设备的驱动通常会协商设备协议版本，并可能检测该协议版本以判断是否存在特定的设备功能。)

Code Packaging(代码组织)
------------------------
Hyper-V related code appears in the Linux kernel code tree in three
main areas:
(Hyper-V 相关代码在 Linux 内核代码树中主要出现在三个区域：)

1. drivers/hv

2. arch/x86/hyperv and arch/arm64/hyperv
   (arch/x86/hyperv 与 arch/arm64/hyperv)

3. individual device driver areas such as drivers/scsi, drivers/net,
   drivers/clocksource, etc.
   (各个独立的设备驱动区域，例如 drivers/scsi、drivers/net、drivers/clocksource 等。)

A few miscellaneous files appear elsewhere. See the full list under
"Hyper-V/Azure CORE AND DRIVERS" and "DRM DRIVER FOR HYPERV
SYNTHETIC VIDEO DEVICE" in the MAINTAINERS file.
(还有少量杂项文件位于其他位置。完整列表见 MAINTAINERS 文件中的 "Hyper-V/Azure CORE AND DRIVERS" 与 "DRM DRIVER FOR HYPERV SYNTHETIC VIDEO DEVICE" 条目。)

The code in #1 and #2 is built only when CONFIG_HYPERV is set.
Similarly, the code for most Hyper-V related drivers is built only
when CONFIG_HYPERV is set.
(#1 与 #2 中的代码只在设置 CONFIG_HYPERV 时构建。类似地，大多数 Hyper-V 相关驱动的代码也只在设置 CONFIG_HYPERV 时构建。)

Most Hyper-V related code in #1 and #3 can be built as a module.
The architecture specific code in #2 must be built-in.  Also,
drivers/hv/hv_common.c is low-level code that is common across
architectures and must be built-in.
(#1 与 #3 中大多数 Hyper-V 相关代码可以构建为模块。#2 中的架构相关代码必须内建。此外，drivers/hv/hv_common.c 是跨架构通用的底层代码，必须内建。)
