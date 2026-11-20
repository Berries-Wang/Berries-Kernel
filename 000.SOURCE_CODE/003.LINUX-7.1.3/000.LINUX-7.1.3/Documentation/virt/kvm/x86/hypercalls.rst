.. SPDX-License-Identifier: GPL-2.0

=======================================
Linux KVM Hypercall(Linux KVM 超级调用)
=======================================

X86:
 KVM Hypercalls have a three-byte sequence of either the vmcall or the vmmcall
 instruction. The hypervisor can replace it with instructions that are
 guaranteed to be supported.

 Up to four arguments may be passed in rbx, rcx, rdx, and rsi respectively.
 The hypercall number should be placed in rax and the return value will be
 placed in rax.  No other registers will be clobbered unless explicitly stated
 by the particular hypercall.

 (X86：KVM 超级调用具有三字节序列，使用 vmcall 或 vmmcall 指令。管理程序可以用确保受支持的指令替换它。

 最多可以在 rbx、rcx、rdx 和 rsi 中分别传入四个参数。超级调用号应放入 rax，返回值也将放在 rax。除非特定超级调用明确说明，否则不会破坏任何其他寄存器。)

S390:
  R2-R7 are used for parameters 1-6. In addition, R1 is used for hypercall
  number. The return value is written to R2.

  S390 uses diagnose instruction as hypercall (0x500) along with hypercall
  number in R1.

  For further information on the S390 diagnose call as supported by KVM,
  refer to Documentation/virt/kvm/s390/s390-diag.rst.

  (S390：R2-R7 用于参数 1-6。此外，R1 用于超级调用号。返回值写入 R2。

  S390 使用 diagnose 指令 (0x500) 作为超级调用，超级调用号放在 R1 中。

  有关 KVM 支持的 S390 diagnose 调用的更多信息，请参阅 Documentation/virt/kvm/s390/s390-diag.rst。)

PowerPC:
  It uses R3-R10 and hypercall number in R11. R4-R11 are used as output registers.
  Return value is placed in R3.

  KVM hypercalls uses 4 byte opcode, that are patched with 'hypercall-instructions'
  property inside the device tree's /hypervisor node.
  For more information refer to Documentation/virt/kvm/ppc-pv.rst

  (PowerPC：使用 R3-R10，超级调用号放在 R11。R4-R11 用作输出寄存器。返回值放在 R3。

  KVM 超级调用使用 4 字节操作码，通过设备树 /hypervisor 节点内的 'hypercall-instructions' 属性进行修补。更多信息请参阅 Documentation/virt/kvm/ppc-pv.rst)

MIPS:
  KVM hypercalls use the HYPCALL instruction with code 0 and the hypercall
  number in $2 (v0). Up to four arguments may be placed in $4-$7 (a0-a3) and
  the return value is placed in $2 (v0).

  (MIPS：KVM 超级调用使用代码为 0 的 HYPCALL 指令，超级调用号放在 $2 (v0) 中。最多可在 $4-$7 (a0-a3) 中放入四个参数，返回值放在 $2 (v0) 中。)

KVM Hypercalls Documentation(KVM 超级调用文档)
==============================================

The template for each hypercall is:
1. Hypercall name.
2. Architecture(s)
3. Status (deprecated, obsolete, active)
4. Purpose
(每个超级调用的文档模板为：
1. 超级调用名称。
2. 架构。
3. 状态(已弃用、已废弃、活跃)。
4. 用途。)

1. KVM_HC_VAPIC_POLL_IRQ(VAPIC 轮询 IRQ)
----------------------------------------

:Architecture: x86
(架构： x86)
:Status: active
(状态： active)
:Purpose: Trigger guest exit so that the host can check for pending
          interrupts on reentry.
          (用途：触发虚拟机退出，使宿主机能够在重入时检查待处理的中断。)

2. KVM_HC_MMU_OP(MMU 操作)
--------------------------

:Architecture: x86
(架构： x86)
:Status: deprecated.
(状态： deprecated。)
:Purpose: Support MMU operations such as writing to PTE,
          flushing TLB, release PT.
          (用途：支持写入 PTE、刷新 TLB、释放 PT 等 MMU 操作。)

3. KVM_HC_FEATURES(特性查询)
----------------------------

:Architecture: PPC
(架构： PPC)
:Status: active
(状态： active)
:Purpose: Expose hypercall availability to the guest. On x86 platforms, cpuid
          used to enumerate which hypercalls are available. On PPC, either
	  device tree based lookup ( which is also what EPAPR dictates)
	  OR KVM specific enumeration mechanism (which is this hypercall)
	  can be used.
          (用途：向虚拟机暴露超级调用的可用性。在 x86 平台上，使用 cpuid 枚举可用的超级调用。在 PPC 上，既可以使用基于设备树的查找(这也是 EPAPR 所规定的)，也可以使用 KVM 特有的枚举机制(即本超级调用)。)

4. KVM_HC_PPC_MAP_MAGIC_PAGE(PPC 映射魔法页)
--------------------------------------------

:Architecture: PPC
(架构： PPC)
:Status: active
(状态： active)
:Purpose: To enable communication between the hypervisor and guest there is a
	  shared page that contains parts of supervisor visible register state.
	  The guest can map this shared page to access its supervisor register
	  through memory using this hypercall.
          (用途：为实现管理程序与虚拟机之间的通信，存在一个包含部分管理态可见寄存器状态的共享页。虚拟机可以使用本超级调用把这个共享页映射进来，通过内存访问其管理态寄存器。)

5. KVM_HC_KICK_CPU(唤醒 CPU)
----------------------------

:Architecture: x86
(架构： x86)
:Status: active
(状态： active)
:Purpose: Hypercall used to wakeup a vcpu from HLT state
          (用途：用于把 vcpu 从 HLT 状态唤醒的超级调用)
:Usage example:
  A vcpu of a paravirtualized guest that is busywaiting in guest
  kernel mode for an event to occur (ex: a spinlock to become available) can
  execute HLT instruction once it has busy-waited for more than a threshold
  time-interval. Execution of HLT instruction would cause the hypervisor to put
  the vcpu to sleep until occurrence of an appropriate event. Another vcpu of the
  same guest can wakeup the sleeping vcpu by issuing KVM_HC_KICK_CPU hypercall,
  specifying APIC ID (a1) of the vcpu to be woken up. An additional argument (a0)
  is used in the hypercall for future use.
  (用法示例：半虚拟化虚拟机中，在客户内核模式下忙等某个事件发生(例如等待自旋锁变为可用)的 vcpu，在忙等超过阈值时间间隔后可以执行 HLT 指令。执行 HLT 指令会使管理程序让该 vcpu 休眠，直到相应事件发生。同一虚拟机的另一个 vcpu 可以通过发出 KVM_HC_KICK_CPU 超级调用来唤醒休眠的 vcpu，并指定要唤醒的 vcpu 的 APIC ID (a1)。超级调用中的另一个参数 (a0) 留作将来使用。)


6. KVM_HC_CLOCK_PAIRING(时钟配对)
---------------------------------
:Architecture: x86
(架构： x86)
:Status: active
(状态： active)
:Purpose: Hypercall used to synchronize host and guest clocks.
          (用途：用于同步宿主机与虚拟机时钟的超级调用。)

Usage:
(用法：)

a0: guest physical address where host copies
"struct kvm_clock_offset" structure.
(a0：宿主机复制 "struct kvm_clock_offset" 结构体的虚拟机物理地址。)

a1: clock_type, ATM only KVM_CLOCK_PAIRING_WALLCLOCK (0)
is supported (corresponding to the host's CLOCK_REALTIME clock).
(a1：clock_type。目前仅支持 KVM_CLOCK_PAIRING_WALLCLOCK (0)(对应宿主机的 CLOCK_REALTIME 时钟)。)

       ::

		struct kvm_clock_pairing {
			__s64 sec;
			__s64 nsec;
			__u64 tsc;
			__u32 flags;
			__u32 pad[9];
		};

       Where:
       (其中：)
               * sec: seconds from clock_type clock.
                 (sec：来自 clock_type 时钟的秒数。)
               * nsec: nanoseconds from clock_type clock.
                 (nsec：来自 clock_type 时钟的纳秒数。)
               * tsc: guest TSC value used to calculate sec/nsec pair
                 (tsc：用于计算 sec/nsec 对的虚拟机 TSC 值)
               * flags: flags, unused (0) at the moment.
                 (flags：标志，目前未使用 (0)。)

The hypercall lets a guest compute a precise timestamp across
host and guest.  The guest can use the returned TSC value to
compute the CLOCK_REALTIME for its clock, at the same instant.
(该超级调用让虚拟机能够跨宿主机与虚拟机计算精确的时间戳。虚拟机可以使用返回的 TSC 值计算同一时刻其时钟的 CLOCK_REALTIME。)

Returns KVM_EOPNOTSUPP if the host does not use TSC clocksource,
or if clock type is different than KVM_CLOCK_PAIRING_WALLCLOCK.
(如果宿主机未使用 TSC 时钟源，或时钟类型不是 KVM_CLOCK_PAIRING_WALLCLOCK，则返回 KVM_EOPNOTSUPP。)

7. KVM_HC_SEND_IPI(发送 IPI)
----------------------------

:Architecture: x86
(架构： x86)
:Status: active
(状态： active)
:Purpose: Send IPIs to multiple vCPUs.
          (用途：向多个 vCPU 发送 IPI。)

- a0: lower part of the bitmap of destination APIC IDs
  (a0：目标 APIC ID 位图的低半部分)
- a1: higher part of the bitmap of destination APIC IDs
  (a1：目标 APIC ID 位图的高半部分)
- a2: the lowest APIC ID in bitmap
  (a2：位图中最低的 APIC ID)
- a3: APIC ICR
  (a3：APIC ICR)

The hypercall lets a guest send multicast IPIs, with at most 128
128 destinations per hypercall in 64-bit mode and 64 vCPUs per
hypercall in 32-bit mode.  The destinations are represented by a
bitmap contained in the first two arguments (a0 and a1). Bit 0 of
a0 corresponds to the APIC ID in the third argument (a2), bit 1
corresponds to the APIC ID a2+1, and so on.
(该超级调用让虚拟机发送多播 IPI：在 64 位模式下每次超级调用最多 128 个目标，在 32 位模式下每次最多 64 个 vCPU。目标由前两个参数 (a0 和 a1) 中的位图表示。a0 的位 0 对应第三个参数 (a2) 中的 APIC ID，位 1 对应 APIC ID a2+1，依此类推。)

Returns the number of CPUs to which the IPIs were delivered successfully.
(返回 IPI 成功送达的 CPU 数量。)

8. KVM_HC_SCHED_YIELD(调度让出)
-------------------------------

:Architecture: x86
(架构： x86)
:Status: active
(状态： active)
:Purpose: Hypercall used to yield if the IPI target vCPU is preempted
          (用途：当 IPI 目标 vCPU 被抢占时用于让出 CPU 的超级调用)

a0: destination APIC ID
(a0：目标 APIC ID)

:Usage example: When sending a call-function IPI-many to vCPUs, yield if
	        any of the IPI target vCPUs was preempted.
	        (用法示例：向多个 vCPU 发送 call-function IPI-many 时，如果任一 IPI 目标 vCPU 已被抢占，则让出 CPU。)

9. KVM_HC_MAP_GPA_RANGE(映射 GPA 范围)
--------------------------------------
:Architecture: x86
(架构： x86)
:Status: active
(状态： active)
:Purpose: Request KVM to map a GPA range with the specified attributes.
          (用途：请求 KVM 以指定属性映射一段 GPA 范围。)

a0: the guest physical address of the start page
(a0：起始页的虚拟机物理地址)
a1: the number of (4kb) pages (must be contiguous in GPA space)
(a1：(4kb) 页数(在 GPA 空间中必须连续))
a2: attributes
(a2：属性)

    Where 'attributes' :
        * bits  3:0 - preferred page size encoding 0 = 4kb, 1 = 2mb, 2 = 1gb, etc...
        * bit     4 - plaintext = 0, encrypted = 1
        * bits 63:5 - reserved (must be zero)

    (其中 'attributes' ：
        * 位 3:0 - 首选页大小编码，0 = 4kb，1 = 2mb，2 = 1gb 等...
        * 位 4 - 明文 = 0，加密 = 1
        * 位 63:5 - 保留(必须为零))

**Implementation note**: this hypercall is implemented in userspace via
the KVM_CAP_EXIT_HYPERCALL capability. Userspace must enable that capability
before advertising KVM_FEATURE_HC_MAP_GPA_RANGE in the guest CPUID.  In
addition, if the guest supports KVM_FEATURE_MIGRATION_CONTROL, userspace
must also set up an MSR filter to process writes to MSR_KVM_MIGRATION_CONTROL.
(**实现说明**：该超级调用在用户空间通过 KVM_CAP_EXIT_HYPERCALL 能力实现。在虚拟机 CPUID 中公告 KVM_FEATURE_HC_MAP_GPA_RANGE 之前，用户空间必须启用该能力。此外，如果虚拟机支持 KVM_FEATURE_MIGRATION_CONTROL，用户空间还必须设置一个 MSR 过滤器来处理对 MSR_KVM_MIGRATION_CONTROL 的写入。)
