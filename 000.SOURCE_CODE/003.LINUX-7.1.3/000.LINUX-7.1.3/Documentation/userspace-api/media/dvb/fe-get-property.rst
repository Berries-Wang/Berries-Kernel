.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_GET_PROPERTY:

**************************************
ioctl FE_SET_PROPERTY, FE_GET_PROPERTY
**************************************

Name(名称)
==========

FE_SET_PROPERTY - FE_GET_PROPERTY - FE_SET_PROPERTY sets one or more frontend properties. - FE_GET_PROPERTY returns one or more frontend properties.
(FE_SET_PROPERTY - FE_GET_PROPERTY - FE_SET_PROPERTY 设置一个或多个前
端属性。 - FE_GET_PROPERTY 返回一个或多个前端属性。)

Synopsis(概要)
==============

.. c:macro:: FE_GET_PROPERTY

``int ioctl(int fd, FE_GET_PROPERTY, struct dtv_properties *argp)``

.. c:macro:: FE_SET_PROPERTY

``int ioctl(int fd, FE_SET_PROPERTY, struct dtv_properties *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`dtv_properties`.
    (指向 struct :c:type:`dtv_properties` 的指针。)

Description(描述)
=================

All Digital TV frontend devices support the ``FE_SET_PROPERTY`` and
``FE_GET_PROPERTY`` ioctls. The supported properties and statistics
depends on the delivery system and on the device:
(所有数字电视前端设备都支持 ``FE_SET_PROPERTY`` 与 ``FE_GET_PROPERTY``
ioctl。所支持的属性和统计信息取决于传送系统 (delivery system) 与设
备：)

-  ``FE_SET_PROPERTY:``

   -  This ioctl is used to set one or more frontend properties.
      (此 ioctl 用于设置一个或多个前端属性。)

   -  This is the basic command to request the frontend to tune into
      some frequency and to start decoding the digital TV signal.
      (这是请求前端调谐到某个频率并开始解码数字电视信号的基本命令。)

   -  This call requires read/write access to the device.
      (此调用需要对设备的读写访问权限。)

.. note::

   At return, the values aren't updated to reflect the actual
   parameters used. If the actual parameters are needed, an explicit
   call to ``FE_GET_PROPERTY`` is needed.
   (返回时，这些值不会更新为实际使用的参数。如果需要实际参数，需要显
   式调用 ``FE_GET_PROPERTY``。)

-  ``FE_GET_PROPERTY:``

   -  This ioctl is used to get properties and statistics from the
      frontend.
      (此 ioctl 用于从前端获取属性和统计信息。)

   -  No properties are changed, and statistics aren't reset.
      (不改变任何属性，统计信息也不会被重置。)

   -  This call only requires read-only access to the device.
      (此调用只需要对设备的只读访问权限。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并适当设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
