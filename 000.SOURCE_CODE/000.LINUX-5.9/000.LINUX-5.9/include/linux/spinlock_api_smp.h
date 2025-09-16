#ifndef __LINUX_SPINLOCK_API_SMP_H
#define __LINUX_SPINLOCK_API_SMP_H

#ifndef __LINUX_SPINLOCK_H
# error "please don't include this file directly"
#endif

/*
 * include/linux/spinlock_api_smp.h
 *
 * spinlock API declarations on SMP (and debug)
 * (implemented in kernel/spinlock.c)
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

int in_lock_functions(unsigned long addr);

#define assert_raw_spin_locked(x)	BUG_ON(!raw_spin_is_locked(x))

void __lockfunc _raw_spin_lock(raw_spinlock_t *lock)		__acquires(lock);
void __lockfunc _raw_spin_lock_nested(raw_spinlock_t *lock, int subclass)
								__acquires(lock);
void __lockfunc
_raw_spin_lock_nest_lock(raw_spinlock_t *lock, struct lockdep_map *map)
								__acquires(lock);
void __lockfunc _raw_spin_lock_bh(raw_spinlock_t *lock)		__acquires(lock);
void __lockfunc _raw_spin_lock_irq(raw_spinlock_t *lock)
								__acquires(lock);

unsigned long __lockfunc _raw_spin_lock_irqsave(raw_spinlock_t *lock)
								__acquires(lock);
unsigned long __lockfunc
_raw_spin_lock_irqsave_nested(raw_spinlock_t *lock, int subclass)
								__acquires(lock);
int __lockfunc _raw_spin_trylock(raw_spinlock_t *lock);
int __lockfunc _raw_spin_trylock_bh(raw_spinlock_t *lock);
void __lockfunc _raw_spin_unlock(raw_spinlock_t *lock)		__releases(lock);
void __lockfunc _raw_spin_unlock_bh(raw_spinlock_t *lock)	__releases(lock);
void __lockfunc _raw_spin_unlock_irq(raw_spinlock_t *lock)	__releases(lock);
void __lockfunc
_raw_spin_unlock_irqrestore(raw_spinlock_t *lock, unsigned long flags)
								__releases(lock);

#ifdef CONFIG_INLINE_SPIN_LOCK
#define _raw_spin_lock(lock) __raw_spin_lock(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_LOCK_BH
#define _raw_spin_lock_bh(lock) __raw_spin_lock_bh(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_LOCK_IRQ
#define _raw_spin_lock_irq(lock) __raw_spin_lock_irq(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_LOCK_IRQSAVE
#define _raw_spin_lock_irqsave(lock) __raw_spin_lock_irqsave(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_TRYLOCK
#define _raw_spin_trylock(lock) __raw_spin_trylock(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_TRYLOCK_BH
#define _raw_spin_trylock_bh(lock) __raw_spin_trylock_bh(lock)
#endif

#ifndef CONFIG_UNINLINE_SPIN_UNLOCK
#define _raw_spin_unlock(lock) __raw_spin_unlock(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_UNLOCK_BH
#define _raw_spin_unlock_bh(lock) __raw_spin_unlock_bh(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_UNLOCK_IRQ
#define _raw_spin_unlock_irq(lock) __raw_spin_unlock_irq(lock)
#endif

#ifdef CONFIG_INLINE_SPIN_UNLOCK_IRQRESTORE
#define _raw_spin_unlock_irqrestore(lock, flags) __raw_spin_unlock_irqrestore(lock, flags)
#endif

static inline int __raw_spin_trylock(raw_spinlock_t *lock)
{
	preempt_disable();
	if (do_raw_spin_trylock(lock)) {
		spin_acquire(&lock->dep_map, 0, 1, _RET_IP_);
		return 1;
	}
	preempt_enable();
	return 0;
}

/*
 * If lockdep is enabled then we use the non-preemption spin-ops
 * even on CONFIG_PREEMPTION, because lockdep assumes that interrupts are
 * not re-enabled during lock-acquire (which the preempt-spin-ops do):
 */
#if !defined(CONFIG_GENERIC_LOCKBREAK) || defined(CONFIG_DEBUG_LOCK_ALLOC)

/**
 *  _raw_spin_lock_irqsave() 是一个底层自旋锁函数，用于在禁用本地中断的情况下获取自旋锁，
 * 确保临界区的原子性和安全性，尤其适用于中断上下文或要求严格时序的场景
 */
static inline unsigned long __raw_spin_lock_irqsave(raw_spinlock_t *lock)
{
	unsigned long flags;

	// 保存当前中断状态并禁用本地 CPU 的中断
	local_irq_save(flags);
	// 禁止内核抢占
	preempt_disable();

	// arm64中，这其实是一个空的
	spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);

	/*
	 * On lockdep we dont want the hand-coded irq-enable of
	 * do_raw_spin_lock_flags() code, because lockdep assumes
	 * that interrupts are not re-enabled during lock-acquire:
	 */
#ifdef CONFIG_LOCKDEP
	LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
#else
	do_raw_spin_lock_flags(lock, &flags);
#endif
	return flags;
}

static inline void __raw_spin_lock_irq(raw_spinlock_t *lock)
{
	// 关闭本地CPU中断
	local_irq_disable();
	preempt_disable();
	spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
	LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}

static inline void __raw_spin_lock_bh(raw_spinlock_t *lock)
{
	__local_bh_disable_ip(_RET_IP_, SOFTIRQ_LOCK_OFFSET);
	spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
	LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}


/**
 * 自旋锁实现
 */
static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
	// 关闭内核抢占
	preempt_disable();

	/**
	 * 操作的是run_queue的锁的属性: 
	 * arm64: include/linux/lockdep.h:488:  #define spin_acquire(l, s, t, i)      lock_acquire_exclusive(l, s, t, NULL, i)
	 * 
	 * arm64中，这行代码没有了
	 */
	spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);

	// 直接调用do_raw_spin_lock: 000.LINUX-5.9/include/linux/spinlock.h
	LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}

#endif /* !CONFIG_GENERIC_LOCKBREAK || CONFIG_DEBUG_LOCK_ALLOC */

static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	spin_release(&lock->dep_map, _RET_IP_);
	do_raw_spin_unlock(lock);
	preempt_enable();
}

static inline void __raw_spin_unlock_irqrestore(raw_spinlock_t *lock,
					    unsigned long flags)
{
	spin_release(&lock->dep_map, _RET_IP_);
	do_raw_spin_unlock(lock);
	local_irq_restore(flags);
	preempt_enable();
}

static inline void __raw_spin_unlock_irq(raw_spinlock_t *lock)
{
	spin_release(&lock->dep_map, _RET_IP_);
	do_raw_spin_unlock(lock);
	local_irq_enable();
	preempt_enable();
}

static inline void __raw_spin_unlock_bh(raw_spinlock_t *lock)
{
	spin_release(&lock->dep_map, _RET_IP_);
	do_raw_spin_unlock(lock);
	__local_bh_enable_ip(_RET_IP_, SOFTIRQ_LOCK_OFFSET);
}

static inline int __raw_spin_trylock_bh(raw_spinlock_t *lock)
{
	__local_bh_disable_ip(_RET_IP_, SOFTIRQ_LOCK_OFFSET);
	if (do_raw_spin_trylock(lock)) {
		spin_acquire(&lock->dep_map, 0, 1, _RET_IP_);
		return 1;
	}
	__local_bh_enable_ip(_RET_IP_, SOFTIRQ_LOCK_OFFSET);
	return 0;
}

#include <linux/rwlock_api_smp.h>

#endif /* __LINUX_SPINLOCK_API_SMP_H */
