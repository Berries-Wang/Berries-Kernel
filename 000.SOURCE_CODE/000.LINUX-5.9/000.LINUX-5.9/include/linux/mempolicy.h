/* SPDX-License-Identifier: GPL-2.0 */
/*
 * NUMA memory policies for Linux.
 * Copyright 2003,2004 Andi Kleen SuSE Labs
 */
#ifndef _LINUX_MEMPOLICY_H
#define _LINUX_MEMPOLICY_H 1

#include <linux/sched.h>
#include <linux/mmzone.h>
#include <linux/dax.h>
#include <linux/slab.h>
#include <linux/rbtree.h>
#include <linux/spinlock.h>
#include <linux/nodemask.h>
#include <linux/pagemap.h>
#include <uapi/linux/mempolicy.h>

struct mm_struct;

#ifdef CONFIG_NUMA

/*
 * Describe a memory policy.
 *
 * A mempolicy can be either associated with a process or with a VMA.
 * For VMA related allocations the VMA policy is preferred, otherwise
 * the process policy is used. Interrupts ignore the memory policy
 * of the current process.
 * (内存策略可与进程关联，也可与虚拟内存区域（VMA）关联。针对VMA相关的内存分配操作时，
 * 将优先采用VMA策略；否则将使用进程策略。中断处理会忽略当前进程的内存策略。)
 *
 * Locking policy for interleave:(交错策略的锁定机制：)
 * In process context there is no locking because only the process accesses
 * its own state. All vma manipulation is somewhat protected by a down_read on
 * mmap_lock.
 * (在进程上下文中无需加锁，因为只有进程自身能访问其内部状态。所有虚拟内存区域（VMA）操作均通过对mmap_lock的读锁定提供一定程度的保护)
 *
 * Freeing policy:(内存策略对象的释放机制)
 * Mempolicy objects are reference counted.  A mempolicy will be freed when
 * mpol_put() decrements the reference count to zero.
 * (内存策略对象采用引用计数管理。当mpol_put()函数将引用计数递减至零时，内存策略对象将被释放)
 *
 * Duplicating policy objects:(内存策略对象的复制机制：)
 * mpol_dup() allocates a new mempolicy and copies the specified mempolicy
 * to the new storage.  The reference count of the new object is initialized
 * to 1, representing the caller of mpol_dup().
 * (mpol_dup()函数会分配新的内存策略对象，并将指定策略复制到新存储空间。新对象的引用计数被初始化为1，代表调用mpol_dup()的持有者。)
 */
struct mempolicy {
	atomic_t refcnt;
	unsigned short mode; /* See MPOL_* above */
	unsigned short flags; /* See set_mempolicy() MPOL_F_* above */
	union {
		short preferred_node; /* preferred */
		nodemask_t nodes; /* interleave/bind */
		/* undefined for default */
	} v;
	union {
		nodemask_t cpuset_mems_allowed; /* relative to these nodes */
		nodemask_t user_nodemask; /* nodemask passed by user */
	} w;
};

/*
 * Support for managing mempolicy data objects (clone, copy, destroy)
 * The default fast path of a NULL MPOL_DEFAULT policy is always inlined.
 */

extern void __mpol_put(struct mempolicy *pol);
static inline void mpol_put(struct mempolicy *pol)
{
	if (pol)
		__mpol_put(pol);
}

/*
 * Does mempolicy pol need explicit unref after use?
 * Currently only needed for shared policies.
 */
static inline int mpol_needs_cond_ref(struct mempolicy *pol)
{
	return (pol && (pol->flags & MPOL_F_SHARED));
}

static inline void mpol_cond_put(struct mempolicy *pol)
{
	if (mpol_needs_cond_ref(pol))
		__mpol_put(pol);
}

extern struct mempolicy *__mpol_dup(struct mempolicy *pol);
static inline struct mempolicy *mpol_dup(struct mempolicy *pol)
{
	if (pol)
		pol = __mpol_dup(pol);
	return pol;
}

#define vma_policy(vma) ((vma)->vm_policy)

static inline void mpol_get(struct mempolicy *pol)
{
	if (pol)
		atomic_inc(&pol->refcnt);
}

extern bool __mpol_equal(struct mempolicy *a, struct mempolicy *b);
static inline bool mpol_equal(struct mempolicy *a, struct mempolicy *b)
{
	if (a == b)
		return true;
	return __mpol_equal(a, b);
}

/*
 * Tree of shared policies for a shared memory region.
 * Maintain the policies in a pseudo mm that contains vmas. The vmas
 * carry the policy. As a special twist the pseudo mm is indexed in pages, not
 * bytes, so that we can work with shared memory segments bigger than
 * unsigned long.
 */

struct sp_node {
	struct rb_node nd;
	unsigned long start, end;
	struct mempolicy *policy;
};

struct shared_policy {
	struct rb_root root;
	rwlock_t lock;
};

int vma_dup_policy(struct vm_area_struct *src, struct vm_area_struct *dst);
void mpol_shared_policy_init(struct shared_policy *sp, struct mempolicy *mpol);
int mpol_set_shared_policy(struct shared_policy *info,
				struct vm_area_struct *vma,
				struct mempolicy *new);
void mpol_free_shared_policy(struct shared_policy *p);
struct mempolicy *mpol_shared_policy_lookup(struct shared_policy *sp,
					    unsigned long idx);

struct mempolicy *get_task_policy(struct task_struct *p);
struct mempolicy *__get_vma_policy(struct vm_area_struct *vma,
		unsigned long addr);
bool vma_policy_mof(struct vm_area_struct *vma);

extern void numa_default_policy(void);
extern void numa_policy_init(void);
extern void mpol_rebind_task(struct task_struct *tsk, const nodemask_t *new);
extern void mpol_rebind_mm(struct mm_struct *mm, nodemask_t *new);

extern int huge_node(struct vm_area_struct *vma,
				unsigned long addr, gfp_t gfp_flags,
				struct mempolicy **mpol, nodemask_t **nodemask);
extern bool init_nodemask_of_mempolicy(nodemask_t *mask);
extern bool mempolicy_nodemask_intersects(struct task_struct *tsk,
				const nodemask_t *mask);
extern nodemask_t *policy_nodemask(gfp_t gfp, struct mempolicy *policy);

static inline nodemask_t *policy_nodemask_current(gfp_t gfp)
{
	struct mempolicy *mpol = get_task_policy(current);

	return policy_nodemask(gfp, mpol);
}

extern unsigned int mempolicy_slab_node(void);

extern enum zone_type policy_zone;

static inline void check_highest_zone(enum zone_type k)
{
	if (k > policy_zone && k != ZONE_MOVABLE)
		policy_zone = k;
}

int do_migrate_pages(struct mm_struct *mm, const nodemask_t *from,
		     const nodemask_t *to, int flags);


#ifdef CONFIG_TMPFS
extern int mpol_parse_str(char *str, struct mempolicy **mpol);
#endif

extern void mpol_to_str(char *buffer, int maxlen, struct mempolicy *pol);

/* Check if a vma is migratable */
extern bool vma_migratable(struct vm_area_struct *vma);

extern int mpol_misplaced(struct page *, struct vm_area_struct *, unsigned long);
extern void mpol_put_task_policy(struct task_struct *);

#else

struct mempolicy {};

static inline bool mpol_equal(struct mempolicy *a, struct mempolicy *b)
{
	return true;
}

static inline void mpol_put(struct mempolicy *p)
{
}

static inline void mpol_cond_put(struct mempolicy *pol)
{
}

static inline void mpol_get(struct mempolicy *pol)
{
}

struct shared_policy {};

static inline void mpol_shared_policy_init(struct shared_policy *sp,
						struct mempolicy *mpol)
{
}

static inline void mpol_free_shared_policy(struct shared_policy *p)
{
}

static inline struct mempolicy *
mpol_shared_policy_lookup(struct shared_policy *sp, unsigned long idx)
{
	return NULL;
}

#define vma_policy(vma) NULL

static inline int
vma_dup_policy(struct vm_area_struct *src, struct vm_area_struct *dst)
{
	return 0;
}

static inline void numa_policy_init(void)
{
}

static inline void numa_default_policy(void)
{
}

static inline void mpol_rebind_task(struct task_struct *tsk,
				const nodemask_t *new)
{
}

static inline void mpol_rebind_mm(struct mm_struct *mm, nodemask_t *new)
{
}

static inline int huge_node(struct vm_area_struct *vma,
				unsigned long addr, gfp_t gfp_flags,
				struct mempolicy **mpol, nodemask_t **nodemask)
{
	*mpol = NULL;
	*nodemask = NULL;
	return 0;
}

static inline bool init_nodemask_of_mempolicy(nodemask_t *m)
{
	return false;
}

static inline int do_migrate_pages(struct mm_struct *mm, const nodemask_t *from,
				   const nodemask_t *to, int flags)
{
	return 0;
}

static inline void check_highest_zone(int k)
{
}

#ifdef CONFIG_TMPFS
static inline int mpol_parse_str(char *str, struct mempolicy **mpol)
{
	return 1;	/* error */
}
#endif

static inline int mpol_misplaced(struct page *page, struct vm_area_struct *vma,
				 unsigned long address)
{
	return -1; /* no node preference */
}

static inline void mpol_put_task_policy(struct task_struct *task)
{
}

static inline nodemask_t *policy_nodemask_current(gfp_t gfp)
{
	return NULL;
}
#endif /* CONFIG_NUMA */
#endif
