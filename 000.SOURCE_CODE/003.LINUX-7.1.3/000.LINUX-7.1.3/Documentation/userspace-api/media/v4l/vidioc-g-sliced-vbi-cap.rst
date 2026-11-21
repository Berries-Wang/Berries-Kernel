.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_SLICED_VBI_CAP:

***************************************************
ioctl VIDIOC_G_SLICED_VBI_CAP(查询 sliced VBI 能力)
***************************************************

Name(名称)
==========

VIDIOC_G_SLICED_VBI_CAP - Query sliced VBI capabilities
(VIDIOC_G_SLICED_VBI_CAP - 查询切片式 VBI 能力)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_SLICED_VBI_CAP

``int ioctl(int fd, VIDIOC_G_SLICED_VBI_CAP, struct v4l2_sliced_vbi_cap *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_sliced_vbi_cap`.
    (指向结构体 :c:type:`v4l2_sliced_vbi_cap` 的指针。)

Description(描述)
=================

To find out which data services are supported by a sliced VBI capture or
output device, applications initialize the ``type`` field of a struct
:c:type:`v4l2_sliced_vbi_cap`, clear the
``reserved`` array and call the :ref:`VIDIOC_G_SLICED_VBI_CAP <VIDIOC_G_SLICED_VBI_CAP>` ioctl. The
driver fills in the remaining fields or returns an ``EINVAL`` error code if
the sliced VBI API is unsupported or ``type`` is invalid.
(要查明切片式 VBI 采集或输出设备支持哪些数据服务，应用程序需初始化结构体 :c:type:`v4l2_sliced_vbi_cap` 的 ``type`` 字段，清空 ``reserved`` 数组，然后调用 :ref:`VIDIOC_G_SLICED_VBI_CAP <VIDIOC_G_SLICED_VBI_CAP>` ioctl。驱动会填入其余字段；如果不支持 sliced VBI API 或 ``type`` 无效，则返回 ``EINVAL`` 错误码。)

.. note::

   The ``type`` field was added, and the ioctl changed from read-only
   to write-read, in Linux 2.6.19.
   (``type`` 字段是在 Linux 2.6.19 中新增的，该 ioctl 也随之从只读改为写-读。)

.. c:type:: v4l2_sliced_vbi_cap

.. tabularcolumns:: |p{1.4cm}|p{4.4cm}|p{4.5cm}|p{3.6cm}|p{3.6cm}|

.. flat-table:: struct v4l2_sliced_vbi_cap
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 3 2 2 2

    * - __u16
      - ``service_set``
      - :cspan:`2` A set of all data services supported by the driver.

	Equal to the union of all elements of the ``service_lines`` array.
        (驱动支持的全部数据服务的集合，等于 ``service_lines`` 数组所有元素的并集。)
    * - __u16
      - ``service_lines``\ [2][24]
      - :cspan:`2` Each element of this array contains a set of data
	services the hardware can look for or insert into a particular
	scan line. Data services are defined in :ref:`vbi-services`.
	Array indices map to ITU-R line numbers\ [#f1]_ as follows:
        (该数组的每个元素包含硬件可以在特定扫描行上查找或插入的一组数据服务。数据服务在 vbi-services 一节中定义。数组下标到 ITU-R 行号的映射如下：)
    * -
      -
      - Element(元素)
      - 525 line systems(525 行系统)
      - 625 line systems(625 行系统)
    * -
      -
      - ``service_lines``\ [0][1]
      - 1
      - 1
    * -
      -
      - ``service_lines``\ [0][23]
      - 23
      - 23
    * -
      -
      - ``service_lines``\ [1][1]
      - 264
      - 314
    * -
      -
      - ``service_lines``\ [1][23]
      - 286
      - 336
    * -
    * -
      -
      - :cspan:`2` The number of VBI lines the hardware can capture or
	output per frame, or the number of services it can identify on a
	given line may be limited. For example on PAL line 16 the hardware
	may be able to look for a VPS or Teletext signal, but not both at
	the same time. Applications can learn about these limits using the
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl as described in
	:ref:`sliced`.
        (硬件每帧能够采集或输出的 VBI 行数，或它在给定行上能够识别的服务数量可能是受限的。例如在 PAL 第 16 行上，硬件或许能查找 VPS 或图文电视信号，但不能同时查找两者。应用可按 sliced 一节的说明，使用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 了解这些限制。)
    * -
    * -
      -
      - :cspan:`2` Drivers must set ``service_lines`` [0][0] and
	``service_lines``\ [1][0] to zero.
        (驱动必须把 ``service_lines`` [0][0] 和 ``service_lines``\ [1][0] 设为零。)
    * - __u32
      - ``type``
      - Type of the data stream, see :c:type:`v4l2_buf_type`. Should be
	``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE`` or
	``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT``.
        (数据流的类型，见 :c:type:`v4l2_buf_type`。应为 ``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE`` 或 ``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT`` 。)
    * - __u32
      - ``reserved``\ [3]
      - :cspan:`2` This array is reserved for future extensions.

	Applications and drivers must set it to zero.
        (该数组保留用于未来扩展。应用与驱动必须将其置零。)

.. [#f1]

   See also :ref:`vbi-525` and :ref:`vbi-625`.
   (另请参阅 :ref:`vbi-525` 与 :ref:`vbi-625`。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{3.9cm}|p{1.0cm}|p{2.0cm}|p{3.0cm}|p{7.0cm}|

.. _vbi-services:

.. flat-table:: Sliced VBI services
    :header-rows:  1
    :stub-columns: 0
    :widths:       2 1 1 2 2

    * - Symbol(符号)
      - Value(值)
      - Reference(参考)
      - Lines, usually(通常所在行)
      - Payload(载荷)
    * - ``V4L2_SLICED_TELETEXT_B`` (Teletext System B)
      - 0x0001
      - :ref:`ets300706`,

	:ref:`itu653`
      - PAL/SECAM line 7-22, 320-335 (second field 7-22)
        (PAL/SECAM 行 7-22、320-335，第二场为 7-22)
      - Last 42 of the 45 byte Teletext packet, that is without clock
	run-in and framing code, lsb first transmitted.
        (45 字节图文电视分组的最后 42 字节，即时钟运行前导与帧码之后的部分，按 lsb 先传输。)
    * - ``V4L2_SLICED_VPS``
      - 0x0400
      - :ref:`ets300231`
      - PAL line 16
        (PAL 行 16)
      - Byte number 3 to 15 according to Figure 9 of ETS 300 231, lsb
	first transmitted.
        (按 ETS 300 231 图 9 的第 3 至第 15 字节，按 lsb 先传输。)
    * - ``V4L2_SLICED_CAPTION_525``
      - 0x1000
      - :ref:`cea608`
      - NTSC line 21, 284 (second field 21)
        (NTSC 行 21、284，第二场为 21)
      - Two bytes in transmission order, including parity bit, lsb first
	transmitted.
        (按传输顺序排列的两个字节，含奇偶校验位，按 lsb 先传输。)
    * - ``V4L2_SLICED_WSS_625``
      - 0x4000
      - :ref:`en300294`,

	:ref:`itu1119`
      - PAL/SECAM line 23
        (PAL/SECAM 行 23)
      - See :ref:`v4l2-sliced-vbi-cap-wss-625-payload` below.
        (载荷见下文 v4l2-sliced-vbi-cap-wss-625-payload 一节。)
    * - ``V4L2_SLICED_VBI_525``
      - 0x1000
      - :cspan:`2` Set of services applicable to 525 line systems.
        (适用于 525 行系统的服务集合。)
    * - ``V4L2_SLICED_VBI_625``
      - 0x4401
      - :cspan:`2` Set of services applicable to 625 line systems.
        (适用于 625 行系统的服务集合。)


.. raw:: latex

    \normalsize

.. _v4l2-sliced-vbi-cap-wss-625-payload:

V4L2_SLICED_VBI_CAP WSS_625 payload(WSS_625 载荷格式)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The payload for ``V4L2_SLICED_WSS_625`` is:
(``V4L2_SLICED_WSS_625`` 的载荷如下：)

	    +-----+------------------+-----------------------+
	    |Byte |        0         |           1           |
	    +-----+--------+---------+-----------+-----------+
	    |     | msb    | lsb     | msb       | lsb       |
	    |     +-+-+-+--+--+-+-+--+--+-+--+---+---+--+-+--+
	    | Bit |7|6|5|4 | 3|2|1|0 | x|x|13|12 | 11|10|9|8 |
	    +-----+-+-+-+--+--+-+-+--+--+-+--+---+---+--+-+--+


Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并将 ``errno`` 变量设置为相应值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The value in the ``type`` field is wrong.
    (``type`` 字段中的值错误。)
