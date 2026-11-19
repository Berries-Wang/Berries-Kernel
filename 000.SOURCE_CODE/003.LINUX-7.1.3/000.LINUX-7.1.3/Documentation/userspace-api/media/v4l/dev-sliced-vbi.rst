.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _sliced:

********************************************
Sliced VBI Data Interface(切片 VBI 数据接口)
********************************************

VBI stands for Vertical Blanking Interval, a gap in the sequence of
lines of an analog video signal. During VBI no picture information is
transmitted, allowing some time while the electron beam of a cathode ray
tube TV returns to the top of the screen.
(VBI 指垂直消隐间隔 (Vertical Blanking Interval)，即模拟视频信号行序列中的一段空隙。在 VBI 期间不传输图像信息，这段时间留给阴极射线管电视的电子束返回屏幕顶部。)

Sliced VBI devices use hardware to demodulate data transmitted in the
VBI. V4L2 drivers shall *not* do this by software, see also the
:ref:`raw VBI interface <raw-vbi>`. The data is passed as short
packets of fixed size, covering one scan line each. The number of
packets per video frame is variable.
(切片 VBI 设备用硬件解调在 VBI 中传输的数据。V4L2 驱动 *不应* 用软件来做这件事，另见 :ref:`raw VBI interface <raw-vbi>`。数据以固定长度的短数据包传递，每个数据包覆盖一条扫描行。每个视频帧的数据包数量是可变的。)

Sliced VBI capture and output devices are accessed through the same
character special files as raw VBI devices. When a driver supports both
interfaces, the default function of a ``/dev/vbi`` device is *raw* VBI
capturing or output, and the sliced VBI function is only available after
calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl as defined
below. Likewise a ``/dev/video`` device may support the sliced VBI API,
however the default function here is video capturing or output.
Different file descriptors must be used to pass raw and sliced VBI data
simultaneously, if this is supported by the driver.
(切片 VBI 采集与输出设备通过与 raw VBI 设备相同的字符特殊文件访问。当驱动同时支持两种接口时，``/dev/vbi`` 设备的默认功能是 *raw* VBI 采集或输出，切片 VBI 功能只有在按下文定义调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 之后才可用。同样，``/dev/video`` 设备也可能支持切片 VBI API，但其默认功能是视频采集或输出。如果驱动支持，同时传递 raw 和切片 VBI 数据必须使用不同的文件描述符。)

Querying Capabilities(能力查询)
===============================

Devices supporting the sliced VBI capturing or output API set the
``V4L2_CAP_SLICED_VBI_CAPTURE`` or ``V4L2_CAP_SLICED_VBI_OUTPUT`` flag
respectively, in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. At least one of the
read/write or streaming :ref:`I/O methods <io>` must be
supported. Sliced VBI devices may have a tuner or modulator.
(支持切片 VBI 采集或输出 API 的设备会分别在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置 ``V4L2_CAP_SLICED_VBI_CAPTURE`` 或 ``V4L2_CAP_SLICED_VBI_OUTPUT`` 标志。必须至少支持读写或流式 :ref:`I/O methods <io>` 之一。切片 VBI 设备可能有调谐器或调制器。)

Supplemental Functions(辅助功能)
================================

Sliced VBI devices shall support :ref:`video input or output <video>`
and :ref:`tuner or modulator <tuner>` ioctls if they have these
capabilities, and they may support :ref:`control` ioctls.
The :ref:`video standard <standard>` ioctls provide information vital
to program a sliced VBI device, therefore must be supported.
(切片 VBI 设备如果具备相应能力，就应支持 :ref:`video input or output <video>` 和 :ref:`tuner or modulator <tuner>` ioctl，并可以支持 :ref:`control` ioctl。:ref:`video standard <standard>` ioctl 提供对切片 VBI 设备编程至关重要的信息，因此必须支持。)

.. _sliced-vbi-format-negotiation:

Sliced VBI Format Negotiation(切片 VBI 格式协商)
================================================

To find out which data services are supported by the hardware
applications can call the
:ref:`VIDIOC_G_SLICED_VBI_CAP <VIDIOC_G_SLICED_VBI_CAP>` ioctl.
All drivers implementing the sliced VBI interface must support this
ioctl. The results may differ from those of the
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl when the number of VBI
lines the hardware can capture or output per frame, or the number of
services it can identify on a given line are limited. For example on PAL
line 16 the hardware may be able to look for a VPS or Teletext signal,
but not both at the same time.
(要了解硬件支持哪些数据服务，应用可以调用 :ref:`VIDIOC_G_SLICED_VBI_CAP <VIDIOC_G_SLICED_VBI_CAP>` ioctl。所有实现切片 VBI 接口的驱动都必须支持该 ioctl。当硬件每帧能采集或输出的 VBI 行数、或它在给定行上能识别的服务数受限时，其结果可能与 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 的结果不同。例如在 PAL 第 16 行上，硬件也许能查找 VPS 或图文电视信号，但不能同时查找两者。)

To determine the currently selected services applications set the
``type`` field of struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE`` or
``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT``, and the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl fills the ``fmt.sliced``
member, a struct
:c:type:`v4l2_sliced_vbi_format`.
(要查询当前选择的服务，应用把 struct :c:type:`v4l2_format` 的 ``type`` 字段设置为 ``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE`` 或 ``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT``，:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl 会填充 ``fmt.sliced`` 成员，即一个 struct :c:type:`v4l2_sliced_vbi_format`。)

Applications can request different parameters by initializing or
modifying the ``fmt.sliced`` member and calling the
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with a pointer to the
struct :c:type:`v4l2_format` structure.
(应用可以初始化或修改 ``fmt.sliced`` 成员，并用指向 struct :c:type:`v4l2_format` 结构体的指针调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 来请求不同的参数。)

The sliced VBI API is more complicated than the raw VBI API because the
hardware must be told which VBI service to expect on each scan line. Not
all services may be supported by the hardware on all lines (this is
especially true for VBI output where Teletext is often unsupported and
other services can only be inserted in one specific line). In many
cases, however, it is sufficient to just set the ``service_set`` field
to the required services and let the driver fill the ``service_lines``
array according to hardware capabilities. Only if more precise control
is needed should the programmer set the ``service_lines`` array
explicitly.
(切片 VBI API 比 raw VBI API 更复杂，因为必须告诉硬件在每条扫描行上预期哪种 VBI 服务。硬件并非在所有行上都支持所有服务 (对 VBI 输出尤其如此：图文电视常常不受支持，而其他服务只能插入到某一条特定行)。不过在许多情况下，只需把 ``service_set`` 字段设置为所需的服务，让驱动按硬件能力填充 ``service_lines`` 数组即可。只有需要更精确控制时，程序员才应显式设置 ``service_lines`` 数组。)

The :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl modifies the parameters
according to hardware capabilities. When the driver allocates resources
at this point, it may return an ``EBUSY`` error code if the required
resources are temporarily unavailable. Other resource allocation points
which may return ``EBUSY`` can be the
:ref:`VIDIOC_STREAMON` ioctl and the first
:c:func:`read()`, :c:func:`write()` and
:c:func:`select()` call.
(:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 会按硬件能力修改参数。如果驱动此时分配资源，那么在所需资源暂时不可用时，它可能返回 ``EBUSY`` 错误码。其他可能返回 ``EBUSY`` 的资源分配点还有 :ref:`VIDIOC_STREAMON` ioctl 以及首次 :c:func:`read()` 、 :c:func:`write()` 和 :c:func:`select()` 调用。)

.. c:type:: v4l2_sliced_vbi_format

struct v4l2_sliced_vbi_format
-----------------------------

.. raw:: latex

    \begingroup
    \scriptsize
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{.85cm}|p{3.3cm}|p{4.45cm}|p{4.45cm}|p{4.45cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 3 2 2 2

    * - __u16
      - ``service_set``
      - :cspan:`2`

	If ``service_set`` is non-zero when passed with
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` or
	:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`, the ``service_lines``
	array will be filled by the driver according to the services
	specified in this field. For example, if ``service_set`` is
	initialized with ``V4L2_SLICED_TELETEXT_B | V4L2_SLICED_WSS_625``,
	a driver for the cx25840 video decoder sets lines 7-22 of both
	fields [#f1]_ to ``V4L2_SLICED_TELETEXT_B`` and line 23 of the first
	field to ``V4L2_SLICED_WSS_625``. If ``service_set`` is set to
	zero, then the values of ``service_lines`` will be used instead.

	(如果传入 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 或 :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` 时 ``service_set`` 非零，驱动会按本字段指定的服务集填充 ``service_lines`` 数组。例如，若 ``service_set`` 初始化为 ``V4L2_SLICED_TELETEXT_B | V4L2_SLICED_WSS_625``，cx25840 视频解码器驱动会把两场的 7-22 行设置为 ``V4L2_SLICED_TELETEXT_B``，并把第一场的第 23 行设置为 ``V4L2_SLICED_WSS_625``。如果 ``service_set`` 置零，则改用 ``service_lines`` 的值。)

	On return the driver sets this field to the union of all elements
	of the returned ``service_lines`` array. It may contain less
	services than requested, perhaps just one, if the hardware cannot
	handle more services simultaneously. It may be empty (zero) if
	none of the requested services are supported by the hardware.
	(返回时，驱动把本字段设置为返回的 ``service_lines`` 数组所有元素的并集。如果硬件无法同时处理更多服务，它包含的服务可能少于请求的，甚至只有一个。如果硬件不支持任何请求的服务，它可能为空 (零)。)
    * - __u16
      - ``service_lines``\ [2][24]
      - :cspan:`2`

	Applications initialize this array with sets of data services the
	driver shall look for or insert on the respective scan line.
	Subject to hardware capabilities drivers return the requested set,
	a subset, which may be just a single service, or an empty set.
	When the hardware cannot handle multiple services on the same line
	the driver shall choose one. No assumptions can be made on which
	service the driver chooses.
	(应用初始化这个数组时，给出驱动应在相应扫描行上查找或插入的数据服务集合。在硬件能力允许的范围内，驱动返回请求的集合、其子集 (可能只有一个服务) 或空集。当硬件无法在同一行上处理多个服务时，由驱动选择其一，且无法对驱动会选哪个服务做任何假设。)

	Data services are defined in :ref:`vbi-services2`. Array indices
	map to ITU-R line numbers\ [#f2]_ as follows:
	(数据服务在 :ref:`vbi-services2` 中定义。数组下标与 ITU-R 行号的对应关系如下：)
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
      -
      - :cspan:`2` Drivers must set ``service_lines`` [0][0] and
	``service_lines``\ [1][0] to zero. The
	``V4L2_VBI_ITU_525_F1_START``, ``V4L2_VBI_ITU_525_F2_START``,
	``V4L2_VBI_ITU_625_F1_START`` and ``V4L2_VBI_ITU_625_F2_START``
	defines give the start line numbers for each field for each 525 or
	625 line format as a convenience. Don't forget that ITU line
	numbering starts at 1, not 0.
	(驱动必须把 ``service_lines`` [0][0] 和 ``service_lines``\ [1][0] 设为零。``V4L2_VBI_ITU_525_F1_START``、``V4L2_VBI_ITU_525_F2_START``、``V4L2_VBI_ITU_625_F1_START`` 和 ``V4L2_VBI_ITU_625_F2_START`` 这几个定义便利地给出了每种 525 或 625 行格式各场的起始行号。别忘了 ITU 行号从 1 开始，而不是 0。)
    * - __u32
      - ``io_size``
      - :cspan:`2` Maximum number of bytes passed by one
	:c:func:`read()` or :c:func:`write()` call,
	and the buffer size in bytes for the
	:ref:`VIDIOC_QBUF` and
	:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. Drivers set this field
	to the size of struct
	:c:type:`v4l2_sliced_vbi_data` times the
	number of non-zero elements in the returned ``service_lines``
	array (that is the number of lines potentially carrying data).
	(一次 :c:func:`read()` 或 :c:func:`write()` 调用传递的最大字节数，也是 :ref:`VIDIOC_QBUF` 和 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl 的缓冲区字节大小。驱动把这个字段设置为 struct :c:type:`v4l2_sliced_vbi_data` 的大小乘以返回的 ``service_lines`` 数组中非零元素的个数 (即可能承载数据的行数)。)
    * - __u32
      - ``reserved``\ [2]
      - :cspan:`2` This array is reserved for future extensions.

	Applications and drivers must set it to zero.
	(此数组保留供未来扩展使用。应用和驱动必须把它置零。)

.. raw:: latex

    \endgroup

.. _vbi-services2:

Sliced VBI services(切片 VBI 服务)
----------------------------------

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{4.2cm}|p{1.1cm}|p{2.1cm}|p{2.0cm}|p{6.5cm}|

.. flat-table::
    :header-rows:  1
    :stub-columns: 0
    :widths:       2 1 1 2 2

    * - Symbol(符号)
      - Value(值)
      - Reference(参考)
      - Lines, usually(通常所在行)
      - Payload(载荷)
    * - ``V4L2_SLICED_TELETEXT_B`` (Teletext System B)(图文电视系统 B)
      - 0x0001
      - :ref:`ets300706`,

	:ref:`itu653`
      - PAL/SECAM line 7-22, 320-335 (second field 7-22)
        (PAL/SECAM 行 7-22、320-335 (第二场 7-22))
      - Last 42 of the 45 byte Teletext packet, that is without clock
	run-in and framing code, lsb first transmitted.
	(图文电视 45 字节包的最后 42 字节，即不含时钟插入 (clock run-in) 和帧码 (framing code)，按 lsb 在前传输。)
    * - ``V4L2_SLICED_VPS``(视频节目系统 VPS)
      - 0x0400
      - :ref:`ets300231`
      - PAL line 16
        (PAL 第 16 行)
      - Byte number 3 to 15 according to Figure 9 of ETS 300 231, lsb
	first transmitted.
	(按 ETS 300 231 图 9 的字节 3 到 15，lsb 在前传输。)
    * - ``V4L2_SLICED_CAPTION_525``(隐藏式字幕)
      - 0x1000
      - :ref:`cea608`
      - NTSC line 21, 284 (second field 21)
        (NTSC 行 21、284 (第二场 21))
      - Two bytes in transmission order, including parity bit, lsb first
	transmitted.
	(按传输顺序的两个字节，含奇偶校验位，lsb 在前传输。)
    * - ``V4L2_SLICED_WSS_625``(宽屏信号 WSS)
      - 0x4000
      - :ref:`itu1119`,

	:ref:`en300294`
      - PAL/SECAM line 23
        (PAL/SECAM 第 23 行)
      -  See :ref:`v4l2-sliced-wss-625-payload` below.
         (见下文 :ref:`v4l2-sliced-wss-625-payload`。)
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

Drivers may return an ``EINVAL`` error code when applications attempt to
read or write data without prior format negotiation, after switching the
video standard (which may invalidate the negotiated VBI parameters) and
after switching the video input (which may change the video standard as
a side effect). The :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl may
return an ``EBUSY`` error code when applications attempt to change the
format while i/o is in progress (between a
:ref:`VIDIOC_STREAMON` and
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` call, and after the first
:c:func:`read()` or :c:func:`write()` call).
(当应用在没有事先格式协商的情况下、在切换视频制式之后 (这可能使协商好的 VBI 参数失效)、或在切换视频输入之后 (这可能作为副作用改变视频制式) 试图读写数据时，驱动可以返回 ``EINVAL`` 错误码。当应用在 i/o 进行期间 (在 :ref:`VIDIOC_STREAMON` 与 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 调用之间，以及首次 :c:func:`read()` 或 :c:func:`write()` 调用之后) 试图更改格式时，:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 可能返回 ``EBUSY`` 错误码。)

.. _v4l2-sliced-wss-625-payload:

V4L2_SLICED_WSS_625 payload(V4L2_SLICED_WSS_625 载荷)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The payload for ``V4L2_SLICED_WSS_625`` is:
( ``V4L2_SLICED_WSS_625`` 的载荷如下：)

           +-----+------------------+-----------------------+
	   |Byte |        0         |           1           |
           +-----+--------+---------+-----------+-----------+
	   |     | msb    | lsb     | msb       | lsb       |
           |     +-+-+-+--+--+-+-+--+--+-+--+---+---+--+-+--+
	   | Bit |7|6|5|4 | 3|2|1|0 | x|x|13|12 | 11|10|9|8 |
           +-----+-+-+-+--+--+-+-+--+--+-+--+---+---+--+-+--+

Reading and writing sliced VBI data(读写切片 VBI 数据)
======================================================

A single :c:func:`read()` or :c:func:`write()`
call must pass all data belonging to one video frame. That is an array
of struct :c:type:`v4l2_sliced_vbi_data` structures with one or
more elements and a total size not exceeding ``io_size`` bytes. Likewise
in streaming I/O mode one buffer of ``io_size`` bytes must contain data
of one video frame. The ``id`` of unused
struct :c:type:`v4l2_sliced_vbi_data` elements must be zero.
(一次 :c:func:`read()` 或 :c:func:`write()` 调用必须传递属于同一个视频帧的全部数据。也就是一个由 struct :c:type:`v4l2_sliced_vbi_data` 结构体组成的数组，含一个或多个元素，总大小不超过 ``io_size`` 字节。同样，在流式 I/O 模式下，一个 ``io_size`` 字节的缓冲区必须包含一个视频帧的数据。未使用的 struct :c:type:`v4l2_sliced_vbi_data` 元素的 ``id`` 必须为零。)

.. c:type:: v4l2_sliced_vbi_data

struct v4l2_sliced_vbi_data(v4l2_sliced_vbi_data 结构体)
--------------------------------------------------------

.. tabularcolumns:: |p{1.2cm}|p{2.2cm}|p{13.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - __u32
      - ``id``
      - A flag from :ref:`vbi-services` identifying the type of data in
	this packet. Only a single bit must be set. When the ``id`` of a
	captured packet is zero, the packet is empty and the contents of
	other fields are undefined. Applications shall ignore empty
	packets. When the ``id`` of a packet for output is zero the
	contents of the ``data`` field are undefined and the driver must
	no longer insert data on the requested ``field`` and ``line``.
	(来自 :ref:`vbi-services` 的一个标志，用于标识本数据包中数据的类型。只能置一个比特。若捕获到的数据包 ``id`` 为零，则该包为空包，其他字段的内容未定义，应用应忽略空包。若输出数据包的 ``id`` 为零，则 ``data`` 字段的内容未定义，驱动不得再在请求的 ``field`` 和 ``line`` 上插入数据。)
    * - __u32
      - ``field``
      - The video field number this data has been captured from, or shall
	be inserted at. ``0`` for the first field, ``1`` for the second
	field.
	(本数据捕获自、或应插入到的视频场号。第一场为 ``0``，第二场为 ``1``。)
    * - __u32
      - ``line``
      - The field (as opposed to frame) line number this data has been
	captured from, or shall be inserted at. See :ref:`vbi-525` and
	:ref:`vbi-625` for valid values. Sliced VBI capture devices can
	set the line number of all packets to ``0`` if the hardware cannot
	reliably identify scan lines. The field number must always be
	valid.
	(本数据捕获自、或应插入到的场内 (相对于帧) 行号。有效取值见 :ref:`vbi-525` 和 :ref:`vbi-625`。如果硬件无法可靠识别扫描行，切片 VBI 捕获设备可以把所有数据包的行号都设为 ``0``。场号必须始终有效。)
    * - __u32
      - ``reserved``
      - This field is reserved for future extensions. Applications and
	drivers must set it to zero.
	(此字段保留供未来扩展使用。应用和驱动必须把它置零。)
    * - __u8
      - ``data``\ [48]
      - The packet payload. See :ref:`vbi-services` for the contents and
	number of bytes passed for each data type. The contents of padding
	bytes at the end of this array are undefined, drivers and
	applications shall ignore them.
	(数据包载荷。各数据类型传递的内容与字节数见 :ref:`vbi-services`。本数组末尾填充字节的内容未定义，驱动和应用应忽略它们。)

Packets are always passed in ascending line number order, without
duplicate line numbers. The :c:func:`write()` function and
the :ref:`VIDIOC_QBUF` ioctl must return an ``EINVAL``
error code when applications violate this rule. They must also return an
EINVAL error code when applications pass an incorrect field or line
number, or a combination of ``field``, ``line`` and ``id`` which has not
been negotiated with the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` or
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl. When the line numbers are
unknown the driver must pass the packets in transmitted order. The
driver can insert empty packets with ``id`` set to zero anywhere in the
packet array.
(数据包总是按行号升序传递，且行号不重复。当应用违反这一规则时，:c:func:`write()` 函数和 :ref:`VIDIOC_QBUF` ioctl 必须返回 ``EINVAL`` 错误码。当应用传递错误的场号或行号、或传递未经 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 或 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商过的 ``field``、``line`` 与 ``id`` 组合时，它们也必须返回 EINVAL 错误码。当行号未知时，驱动必须按传输顺序传递数据包。驱动可以在数据包数组的任意位置插入 ``id`` 置零的空数据包。)

To assure synchronization and to distinguish from frame dropping, when a
captured frame does not carry any of the requested data services drivers
must pass one or more empty packets. When an application fails to pass
VBI data in time for output, the driver must output the last VPS and WSS
packet again, and disable the output of Closed Caption and Teletext
data, or output data which is ignored by Closed Caption and Teletext
decoders.
(为了保证同步并与丢帧相区分，当捕获的帧不携带任何请求的数据服务时，驱动必须传递一个或多个空数据包。当应用未能及时传递用于输出的 VBI 数据时，驱动必须再次输出最后的 VPS 和 WSS 数据包，并禁用隐藏式字幕 (Closed Caption) 和图文电视数据的输出，或者输出会被隐藏式字幕和图文电视解码器忽略的数据。)

A sliced VBI device may support :ref:`read/write <rw>` and/or
streaming (:ref:`memory mapping <mmap>` and/or
:ref:`user pointer <userp>`) I/O. The latter bears the possibility of
synchronizing video and VBI data by using buffer timestamps.
(切片 VBI 设备可以支持 :ref:`read/write <rw>` 和/或流式 (:ref:`memory mapping <mmap>` 和/或 :ref:`user pointer <userp>`) I/O。后者可以通过缓冲区时间戳来同步视频与 VBI 数据。)

Sliced VBI Data in MPEG Streams(MPEG 流中的切片 VBI 数据)
=========================================================

If a device can produce an MPEG output stream, it may be capable of
providing
:ref:`negotiated sliced VBI services <sliced-vbi-format-negotiation>`
as data embedded in the MPEG stream. Users or applications control this
sliced VBI data insertion with the
:ref:`V4L2_CID_MPEG_STREAM_VBI_FMT <v4l2-mpeg-stream-vbi-fmt>`
control.
(如果设备能产生 MPEG 输出流，它可能有能力把 :ref:`negotiated sliced VBI services <sliced-vbi-format-negotiation>` 作为内嵌在 MPEG 流中的数据提供。用户或应用通过 :ref:`V4L2_CID_MPEG_STREAM_VBI_FMT <v4l2-mpeg-stream-vbi-fmt>` 控制项来控制这种切片 VBI 数据的插入。)

If the driver does not provide the
:ref:`V4L2_CID_MPEG_STREAM_VBI_FMT <v4l2-mpeg-stream-vbi-fmt>`
control, or only allows that control to be set to
:ref:`V4L2_MPEG_STREAM_VBI_FMT_NONE <v4l2-mpeg-stream-vbi-fmt>`,
then the device cannot embed sliced VBI data in the MPEG stream.
(如果驱动不提供 :ref:`V4L2_CID_MPEG_STREAM_VBI_FMT <v4l2-mpeg-stream-vbi-fmt>` 控制项，或只允许把该控制项设置为 :ref:`V4L2_MPEG_STREAM_VBI_FMT_NONE <v4l2-mpeg-stream-vbi-fmt>`，则设备无法在 MPEG 流中内嵌切片 VBI 数据。)

The
:ref:`V4L2_CID_MPEG_STREAM_VBI_FMT <v4l2-mpeg-stream-vbi-fmt>`
control does not implicitly set the device driver to capture nor cease
capturing sliced VBI data. The control only indicates to embed sliced
VBI data in the MPEG stream, if an application has negotiated sliced VBI
service be captured.
(:ref:`V4L2_CID_MPEG_STREAM_VBI_FMT <v4l2-mpeg-stream-vbi-fmt>` 控制项不会隐式地让设备驱动开始或停止捕获切片 VBI 数据。该控制项只表示把切片 VBI 数据内嵌进 MPEG 流——前提是应用已协商好要捕获切片 VBI 服务。)

It may also be the case that a device can embed sliced VBI data in only
certain types of MPEG streams: for example in an MPEG-2 PS but not an
MPEG-2 TS. In this situation, if sliced VBI data insertion is requested,
the sliced VBI data will be embedded in MPEG stream types when
supported, and silently omitted from MPEG stream types where sliced VBI
data insertion is not supported by the device.
(设备也可能只能在某些类型的 MPEG 流中内嵌切片 VBI 数据：例如能内嵌进 MPEG-2 PS 却不能内嵌进 MPEG-2 TS。在这种情况下，如果请求了插入切片 VBI 数据，则在支持的 MPEG 流类型中会内嵌切片 VBI 数据，而在设备不支持插入切片 VBI 数据的 MPEG 流类型中会被静默省略。)

The following subsections specify the format of the embedded sliced VBI
data.
(以下小节规定内嵌切片 VBI 数据的格式。)

MPEG Stream Embedded, Sliced VBI Data Format: NONE(MPEG 流内嵌切片 VBI 数据格式：NONE)
--------------------------------------------------------------------------------------

The
:ref:`V4L2_MPEG_STREAM_VBI_FMT_NONE <v4l2-mpeg-stream-vbi-fmt>`
embedded sliced VBI format shall be interpreted by drivers as a control
to cease embedding sliced VBI data in MPEG streams. Neither the device
nor driver shall insert "empty" embedded sliced VBI data packets in the
MPEG stream when this format is set. No MPEG stream data structures are
specified for this format.
(:ref:`V4L2_MPEG_STREAM_VBI_FMT_NONE <v4l2-mpeg-stream-vbi-fmt>` 内嵌切片 VBI 格式应被驱动解释为停止在 MPEG 流中内嵌切片 VBI 数据的控制指令。设置此格式时，设备与驱动都不得在 MPEG 流中插入"空"的内嵌切片 VBI 数据包。此格式没有规定任何 MPEG 流数据结构。)

MPEG Stream Embedded, Sliced VBI Data Format: IVTV(MPEG 流内嵌切片 VBI 数据格式：IVTV)
--------------------------------------------------------------------------------------

The
:ref:`V4L2_MPEG_STREAM_VBI_FMT_IVTV <v4l2-mpeg-stream-vbi-fmt>`
embedded sliced VBI format, when supported, indicates to the driver to
embed up to 36 lines of sliced VBI data per frame in an MPEG-2 *Private
Stream 1 PES* packet encapsulated in an MPEG-2 *Program Pack* in the
MPEG stream.
(:ref:`V4L2_MPEG_STREAM_VBI_FMT_IVTV <v4l2-mpeg-stream-vbi-fmt>` 内嵌切片 VBI 格式在受支持时，表示驱动要把每帧最多 36 行的切片 VBI 数据内嵌进 MPEG 流中一个封装在 MPEG-2 *Program Pack* 内的 MPEG-2 *Private Stream 1 PES* 数据包。)

*Historical context*: This format specification originates from a
custom, embedded, sliced VBI data format used by the ``ivtv`` driver.
This format has already been informally specified in the kernel sources
in the file ``Documentation/userspace-api/media/drivers/cx2341x-uapi.rst`` . The
maximum size of the payload and other aspects of this format are driven
by the CX23415 MPEG decoder's capabilities and limitations with respect
to extracting, decoding, and displaying sliced VBI data embedded within
an MPEG stream.
(*历史背景*：本格式规范源自 ``ivtv`` 驱动使用的一种自定义内嵌切片 VBI 数据格式。该格式此前已在内核源码的 ``Documentation/userspace-api/media/drivers/cx2341x-uapi.rst`` 文件中被非正式地规定。载荷的最大尺寸及本格式的其他方面，取决于 CX23415 MPEG 解码器在提取、解码和显示内嵌于 MPEG 流中的切片 VBI 数据方面的能力与限制。)

This format's use is *not* exclusive to the ``ivtv`` driver *nor*
exclusive to CX2341x devices, as the sliced VBI data packet insertion
into the MPEG stream is implemented in driver software. At least the
``cx18`` driver provides sliced VBI data insertion into an MPEG-2 PS in
this format as well.
(此格式的使用并非 ``ivtv`` 驱动独有，也 *不* 是 CX2341x 设备独有，因为把切片 VBI 数据包插入 MPEG 流是在驱动软件中实现的。至少 ``cx18`` 驱动也以这种格式向 MPEG-2 PS 提供切片 VBI 数据插入。)

The following definitions specify the payload of the MPEG-2 *Private
Stream 1 PES* packets that contain sliced VBI data when
:ref:`V4L2_MPEG_STREAM_VBI_FMT_IVTV <v4l2-mpeg-stream-vbi-fmt>`
is set. (The MPEG-2 *Private Stream 1 PES* packet header and
encapsulating MPEG-2 *Program Pack* header are not detailed here. Please
refer to the MPEG-2 specifications for details on those packet headers.)
(以下定义规定了设置 :ref:`V4L2_MPEG_STREAM_VBI_FMT_IVTV <v4l2-mpeg-stream-vbi-fmt>` 时，包含切片 VBI 数据的 MPEG-2 *Private Stream 1 PES* 数据包的载荷。(MPEG-2 *Private Stream 1 PES* 包头及其外层 MPEG-2 *Program Pack* 包头在此不详述。这些包头结构的细节请参阅 MPEG-2 规范。))

The payload of the MPEG-2 *Private Stream 1 PES* packets that contain
sliced VBI data is specified by struct
:c:type:`v4l2_mpeg_vbi_fmt_ivtv`. The
payload is variable length, depending on the actual number of lines of
sliced VBI data present in a video frame. The payload may be padded at
the end with unspecified fill bytes to align the end of the payload to a
4-byte boundary. The payload shall never exceed 1552 bytes (2 fields
with 18 lines/field with 43 bytes of data/line and a 4 byte magic
number).
(包含切片 VBI 数据的 MPEG-2 *Private Stream 1 PES* 数据包的载荷由 struct :c:type:`v4l2_mpeg_vbi_fmt_ivtv` 规定。载荷是变长的，取决于视频帧中实际存在的切片 VBI 数据行数。载荷末尾可以用未指定的填充字节补齐，使载荷末尾对齐到 4 字节边界。载荷绝不能超过 1552 字节 (2 场 × 每场 18 行 × 每行 43 字节数据，外加 4 字节魔数)。)

.. c:type:: v4l2_mpeg_vbi_fmt_ivtv

struct v4l2_mpeg_vbi_fmt_ivtv(v4l2_mpeg_vbi_fmt_ivtv 结构体)
------------------------------------------------------------

.. tabularcolumns:: |p{4.2cm}|p{2.0cm}|p{11.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``magic``\ [4]
      - A "magic" constant from :ref:`v4l2-mpeg-vbi-fmt-ivtv-magic` that
	indicates this is a valid sliced VBI data payload and also
	indicates which member of the anonymous union, ``itv0`` or
	``ITV0``, to use for the payload data.
	(来自 :ref:`v4l2-mpeg-vbi-fmt-ivtv-magic` 的"魔数"常量，表明这是有效的切片 VBI 数据载荷，并指明匿名 union 中使用哪个成员 (``itv0`` 或 ``ITV0``) 来承载数据。)
    * - union {
      - (anonymous)
    * - struct :c:type:`v4l2_mpeg_vbi_itv0`
      - ``itv0``
      - The primary form of the sliced VBI data payload that contains
	anywhere from 1 to 35 lines of sliced VBI data. Line masks are
	provided in this form of the payload indicating which VBI lines
	are provided.
	(切片 VBI 数据载荷的主要形式，可包含 1 到 35 行切片 VBI 数据。这种载荷形式提供行掩码，指明提供了哪些 VBI 行。)
    * - struct :ref:`v4l2_mpeg_vbi_ITV0 <v4l2-mpeg-vbi-itv0-1>`
      - ``ITV0``
      - An alternate form of the sliced VBI data payload used when 36
	lines of sliced VBI data are present. No line masks are provided
	in this form of the payload; all valid line mask bits are
	implicitly set.
	(切片 VBI 数据载荷的另一种形式，在包含 36 行切片 VBI 数据时使用。这种载荷形式不提供行掩码；所有有效的行掩码位都隐含置位。)
    * - }
      -

.. _v4l2-mpeg-vbi-fmt-ivtv-magic:

Magic Constants for struct v4l2_mpeg_vbi_fmt_ivtv magic field(struct v4l2_mpeg_vbi_fmt_ivtv magic 字段的魔数常量)
-----------------------------------------------------------------------------------------------------------------

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. flat-table::
    :header-rows:  1
    :stub-columns: 0
    :widths:       3 1 4

    * - Defined Symbol(定义的符号)
      - Value(值)
      - Description(描述)
    * - ``V4L2_MPEG_VBI_IVTV_MAGIC0``
      - "itv0"
      - Indicates the ``itv0`` member of the union in struct
	:c:type:`v4l2_mpeg_vbi_fmt_ivtv` is
	valid.
	(表明 struct :c:type:`v4l2_mpeg_vbi_fmt_ivtv` 中 union 的 ``itv0`` 成员有效。)
    * - ``V4L2_MPEG_VBI_IVTV_MAGIC1``
      - "ITV0"
      - Indicates the ``ITV0`` member of the union in struct
	:c:type:`v4l2_mpeg_vbi_fmt_ivtv` is
	valid and that 36 lines of sliced VBI data are present.
	(表明 struct :c:type:`v4l2_mpeg_vbi_fmt_ivtv` 中 union 的 ``ITV0`` 成员有效，且包含 36 行切片 VBI 数据。)


.. c:type:: v4l2_mpeg_vbi_itv0

.. c:type:: v4l2_mpeg_vbi_ITV0

structs v4l2_mpeg_vbi_itv0 and v4l2_mpeg_vbi_ITV0(v4l2_mpeg_vbi_itv0 与 v4l2_mpeg_vbi_ITV0 结构体)
--------------------------------------------------------------------------------------------------

.. raw:: latex

   \footnotesize

.. tabularcolumns:: |p{4.6cm}|p{2.0cm}|p{10.7cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __le32
      - ``linemask``\ [2]
      - Bitmasks indicating the VBI service lines present. These
	``linemask`` values are stored in little endian byte order in the
	MPEG stream. Some reference ``linemask`` bit positions with their
	corresponding VBI line number and video field are given below.
	b\ :sub:`0` indicates the least significant bit of a ``linemask``
	value:


	::

	    linemask[0] b0:     line  6  first field
	    linemask[0] b17:    line 23  first field
	    linemask[0] b18:    line  6  second field
	    linemask[0] b31:    line 19  second field
	    linemask[1] b0:     line 20  second field
	    linemask[1] b3:     line 23  second field
	    linemask[1] b4-b31: unused and set to 0

	(指明存在的 VBI 服务行的位掩码。这些 ``linemask`` 值在 MPEG 流中以小端字节序存储。下面给出若干 ``linemask`` 位位置及其对应的 VBI 行号和视频场；b\ :sub:`0` 表示 ``linemask`` 值的最低有效位。)

    * - struct
	:c:type:`v4l2_mpeg_vbi_itv0_line`
      - ``line``\ [35]
      - This is a variable length array that holds from 1 to 35 lines of
	sliced VBI data. The sliced VBI data lines present correspond to
	the bits set in the ``linemask`` array, starting from b\ :sub:`0`
	of ``linemask``\ [0] up through b\ :sub:`31` of ``linemask``\ [0],
	and from b\ :sub:`0` of ``linemask``\ [1] up through b\ :sub:`3` of
	``linemask``\ [1]. ``line``\ [0] corresponds to the first bit
	found set in the ``linemask`` array, ``line``\ [1] corresponds to
	the second bit found set in the ``linemask`` array, etc. If no
	``linemask`` array bits are set, then ``line``\ [0] may contain
	one line of unspecified data that should be ignored by
	applications.
	(这是一个变长数组，保存 1 到 35 行切片 VBI 数据。存在的切片 VBI 数据行对应 ``linemask`` 数组中置位的比特，从 ``linemask``\ [0] 的 b\ :sub:`0` 一直到 ``linemask``\ [0] 的 b\ :sub:`31`，再从 ``linemask``\ [1] 的 b\ :sub:`0` 到 ``linemask``\ [1] 的 b\ :sub:`3`。``line``\ [0] 对应 ``linemask`` 数组中找到的第一个置位比特，``line``\ [1] 对应第二个，依此类推。如果 ``linemask`` 数组没有任何比特置位，则 ``line``\ [0] 可能包含一行未指定的数据，应用应忽略它。)

.. raw:: latex

   \normalsize

.. _v4l2-mpeg-vbi-itv0-1:

struct v4l2_mpeg_vbi_ITV0(v4l2_mpeg_vbi_ITV0 结构体)
----------------------------------------------------

.. tabularcolumns:: |p{5.2cm}|p{2.4cm}|p{9.7cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct
	:c:type:`v4l2_mpeg_vbi_itv0_line`
      - ``line``\ [36]
      - A fixed length array of 36 lines of sliced VBI data. ``line``\ [0]
	through ``line``\ [17] correspond to lines 6 through 23 of the
	first field. ``line``\ [18] through ``line``\ [35] corresponds to
	lines 6 through 23 of the second field.
	(36 行切片 VBI 数据的定长数组。``line``\ [0] 到 ``line``\ [17] 对应第一场的第 6 到 23 行；``line``\ [18] 到 ``line``\ [35] 对应第二场的第 6 到 23 行。)


.. c:type:: v4l2_mpeg_vbi_itv0_line

struct v4l2_mpeg_vbi_itv0_line(v4l2_mpeg_vbi_itv0_line 结构体)
--------------------------------------------------------------

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``id``
      - A line identifier value from
	:ref:`ITV0-Line-Identifier-Constants` that indicates the type of
	sliced VBI data stored on this line.
	(来自 :ref:`ITV0-Line-Identifier-Constants` 的行标识符值，表明本行存储的切片 VBI 数据类型。)
    * - __u8
      - ``data``\ [42]
      - The sliced VBI data for the line.
	(该行的切片 VBI 数据。)


.. _ITV0-Line-Identifier-Constants:

Line Identifiers for struct v4l2_mpeg_vbi_itv0_line id field(struct v4l2_mpeg_vbi_itv0_line id 字段的行标识符)
--------------------------------------------------------------------------------------------------------------

.. tabularcolumns:: |p{7.0cm}|p{1.8cm}|p{8.5cm}|

.. flat-table::
    :header-rows:  1
    :stub-columns: 0
    :widths:       3 1 4

    * - Defined Symbol(定义的符号)
      - Value(值)
      - Description(描述)
    * - ``V4L2_MPEG_VBI_IVTV_TELETEXT_B``
      - 1
      - Refer to :ref:`Sliced VBI services <vbi-services2>` for a
	description of the line payload.
	(行的载荷描述见 :ref:`Sliced VBI services <vbi-services2>`。)
    * - ``V4L2_MPEG_VBI_IVTV_CAPTION_525``
      - 4
      - Refer to :ref:`Sliced VBI services <vbi-services2>` for a
	description of the line payload.
	(行的载荷描述见 :ref:`Sliced VBI services <vbi-services2>`。)
    * - ``V4L2_MPEG_VBI_IVTV_WSS_625``
      - 5
      - Refer to :ref:`Sliced VBI services <vbi-services2>` for a
	description of the line payload.
	(行的载荷描述见 :ref:`Sliced VBI services <vbi-services2>`。)
    * - ``V4L2_MPEG_VBI_IVTV_VPS``
      - 7
      - Refer to :ref:`Sliced VBI services <vbi-services2>` for a
	description of the line payload.
	(行的载荷描述见 :ref:`Sliced VBI services <vbi-services2>`。)


.. [#f1]
   According to :ref:`ETS 300 706 <ets300706>` lines 6-22 of the first
   field and lines 5-22 of the second field may carry Teletext data.
   (根据 :ref:`ETS 300 706 <ets300706>`，第一场的 6-22 行与第二场的 5-22 行可以承载图文电视数据。)

.. [#f2]
   See also :ref:`vbi-525` and :ref:`vbi-625`.
   (另见 :ref:`vbi-525` 与 :ref:`vbi-625`。)
