.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_READ_UNCORRECTED_BLOCKS:

******************************************
FE_READ_UNCORRECTED_BLOCKS(读取未纠正块数)
******************************************

Name(名称)
==========

FE_READ_UNCORRECTED_BLOCKS

.. attention:: This ioctl is deprecated.
    (注意：此 ioctl 已被弃用。)

Synopsis(概要)
==============

.. c:macro:: FE_READ_UNCORRECTED_BLOCKS

``int ioctl(int fd, FE_READ_UNCORRECTED_BLOCKS, uint32_t *ublocks)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``ublocks``
    The total number of uncorrected blocks seen by the driver so far.
    (驱动迄今为止看到的未纠正块的总数。)

Description(描述)
=================

This ioctl call returns the number of uncorrected blocks detected by the
device driver during its lifetime. For meaningful measurements, the
increment in block count during a specific time interval should be
calculated. For this command, read-only access to the device is
sufficient.
(此 ioctl 调用返回设备驱动在其生存期内检测到的未纠正块数量。要有意义的
测量，应计算特定时间间隔内块计数的增量。对于此命令，对设备的只读访问就
足够了。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
