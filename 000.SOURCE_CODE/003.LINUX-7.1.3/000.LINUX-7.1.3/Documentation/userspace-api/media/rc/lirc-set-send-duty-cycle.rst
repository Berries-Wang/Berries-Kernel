.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_send_duty_cycle:

**********************************************
ioctl LIRC_SET_SEND_DUTY_CYCLE(设置发送占空比)
**********************************************

Name(名称)
==========

LIRC_SET_SEND_DUTY_CYCLE - Set the duty cycle of the carrier signal for
IR transmit.
(LIRC_SET_SEND_DUTY_CYCLE - 设置红外发射载波信号的占空比。)

Synopsis(概要)
==============

.. c:macro:: LIRC_SET_SEND_DUTY_CYCLE

``int ioctl(int fd, LIRC_SET_SEND_DUTY_CYCLE, __u32 *duty_cycle)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``duty_cycle``
    Duty cycle, describing the pulse width in percent (from 1 to 99) of
    the total cycle. Values 0 and 100 are reserved.
    (占空比，描述脉冲宽度占总周期的百分比（1 到 99）。值 0 和 100 为保留值。)

Description(描述)
=================

Get/set the duty cycle of the carrier signal for IR transmit.
(获取/设置红外发射载波信号的占空比。)

Currently, no special meaning is defined for 0 or 100, but this
could be used to switch off carrier generation in the future, so
these values should be reserved.
(目前，0 和 100 尚无特殊含义，但将来可能用于关闭载波生成，因此这些值应
予保留。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
