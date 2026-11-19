===================
Workqueue(工作队列)
===================

:Date: September, 2010
(日期: 2010年9月)
:Author: Tejun Heo <tj@kernel.org>
(作者: Tejun Heo <tj@kernel.org>)
:Author: Florian Mickler <florian@mickler.org>
(作者: Florian Mickler <florian@mickler.org>)


Introduction(引言)
==================

There are many cases where an asynchronous process execution context
is needed and the workqueue (wq) API is the most commonly used
mechanism for such cases.
(很多场景都需要异步的进程执行上下文，而 workqueue (工作队列，wq) API 正是这类场景中最常用的机制。)

When such an asynchronous execution context is needed, a work item
describing which function to execute is put on a queue.  An
independent thread serves as the asynchronous execution context.  The
queue is called workqueue and the thread is called worker.
(当需要这样的异步执行上下文时，一个描述要执行哪个函数的工作项 (work item) 被放入队列。一个独立的线程充当异步执行上下文。这个队列称为 workqueue (工作队列)，这个线程称为 worker (工作者)。)

While there are work items on the workqueue the worker executes the
functions associated with the work items one after the other.  When
there is no work item left on the workqueue the worker becomes idle.
When a new work item gets queued, the worker begins executing again.
(只要工作队列上还有工作项，worker 就会逐个执行与工作项关联的函数。当工作队列上没有剩余工作项时，worker 进入空闲状态。一旦有新的工作项入队，worker 又会重新开始执行。)


Why Concurrency Managed Workqueue?(为什么需要并发管理工作队列？)
================================================================

In the original wq implementation, a multi threaded (MT) wq had one
worker thread per CPU and a single threaded (ST) wq had one worker
thread system-wide.  A single MT wq needed to keep around the same
number of workers as the number of CPUs.  The kernel grew a lot of MT
wq users over the years and with the number of CPU cores continuously
rising, some systems saturated the default 32k PID space just booting
up.
(在最初的 wq 实现中，多线程 (MT) wq 为每个 CPU 配一个 worker 线程，而单线程 (ST) wq 全系统只有一个 worker 线程。一个 MT wq 需要保持与 CPU 数量大致相同的 worker 数量。多年来内核积累了大量 MT wq 用户，随着 CPU 核心数量持续增长，有些系统仅仅启动过程就把默认 32k 的 PID 空间消耗殆尽。)

Although MT wq wasted a lot of resource, the level of concurrency
provided was unsatisfactory.  The limitation was common to both ST and
MT wq albeit less severe on MT.  Each wq maintained its own separate
worker pool.  An MT wq could provide only one execution context per CPU
while an ST wq one for the whole system.  Work items had to compete for
those very limited execution contexts leading to various problems
including proneness to deadlocks around the single execution context.
(MT wq 浪费了大量资源，但提供的并发程度却并不令人满意。这一局限为 ST 与 MT wq 所共有，只是在 MT 上稍轻。每个 wq 都维护自己独立的 worker 池。MT wq 每个 CPU 只能提供一个执行上下文，ST wq 则整个系统只有一个。工作项不得不争夺这些极其有限的执行上下文，从而引发各种问题，包括围绕单一执行上下文容易产生死锁。)

The tension between the provided level of concurrency and resource
usage also forced its users to make unnecessary tradeoffs like libata
choosing to use ST wq for polling PIOs and accepting an unnecessary
limitation that no two polling PIOs can progress at the same time.  As
MT wq don't provide much better concurrency, users which require
higher level of concurrency, like async or fscache, had to implement
their own thread pool.
(提供的并发程度与资源占用之间的这种矛盾，也迫使用者做出不必要的取舍：例如 libata 选择把轮询 PIO 放在 ST wq 上，从而接受"两个轮询 PIO 不能同时推进"这一不必要的限制。由于 MT wq 并没有提供好多少的并发能力，需要更高并发程度的用户 (如 async 或 fscache) 只能自行实现线程池。)

Concurrency Managed Workqueue (cmwq) is a reimplementation of wq with
focus on the following goals.
(并发管理工作队列 (Concurrency Managed Workqueue，cmwq) 是 wq 的重新实现，聚焦于以下目标。)

* Maintain compatibility with the original workqueue API.
  (与最初的 workqueue API 保持兼容。)

* Use per-CPU unified worker pools shared by all wq to provide
  flexible level of concurrency on demand without wasting a lot of
  resource.
  (使用由所有 wq 共享的每 CPU 统一 worker 池，在不浪费大量资源的前提下按需提供灵活的并发程度。)

* Automatically regulate worker pool and level of concurrency so that
  the API users don't need to worry about such details.
  (自动调节 worker 池与并发程度，使 API 用户无需为这类细节操心。)


The Design(设计)
================

In order to ease the asynchronous execution of functions a new
abstraction, the work item, is introduced.
(为了简化函数的异步执行，引入了一个新的抽象：工作项 (work item)。)

A work item is a simple struct that holds a pointer to the function
that is to be executed asynchronously.  Whenever a driver or subsystem
wants a function to be executed asynchronously it has to set up a work
item pointing to that function and queue that work item on a
workqueue.
(工作项是一个简单的结构体，保存着指向待异步执行函数的指针。每当驱动或子系统想让某个函数异步执行时，它必须设置一个指向该函数的工作项，并把该工作项放到某个工作队列上。)

A work item can be executed in either a thread or the BH (softirq) context.
(工作项既可以在线程上下文中执行，也可以在 BH (softirq) 上下文中执行。)

For threaded workqueues, special purpose threads, called [k]workers, execute
the functions off of the queue, one after the other. If no work is queued,
the worker threads become idle. These worker threads are managed in
worker-pools.
(对于线程型 workqueue，被称为 [k]worker 的专用线程从队列中逐个取出函数并执行。如果没有工作入队，worker 线程就进入空闲。这些 worker 线程被纳入 worker-pool (worker 池) 中管理。)

The cmwq design differentiates between the user-facing workqueues that
subsystems and drivers queue work items on and the backend mechanism
which manages worker-pools and processes the queued work items.
(cmwq 的设计把面向用户的 workqueue (子系统和驱动把工作项入队到其上) 与管理 worker-pool、处理已入队工作项的后端机制区分开来。)

There are two worker-pools, one for normal work items and the other
for high priority ones, for each possible CPU and some extra
worker-pools to serve work items queued on unbound workqueues - the
number of these backing pools is dynamic.
(对于每个可能的 CPU，都有两个 worker-pool：一个服务于普通工作项，另一个服务于高优先级工作项；此外还有一些额外的 worker-pool，服务于入队到非绑定 (unbound) workqueue 的工作项——这些后备池的数量是动态的。)

BH workqueues use the same framework. However, as there can only be one
concurrent execution context, there's no need to worry about concurrency.
Each per-CPU BH worker pool contains only one pseudo worker which represents
the BH execution context. A BH workqueue can be considered a convenience
interface to softirq.
(BH workqueue 使用同一框架。不过，由于并发执行上下文只能有一个，无需操心并发问题。每个每 CPU BH worker 池只包含一个伪 (pseudo) worker，代表 BH 执行上下文。BH workqueue 可以视为 softirq 的一个便利接口。)

Subsystems and drivers can create and queue work items through special
workqueue API functions as they see fit. They can influence some
aspects of the way the work items are executed by setting flags on the
workqueue they are putting the work item on. These flags include
things like CPU locality, concurrency limits, priority and more.  To
get a detailed overview refer to the API description of
``alloc_workqueue()`` below.
(子系统和驱动可以按需通过专门的 workqueue API 函数创建并入队工作项。它们可以通过在放置工作项的 workqueue 上设置标志，影响工作项执行方式的某些方面。这些标志包括 CPU 亲和性 (locality)、并发限制、优先级等等。要详细了解，请参阅下文 ``alloc_workqueue()`` 的 API 描述。)

When a work item is queued to a workqueue, the target worker-pool is
determined according to the queue parameters and workqueue attributes
and appended on the shared worklist of the worker-pool.  For example,
unless specifically overridden, a work item of a bound workqueue will
be queued on the worklist of either normal or highpri worker-pool that
is associated to the CPU the issuer is running on.
(当一个工作项被入队到某个 workqueue 时，目标 worker-pool 会根据入队参数和 workqueue 属性来确定，然后追加到该 worker-pool 的共享工作列表 (worklist) 上。例如，除非被专门覆盖，绑定 (bound) workqueue 的工作项会被入队到与发行者 (issuer) 正在运行的 CPU 相关联的 normal 或 highpri worker-pool 的工作列表上。)

For any thread pool implementation, managing the concurrency level
(how many execution contexts are active) is an important issue.  cmwq
tries to keep the concurrency at a minimal but sufficient level.
Minimal to save resources and sufficient in that the system is used at
its full capacity.
(对任何线程池实现而言，管理并发级别 (有多少执行上下文处于活动状态) 都是一个重要问题。cmwq 尽量把并发保持在最小但足够的水平：最小是为了节省资源，足够是指系统能力得到充分利用。)

Each worker-pool bound to an actual CPU implements concurrency
management by hooking into the scheduler.  The worker-pool is notified
whenever an active worker wakes up or sleeps and keeps track of the
number of the currently runnable workers.  Generally, work items are
not expected to hog a CPU and consume many cycles.  That means
maintaining just enough concurrency to prevent work processing from
stalling should be optimal.  As long as there are one or more runnable
workers on the CPU, the worker-pool doesn't start execution of a new
work, but, when the last running worker goes to sleep, it immediately
schedules a new worker so that the CPU doesn't sit idle while there
are pending work items.  This allows using a minimal number of workers
without losing execution bandwidth.
(每个绑定到实际 CPU 的 worker-pool 通过挂接调度器来实现并发管理。每当活动 worker 醒来或睡眠时，worker-pool 都会收到通知，并跟踪当前可运行 worker 的数量。一般而言，工作项不应霸占 CPU、消耗大量周期。这意味着只要维持刚好足够的并发、避免工作处理停滞就是最优的。只要 CPU 上有一个或多个可运行的 worker，worker-pool 就不会开始执行新工作；而当最后一个运行中的 worker 睡眠时，它会立即调度一个新 worker，使 CPU 在还有待处理工作项时不至于闲置。这样就能以最少数量的 worker 运作而不损失执行带宽。)

Keeping idle workers around doesn't cost other than the memory space
for kthreads, so cmwq holds onto idle ones for a while before killing
them.
(保留空闲 worker 除了占用 kthread 的内存空间外并无其他开销，因此 cmwq 会把空闲 worker 保留一段时间再销毁它们。)

For unbound workqueues, the number of backing pools is dynamic.
Unbound workqueue can be assigned custom attributes using
``apply_workqueue_attrs()`` and workqueue will automatically create
backing worker pools matching the attributes.  The responsibility of
regulating concurrency level is on the users.  There is also a flag to
mark a bound wq to ignore the concurrency management.  Please refer to
the API section for details.
(对于非绑定 workqueue，后备池的数量是动态的。可以使用 ``apply_workqueue_attrs()`` 为非绑定 workqueue 指定自定义属性，workqueue 会自动创建与属性匹配的后备 worker 池。调节并发级别的责任落在用户身上。还有一个标志可用来标记某个绑定的 wq 忽略并发管理。详情请参阅 API 一节。)

Forward progress guarantee relies on that workers can be created when
more execution contexts are necessary, which in turn is guaranteed
through the use of rescue workers.  All work items which might be used
on code paths that handle memory reclaim are required to be queued on
wq's that have a rescue-worker reserved for execution under memory
pressure.  Else it is possible that the worker-pool deadlocks waiting
for execution contexts to free up.
(前向进展保证依赖于"在需要更多执行上下文时能够创建 worker"，而这又通过使用救援 (rescue) worker 来保证。所有可能在处理内存回收的代码路径上使用的工作项，都必须入队到拥有常驻救援 worker、可在内存压力下执行的 wq 上。否则 worker-pool 可能因等待执行上下文释放而死锁。)


Application Programming Interface (API)(应用程序编程接口 (API))
===============================================================

``alloc_workqueue()`` allocates a wq.  The original
``create_*workqueue()`` functions are deprecated and scheduled for
removal.  ``alloc_workqueue()`` takes three arguments - ``@name``,
``@flags`` and ``@max_active``.  ``@name`` is the name of the wq and
also used as the name of the rescuer thread if there is one.
(``alloc_workqueue()`` 分配一个 wq。最初的 ``create_*workqueue()`` 函数已被弃用并计划移除。 ``alloc_workqueue()`` 接受三个参数—— ``@name`` 、 ``@flags`` 和 ``@max_active`` 。 ``@name`` 是 wq 的名字，如果存在救援线程，也用作救援线程的名字。)

A wq no longer manages execution resources but serves as a domain for
forward progress guarantee, flush and work item attributes. ``@flags``
and ``@max_active`` control how work items are assigned execution
resources, scheduled and executed.
(wq 不再管理执行资源，而是充当前向进展保证、flush 与工作项属性的一个域 (domain)。 ``@flags`` 和 ``@max_active`` 控制如何为工作项分配执行资源、如何调度和执行。)


``flags`` (标志)
----------------

``WQ_BH``
  BH workqueues can be considered a convenience interface to softirq. BH
  workqueues are always per-CPU and all BH work items are executed in the
  queueing CPU's softirq context in the queueing order.
  (BH workqueue 可视为 softirq 的便利接口。BH workqueue 总是每 CPU 的，所有 BH 工作项都在入队 CPU 的 softirq 上下文中按入队顺序执行。)

  All BH workqueues must have 0 ``max_active`` and ``WQ_HIGHPRI`` is the
  only allowed additional flag.
  (所有 BH workqueue 的 ``max_active`` 必须为 0，而 ``WQ_HIGHPRI`` 是唯一允许附加的标志。)

  BH work items cannot sleep. All other features such as delayed queueing,
  flushing and canceling are supported.
  (BH 工作项不能睡眠。延迟入队、flush 和取消等其他特性均受支持。)

``WQ_PERCPU``
  Work items queued to a per-cpu wq are bound to a specific CPU.
  This flag is the right choice when cpu locality is important.
  (入队到每 CPU wq 的工作项绑定到特定 CPU。当 CPU 亲和性很重要时，此标志是正确选择。)

  This flag is the complement of ``WQ_UNBOUND``.
  (此标志是 ``WQ_UNBOUND`` 的补集。)

``WQ_UNBOUND``
  Work items queued to an unbound wq are served by the special
  worker-pools which host workers which are not bound to any
  specific CPU.  This makes the wq behave as a simple execution
  context provider without concurrency management.  The unbound
  worker-pools try to start execution of work items as soon as
  possible.  Unbound wq sacrifices locality but is useful for
  the following cases.
  (入队到非绑定 wq 的工作项由专门的 worker-pool 服务，这些池中的 worker 不绑定到任何特定 CPU。这使 wq 表现为一个简单的执行上下文提供者，没有并发管理。非绑定 worker-pool 会尽可能快地开始执行工作项。非绑定 wq 牺牲了亲和性，但在以下情况下有用：)

  * Wide fluctuation in the concurrency level requirement is
    expected and using bound wq may end up creating large number
    of mostly unused workers across different CPUs as the issuer
    hops through different CPUs.
    (预期并发级别需求会大幅波动，而使用绑定 wq 会随着发行者在不同 CPU 之间跳转，在各 CPU 上创建大量基本闲置的 worker。)

  * Long running CPU intensive workloads which can be better
    managed by the system scheduler.
    (长时间运行的 CPU 密集型负载，由系统调度器管理更合适。)

``WQ_FREEZABLE``
  A freezable wq participates in the freeze phase of the system
  suspend operations.  Work items on the wq are drained and no
  new work item starts execution until thawed.
  (可冻结 (freezable) 的 wq 参与系统挂起操作的冻结阶段。wq 上的工作项会被排空，且在解冻之前不会有新的工作项开始执行。)

``WQ_MEM_RECLAIM``
  All wq which might be used in the memory reclaim paths **MUST**
  have this flag set.  The wq is guaranteed to have at least one
  execution context regardless of memory pressure.
  (所有可能用于内存回收路径的 wq **必须** 设置此标志。该 wq 保证无论内存压力如何都至少拥有一个执行上下文。)

``WQ_HIGHPRI``
  Work items of a highpri wq are queued to the highpri
  worker-pool of the target cpu.  Highpri worker-pools are
  served by worker threads with elevated nice level.
  (高优先级 (highpri) wq 的工作项被入队到目标 CPU 的 highpri worker-pool。highpri worker-pool 由 nice 值被调高的 worker 线程服务。)

  Note that normal and highpri worker-pools don't interact with
  each other.  Each maintains its separate pool of workers and
  implements concurrency management among its workers.
  (注意，normal 与 highpri worker-pool 互不交互。各自维护独立的 worker 池，并在自己的 worker 之间实现并发管理。)

``WQ_CPU_INTENSIVE``
  Work items of a CPU intensive wq do not contribute to the
  concurrency level.  In other words, runnable CPU intensive
  work items will not prevent other work items in the same
  worker-pool from starting execution.  This is useful for bound
  work items which are expected to hog CPU cycles so that their
  execution is regulated by the system scheduler.
  (CPU 密集型 wq 的工作项不计入并发级别。换句话说，可运行的 CPU 密集型工作项不会阻止同一 worker-pool 中其他工作项开始执行。这对预期会霸占 CPU 周期的绑定工作项很有用——让它们的执行交给系统调度器去调节。)

  Although CPU intensive work items don't contribute to the
  concurrency level, start of their executions is still
  regulated by the concurrency management and runnable
  non-CPU-intensive work items can delay execution of CPU
  intensive work items.
  (虽然 CPU 密集型工作项不计入并发级别，但其执行的启动仍受并发管理调节，而且可运行的非 CPU 密集型工作项可能延迟 CPU 密集型工作项的执行。)

  This flag is meaningless for unbound wq.
  (此标志对非绑定 wq 无意义。)


``max_active`` (最大活跃数)
---------------------------

``@max_active`` determines the maximum number of execution contexts per
CPU which can be assigned to the work items of a wq. For example, with
``@max_active`` of 16, at most 16 work items of the wq can be executing
at the same time per CPU. This is always a per-CPU attribute, even for
unbound workqueues.
(``@max_active`` 决定每个 CPU 可分配给某个 wq 工作项的最大执行上下文数量。例如 ``@max_active`` 为 16 时，该 wq 的每个 CPU 上最多可同时执行 16 个工作项。这始终是每 CPU 属性，即使对非绑定 workqueue 也是如此。)

The maximum limit for ``@max_active`` is 2048 and the default value used
when 0 is specified is 1024. These values are chosen sufficiently high
such that they are not the limiting factor while providing protection in
runaway cases.
(``@max_active`` 的上限是 2048，指定 0 时使用的默认值为 1024。这些值被选得足够高，使其不会成为限制因素，同时在失控情形下仍能提供保护。)

The number of active work items of a wq is usually regulated by the
users of the wq, more specifically, by how many work items the users
may queue at the same time.  Unless there is a specific need for
throttling the number of active work items, specifying '0' is
recommended.
(wq 的活跃工作项数量通常由 wq 的用户调节，更具体地说，由用户可以同时入队多少工作项来决定。除非有专门限制活跃工作项数量的需要，否则建议指定 '0'。)

Some users depend on strict execution ordering where only one work item
is in flight at any given time and the work items are processed in
queueing order. While the combination of ``@max_active`` of 1 and
``WQ_UNBOUND`` used to achieve this behavior, this is no longer the
case. Use alloc_ordered_workqueue() instead.
(有些用户依赖严格的执行顺序：任一时刻只有一个工作项在处理中，且工作项按入队顺序处理。虽然 ``@max_active`` 为 1 与 ``WQ_UNBOUND`` 的组合曾经可以实现这种行为，但现在已经不行了。请改用 alloc_ordered_workqueue()。)


Example Execution Scenarios(示例执行场景)
=========================================

The following example execution scenarios try to illustrate how cmwq
behave under different configurations.
(下面的示例执行场景试图说明 cmwq 在不同配置下的行为。)

 Work items w0, w1, w2 are queued to a bound wq q0 on the same CPU.
 w0 burns CPU for 5ms then sleeps for 10ms then burns CPU for 5ms
 again before finishing.  w1 and w2 burn CPU for 5ms then sleep for
 10ms.
 (工作项 w0、w1、w2 在同一 CPU 上被入队到绑定的 wq q0。w0 消耗 CPU 5ms，然后睡眠 10ms，接着再消耗 CPU 5ms 后结束。w1 和 w2 各消耗 CPU 5ms，然后睡眠 10ms。)

Ignoring all other tasks, works and processing overhead, and assuming
simple FIFO scheduling, the following is one highly simplified version
of possible sequences of events with the original wq. ::

 TIME IN MSECS	EVENT
 0		w0 starts and burns CPU
 5		w0 sleeps
 15		w0 wakes up and burns CPU
 20		w0 finishes
 20		w1 starts and burns CPU
 25		w1 sleeps
 35		w1 wakes up and finishes
 35		w2 starts and burns CPU
 40		w2 sleeps
 50		w2 wakes up and finishes

(忽略所有其他任务、工作与处理开销，并假设简单的 FIFO 调度，下面是使用最初的 wq 时可能事件序列的一个高度简化版本。)

And with cmwq with ``@max_active`` >= 3, ::

 TIME IN MSECS	EVENT
 0		w0 starts and burns CPU
 5		w0 sleeps
 5		w1 starts and burns CPU
 10		w1 sleeps
 10		w2 starts and burns CPU
 15		w2 sleeps
 15		w0 wakes up and burns CPU
 20		w0 finishes
 20		w1 wakes up and finishes
 25		w2 wakes up and finishes

(而当使用 ``@max_active`` >= 3 的 cmwq 时，情形如下。)

If ``@max_active`` == 2, ::

 TIME IN MSECS	EVENT
 0		w0 starts and burns CPU
 5		w0 sleeps
 5		w1 starts and burns CPU
 10		w1 sleeps
 15		w0 wakes up and burns CPU
 20		w0 finishes
 20		w1 wakes up and finishes
 20		w2 starts and burns CPU
 25		w2 sleeps
 35		w2 wakes up and finishes

(如果 ``@max_active`` == 2，情形如下。)

Now, let's assume w1 and w2 are queued to a different wq q1 which has
``WQ_CPU_INTENSIVE`` set, ::

 TIME IN MSECS	EVENT
 0		w0 starts and burns CPU
 5		w0 sleeps
 5		w1 and w2 start and burn CPU
 10		w1 sleeps
 15		w2 sleeps
 15		w0 wakes up and burns CPU
 20		w0 finishes
 20		w1 wakes up and finishes
 25		w2 wakes up and finishes

(现在，假设 w1 和 w2 被入队到设置了 ``WQ_CPU_INTENSIVE`` 的另一个 wq q1，情形如下。)


Guidelines(准则)
================

* Do not forget to use ``WQ_MEM_RECLAIM`` if a wq may process work
  items which are used during memory reclaim.  Each wq with
  ``WQ_MEM_RECLAIM`` set has an execution context reserved for it.  If
  there is dependency among multiple work items used during memory
  reclaim, they should be queued to separate wq each with
  ``WQ_MEM_RECLAIM``.
  (如果某个 wq 可能处理在内存回收期间用到的工作项，不要忘记使用 ``WQ_MEM_RECLAIM`` 。设置了 ``WQ_MEM_RECLAIM`` 的每个 wq 都有一个为其保留的执行上下文。如果内存回收期间用到的多个工作项之间存在依赖，它们应分别入队到各自设置了 ``WQ_MEM_RECLAIM`` 的独立 wq 上。)

* Unless strict ordering is required, there is no need to use ST wq.
  (除非要求严格顺序，否则没有必要使用 ST wq。)

* Unless there is a specific need, using 0 for @max_active is
  recommended.  In most use cases, concurrency level usually stays
  well under the default limit.
  (除非有特定需要，建议把 @max_active 设为 0。在大多数用例中，并发级别通常远低于默认上限。)

* A wq serves as a domain for forward progress guarantee
  (``WQ_MEM_RECLAIM``, flush and work item attributes.  Work items
  which are not involved in memory reclaim and don't need to be
  flushed as a part of a group of work items, and don't require any
  special attribute, can use one of the system wq.  There is no
  difference in execution characteristics between using a dedicated wq
  and a system wq.
  (wq 充当前向进展保证 ( ``WQ_MEM_RECLAIM`` )、flush 与工作项属性方面的域。不涉及内存回收、不需要作为一组工作项的一部分被 flush、也不要求任何特殊属性的工作项，可以使用某个系统 wq。使用专用 wq 与系统 wq 在执行特性上没有差别。)

  Note: If something may generate more than @max_active outstanding
  work items (do stress test your producers), it may saturate a system
  wq and potentially lead to deadlock. It should utilize its own
  dedicated workqueue rather than the system wq.
  (注意：如果某个组件可能产生超过 @max_active 的未完成工作项 (请务必对你的生产者做压力测试)，它可能占满一个系统 wq 并潜在导致死锁。它应使用自己的专用 workqueue，而不是系统 wq。)

* Unless work items are expected to consume a huge amount of CPU
  cycles, using a bound wq is usually beneficial due to the increased
  level of locality in wq operations and work item execution.
  (除非预期工作项要消耗大量 CPU 周期，否则使用绑定的 wq 通常更有利，因为 wq 操作与工作项执行中的亲和程度更高。)


Affinity Scopes(亲和性作用域)
=============================

An unbound workqueue groups CPUs according to its affinity scope to improve
cache locality. For example, if a workqueue is using the default affinity
scope of "cache_shard", it will group CPUs into sub-LLC shards. A work item
queued on the workqueue will be assigned to a worker on one of the CPUs
within the same shard as the issuing CPU.
Once started, the worker may or may not be allowed to move outside the scope
depending on the ``affinity_strict`` setting of the scope.
(非绑定 workqueue 按其亲和性作用域 (affinity scope) 对 CPU 分组以改善缓存亲和性。例如，如果某个 workqueue 使用默认亲和性作用域 "cache_shard"，它会把 CPU 分组为 sub-LLC 分片 (shard)。入队到该 workqueue 的工作项将被分配给一个 worker，该 worker 位于与发行 CPU 同一分片内的某个 CPU 上。一旦启动，worker 是否被允许移出作用域，取决于该作用域的 ``affinity_strict`` 设置。)

Workqueue currently supports the following affinity scopes.
(workqueue 目前支持以下亲和性作用域。)

``default``
  Use the scope in module parameter ``workqueue.default_affinity_scope``
  which is always set to one of the scopes below.
  (使用模块参数 ``workqueue.default_affinity_scope`` 中设置的作用域，该参数总是被设置为下列作用域之一。)

``cpu``
  CPUs are not grouped. A work item issued on one CPU is processed by a
  worker on the same CPU. This makes unbound workqueues behave as per-cpu
  workqueues without concurrency management.
  (不对 CPU 分组。在某个 CPU 上发出工作项由同一 CPU 上的 worker 处理。这使非绑定 workqueue 表现为没有并发管理的每 CPU workqueue。)

``smt``
  CPUs are grouped according to SMT boundaries. This usually means that the
  logical threads of each physical CPU core are grouped together.
  (按 SMT 边界对 CPU 分组。通常意味着每个物理 CPU 核心的逻辑线程被分在一起。)

``cache``
  CPUs are grouped according to cache boundaries. Which specific cache
  boundary is used is determined by the arch code. L3 is used in a lot of
  cases.
  (按缓存边界对 CPU 分组。具体使用哪个缓存边界由架构代码决定。很多情况下使用 L3。)

``cache_shard``
  CPUs are grouped into sub-LLC shards of at most ``wq_cache_shard_size``
  cores (default 8, tunable via the ``workqueue.cache_shard_size`` boot
  parameter). Shards are always split on core (SMT group) boundaries.
  This is the default affinity scope.
  (把 CPU 分组为最多 ``wq_cache_shard_size`` 个核心的 sub-LLC 分片 (默认 8，可通过 ``workqueue.cache_shard_size`` 启动参数调整)。分片总是按核心 (SMT 组) 边界切分。这是默认的亲和性作用域。)

``numa``
  CPUs are grouped according to NUMA boundaries.
  (按 NUMA 边界对 CPU 分组。)

``system``
  All CPUs are put in the same group. Workqueue makes no effort to process a
  work item on a CPU close to the issuing CPU.
  (所有 CPU 被放入同一组。workqueue 不努力让工作项在靠近发出它的 CPU 上处理。)

The default affinity scope can be changed with the module parameter
``workqueue.default_affinity_scope`` and a specific workqueue's affinity
scope can be changed using ``apply_workqueue_attrs()``.
(默认亲和性作用域可以用模块参数 ``workqueue.default_affinity_scope`` 更改；特定 workqueue 的亲和性作用域可以用 ``apply_workqueue_attrs()`` 更改。)

If ``WQ_SYSFS`` is set, the workqueue will have the following affinity scope
related interface files under its ``/sys/devices/virtual/workqueue/WQ_NAME/``
directory.
(如果设置了 ``WQ_SYSFS``，workqueue 会在其 ``/sys/devices/virtual/workqueue/WQ_NAME/`` 目录下拥有以下与亲和性作用域相关的接口文件。)

``affinity_scope``
  Read to see the current affinity scope. Write to change.
  (读取可查看当前亲和性作用域，写入可更改。)

  When default is the current scope, reading this file will also show the
  current effective scope in parentheses, for example, ``default (cache)``.
  (当 default 是当前作用域时，读取该文件还会以括号显示当前实际生效的作用域，例如 ``default (cache)`` 。)

``affinity_strict``
  0 by default indicating that affinity scopes are not strict. When a work
  item starts execution, workqueue makes a best-effort attempt to ensure
  that the worker is inside its affinity scope, which is called
  repatriation. Once started, the scheduler is free to move the worker
  anywhere in the system as it sees fit. This enables benefiting from scope
  locality while still being able to utilize other CPUs if necessary and
  available.
  (默认为 0，表示亲和性作用域不是严格的。当一个工作项开始执行时，workqueue 会尽力确保 worker 位于其亲和性作用域内，这称为遣返 (repatriation)。启动之后，调度器可以自行把 worker 移动到系统中的任何位置。这样既能从作用域亲和性中受益，又能在需要且有可用 CPU 时利用其他 CPU。)

  If set to 1, all workers of the scope are guaranteed always to be in the
  scope. This may be useful when crossing affinity scopes has other
  implications, for example, in terms of power consumption or workload
  isolation. Strict NUMA scope can also be used to match the workqueue
  behavior of older kernels.
  (若设置为 1，则保证该作用域的所有 worker 始终位于作用域内。当跨越亲和性作用域还有其他影响 (例如功耗或负载隔离方面) 时，这可能有用。严格的 NUMA 作用域还可用于匹配旧内核的 workqueue 行为。)


Affinity Scopes and Performance(亲和性作用域与性能)
===================================================

It'd be ideal if an unbound workqueue's behavior is optimal for vast
majority of use cases without further tuning. Unfortunately, in the current
kernel, there exists a pronounced trade-off between locality and utilization
necessitating explicit configurations when workqueues are heavily used.
(理想情况是非绑定 workqueue 无需进一步调优就可在绝大多数用例中表现最优。遗憾的是，在当前内核中，亲和性与利用率之间存在明显的取舍，在重度使用 workqueue 时需要显式配置。)

Higher locality leads to higher efficiency where more work is performed for
the same number of consumed CPU cycles. However, higher locality may also
cause lower overall system utilization if the work items are not spread
enough across the affinity scopes by the issuers. The following performance
testing with dm-crypt clearly illustrates this trade-off.
(更高的亲和性带来更高的效率——同样数量的 CPU 周期能完成更多工作。然而，如果发行者没有把工作项足够分散到各亲和性作用域中，更高的亲和性也可能造成整体系统利用率下降。下面用 dm-crypt 进行的性能测试清楚地展示了这一取舍。)

The tests are run on a CPU with 12-cores/24-threads split across four L3
caches (AMD Ryzen 9 3900x). CPU clock boost is turned off for consistency.
``/dev/dm-0`` is a dm-crypt device created on NVME SSD (Samsung 990 PRO) and
opened with ``cryptsetup`` with default settings.
(测试在一颗 12 核/24 线程、划分到四个 L3 缓存的 CPU (AMD Ryzen 9 3900x) 上进行。为保证一致性，CPU 时钟加速已关闭。 ``/dev/dm-0`` 是在 NVME SSD (Samsung 990 PRO) 上创建、以默认设置用 ``cryptsetup`` 打开的 dm-crypt 设备。)


Scenario 1: Enough issuers and work spread across the machine(场景 1：发行者与工作遍布全机，数量充足)
-----------------------------------------------------------------------------------------------------

The command used: ::

  $ fio --filename=/dev/dm-0 --direct=1 --rw=randrw --bs=32k --ioengine=libaio \
    --iodepth=64 --runtime=60 --numjobs=24 --time_based --group_reporting \
    --name=iops-test-job --verify=sha512

(所用命令如下。)

There are 24 issuers, each issuing 64 IOs concurrently. ``--verify=sha512``
makes ``fio`` generate and read back the content each time which makes
execution locality matter between the issuer and ``kcryptd``. The following
are the read bandwidths and CPU utilizations depending on different affinity
scope settings on ``kcryptd`` measured over five runs. Bandwidths are in
MiBps, and CPU util in percents.
(共有 24 个发行者，每个并发发出 64 个 IO。 ``--verify=sha512`` 使 ``fio`` 每次都生成并回读内容，这使得发行者与 ``kcryptd`` 之间的执行亲和性变得重要。下表是在 ``kcryptd`` 上设置不同亲和性作用域时，五次运行测得的读带宽与 CPU 占用率。带宽单位为 MiBps，CPU 占用率单位为百分比。)

.. list-table::
   :widths: 16 20 20
   :header-rows: 1

   * - Affinity(亲和性)
     - Bandwidth (MiBps)(带宽)
     - CPU util (%)(CPU 占用率)

   * - system
     - 1159.40 ±1.34
     - 99.31 ±0.02

   * - cache
     - 1166.40 ±0.89
     - 99.34 ±0.01

   * - cache (strict)
     - 1166.00 ±0.71
     - 99.35 ±0.01

With enough issuers spread across the system, there is no downside to
"cache", strict or otherwise. All three configurations saturate the whole
machine but the cache-affine ones outperform by 0.6% thanks to improved
locality.
(当发行者数量充足且遍布整个系统时，"cache" (无论严格与否) 没有坏处。三种配置都能让整机饱和，但由于亲和性改善，cache 亲和的配置高出 0.6%。)


Scenario 2: Fewer issuers, enough work for saturation(场景 2：更少的发行者，足以饱和的工作量)
---------------------------------------------------------------------------------------------

The command used: ::

  $ fio --filename=/dev/dm-0 --direct=1 --rw=randrw --bs=32k \
    --ioengine=libaio --iodepth=64 --runtime=60 --numjobs=8 \
    --time_based --group_reporting --name=iops-test-job --verify=sha512

(所用命令如下。)

The only difference from the previous scenario is ``--numjobs=8``. There are
a third of the issuers but is still enough total work to saturate the
system.
(与上一场景唯一的区别是 ``--numjobs=8``。发行者只有三分之一，但总工作量仍足以使系统饱和。)

.. list-table::
   :widths: 16 20 20
   :header-rows: 1

   * - Affinity(亲和性)
     - Bandwidth (MiBps)(带宽)
     - CPU util (%)(CPU 占用率)

   * - system
     - 1155.40 ±0.89
     - 97.41 ±0.05

   * - cache
     - 1154.40 ±1.14
     - 96.15 ±0.09

   * - cache (strict)
     - 1112.00 ±4.64
     - 93.26 ±0.35

This is more than enough work to saturate the system. Both "system" and
"cache" are nearly saturating the machine but not fully. "cache" is using
less CPU but the better efficiency puts it at the same bandwidth as
"system".
(这一工作量远超系统饱和所需。"system" 与 "cache" 都接近但未完全使整机饱和。"cache" 使用的 CPU 更少，但更高的效率使它与 "system" 达到相同带宽。)

Eight issuers moving around over four L3 cache scope still allow "cache
(strict)" to mostly saturate the machine but the loss of work conservation
is now starting to hurt with 3.7% bandwidth loss.
(八个发行者在四个 L3 缓存作用域间游走，"cache (strict)" 仍能大体使整机饱和，但工作保全性 (work conservation) 的损失开始造成伤害：带宽损失 3.7%。)


Scenario 3: Even fewer issuers, not enough work to saturate(场景 3：更少的发行者，不足以饱和)
---------------------------------------------------------------------------------------------

The command used: ::

  $ fio --filename=/dev/dm-0 --direct=1 --rw=randrw --bs=32k \
    --ioengine=libaio --iodepth=64 --runtime=60 --numjobs=4 \
    --time_based --group_reporting --name=iops-test-job --verify=sha512

(所用命令如下。)

Again, the only difference is ``--numjobs=4``. With the number of issuers
reduced to four, there now isn't enough work to saturate the whole system
and the bandwidth becomes dependent on completion latencies.
(同样，唯一的区别是 ``--numjobs=4``。发行者减少到四个后，工作已不足以使整个系统饱和，带宽变得依赖于完成延迟。)

.. list-table::
   :widths: 16 20 20
   :header-rows: 1

   * - Affinity(亲和性)
     - Bandwidth (MiBps)(带宽)
     - CPU util (%)(CPU 占用率)

   * - system
     - 993.60 ±1.82
     - 75.49 ±0.06

   * - cache
     - 973.40 ±1.52
     - 74.90 ±0.07

   * - cache (strict)
     - 828.20 ±4.49
     - 66.84 ±0.29

Now, the tradeoff between locality and utilization is clearer. "cache" shows
2% bandwidth loss compared to "system" and "cache (struct)" whopping 20%.
(此时，亲和性与利用率之间的取舍更加清晰。"cache" 相比 "system" 有 2% 的带宽损失，而 "cache (struct)" 损失高达 20%。)


Conclusion and Recommendations(结论与建议)
------------------------------------------

In the above experiments, the efficiency advantage of the "cache" affinity
scope over "system" is, while consistent and noticeable, small. However, the
impact is dependent on the distances between the scopes and may be more
pronounced in processors with more complex topologies.
(在上述实验中，"cache" 亲和性作用域相对 "system" 的效率优势虽然一致且可见，但幅度很小。不过，其影响取决于作用域之间的距离，在拓扑更复杂的处理器上可能更明显。)

While the loss of work-conservation in certain scenarios hurts, it is a lot
better than "cache (strict)" and maximizing workqueue utilization is
unlikely to be the common case anyway. As such, "cache" is the default
affinity scope for unbound pools.
(虽然某些场景下工作保全性的损失会造成伤害，但它比 "cache (strict)" 好得多，而且最大化 workqueue 利用率本来就不太可能是常见情形。因此，"cache" 是非绑定池的默认亲和性作用域。)

* As there is no one option which is great for most cases, workqueue usages
  that may consume a significant amount of CPU are recommended to configure
  the workqueues using ``apply_workqueue_attrs()`` and/or enable
  ``WQ_SYSFS``.
  (由于没有一个选项在大多数情况下都表现出色，建议可能消耗大量 CPU 的 workqueue 使用者用 ``apply_workqueue_attrs()`` 配置 workqueue，和/或启用 ``WQ_SYSFS``。)

* An unbound workqueue with strict "cpu" affinity scope behaves the same as
  ``WQ_CPU_INTENSIVE`` per-cpu workqueue. There is no real advanage to the
  latter and an unbound workqueue provides a lot more flexibility.
  (带有严格 "cpu" 亲和性作用域的非绑定 workqueue，其行为与 ``WQ_CPU_INTENSIVE`` 的每 CPU workqueue 相同。后者并无真正优势，而非绑定 workqueue 提供了更多的灵活性。)

* Affinity scopes are introduced in Linux v6.5. To emulate the previous
  behavior, use strict "numa" affinity scope.
  (亲和性作用域在 Linux v6.5 中引入。要模拟之前的行为，请使用严格的 "numa" 亲和性作用域。)

* The loss of work-conservation in non-strict affinity scopes is likely
  originating from the scheduler. There is no theoretical reason why the
  kernel wouldn't be able to do the right thing and maintain
  work-conservation in most cases. As such, it is possible that future
  scheduler improvements may make most of these tunables unnecessary.
  (非严格亲和性作用域下的工作保全性损失很可能源自调度器。从理论上讲，内核没有理由不能在大多数情况下做出正确行为并维持工作保全性。因此，未来的调度器改进可能会使这些可调项中的大部分变得不再必要。)


Examining Configuration(查看配置)
=================================

Use tools/workqueue/wq_dump.py to examine unbound CPU affinity
configuration, worker pools and how workqueues map to the pools: ::

  $ tools/workqueue/wq_dump.py
  Affinity Scopes
  ===============
  wq_unbound_cpumask=0000000f

  CPU
    nr_pods  4
    pod_cpus [0]=00000001 [1]=00000002 [2]=00000004 [3]=00000008
    pod_node [0]=0 [1]=0 [2]=1 [3]=1
    cpu_pod  [0]=0 [1]=1 [2]=2 [3]=3

  SMT
    nr_pods  4
    pod_cpus [0]=00000001 [1]=00000002 [2]=00000004 [3]=00000008
    pod_node [0]=0 [1]=0 [2]=1 [3]=1
    cpu_pod  [0]=0 [1]=1 [2]=2 [3]=3

  CACHE (default)
    nr_pods  2
    pod_cpus [0]=00000003 [1]=0000000c
    pod_node [0]=0 [1]=1
    cpu_pod  [0]=0 [1]=0 [2]=1 [3]=1

  NUMA
    nr_pods  2
    pod_cpus [0]=00000003 [1]=0000000c
    pod_node [0]=0 [1]=1
    cpu_pod  [0]=0 [1]=0 [2]=1 [3]=1

  SYSTEM
    nr_pods  1
    pod_cpus [0]=0000000f
    pod_node [0]=-1
    cpu_pod  [0]=0 [1]=0 [2]=0 [3]=0

  Worker Pools
  ============
  pool[00] ref= 1 nice=  0 idle/workers=  4/  4 cpu=  0
  pool[01] ref= 1 nice=-20 idle/workers=  2/  2 cpu=  0
  pool[02] ref= 1 nice=  0 idle/workers=  4/  4 cpu=  1
  pool[03] ref= 1 nice=-20 idle/workers=  2/  2 cpu=  1
  pool[04] ref= 1 nice=  0 idle/workers=  4/  4 cpu=  2
  pool[05] ref= 1 nice=-20 idle/workers=  2/  2 cpu=  2
  pool[06] ref= 1 nice=  0 idle/workers=  3/  3 cpu=  3
  pool[07] ref= 1 nice=-20 idle/workers=  2/  2 cpu=  3
  pool[08] ref=42 nice=  0 idle/workers=  6/  6 cpus=0000000f
  pool[09] ref=28 nice=  0 idle/workers=  3/  3 cpus=00000003
  pool[10] ref=28 nice=  0 idle/workers= 17/ 17 cpus=0000000c
  pool[11] ref= 1 nice=-20 idle/workers=  1/  1 cpus=0000000f
  pool[12] ref= 2 nice=-20 idle/workers=  1/  1 cpus=00000003
  pool[13] ref= 2 nice=-20 idle/workers=  1/  1 cpus=0000000c

  Workqueue CPU -> pool
  =====================
  [    workqueue \ CPU              0  1  2  3 dfl]
  events                   percpu   0  2  4  6
  events_highpri           percpu   1  3  5  7
  events_long              percpu   0  2  4  6
  events_unbound           unbound  9  9 10 10  8
  events_freezable         percpu   0  2  4  6
  events_power_efficient   percpu   0  2  4  6
  events_freezable_pwr_ef  percpu   0  2  4  6
  rcu_gp                   percpu   0  2  4  6
  rcu_par_gp               percpu   0  2  4  6
  slub_flushwq             percpu   0  2  4  6
  netns                    ordered  8  8  8  8  8
  ...

(使用 tools/workqueue/wq_dump.py 查看非绑定 CPU 亲和性配置、worker 池以及 workqueue 到池的映射关系。)

See the command's help message for more info.
(更多信息请参阅该命令的帮助信息。)


Monitoring(监视)
================

Use tools/workqueue/wq_monitor.py to monitor workqueue operations: ::

  $ tools/workqueue/wq_monitor.py events
                              total  infl  CPUtime  CPUhog CMW/RPR  mayday rescued
  events                      18545     0      6.1       0       5       -       -
  events_highpri                  8     0      0.0       0       0       -       -
  events_long                     3     0      0.0       0       0       -       -
  events_unbound              38306     0      0.1       -       7       -       -
  events_freezable                0     0      0.0       0       0       -       -
  events_power_efficient      29598     0      0.2       0       0       -       -
  events_freezable_pwr_ef        10     0      0.0       0       0       -       -
  sock_diag_events                0     0      0.0       0       0       -       -

                              total  infl  CPUtime  CPUhog CMW/RPR  mayday rescued
  events                      18548     0      6.1       0       5       -       -
  events_highpri                  8     0      0.0       0       0       -       -
  events_long                     3     0      0.0       0       0       -       -
  events_unbound              38322     0      0.1       -       7       -       -
  events_freezable                0     0      0.0       0       0       -       -
  events_power_efficient      29603     0      0.2       0       0       -       -
  events_freezable_pwr_ef        10     0      0.0       0       0       -       -
  sock_diag_events                0     0      0.0       0       0       -       -

  ...

(使用 tools/workqueue/wq_monitor.py 监视 workqueue 操作。)

See the command's help message for more info.
(更多信息请参阅该命令的帮助信息。)


Debugging(调试)
===============

Because the work functions are executed by generic worker threads
there are a few tricks needed to shed some light on misbehaving
workqueue users.
(由于工作函数由通用的 worker 线程执行，要弄清行为不端的 workqueue 用户还需要一些技巧。)

Worker threads show up in the process list as: ::

  root      5671  0.0  0.0      0     0 ?        S    12:07   0:00 [kworker/0:1]
  root      5672  0.0  0.0      0     0 ?        S    12:07   0:00 [kworker/1:2]
  root      5673  0.0  0.0      0     0 ?        S    12:12   0:00 [kworker/0:0]
  root      5674  0.0  0.0      0     0 ?        S    12:13   0:00 [kworker/1:0]

(worker 线程在进程列表中显示为：)

If kworkers are going crazy (using too much cpu), there are two types
of possible problems:
(如果 kworker 行为异常 (占用过多 CPU)，可能的问题有两类：)

	1. Something being scheduled in rapid succession
	(1. 某个东西在快速连续地被调度)
	2. A single work item that consumes lots of cpu cycles
	(2. 单个工作项消耗大量 CPU 周期)

The first one can be tracked using tracing: ::

	$ echo workqueue:workqueue_queue_work > /sys/kernel/tracing/set_event
	$ cat /sys/kernel/tracing/trace_pipe > out.txt
	(wait a few secs)
	^C

(第一类问题可以用跟踪手段排查：)

If something is busy looping on work queueing, it would be dominating
the output and the offender can be determined with the work item
function.
(如果有东西在忙于循环提交工作队列任务，它会霸占输出，可以通过工作项函数确定肇事者。)

For the second type of problems it should be possible to just check
the stack trace of the offending worker thread. ::

	$ cat /proc/THE_OFFENDING_KWORKER/stack

(对于第二类问题，直接查看肇事 worker 线程的栈回溯即可。)

The work item's function should be trivially visible in the stack
trace.
(工作项的函数在栈回溯中应当一目了然。)


Non-reentrance Conditions(不可重入条件)
=======================================

Workqueue guarantees that a work item cannot be re-entrant if the following
conditions hold after a work item gets queued:
(如果一个工作项入队后以下条件成立，workqueue 保证该工作项不会重入：)

        1. The work function hasn't been changed.
        (1. 工作函数没有被更改。)
        2. No one queues the work item to another workqueue.
        (2. 没有人把该工作项入队到另一个 workqueue。)
        3. The work item hasn't been reinitiated.
        (3. 该工作项没有被重新初始化。)

In other words, if the above conditions hold, the work item is guaranteed to be
executed by at most one worker system-wide at any given time.
(换句话说，如果上述条件成立，保证在任一时刻全系统最多只有一个 worker 在执行该工作项。)

Note that requeuing the work item (to the same queue) in the self function
doesn't break these conditions, so it's safe to do. Otherwise, caution is
required when breaking the conditions inside a work function.
(注意，在工作函数自身内部把工作项重新入队 (到同一队列) 不会破坏这些条件，因此这样做是安全的。否则，在工作函数内部破坏这些条件时需要谨慎。)


Kernel Inline Documentations Reference(内核内嵌文档参考)
========================================================

.. kernel-doc:: include/linux/workqueue.h

.. kernel-doc:: kernel/workqueue.c
