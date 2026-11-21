.. SPDX-License-Identifier: GPL-2.0

===========================================
Affinity managed interrupts(亲和性受管中断)
===========================================

The IRQ core provides support for managing interrupts according to a specified
CPU affinity. Under normal operation, an interrupt is associated with a
particular CPU. If that CPU is taken offline, the interrupt is migrated to
another online CPU.
(IRQ 核心提供了根据指定的 CPU 亲和性来管理中断的支持。在正常操作下，中断与某个特定 CPU 相关联。如果该 CPU 下线，中断会迁移到另一个在线 CPU。)

Devices with large numbers of interrupt vectors can stress the available vector
space. For example, an NVMe device with 128 I/O queues typically requests one
interrupt per queue on systems with at least 128 CPUs. Two such devices
therefore request 256 interrupts. On x86, the interrupt vector space is
notoriously low, providing only 256 vectors per CPU, and the kernel reserves a
subset of these, further reducing the number available for device interrupts.
In practice this is not an issue because the interrupts are distributed across
many CPUs, so each CPU only receives a small number of vectors.
(拥有大量中断向量的设备会给可用的向量空间带来压力。例如，在拥有至少 128 个 CPU 的系统上，带有 128 个 I/O 队列的 NVMe 设备通常为每个队列请求一个中断。两个这样的设备因此会请求 256 个中断。在 x86 上，中断向量空间出了名地小，每个 CPU 只提供 256 个向量，而内核又保留了其中一个子集，进一步减少了可用于设备中断的数量。在实践中这不是问题，因为中断分布在许多 CPU 上，所以每个 CPU 只接收少量向量。)

During system suspend, however, all secondary CPUs are taken offline and all
interrupts are migrated to the single CPU that remains online. This can exhaust
the available interrupt vectors on that CPU and cause the suspend operation to
fail.
(然而，在系统挂起期间，所有辅助 CPU 都会下线，所有中断都会迁移到保持在线的那个 CPU。这可能会耗尽该 CPU 上可用的中断向量，导致挂起操作失败。)

Affinity‑managed interrupts address this limitation. Each interrupt is assigned
a CPU affinity mask that specifies the set of CPUs on which the interrupt may
be targeted. When a CPU in the mask goes offline, the interrupt is moved to the
next CPU in the mask. If the last CPU in the mask goes offline, the interrupt
is shut down. Drivers using affinity‑managed interrupts must ensure that the
associated queue is quiesced before the interrupt is disabled so that no
further interrupts are generated. When a CPU in the affinity mask comes back
online, the interrupt is re‑enabled.
(亲和性受管中断解决了这一限制。每个中断都会被分配一个 CPU 亲和性掩码，指定该中断可以定向到的 CPU 集合。当掩码中的某个 CPU 下线时，中断会移动到掩码中的下一个 CPU。如果掩码中的最后一个 CPU 下线，中断会被关闭。使用亲和性受管中断的驱动程序必须确保在禁用中断之前关联的队列已静止，以便不再产生进一步的中断。当亲和性掩码中的某个 CPU 重新上线时，中断会被重新启用。)

Implementation(实现)
--------------------

Devices must provide per‑instance interrupts, such as per‑I/O‑queue interrupts
for storage devices like NVMe. The driver allocates interrupt vectors with the
required affinity settings using struct irq_affinity. For MSI‑X devices, this
is done via pci_alloc_irq_vectors_affinity() with the PCI_IRQ_AFFINITY flag
set.
(设备必须提供每实例中断，例如像 NVMe 这样的存储设备的每 I/O 队列中断。驱动程序使用 struct irq_affinity 以所需的亲和性设置分配中断向量。对于 MSI‑X 设备，这通过 pci_alloc_irq_vectors_affinity() 并设置 PCI_IRQ_AFFINITY 标志来完成。)

Based on the provided affinity information, the IRQ core attempts to spread the
interrupts evenly across the system. The affinity masks are computed during
this allocation step, but the final IRQ assignment is performed when
request_irq() is invoked.
(根据提供的亲和性信息，IRQ 核心尝试把中断均匀地分布到整个系统。亲和性掩码在此分配步骤中计算，但最终的 IRQ 分配在调用 request_irq() 时执行。)

Isolated CPUs(隔离的 CPU)
-------------------------

The affinity of managed interrupts is handled entirely in the kernel and cannot
be modified from user space through the /proc interfaces. The managed_irq
sub‑parameter of the isolcpus boot option specifies a CPU mask that managed
interrupts should attempt to avoid. This isolation is best‑effort and only
applies if the automatically assigned interrupt mask also contains online CPUs
outside the avoided mask. If the requested mask contains only isolated CPUs,
the setting has no effect.
(受管中断的亲和性完全在内核中处理，不能从用户空间通过 /proc 接口修改。isolcpus 启动选项的 managed_irq 子参数指定受管中断应尽力避开的一个 CPU 掩码。这种隔离是尽力而为的，并且仅在自动分配的中断掩码同时包含所避开掩码之外的在线 CPU 时才生效。如果请求的掩码只包含隔离的 CPU，则该设置无效。)

CPUs listed in the avoided mask remain part of the interrupt’s affinity mask.
This means that if all non‑isolated CPUs go offline while isolated CPUs remain
online, the interrupt will be assigned to one of the isolated CPUs.
(列在被避开掩码中的 CPU 仍是中断亲和性掩码的一部分。这意味着如果所有非隔离的 CPU 都下线而隔离的 CPU 保持在线，中断将被分配给其中一个隔离的 CPU。)

The following examples assume a system with 8 CPUs.
(以下示例假设系统有 8 个 CPU。)

- A QEMU instance is booted with "-device virtio-scsi-pci".
  The MSI‑X device exposes 11 interrupts: 3 "management" interrupts and 8
  "queue" interrupts. The driver requests the 8 queue interrupts, each of which
  is affine to exactly one CPU. If that CPU goes offline, the interrupt is shut
  down.
  (一个 QEMU 实例以 "-device virtio-scsi-pci" 启动。该 MSI‑X 设备暴露 11 个中断：3 个"管理"中断和 8 个"队列"中断。驱动程序请求这 8 个队列中断，每个都恰好绑定到一个 CPU。如果该 CPU 下线，中断会被关闭。)

  Assuming interrupt 48 is one of the queue interrupts, the following appears::

    /proc/irq/48/effective_affinity_list:7
    /proc/irq/48/smp_affinity_list:7

  (假设中断 48 是队列中断之一，则会出现以下内容：)

  This indicates that the interrupt is served only by CPU7. Shutting down CPU7
  does not migrate the interrupt to another CPU::

    /proc/irq/48/effective_affinity_list:0
    /proc/irq/48/smp_affinity_list:7

  (这表明该中断仅由 CPU7 服务。关闭 CPU7 不会把中断迁移到另一个 CPU：)

  This can be verified via the debugfs interface
  (/sys/kernel/debug/irq/irqs/48). The dstate field will include
  IRQD_IRQ_DISABLED, IRQD_IRQ_MASKED and IRQD_MANAGED_SHUTDOWN.
  (这可以通过 debugfs 接口（/sys/kernel/debug/irq/irqs/48）验证。dstate 字段将包含 IRQD_IRQ_DISABLED、IRQD_IRQ_MASKED 和 IRQD_MANAGED_SHUTDOWN。)

- A QEMU instance is booted with "-device virtio-scsi-pci,num_queues=2"
  and the kernel command line includes:
  "irqaffinity=0,1 isolcpus=domain,2-7 isolcpus=managed_irq,1-3,5-7".
  The MSI‑X device exposes 5 interrupts: 3 management interrupts and 2 queue
  interrupts. The management interrupts follow the irqaffinity= setting. The
  queue interrupts are spread across available CPUs::

    /proc/irq/47/effective_affinity_list:0
    /proc/irq/47/smp_affinity_list:0-3
    /proc/irq/48/effective_affinity_list:4
    /proc/irq/48/smp_affinity_list:4-7

  (一个 QEMU 实例以 "-device virtio-scsi-pci,num_queues=2" 启动，并且内核命令行包括："irqaffinity=0,1 isolcpus=domain,2-7 isolcpus=managed_irq,1-3,5-7"。该 MSI‑X 设备暴露 5 个中断：3 个管理中断和 2 个队列中断。管理中断遵循 irqaffinity= 设置。队列中断分布在可用的 CPU 上：)

  The two queue interrupts are evenly distributed. Interrupt 48 is placed on CPU4
  because the managed_irq mask avoids CPUs 5–7 when possible.
  (这两个队列中断被均匀分布。中断 48 被放置在 CPU4 上，因为 managed_irq 掩码在可能的情况下避开 CPU 5–7。)

  Replacing the managed_irq argument with "isolcpus=managed_irq,1-3,4-5,7"
  results in::

    /proc/irq/48/effective_affinity_list:6
    /proc/irq/48/smp_affinity_list:4-7

  (把 managed_irq 参数替换为 "isolcpus=managed_irq,1-3,4-5,7" 的结果是：)

  Interrupt 48 is now served on CPU6 because the system avoids CPUs 4, 5 and
  7. If CPU6 is taken offline, the interrupt migrates to one of the "isolated"
  CPUs::

    /proc/irq/48/effective_affinity_list:7
    /proc/irq/48/smp_affinity_list:4-7

  (中断 48 现在在 CPU6 上服务，因为系统避开了 CPU 4、5 和 7。如果 CPU6 下线，中断会迁移到其中一个"隔离"的 CPU：)

  The interrupt is shut down once all CPUs listed in its smp_affinity mask are
  offline.
  (一旦其 smp_affinity 掩码中列出的所有 CPU 都下线，中断就会被关闭。)
