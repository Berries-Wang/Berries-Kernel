.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_HANDLE_SET_LINE_VALUES_IOCTL:

*********************************************************
GPIO_HANDLE_SET_LINE_VALUES_IOCTL(设置已请求输出线路的值)
*********************************************************
.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-line-set-values-ioctl.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-line-set-values-ioctl.rst
    取代。)

Name(名称)
==========

GPIO_HANDLE_SET_LINE_VALUES_IOCTL - Set the values of all requested output lines.(GPIO_HANDLE_SET_LINE_VALUES_IOCTL - 设置所有已请求输出线路的值。)

Synopsis(概要)
==============

.. c:macro:: GPIO_HANDLE_SET_LINE_VALUES_IOCTL

``int ioctl(int handle_fd, GPIO_HANDLE_SET_LINE_VALUES_IOCTL, struct gpiohandle_data *values)``

Arguments(参数)
===============

``handle_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpiohandle_request>` by gpio-get-linehandle-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-get-linehandle-ioctl.rst 中的
    :c:type:`request.fd<gpiohandle_request>` 返回。)

``values``
    The :c:type:`line_values<gpiohandle_data>` to set.
    (要设置的 :c:type:`line_values<gpiohandle_data>`。)

Description(描述)
=================

Set the values of all requested output lines.
(设置所有已请求输出线路的值。)

The values set are logical, indicating if the line is to be active or inactive.
The ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` flag controls the mapping between logical
values (active/inactive) and physical values (high/low).
If  ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` is not set then active is high and
inactive is low. If ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` is set then active is low
and inactive is high.
(设置的值是逻辑值，表明线路应为活动还是非活动状态。
``GPIOHANDLE_REQUEST_ACTIVE_LOW`` 标志控制逻辑值 (活动/非活动) 与物理值
(高/低) 之间的映射。如果未设置 ``GPIOHANDLE_REQUEST_ACTIVE_LOW``，则活动为
高电平、非活动为低电平；如果设置了 ``GPIOHANDLE_REQUEST_ACTIVE_LOW``，则
活动为低电平、非活动为高电平。)

Only the values of output lines may be set.
Attempting to set the value of input lines is an error (**EPERM**).
(只能设置输出线路的值。试图设置输入线路的值是一个错误 (**EPERM**)。)

Return Value(返回值)
====================

On success 0.
(成功时返回 0。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
