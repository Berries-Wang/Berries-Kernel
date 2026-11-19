.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _diff-v4l:

****************************************************
Differences between V4L and V4L2(V4L 与 V4L2 的差异)
****************************************************

The Video For Linux API was first introduced in Linux 2.1 to unify and
replace various TV and radio device related interfaces, developed
independently by driver writers in prior years. Starting with Linux 2.5
the much improved V4L2 API replaces the V4L API. The support for the old
V4L calls were removed from Kernel, but the library :ref:`libv4l`
supports the conversion of a V4L API system call into a V4L2 one.
(Video For Linux API 最早在 Linux 2.1 中引入，用于统一并取代此前几年由驱动
作者独立开发的各类电视和广播设备相关接口。从 Linux 2.5 开始，大幅改进的
V4L2 API 取代了 V4L API。对旧 V4L 调用的支持已从内核中移除，但
:ref:`libv4l` 库支持把 V4L API 系统调用转换为 V4L2 系统调用。)

Opening and Closing Devices(打开与关闭设备)
===========================================

For compatibility reasons the character device file names recommended
for V4L2 video capture, overlay, radio and raw vbi capture devices did
not change from those used by V4L. They are listed in :ref:`devices`
and below in :ref:`v4l-dev`.
(出于兼容性原因，为 V4L2 视频捕获、叠加、广播和原始 vbi 捕获设备推荐的字符
设备文件名与 V4L 所用的相同。它们列在 :ref:`devices` 以及下文 :ref:`v4l-dev`
中。)

The teletext devices (minor range 192-223) have been removed in V4L2 and
no longer exist. There is no hardware available anymore for handling
pure teletext. Instead raw or sliced VBI is used.
(图文电视 (teletext) 设备（次设备号范围 192-223）在 V4L2 中已被移除，不再
存在。现在已经没有能处理纯图文电视的硬件了，取而代之的是原始或切片
VBI。)

The V4L ``videodev`` module automatically assigns minor numbers to
drivers in load order, depending on the registered device type. We
recommend that V4L2 drivers by default register devices with the same
numbers, but the system administrator can assign arbitrary minor numbers
using driver module options. The major device number remains 81.
(V4L 的 ``videodev`` 模块会根据注册的设备类型，按加载顺序自动为驱动分配次
设备号。我们建议 V4L2 驱动默认以相同的次设备号注册设备，但系统管理员可以
通过驱动模块选项分配任意次设备号。主设备号保持为 81。)

.. _v4l-dev:

.. flat-table:: V4L Device Types, Names and Numbers
    :header-rows:  1
    :stub-columns: 0

    * - Device Type(设备类型)
      - File Name(文件名)
      - Minor Numbers(次设备号)
    * - Video capture and overlay(视频捕获与叠加)
      - ``/dev/video`` and ``/dev/bttv0``\  [#f1]_, ``/dev/video0`` to
	``/dev/video63``
      - 0-63
    * - Radio receiver(广播接收器)
      - ``/dev/radio``\  [#f2]_, ``/dev/radio0`` to ``/dev/radio63``
      - 64-127
    * - Raw VBI capture(原始 VBI 捕获)
      - ``/dev/vbi``, ``/dev/vbi0`` to ``/dev/vbi31``
      - 224-255

V4L prohibits (or used to prohibit) multiple opens of a device file.
V4L2 drivers *may* support multiple opens, see :ref:`open` for details
and consequences.
(V4L 禁止（或曾经禁止）多次打开同一设备文件。V4L2 驱动 *可以* 支持多次
打开，细节与后果参见 :ref:`open`。)

V4L drivers respond to V4L2 ioctls with an ``EINVAL`` error code.
(V4L 驱动对 V4L2 ioctl 以 ``EINVAL`` 错误码作回应。)

Querying Capabilities(查询能力)
===============================

The V4L ``VIDIOCGCAP`` ioctl is equivalent to V4L2's
:ref:`VIDIOC_QUERYCAP`.
(V4L 的 ``VIDIOCGCAP`` ioctl 等价于 V4L2 的 :ref:`VIDIOC_QUERYCAP`。)

The ``name`` field in struct ``video_capability`` became
``card`` in struct :c:type:`v4l2_capability`, ``type``
was replaced by ``capabilities``. Note V4L2 does not distinguish between
device types like this, better think of basic video input, video output
and radio devices supporting a set of related functions like video
capturing, video overlay and VBI capturing. See :ref:`open` for an
introduction.
(struct ``video_capability`` 中的 ``name`` 字段变成了 struct
:c:type:`v4l2_capability` 中的 ``card``，``type`` 则被 ``capabilities``
取代。注意，V4L2 不像这样区分设备类型；更应把设备理解为支持一组相关功能
（如视频捕获、视频叠加和 VBI 捕获）的基本视频输入、视频输出和广播设备。
简介参见 :ref:`open`。)

.. raw:: latex

   \small

.. tabularcolumns:: |p{5.3cm}|p{6.7cm}|p{5.3cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - ``struct video_capability`` ``type``
      - struct :c:type:`v4l2_capability`
	``capabilities`` flags
      - Purpose(用途)
    * - ``VID_TYPE_CAPTURE``
      - ``V4L2_CAP_VIDEO_CAPTURE``
      - The :ref:`video capture <capture>` interface is supported.
	(支持 :ref:`video capture <capture>` 接口。)
    * - ``VID_TYPE_TUNER``
      - ``V4L2_CAP_TUNER``
      - The device has a :ref:`tuner or modulator <tuner>`.
	(设备拥有 :ref:`tuner or modulator <tuner>`。)
    * - ``VID_TYPE_TELETEXT``
      - ``V4L2_CAP_VBI_CAPTURE``
      - The :ref:`raw VBI capture <raw-vbi>` interface is supported.
	(支持 :ref:`raw VBI capture <raw-vbi>` 接口。)
    * - ``VID_TYPE_OVERLAY``
      - ``V4L2_CAP_VIDEO_OVERLAY``
      - The :ref:`video overlay <overlay>` interface is supported.
	(支持 :ref:`video overlay <overlay>` 接口。)
    * - ``VID_TYPE_CHROMAKEY``
      - ``V4L2_FBUF_CAP_CHROMAKEY`` in field ``capability`` of struct
	:c:type:`v4l2_framebuffer`
      - Whether chromakey overlay is supported. For more information on
	overlay see :ref:`overlay`.
	(是否支持色键 (chromakey) 叠加。叠加的更多信息参见 :ref:`overlay`。)
    * - ``VID_TYPE_CLIPPING``
      - ``V4L2_FBUF_CAP_LIST_CLIPPING`` and
	``V4L2_FBUF_CAP_BITMAP_CLIPPING`` in field ``capability`` of
	struct :c:type:`v4l2_framebuffer`
      - Whether clipping the overlaid image is supported, see
	:ref:`overlay`.
	(是否支持对叠加图像进行剪裁 (clipping)，参见 :ref:`overlay`。)
    * - ``VID_TYPE_FRAMERAM``
      - ``V4L2_FBUF_CAP_EXTERNOVERLAY`` *not set* in field ``capability``
	of struct :c:type:`v4l2_framebuffer`
      - Whether overlay overwrites frame buffer memory, see
	:ref:`overlay`.
	(叠加是否覆写帧缓冲内存，参见 :ref:`overlay`。)
    * - ``VID_TYPE_SCALES``
      - ``-``
      - This flag indicates if the hardware can scale images. The V4L2 API
	implies the scale factor by setting the cropping dimensions and
	image size with the :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` and
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, respectively. The
	driver returns the closest sizes possible. For more information on
	cropping and scaling see :ref:`crop`.
	(此标志指示硬件是否能缩放图像。V4L2 API 通过分别用
	:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` 和 :ref:`VIDIOC_S_FMT
	<VIDIOC_G_FMT>` ioctl 设置裁剪尺寸与图像大小来隐含给出缩放因子。驱动
	返回尽可能接近的尺寸。裁剪与缩放的更多信息参见 :ref:`crop`。)
    * - ``VID_TYPE_MONOCHROME``
      - ``-``
      - Applications can enumerate the supported image formats with the
	:ref:`VIDIOC_ENUM_FMT` ioctl to determine if
	the device supports grey scale capturing only. For more
	information on image formats see :ref:`pixfmt`.
	(应用程序可以用 :ref:`VIDIOC_ENUM_FMT` ioctl 枚举受支持的图像格式，以
	确定设备是否只支持灰度捕获。图像格式的更多信息参见 :ref:`pixfmt`。)
    * - ``VID_TYPE_SUBCAPTURE``
      - ``-``
      - Applications can call the :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>`
	ioctl to determine if the device supports capturing a subsection
	of the full picture ("cropping" in V4L2). If not, the ioctl
	returns the ``EINVAL`` error code. For more information on cropping
	and scaling see :ref:`crop`.
	(应用程序可以调用 :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` ioctl 来确定
	设备是否支持捕获整幅图像的一个子区域（V4L2 中称"裁剪"）。如果不支持，
	该 ioctl 返回 ``EINVAL`` 错误码。裁剪与缩放的更多信息参见
	:ref:`crop`。)
    * - ``VID_TYPE_MPEG_DECODER``
      - ``-``
      - Applications can enumerate the supported image formats with the
	:ref:`VIDIOC_ENUM_FMT` ioctl to determine if
	the device supports MPEG streams.
	(应用程序可以用 :ref:`VIDIOC_ENUM_FMT` ioctl 枚举受支持的图像格式，
	以确定设备是否支持 MPEG 流。)
    * - ``VID_TYPE_MPEG_ENCODER``
      - ``-``
      - See above.
	(参见上文。)
    * - ``VID_TYPE_MJPEG_DECODER``
      - ``-``
      - See above.
	(参见上文。)
    * - ``VID_TYPE_MJPEG_ENCODER``
      - ``-``
      - See above.
	(参见上文。)

.. raw:: latex

   \normalsize

The ``audios`` field was replaced by ``capabilities`` flag
``V4L2_CAP_AUDIO``, indicating *if* the device has any audio inputs or
outputs. To determine their number applications can enumerate audio
inputs with the :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` ioctl. The
audio ioctls are described in :ref:`audio`.
(``audios`` 字段被 ``capabilities`` 标志 ``V4L2_CAP_AUDIO`` 取代，指示设备
*是否* 拥有任何音频输入或输出。要确定其数量，应用程序可以用
:ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` ioctl 枚举音频输入。音频 ioctl 在
:ref:`audio` 中描述。)

The ``maxwidth``, ``maxheight``, ``minwidth`` and ``minheight`` fields
were removed. Calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` or
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl with the desired
dimensions returns the closest size possible, taking into account the
current video standard, cropping and scaling limitations.
(``maxwidth``、``maxheight``、``minwidth`` 和 ``minheight`` 字段已被移除。
以期望的尺寸调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 或
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl 会返回尽可能接近的尺寸，同时考虑
当前视频标准以及裁剪与缩放限制。)

Video Sources(视频源)
=====================

V4L provides the ``VIDIOCGCHAN`` and ``VIDIOCSCHAN`` ioctl using struct
``video_channel`` to enumerate the video inputs of a V4L
device. The equivalent V4L2 ioctls are
:ref:`VIDIOC_ENUMINPUT`,
:ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` and
:ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` using struct
:c:type:`v4l2_input` as discussed in :ref:`video`.
(V4L 提供使用 struct ``video_channel`` 的 ``VIDIOCGCHAN`` 和
``VIDIOCSCHAN`` ioctl 来枚举 V4L 设备的视频输入。等价的 V4L2 ioctl 是使用
struct :c:type:`v4l2_input` 的 :ref:`VIDIOC_ENUMINPUT`、
:ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` 和
:ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>`，在 :ref:`video` 中讨论。)

The ``channel`` field counting inputs was renamed to ``index``, the
video input types were renamed as follows:
(用于计数的 ``channel`` 字段更名为 ``index``，视频输入类型更名如下：)


.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - struct ``video_channel`` ``type``
      - struct :c:type:`v4l2_input` ``type``
    * - ``VIDEO_TYPE_TV``
      - ``V4L2_INPUT_TYPE_TUNER``
    * - ``VIDEO_TYPE_CAMERA``
      - ``V4L2_INPUT_TYPE_CAMERA``

Unlike the ``tuners`` field expressing the number of tuners of this
input, V4L2 assumes each video input is connected to at most one tuner.
However a tuner can have more than one input, i. e. RF connectors, and a
device can have multiple tuners. The index number of the tuner
associated with the input, if any, is stored in field ``tuner`` of
struct :c:type:`v4l2_input`. Enumeration of tuners is
discussed in :ref:`tuner`.
(与表示此输入的调谐器数量的 ``tuners`` 字段不同，V4L2 假定每个视频输入至多
连接一个调谐器。不过一个调谐器可以有多个输入（即射频连接器），且设备可以
有多个调谐器。与该输入关联的调谐器（如果存在）的索引号存储在 struct
:c:type:`v4l2_input` 的 ``tuner`` 字段中。调谐器的枚举在 :ref:`tuner`
中讨论。)

The redundant ``VIDEO_VC_TUNER`` flag was dropped. Video inputs
associated with a tuner are of type ``V4L2_INPUT_TYPE_TUNER``. The
``VIDEO_VC_AUDIO`` flag was replaced by the ``audioset`` field. V4L2
considers devices with up to 32 audio inputs. Each set bit in the
``audioset`` field represents one audio input this video input combines
with. For information about audio inputs and how to switch between them
see :ref:`audio`.
(冗余的 ``VIDEO_VC_TUNER`` 标志被移除。与调谐器关联的视频输入类型为
``V4L2_INPUT_TYPE_TUNER``。 ``VIDEO_VC_AUDIO`` 标志被 ``audioset`` 字段
取代。V4L2 考虑最多拥有 32 个音频输入的设备。 ``audioset`` 字段中每个置位
的比特代表此视频输入与之组合的一个音频输入。关于音频输入及其切换方式，
参见 :ref:`audio`。)

The ``norm`` field describing the supported video standards was replaced
by ``std``. The V4L specification mentions a flag ``VIDEO_VC_NORM``
indicating whether the standard can be changed. This flag was a later
addition together with the ``norm`` field and has been removed in the
meantime. V4L2 has a similar, albeit more comprehensive approach to
video standards, see :ref:`standard` for more information.
(描述受支持视频制式的 ``norm`` 字段被 ``std`` 取代。V4L 规范提到一个标志
``VIDEO_VC_NORM``，指示制式是否可以更改。该标志是与 ``norm`` 字段一同
后期加入的，此后已被移除。V4L2 对视频制式有一个类似但更全面的方法，
更多信息参见 :ref:`standard`。)

Tuning(调谐)
============

The V4L ``VIDIOCGTUNER`` and ``VIDIOCSTUNER`` ioctl and struct
``video_tuner`` can be used to enumerate the tuners of a
V4L TV or radio device. The equivalent V4L2 ioctls are
:ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` and
:ref:`VIDIOC_S_TUNER <VIDIOC_G_TUNER>` using struct
:c:type:`v4l2_tuner`. Tuners are covered in :ref:`tuner`.
(V4L 的 ``VIDIOCGTUNER`` 和 ``VIDIOCSTUNER`` ioctl 以及 struct
``video_tuner`` 可用于枚举 V4L 电视或广播设备的调谐器。等价的 V4L2 ioctl
是使用 struct :c:type:`v4l2_tuner` 的
:ref:`VIDIOC_G_TUNER <VIDIOC_G_TUNER>` 和
:ref:`VIDIOC_S_TUNER <VIDIOC_G_TUNER>`。调谐器在 :ref:`tuner` 中
介绍。)

The ``tuner`` field counting tuners was renamed to ``index``. The fields
``name``, ``rangelow`` and ``rangehigh`` remained unchanged.
(用于计数的 ``tuner`` 字段更名为 ``index``。 ``name``、``rangelow`` 和
``rangehigh`` 字段保持不变。)

The ``VIDEO_TUNER_PAL``, ``VIDEO_TUNER_NTSC`` and ``VIDEO_TUNER_SECAM``
flags indicating the supported video standards were dropped. This
information is now contained in the associated struct
:c:type:`v4l2_input`. No replacement exists for the
``VIDEO_TUNER_NORM`` flag indicating whether the video standard can be
switched. The ``mode`` field to select a different video standard was
replaced by a whole new set of ioctls and structures described in
:ref:`standard`. Due to its ubiquity it should be mentioned the BTTV
driver supports several standards in addition to the regular
``VIDEO_MODE_PAL`` (0), ``VIDEO_MODE_NTSC``, ``VIDEO_MODE_SECAM`` and
``VIDEO_MODE_AUTO`` (3). Namely N/PAL Argentina, M/PAL, N/PAL, and NTSC
Japan with numbers 3-6 (sic).
(指示受支持视频制式的 ``VIDEO_TUNER_PAL``、``VIDEO_TUNER_NTSC`` 和
``VIDEO_TUNER_SECAM`` 标志被移除。这些信息现在包含在关联的 struct
:c:type:`v4l2_input` 中。指示视频制式是否可切换的 ``VIDEO_TUNER_NORM``
标志没有替代品。用于选择不同视频制式的 ``mode`` 字段被 :ref:`standard`
中描述的一整套全新 ioctl 和结构体取代。由于其无处不在，应当提及 BTTV 驱动
除常规的 ``VIDEO_MODE_PAL`` (0)、``VIDEO_MODE_NTSC``、
``VIDEO_MODE_SECAM`` 和 ``VIDEO_MODE_AUTO`` (3) 之外还支持几种制式：即
N/PAL Argentina、M/PAL、N/PAL 和 NTSC Japan，编号 3-6（原文如此）。)

The ``VIDEO_TUNER_STEREO_ON`` flag indicating stereo reception became
``V4L2_TUNER_SUB_STEREO`` in field ``rxsubchans``. This field also
permits the detection of monaural and bilingual audio, see the
definition of struct :c:type:`v4l2_tuner` for details.
Presently no replacement exists for the ``VIDEO_TUNER_RDS_ON`` and
``VIDEO_TUNER_MBS_ON`` flags.
(指示立体声接收的 ``VIDEO_TUNER_STEREO_ON`` 标志变成了 ``rxsubchans``
字段中的 ``V4L2_TUNER_SUB_STEREO``。该字段还允许检测单声道与双语音频，
详情参见 struct :c:type:`v4l2_tuner` 的定义。目前 ``VIDEO_TUNER_RDS_ON``
和 ``VIDEO_TUNER_MBS_ON`` 标志尚无替代品。)

The ``VIDEO_TUNER_LOW`` flag was renamed to ``V4L2_TUNER_CAP_LOW`` in
the struct :c:type:`v4l2_tuner` ``capability`` field.
( ``VIDEO_TUNER_LOW`` 标志在 struct :c:type:`v4l2_tuner` 的
``capability`` 字段中被更名为 ``V4L2_TUNER_CAP_LOW``。)

The ``VIDIOCGFREQ`` and ``VIDIOCSFREQ`` ioctl to change the tuner
frequency where renamed to
:ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` and
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>`. They take a pointer
to a struct :c:type:`v4l2_frequency` instead of an
unsigned long integer.
(更改调谐器频率的 ``VIDIOCGFREQ`` 和 ``VIDIOCSFREQ`` ioctl 被更名为
:ref:`VIDIOC_G_FREQUENCY <VIDIOC_G_FREQUENCY>` 和
:ref:`VIDIOC_S_FREQUENCY <VIDIOC_G_FREQUENCY>`。它们接受指向 struct
:c:type:`v4l2_frequency` 的指针，而不是无符号长整数。)

.. _v4l-image-properties:

Image Properties(图像属性)
==========================

V4L2 has no equivalent of the ``VIDIOCGPICT`` and ``VIDIOCSPICT`` ioctl
and struct ``video_picture``. The following fields where
replaced by V4L2 controls accessible with the
:ref:`VIDIOC_QUERYCTRL`,
:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` and
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls:
(V4L2 没有 ``VIDIOCGPICT`` 和 ``VIDIOCSPICT`` ioctl 以及 struct
``video_picture`` 的等价物。以下字段被 V4L2 控制项取代，这些控制项可通过
:ref:`VIDIOC_QUERYCTRL`、:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` 和
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl 访问：)


.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - struct ``video_picture``
      - V4L2 Control ID(V4L2 控制项 ID)
    * - ``brightness``
      - ``V4L2_CID_BRIGHTNESS``
    * - ``hue``
      - ``V4L2_CID_HUE``
    * - ``colour``
      - ``V4L2_CID_SATURATION``
    * - ``contrast``
      - ``V4L2_CID_CONTRAST``
    * - ``whiteness``
      - ``V4L2_CID_WHITENESS``

The V4L picture controls are assumed to range from 0 to 65535 with no
particular reset value. The V4L2 API permits arbitrary limits and
defaults which can be queried with the
:ref:`VIDIOC_QUERYCTRL` ioctl. For general
information about controls see :ref:`control`.
(V4L 图像控制项假定范围为 0 到 65535，没有特定的复位值。V4L2 API 允许任意
的限制与默认值，可以用 :ref:`VIDIOC_QUERYCTRL` ioctl 查询。关于控制项的
一般信息参见 :ref:`control`。)

The ``depth`` (average number of bits per pixel) of a video image is
implied by the selected image format. V4L2 does not explicitly provide
such information assuming applications recognizing the format are aware
of the image depth and others need not know. The ``palette`` field moved
into the struct :c:type:`v4l2_pix_format`:
(视频图像的 ``depth``（每像素平均位数）由所选图像格式隐含给出。V4L2 不显式
提供此类信息，假定能识别格式的应用程序知道图像深度，其他应用程序无需知道。
``palette`` 字段移入了 struct :c:type:`v4l2_pix_format`：)


.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - struct ``video_picture`` ``palette``
      - struct :c:type:`v4l2_pix_format` ``pixfmt``
    * - ``VIDEO_PALETTE_GREY``
      - :ref:`V4L2_PIX_FMT_GREY <V4L2-PIX-FMT-GREY>`
    * - ``VIDEO_PALETTE_HI240``
      - :ref:`V4L2_PIX_FMT_HI240 <pixfmt-reserved>` [#f3]_
    * - ``VIDEO_PALETTE_RGB565``
      - :ref:`V4L2_PIX_FMT_RGB565 <pixfmt-rgb>`
    * - ``VIDEO_PALETTE_RGB555``
      - :ref:`V4L2_PIX_FMT_RGB555 <pixfmt-rgb>`
    * - ``VIDEO_PALETTE_RGB24``
      - :ref:`V4L2_PIX_FMT_BGR24 <pixfmt-rgb>`
    * - ``VIDEO_PALETTE_RGB32``
      - :ref:`V4L2_PIX_FMT_BGR32 <pixfmt-rgb>` [#f4]_
    * - ``VIDEO_PALETTE_YUV422``
      - :ref:`V4L2_PIX_FMT_YUYV <V4L2-PIX-FMT-YUYV>`
    * - ``VIDEO_PALETTE_YUYV``\  [#f5]_
      - :ref:`V4L2_PIX_FMT_YUYV <V4L2-PIX-FMT-YUYV>`
    * - ``VIDEO_PALETTE_UYVY``
      - :ref:`V4L2_PIX_FMT_UYVY <V4L2-PIX-FMT-UYVY>`
    * - ``VIDEO_PALETTE_YUV420``
      - None
    * - ``VIDEO_PALETTE_YUV411``
      - :ref:`V4L2_PIX_FMT_Y41P <V4L2-PIX-FMT-Y41P>` [#f6]_
    * - ``VIDEO_PALETTE_RAW``
      - None [#f7]_
    * - ``VIDEO_PALETTE_YUV422P``
      - :ref:`V4L2_PIX_FMT_YUV422P <V4L2-PIX-FMT-YUV422P>`
    * - ``VIDEO_PALETTE_YUV411P``
      - :ref:`V4L2_PIX_FMT_YUV411P <V4L2-PIX-FMT-YUV411P>` [#f8]_
    * - ``VIDEO_PALETTE_YUV420P``
      - :ref:`V4L2_PIX_FMT_YVU420 <V4L2-PIX-FMT-YVU420>`
    * - ``VIDEO_PALETTE_YUV410P``
      - :ref:`V4L2_PIX_FMT_YVU410 <V4L2-PIX-FMT-YVU410>`

V4L2 image formats are defined in :ref:`pixfmt`. The image format can
be selected with the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
(V4L2 图像格式定义在 :ref:`pixfmt` 中。图像格式可以用
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 选择。)

Audio(音频)
===========

The ``VIDIOCGAUDIO`` and ``VIDIOCSAUDIO`` ioctl and struct
``video_audio`` are used to enumerate the audio inputs
of a V4L device. The equivalent V4L2 ioctls are
:ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` and
:ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>` using struct
:c:type:`v4l2_audio` as discussed in :ref:`audio`.
( ``VIDIOCGAUDIO`` 和 ``VIDIOCSAUDIO`` ioctl 以及 struct ``video_audio``
用于枚举 V4L 设备的音频输入。等价的 V4L2 ioctl 是使用 struct
:c:type:`v4l2_audio` 的 :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` 和
:ref:`VIDIOC_S_AUDIO <VIDIOC_G_AUDIO>`，在 :ref:`audio` 中讨论。)

The ``audio`` "channel number" field counting audio inputs was renamed
to ``index``.
(用于计数的 ``audio`` "信道号"字段更名为 ``index``。)

On ``VIDIOCSAUDIO`` the ``mode`` field selects *one* of the
``VIDEO_SOUND_MONO``, ``VIDEO_SOUND_STEREO``, ``VIDEO_SOUND_LANG1`` or
``VIDEO_SOUND_LANG2`` audio demodulation modes. When the current audio
standard is BTSC ``VIDEO_SOUND_LANG2`` refers to SAP and
``VIDEO_SOUND_LANG1`` is meaningless. Also undocumented in the V4L
specification, there is no way to query the selected mode. On
``VIDIOCGAUDIO`` the driver returns the *actually received* audio
programmes in this field. In the V4L2 API this information is stored in
the struct :c:type:`v4l2_tuner` ``rxsubchans`` and
``audmode`` fields, respectively. See :ref:`tuner` for more
information on tuners. Related to audio modes struct
:c:type:`v4l2_audio` also reports if this is a mono or
stereo input, regardless if the source is a tuner.
(在 ``VIDIOCSAUDIO`` 上， ``mode`` 字段选择 ``VIDEO_SOUND_MONO``、
``VIDEO_SOUND_STEREO``、``VIDEO_SOUND_LANG1`` 或 ``VIDEO_SOUND_LANG2``
音频解调模式之 *一*。当当前音频制式为 BTSC 时， ``VIDEO_SOUND_LANG2``
指 SAP，而 ``VIDEO_SOUND_LANG1`` 无意义。V4L 规范同样未记载的是：没有办法
查询所选模式。在 ``VIDIOCGAUDIO`` 上，驱动在该字段中返回 *实际接收到的*
音频节目。在 V4L2 API 中，这些信息分别存储在 struct
:c:type:`v4l2_tuner` 的 ``rxsubchans`` 和 ``audmode`` 字段中。关于调谐器
的更多信息参见 :ref:`tuner`。与音频模式相关的是，struct
:c:type:`v4l2_audio` 还报告该输入是单声道还是立体声，无论其来源是否为
调谐器。)

The following fields where replaced by V4L2 controls accessible with the
:ref:`VIDIOC_QUERYCTRL`,
:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` and
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls:
(以下字段被 V4L2 控制项取代，这些控制项可通过 :ref:`VIDIOC_QUERYCTRL`、
:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` 和
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl 访问：)


.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - struct ``video_audio``
      - V4L2 Control ID(V4L2 控制项 ID)
    * - ``volume``
      - ``V4L2_CID_AUDIO_VOLUME``
    * - ``bass``
      - ``V4L2_CID_AUDIO_BASS``
    * - ``treble``
      - ``V4L2_CID_AUDIO_TREBLE``
    * - ``balance``
      - ``V4L2_CID_AUDIO_BALANCE``

To determine which of these controls are supported by a driver V4L
provides the ``flags`` ``VIDEO_AUDIO_VOLUME``, ``VIDEO_AUDIO_BASS``,
``VIDEO_AUDIO_TREBLE`` and ``VIDEO_AUDIO_BALANCE``. In the V4L2 API the
:ref:`VIDIOC_QUERYCTRL` ioctl reports if the
respective control is supported. Accordingly the ``VIDEO_AUDIO_MUTABLE``
and ``VIDEO_AUDIO_MUTE`` flags where replaced by the boolean
``V4L2_CID_AUDIO_MUTE`` control.
(为确定驱动支持这些控制项中的哪些，V4L 提供了 ``flags``
``VIDEO_AUDIO_VOLUME``、``VIDEO_AUDIO_BASS``、``VIDEO_AUDIO_TREBLE`` 和
``VIDEO_AUDIO_BALANCE``。在 V4L2 API 中，:ref:`VIDIOC_QUERYCTRL` ioctl
报告相应控制项是否受支持。相应地， ``VIDEO_AUDIO_MUTABLE`` 和
``VIDEO_AUDIO_MUTE`` 标志被布尔型 ``V4L2_CID_AUDIO_MUTE`` 控制项
取代。)

All V4L2 controls have a ``step`` attribute replacing the struct
``video_audio`` ``step`` field. The V4L audio controls
are assumed to range from 0 to 65535 with no particular reset value. The
V4L2 API permits arbitrary limits and defaults which can be queried with
the :ref:`VIDIOC_QUERYCTRL` ioctl. For general
information about controls see :ref:`control`.
(所有 V4L2 控制项都有 ``step`` 属性，取代 struct ``video_audio`` 的
``step`` 字段。V4L 音频控制项假定范围为 0 到 65535，没有特定的复位值。
V4L2 API 允许任意的限制与默认值，可以用 :ref:`VIDIOC_QUERYCTRL` ioctl
查询。关于控制项的一般信息参见 :ref:`control`。)

Frame Buffer Overlay(帧缓冲叠加)
================================

The V4L2 ioctls equivalent to ``VIDIOCGFBUF`` and ``VIDIOCSFBUF`` are
:ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` and
:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`. The ``base`` field of struct
``video_buffer`` remained unchanged, except V4L2 defines
a flag to indicate non-destructive overlays instead of a ``NULL``
pointer. All other fields moved into the struct
:c:type:`v4l2_pix_format` ``fmt`` substructure of
struct :c:type:`v4l2_framebuffer`. The ``depth``
field was replaced by ``pixelformat``. See :ref:`pixfmt-rgb` for a
list of RGB formats and their respective color depths.
(等价于 ``VIDIOCGFBUF`` 和 ``VIDIOCSFBUF`` 的 V4L2 ioctl 是
:ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 和
:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`。struct ``video_buffer`` 的
``base`` 字段保持不变，只是 V4L2 定义了一个标志来指示非破坏性叠加，而不是
使用 ``NULL`` 指针。所有其他字段移入了 struct :c:type:`v4l2_framebuffer`
的 struct :c:type:`v4l2_pix_format` ``fmt`` 子结构。 ``depth`` 字段被
``pixelformat`` 取代。RGB 格式及其各自颜色深度的列表参见
:ref:`pixfmt-rgb`。)

Instead of the special ioctls ``VIDIOCGWIN`` and ``VIDIOCSWIN`` V4L2
uses the general-purpose data format negotiation ioctls
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`. They take a pointer to a struct
:c:type:`v4l2_format` as argument. Here the ``win`` member
of the ``fmt`` union is used, a struct
:c:type:`v4l2_window`.
(V4L2 不再使用特殊的 ``VIDIOCGWIN`` 和 ``VIDIOCSWIN`` ioctl，而是使用
通用的数据格式协商 ioctl :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`。它们接受指向 struct
:c:type:`v4l2_format` 的指针作为参数。这里使用 ``fmt`` 联合体的 ``win``
成员，即一个 struct :c:type:`v4l2_window`。)

The ``x``, ``y``, ``width`` and ``height`` fields of struct
``video_window`` moved into struct
:c:type:`v4l2_rect` substructure ``w`` of struct
:c:type:`v4l2_window`. The ``chromakey``, ``clips``, and
``clipcount`` fields remained unchanged. Struct
``video_clip`` was renamed to struct
:c:type:`v4l2_clip`, also containing a struct
:c:type:`v4l2_rect`, but the semantics are still the same.
(struct ``video_window`` 的 ``x``、``y``、``width`` 和 ``height`` 字段
移入了 struct :c:type:`v4l2_window` 的 struct :c:type:`v4l2_rect`
子结构 ``w``。 ``chromakey``、``clips`` 和 ``clipcount`` 字段保持不变。
struct ``video_clip`` 被更名为 struct :c:type:`v4l2_clip`，同样包含一个
struct :c:type:`v4l2_rect`，但语义仍然相同。)

The ``VIDEO_WINDOW_INTERLACE`` flag was dropped. Instead applications
must set the ``field`` field to ``V4L2_FIELD_ANY`` or
``V4L2_FIELD_INTERLACED``. The ``VIDEO_WINDOW_CHROMAKEY`` flag moved
into struct :c:type:`v4l2_framebuffer`, under the new
name ``V4L2_FBUF_FLAG_CHROMAKEY``.
( ``VIDEO_WINDOW_INTERLACE`` 标志被移除。取而代之，应用程序必须把
``field`` 字段设置为 ``V4L2_FIELD_ANY`` 或 ``V4L2_FIELD_INTERLACED``。
``VIDEO_WINDOW_CHROMAKEY`` 标志移入了 struct
:c:type:`v4l2_framebuffer`，新名称为 ``V4L2_FBUF_FLAG_CHROMAKEY``。)

In V4L, storing a bitmap pointer in ``clips`` and setting ``clipcount``
to ``VIDEO_CLIP_BITMAP`` (-1) requests bitmap clipping, using a fixed
size bitmap of 1024 × 625 bits. Struct :c:type:`v4l2_window`
has a separate ``bitmap`` pointer field for this purpose and the bitmap
size is determined by ``w.width`` and ``w.height``.
(在 V4L 中，把位图指针存储在 ``clips`` 中并把 ``clipcount`` 设置为
``VIDEO_CLIP_BITMAP`` (-1) 即请求位图剪裁，使用固定大小 1024 × 625 位的
位图。struct :c:type:`v4l2_window` 为此提供一个单独的 ``bitmap`` 指针
字段，位图大小由 ``w.width`` 和 ``w.height`` 决定。)

The ``VIDIOCCAPTURE`` ioctl to enable or disable overlay was renamed to
:ref:`VIDIOC_OVERLAY`.
(用于启用或禁用叠加的 ``VIDIOCCAPTURE`` ioctl 被更名为
:ref:`VIDIOC_OVERLAY`。)

Cropping(裁剪)
==============

To capture only a subsection of the full picture V4L defines the
``VIDIOCGCAPTURE`` and ``VIDIOCSCAPTURE`` ioctls using struct
``video_capture``. The equivalent V4L2 ioctls are
:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` and
:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` using struct
:c:type:`v4l2_crop`, and the related
:ref:`VIDIOC_CROPCAP` ioctl. This is a rather
complex matter, see :ref:`crop` for details.
(为了只捕获整幅图像的一个子区域，V4L 定义了使用 struct
``video_capture`` 的 ``VIDIOCGCAPTURE`` 和 ``VIDIOCSCAPTURE`` ioctl。
等价的 V4L2 ioctl 是使用 struct :c:type:`v4l2_crop` 的
:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` 和
:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>`，以及相关的
:ref:`VIDIOC_CROPCAP` ioctl。这是相当复杂的问题，详情参见
:ref:`crop`。)

The ``x``, ``y``, ``width`` and ``height`` fields moved into struct
:c:type:`v4l2_rect` substructure ``c`` of struct
:c:type:`v4l2_crop`. The ``decimation`` field was dropped. In
the V4L2 API the scaling factor is implied by the size of the cropping
rectangle and the size of the captured or overlaid image.
( ``x``、``y``、``width`` 和 ``height`` 字段移入了 struct
:c:type:`v4l2_crop` 的 struct :c:type:`v4l2_rect` 子结构 ``c``。
``decimation`` 字段被移除。在 V4L2 API 中，缩放因子由裁剪矩形的尺寸与
捕获或叠加图像的尺寸隐含给出。)

The ``VIDEO_CAPTURE_ODD`` and ``VIDEO_CAPTURE_EVEN`` flags to capture
only the odd or even field, respectively, were replaced by
``V4L2_FIELD_TOP`` and ``V4L2_FIELD_BOTTOM`` in the field named
``field`` of struct :c:type:`v4l2_pix_format` and
struct :c:type:`v4l2_window`. These structures are used to
select a capture or overlay format with the
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
(分别用于只捕获奇场或偶场的 ``VIDEO_CAPTURE_ODD`` 和
``VIDEO_CAPTURE_EVEN`` 标志，被 struct :c:type:`v4l2_pix_format` 和
struct :c:type:`v4l2_window` 中名为 ``field`` 的字段里的
``V4L2_FIELD_TOP`` 和 ``V4L2_FIELD_BOTTOM`` 取代。这些结构体用于通过
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 选择捕获或叠加格式。)

Reading Images, Memory Mapping(读取图像与内存映射)
==================================================

Capturing using the read method(通过 read 方法捕获)
---------------------------------------------------

There is no essential difference between reading images from a V4L or
V4L2 device using the :c:func:`read()` function, however V4L2
drivers are not required to support this I/O method. Applications can
determine if the function is available with the
:ref:`VIDIOC_QUERYCAP` ioctl. All V4L2 devices
exchanging data with applications must support the
:c:func:`select()` and :c:func:`poll()`
functions.
(使用 :c:func:`read()` 函数从 V4L 或 V4L2 设备读取图像没有本质区别，
但 V4L2 驱动不要求支持这种 I/O 方法。应用程序可以用
:ref:`VIDIOC_QUERYCAP` ioctl 确定该函数是否可用。所有与应用程序交换数据
的 V4L2 设备都必须支持 :c:func:`select()` 和 :c:func:`poll()`
函数。)

To select an image format and size, V4L provides the ``VIDIOCSPICT`` and
``VIDIOCSWIN`` ioctls. V4L2 uses the general-purpose data format
negotiation ioctls :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`. They take a pointer to a struct
:c:type:`v4l2_format` as argument, here the struct
:c:type:`v4l2_pix_format` named ``pix`` of its
``fmt`` union is used.
(要选择图像格式与尺寸，V4L 提供 ``VIDIOCSPICT`` 和 ``VIDIOCSWIN`` ioctl。
V4L2 使用通用的数据格式协商 ioctl :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`。它们接受指向 struct
:c:type:`v4l2_format` 的指针作为参数，这里使用其 ``fmt`` 联合体中名为
``pix`` 的 struct :c:type:`v4l2_pix_format`。)

For more information about the V4L2 read interface see :ref:`rw`.
(关于 V4L2 read 接口的更多信息参见 :ref:`rw`。)

Capturing using memory mapping(通过内存映射捕获)
------------------------------------------------

Applications can read from V4L devices by mapping buffers in device
memory, or more often just buffers allocated in DMA-able system memory,
into their address space. This avoids the data copying overhead of the
read method. V4L2 supports memory mapping as well, with a few
differences.
(应用程序可以通过把设备内存中的缓冲区——或更常见的、分配在可 DMA 的系统
内存中的缓冲区——映射到自己的地址空间来从 V4L 设备读取。这避免了 read 方法
的数据复制开销。V4L2 同样支持内存映射，但有几点差异。)


.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - V4L
      - V4L2
    * -
      - The image format must be selected before buffers are allocated,
	with the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl. When no
	format is selected the driver may use the last, possibly by
	another application requested format.
	(必须在分配缓冲区之前用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 选择
	图像格式。如果未选择格式，驱动可能使用上一次——可能是另一个应用
	程序请求的——格式。)
    * - Applications cannot change the number of buffers. The it is built
	into the driver, unless it has a module option to change the
	number when the driver module is loaded.
	(应用程序不能更改缓冲区数量。缓冲区数量内建于驱动中，除非驱动模块
	加载时有更改数量的模块选项。)
      - The :ref:`VIDIOC_REQBUFS` ioctl allocates the
	desired number of buffers, this is a required step in the
	initialization sequence.
	( :ref:`VIDIOC_REQBUFS` ioctl 分配所需数量的缓冲区，这是初始化序列
	中必需的一步。)
    * - Drivers map all buffers as one contiguous range of memory. The
	``VIDIOCGMBUF`` ioctl is available to query the number of buffers,
	the offset of each buffer from the start of the virtual file, and
	the overall amount of memory used, which can be used as arguments
	for the :c:func:`mmap()` function.
	(驱动把所有缓冲区作为一段连续内存映射。 ``VIDIOCGMBUF`` ioctl 可用于
	查询缓冲区数量、每个缓冲区相对虚拟文件起始的偏移以及所用内存总量，
	这些可以作为 :c:func:`mmap()` 函数的参数。)
      - Buffers are individually mapped. The offset and size of each
	buffer can be determined with the
	:ref:`VIDIOC_QUERYBUF` ioctl.
	(缓冲区被单独映射。每个缓冲区的偏移与大小可以用
	:ref:`VIDIOC_QUERYBUF` ioctl 确定。)
    * - The ``VIDIOCMCAPTURE`` ioctl prepares a buffer for capturing. It
	also determines the image format for this buffer. The ioctl
	returns immediately, eventually with an ``EAGAIN`` error code if no
	video signal had been detected. When the driver supports more than
	one buffer applications can call the ioctl multiple times and thus
	have multiple outstanding capture requests.

	The ``VIDIOCSYNC`` ioctl suspends execution until a particular
	buffer has been filled.
	( ``VIDIOCMCAPTURE`` ioctl 为捕获准备一个缓冲区，同时确定此缓冲区的
	图像格式。该 ioctl 立即返回，如果未检测到视频信号，可能带 ``EAGAIN``
	错误码。当驱动支持多于一个缓冲区时，应用程序可以多次调用该 ioctl，
	从而有多个未完成的捕获请求。

	``VIDIOCSYNC`` ioctl 会挂起执行，直到特定缓冲区被填充完毕。)
      - Drivers maintain an incoming and outgoing queue.
	:ref:`VIDIOC_QBUF` enqueues any empty buffer into
	the incoming queue. Filled buffers are dequeued from the outgoing
	queue with the :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. To wait
	until filled buffers become available this function,
	:c:func:`select()` or :c:func:`poll()` can
	be used. The :ref:`VIDIOC_STREAMON` ioctl
	must be called once after enqueuing one or more buffers to start
	capturing. Its counterpart
	:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` stops capturing and
	dequeues all buffers from both queues. Applications can query the
	signal status, if known, with the
	:ref:`VIDIOC_ENUMINPUT` ioctl.
	(驱动维护一个入队列和一个出队列。 :ref:`VIDIOC_QBUF` 把任何空缓冲区
	排入入队列。已填充的缓冲区用 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl
	从出队列取出。要等待已填充缓冲区可用，可以使用该函数、
	:c:func:`select()` 或 :c:func:`poll()`。把一个或多个缓冲区入队之后
	必须调用一次 :ref:`VIDIOC_STREAMON` ioctl 来开始捕获。与之对应的
	:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 停止捕获并把所有缓冲区从
	两个队列中取出。应用程序可以用 :ref:`VIDIOC_ENUMINPUT` ioctl 查询
	信号状态（如果已知）。)

For a more in-depth discussion of memory mapping and examples, see
:ref:`mmap`.
(关于内存映射的更深入讨论与示例，参见 :ref:`mmap`。)

Reading Raw VBI Data(读取原始 VBI 数据)
=======================================

Originally the V4L API did not specify a raw VBI capture interface, only
the device file ``/dev/vbi`` was reserved for this purpose. The only
driver supporting this interface was the BTTV driver, de-facto defining
the V4L VBI interface. Reading from the device yields a raw VBI image
with the following parameters:
(最初 V4L API 并未规定原始 VBI 捕获接口，只有设备文件 ``/dev/vbi`` 为此
目的保留。支持此接口的唯一驱动是 BTTV 驱动，它事实上定义了 V4L VBI 接口。
从设备读取会得到具有以下参数的原始 VBI 图像：)


.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * - struct :c:type:`v4l2_vbi_format`
      - V4L, BTTV driver
    * - sampling_rate
      - 28636363 Hz NTSC (or any other 525-line standard); 35468950 Hz PAL
	and SECAM (625-line standards)
	(28636363 Hz NTSC（或任何其他 525 行制式）；35468950 Hz PAL 与 SECAM
	（625 行制式）)
    * - offset
      - ?
    * - samples_per_line
      - 2048
    * - sample_format
      - V4L2_PIX_FMT_GREY. The last four bytes (a machine endianness
	integer) contain a frame counter.
	(V4L2_PIX_FMT_GREY。最后四个字节（一个按机器字节序的整数）包含帧
	计数器。)
    * - start[]
      - 10, 273 NTSC; 22, 335 PAL and SECAM
	(10, 273 NTSC；22, 335 PAL 与 SECAM)
    * - count[]
      - 16, 16 [#f9]_
    * - flags
      - 0

Undocumented in the V4L specification, in Linux 2.3 the
``VIDIOCGVBIFMT`` and ``VIDIOCSVBIFMT`` ioctls using struct
``vbi_format`` were added to determine the VBI image
parameters. These ioctls are only partially compatible with the V4L2 VBI
interface specified in :ref:`raw-vbi`.
(V4L 规范未记载的是：Linux 2.3 中加入了使用 struct ``vbi_format`` 的
``VIDIOCGVBIFMT`` 和 ``VIDIOCSVBIFMT`` ioctl 来确定 VBI 图像参数。这些
ioctl 与 :ref:`raw-vbi` 中规定的 V4L2 VBI 接口只部分兼容。)

An ``offset`` field does not exist, ``sample_format`` is supposed to be
``VIDEO_PALETTE_RAW``, equivalent to ``V4L2_PIX_FMT_GREY``. The
remaining fields are probably equivalent to struct
:c:type:`v4l2_vbi_format`.
(不存在 ``offset`` 字段； ``sample_format`` 应为 ``VIDEO_PALETTE_RAW``，
等价于 ``V4L2_PIX_FMT_GREY``。其余字段可能与 struct
:c:type:`v4l2_vbi_format` 等价。)

Apparently only the Zoran (ZR 36120) driver implements these ioctls. The
semantics differ from those specified for V4L2 in two ways. The
parameters are reset on :c:func:`open()` and
``VIDIOCSVBIFMT`` always returns an ``EINVAL`` error code if the parameters
are invalid.
(显然只有 Zoran (ZR 36120) 驱动实现了这些 ioctl。其语义在两点上不同于为
V4L2 规定的语义：参数在 :c:func:`open()` 时被重置，并且如果参数无效，
``VIDIOCSVBIFMT`` 总是返回 ``EINVAL`` 错误码。)

Miscellaneous(杂项)
===================

V4L2 has no equivalent of the ``VIDIOCGUNIT`` ioctl. Applications can
find the VBI device associated with a video capture device (or vice
versa) by reopening the device and requesting VBI data. For details see
:ref:`open`.
(V4L2 没有 ``VIDIOCGUNIT`` ioctl 的等价物。应用程序可以通过重新打开设备并
请求 VBI 数据，找到与视频捕获设备关联的 VBI 设备（或反之）。详情参见
:ref:`open`。)

No replacement exists for ``VIDIOCKEY``, and the V4L functions for
microcode programming. A new interface for MPEG compression and playback
devices is documented in :ref:`extended-controls`.
( ``VIDIOCKEY`` 以及 V4L 的微码编程函数没有替代品。用于 MPEG 压缩与回放
设备的新接口在 :ref:`extended-controls` 中说明。)

.. [#f1]
   According to Documentation/admin-guide/devices.rst these should be symbolic links
   to ``/dev/video0``. Note the original bttv interface is not
   compatible with V4L or V4L2.
   (根据 Documentation/admin-guide/devices.rst，这些应是指向
   ``/dev/video0`` 的符号链接。注意最初的 bttv 接口与 V4L 或 V4L2 都不
   兼容。)

.. [#f2]
   According to ``Documentation/admin-guide/devices.rst`` a symbolic link to
   ``/dev/radio0``.
   (根据 ``Documentation/admin-guide/devices.rst``，是指向
   ``/dev/radio0`` 的符号链接。)

.. [#f3]
   This is a custom format used by the BTTV driver, not one of the V4L2
   standard formats.
   (这是 BTTV 驱动使用的自定义格式，不是 V4L2 标准格式之一。)

.. [#f4]
   Presumably all V4L RGB formats are little-endian, although some
   drivers might interpret them according to machine endianness. V4L2
   defines little-endian, big-endian and red/blue swapped variants. For
   details see :ref:`pixfmt-rgb`.
   (据推测，所有 V4L RGB 格式都是小端序，尽管有些驱动可能按机器字节序解释
   它们。V4L2 定义了小端序、大端序以及红蓝交换的变体。详情参见
   :ref:`pixfmt-rgb`。)

.. [#f5]
   ``VIDEO_PALETTE_YUV422`` and ``VIDEO_PALETTE_YUYV`` are the same
   formats. Some V4L drivers respond to one, some to the other.
   ( ``VIDEO_PALETTE_YUV422`` 和 ``VIDEO_PALETTE_YUYV`` 是相同的格式。
   一些 V4L 驱动响应其中一个，另一些响应另一个。)

.. [#f6]
   Not to be confused with ``V4L2_PIX_FMT_YUV411P``, which is a planar
   format.
   (不要与 ``V4L2_PIX_FMT_YUV411P`` 混淆，后者是平面 (planar) 格式。)

.. [#f7]
   V4L explains this as: "RAW capture (BT848)"
   (V4L 对此的解释是："RAW capture (BT848)")

.. [#f8]
   Not to be confused with ``V4L2_PIX_FMT_Y41P``, which is a packed
   format.
   (不要与 ``V4L2_PIX_FMT_Y41P`` 混淆，后者是紧缩 (packed) 格式。)

.. [#f9]
   Old driver versions used different values, eventually the custom
   ``BTTV_VBISIZE`` ioctl was added to query the correct values.
   (旧驱动版本使用不同的值；最终加入了自定义的 ``BTTV_VBISIZE`` ioctl 来
   查询正确的值。)
