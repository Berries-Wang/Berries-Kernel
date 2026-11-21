.. SPDX-License-Identifier: GPL-2.0

==============================================================
GPIO Character Device Userspace API(GPIO 字符设备用户空间 API)
==============================================================

This is latest version (v2) of the character device API, as defined in
``include/uapi/linux/gpio.h.``
(这是最新版本（v2）的字符设备 API，定义在 ``include/uapi/linux/gpio.h.`` 中。)

First added in 5.10.
(最早加入于 5.10。)

.. note::
   Do NOT abuse userspace APIs to control hardware that has proper kernel
   drivers. There may already be a driver for your use case, and an existing
   kernel driver is sure to provide a superior solution to bitbashing
   from userspace.
   (切勿滥用用户空间 API 来控制那些有合适内核驱动程序的硬件。你的使用场景可能已经有驱动程序了，而且现有的内核驱动程序必定能提供优于从用户空间进行位操纵的解决方案。)

   Read Documentation/driver-api/gpio/drivers-on-gpio.rst to avoid reinventing
   kernel wheels in userspace.
   (阅读 Documentation/driver-api/gpio/drivers-on-gpio.rst，避免在用户空间重复发明内核的轮子。)

   Similarly, for multi-function lines there may be other subsystems, such as
   Documentation/spi/index.rst, Documentation/i2c/index.rst,
   Documentation/driver-api/pwm.rst, Documentation/w1/index.rst etc, that
   provide suitable drivers and APIs for your hardware.
   (类似地，对于多功能线路，可能还有其他子系统（例如 Documentation/spi/index.rst 、 Documentation/i2c/index.rst 、 Documentation/driver-api/pwm.rst 、 Documentation/w1/index.rst 等）为你的硬件提供合适的驱动程序和 API。)

Basic examples using the character device API can be found in ``tools/gpio/*``.
(使用字符设备 API 的基本示例可以在 ``tools/gpio/*`` 中找到。)

The API is based around two major objects, the :ref:`gpio-v2-chip` and the
:ref:`gpio-v2-line-request`.
(该 API 围绕两个主要对象构建：:ref:`gpio-v2-chip` 和 :ref:`gpio-v2-line-request` 。)

.. _gpio-v2-chip:

Chip(芯片)
==========

The Chip represents a single GPIO chip and is exposed to userspace using device
files of the form ``/dev/gpiochipX``.
(Chip 表示单个 GPIO 芯片，通过形如 ``/dev/gpiochipX`` 的设备文件暴露给用户空间。)

Each chip supports a number of GPIO lines,
:c:type:`chip.lines<gpiochip_info>`. Lines on the chip are identified by an
``offset`` in the range from 0 to ``chip.lines - 1``, i.e. `[0,chip.lines)`.
(每个芯片支持若干条 GPIO 线路，即 :c:type:`chip.lines<gpiochip_info>` 。芯片上的线路通过 0 到 ``chip.lines - 1`` 范围内的 ``offset`` 来标识，即 `[0,chip.lines)` 。)

Lines are requested from the chip using gpio-v2-get-line-ioctl.rst
and the resulting line request is used to access the GPIO chip's lines or
monitor the lines for edge events.
(通过 gpio-v2-get-line-ioctl.rst 向芯片请求线路，返回的线路请求用于访问 GPIO 芯片的线路或监控线路的边沿事件。)

Within this documentation, the file descriptor returned by calling `open()`
on the GPIO device file is referred to as ``chip_fd``.
(在本文档中，在 GPIO 设备文件上调用 `open()` 所返回的文件描述符称为 ``chip_fd`` 。)

Operations(操作)
----------------

The following operations may be performed on the chip:
(可以对芯片执行以下操作：)

.. toctree::
   :titlesonly:

   Get Line <gpio-v2-get-line-ioctl>
   Get Chip Info <gpio-get-chipinfo-ioctl>
   Get Line Info <gpio-v2-get-lineinfo-ioctl>
   Watch Line Info <gpio-v2-get-lineinfo-watch-ioctl>
   Unwatch Line Info <gpio-get-lineinfo-unwatch-ioctl>
   Read Line Info Changed Events <gpio-v2-lineinfo-changed-read>

.. _gpio-v2-line-request:

Line Request(线路请求)
======================

Line requests are created by gpio-v2-get-line-ioctl.rst and provide
access to a set of requested lines.  The line request is exposed to userspace
via the anonymous file descriptor returned in
:c:type:`request.fd<gpio_v2_line_request>` by gpio-v2-get-line-ioctl.rst.
(线路请求由 gpio-v2-get-line-ioctl.rst 创建，提供对一组已请求线路的访问。线路请求通过 gpio-v2-get-line-ioctl.rst 在 :c:type:`request.fd<gpio_v2_line_request>` 中返回的匿名文件描述符暴露给用户空间。)

Within this documentation, the line request file descriptor is referred to
as ``req_fd``.
(在本文档中，线路请求文件描述符称为 ``req_fd`` 。)

Operations(操作)
----------------

The following operations may be performed on the line request:
(可以对线路请求执行以下操作：)

.. toctree::
   :titlesonly:

   Get Line Values <gpio-v2-line-get-values-ioctl>
   Set Line Values <gpio-v2-line-set-values-ioctl>
   Read Line Edge Events <gpio-v2-line-event-read>
   Reconfigure Lines <gpio-v2-line-set-config-ioctl>

Types(类型)
===========

This section contains the structs and enums that are referenced by the API v2,
as defined in ``include/uapi/linux/gpio.h``.
(本节包含 API v2 所引用的结构体和枚举，定义在 ``include/uapi/linux/gpio.h`` 中。)

.. kernel-doc:: include/uapi/linux/gpio.h
   :identifiers:
    gpio_v2_line_attr_id
    gpio_v2_line_attribute
    gpio_v2_line_changed_type
    gpio_v2_line_config
    gpio_v2_line_config_attribute
    gpio_v2_line_event
    gpio_v2_line_event_id
    gpio_v2_line_flag
    gpio_v2_line_info
    gpio_v2_line_info_changed
    gpio_v2_line_request
    gpio_v2_line_values
    gpiochip_info

.. toctree::
   :hidden:

   error-codes
