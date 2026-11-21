.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _cec-func-poll:

**********
cec poll()
**********

Name(名称)
==========

cec-poll - Wait for some event on a file descriptor
(cec-poll - 在文件描述符上等待某个事件。)

Synopsis(概要)
==============

.. code-block:: c

    #include <sys/poll.h>

.. c:function:: int poll( struct pollfd *ufds, unsigned int nfds, int timeout )

Arguments(参数)
===============

``ufds``
   List of FD events to be watched
   (要监视的 FD 事件列表。)

``nfds``
   Number of FD events at the \*ufds array
   ( \*ufds 数组中 FD 事件的数量。)

``timeout``
   Timeout to wait for events
   (等待事件的超时时间。)

Description(描述)
=================

With the :c:func:`poll()` function applications can wait for CEC
events.
(应用程序可以使用 :c:func:`poll()` 函数等待 CEC 事件。)

On success :c:func:`poll()` returns the number of file descriptors
that have been selected (that is, file descriptors for which the
``revents`` field of the respective struct :c:type:`pollfd`
is non-zero). CEC devices set the ``POLLIN`` and ``POLLRDNORM`` flags in
the ``revents`` field if there are messages in the receive queue. If the
transmit queue has room for new messages, the ``POLLOUT`` and
``POLLWRNORM`` flags are set. If there are events in the event queue,
then the ``POLLPRI`` flag is set. When the function times out it returns
a value of zero, on failure it returns -1 and the ``errno`` variable is
set appropriately.
(成功时， :c:func:`poll()` 返回被选中的文件描述符的数量 (即相应 struct
:c:type:`pollfd` 的 ``revents`` 字段非零的那些文件描述符)。如果接收队
列中有消息，CEC 设备会在 ``revents`` 字段中设置 ``POLLIN`` 与
``POLLRDNORM`` 标志。如果发送队列还有空间容纳新消息，则会设置
``POLLOUT`` 与 ``POLLWRNORM`` 标志。如果事件队列中有事件，则会设置
``POLLPRI`` 标志。函数超时时返回零；失败时返回 -1 并适当设置
``errno`` 变量。)

For more details see the :c:func:`poll()` manual page.
(更多细节见 :c:func:`poll()` 手册页。)

Return Value(返回值)
====================

On success, :c:func:`poll()` returns the number structures which have
non-zero ``revents`` fields, or zero if the call timed out. On error -1
is returned, and the ``errno`` variable is set appropriately:
(成功时， :c:func:`poll()` 返回 ``revents`` 字段非零的结构数量；如果
调用超时则返回零。出错时返回 -1，并适当设置 ``errno`` 变量：)

``EBADF``
    One or more of the ``ufds`` members specify an invalid file
    descriptor.
    ( ``ufds`` 的一个或多个成员指定了无效的文件描述符。)

``EFAULT``
    ``ufds`` references an inaccessible memory area.
    ( ``ufds`` 引用了不可访问的内存区域。)

``EINTR``
    The call was interrupted by a signal.
    (调用被信号中断。)

``EINVAL``
    The ``nfds`` value exceeds the ``RLIMIT_NOFILE`` value. Use
    ``getrlimit()`` to obtain this value.
    ( ``nfds`` 值超过了 ``RLIMIT_NOFILE`` 值。使用 ``getrlimit()`` 获取
    该值。)
