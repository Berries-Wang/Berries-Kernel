.. _memory_allocation:

=====================================
Memory Allocation Guide(内存分配指南)
=====================================

Linux provides a variety of APIs for memory allocation. You can
allocate small chunks using `kmalloc` or `kmem_cache_alloc` families,
large virtually contiguous areas using `vmalloc` and its derivatives,
or you can directly request pages from the page allocator with
`alloc_pages`. It is also possible to use more specialized allocators,
for instance `cma_alloc` or `zs_malloc`.
(Linux 提供多种内存分配 API。你可以用 `kmalloc` 或 `kmem_cache_alloc` 族分配小块内存，用 `vmalloc` 及其衍生函数分配大的虚拟连续区域，也可以用 `alloc_pages` 直接向页分配器请求页。还可以使用更专门的分配器，例如 `cma_alloc` 或 `zs_malloc`。)

Most of the memory allocation APIs use GFP flags to express how that
memory should be allocated. The GFP acronym stands for "get free
pages", the underlying memory allocation function.
(大多数内存分配 API 使用 GFP 标志来表达内存应如何分配。GFP 是 "get free pages"(获取空闲页)的缩写，即底层的内存分配函数。)

Diversity of the allocation APIs combined with the numerous GFP flags
makes the question "How should I allocate memory?" not that easy to
answer, although very likely you should use

::

  kzalloc(<size>, GFP_KERNEL);

(分配 API 的多样性加上众多可用的 GFP 标志，使"我该如何分配内存？"这个问题不那么容易回答，不过你很可能应该使用：)

Of course there are cases when other allocation APIs and different GFP
flags must be used.
(当然，也存在必须使用其他分配 API 和不同 GFP 标志的情形。)

Get Free Page flags(Get Free Page 标志)
=======================================

The GFP flags control the allocators behavior. They tell what memory
zones can be used, how hard the allocator should try to find free
memory, whether the memory can be accessed by the userspace etc. The
:ref:`Documentation/core-api/mm-api.rst <mm-api-gfp-flags>` provides
reference documentation for the GFP flags and their combinations and
here we briefly outline their recommended usage:
(GFP 标志控制分配器的行为。它们规定可以使用哪些内存 zone、分配器应以多大的努力去寻找空闲内存、该内存是否可被用户空间访问等等。:ref:`Documentation/core-api/mm-api.rst <mm-api-gfp-flags>` 提供了 GFP 标志及其组合的参考文档，这里我们简要概述其推荐用法：)

  * Most of the time ``GFP_KERNEL`` is what you need. Memory for the
    kernel data structures, DMAable memory, inode cache, all these and
    many other allocations types can use ``GFP_KERNEL``. Note, that
    using ``GFP_KERNEL`` implies ``GFP_RECLAIM``, which means that
    direct reclaim may be triggered under memory pressure; the calling
    context must be allowed to sleep.
    (大多数情况下 ``GFP_KERNEL`` 就是你所需要的。内核数据结构的内存、可 DMA 的内存、inode 缓存——这些以及许多其他分配类型都可以使用 ``GFP_KERNEL``。注意，使用 ``GFP_KERNEL`` 隐含 ``GFP_RECLAIM``，这意味着在内存压力下可能触发直接回收；调用上下文必须允许睡眠。)
  * If the allocation is performed from an atomic context, e.g interrupt
    handler, use ``GFP_NOWAIT``. This flag prevents direct reclaim and
    IO or filesystem operations. Consequently, under memory pressure
    ``GFP_NOWAIT`` allocation is likely to fail. Users of this flag need
    to provide a suitable fallback to cope with such failures where
    appropriate.
    (如果从原子上下文(例如中断处理程序)进行分配，应使用 ``GFP_NOWAIT``。该标志阻止直接回收以及 IO 或文件系统操作。因此，在内存压力下 ``GFP_NOWAIT`` 分配很可能失败。使用此标志的人需要在合适之处提供适当的回退方案来应对这类失败。)
  * If you think that accessing memory reserves is justified and the kernel
    will be stressed unless allocation succeeds, you may use ``GFP_ATOMIC``.
    (如果你认为访问内存储备是正当的，且分配不成功内核就会陷入困境，可以使用 ``GFP_ATOMIC``。)
  * Untrusted allocations triggered from userspace should be a subject
    of kmem accounting and must have ``__GFP_ACCOUNT`` bit set. There
    is the handy ``GFP_KERNEL_ACCOUNT`` shortcut for ``GFP_KERNEL``
    allocations that should be accounted.
    (由用户空间触发的不可信分配应纳入 kmem 计量，必须设置 ``__GFP_ACCOUNT`` 位。对于需要计量的 ``GFP_KERNEL`` 分配，有个便捷的 ``GFP_KERNEL_ACCOUNT`` 简写。)
  * Userspace allocations should use either of the ``GFP_USER``,
    ``GFP_HIGHUSER`` or ``GFP_HIGHUSER_MOVABLE`` flags. The longer
    the flag name the less restrictive it is.
    (用户空间分配应使用 ``GFP_USER``、``GFP_HIGHUSER`` 或 ``GFP_HIGHUSER_MOVABLE`` 之一。标志名越长，限制越少。)

    ``GFP_HIGHUSER_MOVABLE`` does not require that allocated memory
    will be directly accessible by the kernel and implies that the
    data is movable.
    (``GFP_HIGHUSER_MOVABLE`` 不要求所分配内存可被内核直接访问，并意味着数据是可移动的。)

    ``GFP_HIGHUSER`` means that the allocated memory is not movable,
    but it is not required to be directly accessible by the kernel. An
    example may be a hardware allocation that maps data directly into
    userspace but has no addressing limitations.
    (``GFP_HIGHUSER`` 表示所分配内存不可移动，但不要求可被内核直接访问。一个例子是把数据直接映射到用户空间但无寻址限制的硬件分配。)

    ``GFP_USER`` means that the allocated memory is not movable and it
    must be directly accessible by the kernel.
    (``GFP_USER`` 表示所分配内存不可移动，且必须可被内核直接访问。)

You may notice that quite a few allocations in the existing code
specify ``GFP_NOIO`` or ``GFP_NOFS``. Historically, they were used to
prevent recursion deadlocks caused by direct memory reclaim calling
back into the FS or IO paths and blocking on already held
resources. Since 4.12 the preferred way to address this issue is to
use new scope APIs described in
:ref:`Documentation/core-api/gfp_mask-from-fs-io.rst <gfp_mask_from_fs_io>`.
(你可能注意到现有代码中有不少分配指定了 ``GFP_NOIO`` 或 ``GFP_NOFS``。历史上，它们用于防止直接内存回收回调进入 FS 或 IO 路径并阻塞在已持有资源上所导致的递归死锁。自 4.12 起，解决这一问题的首选方式是使用 :ref:`Documentation/core-api/gfp_mask-from-fs-io.rst <gfp_mask_from_fs_io>` 中描述的新作用域 API。)

Other legacy GFP flags are ``GFP_DMA`` and ``GFP_DMA32``. They are
used to ensure that the allocated memory is accessible by hardware
with limited addressing capabilities. So unless you are writing a
driver for a device with such restrictions, avoid using these flags.
And even with hardware with restrictions it is preferable to use
`dma_alloc*` APIs.
(其他遗留的 GFP 标志有 ``GFP_DMA`` 与 ``GFP_DMA32``。它们用于确保所分配内存可被寻址能力受限的硬件访问。因此，除非你正在为有此类限制的设备编写驱动，否则应避免使用这些标志。即使硬件确有限制，也更推荐使用 `dma_alloc*` API。)

GFP flags and reclaim behavior(GFP 标志与回收行为)
--------------------------------------------------
Memory allocations may trigger direct or background reclaim and it is
useful to understand how hard the page allocator will try to satisfy that
or another request.
(内存分配可能触发直接回收或后台回收，理解页分配器会以多大努力去满足这个或那个请求是很有用的。)

  * ``GFP_KERNEL & ~__GFP_RECLAIM`` - optimistic allocation without _any_
    attempt to free memory at all. The most light weight mode which even
    doesn't kick the background reclaim. Should be used carefully because it
    might deplete the memory and the next user might hit the more aggressive
    reclaim.
    (``GFP_KERNEL & ~__GFP_RECLAIM`` - 乐观分配，完全*不做*任何释放内存的尝试。最轻量的模式，甚至不会触发后台回收。应谨慎使用，因为它可能耗尽内存，下一个使用者可能会撞上更激烈的回收。)

  * ``GFP_KERNEL & ~__GFP_DIRECT_RECLAIM`` (or ``GFP_NOWAIT``)- optimistic
    allocation without any attempt to free memory from the current
    context but can wake kswapd to reclaim memory if the zone is below
    the low watermark. Can be used from either atomic contexts or when
    the request is a performance optimization and there is another
    fallback for a slow path.
    (``GFP_KERNEL & ~__GFP_DIRECT_RECLAIM``(即 ``GFP_NOWAIT``)- 乐观分配，不在当前上下文中做任何释放内存的尝试，但如果 zone 低于低水位线，可以唤醒 kswapd 回收内存。可用于原子上下文，或该请求是性能优化且慢路径另有回退方案的情形。)

  * ``(GFP_KERNEL|__GFP_HIGH) & ~__GFP_DIRECT_RECLAIM`` (aka ``GFP_ATOMIC``) -
    non sleeping allocation with an expensive fallback so it can access
    some portion of memory reserves. Usually used from interrupt/bottom-half
    context with an expensive slow path fallback.
    (``(GFP_KERNEL|__GFP_HIGH) & ~__GFP_DIRECT_RECLAIM``(即 ``GFP_ATOMIC``)- 不睡眠的分配，带有昂贵的回退手段，因此可以访问一部分内储存备。通常在中断/下半部上下文中使用，其慢路径回退代价高昂。)

  * ``GFP_KERNEL`` - both background and direct reclaim are allowed and the
    **default** page allocator behavior is used. That means that not costly
    allocation requests are basically no-fail but there is no guarantee of
    that behavior so failures have to be checked properly by callers
    (e.g. OOM killer victim is allowed to fail currently).
    (``GFP_KERNEL`` - 允许后台回收与直接回收，使用**默认**页分配器行为。这意味着代价不高的分配请求基本上不会失败，但这一行为没有保证，因此调用者必须正确检查失败(例如当前 OOM killer 的受害者允许分配失败)。)

  * ``GFP_KERNEL | __GFP_NORETRY`` - overrides the default allocator behavior
    and all allocation requests fail early rather than cause disruptive
    reclaim (one round of reclaim in this implementation). The OOM killer
    is not invoked.
    (``GFP_KERNEL | __GFP_NORETRY`` - 覆盖默认分配器行为，所有分配请求尽早失败，而不是引发破坏性回收(在本实现中为回收一轮)。不会调用 OOM killer。)

  * ``GFP_KERNEL | __GFP_RETRY_MAYFAIL`` - overrides the default allocator
    behavior and all allocation requests try really hard. The request
    will fail if the reclaim cannot make any progress. The OOM killer
    won't be triggered.
    (``GFP_KERNEL | __GFP_RETRY_MAYFAIL`` - 覆盖默认分配器行为，所有分配请求都会非常努力地尝试。如果回收无法取得任何进展，请求将失败。不会触发 OOM killer。)

  * ``GFP_KERNEL | __GFP_NOFAIL`` - overrides the default allocator behavior
    and all allocation requests will loop endlessly until they succeed.
    This might be really dangerous especially for larger orders.
    (``GFP_KERNEL | __GFP_NOFAIL`` - 覆盖默认分配器行为，所有分配请求将无休止地循环直到成功。这可能非常危险，对更大的 order 尤其如此。)

Selecting memory allocator(选择内存分配器)
==========================================

The most straightforward way to allocate memory is to use a function
from the kmalloc() family. And, to be on the safe side it's best to use
routines that set memory to zero, like kzalloc(). If you need to
allocate memory for an array, there are kmalloc_array() and kcalloc()
helpers. The helpers struct_size(), array_size() and array3_size() can
be used to safely calculate object sizes without overflowing.
(最直接的内存分配方式是使用 kmalloc() 族中的函数。为保险起见，最好使用把内存清零的例程，如 kzalloc()。如果需要为数组分配内存，有 kmalloc_array() 与 kcalloc() 辅助函数。辅助函数 struct_size()、array_size() 与 array3_size() 可用于安全地计算对象大小而不会溢出。)

The maximal size of a chunk that can be allocated with `kmalloc` is
limited. The actual limit depends on the hardware and the kernel
configuration, but it is a good practice to use `kmalloc` for objects
smaller than page size.
(用 `kmalloc` 可分配的块的最大大小是受限的。实际限制取决于硬件与内核配置，但对小于页大小的对象使用 `kmalloc` 是个好习惯。)

The address of a chunk allocated with `kmalloc` is aligned to at least
ARCH_KMALLOC_MINALIGN bytes. For sizes which are a power of two, the
alignment is also guaranteed to be at least the respective size. For other
sizes, the alignment is guaranteed to be at least the largest power-of-two
divisor of the size.
(用 `kmalloc` 分配的块的地址至少按 ARCH_KMALLOC_MINALIGN 字节对齐。对于 2 的幂次的大小，对齐还保证至少为该大小本身。对其他大小，对齐保证至少为该大小的最大 2 的幂次因子。)

Chunks allocated with kmalloc() can be resized with krealloc(). Similarly
to kmalloc_array(): a helper for resizing arrays is provided in the form of
krealloc_array().
(用 kmalloc() 分配的块可以用 krealloc() 调整大小。与 kmalloc_array() 类似：调整数组大小的辅助函数是 krealloc_array()。)

For large allocations you can use vmalloc() and vzalloc(), or directly
request pages from the page allocator. The memory allocated by `vmalloc`
and related functions is not physically contiguous.
(对于大分配，可以使用 vmalloc() 与 vzalloc()，或直接向页分配器请求页。由 `vmalloc` 及相关函数分配的内存不是物理连续的。)

If you are not sure whether the allocation size is too large for
`kmalloc`, it is possible to use kvmalloc() and its derivatives. It will
try to allocate memory with `kmalloc` and if the allocation fails it
will be retried with `vmalloc`. There are restrictions on which GFP
flags can be used with `kvmalloc`; please see kvmalloc_node() reference
documentation. Note that `kvmalloc` may return memory that is not
physically contiguous.
(如果不确定分配大小对 `kmalloc` 而言是否过大，可以使用 kvmalloc() 及其衍生函数。它会先尝试用 `kmalloc` 分配内存，若失败则改用 `vmalloc` 重试。`kvmalloc` 对可用的 GFP 标志有限制；请参阅 kvmalloc_node() 的参考文档。注意，`kvmalloc` 返回的内存可能不是物理连续的。)

If you need to allocate many identical objects you can use the slab
cache allocator. The cache should be set up with kmem_cache_create() or
kmem_cache_create_usercopy() before it can be used. The second function
should be used if a part of the cache might be copied to the userspace.
After the cache is created kmem_cache_alloc() and its convenience
wrappers can allocate memory from that cache.
(如果需要分配许多相同对象，可以使用 slab 缓存分配器。缓存应先用 kmem_cache_create() 或 kmem_cache_create_usercopy() 建立才能使用。如果缓存的一部分可能被复制到用户空间，应使用第二个函数。缓存创建之后，kmem_cache_alloc() 及其便捷包装函数即可从该缓存分配内存。)

When the allocated memory is no longer needed it must be freed.
(当分配的内存不再需要时，必须将其释放。)

Objects allocated by `kmalloc` can be freed by `kfree` or `kvfree`. Objects
allocated by `kmem_cache_alloc` can be freed with `kmem_cache_free`, `kfree`
or `kvfree`, where the latter two might be more convenient thanks to not
needing the kmem_cache pointer.
(由 `kmalloc` 分配的对象可以用 `kfree` 或 `kvfree` 释放。由 `kmem_cache_alloc` 分配的对象可以用 `kmem_cache_free`、`kfree` 或 `kvfree` 释放，其中后两个因为不需要 kmem_cache 指针而可能更方便。)

The same rules apply to _bulk and _rcu flavors of freeing functions.
(同样的规则适用于释放函数的 _bulk 与 _rcu 变体。)

Memory allocated by `vmalloc` can be freed with `vfree` or `kvfree`.
Memory allocated by `kvmalloc` can be freed with `kvfree`.
Caches created by `kmem_cache_create` should be freed with
`kmem_cache_destroy` only after freeing all the allocated objects first.
(由 `vmalloc` 分配的内存可以用 `vfree` 或 `kvfree` 释放。由 `kvmalloc` 分配的内存可以用 `kvfree` 释放。由 `kmem_cache_create` 创建的缓存，只有在先释放所有已分配对象之后，才应使用 `kmem_cache_destroy` 释放。)
