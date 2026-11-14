The Linux Journalling API
(Linux 日志 API)
=========================

Overview
(概述)
--------

Details
(细节)
~~~~~~~

The journalling layer is easy to use. You need to first of all create a
journal_t data structure. There are two calls to do this dependent on
how you decide to allocate the physical media on which the journal
resides. The jbd2_journal_init_inode() call is for journals stored in
filesystem inodes, or the jbd2_journal_init_dev() call can be used
for journal stored on a raw device (in a continuous range of blocks). A
journal_t is a typedef for a struct pointer, so when you are finally
finished make sure you call jbd2_journal_destroy() on it to free up
any used kernel memory.
(日志层易于使用。首先你需要创建一个 journal_t 数据结构。根据你决定如何分配日志所在的物理介质，有两个调用可用于完成此操作。jbd2_journal_init_inode() 调用用于存储在文件系统 inode 中的日志，而 jbd2_journal_init_dev() 调用可用于存储在原始设备上(在连续的块范围内)的日志。journal_t 是一个结构体指针的 typedef，所以当你最终完成时，请确保对它调用 jbd2_journal_destroy() 以释放任何已使用的内核内存。)

Once you have got your journal_t object you need to 'mount' or load the
journal file. The journalling layer expects the space for the journal
was already allocated and initialized properly by the userspace tools.
When loading the journal you must call jbd2_journal_load() to process
journal contents. If the client file system detects the journal contents
does not need to be processed (or even need not have valid contents), it
may call jbd2_journal_wipe() to clear the journal contents before
calling jbd2_journal_load().
(一旦你得到了你的 journal_t 对象，你就需要"挂载"或加载日志文件。日志层期望日志的空间已经由用户空间工具正确地分配并初始化。在加载日志时，你必须调用 jbd2_journal_load() 来处理日志内容。如果客户端文件系统检测到日志内容不需要处理(甚至不需要具有有效内容)，它可以在调用 jbd2_journal_load() 之前先调用 jbd2_journal_wipe() 来清除日志内容。)

Note that jbd2_journal_wipe(..,0) calls
jbd2_journal_skip_recovery() for you if it detects any outstanding
transactions in the journal and similarly jbd2_journal_load() will
call jbd2_journal_recover() if necessary. I would advise reading
ext4_load_journal() in fs/ext4/super.c for examples on this stage.
(注意，如果 jbd2_journal_wipe(..,0) 检测到日志中存在任何未完成的事务，它会替你调用 jbd2_journal_skip_recovery()；类似地，jbd2_journal_load() 会在必要时调用 jbd2_journal_recover()。我建议阅读 fs/ext4/super.c 中的 ext4_load_journal()，作为这一阶段的示例。)

Now you can go ahead and start modifying the underlying filesystem.
Almost.
(现在你可以着手开始修改底层文件系统了。差不多是这样。)

You still need to actually journal your filesystem changes, this is done
by wrapping them into transactions. Additionally you also need to wrap
the modification of each of the buffers with calls to the journal layer,
so it knows what the modifications you are actually making are. To do
this use jbd2_journal_start() which returns a transaction handle.
(你仍然需要实际地把你的文件系统变更记入日志，这是通过把它们包装进事务来完成的。此外，你还需要用对日志层的调用来包装每个缓冲的修改，这样日志层才知道你实际所做的修改是什么。为此可以使用 jbd2_journal_start()，它会返回一个事务句柄。)

jbd2_journal_start() and its counterpart jbd2_journal_stop(),
which indicates the end of a transaction are nestable calls, so you can
reenter a transaction if necessary, but remember you must call
jbd2_journal_stop() the same number of times as
jbd2_journal_start() before the transaction is completed (or more
accurately leaves the update phase). Ext4/VFS makes use of this feature to
simplify handling of inode dirtying, quota support, etc.
(jbd2_journal_start() 与其对应的 jbd2_journal_stop()(它表示一个事务的结束)是可嵌套的调用，所以必要时你可以重新进入一个事务，但请记住，在事务完成之前(更准确地说，是离开更新阶段之前)，你必须以与 jbd2_journal_start() 相同的次数调用 jbd2_journal_stop()。Ext4/VFS 利用这一特性来简化对 inode 弄脏、配额支持等的处理。)

Inside each transaction you need to wrap the modifications to the
individual buffers (blocks). Before you start to modify a buffer you
need to call jbd2_journal_get_create_access() /
jbd2_journal_get_write_access() /
jbd2_journal_get_undo_access() as appropriate, this allows the
journalling layer to copy the unmodified
data if it needs to. After all the buffer may be part of a previously
uncommitted transaction. At this point you are at last ready to modify a
buffer, and once you are have done so you need to call
jbd2_journal_dirty_metadata(). Or if you've asked for access to a
buffer you now know is now longer required to be pushed back on the
device you can call jbd2_journal_forget() in much the same way as you
might have used bforget() in the past.
(在每个事务内部，你需要包装对各个缓冲(块)的修改。在你开始修改一个缓冲之前，你需要视情况调用 jbd2_journal_get_create_access() / jbd2_journal_get_write_access() / jbd2_journal_get_undo_access()，这使得日志层能够在需要时复制未修改的数据。毕竟该缓冲可能是某个先前未提交事务的一部分。此时你终于准备好修改一个缓冲了，一旦你修改完成，就需要调用 jbd2_journal_dirty_metadata()。或者，如果你申请了对某个缓冲的访问，而现在知道不再需要把它推回设备，你可以调用 jbd2_journal_forget()，其方式与你过去可能使用 bforget() 的方式非常相似。)

A jbd2_journal_flush() may be called at any time to commit and
checkpoint all your transactions.
(可以随时调用 jbd2_journal_flush() 来提交你的所有事务并对其进行检查点处理。)

Then at umount time , in your put_super() you can then call
jbd2_journal_destroy() to clean up your in-core journal object.
(然后在卸载时，在你的 put_super() 中可以调用 jbd2_journal_destroy() 来清理你的核心内日志对象。)

Unfortunately there a couple of ways the journal layer can cause a
deadlock. The first thing to note is that each task can only have a
single outstanding transaction at any one time, remember nothing commits
until the outermost jbd2_journal_stop(). This means you must complete
the transaction at the end of each file/inode/address etc. operation you
perform, so that the journalling system isn't re-entered on another
journal. Since transactions can't be nested/batched across differing
journals, and another filesystem other than yours (say ext4) may be
modified in a later syscall.
(遗憾的是，有几种方式可能导致日志层发生死锁。首先要注意的是，每个任务在同一时刻只能有一个未完成的事务，记住在最外层的 jbd2_journal_stop() 之前什么都不会提交。这意味着你必须在你执行的每个文件/inode/地址等操作的末尾完成该事务，这样日志系统就不会在另一个日志上被重入。因为事务不能跨不同的日志进行嵌套/批量处理，而另一个文件系统(比如 ext4)可能会在随后的系统调用中被修改。)

The second case to bear in mind is that jbd2_journal_start() can block
if there isn't enough space in the journal for your transaction (based
on the passed nblocks param) - when it blocks it merely(!) needs to wait
for transactions to complete and be committed from other tasks, so
essentially we are waiting for jbd2_journal_stop(). So to avoid
deadlocks you must treat jbd2_journal_start() /
jbd2_journal_stop() as if they were semaphores and include them in
your semaphore ordering rules to prevent
deadlocks. Note that jbd2_journal_extend() has similar blocking
behaviour to jbd2_journal_start() so you can deadlock here just as
easily as on jbd2_journal_start().
(需要记住的第二种情况是，如果日志中没有足够的空间容纳你的事务(根据传入的 nblocks 参数)，jbd2_journal_start() 可能会阻塞——当它阻塞时，它只是(!)需要等待其他任务的事务完成并提交，所以本质上我们是在等待 jbd2_journal_stop()。因此，为了避免死锁，你必须把 jbd2_journal_start() / jbd2_journal_stop() 当作信号量来对待，并把它们纳入你的信号量排序规则中以防止死锁。注意，jbd2_journal_extend() 具有与 jbd2_journal_start() 类似的阻塞行为，所以你在这一点上死锁的可能性与在 jbd2_journal_start() 上一样大。)

Try to reserve the right number of blocks the first time. ;-). This will
be the maximum number of blocks you are going to touch in this
transaction. I advise having a look at at least ext4_jbd.h to see the
basis on which ext4 uses to make these decisions.
(争取第一次就保留正确数量的块。;-)。这将是你在这个事务中要触及的块的最大数量。我建议至少看一看 ext4_jbd.h，以了解 ext4 做出这些决策所依据的基础。)

Another wriggle to watch out for is your on-disk block allocation
strategy. Why? Because, if you do a delete, you need to ensure you
haven't reused any of the freed blocks until the transaction freeing
these blocks commits. If you reused these blocks and crash happens,
there is no way to restore the contents of the reallocated blocks at the
end of the last fully committed transaction. One simple way of doing
this is to mark blocks as free in internal in-memory block allocation
structures only after the transaction freeing them commits. Ext4 uses
journal commit callback for this purpose.
(另一个需要留意的波折是你的磁盘上块分配策略。为什么？因为，如果你执行一个删除操作，你需要确保在释放这些块的事务提交之前，你没有复用任何被释放的块。如果你复用了这些块而此时发生崩溃，那么在最后一个完全提交的事务的末尾，将无法恢复被重新分配的块的内容。一种简单的做法是，只有在释放这些块的事务提交之后，才在内部的内存中块分配结构里把这些块标记为空闲。Ext4 为此使用日志提交回调。)

With journal commit callbacks you can ask the journalling layer to call
a callback function when the transaction is finally committed to disk,
so that you can do some of your own management. You ask the journalling
layer for calling the callback by simply setting
``journal->j_commit_callback`` function pointer and that function is
called after each transaction commit.
(通过日志提交回调，你可以要求日志层在事务最终提交到磁盘时调用一个回调函数，这样你就可以做一些自己的管理工作。你只需设置 ``journal->j_commit_callback`` 函数指针来要求日志层调用该回调，该函数会在每次事务提交之后被调用。)

JBD2 also provides a way to block all transaction updates via
jbd2_journal_lock_updates() /
jbd2_journal_unlock_updates(). Ext4 uses this when it wants a
window with a clean and stable fs for a moment. E.g.
(JBD2 还提供了一种通过 jbd2_journal_lock_updates() / jbd2_journal_unlock_updates() 阻止所有事务更新的方法。当 Ext4 想要拥有一个干净的、稳定的 fs 的短暂窗口时，会使用它。例如：)

::


        jbd2_journal_lock_updates() //stop new stuff happening..
        jbd2_journal_flush()        // checkpoint everything.
        ..do stuff on stable fs
        jbd2_journal_unlock_updates() // carry on with filesystem use.

The opportunities for abuse and DOS attacks with this should be obvious,
if you allow unprivileged userspace to trigger codepaths containing
these calls.
(如果你允许非特权的用户空间触发包含这些调用的代码路径，由此带来的滥用和 DOS 攻击机会应当是显而易见的。)

Fast commits
(快速提交)
~~~~~~~~~~~~

JBD2 to also allows you to perform file-system specific delta commits known as
fast commits. In order to use fast commits, you will need to set following
callbacks that perform corresponding work:
(JBD2 还允许你执行文件系统特有的增量提交，即所谓的快速提交。为了使用快速提交，你需要设置以下执行相应工作的回调：)

`journal->j_fc_cleanup_cb`: Cleanup function called after every full commit and
fast commit.
(`journal->j_fc_cleanup_cb`：在每次完整提交和快速提交之后调用的清理函数。)

`journal->j_fc_replay_cb`: Replay function called for replay of fast commit
blocks.
(`journal->j_fc_replay_cb`：用于重放快速提交块的重放函数。)

File system is free to perform fast commits as and when it wants as long as it
gets permission from JBD2 to do so by calling the function
:c:func:`jbd2_fc_begin_commit()`. Once a fast commit is done, the client
file  system should tell JBD2 about it by calling
:c:func:`jbd2_fc_end_commit()`. If the file system wants JBD2 to perform a full
commit immediately after stopping the fast commit it can do so by calling
:c:func:`jbd2_fc_end_commit_fallback()`. This is useful if fast commit operation
fails for some reason and the only way to guarantee consistency is for JBD2 to
perform the full traditional commit.
(文件系统可以在它想要的时候自由地执行快速提交，只要它通过调用函数 :c:func:`jbd2_fc_begin_commit()` 获得了 JBD2 的许可。一旦快速提交完成，客户端文件系统应通过调用 :c:func:`jbd2_fc_end_commit()` 把这一情况告知 JBD2。如果文件系统希望在停止快速提交之后立即让 JBD2 执行一次完整提交，它可以通过调用 :c:func:`jbd2_fc_end_commit_fallback()` 来做到。如果快速提交操作因某种原因失败，而保证一致性的唯一办法是让 JBD2 执行完整的传统提交，那么这会很有用。)

JBD2 helper functions to manage fast commit buffers. File system can use
:c:func:`jbd2_fc_get_buf()` and :c:func:`jbd2_fc_wait_bufs()` to allocate
and wait on IO completion of fast commit buffers.
(这些是管理快速提交缓冲的 JBD2 辅助函数。文件系统可以使用 :c:func:`jbd2_fc_get_buf()` 和 :c:func:`jbd2_fc_wait_bufs()` 来分配快速提交缓冲并等待其 I/O 完成。)

Currently, only Ext4 implements fast commits. For details of its implementation
of fast commits, please refer to the top level comments in
fs/ext4/fast_commit.c.
(目前，只有 Ext4 实现了快速提交。关于其快速提交实现的细节，请参阅 fs/ext4/fast_commit.c 顶层的注释。)

Summary
(小结)
~~~~~~~

Using the journal is a matter of wrapping the different context changes,
being each mount, each modification (transaction) and each changed
buffer to tell the journalling layer about them.
(使用日志就是把各种不同的上下文变更——每次挂载、每次修改(事务)以及每个被修改的缓冲——包装起来，把这些情况告知日志层。)

Data Types
(数据类型)
----------

The journalling layer uses typedefs to 'hide' the concrete definitions
of the structures used. As a client of the JBD2 layer you can just rely
on the using the pointer as a magic cookie of some sort. Obviously the
hiding is not enforced as this is 'C'.
(日志层使用 typedef 来"隐藏"所用结构体的具体定义。作为 JBD2 层的客户端，你可以只把这些指针当作某种魔法令牌来依赖。显然，这种隐藏并没有被强制执行，因为这是 'C' 语言。)

Structures
(结构体)
~~~~~~~~~~

.. kernel-doc:: include/linux/jbd2.h
   :internal:

Functions
(函数)
---------

The functions here are split into two groups those that affect a journal
as a whole, and those which are used to manage transactions
(这里的函数被分成两组：那些影响日志整体行为的函数，以及那些用于管理事务的函数)

Journal Level
(日志级别)
~~~~~~~~~~~~~

.. kernel-doc:: fs/jbd2/journal.c
   :export:

.. kernel-doc:: fs/jbd2/recovery.c
   :internal:

Transaction Level
(事务级别)
~~~~~~~~~~~~~~~~~~

.. kernel-doc:: fs/jbd2/transaction.c

See also
(另请参阅)
--------

`Journaling the Linux ext2fs Filesystem, LinuxExpo 98, Stephen
Tweedie <http://kernel.org/pub/linux/kernel/people/sct/ext3/journal-design.ps.gz>`__
(《Journaling the Linux ext2fs Filesystem》，LinuxExpo 98，Stephen Tweedie，<http://kernel.org/pub/linux/kernel/people/sct/ext3/journal-design.ps.gz>)

`Ext3 Journalling FileSystem, OLS 2000, Dr. Stephen
Tweedie <http://olstrans.sourceforge.net/release/OLS2000-ext3/OLS2000-ext3.html>`__
(《Ext3 Journalling FileSystem》，OLS 2000，Dr. Stephen Tweedie，<http://olstrans.sourceforge.net/release/OLS2000-ext3/OLS2000-ext3.html>)

