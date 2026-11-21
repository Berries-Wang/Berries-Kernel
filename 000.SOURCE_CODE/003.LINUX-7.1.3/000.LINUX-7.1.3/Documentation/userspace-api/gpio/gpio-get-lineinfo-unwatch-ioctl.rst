.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_GET_LINEINFO_UNWATCH_IOCTL:

*****************************************************
GPIO_GET_LINEINFO_UNWATCH_IOCTL(停止监视线路信息变化)
*****************************************************

Name(名称)
==========

GPIO_GET_LINEINFO_UNWATCH_IOCTL - Disable watching a line for changes to its
requested state and configuration information.
(GPIO_GET_LINEINFO_UNWATCH_IOCTL - 停止监视某条线路的请求状态与配置信息的
变化。)

Synopsis(概要)
==============

.. c:macro:: GPIO_GET_LINEINFO_UNWATCH_IOCTL

``int ioctl(int chip_fd, GPIO_GET_LINEINFO_UNWATCH_IOCTL, u32 *offset)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备的文件描述符。)

``offset``
    The offset of the line to no longer watch.
    (不再监视的线路的偏移。)

Description(描述)
=================

Remove the line from the list of lines being watched on this ``chip_fd``.
(把该线路从此 ``chip_fd`` 上正在被监视的线路列表中移除。)

This is the reverse of gpio-v2-get-lineinfo-watch-ioctl.rst (v2) and
gpio-get-lineinfo-watch-ioctl.rst (v1).
(它是 gpio-v2-get-lineinfo-watch-ioctl.rst (v2) 和
gpio-get-lineinfo-watch-ioctl.rst (v1) 的逆操作。)

Unwatching a line that is not watched is an error (**EBUSY**).
(对未处于监视状态的线路执行取消监视是一个错误 (**EBUSY**)。)

First added in 5.7.
(最早添加于 5.7。)

Return Value(返回值)
====================

On success 0.
(成功时返回 0。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
