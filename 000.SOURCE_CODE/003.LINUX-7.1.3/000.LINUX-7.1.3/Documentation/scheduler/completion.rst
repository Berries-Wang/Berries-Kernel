==============================================================================
Completions - "wait for completion" barrier APIs(完成量 - "等待完成" 屏障 API)
==============================================================================

Introduction:(引言)
--------------------

If you have one or more threads that must wait for some kernel activity
to have reached a point or a specific state, completions can provide a
race-free solution to this problem. Semantically they are somewhat like a
pthread_barrier() and have similar use-cases.
(如果你有一个或多个线程必须等待某个内核活动达到某一点或某一特定状态，
完成量 (completion) 可以为这个问题提供无竞态的解决方案。从语义上讲，它们
有点类似 pthread_barrier()，并且有相似的用例。)

Completions are a code synchronization mechanism which is preferable to any
misuse of locks/semaphores and busy-loops. Any time you think of using
yield() or some quirky msleep(1) loop to allow something else to proceed,
you probably want to look into using one of the wait_for_completion*()
calls and complete() instead.
(完成量是一种代码同步机制，比任何对锁/信号量的误用和忙等待循环都更可取。
每当你想用 yield() 或某种古怪的 msleep(1) 循环来让其他事情得以进行时，你
多半应该考虑改用 wait_for_completion*() 调用之一和 complete()。)

The advantage of using completions is that they have a well defined, focused
purpose which makes it very easy to see the intent of the code, but they
also result in more efficient code as all threads can continue execution
until the result is actually needed, and both the waiting and the signalling
is highly efficient using low level scheduler sleep/wakeup facilities.
(使用完成量的优点在于它们具有定义良好、目标专注的用途，使代码的意图非常
容易看清；而且它们还能带来更高效的代码，因为所有线程都可以继续执行，直到
真正需要结果为止，并且等待与通知都借助底层调度器的睡眠/唤醒设施高效完成。)

Completions are built on top of the waitqueue and wakeup infrastructure of
the Linux scheduler. The event the threads on the waitqueue are waiting for
is reduced to a simple flag in 'struct completion', appropriately called "done".
(完成量构建在 Linux 调度器的等待队列 (waitqueue) 与唤醒基础设施之上。等待
队列上的线程所等待的事件，被简化为 'struct completion' 中一个恰如其分地
叫做 "done" 的简单标志。)

As completions are scheduling related, the code can be found in
kernel/sched/completion.c.
(由于完成量与调度相关，相关代码位于 kernel/sched/completion.c。)


Usage:(用法)
-------------

There are three main parts to using completions:
(使用完成量有三个主要部分：)

 - the initialization of the 'struct completion' synchronization object
   (初始化 'struct completion' 同步对象)
 - the waiting part through a call to one of the variants of wait_for_completion(),
   (通过调用 wait_for_completion() 的某个变体来进行等待)
 - the signaling side through a call to complete() or complete_all().
   (通过调用 complete() 或 complete_all() 进行通知)

There are also some helper functions for checking the state of completions.
Note that while initialization must happen first, the waiting and signaling
part can happen in any order. I.e. it's entirely normal for a thread
to have marked a completion as 'done' before another thread checks whether
it has to wait for it.
(还有一些用于检查完成量状态的辅助函数。注意，虽然初始化必须最先发生，但
等待与通知可以以任意顺序进行。也就是说，一个线程在另一个线程检查是否需要
等待之前，就已把完成量标记为 'done'，这是完全正常的。)

To use completions you need to #include <linux/completion.h> and
create a static or dynamic variable of type 'struct completion',
which has only two fields::

	struct completion {
		unsigned int done;
		struct swait_queue_head wait;
	};

(要使用完成量，你需要 #include <linux/completion.h>，并创建一个类型为
'struct completion' 的静态或动态变量，它只有两个字段：)

This provides the ->wait waitqueue to place tasks on for waiting (if any), and
the ->done completion flag for indicating whether it's completed or not.
(这提供了 ->wait 等待队列用于放置等待的任务 (如果有)，以及 ->done 完成
标志用于指示是否已经完成。)

Completions should be named to refer to the event that is being synchronized on.
A good example is::

	wait_for_completion(&early_console_added);

	complete(&early_console_added);

(完成量的命名应当指向其所同步的事件。一个很好的例子是：)

Good, intuitive naming (as always) helps code readability. Naming a completion
'complete' is not helpful unless the purpose is super obvious...
(好的、直观的命名 (一如既往) 有助于提高代码可读性。把完成量命名为
'complete' 毫无帮助，除非其用途极其明显……)


Initializing completions:(初始化完成量)
---------------------------------------

Dynamically allocated completion objects should preferably be embedded in data
structures that are assured to be alive for the life-time of the function/driver,
to prevent races with asynchronous complete() calls from occurring.
(动态分配的完成量对象最好嵌入到那些保证在函数/驱动生命周期内一直存活的
数据结构中，以防止与异步 complete() 调用之间发生竞态。)

Particular care should be taken when using the _timeout() or _killable()/_interruptible()
variants of wait_for_completion(), as it must be assured that memory de-allocation
does not happen until all related activities (complete() or reinit_completion())
have taken place, even if these wait functions return prematurely due to a timeout
or a signal triggering.
(使用 wait_for_completion() 的 _timeout() 或
_killable()/_interruptible() 变体时要特别小心，必须确保在所有相关活动
(complete() 或 reinit_completion()) 都发生之前不会释放内存，即使这些等待
函数因超时或信号触发而过早返回。)

Initializing of dynamically allocated completion objects is done via a call to
init_completion()::

	init_completion(&dynamic_object->done);

(动态分配的完成量对象通过调用 init_completion() 来初始化：)

In this call we initialize the waitqueue and set ->done to 0, i.e. "not completed"
or "not done".
(在该调用中，我们初始化等待队列并把 ->done 设置为 0，即 "未完成" 或
"未就绪"。)

The re-initialization function, reinit_completion(), simply resets the
->done field to 0 ("not done"), without touching the waitqueue.
Callers of this function must make sure that there are no racy
wait_for_completion() calls going on in parallel.
(重新初始化函数 reinit_completion() 只是把 ->done 字段重置为
0 ("未完成")，而不触碰等待队列。该函数的调用者必须确保没有并行进行的
wait_for_completion() 竞态调用。)

Calling init_completion() on the same completion object twice is
most likely a bug as it re-initializes the queue to an empty queue and
enqueued tasks could get "lost" - use reinit_completion() in that case,
but be aware of other races.
(对同一个完成量对象调用 init_completion() 两次很可能是 bug，因为它会把
队列重新初始化为空队列，已入队的任务可能会 "丢失" —— 在这种情况下请
改用 reinit_completion()，但也要注意其他竞态。)

For static declaration and initialization, macros are available.
(对于静态声明与初始化，有相应的宏可用。)

For static (or global) declarations in file scope you can use
DECLARE_COMPLETION()::

	static DECLARE_COMPLETION(setup_done);
	DECLARE_COMPLETION(setup_done);

(对于文件作用域的静态 (或全局) 声明，可以使用 DECLARE_COMPLETION()：)

Note that in this case the completion is boot time (or module load time)
initialized to 'not done' and doesn't require an init_completion() call.
(注意，在这种情况下，完成量会在启动时 (或模块加载时) 被初始化为 'not
done' 状态，不需要再调用 init_completion()。)

When a completion is declared as a local variable within a function,
then the initialization should always use DECLARE_COMPLETION_ONSTACK()
explicitly, not just to make lockdep happy, but also to make it clear
that limited scope had been considered and is intentional::

	DECLARE_COMPLETION_ONSTACK(setup_done)

(当完成量被声明为函数内的局部变量时，初始化应当总是显式使用
DECLARE_COMPLETION_ONSTACK()，这不仅是为了让 lockdep 满意，也是为了表明
已经考虑过受限的作用域并且是有意为之：)

Note that when using completion objects as local variables you must be
acutely aware of the short life time of the function stack: the function
must not return to a calling context until all activities (such as waiting
threads) have ceased and the completion object is completely unused.
(注意，当把完成量对象用作局部变量时，你必须敏锐地意识到函数栈的短暂生命
周期：在所有活动 (例如等待中的线程) 都已停止、完成量对象被完全弃用之前，
函数绝不能返回到调用上下文。)

To emphasise this again: in particular when using some of the waiting API variants
with more complex outcomes, such as the timeout or signalling (_timeout(),
_killable() and _interruptible()) variants, the wait might complete
prematurely while the object might still be in use by another thread - and a return
from the wait_on_completion*() caller function will deallocate the function
stack and cause subtle data corruption if a complete() is done in some
other thread. Simple testing might not trigger these kinds of races.
(再强调一次：特别是在使用一些具有更复杂结果的等待 API 变体时，例如超时或
通知类的 (_timeout()、_killable() 和 _interruptible()) 变体，等待可能过早
完成，而对象可能仍被另一个线程使用 —— 此时从 wait_on_completion*() 调用
者函数返回将释放函数栈，如果此时另一个线程进行了 complete() 调用，会导致
隐蔽的数据损坏。简单的测试可能不会触发这类竞态。)

If unsure, use dynamically allocated completion objects, preferably embedded
in some other long lived object that has a boringly long life time which
exceeds the life time of any helper threads using the completion object,
or has a lock or other synchronization mechanism to make sure complete()
is not called on a freed object.
(如果不确定，请使用动态分配的完成量对象，最好把它嵌入到某个生命期长得
乏味、超过任何使用该完成量对象的辅助线程生命期的其他对象中，或者让该对象
拥有锁或其他同步机制，以确保不会对已释放的对象调用 complete()。)

A naive DECLARE_COMPLETION() on the stack triggers a lockdep warning.
(在栈上直接使用朴素的 DECLARE_COMPLETION() 会触发 lockdep 警告。)

Waiting for completions:(等待完成量)
------------------------------------

For a thread to wait for some concurrent activity to finish, it
calls wait_for_completion() on the initialized completion structure::

	void wait_for_completion(struct completion *done)

(线程要等待某个并发活动完成，就在已初始化的完成量结构上调用
wait_for_completion()：)

A typical usage scenario is::

	CPU#1					CPU#2

	struct completion setup_done;

	init_completion(&setup_done);
	initialize_work(...,&setup_done,...);

	/* run non-dependent code */		/* do setup */

	wait_for_completion(&setup_done);	complete(&setup_done);

(一个典型的使用场景是：)

This is not implying any particular order between wait_for_completion() and
the call to complete() - if the call to complete() happened before the call
to wait_for_completion() then the waiting side simply will continue
immediately as all dependencies are satisfied; if not, it will block until
completion is signaled by complete().
(这并不意味着 wait_for_completion() 与 complete() 调用之间存在任何特定的
先后顺序 —— 如果 complete() 的调用发生在 wait_for_completion() 调用之前，
那么等待方会因为所有依赖都已满足而立即继续；否则，它将一直阻塞，直到
完成量被 complete() 通知。)

Note that wait_for_completion() is calling spin_lock_irq()/spin_unlock_irq(),
so it can only be called safely when you know that interrupts are enabled.
Calling it from IRQs-off atomic contexts will result in hard-to-detect
spurious enabling of interrupts.
(注意，wait_for_completion() 会调用 spin_lock_irq()/spin_unlock_irq()，
因此只有在你知道中断处于启用状态时才能安全地调用它。在关 IRQ 的原子
上下文中调用它会导致难以察觉的虚假开中断。)

The default behavior is to wait without a timeout and to mark the task as
uninterruptible. wait_for_completion() and its variants are only safe
in process context (as they can sleep) but not in atomic context,
interrupt context, with disabled IRQs, or preemption is disabled - see also
try_wait_for_completion() below for handling completion in atomic/interrupt
context.
(默认行为是不带超时地等待，并把任务标记为不可中断。wait_for_completion()
及其变体只在进程上下文中安全 (因为它们可以睡眠)，而不能在原子上下文、
中断上下文、IRQ 被禁用或抢占被禁用的情况下使用 —— 关于在原子/中断上下文
中处理完成量，另见下文的 try_wait_for_completion()。)

As all variants of wait_for_completion() can (obviously) block for a long
time depending on the nature of the activity they are waiting for, so in
most cases you probably don't want to call this with held mutexes.
(由于 wait_for_completion() 的所有变体 (显然) 都可能根据所等待活动的性质
而长时间阻塞，所以在大多数情况下，你可能不希望在持有互斥锁的情况下调用
它。)


wait_for_completion*() variants available:(可用的 wait_for_completion*() 变体)
------------------------------------------------------------------------------

The below variants all return status and this status should be checked in
most(/all) cases - in cases where the status is deliberately not checked you
probably want to make a note explaining this (e.g. see
arch/arm/kernel/smp.c:__cpu_up()).
(下述变体都会返回状态，在大多数 (或全部) 情况下都应当检查该状态 —— 在刻意
不检查状态的情况下，你可能想加一条注释说明原因 (例如参见
arch/arm/kernel/smp.c:__cpu_up())。)

A common problem that occurs is to have unclean assignment of return types,
so take care to assign return-values to variables of the proper type.
(一个常见的问题是返回类型赋值不清晰，所以要注意把返回值赋给类型正确的
变量。)

Checking for the specific meaning of return values also has been found
to be quite inaccurate, e.g. constructs like::

	if (!wait_for_completion_interruptible_timeout(...))

(对返回值具体含义的判断也常常被发现很不准确，例如像这样的构造：)

... would execute the same code path for successful completion and for the
interrupted case - which is probably not what you want::

	int wait_for_completion_interruptible(struct completion *done)

(……它会对成功完成与被中断两种情况执行相同的代码路径 —— 这多半不是你
想要的：)

This function marks the task TASK_INTERRUPTIBLE while it is waiting.
If a signal was received while waiting it will return -ERESTARTSYS; 0 otherwise::

	unsigned long wait_for_completion_timeout(struct completion *done, unsigned long timeout)

(该函数在等待期间把任务标记为 TASK_INTERRUPTIBLE。如果在等待期间收到
信号，它将返回 -ERESTARTSYS；否则返回 0：)

The task is marked as TASK_UNINTERRUPTIBLE and will wait at most 'timeout'
jiffies. If a timeout occurs it returns 0, else the remaining time in
jiffies (but at least 1).
(任务被标记为 TASK_UNINTERRUPTIBLE，最多等待 'timeout' 个 jiffies。如果
发生超时则返回 0，否则返回剩余的 jiffies 数 (但至少为 1)。)

Timeouts are preferably calculated with msecs_to_jiffies() or usecs_to_jiffies(),
to make the code largely HZ-invariant.
(超时值最好用 msecs_to_jiffies() 或 usecs_to_jiffies() 来计算，使代码在
很大程度上与 HZ 无关。)

If the returned timeout value is deliberately ignored a comment should probably explain
why (e.g. see drivers/mfd/wm8350-core.c wm8350_read_auxadc())::

	long wait_for_completion_interruptible_timeout(struct completion *done, unsigned long timeout)

(如果刻意忽略返回的超时值，应当用注释说明原因 (例如参见
drivers/mfd/wm8350-core.c 的 wm8350_read_auxadc())：)

This function passes a timeout in jiffies and marks the task as
TASK_INTERRUPTIBLE. If a signal was received it will return -ERESTARTSYS;
otherwise it returns 0 if the completion timed out, or the remaining time in
jiffies if completion occurred.
(该函数传入以 jiffies 为单位的超时值，并把任务标记为
TASK_INTERRUPTIBLE。如果收到信号，它将返回 -ERESTARTSYS；否则，如果完成
量超时则返回 0，如果完成已发生则返回剩余的 jiffies 数。)

Further variants include _killable which uses TASK_KILLABLE as the
designated tasks state and will return -ERESTARTSYS if it is interrupted,
or 0 if completion was achieved.  There is a _timeout variant as well::

	long wait_for_completion_killable(struct completion *done)
	long wait_for_completion_killable_timeout(struct completion *done, unsigned long timeout)

(更多变体包括 _killable，它使用 TASK_KILLABLE 作为指定的任务状态，如果被
中断将返回 -ERESTARTSYS，完成则返回 0。还有一个 _timeout 变体：)

The _io variants wait_for_completion_io() behave the same as the non-_io
variants, except for accounting waiting time as 'waiting on IO', which has
an impact on how the task is accounted in scheduling/IO stats::

	void wait_for_completion_io(struct completion *done)
	unsigned long wait_for_completion_io_timeout(struct completion *done, unsigned long timeout)


(wait_for_completion_io() 的 _io 变体与非 _io 变体行为相同，只是把等待
时间记为 "等待 IO"，这会影响任务在调度/IO 统计中的计算方式：)


Signaling completions:(通知完成量)
----------------------------------

A thread that wants to signal that the conditions for continuation have been
achieved calls complete() to signal exactly one of the waiters that it can
continue::

	void complete(struct completion *done)

(想要通知继续执行的条件已经满足的线程，调用 complete() 来确切地通知其中
一个等待者可以继续：)

... or calls complete_all() to signal all current and future waiters::

	void complete_all(struct completion *done)

(……或者调用 complete_all() 来通知所有当前与未来的等待者：)

The signaling will work as expected even if completions are signaled before
a thread starts waiting. This is achieved by the waiter "consuming"
(decrementing) the done field of 'struct completion'. Waiting threads
wakeup order is the same in which they were enqueued (FIFO order).
(即使完成量在线程开始等待之前就被通知，通知也会按预期工作。这是通过等待
者 "消费" (递减) 'struct completion' 的 done 字段来实现的。等待线程的唤醒
顺序与它们入队的顺序相同 (FIFO 顺序)。)

If complete() is called multiple times then this will allow for that number
of waiters to continue - each call to complete() will simply increment the
done field. Calling complete_all() multiple times is a bug though. Both
complete() and complete_all() can be called in IRQ/atomic context safely.
(如果 complete() 被调用多次，那么相应数量的等待者可以继续 —— 每次
complete() 调用都会简单地递增 done 字段。但多次调用 complete_all() 则是
一个 bug。complete() 与 complete_all() 都可以安全地在 IRQ/原子上下文中
调用。)

There can only be one thread calling complete() or complete_all() on a
particular 'struct completion' at any time - serialized through the wait
queue spinlock. Any such concurrent calls to complete() or complete_all()
probably are a design bug.
(任何时刻只能有一个线程在特定的 'struct completion' 上调用 complete() 或
complete_all() —— 通过等待队列自旋锁进行串行化。任何此类对 complete() 或
complete_all() 的并发调用都很可能是设计上的 bug。)

Signaling completion from IRQ context is fine as it will appropriately
lock with spin_lock_irqsave()/spin_unlock_irqrestore() and it will never
sleep.
(从 IRQ 上下文通知完成量是没有问题的，因为它会恰当地使用
spin_lock_irqsave()/spin_unlock_irqrestore() 加锁，并且从不睡眠。)


try_wait_for_completion()/completion_done():(try_wait_for_completion()/completion_done())
------------------------------------------------------------------------------------------

The try_wait_for_completion() function will not put the thread on the wait
queue but rather returns false if it would need to enqueue (block) the thread,
else it consumes one posted completion and returns true::

	bool try_wait_for_completion(struct completion *done)

(try_wait_for_completion() 函数不会把线程放到等待队列上，而是当它需要把
线程入队 (阻塞) 时返回 false，否则消费一个已通知的完成量并返回 true：)

Finally, to check the state of a completion without changing it in any way,
call completion_done(), which returns false if there are no posted
completions that were not yet consumed by waiters (implying that there are
waiters) and true otherwise::

	bool completion_done(struct completion *done)

(最后，要在不改变完成量状态的前提下检查其状态，可以调用
completion_done()，当没有尚未被等待者消费的已通知完成量时 (这意味着还有
等待者) 它返回 false，否则返回 true：)

Both try_wait_for_completion() and completion_done() are safe to be called in
IRQ or atomic context.
(try_wait_for_completion() 与 completion_done() 都可以安全地在 IRQ 或
原子上下文中调用。)
