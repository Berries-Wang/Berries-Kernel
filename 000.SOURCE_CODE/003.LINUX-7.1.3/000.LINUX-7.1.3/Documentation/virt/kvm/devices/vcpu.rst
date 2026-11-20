.. SPDX-License-Identifier: GPL-2.0

======================================
Generic vcpu interface(通用 vcpu 接口)
======================================

The virtual cpu "device" also accepts the ioctls KVM_SET_DEVICE_ATTR,
KVM_GET_DEVICE_ATTR, and KVM_HAS_DEVICE_ATTR. The interface uses the same struct
kvm_device_attr as other devices, but targets VCPU-wide settings and controls.
(虚拟 CPU "设备" 也接受 KVM_SET_DEVICE_ATTR、KVM_GET_DEVICE_ATTR 和
KVM_HAS_DEVICE_ATTR 这几个 ioctl。该接口使用与其他设备相同的 struct
kvm_device_attr，但面向的是 VCPU 范围的设置与控制。)

The groups and attributes per virtual cpu, if any, are architecture specific.
(每个虚拟 CPU 的组与属性 (如果有的话) 取决于具体架构。)

1. GROUP: KVM_ARM_VCPU_PMU_V3_CTRL(1. 组：KVM_ARM_VCPU_PMU_V3_CTRL)
===================================================================

:Architectures: ARM64
(架构：ARM64)

1.1. ATTRIBUTE: KVM_ARM_VCPU_PMU_V3_IRQ(1.1. 属性：KVM_ARM_VCPU_PMU_V3_IRQ)
---------------------------------------------------------------------------

:Parameters: in kvm_device_attr.addr the address for PMU overflow interrupt is a
	     pointer to an int
(参数：kvm_device_attr.addr 中是 PMU 溢出中断的地址，为一个指向 int 的
指针。)

Returns:(返回值：)

	 =======  ========================================================
	 -EBUSY   The PMU overflow interrupt is already set (PMU 溢出中断已被设置)
	 -EFAULT  Error reading interrupt number (读取中断号出错)
	 -ENXIO   PMUv3 not supported or the overflow interrupt not set
		  when attempting to get it (不支持 PMUv3，或尝试获取时溢出中断未设置)
	 -ENODEV  KVM_ARM_VCPU_PMU_V3 feature missing from VCPU (VCPU 缺少 KVM_ARM_VCPU_PMU_V3 特性)
	 -EINVAL  Invalid PMU overflow interrupt number supplied or
		  trying to set the IRQ number without using an in-kernel
		  irqchip. (提供的 PMU 溢出中断号无效，或未使用内核内 irqchip 就试图设置 IRQ 号)
	 =======  ========================================================

A value describing the PMUv3 (Performance Monitor Unit v3) overflow interrupt
number for this vcpu. This interrupt could be a PPI or SPI, but the interrupt
type must be same for each vcpu. As a PPI, the interrupt number is the same for
all vcpus, while as an SPI it must be a separate number per vcpu. For
GICv5-based guests, the architected PPI (23) must be used.
(一个描述此 vcpu 的 PMUv3 (Performance Monitor Unit v3，性能监控单元 v3)
溢出中断号的值。该中断可以是 PPI 或 SPI，但对每个 vcpu 而言中断类型必须
相同。作为 PPI 时，所有 vcpu 的中断号都相同；而作为 SPI 时，每个 vcpu 必须
使用各自独立的中断号。对于基于 GICv5 的客户机，必须使用架构规定的 PPI
(23)。)

1.2 ATTRIBUTE: KVM_ARM_VCPU_PMU_V3_INIT(1.2 属性：KVM_ARM_VCPU_PMU_V3_INIT)
---------------------------------------------------------------------------

:Parameters: no additional parameter in kvm_device_attr.addr
(参数：kvm_device_attr.addr 中没有额外参数。)

Returns:(返回值：)

	 =======  ======================================================
	 -EEXIST  Interrupt number already used (中断号已被使用)
	 -ENODEV  PMUv3 not supported or GIC not initialized (不支持 PMUv3 或 GIC 未初始化)
	 -ENXIO   PMUv3 not supported, missing VCPU feature or interrupt
		  number not set (non-GICv5 guests, only) (不支持 PMUv3、缺少 VCPU 特性或中断号未设置 (仅非 GICv5 客户机))
	 -EBUSY   PMUv3 already initialized (PMUv3 已初始化)
	 =======  ======================================================

Request the initialization of the PMUv3.  If using the PMUv3 with an in-kernel
virtual GIC implementation, this must be done after initializing the in-kernel
irqchip.
(请求初始化 PMUv3。如果将 PMUv3 与内核内虚拟 GIC 实现一起使用，则必须在
初始化内核内 irqchip 之后再执行此操作。)

1.3 ATTRIBUTE: KVM_ARM_VCPU_PMU_V3_FILTER(1.3 属性：KVM_ARM_VCPU_PMU_V3_FILTER)
-------------------------------------------------------------------------------

:Parameters: in kvm_device_attr.addr the address for a PMU event filter is a
             pointer to a struct kvm_pmu_event_filter
(参数：kvm_device_attr.addr 中是 PMU 事件过滤器的地址，为一个指向 struct
kvm_pmu_event_filter 的指针。)

:Returns:(返回值：)

	 =======  ======================================================
	 -ENODEV  PMUv3 not supported or GIC not initialized (不支持 PMUv3 或 GIC 未初始化)
	 -ENXIO   PMUv3 not properly configured or in-kernel irqchip not
	 	  configured as required prior to calling this attribute (PMUv3 配置不正确，或调用此属性之前内核内 irqchip 未按要求配置)
	 -EBUSY   PMUv3 already initialized or a VCPU has already run (PMUv3 已初始化或某个 VCPU 已经运行)
	 -EINVAL  Invalid filter range (过滤器范围无效)
	 =======  ======================================================

Request the installation of a PMU event filter described as follows::

    struct kvm_pmu_event_filter {
	    __u16	base_event;
	    __u16	nevents;

    #define KVM_PMU_EVENT_ALLOW	0
    #define KVM_PMU_EVENT_DENY	1

	    __u8	action;
	    __u8	pad[3];
    };

(请求安装一个如下所述的 PMU 事件过滤器：)

A filter range is defined as the range [@base_event, @base_event + @nevents),
together with an @action (KVM_PMU_EVENT_ALLOW or KVM_PMU_EVENT_DENY). The
first registered range defines the global policy (global ALLOW if the first
@action is DENY, global DENY if the first @action is ALLOW). Multiple ranges
can be programmed, and must fit within the event space defined by the PMU
architecture (10 bits on ARMv8.0, 16 bits from ARMv8.1 onwards).
(过滤器范围定义为区间 [@base_event, @base_event + @nevents)，外加一个
@action (KVM_PMU_EVENT_ALLOW 或 KVM_PMU_EVENT_DENY)。第一个注册的范围定义
全局策略 (若第一个 @action 为 DENY 则全局 ALLOW；若第一个 @action 为 ALLOW
则全局 DENY)。可以编程多个范围，且必须落在 PMU 架构定义的事件空间内
(ARMv8.0 为 10 位，从 ARMv8.1 起为 16 位)。)

Note: "Cancelling" a filter by registering the opposite action for the same
range doesn't change the default action. For example, installing an ALLOW
filter for event range [0:10) as the first filter and then applying a DENY
action for the same range will leave the whole range as disabled.
(注意：通过为同一范围注册相反的动作来 "取消" 过滤器并不会改变默认动作。
例如，先为事件范围 [0:10) 安装 ALLOW 过滤器作为第一个过滤器，然后又对同一
范围应用 DENY 动作，将使整个范围保持禁用状态。)

Restrictions: Event 0 (SW_INCR) is never filtered, as it doesn't count a
hardware event. Filtering event 0x1E (CHAIN) has no effect either, as it
isn't strictly speaking an event. Filtering the cycle counter is possible
using event 0x11 (CPU_CYCLES).
(限制：事件 0 (SW_INCR) 永远不会被过滤，因为它并不统计硬件事件。过滤事件
0x1E (CHAIN) 也没有效果，因为严格来说它不是一个事件。可以使用事件 0x11
(CPU_CYCLES) 来过滤周期计数器。)

1.4 ATTRIBUTE: KVM_ARM_VCPU_PMU_V3_SET_PMU(1.4 属性：KVM_ARM_VCPU_PMU_V3_SET_PMU)
---------------------------------------------------------------------------------

:Parameters: in kvm_device_attr.addr the address to an int representing the PMU
             identifier.
(参数：kvm_device_attr.addr 中是指向一个 int 的地址，该 int 表示 PMU
标识符。)

:Returns:(返回值：)

	 =======  ====================================================
	 -EBUSY   PMUv3 already initialized, a VCPU has already run or
                  an event filter has already been set (PMUv3 已初始化、VCPU 已运行或事件过滤器已被设置)
	 -EFAULT  Error accessing the PMU identifier (访问 PMU 标识符出错)
	 -ENXIO   PMU not found (未找到 PMU)
	 -ENODEV  PMUv3 not supported or GIC not initialized (不支持 PMUv3 或 GIC 未初始化)
	 -ENOMEM  Could not allocate memory (无法分配内存)
	 =======  ====================================================

Request that the VCPU uses the specified hardware PMU when creating guest events
for the purpose of PMU emulation. The PMU identifier can be read from the "type"
file for the desired PMU instance under /sys/devices (or, equivalent,
/sys/bus/even_source). This attribute is particularly useful on heterogeneous
systems where there are at least two CPU PMUs on the system. The PMU that is set
for one VCPU will be used by all the other VCPUs. It isn't possible to set a PMU
if a PMU event filter is already present.
(请求 VCPU 在为模拟 PMU 而创建客户机事件时使用指定的硬件 PMU。PMU 标识符
可以从 /sys/devices 下目标 PMU 实例的 "type" 文件读取 (等价地，
/sys/bus/even_source)。在系统上至少存在两个 CPU PMU 的异构系统上，该属性
尤其有用。为一个 VCPU 设置的 PMU 将被所有其他 VCPU 使用。如果 PMU 事件
过滤器已经存在，则无法设置 PMU。)

Note that KVM will not make any attempts to run the VCPU on the physical CPUs
associated with the PMU specified by this attribute. This is entirely left to
userspace. However, attempting to run the VCPU on a physical CPU not supported
by the PMU will fail and KVM_RUN will return with
exit_reason = KVM_EXIT_FAIL_ENTRY and populate the fail_entry struct by setting
hardare_entry_failure_reason field to KVM_EXIT_FAIL_ENTRY_CPU_UNSUPPORTED and
the cpu field to the processor id.
(注意，KVM 不会做任何让 VCPU 运行在与该属性指定的 PMU 相关联的物理 CPU 上
的尝试；这完全交由用户态处理。不过，试图在不被该 PMU 支持的物理 CPU 上
运行 VCPU 将会失败，KVM_RUN 会返回 exit_reason = KVM_EXIT_FAIL_ENTRY，
并通过把 fail_entry 结构的 hardare_entry_failure_reason 字段设置为
KVM_EXIT_FAIL_ENTRY_CPU_UNSUPPORTED、cpu 字段设置为处理器 id 来填充
fail_entry 结构。)

1.5 ATTRIBUTE: KVM_ARM_VCPU_PMU_V3_SET_NR_COUNTERS(1.5 属性：KVM_ARM_VCPU_PMU_V3_SET_NR_COUNTERS)
-------------------------------------------------------------------------------------------------

:Parameters: in kvm_device_attr.addr the address to an unsigned int
	     representing the maximum value taken by PMCR_EL0.N
(参数：kvm_device_attr.addr 中是指向一个 unsigned int 的地址，该值表示
PMCR_EL0.N 的最大取值。)

:Returns:(返回值：)

	 =======  ====================================================
	 -EBUSY   PMUv3 already initialized, a VCPU has already run or
                  an event filter has already been set (PMUv3 已初始化、VCPU 已运行或事件过滤器已被设置)
	 -EFAULT  Error accessing the value pointed to by addr (访问 addr 所指向的值出错)
	 -ENODEV  PMUv3 not supported or GIC not initialized (不支持 PMUv3 或 GIC 未初始化)
	 -EINVAL  No PMUv3 explicitly selected, or value of N out of
	 	  range (未显式选择 PMUv3，或 N 的值超出范围)
	 =======  ====================================================

Set the number of implemented event counters in the virtual PMU. This
mandates that a PMU has explicitly been selected via
KVM_ARM_VCPU_PMU_V3_SET_PMU, and will fail when no PMU has been
explicitly selected, or the number of counters is out of range for the
selected PMU. Selecting a new PMU cancels the effect of setting this
attribute.
(设置虚拟 PMU 中已实现的事件计数器数量。这要求已经通过
KVM_ARM_VCPU_PMU_V3_SET_PMU 显式选择了 PMU；当没有显式选择 PMU、或计数器
数量超出所选 PMU 的范围时将会失败。选择新的 PMU 会取消设置此属性所产生的
效果。)

2. GROUP: KVM_ARM_VCPU_TIMER_CTRL(2. 组：KVM_ARM_VCPU_TIMER_CTRL)
=================================================================

:Architectures: ARM64
(架构：ARM64)

2.1. ATTRIBUTES: KVM_ARM_VCPU_TIMER_IRQ_{VTIMER,PTIMER,HVTIMER,HPTIMER}(2.1. 属性：KVM_ARM_VCPU_TIMER_IRQ_{VTIMER,PTIMER,HVTIMER,HPTIMER})
------------------------------------------------------------------------------------------------------------------------------------------

:Parameters: in kvm_device_attr.addr the address for the timer interrupt is a
	     pointer to an int
(参数：kvm_device_attr.addr 中是定时器中断的地址，为一个指向 int 的
指针。)

Returns:(返回值：)

	 =======  =================================
	 -EINVAL  Invalid timer interrupt number (定时器中断号无效)
	 -EBUSY   One or more VCPUs has already run (一个或多个 VCPU 已经运行)
	 =======  =================================

A value describing the architected timer interrupt number when connected to an
in-kernel virtual GIC.  These must be a PPI (16 <= intid < 32).  Setting the
attribute overrides the default values (see below).
(一个描述连接到内核内虚拟 GIC 时架构定时器中断号的值。这些必须是 PPI
(16 <= intid < 32)。设置该属性会覆盖默认值 (见下文)。)

==============================  ==========================================
KVM_ARM_VCPU_TIMER_IRQ_VTIMER   The EL1 virtual timer intid (default: 27) (EL1 虚拟定时器的 intid，默认值为 27)
KVM_ARM_VCPU_TIMER_IRQ_PTIMER   The EL1 physical timer intid (default: 30) (EL1 物理定时器的 intid，默认值为 30)
KVM_ARM_VCPU_TIMER_IRQ_HVTIMER  The EL2 virtual timer intid (default: 28) (EL2 虚拟定时器的 intid，默认值为 28)
KVM_ARM_VCPU_TIMER_IRQ_HPTIMER  The EL2 physical timer intid (default: 26) (EL2 物理定时器的 intid，默认值为 26)
==============================  ==========================================

Setting the same PPI for different timers will prevent the VCPUs from running.
Setting the interrupt number on a VCPU configures all VCPUs created at that
time to use the number provided for a given timer, overwriting any previously
configured values on other VCPUs.  Userspace should configure the interrupt
numbers on at least one VCPU after creating all VCPUs and before running any
VCPUs.
(为不同定时器设置相同的 PPI 将阻止 VCPU 运行。在某个 VCPU 上设置中断号，
会使当时已创建的所有 VCPU 都对给定定时器使用所提供的编号，并覆盖其他 VCPU
上先前配置的值。用户态应当在创建全部 VCPU 之后、运行任何 VCPU 之前，在
至少一个 VCPU 上配置中断号。)

.. _kvm_arm_vcpu_pvtime_ctrl:

3. GROUP: KVM_ARM_VCPU_PVTIME_CTRL(3. 组：KVM_ARM_VCPU_PVTIME_CTRL)
===================================================================

:Architectures: ARM64
(架构：ARM64)

3.1 ATTRIBUTE: KVM_ARM_VCPU_PVTIME_IPA(3.1 属性：KVM_ARM_VCPU_PVTIME_IPA)
-------------------------------------------------------------------------

:Parameters: 64-bit base address
(参数：64 位基地址。)

Returns:(返回值：)

	 =======  ======================================
	 -ENXIO   Stolen time not implemented (未实现 stolen time)
	 -EEXIST  Base address already set for this VCPU (此 VCPU 的基地址已被设置)
	 -EINVAL  Base address not 64 byte aligned (基地址未按 64 字节对齐)
	 =======  ======================================

Specifies the base address of the stolen time structure for this VCPU. The
base address must be 64 byte aligned and exist within a valid guest memory
region. See Documentation/virt/kvm/arm/pvtime.rst for more information
including the layout of the stolen time structure.
(指定此 VCPU 的 stolen time 结构的基地址。基地址必须按 64 字节对齐，并且
位于有效的客户机内存区域内。更多信息 (包括 stolen time 结构的布局) 参见
Documentation/virt/kvm/arm/pvtime.rst。)

4. GROUP: KVM_VCPU_TSC_CTRL(4. 组：KVM_VCPU_TSC_CTRL)
=====================================================

:Architectures: x86
(架构：x86)

4.1 ATTRIBUTE: KVM_VCPU_TSC_OFFSET(4.1 属性：KVM_VCPU_TSC_OFFSET)

:Parameters: 64-bit unsigned TSC offset
(参数：64 位无符号 TSC 偏移。)

Returns:(返回值：)

	 ======= ======================================
	 -EFAULT Error reading/writing the provided
		 parameter address. (读取/写入所提供的参数地址出错)
	 -ENXIO  Attribute not supported (不支持该属性)
	 ======= ======================================

Specifies the guest's TSC offset relative to the host's TSC. The guest's
TSC is then derived by the following equation:
(指定客户机 TSC 相对于宿主机 TSC 的偏移。客户机的 TSC 随后由以下公式推导
得出：)

  guest_tsc = host_tsc + KVM_VCPU_TSC_OFFSET

This attribute is useful to adjust the guest's TSC on live migration,
so that the TSC counts the time during which the VM was paused. The
following describes a possible algorithm to use for this purpose.
(该属性可用于在热迁移 (live migration) 时调整客户机的 TSC，使 TSC 把 VM
被暂停期间的时间也计算在内。下文描述了可用于此目的的一种可能算法。)

From the source VMM process:
(来自源端 VMM 进程：)

1. Invoke the KVM_GET_CLOCK ioctl to record the host TSC (tsc_src),
   kvmclock nanoseconds (guest_src), and host CLOCK_REALTIME nanoseconds
   (host_src).
   (调用 KVM_GET_CLOCK ioctl，记录宿主机 TSC (tsc_src)、kvmclock 纳秒数
   (guest_src) 以及宿主机 CLOCK_REALTIME 纳秒数 (host_src)。)

2. Read the KVM_VCPU_TSC_OFFSET attribute for every vCPU to record the
   guest TSC offset (ofs_src[i]).
   (读取每个 vCPU 的 KVM_VCPU_TSC_OFFSET 属性，记录客户机 TSC 偏移
   (ofs_src[i])。)

3. Invoke the KVM_GET_TSC_KHZ ioctl to record the frequency of the
   guest's TSC (freq).
   (调用 KVM_GET_TSC_KHZ ioctl，记录客户机 TSC 的频率 (freq)。)

From the destination VMM process:
(来自目的端 VMM 进程：)

4. Invoke the KVM_SET_CLOCK ioctl, providing the source nanoseconds from
   kvmclock (guest_src) and CLOCK_REALTIME (host_src) in their respective
   fields.  Ensure that the KVM_CLOCK_REALTIME flag is set in the provided
   structure.
   (调用 KVM_SET_CLOCK ioctl，把来自 kvmclock (guest_src) 与
   CLOCK_REALTIME (host_src) 的源端纳秒数填入各自的字段。确保在所提供的
   结构中设置了 KVM_CLOCK_REALTIME 标志。)

   KVM will advance the VM's kvmclock to account for elapsed time since
   recording the clock values.  Note that this will cause problems in
   the guest (e.g., timeouts) unless CLOCK_REALTIME is synchronized
   between the source and destination, and a reasonably short time passes
   between the source pausing the VMs and the destination executing
   steps 4-7.
   (KVM 会把 VM 的 kvmclock 向前推进，以计入记录时钟值之后流逝的时间。
   注意，除非 CLOCK_REALTIME 在源端与目的端之间已经同步，并且从源端暂停
   VM 到目的端执行步骤 4-7 之间经过的时间足够短，否则这会在客户机中造成
   问题 (例如超时)。)

5. Invoke the KVM_GET_CLOCK ioctl to record the host TSC (tsc_dest) and
   kvmclock nanoseconds (guest_dest).
   (调用 KVM_GET_CLOCK ioctl，记录宿主机 TSC (tsc_dest) 与 kvmclock 纳秒
   数 (guest_dest)。)

6. Adjust the guest TSC offsets for every vCPU to account for (1) time
   elapsed since recording state and (2) difference in TSCs between the
   source and destination machine:
   (为每个 vCPU 调整客户机 TSC 偏移，以计入 (1) 记录状态之后流逝的时间，
   以及 (2) 源端与目的端机器之间 TSC 的差异：)

   ofs_dst[i] = ofs_src[i] -
     (guest_src - guest_dest) * freq +
     (tsc_src - tsc_dest)

   ("ofs[i] + tsc - guest * freq" is the guest TSC value corresponding to
   a time of 0 in kvmclock.  The above formula ensures that it is the
   same on the destination as it was on the source).
   ("ofs[i] + tsc - guest * freq" 是 kvmclock 时刻为 0 时对应的客户机 TSC
   值。上述公式保证该值在目的端与在源端时相同。)

7. Write the KVM_VCPU_TSC_OFFSET attribute for every vCPU with the
   respective value derived in the previous step.
   (用上一步推导出的相应值，为每个 vCPU 写入 KVM_VCPU_TSC_OFFSET 属性。)
