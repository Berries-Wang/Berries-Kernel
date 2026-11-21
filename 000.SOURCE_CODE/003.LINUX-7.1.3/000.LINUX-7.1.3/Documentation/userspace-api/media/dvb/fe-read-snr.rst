.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_READ_SNR:

***********************
FE_READ_SNR(读取信噪比)
***********************

Name(名称)
==========

FE_READ_SNR

.. attention:: This ioctl is deprecated.
    (注意：此 ioctl 已被弃用。)

Synopsis(概要)
==============

.. c:macro:: FE_READ_SNR

``int ioctl(int fd, FE_READ_SNR, int16_t *snr)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``snr``
    The signal-to-noise ratio is stored into \*snr.
    (信噪比被存储到 \*snr 中。)

Description(描述)
=================

This ioctl call returns the signal-to-noise ratio for the signal
currently received by the front-end. For this command, read-only access
to the device is sufficient.
(此 ioctl 调用返回前端当前接收到的信号的信噪比。对于此命令，对设备的只读
访问就足够了。)

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
