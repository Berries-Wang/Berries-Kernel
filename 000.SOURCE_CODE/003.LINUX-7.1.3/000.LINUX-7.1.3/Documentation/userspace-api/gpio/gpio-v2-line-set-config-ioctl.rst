.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_LINE_SET_CONFIG_IOCTL:

*******************************************
GPIO_V2_LINE_SET_CONFIG_IOCTL(更新线路配置)
*******************************************

Name(名称)
==========

GPIO_V2_LINE_SET_CONFIG_IOCTL - Update the configuration of previously requested lines.(GPIO_V2_LINE_SET_CONFIG_IOCTL - 更新先前请求线路的配置。)

Synopsis(概要)
==============

.. c:macro:: GPIO_V2_LINE_SET_CONFIG_IOCTL

``int ioctl(int req_fd, GPIO_V2_LINE_SET_CONFIG_IOCTL, struct gpio_v2_line_config *config)``

Arguments(参数)
===============

``req_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpio_v2_line_request>` by gpio-v2-get-line-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-v2-get-line-ioctl.rst 中的
    :c:type:`request.fd<gpio_v2_line_request>` 返回。)

``config``
    The new :c:type:`configuration<gpio_v2_line_config>` to apply to the
    requested lines.
    (要应用于所请求线路的新 :c:type:`configuration<gpio_v2_line_config>`
    配置。)

Description(描述)
=================

Update the configuration of previously requested lines, without releasing the
line or introducing potential glitches.
(更新先前请求线路的配置，而不释放线路或引入潜在的毛刺。)

The new configuration must specify a configuration for all requested lines.
(新配置必须为所有已请求的线路指定配置。)

The same :ref:`gpio-v2-get-line-config-rules` and
:ref:`gpio-v2-get-line-config-support` that apply when requesting the lines
also apply when updating the line configuration, with the additional
restriction that a direction flag must be set to enable reconfiguration.
If no direction flag is set in the configuration for a given line then the
configuration for that line is left unchanged.
(请求线路时适用的 :ref:`gpio-v2-get-line-config-rules` 和
:ref:`gpio-v2-get-line-config-support` 在更新线路配置时同样适用，另外还有
一个附加限制：必须设置方向标志才能启用重新配置。如果配置中某条线路没有
设置方向标志，则该线路的配置保持不变。)

The motivating use case for this command is changing direction of
bi-directional lines between input and output, but it may also be used to
dynamically control edge detection, or more generally move lines seamlessly
from one configuration state to another.
(此命令的典型用途是在输入与输出之间切换双向线路的方向，但它也可用于动态
控制边沿检测，或更一般地让线路从一个配置状态无缝迁移到另一个配置状态。)

To only change the value of output lines, use
gpio-v2-line-set-values-ioctl.rst.
(若只想改变输出线路的值，请使用 gpio-v2-line-set-values-ioctl.rst。)

Return Value(返回值)
====================

On success 0.
(成功时返回 0。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1，并相应设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
