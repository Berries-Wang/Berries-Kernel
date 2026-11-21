.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_JPEGCOMP:

******************************************
ioctl VIDIOC_G_JPEGCOMP, VIDIOC_S_JPEGCOMP
******************************************

Name(名称)
==========

VIDIOC_G_JPEGCOMP - VIDIOC_S_JPEGCOMP

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_JPEGCOMP

``int ioctl(int fd, VIDIOC_G_JPEGCOMP, v4l2_jpegcompression *argp)``

.. c:macro:: VIDIOC_S_JPEGCOMP

``int ioctl(int fd, VIDIOC_S_JPEGCOMP, const v4l2_jpegcompression *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_jpegcompression`.
    (指向 struct :c:type:`v4l2_jpegcompression` 的指针。)

Description(描述)
=================

These ioctls are **deprecated**. New drivers and applications should use
:ref:`JPEG class controls <jpeg-controls>` for image quality and JPEG
markers control.
(这些 ioctl 已 **被弃用**。新的驱动程序和应用程序应使用 :ref:`JPEG class controls <jpeg-controls>` 来控制图像质量和 JPEG 标记。)

[to do]

Ronald Bultje elaborates:
(Ronald Bultje 详细说明：)

APP is some application-specific information. The application can set it
itself, and it'll be stored in the JPEG-encoded fields (eg; interlacing
information for in an AVI or so). COM is the same, but it's comments,
like 'encoded by me' or so.
(APP 是一些应用程序特定的信息。应用程序可以自行设置它，它会被存储在 JPEG 编码的字段中（例如 AVI 中的隔行扫描信息等）。COM 与此相同，但它是注释，例如 'encoded by me' 之类。)

jpeg_markers describes whether the huffman tables, quantization tables
and the restart interval information (all JPEG-specific stuff) should be
stored in the JPEG-encoded fields. These define how the JPEG field is
encoded. If you omit them, applications assume you've used standard
encoding. You usually do want to add them.
(jpeg_markers 描述 huffman 表、量化表和重启间隔信息（所有 JPEG 特有的内容）是否应存储在 JPEG 编码的字段中。这些定义了 JPEG 字段的编码方式。如果省略它们，应用程序会假定你使用了标准编码。通常你确实需要添加它们。)

.. tabularcolumns:: |p{1.2cm}|p{3.0cm}|p{13.1cm}|

.. c:type:: v4l2_jpegcompression

.. flat-table:: struct v4l2_jpegcompression
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - int
      - ``quality``
      - Deprecated. If
	:ref:`V4L2_CID_JPEG_COMPRESSION_QUALITY <jpeg-quality-control>`
	control is exposed by a driver applications should use it instead
	and ignore this field.
        (已弃用。如果驱动程序暴露了 :ref:`V4L2_CID_JPEG_COMPRESSION_QUALITY <jpeg-quality-control>` 控件，应用程序应改用该控件并忽略此字段。)
    * - int
      - ``APPn``
      -
    * - int
      - ``APP_len``
      -
    * - char
      - ``APP_data``\ [60]
      -
    * - int
      - ``COM_len``
      -
    * - char
      - ``COM_data``\ [60]
      -
    * - __u32
      - ``jpeg_markers``
      - See :ref:`jpeg-markers`. Deprecated. If
	:ref:`V4L2_CID_JPEG_ACTIVE_MARKER <jpeg-active-marker-control>`
	control is exposed by a driver applications should use it instead
	and ignore this field.
        (参见 :ref:`jpeg-markers` 。已弃用。如果驱动程序暴露了 :ref:`V4L2_CID_JPEG_ACTIVE_MARKER <jpeg-active-marker-control>` 控件，应用程序应改用该控件并忽略此字段。)

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _jpeg-markers:

.. flat-table:: JPEG Markers Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_JPEG_MARKER_DHT``
      - (1<<3)
      - Define Huffman Tables
        (定义 Huffman 表)
    * - ``V4L2_JPEG_MARKER_DQT``
      - (1<<4)
      - Define Quantization Tables
        (定义量化表)
    * - ``V4L2_JPEG_MARKER_DRI``
      - (1<<5)
      - Define Restart Interval
        (定义重启间隔)
    * - ``V4L2_JPEG_MARKER_COM``
      - (1<<6)
      - Comment segment
        (注释段)
    * - ``V4L2_JPEG_MARKER_APP``
      - (1<<7)
      - App segment, driver will always use APP0
        (APP 段，驱动程序将始终使用 APP0)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
