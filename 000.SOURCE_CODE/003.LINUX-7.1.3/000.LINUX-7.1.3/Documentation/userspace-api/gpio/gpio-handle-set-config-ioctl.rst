.. SPDX-License-Identifier: GPL-2.0

.. _GPIOHANDLE_SET_CONFIG_IOCTL:

*****************************************
GPIOHANDLE_SET_CONFIG_IOCTL(更新线路配置)
*****************************************

.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-line-set-config-ioctl.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-line-set-config-ioctl.rst
    取代。)

Name(名称)
==========

GPIOHANDLE_SET_CONFIG_IOCTL - Update the configuration of previously requested lines.(GPIOHANDLE_SET_CONFIG_IOCTL - 更新先前请求线路的配置。)

Synopsis(概要)
==============

.. c:macro:: GPIOHANDLE_SET_CONFIG_IOCTL

``int ioctl(int handle_fd, GPIOHANDLE_SET_CONFIG_IOCTL, struct gpiohandle_config *config)``

Arguments(参数)
===============

``handle_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpiohandle_request>` by gpio-get-linehandle-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-get-linehandle-ioctl.rst 中的
    :c:type:`request.fd<gpiohandle_request>` 返回。)

``config``
    The new :c:type:`configuration<gpiohandle_config>` to apply to the
    requested lines.
    (要应用于所请求线路的新 :c:type:`configuration<gpiohandle_config>` 配置。)

Description(描述)
=================

Update the configuration of previously requested lines, without releasing the
line or introducing potential glitches.
(更新先前请求线路的配置，而不释放线路或引入潜在的毛刺。)

The configuration applies to all requested lines.
(该配置适用于所有已请求的线路。)

The same :ref:`gpio-get-linehandle-config-rules` and
:ref:`gpio-get-linehandle-config-support` that apply when requesting the
lines also apply when updating the line configuration, with the additional
restriction that a direction flag must be set. Requesting an invalid
configuration, including without a direction flag set, is an error
(**EINVAL**).
(请求线路时适用的 :ref:`gpio-get-linehandle-config-rules` 和
:ref:`gpio-get-linehandle-config-support` 在更新线路配置时同样适用，另外还有
一个附加限制：必须设置方向标志。请求无效配置 (包括未设置方向标志) 会报错误
(**EINVAL**)。)

The motivating use case for this command is changing direction of
bi-directional lines between input and output, but it may be used more
generally to move lines seamlessly from one configuration state to another.
(此命令的典型用途是在输入与输出之间切换双向线路的方向，但它也可以更广泛地
用于让线路从一个配置状态无缝迁移到另一个配置状态。)

To only change the value of output lines, use
gpio-handle-set-line-values-ioctl.rst.
(若只想改变输出线路的值，请使用 gpio-handle-set-line-values-ioctl.rst。)

First added in 5.5.
(最早添加于 5.5。)

Return Value(返回值)
====================

On success 0.
(成功时返回 0。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并将 ``errno`` 变量设置为相应值。常见错误码在 error-codes.rst
中描述。)
