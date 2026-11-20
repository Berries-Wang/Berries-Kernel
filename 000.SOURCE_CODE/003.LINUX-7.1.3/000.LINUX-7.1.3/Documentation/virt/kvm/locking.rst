.. SPDX-License-Identifier: GPL-2.0

=============================
KVM Lock Overview(KVM 锁概览)
=============================

1. Acquisition Orders(加锁顺序)
-------------------------------

The acquisition orders for mutexes are as follows:
(互斥锁的获取顺序如下：)

- cpus_read_lock() is taken outside kvm_lock
  (cpus_read_lock() 在 kvm_lock 之外获取)

- kvm_usage_lock is taken outside cpus_read_lock()
  (kvm_usage_lock 在 cpus_read_lock() 之外获取)

- kvm->lock is taken outside vcpu->mutex
  (kvm->lock 在 vcpu->mutex 之外获取)

- kvm->lock is taken outside kvm->slots_lock and kvm->irq_lock
  (kvm->lock 在 kvm->slots_lock 与 kvm->irq_lock 之外获取)

- vcpu->mutex is taken outside kvm->slots_lock and kvm->slots_arch_lock
  (vcpu->mutex 在 kvm->slots_lock 与 kvm->slots_arch_lock 之外获取)

- kvm->slots_lock is taken outside kvm->irq_lock, though acquiring
  them together is quite rare.
  (kvm->slots_lock 在 kvm->irq_lock 之外获取，不过同时获取两者的情况相当少见。)

- kvm->mn_active_invalidate_count ensures that pairs of
  invalidate_range_start() and invalidate_range_end() callbacks
  use the same memslots array.  kvm->slots_lock and kvm->slots_arch_lock
  are taken on the waiting side when modifying memslots, so MMU notifiers
  must not take either kvm->slots_lock or kvm->slots_arch_lock.
  (kvm->mn_active_invalidate_count 确保成对的 invalidate_range_start() 与 invalidate_range_end() 回调使用同一个 memslots 数组。修改 memslots 时，等待方会持有 kvm->slots_lock 与 kvm->slots_arch_lock，因此 MMU 通知器绝不能获取 kvm->slots_lock 或 kvm->slots_arch_lock 中的任何一个。)

cpus_read_lock() vs kvm_lock:
(cpus_read_lock() 与 kvm_lock 的关系：)

- Taking cpus_read_lock() outside of kvm_lock is problematic, despite that
  being the official ordering, as it is quite easy to unknowingly trigger
  cpus_read_lock() while holding kvm_lock.  Use caution when walking vm_list,
  e.g. avoid complex operations when possible.
  (在 kvm_lock 之外获取 cpus_read_lock() 是有问题的——尽管这是官方规定的顺序——因为在持有 kvm_lock 时很容易在不知不觉间触发 cpus_read_lock()。遍历 vm_list 时要小心，例如尽量避开复杂的操作。)

For SRCU:
(对于 SRCU：)

- ``synchronize_srcu(&kvm->srcu)`` is called inside critical sections
  for kvm->lock, vcpu->mutex and kvm->slots_lock.  These locks _cannot_
  be taken inside a kvm->srcu read-side critical section; that is, the
  following is broken::

      srcu_read_lock(&kvm->srcu);
      mutex_lock(&kvm->slots_lock);

  (``synchronize_srcu(&kvm->srcu)`` 会在 kvm->lock、vcpu->mutex 与 kvm->slots_lock 的临界区内被调用。这些锁不能在 kvm->srcu 读侧临界区内获取；也就是说，上面的用法是错误的。)

- kvm->slots_arch_lock instead is released before the call to
  ``synchronize_srcu()``.  It _can_ therefore be taken inside a
  kvm->srcu read-side critical section, for example while processing
  a vmexit.
  (而 kvm->slots_arch_lock 则在调用 ``synchronize_srcu()`` 之前释放。因此它可以在 kvm->srcu 读侧临界区内获取，例如在处理 vmexit 时。)

On x86:
(在 x86 上：)

- vcpu->mutex is taken outside kvm->arch.hyperv.hv_lock and kvm->arch.xen.xen_lock
  (vcpu->mutex 在 kvm->arch.hyperv.hv_lock 与 kvm->arch.xen.xen_lock 之外获取)

- kvm->arch.mmu_lock is an rwlock; critical sections for
  kvm->arch.tdp_mmu_pages_lock and kvm->arch.mmu_unsync_pages_lock must
  also take kvm->arch.mmu_lock
  (kvm->arch.mmu_lock 是一个 rwlock；kvm->arch.tdp_mmu_pages_lock 与 kvm->arch.mmu_unsync_pages_lock 的临界区还必须获取 kvm->arch.mmu_lock)

Everything else is a leaf: no other lock is taken inside the critical
sections.
(其余的都是叶锁：临界区内不获取任何其他锁。)

2. Exception(例外)
------------------

Fast page fault:
(快速页错误（fast page fault）：)

Fast page fault is the fast path which fixes the guest page fault out of
the mmu-lock on x86. Currently, the page fault can be fast in one of the
following two cases:
(快速页错误是 x86 上在 mmu-lock 之外修复客户机页错误的快速路径。目前，页错误在以下两种情况之一中可以走快速路径：)

1. Access Tracking: The SPTE is not present, but it is marked for access
   tracking. That means we need to restore the saved R/X bits. This is
   described in more detail later below.
   (1. 访问跟踪（Access Tracking）：SPTE 不存在，但被标记为访问跟踪。这意味着我们需要恢复保存的 R/X 位。下文会更详细地描述。)

2. Write-Protection: The SPTE is present and the fault is caused by
   write-protect. That means we just need to change the W bit of the spte.
   (2. 写保护（Write-Protection）：SPTE 存在，且错误由写保护引起。这意味着我们只需要修改 spte 的 W 位。)

What we use to avoid all the races is the Host-writable bit and MMU-writable bit
on the spte:
(我们用来避免所有竞态的，是 spte 上的 Host-writable 位与 MMU-writable 位：)

- Host-writable means the gfn is writable in the host kernel page tables and in
  its KVM memslot.
  (Host-writable 表示 gfn 在宿主机内核页表及其 KVM memslot 中是可写的。)
- MMU-writable means the gfn is writable in the guest's mmu and it is not
  write-protected by shadow page write-protection.
  (MMU-writable 表示 gfn 在客户机 mmu 中是可写的，且未被影子页写保护机制写保护。)

On fast page fault path, we will use cmpxchg to atomically set the spte W
bit if spte.HOST_WRITEABLE = 1 and spte.WRITE_PROTECT = 1, to restore the saved
R/X bits if for an access-traced spte, or both. This is safe because whenever
changing these bits can be detected by cmpxchg.
(在快速页错误路径上，如果 spte.HOST_WRITEABLE = 1 且 spte.WRITE_PROTECT = 1，我们会用 cmpxchg 原子地置位 spte 的 W 位；对被访问跟踪的 spte 则恢复保存的 R/X 位；或者两者兼而有之。这是安全的，因为对这些位的任何修改都能被 cmpxchg 检测到。)

But we need carefully check these cases:
(但我们需要仔细检查以下情况：)

1) The mapping from gfn to pfn
(1) gfn 到 pfn 的映射)

The mapping from gfn to pfn may be changed since we can only ensure the pfn
is not changed during cmpxchg. This is a ABA problem, for example, below case
will happen:
(gfn 到 pfn 的映射可能改变，因为我们只能保证 pfn 在 cmpxchg 期间不变。这是一个 ABA 问题，例如下面这种情况就可能发生：)

+------------------------------------------------------------------------+
| At the beginning::                                                     |
|                                                                        |
|	gpte = gfn1                                                      |
|	gfn1 is mapped to pfn1 on host                                   |
|	spte is the shadow page table entry corresponding with gpte and  |
|	spte = pfn1                                                      |
+------------------------------------------------------------------------+
| On fast page fault path:                                               |
+------------------------------------+-----------------------------------+
| CPU 0:                             | CPU 1:                            |
+------------------------------------+-----------------------------------+
| ::                                 |                                   |
|                                    |                                   |
|   old_spte = *spte;                |                                   |
+------------------------------------+-----------------------------------+
|                                    | pfn1 is swapped out::             |
|                                    |                                   |
|                                    |    spte = 0;                      |
|                                    |                                   |
|                                    | pfn1 is re-alloced for gfn2.      |
|                                    |                                   |
|                                    | gpte is changed to point to       |
|                                    | gfn2 by the guest::               |
|                                    |                                   |
|                                    |    spte = pfn1;                   |
+------------------------------------+-----------------------------------+
| ::                                                                     |
|                                                                        |
|   if (cmpxchg(spte, old_spte, old_spte+W)                              |
|	mark_page_dirty(vcpu->kvm, gfn1)                                 |
|            OOPS!!!                                                     |
+------------------------------------------------------------------------+

We dirty-log for gfn1, that means gfn2 is lost in dirty-bitmap.
(我们为 gfn1 记录了脏日志，这意味着 gfn2 在脏位图中丢失了。)

For direct sp, we can easily avoid it since the spte of direct sp is fixed
to gfn.  For indirect sp, we disabled fast page fault for simplicity.
(对 direct sp 而言，其 spte 与 gfn 一一固定，可以轻松避免此问题。对 indirect sp，为简单起见我们禁用了快速页错误。)

A solution for indirect sp could be to pin the gfn before the cmpxchg.  After
the pinning:
(对 indirect sp 的一个解决方案是在 cmpxchg 之前固定（pin）gfn。固定之后：)

- We have held the refcount of pfn; that means the pfn can not be freed and
  be reused for another gfn.
  (我们已经持有了 pfn 的引用计数；这意味着 pfn 不会被释放并被复用于另一个 gfn。)
- The pfn is writable and therefore it cannot be shared between different gfns
  by KSM.
  (该 pfn 是可写的，因此不会被 KSM 在不同 gfn 之间共享。)

Then, we can ensure the dirty bitmaps is correctly set for a gfn.
(这样，我们就能确保每个 gfn 的脏位图都被正确设置。)

2) Dirty bit tracking
(2) 脏位跟踪)

In the original code, the spte can be fast updated (non-atomically) if the
spte is read-only and the Accessed bit has already been set since the
Accessed bit and Dirty bit can not be lost.
(在最初的代码中，如果 spte 是只读的且 Accessed 位已置位，spte 就可以被快速更新（非原子地），因为 Accessed 位与 Dirty 位不会丢失。)

But it is not true after fast page fault since the spte can be marked
writable between reading spte and updating spte. Like below case:
(但在快速页错误引入之后这不再成立，因为 spte 可能在读取 spte 与更新 spte 之间被标记为可写。如下面这种情况：)

+-------------------------------------------------------------------------+
| At the beginning::                                                      |
|                                                                         |
|  spte.W = 0                                                             |
|  spte.Accessed = 1                                                      |
+-------------------------------------+-----------------------------------+
| CPU 0:                              | CPU 1:                            |
+-------------------------------------+-----------------------------------+
| In mmu_spte_update()::              |                                   |
|                                     |                                   |
|  old_spte = *spte;                  |                                   |
|                                     |                                   |
|                                     |                                   |
|  /* 'if' condition is satisfied. */ |                                   |
|  if (old_spte.Accessed == 1 &&      |                                   |
|       old_spte.W == 0)              |                                   |
|     spte = new_spte;                |                                   |
+-------------------------------------+-----------------------------------+
|                                     | on fast page fault path::         |
|                                     |                                   |
|                                     |    spte.W = 1                     |
|                                     |                                   |
|                                     | memory write on the spte::        |
|                                     |                                   |
|                                     |    spte.Dirty = 1                 |
+-------------------------------------+-----------------------------------+
|  ::                                 |                                   |
|                                     |                                   |
|   else                              |                                   |
|     old_spte = xchg(spte, new_spte);|                                   |
|   if (old_spte.Accessed &&          |                                   |
|       !new_spte.Accessed)           |                                   |
|     flush = true;                   |                                   |
|   if (old_spte.Dirty &&             |                                   |
|       !new_spte.Dirty)              |                                   |
|     flush = true;                   |                                   |
|     OOPS!!!                         |                                   |
+-------------------------------------+-----------------------------------+

The Dirty bit is lost in this case.
(这种情况下 Dirty 位丢失了。)

In order to avoid this kind of issue, we always treat the spte as "volatile"
if it can be updated out of mmu-lock [see spte_needs_atomic_update()]; it means
the spte is always atomically updated in this case.
(为避免这类问题，如果 spte 可能脱离 mmu-lock 被更新，我们总是把它当作"易变的"（volatile）[见 spte_needs_atomic_update()]；这意味着这种情况下 spte 总是被原子地更新。)

3) flush tlbs due to spte updated
(3) 因 spte 更新而刷新 TLB)

If the spte is updated from writable to read-only, we should flush all TLBs,
otherwise rmap_write_protect will find a read-only spte, even though the
writable spte might be cached on a CPU's TLB.
(如果 spte 从可写更新为只读，我们应当刷新所有 TLB，否则 rmap_write_protect 会找到一个只读 spte，而可写的 spte 可能还缓存在某个 CPU 的 TLB 中。)

As mentioned before, the spte can be updated to writable out of mmu-lock on
fast page fault path. In order to easily audit the path, we see if TLBs needing
to be flushed caused this reason in mmu_spte_update() since this is a common
function to update spte (present -> present).
(如前所述，spte 可以在快速页错误路径上脱离 mmu-lock 更新为可写。为了便于审计该路径，我们在 mmu_spte_update() 中检查是否存在因该原因而需要刷新的 TLB——因为它是更新 spte（present -> present）的公共函数。)

Since the spte is "volatile" if it can be updated out of mmu-lock, we always
atomically update the spte and the race caused by fast page fault can be avoided.
See the comments in spte_needs_atomic_update() and mmu_spte_update().
(由于 spte 在可能脱离 mmu-lock 更新时是"易变的"，我们总是原子地更新 spte，快速页错误引起的竞态因此得以避免。参见 spte_needs_atomic_update() 与 mmu_spte_update() 中的注释。)

Lockless Access Tracking:
(无锁访问跟踪：)

This is used for Intel CPUs that are using EPT but do not support the EPT A/D
bits. In this case, PTEs are tagged as A/D disabled (using ignored bits), and
when the KVM MMU notifier is called to track accesses to a page (via
kvm_mmu_notifier_clear_flush_young), it marks the PTE not-present in hardware
by clearing the RWX bits in the PTE and storing the original R & X bits in more
unused/ignored bits. When the VM tries to access the page later on, a fault is
generated and the fast page fault mechanism described above is used to
atomically restore the PTE to a Present state. The W bit is not saved when the
PTE is marked for access tracking and during restoration to the Present state,
the W bit is set depending on whether or not it was a write access. If it
wasn't, then the W bit will remain clear until a write access happens, at which
time it will be set using the Dirty tracking mechanism described above.
(这用于使用 EPT 但不支持 EPT A/D 位的 Intel CPU。这种情况下，PTE 被标记为 A/D 禁用（利用忽略位）；当 KVM MMU 通知器被调用以跟踪对某页的访问时（通过 kvm_mmu_notifier_clear_flush_young），它会清除 PTE 中的 RWX 位使 PTE 在硬件中呈现为不存在，并把原始的 R 与 X 位存入更多未使用/忽略的位中。当 VM 稍后试图访问该页时会产生 fault，并使用上文描述的快速页错误机制把 PTE 原子地恢复到 Present 状态。PTE 被标记为访问跟踪时不保存 W 位；恢复到 Present 状态时，W 位根据这是否为写访问来设置。如果不是，W 位将保持清除，直到发生写访问——届时再通过上述脏位跟踪机制置位。)

3. Reference(参考)
------------------

``kvm_lock`` (KVM 全局锁)
^^^^^^^^^^^^^^^^^^^^^^^^^

:Type:		mutex
(类型： mutex)
:Arch:		any
(架构： 任意)
:Protects:	- vm_list
(保护对象： vm_list)

``kvm_usage_lock`` (使用计数锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:Type:		mutex
(类型： mutex)
:Arch:		any
(架构： 任意)
:Protects:	- kvm_usage_count
		- hardware virtualization enable/disable
(保护对象： - kvm_usage_count；- 硬件虚拟化的启用/禁用)
:Comment:	Exists to allow taking cpus_read_lock() while kvm_usage_count is
		protected, which simplifies the virtualization enabling logic.
(说明： 它的存在是为了允许在 kvm_usage_count 受保护的同时获取 cpus_read_lock()，从而简化启用虚拟化的逻辑。)

``kvm->mn_invalidate_lock`` (MMU 通知器失效锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:Type:          spinlock_t
(类型： spinlock_t)
:Arch:          any
(架构： 任意)
:Protects:      mn_active_invalidate_count, mn_memslots_update_rcuwait
(保护对象： mn_active_invalidate_count、mn_memslots_update_rcuwait)

``kvm_arch::tsc_write_lock`` (TSC 写锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:Type:		raw_spinlock_t
(类型： raw_spinlock_t)
:Arch:		x86
(架构： x86)
:Protects:	- kvm_arch::{last_tsc_write,last_tsc_nsec,last_tsc_offset}
		- tsc offset in vmcb
(保护对象： - kvm_arch::{last_tsc_write,last_tsc_nsec,last_tsc_offset}；- vmcb 中的 tsc 偏移)
:Comment:	'raw' because updating the tsc offsets must not be preempted.
(说明： 用 'raw' 是因为更新 tsc 偏移绝不能被抢占。)

``kvm->mmu_lock`` (MMU 锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^
:Type:		spinlock_t or rwlock_t
(类型： spinlock_t 或 rwlock_t)
:Arch:		any
(架构： 任意)
:Protects:	-shadow page/shadow tlb entry
(保护对象： 影子页/影子 TLB 表项)
:Comment:	it is a spinlock since it is used in mmu notifier.
(说明： 它是自旋锁，因为它在 mmu 通知器中使用。)

``kvm->srcu`` (SRCU 锁)
^^^^^^^^^^^^^^^^^^^^^^^
:Type:		srcu lock
(类型： srcu 锁)
:Arch:		any
(架构： 任意)
:Protects:	- kvm->memslots
		- kvm->buses
(保护对象： - kvm->memslots；- kvm->buses)
:Comment:	The srcu read lock must be held while accessing memslots (e.g.
		when using gfn_to_* functions) and while accessing in-kernel
		MMIO/PIO address->device structure mapping (kvm->buses).
		The srcu index can be stored in kvm_vcpu->srcu_idx per vcpu
		if it is needed by multiple functions.
(说明： 访问 memslots（例如使用 gfn_to_* 函数）时，以及访问内核内 MMIO/PIO 地址->设备结构映射（kvm->buses）时，必须持有 srcu 读锁。如果多个函数都需要，srcu 索引可以按 vcpu 存储在 kvm_vcpu->srcu_idx 中。)

``kvm->slots_arch_lock`` (槽架构锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
:Type:          mutex
(类型： mutex)
:Arch:          any (only needed on x86 though)
(架构： 任意（不过仅在 x86 上需要）)
:Protects:      any arch-specific fields of memslots that have to be modified
                in a ``kvm->srcu`` read-side critical section.
(保护对象： 必须在 ``kvm->srcu`` 读侧临界区内修改的 memslots 的任何架构相关字段。)
:Comment:       must be held before reading the pointer to the current memslots,
                until after all changes to the memslots are complete
(说明： 必须在读取当前 memslots 指针之前持有，直到对 memslots 的全部修改完成之后。)

``wakeup_vcpus_on_cpu_lock`` (唤醒 vCPU 锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
:Type:		spinlock_t
(类型： spinlock_t)
:Arch:		x86
(架构： x86)
:Protects:	wakeup_vcpus_on_cpu
(保护对象： wakeup_vcpus_on_cpu)
:Comment:	This is a per-CPU lock and it is used for VT-d posted-interrupts.
		When VT-d posted-interrupts are supported and the VM has assigned
		devices, we put the blocked vCPU on the list blocked_vcpu_on_cpu
		protected by blocked_vcpu_on_cpu_lock. When VT-d hardware issues
		wakeup notification event since external interrupts from the
		assigned devices happens, we will find the vCPU on the list to
		wakeup.
(说明： 这是每 CPU 锁，用于 VT-d posted-interrupt。当支持 VT-d posted-interrupt 且 VM 有已分配的设备时，我们把被阻塞的 vCPU 放到由 blocked_vcpu_on_cpu_lock 保护的 blocked_vcpu_on_cpu 列表上。当已分配设备产生外部中断、VT-d 硬件发出唤醒通知事件时，我们会在列表中找到该 vCPU 并唤醒它。)

``vendor_module_lock`` (厂商模块锁)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
:Type:		mutex
(类型： mutex)
:Arch:		x86
(架构： x86)
:Protects:	loading a vendor module (kvm_amd or kvm_intel)
(保护对象： 加载厂商模块（kvm_amd 或 kvm_intel）)
:Comment:	Exists because using kvm_lock leads to deadlock.  kvm_lock is taken
    in notifiers, e.g. __kvmclock_cpufreq_notifier(), that may be invoked while
    cpu_hotplug_lock is held, e.g. from cpufreq_boost_trigger_state(), and many
    operations need to take cpu_hotplug_lock when loading a vendor module, e.g.
    updating static calls.
(说明： 存在的原因是使用 kvm_lock 会导致死锁。kvm_lock 会在通知器（例如 __kvmclock_cpufreq_notifier()）中被获取，而这些通知器可能在持有 cpu_hotplug_lock 期间被调用（例如来自 cpufreq_boost_trigger_state()）；而加载厂商模块时的许多操作又需要获取 cpu_hotplug_lock，例如更新静态调用（static calls）。)
