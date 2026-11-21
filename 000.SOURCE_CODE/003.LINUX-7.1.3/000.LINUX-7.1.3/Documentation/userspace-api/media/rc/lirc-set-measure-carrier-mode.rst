.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_set_measure_carrier_mode:

******************************************************
ioctl LIRC_SET_MEASURE_CARRIER_MODE(启用/停用测量模式)
******************************************************

Name(名称)
==========

LIRC_SET_MEASURE_CARRIER_MODE - enable or disable measure mode
(LIRC_SET_MEASURE_CARRIER_MODE - 启用或停用测量模式)

Synopsis(概要)
==============

.. c:macro:: LIRC_SET_MEASURE_CARRIER_MODE

``int ioctl(int fd, LIRC_SET_MEASURE_CARRIER_MODE, __u32 *enable)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``enable``
    enable = 1 means enable measure mode, enable = 0 means disable measure
    mode.
    (enable = 1 表示启用测量模式，enable = 0 表示停用测量模式。)

Description(描述)
=================

.. _lirc-mode2-frequency:

Enable or disable measure mode. If enabled, from the next key
press on, the driver will send ``LIRC_MODE2_FREQUENCY`` packets. By
default this should be turned off.
(启用或停用测量模式。若启用，则从下一次按键起，驱动将发送
``LIRC_MODE2_FREQUENCY`` 数据包。默认应关闭此模式。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
