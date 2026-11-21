.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_CTRL:

******************************************************
ioctl VIDIOC_G_CTRL, VIDIOC_S_CTRL(获取或设置控制的值)
******************************************************

Name(名称)
==========

VIDIOC_G_CTRL - VIDIOC_S_CTRL - Get or set the value of a control(VIDIOC_G_CTRL - VIDIOC_S_CTRL - 获取或设置控制的值)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_CTRL

``int ioctl(int fd, VIDIOC_G_CTRL, struct v4l2_control *argp)``

.. c:macro:: VIDIOC_S_CTRL

``int ioctl(int fd, VIDIOC_S_CTRL, struct v4l2_control *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_control`.
    (指向 struct :c:type:`v4l2_control` 的指针。)

Description(描述)
=================

To get the current value of a control applications initialize the ``id``
field of a struct :c:type:`v4l2_control` and call the
:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` ioctl with a pointer to this structure. To change the
value of a control applications initialize the ``id`` and ``value``
fields of a struct :c:type:`v4l2_control` and call the
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl.
(要获取控制的当前值，应用程序初始化 struct :c:type:`v4l2_control` 的
``id`` 字段，并以指向该结构体的指针调用 :ref:`VIDIOC_G_CTRL
<VIDIOC_G_CTRL>` ioctl。要更改控制的值，应用程序初始化 struct
:c:type:`v4l2_control` 的 ``id`` 和 ``value`` 字段，并以指向该结构体的
指针调用 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl。)

When the ``id`` is invalid drivers return an ``EINVAL`` error code. When the
``value`` is out of bounds drivers can choose to take the closest valid
value or return an ``ERANGE`` error code, whatever seems more appropriate.
However, :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` is a write-only ioctl, it does not return the
actual new value. If the ``value`` is inappropriate for the control
(e.g. if it refers to an unsupported menu index of a menu control), then
EINVAL error code is returned as well.
(当 ``id`` 无效时，驱动程序返回 ``EINVAL`` 错误码。当 ``value`` 超出
边界时，驱动程序可以选择取最接近的有效值，或返回 ``ERANGE`` 错误码，
哪种看起来更合适就选哪种。但 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` 是
只写 ioctl，不返回实际的新值。如果 ``value`` 对该控制不合适(例如它
引用了菜单控制的一个不受支持的菜单索引)，那么同样返回 EINVAL
错误码。)

These ioctls work only with user controls. For other control classes the
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`,
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` or
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` must be used.
(这些 ioctl 只适用于用户控制。对于其他控制类，必须使用
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`、
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 或
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`。)

.. c:type:: v4l2_control

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_control
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``id``
      - Identifies the control, set by the application.
        (标识该控制，由应用程序设置。)
    * - __s32
      - ``value``
      - New value or current value.
        (新值或当前值。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_control` ``id`` is invalid
    or the ``value`` is inappropriate for the given control (i.e. if a
    menu item is selected that is not supported by the driver according
    to :ref:`VIDIOC_QUERYMENU <VIDIOC_QUERYCTRL>`).
    (struct :c:type:`v4l2_control` 的 ``id`` 无效，或 ``value`` 对给定
    的控制不合适(即根据 :ref:`VIDIOC_QUERYMENU <VIDIOC_QUERYCTRL>`，
    选中了一个驱动程序不支持的菜单项)。)

ERANGE
    The struct :c:type:`v4l2_control` ``value`` is out of
    bounds.
    (struct :c:type:`v4l2_control` 的 ``value`` 超出边界。)

EBUSY
    The control is temporarily not changeable, possibly because another
    applications took over control of the device function this control
    belongs to.
    (该控制暂时不可更改，可能是因为另一个应用程序接管了此控制所属的
    设备功能的控制权。)

EACCES
    Attempt to set a read-only control or to get a write-only control.
    (试图设置只读控制或获取只写控制。)

    Or if there is an attempt to set an inactive control and the driver is
    not capable of caching the new value until the control is active again.
    (或者，如果试图设置一个非活动控制，而驱动程序无法在控制重新变为
    活动之前缓存新值。)
