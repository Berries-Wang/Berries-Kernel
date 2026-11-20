.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _raw-vbi:

*****************************************
Raw VBI Data Interface(原始 VBI 数据接口)
*****************************************

VBI is an abbreviation of Vertical Blanking Interval, a gap in the
sequence of lines of an analog video signal. During VBI no picture
information is transmitted, allowing some time while the electron beam
of a cathode ray tube TV returns to the top of the screen. Using an
oscilloscope you will find here the vertical synchronization pulses and
short data packages ASK modulated [#f1]_ onto the video signal. These are
transmissions of services such as Teletext or Closed Caption.
(VBI 是 Vertical Blanking Interval (场消隐期) 的缩写，是模拟视频信号行
序列中的一个空隙。在 VBI 期间不传输图像信息，这让阴极射线管电视的电子束
有时间返回屏幕顶部。用示波器在这里可以找到场同步脉冲，以及 ASK 调制到
视频信号上的短数据包。它们是诸如 Teletext (电传文字) 或 Closed Caption
(隐藏式字幕) 之类服务的传输内容。)

Subject of this interface type is raw VBI data, as sampled off a video
signal, or to be added to a signal for output. The data format is
similar to uncompressed video images, a number of lines times a number
of samples per line, we call this a VBI image.
(本接口类型的对象是原始 VBI 数据，即从视频信号中采样得到的数据，或将要
加入信号以便输出的数据。其数据格式与未压缩的视频图像类似：若干行乘以
每行若干个采样点，我们称之为 VBI 图像。)

Conventionally V4L2 VBI devices are accessed through character device
special files named ``/dev/vbi`` and ``/dev/vbi0`` to ``/dev/vbi31``
with major number 81 and minor numbers 224 to 255. ``/dev/vbi`` is
typically a symbolic link to the preferred VBI device. This convention
applies to both input and output devices.
(按照惯例，V4L2 VBI 设备通过名为 ``/dev/vbi`` 与 ``/dev/vbi0`` 到
``/dev/vbi31`` 的字符设备特殊文件访问，主设备号为 81，次设备号为 224 到
255。``/dev/vbi`` 通常是指向首选 VBI 设备的符号链接。这一惯例同时适用
于输入与输出设备。)

To address the problems of finding related video and VBI devices VBI
capturing and output is also available as device function under
``/dev/video``. To capture or output raw VBI data with these devices
applications must call the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
Accessed as ``/dev/vbi``, raw VBI capturing or output is the default
device function.
(为了解决查找相关视频与 VBI 设备的问题，VBI 采集与输出也可以作为
``/dev/video`` 下的设备功能来使用。要用这些设备采集或输出原始 VBI 数据，
应用程序必须调用 VIDIOC_S_FMT ioctl。以
``/dev/vbi`` 访问时，原始 VBI 采集或输出是默认的设备功能。)

Querying Capabilities(查询能力)
===============================

Devices supporting the raw VBI capturing or output API set the
``V4L2_CAP_VBI_CAPTURE`` or ``V4L2_CAP_VBI_OUTPUT`` flags, respectively,
in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. At least one of the
read/write or streaming I/O methods must be supported. VBI
devices may or may not have a tuner or modulator.
(支持原始 VBI 采集或输出 API 的设备，会相应地在 VIDIOC_QUERYCAP ioctl
返回的 struct v4l2_capability 的 ``capabilities`` 字段中设置
``V4L2_CAP_VBI_CAPTURE`` 或 ``V4L2_CAP_VBI_OUTPUT`` 标志。必须至少支持
读/写或流式 I/O 方法之一。VBI 设备可能有也可能没有调谐器或调制器。)

Supplemental Functions(补充功能)
================================

VBI devices shall support :ref:`video input or output <video>`,
:ref:`tuner or modulator <tuner>`, and :ref:`controls <control>`
ioctls as needed. The :ref:`video standard <standard>` ioctls provide
information vital to program a VBI device, therefore must be supported.
(VBI 设备应按需支持视频输入或输出、调谐器或调制器以及控制 ioctl。视频
标准 ioctl 提供对 VBI 设备编程至关重要的信息，因此必须受支持。)

Raw VBI Format Negotiation(原始 VBI 格式协商)
==============================================

Raw VBI sampling abilities can vary, in particular the sampling
frequency. To properly interpret the data V4L2 specifies an ioctl to
query the sampling parameters. Moreover, to allow for some flexibility
applications can also suggest different parameters.
(原始 VBI 的采样能力可能各不相同，尤其是采样频率。为了正确解释数据，
V4L2 规定了一个用于查询采样参数的 ioctl。此外，为了保留一定的灵活性，
应用程序也可以建议不同的参数。)

As usual these parameters are *not* reset at :c:func:`open()`
time to permit Unix tool chains, programming a device and then reading
from it as if it was a plain file. Well written V4L2 applications should
always ensure they really get what they want, requesting reasonable
parameters and then checking if the actual parameters are suitable.
(与通常一样，这些参数 *不会* 在 open() 时被重置，以便支持 Unix 工具链
—— 先对设备编程，然后像读取普通文件一样从中读取。编写良好的 V4L2 应用
程序应始终确保自己真正得到了想要的东西：请求合理的参数，然后检查实际参数
是否合适。)

To query the current raw VBI capture parameters applications set the
``type`` field of a struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VBI_CAPTURE`` or ``V4L2_BUF_TYPE_VBI_OUTPUT``, and call
the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl with a pointer to this
structure. Drivers fill the struct
:c:type:`v4l2_vbi_format` ``vbi`` member of the
``fmt`` union.
(要查询当前的原始 VBI 采集参数，应用程序把 struct v4l2_format 的
``type`` 字段设置为 ``V4L2_BUF_TYPE_VBI_CAPTURE`` 或
``V4L2_BUF_TYPE_VBI_OUTPUT``，并调用 VIDIOC_G_FMT ioctl，传入指向该结构
的指针。驱动会填充 ``fmt`` 联合体中 struct v4l2_vbi_format 类型的
``vbi`` 成员。)

To request different parameters applications set the ``type`` field of a
struct :c:type:`v4l2_format` as above and initialize all
fields of the struct :c:type:`v4l2_vbi_format`
``vbi`` member of the ``fmt`` union, or better just modify the results
of :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`, and call the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl with a pointer to this structure. Drivers return an ``EINVAL`` error
code only when the given parameters are ambiguous, otherwise they modify
the parameters according to the hardware capabilities and return the
actual parameters. When the driver allocates resources at this point, it
may return an ``EBUSY`` error code to indicate the returned parameters are
valid but the required resources are currently not available. That may
happen for instance when the video and VBI areas to capture would
overlap, or when the driver supports multiple opens and another process
already requested VBI capturing or output. Anyway, applications must
expect other resource allocation points which may return ``EBUSY``, at the
:ref:`VIDIOC_STREAMON` ioctl and the first :c:func:`read()`
, :c:func:`write()` and :c:func:`select()` calls.
(要请求不同的参数，应用程序按上述方式设置 struct v4l2_format 的
``type`` 字段，初始化 ``fmt`` 联合体中 struct v4l2_vbi_format 类型
``vbi`` 成员的所有字段 (或者更好的做法是直接修改 VIDIOC_G_FMT 的结果)，
并调用 VIDIOC_S_FMT ioctl，传入指向该结构的指针。只有当给定参数有歧义
时，驱动才返回 ``EINVAL`` 错误码；否则驱动会根据硬件能力修改参数并返回
实际参数。当驱动在此时分配资源时，它可能返回 ``EBUSY`` 错误码，以表明
返回的参数有效但所需的资源当前不可用。例如，当要采集的视频与 VBI 区域
会重叠时，或者当驱动支持多次打开而另一个进程已经请求了 VBI 采集或输出时，
就可能发生这种情况。无论如何，应用程序必须预期其他可能返回 ``EBUSY`` 的
资源分配点，例如 VIDIOC_STREAMON ioctl 以及最先的 read()、write() 和
select() 调用。)

VBI devices must implement both the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, even if :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ignores all requests
and always returns default parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` does.
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` is optional.
(VBI 设备必须同时实现 VIDIOC_G_FMT 与 VIDIOC_S_FMT ioctl，即使
VIDIOC_S_FMT 忽略所有请求并总是像 VIDIOC_G_FMT 那样返回默认参数。
VIDIOC_TRY_FMT 是可选的。)

.. tabularcolumns:: |p{1.6cm}|p{4.2cm}|p{11.5cm}|

.. c:type:: v4l2_vbi_format

.. cssclass:: longtable

.. flat-table:: struct v4l2_vbi_format
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``sampling_rate``
      - Samples per second, i. e. unit 1 Hz. (每秒采样数，即单位为 1 Hz。)
    * - __u32
      - ``offset``
      - Horizontal offset of the VBI image, relative to the leading edge
	of the line synchronization pulse and counted in samples: The
	first sample in the VBI image will be located ``offset`` /
	``sampling_rate`` seconds following the leading edge. See also
	:ref:`vbi-hsync`. (VBI 图像的水平偏移，相对于行同步脉冲的前沿并以
	采样点计数：VBI 图像中的第一个采样点将位于前沿之后 ``offset`` /
	``sampling_rate`` 秒处。另参见 vbi-hsync。)
    * - __u32
      - ``samples_per_line``
      -
    * - __u32
      - ``sample_format``
      - Defines the sample format as in :ref:`pixfmt`, a
	four-character-code. [#f2]_ Usually this is ``V4L2_PIX_FMT_GREY``,
	i. e. each sample consists of 8 bits with lower values oriented
	towards the black level. Do not assume any other correlation of
	values with the signal level. For example, the MSB does not
	necessarily indicate if the signal is 'high' or 'low' because 128
	may not be the mean value of the signal. Drivers shall not convert
	the sample format by software. (按照 pixfmt 一节的方式定义采样格式，
	为一个四字符码。通常这是 ``V4L2_PIX_FMT_GREY``，即每个采样点由 8 位
	组成，值越小越靠近黑电平。不要假设值与信号电平之间有任何其他对应
	关系。例如，最高有效位不一定能指示信号是 '高' 还是 '低'，因为 128
	未必是信号的平均值。驱动不得用软件转换采样格式。)
    * - __u32
      - ``start``\ [#f2]_
      - This is the scanning system line number associated with the first
	line of the VBI image, of the first and the second field
	respectively. See :ref:`vbi-525` and :ref:`vbi-625` for valid
	values. The ``V4L2_VBI_ITU_525_F1_START``,
	``V4L2_VBI_ITU_525_F2_START``, ``V4L2_VBI_ITU_625_F1_START`` and
	``V4L2_VBI_ITU_625_F2_START`` defines give the start line numbers
	for each field for each 525 or 625 line format as a convenience.
	Don't forget that ITU line numbering starts at 1, not 0. VBI input
	drivers can return start values 0 if the hardware cannot reliable
	identify scanning lines, VBI acquisition may not require this
	information. (这是与 VBI 图像第一行相关联的扫描系统行号，分别对应
	第一场与第二场。有效值参见 vbi-525 与 vbi-625。为方便起见，
	``V4L2_VBI_ITU_525_F1_START``、``V4L2_VBI_ITU_525_F2_START``、
	``V4L2_VBI_ITU_625_F1_START`` 与 ``V4L2_VBI_ITU_625_F2_START`` 定义
	给出了每种 525 或 625 行格式中每一场的起始行号。别忘了 ITU 行号从
	1 开始，而不是 0。如果硬件无法可靠地识别扫描行，VBI 输入驱动可以
	返回 0 作为起始值，此时 VBI 采集可能不需要该信息。)
    * - __u32
      - ``count``\ [#f2]_
      - The number of lines in the first and second field image,
	respectively. (第一场与第二场图像各自的行数。)
    * - :cspan:`2`

	Drivers should be as flexibility as possible. For example, it may
	be possible to extend or move the VBI capture window down to the
	picture area, implementing a 'full field mode' to capture data
	service transmissions embedded in the picture.
	(驱动应尽可能灵活。例如，也许可以把 VBI 采集窗口向下扩展或移动到
	图像区域，实现一种 '全场模式' (full field mode) 来采集嵌入在图像中
	的数据服务传输。)

	An application can set the first or second ``count`` value to zero
	if no data is required from the respective field; ``count``\ [1]
	if the scanning system is progressive, i. e. not interlaced. The
	corresponding start value shall be ignored by the application and
	driver. Anyway, drivers may not support single field capturing and
	return both count values non-zero.
	(如果不需要来自相应场的数据，应用程序可以把第一个或第二个 ``count``
	值设为 0；如果扫描系统是逐行的 (即非隔行的)，则 count[1] 为 0。
	相应的起始值应被应用程序和驱动忽略。不过，驱动可能不支持单场采集，
	并返回两个都非零的 count 值。)

	Both ``count`` values set to zero, or line numbers are outside the
	bounds depicted\ [#f4]_, or a field image covering lines of two
	fields, are invalid and shall not be returned by the driver.
	(两个 ``count`` 值都设为 0、行号超出所示边界、或某场图像覆盖了两个
	场的行，均为无效，驱动不得返回这样的设置。)

	To initialize the ``start`` and ``count`` fields, applications
	must first determine the current video standard selection. The
	:ref:`v4l2_std_id <v4l2-std-id>` or the ``framelines`` field
	of struct :c:type:`v4l2_standard` can be evaluated
	for this purpose.
	(要初始化 ``start`` 与 ``count`` 字段，应用程序必须先确定当前选择的
	视频标准。为此可以评估 v4l2_std_id 或 struct v4l2_standard 的
	``framelines`` 字段。)
    * - __u32
      - ``flags``
      - See :ref:`vbifmt-flags` below. Currently only drivers set flags,
	applications must set this field to zero.
	(见下文的 vbifmt-flags 一节。目前只有驱动会设置标志，应用程序必须
	将此字段设为 0。)
    * - __u32
      - ``reserved``\ [#f2]_
      - This array is reserved for future extensions. Drivers and
	applications must set it to zero.
	(此数组预留给未来扩展。驱动和应用程序必须将其设为 0。)

.. tabularcolumns:: |p{4.4cm}|p{1.5cm}|p{11.4cm}|

.. _vbifmt-flags:

.. flat-table:: Raw VBI Format Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_VBI_UNSYNC``
      - 0x0001
      - This flag indicates hardware which does not properly distinguish
	between fields. Normally the VBI image stores the first field
	(lower scanning line numbers) first in memory. This may be a top
	or bottom field depending on the video standard. When this flag is
	set the first or second field may be stored first, however the
	fields are still in correct temporal order with the older field
	first in memory. [#f3]_ (此标志表明硬件不能正确地区分各个场。通常，
	VBI 图像在内存中先存储第一场 (扫描行号较小的场)。根据视频标准的
	不同，这可能是一场顶场或底场。设置了此标志时，第一场或第二场都可能
	先存储，但两个场仍保持正确的时间顺序，较早的场在内存中靠前。)
    * - ``V4L2_VBI_INTERLACED``
      - 0x0002
      - By default the two field images will be passed sequentially; all
	lines of the first field followed by all lines of the second field
	(compare :ref:`field-order` ``V4L2_FIELD_SEQ_TB`` and
	``V4L2_FIELD_SEQ_BT``, whether the top or bottom field is first in
	memory depends on the video standard). When this flag is set, the
	two fields are interlaced (cf. ``V4L2_FIELD_INTERLACED``). The
	first line of the first field followed by the first line of the
	second field, then the two second lines, and so on. Such a layout
	may be necessary when the hardware has been programmed to capture
	or output interlaced video images and is unable to separate the
	fields for VBI capturing at the same time. For simplicity setting
	this flag implies that both ``count`` values are equal and
	non-zero. (默认情况下，两场图像将按顺序传递：第一场的所有行之后跟着
	第二场的所有行 (比较 field-order 一节的 ``V4L2_FIELD_SEQ_TB`` 与
	``V4L2_FIELD_SEQ_BT``；内存中先存储顶场还是底场取决于视频标准)。
	设置此标志后，两个场将交错 (相当于 ``V4L2_FIELD_INTERLACED``)：第一
	场的第一行之后是第二场的第一行，然后是两个第二行，依此类推。当硬件
	被编程为采集或输出隔行的视频图像、且无法在采集 VBI 的同时分离场时，
	这种布局可能是必需的。为简单起见，设置此标志意味着两个 ``count``
	值相等且非零。)


.. _vbi-hsync:

.. kernel-figure:: vbi_hsync.svg
    :alt:   vbi_hsync.svg
    :align: center

    Line synchronization

.. _vbi-525:

.. kernel-figure:: vbi_525.svg
    :alt:   vbi_525.svg
    :align: center

    ITU-R 525 line numbering (M/NTSC and M/PAL)

.. _vbi-625:

.. kernel-figure:: vbi_625.svg
    :alt:   vbi_625.svg
    :align: center

    ITU-R 625 line numbering

Remember the VBI image format depends on the selected video standard,
therefore the application must choose a new standard or query the
current standard first. Attempts to read or write data ahead of format
negotiation, or after switching the video standard which may invalidate
the negotiated VBI parameters, should be refused by the driver. A format
change during active I/O is not permitted.
(请记住，VBI 图像格式取决于所选的视频标准，因此应用程序必须先选择一个新
标准或查询当前标准。在格式协商之前读取或写入数据的尝试，或者在切换可能
导致已协商的 VBI 参数失效的视频标准之后这样做，都应被驱动拒绝。在活动的
I/O 期间不允许更改格式。)

Reading and writing VBI images(读写 VBI 图像)
=============================================

To assure synchronization with the field number and easier
implementation, the smallest unit of data passed at a time is one frame,
consisting of two fields of VBI images immediately following in memory.
(为了保证与场号同步并便于实现，一次传递的最小数据单位是一帧，它由内存中
紧邻的两个 VBI 图像场组成。)

The total size of a frame computes as follows:
(一帧的总大小按如下方式计算：)

.. code-block:: c

    (count[0] + count[1]) * samples_per_line * sample size in bytes

The sample size is most likely always one byte, applications must check
the ``sample_format`` field though, to function properly with other
drivers.
(采样大小最可能始终是 1 字节，不过应用程序必须检查 ``sample_format``
字段，以便与其他驱动正确配合工作。)

A VBI device may support :ref:`read/write <rw>` and/or streaming
(:ref:`memory mapping <mmap>` or :ref:`user pointer <userp>`) I/O.
The latter bears the possibility of synchronizing video and VBI data by
using buffer timestamps.
(VBI 设备可以支持读/写和/或流式 (内存映射或用户指针) I/O。后者提供了
通过使用缓冲区时间戳来同步视频与 VBI 数据的可能性。)

Remember the :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` ioctl and the
first :c:func:`read()`, :c:func:`write()` and
:c:func:`select()` call can be resource allocation
points returning an ``EBUSY`` error code if the required hardware resources
are temporarily unavailable, for example the device is already in use by
another process.
(请记住，VIDIOC_STREAMON ioctl 以及最先的 read()、write() 和 select()
调用可能是资源分配点，在所需的硬件资源暂时不可用时 (例如设备已被另一个
进程使用) 会返回 ``EBUSY`` 错误码。)

.. [#f1]
   ASK: Amplitude-Shift Keying. A high signal level represents a '1'
   bit, a low level a '0' bit.
   (ASK：幅移键控 (Amplitude-Shift Keying)。高信号电平表示 '1' 位，低
   电平表示 '0' 位。)

.. [#f2]
   A few devices may be unable to sample VBI data at all but can extend
   the video capture window to the VBI region.
   (少数设备可能根本无法采样 VBI 数据，但可以把视频采集窗口扩展到 VBI
   区域。)

.. [#f3]
   Most VBI services transmit on both fields, but some have different
   semantics depending on the field number. These cannot be reliable
   decoded or encoded when ``V4L2_VBI_UNSYNC`` is set.
   (大多数 VBI 服务在两个场上都传输，但有些服务的语义取决于场号。当设置
   了 ``V4L2_VBI_UNSYNC`` 时，这些服务无法被可靠地解码或编码。)

.. [#f4]
   The valid values ar shown at :ref:`vbi-525` and :ref:`vbi-625`.
   (有效值显示在 vbi-525 与 vbi-625 中。)
