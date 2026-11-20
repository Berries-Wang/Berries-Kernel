.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-meta-fmt-d4xx:

*******************************
V4L2_META_FMT_D4XX ('D4XX')
*******************************

Intel D4xx UVC Cameras Metadata
(Intel D4xx UVC 相机元数据)


Description(描述)
=================

Intel D4xx (D435, D455 and others) cameras include per-frame metadata in their UVC
payload headers, following the Microsoft(R) UVC extension proposal [1_]. That
means, that the private D4XX metadata, following the standard UVC header, is
organised in blocks. D4XX cameras implement several standard block types,
proposed by Microsoft, and several proprietary ones. Supported standard metadata
types are MetadataId_CaptureStats (ID 3), MetadataId_CameraExtrinsics (ID 4),
and MetadataId_CameraIntrinsics (ID 5). For their description see [1_]. This
document describes proprietary metadata types, used by D4xx cameras.
(Intel D4xx(D435、D455 等)相机在其 UVC 载荷头中包含每帧元数据，遵循
Microsoft(R) UVC 扩展提案 [1_]。也就是说，紧跟标准 UVC 头之后的 D4XX 私有
元数据以块 (block) 为单位组织。D4XX 相机实现了 Microsoft 提出的若干标准块
类型，以及若干专有块类型。受支持的标准元数据类型为 MetadataId_CaptureStats
(ID 3)、MetadataId_CameraExtrinsics (ID 4) 和 MetadataId_CameraIntrinsics
(ID 5)，其描述见 [1_]。本文档描述 D4xx 相机使用的专有元数据类型。)

V4L2_META_FMT_D4XX buffers follow the metadata buffer layout of
V4L2_META_FMT_UVC with the only difference, that it also includes proprietary
payload header data. D4xx cameras use bulk transfers and only send one payload
per frame, therefore their headers cannot be larger than 255 bytes.
(V4L2_META_FMT_D4XX 缓冲区遵循 V4L2_META_FMT_UVC 的元数据缓冲区布局，唯一
的区别是它还包含专有的载荷头数据。D4xx 相机使用 bulk 传输，每帧只发送一个
载荷，因此其头不会大于 255 字节。)

This document implements Intel Configuration version 3 [9_].
(本文档实现 Intel Configuration 版本 3 [9_]。)

Below are proprietary Microsoft style metadata types, used by D4xx cameras,
where all fields are in little endian order:
(下面是 D4xx 相机使用的 Microsoft 风格专有元数据类型，所有字段均为小端序：)

.. tabularcolumns:: |p{5.0cm}|p{12.5cm}|


.. flat-table:: D4xx metadata
    :widths: 1 2
    :header-rows:  1
    :stub-columns: 0

    * - **Field** (字段)
      - **Description** (描述)
    * - :cspan:`1` *Depth Control* (深度控制)
    * - __u32 ID
      - 0x80000000
    * - __u32 Size
      - Size in bytes, include ID (all protocol versions: 60)
        (以字节计的大小，包含 ID(所有协议版本均为 60))
    * - __u32 Version
      - Version of this structure. The documentation herein covers versions 1,
        2 and 3. The version number will be incremented when new fields are
        added.
        (该结构的版本。本文覆盖版本 1、2 和 3。新增字段时版本号会递增。)
    * - __u32 Flags
      - A bitmask of flags: see [2_] below
        (标志位掩码：见下文 [2_])
    * - __u32 Gain
      - Gain value in internal units, same as the V4L2_CID_GAIN control, used to
	capture the frame
	(增益值，内部单位，与 V4L2_CID_GAIN 控件相同，用于捕获该帧)
    * - __u32 Exposure
      - Exposure time (in microseconds) used to capture the frame
        (捕获该帧所用的曝光时间(微秒))
    * - __u32 Laser power
      - Power of the laser LED 0-360, used for depth measurement
        (激光 LED 的功率 0-360，用于深度测量)
    * - __u32 AE mode
      - 0: manual; 1: automatic exposure
        (0：手动；1：自动曝光)
    * - __u32 Exposure priority
      - Exposure priority value: 0 - constant frame rate
        (曝光优先级值：0 - 恒定帧率)
    * - __u32 AE ROI left
      - Left border of the AE Region of Interest (all ROI values are in pixels
	and lie between 0 and maximum width or height respectively)
	(AE 感兴趣区域 (ROI) 的左边界(所有 ROI 值以像素为单位，且分别介于 0 与
	最大宽度或高度之间))
    * - __u32 AE ROI right
      - Right border of the AE Region of Interest
        (AE 感兴趣区域的右边界)
    * - __u32 AE ROI top
      - Top border of the AE Region of Interest
        (AE 感兴趣区域的上边界)
    * - __u32 AE ROI bottom
      - Bottom border of the AE Region of Interest
        (AE 感兴趣区域的下边界)
    * - __u32 Preset
      - Preset selector value, default: 0, unless changed by the user
        (预设选择器值，默认为 0，除非用户更改)
    * - __u8 Emitter mode (v3 only) (__u32 Laser mode for v1) [8_]
      - 0: off, 1: on, same as __u32 Laser mode for v1
        (0：关；1：开，在 v1 中与 __u32 Laser mode 相同)
    * - __u8 RFU byte (v3 only)
      - Spare byte for future use
        (留作未来使用的备用字节)
    * - __u16 LED Power (v3 only)
      - Led power value 0-360 (F416 SKU)
        (LED 功率值 0-360(F416 SKU))
    * - :cspan:`1` *Capture Timing* (捕获时序)
    * - __u32 ID
      - 0x80000001
    * - __u32 Size
      - Size in bytes, include ID (all protocol versions: 40)
        (以字节计的大小，包含 ID(所有协议版本均为 40))
    * - __u32 Version
      - Version of this structure. The documentation herein corresponds to
        version xxx. The version number will be incremented when new fields are
        added.
        (该结构的版本。本文对应版本 xxx。新增字段时版本号会递增。)
    * - __u32 Flags
      - A bitmask of flags: see [3_] below
        (标志位掩码：见下文 [3_])
    * - __u32 Frame counter
      - Monotonically increasing counter
        (单调递增的计数器)
    * - __u32 Optical time
      - Time in microseconds from the beginning of a frame till its middle
        (从帧开始到帧中点的时间，以微秒计)
    * - __u32 Readout time
      - Time, used to read out a frame in microseconds
        (读出一帧所用的时间，以微秒计)
    * - __u32 Exposure time
      - Frame exposure time in microseconds
        (帧曝光时间，以微秒计)
    * - __u32 Frame interval
      - In microseconds = 1000000 / framerate
        (以微秒计 = 1000000 / 帧率)
    * - __u32 Pipe latency
      - Time in microseconds from start of frame to data in USB buffer
        (从帧开始到数据进入 USB 缓冲区的时间，以微秒计)
    * - :cspan:`1` *Configuration* (配置)
    * - __u32 ID
      - 0x80000002
    * - __u32 Size
      - Size in bytes, include ID (v1:36, v3:40)
        (以字节计的大小，包含 ID(v1:36，v3:40))
    * - __u32 Version
      - Version of this structure. The documentation herein corresponds to
        version xxx. The version number will be incremented when new fields are
        added.
        (该结构的版本。本文对应版本 xxx。新增字段时版本号会递增。)
    * - __u32 Flags
      - A bitmask of flags: see [4_] below
        (标志位掩码：见下文 [4_])
    * - __u8 Hardware type
      - Camera hardware version [5_]
        (相机硬件版本 [5_])
    * - __u8 SKU ID
      - Camera hardware configuration [6_]
        (相机硬件配置 [6_])
    * - __u32 Cookie
      - Internal synchronisation
        (内部同步)
    * - __u16 Format
      - Image format code [7_]
        (图像格式码 [7_])
    * - __u16 Width
      - Width in pixels
        (宽度，以像素计)
    * - __u16 Height
      - Height in pixels
        (高度，以像素计)
    * - __u16 Framerate
      - Requested frame rate per second
        (请求的每秒帧率)
    * - __u16 Trigger
      - Byte 0: bit 0: depth and RGB are synchronised, bit 1: external trigger
        (字节 0：位 0：深度与 RGB 已同步；位 1：外部触发)
    * - __u16 Calibration count (v3 only)
      - Calibration counter, see [4_] below
        (标定计数器，见下文 [4_])
    * - __u8 GPIO input data (v3 only)
      - GPIO readout, see [4_] below (Supported from FW 5.12.7.0)
        (GPIO 读数，见下文 [4_](自 FW 5.12.7.0 起支持))
    * - __u32 Sub-preset info (v3 only)
      - Sub-preset choice information, see [4_] below
        (子预设选择信息，见下文 [4_])
    * - __u8 reserved (v3 only)
      - RFU byte.
        (RFU 字节。)

.. _1:

[1] https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/uvc-extensions-1-5

.. _2:

[2] Depth Control flags specify which fields are valid: ::

  0x00000001 Gain
  0x00000002 Exposure
  0x00000004 Laser power
  0x00000008 AE mode
  0x00000010 Exposure priority
  0x00000020 AE ROI
  0x00000040 Preset
  0x00000080 Emitter mode
  0x00000100 LED Power

([2] Depth Control(深度控制)标志指明哪些字段有效：)

.. _3:

[3] Capture Timing flags specify which fields are valid: ::

  0x00000001 Frame counter
  0x00000002 Optical time
  0x00000004 Readout time
  0x00000008 Exposure time
  0x00000010 Frame interval
  0x00000020 Pipe latency

([3] Capture Timing(捕获时序)标志指明哪些字段有效：)

.. _4:

[4] Configuration flags specify which fields are valid: ::

  0x00000001 Hardware type
  0x00000002 SKU ID
  0x00000004 Cookie
  0x00000008 Format
  0x00000010 Width
  0x00000020 Height
  0x00000040 Framerate
  0x00000080 Trigger
  0x00000100 Cal count
  0x00000200 GPIO Input Data
  0x00000400 Sub-preset Info

([4] Configuration(配置)标志指明哪些字段有效：)

.. _5:

[5] Camera model: ::

  0 DS5
  1 IVCAM2

([5] 相机型号：)

.. _6:

[6] 8-bit camera hardware configuration bitfield: ::

  [1:0] depthCamera
	00: no depth
	01: standard depth
	10: wide depth
	11: reserved
  [2]   depthIsActive - has a laser projector
  [3]   RGB presence
  [4]   Inertial Measurement Unit (IMU) presence
  [5]   projectorType
	0: HPTG
	1: Princeton
  [6]   0: a projector, 1: an LED
  [7]   reserved

([6] 8 位相机硬件配置位域：)

.. _7:

[7] Image format codes per video streaming interface:
(各视频流接口的图像格式码：)

Depth: ::

  1 Z16
  2 Z

(深度 (Depth)：)

Left sensor: ::

  1 Y8
  2 UYVY
  3 R8L8
  4 Calibration
  5 W10

(左传感器 (Left sensor)：)

Fish Eye sensor: ::

  1 RAW8

(鱼眼传感器 (Fish Eye sensor)：)

.. _8:

[8] The "Laser mode" has been replaced in version 3 by three different fields.
"Laser" has been renamed to "Emitter" as there are multiple technologies for
camera projectors. As we have another field for "Laser Power" we introduced
"LED Power" for extra emitter.
([8] "Laser mode" 在版本 3 中被三个不同的字段取代。"Laser" 被改名为
"Emitter"，因为相机投影器存在多种技术。由于我们已有 "Laser Power" 字段，为
额外的发射器引入了 "LED Power"。)

The "Laser mode" __u32 fields has been split into: ::
   1 __u8 Emitter mode
   2 __u8 RFU byte
   3 __u16 LED Power

("Laser mode" 的 __u32 字段被拆分为：)

This is a change between versions 1 and 3. All versions 1, 2 and 3 are backward
compatible with the same data format and they are supported. See [2_] for which
attributes are valid.
(这是版本 1 与版本 3 之间的变化。版本 1、2 和 3 都向后兼容相同的数据格式，并
都受支持。哪些属性有效见 [2_]。)

.. _9:

[9] LibRealSense SDK metadata source:
https://github.com/IntelRealSense/librealsense/blob/master/src/metadata.h
(LibRealSense SDK 元数据来源：)
