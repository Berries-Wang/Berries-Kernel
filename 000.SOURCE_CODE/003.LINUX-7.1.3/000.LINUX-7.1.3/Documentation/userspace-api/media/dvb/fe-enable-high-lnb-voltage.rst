.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_ENABLE_HIGH_LNB_VOLTAGE:

*************************************************
ioctl FE_ENABLE_HIGH_LNB_VOLTAGE(启用高 LNB 电压)
*************************************************

Name(名称)
==========

FE_ENABLE_HIGH_LNB_VOLTAGE - Select output DC level between normal LNBf voltages or higher LNBf - voltages.(FE_ENABLE_HIGH_LNB_VOLTAGE - 在正常 LNBf 电压与更高的 LNBf 电压之间选择输出直流电平。)

Synopsis(概要)
==============

.. c:macro:: FE_ENABLE_HIGH_LNB_VOLTAGE

``int ioctl(int fd, FE_ENABLE_HIGH_LNB_VOLTAGE, unsigned int high)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``high``
    Valid flags:
    (有效标志：)

    -  0 - normal 13V and 18V.(0 - 正常的 13V 和 18V。)

    -  >0 - enables slightly higher voltages instead of 13/18V, in order
       to compensate for long antenna cables.(大于 0 - 启用略高于 13/18V
       的电压，以补偿较长的天线电缆。)

Description(描述)
=================

Select output DC level between normal LNBf voltages or higher LNBf
voltages between 0 (normal) or a value grater than 0 for higher
voltages.
(在正常 LNBf 电压或更高的 LNBf 电压之间选择输出直流电平：0 表示正常，大于
0 的值表示更高电压。)

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
