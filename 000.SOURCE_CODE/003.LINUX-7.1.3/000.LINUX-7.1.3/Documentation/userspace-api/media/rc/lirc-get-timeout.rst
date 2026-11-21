.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_get_min_timeout:
.. _lirc_get_max_timeout:

**************************************************************************
ioctls LIRC_GET_MIN_TIMEOUT and LIRC_GET_MAX_TIMEOUT(获取 IR 接收超时范围)
**************************************************************************

Name(名称)
==========

LIRC_GET_MIN_TIMEOUT / LIRC_GET_MAX_TIMEOUT - Obtain the possible timeout
range for IR receive.
(LIRC_GET_MIN_TIMEOUT / LIRC_GET_MAX_TIMEOUT - 获取 IR 接收可设置的超时
范围。)

Synopsis(概要)
==============

.. c:macro:: LIRC_GET_MIN_TIMEOUT

``int ioctl(int fd, LIRC_GET_MIN_TIMEOUT, __u32 *timeout)``

.. c:macro:: LIRC_GET_MAX_TIMEOUT

``int ioctl(int fd, LIRC_GET_MAX_TIMEOUT, __u32 *timeout)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``timeout``
    Timeout, in microseconds.
    (超时，单位为微秒。)

Description(描述)
=================

Some devices have internal timers that can be used to detect when
there's no IR activity for a long time. This can help lircd in
detecting that a IR signal is finished and can speed up the decoding
process. Returns an integer value with the minimum/maximum timeout
that can be set.
(有些设备带有内部定时器，可用于检测长时间没有 IR 活动的情况。这有助于
lircd 判断 IR 信号已经结束，并可加快解码过程。返回一个整数值，即可设置的
最小/最大超时。)

.. note::

   Some devices have a fixed timeout, in that case
   both ioctls will return the same value even though the timeout
   cannot be changed via :ref:`LIRC_SET_REC_TIMEOUT`.
   (有些设备具有固定超时。这种情况下，即使超时无法通过
   :ref:`LIRC_SET_REC_TIMEOUT` 更改，两个 ioctl 也会返回相同的值。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
