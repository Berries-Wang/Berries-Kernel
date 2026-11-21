.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

**********************************************
Multi-planar format structures(多平面格式结构)
**********************************************

The struct :c:type:`v4l2_plane_pix_format` structures define size
and layout for each of the planes in a multi-planar format. The
struct :c:type:`v4l2_pix_format_mplane` structure contains
information common to all planes (such as image width and height) and an
array of struct :c:type:`v4l2_plane_pix_format` structures,
describing all planes of that format.
(struct :c:type:`v4l2_plane_pix_format` 结构定义多平面格式中每个平面的大小和布局。struct :c:type:`v4l2_pix_format_mplane` 结构包含所有平面共有的信息（例如图像宽度和高度）以及一个 struct :c:type:`v4l2_plane_pix_format` 结构数组，描述该格式的所有平面。)



.. tabularcolumns:: |p{1.4cm}|p{4.0cm}|p{11.9cm}|

.. c:type:: v4l2_plane_pix_format

.. flat-table:: struct v4l2_plane_pix_format
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``sizeimage``
      - Maximum size in bytes required for image data in this plane,
	set by the driver. When the image consists of variable length
	compressed data this is the number of bytes required by the
	codec to support the worst-case compression scenario.
        (此平面中图像数据所需的最大字节大小，由驱动程序设置。当图像由可变长度的压缩数据组成时，这是编解码器在最坏压缩情况下所需的字节数。)

	The driver will set the value for uncompressed images.
        (对于未压缩的图像，驱动程序会设置该值。)

	Clients are allowed to set the sizeimage field for variable length
	compressed data flagged with ``V4L2_FMT_FLAG_COMPRESSED`` at
	:ref:`VIDIOC_ENUM_FMT`, but the driver may ignore it and set the
	value itself, or it may modify the provided value based on
	alignment requirements or minimum/maximum size requirements.
	If the client wants to leave this to the driver, then it should
	set sizeimage to 0.
        (允许客户端为在 :ref:`VIDIOC_ENUM_FMT` 处标记了 ``V4L2_FMT_FLAG_COMPRESSED`` 的可变长度压缩数据设置 sizeimage 字段，但驱动程序可以忽略它并自行设置值，或者可以根据对齐要求或最小/最大尺寸要求修改所提供的值。如果客户端想把这一点留给驱动程序处理，则应将 sizeimage 设置为 0。)
    * - __u32
      - ``bytesperline``
      - Distance in bytes between the leftmost pixels in two adjacent
	lines. See struct :c:type:`v4l2_pix_format`.
        (两条相邻行中最左侧像素之间以字节为单位的距离。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u16
      - ``reserved[6]``
      - Reserved for future extensions. Should be zeroed by drivers and
	applications.
        (保留用于未来的扩展。驱动程序和应用程序应将其置零。)


.. raw:: latex

    \small

.. tabularcolumns:: |p{4.4cm}|p{5.6cm}|p{7.3cm}|

.. c:type:: v4l2_pix_format_mplane

.. flat-table:: struct v4l2_pix_format_mplane
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``width``
      - Image width in pixels. See struct
	:c:type:`v4l2_pix_format`.
        (图像宽度，以像素为单位。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u32
      - ``height``
      - Image height in pixels. See struct
	:c:type:`v4l2_pix_format`.
        (图像高度，以像素为单位。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u32
      - ``pixelformat``
      - The pixel format. Both single- and multi-planar four character
	codes can be used.
        (像素格式。单平面和多平面的四字符代码都可以使用。)
    * - __u32
      - ``field``
      - Field order, from enum :c:type:`v4l2_field`.
        See struct :c:type:`v4l2_pix_format`.
        (场序，来自 enum :c:type:`v4l2_field` 。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u32
      - ``colorspace``
      - Colorspace encoding, from enum :c:type:`v4l2_colorspace`.
        See struct :c:type:`v4l2_pix_format`.
        (色彩空间编码，来自 enum :c:type:`v4l2_colorspace` 。参见 struct :c:type:`v4l2_pix_format` 。)
    * - struct :c:type:`v4l2_plane_pix_format`
      - ``plane_fmt[VIDEO_MAX_PLANES]``
      - An array of structures describing format of each plane this pixel
	format consists of. The number of valid entries in this array has
	to be put in the ``num_planes`` field.
        (描述此像素格式所包含的每个平面的格式的结构数组。此数组中有效条目的数量必须填入 ``num_planes`` 字段。)
    * - __u8
      - ``num_planes``
      - Number of planes (i.e. separate memory buffers) for this format
	and the number of valid entries in the ``plane_fmt`` array.
        (此格式的平面（即独立内存缓冲区）数量，以及 ``plane_fmt`` 数组中有效条目的数量。)
    * - __u8
      - ``flags``
      - Flags set by the application or driver, see :ref:`format-flags`.
        (由应用程序或驱动程序设置的标志，参见 :ref:`format-flags` 。)
    * - union {
      - (anonymous)
    * - __u8
      - ``ycbcr_enc``
      - Y'CbCr encoding, from enum :c:type:`v4l2_ycbcr_encoding`.
	See struct :c:type:`v4l2_pix_format`.
        (Y'CbCr 编码，来自 enum :c:type:`v4l2_ycbcr_encoding` 。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u8
      - ``hsv_enc``
      - HSV encoding, from enum :c:type:`v4l2_hsv_encoding`.
	See struct :c:type:`v4l2_pix_format`.
        (HSV 编码，来自 enum :c:type:`v4l2_hsv_encoding` 。参见 struct :c:type:`v4l2_pix_format` 。)
    * - }
      -
    * - __u8
      - ``quantization``
      - Quantization range, from enum :c:type:`v4l2_quantization`.
	See struct :c:type:`v4l2_pix_format`.
        (量化范围，来自 enum :c:type:`v4l2_quantization` 。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u8
      - ``xfer_func``
      - Transfer function, from enum :c:type:`v4l2_xfer_func`.
	See struct :c:type:`v4l2_pix_format`.
        (传递函数，来自 enum :c:type:`v4l2_xfer_func` 。参见 struct :c:type:`v4l2_pix_format` 。)
    * - __u8
      - ``reserved[7]``
      - Reserved for future extensions. Should be zeroed by drivers and
	applications.
        (保留用于未来的扩展。驱动程序和应用程序应将其置零。)

.. raw:: latex

    \normalsize
