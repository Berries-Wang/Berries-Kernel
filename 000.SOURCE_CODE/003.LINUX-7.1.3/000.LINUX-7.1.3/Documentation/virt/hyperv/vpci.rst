.. SPDX-License-Identifier: GPL-2.0

PCI pass-thru devices(PCI 直通设备)
===================================
In a Hyper-V guest VM, PCI pass-thru devices (also called
virtual PCI devices, or vPCI devices) are physical PCI devices
that are mapped directly into the VM's physical address space.
Guest device drivers can interact directly with the hardware
without intermediation by the host hypervisor.  This approach
provides higher bandwidth access to the device with lower
latency, compared with devices that are virtualized by the
hypervisor.  The device should appear to the guest just as it
would when running on bare metal, so no changes are required
to the Linux device drivers for the device.
(在 Hyper-V 客户机 VM 中，PCI 直通设备（也称为虚拟 PCI 设备或 vPCI 设备）是被直接映射进 VM 物理地址空间的物理 PCI 设备。客户机设备驱动可以无需宿主机 hypervisor 居中介入而直接与硬件交互。与由 hypervisor 虚拟化的设备相比，这种方式以更低的延迟提供对设备更高的带宽访问。设备在客户机看来应当与在裸机上运行时一模一样，因此该设备的 Linux 设备驱动无需任何修改。)

Hyper-V terminology for vPCI devices is "Discrete Device
Assignment" (DDA).  Public documentation for Hyper-V DDA is
available here: `DDA`_
(Hyper-V 对 vPCI 设备的称呼是"离散设备分配"（Discrete Device Assignment，DDA）。Hyper-V DDA 的公开文档见： `DDA`_)

.. _DDA: https://learn.microsoft.com/en-us/windows-server/virtualization/hyper-v/plan/plan-for-deploying-devices-using-discrete-device-assignment

DDA is typically used for storage controllers, such as NVMe,
and for GPUs.  A similar mechanism for NICs is called SR-IOV
and produces the same benefits by allowing a guest device
driver to interact directly with the hardware.  See Hyper-V
public documentation here: `SR-IOV`_
(DDA 通常用于存储控制器（如 NVMe）和 GPU。网卡类似的机制称为 SR-IOV，它允许客户机设备驱动直接与硬件交互，带来同样的好处。Hyper-V 公开文档见： `SR-IOV`_)

.. _SR-IOV: https://learn.microsoft.com/en-us/windows-hardware/drivers/network/overview-of-single-root-i-o-virtualization--sr-iov-

This discussion of vPCI devices includes DDA and SR-IOV
devices.
(本文对 vPCI 设备的讨论涵盖 DDA 与 SR-IOV 设备。)

Device Presentation(设备呈现)
-----------------------------
Hyper-V provides full PCI functionality for a vPCI device when
it is operating, so the Linux device driver for the device can
be used unchanged, provided it uses the correct Linux kernel
APIs for accessing PCI config space and for other integration
with Linux.  But the initial detection of the PCI device and
its integration with the Linux PCI subsystem must use Hyper-V
specific mechanisms.  Consequently, vPCI devices on Hyper-V
have a dual identity.  They are initially presented to Linux
guests as VMBus devices via the standard VMBus "offer"
mechanism, so they have a VMBus identity and appear under
/sys/bus/vmbus/devices.  The VMBus vPCI driver in Linux at
drivers/pci/controller/pci-hyperv.c handles a newly introduced
vPCI device by fabricating a PCI bus topology and creating all
the normal PCI device data structures in Linux that would
exist if the PCI device were discovered via ACPI on a bare-
metal system.  Once those data structures are set up, the
device also has a normal PCI identity in Linux, and the normal
Linux device driver for the vPCI device can function as if it
were running in Linux on bare-metal.  Because vPCI devices are
presented dynamically through the VMBus offer mechanism, they
do not appear in the Linux guest's ACPI tables.  vPCI devices
may be added to a VM or removed from a VM at any time during
the life of the VM, and not just during initial boot.
(当 vPCI 设备工作时，Hyper-V 为其提供完整的 PCI 功能，因此设备的 Linux 设备驱动可以原样使用——只要它使用正确的 Linux 内核 API 访问 PCI 配置空间并完成与 Linux 的其他集成。但 PCI 设备的初始检测及其与 Linux PCI 子系统的集成必须使用 Hyper-V 特有的机制。因此，Hyper-V 上的 vPCI 设备具有双重身份。它们最初通过标准的 VMBus "offer" 机制作为 VMBus 设备呈现给 Linux 客户机，因此拥有 VMBus 身份并出现在 /sys/bus/vmbus/devices 下。Linux 中位于 drivers/pci/controller/pci-hyperv.c 的 VMBus vPCI 驱动处理新引入的 vPCI 设备的方式是：构造一个 PCI 总线拓扑，并创建 PCI 设备若在裸机系统上经 ACPI 发现时本会存在的全部常规 PCI 设备数据结构。这些数据结构建立之后，该设备在 Linux 中还拥有常规 PCI 身份，vPCI 设备的常规 Linux 设备驱动就能像运行在裸机 Linux 上一样工作。由于 vPCI 设备是通过 VMBus offer 机制动态呈现的，它们不会出现在 Linux 客户机的 ACPI 表中。vPCI 设备可以在 VM 生命周期内的任意时刻加入或移出 VM，而不只在初始引导时。)

With this approach, the vPCI device is a VMBus device and a
PCI device at the same time.  In response to the VMBus offer
message, the hv_pci_probe() function runs and establishes a
VMBus connection to the vPCI VSP on the Hyper-V host.  That
connection has a single VMBus channel.  The channel is used to
exchange messages with the vPCI VSP for the purpose of setting
up and configuring the vPCI device in Linux.  Once the device
is fully configured in Linux as a PCI device, the VMBus
channel is used only if Linux changes the vCPU to be interrupted
in the guest, or if the vPCI device is removed from
the VM while the VM is running.  The ongoing operation of the
device happens directly between the Linux device driver for
the device and the hardware, with VMBus and the VMBus channel
playing no role.
(在这种方式下，vPCI 设备同时是 VMBus 设备和 PCI 设备。响应 VMBus offer 消息时，hv_pci_probe() 函数运行，并与 Hyper-V 主机上的 vPCI VSP 建立 VMBus 连接。该连接只有单个 VMBus 通道。该通道用于与 vPCI VSP 交换消息，以便在 Linux 中设置和配置 vPCI 设备。一旦设备在 Linux 中作为 PCI 设备完成全部配置，VMBus 通道就只在两种情况下使用：Linux 更改客户机中被中断的 vCPU，或在 VM 运行期间 vPCI 设备被移出 VM。设备的持续运行直接发生在该设备的 Linux 设备驱动与硬件之间，VMBus 及 VMBus 通道不再参与。)

PCI Device Setup(PCI 设备设置)
------------------------------
PCI device setup follows a sequence that Hyper-V originally
created for Windows guests, and that can be ill-suited for
Linux guests due to differences in the overall structure of
the Linux PCI subsystem compared with Windows.  Nonetheless,
with a bit of hackery in the Hyper-V virtual PCI driver for
Linux, the virtual PCI device is setup in Linux so that
generic Linux PCI subsystem code and the Linux driver for the
device "just work".
(PCI 设备设置遵循 Hyper-V 最初为 Windows 客户机设计的流程；由于 Linux PCI 子系统的整体结构与 Windows 不同，这一流程可能并不适合 Linux 客户机。尽管如此，通过在面向 Linux 的 Hyper-V 虚拟 PCI 驱动中做一些变通处理，虚拟 PCI 设备在 Linux 中的设置使得通用 Linux PCI 子系统代码与设备对应的 Linux 驱动能够"开箱即用"。)

Each vPCI device is set up in Linux to be in its own PCI
domain with a host bridge.  The PCI domainID is derived from
bytes 4 and 5 of the instance GUID assigned to the VMBus vPCI
device.  The Hyper-V host does not guarantee that these bytes
are unique, so hv_pci_probe() has an algorithm to resolve
collisions.  The collision resolution is intended to be stable
across reboots of the same VM so that the PCI domainIDs don't
change, as the domainID appears in the user space
configuration of some devices.
(每个 vPCI 设备在 Linux 中都被设置为位于自己的 PCI 域（domain）中，并配有宿主桥。PCI domainID 由分配给 VMBus vPCI 设备的实例 GUID 的第 4、5 字节派生。Hyper-V 主机并不保证这些字节唯一，因此 hv_pci_probe() 有一个解决冲突的算法。冲突消解力求在同一 VM 重启后保持稳定，使 PCI domainID 不变，因为 domainID 会出现在某些设备的用户空间配置中。)

hv_pci_probe() allocates a guest MMIO range to be used as PCI
config space for the device.  This MMIO range is communicated
to the Hyper-V host over the VMBus channel as part of telling
the host that the device is ready to enter d0.  See
hv_pci_enter_d0().  When the guest subsequently accesses this
MMIO range, the Hyper-V host intercepts the accesses and maps
them to the physical device PCI config space.
(hv_pci_probe() 分配一段客户机 MMIO 范围，用作该设备的 PCI 配置空间。在告知主机设备已准备好进入 d0 时，这段 MMIO 范围通过 VMBus 通道通知 Hyper-V 主机。参见 hv_pci_enter_d0()。当客户机随后访问这段 MMIO 范围时，Hyper-V 主机会拦截这些访问，并把它们映射到物理设备的 PCI 配置空间。)

hv_pci_probe() also gets BAR information for the device from
the Hyper-V host, and uses this information to allocate MMIO
space for the BARs.  That MMIO space is then setup to be
associated with the host bridge so that it works when generic
PCI subsystem code in Linux processes the BARs.
(hv_pci_probe() 还从 Hyper-V 主机获取设备的 BAR 信息，并据此为 BAR 分配 MMIO 空间。随后该 MMIO 空间被设置为与宿主桥关联，使 Linux 中通用 PCI 子系统代码处理 BAR 时能够正常工作。)

Finally, hv_pci_probe() creates the root PCI bus.  At this
point the Hyper-V virtual PCI driver hackery is done, and the
normal Linux PCI machinery for scanning the root bus works to
detect the device, to perform driver matching, and to
initialize the driver and device.
(最后，hv_pci_probe() 创建根 PCI 总线。至此，Hyper-V 虚拟 PCI 驱动的变通处理完成，用于扫描根总线的常规 Linux PCI 机制开始工作：检测设备、执行驱动匹配，并初始化驱动与设备。)

PCI Device Removal(PCI 设备移除)
--------------------------------
A Hyper-V host may initiate removal of a vPCI device from a
guest VM at any time during the life of the VM.  The removal
is instigated by an admin action taken on the Hyper-V host and
is not under the control of the guest OS.
(Hyper-V 主机可以在 VM 生命周期内的任意时刻发起从客户机 VM 中移除 vPCI 设备。移除由在 Hyper-V 主机上执行的管理操作触发，不受客户机操作系统控制。)

A guest VM is notified of the removal by an unsolicited
"Eject" message sent from the host to the guest over the VMBus
channel associated with the vPCI device.  Upon receipt of such
a message, the Hyper-V virtual PCI driver in Linux
asynchronously invokes Linux kernel PCI subsystem calls to
shutdown and remove the device.  When those calls are
complete, an "Ejection Complete" message is sent back to
Hyper-V over the VMBus channel indicating that the device has
been removed.  At this point, Hyper-V sends a VMBus rescind
message to the Linux guest, which the VMBus driver in Linux
processes by removing the VMBus identity for the device.  Once
that processing is complete, all vestiges of the device having
been present are gone from the Linux kernel.  The rescind
message also indicates to the guest that Hyper-V has stopped
providing support for the vPCI device in the guest.  If the
guest were to attempt to access that device's MMIO space, it
would be an invalid reference. Hypercalls affecting the device
return errors, and any further messages sent in the VMBus
channel are ignored.
(客户机 VM 通过主机经与该 vPCI 设备关联的 VMBus 通道主动发送的"Eject"消息获知移除。收到此类消息后，Linux 中的 Hyper-V 虚拟 PCI 驱动异步调用 Linux 内核 PCI 子系统调用来关闭并移除设备。这些调用完成后，一条"Ejection Complete"消息经 VMBus 通道发回 Hyper-V，表明设备已被移除。此时，Hyper-V 向 Linux 客户机发送 VMBus rescind 消息，Linux 中的 VMBus 驱动通过移除该设备的 VMBus 身份来处理它。处理完成后，该设备曾经存在的所有痕迹都从 Linux 内核中消失。rescind 消息还向客户机表明 Hyper-V 已停止在客户机中为该 vPCI 设备提供支持。如果客户机试图访问该设备的 MMIO 空间，将是无效引用。影响该设备的 hypercall 返回错误，VMBus 通道中后续发送的任何消息都被忽略。)

After sending the Eject message, Hyper-V allows the guest VM
60 seconds to cleanly shutdown the device and respond with
Ejection Complete before sending the VMBus rescind
message.  If for any reason the Eject steps don't complete
within the allowed 60 seconds, the Hyper-V host forcibly
performs the rescind steps, which will likely result in
cascading errors in the guest because the device is now no
longer present from the guest standpoint and accessing the
device MMIO space will fail.
(发送 Eject 消息后，Hyper-V 允许客户机 VM 在 60 秒内干净地关闭设备并以 Ejection Complete 应答，之后才发送 VMBus rescind 消息。如果由于任何原因 Eject 步骤未在允许的 60 秒内完成，Hyper-V 主机会强制执行 rescind 步骤——这很可能在客户机中引发连锁错误，因为从客户机角度看设备已不存在，访问设备 MMIO 空间将会失败。)

Because ejection is asynchronous and can happen at any point
during the guest VM lifecycle, proper synchronization in the
Hyper-V virtual PCI driver is very tricky.  Ejection has been
observed even before a newly offered vPCI device has been
fully setup.  The Hyper-V virtual PCI driver has been updated
several times over the years to fix race conditions when
ejections happen at inopportune times. Care must be taken when
modifying this code to prevent re-introducing such problems.
See comments in the code.
(由于弹出是异步的、可能发生在客户机 VM 生命周期的任意时刻，Hyper-V 虚拟 PCI 驱动中的正确同步非常棘手。甚至观察到新提供的 vPCI 设备尚未完全设置好就发生了弹出。多年来 Hyper-V 虚拟 PCI 驱动已多次更新，以修复在不恰当时机发生弹出时的竞态条件。修改这段代码时必须小心，避免再次引入此类问题。参见代码中的注释。)

Interrupt Assignment(中断分配)
------------------------------
The Hyper-V virtual PCI driver supports vPCI devices using
MSI, multi-MSI, or MSI-X.  Assigning the guest vCPU that will
receive the interrupt for a particular MSI or MSI-X message is
complex because of the way the Linux setup of IRQs maps onto
the Hyper-V interfaces.  For the single-MSI and MSI-X cases,
Linux calls hv_compse_msi_msg() twice, with the first call
containing a dummy vCPU and the second call containing the
real vCPU.  Furthermore, hv_irq_unmask() is finally called
(on x86) or the GICD registers are set (on arm64) to specify
the real vCPU again.  Each of these three calls interact
with Hyper-V, which must decide which physical CPU should
receive the interrupt before it is forwarded to the guest VM.
Unfortunately, the Hyper-V decision-making process is a bit
limited, and can result in concentrating the physical
interrupts on a single CPU, causing a performance bottleneck.
See details about how this is resolved in the extensive
comment above the function hv_compose_msi_req_get_cpu().
(Hyper-V 虚拟 PCI 驱动支持使用 MSI、multi-MSI 或 MSI-X 的 vPCI 设备。为特定 MSI 或 MSI-X 消息分配接收中断的客户机 vCPU 是复杂的，因为 Linux 设置 IRQ 的方式映射到 Hyper-V 接口上的方式所致。对单 MSI 与 MSI-X 的情况，Linux 调用 hv_compse_msi_msg() 两次：第一次带一个占位 vCPU，第二次带真实 vCPU。此外，最后还会调用 hv_irq_unmask()（x86 上）或设置 GICD 寄存器（arm64 上）再次指定真实 vCPU。这三次调用都会与 Hyper-V 交互，Hyper-V 必须在中断转发给客户机 VM 之前决定由哪个物理 CPU 接收该中断。遗憾的是，Hyper-V 的决策过程有些受限，可能导致物理中断集中在单个 CPU 上，造成性能瓶颈。如何解决此问题的细节，参见 hv_compose_msi_req_get_cpu() 函数上方的大段注释。)

The Hyper-V virtual PCI driver implements the
irq_chip.irq_compose_msi_msg function as hv_compose_msi_msg().
Unfortunately, on Hyper-V the implementation requires sending
a VMBus message to the Hyper-V host and awaiting an interrupt
indicating receipt of a reply message.  Since
irq_chip.irq_compose_msi_msg can be called with IRQ locks
held, it doesn't work to do the normal sleep until awakened by
the interrupt. Instead hv_compose_msi_msg() must send the
VMBus message, and then poll for the completion message. As
further complexity, the vPCI device could be ejected/rescinded
while the polling is in progress, so this scenario must be
detected as well.  See comments in the code regarding this
very tricky area.
(Hyper-V 虚拟 PCI 驱动把 irq_chip.irq_compose_msi_msg 函数实现为 hv_compose_msi_msg()。遗憾的是，在 Hyper-V 上该实现需要向 Hyper-V 主机发送 VMBus 消息，并等待指示已收到应答消息的中断。由于 irq_chip.irq_compose_msi_msg 可能 在持有 IRQ 锁的情况下被调用，采用"睡眠直到被中断唤醒"的常规做法行不通。因此 hv_compose_msi_msg() 必须发送 VMBus 消息，然后轮询完成消息。更复杂的是，轮询进行期间 vPCI 设备可能被弹出/rescind，所以还必须检测这种情形。关于这个非常棘手的领域，参见代码中的注释。)

Most of the code in the Hyper-V virtual PCI driver (pci-
hyperv.c) applies to Hyper-V and Linux guests running on x86
and on arm64 architectures.  But there are differences in how
interrupt assignments are managed.  On x86, the Hyper-V
virtual PCI driver in the guest must make a hypercall to tell
Hyper-V which guest vCPU should be interrupted by each
MSI/MSI-X interrupt, and the x86 interrupt vector number that
the x86_vector IRQ domain has picked for the interrupt.  This
hypercall is made by hv_arch_irq_unmask().  On arm64, the
Hyper-V virtual PCI driver manages the allocation of an SPI
for each MSI/MSI-X interrupt.  The Hyper-V virtual PCI driver
stores the allocated SPI in the architectural GICD registers,
which Hyper-V emulates, so no hypercall is necessary as with
x86.  Hyper-V does not support using LPIs for vPCI devices in
arm64 guest VMs because it does not emulate a GICv3 ITS.
(Hyper-V 虚拟 PCI 驱动（pci-hyperv.c）的大部分代码同时适用于运行在 x86 与 arm64 体系结构上的 Hyper-V 和 Linux 客户机。但中断分配的管理方式有所不同。在 x86 上，客户机中的 Hyper-V 虚拟 PCI 驱动必须发起 hypercall，告知 Hyper-V 每个 MSI/MSI-X 中断应中断哪个客户机 vCPU，以及 x86_vector IRQ 域为该中断选定的 x86 中断向量号。该 hypercall 由 hv_arch_irq_unmask() 发起。在 arm64 上，Hyper-V 虚拟 PCI 驱动管理为每个 MSI/MSI-X 中断分配 SPI 的工作。Hyper-V 虚拟 PCI 驱动把分配的 SPI 存储在体系结构的 GICD 寄存器中（由 Hyper-V 模拟），因此无需像 x86 那样发起 hypercall。Hyper-V 不支持在 arm64 客户机 VM 中对 vPCI 设备使用 LPI，因为它不模拟 GICv3 ITS。)

The Hyper-V virtual PCI driver in Linux supports vPCI devices
whose drivers create managed or unmanaged Linux IRQs.  If the
smp_affinity for an unmanaged IRQ is updated via the /proc/irq
interface, the Hyper-V virtual PCI driver is called to tell
the Hyper-V host to change the interrupt targeting and
everything works properly.  However, on x86 if the x86_vector
IRQ domain needs to reassign an interrupt vector due to
running out of vectors on a CPU, there's no path to inform the
Hyper-V host of the change, and things break.  Fortunately,
guest VMs operate in a constrained device environment where
using all the vectors on a CPU doesn't happen. Since such a
problem is only a theoretical concern rather than a practical
concern, it has been left unaddressed.
(Linux 中的 Hyper-V 虚拟 PCI 驱动支持其驱动创建托管或非托管 Linux IRQ 的 vPCI 设备。如果通过 /proc/irq 接口更新非托管 IRQ 的 smp_affinity，Hyper-V 虚拟 PCI 驱动会被调用，告知 Hyper-V 主机更改中断定向，一切正常。然而在 x86 上，如果 x86_vector IRQ 域因为某 CPU 的向量耗尽而需要重新分配中断向量，则没有路径把这一变化告知 Hyper-V 主机，事情就会出错。幸运的是，客户机 VM 运行在受约束的设备环境中，不会把一个 CPU 上的向量用光。由于这类问题只是理论上的担忧而非现实问题，故未予处理。)

DMA(DMA)
--------
By default, Hyper-V pins all guest VM memory in the host
when the VM is created, and programs the physical IOMMU to
allow the VM to have DMA access to all its memory.  Hence
it is safe to assign PCI devices to the VM, and allow the
guest operating system to program the DMA transfers.  The
physical IOMMU prevents a malicious guest from initiating
DMA to memory belonging to the host or to other VMs on the
host. From the Linux guest standpoint, such DMA transfers
are in "direct" mode since Hyper-V does not provide a virtual
IOMMU in the guest.
(默认情况下，创建 VM 时 Hyper-V 会把客户机 VM 的全部内存固定（pin）在宿主机中，并编程物理 IOMMU 允许 VM 对其全部内存进行 DMA 访问。因此把 PCI 设备分配给 VM、并允许客户机操作系统编程 DMA 传输是安全的。物理 IOMMU 防止恶意客户机对属于宿主机或宿主机上其他 VM 的内存发起 DMA。从 Linux 客户机的角度看，这类 DMA 传输处于"直接"（direct）模式，因为 Hyper-V 不在客户机中提供虚拟 IOMMU。)

Hyper-V assumes that physical PCI devices always perform
cache-coherent DMA.  When running on x86, this behavior is
required by the architecture.  When running on arm64, the
architecture allows for both cache-coherent and
non-cache-coherent devices, with the behavior of each device
specified in the ACPI DSDT.  But when a PCI device is assigned
to a guest VM, that device does not appear in the DSDT, so the
Hyper-V VMBus driver propagates cache-coherency information
from the VMBus node in the ACPI DSDT to all VMBus devices,
including vPCI devices (since they have a dual identity as a VMBus
device and as a PCI device).  See vmbus_dma_configure().
Current Hyper-V versions always indicate that the VMBus is
cache coherent, so vPCI devices on arm64 always get marked as
cache coherent and the CPU does not perform any sync
operations as part of dma_map/unmap_*() calls.
(Hyper-V 假定物理 PCI 设备总是执行缓存一致的 DMA。在 x86 上运行时，这是体系结构的要求。在 arm64 上运行时，体系结构允许缓存一致与非缓存一致的设备并存，每个设备的行为在 ACPI DSDT 中指定。但当 PCI 设备被分配给客户机 VM 时，该设备不会出现在 DSDT 中，因此 Hyper-V VMBus 驱动把 ACPI DSDT 中 VMBus 节点的缓存一致性信息传递给所有 VMBus 设备，包括 vPCI 设备（因为它们兼具 VMBus 设备与 PCI 设备的双重身份）。参见 vmbus_dma_configure()。当前的 Hyper-V 版本总是表明 VMBus 是缓存一致的，因此 arm64 上的 vPCI 设备总是被标记为缓存一致，CPU 不会在 dma_map/unmap_*() 调用中执行任何同步操作。)

vPCI protocol versions(vPCI 协议版本)
-------------------------------------
As previously described, during vPCI device setup and teardown
messages are passed over a VMBus channel between the Hyper-V
host and the Hyper-v vPCI driver in the Linux guest.  Some
messages have been revised in newer versions of Hyper-V, so
the guest and host must agree on the vPCI protocol version to
be used.  The version is negotiated when communication over
the VMBus channel is first established.  See
hv_pci_protocol_negotiation(). Newer versions of the protocol
extend support to VMs with more than 64 vCPUs, and provide
additional information about the vPCI device, such as the
guest virtual NUMA node to which it is most closely affined in
the underlying hardware.
(如前所述，在 vPCI 设备的设置与拆除过程中，消息经 VMBus 通道在 Hyper-V 主机与 Linux 客户机中的 Hyper-V vPCI 驱动之间传递。较新版本的 Hyper-V 对一些消息做了修订，因此客户机与主机必须就所使用的 vPCI 协议版本达成一致。版本在首次通过 VMBus 通道建立通信时协商。参见 hv_pci_protocol_negotiation()。较新的协议版本把支持扩展到超过 64 个 vCPU 的 VM，并提供关于 vPCI 设备的额外信息，例如它在底层硬件中最紧密亲和的客户机虚拟 NUMA 节点。)

Guest NUMA node affinity(客户机 NUMA 节点亲和性)
------------------------------------------------
When the vPCI protocol version provides it, the guest NUMA
node affinity of the vPCI device is stored as part of the Linux
device information for subsequent use by the Linux driver. See
hv_pci_assign_numa_node().  If the negotiated protocol version
does not support the host providing NUMA affinity information,
the Linux guest defaults the device NUMA node to 0.  But even
when the negotiated protocol version includes NUMA affinity
information, the ability of the host to provide such
information depends on certain host configuration options.  If
the guest receives NUMA node value "0", it could mean NUMA
node 0, or it could mean "no information is available".
Unfortunately it is not possible to distinguish the two cases
from the guest side.
(当 vPCI 协议版本提供该能力时，vPCI 设备的客户机 NUMA 节点亲和性会作为 Linux 设备信息的一部分保存，供 Linux 驱动后续使用。参见 hv_pci_assign_numa_node()。如果协商的协议版本不支持主机提供 NUMA 亲和性信息，Linux 客户机把设备 NUMA 节点默认为 0。但即使协商的协议版本包含 NUMA 亲和性信息，主机能否提供此类信息也取决于某些主机配置选项。如果客户机收到的 NUMA 节点值为 "0"，它可能表示 NUMA 节点 0，也可能表示"无可用信息"。遗憾的是，从客户机一侧无法区分这两种情况。)

PCI config space access in a CoCo VM(CoCo VM 中的 PCI 配置空间访问)
-------------------------------------------------------------------
Linux PCI device drivers access PCI config space using a
standard set of functions provided by the Linux PCI subsystem.
In Hyper-V guests these standard functions map to functions
hv_pcifront_read_config() and hv_pcifront_write_config()
in the Hyper-V virtual PCI driver.  In normal VMs,
these hv_pcifront_*() functions directly access the PCI config
space, and the accesses trap to Hyper-V to be handled.
But in CoCo VMs, memory encryption prevents Hyper-V
from reading the guest instruction stream to emulate the
access, so the hv_pcifront_*() functions must invoke
hypercalls with explicit arguments describing the access to be
made.
(Linux PCI 设备驱动使用 Linux PCI 子系统提供的一组标准函数访问 PCI 配置空间。在 Hyper-V 客户机中，这些标准函数映射到 Hyper-V 虚拟 PCI 驱动中的 hv_pcifront_read_config() 与 hv_pcifront_write_config() 函数。在普通 VM 中，这些 hv_pcifront_*() 函数直接访问 PCI 配置空间，访问陷入（trap）Hyper-V 由其处理。但在 CoCo VM 中，内存加密使 Hyper-V 无法读取客户机指令流来模拟访问，因此 hv_pcifront_*() 函数必须发起 hypercall，用显式参数描述要进行的访问。)

Config Block back-channel(Config Block 反向通道)
------------------------------------------------
The Hyper-V host and Hyper-V virtual PCI driver in Linux
together implement a non-standard back-channel communication
path between the host and guest.  The back-channel path uses
messages sent over the VMBus channel associated with the vPCI
device.  The functions hyperv_read_cfg_blk() and
hyperv_write_cfg_blk() are the primary interfaces provided to
other parts of the Linux kernel.  As of this writing, these
interfaces are used only by the Mellanox mlx5 driver to pass
diagnostic data to a Hyper-V host running in the Azure public
cloud.  The functions hyperv_read_cfg_blk() and
hyperv_write_cfg_blk() are implemented in a separate module
(pci-hyperv-intf.c, under CONFIG_PCI_HYPERV_INTERFACE) that
effectively stubs them out when running in non-Hyper-V
environments.
(Hyper-V 主机与 Linux 中的 Hyper-V 虚拟 PCI 驱动共同实现了主机与客户机之间一条非标准的反向通道（back-channel）通信路径。反向通道路径使用经与该 vPCI 设备关联的 VMBus 通道发送的消息。函数 hyperv_read_cfg_blk() 与 hyperv_write_cfg_blk() 是提供给 Linux 内核其他部分的主要接口。截至撰写本文时，这些接口只被 Mellanox mlx5 驱动使用，用于向运行在 Azure 公有云中的 Hyper-V 主机传递诊断数据。hyperv_read_cfg_blk() 与 hyperv_write_cfg_blk() 函数实现在一个独立模块（pci-hyperv-intf.c，位于 CONFIG_PCI_HYPERV_INTERFACE 下）中，在非 Hyper-V 环境中运行时该模块实际上会把它们置为空桩。)
