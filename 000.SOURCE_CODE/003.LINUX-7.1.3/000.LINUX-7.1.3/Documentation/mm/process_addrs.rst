.. SPDX-License-Identifier: GPL-2.0

===========================
Process Addresses(进程地址)
===========================

.. toctree::
   :maxdepth: 3


Userland memory ranges are tracked by the kernel via Virtual Memory Areas or
'VMA's of type :c:struct:`!struct vm_area_struct`.
(用户态内存范围由内核通过虚拟内存区域 (Virtual Memory Area, VMA)——即 :c:struct:`!struct vm_area_struct` 类型的对象——来跟踪。)

Each VMA describes a virtually contiguous memory range with identical
attributes, each described by a :c:struct:`!struct vm_area_struct`
object. Userland access outside of VMAs is invalid except in the case where an
adjacent stack VMA could be extended to contain the accessed address.
(每个 VMA 描述一段属性相同的、虚拟地址连续的内存范围，每个范围由一个 :c:struct:`!struct vm_area_struct` 对象描述。VMA 之外的用户态访问是无效的，除非相邻的栈 VMA 可以扩展到包含被访问的地址。)

All VMAs are contained within one and only one virtual address space, described
by a :c:struct:`!struct mm_struct` object which is referenced by all tasks (that is,
threads) which share the virtual address space. We refer to this as the
:c:struct:`!mm`.
(所有 VMA 都包含在一个且仅一个虚拟地址空间中；该地址空间由一个 :c:struct:`!struct mm_struct` 对象描述，共享该虚拟地址空间的所有任务(即线程)都引用它。我们将其称为 :c:struct:`!mm`。)

Each mm object contains a maple tree data structure which describes all VMAs
within the virtual address space.
(每个 mm 对象都包含一个 maple tree 数据结构，用于描述该虚拟地址空间中的所有 VMA。)

.. note:: An exception to this is the 'gate' VMA which is provided by
          architectures which use :c:struct:`!vsyscall` and is a global static
          object which does not belong to any specific mm.
          (注意：一个例外是由使用 :c:struct:`!vsyscall` 的架构提供的 'gate' VMA——它是一个全局静态对象，不属于任何特定的 mm。)

-------------
Locking(加锁)
-------------

The kernel is designed to be highly scalable against concurrent read operations
on VMA **metadata** so a complicated set of locks are required to ensure memory
corruption does not occur.
(内核设计为对 VMA **元数据** 的并发读操作具有高度可扩展性，因此需要一套复杂的锁来确保不会发生内存损坏。)

.. note:: Locking VMAs for their metadata does not have any impact on the memory
          they describe nor the page tables that map them.
          (注意：为 VMA 的元数据加锁，对其描述的内存本身或映射它们的页表没有任何影响。)

Terminology(术语)
-----------------

* **mmap locks** - Each MM has a read/write semaphore :c:member:`!mmap_lock`
  which locks at a process address space granularity which can be acquired via
  :c:func:`!mmap_read_lock`, :c:func:`!mmap_write_lock` and variants.
  (**mmap 锁** - 每个 MM 都有一个读/写信号量 :c:member:`!mmap_lock`，它以进程地址空间为粒度加锁，可通过 :c:func:`!mmap_read_lock`、:c:func:`!mmap_write_lock` 及其变体获取。)
* **VMA locks** - The VMA lock is at VMA granularity (of course) which behaves
  as a read/write semaphore in practice. A VMA read lock is obtained via
  :c:func:`!lock_vma_under_rcu` (and unlocked via :c:func:`!vma_end_read`) and a
  write lock via vma_start_write() or vma_start_write_killable()
  (all VMA write locks are unlocked
  automatically when the mmap write lock is released). To take a VMA write lock
  you **must** have already acquired an :c:func:`!mmap_write_lock`.
  (**VMA 锁** - VMA 锁当然是以 VMA 为粒度的，实际表现如同读/写信号量。VMA 读锁通过 :c:func:`!lock_vma_under_rcu` 获取(并通过 :c:func:`!vma_end_read` 解锁)，写锁通过 vma_start_write() 或 vma_start_write_killable() 获取(mmap 写锁释放时，所有 VMA 写锁都会自动解锁)。要获取 VMA 写锁，你 **必须** 已经持有 :c:func:`!mmap_write_lock`。)
* **rmap locks** - When trying to access VMAs through the reverse mapping via a
  :c:struct:`!struct address_space` or :c:struct:`!struct anon_vma` object
  (reachable from a folio via :c:member:`!folio->mapping`). VMAs must be stabilised via
  :c:func:`!anon_vma_[try]lock_read` or :c:func:`!anon_vma_[try]lock_write` for
  anonymous memory and :c:func:`!i_mmap_[try]lock_read` or
  :c:func:`!i_mmap_[try]lock_write` for file-backed memory. We refer to these
  locks as the reverse mapping locks, or 'rmap locks' for brevity.
  (**rmap 锁** - 当试图通过反向映射(经由 :c:struct:`!struct address_space` 或 :c:struct:`!struct anon_vma` 对象——可从 folio 的 :c:member:`!folio->mapping` 到达)访问 VMA 时使用。匿名内存必须通过 :c:func:`!anon_vma_[try]lock_read` 或 :c:func:`!anon_vma_[try]lock_write` 使 VMA 保持稳定，文件映射内存则使用 :c:func:`!i_mmap_[try]lock_read` 或 :c:func:`!i_mmap_[try]lock_write`。我们把这些锁称为反向映射锁，简称 'rmap 锁'。)

We discuss page table locks separately in the dedicated section below.
(页表锁将在下面的专门章节中单独讨论。)

The first thing **any** of these locks achieve is to **stabilise** the VMA
within the MM tree. That is, guaranteeing that the VMA object will not be
deleted from under you nor modified (except for some specific fields
described below).
(这些锁中 **任何** 一个锁首先做到的，都是让 VMA 在 MM 树中 **保持稳定** (stabilise)。也就是说，保证 VMA 对象不会在你脚下被删除，也不会被修改(下文描述的某些特定字段除外)。)

Stabilising a VMA also keeps the address space described by it around.
(稳定一个 VMA 同时也保证其描述的地址空间继续存在。)

Lock usage(锁的使用)
--------------------

If you want to **read** VMA metadata fields or just keep the VMA stable, you
must do one of the following:
(如果想 **读取** VMA 元数据字段，或只是让 VMA 保持稳定，必须做到以下之一：)

* Obtain an mmap read lock at the MM granularity via :c:func:`!mmap_read_lock` (or a
  suitable variant), unlocking it with a matching :c:func:`!mmap_read_unlock` when
  you're done with the VMA, *or*
  (以 MM 为粒度通过 :c:func:`!mmap_read_lock` (或合适的变体)获取 mmap 读锁，用完 VMA 后用匹配的 :c:func:`!mmap_read_unlock` 解锁，*或者*)
* Try to obtain a VMA read lock via :c:func:`!lock_vma_under_rcu`. This tries to
  acquire the lock atomically so might fail, in which case fall-back logic is
  required to instead obtain an mmap read lock if this returns :c:macro:`!NULL`,
  *or*
  (尝试通过 :c:func:`!lock_vma_under_rcu` 获取 VMA 读锁。它以原子方式尝试获取锁，因此可能失败；若返回 :c:macro:`!NULL`，则需要回退逻辑改为获取 mmap 读锁，*或者*)
* Acquire an rmap lock before traversing the locked interval tree (whether
  anonymous or file-backed) to obtain the required VMA.
  (在遍历加了锁的区间树(无论匿名还是文件映射)以取得所需 VMA 之前，先获取 rmap 锁。)

If you want to **write** VMA metadata fields, then things vary depending on the
field (we explore each VMA field in detail below). For the majority you must:
(如果想 **写入** VMA 元数据字段，情况因字段而异(下文将详细讨论每个 VMA 字段)。对大多数字段你必须：)

* Obtain an mmap write lock at the MM granularity via :c:func:`!mmap_write_lock` (or a
  suitable variant), unlocking it with a matching :c:func:`!mmap_write_unlock` when
  you're done with the VMA, *and*
  (以 MM 为粒度通过 :c:func:`!mmap_write_lock` (或合适的变体)获取 mmap 写锁，用完 VMA 后用匹配的 :c:func:`!mmap_write_unlock` 解锁，*并且*)
* Obtain a VMA write lock via :c:func:`!vma_start_write` for each VMA you wish to
  modify, which will be released automatically when :c:func:`!mmap_write_unlock` is
  called.
  (对你想修改的每个 VMA，通过 :c:func:`!vma_start_write` 获取 VMA 写锁；当调用 :c:func:`!mmap_write_unlock` 时它会自动释放。)
* If you want to be able to write to **any** field, you must also hide the VMA
  from the reverse mapping by obtaining an **rmap write lock**.
  (如果希望对 **任何** 字段都可写，还必须获取 **rmap 写锁**，把 VMA 从反向映射中隐藏起来。)

VMA locks are special in that you must obtain an mmap **write** lock **first**
in order to obtain a VMA **write** lock. A VMA **read** lock however can be
obtained without any other lock (:c:func:`!lock_vma_under_rcu` will acquire then
release an RCU lock to lookup the VMA for you).
(VMA 锁的特殊之处在于：要获取 VMA **写** 锁，必须 **先** 获取 mmap **写** 锁。而 VMA **读** 锁可以在不持有任何其他锁的情况下获取(:c:func:`!lock_vma_under_rcu` 会获取再释放一个 RCU 锁来替你查找 VMA)。)

This constrains the impact of writers on readers, as a writer can interact with
one VMA while a reader interacts with another simultaneously.
(这限制了写者对读者的影响：写者与一个 VMA 交互时，读者可以同时与另一个 VMA 交互。)

.. note:: The primary users of VMA read locks are page fault handlers, which
          means that without a VMA write lock, page faults will run concurrent with
          whatever you are doing.
          (注意：VMA 读锁的主要使用者是缺页异常处理程序，这意味着如果没有 VMA 写锁，缺页异常将与你的操作并发运行。)

Examining all valid lock states:
(下面列出所有合法的加锁状态组合。)

.. table::

   ========= ======== ========= ======= ===== =========== ==========
   mmap lock VMA lock rmap lock Stable? Read? Write most? Write all?
   ========= ======== ========= ======= ===== =========== ==========
   \-        \-       \-        N       N     N           N
   \-        R        \-        Y       Y     N           N
   \-        \-       R/W       Y       Y     N           N
   R/W       \-/R     \-/R/W    Y       Y     N           N
   W         W        \-/R      Y       Y     Y           N
   W         W        W         Y       Y     Y           Y
   ========= ======== ========= ======= ===== =========== ==========

.. warning:: While it's possible to obtain a VMA lock while holding an mmap read lock,
             attempting to do the reverse is invalid as it can result in deadlock - if
             another task already holds an mmap write lock and attempts to acquire a VMA
             write lock that will deadlock on the VMA read lock.
             (警告：虽然可以在持有 mmap 读锁的同时获取 VMA 锁，但反过来做是无效的，可能导致死锁——如果另一个任务已持有 mmap 写锁并尝试获取 VMA 写锁，它将死等 VMA 读锁。)

All of these locks behave as read/write semaphores in practice, so you can
obtain either a read or a write lock for each of these.
(实践中所有这些锁都表现为读/写信号量，因此对每一种锁都可以获取读锁或写锁。)

.. note:: Generally speaking, a read/write semaphore is a class of lock which
          permits concurrent readers. However a write lock can only be obtained
          once all readers have left the critical region (and pending readers
          made to wait).

          This renders read locks on a read/write semaphore concurrent with other
          readers and write locks exclusive against all others holding the semaphore.
          (注意：一般而言，读/写信号量是一类允许读者并发持有的锁。但只有当所有读者都离开临界区(并让等待中的读者继续等待)之后，才能获取写锁。

          这意味着读/写信号量上的读锁可以与其他读者并发，而写锁对持有该信号量的所有其他人是排他的。)

VMA fields(VMA 字段)
^^^^^^^^^^^^^^^^^^^^

We can subdivide :c:struct:`!struct vm_area_struct` fields by their purpose, which makes it
easier to explore their locking characteristics:
(我们可以按用途对 :c:struct:`!struct vm_area_struct` 的字段进行分类，这样更容易考察它们的加锁特性：)

.. note:: We exclude VMA lock-specific fields here to avoid confusion, as these
          are in effect an internal implementation detail.
          (注意：为避免混淆，这里不列出 VMA 锁专用的字段，它们实际上是内部实现细节。)

.. table:: Virtual layout fields

   ===================== ======================================== ===========
   Field(字段)           Description(描述)                        Write lock
   ===================== ======================================== ===========
   :c:member:`!vm_start` Inclusive start virtual address of range mmap write,
                         VMA describes.                           VMA write,
                                                                  rmap write.
                         (VMA 所描述范围的起始虚拟地址，包含式。) (mmap 写锁、VMA 写锁、rmap 写锁。)
   :c:member:`!vm_end`   Exclusive end virtual address of range   mmap write,
                         VMA describes.                           VMA write,
                                                                  rmap write.
                         (VMA 所描述范围的结束虚拟地址，
                         不包含式。)                              (mmap 写锁、VMA 写锁、rmap 写锁。)
   :c:member:`!vm_pgoff` Describes the page offset into the file, mmap write,
                         the original page offset within the      VMA write,
                         virtual address space (prior to any      rmap write.
                         :c:func:`!mremap`), or PFN if a PFN map
                         and the architecture does not support
                         :c:macro:`!CONFIG_ARCH_HAS_PTE_SPECIAL`.
                         (描述指向文件的页偏移——即虚拟地址
                         空间中最初的页偏移(在任何
                         :c:func:`!mremap` 之前)；若为
                         PFN 映射且架构不支持
                         :c:macro:`!CONFIG_ARCH_HAS_PTE_SPECIAL`
                         ，则为 PFN。)
                                                                  (mmap 写锁、VMA 写锁、rmap 写锁。)
   ===================== ======================================== ===========

These fields describes the size, start and end of the VMA, and as such cannot be
modified without first being hidden from the reverse mapping since these fields
are used to locate VMAs within the reverse mapping interval trees.
(这些字段描述 VMA 的大小、起点和终点，因此如果不先把 VMA 从反向映射中隐藏，就不能修改它们——因为这些字段被用来在反向映射区间树中定位 VMA。)

.. table:: Core fields

   ============================ ======================================== =========================
   Field(字段)                  Description(描述)                        Write lock
   ============================ ======================================== =========================
   :c:member:`!vm_mm`           Containing mm_struct.                    None - written once on
                                                                         initial map.
                                (所属的 mm_struct。)
                                                                         (无——仅在初次映射时写入一次。)
   :c:member:`!vm_page_prot`    Architecture-specific page table         mmap write, VMA write.
                                protection bits determined from VMA
                                flags.
                                (VMA flags 决定的架构特定页表保护位。)
                                                                         (mmap 写锁、VMA 写锁。)
   :c:member:`!vm_flags`        Read-only access to VMA flags describing N/A
                                attributes of the VMA, in union with
                                private writable
                                :c:member:`!__vm_flags`.
                                (对描述 VMA 属性的 flags 的只读访问，
                                与私有的可写 :c:member:`!__vm_flags`
                                处于同一联合体(union)中。)
   :c:member:`!__vm_flags`      Private, writable access to VMA flags    mmap write, VMA write.
                                field, updated by
                                :c:func:`!vm_flags_*` functions.
                                (VMA flags 字段的私有可写访问，
                                由 :c:func:`!vm_flags_*` 函数更新。)
                                                                         (mmap 写锁、VMA 写锁。)
   :c:member:`!vm_file`         If the VMA is file-backed, points to a   None - written once on
                                struct file object describing the        initial map.
                                underlying file, if anonymous then
                                :c:macro:`!NULL`.
                                (若 VMA 为文件映射，则指向描述底层文件
                                的 struct file 对象；若为匿名映射则为
                                :c:macro:`!NULL`。)
                                                                         (无——仅在初次映射时写入一次。)
   :c:member:`!vm_ops`          If the VMA is file-backed, then either   None - Written once on
                                the driver or file-system provides a     initial map by
                                :c:struct:`!struct vm_operations_struct` :c:func:`!f_ops->mmap()`.
                                object describing callbacks to be
                                invoked on VMA lifetime events.
                                (若 VMA 为文件映射，
                                则由驱动或文件系统提供
                                :c:struct:`!struct vm_operations_struct`
                                对象，描述在 VMA 生命周期事件时
                                调用的回调。)
                                                                         (无——在初次映射时由
                                                                         :c:func:`!f_ops->mmap()` 写入。)
   :c:member:`!vm_private_data` A :c:member:`!void *` field for          Handled by driver.
                                driver-specific metadata.
                                (用于驱动特定元数据的
                                :c:member:`!void *` 字段。)
                                                                         (由驱动处理。)
   ============================ ======================================== =========================

These are the core fields which describe the MM the VMA belongs to and its attributes.
(这些是描述 VMA 所属的 MM 及其属性的核心字段。)

.. table:: Config-specific fields

   ================================= ===================== ======================================== ===============
   Field(字段)                       Configuration option  Description(描述)                        Write lock
   ================================= ===================== ======================================== ===============
   :c:member:`!anon_name`            CONFIG_ANON_VMA_NAME  A field for storing a                    mmap write,
                                                           :c:struct:`!struct anon_vma_name`        VMA write.
                                                           object providing a name for anonymous
                                                           mappings, or :c:macro:`!NULL` if none
                                                           is set or the VMA is file-backed. The
							   underlying object is reference counted
							   and can be shared across multiple VMAs
							   for scalability.
                                                           (存储匿名映射名称的字段，其中保存一个
                                                           :c:struct:`!struct anon_vma_name`
                                                           对象；若未设置名称或 VMA 为文件映射，
                                                           则为 :c:macro:`!NULL`。
                                                           底层对象采用引用计数，并可跨多个
                                                           VMA 共享，以获得更好的扩展性。)
                                                                                                    (mmap 写锁、VMA 写锁。)
   :c:member:`!swap_readahead_info`  CONFIG_SWAP           Metadata used by the swap mechanism      mmap read,
                                                           to perform readahead. This field is      swap-specific
                                                           accessed atomically.                     lock.
                                                           (交换机制用于执行预读的元数据。该字段
                                                           以原子方式访问。)
                                                                                                    (mmap 读锁、swap 专用锁。)
   :c:member:`!vm_policy`            CONFIG_NUMA           :c:type:`!mempolicy` object which        mmap write,
                                                           describes the NUMA behaviour of the      VMA write.
                                                           VMA. The underlying object is reference
							   counted.
                                                           (描述该 VMA 的 NUMA 行为的
                                                           :c:type:`!mempolicy` 对象。底层对象
                                                           采用引用计数。)
                                                                                                    (mmap 写锁、VMA 写锁。)
   :c:member:`!numab_state`          CONFIG_NUMA_BALANCING :c:type:`!vma_numab_state` object which  mmap read,
                                                           describes the current state of           numab-specific
                                                           NUMA balancing in relation to this VMA.  lock.
                                                           Updated under mmap read lock by
                                                           :c:func:`!task_numa_work`.
                                                           (描述与该 VMA 相关的 NUMA balancing
                                                           当前状态的 :c:type:`!vma_numab_state`
                                                           对象。在 mmap 读锁下由
                                                           :c:func:`!task_numa_work` 更新。)
                                                                                                    (mmap 读锁、numab 专用锁。)
   :c:member:`!vm_userfaultfd_ctx`   CONFIG_USERFAULTFD    Userfaultfd context wrapper object of    mmap write,
                                                           type :c:type:`!vm_userfaultfd_ctx`,      VMA write.
                                                           either of zero size if userfaultfd is
                                                           disabled, or containing a pointer
                                                           to an underlying
                                                           :c:type:`!userfaultfd_ctx` object which
                                                           describes userfaultfd metadata.
                                                           (userfaultfd 上下文包装对象，类型为
                                                           :c:type:`!vm_userfaultfd_ctx`，若
                                                           userfaultfd 被禁用则大小为零，否则包含
                                                           指向描述 userfaultfd 元数据的底层
                                                           :c:type:`!userfaultfd_ctx` 对象的
                                                           指针。)
                                                                                                    (mmap 写锁、VMA 写锁。)
   ================================= ===================== ======================================== ===============

These fields are present or not depending on whether the relevant kernel
configuration option is set.
(这些字段是否存在，取决于相关内核配置选项是否开启。)

.. table:: Reverse mapping fields

   =================================== ========================================= ============================
   Field(字段)                         Description(描述)                         Write lock
   =================================== ========================================= ============================
   :c:member:`!shared.rb`              A red/black tree node used, if the        mmap write, VMA write,
                                       mapping is file-backed, to place the VMA  i_mmap write.
                                       in the
                                       :c:member:`!struct address_space->i_mmap`
                                       red/black interval tree.
                                       (红黑树节点；若映射为文件映射，用于把
                                       VMA 放入
                                       :c:member:`!struct address_space->i_mmap`
                                       红黑区间树中。)
                                                                                 (mmap 写锁、VMA 写锁、i_mmap 写锁。)
   :c:member:`!shared.rb_subtree_last` Metadata used for management of the       mmap write, VMA write,
                                       interval tree if the VMA is file-backed.  i_mmap write.
                                       (若 VMA 为文件映射，用于管理区间树的
                                       元数据。)
                                                                                 (mmap 写锁、VMA 写锁、i_mmap 写锁。)
   :c:member:`!anon_vma_chain`         List of pointers to both forked/CoW’d     mmap read, anon_vma write.
                                       :c:type:`!anon_vma` objects and
                                       :c:member:`!vma->anon_vma` if it is
                                       non-:c:macro:`!NULL`.
                                       (指向 forked/CoW 的 :c:type:`!anon_vma`
                                       对象以及 :c:member:`!vma->anon_vma`
                                       的列表(若后者非 :c:macro:`!NULL`)。)
                                                                                 (mmap 读锁、anon_vma 写锁。)
   :c:member:`!anon_vma`               :c:type:`!anon_vma` object used by        When :c:macro:`NULL` and
                                       anonymous folios mapped exclusively to    setting non-:c:macro:`NULL`:
                                       this VMA. Initially set by                mmap read, page_table_lock.
                                       :c:func:`!anon_vma_prepare` serialised
                                       by the :c:macro:`!page_table_lock`. This  When non-:c:macro:`NULL` and
                                       is set as soon as any page is faulted in. setting :c:macro:`NULL`:
                                                                                 mmap write, VMA write,
                                                                                 anon_vma write.
                                       (:c:type:`!anon_vma` 对象，由独占地映射到
                                       该 VMA 的匿名 folio 所使用。最初由
                                       :c:func:`!anon_vma_prepare` 设置，并由
                                       :c:macro:`!page_table_lock`
                                       串行化。只要有任何页被调入，即会设置。)
                                                                                 (当为 :c:macro:`NULL` 且设为非
                                                                                 :c:macro:`NULL` 时：mmap 读锁、
                                                                                 page_table_lock。当非 :c:macro:`NULL`
                                                                                 且设为 :c:macro:`NULL` 时：mmap 写锁、
                                                                                 VMA 写锁、anon_vma 写锁。)
   =================================== ========================================= ============================

These fields are used to both place the VMA within the reverse mapping, and for
anonymous mappings, to be able to access both related :c:struct:`!struct anon_vma` objects
and the :c:struct:`!struct anon_vma` in which folios mapped exclusively to this VMA should
reside.
(这些字段一方面用于把 VMA 放入反向映射，另一方面对于匿名映射，还用于访问相关的
:c:struct:`!struct anon_vma` 对象，以及 folio 独占地映射到该 VMA 时应驻留的那个
:c:struct:`!struct anon_vma`。)

.. note:: If a file-backed mapping is mapped with :c:macro:`!MAP_PRIVATE` set
          then it can be in both the :c:type:`!anon_vma` and :c:type:`!i_mmap`
          trees at the same time, so all of these fields might be utilised at
          once.
          (注意：如果一个文件映射在映射时设置了 :c:macro:`!MAP_PRIVATE` ，那么它可以
          同时位于 :c:type:`!anon_vma` 和 :c:type:`!i_mmap` 两棵树中，因此所有这些
          字段可能同时被用到。)

Page tables(页表)
-----------------

We won't speak exhaustively on the subject but broadly speaking, page tables map
virtual addresses to physical ones through a series of page tables, each of
which contain entries with physical addresses for the next page table level
(along with flags), and at the leaf level the physical addresses of the
underlying physical data pages or a special entry such as a swap entry,
migration entry or other special marker. Offsets into these pages are provided
by the virtual address itself.
(这里不再赘述细节，但大体而言，页表通过一系列页表把虚拟地址映射为物理地址：其中每一级
页表都包含指向下一级页表物理地址的表项(连同标志位)，而在叶子层级，则是底层数据页的
物理地址，或诸如 swap 项、迁移项或其他特殊标记之类的特殊表项。页内偏移由虚拟地址本身
提供。)

In Linux these are divided into five levels - PGD, P4D, PUD, PMD and PTE. Huge
pages might eliminate one or two of these levels, but when this is the case we
typically refer to the leaf level as the PTE level regardless.
(在 Linux 中，页表分为五级——PGD、P4D、PUD、PMD 和 PTE。大页可能消除其中一到两级，
但即便如此，我们通常仍把叶子层级称为 PTE 级。)

.. note:: In instances where the architecture supports fewer page tables than
	  five the kernel cleverly 'folds' page table levels, that is stubbing
	  out functions related to the skipped levels. This allows us to
	  conceptually act as if there were always five levels, even if the
	  compiler might, in practice, eliminate any code relating to missing
	  ones.
          (注意：在架构支持的页表层级少于五级的情况下，内核会聪明地"折叠"页表层级，
          即把与被跳过层级相关的函数做成桩。这让我们在概念上可以始终当作有五级来处理，
          即使编译器实际上可能会把与缺失层级相关的任何代码都消除掉。)

There are four key operations typically performed on page tables:
(页表上通常执行四种关键操作：)

1. **Traversing** page tables - Simply reading page tables in order to traverse
   them. This only requires that the VMA is kept stable, so a lock which
   establishes this suffices for traversal (there are also lockless variants
   which eliminate even this requirement, such as :c:func:`!gup_fast`). There is
   also a special case of page table traversal for non-VMA regions which we
   consider separately below.
   (**遍历** 页表——即单纯地读取页表以遍历它们。这只需要让 VMA 保持稳定，因此任何
   能建立这一点的锁都足以完成遍历(也有一些无锁变体，连这个要求都不需要，例如
   :c:func:`!gup_fast`)。针对非 VMA 区域的页表遍历是一个特例，我们在下文单独讨论。)
2. **Installing** page table mappings - Whether creating a new mapping or
   modifying an existing one in such a way as to change its identity. This
   requires that the VMA is kept stable via an mmap or VMA lock (explicitly not
   rmap locks).
   (**安装** 页表映射——无论是创建新映射，还是以改变其身份的方式修改现有映射。这
   要求通过 mmap 锁或 VMA 锁使 VMA 保持稳定(明确地不包括 rmap 锁)。)
3. **Zapping/unmapping** page table entries - This is what the kernel calls
   clearing page table mappings at the leaf level only, whilst leaving all page
   tables in place. This is a very common operation in the kernel performed on
   file truncation, the :c:macro:`!MADV_DONTNEED` operation via
   :c:func:`!madvise`, and others. This is performed by a number of functions
   including :c:func:`!unmap_mapping_range` and :c:func:`!unmap_mapping_pages`.
   The VMA need only be kept stable for this operation.
   (**清空/解除映射 (zap/unmap)** 页表表项——这是内核对"仅清除叶子层级的页表映射、
   同时保留所有页表结构"的说法。这是内核中非常常见的操作，在文件截断 (file
   truncation)、通过 :c:func:`!madvise` 执行的 :c:macro:`!MADV_DONTNEED` 操作等场景中
   进行。执行它的函数有很多，包括 :c:func:`!unmap_mapping_range` 和
   :c:func:`!unmap_mapping_pages`。该操作只需保持 VMA 稳定。)
4. **Freeing** page tables - When finally the kernel removes page tables from a
   userland process (typically via :c:func:`!free_pgtables`) extreme care must
   be taken to ensure this is done safely, as this logic finally frees all page
   tables in the specified range, ignoring existing leaf entries (it assumes the
   caller has both zapped the range and prevented any further faults or
   modifications within it).
   (**释放** 页表——当内核最终从一个用户态进程中移除页表时(通常通过
   :c:func:`!free_pgtables`)，必须格外小心以确保这样做是安全的，因为这段逻辑最终会
   释放指定范围内的所有页表，并忽略现有的叶子表项(它假定调用方既已清空该范围，又
   阻止了其中的任何后续缺页或修改)。)

.. note:: Modifying mappings for reclaim or migration is performed under rmap
          lock as it, like zapping, does not fundamentally modify the identity
          of what is being mapped.
          (注意：为回收 (reclaim) 或迁移 (migration) 而修改映射是在 rmap 锁下进行的，
          因为与清空一样，它并不会从根本上改变被映射对象的身份。)

**Traversing** and **zapping** ranges can be performed holding any one of the
locks described in the terminology section above - that is the mmap lock, the
VMA lock or either of the reverse mapping locks.
(**遍历** 与 **清空** 范围的操作，可以在持有上文术语一节所述任意一把锁的情况下进行
——即 mmap 锁、VMA 锁或两把反向映射锁中的任一把。)

That is - as long as you keep the relevant VMA **stable** - you are good to go
ahead and perform these operations on page tables (though internally, kernel
operations that perform writes also acquire internal page table locks to
serialise - see the page table implementation detail section for more details).
(也就是说——只要让相关 VMA 保持 **稳定** ——你就可以放心地对页表执行这些操作
(不过在内部，执行写操作的内核操作还会获取内部页表锁来串行化——详见页表实现细节
一节)。)

.. note:: We free empty PTE tables on zap under the RCU lock - this does not
          change the aforementioned locking requirements around zapping.
          (注意：清空时我们会在 RCU 锁下释放空的 PTE 表——这不会改变上述围绕清空
          操作的加锁要求。)

When **installing** page table entries, the mmap or VMA lock must be held to
keep the VMA stable. We explore why this is in the page table locking details
section below.
(**安装** 页表表项时，必须持有 mmap 锁或 VMA 锁以保持 VMA 稳定。下文的页表加锁
细节一节会探讨其中的原因。)

**Freeing** page tables is an entirely internal memory management operation and
has special requirements (see the page freeing section below for more details).
(**释放** 页表是完全内部的内存管理操作，有特殊要求(详见下文的页表释放一节)。)

.. warning:: When **freeing** page tables, it must not be possible for VMAs
             containing the ranges those page tables map to be accessible via
             the reverse mapping.

             The :c:func:`!free_pgtables` function removes the relevant VMAs
             from the reverse mappings, but no other VMAs can be permitted to be
             accessible and span the specified range.
             (警告：**释放** 页表时，包含这些页表所映射范围的 VMA 绝不能还能通过
             反向映射访问到。:c:func:`!free_pgtables` 函数会把相关 VMA 从反向映射中
             移除，但不允许任何其他 VMA 仍可访问并跨越指定范围。)

Traversing non-VMA page tables(遍历非 VMA 页表)
-----------------------------------------------

We've focused above on traversal of page tables belonging to VMAs. It is also
possible to traverse page tables which are not represented by VMAs.
(上文我们聚焦于遍历属于 VMA 的页表。但也可以遍历不由任何 VMA 表示的页表。)

Kernel page table mappings themselves are generally managed but whatever part of
the kernel established them and the aforementioned locking rules do not apply -
for instance vmalloc has its own set of locks which are utilised for
establishing and tearing down page its page tables.
(内核页表映射本身通常被妥善管理，但无论是内核的哪个部分建立了它们，前述加锁规则都
不适用——例如 vmalloc 有自己的一组锁，用于建立和拆除其页表。)

However, for convenience we provide the :c:func:`!walk_kernel_page_table_range`
function which is synchronised via the mmap lock on the :c:macro:`!init_mm`
kernel instantiation of the :c:struct:`!struct mm_struct` metadata object.
(不过为了方便，我们提供了 :c:func:`!walk_kernel_page_table_range` 函数，它通过
:c:macro:`!init_mm` ——即内核对 :c:struct:`!struct mm_struct` 元数据对象的实例化
——上的 mmap 锁进行同步。)

If an operation requires exclusive access, a write lock is used, but if not, a
read lock suffices - we assert only that at least a read lock has been acquired.
(如果操作需要独占访问，就使用写锁；否则读锁即可——我们只断言至少已获取读锁。)

Since, aside from vmalloc and memory hot plug, kernel page tables are not torn
down all that often - this usually suffices, however any caller of this
functionality must ensure that any additionally required locks are acquired in
advance.
(由于除 vmalloc 和内存热插拔之外，内核页表并不常被拆除——这通常已经足够；不过，任何
调用此功能的人必须确保提前获取所有额外需要的锁。)

We also permit a truly unusual case is the traversal of non-VMA ranges in
**userland** ranges, as provided for by :c:func:`!walk_page_range_debug`.
(我们还允许一个真正罕见的情况：遍历 **用户态** 范围中的非 VMA 区域，如
:c:func:`!walk_page_range_debug` 所提供的那样。)

This has only one user - the general page table dumping logic (implemented in
:c:macro:`!mm/ptdump.c`) - which seeks to expose all mappings for debug purposes
even if they are highly unusual (possibly architecture-specific) and are not
backed by a VMA.
(它只有一个使用者——通用页表转储逻辑(实现于 :c:macro:`!mm/ptdump.c`)——其目的
是为了调试而暴露所有映射，即使它们极不寻常(可能是架构特定的)且没有 VMA 支撑。)

We must take great care in this case, as the :c:func:`!munmap` implementation
detaches VMAs under an mmap write lock before tearing down page tables under a
downgraded mmap read lock.
(在这种情况下必须极其小心，因为 :c:func:`!munmap` 的实现会先在 mmap 写锁下分离
VMA，再在降级后的 mmap 读锁下拆除页表。)

This means such an operation could race with this, and thus an mmap **write**
lock is required.
(这意味着这类操作可能与它发生竞态，因此需要 mmap **写** 锁。)

Lock ordering(锁顺序)
---------------------

As we have multiple locks across the kernel which may or may not be taken at the
same time as explicit mm or VMA locks, we have to be wary of lock inversion, and
the **order** in which locks are acquired and released becomes very important.
(内核中存在多把锁，它们可能与显式的 mm 或 VMA 锁同时持有，也可能不会，因此我们必须
警惕锁反转 (lock inversion)，获取与释放锁的 **顺序** 变得非常重要。)

.. note:: Lock inversion occurs when two threads need to acquire multiple locks,
   but in doing so inadvertently cause a mutual deadlock.

   For example, consider thread 1 which holds lock A and tries to acquire lock B,
   while thread 2 holds lock B and tries to acquire lock A.

   Both threads are now deadlocked on each other. However, had they attempted to
   acquire locks in the same order, one would have waited for the other to
   complete its work and no deadlock would have occurred.
   (注意：锁反转发生在两个线程都需要获取多把锁、但在此过程中不经意地造成相互死锁的
   时候。例如，线程 1 持有锁 A 并尝试获取锁 B，而线程 2 持有锁 B 并尝试获取锁 A。
   此时两个线程彼此死锁。但如果它们按相同顺序获取锁，其中一个就会等待另一个完成
   工作，死锁便不会发生。)

The opening comment in :c:macro:`!mm/rmap.c` describes in detail the required
ordering of locks within memory management code:
(:c:macro:`!mm/rmap.c` 开头的注释详细描述了内存管理代码中所需的锁顺序：)

.. code-block::

  inode->i_rwsem        (while writing or truncating, not reading or faulting)
    mm->mmap_lock
      mapping->invalidate_lock (in filemap_fault)
        folio_lock
          hugetlbfs_i_mmap_rwsem_key (in huge_pmd_share, see hugetlbfs below)
            vma_start_write
              mapping->i_mmap_rwsem
                anon_vma->rwsem
                  mm->page_table_lock or pte_lock
                    swap_lock (in swap_duplicate, swap_info_get)
                      mmlist_lock (in mmput, drain_mmlist and others)
                      mapping->private_lock (in block_dirty_folio)
                          i_pages lock (widely used)
                            lruvec->lru_lock (in folio_lruvec_lock_irq)
                      inode->i_lock (in set_page_dirty's __mark_inode_dirty)
                      bdi.wb->list_lock (in set_page_dirty's __mark_inode_dirty)
                        sb_lock (within inode_lock in fs/fs-writeback.c)
                        i_pages lock (widely used, in set_page_dirty,
                                  in arch-dependent flush_dcache_mmap_lock,
                                  within bdi.wb->list_lock in __sync_single_inode)

There is also a file-system specific lock ordering comment located at the top of
:c:macro:`!mm/filemap.c`:
(:c:macro:`!mm/filemap.c` 顶部还有一段文件系统特定的锁顺序注释：)

.. code-block::

  ->i_mmap_rwsem                        (truncate_pagecache)
    ->private_lock                      (__free_pte->block_dirty_folio)
      ->swap_lock                       (exclusive_swap_page, others)
        ->i_pages lock

  ->i_rwsem
    ->invalidate_lock                   (acquired by fs in truncate path)
      ->i_mmap_rwsem                    (truncate->unmap_mapping_range)

  ->mmap_lock
    ->i_mmap_rwsem
      ->page_table_lock or pte_lock     (various, mainly in memory.c)
        ->i_pages lock                  (arch-dependent flush_dcache_mmap_lock)

  ->mmap_lock
    ->invalidate_lock                   (filemap_fault)
      ->lock_page                       (filemap_fault, access_process_vm)

  ->i_rwsem                             (generic_perform_write)
    ->mmap_lock                         (fault_in_readable->do_page_fault)

  bdi->wb.list_lock
    sb_lock                             (fs/fs-writeback.c)
    ->i_pages lock                      (__sync_single_inode)

  ->i_mmap_rwsem
    ->anon_vma.lock                     (vma_merge)

  ->anon_vma.lock
    ->page_table_lock or pte_lock       (anon_vma_prepare and various)

  ->page_table_lock or pte_lock
    ->swap_lock                         (try_to_unmap_one)
    ->private_lock                      (try_to_unmap_one)
    ->i_pages lock                      (try_to_unmap_one)
    ->lruvec->lru_lock                  (follow_page_mask->mark_page_accessed)
    ->lruvec->lru_lock                  (check_pte_range->folio_isolate_lru)
    ->private_lock                      (folio_remove_rmap_pte->set_page_dirty)
    ->i_pages lock                      (folio_remove_rmap_pte->set_page_dirty)
    bdi.wb->list_lock                   (folio_remove_rmap_pte->set_page_dirty)
    ->inode->i_lock                     (folio_remove_rmap_pte->set_page_dirty)
    bdi.wb->list_lock                   (zap_pte_range->set_page_dirty)
    ->inode->i_lock                     (zap_pte_range->set_page_dirty)
    ->private_lock                      (zap_pte_range->block_dirty_folio)

Please check the current state of these comments which may have changed since
the time of writing of this document.
(请注意查看这些注释的当前状态，它们自本文撰写之时起可能已经发生变化。)

--------------------------------------------
Locking Implementation Details(加锁实现细节)
--------------------------------------------
.. warning:: Locking rules for PTE-level page tables are very different from
             locking rules for page tables at other levels.
             (警告：PTE 级页表的加锁规则与其他层级的页表有很大不同。)

Page table locking details(页表加锁细节)
----------------------------------------
.. note:: This section explores page table locking requirements for page tables
          encompassed by a VMA. See the above section on non-VMA page table
          traversal for details on how we handle that case.
          (注意：本节讨论被 VMA 包含的页表的加锁要求。关于非 VMA 页表遍历的处理
          方式，请参见上文相应章节。)

In addition to the locks described in the terminology section above, we have
additional locks dedicated to page tables:
(除上文术语一节描述的锁之外，还有一些专门用于页表的锁：)

* **Higher level page table locks** - Higher level page tables, that is PGD, P4D
  and PUD each make use of the process address space granularity
  :c:member:`!mm->page_table_lock` lock when modified.
  (**高层页表锁** ——高层页表，即 PGD、P4D 和 PUD，在被修改时各自使用进程地址空间
  粒度的 :c:member:`!mm->page_table_lock` 锁。)

* **Fine-grained page table locks** - PMDs and PTEs each have fine-grained locks
  either kept within the folios describing the page tables or allocated
  separated and pointed at by the folios if :c:macro:`!ALLOC_SPLIT_PTLOCKS` is
  set. The PMD spin lock is obtained via :c:func:`!pmd_lock`, however PTEs are
  mapped into higher memory (if a 32-bit system) and carefully locked via
  :c:func:`!pte_offset_map_lock`.
  (**细粒度页表锁** ——PMD 和 PTE 各有细粒度锁：这些锁要么保存在描述页表的 folio
  中，要么在设置了 :c:macro:`!ALLOC_SPLIT_PTLOCKS` 时单独分配并由 folio 指向。PMD
  自旋锁通过 :c:func:`!pmd_lock` 获取；而 PTE(在 32 位系统上)会被映射到高端内存，
  并通过 :c:func:`!pte_offset_map_lock` 小心地加锁。)

These locks represent the minimum required to interact with each page table
level, but there are further requirements.
(这些锁代表了与每一级页表交互所需的最低要求，但还有更多要求。)

Importantly, note that on a **traversal** of page tables, sometimes no such
locks are taken. However, at the PTE level, at least concurrent page table
deletion must be prevented (using RCU) and the page table must be mapped into
high memory, see below.
(重要提示：在 **遍历** 页表时，有时根本不获取这类锁。但在 PTE 级，至少必须防止
并发的页表删除(使用 RCU)，且页表必须已映射进高端内存，见下文。)

Whether care is taken on reading the page table entries depends on the
architecture, see the section on atomicity below.
(读取页表表项时是否需要特殊处理取决于架构，参见下文的原子性一节。)

Locking rules(加锁规则)
^^^^^^^^^^^^^^^^^^^^^^^

We establish basic locking rules when interacting with page tables:
(在与页表交互时，我们确立以下基本加锁规则：)

* When changing a page table entry the page table lock for that page table
  **must** be held, except if you can safely assume nobody can access the page
  tables concurrently (such as on invocation of :c:func:`!free_pgtables`).
  (修改某个页表表项时，**必须** 持有该页表的页表锁——除非你可以安全地假定没有人会
  并发访问这些页表(例如在调用 :c:func:`!free_pgtables` 时)。)
* Reads from and writes to page table entries must be *appropriately*
  atomic. See the section on atomicity below for details.
  (对页表表项的读与写都必须 *恰当地* 保持原子性。细节参见下文的原子性一节。)
* Populating previously empty entries requires that the mmap or VMA locks are
  held (read or write), doing so with only rmap locks would be dangerous (see
  the warning below).
  (填充先前为空的表项要求持有 mmap 锁或 VMA 锁(读或写)；仅持 rmap 锁这样做是
  危险的(见下文警告)。)
* As mentioned previously, zapping can be performed while simply keeping the VMA
  stable, that is holding any one of the mmap, VMA or rmap locks.
  (如前所述，只需保持 VMA 稳定即可执行清空，也就是持有 mmap、VMA 或 rmap 锁中的
  任意一把。)

.. warning:: Populating previously empty entries is dangerous as, when unmapping
             VMAs, :c:func:`!vms_clear_ptes` has a window of time between
             zapping (via :c:func:`!unmap_vmas`) and freeing page tables (via
             :c:func:`!free_pgtables`), where the VMA is still visible in the
             rmap tree. :c:func:`!free_pgtables` assumes that the zap has
             already been performed and removes PTEs unconditionally (along with
             all other page tables in the freed range), so installing new PTE
             entries could leak memory and also cause other unexpected and
             dangerous behaviour.
             (警告：填充先前为空的表项是危险的：在解除 VMA 映射时，
             :c:func:`!vms_clear_ptes` 在清空(通过 :c:func:`!unmap_vmas`)与释放
             页表(通过 :c:func:`!free_pgtables`)之间有一个时间窗口，此时 VMA 在
             rmap 树中仍然可见。:c:func:`!free_pgtables` 假定清空已经完成，并无条件
             地移除 PTE(连同被释放范围内的所有其他页表)，因此安装新的 PTE 表项可能
             导致内存泄漏，并引发其他意外且危险的行为。)

There are additional rules applicable when moving page tables, which we discuss
in the section on this topic below.
(移动页表时还有额外的规则，我们将在下文相关章节讨论。)

PTE-level page tables are different from page tables at other levels, and there
are extra requirements for accessing them:
(PTE 级页表与其他层级的页表不同，访问它们有额外要求：)

* On 32-bit architectures, they may be in high memory (meaning they need to be
  mapped into kernel memory to be accessible).
  (在 32 位架构上，它们可能位于高端内存中(即需要先映射进内核内存才能访问)。)
* When empty, they can be unlinked and RCU-freed while holding an mmap lock or
  rmap lock for reading in combination with the PTE and PMD page table locks.
  In particular, this happens in :c:func:`!retract_page_tables` when handling
  :c:macro:`!MADV_COLLAPSE`.
  So accessing PTE-level page tables requires at least holding an RCU read lock;
  but that only suffices for readers that can tolerate racing with concurrent
  page table updates such that an empty PTE is observed (in a page table that
  has actually already been detached and marked for RCU freeing) while another
  new page table has been installed in the same location and filled with
  entries. Writers normally need to take the PTE lock and revalidate that the
  PMD entry still refers to the same PTE-level page table.
  If the writer does not care whether it is the same PTE-level page table, it
  can take the PMD lock and revalidate that the contents of pmd entry still meet
  the requirements. In particular, this also happens in :c:func:`!retract_page_tables`
  when handling :c:macro:`!MADV_COLLAPSE`.
  (当它们为空时，可以在持有用于读取的 mmap 锁或 rmap 锁、并结合 PTE 与 PMD 页表锁
  的情况下，把它们解除链接并经 RCU 释放。这尤其发生在处理
  :c:macro:`!MADV_COLLAPSE` 时的 :c:func:`!retract_page_tables` 中。因此，访问
  PTE 级页表至少要持有 RCU 读锁；但这只对能容忍与并发页表更新竞态的读者足够——即
  观察到空 PTE(该页表实际上已被分离并标记为待 RCU 释放)，而同一位置已安装了另一个
  新页表并填充了表项。写者通常需要获取 PTE 锁并重新验证 PMD 表项仍指向同一个 PTE 级
  页表。如果写者不在乎是否是同一个 PTE 级页表，可以获取 PMD 锁并重新验证 pmd 表项的
  内容仍满足要求。这同样发生在处理 :c:macro:`!MADV_COLLAPSE` 时的
  :c:func:`!retract_page_tables` 中。)

To access PTE-level page tables, a helper like :c:func:`!pte_offset_map_lock` or
:c:func:`!pte_offset_map` can be used depending on stability requirements.
These map the page table into kernel memory if required, take the RCU lock, and
depending on variant, may also look up or acquire the PTE lock.
See the comment on :c:func:`!pte_offset_map_lock`.
(要访问 PTE 级页表，可根据稳定性要求使用 :c:func:`!pte_offset_map_lock` 或
:c:func:`!pte_offset_map` 之类的辅助函数。它们会在需要时把页表映射进内核内存、获取
RCU 锁，并且视变体不同，还可能查找或获取 PTE 锁。参见
:c:func:`!pte_offset_map_lock` 的注释。)

Atomicity(原子性)
^^^^^^^^^^^^^^^^^

Regardless of page table locks, the MMU hardware concurrently updates accessed
and dirty bits (perhaps more, depending on architecture). Additionally, page
table traversal operations in parallel (though holding the VMA stable) and
functionality like GUP-fast locklessly traverses (that is reads) page tables,
without even keeping the VMA stable at all.
(无论页表锁如何，MMU 硬件都会并发更新 accessed 与 dirty 位(取决于架构，可能还有
更多)。此外，页表遍历操作可以并行进行(尽管保持 VMA 稳定)，而 GUP-fast 之类的功能
甚至会完全不加锁、也不保持 VMA 稳定地遍历(即读取)页表。)

When performing a page table traversal and keeping the VMA stable, whether a
read must be performed once and only once or not depends on the architecture
(for instance x86-64 does not require any special precautions).
(在执行页表遍历并保持 VMA 稳定时，读操作是否必须一次且仅一次，取决于架构(例如
x86-64 不需要任何特殊防范)。)

If a write is being performed, or if a read informs whether a write takes place
(on an installation of a page table entry say, for instance in
:c:func:`!__pud_install`), special care must always be taken. In these cases we
can never assume that page table locks give us entirely exclusive access, and
must retrieve page table entries once and only once.
(如果正在执行写操作，或者一次读操作会决定是否进行写(例如在安装页表表项时，比如
:c:func:`!__pud_install` 中)，则始终必须特别小心。在这些情况下，我们绝不能假定
页表锁给了我们完全独占的访问，必须一次且仅一次地读取页表表项。)

If we are reading page table entries, then we need only ensure that the compiler
does not rearrange our loads. This is achieved via :c:func:`!pXXp_get`
functions - :c:func:`!pgdp_get`, :c:func:`!p4dp_get`, :c:func:`!pudp_get`,
:c:func:`!pmdp_get`, and :c:func:`!ptep_get`.
(如果我们只是读取页表表项，那么只需确保编译器不会重排我们的加载。这通过
:c:func:`!pXXp_get` 系列函数实现——:c:func:`!pgdp_get`、:c:func:`!p4dp_get`、
:c:func:`!pudp_get`、:c:func:`!pmdp_get` 与 :c:func:`!ptep_get`。)

Each of these uses :c:func:`!READ_ONCE` to guarantee that the compiler reads
the page table entry only once.
(它们每一个都使用 :c:func:`!READ_ONCE` 来保证编译器只读取一次页表表项。)

However, if we wish to manipulate an existing page table entry and care about
the previously stored data, we must go further and use an hardware atomic
operation as, for example, in :c:func:`!ptep_get_and_clear`.
(不过，如果我们想操作一个已有的页表表项并且关心先前存储的数据，就必须更进一步，
使用硬件原子操作，例如 :c:func:`!ptep_get_and_clear` 中那样。)

Equally, operations that do not rely on the VMA being held stable, such as
GUP-fast (see :c:func:`!gup_fast` and its various page table level handlers like
:c:func:`!gup_fast_pte_range`), must very carefully interact with page table
entries, using functions such as :c:func:`!ptep_get_lockless` and equivalent for
higher level page table levels.
(同样，不依赖 VMA 保持稳定的操作，例如 GUP-fast(见 :c:func:`!gup_fast` 及其各级
页表处理函数，如 :c:func:`!gup_fast_pte_range`)，必须以非常小心的方式与页表表项
交互，使用 :c:func:`!ptep_get_lockless` 之类的函数，以及更高层级页表上的等价函数。)

Writes to page table entries must also be appropriately atomic, as established
by :c:func:`!set_pXX` functions - :c:func:`!set_pgd`, :c:func:`!set_p4d`,
:c:func:`!set_pud`, :c:func:`!set_pmd`, and :c:func:`!set_pte`.
(对页表表项的写也必须恰当地保持原子性，由 :c:func:`!set_pXX` 系列函数确立——
:c:func:`!set_pgd`、:c:func:`!set_p4d`、:c:func:`!set_pud`、:c:func:`!set_pmd`
与 :c:func:`!set_pte`。)

Equally functions which clear page table entries must be appropriately atomic,
as in :c:func:`!pXX_clear` functions - :c:func:`!pgd_clear`,
:c:func:`!p4d_clear`, :c:func:`!pud_clear`, :c:func:`!pmd_clear`, and
:c:func:`!pte_clear`.
(同样，清除页表表项的函数也必须恰当地保持原子性，如 :c:func:`!pXX_clear` 系列
——:c:func:`!pgd_clear`、:c:func:`!p4d_clear`、:c:func:`!pud_clear`、
:c:func:`!pmd_clear` 与 :c:func:`!pte_clear`。)

Page table installation(页表安装)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Page table installation is performed with the VMA held stable explicitly by an
mmap or VMA lock in read or write mode (see the warning in the locking rules
section for details as to why).
(页表安装是在 VMA 被显式地以读或写模式的 mmap 锁或 VMA 锁保持稳定的情况下进行的
(原因详见加锁规则一节中的警告)。)

When allocating a P4D, PUD or PMD and setting the relevant entry in the above
PGD, P4D or PUD, the :c:member:`!mm->page_table_lock` must be held. This is
acquired in :c:func:`!__p4d_alloc`, :c:func:`!__pud_alloc` and
:c:func:`!__pmd_alloc` respectively.
(分配 P4D、PUD 或 PMD 并在其上层的 PGD、P4D 或 PUD 中设置相应表项时，必须持有
:c:member:`!mm->page_table_lock`。分别在 :c:func:`!__p4d_alloc`、
:c:func:`!__pud_alloc` 与 :c:func:`!__pmd_alloc` 中获取。)

.. note:: :c:func:`!__pmd_alloc` actually invokes :c:func:`!pud_lock` and
   :c:func:`!pud_lockptr` in turn, however at the time of writing it ultimately
   references the :c:member:`!mm->page_table_lock`.
   (注意：:c:func:`!__pmd_alloc` 实际上会依次调用 :c:func:`!pud_lock` 与
   :c:func:`!pud_lockptr`，不过截至撰写本文时，它最终引用的仍是
   :c:member:`!mm->page_table_lock`。)

Allocating a PTE will either use the :c:member:`!mm->page_table_lock` or, if
:c:macro:`!USE_SPLIT_PMD_PTLOCKS` is defined, a lock embedded in the PMD
physical page metadata in the form of a :c:struct:`!struct ptdesc`, acquired by
:c:func:`!pmd_ptdesc` called from :c:func:`!pmd_lock` and ultimately
:c:func:`!__pte_alloc`.
(分配 PTE 时要么使用 :c:member:`!mm->page_table_lock`，要么在定义了
:c:macro:`!USE_SPLIT_PMD_PTLOCKS` 的情况下，使用以 :c:struct:`!struct ptdesc`
形式内嵌于 PMD 物理页元数据中的锁——它由 :c:func:`!pmd_lock` 中调用的
:c:func:`!pmd_ptdesc` 获取，最终用于 :c:func:`!__pte_alloc`。)

Finally, modifying the contents of the PTE requires special treatment, as the
PTE page table lock must be acquired whenever we want stable and exclusive
access to entries contained within a PTE, especially when we wish to modify
them.
(最后，修改 PTE 的内容需要特殊处理：每当我们要对 PTE 所含表项进行稳定且独占的访问
时——尤其是想修改它们时——都必须获取 PTE 页表锁。)

This is performed via :c:func:`!pte_offset_map_lock` which carefully checks to
ensure that the PTE hasn't changed from under us, ultimately invoking
:c:func:`!pte_lockptr` to obtain a spin lock at PTE granularity contained within
the :c:struct:`!struct ptdesc` associated with the physical PTE page. The lock
must be released via :c:func:`!pte_unmap_unlock`.
(这通过 :c:func:`!pte_offset_map_lock` 完成，它会仔细检查以确保 PTE 没有在我们
脚下发生变化，最终调用 :c:func:`!pte_lockptr` 获取一个 PTE 粒度的自旋锁，该锁内嵌
在与物理 PTE 页关联的 :c:struct:`!struct ptdesc` 中。锁必须通过
:c:func:`!pte_unmap_unlock` 释放。)

.. note:: There are some variants on this, such as
   :c:func:`!pte_offset_map_rw_nolock` when we know we hold the PTE stable but
   for brevity we do not explore this.  See the comment for
   :c:func:`!pte_offset_map_lock` for more details.
   (注意：这有一些变体，例如在已知我们使 PTE 保持稳定时使用的
   :c:func:`!pte_offset_map_rw_nolock`；为简洁起见我们不展开。更多细节参见
   :c:func:`!pte_offset_map_lock` 的注释。)

When modifying data in ranges we typically only wish to allocate higher page
tables as necessary, using these locks to avoid races or overwriting anything,
and set/clear data at the PTE level as required (for instance when page faulting
or zapping).
(修改范围内的数据时，我们通常只希望按需分配更高级的页表，利用这些锁避免竞态或覆盖
任何东西，并按需在 PTE 级设置/清除数据(例如缺页或清空时)。)

A typical pattern taken when traversing page table entries to install a new
mapping is to optimistically determine whether the page table entry in the table
above is empty, if so, only then acquiring the page table lock and checking
again to see if it was allocated underneath us.
(遍历页表表项以安装新映射时的典型模式是：乐观地判断上一级表中的页表表项是否为空，
若为空，才获取页表锁并再次检查它是否已在我们脚下被分配。)

This allows for a traversal with page table locks only being taken when
required. An example of this is :c:func:`!__pud_alloc`.
(这使遍历只需在必要时才获取页表锁。:c:func:`!__pud_alloc` 就是一个例子。)

At the leaf page table, that is the PTE, we can't entirely rely on this pattern
as we have separate PMD and PTE locks and a THP collapse for instance might have
eliminated the PMD entry as well as the PTE from under us.
(在叶子页表(即 PTE)处，我们不能完全依赖这种模式，因为 PMD 与 PTE 锁是分开的，
而且例如 THP collapse 可能已把我们脚下的 PMD 表项连同 PTE 一起消除。)

This is why :c:func:`!pte_offset_map_lock` locklessly retrieves the PMD entry
for the PTE, carefully checking it is as expected, before acquiring the
PTE-specific lock, and then *again* checking that the PMD entry is as expected.
(这就是 :c:func:`!pte_offset_map_lock` 先以无锁方式取得该 PTE 的 PMD 表项并仔细
检查其符合预期，然后获取 PTE 专属锁，*再* 次检查 PMD 表项仍符合预期的原因。)

If a THP collapse (or similar) were to occur then the lock on both pages would
be acquired, so we can ensure this is prevented while the PTE lock is held.
(如果发生 THP collapse(或类似情况)，两页的锁都会被获取，因此可以确保在持有 PTE
锁期间不会发生这种事。)

Installing entries this way ensures mutual exclusion on write.
(以这种方式安装表项可确保写操作互斥。)

Page table freeing(页表释放)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tearing down page tables themselves is something that requires significant
care. There must be no way that page tables designated for removal can be
traversed or referenced by concurrent tasks.
(拆除页表本身是需要格外小心的事情。绝不能让被指定要移除的页表有任何可能被并发任务
遍历或引用。)

It is insufficient to simply hold an mmap write lock and VMA lock (which will
prevent racing faults, and rmap operations), as a file-backed mapping can be
truncated under the :c:struct:`!struct address_space->i_mmap_rwsem` alone.
(仅仅持有 mmap 写锁和 VMA 锁(这能防止竞态缺页与 rmap 操作)并不足够，因为文件映射
可以仅在 :c:struct:`!struct address_space->i_mmap_rwsem` 之下就被截断。)

As a result, no VMA which can be accessed via the reverse mapping (either
through the :c:struct:`!struct anon_vma->rb_root` or the :c:member:`!struct
address_space->i_mmap` interval trees) can have its page tables torn down.
(因此，任何可以通过反向映射访问到的 VMA(无论是通过
:c:struct:`!struct anon_vma->rb_root` 还是 :c:member:`!struct
address_space->i_mmap` 区间树)都不能拆除其页表。)

The operation is typically performed via :c:func:`!free_pgtables`, which assumes
either the mmap write lock has been taken (as specified by its
:c:member:`!mm_wr_locked` parameter), or that the VMA is already unreachable.
(该操作通常通过 :c:func:`!free_pgtables` 完成，它假定要么已获取 mmap 写锁(如其
:c:member:`!mm_wr_locked` 参数所指定)，要么该 VMA 已不可达。)

It carefully removes the VMA from all reverse mappings, however it's important
that no new ones overlap these or any route remain to permit access to addresses
within the range whose page tables are being torn down.
(它会小心地把 VMA 从所有反向映射中移除；重要的是，不允许任何新 VMA 与之重叠，
也不允许存在任何仍能访问正在被拆除页表的范围内的地址的途径。)

Additionally, it assumes that a zap has already been performed and steps have
been taken to ensure that no further page table entries can be installed between
the zap and the invocation of :c:func:`!free_pgtables`.
(此外，它假定清空已经完成，并且已采取措施确保在清空与调用
:c:func:`!free_pgtables` 之间不会再安装任何页表表项。)

Since it is assumed that all such steps have been taken, page table entries are
cleared without page table locks (in the :c:func:`!pgd_clear`, :c:func:`!p4d_clear`,
:c:func:`!pud_clear`, and :c:func:`!pmd_clear` functions.
(由于假定所有这些步骤都已完成，页表表项的清除不再持有页表锁(见
:c:func:`!pgd_clear`、:c:func:`!p4d_clear`、:c:func:`!pud_clear` 与
:c:func:`!pmd_clear` 函数)。

.. note:: It is possible for leaf page tables to be torn down independent of
          the page tables above it as is done by
          :c:func:`!retract_page_tables`, which is performed under the i_mmap
          read lock, PMD, and PTE page table locks, without this level of care.
          (注意：叶子页表可以独立于其上层的页表被拆除，例如
          :c:func:`!retract_page_tables` 的做法——它在 i_mmap 读锁以及 PMD、PTE
          页表锁下执行，无需这种程度的保护。)

Page table moving(页表迁移)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some functions manipulate page table levels above PMD (that is PUD, P4D and PGD
page tables). Most notable of these is :c:func:`!mremap`, which is capable of
moving higher level page tables.
(有些函数会操作 PMD 之上的页表层级(即 PUD、P4D 和 PGD 页表)。其中最著名的是
:c:func:`!mremap`，它能够移动更高层级的页表。)

In these instances, it is required that **all** locks are taken, that is
the mmap lock, the VMA lock and the relevant rmap locks.
(在这些场景下，要求获取 **全部** 锁——即 mmap 锁、VMA 锁以及相关的 rmap 锁。)

You can observe this in the :c:func:`!mremap` implementation in the functions
:c:func:`!take_rmap_locks` and :c:func:`!drop_rmap_locks` which perform the rmap
side of lock acquisition, invoked ultimately by :c:func:`!move_page_tables`.
(你可以在 :c:func:`!mremap` 的实现中观察到这一点：:c:func:`!take_rmap_locks` 与
:c:func:`!drop_rmap_locks` 负责锁获取的 rmap 一侧，最终由
:c:func:`!move_page_tables` 调用。)

VMA lock internals(VMA 锁内部机制)
----------------------------------

Overview(概述)
^^^^^^^^^^^^^^

VMA read locking is entirely optimistic - if the lock is contended or a competing
write has started, then we do not obtain a read lock.
(VMA 读锁完全是乐观的——如果锁有争用或竞争性写已经开始，我们就不会获得读锁。)

A VMA **read** lock is obtained by :c:func:`!lock_vma_under_rcu`, which first
calls :c:func:`!rcu_read_lock` to ensure that the VMA is looked up in an RCU
critical section, then attempts to VMA lock it via :c:func:`!vma_start_read`,
before releasing the RCU lock via :c:func:`!rcu_read_unlock`.
(VMA **读** 锁通过 :c:func:`!lock_vma_under_rcu` 获取：它先调用
:c:func:`!rcu_read_lock` 以确保在 RCU 临界区内查找 VMA，然后尝试通过
:c:func:`!vma_start_read` 获取 VMA 锁，最后通过 :c:func:`!rcu_read_unlock` 释放
RCU 锁。)

In cases when the user already holds mmap read lock, :c:func:`!vma_start_read_locked`
and :c:func:`!vma_start_read_locked_nested` can be used. These functions do not
fail due to lock contention but the caller should still check their return values
in case they fail for other reasons.
(在调用者已持有 mmap 读锁的情况下，可以使用 :c:func:`!vma_start_read_locked` 与
:c:func:`!vma_start_read_locked_nested`。这两个函数不会因锁争用而失败，但调用者
仍应检查其返回值，以防它们因其他原因失败。)

VMA read locks increment :c:member:`!vma.vm_refcnt` reference counter for their
duration and the caller of :c:func:`!lock_vma_under_rcu` must drop it via
:c:func:`!vma_end_read`.
(VMA 读锁在其存续期间会递增 :c:member:`!vma.vm_refcnt` 引用计数，
:c:func:`!lock_vma_under_rcu` 的调用者必须通过 :c:func:`!vma_end_read` 将其
递减。)

VMA **write** locks are acquired via :c:func:`!vma_start_write` in instances where a
VMA is about to be modified, unlike :c:func:`!vma_start_read` the lock is always
acquired. An mmap write lock **must** be held for the duration of the VMA write
lock, releasing or downgrading the mmap write lock also releases the VMA write
lock so there is no :c:func:`!vma_end_write` function.
(VMA **写** 锁在 VMA 即将被修改时通过 :c:func:`!vma_start_write` 获取；与
:c:func:`!vma_start_read` 不同，这种锁总是能获取成功。在 VMA 写锁存续期间
**必须** 持有 mmap 写锁，释放或降级 mmap 写锁也会释放 VMA 写锁，因此不存在
:c:func:`!vma_end_write` 函数。)

Note that when write-locking a VMA lock, the :c:member:`!vma.vm_refcnt` is temporarily
modified so that readers can detect the presense of a writer. The reference counter is
restored once the vma sequence number used for serialisation is updated.
(注意，对 VMA 锁加写锁时会临时修改 :c:member:`!vma.vm_refcnt`，让读者能够察觉
写者的存在。一旦用于串行化的 VMA 序列号被更新，引用计数就会恢复。)

This ensures the semantics we require - VMA write locks provide exclusive write
access to the VMA.
(这就保证了我们所需要的语义——VMA 写锁提供对 VMA 的独占写访问。)

Implementation details(实现细节)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VMA lock mechanism is designed to be a lightweight means of avoiding the use
of the heavily contended mmap lock. It is implemented using a combination of a
reference counter and sequence numbers belonging to the containing
:c:struct:`!struct mm_struct` and the VMA.
(VMA 锁机制被设计为一种轻量级手段，用来避免使用争用严重的 mmap 锁。它由引用计数与
属于所属 :c:struct:`!struct mm_struct` 及 VMA 的序列号组合实现。)

Read locks are acquired via :c:func:`!vma_start_read`, which is an optimistic
operation, i.e. it tries to acquire a read lock but returns false if it is
unable to do so. At the end of the read operation, :c:func:`!vma_end_read` is
called to release the VMA read lock.
(读锁通过 :c:func:`!vma_start_read` 获取，这是一个乐观操作：它尝试获取读锁，若无法
获取则返回 false。读操作结束时调用 :c:func:`!vma_end_read` 释放 VMA 读锁。)

Invoking :c:func:`!vma_start_read` requires that :c:func:`!rcu_read_lock` has
been called first, establishing that we are in an RCU critical section upon VMA
read lock acquisition. Once acquired, the RCU lock can be released as it is only
required for lookup. This is abstracted by :c:func:`!lock_vma_under_rcu` which
is the interface a user should use.
(调用 :c:func:`!vma_start_read` 要求先调用 :c:func:`!rcu_read_lock`，以确保获取
VMA 读锁时处于 RCU 临界区内。一旦获取成功，RCU 锁即可释放，因为只有查找才需要它。
这一点由 :c:func:`!lock_vma_under_rcu` 抽象封装，用户应当使用该接口。)

Writing requires the mmap to be write-locked and the VMA lock to be acquired via
:c:func:`!vma_start_write`, however the write lock is released by the termination or
downgrade of the mmap write lock so no :c:func:`!vma_end_write` is required.
(写操作要求 mmap 处于写锁状态，并通过 :c:func:`!vma_start_write` 获取 VMA 锁；
不过写锁会随 mmap 写锁的释放或降级而释放，因此不需要 :c:func:`!vma_end_write`。)

All this is achieved by the use of per-mm and per-VMA sequence counts, which are
used in order to reduce complexity, especially for operations which write-lock
multiple VMAs at once.
(这一切都通过每 mm 与每 VMA 的序列计数实现，以降低复杂度，尤其是对一次写锁多个 VMA
的操作而言。)

If the mm sequence count, :c:member:`!mm->mm_lock_seq` is equal to the VMA
sequence count :c:member:`!vma->vm_lock_seq` then the VMA is write-locked. If
they differ, then it is not.
(如果 mm 的序列计数 :c:member:`!mm->mm_lock_seq` 等于 VMA 的序列计数
:c:member:`!vma->vm_lock_seq`，则该 VMA 处于写锁状态；若两者不同，则不是。)

Each time the mmap write lock is released in :c:func:`!mmap_write_unlock` or
:c:func:`!mmap_write_downgrade`, :c:func:`!vma_end_write_all` is invoked which
also increments :c:member:`!mm->mm_lock_seq` via
:c:func:`!mm_lock_seqcount_end`.
(每次在 :c:func:`!mmap_write_unlock` 或 :c:func:`!mmap_write_downgrade` 中释放
mmap 写锁时，都会调用 :c:func:`!vma_end_write_all`，它还会通过
:c:func:`!mm_lock_seqcount_end` 递增 :c:member:`!mm->mm_lock_seq`。)

This way, we ensure that, regardless of the VMA's sequence number, a write lock
is never incorrectly indicated and that when we release an mmap write lock we
efficiently release **all** VMA write locks contained within the mmap at the
same time.
(这样，无论 VMA 的序列号如何，我们都确保写锁永远不会被错误地指示，并且在释放 mmap
写锁时，同时高效地释放该 mmap 中包含的 **所有** VMA 写锁。)

Since the mmap write lock is exclusive against others who hold it, the automatic
release of any VMA locks on its release makes sense, as you would never want to
keep VMAs locked across entirely separate write operations. It also maintains
correct lock ordering.
(由于 mmap 写锁对其他持有者而言是排他的，它在释放时自动释放所有 VMA 锁是合理的：
你绝不会想让 VMA 的锁跨完全独立的写操作继续保持。这也维持了正确的锁顺序。)

Each time a VMA read lock is acquired, we increment :c:member:`!vma.vm_refcnt`
reference counter and check that the sequence count of the VMA does not match
that of the mm.
(每次获取 VMA 读锁时，我们递增 :c:member:`!vma.vm_refcnt` 引用计数，并检查 VMA 的
序列计数是否与 mm 的不一致。)

If it does, the read lock fails and :c:member:`!vma.vm_refcnt` is dropped.
If it does not, we keep the reference counter raised, excluding writers, but
permitting other readers, who can also obtain this lock under RCU.
(如果一致，读锁失败并递减 :c:member:`!vma.vm_refcnt`。如果不一致，我们保持引用
计数处于增加状态，把写者排除在外，但允许其他读者——他们同样可以在 RCU 下获得
此锁。)

Importantly, maple tree operations performed in :c:func:`!lock_vma_under_rcu`
are also RCU safe, so the whole read lock operation is guaranteed to function
correctly.
(重要的是，在 :c:func:`!lock_vma_under_rcu` 中执行的 maple tree 操作也是 RCU 安全
的，因此整个读锁操作保证能正确运行。)

On the write side, we set a bit in :c:member:`!vma.vm_refcnt` which can't be
modified by readers and wait for all readers to drop their reference count.
Once there are no readers, the VMA's sequence number is set to match that of
the mm. During this entire operation mmap write lock is held.
(在写一侧，我们在 :c:member:`!vma.vm_refcnt` 中设置一个读者无法修改的位，并等待
所有读者递减其引用计数。一旦没有读者，就把 VMA 的序列号设置为与 mm 一致。整个
过程中始终持有 mmap 写锁。)

This way, if any read locks are in effect, :c:func:`!vma_start_write` will sleep
until these are finished and mutual exclusion is achieved.
(这样，如果还有任何读锁在生效，:c:func:`!vma_start_write` 就会休眠，直到它们结束
并达成互斥。)

After setting the VMA's sequence number, the bit in :c:member:`!vma.vm_refcnt`
indicating a writer is cleared. From this point on, VMA's sequence number will
indicate VMA's write-locked state until mmap write lock is dropped or downgraded.
(设置 VMA 的序列号之后，:c:member:`!vma.vm_refcnt` 中指示写者的位即被清除。从此刻
起，VMA 的序列号将指示 VMA 的写锁状态，直到 mmap 写锁被释放或降级。)

This clever combination of a reference counter and sequence count allows for
fast RCU-based per-VMA lock acquisition (especially on page fault, though
utilised elsewhere) with minimal complexity around lock ordering.
(引用计数与序列计数的巧妙组合，实现了基于 RCU 的快速按 VMA 锁获取(尤其是在缺页
异常中，不过其他场景也在使用)，同时把锁顺序方面的复杂度降到最低。)

mmap write lock downgrading(mmap 写锁降级)
------------------------------------------

When an mmap write lock is held one has exclusive access to resources within the
mmap (with the usual caveats about requiring VMA write locks to avoid races with
tasks holding VMA read locks).
(持有 mmap 写锁时，你对 mmap 内的资源拥有独占访问(通常仍需注意：需要 VMA 写锁以
避免与持有 VMA 读锁的任务发生竞态)。)

It is then possible to **downgrade** from a write lock to a read lock via
:c:func:`!mmap_write_downgrade` which, similar to :c:func:`!mmap_write_unlock`,
implicitly terminates all VMA write locks via :c:func:`!vma_end_write_all`, but
importantly does not relinquish the mmap lock while downgrading, therefore
keeping the locked virtual address space stable.
(随后可以通过 :c:func:`!mmap_write_downgrade` 把写锁 **降级** 为读锁。它类似于
:c:func:`!mmap_write_unlock`，会通过 :c:func:`!vma_end_write_all` 隐式终结所有
VMA 写锁，但重要的是它在降级时并不放弃 mmap 锁，因此保持被锁定的虚拟地址空间
稳定。)

An interesting consequence of this is that downgraded locks are exclusive
against any other task possessing a downgraded lock (since a racing task would
have to acquire a write lock first to downgrade it, and the downgraded lock
prevents a new write lock from being obtained until the original lock is
released).
(一个有趣的推论是：降级后的锁对任何其他持有降级锁的任务都是排他的(因为竞争任务
必须先获取写锁才能降级，而降级锁会阻止获取新的写锁，直到原锁被释放)。)

For clarity, we map read (R)/downgraded write (D)/write (W) locks against one
another showing which locks exclude the others:
(为清晰起见，我们把读 (R)/降级写 (D)/写 (W) 锁两两对照，标明哪些锁相互排斥：)

.. list-table:: Lock exclusivity
   :widths: 5 5 5 5
   :header-rows: 1
   :stub-columns: 1

   * -
     - R
     - D
     - W
   * - R
     - N
     - N
     - Y
   * - D
     - N
     - Y
     - Y
   * - W
     - Y
     - Y
     - Y

Here a Y indicates the locks in the matching row/column are mutually exclusive,
and N indicates that they are not.
(表中 Y 表示对应行/列的锁相互排斥，N 表示不排斥。)

Stack expansion(栈扩展)
-----------------------

Stack expansion throws up additional complexities in that we cannot permit there
to be racing page faults, as a result we invoke :c:func:`!vma_start_write` to
prevent this in :c:func:`!expand_downwards` or :c:func:`!expand_upwards`.
(栈扩展带来了额外的复杂性：我们不允许出现竞争的缺页异常，因此在
:c:func:`!expand_downwards` 与 :c:func:`!expand_upwards` 中调用
:c:func:`!vma_start_write` 来防止这种情况。)

--------------------------------------
Functions and structures(函数与结构体)
--------------------------------------

.. kernel-doc:: include/linux/mmap_lock.h
