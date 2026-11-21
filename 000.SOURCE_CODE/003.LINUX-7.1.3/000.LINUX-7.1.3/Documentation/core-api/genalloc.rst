=======================================================
The genalloc/genpool subsystem(genalloc/genpool 子系统)
=======================================================

There are a number of memory-allocation subsystems in the kernel, each
aimed at a specific need.  Sometimes, however, a kernel developer needs to
implement a new allocator for a specific range of special-purpose memory;
often that memory is located on a device somewhere.  The author of the
driver for that device can certainly write a little allocator to get the
job done, but that is the way to fill the kernel with dozens of poorly
tested allocators.  Back in 2005, Jes Sorensen lifted one of those
allocators from the sym53c8xx_2 driver and posted_ it as a generic module
for the creation of ad hoc memory allocators.  This code was merged
for the 2.6.13 release; it has been modified considerably since then.
(内核中有许多内存分配子系统，每个都针对特定的需求。然而，有时内核开发者
需要为某个特定范围的特殊用途内存实现一个新的分配器；这些内存通常位于
某个设备上。该设备的驱动作者当然可以自己编写一个小分配器来完成任务，但
那样做只会让内核充满大量未经充分测试的分配器。早在 2005 年，Jes Sorensen
从 sym53c8xx_2 驱动中提取了一个这样的分配器，并将其作为用于创建临时
内存分配器的通用模块发布 (posted)。这段代码在 2.6.13 版本中被合并；此后
它已经被大幅修改。)

.. _posted: https://lwn.net/Articles/125842/

Code using this allocator should include <linux/genalloc.h>.  The action
begins with the creation of a pool using one of:
(使用此分配器的代码应包含 <linux/genalloc.h>。一切始于用以下函数之一
创建一个池：)

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_create		

.. kernel-doc:: lib/genalloc.c
   :functions: devm_gen_pool_create

A call to gen_pool_create() will create a pool.  The granularity of
allocations is set with min_alloc_order; it is a log-base-2 number like
those used by the page allocator, but it refers to bytes rather than pages.
So, if min_alloc_order is passed as 3, then all allocations will be a
multiple of eight bytes.  Increasing min_alloc_order decreases the memory
required to track the memory in the pool.  The nid parameter specifies
which NUMA node should be used for the allocation of the housekeeping
structures; it can be -1 if the caller doesn't care.
(调用 gen_pool_create() 将创建一个池。分配的粒度由 min_alloc_order 设置；
它是一个以 2 为底的对数值，与页分配器使用的数值类似，但它以字节而不是页
为单位。因此，如果 min_alloc_order 传入 3，那么所有分配都将是八字节的
倍数。增大 min_alloc_order 会减少跟踪池中内存所需的内存量。nid 参数指定
应使用哪个 NUMA 节点来分配管理用的结构；如果调用者不关心，可以传 -1。)

The "managed" interface devm_gen_pool_create() ties the pool to a
specific device.  Among other things, it will automatically clean up the
pool when the given device is destroyed.
("managed" 接口 devm_gen_pool_create() 把池绑定到特定设备。除其他事项外，
它会在给定设备销毁时自动清理池。)

A pool is shut down with:
(用以下函数关闭池：)

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_destroy

It's worth noting that, if there are still allocations outstanding from the
given pool, this function will take the rather extreme step of invoking
BUG(), crashing the entire system.  You have been warned.
(值得注意的是，如果给定池仍有未释放的分配，这个函数会采取相当极端的
做法——调用 BUG()，使整个系统崩溃。已经警告过你了。)

A freshly created pool has no memory to allocate.  It is fairly useless in
that state, so one of the first orders of business is usually to add memory
to the pool.  That can be done with one of:
(新建的池没有可分配的内存。在这种状态下它几乎毫无用处，因此首要任务
通常是向池中添加内存。这可以用以下函数之一完成：)

.. kernel-doc:: include/linux/genalloc.h
   :functions: gen_pool_add

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_add_owner

A call to gen_pool_add() will place the size bytes of memory
starting at addr (in the kernel's virtual address space) into the given
pool, once again using nid as the node ID for ancillary memory allocations.
The gen_pool_add_virt() variant associates an explicit physical
address with the memory; this is only necessary if the pool will be used
for DMA allocations.
(调用 gen_pool_add() 会把从 addr 开始（在内核虚拟地址空间中）的 size
字节内存放入给定池，同样使用 nid 作为辅助内存分配的节点 ID。
gen_pool_add_virt() 变体会把一个显式物理地址与该内存关联起来；只有当
该池将用于 DMA 分配时才需要这样做。)

The functions for allocating memory from the pool (and putting it back)
are:
(从池中分配内存（以及把内存归还池中）的函数有：)

.. kernel-doc:: include/linux/genalloc.h
   :functions: gen_pool_alloc

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_dma_alloc

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_free_owner

As one would expect, gen_pool_alloc() will allocate size< bytes
from the given pool.  The gen_pool_dma_alloc() variant allocates
memory for use with DMA operations, returning the associated physical
address in the space pointed to by dma.  This will only work if the memory
was added with gen_pool_add_virt().  Note that this function
departs from the usual genpool pattern of using unsigned long values to
represent kernel addresses; it returns a void * instead.
(正如预期的那样，gen_pool_alloc() 将从给定池分配 size 字节。
gen_pool_dma_alloc() 变体分配用于 DMA 操作的内存，并在 dma 指向的空间中
返回关联的物理地址。只有当内存是通过 gen_pool_add_virt() 添加时，这才
有效。注意，此函数背离了 genpool 使用 unsigned long 值表示内核地址的
通常模式；它改为返回 void *。)

That all seems relatively simple; indeed, some developers clearly found it
to be too simple.  After all, the interface above provides no control over
how the allocation functions choose which specific piece of memory to
return.  If that sort of control is needed, the following functions will be
of interest:
(这一切看起来都相当简单；实际上，一些开发者显然觉得它太简单了。毕竟，
上面的接口没有提供对分配函数如何选择要返回的具体内存块的控制。如果需要
那种控制，以下函数会很有用：)

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_alloc_algo_owner

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_set_algo

Allocations with gen_pool_alloc_algo() specify an algorithm to be
used to choose the memory to be allocated; the default algorithm can be set
with gen_pool_set_algo().  The data value is passed to the
algorithm; most ignore it, but it is occasionally needed.  One can,
naturally, write a special-purpose algorithm, but there is a fair set
already available:
(使用 gen_pool_alloc_algo() 的分配会指定一个算法，用于选择要分配的内存；
默认算法可以用 gen_pool_set_algo() 设置。data 值会传递给该算法；大多数
算法会忽略它，但偶尔会用到。当然，可以编写专用的算法，但已经有一批现成的
算法可用：)

- gen_pool_first_fit is a simple first-fit allocator; this is the default
  algorithm if none other has been specified.
  (gen_pool_first_fit 是一个简单的首次适应 (first-fit) 分配器；如果没有
  指定其他算法，这就是默认算法。)

- gen_pool_first_fit_align forces the allocation to have a specific
  alignment (passed via data in a genpool_data_align structure).
  (gen_pool_first_fit_align 强制分配具有特定的对齐（通过 genpool_data_align
  结构中的 data 传入）。)

- gen_pool_first_fit_order_align aligns the allocation to the order of the
  size.  A 60-byte allocation will thus be 64-byte aligned, for example.
  (gen_pool_first_fit_order_align 把分配按 size 的 order 对齐。例如，一个
  60 字节的分配将按 64 字节对齐。)

- gen_pool_best_fit, as one would expect, is a simple best-fit allocator.
  (gen_pool_best_fit，正如预期的那样，是一个简单的最佳适应 (best-fit)
  分配器。)

- gen_pool_fixed_alloc allocates at a specific offset (passed in a
  genpool_data_fixed structure via the data parameter) within the pool.
  If the indicated memory is not available the allocation fails.
  (gen_pool_fixed_alloc 在池内的特定偏移处分配（通过 data 参数在
  genpool_data_fixed 结构中传入）。如果指定的内存不可用，分配将失败。)

There is a handful of other functions, mostly for purposes like querying
the space available in the pool or iterating through chunks of memory.
Most users, however, should not need much beyond what has been described
above.  With luck, wider awareness of this module will help to prevent the
writing of special-purpose memory allocators in the future.
(还有一些其他函数，主要用于查询池中的可用空间或遍历内存块之类的目的。
不过，大多数用户应该不需要上述内容之外的功能。如果运气好，对这个模块
更广泛的了解将有助于防止将来再编写专用的内存分配器。)

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_virt_to_phys

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_for_each_chunk

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_has_addr

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_avail

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_size

.. kernel-doc:: lib/genalloc.c
   :functions: gen_pool_get

.. kernel-doc:: lib/genalloc.c
   :functions: of_gen_pool_get
