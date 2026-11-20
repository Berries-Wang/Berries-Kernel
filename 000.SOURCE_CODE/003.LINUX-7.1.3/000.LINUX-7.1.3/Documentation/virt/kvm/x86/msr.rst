.. SPDX-License-Identifier: GPL-2.0

===============================
KVM-specific MSRs(KVM 专有 MSR)
===============================

:Author: Glauber Costa <glommer@redhat.com>, Red Hat Inc, 2010
(作者: Glauber Costa <glommer@redhat.com>，Red Hat Inc，2010)

KVM makes use of some custom MSRs to service some requests.
(KVM 使用一些自定义 MSR 来满足某些请求。)

Custom MSRs have a range reserved for them, that goes from
0x4b564d00 to 0x4b564dff. There are MSRs outside this area,
but they are deprecated and their use is discouraged.
(自定义 MSR 保留了一个范围，从 0x4b564d00 到 0x4b564dff。该范围之外也有一些 MSR，但它们已废弃，不鼓励使用。)

Custom MSR list(自定义 MSR 列表)
--------------------------------

The current supported Custom MSR list is:
(当前支持的自定义 MSR 列表如下：)

MSR_KVM_WALL_CLOCK_NEW:
	0x4b564d00

data:
	4-byte alignment physical address of a memory area which must be
	in guest RAM. This memory is expected to hold a copy of the following
	structure::

	 struct pvclock_wall_clock {
		u32   version;
		u32   sec;
		u32   nsec;
	  } __attribute__((__packed__));

	(4 字节对齐的物理地址，指向一个必须位于客户机 RAM 中的内存区域。这块内存预期保存如下结构的副本：)

	whose data will be filled in by the hypervisor. The hypervisor is only
	guaranteed to update this data at the moment of MSR write.
	Users that want to reliably query this information more than once have
	to write more than once to this MSR. Fields have the following meanings:
	(其数据将由 hypervisor 填写。hypervisor 只保证在写 MSR 的那一刻更新这些数据。想要可靠地多次查询该信息的用户，必须对这个 MSR 写入多次。各字段含义如下：)

	version:
		guest has to check version before and after grabbing
		time information and check that they are both equal and even.
		An odd version indicates an in-progress update.
		(客户机必须在抓取时间信息前后分别检查 version，并确认两者相等且为偶数。奇数的 version 表示更新正在进行中。)

	sec:
		 number of seconds for wallclock at time of boot.
		 (引导时刻墙钟时间的秒数。)

	nsec:
		 number of nanoseconds for wallclock at time of boot.
		 (引导时刻墙钟时间的纳秒数。)

	In order to get the current wallclock time, the system_time from
	MSR_KVM_SYSTEM_TIME_NEW needs to be added.
	(要得到当前的墙钟时间，需要加上 MSR_KVM_SYSTEM_TIME_NEW 中的 system_time。)

	Note that although MSRs are per-CPU entities, the effect of this
	particular MSR is global.
	(注意，虽然 MSR 是每 CPU 的实体，但这个特定 MSR 的效果是全局的。)

	Availability of this MSR must be checked via bit 3 in 0x4000001 cpuid
	leaf prior to usage.
	(使用前必须通过 0x4000001 CPUID 叶的位 3 检查该 MSR 的可用性。)

MSR_KVM_SYSTEM_TIME_NEW:
	0x4b564d01

data:
	4-byte aligned physical address of a memory area which must be in
	guest RAM, plus an enable bit in bit 0. This memory is expected to hold
	a copy of the following structure::

	  struct pvclock_vcpu_time_info {
		u32   version;
		u32   pad0;
		u64   tsc_timestamp;
		u64   system_time;
		u32   tsc_to_system_mul;
		s8    tsc_shift;
		u8    flags;
		u8    pad[2];
	  } __attribute__((__packed__)); /* 32 bytes */

	(4 字节对齐的物理地址，指向一个必须位于客户机 RAM 中的内存区域，外加位 0 处的一个使能位。这块内存预期保存如下结构的副本：)

	whose data will be filled in by the hypervisor periodically. Only one
	write, or registration, is needed for each VCPU. The interval between
	updates of this structure is arbitrary and implementation-dependent.
	The hypervisor may update this structure at any time it sees fit until
	anything with bit0 == 0 is written to it.
	(其数据将由 hypervisor 周期性地填写。每个 VCPU 只需写入一次，即注册一次。该结构两次更新之间的间隔是任意的、取决于实现。在写入任何 bit0 == 0 的值之前，hypervisor 可以在它认为合适的任何时候更新该结构。)

	Fields have the following meanings:
	(各字段含义如下：)

	version:
		guest has to check version before and after grabbing
		time information and check that they are both equal and even.
		An odd version indicates an in-progress update.
		(客户机必须在抓取时间信息前后分别检查 version，并确认两者相等且为偶数。奇数的 version 表示更新正在进行中。)

	tsc_timestamp:
		the tsc value at the current VCPU at the time
		of the update of this structure. Guests can subtract this value
		from current tsc to derive a notion of elapsed time since the
		structure update.
		(更新该结构时当前 VCPU 的 TSC 值。客户机可以用当前 TSC 减去该值，得到自该结构更新以来经过的时间。)

	system_time:
		a host notion of monotonic time, including sleep
		time at the time this structure was last updated. Unit is
		nanoseconds.
		(该结构最后一次更新时，宿主机所理解的单调时间，含休眠时间。单位为纳秒。)

	tsc_to_system_mul:
		multiplier to be used when converting
		tsc-related quantity to nanoseconds
		(把 TSC 相关量转换为纳秒时使用的乘数。)

	tsc_shift:
		shift to be used when converting tsc-related
		quantity to nanoseconds. This shift will ensure that
		multiplication with tsc_to_system_mul does not overflow.
		A positive value denotes a left shift, a negative value
		a right shift.
		(把 TSC 相关量转换为纳秒时使用的移位。该移位保证与 tsc_to_system_mul 相乘不会溢出。正值表示左移，负值表示右移。)

		The conversion from tsc to nanoseconds involves an additional
		right shift by 32 bits. With this information, guests can
		derive per-CPU time by doing::

			time = (current_tsc - tsc_timestamp)
			if (tsc_shift >= 0)
				time <<= tsc_shift;
			else
				time >>= -tsc_shift;
			time = (time * tsc_to_system_mul) >> 32
			time = time + system_time

		(从 TSC 到纳秒的转换还包含一次额外的右移 32 位。借助这些信息，客户机可以按下面的方法计算每 CPU 时间：)

	flags:
		bits in this field indicate extended capabilities
		coordinated between the guest and the hypervisor. Availability
		of specific flags has to be checked in 0x40000001 cpuid leaf.
		Current flags are:
		(该字段中的位表示客户机与 hypervisor 之间协调的扩展能力。特定标志的可用性必须在 0x40000001 CPUID 叶中检查。当前的标志有：)


		+-----------+--------------+----------------------------------+
		| flag bit  | cpuid bit    | meaning			      |
		| (标志位)  | (CPUID 位)   | (含义)                           |
		+-----------+--------------+----------------------------------+
		|	    |		   | time measures taken across       |
		|    0      |	   24      | multiple cpus are guaranteed to  |
		|	    |		   | be monotonic		      |
		|	    |		   | (跨多个 CPU 取得的时间度量保证单 |
		|	    |		   |   调)                            |
		+-----------+--------------+----------------------------------+
		|	    |		   | guest vcpu has been paused by    |
		|    1	    |	  N/A	   | the host			      |
		|	    |		   | See 4.70 in api.txt	      |
		|	    |		   | (客户机 vcpu 已被宿主机暂停，参  |
		|	    |		   |   见 api.txt 中的 4.70 节)       |
		+-----------+--------------+----------------------------------+

	Availability of this MSR must be checked via bit 3 in 0x4000001 cpuid
	leaf prior to usage.
	(使用前必须通过 0x4000001 CPUID 叶的位 3 检查该 MSR 的可用性。)


MSR_KVM_WALL_CLOCK:
	0x11

data and functioning:
	same as MSR_KVM_WALL_CLOCK_NEW. Use that instead.
	(数据与功能同 MSR_KVM_WALL_CLOCK_NEW。请改用后者。)

	This MSR falls outside the reserved KVM range and may be removed in the
	future. Its usage is deprecated.
	(该 MSR 不在保留的 KVM 范围之内，将来可能被移除。其用法已废弃。)

	Availability of this MSR must be checked via bit 0 in 0x4000001 cpuid
	leaf prior to usage.
	(使用前必须通过 0x4000001 CPUID 叶的位 0 检查该 MSR 的可用性。)

MSR_KVM_SYSTEM_TIME:
	0x12

data and functioning:
	same as MSR_KVM_SYSTEM_TIME_NEW. Use that instead.
	(数据与功能同 MSR_KVM_SYSTEM_TIME_NEW。请改用后者。)

	This MSR falls outside the reserved KVM range and may be removed in the
	future. Its usage is deprecated.
	(该 MSR 不在保留的 KVM 范围之内，将来可能被移除。其用法已废弃。)

	Availability of this MSR must be checked via bit 0 in 0x4000001 cpuid
	leaf prior to usage.
	(使用前必须通过 0x4000001 CPUID 叶的位 0 检查该 MSR 的可用性。)

	The suggested algorithm for detecting kvmclock presence is then::

		if (!kvm_para_available())    /* refer to cpuid.txt */
			return NON_PRESENT;

		flags = cpuid_eax(0x40000001);
		if (flags & 3) {
			msr_kvm_system_time = MSR_KVM_SYSTEM_TIME_NEW;
			msr_kvm_wall_clock = MSR_KVM_WALL_CLOCK_NEW;
			return PRESENT;
		} else if (flags & 0) {
			msr_kvm_system_time = MSR_KVM_SYSTEM_TIME;
			msr_kvm_wall_clock = MSR_KVM_WALL_CLOCK;
			return PRESENT;
		} else
			return NON_PRESENT;

	(于是，检测 kvmclock 是否存在的建议算法如下：)

MSR_KVM_ASYNC_PF_EN:
	0x4b564d02

data:
	Asynchronous page fault (APF) control MSR.
	(异步缺页 (Asynchronous Page Fault，APF) 控制 MSR。)

	Bits 63-6 hold 64-byte aligned physical address of a 64 byte memory area
	which must be in guest RAM. This memory is expected to hold the
	following structure::

	  struct kvm_vcpu_pv_apf_data {
		/* Used for 'page not present' events delivered via #PF */
		__u32 flags;

		/* Used for 'page ready' events delivered via interrupt notification */
		__u32 token;

		__u8 pad[56];
	  };

	(位 63-6 保存 64 字节对齐的物理地址，指向一个必须位于客户机 RAM 中的 64 字节内存区域。这块内存预期保存如下结构：)

	Bits 5-4 of the MSR are reserved and should be zero. Bit 0 is set to 1
	when asynchronous page faults are enabled on the vcpu, 0 when disabled.
	Bit 1 is 1 if asynchronous page faults can be injected when vcpu is in
	cpl == 0. Bit 2 is 1 if asynchronous page faults are delivered to L1 as
	#PF vmexits.  Bit 2 can be set only if KVM_FEATURE_ASYNC_PF_VMEXIT is
	present in CPUID. Bit 3 enables interrupt based delivery of 'page ready'
	events. Bit 3 can only be set if KVM_FEATURE_ASYNC_PF_INT is present in
	CPUID.
	(该 MSR 的位 5-4 保留，应为 0。位 0 在 vcpu 上启用异步缺页时置 1，禁用时为 0。如果 vcpu 处于 cpl == 0 时也可以注入异步缺页，则位 1 为 1。如果异步缺页以 #PF vmexit 的形式递交到 L1，则位 2 为 1。只有 CPUID 中存在 KVM_FEATURE_ASYNC_PF_VMEXIT 时才能设置位 2。位 3 启用基于中断的 'page ready' 事件递交。只有 CPUID 中存在 KVM_FEATURE_ASYNC_PF_INT 时才能设置位 3。)

	'Page not present' events are currently always delivered as synthetic
	#PF exception. During delivery of these events APF CR2 register contains
	a token that will be used to notify the guest when missing page becomes
	available. Also, to make it possible to distinguish between real #PF and
	APF, first 4 bytes of 64 byte memory location ('flags') will be written
	to by the hypervisor at the time of injection. Only first bit of 'flags'
	is currently supported, when set, it indicates that the guest is dealing
	with asynchronous 'page not present' event. If during a page fault APF
	'flags' is '0' it means that this is regular page fault. Guest is
	supposed to clear 'flags' when it is done handling #PF exception so the
	next event can be delivered.
	('page not present' 事件目前总是以合成 #PF 异常的形式递交。递交这些事件期间，APF 的 CR2 寄存器包含一个令牌 (token)，当缺失的页变为可用时将用它通知客户机。此外，为了能够区分真实 #PF 与 APF，注入时 hypervisor 会写入 64 字节内存位置的前 4 字节('flags')。目前只支持 'flags' 的第一位，该位置位表示客户机正在处理异步 'page not present' 事件。如果缺页期间 APF 的 'flags' 为 '0'，则说明这是普通缺页。客户机处理完 #PF 异常后应清除 'flags'，以便递交下一个事件。)

	Note, since APF 'page not present' events use the same exception vector
	as regular page fault, guest must reset 'flags' to '0' before it does
	something that can generate normal page fault.
	(注意，由于 APF 的 'page not present' 事件与普通缺页使用同一个异常向量，客户机在做任何可能产生普通缺页的事情之前，必须先把 'flags' 重置为 '0'。)

	Bytes 4-7 of 64 byte memory location ('token') will be written to by the
	hypervisor at the time of APF 'page ready' event injection. The content
	of these bytes is a token which was previously delivered in CR2 as
	'page not present' event. The event indicates the page is now available.
	Guest is supposed to write '0' to 'token' when it is done handling
	'page ready' event and to write '1' to MSR_KVM_ASYNC_PF_ACK after
	clearing the location; writing to the MSR forces KVM to re-scan its
	queue and deliver the next pending notification.
	(64 字节内存位置的第 4-7 字节('token')会在 APF 'page ready' 事件注入时由 hypervisor 写入。这些字节的内容是一个令牌，即此前作为 'page not present' 事件在 CR2 中递交的那个。该事件表明页现已可用。客户机处理完 'page ready' 事件后应向 'token' 写入 '0'，并在清除该位置后向 MSR_KVM_ASYNC_PF_ACK 写入 '1'；写该 MSR 会迫使 KVM 重新扫描其队列并递交下一个待处理通知。)

	Note, MSR_KVM_ASYNC_PF_INT MSR specifying the interrupt vector for 'page
	ready' APF delivery needs to be written to before enabling APF mechanism
	in MSR_KVM_ASYNC_PF_EN or interrupt #0 can get injected. The MSR is
	available if KVM_FEATURE_ASYNC_PF_INT is present in CPUID.
	(注意，在 MSR_KVM_ASYNC_PF_EN 中启用 APF 机制之前，必须先写入指定 'page ready' APF 递交所用中断向量的 MSR_KVM_ASYNC_PF_INT，否则可能注入 0 号中断。只有 CPUID 中存在 KVM_FEATURE_ASYNC_PF_INT 时该 MSR 才可用。)

	Note, previously, 'page ready' events were delivered via the same #PF
	exception as 'page not present' events but this is now deprecated. If
	bit 3 (interrupt based delivery) is not set APF events are not delivered.
	(注意，以前 'page ready' 事件与 'page not present' 事件经由同一个 #PF 异常递交，但这现在已废弃。如果位 3(基于中断的递交)未置位，则 APF 事件不会被递交。)

	If APF is disabled while there are outstanding APFs, they will
	not be delivered.
	(如果在还有未处理 APF 的情况下禁用 APF，这些 APF 将不会被递交。)

	Currently 'page ready' APF events will be always delivered on the
	same vcpu as 'page not present' event was, but guest should not rely on
	that.
	(目前 'page ready' APF 事件总会在与 'page not present' 事件相同的 vcpu 上递交，但客户机不应依赖这一点。)

MSR_KVM_STEAL_TIME:
	0x4b564d03

data:
	64-byte alignment physical address of a memory area which must be
	in guest RAM, plus an enable bit in bit 0. This memory is expected to
	hold a copy of the following structure::

	  struct kvm_steal_time {
		__u64 steal;
		__u32 version;
		__u32 flags;
		__u8  preempted;
		__u8  u8_pad[3];
		__u32 pad[11];
	  }

	(64 字节对齐的物理地址，指向一个必须位于客户机 RAM 中的内存区域，外加位 0 处的一个使能位。这块内存预期保存如下结构的副本：)

	whose data will be filled in by the hypervisor periodically. Only one
	write, or registration, is needed for each VCPU. The interval between
	updates of this structure is arbitrary and implementation-dependent.
	The hypervisor may update this structure at any time it sees fit until
	anything with bit0 == 0 is written to it. Guest is required to make sure
	this structure is initialized to zero.
	(其数据将由 hypervisor 周期性地填写。每个 VCPU 只需写入一次，即注册一次。该结构两次更新之间的间隔是任意的、取决于实现。在写入任何 bit0 == 0 的值之前，hypervisor 可以在它认为合适的任何时候更新该结构。客户机必须确保该结构被初始化为零。)

	Fields have the following meanings:
	(各字段含义如下：)

	version:
		a sequence counter. In other words, guest has to check
		this field before and after grabbing time information and make
		sure they are both equal and even. An odd version indicates an
		in-progress update.
		(一个序列计数器。换言之，客户机必须在抓取时间信息前后分别检查该字段，并确认两者相等且为偶数。奇数的 version 表示更新正在进行中。)

	flags:
		At this point, always zero. May be used to indicate
		changes in this structure in the future.
		(目前始终为零。将来可能用于指示该结构的变更。)

	steal:
		the amount of time in which this vCPU did not run, in
		nanoseconds. Time during which the vcpu is idle, will not be
		reported as steal time.
		(该 vCPU 未运行的时间总量，单位纳秒。vcpu 空闲期间的时间不会计为被窃取时间 (steal time)。)

	preempted:
		indicate the vCPU who owns this struct is running or
		not. Non-zero values mean the vCPU has been preempted. Zero
		means the vCPU is not preempted. NOTE, it is always zero if the
		the hypervisor doesn't support this field.
		(指示拥有该结构的 vCPU 是否正在运行。非零值表示该 vCPU 已被抢占；零表示未被抢占。注意，如果 hypervisor 不支持该字段，则它始终为零。)

MSR_KVM_EOI_EN:
	0x4b564d04

data:
	Bit 0 is 1 when PV end of interrupt is enabled on the vcpu; 0
	when disabled.  Bit 1 is reserved and must be zero.  When PV end of
	interrupt is enabled (bit 0 set), bits 63-2 hold a 4-byte aligned
	physical address of a 4 byte memory area which must be in guest RAM and
	must be zeroed.
	(位 0 在 vcpu 上启用 PV 中断结束 (PV end of interrupt) 时为 1，禁用时为 0。位 1 保留，必须为零。当 PV 中断结束启用(位 0 置位)时，位 63-2 保存 4 字节对齐的物理地址，指向一个必须位于客户机 RAM 中且必须清零的 4 字节内存区域。)

	The first, least significant bit of 4 byte memory location will be
	written to by the hypervisor, typically at the time of interrupt
	injection.  Value of 1 means that guest can skip writing EOI to the apic
	(using MSR or MMIO write); instead, it is sufficient to signal
	EOI by clearing the bit in guest memory - this location will
	later be polled by the hypervisor.
	Value of 0 means that the EOI write is required.
	(4 字节内存位置的最低有效位将由 hypervisor 写入，通常发生在注入中断时。值为 1 表示客户机可以跳过向 APIC 写 EOI(通过 MSR 或 MMIO 写)；改为通过清除客户机内存中的该位来发出 EOI 信号即可——hypervisor 稍后会轮询该位置。值为 0 表示仍需要写 EOI。)

	It is always safe for the guest to ignore the optimization and perform
	the APIC EOI write anyway.
	(对客户机而言，忽略这一优化、照常执行 APIC EOI 写入永远是安全的。)

	Hypervisor is guaranteed to only modify this least
	significant bit while in the current VCPU context, this means that
	guest does not need to use either lock prefix or memory ordering
	primitives to synchronise with the hypervisor.
	(可以保证 hypervisor 只会在当前 VCPU 上下文中修改这个最低有效位，这意味着客户机不需要使用锁前缀或内存序原语来与 hypervisor 同步。)

	However, hypervisor can set and clear this memory bit at any time:
	therefore to make sure hypervisor does not interrupt the
	guest and clear the least significant bit in the memory area
	in the window between guest testing it to detect
	whether it can skip EOI apic write and between guest
	clearing it to signal EOI to the hypervisor,
	guest must both read the least significant bit in the memory area and
	clear it using a single CPU instruction, such as test and clear, or
	compare and exchange.
	(不过，hypervisor 可以在任何时候设置和清除这个内存位：因此，为了保证 hypervisor 不会在客户机测试该位以判断能否跳过 APIC EOI 写、与客户机清除该位以向 hypervisor 发出 EOI 信号之间的窗口里打断客户机并清除内存区域中的最低有效位，客户机必须用单条 CPU 指令同时完成对该最低有效位的读取与清除，例如 test and clear 或 compare and exchange。)

MSR_KVM_POLL_CONTROL:
	0x4b564d05

	Control host-side polling.
	(控制宿主机侧的轮询。)

data:
	Bit 0 enables (1) or disables (0) host-side HLT polling logic.
	(位 0 启用 (1) 或禁用 (0) 宿主机侧的 HLT 轮询逻辑。)

	KVM guests can request the host not to poll on HLT, for example if
	they are performing polling themselves.
	(KVM 客户机可以请求宿主机不要在 HLT 上轮询，例如当它们自己在执行轮询时。)

MSR_KVM_ASYNC_PF_INT:
	0x4b564d06

data:
	Second asynchronous page fault (APF) control MSR.
	(第二个异步缺页 (APF) 控制 MSR。)

	Bits 0-7: APIC vector for delivery of 'page ready' APF events.
	Bits 8-63: Reserved

	Interrupt vector for asynchnonous 'page ready' notifications delivery.
	The vector has to be set up before asynchronous page fault mechanism
	is enabled in MSR_KVM_ASYNC_PF_EN.  The MSR is only available if
	KVM_FEATURE_ASYNC_PF_INT is present in CPUID.
	(位 0-7：递交 'page ready' APF 事件的 APIC 向量。位 8-63：保留。

	用于异步递交 'page ready' 通知的中断向量。必须在 MSR_KVM_ASYNC_PF_EN 中启用异步缺页机制之前设置好该向量。只有 CPUID 中存在 KVM_FEATURE_ASYNC_PF_INT 时该 MSR 才可用。)

MSR_KVM_ASYNC_PF_ACK:
	0x4b564d07

data:
	Asynchronous page fault (APF) acknowledgment.
	(异步缺页 (APF) 确认。)

	When the guest is done processing 'page ready' APF event and 'token'
	field in 'struct kvm_vcpu_pv_apf_data' is cleared it is supposed to
	write '1' to bit 0 of the MSR, this causes the host to re-scan its queue
	and check if there are more notifications pending. The MSR is available
	if KVM_FEATURE_ASYNC_PF_INT is present in CPUID.
	(当客户机处理完 'page ready' APF 事件并清除 'struct kvm_vcpu_pv_apf_data' 中的 'token' 字段后，应向该 MSR 的位 0 写入 '1'，这会使宿主机重新扫描其队列并检查是否还有待递交的通知。只有 CPUID 中存在 KVM_FEATURE_ASYNC_PF_INT 时该 MSR 才可用。)

MSR_KVM_MIGRATION_CONTROL:
        0x4b564d08

data:
        This MSR is available if KVM_FEATURE_MIGRATION_CONTROL is present in
        CPUID.  Bit 0 represents whether live migration of the guest is allowed.
        (只有 CPUID 中存在 KVM_FEATURE_MIGRATION_CONTROL 时该 MSR 才可用。位 0 表示是否允许对客户机做热迁移 (live migration)。)

        When a guest is started, bit 0 will be 0 if the guest has encrypted
        memory and 1 if the guest does not have encrypted memory.  If the
        guest is communicating page encryption status to the host using the
        ``KVM_HC_MAP_GPA_RANGE`` hypercall, it can set bit 0 in this MSR to
        allow live migration of the guest.
        (客户机启动时，若其内存已加密则位 0 为 0，未加密则为 1。如果客户机正通过 ``KVM_HC_MAP_GPA_RANGE`` 超级调用向宿主机通报页加密状态，则它可以置位本 MSR 的位 0 以允许对该客户机做热迁移。)
