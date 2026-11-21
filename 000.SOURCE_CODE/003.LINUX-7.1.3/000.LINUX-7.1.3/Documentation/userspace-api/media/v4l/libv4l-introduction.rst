.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _libv4l-introduction:

******************
Introduction(引言)
******************

libv4l is a collection of libraries which adds a thin abstraction layer
on top of video4linux2 devices. The purpose of this (thin) layer is to
make it easy for application writers to support a wide variety of
devices without having to write separate code for different devices in
the same class.
(libv4l 是一组库的集合，它在 video4linux2 设备之上添加了一个薄抽象层。这个(薄)层的目的是让应用编写者可以轻松支持各种各样的设备，而不必为同一类别的不同设备分别编写代码。)

An example of using libv4l is provided by
:ref:`v4l2grab <v4l2grab-example>`.
(:ref:`v4l2grab <v4l2grab-example>` 提供了一个使用 libv4l 的示例。)

libv4l consists of 3 different libraries:
(libv4l 由 3 个不同的库组成：)

libv4lconvert(转换库)
=====================

libv4lconvert is a library that converts several different pixelformats
found in V4L2 drivers into a few common RGB and YUY formats.
(libv4lconvert 是一个库，它把 V4L2 驱动中出现的多种不同像素格式转换为少数几种常见的 RGB 与 YUY 格式。)

It currently accepts the following V4L2 driver formats:
:ref:`V4L2_PIX_FMT_BGR24 <V4L2-PIX-FMT-BGR24>`,
:ref:`V4L2_PIX_FMT_NV12_16L16 <V4L2-PIX-FMT-NV12-16L16>`,
:ref:`V4L2_PIX_FMT_JPEG <V4L2-PIX-FMT-JPEG>`,
:ref:`V4L2_PIX_FMT_MJPEG <V4L2-PIX-FMT-MJPEG>`,
:ref:`V4L2_PIX_FMT_MR97310A <V4L2-PIX-FMT-MR97310A>`,
:ref:`V4L2_PIX_FMT_OV511 <V4L2-PIX-FMT-OV511>`,
:ref:`V4L2_PIX_FMT_OV518 <V4L2-PIX-FMT-OV518>`,
:ref:`V4L2_PIX_FMT_PAC207 <V4L2-PIX-FMT-PAC207>`,
:ref:`V4L2_PIX_FMT_PJPG <V4L2-PIX-FMT-PJPG>`,
:ref:`V4L2_PIX_FMT_RGB24 <V4L2-PIX-FMT-RGB24>`,
:ref:`V4L2_PIX_FMT_SBGGR8 <V4L2-PIX-FMT-SBGGR8>`,
:ref:`V4L2_PIX_FMT_SGBRG8 <V4L2-PIX-FMT-SGBRG8>`,
:ref:`V4L2_PIX_FMT_SGRBG8 <V4L2-PIX-FMT-SGRBG8>`,
:ref:`V4L2_PIX_FMT_SN9C10X <V4L2-PIX-FMT-SN9C10X>`,
:ref:`V4L2_PIX_FMT_SN9C20X_I420 <V4L2-PIX-FMT-SN9C20X-I420>`,
:ref:`V4L2_PIX_FMT_SPCA501 <V4L2-PIX-FMT-SPCA501>`,
:ref:`V4L2_PIX_FMT_SPCA505 <V4L2-PIX-FMT-SPCA505>`,
:ref:`V4L2_PIX_FMT_SPCA508 <V4L2-PIX-FMT-SPCA508>`,
:ref:`V4L2_PIX_FMT_SPCA561 <V4L2-PIX-FMT-SPCA561>`,
:ref:`V4L2_PIX_FMT_SQ905C <V4L2-PIX-FMT-SQ905C>`,
:ref:`V4L2_PIX_FMT_SRGGB8 <V4L2-PIX-FMT-SRGGB8>`,
:ref:`V4L2_PIX_FMT_UYVY <V4L2-PIX-FMT-UYVY>`,
:ref:`V4L2_PIX_FMT_YUV420 <V4L2-PIX-FMT-YUV420>`,
:ref:`V4L2_PIX_FMT_YUYV <V4L2-PIX-FMT-YUYV>`,
:ref:`V4L2_PIX_FMT_YVU420 <V4L2-PIX-FMT-YVU420>`, and
:ref:`V4L2_PIX_FMT_YVYU <V4L2-PIX-FMT-YVYU>`.
(它目前接受以下 V4L2 驱动格式：V4L2_PIX_FMT_BGR24、V4L2_PIX_FMT_NV12_16L16、V4L2_PIX_FMT_JPEG、V4L2_PIX_FMT_MJPEG、
V4L2_PIX_FMT_MR97310A、V4L2_PIX_FMT_OV511、V4L2_PIX_FMT_OV518、V4L2_PIX_FMT_PAC207、V4L2_PIX_FMT_PJPG、
V4L2_PIX_FMT_RGB24、V4L2_PIX_FMT_SBGGR8、V4L2_PIX_FMT_SGBRG8、V4L2_PIX_FMT_SGRBG8、V4L2_PIX_FMT_SN9C10X、
V4L2_PIX_FMT_SN9C20X_I420、V4L2_PIX_FMT_SPCA501、V4L2_PIX_FMT_SPCA505、V4L2_PIX_FMT_SPCA508、V4L2_PIX_FMT_SPCA561、
V4L2_PIX_FMT_SQ905C、V4L2_PIX_FMT_SRGGB8、V4L2_PIX_FMT_UYVY、V4L2_PIX_FMT_YUV420、V4L2_PIX_FMT_YUYV、
V4L2_PIX_FMT_YVU420，以及 V4L2_PIX_FMT_YVYU。)

Later on libv4lconvert was expanded to also be able to do various video
processing functions to improve webcam video quality. The video
processing is split in to 2 parts: libv4lconvert/control and
libv4lconvert/processing.
(后来 libv4lconvert 得到扩展，还能执行各种视频处理功能，以提升摄像头视频质量。视频处理分为两部分：libv4lconvert/control 与 libv4lconvert/processing。)

The control part is used to offer video controls which can be used to
control the video processing functions made available by
libv4lconvert/processing. These controls are stored application wide
(until reboot) by using a persistent shared memory object.
(control 部分用于提供视频控制项，可用来控制 libv4lconvert/processing 所提供的视频处理功能。这些控制项通过一个持久化的共享内存对象按应用全局存储(直到重启)。)

libv4lconvert/processing offers the actual video processing
functionality.
(libv4lconvert/processing 提供实际的视频处理功能。)

libv4l1(兼容库)
===============

This library offers functions that can be used to quickly make v4l1
applications work with v4l2 devices. These functions work exactly like
the normal open/close/etc, except that libv4l1 does full emulation of
the v4l1 api on top of v4l2 drivers, in case of v4l1 drivers it will
just pass calls through.
(该库提供一些函数，可让 v4l1 应用快速适配 v4l2 设备。这些函数的工作方式与普通的 open/close/etc 完全一样，区别在于：对 v4l2 驱动，libv4l1 会在其上完整模拟 v4l1 API；而对 v4l1 驱动，它只是直接透传调用。)

Since those functions are emulations of the old V4L1 API, it shouldn't
be used for new applications.
(由于这些函数是对旧 V4L1 API 的模拟，不应在新应用中使用。)

libv4l2(主库)
=============

This library should be used for all modern V4L2 applications.
(所有现代 V4L2 应用都应使用该库。)

It provides handles to call V4L2 open/ioctl/close/poll methods. Instead
of just providing the raw output of the device, it enhances the calls in
the sense that it will use libv4lconvert to provide more video formats
and to enhance the image quality.
(它提供调用 V4L2 open/ioctl/close/poll 方法的句柄。它并非只提供设备的原始输出，而是增强这些调用：利用 libv4lconvert 提供更多视频格式并提升图像质量。)

In most cases, libv4l2 just passes the calls directly through to the
v4l2 driver, intercepting the calls to
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`,
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`,
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`,
:ref:`VIDIOC_ENUM_FRAMESIZES <VIDIOC_ENUM_FRAMESIZES>` and
:ref:`VIDIOC_ENUM_FRAMEINTERVALS <VIDIOC_ENUM_FRAMEINTERVALS>` in
order to emulate the formats
:ref:`V4L2_PIX_FMT_BGR24 <V4L2-PIX-FMT-BGR24>`,
:ref:`V4L2_PIX_FMT_RGB24 <V4L2-PIX-FMT-RGB24>`,
:ref:`V4L2_PIX_FMT_YUV420 <V4L2-PIX-FMT-YUV420>`, and
:ref:`V4L2_PIX_FMT_YVU420 <V4L2-PIX-FMT-YVU420>`, if they aren't
available in the driver. :ref:`VIDIOC_ENUM_FMT <VIDIOC_ENUM_FMT>`
keeps enumerating the hardware supported formats, plus the emulated
formats offered by libv4l at the end.
(在大多数情况下，libv4l2 只是把调用直接透传给 v4l2 驱动，仅拦截对 :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`、:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`、:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`、:ref:`VIDIOC_ENUM_FRAMESIZES <VIDIOC_ENUM_FRAMESIZES>` 与 :ref:`VIDIOC_ENUM_FRAMEINTERVALS <VIDIOC_ENUM_FRAMEINTERVALS>` 的调用，以便在驱动不支持这些格式时模拟 :ref:`V4L2_PIX_FMT_BGR24 <V4L2-PIX-FMT-BGR24>`、:ref:`V4L2_PIX_FMT_RGB24 <V4L2-PIX-FMT-RGB24>`、:ref:`V4L2_PIX_FMT_YUV420 <V4L2-PIX-FMT-YUV420>` 与 :ref:`V4L2_PIX_FMT_YVU420 <V4L2-PIX-FMT-YVU420>`。:ref:`VIDIOC_ENUM_FMT <VIDIOC_ENUM_FMT>` 则继续枚举硬件支持的格式，并在末尾附加 libv4l 提供的模拟格式。)

.. _libv4l-ops:

Libv4l device control functions(Libv4l 设备控制函数)
----------------------------------------------------

The common file operation methods are provided by libv4l.
(公共的文件操作方法由 libv4l 提供。)

Those functions operate just like the gcc function ``dup()`` and
V4L2 functions
:c:func:`open()`, :c:func:`close()`,
:c:func:`ioctl()`, :c:func:`read()`,
:c:func:`mmap()` and :c:func:`munmap()`:
(这些函数的行为与 gcc 的 ``dup()`` 函数以及 V4L2 的 :c:func:`open()`、:c:func:`close()`、:c:func:`ioctl()`、:c:func:`read()`、:c:func:`mmap()` 和 :c:func:`munmap()` 完全一致：)

.. c:function:: int v4l2_open(const char *file, int oflag, ...)

   operates like the :c:func:`open()` function.
   (行为与 :c:func:`open()` 函数一致。)

.. c:function:: int v4l2_close(int fd)

   operates like the :c:func:`close()` function.
   (行为与 :c:func:`close()` 函数一致。)

.. c:function:: int v4l2_dup(int fd)

   operates like the libc ``dup()`` function, duplicating a file handler.
   (行为与 libc 的 ``dup()`` 函数一致，用于复制一个文件句柄。)

.. c:function:: int v4l2_ioctl (int fd, unsigned long int request, ...)

   operates like the :c:func:`ioctl()` function.
   (行为与 :c:func:`ioctl()` 函数一致。)

.. c:function:: int v4l2_read (int fd, void* buffer, size_t n)

   operates like the :c:func:`read()` function.
   (行为与 :c:func:`read()` 函数一致。)

.. c:function:: void *v4l2_mmap(void *start, size_t length, int prot, int flags, int fd, int64_t offset);

   operates like the :c:func:`mmap()` function.
   (行为与 :c:func:`mmap()` 函数一致。)

.. c:function:: int v4l2_munmap(void *_start, size_t length);

   operates like the :c:func:`munmap()` function.
   (行为与 :c:func:`munmap()` 函数一致。)

Those functions provide additional control:
(这些函数提供额外的控制：)

.. c:function:: int v4l2_fd_open(int fd, int v4l2_flags)

   opens an already opened fd for further use through v4l2lib and possibly
   modify libv4l2's default behavior through the ``v4l2_flags`` argument.
   Currently, ``v4l2_flags`` can be ``V4L2_DISABLE_CONVERSION``, to disable
   format conversion.
   (打开一个已打开的 fd 供 v4l2lib 继续使用，并可通过 ``v4l2_flags`` 参数修改 libv4l2 的默认行为。目前 ``v4l2_flags`` 可取 ``V4L2_DISABLE_CONVERSION``，用于禁用格式转换。)

.. c:function:: int v4l2_set_control(int fd, int cid, int value)

   This function takes a value of 0 - 65535, and then scales that range to the
   actual range of the given v4l control id, and then if the cid exists and is
   not locked sets the cid to the scaled value.
   (该函数接受 0 - 65535 的值，先把该范围缩放到给定 v4l 控制 id 的实际范围，然后在 cid 存在且未被锁定时，把 cid 设置为缩放后的值。)

.. c:function:: int v4l2_get_control(int fd, int cid)

   This function returns a value of 0 - 65535, scaled to from the actual range
   of the given v4l control id. when the cid does not exist, could not be
   accessed for some reason, or some error occurred 0 is returned.
   (该函数返回一个 0 - 65535 的值，由给定 v4l 控制 id 的实际范围缩放而来。当 cid 不存在、因某些原因无法访问或发生错误时，返回 0。)

v4l1compat.so wrapper library(v4l1compat.so 包装库)
===================================================

This library intercepts calls to
:c:func:`open()`, :c:func:`close()`,
:c:func:`ioctl()`, :c:func:`mmap()` and
:c:func:`munmap()`
operations and redirects them to the libv4l counterparts, by using
``LD_PRELOAD=/usr/lib/v4l1compat.so``. It also emulates V4L1 calls via V4L2
API.
(该库通过 ``LD_PRELOAD=/usr/lib/v4l1compat.so`` 拦截对 :c:func:`open()`、:c:func:`close()`、:c:func:`ioctl()`、:c:func:`mmap()` 与 :c:func:`munmap()` 操作的调用，并把它们重定向到 libv4l 对应的函数。它还通过 V4L2 API 模拟 V4L1 调用。)

It allows usage of binary legacy applications that still don't use
libv4l.
(它让仍然没有使用 libv4l 的旧二进制应用也能继续工作。)
