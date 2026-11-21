.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _request-func-poll:

**************
request poll()
**************

Name(名称)
==========

request-poll - Wait for some event on a file descriptor
(request-poll - 在文件描述符上等待某个事件。)

Synopsis(概要)
==============

.. code-block:: c

    #include <sys/poll.h>

.. c:function:: int poll( struct pollfd *ufds, unsigned int nfds, int timeout )

Arguments(参数)
===============

``ufds``
   List of file descriptor events to be watched
   (要监视的文件描述符事件列表。)

``nfds``
   Number of file descriptor events at the \*ufds array
   ( \*ufds 数组中文件描述符事件的数量。)

``timeout``
   Timeout to wait for events
   (等待事件的超时时间。)

Description(描述)
=================

With the :c:func:`poll()` function applications can wait
for a request to complete.
(应用程序可以使用 :c:func:`poll()` 函数等待请求完成。)

On success :c:func:`poll()` returns the number of file
descriptors that have been selected (that is, file descriptors for which the
``revents`` field of the respective struct :c:type:`pollfd`
is non-zero). Request file descriptor set the ``POLLPRI`` flag in ``revents``
when the request was completed.  When the function times out it returns
a value of zero, on failure it returns -1 and the ``errno`` variable is
set appropriately.
(成功时， :c:func:`poll()` 返回被选中的文件描述符的数量 (即相应 struct
:c:type:`pollfd` 的 ``revents`` 字段非零的那些文件描述符)。请求完成
时，请求文件描述符会在 ``revents`` 中设置 ``POLLPRI`` 标志。函数超时
时返回零；失败时返回 -1 并适当设置 ``errno`` 变量。)

Attempting to poll for a request that is not yet queued will
set the ``POLLERR`` flag in ``revents``.
(试图对一个尚未入队的请求进行 poll，会在 ``revents`` 中设置
``POLLERR`` 标志。)

Return Value(返回值)
====================

On success, :c:func:`poll()` returns the number of
structures which have non-zero ``revents`` fields, or zero if the call
timed out. On error -1 is returned, and the ``errno`` variable is set
appropriately:
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
