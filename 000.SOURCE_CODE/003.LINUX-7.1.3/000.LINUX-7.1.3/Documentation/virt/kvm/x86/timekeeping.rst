.. SPDX-License-Identifier: GPL-2.0

=================================================================================
Timekeeping Virtualization for X86-Based Architectures(基于 x86 架构的计时虚拟化)
=================================================================================

:Author: Zachary Amsden <zamsden@redhat.com>
(作者：Zachary Amsden <zamsden@redhat.com>)
:Copyright: (c) 2010, Red Hat.  All rights reserved.
(版权：(c) 2010，Red Hat。保留所有权利。)

.. Contents

   1) Overview
   2) Timing Devices
   3) TSC Hardware
   4) Virtualization Problems

1. Overview(概述)
=================

One of the most complicated parts of the X86 platform, and specifically,
the virtualization of this platform is the plethora of timing devices available
and the complexity of emulating those devices.  In addition, virtualization of
time introduces a new set of challenges because it introduces a multiplexed
division of time beyond the control of the guest CPU.
(X86 平台最复杂的部分之一——尤其是这个平台的虚拟化——在于数量繁多的计时设备以及模拟这些设备的复杂性。此外，时间的虚拟化带来了一组新的挑战，因为它引入了一种超出客户机 CPU 控制的、经复用的时间划分。)

First, we will describe the various timekeeping hardware available, then
present some of the problems which arise and solutions available, giving
specific recommendations for certain classes of KVM guests.
(我们将首先描述各种可用的计时硬件，然后提出由此产生的一些问题与可行的解决方案，并针对特定类别的 KVM 客户机给出具体建议。)

The purpose of this document is to collect data and information relevant to
timekeeping which may be difficult to find elsewhere, specifically,
information relevant to KVM and hardware-based virtualization.
(本文档的目的是收集与计时相关、且在其他地方可能难以找到的数据与信息，特别是与 KVM 和基于硬件的虚拟化相关的信息。)

2. Timing Devices(计时设备)
===========================

First we discuss the basic hardware devices available.  TSC and the related
KVM clock are special enough to warrant a full exposition and are described in
the following section.
(我们先讨论可用的基本硬件设备。TSC 和相关的 KVM clock 比较特殊，值得完整展开，将在下一节中描述。)

2.1. i8254 - PIT(i8254——可编程中断定时器 PIT)
---------------------------------------------

One of the first timer devices available is the programmable interrupt timer,
or PIT.  The PIT has a fixed frequency 1.193182 MHz base clock and three
channels which can be programmed to deliver periodic or one-shot interrupts.
These three channels can be configured in different modes and have individual
counters.  Channel 1 and 2 were not available for general use in the original
IBM PC, and historically were connected to control RAM refresh and the PC
speaker.  Now the PIT is typically integrated as part of an emulated chipset
and a separate physical PIT is not used.
(最早出现的定时器设备之一是可编程中断定时器 (PIT)。PIT 具有固定频率 1.193182 MHz 的基准时钟和三个通道，可编程为产生周期性或单发 (one-shot) 中断。这三个通道可以配置成不同模式，并拥有各自独立的计数器。在最初的 IBM PC 中，通道 1 和 2 不作一般用途，历史上分别连接到内存刷新控制和 PC 扬声器。如今 PIT 通常作为仿真芯片组的一部分集成，不再使用单独的物理 PIT。)

The PIT uses I/O ports 0x40 - 0x43.  Access to the 16-bit counters is done
using single or multiple byte access to the I/O ports.  There are 6 modes
available, but not all modes are available to all timers, as only timer 2
has a connected gate input, required for modes 1 and 5.  The gate line is
controlled by port 61h, bit 0, as illustrated in the following diagram::

  --------------             ----------------
  |            |           |                |
  |  1.1932 MHz|---------->| CLOCK      OUT | ---------> IRQ 0
  |    Clock   |   |       |                |
  --------------   |    +->| GATE  TIMER 0  |
                   |        ----------------
                   |
                   |        ----------------
                   |       |                |
                   |------>| CLOCK      OUT | ---------> 66.3 KHZ DRAM
                   |       |                |            (aka /dev/null)
                   |    +->| GATE  TIMER 1  |
                   |        ----------------
                   |
                   |        ----------------
                   |       |                |
                   |------>| CLOCK      OUT | ---------> Port 61h, bit 5
                           |                |      |
  Port 61h, bit 0 -------->| GATE  TIMER 2  |       \_.----   ____
                            ----------------         _|    )--|LPF|---Speaker
                                                    / *----   \___/
  Port 61h, bit 1 ---------------------------------/

(门控线由端口 61h 的比特 0 控制，如下图所示：)

The timer modes are now described.
(下面描述定时器的各种模式。)

Mode 0: Single Timeout.
(Mode 0：单次超时。)

 This is a one-shot software timeout that counts down
 when the gate is high (always true for timers 0 and 1).  When the count
 reaches zero, the output goes high.
 (这是一次单发 (one-shot) 软件超时，在门控为高时 (对定时器 0 和 1 始终为真) 递减计数。计数到零时，输出变为高电平。)

Mode 1: Triggered One-shot.
(Mode 1：触发式单发。)

 The output is initially set high.  When the gate
 line is set high, a countdown is initiated (which does not stop if the gate is
 lowered), during which the output is set low.  When the count reaches zero,
 the output goes high.
 (输出初始为高电平。当门控线置高时启动一次倒计数 (门控拉低也不会停止)，期间输出置为低电平。计数到零时，输出变回高电平。)

Mode 2: Rate Generator.
(Mode 2：速率发生器。)

 The output is initially set high.  When the countdown
 reaches 1, the output goes low for one count and then returns high.  The value
 is reloaded and the countdown automatically resumes.  If the gate line goes
 low, the count is halted.  If the output is low when the gate is lowered, the
 output automatically goes high (this only affects timer 2).
 (输出初始为高电平。当倒计数到 1 时，输出变低一个计数，然后回到高电平。计数值重装，倒计数自动恢复。若门控线变低，计数暂停。若门控拉低时输出为低，输出会自动变高 (这只影响定时器 2)。)

Mode 3: Square Wave.
(Mode 3：方波。)

 This generates a high / low square wave.  The count
 determines the length of the pulse, which alternates between high and low
 when zero is reached.  The count only proceeds when gate is high and is
 automatically reloaded on reaching zero.  The count is decremented twice at
 each clock to generate a full high / low cycle at the full periodic rate.
 If the count is even, the clock remains high for N/2 counts and low for N/2
 counts; if the clock is odd, the clock is high for (N+1)/2 counts and low
 for (N-1)/2 counts.  Only even values are latched by the counter, so odd
 values are not observed when reading.  This is the intended mode for timer 2,
 which generates sine-like tones by low-pass filtering the square wave output.
 (这会产生高/低方波。计数值决定脉冲长度，计数到零时在高与低之间交替。计数只在门控为高时进行，并在到达零时自动重装。每个时钟计数被递减两次，以便以完整周期速率产生一个完整的高/低周期。若计数值为偶数，时钟保持高电平 N/2 个计数、低电平 N/2 个计数；若为奇数，则高电平 (N+1)/2 个计数、低电平 (N-1)/2 个计数。计数器只锁存偶数值，因此读取时看不到奇数值。这是定时器 2 的预期模式，通过对方波输出做低通滤波来产生类似正弦的音调。)

Mode 4: Software Strobe.
(Mode 4：软件选通。)

 After programming this mode and loading the counter,
 the output remains high until the counter reaches zero.  Then the output
 goes low for 1 clock cycle and returns high.  The counter is not reloaded.
 Counting only occurs when gate is high.
 (编程此模式并装入计数器之后，输出保持高电平，直到计数器到达零。然后输出变低 1 个时钟周期并回到高电平。计数器不重装。只在门控为高时才计数。)

Mode 5: Hardware Strobe.
(Mode 5：硬件选通。)

 After programming and loading the counter, the
 output remains high.  When the gate is raised, a countdown is initiated
 (which does not stop if the gate is lowered).  When the counter reaches zero,
 the output goes low for 1 clock cycle and then returns high.  The counter is
 not reloaded.
 (编程并装入计数器之后，输出保持高电平。当门控拉高时启动一次倒计数 (门控拉低也不会停止)。计数器到达零时，输出变低 1 个时钟周期，然后回到高电平。计数器不重装。)

In addition to normal binary counting, the PIT supports BCD counting.  The
command port, 0x43 is used to set the counter and mode for each of the three
timers.
(除普通二进制计数外，PIT 还支持 BCD 计数。命令端口 0x43 用于为三个定时器分别设置计数器和模式。)

PIT commands, issued to port 0x43, using the following bit encoding::

  Bit 7-4: Command (See table below)
  Bit 3-1: Mode (000 = Mode 0, 101 = Mode 5, 11X = undefined)
  Bit 0  : Binary (0) / BCD (1)

(发往端口 0x43 的 PIT 命令采用如下比特编码：)

Command table::

  0000 - Latch Timer 0 count for port 0x40
	sample and hold the count to be read in port 0x40;
	additional commands ignored until counter is read;
	mode bits ignored.

  0001 - Set Timer 0 LSB mode for port 0x40
	set timer to read LSB only and force MSB to zero;
	mode bits set timer mode

  0010 - Set Timer 0 MSB mode for port 0x40
	set timer to read MSB only and force LSB to zero;
	mode bits set timer mode

  0011 - Set Timer 0 16-bit mode for port 0x40
	set timer to read / write LSB first, then MSB;
	mode bits set timer mode

  0100 - Latch Timer 1 count for port 0x41 - as described above
  0101 - Set Timer 1 LSB mode for port 0x41 - as described above
  0110 - Set Timer 1 MSB mode for port 0x41 - as described above
  0111 - Set Timer 1 16-bit mode for port 0x41 - as described above

  1000 - Latch Timer 2 count for port 0x42 - as described above
  1001 - Set Timer 2 LSB mode for port 0x42 - as described above
  1010 - Set Timer 2 MSB mode for port 0x42 - as described above
  1011 - Set Timer 2 16-bit mode for port 0x42 as described above

  1101 - General counter latch
	Latch combination of counters into corresponding ports
	Bit 3 = Counter 2
	Bit 2 = Counter 1
	Bit 1 = Counter 0
	Bit 0 = Unused

  1110 - Latch timer status
	Latch combination of counter mode into corresponding ports
	Bit 3 = Counter 2
	Bit 2 = Counter 1
	Bit 1 = Counter 0

	The output of ports 0x40-0x42 following this command will be:

	Bit 7 = Output pin
	Bit 6 = Count loaded (0 if timer has expired)
	Bit 5-4 = Read / Write mode
	    01 = MSB only
	    10 = LSB only
	    11 = LSB / MSB (16-bit)
	Bit 3-1 = Mode
	Bit 0 = Binary (0) / BCD mode (1)

(发往端口 0x43 的 PIT 命令表如下：)

2.2. RTC(实时时钟 RTC)
----------------------

The second device which was available in the original PC was the MC146818 real
time clock.  The original device is now obsolete, and usually emulated by the
system chipset, sometimes by an HPET and some frankenstein IRQ routing.
(最初的 PC 中第二个可用设备是 MC146818 实时时钟。该原始设备如今已经过时，通常由系统芯片组仿真，有时由 HPET 加上一些"科学怪人"式的 IRQ 路由来仿真。)

The RTC is accessed through CMOS variables, which uses an index register to
control which bytes are read.  Since there is only one index register, read
of the CMOS and read of the RTC require lock protection (in addition, it is
dangerous to allow userspace utilities such as hwclock to have direct RTC
access, as they could corrupt kernel reads and writes of CMOS memory).
(RTC 通过 CMOS 变量访问，后者用一个索引寄存器控制读取哪些字节。由于只有一个索引寄存器，读 CMOS 与读 RTC 都需要锁保护 (此外，允许 hwclock 这类用户态工具直接访问 RTC 是危险的，它们可能破坏内核对 CMOS 内存的读写)。)

The RTC generates an interrupt which is usually routed to IRQ 8.  The interrupt
can function as a periodic timer, an additional once a day alarm, and can issue
interrupts after an update of the CMOS registers by the MC146818 is complete.
The type of interrupt is signalled in the RTC status registers.
(RTC 产生的中断通常路由到 IRQ 8。该中断可以作为周期定时器、附加的每日一次闹钟，还可以在 MC146818 完成 CMOS 寄存器更新之后发出中断。中断类型由 RTC 状态寄存器给出信号。)

The RTC will update the current time fields by battery power even while the
system is off.  The current time fields should not be read while an update is
in progress, as indicated in the status register.
(即使系统关机，RTC 也会靠电池供电更新当前时间字段。当状态寄存器指示更新正在进行时，不应读取当前时间字段。)

The clock uses a 32.768kHz crystal, so bits 6-4 of register A should be
programmed to a 32kHz divider if the RTC is to count seconds.
(时钟使用 32.768kHz 晶振，因此若要让 RTC 计秒，应把寄存器 A 的比特 6-4 编程为 32kHz 分频。)

This is the RAM map originally used for the RTC/CMOS::

  Location    Size    Description
  ------------------------------------------
  00h         byte    Current second (BCD)
  01h         byte    Seconds alarm (BCD)
  02h         byte    Current minute (BCD)
  03h         byte    Minutes alarm (BCD)
  04h         byte    Current hour (BCD)
  05h         byte    Hours alarm (BCD)
  06h         byte    Current day of week (BCD)
  07h         byte    Current day of month (BCD)
  08h         byte    Current month (BCD)
  09h         byte    Current year (BCD)
  0Ah         byte    Register A
                       bit 7   = Update in progress
                       bit 6-4 = Divider for clock
                                  000 = 4.194 MHz
                                  001 = 1.049 MHz
                                  010 = 32 kHz
                                  10X = test modes
                                  110 = reset / disable
                                  111 = reset / disable
                       bit 3-0 = Rate selection for periodic interrupt
                                  000 = periodic timer disabled
                                  001 = 3.90625 uS
                                  010 = 7.8125 uS
                                  011 = .122070 mS
                                  100 = .244141 mS
                                     ...
                                 1101 = 125 mS
                                 1110 = 250 mS
                                 1111 = 500 mS
  0Bh         byte    Register B
                       bit 7   = Run (0) / Halt (1)
                       bit 6   = Periodic interrupt enable
                       bit 5   = Alarm interrupt enable
                       bit 4   = Update-ended interrupt enable
                       bit 3   = Square wave interrupt enable
                       bit 2   = BCD calendar (0) / Binary (1)
                       bit 1   = 12-hour mode (0) / 24-hour mode (1)
                       bit 0   = 0 (DST off) / 1 (DST enabled)
  OCh         byte    Register C (read only)
                       bit 7   = interrupt request flag (IRQF)
                       bit 6   = periodic interrupt flag (PF)
                       bit 5   = alarm interrupt flag (AF)
                       bit 4   = update interrupt flag (UF)
                       bit 3-0 = reserved
  ODh         byte    Register D (read only)
                       bit 7   = RTC has power
                       bit 6-0 = reserved
  32h         byte    Current century BCD (*)
  (*) location vendor specific and now determined from ACPI global tables

(上面是 RTC/CMOS 最初使用的 RAM 映射表：)

2.3. APIC(高级可编程中断控制器 APIC)
------------------------------------

On Pentium and later processors, an on-board timer is available to each CPU
as part of the Advanced Programmable Interrupt Controller.  The APIC is
accessed through memory-mapped registers and provides interrupt service to each
CPU, used for IPIs and local timer interrupts.
(从 Pentium 及其后的处理器开始，每个 CPU 都有一个片上定时器，它是高级可编程中断控制器 (APIC) 的一部分。APIC 通过内存映射寄存器访问，为每个 CPU 提供中断服务，用于 IPI 和本地定时器中断。)

Although in theory the APIC is a safe and stable source for local interrupts,
in practice, many bugs and glitches have occurred due to the special nature of
the APIC CPU-local memory-mapped hardware.  Beware that CPU errata may affect
the use of the APIC and that workarounds may be required.  In addition, some of
these workarounds pose unique constraints for virtualization - requiring either
extra overhead incurred from extra reads of memory-mapped I/O or additional
functionality that may be more computationally expensive to implement.
(尽管理论上 APIC 是本地中断安全而稳定的来源，但实践中，由于 APIC 这种 CPU 本地的内存映射硬件的特殊性质，已经出现过许多 bug 和小故障。要注意 CPU 勘误可能影响 APIC 的使用，并且可能需要变通办法。此外，其中一些变通办法给虚拟化带来了独特的约束——要么需要为额外的内存映射 I/O 读取付出额外开销，要么需要实现计算上可能更昂贵的额外功能。)

Since the APIC is documented quite well in the Intel and AMD manuals, we will
avoid repetition of the detail here.  It should be pointed out that the APIC
timer is programmed through the LVT (local vector timer) register, is capable
of one-shot or periodic operation, and is based on the bus clock divided down
by the programmable divider register.
(由于 APIC 在 Intel 和 AMD 手册中已有相当完善的文档，这里不再重复细节。需要指出的是，APIC 定时器通过 LVT (local vector timer，本地向量定时器) 寄存器编程，可进行单发或周期操作，并以总线时钟经可编程分频寄存器分频后的频率为基准。)

2.4. HPET(高精度事件定时器 HPET)
--------------------------------

HPET is quite complex, and was originally intended to replace the PIT / RTC
support of the X86 PC.  It remains to be seen whether that will be the case, as
the de facto standard of PC hardware is to emulate these older devices.  Some
systems designated as legacy free may support only the HPET as a hardware timer
device.
(HPET 相当复杂，最初打算用它取代 X86 PC 的 PIT / RTC 支持。它能否如愿仍有待观察，因为 PC 硬件的事实标准是仿真这些较老的设备。一些标榜"无遗留 (legacy free)"的系统可能只支持 HPET 作为硬件定时器设备。)

The HPET spec is rather loose and vague, requiring at least 3 hardware timers,
but allowing implementation freedom to support many more.  It also imposes no
fixed rate on the timer frequency, but does impose some extremal values on
frequency, error and slew.
(HPET 规范相当宽松和含糊：要求至少 3 个硬件定时器，但允许实现自行支持更多。它也没有对定时器频率规定固定值，但对频率、误差和偏摆 (slew) 规定了一些极限值。)

In general, the HPET is recommended as a high precision (compared to PIT /RTC)
time source which is independent of local variation (as there is only one HPET
in any given system).  The HPET is also memory-mapped, and its presence is
indicated through ACPI tables by the BIOS.
(总体而言，推荐把 HPET 用作高精度 (相比 PIT / RTC) 的时钟源，它不受本地差异影响 (因为每个系统中只有一个 HPET)。HPET 也是内存映射的，其存在由 BIOS 通过 ACPI 表指明。)

Detailed specification of the HPET is beyond the current scope of this
document, as it is also very well documented elsewhere.
(HPET 的详细规范超出本文档当前的范围，它在其他地方也有很好的文档。)

2.5. Offboard Timers(板外定时器)
--------------------------------

Several cards, both proprietary (watchdog boards) and commonplace (e1000) have
timing chips built into the cards which may have registers which are accessible
to kernel or user drivers.  To the author's knowledge, using these to generate
a clocksource for a Linux or other kernel has not yet been attempted and is in
general frowned upon as not playing by the agreed rules of the game.  Such a
timer device would require additional support to be virtualized properly and is
not considered important at this time as no known operating system does this.
(一些板卡，无论是专有的 (看门狗板卡) 还是常见的 (e1000)，都在卡上内置了计时芯片，其寄存器可能对内核或用户态驱动可访问。据作者所知，还没有人尝试用它们为 Linux 或其他内核生成时钟源，这种做法总体上也不被认可，被视为不遵守既定的游戏规则。这样的定时器设备需要额外的支持才能被正确虚拟化，但由于没有已知的操作系统这样做，目前不认为它重要。)

3. TSC Hardware(TSC 硬件)
=========================

The TSC or time stamp counter is relatively simple in theory; it counts
instruction cycles issued by the processor, which can be used as a measure of
time.  In practice, due to a number of problems, it is the most complicated
timekeeping device to use.
(TSC 即时间戳计数器 (time stamp counter)，理论上相当简单：它对处理器发射的指令周期计数，可以用作时间的度量。但在实践中，由于诸多问题，它是使用起来最复杂的计时设备。)

The TSC is represented internally as a 64-bit MSR which can be read with the
RDMSR, RDTSC, or RDTSCP (when available) instructions.  In the past, hardware
limitations made it possible to write the TSC, but generally on old hardware it
was only possible to write the low 32-bits of the 64-bit counter, and the upper
32-bits of the counter were cleared.  Now, however, on Intel processors family
0Fh, for models 3, 4 and 6, and family 06h, models e and f, this restriction
has been lifted and all 64-bits are writable.  On AMD systems, the ability to
write the TSC MSR is not an architectural guarantee.
(TSC 在内部表示为一个 64 位 MSR，可用 RDMSR、RDTSC 或 RDTSCP (如果可用) 指令读取。过去，由于硬件限制，写 TSC 是可能的，但一般在旧硬件上只能写 64 位计数器的低 32 位，高 32 位会被清零。不过现在，在 Intel 0Fh 家族的型号 3、4、6 以及 06h 家族的型号 e、f 上，这一限制已被取消，全部 64 位都可写。在 AMD 系统上，写 TSC MSR 的能力并非架构保证。)

The TSC is accessible from CPL-0 and conditionally, for CPL > 0 software by
means of the CR4.TSD bit, which when enabled, disables CPL > 0 TSC access.
(CPL-0 总是能访问 TSC；对 CPL > 0 的软件，则通过 CR4.TSD 位有条件地允许访问——该位置位时会禁用 CPL > 0 的 TSC 访问。)

Some vendors have implemented an additional instruction, RDTSCP, which returns
atomically not just the TSC, but an indicator which corresponds to the
processor number.  This can be used to index into an array of TSC variables to
determine offset information in SMP systems where TSCs are not synchronized.
The presence of this instruction must be determined by consulting CPUID feature
bits.
(一些厂商实现了额外的指令 RDTSCP，它原子地返回 TSC 以及一个对应处理器编号的指示值。在 TSC 不同步的 SMP 系统中，可以用它索引一组 TSC 变量来确定偏移信息。该指令是否存在必须通过查询 CPUID 特性位来确定。)

Both VMX and SVM provide extension fields in the virtualization hardware which
allows the guest visible TSC to be offset by a constant.  Newer implementations
promise to allow the TSC to additionally be scaled, but this hardware is not
yet widely available.
(VMX 与 SVM 都在虚拟化硬件中提供了扩展字段，使客户机可见的 TSC 可以偏移一个常量。较新的实现还承诺允许对 TSC 进行缩放，但这类硬件尚未广泛普及。)

3.1. TSC synchronization(TSC 同步)
----------------------------------

The TSC is a CPU-local clock in most implementations.  This means, on SMP
platforms, the TSCs of different CPUs may start at different times depending
on when the CPUs are powered on.  Generally, CPUs on the same die will share
the same clock, however, this is not always the case.
(在大多数实现中，TSC 是 CPU 本地的时钟。这意味着在 SMP 平台上，不同 CPU 的 TSC 可能因 CPU 上电时间不同而在不同时刻启动。一般来说，同一 die 上的 CPU 共享同一个时钟，但并不总是如此。)

The BIOS may attempt to resynchronize the TSCs during the poweron process and
the operating system or other system software may attempt to do this as well.
Several hardware limitations make the problem worse - if it is not possible to
write the full 64-bits of the TSC, it may be impossible to match the TSC in
newly arriving CPUs to that of the rest of the system, resulting in
unsynchronized TSCs.  This may be done by BIOS or system software, but in
practice, getting a perfectly synchronized TSC will not be possible unless all
values are read from the same clock, which generally only is possible on single
socket systems or those with special hardware support.
(BIOS 可能会在上电过程中尝试重新同步各 TSC，操作系统或其他系统软件也可能尝试这样做。几个硬件限制使问题更糟——如果无法写入 TSC 的全部 64 位，就可能无法把新加入 CPU 的 TSC 与系统其余部分匹配，导致 TSC 不同步。这可以由 BIOS 或系统软件完成，但实际上，除非所有值都读自同一个时钟，否则不可能得到完美同步的 TSC，而这通常只有在单插槽系统或具有特殊硬件支持的系统上才可行。)

3.2. TSC and CPU hotplug(TSC 与 CPU 热插拔)
-------------------------------------------

As touched on already, CPUs which arrive later than the boot time of the system
may not have a TSC value that is synchronized with the rest of the system.
Either system software, BIOS, or SMM code may actually try to establish the TSC
to a value matching the rest of the system, but a perfect match is usually not
a guarantee.  This can have the effect of bringing a system from a state where
TSC is synchronized back to a state where TSC synchronization flaws, however
small, may be exposed to the OS and any virtualization environment.
(如前文所述，晚于系统启动时间到达的 CPU，其 TSC 值可能与系统其余部分不同步。系统软件、BIOS 或 SMM 代码确实可能尝试把 TSC 设为与系统其余部分匹配的值，但通常不能保证完美匹配。这可能使系统从 TSC 已同步的状态退回到 TSC 同步缺陷——无论多小——可能暴露给操作系统和任何虚拟化环境的状态。)

3.3. TSC and multi-socket / NUMA(TSC 与多插槽 / NUMA)
-----------------------------------------------------

Multi-socket systems, especially large multi-socket systems are likely to have
individual clocksources rather than a single, universally distributed clock.
Since these clocks are driven by different crystals, they will not have
perfectly matched frequency, and temperature and electrical variations will
cause the CPU clocks, and thus the TSCs to drift over time.  Depending on the
exact clock and bus design, the drift may or may not be fixed in absolute
error, and may accumulate over time.
(多插槽系统，尤其是大型多插槽系统，很可能拥有各自独立的时钟源，而不是单一的全局分发时钟。由于这些时钟由不同的晶振驱动，其频率不会完全匹配，温度与电气特性的变化会导致 CPU 时钟——也就是 TSC——随时间漂移。取决于具体的时钟与总线设计，漂移的绝对误差可能是固定的，也可能随时间累积。)

In addition, very large systems may deliberately slew the clocks of individual
cores.  This technique, known as spread-spectrum clocking, reduces EMI at the
clock frequency and harmonics of it, which may be required to pass FCC
standards for telecommunications and computer equipment.
(此外，超大型系统可能故意对各个核的时钟做偏摆 (slew)。这种技术称为扩频时钟 (spread-spectrum clocking)，可降低时钟频率及其谐波上的电磁干扰 (EMI)，这可能是通过电信与计算机设备 FCC 标准所必需的。)

It is recommended not to trust the TSCs to remain synchronized on NUMA or
multiple socket systems for these reasons.
(出于这些原因，建议不要指望 NUMA 或多插槽系统上的 TSC 保持同步。)

3.4. TSC and C-states(TSC 与 C-states)
--------------------------------------

C-states, or idling states of the processor, especially C1E and deeper sleep
states may be problematic for TSC as well.  The TSC may stop advancing in such
a state, resulting in a TSC which is behind that of other CPUs when execution
is resumed.  Such CPUs must be detected and flagged by the operating system
based on CPU and chipset identifications.
(C-states 即处理器的空闲状态，特别是 C1E 和更深的睡眠状态，对 TSC 也可能有问题。在这种状态下 TSC 可能停止前进，导致恢复执行时该 CPU 的 TSC 落后于其他 CPU。操作系统必须根据 CPU 与芯片组的标识检测并标记这类 CPU。)

The TSC in such a case may be corrected by catching it up to a known external
clocksource.
(这种情况下，可以通过把 TSC 追赶到某个已知的外部时钟源来纠正它。)

3.5. TSC frequency change / P-states(TSC 频率变化 / P-states)
-------------------------------------------------------------

To make things slightly more interesting, some CPUs may change frequency.  They
may or may not run the TSC at the same rate, and because the frequency change
may be staggered or slewed, at some points in time, the TSC rate may not be
known other than falling within a range of values.  In this case, the TSC will
not be a stable time source, and must be calibrated against a known, stable,
external clock to be a usable source of time.
(让事情稍微更有趣一些：某些 CPU 可能改变频率。它们可能让 TSC 以相同速率运行，也可能不会；而且由于频率变化可能是分阶段的或渐变的，在某些时刻，TSC 速率除了"落在某个数值范围内"之外可能不得而知。这种情况下，TSC 不会是稳定的时间源，必须对照已知的、稳定的外部时钟进行校准，才能作为可用的时间来源。)

Whether the TSC runs at a constant rate or scales with the P-state is model
dependent and must be determined by inspecting CPUID, chipset or vendor
specific MSR fields.
(TSC 以恒定速率运行还是随 P-state 缩放取决于具体型号，必须通过查看 CPUID、芯片组或厂商特定的 MSR 字段来确定。)

In addition, some vendors have known bugs where the P-state is actually
compensated for properly during normal operation, but when the processor is
inactive, the P-state may be raised temporarily to service cache misses from
other processors.  In such cases, the TSC on halted CPUs could advance faster
than that of non-halted processors.  AMD Turion processors are known to have
this problem.
(此外，一些厂商有已知 bug：正常运行时 P-state 实际上得到了正确补偿，但当处理器不活动时，P-state 可能被临时提高，以便为其他处理器的缓存缺失服务。这种情况下，停机 CPU 的 TSC 可能比未停机处理器的 TSC 走得更快。已知 AMD Turion 处理器存在此问题。)

3.6. TSC and STPCLK / T-states(TSC 与 STPCLK / T-states)
--------------------------------------------------------

External signals given to the processor may also have the effect of stopping
the TSC.  This is typically done for thermal emergency power control to prevent
an overheating condition, and typically, there is no way to detect that this
condition has happened.
(发给处理器的外部信号也可能使 TSC 停止。这通常用于热紧急电源控制以防止过热，而且通常没有办法检测到这种情况已经发生。)

3.7. TSC virtualization - VMX(TSC 虚拟化——VMX)
----------------------------------------------

VMX provides conditional trapping of RDTSC, RDMSR, WRMSR and RDTSCP
instructions, which is enough for full virtualization of TSC in any manner.  In
addition, VMX allows passing through the host TSC plus an additional TSC_OFFSET
field specified in the VMCS.  Special instructions must be used to read and
write the VMCS field.
(VMX 提供对 RDTSC、RDMSR、WRMSR 和 RDTSCP 指令的条件捕获，这足以用任何方式对 TSC 进行完整虚拟化。此外，VMX 允许透传主机 TSC 并加上 VMCS 中指定的额外 TSC_OFFSET 字段。读写 VMCS 字段必须使用特殊指令。)

3.8. TSC virtualization - SVM(TSC 虚拟化——SVM)
----------------------------------------------

SVM provides conditional trapping of RDTSC, RDMSR, WRMSR and RDTSCP
instructions, which is enough for full virtualization of TSC in any manner.  In
addition, SVM allows passing through the host TSC plus an additional offset
field specified in the SVM control block.
(SVM 提供对 RDTSC、RDMSR、WRMSR 和 RDTSCP 指令的条件捕获，这足以用任何方式对 TSC 进行完整虚拟化。此外，SVM 允许透传主机 TSC 并加上 SVM 控制块中指定的额外偏移字段。)

3.9. TSC feature bits in Linux(Linux 中的 TSC 特性位)
-----------------------------------------------------

In summary, there is no way to guarantee the TSC remains in perfect
synchronization unless it is explicitly guaranteed by the architecture.  Even
if so, the TSCs in multi-sockets or NUMA systems may still run independently
despite being locally consistent.
(总而言之，除非架构明确保证，否则没有办法保证 TSC 保持完美同步。即使有保证，多插槽或 NUMA 系统中的 TSC 仍可能在局部一致的同时各自独立运行。)

The following feature bits are used by Linux to signal various TSC attributes,
but they can only be taken to be meaningful for UP or single node systems.
(Linux 使用以下特性位来标示各种 TSC 属性，但它们只能被认为对 UP 或单节点系统有意义。)

=========================	=======================================
X86_FEATURE_TSC			The TSC is available in hardware (硬件提供 TSC)
X86_FEATURE_RDTSCP		The RDTSCP instruction is available (提供 RDTSCP 指令)
X86_FEATURE_CONSTANT_TSC	The TSC rate is unchanged with P-states (TSC 速率不随 P-state 变化)
X86_FEATURE_NONSTOP_TSC		The TSC does not stop in C-states (TSC 在 C-states 下不停止)
X86_FEATURE_TSC_RELIABLE	TSC sync checks are skipped (VMware) (跳过 TSC 同步检查 (VMware))
=========================	=======================================

4. Virtualization Problems(虚拟化问题)
======================================

Timekeeping is especially problematic for virtualization because a number of
challenges arise.  The most obvious problem is that time is now shared between
the host and, potentially, a number of virtual machines.  Thus the virtual
operating system does not run with 100% usage of the CPU, despite the fact that
it may very well make that assumption.  It may expect it to remain true to very
exacting bounds when interrupt sources are disabled, but in reality only its
virtual interrupt sources are disabled, and the machine may still be preempted
at any time.  This causes problems as the passage of real time, the injection
of machine interrupts and the associated clock sources are no longer completely
synchronized with real time.
(计时对虚拟化来说问题尤其多，因为会出现许多挑战。最明显的问题是：时间现在由主机与 (潜在的) 多个虚拟机共享。因此，虚拟操作系统并不是以 100% 的 CPU 占用率运行的，尽管它很可能做出这样的假设。在中断源被禁用时，它可能期望这一点保持在非常严格的界限内，但实际上只有它的虚拟中断源被禁用，机器仍可能随时被抢占。这会带来问题，因为真实时间的流逝、机器中断的注入以及相关时钟源不再与真实时间完全同步。)

This same problem can occur on native hardware to a degree, as SMM mode may
steal cycles from the naturally on X86 systems when SMM mode is used by the
BIOS, but not in such an extreme fashion.  However, the fact that SMM mode may
cause similar problems to virtualization makes it a good justification for
solving many of these problems on bare metal.
(同样的问题在原生硬件上也可能在一定程度上出现：当 BIOS 使用 SMM 模式时，SMM 模式可能从 X86 系统上"理所当然"的运行中窃取周期，只是没有这么极端。不过，SMM 模式可能造成与虚拟化类似的问题，这恰好为在裸机上解决其中许多问题提供了充分理由。)

4.1. Interrupt clocking(中断计时)
---------------------------------

One of the most immediate problems that occurs with legacy operating systems
is that the system timekeeping routines are often designed to keep track of
time by counting periodic interrupts.  These interrupts may come from the PIT
or the RTC, but the problem is the same: the host virtualization engine may not
be able to deliver the proper number of interrupts per second, and so guest
time may fall behind.  This is especially problematic if a high interrupt rate
is selected, such as 1000 HZ, which is unfortunately the default for many Linux
guests.
(遗留操作系统最直接的问题之一是：系统计时例程往往被设计成通过统计周期性中断来跟踪时间。这些中断可能来自 PIT 或 RTC，但问题是一样的：主机虚拟化引擎可能无法每秒递交正确数量的中断，于是客户机时间可能落后。如果选择了较高的中断频率 (例如 1000 HZ，不幸的是这是许多 Linux 客户机的默认值)，问题尤其严重。)

There are three approaches to solving this problem; first, it may be possible
to simply ignore it.  Guests which have a separate time source for tracking
'wall clock' or 'real time' may not need any adjustment of their interrupts to
maintain proper time.  If this is not sufficient, it may be necessary to inject
additional interrupts into the guest in order to increase the effective
interrupt rate.  This approach leads to complications in extreme conditions,
where host load or guest lag is too much to compensate for, and thus another
solution to the problem has risen: the guest may need to become aware of lost
ticks and compensate for them internally.  Although promising in theory, the
implementation of this policy in Linux has been extremely error prone, and a
number of buggy variants of lost tick compensation are distributed across
commonly used Linux systems.
(解决这个问题有三种途径。第一，也许可以直接无视它。有单独时间源来跟踪"墙上时钟"或"真实时间"的客户机，可能不需要对中断做任何调整就能维持正确时间。如果这还不够，可能需要向客户机注入额外中断以提高有效中断频率。这种做法在极端条件下会导致复杂化——当主机负载或客户机滞后多到无法补偿时——于是出现了另一种解决方案：客户机可能需要感知丢失的节拍 (lost ticks) 并在内部补偿。尽管理论上前景不错，但该策略在 Linux 中的实现极易出错，许多有 bug 的丢失节拍补偿变体散布在常用的 Linux 系统中。)

Windows uses periodic RTC clocking as a means of keeping time internally, and
thus requires interrupt slewing to keep proper time.  It does use a low enough
rate (ed: is it 18.2 Hz?) however that it has not yet been a problem in
practice.
(Windows 在内部使用周期性 RTC 计时作为保持时间的手段，因此需要中断偏摆 (slewing) 来保持正确时间。不过它使用的频率足够低 (编者按：是 18.2 Hz 吗？)，因此实践中尚未成为问题。)

4.2. TSC sampling and serialization(TSC 采样与串行化)
-----------------------------------------------------

As the highest precision time source available, the cycle counter of the CPU
has aroused much interest from developers.  As explained above, this timer has
many problems unique to its nature as a local, potentially unstable and
potentially unsynchronized source.  One issue which is not unique to the TSC,
but is highlighted because of its very precise nature is sampling delay.  By
definition, the counter, once read is already old.  However, it is also
possible for the counter to be read ahead of the actual use of the result.
This is a consequence of the superscalar execution of the instruction stream,
which may execute instructions out of order.  Such execution is called
non-serialized.  Forcing serialized execution is necessary for precise
measurement with the TSC, and requires a serializing instruction, such as CPUID
or an MSR read.
(作为可用的最高精度时间源，CPU 的周期计数器引起了开发者极大的兴趣。如上所述，这个定时器因其作为本地的、潜在不稳定且潜在不同步的源这一本性而有许多独有问题。采样延迟并非 TSC 独有，但因其非常精确的本性而格外突出。顾名思义，计数器一旦被读出就已经是旧值。而且，计数器还可能在结果被实际使用之前就被读取。这是指令流超标量执行的后果——指令可能乱序执行。这样的执行称为非串行化 (non-serialized)。要用 TSC 做精确测量，必须强制串行化执行，这需要一个串行化指令，例如 CPUID 或 MSR 读取。)

Since CPUID may actually be virtualized by a trap and emulate mechanism, this
serialization can pose a performance issue for hardware virtualization.  An
accurate time stamp counter reading may therefore not always be available, and
it may be necessary for an implementation to guard against "backwards" reads of
the TSC as seen from other CPUs, even in an otherwise perfectly synchronized
system.
(由于 CPUID 实际上可能通过陷阱与模拟 (trap and emulate) 机制被虚拟化，这种串行化可能给硬件虚拟化带来性能问题。因此，准确的时间戳计数器读数并非总是可得，实现可能需要防范从其他 CPU 看到的 TSC "倒退"读数——即使在其他方面完美同步的系统中也是如此。)

4.3. Timespec aliasing(timespec 别名混叠)
-----------------------------------------

Additionally, this lack of serialization from the TSC poses another challenge
when using results of the TSC when measured against another time source.  As
the TSC is much higher precision, many possible values of the TSC may be read
while another clock is still expressing the same value.
(此外，TSC 缺乏串行化这一点，在把 TSC 读数与另一个时间源对照使用时带来另一个挑战。由于 TSC 精度高得多，在另一个时钟仍表达同一值的期间，TSC 可能读出许多不同的值。)

That is, you may read (T,T+10) while external clock C maintains the same value.
Due to non-serialized reads, you may actually end up with a range which
fluctuates - from (T-1.. T+10).  Thus, any time calculated from a TSC, but
calibrated against an external value may have a range of valid values.
Re-calibrating this computation may actually cause time, as computed after the
calibration, to go backwards, compared with time computed before the
calibration.
(也就是说，在外部时钟 C 维持同一值期间，你可能读到 (T,T+10)。由于读取是非串行化的，你实际可能得到一个波动的区间——从 (T-1..T+10)。因此，任何由 TSC 计算但对照外部值校准的时间都可能有多个有效值。重新校准这一计算实际上可能使校准后计算出的时间相对于校准前计算出的时间倒退。)

This problem is particularly pronounced with an internal time source in Linux,
the kernel time, which is expressed in the theoretically high resolution
timespec - but which advances in much larger granularity intervals, sometimes
at the rate of jiffies, and possibly in catchup modes, at a much larger step.
(这个问题在 Linux 的一个内部时间源——内核时间 (kernel time)——上尤为明显：它以理论分辨率很高的 timespec 表达，却以大得多的粒度间隔前进，有时以 jiffies 的速率，而在追赶 (catchup) 模式下步长可能大得多。)

This aliasing requires care in the computation and recalibration of kvmclock
and any other values derived from TSC computation (such as TSC virtualization
itself).
(这种混叠要求在计算和重新校准 kvmclock 以及任何其他由 TSC 计算派生的值 (例如 TSC 虚拟化本身) 时多加小心。)

4.4. Migration(迁移)
--------------------

Migration of a virtual machine raises problems for timekeeping in two ways.
First, the migration itself may take time, during which interrupts cannot be
delivered, and after which, the guest time may need to be caught up.  NTP may
be able to help to some degree here, as the clock correction required is
typically small enough to fall in the NTP-correctable window.
(虚拟机迁移在两方面给计时带来问题。首先，迁移本身可能耗时，期间中断无法递交，之后客户机时间可能需要追赶。NTP 在这里可能能帮上一些忙，因为所需的时钟校正通常小到落在 NTP 可校正窗口内。)

An additional concern is that timers based off the TSC (or HPET, if the raw bus
clock is exposed) may now be running at different rates, requiring compensation
in some way in the hypervisor by virtualizing these timers.  In addition,
migrating to a faster machine may preclude the use of a passthrough TSC, as a
faster clock cannot be made visible to a guest without the potential of time
advancing faster than usual.  A slower clock is less of a problem, as it can
always be caught up to the original rate.  KVM clock avoids these problems by
simply storing multipliers and offsets against the TSC for the guest to convert
back into nanosecond resolution values.
(另一个顾虑是：基于 TSC (或 HPET，如果暴露了原始总线时钟) 的定时器现在可能以不同的速率运行，需要 hypervisor 通过虚拟化这些定时器来以某种方式补偿。此外，迁移到更快的机器可能使直通 (passthrough) TSC 不可用，因为如果让客户机看到更快的时钟，时间就可能比平常走得快。较慢的时钟问题不大，因为总可以追赶回原始速率。KVM clock 通过简单地存储相对于 TSC 的乘数与偏移、由客户机换算回纳秒分辨率的值，避开了这些问题。)

4.5. Scheduling(调度)
---------------------

Since scheduling may be based on precise timing and firing of interrupts, the
scheduling algorithms of an operating system may be adversely affected by
virtualization.  In theory, the effect is random and should be universally
distributed, but in contrived as well as real scenarios (guest device access,
causes of virtualization exits, possible context switch), this may not always
be the case.  The effect of this has not been well studied.
(由于调度可能基于精确的定时与中断触发，操作系统的调度算法可能受到虚拟化的不利影响。理论上，这种影响是随机的、应当普遍分布，但在人为构造的以及真实场景中 (客户机设备访问、导致虚拟化退出的原因、可能的上下文切换)，情况未必总是如此。这种影响尚未得到充分研究。)

In an attempt to work around this, several implementations have provided a
paravirtualized scheduler clock, which reveals the true amount of CPU time for
which a virtual machine has been running.
(为了绕过这个问题，一些实现提供了半虚拟化的调度器时钟，它揭示虚拟机实际已运行的真实 CPU 时间量。)

4.6. Watchdogs(看门狗)
----------------------

Watchdog timers, such as the lock detector in Linux may fire accidentally when
running under hardware virtualization due to timer interrupts being delayed or
misinterpretation of the passage of real time.  Usually, these warnings are
spurious and can be ignored, but in some circumstances it may be necessary to
disable such detection.
(看门狗定时器 (例如 Linux 中的锁检测器) 在硬件虚拟化下运行时，可能因定时器中断被延迟或对真实时间流逝的误判而误触发。通常这些告警是虚假的、可以忽略，但在某些情况下可能有必要禁用这类检测。)

4.7. Delays and precision timing(延迟与精密计时)
------------------------------------------------

Precise timing and delays may not be possible in a virtualized system.  This
can happen if the system is controlling physical hardware, or issues delays to
compensate for slower I/O to and from devices.  The first issue is not solvable
in general for a virtualized system; hardware control software can't be
adequately virtualized without a full real-time operating system, which would
require an RT aware virtualization platform.
(在虚拟化系统中，精密计时与延迟可能无法实现。当系统控制物理硬件、或为了补偿设备间较慢的 I/O 而插入延迟时，就可能发生这种情况。第一个问题对虚拟化系统总体上无解；没有完整的实时操作系统，硬件控制软件无法被充分虚拟化，而这需要具备 RT 感知的虚拟化平台。)

The second issue may cause performance problems, but this is unlikely to be a
significant issue.  In many cases these delays may be eliminated through
configuration or paravirtualization.
(第二个问题可能造成性能问题，但不太可能成为显著问题。在许多情况下，这些延迟可以通过配置或半虚拟化消除。)

4.8. Covert channels and leaks(隐蔽信道与信息泄漏)
--------------------------------------------------

In addition to the above problems, time information will inevitably leak to the
guest about the host in anything but a perfect implementation of virtualized
time.  This may allow the guest to infer the presence of a hypervisor (as in a
red-pill type detection), and it may allow information to leak between guests
by using CPU utilization itself as a signalling channel.  Preventing such
problems would require completely isolated virtual time which may not track
real time any longer.  This may be useful in certain security or QA contexts,
but in general isn't recommended for real-world deployment scenarios.
(除上述问题之外，只要虚拟化时间的实现不够完美，时间信息就不可避免地会把主机的信息泄漏给客户机。这可能让客户机推断出 hypervisor 的存在 (如"红药丸"式检测)，也可能让客户机之间通过把 CPU 利用率本身用作信号信道而泄漏信息。要防止这类问题，需要完全隔离的虚拟时间——它可能不再跟踪真实时间。这在某些安全或 QA 场景下可能有用，但总体不建议用于真实部署场景。)
