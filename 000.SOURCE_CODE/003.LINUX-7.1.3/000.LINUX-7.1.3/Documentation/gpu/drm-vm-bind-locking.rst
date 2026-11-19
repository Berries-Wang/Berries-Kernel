.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

=============================
VM_BIND locking(VM_BIND 加锁)
=============================

This document attempts to describe what's needed to get VM_BIND locking right,
including the userptr mmu_notifier locking. It also discusses some
optimizations to get rid of the looping through of all userptr mappings and
external / shared object mappings that is needed in the simplest
implementation. In addition, there is a section describing the VM_BIND locking
required for implementing recoverable pagefaults.
(本文试图描述把 VM_BIND 加锁做对需要什么，包括 userptr mmu_notifier 加锁。
文中还讨论了一些优化手段，用以摆脱最简单实现中所需要的、对所有 userptr 映
射和外部/共享对象映射的循环遍历。此外，还有一节描述实现可恢复缺页异常
(recoverable pagefaults)所需的 VM_BIND 加锁。)

The DRM GPUVM set of helpers(DRM GPUVM 辅助函数集)
==================================================

There is a set of helpers for drivers implementing VM_BIND, and this
set of helpers implements much, but not all of the locking described
in this document. In particular, it is currently lacking a userptr
implementation. This document does not intend to describe the DRM GPUVM
implementation in detail, but it is covered in :ref:`its own
documentation <drm_gpuvm>`. It is highly recommended for any driver
implementing VM_BIND to use the DRM GPUVM helpers and to extend it if
common functionality is missing.
(为实现 VM_BIND 的驱动提供了一组辅助函数，这组辅助函数实现了本文所述加锁
的大部分但并非全部。特别是，它目前还缺少 userptr 实现。本文不打算详细描述
DRM GPUVM 的实现，相关内容见 :ref:`its own documentation <drm_gpuvm>`。强
烈建议任何实现 VM_BIND 的驱动使用 DRM GPUVM 辅助函数，并在缺少通用功能时
对其进行扩展。)

Nomenclature(术语)
==================

* ``gpu_vm``: Abstraction of a virtual GPU address space with
  meta-data. Typically one per client (DRM file-private), or one per
  execution context.
  (``gpu_vm`` ：带元数据的虚拟 GPU 地址空间的抽象。通常每个 client(DRM
  file-private)一个，或每个执行上下文一个。)
* ``gpu_vma``: Abstraction of a GPU address range within a gpu_vm with
  associated meta-data. The backing storage of a gpu_vma can either be
  a GEM object or anonymous or page-cache pages mapped also into the CPU
  address space for the process.
  (``gpu_vma`` ：gpu_vm 内一个 GPU 地址范围及其关联元数据的抽象。
  gpu_vma 的后备存储要么是 GEM 对象，要么是同时映射到该进程 CPU 地址空间
  的匿名页或 page-cache 页。)
* ``gpu_vm_bo``: Abstracts the association of a GEM object and
  a VM. The GEM object maintains a list of gpu_vm_bos, where each gpu_vm_bo
  maintains a list of gpu_vmas.
  (``gpu_vm_bo`` ：抽象 GEM 对象与 VM 之间的关联。GEM 对象维护一个
  gpu_vm_bo 链表，而每个 gpu_vm_bo 又维护一个 gpu_vma 链表。)
* ``userptr gpu_vma or just userptr``: A gpu_vma, whose backing store
  is anonymous or page-cache pages as described above.
  (``userptr gpu_vma or just userptr`` ：后备存储为如上所述匿名页或
  page-cache 页的 gpu_vma。)
* ``revalidating``: Revalidating a gpu_vma means making the latest version
  of the backing store resident and making sure the gpu_vma's
  page-table entries point to that backing store.
  (``revalidating`` ：重新验证 (revalidate) 一个 gpu_vma，意思是让后备存
  储的最新版本驻留 (resident)，并确保 gpu_vma 的页表项指向该后备存储。)
* ``dma_fence``: A struct dma_fence that is similar to a struct completion
  and which tracks GPU activity. When the GPU activity is finished,
  the dma_fence signals. Please refer to the ``DMA Fences`` section of
  the :doc:`dma-buf doc </driver-api/dma-buf>`.
  (``dma_fence`` ：一种 struct dma_fence，类似 struct completion，用于跟
  踪 GPU 活动。GPU 活动结束时 dma_fence 会发出信号。请参阅
  :doc:`dma-buf doc </driver-api/dma-buf>` 的 "DMA Fences" 一节。)
* ``dma_resv``: A struct dma_resv (a.k.a reservation object) that is used
  to track GPU activity in the form of multiple dma_fences on a
  gpu_vm or a GEM object. The dma_resv contains an array / list
  of dma_fences and a lock that needs to be held when adding
  additional dma_fences to the dma_resv. The lock is of a type that
  allows deadlock-safe locking of multiple dma_resvs in arbitrary
  order. Please refer to the ``Reservation Objects`` section of the
  :doc:`dma-buf doc </driver-api/dma-buf>`.
  (``dma_resv`` ：一种 struct dma_resv(又名 reservation object)，用于以
  多个 dma_fence 的形式跟踪 gpu_vm 或 GEM 对象上的 GPU 活动。dma_resv 包
  含一个 dma_fence 数组/链表，以及一把在向 dma_resv 添加更多 dma_fence 时
  必须持有的锁。该锁的类型支持以任意顺序对多个 dma_resv 做无死锁加锁。请
  参阅 :doc:`dma-buf doc </driver-api/dma-buf>` 的 "Reservation Objects"
  一节。)
* ``exec function``: An exec function is a function that revalidates all
  affected gpu_vmas, submits a GPU command batch and registers the
  dma_fence representing the GPU command's activity with all affected
  dma_resvs. For completeness, although not covered by this document,
  it's worth mentioning that an exec function may also be the
  revalidation worker that is used by some drivers in compute /
  long-running mode.
  (``exec function`` ：exec function 是这样一个函数：重新验证所有受影响
  的 gpu_vma，提交一个 GPU 命令批次，并把代表该 GPU 命令活动的 dma_fence
  注册到所有受影响的 dma_resv 上。为完整起见，虽然本文不涉及，但值得一提
  的是，exec function 也可能是一些驱动在 compute / long-running 模式下使
  用的重新验证工作者 (revalidation worker)。)
* ``local object``: A GEM object which is only mapped within a
  single VM. Local GEM objects share the gpu_vm's dma_resv.
  (``local object`` ：只在单个 VM 内映射的 GEM 对象。本地 GEM 对象共享
  gpu_vm 的 dma_resv。)
* ``external object``: a.k.a shared object: A GEM object which may be shared
  by multiple gpu_vms and whose backing storage may be shared with
  other drivers.
  (``external object`` ：又名 shared object：可能被多个 gpu_vm 共享、且
  其后备存储可能与其他驱动共享的 GEM 对象。)

Locks and locking order(锁与加锁顺序)
=====================================

One of the benefits of VM_BIND is that local GEM objects share the gpu_vm's
dma_resv object and hence the dma_resv lock. So, even with a huge
number of local GEM objects, only one lock is needed to make the exec
sequence atomic.
(VM_BIND 的一个好处是，本地 GEM 对象共享 gpu_vm 的 dma_resv 对象，进而共
享 dma_resv 锁。因此，即使本地 GEM 对象数量巨大，也只需一把锁就能让 exec
序列保持原子性。)

The following locks and locking orders are used:
(使用以下锁和加锁顺序：)

* The ``gpu_vm->lock`` (optionally an rwsem). Protects the gpu_vm's
  data structure keeping track of gpu_vmas. It can also protect the
  gpu_vm's list of userptr gpu_vmas. With a CPU mm analogy this would
  correspond to the mmap_lock. An rwsem allows several readers to walk
  the VM tree concurrently, but the benefit of that concurrency most
  likely varies from driver to driver.
  (``gpu_vm->lock`` (可选地是一个 rwsem)。保护 gpu_vm 中跟踪 gpu_vma 的
  数据结构。它也可以保护 gpu_vm 的 userptr gpu_vma 链表。用 CPU mm 作类
  比，它对应 mmap_lock。rwsem 允许多个读者并发遍历 VM 树，但这种并发收益
  很可能因驱动而异。)
* The ``userptr_seqlock``. This lock is taken in read mode for each
  userptr gpu_vma on the gpu_vm's userptr list, and in write mode during mmu
  notifier invalidation. This is not a real seqlock but described in
  ``mm/mmu_notifier.c`` as a "Collision-retry read-side/write-side
  'lock' a lot like a seqcount. However this allows multiple
  write-sides to hold it at once...". The read side critical section
  is enclosed by ``mmu_interval_read_begin() /
  mmu_interval_read_retry()`` with ``mmu_interval_read_begin()``
  sleeping if the write side is held.
  The write side is held by the core mm while calling mmu interval
  invalidation notifiers.
  (``userptr_seqlock`` 。对 gpu_vm 的 userptr 链表上的每个 userptr
  gpu_vma 以读模式获取，在 mmu notifier 失效 (invalidation) 期间以写模式
  获取。它并不是真正的 seqlock，``mm/mmu_notifier.c`` 把它描述为一种
  "Collision-retry read-side/write-side 'lock' a lot like a seqcount.
  However this allows multiple write-sides to hold it at once..."。读侧
  临界区由 ``mmu_interval_read_begin() / mmu_interval_read_retry()`` 包
  裹；如果写侧被持有，``mmu_interval_read_begin()`` 会休眠。写侧由核心
  mm 在调用 mmu interval 失效通知回调时持有。)
* The ``gpu_vm->resv`` lock. Protects the gpu_vm's list of gpu_vmas needing
  rebinding, as well as the residency state of all the gpu_vm's local
  GEM objects.
  Furthermore, it typically protects the gpu_vm's list of evicted and
  external GEM objects.
  (``gpu_vm->resv`` 锁。保护 gpu_vm 中需要重绑定 (rebinding) 的 gpu_vma
  链表，以及 gpu_vm 所有本地 GEM 对象的驻留状态。此外，它通常还保护
  gpu_vm 的已逐出 (evicted) 与外部 GEM 对象链表。)
* The ``gpu_vm->userptr_notifier_lock``. This is an rwsem that is
  taken in read mode during exec and write mode during a mmu notifier
  invalidation. The userptr notifier lock is per gpu_vm.
  (``gpu_vm->userptr_notifier_lock``。这是一个 rwsem：exec 期间以读模式
  获取，mmu notifier 失效期间以写模式获取。userptr notifier 锁是每个
  gpu_vm 一把。)
* The ``gem_object->gpuva_lock`` This lock protects the GEM object's
  list of gpu_vm_bos. This is usually the same lock as the GEM
  object's dma_resv, but some drivers protects this list differently,
  see below.
  (``gem_object->gpuva_lock`` 。该锁保护 GEM 对象的 gpu_vm_bo 链表。它通
  常就是 GEM 对象 dma_resv 的那把锁，但有些驱动以不同方式保护该链表，见
  下文。)
* The ``gpu_vm list spinlocks``. With some implementations they are needed
  to be able to update the gpu_vm evicted- and external object
  list. For those implementations, the spinlocks are grabbed when the
  lists are manipulated. However, to avoid locking order violations
  with the dma_resv locks, a special scheme is needed when iterating
  over the lists.
  (``gpu_vm list spinlocks`` 。在某些实现中，需要有它们才能更新 gpu_vm 的
  已逐出对象链表和外部对象链表。对这些实现而言，操作链表时会获取
  spinlock。但为了避免与 dma_resv 锁发生加锁顺序冲突，迭代这些链表时需要
  采用特殊方案。)

.. _gpu_vma lifetime:

Protection and lifetime of gpu_vm_bos and gpu_vmas(gpu_vm_bo 与 gpu_vma 的保护与生命周期)
=========================================================================================

The GEM object's list of gpu_vm_bos, and the gpu_vm_bo's list of gpu_vmas
is protected by the ``gem_object->gpuva_lock``, which is typically the
same as the GEM object's dma_resv, but if the driver
needs to access these lists from within a dma_fence signalling
critical section, it can instead choose to protect it with a
separate lock, which can be locked from within the dma_fence signalling
critical section. Such drivers then need to pay additional attention
to what locks need to be taken from within the loop when iterating
over the gpu_vm_bo and gpu_vma lists to avoid locking-order violations.
(GEM 对象的 gpu_vm_bo 链表以及 gpu_vm_bo 的 gpu_vma 链表由
``gem_object->gpuva_lock`` 保护，它通常与 GEM 对象的 dma_resv 相同。但如
果驱动需要在 dma_fence 信号临界区内访问这些链表，它也可以选择用一把单独
的锁来保护它们，这把锁可以在 dma_fence 信号临界区内加锁。这样的驱动还需
要额外注意：迭代 gpu_vm_bo 和 gpu_vma 链表时，循环内需要获取哪些锁，以免
发生加锁顺序冲突。)

The DRM GPUVM set of helpers provide lockdep asserts that this lock is
held in relevant situations and also provides a means of making itself
aware of which lock is actually used: :c:func:`drm_gem_gpuva_set_lock`.
(DRM GPUVM 辅助函数集通过 lockdep 断言在相关情形下该锁已被持有，还提供了
让它自己知道实际使用的是哪把锁的途径：:c:func:`drm_gem_gpuva_set_lock`。)

Each gpu_vm_bo holds a reference counted pointer to the underlying GEM
object, and each gpu_vma holds a reference counted pointer to the
gpu_vm_bo. When iterating over the GEM object's list of gpu_vm_bos and
over the gpu_vm_bo's list of gpu_vmas, the ``gem_object->gpuva_lock`` must
not be dropped, otherwise, gpu_vmas attached to a gpu_vm_bo may
disappear without notice since those are not reference-counted. A
driver may implement its own scheme to allow this at the expense of
additional complexity, but this is outside the scope of this document.
(每个 gpu_vm_bo 都持有指向底层 GEM 对象的引用计数指针，每个 gpu_vma 都持
有指向 gpu_vm_bo 的引用计数指针。在遍历 GEM 对象的 gpu_vm_bo 链表和
gpu_vm_bo 的 gpu_vma 链表时，不能释放 ``gem_object->gpuva_lock``；否则，
挂接到某个 gpu_vm_bo 的 gpu_vma 可能会在毫无征兆的情况下消失，因为它们没
有引用计数。驱动可以实现自己的方案来允许这样做，但代价是额外的复杂度，这
超出了本文的范围。)

In the DRM GPUVM implementation, each gpu_vm_bo and each gpu_vma
holds a reference count on the gpu_vm itself. Due to this, and to avoid circular
reference counting, cleanup of the gpu_vm's gpu_vmas must not be done from the
gpu_vm's destructor. Drivers typically implements a gpu_vm close
function for this cleanup. The gpu_vm close function will abort gpu
execution using this VM, unmap all gpu_vmas and release page-table memory.
(在 DRM GPUVM 实现中，每个 gpu_vm_bo 和每个 gpu_vma 都持有 gpu_vm 自身的
引用计数。因此，为了避免循环引用计数，不能在 gpu_vm 的析构函数里清理
gpu_vm 的 gpu_vma。驱动通常为此实现一个 gpu_vm close 函数。gpu_vm close
函数会中止使用该 VM 的 GPU 执行，解除所有 gpu_vma 的映射，并释放页表内
存。)

Revalidation and eviction of local objects(本地对象的重新验证与逐出)
====================================================================

Note that in all the code examples given below we use simplified
pseudo-code. In particular, the dma_resv deadlock avoidance algorithm
as well as reserving memory for dma_resv fences is left out.
(注意，下面给出的所有代码示例都使用了简化伪代码。特别是省略了 dma_resv
死锁避免算法，以及为 dma_resv fence 预留内存的部分。)

Revalidation(重新验证)
______________________
With VM_BIND, all local objects need to be resident when the gpu is
executing using the gpu_vm, and the objects need to have valid
gpu_vmas set up pointing to them. Typically, each gpu command buffer
submission is therefore preceded with a re-validation section:
(使用 VM_BIND 时，GPU 使用 gpu_vm 执行期间，所有本地对象都必须处于驻留
(resident)状态，且这些对象需要有指向它们的有效 gpu_vma。因此，通常每次
GPU 命令缓冲区提交之前都有一个重新验证 (re-validation) 段：)

.. code-block:: C

   dma_resv_lock(gpu_vm->resv);

   // Validation section starts here.
   for_each_gpu_vm_bo_on_evict_list(&gpu_vm->evict_list, &gpu_vm_bo) {
           validate_gem_bo(&gpu_vm_bo->gem_bo);

           // The following list iteration needs the Gem object's
           // dma_resv to be held (it protects the gpu_vm_bo's list of
           // gpu_vmas, but since local gem objects share the gpu_vm's
           // dma_resv, it is already held at this point.
           for_each_gpu_vma_of_gpu_vm_bo(&gpu_vm_bo, &gpu_vma)
                  move_gpu_vma_to_rebind_list(&gpu_vma, &gpu_vm->rebind_list);
   }

   for_each_gpu_vma_on_rebind_list(&gpu vm->rebind_list, &gpu_vma) {
           rebind_gpu_vma(&gpu_vma);
           remove_gpu_vma_from_rebind_list(&gpu_vma);
   }
   // Validation section ends here, and job submission starts.

   add_dependencies(&gpu_job, &gpu_vm->resv);
   job_dma_fence = gpu_submit(&gpu_job));

   add_dma_fence(job_dma_fence, &gpu_vm->resv);
   dma_resv_unlock(gpu_vm->resv);

The reason for having a separate gpu_vm rebind list is that there
might be userptr gpu_vmas that are not mapping a buffer object that
also need rebinding.
(之所以要有一个单独的 gpu_vm 重绑定 (rebind) 链表，是因为可能还有一些未
映射缓冲对象、但也需要重绑定的 userptr gpu_vma。)

Eviction(逐出)
______________

Eviction of one of these local objects will then look similar to the
following:
(逐出其中一个本地对象的过程则大致如下：)

.. code-block:: C

   obj = get_object_from_lru();

   dma_resv_lock(obj->resv);
   for_each_gpu_vm_bo_of_obj(obj, &gpu_vm_bo);
           add_gpu_vm_bo_to_evict_list(&gpu_vm_bo, &gpu_vm->evict_list);

   add_dependencies(&eviction_job, &obj->resv);
   job_dma_fence = gpu_submit(&eviction_job);
   add_dma_fence(&obj->resv, job_dma_fence);

   dma_resv_unlock(&obj->resv);
   put_object(obj);

Note that since the object is local to the gpu_vm, it will share the gpu_vm's
dma_resv lock such that ``obj->resv == gpu_vm->resv``.
The gpu_vm_bos marked for eviction are put on the gpu_vm's evict list,
which is protected by ``gpu_vm->resv``. During eviction all local
objects have their dma_resv locked and, due to the above equality, also
the gpu_vm's dma_resv protecting the gpu_vm's evict list is locked.
(注意，由于对象对 gpu_vm 是本地的，它会共享 gpu_vm 的 dma_resv 锁，即
``obj->resv == gpu_vm->resv`` 。被标记为待逐出的 gpu_vm_bo 会放到 gpu_vm
的逐出 (evict) 链表上，该链表由 ``gpu_vm->resv`` 保护。逐出期间，所有本
地对象的 dma_resv 都被锁定；由于上述等式，保护 gpu_vm 逐出链表的 gpu_vm
dma_resv 也处于锁定状态。)

With VM_BIND, gpu_vmas don't need to be unbound before eviction,
since the driver must ensure that the eviction blit or copy will wait
for GPU idle or depend on all previous GPU activity. Furthermore, any
subsequent attempt by the GPU to access freed memory through the
gpu_vma will be preceded by a new exec function, with a revalidation
section which will make sure all gpu_vmas are rebound. The eviction
code holding the object's dma_resv while revalidating will ensure a
new exec function may not race with the eviction.
(使用 VM_BIND 时，gpu_vma 在逐出前不需要解绑 (unbound)，因为驱动必须确保
逐出的 blit 或拷贝操作会等待 GPU 空闲，或依赖于所有先前的 GPU 活动。此
外，GPU 后续任何通过 gpu_vma 访问已释放内存的尝试，都会先经过一个新的
exec function，其重新验证段会确保所有 gpu_vma 都被重绑定。逐出代码在重新
验证期间持有对象的 dma_resv，这将确保新的 exec function 不会与逐出操作竞
争。)

A driver can be implemented in such a way that, on each exec function,
only a subset of vmas are selected for rebind.  In this case, all vmas that are
*not* selected for rebind must be unbound before the exec
function workload is submitted.
(驱动也可以实现成：每次 exec function 只选择一部分 vma 参与重绑定。这种
情况下，所有 *未* 被选中重绑定的 vma 必须在提交 exec function 工作负载之
前解绑。)

Locking with external buffer objects(外部缓冲对象的加锁)
========================================================

Since external buffer objects may be shared by multiple gpu_vm's they
can't share their reservation object with a single gpu_vm. Instead
they need to have a reservation object of their own. The external
objects bound to a gpu_vm using one or many gpu_vmas are therefore put on a
per-gpu_vm list which is protected by the gpu_vm's dma_resv lock or
one of the :ref:`gpu_vm list spinlocks <Spinlock iteration>`. Once
the gpu_vm's reservation object is locked, it is safe to traverse the
external object list and lock the dma_resvs of all external
objects. However, if instead a list spinlock is used, a more elaborate
iteration scheme needs to be used.
(由于外部缓冲对象可能被多个 gpu_vm 共享，它们不能与单个 gpu_vm 共享其
reservation object，而是需要拥有自己的 reservation object。因此，通过一
个或多个 gpu_vma 绑定到 gpu_vm 的外部对象，会被放到一个 per-gpu_vm 链表
上，该链表由 gpu_vm 的 dma_resv 锁或 :ref:`gpu_vm list spinlocks <Spinlock
iteration>` 之一保护。一旦 gpu_vm 的 reservation object 被锁定，就可以安
全地遍历外部对象链表并锁定所有外部对象的 dma_resv。但如果改用链表
spinlock，则需要使用更精巧的迭代方案。)

At eviction time, the gpu_vm_bos of *all* the gpu_vms an external
object is bound to need to be put on their gpu_vm's evict list.
However, when evicting an external object, the dma_resvs of the
gpu_vms the object is bound to are typically not held. Only
the object's private dma_resv can be guaranteed to be held. If there
is a ww_acquire context at hand at eviction time we could grab those
dma_resvs but that could cause expensive ww_mutex rollbacks. A simple
option is to just mark the gpu_vm_bos of the evicted gem object with
an ``evicted`` bool that is inspected before the next time the
corresponding gpu_vm evicted list needs to be traversed. For example, when
traversing the list of external objects and locking them. At that time,
both the gpu_vm's dma_resv and the object's dma_resv is held, and the
gpu_vm_bo marked evicted, can then be added to the gpu_vm's list of
evicted gpu_vm_bos. The ``evicted`` bool is formally protected by the
object's dma_resv.
(逐出时，外部对象所绑定的 *所有* gpu_vm 的 gpu_vm_bo 都需要放到各自
gpu_vm 的逐出链表上。然而，逐出外部对象时，通常并不持有该对象所绑定
gpu_vm 的 dma_resv；只有对象自己的私有 dma_resv 能保证被持有。如果逐出时
手头有一个 ww_acquire 上下文，我们可以抓取那些 dma_resv，但这可能导致代
价高昂的 ww_mutex 回滚。一个简单的办法是：只用一个 ``evicted`` 布尔值标
记被逐出 gem 对象的各个 gpu_vm_bo，并在下一次需要遍历相应 gpu_vm 逐出链
表之前检查它。例如，在遍历外部对象链表并对它们加锁时。此时 gpu_vm 的
dma_resv 和对象的 dma_resv 都被持有，被标记为 evicted 的 gpu_vm_bo 便可
以加入 gpu_vm 的已逐出 gpu_vm_bo 链表。``evicted`` 布尔值形式上由对象的
dma_resv 保护。)

The exec function becomes

.. code-block:: C

   dma_resv_lock(gpu_vm->resv);

   // External object list is protected by the gpu_vm->resv lock.
   for_each_gpu_vm_bo_on_extobj_list(gpu_vm, &gpu_vm_bo) {
           dma_resv_lock(gpu_vm_bo.gem_obj->resv);
           if (gpu_vm_bo_marked_evicted(&gpu_vm_bo))
                   add_gpu_vm_bo_to_evict_list(&gpu_vm_bo, &gpu_vm->evict_list);
   }

   for_each_gpu_vm_bo_on_evict_list(&gpu_vm->evict_list, &gpu_vm_bo) {
           validate_gem_bo(&gpu_vm_bo->gem_bo);

           for_each_gpu_vma_of_gpu_vm_bo(&gpu_vm_bo, &gpu_vma)
                  move_gpu_vma_to_rebind_list(&gpu_vma, &gpu_vm->rebind_list);
   }

   for_each_gpu_vma_on_rebind_list(&gpu vm->rebind_list, &gpu_vma) {
           rebind_gpu_vma(&gpu_vma);
           remove_gpu_vma_from_rebind_list(&gpu_vma);
   }

   add_dependencies(&gpu_job, &gpu_vm->resv);
   job_dma_fence = gpu_submit(&gpu_job));

   add_dma_fence(job_dma_fence, &gpu_vm->resv);
   for_each_external_obj(gpu_vm, &obj)
          add_dma_fence(job_dma_fence, &obj->resv);
   dma_resv_unlock_all_resv_locks();

And the corresponding shared-object aware eviction would look like:
(而相应的、感知共享对象 (shared-object aware) 的逐出则如下所示：)

.. code-block:: C

   obj = get_object_from_lru();

   dma_resv_lock(obj->resv);
   for_each_gpu_vm_bo_of_obj(obj, &gpu_vm_bo)
           if (object_is_vm_local(obj))
                add_gpu_vm_bo_to_evict_list(&gpu_vm_bo, &gpu_vm->evict_list);
           else
                mark_gpu_vm_bo_evicted(&gpu_vm_bo);

   add_dependencies(&eviction_job, &obj->resv);
   job_dma_fence = gpu_submit(&eviction_job);
   add_dma_fence(&obj->resv, job_dma_fence);

   dma_resv_unlock(&obj->resv);
   put_object(obj);

.. _Spinlock iteration:

Accessing the gpu_vm's lists without the dma_resv lock held(不持有 dma_resv 锁访问 gpu_vm 的链表)
=================================================================================================

Some drivers will hold the gpu_vm's dma_resv lock when accessing the
gpu_vm's evict list and external objects lists. However, there are
drivers that need to access these lists without the dma_resv lock
held, for example due to asynchronous state updates from within the
dma_fence signalling critical path. In such cases, a spinlock can be
used to protect manipulation of the lists. However, since higher level
sleeping locks need to be taken for each list item while iterating
over the lists, the items already iterated over need to be
temporarily moved to a private list and the spinlock released
while processing each item:
(有些驱动在访问 gpu_vm 的逐出链表和外部对象链表时会持有 gpu_vm 的
dma_resv 锁。但也有一些驱动需要在不持有 dma_resv 锁的情况下访问这些链
表，例如由于 dma_fence 信号临界路径内的异步状态更新。这种情况下可以用
spinlock 保护对链表的操作。但由于迭代链表时需要为每个链表
项获取更高层的可睡眠锁，已迭代过的项需要临时移到一个私有链表，并在处理每
个项期间释放 spinlock：)

.. code block:: C

    struct list_head still_in_list;

    INIT_LIST_HEAD(&still_in_list);

    spin_lock(&gpu_vm->list_lock);
    do {
            struct list_head *entry = list_first_entry_or_null(&gpu_vm->list, head);

            if (!entry)
                    break;

            list_move_tail(&entry->head, &still_in_list);
            list_entry_get_unless_zero(entry);
            spin_unlock(&gpu_vm->list_lock);

            process(entry);

            spin_lock(&gpu_vm->list_lock);
            list_entry_put(entry);
    } while (true);

    list_splice_tail(&still_in_list, &gpu_vm->list);
    spin_unlock(&gpu_vm->list_lock);

Due to the additional locking and atomic operations, drivers that *can*
avoid accessing the gpu_vm's list outside of the dma_resv lock
might want to avoid also this iteration scheme. Particularly, if the
driver anticipates a large number of list items. For lists where the
anticipated number of list items is small, where list iteration doesn't
happen very often or if there is a significant additional cost
associated with each iteration, the atomic operation overhead
associated with this type of iteration is, most likely, negligible. Note that
if this scheme is used, it is necessary to make sure this list
iteration is protected by an outer level lock or semaphore, since list
items are temporarily pulled off the list while iterating, and it is
also worth mentioning that the local list ``still_in_list`` should
also be considered protected by the ``gpu_vm->list_lock``, and it is
thus possible that items can be removed also from the local list
concurrently with list iteration.
(由于额外的加锁和原子操作，那些 *能够* 避免在 dma_resv 锁之外访问 gpu_vm
链表的驱动，可能也想避免这种迭代方案——尤其是当驱动预计链表项数量很大
时。而对于预计链表项数量少、链表迭代不频繁、或每次迭代都有显著额外开销的
链表，这种迭代方式带来的原子操作开销很可能可以忽略不计。注意，如果使用这
种方案，必须确保该链表迭代受到外层锁或信号量的保护，因为迭代期间链表项会
被临时从链表上摘下。还值得一提的是，本地链表 ``still_in_list`` 也应视为受
``gpu_vm->list_lock`` 保护，因此在链表迭代的同时，项也可能从本地链表中
被移除。)

Please refer to the :ref:`DRM GPUVM locking section
<drm_gpuvm_locking>` and its internal
:c:func:`get_next_vm_bo_from_list` function.
(请参阅 :ref:`DRM GPUVM locking section <drm_gpuvm_locking>` 及其内部的
:c:func:`get_next_vm_bo_from_list` 函数。)


userptr gpu_vmas(userptr 类 gpu_vma)
====================================

A userptr gpu_vma is a gpu_vma that, instead of mapping a buffer object to a
GPU virtual address range, directly maps a CPU mm range of anonymous-
or file page-cache pages.
A very simple approach would be to just pin the pages using
pin_user_pages() at bind time and unpin them at unbind time, but this
creates a Denial-Of-Service vector since a single user-space process
would be able to pin down all of system memory, which is not
desirable. (For special use-cases and assuming proper accounting pinning might
still be a desirable feature, though). What we need to do in the
general case is to obtain a reference to the desired pages, make sure
we are notified using a MMU notifier just before the CPU mm unmaps the
pages, dirty them if they are not mapped read-only to the GPU, and
then drop the reference.
(userptr gpu_vma 是这样一种 gpu_vma：它不把缓冲对象映射到 GPU 虚拟地址范
围，而是直接映射一段 CPU mm 的匿名页或文件 page-cache 页范围。一个非常简
单的做法是在绑定时用 pin_user_pages() 固定 (pin) 这些页，在解绑时解除固
定，但这会带来拒绝服务 (Denial-Of-Service) 攻击面，因为单个用户空间进程
就能把全部系统内存固定住，这是不可取的。(不过，对特殊用例并在正确记账的
前提下，pinning 或许仍是可取的功能。)一般情况下我们需要做的是：获取所需
页面的引用，确保在 CPU mm 解除这些页的映射之前会通过 MMU notifier 收到通
知，如果这些页对 GPU 不是只读映射则将其置脏 (dirty)，然后放下引用。)

When we are notified by the MMU notifier that CPU mm is about to drop the
pages, we need to stop GPU access to the pages by waiting for VM idle
in the MMU notifier and make sure that before the next time the GPU
tries to access whatever is now present in the CPU mm range, we unmap
the old pages from the GPU page tables and repeat the process of
obtaining new page references. (See the :ref:`notifier example
<Invalidation example>` below). Note that when the core mm decides to
laundry pages, we get such an unmap MMU notification and can mark the
pages dirty again before the next GPU access. We also get similar MMU
notifications for NUMA accounting which the GPU driver doesn't really
need to care about, but so far it has proven difficult to exclude
certain notifications.
(当 MMU notifier 通知我们 CPU mm 即将释放这些页时，我们需要通过在 MMU
notifier 中等待 VM 空闲来停止 GPU 对这些页的访问，并确保在 GPU 下一次尝试
访问 CPU mm 范围内现有内容之前，先从 GPU 页表中解除旧页的映射，再重新执行
获取新页引用的流程。(参见下文的 :ref:`notifier example <Invalidation
example>`)。注意，当核心 mm 决定清洗 (laundry) 页面时，我们会收到这样的解
除映射 MMU 通知，从而可以在下一次 GPU 访问之前再次把页标记为脏。NUMA 记
账也会产生类似的 MMU 通知，GPU 驱动其实并不需要关心，但迄今为止要排除某
些特定通知仍很困难。)

Using a MMU notifier for device DMA (and other methods) is described in
:ref:`the pin_user_pages() documentation <mmu-notifier-registration-case>`.
(在设备 DMA 中使用 MMU notifier(以及其他方法)在
:ref:`the pin_user_pages() documentation <mmu-notifier-registration-case>`
中有描述。)

Now, the method of obtaining struct page references using
get_user_pages() unfortunately can't be used under a dma_resv lock
since that would violate the locking order of the dma_resv lock vs the
mmap_lock that is grabbed when resolving a CPU pagefault. This means
the gpu_vm's list of userptr gpu_vmas needs to be protected by an
outer lock, which in our example below is the ``gpu_vm->lock``.
(遗憾的是，用 get_user_pages() 获取 struct page 引用的方法不能在持有
dma_resv 锁的情况下使用，因为这会违反 dma_resv 锁相对 mmap_lock 的加锁顺
序——解析 CPU 缺页异常时会获取 mmap_lock。这意味着 gpu_vm 的 userptr
gpu_vma 链表需要由一把外层锁保护，在下文的示例中就是 ``gpu_vm->lock`` 。)

The MMU interval seqlock for a userptr gpu_vma is used in the following
way:
(userptr gpu_vma 的 MMU interval seqlock 按如下方式使用：)

.. code-block:: C

   // Exclusive locking mode here is strictly needed only if there are
   // invalidated userptr gpu_vmas present, to avoid concurrent userptr
   // revalidations of the same userptr gpu_vma.
   down_write(&gpu_vm->lock);
   retry:

   // Note: mmu_interval_read_begin() blocks until there is no
   // invalidation notifier running anymore.
   seq = mmu_interval_read_begin(&gpu_vma->userptr_interval);
   if (seq != gpu_vma->saved_seq) {
           obtain_new_page_pointers(&gpu_vma);
           dma_resv_lock(&gpu_vm->resv);
           add_gpu_vma_to_revalidate_list(&gpu_vma, &gpu_vm);
           dma_resv_unlock(&gpu_vm->resv);
           gpu_vma->saved_seq = seq;
   }

   // The usual revalidation goes here.

   // Final userptr sequence validation may not happen before the
   // submission dma_fence is added to the gpu_vm's resv, from the POW
   // of the MMU invalidation notifier. Hence the
   // userptr_notifier_lock that will make them appear atomic.

   add_dependencies(&gpu_job, &gpu_vm->resv);
   down_read(&gpu_vm->userptr_notifier_lock);
   if (mmu_interval_read_retry(&gpu_vma->userptr_interval, gpu_vma->saved_seq)) {
          up_read(&gpu_vm->userptr_notifier_lock);
          goto retry;
   }

   job_dma_fence = gpu_submit(&gpu_job));

   add_dma_fence(job_dma_fence, &gpu_vm->resv);

   for_each_external_obj(gpu_vm, &obj)
          add_dma_fence(job_dma_fence, &obj->resv);

   dma_resv_unlock_all_resv_locks();
   up_read(&gpu_vm->userptr_notifier_lock);
   up_write(&gpu_vm->lock);

The code between ``mmu_interval_read_begin()`` and the
``mmu_interval_read_retry()`` marks the read side critical section of
what we call the ``userptr_seqlock``. In reality, the gpu_vm's userptr
gpu_vma list is looped through, and the check is done for *all* of its
userptr gpu_vmas, although we only show a single one here.
(``mmu_interval_read_begin()`` 与 ``mmu_interval_read_retry()`` 之间的代
码标记了我们称之为 ``userptr_seqlock`` 的读侧临界区。实际上会遍历 gpu_vm
的 userptr gpu_vma 链表，并对其 *所有* userptr gpu_vma 做该检查，只不过这
里只展示了其中一个。)

The userptr gpu_vma MMU invalidation notifier might be called from
reclaim context and, again, to avoid locking order violations, we can't
take any dma_resv lock nor the gpu_vm->lock from within it.
(userptr gpu_vma 的 MMU 失效通知回调可能会在回收 (reclaim) 上下文中被调
用；同样地，为避免加锁顺序冲突，不能在它内部获取任何 dma_resv 锁，也不能
获取 gpu_vm->lock。)

.. _Invalidation example:
.. code-block:: C

  bool gpu_vma_userptr_invalidate(userptr_interval, cur_seq)
  {
          // Make sure the exec function either sees the new sequence
          // and backs off or we wait for the dma-fence:

          down_write(&gpu_vm->userptr_notifier_lock);
          mmu_interval_set_seq(userptr_interval, cur_seq);
          up_write(&gpu_vm->userptr_notifier_lock);

          // At this point, the exec function can't succeed in
          // submitting a new job, because cur_seq is an invalid
          // sequence number and will always cause a retry. When all
          // invalidation callbacks, the mmu notifier core will flip
          // the sequence number to a valid one. However we need to
          // stop gpu access to the old pages here.

          dma_resv_wait_timeout(&gpu_vm->resv, DMA_RESV_USAGE_BOOKKEEP,
                                false, MAX_SCHEDULE_TIMEOUT);
          return true;
  }

When this invalidation notifier returns, the GPU can no longer be
accessing the old pages of the userptr gpu_vma and needs to redo the
page-binding before a new GPU submission can succeed.
(当这个失效通知回调返回时，GPU 已无法再访问 userptr gpu_vma 的旧页，并且
在新的 GPU 提交能够成功之前需要重做页绑定。)

Efficient userptr gpu_vma exec_function iteration(高效的 userptr gpu_vma exec_function 迭代)
____________________________________________________________________________________________

If the gpu_vm's list of userptr gpu_vmas becomes large, it's
inefficient to iterate through the complete lists of userptrs on each
exec function to check whether each userptr gpu_vma's saved
sequence number is stale. A solution to this is to put all
*invalidated* userptr gpu_vmas on a separate gpu_vm list and
only check the gpu_vmas present on this list on each exec
function. This list will then lend itself very-well to the spinlock
locking scheme that is
:ref:`described in the spinlock iteration section <Spinlock iteration>`, since
in the mmu notifier, where we add the invalidated gpu_vmas to the
list, it's not possible to take any outer locks like the
``gpu_vm->lock`` or the ``gpu_vm->resv`` lock. Note that the
``gpu_vm->lock`` still needs to be taken while iterating to ensure the list is
complete, as also mentioned in that section.
(如果 gpu_vm 的 userptr gpu_vma 链表变得很大，每次 exec function 都完整遍
历 userptr 链表以检查每个 userptr gpu_vma 保存的序号是否过期就太低效了。
一种解决办法是把所有 *已失效* (invalidated) 的 userptr gpu_vma 放到一个
单独的 gpu_vm 链表上，每次 exec function 只检查该链表上的 gpu_vma。这个
链表非常适合 :ref:`described in the spinlock iteration section <Spinlock
iteration>` 的 spinlock 加锁方案，因为在 mmu notifier——即把已失效
gpu_vma 加入链表的地方——不可能获取 ``gpu_vm->lock`` 或 ``gpu_vm->resv``
锁之类的任何外层锁。注意，正如该节所述，迭代期间仍需要获取
``gpu_vm->lock`` 以确保链表完整。)

If using an invalidated userptr list like this, the retry check in the
exec function trivially becomes a check for invalidated list empty.
(如果使用这样的已失效 userptr 链表，exec function 中的重试检查就简单地表
示为检查已失效链表是否为空。)

Locking at bind and unbind time(绑定与解绑时的加锁)
===================================================

At bind time, assuming a GEM object backed gpu_vma, each
gpu_vma needs to be associated with a gpu_vm_bo and that
gpu_vm_bo in turn needs to be added to the GEM object's
gpu_vm_bo list, and possibly to the gpu_vm's external object
list. This is referred to as *linking* the gpu_vma, and typically
requires that the ``gpu_vm->lock`` and the ``gem_object->gpuva_lock``
are held. When unlinking a gpu_vma the same locks should be held,
and that ensures that when iterating over ``gpu_vmas`, either under
the ``gpu_vm->resv`` or the GEM object's dma_resv, that the gpu_vmas
stay alive as long as the lock under which we iterate is not released. For
userptr gpu_vmas it's similarly required that during vma destroy, the
outer ``gpu_vm->lock`` is held, since otherwise when iterating over
the invalidated userptr list as described in the previous section,
there is nothing keeping those userptr gpu_vmas alive.
(绑定时，假设是 GEM 对象作后备的 gpu_vma，每个 gpu_vma 都需要关联到一个
gpu_vm_bo，而这个 gpu_vm_bo 又需要加入 GEM 对象的 gpu_vm_bo 链表，并可能
加入 gpu_vm 的外部对象链表。这称为 *linking* (链接) gpu_vma，通常要求持
有 ``gpu_vm->lock`` 和 ``gem_object->gpuva_lock``。解绑 (unlinking) 一个
gpu_vma 时应持有同样的锁，这确保了在 ``gpu_vm->resv`` 或 GEM 对象的
dma_resv 之下遍历 ``gpu_vmas`` 时，只要我们据以遍历的锁未释放，gpu_vma 就
一直存活。对 userptr gpu_vma，同样要求在 vma 销毁期间持有外层
``gpu_vm->lock``，否则按上一节所述迭代已失效 userptr 链表时，将没有任何
东西维持这些 userptr gpu_vma 存活。)

Locking for recoverable page-fault page-table updates(可恢复缺页异常的页表更新加锁)
===================================================================================

There are two important things we need to ensure with locking for
recoverable page-faults:
(关于可恢复缺页异常 (recoverable page-faults) 的加锁，我们需要确保两个重
要事项：)

* At the time we return pages back to the system / allocator for
  reuse, there should be no remaining GPU mappings and any GPU TLB
  must have been flushed.
  (当我们把页交还系统/分配器复用时，不应再有任何残留的 GPU 映射，并且必
  须已刷新所有 GPU TLB。)
* The unmapping and mapping of a gpu_vma must not race.
  (gpu_vma 的解除映射与建立映射不得相互竞争。)

Since the unmapping (or zapping) of GPU ptes is typically taking place
where it is hard or even impossible to take any outer level locks we
must either introduce a new lock that is held at both mapping and
unmapping time, or look at the locks we do hold at unmapping time and
make sure that they are held also at mapping time. For userptr
gpu_vmas, the ``userptr_seqlock`` is held in write mode in the mmu
invalidation notifier where zapping happens. Hence, if the
``userptr_seqlock`` as well as the ``gpu_vm->userptr_notifier_lock``
is held in read mode during mapping, it will not race with the
zapping. For GEM object backed gpu_vmas, zapping will take place under
the GEM object's dma_resv and ensuring that the dma_resv is held also
when populating the page-tables for any gpu_vma pointing to the GEM
object, will similarly ensure we are race-free.
(由于 GPU pte 的解除映射(或者说 zap)通常发生在难以甚至不可能获取任何外
层锁的地方，我们要么引入一把在映射和解映射时都持有的新锁，要么考察解映射
时我们实际持有的锁，并确保映射时也持有它们。对 userptr gpu_vma，
``userptr_seqlock`` 在发生 zap 的 mmu 失效通知回调中以写模式持有。因此，
如果映射期间以读模式持有 ``userptr_seqlock`` 以及
``gpu_vm->userptr_notifier_lock``，就不会与 zap 竞争。对 GEM 对象作后备的
gpu_vma，zap 会在 GEM 对象的 dma_resv 之下进行；只要为指向该 GEM 对象的任
何 gpu_vma 填充页表时也持有 dma_resv，同样能确保我们免于竞争。)

If any part of the mapping is performed asynchronously
under a dma-fence with these locks released, the zapping will need to
wait for that dma-fence to signal under the relevant lock before
starting to modify the page-table.
(如果映射的任何部分是在这些锁已释放的情况下于 dma-fence 之下异步进行的，
那么 zap 在开始修改页表之前，将需要在相关锁的保护下等待该 dma-fence 发出
信号。)

Since modifying the
page-table structure in a way that frees up page-table memory
might also require outer level locks, the zapping of GPU ptes
typically focuses only on zeroing page-table or page-directory entries
and flushing TLB, whereas freeing of page-table memory is deferred to
unbind or rebind time.
(由于以释放页表内存的方式修改页表结构可能同样需要外层锁，GPU pte 的
zap 通常只专注于清零页表项或页目录项并刷新 TLB，而页表内存的释放被推迟到
解绑或重绑定时进行。)
