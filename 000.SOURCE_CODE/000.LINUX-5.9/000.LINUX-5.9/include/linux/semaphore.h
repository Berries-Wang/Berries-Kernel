/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Please see kernel/locking/semaphore.c for documentation of these functions
 */
#ifndef __LINUX_SEMAPHORE_H
#define __LINUX_SEMAPHORE_H

#include <linux/list.h>
#include <linux/spinlock.h>

/**
 * 信号量是一个计数器，它支持两个操作原语，即P和V操作。P和V原指荷兰语中的两个单词，分别表示减少和增加，后来美国人把它改成down和up，现在Linux内核中也叫这两个名字
 * 具体逻辑是什么?
 * 
 * 
 */
/* Please don't access any members of this structure directly */
struct semaphore {
	raw_spinlock_t		lock;    // 自旋锁变量，保护semaphore中的count 和 wait_list成员
	unsigned int		count;   // 允许进入临界区的内核执行路径个数
	struct list_head	wait_list; // 管理所有在该信号量上睡眠的进程，没有成功获取锁的进程会在这个链表上睡眠
};

#define __SEMAPHORE_INITIALIZER(name, n)				\
{									\
	.lock		= __RAW_SPIN_LOCK_UNLOCKED((name).lock),	\
	.count		= n,						\
	.wait_list	= LIST_HEAD_INIT((name).wait_list),		\
}

#define DEFINE_SEMAPHORE(name)	\
	struct semaphore name = __SEMAPHORE_INITIALIZER(name, 1)

/**
 * 信号量初始化
 */
static inline void sema_init(struct semaphore *sem, int val)
{
	static struct lock_class_key __key;
	*sem = (struct semaphore) __SEMAPHORE_INITIALIZER(*sem, val);
	lockdep_init_map(&sem->lock.dep_map, "semaphore->lock", &__key, 0);
}

/**
 * down_interruptible()函数在争用信号量失败时进入可中断的睡眠状态，而down()函数进入不可中断的睡眠状态。
 */
extern void down(struct semaphore *sem);
extern int __must_check down_interruptible(struct semaphore *sem);
extern int __must_check down_killable(struct semaphore *sem);
/**
 * 若down_trylock()函数返回0，表示成功获取锁；若返回1，表示获取锁失败
 */
extern int __must_check down_trylock(struct semaphore *sem);
extern int __must_check down_timeout(struct semaphore *sem, long jiffies);
extern void up(struct semaphore *sem);

#endif /* __LINUX_SEMAPHORE_H */
