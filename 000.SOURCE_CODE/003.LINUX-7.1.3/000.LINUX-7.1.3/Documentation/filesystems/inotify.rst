.. SPDX-License-Identifier: GPL-2.0

===============================================================
Inotify - A Powerful yet Simple File Change Notification System
===============================================================

(Inotify - 一个强大而简单的文件更改通知系统)



Document started 15 Mar 2005 by Robert Love <rml@novell.com>
(本文档由 Robert Love <rml@novell.com> 于 2005 年 3 月 15 日开始编写)

Document updated 4 Jan 2015 by Zhang Zhen <zhenzhang.zhang@huawei.com>
(本文档由 Zhang Zhen <zhenzhang.zhang@huawei.com> 于 2015 年 1 月 4 日更新)

	- Deleted obsoleted interface, just refer to manpages for user interface.
	  (删除了已过时的接口，用户接口请参阅手册页。)

(i) Rationale
((i) 理由)

Q:
   What is the design decision behind not tying the watch to the open fd of
   the watched object?
   (将监视项(watch)与被监视对象的已打开 fd 绑定这一设计决策背后的考量是什么？)

A:
   Watches are associated with an open inotify device, not an open file.
   This solves the primary problem with dnotify: keeping the file open pins
   the file and thus, worse, pins the mount.  Dnotify is therefore infeasible
   for use on a desktop system with removable media as the media cannot be
   unmounted.  Watching a file should not require that it be open.
   (监视项与已打开的 inotify 设备相关联，而不是与打开的文件相关联。这解决了 dnotify 的主要问题：保持文件打开会固定(pin)该文件，更糟的是，还会固定挂载点。因此，dnotify 在使用可移动介质的桌面系统上不可行，因为介质无法被卸载。监视一个文件不应该要求它处于打开状态。)

Q:
   What is the design decision behind using an-fd-per-instance as opposed to
   an fd-per-watch?
   (使用每实例一个 fd 而不是每个监视项一个 fd 这一设计决策背后的考量是什么？)

A:
   An fd-per-watch quickly consumes more file descriptors than are allowed,
   more fd's than are feasible to manage, and more fd's than are optimally
   select()-able.  Yes, root can bump the per-process fd limit and yes, users
   can use epoll, but requiring both is a silly and extraneous requirement.
   A watch consumes less memory than an open file, separating the number
   spaces is thus sensible.  The current design is what user-space developers
   want: Users initialize inotify, once, and add n watches, requiring but one
   fd and no twiddling with fd limits.  Initializing an inotify instance two
   thousand times is silly.  If we can implement user-space's preferences
   cleanly--and we can, the idr layer makes stuff like this trivial--then we
   should.
   (每个监视项一个 fd 会很快消耗超过允许数量的文件描述符、多到难以管理的 fd，以及多到不适合 select() 的 fd。是的，root 可以提高每进程的 fd 限制，是的，用户可以使用 epoll，但同时要求这两者是一个愚蠢且多余的要求。监视项比打开的文件消耗更少的内存，因此将两个编号空间分开是合理的。当前的设计正是用户空间开发者想要的：用户只需初始化一次 inotify，然后添加 n 个监视项，只需要一个 fd，也无需折腾 fd 限制。把 inotify 实例初始化两千次是愚蠢的。如果我们能够干净利落地实现用户空间的偏好——我们可以，idr 层使这类事情变得轻而易举——那么我们就应该这么做。)

   There are other good arguments.  With a single fd, there is a single
   item to block on, which is mapped to a single queue of events.  The single
   fd returns all watch events and also any potential out-of-band data.  If
   every fd was a separate watch,
   (还有其他好的理由。使用单个 fd，就只有一个可阻塞的对象，它映射到单个事件队列。单个 fd 返回所有监视项事件以及任何潜在的带外数据。如果每个 fd 都是单独的监视项，)

   - There would be no way to get event ordering.  Events on file foo and
     file bar would pop poll() on both fd's, but there would be no way to tell
     which happened first.  A single queue trivially gives you ordering.  Such
     ordering is crucial to existing applications such as Beagle.  Imagine
     "mv a b ; mv b a" events without ordering.
     (将无法获得事件顺序。文件 foo 和文件 bar 上的事件会在两个 fd 上都弹出 poll()，但无法分辨哪个先发生。单个队列轻而易举地为你提供顺序。这种顺序对 Beagle 等现有应用至关重要。想象一下没有顺序的 "mv a b ; mv b a" 事件。)

   - We'd have to maintain n fd's and n internal queues with state,
     versus just one.  It is a lot messier in the kernel.  A single, linear
     queue is the data structure that makes sense.
     (我们将不得不维护 n 个 fd 和 n 个带状态的内部队列，而不是只有一个。这在内核中要混乱得多。单一的线性队列才是合理的数据结构。)

   - User-space developers prefer the current API.  The Beagle guys, for
     example, love it.  Trust me, I asked.  It is not a surprise: Who'd want
     to manage and block on 1000 fd's via select?
     (用户空间开发者更喜欢当前的 API。例如 Beagle 的人就很喜欢它。相信我，我问过。这并不奇怪：谁会想通过 select 管理并阻塞在 1000 个 fd 上呢？)

   - No way to get out of band data.
     (无法获得带外数据。)

   - 1024 is still too low.  ;-)
     (1024 仍然太低。;-))

   When you talk about designing a file change notification system that
   scales to 1000s of directories, juggling 1000s of fd's just does not seem
   the right interface.  It is too heavy.
   (当你要设计一个可扩展到数千个目录的文件更改通知系统时，同时应付数千个 fd 似乎并不是正确的接口。它太重了。)

   Additionally, it _is_ possible to  more than one instance  and
   juggle more than one queue and thus more than one associated fd.  There
   need not be a one-fd-per-process mapping; it is one-fd-per-queue and a
   process can easily want more than one queue.
   (此外，确实可以创建多于一个实例并应付多个队列，从而应付多个关联的 fd。不必是每进程一个 fd 的映射；它是每队列一个 fd，而一个进程很容易想要多于一个队列。)

Q:
   Why the system call approach?
   (为什么采用系统调用的方式？)

A:
   The poor user-space interface is the second biggest problem with dnotify.
   Signals are a terrible, terrible interface for file notification.  Or for
   anything, for that matter.  The ideal solution, from all perspectives, is a
   file descriptor-based one that allows basic file I/O and poll/select.
   Obtaining the fd and managing the watches could have been done either via a
   device file or a family of new system calls.  We decided to implement a
   family of system calls because that is the preferred approach for new kernel
   interfaces.  The only real difference was whether we wanted to use open(2)
   and ioctl(2) or a couple of new system calls.  System calls beat ioctls.
   (糟糕的用户空间接口是 dnotify 的第二大问题。信号是一种糟糕透顶的文件通知接口——事实上对任何东西都是如此。从所有角度来看，理想的解决方案是基于文件描述符的方案，它允许基本的文件 I/O 和 poll/select。获取 fd 和管理监视项本可以通过设备文件或一族新的系统调用来完成。我们决定实现一族系统调用，因为这是新内核接口的首选方式。唯一真正的区别在于我们是要使用 open(2) 和 ioctl(2) 还是几个新的系统调用。系统调用胜过 ioctl。)

