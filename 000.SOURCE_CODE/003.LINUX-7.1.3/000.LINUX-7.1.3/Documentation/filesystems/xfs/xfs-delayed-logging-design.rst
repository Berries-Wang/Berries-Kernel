.. SPDX-License-Identifier: GPL-2.0

==================
XFS Logging Design
==================

(XFS 日志设计)

Preamble
(前言)
========

This document describes the design and algorithms that the XFS journalling
subsystem is based on. This document describes the design and algorithms that
the XFS journalling subsystem is based on so that readers may familiarize
themselves with the general concepts of how transaction processing in XFS works.
(本文档描述 XFS 日志子系统所基于的设计和算法。本文档描述 XFS 日志子系统所基于的设计和算法，以便读者熟悉 XFS 中事务处理工作方式的一般概念。)

We begin with an overview of transactions in XFS, followed by describing how
transaction reservations are structured and accounted, and then move into how we
guarantee forwards progress for long running transactions with finite initial
reservations bounds. At this point we need to explain how relogging works. With
the basic concepts covered, the design of the delayed logging mechanism is
documented.
(我们从 XFS 中事务的概述开始，接着描述事务预留是如何构造和记账的，然后进入我们如何用有限的初始预留上限为长时间运行的事务保证向前推进。在这一点上，我们需要解释重新日志记录(re-logging)是如何工作的。在介绍完这些基本概念之后，将记录延迟日志(delayed logging)机制的设计。)


Introduction
(引言)
============

XFS uses Write Ahead Logging for ensuring changes to the filesystem metadata
are atomic and recoverable. For reasons of space and time efficiency, the
logging mechanisms are varied and complex, combining intents, logical and
physical logging mechanisms to provide the necessary recovery guarantees the
filesystem requires.
(XFS 使用预写日志(Write Ahead Logging)来确保对文件系统元数据的更改是原子的且可恢复的。出于空间和时间效率的考虑，日志机制多种多样且复杂，组合了意图(intent)、逻辑和物理日志机制，以提供文件系统所需的必要恢复保证。)

Some objects, such as inodes and dquots, are logged in logical format where the
details logged are made up of the changes to in-core structures rather than
on-disk structures. Other objects - typically buffers - have their physical
changes logged. Long running atomic modifications have individual changes
chained together by intents, ensuring that journal recovery can restart and
finish an operation that was only partially done when the system stopped
functioning.
(一些对象，例如 inode 和 dquot，以逻辑格式进行日志记录，其中记录的细节由内存中(in-core)结构的更改而非磁盘上结构的更改构成。其他对象——通常是缓冲——则记录其物理更改。长时间运行的原子修改通过 intent 将各个单独的更改链接在一起，从而确保日志恢复能够重新启动并完成在系统停止正常工作时仅部分完成的操作。)

The reason for these differences is to keep the amount of log space and CPU time
required to process objects being modified as small as possible and hence the
logging overhead as low as possible. Some items are very frequently modified,
and some parts of objects are more frequently modified than others, so keeping
the overhead of metadata logging low is of prime importance.
(这些差异的原因是使处理被修改对象所需的日志空间和 CPU 时间尽可能少，从而使日志开销尽可能低。有些项(item)被非常频繁地修改，并且对象的某些部分比其他部分修改得更频繁，因此保持元数据日志记录的低开销至关重要。)

The method used to log an item or chain modifications together isn't
particularly important in the scope of this document. It suffices to know that
the method used for logging a particular object or chaining modifications
together are different and are dependent on the object and/or modification being
performed. The logging subsystem only cares that certain specific rules are
followed to guarantee forwards progress and prevent deadlocks.
(用于记录某个项的日志或将修改链接在一起的方法在本文档的范围内并不特别重要。只需知道：记录特定对象日志或链接修改所用的方法各不相同，并且取决于正在执行的对象和/或修改。日志子系统只关心是否遵循某些特定的规则以保证向前推进并防止死锁。)


Transactions in XFS
(XFS 中的事务)
===================

XFS has two types of high level transactions, defined by the type of log space
reservation they take. These are known as "one shot" and "permanent"
transactions. Permanent transaction reservations can take reservations that span
commit boundaries, whilst "one shot" transactions are for a single atomic
modification.
(XFS 有两种类型的高层事务，由它们所采用的日志空间预留的类型定义。它们被称为"一次性(one shot)"和"永久(permanent)"事务。永久事务预留可以采用跨越提交边界的预留，而"一次性"事务用于单个原子修改。)

The type and size of reservation must be matched to the modification taking
place.  This means that permanent transactions can be used for one-shot
modifications, but one-shot reservations cannot be used for permanent
transactions.
(预留的类型和大小必须与正在进行的修改相匹配。这意味着永久事务可以用于一次性修改，但一次性预留不能用于永久事务。)

In the code, a one-shot transaction pattern looks somewhat like this::
(在代码中，一次性事务的模式大致如下所示：)

	tp = xfs_trans_alloc(<reservation>)
	<lock items>
	<join item to transaction>
	<do modification>
	xfs_trans_commit(tp);

As items are modified in the transaction, the dirty regions in those items are
tracked via the transaction handle.  Once the transaction is committed, all
resources joined to it are released, along with the remaining unused reservation
space that was taken at the transaction allocation time.
(当项在事务中被修改时，这些项中的脏区域通过事务句柄进行跟踪。一旦事务被提交，所有连接到该事务的资源都会被释放，连同在事务分配时取得的剩余未使用的预留空间。)

In contrast, a permanent transaction is made up of multiple linked individual
transactions, and the pattern looks like this::
(相比之下，永久事务由多个相互链接的单独事务组成，其模式如下所示：)

	tp = xfs_trans_alloc(<reservation>)
	xfs_ilock(ip, XFS_ILOCK_EXCL)

	loop {
		xfs_trans_ijoin(tp, 0);
		<do modification>
		xfs_trans_log_inode(tp, ip);
		xfs_trans_roll(&tp);
	}

	xfs_trans_commit(tp);
	xfs_iunlock(ip, XFS_ILOCK_EXCL);

While this might look similar to a one-shot transaction, there is an important
difference: xfs_trans_roll() performs a specific operation that links two
transactions together::
(虽然这看起来可能与一次性事务相似，但有一个重要区别：xfs_trans_roll() 执行一种将两个事务链接在一起的特定操作：)

	ntp = xfs_trans_dup(tp);
	xfs_trans_commit(tp);
	xfs_trans_reserve(ntp);

This results in a series of "rolling transactions" where the inode is locked
across the entire chain of transactions.  Hence while this series of rolling
transactions is running, nothing else can read from or write to the inode and
this provides a mechanism for complex changes to appear atomic from an external
observer's point of view.
(这会产生一系列"滚动事务(rolling transactions)"，其中 inode 在整个事务链上保持锁定。因此，在这一系列滚动事务运行期间，没有其他任何东西可以读取或写入该 inode，这为复杂更改从外部观察者的角度看起来是原子的提供了一种机制。)

It is important to note that a series of rolling transactions in a permanent
transaction does not form an atomic change in the journal. While each
individual modification is atomic, the chain is *not atomic*. If we crash half
way through, then recovery will only replay up to the last transactional
modification the loop made that was committed to the journal.
(需要注意的是，永久事务中的一系列滚动事务并不在日志中形成一个原子更改。虽然每个单独的修改是原子的，但整个链 *不是原子的*。如果我们在中途崩溃，那么恢复将只重放到该循环所做的、已提交到日志的最后一个事务修改为止。)

This affects long running permanent transactions in that it is not possible to
predict how much of a long running operation will actually be recovered because
there is no guarantee of how much of the operation reached stale storage. Hence
if a long running operation requires multiple transactions to fully complete,
the high level operation must use intents and deferred operations to guarantee
recovery can complete the operation once the first transactions is persisted in
the on-disk journal.
(这会影响长时间运行的永久事务：由于无法保证操作中有多少到达了稳定存储，因此无法预测长时间运行的操作实际会有多少被恢复。因此，如果一个长时间运行的操作需要多个事务才能完全完成，那么高层操作必须使用 intent 和延迟操作(deferred operations)来保证在第一个事务持久化到磁盘上的日志之后，恢复能够完成该操作。)


Transactions are Asynchronous
(事务是异步的)
=============================

In XFS, all high level transactions are asynchronous by default. This means that
xfs_trans_commit() does not guarantee that the modification has been committed
to stable storage when it returns. Hence when a system crashes, not all the
completed transactions will be replayed during recovery.
(在 XFS 中，所有高层事务默认都是异步的。这意味着 xfs_trans_commit() 在返回时并不保证修改已经提交到稳定存储。因此当系统崩溃时，并非所有已完成的事务都会在恢复期间被重放。)

However, the logging subsystem does provide global ordering guarantees, such
that if a specific change is seen after recovery, all metadata modifications
that were committed prior to that change will also be seen.
(不过，日志子系统确实提供全局顺序保证，即如果在恢复后看到某个特定更改，那么在该更改之前提交的所有元数据修改也都会被看到。)

For single shot operations that need to reach stable storage immediately, or
ensuring that a long running permanent transaction is fully committed once it is
complete, we can explicitly tag a transaction as synchronous. This will trigger
a "log force" to flush the outstanding committed transactions to stable storage
in the journal and wait for that to complete.
(对于需要立即到达稳定存储的单次操作，或者需要确保长时间运行的永久事务在完成后被完整提交的情况，我们可以显式地将事务标记为同步。这将触发"日志强制(log force)"，把未决的已提交事务刷新到日志中的稳定存储并等待其完成。)

Synchronous transactions are rarely used, however, because they limit logging
throughput to the IO latency limitations of the underlying storage. Instead, we
tend to use log forces to ensure modifications are on stable storage only when
a user operation requires a synchronisation point to occur (e.g. fsync).
(然而，同步事务很少使用，因为它们会将日志吞吐量限制在底层存储的 IO 延迟水平。相反，我们倾向于只在用户操作需要出现同步点时(例如 fsync)才使用日志强制来确保修改位于稳定存储上。)


Transaction Reservations
(事务预留)
========================

It has been mentioned a number of times now that the logging subsystem needs to
provide a forwards progress guarantee so that no modification ever stalls
because it can't be written to the journal due to a lack of space in the
journal. This is achieved by the transaction reservations that are made when
a transaction is first allocated. For permanent transactions, these reservations
are maintained as part of the transaction rolling mechanism.
(前面已经多次提到，日志子系统需要提供向前推进的保证，使得任何修改都不会因为日志中没有足够的空间可供写入而停滞。这是通过在事务首次分配时进行的事务预留来实现的。对于永久事务，这些预留作为事务滚动机制的一部分被维持。)

A transaction reservation provides a guarantee that there is physical log space
available to write the modification into the journal before we start making
modifications to objects and items. As such, the reservation needs to be large
enough to take into account the amount of metadata that the change might need to
log in the worst case. This means that if we are modifying a btree in the
transaction, we have to reserve enough space to record a full leaf-to-root split
of the btree. As such, the reservations are quite complex because we have to
take into account all the hidden changes that might occur.
(事务预留提供了一种保证：在我们开始对对象和项进行修改之前，有物理日志空间可用于将修改写入日志。因此，预留需要足够大，以考虑到该更改在最坏情况下可能需要记录的元数据量。这意味着如果我们在事务中修改一个 btree，我们必须预留足够的空间来记录 btree 的完整叶子到根的分裂。因此，预留相当复杂，因为我们必须考虑到所有可能发生的隐藏更改。)

For example, a user data extent allocation involves allocating an extent from
free space, which modifies the free space trees. That's two btrees.  Inserting
the extent into the inode's extent map might require a split of the extent map
btree, which requires another allocation that can modify the free space trees
again.  Then we might have to update reverse mappings, which modifies yet
another btree which might require more space. And so on.  Hence the amount of
metadata that a "simple" operation can modify can be quite large.
(例如，一个用户数据 extent 分配涉及从空闲空间中分配一个 extent，这会修改空闲空间树。那是两个 btree。将 extent 插入 inode 的 extent 映射可能需要分裂 extent 映射 btree，这需要另一次分配，而该分配又可能再次修改空闲空间树。然后我们可能还必须更新反向映射，这又会修改另一个 btree，从而可能需要更多空间。依此类推。因此，一个"简单"操作可以修改的元数据量可能相当大。)

This "worst case" calculation provides us with the static "unit reservation"
for the transaction that is calculated at mount time. We must guarantee that the
log has this much space available before the transaction is allowed to proceed
so that when we come to write the dirty metadata into the log we don't run out
of log space half way through the write.
(这种"最坏情况"计算为我们提供了在挂载时计算的事务的静态"单元预留(unit reservation)"。在允许事务继续进行之前，我们必须保证日志有这么多的可用空间，这样当我们把脏元数据写入日志时，才不会在写入中途耗尽日志空间。)

For one-shot transactions, a single unit space reservation is all that is
required for the transaction to proceed. For permanent transactions, however, we
also have a "log count" that affects the size of the reservation that is to be
made.
(对于一次性事务，单个单元空间预留就足以让事务继续进行。而对于永久事务，我们还有一个影响预留大小的"日志计数(log count)"。)

While a permanent transaction can get by with a single unit of space
reservation, it is somewhat inefficient to do this as it requires the
transaction rolling mechanism to re-reserve space on every transaction roll. We
know from the implementation of the permanent transactions how many transaction
rolls are likely for the common modifications that need to be made.
(虽然永久事务可以只用单个单元的空间预留应付，但这样做效率有些低下，因为它要求事务滚动机制在每次事务滚动时重新预留空间。从永久事务的实现中，我们可以知道对于需要进行的常见修改，事务滚动可能发生多少次。)

For example, an inode allocation is typically two transactions - one to
physically allocate a free inode chunk on disk, and another to allocate an inode
from an inode chunk that has free inodes in it.  Hence for an inode allocation
transaction, we might set the reservation log count to a value of 2 to indicate
that the common/fast path transaction will commit two linked transactions in a
chain. Each time a permanent transaction rolls, it consumes an entire unit
reservation.
(例如，一个 inode 分配通常是两个事务——一个用于在磁盘上物理分配一个空闲 inode 块(chunk)，另一个用于从还有空闲 inode 的 inode 块中分配一个 inode。因此，对于 inode 分配事务，我们可以将预留日志计数设置为 2，以表示常见/快速路径事务将在一条链中提交两个相互链接的事务。永久事务每次滚动时，都会消耗一个完整的单元预留。)

Hence when the permanent transaction is first allocated, the log space
reservation is increased from a single unit reservation to multiple unit
reservations. That multiple is defined by the reservation log count, and this
means we can roll the transaction multiple times before we have to re-reserve
log space when we roll the transaction. This ensures that the common
modifications we make only need to reserve log space once.
(因此，当永久事务首次分配时，日志空间预留从单个单元预留增加到多个单元预留。该倍数由预留日志计数定义，这意味着在必须重新预留日志空间之前，我们可以在滚动事务时多次滚动事务。这确保了我们进行的常见修改只需要预留一次日志空间。)

If the log count for a permanent transaction reaches zero, then it needs to
re-reserve physical space in the log. This is somewhat complex, and requires
an understanding of how the log accounts for space that has been reserved.
(如果永久事务的日志计数达到零，那么它需要重新预留日志中的物理空间。这有些复杂，需要理解日志如何为已预留的空间记账。)


Log Space Accounting
(日志空间记账)
====================

The position in the log is typically referred to as a Log Sequence Number (LSN).
The log is circular, so the positions in the log are defined by the combination
of a cycle number - the number of times the log has been overwritten - and the
offset into the log.  A LSN carries the cycle in the upper 32 bits and the
offset in the lower 32 bits. The offset is in units of "basic blocks" (512
bytes). Hence we can do relatively simple LSN based math to keep track of
available space in the log.
(日志中的位置通常被称为日志序列号(Log Sequence Number, LSN)。日志是循环的，因此日志中的位置由循环号(cycle number)——日志被覆写的次数——与日志内偏移量的组合定义。LSN 在高 32 位中携带循环号，在低 32 位中携带偏移量。偏移量以"基本块"(512 字节)为单位。因此，我们可以进行相对简单的基于 LSN 的数学运算来跟踪日志中的可用空间。)

Log space accounting is done via a pair of constructs called "grant heads".  The
position of the grant heads is an absolute value, so the amount of space
available in the log is defined by the distance between the position of the
grant head and the current log tail. That is, how much space can be
reserved/consumed before the grant heads would fully wrap the log and overtake
the tail position.
(日志空间记账通过一对称为"授权头(grant heads)"的结构完成。授权头的位置是一个绝对值，因此日志中的可用空间由授权头的位置与当前日志尾之间的距离定义。也就是说，在授权头完全绕回日志并越过尾位置之前，可以预留/消耗多少空间。)

The first grant head is the "reserve" head. This tracks the byte count of the
reservations currently held by active transactions. It is a purely in-memory
accounting of the space reservation and, as such, actually tracks byte offsets
into the log rather than basic blocks. Hence it technically isn't using LSNs to
represent the log position, but it is still treated like a split {cycle,offset}
tuple for the purposes of tracking reservation space.
(第一个授权头是"预留(reserve)"头。它跟踪当前由活跃事务持有的预留的字节计数。它纯粹是对空间预留的内存中记账，因此实际跟踪的是日志内的字节偏移量而不是基本块。因此，严格来说它并没有使用 LSN 来表示日志位置，但为了跟踪预留空间的目的，它仍然被当作一个分割的 {cycle,offset} 元组来对待。)

The reserve grant head is used to accurately account for exact transaction
reservations amounts and the exact byte count that modifications actually make
and need to write into the log. The reserve head is used to prevent new
transactions from taking new reservations when the head reaches the current
tail. It will block new reservations in a FIFO queue and as the log tail moves
forward it will wake them in order once sufficient space is available. This FIFO
mechanism ensures no transaction is starved of resources when log space
shortages occur.
(预留授权头用于准确记账确切的事务预留量以及修改实际产生并需要写入日志的确切字节数。预留头用于在头到达当前尾时阻止新事务取得新的预留。它会将新的预留阻塞在一个 FIFO 队列中，并且随着日志尾向前移动，一旦有足够的空间可用，就会按顺序唤醒它们。这种 FIFO 机制确保在日志空间短缺时没有任何事务被饿死。)

The other grant head is the "write" head. Unlike the reserve head, this grant
head contains an LSN and it tracks the physical space usage in the log. While
this might sound like it is accounting the same state as the reserve grant head
- and it mostly does track exactly the same location as the reserve grant head -
there are critical differences in behaviour between them that provides the
forwards progress guarantees that rolling permanent transactions require.
(另一个授权头是"写(write)"头。与预留头不同，这个授权头包含一个 LSN，它跟踪日志中的物理空间使用情况。虽然这听起来像是在记账与预留授权头相同的状态——而且它大多数时候确实跟踪与预留授权头完全相同的位置——但两者之间的行为差异提供了滚动永久事务所需的向前推进保证。)

These differences when a permanent transaction is rolled and the internal "log
count" reaches zero and the initial set of unit reservations have been
exhausted. At this point, we still require a log space reservation to continue
the next transaction in the sequeunce, but we have none remaining. We cannot
sleep during the transaction commit process waiting for new log space to become
available, as we may end up on the end of the FIFO queue and the items we have
locked while we sleep could end up pinning the tail of the log before there is
enough free space in the log to fulfill all of the pending reservations and
then wake up transaction commit in progress.
(这些差异出现在永久事务滚动、内部"日志计数"达到零且初始的单元预留已被耗尽之时。此时，我们仍然需要一个日志空间预留来继续链条中的下一个事务，但我们已经没有任何剩余。我们不能在事务提交过程中睡眠等待新的日志空间变为可用，因为我们可能最终排在 FIFO 队列的末尾，而我们在睡眠期间锁定的项，可能会在日志中出现足够的空闲空间来满足所有未决的预留、进而唤醒正在进行的事务提交之前，就把日志尾钉住(pin)。)

To take a new reservation without sleeping requires us to be able to take a
reservation even if there is no reservation space currently available. That is,
we need to be able to *overcommit* the log reservation space. As has already
been detailed, we cannot overcommit physical log space. However, the reserve
grant head does not track physical space - it only accounts for the amount of
reservations we currently have outstanding. Hence if the reserve head passes
over the tail of the log all it means is that new reservations will be throttled
immediately and remain throttled until the log tail is moved forward far enough
to remove the overcommit and start taking new reservations. In other words, we
can overcommit the reserve head without violating the physical log head and tail
rules.
(要在不睡眠的情况下取得新预留，要求我们即使当前没有可用的预留空间也能取得预留。也就是说，我们需要能够 *超额提交(overcommit)* 日志预留空间。如前文所述，我们不能超额提交物理日志空间。然而，预留授权头并不跟踪物理空间——它只记账我们当前未决的预留量。因此，如果预留头越过日志尾，那只是意味着新的预留会立即被限流，并保持限流状态，直到日志尾向前移动足够远以消除超额提交并开始接受新的预留。换句话说，我们可以在不违反物理日志头尾规则的情况下超额提交预留头。)

As a result, permanent transactions only "regrant" reservation space during
xfs_trans_commit() calls, while the physical log space reservation - tracked by
the write head - is then reserved separately by a call to xfs_log_reserve()
after the commit completes. Once the commit completes, we can sleep waiting for
physical log space to be reserved from the write grant head, but only if one
critical rule has been observed::
(因此，永久事务只在 xfs_trans_commit() 调用期间"重新授予(regrant)"预留空间，而物理日志空间预留——由写头跟踪——则在提交完成之后通过调用 xfs_log_reserve() 单独预留。一旦提交完成，我们就可以睡眠等待从写授权头预留物理日志空间，但前提是必须遵守以下一条关键规则：)

	Code using permanent reservations must always log the items they hold
	locked across each transaction they roll in the chain.

"Re-logging" the locked items on every transaction roll ensures that the items
attached to the transaction chain being rolled are always relocated to the
physical head of the log and so do not pin the tail of the log. If a locked item
pins the tail of the log when we sleep on the write reservation, then we will
deadlock the log as we cannot take the locks needed to write back that item and
move the tail of the log forwards to free up write grant space. Re-logging the
locked items avoids this deadlock and guarantees that the log reservation we are
making cannot self-deadlock.
(在每次事务滚动时"重新记录日志(re-log)"被锁定的项，可以确保附着到正在滚动的事务链上的项总是被重新定位到日志的物理头，因此不会钉住日志尾。如果当我们等待写预留而睡眠时，一个被锁定的项钉住了日志尾，那么我们将使日志死锁，因为我们无法取得回写该项所需的锁，也就无法将日志尾向前移动以释放写授权空间。重新记录被锁定项的日志避免了这种死锁，并保证我们正在进行的日志预留不会自我死锁。)

If all rolling transactions obey this rule, then they can all make forwards
progress independently because nothing will block the progress of the log
tail moving forwards and hence ensuring that write grant space is always
(eventually) made available to permanent transactions no matter how many times
they roll.
(如果所有滚动事务都遵守这条规则，那么它们都可以独立地向前推进，因为没有什么会阻碍日志尾向前移动的进程，从而确保无论永久事务滚动多少次，写授权空间总是(最终)对它们可用。)


Re-logging Explained
(重新日志记录详解)
====================

XFS allows multiple separate modifications to a single object to be carried in
the log at any given time.  This allows the log to avoid needing to flush each
change to disk before recording a new change to the object. XFS does this via a
method called "re-logging". Conceptually, this is quite simple - all it requires
is that any new change to the object is recorded with a *new copy* of all the
existing changes in the new transaction that is written to the log.
(XFS 允许在任意给定时刻在日志中携带对单个对象的多个相互独立的修改。这使日志无需在记录对象的新更改之前先将每个更改刷新到磁盘。XFS 通过一种称为"重新日志记录(re-logging)"的方法做到这一点。从概念上讲，这相当简单——它只要求对对象的任何新更改都与所有现有更改的 *新副本* 一起记录在写入日志的新事务中。)

That is, if we have a sequence of changes A through to F, and the object was
written to disk after change D, we would see in the log the following series
of transactions, their contents and the log sequence number (LSN) of the
transaction::
(也就是说，如果我们有一个从 A 到 F 的更改序列，并且该对象在更改 D 之后被写入了磁盘，我们就会在日志中看到下面这一系列事务、它们的内容以及事务的日志序列号(LSN)：)

	Transaction		Contents	LSN
	   A			   A		   X
	   B			  A+B		  X+n
	   C			 A+B+C		 X+n+m
	   D			A+B+C+D		X+n+m+o
	    <object written to disk>
	   E			   E		   Y (> X+n+m+o)
	   F			  E+F		  Y+p

In other words, each time an object is relogged, the new transaction contains
the aggregation of all the previous changes currently held only in the log.
(换句话说，每当一个对象被重新记录日志时，新事务都包含当前仅保存在日志中的所有先前更改的聚合。)

This relogging technique allows objects to be moved forward in the log so that
an object being relogged does not prevent the tail of the log from ever moving
forward.  This can be seen in the table above by the changing (increasing) LSN
of each subsequent transaction, and it's the technique that allows us to
implement long-running, multiple-commit permanent transactions. 
(这种重新日志记录技术使对象能够在日志中向前移动，从而被重新记录日志的对象不会阻止日志尾向前移动。在上面表格中可以通过每个后续事务不断变化(增加)的 LSN 看到这一点，这正是使我们能够实现长时间运行、多次提交的永久事务的技术。)

A typical example of a rolling transaction is the removal of extents from an
inode which can only be done at a rate of two extents per transaction because
of reservation size limitations. Hence a rolling extent removal transaction
keeps relogging the inode and btree buffers as they get modified in each
removal operation. This keeps them moving forward in the log as the operation
progresses, ensuring that current operation never gets blocked by itself if the
log wraps around.
(滚动事务的一个典型例子是从 inode 中移除 extent，由于预留大小的限制，每个事务只能以两个 extent 的速率进行移除。因此，滚动 extent 移除事务在每次移除操作中不断为被修改的 inode 和 btree 缓冲重新记录日志。这使它们随着操作的进行在日志中不断向前移动，确保当前操作在日志回绕时永远不会被自己阻塞。)

Hence it can be seen that the relogging operation is fundamental to the correct
working of the XFS journalling subsystem. From the above description, most
people should be able to see why the XFS metadata operations writes so much to
the log - repeated operations to the same objects write the same changes to
the log over and over again. Worse is the fact that objects tend to get
dirtier as they get relogged, so each subsequent transaction is writing more
metadata into the log.
(由此可见，重新日志记录操作对于 XFS 日志子系统的正确工作至关重要。从上面的描述中，大多数人应该能够看出为什么 XFS 元数据操作向日志写入如此多的内容——对相同对象的重复操作把相同的更改一遍又一遍地写入日志。更糟的是，对象在重新记录日志时往往会变得更脏，因此每个后续事务都会向日志写入更多的元数据。)

It should now also be obvious how relogging and asynchronous transactions go
hand in hand. That is, transactions don't get written to the physical journal
until either a log buffer is filled (a log buffer can hold multiple
transactions) or a synchronous operation forces the log buffers holding the
transactions to disk. This means that XFS is doing aggregation of transactions
in memory - batching them, if you like - to minimise the impact of the log IO on
transaction throughput.
(现在也应该清楚重新日志记录与异步事务是如何相辅相成的。也就是说，事务在被写入物理日志之前，要么是日志缓冲被填满(一个日志缓冲可以容纳多个事务)，要么是某个同步操作强制将持有这些事务的日志缓冲写入磁盘。这意味着 XFS 在内存中对事务进行聚合——如果你愿意，也可以称之为批量处理——以最小化日志 IO 对事务吞吐量的影响。)

The limitation on asynchronous transaction throughput is the number and size of
log buffers made available by the log manager. By default there are 8 log
buffers available and the size of each is 32kB - the size can be increased up
to 256kB by use of a mount option.
(异步事务吞吐量的限制因素是日志管理器提供的日志缓冲的数量和大小。默认情况下有 8 个日志缓冲可用，每个的大小为 32kB——通过挂载选项可以将大小增加到 256kB。)

Effectively, this gives us the maximum bound of outstanding metadata changes
that can be made to the filesystem at any point in time - if all the log
buffers are full and under IO, then no more transactions can be committed until
the current batch completes. It is now common for a single current CPU core to
be to able to issue enough transactions to keep the log buffers full and under
IO permanently. Hence the XFS journalling subsystem can be considered to be IO
bound.
(实际上，这为我们给出了在任意时间点可以对文件系统做出的未决元数据更改的最大上限——如果所有日志缓冲都已满且正处于 IO 之中，那么在当前这一批完成之前，不能再提交更多的事务。如今，单个当前 CPU 核心能够发出足够多的事务、使日志缓冲永久保持满且处于 IO 之中，已属常见。因此，XFS 日志子系统可以被认为是 IO 受限的。)

Delayed Logging: Concepts
(延迟日志：概念)
=========================

The key thing to note about the asynchronous logging combined with the
relogging technique XFS uses is that we can be relogging changed objects
multiple times before they are committed to disk in the log buffers. If we
return to the previous relogging example, it is entirely possible that
transactions A through D are committed to disk in the same log buffer.
(关于异步日志记录与 XFS 使用的重新日志记录技术相结合，需要注意的关键一点是：在这些被更改的对象经由日志缓冲提交到磁盘之前，我们可能会对它们进行多次重新日志记录。如果我们回到前面的重新日志记录示例，事务 A 到 D 完全有可能在同一个日志缓冲中被提交到磁盘。)

That is, a single log buffer may contain multiple copies of the same object,
but only one of those copies needs to be there - the last one "D", as it
contains all the changes from the previous changes. In other words, we have one
necessary copy in the log buffer, and three stale copies that are simply
wasting space. When we are doing repeated operations on the same set of
objects, these "stale objects" can be over 90% of the space used in the log
buffers. It is clear that reducing the number of stale objects written to the
log would greatly reduce the amount of metadata we write to the log, and this
is the fundamental goal of delayed logging.
(也就是说，单个日志缓冲可能包含同一对象的多个副本，但其中只有一个副本是必需的——最后一个"D"，因为它包含了之前所有更改的内容。换句话说，日志缓冲中有一个必要的副本，以及三个只是在浪费空间的过时副本。当我们对同一组对象进行重复操作时，这些"过时对象"可能占日志缓冲所用空间的 90% 以上。显然，减少写入日志的过时对象数量将大大减少我们写入日志的元数据量，而这正是延迟日志的根本目标。)

From a conceptual point of view, XFS is already doing relogging in memory (where
memory == log buffer), only it is doing it extremely inefficiently. It is using
logical to physical formatting to do the relogging because there is no
infrastructure to keep track of logical changes in memory prior to physically
formatting the changes in a transaction to the log buffer. Hence we cannot avoid
accumulating stale objects in the log buffers.
(从概念的角度来看，XFS 已经在内存中(即内存 == 日志缓冲)进行重新日志记录，只是做得极其低效。它使用从逻辑到物理的格式化来进行重新日志记录，因为在将事务中的更改物理格式化到日志缓冲之前，没有任何基础设施在内存中跟踪逻辑更改。因此，我们无法避免在日志缓冲中积累过时对象。)

Delayed logging is the name we've given to keeping and tracking transactional
changes to objects in memory outside the log buffer infrastructure. Because of
the relogging concept fundamental to the XFS journalling subsystem, this is
actually relatively easy to do - all the changes to logged items are already
tracked in the current infrastructure. The big problem is how to accumulate
them and get them to the log in a consistent, recoverable manner.
Describing the problems and how they have been solved is the focus of this
document.
(延迟日志是我们为在内存中、日志缓冲基础设施之外保存和跟踪对对象的事务性更改所起的名字。由于重新日志记录是 XFS 日志子系统的基本概念，这实际上相对容易做到——所有对被记录项的更改都已经在现有基础设施中被跟踪。最大的问题是如何累积它们，并以一致的、可恢复的方式将它们送到日志中。描述这些问题以及它们是如何被解决的，是本文档的重点。)

One of the key changes that delayed logging makes to the operation of the
journalling subsystem is that it disassociates the amount of outstanding
metadata changes from the size and number of log buffers available. In other
words, instead of there only being a maximum of 2MB of transaction changes not
written to the log at any point in time, there may be a much greater amount
being accumulated in memory. Hence the potential for loss of metadata on a
crash is much greater than for the existing logging mechanism.
(延迟日志对日志子系统操作的关键改变之一是，它将未决元数据更改的数量与可用的日志缓冲的大小和数量解耦。换句话说，不再是在任意时间点最多只有 2MB 的事务更改未写入日志，而是可能在内存中积累大得多的量。因此，崩溃时元数据丢失的可能性比现有的日志机制要大得多。)

It should be noted that this does not change the guarantee that log recovery
will result in a consistent filesystem. What it does mean is that as far as the
recovered filesystem is concerned, there may be many thousands of transactions
that simply did not occur as a result of the crash. This makes it even more
important that applications that care about their data use fsync() where they
need to ensure application level data integrity is maintained.
(应当指出，这并不会改变日志恢复将得到一致文件系统的保证。它意味着的是，就恢复后的文件系统而言，可能有数千个事务因崩溃而根本没有发生。这使得关心自己数据的应用程序在需要确保应用程序级数据完整性得以维持的地方使用 fsync() 变得更加重要。)

It should be noted that delayed logging is not an innovative new concept that
warrants rigorous proofs to determine whether it is correct or not. The method
of accumulating changes in memory for some period before writing them to the
log is used effectively in many filesystems including ext3 and ext4. Hence
no time is spent in this document trying to convince the reader that the
concept is sound. Instead it is simply considered a "solved problem" and as
such implementing it in XFS is purely an exercise in software engineering.
(应当指出，延迟日志并不是一个需要严格证明以确定其是否正确的创新性新概念。在写入日志之前在内存中将更改积累一段时间，这一方法已经在包括 ext3 和 ext4 在内的许多文件系统中得到有效使用。因此，本文档不花时间试图说服读者这一概念是可靠的。它只是被视为一个"已解决的问题"，因此在 XFS 中实现它纯粹是一项软件工程练习。)

The fundamental requirements for delayed logging in XFS are simple:
(XFS 中延迟日志的基本要求很简单：)

	1. Reduce the amount of metadata written to the log by at least
	   an order of magnitude.
	   (至少将写入日志的元数据量减少一个数量级。)
	2. Supply sufficient statistics to validate Requirement #1.
	   (提供足够的统计信息来验证需求 #1。)
	3. Supply sufficient new tracing infrastructure to be able to debug
	   problems with the new code.
	   (提供足够的新跟踪基础设施，以便能够调试新代码中的问题。)
	4. No on-disk format change (metadata or log format).
	   (不改变磁盘格式(元数据或日志格式)。)
	5. Enable and disable with a mount option.
	   (通过挂载选项启用和禁用。)
	6. No performance regressions for synchronous transaction workloads.
	   (同步事务工作负载没有性能回归。)

Delayed Logging: Design
(延迟日志：设计)
=======================

Storing Changes
(存储更改)
---------------

The problem with accumulating changes at a logical level (i.e. just using the
existing log item dirty region tracking) is that when it comes to writing the
changes to the log buffers, we need to ensure that the object we are formatting
is not changing while we do this. This requires locking the object to prevent
concurrent modification. Hence flushing the logical changes to the log would
require us to lock every object, format them, and then unlock them again.
(在逻辑层面积累更改的问题(即仅使用现有的日志项脏区域跟踪)在于，当需要将更改写入日志缓冲时，我们需要确保正在格式化的对象在此期间没有发生变化。这需要锁定对象以防止并发修改。因此，将逻辑更改刷新到日志将要求我们锁定每个对象、格式化它们，然后再解锁。)

This introduces lots of scope for deadlocks with transactions that are already
running. For example, a transaction has object A locked and modified, but needs
the delayed logging tracking lock to commit the transaction. However, the
flushing thread has the delayed logging tracking lock already held, and is
trying to get the lock on object A to flush it to the log buffer. This appears
to be an unsolvable deadlock condition, and it was solving this problem that
was the barrier to implementing delayed logging for so long.
(这引入了许多与已经在运行的事务发生死锁的可能性。例如，一个事务已经锁定并修改了对象 A，但需要延迟日志跟踪锁来提交事务。然而，刷新线程已经持有延迟日志跟踪锁，并试图获取对象 A 的锁以便将其刷新到日志缓冲。这看起来是一个无法解决死锁的条件，而正是解决这个问题成为了长期以来实现延迟日志的障碍。)

The solution is relatively simple - it just took a long time to recognise it.
Put simply, the current logging code formats the changes to each item into an
vector array that points to the changed regions in the item. The log write code
simply copies the memory these vectors point to into the log buffer during
transaction commit while the item is locked in the transaction. Instead of
using the log buffer as the destination of the formatting code, we can use an
allocated memory buffer big enough to fit the formatted vector.
(解决方案相对简单——只是花了很长时间才认识到它。简而言之，当前的日志记录代码将每个项的更改格式化为一个向量数组，该数组指向项中被更改的区域。日志写入代码只是在事务提交期间、项被锁定在事务中时，把这些向量所指向的内存复制到日志缓冲中。我们可以使用一个分配好的、大到足以容纳格式化后向量的内存缓冲作为格式化代码的目标，而不是使用日志缓冲。)

If we then copy the vector into the memory buffer and rewrite the vector to
point to the memory buffer rather than the object itself, we now have a copy of
the changes in a format that is compatible with the log buffer writing code.
that does not require us to lock the item to access. This formatting and
rewriting can all be done while the object is locked during transaction commit,
resulting in a vector that is transactionally consistent and can be accessed
without needing to lock the owning item.
(如果我们随后把向量复制到内存缓冲中，并重写向量使其指向内存缓冲而不是对象本身，我们现在就拥有了一份以与日志缓冲写入代码兼容的格式保存的更改副本，访问它不需要我们锁定该项。这种格式化和重写都可以在事务提交期间对象被锁定时完成，从而产生一个在事务上一致的向量，无需锁定其所属项即可访问。)

Hence we avoid the need to lock items when we need to flush outstanding
asynchronous transactions to the log. The differences between the existing
formatting method and the delayed logging formatting can be seen in the
diagram below.
(因此，当我们需要把未决的异步事务刷新到日志时，我们避免了锁定项的需要。现有格式化方法与延迟日志格式化之间的差异可以在下图中看到。)

Current format log vector::
(当前格式的日志向量：)

    Object    +---------------------------------------------+
    Vector 1      +----+
    Vector 2                    +----+
    Vector 3                                   +----------+

After formatting::
(格式化之后：)

    Log Buffer    +-V1-+-V2-+----V3----+

Delayed logging vector::
(延迟日志向量：)

    Object    +---------------------------------------------+
    Vector 1      +----+
    Vector 2                    +----+
    Vector 3                                   +----------+

After formatting::
(格式化之后：)

    Memory Buffer +-V1-+-V2-+----V3----+
    Vector 1      +----+
    Vector 2           +----+
    Vector 3                +----------+

The memory buffer and associated vector need to be passed as a single object,
but still need to be associated with the parent object so if the object is
relogged we can replace the current memory buffer with a new memory buffer that
contains the latest changes.
(内存缓冲和关联的向量需要作为一个单独的对象传递，但仍需要与父对象相关联，这样当对象被重新记录日志时，我们可以用一个新的内存缓冲替换当前的内存缓冲，新缓冲包含最新的更改。)

The reason for keeping the vector around after we've formatted the memory
buffer is to support splitting vectors across log buffer boundaries correctly.
If we don't keep the vector around, we do not know where the region boundaries
are in the item, so we'd need a new encapsulation method for regions in the log
buffer writing (i.e. double encapsulation). This would be an on-disk format
change and as such is not desirable.  It also means we'd have to write the log
region headers in the formatting stage, which is problematic as there is per
region state that needs to be placed into the headers during the log write.
(在格式化内存缓冲之后保留向量的原因，是为了正确支持跨日志缓冲边界拆分向量。如果我们不保留向量，我们就不知道项中区域边界的位置，因此我们需要一种新的封装方法来处理日志缓冲写入中的区域(即双重封装)。这将是一次磁盘格式更改，因此不可取。这也意味着我们必须在格式化阶段写入日志区域头部，而这是有问题的，因为在日志写入期间有一些需要放入头部的每区域状态。)

Hence we need to keep the vector, but by attaching the memory buffer to it and
rewriting the vector addresses to point at the memory buffer we end up with a
self-describing object that can be passed to the log buffer write code to be
handled in exactly the same manner as the existing log vectors are handled.
Hence we avoid needing a new on-disk format to handle items that have been
relogged in memory.
(因此我们需要保留向量，但通过把内存缓冲附着到它上面并重写向量地址使其指向内存缓冲，我们最终得到一个自描述对象，它可以被传递给日志缓冲写入代码，并以与现有日志向量完全相同的方式处理。因此，我们避免了为处理在内存中被重新记录日志的项而需要新的磁盘格式。)


Tracking Changes
(跟踪更改)
----------------

Now that we can record transactional changes in memory in a form that allows
them to be used without limitations, we need to be able to track and accumulate
them so that they can be written to the log at some later point in time.  The
log item is the natural place to store this vector and buffer, and also makes sense
to be the object that is used to track committed objects as it will always
exist once the object has been included in a transaction.
(既然我们能够在内存中以可以不受限制使用的形式记录事务性更改，我们就需要能够跟踪和积累它们，以便在之后的某个时间点将它们写入日志。日志项是存储这个向量和缓冲的天然位置，并且作为用于跟踪已提交对象的对象也很合理，因为一旦对象被包含在事务中，它就会一直存在。)

The log item is already used to track the log items that have been written to
the log but not yet written to disk. Such log items are considered "active"
and as such are stored in the Active Item List (AIL) which is a LSN-ordered
double linked list. Items are inserted into this list during log buffer IO
completion, after which they are unpinned and can be written to disk. An object
that is in the AIL can be relogged, which causes the object to be pinned again
and then moved forward in the AIL when the log buffer IO completes for that
transaction.
(日志项已经被用来跟踪那些已写入日志但尚未写入磁盘的日志项。这样的日志项被认为是"活跃的(active)"，因此被存储在活跃项列表(Active Item List, AIL)中，这是一个按 LSN 排序的双向链表。项在日志缓冲 IO 完成期间被插入该列表，之后它们被解除钉住(pin)并可以写入磁盘。位于 AIL 中的对象可以被重新记录日志，这会使对象再次被钉住，并在该事务的日志缓冲 IO 完成时在 AIL 中向前移动。)

Essentially, this shows that an item that is in the AIL can still be modified
and relogged, so any tracking must be separate to the AIL infrastructure. As
such, we cannot reuse the AIL list pointers for tracking committed items, nor
can we store state in any field that is protected by the AIL lock. Hence the
committed item tracking needs its own locks, lists and state fields in the log
item.
(本质上，这表明位于 AIL 中的项仍然可以被修改和重新记录日志，因此任何跟踪都必须独立于 AIL 基础设施。因此，我们不能复用 AIL 链表指针来跟踪已提交的项，也不能在任何受 AIL 锁保护的字段中存储状态。所以，已提交项的跟踪需要在日志项中拥有自己的锁、链表和状态字段。)

Similar to the AIL, tracking of committed items is done through a new list
called the Committed Item List (CIL).  The list tracks log items that have been
committed and have formatted memory buffers attached to them. It tracks objects
in transaction commit order, so when an object is relogged it is removed from
its place in the list and re-inserted at the tail. This is entirely arbitrary
and done to make it easy for debugging - the last items in the list are the
ones that are most recently modified. Ordering of the CIL is not necessary for
transactional integrity (as discussed in the next section) so the ordering is
done for convenience/sanity of the developers.
(与 AIL 类似，对已提交项的跟踪通过一个称为已提交项列表(Committed Item List, CIL)的新链表完成。该列表跟踪已被提交并附着了格式化内存缓冲的日志项。它按事务提交顺序跟踪对象，因此当一个对象被重新记录日志时，它会从列表中的原位置移除并重新插入到尾部。这完全是任意的选择，只是为了便于调试——列表中靠后的项就是最近被修改的项。CIL 的顺序对于事务完整性并不是必需的(将在下一节讨论)，因此这样做只是为了开发者的便利/合理性。)


Delayed Logging: Checkpoints
(延迟日志：检查点)
----------------------------

When we have a log synchronisation event, commonly known as a "log force",
all the items in the CIL must be written into the log via the log buffers.
We need to write these items in the order that they exist in the CIL, and they
need to be written as an atomic transaction. The need for all the objects to be
written as an atomic transaction comes from the requirements of relogging and
log replay - all the changes in all the objects in a given transaction must
either be completely replayed during log recovery, or not replayed at all. If
a transaction is not replayed because it is not complete in the log, then
no later transactions should be replayed, either.
(当我们有一个日志同步事件时——通常称为"日志强制(log force)"——CIL 中的所有项都必须通过日志缓冲写入日志。我们需要按照这些项在 CIL 中存在的顺序写入它们，并且它们需要作为一个原子事务被写入。所有对象需要作为一个原子事务写入的原因来自重新日志记录和日志重放的要求——给定事务中所有对象的所有更改要么在日志恢复期间被完全重放，要么完全不被重放。如果一个事务因为它在日志中不完整而未被重放，那么后续的事务也不应被重放。)

To fulfill this requirement, we need to write the entire CIL in a single log
transaction. Fortunately, the XFS log code has no fixed limit on the size of a
transaction, nor does the log replay code. The only fundamental limit is that
the transaction cannot be larger than just under half the size of the log.  The
reason for this limit is that to find the head and tail of the log, there must
be at least one complete transaction in the log at any given time. If a
transaction is larger than half the log, then there is the possibility that a
crash during the write of a such a transaction could partially overwrite the
only complete previous transaction in the log. This will result in a recovery
failure and an inconsistent filesystem and hence we must enforce the maximum
size of a checkpoint to be slightly less than a half the log.
(为了满足这一要求，我们需要把整个 CIL 写成单个日志事务。幸运的是，XFS 日志代码对事务的大小没有固定限制，日志重放代码也没有。唯一的根本限制是事务的大小不能超过日志大小的接近一半。这个限制的原因是：为了找到日志的头和尾，在任意给定时刻日志中必须至少有一个完整的事务。如果事务大于日志的一半，那么在写入这样一个事务期间发生崩溃时，就有可能部分覆写日志中唯一完整的先前事务。这将导致恢复失败和文件系统不一致，因此我们必须强制检查点的最大大小略小于日志的一半。)

Apart from this size requirement, a checkpoint transaction looks no different
to any other transaction - it contains a transaction header, a series of
formatted log items and a commit record at the tail. From a recovery
perspective, the checkpoint transaction is also no different - just a lot
bigger with a lot more items in it. The worst case effect of this is that we
might need to tune the recovery transaction object hash size.
(除了这个大小要求之外，检查点事务看起来与其他任何事务没有什么不同——它包含一个事务头部、一系列格式化后的日志项以及位于尾部的提交记录。从恢复的角度来看，检查点事务也没有什么不同——只是更大、里面包含更多的项而已。其最坏情况的影响是我们可能需要调整恢复事务对象哈希表的大小。)

Because the checkpoint is just another transaction and all the changes to log
items are stored as log vectors, we can use the existing log buffer writing
code to write the changes into the log. To do this efficiently, we need to
minimise the time we hold the CIL locked while writing the checkpoint
transaction. The current log write code enables us to do this easily with the
way it separates the writing of the transaction contents (the log vectors) from
the transaction commit record, but tracking this requires us to have a
per-checkpoint context that travels through the log write process through to
checkpoint completion.
(由于检查点只是另一个事务，并且所有对日志项的更改都存储为日志向量，我们可以使用现有的日志缓冲写入代码将更改写入日志。为了高效地做到这一点，我们需要在写入检查点事务时尽量减少持有 CIL 锁的时间。当前的日志写入代码将事务内容(日志向量)的写入与事务提交记录的写入分离，这使我们能够轻松做到这一点，但对此进行跟踪要求我们拥有一个随日志写入过程一直走到检查点完成的每检查点上下文(per-checkpoint context)。)

Hence a checkpoint has a context that tracks the state of the current
checkpoint from initiation to checkpoint completion. A new context is initiated
at the same time a checkpoint transaction is started. That is, when we remove
all the current items from the CIL during a checkpoint operation, we move all
those changes into the current checkpoint context. We then initialise a new
context and attach that to the CIL for aggregation of new transactions.
(因此，一个检查点拥有一个上下文，用于跟踪当前检查点从发起到检查点完成的状态。新的上下文在检查点事务启动的同时发起。也就是说，当我们在检查点操作期间从 CIL 中移除所有当前项时，我们把所有这些更改移入当前检查点上下文。然后我们初始化一个新的上下文并将其附着到 CIL，用于聚合新事务。)

This allows us to unlock the CIL immediately after transfer of all the
committed items and effectively allows new transactions to be issued while we
are formatting the checkpoint into the log. It also allows concurrent
checkpoints to be written into the log buffers in the case of log force heavy
workloads, just like the existing transaction commit code does. This, however,
requires that we strictly order the commit records in the log so that
checkpoint sequence order is maintained during log replay.
(这使我们能够在所有已提交项转移之后立即解锁 CIL，实际上允许在我们将检查点格式化到日志中的同时发出新事务。在日志强制繁重的工作负载下，它还允许多个并发检查点被写入日志缓冲，就像现有的事务提交代码所做的那样。然而，这要求我们严格排序日志中的提交记录，以便在日志重放期间保持检查点序列顺序。)

To ensure that we can be writing an item into a checkpoint transaction at
the same time another transaction modifies the item and inserts the log item
into the new CIL, then checkpoint transaction commit code cannot use log items
to store the list of log vectors that need to be written into the transaction.
Hence log vectors need to be able to be chained together to allow them to be
detached from the log items. That is, when the CIL is flushed the memory
buffer and log vector attached to each log item needs to be attached to the
checkpoint context so that the log item can be released. In diagrammatic form,
the CIL would look like this before the flush::

	CIL Head
	   |
	   V
	Log Item <-> log vector 1	-> memory buffer
	   |				-> vector array
	   V
	Log Item <-> log vector 2	-> memory buffer
	   |				-> vector array
	   V
	......
	   |
	   V
	Log Item <-> log vector N-1	-> memory buffer
	   |				-> vector array
	   V
	Log Item <-> log vector N	-> memory buffer
					-> vector array

(为了确保我们能够在把一个项写入检查点事务的同时，另一个事务修改该项并将日志项插入新的 CIL，检查点事务提交代码就不能使用日志项来存储需要写入事务的日志向量列表。因此，日志向量需要能够链接在一起，使它们可以从日志项上分离。也就是说，当 CIL 被刷新时，附着在每个日志项上的内存缓冲和日志向量需要附着到检查点上下文上，以便释放该日志项。用图示来表示，刷新之前 CIL 如下所示：)

And after the flush the CIL head is empty, and the checkpoint context log
vector list would look like::

	Checkpoint Context
	   |
	   V
	log vector 1	-> memory buffer
	   |		-> vector array
	   |		-> Log Item
	   V
	log vector 2	-> memory buffer
	   |		-> vector array
	   |		-> Log Item
	   V
	......
	   |
	   V
	log vector N-1	-> memory buffer
	   |		-> vector array
	   |		-> Log Item
	   V
	log vector N	-> memory buffer
			-> vector array
			-> Log Item

(刷新之后 CIL 头为空，而检查点上下文的日志向量列表则如下所示：)

Once this transfer is done, the CIL can be unlocked and new transactions can
start, while the checkpoint flush code works over the log vector chain to
commit the checkpoint.
(一旦这个转移完成，CIL 就可以解锁并开始新事务，而检查点刷新代码则处理日志向量链以提交检查点。)

Once the checkpoint is written into the log buffers, the checkpoint context is
attached to the log buffer that the commit record was written to along with a
completion callback. Log IO completion will call that callback, which can then
run transaction committed processing for the log items (i.e. insert into AIL
and unpin) in the log vector chain and then free the log vector chain and
checkpoint context.
(一旦检查点被写入日志缓冲，检查点上下文就会与提交记录一起附着到提交记录所写入的那个日志缓冲上，并附带一个完成回调。日志 IO 完成将调用该回调，然后它可以为日志向量链中的日志项运行事务已提交处理(即插入 AIL 并解除钉住)，然后释放日志向量链和检查点上下文。)

Discussion Point: I am uncertain as to whether the log item is the most
efficient way to track vectors, even though it seems like the natural way to do
it. The fact that we walk the log items (in the CIL) just to chain the log
vectors and break the link between the log item and the log vector means that
we take a cache line hit for the log item list modification, then another for
the log vector chaining. If we track by the log vectors, then we only need to
break the link between the log item and the log vector, which means we should
dirty only the log item cachelines. Normally I wouldn't be concerned about one
vs two dirty cachelines except for the fact I've seen upwards of 80,000 log
vectors in one checkpoint transaction. I'd guess this is a "measure and
compare" situation that can be done after a working and reviewed implementation
is in the dev tree....
(讨论点：我不确定日志项是否是跟踪向量的最有效方式，尽管它看起来是自然的做法。我们遍历日志项(在 CIL 中)只是为了链接日志向量并断开日志项与日志向量之间的链接，这意味着我们在日志项链表修改上付出一次缓存行命中，然后在日志向量链接上再付出一次。如果我们按日志向量进行跟踪，那么我们只需要断开日志项与日志向量之间的链接，这意味着我们应该只弄脏日志项的缓存行。通常我不会在意一个还是两个脏缓存行，只是我曾经在一个检查点事务中看到多达 80,000 个日志向量。我猜这是一个"测量并比较"的情形，可以在一个可工作且经过评审的实现进入 dev 树之后再做……)

Delayed Logging: Checkpoint Sequencing
(延迟日志：检查点排序)
--------------------------------------

One of the key aspects of the XFS transaction subsystem is that it tags
committed transactions with the log sequence number of the transaction commit.
This allows transactions to be issued asynchronously even though there may be
future operations that cannot be completed until that transaction is fully
committed to the log. In the rare case that a dependent operation occurs (e.g.
re-using a freed metadata extent for a data extent), a special, optimised log
force can be issued to force the dependent transaction to disk immediately.
(XFS 事务子系统的关键方面之一是，它用事务提交的日志序列号来标记已提交的事务。这允许事务被异步发出，即使可能存在一些必须等该事务完全提交到日志之后才能完成的未来操作。在出现依赖操作的罕见情况下(例如将一个已释放的元数据 extent 重新用作数据 extent)，可以发出一个特殊的、经过优化的日志强制，立即将依赖的事务强制到磁盘。)

To do this, transactions need to record the LSN of the commit record of the
transaction. This LSN comes directly from the log buffer the transaction is
written into. While this works just fine for the existing transaction
mechanism, it does not work for delayed logging because transactions are not
written directly into the log buffers. Hence some other method of sequencing
transactions is required.
(为此，事务需要记录该事务提交记录的 LSN。这个 LSN 直接来自事务被写入的日志缓冲。虽然这对现有的事务机制工作得很好，但对延迟日志却不适用，因为事务不是直接写入日志缓冲的。因此需要某种其他的事务排序方法。)

As discussed in the checkpoint section, delayed logging uses per-checkpoint
contexts, and as such it is simple to assign a sequence number to each
checkpoint. Because the switching of checkpoint contexts must be done
atomically, it is simple to ensure that each new context has a monotonically
increasing sequence number assigned to it without the need for an external
atomic counter - we can just take the current context sequence number and add
one to it for the new context.
(如检查点一节所讨论的，延迟日志使用每检查点上下文，因此为每个检查点分配一个序列号很简单。由于检查点上下文的切换必须原子地完成，因此可以很容易地确保每个新上下文都分配到一个单调递增的序列号，而无需外部的原子计数器——我们只需取当前上下文的序列号并为其加一，作为新上下文的序列号即可。)

Then, instead of assigning a log buffer LSN to the transaction commit LSN
during the commit, we can assign the current checkpoint sequence. This allows
operations that track transactions that have not yet completed know what
checkpoint sequence needs to be committed before they can continue. As a
result, the code that forces the log to a specific LSN now needs to ensure that
the log forces to a specific checkpoint.
(然后，在提交期间不再为事务提交 LSN 分配日志缓冲 LSN，而是分配当前检查点序列。这使得跟踪尚未完成的事务的操作知道在它们能够继续之前需要提交哪个检查点序列。因此，将日志强制到特定 LSN 的代码现在需要确保日志强制到特定的检查点。)

To ensure that we can do this, we need to track all the checkpoint contexts
that are currently committing to the log. When we flush a checkpoint, the
context gets added to a "committing" list which can be searched. When a
checkpoint commit completes, it is removed from the committing list. Because
the checkpoint context records the LSN of the commit record for the checkpoint,
we can also wait on the log buffer that contains the commit record, thereby
using the existing log force mechanisms to execute synchronous forces.
(为了确保我们能够做到这一点，我们需要跟踪当前正在向日志提交的所有检查点上下文。当我们刷新一个检查点时，该上下文会被加入一个可以搜索的"提交中(committing)"列表。当一个检查点提交完成时，它会从提交中列表中移除。由于检查点上下文记录了该检查点提交记录的 LSN，我们还可以等待包含提交记录的那个日志缓冲，从而利用现有的日志强制机制来执行同步强制。)

It should be noted that the synchronous forces may need to be extended with
mitigation algorithms similar to the current log buffer code to allow
aggregation of multiple synchronous transactions if there are already
synchronous transactions being flushed. Investigation of the performance of the
current design is needed before making any decisions here.
(应当指出，如果已经有同步事务正在被刷新，同步强制可能需要用类似于当前日志缓冲代码的缓解算法进行扩展，以允许聚合多个同步事务。在做出任何决定之前，需要对当前设计的性能进行研究。)

The main concern with log forces is to ensure that all the previous checkpoints
are also committed to disk before the one we need to wait for. Therefore we
need to check that all the prior contexts in the committing list are also
complete before waiting on the one we need to complete. We do this
synchronisation in the log force code so that we don't need to wait anywhere
else for such serialisation - it only matters when we do a log force.
(日志强制的主要关注点是确保所有先前的检查点在我们需要等待的那个之前也被提交到磁盘。因此，在等待我们需要完成的那一个之前，我们需要检查提交中列表中所有先前上下文是否也已完成。我们在日志强制代码中做这种同步，这样我们就不需要在其他任何地方等待这种串行化——只有在我们执行日志强制时它才有影响。)

The only remaining complexity is that a log force now also has to handle the
case where the forcing sequence number is the same as the current context. That
is, we need to flush the CIL and potentially wait for it to complete. This is a
simple addition to the existing log forcing code to check the sequence numbers
and push if required. Indeed, placing the current sequence checkpoint flush in
the log force code enables the current mechanism for issuing synchronous
transactions to remain untouched (i.e. commit an asynchronous transaction, then
force the log at the LSN of that transaction) and so the higher level code
behaves the same regardless of whether delayed logging is being used or not.
(剩下的唯一复杂性是，日志强制现在还必须处理强制序列号与当前上下文相同的情况。也就是说，我们需要刷新 CIL 并可能等待其完成。这是对现有日志强制代码的一个简单补充：检查序列号并在需要时推送。实际上，把当前序列的检查点刷新放在日志强制代码中，使发出同步事务的现有机制保持不变(即提交一个异步事务，然后在该事务的 LSN 处强制日志)，因此无论是否使用延迟日志，高层代码的行为都是相同的。)

Delayed Logging: Checkpoint Log Space Accounting
(延迟日志：检查点日志空间记账)
------------------------------------------------

The big issue for a checkpoint transaction is the log space reservation for the
transaction. We don't know how big a checkpoint transaction is going to be
ahead of time, nor how many log buffers it will take to write out, nor the
number of split log vector regions are going to be used. We can track the
amount of log space required as we add items to the commit item list, but we
still need to reserve the space in the log for the checkpoint.
(检查点事务的大问题是为该事务进行的日志空间预留。我们无法提前知道检查点事务会有多大，也无法知道写出它需要多少个日志缓冲，也不知道会使用多少个拆分的日志向量区域。我们可以在向提交项列表添加项的同时跟踪所需的日志空间量，但我们仍然需要在日志中为检查点预留空间。)

A typical transaction reserves enough space in the log for the worst case space
usage of the transaction. The reservation accounts for log record headers,
transaction and region headers, headers for split regions, buffer tail padding,
etc. as well as the actual space for all the changed metadata in the
transaction. While some of this is fixed overhead, much of it is dependent on
the size of the transaction and the number of regions being logged (the number
of log vectors in the transaction).
(一个典型的事务为事务的最坏情况空间使用预留了足够的日志空间。该预留考虑了日志记录头部、事务和区域头部、拆分区域的头部、缓冲尾部填充等等，以及事务中所有被更改元数据的实际空间。虽然其中一部分是固定开销，但大部分取决于事务的大小以及被记录日志的区域数量(事务中日志向量的数量)。)

An example of the differences would be logging directory changes versus logging
inode changes. If you modify lots of inode cores (e.g. ``chmod -R g+w *``), then
there are lots of transactions that only contain an inode core and an inode log
format structure. That is, two vectors totaling roughly 150 bytes. If we modify
10,000 inodes, we have about 1.5MB of metadata to write in 20,000 vectors. Each
vector is 12 bytes, so the total to be logged is approximately 1.75MB. In
comparison, if we are logging full directory buffers, they are typically 4KB
each, so we in 1.5MB of directory buffers we'd have roughly 400 buffers and a
buffer format structure for each buffer - roughly 800 vectors or 1.51MB total
space.  From this, it should be obvious that a static log space reservation is
not particularly flexible and is difficult to select the "optimal value" for
all workloads.
(差异的一个例子是记录目录更改与记录 inode 更改的对比。如果你修改大量 inode 核心(例如 ``chmod -R g+w *``)，那么会有很多事务只包含一个 inode 核心和一个 inode 日志格式结构。也就是说，两个向量总计约 150 字节。如果我们修改 10,000 个 inode，就有大约 1.5MB 的元数据要写入 20,000 个向量。每个向量 12 字节，因此要记录的总量约为 1.75MB。相比之下，如果我们记录完整的目录缓冲，它们通常每个 4KB，因此在 1.5MB 的目录缓冲中我们大约有 400 个缓冲和每个缓冲一个缓冲格式结构——大约 800 个向量或总计 1.51MB 的空间。由此应该很明显，静态日志空间预留并不特别灵活，而且很难为所有工作负载选择"最优值"。)

Further, if we are going to use a static reservation, which bit of the entire
reservation does it cover? We account for space used by the transaction
reservation by tracking the space currently used by the object in the CIL and
then calculating the increase or decrease in space used as the object is
relogged. This allows for a checkpoint reservation to only have to account for
log buffer metadata used such as log header records.
(此外，如果我们要使用静态预留，整个预留中的哪一部分用于它？我们通过跟踪对象当前在 CIL 中使用的空间，然后在对象被重新记录日志时计算所用空间的增减，来为事务预留所使用的空间记账。这使得检查点预留只需要考虑所用到的日志缓冲元数据，例如日志头部记录。)

However, even using a static reservation for just the log metadata is
problematic. Typically log record headers use at least 16KB of log space per
1MB of log space consumed (512 bytes per 32k) and the reservation needs to be
large enough to handle arbitrary sized checkpoint transactions. This
reservation needs to be made before the checkpoint is started, and we need to
be able to reserve the space without sleeping.  For a 8MB checkpoint, we need a
reservation of around 150KB, which is a non-trivial amount of space.
(然而，即使只对日志元数据使用静态预留也是有问题的。通常，每消耗 1MB 日志空间，日志记录头部至少要使用 16KB 的日志空间(每 32k 为 512 字节)，而预留需要足够大以处理任意大小的检查点事务。这个预留需要在检查点开始之前完成，而且我们需要能够在不睡眠的情况下预留空间。对于一个 8MB 的检查点，我们需要大约 150KB 的预留，这是一个不小的空间量。)

A static reservation needs to manipulate the log grant counters - we can take a
permanent reservation on the space, but we still need to make sure we refresh
the write reservation (the actual space available to the transaction) after
every checkpoint transaction completion. Unfortunately, if this space is not
available when required, then the regrant code will sleep waiting for it.
(静态预留需要操作日志授权计数器——我们可以对空间进行永久预留，但我们仍然需要确保在每个检查点事务完成之后刷新写预留(事务实际可用的空间)。不幸的是，如果这个空间在需要时不可用，那么重新授予代码将睡眠等待。)

The problem with this is that it can lead to deadlocks as we may need to commit
checkpoints to be able to free up log space (refer back to the description of
rolling transactions for an example of this).  Hence we *must* always have
space available in the log if we are to use static reservations, and that is
very difficult and complex to arrange. It is possible to do, but there is a
simpler way.
(这样做的问题在于它可能导致死锁，因为我们可能需要提交检查点才能释放日志空间(关于这种情况的例子，可以回看前面关于滚动事务的描述)。因此，如果我们要使用静态预留，我们 *必须* 始终在日志中有可用的空间，而这非常难以安排且复杂。这是可以做到的，但有一种更简单的方法。)

The simpler way of doing this is tracking the entire log space used by the
items in the CIL and using this to dynamically calculate the amount of log
space required by the log metadata. If this log metadata space changes as a
result of a transaction commit inserting a new memory buffer into the CIL, then
the difference in space required is removed from the transaction that causes
the change. Transactions at this level will *always* have enough space
available in their reservation for this as they have already reserved the
maximal amount of log metadata space they require, and such a delta reservation
will always be less than or equal to the maximal amount in the reservation.
(更简单的方法是跟踪 CIL 中各项所使用的全部日志空间，并用它来动态计算日志元数据所需的日志空间量。如果一个事务提交向 CIL 插入一个新的内存缓冲导致这个日志元数据空间发生变化，那么所需空间的差额就从引起该变化的事务中扣除。这一层次的事务的预留中 *总是* 有足够的空间可用于此，因为它们已经预留了所需的日志元数据空间的最大量，而这样的差额预留总是小于或等于预留中的最大量。)

Hence we can grow the checkpoint transaction reservation dynamically as items
are added to the CIL and avoid the need for reserving and regranting log space
up front. This avoids deadlocks and removes a blocking point from the
checkpoint flush code.
(因此，我们可以随着项被加入 CIL 而动态增长检查点事务预留，避免了预先预留和重新授予日志空间的需要。这避免了死锁，并从检查点刷新代码中移除了一个阻塞点。)

As mentioned early, transactions can't grow to more than half the size of the
log. Hence as part of the reservation growing, we need to also check the size
of the reservation against the maximum allowed transaction size. If we reach
the maximum threshold, we need to push the CIL to the log. This is effectively
a "background flush" and is done on demand. This is identical to
a CIL push triggered by a log force, only that there is no waiting for the
checkpoint commit to complete. This background push is checked and executed by
transaction commit code.
(如前面提到的，事务不能增长到超过日志大小的一半。因此，作为预留增长的一部分，我们还需要根据允许的最大事务大小来检查预留的大小。如果达到最大阈值，我们需要把 CIL 推送到日志。这实际上是一种"后台刷新"，按需进行。它与由日志强制触发的 CIL 推送完全相同，只是不等待检查点提交完成。这个后台推送由事务提交代码检查并执行。)

If the transaction subsystem goes idle while we still have items in the CIL,
they will be flushed by the periodic log force issued by the xfssyncd. This log
force will push the CIL to disk, and if the transaction subsystem stays idle,
allow the idle log to be covered (effectively marked clean) in exactly the same
manner that is done for the existing logging method. A discussion point is
whether this log force needs to be done more frequently than the current rate
which is once every 30s.
(如果事务子系统进入空闲状态而 CIL 中仍有项，它们将通过 xfssyncd 发出的周期性日志强制被刷新。这个日志强制会把 CIL 推送到磁盘，如果事务子系统保持空闲，则允许空闲日志被覆盖(有效地标记为干净)，其方式与现有日志记录方法所做的方式完全相同。一个讨论点是，这个日志强制是否需要比当前每 30 秒一次的频率更频繁地执行。)


Delayed Logging: Log Item Pinning
(延迟日志：日志项钉住)
---------------------------------

Currently log items are pinned during transaction commit while the items are
still locked. This happens just after the items are formatted, though it could
be done any time before the items are unlocked. The result of this mechanism is
that items get pinned once for every transaction that is committed to the log
buffers. Hence items that are relogged in the log buffers will have a pin count
for every outstanding transaction they were dirtied in. When each of these
transactions is completed, they will unpin the item once. As a result, the item
only becomes unpinned when all the transactions complete and there are no
pending transactions. Thus the pinning and unpinning of a log item is symmetric
as there is a 1:1 relationship with transaction commit and log item completion.
(目前，日志项在事务提交期间、项仍被锁定时被钉住(pin)。这发生在项被格式化之后，尽管它也可以在项被解锁之前的任何时间完成。这种机制的结果是，项为每个提交到日志缓冲的事务被钉住一次。因此，在日志缓冲中被重新记录日志的项，对于它们在其中变脏的每个未决事务都有一个钉住计数。当这些事务中的每一个完成时，它们会将该项解除钉住一次。因此，只有当所有事务都完成且没有未决事务时，项才变为未钉住状态。于是，日志项的钉住与解除钉住是对称的，与事务提交和日志项完成之间存在 1:1 的关系。)

For delayed logging, however, we have an asymmetric transaction commit to
completion relationship. Every time an object is relogged in the CIL it goes
through the commit process without a corresponding completion being registered.
That is, we now have a many-to-one relationship between transaction commit and
log item completion. The result of this is that pinning and unpinning of the
log items becomes unbalanced if we retain the "pin on transaction commit, unpin
on transaction completion" model.
(然而，对于延迟日志，我们有一种从事务提交到完成不对称的关系。每当一个对象在 CIL 中被重新记录日志时，它都会经历提交过程，而没有相应的完成被注册。也就是说，我们现在在事务提交与日志项完成之间拥有多对一的关系。其结果是，如果我们保留"提交时钉住、完成时解除钉住"的模型，日志项的钉住和解除钉住就会变得不平衡。)

To keep pin/unpin symmetry, the algorithm needs to change to a "pin on
insertion into the CIL, unpin on checkpoint completion". In other words, the
pinning and unpinning becomes symmetric around a checkpoint context. We have to
pin the object the first time it is inserted into the CIL - if it is already in
the CIL during a transaction commit, then we do not pin it again. Because there
can be multiple outstanding checkpoint contexts, we can still see elevated pin
counts, but as each checkpoint completes the pin count will retain the correct
value according to its context.
(为了保持钉住/解除钉住的对称性，算法需要改为"插入 CIL 时钉住、检查点完成时解除钉住"。换句话说，钉住和解除钉住围绕检查点上下文变得对称。我们必须在对象第一次被插入 CIL 时钉住它——如果它在事务提交期间已经在 CIL 中，那么我们不再钉住它。由于可能有多个未决的检查点上下文，我们仍然可能看到偏高的钉住计数，但随着每个检查点完成，钉住计数将根据其上下文保持正确的值。)

Just to make matters slightly more complex, this checkpoint level context
for the pin count means that the pinning of an item must take place under the
CIL commit/flush lock. If we pin the object outside this lock, we cannot
guarantee which context the pin count is associated with. This is because of
the fact pinning the item is dependent on whether the item is present in the
current CIL or not. If we don't pin the CIL first before we check and pin the
object, we have a race with CIL being flushed between the check and the pin
(or not pinning, as the case may be). Hence we must hold the CIL flush/commit
lock to guarantee that we pin the items correctly.
(为了让事情稍微更复杂一点，这个检查点级别的钉住计数上下文意味着对项的钉住必须在 CIL 提交/刷新锁之下进行。如果我们在锁之外钉住对象，我们就无法保证钉住计数与哪个上下文相关联。这是因为钉住项取决于该项是否存在于当前 CIL 中。如果我们在检查并钉住对象之前不先持有 CIL 锁，我们就会在检查和钉住之间与 CIL 被刷新产生竞争(或者按情况而不钉住)。因此，我们必须持有 CIL 刷新/提交锁来保证我们正确地钉住各项。)

Delayed Logging: Concurrent Scalability
(延迟日志：并发可扩展性)
---------------------------------------

A fundamental requirement for the CIL is that accesses through transaction
commits must scale to many concurrent commits. The current transaction commit
code does not break down even when there are transactions coming from 2048
processors at once. The current transaction code does not go any faster than if
there was only one CPU using it, but it does not slow down either.
(CIL 的一个基本要求是，通过事务提交进行的访问必须能够扩展到大量并发提交。当前的事务提交代码即使同时有来自 2048 个处理器的事务也不会崩溃。当前的事务代码并不比只有一个 CPU 使用它时更快，但它也不会变慢。)

As a result, the delayed logging transaction commit code needs to be designed
for concurrency from the ground up. It is obvious that there are serialisation
points in the design - the three important ones are:
(因此，延迟日志事务提交代码需要从底层开始就为并发而设计。显然设计中存在串行化点——其中三个重要的点是：)

	1. Locking out new transaction commits while flushing the CIL
	   (在刷新 CIL 期间阻塞新的事务提交)
	2. Adding items to the CIL and updating item space accounting
	   (向 CIL 添加项并更新项空间记账)
	3. Checkpoint commit ordering
	   (检查点提交排序)

Looking at the transaction commit and CIL flushing interactions, it is clear
that we have a many-to-one interaction here. That is, the only restriction on
the number of concurrent transactions that can be trying to commit at once is
the amount of space available in the log for their reservations. The practical
limit here is in the order of several hundred concurrent transactions for a
128MB log, which means that it is generally one per CPU in a machine.
(观察事务提交与 CIL 刷新之间的交互，可以清楚地看到这里存在多对一的交互。也就是说，对可以同时尝试提交的并发事务数量的唯一限制，是日志中可用于它们预留的空间量。对于 128MB 的日志，这里的实际上限是数百个并发事务的量级，这意味着在机器中通常是每个 CPU 一个。)

The amount of time a transaction commit needs to hold out a flush is a
relatively long period of time - the pinning of log items needs to be done
while we are holding out a CIL flush, so at the moment that means it is held
across the formatting of the objects into memory buffers (i.e. while memcpy()s
are in progress). Ultimately a two pass algorithm where the formatting is done
separately to the pinning of objects could be used to reduce the hold time of
the transaction commit side.
(事务提交需要推迟刷新的时间是一个相对较长的时期——日志项的钉住需要在我们推迟 CIL 刷新期间进行，所以目前这意味着它在对象被格式化到内存缓冲期间(即 memcpy() 进行期间)一直被持有。最终，可以采用一种两遍算法，将格式化与对象的钉住分开进行，以减少事务提交一侧的持有时间。)

Because of the number of potential transaction commit side holders, the lock
really needs to be a sleeping lock - if the CIL flush takes the lock, we do not
want every other CPU in the machine spinning on the CIL lock. Given that
flushing the CIL could involve walking a list of tens of thousands of log
items, it will get held for a significant time and so spin contention is a
significant concern. Preventing lots of CPUs spinning doing nothing is the
main reason for choosing a sleeping lock even though nothing in either the
transaction commit or CIL flush side sleeps with the lock held.
(由于事务提交侧潜在持有者的数量，这个锁确实需要是一个睡眠锁——如果 CIL 刷新持有该锁，我们不希望机器中的每个其他 CPU 都在 CIL 锁上自旋。考虑到刷新 CIL 可能涉及遍历包含数万个日志项的列表，它会被持有相当长的时间，因此自旋竞争是一个重大隐忧。即使事务提交侧或 CIL 刷新侧都没有在持锁的情况下睡眠，选择睡眠锁的主要理由也是防止大量 CPU 无所事事地自旋。)

It should also be noted that CIL flushing is also a relatively rare operation
compared to transaction commit for asynchronous transaction workloads - only
time will tell if using a read-write semaphore for exclusion will limit
transaction commit concurrency due to cache line bouncing of the lock on the
read side.
(还应当指出，对于异步事务工作负载，与事务提交相比，CIL 刷新也是相对少见的操作——使用读写信号量进行互斥是否会因为锁在读取侧的缓存行弹跳而限制事务提交的并发性，只有时间才能证明。)

The second serialisation point is on the transaction commit side where items
are inserted into the CIL. Because transactions can enter this code
concurrently, the CIL needs to be protected separately from the above
commit/flush exclusion. It also needs to be an exclusive lock but it is only
held for a very short time and so a spin lock is appropriate here. It is
possible that this lock will become a contention point, but given the short
hold time once per transaction I think that contention is unlikely.
(第二个串行化点在事务提交侧，即项被插入 CIL 的地方。由于事务可以并发地进入这段代码，CIL 需要与上述提交/刷新互斥分开保护。它也需要是一个独占锁，但它只会被持有很短的时间，因此这里使用自旋锁是合适的。这个锁有可能成为竞争点，但考虑到每个事务只有一次的短暂持有时间，我认为竞争不太可能发生。)

The final serialisation point is the checkpoint commit record ordering code
that is run as part of the checkpoint commit and log force sequencing. The code
path that triggers a CIL flush (i.e. whatever triggers the log force) will enter
an ordering loop after writing all the log vectors into the log buffers but
before writing the commit record. This loop walks the list of committing
checkpoints and needs to block waiting for checkpoints to complete their commit
record write. As a result it needs a lock and a wait variable. Log force
sequencing also requires the same lock, list walk, and blocking mechanism to
ensure completion of checkpoints.
(最后一个串行化点是作为检查点提交和日志强制排序的一部分运行的检查点提交记录排序代码。触发 CIL 刷新的代码路径(即无论什么触发日志强制)会在将所有日志向量写入日志缓冲之后、写入提交记录之前，进入一个排序循环。这个循环遍历提交中检查点的列表，需要阻塞等待检查点完成其提交记录写入。因此，它需要一个锁和一个等待变量。日志强制排序也需要相同的锁、列表遍历和阻塞机制来确保检查点完成。)

These two sequencing operations can use the mechanism even though the
events they are waiting for are different. The checkpoint commit record
sequencing needs to wait until checkpoint contexts contain a commit LSN
(obtained through completion of a commit record write) while log force
sequencing needs to wait until previous checkpoint contexts are removed from
the committing list (i.e. they've completed). A simple wait variable and
broadcast wakeups (thundering herds) has been used to implement these two
serialisation queues. They use the same lock as the CIL, too. If we see too
much contention on the CIL lock, or too many context switches as a result of
the broadcast wakeups these operations can be put under a new spinlock and
given separate wait lists to reduce lock contention and the number of processes
woken by the wrong event.
(这两个排序操作可以使用同一机制，尽管它们等待的事件不同。检查点提交记录排序需要等待直到检查点上下文包含提交 LSN(通过提交记录写入的完成获得)，而日志强制排序需要等待直到先前的检查点上下文从提交中列表中移除(即它们已完成)。一个简单的等待变量和广播唤醒(惊群)已被用来实现这两个串行化队列。它们也使用与 CIL 相同的锁。如果我们看到 CIL 锁上有太多竞争，或者由于广播唤醒导致太多上下文切换，这些操作可以被置于一个新的自旋锁之下，并给予单独的等待列表，以减少锁竞争和被错误事件唤醒的进程数量。)


Lifecycle Changes
(生命周期变化)
-----------------

The existing log item life cycle is as follows::
(现有的日志项生命周期如下：)

	1. Transaction allocate
	2. Transaction reserve
	3. Lock item
	4. Join item to transaction
		If not already attached,
			Allocate log item
			Attach log item to owner item
		Attach log item to transaction
	5. Modify item
		Record modifications in log item
	6. Transaction commit
		Pin item in memory
		Format item into log buffer
		Write commit LSN into transaction
		Unlock item
		Attach transaction to log buffer

	<log buffer IO dispatched>
	<log buffer IO completes>

	7. Transaction completion
		Mark log item committed
		Insert log item into AIL
			Write commit LSN into log item
		Unpin log item
	8. AIL traversal
		Lock item
		Mark log item clean
		Flush item to disk

	<item IO completion>

	9. Log item removed from AIL
		Moves log tail
		Item unlocked

Essentially, steps 1-6 operate independently from step 7, which is also
independent of steps 8-9. An item can be locked in steps 1-6 or steps 8-9
at the same time step 7 is occurring, but only steps 1-6 or 8-9 can occur
at the same time. If the log item is in the AIL or between steps 6 and 7
and steps 1-6 are re-entered, then the item is relogged. Only when steps 8-9
are entered and completed is the object considered clean.
(本质上，步骤 1-6 独立于步骤 7 运行，步骤 7 也独立于步骤 8-9。一个项可以在步骤 1-6 或步骤 8-9 中被锁定，同时步骤 7 正在发生，但同时只能发生步骤 1-6 或 8-9。如果日志项在 AIL 中或在步骤 6 和 7 之间，而步骤 1-6 被重新进入，那么该项被重新记录日志。只有当步骤 8-9 被进入并完成之后，该对象才被视为干净的。)

With delayed logging, there are new steps inserted into the life cycle::
(对于延迟日志，生命周期中插入了新的步骤：)

	1. Transaction allocate
	2. Transaction reserve
	3. Lock item
	4. Join item to transaction
		If not already attached,
			Allocate log item
			Attach log item to owner item
		Attach log item to transaction
	5. Modify item
		Record modifications in log item
	6. Transaction commit
		Pin item in memory if not pinned in CIL
		Format item into log vector + buffer
		Attach log vector and buffer to log item
		Insert log item into CIL
		Write CIL context sequence into transaction
		Unlock item

	<next log force>

	7. CIL push
		lock CIL flush
		Chain log vectors and buffers together
		Remove items from CIL
		unlock CIL flush
		write log vectors into log
		sequence commit records
		attach checkpoint context to log buffer

	<log buffer IO dispatched>
	<log buffer IO completes>

	8. Checkpoint completion
		Mark log item committed
		Insert item into AIL
			Write commit LSN into log item
		Unpin log item
	9. AIL traversal
		Lock item
		Mark log item clean
		Flush item to disk
	<item IO completion>
	10. Log item removed from AIL
		Moves log tail
		Item unlocked

From this, it can be seen that the only life cycle differences between the two
logging methods are in the middle of the life cycle - they still have the same
beginning and end and execution constraints. The only differences are in the
committing of the log items to the log itself and the completion processing.
Hence delayed logging should not introduce any constraints on log item
behaviour, allocation or freeing that don't already exist.
(由此可以看出，两种日志方法之间唯一的生命周期差异位于生命周期的中部——它们仍然具有相同的开始、结束和执行约束。唯一的差异在于将日志项提交到日志本身的方式以及完成处理。因此，延迟日志不应该对日志项的行为、分配或释放引入任何尚不存在的约束。)

As a result of this zero-impact "insertion" of delayed logging infrastructure
and the design of the internal structures to avoid on disk format changes, we
can basically switch between delayed logging and the existing mechanism with a
mount option. Fundamentally, there is no reason why the log manager would not
be able to swap methods automatically and transparently depending on load
characteristics, but this should not be necessary if delayed logging works as
designed.
(由于延迟日志基础设施的这种零影响"插入"，以及内部结构为避免磁盘格式更改而做的设计，我们基本上可以通过一个挂载选项在延迟日志和现有机制之间切换。从根本上说，没有理由认为日志管理器不能根据负载特征自动且透明地切换方法，但如果延迟日志按设计工作，这应该是不必要的。)
