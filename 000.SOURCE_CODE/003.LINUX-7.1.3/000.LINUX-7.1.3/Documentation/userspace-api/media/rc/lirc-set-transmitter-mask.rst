.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_transmitter_mask:

***********************************************
ioctl LIRC_SET_TRANSMITTER_MASK(设置发射器掩码)
***********************************************

Name(名称)
==========

LIRC_SET_TRANSMITTER_MASK - Enables send codes on a given set of transmitters(LIRC_SET_TRANSMITTER_MASK - 启用在给定的一组发射器上发送代码)

Synopsis(概要)
==============

.. c:macro:: LIRC_SET_TRANSMITTER_MASK

``int ioctl(int fd, LIRC_SET_TRANSMITTER_MASK, __u32 *mask)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``mask``
    Mask with channels to enable tx. Channel 0 is the least significant bit.
    (要启用发送 (tx) 的通道掩码。通道 0 对应最低有效位。)

Description(描述)
=================

Some IR TX devices have multiple output channels, in such case,
:ref:`LIRC_CAN_SET_TRANSMITTER_MASK <LIRC-CAN-SET-TRANSMITTER-MASK>` is
returned via :ref:`LIRC_GET_FEATURES` and this ioctl sets what channels will
send IR codes.
(有些 IR 发射设备有多个输出通道。这种情况下，会通过 :ref:`LIRC_GET_FEATURES`
返回 :ref:`LIRC_CAN_SET_TRANSMITTER_MASK <LIRC-CAN-SET-TRANSMITTER-MASK>` ，
此 ioctl 设置由哪些通道发送 IR 代码。)

This ioctl enables the given set of transmitters. The first transmitter is
encoded by the least significant bit and so on.
(此 ioctl 启用给定的一组发射器。第一个发射器由最低有效位编码，依此类推。)

When an invalid bit mask is given, i.e. a bit is set, even though the device
does not have so many transitters, then this ioctl returns the number of
available transitters and does nothing otherwise.
(当给出无效位掩码时，即虽然设备没有那么多发射器却设置了相应位，此 ioctl 会
返回可用发射器的数量；否则不做任何事。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
