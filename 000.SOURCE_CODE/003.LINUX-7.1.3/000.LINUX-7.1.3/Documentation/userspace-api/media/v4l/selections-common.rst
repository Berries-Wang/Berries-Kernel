.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-selections-common:

==========================================
Common selection definitions(选择公共定义)
==========================================

While the :ref:`V4L2 selection API <selection-api>` and
:ref:`V4L2 subdev selection APIs <v4l2-subdev-selections>` are very
similar, there's one fundamental difference between the two. On
sub-device API, the selection rectangle refers to the media bus format,
and is bound to a sub-device's pad. On the V4L2 interface the selection
rectangles refer to the in-memory pixel format.
(虽然 :ref:`V4L2 selection API <selection-api>` 与 :ref:`V4L2 subdev selection APIs <v4l2-subdev-selections>` 非常相似，但两者之间有一个根本区别：在子设备 API 中，选择矩形针对的是 media bus 格式，并绑定到子设备的 pad；而在 V4L2 接口中，选择矩形针对的是内存中的像素格式。)

This section defines the common definitions of the selection interfaces
on the two APIs.
(本节定义这两个 API 上选择接口的公共定义。)


.. toctree::
    :maxdepth: 1

    v4l2-selection-targets
    v4l2-selection-flags
