.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

.. _napi:

====
NAPI
====

(NAPI)

NAPI is the event handling mechanism used by the Linux networking stack.
The name NAPI no longer stands for anything in particular [#]_.
(NAPI 是 Linux 网络栈所使用的事件处理机制。NAPI 这个名字如今已不再代表任何特定含义。)

In basic operation the device notifies the host about new events
via an interrupt.
(在基本工作模式下，设备通过中断通知主机有新事件。)
The host then schedules a NAPI instance to process the events.
(随后主机调度一个 NAPI 实例来处理这些事件。)
The device may also be polled for events via NAPI without receiving
interrupts first (:ref:`busy polling<poll>`).
(也可以在未先收到中断的情况下，通过 NAPI 轮询设备来获取事件 (参见 :ref:`busy polling<poll>`)。)

NAPI processing usually happens in the software interrupt context,
but there is an option to use :ref:`separate kernel threads<threaded>`
for NAPI processing.
(NAPI 处理通常发生在软中断上下文中，但也可以选择使用 :ref:`separate kernel threads<threaded>` 来进行 NAPI 处理。)

All in all NAPI abstracts away from the drivers the context and configuration
of event (packet Rx and Tx) processing.
(总而言之，NAPI 为驱动程序抽象掉了事件 (数据包收发) 处理的上下文和配置。)

Driver API(驱动程序 API)
========================

The two most important elements of NAPI are the struct napi_struct
and the associated poll method. struct napi_struct holds the state
of the NAPI instance while the method is the driver-specific event
handler. The method will typically free Tx packets that have been
transmitted and process newly received packets.
(NAPI 最重要的两个元素是 struct napi_struct 和与之关联的 poll 方法。struct napi_struct 保存 NAPI 实例的状态，而该方法则是驱动程序特定的事件处理程序。该方法通常会释放已发送的 Tx 数据包，并处理新接收的数据包。)

.. _drv_ctrl:

Control API(控制 API)
---------------------

netif_napi_add() and netif_napi_del() add/remove a NAPI instance
from the system. The instances are attached to the netdevice passed
as argument (and will be deleted automatically when netdevice is
unregistered). Instances are added in a disabled state.
(netif_napi_add() 和 netif_napi_del() 用于向系统添加/移除 NAPI 实例。这些实例会附加到作为参数传入的 netdevice 上 (并在 netdevice 注销时自动删除)。实例添加时处于禁用状态。)

napi_enable() and napi_disable() manage the disabled state.
A disabled NAPI can't be scheduled and its poll method is guaranteed
to not be invoked. napi_disable() waits for ownership of the NAPI
instance to be released.
(napi_enable() 和 napi_disable() 管理禁用状态。被禁用的 NAPI 无法被调度，并且保证其 poll 方法不会被调用。napi_disable() 会等待 NAPI 实例的所有权被释放。)

The control APIs are not idempotent. Control API calls are safe against
concurrent use of datapath APIs but an incorrect sequence of control API
calls may result in crashes, deadlocks, or race conditions. For example,
calling napi_disable() multiple times in a row will deadlock.
(控制 API 不是幂等的。控制 API 调用与数据路径 API 的并发使用是安全的，但控制 API 调用顺序不正确可能导致崩溃、死锁或竞态条件。例如，连续多次调用 napi_disable() 会导致死锁。)

Datapath API(数据路径 API)
--------------------------

napi_schedule() is the basic method of scheduling a NAPI poll.
Drivers should call this function in their interrupt handler
(see :ref:`drv_sched` for more info). A successful call to napi_schedule()
will take ownership of the NAPI instance.
(napi_schedule() 是调度 NAPI 轮询的基本方法。驱动程序应在其 interrupt handler 中调用此函数 (更多信息参见 :ref:`drv_sched`)。成功调用 napi_schedule() 将取得 NAPI 实例的所有权。)

Later, after NAPI is scheduled, the driver's poll method will be
called to process the events/packets. The method takes a ``budget``
argument - drivers can process completions for any number of Tx
packets but should only process up to ``budget`` number of
Rx packets. Rx processing is usually much more expensive.
(之后，在 NAPI 被调度后，驱动程序的 poll 方法将被调用以处理事件/数据包。该方法接受一个 ``budget`` 参数 —— 驱动程序可以处理任意数量的 Tx 数据包完成事件，但只应处理最多 ``budget`` 个 Rx 数据包。Rx 处理的开销通常要大得多。)

In other words for Rx processing the ``budget`` argument limits how many
packets driver can process in a single poll. Rx specific APIs like page
pool or XDP cannot be used at all when ``budget`` is 0.
skb Tx processing should happen regardless of the ``budget``, but if
the argument is 0 driver cannot call any XDP (or page pool) APIs.
(换言之，对于 Rx 处理，``budget`` 参数限制了驱动程序在单次轮询中可以处理的数据包数量。当 ``budget`` 为 0 时，完全不能使用页面池 (page pool) 或 XDP 等 Rx 专用 API。skb Tx 处理则无论 ``budget`` 如何都应进行，但如果该参数为 0，驱动程序不能调用任何 XDP (或页面池) API。)

.. warning::

   The ``budget`` argument may be 0 if core tries to only process
   skb Tx completions and no Rx or XDP packets.
   (如果内核尝试只处理 skb Tx 完成事件而不处理 Rx 或 XDP 数据包，``budget`` 参数可能为 0。)

The poll method returns the amount of work done. If the driver still
has outstanding work to do (e.g. ``budget`` was exhausted)
the poll method should return exactly ``budget``. In that case,
the NAPI instance will be serviced/polled again (without the
need to be scheduled).
(poll 方法返回已完成的工作量。如果驱动程序仍有未完成的工作 (例如 ``budget`` 已用尽)，poll 方法应精确返回 ``budget``。在这种情况下，NAPI 实例将被再次服务/轮询 (无需重新调度)。)

If event processing has been completed (all outstanding packets
processed) the poll method should call napi_complete_done()
before returning. napi_complete_done() releases the ownership
of the instance.
(如果事件处理已完成 (所有未决数据包都已处理)，poll 方法应在返回前调用 napi_complete_done()。napi_complete_done() 会释放实例的所有权。)

.. warning::

   The case of finishing all events and using exactly ``budget``
   must be handled carefully. There is no way to report this
   (rare) condition to the stack, so the driver must either
   not call napi_complete_done() and wait to be called again,
   or return ``budget - 1``.
   (处理完所有事件且恰好用完 ``budget`` 的情况必须小心处理。没有办法向协议栈报告这种 (罕见的) 情况，因此驱动程序必须要么不调用 napi_complete_done() 并等待再次被调用，要么返回 ``budget - 1`` 。)

   If the ``budget`` is 0 napi_complete_done() should never be called.
   (如果 ``budget`` 为 0，则绝不应调用 napi_complete_done()。)

Call sequence(调用顺序)
-----------------------

Drivers should not make assumptions about the exact sequencing
of calls. The poll method may be called without the driver scheduling
the instance (unless the instance is disabled). Similarly,
it's not guaranteed that the poll method will be called, even
if napi_schedule() succeeded (e.g. if the instance gets disabled).
(驱动程序不应对调用的确切顺序做出假设。即使驱动程序没有调度实例，poll 方法也可能被调用 (除非实例被禁用)。类似地，即使 napi_schedule() 调用成功，也不保证 poll 方法一定会被调用 (例如实例被禁用时)。)

As mentioned in the :ref:`drv_ctrl` section - napi_disable() and subsequent
calls to the poll method only wait for the ownership of the instance
to be released, not for the poll method to exit. This means that
drivers should avoid accessing any data structures after calling
napi_complete_done().
(如 :ref:`drv_ctrl` 一节所述 —— napi_disable() 以及随后对 poll 方法的调用只等待实例的所有权被释放，而不等待 poll 方法退出。这意味着驱动程序应避免在调用 napi_complete_done() 之后访问任何数据结构。)

.. _drv_sched:

Scheduling and IRQ masking(调度与 IRQ 屏蔽)
-------------------------------------------

Drivers should keep the interrupts masked after scheduling
the NAPI instance - until NAPI polling finishes any further
interrupts are unnecessary.
(驱动程序应在调度 NAPI 实例后保持中断屏蔽状态 —— 在 NAPI 轮询结束之前，任何后续中断都是不必要的。)

Drivers which have to mask the interrupts explicitly (as opposed
to IRQ being auto-masked by the device) should use the napi_schedule_prep()
and __napi_schedule() calls:
(必须显式屏蔽中断的驱动程序 (与由设备自动屏蔽 IRQ 相对) 应使用 napi_schedule_prep() 和 __napi_schedule() 调用：)

.. code-block:: c

  if (napi_schedule_prep(&v->napi)) {
      mydrv_mask_rxtx_irq(v->idx);
      /* schedule after masking to avoid races */
      __napi_schedule(&v->napi);
  }

IRQ should only be unmasked after a successful call to napi_complete_done():
(只有在成功调用 napi_complete_done() 之后才应解除 IRQ 屏蔽：)

.. code-block:: c

  if (budget && napi_complete_done(&v->napi, work_done)) {
    mydrv_unmask_rxtx_irq(v->idx);
    return min(work_done, budget - 1);
  }

napi_schedule_irqoff() is a variant of napi_schedule() which takes advantage
of guarantees given by being invoked in IRQ context (no need to
mask interrupts). napi_schedule_irqoff() will fall back to napi_schedule() if
IRQs are threaded (such as if ``PREEMPT_RT`` is enabled).
(napi_schedule_irqoff() 是 napi_schedule() 的一种变体，它利用了在中断上下文中被调用所带来的保证 (无需屏蔽中断)。如果 IRQ 是线程化的 (例如启用了 ``PREEMPT_RT``)，napi_schedule_irqoff() 会退回到 napi_schedule()。)

Instance to queue mapping(实例与队列的映射)
-------------------------------------------

Modern devices have multiple NAPI instances (struct napi_struct) per
interface. There is no strong requirement on how the instances are
mapped to queues and interrupts. NAPI is primarily a polling/processing
abstraction without specific user-facing semantics. That said, most networking
devices end up using NAPI in fairly similar ways.
(现代设备在每个接口上有多个 NAPI 实例 (struct napi_struct)。关于实例如何映射到队列和中断，没有严格的要求。NAPI 本质上是一种轮询/处理抽象，没有特定的面向用户的语义。话虽如此，大多数网络设备最终都以相当相似的方式使用 NAPI。)

NAPI instances most often correspond 1:1:1 to interrupts and queue pairs
(queue pair is a set of a single Rx and single Tx queue).
(NAPI 实例最常与中断和队列对呈 1:1:1 对应关系 (队列对是一组单个 Rx 队列和单个 Tx 队列)。)

In less common cases a NAPI instance may be used for multiple queues
or Rx and Tx queues can be serviced by separate NAPI instances on a single
core. Regardless of the queue assignment, however, there is usually still
a 1:1 mapping between NAPI instances and interrupts.
(在不太常见的情况下，一个 NAPI 实例可能用于多个队列，或者 Rx 和 Tx 队列可以由单个核心上的不同 NAPI 实例来服务。不过，无论队列如何分配，NAPI 实例与中断之间通常仍保持 1:1 映射。)

It's worth noting that the ethtool API uses a "channel" terminology where
each channel can be either ``rx``, ``tx`` or ``combined``. It's not clear
what constitutes a channel; the recommended interpretation is to understand
a channel as an IRQ/NAPI which services queues of a given type. For example,
a configuration of 1 ``rx``, 1 ``tx`` and 1 ``combined`` channel is expected
to utilize 3 interrupts, 2 Rx and 2 Tx queues.
(值得注意的是，ethtool API 使用 "channel (通道)" 这一术语，每个通道可以是 ``rx`` 、``tx`` 或 ``combined`` 。通道由什么构成并不明确；建议的解释是将通道理解为服务于给定类型队列的 IRQ/NAPI。例如，1 个 ``rx`` 、1 个 ``tx`` 和 1 个 ``combined`` 通道的配置预期会使用 3 个中断、2 个 Rx 队列和 2 个 Tx 队列。)

Persistent NAPI config(持久化 NAPI 配置)
----------------------------------------

Drivers often allocate and free NAPI instances dynamically. This leads to loss
of NAPI-related user configuration each time NAPI instances are reallocated.
The netif_napi_add_config() API prevents this loss of configuration by
associating each NAPI instance with a persistent NAPI configuration based on
a driver defined index value, like a queue number.
(驱动程序经常动态分配和释放 NAPI 实例。这会导致每次重新分配 NAPI 实例时丢失与 NAPI 相关的用户配置。netif_napi_add_config() API 通过基于驱动程序定义的索引值 (如队列编号) 将每个 NAPI 实例与持久的 NAPI 配置相关联，从而防止这种配置丢失。)

Using this API allows for persistent NAPI IDs (among other settings), which can
be beneficial to userspace programs using ``SO_INCOMING_NAPI_ID``. See the
sections below for other NAPI configuration settings.
(使用此 API 可以获得持久的 NAPI ID (以及其他设置)，这对使用 ``SO_INCOMING_NAPI_ID`` 的用户空间程序有益。其他 NAPI 配置设置参见下文各节。)

Drivers should try to use netif_napi_add_config() whenever possible.
(驱动程序应尽可能尝试使用 netif_napi_add_config()。)

User API(用户 API)
==================

User interactions with NAPI depend on NAPI instance ID. The instance IDs
are only visible to the user thru the ``SO_INCOMING_NAPI_ID`` socket option.
(用户与 NAPI 的交互依赖于 NAPI 实例 ID。实例 ID 仅通过 ``SO_INCOMING_NAPI_ID`` 套接字选项对用户可见。)

Users can query NAPI IDs for a device or device queue using netlink. This can
be done programmatically in a user application or by using a script included in
the kernel source tree: ``tools/net/ynl/pyynl/cli.py``.
(用户可以使用 netlink 查询设备或设备队列的 NAPI ID。这可以在用户应用程序中以编程方式完成，或使用内核源码树中包含的脚本 ``tools/net/ynl/pyynl/cli.py`` 来完成。)

For example, using the script to dump all of the queues for a device (which
will reveal each queue's NAPI ID):
(例如，使用该脚本转储设备的所有队列 (这将显示每个队列的 NAPI ID)：)

.. code-block:: bash

   $ kernel-source/tools/net/ynl/pyynl/cli.py \
             --spec Documentation/netlink/specs/netdev.yaml \
             --dump queue-get \
             --json='{"ifindex": 2}'

See ``Documentation/netlink/specs/netdev.yaml`` for more details on
available operations and attributes.
(有关可用操作和属性的更多细节，参见 ``Documentation/netlink/specs/netdev.yaml`` 。)

Software IRQ coalescing(软件 IRQ 合并)
--------------------------------------

NAPI does not perform any explicit event coalescing by default.
In most scenarios batching happens due to IRQ coalescing which is done
by the device. There are cases where software coalescing is helpful.
(默认情况下，NAPI 不执行任何显式的事件合并。在大多数场景下，批量处理是因设备所进行的 IRQ 合并而发生的。在某些情况下，软件合并会有所帮助。)

NAPI can be configured to arm a repoll timer instead of unmasking
the hardware interrupts as soon as all packets are processed.
The ``gro_flush_timeout`` sysfs configuration of the netdevice
is reused to control the delay of the timer, while
``napi_defer_hard_irqs`` controls the number of consecutive empty polls
before NAPI gives up and goes back to using hardware IRQs.
(NAPI 可以被配置为在所有数据包处理完毕后启动一个重新轮询定时器，而不是立即解除硬件中断屏蔽。netdevice 的 ``gro_flush_timeout`` sysfs 配置被复用来控制定时器的延迟，而 ``napi_defer_hard_irqs`` 则控制在 NAPI 放弃并改回使用硬件 IRQ 之前连续空轮询的次数。)

The above parameters can also be set on a per-NAPI basis using netlink via
netdev-genl. When used with netlink and configured on a per-NAPI basis, the
parameters mentioned above use hyphens instead of underscores:
``gro-flush-timeout`` and ``napi-defer-hard-irqs``.
(上述参数也可以通过 netlink 经由 netdev-genl 按每个 NAPI 进行设置。当与 netlink 一起使用并按 NAPI 配置时，上述参数使用连字符而非下划线：``gro-flush-timeout`` 和 ``napi-defer-hard-irqs`` 。)

Per-NAPI configuration can be done programmatically in a user application
or by using a script included in the kernel source tree:
``tools/net/ynl/pyynl/cli.py``.
(按 NAPI 进行配置可以在用户应用程序中以编程方式完成，或使用内核源码树中包含的脚本 ``tools/net/ynl/pyynl/cli.py`` 来完成。)

For example, using the script:
(例如，使用该脚本：)

.. code-block:: bash

  $ kernel-source/tools/net/ynl/pyynl/cli.py \
            --spec Documentation/netlink/specs/netdev.yaml \
            --do napi-set \
            --json='{"id": 345,
                     "defer-hard-irqs": 111,
                     "gro-flush-timeout": 11111}'

Similarly, the parameter ``irq-suspend-timeout`` can be set using netlink
via netdev-genl. There is no global sysfs parameter for this value.
(类似地，参数 ``irq-suspend-timeout`` 可以通过 netlink 经由 netdev-genl 设置。此值没有全局的 sysfs 参数。)

``irq-suspend-timeout`` is used to determine how long an application can
completely suspend IRQs. It is used in combination with SO_PREFER_BUSY_POLL,
which can be set on a per-epoll context basis with ``EPIOCSPARAMS`` ioctl.
(``irq-suspend-timeout`` 用于确定应用程序可以完全挂起 IRQ 多长时间。它与 SO_PREFER_BUSY_POLL 结合使用，后者可以通过 ``EPIOCSPARAMS`` ioctl 按每个 epoll 上下文进行设置。)

.. _poll:

Busy polling(忙轮询)
--------------------

Busy polling allows a user process to check for incoming packets before
the device interrupt fires. As is the case with any busy polling it trades
off CPU cycles for lower latency (production uses of NAPI busy polling
are not well known).
(忙轮询允许用户进程在设备中断触发之前检查传入的数据包。与任何忙轮询一样，它以 CPU 周期换取更低的延迟 (NAPI 忙轮询在生产环境中的使用并不广为人知)。)

Busy polling is enabled by either setting ``SO_BUSY_POLL`` on
selected sockets or using the global ``net.core.busy_poll`` and
``net.core.busy_read`` sysctls. An io_uring API for NAPI busy polling
also exists. Threaded polling of NAPI also has a mode to busy poll for
packets (:ref:`threaded busy polling<threaded_busy_poll>`) using the NAPI
processing kthread.
(通过在选定的套接字上设置 ``SO_BUSY_POLL`` ，或使用全局的 ``net.core.busy_poll`` 和 ``net.core.busy_read`` sysctl，可以启用忙轮询。还存在用于 NAPI 忙轮询的 io_uring API。NAPI 的线程化轮询也有一种使用 NAPI 处理 kthread 对数据包进行忙轮询的模式 (:ref:`threaded busy polling<threaded_busy_poll>`)。)

epoll-based busy polling(基于 epoll 的忙轮询)
---------------------------------------------

It is possible to trigger packet processing directly from calls to
``epoll_wait``. In order to use this feature, a user application must ensure
all file descriptors which are added to an epoll context have the same NAPI ID.
(可以直接通过调用 ``epoll_wait`` 来触发数据包处理。为了使用此特性，用户应用程序必须确保添加到 epoll 上下文中的所有文件描述符都具有相同的 NAPI ID。)

If the application uses a dedicated acceptor thread, the application can obtain
the NAPI ID of the incoming connection using SO_INCOMING_NAPI_ID and then
distribute that file descriptor to a worker thread. The worker thread would add
the file descriptor to its epoll context. This would ensure each worker thread
has an epoll context with FDs that have the same NAPI ID.
(如果应用程序使用专用的接受 (acceptor) 线程，应用程序可以使用 SO_INCOMING_NAPI_ID 获取传入连接的 NAPI ID，然后将该文件描述符分发给某个工作线程。工作线程会将该文件描述符加入其 epoll 上下文。这将确保每个工作线程的 epoll 上下文中的文件描述符都具有相同的 NAPI ID。)

Alternatively, if the application uses SO_REUSEPORT, a bpf or ebpf program can
be inserted to distribute incoming connections to threads such that each thread
is only given incoming connections with the same NAPI ID. Care must be taken to
carefully handle cases where a system may have multiple NICs.
(或者，如果应用程序使用 SO_REUSEPORT，可以插入一个 bpf 或 ebpf 程序将传入连接分发到各线程，使每个线程只获得具有相同 NAPI ID 的传入连接。当系统可能有多个网卡时，必须小心处理这种情况。)

In order to enable busy polling, there are two choices:
(要启用忙轮询，有两种选择：)

1. ``/proc/sys/net/core/busy_poll`` can be set with a time in useconds to busy
   loop waiting for events. This is a system-wide setting and will cause all
   epoll-based applications to busy poll when they call epoll_wait. This may
   not be desirable as many applications may not have the need to busy poll.
   (可以将 ``/proc/sys/net/core/busy_poll`` 设置为一个以微秒为单位的时间，用于忙等待事件。这是一个系统级设置，会使所有基于 epoll 的应用程序在调用 epoll_wait 时进行忙轮询。这可能不是所期望的，因为许多应用程序可能并不需要忙轮询。)

2. Applications using recent kernels can issue an ioctl on the epoll context
   file descriptor to set (``EPIOCSPARAMS``) or get (``EPIOCGPARAMS``) ``struct
   epoll_params``:, which user programs can define as follows:
   (使用较新内核的应用程序可以在 epoll 上下文文件描述符上发起 ioctl，用 ``EPIOCSPARAMS`` 设置或用 ``EPIOCGPARAMS`` 获取 ``struct epoll_params`` ，用户程序可以按如下方式定义它：)

.. code-block:: c

  struct epoll_params {
      uint32_t busy_poll_usecs;
      uint16_t busy_poll_budget;
      uint8_t prefer_busy_poll;

      /* pad the struct to a multiple of 64bits */
      uint8_t __pad;
  };

IRQ mitigation(IRQ 缓解)
------------------------

While busy polling is supposed to be used by low latency applications,
a similar mechanism can be used for IRQ mitigation.
(虽然忙轮询本意是供低延迟应用程序使用，但类似的机制也可用于缓解 IRQ。)

Very high request-per-second applications (especially routing/forwarding
applications and especially applications using AF_XDP sockets) may not
want to be interrupted until they finish processing a request or a batch
of packets.
(每秒请求数非常高的应用程序 (尤其是路由/转发应用程序，尤其是使用 AF_XDP 套接字的应用程序) 可能不希望在完成处理一个请求或一批数据包之前被打断。)

Such applications can pledge to the kernel that they will perform a busy
polling operation periodically, and the driver should keep the device IRQs
permanently masked. This mode is enabled by using the ``SO_PREFER_BUSY_POLL``
socket option. To avoid system misbehavior the pledge is revoked
if ``gro_flush_timeout`` passes without any busy poll call. For epoll-based
busy polling applications, the ``prefer_busy_poll`` field of ``struct
epoll_params`` can be set to 1 and the ``EPIOCSPARAMS`` ioctl can be issued to
enable this mode. See the above section for more details.
(此类应用程序可以向内核承诺它们会周期性地执行忙轮询操作，而驱动程序应将设备 IRQ 永久保持屏蔽状态。此模式通过使用 ``SO_PREFER_BUSY_POLL`` 套接字选项启用。为避免系统行为异常，如果 ``gro_flush_timeout`` 超时且期间没有任何忙轮询调用，该承诺将被撤销。对于基于 epoll 的忙轮询应用程序，可以将 ``struct epoll_params`` 的 ``prefer_busy_poll`` 字段设置为 1，并发出 ``EPIOCSPARAMS`` ioctl 来启用此模式。更多细节参见上节。)

The NAPI budget for busy polling is lower than the default (which makes
sense given the low latency intention of normal busy polling). This is
not the case with IRQ mitigation, however, so the budget can be adjusted
with the ``SO_BUSY_POLL_BUDGET`` socket option. For epoll-based busy polling
applications, the ``busy_poll_budget`` field can be adjusted to the desired value
in ``struct epoll_params`` and set on a specific epoll context using the ``EPIOCSPARAMS``
ioctl. See the above section for more details.
(忙轮询的 NAPI 预算低于默认值 (考虑到普通忙轮询的低延迟意图，这是合理的)。但 IRQ 缓解的情况并非如此，因此可以使用 ``SO_BUSY_POLL_BUDGET`` 套接字选项调整预算。对于基于 epoll 的忙轮询应用程序，可以在 ``struct epoll_params`` 中将 ``busy_poll_budget`` 字段调整为所需值，并使用 ``EPIOCSPARAMS`` ioctl 在特定的 epoll 上下文上设置。更多细节参见上节。)

It is important to note that choosing a large value for ``gro_flush_timeout``
will defer IRQs to allow for better batch processing, but will induce latency
when the system is not fully loaded. Choosing a small value for
``gro_flush_timeout`` can cause interference of the user application which is
attempting to busy poll by device IRQs and softirq processing. This value
should be chosen carefully with these tradeoffs in mind. epoll-based busy
polling applications may be able to mitigate how much user processing happens
by choosing an appropriate value for ``maxevents``.
(需要注意的是，为 ``gro_flush_timeout`` 选择较大的值会推迟 IRQ，以获得更好的批量处理效果，但在系统未满载时会引入延迟。为 ``gro_flush_timeout`` 选择较小的值可能导致试图忙轮询的用户应用受到设备 IRQ 和软中断处理的干扰。选择此值时应结合这些权衡仔细考虑。基于 epoll 的忙轮询应用程序可以通过为 ``maxevents`` 选择合适的值来减少用户态处理的数量。)

Users may want to consider an alternate approach, IRQ suspension, to help deal
with these tradeoffs.
(用户可能需要考虑另一种方法 —— IRQ 挂起，以帮助应对这些权衡。)

IRQ suspension(IRQ 挂起)
------------------------

IRQ suspension is a mechanism wherein device IRQs are masked while epoll
triggers NAPI packet processing.
(IRQ 挂起是一种在 epoll 触发 NAPI 数据包处理期间屏蔽设备 IRQ 的机制。)

While application calls to epoll_wait successfully retrieve events, the kernel will
defer the IRQ suspension timer. If the kernel does not retrieve any events
while busy polling (for example, because network traffic levels subsided), IRQ
suspension is disabled and the IRQ mitigation strategies described above are
engaged.
(只要应用程序对 epoll_wait 的调用成功获取到事件，内核就会推迟 IRQ 挂起定时器。如果内核在忙轮询期间没有获取到任何事件 (例如由于网络流量水平下降)，IRQ 挂起将被禁用，并转而启用上述 IRQ 缓解策略。)

This allows users to balance CPU consumption with network processing
efficiency.
(这让用户可以在 CPU 消耗与网络处理效率之间取得平衡。)

To use this mechanism:
(要使用此机制：)

  1. The per-NAPI config parameter ``irq-suspend-timeout`` should be set to the
     maximum time (in nanoseconds) the application can have its IRQs
     suspended. This is done using netlink, as described above. This timeout
     serves as a safety mechanism to restart IRQ driver interrupt processing if
     the application has stalled. This value should be chosen so that it covers
     the amount of time the user application needs to process data from its
     call to epoll_wait, noting that applications can control how much data
     they retrieve by setting ``max_events`` when calling epoll_wait.
     (每个 NAPI 的配置参数 ``irq-suspend-timeout`` 应设置为应用程序可以挂起其 IRQ 的最大时间 (以纳秒为单位)。如上文所述，这通过 netlink 完成。该超时作为一种安全机制，在应用程序停滞时重启 IRQ 驱动程序的中断处理。此值的选取应覆盖用户应用处理其 epoll_wait 调用所返回数据所需的时间；注意，应用程序可以通过在调用 epoll_wait 时设置 ``max_events`` 来控制其获取的数据量。)

  2. The sysfs parameter or per-NAPI config parameters ``gro_flush_timeout``
     and ``napi_defer_hard_irqs`` can be set to low values. They will be used
     to defer IRQs after busy poll has found no data.
     (sysfs 参数或每 NAPI 配置参数 ``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` 可以设置为较小的值。它们将用于在忙轮询未发现数据后推迟 IRQ。)

  3. The ``prefer_busy_poll`` flag must be set to true. This can be done using
     the ``EPIOCSPARAMS`` ioctl as described above.
     (必须将 ``prefer_busy_poll`` 标志设置为 true。可以使用 ``EPIOCSPARAMS`` ioctl 按上述方法完成。)

  4. The application uses epoll as described above to trigger NAPI packet
     processing.
     (应用程序按上述方式使用 epoll 来触发 NAPI 数据包处理。)

As mentioned above, as long as subsequent calls to epoll_wait return events to
userland, the ``irq-suspend-timeout`` is deferred and IRQs are disabled. This
allows the application to process data without interference.
(如上所述，只要后续对 epoll_wait 的调用向用户态返回事件，``irq-suspend-timeout`` 就会被推迟，IRQ 保持禁用。这使应用程序能够不受干扰地处理数据。)

Once a call to epoll_wait results in no events being found, IRQ suspension is
automatically disabled and the ``gro_flush_timeout`` and
``napi_defer_hard_irqs`` mitigation mechanisms take over.
(一旦某次 epoll_wait 调用没有找到任何事件，IRQ 挂起将自动禁用，``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` 缓解机制将接管。)

It is expected that ``irq-suspend-timeout`` will be set to a value much larger
than ``gro_flush_timeout`` as ``irq-suspend-timeout`` should suspend IRQs for
the duration of one userland processing cycle.
(预期 ``irq-suspend-timeout`` 会设置为比 ``gro_flush_timeout`` 大得多的值，因为 ``irq-suspend-timeout`` 应在一个用户态处理周期的时长内挂起 IRQ。)

While it is not strictly necessary to use ``napi_defer_hard_irqs`` and
``gro_flush_timeout`` to use IRQ suspension, their use is strongly
recommended.
(虽然使用 IRQ 挂起并不严格要求使用 ``napi_defer_hard_irqs`` 和 ``gro_flush_timeout`` ，但强烈建议使用它们。)

IRQ suspension causes the system to alternate between polling mode and
irq-driven packet delivery. During busy periods, ``irq-suspend-timeout``
overrides ``gro_flush_timeout`` and keeps the system busy polling, but when
epoll finds no events, the setting of ``gro_flush_timeout`` and
``napi_defer_hard_irqs`` determine the next step.
(IRQ 挂起使系统在轮询模式与 IRQ 驱动的数据包交付之间交替切换。繁忙期间，``irq-suspend-timeout`` 会覆盖 ``gro_flush_timeout`` 并使系统保持忙轮询；而当 epoll 未发现事件时，``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` 的设置将决定下一步。)

There are essentially three possible loops for network processing and
packet delivery:

1) hardirq -> softirq -> napi poll; basic interrupt delivery
2) timer -> softirq -> napi poll; deferred irq processing
3) epoll -> busy-poll -> napi poll; busy looping

(网络处理和数据包交付基本上有三种可能的循环：
1) hardirq -> softirq -> napi poll；基本中断交付
2) timer -> softirq -> napi poll；延迟的 IRQ 处理
3) epoll -> busy-poll -> napi poll；忙循环)

Loop 2 can take control from Loop 1, if ``gro_flush_timeout`` and
``napi_defer_hard_irqs`` are set.
(如果设置了 ``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` ，循环 2 可以从循环 1 手中接管控制。)

If ``gro_flush_timeout`` and ``napi_defer_hard_irqs`` are set, Loops 2
and 3 "wrestle" with each other for control.
(如果设置了 ``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` ，循环 2 和循环 3 会相互"角力"争夺控制权。)

During busy periods, ``irq-suspend-timeout`` is used as timer in Loop 2,
which essentially tilts network processing in favour of Loop 3.
(繁忙期间，``irq-suspend-timeout`` 被用作循环 2 的定时器，这实际上使网络处理向循环 3 倾斜。)

If ``gro_flush_timeout`` and ``napi_defer_hard_irqs`` are not set, Loop 3
cannot take control from Loop 1.
(如果未设置 ``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` ，循环 3 无法从循环 1 手中接管控制。)

Therefore, setting ``gro_flush_timeout`` and ``napi_defer_hard_irqs`` is
the recommended usage, because otherwise setting ``irq-suspend-timeout``
might not have any discernible effect.
(因此，推荐的做法是设置 ``gro_flush_timeout`` 和 ``napi_defer_hard_irqs`` ，否则设置 ``irq-suspend-timeout`` 可能不会有任何明显效果。)

.. _threaded_busy_poll:

Threaded NAPI busy polling(线程化 NAPI 忙轮询)
----------------------------------------------

Threaded NAPI busy polling extends threaded NAPI and adds support to do
continuous busy polling of the NAPI. This can be useful for forwarding or
AF_XDP applications.
(线程化 NAPI 忙轮询扩展了线程化 NAPI，增加了对 NAPI 进行持续忙轮询的支持。这对转发或 AF_XDP 应用程序可能有用。)

Threaded NAPI busy polling can be enabled on per NIC queue basis using Netlink.
(可以使用 Netlink 按 NIC 队列启用线程化 NAPI 忙轮询。)

For example, using the following script:
(例如，使用以下脚本：)

.. code-block:: bash

  $ ynl --family netdev --do napi-set \
            --json='{"id": 66, "threaded": "busy-poll"}'

The kernel will create a kthread that busy polls on this NAPI.
(内核将创建一个对该 NAPI 进行忙轮询的 kthread。)

The user may elect to set the CPU affinity of this kthread to an unused CPU
core to improve how often the NAPI is polled at the expense of wasted CPU
cycles. Note that this will keep the CPU core busy with 100% usage.
(用户可以选择将此 kthread 的 CPU 亲和性设置为一个未使用的 CPU 核心，以提高 NAPI 被轮询的频率，代价是浪费 CPU 周期。注意，这将使该 CPU 核心保持 100% 的占用率。)

Once threaded busy polling is enabled for a NAPI, PID of the kthread can be
retrieved using Netlink so the affinity of the kthread can be set up.
(一旦为某个 NAPI 启用了线程化忙轮询，就可以使用 Netlink 获取该 kthread 的 PID，从而设置该 kthread 的亲和性。)

For example, the following script can be used to fetch the PID:
(例如，可以使用以下脚本获取 PID：)

.. code-block:: bash

  $ ynl --family netdev --do napi-get --json='{"id": 66}'

This will output something like following, the pid `258` is the PID of the
kthread that is polling this NAPI.
(这将输出类似如下的内容，其中 pid `258` 是轮询此 NAPI 的 kthread 的 PID。)

.. code-block:: bash

  $ {'defer-hard-irqs': 0,
     'gro-flush-timeout': 0,
     'id': 66,
     'ifindex': 2,
     'irq-suspend-timeout': 0,
     'pid': 258,
     'threaded': 'busy-poll'}

.. _threaded:

Threaded NAPI(线程化 NAPI)
--------------------------

Threaded NAPI is an operating mode that uses dedicated kernel
threads rather than software IRQ context for NAPI processing.
Each threaded NAPI instance will spawn a separate thread
(called ``napi/${ifc-name}-${napi-id}``).
(线程化 NAPI 是一种使用专用内核线程而非软中断上下文进行 NAPI 处理的工作模式。每个线程化 NAPI 实例会派生一个单独的线程 (称为 ``napi/${ifc-name}-${napi-id}`` )。)

It is recommended to pin each kernel thread to a single CPU, the same
CPU as the CPU which services the interrupt. Note that the mapping
between IRQs and NAPI instances may not be trivial (and is driver
dependent). The NAPI instance IDs will be assigned in the opposite
order than the process IDs of the kernel threads.
(建议将每个内核线程绑定到单个 CPU，即与服务该中断的 CPU 相同的 CPU。注意，IRQ 与 NAPI 实例之间的映射可能并不简单 (并且取决于驱动程序)。NAPI 实例 ID 的分配顺序将与内核线程的进程 ID 顺序相反。)

Threaded NAPI is controlled by writing 0/1 to the ``threaded`` file in
netdev's sysfs directory. It can also be enabled for a specific NAPI using
netlink interface.
(线程化 NAPI 通过向 netdev 的 sysfs 目录中的 ``threaded`` 文件写入 0/1 来控制。也可以使用 netlink 接口为特定的 NAPI 启用它。)

For example, using the script:
(例如，使用该脚本：)

.. code-block:: bash

  $ ynl --family netdev --do napi-set --json='{"id": 66, "threaded": 1}'

.. rubric:: Footnotes

.. [#] NAPI was originally referred to as New API in 2.4 Linux.
       (NAPI 最初在 2.4 Linux 中被称为 New API。)
