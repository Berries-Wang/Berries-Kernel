/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_PRIO_H
#define _LINUX_SCHED_PRIO_H

#define MAX_NICE	19
#define MIN_NICE	-20
#define NICE_WIDTH	(MAX_NICE - MIN_NICE + 1)

/*
 * Priority of a process goes from 0..MAX_PRIO-1, valid RT
 * priority is 0..MAX_RT_PRIO-1, and SCHED_NORMAL/SCHED_BATCH
 * tasks are in the range MAX_RT_PRIO..MAX_PRIO-1. Priority
 * values are inverted: lower p->prio value means higher priority.
 * (进程的优先级范围为 0 至 MAX_PRIO-1，有效的 RT 优先级范围为 0 至 MAX_RT_PRIO-1，
 * SCHED_NORMAL/SCHED_BATCH 任务的优先级范围为 MAX_RT_PRIO 至 MAX_PRIO-1。
 * 优先级值是相反的：p->prio 值越低，优先级越高。)
 *
 * The MAX_USER_RT_PRIO value allows the actual maximum
 * RT priority to be separate from the value exported to
 * user-space.  This allows kernel threads to set their
 * priority to a value higher than any user task. Note:
 * MAX_RT_PRIO must not be smaller than MAX_USER_RT_PRIO.
 * (MAX_USER_RT_PRIO 值允许将实际的最大 RT 优先级与导出到用户空间的值区分开来。
 * 这允许内核线程将其优先级设置为高于任何用户任务的值。
 * 注意：MAX_RT_PRIO 不得小于 MAX_USER_RT_PRIO。)
 */

#define MAX_USER_RT_PRIO	100
#define MAX_RT_PRIO		MAX_USER_RT_PRIO

#define MAX_PRIO		(MAX_RT_PRIO + NICE_WIDTH)
#define DEFAULT_PRIO		(MAX_RT_PRIO + NICE_WIDTH / 2)

/*
 * Convert user-nice values [ -20 ... 0 ... 19 ]
 * to static priority [ MAX_RT_PRIO..MAX_PRIO-1 ],
 * and back.
 */
#define NICE_TO_PRIO(nice)	((nice) + DEFAULT_PRIO)
#define PRIO_TO_NICE(prio)	((prio) - DEFAULT_PRIO)

/*
 * 'User priority' is the nice value converted to something we
 * can work with better when scaling various scheduler parameters,
 * it's a [ 0 ... 39 ] range.
 */
#define USER_PRIO(p)		((p)-MAX_RT_PRIO)
#define TASK_USER_PRIO(p)	USER_PRIO((p)->static_prio)
#define MAX_USER_PRIO		(USER_PRIO(MAX_PRIO))

/*
 * Convert nice value [19,-20] to rlimit style value [1,40].
 */
static inline long nice_to_rlimit(long nice)
{
	return (MAX_NICE - nice + 1);
}

/*
 * Convert rlimit style value [1,40] to nice value [-20, 19].
 */
static inline long rlimit_to_nice(long prio)
{
	return (MAX_NICE - prio + 1);
}

#endif /* _LINUX_SCHED_PRIO_H */
