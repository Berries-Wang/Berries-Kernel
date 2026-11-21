.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_GET_LINEHANDLE_IOCTL:

*************************
GPIO_GET_LINEHANDLE_IOCTL
*************************

.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-get-line-ioctl.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-get-line-ioctl.rst 取代。)

Name(名称)
==========

GPIO_GET_LINEHANDLE_IOCTL - Request a line or lines from the kernel.
(GPIO_GET_LINEHANDLE_IOCTL - 向内核请求一条或多条线路。)

Synopsis(概要)
==============

.. c:macro:: GPIO_GET_LINEHANDLE_IOCTL

``int ioctl(int chip_fd, GPIO_GET_LINEHANDLE_IOCTL, struct gpiohandle_request *request)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备的文件描述符。)

``request``
    The :c:type:`handle_request<gpiohandle_request>` specifying the lines to
    request and their configuration.
    (:c:type:`handle_request<gpiohandle_request>` 指定要请求的线路及其配置。)

Description(描述)
=================

Request a line or lines from the kernel.
(向内核请求一条或多条线路。)

While multiple lines may be requested, the same configuration applies to all
lines in the request.
(虽然可以请求多条线路，但同一配置会应用于请求中的所有线路。)

On success, the requesting process is granted exclusive access to the line
value and write access to the line configuration.
(成功时，请求进程获得对线路值的独占访问权以及对线路配置的写访问权。)

The state of a line, including the value of output lines, is guaranteed to
remain as requested until the returned file descriptor is closed. Once the
file descriptor is closed, the state of the line becomes uncontrolled from
the userspace perspective, and may revert to its default state.
(线路的状态（包括输出线路的值）保证保持为所请求的状态，直到返回的文件描述符被关闭。文件描述符一旦关闭，从用户空间的角度来看，线路的状态就不再受控，并且可能恢复为其默认状态。)

Requesting a line already in use is an error (**EBUSY**).
(请求一条已被占用的线路是一个错误（**EBUSY**）。)

Closing the ``chip_fd`` has no effect on existing line handles.
(关闭 ``chip_fd`` 对现有的线路句柄没有影响。)

.. _gpio-get-linehandle-config-rules:

Configuration Rules(配置规则)
-----------------------------

The following configuration rules apply:
(适用以下配置规则：)

The direction flags, ``GPIOHANDLE_REQUEST_INPUT`` and
``GPIOHANDLE_REQUEST_OUTPUT``, cannot be combined. If neither are set then the
only other flag that may be set is ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` and the
line is requested "as-is" to allow reading of the line value without altering
the electrical configuration.
(方向标志 ``GPIOHANDLE_REQUEST_INPUT`` 和 ``GPIOHANDLE_REQUEST_OUTPUT`` 不能组合。如果两者都未设置，那么唯一可以设置的其他标志是 ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` ，并且线路按"原样"请求，以便在不改变电气配置的情况下读取线路值。)

The drive flags, ``GPIOHANDLE_REQUEST_OPEN_xxx``, require the
``GPIOHANDLE_REQUEST_OUTPUT`` to be set.
Only one drive flag may be set.
If none are set then the line is assumed push-pull.
(驱动器标志 ``GPIOHANDLE_REQUEST_OPEN_xxx`` 要求设置 ``GPIOHANDLE_REQUEST_OUTPUT`` 。只能设置一个驱动器标志。如果一个都未设置，则假定线路为推挽输出。)

Only one bias flag, ``GPIOHANDLE_REQUEST_BIAS_xxx``, may be set, and
it requires a direction flag to also be set.
If no bias flags are set then the bias configuration is not changed.
(只能设置一个偏置标志 ``GPIOHANDLE_REQUEST_BIAS_xxx`` ，并且它还要求设置一个方向标志。如果未设置任何偏置标志，则偏置配置不变。)

Requesting an invalid configuration is an error (**EINVAL**).
(请求无效的配置是一个错误（**EINVAL**）。)


.. _gpio-get-linehandle-config-support:

Configuration Support(配置支持)
-------------------------------

Where the requested configuration is not directly supported by the underlying
hardware and driver, the kernel applies one of these approaches:
(当底层硬件和驱动程序不直接支持所请求的配置时，内核会采用以下方式之一：)

 - reject the request
   (拒绝该请求)
 - emulate the feature in software
   (在软件中模拟该特性)
 - treat the feature as best effort
   (将该特性视为尽力而为)

The approach applied depends on whether the feature can reasonably be emulated
in software, and the impact on the hardware and userspace if the feature is not
supported.
The approach applied for each feature is as follows:
(采用哪种方式取决于该特性是否可以合理地在软件中模拟，以及在不支持该特性的情况下对硬件和用户空间的影响。每种特性采用的方式如下：)

==============   ===========
Feature          Approach
==============   ===========
Bias             best effort
Direction        reject
Drive            emulate
==============   ===========

(特性              处理方式：偏置为尽力而为、方向为拒绝、驱动为软件模拟。)

Bias is treated as best effort to allow userspace to apply the same
configuration for platforms that support internal bias as those that require
external bias.
Worst case the line floats rather than being biased as expected.
(偏置被视为尽力而为，以便让用户空间在支持内部偏置的平台上与需要外部偏置的平台上应用相同的配置。最坏的情况是线路浮空而不是按预期偏置。)

Drive is emulated by switching the line to an input when the line should not
be driven.
(驱动器通过在线路不应被驱动时将其切换为输入来模拟。)

In all cases, the configuration reported by gpio-get-lineinfo-ioctl.rst
is the requested configuration, not the resulting hardware configuration.
Userspace cannot determine if a feature is supported in hardware, is
emulated, or is best effort.
(在所有情况下，gpio-get-lineinfo-ioctl.rst 报告的配置都是所请求的配置，而不是最终的硬件配置。用户空间无法判断某特性是硬件支持的、模拟的还是尽力而为的。)

Return Value(返回值)
====================

On success 0 and the :c:type:`request.fd<gpiohandle_request>` contains the
file descriptor for the request.
(成功时返回 0，并且 :c:type:`request.fd<gpiohandle_request>` 包含该请求的文件描述符。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在 error-codes.rst 中描述。)
