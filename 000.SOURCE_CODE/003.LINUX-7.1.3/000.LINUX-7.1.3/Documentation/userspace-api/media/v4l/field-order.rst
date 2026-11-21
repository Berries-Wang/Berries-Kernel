.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _field-order:

*****************
Field Order(场序)
*****************

We have to distinguish between progressive and interlaced video.
Progressive video transmits all lines of a video image sequentially.
Interlaced video divides an image into two fields, containing only the
odd and even lines of the image, respectively. Alternating the so called
odd and even field are transmitted, and due to a small delay between
fields a cathode ray TV displays the lines interleaved, yielding the
original frame. This curious technique was invented because at refresh
rates similar to film the image would fade out too quickly. Transmitting
fields reduces the flicker without the necessity of doubling the frame
rate and with it the bandwidth required for each channel.
(我们必须区分逐行扫描 (progressive) 与隔行扫描 (interlaced) 视频。逐行视频按顺序传输视频图像的所有行。隔行视频将一幅图像分成两个场 (field)，分别只包含图像的奇数行和偶数行。所谓的奇场和偶场交替传输，由于场之间存在微小的延迟，阴极射线管电视会交错地显示这些行，从而还原出原始帧。这种奇特的技术之所以被发明，是因为在与电影相似的刷新率下，图像会过快地消退。传输场可以在不必使帧率以及相应信道所需带宽加倍的情况下减少闪烁。)

It is important to understand a video camera does not expose one frame
at a time, merely transmitting the frames separated into fields. The
fields are in fact captured at two different instances in time. An
object on screen may well move between one field and the next. For
applications analysing motion it is of paramount importance to recognize
which field of a frame is older, the *temporal order*.
(重要的是要理解，视频相机并不是一次曝光一帧，而只是把帧拆分成场来传输。这些场实际上是在两个不同的时间点捕获的。屏幕上的物体很可能在相邻两个场之间移动。对于分析运动的应用程序而言，至关重要的事情是识别帧中哪个场更旧，即*时间顺序* (temporal order)。)

When the driver provides or accepts images field by field rather than
interleaved, it is also important applications understand how the fields
combine to frames. We distinguish between top (aka odd) and bottom (aka
even) fields, the *spatial order*: The first line of the top field is
the first line of an interlaced frame, the first line of the bottom
field is the second line of that frame.
(当驱动按场(而非交错)提供或接受图像时，应用程序同样必须理解场如何组合成帧。我们区分顶场 (top，又称为奇场) 和底场 (bottom，又称为偶场)，即*空间顺序* (spatial order)：顶场的第一行是隔行帧的第一行，底场的第一行是该帧的第二行。)

However because fields were captured one after the other, arguing
whether a frame commences with the top or bottom field is pointless. Any
two successive top and bottom, or bottom and top fields yield a valid
frame. Only when the source was progressive to begin with, e. g. when
transferring film to video, two fields may come from the same frame,
creating a natural order.
(然而，由于场是一个接一个捕获的，争论一个帧是始于顶场还是底场毫无意义。任何两个连续的顶场和底场(或底场和顶场)都能组成一个有效的帧。只有当源本身是逐行的——例如在将胶片转换为视频时——两个场才可能来自同一帧，从而形成一种自然顺序。)

Counter to intuition the top field is not necessarily the older field.
Whether the older field contains the top or bottom lines is a convention
determined by the video standard. Hence the distinction between temporal
and spatial order of fields. The diagrams below should make this
clearer.
(与直觉相反，顶场不一定是更旧的场。更旧的场包含的是顶场还是底场的行，这是由视频标准决定的约定。因此才有场的时间顺序与空间顺序之分。下面的图示应当能让这一点更清楚。)

In V4L it is assumed that all video cameras transmit fields on the media
bus in the same order they were captured, so if the top field was
captured first (is the older field), the top field is also transmitted
first on the bus.
(在 V4L 中，假设所有视频相机在媒体总线上按捕获时的相同顺序传输场，因此如果顶场先被捕获(是更旧的场)，那么顶场也会先在总线上传输。)

All video capture and output devices must report the current field
order. Some drivers may permit the selection of a different order, to
this end applications initialize the ``field`` field of struct
:c:type:`v4l2_pix_format` before calling the
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl. If this is not desired it
should have the value ``V4L2_FIELD_ANY`` (0).
(所有视频捕获和输出设备都必须报告当前的场序。有些驱动可能允许选择不同的顺序；为此，应用程序在调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 之前初始化 struct :c:type:`v4l2_pix_format` 的 ``field`` 字段。如果不需要这样做，它应取值 ``V4L2_FIELD_ANY`` (0)。)


enum v4l2_field(场序枚举)
=========================

.. c:type:: v4l2_field

.. tabularcolumns:: |p{5.8cm}|p{0.6cm}|p{10.9cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FIELD_ANY``
      - 0
      - Applications request this field order when any field format
	is acceptable. Drivers choose depending on hardware capabilities or
	e.g. the requested image size, and return the actual field order.
	Drivers must never return ``V4L2_FIELD_ANY``.
	If multiple field orders are possible the
	driver must choose one of the possible field orders during
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` or
	:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`. struct
	:c:type:`v4l2_buffer` ``field`` can never be
	``V4L2_FIELD_ANY``.
	(当任何场序格式都可以接受时，应用程序请求此场序。驱动根据硬件
	能力或例如请求的图像大小进行选择，并返回实际的场序。驱动绝不
	能返回 ``V4L2_FIELD_ANY`` 。如果可能存在多种场序，驱动必须在
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 或
	:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` 期间选择一种可能的场序。
	struct :c:type:`v4l2_buffer` 的 ``field`` 绝不能是
	``V4L2_FIELD_ANY`` 。)
    * - ``V4L2_FIELD_NONE``
      - 1
      - Images are in progressive (frame-based) format, not interlaced
        (field-based).
	(图像为逐行 (基于帧) 格式，而非隔行 (基于场) 格式。)
    * - ``V4L2_FIELD_TOP``
      - 2
      - Images consist of the top (aka odd) field only.
	(图像仅由顶场(又称为奇场)组成。)
    * - ``V4L2_FIELD_BOTTOM``
      - 3
      - Images consist of the bottom (aka even) field only. Applications
	may wish to prevent a device from capturing interlaced images
	because they will have "comb" or "feathering" artefacts around
	moving objects.
	(图像仅由底场(又称为偶场)组成。应用程序可能希望防止设备捕获
	隔行图像，因为它们会在移动的物体周围出现 "comb" (梳齿) 或
	"feathering" (羽毛状) 伪影。)
    * - ``V4L2_FIELD_INTERLACED``
      - 4
      - Images contain both fields, interleaved line by line. The temporal
	order of the fields (whether the top or bottom field is older)
	depends on the current video standard. In M/NTSC the bottom
	field is the older field. In all other standards the top field
	is the older field.
	(图像包含两个场，逐行交错。场的时间顺序(顶场还是底场更旧)
	取决于当前的视频标准。在 M/NTSC 中，底场是更旧的场。在所有
	其他标准中，顶场是更旧的场。)
    * - ``V4L2_FIELD_SEQ_TB``
      - 5
      - Images contain both fields, the top field lines are stored first
	in memory, immediately followed by the bottom field lines. Fields
	are always stored in temporal order, the older one first in
	memory. Image sizes refer to the frame, not fields.
	(图像包含两个场，顶场的行先存储在内存中，紧接着是底场的行。
	场始终按时间顺序存储，较旧的在前。图像大小指帧而非场。)
    * - ``V4L2_FIELD_SEQ_BT``
      - 6
      - Images contain both fields, the bottom field lines are stored
	first in memory, immediately followed by the top field lines.
	Fields are always stored in temporal order, the older one first in
	memory. Image sizes refer to the frame, not fields.
	(图像包含两个场，底场的行先存储在内存中，紧接着是顶场的行。
	场始终按时间顺序存储，较旧的在前。图像大小指帧而非场。)
    * - ``V4L2_FIELD_ALTERNATE``
      - 7
      - The two fields of a frame are passed in separate buffers, in
	temporal order, i. e. the older one first. To indicate the field
	parity (whether the current field is a top or bottom field) the
	driver or application, depending on data direction, must set
	struct :c:type:`v4l2_buffer` ``field`` to
	``V4L2_FIELD_TOP`` or ``V4L2_FIELD_BOTTOM``. Any two successive
	fields pair to build a frame. If fields are successive, without
	any dropped fields between them (fields can drop individually),
	can be determined from the struct
	:c:type:`v4l2_buffer` ``sequence`` field. This
	format cannot be selected when using the read/write I/O method
	since there is no way to communicate if a field was a top or
	bottom field.
	(一帧的两个场以单独的缓冲区按时间顺序传递，即较旧的先传。
	为指示场的奇偶性(当前场是顶场还是底场)，驱动或应用程序
	(取决于数据方向)必须将 struct :c:type:`v4l2_buffer` 的
	``field`` 设置为 ``V4L2_FIELD_TOP`` 或 ``V4L2_FIELD_BOTTOM`` 。
	任何两个连续的场配对构成一帧。场是否连续(它们之间没有任何
	丢失的场——场可以单独丢失)可以从 struct
	:c:type:`v4l2_buffer` 的 ``sequence`` 字段确定。使用读/写
	I/O 方法时无法选择此格式，因为没有办法传达某个场是顶场还是
	底场。)
    * - ``V4L2_FIELD_INTERLACED_TB``
      - 8
      - Images contain both fields, interleaved line by line, top field
	first. The top field is the older field.
	(图像包含两个场，逐行交错，顶场在前。顶场是更旧的场。)
    * - ``V4L2_FIELD_INTERLACED_BT``
      - 9
      - Images contain both fields, interleaved line by line, top field
	first. The bottom field is the older field.
	(图像包含两个场，逐行交错，顶场在前。底场是更旧的场。)



.. _fieldseq-tb:

Field Order, Top Field First Transmitted(场序，顶场先发送)
==========================================================

.. kernel-figure:: fieldseq_tb.svg
    :alt:    fieldseq_tb.svg
    :align:  center

    Field Order, Top Field First Transmitted


.. _fieldseq-bt:

Field Order, Bottom Field First Transmitted(场序，底场先发送)
=============================================================

.. kernel-figure:: fieldseq_bt.svg
    :alt:    fieldseq_bt.svg
    :align:  center

    Field Order, Bottom Field First Transmitted
