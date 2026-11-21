.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_G_ROUTING:

**************************************************************************************************************
ioctl VIDIOC_SUBDEV_G_ROUTING, VIDIOC_SUBDEV_S_ROUTING(ioctl VIDIOC_SUBDEV_G_ROUTING、VIDIOC_SUBDEV_S_ROUTING)
**************************************************************************************************************

Name(名称)
==========

VIDIOC_SUBDEV_G_ROUTING - VIDIOC_SUBDEV_S_ROUTING - Get or set routing between streams of media pads in a media entity.
(VIDIOC_SUBDEV_G_ROUTING - VIDIOC_SUBDEV_S_ROUTING - 获取或设置媒体实体中媒体 pad 各流之间的路由。)


Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_G_ROUTING

``int ioctl(int fd, VIDIOC_SUBDEV_G_ROUTING, struct v4l2_subdev_routing *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_ROUTING

``int ioctl(int fd, VIDIOC_SUBDEV_S_ROUTING, struct v4l2_subdev_routing *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :ref:`open() <func-open>`.
    (由 :ref:`open() <func-open>` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_routing`.
    (指向 struct :c:type:`v4l2_subdev_routing` 的指针。)


Description(描述)
=================

These ioctls are used to get and set the routing in a media entity.
The routing configuration determines the flows of data inside an entity.
(这些 ioctl 用于获取和设置媒体实体中的路由。路由配置决定了实体内部的数据流。)

Drivers report their current routing tables using the
``VIDIOC_SUBDEV_G_ROUTING`` ioctl and application may enable or disable routes
with the ``VIDIOC_SUBDEV_S_ROUTING`` ioctl, by adding or removing routes and
setting or clearing flags of the ``flags`` field of a struct
:c:type:`v4l2_subdev_route`. Similarly to ``VIDIOC_SUBDEV_G_ROUTING``, also
``VIDIOC_SUBDEV_S_ROUTING`` returns the routes back to the user.
(驱动使用 ``VIDIOC_SUBDEV_G_ROUTING`` ioctl 报告其当前的路由表，应用程序可以使用 ``VIDIOC_SUBDEV_S_ROUTING`` ioctl 启用或禁用路由：通过添加或删除路由，以及设置或清除 struct :c:type:`v4l2_subdev_route` 的 ``flags`` 字段中的标志。与 ``VIDIOC_SUBDEV_G_ROUTING`` 类似， ``VIDIOC_SUBDEV_S_ROUTING`` 也会将路由返回给用户。)

All stream configurations are reset when ``VIDIOC_SUBDEV_S_ROUTING`` is called.
This means that the userspace must reconfigure all stream formats and selections
after calling the ioctl with e.g. ``VIDIOC_SUBDEV_S_FMT``.
(当调用 ``VIDIOC_SUBDEV_S_ROUTING`` 时，所有流配置都会被重置。这意味着用户空间必须在调用该 ioctl 之后，使用例如 ``VIDIOC_SUBDEV_S_FMT`` 重新配置所有流的格式和选择。)

Only subdevices which have both sink and source pads can support routing.
(只有同时具有 sink pad 和 source pad 的子设备才能支持路由。)

The ``len_routes`` field indicates the number of routes that can fit in the
``routes`` array allocated by userspace. It is set by applications for both
ioctls to indicate how many routes the kernel can return, and is never modified
by the kernel.
( ``len_routes`` 字段表示可以容纳在用户空间所分配的 ``routes`` 数组中的路由数量。应用程序对两个 ioctl 都会设置它，以指示内核最多可以返回多少条路由；内核从不修改它。)

The ``num_routes`` field indicates the number of routes in the routing
table. For ``VIDIOC_SUBDEV_S_ROUTING``, it is set by userspace to the number of
routes that the application stored in the ``routes`` array. For both ioctls, it
is returned by the kernel and indicates how many routes are stored in the
subdevice routing table. This may be smaller or larger than the value of
``num_routes`` set by the application for ``VIDIOC_SUBDEV_S_ROUTING``, as
drivers may adjust the requested routing table.
( ``num_routes`` 字段表示路由表中的路由数量。对于 ``VIDIOC_SUBDEV_S_ROUTING``，用户空间将其设置为应用程序存储在 ``routes`` 数组中的路由数量。对于这两个 ioctl，它都由内核返回，表示子设备路由表中存储了多少条路由。它可能小于或大于应用程序为 ``VIDIOC_SUBDEV_S_ROUTING`` 设置的 ``num_routes`` 值，因为驱动可能会调整所请求的路由表。)

The kernel can return a ``num_routes`` value larger than ``len_routes`` from
both ioctls. This indicates thare are more routes in the routing table than fits
the ``routes`` array. In this case, the ``routes`` array is filled by the kernel
with the first ``len_routes`` entries of the subdevice routing table. This is
not considered to be an error, and the ioctl call succeeds. If the applications
wants to retrieve the missing routes, it can issue a new
``VIDIOC_SUBDEV_G_ROUTING`` call with a large enough ``routes`` array.
(内核可以从两个 ioctl 返回大于 ``len_routes`` 的 ``num_routes`` 值。这表示路由表中的路由数量超过了 ``routes`` 数组能容纳的数量。在这种情况下，内核会用子设备路由表的前 ``len_routes`` 个条目填充 ``routes`` 数组。这不被视为错误，ioctl 调用会成功。如果应用程序想获取缺失的路由，它可以用一个足够大的 ``routes`` 数组再次发出 ``VIDIOC_SUBDEV_G_ROUTING`` 调用。)

``VIDIOC_SUBDEV_S_ROUTING`` may return more routes than the user provided in
``num_routes`` field due to e.g. hardware properties.
(由于例如硬件特性等原因， ``VIDIOC_SUBDEV_S_ROUTING`` 可能返回比用户在 ``num_routes`` 字段中提供的数量更多的路由。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.7cm}|

.. c:type:: v4l2_subdev_routing

.. flat-table:: struct v4l2_subdev_routing
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``which``
      - Routing table to be accessed, from enum
        :ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
	(要访问的路由表，来自枚举
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
    * - __u32
      - ``len_routes``
      - The length of the array (as in memory reserved for the array)
	(数组的长度(即为此数组预留的内存大小))
    * - struct :c:type:`v4l2_subdev_route`
      - ``routes[]``
      - Array of struct :c:type:`v4l2_subdev_route` entries
	(struct :c:type:`v4l2_subdev_route` 条目的数组)
    * - __u32
      - ``num_routes``
      - Number of entries of the routes array
	(routes 数组的条目数量)
    * - __u32
      - ``reserved``\ [11]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
	(保留供将来扩展使用。应用程序和驱动必须将该数组置零。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.7cm}|

.. c:type:: v4l2_subdev_route

.. flat-table:: struct v4l2_subdev_route
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``sink_pad``
      - Sink pad number.(sink pad 编号。)
    * - __u32
      - ``sink_stream``
      - Sink pad stream number.(sink pad 流编号。)
    * - __u32
      - ``source_pad``
      - Source pad number.(source pad 编号。)
    * - __u32
      - ``source_stream``
      - Source pad stream number.(source pad 流编号。)
    * - __u32
      - ``flags``
      - Route enable/disable flags
	:ref:`v4l2_subdev_routing_flags <v4l2-subdev-routing-flags>`.
	(路由启用/禁用标志，见
	:ref:`v4l2_subdev_routing_flags <v4l2-subdev-routing-flags>` 。)
    * - __u32
      - ``reserved``\ [5]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
	(保留供将来扩展使用。应用程序和驱动必须将该数组置零。)

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.7cm}|

.. _v4l2-subdev-routing-flags:

.. flat-table:: enum v4l2_subdev_routing_flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - V4L2_SUBDEV_ROUTE_FL_ACTIVE
      - 0x0001
      - The route is enabled. Set by applications.(该路由已启用。由应用程序设置。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
   The sink or source pad identifiers reference a non-existing pad or reference
   pads of different types (ie. the sink_pad identifiers refers to a source
   pad), the ``which`` field has an unsupported value, or, for
   ``VIDIOC_SUBDEV_S_ROUTING``, the num_routes field set by the application is
   larger than the len_routes field value.
   (sink 或 source pad 标识符引用了不存在的 pad，或引用了不同类型的 pad(即
   sink_pad 标识符引用了 source pad)； ``which`` 字段的值不受支持；或者对于
   ``VIDIOC_SUBDEV_S_ROUTING``，应用程序设置的 num_routes 字段大于 len_routes
   字段的值。)

ENXIO
   The application requested routes cannot be created or the state of
   the specified routes cannot be modified. Only returned for
   ``VIDIOC_SUBDEV_S_ROUTING``.
   (应用程序请求的路由无法创建，或指定路由的状态无法修改。仅对
   ``VIDIOC_SUBDEV_S_ROUTING`` 返回。)

E2BIG
   The application provided ``num_routes`` for ``VIDIOC_SUBDEV_S_ROUTING`` is
   larger than the number of routes the driver can handle.
   (应用程序为 ``VIDIOC_SUBDEV_S_ROUTING`` 提供的 ``num_routes`` 大于驱动
   能处理的路由数量。)
