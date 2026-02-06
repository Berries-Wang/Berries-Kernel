/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GFP_H
#define __LINUX_GFP_H

#include <linux/mmdebug.h>
#include <linux/mmzone.h>
#include <linux/stddef.h>
#include <linux/linkage.h>
#include <linux/topology.h>

struct vm_area_struct;

/*
 * In case of changes, please don't forget to update
 * include/trace/events/mmflags.h and tools/perf/builtin-kmem.c
 */

/* Plain integer GFP bitmasks. Do not use this directly. */
#define ___GFP_DMA		0x01u
#define ___GFP_HIGHMEM		0x02u
#define ___GFP_DMA32		0x04u
#define ___GFP_MOVABLE		0x08u
#define ___GFP_RECLAIMABLE	0x10u
#define ___GFP_HIGH		0x20u
#define ___GFP_IO		0x40u
#define ___GFP_FS		0x80u
#define ___GFP_ZERO		0x100u
#define ___GFP_ATOMIC		0x200u
#define ___GFP_DIRECT_RECLAIM	0x400u
#define ___GFP_KSWAPD_RECLAIM	0x800u
#define ___GFP_WRITE		0x1000u
#define ___GFP_NOWARN		0x2000u
#define ___GFP_RETRY_MAYFAIL	0x4000u
#define ___GFP_NOFAIL		0x8000u
#define ___GFP_NORETRY		0x10000u
#define ___GFP_MEMALLOC		0x20000u
#define ___GFP_COMP		0x40000u
#define ___GFP_NOMEMALLOC	0x80000u
#define ___GFP_HARDWALL		0x100000u
#define ___GFP_THISNODE		0x200000u
#define ___GFP_ACCOUNT		0x400000u
#ifdef CONFIG_LOCKDEP
#define ___GFP_NOLOCKDEP	0x800000u
#else
#define ___GFP_NOLOCKDEP	0
#endif
/* If the above are modified, __GFP_BITS_SHIFT may need updating */

/**
 * 参考: #4.1.2　分配掩码 
 *
 * Physical address zone modifiers (see linux/mmzone.h - low four bits)
 *
 * Do not put any conditional on these. If necessary modify the definitions
 * without the underscores and use them consistently. The definitions here may
 * be used in bit comparisons.
 */
#define __GFP_DMA	((__force gfp_t)___GFP_DMA)
#define __GFP_HIGHMEM	((__force gfp_t)___GFP_HIGHMEM)
#define __GFP_DMA32	((__force gfp_t)___GFP_DMA32)
#define __GFP_MOVABLE	((__force gfp_t)___GFP_MOVABLE)  /* ZONE_MOVABLE allowed */
#define GFP_ZONEMASK	(__GFP_DMA|__GFP_HIGHMEM|__GFP_DMA32|__GFP_MOVABLE)

/**
 * DOC: Page mobility and placement hints
 *
 * Page mobility and placement hints (页面可迁移性与放置提示)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * These flags provide hints about how mobile the page is. Pages with similar
 * mobility are placed within the same pageblocks to minimise problems due
 * to external fragmentation.
 * (这些标志提供了关于页面移动性的提示。具有相似移动性的页面会被放置在相同的页面块中，以尽量减少外部碎片导致的问题)
 *
 * %__GFP_MOVABLE (also a zone modifier) indicates that the page can be
 * moved by page migration during memory compaction or can be reclaimed.
 * (__GFP_MOVABLE（同样是一个区域修饰符）表示该页面可以在内存规整期间通过页迁移被移动，或者可以被回收)
 *
 * %__GFP_RECLAIMABLE is used for slab allocations that specify
 * SLAB_RECLAIM_ACCOUNT and whose pages can be freed via shrinkers.
 * (__GFP_RECLAIMABLE 用于指定了 SLAB_RECLAIM_ACCOUNT 且其页面可通过收缩器释放的 slab 分配)
 *
 * %__GFP_WRITE indicates the caller intends to dirty the page. Where possible,
 * these pages will be spread between local zones to avoid all the dirty
 * pages being in one zone (fair zone allocation policy).
 * (这表明调用者打算弄脏该页。在可能的情况下，这些页面将被分散到不同的本地内存区域，
 * 以防止所有脏页集中在单一区域（即公平区域分配策略）)
 *
 * %__GFP_HARDWALL enforces the cpuset memory allocation policy.
 * (强制执行cpuset内存分配策略)
 *
 * %__GFP_THISNODE forces the allocation to be satisfied from the requested
 * node with no fallbacks or placement policy enforcements.
 * (强制要求分配必须由指定节点完成，既不启用备用方案，也不执行任何放置策略)
 *
 * %__GFP_ACCOUNT causes the allocation to be accounted to kmemcg.
 * (使得该分配由kmemcg进行记账)
 */
#define __GFP_RECLAIMABLE ((__force gfp_t)___GFP_RECLAIMABLE)
#define __GFP_WRITE	((__force gfp_t)___GFP_WRITE)
#define __GFP_HARDWALL   ((__force gfp_t)___GFP_HARDWALL)
#define __GFP_THISNODE	((__force gfp_t)___GFP_THISNODE)
#define __GFP_ACCOUNT	((__force gfp_t)___GFP_ACCOUNT)

/**
 * DOC: Watermark modifiers
 * > 预留内存，防止彻底耗尽!
 *
 * Watermark modifiers -- controls access to emergency reserves
 * (水位线修正符（Watermark modifiers）—— 控制对紧急预留内存（emergency reserves）的访问权限。)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * %__GFP_HIGH indicates that the caller is high-priority and that granting
 * the request is necessary before the system can make forward progress.
 * For example, creating an IO context to clean pages.
 * (%__GFP_HIGH 表示调用者具有高优先级，且在系统取得进一步进展（forward progress）之前，必须准予该请求。例如：为了清理内存页而创建 IO 上下文。)
 * 
 * %__GFP_ATOMIC indicates that the caller cannot reclaim or sleep and is
 * high priority. Users are typically interrupt handlers. This may be
 * used in conjunction with %__GFP_HIGH
 * (%__GFP_ATOMIC 表示调用者既不能执行内存回收，也不能进入睡眠，并且具有高优先级。该标志的使用者通常是中断处理程序（interrupt handlers）。它常与 %__GFP_HIGH 结合使用)
 *
 * %__GFP_MEMALLOC allows access to all memory. This should only be used when
 * the caller guarantees the allocation will allow more memory to be freed
 * very shortly e.g. process exiting or swapping. Users either should
 * be the MM or co-ordinating closely with the VM (e.g. swap over NFS).
 * Users of this flag have to be extremely careful to not deplete the reserve
 * completely and implement a throttling mechanism which controls the
 * consumption of the reserve based on the amount of freed memory.
 * Usage of a pre-allocated pool (e.g. mempool) should be always considered
 * before using this flag.
 * (%__GFP_MEMALLOC 允许访问所有内存。只有当调用者能够保证此次分配将很快释放更多内存时，才应使用此标志，
 * 例如进程退出或交换（swapping）过程。使用者应当是内存管理子系统（MM）本身，
 * 或者是与虚拟内存（VM）紧密协作的组件（例如通过 NFS 进行交换）。
 * 该标志的使用者必须极其谨慎，以防彻底耗尽预留内存；同时必须实现一种节流机制（throttling mechanism），
 * 根据已释放内存的数量来控制预留内存的消耗。在使用此标志之前，应始终优先考虑使用预分配池（如 mempool）)
 *
 * %__GFP_NOMEMALLOC is used to explicitly forbid access to emergency reserves.
 * This takes precedence over the %__GFP_MEMALLOC flag if both are set.
 * (%__GFP_NOMEMALLOC 用于明确禁止访问紧急预留内存。如果同时设置了此标志与 %__GFP_NOMEMALLOC，则本标志的优先级高于 %__GFP_MEMALLOC。)
 */
#define __GFP_ATOMIC	((__force gfp_t)___GFP_ATOMIC)
#define __GFP_HIGH	((__force gfp_t)___GFP_HIGH)
#define __GFP_MEMALLOC	((__force gfp_t)___GFP_MEMALLOC)
#define __GFP_NOMEMALLOC ((__force gfp_t)___GFP_NOMEMALLOC)

/**
 * DOC: Reclaim modifiers
 *
 * Reclaim modifiers
 * ~~~~~~~~~~~~~~~~~
 * Please note that all the following flags are only applicable to sleepable
 * allocations (e.g. %GFP_NOWAIT and %GFP_ATOMIC will ignore them).
 *
 * %__GFP_IO can start physical IO.
 *
 * %__GFP_FS can call down to the low-level FS. Clearing the flag avoids the
 * allocator recursing into the filesystem which might already be holding
 * locks.
 * (%__GFP_FS 允许调用到底层文件系统（FS）。清除此标志可以避免分配器递归调用到文件系统，因为文件系统此时可能已经持有了相关锁。)
 *
 * %__GFP_DIRECT_RECLAIM indicates that the caller may enter direct reclaim.
 * This flag can be cleared to avoid unnecessary delays when a fallback
 * option is available.
 * (%__GFP_DIRECT_RECLAIM 表示调用者可能会进入‘直接回收’流程。当存在备选方案（fallback option）时，可以清除此标志，以避免不必要的延迟。)
 *
 * %__GFP_KSWAPD_RECLAIM indicates that the caller wants to wake kswapd when
 * the low watermark is reached and have it reclaim pages until the high
 * watermark is reached. A caller may wish to clear this flag when fallback
 * options are available and the reclaim is likely to disrupt the system. The
 * canonical example is THP allocation where a fallback is cheap but
 * reclaim/compaction may cause indirect stalls.
 * (%__GFP_KSWAPD_RECLAIM 表示当内存达到‘低水位线（low watermark）’时，
 * 调用者希望唤醒 kswapd 内核线程，并让其持续回收内存页，
 * 直到达到‘高水位线（high watermark）’。
 * 如果存在备选方案（fallback options），且回收操作可能会对系统造成干扰，
 * 调用者可能会希望清除此标志。最典型的例子是 THP（透明巨页）分配：
 * 此时回退到普通页面的代价很小，但回收或紧缩（compaction）操作可能会导致间接的性能停顿（stalls）。)
 *
 * %__GFP_RECLAIM is shorthand to allow/forbid both direct and kswapd reclaim.
 *(%__GFP_RECLAIM 是一个缩写标志，用于同时允许或禁止‘直接回收（direct reclaim）’和‘kswapd 内核线程回收’。)
 * 
 * The default allocator behavior depends on the request size. We have a concept
 * of so called costly allocations (with order > %PAGE_ALLOC_COSTLY_ORDER).
 * !costly allocations are too essential to fail so they are implicitly
 * non-failing by default (with some exceptions like OOM victims might fail so
 * the caller still has to check for failures) while costly requests try to be
 * not disruptive and back off even without invoking the OOM killer.
 * The following three modifiers might be used to override some of these
 * implicit rules
 * (默认的分配器行为取决于请求的大小。
 * 我们引入了所谓的‘昂贵分配（costly allocations）’概念（即分配阶数 order > %PAGE_ALLOC_COSTLY_ORDER）。
 * ‘非昂贵分配（!costly allocations）’由于过于关键而不能失败，因此默认情况下它们被隐式设置为‘永不失败’（存在少数例外，例如被 OOM 选中的进程可能会失败，因此调用者仍需检查结果）；
 * 而‘昂贵请求’则倾向于不对系统造成干扰，即使不触发 OOM Killer 也会选择主动退避（back off）。以下三个修正符可用于覆盖这些隐式规则：)
 *
 * %__GFP_NORETRY: The VM implementation will try only very lightweight
 * memory direct reclaim to get some memory under memory pressure (thus
 * it can sleep). It will avoid disruptive actions like OOM killer. The
 * caller must handle the failure which is quite likely to happen under
 * heavy memory pressure. The flag is suitable when failure can easily be
 * handled at small cost, such as reduced throughput
 * (虚拟内存（VM）的实现将仅尝试极其轻量级的内存直接回收（Direct Reclaim），
 * 以在内存压力下获取少量内存（因此可能会发生睡眠）。
 * 它会避免触发像 OOM Killer（内存溢出杀手）这类破坏性动作。
 * 调用者必须能够处理极有可能在重度内存压力下发生的分配失败。
 * 该标志位适用于分配失败易于处理且代价较小的场景，例如仅导致吞吐量下降的情况。)
 *
 * %__GFP_RETRY_MAYFAIL: The VM implementation will retry memory reclaim
 * procedures that have previously failed if there is some indication
 * that progress has been made else where.  It can wait for other
 * tasks to attempt high level approaches to freeing memory such as
 * compaction (which removes fragmentation) and page-out.
 * There is still a definite limit to the number of retries, but it is
 * a larger limit than with %__GFP_NORETRY.
 * Allocations with this flag may fail, but only when there is
 * genuinely little unused memory. While these allocations do not
 * directly trigger the OOM killer, their failure indicates that
 * the system is likely to need to use the OOM killer soon.  The
 * caller must handle failure, but can reasonably do so by failing
 * a higher-level request, or completing it only in a much less
 * efficient manner.
 * If the allocation does fail, and the caller is in a position to
 * free some non-essential memory, doing so could benefit the system
 * as a whole.
 * (%__GFP_RETRY_MAYFAIL：如果有迹象表明其他地方取得了进展，VM（虚拟内存）实现将重试之前失败过的内存回收程序。
 * 它会等待其他任务尝试更高级别的内存释放方法，例如紧缩（Compaction，用于消除碎片）和换出（Page-out）。
 * 重试次数仍有明确限制，但该限制比 %__GFP_NORETRY 更大。带有此标志的分配可能会失败，
 * 但通常仅在空闲内存确实极少时才会发生。虽然这些分配不会直接触发 OOM Killer，
 * 但它们的失败预示着系统可能很快就需要动用 OOM Killer 了。调用者必须处理失败情况，
 * 常见做法是让上层请求失败，或者改用效率低得多的方式来完成任务。如果分配确实失败了，
 * 且调用者有能力释放一些非必需内存，那么这样做将对整个系统大有裨益。)
 *
 * %__GFP_NOFAIL: The VM implementation _must_ retry infinitely: the caller
 * cannot handle allocation failures. The allocation could block
 * indefinitely but will never return with failure. Testing for
 * failure is pointless.
 * New users should be evaluated carefully (and the flag should be
 * used only when there is no reasonable failure policy) but it is
 * definitely preferable to use the flag rather than opencode endless
 * loop around allocator.
 * Using this flag for costly allocations is _highly_ discouraged.
 * (%__GFP_NOFAIL：VM（虚拟内存）实现必须进行无限次重试：因为调用者无法处理分配失败的情况。
 * 该分配可能会无限期阻塞，但绝不会返回失败结果。因此，对返回值进行失败检测是没有意义的。
 * 对于新加入的使用场景应进行严格评估（仅当不存在合理的失败处理策略时才使用此标志），
 * 但使用该标志绝对优于在分配器外层手动编写一个‘死循环’。
 * 此外，极度不鼓励在昂贵分配（costly allocations）中使用此标志。)
 */
#define __GFP_IO	((__force gfp_t)___GFP_IO)
#define __GFP_FS	((__force gfp_t)___GFP_FS)
#define __GFP_DIRECT_RECLAIM	((__force gfp_t)___GFP_DIRECT_RECLAIM) /* Caller can reclaim */
#define __GFP_KSWAPD_RECLAIM	((__force gfp_t)___GFP_KSWAPD_RECLAIM) /* kswapd can wake */
/**
 * 用于允许或者禁止直接页面回收和kswapd内核线程
 * 
 * 	 * kswapd: Kernel Swap Daemon（内核交换守护进程）
 *	 * >> 参考:[001.UNIX-DOCS/000.内存管理/017.关键词记录/000.kswapd是什么.md]
 */
#define __GFP_RECLAIM ((__force gfp_t)(___GFP_DIRECT_RECLAIM|___GFP_KSWAPD_RECLAIM))
#define __GFP_RETRY_MAYFAIL	((__force gfp_t)___GFP_RETRY_MAYFAIL)
#define __GFP_NOFAIL	((__force gfp_t)___GFP_NOFAIL)
#define __GFP_NORETRY	((__force gfp_t)___GFP_NORETRY)

/**
 * DOC: Action modifiers
 *
 * Action modifiers
 * ~~~~~~~~~~~~~~~~
 *
 * %__GFP_NOWARN suppresses allocation failure reports. 
 * (%__GFP_NOWARN 用于抑制分配失败时的报告)
 * > 让内存分配器在拿不到内存时“保持沉默”
 *
 * %__GFP_COMP address(处理/解决) compound page metadata.(处理复合页（compound page）的元数据。)
 *
 * %__GFP_ZERO returns a zeroed page on success.
 * (%__GFP_ZERO 表示在分配成功时，返回一个已经过清零处理（zeroed）的页面)
 */
#define __GFP_NOWARN	((__force gfp_t)___GFP_NOWARN)
#define __GFP_COMP	((__force gfp_t)___GFP_COMP)
#define __GFP_ZERO	((__force gfp_t)___GFP_ZERO)

/* Disable lockdep for GFP context tracking */
#define __GFP_NOLOCKDEP ((__force gfp_t)___GFP_NOLOCKDEP)

/* Room for N __GFP_FOO bits */
#define __GFP_BITS_SHIFT (23 + IS_ENABLED(CONFIG_LOCKDEP))
#define __GFP_BITS_MASK ((__force gfp_t)((1 << __GFP_BITS_SHIFT) - 1))

/**
 * 都什么意思？
 * > [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#4.1.2　分配掩码
 * 
 * DOC: Useful GFP flag combinations
 *
 * Useful GFP flag combinations（有用的 GFP 标志组合）
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Useful GFP flag combinations that are commonly used. It is recommended
 * that subsystems start with one of these combinations and then set/clear
 * %__GFP_FOO flags as necessary.
 * (常用的 GFP 标志位组合。建议各子系统先从以下组合之一开始，然后根据需要设置或清除具体的 %__GFP_FOO 标志)
 *
 * %GFP_ATOMIC users can not sleep and need the allocation to succeed. A lower
 * watermark is applied to allow access to "atomic reserves"
 *(%GFP_ATOMIC 的使用者不能进入睡眠，并且要求分配必须成功。系统会应用一个更低的水位线（watermark），以允许其访问‘原子预留内存（atomic reserves）’。)
 *
 * %GFP_KERNEL is typical for kernel-internal allocations. The caller requires
 * %ZONE_NORMAL or a lower zone for direct access but can direct reclaim.
 *(%GFP_KERNEL 是典型的内核内部分配标志。调用者要求使用 %ZONE_NORMAL 或更低层级的内存区域以进行直接访问，但允许触发直接回收（direct reclaim）)
 *
 * %GFP_KERNEL_ACCOUNT is the same as GFP_KERNEL, except the allocation is
 * accounted to kmemcg.
 * (%GFP_KERNEL_ACCOUNT 与 GFP_KERNEL 完全相同，唯一的区别是该分配会被计入内核内存控制组（kmemcg）的统计中)
 *
 * %GFP_NOWAIT is for kernel allocations that should not stall for direct
 * reclaim, start physical IO or use any filesystem callback.
 * (%GFP_NOWAIT 用于内核分配，要求不得因直接回收（direct reclaim）而停顿，不得启动物理 I/O，也不得使用任何文件系统回调。)
 *
 * %GFP_NOIO will use direct reclaim to discard clean pages or slab pages
 * that do not require the starting of any physical IO.
 * Please try to avoid using this flag directly and instead use
 * memalloc_noio_{save,restore} to mark the whole scope which cannot
 * perform any IO with a short explanation why. All allocation requests
 * will inherit GFP_NOIO implicitly.
 * (%GFP_NOIO 会通过直接回收（direct reclaim）来释放那些不需要启动任何物理 I/O 的干净页（clean pages）或 Slab 页。
 * 请尽量避免直接使用此标志，而是改用 memalloc_noio_{save,restore} 宏来标记整个无法执行 I/O 的代码范围，
 * 并简要解释原因。在此范围内，所有的分配请求都会隐式继承 GFP_NOIO 标志。)
 *
 * %GFP_NOFS will use direct reclaim but will not use any filesystem interfaces.
 * Please try to avoid using this flag directly and instead use
 * memalloc_nofs_{save,restore} to mark the whole scope which cannot/shouldn't
 * recurse into the FS layer with a short explanation why. All allocation
 * requests will inherit GFP_NOFS implicitly.
 * (%GFP_NOFS 会触发直接回收（direct reclaim），但不会使用任何文件系统接口。
 * 请尽量避免直接使用此标志，而是改用 memalloc_nofs_{save,restore} 宏来标记整个‘不能或不应递归进入文件系统层’的代码范围，
 * 并简要说明原因。在该范围内的所有分配请求都将隐式继承 GFP_NOFS 属性。)
 *
 * %GFP_USER is for userspace allocations that also need to be directly
 * accessibly by the kernel or hardware. It is typically used by hardware
 * for buffers that are mapped to userspace (e.g. graphics) that hardware
 * still must DMA to. cpuset limits are enforced for these allocations.
 * (%GFP_USER 用于用户空间分配，且该分配同时需要能被内核或硬件直接访问。它通常用于硬件缓冲区，
 * 这些缓冲区被映射到用户空间（例如显卡），同时硬件仍必须对其进行 DMA 操作。
 * 此类分配会强制执行 cpuset 限制。)
 *
 * %GFP_DMA exists for historical reasons and should be avoided where possible.
 * The flags indicates that the caller requires that the lowest zone be
 * used (%ZONE_DMA or 16M on x86-64). Ideally, this would be removed but
 * it would require careful auditing as some users really require it and
 * others use the flag to avoid lowmem reserves in %ZONE_DMA and treat the
 * lowest zone as a type of emergency reserve.
 * (%GFP_DMA 的存在是由于历史原因，应尽可能避免使用。
 * 该标志表示调用者要求使用最低层级的内存区域（即 %ZONE_NORMAL 之下，在 x86-64 架构上通常指前 16MB 区域）。
 * 理想情况下，该标志应该被移除，但这需要进行仔细的审计，因为某些使用者确实需要它，
 * 而另一些使用者则是利用该标志来绕过 %ZONE_DMA 中的低端内存预留限制，将最低区域当作一种紧急预留内存来使用。)
 *
 * %GFP_DMA32 is similar to %GFP_DMA except that the caller requires a 32-bit
 * address.
 * (%GFP_DMA32 与 %GFP_DMA 类似，不同之处在于调用者要求分配的是 32 位（32-bit）地址范围内的内存)
 *
 * %GFP_HIGHUSER is for userspace allocations that may be mapped to userspace,
 * do not need to be directly accessible by the kernel but that cannot
 * move once in use. An example may be a hardware allocation that maps
 * data directly into userspace but has no addressing limitations.
 * (%GFP_HIGHUSER 用于用户空间分配，这些分配可能会映射到用户空间，不需要内核直接访问，
 * 但一旦投入使用便不可移动。例如：一种将数据直接映射到用户空间，且没有任何寻址限制的硬件分配)
 * 
 * %GFP_HIGHUSER_MOVABLE is for userspace allocations that the kernel does not
 * need direct access to but can use kmap() when access is required. They
 * are expected to be movable via page reclaim or page migration. Typically,
 * pages on the LRU would also be allocated with %GFP_HIGHUSER_MOVABLE.
 * (%GFP_HIGHUSER_MOVABLE 用于用户空间分配。内核不需要对其进行直接访问，
 * 但在需要访问时可以使用 kmap()。这类页面预期可以通过页面回收（page reclaim）或页面迁移（page migration）进行移动。
 * 通常情况下，处于 LRU（最近最少使用）链表上的页面也会使用 %GFP_HIGHUSER_MOVABLE 进行分配)
 * 
 *
 * %GFP_TRANSHUGE and %GFP_TRANSHUGE_LIGHT are used for THP allocations. They
 * are compound allocations that will generally fail quickly if memory is not
 * available and will not wake kswapd/kcompactd on failure. The _LIGHT
 * version does not attempt reclaim/compaction at all and is by default used
 * in page fault path, while the non-light is used by khugepaged.
 * (%GFP_TRANSHUGE 和 %GFP_TRANSHUGE_LIGHT 用于透明巨页（THP）分配。它们属于复合页（compound pages）分配，
 * 通常在内存不足时会迅速返回失败，且失败时不会唤醒 kswapd 或 kcompactd。
 * 其中 _LIGHT 版本完全不尝试回收或规整（reclaim/compaction），默认用于缺页异常路径；
 * 而非轻量级（non-light）版本则由 khugepaged 线程使用。)
 */
#define GFP_ATOMIC	(__GFP_HIGH|__GFP_ATOMIC|__GFP_KSWAPD_RECLAIM)
#define GFP_KERNEL	(__GFP_RECLAIM | __GFP_IO | __GFP_FS)
#define GFP_KERNEL_ACCOUNT (GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOWAIT	(__GFP_KSWAPD_RECLAIM)
#define GFP_NOIO	(__GFP_RECLAIM)
#define GFP_NOFS	(__GFP_RECLAIM | __GFP_IO)
#define GFP_USER	(__GFP_RECLAIM | __GFP_IO | __GFP_FS | __GFP_HARDWALL)
#define GFP_DMA		__GFP_DMA
#define GFP_DMA32	__GFP_DMA32
#define GFP_HIGHUSER	(GFP_USER | __GFP_HIGHMEM)
#define GFP_HIGHUSER_MOVABLE	(GFP_HIGHUSER | __GFP_MOVABLE)
#define GFP_TRANSHUGE_LIGHT	((GFP_HIGHUSER_MOVABLE | __GFP_COMP | \
			 __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM)
#define GFP_TRANSHUGE	(GFP_TRANSHUGE_LIGHT | __GFP_DIRECT_RECLAIM)

/* Convert GFP flags to their corresponding migrate type */
#define GFP_MOVABLE_MASK (__GFP_RECLAIMABLE|__GFP_MOVABLE)
#define GFP_MOVABLE_SHIFT 3

static inline int gfp_migratetype(const gfp_t gfp_flags)
{
	VM_WARN_ON((gfp_flags & GFP_MOVABLE_MASK) == GFP_MOVABLE_MASK);
	BUILD_BUG_ON((1UL << GFP_MOVABLE_SHIFT) != ___GFP_MOVABLE);
	BUILD_BUG_ON((___GFP_MOVABLE >> GFP_MOVABLE_SHIFT) != MIGRATE_MOVABLE);

	if (unlikely(page_group_by_mobility_disabled))
		return MIGRATE_UNMOVABLE;

	/* Group based on mobility */
	return (gfp_flags & GFP_MOVABLE_MASK) >> GFP_MOVABLE_SHIFT;
}
#undef GFP_MOVABLE_MASK
#undef GFP_MOVABLE_SHIFT

static inline bool gfpflags_allow_blocking(const gfp_t gfp_flags)
{
	return !!(gfp_flags & __GFP_DIRECT_RECLAIM);
}

/**
 * gfpflags_normal_context - is gfp_flags a normal sleepable context?
 * (gfp_flags 本身是否属于正常的、可睡眠的上下文?)
 * @gfp_flags: gfp_flags to test
 *
 * Test whether @gfp_flags indicates that the allocation is from the
 * %current context and allowed to sleep.
 *
 * An allocation being allowed to block doesn't mean it owns the %current
 * context.  When direct reclaim path tries to allocate memory, the
 * allocation context is nested inside whatever %current was doing at the
 * time of the original allocation.  The nested allocation may be allowed
 * to block but modifying anything %current owns can corrupt the outer
 * context's expectations.
 * (一个分配被允许阻塞（block），并不意味着它拥有当前的上下文（%current context）。
 * 当直接回收（direct reclaim）路径尝试分配内存时，该分配上下文是嵌套在原始分配时 %current 进程所执行的任务之中的。
 * 虽然嵌套的分配可能被允许阻塞，但如果修改了任何 %current 所拥有的资源，都可能会破坏外部上下文的预期（导致数据损坏或逻辑错误）)
 *
 * %true result from this function indicates that the allocation context
 * can sleep and use anything that's associated with %current.
 * (该函数返回 %true 结果表明：分配上下文（allocation context）既可以进入睡眠，也可以使用与 %current（当前进程）相关联的任何资源。)
 */
static inline bool gfpflags_normal_context(const gfp_t gfp_flags)
{
	return (gfp_flags & (__GFP_DIRECT_RECLAIM | __GFP_MEMALLOC)) ==
		__GFP_DIRECT_RECLAIM;
}

#ifdef CONFIG_HIGHMEM
#define OPT_ZONE_HIGHMEM ZONE_HIGHMEM
#else
#define OPT_ZONE_HIGHMEM ZONE_NORMAL
#endif

#ifdef CONFIG_ZONE_DMA
#define OPT_ZONE_DMA ZONE_DMA
#else
#define OPT_ZONE_DMA ZONE_NORMAL
#endif

#ifdef CONFIG_ZONE_DMA32
#define OPT_ZONE_DMA32 ZONE_DMA32
#else
#define OPT_ZONE_DMA32 ZONE_NORMAL
#endif

/**
 * GFP_ZONE_TABLE is a word size bitstring that is used for looking up the
 * zone to use given the lowest 4 bits of gfp_t. Entries are GFP_ZONES_SHIFT
 * bits long and there are 16 of them to cover all possible combinations of
 * __GFP_DMA, __GFP_DMA32, __GFP_MOVABLE and __GFP_HIGHMEM.
 * (GFP_ZONE_TABLE 是一个字长（word size）大小的位串，用于根据 gfp_t 的最低 4 位来查表确定应使用的内存区域（Zone）。
 * 表中的每个条目长度为 GFP_ZONES_SHIFT 位，总共有 16 个条目，用以覆盖 __GFP_DMA、__GFP_DMA32、__GFP_MOVABLE 和 __GFP_HIGHMEM 这四个标志的所有可能组合。)
 *
 * The zone fallback order is MOVABLE=>HIGHMEM=>NORMAL=>DMA32=>DMA.
 * But GFP_MOVABLE is not only a zone specifier but also an allocation
 * policy. Therefore __GFP_MOVABLE plus another zone selector is valid.
 * Only 1 bit of the lowest 3 bits (DMA,DMA32,HIGHMEM) can be set to "1".
 * (内存区域的后备路径（fallback order）顺序为：MOVABLE => HIGHMEM => NORMAL => DMA32 => DMA。
 * 但是，__GFP_MOVABLE 不仅仅是一个区域指定符，它还代表一种分配策略。因此，将 __GFP_MOVABLE 与另一个区域选择标志组合使用是有效的。在最低的 3 位标志（DMA、DMA32、HIGHMEM）中，同时只能有 1 位被设置为 ‘1’。)
 *
 *       bit       result
 *       =================
 *       0x0    => NORMAL
 *       0x1    => DMA or NORMAL
 *       0x2    => HIGHMEM or NORMAL
 *       0x3    => BAD (DMA+HIGHMEM)
 *       0x4    => DMA32 or NORMAL
 *       0x5    => BAD (DMA+DMA32)
 *       0x6    => BAD (HIGHMEM+DMA32)
 *       0x7    => BAD (HIGHMEM+DMA32+DMA)
 *       0x8    => NORMAL (MOVABLE+0)
 *       0x9    => DMA or NORMAL (MOVABLE+DMA)
 *       0xa    => MOVABLE (Movable is valid only if HIGHMEM is set too)
 *       0xb    => BAD (MOVABLE+HIGHMEM+DMA)
 *       0xc    => DMA32 or NORMAL (MOVABLE+DMA32)
 *       0xd    => BAD (MOVABLE+DMA32+DMA)
 *       0xe    => BAD (MOVABLE+DMA32+HIGHMEM)
 *       0xf    => BAD (MOVABLE+DMA32+HIGHMEM+DMA)
 *
 * GFP_ZONES_SHIFT must be <= 2 on 32 bit platforms.
 */

#if defined(CONFIG_ZONE_DEVICE) && (MAX_NR_ZONES-1) <= 4
/* ZONE_DEVICE is not a valid GFP zone specifier */
#define GFP_ZONES_SHIFT 2
#else
#define GFP_ZONES_SHIFT ZONES_SHIFT
#endif

#if 16 * GFP_ZONES_SHIFT > BITS_PER_LONG
#error GFP_ZONES_SHIFT too large to create GFP_ZONE_TABLE integer
#endif

#define GFP_ZONE_TABLE ( \
	(ZONE_NORMAL << 0 * GFP_ZONES_SHIFT)				       \
	| (OPT_ZONE_DMA << ___GFP_DMA * GFP_ZONES_SHIFT)		       \
	| (OPT_ZONE_HIGHMEM << ___GFP_HIGHMEM * GFP_ZONES_SHIFT)	       \
	| (OPT_ZONE_DMA32 << ___GFP_DMA32 * GFP_ZONES_SHIFT)		       \
	| (ZONE_NORMAL << ___GFP_MOVABLE * GFP_ZONES_SHIFT)		       \
	| (OPT_ZONE_DMA << (___GFP_MOVABLE | ___GFP_DMA) * GFP_ZONES_SHIFT)    \
	| (ZONE_MOVABLE << (___GFP_MOVABLE | ___GFP_HIGHMEM) * GFP_ZONES_SHIFT)\
	| (OPT_ZONE_DMA32 << (___GFP_MOVABLE | ___GFP_DMA32) * GFP_ZONES_SHIFT)\
)

/*
 * GFP_ZONE_BAD is a bitmap for all combinations of __GFP_DMA, __GFP_DMA32
 * __GFP_HIGHMEM and __GFP_MOVABLE that are not permitted. One flag per
 * entry starting with bit 0. Bit is set if the combination is not
 * allowed.
 */
#define GFP_ZONE_BAD ( \
	1 << (___GFP_DMA | ___GFP_HIGHMEM)				      \
	| 1 << (___GFP_DMA | ___GFP_DMA32)				      \
	| 1 << (___GFP_DMA32 | ___GFP_HIGHMEM)				      \
	| 1 << (___GFP_DMA | ___GFP_DMA32 | ___GFP_HIGHMEM)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_HIGHMEM | ___GFP_DMA)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_DMA32 | ___GFP_DMA)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_DMA32 | ___GFP_HIGHMEM)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_DMA32 | ___GFP_DMA | ___GFP_HIGHMEM)  \
)

static inline enum zone_type gfp_zone(gfp_t flags)
{
	enum zone_type z;
	int bit = (__force int) (flags & GFP_ZONEMASK);

	z = (GFP_ZONE_TABLE >> (bit * GFP_ZONES_SHIFT)) &
					 ((1 << GFP_ZONES_SHIFT) - 1);
	VM_BUG_ON((GFP_ZONE_BAD >> bit) & 1);
	return z;
}

/**
 * There is only one page-allocator function, and two main namespaces to
 * it. The alloc_page*() variants return 'struct page *' and as such
 * can allocate highmem pages, the *get*page*() variants return
 * virtual kernel addresses to the allocated page(s).
 * 页面分配器仅有一个核心函数，但提供两类主要接口。
 * alloc_page*()系列函数返回struct page *类型，因此能够分配高端内存页面；
 * 而*get*page*()系列函数则返回已分配页面的虚拟内核地址。
 * 
 * 优先从本地内存分配
 */
static inline int gfp_zonelist(gfp_t flags)
{
#ifdef CONFIG_NUMA
	if (unlikely(flags & __GFP_THISNODE))
		return ZONELIST_NOFALLBACK;
#endif
	return ZONELIST_FALLBACK;
}

/*
 * We get the zone list from the current node and the gfp_mask.
 * This zone list contains a maximum of MAXNODES*MAX_NR_ZONES zones.
 * There are two zonelists per node, one for all zones with memory and
 * one containing just zones from the node the zonelist belongs to.
 * (我们从当前节点和gfp_mask获取区域列表。该区域列表最多包含MAXNODES*MAX_NR_ZONES个区域。
 * 每个节点有两个区域列表：一个包含所有具有内存的区域，另一个仅包含该节点所属区域的区域列表) ?
 *
 * For the normal case of non-DISCONTIGMEM systems the NODE_DATA() gets
 * optimized to &contig_page_data at compile-time.
 * (在非间断内存（非DISCONTIGMEM）系统的常规情况下，
 * NODE_DATA()会在编译时被优化为&contig_page_data) ?
 */
static inline struct zonelist *node_zonelist(int nid, gfp_t flags)
{
	// 选择zonelist , 选择本地内存还是远端内存
	return NODE_DATA(nid)->node_zonelists + gfp_zonelist(flags);
}

#ifndef HAVE_ARCH_FREE_PAGE
static inline void arch_free_page(struct page *page, int order) { }
#endif
#ifndef HAVE_ARCH_ALLOC_PAGE
static inline void arch_alloc_page(struct page *page, int order) { }
#endif
#ifndef HAVE_ARCH_MAKE_PAGE_ACCESSIBLE
static inline int arch_make_page_accessible(struct page *page)
{
	return 0;
}
#endif

struct page *
__alloc_pages_nodemask(gfp_t gfp_mask, unsigned int order, int preferred_nid,
							nodemask_t *nodemask);


/**
 * 分配2^order个连续物理页面
 * 
 * @param perferred_nid preferred_nid（Preferred Node ID）通常用于 NUMA 架构，表示优先从哪个 NUMA 节点（Node）分配内存
 * 
 * @return 第一个物理页面的page数据结构
 */
static inline struct page *
__alloc_pages(gfp_t gfp_mask, unsigned int order, int preferred_nid)
{
	return __alloc_pages_nodemask(gfp_mask, order, preferred_nid, NULL);
}

/**
 * Allocate pages, preferring the node given as nid. The node must be valid and
 * online. For more general interface, see alloc_pages_node().
 * (分配内存页(2^order个物理页)，优先选择给定的节点（nid）。该节点必须有效且在线。更通用的接口请参见 alloc_pages_node())
 */
static inline struct page *
__alloc_pages_node(int nid, gfp_t gfp_mask, unsigned int order)
{
	VM_BUG_ON(nid < 0 || nid >= MAX_NUMNODES);
	VM_WARN_ON((gfp_mask & __GFP_THISNODE) && !node_online(nid));

	return __alloc_pages(gfp_mask, order, nid);
}

/*
 * Allocate pages, preferring the node given as nid. When nid == NUMA_NO_NODE,
 * prefer the current CPU's closest node. Otherwise node must be valid and
 * online.
 */
static inline struct page *alloc_pages_node(int nid, gfp_t gfp_mask,
						unsigned int order)
{
	if (nid == NUMA_NO_NODE)
		nid = numa_mem_id();

	return __alloc_pages_node(nid, gfp_mask, order);
}

#ifdef CONFIG_NUMA
extern struct page *alloc_pages_current(gfp_t gfp_mask, unsigned order);

/**
 * 分配物理页面 , 分配2^order个物理页面
 */
static inline struct page *alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	return alloc_pages_current(gfp_mask, order);
}
/**
 * [mm/mempolicy.c]?
 */
extern struct page *alloc_pages_vma(gfp_t gfp_mask, int order,
			struct vm_area_struct *vma, unsigned long addr,
			int node, bool hugepage);
#define alloc_hugepage_vma(gfp_mask, vma, addr, order) \
	alloc_pages_vma(gfp_mask, order, vma, addr, numa_node_id(), true)
#else
#define alloc_pages(gfp_mask, order) \
		alloc_pages_node(numa_node_id(), gfp_mask, order)
#define alloc_pages_vma(gfp_mask, order, vma, addr, node, false)\
	alloc_pages(gfp_mask, order)
#define alloc_hugepage_vma(gfp_mask, vma, addr, order) \
	alloc_pages(gfp_mask, order)
#endif
#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)
#define alloc_page_vma(gfp_mask, vma, addr)			\
	alloc_pages_vma(gfp_mask, 0, vma, addr, numa_node_id(), false)
#define alloc_page_vma_node(gfp_mask, vma, addr, node)		\
	alloc_pages_vma(gfp_mask, 0, vma, addr, node, false)

extern unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order);
extern unsigned long get_zeroed_page(gfp_t gfp_mask);

void *alloc_pages_exact(size_t size, gfp_t gfp_mask);
void free_pages_exact(void *virt, size_t size);
void * __meminit alloc_pages_exact_nid(int nid, size_t size, gfp_t gfp_mask);

#define __get_free_page(gfp_mask) \
		__get_free_pages((gfp_mask), 0)

#define __get_dma_pages(gfp_mask, order) \
		__get_free_pages((gfp_mask) | GFP_DMA, (order))

extern void __free_pages(struct page *page, unsigned int order);
extern void free_pages(unsigned long addr, unsigned int order);
extern void free_unref_page(struct page *page);
extern void free_unref_page_list(struct list_head *list);

struct page_frag_cache;
extern void __page_frag_cache_drain(struct page *page, unsigned int count);
extern void *page_frag_alloc(struct page_frag_cache *nc,
			     unsigned int fragsz, gfp_t gfp_mask);
extern void page_frag_free(void *addr);

#define __free_page(page) __free_pages((page), 0)
#define free_page(addr) free_pages((addr), 0)

void page_alloc_init(void);
void drain_zone_pages(struct zone *zone, struct per_cpu_pages *pcp);
void drain_all_pages(struct zone *zone);
void drain_local_pages(struct zone *zone);

void page_alloc_init_late(void);

/*
 * gfp_allowed_mask is set to GFP_BOOT_MASK during early boot to restrict what
 * GFP flags are used before interrupts are enabled. Once interrupts are
 * enabled, it is set to __GFP_BITS_MASK while the system is running. During
 * hibernation, it is used by PM to avoid I/O during memory allocation while
 * devices are suspended.
 */
extern gfp_t gfp_allowed_mask;

/* Returns true if the gfp_mask allows use of ALLOC_NO_WATERMARK */
bool gfp_pfmemalloc_allowed(gfp_t gfp_mask);

extern void pm_restrict_gfp_mask(void);
extern void pm_restore_gfp_mask(void);

#ifdef CONFIG_PM_SLEEP
extern bool pm_suspended_storage(void);
#else
static inline bool pm_suspended_storage(void)
{
	return false;
}
#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_CONTIG_ALLOC
/* The below functions must be run on a range from a single zone. */
extern int alloc_contig_range(unsigned long start, unsigned long end,
			      unsigned migratetype, gfp_t gfp_mask);
extern struct page *alloc_contig_pages(unsigned long nr_pages, gfp_t gfp_mask,
				       int nid, nodemask_t *nodemask);
#endif
void free_contig_range(unsigned long pfn, unsigned int nr_pages);

#ifdef CONFIG_CMA
/* CMA stuff */
extern void init_cma_reserved_pageblock(struct page *page);
#endif

#endif /* __LINUX_GFP_H */
