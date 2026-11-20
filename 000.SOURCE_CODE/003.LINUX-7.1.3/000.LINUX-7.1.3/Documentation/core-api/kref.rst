===================================================================================
Adding reference counters (krefs) to kernel objects(为内核对象添加引用计数（kref）)
===================================================================================

:Author: Corey Minyard <minyard@acm.org>
(作者: Corey Minyard <minyard@acm.org>)
:Author: Thomas Hellström <thomas.hellstrom@linux.intel.com>
(作者: Thomas Hellström <thomas.hellstrom@linux.intel.com>)

A lot of this was lifted from Greg Kroah-Hartman's 2004 OLS paper and
presentation on krefs, which can be found at:
(本文很多内容取自 Greg Kroah-Hartman 2004 年 OLS 上关于 kref 的论文与演讲，可在以下地址找到：)

  - http://www.kroah.com/linux/talks/ols_2004_kref_paper/Reprint-Kroah-Hartman-OLS2004.pdf
  - http://www.kroah.com/linux/talks/ols_2004_kref_talk/

Introduction(引言)
==================

krefs allow you to add reference counters to your objects.  If you
have objects that are used in multiple places and passed around, and
you don't have refcounts, your code is almost certainly broken.  If
you want refcounts, krefs are the way to go.
(kref 让你能够为对象添加引用计数。如果你的对象在多处使用、四处传递，而你没有引用计数，那么你的代码几乎肯定有问题。如果你需要引用计数，kref 就是正确的选择。)

To use a kref, add one to your data structures like::

    struct my_data
    {
	.
	.
    struct kref refcount;
	.
	.
    };

(要使用 kref，把它加进你的数据结构即可，如上所示。)

The kref can occur anywhere within the data structure.
(kref 可以位于数据结构中的任意位置。)

Initialization(初始化)
======================

You must initialize the kref after you allocate it.  To do this, call
kref_init as so::

     struct my_data *data;

     data = kmalloc(sizeof(*data), GFP_KERNEL);
     if (!data)
            return -ENOMEM;
     kref_init(&data->refcount);

(分配 kref 之后必须对它初始化。为此，像上面这样调用 kref_init。)

This sets the refcount in the kref to 1.
(这会把 kref 中的引用计数设为 1。)

Kref rules(kref 规则)
=====================

Once you have an initialized kref, you must follow the following
rules:
(一旦拥有一个已初始化的 kref，就必须遵守以下规则：)

1) If you make a non-temporary copy of a pointer, especially if
   it can be passed to another thread of execution, you must
   increment the refcount with kref_get() before passing it off::

       kref_get(&data->refcount);

   (如果制作了指针的非临时副本——尤其是它可能被传递给另一个执行线程——必须在移交之前用 kref_get() 增加引用计数：)

   If you already have a valid pointer to a kref-ed structure (the
   refcount cannot go to zero) you may do this without a lock.
   (如果你已经持有指向 kref-ed 结构的有效指针（引用计数不会降到 0），则可以不加锁执行此操作。)

2) When you are done with a pointer, you must call kref_put()::

       kref_put(&data->refcount, data_release);

   (用完一个指针之后，必须调用 kref_put()：)

   If this is the last reference to the pointer, the release
   routine will be called.  If the code never tries to get
   a valid pointer to a kref-ed structure without already
   holding a valid pointer, it is safe to do this without
   a lock.
   (如果这是该指针的最后一个引用，释放例程将被调用。如果代码从不在未持有有效指针的情况下尝试获取指向 kref-ed 结构的有效指针，那么不加锁这样做是安全的。)

3) If the code attempts to gain a reference to a kref-ed structure
   without already holding a valid pointer, it must serialize access
   where a kref_put() cannot occur during the kref_get(), and the
   structure must remain valid during the kref_get().
   (如果代码试图在未持有有效指针的情况下获取对 kref-ed 结构的引用，就必须对访问做串行化，使 kref_put() 不可能在 kref_get() 期间发生，并且该结构在 kref_get() 期间必须保持有效。)

For example, if you allocate some data and then pass it to another
thread to process::

    void data_release(struct kref *ref)
    {
	struct my_data *data = container_of(ref, struct my_data, refcount);
	kfree(data);
    }

    void more_data_handling(void *cb_data)
    {
	struct my_data *data = cb_data;
	.
	. do stuff with data here
	.
	kref_put(&data->refcount, data_release);
    }

    int my_data_handler(void)
    {
	int rv = 0;
	struct my_data *data;
	struct task_struct *task;
	data = kmalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	kref_init(&data->refcount);

	kref_get(&data->refcount);
	task = kthread_run(more_data_handling, data, "more_data_handling");
	if (task == ERR_PTR(-ENOMEM)) {
		rv = -ENOMEM;
	        kref_put(&data->refcount, data_release);
		goto out;
	}

	.
	. do stuff with data here
	.
    out:
	kref_put(&data->refcount, data_release);
	return rv;
    }

(例如，如果你分配了一些数据，然后把它交给另一个线程处理，就可以像上面这样写。)

This way, it doesn't matter what order the two threads handle the
data, the kref_put() handles knowing when the data is not referenced
any more and releasing it.  The kref_get() does not require a lock,
since we already have a valid pointer that we own a refcount for.  The
put needs no lock because nothing tries to get the data without
already holding a pointer.
(这样，两个线程以何种顺序处理数据都无关紧要，kref_put() 负责判断数据何时不再被引用并释放它。kref_get() 无需加锁，因为我们已经拥有一个对其持有引用计数的有效指针。put 也无需加锁，因为没有任何代码会在未持有指针的情况下尝试获取数据。)

In the above example, kref_put() will be called 2 times in both success
and error paths. This is necessary because the reference count got
incremented 2 times by kref_init() and kref_get().
(在上面的示例中，无论成功还是出错路径，kref_put() 都会被调用 2 次。这是必要的，因为引用计数已被 kref_init() 和 kref_get() 各加了一次。)

Note that the "before" in rule 1 is very important.  You should never
do something like::

	task = kthread_run(more_data_handling, data, "more_data_handling");
	if (task == ERR_PTR(-ENOMEM)) {
		rv = -ENOMEM;
		goto out;
	} else
		/* BAD BAD BAD - get is after the handoff */
		kref_get(&data->refcount);

(注意，规则 1 中的"之前"非常重要。绝不要像下面这样做：)

Don't assume you know what you are doing and use the above construct.
First of all, you may not know what you are doing.  Second, you may
know what you are doing (there are some situations where locking is
involved where the above may be legal) but someone else who doesn't
know what they are doing may change the code or copy the code.  It's
bad style.  Don't do it.
(不要自以为知道自己在做什么而使用上面的构造。首先，你可能并不知道自己在做什么。其次，即使你知道自己在做什么（在某些涉及锁的情况下上面的写法可能是合法的），其他不知道自己在做什么的人也可能修改或复制这段代码。这是坏风格，不要这样做。)

There are some situations where you can optimize the gets and puts.
For instance, if you are done with an object and enqueuing it for
something else or passing it off to something else, there is no reason
to do a get then a put::

	/* Silly extra get and put */
	kref_get(&obj->ref);
	enqueue(obj);
	kref_put(&obj->ref, obj_cleanup);

(有些情况下你可以对 get 和 put 做优化。例如，如果你已经用完一个对象，正把它入队作其他用途或移交给别处，就没有必要先 get 再 put：)

Just do the enqueue.  A comment about this is always welcome::

	enqueue(obj);
	/* We are done with obj, so we pass our refcount off
	   to the queue.  DON'T TOUCH obj AFTER HERE! */

(直接执行入队即可。像上面那样加一条注释说明移交引用计数总是受欢迎的：)

The last rule (rule 3) is the nastiest one to handle.  Say, for
instance, you have a list of items that are each kref-ed, and you wish
to get the first one.  You can't just pull the first item off the list
and kref_get() it.  That violates rule 3 because you are not already
holding a valid pointer.  You must add a mutex (or some other lock).
For instance::

	static DEFINE_MUTEX(mutex);
	static LIST_HEAD(q);
	struct my_data
	{
		struct kref      refcount;
		struct list_head link;
	};

	static struct my_data *get_entry()
	{
		struct my_data *entry = NULL;
		mutex_lock(&mutex);
		if (!list_empty(&q)) {
			entry = container_of(q.next, struct my_data, link);
			kref_get(&entry->refcount);
		}
		mutex_unlock(&mutex);
		return entry;
	}

	static void release_entry(struct kref *ref)
	{
		struct my_data *entry = container_of(ref, struct my_data, refcount);

		list_del(&entry->link);
		kfree(entry);
	}

	static void put_entry(struct my_data *entry)
	{
		mutex_lock(&mutex);
		kref_put(&entry->refcount, release_entry);
		mutex_unlock(&mutex);
	}

(最后一条规则（规则 3）是最难处理的。比如说，你有一个链表，其中每一项都带 kref，而你想取出第一项。你不能直接从链表取下第一项然后对它 kref_get()——这违反规则 3，因为你并没有预先持有有效指针。你必须加一个互斥量（或其他锁）。例如像上面这样：)

The kref_put() return value is useful if you do not want to hold the
lock during the whole release operation.  Say you didn't want to call
kfree() with the lock held in the example above (since it is kind of
pointless to do so).  You could use kref_put() as follows::

	static void release_entry(struct kref *ref)
	{
		/* All work is done after the return from kref_put(). */
	}

	static void put_entry(struct my_data *entry)
	{
		mutex_lock(&mutex);
		if (kref_put(&entry->refcount, release_entry)) {
			list_del(&entry->link);
			mutex_unlock(&mutex);
			kfree(entry);
		} else
			mutex_unlock(&mutex);
	}

(kref_put() 的返回值很有用——如果你不想在整个释放操作期间都持有锁的话。比如说，在上面的例子中你不想在持有锁的情况下调用 kfree()（因为这没什么意义），就可以像下面这样使用 kref_put()：)

This is really more useful if you have to call other routines as part
of the free operations that could take a long time or might claim the
same lock.  Note that doing everything in the release routine is still
preferred as it is a little neater.
(如果释放操作中还需要调用其他可能耗时较长、或可能请求同一把锁的例程，这种做法才真正更有用。注意，把所有工作都放在释放例程中完成仍是首选，因为它更整洁一些。)

The above example could also be optimized using kref_get_unless_zero() in
the following way::

	static struct my_data *get_entry()
	{
		struct my_data *entry = NULL;
		mutex_lock(&mutex);
		if (!list_empty(&q)) {
			entry = container_of(q.next, struct my_data, link);
			if (!kref_get_unless_zero(&entry->refcount))
				entry = NULL;
		}
		mutex_unlock(&mutex);
		return entry;
	}

	static void release_entry(struct kref *ref)
	{
		struct my_data *entry = container_of(ref, struct my_data, refcount);

		mutex_lock(&mutex);
		list_del(&entry->link);
		mutex_unlock(&mutex);
		kfree(entry);
	}

	static void put_entry(struct my_data *entry)
	{
		kref_put(&entry->refcount, release_entry);
	}

(上面的例子也可以用 kref_get_unless_zero() 按下面的方式优化：)

Which is useful to remove the mutex lock around kref_put() in put_entry(), but
it's important that kref_get_unless_zero is enclosed in the same critical
section that finds the entry in the lookup table,
otherwise kref_get_unless_zero may reference already freed memory.
Note that it is illegal to use kref_get_unless_zero without checking its
return value. If you are sure (by already having a valid pointer) that
kref_get_unless_zero() will return true, then use kref_get() instead.
(这样可以去掉 put_entry() 中围绕 kref_put() 的互斥锁；但重要的是，kref_get_unless_zero 必须与在查找表中找到该表项的代码处于同一个临界区内，否则 kref_get_unless_zero 可能引用已被释放的内存。注意，不检查 kref_get_unless_zero 的返回值就使用它是非法的。如果你确信（因为已经持有有效指针）kref_get_unless_zero() 会返回 true，那就改用 kref_get()。)

Krefs and RCU(kref 与 RCU)
==========================

The function kref_get_unless_zero also makes it possible to use rcu
locking for lookups in the above example::

	struct my_data
	{
		struct rcu_head rhead;
		.
		struct kref refcount;
		.
		.
	};

	static struct my_data *get_entry_rcu()
	{
		struct my_data *entry = NULL;
		rcu_read_lock();
		if (!list_empty(&q)) {
			entry = container_of(q.next, struct my_data, link);
			if (!kref_get_unless_zero(&entry->refcount))
				entry = NULL;
		}
		rcu_read_unlock();
		return entry;
	}

	static void release_entry_rcu(struct kref *ref)
	{
		struct my_data *entry = container_of(ref, struct my_data, refcount);

		mutex_lock(&mutex);
		list_del_rcu(&entry->link);
		mutex_unlock(&mutex);
		kfree_rcu(entry, rhead);
	}

	static void put_entry(struct my_data *entry)
	{
		kref_put(&entry->refcount, release_entry_rcu);
	}

(函数 kref_get_unless_zero 还使上面的例子可以在查找时使用 RCU 锁：)

But note that the struct kref member needs to remain in valid memory for a
rcu grace period after release_entry_rcu was called. That can be accomplished
by using kfree_rcu(entry, rhead) as done above, or by calling synchronize_rcu()
before using kfree, but note that synchronize_rcu() may sleep for a
substantial amount of time.
(但要注意，在 release_entry_rcu 被调用之后，struct kref 成员必须在一个 RCU 宽限期内保持内存有效。这可以像上面那样用 kfree_rcu(entry, rhead) 实现，也可以在使用 kfree 之前先调用 synchronize_rcu()，但要注意 synchronize_rcu() 可能睡眠相当长的时间。)

Functions and structures(函数与结构)
====================================

.. kernel-doc:: include/linux/kref.h
