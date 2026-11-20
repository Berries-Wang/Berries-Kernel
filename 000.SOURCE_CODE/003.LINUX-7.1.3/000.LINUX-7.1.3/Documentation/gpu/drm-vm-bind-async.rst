.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

==================================
Asynchronous VM_BIND(异步 VM_BIND)
==================================

Nomenclature:(术语)
===================

* ``VRAM``: On-device memory. Sometimes referred to as device local memory.
  (``VRAM``：片上内存。有时也称为设备本地内存（device local memory）。)

* ``gpu_vm``: A virtual GPU address space. Typically per process, but
  can be shared by multiple processes.
  (``gpu_vm``：虚拟 GPU 地址空间。通常每个进程一个，但也可以由多个进程共享。)

* ``VM_BIND``: An operation or a list of operations to modify a gpu_vm using
  an IOCTL. The operations include mapping and unmapping system- or
  VRAM memory.
  (``VM_BIND``：通过 IOCTL 修改 gpu_vm 的一个操作或一组操作。操作包括映射与取消映射系统内存或 VRAM 内存。)

* ``syncobj``: A container that abstracts synchronization objects. The
  synchronization objects can be either generic, like dma-fences or
  driver specific. A syncobj typically indicates the type of the
  underlying synchronization object.
  (``syncobj``：对同步对象进行抽象的容器。同步对象可以是通用的（如 dma-fence），也可以是驱动专属的。syncobj 通常指明底层同步对象的类型。)

* ``in-syncobj``: Argument to a VM_BIND IOCTL, the VM_BIND operation waits
  for these before starting.
  (``in-syncobj``：VM_BIND IOCTL 的参数，VM_BIND 操作在开始之前等待它们。)

* ``out-syncobj``: Argument to a VM_BIND_IOCTL, the VM_BIND operation
  signals these when the bind operation is complete.
  (``out-syncobj``：VM_BIND IOCTL 的参数，绑定操作完成时 VM_BIND 操作对它们发信号。)

* ``dma-fence``: A cross-driver synchronization object. A basic
  understanding of dma-fences is required to digest this
  document. Please refer to the ``DMA Fences`` section of the
  :doc:`dma-buf doc </driver-api/dma-buf>`.
  (``dma-fence``：跨驱动同步对象。理解本文档需要对 dma-fence 有基本的了解。请参阅 :doc:`dma-buf doc </driver-api/dma-buf>` 的 ``DMA Fences`` 一节。)

* ``memory fence``: A synchronization object, different from a dma-fence.
  A memory fence uses the value of a specified memory location to determine
  signaled status. A memory fence can be awaited and signaled by both
  the GPU and CPU. Memory fences are sometimes referred to as
  user-fences, userspace-fences or gpu futexes and do not necessarily obey
  the dma-fence rule of signaling within a "reasonable amount of time".
  The kernel should thus avoid waiting for memory fences with locks held.
  (``memory fence``：与 dma-fence 不同的同步对象。内存栅栏（memory fence）用指定内存位置的值来判定已发信号状态。GPU 与 CPU 都可以等待和发出内存栅栏。内存栅栏有时被称为 user-fence、userspace-fence 或 gpu futex，并且不一定遵守 dma-fence"在合理时间内发信号"的规则。因此内核应避免在持有锁的情况下等待内存栅栏。)

* ``long-running workload``: A workload that may take more than the
  current stipulated dma-fence maximum signal delay to complete and
  which therefore needs to set the gpu_vm or the GPU execution context in
  a certain mode that disallows completion dma-fences.
  (``long-running workload``：可能超过当前规定的 dma-fence 最大发信号延迟才能完成的工作负载，因此需要把 gpu_vm 或 GPU 执行上下文设置为不允许完成 dma-fence 的特定模式。)

* ``exec function``: An exec function is a function that revalidates all
  affected gpu_vmas, submits a GPU command batch and registers the
  dma_fence representing the GPU command's activity with all affected
  dma_resvs. For completeness, although not covered by this document,
  it's worth mentioning that an exec function may also be the
  revalidation worker that is used by some drivers in compute /
  long-running mode.
  (``exec function``：exec 函数是这样一个函数：重新验证所有受影响的 gpu_vma，提交 GPU 命令批处理，并把代表 GPU 命令活动的 dma_fence 注册到所有受影响的 dma_resv 上。为完整起见，尽管本文档不涉及，值得一提的是 exec 函数也可以是一些驱动在计算/长时间运行模式下使用的重新验证工作线程。)

* ``bind context``: A context identifier used for the VM_BIND
  operation. VM_BIND operations that use the same bind context can be
  assumed, where it matters, to complete in order of submission. No such
  assumptions can be made for VM_BIND operations using separate bind contexts.
  (``bind context``：VM_BIND 操作使用的上下文标识符。可以认为（在重要的场合）使用同一绑定上下文的 VM_BIND 操作按提交顺序完成；对使用不同绑定上下文的 VM_BIND 操作则不能做这种假设。)

* ``UMD``: User-mode driver.
  (``UMD``：用户态驱动。)

* ``KMD``: Kernel-mode driver.
  (``KMD``：内核态驱动。)


Synchronous / Asynchronous VM_BIND operation(同步 / 异步 VM_BIND 操作)
======================================================================

Synchronous VM_BIND(同步 VM_BIND)
_________________________________
With Synchronous VM_BIND, the VM_BIND operations all complete before the
IOCTL returns. A synchronous VM_BIND takes neither in-fences nor
out-fences. Synchronous VM_BIND may block and wait for GPU operations;
for example swap-in or clearing, or even previous binds.
(同步 VM_BIND 下，所有 VM_BIND 操作都在 IOCTL 返回之前完成。同步 VM_BIND 既不接受 in-fence 也不接受 out-fence。同步 VM_BIND 可能阻塞并等待 GPU 操作——例如换入（swap-in）、清空，甚至之前的绑定。)

Asynchronous VM_BIND(异步 VM_BIND)
__________________________________
Asynchronous VM_BIND accepts both in-syncobjs and out-syncobjs. While the
IOCTL may return immediately, the VM_BIND operations wait for the in-syncobjs
before modifying the GPU page-tables, and signal the out-syncobjs when
the modification is done in the sense that the next exec function that
awaits for the out-syncobjs will see the change. Errors are reported
synchronously.
In low-memory situations the implementation may block, performing the
VM_BIND synchronously, because there might not be enough memory
immediately available for preparing the asynchronous operation.
(异步 VM_BIND 同时接受 in-syncobj 与 out-syncobj。虽然 IOCTL 可能立即返回，但 VM_BIND 操作会先等待 in-syncobj 才修改 GPU 页表；当修改"完成"时——即下一个等待这些 out-syncobj 的 exec 函数能看到变更时——对 out-syncobj 发信号。错误是同步上报的。在内存不足的情况下，实现可能阻塞、以同步方式执行 VM_BIND，因为可能没有足够的内存立即可用于准备异步操作。)

If the VM_BIND IOCTL takes a list or an array of operations as an argument,
the in-syncobjs needs to signal before the first operation starts to
execute, and the out-syncobjs signal after the last operation
completes. Operations in the operation list can be assumed, where it
matters, to complete in order.
(如果 VM_BIND IOCTL 以一组操作列表或数组为参数，则 in-syncobj 需要在第一个操作开始执行之前发信号，而 out-syncobj 在最后一个操作完成后发信号。在重要的场合，可以认为操作列表中的操作按顺序完成。)

Since asynchronous VM_BIND operations may use dma-fences embedded in
out-syncobjs and internally in KMD to signal bind completion,  any
memory fences given as VM_BIND in-fences need to be awaited
synchronously before the VM_BIND ioctl returns, since dma-fences,
required to signal in a reasonable amount of time, can never be made
to depend on memory fences that don't have such a restriction.
(由于异步 VM_BIND 操作可能使用内嵌于 out-syncobj 及 KMD 内部的 dma-fence 来为绑定完成发信号，作为 VM_BIND in-fence 给出的任何内存栅栏都必须在 VM_BIND ioctl 返回之前被同步等待——因为 dma-fence 被要求在合理时间内发信号，绝不能让它依赖于没有这种限制的内存栅栏。)

The purpose of an Asynchronous VM_BIND operation is for user-mode
drivers to be able to pipeline interleaved gpu_vm modifications and
exec functions. For long-running workloads, such pipelining of a bind
operation is not allowed and any in-fences need to be awaited
synchronously. The reason for this is twofold. First, any memory
fences gated by a long-running workload and used as in-syncobjs for the
VM_BIND operation will need to be awaited synchronously anyway (see
above). Second, any dma-fences used as in-syncobjs for VM_BIND
operations for long-running workloads will not allow for pipelining
anyway since long-running workloads don't allow for dma-fences as
out-syncobjs, so while theoretically possible the use of them is
questionable and should be rejected until there is a valuable use-case.
Note that this is not a limitation imposed by dma-fence rules, but
rather a limitation imposed to keep KMD implementation simple. It does
not affect using dma-fences as dependencies for the long-running
workload itself, which is allowed by dma-fence rules, but rather for
the VM_BIND operation only.
(异步 VM_BIND 操作的目的是让用户态驱动能够把交错的 gpu_vm 修改与 exec 函数流水线化。对长时间运行的工作负载，不允许这种绑定操作的流水线化，任何 in-fence 都必须同步等待。原因有二。其一，由长时间运行工作负载门控、用作 VM_BIND 操作 in-syncobj 的任何内存栅栏反正都需要同步等待（见上文）。其二，对长时间运行工作负载的 VM_BIND 操作而言，用作 in-syncobj 的任何 dma-fence 反正都无法流水线化——因为长时间运行的工作负载不允许把 dma-fence 用作 out-syncobj——所以尽管理论上可行，在出现有价值的用例之前，这种用法应被拒绝。注意，这不是 dma-fence 规则施加的限制，而是为保持 KMD 实现简单而施加的限制。它并不影响把 dma-fence 用作长时间运行工作负载自身的依赖（dma-fence 规则允许这样做），而只影响 VM_BIND 操作。)

An asynchronous VM_BIND operation may take substantial time to
complete and signal the out_fence. In particular if the operation is
deeply pipelined behind other VM_BIND operations and workloads
submitted using exec functions. In that case, UMD might want to avoid a
subsequent VM_BIND operation to be queued behind the first one if
there are no explicit dependencies. In order to circumvent such a queue-up, a
VM_BIND implementation may allow for VM_BIND contexts to be
created. For each context, VM_BIND operations will be guaranteed to
complete in the order they were submitted, but that is not the case
for VM_BIND operations executing on separate VM_BIND contexts. Instead
KMD will attempt to execute such VM_BIND operations in parallel but
leaving no guarantee that they will actually be executed in
parallel. There may be internal implicit dependencies that only KMD knows
about, for example page-table structure changes. A way to attempt
to avoid such internal dependencies is to have different VM_BIND
contexts use separate regions of a VM.
(异步 VM_BIND 操作可能需要相当长的时间才能完成并对 out_fence 发信号——特别是当该操作被深度流水线化、排在其他 VM_BIND 操作和使用 exec 函数提交的工作负载之后时。这种情况下，如果没有显式依赖，UMD 可能不希望后续的 VM_BIND 操作排在第一个之后排队。为了避免这种排队，VM_BIND 实现可以允许创建 VM_BIND 上下文。对每个上下文，VM_BIND 操作保证按提交顺序完成；但在不同 VM_BIND 上下文上执行的 VM_BIND 操作则没有这种保证。此时 KMD 会尝试并行执行这类 VM_BIND 操作，但不保证它们真的并行执行。可能存在只有 KMD 知道的内部隐式依赖，例如页表结构变更。尝试避免这类内部依赖的一个办法，是让不同的 VM_BIND 上下文使用 VM 的不同区域。)

Also for VM_BINDS for long-running gpu_vms the user-mode driver should typically
select memory fences as out-fences since that gives greater flexibility for
the kernel mode driver to inject other operations into the bind /
unbind operations. Like for example inserting breakpoints into batch
buffers. The workload execution can then easily be pipelined behind
the bind completion using the memory out-fence as the signal condition
for a GPU semaphore embedded by UMD in the workload.
(对于长时间运行 gpu_vm 的 VM_BIND，用户态驱动通常也应选择内存栅栏作为 out-fence，因为这给了内核态驱动更大的灵活性，可以向绑定/解绑操作中注入其他操作，例如向批处理缓冲区插入断点。这样，工作负载的执行可以轻松地流水线化到绑定完成之后——用内存 out-fence 作为 UMD 内嵌于工作负载中的 GPU 信号量的发信号条件。)

There is no difference in the operations supported or in
multi-operation support between asynchronous VM_BIND and synchronous VM_BIND.
(异步 VM_BIND 与同步 VM_BIND 在所支持的操作以及多操作支持方面没有区别。)

Multi-operation VM_BIND IOCTL error handling and interrupts(多操作 VM_BIND IOCTL 的错误处理与中断)
==================================================================================================

The VM_BIND operations of the IOCTL may error for various reasons, for
example due to lack of resources to complete and due to interrupted
waits.
In these situations UMD should preferably restart the IOCTL after
taking suitable action.
If UMD has over-committed a memory resource, an -ENOSPC error will be
returned, and UMD may then unbind resources that are not used at the
moment and rerun the IOCTL. On -EINTR, UMD should simply rerun the
IOCTL and on -ENOMEM user-space may either attempt to free known
system memory resources or fail. In case of UMD deciding to fail a
bind operation, due to an error return, no additional action is needed
to clean up the failed operation, and the VM is left in the same state
as it was before the failing IOCTL.
Unbind operations are guaranteed not to return any errors due to
resource constraints, but may return errors due to, for example,
invalid arguments or the gpu_vm being banned.
In the case an unexpected error happens during the asynchronous bind
process, the gpu_vm will be banned, and attempts to use it after banning
will return -ENOENT.
(IOCTL 的 VM_BIND 操作可能因各种原因出错，例如缺少完成所需的资源、等待被中断等。这些情况下，UMD 最好在采取适当措施后重启 IOCTL。如果 UMD 过度占用（over-commit）了内存资源，将返回 -ENOSPC 错误，UMD 随后可以解绑当前未使用的资源并重新运行 IOCTL。遇到 -EINTR，UMD 只需重新运行 IOCTL；遇到 -ENOMEM，用户空间可以尝试释放已知的系统内存资源，或者失败。如果 UMD 因错误返回而决定让绑定操作失败，则不需要额外的清理动作，VM 将保持与失败的 IOCTL 之前相同的状态。解绑操作保证不会因资源约束而返回错误，但可能因无效参数或 gpu_vm 被封禁（banned）等原因返回错误。如果异步绑定过程中发生意外错误，gpu_vm 将被封锁，封锁之后试图使用它会返回 -ENOENT。)

Example: The Xe VM_BIND uAPI(示例：Xe 的 VM_BIND uAPI)
======================================================

Starting with the VM_BIND operation struct, the IOCTL call can take
zero, one or many such operations. A zero number means only the
synchronization part of the IOCTL is carried out: an asynchronous
VM_BIND updates the syncobjects, whereas a sync VM_BIND waits for the
implicit dependencies to be fulfilled.
(先看 VM_BIND 操作结构体：IOCTL 调用可以接受零个、一个或许多这样的操作。零个意味着只执行 IOCTL 的同步部分：异步 VM_BIND 更新 syncobject，而同步 VM_BIND 等待隐式依赖得到满足。)

.. code-block:: c

   struct drm_xe_vm_bind_op {
	/**
	 * @obj: GEM object to operate on, MBZ for MAP_USERPTR, MBZ for UNMAP
	 */
	__u32 obj;

	/** @pad: MBZ */
	__u32 pad;

	union {
		/**
		 * @obj_offset: Offset into the object for MAP.
		 */
		__u64 obj_offset;

		/** @userptr: user virtual address for MAP_USERPTR */
		__u64 userptr;
	};

	/**
	 * @range: Number of bytes from the object to bind to addr, MBZ for UNMAP_ALL
	 */
	__u64 range;

	/** @addr: Address to operate on, MBZ for UNMAP_ALL */
	__u64 addr;

	/**
	 * @tile_mask: Mask for which tiles to create binds for, 0 == All tiles,
	 * only applies to creating new VMAs
	 */
	__u64 tile_mask;

       /* Map (parts of) an object into the GPU virtual address range.
    #define XE_VM_BIND_OP_MAP		0x0
        /* Unmap a GPU virtual address range */
    #define XE_VM_BIND_OP_UNMAP		0x1
        /*
	 * Map a CPU virtual address range into a GPU virtual
	 * address range.
	 */
    #define XE_VM_BIND_OP_MAP_USERPTR	0x2
        /* Unmap a gem object from the VM. */
    #define XE_VM_BIND_OP_UNMAP_ALL	0x3
        /*
	 * Make the backing memory of an address range resident if
	 * possible. Note that this doesn't pin backing memory.
	 */
    #define XE_VM_BIND_OP_PREFETCH	0x4

        /* Make the GPU map readonly. */
    #define XE_VM_BIND_FLAG_READONLY	(0x1 << 16)
	/*
	 * Valid on a faulting VM only, do the MAP operation immediately rather
	 * than deferring the MAP to the page fault handler.
	 */
    #define XE_VM_BIND_FLAG_IMMEDIATE	(0x1 << 17)
	/*
	 * When the NULL flag is set, the page tables are setup with a special
	 * bit which indicates writes are dropped and all reads return zero.  In
	 * the future, the NULL flags will only be valid for XE_VM_BIND_OP_MAP
	 * operations, the BO handle MBZ, and the BO offset MBZ. This flag is
	 * intended to implement VK sparse bindings.
	 */
    #define XE_VM_BIND_FLAG_NULL	(0x1 << 18)
	/** @op: Operation to perform (lower 16 bits) and flags (upper 16 bits) */
	__u32 op;

	/** @mem_region: Memory region to prefetch VMA to, instance not a mask */
	__u32 region;

	/** @reserved: Reserved */
	__u64 reserved[2];
   };


The VM_BIND IOCTL argument itself, looks like follows. Note that for
synchronous VM_BIND, the num_syncs and syncs fields must be zero. Here
the ``exec_queue_id`` field is the VM_BIND context discussed previously
that is used to facilitate out-of-order VM_BINDs.
(VM_BIND IOCTL 参数本身如下所示。注意，对同步 VM_BIND，num_syncs 与 syncs 字段必须为 0。这里的 ``exec_queue_id`` 字段就是前文讨论过的 VM_BIND 上下文，用于支持乱序的 VM_BIND。)

.. code-block:: c

    struct drm_xe_vm_bind {
	/** @extensions: Pointer to the first extension struct, if any */
	__u64 extensions;

	/** @vm_id: The ID of the VM to bind to */
	__u32 vm_id;

	/**
	 * @exec_queue_id: exec_queue_id, must be of class DRM_XE_ENGINE_CLASS_VM_BIND
	 * and exec queue must have same vm_id. If zero, the default VM bind engine
	 * is used.
	 */
	__u32 exec_queue_id;

	/** @num_binds: number of binds in this IOCTL */
	__u32 num_binds;

        /* If set, perform an async VM_BIND, if clear a sync VM_BIND */
    #define XE_VM_BIND_IOCTL_FLAG_ASYNC	(0x1 << 0)

	/** @flag: Flags controlling all operations in this ioctl. */
	__u32 flags;

	union {
		/** @bind: used if num_binds == 1 */
		struct drm_xe_vm_bind_op bind;

		/**
		 * @vector_of_binds: userptr to array of struct
		 * drm_xe_vm_bind_op if num_binds > 1
		 */
		__u64 vector_of_binds;
	};

	/** @num_syncs: amount of syncs to wait for or to signal on completion. */
	__u32 num_syncs;

	/** @pad2: MBZ */
	__u32 pad2;

	/** @syncs: pointer to struct drm_xe_sync array */
	__u64 syncs;

	/** @reserved: Reserved */
	__u64 reserved[2];
    };
