===============================
BPF ring buffer(BPF 环形缓冲区)
===============================

This document describes BPF ring buffer design, API, and implementation details.
(本文档描述 BPF 环形缓冲区的设计、API 与实现细节。)

.. contents::
    :local:
    :depth: 2

Motivation(动机)
----------------

There are two distinctive motivators for this work, which are not satisfied by
existing perf buffer, which prompted creation of a new ring buffer
implementation.
(促成这一新环形缓冲区实现的动机有两个，它们是现有 perf 缓冲区所无法满足的：)

- more efficient memory utilization by sharing ring buffer across CPUs;
  (通过在多个 CPU 之间共享环形缓冲区来更高效地利用内存；)
- preserving ordering of events that happen sequentially in time, even across
  multiple CPUs (e.g., fork/exec/exit events for a task).
  (保持按时间顺序发生的事件的先后次序，即使跨越多个 CPU 也是如此(例如某个任务的 fork/exec/exit 事件)。)

These two problems are independent, but perf buffer fails to satisfy both.
Both are a result of a choice to have per-CPU perf ring buffer.  Both can be
also solved by having an MPSC implementation of ring buffer. The ordering
problem could technically be solved for perf buffer with some in-kernel
counting, but given the first one requires an MPSC buffer, the same solution
would solve the second problem automatically.
(这两个问题相互独立，但 perf 缓冲区无法同时满足两者。两者都是选择每 CPU perf 环形缓冲区所导致的结果。采用 MPSC(多生产者单消费者)实现的环形缓冲区可以同时解决这两个问题。从技术上讲，顺序问题也可以通过在内核内做一些计数来为 perf 缓冲区解决，但由于第一个问题本身就需要 MPSC 缓冲区，同样的方案会自动解决第二个问题。)

Semantics and APIs(语义与 API)
------------------------------

Single ring buffer is presented to BPF programs as an instance of BPF map of
type ``BPF_MAP_TYPE_RINGBUF``. Two other alternatives considered, but
ultimately rejected.
(单个环形缓冲区以 ``BPF_MAP_TYPE_RINGBUF`` 类型 BPF map 实例的形式呈现给 BPF 程序。还考虑过另外两种替代方案，但最终都被否决。)

One way would be to, similar to ``BPF_MAP_TYPE_PERF_EVENT_ARRAY``, make
``BPF_MAP_TYPE_RINGBUF`` could represent an array of ring buffers, but not
enforce "same CPU only" rule. This would be more familiar interface compatible
with existing perf buffer use in BPF, but would fail if application needed more
advanced logic to lookup ring buffer by arbitrary key.
``BPF_MAP_TYPE_HASH_OF_MAPS`` addresses this with current approach.
Additionally, given the performance of BPF ringbuf, many use cases would just
opt into a simple single ring buffer shared among all CPUs, for which current
approach would be an overkill.
(一种做法是，类似 ``BPF_MAP_TYPE_PERF_EVENT_ARRAY``，让 ``BPF_MAP_TYPE_RINGBUF`` 表示一个环形缓冲区数组，但不强制"仅限同一 CPU"的规则。这会是更让人熟悉的接口，与 BPF 中现有的 perf 缓冲区用法兼容，但如果应用程序需要更高级的逻辑、按任意键查找环形缓冲区，它就会失效。当前的方案配合 ``BPF_MAP_TYPE_HASH_OF_MAPS`` 可以解决这一问题。此外，鉴于 BPF ringbuf 的性能，许多使用场景会直接选择在所有 CPU 之间共享的简单单一环形缓冲区，对这些场景来说上述方案就过于复杂了。)

Another approach could introduce a new concept, alongside BPF map, to represent
generic "container" object, which doesn't necessarily have key/value interface
with lookup/update/delete operations. This approach would add a lot of extra
infrastructure that has to be built for observability and verifier support. It
would also add another concept that BPF developers would have to familiarize
themselves with, new syntax in libbpf, etc. But then would really provide no
additional benefits over the approach of using a map.  ``BPF_MAP_TYPE_RINGBUF``
doesn't support lookup/update/delete operations, but so doesn't few other map
types (e.g., queue and stack; array doesn't support delete, etc).
(另一种做法是在 BPF map 之外引入一个新概念来表示通用的"容器"对象，它不必具有带查找/更新/删除操作的键/值接口。这种做法需要为可观测性与校验器支持额外构建大量基础设施，还会给 BPF 开发者增加一个需要熟悉的新概念、libbpf 中的新语法等等。但相比使用 map 的方案，它实际上不会带来任何额外的好处。 ``BPF_MAP_TYPE_RINGBUF`` 不支持查找/更新/删除操作，但其他一些 map 类型同样不支持(例如 queue 和 stack；array 不支持删除，等等)。)

The approach chosen has an advantage of re-using existing BPF map
infrastructure (introspection APIs in kernel, libbpf support, etc), being
familiar concept (no need to teach users a new type of object in BPF program),
and utilizing existing tooling (bpftool). For common scenario of using a single
ring buffer for all CPUs, it's as simple and straightforward, as would be with
a dedicated "container" object. On the other hand, by being a map, it can be
combined with ``ARRAY_OF_MAPS`` and ``HASH_OF_MAPS`` map-in-maps to implement
a wide variety of topologies, from one ring buffer for each CPU (e.g., as
a replacement for perf buffer use cases), to a complicated application
hashing/sharding of ring buffers (e.g., having a small pool of ring buffers
with hashed task's tgid being a look up key to preserve order, but reduce
contention).
(所选方案的优势在于：复用现有的 BPF map 基础设施(内核中的内省 API、libbpf 支持等)、概念上让人熟悉(无需向用户教授 BPF 程序中的新对象类型)，并能利用现有工具(bpftool)。对于所有 CPU 使用单个环形缓冲区的常见场景，它和专用的"容器"对象一样简单直接。另一方面，作为一个 map，它可以与 ``ARRAY_OF_MAPS`` 和 ``HASH_OF_MAPS`` 这类 map-in-map 组合，实现各种各样的拓扑：从每个 CPU 一个环形缓冲区(例如作为 perf 缓冲区使用场景的替代)，到复杂的应用层哈希/分片环形缓冲区(例如用一个小的环形缓冲区池，以哈希后的任务 tgid 作为查找键来保持顺序，同时减少争用)。)

Key and value sizes are enforced to be zero. ``max_entries`` is used to specify
the size of ring buffer and has to be a power of 2 value.
(键与值的大小被强制为零。 ``max_entries`` 用于指定环形缓冲区的大小，且必须是 2 的幂。)

There are a bunch of similarities between perf buffer
(``BPF_MAP_TYPE_PERF_EVENT_ARRAY``) and new BPF ring buffer semantics:
(perf 缓冲区( ``BPF_MAP_TYPE_PERF_EVENT_ARRAY`` )与新的 BPF 环形缓冲区语义之间有许多相似之处：)

- variable-length records;
  (变长记录；)
- if there is no more space left in ring buffer, reservation fails, no
  blocking;
  (如果环形缓冲区没有剩余空间，预留会失败，不会阻塞；)
- memory-mappable data area for user-space applications for ease of
  consumption and high performance;
  (数据区域可被映射进用户空间应用程序，便于消费并获得高性能；)
- epoll notifications for new incoming data;
  (有新数据到来时的 epoll 通知；)
- but still the ability to do busy polling for new data to achieve the
  lowest latency, if necessary.
  (但必要时仍可以通过忙轮询获取新数据以实现最低延迟。)

BPF ringbuf provides two sets of APIs to BPF programs:
(BPF ringbuf 向 BPF 程序提供两组 API：)

- ``bpf_ringbuf_output()`` allows to *copy* data from one place to a ring
  buffer, similarly to ``bpf_perf_event_output()``;
  ( ``bpf_ringbuf_output()`` 允许把数据从一处 *复制* 到环形缓冲区，类似于 ``bpf_perf_event_output()``；)
- ``bpf_ringbuf_reserve()``/``bpf_ringbuf_commit()``/``bpf_ringbuf_discard()``
  APIs split the whole process into two steps. First, a fixed amount of space
  is reserved. If successful, a pointer to a data inside ring buffer data
  area is returned, which BPF programs can use similarly to a data inside
  array/hash maps. Once ready, this piece of memory is either committed or
  discarded. Discard is similar to commit, but makes consumer ignore the
  record.
  ( ``bpf_ringbuf_reserve()`` / ``bpf_ringbuf_commit()`` / ``bpf_ringbuf_discard()`` 这组 API 把整个过程拆成两步。首先预留固定大小的空间。如果成功，会返回一个指向环形缓冲区数据区内部数据的指针，BPF 程序可以像使用 array/hash map 中的数据一样使用它。就绪后，这块内存要么被提交(commit)，要么被丢弃(discard)。丢弃与提交类似，但会让消费者忽略该记录。)

``bpf_ringbuf_output()`` has disadvantage of incurring extra memory copy,
because record has to be prepared in some other place first. But it allows to
submit records of the length that's not known to verifier beforehand. It also
closely matches ``bpf_perf_event_output()``, so will simplify migration
significantly.
( ``bpf_ringbuf_output()`` 的缺点是会引入额外的内存复制，因为记录必须先在其他地方准备好。但它允许提交校验器事先不知道长度的记录。它也与 ``bpf_perf_event_output()`` 非常接近，因此能显著简化迁移。)

``bpf_ringbuf_reserve()`` avoids the extra copy of memory by providing a memory
pointer directly to ring buffer memory. In a lot of cases records are larger
than BPF stack space allows, so many programs have use extra per-CPU array as
a temporary heap for preparing sample. bpf_ringbuf_reserve() avoid this needs
completely. But in exchange, it only allows a known constant size of memory to
be reserved, such that verifier can verify that BPF program can't access memory
outside its reserved record space. bpf_ringbuf_output(), while slightly slower
due to extra memory copy, covers some use cases that are not suitable for
``bpf_ringbuf_reserve()``.
( ``bpf_ringbuf_reserve()`` 通过直接提供指向环形缓冲区内存的指针，避免了额外的内存复制。在很多情况下，记录的大小超过了 BPF 栈空间允许的范围，因此许多程序不得不使用额外的每 CPU 数组作为准备采样数据的临时堆。bpf_ringbuf_reserve() 完全避免了这一需要。但作为交换，它只允许预留已知常量大小的内存，使校验器能够验证 BPF 程序无法访问其预留记录空间之外的内存。bpf_ringbuf_output() 虽然因额外内存复制而稍慢，但覆盖了一些不适合 ``bpf_ringbuf_reserve()`` 的使用场景。)

The difference between commit and discard is very small. Discard just marks
a record as discarded, and such records are supposed to be ignored by consumer
code. Discard is useful for some advanced use-cases, such as ensuring
all-or-nothing multi-record submission, or emulating temporary
``malloc()``/``free()`` within single BPF program invocation.
(提交与丢弃的区别很小。丢弃只是把一条记录标记为已丢弃，消费代码应当忽略这类记录。丢弃对某些高级使用场景很有用，例如确保多条记录要么全部提交要么全不提交，或在单次 BPF 程序调用内模拟临时的 ``malloc()`` / ``free()``。)

Each reserved record is tracked by verifier through existing
reference-tracking logic, similar to socket ref-tracking. It is thus
impossible to reserve a record, but forget to submit (or discard) it.
(每条被预留的记录都由校验器通过现有的引用跟踪逻辑进行跟踪，类似于套接字引用跟踪。因此，预留一条记录后却忘记提交(或丢弃)它是不可能发生的。)

``bpf_ringbuf_query()`` helper allows to query various properties of ring
buffer.  Currently 4 are supported:
( ``bpf_ringbuf_query()`` 辅助函数允许查询环形缓冲区的各种属性。目前支持 4 种：)

- ``BPF_RB_AVAIL_DATA`` returns amount of unconsumed data in ring buffer;
  ( ``BPF_RB_AVAIL_DATA`` 返回环形缓冲区中未被消费的数据量；)
- ``BPF_RB_RING_SIZE`` returns the size of ring buffer;
  ( ``BPF_RB_RING_SIZE`` 返回环形缓冲区的大小；)
- ``BPF_RB_CONS_POS``/``BPF_RB_PROD_POS`` returns current logical position
  of consumer/producer, respectively.
  ( ``BPF_RB_CONS_POS`` / ``BPF_RB_PROD_POS`` 分别返回消费者/生产者当前的逻辑位置。)

Returned values are momentarily snapshots of ring buffer state and could be
off by the time helper returns, so this should be used only for
debugging/reporting reasons or for implementing various heuristics, that take
into account highly-changeable nature of some of those characteristics.
(返回的值只是环形缓冲区状态某一时刻的快照，等辅助函数返回时可能已经不准，因此只应用于调试/报告目的，或用于实现各种启发式策略——这些策略要考虑到其中某些特性高度易变的本质。)

One such heuristic might involve more fine-grained control over poll/epoll
notifications about new data availability in ring buffer. Together with
``BPF_RB_NO_WAKEUP``/``BPF_RB_FORCE_WAKEUP`` flags for output/commit/discard
helpers, it allows BPF program a high degree of control and, e.g., more
efficient batched notifications. Default self-balancing strategy, though,
should be adequate for most applications and will work reliable and efficiently
already.
(其中一种启发式策略可能涉及对环形缓冲区新数据可用性的 poll/epoll 通知进行更细粒度的控制。结合输出/提交/丢弃辅助函数的 ``BPF_RB_NO_WAKEUP`` / ``BPF_RB_FORCE_WAKEUP`` 标志，它让 BPF 程序拥有高度的控制能力，例如实现更高效的批量通知。不过，默认的自平衡策略对大多数应用程序来说应当已经足够，并且已经可以可靠而高效地工作。)

Design and Implementation(设计与实现)
-------------------------------------

This reserve/commit schema allows a natural way for multiple producers, either
on different CPUs or even on the same CPU/in the same BPF program, to reserve
independent records and work with them without blocking other producers. This
means that if BPF program was interrupted by another BPF program sharing the
same ring buffer, they will both get a record reserved (provided there is
enough space left) and can work with it and submit it independently. This
applies to NMI context as well, except that due to using a spinlock during
reservation, in NMI context, ``bpf_ringbuf_reserve()`` might fail to get
a lock, in which case reservation will fail even if ring buffer is not full.
(这种预留/提交模式为多个生产者提供了一条自然的途径——无论是在不同 CPU 上，还是在同一 CPU 上/同一 BPF 程序内——都可以预留独立的记录并对其进行操作，而不会阻塞其他生产者。这意味着，如果某个 BPF 程序被另一个共享同一环形缓冲区的 BPF 程序中断，两者都会各自预留到一条记录(前提是剩余空间足够)，并且可以独立地对其进行操作并提交。这一结论同样适用于 NMI 上下文，但由于预留期间要使用自旋锁，在 NMI 上下文中 ``bpf_ringbuf_reserve()`` 可能拿不到锁，此时即使环形缓冲区未满，预留也会失败。)

The ring buffer itself internally is implemented as a power-of-2 sized
circular buffer, with two logical and ever-increasing counters (which might
wrap around on 32-bit architectures, that's not a problem):
(环形缓冲区本身在内部实现为大小为 2 的幂的环形缓冲区，带有两个逻辑上持续递增的计数器(在 32 位架构上可能会回绕，这不是问题)：)

- consumer counter shows up to which logical position consumer consumed the
  data;
  (消费者计数器显示消费者已消费数据到哪个逻辑位置；)
- producer counter denotes amount of data reserved by all producers.
  (生产者计数器表示所有生产者已预留的数据量。)

Each time a record is reserved, producer that "owns" the record will
successfully advance producer counter. At that point, data is still not yet
ready to be consumed, though. Each record has 8 byte header, which contains the
length of reserved record, as well as two extra bits: busy bit to denote that
record is still being worked on, and discard bit, which might be set at commit
time if record is discarded. In the latter case, consumer is supposed to skip
the record and move on to the next one. Record header also encodes record's
relative offset from the beginning of ring buffer data area (in pages). This
allows ``bpf_ringbuf_commit()``/``bpf_ringbuf_discard()`` to accept only the
pointer to the record itself, without requiring also the pointer to ring buffer
itself. Ring buffer memory location will be restored from record metadata
header. This significantly simplifies verifier, as well as improving API
usability.
(每次预留记录时，"拥有"该记录的生产者都会成功推进生产者计数器。但在那一刻，数据仍未就绪、不可消费。每条记录有一个 8 字节的头部，其中包含被预留记录的长度，以及两个额外的位：忙位(busy bit)表示记录仍在处理中；丢弃位(discard bit)可能在提交时被置位，表示记录被丢弃。在后一种情况下，消费者应当跳过该记录并继续处理下一条。记录头部还编码了记录相对环形缓冲区数据区起始处(以页为单位)的偏移。这使得 ``bpf_ringbuf_commit()`` / ``bpf_ringbuf_discard()`` 可以只接受指向记录本身的指针，而不需要同时给出指向环形缓冲区本身的指针——环形缓冲区的内存位置可以从记录元数据头部恢复。这显著简化了校验器，也改善了 API 的易用性。)

Producer counter increments are serialized under spinlock, so there is
a strict ordering between reservations. Commits, on the other hand, are
completely lockless and independent. All records become available to consumer
in the order of reservations, but only after all previous records where
already committed. It is thus possible for slow producers to temporarily hold
off submitted records, that were reserved later.
(生产者计数器的递增在自旋锁下串行化，因此各次预留之间存在严格的顺序。而提交则是完全无锁且相互独立的。所有记录按预留顺序对消费者可见，但前提是之前所有的记录都已经提交。因此，慢的生产者有可能暂时拖住比它更晚预留、但已提交的记录。)

One interesting implementation bit, that significantly simplifies (and thus
speeds up as well) implementation of both producers and consumers is how data
area is mapped twice contiguously back-to-back in the virtual memory. This
allows to not take any special measures for samples that have to wrap around
at the end of the circular buffer data area, because the next page after the
last data page would be first data page again, and thus the sample will still
appear completely contiguous in virtual memory. See comment and a simple ASCII
diagram showing this visually in ``bpf_ringbuf_area_alloc()``.
(实现中有一个有趣的细节，它显著简化(从而也加速)了生产者与消费者双方的实现：数据区在虚拟内存中被连续地、背靠背地映射两次。这样，对于必须在环形缓冲区数据区末尾回绕的采样数据，无需采取任何特殊措施，因为最后一个数据页之后的下一页会再次是第一个数据页，采样数据在虚拟内存中看起来仍然是完全连续的。可视化展示这一点的注释和简单 ASCII 图参见 ``bpf_ringbuf_area_alloc()``。)

Another feature that distinguishes BPF ringbuf from perf ring buffer is
a self-pacing notifications of new data being availability.
``bpf_ringbuf_commit()`` implementation will send a notification of new record
being available after commit only if consumer has already caught up right up to
the record being committed. If not, consumer still has to catch up and thus
will see new data anyways without needing an extra poll notification.
Benchmarks (see tools/testing/selftests/bpf/benchs/bench_ringbufs.c) show that
this allows to achieve a very high throughput without having to resort to
tricks like "notify only every Nth sample", which are necessary with perf
buffer. For extreme cases, when BPF program wants more manual control of
notifications, commit/discard/output helpers accept ``BPF_RB_NO_WAKEUP`` and
``BPF_RB_FORCE_WAKEUP`` flags, which give full control over notifications of
data availability, but require extra caution and diligence in using this API.
(BPF ringbuf 区别于 perf 环形缓冲区的另一个特性是新数据可用性的自调节(self-pacing)通知。 ``bpf_ringbuf_commit()`` 的实现只有在消费者已经追平到正被提交的记录时，才会在提交后发送新记录可用的通知。否则，消费者仍需继续追赶，反正会看到新数据，无需额外的轮询通知。基准测试(见 tools/testing/selftests/bpf/benchs/bench_ringbufs.c)表明，这可以在不必借助"每 N 个采样才通知一次"这类 perf 缓冲区所必需的技巧的情况下，达到非常高的吞吐量。对于极端情况，当 BPF 程序想要对通知做更多手动控制时，提交/丢弃/输出辅助函数接受 ``BPF_RB_NO_WAKEUP`` 与 ``BPF_RB_FORCE_WAKEUP`` 标志，它们提供对数据可用性通知的完全控制，但使用这一 API 时需要格外小心和严谨。)
