/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Queued spinlock  排队自旋锁
 *
 * (C) Copyright 2013-2015 Hewlett-Packard Development Company, L.P.
 * (C) Copyright 2015 Hewlett-Packard Enterprise Development LP
 *
 * Authors: Waiman Long <waiman.long@hpe.com>
 */
#ifndef __ASM_GENERIC_QSPINLOCK_H
#define __ASM_GENERIC_QSPINLOCK_H

#include <asm-generic/qspinlock_types.h>
#include <linux/atomic.h>

#ifndef queued_spin_is_locked
/**
 * queued_spin_is_locked - is the spinlock locked?
 * @lock: Pointer to queued spinlock structure
 * Return: 1 if it is locked, 0 otherwise
 */
static __always_inline int queued_spin_is_locked(struct qspinlock *lock)
{
	/*
	 * Any !0 state indicates it is locked, even if _Q_LOCKED_VAL
	 * isn't immediately observable.
	 */
	return atomic_read(&lock->val);
}
#endif

/**
 * queued_spin_value_unlocked - is the spinlock structure unlocked?
 * @lock: queued spinlock structure
 * Return: 1 if it is unlocked, 0 otherwise
 *
 * N.B. Whenever there are tasks waiting for the lock, it is considered
 *      locked wrt the lockref code to avoid lock stealing by the lockref
 *      code and change things underneath the lock. This also allows some
 *      optimizations to be applied without conflict with lockref.
 */
static __always_inline int queued_spin_value_unlocked(struct qspinlock lock)
{
	return !atomic_read(&lock.val);
}

/**
 * queued_spin_is_contended - check if the lock is contended
 * @lock : Pointer to queued spinlock structure
 * Return: 1 if lock contended, 0 otherwise
 */
static __always_inline int queued_spin_is_contended(struct qspinlock *lock)
{
	return atomic_read(&lock->val) & ~_Q_LOCKED_MASK;
}
/**
 * queued_spin_trylock - try to acquire the queued spinlock
 * @lock : Pointer to queued spinlock structure
 * Return: 1 if lock acquired, 0 if failed
 */
static __always_inline int queued_spin_trylock(struct qspinlock *lock)
{
	u32 val = atomic_read(&lock->val);

	if (unlikely(val))
		return 0;

	return likely(atomic_try_cmpxchg_acquire(&lock->val, &val, _Q_LOCKED_VAL));
}

extern void queued_spin_lock_slowpath(struct qspinlock *lock, u32 val);

#ifndef queued_spin_lock
/**
 * 
 * 内核5.x+是排队自旋锁，参考:[Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.3　经典自旋锁
 * 
 * queued_spin_lock - acquire a queued spinlock (获取排队自旋锁)
 * @lock: Pointer to queued spinlock structure
 * 
 * 
 * ### arm64:
 * #0  arch_static_branch_jump (branch=<optimized out>, key=<optimized out>) at ./arch/arm64/include/asm/jump_label.h:38
 * #1  system_uses_lse_atomics () at ./arch/arm64/include/asm/lse.h:28
 * #2  __cmpxchg_case_acq_32 (new=<optimized out>, old=<optimized out>, ptr=<optimized out>) at ./arch/arm64/include/asm/cmpxchg.h:121
 * #3  __cmpxchg_acq (size=<optimized out>, new=<optimized out>, old=<optimized out>, ptr=<optimized out>) at ./arch/arm64/include/asm/cmpxchg.h:173
 * #4  arch_atomic_try_cmpxchg_acquire (new=<optimized out>, old=<optimized out>, v=<optimized out>) at ./include/linux/atomic-arch-fallback.h:933
 * #5  atomic_try_cmpxchg_acquire (new=<optimized out>, old=<optimized out>, v=<optimized out>) at ./include/asm-generic/atomic-instrumented.h:707
 * #6  queued_spin_lock (lock=<optimized out>) at ./include/asm-generic/qspinlock.h:82
 * #7  do_raw_spin_lock (lock=<optimized out>) at ./include/linux/spinlock.h:183
 * #8  __raw_spin_lock (lock=0xffff800011e2abd8 <logbuf_lock>) at ./include/linux/spinlock_api_smp.h:154
 *
 */
static __always_inline void queued_spin_lock(struct qspinlock *lock)
{
	u32 val = 0;

	/**
	 * 先加乐观锁 , 注意，这是给自旋锁 （后面的是MCS锁）， 获取到了MCS锁不代表可以获取自旋锁
	 */
	if (likely(atomic_try_cmpxchg_acquire(&lock->val, &val, _Q_LOCKED_VAL))) {
		return;
	}

	// 没有获取到锁，进入队列等待获取锁 (CPU0已获取锁，CPU1尝试获取锁): qspinlock.c 
	queued_spin_lock_slowpath(lock, val);
}
#endif

#ifndef queued_spin_unlock
/**
 * queued_spin_unlock - release a queued spinlock
 * @lock : Pointer to queued spinlock structure
 */
static __always_inline void queued_spin_unlock(struct qspinlock *lock)
{
	/*
	 * unlock() needs release semantics:
	 */
	smp_store_release(&lock->locked, 0);
}
#endif

#ifndef virt_spin_lock
static __always_inline bool virt_spin_lock(struct qspinlock *lock)
{
	return false;
}
#endif

/*
 * Remapping spinlock architecture specific functions to the corresponding
 * queued spinlock functions.
 */
#define arch_spin_is_locked(l)		queued_spin_is_locked(l)
#define arch_spin_is_contended(l)	queued_spin_is_contended(l)
#define arch_spin_value_unlocked(l)	queued_spin_value_unlocked(l)
#define arch_spin_lock(l)		queued_spin_lock(l)
#define arch_spin_trylock(l)		queued_spin_trylock(l)
#define arch_spin_unlock(l)		queued_spin_unlock(l)

#endif /* __ASM_GENERIC_QSPINLOCK_H */
