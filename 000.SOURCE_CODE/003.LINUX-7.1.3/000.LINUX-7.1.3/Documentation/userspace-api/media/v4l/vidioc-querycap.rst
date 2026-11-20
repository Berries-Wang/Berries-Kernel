.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_QUERYCAP:

********************************************
ioctl VIDIOC_QUERYCAP(ioctl VIDIOC_QUERYCAP)
********************************************

Name(名称)
==========

VIDIOC_QUERYCAP - Query device capabilities
(VIDIOC_QUERYCAP - 查询设备能力)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_QUERYCAP

``int ioctl(int fd, VIDIOC_QUERYCAP, struct v4l2_capability *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 open() 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_capability`.
    (指向 struct v4l2_capability 的指针。)

Description(描述)
=================

All V4L2 devices support the ``VIDIOC_QUERYCAP`` ioctl. It is used to
identify kernel devices compatible with this specification and to obtain
information about driver and hardware capabilities. The ioctl takes a
pointer to a struct :c:type:`v4l2_capability` which is
filled by the driver. When the driver is not compatible with this
specification the ioctl returns an ``EINVAL`` error code.
(所有 V4L2 设备都支持 ``VIDIOC_QUERYCAP`` ioctl。它用于识别与本规范兼容
的内核设备，并获取关于驱动与硬件能力的信息。该 ioctl 接受一个指向
struct v4l2_capability 的指针，该结构由驱动填充。当驱动与本规范不兼容
时，该 ioctl 返回 ``EINVAL`` 错误码。)

.. c:type:: v4l2_capability

.. tabularcolumns:: |p{1.4cm}|p{2.8cm}|p{13.1cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_capability
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 4 20

    * - __u8
      - ``driver``\ [16]
      - Name of the driver, a unique NUL-terminated ASCII string. For
	example: "bttv". Driver specific applications can use this
	information to verify the driver identity. It is also useful to
	work around known bugs, or to identify drivers in error reports.
	(驱动名称，为一个唯一的、以 NUL 结尾的 ASCII 字符串。例如："bttv"。
	针对特定驱动的应用程序可以用此信息来验证驱动的身份。它在规避已知
	bug 或在错误报告中识别驱动时也很有用。)

	Storing strings in fixed sized arrays is bad practice but
	unavoidable here. Drivers and applications should take precautions
	to never read or write beyond the end of the array and to make
	sure the strings are properly NUL-terminated.
	(把字符串存储在定长数组中是糟糕的做法，但在这里不可避免。驱动和
	应用程序应当采取预防措施，绝不读写超出数组末尾的内容，并确保字符串
	正确地以 NUL 结尾。)
    * - __u8
      - ``card``\ [32]
      - Name of the device, a NUL-terminated UTF-8 string. For example:
	"Yoyodyne TV/FM". One driver may support different brands or
	models of video hardware. This information is intended for users,
	for example in a menu of available devices. Since multiple TV
	cards of the same brand may be installed which are supported by
	the same driver, this name should be combined with the character
	device file name (e. g. ``/dev/video2``) or the ``bus_info``
	string to avoid ambiguities.
	(设备名称，为一个以 NUL 结尾的 UTF-8 字符串。例如："Yoyodyne
	TV/FM"。一个驱动可能支持不同品牌或型号的视频硬件。此信息面向用户，
	例如出现在可用设备的菜单中。由于同一品牌的多块电视卡可能被安装且
	由同一个驱动支持，此名称应当与字符设备文件名 (例如 ``/dev/video2``)
	或 ``bus_info`` 字符串组合使用，以避免歧义。)
    * - __u8
      - ``bus_info``\ [32]
      - Location of the device in the system, a NUL-terminated ASCII
	string. For example: "PCI:0000:05:06.0". This information is
	intended for users, to distinguish multiple identical devices. If
	no such information is available the field must simply count the
	devices controlled by the driver ("platform:vivid-000"). The
	bus_info must start with "PCI:" for PCI boards, "PCIe:" for PCI
	Express boards, "usb-" for USB devices, "I2C:" for i2c devices,
	"ISA:" for ISA devices, "parport" for parallel port devices and
	"platform:" for platform devices.
	(设备在系统中的位置，为一个以 NUL 结尾的 ASCII 字符串。例如：
	"PCI:0000:05:06.0"。此信息面向用户，用于区分多个相同的设备。如果
	没有此类信息可用，该字段只需对驱动所控制的设备进行计数
	("platform:vivid-000")。bus_info 必须以 "PCI:" 开头 (PCI 板卡)、
	"PCIe:" 开头 (PCI Express 板卡)、"usb-" 开头 (USB 设备)、"I2C:" 开头
	(i2c 设备)、"ISA:" 开头 (ISA 设备)、"parport" 开头 (并口设备)、
	"platform:" 开头 (平台设备)。)
    * - __u32
      - ``version``
      - Version number of the driver.
	(驱动程序的版本号。)

	Starting with kernel 3.1, the version reported is provided by the
	V4L2 subsystem following the kernel numbering scheme. However, it
	may not always return the same version as the kernel if, for
	example, a stable or distribution-modified kernel uses the V4L2
	stack from a newer kernel.
	(从内核 3.1 开始，报告的版本由 V4L2 子系统按照内核编号方案提供。
	不过，它并不总是返回与内核相同的版本，例如当稳定版或发行版修改过的
	内核使用来自更新内核的 V4L2 栈时。)

	The version number is formatted using the ``KERNEL_VERSION()``
	macro. For example if the media stack corresponds to the V4L2
	version shipped with Kernel 4.14, it would be equivalent to:
	(版本号使用 ``KERNEL_VERSION()`` 宏来格式化。例如，如果媒体栈对应
	于内核 4.14 附带的 V4L2 版本，它将等价于：)
    * - :cspan:`2`

	``#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))``

	``__u32 version = KERNEL_VERSION(4, 14, 0);``

	``printf ("Version: %u.%u.%u\\n",``

	``(version >> 16) & 0xFF, (version >> 8) & 0xFF, version & 0xFF);``
    * - __u32
      - ``capabilities``
      - Available capabilities of the physical device as a whole, see
	:ref:`device-capabilities`. The same physical device can export
	multiple devices in /dev (e.g. /dev/videoX, /dev/vbiY and
	/dev/radioZ). The ``capabilities`` field should contain a union of
	all capabilities available around the several V4L2 devices
	exported to userspace. For all those devices the ``capabilities``
	field returns the same set of capabilities. This allows
	applications to open just one of the devices (typically the video
	device) and discover whether video, vbi and/or radio are also
	supported.
	(物理设备整体可用的能力，参见 device-capabilities 一节。同一个物理
	设备可以在 /dev 中导出多个设备 (例如 /dev/videoX、/dev/vbiY 和
	/dev/radioZ)。``capabilities`` 字段应当包含导出到用户空间的多个
	V4L2 设备周围所有可用能力的并集。对所有这些设备，``capabilities``
	字段都返回同一组能力。这使得应用程序只需打开其中一个设备 (通常是
	视频设备)，就能发现是否还支持 video、vbi 和/或 radio。)
    * - __u32
      - ``device_caps``
      - Device capabilities of the opened device, see
	:ref:`device-capabilities`. Should contain the available
	capabilities of that specific device node. So, for example,
	``device_caps`` of a radio device will only contain radio related
	capabilities and no video or vbi capabilities. This field is only
	set if the ``capabilities`` field contains the
	``V4L2_CAP_DEVICE_CAPS`` capability. Only the ``capabilities``
	field can have the ``V4L2_CAP_DEVICE_CAPS`` capability,
	``device_caps`` will never set ``V4L2_CAP_DEVICE_CAPS``.
	(已打开设备的能力，参见 device-capabilities 一节。应当包含该特定
	设备节点可用的能力。例如，radio 设备的 ``device_caps`` 将只包含与
	radio 相关的能力，而不包含 video 或 vbi 能力。只有当
	``capabilities`` 字段包含 ``V4L2_CAP_DEVICE_CAPS`` 能力时才会设置
	此字段。只有 ``capabilities`` 字段可以拥有
	``V4L2_CAP_DEVICE_CAPS`` 能力，``device_caps`` 永远不会设置
	``V4L2_CAP_DEVICE_CAPS``。)
    * - __u32
      - ``reserved``\ [3]
      - Reserved for future extensions. Drivers must set this array to
	zero.
	(预留给未来扩展。驱动必须将此数组设为 0。)


.. tabularcolumns:: |p{7.0cm}|p{2.6cm}|p{7.7cm}|

.. _device-capabilities:

.. cssclass:: longtable

.. flat-table:: Device Capabilities Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_CAP_VIDEO_CAPTURE``
      - 0x00000001
      - The device supports the single-planar API through the
	:ref:`Video Capture <capture>` interface.
	(设备通过 Video Capture (视频采集) 接口支持单平面 (single-planar)
	API。)
    * - ``V4L2_CAP_VIDEO_CAPTURE_MPLANE``
      - 0x00001000
      - The device supports the :ref:`multi-planar API <planar-apis>`
	through the :ref:`Video Capture <capture>` interface.
	(设备通过 Video Capture (视频采集) 接口支持多平面 (multi-planar)
	API。)
    * - ``V4L2_CAP_VIDEO_OUTPUT``
      - 0x00000002
      - The device supports the single-planar API through the
	:ref:`Video Output <output>` interface.
	(设备通过 Video Output (视频输出) 接口支持单平面 API。)
    * - ``V4L2_CAP_VIDEO_OUTPUT_MPLANE``
      - 0x00002000
      - The device supports the :ref:`multi-planar API <planar-apis>`
	through the :ref:`Video Output <output>` interface.
	(设备通过 Video Output (视频输出) 接口支持多平面 API。)
    * - ``V4L2_CAP_VIDEO_M2M``
      - 0x00008000
      - The device supports the single-planar API through the Video
	Memory-To-Memory interface.
	(设备通过 Video Memory-To-Memory (视频内存到内存) 接口支持单平面
	API。)
    * - ``V4L2_CAP_VIDEO_M2M_MPLANE``
      - 0x00004000
      - The device supports the :ref:`multi-planar API <planar-apis>`
	through the Video Memory-To-Memory interface.
	(设备通过 Video Memory-To-Memory 接口支持多平面 API。)
    * - ``V4L2_CAP_VIDEO_OVERLAY``
      - 0x00000004
      - The device supports the :ref:`Video Overlay <overlay>`
	interface. A video overlay device typically stores captured images
	directly in the video memory of a graphics card, with hardware
	clipping and scaling.
	(设备支持 Video Overlay (视频叠加) 接口。视频叠加设备通常把采集到的
	图像直接存储到显卡的视频内存中，并进行硬件裁剪与缩放。)
    * - ``V4L2_CAP_VBI_CAPTURE``
      - 0x00000010
      - The device supports the :ref:`Raw VBI Capture <raw-vbi>`
	interface, providing Teletext and Closed Caption data.
	(设备支持 Raw VBI Capture (原始 VBI 采集) 接口，提供 Teletext 与
	Closed Caption 数据。)
    * - ``V4L2_CAP_VBI_OUTPUT``
      - 0x00000020
      - The device supports the :ref:`Raw VBI Output <raw-vbi>`
	interface.
	(设备支持 Raw VBI Output (原始 VBI 输出) 接口。)
    * - ``V4L2_CAP_SLICED_VBI_CAPTURE``
      - 0x00000040
      - The device supports the :ref:`Sliced VBI Capture <sliced>`
	interface.
	(设备支持 Sliced VBI Capture (切片 VBI 采集) 接口。)
    * - ``V4L2_CAP_SLICED_VBI_OUTPUT``
      - 0x00000080
      - The device supports the :ref:`Sliced VBI Output <sliced>`
	interface.
	(设备支持 Sliced VBI Output (切片 VBI 输出) 接口。)
    * - ``V4L2_CAP_RDS_CAPTURE``
      - 0x00000100
      - The device supports the :ref:`RDS <rds>` capture interface.
	(设备支持 RDS 采集接口。)
    * - ``V4L2_CAP_VIDEO_OUTPUT_OVERLAY``
      - 0x00000200
      - The device supports the :ref:`Video Output Overlay <osd>` (OSD)
	interface. Unlike the *Video Overlay* interface, this is a
	secondary function of video output devices and overlays an image
	onto an outgoing video signal. When the driver sets this flag, it
	must clear the ``V4L2_CAP_VIDEO_OVERLAY`` flag and vice
	versa. [#f1]_ (设备支持 Video Output Overlay (视频输出叠加, OSD)
	接口。与 Video Overlay 接口不同，它是视频输出设备的次要功能，把一幅
	图像叠加到输出的视频信号上。当驱动设置此标志时，必须清除
	``V4L2_CAP_VIDEO_OVERLAY`` 标志，反之亦然。)
    * - ``V4L2_CAP_HW_FREQ_SEEK``
      - 0x00000400
      - The device supports the
	:ref:`VIDIOC_S_HW_FREQ_SEEK` ioctl
	for hardware frequency seeking.
	(设备支持 VIDIOC_S_HW_FREQ_SEEK ioctl 来进行硬件频率搜索。)
    * - ``V4L2_CAP_RDS_OUTPUT``
      - 0x00000800
      - The device supports the :ref:`RDS <rds>` output interface.
	(设备支持 RDS 输出接口。)
    * - ``V4L2_CAP_TUNER``
      - 0x00010000
      - The device has some sort of tuner to receive RF-modulated video
	signals. For more information about tuner programming see
	:ref:`tuner`.
	(设备具有某种调谐器来接收射频调制的视频信号。关于调谐器编程的更多
	信息参见 tuner 一节。)
    * - ``V4L2_CAP_AUDIO``
      - 0x00020000
      - The device has audio inputs or outputs. It may or may not support
	audio recording or playback, in PCM or compressed formats. PCM
	audio support must be implemented as ALSA or OSS interface. For
	more information on audio inputs and outputs see :ref:`audio`.
	(设备具有音频输入或输出。它可能支持也可能不支持音频录制或播放，
	PCM 或压缩格式皆可。PCM 音频支持必须实现为 ALSA 或 OSS 接口。关于
	音频输入与输出的更多信息参见 audio 一节。)
    * - ``V4L2_CAP_RADIO``
      - 0x00040000
      - This is a radio receiver.
	(这是一个收音机接收设备。)
    * - ``V4L2_CAP_MODULATOR``
      - 0x00080000
      - The device has some sort of modulator to emit RF-modulated
	video/audio signals. For more information about modulator
	programming see :ref:`tuner`.
	(设备具有某种调制器来发射射频调制的视频/音频信号。关于调制器编程的
	更多信息参见 tuner 一节。)
    * - ``V4L2_CAP_SDR_CAPTURE``
      - 0x00100000
      - The device supports the :ref:`SDR Capture <sdr>` interface.
	(设备支持 SDR Capture (SDR 采集) 接口。)
    * - ``V4L2_CAP_EXT_PIX_FORMAT``
      - 0x00200000
      - The device supports the struct
	:c:type:`v4l2_pix_format` extended fields.
	(设备支持 struct v4l2_pix_format 的扩展字段。)
    * - ``V4L2_CAP_SDR_OUTPUT``
      - 0x00400000
      - The device supports the :ref:`SDR Output <sdr>` interface.
	(设备支持 SDR Output (SDR 输出) 接口。)
    * - ``V4L2_CAP_META_CAPTURE``
      - 0x00800000
      - The device supports the :ref:`metadata` capture interface.
	(设备支持 metadata (元数据) 采集接口。)
    * - ``V4L2_CAP_READWRITE``
      - 0x01000000
      - The device supports the :c:func:`read()` and/or
	:c:func:`write()` I/O methods.
	(设备支持 read() 和/或 write() I/O 方法。)
    * - ``V4L2_CAP_EDID``
      - 0x02000000
      - The device stores the EDID for a video input, or retrieves the EDID for a video
        output. It is a standalone EDID device, so no video streaming etc. will take place.
	(设备为视频输入存储 EDID，或为视频输出获取 EDID。它是一个独立的
	EDID 设备，因此不会进行视频流传输等操作。)

        For a video input this is typically an eeprom that supports the
        :ref:`VESA Enhanced Display Data Channel Standard <vesaeddc>`. It can be something
        else as well, for example a micro controller.
	(对于视频输入，这通常是一个支持 VESA 增强显示数据通道标准的
	eeprom；也可能是别的东西，例如一个微控制器。)

        For a video output this is typically read from an external device such as an
        HDMI splitter accessed by a serial port.
	(对于视频输出，这通常从外部设备读取，例如通过串口访问的 HDMI
	分配器。)
    * - ``V4L2_CAP_STREAMING``
      - 0x04000000
      - The device supports the :ref:`streaming <mmap>` I/O method.
	(设备支持 streaming (流式) I/O 方法。)
    * - ``V4L2_CAP_META_OUTPUT``
      - 0x08000000
      - The device supports the :ref:`metadata` output interface.
	(设备支持 metadata (元数据) 输出接口。)
    * - ``V4L2_CAP_TOUCH``
      - 0x10000000
      - This is a touch device.
	(这是一个触摸设备。)
    * - ``V4L2_CAP_IO_MC``
      - 0x20000000
      - There is only one input and/or output seen from userspace. The whole
        video topology configuration, including which I/O entity is routed to
        the input/output, is configured by userspace via the Media Controller.
        See :ref:`media_controller`.
	(从用户空间的角度看只有一个输入和/或输出。整个视频拓扑配置 (包括
	哪个 I/O 实体被路由到输入/输出) 由用户空间通过 Media Controller
	配置。参见 media_controller 一节。)
    * - ``V4L2_CAP_DEVICE_CAPS``
      - 0x80000000
      - The driver fills the ``device_caps`` field. This capability can
	only appear in the ``capabilities`` field and never in the
	``device_caps`` field.
	(驱动会填充 ``device_caps`` 字段。此能力只能出现在
	``capabilities`` 字段中，绝不会出现在 ``device_caps`` 字段中。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并相应地设置 ``errno`` 变量。通用错误码在
Generic Error Codes 一章中描述。)

.. [#f1]
   The struct :c:type:`v4l2_framebuffer` lacks an
   enum :c:type:`v4l2_buf_type` field, therefore the
   type of overlay is implied by the driver capabilities.
   (struct v4l2_framebuffer 缺少一个 enum v4l2_buf_type 字段，因此叠加
   类型由驱动能力来暗示。)
