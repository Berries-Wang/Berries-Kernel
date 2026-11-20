
.. _local_ops:

===========================================================================
Semantics and Behavior of Local Atomic Operations(本地原子操作的语义与行为)
===========================================================================

:Author: Mathieu Desnoyers
(作者： Mathieu Desnoyers)


This document explains the purpose of the local atomic operations, how
to implement them for any given architecture and shows how they can be used
properly. It also stresses on the precautions that must be taken when reading
those local variables across CPUs when the order of memory writes matters.
(本文档说明本地原子操作的用途、如何在任意给定架构上实现它们，以及如何正确使用它们。它还特别强调：当内存写入顺序有影响时，跨 CPU 读取这些本地变量必须注意的事项。)

.. note::

    Note that ``local_t`` based operations are not recommended for general
    kernel use. Please use the ``this_cpu`` operations instead unless there is
    really a special purpose. Most uses of ``local_t`` in the kernel have been
    replaced by ``this_cpu`` operations. ``this_cpu`` operations combine the
    relocation with the ``local_t`` like semantics in a single instruction and
    yield more compact and faster executing code.
    (注意：基于 ``local_t`` 的操作不建议在内核中普遍使用。除非确有特殊目的，请改用 ``this_cpu`` 操作。内核中对 ``local_t`` 的大多数使用已被 ``this_cpu`` 操作取代。``this_cpu`` 操作把重定位与类似 ``local_t`` 的语义合并在单条指令中，生成更紧凑、执行更快的代码。)


Purpose of local atomic operations(本地原子操作的用途)
======================================================

Local atomic operations are meant to provide fast and highly reentrant per CPU
counters. They minimize the performance cost of standard atomic operations by
removing the LOCK prefix and memory barriers normally required to synchronize
across CPUs.
(本地原子操作旨在提供快速且高度可重入的 per CPU 计数器。它们通过去掉 LOCK 前缀以及跨 CPU 同步通常所需的内存屏障，把标准原子操作的性能开销降到最低。)

Having fast per CPU atomic counters is interesting in many cases: it does not
require disabling interrupts to protect from interrupt handlers and it permits
coherent counters in NMI handlers. It is especially useful for tracing purposes
and for various performance monitoring counters.
(拥有快速的 per CPU 原子计数器在许多场景下都很有价值：它无需为了防护中断处理程序而禁用中断，并且允许在 NMI 处理程序中保持计数器的一致性。它对跟踪用途以及各类性能监控计数器尤其有用。)

Local atomic operations only guarantee variable modification atomicity wrt the
CPU which owns the data. Therefore, care must taken to make sure that only one
CPU writes to the ``local_t`` data. This is done by using per cpu data and
making sure that we modify it from within a preemption safe context. It is
however permitted to read ``local_t`` data from any CPU: it will then appear to
be written out of order wrt other memory writes by the owner CPU.
(本地原子操作只保证相对于拥有该数据的 CPU 的变量修改原子性。因此，必须小心确保只有一个 CPU 写入 ``local_t`` 数据。这通过使用 per cpu 数据，并确保在抢占安全的上下文中修改它来实现。不过，允许从任意 CPU 读取 ``local_t`` 数据：此时相对于拥有者 CPU 的其他内存写入，它看起来像是乱序写入的。)


Implementation for a given architecture(针对给定架构的实现)
===========================================================

It can be done by slightly modifying the standard atomic operations: only
their UP variant must be kept. It typically means removing LOCK prefix (on
i386 and x86_64) and any SMP synchronization barrier. If the architecture does
not have a different behavior between SMP and UP, including
``asm-generic/local.h`` in your architecture's ``local.h`` is sufficient.
(做法是对标准原子操作稍作修改：只保留其 UP 变体。这通常意味着去掉 LOCK 前缀(在 i386 和 x86_64 上)以及任何 SMP 同步屏障。如果架构在 SMP 与 UP 之间没有行为差异，那么只需在架构的 ``local.h`` 中包含 ``asm-generic/local.h`` 即可。)

The ``local_t`` type is defined as an opaque ``signed long`` by embedding an
``atomic_long_t`` inside a structure. This is made so a cast from this type to
a ``long`` fails. The definition looks like::

    typedef struct { atomic_long_t a; } local_t;

(``local_t`` 类型通过在结构体内嵌入一个 ``atomic_long_t`` 定义为不透明的 ``signed long``。这样做是为了让从该类型到 ``long`` 的强制转换失败。定义如下：)


Rules to follow when using local atomic operations(使用本地原子操作的规则)
==========================================================================

* Variables touched by local ops must be per cpu variables.
  (被本地操作触及的变量必须是 per cpu 变量。)
* *Only* the CPU owner of these variables must write to them.
  (*只有* 这些变量的所属 CPU 才能写入它们。)
* This CPU can use local ops from any context (process, irq, softirq, nmi, ...)
  to update its ``local_t`` variables.
  (本 CPU 可以在任何上下文(进程、irq、softirq、nmi 等)中使用本地操作来更新自己的 ``local_t`` 变量。)
* Preemption (or interrupts) must be disabled when using local ops in
  process context to make sure the process won't be migrated to a
  different CPU between getting the per-cpu variable and doing the
  actual local op.
  (在进程上下文中使用本地操作时必须禁用抢占(或中断)，以确保进程在获取 per-cpu 变量与执行实际本地操作之间不会被迁移到其他 CPU。)
* When using local ops in interrupt context, no special care must be
  taken on a mainline kernel, since they will run on the local CPU with
  preemption already disabled. I suggest, however, to explicitly
  disable preemption anyway to make sure it will still work correctly on
  -rt kernels.
  (在中断上下文中使用本地操作时，在主线内核上无需特别处理，因为它们运行在本地 CPU 上且抢占已被禁用。不过我仍建议显式禁用抢占，以确保在 -rt 内核上也能正确工作。)
* Reading the local cpu variable will provide the current copy of the
  variable.
  (读取本地 cpu 变量将得到该变量的当前副本。)
* Reads of these variables can be done from any CPU, because updates to
  "``long``", aligned, variables are always atomic. Since no memory
  synchronization is done by the writer CPU, an outdated copy of the
  variable can be read when reading some *other* cpu's variables.
  (对这些变量的读取可以在任意 CPU 上进行，因为对齐的 ``long`` 类型变量的更新总是原子的。由于写者 CPU 不做任何内存同步，读取*其他* cpu 的变量时可能读到过期的副本。)


How to use local atomic operations(如何使用本地原子操作)
========================================================

::

    #include <linux/percpu.h>
    #include <asm/local.h>

    static DEFINE_PER_CPU(local_t, counters) = LOCAL_INIT(0);


Counting(计数)
==============

Counting is done on all the bits of a signed long.
(计数是在有符号长整型的所有位上进行的。)

In preemptible context, use ``get_cpu_var()`` and ``put_cpu_var()`` around
local atomic operations: it makes sure that preemption is disabled around write
access to the per cpu variable. For instance::

    local_inc(&get_cpu_var(counters));
    put_cpu_var(counters);

(在可抢占上下文中，应在本地原子操作前后使用 ``get_cpu_var()`` 与 ``put_cpu_var()``：这样可确保对 per cpu 变量的写访问期间抢占处于禁用状态。例如：)

If you are already in a preemption-safe context, you can use
``this_cpu_ptr()`` instead::

    local_inc(this_cpu_ptr(&counters));

(如果你已经处于抢占安全的上下文中，可以改用 ``this_cpu_ptr()``：)



Reading the counters(读取计数器)
================================

Those local counters can be read from foreign CPUs to sum the count. Note that
the data seen by local_read across CPUs must be considered to be out of order
relatively to other memory writes happening on the CPU that owns the data::

    long sum = 0;
    for_each_online_cpu(cpu)
            sum += local_read(&per_cpu(counters, cpu));

(这些本地计数器可以从其他 CPU 读取以求出总和。注意：跨 CPU 执行 local_read 看到的数据，必须视为相对于拥有数据的 CPU 上发生的其他内存写入是乱序的。)

If you want to use a remote local_read to synchronize access to a resource
between CPUs, explicit ``smp_wmb()`` and ``smp_rmb()`` memory barriers must be used
respectively on the writer and the reader CPUs. It would be the case if you use
the ``local_t`` variable as a counter of bytes written in a buffer: there should
be a ``smp_wmb()`` between the buffer write and the counter increment and also a
``smp_rmb()`` between the counter read and the buffer read.
(如果你想用远程的 local_read 在 CPU 之间同步对某个资源的访问，必须在写者与读者 CPU 上分别显式使用 ``smp_wmb()`` 与 ``smp_rmb()`` 内存屏障。例如把 ``local_t`` 变量用作缓冲区已写入字节数的计数器时：缓冲区写入与计数器递增之间应有 ``smp_wmb()``，计数器读取与缓冲区读取之间应有 ``smp_rmb()``。)


Here is a sample module which implements a basic per cpu counter using
``local.h``::

    /* test-local.c
     *
     * Sample module for local.h usage.
     */


    #include <asm/local.h>
    #include <linux/module.h>
    #include <linux/timer.h>

    static DEFINE_PER_CPU(local_t, counters) = LOCAL_INIT(0);

    static struct timer_list test_timer;

    /* IPI called on each CPU. */
    static void test_each(void *info)
    {
            /* Increment the counter from a non preemptible context */
            printk("Increment on cpu %d\n", smp_processor_id());
            local_inc(this_cpu_ptr(&counters));

            /* This is what incrementing the variable would look like within a
             * preemptible context (it disables preemption) :
             *
             * local_inc(&get_cpu_var(counters));
             * put_cpu_var(counters);
             */
    }

    static void do_test_timer(unsigned long data)
    {
            int cpu;

            /* Increment the counters */
            on_each_cpu(test_each, NULL, 1);
            /* Read all the counters */
            printk("Counters read from CPU %d\n", smp_processor_id());
            for_each_online_cpu(cpu) {
                    printk("Read : CPU %d, count %ld\n", cpu,
                            local_read(&per_cpu(counters, cpu)));
            }
            mod_timer(&test_timer, jiffies + 1000);
    }

    static int __init test_init(void)
    {
            /* initialize the timer that will increment the counter */
            timer_setup(&test_timer, do_test_timer, 0);
            mod_timer(&test_timer, jiffies + 1);

            return 0;
    }

    static void __exit test_exit(void)
    {
            timer_shutdown_sync(&test_timer);
    }

    module_init(test_init);
    module_exit(test_exit);

    MODULE_LICENSE("GPL");
    MODULE_AUTHOR("Mathieu Desnoyers");
    MODULE_DESCRIPTION("Local Atomic Ops");

(下面是一个使用 ``local.h`` 实现基本 per cpu 计数器的示例模块：)
