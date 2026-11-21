.. SPDX-License-Identifier: GPL-2.0

==============
futex2(futex2)
==============

:Author: André Almeida <andrealmeid@collabora.com>
(作者：André Almeida <andrealmeid@collabora.com>)

futex, or fast user mutex, is a set of syscalls to allow userspace to create
performant synchronization mechanisms, such as mutexes, semaphores and
conditional variables in userspace. C standard libraries, like glibc, uses it
as a means to implement more high level interfaces like pthreads.
(futex，即快速用户互斥体 (fast user mutex)，是一组允许用户空间在用户态创建
高性能同步机制的系统调用，例如互斥体、信号量和条件变量。像 glibc 这样的
C 标准库用它来实现 pthreads 等更高级的接口。)

futex2 is a followup version of the initial futex syscall, designed to overcome
limitations of the original interface.
(futex2 是最初的 futex 系统调用的后续版本，旨在克服原接口的局限。)

User API(用户空间 API)
======================

``futex_waitv()`` (futex_waitv 系统调用)
----------------------------------------

Wait on an array of futexes, wake on any::

  futex_waitv(struct futex_waitv *waiters, unsigned int nr_futexes,
              unsigned int flags, struct timespec *timeout, clockid_t clockid)

  struct futex_waitv {
        __u64 val;
        __u64 uaddr;
        __u32 flags;
        __u32 __reserved;
  };

(在一组 futex 上等待，其中任何一个被唤醒时即被唤醒。)

Userspace sets an array of struct futex_waitv (up to a max of 128 entries),
using ``uaddr`` for the address to wait for, ``val`` for the expected value
and ``flags`` to specify the type (e.g. private) and size of futex.
``__reserved`` needs to be 0, but it can be used for future extension. The
pointer for the first item of the array is passed as ``waiters``. An invalid
address for ``waiters`` or for any ``uaddr`` returns ``-EFAULT``.
(用户空间设置一个 struct futex_waitv 数组 (最多 128 个条目)，其中
``uaddr`` 为要等待的地址，``val`` 为期望值，``flags`` 用于指定 futex 的
类型 (例如私有) 与大小。``__reserved`` 需要为 0，但可用于将来的扩展。
数组第一项的指针通过 ``waiters`` 传入。若 ``waiters`` 或任何 ``uaddr``
的地址无效，则返回 ``-EFAULT``。)

If userspace has 32-bit pointers, it should do a explicit cast to make sure
the upper bits are zeroed. ``uintptr_t`` does the tricky and it works for
both 32/64-bit pointers.
(如果用户空间使用 32 位指针，应进行显式转换以确保高位被清零。
``uintptr_t`` 可以巧妙地做到这一点，并且对 32/64 位指针都有效。)

``nr_futexes`` specifies the size of the array. Numbers out of [1, 128]
interval will make the syscall return ``-EINVAL``.
( ``nr_futexes`` 指定数组的大小。超出 [1, 128] 区间的数值会使该系统调用
返回 ``-EINVAL``。)

The ``flags`` argument of the syscall needs to be 0, but it can be used for
future extension.
(该系统调用的 ``flags`` 参数需要为 0，但可用于将来的扩展。)

For each entry in ``waiters`` array, the current value at ``uaddr`` is compared
to ``val``. If it's different, the syscall undo all the work done so far and
return ``-EAGAIN``. If all tests and verifications succeeds, syscall waits until
one of the following happens:
(对于 ``waiters`` 数组中的每个条目，``uaddr`` 处的当前值会与 ``val`` 进行
比较。如果不同，该系统调用会撤销到目前为止已完成的全部工作并返回
``-EAGAIN``。如果所有测试和检查都通过，系统调用将一直等待，直到发生下列
情况之一：)

- The timeout expires, returning ``-ETIMEOUT``.
  (超时到期，返回 ``-ETIMEOUT``。)
- A signal was sent to the sleeping task, returning ``-ERESTARTSYS``.
  (有信号被发送到正在睡眠的任务，返回 ``-ERESTARTSYS``。)
- Some futex at the list was woken, returning the index of some waked futex.
  (列表中的某个 futex 被唤醒，返回某个被唤醒 futex 的索引。)

An example of how to use the interface can be found at ``tools/testing/selftests/futex/functional/futex_waitv.c``.
(关于如何使用该接口的示例可在
``tools/testing/selftests/futex/functional/futex_waitv.c`` 中找到。)

Timeout(超时)
-------------

``struct timespec *timeout`` argument is an optional argument that points to an
absolute timeout. You need to specify the type of clock being used at
``clockid`` argument. ``CLOCK_MONOTONIC`` and ``CLOCK_REALTIME`` are supported.
This syscall accepts only 64bit timespec structs.
( ``struct timespec *timeout`` 参数是可选参数，指向一个绝对超时。你需要在
``clockid`` 参数中指定所使用的时钟类型。支持 ``CLOCK_MONOTONIC`` 与
``CLOCK_REALTIME``。该系统调用只接受 64 位的 timespec 结构体。)

Types of futex(futex 的类型)
----------------------------

A futex can be either private or shared. Private is used for processes that
shares the same memory space and the virtual address of the futex will be the
same for all processes. This allows for optimizations in the kernel. To use
private futexes, it's necessary to specify ``FUTEX_PRIVATE_FLAG`` in the futex
flag. For processes that doesn't share the same memory space and therefore can
have different virtual addresses for the same futex (using, for instance, a
file-backed shared memory) requires different internal mechanisms to be get
properly enqueued. This is the default behavior, and it works with both private
and shared futexes.
(futex 可以是私有的或共享的。私有 futex 用于共享同一内存空间的进程，此时
futex 的虚拟地址对所有进程都相同。这允许内核进行优化。要使用私有 futex，
必须在 futex 标志中指定 ``FUTEX_PRIVATE_FLAG``。对于不共享同一内存空间、
因而同一个 futex 可能具有不同虚拟地址的进程 (例如使用基于文件的共享内存
时)，则需要不同的内部机制才能正确入队。这是默认行为，且对私有和共享
futex 都适用。)

Futexes can be of different sizes: 8, 16, 32 or 64 bits. Currently, the only
supported one is 32 bit sized futex, and it need to be specified using
``FUTEX_32`` flag.
(futex 可以有不同的尺寸：8、16、32 或 64 位。当前只支持 32 位大小的
futex，并且需要通过 ``FUTEX_32`` 标志来指定。)
