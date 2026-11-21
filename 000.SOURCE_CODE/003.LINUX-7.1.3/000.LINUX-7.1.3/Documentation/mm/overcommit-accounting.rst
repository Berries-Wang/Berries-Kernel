===================================
Overcommit Accounting(超额提交记账)
===================================

The Linux kernel supports the following overcommit handling modes
(Linux 内核支持以下超额提交处理模式。)

0
	Heuristic overcommit handling. Obvious overcommits of address
	space are refused. Used for a typical system. It ensures a
	seriously wild allocation fails while allowing overcommit to
	reduce swap usage. This is the default.
	(启发式超额提交处理。明显超出地址空间的超额提交会被拒绝。用于典型
	系统。它确保严重失控的分配失败，同时允许超额提交以减少交换空间的
	使用。这是默认值。)

1
	Always overcommit. Appropriate for some scientific
	applications. Classic example is code using sparse arrays and
	just relying on the virtual memory consisting almost entirely
	of zero pages.
	(总是超额提交。适用于某些科学应用。经典的例子是使用稀疏数组、仅
	依赖于几乎完全由零页组成的虚拟内存的代码。)

2
	Don't overcommit. The total address space commit for the
	system is not permitted to exceed swap + a configurable amount
	(default is 50%) of physical RAM.  Depending on the amount you
	use, in most situations this means a process will not be
	killed while accessing pages but will receive errors on memory
	allocation as appropriate.
	(不超额提交。系统的地址空间提交总量不允许超过交换空间 + 物理内存
	的一个可配置数额 (默认为 50%)。根据你所使用的数额，在大多数情况
	下，这意味着进程在访问页面时不会被杀死，但会在内存分配时适当地收
	到错误。)

	Useful for applications that want to guarantee their memory
	allocations will be available in the future without having to
	initialize every page.
	(对于希望保证其内存分配将来可用而不必初始化每个页面的应用程序很有
	用。)

The overcommit policy is set via the sysctl ``vm.overcommit_memory``.
(超额提交策略通过 sysctl ``vm.overcommit_memory`` 设置。)

The overcommit amount can be set via ``vm.overcommit_ratio`` (percentage)
or ``vm.overcommit_kbytes`` (absolute value). These only have an effect
when ``vm.overcommit_memory`` is set to 2.
(超额提交数额可通过 ``vm.overcommit_ratio`` (百分比) 或
``vm.overcommit_kbytes`` (绝对值) 设置。它们只在 ``vm.overcommit_memory``
被设为 2 时才生效。)

The current overcommit limit and amount committed are viewable in
``/proc/meminfo`` as CommitLimit and Committed_AS respectively.
(当前的超额提交限额与已提交数额可在 ``/proc/meminfo`` 中分别以
CommitLimit 和 Committed_AS 查看。)

Gotchas(注意事项)
=================

The C language stack growth does an implicit mremap. If you want absolute
guarantees and run close to the edge you MUST mmap your stack for the
largest size you think you will need. For typical stack usage this does
not matter much but it's a corner case if you really really care
(C 语言的栈增长会执行一次隐式的 mremap。如果你想要绝对的保证并且运行在
极限边缘，你 *必须* mmap 你认为所需最大尺寸的栈。对于典型的栈使用方式，
这无关紧要，但如果你真的非常在意，它就是一个边界情况。)

In mode 2 the MAP_NORESERVE flag is ignored.
(在模式 2 中，MAP_NORESERVE 标志会被忽略。)


How It Works(工作原理)
======================

The overcommit is based on the following rules
(超额提交基于以下规则。)

For a file backed map
(针对基于文件的映射)

	| SHARED or READ-only	-	0 cost (the file is the map not swap)
	| PRIVATE WRITABLE	-	size of mapping per instance

For an anonymous or ``/dev/zero`` map
(针对匿名映射或 ``/dev/zero`` 映射)

	| SHARED			-	size of mapping
	| PRIVATE READ-only	-	0 cost (but of little use)
	| PRIVATE WRITABLE	-	size of mapping per instance

Additional accounting
(额外记账)

	| Pages made writable copies by mmap
	| shmfs memory drawn from the same pool

Status(现状)
============

*	We account mmap memory mappings
	(我们记账 mmap 内存映射。)
*	We account mprotect changes in commit
	(我们把 mprotect 的变化计入 commit。)
*	We account mremap changes in size
	(我们记账大小的 mremap 变化。)
*	We account brk
	(我们记账 brk。)
*	We account munmap
	(我们记账 munmap。)
*	We report the commit status in /proc
	(我们在 /proc 中报告 commit 状态。)
*	Account and check on fork
	(在 fork 时记账并检查。)
*	Review stack handling/building on exec
	(审查 exec 时的栈处理/构建。)
*	SHMfs accounting
	(SHMfs 记账。)
*	Implement actual limit enforcement
	(实现实际的限额强制执行。)

To Do(待办事项)
===============
*	Account ptrace pages (this is hard)
	(记账 ptrace 页面 (这很难)。)
