.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_GET_LINEINFO_IOCTL:

********************************************
GPIO_V2_GET_LINEINFO_IOCTL(获取线路公开信息)
********************************************

Name(名称)
==========

GPIO_V2_GET_LINEINFO_IOCTL - Get the publicly available information for a line.(GPIO_V2_GET_LINEINFO_IOCTL - 获取某条线路的公开可用信息。)

Synopsis(概要)
==============

.. c:macro:: GPIO_V2_GET_LINEINFO_IOCTL

``int ioctl(int chip_fd, GPIO_V2_GET_LINEINFO_IOCTL, struct gpio_v2_line_info *info)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备的文件描述符。)

``info``
    The :c:type:`line_info<gpio_v2_line_info>` to be populated, with the
    ``offset`` field set to indicate the line to be collected.
    (待填充的 :c:type:`line_info<gpio_v2_line_info>`，其 ``offset`` 字段应
    设置为指示要收集的线路。)

Description(描述)
=================

Get the publicly available information for a line.
(获取某条线路的公开可用信息。)

This information is available independent of whether the line is in use.
(无论该线路是否正在使用，此信息都可用。)

.. note::
    The line info does not include the line value.
    (线路信息不包含线路值。)

    The line must be requested using gpio-v2-get-line-ioctl.rst to access its
    value.
    (必须使用 gpio-v2-get-line-ioctl.rst 请求该线路才能访问其值。)

Return Value(返回值)
====================

On success 0 and ``info`` is populated with the chip info.
(成功时返回 0，``info`` 中填充芯片信息。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
