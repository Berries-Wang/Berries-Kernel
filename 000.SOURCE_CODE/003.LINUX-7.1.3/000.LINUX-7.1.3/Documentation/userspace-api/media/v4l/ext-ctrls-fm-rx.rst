.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _fm-rx-controls:

************************************************
FM Receiver Control Reference(FM 接收器控件参考)
************************************************

The FM Receiver (FM_RX) class includes controls for common features of
FM Reception capable devices.
(FM 接收器 (FM_RX) 类包含收音接收设备常见功能的控件。)


.. _fm-rx-control-id:

FM_RX Control IDs(FM_RX 控件 ID)
================================

``V4L2_CID_FM_RX_CLASS (class)``
    The FM_RX class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (FM_RX 类描述符。对控件调用 :ref:`VIDIOC_QUERYCTRL` 将返回该控件
    类的描述。)

``V4L2_CID_RDS_RECEPTION (boolean)``
    Enables/disables RDS reception by the radio tuner
    (启用/禁用收音调谐器的 RDS 接收。)

``V4L2_CID_RDS_RX_PTY (integer)``
    Gets RDS Programme Type field. This encodes up to 31 pre-defined
    programme types.
    (获取 RDS 节目类型 (Programme Type) 字段。它编码最多 31 个预定义的
    节目类型。)

``V4L2_CID_RDS_RX_PS_NAME (string)``
    Gets the Programme Service name (PS_NAME). It is intended for
    static display on a receiver. It is the primary aid to listeners in
    programme service identification and selection. In Annex E of
    :ref:`iec62106`, the RDS specification, there is a full
    description of the correct character encoding for Programme Service
    name strings. Also from RDS specification, PS is usually a single
    eight character text. However, it is also possible to find receivers
    which can scroll strings sized as 8 x N characters. So, this control
    must be configured with steps of 8 characters. The result is it must
    always contain a string with size multiple of 8.
    (获取节目服务名 (PS_NAME)。它用于接收机上的静态显示。它是听众识别
    和选择节目服务的主要辅助手段。在 :ref:`iec62106` (RDS 规范) 的附录
    E 中，对节目服务名字符串的正确字符编码有完整描述。同样来自 RDS 规
    范：PS 通常是一个 8 字符的文本。但是也可能见到能够滚动显示 8 x N
    字符大小字符串的接收机。因此，该控件必须以 8 字符为步长进行配置。
    结果是它必须始终包含大小为 8 的倍数的字符串。)

``V4L2_CID_RDS_RX_RADIO_TEXT (string)``
    Gets the Radio Text info. It is a textual description of what is
    being broadcasted. RDS Radio Text can be applied when broadcaster
    wishes to transmit longer PS names, programme-related information or
    any other text. In these cases, RadioText can be used in addition to
    ``V4L2_CID_RDS_RX_PS_NAME``. The encoding for Radio Text strings is
    also fully described in Annex E of :ref:`iec62106`. The length of
    Radio Text strings depends on which RDS Block is being used to
    transmit it, either 32 (2A block) or 64 (2B block). However, it is
    also possible to find receivers which can scroll strings sized as 32
    x N or 64 x N characters. So, this control must be configured with
    steps of 32 or 64 characters. The result is it must always contain a
    string with size multiple of 32 or 64.
    (获取电台文本 (Radio Text) 信息。它是对正在广播内容的文字描述。当
    广播者希望传送更长的 PS 名、节目相关信息或任何其他文本时，可以应
    用 RDS Radio Text。在这些情况下，RadioText 可以在
    ``V4L2_CID_RDS_RX_PS_NAME`` 之外额外使用。Radio Text 字符串的编码
    同样在 :ref:`iec62106` 的附录 E 中完整描述。Radio Text 字符串的长
    度取决于使用哪个 RDS 块来传送它：要么 32 (2A 块)，要么 64 (2B 块)。
    但是也可能见到能够滚动显示 32 x N 或 64 x N 字符大小字符串的接收
    机。因此，该控件必须以 32 或 64 字符为步长进行配置。结果是它必须
    始终包含大小为 32 或 64 的倍数的字符串。)

``V4L2_CID_RDS_RX_TRAFFIC_ANNOUNCEMENT (boolean)``
    If set, then a traffic announcement is in progress.
    (若置位，则正在播报交通通告。)

``V4L2_CID_RDS_RX_TRAFFIC_PROGRAM (boolean)``
    If set, then the tuned programme carries traffic announcements.
    (若置位，则当前调谐到的节目携带交通通告。)

``V4L2_CID_RDS_RX_MUSIC_SPEECH (boolean)``
    If set, then this channel broadcasts music. If cleared, then it
    broadcasts speech. If the transmitter doesn't make this distinction,
    then it will be set.
    (若置位，则该频道广播音乐；若清零，则广播语音。如果发送端不做此区
    分，则该位将被置位。)

``V4L2_CID_TUNE_DEEMPHASIS (enum)``
    Configures the de-emphasis value for reception. A de-emphasis filter
    is applied to the broadcast to accentuate the high audio
    frequencies. Depending on the region, a time constant of either 50
    or 75 microseconds is used. The enum v4l2_deemphasis defines possible
    values for de-emphasis. They are:
    (配置接收用的去加重 (de-emphasis) 值。广播会应用去加重滤波器以强调
    高音频频率。根据地区不同，使用 50 或 75 微秒的时间常数。enum
    v4l2_deemphasis 定义了去加重的可能取值。它们是：)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_DEEMPHASIS_DISABLED``
      - No de-emphasis is applied.
        (不应用去加重。)
    * - ``V4L2_DEEMPHASIS_50_uS``
      - A de-emphasis of 50 uS is used.
        (使用 50 uS 的去加重。)
    * - ``V4L2_DEEMPHASIS_75_uS``
      - A de-emphasis of 75 uS is used.
        (使用 75 uS 的去加重。)
