.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_wideband_receiver:

************************************************
ioctl LIRC_SET_WIDEBAND_RECEIVER(启用宽带接收器)
************************************************

Name(名称)
==========

LIRC_SET_WIDEBAND_RECEIVER - enable wide band receiver.(LIRC_SET_WIDEBAND_RECEIVER - 启用宽带接收器。)

Synopsis(概要)
==============

.. c:macro:: LIRC_SET_WIDEBAND_RECEIVER

``int ioctl(int fd, LIRC_SET_WIDEBAND_RECEIVER, __u32 *enable)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``enable``
    enable = 1 means enable wideband receiver, enable = 0 means disable
    wideband receiver.
    (enable = 1 表示启用宽带接收器，enable = 0 表示禁用宽带接收器。)

Description(描述)
=================

Some receivers are equipped with special wide band receiver which is
intended to be used to learn output of existing remote. This ioctl
allows enabling or disabling it.
(有些接收器带有特殊的宽带接收器，用于学习现有遥控器的输出。此 ioctl 允许
启用或禁用它。)

This might be useful of receivers that have otherwise narrow band receiver
that prevents them to be used with some remotes. Wide band receiver might
also be more precise. On the other hand its disadvantage it usually
reduced range of reception.
(这对于那些原本只有窄带接收器、因而无法与某些遥控器配合使用的接收器可能有
用。宽带接收器也可能更精确。另一方面，它的缺点通常是接收范围缩小。)

.. note::

    Wide band receiver might be implicitly enabled if you enable
    carrier reports. In that case it will be disabled as soon as you disable
    carrier reports. Trying to disable wide band receiver while carrier
    reports are active will do nothing.
    (如果启用了载波报告 (carrier reports)，宽带接收器可能会被隐式启用。在
    这种情况下，一旦禁用载波报告，它就会被禁用。在载波报告处于活动状态时
    试图禁用宽带接收器将不起任何作用。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
