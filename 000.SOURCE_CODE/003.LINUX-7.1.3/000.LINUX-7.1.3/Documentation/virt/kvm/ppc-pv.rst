.. SPDX-License-Identifier: GPL-2.0

=======================================================
The PPC KVM paravirtual interface(PPC KVM 半虚拟化接口)
=======================================================

The basic execution principle by which KVM on PowerPC works is to run all kernel
space code in PR=1 which is user space. This way we trap all privileged
instructions and can emulate them accordingly.
(KVM on PowerPC 的基本执行原理是让所有内核态代码都运行在 PR=1(即用户态)下。这样我们可以捕获所有特权指令，并相应地对其进行模拟。)

Unfortunately that is also the downfall. There are quite some privileged
instructions that needlessly return us to the hypervisor even though they
could be handled differently.
(不幸的是，这也是其缺陷所在。有相当多的特权指令明明可以用不同的方式处理，却不必要地使我们返回到 hypervisor。)

This is what the PPC PV interface helps with. It takes privileged instructions
and transforms them into unprivileged ones with some help from the hypervisor.
This cuts down virtualization costs by about 50% on some of my benchmarks.
(这正是 PPC PV 接口所要解决的问题。它在 hypervisor 的一些帮助下，把特权指令转换成非特权指令。在一些基准测试中，这将虚拟化开销降低了约 50%。)

The code for that interface can be found in arch/powerpc/kernel/kvm*
(该接口的代码可以在 arch/powerpc/kernel/kvm* 中找到)

Querying for existence(查询是否存在)
====================================

To find out if we're running on KVM or not, we leverage the device tree. When
Linux is running on KVM, a node /hypervisor exists. That node contains a
compatible property with the value "linux,kvm".
(要弄清我们是否运行在 KVM 之上，可以借助设备树。当 Linux 运行在 KVM 上时，会存在一个 /hypervisor 节点。该节点包含一个 compatible 属性，其值为 "linux,kvm"。)

Once you determined you're running under a PV capable KVM, you can now use
hypercalls as described below.
(一旦确定自己运行在支持 PV 的 KVM 之下，就可以按下文所述使用超调用(hypercall)了。)

KVM hypercalls(KVM 超调用)
==========================

Inside the device tree's /hypervisor node there's a property called
'hypercall-instructions'. This property contains at most 4 opcodes that make
up the hypercall. To call a hypercall, just call these instructions.
(在设备树的 /hypervisor 节点中有一个名为 'hypercall-instructions' 的属性。该属性包含最多 4 条构成超调用的操作码。要发起一次超调用，只需执行这些指令即可。)

The parameters are as follows:
(参数如下：)

        ========	================	================
	Register	IN			OUT
	寄存器		入口			出口
        ========	================	================
	r0		-			volatile(易变)
	r3		1st parameter		Return code(返回码)
			第 1 个参数
	r4		2nd parameter		1st output value(第 1 个输出值)
			第 2 个参数
	r5		3rd parameter		2nd output value(第 2 个输出值)
			第 3 个参数
	r6		4th parameter		3rd output value(第 3 个输出值)
			第 4 个参数
	r7		5th parameter		4th output value(第 4 个输出值)
			第 5 个参数
	r8		6th parameter		5th output value(第 5 个输出值)
			第 6 个参数
	r9		7th parameter		6th output value(第 6 个输出值)
			第 7 个参数
	r10		8th parameter		7th output value(第 7 个输出值)
			第 8 个参数
	r11		hypercall number	8th output value(第 8 个输出值)
			超调用号
	r12		-			volatile(易变)
        ========	================	================

Hypercall definitions are shared in generic code, so the same hypercall numbers
apply for x86 and powerpc alike with the exception that each KVM hypercall
also needs to be ORed with the KVM vendor code which is (42 << 16).
(超调用的定义在通用代码中共享，因此相同的超调用编号同时适用于 x86 和 powerpc；唯一的例外是，每个 KVM 超调用还需要与 KVM 厂商标识码(42 << 16)进行按位或运算。)

Return codes can be as follows:
(返回码可能如下：)

	====		=========================
	Code		Meaning
	编码		含义
	====		=========================
	0		Success(成功)
	12		Hypercall not implemented(未实现该超调用)
	<0		Error(错误)
	====		=========================

The magic page(魔法页)
======================

To enable communication between the hypervisor and guest there is a new shared
page that contains parts of supervisor visible register state. The guest can
map this shared page using the KVM hypercall KVM_HC_PPC_MAP_MAGIC_PAGE.
(为了实现 hypervisor 与客户机之间的通信，有一个新的共享页，其中包含部分对特权级可见的寄存器状态。客户机可以使用 KVM 超调用 KVM_HC_PPC_MAP_MAGIC_PAGE 来映射这个共享页。)

With this hypercall issued the guest always gets the magic page mapped at the
desired location. The first parameter indicates the effective address when the
MMU is enabled. The second parameter indicates the address in real mode, if
applicable to the target. For now, we always map the page to -4096. This way we
can access it using absolute load and store functions. The following
instruction reads the first field of the magic page::

	ld	rX, -4096(0)

(发出该超调用后，客户机总是能把魔法页(magic page)映射到期望的位置。第一个参数指示启用 MMU 时的有效地址。第二个参数指示实模式下的地址(如果目标平台适用)。目前，我们总是把该页映射到 -4096。这样我们就可以使用绝对寻址的加载/存储函数来访问它。以下指令读取魔法页的第一个字段：)

The interface is designed to be extensible should there be need later to add
additional registers to the magic page. If you add fields to the magic page,
also define a new hypercall feature to indicate that the host can give you more
registers. Only if the host supports the additional features, make use of them.
(该接口在设计上是可扩展的，以便日后需要时可以向魔法页添加更多寄存器。如果你向魔法页添加了字段，还应当定义一个新的超调用特性标志，以表明宿主机可以为你提供更多寄存器。只有当宿主机支持这些附加特性时，才使用它们。)

The magic page layout is described by struct kvm_vcpu_arch_shared
in arch/powerpc/include/uapi/asm/kvm_para.h.
(魔法页的布局由 arch/powerpc/include/uapi/asm/kvm_para.h 中的 struct kvm_vcpu_arch_shared 描述。)

Magic page features(魔法页特性)
===============================

When mapping the magic page using the KVM hypercall KVM_HC_PPC_MAP_MAGIC_PAGE,
a second return value is passed to the guest. This second return value contains
a bitmap of available features inside the magic page.
(使用 KVM 超调用 KVM_HC_PPC_MAP_MAGIC_PAGE 映射魔法页时，会有第二个返回值传递给客户机。该第二个返回值包含魔法页内可用特性的位图。)

The following enhancements to the magic page are currently available:
(目前可用的魔法页增强功能如下：)

  ============================  =======================================
  KVM_MAGIC_FEAT_SR		Maps SR registers r/w in the magic page(将 SR 寄存器以可读写方式映射到魔法页)
  KVM_MAGIC_FEAT_MAS0_TO_SPRG7	Maps MASn, ESR, PIR and high SPRGs(映射 MASn、ESR、PIR 与高位 SPRG)
  ============================  =======================================

For enhanced features in the magic page, please check for the existence of the
feature before using them!
(对于魔法页的增强特性，使用之前请务必检查该特性是否存在！)

Magic page flags(魔法页标志)
============================

In addition to features that indicate whether a host is capable of a particular
feature we also have a channel for a guest to tell the host whether it's capable
of something. This is what we call "flags".
(除了用于表明宿主机是否具备某项特性的特性标志之外，我们还有一个通道让客户机告知宿主机自己是否具备某种能力。这就是我们所说的"标志"(flags)。)

Flags are passed to the host in the low 12 bits of the Effective Address.
(标志通过有效地址的低 12 位传递给宿主机。)

The following flags are currently available for a guest to expose:
(客户机目前可以暴露的标志如下：)

  MAGIC_PAGE_FLAG_NOT_MAPPED_NX Guest handles NX bits correctly wrt magic page
  (MAGIC_PAGE_FLAG_NOT_MAPPED_NX：客户机在涉及魔法页时正确处理 NX 位。)

MSR bits(MSR 位)
================

The MSR contains bits that require hypervisor intervention and bits that do
not require direct hypervisor intervention because they only get interpreted
when entering the guest or don't have any impact on the hypervisor's behavior.
(MSR 中既包含需要 hypervisor 干预的位，也包含不需要 hypervisor 直接干预的位——后者要么只在进入客户机时才被解释，要么对 hypervisor 的行为没有任何影响。)

The following bits are safe to be set inside the guest:
(在客户机内部设置以下位是安全的：)

  - MSR_EE
    (MSR_EE)
  - MSR_RI
    (MSR_RI)

If any other bit changes in the MSR, please still use mtmsr(d).
(如果 MSR 中的任何其他位发生变化，请仍然使用 mtmsr(d)。)

Patched instructions(被补丁替换的指令)
======================================

The "ld" and "std" instructions are transformed to "lwz" and "stw" instructions
respectively on 32-bit systems with an added offset of 4 to accommodate for big
endianness.
(在 32 位系统上，"ld" 和 "std" 指令分别被转换成 "lwz" 和 "stw" 指令，并加上偏移量 4 以适应大端序。)

The following is a list of mapping the Linux kernel performs when running as
guest. Implementing any of those mappings is optional, as the instruction traps
also act on the shared page. So calling privileged instructions still works as
before.
(以下是 Linux 内核作为客户机运行时所执行的映射列表。这些映射的实现都是可选的，因为指令陷阱同样会作用于共享页。因此，调用特权指令仍然可以像以前一样工作。)

======================= ================================
From			To
源			目标
======================= ================================
mfmsr	rX		ld	rX, magic_page->msr
mfsprg	rX, 0		ld	rX, magic_page->sprg0
mfsprg	rX, 1		ld	rX, magic_page->sprg1
mfsprg	rX, 2		ld	rX, magic_page->sprg2
mfsprg	rX, 3		ld	rX, magic_page->sprg3
mfsrr0	rX		ld	rX, magic_page->srr0
mfsrr1	rX		ld	rX, magic_page->srr1
mfdar	rX		ld	rX, magic_page->dar
mfdsisr	rX		lwz	rX, magic_page->dsisr

mtmsr	rX		std	rX, magic_page->msr
mtsprg	0, rX		std	rX, magic_page->sprg0
mtsprg	1, rX		std	rX, magic_page->sprg1
mtsprg	2, rX		std	rX, magic_page->sprg2
mtsprg	3, rX		std	rX, magic_page->sprg3
mtsrr0	rX		std	rX, magic_page->srr0
mtsrr1	rX		std	rX, magic_page->srr1
mtdar	rX		std	rX, magic_page->dar
mtdsisr	rX		stw	rX, magic_page->dsisr

tlbsync			nop

mtmsrd	rX, 0		b	<special mtmsr section>(跳转到特殊的 mtmsr 处理段)
mtmsr	rX		b	<special mtmsr section>(跳转到特殊的 mtmsr 处理段)

mtmsrd	rX, 1		b	<special mtmsrd section>(跳转到特殊的 mtmsrd 处理段)

[Book3S only]			(仅 Book3S)
mtsrin	rX, rY		b	<special mtsrin section>(跳转到特殊的 mtsrin 处理段)

[BookE only]			(仅 BookE)
wrteei	[0|1]		b	<special wrteei section>(跳转到特殊的 wrteei 处理段)
======================= ================================

Some instructions require more logic to determine what's going on than a load
or store instruction can deliver. To enable patching of those, we keep some
RAM around where we can live translate instructions to. What happens is the
following:
(有些指令需要比一条加载或存储指令所能实现的更多的逻辑来判断当前状况。为了能够对这些指令打补丁，我们保留了一些 RAM 用于就地翻译指令。其过程如下：)

	1) copy emulation code to memory
	2) patch that code to fit the emulated instruction
	3) patch that code to return to the original pc + 4
	4) patch the original instruction to branch to the new code

(1) 把模拟代码复制到内存中；2) 对该代码打补丁，使其匹配被模拟的指令；3) 对该代码打补丁，使其返回到原始 pc + 4；4) 对原始指令打补丁，使其跳转到新代码。)

That way we can inject an arbitrary amount of code as replacement for a single
instruction. This allows us to check for pending interrupts when setting EE=1
for example.
(通过这种方式，我们可以注入任意数量的代码来替换单条指令。例如，这使得我们在设置 EE=1 时能够检查是否有待处理的中断。)

Hypercall ABIs in KVM on PowerPC(PowerPC 上 KVM 的超调用 ABI)
=============================================================

1) KVM hypercalls (ePAPR)(1) KVM 超调用(ePAPR)

These are ePAPR compliant hypercall implementation (mentioned above). Even
generic hypercalls are implemented here, like the ePAPR idle hcall. These are
available on all targets.
(这些是符合 ePAPR 的超调用实现(如上所述)。即使是通用超调用也在这里实现，例如 ePAPR idle hcall。它们在所有目标上都可用。)

2) PAPR hypercalls(2) PAPR 超调用)

PAPR hypercalls are needed to run server PowerPC PAPR guests (-M pseries in QEMU).
These are the same hypercalls that pHyp, the POWER hypervisor, implements. Some of
them are handled in the kernel, some are handled in user space. This is only
available on book3s_64.
(运行服务器级 PowerPC PAPR 客户机(QEMU 中的 -M pseries)时需要 PAPR 超调用。这些与 POWER hypervisor pHyp 所实现的超调用相同。其中一些在内核中处理，另一些在用户空间处理。仅在 book3s_64 上可用。)

3) OSI hypercalls(3) OSI 超调用)

Mac-on-Linux is another user of KVM on PowerPC, which has its own hypercall (long
before KVM). This is supported to maintain compatibility. All these hypercalls get
forwarded to user space. This is only useful on book3s_32, but can be used with
book3s_64 as well.
(Mac-on-Linux 是 KVM on PowerPC 的另一个使用者，它拥有自己的超调用(远早于 KVM 出现)。支持这些超调用是为了维持兼容性。所有这些超调用都会被转发到用户空间。它们只在 book3s_32 上有用，但也可以在 book3s_64 上使用。)
