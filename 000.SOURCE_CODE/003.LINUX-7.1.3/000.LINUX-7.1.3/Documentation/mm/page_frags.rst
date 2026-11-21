======================
Page fragments(页片段)
======================

A page fragment is an arbitrary-length arbitrary-offset area of memory
which resides within a 0 or higher order compound page.  Multiple
fragments within that page are individually refcounted, in the page's
reference counter.
(页片段（page fragment）是位于 0 阶或更高阶复合页内的任意长度、任意偏移的
内存区域。该页内的多个片段在页的引用计数中被单独计数引用。)

The page_frag functions, page_frag_alloc and page_frag_free, provide a
simple allocation framework for page fragments.  This is used by the
network stack and network device drivers to provide a backing region of
memory for use as either an sk_buff->head, or to be used in the "frags"
portion of skb_shared_info.
(page_frag 函数（page_frag_alloc 和 page_frag_free）为页片段提供了一个
简单的分配框架。网络栈和网络设备驱动使用它来提供一块后备内存区域，用作
sk_buff->head，或用于 skb_shared_info 的 "frags" 部分。)

In order to make use of the page fragment APIs a backing page fragment
cache is needed.  This provides a central point for the fragment allocation
and tracks allows multiple calls to make use of a cached page.  The
advantage to doing this is that multiple calls to get_page can be avoided
which can be expensive at allocation time.  However due to the nature of
this caching it is required that any calls to the cache be protected by
either a per-cpu limitation, or a per-cpu limitation and forcing interrupts
to be disabled when executing the fragment allocation.
(要使用页片段 API，需要一个后备页片段缓存。它为片段分配提供一个中心点，并
跟踪、允许多次调用复用同一个缓存页。这样做的好处是可以避免多次调用
get_page——在分配时这可能开销很大。但由于这种缓存的性质，对缓存的所有调用
都必须受 per-cpu 限制保护，或受 per-cpu 限制并在执行片段分配时强制关中断的
保护。)

The network stack uses two separate caches per CPU to handle fragment
allocation.  The netdev_alloc_cache is used by callers making use of the
netdev_alloc_frag and __netdev_alloc_skb calls.  The napi_alloc_cache is
used by callers of the __napi_alloc_frag and napi_alloc_skb calls.  The
main difference between these two calls is the context in which they may be
called.  The "netdev" prefixed functions are usable in any context as these
functions will disable interrupts, while the "napi" prefixed functions are
only usable within the softirq context.
(网络栈在每个 CPU 上使用两个独立的缓存来处理片段分配。netdev_alloc_cache
由使用 netdev_alloc_frag 和 __netdev_alloc_skb 调用的调用者使用；
napi_alloc_cache 由 __napi_alloc_frag 和 napi_alloc_skb 调用的调用者使用。
这两类调用之间的主要区别在于可调用它们的上下文：带 "netdev" 前缀的函数可在
任何上下文中使用，因为这些函数会自行关中断；而带 "napi" 前缀的函数只能在
softirq 上下文中使用。)

Many network device drivers use a similar methodology for allocating page
fragments, but the page fragments are cached at the ring or descriptor
level.  In order to enable these cases it is necessary to provide a generic
way of tearing down a page cache.  For this reason __page_frag_cache_drain
was implemented.  It allows for freeing multiple references from a single
page via a single call.  The advantage to doing this is that it allows for
cleaning up the multiple references that were added to a page in order to
avoid calling get_page per allocation.
(许多网络设备驱动采用类似的方法分配页片段，但页片段缓存在环（ring）或描述
符层面。为了支持这些场景，需要提供一种通用的拆除页缓存的方法。为此实现了
__page_frag_cache_drain。它允许通过一次调用释放单个页上的多个引用。这样
做的好处是可以清理为避免每次分配都调用 get_page 而添加到页上的多个引用。)

Alexander Duyck, Nov 29, 2016.
(Alexander Duyck，2016 年 11 月 29 日。)
