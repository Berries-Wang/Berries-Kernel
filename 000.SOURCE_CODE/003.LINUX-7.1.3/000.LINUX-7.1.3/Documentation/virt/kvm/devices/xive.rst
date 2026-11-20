.. SPDX-License-Identifier: GPL-2.0

==================================================================================================
POWER9 eXternal Interrupt Virtualization Engine (XIVE Gen1)(POWER9 外部中断虚拟化引擎 (XIVE Gen1))
==================================================================================================

Device types supported:(支持的设备类型：)
  - KVM_DEV_TYPE_XIVE     POWER9 XIVE Interrupt Controller generation 1(POWER9 XIVE 第一代中断控制器)

This device acts as a VM interrupt controller. It provides the KVM
interface to configure the interrupt sources of a VM in the underlying
POWER9 XIVE interrupt controller.
(该设备充当 VM 的中断控制器。它提供 KVM 接口，用于在底层 POWER9 XIVE 中断控制器中配置 VM 的中断源。)

Only one XIVE instance may be instantiated. A guest XIVE device
requires a POWER9 host and the guest OS should have support for the
XIVE native exploitation interrupt mode. If not, it should run using
the legacy interrupt mode, referred as XICS (POWER7/8).
(只能实例化一个 XIVE 实例。客户机 XIVE 设备需要 POWER9 宿主机，且客户机操作系统应支持 XIVE 原生 exploitation 中断模式；若不支持，则应使用旧式中断模式运行，即 XICS (POWER7/8)。)

* Device Mappings(设备映射)

  The KVM device exposes different MMIO ranges of the XIVE HW which
  are required for interrupt management. These are exposed to the
  guest in VMAs populated with a custom VM fault handler.
  (KVM 设备暴露 XIVE 硬件中中断管理所需的多段不同 MMIO 地址范围。这些范围通过由自定义 VM 缺页处理函数填充的 VMA 暴露给客户机。)

  1. Thread Interrupt Management Area (TIMA)(线程中断管理区 (TIMA))

  Each thread has an associated Thread Interrupt Management context
  composed of a set of registers. These registers let the thread
  handle priority management and interrupt acknowledgment. The most
  important are :
  (每个线程都有一组寄存器构成的线程中断管理上下文。这些寄存器让线程进行优先级管理与中断确认。其中最重要的是：)

      - Interrupt Pending Buffer     (IPB)(中断挂起缓冲区)
      - Current Processor Priority   (CPPR)(当前处理器优先级)
      - Notification Source Register (NSR)(通知源寄存器)

  They are exposed to software in four different pages each proposing
  a view with a different privilege. The first page is for the
  physical thread context and the second for the hypervisor. Only the
  third (operating system) and the fourth (user level) are exposed the
  guest.
  (它们以四个不同的页面暴露给软件，每个页面提供不同特权级别的视图。第一页用于物理线程上下文，第二页用于 hypervisor。只有第三页 (操作系统) 和第四页 (用户态) 会暴露给客户机。)

  2. Event State Buffer (ESB)(事件状态缓冲区 (ESB))

  Each source is associated with an Event State Buffer (ESB) with
  either a pair of even/odd pair of pages which provides commands to
  manage the source: to trigger, to EOI, to turn off the source for
  instance.
  (每个中断源都关联一个事件状态缓冲区 (ESB)，由一对偶/奇页对组成，其中提供了管理该中断源的命令：例如触发、EOI、关闭该中断源。)

  3. Device pass-through(设备直通)

  When a device is passed-through into the guest, the source
  interrupts are from a different HW controller (PHB4) and the ESB
  pages exposed to the guest should accommodate this change.
  (当设备直通 (pass-through) 进客户机时，中断源来自另一个硬件控制器 (PHB4)，暴露给客户机的 ESB 页面需要适应这一变化。)

  The passthru_irq helpers, kvmppc_xive_set_mapped() and
  kvmppc_xive_clr_mapped() are called when the device HW irqs are
  mapped into or unmapped from the guest IRQ number space. The KVM
  device extends these helpers to clear the ESB pages of the guest IRQ
  number being mapped and then lets the VM fault handler repopulate.
  The handler will insert the ESB page corresponding to the HW
  interrupt of the device being passed-through or the initial IPI ESB
  page if the device has being removed.
  (当设备硬件中断映射进客户机 IRQ 编号空间或从中解映射时，会调用 passthru_irq 辅助函数 kvmppc_xive_set_mapped() 与 kvmppc_xive_clr_mapped()。KVM 设备扩展了这些辅助函数，先清除被映射客户机 IRQ 编号对应的 ESB 页面，再让 VM 缺页处理函数重新填充。该处理函数会插入与被直通设备硬件中断对应的 ESB 页；若设备已被移除，则插入初始 IPI ESB 页。)

  The ESB remapping is fully transparent to the guest and the OS
  device driver. All handling is done within VFIO and the above
  helpers in KVM-PPC.
  (ESB 重映射对客户机与操作系统设备驱动完全透明。全部处理都在 VFIO 以及 KVM-PPC 中上述辅助函数内完成。)

* Groups:(组：)

1. KVM_DEV_XIVE_GRP_CTRL
     Provides global controls on the device
     (提供针对设备的全局控制。)

  Attributes:(属性：)
    1.1 KVM_DEV_XIVE_RESET (write only)(只写)
    Resets the interrupt controller configuration for sources and event
    queues. To be used by kexec and kdump.
    (重置中断控制器中关于中断源与事件队列的配置。供 kexec 与 kdump 使用。)

    Errors: none(无)

    1.2 KVM_DEV_XIVE_EQ_SYNC (write only)(只写)
    Sync all the sources and queues and mark the EQ pages dirty. This
    to make sure that a consistent memory state is captured when
    migrating the VM.
    (同步所有中断源与队列，并把 EQ 页面标记为脏。这是为了确保在迁移 VM 时能够捕获一致的内存状态。)

    Errors: none(无)

    1.3 KVM_DEV_XIVE_NR_SERVERS (write only)(只写)
    The kvm_device_attr.addr points to a __u32 value which is the number of
    interrupt server numbers (ie, highest possible vcpu id plus one).
    (kvm_device_attr.addr 指向一个 __u32 值，即中断服务器编号的数量 (也就是可能的最高 vcpu id 加一)。)

    Errors:(错误：)

      =======  ==========================================
      -EINVAL  Value greater than KVM_MAX_VCPU_IDS.(大于 KVM_MAX_VCPU_IDS。)
      -EFAULT  Invalid user pointer for attr->addr.(attr->addr 用户指针无效。)
      -EBUSY   A vCPU is already connected to the device.(已有 vCPU 连接到该设备。)
      =======  ==========================================

2. KVM_DEV_XIVE_GRP_SOURCE (write only)(只写)
     Initializes a new source in the XIVE device and mask it.
     (在 XIVE 设备中初始化一个新中断源并将其屏蔽。)

  Attributes:(属性：)
    Interrupt source number  (64-bit)(中断源编号 (64 位))

  The kvm_device_attr.addr points to a __u64 value::

    bits:     | 63   ....  2 |   1   |   0
    values:   |    unused    | level | type

  (kvm_device_attr.addr 指向一个 __u64 值：)

  - type:  0:MSI 1:LSI(类型：0 为 MSI，1 为 LSI)
  - level: assertion level in case of an LSI.(电平：LSI 情况下的断言电平)

  Errors:(错误：)

    =======  ==========================================
    -E2BIG   Interrupt source number is out of range(中断源编号超出范围)
    -ENOMEM  Could not create a new source block(无法创建新的中断源块)
    -EFAULT  Invalid user pointer for attr->addr.(attr->addr 用户指针无效。)
    -ENXIO   Could not allocate underlying HW interrupt(无法分配底层硬件中断)
    =======  ==========================================

3. KVM_DEV_XIVE_GRP_SOURCE_CONFIG (write only)(只写)
     Configures source targeting
     (配置中断源的路由目标)

  Attributes:(属性：)
    Interrupt source number  (64-bit)(中断源编号 (64 位))

  The kvm_device_attr.addr points to a __u64 value::

    bits:     | 63   ....  33 |  32  | 31 .. 3 |  2 .. 0
    values:   |    eisn       | mask |  server | priority

  (kvm_device_attr.addr 指向一个 __u64 值：)

  - priority: 0-7 interrupt priority level(优先级：0-7 中断优先级)
  - server: CPU number chosen to handle the interrupt(服务器：选择处理该中断的 CPU 编号)
  - mask: mask flag (unused)(屏蔽标志 (未使用))
  - eisn: Effective Interrupt Source Number(有效中断源编号)

  Errors:(错误：)

    =======  =======================================================
    -ENOENT  Unknown source number(未知的中断源编号)
    -EINVAL  Not initialized source number(中断源编号未初始化)
    -EINVAL  Invalid priority(优先级无效)
    -EINVAL  Invalid CPU number.(CPU 编号无效。)
    -EFAULT  Invalid user pointer for attr->addr.(attr->addr 用户指针无效。)
    -ENXIO   CPU event queues not configured or configuration of the
	     underlying HW interrupt failed(CPU 事件队列未配置，或底层硬件中断配置失败)
    -EBUSY   No CPU available to serve interrupt(没有可用的 CPU 来服务该中断)
    =======  =======================================================

4. KVM_DEV_XIVE_GRP_EQ_CONFIG (read-write)(读写)
     Configures an event queue of a CPU
     (配置某个 CPU 的事件队列)

  Attributes:(属性：)
    EQ descriptor identifier (64-bit)(EQ 描述符标识 (64 位))

  The EQ descriptor identifier is a tuple (server, priority)::

    bits:     | 63   ....  32 | 31 .. 3 |  2 .. 0
    values:   |    unused     |  server | priority

  (EQ 描述符标识是一个 (server, priority) 元组：)

  The kvm_device_attr.addr points to::

    struct kvm_ppc_xive_eq {
	__u32 flags;
	__u32 qshift;
	__u64 qaddr;
	__u32 qtoggle;
	__u32 qindex;
	__u8  pad[40];
    };

  (kvm_device_attr.addr 指向：)

  - flags: queue flags
      KVM_XIVE_EQ_ALWAYS_NOTIFY (required)
	forces notification without using the coalescing mechanism
	provided by the XIVE END ESBs.
	(flags：队列标志。KVM_XIVE_EQ_ALWAYS_NOTIFY (必需)：不使用 XIVE END ESB 提供的聚合机制，强制进行通知。)
  - qshift: queue size (power of 2)(qshift：队列大小 (2 的幂))
  - qaddr: real address of queue(qaddr：队列的实地址)
  - qtoggle: current queue toggle bit(qtoggle：当前队列翻转位)
  - qindex: current queue index(qindex：当前队列索引)
  - pad: reserved for future use(pad：留作将来使用)

  Errors:(错误：)

    =======  =========================================
    -ENOENT  Invalid CPU number(CPU 编号无效)
    -EINVAL  Invalid priority(优先级无效)
    -EINVAL  Invalid flags(标志无效)
    -EINVAL  Invalid queue size(队列大小无效)
    -EINVAL  Invalid queue address(队列地址无效)
    -EFAULT  Invalid user pointer for attr->addr.(attr->addr 用户指针无效。)
    -EIO     Configuration of the underlying HW failed(底层硬件配置失败)
    =======  =========================================

5. KVM_DEV_XIVE_GRP_SOURCE_SYNC (write only)(只写)
     Synchronize the source to flush event notifications
     (同步中断源以冲刷事件通知)

  Attributes:(属性：)
    Interrupt source number  (64-bit)(中断源编号 (64 位))

  Errors:(错误：)

    =======  =============================
    -ENOENT  Unknown source number(未知的中断源编号)
    -EINVAL  Not initialized source number(中断源编号未初始化)
    =======  =============================

* VCPU state(VCPU 状态)

  The XIVE IC maintains VP interrupt state in an internal structure
  called the NVT. When a VP is not dispatched on a HW processor
  thread, this structure can be updated by HW if the VP is the target
  of an event notification.
  (XIVE 中断控制器在一个叫 NVT 的内部结构中维护 VP 中断状态。当 VP 未被调度到某个硬件处理器线程上时，若该 VP 是某个事件通知的目标，硬件仍可更新此结构。)

  It is important for migration to capture the cached IPB from the NVT
  as it synthesizes the priorities of the pending interrupts. We
  capture a bit more to report debug information.
  (对迁移而言，从 NVT 中捕获缓存的 IPB 很重要，因为它综合了挂起中断的优先级。我们还多捕获了一点信息用于报告调试信息。)

  KVM_REG_PPC_VP_STATE (2 * 64bits)::

    bits:     |  63  ....  32  |  31  ....  0  |
    values:   |   TIMA word0   |   TIMA word1  |
    bits:     | 127       ..........       64  |
    values:   |            unused              |

  (KVM_REG_PPC_VP_STATE (2 * 64 位)：)

* Migration:(迁移：)

  Saving the state of a VM using the XIVE native exploitation mode
  should follow a specific sequence. When the VM is stopped :
  (保存使用 XIVE 原生 exploitation 模式的 VM 状态应遵循特定顺序。当 VM 停止时：)

  1. Mask all sources (PQ=01) to stop the flow of events.
     (屏蔽所有中断源 (PQ=01) 以停止事件流。)

  2. Sync the XIVE device with the KVM control KVM_DEV_XIVE_EQ_SYNC to
  flush any in-flight event notification and to stabilize the EQs. At
  this stage, the EQ pages are marked dirty to make sure they are
  transferred in the migration sequence.
  (使用 KVM 控制 KVM_DEV_XIVE_EQ_SYNC 同步 XIVE 设备，以冲刷所有进行中的事件通知并稳定各个 EQ。在此阶段，EQ 页面被标记为脏，以确保它们会被包含进迁移序列。)

  3. Capture the state of the source targeting, the EQs configuration
  and the state of thread interrupt context registers.
  (捕获中断源路由目标的状态、各 EQ 的配置以及线程中断上下文寄存器的状态。)

  Restore is similar:(恢复流程与之类似：)

  1. Restore the EQ configuration. As targeting depends on it.
     (恢复 EQ 配置，因为路由目标依赖于此。)
  2. Restore targeting(恢复路由目标)
  3. Restore the thread interrupt contexts(恢复线程中断上下文)
  4. Restore the source states(恢复中断源状态)
  5. Let the vCPU run(让 vCPU 运行)
