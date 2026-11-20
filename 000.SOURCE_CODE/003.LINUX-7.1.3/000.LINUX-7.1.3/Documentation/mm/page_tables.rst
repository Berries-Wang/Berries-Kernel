.. SPDX-License-Identifier: GPL-2.0

=================
Page Tables(页表)
=================

Paged virtual memory was invented along with virtual memory as a concept in
1962 on the Ferranti Atlas Computer which was the first computer with paged
virtual memory. The feature migrated to newer computers and became a de facto
feature of all Unix-like systems as time went by. In 1985 the feature was
included in the Intel 80386, which was the CPU Linux 1.0 was developed on.
(分页虚拟内存是伴随虚拟内存概念于 1962 年在 Ferranti Atlas 计算机上问世的，
它是第一台具有分页虚拟内存的计算机。随着时间的推移，这一特性迁移到更新的
计算机上，并成为所有类 Unix 系统的事实特性。1985 年，该特性被纳入
Intel 80386 —— Linux 1.0 就是在这款 CPU 上开发的。)

Page tables map virtual addresses as seen by the CPU into physical addresses
as seen on the external memory bus.
(页表把 CPU 所见的虚拟地址映射为外部内存总线上所见物理地址。)

Linux defines page tables as a hierarchy which is currently five levels in
height. The architecture code for each supported architecture will then
map this to the restrictions of the hardware.
(Linux 把页表定义为一个层级结构，目前高度为五级。每种受支持架构的架构
代码会把它映射到该硬件的限制之上。)

The physical address corresponding to the virtual address is often referenced
by the underlying physical page frame. The **page frame number** or **pfn**
is the physical address of the page (as seen on the external memory bus)
divided by `PAGE_SIZE`.
(与虚拟地址对应的物理地址，通常通过底层的物理页框来引用。**页框号**
(page frame number) 或 **pfn** 是页的物理地址 (在外部内存总线上所见)
除以 `PAGE_SIZE` 得到的结果。)

Physical memory address 0 will be *pfn 0* and the highest pfn will be
the last page of physical memory the external address bus of the CPU can
address.
(物理内存地址 0 将是 *pfn 0*，而最高的 pfn 将是 CPU 的外部地址总线所能
寻址的物理内存的最后一页。)

With a page granularity of 4KB and an address range of 32 bits, pfn 0 is at
address 0x00000000, pfn 1 is at address 0x00001000, pfn 2 is at 0x00002000
and so on until we reach pfn 0xfffff at 0xfffff000. With 16KB pages pfns are
at 0x00004000, 0x00008000 ... 0xffffc000 and pfn goes from 0 to 0x3ffff.
(页粒度为 4KB、地址范围为 32 位时，pfn 0 位于地址 0x00000000，pfn 1 位于
0x00001000，pfn 2 位于 0x00002000，依此类推，直到 pfn 0xfffff 位于
0xfffff000。使用 16KB 页时，pfn 位于 0x00004000、0x00008000 ……
0xffffc000，pfn 从 0 到 0x3ffff。)

As you can see, with 4KB pages the page base address uses bits 12-31 of the
address, and this is why `PAGE_SHIFT` in this case is defined as 12 and
`PAGE_SIZE` is usually defined in terms of the page shift as `(1 << PAGE_SHIFT)`
(可以看到，使用 4KB 页时，页基地址使用地址的位 12-31，这就是为什么这种
情况下 `PAGE_SHIFT` 被定义为 12，而 `PAGE_SIZE` 通常用页移位来定义，即
`(1 << PAGE_SHIFT)`。)

Over time a deeper hierarchy has been developed in response to increasing memory
sizes. When Linux was created, 4KB pages and a single page table called
`swapper_pg_dir` with 1024 entries was used, covering 4MB which coincided with
the fact that Torvalds's first computer had 4MB of physical memory. Entries in
this single table were referred to as *PTEs* - page table entries.
(随着时间推移，为了应对不断增长的内存容量，更深的层级被发展出来。Linux
创建时使用 4KB 页和一个名为 `swapper_pg_dir` 的单级页表，它有 1024 个
表项，覆盖 4MB —— 这恰好与 Torvalds 第一台计算机拥有 4MB 物理内存相
符。这张单一表中的表项被称为 *PTE* —— 页表项 (page table entries)。)

The software page table hierarchy reflects the fact that page table hardware has
become hierarchical and that in turn is done to save page table memory and
speed up mapping.
(软件页表层级反映了页表硬件已经分层化的事实，而这样做的目的又是为了
节省页表内存并加速映射。)

One could of course imagine a single, linear page table with enormous amounts
of entries, breaking down the whole memory into single pages. Such a page table
would be very sparse, because large portions of the virtual memory usually
remains unused. By using hierarchical page tables large holes in the virtual
address space does not waste valuable page table memory, because it will suffice
to mark large areas as unmapped at a higher level in the page table hierarchy.
(当然，人们可以想象一个拥有海量条目的单一线性页表，把整个内存拆分为单个
页。这样的页表会非常稀疏，因为虚拟内存的大部分通常未被使用。通过使用
分层页表，虚拟地址空间中的大空洞不会浪费宝贵的页表内存，因为只需在页表
层级的更高层把大片区域标记为未映射即可。)

Additionally, on modern CPUs, a higher level page table entry can point directly
to a physical memory range, which allows mapping a contiguous range of several
megabytes or even gigabytes in a single high-level page table entry, taking
shortcuts in mapping virtual memory to physical memory: there is no need to
traverse deeper in the hierarchy when you find a large mapped range like this.
(此外，在现代 CPU 上，更高层的页表项可以直接指向一个物理内存范围，从而
允许在单个高层页表项中映射数兆字节甚至数 GB 的连续范围，在虚拟内存到
物理内存的映射中抄近路：当找到这样的大范围已映射区域时，无需再深入遍历
层级。)

The page table hierarchy has now developed into this::

  +-----+
  | PGD |
  +-----+
     |
     |   +-----+
     +-->| P4D |
         +-----+
            |
            |   +-----+
            +-->| PUD |
                +-----+
                   |
                   |   +-----+
                   +-->| PMD |
                       +-----+
                          |
                          |   +-----+
                          +-->| PTE |
                              +-----+

(页表层级如今已经发展成如下形式：)

Symbols on the different levels of the page table hierarchy have the following
meaning beginning from the bottom:
(页表层级中不同层级上的符号，从最底层开始有以下含义：)

- **pte**, `pte_t`, `pteval_t` = **Page Table Entry** - mentioned earlier.
  The *pte* is an array of `PTRS_PER_PTE` elements of the `pteval_t` type, each
  mapping a single page of virtual memory to a single page of physical memory.
  The architecture defines the size and contents of `pteval_t`.
  ( **pte**、`pte_t`、`pteval_t` = **页表项 (Page Table Entry)** - 前文已经
  提到。*pte* 是由 `PTRS_PER_PTE` 个 `pteval_t` 类型元素组成的数组，每个
  元素把一页虚拟内存映射到一页物理内存。架构定义 `pteval_t` 的大小与
  内容。)

  A typical example is that the `pteval_t` is a 32- or 64-bit value with the
  upper bits being a **pfn** (page frame number), and the lower bits being some
  architecture-specific bits such as memory protection.
  (一个典型的例子是：`pteval_t` 是一个 32 位或 64 位的值，其高位是
  **pfn** (页框号)，低位是一些架构特定的位，例如内存保护。)

  The **entry** part of the name is a bit confusing because while in Linux 1.0
  this did refer to a single page table entry in the single top level page
  table, it was retrofitted to be an array of mapping elements when two-level
  page tables were first introduced, so the *pte* is the lowermost page
  *table*, not a page table *entry*.
  (名字中的 **entry** 部分有点令人困惑：虽然在 Linux 1.0 中它确实指的是
  单一顶层页表中的单个页表项，但当两级页表首次引入时，它被改造为表示映射
  元素的数组，所以 *pte* 是最底层的页 *表*，而不是页表 *项*。)

- **pmd**, `pmd_t`, `pmdval_t` = **Page Middle Directory**, the hierarchy right
  above the *pte*, with `PTRS_PER_PMD` references to the *pte*:s.
  ( **pmd**、`pmd_t`、`pmdval_t` = **页中间目录 (Page Middle Directory)**，
  即紧邻 *pte* 之上的层级，带有 `PTRS_PER_PMD` 个指向 *pte* 的引用。)

- **pud**, `pud_t`, `pudval_t` = **Page Upper Directory** was introduced after
  the other levels to handle 4-level page tables. It is potentially unused,
  or *folded* as we will discuss later.
  ( **pud**、`pud_t`、`pudval_t` = **页上层目录 (Page Upper Directory)**，
  是在其他层级之后为处理 4 级页表而引入的。它有可能未被使用，或者如后文
  所述被 *折叠*。)

- **p4d**, `p4d_t`, `p4dval_t` = **Page Level 4 Directory** was introduced to
  handle 5-level page tables after the *pud* was introduced. Now it was clear
  that we needed to replace *pgd*, *pmd*, *pud* etc with a figure indicating the
  directory level and that we cannot go on with ad hoc names any more. This
  is only used on systems which actually have 5 levels of page tables, otherwise
  it is folded.
  ( **p4d**、`p4d_t`、`p4dval_t` = **第 4 级页目录 (Page Level 4
  Directory)**，是在 *pud* 之后为处理 5 级页表而引入的。此时人们清楚地
  认识到，需要用指示目录层级的数字来替换 *pgd*、*pmd*、*pud* 等，不能再
  继续使用这些临时起的名字了。它只在真正拥有 5 级页表的系统上使用，否则
  会被折叠。)

- **pgd**, `pgd_t`, `pgdval_t` = **Page Global Directory** - the Linux kernel
  main page table handling the PGD for the kernel memory is still found in
  `swapper_pg_dir`, but each userspace process in the system also has its own
  memory context and thus its own *pgd*, found in `struct mm_struct` which
  in turn is referenced to in each `struct task_struct`. So tasks have memory
  context in the form of a `struct mm_struct` and this in turn has a
  `struct pgt_t *pgd` pointer to the corresponding page global directory.
  ( **pgd**、`pgd_t`、`pgdval_t` = **页全局目录 (Page Global Directory)**
  - Linux 内核主页表：处理内核内存 PGD 的表仍然位于 `swapper_pg_dir` 中，
  但系统中的每个用户空间进程也拥有自己的内存上下文，因而拥有自己的
  *pgd*，它位于 `struct mm_struct` 中，而后者又被每个 `struct
  task_struct` 引用。因此，任务以 `struct mm_struct` 的形式拥有内存
  上下文，而它又有一个 `struct pgt_t *pgd` 指针指向对应的页全局目录。)

To repeat: each level in the page table hierarchy is a *array of pointers*, so
the **pgd** contains `PTRS_PER_PGD` pointers to the next level below, **p4d**
contains `PTRS_PER_P4D` pointers to **pud** items and so on. The number of
pointers on each level is architecture-defined.::

        PMD
  --> +-----+           PTE
      | ptr |-------> +-----+
      | ptr |-        | ptr |-------> PAGE
      | ptr | \       | ptr |
      | ptr |  \        ...
      | ... |   \
      | ptr |    \         PTE
      +-----+     +----> +-----+
                         | ptr |-------> PAGE
                         | ptr |
                           ...

(再重复一遍：页表层级的每一层都是一个 *指针数组*，因此 **pgd** 包含
`PTRS_PER_PGD` 个指向下一层的指针，**p4d** 包含 `PTRS_PER_P4D` 个指向
**pud** 条目的指针，依此类推。每一层指针的数量由架构定义。)

Page Table Folding(页表折叠)
============================

If the architecture does not use all the page table levels, they can be *folded*
which means skipped, and all operations performed on page tables will be
compile-time augmented to just skip a level when accessing the next lower
level.
(如果架构不使用全部的页表层级，它们可以被 *折叠*，这意味着被跳过；对页
表执行的所有操作都会在编译时被改写，从而在访问下一个更低层级时直接跳过
某一层。)

Page table handling code that wishes to be architecture-neutral, such as the
virtual memory manager, will need to be written so that it traverses all of the
currently five levels. This style should also be preferred for
architecture-specific code, so as to be robust to future changes.
(希望做到架构无关的页表处理代码，例如虚拟内存管理器，需要写成遍历当前
全部五级的方式。这种风格对架构特定代码也应当是首选，以便对未来变化保持
健壮。)


MMU, TLB, and Page Faults(MMU、TLB 与页错误)
============================================

The `Memory Management Unit (MMU)` is a hardware component that handles virtual
to physical address translations. It may use relatively small caches in hardware
called `Translation Lookaside Buffers (TLBs)` and `Page Walk Caches` to speed up
these translations.
(内存管理单元 (MMU) 是一个处理虚拟地址到物理地址转换的硬件组件。它可以
使用硬件中一些相对较小的缓存来加速这些转换，这些缓存称为 `Translation
Lookaside Buffers (TLBs)` (旁路转换缓冲) 与 `页遍历缓存 (Page Walk
Caches)`。)

When CPU accesses a memory location, it provides a virtual address to the MMU,
which checks if there is the existing translation in the TLB or in the Page
Walk Caches (on architectures that support them). If no translation is found,
MMU uses the page walks to determine the physical address and create the map.
(当 CPU 访问一个内存位置时，它向 MMU 提供一个虚拟地址，MMU 检查 TLB 或
页遍历缓存 (在支持它们的架构上) 中是否已有现存的转换。如果没有找到转换，
MMU 使用页遍历 (page walk) 来确定物理地址并建立映射。)

The dirty bit for a page is set (i.e., turned on) when the page is written to.
Each page of memory has associated permission and dirty bits. The latter
indicate that the page has been modified since it was loaded into memory.
(页的脏位 (dirty bit) 在页被写入时被置位 (即打开)。每一页内存都有相关联
的权限位与脏位。后者表明该页自被加载到内存以来已被修改。)

If nothing prevents it, eventually the physical memory can be accessed and the
requested operation on the physical frame is performed.
(如果没有东西阻止它，最终物理内存可以被访问，并对物理页框执行请求的
操作。)

There are several reasons why the MMU can't find certain translations. It could
happen because the CPU is trying to access memory that the current task is not
permitted to, or because the data is not present into physical memory.
(MMU 找不到某些转换的原因有多种。可能是因为 CPU 正试图访问当前任务不被
允许访问的内存，或者因为数据不在物理内存中。)

When these conditions happen, the MMU triggers page faults, which are types of
exceptions that signal the CPU to pause the current execution and run a special
function to handle the mentioned exceptions.
(当这些情况发生时，MMU 会触发页错误 (page fault)，这是一类异常，它向
CPU 发出信号，要求暂停当前执行并运行一个特殊函数来处理所述异常。)

There are common and expected causes of page faults. These are triggered by
process management optimization techniques called "Lazy Allocation" and
"Copy-on-Write". Page faults may also happen when frames have been swapped out
to persistent storage (swap partition or file) and evicted from their physical
locations.
(页错误有一些常见且符合预期的起因。它们由称为 "惰性分配" (Lazy
Allocation) 与 "写时复制" (Copy-on-Write) 的进程管理优化技术触发。当
页框已被换出到持久存储 (交换分区或文件) 并从其物理位置被逐出时，也可能
发生页错误。)

These techniques improve memory efficiency, reduce latency, and minimize space
occupation. This document won't go deeper into the details of "Lazy Allocation"
and "Copy-on-Write" because these subjects are out of scope as they belong to
Process Address Management.
(这些技术提高了内存效率、降低了延迟并最小化了空间占用。本文档不会深入
探讨 "惰性分配" 与 "写时复制" 的细节，因为这些主题属于进程地址管理，
超出了本文的范围。)

Swapping differentiates itself from the other mentioned techniques because it's
undesirable since it's performed as a means to reduce memory under heavy
pressure.
(换出 (Swapping) 与前面提到的其他技术不同，因为它是不受欢迎的：它是在
内存压力很大时，作为一种减少内存占用的手段来执行的。)

Swapping can't work for memory mapped by kernel logical addresses. These are a
subset of the kernel virtual space that directly maps a contiguous range of
physical memory. Given any logical address, its physical address is determined
with simple arithmetic on an offset. Accesses to logical addresses are fast
because they avoid the need for complex page table lookups at the expenses of
frames not being evictable and pageable out.
(换出对由内核逻辑地址映射的内存不起作用。这些地址是内核虚拟空间的一个
子集，直接映射一段连续的物理内存范围。给定任意逻辑地址，其物理地址可以
通过对偏移量做简单算术运算来确定。对逻辑地址的访问很快，因为它们避免了
复杂的页表查找，代价是这些页框不可被逐出和换出。)

If the kernel fails to make room for the data that must be present in the
physical frames, the kernel invokes the out-of-memory (OOM) killer to make room
by terminating lower priority processes until pressure reduces under a safe
threshold.
(如果内核无法为必须驻留在物理页框中的数据腾出空间，内核会调用内存耗尽
(OOM) killer，通过终止较低优先级的进程来腾出空间，直到压力降到安全阈值
以下。)

Additionally, page faults may be also caused by code bugs or by maliciously
crafted addresses that the CPU is instructed to access. A thread of a process
could use instructions to address (non-shared) memory which does not belong to
its own address space, or could try to execute an instruction that wants to write
to a read-only location.
(此外，页错误也可能由代码 bug 或由 CPU 被指示访问的恶意构造的地址引起。
进程的线程可能使用指令去寻址不属于自己地址空间的 (非共享) 内存，或者
可能试图执行想要写入只读位置的指令。)

If the above-mentioned conditions happen in user-space, the kernel sends a
`Segmentation Fault` (SIGSEGV) signal to the current thread. That signal usually
causes the termination of the thread and of the process it belongs to.
(如果上述情况发生在用户空间，内核会向当前线程发送 `Segmentation Fault`
(SIGSEGV) 信号。该信号通常导致该线程及其所属进程终止。)

This document is going to simplify and show an high altitude view of how the
Linux kernel handles these page faults, creates tables and tables' entries,
check if memory is present and, if not, requests to load data from persistent
storage or from other devices, and updates the MMU and its caches.
(本文档将简化并从高空视角展示 Linux 内核如何处理这些页错误、创建表及表
的条目、检查内存是否存在，如果不存在则请求从持久存储或其他设备加载数据，
并更新 MMU 及其缓存。)

The first steps are architecture dependent. Most architectures jump to
`do_page_fault()`, whereas the x86 interrupt handler is defined by the
`DEFINE_IDTENTRY_RAW_ERRORCODE()` macro which calls `handle_page_fault()`.
(最初的步骤依赖架构。大多数架构跳转到 `do_page_fault()`，而 x86 的中断
处理函数由 `DEFINE_IDTENTRY_RAW_ERRORCODE()` 宏定义，它调用
`handle_page_fault()`。)

Whatever the routes, all architectures end up to the invocation of
`handle_mm_fault()` which, in turn, (likely) ends up calling
`__handle_mm_fault()` to carry out the actual work of allocating the page
tables.
(无论走哪条路线，所有架构最终都会调用 `handle_mm_fault()`，而它 (很可能)
又最终调用 `__handle_mm_fault()` 来执行分配页表的实际工作。)

The unfortunate case of not being able to call `__handle_mm_fault()` means
that the virtual address is pointing to areas of physical memory which are not
permitted to be accessed (at least from the current context). This
condition resolves to the kernel sending the above-mentioned SIGSEGV signal
to the process and leads to the consequences already explained.
(无法调用 `__handle_mm_fault()` 的不幸情况意味着虚拟地址指向不允许访问
(至少从当前上下文不允许) 的物理内存区域。这种情况最终表现为内核向进程
发送上述 SIGSEGV 信号，并导致已经解释过的后果。)

`__handle_mm_fault()` carries out its work by calling several functions to
find the entry's offsets of the upper layers of the page tables and allocate
the tables that it may need.
(`__handle_mm_fault()` 通过调用若干函数来完成其工作：查找页表各上层的
条目偏移，并分配它可能需要的表。)

The functions that look for the offset have names like `*_offset()`, where the
"*" is for pgd, p4d, pud, pmd, pte; instead the functions to allocate the
corresponding tables, layer by layer, are called `*_alloc`, using the
above-mentioned convention to name them after the corresponding types of tables
in the hierarchy.
(查找偏移的函数名字形如 `*_offset()`，其中 "*" 是 pgd、p4d、pud、pmd、
pte；而逐层分配相应表的函数则称为 `*_alloc`，按照上述约定，以层级中
对应的表类型来命名。)

The page table walk may end at one of the middle or upper layers (PMD, PUD).
(页表遍历可能在中间或上层 (PMD、PUD) 结束。)

Linux supports larger page sizes than the usual 4KB (i.e., the so called
`huge pages`). When using these kinds of larger pages, higher level pages can
directly map them, with no need to use lower level page entries (PTE). Huge
pages contain large contiguous physical regions that usually span from 2MB to
1GB. They are respectively mapped by the PMD and PUD page entries.
(Linux 支持比通常的 4KB 更大的页尺寸 (即所谓的 `huge pages`)。使用这类
更大的页时，更高层的页可以直接映射它们，无需使用更低层的页表项 (PTE)。
巨页包含大块的连续物理区域，通常从 2MB 到 1GB 不等。它们分别由 PMD 与
PUD 页表项映射。)

The huge pages bring with them several benefits like reduced TLB pressure,
reduced page table overhead, memory allocation efficiency, and performance
improvement for certain workloads. However, these benefits come with
trade-offs, like wasted memory and allocation challenges.
(巨页带来了若干好处，例如降低 TLB 压力、降低页表开销、提高内存分配
效率以及某些工作负载的性能提升。不过，这些好处伴随着权衡，例如内存
浪费与分配上的挑战。)

At the very end of the walk with allocations, if it didn't return errors,
`__handle_mm_fault()` finally calls `handle_pte_fault()`, which via `do_fault()`
performs one of `do_read_fault()`, `do_cow_fault()`, `do_shared_fault()`.
"read", "cow", "shared" give hints about the reasons and the kind of fault it's
handling.
(在带分配的遍历的最后，如果没有返回错误，`__handle_mm_fault()` 最终会
调用 `handle_pte_fault()`，后者经由 `do_fault()` 执行 `do_read_fault()`、
`do_cow_fault()`、`do_shared_fault()` 三者之一。"read"、"cow"、"shared"
提示了它正在处理的错误的原因与类型。)

The actual implementation of the workflow is very complex. Its design allows
Linux to handle page faults in a way that is tailored to the specific
characteristics of each architecture, while still sharing a common overall
structure.
(该工作流的实际实现非常复杂。其设计使 Linux 能够以针对每种架构特性量身
定制的方式处理页错误，同时仍然共享一个共同的整体结构。)

To conclude this high altitude view of how Linux handles page faults, let's
add that the page faults handler can be disabled and enabled respectively with
`pagefault_disable()` and `pagefault_enable()`.
(作为对 Linux 如何处理页错误这一高空视角的总结，还要补充一点：页错误
处理程序可以分别用 `pagefault_disable()` 与 `pagefault_enable()` 禁用和
启用。)

Several code paths make use of the latter two functions because they need to
disable traps into the page faults handler, mostly to prevent deadlocks.
(一些代码路径会使用后两个函数，因为它们需要禁用陷入页错误处理程序的
陷阱，主要是为了防止死锁。)
