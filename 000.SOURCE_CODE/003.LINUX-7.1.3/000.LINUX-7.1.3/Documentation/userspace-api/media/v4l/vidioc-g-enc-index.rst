.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_ENC_INDEX:

**************************************
ioctl VIDIOC_G_ENC_INDEX(获取编码索引)
**************************************

Name(名称)
==========

VIDIOC_G_ENC_INDEX - Get meta data about a compressed video stream
(VIDIOC_G_ENC_INDEX - 获取关于压缩视频流的元数据)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_ENC_INDEX

``int ioctl(int fd, VIDIOC_G_ENC_INDEX, struct v4l2_enc_idx *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_enc_idx`.
    (指向 struct :c:type:`v4l2_enc_idx` 的指针。)

Description(描述)
=================

The :ref:`VIDIOC_G_ENC_INDEX <VIDIOC_G_ENC_INDEX>` ioctl provides meta data about a compressed
video stream the same or another application currently reads from the
driver, which is useful for random access into the stream without
decoding it.
(:ref:`VIDIOC_G_ENC_INDEX <VIDIOC_G_ENC_INDEX>` ioctl 提供关于某个压缩视频流
的元数据，该视频流是同一个或另一个应用当前正从驱动读取的；这对于不解码
就随机访问该流很有用。)

To read the data applications must call :ref:`VIDIOC_G_ENC_INDEX <VIDIOC_G_ENC_INDEX>` with a
pointer to a struct :c:type:`v4l2_enc_idx`. On success
the driver fills the ``entry`` array, stores the number of elements
written in the ``entries`` field, and initializes the ``entries_cap``
field.
(要读取数据，应用程序必须用指向 struct :c:type:`v4l2_enc_idx` 的指针调用
:ref:`VIDIOC_G_ENC_INDEX <VIDIOC_G_ENC_INDEX>`。成功时，驱动会填充
``entry`` 数组，把写入的元素个数存储在 ``entries`` 字段中，并初始化
``entries_cap`` 字段。)

Each element of the ``entry`` array contains meta data about one
picture. A :ref:`VIDIOC_G_ENC_INDEX <VIDIOC_G_ENC_INDEX>` call reads up to
``V4L2_ENC_IDX_ENTRIES`` entries from a driver buffer, which can hold up
to ``entries_cap`` entries. This number can be lower or higher than
``V4L2_ENC_IDX_ENTRIES``, but not zero. When the application fails to
read the meta data in time the oldest entries will be lost. When the
buffer is empty or no capturing/encoding is in progress, ``entries``
will be zero.
(``entry`` 数组的每个元素包含关于一幅图像的元数据。一次
:ref:`VIDIOC_G_ENC_INDEX <VIDIOC_G_ENC_INDEX>` 调用会从一个驱动缓冲区读取
最多 ``V4L2_ENC_IDX_ENTRIES`` 个条目，该缓冲区最多可容纳 ``entries_cap``
个条目。这个数可以低于或高于 ``V4L2_ENC_IDX_ENTRIES``，但不能为零。当
应用程序未能及时读取元数据时，最旧的条目将丢失。当缓冲区为空或没有进行
采集/编码时，``entries`` 将为零。)

Currently this ioctl is only defined for MPEG-2 program streams and
video elementary streams.
(目前此 ioctl 只为 MPEG-2 节目流和视频基本流定义。)

.. tabularcolumns:: |p{4.2cm}|p{6.2cm}|p{6.9cm}|

.. c:type:: v4l2_enc_idx

.. flat-table:: struct v4l2_enc_idx
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 3 8

    * - __u32
      - ``entries``
      - The number of entries the driver stored in the ``entry`` array.
	(驱动存储在 ``entry`` 数组中的条目数量。)
    * - __u32
      - ``entries_cap``
      - The number of entries the driver can buffer. Must be greater than
	zero.
	(驱动可以缓冲的条目数量。必须大于零。)
    * - __u32
      - ``reserved``\ [4]
      - Reserved for future extensions. Drivers must set the
	array to zero.
	(为未来扩展保留。驱动必须将数组置零。)
    * - struct :c:type:`v4l2_enc_idx_entry`
      - ``entry``\ [``V4L2_ENC_IDX_ENTRIES``]
      - Meta data about a compressed video stream. Each element of the
	array corresponds to one picture, sorted in ascending order by
	their ``offset``.
	(关于压缩视频流的元数据。数组的每个元素对应一幅图像，按其
	``offset`` 升序排序。)


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_enc_idx_entry

.. flat-table:: struct v4l2_enc_idx_entry
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``offset``
      - The offset in bytes from the beginning of the compressed video
	stream to the beginning of this picture, that is a *PES packet
	header* as defined in :ref:`mpeg2part1` or a *picture header* as
	defined in :ref:`mpeg2part2`. When the encoder is stopped, the
	driver resets the offset to zero.
	(从压缩视频流开始处到本图像开始处的偏移量（字节），即 :ref:`mpeg2part1`
	中定义的 *PES packet header*（PES 包头），或 :ref:`mpeg2part2` 中定义的
	*picture header*（图像头）。当编码器停止时，驱动会把偏移量重置为
	零。)
    * - __u64
      - ``pts``
      - The 33 bit *Presentation Time Stamp* of this picture as defined in
	:ref:`mpeg2part1`.
	(此图像的 33 位 *Presentation Time Stamp*（显示时间戳），定义见
	:ref:`mpeg2part1`。)
    * - __u32
      - ``length``
      - The length of this picture in bytes.
	(此图像的长度，以字节为单位。)
    * - __u32
      - ``flags``
      - Flags containing the coding type of this picture, see
	:ref:`enc-idx-flags`.
	(包含此图像编码类型的标志，参见 :ref:`enc-idx-flags`。)
    * - __u32
      - ``reserved``\ [2]
      - Reserved for future extensions. Drivers must set the array to
	zero.
	(为未来扩展保留。驱动必须将数组置零。)

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _enc-idx-flags:

.. flat-table:: Index Entry Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_ENC_IDX_FRAME_I``
      - 0x00
      - This is an Intra-coded picture.
	(这是一幅帧内编码 (Intra-coded) 图像。)
    * - ``V4L2_ENC_IDX_FRAME_P``
      - 0x01
      - This is a Predictive-coded picture.
	(这是一幅前向预测编码 (Predictive-coded) 图像。)
    * - ``V4L2_ENC_IDX_FRAME_B``
      - 0x02
      - This is a Bidirectionally predictive-coded picture.
	(这是一幅双向预测编码 (Bidirectionally predictive-coded) 图像。)
    * - ``V4L2_ENC_IDX_FRAME_MASK``
      - 0x0F
      - *AND* the flags field with this mask to obtain the picture coding
	type.
	(把 flags 字段与此掩码相与 (AND)，以获得图像编码类型。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
