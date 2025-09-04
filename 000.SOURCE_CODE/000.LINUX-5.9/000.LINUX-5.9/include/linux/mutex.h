/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Mutexes: blocking mutual exclusion locks
 *
 * started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005, 2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * This file contains the main data structure and API definitions.
 */
#ifndef __LINUX_MUTEX_H
#define __LINUX_MUTEX_H

#include <asm/current.h>
#include <linux/list.h>
#include <linux/spinlock_types.h>
#include <linux/lockdep.h>
#include <linux/atomic.h>
#include <asm/processor.h>
#include <linux/osq_lock.h>
#include <linux/debug_locks.h>

struct ww_acquire_ctx;

/**
 * 互斥锁
 * 
 * Simple, straightforward mutexes with strict semantics:(简单直接、具有严格语义的互斥锁。)
 *
 * - only one task can hold the mutex at a time
 * - only the owner can unlock the mutex
 * - multiple unlocks are not permitted
 * - recursive(递归) locking is not permitted
 * - a mutex object must be initialized via the API
 * - a mutex object must not be initialized via memset or copying
 * - task may not exit with mutex held (任务不得持有互斥锁退出)
 * - memory areas where held locks reside must not be freed (不得释放持有锁的内存区域)
 * - held mutexes must not be reinitialized (不得重新初始化已持有的互斥锁)
 * - mutexes may not be used in hardware or software interrupt
 *   contexts such as tasklets and timers (禁止在硬件或软件中断上下文中使用互斥锁，例如任务粒度和定时器)
 *
 * These semantics are fully enforced when DEBUG_MUTEXES is
 * enabled. Furthermore, besides enforcing the above rules, the mutex
 * debugging code also implements a number of additional features
 * that make lock debugging easier and faster:
 * (当启用DEBUG_MUTEXES（调试互斥锁）功能时，这些语义规则将得到全面强制执行。
 * 此外，除了强制实施上述规则外，互斥锁调试代码还实现了若干附加功能，这些功能能够使锁调试工作更易于进行且更高效。)
 *  启用 DEBUG_MUTEXES ， 就需要修改.config文件
 * 
 * - uses symbolic names of mutexes, whenever they are printed in debug output
 *   (在调试输出中需要显示互斥锁名称时，系统会使用其符号名称进行展示)
 * 
 * - point-of-acquire tracking, symbolic lookup of function names
 *   (获取点追踪功能，函数名称的符号化查询)
 * 
 * - list of all locks held in the system, printout of them
 *   
 * - owner tracking(所有者追踪)
 * - detects self-recursing locks and prints out all relevant info 
 *   (检测自递归锁并输出所有相关信息)
 * 
 * - detects multi-task circular deadlocks and prints out all affected
 *   locks and tasks (and only those tasks)
 *   (检测多任务循环死锁，并精确输出所有受影响的锁及任务（仅包含相关任务）)
 */
struct mutex {
	/**
	 * 0表示锁没有未被持有，非零值则表示锁持有者的task_struct指针的值。另外，最低3位有特殊的含义
	 */
	atomic_long_t owner;
	/**
	 * 自旋锁，用于保护 wait_list睡眠等待队列
	 * > 其实也是保证owner，只有获取到了wait_lock,才能去修改owner，也是一种保护
	 * > > 见 void mutex_lock(struct mutex *lock) 方法实现
	 */
	spinlock_t wait_lock;
#ifdef CONFIG_MUTEX_SPIN_ON_OWNER
	/**
     * 用于实现MCS锁机制
     */
	struct optimistic_spin_queue osq; /* Spinner MCS lock */
#endif
	/**
    * 用于管理所有在互斥锁上睡眠的进程，没有获取到锁的进程会在此链上睡眠 
    */
	struct list_head wait_list;
#ifdef CONFIG_DEBUG_MUTEXES
	void *magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
};

struct ww_class;
struct ww_acquire_ctx;

/**
 * ww_mutex（Wait/Wound Mutex，等待/创伤互斥锁），也称为 Wound-Wait 锁，是 Linux 内核中一种特殊的互斥锁，它用于实现对多个锁的原子获取，并内置了死锁避免/处理机制
 * 即 当死锁发生的时候，ww_mutex能使其中一方(B)主动让出自己持有的锁，那么另一个任务(A)能够正常执行；当任务A执行完成后，任务B也可以获取锁继续执行
 * 
 * 这个机制是怎么实现的?
 */
struct ww_mutex {
	struct mutex base;
	struct ww_acquire_ctx *ctx;
#ifdef CONFIG_DEBUG_MUTEXES
	struct ww_class *ww_class;
#endif
};

/*
 * This is the control structure for tasks blocked on mutex,
 * which resides on the blocked task's kernel stack:
 */
struct mutex_waiter {
	struct list_head	list;
	struct task_struct	*task;
	struct ww_acquire_ctx	*ww_ctx;
#ifdef CONFIG_DEBUG_MUTEXES
	void			*magic;
#endif
};

#ifdef CONFIG_DEBUG_MUTEXES

#define __DEBUG_MUTEX_INITIALIZER(lockname)				\
	, .magic = &lockname

extern void mutex_destroy(struct mutex *lock);

#else

# define __DEBUG_MUTEX_INITIALIZER(lockname)

static inline void mutex_destroy(struct mutex *lock) {}

#endif

/**
 * 互斥锁初始化-方式2
 * mutex_init - initialize the mutex
 * @mutex: the mutex to be initialized
 *
 * Initialize the mutex to unlocked state.
 *
 * It is not allowed to initialize an already locked mutex.
 */
#define mutex_init(mutex)						\
do {									\
	static struct lock_class_key __key;				\
									\
	__mutex_init((mutex), #mutex, &__key);				\
} while (0)

#ifdef CONFIG_DEBUG_LOCK_ALLOC
# define __DEP_MAP_MUTEX_INITIALIZER(lockname)			\
		, .dep_map = {					\
			.name = #lockname,			\
			.wait_type_inner = LD_WAIT_SLEEP,	\
		}
#else
# define __DEP_MAP_MUTEX_INITIALIZER(lockname)
#endif

#define __MUTEX_INITIALIZER(lockname) \
		{ .owner = ATOMIC_LONG_INIT(0) \
		, .wait_lock = __SPIN_LOCK_UNLOCKED(lockname.wait_lock) \
		, .wait_list = LIST_HEAD_INIT(lockname.wait_list) \
		__DEBUG_MUTEX_INITIALIZER(lockname) \
		__DEP_MAP_MUTEX_INITIALIZER(lockname) }

/**
 * 互斥锁初始化-方式1
 */
#define DEFINE_MUTEX(mutexname) \
	struct mutex mutexname = __MUTEX_INITIALIZER(mutexname)

extern void __mutex_init(struct mutex *lock, const char *name,
			 struct lock_class_key *key);

/**
 * mutex_is_locked - is the mutex locked
 * @lock: the mutex to be queried
 *
 * Returns true if the mutex is locked, false if unlocked.
 */
extern bool mutex_is_locked(struct mutex *lock);

/*
 * See kernel/locking/mutex.c for detailed documentation of these APIs.
 * Also see Documentation/locking/mutex-design.rst.
 */
#ifdef CONFIG_DEBUG_LOCK_ALLOC
extern void mutex_lock_nested(struct mutex *lock, unsigned int subclass);
extern void _mutex_lock_nest_lock(struct mutex *lock, struct lockdep_map *nest_lock);

extern int __must_check mutex_lock_interruptible_nested(struct mutex *lock,
					unsigned int subclass);
extern int __must_check mutex_lock_killable_nested(struct mutex *lock,
					unsigned int subclass);
extern void mutex_lock_io_nested(struct mutex *lock, unsigned int subclass);

#define mutex_lock(lock) mutex_lock_nested(lock, 0)
#define mutex_lock_interruptible(lock) mutex_lock_interruptible_nested(lock, 0)
#define mutex_lock_killable(lock) mutex_lock_killable_nested(lock, 0)
#define mutex_lock_io(lock) mutex_lock_io_nested(lock, 0)

#define mutex_lock_nest_lock(lock, nest_lock)				\
do {									\
	typecheck(struct lockdep_map *, &(nest_lock)->dep_map);	\
	_mutex_lock_nest_lock(lock, &(nest_lock)->dep_map);		\
} while (0)

#else
extern void mutex_lock(struct mutex *lock);
extern int __must_check mutex_lock_interruptible(struct mutex *lock);
extern int __must_check mutex_lock_killable(struct mutex *lock);
extern void mutex_lock_io(struct mutex *lock);

# define mutex_lock_nested(lock, subclass) mutex_lock(lock)
# define mutex_lock_interruptible_nested(lock, subclass) mutex_lock_interruptible(lock)
# define mutex_lock_killable_nested(lock, subclass) mutex_lock_killable(lock)
# define mutex_lock_nest_lock(lock, nest_lock) mutex_lock(lock)
# define mutex_lock_io_nested(lock, subclass) mutex_lock(lock)
#endif

/*
 * NOTE: mutex_trylock() follows the spin_trylock() convention,
 *       not the down_trylock() convention!
 *
 * Returns 1 if the mutex has been acquired successfully, and 0 on contention.
 */
extern int mutex_trylock(struct mutex *lock);
extern void mutex_unlock(struct mutex *lock);

extern int atomic_dec_and_mutex_lock(atomic_t *cnt, struct mutex *lock);

/*
 * These values are chosen such that FAIL and SUCCESS match the
 * values of the regular mutex_trylock().
 */
enum mutex_trylock_recursive_enum {
	MUTEX_TRYLOCK_FAILED    = 0,
	MUTEX_TRYLOCK_SUCCESS   = 1,
	MUTEX_TRYLOCK_RECURSIVE,
};

/**
 * mutex_trylock_recursive - trylock variant that allows recursive locking
 * @lock: mutex to be locked
 *
 * This function should not be used, _ever_. It is purely for hysterical GEM
 * raisins, and once those are gone this will be removed.
 *
 * Returns:
 *  - MUTEX_TRYLOCK_FAILED    - trylock failed,
 *  - MUTEX_TRYLOCK_SUCCESS   - lock acquired,
 *  - MUTEX_TRYLOCK_RECURSIVE - we already owned the lock.
 */
extern /* __deprecated */ __must_check enum mutex_trylock_recursive_enum
mutex_trylock_recursive(struct mutex *lock);

#endif /* __LINUX_MUTEX_H */
