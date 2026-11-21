=======================================
Scheduler Nice Design(调度器 nice 设计)
=======================================

This document explains the thinking about the revamped and streamlined
nice-levels implementation in the new Linux scheduler.
(本文档解释了新 Linux 调度器中经过改造和精简的 nice 级别实现背后的思路。)

Nice levels were always pretty weak under Linux and people continuously
pestered us to make nice +19 tasks use up much less CPU time.
(nice 级别在 Linux 下一直都比较弱，人们不断纠缠我们，要求让 nice +19 的任务占用少得多的 CPU 时间。)

Unfortunately that was not that easy to implement under the old
scheduler, (otherwise we'd have done it long ago) because nice level
support was historically coupled to timeslice length, and timeslice
units were driven by the HZ tick, so the smallest timeslice was 1/HZ.
(遗憾的是，在旧调度器下实现这一点并不那么容易（否则我们早就做了），因为 nice 级别支持在历史上与时间片长度耦合在一起，而时间片单位由 HZ 节拍驱动，所以最小时间片是 1/HZ。)

In the O(1) scheduler (in 2003) we changed negative nice levels to be
much stronger than they were before in 2.4 (and people were happy about
that change), and we also intentionally calibrated the linear timeslice
rule so that nice +19 level would be _exactly_ 1 jiffy. To better
understand it, the timeslice graph went like this (cheesy ASCII art
alert!)::


                   A
             \     | [timeslice length]
              \    |
               \   |
                \  |
                 \ |
                  \|___100msecs
                   |^ . _
                   |      ^ . _
                   |            ^ . _
 -*----------------------------------*-----> [nice level]
 -20               |                +19
                   |
                   |

(在 O(1) 调度器中（2003 年），我们把负 nice 级别改得比 2.4 中强得多（人们对这一改动很满意），并且我们还故意校准了线性时间片规则，使 nice +19 级别正好是 1 个 jiffy。为了更好地理解它，时间片图形如下所示（请注意这幅蹩脚的 ASCII 图！）：)

So that if someone wanted to really renice tasks, +19 would give a much
bigger hit than the normal linear rule would do. (The solution of
changing the ABI to extend priorities was discarded early on.)
(这样一来，如果有人想真正调整任务的 nice 值，+19 会比正常的线性规则造成大得多的削减。（通过更改 ABI 来扩展优先级的方案很早就被否决了。）)

This approach worked to some degree for some time, but later on with
HZ=1000 it caused 1 jiffy to be 1 msec, which meant 0.1% CPU usage which
we felt to be a bit excessive. Excessive _not_ because it's too small of
a CPU utilization, but because it causes too frequent (once per
millisec) rescheduling. (and would thus trash the cache, etc. Remember,
this was long ago when hardware was weaker and caches were smaller, and
people were running number crunching apps at nice +19.)
(这种方法在一定程度上起了一段时间的作用，但后来在 HZ=1000 时它导致 1 个 jiffy 变成 1 毫秒，这意味着 0.1% 的 CPU 占用率，我们觉得这有点过头了。说它过头并不是因为 CPU 利用率太小，而是因为它导致过于频繁的（每毫秒一次）重新调度。（因此会破坏缓存等。记住，这是很久以前的事了，那时硬件更弱、缓存更小，而人们在 nice +19 下运行数值计算应用程序。）)

So for HZ=1000 we changed nice +19 to 5msecs, because that felt like the
right minimal granularity - and this translates to 5% CPU utilization.
But the fundamental HZ-sensitive property for nice+19 still remained,
and we never got a single complaint about nice +19 being too _weak_ in
terms of CPU utilization, we only got complaints about it (still) being
too _strong_ :-)
(所以对于 HZ=1000，我们把 nice +19 改为 5 毫秒，因为那感觉像是最合适的最小粒度——这相当于 5% 的 CPU 利用率。但 nice+19 的这种 HZ 敏感特性仍然存在，而且我们从没有收到过哪怕一条关于 nice +19 在 CPU 利用率方面太弱的抱怨，我们收到的只是关于它（仍然）太强的抱怨 :-)）

To sum it up: we always wanted to make nice levels more consistent, but
within the constraints of HZ and jiffies and their nasty design level
coupling to timeslices and granularity it was not really viable.
(总而言之：我们一直想让 nice 级别更加一致，但在 HZ 和 jiffies 的约束以及它们与时间片和粒度的讨厌的设计级耦合之下，这并不真正可行。)

The second (less frequent but still periodically occurring) complaint
about Linux's nice level support was its asymmetry around the origin
(which you can see demonstrated in the picture above), or more
accurately: the fact that nice level behavior depended on the _absolute_
nice level as well, while the nice API itself is fundamentally
"relative":
(对 Linux nice 级别支持的第二个（不那么频繁但仍会周期性出现的）抱怨是它在原点附近的不对称性（你可以在上图中看到），或者更准确地说：nice 级别行为还取决于绝对 nice 级别这一事实，而 nice API 本身从根本上就是"相对"的：)

   int nice(int inc);

   asmlinkage long sys_nice(int increment)

(the first one is the glibc API, the second one is the syscall API.)
Note that the 'inc' is relative to the current nice level. Tools like
bash's "nice" command mirror this relative API.
((前者是 glibc API，后者是系统调用 API。)注意 'inc' 是相对于当前 nice 级别的。像 bash 的 "nice" 命令之类的工具沿用了这种相对 API。)

With the old scheduler, if you for example started a niced task with +1
and another task with +2, the CPU split between the two tasks would
depend on the nice level of the parent shell - if it was at nice -10 the
CPU split was different than if it was at +5 or +10.
(使用旧调度器时，例如你启动了一个 nice 为 +1 的任务和另一个 nice 为 +2 的任务，这两个任务之间的 CPU 分配将取决于父 shell 的 nice 级别——如果它处于 nice -10，CPU 分配会与它处于 +5 或 +10 时不同。)

A third complaint against Linux's nice level support was that negative
nice levels were not 'punchy enough', so lots of people had to resort to
run audio (and other multimedia) apps under RT priorities such as
SCHED_FIFO. But this caused other problems: SCHED_FIFO is not starvation
proof, and a buggy SCHED_FIFO app can also lock up the system for good.
(对 Linux nice 级别支持的第三个抱怨是负 nice 级别"不够有力"，所以很多人不得不求助于在 SCHED_FIFO 等 RT 优先级下运行音频（和其他多媒体）应用程序。但这带来了其他问题：SCHED_FIFO 不能防止饥饿，而且有缺陷的 SCHED_FIFO 应用程序也可能把系统永久锁死。)

The new scheduler in v2.6.23 addresses all three types of complaints:
(v2.6.23 中的新调度器解决了所有这三类抱怨：)

To address the first complaint (of nice levels being not "punchy"
enough), the scheduler was decoupled from 'time slice' and HZ concepts
(and granularity was made a separate concept from nice levels) and thus
it was possible to implement better and more consistent nice +19
support: with the new scheduler nice +19 tasks get a HZ-independent
1.5%, instead of the variable 3%-5%-9% range they got in the old
scheduler.
(为了解决第一个抱怨（nice 级别不够"有力"），调度器与'时间片'和 HZ 概念解耦（粒度被做成与 nice 级别分开的概念），因此可以实现更好、更一致的 nice +19 支持：在新调度器下，nice +19 任务获得与 HZ 无关的 1.5%，而不是旧调度器中 3%-5%-9% 的可变范围。)

To address the second complaint (of nice levels not being consistent),
the new scheduler makes nice(1) have the same CPU utilization effect on
tasks, regardless of their absolute nice levels. So on the new
scheduler, running a nice +10 and a nice 11 task has the same CPU
utilization "split" between them as running a nice -5 and a nice -4
task. (one will get 55% of the CPU, the other 45%.) That is why nice
levels were changed to be "multiplicative" (or exponential) - that way
it does not matter which nice level you start out from, the 'relative
result' will always be the same.
(为了解决第二个抱怨（nice 级别不一致），新调度器使 nice(1) 对任务的 CPU 利用率影响相同，而不管它们的绝对 nice 级别如何。因此在新调度器上，运行一个 nice +10 和一个 nice 11 的任务，它们之间的 CPU 利用率"分配"与运行一个 nice -5 和一个 nice -4 的任务相同。（一个获得 55% 的 CPU，另一个获得 45%。）这就是为什么 nice 级别被改为"乘法"（或指数）式——这样无论你从哪个 nice 级别开始，"相对结果"总是相同的。)

The third complaint (of negative nice levels not being "punchy" enough
and forcing audio apps to run under the more dangerous SCHED_FIFO
scheduling policy) is addressed by the new scheduler almost
automatically: stronger negative nice levels are an automatic
side-effect of the recalibrated dynamic range of nice levels.
(第三个抱怨（负 nice 级别不够"有力"、迫使音频应用程序在更危险的 SCHED_FIFO 调度策略下运行）几乎被新调度器自动解决：更强有力的负 nice 级别是重新校准后的 nice 级别动态范围的自动副作用。)
