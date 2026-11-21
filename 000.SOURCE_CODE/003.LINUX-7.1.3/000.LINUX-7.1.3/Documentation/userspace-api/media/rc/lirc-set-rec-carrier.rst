.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_rec_carrier:

****************************************
ioctl LIRC_SET_REC_CARRIER(设置接收载波)
****************************************

Name(名称)
==========

LIRC_SET_REC_CARRIER - Set carrier used to modulate IR receive.
(LIRC_SET_REC_CARRIER - 设置用于调制红外接收的载波。)

Synopsis(概要)
==============

.. c:macro:: LIRC_SET_REC_CARRIER

``int ioctl(int fd, LIRC_SET_REC_CARRIER, __u32 *frequency)``

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

Set receive carrier used to modulate IR PWM pulses and spaces.
(设置用于调制红外 PWM 脉冲与空隙的接收载波。)

.. note::

   If called together with :ref:`LIRC_SET_REC_CARRIER_RANGE`, this ioctl
   sets the upper bound frequency that will be recognized by the device.
   (若与 :ref:`LIRC_SET_REC_CARRIER_RANGE` 一同调用，此 ioctl 设置设备能够
   识别的频率上限。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
