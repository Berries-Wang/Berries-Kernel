=============================
EEVDF Scheduler(EEVDF 调度器)
=============================

The "Earliest Eligible Virtual Deadline First" (EEVDF) was first introduced
in a scientific publication in 1995 [1]. The Linux kernel began
transitioning to EEVDF in version 6.6 (as a new option in 2024), moving
away from the earlier Completely Fair Scheduler (CFS) in favor of a version
of EEVDF proposed by Peter Zijlstra in 2023 [2-4]. More information
regarding CFS can be found in
Documentation/scheduler/sched-design-CFS.rst.
("最早合格虚拟截止期优先"（Earliest Eligible Virtual Deadline First，
EEVDF）于 1995 年在一篇科学论文中首次提出 [1]。Linux 内核自 6.6 版本起
（2024 年作为新选项）开始向 EEVDF 过渡，放弃早期的完全公平调度器（CFS），
转而采用 Peter Zijlstra 于 2023 年提出的 EEVDF 版本 [2-4]。关于 CFS 的更多
信息可在 Documentation/scheduler/sched-design-CFS.rst 中找到。)

Similarly to CFS, EEVDF aims to distribute CPU time equally among all
runnable tasks with the same priority. To do so, it assigns a virtual run
time to each task, creating a "lag" value that can be used to determine
whether a task has received its fair share of CPU time. In this way, a task
with a positive lag is owed CPU time, while a negative lag means the task
has exceeded its portion. EEVDF picks tasks with lag greater or equal to
zero and calculates a virtual deadline (VD) for each, selecting the task
with the earliest VD to execute next. It's important to note that this
allows latency-sensitive tasks with shorter time slices to be prioritized,
which helps with their responsiveness.
(与 CFS 类似，EEVDF 旨在将 CPU 时间平均分配给所有具有相同优先级的可运行
任务。为此，它为每个任务分配一个虚拟运行时间，产生一个"滞后"（lag）值，可
用于判断任务是否获得了其应得的 CPU 时间份额。这样，滞后为正的任务尚欠 CPU
时间，而滞后为负则意味着任务已超出其份额。EEVDF 选取滞后大于等于零的任务，
并为每个任务计算一个虚拟截止期（VD），选择 VD 最早的任务作为下一个执行者。
值得注意的是，这使得时间片较短的时延敏感型任务能够被优先处理，有助于提升
其响应速度。)

There are ongoing discussions on how to manage lag, especially for sleeping
tasks; but at the time of writing EEVDF uses a "decaying" mechanism based
on virtual run time (VRT). This prevents tasks from exploiting the system
by sleeping briefly to reset their negative lag: when a task sleeps, it
remains on the run queue but marked for "deferred dequeue," allowing its
lag to decay over VRT. Hence, long-sleeping tasks eventually have their lag
reset. Finally, tasks can preempt others if their VD is earlier, and tasks
can request specific time slices using the new sched_setattr() system call,
which further facilitates the job of latency-sensitive applications.
(关于如何管理滞后、尤其是对睡眠任务，目前仍有持续讨论；但在撰写本文时，
EEVDF 使用一种基于虚拟运行时间（VRT）的"衰减"机制。这可以防止任务通过短暂
睡眠来重置其负滞后从而钻系统空子：当任务睡眠时，它仍留在运行队列中，但会被
标记为"延迟出队"，使其滞后随 VRT 衰减。因此，长时间睡眠的任务其滞后最终会
被重置。此外，VD 更早的任务可以抢占其他任务；任务还可以使用新的
sched_setattr() 系统调用请求特定的时间片，这进一步便利了时延敏感型应用。)

REFERENCES(参考文献)
====================

[1] https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=805acf7726282721504c8f00575d91ebfd750564

[2] https://lore.kernel.org/lkml/a79014e6-ea83-b316-1e12-2ae056bda6fa@linux.vnet.ibm.com/

[3] https://lwn.net/Articles/969062/

[4] https://lwn.net/Articles/925371/
