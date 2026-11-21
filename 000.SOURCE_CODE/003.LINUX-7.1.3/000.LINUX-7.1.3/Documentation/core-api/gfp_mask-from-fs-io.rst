.. _gfp_mask_from_fs_io:

================================================================
GFP masks used from FS/IO context(FS/IO 上下文中使用的 GFP 掩码)
================================================================

:Date: May, 2018
(日期：2018 年 5 月)
:Author: Michal Hocko <mhocko@kernel.org>
(作者：Michal Hocko <mhocko@kernel.org>)

Introduction(简介)
==================

Code paths in the filesystem and IO stacks must be careful when
allocating memory to prevent recursion deadlocks caused by direct
memory reclaim calling back into the FS or IO paths and blocking on
already held resources (e.g. locks - most commonly those used for the
transaction context).
(文件系统与 IO 栈中的代码路径在分配内存时必须小心，以防止直接内存回
收回调到 FS 或 IO 路径、并阻塞在已持有的资源 (例如锁——最常见的是用
于事务上下文的锁) 上而造成的递归死锁。)

The traditional way to avoid this deadlock problem is to clear __GFP_FS
respectively __GFP_IO (note the latter implies clearing the first as well) in
the gfp mask when calling an allocator. GFP_NOFS respectively GFP_NOIO can be
used as shortcut. It turned out though that above approach has led to
abuses when the restricted gfp mask is used "just in case" without a
deeper consideration which leads to problems because an excessive use
of GFP_NOFS/GFP_NOIO can lead to memory over-reclaim or other memory
reclaim issues.
(避免这种死锁问题的传统方法是在调用分配器时，在 gfp 掩码中清除
__GFP_FS 或 __GFP_IO (注意后者意味着同时清除前者)。可以使用
GFP_NOFS 或 GFP_NOIO 作为快捷方式。然而事实证明，上述做法导致了
一些滥用：在不加深入考虑的情况下"以防万一"地使用受限的 gfp 掩码会
带来问题，因为过度使用 GFP_NOFS/GFP_NOIO 可能导致内存过度回收或其
他内存回收问题。)

New API(新 API)
===============

Since 4.12 we do have a generic scope API for both NOFS and NOIO context
``memalloc_nofs_save``, ``memalloc_nofs_restore`` respectively ``memalloc_noio_save``,
``memalloc_noio_restore`` which allow to mark a scope to be a critical
section from a filesystem or I/O point of view. Any allocation from that
scope will inherently drop __GFP_FS respectively __GFP_IO from the given
mask so no memory allocation can recurse back in the FS/IO.
(从 4.12 起，我们为 NOFS 与 NOIO 上下文提供了通用作用域 API：
``memalloc_nofs_save`` 、 ``memalloc_nofs_restore`` 以及
``memalloc_noio_save`` 、 ``memalloc_noio_restore`` ，它们允许把某个作
用域标记为从文件系统或 I/O 角度看的临界区。来自该作用域的任何分配都
会自动从给定掩码中去掉 __GFP_FS 或 __GFP_IO，因此任何内存分配都不会
递归回 FS/IO。)

.. kernel-doc:: include/linux/sched/mm.h
   :functions: memalloc_nofs_save memalloc_nofs_restore
.. kernel-doc:: include/linux/sched/mm.h
   :functions: memalloc_noio_save memalloc_noio_restore

FS/IO code then simply calls the appropriate save function before
any critical section with respect to the reclaim is started - e.g.
lock shared with the reclaim context or when a transaction context
nesting would be possible via reclaim. The restore function should be
called when the critical section ends. All that ideally along with an
explanation what is the reclaim context for easier maintenance.
(然后，FS/IO 代码只需在一个相对于回收而言的临界区即将开始之前调用
相应的 save 函数——例如与回收上下文共享的锁，或者可能通过回收发生
事务上下文嵌套之时。应在临界区结束时调用 restore 函数。理想情况下，
还应一并解释回收上下文是什么，以便于后续维护。)

Please note that the proper pairing of save/restore functions
allows nesting so it is safe to call ``memalloc_noio_save`` or
``memalloc_noio_restore`` respectively from an existing NOIO or NOFS
scope.
(请注意，save/restore 函数的正确配对允许嵌套，因此从已有的 NOIO 或
NOFS 作用域中分别调用 ``memalloc_noio_save`` 或
``memalloc_noio_restore`` 是安全的。)

What about __vmalloc(GFP_NOFS)(那么 __vmalloc(GFP_NOFS) 呢？)
=============================================================

Since v5.17, and specifically after the commit 451769ebb7e79 ("mm/vmalloc:
alloc GFP_NO{FS,IO} for vmalloc"), GFP_NOFS/GFP_NOIO are now supported in
``[k]vmalloc`` by implicitly using scope API.
(自 v5.17 起，特别是在提交 451769ebb7e79 ("mm/vmalloc: alloc
GFP_NO{FS,IO} for vmalloc") 之后， ``[k]vmalloc`` 通过隐式使用作用域
API 现已支持 GFP_NOFS/GFP_NOIO。)

In earlier kernels ``vmalloc`` didn't support GFP_NOFS semantic because there
were hardcoded GFP_KERNEL allocations deep inside the allocator. That means
that calling ``vmalloc`` with GFP_NOFS/GFP_NOIO was almost always a bug.
(在更早的内核中， ``vmalloc`` 不支持 GFP_NOFS 语义，因为分配器深处存
在硬编码的 GFP_KERNEL 分配。这意味着用 GFP_NOFS/GFP_NOIO 调用
``vmalloc`` 几乎总是个 bug。)

In the ideal world, upper layers should already mark dangerous contexts
and so no special care is required and ``vmalloc`` should be called without any
problems. Sometimes if the context is not really clear or there are layering
violations then the recommended way around that (on pre-v5.17 kernels) is to
wrap ``vmalloc`` by the scope API with a comment explaining the problem.
(在理想世界中，上层应当已经标记了危险的上下文，因此无需特殊处理，
``vmalloc`` 可以毫无问题地被调用。有时，如果上下文不清晰或存在层次
违例，那么 (在 v5.17 之前的内核上) 推荐的绕过方法是用作用域 API 包
装 ``vmalloc``，并加注释说明问题。)
