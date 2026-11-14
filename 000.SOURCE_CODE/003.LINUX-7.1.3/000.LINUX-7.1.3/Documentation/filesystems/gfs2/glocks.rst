.. SPDX-License-Identifier: GPL-2.0

============================
Glock internal locking rules
============================

(Glock 内部加锁规则)

This documents the basic principles of the glock state machine
internals. Each glock (struct gfs2_glock in fs/gfs2/incore.h)
has two main (internal) locks:
(本文档描述 glock 状态机内部的基本原理。每个 glock(fs/gfs2/incore.h 中的 struct gfs2_glock)有两个主要(内部)锁：)

 1. A spinlock (gl_lockref.lock) which protects the internal state such
    as gl_state, gl_target and the list of holders (gl_holders)
    (自旋锁(gl_lockref.lock)，保护诸如 gl_state、gl_target 以及持有者(holder)列表(gl_holders)之类的内部状态)
 2. A non-blocking bit lock, GLF_LOCK, which is used to prevent other
    threads from making calls to the DLM, etc. at the same time. If a
    thread takes this lock, it must then call run_queue (usually via the
    workqueue) when it releases it in order to ensure any pending tasks
    are completed.
    (非阻塞位锁 GLF_LOCK，用于防止其他线程同时调用 DLM 等。如果某个线程获取了此锁，那么它必须在释放该锁时调用 run_queue(通常通过 workqueue)，以确保所有挂起的任务都得到完成。)

The gl_holders list contains all the queued lock requests (not
just the holders) associated with the glock. If there are any
held locks, then they will be contiguous entries at the head
of the list. Locks are granted in strictly the order that they
are queued.
(gl_holders 列表包含与该 glock 关联的所有已排队的锁请求(不仅仅是持有者)。如果存在任何已持有的锁，那么它们将是列表头部连续的条目。锁严格按照它们排队的顺序被授予。)

There are three lock states that users of the glock layer can request,
namely shared (SH), deferred (DF) and exclusive (EX). Those translate
to the following DLM lock modes:
(glock 层的使用者可以请求三种锁状态，即共享(SH)、延迟(DF)和独占(EX)。它们对应以下 DLM 锁模式：)

==========	====== =====================================================
Glock mode      DLM    lock mode
==========	====== =====================================================
    UN          IV/NL  Unlocked (no DLM lock associated with glock) or NL (未加锁(没有与 glock 关联的 DLM 锁)或 NL)
    SH          PR     (Protected read) (保护读)
    DF          CW     (Concurrent write) (并发写)
    EX          EX     (Exclusive) (独占)

Thus DF is basically a shared mode which is incompatible with the "normal"
shared lock mode, SH. In GFS2 the DF mode is used exclusively for direct I/O
operations. The glocks are basically a lock plus some routines which deal
with cache management. The following rules apply for the cache:
(因此，DF 基本上是一种与"普通"共享锁模式 SH 不兼容的共享模式。在 GFS2 中，DF 模式专门用于直接 I/O 操作。glock 基本上是一个锁加上一些处理缓存管理的例程。以下规则适用于缓存：)

==========      ==============   ==========   ==========   ==============
Glock mode      Cache Metadata   Cache data   Dirty Data   Dirty Metadata
==========      ==============   ==========   ==========   ==============
    UN                No            No            No            No
    DF                Yes           No            No            No
    SH                Yes           Yes           No            No
    EX                Yes           Yes           Yes           Yes
==========      ==============   ==========   ==========   ==============

These rules are implemented using the various glock operations which
are defined for each type of glock. Not all types of glocks use
all the modes. Only inode glocks use the DF mode for example.
(这些规则通过为每种类型的 glock 定义的各种 glock 操作来实现。并非所有类型的 glock 都使用所有模式。例如，只有 inode glock 使用 DF 模式。)

Table of glock operations and per type constants:
(glock 操作与各类型常量表：)

==============     =============================================================
Field              Purpose
==============     =============================================================
go_sync            Called before remote state change (e.g. to sync dirty data) (在远程状态变更之前调用(例如同步脏数据))
go_xmote_bh        Called after remote state change (e.g. to refill cache) (在远程状态变更之后调用(例如重新填充缓存))
go_inval           Called if remote state change requires invalidating the cache (如果远程状态变更需要使缓存无效则调用)
go_instantiate     Called when a glock has been acquired (当 glock 已被获取时调用)
go_held            Called every time a glock holder is acquired (每当获取一个 glock holder 时调用)
go_dump            Called to print content of object for debugfs file, or on
                   error to dump glock to the log. (为 debugfs 文件打印对象内容时调用，或在出错时将 glock 转储到日志中。)
go_callback	   Called if the DLM sends a callback to drop this lock (如果 DLM 发送要求丢弃此锁的回调则调用)
go_unlocked        Called when a glock is unlocked (dlm_unlock()) (当 glock 被解锁时调用(dlm_unlock()))
go_type            The type of the glock, ``LM_TYPE_*`` (glock 的类型，``LM_TYPE_*``)
go_flags	   GLOF_ASPACE is set, if the glock has an address space
                   associated with it (如果 glock 关联有地址空间，则设置 GLOF_ASPACE)
==============     =============================================================

The minimum hold time for each lock is the time after a remote lock
grant for which we ignore remote demote requests. This is in order to
prevent a situation where locks are being bounced around the cluster
from node to node with none of the nodes making any progress. This
tends to show up most with shared mmapped files which are being written
to by multiple nodes. By delaying the demotion in response to a
remote callback, that gives the userspace program time to make
some progress before the pages are unmapped.
(每把锁的最小持有时间是指在远程锁被授予之后的一段时间，在此期间我们会忽略远程的降级请求。这是为了防止锁在集群中从一个节点弹到另一个节点、而没有任何节点取得任何进展的情况。这种情况最常出现在由多个节点写入的共享 mmap 文件上。通过延迟对远程回调的降级响应，可以在页被取消映射之前给用户空间程序留出时间取得一些进展。)

Eventually, we hope to make the glock "EX" mode locally shared such that any
local locking will be done with the i_mutex as required rather than via the
glock.
(最终，我们希望让 glock 的 "EX" 模式在本地共享，这样任何本地加锁都将根据需要通过 i_mutex 完成，而不是通过 glock。)

Locking rules for glock operations:
(glock 操作的加锁规则：)

==============   ======================    =============================
Operation        GLF_LOCK bit lock held    gl_lockref.lock spinlock held
==============   ======================    =============================
go_sync               Yes                       No
go_xmote_bh           Yes                       No
go_inval              Yes                       No
go_instantiate        No                        No
go_held               No                        No
go_dump               Sometimes                 Yes
go_callback           Sometimes (N/A)           Yes
go_unlocked           Yes                       No
==============   ======================    =============================

.. Note::

   Operations must not drop either the bit lock or the spinlock
   if its held on entry. go_dump and do_demote_ok must never block.
   Note that go_dump will only be called if the glock's state
   indicates that it is caching up-to-date data.
   (进入时如果已持有位锁或自旋锁，操作就不得丢弃它们中的任何一个。go_dump 和 do_demote_ok 绝不能阻塞。注意，只有当 glock 的状态表明它正在缓存最新数据时，才会调用 go_dump。)

Glock locking order within GFS2:
(GFS2 内的 glock 加锁顺序：)

 1. i_rwsem (if required)
    (i_rwsem(如果需要))
 2. Rename glock (for rename only)
    (重命名 glock(仅用于重命名))
 3. Inode glock(s)
    (Parents before children, inodes at "same level" with same parent in
    lock number order)
    (inode glock(父项在子项之前，同一父项下处于"同一层级"的 inode 按锁号顺序排列))
 4. Rgrp glock(s) (for (de)allocation operations)
    (资源组 glock(用于(去)分配操作))
 5. Transaction glock (via gfs2_trans_begin) for non-read operations
    (事务 glock(通过 gfs2_trans_begin)，用于非读操作)
 6. i_rw_mutex (if required)
    (i_rw_mutex(如果需要))
 7. Page lock  (always last, very important!)
    (页锁(总是最后，非常重要！))

There are two glocks per inode. One deals with access to the inode
itself (locking order as above), and the other, known as the iopen
glock is used in conjunction with the i_nlink field in the inode to
determine the lifetime of the inode in question. Locking of inodes
is on a per-inode basis. Locking of rgrps is on a per rgrp basis.
In general we prefer to lock local locks prior to cluster locks.
(每个 inode 有两个 glock。一个处理对 inode 本身的访问(加锁顺序如上)，另一个被称为 iopen glock，与 inode 中的 i_nlink 字段配合使用，以确定相关 inode 的生命周期。inode 的加锁是按每个 inode 进行的。资源组的加锁是按每个资源组进行的。一般来说，我们倾向于在集群锁之前先加本地锁。)

Glock Statistics
(Glock 统计)
----------------

The stats are divided into two sets: those relating to the
super block and those relating to an individual glock. The
super block stats are done on a per cpu basis in order to
try and reduce the overhead of gathering them. They are also
further divided by glock type. All timings are in nanoseconds.
(统计分为两组：与超级块相关的和与单个 glock 相关的。超级块统计按每个 CPU 进行，以尽量减少收集它们的开销。它们还按 glock 类型进一步划分。所有计时都以纳秒为单位。)

In the case of both the super block and glock statistics,
the same information is gathered in each case. The super
block timing statistics are used to provide default values for
the glock timing statistics, so that newly created glocks
should have, as far as possible, a sensible starting point.
The per-glock counters are initialised to zero when the
glock is created. The per-glock statistics are lost when
the glock is ejected from memory.
(对于超级块统计和 glock 统计，两者收集的信息是相同的。超级块计时统计用于为 glock 计时统计提供默认值，这样新创建的 glock 应尽可能地有一个合理的起点。每个 glock 的计数器在 glock 创建时被初始化为零。当 glock 被逐出内存时，每个 glock 的统计信息会丢失。)

The statistics are divided into three pairs of mean and
variance, plus two counters. The mean/variance pairs are
smoothed exponential estimates and the algorithm used is
one which will be very familiar to those used to calculation
of round trip times in network code. See "TCP/IP Illustrated,
Volume 1", W. Richard Stevens, sect 21.3, "Round-Trip Time Measurement",
p. 299 and onwards. Also, Volume 2, Sect. 25.10, p. 838 and onwards.
Unlike the TCP/IP Illustrated case, the mean and variance are
not scaled, but are in units of integer nanoseconds.
(统计分为三对均值和方差，外加两个计数器。均值/方差对是平滑的指数估计，所使用的算法对于那些熟悉网络代码中往返时间计算的人来说会非常熟悉。参见 "TCP/IP Illustrated, Volume 1"，W. Richard Stevens，第 21.3 节 "Round-Trip Time Measurement"，第 299 页及后续。另外，第 2 卷第 25.10 节，第 838 页及后续。与 TCP/IP Illustrated 的情况不同，均值和方差不做缩放，而是以整数纳秒为单位。)

The three pairs of mean/variance measure the following
things:
(这三对均值/方差度量以下内容：)

 1. DLM lock time (non-blocking requests)
    (DLM 加锁时间(非阻塞请求))
 2. DLM lock time (blocking requests)
    (DLM 加锁时间(阻塞请求))
 3. Inter-request time (again to the DLM)
    (请求间时间(同样是对 DLM 而言))

A non-blocking request is one which will complete right
away, whatever the state of the DLM lock in question. That
currently means any requests when (a) the current state of
the lock is exclusive, i.e. a lock demotion (b) the requested
state is either null or unlocked (again, a demotion) or (c) the
"try lock" flag is set. A blocking request covers all the other
lock requests.
(非阻塞请求是无论相关 DLM 锁处于何种状态都会立即完成的请求。目前这意味着以下情况下的任何请求：(a) 锁的当前状态是独占，即锁降级；(b) 请求的状态是 null 或未加锁(同样，是降级)；或 (c) 设置了 "try lock" 标志。阻塞请求涵盖所有其他锁请求。)

There are two counters. The first is there primarily to show
how many lock requests have been made, and thus how much data
has gone into the mean/variance calculations. The other counter
is counting queuing of holders at the top layer of the glock
code. Hopefully that number will be a lot larger than the number
of dlm lock requests issued.
(有两个计数器。第一个主要是为了显示已经发出了多少锁请求，从而显示有多少数据进入了均值/方差计算。另一个计数器统计 glock 代码顶层持有的排队情况。希望该数字会远大于发出的 dlm 锁请求数量。)

So why gather these statistics? There are several reasons
we'd like to get a better idea of these timings:
(那么为什么要收集这些统计信息呢？我们希望更好地了解这些计时，有以下几个原因：)

1. To be able to better set the glock "min hold time"
   (为了能够更好地设置 glock 的"最小持有时间")
2. To spot performance issues more easily
   (为了更容易发现性能问题)
3. To improve the algorithm for selecting resource groups for
   allocation (to base it on lock wait time, rather than blindly
   using a "try lock")
   (为了改进选择资源组进行分配的算法(将其基于锁等待时间，而不是盲目地使用 "try lock"))

Due to the smoothing action of the updates, a step change in
some input quantity being sampled will only fully be taken
into account after 8 samples (or 4 for the variance) and this
needs to be carefully considered when interpreting the
results.
(由于更新的平滑作用，被采样的某个输入量的阶跃变化只有在 8 个采样之后(方差为 4 个)才会被完全计入，在解释结果时需要仔细考虑这一点。)

Knowing both the time it takes a lock request to complete and
the average time between lock requests for a glock means we
can compute the total percentage of the time for which the
node is able to use a glock vs. time that the rest of the
cluster has its share. That will be very useful when setting
the lock min hold time.
(既知道锁请求完成所需的时间，又知道一个 glock 的锁请求之间的平均间隔，意味着我们可以计算出节点能够使用某个 glock 的时间占总时间的百分比，以及集群其余部分占有其份额的时间。这在设置锁的最小持有时间时非常有用。)

Great care has been taken to ensure that we
measure exactly the quantities that we want, as accurately
as possible. There are always inaccuracies in any
measuring system, but I hope this is as accurate as we
can reasonably make it.
(我们已经非常小心地确保精确地、尽可能准确地测量我们想要的量。任何测量系统总是存在不精确之处，但我希望这已经达到了我们合理能做到的准确程度。)

Per sb stats can be found here::
(每个 sb 的统计可在这里找到：)

    /sys/kernel/debug/gfs2/<fsname>/sbstats

Per glock stats can be found here::
(每个 glock 的统计可在这里找到：)

    /sys/kernel/debug/gfs2/<fsname>/glstats

Assuming that debugfs is mounted on /sys/kernel/debug and also
that <fsname> is replaced with the name of the gfs2 filesystem
in question.
(前提是 debugfs 已挂载到 /sys/kernel/debug 上，并且 <fsname> 已被替换为相关 gfs2 文件系统的名称。)

The abbreviations used in the output as are follows:
(输出中使用的缩写如下：)

=========  ================================================================
srtt       Smoothed round trip time for non blocking dlm requests (非阻塞 dlm 请求的平滑往返时间)
srttvar    Variance estimate for srtt (srtt 的方差估计)
srttb      Smoothed round trip time for (potentially) blocking dlm requests ((可能)阻塞的 dlm 请求的平滑往返时间)
srttvarb   Variance estimate for srttb (srttb 的方差估计)
sirt       Smoothed inter request time (for dlm requests) (平滑的请求间时间(针对 dlm 请求))
sirtvar    Variance estimate for sirt (sirt 的方差估计)
dlm        Number of dlm requests made (dcnt in glstats file) (发出的 dlm 请求数量(glstats 文件中的 dcnt))
queue      Number of glock requests queued (qcnt in glstats file) (排队的 glock 请求数量(glstats 文件中的 qcnt))
=========  ================================================================

The sbstats file contains a set of these stats for each glock type (so 8 lines
for each type) and for each cpu (one column per cpu). The glstats file contains
a set of these stats for each glock in a similar format to the glocks file, but
using the format mean/variance for each of the timing stats.
(sbstats 文件针对每种 glock 类型(即每种类型 8 行)以及每个 CPU(每个 CPU 一列)包含一组这些统计。glstats 文件针对每个 glock 包含一组这些统计，格式与 glocks 文件类似，但每个计时统计使用 均值/方差 的格式。)

The gfs2_glock_lock_time tracepoint prints out the current values of the stats
for the glock in question, along with some addition information on each dlm
reply that is received:
(gfs2_glock_lock_time 跟踪点打印出相关 glock 的统计信息的当前值，以及收到的每个 dlm 应答的一些附加信息：)

======   =======================================
status   The status of the dlm request (dlm 请求的状态)
flags    The dlm request flags (dlm 请求标志)
tdiff    The time taken by this specific request (此特定请求所花费的时间)
======   =======================================

(remaining fields as per above list)
((其余字段参见上面的列表))


