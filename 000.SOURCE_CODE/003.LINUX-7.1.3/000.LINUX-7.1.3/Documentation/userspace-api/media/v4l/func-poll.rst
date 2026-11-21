.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-poll:

***********
V4L2 poll()
***********

Name(名称)
==========

v4l2-poll - Wait for some event on a file descriptor
(v4l2-poll - 等待文件描述符上的某个事件。)

Synopsis(概要)
==============

.. code-block:: c

    #include <sys/poll.h>

.. c:function:: int poll( struct pollfd *ufds, unsigned int nfds, int timeout )

Arguments(参数)
===============


Description(描述)
=================

With the :c:func:`poll()` function applications can suspend execution
until the driver has captured data or is ready to accept data for
output.
(使用 :c:func:`poll()` 函数，应用程序可以挂起执行，直到驱动程序已捕获数据或已准备好接受输出数据。)

When streaming I/O has been negotiated this function waits until a
buffer has been filled by the capture device and can be dequeued with
the :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. For output devices this
function waits until the device is ready to accept a new buffer to be
queued up with the :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl for
display. When buffers are already in the outgoing queue of the driver
(capture) or the incoming queue isn't full (display) the function
returns immediately.
(当已协商流式 I/O 时，此函数会等待直到捕获设备填满一个缓冲区并可以用 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl 出队。对于输出设备，此函数会等待直到设备准备好接受一个新缓冲区，以便用 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl 入队进行显示。当缓冲区已经在驱动程序的输出队列中（捕获）或传入队列未满（显示）时，函数立即返回。)

On success :c:func:`poll()` returns the number of file descriptors
that have been selected (that is, file descriptors for which the
``revents`` field of the respective ``struct pollfd`` structure
is non-zero). Capture devices set the ``POLLIN`` and ``POLLRDNORM``
flags in the ``revents`` field, output devices the ``POLLOUT`` and
``POLLWRNORM`` flags. When the function timed out it returns a value of
zero, on failure it returns -1 and the ``errno`` variable is set
appropriately. When the application did not call
:ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` the :c:func:`poll()`
function succeeds, but sets the ``POLLERR`` flag in the ``revents``
field. When the application has called
:ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` for a capture device but
hasn't yet called :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`, the
:c:func:`poll()` function succeeds and sets the ``POLLERR`` flag in
the ``revents`` field. For output devices this same situation will cause
:c:func:`poll()` to succeed as well, but it sets the ``POLLOUT`` and
``POLLWRNORM`` flags in the ``revents`` field.
(成功时 :c:func:`poll()` 返回被选中的文件描述符的数量（即相应 ``struct pollfd`` 结构的 ``revents`` 字段非零的文件描述符）。捕获设备在 ``revents`` 字段中设置 ``POLLIN`` 和 ``POLLRDNORM`` 标志，输出设备设置 ``POLLOUT`` 和 ``POLLWRNORM`` 标志。当函数超时时它返回零，失败时返回 -1 并适当设置 ``errno`` 变量。当应用程序尚未调用 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 时，:c:func:`poll()` 函数成功，但在 ``revents`` 字段中设置 ``POLLERR`` 标志。当应用程序已为捕获设备调用 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 但尚未调用 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` 时，:c:func:`poll()` 函数成功并在 ``revents`` 字段中设置 ``POLLERR`` 标志。对于输出设备，同样的情况也会使 :c:func:`poll()` 成功，但它会在 ``revents`` 字段中设置 ``POLLOUT`` 和 ``POLLWRNORM`` 标志。)

If an event occurred (see :ref:`VIDIOC_DQEVENT`)
then ``POLLPRI`` will be set in the ``revents`` field and
:c:func:`poll()` will return.
(如果发生了事件（参见 :ref:`VIDIOC_DQEVENT` ），则 ``POLLPRI`` 将在 ``revents`` 字段中置位，:c:func:`poll()` 将返回。)

When use of the :c:func:`read()` function has been negotiated and the
driver does not capture yet, the :c:func:`poll()` function starts
capturing. When that fails it returns a ``POLLERR`` as above. Otherwise
it waits until data has been captured and can be read. When the driver
captures continuously (as opposed to, for example, still images) the
function may return immediately.
(当已协商使用 :c:func:`read()` 函数而驱动程序尚未捕获时，:c:func:`poll()` 函数开始捕获。如果失败，它会像上面那样返回 ``POLLERR`` 。否则，它等待直到数据已被捕获并且可以读取。当驱动程序连续捕获（相对于例如静态图像）时，函数可能立即返回。)

When use of the :c:func:`write()` function has been negotiated and the
driver does not stream yet, the :c:func:`poll()` function starts
streaming. When that fails it returns a ``POLLERR`` as above. Otherwise
it waits until the driver is ready for a non-blocking
:c:func:`write()` call.
(当已协商使用 :c:func:`write()` 函数而驱动程序尚未流式传输时，:c:func:`poll()` 函数开始流式传输。如果失败，它会像上面那样返回 ``POLLERR`` 。否则，它等待直到驱动程序准备好进行非阻塞的 :c:func:`write()` 调用。)

If the caller is only interested in events (just ``POLLPRI`` is set in
the ``events`` field), then :c:func:`poll()` will *not* start
streaming if the driver does not stream yet. This makes it possible to
just poll for events and not for buffers.
(如果调用者只对事件感兴趣（ ``events`` 字段中只设置了 ``POLLPRI`` ），那么在驱动程序尚未流式传输时，:c:func:`poll()` 将 *不会* 开始流式传输。这使得可以只轮询事件而不轮询缓冲区。)

All drivers implementing the :c:func:`read()` or :c:func:`write()`
function or streaming I/O must also support the :c:func:`poll()`
function.
(所有实现 :c:func:`read()` 或 :c:func:`write()` 函数或流式 I/O 的驱动程序还必须支持 :c:func:`poll()` 函数。)

For more details see the :c:func:`poll()` manual page.
(更多细节参见 :c:func:`poll()` 手册页。)

Return Value(返回值)
====================

On success, :c:func:`poll()` returns the number structures which have
non-zero ``revents`` fields, or zero if the call timed out. On error -1
is returned, and the ``errno`` variable is set appropriately:
(成功时，:c:func:`poll()` 返回 ``revents`` 字段非零的结构体数量，如果调用超时则返回零。出错时返回 -1，并适当设置 ``errno`` 变量：)

EBADF
    One or more of the ``ufds`` members specify an invalid file
    descriptor.
    (一个或多个 ``ufds`` 成员指定了无效的文件描述符。)

EBUSY
    The driver does not support multiple read or write streams and the
    device is already in use.
    (驱动程序不支持多个读或写流，且设备已被占用。)

EFAULT
    ``ufds`` references an inaccessible memory area.
    (``ufds`` 引用了不可访问的内存区域。)

EINTR
    The call was interrupted by a signal.
    (调用被信号中断。)

EINVAL
    The ``nfds`` value exceeds the ``RLIMIT_NOFILE`` value. Use
    ``getrlimit()`` to obtain this value.
    (``nfds`` 值超过 ``RLIMIT_NOFILE`` 值。使用 ``getrlimit()`` 获取此值。)
