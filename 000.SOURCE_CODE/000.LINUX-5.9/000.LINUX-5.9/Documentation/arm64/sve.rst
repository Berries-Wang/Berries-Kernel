===================================================
Scalable Vector Extension support for AArch64 Linux
(AArch64 Linux 的可伸缩向量扩展支持)
===================================================

Author: Dave Martin <Dave.Martin@arm.com>
(作者: Dave Martin <Dave.Martin@arm.com>)

Date:   4 August 2017
(日期: 2017年8月4日)

This document outlines briefly the interface provided to userspace by Linux in
order to support use of the ARM Scalable Vector Extension (SVE).
(本文档简要概述了 Linux 为用户空间提供的接口，以支持 ARM 可伸缩向量扩展(SVE)的使用。)

This is an outline of the most important features and issues only and not
intended to be exhaustive.
(本文仅概述最重要的功能和问题，并不意图面面俱到。)

This document does not aim to describe the SVE architecture or programmer's
model.  To aid understanding, a minimal description of relevant programmer's
model features for SVE is included in Appendix A.
(本文档不旨在描述 SVE 架构或编程模型。为帮助理解，附录 A 中包含了 SVE 相关编程模型特性的最小描述。)


1.  General
(1. 概述)
-----------

* SVE registers Z0..Z31, P0..P15 and FFR and the current vector length VL, are
  tracked per-thread.
  (SVE 寄存器 Z0..Z31、P0..P15 和 FFR 以及当前向量长度 VL，均按线程进行跟踪。)

* The presence of SVE is reported to userspace via HWCAP_SVE in the aux vector
  AT_HWCAP entry.  Presence of this flag implies the presence of the SVE
  instructions and registers, and the Linux-specific system interfaces
  described in this document.  SVE is reported in /proc/cpuinfo as "sve".
  (SVE 的存在通过辅助向量 AT_HWCAP 条目中的 HWCAP_SVE 向用户空间报告。此标志的存在意味着 SVE 指令和寄存器以及本文档中描述的 Linux 特定系统接口的存在。SVE 在 /proc/cpuinfo 中报告为 "sve"。)

* Support for the execution of SVE instructions in userspace can also be
  detected by reading the CPU ID register ID_AA64PFR0_EL1 using an MRS
  instruction, and checking that the value of the SVE field is nonzero. [3]
  (用户空间中 SVE 指令执行的支持也可以通过使用 MRS 指令读取 CPU ID 寄存器 ID_AA64PFR0_EL1，并检查 SVE 字段的值是否为非零来检测。[3])

  It does not guarantee the presence of the system interfaces described in the
  following sections: software that needs to verify that those interfaces are
  present must check for HWCAP_SVE instead.
  (这并不保证以下章节中描述的系统接口的存在：需要验证这些接口是否存在的软件必须检查 HWCAP_SVE。)

* On hardware that supports the SVE2 extensions, HWCAP2_SVE2 will also
  be reported in the AT_HWCAP2 aux vector entry.  In addition to this,
  optional extensions to SVE2 may be reported by the presence of:
  (在支持 SVE2 扩展的硬件上，HWCAP2_SVE2 也将在 AT_HWCAP2 辅助向量条目中报告。除此之外，SVE2 的可选扩展可以通过以下标志的存在来报告：)

	HWCAP2_SVE2
	HWCAP2_SVEAES
	HWCAP2_SVEPMULL
	HWCAP2_SVEBITPERM
	HWCAP2_SVESHA3
	HWCAP2_SVESM4

  This list may be extended over time as the SVE architecture evolves.
  (随着 SVE 架构的发展，此列表可能会随时间扩展。)

  These extensions are also reported via the CPU ID register ID_AA64ZFR0_EL1,
  which userspace can read using an MRS instruction.  See elf_hwcaps.txt and
  cpu-feature-registers.txt for details.
  (这些扩展也通过 CPU ID 寄存器 ID_AA64ZFR0_EL1 报告，用户空间可以使用 MRS 指令读取。详情请参见 elf_hwcaps.txt 和 cpu-feature-registers.txt。)

* Debuggers should restrict themselves to interacting with the target via the
  NT_ARM_SVE regset.  The recommended way of detecting support for this regset
  is to connect to a target process first and then attempt a
  ptrace(PTRACE_GETREGSET, pid, NT_ARM_SVE, &iov).
  (调试器应限制自己通过 NT_ARM_SVE 寄存器集与目标交互。检测此寄存器集支持的推荐方法是先连接到目标进程，然后尝试 ptrace(PTRACE_GETREGSET, pid, NT_ARM_SVE, &iov)。)

* Whenever SVE scalable register values (Zn, Pn, FFR) are exchanged in memory
  between userspace and the kernel, the register value is encoded in memory in
  an endianness-invariant layout, with bits [(8 * i + 7) : (8 * i)] encoded at
  byte offset i from the start of the memory representation.  This affects for
  example the signal frame (struct sve_context) and ptrace interface
  (struct user_sve_header) and associated data.
  (每当用户空间和内核之间在内存中交换 SVE 可伸缩寄存器值(Zn、Pn、FFR)时，寄存器值以字节序不变的布局编码在内存中，位[(8 * i + 7): (8 * i)]编码在从内存表示起始处偏移 i 字节的位置。这影响例如信号帧(struct sve_context)和 ptrace 接口(struct user_sve_header)及相关数据。)

  Beware that on big-endian systems this results in a different byte order than
  for the FPSIMD V-registers, which are stored as single host-endian 128-bit
  values, with bits [(127 - 8 * i) : (120 - 8 * i)] of the register encoded at
  byte offset i.  (struct fpsimd_context, struct user_fpsimd_state).
  (注意，在大端系统上，这会导致与 FPSIMD V 寄存器不同的字节序，后者存储为单个主机字节序的 128 位值，寄存器的位[(127 - 8 * i): (120 - 8 * i)]编码在字节偏移 i 处。(struct fpsimd_context, struct user_fpsimd_state)。)


2.  Vector length terminology
(2. 向量长度术语)
-----------------------------

The size of an SVE vector (Z) register is referred to as the "vector length".
(SVE 向量(Z)寄存器的大小称为"向量长度"。)

To avoid confusion about the units used to express vector length, the kernel
adopts the following conventions:
(为避免混淆用于表示向量长度的单位，内核采用以下约定：)

* Vector length (VL) = size of a Z-register in bytes
  (向量长度(VL) = Z 寄存器的大小，以字节为单位)

* Vector quadwords (VQ) = size of a Z-register in units of 128 bits
  (向量四字(VQ) = Z 寄存器的大小，以 128 位为单位)

(So, VL = 16 * VQ.)
(因此，VL = 16 * VQ。)

The VQ convention is used where the underlying granularity is important, such
as in data structure definitions.  In most other situations, the VL convention
is used.  This is consistent with the meaning of the "VL" pseudo-register in
the SVE instruction set architecture.
(在底层粒度很重要的情况下使用 VQ 约定，例如在数据结构定义中。在大多数其他情况下，使用 VL 约定。这与 SVE 指令集架构中"VL"伪寄存器的含义一致。)


3.  System call behaviour
(3. 系统调用行为)
-------------------------

* On syscall, V0..V31 are preserved (as without SVE).  Thus, bits [127:0] of
  Z0..Z31 are preserved.  All other bits of Z0..Z31, and all of P0..P15 and FFR
  become unspecified on return from a syscall.
  (在系统调用时，V0..V31 被保留(与没有 SVE 时一样)。因此，Z0..Z31 的位[127:0]被保留。Z0..Z31 的所有其他位，以及 P0..P15 和 FFR 的所有位在从系统调用返回时变为未指定值。)

* The SVE registers are not used to pass arguments to or receive results from
  any syscall.
  (SVE 寄存器不用于向任何系统调用传递参数或从任何系统调用接收结果。)

* In practice the affected registers/bits will be preserved or will be replaced
  with zeros on return from a syscall, but userspace should not make
  assumptions about this.  The kernel behaviour may vary on a case-by-case
  basis.
  (实际上，受影响的寄存器/位在从系统调用返回时将被保留或替换为零，但用户空间不应对此做出假设。内核行为可能因具体情况而异。)

* All other SVE state of a thread, including the currently configured vector
  length, the state of the PR_SVE_VL_INHERIT flag, and the deferred vector
  length (if any), is preserved across all syscalls, subject to the specific
  exceptions for execve() described in section 6.
  (线程的所有其他 SVE 状态，包括当前配置的向量长度、PR_SVE_VL_INHERIT 标志的状态以及延迟向量长度(如果有)，在所有系统调用中都被保留，但第 6 节中描述的 execve() 的特定例外情况除外。)

  In particular, on return from a fork() or clone(), the parent and new child
  process or thread share identical SVE configuration, matching that of the
  parent before the call.
  (特别是，在从 fork() 或 clone() 返回时，父进程和新子进程或线程共享相同的 SVE 配置，与调用前父进程的配置匹配。)


4.  Signal handling
(4. 信号处理)
-------------------

* A new signal frame record sve_context encodes the SVE registers on signal
  delivery. [1]
  (新的信号帧记录 sve_context 在信号传递时编码 SVE 寄存器。[1])

* This record is supplementary to fpsimd_context.  The FPSR and FPCR registers
  are only present in fpsimd_context.  For convenience, the content of V0..V31
  is duplicated between sve_context and fpsimd_context.
  (此记录是 fpsimd_context 的补充。FPSR 和 FPCR 寄存器仅存在于 fpsimd_context 中。为方便起见，V0..V31 的内容在 sve_context 和 fpsimd_context 之间重复。)

* The signal frame record for SVE always contains basic metadata, in particular
  the thread's vector length (in sve_context.vl).
  (SVE 的信号帧记录始终包含基本元数据，特别是线程的向量长度(在 sve_context.vl 中)。)

* The SVE registers may or may not be included in the record, depending on
  whether the registers are live for the thread.  The registers are present if
  and only if:
  sve_context.head.size >= SVE_SIG_CONTEXT_SIZE(sve_vq_from_vl(sve_context.vl)).
  (SVE 寄存器可能包含在记录中，也可能不包含，这取决于寄存器对线程是否活跃。寄存器存在当且仅当:
  sve_context.head.size >= SVE_SIG_CONTEXT_SIZE(sve_vq_from_vl(sve_context.vl))。)

* If the registers are present, the remainder of the record has a vl-dependent
  size and layout.  Macros SVE_SIG_* are defined [1] to facilitate access to
  the members.
  (如果寄存器存在，记录的其余部分具有依赖于 vl 的大小和布局。宏 SVE_SIG_* 在[1]中定义，以便于访问成员。)

* Each scalable register (Zn, Pn, FFR) is stored in an endianness-invariant
  layout, with bits [(8 * i + 7) : (8 * i)] stored at byte offset i from the
  start of the register's representation in memory.
  (每个可伸缩寄存器(Zn、Pn、FFR)以字节序不变的布局存储，位[(8 * i + 7): (8 * i)]存储在从寄存器内存表示起始处偏移 i 字节的位置。)

* If the SVE context is too big to fit in sigcontext.__reserved[], then extra
  space is allocated on the stack, an extra_context record is written in
  __reserved[] referencing this space.  sve_context is then written in the
  extra space.  Refer to [1] for further details about this mechanism.
  (如果 SVE 上下文太大无法放入 sigcontext.__reserved[]，则在栈上分配额外空间，在 __reserved[] 中写入一个 extra_context 记录来引用此空间。然后 sve_context 被写入额外空间。有关此机制的更多细节，请参考[1]。)


5.  Signal return
(5. 信号返回)
-----------------

When returning from a signal handler:
(当从信号处理程序返回时：)

* If there is no sve_context record in the signal frame, or if the record is
  present but contains no register data as desribed in the previous section,
  then the SVE registers/bits become non-live and take unspecified values.
  (如果信号帧中没有 sve_context 记录，或者记录存在但不包含前一节所述的寄存器数据，则 SVE 寄存器/位变为非活跃并取未指定值。)

* If sve_context is present in the signal frame and contains full register
  data, the SVE registers become live and are populated with the specified
  data.  However, for backward compatibility reasons, bits [127:0] of Z0..Z31
  are always restored from the corresponding members of fpsimd_context.vregs[]
  and not from sve_context.  The remaining bits are restored from sve_context.
  (如果 sve_context 存在于信号帧中且包含完整的寄存器数据，SVE 寄存器变为活跃并使用指定的数据填充。但是，出于向后兼容的原因，Z0..Z31 的位[127:0]始终从 fpsimd_context.vregs[] 的相应成员恢复，而不是从 sve_context 恢复。其余位从 sve_context 恢复。)

* Inclusion of fpsimd_context in the signal frame remains mandatory,
  irrespective of whether sve_context is present or not.
  (无论 sve_context 是否存在，信号帧中都必须包含 fpsimd_context。)

* The vector length cannot be changed via signal return.  If sve_context.vl in
  the signal frame does not match the current vector length, the signal return
  attempt is treated as illegal, resulting in a forced SIGSEGV.
  (向量长度不能通过信号返回来改变。如果信号帧中的 sve_context.vl 与当前向量长度不匹配，信号返回尝试将被视为非法，导致强制 SIGSEGV。)


6.  prctl extensions
(6. prctl 扩展)
--------------------

Some new prctl() calls are added to allow programs to manage the SVE vector
length:
(添加了一些新的 prctl() 调用，允许程序管理 SVE 向量长度：)

prctl(PR_SVE_SET_VL, unsigned long arg)

    Sets the vector length of the calling thread and related flags, where
    arg == vl | flags.  Other threads of the calling process are unaffected.
    (设置调用线程的向量长度和相关标志，其中 arg == vl | flags。调用进程的其他线程不受影响。)

    vl is the desired vector length, where sve_vl_valid(vl) must be true.
    (vl 是所需的向量长度，其中 sve_vl_valid(vl) 必须为真。)

    flags:
    (标志：)

	PR_SVE_VL_INHERIT

	    Inherit the current vector length across execve().  Otherwise, the
	    vector length is reset to the system default at execve().  (See
	    Section 9.)
	    (跨 execve() 继承当前向量长度。否则，向量长度在 execve() 时重置为系统默认值。(参见第 9 节。))

	PR_SVE_SET_VL_ONEXEC

	    Defer the requested vector length change until the next execve()
	    performed by this thread.
	    (将请求的向量长度更改推迟到此线程执行的下一个 execve()。)

	    The effect is equivalent to implicit exceution of the following
	    call immediately after the next execve() (if any) by the thread:
	    (其效果等效于在线程的下一个 execve()(如果有)之后立即隐式执行以下调用：)

		prctl(PR_SVE_SET_VL, arg & ~PR_SVE_SET_VL_ONEXEC)

	    This allows launching of a new program with a different vector
	    length, while avoiding runtime side effects in the caller.
	    (这允许以不同的向量长度启动新程序，同时避免调用者中的运行时副作用。)


	    Without PR_SVE_SET_VL_ONEXEC, the requested change takes effect
	    immediately.
	    (如果没有 PR_SVE_SET_VL_ONEXEC，请求的更改立即生效。)


    Return value: a nonnegative on success, or a negative value on error:
	EINVAL: SVE not supported, invalid vector length requested, or
	    invalid flags.
    (返回值: 成功时返回非负值，错误时返回负值：
	EINVAL: SVE 不支持、请求了无效的向量长度或无效的标志。)


    On success:
    (成功时：)

    * Either the calling thread's vector length or the deferred vector length
      to be applied at the next execve() by the thread (dependent on whether
      PR_SVE_SET_VL_ONEXEC is present in arg), is set to the largest value
      supported by the system that is less than or equal to vl.  If vl ==
      SVE_VL_MAX, the value set will be the largest value supported by the
      system.
      (调用线程的向量长度或线程在下一次 execve() 时要应用的延迟向量长度(取决于 arg 中是否存在 PR_SVE_SET_VL_ONEXEC)，被设置为系统支持的小于或等于 vl 的最大值。如果 vl == SVE_VL_MAX，设置的值将是系统支持的最大值。)

    * Any previously outstanding deferred vector length change in the calling
      thread is cancelled.
      (调用线程中任何先前未完成的延迟向量长度更改将被取消。)

    * The returned value describes the resulting configuration, encoded as for
      PR_SVE_GET_VL.  The vector length reported in this value is the new
      current vector length for this thread if PR_SVE_SET_VL_ONEXEC was not
      present in arg; otherwise, the reported vector length is the deferred
      vector length that will be applied at the next execve() by the calling
      thread.
      (返回的值描述结果配置，编码方式与 PR_SVE_GET_VL 相同。如果 arg 中不存在 PR_SVE_SET_VL_ONEXEC，此值中报告的向量长度是此线程的新当前向量长度；否则，报告的向量长度是将由调用线程在下一次 execve() 时应用的延迟向量长度。)

    * Changing the vector length causes all of P0..P15, FFR and all bits of
      Z0..Z31 except for Z0 bits [127:0] .. Z31 bits [127:0] to become
      unspecified.  Calling PR_SVE_SET_VL with vl equal to the thread's current
      vector length, or calling PR_SVE_SET_VL with the PR_SVE_SET_VL_ONEXEC
      flag, does not constitute a change to the vector length for this purpose.
      (更改向量长度会导致 P0..P15、FFR 和 Z0..Z31 的所有位(除了 Z0 位[127:0] .. Z31 位[127:0])变为未指定。使用等于线程当前向量长度的 vl 调用 PR_SVE_SET_VL，或使用 PR_SVE_SET_VL_ONEXEC 标志调用 PR_SVE_SET_VL，不会构成对此目的的向量长度更改。)


prctl(PR_SVE_GET_VL)

    Gets the vector length of the calling thread.
    (获取调用线程的向量长度。)

    The following flag may be OR-ed into the result:
    (以下标志可以通过 OR 运算加入结果中：)

	PR_SVE_VL_INHERIT

	    Vector length will be inherited across execve().
	    (向量长度将在 execve() 时被继承。)

    There is no way to determine whether there is an outstanding deferred
    vector length change (which would only normally be the case between a
    fork() or vfork() and the corresponding execve() in typical use).
    (无法确定是否存在未完成的延迟向量长度更改(通常仅在 fork() 或 vfork() 与典型使用中相应的 execve() 之间才会出现这种情况)。)

    To extract the vector length from the result, and it with
    PR_SVE_VL_LEN_MASK.
    (要从结果中提取向量长度，将其与 PR_SVE_VL_LEN_MASK 进行按位与运算。)

    Return value: a nonnegative value on success, or a negative value on error:
	EINVAL: SVE not supported.
    (返回值: 成功时返回非负值，错误时返回负值：
	EINVAL: SVE 不支持。)


7.  ptrace extensions
(7. ptrace 扩展)
---------------------

* A new regset NT_ARM_SVE is defined for use with PTRACE_GETREGSET and
  PTRACE_SETREGSET.
  (定义了一个新的寄存器集 NT_ARM_SVE，用于 PTRACE_GETREGSET 和 PTRACE_SETREGSET。)

  Refer to [2] for definitions.
  (定义请参考[2]。)

The regset data starts with struct user_sve_header, containing:
(寄存器集数据以 struct user_sve_header 开始，包含：)

    size

	Size of the complete regset, in bytes.
	This depends on vl and possibly on other things in the future.
	(完整寄存器集的大小，以字节为单位。
	这取决于 vl，将来可能还取决于其他因素。)

	If a call to PTRACE_GETREGSET requests less data than the value of
	size, the caller can allocate a larger buffer and retry in order to
	read the complete regset.
	(如果对 PTRACE_GETREGSET 的调用请求的数据少于 size 的值，调用者可以分配更大的缓冲区并重试，以便读取完整的寄存器集。)

    max_size

	Maximum size in bytes that the regset can grow to for the target
	thread.  The regset won't grow bigger than this even if the target
	thread changes its vector length etc.
	(目标线程的寄存器集可以增长到的最大大小(以字节为单位)。即使目标线程更改其向量长度等，寄存器集也不会增长超过此大小。)

    vl

	Target thread's current vector length, in bytes.
	(目标线程的当前向量长度，以字节为单位。)

    max_vl

	Maximum possible vector length for the target thread.
	(目标线程的最大可能向量长度。)

    flags

	either
	(以下之一)

	    SVE_PT_REGS_FPSIMD

		SVE registers are not live (GETREGSET) or are to be made
		non-live (SETREGSET).
		(SVE 寄存器不活跃(GETREGSET)或将被设为不活跃(SETREGSET)。)

		The payload is of type struct user_fpsimd_state, with the same
		meaning as for NT_PRFPREG, starting at offset
		SVE_PT_FPSIMD_OFFSET from the start of user_sve_header.
		(有效载荷类型为 struct user_fpsimd_state，其含义与 NT_PRFPREG 相同，从 user_sve_header 起始处的 SVE_PT_FPSIMD_OFFSET 偏移处开始。)

		Extra data might be appended in the future: the size of the
		payload should be obtained using SVE_PT_FPSIMD_SIZE(vq, flags).
		(将来可能会追加额外数据：有效载荷的大小应使用 SVE_PT_FPSIMD_SIZE(vq, flags) 获取。)

		vq should be obtained using sve_vq_from_vl(vl).
		(vq 应使用 sve_vq_from_vl(vl) 获取。)

		or
		(或)

	    SVE_PT_REGS_SVE

		SVE registers are live (GETREGSET) or are to be made live
		(SETREGSET).
		(SVE 寄存器活跃(GETREGSET)或将被设为活跃(SETREGSET)。)

		The payload contains the SVE register data, starting at offset
		SVE_PT_SVE_OFFSET from the start of user_sve_header, and with
		size SVE_PT_SVE_SIZE(vq, flags);
		(有效载荷包含 SVE 寄存器数据，从 user_sve_header 起始处的 SVE_PT_SVE_OFFSET 偏移处开始，大小为 SVE_PT_SVE_SIZE(vq, flags);)

	... OR-ed with zero or more of the following flags, which have the same
	meaning and behaviour as the corresponding PR_SET_VL_* flags:
	(……可与零个或多个以下标志进行 OR 运算，这些标志与相应的 PR_SET_VL_* 标志具有相同的含义和行为：)

	    SVE_PT_VL_INHERIT

	    SVE_PT_VL_ONEXEC (SETREGSET only).
	    (SVE_PT_VL_ONEXEC (仅 SETREGSET)。)

* The effects of changing the vector length and/or flags are equivalent to
  those documented for PR_SVE_SET_VL.
  (更改向量长度和/或标志的效果等效于 PR_SVE_SET_VL 文档中描述的效果。)

  The caller must make a further GETREGSET call if it needs to know what VL is
  actually set by SETREGSET, unless is it known in advance that the requested
  VL is supported.
  (如果调用者需要知道 SETREGSET 实际设置了什么 VL，则必须进行进一步的 GETREGSET 调用，除非预先知道请求的 VL 是受支持的。)

* In the SVE_PT_REGS_SVE case, the size and layout of the payload depends on
  the header fields.  The SVE_PT_SVE_*() macros are provided to facilitate
  access to the members.
  (在 SVE_PT_REGS_SVE 情况下，有效载荷的大小和布局取决于头部字段。提供了 SVE_PT_SVE_*() 宏以便于访问成员。)

* In either case, for SETREGSET it is permissible to omit the payload, in which
  case only the vector length and flags are changed (along with any
  consequences of those changes).
  (在任何一种情况下，对于 SETREGSET，允许省略有效载荷，此时仅更改向量长度和标志(以及这些更改的任何后果)。)

* For SETREGSET, if an SVE_PT_REGS_SVE payload is present and the
  requested VL is not supported, the effect will be the same as if the
  payload were omitted, except that an EIO error is reported.  No
  attempt is made to translate the payload data to the correct layout
  for the vector length actually set.  The thread's FPSIMD state is
  preserved, but the remaining bits of the SVE registers become
  unspecified.  It is up to the caller to translate the payload layout
  for the actual VL and retry.
  (对于 SETREGSET，如果存在 SVE_PT_REGS_SVE 有效载荷且请求的 VL 不受支持，效果将与省略有效载荷相同，只是会报告 EIO 错误。不会尝试将有效载荷数据转换为实际设置的向量长度的正确布局。线程的 FPSIMD 状态被保留，但 SVE 寄存器的其余位变为未指定。由调用者来转换实际 VL 的有效载荷布局并重试。)

* The effect of writing a partial, incomplete payload is unspecified.
  (写入部分、不完整的有效载荷的效果是未指定的。)


8.  ELF coredump extensions
(8. ELF 核心转储扩展)
---------------------------

* A NT_ARM_SVE note will be added to each coredump for each thread of the
  dumped process.  The contents will be equivalent to the data that would have
  been read if a PTRACE_GETREGSET of NT_ARM_SVE were executed for each thread
  when the coredump was generated.
  (将为转储进程的每个线程向每个核心转储添加一个 NT_ARM_SVE 注释。其内容等效于在生成核心转储时对每个线程执行 NT_ARM_SVE 的 PTRACE_GETREGSET 所读取的数据。)


9.  System runtime configuration
(9. 系统运行时配置)
--------------------------------

* To mitigate the ABI impact of expansion of the signal frame, a policy
  mechanism is provided for administrators, distro maintainers and developers
  to set the default vector length for userspace processes:
  (为了减轻信号帧扩展对 ABI 的影响，为管理员、发行版维护者和开发人员提供了一个策略机制来设置用户空间进程的默认向量长度：)

/proc/sys/abi/sve_default_vector_length

    Writing the text representation of an integer to this file sets the system
    default vector length to the specified value, unless the value is greater
    than the maximum vector length supported by the system in which case the
    default vector length is set to that maximum.
    (将整数的文本表示写入此文件会将系统默认向量长度设置为指定值，除非该值大于系统支持的最大向量长度，在这种情况下，默认向量长度将设置为该最大值。)

    The result can be determined by reopening the file and reading its
    contents.
    (可以通过重新打开文件并读取其内容来确定结果。)

    At boot, the default vector length is initially set to 64 or the maximum
    supported vector length, whichever is smaller.  This determines the initial
    vector length of the init process (PID 1).
    (在启动时，默认向量长度最初设置为 64 或最大支持的向量长度中较小的那个。这决定了 init 进程(PID 1)的初始向量长度。)

    Reading this file returns the current system default vector length.
    (读取此文件返回当前系统默认向量长度。)

* At every execve() call, the new vector length of the new process is set to
  the system default vector length, unless
  (在每次 execve() 调用时，新进程的新向量长度被设置为系统默认向量长度，除非)

    * PR_SVE_VL_INHERIT (or equivalently SVE_PT_VL_INHERIT) is set for the
      calling thread, or
      (为调用线程设置了 PR_SVE_VL_INHERIT(或等效的 SVE_PT_VL_INHERIT)，或)

    * a deferred vector length change is pending, established via the
      PR_SVE_SET_VL_ONEXEC flag (or SVE_PT_VL_ONEXEC).
      (存在通过 PR_SVE_SET_VL_ONEXEC 标志(或 SVE_PT_VL_ONEXEC)建立的待处理延迟向量长度更改。)

* Modifying the system default vector length does not affect the vector length
  of any existing process or thread that does not make an execve() call.
  (修改系统默认向量长度不会影响任何不进行 execve() 调用的现有进程或线程的向量长度。)


Appendix A.  SVE programmer's model (informative)
(附录 A. SVE 编程模型(信息性))
=================================================

This section provides a minimal description of the additions made by SVE to the
ARMv8-A programmer's model that are relevant to this document.
(本节提供了 SVE 对 ARMv8-A 编程模型所做补充的最小描述，这些补充与本文档相关。)

Note: This section is for information only and not intended to be complete or
to replace any architectural specification.
(注意: 本节仅供参考，不旨在完整或替代任何架构规范。)

A.1.  Registers
(寄存器)
---------------

In A64 state, SVE adds the following:
(在 A64 状态下，SVE 添加了以下内容：)

* 32 8VL-bit vector registers Z0..Z31
  (32 个 8VL 位向量寄存器 Z0..Z31)
  For each Zn, Zn bits [127:0] alias the ARMv8-A vector register Vn.
  (对于每个 Zn，Zn 位[127:0]是 ARMv8-A 向量寄存器 Vn 的别名。)

  A register write using a Vn register name zeros all bits of the corresponding
  Zn except for bits [127:0].
  (使用 Vn 寄存器名称的寄存器写入会将相应 Zn 的所有位清零，除了位[127:0]。)

* 16 VL-bit predicate registers P0..P15
  (16 个 VL 位谓词寄存器 P0..P15)

* 1 VL-bit special-purpose predicate register FFR (the "first-fault register")
  (1 个 VL 位专用谓词寄存器 FFR("首次故障寄存器"))

* a VL "pseudo-register" that determines the size of each vector register
  (一个 VL"伪寄存器"，决定每个向量寄存器的大小)

  The SVE instruction set architecture provides no way to write VL directly.
  Instead, it can be modified only by EL1 and above, by writing appropriate
  system registers.
  (SVE 指令集架构不提供直接写入 VL 的方式。相反，它只能由 EL1 及以上级别通过写入适当的系统寄存器来修改。)

* The value of VL can be configured at runtime by EL1 and above:
  16 <= VL <= VLmax, where VL must be a multiple of 16.
  (VL 的值可以在运行时由 EL1 及以上级别配置:
  16 <= VL <= VLmax，其中 VL 必须是 16 的倍数。)

* The maximum vector length is determined by the hardware:
  16 <= VLmax <= 256.
  (最大向量长度由硬件决定:
  16 <= VLmax <= 256。)

  (The SVE architecture specifies 256, but permits future architecture
  revisions to raise this limit.)
  ((SVE 架构规定为 256，但允许未来的架构修订提高此限制。))

* FPSR and FPCR are retained from ARMv8-A, and interact with SVE floating-point
  operations in a similar way to the way in which they interact with ARMv8
  floating-point operations::
  (FPSR 和 FPCR 从 ARMv8-A 保留，并与 SVE 浮点运算交互，方式类似于它们与 ARMv8 浮点运算的交互方式：)

         8VL-1                       128               0  bit index
        +----          ////            -----------------+
     Z0 |                               :       V0      |
      :                                          :
     Z7 |                               :       V7      |
     Z8 |                               :     * V8      |
      :                                       :  :
    Z15 |                               :     *V15      |
    Z16 |                               :      V16      |
      :                                          :
    Z31 |                               :      V31      |
        +----          ////            -----------------+
                                                 31    0
         VL-1                  0                +-------+
        +----       ////      --+          FPSR |       |
     P0 |                       |               +-------+
      : |                       |         *FPCR |       |
    P15 |                       |               +-------+
        +----       ////      --+
    FFR |                       |               +-----+
        +----       ////      --+            VL |     |
                                                +-----+

(*) callee-save:
    This only applies to bits [63:0] of Z-/V-registers.
    FPCR contains callee-save and caller-save bits.  See [4] for details.
((*) 被调用者保存:
    这仅适用于 Z-/V-寄存器的位[63:0]。
    FPCR 包含被调用者保存和调用者保存位。详情请参见[4]。)


A.2.  Procedure call standard
(过程调用标准)
-----------------------------

The ARMv8-A base procedure call standard is extended as follows with respect to
the additional SVE register state:
(ARMv8-A 基础过程调用标准针对额外的 SVE 寄存器状态扩展如下：)

* All SVE register bits that are not shared with FP/SIMD are caller-save.
  (所有不与 FP/SIMD 共享的 SVE 寄存器位都是调用者保存的。)

* Z8 bits [63:0] .. Z15 bits [63:0] are callee-save.
  (Z8 位[63:0] .. Z15 位[63:0]是被调用者保存的。)

  This follows from the way these bits are mapped to V8..V15, which are caller-
  save in the base procedure call standard.
  (这是因为这些位映射到 V8..V15，而 V8..V15 在基础过程调用标准中是调用者保存的。)


Appendix B.  ARMv8-A FP/SIMD programmer's model
(附录 B. ARMv8-A FP/SIMD 编程模型)
===============================================

Note: This section is for information only and not intended to be complete or
to replace any architectural specification.
(注意: 本节仅供参考，不旨在完整或替代任何架构规范。)

Refer to [4] for more information.
(更多信息请参考[4]。)

ARMv8-A defines the following floating-point / SIMD register state:
(ARMv8-A 定义了以下浮点/SIMD 寄存器状态：)

* 32 128-bit vector registers V0..V31
  (32 个 128 位向量寄存器 V0..V31)
* 2 32-bit status/control registers FPSR, FPCR
  (2 个 32 位状态/控制寄存器 FPSR, FPCR)

::

         127           0  bit index
        +---------------+
     V0 |               |
      : :               :
     V7 |               |
   * V8 |               |
   :  : :               :
   *V15 |               |
    V16 |               |
      : :               :
    V31 |               |
        +---------------+

                 31    0
                +-------+
           FPSR |       |
                +-------+
          *FPCR |       |
                +-------+

(*) callee-save:
    This only applies to bits [63:0] of V-registers.
    FPCR contains a mixture of callee-save and caller-save bits.
((*) 被调用者保存:
    这仅适用于 V-寄存器的位[63:0]。
    FPCR 包含被调用者保存和调用者保存位的混合。)


References
(参考文献)
==========

[1] arch/arm64/include/uapi/asm/sigcontext.h
    AArch64 Linux signal ABI definitions
    (AArch64 Linux 信号 ABI 定义)

[2] arch/arm64/include/uapi/asm/ptrace.h
    AArch64 Linux ptrace ABI definitions
    (AArch64 Linux ptrace ABI 定义)

[3] Documentation/arm64/cpu-feature-registers.rst

[4] ARM IHI0055C
    http://infocenter.arm.com/help/topic/com.arm.doc.ihi0055c/IHI0055C_beta_aapcs64.pdf
    http://infocenter.arm.com/help/topic/com.arm.doc.subset.swdev.abi/index.html
    Procedure Call Standard for the ARM 64-bit Architecture (AArch64)
    (ARM 64 位架构(AArch64)的过程调用标准)
