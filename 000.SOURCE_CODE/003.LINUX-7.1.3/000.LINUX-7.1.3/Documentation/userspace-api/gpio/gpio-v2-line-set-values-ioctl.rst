.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_LINE_SET_VALUES_IOCTL:

*****************************************************
GPIO_V2_LINE_SET_VALUES_IOCTL(设置已请求输出线路的值)
*****************************************************

Name(名称)
==========

GPIO_V2_LINE_SET_VALUES_IOCTL - Set the values of requested output lines.(GPIO_V2_LINE_SET_VALUES_IOCTL - 设置已请求输出线路的值。)

Synopsis(概要)
==============

.. c:macro:: GPIO_V2_LINE_SET_VALUES_IOCTL

``int ioctl(int req_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, struct gpio_v2_line_values *values)``

Arguments(参数)
===============

``req_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpio_v2_line_request>` by gpio-v2-get-line-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-v2-get-line-ioctl.rst 中的
    :c:type:`request.fd<gpio_v2_line_request>` 返回。)

``values``
    The :c:type:`line_values<gpio_v2_line_values>` to set with the ``mask`` set
    to indicate the subset of requested lines to set and ``bits`` set to
    indicate the new value.
    (要设置的 :c:type:`line_values<gpio_v2_line_values>`，其 ``mask`` 应设置
    为指示要设置的已请求线路的子集，``bits`` 应设置为指示新值。)

Description(描述)
=================

Set the values of requested output lines.
(设置已请求输出线路的值。)

The values set are logical, indicating if the line is to be active or inactive.
The ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` flag controls the mapping between logical
values (active/inactive) and physical values (high/low).
If ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` is not set then active is high and inactive
is low.  If ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` is set then active is low and
inactive is high.
(设置的值是逻辑值，表明线路应为活动还是非活动状态。
``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` 标志控制逻辑值 (活动/非活动) 与物理值
(高/低) 之间的映射。如果未设置 ``GPIO_V2_LINE_FLAG_ACTIVE_LOW``，则活动为
高电平、非活动为低电平；如果设置了 ``GPIO_V2_LINE_FLAG_ACTIVE_LOW``，则
活动为低电平、非活动为高电平。)

Only the values of output lines may be set.
Attempting to set the value of an input line is an error (**EPERM**).
(只能设置输出线路的值。试图设置输入线路的值是一个错误 (**EPERM**)。)

Return Value(返回值)
====================

On success 0.
(成功时返回 0。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
