.. SPDX-License-Identifier: GPL-2.0

============================================================
The padata parallel execution mechanism(padata 并行执行机制)
============================================================

:Date: May 2020
(日期: 2020年5月)

Padata is a mechanism by which the kernel can farm jobs out to be done in
parallel on multiple CPUs while optionally retaining their ordering.
(Padata 是一种内核将作业分发到多个 CPU 上并行执行、同时可选择保持其顺序的机制。)

It was originally developed for IPsec, which needs to perform encryption and
decryption on large numbers of packets without reordering those packets.  This
is currently the sole consumer of padata's serialized job support.
(它最初是为 IPsec 开发的，IPsec 需要对大量数据包进行加密和解密而又不重排这些数据包。这目前是 padata 串行作业支持的唯一使用者。)

Padata also supports multithreaded jobs, splitting up the job evenly while load
balancing and coordinating between threads.
(Padata 还支持多线程作业，在负载均衡和线程间协调的同时将作业均匀拆分。)

Running Serialized Jobs(运行串行作业)
=====================================

Initializing(初始化)
--------------------

The first step in using padata to run serialized jobs is to set up a
padata_instance structure for overall control of how jobs are to be run::

    #include <linux/padata.h>

    struct padata_instance *padata_alloc(const char *name);

(使用 padata 运行串行作业的第一步，是建立一个 padata_instance 结构，用于总体控制作业的运行方式。)

'name' simply identifies the instance.
('name' 只是用来标识该实例。)

Then, complete padata initialization by allocating a padata_shell::

   struct padata_shell *padata_alloc_shell(struct padata_instance *pinst);

(然后，通过分配一个 padata_shell 来完成 padata 的初始化：)

A padata_shell is used to submit a job to padata and allows a series of such
jobs to be serialized independently.  A padata_instance may have one or more
padata_shells associated with it, each allowing a separate series of jobs.
(padata_shell 用于向 padata 提交作业，并允许一系列这样的作业彼此独立地串行化。一个 padata_instance 可以关联一个或多个 padata_shell，每个 padata_shell 允许一个独立的作业序列。)

Modifying cpumasks(修改 cpumasks)
---------------------------------

The CPUs used to run jobs can be changed in two ways, programmatically with
padata_set_cpumask() or via sysfs.  The former is defined::

    int padata_set_cpumask(struct padata_instance *pinst, int cpumask_type,
			   cpumask_var_t cpumask);

(用于运行作业的 CPU 可以通过两种方式更改：以编程方式调用 padata_set_cpumask()，或通过 sysfs。前者的定义如下：)

Here cpumask_type is one of PADATA_CPU_PARALLEL or PADATA_CPU_SERIAL, where a
parallel cpumask describes which processors will be used to execute jobs
submitted to this instance in parallel and a serial cpumask defines which
processors are allowed to be used as the serialization callback processor.
cpumask specifies the new cpumask to use.
(这里 cpumask_type 是 PADATA_CPU_PARALLEL 或 PADATA_CPU_SERIAL 之一：并行 cpumask 描述哪些处理器将用于并行执行提交到此实例的作业，而串行 cpumask 定义哪些处理器可以被用作串行化回调处理器。cpumask 指定要使用的新 cpumask。)

There may be sysfs files for an instance's cpumasks.  For example, pcrypt's
live in /sys/kernel/pcrypt/<instance-name>.  Within an instance's directory
there are two files, parallel_cpumask and serial_cpumask, and either cpumask
may be changed by echoing a bitmask into the file, for example::

    echo f > /sys/kernel/pcrypt/pencrypt/parallel_cpumask

(一个实例的 cpumask 可能有对应的 sysfs 文件。例如，pcrypt 的文件位于 /sys/kernel/pcrypt/<instance-name> 目录中。在一个实例的目录下有两个文件 parallel_cpumask 和 serial_cpumask，可以通过向文件中回显 (echo) 一个位掩码来更改相应的 cpumask，例如：)

Reading one of these files shows the user-supplied cpumask, which may be
different from the 'usable' cpumask.
(读取这些文件中的某一个会显示用户提供的 cpumask，它可能与 'usable' cpumask 不同。)

Padata maintains two pairs of cpumasks internally, the user-supplied cpumasks
and the 'usable' cpumasks.  (Each pair consists of a parallel and a serial
cpumask.)  The user-supplied cpumasks default to all possible CPUs on instance
allocation and may be changed as above.  The usable cpumasks are always a
subset of the user-supplied cpumasks and contain only the online CPUs in the
user-supplied masks; these are the cpumasks padata actually uses.  So it is
legal to supply a cpumask to padata that contains offline CPUs.  Once an
offline CPU in the user-supplied cpumask comes online, padata is going to use
it.
(Padata 在内部维护着两对 cpumask：用户提供的 cpumask 和 'usable' cpumask。(每对都由一个并行 cpumask 和一个串行 cpumask 组成。) 用户提供的 cpumask 在实例分配时默认为所有可能的 CPU，并且可以按上述方式更改。可用的 cpumask 始终是用户提供的 cpumask 的子集，仅包含用户提供掩码中处于在线状态的 CPU；这些才是 padata 实际使用的 cpumask。因此，向 padata 提供一个包含离线 CPU 的 cpumask 是合法的。一旦用户提供的 cpumask 中的离线 CPU 上线，padata 就会使用它。)

Changing the CPU masks are expensive operations, so it should not be done with
great frequency.
(更改 CPU 掩码是代价高昂的操作，因此不应频繁进行。)

Running A Job(运行一个作业)
---------------------------

Actually submitting work to the padata instance requires the creation of a
padata_priv structure, which represents one job::

    struct padata_priv {
        /* Other stuff here... */
	void                    (*parallel)(struct padata_priv *padata);
	void                    (*serial)(struct padata_priv *padata);
    };

(实际向 padata 实例提交工作需要创建一个 padata_priv 结构，它表示一个作业。)

This structure will almost certainly be embedded within some larger
structure specific to the work to be done.  Most of its fields are private to
padata, but the structure should be zeroed at initialisation time, and the
parallel() and serial() functions should be provided.  Those functions will
be called in the process of getting the work done as we will see
momentarily.
(这个结构几乎肯定会嵌入到与要完成的工作相关的某个更大的结构中。它的大多数字段对 padata 来说是私有的，但该结构应在初始化时清零，并且应提供 parallel() 和 serial() 函数。正如我们稍后将看到的，在完成工作的过程中这些函数会被调用。)

The submission of the job is done with::

    int padata_do_parallel(struct padata_shell *ps,
		           struct padata_priv *padata, int *cb_cpu);

(作业的提交通过以下函数完成：)

The ps and padata structures must be set up as described above; cb_cpu
points to the preferred CPU to be used for the final callback when the job is
done; it must be in the current instance's CPU mask (if not the cb_cpu pointer
is updated to point to the CPU actually chosen).  The return value from
padata_do_parallel() is zero on success, indicating that the job is in
progress. -EBUSY means that somebody, somewhere else is messing with the
instance's CPU mask, while -EINVAL is a complaint about cb_cpu not being in the
serial cpumask, no online CPUs in the parallel or serial cpumasks, or a stopped
instance.
(ps 和 padata 结构必须按上文所述进行设置；cb_cpu 指向作业完成时用于最终回调的首选 CPU；它必须位于当前实例的 CPU 掩码中(如果不是，cb_cpu 指针会被更新为指向实际选定的 CPU)。padata_do_parallel() 的返回值在成功时为零，表示作业正在进行中。-EBUSY 表示其他地方有人在改动该实例的 CPU 掩码，而 -EINVAL 则是对以下情况的抱怨：cb_cpu 不在串行 cpumask 中、并行或串行 cpumask 中没有在线 CPU，或者实例已停止。)

Each job submitted to padata_do_parallel() will, in turn, be passed to
exactly one call to the above-mentioned parallel() function, on one CPU, so
true parallelism is achieved by submitting multiple jobs.  parallel() runs with
software interrupts disabled and thus cannot sleep.  The parallel()
function gets the padata_priv structure pointer as its lone parameter;
information about the actual work to be done is probably obtained by using
container_of() to find the enclosing structure.
(每个提交给 padata_do_parallel() 的作业都会依次在某个 CPU 上被恰好一次地传递给上述 parallel() 函数，因此真正的并行是通过提交多个作业来实现的。parallel() 在软件中断被禁用的情况下运行，因此不能睡眠。parallel() 函数以 padata_priv 结构指针作为其唯一参数；有关实际要完成的工作的信息通常通过 container_of() 找到外围结构来获取。)

Note that parallel() has no return value; the padata subsystem assumes that
parallel() will take responsibility for the job from this point.  The job
need not be completed during this call, but, if parallel() leaves work
outstanding, it should be prepared to be called again with a new job before
the previous one completes.
(注意 parallel() 没有返回值；padata 子系统假定从这一刻起 parallel() 将对该作业负责。作业不必在这次调用中完成，但如果 parallel() 留下了未完成的工作，它应该做好准备，在上一个作业完成之前被再次调用以处理新作业。)

Serializing Jobs(串行化作业)
----------------------------

When a job does complete, parallel() (or whatever function actually finishes
the work) should inform padata of the fact with a call to::

    void padata_do_serial(struct padata_priv *padata);

(当一个作业确实完成时，parallel()(或实际完成工作的任何函数)应通过调用以下函数将这一事实通知 padata：)

At some point in the future, padata_do_serial() will trigger a call to the
serial() function in the padata_priv structure.  That call will happen on
the CPU requested in the initial call to padata_do_parallel(); it, too, is
run with local software interrupts disabled.
Note that this call may be deferred for a while since the padata code takes
pains to ensure that jobs are completed in the order in which they were
submitted.
(在未来的某个时刻，padata_do_serial() 会触发对 padata_priv 结构中 serial() 函数的调用。该调用将发生在最初调用 padata_do_parallel() 时所请求的 CPU 上；它同样在本地软件中断被禁用的情况下运行。注意，这个调用可能会延迟一段时间，因为 padata 代码会尽力确保作业按提交的顺序完成。)

Destroying(销毁)
----------------

Cleaning up a padata instance predictably involves calling the two free
functions that correspond to the allocation in reverse::

    void padata_free_shell(struct padata_shell *ps);
    void padata_free(struct padata_instance *pinst);

(清理 padata 实例不出所料地涉及按与分配相反的顺序调用对应的两个释放函数。)

It is the user's responsibility to ensure all outstanding jobs are complete
before any of the above are called.
(用户有责任在调用上述任何函数之前确保所有未完成的作业都已完成。)

Running Multithreaded Jobs(运行多线程作业)
==========================================

A multithreaded job has a main thread and zero or more helper threads, with the
main thread participating in the job and then waiting until all helpers have
finished.  padata splits the job into units called chunks, where a chunk is a
piece of the job that one thread completes in one call to the thread function.
(多线程作业有一个主线程和零个或多个辅助线程，主线程参与作业，然后等待所有辅助线程完成。padata 将作业拆分为称为块 (chunk) 的单元，一个块是指一个线程在一次调用线程函数中完成的那部分作业。)

A user has to do three things to run a multithreaded job.  First, describe the
job by defining a padata_mt_job structure, which is explained in the Interface
section.  This includes a pointer to the thread function, which padata will
call each time it assigns a job chunk to a thread.  Then, define the thread
function, which accepts three arguments, ``start``, ``end``, and ``arg``, where
the first two delimit the range that the thread operates on and the last is a
pointer to the job's shared state, if any.  Prepare the shared state, which is
typically allocated on the main thread's stack.  Last, call
padata_do_multithreaded(), which will return once the job is finished.
(用户必须做三件事才能运行多线程作业。首先，通过定义一个 padata_mt_job 结构来描述作业，该结构将在接口 (Interface) 一节中说明。其中包括一个指向线程函数的指针，padata 每次将一个作业块分配给某个线程时都会调用该函数。然后，定义线程函数，它接受三个参数 ``start`` 、 ``end`` 和 ``arg`` ，前两个参数界定了线程的操作范围，最后一个参数是指向作业共享状态(如果有的话)的指针。准备好共享状态，它通常分配在主线程的栈上。最后，调用 padata_do_multithreaded()，它会在作业完成后返回。)

Interface(接口)
===============

.. kernel-doc:: include/linux/padata.h
.. kernel-doc:: kernel/padata.c
