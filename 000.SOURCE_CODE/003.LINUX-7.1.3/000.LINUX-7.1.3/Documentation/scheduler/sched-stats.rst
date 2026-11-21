================================
Scheduler Statistics(调度器统计)
================================

Version 17 of schedstats removed 'lb_imbalance' field as it has no
significance anymore and instead added more relevant fields namely
'lb_imbalance_load', 'lb_imbalance_util', 'lb_imbalance_task' and
'lb_imbalance_misfit'. The domain field prints the name of the
corresponding sched domain from this version onwards.
(schedstats 第 17 版移除了已无意义的 'lb_imbalance' 字段，改为添加了更相关的字段，即 'lb_imbalance_load'、'lb_imbalance_util'、'lb_imbalance_task' 与 'lb_imbalance_misfit'。从这一版本起，domain 字段会打印相应调度域的名称。)

Version 16 of schedstats changed the order of definitions within
'enum cpu_idle_type', which changed the order of [CPU_MAX_IDLE_TYPES]
columns in show_schedstat(). In particular the position of CPU_IDLE
and __CPU_NOT_IDLE changed places. The size of the array is unchanged.
(schedstats 第 16 版改变了 'enum cpu_idle_type' 中定义的顺序，从而改变了 show_schedstat() 中 [CPU_MAX_IDLE_TYPES] 列的顺序。特别是 CPU_IDLE 与 __CPU_NOT_IDLE 的位置互换了。数组的大小不变。)

Version 15 of schedstats dropped counters for some sched_yield:
yld_exp_empty, yld_act_empty and yld_both_empty. Otherwise, it is
identical to version 14. Details are available at
(schedstats 第 15 版去掉了 sched_yield 的一些计数器：yld_exp_empty、yld_act_empty 与 yld_both_empty。除此之外它与第 14 版相同。详情见：)

	https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/scheduler/sched-stats.txt?id=1e1dbb259c79b

Version 14 of schedstats includes support for sched_domains, which hit the
mainline kernel in 2.6.20 although it is identical to the stats from version
12 which was in the kernel from 2.6.13-2.6.19 (version 13 never saw a kernel
release).  Some counters make more sense to be per-runqueue; other to be
per-domain.  Note that domains (and their associated information) will only
be pertinent and available on machines utilizing CONFIG_SMP.
(schedstats 第 14 版加入了对 sched_domain 的支持，该支持在 2.6.20 进入主线内核，不过它与 2.6.13-2.6.19 内核中第 12 版的统计信息完全相同(第 13 版从未随内核发布过)。有些计数器作为每运行队列(per-runqueue)统计更有意义，另一些则作为每域(per-domain)统计更有意义。注意，域(及其相关信息)只在使用 CONFIG_SMP 的机器上才有意义并可用。)

In version 14 of schedstat, there is at least one level of domain
statistics for each cpu listed, and there may well be more than one
domain.  Domains have no particular names in this implementation, but
the highest numbered one typically arbitrates balancing across all the
cpus on the machine, while domain0 is the most tightly focused domain,
sometimes balancing only between pairs of cpus.  At this time, there
are no architectures which need more than three domain levels. The first
field in the domain stats is a bit map indicating which cpus are affected
by that domain. Details are available at
(在 schedstat 第 14 版中，所列出的每个 cpu 都至少有一级域统计，而且很可能不止一个域。在这个实现中，域没有特定的名称，但编号最高的域通常负责在机器的所有 cpu 之间仲裁负载均衡，而 domain0 是关注范围最小的域，有时只在成对的 cpu 之间做均衡。目前没有任何架构需要超过三级的域。域统计中的第一个字段是一个位图，指示受该域影响的 cpu。详情见：)

	https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/sched-stats.txt?id=b762f3ffb797c

The schedstat documentation is maintained version 10 onwards and is not
updated for version 11 and 12. The details for version 10 are available at
(schedstat 文档从第 10 版起开始维护，第 11 版与第 12 版的文档未做更新。第 10 版的详情见：)

	https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/sched-stats.txt?id=1da177e4c3f4

These fields are counters, and only increment.  Programs which make use
of these will need to start with a baseline observation and then calculate
the change in the counters at each subsequent observation.  A perl script
which does this for many of the fields is available at
(这些字段都是计数器，只会递增。使用这些字段的程序需要先取得一个基线观测值，然后在每次后续观测时计算计数器的变化量。一个对其中许多字段执行此操作的 perl 脚本可在以下位置获取：)

    http://eaglet.pdxhosts.com/rick/linux/schedstat/

Note that any such script will necessarily be version-specific, as the main
reason to change versions is changes in the output format.  For those wishing
to write their own scripts, the fields are described here.
(注意，任何此类脚本必然是版本特定的，因为更改版本的主要原因就是输出格式的变化。对于想自己编写脚本的人，这里对各字段加以说明。)

CPU statistics(CPU 统计)
------------------------
cpu<N> 1 2 3 4 5 6 7 8 9

First field is a sched_yield() statistic:
(第一个字段是 sched_yield() 统计：)

     1) # of times sched_yield() was called
        (1) 调用 sched_yield() 的次数)

Next three are schedule() statistics:
(接下来三个是 schedule() 统计：)

     2) This field is a legacy array expiration count field used in the O(1)
	scheduler. We kept it for ABI compatibility, but it is always set to zero.
        (2) 该字段是 O(1) 调度器中使用的遗留数组过期计数字段。为保持 ABI 兼容而保留，但始终为零。)
     3) # of times schedule() was called
        (3) 调用 schedule() 的次数)
     4) # of times schedule() left the processor idle
        (4) schedule() 使处理器空闲的次数)

Next two are try_to_wake_up() statistics:
(接下来两个是 try_to_wake_up() 统计：)

     5) # of times try_to_wake_up() was called
        (5) 调用 try_to_wake_up() 的次数)
     6) # of times try_to_wake_up() was called to wake up the local cpu
        (6) 为唤醒本地 cpu 而调用 try_to_wake_up() 的次数)

Next three are statistics describing scheduling latency:
(接下来三个是描述调度延迟的统计：)

     7) sum of all time spent running by tasks on this processor (in nanoseconds)
        (7) 该处理器上各任务运行所耗时间的总和(单位为纳秒))
     8) sum of all time spent waiting to run by tasks on this processor (in
        nanoseconds)
        (8) 该处理器上各任务等待运行所耗时间的总和(单位为纳秒))
     9) # of timeslices run on this cpu
        (9) 在该 cpu 上运行的时间片数量)


Domain statistics(域统计)
-------------------------
One of these is produced per domain for each cpu described. (Note that if
CONFIG_SMP is not defined, *no* domains are utilized and these lines
will not appear in the output.)
(对所述每个 cpu 的每个域都会生成一条这样的记录。(注意，如果未定义 CONFIG_SMP，则 *不会* 使用任何域，输出中也不会出现这些行。))

domain<N> <name> <cpumask> 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45

The <name> field prints the name of the sched domain and is only supported
with schedstat version >= 17. On previous versions, <cpumask> is the first
field.
(<name> 字段打印调度域的名称，仅在 schedstat 版本 >= 17 时受支持。在更早的版本中，<cpumask> 是第一个字段。)

The <cpumask> field is a bit mask indicating what cpus this domain operates
over.
(<cpumask> 字段是一个位掩码，指示该域作用于哪些 cpu。)

The next 33 are a variety of sched_balance_rq() statistics in grouped into types
of idleness (busy, idle and newly idle):
(接下来的 33 个是 sched_balance_rq() 的各种统计，按空闲状态类型(繁忙 busy、空闲 idle、新近空闲 newly idle)分组：)

    1)  # of times in this domain sched_balance_rq() was called when the
        cpu was busy
        (1) 在该域中，cpu 繁忙时调用 sched_balance_rq() 的次数)
    2)  # of times in this domain sched_balance_rq() checked but found the
        load did not require balancing when busy
        (2) 在该域中，cpu 繁忙时 sched_balance_rq() 做了检查但发现负载无需均衡的次数)
    3)  # of times in this domain sched_balance_rq() tried to move one or
        more tasks and failed, when the cpu was busy
        (3) 在该域中，cpu 繁忙时 sched_balance_rq() 尝试移动一个或多个任务但失败的次数)
    4)  Total imbalance in load in this domain when the cpu was busy
        (4) cpu 繁忙时该域中负载的总失衡量)
    5)  Total imbalance in utilization in this domain when the cpu was busy
        (5) cpu 繁忙时该域中利用率的总失衡量)
    6)  Total imbalance in number of tasks in this domain when the cpu was busy
        (6) cpu 繁忙时该域中任务数的总失衡量)
    7)  Total imbalance due to misfit tasks in this domain when the cpu was
        busy
        (7) cpu 繁忙时该域中因任务错配(misfit)导致的总失衡量)
    8)  # of times in this domain detach_task() was called when busy
        (8) 繁忙时在该域中调用 detach_task() 的次数)
    9)  # of times in this domain detach_task() was called even though the
        target task was cache-hot when busy
        (9) 繁忙时在该域中，即使目标任务缓存热(cache-hot)仍调用 detach_task() 的次数)
    10) # of times in this domain sched_balance_rq() was called but did not
        find a busier queue while the cpu was busy
        (10) 在该域中，cpu 繁忙时调用 sched_balance_rq() 但未找到更繁忙队列的次数)
    11) # of times in this domain a busier queue was found while the cpu
        was busy but no busier group was found
        (11) 在该域中，cpu 繁忙时找到了更繁忙的队列但未找到更繁忙组的次数)

    12) # of times in this domain sched_balance_rq() was called when the
        cpu was idle
        (12) 在该域中，cpu 空闲时调用 sched_balance_rq() 的次数)
    13) # of times in this domain sched_balance_rq() checked but found
        the load did not require balancing when the cpu was idle
        (13) 在该域中，cpu 空闲时 sched_balance_rq() 做了检查但发现负载无需均衡的次数)
    14) # of times in this domain sched_balance_rq() tried to move one or
        more tasks and failed, when the cpu was idle
        (14) 在该域中，cpu 空闲时 sched_balance_rq() 尝试移动一个或多个任务但失败的次数)
    15) Total imbalance in load in this domain when the cpu was idle
        (15) cpu 空闲时该域中负载的总失衡量)
    16) Total imbalance in utilization in this domain when the cpu was idle
        (16) cpu 空闲时该域中利用率的总失衡量)
    17) Total imbalance in number of tasks in this domain when the cpu was idle
        (17) cpu 空闲时该域中任务数的总失衡量)
    18) Total imbalance due to misfit tasks in this domain when the cpu was
        idle
        (18) cpu 空闲时该域中因任务错配导致的总失衡量)
    19) # of times in this domain detach_task() was called when the cpu
        was idle
        (19) cpu 空闲时在该域中调用 detach_task() 的次数)
    20) # of times in this domain detach_task() was called even though
        the target task was cache-hot when idle
        (20) cpu 空闲时在该域中，即使目标任务缓存热仍调用 detach_task() 的次数)
    21) # of times in this domain sched_balance_rq() was called but did
        not find a busier queue while the cpu was idle
        (21) 在该域中，cpu 空闲时调用 sched_balance_rq() 但未找到更繁忙队列的次数)
    22) # of times in this domain a busier queue was found while the
        cpu was idle but no busier group was found
        (22) 在该域中，cpu 空闲时找到了更繁忙的队列但未找到更繁忙组的次数)

    23) # of times in this domain sched_balance_rq() was called when the
        cpu was just becoming idle
        (23) 在该域中，cpu 刚要进入空闲时调用 sched_balance_rq() 的次数)
    24) # of times in this domain sched_balance_rq() checked but found the
        load did not require balancing when the cpu was just becoming idle
        (24) 在该域中，cpu 刚要进入空闲时 sched_balance_rq() 做了检查但发现负载无需均衡的次数)
    25) # of times in this domain sched_balance_rq() tried to move one or more
        tasks and failed, when the cpu was just becoming idle
        (25) 在该域中，cpu 刚要进入空闲时 sched_balance_rq() 尝试移动一个或多个任务但失败的次数)
    26) Total imbalance in load in this domain when the cpu was just becoming
        idle
        (26) cpu 刚要进入空闲时该域中负载的总失衡量)
    27) Total imbalance in utilization in this domain when the cpu was just
        becoming idle
        (27) cpu 刚要进入空闲时该域中利用率的总失衡量)
    28) Total imbalance in number of tasks in this domain when the cpu was just
        becoming idle
        (28) cpu 刚要进入空闲时该域中任务数的总失衡量)
    29) Total imbalance due to misfit tasks in this domain when the cpu was
        just becoming idle
        (29) cpu 刚要进入空闲时该域中因任务错配导致的总失衡量)
    30) # of times in this domain detach_task() was called when newly idle
        (30) 新近空闲时在该域中调用 detach_task() 的次数)
    31) # of times in this domain detach_task() was called even though the
        target task was cache-hot when just becoming idle
        (31) cpu 刚要进入空闲时在该域中，即使目标任务缓存热仍调用 detach_task() 的次数)
    32) # of times in this domain sched_balance_rq() was called but did not
        find a busier queue while the cpu was just becoming idle
        (32) 在该域中，cpu 刚要进入空闲时调用 sched_balance_rq() 但未找到更繁忙队列的次数)
    33) # of times in this domain a busier queue was found while the cpu
        was just becoming idle but no busier group was found
        (33) 在该域中，cpu 刚要进入空闲时找到了更繁忙的队列但未找到更繁忙组的次数)

   Next three are active_load_balance() statistics:
   (接下来三个是 active_load_balance() 统计：)

    34) # of times active_load_balance() was called
        (34) 调用 active_load_balance() 的次数)
    35) # of times active_load_balance() tried to move a task and failed
        (35) active_load_balance() 尝试移动任务但失败的次数)
    36) # of times active_load_balance() successfully moved a task
        (36) active_load_balance() 成功移动任务的次数)

   Next three are sched_balance_exec() statistics:
   (接下来三个是 sched_balance_exec() 统计：)

    37) sbe_cnt is not used
        (37) sbe_cnt 未使用)
    38) sbe_balanced is not used
        (38) sbe_balanced 未使用)
    39) sbe_pushed is not used
        (39) sbe_pushed 未使用)

   Next three are sched_balance_fork() statistics:
   (接下来三个是 sched_balance_fork() 统计：)

    40) sbf_cnt is not used
        (40) sbf_cnt 未使用)
    41) sbf_balanced is not used
        (41) sbf_balanced 未使用)
    42) sbf_pushed is not used
        (42) sbf_pushed 未使用)

   Next three are try_to_wake_up() statistics:
   (接下来三个是 try_to_wake_up() 统计：)

    43) # of times in this domain try_to_wake_up() awoke a task that
        last ran on a different cpu in this domain
        (43) 在该域中，try_to_wake_up() 唤醒上次在该域其他 cpu 上运行的任务的次数)
    44) # of times in this domain try_to_wake_up() moved a task to the
        waking cpu because it was cache-cold on its own cpu anyway
        (44) 在该域中，try_to_wake_up() 因任务在其自身 cpu 上反正也是缓存冷(cache-cold)而把它移动到唤醒 cpu 的次数)
    45) # of times in this domain try_to_wake_up() started passive balancing
        (45) 在该域中，try_to_wake_up() 启动被动均衡的次数)

/proc/<pid>/schedstat(原名)
---------------------------
schedstats also adds a new /proc/<pid>/schedstat file to include some of
the same information on a per-process level.  There are three fields in
this file correlating for that process to:
(schedstats 还新增了 /proc/<pid>/schedstat 文件，以便在每进程层面提供部分相同的信息。该文件中有三个字段，对该进程分别对应：)

     1) time spent on the cpu (in nanoseconds)
        (1) 在 cpu 上花费的时间(单位为纳秒))
     2) time spent waiting on a runqueue (in nanoseconds)
        (2) 在运行队列上等待花费的时间(单位为纳秒))
     3) # of timeslices run on this cpu
        (3) 在该 cpu 上运行的时间片数量)

A program could be easily written to make use of these extra fields to
report on how well a particular process or set of processes is faring
under the scheduler's policies.  A simple version of such a program is
available at
(可以很容易地编写程序来利用这些额外字段，报告特定进程或一组进程在调度器策略下的运行情况。此类程序的一个简单版本可在以下位置获取：)

    http://eaglet.pdxhosts.com/rick/linux/schedstat/v12/latency.c
