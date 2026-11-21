When do you need to notify inside page table lock ?(什么时候需要在页表锁内通知？)
=================================================================================

When clearing a pte/pmd we are given a choice to notify the event through
(notify version of \*_clear_flush call mmu_notifier_invalidate_range) under
the page table lock. But that notification is not necessary in all cases.
(在清除 pte/pmd 时，我们可以选择在页表锁下通过(notify 版本的
\*_clear_flush 调用，即 mmu_notifier_invalidate_range)来通知该事件。
但这种通知并非在所有情况下都是必需的。)

For secondary TLB (non CPU TLB) like IOMMU TLB or device TLB (when device use
thing like ATS/PASID to get the IOMMU to walk the CPU page table to access a
process virtual address space). There is only 2 cases when you need to notify
those secondary TLB while holding page table lock when clearing a pte/pmd:
(对于诸如 IOMMU TLB 或设备 TLB 之类的次级 TLB(非 CPU TLB)(当设备
使用 ATS/PASID 之类的机制让 IOMMU 遍历 CPU 页表以访问进程虚拟地址
空间时)。在清除 pte/pmd 时，只有两种情况需要在持有页表锁的同时通知
这些次级 TLB：)

  A) page backing address is free before mmu_notifier_invalidate_range_end()
     (A) 地址背后的页面在 mmu_notifier_invalidate_range_end() 之前被
     释放)
  B) a page table entry is updated to point to a new page (COW, write fault
     on zero page, __replace_page(), ...)
     (B) 页表项被更新为指向一个新页面(COW、对零页的写故障、
     __replace_page() 等))

Case A is obvious you do not want to take the risk for the device to write to
a page that might now be used by some completely different task.
(情况 A 很明显：你不想冒这样的风险——设备写入一个现在可能已被某个
完全不同任务使用的页面。)

Case B is more subtle. For correctness it requires the following sequence to
happen:
(情况 B 更微妙。为保证正确性，它要求以下顺序发生：)

  - take page table lock
    (获取页表锁)
  - clear page table entry and notify ([pmd/pte]p_huge_clear_flush_notify())
    (清除页表项并通知([pmd/pte]p_huge_clear_flush_notify()))
  - set page table entry to point to new page
    (把页表项设置为指向新页面)

If clearing the page table entry is not followed by a notify before setting
the new pte/pmd value then you can break memory model like C11 or C++11 for
the device.
(如果在设置新的 pte/pmd 值之前，清除页表项的操作没有伴随一次通知，
那么对于设备而言，你可能会破坏像 C11 或 C++11 这样的内存模型。)

Consider the following scenario (device use a feature similar to ATS/PASID):
(考虑以下场景(设备使用类似 ATS/PASID 的功能)：)

Two address addrA and addrB such that \|addrA - addrB\| >= PAGE_SIZE we assume
they are write protected for COW (other case of B apply too).

::

 [Time N] --------------------------------------------------------------------
 CPU-thread-0  {try to write to addrA}
 CPU-thread-1  {try to write to addrB}
 CPU-thread-2  {}
 CPU-thread-3  {}
 DEV-thread-0  {read addrA and populate device TLB}
 DEV-thread-2  {read addrB and populate device TLB}
 [Time N+1] ------------------------------------------------------------------
 CPU-thread-0  {COW_step0: {mmu_notifier_invalidate_range_start(addrA)}}
 CPU-thread-1  {COW_step0: {mmu_notifier_invalidate_range_start(addrB)}}
 CPU-thread-2  {}
 CPU-thread-3  {}
 DEV-thread-0  {}
 DEV-thread-2  {}
 [Time N+2] ------------------------------------------------------------------
 CPU-thread-0  {COW_step1: {update page table to point to new page for addrA}}
 CPU-thread-1  {COW_step1: {update page table to point to new page for addrB}}
 CPU-thread-2  {}
 CPU-thread-3  {}
 DEV-thread-0  {}
 DEV-thread-2  {}
 [Time N+3] ------------------------------------------------------------------
 CPU-thread-0  {preempted}
 CPU-thread-1  {preempted}
 CPU-thread-2  {write to addrA which is a write to new page}
 CPU-thread-3  {}
 DEV-thread-0  {}
 DEV-thread-2  {}
 [Time N+3] ------------------------------------------------------------------
 CPU-thread-0  {preempted}
 CPU-thread-1  {preempted}
 CPU-thread-2  {}
 CPU-thread-3  {write to addrB which is a write to new page}
 DEV-thread-0  {}
 DEV-thread-2  {}
 [Time N+4] ------------------------------------------------------------------
 CPU-thread-0  {preempted}
 CPU-thread-1  {COW_step3: {mmu_notifier_invalidate_range_end(addrB)}}
 CPU-thread-2  {}
 CPU-thread-3  {}
 DEV-thread-0  {}
 DEV-thread-2  {}
 [Time N+5] ------------------------------------------------------------------
 CPU-thread-0  {preempted}
 CPU-thread-1  {}
 CPU-thread-2  {}
 CPU-thread-3  {}
 DEV-thread-0  {read addrA from old page}
 DEV-thread-2  {read addrB from new page}

(两个地址 addrA 和 addrB 满足 \|addrA - addrB\| >= PAGE_SIZE，我们
假设它们因 COW 而被写保护(B 的其他情况同样适用)。)

So here because at time N+2 the clear page table entry was not pair with a
notification to invalidate the secondary TLB, the device see the new value for
addrB before seeing the new value for addrA. This break total memory ordering
for the device.
(所以这里，由于在时刻 N+2，清除页表项的操作没有与一次使次级 TLB
失效的通知配对，设备在看到 addrA 的新值之前先看到了 addrB 的新值。
这破坏了设备的全序内存序(total memory ordering)。)

When changing a pte to write protect or to point to a new write protected page
with same content (KSM) it is fine to delay the mmu_notifier_invalidate_range
call to mmu_notifier_invalidate_range_end() outside the page table lock. This
is true even if the thread doing the page table update is preempted right after
releasing page table lock but before call mmu_notifier_invalidate_range_end().
(当把一个 pte 改为写保护，或指向一个内容相同但写保护的新页面(KSM)
时，把 mmu_notifier_invalidate_range 调用延迟到页表锁之外的
mmu_notifier_invalidate_range_end() 是可以的。即使执行页表更新的线程
在释放页表锁之后、调用 mmu_notifier_invalidate_range_end() 之前恰好
被抢占，也是如此。)
