==========================
Memory Balancing(内存平衡)
==========================

Started Jan 2000 by Kanoj Sarcar <kanoj@sgi.com>
(由 Kanoj Sarcar <kanoj@sgi.com> 于 2000 年 1 月启动。)

Memory balancing is needed for !__GFP_HIGH and !__GFP_KSWAPD_RECLAIM as
well as for non __GFP_IO allocations.
(对 !__GFP_HIGH 和 !__GFP_KSWAPD_RECLAIM，以及非 __GFP_IO 的分配来说，
内存平衡是必需的。)

The first reason why a caller may avoid reclaim is that the caller can not
sleep due to holding a spinlock or is in interrupt context. The second may
be that the caller is willing to fail the allocation without incurring the
overhead of page reclaim. This may happen for opportunistic high-order
allocation requests that have order-0 fallback options. In such cases,
the caller may also wish to avoid waking kswapd.
(调用者避免回收的第一个原因是：调用者由于持有自旋锁或处于中断上下文
而无法睡眠。第二个原因可能是调用者愿意让分配失败，而不愿付出页面
回收的开销。对于带有 order-0 回退选项的机会性高阶分配请求，可能会
出现这种情况。此时，调用者可能还希望避免唤醒 kswapd。)

__GFP_IO allocation requests are made to prevent file system deadlocks.
(设置 __GFP_IO 分配请求是为了防止文件系统死锁。)

In the absence of non sleepable allocation requests, it seems detrimental
to be doing balancing. Page reclamation can be kicked off lazily, that
is, only when needed (aka zone free memory is 0), instead of making it
a proactive process.
(在没有不可睡眠分配请求的情况下，进行平衡似乎是有害的。页面回收可以
惰性地启动，也就是只在需要时(即 zone 空闲内存为 0 时)才启动，而不是
把它变成一个主动的过程。)

That being said, the kernel should try to fulfill requests for direct
mapped pages from the direct mapped pool, instead of falling back on
the dma pool, so as to keep the dma pool filled for dma requests (atomic
or not). A similar argument applies to highmem and direct mapped pages.
OTOH, if there is a lot of free dma pages, it is preferable to satisfy
regular memory requests by allocating one from the dma pool, instead
of incurring the overhead of regular zone balancing.
(话虽如此，内核应尽量从直接映射池满足直接映射页面的请求，而不是回退
到 dma 池，以便为 dma 请求(无论是否为原子)保持 dma 池的库存。类似的
论点也适用于 highmem 与直接映射页面之间。另一方面，如果有大量空闲的
dma 页面，那么通过从 dma 池分配一个页面来满足常规内存请求会更好，
而不是付出常规 zone 平衡的开销。)

In 2.2, memory balancing/page reclamation would kick off only when the
_total_ number of free pages fell below 1/64 th of total memory. With the
right ratio of dma and regular memory, it is quite possible that balancing
would not be done even when the dma zone was completely empty. 2.2 has
been running production machines of varying memory sizes, and seems to be
doing fine even with the presence of this problem. In 2.3, due to
HIGHMEM, this problem is aggravated.
(在 2.2 中，只有当空闲页面的_总_数低于总内存的 1/64 时才会启动内存
平衡/页面回收。在 dma 与常规内存比例合适的情况下，即使 dma zone 完全
为空，也很可能不会进行平衡。2.2 已在各种内存容量的生产机器上运行，
即使存在这个问题似乎也运转良好。在 2.3 中，由于 HIGHMEM，这个问题被
加剧了。)

In 2.3, zone balancing can be done in one of two ways: depending on the
zone size (and possibly of the size of lower class zones), we can decide
at init time how many free pages we should aim for while balancing any
zone. The good part is, while balancing, we do not need to look at sizes
of lower class zones, the bad part is, we might do too frequent balancing
due to ignoring possibly lower usage in the lower class zones. Also,
with a slight change in the allocation routine, it is possible to reduce
the memclass() macro to be a simple equality.
(在 2.3 中，zone 平衡可以用两种方式之一完成：根据 zone 大小(以及可能
的更低级别 zone 的大小)，我们可以在初始化时决定平衡任何 zone 时应以
多少空闲页面为目标。好处是，平衡时无需查看更低级别 zone 的大小；坏处
是，由于忽略了更低级别 zone 可能较低的用量，我们可能做过于频繁的
平衡。此外，只要对分配例程稍作修改，就有可能把 memclass() 宏简化为
一个简单的相等判断。)

Another possible solution is that we balance only when the free memory
of a zone _and_ all its lower class zones falls below 1/64th of the
total memory in the zone and its lower class zones. This fixes the 2.2
balancing problem, and stays as close to 2.2 behavior as possible. Also,
the balancing algorithm works the same way on the various architectures,
which have different numbers and types of zones. If we wanted to get
fancy, we could assign different weights to free pages in different
zones in the future.
(另一种可能的方案是：只有当某个 zone _以及_ 其所有更低级别 zone 的
空闲内存低于这些 zone 总内存的 1/64 时才进行平衡。这修复了 2.2 的
平衡问题，并尽可能贴近 2.2 的行为。此外，该平衡算法在拥有不同 zone
数量和类型的各种架构上的工作方式相同。如果想做得更精细，将来可以为
不同 zone 中的空闲页面赋予不同的权重。)

Note that if the size of the regular zone is huge compared to dma zone,
it becomes less significant to consider the free dma pages while
deciding whether to balance the regular zone. The first solution
becomes more attractive then.
(注意，如果常规 zone 相对于 dma zone 非常大，那么在决定是否平衡常规
zone 时，考虑空闲 dma 页面的意义就变小了。此时第一种方案更有
吸引力。)

The appended patch implements the second solution. It also "fixes" two
problems: first, kswapd is woken up as in 2.2 on low memory conditions
for non-sleepable allocations. Second, the HIGHMEM zone is also balanced,
so as to give a fighting chance for replace_with_highmem() to get a
HIGHMEM page, as well as to ensure that HIGHMEM allocations do not
fall back into regular zone. This also makes sure that HIGHMEM pages
are not leaked (for example, in situations where a HIGHMEM page is in
the swapcache but is not being used by anyone)
(随附的补丁实现了第二种方案。它还"修复"了两个问题：第一，像 2.2 那样，
在内存不足时为不可睡眠分配唤醒 kswapd。第二，HIGHMEM zone 也会被
平衡，从而给 replace_with_highmem() 一个争取 HIGHMEM 页面的机会，同时
确保 HIGHMEM 分配不会回退到常规 zone。这也确保 HIGHMEM 页面不会泄漏
(例如，在某个 HIGHMEM 页面位于 swapcache 中但没有任何人使用的情况下)。)

kswapd also needs to know about the zones it should balance. kswapd is
primarily needed in a situation where balancing can not be done,
probably because all allocation requests are coming from intr context
and all process contexts are sleeping. For 2.3, kswapd does not really
need to balance the highmem zone, since intr context does not request
highmem pages. kswapd looks at the zone_wake_kswapd field in the zone
structure to decide whether a zone needs balancing.
(kswapd 还需要知道它应该平衡哪些 zone。kswapd 主要在无法进行平衡的
情况下才被需要——可能是因为所有分配请求都来自中断上下文，而所有进程
上下文都在睡眠。对 2.3 而言，kswapd 实际上不需要平衡 highmem zone，
因为中断上下文不会请求 highmem 页面。kswapd 查看 zone 结构中的
zone_wake_kswapd 字段来决定某个 zone 是否需要平衡。)

Page stealing from process memory and shm is done if stealing the page would
alleviate memory pressure on any zone in the page's node that has fallen below
its watermark.
(如果偷取页面能够缓解该页面所在节点中任何低于其水位线的 zone 的内存
压力，就会从进程内存和 shm 中偷取页面。)

watermark[WMARK_MIN/WMARK_LOW/WMARK_HIGH]/low_on_memory/zone_wake_kswapd: These
are per-zone fields, used to determine when a zone needs to be balanced. When
the number of pages falls below watermark[WMARK_MIN], the hysteric field
low_on_memory gets set. This stays set till the number of free pages becomes
watermark[WMARK_HIGH]. When low_on_memory is set, page allocation requests will
try to free some pages in the zone (providing GFP_WAIT is set in the request).
Orthogonal to this, is the decision to poke kswapd to free some zone pages.
That decision is not hysteresis based, and is done when the number of free
pages is below watermark[WMARK_LOW]; in which case zone_wake_kswapd is also set.
(watermark[WMARK_MIN/WMARK_LOW/WMARK_HIGH]/low_on_memory/zone_wake_kswapd：
这些是 per-zone 字段，用于确定某个 zone 何时需要平衡。当页面数量低于
watermark[WMARK_MIN] 时，滞回(hysteric)字段 low_on_memory 被置位。
它会一直保持置位，直到空闲页面数达到 watermark[WMARK_HIGH]。当
low_on_memory 置位时，页面分配请求会尝试在该 zone 中释放一些页面
(前提是请求中设置了 GFP_WAIT)。与此正交的，是是否去戳一下 kswapd 以
释放一些 zone 页面的决定。该决定不基于滞回，它在空闲页面数低于
watermark[WMARK_LOW] 时做出；此时 zone_wake_kswapd 也会被置位。)


(Good) Ideas that I have heard:
(听到过的(好)想法：)

1. Dynamic experience should influence balancing: number of failed requests
   for a zone can be tracked and fed into the balancing scheme (jalvo@mbay.net)
   (动态经验应当影响平衡：可以跟踪某个 zone 的失败请求次数，并把这些
   数据输入平衡方案(jalvo@mbay.net)。)
2. Implement a replace_with_highmem()-like replace_with_regular() to preserve
   dma pages. (lkd@tantalophile.demon.co.uk)
   (实现一个类似 replace_with_highmem() 的 replace_with_regular() 来
   保留 dma 页面(lkd@tantalophile.demon.co.uk)。)
