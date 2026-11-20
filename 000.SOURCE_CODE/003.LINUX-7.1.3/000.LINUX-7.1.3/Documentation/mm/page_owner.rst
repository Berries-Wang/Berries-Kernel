================================================================================
page owner: Tracking about who allocated each page(page owner：跟踪每页的分配者)
================================================================================

Introduction(引言)
==================

page owner is for the tracking about who allocated each page.
It can be used to debug memory leak or to find a memory hogger.
When allocation happens, information about allocation such as call stack
and order of pages is stored into certain storage for each page.
When we need to know about status of all pages, we can get and analyze
this information.
(page owner 用于跟踪每页的分配者。它可以用来调试内存泄漏或找出内存占用大户。
当发生分配时，诸如调用栈和页阶 (order) 等分配信息会按页存入特定的存储区。当我们
需要了解所有页的状态时，就可以获取并分析这些信息。)

Although we already have tracepoint for tracing page allocation/free,
using it for analyzing who allocate each page is rather complex. We need
to enlarge the trace buffer for preventing overlapping until userspace
program launched. And, launched program continually dump out the trace
buffer for later analysis and it would change system behaviour with more
possibility rather than just keeping it in memory, so bad for debugging.
(虽然我们已经有用于跟踪页分配/释放的 tracepoint，但用它来分析每页的分配者相当
复杂。我们需要扩大 trace 缓冲区，以免在用户空间程序启动之前发生记录相互覆盖。
而启动后的程序要不断把 trace 缓冲区导出以供后续分析，这比仅仅把它保留在内存中
更有可能改变系统行为，不利于调试。)

page owner can also be used for various purposes. For example, accurate
fragmentation statistics can be obtained through gfp flag information of
each page. It is already implemented and activated if page owner is
enabled. Other usages are more than welcome.
(page owner 还可用于多种其他目的。例如，通过每页的 gfp 标志信息可以获得精确的
碎片统计。这一功能已经实现，只要启用了 page owner 即会激活。欢迎更多的用法。)

It can also be used to show all the stacks and their current number of
allocated base pages, which gives us a quick overview of where the memory
is going without the need to screen through all the pages and match the
allocation and free operation. It's also possible to show only a numeric
identifier of all the stacks (without stack traces) and their number of
allocated base pages (faster to read and parse, eg, for monitoring) that
can be matched with stacks later (show_handles and show_stacks_handles).
(它还可以用来显示所有调用栈及其当前分配的基准页 (base page) 数量，让我们无需
逐页筛查并匹配分配与释放操作，就能快速了解内存的去向。也可以只显示所有调用栈
的数字标识符(不含栈回溯)及其分配的基准页数(读取和解析更快，例如用于监控)，
之后再与调用栈进行匹配 (show_handles 和 show_stacks_handles)。)

page owner is disabled by default. So, if you'd like to use it, you need
to add "page_owner=on" to your boot cmdline. If the kernel is built
with page owner and page owner is disabled in runtime due to not enabling
boot option, runtime overhead is marginal. If disabled in runtime, it
doesn't require memory to store owner information, so there is no runtime
memory overhead. And, page owner inserts just two unlikely branches into
the page allocator hotpath and if not enabled, then allocation is done
like as the kernel without page owner. These two unlikely branches should
not affect to allocation performance, especially if the static keys jump
label patching functionality is available. Following is the kernel's code
size change due to this facility.
(page owner 默认禁用。因此，如果想使用它，需要在启动命令行中加入
"page_owner=on"。如果内核编译时带上了 page owner，但因未启用启动选项而在运行
时处于禁用状态，运行时开销极小。运行时禁用时，它不需要内存来存储所有者信息，
因此没有运行时内存开销。而且，page owner 只在页分配器热路径中插入两个 unlikely
分支；若未启用，分配行为与没有 page owner 的内核完全一样。这两个 unlikely
分支不应影响分配性能，尤其是在静态键跳转标签 (static keys jump label) 修补
功能可用的场合。下面是由于这一设施导致的内核代码体积变化。)

Although enabling page owner increases kernel size by several kilobytes,
most of this code is outside page allocator and its hot path. Building
the kernel with page owner and turning it on if needed would be great
option to debug kernel memory problem.
(虽然启用 page owner 会使内核体积增加几 KB，但其中大部分代码位于页分配器及其
热路径之外。编译内核时带上 page owner 并在需要时打开它，是调试内核内存问题的
绝佳选择。)

There is one notice that is caused by implementation detail. page owner
stores information into the memory from struct page extension. This memory
is initialized some time later than that page allocator starts in sparse
memory system, so, until initialization, many pages can be allocated and
they would have no owner information. To fix it up, these early allocated
pages are investigated and marked as allocated in initialization phase.
Although it doesn't mean that they have the right owner information,
at least, we can tell whether the page is allocated or not,
more accurately. On 2GB memory x86-64 VM box, 13343 early allocated pages
are caught and marked, although they are mostly allocated from struct
page extension feature. Anyway, after that, no page is left in
un-tracking state.
(有一个由实现细节导致的注意事项。page owner 把信息存入来自 struct page 扩展的
内存。在稀疏 (sparse) 内存系统中，这块内存的初始化比页分配器的启动要晚一些，
因此在初始化完成之前，许多页可能已被分配且没有所有者信息。为修复这一点，这些
早期分配的页会在初始化阶段被核查并标记为"已分配"。虽然这并不意味着它们拥有
正确的所有者信息，但至少我们能更准确地判断页是否已被分配。在一台 2GB 内存的
x86-64 虚拟机上，共发现并标记了 13343 个早期分配的页，尽管它们大多来自
struct page extension 特性。无论如何，此后不再有页处于无法跟踪的状态。)

Usage(用法)
===========

1) Build user-space helper::

	cd tools/mm
	make page_owner_sort

   (编译用户空间辅助工具：)

2) Enable page owner: add "page_owner=on" to boot cmdline.
   (启用 page owner：在启动命令行中添加 "page_owner=on"。)

3) Do the job that you want to debug.
   (执行你想要调试的工作。)

4) Analyze information from page owner::

	cat /sys/kernel/debug/page_owner_stacks/show_stacks > stacks.txt
	cat stacks.txt
	 post_alloc_hook+0x177/0x1a0
	 get_page_from_freelist+0xd01/0xd80
	 __alloc_pages+0x39e/0x7e0
	 allocate_slab+0xbc/0x3f0
	 ___slab_alloc+0x528/0x8a0
	 kmem_cache_alloc+0x224/0x3b0
	 sk_prot_alloc+0x58/0x1a0
	 sk_alloc+0x32/0x4f0
	 inet_create+0x427/0xb50
	 __sock_create+0x2e4/0x650
	 inet_ctl_sock_create+0x30/0x180
	 igmp_net_init+0xc1/0x130
	 ops_init+0x167/0x410
	 setup_net+0x304/0xa60
	 copy_net_ns+0x29b/0x4a0
	 create_new_namespaces+0x4a1/0x820
	nr_base_pages: 16
	...
	...
	echo 7000 > /sys/kernel/debug/page_owner_stacks/count_threshold
	cat /sys/kernel/debug/page_owner_stacks/show_stacks> stacks_7000.txt
	cat stacks_7000.txt
	 post_alloc_hook+0x177/0x1a0
	 get_page_from_freelist+0xd01/0xd80
	 __alloc_pages+0x39e/0x7e0
	 alloc_pages_mpol+0x22e/0x490
	 folio_alloc+0xd5/0x110
	 filemap_alloc_folio+0x78/0x230
	 page_cache_ra_order+0x287/0x6f0
	 filemap_get_pages+0x517/0x1160
	 filemap_read+0x304/0x9f0
	 xfs_file_buffered_read+0xe6/0x1d0 [xfs]
	 xfs_file_read_iter+0x1f0/0x380 [xfs]
	 __kernel_read+0x3b9/0x730
	 kernel_read_file+0x309/0x4d0
	 __do_sys_finit_module+0x381/0x730
	 do_syscall_64+0x8d/0x150
	 entry_SYSCALL_64_after_hwframe+0x62/0x6a
	nr_base_pages: 20824
	...

	cat /sys/kernel/debug/page_owner_stacks/show_handles > handles_7000.txt
	cat handles_7000.txt
	handle: 42
	nr_base_pages: 20824
	...

	cat /sys/kernel/debug/page_owner_stacks/show_stacks_handles > stacks_handles.txt
	cat stacks_handles.txt
	 post_alloc_hook+0x177/0x1a0
	 get_page_from_freelist+0xd01/0xd80
	 __alloc_pages+0x39e/0x7e0
	 alloc_pages_mpol+0x22e/0x490
	 folio_alloc+0xd5/0x110
	 filemap_alloc_folio+0x78/0x230
	 page_cache_ra_order+0x287/0x6f0
	 filemap_get_pages+0x517/0x1160
	 filemap_read+0x304/0x9f0
	 xfs_file_buffered_read+0xe6/0x1d0 [xfs]
	 xfs_file_read_iter+0x1f0/0x380 [xfs]
	 __kernel_read+0x3b9/0x730
	 kernel_read_file+0x309/0x4d0
	 __do_sys_finit_module+0x381/0x730
	 do_syscall_64+0x8d/0x150
	 entry_SYSCALL_64_after_hwframe+0x62/0x6a
	handle: 42
	...

	cat /sys/kernel/debug/page_owner > page_owner_full.txt
	./page_owner_sort page_owner_full.txt sorted_page_owner.txt

   (从 page owner 分析信息：)

   The general output of ``page_owner_full.txt`` is as follows::

	Page allocated via order XXX, ...
	PFN XXX ...
	// Detailed stack

	Page allocated via order XXX, ...
	PFN XXX ...
	// Detailed stack
    By default, it will do full pfn dump, to start with a given pfn,
    page_owner supports fseek.

    FILE *fp = fopen("/sys/kernel/debug/page_owner", "r");
    fseek(fp, pfn_start, SEEK_SET);

   (``page_owner_full.txt`` 的一般输出如下：)

   The ``page_owner_sort`` tool ignores ``PFN`` rows, puts the remaining rows
   in buf, uses regexp to extract the page order value, counts the times
   and pages of buf, and finally sorts them according to the parameter(s).
   (``page_owner_sort`` 工具忽略 ``PFN`` 行，把其余行放入 buf，用正则表达式
   提取页阶 (page order) 值，统计 buf 的次数与页数，最后按参数进行排序。)

   See the result about who allocated each page
   in the ``sorted_page_owner.txt``. General output::

	XXX times, XXX pages:
	Page allocated via order XXX, ...
	// Detailed stack

   (在 ``sorted_page_owner.txt`` 中查看谁分配了每一页的结果。一般输出如下：)

   By default, ``page_owner_sort`` is sorted according to the times of buf.
   If you want to sort by the page nums of buf, use the ``-m`` parameter.
   The detailed parameters are:
   (默认情况下，``page_owner_sort`` 按 buf 的次数排序。如果想按 buf 的页数
   排序，请使用 ``-m`` 参数。详细参数如下：)

   fundamental function::

	Sort:
		-a		Sort by memory allocation time.
		-m		Sort by total memory.
		-p		Sort by pid.
		-P		Sort by tgid.
		-n		Sort by task command name.
		-r		Sort by memory release time.
		-s		Sort by stack trace.
		-t		Sort by times (default).
		--sort <order>	Specify sorting order.  Sorting syntax is [+|-]key[,[+|-]key[,...]].
				Choose a key from the **STANDARD FORMAT SPECIFIERS** section. The "+" is
				optional since default direction is increasing numerical or lexicographic
				order. Mixed use of abbreviated and complete-form of keys is allowed.

			Examples:
					./page_owner_sort <input> <output> --sort=n,+pid,-tgid
					./page_owner_sort <input> <output> --sort=at

   (基本功能：)

   additional function::

	Cull:
		--cull <rules>
				Specify culling rules.Culling syntax is key[,key[,...]].Choose a
				multi-letter key from the **STANDARD FORMAT SPECIFIERS** section.

		<rules> is a single argument in the form of a comma-separated list,
		which offers a way to specify individual culling rules.  The recognized
		keywords are described in the **STANDARD FORMAT SPECIFIERS** section below.
		<rules> can be specified by the sequence of keys k1,k2, ..., as described in
		the STANDARD SORT KEYS section below. Mixed use of abbreviated and
		complete-form of keys is allowed.

		Examples:
				./page_owner_sort <input> <output> --cull=stacktrace
				./page_owner_sort <input> <output> --cull=st,pid,name
				./page_owner_sort <input> <output> --cull=n,f

	Filter:
		-f		Filter out the information of blocks whose memory has been released.

	Select:
		--pid <pidlist>		Select by pid. This selects the blocks whose process ID
					numbers appear in <pidlist>.
		--tgid <tgidlist>	Select by tgid. This selects the blocks whose thread
					group ID numbers appear in <tgidlist>.
		--name <cmdlist>	Select by task command name. This selects the blocks whose
					task command name appear in <cmdlist>.

		<pidlist>, <tgidlist>, <cmdlist> are single arguments in the form of a comma-separated list,
		which offers a way to specify individual selecting rules.


			Examples:
					./page_owner_sort <input> <output> --pid=1
					./page_owner_sort <input> <output> --tgid=1,2,3
					./page_owner_sort <input> <output> --name name1,name2

   (附加功能：)

STANDARD FORMAT SPECIFIERS(标准格式说明符)
==========================================
::

  For --sort option:

	KEY		LONG		DESCRIPTION
	p		pid		process ID
	tg		tgid		thread group ID
	n		name		task command name
	st		stacktrace	stack trace of the page allocation
	T		txt		full text of block
	ft		free_ts		timestamp of the page when it was released
	at		alloc_ts	timestamp of the page when it was allocated
	ator		allocator	memory allocator for pages

  For --cull option:

	KEY		LONG		DESCRIPTION
	p		pid		process ID
	tg		tgid		thread group ID
	n		name		task command name
	f		free		whether the page has been released or not
	st		stacktrace	stack trace of the page allocation
	ator		allocator	memory allocator for pages
