====================================
The Linux Kernel API(Linux 内核 API)
====================================


Basic C Library Functions(基础 C 库函数)
========================================

When writing drivers, you cannot in general use routines which are from
the C Library. Some of the functions have been found generally useful
and they are listed below. The behaviour of these functions may vary
slightly from those defined by ANSI, and these deviations are noted in
the text.
(编写驱动程序时，通常不能使用来自 C 库的例程。人们发现其中一些函数普遍有用，它们列在下面。这些函数的行为可能与 ANSI 定义的略有不同，这些差异已在文中注明。)

String Conversions(字符串转换)
------------------------------

.. kernel-doc:: lib/vsprintf.c
   :export:

.. kernel-doc:: include/linux/kstrtox.h
   :functions: kstrtol kstrtoul

.. kernel-doc:: lib/kstrtox.c
   :export:

.. kernel-doc:: lib/string_helpers.c
   :export:

String Manipulation(字符串操作)
-------------------------------

.. kernel-doc:: include/linux/fortify-string.h
   :internal:

.. kernel-doc:: lib/string.c
   :export:

.. kernel-doc:: include/linux/string.h
   :internal:

.. kernel-doc:: mm/util.c
   :functions: kstrdup kstrdup_const kstrndup kmemdup kmemdup_nul memdup_user
               vmemdup_user strndup_user memdup_user_nul

Basic Kernel Library Functions(基础内核库函数)
==============================================

The Linux kernel provides more basic utility functions.
(Linux 内核提供了更多基础实用函数。)

Bit Operations(位运算)
----------------------

.. kernel-doc:: include/asm-generic/bitops/instrumented-atomic.h
   :internal:

.. kernel-doc:: include/asm-generic/bitops/instrumented-non-atomic.h
   :internal:

.. kernel-doc:: include/asm-generic/bitops/instrumented-lock.h
   :internal:

Bitmap Operations(位图运算)
---------------------------

.. kernel-doc:: lib/bitmap.c
   :doc: bitmap introduction

.. kernel-doc:: include/linux/bitmap.h
   :doc: declare bitmap

.. kernel-doc:: include/linux/bitmap.h
   :doc: bitmap overview

.. kernel-doc:: include/linux/bitmap.h
   :doc: bitmap bitops

.. kernel-doc:: lib/bitmap.c
   :export:

.. kernel-doc:: lib/bitmap.c
   :internal:

.. kernel-doc:: include/linux/bitmap.h
   :internal:

Command-line Parsing(命令行解析)
--------------------------------

.. kernel-doc:: lib/cmdline.c
   :export:

Error Pointers(错误指针)
------------------------

.. kernel-doc:: include/linux/err.h
   :internal:

Sorting(排序)
-------------

.. kernel-doc:: lib/sort.c
   :export:

.. kernel-doc:: lib/list_sort.c
   :export:

Text Searching(文本搜索)
------------------------

.. kernel-doc:: lib/textsearch.c
   :doc: ts_intro

.. kernel-doc:: lib/textsearch.c
   :export:

.. kernel-doc:: include/linux/textsearch.h
   :functions: textsearch_find textsearch_next \
               textsearch_get_pattern textsearch_get_pattern_len

CRC and Math Functions in Linux(Linux 中的 CRC 与数学函数)
==========================================================

Arithmetic Overflow Checking(算术溢出检查)
------------------------------------------

.. kernel-doc:: include/linux/overflow.h
   :internal:

CRC Functions(CRC 函数)
-----------------------

.. kernel-doc:: lib/crc/crc4.c
   :export:

.. kernel-doc:: lib/crc/crc7.c
   :export:

.. kernel-doc:: lib/crc/crc8.c
   :export:

.. kernel-doc:: lib/crc/crc16.c
   :export:

.. kernel-doc:: lib/crc/crc-ccitt.c
   :export:

.. kernel-doc:: lib/crc/crc-itu-t.c
   :export:

.. kernel-doc:: include/linux/crc32.h

.. kernel-doc:: include/linux/crc64.h

Base 2 log and power Functions(以 2 为底的对数与幂函数)
-------------------------------------------------------

.. kernel-doc:: include/linux/log2.h
   :internal:

Integer log and power Functions(整数对数与幂函数)
-------------------------------------------------

.. kernel-doc:: include/linux/int_log.h

.. kernel-doc:: lib/math/int_pow.c
   :export:

.. kernel-doc:: lib/math/int_sqrt.c
   :export:

Division Functions(除法函数)
----------------------------

.. kernel-doc:: include/asm-generic/div64.h
   :functions: do_div

.. kernel-doc:: include/linux/math64.h
   :internal:

.. kernel-doc:: lib/math/gcd.c
   :export:

UUID/GUID(UUID/GUID 标识符)
---------------------------

.. kernel-doc:: lib/uuid.c
   :export:

Kernel IPC facilities(内核 IPC 设施)
====================================

IPC utilities(IPC 实用工具)
---------------------------

.. kernel-doc:: ipc/util.c
   :internal:

FIFO Buffer(FIFO 缓冲区)
========================

kfifo interface(kfifo 接口)
---------------------------

.. kernel-doc:: include/linux/kfifo.h
   :internal:

relay interface support(relay 接口支持)
=======================================

Relay interface support is designed to provide an efficient mechanism
for tools and facilities to relay large amounts of data from kernel
space to user space.
(relay 接口支持旨在为各种工具与设施提供一种高效机制，用于把大量数据从内核空间中继到用户空间。)

relay interface(relay 接口)
---------------------------

.. kernel-doc:: kernel/relay.c
   :export:

.. kernel-doc:: kernel/relay.c
   :internal:

Module Support(模块支持)
========================

Kernel module auto-loading(内核模块自动加载)
--------------------------------------------

.. kernel-doc:: kernel/module/kmod.c
   :export:

Module debugging(模块调试)
--------------------------

.. kernel-doc:: kernel/module/stats.c
   :doc: module debugging statistics overview

dup_failed_modules - tracks duplicate failed modules(dup_failed_modules：跟踪重复的失败模块)
********************************************************************************************

.. kernel-doc:: kernel/module/stats.c
   :doc: dup_failed_modules - tracks duplicate failed modules

module statistics debugfs counters(module statistics debugfs 计数器)
********************************************************************

.. kernel-doc:: kernel/module/stats.c
   :doc: module statistics debugfs counters

Inter Module support(模块间支持)
--------------------------------

Refer to the files in kernel/module/ for more information.
(更多信息请参阅 kernel/module/ 中的文件。)

Hardware Interfaces(硬件接口)
=============================

DMA Channels(DMA 通道)
----------------------

.. kernel-doc:: kernel/dma.c
   :export:

Resources Management(资源管理)
------------------------------

.. kernel-doc:: kernel/resource.c
   :internal:

.. kernel-doc:: kernel/resource.c
   :export:

MTRR Handling(MTRR 处理)
------------------------

.. kernel-doc:: arch/x86/kernel/cpu/mtrr/mtrr.c
   :export:

Security Framework(安全框架)
============================

.. kernel-doc:: security/security.c
   :internal:

.. kernel-doc:: security/inode.c
   :export:

Audit Interfaces(审计接口)
==========================

.. kernel-doc:: kernel/audit.c
   :export:

.. kernel-doc:: kernel/auditsc.c
   :internal:

.. kernel-doc:: kernel/auditfilter.c
   :internal:

Accounting Framework(记账框架)
==============================

.. kernel-doc:: kernel/acct.c
   :internal:

Block Devices(块设备)
=====================

.. kernel-doc:: include/linux/bio.h
.. kernel-doc:: block/blk-core.c
   :export:

.. kernel-doc:: block/blk-core.c
   :internal:

.. kernel-doc:: block/blk-map.c
   :export:

.. kernel-doc:: block/blk-sysfs.c
   :internal:

.. kernel-doc:: block/blk-settings.c
   :export:

.. kernel-doc:: block/blk-flush.c
   :export:

.. kernel-doc:: block/blk-lib.c
   :export:

.. kernel-doc:: block/blk-integrity.c
   :export:

.. kernel-doc:: kernel/trace/blktrace.c
   :internal:

.. kernel-doc:: block/genhd.c
   :internal:

.. kernel-doc:: block/genhd.c
   :export:

.. kernel-doc:: block/bdev.c
   :export:

Char devices(字符设备)
======================

.. kernel-doc:: fs/char_dev.c
   :export:

Clock Framework(时钟框架)
=========================

The clock framework defines programming interfaces to support software
management of the system clock tree. This framework is widely used with
System-On-Chip (SOC) platforms to support power management and various
devices which may need custom clock rates. Note that these "clocks"
don't relate to timekeeping or real time clocks (RTCs), each of which
have separate frameworks. These :c:type:`struct clk <clk>`
instances may be used to manage for example a 96 MHz signal that is used
to shift bits into and out of peripherals or busses, or otherwise
trigger synchronous state machine transitions in system hardware.
(时钟框架定义了一些编程接口，用于支持对系统时钟树的软件管理。该框架广泛用于片上系统(SOC)平台，以支持电源管理以及各种可能需要自定义时钟频率的设备。注意，这些 "时钟" 与计时(timekeeping)或实时时钟(RTC)无关——它们各有自己独立的框架。这些 :c:type:`struct clk <clk>` 实例可用于管理例如 96 MHz 的信号——这种信号用于把位串行移入或移出外设或总线，或者以其他方式触发系统硬件中同步状态机的转换。)

Power management is supported by explicit software clock gating: unused
clocks are disabled, so the system doesn't waste power changing the
state of transistors that aren't in active use. On some systems this may
be backed by hardware clock gating, where clocks are gated without being
disabled in software. Sections of chips that are powered but not clocked
may be able to retain their last state. This low power state is often
called a *retention mode*. This mode still incurs leakage currents,
especially with finer circuit geometries, but for CMOS circuits power is
mostly used by clocked state changes.
(电源管理通过显式的软件时钟门控来支持：未使用的时钟被禁用，这样系统就不会浪费电力去改变并非活跃使用的晶体管的状态。在某些系统上，这可以由硬件时钟门控支撑——时钟被门控但在软件中并未禁用。已上电但未提供时钟的芯片区段或许能保留其最后状态。这种低功耗状态通常称为 *保持模式* (retention mode)。这种模式仍有漏电流，特别是在更精细的电路几何尺寸下，但对 CMOS 电路而言，电力主要消耗在时钟驱动的状态变化上。)

Power-aware drivers only enable their clocks when the device they manage
is in active use. Also, system sleep states often differ according to
which clock domains are active: while a "standby" state may allow wakeup
from several active domains, a "mem" (suspend-to-RAM) state may require
a more wholesale shutdown of clocks derived from higher speed PLLs and
oscillators, limiting the number of possible wakeup event sources. A
driver's suspend method may need to be aware of system-specific clock
constraints on the target sleep state.
(功耗感知的驱动只在其管理的设备处于活跃使用时才启用自己的时钟。此外，系统睡眠状态也常常因哪些时钟域处于活跃而不同： "standby" (待机)状态可以允许从多个活跃域唤醒，而 "mem" (挂起到内存)状态则可能要求更彻底地关闭来自较高速 PLL 与振荡器的时钟，从而限制可能的唤醒事件源的数量。驱动的 suspend 方法可能需要了解目标睡眠状态上系统特定的时钟约束。)

Some platforms support programmable clock generators. These can be used
by external chips of various kinds, such as other CPUs, multimedia
codecs, and devices with strict requirements for interface clocking.
(一些平台支持可编程时钟发生器。它们可以供各种外部芯片使用，例如其他 CPU、多媒体编解码器，以及对接口时钟有严格要求的设备。)

.. kernel-doc:: include/linux/clk.h
   :internal:

Synchronization Primitives(同步原语)
====================================

Read-Copy Update (RCU)(读-复制更新(RCU))
----------------------------------------

.. kernel-doc:: include/linux/rcupdate.h

.. kernel-doc:: kernel/rcu/tree.c

.. kernel-doc:: kernel/rcu/tree_exp.h

.. kernel-doc:: kernel/rcu/update.c

.. kernel-doc:: include/linux/srcu.h

.. kernel-doc:: kernel/rcu/srcutree.c

.. kernel-doc:: include/linux/rculist_bl.h

.. kernel-doc:: include/linux/rculist.h

.. kernel-doc:: include/linux/rculist_nulls.h

.. kernel-doc:: include/linux/rcu_sync.h

.. kernel-doc:: kernel/rcu/sync.c

.. kernel-doc:: kernel/rcu/tasks.h

.. kernel-doc:: kernel/rcu/tree_stall.h

.. kernel-doc:: include/linux/rcupdate_trace.h

.. kernel-doc:: include/linux/rcupdate_wait.h

.. kernel-doc:: include/linux/rcuref.h

.. kernel-doc:: include/linux/rcutree.h
