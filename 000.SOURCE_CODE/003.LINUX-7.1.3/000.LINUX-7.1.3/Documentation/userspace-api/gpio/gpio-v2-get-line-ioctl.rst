.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_GET_LINE_IOCTL:

****************************
GPIO_V2_GET_LINE_IOCTL(原名)
****************************

Name(名称)
==========

GPIO_V2_GET_LINE_IOCTL - Request a line or lines from the kernel.
(GPIO_V2_GET_LINE_IOCTL - 向内核请求一条或多条线路。)

Synopsis(概要)
==============

.. c:macro:: GPIO_V2_GET_LINE_IOCTL

``int ioctl(int chip_fd, GPIO_V2_GET_LINE_IOCTL, struct gpio_v2_line_request *request)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备的文件描述符。)

``request``
    The :c:type:`line_request<gpio_v2_line_request>` specifying the lines
    to request and their configuration.
    (指定要请求的线路及其配置的
    :c:type:`line_request<gpio_v2_line_request>`。)

Description(描述)
=================

On success, the requesting process is granted exclusive access to the line
value, write access to the line configuration, and may receive events when
edges are detected on the line, all of which are described in more detail in
:ref:`gpio-v2-line-request`.
(成功时，请求进程获得对该线路值的独占访问权、对线路配置的写权限，并可以在
检测到线路边沿时接收事件。所有这些都在 :ref:`gpio-v2-line-request` 中有更
详细的描述。)

A number of lines may be requested in the one line request, and request
operations are performed on the requested lines by the kernel as atomically
as possible. e.g. gpio-v2-line-get-values-ioctl.rst will read all the
requested lines at once.
(可以在一次线路请求中请求多条线路，内核会尽可能以原子方式对所请求的线路
执行请求操作。例如 gpio-v2-line-get-values-ioctl.rst 会一次性读取所有被
请求的线路。)

The state of a line, including the value of output lines, is guaranteed to
remain as requested until the returned file descriptor is closed. Once the
file descriptor is closed, the state of the line becomes uncontrolled from
the userspace perspective, and may revert to its default state.
(线路的状态（包括输出线路的值）保证在返回的文件描述符关闭之前保持所请求的
状态。一旦文件描述符关闭，从用户空间的角度来看，线路状态就不再受控，并可能
恢复到其默认状态。)

Requesting a line already in use is an error (**EBUSY**).
(请求一条已被使用的线路是一个错误 (**EBUSY**)。)

Closing the ``chip_fd`` has no effect on existing line requests.
(关闭 ``chip_fd`` 对现有的线路请求没有影响。)

.. _gpio-v2-get-line-config-rules:

Configuration Rules(配置规则)
-----------------------------

For any given requested line, the following configuration rules apply:
(对于任何给定的被请求线路，适用以下配置规则：)

The direction flags, ``GPIO_V2_LINE_FLAG_INPUT`` and
``GPIO_V2_LINE_FLAG_OUTPUT``, cannot be combined. If neither are set then
the only other flag that may be set is ``GPIO_V2_LINE_FLAG_ACTIVE_LOW``
and the line is requested "as-is" to allow reading of the line value
without altering the electrical configuration.
(方向标志 ``GPIO_V2_LINE_FLAG_INPUT`` 与 ``GPIO_V2_LINE_FLAG_OUTPUT`` 不能
组合。如果两者都未设置，则唯一可以设置的其他标志是
``GPIO_V2_LINE_FLAG_ACTIVE_LOW``，且线路按 "as-is"（原样）请求，以便在不
改变电气配置的情况下读取线路值。)

The drive flags, ``GPIO_V2_LINE_FLAG_OPEN_xxx``, require the
``GPIO_V2_LINE_FLAG_OUTPUT`` to be set.
Only one drive flag may be set.
If none are set then the line is assumed push-pull.
(驱动 (drive) 标志 ``GPIO_V2_LINE_FLAG_OPEN_xxx`` 要求设置
``GPIO_V2_LINE_FLAG_OUTPUT``。只能设置一个驱动标志。如果一个都没有设置，
则线路被假定为推挽 (push-pull)。)

Only one bias flag, ``GPIO_V2_LINE_FLAG_BIAS_xxx``, may be set, and it
requires a direction flag to also be set.
If no bias flags are set then the bias configuration is not changed.
(只能设置一个偏置 (bias) 标志 ``GPIO_V2_LINE_FLAG_BIAS_xxx``，并且它要求
同时设置一个方向标志。如果没有设置偏置标志，则偏置配置保持不变。)

The edge flags, ``GPIO_V2_LINE_FLAG_EDGE_xxx``, require
``GPIO_V2_LINE_FLAG_INPUT`` to be set and may be combined to detect both rising
and falling edges.  Requesting edge detection from a line that does not support
it is an error (**ENXIO**).
(边沿 (edge) 标志 ``GPIO_V2_LINE_FLAG_EDGE_xxx`` 要求设置
``GPIO_V2_LINE_FLAG_INPUT``，并且可以组合起来同时检测上升沿和下降沿。从
不支持边沿检测的线路请求边沿检测是一个错误 (**ENXIO**)。)

Only one event clock flag, ``GPIO_V2_LINE_FLAG_EVENT_CLOCK_xxx``, may be set.
If none are set then the event clock defaults to ``CLOCK_MONOTONIC``.
The ``GPIO_V2_LINE_FLAG_EVENT_CLOCK_HTE`` flag requires supporting hardware
and a kernel with ``CONFIG_HTE`` set.  Requesting HTE from a device that
doesn't support it is an error (**EOPNOTSUPP**).
(只能设置一个事件时钟标志 ``GPIO_V2_LINE_FLAG_EVENT_CLOCK_xxx``。如果一个
都没有设置，则事件时钟默认为 ``CLOCK_MONOTONIC``。
``GPIO_V2_LINE_FLAG_EVENT_CLOCK_HTE`` 标志需要受支持的硬件以及设置了
``CONFIG_HTE`` 的内核。从不支持 HTE 的设备请求 HTE 是一个错误
(**EOPNOTSUPP**)。)

The :c:type:`debounce_period_us<gpio_v2_line_attribute>` attribute may only
be applied to lines with ``GPIO_V2_LINE_FLAG_INPUT`` set. When set, debounce
applies to both the values returned by gpio-v2-line-get-values-ioctl.rst and
the edges returned by gpio-v2-line-event-read.rst.  If not
supported directly by hardware, debouncing is emulated in software by the
kernel.  Requesting debounce on a line that supports neither debounce in
hardware nor interrupts, as required for software emulation, is an error
(**ENXIO**).
(:c:type:`debounce_period_us<gpio_v2_line_attribute>` 属性只能应用于设置了
``GPIO_V2_LINE_FLAG_INPUT`` 的线路。设置后，去抖动 (debounce) 既适用于
gpio-v2-line-get-values-ioctl.rst 返回的值，也适用于
gpio-v2-line-event-read.rst 返回的边沿。如果硬件不直接支持，去抖动由内核在
软件中模拟。在不支持硬件去抖动、也不支持软件模拟所需的硬件中断的线路上
请求去抖动是一个错误 (**ENXIO**)。)

Requesting an invalid configuration is an error (**EINVAL**).
(请求无效的配置是一个错误 (**EINVAL**)。)

.. _gpio-v2-get-line-config-support:

Configuration Support(配置支持)
-------------------------------

Where the requested configuration is not directly supported by the underlying
hardware and driver, the kernel applies one of these approaches:
(当底层硬件和驱动不能直接支持所请求的配置时，内核会采用以下方法之一：)

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
(采用哪种方法取决于该特性能否在软件中合理地模拟，以及不支持该特性时对硬件
和用户空间的影响。每种特性采用的方法如下：)

==============   ===========
Feature          Approach
特性             方法
==============   ===========
Bias             best effort (尽力而为)
Debounce         emulate (软件模拟)
Direction        reject (拒绝)
Drive            emulate (软件模拟)
Edge Detection   reject (拒绝)
==============   ===========

Bias is treated as best effort to allow userspace to apply the same
configuration for platforms that support internal bias as those that require
external bias.
Worst case the line floats rather than being biased as expected.
(偏置 (Bias) 被视为尽力而为，以便用户空间可以为支持内部偏置的平台和需要
外部偏置的平台应用相同的配置。最坏的情况是线路悬空，而不是按预期被
偏置。)

Debounce is emulated by applying a filter to hardware interrupts on the line.
An edge event is generated after an edge is detected and the line remains
stable for the debounce period.
The event timestamp corresponds to the end of the debounce period.
(去抖动 (Debounce) 通过对线路的硬件中断应用过滤器来模拟。在检测到边沿且
线路在去抖动期间保持稳定之后，生成边沿事件。事件时间戳对应去抖动周期的
结束时刻。)

Drive is emulated by switching the line to an input when the line should not
be actively driven.
(驱动 (Drive) 的模拟方式是：在线路不应被主动驱动时，把线路切换为输入。)

Edge detection requires interrupt support, and is rejected if that is not
supported. Emulation by polling can still be performed from userspace.
(边沿检测需要中断支持，如果不支持则被拒绝。用户空间仍然可以通过轮询进行
模拟。)

In all cases, the configuration reported by gpio-v2-get-lineinfo-ioctl.rst
is the requested configuration, not the resulting hardware configuration.
Userspace cannot determine if a feature is supported in hardware, is
emulated, or is best effort.
(在所有情况下，gpio-v2-get-lineinfo-ioctl.rst 报告的配置都是所请求的配置，
而不是最终的硬件配置。用户空间无法判断某个特性是硬件支持、软件模拟还是
尽力而为。)

Return Value(返回值)
====================

On success 0 and the :c:type:`request.fd<gpio_v2_line_request>` contains the
file descriptor for the request.
(成功时返回 0，并且 :c:type:`request.fd<gpio_v2_line_request>` 包含该请求
的文件描述符。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并适当设置 ``errno`` 变量。常见错误码在 error-codes.rst 中
描述。)
