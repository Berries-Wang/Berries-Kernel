.. SPDX-License-Identifier: GPL-2.0

=========================================================
Virtually Mapped Kernel Stack Support(虚拟映射内核栈支持)
=========================================================

:Author: Shuah Khan <skhan@linuxfoundation.org>
(作者：Shuah Khan <skhan@linuxfoundation.org>)

.. contents:: :local:

Overview(概述)
---------------

This is a compilation of information from the code and original patch
series that introduced the `Virtually Mapped Kernel Stacks feature
<https://lwn.net/Articles/694348/>`
(本文汇集了来自代码以及引入 `Virtually Mapped Kernel Stacks feature
<https://lwn.net/Articles/694348/>` 的原始补丁系列的信息。)

Introduction(引言)
------------------

Kernel stack overflows are often hard to debug and make the kernel
susceptible to exploits. Problems could show up at a later time making
it difficult to isolate and root-cause.
(内核栈溢出通常难以调试，并使内核容易被利用。问题可能在稍后的时间才显现，
这使得隔离和根因分析变得困难。)

Virtually mapped kernel stacks with guard pages cause kernel stack
overflows to be caught immediately rather than causing difficult to
diagnose corruptions.
(带保护页的虚拟映射内核栈能让内核栈溢出被立即捕获，而不是造成难以诊断的
损坏。)

HAVE_ARCH_VMAP_STACK and VMAP_STACK configuration options enable
support for virtually mapped stacks with guard pages. This feature
causes reliable faults when the stack overflows. The usability of
the stack trace after overflow and response to the overflow itself
is architecture dependent.
(HAVE_ARCH_VMAP_STACK 和 VMAP_STACK 配置选项启用对带保护页的虚拟映射栈的
支持。此特性在栈溢出时引发可靠的缺页错误。溢出后栈回溯的可用性以及对溢出
本身的响应如何，取决于具体架构。)

.. note::
        As of this writing, arm64, powerpc, riscv, s390, um, and x86 have
        support for VMAP_STACK.
        (截至撰写本文时，arm64、powerpc、riscv、s390、um 和 x86 已支持
        VMAP_STACK。)

HAVE_ARCH_VMAP_STACK(原名)
--------------------------

Architectures that can support Virtually Mapped Kernel Stacks should
enable this bool configuration option. The requirements are:
(能够支持虚拟映射内核栈的架构应启用这个布尔配置选项。要求如下：)

- vmalloc space must be large enough to hold many kernel stacks. This
  may rule out many 32-bit architectures.
  (vmalloc 空间必须足够大，以容纳许多内核栈。这可能会排除许多 32 位
  架构。)
- Stacks in vmalloc space need to work reliably.  For example, if
  vmap page tables are created on demand, either this mechanism
  needs to work while the stack points to a virtual address with
  unpopulated page tables or arch code (switch_to() and switch_mm(),
  most likely) needs to ensure that the stack's page table entries
  are populated before running on a possibly unpopulated stack.
  (vmalloc 空间中的栈需要可靠地工作。例如，如果 vmap 页表是按需创建的，
  那么要么这一机制需要在栈指向一个页表尚未填充的虚拟地址时仍能工作，
  要么架构代码（很可能是 switch_to() 和 switch_mm()）需要确保在运行于
  可能未填充页表的栈上之前，栈的页表项已被填充。)
- If the stack overflows into a guard page, something reasonable
  should happen. The definition of "reasonable" is flexible, but
  instantly rebooting without logging anything would be unfriendly.
  (如果栈溢出进入保护页，应当发生某种合理的事情。"合理"的定义是灵活的，
  但不记录任何日志就立即重启是不友好的。)

VMAP_STACK(原名)
----------------

When enabled, the VMAP_STACK bool configuration option allocates virtually
mapped task stacks. This option depends on HAVE_ARCH_VMAP_STACK.
(启用后，VMAP_STACK 布尔配置选项会分配虚拟映射的任务栈。此选项依赖于
HAVE_ARCH_VMAP_STACK。)

- Enable this if you want the use virtually-mapped kernel stacks
  with guard pages. This causes kernel stack overflows to be caught
  immediately rather than causing difficult-to-diagnose corruption.
  (如果你希望使用带保护页的虚拟映射内核栈，则启用此项。这会使内核栈溢出
  被立即捕获，而不是造成难以诊断的损坏。)

.. note::

        Using this feature with KASAN requires architecture support
        for backing virtual mappings with real shadow memory, and
        KASAN_VMALLOC must be enabled.
        (将此特性与 KASAN 一起使用，需要架构支持用真实的影子内存
        (shadow memory) 来支撑虚拟映射，并且必须启用 KASAN_VMALLOC。)

.. note::

        VMAP_STACK is enabled, it is not possible to run DMA on stack
        allocated data.
        (启用 VMAP_STACK 后，无法对栈上分配的数据执行 DMA。)

Kernel configuration options and dependencies keep changing. Refer to
the latest code base:
(内核配置选项和依赖关系不断变化。请参考最新的代码库：)

`Kconfig <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/Kconfig>`

Allocation(分配)
----------------

When a new kernel thread is created, a thread stack is allocated from
virtually contiguous memory pages from the page level allocator. These
pages are mapped into contiguous kernel virtual space with PAGE_KERNEL
protections.
(创建新内核线程时，线程栈从页级分配器的虚拟连续内存页中分配。这些页以
PAGE_KERNEL 保护被映射到连续的内核虚拟空间。)

alloc_thread_stack_node() calls __vmalloc_node_range() to allocate stack
with PAGE_KERNEL protections.
(alloc_thread_stack_node() 调用 __vmalloc_node_range()，以 PAGE_KERNEL
保护分配栈。)

- Allocated stacks are cached and later reused by new threads, so memcg
  accounting is performed manually on assigning/releasing stacks to tasks.
  Hence, __vmalloc_node_range is called without __GFP_ACCOUNT.
  (分配的栈会被缓存并稍后由新线程复用，因此在把栈分配/释放给任务时需要
  手动进行 memcg 记账。因此，调用 __vmalloc_node_range 时不带
  __GFP_ACCOUNT。)
- vm_struct is cached to be able to find when thread free is initiated
  in interrupt context. free_thread_stack() can be called in interrupt
  context.
  (缓存 vm_struct 是为了能够发现线程释放何时在中断上下文中启动。
  free_thread_stack() 可以在中断上下文中被调用。)
- On arm64, all VMAP's stacks need to have the same alignment to ensure
  that VMAP'd stack overflow detection works correctly. Arch specific
  vmap stack allocator takes care of this detail.
  (在 arm64 上，所有 VMAP 的栈需要具有相同的对齐，以确保 VMAP 栈溢出
  检测正确工作。架构特定的 vmap 栈分配器负责处理这一细节。)
- This does not address interrupt stacks - according to the original patch
  (这不涉及中断栈——参见原始补丁。)

Thread stack allocation is initiated from clone(), fork(), vfork(),
kernel_thread() via kernel_clone(). These are a few hints for searching
the code base to understand when and how a thread stack is allocated.
(线程栈分配由 clone()、fork()、vfork()、kernel_thread() 经由
kernel_clone() 发起。这些是在代码库中搜索、以了解线程栈何时以及如何被
分配的一些线索。)

Bulk of the code is in:
`kernel/fork.c <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/kernel/fork.c>`.
(大部分代码位于：)

stack_vm_area pointer in task_struct keeps track of the virtually allocated
stack and a non-null stack_vm_area pointer serves as an indication that the
virtually mapped kernel stacks are enabled.
(task_struct 中的 stack_vm_area 指针跟踪虚拟分配的栈；非空的 stack_vm_area
指针表明虚拟映射内核栈已启用。)

::

        struct vm_struct *stack_vm_area;

Stack overflow handling(栈溢出处理)
-----------------------------------

Leading and trailing guard pages help detect stack overflows. When the stack
overflows into the guard pages, handlers have to be careful not to overflow
the stack again. When handlers are called, it is likely that very little
stack space is left.
(前置和尾随保护页有助于检测栈溢出。当栈溢出进入保护页时，处理程序必须
小心，不要再次溢出栈。处理程序被调用时，剩余的栈空间很可能已经非常少。)

On x86, this is done by handling the page fault indicating the kernel
stack overflow on the double-fault stack.
(在 x86 上，这是通过在双重故障栈 (double-fault stack) 上处理指示内核栈
溢出的缺页错误来完成的。)

Testing VMAP allocation with guard pages(用保护页测试 VMAP 分配)
----------------------------------------------------------------

How do we ensure that VMAP_STACK is actually allocating with a leading
and trailing guard page? The following lkdtm tests can help detect any
regressions.
(我们如何确保 VMAP_STACK 确实在分配时带有前置和尾随保护页？以下 lkdtm
测试有助于检测任何回归。)

::

        void lkdtm_STACK_GUARD_PAGE_LEADING()
        void lkdtm_STACK_GUARD_PAGE_TRAILING()

Conclusions(结论)
-----------------

- A percpu cache of vmalloced stacks appears to be a bit faster than a
  high-order stack allocation, at least when the cache hits.
  (vmalloced 栈的 percpu 缓存似乎比高阶栈分配稍快，至少在缓存命中时
  如此。)
- THREAD_INFO_IN_TASK gets rid of arch-specific thread_info entirely and
  simply embed the thread_info (containing only flags) and 'int cpu' into
  task_struct.
  (THREAD_INFO_IN_TASK 完全摆脱了架构特定的 thread_info，只是把
  thread_info（仅包含标志）和 "int cpu" 嵌入 task_struct。)
- The thread stack can be freed as soon as the task is dead (without
  waiting for RCU) and then, if vmapped stacks are in use, cache the
  entire stack for reuse on the same cpu.
  (线程栈可以在任务死亡后立即释放（无需等待 RCU）；然后，如果正在使用
  vmapped 栈，则把整个栈缓存起来，以便在同一 CPU 上复用。)
