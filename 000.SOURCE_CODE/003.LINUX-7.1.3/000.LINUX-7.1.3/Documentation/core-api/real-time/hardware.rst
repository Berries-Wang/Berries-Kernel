.. SPDX-License-Identifier: GPL-2.0

==============================
Considering hardware(硬件考量)
==============================

:Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>

(作者: Sebastian Andrzej Siewior <bigeasy@linutronix.de>)

The way a workload is handled can be influenced by the hardware it runs on.
Key components include the CPU, memory, and the buses that connect them.
These resources are shared among all applications on the system.
As a result, heavy utilization of one resource by a single application
can affect the deterministic handling of workloads in other applications.
(工作负载的处理方式会受到其运行硬件的影响。关键组件包括 CPU、内存以及连接它们的总线。这些资源由系统上的所有应用程序共享。因此，单个应用程序对某一资源的大量占用可能会影响其他应用程序中工作负载的确定性处理。)

Below is a brief overview.
(以下是一个简要概览。)

System memory and cache(系统内存与缓存)
---------------------------------------

Main memory and the associated caches are the most common shared resources among
tasks in a system. One task can dominate the available caches, forcing another
task to wait until a cache line is written back to main memory before it can
proceed. The impact of this contention varies based on write patterns and the
size of the caches available. Larger caches may reduce stalls because more lines
can be buffered before being written back. Conversely, certain write patterns
may trigger the cache controller to flush many lines at once, causing
applications to stall until the operation completes.
(主内存和相关的缓存是系统中任务之间最常见的共享资源。一个任务可能独占可用的缓存，迫使另一个任务在缓存行被写回主内存之前等待而无法继续。这种争用的影响因写入模式和可用缓存的大小而异。更大的缓存可以减少停顿，因为在写回之前可以缓冲更多缓存行。相反，某些写入模式可能触发缓存控制器一次刷新大量缓存行，导致应用程序停顿直到操作完成。)

This issue can be partly mitigated if applications do not share the same CPU
cache. The kernel is aware of the cache topology and exports this information to
user space. Tools such as **lstopo** from the Portable Hardware Locality (hwloc)
project (https://www.open-mpi.org/projects/hwloc/) can visualize the hierarchy.
(如果应用程序不共享同一个 CPU 缓存，这个问题可以得到部分缓解。内核知晓缓存拓扑，并将该信息导出到用户空间。便携式硬件本地化（hwloc）项目的 **lstopo** 等工具（https://www.open-mpi.org/projects/hwloc/）可以可视化这一层级结构。)

Avoiding shared L2 or L3 caches is not always possible. Even when cache sharing
is minimized, bottlenecks can still occur when accessing system memory. Memory
is used not only by the CPU but also by peripheral devices via DMA, such as
graphics cards or network adapters.
(避免共享 L2 或 L3 缓存并不总是可行。即使缓存共享被最小化，访问系统内存时仍可能出现瓶颈。内存不仅被 CPU 使用，还被外围设备通过 DMA 使用，例如显卡或网络适配器。)

In some cases, cache and memory bottlenecks can be controlled if the hardware
provides the necessary support. On x86 systems, Intel offers Cache Allocation
Technology (CAT), which enables cache partitioning among applications and
provides control over the interconnect. AMD provides similar functionality under
Platform Quality of Service (PQoS). On Arm64, the equivalent is Memory
System Resource Partitioning and Monitoring (MPAM).
(在某些情况下，如果硬件提供必要的支持，缓存和内存瓶颈是可以控制的。在 x86 系统上，Intel 提供缓存分配技术（CAT），可在应用程序之间进行缓存分区，并提供对互连的控制。AMD 在平台服务质量（PQoS）之下提供类似功能。在 Arm64 上，对应的是内存系统资源分区与监控（MPAM）。)

These features can be configured through the Linux Resource Control interface.
For details, see Documentation/filesystems/resctrl.rst.
(这些特性可以通过 Linux 资源控制接口进行配置。详情参见 Documentation/filesystems/resctrl.rst。)

The perf tool can be used to monitor cache behavior. It can analyze
cache misses of an application and compare how they change under
different workloads on a neighboring CPU. Even more powerful, the perf
c2c tool can help identify cache-to-cache issues, where multiple CPU
cores repeatedly access and modify data on the same cache line.
(perf 工具可用于监控缓存行为。它可以分析应用程序的缓存未命中情况，并比较它们在相邻 CPU 上不同负载下的变化。更强大的是，perf c2c 工具可以帮助识别缓存到缓存的问题，即多个 CPU 核心反复访问并修改同一缓存行上的数据。)

Hardware buses(硬件总线)
------------------------

Real-time systems often need to access hardware directly to perform their work.
Any latency in this process is undesirable, as it can affect the outcome of the
task. For example, on an I/O bus, a changed output may not become immediately
visible but instead appear with variable delay depending on the latency of the
bus used for communication.
(实时系统通常需要直接访问硬件来完成其工作。此过程中的任何延迟都是不可取的，因为它会影响任务的结果。例如，在 I/O 总线上，更改后的输出可能不会立即变为可见，而是根据用于通信的总线的延迟，以可变延迟出现。)

A bus such as PCI is relatively simple because register accesses are routed
directly to the connected device. In the worst case, a read operation stalls the
CPU until the device responds.
(像 PCI 这样的总线相对简单，因为寄存器访问被直接路由到所连接的设备。在最坏的情况下，读操作会使 CPU 停顿，直到设备响应。)

A bus such as USB is more complex, involving multiple layers. A register read
or write is wrapped in a USB Request Block (URB), which is then sent by the
USB host controller to the device. Timing and latency are influenced by the
underlying USB bus. Requests cannot be sent immediately; they must align with
the next frame boundary according to the endpoint type and the host controller's
scheduling rules. This can introduce delays and additional latency. For example,
a network device connected via USB may still deliver sufficient throughput, but
the added latency when sending or receiving packets may fail to meet the
requirements of certain real-time use cases.
(像 USB 这样的总线更为复杂，涉及多个层次。寄存器读或写被封装在 USB 请求块（URB）中，然后由 USB 主机控制器发送到设备。时序和延迟受底层 USB 总线的影响。请求无法立即发送；它们必须根据端点类型和主机控制器的调度规则与下一帧边界对齐。这会引入延迟和额外的等待。例如，通过 USB 连接的网络设备可能仍能提供足够的吞吐量，但发送或接收数据包时增加的延迟可能无法满足某些实时用例的要求。)

Additional restrictions on bus latency can arise from power management. For
instance, PCIe with Active State Power Management (ASPM) enabled can suspend
the link between the device and the host. While this behavior is beneficial for
power savings, it delays device access and adds latency to responses. This issue
is not limited to PCIe; internal buses within a System-on-Chip (SoC) can also be
affected by power management mechanisms.
(电源管理也会对总线延迟施加额外限制。例如，启用了活动状态电源管理（ASPM）的 PCIe 可以挂起设备与主机之间的链路。虽然这种行为有利于节省功耗，但它延迟了设备访问并增加了响应延迟。这个问题不仅限于 PCIe；片上系统（SoC）内部的总线也可能受到电源管理机制的影响。)

Virtualization(虚拟化)
----------------------

In a virtualized environment such as KVM, each guest CPU is represented as a
thread on the host. If such a thread runs with real-time priority, the system
should be tested to confirm it can sustain this behavior over extended periods.
Because of its priority, the thread will not be preempted by lower-priority
threads (such as SCHED_OTHER), which may then receive no CPU time. This can
cause problems if a lower-priority thread is pinned to a CPU already occupied by
a real-time task and unable to make progress. Even if a CPU has been isolated,
the system may still (accidentally) start a per‑CPU thread on that CPU.
Ensuring that a guest CPU goes idle is difficult, as it requires avoiding both
task scheduling and interrupt handling. Furthermore, if the guest CPU does go
idle but the guest system is booted with the option **idle=poll**, the guest
CPU will never enter an idle state and will instead spin until an event
arrives.
(在诸如 KVM 之类的虚拟化环境中，每个客户 CPU 在主机上表示为一个线程。如果这样的线程以实时优先级运行，应对系统进行测试，确认它能在长时间内维持这种行为。由于其优先级，该线程不会被低优先级线程（例如 SCHED_OTHER）抢占，后者可能因此得不到 CPU 时间。如果一个低优先级线程被固定在一个已被实时任务占用的 CPU 上而无法取得进展，这可能会造成问题。即使某个 CPU 已被隔离，系统仍可能（意外地）在该 CPU 上启动 per-CPU 线程。要确保客户 CPU 进入空闲状态非常困难，因为这既要求避免任务调度，又要求避免中断处理。此外，如果客户 CPU 确实进入空闲状态，但客户系统是用 **idle=poll** 选项启动的，那么客户 CPU 永远不会进入空闲状态，而是会在事件到来之前持续空转。)

Device handling introduces additional considerations. Emulated PCI devices or
VirtIO devices require a counterpart on the host to complete requests. This
adds latency because the host must intercept and either process the request
directly or schedule a thread for its completion. These delays can be avoided if
the required PCI device is passed directly through to the guest. Some devices,
such as networking or storage controllers, support the PCIe SR-IOV feature.
SR-IOV allows a single PCIe device to be divided into multiple virtual functions,
which can then be assigned to different guests.
(设备处理还引入了额外的考量。模拟的 PCI 设备或 VirtIO 设备需要在主机上有对应的实体来完成请求。这会增加延迟，因为主机必须拦截请求，然后直接处理它或调度一个线程来完成它。如果所需的 PCI 设备被直接直通给客户机，则可以避免这些延迟。某些设备（例如网络或存储控制器）支持 PCIe SR-IOV 特性。SR-IOV 允许将单个 PCIe 设备划分为多个虚拟功能，然后可将它们分配给不同的客户机。)

Networking(网络)
----------------

For low-latency networking, the full networking stack may be undesirable, as it
can introduce additional sources of delay. In this context, XDP can be used
as a shortcut to bypass much of the stack while still relying on the kernel's
network driver.
(对于低延迟网络，完整的网络协议栈可能是不可取的，因为它会引入额外的延迟来源。在这种场景下，XDP 可以作为一条捷径，在绕过协议栈大部分内容的同时仍依赖内核的网络驱动。)

The requirements are that the network driver must support XDP- preferably using
an "skb pool" and that the application must use an XDP socket. Additional
configuration may involve BPF filters, tuning networking queues, or configuring
qdiscs for time-based transmission. These techniques are often
applied in Time-Sensitive Networking (TSN) environments.
(要求是：网络驱动必须支持 XDP——最好使用 "skb pool"（skb 池），并且应用程序必须使用 XDP socket。额外的配置可能涉及 BPF 过滤器、调整网络队列，或为基于时间的传输配置 qdisc。这些技术通常应用于时间敏感网络（TSN）环境中。)

Documenting all required steps exceeds the scope of this text. For detailed
guidance, see the TSN documentation at https://tsn.readthedocs.io.
(完整记录所有所需步骤超出了本文的范围。详细指南参见 TSN 文档 https://tsn.readthedocs.io。)

Another useful resource is the Linux Real-Time Communication Testbench
https://github.com/Linutronix/RTC-Testbench.
The goal of this project is to validate real-time network communication. It can
be thought of as a "cyclictest" for networking and also serves as a starting
point for application development.
(另一个有用的资源是 Linux 实时通信测试平台 https://github.com/Linutronix/RTC-Testbench。该项目的目标是验证实时网络通信。它可以被看作网络领域的 "cyclictest"，也可作为应用开发的起点。)
