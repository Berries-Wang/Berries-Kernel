.. SPDX-License-Identifier: GPL-2.0

=========================================================================
GPIO Character Device Userspace API (v1)(GPIO 字符设备用户空间 API（v1）)
=========================================================================

.. warning::
   This API is obsoleted by chardev.rst (v2).
   (此 API 已被 chardev.rst（v2）取代。)

   New developments should use the v2 API, and existing developments are
   encouraged to migrate as soon as possible, as this API will be removed
   in the future. The v2 API is a functional superset of the v1 API so any
   v1 call can be directly translated to a v2 equivalent.
   (新的开发应使用 v2 API，并鼓励现有开发尽快迁移，因为此 API 将来会被移除。v2 API 是 v1 API 的功能超集，因此任何 v1 调用都可以直接转换为对应的 v2 调用。)

   This interface will continue to be maintained for the migration period,
   but new features will only be added to the new API.
   (在迁移期间此接口将继续得到维护，但新特性只会添加到新 API 中。)

First added in 4.8.
(最早加入于 4.8。)

The API is based around three major objects, the :ref:`gpio-v1-chip`, the
:ref:`gpio-v1-line-handle`, and the :ref:`gpio-v1-line-event`.
(该 API 围绕三个主要对象构建：:ref:`gpio-v1-chip` 、 :ref:`gpio-v1-line-handle` 和 :ref:`gpio-v1-line-event` 。)

Where "line event" is used in this document it refers to the request that can
monitor a line for edge events, not the edge events themselves.
(本文中使用 "line event"（线路事件）时，指的是可以监控一条线路的边沿事件的请求，而不是边沿事件本身。)

.. _gpio-v1-chip:

Chip(芯片)
==========

The Chip represents a single GPIO chip and is exposed to userspace using device
files of the form ``/dev/gpiochipX``.
(Chip 表示单个 GPIO 芯片，通过形如 ``/dev/gpiochipX`` 的设备文件暴露给用户空间。)

Each chip supports a number of GPIO lines,
:c:type:`chip.lines<gpiochip_info>`. Lines on the chip are identified by an
``offset`` in the range from 0 to ``chip.lines - 1``, i.e. `[0,chip.lines)`.
(每个芯片支持若干条 GPIO 线路，即 :c:type:`chip.lines<gpiochip_info>` 。芯片上的线路通过 0 到 ``chip.lines - 1`` 范围内的 ``offset`` 来标识，即 `[0,chip.lines)` 。)

Lines are requested from the chip using either gpio-get-linehandle-ioctl.rst
and the resulting line handle is used to access the GPIO chip's lines, or
gpio-get-lineevent-ioctl.rst and the resulting line event is used to monitor
a GPIO line for edge events.
(向芯片请求线路时，要么使用 gpio-get-linehandle-ioctl.rst，其返回的线路句柄用于访问 GPIO 芯片的线路；要么使用 gpio-get-lineevent-ioctl.rst，其返回的线路事件用于监控 GPIO 线路的边沿事件。)

Within this documentation, the file descriptor returned by calling `open()`
on the GPIO device file is referred to as ``chip_fd``.
(在本文档中，在 GPIO 设备文件上调用 `open()` 所返回的文件描述符称为 ``chip_fd`` 。)

Operations(操作)
----------------

The following operations may be performed on the chip:
(可以对芯片执行以下操作：)

.. toctree::
   :titlesonly:

   Get Line Handle <gpio-get-linehandle-ioctl>
   Get Line Event <gpio-get-lineevent-ioctl>
   Get Chip Info <gpio-get-chipinfo-ioctl>
   Get Line Info <gpio-get-lineinfo-ioctl>
   Watch Line Info <gpio-get-lineinfo-watch-ioctl>
   Unwatch Line Info <gpio-get-lineinfo-unwatch-ioctl>
   Read Line Info Changed Events <gpio-lineinfo-changed-read>

.. _gpio-v1-line-handle:

Line Handle(线路句柄)
=====================

Line handles are created by gpio-get-linehandle-ioctl.rst and provide
access to a set of requested lines.  The line handle is exposed to userspace
via the anonymous file descriptor returned  in
:c:type:`request.fd<gpiohandle_request>` by gpio-get-linehandle-ioctl.rst.
(线路句柄由 gpio-get-linehandle-ioctl.rst 创建，提供对一组已请求线路的访问。线路句柄通过 gpio-get-linehandle-ioctl.rst 在 :c:type:`request.fd<gpiohandle_request>` 中返回的匿名文件描述符暴露给用户空间。)

Within this documentation, the line handle file descriptor is referred to
as ``handle_fd``.
(在本文档中，线路句柄文件描述符称为 ``handle_fd`` 。)

Operations(操作)
----------------

The following operations may be performed on the line handle:
(可以对线路句柄执行以下操作：)

.. toctree::
   :titlesonly:

   Get Line Values <gpio-handle-get-line-values-ioctl>
   Set Line Values <gpio-handle-set-line-values-ioctl>
   Reconfigure Lines <gpio-handle-set-config-ioctl>

.. _gpio-v1-line-event:

Line Event(线路事件)
====================

Line events are created by gpio-get-lineevent-ioctl.rst and provide
access to a requested line.  The line event is exposed to userspace
via the anonymous file descriptor returned  in
:c:type:`request.fd<gpioevent_request>` by gpio-get-lineevent-ioctl.rst.
(线路事件由 gpio-get-lineevent-ioctl.rst 创建，提供对一条已请求线路的访问。线路事件通过 gpio-get-lineevent-ioctl.rst 在 :c:type:`request.fd<gpioevent_request>` 中返回的匿名文件描述符暴露给用户空间。)

Within this documentation, the line event file descriptor is referred to
as ``event_fd``.
(在本文档中，线路事件文件描述符称为 ``event_fd`` 。)

Operations(操作)
----------------

The following operations may be performed on the line event:
(可以对线路事件执行以下操作：)

.. toctree::
   :titlesonly:

   Get Line Value <gpio-handle-get-line-values-ioctl>
   Read Line Edge Events <gpio-lineevent-data-read>

Types(类型)
===========

This section contains the structs that are referenced by the ABI v1.
(本节包含 ABI v1 所引用的结构体。)

The :c:type:`struct gpiochip_info<gpiochip_info>` is common to ABI v1 and v2.
(:c:type:`struct gpiochip_info<gpiochip_info>` 为 ABI v1 和 v2 所共用。)

.. kernel-doc:: include/uapi/linux/gpio.h
   :identifiers:
    gpioevent_data
    gpioevent_request
    gpiohandle_config
    gpiohandle_data
    gpiohandle_request
    gpioline_info
    gpioline_info_changed

.. toctree::
   :hidden:

   error-codes
