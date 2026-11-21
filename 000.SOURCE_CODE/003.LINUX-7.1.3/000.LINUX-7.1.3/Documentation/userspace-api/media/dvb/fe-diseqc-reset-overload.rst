.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_DISEQC_RESET_OVERLOAD:

**************************************************
ioctl FE_DISEQC_RESET_OVERLOAD(恢复天线子系统供电)
**************************************************

Name(名称)
==========

FE_DISEQC_RESET_OVERLOAD - Restores the power to the antenna subsystem, if it was powered off due - to power overload.
(FE_DISEQC_RESET_OVERLOAD - 若天线子系统因电源过载而断电，则恢复其供电。)

Synopsis(概要)
==============

.. c:macro:: FE_DISEQC_RESET_OVERLOAD

``int ioctl(int fd, FE_DISEQC_RESET_OVERLOAD, NULL)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

Description(描述)
=================

If the bus has been automatically powered off due to power overload,
this ioctl call restores the power to the bus. The call requires
read/write access to the device. This call has no effect if the device
is manually powered off. Not all Digital TV adapters support this ioctl.
(如果总线已因电源过载而自动断电，此 ioctl 调用会恢复总线的供电。该调用需要
对设备的读写权限。若设备已被手动断电，此调用无效。并非所有数字电视适配器都
支持此 ioctl。)

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
