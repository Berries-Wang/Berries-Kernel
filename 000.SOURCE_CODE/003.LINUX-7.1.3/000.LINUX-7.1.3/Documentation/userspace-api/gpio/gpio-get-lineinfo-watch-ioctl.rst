.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_GET_LINEINFO_WATCH_IOCTL:

*****************************
GPIO_GET_LINEINFO_WATCH_IOCTL
*****************************

.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-get-lineinfo-watch-ioctl.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-get-lineinfo-watch-ioctl.rst
    取代。)

Name(名称)
==========

GPIO_GET_LINEINFO_WATCH_IOCTL - Enable watching a line for changes to its
request state and configuration information.
(GPIO_GET_LINEINFO_WATCH_IOCTL - 启用对某条线路的请求状态与配置信息
变化的监视。)

Synopsis(概要)
==============

.. c:macro:: GPIO_GET_LINEINFO_WATCH_IOCTL

``int ioctl(int chip_fd, GPIO_GET_LINEINFO_WATCH_IOCTL, struct gpioline_info *info)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备文件描述符。)

``info``
    The :c:type:`line_info<gpioline_info>` struct to be populated, with
    the ``offset`` set to indicate the line to watch
    (要被填充的 :c:type:`line_info<gpioline_info>` 结构，其 ``offset``
    被设置为指示要监视的线路)

Description(描述)
=================

Enable watching a line for changes to its request state and configuration
information. Changes to line info include a line being requested, released
or reconfigured.
(启用对某条线路的请求状态与配置信息变化的监视。线路信息的变化包括线
路被请求、被释放或被重新配置。)

.. note::
    Watching line info is not generally required, and would typically only be
    used by a system monitoring component.
    (通常并不需要监视线路信息，这一般只由系统监控组件使用。)

    The line info does NOT include the line value.
    (线路信息 *不* 包含线路的值。)

    The line must be requested using gpio-get-linehandle-ioctl.rst or
    gpio-get-lineevent-ioctl.rst to access its value, and the line event can
    monitor a line for events using gpio-lineevent-data-read.rst.
    (必须使用 gpio-get-linehandle-ioctl.rst 或 gpio-get-lineevent-ioctl.rst
    请求该线路才能访问其值；线路事件可以使用 gpio-lineevent-data-read.rst
    监视线路上的事件。)

By default all lines are unwatched when the GPIO chip is opened.
(默认情况下，GPIO 芯片被打开时所有线路都处于未监视状态。)

Multiple lines may be watched simultaneously by adding a watch for each.
(可以通过为每条线路添加监视来同时监视多条线路。)

Once a watch is set, any changes to line info will generate events which can be
read from the ``chip_fd`` as described in
gpio-lineinfo-changed-read.rst.
(一旦设置了监视，线路信息的任何变化都会产生事件，可以按
gpio-lineinfo-changed-read.rst 中所述从 ``chip_fd`` 读取。)

Adding a watch to a line that is already watched is an error (**EBUSY**).
(向一条已经被监视的线路添加监视是一个错误 (**EBUSY**)。)

Watches are specific to the ``chip_fd`` and are independent of watches
on the same GPIO chip opened with a separate call to `open()`.
(监视是特定于 ``chip_fd`` 的，与通过单独调用 `open()` 打开的同一 GPIO
芯片上的监视相互独立。)

First added in 5.7.
(首次添加于 5.7。)

Return Value(返回值)
====================

On success 0 and ``info`` is populated with the current line info.
(成功时返回 0，且 ``info`` 被填充为当前线路信息。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
