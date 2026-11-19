.. SPDX-License-Identifier: GPL-2.0

========================================
The x86 kvm shadow mmu(x86 kvm 影子 MMU)
========================================

The mmu (in arch/x86/kvm, files mmu.[ch] and paging_tmpl.h) is responsible
for presenting a standard x86 mmu to the guest, while translating guest
physical addresses to host physical addresses.
(mmu 位于 arch/x86/kvm (文件 mmu.[ch] 与 paging_tmpl.h)，负责向客户机呈现一个
标准的 x86 MMU，同时把客户机物理地址翻译成宿主机物理地址。)

The mmu code attempts to satisfy the following requirements:
(mmu 代码试图满足以下要求：)

- correctness:
	       the guest should not be able to determine that it is running
               on an emulated mmu except for timing (we attempt to comply
               with the specification, not emulate the characteristics of
               a particular implementation such as tlb size)

  (正确性：客户机除时序之外不应能察觉自己运行在模拟的 MMU 上 (我们试图遵循
  规范，而不模拟特定实现的特征，例如 tlb 大小)。)
- security:
	       the guest must not be able to touch host memory not assigned
               to it

  (安全性：客户机必须不能触碰未分配给它的宿主机内存。)
- performance:
               minimize the performance penalty imposed by the mmu

  (性能：把 mmu 造成的性能损失降到最低。)
- scaling:
               need to scale to large memory and large vcpu guests

  (可扩展性：需要能扩展到大内存、多 vcpu 的客户机。)
- hardware:
               support the full range of x86 virtualization hardware

  (硬件：支持全系列的 x86 虚拟化硬件。)
- integration:
               Linux memory management code must be in control of guest memory
               so that swapping, page migration, page merging, transparent
               hugepages, and similar features work without change

  (集成：Linux 内存管理代码必须掌控客户机内存，使得交换、页迁移、页合并、
  透明大页等特性无需改动即可工作。)
- dirty tracking:
               report writes to guest memory to enable live migration
               and framebuffer-based displays

  (脏页跟踪：报告对客户机内存的写入，以支持热迁移和基于帧缓冲的显示。)
- footprint:
               keep the amount of pinned kernel memory low (most memory
               should be shrinkable)

  (占用：保持被固定的内核内存量较低 (大部分内存应当是可收缩的)。)
- reliability:
               avoid multipage or GFP_ATOMIC allocations

  (可靠性：避免多页或 GFP_ATOMIC 分配。)

Acronyms(缩略语)
================

====  ====================================================================
pfn   host page frame number(主机页帧号)
hpa   host physical address(宿主机物理地址)
hva   host virtual address(宿主机虚拟地址)
gfn   guest frame number(客户机页帧号)
gpa   guest physical address(客户机物理地址)
gva   guest virtual address(客户机虚拟地址)
ngpa  nested guest physical address(嵌套客户机物理地址)
ngva  nested guest virtual address(嵌套客户机虚拟地址)
pte   page table entry (used also to refer generically to paging structure
      entries)(页表项 (也用于泛指分页结构条目))
gpte  guest pte (referring to gfns)(客户机 pte (指向 gfn))
spte  shadow pte (referring to pfns)(影子 pte (指向 pfn))
tdp   two dimensional paging (vendor neutral term for NPT and EPT)(二维分页 (NPT 与 EPT 的厂商中立术语))
====  ====================================================================

Virtual and real hardware supported(支持的虚拟与真实硬件)
=========================================================

The mmu supports first-generation mmu hardware, which allows an atomic switch
of the current paging mode and cr3 during guest entry, as well as
two-dimensional paging (AMD's NPT and Intel's EPT).  The emulated hardware
it exposes is the traditional 2/3/4 level x86 mmu, with support for global
pages, pae, pse, pse36, cr0.wp, and 1GB pages. Emulated hardware also
able to expose NPT capable hardware on NPT capable hosts.
(mmu 支持第一代 MMU 硬件——它允许在客户机进入时原子地切换当前分页模式和 cr3，
也支持二维分页 (AMD 的 NPT 和 Intel 的 EPT)。它暴露的模拟硬件是传统的 2/3/4 级
x86 MMU，支持全局页、pae、pse、pse36、cr0.wp 和 1GB 页。在支持 NPT 的宿主机
上，模拟硬件还能暴露具备 NPT 能力的硬件。)

Translation(地址翻译)
=====================

The primary job of the mmu is to program the processor's mmu to translate
addresses for the guest.  Different translations are required at different
times:
(mmu 的首要工作是配置处理器的 MMU 来为客户机翻译地址。不同时刻需要不同的
翻译：)

- when guest paging is disabled, we translate guest physical addresses to
  host physical addresses (gpa->hpa)
  (客户机分页禁用时，我们把客户机物理地址翻译为宿主机物理地址 (gpa->hpa)。)
- when guest paging is enabled, we translate guest virtual addresses, to
  guest physical addresses, to host physical addresses (gva->gpa->hpa)
  (客户机分页启用时，我们把客户机虚拟地址翻译为客户机物理地址，再翻译为
  宿主机物理地址 (gva->gpa->hpa)。)
- when the guest launches a guest of its own, we translate nested guest
  virtual addresses, to nested guest physical addresses, to guest physical
  addresses, to host physical addresses (ngva->ngpa->gpa->hpa)
  (当客户机启动自己的客户机时，我们把嵌套客户机虚拟地址翻译为嵌套客户机
  物理地址，再翻译为客户机物理地址，最后翻译为宿主机物理地址
  (ngva->ngpa->gpa->hpa)。)

The primary challenge is to encode between 1 and 3 translations into hardware
that support only 1 (traditional) and 2 (tdp) translations.  When the
number of required translations matches the hardware, the mmu operates in
direct mode; otherwise it operates in shadow mode (see below).
(主要挑战在于把 1 到 3 层翻译编码进只支持 1 层 (传统) 或 2 层 (tdp) 翻译的
硬件中。当所需翻译层数与硬件匹配时，mmu 以直接模式 (direct mode) 运行；
否则以影子模式 (shadow mode) 运行 (见下文)。)

Memory(内存)
============

Guest memory (gpa) is part of the user address space of the process that is
using kvm.  Userspace defines the translation between guest addresses and user
addresses (gpa->hva); note that two gpas may alias to the same hva, but not
vice versa.
(客户机内存 (gpa) 是使用 kvm 的进程用户地址空间的一部分。用户空间定义客户机
地址与用户地址之间的翻译 (gpa->hva)；注意两个 gpa 可能别名到同一个 hva，
但反之不然。)

These hvas may be backed using any method available to the host: anonymous
memory, file backed memory, and device memory.  Memory might be paged by the
host at any time.
(这些 hva 可以由宿主机可用的任何方式支撑：匿名内存、文件支撑内存和设备内存。
宿主机随时可能把这些内存换出分页。)

Events(事件)
============

The mmu is driven by events, some from the guest, some from the host.
(mmu 由事件驱动，其中一些来自客户机，一些来自宿主机。)

Guest generated events:
(客户机产生的事件：)

- writes to control registers (especially cr3)
  (写控制寄存器 (尤其是 cr3)。)
- invlpg/invlpga instruction execution
  (执行 invlpg/invlpga 指令。)
- access to missing or protected translations
  (访问缺失的或受保护的翻译。)

Host generated events:
(宿主机产生的事件：)

- changes in the gpa->hpa translation (either through gpa->hva changes or
  through hva->hpa changes)
  (gpa->hpa 翻译的改变 (通过 gpa->hva 改变或 hva->hpa 改变)。)
- memory pressure (the shrinker)
  (内存压力 (shrinker)。)

Shadow pages(影子页)
====================

The principal data structure is the shadow page, 'struct kvm_mmu_page'.  A
shadow page contains 512 sptes, which can be either leaf or nonleaf sptes.  A
shadow page may contain a mix of leaf and nonleaf sptes.
(主要数据结构是影子页 (shadow page)，即 'struct kvm_mmu_page'。一个影子页包含
512 个 spte，它们可以是叶 spte 或非叶 spte。一个影子页可以同时包含叶 spte 和
非叶 spte 的混合。)

A nonleaf spte allows the hardware mmu to reach the leaf pages and
is not related to a translation directly.  It points to other shadow pages.
(非叶 spte 使硬件 MMU 能够到达叶页，它不直接对应某个翻译。它指向其他影子页。)

A leaf spte corresponds to either one or two translations encoded into
one paging structure entry.  These are always the lowest level of the
translation stack, with optional higher level translations left to NPT/EPT.
Leaf ptes point at guest pages.
(叶 spte 对应编码进一个分页结构表项中的一到两层翻译。它们始终是翻译栈的最低
层，更高层的翻译 (可选) 留给 NPT/EPT。叶 pte 指向客户机页。)

The following table shows translations encoded by leaf ptes, with higher-level
translations in parentheses:
(下表展示由叶 pte 编码的翻译，括号中是更高层的翻译：)

 Non-nested guests::

  nonpaging:     gpa->hpa
  paging:        gva->gpa->hpa
  paging, tdp:   (gva->)gpa->hpa

 Nested guests::

  non-tdp:       ngva->gpa->hpa  (*)
  tdp:           (ngva->)ngpa->gpa->hpa

  (*) the guest hypervisor will encode the ngva->gpa translation into its page
      tables if npt is not present

(非嵌套客户机如上表所示；嵌套客户机亦然。若不存在 NPT，客户机虚拟机管理器
会把 ngva->gpa 翻译编码进自己的页表。)

Shadow pages contain the following information:

(影子页包含以下信息：)

  role.level:
    The level in the shadow paging hierarchy that this shadow page belongs to.
    1=4k sptes, 2=2M sptes, 3=1G sptes, etc.
    (该影子页在影子分页层级中所处的级别。1=4k spte，2=2M spte，3=1G spte，
    依此类推。)
  role.direct:
    If set, leaf sptes reachable from this page are for a linear range.
    Examples include real mode translation, large guest pages backed by small
    host pages, and gpa->hpa translations when NPT or EPT is active.
    The linear range starts at (gfn << PAGE_SHIFT) and its size is determined
    by role.level (2MB for first level, 1GB for second level, 0.5TB for third
    level, 256TB for fourth level)
    If clear, this page corresponds to a guest page table denoted by the gfn
    field.
    (若置位，则从本页可达的叶 spte 对应于一段线性范围。例子包括实模式翻译、
    由小宿主机页支撑的大客户机页，以及 NPT/EPT 激活时的 gpa->hpa 翻译。线性
    范围从 (gfn << PAGE_SHIFT) 开始，其大小由 role.level 决定 (第一级为 2MB，
    第二级 1GB，第三级 0.5TB，第四级 256TB)。若清除，则本页对应由 gfn 字段
    指示的一张客户机页表。)
  role.quadrant:
    When role.has_4_byte_gpte=1, the guest uses 32-bit gptes while the host uses 64-bit
    sptes.  That means a guest page table contains more ptes than the host,
    so multiple shadow pages are needed to shadow one guest page.
    For first-level shadow pages, role.quadrant can be 0 or 1 and denotes the
    first or second 512-gpte block in the guest page table.  For second-level
    page tables, each 32-bit gpte is converted to two 64-bit sptes
    (since each first-level guest page is shadowed by two first-level
    shadow pages) so role.quadrant takes values in the range 0..3.  Each
    quadrant maps 1GB virtual address space.
    (当 role.has_4_byte_gpte=1 时，客户机使用 32 位 gpte，而宿主机使用 64 位
    spte。这意味着一张客户机页表包含的 pte 比宿主机多，因此需要多张影子页来
    影子化一张客户机页。对第一级影子页，role.quadrant 可为 0 或 1，表示客户机
    页表中的第一个或第二个 512-gpte 块。对第二级页表，每个 32 位 gpte 被转换
    为两个 64 位 spte (因为每个第一级客户机页由两张第一级影子页影子化)，所以
    role.quadrant 取值范围为 0..3。每个象限映射 1GB 虚拟地址空间。)
  role.access:
    Inherited guest access permissions from the parent ptes in the form uwx.
    Note execute permission is positive, not negative.
    (从父 pte 继承的客户机访问权限，形式为 uwx。注意执行权限是正向表示的，
    而非反向。)
  role.invalid:
    The page is invalid and should not be used.  It is a root page that is
    currently pinned (by a cpu hardware register pointing to it); once it is
    unpinned it will be destroyed.
    (该页无效，不应使用。它是一个当前被固定 (pin) 的根页 (由指向它的 cpu
    硬件寄存器固定)；一旦解除固定，它将被销毁。)
  role.has_4_byte_gpte:
    Reflects the size of the guest PTE for which the page is valid, i.e. '0'
    if direct map or 64-bit gptes are in use, '1' if 32-bit gptes are in use.
    (反映该页对其有效的客户机 PTE 大小：直接映射或使用 64 位 gpte 时为 '0'，
    使用 32 位 gpte 时为 '1'。)
  role.efer_nx:
    Contains the value of efer.nx for which the page is valid.
    (包含该页对其有效的 efer.nx 值。)
  role.cr0_wp:
    Contains the value of cr0.wp for which the page is valid.
    (包含该页对其有效的 cr0.wp 值。)
  role.smep_andnot_wp:
    Contains the value of cr4.smep && !cr0.wp for which the page is valid
    (pages for which this is true are different from other pages; see the
    treatment of cr0.wp=0 below).
    (包含该页对其有效的 cr4.smep && !cr0.wp 值 (满足此条件的页与其他页不同；
    见下文对 cr0.wp=0 的处理)。)
  role.smap_andnot_wp:
    Contains the value of cr4.smap && !cr0.wp for which the page is valid
    (pages for which this is true are different from other pages; see the
    treatment of cr0.wp=0 below).
    (包含该页对其有效的 cr4.smap && !cr0.wp 值 (满足此条件的页与其他页不同；
    见下文对 cr0.wp=0 的处理)。)
  role.smm:
    Is 1 if the page is valid in system management mode.  This field
    determines which of the kvm_memslots array was used to build this
    shadow page; it is also used to go back from a struct kvm_mmu_page
    to a memslot, through the kvm_memslots_for_spte_role macro and
    __gfn_to_memslot.
    (若该页在系统管理模式 (SMM) 下有效则为 1。该字段决定构建此影子页时使用
    的是 kvm_memslots 数组中的哪一个；也用于通过 kvm_memslots_for_spte_role
    宏和 __gfn_to_memslot 从 struct kvm_mmu_page 回溯到 memslot。)
  role.ad_disabled:
    Is 1 if the MMU instance cannot use A/D bits.  EPT did not have A/D
    bits before Haswell; shadow EPT page tables also cannot use A/D bits
    if the L1 hypervisor does not enable them.
    (若该 MMU 实例不能使用 A/D 位则为 1。Haswell 之前 EPT 没有 A/D 位；若 L1
    虚拟机管理器未启用它们，影子 EPT 页表也不能使用 A/D 位。)
  role.guest_mode:
    Indicates the shadow page is created for a nested guest.
    (指示该影子页是为嵌套客户机创建的。)
  role.passthrough:
    The page is not backed by a guest page table, but its first entry
    points to one.  This is set if NPT uses 5-level page tables (host
    CR4.LA57=1) and is shadowing L1's 4-level NPT (L1 CR4.LA57=0).
    (该页不由客户机页表支撑，但其第一个表项指向一张客户机页表。当 NPT 使用
    5 级页表 (宿主机 CR4.LA57=1) 且正在影子化 L1 的 4 级 NPT (L1 CR4.LA57=0)
    时会设置此标志。)
  mmu_valid_gen:
    The MMU generation of this page, used to fast zap of all MMU pages within a
    VM without blocking vCPUs too long. Specifically, KVM updates the per-VM
    valid MMU generation which causes the mismatch of mmu_valid_gen for each mmu
    page. This makes all existing MMU pages obsolete. Obsolete pages can't be
    used. Therefore, vCPUs must load a new, valid root before re-entering the
    guest. The MMU generation is only ever '0' or '1'. Note, the TDP MMU doesn't
    use this field as non-root TDP MMU pages are reachable only from their
    owning root. Thus it suffices for TDP MMU to use role.invalid in root pages
    to invalidate all MMU pages.
    (本页的 MMU 代数，用于在不长时间阻塞 vCPU 的情况下快速清除 VM 内的所有
    MMU 页。具体而言，KVM 更新每 VM 的有效 MMU 代数，使每个 MMU 页的
    mmu_valid_gen 不匹配，从而使所有现存 MMU 页过时。过时页不能被使用。因此，
    vCPU 必须在重新进入客户机之前加载新的有效根页。MMU 代数只会是 '0' 或
    '1'。注意，TDP MMU 不使用该字段，因为非根 TDP MMU 页只能从其所属根页
    到达。因此对 TDP MMU 而言，用根页中的 role.invalid 来使所有 MMU 页失效
    就足够了。)
  gfn:
    Either the guest page table containing the translations shadowed by this
    page, or the base page frame for linear translations.  See role.direct.
    (要么是被本页影子化的翻译所在的客户机页表，要么是线性翻译的基页帧。
    参见 role.direct。)
  spt:
    A pageful of 64-bit sptes containing the translations for this page.
    Accessed by both kvm and hardware.
    The page pointed to by spt will have its page->private pointing back
    at the shadow page structure.
    sptes in spt point either at guest pages, or at lower-level shadow pages.
    Specifically, if sp1 and sp2 are shadow pages, then sp1->spt[n] may point
    at __pa(sp2->spt).  sp2 will point back at sp1 through parent_pte.
    The spt array forms a DAG structure with the shadow page as a node, and
    guest pages as leaves.
    (一整页 64 位 spte，包含本页的翻译。由 kvm 和硬件共同访问。spt 所指向的
    页，其 page->private 会指回影子页结构。spt 中的 spte 要么指向客户机页，
    要么指向更低层的影子页。具体来说，若 sp1 和 sp2 是影子页，则 sp1->spt[n]
    可能指向 __pa(sp2->spt)。sp2 通过 parent_pte 指回 sp1。spt 数组构成一个
    以影子页为节点、客户机页为叶子的 DAG 结构。)
  shadowed_translation:
    An array of 512 shadow translation entries, one for each present pte. Used
    to perform a reverse map from a pte to a gfn as well as its access
    permission. When role.direct is set, the shadow_translation array is not
    allocated. This is because the gfn contained in any element of this array
    can be calculated from the gfn field when used.  In addition, when
    role.direct is set, KVM does not track access permission for each of the
    gfn. See role.direct and gfn.
    (由 512 个影子翻译条目组成的数组，每个存在的 pte 一个。用于执行从 pte 到
    gfn 及其访问权限的反向映射。当 role.direct 置位时，不分配
    shadow_translation 数组，因为该数组任一元素包含的 gfn 可以在使用时从 gfn
    字段计算得出。此外，当 role.direct 置位时，KVM 不跟踪每个 gfn 的访问
    权限。参见 role.direct 与 gfn。)
  root_count / tdp_mmu_root_count:
     root_count is a reference counter for root shadow pages in Shadow MMU.
     vCPUs elevate the refcount when getting a shadow page that will be used as
     a root page, i.e. page that will be loaded into hardware directly (CR3,
     PDPTRs, nCR3 EPTP). Root pages cannot be destroyed while their refcount is
     non-zero. See role.invalid. tdp_mmu_root_count is similar but exclusively
     used in TDP MMU as an atomic refcount.
     (root_count 是影子 MMU 中根影子页的引用计数器。vCPU 在获取将用作根页的
     影子页 (即将被直接加载到硬件的页，CR3、PDPTR、nCR3 EPTP) 时提升引用
     计数。根页在引用计数非零期间不能被销毁。参见 role.invalid。
     tdp_mmu_root_count 与之类似，但专用于 TDP MMU，作为原子引用计数。)
  parent_ptes:
    The reverse mapping for the pte/ptes pointing at this page's spt. If
    parent_ptes bit 0 is zero, only one spte points at this page and
    parent_ptes points at this single spte, otherwise, there exists multiple
    sptes pointing at this page and (parent_ptes & ~0x1) points at a data
    structure with a list of parent sptes.
    (指向本页 spt 的 pte 的反向映射。若 parent_ptes 的位 0 为零，则只有一个
    spte 指向本页，parent_ptes 指向这个单独的 spte；否则存在多个指向本页的
    spte，(parent_ptes & ~0x1) 指向一个带有父 spte 列表的数据结构。)
  ptep:
    The kernel virtual address of the SPTE that points at this shadow page.
    Used exclusively by the TDP MMU, this field is a union with parent_ptes.
    (指向本影子页的 SPTE 的内核虚拟地址。仅供 TDP MMU 使用；该字段与
    parent_ptes 是一个联合。)
  unsync:
    If true, then the translations in this page may not match the guest's
    translation.  This is equivalent to the state of the tlb when a pte is
    changed but before the tlb entry is flushed.  Accordingly, unsync ptes
    are synchronized when the guest executes invlpg or flushes its tlb by
    other means.  Valid for leaf pages.
    (若为真，则本页中的翻译可能与客户机的翻译不一致。这等价于 pte 被修改但
    tlb 表项尚未刷新时的 tlb 状态。相应地，当客户机执行 invlpg或以其他方式
    刷新其 tlb 时，未同步的 pte 会被同步。仅对叶页有效。)
  unsync_children:
    How many sptes in the page point at pages that are unsync (or have
    unsynchronized children).
    (本页中有多少个 spte 指向未同步的页 (或拥有未同步子节点的页)。)
  unsync_child_bitmap:
    A bitmap indicating which sptes in spt point (directly or indirectly) at
    pages that may be unsynchronized.  Used to quickly locate all unsynchronized
    pages reachable from a given page.
    (一个位图，指示 spt 中哪些 spte (直接或间接) 指向可能未同步的页。用于快速
    定位从给定页可达的所有未同步页。)
  clear_spte_count:
    Only present on 32-bit hosts, where a 64-bit spte cannot be written
    atomically.  The reader uses this while running out of the MMU lock
    to detect in-progress updates and retry them until the writer has
    finished the write.
    (仅存在于 32 位宿主机上，那里 64 位 spte 无法原子地写入。读者在 MMU 锁
    之外运行时用它检测进行中的更新并重试，直到写者完成写入。)
  write_flooding_count:
    A guest may write to a page table many times, causing a lot of
    emulations if the page needs to be write-protected (see "Synchronized
    and unsynchronized pages" below).  Leaf pages can be unsynchronized
    so that they do not trigger frequent emulation, but this is not
    possible for non-leafs.  This field counts the number of emulations
    since the last time the page table was actually used; if emulation
    is triggered too frequently on this page, KVM will unmap the page
    to avoid emulation in the future.
    (客户机可能多次写一张页表，若该页需要写保护就会引发大量模拟 (见下文
    "同步与未同步页")。叶页可以处于未同步状态以免频繁触发模拟，但非叶页
    做不到。该字段统计自上次页表实际被使用以来的模拟次数；如果模拟在该页上
    触发得过于频繁，KVM 将解除该页的映射，以避免今后的模拟。)
  tdp_mmu_page:
    Is 1 if the shadow page is a TDP MMU page. This variable is used to
    bifurcate the control flows for KVM when walking any data structure that
    may contain pages from both TDP MMU and shadow MMU.
    (若该影子页是 TDP MMU 页则为 1。当遍历可能同时包含 TDP MMU 页与影子 MMU
    页的数据结构时，KVM 用该变量分流控制流程。)

Reverse map(反向映射)
=====================

The mmu maintains a reverse mapping whereby all ptes mapping a page can be
reached given its gfn.  This is used, for example, when swapping out a page.
(mmu 维护一个反向映射，给定 gfn 即可到达映射某页的所有 pte。例如换出一页时
会用到它。)

Synchronized and unsynchronized pages(同步与未同步页)
=====================================================

The guest uses two events to synchronize its tlb and page tables: tlb flushes
and page invalidations (invlpg).
(客户机用两种事件来同步其 tlb 和页表：tlb 刷新和页失效 (invlpg)。)

A tlb flush means that we need to synchronize all sptes reachable from the
guest's cr3.  This is expensive, so we keep all guest page tables write
protected, and synchronize sptes to gptes when a gpte is written.
(一次 tlb 刷新意味着我们需要同步从客户机 cr3 可达的所有 spte。这开销很大，
所以我们把所有客户机页表保持为写保护，并在 gpte 被写入时把 spte 同步到
gpte。)

A special case is when a guest page table is reachable from the current
guest cr3.  In this case, the guest is obliged to issue an invlpg instruction
before using the translation.  We take advantage of that by removing write
protection from the guest page, and allowing the guest to modify it freely.
We synchronize modified gptes when the guest invokes invlpg.  This reduces
the amount of emulation we have to do when the guest modifies multiple gptes,
or when the a guest page is no longer used as a page table and is used for
random guest data.
(特殊情况是客户机页表可从当前客户机 cr3 到达。此时，客户机在使用该翻译之前
必须先执行 invlpg 指令。我们利用这一点：移除客户机页的写保护，允许客户机
自由修改它。当客户机调用 invlpg 时，我们同步被修改的 gpte。这减少了客户机
修改多个 gpte 时，或客户机页不再用作页表而改存随机客户机数据时，我们所要做
的模拟工作量。)

As a side effect we have to resynchronize all reachable unsynchronized shadow
pages on a tlb flush.
(副作用是，在 tlb 刷新时我们必须重新同步所有可达的未同步影子页。)


Reaction to events(对事件的反应)
================================

- guest page fault (or npt page fault, or ept violation)
  (客户机缺页 (或 npt 缺页，或 ept 违例)。)

This is the most complicated event.  The cause of a page fault can be:
(这是最复杂的事件。缺页的成因可以是：)

  - a true guest fault (the guest translation won't allow the access) (*)
    (真正的客户机缺页 (客户机翻译不允许该访问) (*)。)
  - access to a missing translation
    (访问缺失的翻译。)
  - access to a protected translation
    (访问受保护的翻译。)
    - when logging dirty pages, memory is write protected

      (记录脏页时，内存是写保护的。)

    - synchronized shadow pages are write protected (*)

      (已同步的影子页是写保护的 (*)。)
  - access to untranslatable memory (mmio)
    (访问不可翻译的内存 (mmio)。)

  (*) not applicable in direct mode
  ( (*) 不适用于直接模式。)

Handling a page fault is performed as follows:
(缺页的处理流程如下：)

 - if the RSV bit of the error code is set, the page fault is caused by guest
   accessing MMIO and cached MMIO information is available.
   (若错误码的 RSV 位置位，则该缺页由客户机访问 MMIO 引起，且缓存了 MMIO
   信息。)

   - walk shadow page table
     (遍历影子页表。)
   - check for valid generation number in the spte (see "Fast invalidation of
     MMIO sptes" below)
     (检查 spte 中的有效代数 (见下文 "MMIO spte 的快速失效")。)
   - cache the information to vcpu->arch.mmio_gva, vcpu->arch.mmio_access and
     vcpu->arch.mmio_gfn, and call the emulator
     (把信息缓存到 vcpu->arch.mmio_gva、vcpu->arch.mmio_access 和
     vcpu->arch.mmio_gfn，并调用模拟器。)

 - If both P bit and R/W bit of error code are set, this could possibly
   be handled as a "fast page fault" (fixed without taking the MMU lock).  See
   the description in Documentation/virt/kvm/locking.rst.
   (若错误码的 P 位与 R/W 位都置位，则可能可以作为 "快速缺页" 处理 (无需获取
   MMU 锁即可修复)。参见 Documentation/virt/kvm/locking.rst 中的描述。)

 - if needed, walk the guest page tables to determine the guest translation
   (gva->gpa or ngpa->gpa)
   (如有必要，遍历客户机页表以确定客户机翻译 (gva->gpa 或 ngpa->gpa)。)

   - if permissions are insufficient, reflect the fault back to the guest
     (若权限不足，把缺页反射回客户机。)

 - determine the host page
   (确定宿主机页。)

   - if this is an mmio request, there is no host page; cache the info to
     vcpu->arch.mmio_gva, vcpu->arch.mmio_access and vcpu->arch.mmio_gfn
     (若这是 mmio 请求，则没有宿主机页；把信息缓存到 vcpu->arch.mmio_gva、
     vcpu->arch.mmio_access 和 vcpu->arch.mmio_gfn。)

 - walk the shadow page table to find the spte for the translation,
   instantiating missing intermediate page tables as necessary
   (遍历影子页表找到该翻译对应的 spte，必要时实例化缺失的中间页表。)

   - If this is an mmio request, cache the mmio info to the spte and set some
     reserved bit on the spte (see callers of kvm_mmu_set_mmio_spte_mask)
     (若这是 mmio 请求，把 mmio 信息缓存到 spte，并在 spte 上设置某个保留位
     (参见 kvm_mmu_set_mmio_spte_mask 的调用者)。)

 - try to unsynchronize the page
   (尝试取消该页的同步状态。)

   - if successful, we can let the guest continue and modify the gpte
     (若成功，我们可以让客户机继续运行并修改 gpte。)

 - emulate the instruction
   (模拟该指令。)

   - if failed, unshadow the page and let the guest continue
     (若失败，取消该页的影子化并让客户机继续运行。)

 - update any translations that were modified by the instruction
   (更新被该指令修改的所有翻译。)

invlpg handling:
(invlpg 的处理：)

  - walk the shadow page hierarchy and drop affected translations
    (遍历影子页层级并丢弃受影响的翻译。)
  - try to reinstantiate the indicated translation in the hope that the
    guest will use it in the near future
    (尝试重新实例化所指示的翻译，希望客户机在不久之后会用到它。)

Guest control register updates:
(客户机控制寄存器更新：)

- mov to cr3
  (mov 到 cr3。)

  - look up new shadow roots
    (查找新的影子根。)
  - synchronize newly reachable shadow pages
    (同步新可达的影子页。)

- mov to cr0/cr4/efer
  (mov 到 cr0/cr4/efer。)

  - set up mmu context for new paging mode
    (为新分页模式建立 mmu 上下文。)
  - look up new shadow roots
    (查找新的影子根。)
  - synchronize newly reachable shadow pages
    (同步新可达的影子页。)

Host translation updates:
(宿主机翻译更新：)

  - mmu notifier called with updated hva
    (mmu notifier 携带更新后的 hva 被调用。)
  - look up affected sptes through reverse map
    (通过反向映射查找受影响的 spte。)
  - drop (or update) translations
    (丢弃 (或更新) 翻译。)

Emulating cr0.wp(模拟 cr0.wp)
=============================

If tdp is not enabled, the host must keep cr0.wp=1 so page write protection
works for the guest kernel, not guest userspace.  When the guest
cr0.wp=1, this does not present a problem.  However when the guest cr0.wp=0,
we cannot map the permissions for gpte.u=1, gpte.w=0 to any spte (the
semantics require allowing any guest kernel access plus user read access).
(若未启用 tdp，宿主机必须保持 cr0.wp=1，使页写保护对客户机内核而非客户机
用户空间生效。当客户机 cr0.wp=1 时，这不成问题。然而当客户机 cr0.wp=0 时，
我们无法把 gpte.u=1、gpte.w=0 的权限映射到任何 spte (其语义要求允许任何
客户机内核访问外加用户读访问)。)

We handle this by mapping the permissions to two possible sptes, depending
on fault type:
(我们按缺页类型把权限映射到两种可能的 spte 来处理：)

- kernel write fault: spte.u=0, spte.w=1 (allows full kernel access,
  disallows user access)
  (内核写缺页：spte.u=0, spte.w=1 (允许完整的内核访问，禁止用户访问)。)
- read fault: spte.u=1, spte.w=0 (allows full read access, disallows kernel
  write access)
  (读缺页：spte.u=1, spte.w=0 (允许完整的读访问，禁止内核写访问)。)

(user write faults generate a #PF)
(用户写缺页会产生一个 #PF。)

In the first case there are two additional complications:
(第一种情况还有两个额外的复杂因素：)

- if CR4.SMEP is enabled: since we've turned the page into a kernel page,
  the kernel may now execute it.  We handle this by also setting spte.nx.
  If we get a user fetch or read fault, we'll change spte.u=1 and
  spte.nx=gpte.nx back.  For this to work, KVM forces EFER.NX to 1 when
  shadow paging is in use.
  (若 CR4.SMEP 启用：由于我们已把该页变成内核页，内核现在可能执行它。我们
  通过同时设置 spte.nx 来处理。若遇到用户取指或读缺页，我们会把 spte.u=1 和
  spte.nx=gpte.nx 改回去。为此，当使用影子分页时 KVM 强制 EFER.NX 为 1。)
- if CR4.SMAP is disabled: since the page has been changed to a kernel
  page, it can not be reused when CR4.SMAP is enabled. We set
  CR4.SMAP && !CR0.WP into shadow page's role to avoid this case. Note,
  here we do not care the case that CR4.SMAP is enabled since KVM will
  directly inject #PF to guest due to failed permission check.
  (若 CR4.SMAP 禁用：由于该页已被改为内核页，在 CR4.SMAP 启用时它不能被
  重用。我们把 CR4.SMAP && !CR0.WP 放进影子页的 role 中以避免这种情况。
  注意，这里我们不关心 CR4.SMAP 启用的情况，因为权限检查失败时 KVM 会直接
  向客户机注入 #PF。)

To prevent an spte that was converted into a kernel page with cr0.wp=0
from being written by the kernel after cr0.wp has changed to 1, we make
the value of cr0.wp part of the page role.  This means that an spte created
with one value of cr0.wp cannot be used when cr0.wp has a different value -
it will simply be missed by the shadow page lookup code.  A similar issue
exists when an spte created with cr0.wp=0 and cr4.smep=0 is used after
changing cr4.smep to 1.  To avoid this, the value of !cr0.wp && cr4.smep
is also made a part of the page role.
(为了防止在 cr0.wp=0 时被转换成内核页的 spte 在 cr0.wp 变为 1 之后被内核
写入，我们把 cr0.wp 的值纳入页的 role。这意味着以某个 cr0.wp 值创建的 spte
不能在 cr0.wp 值不同时使用——影子页查找代码会直接漏掉它。类似的问题也存在于
以 cr0.wp=0、cr4.smep=0 创建的 spte 在 cr4.smep 改为 1 之后被使用的情况。
为避免这一点，!cr0.wp && cr4.smep 的值也被纳入页的 role。)

Large pages(大页)
=================

The mmu supports all combinations of large and small guest and host pages.
Supported page sizes include 4k, 2M, 4M, and 1G.  4M pages are treated as
two separate 2M pages, on both guest and host, since the mmu always uses PAE
paging.
(mmu 支持客户机与宿主机大页、小页的所有组合。支持的页大小包括 4k、2M、4M
和 1G。由于 mmu 总是使用 PAE 分页，4M 页在客户机和宿主机上都被当作两个独立
的 2M 页。)

To instantiate a large spte, four constraints must be satisfied:
(要实例化一个大 spte，必须满足四个约束：)

- the spte must point to a large host page
  (该 spte 必须指向一个大宿主机页。)
- the guest pte must be a large pte of at least equivalent size (if tdp is
  enabled, there is no guest pte and this condition is satisfied)
  (客户机 pte 必须是至少同等大小的大 pte (若启用了 tdp，则没有客户机 pte，
  该条件自动满足)。)
- if the spte will be writeable, the large page frame may not overlap any
  write-protected pages
  (若该 spte 将是可写的，大页帧不得与任何写保护页重叠。)
- the guest page must be wholly contained by a single memory slot
  (客户机页必须完全包含在单个内存槽内。)

To check the last two conditions, the mmu maintains a ->disallow_lpage set of
arrays for each memory slot and large page size.  Every write protected page
causes its disallow_lpage to be incremented, thus preventing instantiation of
a large spte.  The frames at the end of an unaligned memory slot have
artificially inflated ->disallow_lpages so they can never be instantiated.
(为检查最后两个条件，mmu 为每个内存槽和大页大小维护一组 ->disallow_lpage
数组。每个写保护页都会使其 disallow_lpage 递增，从而阻止大 spte 的实例化。
未对齐内存槽末端的帧被人为抬高了 ->disallow_lpage，因此永远不会被实例化。)

Fast invalidation of MMIO sptes(MMIO spte 的快速失效)
=====================================================

As mentioned in "Reaction to events" above, kvm will cache MMIO
information in leaf sptes.  When a new memslot is added or an existing
memslot is changed, this information may become stale and needs to be
invalidated.  This also needs to hold the MMU lock while walking all
shadow pages, and is made more scalable with a similar technique.
(如上文 "对事件的反应" 所述，kvm 会把 MMIO 信息缓存在叶 spte 中。当添加新
memslot 或更改现有 memslot 时，这些信息可能过时，需要失效。这一操作还需要
在遍历所有影子页时持有 MMU 锁，并用类似的技术提高了可扩展性。)

MMIO sptes have a few spare bits, which are used to store a
generation number.  The global generation number is stored in
kvm_memslots(kvm)->generation, and increased whenever guest memory info
changes.
( MMIO spte 有几个空闲位，用于存储代数。全局代数存储在
kvm_memslots(kvm)->generation 中，每当客户机内存信息变化时就递增。)

When KVM finds an MMIO spte, it checks the generation number of the spte.
If the generation number of the spte does not equal the global generation
number, it will ignore the cached MMIO information and handle the page
fault through the slow path.
(当 KVM 找到一个 MMIO spte 时，它会检查该 spte 的代数。如果 spte 的代数不
等于全局代数，它将忽略缓存的 MMIO 信息，并通过慢路径处理该缺页。)

Since only 18 bits are used to store generation-number on mmio spte, all
pages are zapped when there is an overflow.
(由于 mmio spte 上只有 18 位用于存储代数，一旦溢出，所有页都会被清除。)

Unfortunately, a single memory access might access kvm_memslots(kvm) multiple
times, the last one happening when the generation number is retrieved and
stored into the MMIO spte.  Thus, the MMIO spte might be created based on
out-of-date information, but with an up-to-date generation number.
(遗憾的是，一次内存访问可能多次访问 kvm_memslots(kvm)，最后一次发生在取出
代数并存入 MMIO spte 时。因此，MMIO spte 可能是基于过时信息、但带有最新
代数创建的。)

To avoid this, the generation number is incremented again after synchronize_srcu
returns; thus, bit 63 of kvm_memslots(kvm)->generation set to 1 only during a
memslot update, while some SRCU readers might be using the old copy.  We do not
want to use an MMIO sptes created with an odd generation number, and we can do
this without losing a bit in the MMIO spte.  The "update in-progress" bit of the
generation is not stored in MMIO spte, and is so is implicitly zero when the
generation is extracted out of the spte.  If KVM is unlucky and creates an MMIO
spte while an update is in-progress, the next access to the spte will always be
a cache miss.  For example, a subsequent access during the update window will
miss due to the in-progress flag diverging, while an access after the update
window closes will have a higher generation number (as compared to the spte).
(为避免这一点，synchronize_srcu 返回后代数会再次递增；因此，
kvm_memslots(kvm)->generation 的位 63 仅在 memslot 更新期间被置 1，而此时
某些 SRCU 读者可能还在使用旧副本。我们不希望使用以奇数代数创建的 MMIO spte，
并且可以在不损失 MMIO spte 中一个位的前提下做到这一点。代数的 "更新进行中"
位不存储在 MMIO spte 中，因此从 spte 中取出代数时它隐含为零。如果 KVM 运气
不好，在更新进行中创建了 MMIO spte，那么对该 spte 的下一次访问将总是缓存
未命中。例如，更新窗口内的后续访问会因 in-progress 标志不一致而未命中，而
更新窗口结束后的访问则会有更高的代数 (与该 spte 相比)。)


Further reading(延伸阅读)
=========================

- NPT presentation from KVM Forum 2008
  https://www.linux-kvm.org/images/c/c8/KvmForum2008%24kdf2008_21.pdf
  (2008 年 KVM Forum 的 NPT 演讲。)
