.. SPDX-License-Identifier: GPL-2.0

=============================================================
Known limitations of CPU virtualization(CPU 虚拟化的已知局限)
=============================================================

Whenever perfect emulation of a CPU feature is impossible or too hard, KVM
has to choose between not implementing the feature at all or introducing
behavioral differences between virtual machines and bare metal systems.
(当 CPU 特性的完美模拟无法实现或过于困难时，KVM 只能在"完全不实现该特性"与"引入虚拟机和裸机系统之间的行为差异"之间做出选择。)

This file documents some of the known limitations that KVM has in
virtualizing CPU features.
(本文件记录了 KVM 在虚拟化 CPU 特性方面的一些已知局限。)

x86(x86)
========

``KVM_GET_SUPPORTED_CPUID`` issues(KVM_GET_SUPPORTED_CPUID 相关问题)
--------------------------------------------------------------------

x87 features(x87 特性)
~~~~~~~~~~~~~~~~~~~~~~

Unlike most other CPUID feature bits, CPUID[EAX=7,ECX=0]:EBX[6]
(FDP_EXCPTN_ONLY) and CPUID[EAX=7,ECX=0]:EBX]13] (ZERO_FCS_FDS) are
clear if the features are present and set if the features are not present.
(与大多数其他 CPUID 特性位不同，CPUID[EAX=7,ECX=0]:EBX[6]
(FDP_EXCPTN_ONLY) 与 CPUID[EAX=7,ECX=0]:EBX]13] (ZERO_FCS_FDS) 在特性
存在时为清零状态，在特性不存在时为置位状态。)

Clearing these bits in CPUID has no effect on the operation of the guest;
if these bits are set on hardware, the features will not be present on
any virtual machine that runs on that hardware.
(在 CPUID 中清除这些位对客户机的运行没有任何影响；如果这些位在硬件上
已被置位，那么运行在该硬件上的任何虚拟机都不会具备这些特性。)

**Workaround:** It is recommended to always set these bits in guest CPUID.
Note however that any software (e.g ``WIN87EM.DLL``) expecting these features
to be present likely predates these CPUID feature bits, and therefore
doesn't know to check for them anyway.
(解决办法：建议始终在客户机 CPUID 中置位这些位。但请注意，任何期望这些
特性存在的软件 (例如 WIN87EM.DLL) 很可能早于这些 CPUID 特性位出现，因此
反正也不知道要去检查它们。)

``KVM_SET_VCPU_EVENTS`` issue(KVM_SET_VCPU_EVENTS 相关问题)
-----------------------------------------------------------

Invalid KVM_SET_VCPU_EVENTS input with respect to error codes *may* result in
failed VM-Entry on Intel CPUs.  Pre-CET Intel CPUs require that exception
injection through the VMCS correctly set the "error code valid" flag, e.g.
require the flag be set when injecting a #GP, clear when injecting a #UD,
clear when injecting a soft exception, etc.  Intel CPUs that enumerate
IA32_VMX_BASIC[56] as '1' relax VMX's consistency checks, and AMD CPUs have no
restrictions whatsoever.  KVM_SET_VCPU_EVENTS doesn't sanity check the vector
versus "has_error_code", i.e. KVM's ABI follows AMD behavior.
(就错误代码而言，无效的 KVM_SET_VCPU_EVENTS 输入可能会导致 Intel CPU 上的
VM-Entry 失败。Pre-CET Intel CPU 要求通过 VMCS 进行的异常注入正确设置
"error code valid" 标志，例如：注入 #GP 时要求置位该标志，注入 #UD 时要求
清零，注入软异常时要求清零，等等。将 IA32_VMX_BASIC[56] 枚举为 '1' 的
Intel CPU 放宽了 VMX 的一致性检查，而 AMD CPU 则没有任何限制。
KVM_SET_VCPU_EVENTS 不会对 vector 与 "has_error_code" 进行健全性检查，
也就是说 KVM 的 ABI 遵循 AMD 的行为。)

Nested virtualization features(嵌套虚拟化特性)
----------------------------------------------

On AMD CPUs, when GIF is cleared, #DB exceptions or traps due to a breakpoint
register match are ignored and discarded by the CPU. The CPU relies on the VMM
to fully virtualize this behavior, even when vGIF is enabled for the guest
(i.e. vGIF=0 does not cause the CPU to drop #DBs when the guest is running).
KVM does not virtualize this behavior as the complexity is unjustified given
the rarity of the use case. One way to handle this would be for KVM to
intercept the #DB, temporarily disable the breakpoint, single-step over the
instruction, then re-enable the breakpoint.
(在 AMD CPU 上，当 GIF 被清零时，由断点寄存器匹配引发的 #DB 异常或陷阱会被
CPU 忽略并丢弃。CPU 依赖 VMM 来完全虚拟化这一行为，即使已为客户机启用
vGIF (即当客户机运行时，vGIF=0 并不会使 CPU 丢弃 #DB)。考虑到该用例十分
罕见，虚拟化这一行为所需的复杂性并不划算，因此 KVM 没有虚拟化它。一种
处理方式是让 KVM 拦截 #DB，临时禁用断点，单步越过该指令，然后重新启用
断点。)

x2APIC(x2APIC)
--------------
When KVM_X2APIC_API_USE_32BIT_IDS is enabled, KVM activates a hack/quirk that
allows sending events to a single vCPU using its x2APIC ID even if the target
vCPU has legacy xAPIC enabled, e.g. to bring up hotplugged vCPUs via INIT-SIPI
on VMs with > 255 vCPUs.  A side effect of the quirk is that, if multiple vCPUs
have the same physical APIC ID, KVM will deliver events targeting that APIC ID
only to the vCPU with the lowest vCPU ID.  If KVM_X2APIC_API_USE_32BIT_IDS is
not enabled, KVM follows x86 architecture when processing interrupts (all vCPUs
matching the target APIC ID receive the interrupt).
(当 KVM_X2APIC_API_USE_32BIT_IDS 启用时，KVM 会激活一个 hack/quirk，允许
使用单个 vCPU 的 x2APIC ID 向其发送事件，即使目标 vCPU 启用了传统 xAPIC，
例如在 vCPU 数量超过 255 的虚拟机上通过 INIT-SIPI 唤起热插拔的 vCPU。该
quirk 的一个副作用是：如果多个 vCPU 拥有相同的物理 APIC ID，KVM 将把以
该 APIC ID 为目标的事件只投递给 vCPU ID 最小的那个 vCPU。如果
KVM_X2APIC_API_USE_32BIT_IDS 未启用，KVM 在处理中断时遵循 x86 架构
(所有匹配目标 APIC ID 的 vCPU 都会收到该中断)。)

MTRRs(MTRRs)
------------
KVM does not virtualize guest MTRR memory types.  KVM emulates accesses to MTRR
MSRs, i.e. {RD,WR}MSR in the guest will behave as expected, but KVM does not
honor guest MTRRs when determining the effective memory type, and instead
treats all of guest memory as having Writeback (WB) MTRRs.
(KVM 不虚拟化客户机的 MTRR 内存类型。KVM 会模拟对 MTRR MSR 的访问，即
客户机中的 {RD,WR}MSR 会表现得符合预期，但 KVM 在确定有效内存类型时不会
采纳客户机的 MTRR，而是把客户机的全部内存都视为具有 Writeback (WB)
MTRR。)

CR0.CD(CR0.CD)
--------------
KVM does not virtualize CR0.CD on Intel CPUs.  Similar to MTRR MSRs, KVM
emulates CR0.CD accesses so that loads and stores from/to CR0 behave as
expected, but setting CR0.CD=1 has no impact on the cachaeability of guest
memory.
(KVM 在 Intel CPU 上不虚拟化 CR0.CD。与 MTRR MSR 类似，KVM 会模拟对
CR0.CD 的访问，使对 CR0 的加载与存储表现得符合预期，但设置 CR0.CD=1 对
客户机内存的可缓存性没有影响。)

Note, this erratum does not affect AMD CPUs, which fully virtualize CR0.CD in
hardware, i.e. put the CPU caches into "no fill" mode when CR0.CD=1, even when
running in the guest.
(注意，此勘误不影响 AMD CPU，后者在硬件中完全虚拟化 CR0.CD，即当
CR0.CD=1 时把 CPU 缓存置于 "no fill" 模式，即使在客户机中运行时也是如此。)