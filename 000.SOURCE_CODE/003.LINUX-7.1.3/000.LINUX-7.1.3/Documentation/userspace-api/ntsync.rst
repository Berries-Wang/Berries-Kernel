===================================================
NT synchronization primitive driver(NT 同步原语驱动)
===================================================

This page documents the user-space API for the ntsync driver.
(本页记录 ntsync 驱动的用户空间 API。)

ntsync is a support driver for emulation of NT synchronization
primitives by user-space NT emulators. It exists because implementation
in user-space, using existing tools, cannot match Windows performance
while offering accurate semantics. It is implemented entirely in
software, and does not drive any hardware device.
(ntsync 是供用户空间 NT 模拟器仿真 NT 同步原语的辅助驱动。它之所以存在，是因为仅用用户空间、借助现有工具的实现，在提供准确语义的同时无法达到 Windows 的性能。它完全以软件实现，不驱动任何硬件设备。)

This interface is meant as a compatibility tool only, and should not
be used for general synchronization. Instead use generic, versatile
interfaces such as futex(2) and poll(2).
(这一接口仅用作兼容性工具，不应用于一般性的同步。一般同步请改用 futex(2) 与 poll(2) 之类通用、多用途的接口。)

Synchronization primitives(同步原语)
====================================

The ntsync driver exposes three types of synchronization primitives:
semaphores, mutexes, and events.
(ntsync 驱动暴露三种类型的同步原语：信号量 (semaphore)、互斥体 (mutex) 与事件 (event)。)

A semaphore holds a single volatile 32-bit counter, and a static 32-bit
integer denoting the maximum value. It is considered signaled (that is,
can be acquired without contention, or will wake up a waiting thread)
when the counter is nonzero. The counter is decremented by one when a
wait is satisfied. Both the initial and maximum count are established
when the semaphore is created.
(信号量持有一个 volatile 的 32 位计数器，以及一个表示最大值的静态 32 位整数。当计数器非零时，它被视为已发信号 (signaled)——即可以无争用地获取，或会唤醒一个正在等待的线程。等待被满足时计数器减一。初始计数与最大计数都在创建信号量时确定。)

A mutex holds a volatile 32-bit recursion count, and a volatile 32-bit
identifier denoting its owner. A mutex is considered signaled when its
owner is zero (indicating that it is not owned). The recursion count is
incremented when a wait is satisfied, and ownership is set to the given
identifier.
(互斥体持有一个 volatile 的 32 位递归计数，以及一个表示其所有者的 volatile 32 位标识符。当所有者为零(表明未被持有)时，互斥体被视为已发信号。等待被满足时递归计数加一，所有者被设为给定的标识符。)

A mutex also holds an internal flag denoting whether its previous owner
has died; such a mutex is said to be abandoned. Owner death is not
tracked automatically based on thread death, but rather must be
communicated using ``NTSYNC_IOC_MUTEX_KILL``. An abandoned mutex is
inherently considered unowned.
(互斥体还持有一个内部标志，表明其前所有者是否已死亡；这样的互斥体被称为"被弃置" (abandoned)。所有者的死亡不会基于线程死亡被自动跟踪，而必须通过 ``NTSYNC_IOC_MUTEX_KILL`` 来告知。被弃置的互斥体天然被视为无主。)

Except for the "unowned" semantics of zero, the actual value of the
owner identifier is not interpreted by the ntsync driver at all. The
intended use is to store a thread identifier; however, the ntsync
driver does not actually validate that a calling thread provides
consistent or unique identifiers.
(除了零的"无主"语义之外，所有者标识符的实际值不会被 ntsync 驱动以任何方式解读。其预期用途是存储线程标识符；不过，ntsync 驱动并不会真正校验调用线程是否提供了一致或唯一的标识符。)

An event is similar to a semaphore with a maximum count of one. It holds
a volatile boolean state denoting whether it is signaled or not. There
are two types of events, auto-reset and manual-reset. An auto-reset
event is designaled when a wait is satisfied; a manual-reset event is
not. The event type is specified when the event is created.
(事件类似于最大计数为 1 的信号量。它持有一个 volatile 的布尔状态，表示自己是否已发信号。事件有两种类型：自动重置 (auto-reset) 与手动重置 (manual-reset)。自动重置事件在等待被满足时会被撤除信号 (designaled)；手动重置事件则不会。事件类型在创建事件时指定。)

Unless specified otherwise, all operations on an object are atomic and
totally ordered with respect to other operations on the same object.
(除非另有说明，对一个对象的所有操作都是原子的，并且相对于同一对象上的其他操作是完全有序的。)

Objects are represented by files. When all file descriptors to an
object are closed, that object is deleted.
(对象以文件表示。当指向某个对象的所有文件描述符都关闭时，该对象即被删除。)

Char device(字符设备)
=====================

The ntsync driver creates a single char device /dev/ntsync. Each file
description opened on the device represents a unique instance intended
to back an individual NT virtual machine. Objects created by one ntsync
instance may only be used with other objects created by the same
instance.
(ntsync 驱动创建单个字符设备 /dev/ntsync。在该设备上打开的每个文件描述代表一个唯一实例，用于支撑单独的一台 NT 虚拟机。由一个 ntsync 实例创建的对象，只能与同一实例创建的其他对象一起使用。)

ioctl reference(ioctl 参考)
===========================

All operations on the device are done through ioctls. There are four
structures used in ioctl calls::

   struct ntsync_sem_args {
   	__u32 count;
   	__u32 max;
   };

   struct ntsync_mutex_args {
   	__u32 owner;
   	__u32 count;
   };

   struct ntsync_event_args {
   	__u32 signaled;
   	__u32 manual;
   };

   struct ntsync_wait_args {
   	__u64 timeout;
   	__u64 objs;
   	__u32 count;
   	__u32 owner;
   	__u32 index;
   	__u32 alert;
   	__u32 flags;
   	__u32 pad;
   };

Depending on the ioctl, members of the structure may be used as input,
output, or not at all.
(取决于具体的 ioctl，结构的成员可能用作输入、输出，或完全不使用。)

The ioctls on the device file are as follows:
(设备文件上的 ioctl 如下：)

.. c:macro:: NTSYNC_IOC_CREATE_SEM

  Create a semaphore object. Takes a pointer to struct
  :c:type:`ntsync_sem_args`, which is used as follows:
  (创建一个信号量对象。接受一个指向 struct :c:type:`ntsync_sem_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``count``
       - Initial count of the semaphore.(信号量的初始计数。)
     * - ``max``
       - Maximum count of the semaphore.(信号量的最大计数。)

  Fails with ``EINVAL`` if ``count`` is greater than ``max``.
  On success, returns a file descriptor the created semaphore.
  (如果 ``count`` 大于 ``max``，则以 ``EINVAL`` 失败。成功时返回所创建信号量的文件描述符。)

.. c:macro:: NTSYNC_IOC_CREATE_MUTEX

  Create a mutex object. Takes a pointer to struct
  :c:type:`ntsync_mutex_args`, which is used as follows:
  (创建一个互斥体对象。接受一个指向 struct :c:type:`ntsync_mutex_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``count``
       - Initial recursion count of the mutex.(互斥体的初始递归计数。)
     * - ``owner``
       - Initial owner of the mutex.(互斥体的初始所有者。)

  If ``owner`` is nonzero and ``count`` is zero, or if ``owner`` is
  zero and ``count`` is nonzero, the function fails with ``EINVAL``.
  On success, returns a file descriptor the created mutex.
  (如果 ``owner`` 非 0 且 ``count`` 为 0，或 ``owner`` 为 0 且 ``count`` 非 0，函数以 ``EINVAL`` 失败。成功时返回所创建互斥体的文件描述符。)

.. c:macro:: NTSYNC_IOC_CREATE_EVENT

  Create an event object. Takes a pointer to struct
  :c:type:`ntsync_event_args`, which is used as follows:
  (创建一个事件对象。接受一个指向 struct :c:type:`ntsync_event_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``signaled``
       - If nonzero, the event is initially signaled, otherwise
         nonsignaled.(若非 0，事件初始为已发信号状态，否则为未发信号状态。)
     * - ``manual``
       - If nonzero, the event is a manual-reset event, otherwise
         auto-reset.(若非 0，事件为手动重置事件，否则为自动重置。)

  On success, returns a file descriptor the created event.
  (成功时返回所创建事件的文件描述符。)

The ioctls on the individual objects are as follows:
(各个对象上的 ioctl 如下：)

.. c:macro:: NTSYNC_IOC_SEM_POST

  Post to a semaphore object. Takes a pointer to a 32-bit integer,
  which on input holds the count to be added to the semaphore, and on
  output contains its previous count.
  (向信号量对象执行 post 操作。接受一个指向 32 位整数的指针，输入时它持有要加到信号量上的计数，输出时包含信号量原先的计数。)

  If adding to the semaphore's current count would raise the latter
  past the semaphore's maximum count, the ioctl fails with
  ``EOVERFLOW`` and the semaphore is not affected. If raising the
  semaphore's count causes it to become signaled, eligible threads
  waiting on this semaphore will be woken and the semaphore's count
  decremented appropriately.
  (如果加到信号量当前计数上的值会使其超过信号量的最大计数，ioctl 以 ``EOVERFLOW`` 失败，信号量不受影响。如果提升信号量计数使其变为已发信号，则在该信号量上等待的合格线程将被唤醒，信号量计数会被相应递减。)

.. c:macro:: NTSYNC_IOC_MUTEX_UNLOCK

  Release a mutex object. Takes a pointer to struct
  :c:type:`ntsync_mutex_args`, which is used as follows:
  (释放一个互斥体对象。接受一个指向 struct :c:type:`ntsync_mutex_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``owner``
       - Specifies the owner trying to release this mutex.(指定试图释放该互斥体的所有者。)
     * - ``count``
       - On output, contains the previous recursion count.(输出时包含原先的递归计数。)

  If ``owner`` is zero, the ioctl fails with ``EINVAL``. If ``owner``
  is not the current owner of the mutex, the ioctl fails with
  ``EPERM``.
  (如果 ``owner`` 为零，ioctl 以 ``EINVAL`` 失败。如果 ``owner`` 不是该互斥体的当前所有者，ioctl 以 ``EPERM`` 失败。)

  The mutex's count will be decremented by one. If decrementing the
  mutex's count causes it to become zero, the mutex is marked as
  unowned and signaled, and eligible threads waiting on it will be
  woken as appropriate.
  (互斥体的计数将减一。如果递减使计数变为零，互斥体被标记为无主且已发信号，在其上等待的合格线程将被酌情唤醒。)

.. c:macro:: NTSYNC_IOC_SET_EVENT

  Signal an event object. Takes a pointer to a 32-bit integer, which on
  output contains the previous state of the event.
  (对事件对象发信号。接受一个指向 32 位整数的指针，输出时包含事件原先的状态。)

  Eligible threads will be woken, and auto-reset events will be
  designaled appropriately.
  (合格线程将被唤醒，自动重置事件将被酌情撤除信号。)

.. c:macro:: NTSYNC_IOC_RESET_EVENT

  Designal an event object. Takes a pointer to a 32-bit integer, which
  on output contains the previous state of the event.
  (撤除事件对象的信号。接受一个指向 32 位整数的指针，输出时包含事件原先的状态。)

.. c:macro:: NTSYNC_IOC_PULSE_EVENT

  Wake threads waiting on an event object while leaving it in an
  unsignaled state. Takes a pointer to a 32-bit integer, which on
  output contains the previous state of the event.
  (唤醒在事件对象上等待的线程，同时让事件保持未发信号状态。接受一个指向 32 位整数的指针，输出时包含事件原先的状态。)

  A pulse operation can be thought of as a set followed by a reset,
  performed as a single atomic operation. If two threads are waiting on
  an auto-reset event which is pulsed, only one will be woken. If two
  threads are waiting a manual-reset event which is pulsed, both will
  be woken. However, in both cases, the event will be unsignaled
  afterwards, and a simultaneous read operation will always report the
  event as unsignaled.
  (pulse 操作可以理解为一次 set 加一次 reset，作为单个原子操作完成。如果有两个线程在等待一个被 pulse 的自动重置事件，只有一个会被唤醒。如果有两个线程在等待一个被 pulse 的手动重置事件，两者都会被唤醒。不过在这两种情况下，事件之后都处于未发信号状态，同时进行的读操作总会报告该事件未发信号。)

.. c:macro:: NTSYNC_IOC_READ_SEM

  Read the current state of a semaphore object. Takes a pointer to
  struct :c:type:`ntsync_sem_args`, which is used as follows:
  (读取信号量对象的当前状态。接受一个指向 struct :c:type:`ntsync_sem_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``count``
       - On output, contains the current count of the semaphore.(输出时包含信号量的当前计数。)
     * - ``max``
       - On output, contains the maximum count of the semaphore.(输出时包含信号量的最大计数。)

.. c:macro:: NTSYNC_IOC_READ_MUTEX

  Read the current state of a mutex object. Takes a pointer to struct
  :c:type:`ntsync_mutex_args`, which is used as follows:
  (读取互斥体对象的当前状态。接受一个指向 struct :c:type:`ntsync_mutex_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``owner``
       - On output, contains the current owner of the mutex, or zero
         if the mutex is not currently owned.(输出时包含互斥体的当前所有者；若互斥体当前无主则为零。)
     * - ``count``
       - On output, contains the current recursion count of the mutex.(输出时包含互斥体的当前递归计数。)

  If the mutex is marked as abandoned, the function fails with
  ``EOWNERDEAD``. In this case, ``count`` and ``owner`` are set to
  zero.
  (如果互斥体被标记为已弃置，函数以 ``EOWNERDEAD`` 失败。此时 ``count`` 与 ``owner`` 被置为零。)

.. c:macro:: NTSYNC_IOC_READ_EVENT

  Read the current state of an event object. Takes a pointer to struct
  :c:type:`ntsync_event_args`, which is used as follows:
  (读取事件对象的当前状态。接受一个指向 struct :c:type:`ntsync_event_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``signaled``
       - On output, contains the current state of the event.(输出时包含事件的当前状态。)
     * - ``manual``
       - On output, contains 1 if the event is a manual-reset event,
         and 0 otherwise.(输出时若事件为手动重置事件则包含 1，否则为 0。)

.. c:macro:: NTSYNC_IOC_KILL_OWNER

  Mark a mutex as unowned and abandoned if it is owned by the given
  owner. Takes an input-only pointer to a 32-bit integer denoting the
  owner. If the owner is zero, the ioctl fails with ``EINVAL``. If the
  owner does not own the mutex, the function fails with ``EPERM``.
  (如果互斥体由给定所有者持有，则把它标记为无主且已弃置。接受一个仅用于输入的、指向 32 位整数的指针，该整数表示所有者。如果所有者为零，ioctl 以 ``EINVAL`` 失败。如果该所有者并不持有该互斥体，函数以 ``EPERM`` 失败。)

  Eligible threads waiting on the mutex will be woken as appropriate
  (and such waits will fail with ``EOWNERDEAD``, as described below).
  (在互斥体上等待的合格线程将被酌情唤醒(这些等待将以 ``EOWNERDEAD`` 失败，见下文)。)

.. c:macro:: NTSYNC_IOC_WAIT_ANY

  Poll on any of a list of objects, atomically acquiring at most one.
  Takes a pointer to struct :c:type:`ntsync_wait_args`, which is
  used as follows:
  (在对象列表上等待其中任意一个，至多原子地获取一个。接受一个指向 struct :c:type:`ntsync_wait_args` 的指针，该结构按如下方式使用：)

  .. list-table::

     * - ``timeout``
       - Absolute timeout in nanoseconds. If ``NTSYNC_WAIT_REALTIME``
         is set, the timeout is measured against the REALTIME clock;
         otherwise it is measured against the MONOTONIC clock. If the
         timeout is equal to or earlier than the current time, the
         function returns immediately without sleeping. If ``timeout``
         is U64_MAX, the function will sleep until an object is
         signaled, and will not fail with ``ETIMEDOUT``.
         (以纳秒为单位的绝对超时。如果设置了 ``NTSYNC_WAIT_REALTIME``，超时相对 REALTIME 时钟计量；否则相对 MONOTONIC 时钟计量。如果超时时间等于或早于当前时间，函数立即返回而不休眠。如果 ``timeout`` 为 U64_MAX，函数将休眠直到某个对象被发信号，并且不会以 ``ETIMEDOUT`` 失败。)
     * - ``objs``
       - Pointer to an array of ``count`` file descriptors
         (specified as an integer so that the structure has the same
         size regardless of architecture). If any object is
         invalid, the function fails with ``EINVAL``.
         (指向 ``count`` 个文件描述符数组的指针(以整数形式指定，使该结构在任何体系结构上大小相同)。如果任何对象无效，函数以 ``EINVAL`` 失败。)
     * - ``count``
       - Number of objects specified in the ``objs`` array.
         If greater than ``NTSYNC_MAX_WAIT_COUNT``, the function fails
         with ``EINVAL``.
         (``objs`` 数组中指定的对象数量。若大于 ``NTSYNC_MAX_WAIT_COUNT``，函数以 ``EINVAL`` 失败。)
     * - ``owner``
       - Mutex owner identifier. If any object in ``objs`` is a mutex,
         the ioctl will attempt to acquire that mutex on behalf of
         ``owner``. If ``owner`` is zero, the ioctl fails with
         ``EINVAL``.
         (互斥体所有者标识符。如果 ``objs`` 中有任何对象是互斥体，ioctl 会代表 ``owner`` 尝试获取该互斥体。如果 ``owner`` 为零，ioctl 以 ``EINVAL`` 失败。)
     * - ``index``
       - On success, contains the index (into ``objs``) of the object
         which was signaled. If ``alert`` was signaled instead,
         this contains ``count``.
         (成功时，包含被发信号对象在 ``objs`` 中的索引。如果是 ``alert`` 被发信号，则包含 ``count``。)
     * - ``alert``
       - Optional event object file descriptor. If nonzero, this
         specifies an "alert" event object which, if signaled, will
         terminate the wait. If nonzero, the identifier must point to a
         valid event.
         (可选的事件对象文件描述符。若非零，指定一个"警报" (alert) 事件对象，它一旦被发信号就会终止等待。若非零，该标识符必须指向一个有效事件。)
     * - ``flags``
       - Zero or more flags. Currently the only flag is
         ``NTSYNC_WAIT_REALTIME``, which causes the timeout to be
         measured against the REALTIME clock instead of MONOTONIC.
         (零个或多个标志。目前唯一的标志是 ``NTSYNC_WAIT_REALTIME``，它使超时相对 REALTIME 时钟而非 MONOTONIC 计量。)
     * - ``pad``
       - Unused, must be set to zero.(未使用，必须置零。)

  This function attempts to acquire one of the given objects. If unable
  to do so, it sleeps until an object becomes signaled, subsequently
  acquiring it, or the timeout expires. In the latter case the ioctl
  fails with ``ETIMEDOUT``. The function only acquires one object, even
  if multiple objects are signaled.
  (该函数尝试获取给定对象之一。如果无法做到，它会休眠，直到某个对象变为已发信号并随后被它获取，或超时到期。后一种情况下 ioctl 以 ``ETIMEDOUT`` 失败。即使多个对象都已发信号，该函数也只获取一个对象。)

  A semaphore is considered to be signaled if its count is nonzero, and
  is acquired by decrementing its count by one. A mutex is considered
  to be signaled if it is unowned or if its owner matches the ``owner``
  argument, and is acquired by incrementing its recursion count by one
  and setting its owner to the ``owner`` argument. An auto-reset event
  is acquired by designaling it; a manual-reset event is not affected
  by acquisition.
  (如果信号量的计数非零，则它被视为已发信号，获取它的方式是把计数减一。如果互斥体无主，或其所有者与 ``owner`` 参数匹配，则它被视为已发信号，获取它的方式是把递归计数加一并把所有者设为 ``owner`` 参数。自动重置事件通过撤除信号来获取；手动重置事件不受获取影响。)

  Acquisition is atomic and totally ordered with respect to other
  operations on the same object. If two wait operations (with different
  ``owner`` identifiers) are queued on the same mutex, only one is
  signaled. If two wait operations are queued on the same semaphore,
  and a value of one is posted to it, only one is signaled.
  (获取操作是原子的，并且相对于同一对象上的其他操作完全有序。如果两个等待操作(带不同 ``owner`` 标识符)排队在同一个互斥体上，只有一个会被发信号。如果两个等待操作排队在同一个信号量上，而向它 post 了 1，也只有一个会被发信号。)

  If an abandoned mutex is acquired, the ioctl fails with
  ``EOWNERDEAD``. Although this is a failure return, the function may
  otherwise be considered successful. The mutex is marked as owned by
  the given owner (with a recursion count of 1) and as no longer
  abandoned, and ``index`` is still set to the index of the mutex.
  (如果获取到被弃置的互斥体，ioctl 以 ``EOWNERDEAD`` 失败。虽然这是一个失败返回，但除此之外该函数可以视为成功。互斥体被标记为由给定所有者持有(递归计数为 1)且不再弃置，``index`` 仍会被设为该互斥体的索引。)

  The ``alert`` argument is an "extra" event which can terminate the
  wait, independently of all other objects.
  (``alert`` 参数是一个"额外"事件，它可以独立于所有其他对象终止等待。)

  It is valid to pass the same object more than once, including by
  passing the same event in the ``objs`` array and in ``alert``. If a
  wakeup occurs due to that object being signaled, ``index`` is set to
  the lowest index corresponding to that object.
  (多次传入同一对象是合法的，包括在 ``objs`` 数组与 ``alert`` 中传入同一事件。如果因该对象被发信号而唤醒，``index`` 会被设为对应该对象的最小索引。)

  The function may fail with ``EINTR`` if a signal is received.
  (如果收到信号，函数可能以 ``EINTR`` 失败。)

.. c:macro:: NTSYNC_IOC_WAIT_ALL

  Poll on a list of objects, atomically acquiring all of them. Takes a
  pointer to struct :c:type:`ntsync_wait_args`, which is used
  identically to ``NTSYNC_IOC_WAIT_ANY``, except that ``index`` is
  always filled with zero on success if not woken via alert.
  (在对象列表上等待并原子地获取其中全部。接受一个指向 struct :c:type:`ntsync_wait_args` 的指针，其用法与 ``NTSYNC_IOC_WAIT_ANY`` 完全相同，只是成功时(如果不是经 alert 唤醒)``index`` 总是被填为 0。)

  This function attempts to simultaneously acquire all of the given
  objects. If unable to do so, it sleeps until all objects become
  simultaneously signaled, subsequently acquiring them, or the timeout
  expires. In the latter case the ioctl fails with ``ETIMEDOUT`` and no
  objects are modified.
  (该函数尝试同时获取所有给定对象。如果无法做到，它会休眠，直到所有对象同时变为已发信号并随后被它获取，或超时到期。后一种情况下 ioctl 以 ``ETIMEDOUT`` 失败，且不修改任何对象。)

  Objects may become signaled and subsequently designaled (through
  acquisition by other threads) while this thread is sleeping. Only
  once all objects are simultaneously signaled does the ioctl acquire
  them and return. The entire acquisition is atomic and totally ordered
  with respect to other operations on any of the given objects.
  (在该线程休眠期间，对象可能变为已发信号、随后又被撤除信号(因被其他线程获取)。只有当所有对象同时处于已发信号状态时，ioctl 才会获取它们并返回。整个获取是原子的，并且相对于任何给定对象上的其他操作完全有序。)

  If an abandoned mutex is acquired, the ioctl fails with
  ``EOWNERDEAD``. Similarly to ``NTSYNC_IOC_WAIT_ANY``, all objects are
  nevertheless marked as acquired. Note that if multiple mutex objects
  are specified, there is no way to know which were marked as
  abandoned.
  (如果获取到被弃置的互斥体，ioctl 以 ``EOWNERDEAD`` 失败。与 ``NTSYNC_IOC_WAIT_ANY`` 类似，所有对象仍会被标记为已获取。注意，如果指定了多个互斥体对象，将无从得知哪些被标记为弃置。)

  As with "any" waits, the ``alert`` argument is an "extra" event which
  can terminate the wait. Critically, however, an "all" wait will
  succeed if all members in ``objs`` are signaled, *or* if ``alert`` is
  signaled. In the latter case ``index`` will be set to ``count``. As
  with "any" waits, if both conditions are filled, the former takes
  priority, and objects in ``objs`` will be acquired.
  (与 "any" 等待一样，``alert`` 参数是一个可以终止等待的"额外"事件。但关键区别是："all" 等待在 ``objs`` 中所有成员都被发信号 *或* ``alert`` 被发信号时会成功。后一种情况下 ``index`` 会被设为 ``count``。与 "any" 等待一样，如果两个条件都满足，前者优先，``objs`` 中的对象将被获取。)

  Unlike ``NTSYNC_IOC_WAIT_ANY``, it is not valid to pass the same
  object more than once, nor is it valid to pass the same object in
  ``objs`` and in ``alert``. If this is attempted, the function fails
  with ``EINVAL``.
  (与 ``NTSYNC_IOC_WAIT_ANY`` 不同，多次传入同一对象是非法的，在 ``objs`` 与 ``alert`` 中传入同一对象也是非法的。如果尝试这样做，函数以 ``EINVAL`` 失败。)
