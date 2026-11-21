.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_GET_EVENT:

************
FE_GET_EVENT
************

Name(名称)
==========

FE_GET_EVENT
(FE_GET_EVENT 调用。)

.. attention:: This ioctl is deprecated.
               (注意：此 ioctl 已被弃用。)

Synopsis(概要)
==============

.. c:macro:: FE_GET_EVENT

``int ioctl(int fd, FE_GET_EVENT, struct dvb_frontend_event *ev)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``ev``
    Points to the location where the event, if any, is to be stored.
    (指向存储事件 (如果有) 的位置。)

Description(描述)
=================

This ioctl call returns a frontend event if available. If an event is
not available, the behavior depends on whether the device is in blocking
or non-blocking mode. In the latter case, the call fails immediately
with errno set to ``EWOULDBLOCK``. In the former case, the call blocks until
an event becomes available.
(此 ioctl 调用返回一个前端事件 (如果有可用的事件)。如果事件不可用，
其行为取决于设备处于阻塞模式还是非阻塞模式。在后一种情况下，调用立
即失败，errno 被设置为 ``EWOULDBLOCK``。在前一种情况下，调用会阻塞，
直到有事件可用。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并适当设置 ``errno`` 变量。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  .. row 1

       -  ``EWOULDBLOCK``

       -  There is no event pending, and the device is in non-blocking mode.
          (没有待处理的事件，且设备处于非阻塞模式。)

    -  .. row 2

       -  ``EOVERFLOW``

       -  Overflow in event queue - one or more events were lost.
          (事件队列溢出——一个或多个事件丢失了。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
