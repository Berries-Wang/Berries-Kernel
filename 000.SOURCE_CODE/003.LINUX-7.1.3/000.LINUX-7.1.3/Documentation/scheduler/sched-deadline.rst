===========================================
Deadline Task Scheduling(Deadline 任务调度)
===========================================

.. CONTENTS

    0. WARNING
    1. Overview
    2. Scheduling algorithm
      2.1 Main algorithm
      2.2 Bandwidth reclaiming
    3. Scheduling Real-Time Tasks
      3.1 Definitions
      3.2 Schedulability Analysis for Uniprocessor Systems
      3.3 Schedulability Analysis for Multiprocessor Systems
      3.4 Relationship with SCHED_DEADLINE Parameters
    4. Bandwidth management
      4.1 System-wide settings
      4.2 Task interface
      4.3 Default behavior
      4.4 Behavior of sched_yield()
    5. Tasks CPU affinity
      5.1 Using cgroup v1 cpuset controller
      5.2 Using cgroup v2 cpuset controller
    6. Future plans
    A. Test suite
    B. Minimal main()


0. WARNING(0. 警告)
===================

 Fiddling with these settings can result in an unpredictable or even unstable
 system behavior. As for -rt (group) scheduling, it is assumed that root users
 know what they're doing.
 (乱动这些设置可能导致不可预测甚至不稳定的系统行为。与 -rt (组)调度一样，这里假定 root 用户清楚自己在做什么。)


1. Overview(1. 概述)
====================

 The SCHED_DEADLINE policy contained inside the sched_dl scheduling class is
 basically an implementation of the Earliest Deadline First (EDF) scheduling
 algorithm, augmented with a mechanism (called Constant Bandwidth Server, CBS)
 that makes it possible to isolate the behavior of tasks between each other.
 (sched_dl 调度类中所包含的 SCHED_DEADLINE 策略，基本上是最早截止期优先 (Earliest Deadline First, EDF) 调度算法的一种实现，并辅以一个称为恒定带宽服务器 (Constant Bandwidth Server, CBS) 的机制，使各任务之间的行为可以相互隔离。)


2. Scheduling algorithm(2. 调度算法)
====================================

2.1 Main algorithm(2.1 主算法)
------------------------------

 SCHED_DEADLINE [18] uses three parameters, named "runtime", "period", and
 "deadline", to schedule tasks. A SCHED_DEADLINE task should receive
 "runtime" microseconds of execution time every "period" microseconds, and
 these "runtime" microseconds are available within "deadline" microseconds
 from the beginning of the period.  In order to implement this behavior,
 every time the task wakes up, the scheduler computes a "scheduling deadline"
 consistent with the guarantee (using the CBS[2,3] algorithm). Tasks are then
 scheduled using EDF[1] on these scheduling deadlines (the task with the
 earliest scheduling deadline is selected for execution). Notice that the
 task actually receives "runtime" time units within "deadline" if a proper
 "admission control" strategy (see Section "4. Bandwidth management") is used
 (clearly, if the system is overloaded this guarantee cannot be respected).
 (SCHED_DEADLINE [18] 使用三个参数来调度任务，分别名为 "runtime"、"period" 和 "deadline"。一个 SCHED_DEADLINE 任务应当每 "period" 微秒获得 "runtime" 微秒的执行时间，并且这 "runtime" 微秒必须在周期开始后的 "deadline" 微秒之内用完。为实现这一行为，任务每次唤醒时，调度器都会计算一个与上述保证相一致的 "调度截止期" (scheduling deadline)(使用 CBS[2,3] 算法)。随后任务按这些调度截止期用 EDF[1] 调度(调度截止期最早的任务被选中执行)。注意，如果使用了恰当的 "准入控制" (admission control) 策略(见 "4. Bandwidth management" 一节)，任务就能真正在 "deadline" 之内获得 "runtime" 个时间单位(显然，如果系统过载，这一保证就无法兑现)。)

 Summing up, the CBS[2,3] algorithm assigns scheduling deadlines to tasks so
 that each task runs for at most its runtime every period, avoiding any
 interference between different tasks (bandwidth isolation), while the EDF[1]
 algorithm selects the task with the earliest scheduling deadline as the one
 to be executed next. Thanks to this feature, tasks that do not strictly comply
 with the "traditional" real-time task model (see Section 3) can effectively
 use the new policy.
 (概括地说，CBS[2,3] 算法为任务分配调度截止期，使每个任务在每个周期内最多运行其 runtime 那么久，避免不同任务之间的任何相互干扰(带宽隔离)；而 EDF[1] 算法则选择调度截止期最早的任务作为下一个执行者。得益于这一特性，并不严格遵循 "传统" 实时任务模型的任务(见第 3 节)也能有效使用这一新策略。)

 In more details, the CBS algorithm assigns scheduling deadlines to
 tasks in the following way:
 (更具体地说，CBS 算法按如下方式为任务分配调度截止期：)

  - Each SCHED_DEADLINE task is characterized by the "runtime",
    "deadline", and "period" parameters;
    (每个 SCHED_DEADLINE 任务都由 "runtime"、"deadline" 和 "period" 三个参数刻画；)

  - The state of the task is described by a "scheduling deadline", and
    a "remaining runtime". These two parameters are initially set to 0;
    (任务的状态由一个 "调度截止期" 和一个 "剩余运行时间 (remaining runtime)" 描述。这两个参数初始时都设为 0；)

  - When a SCHED_DEADLINE task wakes up (becomes ready for execution),
    the scheduler checks if::

                 remaining runtime                  runtime
        ----------------------------------    >    ---------
        scheduling deadline - current time           period

    (当 SCHED_DEADLINE 任务唤醒(变为可执行)时，调度器检查上面的不等式是否成立。)

    then, if the scheduling deadline is smaller than the current time, or
    this condition is verified, the scheduling deadline and the
    remaining runtime are re-initialized as
    (然后，如果调度截止期小于当前时间，或者上述条件成立，则调度截止期和剩余运行时间按上面的式子重新初始化。)

         scheduling deadline = current time + deadline
         remaining runtime = runtime

    otherwise, the scheduling deadline and the remaining runtime are
    left unchanged;
    (否则，调度截止期和剩余运行时间保持不变；)

  - When a SCHED_DEADLINE task executes for an amount of time t, its
    remaining runtime is decreased as::

         remaining runtime = remaining runtime - t

    (当 SCHED_DEADLINE 任务执行了时长 t，其剩余运行时间按上面的式子递减：)

    (technically, the runtime is decreased at every tick, or when the
    task is descheduled / preempted);
    (从技术上讲，runtime 在每个时钟滴答 (tick) 时递减，或在任务被取消调度/抢占时递减)；)

  - When the remaining runtime becomes less or equal than 0, the task is
    said to be "throttled" (also known as "depleted" in real-time literature)
    and cannot be scheduled until its scheduling deadline. The "replenishment
    time" for this task (see next item) is set to be equal to the current
    value of the scheduling deadline;
    (当剩余运行时间变得小于等于 0 时，称该任务被 "节流" (throttled)(实时文献中也称 "耗尽" (depleted))，并且在其调度截止期之前不能再被调度。该任务的 "补给时间 (replenishment time)"(见下一项)被设置为等于调度截止期当前的值；)

  - When the current time is equal to the replenishment time of a
    throttled task, the scheduling deadline and the remaining runtime are
    updated as::

         scheduling deadline = scheduling deadline + period
         remaining runtime = remaining runtime + runtime

    (当当前时间到达被节流任务的补给时间时，调度截止期和剩余运行时间按上面的式子更新。)

 The SCHED_FLAG_DL_OVERRUN flag in sched_attr's sched_flags field allows a task
 to get informed about runtime overruns through the delivery of SIGXCPU
 signals.
 (sched_attr 的 sched_flags 字段中的 SCHED_FLAG_DL_OVERRUN 标志允许任务通过接收 SIGXCPU 信号来获知 runtime 超限的情况。)


2.2 Bandwidth reclaiming(2.2 带宽回收)
--------------------------------------

 Bandwidth reclaiming for deadline tasks is based on the GRUB (Greedy
 Reclamation of Unused Bandwidth) algorithm [15, 16, 17] and it is enabled
 when flag SCHED_FLAG_RECLAIM is set.
 (deadline 任务的带宽回收基于 GRUB (贪婪回收未使用带宽, Greedy Reclamation of Unused Bandwidth) 算法 [15, 16, 17]，并且在设置了 SCHED_FLAG_RECLAIM 标志时启用。)

 The following diagram illustrates the state names for tasks handled by GRUB::

                             ------------
                 (d)        |   Active   |
              ------------->|            |
              |             | Contending |
              |              ------------
              |                A      |
          ----------           |      |
         |          |          |      |
         | Inactive |          |(b)   | (a)
         |          |          |      |
          ----------           |      |
              A                |      V
              |              ------------
              |             |   Active   |
              --------------|     Non    |
                 (c)        | Contending |
                             ------------

 A task can be in one of the following states:

  - ActiveContending: if it is ready for execution (or executing);
    (ActiveContending：如果它已准备好执行(或正在执行)；)

  - ActiveNonContending: if it just blocked and has not yet surpassed the 0-lag
    time;
    (ActiveNonContending：如果它刚刚阻塞，且尚未超过 0 滞后 (0-lag) 时间；)

  - Inactive: if it is blocked and has surpassed the 0-lag time.
    (Inactive：如果它已阻塞且已超过 0 滞后时间。)

 State transitions:

  (a) When a task blocks, it does not become immediately inactive since its
      bandwidth cannot be immediately reclaimed without breaking the
      real-time guarantees. It therefore enters a transitional state called
      ActiveNonContending. The scheduler arms the "inactive timer" to fire at
      the 0-lag time, when the task's bandwidth can be reclaimed without
      breaking the real-time guarantees.
      (状态转换：
      (a) 任务阻塞时并不会立即变为 inactive，因为立即回收其带宽会破坏实时保证。因此它进入一个称为 ActiveNonContending 的过渡状态。调度器会设置 "非活动定时器 (inactive timer)"，使其在 0 滞后时间到点时触发——届时回收该任务的带宽才不会破坏实时保证。)

      The 0-lag time for a task entering the ActiveNonContending state is
      computed as::

                        (runtime * dl_period)
             deadline - ---------------------
                             dl_runtime

      (进入 ActiveNonContending 状态的任务的 0 滞后时间按上面的式子计算，)

      where runtime is the remaining runtime, while dl_runtime and dl_period
      are the reservation parameters.
      (其中 runtime 是剩余运行时间，dl_runtime 和 dl_period 是预留参数。)

  (b) If the task wakes up before the inactive timer fires, the task re-enters
      the ActiveContending state and the "inactive timer" is canceled.
      In addition, if the task wakes up on a different runqueue, then
      the task's utilization must be removed from the previous runqueue's active
      utilization and must be added to the new runqueue's active utilization.
      In order to avoid races between a task waking up on a runqueue while the
      "inactive timer" is running on a different CPU, the "dl_non_contending"
      flag is used to indicate that a task is not on a runqueue but is active
      (so, the flag is set when the task blocks and is cleared when the
      "inactive timer" fires or when the task  wakes up).
      (为了避免"任务在某个运行队列上唤醒"与"非活动定时器在另一个 CPU 上运行"之间的竞态，使用 "dl_non_contending" 标志来表明任务不在运行队列上但仍处于活动状态(因此，任务阻塞时设置该标志；当 "非活动定时器" 触发或任务唤醒时清除该标志)。)

  (c) When the "inactive timer" fires, the task enters the Inactive state and
      its utilization is removed from the runqueue's active utilization.

  (d) When an inactive task wakes up, it enters the ActiveContending state and
      its utilization is added to the active utilization of the runqueue where
      it has been enqueued.
      ((c) 当 "非活动定时器" 触发时，任务进入 Inactive 状态，其利用率从运行队列的活动利用率中移除。
      (d) 当处于非活动状态的任务唤醒时，它进入 ActiveContending 状态，其利用率被加入它所入队的运行队列的活动利用率。)

 For each runqueue, the algorithm GRUB keeps track of two different bandwidths:

  - Active bandwidth (running_bw): this is the sum of the bandwidths of all
    tasks in active state (i.e., ActiveContending or ActiveNonContending);
    (对每个运行队列，GRUB 算法跟踪以下带宽。活动带宽 (running_bw)：所有处于活动状态(即 ActiveContending 或 ActiveNonContending)任务的带宽之和；)

  - Total bandwidth (this_bw): this is the sum of all tasks "belonging" to the
    runqueue, including the tasks in Inactive state.
    (总带宽 (this_bw)："属于"该运行队列的所有任务(包括 Inactive 状态的任务)的带宽之和。)

  - Maximum usable bandwidth (max_bw): This is the maximum bandwidth usable by
    deadline tasks and is currently set to the RT capacity.
    (最大可用带宽 (max_bw)：deadline 任务可使用的最大带宽，目前设置为 RT 容量。)


 The algorithm reclaims the bandwidth of the tasks in Inactive state.
 It does so by decrementing the runtime of the executing task Ti at a pace equal
 to
 (该算法回收 Inactive 状态任务的带宽。做法是：按下面的速度递减正在执行的任务 Ti 的 runtime)

           dq = -(max{ Ui, (Umax - Uinact - Uextra) } / Umax) dt

 where:

  - Ui is the bandwidth of task Ti;
    (其中：Ui 是任务 Ti 的带宽；)
  - Umax is the maximum reclaimable utilization (subjected to RT throttling
    limits);
    (Umax 是最大可回收利用率(受 RT 节流限制约束)；)
  - Uinact is the (per runqueue) inactive utilization, computed as
    (this_bq - running_bw);
    (Uinact 是(每运行队列的)非活动利用率，按 (this_bq - running_bw) 计算；)
  - Uextra is the (per runqueue) extra reclaimable utilization
    (subjected to RT throttling limits).
    (Uextra 是(每运行队列的)额外可回收利用率(受 RT 节流限制约束)。)


 Let's now see a trivial example of two deadline tasks with runtime equal
 to 4 and period equal to 8 (i.e., bandwidth equal to 0.5)::

         A            Task T1
         |
         |                               |
         |                               |
         |--------                       |----
         |       |                       V
         |---|---|---|---|---|---|---|---|--------->t
         0   1   2   3   4   5   6   7   8


         A            Task T2
         |
         |                               |
         |                               |
         |       ------------------------|
         |       |                       V
         |---|---|---|---|---|---|---|---|--------->t
         0   1   2   3   4   5   6   7   8


         A            running_bw
         |
       1 -----------------               ------
         |               |               |
      0.5-               -----------------
         |                               |
         |---|---|---|---|---|---|---|---|--------->t
         0   1   2   3   4   5   6   7   8


(下面来看一个简单的例子：两个 deadline 任务的 runtime 均为 4、period 均为 8(即带宽为 0.5)，示意图如上。)

  - Time t = 0:

    Both tasks are ready for execution and therefore in ActiveContending state.
    Suppose Task T1 is the first task to start execution.
    Since there are no inactive tasks, its runtime is decreased as dq = -1 dt.
    (时刻 t = 0：
    两个任务都已准备好执行，因此都处于 ActiveContending 状态。假设任务 T1 是第一个开始执行的任务。由于没有 inactive 任务，其 runtime 以 dq = -1 dt 的速度递减。)

  - Time t = 2:

    Suppose that task T1 blocks
    Task T1 therefore enters the ActiveNonContending state. Since its remaining
    runtime is equal to 2, its 0-lag time is equal to t = 4.
    Task T2 start execution, with runtime still decreased as dq = -1 dt since
    there are no inactive tasks.
    (时刻 t = 2：
    假设任务 T1 阻塞。于是 T1 进入 ActiveNonContending 状态。由于其剩余 runtime 等于 2，其 0 滞后时间为 t = 4。任务 T2 开始执行，runtime 仍以 dq = -1 dt 递减，因为没有 inactive 任务。)

  - Time t = 4:

    This is the 0-lag time for Task T1. Since it didn't woken up in the
    meantime, it enters the Inactive state. Its bandwidth is removed from
    running_bw.
    Task T2 continues its execution. However, its runtime is now decreased as
    dq = - 0.5 dt because Uinact = 0.5.
    Task T2 therefore reclaims the bandwidth unused by Task T1.
    (时刻 t = 4：
    这是任务 T1 的 0 滞后时间。由于它在此期间没有唤醒，它进入 Inactive 状态，其带宽从 running_bw 中移除。任务 T2 继续执行，但由于 Uinact = 0.5，其 runtime 现在以 dq = - 0.5 dt 递减。于是 T2 回收了 T1 未使用的带宽。)

  - Time t = 8:

    Task T1 wakes up. It enters the ActiveContending state again, and the
    running_bw is incremented.
    (时刻 t = 8：
    任务 T1 唤醒。它再次进入 ActiveContending 状态，running_bw 随之增加。)


2.3 Energy-aware scheduling(2.3 能耗感知调度)
---------------------------------------------

 When cpufreq's schedutil governor is selected, SCHED_DEADLINE implements the
 GRUB-PA [19] algorithm, reducing the CPU operating frequency to the minimum
 value that still allows to meet the deadlines. This behavior is currently
 implemented only for ARM architectures.
 (当选择 cpufreq 的 schedutil 调频器 (governor) 时，SCHED_DEADLINE 实现 GRUB-PA [19] 算法，把 CPU 工作频率降至仍能满足截止期的最小值。该行为目前仅在 ARM 架构上实现。)

 A particular care must be taken in case the time needed for changing frequency
 is of the same order of magnitude of the reservation period. In such cases,
 setting a fixed CPU frequency results in a lower amount of deadline misses.
 (如果改变频率所需的时间与预留周期在同一数量级，则需要特别注意。这种情况下，设置固定的 CPU 频率会减少截止期延误 (deadline miss) 的数量。)


3. Scheduling Real-Time Tasks(3. 调度实时任务)
==============================================



 ..  BIG FAT WARNING ******************************************************

 .. warning::

   This section contains a (not-thorough) summary on classical deadline
   scheduling theory, and how it applies to SCHED_DEADLINE.
   The reader can "safely" skip to Section 4 if only interested in seeing
   how the scheduling policy can be used. Anyway, we strongly recommend
   to come back here and continue reading (once the urge for testing is
   satisfied :P) to be sure of fully understanding all technical details.
   (警告：本节包含关于经典 deadline 调度理论及其如何应用于 SCHED_DEADLINE 的(并不详尽的)概述。如果读者只想了解如何使用这一调度策略，可以"放心地"直接跳到第 4 节。尽管如此，我们强烈建议在测试的冲动得到满足之后 (:P) 回来这里继续阅读，以确保完全理解所有技术细节。)

 .. ************************************************************************

 There are no limitations on what kind of task can exploit this new
 scheduling discipline, even if it must be said that it is particularly
 suited for periodic or sporadic real-time tasks that need guarantees on their
 timing behavior, e.g., multimedia, streaming, control applications, etc.
 (对能够利用这一新调度规则的任务类型没有任何限制，尽管必须说明的是，它特别适合需要对其时间行为有保证的周期性 (periodic) 或零星性 (sporadic) 实时任务，例如多媒体、流媒体、控制应用等。)

3.1 Definitions(3.1 定义)
-------------------------

 A typical real-time task is composed of a repetition of computation phases
 (task instances, or jobs) which are activated on a periodic or sporadic
 fashion.
 Each job J_j (where J_j is the j^th job of the task) is characterized by an
 arrival time r_j (the time when the job starts), an amount of computation
 time c_j needed to finish the job, and a job absolute deadline d_j, which
 is the time within which the job should be finished. The maximum execution
 time max{c_j} is called "Worst Case Execution Time" (WCET) for the task.
 A real-time task can be periodic with period P if r_{j+1} = r_j + P, or
 sporadic with minimum inter-arrival time P is r_{j+1} >= r_j + P. Finally,
 d_j = r_j + D, where D is the task's relative deadline.
 Summing up, a real-time task can be described as

	Task = (WCET, D, P)

 (典型的实时任务由周期性或零星性激活的计算阶段(任务实例，或称作业 job)的重复构成。每个作业 J_j(J_j 是该任务的第 j 个作业)由以下要素刻画：到达时间 r_j(作业开始的时刻)、完成该作业所需的计算时间 c_j，以及作业绝对截止期 d_j——即作业应在此时刻之前完成。最大执行时间 max{c_j} 称为该任务的 "最坏情况执行时间" (Worst Case Execution Time, WCET)。实时任务可以是周期 P 的周期任务(若 r_{j+1} = r_j + P)，也可以是最小到达间隔为 P 的零星任务(若 r_{j+1} >= r_j + P)。最后，d_j = r_j + D，其中 D 是任务的相对截止期。概括起来，一个实时任务可以描述为 Task = (WCET, D, P)。)

 The utilization of a real-time task is defined as the ratio between its
 WCET and its period (or minimum inter-arrival time), and represents
 the fraction of CPU time needed to execute the task.
 (实时任务的利用率 (utilization) 定义为其 WCET 与其周期(或最小到达间隔)之比，表示执行该任务所需的 CPU 时间份额。)

 If the total utilization U=sum(WCET_i/P_i) is larger than M (with M equal
 to the number of CPUs), then the scheduler is unable to respect all the
 deadlines.
 Note that total utilization is defined as the sum of the utilizations
 WCET_i/P_i over all the real-time tasks in the system. When considering
 multiple real-time tasks, the parameters of the i-th task are indicated
 with the "_i" suffix.
 Moreover, if the total utilization is larger than M, then we risk starving
 non- real-time tasks by real-time tasks.
 If, instead, the total utilization is smaller than M, then non real-time
 tasks will not be starved and the system might be able to respect all the
 deadlines.
 As a matter of fact, in this case it is possible to provide an upper bound
 for tardiness (defined as the maximum between 0 and the difference
 between the finishing time of a job and its absolute deadline).
 More precisely, it can be proven that using a global EDF scheduler the
 maximum tardiness of each task is smaller or equal than
 (如果总利用率 U=sum(WCET_i/P_i) 大于 M(M 为 CPU 数量)，调度器就无法满足所有截止期。
 注意，总利用率定义为系统中所有实时任务的利用率 WCET_i/P_i 之和。在考虑多个实时任务时，第 i 个任务的参数用 "_i" 后缀表示。
 此外，如果总利用率大于 M，实时任务就有把非实时任务饿死的风险。
 反之，如果总利用率小于 M，非实时任务就不会被饿死，系统也许能够满足所有截止期。
 事实上，在这种情况下可以为延误 (tardiness) 提供一个上界(延误定义为 0 与作业完成时刻与其绝对截止期之差两者的最大值)。
 更准确地说，可以证明：使用全局 EDF 调度器时，每个任务的最大延误小于等于上面的式子)

	((M − 1) · WCET_max − WCET_min)/(M − (M − 2) · U_max) + WCET_max

 where WCET_max = max{WCET_i} is the maximum WCET, WCET_min=min{WCET_i}
 is the minimum WCET, and U_max = max{WCET_i/P_i} is the maximum
 utilization[12].
 (其中 WCET_max = max{WCET_i} 是最大 WCET，WCET_min=min{WCET_i} 是最小 WCET，U_max = max{WCET_i/P_i} 是最大利用率[12]。)

3.2 Schedulability Analysis for Uniprocessor Systems(3.2 单处理器系统的可调度性分析)
------------------------------------------------------------------------------------

 If M=1 (uniprocessor system), or in case of partitioned scheduling (each
 real-time task is statically assigned to one and only one CPU), it is
 possible to formally check if all the deadlines are respected.
 If D_i = P_i for all tasks, then EDF is able to respect all the deadlines
 of all the tasks executing on a CPU if and only if the total utilization
 of the tasks running on such a CPU is smaller or equal than 1.
 If D_i != P_i for some task, then it is possible to define the density of
 a task as WCET_i/min{D_i,P_i}, and EDF is able to respect all the deadlines
 of all the tasks running on a CPU if the sum of the densities of the tasks
 running on such a CPU is smaller or equal than 1:
 (如果 M=1(单处理器系统)，或者采用分区调度(每个实时任务被静态地分配到一个且仅一个 CPU 上)，就可以形式化地检查是否满足所有截止期。
 如果所有任务的 D_i = P_i，那么当且仅当运行在该 CPU 上的任务的总利用率小于等于 1 时，EDF 才能满足该 CPU 上所有任务的所有截止期。
 如果某些任务的 D_i != P_i，则可以把任务的密度 (density) 定义为 WCET_i/min{D_i,P_i}，并且只要运行在某个 CPU 上的任务的密度之和小于等于 1，EDF 就能满足该 CPU 上所有任务的所有截止期：)

	sum(WCET_i / min{D_i, P_i}) <= 1

 It is important to notice that this condition is only sufficient, and not
 necessary: there are task sets that are schedulable, but do not respect the
 condition. For example, consider the task set {Task_1,Task_2} composed by
 Task_1=(50ms,50ms,100ms) and Task_2=(10ms,100ms,100ms).
 EDF is clearly able to schedule the two tasks without missing any deadline
 (Task_1 is scheduled as soon as it is released, and finishes just in time
 to respect its deadline; Task_2 is scheduled immediately after Task_1, hence
 its response time cannot be larger than 50ms + 10ms = 60ms) even if
 (重要的是要注意，该条件只是充分的，而非必要的：存在可调度却不满足该条件的任务集。例如，考虑由 Task_1=(50ms,50ms,100ms) 和 Task_2=(10ms,100ms,100ms) 组成的任务集 {Task_1,Task_2}。显然 EDF 能够调度这两个任务而不错过任何截止期(Task_1 一旦释放就被调度，并且刚好赶上其截止期完成；Task_2 在 Task_1 之后立即被调度，因此其响应时间不可能超过 50ms + 10ms = 60ms)，尽管上面的不等式成立)

	50 / min{50,100} + 10 / min{100, 100} = 50 / 50 + 10 / 100 = 1.1

 Of course it is possible to test the exact schedulability of tasks with
 D_i != P_i (checking a condition that is both sufficient and necessary),
 but this cannot be done by comparing the total utilization or density with
 a constant. Instead, the so called "processor demand" approach can be used,
 computing the total amount of CPU time h(t) needed by all the tasks to
 respect all of their deadlines in a time interval of size t, and comparing
 such a time with the interval size t. If h(t) is smaller than t (that is,
 the amount of time needed by the tasks in a time interval of size t is
 smaller than the size of the interval) for all the possible values of t, then
 EDF is able to schedule the tasks respecting all of their deadlines. Since
 performing this check for all possible values of t is impossible, it has been
 proven[4,5,6] that it is sufficient to perform the test for values of t
 between 0 and a maximum value L. The cited papers contain all of the
 mathematical details and explain how to compute h(t) and L.
 In any case, this kind of analysis is too complex as well as too
 time-consuming to be performed on-line. Hence, as explained in Section
 4 Linux uses an admission test based on the tasks' utilizations.
 (当然，对 D_i != P_i 的任务也可以测试其精确可调度性(即检查一个既充分又必要的条件)，但这无法通过把总利用率或密度与某个常数比较来完成。替代办法是使用所谓的 "处理器需求量 (processor demand)" 方法：计算所有任务在一个长度为 t 的时间区间内为满足其全部截止期所需的 CPU 时间总量 h(t)，并把该时间与区间长度 t 比较。如果对所有可能的 t 值都有 h(t) < t(即任务在长度为 t 的时间区间内所需的时间小于区间本身)，那么 EDF 就能调度这些任务并满足其全部截止期。由于对所有可能的 t 值都做这一检查是不可能的，文献[4,5,6]已证明：只需对 0 到最大值 L 之间的 t 值执行该测试即可。所引论文包含全部数学细节，并解释了如何计算 h(t) 和 L。
 无论如何，这类分析过于复杂且耗时，无法在线执行。因此，如第 4 节所述，Linux 使用基于任务利用率的准入测试。)

3.3 Schedulability Analysis for Multiprocessor Systems(3.3 多处理器系统的可调度性分析)
--------------------------------------------------------------------------------------

 On multiprocessor systems with global EDF scheduling (non partitioned
 systems), a sufficient test for schedulability can not be based on the
 utilizations or densities: it can be shown that even if D_i = P_i task
 sets with utilizations slightly larger than 1 can miss deadlines regardless
 of the number of CPUs.
 (在采用全局 EDF 调度(非分区系统)的多处理器系统上，可调度性的充分测试不能基于利用率或密度：可以证明，即使 D_i = P_i，利用率略大于 1 的任务集也可能错过截止期，而与 CPU 数量无关。)

 Consider a set {Task_1,...Task_{M+1}} of M+1 tasks on a system with M
 CPUs, with the first task Task_1=(P,P,P) having period, relative deadline
 and WCET equal to P. The remaining M tasks Task_i=(e,P-1,P-1) have an
 arbitrarily small worst case execution time (indicated as "e" here) and a
 period smaller than the one of the first task. Hence, if all the tasks
 activate at the same time t, global EDF schedules these M tasks first
 (because their absolute deadlines are equal to t + P - 1, hence they are
 smaller than the absolute deadline of Task_1, which is t + P). As a
 result, Task_1 can be scheduled only at time t + e, and will finish at
 time t + e + P, after its absolute deadline. The total utilization of the
 task set is U = M · e / (P - 1) + P / P = M · e / (P - 1) + 1, and for small
 values of e this can become very close to 1. This is known as "Dhall's
 effect"[7]. Note: the example in the original paper by Dhall has been
 slightly simplified here (for example, Dhall more correctly computed
 lim_{e->0}U).
 (考虑在一个有 M 个 CPU 的系统上由 M+1 个任务组成的集合 {Task_1,...Task_{M+1}}，其中第一个任务 Task_1=(P,P,P) 的周期、相对截止期和 WCET 都等于 P。其余 M 个任务 Task_i=(e,P-1,P-1) 的最坏情况执行时间可以任意小(这里记作 "e")，且周期小于第一个任务。因此，如果所有任务都在同一时刻 t 激活，全局 EDF 会先调度这 M 个任务(因为它们的绝对截止期等于 t + P - 1，小于 Task_1 的绝对截止期 t + P)。结果，Task_1 只能在时刻 t + e 被调度，并将在时刻 t + e + P 完成，晚于其绝对截止期。该任务集的总利用率为 U = M · e / (P - 1) + P / P = M · e / (P - 1) + 1，当 e 很小时它可以非常接近 1。这就是著名的 "Dhall 效应"[7]。注：Dhall 原始论文中的例子在这里做了些许简化(例如，Dhall 更准确地计算了 lim_{e->0}U)。)

 More complex schedulability tests for global EDF have been developed in
 real-time literature[8,9], but they are not based on a simple comparison
 between total utilization (or density) and a fixed constant. If all tasks
 have D_i = P_i, a sufficient schedulability condition can be expressed in
 a simple way:
 (实时文献[8,9]中已提出了更复杂的全局 EDF 可调度性测试，但它们都不基于总利用率(或密度)与某个固定常数之间的简单比较。如果所有任务都有 D_i = P_i，则一个充分的可调度性条件可以用简单的方式表达：)

	sum(WCET_i / P_i) <= M - (M - 1) · U_max

 where U_max = max{WCET_i / P_i}[10]. Notice that for U_max = 1,
 M - (M - 1) · U_max becomes M - M + 1 = 1 and this schedulability condition
 just confirms the Dhall's effect. A more complete survey of the literature
 about schedulability tests for multi-processor real-time scheduling can be
 found in [11].
 (其中 U_max = max{WCET_i / P_i}[10]。注意，当 U_max = 1 时，M - (M - 1) · U_max 变为 M - M + 1 = 1，该可调度性条件恰好印证了 Dhall 效应。关于多处理器实时调度可调度性测试的文献，[11] 中有更完整的综述。)

 As seen, enforcing that the total utilization is smaller than M does not
 guarantee that global EDF schedules the tasks without missing any deadline
 (in other words, global EDF is not an optimal scheduling algorithm). However,
 a total utilization smaller than M is enough to guarantee that non real-time
 tasks are not starved and that the tardiness of real-time tasks has an upper
 bound[12] (as previously noted). Different bounds on the maximum tardiness
 experienced by real-time tasks have been developed in various papers[13,14],
 but the theoretical result that is important for SCHED_DEADLINE is that if
 the total utilization is smaller or equal than M then the response times of
 the tasks are limited.
 (如前所见，仅仅强制总利用率小于 M 并不能保证全局 EDF 调度任务时不错过任何截止期(换言之，全局 EDF 不是最优调度算法)。不过，总利用率小于 M 已足以保证非实时任务不会被饿死，且实时任务的延误有上界[12](如前所述)。多篇论文[13,14]给出了实时任务最大延误的不同上界，但对 SCHED_DEADLINE 重要的理论结论是：如果总利用率小于等于 M，那么任务的响应时间是有界的。)

3.4 Relationship with SCHED_DEADLINE Parameters(3.4 与 SCHED_DEADLINE 参数的关系)
---------------------------------------------------------------------------------

 Finally, it is important to understand the relationship between the
 SCHED_DEADLINE scheduling parameters described in Section 2 (runtime,
 deadline and period) and the real-time task parameters (WCET, D, P)
 described in this section. Note that the tasks' temporal constraints are
 represented by its absolute deadlines d_j = r_j + D described above, while
 SCHED_DEADLINE schedules the tasks according to scheduling deadlines (see
 Section 2).
 If an admission test is used to guarantee that the scheduling deadlines
 are respected, then SCHED_DEADLINE can be used to schedule real-time tasks
 guaranteeing that all the jobs' deadlines of a task are respected.
 In order to do this, a task must be scheduled by setting:
 (最后，理解第 2 节描述的 SCHED_DEADLINE 调度参数(runtime、deadline 和 period)与本节描述的实时任务参数(WCET、D、P)之间的关系非常重要。注意，任务的时间约束由上文所述的绝对截止期 d_j = r_j + D 表示，而 SCHED_DEADLINE 按照调度截止期调度任务(见第 2 节)。
 如果使用准入测试来保证调度截止期得到满足，那么 SCHED_DEADLINE 就可以用于调度实时任务，并保证任务的所有作业截止期都得到满足。
 为此，必须按如下方式设置任务参数：)

  - runtime >= WCET
  - deadline = D
  - period <= P

 IOW, if runtime >= WCET and if period is <= P, then the scheduling deadlines
 and the absolute deadlines (d_j) coincide, so a proper admission control
 allows to respect the jobs' absolute deadlines for this task (this is what is
 called "hard schedulability property" and is an extension of Lemma 1 of [2]).
 Notice that if runtime > deadline the admission control will surely reject
 this task, as it is not possible to respect its temporal constraints.

 References:
 (换句话说，如果 runtime >= WCET 且 period <= P，那么调度截止期与绝对截止期 (d_j) 重合，因此恰当的准入控制就能保证该任务的各作业绝对截止期得到满足(这就是所谓的 "硬可调度性" (hard schedulability property)，是文献[2]引理 1 的推广)。
 注意，如果 runtime > deadline，准入控制必定会拒绝该任务，因为其时间约束根本无法满足。

 以下为参考文献条目，保留原文与链接。)

  1 - C. L. Liu and J. W. Layland. Scheduling algorithms for multiprogram-
      ming in a hard-real-time environment. Journal of the Association for
      Computing Machinery, 20(1), 1973.
  2 - L. Abeni , G. Buttazzo. Integrating Multimedia Applications in Hard
      Real-Time Systems. Proceedings of the 19th IEEE Real-time Systems
      Symposium, 1998. http://retis.sssup.it/~giorgio/paps/1998/rtss98-cbs.pdf
  3 - L. Abeni. Server Mechanisms for Multimedia Applications. ReTiS Lab
      Technical Report. http://disi.unitn.it/~abeni/tr-98-01.pdf
  4 - J. Y. Leung and M.L. Merril. A Note on Preemptive Scheduling of
      Periodic, Real-Time Tasks. Information Processing Letters, vol. 11,
      no. 3, pp. 115-118, 1980.
  5 - S. K. Baruah, A. K. Mok and L. E. Rosier. Preemptively Scheduling
      Hard-Real-Time Sporadic Tasks on One Processor. Proceedings of the
      11th IEEE Real-time Systems Symposium, 1990.
  6 - S. K. Baruah, L. E. Rosier and R. R. Howell. Algorithms and Complexity
      Concerning the Preemptive Scheduling of Periodic Real-Time tasks on
      One Processor. Real-Time Systems Journal, vol. 4, no. 2, pp 301-324,
      1990.
  7 - S. J. Dhall and C. L. Liu. On a real-time scheduling problem. Operations
      research, vol. 26, no. 1, pp 127-140, 1978.
  8 - T. Baker. Multiprocessor EDF and Deadline Monotonic Schedulability
      Analysis. Proceedings of the 24th IEEE Real-Time Systems Symposium, 2003.
  9 - T. Baker. An Analysis of EDF Schedulability on a Multiprocessor.
      IEEE Transactions on Parallel and Distributed Systems, vol. 16, no. 8,
      pp 760-768, 2005.
  10 - J. Goossens, S. Funk and S. Baruah, Priority-Driven Scheduling of
       Periodic Task Systems on Multiprocessors. Real-Time Systems Journal,
       vol. 25, no. 2–3, pp. 187–205, 2003.
  11 - R. Davis and A. Burns. A Survey of Hard Real-Time Scheduling for
       Multiprocessor Systems. ACM Computing Surveys, vol. 43, no. 4, 2011.
       http://www-users.cs.york.ac.uk/~robdavis/papers/MPSurveyv5.0.pdf
  12 - U. C. Devi and J. H. Anderson. Tardiness Bounds under Global EDF
       Scheduling on a Multiprocessor. Real-Time Systems Journal, vol. 32,
       no. 2, pp 133-189, 2008.
  13 - P. Valente and G. Lipari. An Upper Bound to the Lateness of Soft
       Real-Time Tasks Scheduled by EDF on Multiprocessors. Proceedings of
       the 26th IEEE Real-Time Systems Symposium, 2005.
  14 - J. Erickson, U. Devi and S. Baruah. Improved tardiness bounds for
       Global EDF. Proceedings of the 22nd Euromicro Conference on
       Real-Time Systems, 2010.
  15 - G. Lipari, S. Baruah, Greedy reclamation of unused bandwidth in
       constant-bandwidth servers, 12th IEEE Euromicro Conference on Real-Time
       Systems, 2000.
  16 - L. Abeni, J. Lelli, C. Scordino, L. Palopoli, Greedy CPU reclaiming for
       SCHED DEADLINE. In Proceedings of the Real-Time Linux Workshop (RTLWS),
       Dusseldorf, Germany, 2014.
  17 - L. Abeni, G. Lipari, A. Parri, Y. Sun, Multicore CPU reclaiming: parallel
       or sequential?. In Proceedings of the 31st Annual ACM Symposium on Applied
       Computing, 2016.
  18 - J. Lelli, C. Scordino, L. Abeni, D. Faggioli, Deadline scheduling in the
       Linux kernel, Software: Practice and Experience, 46(6): 821-839, June
       2016.
  19 - C. Scordino, L. Abeni, J. Lelli, Energy-Aware Real-Time Scheduling in
       the Linux Kernel, 33rd ACM/SIGAPP Symposium On Applied Computing (SAC
       2018), Pau, France, April 2018.


4. Bandwidth management(4. 带宽管理)
====================================

 As previously mentioned, in order for -deadline scheduling to be
 effective and useful (that is, to be able to provide "runtime" time units
 within "deadline"), it is important to have some method to keep the allocation
 of the available fractions of CPU time to the various tasks under control.
 This is usually called "admission control" and if it is not performed, then
 no guarantee can be given on the actual scheduling of the -deadline tasks.
 (如前所述，要让 -deadline 调度有效且有用(即能在 "deadline" 之内提供 "runtime" 个时间单位)，必须有某种方法把可用 CPU 时间份额对各任务的分配控制在掌握之中。这通常称为 "准入控制" (admission control)；如果不执行准入控制，就无法对 -deadline 任务的实际调度给出任何保证。)

 As already stated in Section 3, a necessary condition to be respected to
 correctly schedule a set of real-time tasks is that the total utilization
 is smaller than M. When talking about -deadline tasks, this requires that
 the sum of the ratio between runtime and period for all tasks is smaller
 than M. Notice that the ratio runtime/period is equivalent to the utilization
 of a "traditional" real-time task, and is also often referred to as
 "bandwidth".
 The interface used to control the CPU bandwidth that can be allocated
 to -deadline tasks is similar to the one already used for -rt
 tasks with real-time group scheduling (a.k.a. RT-throttling - see
 Documentation/scheduler/sched-rt-group.rst), and is based on readable/
 writable control files located in procfs (for system wide settings).
 Notice that per-group settings (controlled through cgroupfs) are still not
 defined for -deadline tasks, because more discussion is needed in order to
 figure out how we want to manage SCHED_DEADLINE bandwidth at the task group
 level.
 (如第 3 节所述，正确调度一组实时任务的一个必要条件是总利用率小于 M。对 -deadline 任务而言，这要求所有任务的 runtime 与 period 之比之和小于 M。注意，比值 runtime/period 等价于 "传统" 实时任务的利用率，也常被称为 "带宽" (bandwidth)。
 用于控制可分配给 -deadline 任务的 CPU 带宽的接口，与实时组调度中的 -rt 任务已使用的接口类似(又称 RT-throttling——见 Documentation/scheduler/sched-rt-group.rst)，它基于位于 procfs 中的可读/可写控制文件(用于系统级设置)。
 注意，(通过 cgroupfs 控制的)每分组设置对 -deadline 任务仍未定义，因为在如何于任务组级别管理 SCHED_DEADLINE 带宽这一问题上还需要更多讨论。)

 A main difference between deadline bandwidth management and RT-throttling
 is that -deadline tasks have bandwidth on their own (while -rt ones don't!),
 and thus we don't need a higher level throttling mechanism to enforce the
 desired bandwidth. In other words, this means that interface parameters are
 only used at admission control time (i.e., when the user calls
 sched_setattr()). Scheduling is then performed considering actual tasks'
 parameters, so that CPU bandwidth is allocated to SCHED_DEADLINE tasks
 respecting their needs in terms of granularity. Therefore, using this simple
 interface we can put a cap on total utilization of -deadline tasks (i.e.,
 \Sum (runtime_i / period_i) < global_dl_utilization_cap).
 (deadline 带宽管理与 RT-throttling 的一个主要区别在于：-deadline 任务自身带有带宽(而 -rt 任务没有！)，因此我们不需要更高层的节流机制来强制实施期望的带宽。换言之，这意味着接口参数只在准入控制时使用(即用户调用 sched_setattr() 时)。随后进行调度时考虑的是任务的实际参数，使 CPU 带宽按粒度方面的需求分配给 SCHED_DEADLINE 任务。因此，通过这个简单的接口，我们可以对 -deadline 任务的总利用率设置上限(即 \Sum (runtime_i / period_i) < global_dl_utilization_cap)。)

4.1 System wide settings(4.1 系统级设置)
----------------------------------------

 The system wide settings are configured under the /proc virtual file system.

 For now the -rt knobs are used for -deadline admission control and with
 CONFIG_RT_GROUP_SCHED the -deadline runtime is accounted against the (root)
 -rt runtime. With !CONFIG_RT_GROUP_SCHED the knob only serves for the -dl
 admission control. We realize that this isn't entirely desirable; however, it
 is better to have a small interface for now, and be able to change it easily
 later. The ideal situation (see 5.) is to run -rt tasks from a -deadline
 server; in which case the -rt bandwidth is a direct subset of dl_bw.
 (系统级设置在 /proc 虚拟文件系统下配置。
 目前 -rt 的旋钮 (knob) 被用于 -deadline 的准入控制，并且在 CONFIG_RT_GROUP_SCHED 下，-deadline 的 runtime 会计入 (root) -rt 的 runtime。而在 !CONFIG_RT_GROUP_SCHED 下，该旋钮仅用于 -dl 的准入控制。我们意识到这并不完全理想；不过，现阶段先有一个小接口、以后能够方便地修改，总比没有好。理想情况(见 5.)是从 -deadline 服务器中运行 -rt 任务；此时 -rt 带宽是 dl_bw 的直接子集。)

 This means that, for a root_domain comprising M CPUs, -deadline tasks
 can be created while the sum of their bandwidths stays below:

   M * (sched_rt_runtime_us / sched_rt_period_us)

 It is also possible to disable this bandwidth management logic, and
 be thus free of oversubscribing the system up to any arbitrary level.
 This is done by writing -1 in /proc/sys/kernel/sched_rt_runtime_us.
 (这意味着，对一个由 M 个 CPU 组成的 root_domain，只要其带宽之和保持在下面的值以下，就可以创建 -deadline 任务：

   M * (sched_rt_runtime_us / sched_rt_period_us)

 也可以禁用这一带宽管理逻辑，从而可以任意程度地超额订阅系统。做法是向 /proc/sys/kernel/sched_rt_runtime_us 写入 -1。)


4.2 Task interface(4.2 任务接口)
--------------------------------

 Specifying a periodic/sporadic task that executes for a given amount of
 runtime at each instance, and that is scheduled according to the urgency of
 its own timing constraints needs, in general, a way of declaring:

  - a (maximum/typical) instance execution time,
  - a minimum interval between consecutive instances,
  - a time constraint by which each instance must be completed.

 Therefore:

  * a new struct sched_attr, containing all the necessary fields is
    provided;
  * the new scheduling related syscalls that manipulate it, i.e.,
    sched_setattr() and sched_getattr() are implemented.

 (声明一个周期性/零星性任务——它在每个实例中执行给定的 runtime 时长，并按其自身时间约束的紧迫程度被调度——一般需要一种方式来声明：

  - 每个(最大/典型)实例的执行时间，
  - 相邻实例之间的最小间隔，
  - 每个实例必须在此时间约束之内完成。

 因此：

  * 提供了包含所有必要字段的新结构体 struct sched_attr；
  * 实现了操作它的新的调度相关系统调用，即 sched_setattr() 和 sched_getattr()。)

 The leftover runtime and absolute deadline of a SCHED_DEADLINE task can be
 read using the sched_getattr() syscall, setting the last syscall parameter
 flags to the SCHED_GETATTR_FLAG_DL_DYNAMIC=1 value. This updates the
 runtime left, converts the absolute deadline in CLOCK_MONOTONIC reference,
 then returns these parameters to user-space. The absolute deadline is
 returned as the number of nanoseconds since the CLOCK_MONOTONIC time
 reference (boot instant), as a u64 in the sched_deadline field of sched_attr,
 which can represent nearly 585 years since boot time (calling sched_getattr()
 with flags=0 causes retrieval of the static parameters instead).
 (SCHED_DEADLINE 任务的剩余 runtime 和绝对截止期可以通过 sched_getattr() 系统调用读取，把该系统调用的最后一个参数 flags 设为 SCHED_GETATTR_FLAG_DL_DYNAMIC=1。这会更新剩余 runtime，把绝对截止期转换为 CLOCK_MONOTONIC 时间基准，然后把这些参数返回给用户空间。绝对截止期以自 CLOCK_MONOTONIC 时间基准(开机时刻)以来的纳秒数返回，作为 sched_attr 的 sched_deadline 字段中的一个 u64，它可以表示开机后近 585 年的时间(以 flags=0 调用 sched_getattr() 则改为获取静态参数)。)

 For debugging purposes, these parameters can also be retrieved through
 /proc/<pid>/sched (entries dl.runtime and dl.deadline, both values in ns),
 but: this is highly inefficient; the returned runtime left is not updated as
 done by sched_getattr(); the deadline is provided in kernel rq_clock time
 reference, that is not directly usable from user-space.
 (出于调试目的，这些参数也可以通过 /proc/<pid>/sched 获取(条目 dl.runtime 和 dl.deadline，数值均以 ns 为单位)，但是：这种方式效率极低；返回的剩余 runtime 不会像 sched_getattr() 那样更新；deadline 以内核 rq_clock 时间基准给出，用户空间无法直接使用。)


4.3 Default behavior(4.3 默认行为)
----------------------------------

 The default value for SCHED_DEADLINE bandwidth is to have rt_runtime equal to
 950000. With rt_period equal to 1000000, by default, it means that -deadline
 tasks can use at most 95%, multiplied by the number of CPUs that compose the
 root_domain, for each root_domain.
 This means that non -deadline tasks will receive at least 5% of the CPU time,
 and that -deadline tasks will receive their runtime with a guaranteed
 worst-case delay respect to the "deadline" parameter. If "deadline" = "period"
 and the cpuset mechanism is used to implement partitioned scheduling (see
 Section 5), then this simple setting of the bandwidth management is able to
 deterministically guarantee that -deadline tasks will receive their runtime
 in a period.

 Finally, notice that in order not to jeopardize the admission control a
 -deadline task cannot fork.
 (SCHED_DEADLINE 带宽的默认值是 rt_runtime 等于 950000。在 rt_period 等于 1000000 时，默认意味着每个 root_domain 上的 -deadline 任务最多可以使用 95% 乘以组成该 root_domain 的 CPU 数量。
 这意味着非 -deadline 任务将获得至少 5% 的 CPU 时间，而 -deadline 任务将获得其 runtime，相对 "deadline" 参数具有有保证的最坏情况延迟。如果 "deadline" = "period"，并且使用 cpuset 机制实现分区调度(见第 5 节)，那么这一简单的带宽管理设置就能确定性地保证 -deadline 任务在一个周期内获得其 runtime。
 最后要注意，为了不破坏准入控制，-deadline 任务不能 fork。)


4.4 Behavior of sched_yield()(4.4 sched_yield() 的行为)
-------------------------------------------------------

 When a SCHED_DEADLINE task calls sched_yield(), it gives up its
 remaining runtime and is immediately throttled, until the next
 period, when its runtime will be replenished (a special flag
 dl_yielded is set and used to handle correctly throttling and runtime
 replenishment after a call to sched_yield()).

 This behavior of sched_yield() allows the task to wake-up exactly at
 the beginning of the next period. Also, this may be useful in the
 future with bandwidth reclaiming mechanisms, where sched_yield() will
 make the leftoever runtime available for reclamation by other
 SCHED_DEADLINE tasks.
 (当 SCHED_DEADLINE 任务调用 sched_yield() 时，它会放弃剩余的 runtime 并立即被节流，直到下一个周期其 runtime 得到补给(会设置一个特殊标志 dl_yielded，用于在调用 sched_yield() 之后正确处理节流和 runtime 补给)。
 sched_yield() 的这一行为使任务恰好在下一个周期开始时唤醒。此外，在将来配合带宽回收机制时这可能也有用：sched_yield() 会让剩余的 runtime 可供其他 SCHED_DEADLINE 任务回收。)


5. Tasks CPU affinity(5. 任务的 CPU 亲和性)
===========================================

 Deadline tasks cannot have a cpu affinity mask smaller than the root domain they
 are created on. So, using ``sched_setaffinity(2)`` won't work. Instead, the
 the deadline task should be created in a restricted root domain. This can be
 done using the cpuset controller of either cgroup v1 (deprecated) or cgroup v2.
 See :ref:`Documentation/admin-guide/cgroup-v1/cpusets.rst <cpusets>` and
 :ref:`Documentation/admin-guide/cgroup-v2.rst <cgroup-v2>` for more information.
 (deadline 任务的 CPU 亲和性掩码不能小于其创建时所在的根域 (root domain)。因此，使用 ``sched_setaffinity(2)`` 不会起作用。正确的做法是把 deadline 任务创建在一个受限的根域中。这可以用 cgroup v1(已废弃)或 cgroup v2 的 cpuset 控制器完成。更多信息见 :ref:`Documentation/admin-guide/cgroup-v1/cpusets.rst <cpusets>` 和 :ref:`Documentation/admin-guide/cgroup-v2.rst <cgroup-v2>`。)

5.1 Using cgroup v1 cpuset controller(5.1 使用 cgroup v1 cpuset 控制器)
-----------------------------------------------------------------------

 An example of a simple configuration (pin a -deadline task to CPU0) follows::

   mkdir /dev/cpuset
   mount -t cgroup -o cpuset cpuset /dev/cpuset
   cd /dev/cpuset
   mkdir cpu0
   echo 0 > cpu0/cpuset.cpus
   echo 0 > cpu0/cpuset.mems
   echo 1 > cpuset.cpu_exclusive
   echo 0 > cpuset.sched_load_balance
   echo 1 > cpu0/cpuset.cpu_exclusive
   echo 1 > cpu0/cpuset.mem_exclusive
   echo $$ > cpu0/tasks
   chrt --sched-runtime 100000 --sched-period 200000 --deadline 0 yes > /dev/null

 (以上命令展示了一个简单配置的示例(把一个 -deadline 任务绑到 CPU0)。)

5.2 Using cgroup v2 cpuset controller(5.2 使用 cgroup v2 cpuset 控制器)
-----------------------------------------------------------------------

 Assuming the cgroup v2 root is mounted at ``/sys/fs/cgroup``, an example of a
 simple configuration (pin a -deadline task to CPU0) follows::

   cd /sys/fs/cgroup
   echo '+cpuset' > cgroup.subtree_control
   mkdir deadline_group
   echo 0 > deadline_group/cpuset.cpus
   echo 'root' > deadline_group/cpuset.cpus.partition
   echo $$ > deadline_group/cgroup.procs
   chrt --sched-runtime 100000 --sched-period 200000 --deadline 0 yes > /dev/null

 (以上命令假设 cgroup v2 根挂载在 ``/sys/fs/cgroup``，展示了一个简单配置的示例(把一个 -deadline 任务绑到 CPU0)。)

6. Future plans(6. 未来计划)
============================

 Still missing:
 (仍缺少的功能：)

  - programmatic way to retrieve current runtime and absolute deadline
    (以编程方式获取当前 runtime 和绝对截止期；)
  - refinements to deadline inheritance, especially regarding the possibility
    of retaining bandwidth isolation among non-interacting tasks. This is
    being studied from both theoretical and practical points of view, and
    hopefully we should be able to produce some demonstrative code soon;
    (对 deadline 继承的改进，尤其是在非交互任务之间保持带宽隔离的可能性方面。目前正在从理论和实践两个角度进行研究，希望能尽快拿出一些演示代码；)
  - (c)group based bandwidth management, and maybe scheduling;
    (基于 (c)group 的带宽管理，或许还有调度；)
  - access control for non-root users (and related security concerns to
    address), which is the best way to allow unprivileged use of the mechanisms
    and how to prevent non-root users "cheat" the system?
    (非 root 用户的访问控制(以及需要解决的相关安全问题)：允许无特权用户使用这些机制的最佳方式是什么？又该如何防止非 root 用户"欺骗"系统？)

 As already discussed, we are planning also to merge this work with the EDF
 throttling patches [https://lore.kernel.org/r/cover.1266931410.git.fabio@helm.retis] but we still are in
 the preliminary phases of the merge and we really seek feedback that would
 help us decide on the direction it should take.
 (如前文所讨论的，我们还计划把这项工作与 EDF 节流补丁 [https://lore.kernel.org/r/cover.1266931410.git.fabio@helm.retis] 合并，但目前合并仍处于初步阶段，我们非常希望得到能帮助我们决定其走向的反馈。)

Appendix A. Test suite(附录 A. 测试套件)
========================================

 The SCHED_DEADLINE policy can be easily tested using two applications that
 are part of a wider Linux Scheduler validation suite. The suite is
 available as a GitHub repository: https://github.com/scheduler-tools.
 (可以使用两个应用程序方便地测试 SCHED_DEADLINE 策略，它们是更庞大的 Linux 调度器验证套件的一部分。该套件以 GitHub 仓库形式提供：https://github.com/scheduler-tools。)

 The first testing application is called rt-app and can be used to
 start multiple threads with specific parameters. rt-app supports
 SCHED_{OTHER,FIFO,RR,DEADLINE} scheduling policies and their related
 parameters (e.g., niceness, priority, runtime/deadline/period). rt-app
 is a valuable tool, as it can be used to synthetically recreate certain
 workloads (maybe mimicking real use-cases) and evaluate how the scheduler
 behaves under such workloads. In this way, results are easily reproducible.
 rt-app is available at: https://github.com/scheduler-tools/rt-app.
 (第一个测试应用名为 rt-app，可用于以特定参数启动多个线程。rt-app 支持 SCHED_{OTHER,FIFO,RR,DEADLINE} 调度策略及其相关参数(例如 niceness、priority、runtime/deadline/period)。rt-app 是一个很有价值的工具，因为它可以用来人工合成地重现某些工作负载(也许可以模拟真实的使用场景)，并评估调度器在这些负载下的表现。这样，结果就很容易复现。rt-app 见：https://github.com/scheduler-tools/rt-app。)

 rt-app does not accept command line arguments, and instead reads from a JSON
 configuration file. Here is an example ``config.json``:

 .. code-block:: json

  {
    "tasks": {
      "dl_task": {
        "policy": "SCHED_DEADLINE",
        "priority": 0,
        "dl-runtime": 10000,
        "dl-period": 100000,
        "dl-deadline": 100000
      },
      "fifo_task": {
        "policy": "SCHED_FIFO",
        "priority": 10,
        "runtime": 20000,
        "sleep": 130000
      }
    },
    "global": {
      "duration": 5
    }
  }

 (rt-app 不接受命令行参数，而是从 JSON 配置文件读取。上面是一个 ``config.json`` 示例。)

 On running ``rt-app config.json``, it creates 2 threads. The first one,
 scheduled by SCHED_DEADLINE, executes for 10ms every 100ms. The second one,
 scheduled at SCHED_FIFO priority 10, executes for 20ms every 150ms. The test
 will run for a total of 5 seconds.

 Please refer to the rt-app documentation for the JSON schema and more examples.
 (运行 ``rt-app config.json`` 时，它会创建 2 个线程。第一个由 SCHED_DEADLINE 调度，每 100ms 执行 10ms。第二个以 SCHED_FIFO 优先级 10 调度，每 150ms 执行 20ms。测试总共运行 5 秒。
 JSON 模式 (schema) 及更多示例请参见 rt-app 的文档。)

 The second testing application is done using chrt which has support
 for SCHED_DEADLINE.

 The usage is straightforward::

  # chrt -d -T 10000000 -D 100000000 0 ./my_cpuhog_app

 (第二个测试应用使用支持 SCHED_DEADLINE 的 chrt。
 用法很直接：)

 With this, my_cpuhog_app is put to run inside a SCHED_DEADLINE reservation
 of 10ms every 100ms (note that parameters are expressed in nanoseconds).
 You can also use chrt to create a reservation for an already running
 application, given that you know its pid::

  # chrt -d -T 10000000 -D 100000000 -p 0 my_app_pid

 (这样，my_cpuhog_app 就会在一个每 100ms 提供 10ms 的 SCHED_DEADLINE 预留中运行(注意参数以纳秒表示)。也可以使用 chrt 为一个已在运行的应用创建预留，前提是你知道它的 pid：)

Appendix B. Minimal main()(附录 B. 最小化 main())
=================================================

 We provide in what follows a simple (ugly) self-contained code snippet
 showing how SCHED_DEADLINE reservations can be created by a real-time
 application developer::

   #define _GNU_SOURCE
   #include <unistd.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <time.h>
   #include <linux/unistd.h>
   #include <linux/kernel.h>
   #include <linux/types.h>
   #include <sys/syscall.h>
   #include <pthread.h>

   #define gettid() syscall(__NR_gettid)

   #define SCHED_DEADLINE	6

   /* XXX use the proper syscall numbers */
   #ifdef __x86_64__
   #define __NR_sched_setattr		314
   #define __NR_sched_getattr		315
   #endif

   #ifdef __i386__
   #define __NR_sched_setattr		351
   #define __NR_sched_getattr		352
   #endif

   #ifdef __arm__
   #define __NR_sched_setattr		380
   #define __NR_sched_getattr		381
   #endif

   static volatile int done;

   struct sched_attr {
	__u32 size;

	__u32 sched_policy;
	__u64 sched_flags;

	/* SCHED_NORMAL, SCHED_BATCH */
	__s32 sched_nice;

	/* SCHED_FIFO, SCHED_RR */
	__u32 sched_priority;

	/* SCHED_DEADLINE (nsec) */
	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
   };

   int sched_setattr(pid_t pid,
		  const struct sched_attr *attr,
		  unsigned int flags)
   {
	return syscall(__NR_sched_setattr, pid, attr, flags);
   }

   int sched_getattr(pid_t pid,
		  struct sched_attr *attr,
		  unsigned int size,
		  unsigned int flags)
   {
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
   }

   void *run_deadline(void *data)
   {
	struct sched_attr attr;
	int x = 0;
	int ret;
	unsigned int flags = 0;

	printf("deadline thread started [%ld]\n", gettid());

	attr.size = sizeof(attr);
	attr.sched_flags = 0;
	attr.sched_nice = 0;
	attr.sched_priority = 0;

	/* This creates a 10ms/30ms reservation */
	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime = 10 * 1000 * 1000;
	attr.sched_period = attr.sched_deadline = 30 * 1000 * 1000;

	ret = sched_setattr(0, &attr, flags);
	if (ret < 0) {
		done = 0;
		perror("sched_setattr");
		exit(-1);
	}

	while (!done) {
		x++;
	}

	printf("deadline thread dies [%ld]\n", gettid());
	return NULL;
   }

   int main (int argc, char **argv)
   {
	pthread_t thread;

	printf("main thread [%ld]\n", gettid());

	pthread_create(&thread, NULL, run_deadline, NULL);

	sleep(10);

	done = 1;
	pthread_join(thread, NULL);

	printf("main dies [%ld]\n", gettid());
	return 0;
   }

 (下面提供了一个简单(丑陋)但自包含的代码片段，展示实时应用开发者如何创建 SCHED_DEADLINE 预留。)
