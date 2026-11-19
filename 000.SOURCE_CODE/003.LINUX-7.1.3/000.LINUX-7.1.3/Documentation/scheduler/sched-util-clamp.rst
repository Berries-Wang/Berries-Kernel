.. SPDX-License-Identifier: GPL-2.0

================================
Utilization Clamping(利用率钳制)
================================

1. Introduction(1. 引言)
========================

Utilization clamping, also known as util clamp or uclamp, is a scheduler
feature that allows user space to help in managing the performance requirement
of tasks. It was introduced in v5.3 release. The CGroup support was merged in
v5.4.
(利用率钳制（utilization clamping），也称为 util clamp 或 uclamp，是一个让用户
空间帮助管理任务性能需求的调度器特性。它在 v5.3 版本中引入；CGroup 支持在 v5.4
中合入。)

Uclamp is a hinting mechanism that allows the scheduler to understand the
performance requirements and restrictions of the tasks, thus it helps the
scheduler to make a better decision. And when schedutil cpufreq governor is
used, util clamp will influence the CPU frequency selection as well.
(Uclamp 是一种提示机制，让调度器能够了解任务的性能需求与限制，从而帮助调度器
做出更好的决策。而当使用 schedutil cpufreq 调频器时，util clamp 还会影响 CPU
频率的选择。)

Since the scheduler and schedutil are both driven by PELT (util_avg) signals,
util clamp acts on that to achieve its goal by clamping the signal to a certain
point; hence the name. That is, by clamping utilization we are making the
system run at a certain performance point.
(由于调度器与 schedutil 都由 PELT（util_avg）信号驱动，util clamp 通过把该信号
钳制到某个点来实现其目标——这正是其名称的由来。也就是说，通过钳制利用率，我们
让系统运行在某个性能点上。)

The right way to view util clamp is as a mechanism to make request or hint on
performance constraints. It consists of two tunables:
(看待 util clamp 的正确方式，是把它视为一种对性能约束提出请求或提示的机制。它
由两个可调项组成：)

        * UCLAMP_MIN, which sets the lower bound.
          (UCLAMP_MIN，设置下界。)
        * UCLAMP_MAX, which sets the upper bound.
          (UCLAMP_MAX，设置上界。)

These two bounds will ensure a task will operate within this performance range
of the system. UCLAMP_MIN implies boosting a task, while UCLAMP_MAX implies
capping a task.
(这两个边界将确保任务在系统的这一性能范围内运行。UCLAMP_MIN 意味着提升（boost）
任务，而 UCLAMP_MAX 意味着封顶（cap）任务。)

One can tell the system (scheduler) that some tasks require a minimum
performance point to operate at to deliver the desired user experience. Or one
can tell the system that some tasks should be restricted from consuming too
much resources and should not go above a specific performance point. Viewing
the uclamp values as performance points rather than utilization is a better
abstraction from user space point of view.
(你可以告诉系统（调度器），某些任务需要一个最低性能点才能提供所需的用户体验；
也可以告诉系统，某些任务应当被限制而不能消耗过多资源、不应超过某个特定性能点。
从用户空间的视角来看，把 uclamp 值看作性能点而非利用率是更好的抽象。)

As an example, a game can use util clamp to form a feedback loop with its
perceived Frames Per Second (FPS). It can dynamically increase the minimum
performance point required by its display pipeline to ensure no frame is
dropped. It can also dynamically 'prime' up these tasks if it knows in the
coming few hundred milliseconds a computationally intensive scene is about to
happen.
(举例来说，一个游戏可以利用 util clamp 与其感知到的每秒帧数（FPS）构成反馈回路。
它可以动态提高其显示管线所需的最低性能点，以确保不丢帧。如果它知道接下来几百毫秒
将出现计算密集的场景，还可以提前把这些任务动态 "预热" 起来。)

On mobile hardware where the capability of the devices varies a lot, this
dynamic feedback loop offers a great flexibility to ensure best user experience
given the capabilities of any system.
(在设备能力差异很大的移动硬件上，这种动态反馈回路提供了极大的灵活性，确保在任意
系统能力下都能获得最佳用户体验。)

Of course a static configuration is possible too. The exact usage will depend
on the system, application and the desired outcome.
(当然也可以采用静态配置。具体用法取决于系统、应用以及期望的结果。)

Another example is in Android where tasks are classified as background,
foreground, top-app, etc. Util clamp can be used to constrain how much
resources background tasks are consuming by capping the performance point they
can run at. This constraint helps reserve resources for important tasks, like
the ones belonging to the currently active app (top-app group). Beside this
helps in limiting how much power they consume. This can be more obvious in
heterogeneous systems (e.g. Arm big.LITTLE); the constraint will help bias the
background tasks to stay on the little cores which will ensure that:
(另一个例子是 Android：任务被分类为 background、foreground、top-app 等。
util clamp 可以通过封顶后台任务可运行的性能点，来约束它们消耗多少资源。这一约束
有助于为重要任务保留资源，例如属于当前活跃应用（top-app 组）的任务；除此之外，
它还有助于限制后台任务消耗多少电量。这在异构系统（例如 Arm big.LITTLE）上更为
明显：该约束会促使后台任务留在小核上，从而确保：)

        1. The big cores are free to run top-app tasks immediately. top-app
           tasks are the tasks the user is currently interacting with, hence
           the most important tasks in the system.
           (大核可以立即腾出来运行 top-app 任务。top-app 任务是用户当前正在交互
           的任务，因此是系统中最重要的任务。)
        2. They don't run on a power hungry core and drain battery even if they
           are CPU intensive tasks.
           (它们不会在费电的核上运行而耗尽电池，即使它们是 CPU 密集型任务。)

.. note::
  **little cores**:
    CPUs with capacity < 1024

  **big cores**:
    CPUs with capacity = 1024

  (**little cores** （小核）：容量 < 1024 的 CPU；
  **big cores** （大核）：容量 = 1024 的 CPU。)

By making these uclamp performance requests, or rather hints, user space can
ensure system resources are used optimally to deliver the best possible user
experience.
(通过提出这些 uclamp 性能请求——或者更确切地说是提示——用户空间可以确保系统
资源得到最优利用，以提供尽可能好的用户体验。)

Another use case is to help with **overcoming the ramp up latency inherit in
how scheduler utilization signal is calculated**.
(另一个用例是帮助克服调度器利用率信号计算方式中固有的爬升延迟。)

On the other hand, a busy task for instance that requires to run at maximum
performance point will suffer a delay of ~200ms (PELT HALFIFE = 32ms) for the
scheduler to realize that. This is known to affect workloads like gaming on
mobile devices where frames will drop due to slow response time to select the
higher frequency required for the tasks to finish their work in time. Setting
UCLAMP_MIN=1024 will ensure such tasks will always see the highest performance
level when they start running.
(另一方面，一个需要以最高性能点运行的繁忙任务，要等约 200ms（PELT 半衰期 = 32ms）
之后调度器才能意识到这一点。众所周知，这会影响移动设备上的游戏等工作负载：由于
选择更高频率的响应时间慢，任务无法及时完成工作，从而导致掉帧。设置
UCLAMP_MIN=1024 将确保这类任务在开始运行时总是能看到最高性能等级。)

The overall visible effect goes beyond better perceived user
experience/performance and stretches to help achieve a better overall
performance/watt if used effectively.
(整体上的可见效果不止于更好的用户体验/性能；如果使用得当，还能帮助实现更好的
整体性能/功耗比。)

User space can form a feedback loop with the thermal subsystem too to ensure
the device doesn't heat up to the point where it will throttle.
(用户空间还可以与热子系统构成反馈回路，确保设备不会热到触发限频（throttle）的
程度。)

Both SCHED_NORMAL/OTHER and SCHED_FIFO/RR honour uclamp requests/hints.
(SCHED_NORMAL/OTHER 与 SCHED_FIFO/RR 都会遵从 uclamp 请求/提示。)

In the SCHED_FIFO/RR case, uclamp gives the option to run RT tasks at any
performance point rather than being tied to MAX frequency all the time. Which
can be useful on general purpose systems that run on battery powered devices.
(在 SCHED_FIFO/RR 的情况下，uclamp 提供了让 RT 任务以任意性能点运行的选项，
而不是始终绑死在最大频率上。这在运行于电池供电设备上的通用系统会很有用。)

Note that by design RT tasks don't have per-task PELT signal and must always
run at a constant frequency to combat undeterministic DVFS rampup delays.
(注意，按照设计，RT 任务没有每任务 PELT 信号，并且必须始终以恒定频率运行，以
对抗不确定的 DVFS 爬升延迟。)

Note that using schedutil always implies a single delay to modify the frequency
when an RT task wakes up. This cost is unchanged by using uclamp. Uclamp only
helps picking what frequency to request instead of schedutil always requesting
MAX for all RT tasks.
(注意，使用 schedutil 总是意味着 RT 任务唤醒时修改频率存在一次延迟。这一代价不
因使用 uclamp 而改变。uclamp 只是帮助选择要请求的频率，而不是让 schedutil 对
所有 RT 任务都请求 MAX。)

See :ref:`section 3.4 <uclamp-default-values>` for default values and
:ref:`3.4.1 <sched-util-clamp-min-rt-default>` on how to change RT tasks
default value.
(默认值参见 :ref:`section 3.4 <uclamp-default-values>`；如何修改 RT 任务默认值
参见 :ref:`3.4.1 <sched-util-clamp-min-rt-default>`。)

2. Design(2. 设计)
==================

Util clamp is a property of every task in the system. It sets the boundaries of
its utilization signal; acting as a bias mechanism that influences certain
decisions within the scheduler.
(Util clamp 是系统中每个任务的一种属性。它设置任务利用率信号的边界，充当影响
调度器内部某些决策的偏置机制。)

The actual utilization signal of a task is never clamped in reality. If you
inspect PELT signals at any point of time you should continue to see them as
they are intact. Clamping happens only when needed, e.g: when a task wakes up
and the scheduler needs to select a suitable CPU for it to run on.
(任务的真正利用率信号实际上从不被钳制。如果你在任何时间点检查 PELT 信号，你看到
的仍然是完好如初的信号。钳制只在需要时发生，例如：当任务唤醒而调度器需要为它
选择一个合适的 CPU 来运行时。)

Since the goal of util clamp is to allow requesting a minimum and maximum
performance point for a task to run on, it must be able to influence the
frequency selection as well as task placement to be most effective. Both of
which have implications on the utilization value at CPU runqueue (rq for short)
level, which brings us to the main design challenge.
(由于 util clamp 的目标是允许为任务请求一个最低和最高性能点，它必须能够同时
影响频率选择与任务放置才能最有效。这两者都会影响 CPU 运行队列（简称 rq）级别上
的利用率值，这就引出了主要的设计挑战。)

When a task wakes up on an rq, the utilization signal of the rq will be
affected by the uclamp settings of all the tasks enqueued on it. For example if
a task requests to run at UTIL_MIN = 512, then the util signal of the rq needs
to respect to this request as well as all other requests from all of the
enqueued tasks.
(当任务在某个 rq 上唤醒时，该 rq 的利用率信号会受到其上所有已入队任务的 uclamp
设置的影响。例如，如果某个任务请求以 UTIL_MIN = 512 运行，那么该 rq 的 util
信号就需要尊重这一请求以及所有其他已入队任务的全部请求。)

To be able to aggregate the util clamp value of all the tasks attached to the
rq, uclamp must do some housekeeping at every enqueue/dequeue, which is the
scheduler hot path. Hence care must be taken since any slow down will have
significant impact on a lot of use cases and could hinder its usability in
practice.
(为了能够聚合挂到 rq 上的所有任务的 util clamp 值，uclamp 必须在每次入队/出队时
做一些簿记工作，而这正是调度器的热路径。因此必须小心，因为任何变慢都会对大量
用例产生显著影响，并可能阻碍其在实践中的可用性。)

The way this is handled is by dividing the utilization range into buckets
(struct uclamp_bucket) which allows us to reduce the search space from every
task on the rq to only a subset of tasks on the top-most bucket.
(处理方式是把利用率范围划分为若干桶（struct uclamp_bucket），从而把搜索空间从
rq 上的每个任务缩小到仅最顶端桶中的任务子集。)

When a task is enqueued, the counter in the matching bucket is incremented,
and on dequeue it is decremented. This makes keeping track of the effective
uclamp value at rq level a lot easier.
(任务入队时，匹配桶中的计数器加一；出队时减一。这使得在 rq 级别跟踪有效 uclamp
值变得容易得多。)

As tasks are enqueued and dequeued, we keep track of the current effective
uclamp value of the rq. See :ref:`section 2.1 <uclamp-buckets>` for details on
how this works.
(随着任务入队与出队，我们持续跟踪 rq 当前的有效 uclamp 值。其工作原理详见
:ref:`section 2.1 <uclamp-buckets>`。)

Later at any path that wants to identify the effective uclamp value of the rq,
it will simply need to read this effective uclamp value of the rq at that exact
moment of time it needs to take a decision.
(之后，任何想要获知 rq 有效 uclamp 值的路径，只需在其需要做决策的那个时刻读取
rq 的这一有效 uclamp 值即可。)

For task placement case, only Energy Aware and Capacity Aware Scheduling
(EAS/CAS) make use of uclamp for now, which implies that it is applied on
heterogeneous systems only.
(对于任务放置，目前只有能量感知与容量感知调度（EAS/CAS）使用 uclamp，这意味着
它只应用于异构系统。当任务唤醒时，调度器会查看每个 rq 当前的有效 uclamp 值，
并与任务若入队该处可能产生的新值进行比较，偏向最终能形成最节能组合的 rq。)
When a task wakes up, the scheduler will look at the current effective uclamp
value of every rq and compare it with the potential new value if the task were
to be enqueued there. Favoring the rq that will end up with the most energy
efficient combination.

Similarly in schedutil, when it needs to make a frequency update it will look
at the current effective uclamp value of the rq which is influenced by the set
of tasks currently enqueued there and select the appropriate frequency that
will satisfy constraints from requests.
(类似地，在 schedutil 中，当它需要更新频率时，会查看 rq 当前的有效 uclamp 值
（该值受当前入队于该 rq 的任务集合影响），并选择能满足各请求约束的合适频率。)

Other paths like setting overutilization state (which effectively disables EAS)
make use of uclamp as well. Such cases are considered necessary housekeeping to
allow the 2 main use cases above and will not be covered in detail here as they
could change with implementation details.
(其他路径（例如设置过度利用状态——它会有效地禁用 EAS）也使用 uclamp。这类情形
被视为支撑上述两个主要用例的必要簿记，此处不再详述，因为它们可能随实现细节而
变化。)

.. _uclamp-buckets:

2.1. Buckets(2.1. 桶)
---------------------

::

                           [struct rq]

  (bottom)                                                    (top)

    0                                                          1024
    |                                                           |
    +-----------+-----------+-----------+----   ----+-----------+
    |  Bucket 0 |  Bucket 1 |  Bucket 2 |    ...    |  Bucket N |
    +-----------+-----------+-----------+----   ----+-----------+
       :           :                                   :
       +- p0       +- p3                               +- p4
       :                                               :
       +- p1                                           +- p5
       :
       +- p2


.. note::
  The diagram above is an illustration rather than a true depiction of the
  internal data structure.

  (上面的图只是示意，并非对内部数据结构的真实描绘。)

To reduce the search space when trying to decide the effective uclamp value of
an rq as tasks are enqueued/dequeued, the whole utilization range is divided
into N buckets where N is configured at compile time by setting
CONFIG_UCLAMP_BUCKETS_COUNT. By default it is set to 5.
(为了在任务入队/出队时减少判定 rq 有效 uclamp 值的搜索空间，整个利用率范围被
划分为 N 个桶，N 在编译时通过设置 CONFIG_UCLAMP_BUCKETS_COUNT 配置，默认为 5。)

The rq has a bucket for each uclamp_id tunables: [UCLAMP_MIN, UCLAMP_MAX].
(rq 为每个 uclamp_id 可调项（[UCLAMP_MIN, UCLAMP_MAX]）都有一个桶。)

The range of each bucket is 1024/N. For example, for the default value of
5 there will be 5 buckets, each of which will cover the following range:
(每个桶的范围是 1024/N。例如，对于默认值 5，将有 5 个桶，每个桶覆盖以下范围：)

::

        DELTA = round_closest(1024/5) = 204.8 = 205

        Bucket 0: [0:204]
        Bucket 1: [205:409]
        Bucket 2: [410:614]
        Bucket 3: [615:819]
        Bucket 4: [820:1024]

When a task p with following tunable parameters

::

        p->uclamp[UCLAMP_MIN] = 300
        p->uclamp[UCLAMP_MAX] = 1024

is enqueued into the rq, bucket 1 will be incremented for UCLAMP_MIN and bucket
4 will be incremented for UCLAMP_MAX to reflect the fact the rq has a task in
this range.
(被入队到 rq 时，UCLAMP_MIN 的桶 1 与 UCLAMP_MAX 的桶 4 会分别加一，以反映该 rq
在此范围内有一个任务的事实。)

The rq then keeps track of its current effective uclamp value for each
uclamp_id.
(随后 rq 会针对每个 uclamp_id 跟踪其当前的有效 uclamp 值。)

When a task p is enqueued, the rq value changes to:
(当任务 p 入队时，rq 的值变为：)

::

        // update bucket logic goes here
        rq->uclamp[UCLAMP_MIN] = max(rq->uclamp[UCLAMP_MIN], p->uclamp[UCLAMP_MIN])
        // repeat for UCLAMP_MAX

Similarly, when p is dequeued the rq value changes to:
(类似地，当 p 出队时，rq 的值变为：)

::

        // update bucket logic goes here
        rq->uclamp[UCLAMP_MIN] = search_top_bucket_for_highest_value()
        // repeat for UCLAMP_MAX

When all buckets are empty, the rq uclamp values are reset to system defaults.
See :ref:`section 3.4 <uclamp-default-values>` for details on default values.
(当所有桶都为空时，rq 的 uclamp 值会被重置为系统默认值。默认值的详情参见
:ref:`section 3.4 <uclamp-default-values>`。)


2.2. Max aggregation(2.2. 最大聚合)
-----------------------------------

Util clamp is tuned to honour the request for the task that requires the
highest performance point.
(Util clamp 经过调校，会遵从需要最高性能点的那个任务的请求。)

When multiple tasks are attached to the same rq, then util clamp must make sure
the task that needs the highest performance point gets it even if there's
another task that doesn't need it or is disallowed from reaching this point.
(当多个任务挂到同一个 rq 上时，util clamp 必须确保需要最高性能点的任务能得到
它，即使存在另一个不需要它、或不允许达到该点的任务。)

For example, if there are multiple tasks attached to an rq with the following
values:
(例如，如果一个 rq 上挂有多个具有以下值的任务：)

::

        p0->uclamp[UCLAMP_MIN] = 300
        p0->uclamp[UCLAMP_MAX] = 900

        p1->uclamp[UCLAMP_MIN] = 500
        p1->uclamp[UCLAMP_MAX] = 500

then assuming both p0 and p1 are enqueued to the same rq, both UCLAMP_MIN
and UCLAMP_MAX become:
(那么假设 p0 与 p1 都入队到同一个 rq，UCLAMP_MIN 与 UCLAMP_MAX 将变为：)

::

        rq->uclamp[UCLAMP_MIN] = max(300, 500) = 500
        rq->uclamp[UCLAMP_MAX] = max(900, 500) = 900

As we shall see in :ref:`section 5.1 <uclamp-capping-fail>`, this max
aggregation is the cause of one of limitations when using util clamp, in
particular for UCLAMP_MAX hint when user space would like to save power.
(正如我们在 :ref:`section 5.1 <uclamp-capping-fail>` 中将看到的，这种最大聚合是
util clamp 局限之一的成因，尤其体现在 UCLAMP_MAX 提示上——当用户空间想要省电时。)

2.3. Hierarchical aggregation(2.3. 层级聚合)
--------------------------------------------

As stated earlier, util clamp is a property of every task in the system. But
the actual applied (effective) value can be influenced by more than just the
request made by the task or another actor on its behalf (middleware library).
(如前所述，util clamp 是系统中每个任务的一种属性。但实际施加的（有效）值所受的
影响，不只是来自任务自身（或代表它的其他主体，如中间件库）提出的请求。)

The effective util clamp value of any task is restricted as follows:
(任何任务的有效 util clamp 值按如下方式受到限制：)

  1. By the uclamp settings defined by the cgroup CPU controller it is attached
     to, if any.
     (受它所挂的 cgroup CPU 控制器定义的 uclamp 设置（如果有的话）限制。)
  2. The restricted value in (1) is then further restricted by the system wide
     uclamp settings.
     (第 (1) 步限制后的值，再进一步受系统级 uclamp 设置的限制。)

:ref:`Section 3 <uclamp-interfaces>` discusses the interfaces and will expand
further on that.
(:ref:`Section 3 <uclamp-interfaces>` 讨论了这些接口，并将进一步展开。)

For now suffice to say that if a task makes a request, its actual effective
value will have to adhere to some restrictions imposed by cgroup and system
wide settings.
(目前只需知道：如果某个任务提出了请求，它的实际有效值将不得不遵守由 cgroup 与
系统级设置施加的某些限制。)

The system will still accept the request even if effectively will be beyond the
constraints, but as soon as the task moves to a different cgroup or a sysadmin
modifies the system settings, the request will be satisfied only if it is
within new constraints.
(即使请求实际上超出约束，系统仍会接受它；但一旦任务移动到不同的 cgroup，或系统
管理员修改了系统设置，请求只有在位于新约束之内时才会被满足。)

In other words, this aggregation will not cause an error when a task changes
its uclamp values, but rather the system may not be able to satisfy requests
based on those factors.
(换句话说，这种聚合不会在任务更改其 uclamp 值时导致错误，而是系统可能无法基于
这些因素满足请求。)

2.4. Range(2.4. 取值范围)
-------------------------

Uclamp performance request has the range of 0 to 1024 inclusive.
(Uclamp 性能请求的取值范围是 0 到 1024（含）。)

For cgroup interface percentage is used (that is 0 to 100 inclusive).
Just like other cgroup interfaces, you can use 'max' instead of 100.
(对于 cgroup 接口，使用百分比（即 0 到 100，含）。与其他 cgroup 接口一样，可以
使用 'max' 代替 100。)

.. _uclamp-interfaces:

3. Interfaces(3. 接口)
======================

3.1. Per task interface(3.1. 每任务接口)
----------------------------------------

sched_setattr() syscall was extended to accept two new fields:
(sched_setattr() 系统调用被扩展为接受两个新字段：)

* sched_util_min: requests the minimum performance point the system should run
  at when this task is running. Or lower performance bound.
  (sched_util_min：请求该任务运行时系统应运行的最低性能点，即性能下界。)
* sched_util_max: requests the maximum performance point the system should run
  at when this task is running. Or upper performance bound.
  (sched_util_max：请求该任务运行时系统应运行的最高性能点，即性能上界。)

For example, the following scenario have 40% to 80% utilization constraints:
(例如，下面的场景具有 40% 到 80% 的利用率约束：)

::

        attr->sched_util_min = 40% * 1024;
        attr->sched_util_max = 80% * 1024;

When task @p is running, **the scheduler should try its best to ensure it
starts at 40% performance level**. If the task runs for a long enough time so
that its actual utilization goes above 80%, the utilization, or performance
level, will be capped.
(当任务 @p 运行时，调度器应尽力确保它以 40% 的性能等级启动。如果任务运行了足够
长的时间，以至于其实际利用率超过 80%，那么利用率（即性能等级）将被封顶。)

The special value -1 is used to reset the uclamp settings to the system
default.
(特殊值 -1 用于把 uclamp 设置重置为系统默认值。)

Note that resetting the uclamp value to system default using -1 is not the same
as manually setting uclamp value to system default. This distinction is
important because as we shall see in system interfaces, the default value for
RT could be changed. SCHED_NORMAL/OTHER might gain similar knobs too in the
future.
(注意，用 -1 把 uclamp 值重置为系统默认值，与手动把 uclamp 值设置为系统默认值并
不相同。这一区别很重要，因为正如我们在系统接口中将要看到的，RT 的默认值是可以
被修改的；SCHED_NORMAL/OTHER 将来也可能获得类似的旋钮。)

3.2. cgroup interface(3.2. cgroup 接口)
---------------------------------------

There are two uclamp related values in the CPU cgroup controller:
(CPU cgroup 控制器中有两个与 uclamp 相关的值：)

* cpu.uclamp.min
  (下限保护)
* cpu.uclamp.max
  (上限限制)

When a task is attached to a CPU controller, its uclamp values will be impacted
as follows:
(当任务挂到某个 CPU 控制器时，其 uclamp 值将受到如下影响：)

* cpu.uclamp.min is a protection as described in :ref:`section 3-3 of cgroup
  v2 documentation <cgroupv2-protections-distributor>`.
  (cpu.uclamp.min 是一种保护（protection），如 :ref:`cgroup v2 文档的第 3-3 节
  <cgroupv2-protections-distributor>` 所述。)

  If a task uclamp_min value is lower than cpu.uclamp.min, then the task will
  inherit the cgroup cpu.uclamp.min value.
  (如果任务的 uclamp_min 值低于 cpu.uclamp.min，那么该任务将继承 cgroup 的
  cpu.uclamp.min 值。)

  In a cgroup hierarchy, effective cpu.uclamp.min is the max of (child,
  parent).
  (在 cgroup 层级中，有效的 cpu.uclamp.min 是（子，父）二者的最大值。)

* cpu.uclamp.max is a limit as described in :ref:`section 3-2 of cgroup v2
  documentation <cgroupv2-limits-distributor>`.
  (cpu.uclamp.max 是一种限制（limit），如 :ref:`cgroup v2 文档的第 3-2 节
  <cgroupv2-limits-distributor>` 所述。)

  If a task uclamp_max value is higher than cpu.uclamp.max, then the task will
  inherit the cgroup cpu.uclamp.max value.
  (如果任务的 uclamp_max 值高于 cpu.uclamp.max，那么该任务将继承 cgroup 的
  cpu.uclamp.max 值。)

  In a cgroup hierarchy, effective cpu.uclamp.max is the min of (child,
  parent).
  (在 cgroup 层级中，有效的 cpu.uclamp.max 是（子，父）二者的最小值。)

For example, given following parameters:
(例如，给定以下参数：)

::

        p0->uclamp[UCLAMP_MIN] = // system default;
        p0->uclamp[UCLAMP_MAX] = // system default;

        p1->uclamp[UCLAMP_MIN] = 40% * 1024;
        p1->uclamp[UCLAMP_MAX] = 50% * 1024;

        cgroup0->cpu.uclamp.min = 20% * 1024;
        cgroup0->cpu.uclamp.max = 60% * 1024;

        cgroup1->cpu.uclamp.min = 60% * 1024;
        cgroup1->cpu.uclamp.max = 100% * 1024;

when p0 and p1 are attached to cgroup0, the values become:
(当 p0 与 p1 挂到 cgroup0 时，值变为：)

::

        p0->uclamp[UCLAMP_MIN] = cgroup0->cpu.uclamp.min = 20% * 1024;
        p0->uclamp[UCLAMP_MAX] = cgroup0->cpu.uclamp.max = 60% * 1024;

        p1->uclamp[UCLAMP_MIN] = 40% * 1024; // intact
        p1->uclamp[UCLAMP_MAX] = 50% * 1024; // intact

when p0 and p1 are attached to cgroup1, these instead become:
(当 p0 与 p1 挂到 cgroup1 时，值则变为：)

::

        p0->uclamp[UCLAMP_MIN] = cgroup1->cpu.uclamp.min = 60% * 1024;
        p0->uclamp[UCLAMP_MAX] = cgroup1->cpu.uclamp.max = 100% * 1024;

        p1->uclamp[UCLAMP_MIN] = cgroup1->cpu.uclamp.min = 60% * 1024;
        p1->uclamp[UCLAMP_MAX] = 50% * 1024; // intact

Note that cgroup interfaces allows cpu.uclamp.max value to be lower than
cpu.uclamp.min. Other interfaces don't allow that.
(注意，cgroup 接口允许 cpu.uclamp.max 的值低于 cpu.uclamp.min。其他接口不允许
这样做。)

3.3. System interface(3.3. 系统接口)
------------------------------------

3.3.1 sched_util_clamp_min(系统级 UCLAMP_MIN 范围限制)
------------------------------------------------------

System wide limit of allowed UCLAMP_MIN range. By default it is set to 1024,
which means that permitted effective UCLAMP_MIN range for tasks is [0:1024].
By changing it to 512 for example the range reduces to [0:512]. This is useful
to restrict how much boosting tasks are allowed to acquire.
(系统级允许的 UCLAMP_MIN 范围限制。默认为 1024，这意味着任务被允许的有效
UCLAMP_MIN 范围是 [0:1024]。例如改为 512 会把范围缩小到 [0:512]。这可用于限制
任务允许获得的提升幅度。)

Requests from tasks to go above this knob value will still succeed, but
they won't be satisfied until it is more than p->uclamp[UCLAMP_MIN].
(任务超过该旋钮值的请求仍会成功，但在它大于 p->uclamp[UCLAMP_MIN] 之前不会被
满足。)

The value must be smaller than or equal to sched_util_clamp_max.
(该值必须小于或等于 sched_util_clamp_max。)

3.3.2 sched_util_clamp_max(系统级 UCLAMP_MAX 范围限制)
------------------------------------------------------

System wide limit of allowed UCLAMP_MAX range. By default it is set to 1024,
which means that permitted effective UCLAMP_MAX range for tasks is [0:1024].
(系统级允许的 UCLAMP_MAX 范围限制。默认为 1024，这意味着任务被允许的有效
UCLAMP_MAX 范围是 [0:1024]。)

By changing it to 512 for example the effective allowed range reduces to
[0:512]. This means is that no task can run above 512, which implies that all
rqs are restricted too. IOW, the whole system is capped to half its performance
capacity.
(例如改为 512 会把有效允许范围缩小到 [0:512]。这意味着没有任何任务能运行在 512
以上，这也意味着所有 rq 都受到限制。换言之，整个系统被封顶在其性能容量的一半。)

This is useful to restrict the overall maximum performance point of the system.
For example, it can be handy to limit performance when running low on battery
or when the system wants to limit access to more energy hungry performance
levels when it's in idle state or screen is off.
(这可用于限制系统的整体最高性能点。例如，在电池电量不足时限制性能，或在系统处于
空闲状态或屏幕关闭时限制对更耗电的性能等级的访问，都会很方便。)

Requests from tasks to go above this knob value will still succeed, but they
won't be satisfied until it is more than p->uclamp[UCLAMP_MAX].
(任务超过该旋钮值的请求仍会成功，但在它大于 p->uclamp[UCLAMP_MAX] 之前不会被
满足。)

The value must be greater than or equal to sched_util_clamp_min.
(该值必须大于或等于 sched_util_clamp_min。)

.. _uclamp-default-values:

3.4. Default values(3.4. 默认值)
--------------------------------

By default all SCHED_NORMAL/SCHED_OTHER tasks are initialized to:
(默认情况下，所有 SCHED_NORMAL/SCHED_OTHER 任务初始化为：)

::

        p_fair->uclamp[UCLAMP_MIN] = 0
        p_fair->uclamp[UCLAMP_MAX] = 1024

That is, by default they're boosted to run at the maximum performance point of
changed at boot or runtime. No argument was made yet as to why we should
provide this, but can be added in the future.
(也就是说，默认情况下它们被提升为以系统的最高性能点运行——该默认值可在启动或
运行时修改。目前尚未论证为什么要提供这一点，但将来可以补充。)

For SCHED_FIFO/SCHED_RR tasks:
(对于 SCHED_FIFO/SCHED_RR 任务：)

::

        p_rt->uclamp[UCLAMP_MIN] = 1024
        p_rt->uclamp[UCLAMP_MAX] = 1024

That is by default they're boosted to run at the maximum performance point of
the system which retains the historical behavior of the RT tasks.
(也就是说，默认情况下它们被提升为以系统的最高性能点运行，这保留了 RT 任务的
历史行为。)

RT tasks default uclamp_min value can be modified at boot or runtime via
sysctl. See below section.
(RT 任务的默认 uclamp_min 值可以在启动时或运行时通过 sysctl 修改。参见下文小节。)

.. _sched-util-clamp-min-rt-default:

3.4.1 sched_util_clamp_min_rt_default(RT 任务默认 UCLAMP_MIN)
-------------------------------------------------------------

Running RT tasks at maximum performance point is expensive on battery powered
devices and not necessary. To allow system developer to offer good performance
guarantees for these tasks without pushing it all the way to maximum
performance point, this sysctl knob allows tuning the best boost value to
address the system requirement without burning power running at maximum
performance point all the time.
(在电池供电设备上让 RT 任务以最高性能点运行代价高昂且没有必要。为了让系统开发者
能够在不必把性能一路推到最高点的情况下，为这些任务提供良好的性能保证，这个 sysctl
旋钮允许调校最佳的提升值，以满足系统需求，而不会因始终运行在最高性能点而浪费
电力。)

Application developer are encouraged to use the per task util clamp interface
to ensure they are performance and power aware. Ideally this knob should be set
to 0 by system designers and leave the task of managing performance
requirements to the apps.
(鼓励应用开发者使用每任务的 util clamp 接口，以确保他们同时关注性能与功耗。
理想情况下，系统设计者应把该旋钮设为 0，把管理性能需求的任务留给应用。)

4. How to use util clamp(4. 如何使用 util clamp)
================================================

Util clamp promotes the concept of user space assisted power and performance
management. At the scheduler level there is no info required to make the best
decision. However, with util clamp user space can hint to the scheduler to make
better decision about task placement and frequency selection.
(Util clamp 推崇用户空间辅助的功耗与性能管理这一概念。在调度器层面，并不存在
做出最佳决策所需的信息。然而借助 util clamp，用户空间可以向调度器提示，使其对
任务放置与频率选择做出更好的决策。)

Best results are achieved by not making any assumptions about the system the
application is running on and to use it in conjunction with a feedback loop to
dynamically monitor and adjust. Ultimately this will allow for a better user
experience at a better perf/watt.
(最佳效果来自：不对应用所运行的系统做任何假设，并将其与反馈回路结合使用，以动态
监控与调整。最终这将带来更好的用户体验与更好的每瓦性能。)

For some systems and use cases, static setup will help to achieve good results.
Portability will be a problem in this case. How much work one can do at 100,
200 or 1024 is different for each system. Unless there's a specific target
system, static setup should be avoided.
(对某些系统和用例而言，静态设置有助于取得良好结果。但这种情况下可移植性会成为
问题。在 100、200 或 1024 下能完成多少工作，每个系统都不同。除非有特定的目标
系统，否则应避免静态设置。)

There are enough possibilities to create a whole framework based on util clamp
or self contained app that makes use of it directly.
(基于 util clamp 构建一个完整框架、或写一个直接使用它的自包含应用，都有足够的
空间发挥。)

4.1. Boost important and DVFS-latency-sensitive tasks(4.1. 提升重要且对 DVFS 延迟敏感的任务)
--------------------------------------------------------------------------------------------

A GUI task might not be busy to warrant driving the frequency high when it
wakes up. However, it requires to finish its work within a specific time window
to deliver the desired user experience. The right frequency it requires at
wakeup will be system dependent. On some underpowered systems it will be high,
on other overpowered ones it will be low or 0.
(一个 GUI 任务在唤醒时可能并不繁忙，不足以推动频率升高。但它需要在特定时间窗口
内完成其工作，以提供所需的用户体验。它唤醒时所需的合适频率取决于系统。在一些
性能不足的系统上会很高，在其他性能过剩的系统上则会很低或为 0。)

This task can increase its UCLAMP_MIN value every time it misses the deadline
to ensure on next wake up it runs at a higher performance point. It should try
to approach the lowest UCLAMP_MIN value that allows to meet its deadline on any
particular system to achieve the best possible perf/watt for that system.
(该任务可以在每次错过截止时间时提高自己的 UCLAMP_MIN 值，以确保下一次唤醒时
运行在更高的性能点。它应尽量逼近能在特定系统上满足其截止时间的最低 UCLAMP_MIN
值，以在该系统上取得尽可能好的每瓦性能。)

On heterogeneous systems, it might be important for this task to run on
a faster CPU.
(在异构系统上，让该任务运行在更快的 CPU 上可能很重要。)

**Generally it is advised to perceive the input as performance level or point
which will imply both task placement and frequency selection**.
(一般来说，建议把输入理解为性能等级或性能点，这将同时隐含任务放置与频率选择。)

4.2. Cap background tasks(4.2. 限制后台任务)
--------------------------------------------

Like explained for Android case in the introduction. Any app can lower
UCLAMP_MAX for some background tasks that don't care about performance but
could end up being busy and consume unnecessary system resources on the system.
(正如引言中 Android 案例所解释的那样。任何应用都可以为某些不关心性能、但可能变得
繁忙并消耗不必要系统资源的后台任务调低 UCLAMP_MAX。)

4.3. Powersave mode(4.3. 省电模式)
----------------------------------

sched_util_clamp_max system wide interface can be used to limit all tasks from
operating at the higher performance points which are usually energy
inefficient.
(sched_util_clamp_max 系统级接口可用于限制所有任务运行在通常能效低下的较高性能
点上。)

This is not unique to uclamp as one can achieve the same by reducing max
frequency of the cpufreq governor. It can be considered a more convenient
alternative interface.
(这并非 uclamp 独有——通过调低 cpufreq 调频器的最大频率也能达到同样效果。它可以
被视为一种更方便的替代接口。)

4.4. Per-app performance restriction(4.4. 每应用性能限制)
---------------------------------------------------------

Middleware/Utility can provide the user an option to set UCLAMP_MIN/MAX for an
app every time it is executed to guarantee a minimum performance point and/or
limit it from draining system power at the cost of reduced performance for
these apps.
(中间件/实用程序可以为用户提供选项：在应用每次执行时为其设置 UCLAMP_MIN/MAX，
以保证最低性能点和/或限制其耗尽系统电力——代价是降低这些应用的性能。)

If you want to prevent your laptop from heating up while on the go from
compiling the kernel and happy to sacrifice performance to save power, but
still would like to keep your browser performance intact, uclamp makes it
possible.
(如果你想防止笔记本在移动途中因编译内核而发热，并且乐于牺牲性能来省电，但仍希望
浏览器的性能不受影响，uclamp 让这成为可能。)

5. Limitations(5. 局限性)
=========================

.. _uclamp-capping-fail:

5.1. Capping frequency with uclamp_max fails under certain conditions(5.1. 某些条件下 uclamp_max 频率钳制会失效)
----------------------------------------------------------------------------------------------------------------

If task p0 is capped to run at 512:
(如果任务 p0 被钳制为最多以 512 运行：)

::

        p0->uclamp[UCLAMP_MAX] = 512

and it shares the rq with p1 which is free to run at any performance point:
(而它与可以以任意性能点自由运行的 p1 共享同一个 rq：)

::

        p1->uclamp[UCLAMP_MAX] = 1024

then due to max aggregation the rq will be allowed to reach max performance
point:
(那么由于最大聚合，该 rq 将被允许达到最高性能点：)

::

        rq->uclamp[UCLAMP_MAX] = max(512, 1024) = 1024

Assuming both p0 and p1 have UCLAMP_MIN = 0, then the frequency selection for
the rq will depend on the actual utilization value of the tasks.
(假设 p0 与 p1 的 UCLAMP_MIN 都为 0，那么该 rq 的频率选择将取决于任务的实际
利用率值。)

If p1 is a small task but p0 is a CPU intensive task, then due to the fact that
both are running at the same rq, p1 will cause the frequency capping to be left
from the rq although p1, which is allowed to run at any performance point,
doesn't actually need to run at that frequency.
(如果 p1 是个小任务而 p0 是 CPU 密集型任务，那么由于二者运行在同一个 rq 上，
p1 会导致频率封顶在该 rq 上失效，尽管被允许以任意性能点运行的 p1 实际上并不
需要以那个频率运行。)

5.2. UCLAMP_MAX can break PELT (util_avg) signal(5.2. UCLAMP_MAX 可能破坏 PELT (util_avg) 信号)
-----------------------------------------------------------------------------------------------

PELT assumes that frequency will always increase as the signals grow to ensure
there's always some idle time on the CPU. But with UCLAMP_MAX, this frequency
increase will be prevented which can lead to no idle time in some
circumstances. When there's no idle time, a task will stuck in a busy loop,
which would result in util_avg being 1024.
(PELT 假设频率会随信号增长而总是提高，以确保 CPU 上总有一些空闲时间。但在
UCLAMP_MAX 之下，这种频率提升会被阻止，在某些情况下可能导致完全没有空闲时间。
没有空闲时间时，任务会困在忙循环中，这将导致 util_avg 达到 1024。)

Combing with issue described below, this can lead to unwanted frequency spikes
when severely capped tasks share the rq with a small non capped task.
(与下文描述的问题叠加，当被严重钳制的任务与小的不受限任务共享 rq 时，可能导致
不希望出现的频率尖峰。)

As an example if task p, which have:
(举例来说，如果具有如下参数的任务 p：)

::

        p0->util_avg = 300
        p0->uclamp[UCLAMP_MAX] = 0

wakes up on an idle CPU, then it will run at min frequency (Fmin) this
CPU is capable of. The max CPU frequency (Fmax) matters here as well,
since it designates the shortest computational time to finish the task's
work on this CPU.
(在一个空闲 CPU 上唤醒，那么它将以该 CPU 所能达到的最低频率（Fmin）运行。CPU 的
最高频率（Fmax）在这里也有影响，因为它标定了在该 CPU 上完成该任务工作所需的
最短计算时间。)

::

        rq->uclamp[UCLAMP_MAX] = 0

If the ratio of Fmax/Fmin is 3, then maximum value will be:
(如果 Fmax/Fmin 的比值为 3，那么最大值将是：)

::

        300 * (Fmax/Fmin) = 900

which indicates the CPU will still see idle time since 900 is < 1024. The
_actual_ util_avg will not be 900 though, but somewhere between 300 and 900. As
long as there's idle time, p->util_avg updates will be off by a some margin,
but not proportional to Fmax/Fmin.
(这表明由于 900 < 1024，CPU 仍会看到空闲时间。不过实际的 util_avg 不会是 900，
而是介于 300 与 900 之间的某个值。只要有空闲时间，p->util_avg 的更新就会带有
一定误差，但不会与 Fmax/Fmin 成比例。)

::

        p0->util_avg = 300 + small_error

Now if the ratio of Fmax/Fmin is 4, the maximum value becomes:
(现在如果 Fmax/Fmin 的比值为 4，最大值变为：)

::

        300 * (Fmax/Fmin) = 1200

which is higher than 1024 and indicates that the CPU has no idle time. When
this happens, then the _actual_ util_avg will become:
(这高于 1024，表明 CPU 没有空闲时间。当这种情况发生时，实际的 util_avg 将变为：)

::

        p0->util_avg = 1024

If task p1 wakes up on this CPU, which have:
(如果任务 p1 在这个 CPU 上唤醒，它具有：)

::

        p1->util_avg = 200
        p1->uclamp[UCLAMP_MAX] = 1024

then the effective UCLAMP_MAX for the CPU will be 1024 according to max
aggregation rule. But since the capped p0 task was running and throttled
severely, then the rq->util_avg will be:
(那么根据最大聚合规则，CPU 的有效 UCLAMP_MAX 将是 1024。但由于被钳制的 p0 任务
一直在运行且被严重限流，rq->util_avg 将是：)

::

        p0->util_avg = 1024
        p1->util_avg = 200

        rq->util_avg = 1024
        rq->uclamp[UCLAMP_MAX] = 1024

Hence lead to a frequency spike since if p0 wasn't throttled we should get:
(因此导致频率尖峰，因为如果 p0 没有被限流，我们本应得到：)

::

        p0->util_avg = 300
        p1->util_avg = 200

        rq->util_avg = 500

and run somewhere near mid performance point of that CPU, not the Fmax we get.
(并运行在该 CPU 中段性能点附近的某处，而不是我们实际得到的 Fmax。)

5.3. Schedutil response time issues(5.3. Schedutil 响应时间问题)
----------------------------------------------------------------

schedutil has three limitations:
(schedutil 有三个局限：)

        1. Hardware takes non-zero time to respond to any frequency change
           request. On some platforms can be in the order of few ms.
           (硬件响应任何频率变更请求都需要非零时间。在某些平台上可达几毫秒的量级。)
        2. Non fast-switch systems require a worker deadline thread to wake up
           and perform the frequency change, which adds measurable overhead.
           (非 fast-switch 系统需要一个 worker deadline 线程唤醒并执行频率变更，
           这会带来可测的开销。)
        3. schedutil rate_limit_us drops any requests during this rate_limit_us
           window.
           (schedutil 的 rate_limit_us 会在 rate_limit_us 窗口期间丢弃任何请求。)

If a relatively small task is doing critical job and requires a certain
performance point when it wakes up and starts running, then all these
limitations will prevent it from getting what it wants in the time scale it
expects.
(如果一个相对较小的任务正在做关键工作，并且在其唤醒并开始运行时需要某个特定的
性能点，那么所有这些局限都会使它无法在它预期的时间尺度内得到想要的东西。)

This limitation is not only impactful when using uclamp, but will be more
prevalent as we no longer gradually ramp up or down. We could easily be
jumping between frequencies depending on the order tasks wake up, and their
respective uclamp values.
(这一局限不仅在使用 uclamp 时有影响；由于我们不再逐渐爬升或下降，它会更加突出。
我们可能轻易地在频率之间跳跃，取决于任务唤醒的顺序及它们各自的 uclamp 值。)

We regard that as a limitation of the capabilities of the underlying system
itself.
(我们认为这是底层系统自身能力上的局限。)

There is room to improve the behavior of schedutil rate_limit_us, but not much
to be done for 1 or 2. They are considered hard limitations of the system.
(schedutil 的 rate_limit_us 行为还有改进空间，但对于第 1 或第 2 点则无能为力。
它们被视为系统的硬性局限。)
