.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_PRIORITY:

****************************************************************
ioctl VIDIOC_G_PRIORITY, VIDIOC_S_PRIORITY(查询或请求访问优先级)
****************************************************************

Name(名称)
==========

VIDIOC_G_PRIORITY - VIDIOC_S_PRIORITY - Query or request the access priority associated with a file descriptor(VIDIOC_G_PRIORITY - VIDIOC_S_PRIORITY - 查询或请求与文件描述符关联的访问优先级)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_PRIORITY

``int ioctl(int fd, VIDIOC_G_PRIORITY, enum v4l2_priority *argp)``

.. c:macro:: VIDIOC_S_PRIORITY

``int ioctl(int fd, VIDIOC_S_PRIORITY, const enum v4l2_priority *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to an enum :c:type:`v4l2_priority` type.
    (指向 enum :c:type:`v4l2_priority` 类型的指针。)

Description(描述)
=================

To query the current access priority applications call the
:ref:`VIDIOC_G_PRIORITY <VIDIOC_G_PRIORITY>` ioctl with a pointer to an enum v4l2_priority
variable where the driver stores the current priority.
(要查询当前访问优先级，应用程序调用 :ref:`VIDIOC_G_PRIORITY <VIDIOC_G_PRIORITY>`
ioctl，传一个指向 enum v4l2_priority 变量的指针，驱动程序把当前
优先级存储在其中。)

To request an access priority applications store the desired priority in
an enum v4l2_priority variable and call :ref:`VIDIOC_S_PRIORITY <VIDIOC_G_PRIORITY>` ioctl
with a pointer to this variable.
(要请求一个访问优先级，应用程序把期望的优先级存储在 enum
v4l2_priority 变量中，并调用 :ref:`VIDIOC_S_PRIORITY <VIDIOC_G_PRIORITY>` ioctl，
传指向该变量的指针。)

.. c:type:: v4l2_priority

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. flat-table:: enum v4l2_priority
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_PRIORITY_UNSET``
      - 0
      -
    * - ``V4L2_PRIORITY_BACKGROUND``
      - 1
      - Lowest priority, usually applications running in background, for
	example monitoring VBI transmissions. A proxy application running
	in user space will be necessary if multiple applications want to
	read from a device at this priority.
        (最低优先级，通常是在后台运行的应用程序，例如监控 VBI 传输。
        如果多个应用程序想以此优先级从设备读取，则需要用户空间中运行
        一个代理应用程序。)
    * - ``V4L2_PRIORITY_INTERACTIVE``
      - 2
      -
    * - ``V4L2_PRIORITY_DEFAULT``
      - 2
      - Medium priority, usually applications started and interactively
	controlled by the user. For example TV viewers, Teletext browsers,
	or just "panel" applications to change the channel or video
	controls. This is the default priority unless an application
	requests another.
        (中等优先级，通常是用户启动并交互式控制的应用程序。例如电视
        观看器、图文电视(Teletext)浏览器，或者只是用来切换频道或调整
        视频控制的"面板"应用。除非应用程序请求其他优先级，这是默认
        优先级。)
    * - ``V4L2_PRIORITY_RECORD``
      - 3
      - Highest priority. Only one file descriptor can have this priority,
	it blocks any other fd from changing device properties. Usually
	applications which must not be interrupted, like video recording.
        (最高优先级。只有一个文件描述符可以拥有此优先级，它阻止任何
        其他 fd 更改设备属性。通常是不允许被打断的应用程序，例如视频
        录制。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The requested priority value is invalid.
    (请求的优先级值无效。)

EBUSY
    Another application already requested higher priority.
    (另一个应用程序已经请求了更高的优先级。)
