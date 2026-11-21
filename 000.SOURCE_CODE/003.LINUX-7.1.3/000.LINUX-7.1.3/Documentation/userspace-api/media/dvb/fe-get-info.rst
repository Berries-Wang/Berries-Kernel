.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_GET_INFO:

*******************************
ioctl FE_GET_INFO(查询前端信息)
*******************************

Name(名称)
==========

FE_GET_INFO - Query Digital TV frontend capabilities and returns information
about the - front-end. This call only requires read-only access to the device.
(FE_GET_INFO - 查询数字电视前端能力并返回前端的有关信息。此调用只需要对设备
的只读访问。)

Synopsis(概要)
==============

.. c:macro:: FE_GET_INFO

``int ioctl(int fd, FE_GET_INFO, struct dvb_frontend_info *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    pointer to struct :c:type:`dvb_frontend_info`
    (指向 struct :c:type:`dvb_frontend_info` 的指针)

Description(描述)
=================

All Digital TV frontend devices support the :ref:`FE_GET_INFO` ioctl. It is
used to identify kernel devices compatible with this specification and to
obtain information about driver and hardware capabilities. The ioctl
takes a pointer to dvb_frontend_info which is filled by the driver.
When the driver is not compatible with this specification the ioctl
returns an error.
(所有数字电视前端设备都支持 :ref:`FE_GET_INFO` ioctl。它用于识别与此规范
兼容的内核设备，并获取驱动与硬件能力的信息。该 ioctl 接受一个指向
dvb_frontend_info 的指针，由驱动填充。当驱动与此规范不兼容时，ioctl 返回
错误。)

frontend capabilities(前端能力)
===============================

Capabilities describe what a frontend can do. Some capabilities are
supported only on some specific frontend types.
(能力描述前端能做什么。有些能力只在某些特定的前端类型上受支持。)

The frontend capabilities are described at :c:type:`fe_caps`.
(前端能力在 :c:type:`fe_caps` 中描述。)

Return Value(返回值)
====================

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
