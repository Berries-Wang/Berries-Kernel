/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_OSQ_LOCK_H
#define __LINUX_OSQ_LOCK_H

/**
 * [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.4　MCS锁
 * 
 * MCS算法可以解决自旋锁遇到的问题，显著 “缓解” CPU 高速缓存行颠簸问题
 * MCS算法的核心思想是每个锁的申请者只在本地 CPU 的变量上自旋，而不是全局的变量上
 * 
 * 
 * 加锁函数的函数原型: bool osq_lock(struct optimistic_spin_queue *lock); 看看注释
 */

/*
 * An MCS like lock especially tailored for optimistic spinning for sleeping
 * lock implementations (mutex, rwsem, etc).
 * (一种专为睡眠锁实现（互斥锁、读写信号量等）的乐观自旋而量身定制的类MCS锁。)
 */
struct optimistic_spin_node {
	struct optimistic_spin_node *next, *prev;
	int locked; /* 1 if lock acquired */
	int cpu; /* encoded CPU # + 1 value  被哪个CPU锁住了， 这个#+1表示编码，见 'int curr = encode_cpu(smp_processor_id());'*/
};

/**
 * 这个是lock
 */
struct optimistic_spin_queue {
	/*
	 * Stores an encoded value of the CPU # of the tail node in the queue.
	 * If the queue is empty, then it's set to OSQ_UNLOCKED_VAL.
	 * 
	 * 存储的是编码后的CPU编号
	 */
	atomic_t tail;
};

#define OSQ_UNLOCKED_VAL (0)

/* Init macro and function. */
#define OSQ_LOCK_UNLOCKED { ATOMIC_INIT(OSQ_UNLOCKED_VAL) }

static inline void osq_lock_init(struct optimistic_spin_queue *lock)
{
	atomic_set(&lock->tail, OSQ_UNLOCKED_VAL);
}

extern bool osq_lock(struct optimistic_spin_queue *lock);
extern void osq_unlock(struct optimistic_spin_queue *lock);

static inline bool osq_is_locked(struct optimistic_spin_queue *lock)
{
	return atomic_read(&lock->tail) != OSQ_UNLOCKED_VAL;
}

#endif
