.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-select:

*************
V4L2 select()
*************

Name(名称)
==========

v4l2-select - Synchronous I/O multiplexing
(v4l2-select - 同步 I/O 多路复用。)

Synopsis(概要)
==============

.. code-block:: c

    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>

.. c:function:: int select( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout )

Arguments(参数)
===============

``nfds``
  The highest-numbered file descriptor in any of the three sets, plus 1.
  (三个集合中编号最大的文件描述符加 1。)

``readfds``
  File descriptions to be watched if a read() call won't block.
  (要监视的文件描述，适用于 read() 调用不会阻塞的情况。)

``writefds``
  File descriptions to be watched if a write() won't block.
  (要监视的文件描述，适用于 write() 不会阻塞的情况。)

``exceptfds``
  File descriptions to be watched for V4L2 events.
  (要监视 V4L2 事件的文件描述。)

``timeout``
  Maximum time to wait.
  (等待的最长时间。)

Description(描述)
=================

With the :c:func:`select()` function applications can suspend
execution until the driver has captured data or is ready to accept data
for output.
(使用 :c:func:`select()` 函数，应用程序可以挂起执行，直到驱动程序已捕获数据或已准备好接受输出数据。)

When streaming I/O has been negotiated this function waits until a
buffer has been filled or displayed and can be dequeued with the
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. When buffers are already in
the outgoing queue of the driver the function returns immediately.
(当已协商流式 I/O 时，此函数会等待直到一个缓冲区被填充或显示并可以用 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl 出队。当缓冲区已经在驱动程序的输出队列中时，函数立即返回。)

On success :c:func:`select()` returns the total number of bits set in
``fd_set``. When the function timed out it returns
a value of zero. On failure it returns -1 and the ``errno`` variable is
set appropriately. When the application did not call
:ref:`VIDIOC_QBUF` or
:ref:`VIDIOC_STREAMON` yet the :c:func:`select()`
function succeeds, setting the bit of the file descriptor in ``readfds``
or ``writefds``, but subsequent :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`
calls will fail. [#f1]_
(成功时 :c:func:`select()` 返回 ``fd_set`` 中置位的总位数。当函数超时时它返回零。失败时返回 -1 并适当设置 ``errno`` 变量。当应用程序尚未调用 :ref:`VIDIOC_QBUF` 或 :ref:`VIDIOC_STREAMON` 时，:c:func:`select()` 函数也会成功，在 ``readfds`` 或 ``writefds`` 中设置该文件描述符的位，但随后的 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 调用会失败。)

When use of the :c:func:`read()` function has been negotiated and the
driver does not capture yet, the :c:func:`select()` function starts
capturing. When that fails, :c:func:`select()` returns successful and
a subsequent :c:func:`read()` call, which also attempts to start
capturing, will return an appropriate error code. When the driver
captures continuously (as opposed to, for example, still images) and
data is already available the :c:func:`select()` function returns
immediately.
(当已协商使用 :c:func:`read()` 函数而驱动程序尚未捕获时，:c:func:`select()` 函数会开始捕获。如果失败，:c:func:`select()` 仍返回成功，随后的 :c:func:`read()` 调用（它同样尝试开始捕获）将返回相应的错误码。当驱动程序连续捕获（相对于例如静态图像）且数据已经可用时，:c:func:`select()` 函数立即返回。)

When use of the :c:func:`write()` function has been negotiated the
:c:func:`select()` function just waits until the driver is ready for a
non-blocking :c:func:`write()` call.
(当已协商使用 :c:func:`write()` 函数时，:c:func:`select()` 函数只是等待，直到驱动程序准备好进行非阻塞的 :c:func:`write()` 调用。)

All drivers implementing the :c:func:`read()` or :c:func:`write()`
function or streaming I/O must also support the :c:func:`select()`
function.
(所有实现 :c:func:`read()` 或 :c:func:`write()` 函数或流式 I/O 的驱动程序还必须支持 :c:func:`select()` 函数。)

For more details see the :c:func:`select()` manual page.
(更多细节参见 :c:func:`select()` 手册页。)

Return Value(返回值)
====================

On success, :c:func:`select()` returns the number of descriptors
contained in the three returned descriptor sets, which will be zero if
the timeout expired. On error -1 is returned, and the ``errno`` variable
is set appropriately; the sets and ``timeout`` are undefined. Possible
error codes are:
(成功时，:c:func:`select()` 返回三个返回的描述符集合中所包含的描述符数量，如果超时到期则为零。出错时返回 -1，并适当设置 ``errno`` 变量；各个集合和 ``timeout`` 未定义。可能的错误码有：)

EBADF
    One or more of the file descriptor sets specified a file descriptor
    that is not open.
    (一个或多个文件描述符集合指定了未打开的文件描述符。)

EBUSY
    The driver does not support multiple read or write streams and the
    device is already in use.
    (驱动程序不支持多个读或写流，且设备已被占用。)

EFAULT
    The ``readfds``, ``writefds``, ``exceptfds`` or ``timeout`` pointer
    references an inaccessible memory area.
    (``readfds`` 、 ``writefds`` 、 ``exceptfds`` 或 ``timeout`` 指针引用了不可访问的内存区域。)

EINTR
    The call was interrupted by a signal.
    (调用被信号中断。)

EINVAL
    The ``nfds`` argument is less than zero or greater than
    ``FD_SETSIZE``.
    (``nfds`` 参数小于零或大于 ``FD_SETSIZE`` 。)

.. [#f1]
   The Linux kernel implements :c:func:`select()` like the
   :c:func:`poll()` function, but :c:func:`select()` cannot
   return a ``POLLERR``.
   (Linux 内核像 :c:func:`poll()` 函数一样实现 :c:func:`select()` ，但 :c:func:`select()` 不能返回 ``POLLERR`` 。)
