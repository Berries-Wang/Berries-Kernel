.. SPDX-License-Identifier: GPL-2.0

.. _cpumasks-header-label:

=================================================
BPF cpumask kfuncs(BPF cpumask 内核函数 (kfuncs))
=================================================

1. Introduction(1. 简介)
========================

``struct cpumask`` is a bitmap data structure in the kernel whose indices
reflect the CPUs on the system. Commonly, cpumasks are used to track which CPUs
a task is affinitized to, but they can also be used to e.g. track which cores
are associated with a scheduling domain, which cores on a machine are idle,
etc.
(``struct cpumask`` 是内核中的一种位图数据结构，其索引对应系统上的 CPU。cpumask 通常用于跟踪任务被绑核到哪些 CPU，但也可以用于例如跟踪哪些核心与某个调度域相关联、机器上哪些核心空闲等。)

BPF provides programs with a set of :ref:`kfuncs-header-label` that can be
used to allocate, mutate, query, and free cpumasks.
(BPF 为程序提供了一组 :ref:`kfuncs-header-label`，可用于分配、修改、查询与释放 cpumask。)

2. BPF cpumask objects(2. BPF cpumask 对象)
===========================================

There are two different types of cpumasks that can be used by BPF programs.
(BPF 程序可以使用两种不同类型的 cpumask。)

2.1 ``struct bpf_cpumask *``(2.1 BPF 分配的 cpumask)
----------------------------------------------------

``struct bpf_cpumask *`` is a cpumask that is allocated by BPF, on behalf of a
BPF program, and whose lifecycle is entirely controlled by BPF. These cpumasks
are RCU-protected, can be mutated, can be used as kptrs, and can be safely cast
to a ``struct cpumask *``.
(``struct bpf_cpumask *`` 是由 BPF 代表 BPF 程序分配的 cpumask，其生命周期完全由 BPF 控制。这类 cpumask 受 RCU 保护、可以被修改、可以用作 kptr，并且可以安全地转型为 ``struct cpumask *``。)

2.1.1 ``struct bpf_cpumask *`` lifecycle(2.1.1 ``struct bpf_cpumask *`` 的生命周期)
-----------------------------------------------------------------------------------

A ``struct bpf_cpumask *`` is allocated, acquired, and released, using the
following functions:
(``struct bpf_cpumask *`` 使用以下函数来分配、获取与释放：)

.. kernel-doc:: kernel/bpf/cpumask.c
  :identifiers: bpf_cpumask_create

.. kernel-doc:: kernel/bpf/cpumask.c
  :identifiers: bpf_cpumask_acquire

.. kernel-doc:: kernel/bpf/cpumask.c
  :identifiers: bpf_cpumask_release

For example:
(例如：)

.. code-block:: c

        struct cpumask_map_value {
                struct bpf_cpumask __kptr * cpumask;
        };

        struct array_map {
                __uint(type, BPF_MAP_TYPE_ARRAY);
                __type(key, int);
                __type(value, struct cpumask_map_value);
                __uint(max_entries, 65536);
        } cpumask_map SEC(".maps");

        static int cpumask_map_insert(struct bpf_cpumask *mask, u32 pid)
        {
                struct cpumask_map_value local, *v;
                long status;
                struct bpf_cpumask *old;
                u32 key = pid;

                local.cpumask = NULL;
                status = bpf_map_update_elem(&cpumask_map, &key, &local, 0);
                if (status) {
                        bpf_cpumask_release(mask);
                        return status;
                }

                v = bpf_map_lookup_elem(&cpumask_map, &key);
                if (!v) {
                        bpf_cpumask_release(mask);
                        return -ENOENT;
                }

                old = bpf_kptr_xchg(&v->cpumask, mask);
                if (old)
                        bpf_cpumask_release(old);

                return 0;
        }

        /**
         * A sample tracepoint showing how a task's cpumask can be queried and
         * recorded as a kptr.
         */
        SEC("tp_btf/task_newtask")
        int BPF_PROG(record_task_cpumask, struct task_struct *task, u64 clone_flags)
        {
                struct bpf_cpumask *cpumask;
                int ret;

                cpumask = bpf_cpumask_create();
                if (!cpumask)
                        return -ENOMEM;

                if (!bpf_cpumask_full(task->cpus_ptr))
                        bpf_printk("task %s has CPU affinity", task->comm);

                bpf_cpumask_copy(cpumask, task->cpus_ptr);
                return cpumask_map_insert(cpumask, task->pid);
        }

----

2.1.1 ``struct bpf_cpumask *`` as kptrs(2.1.1 作为 kptr 的 ``struct bpf_cpumask *``)
------------------------------------------------------------------------------------

As mentioned and illustrated above, these ``struct bpf_cpumask *`` objects can
also be stored in a map and used as kptrs. If a ``struct bpf_cpumask *`` is in
a map, the reference can be removed from the map with bpf_kptr_xchg(), or
opportunistically acquired using RCU:
(如上所述并示意，这些 ``struct bpf_cpumask *`` 对象也可以存进 map 并用作 kptr。如果 ``struct bpf_cpumask *`` 在某个 map 中，可以用 bpf_kptr_xchg() 把引用从 map 中移出，或者用 RCU 伺机获取：)

.. code-block:: c

	/* struct containing the struct bpf_cpumask kptr which is stored in the map. */
	struct cpumasks_kfunc_map_value {
		struct bpf_cpumask __kptr * bpf_cpumask;
	};

	/* The map containing struct cpumasks_kfunc_map_value entries. */
	struct {
		__uint(type, BPF_MAP_TYPE_ARRAY);
		__type(key, int);
		__type(value, struct cpumasks_kfunc_map_value);
		__uint(max_entries, 1);
	} cpumasks_kfunc_map SEC(".maps");

	/* ... */

	/**
	 * A simple example tracepoint program showing how a
	 * struct bpf_cpumask * kptr that is stored in a map can
	 * be passed to kfuncs using RCU protection.
	 */
	SEC("tp_btf/cgroup_mkdir")
	int BPF_PROG(cgrp_ancestor_example, struct cgroup *cgrp, const char *path)
	{
		struct bpf_cpumask *kptr;
		struct cpumasks_kfunc_map_value *v;
		u32 key = 0;

		/* Assume a bpf_cpumask * kptr was previously stored in the map. */
		v = bpf_map_lookup_elem(&cpumasks_kfunc_map, &key);
		if (!v)
			return -ENOENT;

		bpf_rcu_read_lock();
		/* Acquire a reference to the bpf_cpumask * kptr that's already stored in the map. */
		kptr = v->cpumask;
		if (!kptr) {
			/* If no bpf_cpumask was present in the map, it's because
			 * we're racing with another CPU that removed it with
			 * bpf_kptr_xchg() between the bpf_map_lookup_elem()
			 * above, and our load of the pointer from the map.
			 */
			bpf_rcu_read_unlock();
			return -EBUSY;
		}

		bpf_cpumask_setall(kptr);
		bpf_rcu_read_unlock();

		return 0;
	}

----

2.2 ``struct cpumask``(2.2 内核 cpumask 位图)
---------------------------------------------

``struct cpumask`` is the object that actually contains the cpumask bitmap
being queried, mutated, etc. A ``struct bpf_cpumask`` wraps a ``struct
cpumask``, which is why it's safe to cast it as such (note however that it is
**not** safe to cast a ``struct cpumask *`` to a ``struct bpf_cpumask *``, and
the verifier will reject any program that tries to do so).
(``struct cpumask`` 是实际包含被查询、修改等的 cpumask 位图的对象。``struct bpf_cpumask`` 包装了一个 ``struct cpumask``，因此把它转型为后者是安全的(但注意，把 ``struct cpumask *`` 转型为 ``struct bpf_cpumask *`` 是 **不** 安全的，校验器会拒绝任何这样做的程序)。)

As we'll see below, any kfunc that mutates its cpumask argument will take a
``struct bpf_cpumask *`` as that argument. Any argument that simply queries the
cpumask will instead take a ``struct cpumask *``.
(如下文所示，任何会修改其 cpumask 参数的 kfunc 都以 ``struct bpf_cpumask *`` 作为该参数。只查询 cpumask 的参数则相反，采用 ``struct cpumask *``。)

3. cpumask kfuncs(3. cpumask 内核函数)
======================================

Above, we described the kfuncs that can be used to allocate, acquire, release,
etc a ``struct bpf_cpumask *``. This section of the document will describe the
kfuncs for mutating and querying cpumasks.
(上文介绍了可用于分配、获取、释放 ``struct bpf_cpumask *`` 等的 kfuncs。本文档的这一节将描述用于修改与查询 cpumask 的 kfuncs。)

3.1 Mutating cpumasks(3.1 修改 cpumask)
---------------------------------------

Some cpumask kfuncs are "read-only" in that they don't mutate any of their
arguments, whereas others mutate at least one argument (which means that the
argument must be a ``struct bpf_cpumask *``, as described above).
(一些 cpumask kfuncs 是"只读"的，不修改任何参数；另一些则至少修改一个参数(如上所述，这意味着该参数必须是 ``struct bpf_cpumask *``)。)

This section will describe all of the cpumask kfuncs which mutate at least one
argument. :ref:`cpumasks-querying-label` below describes the read-only kfuncs.
(本节将描述所有至少修改一个参数的 cpumask kfuncs。下文的 :ref:`cpumasks-querying-label` 描述只读 kfuncs。)

3.1.1 Setting and clearing CPUs(3.1.1 设置与清除 CPU)
-----------------------------------------------------

bpf_cpumask_set_cpu() and bpf_cpumask_clear_cpu() can be used to set and clear
a CPU in a ``struct bpf_cpumask`` respectively:
(bpf_cpumask_set_cpu() 与 bpf_cpumask_clear_cpu() 分别可用于在 ``struct bpf_cpumask`` 中设置与清除一个 CPU：)

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_set_cpu bpf_cpumask_clear_cpu

These kfuncs are pretty straightforward, and can be used, for example, as
follows:
(这些 kfuncs 非常直白，例如可以像下面这样使用：)

.. code-block:: c

        /**
         * A sample tracepoint showing how a cpumask can be queried.
         */
        SEC("tp_btf/task_newtask")
        int BPF_PROG(test_set_clear_cpu, struct task_struct *task, u64 clone_flags)
        {
                struct bpf_cpumask *cpumask;

                cpumask = bpf_cpumask_create();
                if (!cpumask)
                        return -ENOMEM;

                bpf_cpumask_set_cpu(0, cpumask);
                if (!bpf_cpumask_test_cpu(0, cast(cpumask)))
                        /* Should never happen. */
                        goto release_exit;

                bpf_cpumask_clear_cpu(0, cpumask);
                if (bpf_cpumask_test_cpu(0, cast(cpumask)))
                        /* Should never happen. */
                        goto release_exit;

                /* struct cpumask * pointers such as task->cpus_ptr can also be queried. */
                if (bpf_cpumask_test_cpu(0, task->cpus_ptr))
                        bpf_printk("task %s can use CPU %d", task->comm, 0);

        release_exit:
                bpf_cpumask_release(cpumask);
                return 0;
        }

----

bpf_cpumask_test_and_set_cpu() and bpf_cpumask_test_and_clear_cpu() are
complementary kfuncs that allow callers to atomically test and set (or clear)
CPUs:
(bpf_cpumask_test_and_set_cpu() 与 bpf_cpumask_test_and_clear_cpu() 是互补的 kfuncs，允许调用方原子地测试并设置(或清除)CPU：)

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_test_and_set_cpu bpf_cpumask_test_and_clear_cpu

----

We can also set and clear entire ``struct bpf_cpumask *`` objects in one
operation using bpf_cpumask_setall() and bpf_cpumask_clear():
(我们还可以用 bpf_cpumask_setall() 与 bpf_cpumask_clear() 一次操作设置或清除整个 ``struct bpf_cpumask *`` 对象：)

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_setall bpf_cpumask_clear

3.1.2 Operations between cpumasks(3.1.2 cpumask 之间的运算)
-----------------------------------------------------------

In addition to setting and clearing individual CPUs in a single cpumask,
callers can also perform bitwise operations between multiple cpumasks using
bpf_cpumask_and(), bpf_cpumask_or(), and bpf_cpumask_xor():
(除了在单个 cpumask 中设置与清除单个 CPU 之外，调用方还可以使用 bpf_cpumask_and()、bpf_cpumask_or() 与 bpf_cpumask_xor() 在多个 cpumask 之间执行按位运算：)

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_and bpf_cpumask_or bpf_cpumask_xor

The following is an example of how they may be used. Note that some of the
kfuncs shown in this example will be covered in more detail below.
(下面是它们的使用示例。注意，本例中展示的部分 kfuncs 将在下文更详细地介绍。)

.. code-block:: c

        /**
         * A sample tracepoint showing how a cpumask can be mutated using
           bitwise operators (and queried).
         */
        SEC("tp_btf/task_newtask")
        int BPF_PROG(test_and_or_xor, struct task_struct *task, u64 clone_flags)
        {
                struct bpf_cpumask *mask1, *mask2, *dst1, *dst2;

                mask1 = bpf_cpumask_create();
                if (!mask1)
                        return -ENOMEM;

                mask2 = bpf_cpumask_create();
                if (!mask2) {
                        bpf_cpumask_release(mask1);
                        return -ENOMEM;
                }

                // ...Safely create the other two masks... */

                bpf_cpumask_set_cpu(0, mask1);
                bpf_cpumask_set_cpu(1, mask2);
                bpf_cpumask_and(dst1, (const struct cpumask *)mask1, (const struct cpumask *)mask2);
                if (!bpf_cpumask_empty((const struct cpumask *)dst1))
                        /* Should never happen. */
                        goto release_exit;

                bpf_cpumask_or(dst1, (const struct cpumask *)mask1, (const struct cpumask *)mask2);
                if (!bpf_cpumask_test_cpu(0, (const struct cpumask *)dst1))
                        /* Should never happen. */
                        goto release_exit;

                if (!bpf_cpumask_test_cpu(1, (const struct cpumask *)dst1))
                        /* Should never happen. */
                        goto release_exit;

                bpf_cpumask_xor(dst2, (const struct cpumask *)mask1, (const struct cpumask *)mask2);
                if (!bpf_cpumask_equal((const struct cpumask *)dst1,
                                       (const struct cpumask *)dst2))
                        /* Should never happen. */
                        goto release_exit;

         release_exit:
                bpf_cpumask_release(mask1);
                bpf_cpumask_release(mask2);
                bpf_cpumask_release(dst1);
                bpf_cpumask_release(dst2);
                return 0;
        }

----

The contents of an entire cpumask may be copied to another using
bpf_cpumask_copy():
(可以使用 bpf_cpumask_copy() 把一个完整 cpumask 的内容复制到另一个：)

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_copy

----

.. _cpumasks-querying-label:

3.2 Querying cpumasks(3.2 查询 cpumask)
---------------------------------------

In addition to the above kfuncs, there is also a set of read-only kfuncs that
can be used to query the contents of cpumasks.
(除上述 kfuncs 之外，还有一组只读 kfuncs 可用于查询 cpumask 的内容。)

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_first bpf_cpumask_first_zero bpf_cpumask_first_and
                 bpf_cpumask_test_cpu bpf_cpumask_weight

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_equal bpf_cpumask_intersects bpf_cpumask_subset
                 bpf_cpumask_empty bpf_cpumask_full

.. kernel-doc:: kernel/bpf/cpumask.c
   :identifiers: bpf_cpumask_any_distribute bpf_cpumask_any_and_distribute

----

Some example usages of these querying kfuncs were shown above. We will not
replicate those examples here. Note, however, that all of the aforementioned
kfuncs are tested in `tools/testing/selftests/bpf/progs/cpumask_success.c`_, so
please take a look there if you're looking for more examples of how they can be
used.
(上文已展示了这些查询 kfuncs 的一些使用示例，此处不再重复。不过要注意，上述所有 kfuncs 都在 `tools/testing/selftests/bpf/progs/cpumask_success.c`_ 中得到测试；如果想找更多用例，请查看该文件。)

.. _tools/testing/selftests/bpf/progs/cpumask_success.c:
   https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/tools/testing/selftests/bpf/progs/cpumask_success.c


4. Adding BPF cpumask kfuncs(4. 新增 BPF cpumask kfuncs)
========================================================

The set of supported BPF cpumask kfuncs are not (yet) a 1-1 match with the
cpumask operations in include/linux/cpumask.h. Any of those cpumask operations
could easily be encapsulated in a new kfunc if and when required. If you'd like
to support a new cpumask operation, please feel free to submit a patch. If you
do add a new cpumask kfunc, please document it here, and add any relevant
selftest testcases to the cpumask selftest suite.
(受支持的 BPF cpumask kfuncs 集合尚不能与 include/linux/cpumask.h 中的 cpumask 操作一一对应。一旦需要，那些 cpumask 操作中的任何一个都可以轻松封装成新的 kfunc。如果你想支持某个新的 cpumask 操作，请随时提交补丁。如果你确实新增了一个 cpumask kfunc，请在本文档中为其撰写文档，并把相关的 selftest 测试用例加入 cpumask selftest 套件。)
