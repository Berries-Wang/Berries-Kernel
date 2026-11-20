======================================================================
I915 VM_BIND feature design and use cases(I915 VM_BIND 特性设计与用例)
======================================================================

VM_BIND feature(VM_BIND 特性)
==============================
DRM_I915_GEM_VM_BIND/UNBIND ioctls allows UMD to bind/unbind GEM buffer
objects (BOs) or sections of a BOs at specified GPU virtual addresses on a
specified address space (VM). These mappings (also referred to as persistent
mappings) will be persistent across multiple GPU submissions (execbuf calls)
issued by the UMD, without user having to provide a list of all required
mappings during each submission (as required by older execbuf mode).
(DRM_I915_GEM_VM_BIND/UNBIND ioctl 允许 UMD 在指定地址空间 (VM) 上，按指定的 GPU 虚拟地址绑定/解绑定 GEM 缓冲对象 (BO) 或 BO 的部分区段。这些映射 (也称为持久映射) 在 UMD 发起的多次 GPU 提交 (execbuf 调用) 之间保持持久，用户无需像旧的 execbuf 模式那样在每次提交时提供所有所需映射的列表。)

The VM_BIND/UNBIND calls allow UMDs to request a timeline out fence for
signaling the completion of bind/unbind operation.
(VM_BIND/UNBIND 调用允许 UMD 请求一个 timeline out fence，用于发出绑定/解绑定操作完成信号。)

VM_BIND feature is advertised to user via I915_PARAM_VM_BIND_VERSION.
User has to opt-in for VM_BIND mode of binding for an address space (VM)
during VM creation time via I915_VM_CREATE_FLAGS_USE_VM_BIND extension.
(VM_BIND 特性通过 I915_PARAM_VM_BIND_VERSION 向用户宣告。用户必须在创建 VM 时通过 I915_VM_CREATE_FLAGS_USE_VM_BIND 扩展为该地址空间 (VM) 选择启用 VM_BIND 绑定模式。)

VM_BIND/UNBIND ioctl calls executed on different CPU threads concurrently are
not ordered. Furthermore, parts of the VM_BIND/UNBIND operations can be done
asynchronously, when valid out fence is specified.
(在不同 CPU 线程上并发执行的 VM_BIND/UNBIND ioctl 调用之间没有顺序保证。此外，当指定了有效的 out fence 时，VM_BIND/UNBIND 操作的部分工作可以异步完成。)

VM_BIND features include:(VM_BIND 特性包括：)

* Multiple Virtual Address (VA) mappings can map to the same physical pages
  of an object (aliasing).
* VA mapping can map to a partial section of the BO (partial binding).
* Support capture of persistent mappings in the dump upon GPU error.
* Support for userptr gem objects (no special uapi is required for this).
  (多个虚拟地址 (VA) 映射可以映射到对象的同一批物理页 (别名，aliasing)。
  VA 映射可以映射到 BO 的局部区段 (部分绑定，partial binding)。
  支持 GPU 出错时在 dump 中捕获持久映射。
  支持 userptr gem 对象 (无需特殊 uapi)。)

TLB flush consideration(TLB 冲刷考量)
-------------------------------------
The i915 driver flushes the TLB for each submission and when an object's
pages are released. The VM_BIND/UNBIND operation will not do any additional
TLB flush. Any VM_BIND mapping added will be in the working set for subsequent
submissions on that VM and will not be in the working set for currently running
batches (which would require additional TLB flushes, which is not supported).
(i915 驱动在每次提交时以及对象页被释放时冲刷 TLB。VM_BIND/UNBIND 操作不会做任何额外的 TLB 冲刷。新增的 VM_BIND 映射会进入该 VM 后续提交的工作集，但不会进入当前正在运行的批次的工作集 (后者需要额外 TLB 冲刷，不受支持)。)

Execbuf ioctl in VM_BIND mode(VM_BIND 模式下的 execbuf ioctl)
-------------------------------------------------------------
A VM in VM_BIND mode will not support older execbuf mode of binding.
The execbuf ioctl handling in VM_BIND mode differs significantly from the
older execbuf2 ioctl (See struct drm_i915_gem_execbuffer2).
Hence, a new execbuf3 ioctl has been added to support VM_BIND mode. (See
struct drm_i915_gem_execbuffer3). The execbuf3 ioctl will not accept any
execlist. Hence, no support for implicit sync. It is expected that the below
work will be able to support requirements of object dependency setting in all
use cases:
(VM_BIND 模式下的 VM 将不支持旧的 execbuf 绑定模式。VM_BIND 模式下的 execbuf ioctl 处理与旧的 execbuf2 ioctl (见 struct drm_i915_gem_execbuffer2) 差异很大。因此新增了 execbuf3 ioctl 来支持 VM_BIND 模式 (见 struct drm_i915_gem_execbuffer3)。execbuf3 ioctl 不接受任何 execlist，因此不支持隐式同步。预期下面的工作将能满足所有用例中对象依赖项设置的需求：)

"dma-buf: Add an API for exporting sync files"
(https://lwn.net/Articles/859290/)

The new execbuf3 ioctl only works in VM_BIND mode and the VM_BIND mode only
works with execbuf3 ioctl for submission. All BOs mapped on that VM (through
VM_BIND call) at the time of execbuf3 call are deemed required for that
submission.
(新的 execbuf3 ioctl 只在 VM_BIND 模式下工作，而 VM_BIND 模式也只能通过 execbuf3 ioctl 提交。execbuf3 调用时该 VM 上已映射 (通过 VM_BIND 调用) 的所有 BO 都被视为该次提交所必需。)

The execbuf3 ioctl directly specifies the batch addresses instead of as
object handles as in execbuf2 ioctl. The execbuf3 ioctl will also not
support many of the older features like in/out/submit fences, fence array,
default gem context and many more (See struct drm_i915_gem_execbuffer3).
(execbuf3 ioctl 直接指定批次地址，而不是像 execbuf2 ioctl 那样用对象句柄。execbuf3 ioctl 也不再支持许多旧特性，如 in/out/submit fence、fence 数组、默认 gem 上下文等等 (见 struct drm_i915_gem_execbuffer3)。)

In VM_BIND mode, VA allocation is completely managed by the user instead of
the i915 driver. Hence all VA assignment, eviction are not applicable in
VM_BIND mode. Also, for determining object activeness, VM_BIND mode will not
be using the i915_vma active reference tracking. It will instead use dma-resv
object for that (See `VM_BIND dma_resv usage(VM_BIND 的 dma_resv 用法)`_).
(在 VM_BIND 模式下，VA 分配完全由用户管理，而不是由 i915 驱动管理。因此所有 VA 分配、逐出 (eviction) 在 VM_BIND 模式下都不适用。另外，为判定对象活跃性，VM_BIND 模式不会使用 i915_vma 活跃引用跟踪，而是改用 dma-resv 对象 (见 `VM_BIND dma_resv usage(VM_BIND 的 dma_resv 用法)`_)。)

So, a lot of existing code supporting execbuf2 ioctl, like relocations, VA
evictions, vma lookup table, implicit sync, vma active reference tracking etc.,
are not applicable for execbuf3 ioctl. Hence, all execbuf3 specific handling
should be in a separate file and only functionalities common to these ioctls
can be the shared code where possible.
(因此，许多支持 execbuf2 ioctl 的现有代码，如重定位 (relocation)、VA 逐出、vma 查找表、隐式同步、vma 活跃引用跟踪等，都不适用于 execbuf3 ioctl。所以所有 execbuf3 特有的处理都应放在单独的文件中，只有这些 ioctl 共同的功能才尽可能作为共享代码。)

VM_PRIVATE objects(VM_PRIVATE 对象)
-----------------------------------
By default, BOs can be mapped on multiple VMs and can also be dma-buf
exported. Hence these BOs are referred to as Shared BOs.
During each execbuf submission, the request fence must be added to the
dma-resv fence list of all shared BOs mapped on the VM.
(默认情况下，BO 可以映射到多个 VM，也可以被 dma-buf 导出。因此这类 BO 称为共享 BO (Shared BO)。在每次 execbuf 提交期间，必须把请求 fence 加入该 VM 上已映射的所有共享 BO 的 dma-resv fence 列表。)

VM_BIND feature introduces an optimization where user can create BO which
is private to a specified VM via I915_GEM_CREATE_EXT_VM_PRIVATE flag during
BO creation. Unlike Shared BOs, these VM private BOs can only be mapped on
the VM they are private to and can't be dma-buf exported.
All private BOs of a VM share the dma-resv object. Hence during each execbuf
submission, they need only one dma-resv fence list updated. Thus, the fast
path (where required mappings are already bound) submission latency is O(1)
w.r.t the number of VM private BOs.
(VM_BIND 特性引入了一项优化：用户可在创建 BO 时通过 I915_GEM_CREATE_EXT_VM_PRIVATE 标志创建对指定 VM 私有的 BO。与共享 BO 不同，这些 VM 私有 BO 只能映射到其所属的 VM，且不能被 dma-buf 导出。一个 VM 的所有私有 BO 共享同一个 dma-resv 对象。因此在每次 execbuf 提交期间，只需更新一个 dma-resv fence 列表。于是，快速路径 (所需映射已绑定) 的提交延迟相对于 VM 私有 BO 的数量是 O(1)。)

VM_BIND locking hierarchy(VM_BIND 锁层次)
------------------------------------------
The locking design here supports the older (execlist based) execbuf mode, the
newer VM_BIND mode, the VM_BIND mode with GPU page faults and possible future
system allocator support (See `Shared Virtual Memory (SVM) support(共享虚拟内存 (SVM) 支持)`_).
The older execbuf mode and the newer VM_BIND mode without page faults manages
residency of backing storage using dma_fence. The VM_BIND mode with page faults
and the system allocator support do not use any dma_fence at all.
(这里的锁设计同时支持旧的 (基于 execlist 的) execbuf 模式、较新的 VM_BIND 模式、带 GPU 缺页的 VM_BIND 模式以及将来可能的系统分配器支持 (见 `Shared Virtual Memory (SVM) support(共享虚拟内存 (SVM) 支持)`_)。旧的 execbuf 模式与不带缺页的较新 VM_BIND 模式使用 dma_fence 来管理后备存储的驻留。带缺页的 VM_BIND 模式与系统分配器支持则完全不使用 dma_fence。)

VM_BIND locking order is as below.
(VM_BIND 的加锁顺序如下。)

1) Lock-A: A vm_bind mutex will protect vm_bind lists. This lock is taken in
   vm_bind/vm_unbind ioctl calls, in the execbuf path and while releasing the
   mapping.
   (1. Lock-A：一个 vm_bind 互斥锁，保护 vm_bind 列表。该锁在 vm_bind/vm_unbind ioctl 调用、execbuf 路径以及释放映射时获取。)

   In future, when GPU page faults are supported, we can potentially use a
   rwsem instead, so that multiple page fault handlers can take the read side
   lock to lookup the mapping and hence can run in parallel.
   The older execbuf mode of binding do not need this lock.
   (将来支持 GPU 缺页后，可以考虑改用 rwsem，让多个缺页处理函数取读锁来查找映射，从而并行运行。旧的 execbuf 绑定模式不需要这把锁。)

2) Lock-B: The object's dma-resv lock will protect i915_vma state and needs to
   be held while binding/unbinding a vma in the async worker and while updating
   dma-resv fence list of an object. Note that private BOs of a VM will all
   share a dma-resv object.
   (2. Lock-B：对象的 dma-resv 锁保护 i915_vma 状态；在异步工作线程中绑定/解绑定 vma 时、以及更新对象的 dma-resv fence 列表时都需要持有它。注意一个 VM 的私有 BO 全部共享一个 dma-resv 对象。)

   The future system allocator support will use the HMM prescribed locking
   instead.
   (将来的系统分配器支持将改用 HMM 规定的锁。)

3) Lock-C: Spinlock/s to protect some of the VM's lists like the list of
   invalidated vmas (due to eviction and userptr invalidation) etc.
   (3. Lock-C：一个或多个自旋锁，保护 VM 的某些列表，如失效 vma 列表 (因逐出与 userptr 失效产生) 等。)

When GPU page faults are supported, the execbuf path do not take any of these
locks. There we will simply smash the new batch buffer address into the ring and
then tell the scheduler run that. The lock taking only happens from the page
fault handler, where we take lock-A in read mode, whichever lock-B we need to
find the backing storage (dma_resv lock for gem objects, and hmm/core mm for
system allocator) and some additional locks (lock-D) for taking care of page
table races. Page fault mode should not need to ever manipulate the vm lists,
so won't ever need lock-C.
(支持 GPU 缺页后，execbuf 路径不再获取上述任何锁。届时只需把新的批次缓冲地址写入环形缓冲区，然后通知调度器运行它。取锁只发生在缺页处理函数中：以读模式取 lock-A，取查找后备存储所需的相应 lock-B (gem 对象用 dma_resv 锁，系统分配器用 hmm/core mm)，以及处理页表竞态的若干附加锁 (lock-D)。缺页模式应该从不需要操作 vm 列表，因此永远不需要 lock-C。)

VM_BIND LRU handling(VM_BIND LRU 处理)
--------------------------------------
We need to ensure VM_BIND mapped objects are properly LRU tagged to avoid
performance degradation. We will also need support for bulk LRU movement of
VM_BIND objects to avoid additional latencies in execbuf path.
(需要确保 VM_BIND 映射的对象被正确打上 LRU 标记以避免性能退化。还需要支持 VM_BIND 对象的批量 LRU 移动，以避免 execbuf 路径出现额外延迟。)

The page table pages are similar to VM_BIND mapped objects (See
`Evictable page table allocations(可逐出的页表分配)`_) and are maintained per VM and needs to
be pinned in memory when VM is made active (ie., upon an execbuf call with
that VM). So, bulk LRU movement of page table pages is also needed.
(页表页与 VM_BIND 映射的对象类似 (见 `Evictable page table allocations(可逐出的页表分配)`_)，按 VM 维护，并且当 VM 被激活时 (即带着该 VM 的 execbuf 调用时) 需要固定在内存中。因此同样需要对页表页做批量 LRU 移动。)

VM_BIND dma_resv usage(VM_BIND 的 dma_resv 用法)
-------------------------------------------------
Fences needs to be added to all VM_BIND mapped objects. During each execbuf
submission, they are added with DMA_RESV_USAGE_BOOKKEEP usage to prevent
over sync (See enum dma_resv_usage). One can override it with either
DMA_RESV_USAGE_READ or DMA_RESV_USAGE_WRITE usage during explicit object
dependency setting.
(fence 需要加入所有 VM_BIND 映射的对象。在每次 execbuf 提交期间，它们以 DMA_RESV_USAGE_BOOKKEEP 用法加入，以防止过度同步 (见 enum dma_resv_usage)。在显式设置对象依赖时，可以用 DMA_RESV_USAGE_READ 或 DMA_RESV_USAGE_WRITE 用法覆盖它。)

Note that DRM_I915_GEM_WAIT and DRM_I915_GEM_BUSY ioctls do not check for
DMA_RESV_USAGE_BOOKKEEP usage and hence should not be used for end of batch
check. Instead, the execbuf3 out fence should be used for end of batch check
(See struct drm_i915_gem_execbuffer3).
(注意 DRM_I915_GEM_WAIT 与 DRM_I915_GEM_BUSY ioctl 不检查 DMA_RESV_USAGE_BOOKKEEP 用法，因此不能用于批次结束检查。应改用 execbuf3 的 out fence 做批次结束检查 (见 struct drm_i915_gem_execbuffer3)。)

Also, in VM_BIND mode, use dma-resv apis for determining object activeness
(See dma_resv_test_signaled() and dma_resv_wait_timeout()) and do not use the
older i915_vma active reference tracking which is deprecated. This should be
easier to get it working with the current TTM backend.
(另外，在 VM_BIND 模式下应使用 dma-resv API 判定对象活跃性 (见 dma_resv_test_signaled() 与 dma_resv_wait_timeout())，不要使用已废弃的旧 i915_vma 活跃引用跟踪。这在与当前 TTM 后端配合时应该更容易实现。)

Mesa use case(Mesa 用例)
------------------------
VM_BIND can potentially reduce the CPU overhead in Mesa (both Vulkan and Iris),
hence improving performance of CPU-bound applications. It also allows us to
implement Vulkan's Sparse Resources. With increasing GPU hardware performance,
reducing CPU overhead becomes more impactful.
(VM_BIND 有望降低 Mesa (Vulkan 与 Iris 皆是) 中的 CPU 开销，从而提升受 CPU 限制应用的性能。它还让我们得以实现 Vulkan 的稀疏资源 (Sparse Resources)。随着 GPU 硬件性能不断提升，降低 CPU 开销变得越来越重要。)


Other VM_BIND use cases(其他 VM_BIND 用例)
===========================================

Long running Compute contexts(长时间运行的计算上下文)
------------------------------------------------------
Usage of dma-fence expects that they complete in reasonable amount of time.
Compute on the other hand can be long running. Hence it is appropriate for
compute to use user/memory fence (See `User/Memory Fence(用户/内存栅栏)`_) and dma-fence usage
must be limited to in-kernel consumption only.
(dma-fence 的使用预期是它们在合理时间内完成。而计算任务可能长时间运行。因此计算上下文适合使用用户/内存栅栏 (见 `User/Memory Fence(用户/内存栅栏)`_)，dma-fence 的使用必须仅限于内核内消费。)

Where GPU page faults are not available, kernel driver upon buffer invalidation
will initiate a suspend (preemption) of long running context, finish the
invalidation, revalidate the BO and then resume the compute context. This is
done by having a per-context preempt fence which is enabled when someone tries
to wait on it and triggers the context preemption.
(在没有 GPU 缺页支持的情况下，内核驱动在缓冲失效时会发起长时间运行上下文的挂起 (抢占)，完成失效处理、重新验证 BO，然后恢复计算上下文。这是通过每个上下文的 preempt fence 实现的：当有人试图等待它时该 fence 被启用，并触发上下文抢占。)

User/Memory Fence(用户/内存栅栏)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
User/Memory fence is a <address, value> pair. To signal the user fence, the
specified value will be written at the specified virtual address and wakeup the
waiting process. User fence can be signaled either by the GPU or kernel async
worker (like upon bind completion). User can wait on a user fence with a new
user fence wait ioctl.
(用户/内存栅栏 (User/Memory fence) 是一个 <地址, 值> 对。发出用户栅栏信号时，会把指定值写到指定虚拟地址并唤醒等待的进程。用户栅栏可由 GPU 或内核异步工作线程 (例如绑定完成时) 发出。用户可以通过新的 user fence wait ioctl 等待用户栅栏。)

Here is some prior work on this:
(这方面的一些先前工作：)

https://patchwork.freedesktop.org/patch/349417/

Low Latency Submission(低延迟提交)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Allows compute UMD to directly submit GPU jobs instead of through execbuf
ioctl. This is made possible by VM_BIND is not being synchronized against
execbuf. VM_BIND allows bind/unbind of mappings required for the directly
submitted jobs.
(允许计算 UMD 直接提交 GPU 作业，而不经过 execbuf ioctl。这得益于 VM_BIND 不与 execbuf 同步。VM_BIND 允许对直接提交作业所需的映射做绑定/解绑定。)

Debugger(调试器)
-----------------
With debug event interface user space process (debugger) is able to keep track
of and act upon resources created by another process (debugged) and attached
to GPU via vm_bind interface.
(通过调试事件接口，用户态进程 (调试器) 能够跟踪另一个进程 (被调试者) 创建并通过 vm_bind 接口挂接到 GPU 的资源，并对其采取相应操作。)

GPU page faults(GPU 缺页)
--------------------------
GPU page faults when supported (in future), will only be supported in the
VM_BIND mode. While both the older execbuf mode and the newer VM_BIND mode of
binding will require using dma-fence to ensure residency, the GPU page faults
mode when supported, will not use any dma-fence as residency is purely managed
by installing and removing/invalidating page table entries.
(GPU 缺页将来受支持时，将只在 VM_BIND 模式下支持。旧的 execbuf 模式与较新的 VM_BIND 绑定模式都需要使用 dma-fence 来确保驻留，而受支持的 GPU 缺页模式将不使用任何 dma-fence，因为驻留完全通过安装与移除/失效页表项来管理。)

Page level hints settings(页级提示设置)
----------------------------------------
VM_BIND allows any hints setting per mapping instead of per BO. Possible hints
include placement and atomicity. Sub-BO level placement hint will be even more
relevant with upcoming GPU on-demand page fault support.
(VM_BIND 允许按映射而非按 BO 设置各种提示。可能的提示包括放置 (placement) 与原子性。随着即将到来的 GPU 按需缺页支持，子 BO 级别的放置提示将更加有意义。)

Page level Cache/CLOS settings(页级 Cache/CLOS 设置)
-----------------------------------------------------
VM_BIND allows cache/CLOS settings per mapping instead of per BO.
(VM_BIND 允许按映射而非按 BO 设置 cache/CLOS。)

Evictable page table allocations(可逐出的页表分配)
---------------------------------------------------
Make pagetable allocations evictable and manage them similar to VM_BIND
mapped objects. Page table pages are similar to persistent mappings of a
VM (difference here are that the page table pages will not have an i915_vma
structure and after swapping pages back in, parent page link needs to be
updated).
(让页表分配变为可逐出 (evictable)，并按与 VM_BIND 映射对象类似的方式管理。页表页类似于一个 VM 的持久映射 (区别在于页表页没有 i915_vma 结构，且把页换回后需要更新父页链接)。)

Shared Virtual Memory (SVM) support(共享虚拟内存 (SVM) 支持)
-------------------------------------------------------------
VM_BIND interface can be used to map system memory directly (without gem BO
abstraction) using the HMM interface. SVM is only supported with GPU page
faults enabled.
(VM_BIND 接口可用于通过 HMM 接口直接映射系统内存 (不经过 gem BO 抽象)。SVM 只在启用 GPU 缺页时受支持。)

VM_BIND UAPI(VM_BIND UAPI)
===========================

.. kernel-doc:: Documentation/gpu/rfc/i915_vm_bind.h
