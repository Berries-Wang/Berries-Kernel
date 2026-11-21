.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_GET_LINEEVENT_IOCTL:

************************
GPIO_GET_LINEEVENT_IOCTL
************************

.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-get-line-ioctl.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-get-line-ioctl.rst 取代。)

Name(名称)
==========

GPIO_GET_LINEEVENT_IOCTL - Request a line with edge detection from the kernel.
(GPIO_GET_LINEEVENT_IOCTL - 向内核请求一条带边沿检测的线路。)

Synopsis(概要)
==============

.. c:macro:: GPIO_GET_LINEEVENT_IOCTL

``int ioctl(int chip_fd, GPIO_GET_LINEEVENT_IOCTL, struct gpioevent_request *request)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备文件描述符。)

``request``
    The :c:type:`event_request<gpioevent_request>` specifying the line
    to request and its configuration.
    (该 :c:type:`event_request<gpioevent_request>` 指定要请求的线路及
    其配置。)

Description(描述)
=================

Request a line with edge detection from the kernel.
(向内核请求一条带边沿检测的线路。)

On success, the requesting process is granted exclusive access to the line
value and may receive events when edges are detected on the line, as
described in gpio-lineevent-data-read.rst.
(成功时，请求进程获得对该线路值的独占访问权，并且当在线路上检测到边
沿时可以接收事件，如 gpio-lineevent-data-read.rst 中所述。)

The state of a line is guaranteed to remain as requested until the returned
file descriptor is closed. Once the file descriptor is closed, the state of
the line becomes uncontrolled from the userspace perspective, and may revert
to its default state.
(在返回的文件描述符被关闭之前，线路状态保证保持为所请求的状态。一旦
文件描述符被关闭，从用户空间的角度看，线路状态就不再受控，并可能恢
复为其默认状态。)

Requesting a line already in use is an error (**EBUSY**).
(请求一条已在使用中的线路是一个错误 (**EBUSY**)。)

Requesting edge detection on a line that does not support interrupts is an
error (**ENXIO**).
(在不支持中断的线路上请求边沿检测是一个错误 (**ENXIO**)。)

As with the :ref:`line handle<gpio-get-linehandle-config-support>`, the
bias configuration is best effort.
(与 :ref:`line handle<gpio-get-linehandle-config-support>` 一样，偏置
(bias) 配置是尽力而为的。)

Closing the ``chip_fd`` has no effect on existing line events.
(关闭 ``chip_fd`` 对已存在的线路事件没有影响。)

Configuration Rules(配置规则)
-----------------------------

The following configuration rules apply:
(适用以下配置规则：)

The line event is requested as an input, so no flags specific to output lines,
``GPIOHANDLE_REQUEST_OUTPUT``, ``GPIOHANDLE_REQUEST_OPEN_DRAIN``, or
``GPIOHANDLE_REQUEST_OPEN_SOURCE``, may be set.
(线路事件被请求为输入，因此不得设置特定于输出线路的标志
``GPIOHANDLE_REQUEST_OUTPUT`` 、 ``GPIOHANDLE_REQUEST_OPEN_DRAIN`` 或
``GPIOHANDLE_REQUEST_OPEN_SOURCE`` 。)

Only one bias flag, ``GPIOHANDLE_REQUEST_BIAS_xxx``, may be set.
If no bias flags are set then the bias configuration is not changed.
(只能设置一个偏置标志 ``GPIOHANDLE_REQUEST_BIAS_xxx`` 。如果没有设置
任何偏置标志，则偏置配置不会改变。)

The edge flags, ``GPIOEVENT_REQUEST_RISING_EDGE`` and
``GPIOEVENT_REQUEST_FALLING_EDGE``, may be combined to detect both rising
and falling edges.
(边沿标志 ``GPIOEVENT_REQUEST_RISING_EDGE`` 与
``GPIOEVENT_REQUEST_FALLING_EDGE`` 可以组合起来，以同时检测上升沿和
下降沿。)

Requesting an invalid configuration is an error (**EINVAL**).
(请求无效的配置是一个错误 (**EINVAL**)。)

Return Value(返回值)
====================

On success 0 and the :c:type:`request.fd<gpioevent_request>` contains the file
descriptor for the request.
(成功时返回 0，且 :c:type:`request.fd<gpioevent_request>` 包含该请求
的文件描述符。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在
error-codes.rst 中描述。)
