.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _fm-tx-controls:

*************************************************
FM Transmitter Control Reference(FM 发送控制参考)
*************************************************

The FM Transmitter (FM_TX) class includes controls for common features
of FM transmissions capable devices. Currently this class includes
parameters for audio compression, pilot tone generation, audio deviation
limiter, RDS transmission and tuning power features.
(FM 发送器 (FM_TX) 类包含具备 FM 发送能力设备的常见功能控制。目前该类包括音频压缩、导频音生成、音频频偏限幅器、RDS 发送以及调谐功率特性等参数。)


.. _fm-tx-control-id:

FM_TX Control IDs(FM_TX 控制 ID)
================================

``V4L2_CID_FM_TX_CLASS (class)``
    The FM_TX class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (FM_TX 类描述符。对此控制调用 :ref:`VIDIOC_QUERYCTRL` 将返回该控制类的描述。)

``V4L2_CID_RDS_TX_DEVIATION (integer)``
    Configures RDS signal frequency deviation level in Hz. The range and
    step are driver-specific.
    (以 Hz 为单位配置 RDS 信号的频偏电平。范围和步长由驱动决定。)

``V4L2_CID_RDS_TX_PI (integer)``
    Sets the RDS Programme Identification field for transmission.
    (设置要发送的 RDS 节目标识 (Programme Identification) 字段。)

``V4L2_CID_RDS_TX_PTY (integer)``
    Sets the RDS Programme Type field for transmission. This encodes up
    to 31 pre-defined programme types.
    (设置要发送的 RDS 节目类型 (Programme Type) 字段。它最多编码 31 种预定义的节目类型。)

``V4L2_CID_RDS_TX_PS_NAME (string)``
    Sets the Programme Service name (PS_NAME) for transmission. It is
    intended for static display on a receiver. It is the primary aid to
    listeners in programme service identification and selection. In
    Annex E of :ref:`iec62106`, the RDS specification, there is a full
    description of the correct character encoding for Programme Service
    name strings. Also from RDS specification, PS is usually a single
    eight character text. However, it is also possible to find receivers
    which can scroll strings sized as 8 x N characters. So, this control
    must be configured with steps of 8 characters. The result is it must
    always contain a string with size multiple of 8.
    (设置要发送的节目服务名 (Programme Service name，PS_NAME)。它用于在接收端静态显示，是听众识别和选择节目服务的主要辅助手段。在 RDS 规范 :ref:`iec62106` 的附录 E 中，对节目服务名字符串的正确字符编码有完整描述。同样来自 RDS 规范：PS 通常是一个 8 字符的单一文本。不过，也可能遇到可以滚动显示 8 x N 字符长度字符串的接收器。因此，此控制必须以 8 字符为步长进行配置。结果就是它必须始终包含大小为 8 的倍数的字符串。)

``V4L2_CID_RDS_TX_RADIO_TEXT (string)``
    Sets the Radio Text info for transmission. It is a textual
    description of what is being broadcasted. RDS Radio Text can be
    applied when broadcaster wishes to transmit longer PS names,
    programme-related information or any other text. In these cases,
    RadioText should be used in addition to ``V4L2_CID_RDS_TX_PS_NAME``.
    The encoding for Radio Text strings is also fully described in Annex
    E of :ref:`iec62106`. The length of Radio Text strings depends on
    which RDS Block is being used to transmit it, either 32 (2A block)
    or 64 (2B block). However, it is also possible to find receivers
    which can scroll strings sized as 32 x N or 64 x N characters. So,
    this control must be configured with steps of 32 or 64 characters.
    The result is it must always contain a string with size multiple of
    32 or 64.
    (设置要发送的电台文本 (Radio Text) 信息。它是对正在广播内容的文字描述。当广播者希望发送更长的 PS 名称、与节目相关的信息或任何其他文本时，可以使用 RDS Radio Text。在这些情况下，RadioText 应作为 ``V4L2_CID_RDS_TX_PS_NAME`` 的补充使用。Radio Text 字符串的编码同样在 :ref:`iec62106` 的附录 E 中有完整描述。Radio Text 字符串的长度取决于使用哪个 RDS 块来发送它：要么 32 (2A 块)，要么 64 (2B 块)。不过，也可能遇到可以滚动显示 32 x N 或 64 x N 字符长度字符串的接收器。因此，此控制必须以 32 或 64 字符为步长进行配置。结果就是它必须始终包含大小为 32 或 64 的倍数的字符串。)

``V4L2_CID_RDS_TX_MONO_STEREO (boolean)``
    Sets the Mono/Stereo bit of the Decoder Identification code. If set,
    then the audio was recorded as stereo.
    (设置解码器识别码的 Mono/Stereo (单声道/立体声) 位。如果设置，则音频是按立体声录制的。)

``V4L2_CID_RDS_TX_ARTIFICIAL_HEAD (boolean)``
    Sets the
    `Artificial Head <http://en.wikipedia.org/wiki/Artificial_head>`__
    bit of the Decoder Identification code. If set, then the audio was
    recorded using an artificial head.
    (设置解码器识别码的 `Artificial Head <http://en.wikipedia.org/wiki/Artificial_head>`__ (人造头) 位。如果设置，则音频是使用人造头录制的。)

``V4L2_CID_RDS_TX_COMPRESSED (boolean)``
    Sets the Compressed bit of the Decoder Identification code. If set,
    then the audio is compressed.
    (设置解码器识别码的 Compressed (压缩) 位。如果设置，则音频是压缩的。)

``V4L2_CID_RDS_TX_DYNAMIC_PTY (boolean)``
    Sets the Dynamic PTY bit of the Decoder Identification code. If set,
    then the PTY code is dynamically switched.
    (设置解码器识别码的 Dynamic PTY (动态 PTY) 位。如果设置，则 PTY 码会动态切换。)

``V4L2_CID_RDS_TX_TRAFFIC_ANNOUNCEMENT (boolean)``
    If set, then a traffic announcement is in progress.
    (如果设置，则正在播报交通公告。)

``V4L2_CID_RDS_TX_TRAFFIC_PROGRAM (boolean)``
    If set, then the tuned programme carries traffic announcements.
    (如果设置，则当前调谐到的节目携带交通公告。)

``V4L2_CID_RDS_TX_MUSIC_SPEECH (boolean)``
    If set, then this channel broadcasts music. If cleared, then it
    broadcasts speech. If the transmitter doesn't make this distinction,
    then it should be set.
    (如果设置，则此频道广播音乐。如果清除，则广播语音。如果发送器不做这种区分，则应设置此位。)

``V4L2_CID_RDS_TX_ALT_FREQS_ENABLE (boolean)``
    If set, then transmit alternate frequencies.
    (如果设置，则发送备用频率。)

``V4L2_CID_RDS_TX_ALT_FREQS (__u32 array)``
    The alternate frequencies in kHz units. The RDS standard allows for
    up to 25 frequencies to be defined. Drivers may support fewer
    frequencies so check the array size.
    (以 kHz 为单位的备用频率。RDS 标准允许最多定义 25 个频率。驱动可能只支持较少的频率，因此请检查数组大小。)

``V4L2_CID_AUDIO_LIMITER_ENABLED (boolean)``
    Enables or disables the audio deviation limiter feature. The limiter
    is useful when trying to maximize the audio volume, minimize
    receiver-generated distortion and prevent overmodulation.
    (启用或禁用音频频偏限幅器 (limiter) 功能。限幅器在尝试最大化音量、最小化接收器产生的失真并防止过调制时很有用。)

``V4L2_CID_AUDIO_LIMITER_RELEASE_TIME (integer)``
    Sets the audio deviation limiter feature release time. Unit is in
    microseconds. Step and range are driver-specific.
    (设置音频频偏限幅器功能的释放时间 (release time)。单位为微秒。步长和范围由驱动决定。)

``V4L2_CID_AUDIO_LIMITER_DEVIATION (integer)``
    Configures audio frequency deviation level in Hz. The range and step
    are driver-specific.
    (以 Hz 为单位配置音频频偏电平。范围和步长由驱动决定。)

``V4L2_CID_AUDIO_COMPRESSION_ENABLED (boolean)``
    Enables or disables the audio compression feature. This feature
    amplifies signals below the threshold by a fixed gain and compresses
    audio signals above the threshold by the ratio of Threshold/(Gain +
    Threshold).
    (启用或禁用音频压缩功能。此功能将低于阈值的信号放大固定增益，并将高于阈值的音频信号按 Threshold/(Gain + Threshold) 的比率进行压缩。)

``V4L2_CID_AUDIO_COMPRESSION_GAIN (integer)``
    Sets the gain for audio compression feature. It is a dB value. The
    range and step are driver-specific.
    (设置音频压缩功能的增益。它是一个 dB 值。范围和步长由驱动决定。)

``V4L2_CID_AUDIO_COMPRESSION_THRESHOLD (integer)``
    Sets the threshold level for audio compression feature. It is a dB
    value. The range and step are driver-specific.
    (设置音频压缩功能的阈值电平。它是一个 dB 值。范围和步长由驱动决定。)

``V4L2_CID_AUDIO_COMPRESSION_ATTACK_TIME (integer)``
    Sets the attack time for audio compression feature. It is a microseconds
    value. The range and step are driver-specific.
    (设置音频压缩功能的起控时间 (attack time)。它是一个微秒值。范围和步长由驱动决定。)

``V4L2_CID_AUDIO_COMPRESSION_RELEASE_TIME (integer)``
    Sets the release time for audio compression feature. It is a
    microseconds value. The range and step are driver-specific.
    (设置音频压缩功能的释放时间 (release time)。它是一个微秒值。范围和步长由驱动决定。)

``V4L2_CID_PILOT_TONE_ENABLED (boolean)``
    Enables or disables the pilot tone generation feature.
    (启用或禁用导频音 (pilot tone) 生成功能。)

``V4L2_CID_PILOT_TONE_DEVIATION (integer)``
    Configures pilot tone frequency deviation level. Unit is in Hz. The
    range and step are driver-specific.
    (配置导频音的频偏电平。单位为 Hz。范围和步长由驱动决定。)

``V4L2_CID_PILOT_TONE_FREQUENCY (integer)``
    Configures pilot tone frequency value. Unit is in Hz. The range and
    step are driver-specific.
    (配置导频音频率值。单位为 Hz。范围和步长由驱动决定。)

``V4L2_CID_TUNE_PREEMPHASIS (enum)``
    Configures the pre-emphasis value for broadcasting. A pre-emphasis
    filter is applied to the broadcast to accentuate the high audio
    frequencies. Depending on the region, a time constant of either 50
    or 75 microseconds is used. The enum v4l2_preemphasis defines possible
    values for pre-emphasis. They are:
    (配置广播的预加重 (pre-emphasis) 值。预加重滤波器应用于广播以强调高频音频成分。根据地区的不同，使用 50 或 75 微秒的时间常数。枚举 v4l2_preemphasis 定义了预加重的可能取值。它们是：)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_PREEMPHASIS_DISABLED``
      - No pre-emphasis is applied.(不应用预加重。)
    * - ``V4L2_PREEMPHASIS_50_uS``
      - A pre-emphasis of 50 uS is used.(使用 50 uS 的预加重。)
    * - ``V4L2_PREEMPHASIS_75_uS``
      - A pre-emphasis of 75 uS is used.(使用 75 uS 的预加重。)

``V4L2_CID_TUNE_POWER_LEVEL (integer)``
    Sets the output power level for signal transmission. Unit is in
    dBuV. Range and step are driver-specific.
    (设置信号发送的输出功率电平。单位为 dBuV。范围和步长由驱动决定。)

``V4L2_CID_TUNE_ANTENNA_CAPACITOR (integer)``
    This selects the value of antenna tuning capacitor manually or
    automatically if set to zero. Unit, range and step are
    driver-specific.
    (手动选择天线调谐电容的值，设置为 0 时则自动选择。单位、范围和步长由驱动决定。)

For more details about RDS specification, refer to :ref:`iec62106`
document, from CENELEC.
(有关 RDS 规范的更多细节，请参阅 CENELEC 的 :ref:`iec62106` 文档。)
