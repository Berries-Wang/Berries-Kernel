.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_GET_CHIPINFO_IOCTL:

*****************************************
GPIO_GET_CHIPINFO_IOCTL(获取芯片公开信息)
*****************************************

Name(名称)
==========

GPIO_GET_CHIPINFO_IOCTL - Get the publicly available information for a chip.
(GPIO_GET_CHIPINFO_IOCTL - 获取某个芯片可公开获得的信息。)

Synopsis(概要)
==============

.. c:macro:: GPIO_GET_CHIPINFO_IOCTL

``int ioctl(int chip_fd, GPIO_GET_CHIPINFO_IOCTL, struct gpiochip_info *info)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备的文件描述符。)

``info``
    The :c:type:`chip_info<gpiochip_info>` to be populated.
    (要填充的 :c:type:`chip_info<gpiochip_info>`。)

Description(描述)
=================

Gets the publicly available information for a particular GPIO chip.
(获取特定 GPIO 芯片可公开获得的信息。)

Return Value(返回值)
====================

On success 0 and ``info`` is populated with the chip info.
(成功时返回 0，且 ``info`` 被填充为芯片信息。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst 中
描述。)
