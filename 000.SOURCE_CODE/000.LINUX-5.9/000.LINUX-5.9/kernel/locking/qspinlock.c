// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Queued spinlock
 *
 * (C) Copyright 2013-2015 Hewlett-Packard Development Company, L.P.
 * (C) Copyright 2013-2014,2018 Red Hat, Inc.
 * (C) Copyright 2015 Intel Corp.
 * (C) Copyright 2015 Hewlett-Packard Enterprise Development LP
 *
 * Authors: Waiman Long <longman@redhat.com>
 *          Peter Zijlstra <peterz@infradead.org>
 */

#ifndef _GEN_PV_LOCK_SLOWPATH

#include <linux/smp.h>
#include <linux/bug.h>
#include <linux/cpumask.h>
#include <linux/percpu.h>
#include <linux/hardirq.h>
#include <linux/mutex.h>
#include <linux/prefetch.h>
#include <asm/byteorder.h>
#include <asm/qspinlock.h>

/*
 * Include queued spinlock statistics code
 */
#include "qspinlock_stat.h"

/*
 * The basic principle of a queue-based spinlock can best be understood
 * by studying a classic queue-based spinlock implementation called the
 * MCS lock. A copy of the original MCS lock paper ("Algorithms for Scalable
 * Synchronization on Shared-Memory Multiprocessors by Mellor-Crummey and
 * Scott") is available at
 *
 * https://bugzilla.kernel.org/show_bug.cgi?id=206115
 *
 * This queued spinlock implementation is based on the MCS lock, however to
 * make it fit the 4 bytes we assume spinlock_t to be, and preserve its
 * existing API, we must modify it somehow.
 *
 * In particular; where the traditional MCS lock consists of a tail pointer
 * (8 bytes) and needs the next pointer (another 8 bytes) of its own node to
 * unlock the next pending (next->locked), we compress both these: {tail,
 * next->locked} into a single u32 value.
 *
 * Since a spinlock disables recursion of its own context and there is a limit
 * to the contexts that can nest; namely: task, softirq, hardirq, nmi. As there
 * are at most 4 nesting levels, it can be encoded by a 2-bit number. Now
 * we can encode the tail by combining the 2-bit nesting level with the cpu
 * number. With one byte for the lock value and 3 bytes for the tail, only a
 * 32-bit word is now needed. Even though we only need 1 bit for the lock,
 * we extend it to a full byte to achieve better performance for architectures
 * that support atomic byte write.
 *
 * We also change the first spinner to spin on the lock bit instead of its
 * node; whereby avoiding the need to carry a node from lock to unlock, and
 * preserving existing lock API. This also makes the unlock code simpler and
 * faster.
 *
 * N.B. The current implementation only supports architectures that allow
 *      atomic operations on smaller 8-bit and 16-bit data types.
 *
 */

#include "mcs_spinlock.h"
#define MAX_NODES	4

/**
 * On 64-bit architectures, the mcs_spinlock structure will be 16 bytes in
 * size and four of them will fit nicely in one 64-byte cacheline. For
 * pvqspinlock, however, we need more space for extra data. To accommodate
 * that, we insert two more long words to pad it up to 32 bytes. IOW, only
 * two of them can fit in a cacheline in this case. That is OK as it is rare
 * to have more than 2 levels of slowpath nesting in actual use. We don't
 * want to penalize pvqspinlocks to optimize for a rare case in native
 * qspinlocks.
 * (在64位体系结构上，mcs_spinlock结构体的大小为16字节，四个这样的结构体可以完美地放入一个64字节的高速缓存行中。
 * 然而，对于pvqspinlock而言，我们需要更多空间来存储额外数据。为此，我们增加了两个长字（long words）进行填充，使其扩展到32字节。换句话说，在这种情况下，一个缓存行只能容纳两个pvqspinlock结构体。这是可以接受的，因为在真实使用场景中很少出现超过2级的慢路径嵌套。我们不应为了优化原生qspinlock中的罕见情况而牺牲pvqspinlock的性能。)
 */
struct qnode {
	struct mcs_spinlock mcs;
#ifdef CONFIG_PARAVIRT_SPINLOCKS
	long reserved[2];
#endif
};

/*
 * The pending bit spinning loop count.
 * This heuristic is used to limit the number of lockword accesses
 * made by atomic_cond_read_relaxed when waiting for the lock to
 * transition out of the "== _Q_PENDING_VAL" state. We don't spin
 * indefinitely because there's no guarantee that we'll make forward
 * progress.
 */
#ifndef _Q_PENDING_LOOPS
#define _Q_PENDING_LOOPS	1
#endif

/**
 * Per-CPU queue node structures; we can never have more than 4 nested
 * contexts: task, softirq, hardirq, nmi.
 *
 * Exactly fits one 64-byte cacheline on a 64-bit architecture.
 *
 * PV doubles the storage and uses the second cacheline for PV state.
 * 
 * 为每个CPU都定义了4个qnode，用于4个上下文，分别为task、softirq、hardirq和nmi
 * 
 * 在加锁的过程中，该Per-CPU变量‘qnodes’是控制锁在什么上自旋，但是实际加没加上锁，
 * 还是由 struct qspinlock  来控制!!!
 */
static DEFINE_PER_CPU_ALIGNED(struct qnode, qnodes[MAX_NODES]);

/*
 * We must be able to distinguish between no-tail and the tail at 0:0,
 * therefore increment the cpu number by one.
 */

static inline __pure u32 encode_tail(int cpu, int idx)
{
	u32 tail;

	tail  = (cpu + 1) << _Q_TAIL_CPU_OFFSET;
	tail |= idx << _Q_TAIL_IDX_OFFSET; /* assume < 4 */

	return tail;
}

/**
 * 解码自旋锁的各个位
 */
static inline __pure struct mcs_spinlock *decode_tail(u32 tail)
{
	int cpu = (tail >> _Q_TAIL_CPU_OFFSET) - 1;
	int idx = (tail &  _Q_TAIL_IDX_MASK) >> _Q_TAIL_IDX_OFFSET;

	return per_cpu_ptr(&qnodes[idx].mcs, cpu);
}

/**
 * 其核心意思是 “获取”、“抢占”、“抓取”，通常带有 “快速”、“急切”、“独占” 的意味。
 */
static inline __pure
struct mcs_spinlock *grab_mcs_node(struct mcs_spinlock *base, int idx)
{
	return &((struct qnode *)base + idx)->mcs;
}

#define _Q_LOCKED_PENDING_MASK (_Q_LOCKED_MASK | _Q_PENDING_MASK)

#if _Q_PENDING_BITS == 8
/**
 * clear_pending - clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,* -> *,0,*
 */
static __always_inline void clear_pending(struct qspinlock *lock)
{
	WRITE_ONCE(lock->pending, 0);
}

/**
 * clear_pending_set_locked - take ownership and clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,0 -> *,0,1
 *
 * Lock stealing is not allowed if this function is used.
 * (如果使用了此函数，则不允许锁窃取)
 */
static __always_inline void clear_pending_set_locked(struct qspinlock *lock)
{
	// 直接操作内存就很爽，同一个内存地址，根据结构的不同配合上不同的解析方式，操作的结果就不一样
	WRITE_ONCE(lock->locked_pending, _Q_LOCKED_VAL);
}

/*
 * xchg_tail - Put in the new queue tail code word & retrieve previous one
 * @lock : Pointer to queued spinlock structure
 * @tail : The new queue tail code word
 * Return: The previous queue tail code word
 *
 * xchg(lock, tail), which heads an address dependency
 *
 * p,*,* -> n,*,* ; prev = xchg(lock, node)
 */
static __always_inline u32 xchg_tail(struct qspinlock *lock, u32 tail)
{
	/*
	 * We can use relaxed semantics since the caller ensures that the
	 * MCS node is properly initialized before updating the tail.
	 */
	return (u32)xchg_relaxed(&lock->tail,
				 tail >> _Q_TAIL_OFFSET) << _Q_TAIL_OFFSET;
}

#else /* _Q_PENDING_BITS == 8 */

/**
 * clear_pending - clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,* -> *,0,*
 */
static __always_inline void clear_pending(struct qspinlock *lock)
{
	atomic_andnot(_Q_PENDING_VAL, &lock->val);
}

/**
 * clear_pending_set_locked - take ownership and clear the pending bit.
 * @lock: Pointer to queued spinlock structure
 *
 * *,1,0 -> *,0,1
 */
static __always_inline void clear_pending_set_locked(struct qspinlock *lock)
{
	atomic_add(-_Q_PENDING_VAL + _Q_LOCKED_VAL, &lock->val);
}

/**
 * xchg_tail - Put in the new queue tail code word & retrieve previous one
 * @lock : Pointer to queued spinlock structure
 * @tail : The new queue tail code word
 * Return: The previous queue tail code word
 *
 * xchg(lock, tail)
 *
 * p,*,* -> n,*,* ; prev = xchg(lock, node)
 */
static __always_inline u32 xchg_tail(struct qspinlock *lock, u32 tail)
{
	u32 old, new, val = atomic_read(&lock->val);

	for (;;) {
		new = (val & _Q_LOCKED_PENDING_MASK) | tail;
		/*
		 * We can use relaxed semantics since the caller ensures that
		 * the MCS node is properly initialized before updating the
		 * tail.
		 */
		old = atomic_cmpxchg_relaxed(&lock->val, val, new);
		if (old == val)
			break;

		val = old;
	}
	return old;
}
#endif /* _Q_PENDING_BITS == 8 */

/**
 * queued_fetch_set_pending_acquire - fetch the whole lock value and set pending
 * @lock : Pointer to queued spinlock structure
 * Return: The previous lock value
 *
 * *,*,* -> *,1,*
 */
#ifndef queued_fetch_set_pending_acquire
static __always_inline u32 queued_fetch_set_pending_acquire(struct qspinlock *lock)
{
	return atomic_fetch_or_acquire(_Q_PENDING_VAL, &lock->val);
}
#endif

/**
 * set_locked - Set the lock bit and own the lock
 * @lock: Pointer to queued spinlock structure
 *
 * *,*,0 -> *,0,1
 */
static __always_inline void set_locked(struct qspinlock *lock)
{
	WRITE_ONCE(lock->locked, _Q_LOCKED_VAL);
}


/*
 * Generate the native code for queued_spin_unlock_slowpath(); provide NOPs for
 * all the PV callbacks.
 */

static __always_inline void __pv_init_node(struct mcs_spinlock *node) { }
static __always_inline void __pv_wait_node(struct mcs_spinlock *node,
					   struct mcs_spinlock *prev) { }
static __always_inline void __pv_kick_node(struct qspinlock *lock,
					   struct mcs_spinlock *node) { }
static __always_inline u32  __pv_wait_head_or_lock(struct qspinlock *lock,
						   struct mcs_spinlock *node)
						   { return 0; }

#define pv_enabled()		false

#define pv_init_node		__pv_init_node
#define pv_wait_node		__pv_wait_node
#define pv_kick_node		__pv_kick_node
#define pv_wait_head_or_lock	__pv_wait_head_or_lock

#ifdef CONFIG_PARAVIRT_SPINLOCKS
#define queued_spin_lock_slowpath	native_queued_spin_lock_slowpath
#endif

#endif /* _GEN_PV_LOCK_SLOWPATH */

/**
 * 从宏观来看，系统中有成千上万个自旋锁，但是每个CPU只有唯一的mcs_spinlock节点，那么这些自旋锁怎么和这个唯一的mcs_spinlock节点[4]映射呢？
 *    [4]: 代码提前规划定义了4个mcs_spinlock节点，用于不同的上下文——进程上下文task、软中断上下文softirq、硬中断上下文hardirq和不可屏蔽中断上下文nmi。
 * 从微观角度来看，同一时刻一个CPU只能持有一个自旋锁，其他CPU只是在自旋等待这个被持有的自旋锁，因此每个CPU上有一个mcs_spinlock节点就足够了
 * 
 * > 理解一下: 自旋锁是不允许重入的，
 * [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.3.2　自旋锁的变体
 * 进入自旋锁之前已经显式地调用preempt_disable()函数关闭了抢占，因此内核不会主动发生抢占。
 * 
 * 
 * 
 * 在争抢锁的过程中，也是需要处理中断的!
 * > [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.5.5　案例分析：为什么这里pending域要清零
 * 所以，下面的 'idx = node->count++;' 可能就是中断或其他异常处理程序
 * 
 * 排队自旋锁（Queued Spinlock，Qspinlock）机制
 * > [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.5　排队自旋锁
 * 
 * 
 * queued_spin_lock_slowpath - acquire the queued spinlock
 * @lock: Pointer to queued spinlock structure
 * @val: Current value of the queued spinlock 32-bit word
 *
 * 三元组:
 *  [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.5　排队自旋锁
 * 
 * 这个方法得结合
 *  [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#表1.4　qspinlock中val字段的含义
 * 来分析 
 *
 * 
 * (queue tail, pending bit, lock value)
 *
 * uncontended： 无竞争
 * 
 *              fast     :    slow                                  :    unlock
 *                       :                                          :
 * uncontended  (0,0,0) -:--> (0,0,1) ------------------------------:--> (*,*,0)
 *                       :       | ^--------.------.             /  :
 *                       :       v           \      \            |  :
 * pending               :    (0,1,1) +--> (0,1,0)   \           |  :
 *                       :       | ^--'              |           |  :
 *                       :       v                   |           |  :
 * uncontended           :    (n,x,y) +--> (n,0,0) --'           |  :
 *   queue               :       | ^--'                          |  :
 *                       :       v                               |  :
 * contended             :    (*,x,y) +--> (*,0,0) ---> (*,0,1) -'  :
 *   queue               :         ^--'                             :
 * 
 * 
 * 
 * 结合
 *   [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#图1.7　中速申请通道
 * 来分析吧
 * 
 * 
 * 什么情况表示加锁成功?
 *  1. 将locked域设置为1,其他位设置为0
 */
void queued_spin_lock_slowpath(struct qspinlock *lock, u32 val)
{
    // 注意，这里是MCS锁 , 不是自旋锁(qspinlock): 数据结构不一致
	struct mcs_spinlock *prev, *next, *node;
	u32 old, tail;
	int idx;

	BUILD_BUG_ON(CONFIG_NR_CPUS >= (1U << _Q_TAIL_CPU_BITS));

	if (pv_enabled()) {
		goto pv_queue;
	}

	if (virt_spin_lock(lock)) {
		return;
	}

	/*
	 * Wait for in-progress pending->locked hand-overs with a bounded
	 * number of spins so that we guarantee forward progress.
	 * (等待进行中的“pending->locked”所有权移交操作，并通过有限的自旋次数来确保系统的前向推进。)
	 * 
	 * 0,1,0 -> 0,0,1
	 * 
	 * pending 为1 ， 说明是第一顺位继承人
	 */
	if (val == _Q_PENDING_VAL) { // pending 状态,表示是第一顺位继承人
		int cnt = _Q_PENDING_LOOPS; // 进行有限次的自旋
		/**
		 * 在自旋锁(非MCS锁)lock->val上自旋，直到 自旋次数用完 或者 (VAL != _Q_PENDING_VAL)
		 * (VAL != _Q_PENDING_VAL) 可能:
		 * 1) 在快速路径中被抢占了
		 * 2) 其他
		 */
		val = atomic_cond_read_relaxed(&lock->val, (VAL != _Q_PENDING_VAL) || !cnt--);
	}

	/*
	 * If we observe any contention; queue.
	 * 还有竞争(因为之前还是'_Q_PENDING_VAL')，入队
	 * 
     * 被锁住 或者 还有CPU在等待，即三元组中可能其他两元还有值 
	 * ~_Q_LOCKED_MASK 即 '11111111 11111111 11111111 00000000'
	 * 
	 */
	if (val & ~_Q_LOCKED_MASK) {
		goto queue;
	} else {
		// val 就是 00000000 00000000 00000000 xxxxxxxx
	}

	/*
	 * trylock || pending
	 *
	 * 0,0,* -> 0,1,* -> 0,0,1 pending, trylock
	 * 
	 * 将 lock->val 的pending位置位1,其他位不变；
	 * 返回修改之前的lock->val
	 */
	val = queued_fetch_set_pending_acquire(lock);

	/**
	 * If we observe contention, there is a concurrent locker.
	 * (如果我们观察到争用情况，说明存在并发加锁者。)
	 * > 与之前相比，lock->val的一些位发生了变化，说明了还是有并发加锁
	 * 
	 * Undo and queue; our setting of PENDING might have made the
	 * n,0,0 -> 0,0,0 transition fail and it will now be waiting
	 * on @next to become !NULL.
	 */
	if (unlikely(val & ~_Q_LOCKED_MASK)) { 
		/* Undo PENDING if we set it. */
		if (!(val & _Q_PENDING_MASK)) {
			clear_pending(lock); // 回滚 queued_fetch_set_pending_acquire 中的pending位的设置操作
		}

		goto queue;
	}

	/*
	 * We're pending, wait for the owner to go away.
	 *
	 * 0,1,1 -> 0,1,0
	 *
	 * this wait loop must be a load-acquire such that we match the
	 * store-release that clears the locked bit and create lock
	 * sequentiality; this is because not all
	 * clear_pending_set_locked() implementations imply full
	 * barriers.
	 * 这个等待循环必须采用加载获取（load-acquire）操作，
	 * 以便与清空锁定位的存储释放（store-release）操作相匹配，并建立锁的顺序性；
	 * 这是因为并非所有 clear_pending_set_locked() 的实现都包含完整内存屏障。
	 */
	if (val & _Q_LOCKED_MASK) { // 如果lock->val是被锁住了
		// 以低功耗模式等待 '!(VAL & _Q_LOCKED_MASK)' 条件成立，即 等待锁被释放
		atomic_cond_read_acquire(&lock->val, !(VAL & _Q_LOCKED_MASK));
	}

	/**
	 * take ownership and clear the pending bit.
	 * (获取所有权并清除挂起位)
	 *
	 * 0,1,0 -> 0,0,1
	 */
	clear_pending_set_locked(lock); // 将pending域设置为0,locked域设置为1 , 即 加锁成功
	lockevent_inc(lock_pending);
	return;

	/**
	 * 以下就是慢速申请流程
	 * 
	 * End of pending bit optimistic spinning and beginning of MCS queuing.
	 */
queue:
	lockevent_inc(lock_slowpath);
pv_queue:
    // 注意，这里的node的类型是 struct mcs_spinlock , 即 MCS锁 
	node = this_cpu_ptr(&qnodes[0].mcs);
	/**
	 * node->count++ , 都是qnodes[0].mcs
	 * 依次类推，那么就可以控制自旋锁在哪个qnode上自旋了
	 * 
	 * 但是，会有这种情况吗，这种类型的变量是Per-CPU的,
	 * 结合[Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.3.2　自旋锁的变体
	 * `拥有自旋锁的临界区代码必须原子地执行，不能休眠和主动调度`
	 * 应该是不可能，但是又有另一种说法:
	 * `内核开发者针对最极端的中断嵌套场景所做的一种经过深思熟虑的设计决策。其核心原因是为了应对 NMI (Non-Maskable Interrupt, 不可屏蔽中断) 的嵌套问题`
	 */
	idx = node->count++;
	// encode_tail 修改了 tail_cpu 、 tail_idx 域
	tail = encode_tail(smp_processor_id(), idx);

	/**
	 * 4 nodes are allocated based on the assumption that there will
	 * not be nested NMIs taking spinlocks. That may not be true in
	 * some architectures even though the chance of needing more than
	 * 4 nodes will still be extremely unlikely. When that happens,
	 * we fall back to spinning on the lock directly without using
	 * any MCS node. This is not the most elegant solution, but is
	 * simple enough.
	 * (分配4个节点的假设前提是不会有嵌套的NMI（不可屏蔽中断）来获取自旋锁。
	 * 在某些体系结构上，这个假设可能不成立，但即便如此，需要超过4个节点的可能性仍然微乎其微。
	 * 若确实发生这种情况，
	 * 我们将回退到直接在锁上自旋，而不使用任何MCS节点。这不是最优雅的解决方案，但足够简单。)
	 * 
	 * 回退了，tail_idx 设置为0
	 */
	if (unlikely(idx >= MAX_NODES)) {
		lockevent_inc(lock_no_node);
		while (!queued_spin_trylock(lock)) {
			cpu_relax();
		}
		goto release;
	}

	/**
	 * 获取qnodes[idx].mcs
	 */
	node = grab_mcs_node(node, idx);

	/*
	 * Keep counts of non-zero index values:
	 */
	lockevent_cond_inc(lock_use_node2 + idx - 1, idx);

	/**
	 * Ensure that we increment the head node->count before initialising
	 * the actual node. If the compiler is kind enough to reorder these
	 * stores, then an IRQ could overwrite our assignments.
	 * (请确保在初始化实际节点之前递增头节点 node->count。如果编译器对这些存储操作进行了重排序，那么中断（IRQ）可能会覆盖我们的赋值操作。)
	 */
	barrier();

	node->locked = 0;
	node->next = NULL;
	pv_init_node(node);

	/*
	 * We touched a (possibly) cold cacheline in the per-cpu queue node;
	 * attempt the trylock once more in the hope someone let go while we
	 * weren't watching.
	 * (我们触及了每CPU队列节点中一个（可能）冷的缓存行；趁我们没有注意时希望有人已经释放，再次尝试获取锁。)
	 * 
	 * 使用CAS方式进行加锁: 还是对struct qspinlock 上进行加锁
	 */
	if (queued_spin_trylock(lock)) {
		goto release;
	}

	/*
	 * Ensure that the initialisation of @node is complete before we
	 * publish the updated tail via xchg_tail() and potentially link
	 * @node into the waitqueue via WRITE_ONCE(prev->next, node) below.
	 * (请确保在通过 xchg_tail() 发布更新后的尾指针以及后续可能通过 WRITE_ONCE(prev->next, node) 将 @node 链接到等待队列之前，已完成对 @node 的初始化。)
	 */
	smp_wmb();

	/*
	 * Publish the updated tail.
	 * We have already touched the queueing cacheline; don't bother with
	 * pending stuff.
	 *
	 * p,*,* -> n,*,*
	 * 
	 * 修改当前锁的val字段 : 这个tail中有什么? 只设置了CPU域 和 idx域
	 * > 修改的是lock , 而不是mcs锁
	 */
	old = xchg_tail(lock, tail);
	next = NULL;

	/*
	 * if there was a previous node; link it and wait until reaching the
	 * head of the waitqueue.
	 * (如果存在前驱节点，则将其链接到队列并等待，直到抵达等待队列的头部。)
	 * 
	 * 构建等待链表： 将node加入等待队列 
     *  1. 链表的头尾分别存在哪里? -- 就是正在持有锁的CPU，因为当持有锁的CPU释放锁的时候，会唤醒下一个等待节点
	 * 
	 * _Q_TAIL_MASK: 11111111 11111111 00000000 00000000 
	 */
	if (old & _Q_TAIL_MASK) {
		/**
		 * 这里面有 tail_idx的用途
		 */
		prev = decode_tail(old);

		/** 
		 * Link @node into the waitqueue.
		 * (加入等待队列)
		 *  */
		WRITE_ONCE(prev->next, node);

		pv_wait_node(node, prev);

		/**
		 *  这就是自旋操作了,
         * 当前继节点把锁传递给当前节点时，当前CPU会从睡眠状态唤醒，
		 * 然后退出arch_mcs_spin_lock_contended()函数中的while循环 
         * 锁传递？有意思
		 * 
		 * 注意，这是在mcs锁上自旋
		 */
		arch_mcs_spin_lock_contended(&node->locked);

		/*
		 * While waiting for the MCS lock, the next pointer may have
		 * been set by another lock waiter. We optimistically load
		 * the next pointer & prefetch the cacheline for writing
		 * to reduce latency in the upcoming MCS unlock operation.
		 */
		next = READ_ONCE(node->next);
		if (next) {
			prefetchw(next);
		}
	}

	/*
	 * we're at the head of the waitqueue, wait for the owner & pending to
	 * go away.
	 *
	 * *,x,y -> *,0,0
	 *
	 * this wait loop must use a load-acquire such that we match the
	 * store-release that clears the locked bit and create lock
	 * sequentiality; this is because the set_locked() function below
	 * does not imply a full barrier.
	 *
	 * The PV pv_wait_head_or_lock function, if active, will acquire
	 * the lock and return a non-zero value. So we have to skip the
	 * atomic_cond_read_acquire() call. As the next PV queue head hasn't
	 * been designated yet, there is no way for the locked value to become
	 * _Q_SLOW_VAL. So both the set_locked() and the
	 * atomic_cmpxchg_relaxed() calls will be safe.
	 *
	 * If PV isn't active, 0 will be returned instead.
	 *
	 */
	if ((val = pv_wait_head_or_lock(lock, node))) {
		goto locked;
	}
  
	/**
	 * _Q_LOCKED_PENDING_MASK : 00000000 00000000 11111111 11111111
	 * 
	 * 即，自旋等待lock->val的pending locked位变为0
	 */
	val = atomic_cond_read_acquire(&lock->val, !(VAL & _Q_LOCKED_PENDING_MASK));

locked:
	/*
	 * claim the lock: (获取锁)
	 *
	 * n,0,0 -> 0,0,1 : lock, uncontended
	 * *,*,0 -> *,*,1 : lock, contended
	 *
	 * If the queue head is the only one in the queue (lock value == tail)
	 * and nobody is pending, clear the tail code and grab the lock.
	 * Otherwise, we only need to grab the lock.
	 * (如果队列头是队列中的唯一成员（锁值等于尾值）且没有等待者，
	 * 则清除尾部代码并获取锁。否则，我们只需要获取锁即可)
	 */

	/*
	 * In the PV case we might already have _Q_LOCKED_VAL set, because
	 * of lock stealing; therefore we must also allow:
	 *
	 * n,0,1 -> 0,0,1
	 *
	 * Note: at this point: (val & _Q_PENDING_MASK) == 0, because of the
	 *       above wait condition, therefore any concurrent setting of
	 *       PENDING will make the uncontended transition fail.
	 * L-A-01: (此时：(val & _Q_PENDING_MASK) == 0（由于上述等待条件成立），
	 * 因此任何并行的PENDING标志设置都会导致非竞争状态转换失败)
	 */
	if ((val & _Q_TAIL_MASK) == tail) {
		if (atomic_try_cmpxchg_relaxed(&lock->val, &val, _Q_LOCKED_VAL)) {
			goto release; /* No contention(竞争) */
		}
	}

	/*
	 * Either somebody is queued behind us or _Q_PENDING_VAL got set
	 * which will then detect the remaining tail and queue behind us
	 * ensuring we'll see a @next.
	 * (要么有CPU已经排在我们后方，要么设置了 _Q_PENDING_VAL 标志（该标志会检测剩余的尾部并在我们后方排队），这能确保我们最终能观测到 @next 指针)
	 * 
	 * 加锁!
	 */
	set_locked(lock);

	/*
	 * contended path; wait for next if not observed yet, release.
	 */
	if (!next) {
		/**
		 * 自旋等待，直到node->next 不为空 ， 那万一没有再获取锁的需要呢?
		 * > 那就是没有竞争咯(队列中只有一个元素)，那么在上面就已经获取到锁，释放了，就不会再执行到这里了 -- L-A-01
		 */
		next = smp_cond_load_relaxed(&node->next, (VAL));
	}

	/**
     * 将锁传递给下一个节点 , 那下一个节点拿到锁会怎么样呢?
	 * 
	 * 将 next->locked 设置为1
	 */
	arch_mcs_spin_unlock_contended(&next->locked);
	pv_kick_node(lock, next);

release:
	/*
	 * release the node : 释放这个节点?
	 */
	__this_cpu_dec(qnodes[0].mcs.count);
}
EXPORT_SYMBOL(queued_spin_lock_slowpath);

/*
 * Generate the paravirt code for queued_spin_unlock_slowpath().
 */
#if !defined(_GEN_PV_LOCK_SLOWPATH) && defined(CONFIG_PARAVIRT_SPINLOCKS)
#define _GEN_PV_LOCK_SLOWPATH

#undef  pv_enabled
#define pv_enabled()	true

#undef pv_init_node
#undef pv_wait_node
#undef pv_kick_node
#undef pv_wait_head_or_lock

#undef  queued_spin_lock_slowpath
#define queued_spin_lock_slowpath	__pv_queued_spin_lock_slowpath

#include "qspinlock_paravirt.h"
#include "qspinlock.c"

bool nopvspin __initdata;
static __init int parse_nopvspin(char *arg)
{
	nopvspin = true;
	return 0;
}
early_param("nopvspin", parse_nopvspin);
#endif
