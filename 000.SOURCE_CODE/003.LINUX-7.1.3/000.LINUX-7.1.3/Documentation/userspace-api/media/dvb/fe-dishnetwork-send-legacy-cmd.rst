.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_DISHNETWORK_SEND_LEGACY_CMD:

****************************************************
FE_DISHNETWORK_SEND_LEGACY_CMD(发送 DISEqC 传统命令)
****************************************************

Name(名称)
==========

FE_DISHNETWORK_SEND_LEGACY_CMD

Synopsis(概要)
==============

.. c:macro:: FE_DISHNETWORK_SEND_LEGACY_CMD

``int ioctl(int fd, FE_DISHNETWORK_SEND_LEGACY_CMD, unsigned long cmd)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``cmd``
    Sends the specified raw cmd to the dish via DISEqC.
    (通过 DISEqC 把指定的原始命令发送到碟形天线。)

Description(描述)
=================

.. warning::
   This is a very obscure legacy command, used only at stv0299
   driver. Should not be used on newer drivers.
   (这是一个非常晦涩难懂的传统命令，只在 stv0299 驱动中使用。不应在较新的
   驱动上使用。)

It provides a non-standard method for selecting Diseqc voltage on the
frontend, for Dish Network legacy switches.
(它为 Dish Network 传统开关提供了一种在器上选择 Diseqc 电压的非标准
方法。)

As support for this ioctl were added in 2004, this means that such
dishes were already legacy in 2004.
(由于对此 ioctl 的支持是在 2004 年加入的，这说明这类碟形天线在 2004 年
就已经是传统设备了。)

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
