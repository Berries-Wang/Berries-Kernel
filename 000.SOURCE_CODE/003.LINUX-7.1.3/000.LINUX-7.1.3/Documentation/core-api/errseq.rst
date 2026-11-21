========================================
The errseq_t datatype(errseq_t 数据类型)
========================================

An errseq_t is a way of recording errors in one place, and allowing any
number of "subscribers" to tell whether it has changed since a previous
point where it was sampled.
(errseq_t 是一种在一处记录错误的机制，并允许任意数量的 "subscribers"（订阅者）
判断自上一次采样以来该值是否发生过变化。)

The initial use case for this is tracking errors for file
synchronization syscalls (fsync, fdatasync, msync and sync_file_range),
but it may be usable in other situations.
(它的初始用途是跟踪文件同步系统调用（fsync、fdatasync、msync 和
sync_file_range）的错误，但也可能适用于其他场景。)

It's implemented as an unsigned 32-bit value.  The low order bits are
designated to hold an error code (between 1 and MAX_ERRNO).  The upper bits
are used as a counter.  This is done with atomics instead of locking so that
these functions can be called from any context.
(它实现为一个无符号 32 位值。低位被指定用于保存错误码（介于 1 和 MAX_ERRNO
之间）。高位用作计数器。这是用原子操作而非加锁来实现的，以便这些函数可以在
任何上下文中调用。)

Note that there is a risk of collisions if new errors are being recorded
frequently, since we have so few bits to use as a counter.
(注意，如果频繁记录新错误，会有冲突的风险，因为可用作计数器的位数太少了。)

To mitigate this, the bit between the error value and counter is used as
a flag to tell whether the value has been sampled since a new value was
recorded.  That allows us to avoid bumping the counter if no one has
sampled it since the last time an error was recorded.
(为了缓解这个问题，错误值与计数器之间的那一位被用作标志，用于表明自记录新值
以来该值是否已被采样过。这样，如果自上次记录错误以来没有人采样过，就可以避免
递增计数器。)

Thus we end up with a value that looks something like this:
(于是我们最终得到的值大致如下所示：)

+--------------------------------------+----+------------------------+
| 31..13                               | 12 | 11..0                  |
+--------------------------------------+----+------------------------+
| counter                              | SF | errno                  |
+--------------------------------------+----+------------------------+

The general idea is for "watchers" to sample an errseq_t value and keep
it as a running cursor.  That value can later be used to tell whether
any new errors have occurred since that sampling was done, and atomically
record the state at the time that it was checked.  This allows us to
record errors in one place, and then have a number of "watchers" that
can tell whether the value has changed since they last checked it.
(其基本思路是让 "watchers"（观察者）采样一个 errseq_t 值并将其保存为运行
游标。该值之后可用于判断自那次采样以来是否发生了新错误，并以原子方式记录
检查时刻的状态。这使我们能够在一处记录错误，然后让多个 "watchers" 判断该值
自它们上次检查以来是否发生过变化。)

A new errseq_t should always be zeroed out.  An errseq_t value of all zeroes
is the special (but common) case where there has never been an error. An all
zero value thus serves as the "epoch" if one wishes to know whether there
has ever been an error set since it was first initialized.
(新的 errseq_t 应始终清零。全零的 errseq_t 值是一种特殊（但常见）的情况，
表示从未发生过错误。因此，如果想知道自最初初始化以来是否设置过错误，
全零值可作为 "epoch"（纪元）。)

API usage(API 用法)
===================

Let me tell you a story about a worker drone.  Now, he's a good worker
overall, but the company is a little...management heavy.  He has to
report to 77 supervisors today, and tomorrow the "big boss" is coming in
from out of town and he's sure to test the poor fellow too.
(让我给你讲一个关于工蜂（worker drone）的故事。总体而言，他是个好员工，
但这家公司有点……管理层过多。他今天要向 77 位主管汇报，而明天从外地赶来的
"big boss"（大老板）也肯定会考问这个可怜的家伙。)

They're all handing him work to do -- so much he can't keep track of who
handed him what, but that's not really a big problem.  The supervisors
just want to know when he's finished all of the work they've handed him so
far and whether he made any mistakes since they last asked.
(他们都在给他分配工作——多到他记不清谁交给了他什么，但这并不是什么大问题。
主管们只想知道，他何时完成到目前为止他们交给他的全部工作，以及自他们上次
询问以来他是否犯了任何错误。)

He might have made the mistake on work they didn't actually hand him,
but he can't keep track of things at that level of detail, all he can
remember is the most recent mistake that he made.
(他可能是在他们并没有实际交给他的工作上犯了错，但他无法在那种细节层面
记住所有事情，他只能记住自己最近犯的那次错误。)

Here's our worker_drone representation::

        struct worker_drone {
                errseq_t        wd_err; /* for recording errors */
        };

(下面是我们的 worker_drone 表示：)

Every day, the worker_drone starts out with a blank slate::

        struct worker_drone wd;

        wd.wd_err = (errseq_t)0;

(每天，worker_drone 都从一张白纸开始：)

The supervisors come in and get an initial read for the day.  They
don't care about anything that happened before their watch begins::

        struct supervisor {
                errseq_t        s_wd_err; /* private "cursor" for wd_err */
                spinlock_t      s_wd_err_lock; /* protects s_wd_err */
        }

        struct supervisor       su;

        su.s_wd_err = errseq_sample(&wd.wd_err);
        spin_lock_init(&su.s_wd_err_lock);

(主管们进来并获取当天的初始读数。他们不关心其监视开始之前发生的任何事情：)

Now they start handing him tasks to do.  Every few minutes they ask him to
finish up all of the work they've handed him so far.  Then they ask him
whether he made any mistakes on any of it::

        spin_lock(&su.su_wd_err_lock);
        err = errseq_check_and_advance(&wd.wd_err, &su.s_wd_err);
        spin_unlock(&su.su_wd_err_lock);

(现在他们开始给他分配任务。每隔几分钟，他们就要求他完成到目前为止他们交给
他的所有工作，然后问他是否在其中任何一项工作上犯了错误：)

Up to this point, that just keeps returning 0.
(到目前为止，这只是不断返回 0。)

Now, the owners of this company are quite miserly and have given him
substandard equipment with which to do his job. Occasionally it
glitches and he makes a mistake.  He sighs a heavy sigh, and marks it
down::

        errseq_set(&wd.wd_err, -EIO);

(如今，这家公司的老板们相当吝啬，给他的工作设备很不达标。设备偶尔出故障，
他就会犯错。他重重地叹了口气，把错误记录下来：)

...and then gets back to work.  The supervisors eventually poll again
and they each get the error when they next check.  Subsequent calls will
return 0, until another error is recorded, at which point it's reported
to each of them once.
(……然后他继续工作。主管们最终会再次轮询，他们每个人在下一次检查时都会得到
该错误。后续调用将返回 0，直到又记录了另一个错误，届时该错误会向他们每个人
报告一次。)

Note that the supervisors can't tell how many mistakes he made, only
whether one was made since they last checked, and the latest value
recorded.
(注意，主管们无法知道他犯了多少次错误，只能知道自他们上次检查以来是否犯过
错误，以及记录的最新值。)

Occasionally the big boss comes in for a spot check and asks the worker
to do a one-off job for him. He's not really watching the worker
full-time like the supervisors, but he does need to know whether a
mistake occurred while his job was processing.
(偶尔，大老板会来进行突击检查，并要求这位员工为他做一件一次性的工作。他并不
像主管们那样全天候监视这个员工，但他确实需要知道在他的工作处理期间是否发生
了错误。)

He can just sample the current errseq_t in the worker, and then use that
to tell whether an error has occurred later::

        errseq_t since = errseq_sample(&wd.wd_err);
        /* submit some work and wait for it to complete */
        err = errseq_check(&wd.wd_err, since);

(他只需采样 worker 中当前的 errseq_t，然后用它来判断之后是否发生了错误：)

Since he's just going to discard "since" after that point, he doesn't
need to advance it here. He also doesn't need any locking since it's
not usable by anyone else.
(由于他在那之后就会丢弃 "since"，所以不需要在这里推进它。他也不需要任何
锁，因为其他任何人都无法使用它。)

Serializing errseq_t cursor updates(串行化 errseq_t 游标更新)
=============================================================

Note that the errseq_t API does not protect the errseq_t cursor during a
check_and_advance_operation. Only the canonical error code is handled
atomically.  In a situation where more than one task might be using the
same errseq_t cursor at the same time, it's important to serialize
updates to that cursor.
(注意，errseq_t API 在 check_and_advance 操作期间不会保护 errseq_t 游标。
只有规范错误码是原子处理的。在可能有多个任务同时使用同一个 errseq_t 游标
的情况下，必须对该游标的更新进行串行化。)

If that's not done, then it's possible for the cursor to go backward
in which case the same error could be reported more than once.
(如果不这样做，游标就有可能倒退，在这种情况下，同一个错误可能会被报告
多次。)

Because of this, it's often advantageous to first do an errseq_check to
see if anything has changed, and only later do an
errseq_check_and_advance after taking the lock. e.g.::

        if (errseq_check(&wd.wd_err, READ_ONCE(su.s_wd_err)) {
                /* su.s_wd_err is protected by s_wd_err_lock */
                spin_lock(&su.s_wd_err_lock);
                err = errseq_check_and_advance(&wd.wd_err, &su.s_wd_err);
                spin_unlock(&su.s_wd_err_lock);
        }

(因此，先执行一次 errseq_check 看看是否有变化，然后在拿到锁之后再执行
errseq_check_and_advance，往往是有利的。例如：)

That avoids the spinlock in the common case where nothing has changed
since the last time it was checked.
(在自上次检查以来没有任何变化的常见情况下，这可以避免使用自旋锁。)

Functions(函数)
===============

.. kernel-doc:: lib/errseq.c
