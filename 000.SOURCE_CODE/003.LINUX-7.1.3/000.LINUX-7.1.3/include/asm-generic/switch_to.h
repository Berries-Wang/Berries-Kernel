/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Generic task switch macro wrapper.
 *
 * It should be possible to use these on really simple architectures,
 * but it serves more as a starting point for new ports.
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */
#ifndef __ASM_GENERIC_SWITCH_TO_H
#define __ASM_GENERIC_SWITCH_TO_H

#include <linux/thread_info.h>

/*
 * Context switching is now performed out-of-line in switch_to.S
 */
extern struct task_struct *__switch_to(struct task_struct *,
				       struct task_struct *);

/**
 * 为什么switch_to 需要三个参数? last指向的是什么?
 *  - 假设当前从进程A 切换到进程B
 *   + last 指向的是 switch_to 被执行前的prev进程，即 A进程,是一个输出参数,存储 “谁刚把CPU让给了我”(帮人善后:资源释放)
 *   + 在switch_to执行完成之后，那么CPU就切换到next进程(B)执行了， 切换到next进程(B)的内核栈后，prev 就不一定指向的A了
 * 
 * 执行完成之后，CPU就会切换到next进程执行了
 * 
 */
#define switch_to(prev, next, last)					\
	do {								\
		((last) = __switch_to((prev), (next)));			\
	} while (0)

#endif /* __ASM_GENERIC_SWITCH_TO_H */
