.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _jpeg-controls:

*************************************
JPEG Control Reference(JPEG 控制参考)
*************************************

The JPEG class includes controls for common features of JPEG encoders
and decoders. Currently it includes features for codecs implementing
progressive baseline DCT compression process with Huffman entropy
coding.
(JPEG 类包含用于 JPEG 编码器和解码器常见功能的控制项。目前它包含
实现采用 Huffman 熵编码的渐进式基线 DCT 压缩过程的编解码器功能。)


.. _jpeg-control-id:

JPEG Control IDs(JPEG 控制 ID)
==============================

``V4L2_CID_JPEG_CLASS (class)``
    The JPEG class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (JPEG 类描述符。对该控制调用 :ref:`VIDIOC_QUERYCTRL` 将返回此
    控制类的描述。)

``V4L2_CID_JPEG_CHROMA_SUBSAMPLING (menu)``
    The chroma subsampling factors describe how each component of an
    input image is sampled, in respect to maximum sample rate in each
    spatial dimension. See :ref:`itu-t81`, clause A.1.1. for more
    details. The ``V4L2_CID_JPEG_CHROMA_SUBSAMPLING`` control determines
    how Cb and Cr components are downsampled after converting an input
    image from RGB to Y'CbCr color space.
    (色度子采样因子描述输入图像的每个分量相对于每个空间维度最大采样
    率的采样方式。更多信息参见 :ref:`itu-t81` 的 A.1.1 节。
    ``V4L2_CID_JPEG_CHROMA_SUBSAMPLING`` 控制决定在把输入图像从 RGB
    转换到 Y'CbCr 色彩空间之后，如何对 Cb 和 Cr 分量做降采样。)

.. tabularcolumns:: |p{7.5cm}|p{10.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_JPEG_CHROMA_SUBSAMPLING_444``
      - No chroma subsampling, each pixel has Y, Cr and Cb values.
        (无色度子采样，每个像素都有 Y、Cr 和 Cb 值。)
    * - ``V4L2_JPEG_CHROMA_SUBSAMPLING_422``
      - Horizontally subsample Cr, Cb components by a factor of 2.
        (在水平方向上以 2 倍因子对 Cr、Cb 分量做子采样。)
    * - ``V4L2_JPEG_CHROMA_SUBSAMPLING_420``
      - Subsample Cr, Cb components horizontally and vertically by 2.
        (在水平和垂直方向上都以 2 倍对 Cr、Cb 分量做子采样。)
    * - ``V4L2_JPEG_CHROMA_SUBSAMPLING_411``
      - Horizontally subsample Cr, Cb components by a factor of 4.
        (在水平方向上以 4 倍因子对 Cr、Cb 分量做子采样。)
    * - ``V4L2_JPEG_CHROMA_SUBSAMPLING_410``
      - Subsample Cr, Cb components horizontally by 4 and vertically by 2.
        (在水平方向上以 4 倍、垂直方向上以 2 倍对 Cr、Cb 分量做子采样。)
    * - ``V4L2_JPEG_CHROMA_SUBSAMPLING_GRAY``
      - Use only luminance component.
        (仅使用亮度分量。)



``V4L2_CID_JPEG_RESTART_INTERVAL (integer)``
    The restart interval determines an interval of inserting RSTm
    markers (m = 0..7). The purpose of these markers is to additionally
    reinitialize the encoder process, in order to process blocks of an
    image independently. For the lossy compression processes the restart
    interval unit is MCU (Minimum Coded Unit) and its value is contained
    in DRI (Define Restart Interval) marker. If
    ``V4L2_CID_JPEG_RESTART_INTERVAL`` control is set to 0, DRI and RSTm
    markers will not be inserted.
    (重启间隔决定插入 RSTm 标记(m = 0..7)的间隔。这些标记的目的是
    额外地重新初始化编码器进程，以便独立地处理图像的各个块。对于有损
    压缩过程，重启间隔的单位是 MCU(最小编码单元)，其值包含在 DRI
    (Define Restart Interval)标记中。如果
    ``V4L2_CID_JPEG_RESTART_INTERVAL`` 控制被设为 0，则不会插入 DRI
    和 RSTm 标记。)

.. _jpeg-quality-control:

``V4L2_CID_JPEG_COMPRESSION_QUALITY (integer)``
    Determines trade-off between image quality and size.
    It provides simpler method for applications to control image quality,
    without a need for direct reconfiguration of luminance and chrominance
    quantization tables. In cases where a driver uses quantization tables
    configured directly by an application, using interfaces defined
    elsewhere, ``V4L2_CID_JPEG_COMPRESSION_QUALITY`` control should be set by
    driver to 0.
    (决定图像质量与尺寸之间的权衡。它为应用程序提供了一种更简单的
    控制图像质量的方法，无需直接重新配置亮度与色度量化表。在驱动程序
    使用由应用程序通过别处定义的接口直接配置的量化表的情况下，驱动
    程序应把 ``V4L2_CID_JPEG_COMPRESSION_QUALITY`` 控制设为 0。)

    The value range of this control is driver-specific. Only positive,
    non-zero values are meaningful. The recommended range is 1 - 100,
    where larger values correspond to better image quality.
    (该控制的取值范围由驱动程序决定。只有正的非零值才有意义。推荐的
    范围是 1 - 100，值越大对应越好的图像质量。)

.. _jpeg-active-marker-control:

``V4L2_CID_JPEG_ACTIVE_MARKER (bitmask)``
    Specify which JPEG markers are included in compressed stream. This
    control is valid only for encoders.
    (指定压缩流中包含哪些 JPEG 标记。该控制仅对编码器有效。)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_JPEG_ACTIVE_MARKER_APP0``
      - Application data segment APP\ :sub:`0`.
        (应用数据段 APP\ :sub:`0`)
    * - ``V4L2_JPEG_ACTIVE_MARKER_APP1``
      - Application data segment APP\ :sub:`1`.
        (应用数据段 APP\ :sub:`1`)
    * - ``V4L2_JPEG_ACTIVE_MARKER_COM``
      - Comment segment.
        (注释段。)
    * - ``V4L2_JPEG_ACTIVE_MARKER_DQT``
      - Quantization tables segment.
        (量化表段。)
    * - ``V4L2_JPEG_ACTIVE_MARKER_DHT``
      - Huffman tables segment.
        (Huffman 表段。)



For more details about JPEG specification, refer to :ref:`itu-t81`,
:ref:`jfif`, :ref:`w3c-jpeg-jfif`.
(关于 JPEG 规范的更多细节，参见 :ref:`itu-t81`、:ref:`jfif`、
:ref:`w3c-jpeg-jfif`。)
