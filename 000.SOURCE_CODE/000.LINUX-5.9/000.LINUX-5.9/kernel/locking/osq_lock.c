// SPDX-License-Identifier: GPL-2.0
#include <linux/percpu.h>
#include <linux/sched.h>
#include <linux/osq_lock.h>

/*
 * An MCS like lock especially tailored for optimistic spinning for sleeping
 * lock implementations (mutex, rwsem, etc).
 *
 * Using a single mcs node per CPU is safe because sleeping locks should not be
 * called from interrupt context and we have preemption disabled while
 * spinning.
 * 
 * Per-CPU 变量
 * 
 */
static DEFINE_PER_CPU_SHARED_ALIGNED(struct optimistic_spin_node, osq_node);

/*
 * We use the value 0 to represent "no CPU", thus the encoded value
 * will be the CPU number incremented by 1.
 */
static inline int encode_cpu(int cpu_nr)
{
	return cpu_nr + 1;
}

static inline int node_cpu(struct optimistic_spin_node *node)
{
	return node->cpu - 1;
}

static inline struct optimistic_spin_node *decode_cpu(int encoded_cpu_val)
{
	int cpu_nr = encoded_cpu_val - 1;

	return per_cpu_ptr(&osq_node, cpu_nr);
}

/*
 * Get a stable @node->next pointer, either for unlock() or unqueue() purposes.
 * Can return NULL in case we were the last queued and we updated @lock instead.
 */
static inline struct optimistic_spin_node *
osq_wait_next(struct optimistic_spin_queue *lock,
	      struct optimistic_spin_node *node,
	      struct optimistic_spin_node *prev)
{
	struct optimistic_spin_node *next = NULL;
	int curr = encode_cpu(smp_processor_id());
	int old;

	/*
	 * If there is a prev node in queue, then the 'old' value will be
	 * the prev node's CPU #, else it's set to OSQ_UNLOCKED_VAL since if
	 * we're currently last in queue, then the queue will then become empty.
	 */
	old = prev ? prev->cpu : OSQ_UNLOCKED_VAL;

	for (;;) {
		if (atomic_read(&lock->tail) == curr &&
		    atomic_cmpxchg_acquire(&lock->tail, curr, old) == curr) {
			/*
			 * We were the last queued, we moved @lock back. @prev
			 * will now observe @lock and will complete its
			 * unlock()/unqueue().
			 */
			break;
		}

		/*
		 * We must xchg() the @node->next value, because if we were to
		 * leave it in, a concurrent unlock()/unqueue() from
		 * @node->next might complete Step-A and think its @prev is
		 * still valid.
		 *
		 * If the concurrent unlock()/unqueue() wins the race, we'll
		 * wait for either @lock to point to us, through its Step-B, or
		 * wait for a new @node->next from its Step-C.
		 */
		if (node->next) {
			next = xchg(&node->next, NULL);
			if (next)
				break;
		}

		cpu_relax();
	}

	return next;
}

/**
 * 申请MCS锁
 * 
 * 疑问： MCS 锁只有一把吗?
 *   当然不是了,有 ${CPU_NUM}个
 * 
 * 嘿嘿？小伙子，有什么惊讶的地方吗？例如:
 * 1. 如果多个进程(进程切换&系统调用)调用osq_lock函数，
 * 那么就会构建一个等待链表，这个链表的元素都是自己指向自己(假设只是单核CPU)，那么加锁还有什么意义呢?
 * 
 * 2. 内核中，这种的锁只有一把吗?
 * 
 * 首先，对于1,不会构建元素同样的一个链表；
 *      对于2,嗯，可以这么理解，但是你得明白，这是个自旋锁，也不存在锁被获取了，然后切换到其他进程时又再次获取。
 * 
 * 具体的解释参考: 
 *      [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.3.2　自旋锁的变体
 * > 搜索 '在上述场景中，如果CPU0在临界区中发生了进程切换，会是什么情况？'
 * >> 进入自旋锁之前已经显式地调用preempt_disable()函数关闭了抢占，因此内核不会主动发生抢占。
 * include/linux/percpu-defs.h
 */ 
bool osq_lock(struct optimistic_spin_queue *lock)
{
	// MCS 锁的核心思想： 每个锁的申请者只在本地的CPU的变量上自旋，而不是全局变量
	struct optimistic_spin_node *node = this_cpu_ptr(&osq_node);
	struct optimistic_spin_node *prev, *next;
	/**
	 * 获取当前CPU编号
	 */
	int curr = encode_cpu(smp_processor_id());
	int old;

	node->locked = 0;
	node->next = NULL;
	// CPU编号和实际编号存在映射，见: encode_cpu 函数
	node->cpu = curr;

	/*
	 * We need both ACQUIRE (pairs with corresponding RELEASE in
	 * unlock() uncontended, or fastpath) and RELEASE (to publish
	 * the node fields we just initialised) semantics when updating
	 * the lock tail.
	 */
	old = atomic_xchg(&lock->tail, curr);
	// 加锁成功: old == OSQ_UNLOCKED_VAL
	if (old == OSQ_UNLOCKED_VAL) {
		return true;
	}
    
	// 获取到持有锁的CPU编号
	prev = decode_cpu(old);

	/**
	 * 构建链表
	 */

	// 将当前node插入到MCS链表,什么是MCS: [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub]#1.4　MCS锁
	node->prev = prev;

	/*
	 * osq_lock()			unqueue
	 *
	 * node->prev = prev		osq_wait_next()
	 * WMB				MB
	 * prev->next = node		next->prev = prev // unqueue-C
	 *
	 * Here 'node->prev' and 'next->prev' are the same variable and we need
	 * to ensure these stores happen in-order to avoid corrupting the list.
	 * (此处 'node->prev' 和 'next->prev' 是同一变量，我们需要确保这些存储操作按序执行，以避免破坏链表结构。)
	 */
	smp_wmb();

    // MCS 链表是一个双向链表，维护prev节点的next属性
	WRITE_ONCE(prev->next, node);

	/*
	 * Normally @prev is untouchable after the above store; because at that
	 * moment unlock can proceed and wipe the node element from stack.
	 *
	 * However, since our nodes are static per-cpu storage, we're
	 * guaranteed their existence -- this allows us to apply
	 * cmpxchg in an attempt to undo our queueing.
	 */

	/*
	 * Wait to acquire the lock or cancelation. Note that need_resched()
	 * will come with an IPI, which will wake smp_cond_load_relaxed() if it
	 * is implemented with a monitor-wait. vcpu_is_preempted() relies on
	 * polling, be careful.
	 * (等待获取锁或取消操作。请注意，need_resched() 会伴随 IPI 中断到来，
	 * 若 smp_cond_load_relaxed() 是通过 monitor-wait 方式实现的，
	 * 该中断会将其唤醒。vcpu_is_preempted() 依赖于轮询机制，请谨慎使用。)
	 * 
	 * need_resched 表示当前进程需要重新调度
	 * 
	 * 即 已经解锁 或者 当前进程需重新调度 ， vcpu_is_preempted 固定返回false
	 */
	if (smp_cond_load_relaxed( &node->locked,
		    VAL || need_resched() ||vcpu_is_preempted(node_cpu(node->prev)))) {
		return true;
	}

	/* unqueue(出队) */
	/*
	 * Step - A  -- stabilize @prev
	 *
	 * Undo our @prev->next assignment; this will make @prev's
	 * unlock()/unqueue() wait for a next pointer since @lock points to us
	 * (or later).
	 * 
	 * 遍历链表
	 */
	for (;;) {
		/*
		 * cpu_relax() below implies a compiler barrier which would
		 * prevent this comparison being optimized away.
		 */
		if (data_race(prev->next) == node &&
		    cmpxchg(&prev->next, node, NULL) == node)
			break;

		/*
		 * We can only fail the cmpxchg() racing against an unlock(),
		 * in which case we should observe @node->locked becomming
		 * true.
		 * 
		 * 在*{optimistic_spin_node}->locked字段上自旋
		 */
		if (smp_load_acquire(&node->locked)) {
			return true;
		}

		cpu_relax();

		/*
		 * Or we race against a concurrent unqueue()'s step-B, in which
		 * case its step-C will write us a new @node->prev pointer.
		 */
		prev = READ_ONCE(node->prev);
	}

	/*
	 * Step - B -- stabilize @next
	 *
	 * Similar to unlock(), wait for @node->next or move @lock from @node
	 * back to @prev.
	 */

	next = osq_wait_next(lock, node, prev);
	if (!next)
		return false;

	/*
	 * Step - C -- unlink
	 *
	 * @prev is stable because its still waiting for a new @prev->next
	 * pointer, @next is stable because our @node->next pointer is NULL and
	 * it will wait in Step-A.
	 */

	WRITE_ONCE(next->prev, prev);
	WRITE_ONCE(prev->next, next);

	return false;
}


/**
 * 释放MCS锁
 */
void osq_unlock(struct optimistic_spin_queue *lock)
{
	struct optimistic_spin_node *node, *next;
	int curr = encode_cpu(smp_processor_id());

	/*
	 * Fast path for the uncontended case.
	 */
	if (likely(atomic_cmpxchg_release(&lock->tail, curr,
					  OSQ_UNLOCKED_VAL) == curr))
		return;

	/*
	 * Second most likely case.
	 */
	node = this_cpu_ptr(&osq_node);
	next = xchg(&node->next, NULL);
	if (next) {
		WRITE_ONCE(next->locked, 1);
		return;
	}

	next = osq_wait_next(lock, node, NULL);
	if (next)
		WRITE_ONCE(next->locked, 1);
}
