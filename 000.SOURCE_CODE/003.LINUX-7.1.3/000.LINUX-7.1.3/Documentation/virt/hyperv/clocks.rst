.. SPDX-License-Identifier: GPL-2.0

===============================
Clocks and Timers(时钟与定时器)
===============================

arm64(arm64)
------------
On arm64, Hyper-V virtualizes the ARMv8 architectural system counter
and timer. Guest VMs use this virtualized hardware as the Linux
clocksource and clockevents via the standard arm_arch_timer.c
driver, just as they would on bare metal. Linux vDSO support for the
architectural system counter is functional in guest VMs on Hyper-V.
While Hyper-V also provides a synthetic system clock and four synthetic
per-CPU timers as described in the TLFS, they are not used by the
Linux kernel in a Hyper-V guest on arm64.  However, older versions
of Hyper-V for arm64 only partially virtualize the ARMv8
architectural timer, such that the timer does not generate
interrupts in the VM. Because of this limitation, running current
Linux kernel versions on these older Hyper-V versions requires an
out-of-tree patch to use the Hyper-V synthetic clocks/timers instead.
(在 arm64 上，Hyper-V 虚拟化了 ARMv8 架构的系统计数器与定时器。客户机
VM 通过标准的 arm_arch_timer.c 驱动，把这个虚拟化的硬件用作 Linux 的
clocksource 与 clockevents，就像在裸机上一样。架构系统计数器的 Linux
vDSO 支持在 Hyper-V 的客户机 VM 中可用。虽然 Hyper-V 还按 TLFS 的描述
提供了合成系统时钟和四个合成的 per-CPU 定时器，但 arm64 的 Hyper-V 客
户机中的 Linux 内核并不使用它们。不过，arm64 的旧版本 Hyper-V 只部分
虚拟化了 ARMv8 架构定时器，使得该定时器不会在 VM 中产生中断。由于这
一限制，在这些旧版 Hyper-V 上运行较新的 Linux 内核版本，需要使用一个
树外 (out-of-tree) 补丁来改用 Hyper-V 合成时钟/定时器。)

x86/x64(x86/x64)
----------------
On x86/x64, Hyper-V provides guest VMs with a synthetic system clock
and four synthetic per-CPU timers as described in the TLFS. Hyper-V
also provides access to the virtualized TSC via the RDTSC and
related instructions. These TSC instructions do not trap to
the hypervisor and so provide excellent performance in a VM.
Hyper-V performs TSC calibration, and provides the TSC frequency
to the guest VM via a synthetic MSR.  Hyper-V initialization code
in Linux reads this MSR to get the frequency, so it skips TSC
calibration and sets tsc_reliable. Hyper-V provides virtualized
versions of the PIT (in Hyper-V  Generation 1 VMs only), local
APIC timer, and RTC. Hyper-V does not provide a virtualized HPET in
guest VMs.
(在 x86/x64 上，Hyper-V 按 TLFS 的描述为客户机 VM 提供合成系统时钟和
四个合成的 per-CPU 定时器。Hyper-V 还通过 RDTSC 及相关指令提供对虚拟
化 TSC 的访问。这些 TSC 指令不会陷入 (trap) 管理程序，因此在 VM 中提
供出色的性能。Hyper-V 执行 TSC 校准，并通过一个合成 MSR 把 TSC 频率提
供给客户机 VM。Linux 中的 Hyper-V 初始化代码读取该 MSR 以获取频率，
因此跳过 TSC 校准并设置 tsc_reliable。Hyper-V 提供 PIT (仅在 Hyper-V
第 1 代 VM 中)、本地 APIC 定时器和 RTC 的虚拟化版本。Hyper-V 不为客户
机 VM 提供虚拟化的 HPET。)

The Hyper-V synthetic system clock can be read via a synthetic MSR,
but this access traps to the hypervisor. As a faster alternative,
the guest can configure a memory page to be shared between the guest
and the hypervisor.  Hyper-V populates this memory page with a
64-bit scale value and offset value. To read the synthetic clock
value, the guest reads the TSC and then applies the scale and offset
as described in the Hyper-V TLFS. The resulting value advances
at a constant 10 MHz frequency. In the case of a live migration
to a host with a different TSC frequency, Hyper-V adjusts the
scale and offset values in the shared page so that the 10 MHz
frequency is maintained.
(Hyper-V 合成系统时钟可以通过一个合成 MSR 读取，但该访问会陷入管理程
序。作为一种更快的替代方案，客户机可以配置一个在客户机与管理程序之
间共享的内存页。Hyper-V 用一个 64 位的 scale 值和 offset 值填充该内存
页。要读取合成时钟值，客户机读取 TSC，然后按 Hyper-V TLFS 中描述的方
式应用 scale 与 offset。得到的值以恒定的 10 MHz 频率前进。在实时迁移
到具有不同 TSC 频率的主机的情况下，Hyper-V 会调整共享页中的 scale 与
offset 值，以维持 10 MHz 的频率。)

Starting with Windows Server 2022 Hyper-V, Hyper-V uses hardware
support for TSC frequency scaling to enable live migration of VMs
across Hyper-V hosts where the TSC frequency may be different.
When a Linux guest detects that this Hyper-V functionality is
available, it prefers to use Linux's standard TSC-based clocksource.
Otherwise, it uses the clocksource for the Hyper-V synthetic system
clock implemented via the shared page (identified as
"hyperv_clocksource_tsc_page").
(从 Windows Server 2022 Hyper-V 开始，Hyper-V 使用对 TSC 频率缩放的硬
件支持，以便在 TSC 频率可能不同的 Hyper-V 主机之间实时迁移 VM。当
Linux 客户机检测到该 Hyper-V 功能可用时，它优先使用 Linux 标准的基于
TSC 的 clocksource。否则，它使用通过共享页实现的 Hyper-V 合成系统时
钟的 clocksource (标识为 "hyperv_clocksource_tsc_page")。)

The Hyper-V synthetic system clock is available to user space via
vDSO, and gettimeofday() and related system calls can execute
entirely in user space.  The vDSO is implemented by mapping the
shared page with scale and offset values into user space.  User
space code performs the same algorithm of reading the TSC and
applying the scale and offset to get the constant 10 MHz clock.
(Hyper-V 合成系统时钟通过 vDSO 对用户空间可用，gettimeofday() 及相关
系统调用可以完全在用户空间执行。vDSO 的实现方式是把带有 scale 与
offset 值的共享页映射到用户空间。用户空间代码执行相同的算法：读取
TSC 并应用 scale 与 offset，从而得到恒定的 10 MHz 时钟。)

Linux clockevents are based on Hyper-V synthetic timer 0 (stimer0).
While Hyper-V offers 4 synthetic timers for each CPU, Linux only uses
timer 0. In older versions of Hyper-V, an interrupt from stimer0
results in a VMBus control message that is demultiplexed by
vmbus_isr() as described in the Documentation/virt/hyperv/vmbus.rst
documentation. In newer versions of Hyper-V, stimer0 interrupts can
be mapped to an architectural interrupt, which is referred to as
"Direct Mode". Linux prefers to use Direct Mode when available. Since
x86/x64 doesn't support per-CPU interrupts, Direct Mode statically
allocates an x86 interrupt vector (HYPERV_STIMER0_VECTOR) across all CPUs
and explicitly codes it to call the stimer0 interrupt handler. Hence
interrupts from stimer0 are recorded on the "HVS" line in /proc/interrupts
rather than being associated with a Linux IRQ. Clockevents based on the
virtualized PIT and local APIC timer also work, but Hyper-V stimer0
is preferred.
(Linux 的 clockevents 基于 Hyper-V 合成定时器 0 (stimer0)。虽然
Hyper-V 为每个 CPU 提供 4 个合成定时器，但 Linux 只使用定时器 0。在旧
版本 Hyper-V 中，来自 stimer0 的中断会产生一条 VMBus 控制消息，由
vmbus_isr() 按照 Documentation/virt/hyperv/vmbus.rst 文档所述进行解复
用。在较新版本的 Hyper-V 中，stimer0 中断可以映射到一个架构中断，这
被称为 "Direct Mode"。若可用，Linux 优先使用 Direct Mode。由于
x86/x64 不支持 per-CPU 中断，Direct Mode 会在所有 CPU 上静态分配一个
x86 中断向量 (HYPERV_STIMER0_VECTOR)，并显式编写代码调用 stimer0 中断
处理程序。因此，来自 stimer0 的中断记录在 /proc/interrupts 的 "HVS"
行上，而不是与某个 Linux IRQ 关联。基于虚拟化 PIT 和本地 APIC 定时器
的 clockevents 也能工作，但 Hyper-V 的 stimer0 是首选。)

The driver for the Hyper-V synthetic system clock and timers is
drivers/clocksource/hyperv_timer.c.
(Hyper-V 合成系统时钟与定时器的驱动程序是
drivers/clocksource/hyperv_timer.c。)
