==================
hwpoison(硬件投毒)
==================

What is hwpoison?(什么是 hwpoison?)
===================================

Upcoming Intel CPUs have support for recovering from some memory errors
(``MCA recovery``). This requires the OS to declare a page "poisoned",
kill the processes associated with it and avoid using it in the future.
(即将到来的 Intel CPU 支持从某些内存错误中恢复(``MCA recovery``)。这要求操作系统把一个页声明为"已投毒"(poisoned)，杀掉与它关联的进程，并避免今后再使用它。)

This patchkit implements the necessary infrastructure in the VM.
(本补丁集在 VM 中实现了必要的基础设施。)

To quote the overview comment::

	High level machine check handler. Handles pages reported by the
	hardware as being corrupted usually due to a 2bit ECC memory or cache
	failure.

	This focusses on pages detected as corrupted in the background.
	When the current CPU tries to consume corruption the currently
	running process can just be killed directly instead. This implies
	that if the error cannot be handled for some reason it's safe to
	just ignore it because no corruption has been consumed yet. Instead
	when that happens another machine check will happen.

	Handles page cache pages in various states. The tricky part
	here is that we can access any page asynchronous to other VM
	users, because memory failures could happen anytime and anywhere,
	possibly violating some of their assumptions. This is why this code
	has to be extremely careful. Generally it tries to use normal locking
	rules, as in get the standard locks, even if that means the
	error handling takes potentially a long time.

	Some of the operations here are somewhat inefficient and have non
	linear algorithmic complexity, because the data structures have not
	been optimized for this case. This is in particular the case
	for the mapping from a vma to a process. Since this case is expected
	to be rare we hope we can get away with this.

(摘自总览注释：)

The code consists of the high level handler in mm/memory-failure.c,
a new page poison bit and various checks in the VM to handle poisoned
pages.
(这套代码由 mm/memory-failure.c 中的高层处理程序、一个新的页投毒位以及 VM 中处理已投毒页的各种检查组成。)

The main target right now is KVM guests, but it works for all kinds
of applications. KVM support requires a recent qemu-kvm release.
(当前的主要目标是 KVM 客户机，但它对所有类型的应用都有效。KVM 支持需要较新的 qemu-kvm 发行版。)

For the KVM use there was need for a new signal type so that
KVM can inject the machine check into the guest with the proper
address. This in theory allows other applications to handle
memory failures too. The expectation is that most applications
won't do that, but some very specialized ones might.
(就 KVM 用途而言，需要一种新的信号类型，使 KVM 能把机器检查连同正确的地址注入客户机。理论上这也允许其他应用处理内存故障。预期大多数应用不会这样做，但某些非常专门化的应用可能会。)

Failure recovery modes(故障恢复模式)
====================================

There are two (actually three) modes memory failure recovery can be in:
(内存故障恢复可以处于两种(实际上是三种)模式：)

vm.memory_failure_recovery sysctl set to zero:
	All memory failures cause a panic. Do not attempt recovery.
	(vm.memory_failure_recovery sysctl 置零：
	所有内存故障都触发 panic。不尝试恢复。)

early kill
	(can be controlled globally and per process)
	Send SIGBUS to the application as soon as the error is detected
	This allows applications who can process memory errors in a gentle
	way (e.g. drop affected object)
	This is the mode used by KVM qemu.
	(early kill
	(可以全局控制，也可以按进程控制)
	一旦检测到错误就立即向应用发送 SIGBUS。
	这让能够以温和方式处理内存错误的应用受益(例如丢弃受影响的对象)。
	这是 KVM qemu 使用的模式。)

late kill
	Send SIGBUS when the application runs into the corrupted page.
	This is best for memory error unaware applications and default
	Note some pages are always handled as late kill.
	(late kill
	当应用真正访问到已损坏的页时才发送 SIGBUS。
	这对感知不到内存错误的应用最合适，也是默认模式。
	注意：某些页总是按 late kill 处理。)

User control(用户控制)
======================

vm.memory_failure_recovery
	See sysctl.txt

vm.memory_failure_early_kill
	Enable early kill mode globally
	(全局启用 early kill 模式)

PR_MCE_KILL
	Set early/late kill mode/revert to system default
	(设置 early/late kill 模式/恢复为系统默认)

	arg1: PR_MCE_KILL_CLEAR:
		Revert to system default
		(恢复为系统默认)
	arg1: PR_MCE_KILL_SET:
		arg2 defines thread specific mode
		(arg2 定义线程专属模式)

		PR_MCE_KILL_EARLY:
			Early kill
			(early kill)
		PR_MCE_KILL_LATE:
			Late kill
			(late kill)
		PR_MCE_KILL_DEFAULT
			Use system global default
			(使用系统全局默认)

	Note that if you want to have a dedicated thread which handles
	the SIGBUS(BUS_MCEERR_AO) on behalf of the process, you should
	call prctl(PR_MCE_KILL_EARLY) on the designated thread. Otherwise,
	the SIGBUS is sent to the main thread.
	(注意：如果你想用一个专门线程代表进程处理 SIGBUS(BUS_MCEERR_AO)，应当在指定线程上调用 prctl(PR_MCE_KILL_EARLY)。否则，SIGBUS 会发给主线程。)

PR_MCE_KILL_GET
	return current mode
	(返回当前模式)

Testing
=======

* madvise(MADV_HWPOISON, ....) (as root) - Poison a page in the
  process for testing
  (madvise(MADV_HWPOISON, ....)(以 root 身份)- 把进程中某个页投毒，用于测试)

* hwpoison-inject module through debugfs ``/sys/kernel/debug/hwpoison/``
  (通过 debugfs ``/sys/kernel/debug/hwpoison/`` 使用 hwpoison-inject 模块)

  corrupt-pfn
	Inject hwpoison fault at PFN echoed into this file. This does
	some early filtering to avoid corrupted unintended pages in test suites.
	(向本文件回显一个 PFN，在该 PFN 处注入 hwpoison 故障。它会做一些早期过滤，避免测试套件中非预期页被损坏。)

  unpoison-pfn
	Software-unpoison page at PFN echoed into this file. This way
	a page can be reused again.  This only works for Linux
	injected failures, not for real memory failures. Once any hardware
	memory failure happens, this feature is disabled.
	(向本文件回显一个 PFN，以软件方式解除该页的投毒，使该页可再次使用。这只对 Linux 注入的故障有效，对真实内存故障无效。一旦发生任何硬件内存故障，该特性即被禁用。)

  Note these injection interfaces are not stable and might change between
  kernel versions
  (注意：这些注入接口并不稳定，可能在内核版本之间发生变化)

  corrupt-filter-dev-major, corrupt-filter-dev-minor
	Only handle memory failures to pages associated with the file
	system defined by block device major/minor.  -1U is the
	wildcard value.  This should be only used for testing with
	artificial injection.
	(只处理与块设备主/次设备号所定义文件系统关联的页的内存故障。-1U 为通配值。这只应用于人工注入的测试。)

  corrupt-filter-memcg
	Limit injection to pages owned by memgroup. Specified by inode
	number of the memcg.
	(把注入限制在 memgroup 拥有的页上。由该 memcg 的 inode 号指定。)

	Example::

		mkdir /sys/fs/cgroup/mem/hwpoison

	        usemem -m 100 -s 1000 &
		echo `jobs -p` > /sys/fs/cgroup/mem/hwpoison/tasks

		memcg_ino=$(ls -id /sys/fs/cgroup/mem/hwpoison | cut -f1 -d' ')
		echo $memcg_ino > /debug/hwpoison/corrupt-filter-memcg

		page-types -p `pidof init`   --hwpoison  # shall do nothing
		page-types -p `pidof usemem` --hwpoison  # poison its pages

	(示例：)

  corrupt-filter-flags-mask, corrupt-filter-flags-value
	When specified, only poison pages if ((page_flags & mask) ==
	value).  This allows stress testing of many kinds of
	pages. The page_flags are the same as in /proc/kpageflags. The
	flag bits are defined in include/linux/kernel-page-flags.h and
	documented in Documentation/admin-guide/mm/pagemap.rst
	(指定后，仅当 ((page_flags & mask) == value) 时才对页投毒。这允许对多种页进行压力测试。page_flags 与 /proc/kpageflags 中的相同。标志位定义在 include/linux/kernel-page-flags.h 中，并记录于 Documentation/admin-guide/mm/pagemap.rst。)

* Architecture specific MCE injector
  (架构专属的 MCE 注入器)

  x86 has mce-inject, mce-test
  (x86 有 mce-inject 与 mce-test)

  Some portable hwpoison test programs in mce-test, see below.
  (mce-test 中还有一些可移植的 hwpoison 测试程序，见下文。)

References(参考资料)
====================

http://halobates.de/mce-lc09-2.pdf
	Overview presentation from LinuxCon 09
	(LinuxCon 09 的概览演示文稿)

git://git.kernel.org/pub/scm/utils/cpu/mce/mce-test.git
	Test suite (hwpoison specific portable tests in tsrc)
	(测试套件(tsrc 中有 hwpoison 专属的可移植测试))

git://git.kernel.org/pub/scm/utils/cpu/mce/mce-inject.git
	x86 specific injector
	(x86 专属注入器)


Limitations(局限性)
===================
- Not all page types are supported and never will. Most kernel internal
  objects cannot be recovered, only LRU pages for now.
  (并非所有页类型都受支持，将来也不会。大多数内核内部对象无法恢复，目前只支持 LRU 页。)

---
Andi Kleen, Oct 2009
