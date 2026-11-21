.. SPDX-License-Identifier: GPL-2.0

==================================================================
Internal ABI between the kernel and HYP(内核与 HYP 之间的内部 ABI)
==================================================================

This file documents the interaction between the Linux kernel and the
hypervisor layer when running Linux as a hypervisor (for example
KVM). It doesn't cover the interaction of the kernel with the
hypervisor when running as a guest (under Xen, KVM or any other
hypervisor), or any hypervisor-specific interaction when the kernel is
used as a host.
(本文件描述当 Linux 作为管理程序运行时 (例如 KVM)，Linux 内核与管理
程序 (hypervisor) 层之间的交互。它不涵盖内核作为客户机运行时 (在
Xen、KVM 或任何其他管理程序之下) 与管理程序的交互，也不涵盖内核用作
宿主时的任何特定于管理程序的交互。)

Note: KVM/arm has been removed from the kernel. The API described
here is still valid though, as it allows the kernel to kexec when
booted at HYP. It can also be used by a hypervisor other than KVM
if necessary.
(注意：KVM/arm 已从内核中移除。不过这里描述的 API 仍然有效，因为它允
许内核在以 HYP 级别引导时进行 kexec。必要时，它也可以被 KVM 以外的管
理程序使用。)

On arm and arm64 (without VHE), the kernel doesn't run in hypervisor
mode, but still needs to interact with it, allowing a built-in
hypervisor to be either installed or torn down.
(在 arm 和 arm64 (无 VHE) 上，内核不在管理程序模式下运行，但仍需要与
其交互，以允许安装或拆除内建的管理程序。)

In order to achieve this, the kernel must be booted at HYP (arm) or
EL2 (arm64), allowing it to install a set of stubs before dropping to
SVC/EL1. These stubs are accessible by using a 'hvc #0' instruction,
and only act on individual CPUs.
(为了做到这一点，内核必须在 HYP (arm) 或 EL2 (arm64) 级别引导，使其
能在降级到 SVC/EL1 之前安装一组桩 (stub)。这些桩通过使用 'hvc #0' 指
令来访问，并且只作用于单个 CPU。)

Unless specified otherwise, any built-in hypervisor must implement
these functions (see arch/arm{,64}/include/asm/virt.h):
(除非另有说明，任何内建的管理程序都必须实现这些函数 (参见
arch/arm{,64}/include/asm/virt.h)：)

* ::

    r0/x0 = HVC_SET_VECTORS
    r1/x1 = vectors

  Set HVBAR/VBAR_EL2 to 'vectors' to enable a hypervisor. 'vectors'
  must be a physical address, and respect the alignment requirements
  of the architecture. Only implemented by the initial stubs, not by
  Linux hypervisors.
  (把 HVBAR/VBAR_EL2 设置为 'vectors' 以启用管理程序。'vectors' 必须
  是物理地址，并满足架构的对齐要求。仅由初始桩实现，不由 Linux 管理
  程序实现。)

* ::

    r0/x0 = HVC_RESET_VECTORS

  Turn HYP/EL2 MMU off, and reset HVBAR/VBAR_EL2 to the initials
  stubs' exception vector value. This effectively disables an existing
  hypervisor.
  (关闭 HYP/EL2 的 MMU，并把 HVBAR/VBAR_EL2 重置为初始桩的异常向量
  值。这实际上会禁用已存在的管理程序。)

* ::

    r0/x0 = HVC_SOFT_RESTART
    r1/x1 = restart address
    x2 = x0's value when entering the next payload (arm64)
    x3 = x1's value when entering the next payload (arm64)
    x4 = x2's value when entering the next payload (arm64)

  Mask all exceptions, disable the MMU, clear I+D bits, move the arguments
  into place (arm64 only), and jump to the restart address while at HYP/EL2.
  This hypercall is not expected to return to its caller.
  (屏蔽所有异常、禁用 MMU、清除 I+D 位、把参数移到相应位置 (仅
  arm64)，并在 HYP/EL2 级别跳转到重启地址。此超级调用预期不会返回到
  其调用者。)

* ::

    x0 = HVC_FINALISE_EL2 (arm64 only)

  Finish configuring EL2 depending on the command-line options,
  including an attempt to upgrade the kernel's exception level from
  EL1 to EL2 by enabling the VHE mode. This is conditioned by the CPU
  supporting VHE, the EL2 MMU being off, and VHE not being disabled by
  any other means (command line option, for example).
  (根据命令行选项完成 EL2 的配置，包括尝试通过启用 VHE 模式把内核的
  异常级别从 EL1 升级到 EL2。这取决于 CPU 支持 VHE、EL2 的 MMU 已关
  闭、以及 VHE 未被任何其他手段 (例如命令行选项) 禁用。)

Any other value of r0/x0 triggers a hypervisor-specific handling,
which is not documented here.
(r0/x0 的任何其他值都会触发特定于管理程序的处理，此处不予文档化。)

The return value of a stub hypercall is held by r0/x0, and is 0 on
success, and HVC_STUB_ERR on error. A stub hypercall is allowed to
clobber any of the caller-saved registers (x0-x18 on arm64, r0-r3 and
ip on arm). It is thus recommended to use a function call to perform
the hypercall.
(桩超级调用的返回值保存在 r0/x0 中：成功时为 0，出错时为
HVC_STUB_ERR。桩超级调用可以破坏任何调用者保存的寄存器 (arm64 上的
x0-x18，arm 上的 r0-r3 和 ip)。因此建议使用函数调用来执行该超级调
用。)
