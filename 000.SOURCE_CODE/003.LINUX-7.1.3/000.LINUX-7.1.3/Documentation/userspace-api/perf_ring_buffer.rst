.. SPDX-License-Identifier: GPL-2.0

===============================
Perf ring buffer(Perf 环形缓冲)
===============================

.. CONTENTS

    1. Introduction

    2. Ring buffer implementation
    2.1  Basic algorithm
    2.2  Ring buffer for different tracing modes
    2.2.1       Default mode
    2.2.2       Per-thread mode
    2.2.3       Per-CPU mode
    2.2.4       System wide mode
    2.3  Accessing buffer
    2.3.1       Producer-consumer model
    2.3.2       Properties of the ring buffers
    2.3.3       Writing samples into buffer
    2.3.4       Reading samples from buffer
    2.3.5       Memory synchronization

    3. The mechanism of AUX ring buffer
    3.1  The relationship between AUX and regular ring buffers
    3.2  AUX events
    3.3  Snapshot mode


1. Introduction(1. 引言)
========================

The ring buffer is a fundamental mechanism for data transfer.  perf uses
ring buffers to transfer event data from kernel to user space, another
kind of ring buffer which is so called auxiliary (AUX) ring buffer also
plays an important role for hardware tracing with Intel PT, Arm
CoreSight, etc.
(环形缓冲是一种基础的数据传输机制。perf 使用环形缓冲把事件数据从内核传递到用户空间；另一种被称为辅助 (AUX) 环形缓冲的环形缓冲，则在 Intel PT、Arm CoreSight 等硬件追踪中扮演着重要角色。)

The ring buffer implementation is critical but it's also a very
challenging work.  On the one hand, the kernel and perf tool in the user
space use the ring buffer to exchange data and stores data into data
file, thus the ring buffer needs to transfer data with high throughput;
on the other hand, the ring buffer management should avoid significant
overload to distract profiling results.
(环形缓冲的实现至关重要，同时也是一项很有挑战性的工作。一方面，内核与用户空间的 perf 工具使用环形缓冲交换数据并把数据存入数据文件，因此环形缓冲需要具备高吞吐量的数据传输能力；另一方面，环形缓冲的管理应避免显著的开销干扰剖析结果。)

This documentation dives into the details for perf ring buffer with two
parts: firstly it explains the perf ring buffer implementation, then the
second part discusses the AUX ring buffer mechanism.
(本文档分两部分深入剖析 perf 环形缓冲的细节：第一部分解释 perf 环形缓冲的实现，第二部分讨论 AUX 环形缓冲的机制。)

2. Ring buffer implementation(2. 环形缓冲的实现)
================================================

2.1 Basic algorithm(2.1 基本算法)
---------------------------------

That said, a typical ring buffer is managed by a head pointer and a tail
pointer; the head pointer is manipulated by a writer and the tail
pointer is updated by a reader respectively.

::

        +---------------------------+
        |   |   |***|***|***|   |   |
        +---------------------------+
                `-> Tail    `-> Head

        * : the data is filled by the writer.

                Figure 1. Ring buffer

(典型的环形缓冲由一个头指针和一个尾指针来管理；头指针由写者操作，尾指针则由读者更新。)

Perf uses the same way to manage its ring buffer.  In the implementation
there are two key data structures held together in a set of consecutive
pages, the control structure and then the ring buffer itself.  The page
with the control structure in is known as the "user page".  Being held
in continuous virtual addresses simplifies locating the ring buffer
address, it is in the pages after the page with the user page.
(Perf 使用同样的方式管理其环形缓冲。在实现中，两个关键数据结构被一起保存在一组连续的页面里：控制结构在前，其后是环形缓冲本身。控制结构所在的页面被称为 "user page"(用户页)。保存在连续的虚拟地址中简化了环形缓冲地址的定位——它就位于用户页之后的页面中。)

The control structure is named as ``perf_event_mmap_page``, it contains a
head pointer ``data_head`` and a tail pointer ``data_tail``.  When the
kernel starts to fill records into the ring buffer, it updates the head
pointer to reserve the memory so later it can safely store events into
the buffer.  On the other side, when the user page is a writable mapping,
the perf tool has the permission to update the tail pointer after consuming
data from the ring buffer.  Yet another case is for the user page's
read-only mapping, which is to be addressed in the section
:ref:`writing_samples_into_buffer`.
(控制结构名为 ``perf_event_mmap_page`` ，它包含头指针 ``data_head`` 和尾指针 ``data_tail`` 。当内核开始向环形缓冲填充记录时，它会先更新头指针以预留内存，之后便可安全地把事件存入缓冲区。另一方面，当用户页是可写映射时，perf 工具有权在消费完环形缓冲中的数据后更新尾指针。还有一种情况是用户页为只读映射，这将在 :ref:`writing_samples_into_buffer` 一节中讨论。)

::

          user page                          ring buffer
    +---------+---------+   +---------------------------------------+
    |data_head|data_tail|...|   |   |***|***|***|***|***|   |   |   |
    +---------+---------+   +---------------------------------------+
        `          `----------------^                   ^
         `----------------------------------------------|

              * : the data is filled by the writer.

                Figure 2. Perf ring buffer

When using the ``perf record`` tool, we can specify the ring buffer size
with option ``-m`` or ``--mmap-pages=``, the given size will be rounded up
to a power of two that is a multiple of a page size.  Though the kernel
allocates at once for all memory pages, it's deferred to map the pages
to VMA area until the perf tool accesses the buffer from the user space.
In other words, at the first time accesses the buffer's page from user
space in the perf tool, a data abort exception for page fault is taken
and the kernel uses this occasion to map the page into process VMA
(see ``perf_mmap_fault()``), thus the perf tool can continue to access
the page after returning from the exception.
(使用 ``perf record`` 工具时，可以通过选项 ``-m`` 或 ``--mmap-pages=`` 指定环形缓冲的大小，给定的大小会被向上取整为页面大小的整数次幂。虽然内核会一次性分配所有内存页，但在 perf 工具从用户空间访问缓冲区之前，这些页面会被推迟映射到 VMA 区域。换句话说，当 perf 工具第一次从用户空间访问缓冲区的页面时，会触发缺页数据中止异常，内核借机把该页面映射到进程的 VMA 中 (见 ``perf_mmap_fault()`` )，于是 perf 工具可以在异常返回后继续访问该页面。)

2.2 Ring buffer for different tracing modes(2.2 不同追踪模式下的环形缓冲)
-------------------------------------------------------------------------

The perf profiles programs with different modes: default mode, per thread
mode, per cpu mode, and system wide mode.  This section describes these
modes and how the ring buffer meets requirements for them.  At last we
will review the race conditions caused by these modes.
(perf 以不同的模式剖析程序：默认模式、每线程模式、每 CPU 模式和全系统模式。本节描述这些模式以及环形缓冲如何满足它们的需求。最后我们会回顾由这些模式引发的竞争条件。)

2.2.1 Default mode(2.2.1 默认模式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Usually we execute ``perf record`` command followed by a profiling program
name, like below command::

        perf record test_program

(通常我们在执行 ``perf record`` 命令时在其后跟一个被剖析的程序名，如下面的命令：)

This command doesn't specify any options for CPU and thread modes, the
perf tool applies the default mode on the perf event.  It maps all the
CPUs in the system and the profiled program's PID on the perf event, and
it enables inheritance mode on the event so that child tasks inherits
the events.  As a result, the perf event is attributed as::

    evsel::cpus::map[]    = { 0 .. _SC_NPROCESSORS_ONLN-1 }
    evsel::threads::map[] = { pid }
    evsel::attr::inherit  = 1

(该命令没有为 CPU 和线程模式指定任何选项，perf 工具对 perf 事件采用默认模式。它把系统中的所有 CPU 以及被剖析程序的 PID 映射到 perf 事件上，并启用事件的继承模式，使子任务继承这些事件。最终，该 perf 事件的属性为：)

These attributions finally will be reflected on the deployment of ring
buffers.  As shown below, the perf tool allocates individual ring buffer
for each CPU, but it only enables events for the profiled program rather
than for all threads in the system.  The *T1* thread represents the
thread context of the 'test_program', whereas *T2* and *T3* are irrelevant
threads in the system.   The perf samples are exclusively collected for
the *T1* thread and stored in the ring buffer associated with the CPU on
which the *T1* thread is running.
(这些属性最终会反映在环形缓冲的部署上。如下所示，perf 工具为每个 CPU 分配独立的环形缓冲，但只为被剖析的程序启用事件，而不是为系统中的所有线程启用。*T1* 线程代表 'test_program' 的线程上下文，而 *T2* 和 *T3* 是系统中无关的线程。perf 采样只为 *T1* 线程收集，并存储在与 *T1* 线程正在运行的 CPU 相关联的环形缓冲中。)

::

              T1                      T2                 T1
            +----+              +-----------+          +----+
    CPU0    |xxxx|              |xxxxxxxxxxx|          |xxxx|
            +----+--------------+-----------+----------+----+-------->
              |                                          |
              v                                          v
            +-----------------------------------------------------+
            |                  Ring buffer 0                      |
            +-----------------------------------------------------+

                   T1
                 +-----+
    CPU1         |xxxxx|
            -----+-----+--------------------------------------------->
                    |
                    v
            +-----------------------------------------------------+
            |                  Ring buffer 1                      |
            +-----------------------------------------------------+

                                        T1              T3
                                      +----+        +-------+
    CPU2                              |xxxx|        |xxxxxxx|
            --------------------------+----+--------+-------+-------->
                                        |
                                        v
            +-----------------------------------------------------+
            |                  Ring buffer 2                      |
            +-----------------------------------------------------+

                              T1
                       +--------------+
    CPU3               |xxxxxxxxxxxxxx|
            -----------+--------------+------------------------------>
                              |
                              v
            +-----------------------------------------------------+
            |                  Ring buffer 3                      |
            +-----------------------------------------------------+

	    T1: Thread 1; T2: Thread 2; T3: Thread 3
	    x: Thread is in running state

                Figure 3. Ring buffer for default mode

2.2.2 Per-thread mode(2.2.2 每线程模式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By specifying option ``--per-thread`` in perf command, e.g.

::

        perf record --per-thread test_program

(在 perf 命令中指定选项 ``--per-thread`` ，例如：)

The perf event doesn't map to any CPUs and is only bound to the
profiled process, thus, the perf event's attributions are::

    evsel::cpus::map[0]   = { -1 }
    evsel::threads::map[] = { pid }
    evsel::attr::inherit  = 0

(该 perf 事件不映射到任何 CPU，只绑定到被剖析的进程，因此该 perf 事件的属性为：)

In this mode, a single ring buffer is allocated for the profiled thread;
if the thread is scheduled on a CPU, the events on that CPU will be
enabled; and if the thread is scheduled out from the CPU, the events on
the CPU will be disabled.  When the thread is migrated from one CPU to
another, the events are to be disabled on the previous CPU and enabled
on the next CPU correspondingly.
(在此模式下，只为被剖析的线程分配一个环形缓冲；如果该线程被调度到某个 CPU 上，该 CPU 上的事件将被启用；如果该线程被调度出该 CPU，该 CPU 上的事件将被禁用。当线程从一个 CPU 迁移到另一个 CPU 时，事件会在前一个 CPU 上被禁用，并在下一个 CPU 上相应启用。)

::

              T1                      T2                 T1
            +----+              +-----------+          +----+
    CPU0    |xxxx|              |xxxxxxxxxxx|          |xxxx|
            +----+--------------+-----------+----------+----+-------->
              |                                           |
              |    T1                                     |
              |  +-----+                                  |
    CPU1      |  |xxxxx|                                  |
            --|--+-----+----------------------------------|---------->
              |     |                                     |
              |     |                   T1            T3  |
              |     |                 +----+        +---+ |
    CPU2      |     |                 |xxxx|        |xxx| |
            --|-----|-----------------+----+--------+---+-|---------->
              |     |                   |                 |
              |     |         T1        |                 |
              |     |  +--------------+ |                 |
    CPU3      |     |  |xxxxxxxxxxxxxx| |                 |
            --|-----|--+--------------+-|-----------------|---------->
              |     |         |         |                 |
              v     v         v         v                 v
            +-----------------------------------------------------+
            |                  Ring buffer                        |
            +-----------------------------------------------------+

            T1: Thread 1
            x: Thread is in running state

                Figure 4. Ring buffer for per-thread mode

When perf runs in per-thread mode, a ring buffer is allocated for the
profiled thread *T1*.  The ring buffer is dedicated for thread *T1*, if the
thread *T1* is running, the perf events will be recorded into the ring
buffer; when the thread is sleeping, all associated events will be
disabled, thus no trace data will be recorded into the ring buffer.
(当 perf 以每线程模式运行时，会为被剖析的线程 *T1* 分配一个环形缓冲。该环形缓冲专用于线程 *T1* ：如果线程 *T1* 在运行，perf 事件就会被记录到环形缓冲中；当线程睡眠时，所有相关事件都会被禁用，因此不会有追踪数据被记录到环形缓冲中。)

2.2.3 Per-CPU mode(2.2.3 每 CPU 模式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The option ``-C`` is used to collect samples on the list of CPUs, for
example the below perf command receives option ``-C 0,2``::

	perf record -C 0,2 test_program

(选项 ``-C`` 用于在给定的 CPU 列表上收集采样，例如下面的 perf 命令接收选项 ``-C 0,2``：)

It maps the perf event to CPUs 0 and 2, and the event is not associated to any
PID.  Thus the perf event attributions are set as::

    evsel::cpus::map[0]   = { 0, 2 }
    evsel::threads::map[] = { -1 }
    evsel::attr::inherit  = 0

(它把 perf 事件映射到 CPU 0 和 CPU 2，且该事件不关联任何 PID。因此该 perf 事件的属性被设置为：)

This results in the session of ``perf record`` will sample all threads on CPU0
and CPU2, and be terminated until test_program exits.  Even there have tasks
running on CPU1 and CPU3, since the ring buffer is absent for them, any
activities on these two CPUs will be ignored.  A usage case is to combine the
options for per-thread mode and per-CPU mode, e.g. the options ``–C 0,2`` and
``––per–thread`` are specified together, the samples are recorded only when
the profiled thread is scheduled on any of the listed CPUs.
(这使得 ``perf record`` 会话会对 CPU0 和 CPU2 上的所有线程进行采样，直到 test_program 退出才终止。即使 CPU1 和 CPU3 上有任务在运行，由于它们没有对应的环形缓冲，这两个 CPU 上的任何活动都会被忽略。一种用法是把每线程模式与每 CPU 模式的选项组合起来，例如同时指定 ``–C 0,2`` 和 ``––per–thread`` 选项，只有当被剖析的线程被调度到列出的任一 CPU 上时才会记录采样。)

::

              T1                      T2                 T1
            +----+              +-----------+          +----+
    CPU0    |xxxx|              |xxxxxxxxxxx|          |xxxx|
            +----+--------------+-----------+----------+----+-------->
              |                       |                  |
              v                       v                  v
            +-----------------------------------------------------+
            |                  Ring buffer 0                      |
            +-----------------------------------------------------+

                   T1
                 +-----+
    CPU1         |xxxxx|
            -----+-----+--------------------------------------------->

                                        T1              T3
                                      +----+        +-------+
    CPU2                              |xxxx|        |xxxxxxx|
            --------------------------+----+--------+-------+-------->
                                        |               |
                                        v               v
            +-----------------------------------------------------+
            |                  Ring buffer 1                      |
            +-----------------------------------------------------+

                              T1
                       +--------------+
    CPU3               |xxxxxxxxxxxxxx|
            -----------+--------------+------------------------------>

            T1: Thread 1; T2: Thread 2; T3: Thread 3
            x: Thread is in running state

                Figure 5. Ring buffer for per-CPU mode

2.2.4 System wide mode(2.2.4 全系统模式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By using option ``–a`` or ``––all–cpus``, perf collects samples on all CPUs
for all tasks, we call it as the system wide mode, the command is::

        perf record -a test_program

(使用选项 ``–a`` 或 ``––all–cpus`` 时，perf 会收集所有 CPU 上所有任务的采样，我们称之为全系统模式，命令为：)

Similar to the per-CPU mode, the perf event doesn't bind to any PID, and
it maps to all CPUs in the system::

   evsel::cpus::map[]    = { 0 .. _SC_NPROCESSORS_ONLN-1 }
   evsel::threads::map[] = { -1 }
   evsel::attr::inherit  = 0

(与每 CPU 模式类似，该 perf 事件不绑定任何 PID，并映射到系统中的所有 CPU：)

In the system wide mode, every CPU has its own ring buffer, all threads
are monitored during the running state and the samples are recorded into
the ring buffer belonging to the CPU which the events occurred on.
(在全系统模式下，每个 CPU 都有自己的环形缓冲；所有线程在运行态期间都会被监控，采样被记录到事件发生时所在 CPU 的环形缓冲中。)

::

              T1                      T2                 T1
            +----+              +-----------+          +----+
    CPU0    |xxxx|              |xxxxxxxxxxx|          |xxxx|
            +----+--------------+-----------+----------+----+-------->
              |                       |                  |
              v                       v                  v
            +-----------------------------------------------------+
            |                  Ring buffer 0                      |
            +-----------------------------------------------------+

                   T1
                 +-----+
    CPU1         |xxxxx|
            -----+-----+--------------------------------------------->
                    |
                    v
            +-----------------------------------------------------+
            |                  Ring buffer 1                      |
            +-----------------------------------------------------+

                                        T1              T3
                                      +----+        +-------+
    CPU2                              |xxxx|        |xxxxxxx|
            --------------------------+----+--------+-------+-------->
                                        |               |
                                        v               v
            +-----------------------------------------------------+
            |                  Ring buffer 2                      |
            +-----------------------------------------------------+

                              T1
                       +--------------+
    CPU3               |xxxxxxxxxxxxxx|
            -----------+--------------+------------------------------>
                              |
                              v
            +-----------------------------------------------------+
            |                  Ring buffer 3                      |
            +-----------------------------------------------------+

            T1: Thread 1; T2: Thread 2; T3: Thread 3
            x: Thread is in running state

                Figure 6. Ring buffer for system wide mode

2.3 Accessing buffer(2.3 访问缓冲区)
------------------------------------

Based on the understanding of how the ring buffer is allocated in
various modes, this section explains access the ring buffer.
(在理解了各种模式下环形缓冲如何分配的基础上，本节说明如何访问环形缓冲。)

2.3.1 Producer-consumer model(2.3.1 生产者-消费者模型)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the Linux kernel, the PMU events can produce samples which are stored
into the ring buffer; the perf command in user space consumes the
samples by reading out data from the ring buffer and finally saves the
data into the file for post analysis.  It’s a typical producer-consumer
model for using the ring buffer.
(在 Linux 内核中，PMU 事件可以产生采样并存储到环形缓冲中；用户空间的 perf 命令通过从环形缓冲读出数据来消费采样，最终把数据保存到文件中以便事后分析。这是使用环形缓冲的典型生产者-消费者模型。)

The perf process polls on the PMU events and sleeps when no events are
incoming.  To prevent frequent exchanges between the kernel and user
space, the kernel event core layer introduces a watermark, which is
stored in the ``perf_buffer::watermark``.  When a sample is recorded into
the ring buffer, and if the used buffer exceeds the watermark, the
kernel wakes up the perf process to read samples from the ring buffer.
(perf 进程轮询 PMU 事件，在没有事件到来时睡眠。为了避免内核与用户空间之间的频繁交互，内核事件核心层引入了水位线 (watermark)，存储在 ``perf_buffer::watermark`` 中。当采样被记录到环形缓冲，且已用缓冲超过水位线时，内核会唤醒 perf 进程去读取环形缓冲中的采样。)

::

                       Perf
                       / | Read samples
             Polling  /  `--------------|               Ring buffer
                     v                  v    ;---------------------v
    +----------------+     +---------+---------+   +-------------------+
    |Event wait queue|     |data_head|data_tail|   |***|***|   |   |***|
    +----------------+     +---------+---------+   +-------------------+
             ^                  ^ `------------------------^
             | Wake up tasks    | Store samples
          +-----------------------------+
          |  Kernel event core layer    |
          +-----------------------------+

              * : the data is filled by the writer.

                Figure 7. Writing and reading the ring buffer

When the kernel event core layer notifies the user space, because
multiple events might share the same ring buffer for recording samples,
the core layer iterates every event associated with the ring buffer and
wakes up tasks waiting on the event.  This is fulfilled by the kernel
function ``ring_buffer_wakeup()``.
(当内核事件核心层通知用户空间时，由于多个事件可能共享同一个环形缓冲来记录采样，核心层会遍历与该环形缓冲关联的每一个事件，并唤醒等待该事件的任务。这由内核函数 ``ring_buffer_wakeup()`` 来完成。)

After the perf process is woken up, it starts to check the ring buffers
one by one, if it finds any ring buffer containing samples it will read
out the samples for statistics or saving into the data file.  Given the
perf process is able to run on any CPU, this leads to the ring buffer
potentially being accessed from multiple CPUs simultaneously, which
causes race conditions.  The race condition handling is described in the
section :ref:`memory_synchronization`.
(perf 进程被唤醒后，开始逐个检查环形缓冲；一旦发现某个环形缓冲中有采样，就会读出这些采样用于统计或保存到数据文件。由于 perf 进程可能在任意 CPU 上运行，这会导致环形缓冲可能被多个 CPU 同时访问，从而引发竞争条件。竞争条件的处理在 :ref:`memory_synchronization` 一节中描述。)

2.3.2 Properties of the ring buffers(2.3.2 环形缓冲的特性)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Linux kernel supports two write directions for the ring buffer: forward and
backward.  The forward writing saves samples from the beginning of the ring
buffer, the backward writing stores data from the end of the ring buffer with
the reversed direction.  The perf tool determines the writing direction.
(Linux 内核支持两种环形缓冲写入方向：正向和反向。正向写入从环形缓冲的开头保存采样；反向写入则从环形缓冲的末尾开始、以相反的方向存储数据。写入方向由 perf 工具决定。)

Additionally, the tool can map buffers in either read-write mode or read-only
mode to the user space.
(此外，perf 工具可以把缓冲区以读写模式或只读模式映射到用户空间。)

The ring buffer in the read-write mode is mapped with the property
``PROT_READ | PROT_WRITE``.  With the write permission, the perf tool
updates the ``data_tail`` to indicate the data start position.  Combining
with the head pointer ``data_head``, which works as the end position of
the current data, the perf tool can easily know where read out the data
from.
(读写模式下的环形缓冲以 ``PROT_READ | PROT_WRITE`` 属性映射。凭借写权限，perf 工具可以更新 ``data_tail`` 来指示数据起始位置；再结合作为当前数据末尾位置的头指针 ``data_head`` ，perf 工具就能轻松知道从哪里读出数据。)

Alternatively, in the read-only mode, only the kernel keeps to update
the ``data_head`` while the user space cannot access the ``data_tail`` due
to the mapping property ``PROT_READ``.
(而在只读模式下，只有内核持续更新 ``data_head`` ，用户空间由于映射属性为 ``PROT_READ`` 而无法访问 ``data_tail`` 。)

As a result, the matrix below illustrates the various combinations of
direction and mapping characteristics.  The perf tool employs two of these
combinations to support buffer types: the non-overwrite buffer and the
overwritable buffer.
(因此，下表展示了方向与映射特性的各种组合。perf 工具采用其中两种组合来支持缓冲区类型：非覆盖式缓冲区和可覆盖式缓冲区。)

.. list-table::
   :widths: 1 1 1
   :header-rows: 1

   * - Mapping mode(映射模式)
     - Forward(正向)
     - Backward(反向)
   * - read-write(读写)
     - Non-overwrite ring buffer(非覆盖式环形缓冲)
     - Not used(未使用)
   * - read-only(只读)
     - Not used(未使用)
     - Overwritable ring buffer(可覆盖式环形缓冲)

The non-overwrite ring buffer uses the read-write mapping with forward
writing.  It starts to save data from the beginning of the ring buffer
and wrap around when overflow, which is used with the read-write mode in
the normal ring buffer.  When the consumer doesn't keep up with the
producer, it would lose some data, the kernel keeps how many records it
lost and generates the ``PERF_RECORD_LOST`` records in the next time
when it finds a space in the ring buffer.
(非覆盖式环形缓冲使用正向写入的读写映射。它从环形缓冲的开头开始保存数据，溢出时回绕，普通环形缓冲即以读写模式使用这种方式。当消费者跟不上生产者时会丢失一些数据，内核会记录丢失了多少条记录，并在下一次在环形缓冲中找到空间时生成 ``PERF_RECORD_LOST`` 记录。)

The overwritable ring buffer uses the backward writing with the
read-only mode.  It saves the data from the end of the ring buffer and
the ``data_head`` keeps the position of current data, the perf always
knows where it starts to read and until the end of the ring buffer, thus
it don't need the ``data_tail``.  In this mode, it will not generate the
``PERF_RECORD_LOST`` records.
(可覆盖式环形缓冲使用反向写入的只读模式。它从环形缓冲的末尾开始保存数据， ``data_head`` 保存当前数据的位置，perf 始终知道从哪里开始读、一直读到环形缓冲的末尾，因此不需要 ``data_tail`` 。在此模式下不会生成 ``PERF_RECORD_LOST`` 记录。)

.. _writing_samples_into_buffer:

2.3.3 Writing samples into buffer(2.3.3 将采样写入缓冲区)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When a sample is taken and saved into the ring buffer, the kernel
prepares sample fields based on the sample type; then it prepares the
info for writing ring buffer which is stored in the structure
``perf_output_handle``.  In the end, the kernel outputs the sample into
the ring buffer and updates the head pointer in the user page so the
perf tool can see the latest value.
(当采样被采集并保存到环形缓冲时，内核先根据采样类型准备采样字段；然后准备写入环形缓冲所需的信息，这些信息保存在结构体 ``perf_output_handle`` 中。最后，内核把采样输出到环形缓冲，并更新用户页中的头指针，使 perf 工具能看到最新的值。)

The structure ``perf_output_handle`` serves as a temporary context for
tracking the information related to the buffer.  The advantages of it is
that it enables concurrent writing to the buffer by different events.
For example, a software event and a hardware PMU event both are enabled
for profiling, two instances of ``perf_output_handle`` serve as separate
contexts for the software event and the hardware event respectively.
This allows each event to reserve its own memory space for populating
the record data.
(结构体 ``perf_output_handle`` 充当跟踪缓冲区相关信息的临时上下文。它的优点是允许不同事件并发地向缓冲区写入。例如，一个软件事件和一个硬件 PMU 事件同时被启用进行剖析时，两个 ``perf_output_handle`` 实例分别作为软件事件和硬件事件的独立上下文，使每个事件都能预留自己的内存空间来填充记录数据。)

2.3.4 Reading samples from buffer(2.3.4 从缓冲区读取采样)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the user space, the perf tool utilizes the ``perf_event_mmap_page``
structure to handle the head and tail of the buffer.  It also uses
``perf_mmap`` structure to keep track of a context for the ring buffer, this
context includes information about the buffer's starting and ending
addresses.  Additionally, the mask value can be utilized to compute the
circular buffer pointer even for an overflow.
(在用户空间，perf 工具利用 ``perf_event_mmap_page`` 结构体来处理缓冲区的头和尾。它还使用 ``perf_mmap`` 结构体来维护环形缓冲的上下文，该上下文包含缓冲区起始和结束地址的信息。此外，即使发生溢出，也可以利用掩码值来计算循环缓冲区指针。)

Similar to the kernel, the perf tool in the user space first reads out
the recorded data from the ring buffer, and then updates the buffer's
tail pointer ``perf_event_mmap_page::data_tail``.
(与内核类似，用户空间的 perf 工具先从环形缓冲读出已记录的数据，然后更新缓冲区的尾指针 ``perf_event_mmap_page::data_tail`` 。)

.. _memory_synchronization:

2.3.5 Memory synchronization(2.3.5 内存同步)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The modern CPUs with relaxed memory model cannot promise the memory
ordering, this means it’s possible to access the ring buffer and the
``perf_event_mmap_page`` structure out of order.  To assure the specific
sequence for memory accessing perf ring buffer, memory barriers are
used to assure the data dependency.  The rationale for the memory
synchronization is as below::

  Kernel                          User space

  if (LOAD ->data_tail) {         LOAD ->data_head
                   (A)            smp_rmb()        (C)
    STORE $data                   LOAD $data
    smp_wmb()      (B)            smp_mb()         (D)
    STORE ->data_head             STORE ->data_tail
  }

(采用宽松内存模型的现代 CPU 无法保证内存访问顺序，这意味着对环形缓冲和 ``perf_event_mmap_page`` 结构体的访问可能乱序发生。为了确保访问 perf 环形缓冲时的特定顺序，需要使用内存屏障来保证数据依赖。内存同步的基本原理如下：)

The comments in tools/include/linux/ring_buffer.h gives nice description
for why and how to use memory barriers, here we will just provide an
alternative explanation:
(tools/include/linux/ring_buffer.h 中的注释对为什么以及如何使用内存屏障给出了很好的说明，这里我们只提供另一种解释：)

(A) is a control dependency so that CPU assures order between checking
pointer ``perf_event_mmap_page::data_tail`` and filling sample into ring
buffer;
((A) 是控制依赖，用于让 CPU 保证检查指针 ``perf_event_mmap_page::data_tail`` 与向环形缓冲填充采样这两步之间的顺序。)

(D) pairs with (A).  (D) separates the ring buffer data reading from
writing the pointer ``data_tail``, perf tool first consumes samples and then
tells the kernel that the data chunk has been released.  Since a reading
operation is followed by a writing operation, thus (D) is a full memory
barrier.
((D) 与 (A) 配对。(D) 把环形缓冲数据的读取与指针 ``data_tail`` 的写入分开，perf 工具先消费采样，然后告诉内核该数据块已被释放。由于读操作之后紧跟写操作，所以 (D) 是全内存屏障。)

(B) is a writing barrier in the middle of two writing operations, which
makes sure that recording a sample must be prior to updating the head
pointer.
((B) 是位于两个写操作之间的写屏障，确保记录采样必须先于更新头指针。)

(C) pairs with (B).  (C) is a read memory barrier to ensure the head
pointer is fetched before reading samples.
((C) 与 (B) 配对。(C) 是读内存屏障，确保在读取采样之前先获取头指针。)

To implement the above algorithm, the ``perf_output_put_handle()`` function
in the kernel and two helpers ``ring_buffer_read_head()`` and
``ring_buffer_write_tail()`` in the user space are introduced, they rely
on memory barriers as described above to ensure the data dependency.
(为实现上述算法，内核中引入了 ``perf_output_put_handle()`` 函数，用户空间中引入了 ``ring_buffer_read_head()`` 和 ``ring_buffer_write_tail()`` 两个辅助函数，它们依赖如前所述的内存屏障来保证数据依赖。)

Some architectures support one-way permeable barrier with load-acquire
and store-release operations, these barriers are more relaxed with less
performance penalty, so (C) and (D) can be optimized to use barriers
``smp_load_acquire()`` and ``smp_store_release()`` respectively.
(一些架构通过 load-acquire 和 store-release 操作支持单向渗透屏障，这些屏障更宽松、性能开销更小，因此 (C) 和 (D) 可以分别优化为使用 ``smp_load_acquire()`` 和 ``smp_store_release()`` 屏障。)

If an architecture doesn’t support load-acquire and store-release in its
memory model, it will roll back to the old fashion of memory barrier
operations.  In this case, ``smp_load_acquire()`` encapsulates
``READ_ONCE()`` + ``smp_mb()``, since ``smp_mb()`` is costly,
``ring_buffer_read_head()`` doesn't invoke ``smp_load_acquire()`` and it uses
the barriers ``READ_ONCE()`` + ``smp_rmb()`` instead.
(如果架构的内存模型不支持 load-acquire 和 store-release，就会回退到老式的内存屏障操作。在这种情况下， ``smp_load_acquire()`` 封装的是 ``READ_ONCE()`` + ``smp_mb()`` ，由于 ``smp_mb()`` 代价高昂， ``ring_buffer_read_head()`` 不调用 ``smp_load_acquire()`` ，而是改用 ``READ_ONCE()`` + ``smp_rmb()`` 屏障。)

3. The mechanism of AUX ring buffer(3. AUX 环形缓冲的机制)
==========================================================

In this chapter, we will explain the implementation of the AUX ring
buffer.  In the first part it will discuss the connection between the
AUX ring buffer and the regular ring buffer, then the second part will
examine how the AUX ring buffer co-works with the regular ring buffer,
as well as the additional features introduced by the AUX ring buffer for
the sampling mechanism.
(在本章中，我们将解释 AUX 环形缓冲的实现。第一部分讨论 AUX 环形缓冲与常规环形缓冲之间的联系，第二部分研究 AUX 环形缓冲如何与常规环形缓冲协同工作，以及 AUX 环形缓冲为采样机制引入的附加特性。)

3.1 The relationship between AUX and regular ring buffers(3.1 AUX 环形缓冲与常规环形缓冲的关系)
-----------------------------------------------------------------------------------------------

Generally, the AUX ring buffer is an auxiliary for the regular ring
buffer.  The regular ring buffer is primarily used to store the event
samples and every event format complies with the definition in the
union ``perf_event``; the AUX ring buffer is for recording the hardware
trace data and the trace data format is hardware IP dependent.
(总体而言，AUX 环形缓冲是常规环形缓冲的辅助。常规环形缓冲主要用于存储事件采样，每个事件的格式都符合联合体 ``perf_event`` 中的定义；AUX 环形缓冲用于记录硬件追踪数据，追踪数据的格式取决于硬件 IP。)

The general use and advantage of the AUX ring buffer is that it is
written directly by hardware rather than by the kernel.  For example,
regular profile samples that write to the regular ring buffer cause an
interrupt.  Tracing execution requires a high number of samples and
using interrupts would be overwhelming for the regular ring buffer
mechanism.  Having an AUX buffer allows for a region of memory more
decoupled from the kernel and written to directly by hardware tracing.
(AUX 环形缓冲的通用用法和优势在于它由硬件直接写入，而不是由内核写入。例如，写入常规环形缓冲的常规剖析采样会引发中断。执行追踪需要大量的采样，使用中断对常规环形缓冲机制来说将难以承受。有了 AUX 缓冲，就可以有一块与内核更加解耦、由硬件追踪直接写入的内存区域。)

The AUX ring buffer reuses the same algorithm with the regular ring
buffer for the buffer management.  The control structure
``perf_event_mmap_page`` extends the new fields ``aux_head`` and ``aux_tail``
for the head and tail pointers of the AUX ring buffer.
(AUX 环形缓冲在缓冲区管理上复用了与常规环形缓冲相同的算法。控制结构 ``perf_event_mmap_page`` 扩展了新字段 ``aux_head`` 和 ``aux_tail`` ，用作 AUX 环形缓冲的头指针和尾指针。)

During the initialisation phase, besides the mmap()-ed regular ring
buffer, the perf tool invokes a second syscall in the
``auxtrace_mmap__mmap()`` function for the mmap of the AUX buffer with
non-zero file offset; ``rb_alloc_aux()`` in the kernel allocates pages
correspondingly, these pages will be deferred to map into VMA when
handling the page fault, which is the same lazy mechanism with the
regular ring buffer.
(在初始化阶段，除了 mmap() 的常规环形缓冲之外，perf 工具还会在 ``auxtrace_mmap__mmap()`` 函数中以非零文件偏移发起第二个系统调用来 mmap AUX 缓冲区；内核中的 ``rb_alloc_aux()`` 相应地分配页面，这些页面会在处理缺页异常时才被延迟映射进 VMA，与常规环形缓冲采用同样的惰性机制。)

AUX events and AUX trace data are two different things.  Let's see an
example::

        perf record -a -e cycles -e cs_etm// -- sleep 2

(AUX 事件与 AUX 追踪数据是两个不同的概念。让我们看一个例子：)

The above command enables two events: one is the event *cycles* from PMU
and another is the AUX event *cs_etm* from Arm CoreSight, both are saved
into the regular ring buffer while the CoreSight's AUX trace data is
stored in the AUX ring buffer.
(上面的命令启用了两个事件：一个是来自 PMU 的 *cycles* 事件，另一个是来自 Arm CoreSight 的 AUX 事件 *cs_etm* ，两者都保存到常规环形缓冲中，而 CoreSight 的 AUX 追踪数据则存储在 AUX 环形缓冲中。)

As a result, we can see the regular ring buffer and the AUX ring buffer
are allocated in pairs.  The perf in default mode allocates the regular
ring buffer and the AUX ring buffer per CPU-wise, which is the same as
the system wide mode, however, the default mode records samples only for
the profiled program, whereas the latter mode profiles for all programs
in the system.  For per-thread mode, the perf tool allocates only one
regular ring buffer and one AUX ring buffer for the whole session.  For
the per-CPU mode, the perf allocates two kinds of ring buffers for
selected CPUs specified by the option ``-C``.
(由此可见，常规环形缓冲与 AUX 环形缓冲是成对分配的。默认模式下的 perf 按 CPU 为单位分配常规环形缓冲和 AUX 环形缓冲，这一点与全系统模式相同，但默认模式只为被剖析的程序记录采样，而后者对系统中的所有程序进行剖析。每线程模式下，perf 工具在整个会话中只分配一个常规环形缓冲和一个 AUX 环形缓冲。每 CPU 模式下，perf 为选项 ``-C`` 指定的所选 CPU 分配这两种环形缓冲。)

The below figure demonstrates the buffers' layout in the system wide
mode; if there are any activities on one CPU, the AUX event samples and
the hardware trace data will be recorded into the dedicated buffers for
the CPU.
(下图展示了全系统模式下缓冲区的布局；如果某个 CPU 上有任何活动，AUX 事件采样和硬件追踪数据都会被记录到该 CPU 专用的缓冲区中。)

::

              T1                      T2                 T1
            +----+              +-----------+          +----+
    CPU0    |xxxx|              |xxxxxxxxxxx|          |xxxx|
            +----+--------------+-----------+----------+----+-------->
              |                       |                  |
              v                       v                  v
            +-----------------------------------------------------+
            |                  Ring buffer 0                      |
            +-----------------------------------------------------+
              |                       |                  |
              v                       v                  v
            +-----------------------------------------------------+
            |               AUX Ring buffer 0                     |
            +-----------------------------------------------------+

                   T1
                 +-----+
    CPU1         |xxxxx|
            -----+-----+--------------------------------------------->
                    |
                    v
            +-----------------------------------------------------+
            |                  Ring buffer 1                      |
            +-----------------------------------------------------+
                    |
                    v
            +-----------------------------------------------------+
            |               AUX Ring buffer 1                     |
            +-----------------------------------------------------+

                                        T1              T3
                                      +----+        +-------+
    CPU2                              |xxxx|        |xxxxxxx|
            --------------------------+----+--------+-------+-------->
                                        |               |
                                        v               v
            +-----------------------------------------------------+
            |                  Ring buffer 2                      |
            +-----------------------------------------------------+
                                        |               |
                                        v               v
            +-----------------------------------------------------+
            |               AUX Ring buffer 2                     |
            +-----------------------------------------------------+

                              T1
                       +--------------+
    CPU3               |xxxxxxxxxxxxxx|
            -----------+--------------+------------------------------>
                              |
                              v
            +-----------------------------------------------------+
            |                  Ring buffer 3                      |
            +-----------------------------------------------------+
                              |
                              v
            +-----------------------------------------------------+
            |               AUX Ring buffer 3                     |
            +-----------------------------------------------------+

            T1: Thread 1; T2: Thread 2; T3: Thread 3
            x: Thread is in running state

                Figure 8. AUX ring buffer for system wide mode

3.2 AUX events(3.2 AUX 事件)
----------------------------

Similar to ``perf_output_begin()`` and ``perf_output_end()``'s working for the
regular ring buffer, ``perf_aux_output_begin()`` and ``perf_aux_output_end()``
serve for the AUX ring buffer for processing the hardware trace data.
(类似于 ``perf_output_begin()`` 和 ``perf_output_end()`` 在常规环形缓冲上的工作方式， ``perf_aux_output_begin()`` 和 ``perf_aux_output_end()`` 为 AUX 环形缓冲处理硬件追踪数据提供服务。)

Once the hardware trace data is stored into the AUX ring buffer, the PMU
driver will stop hardware tracing by calling the ``pmu::stop()`` callback.
Similar to the regular ring buffer, the AUX ring buffer needs to apply
the memory synchronization mechanism as discussed in the section
:ref:`memory_synchronization`.  Since the AUX ring buffer is managed by the
PMU driver, the barrier (B), which is a writing barrier to ensure the trace
data is externally visible prior to updating the head pointer, is asked
to be implemented in the PMU driver.
(一旦硬件追踪数据被存入 AUX 环形缓冲，PMU 驱动就会调用 ``pmu::stop()`` 回调来停止硬件追踪。与常规环形缓冲一样，AUX 环形缓冲也需要应用 :ref:`memory_synchronization` 一节中讨论的内存同步机制。由于 AUX 环形缓冲由 PMU 驱动管理，屏障 (B)——一种写屏障，用于确保追踪数据在更新头指针之前对外部可见——被要求在 PMU 驱动中实现。)

Then ``pmu::stop()`` can safely call the ``perf_aux_output_end()`` function to
finish two things:
(然后 ``pmu::stop()`` 就可以安全地调用 ``perf_aux_output_end()`` 函数来完成两件事：)

- It fills an event ``PERF_RECORD_AUX`` into the regular ring buffer, this
  event delivers the information of the start address and data size for a
  chunk of hardware trace data has been stored into the AUX ring buffer;
  (它向常规环形缓冲填充一个 ``PERF_RECORD_AUX`` 事件，该事件传递一块硬件追踪数据已存入 AUX 环形缓冲这一信息，包括起始地址和数据大小；)

- Since the hardware trace driver has stored new trace data into the AUX
  ring buffer, the argument *size* indicates how many bytes have been
  consumed by the hardware tracing, thus ``perf_aux_output_end()`` updates the
  header pointer ``perf_buffer::aux_head`` to reflect the latest buffer usage.
  (由于硬件追踪驱动已把新的追踪数据存入 AUX 环形缓冲，参数 *size* 指示硬件追踪消耗了多少字节，因此 ``perf_aux_output_end()`` 会更新头指针 ``perf_buffer::aux_head`` 以反映最新的缓冲区使用情况。)

At the end, the PMU driver will restart hardware tracing.  During this
temporary suspending period, it will lose hardware trace data, which
will introduce a discontinuity during decoding phase.
(最后，PMU 驱动会重启硬件追踪。在这段临时挂起期间会丢失硬件追踪数据，从而在解码阶段引入不连续。)

The event ``PERF_RECORD_AUX`` presents an AUX event which is handled in the
kernel, but it lacks the information for saving the AUX trace data in
the perf file.  When the perf tool copies the trace data from AUX ring
buffer to the perf data file, it synthesizes a ``PERF_RECORD_AUXTRACE``
event which is not a kernel ABI, it's defined by the perf tool to describe
which portion of data in the AUX ring buffer is saved.  Afterwards, the perf
tool reads out the AUX trace data from the perf file based on the
``PERF_RECORD_AUXTRACE`` events, and the ``PERF_RECORD_AUX`` event is used to
decode a chunk of data by correlating with time order.
(事件 ``PERF_RECORD_AUX`` 表示一个由内核处理的 AUX 事件，但它缺少把 AUX 追踪数据保存到 perf 文件所需的信息。当 perf 工具把追踪数据从 AUX 环形缓冲复制到 perf 数据文件时，它会合成一个 ``PERF_RECORD_AUXTRACE`` 事件——这不是内核 ABI，而是由 perf 工具定义的，用于描述 AUX 环形缓冲中的哪一段数据被保存了。之后，perf 工具基于 ``PERF_RECORD_AUXTRACE`` 事件从 perf 文件中读出 AUX 追踪数据，而 ``PERF_RECORD_AUX`` 事件则用于按时间顺序关联解码一段数据。)

3.3 Snapshot mode(3.3 快照模式)
-------------------------------

Perf supports snapshot mode for AUX ring buffer, in this mode, users
only record AUX trace data at a specific time point which users are
interested in.  E.g. below gives an example of how to take snapshots
with 1 second interval with Arm CoreSight::

  perf record -e cs_etm//u -S -a program &
  PERFPID=$!
  while true; do
      kill -USR2 $PERFPID
      sleep 1
  done

(perf 为 AUX 环形缓冲支持快照模式，在此模式下，用户只在感兴趣的特定时间点记录 AUX 追踪数据。例如，下面展示了如何以 1 秒的间隔配合 Arm CoreSight 采集快照：)

The main flow for snapshot mode is:
(快照模式的主流程如下：)

- Before a snapshot is taken, the AUX ring buffer acts in free run mode.
  During free run mode the perf doesn't record any of the AUX events and
  trace data;
  (在采集快照之前，AUX 环形缓冲以自由运行 (free run) 模式工作。在自由运行模式下，perf 不记录任何 AUX 事件和追踪数据；)

- Once the perf tool receives the *USR2* signal, it triggers the callback
  function ``auxtrace_record::snapshot_start()`` to deactivate hardware
  tracing.  The kernel driver then populates the AUX ring buffer with the
  hardware trace data, and the event ``PERF_RECORD_AUX`` is stored in the
  regular ring buffer;
  (一旦 perf 工具收到 *USR2* 信号，它就触发回调函数 ``auxtrace_record::snapshot_start()`` 来停用硬件追踪。随后内核驱动把硬件追踪数据填充到 AUX 环形缓冲中，并把事件 ``PERF_RECORD_AUX`` 存入常规环形缓冲；)

- Then perf tool takes a snapshot, ``record__read_auxtrace_snapshot()``
  reads out the hardware trace data from the AUX ring buffer and saves it
  into perf data file;
  (然后 perf 工具采集快照， ``record__read_auxtrace_snapshot()`` 从 AUX 环形缓冲读出硬件追踪数据并保存到 perf 数据文件中；)

- After the snapshot is finished, ``auxtrace_record::snapshot_finish()``
  restarts the PMU event for AUX tracing.
  (快照完成后， ``auxtrace_record::snapshot_finish()`` 重启 PMU 事件以进行 AUX 追踪。)

The perf only accesses the head pointer ``perf_event_mmap_page::aux_head``
in snapshot mode and doesn’t touch tail pointer ``aux_tail``, this is
because the AUX ring buffer can overflow in free run mode, the tail
pointer is useless in this case.  Alternatively, the callback
``auxtrace_record::find_snapshot()`` is introduced for making the decision
of whether the AUX ring buffer has been wrapped around or not, at the
end it fixes up the AUX buffer's head which are used to calculate the
trace data size.
(在快照模式下，perf 只访问头指针 ``perf_event_mmap_page::aux_head`` 而不碰尾指针 ``aux_tail`` ，这是因为 AUX 环形缓冲在自由运行模式下可能溢出，此时尾指针没有用处。另外，引入了回调 ``auxtrace_record::find_snapshot()`` 来判定 AUX 环形缓冲是否已回绕，最后它会修正 AUX 缓冲区的头指针，该头指针用于计算追踪数据的大小。)

As we know, the buffers' deployment can be per-thread mode, per-CPU
mode, or system wide mode, and the snapshot can be applied to any of
these modes.  Below is an example of taking snapshot with system wide
mode.
(如前所述，缓冲区的部署可以是每线程模式、每 CPU 模式或全系统模式，快照可以应用于其中任何一种模式。下面是以全系统模式采集快照的示例。)

::

                                         Snapshot is taken
                                                 |
                                                 v
                        +------------------------+
                        |  AUX Ring buffer 0     | <- aux_head
                        +------------------------+
                                                 v
                +--------------------------------+
                |          AUX Ring buffer 1     | <- aux_head
                +--------------------------------+
                                                 v
    +--------------------------------------------+
    |                      AUX Ring buffer 2     | <- aux_head
    +--------------------------------------------+
                                                 v
         +---------------------------------------+
         |                 AUX Ring buffer 3     | <- aux_head
         +---------------------------------------+

                Figure 9. Snapshot with system wide mode
