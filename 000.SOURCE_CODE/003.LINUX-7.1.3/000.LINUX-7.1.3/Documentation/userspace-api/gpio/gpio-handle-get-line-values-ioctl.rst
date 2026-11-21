.. SPDX-License-Identifier: GPL-2.0

.. _GPIOHANDLE_GET_LINE_VALUES_IOCTL:

****************************************************
GPIOHANDLE_GET_LINE_VALUES_IOCTL(获取已请求线路的值)
****************************************************
.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-line-get-values-ioctl.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-line-get-values-ioctl.rst
    取代。)

Name(名称)
==========

GPIOHANDLE_GET_LINE_VALUES_IOCTL - Get the values of all requested lines.(GPIOHANDLE_GET_LINE_VALUES_IOCTL - 获取所有已请求线路的值。)

Synopsis(概要)
==============

.. c:macro:: GPIOHANDLE_GET_LINE_VALUES_IOCTL

``int ioctl(int handle_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, struct gpiohandle_data *values)``

Arguments(参数)
===============

``handle_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpiohandle_request>` by gpio-get-linehandle-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-get-linehandle-ioctl.rst 中的
    :c:type:`request.fd<gpiohandle_request>` 返回。)

``values``
    The :c:type:`line_values<gpiohandle_data>` to be populated.
    (待填充的 :c:type:`line_values<gpiohandle_data>`。)

Description(描述)
=================

Get the values of all requested lines.
(获取所有已请求线路的值。)

The values returned are logical, indicating if the line is active or inactive.
The ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` flag controls the mapping between physical
values (high/low) and logical values (active/inactive).
If ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` is not set then high is active and
low is inactive. If ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` is set then low is active
and high is inactive.
(返回的值是逻辑值，表明线路处于活动还是非活动状态。
``GPIOHANDLE_REQUEST_ACTIVE_LOW`` 标志控制物理值 (高/低) 与逻辑值
(活动/非活动) 之间的映射。如果未设置 ``GPIOHANDLE_REQUEST_ACTIVE_LOW``，
则高电平为活动、低电平为非活动；如果设置了 ``GPIOHANDLE_REQUEST_ACTIVE_LOW``，
则低电平为活动、高电平为非活动。)

The values of both input and output lines may be read.
(输入和输出线路的值都可以读取。)

For output lines, the value returned is driver and configuration dependent and
may be either the output buffer (the last requested value set) or the input
buffer (the actual level of the line), and depending on the hardware and
configuration these may differ.
(对于输出线路，返回的值取决于驱动和配置，可能是输出缓冲区 (最后请求设置
的值) 或输入缓冲区 (线路的实际电平)，视硬件与配置不同，两者可能不同。)

This ioctl can also be used to read the line value for line events,
substituting the ``event_fd`` for the ``handle_fd``.  As there is only
one line requested in that case, only the one value is returned in ``values``.
(此 ioctl 也可用于读取线路事件的线路值，此时用 ``event_fd`` 代替
``handle_fd``。由于这种情况下只请求了一条线路，``values`` 中只返回这一个
值。)

Return Value(返回值)
====================

On success 0 and ``values`` populated with the values read.
(成功时返回 0，``values`` 中填充读取到的值。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
