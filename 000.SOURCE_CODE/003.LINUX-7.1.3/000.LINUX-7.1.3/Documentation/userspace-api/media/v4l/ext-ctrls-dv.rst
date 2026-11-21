.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _dv-controls:

*************************************************
Digital Video Control Reference(数字视频控制参考)
*************************************************

The Digital Video control class is intended to control receivers and
transmitters for `VGA <http://en.wikipedia.org/wiki/Vga>`__,
`DVI <http://en.wikipedia.org/wiki/Digital_Visual_Interface>`__
(Digital Visual Interface), HDMI (:ref:`hdmi`) and DisplayPort
(:ref:`dp`). These controls are generally expected to be private to
the receiver or transmitter subdevice that implements them, so they are
only exposed on the ``/dev/v4l-subdev*`` device node.
(数字视频控制类用于控制 `VGA <http://en.wikipedia.org/wiki/Vga>`__、`DVI <http://en.wikipedia.org/wiki/Digital_Visual_Interface>`__（Digital Visual Interface，数字视频接口）、HDMI (:ref:`hdmi`) 和 DisplayPort (:ref:`dp`) 的接收器与发送器。这些控制项通常被认为是实现它们的接收器或发送器子设备所私有的，因此只在 ``/dev/v4l-subdev*`` 设备节点上暴露。)

.. note::

   Note that these devices can have multiple input or output pads which are
   hooked up to e.g. HDMI connectors. Even though the subdevice will
   receive or transmit video from/to only one of those pads, the other pads
   can still be active when it comes to EDID (Extended Display
   Identification Data, :ref:`vesaedid`) and HDCP (High-bandwidth Digital
   Content Protection System, :ref:`hdcp`) processing, allowing the
   device to do the fairly slow EDID/HDCP handling in advance. This allows
   for quick switching between connectors.
   (注意，这些设备可能有多个输入或输出 pad，它们连接到例如 HDMI 连接器。即使子设备只从其中一个 pad 接收视频或向其发送视频，在进行 EDID（Extended Display
   Identification Data，扩展显示识别数据，:ref:`vesaedid`）和 HDCP（High-bandwidth Digital Content Protection System，高带宽数字内容保护系统，:ref:`hdcp`）处理时，其他 pad 仍然可以处于活动状态，从而允许设备提前完成相当缓慢的 EDID/HDCP 处理。这使得连接器之间的快速切换成为可能。)

These pads appear in several of the controls in this section as
bitmasks, one bit for each pad. Bit 0 corresponds to pad 0, bit 1 to pad
1, etc. The maximum value of the control is the set of valid pads.
(这些 pad 以位掩码的形式出现在本节的多个控制项中，每个 pad 对应一位。位 0 对应 pad 0，位 1 对应 pad 1，依此类推。控制项的最大值即有效 pad 的集合。)


.. _dv-control-id:

Digital Video Control IDs(数字视频控制 ID)
==========================================

``V4L2_CID_DV_CLASS (class)``
    The Digital Video class descriptor.
    (数字视频类描述符。)

``V4L2_CID_DV_TX_HOTPLUG (bitmask)``
    Many connectors have a hotplug pin which is high if EDID information
    is available from the source. This control shows the state of the
    hotplug pin as seen by the transmitter. Each bit corresponds to an
    output pad on the transmitter. If an output pad does not have an
    associated hotplug pin, then the bit for that pad will be 0. This
    read-only control is applicable to DVI-D, HDMI and DisplayPort
    connectors.
    (许多连接器都有一个热插拔引脚，当源端可提供 EDID 信息时该引脚为高电平。此控制项显示发送器所看到的热插拔引脚状态。每一位对应发送器上的一个输出
    pad。如果某个输出 pad 没有关联的热插拔引脚，则该 pad 对应的位为 0。此只读控制项适用于 DVI-D、HDMI 和 DisplayPort 连接器。)

``V4L2_CID_DV_TX_RXSENSE (bitmask)``
    Rx Sense is the detection of pull-ups on the TMDS clock lines. This
    normally means that the sink has left/entered standby (i.e. the
    transmitter can sense that the receiver is ready to receive video).
    Each bit corresponds to an output pad on the transmitter. If an
    output pad does not have an associated Rx Sense, then the bit for
    that pad will be 0. This read-only control is applicable to DVI-D
    and HDMI devices.
    (Rx Sense（接收端检测）是对 TMDS 时钟线上拉电阻的检测。这通常意味着接收端已离开/进入待机状态（即发送器能够感知接收器已准备好接收视频）。每一位对应发送器上的一个输出
    pad。如果某个输出 pad 没有关联的 Rx Sense，则该 pad 对应的位为 0。此只读控制项适用于 DVI-D 和 HDMI 设备。)

``V4L2_CID_DV_TX_EDID_PRESENT (bitmask)``
    When the transmitter sees the hotplug signal from the receiver it
    will attempt to read the EDID. If set, then the transmitter has read
    at least the first block (= 128 bytes). Each bit corresponds to an
    output pad on the transmitter. If an output pad does not support
    EDIDs, then the bit for that pad will be 0. This read-only control
    is applicable to VGA, DVI-A/D, HDMI and DisplayPort connectors.
    (当发送器看到来自接收器的热插拔信号时，它会尝试读取 EDID。若置位，则发送器至少已读取第一个块（= 128 字节）。每一位对应发送器上的一个输出
    pad。如果某个输出 pad 不支持 EDID，则该 pad 对应的位为 0。此只读控制项适用于 VGA、DVI-A/D、HDMI 和 DisplayPort 连接器。)

``V4L2_CID_DV_TX_MODE``
    (enum)

enum v4l2_dv_tx_mode -
    HDMI transmitters can transmit in DVI-D mode (just video) or in HDMI
    mode (video + audio + auxiliary data). This control selects which
    mode to use: V4L2_DV_TX_MODE_DVI_D or V4L2_DV_TX_MODE_HDMI.
    This control is applicable to HDMI connectors.
    (HDMI 发送器可以以 DVI-D 模式（仅视频）或 HDMI 模式（视频 + 音频 +
    辅助数据）发送。此控制项选择使用哪种模式：V4L2_DV_TX_MODE_DVI_D 或
    V4L2_DV_TX_MODE_HDMI。此控制项适用于 HDMI 连接器。)

``V4L2_CID_DV_TX_RGB_RANGE``
    (enum)

enum v4l2_dv_rgb_range -
    Select the quantization range for RGB output. V4L2_DV_RANGE_AUTO
    follows the RGB quantization range specified in the standard for the
    video interface (ie. :ref:`cea861` for HDMI).
    V4L2_DV_RANGE_LIMITED and V4L2_DV_RANGE_FULL override the
    standard to be compatible with sinks that have not implemented the
    standard correctly (unfortunately quite common for HDMI and DVI-D).
    Full range allows all possible values to be used whereas limited
    range sets the range to (16 << (N-8)) - (235 << (N-8)) where N is
    the number of bits per component. This control is applicable to VGA,
    DVI-A/D, HDMI and DisplayPort connectors.
    (选择 RGB 输出的量化范围。V4L2_DV_RANGE_AUTO 遵循视频接口标准为该接口规定的
    RGB 量化范围（对 HDMI 即 :ref:`cea861`）。V4L2_DV_RANGE_LIMITED 和
    V4L2_DV_RANGE_FULL 会覆盖该标准，以兼容未能正确实现标准的接收端（遗憾的是，
    这在 HDMI 和 DVI-D 中相当常见）。全范围允许使用所有可能的取值，而有限范围将
    范围设为 (16 << (N-8)) - (235 << (N-8))，其中 N 是每个分量的位数。此控制项
    适用于 VGA、DVI-A/D、HDMI 和 DisplayPort 连接器。)

``V4L2_CID_DV_TX_IT_CONTENT_TYPE``
    (enum)

enum v4l2_dv_it_content_type -
    Configures the IT Content Type of the transmitted video. This
    information is sent over HDMI and DisplayPort connectors as part of
    the AVI InfoFrame. The term 'IT Content' is used for content that
    originates from a computer as opposed to content from a TV broadcast
    or an analog source. The enum v4l2_dv_it_content_type defines
    the possible content types:
    (配置所发送视频的 IT 内容类型。该信息作为 AVI InfoFrame 的一部分通过 HDMI
    和 DisplayPort 连接器发送。术语 "IT Content" 指源自计算机的内容，以区别于
    来自电视广播或模拟信号源的内容。enum v4l2_dv_it_content_type 定义了可能的
    内容类型：)

.. tabularcolumns:: |p{7.3cm}|p{10.2cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_DV_IT_CONTENT_TYPE_GRAPHICS``
      - Graphics content. Pixel data should be passed unfiltered and
	without analog reconstruction.
	(图形内容。像素数据应不加过滤地传送，且不进行模拟重构。)
    * - ``V4L2_DV_IT_CONTENT_TYPE_PHOTO``
      - Photo content. The content is derived from digital still pictures.
	The content should be passed through with minimal scaling and
	picture enhancements.
	(照片内容。内容源自数码静态图片。应以最小的缩放和画面增强直接
	传送该内容。)
    * - ``V4L2_DV_IT_CONTENT_TYPE_CINEMA``
      - Cinema content.
	(电影内容。)
    * - ``V4L2_DV_IT_CONTENT_TYPE_GAME``
      - Game content. Audio and video latency should be minimized.
	(游戏内容。应尽量减小音频和视频延迟。)
    * - ``V4L2_DV_IT_CONTENT_TYPE_NO_ITC``
      - No IT Content information is available and the ITC bit in the AVI
	InfoFrame is set to 0.
	(无 IT Content 信息可用，且 AVI InfoFrame 中的 ITC 位被置为 0。)



``V4L2_CID_DV_RX_POWER_PRESENT (bitmask)``
    Detects whether the receiver receives power from the source (e.g.
    HDMI carries 5V on one of the pins). This is often used to power an
    eeprom which contains EDID information, such that the source can
    read the EDID even if the sink is in standby/power off. Each bit
    corresponds to an input pad on the receiver. If an input pad
    cannot detect whether power is present, then the bit for that pad
    will be 0. This read-only control is applicable to DVI-D, HDMI and
    DisplayPort connectors.
    (检测接收器是否从源端获得供电（例如 HDMI 在其中一个引脚上提供 5V 电压）。
    这常用于为含有 EDID 信息的 EEPROM 供电，使得即使接收端处于待机/关机状态，
    源端也能读取 EDID。每一位对应接收器上的一个输入 pad。如果某个输入 pad 无法
    检测电源是否存在，则该 pad 对应的位为 0。此只读控制项适用于 DVI-D、HDMI 和
    DisplayPort 连接器。)

``V4L2_CID_DV_RX_RGB_RANGE``
    (enum)

enum v4l2_dv_rgb_range -
    Select the quantization range for RGB input. V4L2_DV_RANGE_AUTO
    follows the RGB quantization range specified in the standard for the
    video interface (ie. :ref:`cea861` for HDMI).
    V4L2_DV_RANGE_LIMITED and V4L2_DV_RANGE_FULL override the
    standard to be compatible with sources that have not implemented the
    standard correctly (unfortunately quite common for HDMI and DVI-D).
    Full range allows all possible values to be used whereas limited
    range sets the range to (16 << (N-8)) - (235 << (N-8)) where N is
    the number of bits per component. This control is applicable to VGA,
    DVI-A/D, HDMI and DisplayPort connectors.
    (选择 RGB 输入的量化范围。V4L2_DV_RANGE_AUTO 遵循视频接口标准为该接口规定的
    RGB 量化范围（对 HDMI 即 :ref:`cea861`）。V4L2_DV_RANGE_LIMITED 和
    V4L2_DV_RANGE_FULL 会覆盖该标准，以兼容未能正确实现标准的源端（遗憾的是，
    这在 HDMI 和 DVI-D 中相当常见）。全范围允许使用所有可能的取值，而有限范围将
    范围设为 (16 << (N-8)) - (235 << (N-8))，其中 N 是每个分量的位数。此控制项
    适用于 VGA、DVI-A/D、HDMI 和 DisplayPort 连接器。)

``V4L2_CID_DV_RX_IT_CONTENT_TYPE``
    (enum)

enum v4l2_dv_it_content_type -
    Reads the IT Content Type of the received video. This information is
    sent over HDMI and DisplayPort connectors as part of the AVI
    InfoFrame. The term 'IT Content' is used for content that originates
    from a computer as opposed to content from a TV broadcast or an
    analog source. See ``V4L2_CID_DV_TX_IT_CONTENT_TYPE`` for the
    available content types.
    (读取所接收视频的 IT 内容类型。该信息作为 AVI InfoFrame 的一部分通过 HDMI
    和 DisplayPort 连接器发送。术语 "IT Content" 指源自计算机的内容，以区别于
    来自电视广播或模拟信号源的内容。可用内容类型参见
    ``V4L2_CID_DV_TX_IT_CONTENT_TYPE`` 。)
