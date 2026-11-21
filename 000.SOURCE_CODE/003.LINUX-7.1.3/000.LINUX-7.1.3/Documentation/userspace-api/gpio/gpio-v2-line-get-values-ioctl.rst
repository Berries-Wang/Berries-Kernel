.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_LINE_GET_VALUES_IOCTL:

*************************************************
GPIO_V2_LINE_GET_VALUES_IOCTL(获取已请求线路的值)
*************************************************

Name(名称)
==========

GPIO_V2_LINE_GET_VALUES_IOCTL - Get the values of requested lines.(GPIO_V2_LINE_GET_VALUES_IOCTL - 获取已请求线路的值。)

Synopsis(概要)
==============

.. c:macro:: GPIO_V2_LINE_GET_VALUES_IOCTL

``int ioctl(int req_fd, GPIO_V2_LINE_GET_VALUES_IOCTL, struct gpio_v2_line_values *values)``

Arguments(参数)
===============

``req_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpio_v2_line_request>` by gpio-v2-get-line-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-v2-get-line-ioctl.rst 中的
    :c:type:`request.fd<gpio_v2_line_request>` 返回。)

``values``
    The :c:type:`line_values<gpio_v2_line_values>` to get with the ``mask`` set
    to indicate the subset of requested lines to get.
    (要获取的 :c:type:`line_values<gpio_v2_line_values>`，其 ``mask`` 应
    设置为指示要获取的已请求线路的子集。)

Description(描述)
=================

Get the values of requested lines.
(获取已请求线路的值。)

The values returned are logical, indicating if the line is active or inactive.
The ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` flag controls the mapping between physical
values (high/low) and logical values (active/inactive).
If ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` is not set then high is active and low is
inactive.  If ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` is set then low is active and
high is inactive.
(返回的值是逻辑值，表明线路处于活动还是非活动状态。
``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` 标志控制物理值 (高/低) 与逻辑值
(活动/非活动) 之间的映射。如果未设置 ``GPIO_V2_LINE_FLAG_ACTIVE_LOW``，
则高电平为活动、低电平为非活动；如果设置了 ``GPIO_V2_LINE_FLAG_ACTIVE_LOW``，
则低电平为活动、高电平为非活动。)

The values of both input and output lines may be read.
(输入和输出线路的值都可以读取。)

For output lines, the value returned is driver and configuration dependent and
may be either the output buffer (the last requested value set) or the input
buffer (the actual level of the line), and depending on the hardware and
configuration these may differ.
(对于输出线路，返回的值取决于驱动和配置，可能是输出缓冲区 (最后请求设置
的值) 或输入缓冲区 (线路的实际电平)，视硬件与配置不同，两者可能不同。)

Return Value(返回值)
====================

On success 0 and the corresponding :c:type:`values.bits<gpio_v2_line_values>`
contain the value read.
(成功时返回 0，相应的 :c:type:`values.bits<gpio_v2_line_values>` 中包含
读取到的值。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
