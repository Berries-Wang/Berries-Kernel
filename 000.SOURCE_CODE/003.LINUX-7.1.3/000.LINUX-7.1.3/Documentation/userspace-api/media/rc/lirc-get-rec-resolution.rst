.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_get_rec_resolution:

*********************************************
ioctl LIRC_GET_REC_RESOLUTION(获取接收分辨率)
*********************************************

Name(名称)
==========

LIRC_GET_REC_RESOLUTION - Obtain the value of receive resolution, in microseconds.
(LIRC_GET_REC_RESOLUTION - 获取接收分辨率的值，单位为微秒。)

Synopsis(概要)
==============

.. c:macro:: LIRC_GET_REC_RESOLUTION

``int ioctl(int fd, LIRC_GET_REC_RESOLUTION, __u32 *microseconds)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``microseconds``
    Resolution, in microseconds.
    (分辨率，单位为微秒。)

Description(描述)
=================

Some receivers have maximum resolution which is defined by internal
sample rate or data format limitations. E.g. it's common that
signals can only be reported in 50 microsecond steps.
(部分接收器的最大分辨率由内部采样率或数据格式限制所决定。例如，常见情况是
信号只能以 50 微秒为步长上报。)

This ioctl returns the integer value with such resolution, with can be
used by userspace applications like lircd to automatically adjust the
tolerance value.
(该 ioctl 返回具有此分辨率的整数值，lircd 等用户空间应用程序可用它来自动
调整容差值。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
