.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_rec_carrier_range:

**************************************************
ioctl LIRC_SET_REC_CARRIER_RANGE(设置接收载波范围)
**************************************************

Name(名称)
==========

LIRC_SET_REC_CARRIER_RANGE - Set lower bound of the carrier used to modulate
IR receive.
(LIRC_SET_REC_CARRIER_RANGE - 设置用于调制 IR 接收的载波的下界。)

Synopsis(概要)
==============

.. c:macro:: LIRC_SET_REC_CARRIER_RANGE

``int ioctl(int fd, LIRC_SET_REC_CARRIER_RANGE, __u32 *frequency)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``frequency``
    Frequency of the carrier that modulates PWM data, in Hz.
    (调制 PWM 数据的载波频率，单位为 Hz。)

Description(描述)
=================

This ioctl sets the upper range of carrier frequency that will be recognized
by the IR receiver.
(此 ioctl 设置 IR 接收器将识别的载波频率的上限范围。)

.. note::

   To set a range use :ref:`LIRC_SET_REC_CARRIER_RANGE
   <LIRC_SET_REC_CARRIER_RANGE>` with the lower bound first and later call
   :ref:`LIRC_SET_REC_CARRIER <LIRC_SET_REC_CARRIER>` with the upper bound.
   (要设置一个范围，先用下界调用 :ref:`LIRC_SET_REC_CARRIER_RANGE
   <LIRC_SET_REC_CARRIER_RANGE>`，之后再用上界调用
   :ref:`LIRC_SET_REC_CARRIER <LIRC_SET_REC_CARRIER>` 。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
