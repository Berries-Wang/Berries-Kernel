.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_rec_timeout:
.. _lirc_get_rec_timeout:

****************************************************************************
ioctl LIRC_GET_REC_TIMEOUT and LIRC_SET_REC_TIMEOUT(获取/设置 IR 非活动超时)
****************************************************************************

Name(名称)
==========

LIRC_GET_REC_TIMEOUT/LIRC_SET_REC_TIMEOUT - Get/set the integer value for IR inactivity timeout.(LIRC_GET_REC_TIMEOUT/LIRC_SET_REC_TIMEOUT - 获取/设置 IR 非活动超时的整数值。)

Synopsis(概要)
==============

.. c:macro:: LIRC_GET_REC_TIMEOUT

``int ioctl(int fd, LIRC_GET_REC_TIMEOUT, __u32 *timeout)``

.. c:macro:: LIRC_SET_REC_TIMEOUT

``int ioctl(int fd, LIRC_SET_REC_TIMEOUT, __u32 *timeout)``

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

Get and set the integer value for IR inactivity timeout.
(获取和设置 IR 非活动超时的整数值。)

If supported by the hardware, setting it to 0  disables all hardware timeouts
and data should be reported as soon as possible. If the exact value
cannot be set, then the next possible value _greater_ than the
given value should be set.
(如果硬件支持，将其设置为 0 会禁用所有硬件超时，数据应尽快上报。如果无法
设置确切的值，则应设置比给定值更大 (_greater_) 的下一个可能值。)

.. note::

   The range of supported timeout is given by :ref:`LIRC_GET_MIN_TIMEOUT`.
   (支持的超时范围由 :ref:`LIRC_GET_MIN_TIMEOUT` 给出。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
