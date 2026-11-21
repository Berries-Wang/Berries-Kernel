.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_READ_BER:

***********************
FE_READ_BER(读取误码率)
***********************

Name(名称)
==========

FE_READ_BER

.. attention:: This ioctl is deprecated.
    (注意：此 ioctl 已被弃用。)

Synopsis(概要)
==============

.. c:macro:: FE_READ_BER

``int ioctl(int fd, FE_READ_BER, uint32_t *ber)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``ber``
    The bit error rate is stored into \*ber.
    (误码率被存储到 \*ber 中。)

Description(描述)
=================

This ioctl call returns the bit error rate for the signal currently
received/demodulated by the front-end. For this command, read-only
access to the device is sufficient.
(此 ioctl 调用返回前端当前接收/解调的信号的误码率。对于此命令，对设备的
只读访问就足够了。)

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
