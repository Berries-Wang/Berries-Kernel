.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _control:

***********************
User Controls(用户控件)
***********************

Devices typically have a number of user-settable controls such as
brightness, saturation and so on, which would be presented to the user
on a graphical user interface. But, different devices will have
different controls available, and furthermore, the range of possible
values, and the default value will vary from device to device. The
control ioctls provide the information and a mechanism to create a nice
user interface for these controls that will work correctly with any
device.
(设备通常拥有若干用户可设置的控件，例如亮度、饱和度等，它们会被呈现到图形用户
界面上。但是，不同设备可用的控件不同，而且可能的取值范围和默认值也因设备而异。
控件 ioctl 提供了为这些控件创建良好用户界面所需的信息和机制，可以在任何设备上
正确工作。)

All controls are accessed using an ID value. V4L2 defines several IDs
for specific purposes. Drivers can also implement their own custom
controls using ``V4L2_CID_PRIVATE_BASE``  [#f1]_ and higher values. The
pre-defined control IDs have the prefix ``V4L2_CID_``, and are listed in
:ref:`control-id`. The ID is used when querying the attributes of a
control, and when getting or setting the current value.
(所有控件都通过一个 ID 值来访问。V4L2 为特定用途定义了若干 ID。驱动程序也可以
使用 ``V4L2_CID_PRIVATE_BASE`` 及更高的值实现自己的自定义控件。预定义的控件
ID 以 ``V4L2_CID_`` 为前缀，列于 :ref:`control-id` 。查询控件属性以及读取或
设置当前值时都要用到该 ID。)

Generally applications should present controls to the user without
assumptions about their purpose. Each control comes with a name string
the user is supposed to understand. When the purpose is non-intuitive
the driver writer should provide a user manual, a user interface plug-in
or a driver specific panel application. Predefined IDs were introduced
to change a few controls programmatically, for example to mute a device
during a channel switch.
(一般来说，应用程序应在不假设控件用途的前提下向用户呈现控件。每个控件都带有
一个期望用户能理解的名字字符串。当用途不够直观时，驱动作者应提供用户手册、
用户界面插件或驱动专用的面板应用。引入预定义 ID 是为了能够以编程方式更改少数
控件，例如在频道切换期间将设备静音。)

Drivers may enumerate different controls after switching the current
video input or output, tuner or modulator, or audio input or output.
Different in the sense of other bounds, another default and current
value, step size or other menu items. A control with a certain *custom*
ID can also change name and type.
(在切换当前视频输入或输出、调谐器或调制器、音频输入或输出之后，驱动程序可能
枚举出不同的控件。"不同"指的是不同的上下界、不同的默认值与当前值、步长或其他
菜单项。具有某个 *custom* ID 的控件也可能更改名称和类型。)

If a control is not applicable to the current configuration of the
device (for example, it doesn't apply to the current video input)
drivers set the ``V4L2_CTRL_FLAG_INACTIVE`` flag.
(如果控件不适用于设备当前的配置 (例如它不适用于当前视频输入)，驱动程序会设置
``V4L2_CTRL_FLAG_INACTIVE`` 标志。)

Control values are stored globally, they do not change when switching
except to stay within the reported bounds. They also do not change e. g.
when the device is opened or closed, when the tuner radio frequency is
changed or generally never without application request.
(控件值是全局存储的；切换时除了保持在报告的边界之内，它们不会改变。例如在
设备打开或关闭时、调谐器射频改变时，控件值也不会改变；总之，没有应用程序的
请求它们绝不会改变。)

V4L2 specifies an event mechanism to notify applications when controls
change value (see
:ref:`VIDIOC_SUBSCRIBE_EVENT`, event
``V4L2_EVENT_CTRL``), panel applications might want to make use of that
in order to always reflect the correct control value.
( V4L2 提供了一种事件机制，在控件值改变时通知应用程序 (参见
:ref:`VIDIOC_SUBSCRIBE_EVENT` ，事件 ``V4L2_EVENT_CTRL`` )，面板应用可能希望
利用该机制，以便始终反映正确的控件值。)

All controls use machine endianness.
(所有控件都使用机器字节序。)


.. _control-id:

Control IDs(控件 ID)
====================

``V4L2_CID_BASE``
    First predefined ID, equal to ``V4L2_CID_BRIGHTNESS``.
    (第一个预定义 ID，等于 ``V4L2_CID_BRIGHTNESS`` 。)

``V4L2_CID_USER_BASE``
    Synonym of ``V4L2_CID_BASE``.
    ( ``V4L2_CID_BASE`` 的同义词。)

``V4L2_CID_BRIGHTNESS`` ``(integer)``
    Picture brightness, or more precisely, the black level.
    (图像亮度，更准确地说，是黑色电平。)

``V4L2_CID_CONTRAST`` ``(integer)``
    Picture contrast or luma gain.
    (图像对比度或亮度增益。)

``V4L2_CID_SATURATION`` ``(integer)``
    Picture color saturation or chroma gain.
    (图像色彩饱和度或色度增益。)

``V4L2_CID_HUE`` ``(integer)``
    Hue or color balance.
    (色调或彩色平衡。)

``V4L2_CID_AUDIO_VOLUME`` ``(integer)``
    Overall audio volume. Note some drivers also provide an OSS or ALSA
    mixer interface.
    (整体音量。注意某些驱动程序还提供 OSS 或 ALSA 混音器接口。)

``V4L2_CID_AUDIO_BALANCE`` ``(integer)``
    Audio stereo balance. Minimum corresponds to all the way left,
    maximum to right.
    (音频立体声平衡。最小值对应完全偏左，最大值对应完全偏右。)

``V4L2_CID_AUDIO_BASS`` ``(integer)``
    Audio bass adjustment.
    (音频低音调节。)

``V4L2_CID_AUDIO_TREBLE`` ``(integer)``
    Audio treble adjustment.
    (音频高音调节。)

``V4L2_CID_AUDIO_MUTE`` ``(boolean)``
    Mute audio, i. e. set the volume to zero, however without affecting
    ``V4L2_CID_AUDIO_VOLUME``. Like ALSA drivers, V4L2 drivers must mute
    at load time to avoid excessive noise. Actually the entire device
    should be reset to a low power consumption state.
    (静音音频，即把音量设为零，但不影响 ``V4L2_CID_AUDIO_VOLUME`` 。与 ALSA
    驱动一样，V4L2 驱动必须在加载时静音以避免过大的噪声。实际上整个设备都应
    被复位到低功耗状态。)

``V4L2_CID_AUDIO_LOUDNESS`` ``(boolean)``
    Loudness mode (bass boost).
    (响度模式 (低音增强)。)

``V4L2_CID_BLACK_LEVEL`` ``(integer)``
    Another name for brightness (not a synonym of
    ``V4L2_CID_BRIGHTNESS``). This control is deprecated and should not
    be used in new drivers and applications.
    (亮度的另一个名字 (不是 ``V4L2_CID_BRIGHTNESS`` 的同义词)。该控件已废弃，
    不应在新的驱动程序和应用程序中使用。)

``V4L2_CID_AUTO_WHITE_BALANCE`` ``(boolean)``
    Automatic white balance (cameras).
    (自动白平衡 (摄像头)。)

``V4L2_CID_DO_WHITE_BALANCE`` ``(button)``
    This is an action control. When set (the value is ignored), the
    device will do a white balance and then hold the current setting.
    Contrast this with the boolean ``V4L2_CID_AUTO_WHITE_BALANCE``,
    which, when activated, keeps adjusting the white balance.
    (这是一个动作控件。设置它时 (值被忽略)，设备会执行一次白平衡，然后保持
    当前设置。与之对比，布尔型的 ``V4L2_CID_AUTO_WHITE_BALANCE`` 在激活后会
    持续调整白平衡。)

``V4L2_CID_RED_BALANCE`` ``(integer)``
    Red chroma balance.
    (红色色度平衡。)

``V4L2_CID_BLUE_BALANCE`` ``(integer)``
    Blue chroma balance.
    (蓝色色度平衡。)

``V4L2_CID_GAMMA`` ``(integer)``
    Gamma adjust.
    (伽马调整。)

``V4L2_CID_WHITENESS`` ``(integer)``
    Whiteness for grey-scale devices. This is a synonym for
    ``V4L2_CID_GAMMA``. This control is deprecated and should not be
    used in new drivers and applications.
    (灰度设备的白度。它是 ``V4L2_CID_GAMMA`` 的同义词。该控件已废弃，不应在
    新的驱动程序和应用程序中使用。)

``V4L2_CID_EXPOSURE`` ``(integer)``
    Exposure (cameras). [Unit?]
    (曝光 (摄像头)。[单位？])

``V4L2_CID_AUTOGAIN`` ``(boolean)``
    Automatic gain/exposure control.
    (自动增益/曝光控制。)

``V4L2_CID_GAIN`` ``(integer)``
    Gain control.
    (增益控制。)

    Primarily used to control gain on e.g. TV tuners but also on
    webcams. Most devices control only digital gain with this control
    but on some this could include analogue gain as well. Devices that
    recognise the difference between digital and analogue gain use
    controls ``V4L2_CID_DIGITAL_GAIN`` and ``V4L2_CID_ANALOGUE_GAIN``.
    (主要用于控制电视调谐器等设备的增益，也用于摄像头。大多数设备仅用该控件
    控制数字增益，但在某些设备上它也可能包括模拟增益。能够区分数字增益与模拟
    增益的设备会使用 ``V4L2_CID_DIGITAL_GAIN`` 与 ``V4L2_CID_ANALOGUE_GAIN``
    控件。)

.. _v4l2-cid-hflip:

``V4L2_CID_HFLIP`` ``(boolean)``
    Mirror the picture horizontally.
    (水平镜像图像。)

.. _v4l2-cid-vflip:

``V4L2_CID_VFLIP`` ``(boolean)``
    Mirror the picture vertically.
    (垂直镜像图像。)

.. _v4l2-power-line-frequency:

``V4L2_CID_POWER_LINE_FREQUENCY`` ``(enum)``
    Enables a power line frequency filter to avoid flicker. Possible
    values for ``enum v4l2_power_line_frequency`` are:
    (启用电源线频率滤波器以避免闪烁。 ``enum v4l2_power_line_frequency`` 的
    可能取值如下：)

    ==========================================  ==
    ``V4L2_CID_POWER_LINE_FREQUENCY_DISABLED``	 0
    ``V4L2_CID_POWER_LINE_FREQUENCY_50HZ``	 1
    ``V4L2_CID_POWER_LINE_FREQUENCY_60HZ``	 2
    ``V4L2_CID_POWER_LINE_FREQUENCY_AUTO``	 3
    ==========================================  ==

``V4L2_CID_HUE_AUTO`` ``(boolean)``
    Enables automatic hue control by the device. The effect of setting
    ``V4L2_CID_HUE`` while automatic hue control is enabled is
    undefined, drivers should ignore such request.
    (启用设备端的自动色调控制。在自动色调控制启用时设置 ``V4L2_CID_HUE`` 的
    效果是未定义的，驱动程序应忽略这类请求。)

``V4L2_CID_WHITE_BALANCE_TEMPERATURE`` ``(integer)``
    This control specifies the white balance settings as a color
    temperature in Kelvin. A driver should have a minimum of 2800
    (incandescent) to 6500 (daylight). For more information about color
    temperature see
    `Wikipedia <http://en.wikipedia.org/wiki/Color_temperature>`__.
    (该控件以开尔文为单位的色温来指定白平衡设置。驱动程序的最小值应达到
    2800 (白炽灯) 到 6500 (日光)。关于色温的更多信息请参阅维基百科
    "Color temperature" 词条。)

``V4L2_CID_SHARPNESS`` ``(integer)``
    Adjusts the sharpness filters in a camera. The minimum value
    disables the filters, higher values give a sharper picture.
    (调整摄像机的锐度滤镜。最小值会禁用滤镜，更高的值给出更锐利的图像。)

``V4L2_CID_BACKLIGHT_COMPENSATION`` ``(integer)``
    Adjusts the backlight compensation in a camera. The minimum value
    disables backlight compensation.
    (调整摄像机的背光补偿。最小值会禁用背光补偿。)

``V4L2_CID_CHROMA_AGC`` ``(boolean)``
    Chroma automatic gain control.
    (色度自动增益控制。)

``V4L2_CID_CHROMA_GAIN`` ``(integer)``
    Adjusts the Chroma gain control (for use when chroma AGC is
    disabled).
    (调整色度增益控件 (在色度 AGC 被禁用时使用)。)

``V4L2_CID_COLOR_KILLER`` ``(boolean)``
    Enable the color killer (i. e. force a black & white image in case
    of a weak video signal).
    (启用色度杀手 (即在视频信号较弱时强制黑白图像)。)

.. _v4l2-colorfx:

``V4L2_CID_COLORFX`` ``(enum)``
    Selects a color effect. The following values are defined:
    (选择一种色彩效果。定义了以下取值：)



.. tabularcolumns:: |p{5.7cm}|p{11.8cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 11 24

    * - ``V4L2_COLORFX_NONE``
      - Color effect is disabled.
        (禁用色彩效果。)
    * - ``V4L2_COLORFX_ANTIQUE``
      - An aging (old photo) effect.
        (做旧 (老照片) 效果。)
    * - ``V4L2_COLORFX_ART_FREEZE``
      - Frost color effect.
        (冰冻色彩效果。)
    * - ``V4L2_COLORFX_AQUA``
      - Water color, cool tone.
        (水色，冷色调。)
    * - ``V4L2_COLORFX_BW``
      - Black and white.
        (黑白。)
    * - ``V4L2_COLORFX_EMBOSS``
      - Emboss, the highlights and shadows replace light/dark boundaries
	and low contrast areas are set to a gray background.
        (浮雕，高光和阴影取代明暗边界，低对比度区域被设为灰色背景。)
    * - ``V4L2_COLORFX_GRASS_GREEN``
      - Grass green.
        (草绿色。)
    * - ``V4L2_COLORFX_NEGATIVE``
      - Negative.
        (负片。)
    * - ``V4L2_COLORFX_SEPIA``
      - Sepia tone.
        (棕褐色调。)
    * - ``V4L2_COLORFX_SKETCH``
      - Sketch.
        (素描。)
    * - ``V4L2_COLORFX_SKIN_WHITEN``
      - Skin whiten.
        (肤色增白。)
    * - ``V4L2_COLORFX_SKY_BLUE``
      - Sky blue.
        (天蓝色。)
    * - ``V4L2_COLORFX_SOLARIZATION``
      - Solarization, the image is partially reversed in tone, only color
	values above or below a certain threshold are inverted.
        (曝光过度，图像色调部分反转，仅高于或低于某一阈值的颜色值被反转。)
    * - ``V4L2_COLORFX_SILHOUETTE``
      - Silhouette (outline).
        (剪影 (轮廓)。)
    * - ``V4L2_COLORFX_VIVID``
      - Vivid colors.
        (鲜艳色彩。)
    * - ``V4L2_COLORFX_SET_CBCR``
      - The Cb and Cr chroma components are replaced by fixed coefficients
	determined by ``V4L2_CID_COLORFX_CBCR`` control.
        ( Cb 与 Cr 色度分量被替换为固定的系数，该系数由
        ``V4L2_CID_COLORFX_CBCR`` 控件确定。)
    * - ``V4L2_COLORFX_SET_RGB``
      - The RGB components are replaced by the fixed RGB components determined
        by ``V4L2_CID_COLORFX_RGB`` control.
        ( RGB 分量被替换为固定的 RGB 分量，该分量由
        ``V4L2_CID_COLORFX_RGB`` 控件确定。)


``V4L2_CID_COLORFX_RGB`` ``(integer)``
    Determines the Red, Green, and Blue coefficients for
    ``V4L2_COLORFX_SET_RGB`` color effect.
    Bits [7:0] of the supplied 32 bit value are interpreted as Blue component,
    bits [15:8] as Green component, bits [23:16] as Red component, and
    bits [31:24] must be zero.
    (为 ``V4L2_COLORFX_SET_RGB`` 色彩效果确定红、绿、蓝系数。所给 32 位值的
    位 [7:0] 被解释为蓝色分量，位 [15:8] 为绿色分量，位 [23:16] 为红色分量，
    位 [31:24] 必须为零。)

``V4L2_CID_COLORFX_CBCR`` ``(integer)``
    Determines the Cb and Cr coefficients for ``V4L2_COLORFX_SET_CBCR``
    color effect. Bits [7:0] of the supplied 32 bit value are
    interpreted as Cr component, bits [15:8] as Cb component and bits
    [31:16] must be zero.
    (为 ``V4L2_COLORFX_SET_CBCR`` 色彩效果确定 Cb 与 Cr 系数。所给 32 位值的
    位 [7:0] 被解释为 Cr 分量，位 [15:8] 为 Cb 分量，位 [31:16] 必须为零。)

``V4L2_CID_AUTOBRIGHTNESS`` ``(boolean)``
    Enable Automatic Brightness.
    (启用自动亮度。)

``V4L2_CID_ROTATE`` ``(integer)``
    Rotates the image by specified angle. Common angles are 90, 270 and
    180. Rotating the image to 90 and 270 will reverse the height and
    width of the display window. It is necessary to set the new height
    and width of the picture using the
    :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl according to the
    rotation angle selected.
    (将图像旋转指定的角度。常见角度为 90、270 和 180。将图像旋转 90 和 270
    度会交换显示窗口的高度与宽度。需要根据所选的旋转角度，用
    :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 设置图像的新高度和宽度。)

``V4L2_CID_BG_COLOR`` ``(integer)``
    Sets the background color on the current output device. Background
    color needs to be specified in the RGB24 format. The supplied 32 bit
    value is interpreted as bits 0-7 Red color information, bits 8-15
    Green color information, bits 16-23 Blue color information and bits
    24-31 must be zero.
    (在当前输出设备上设置背景色。背景色需要以 RGB24 格式指定。所给 32 位值的
    位 0-7 被解释为红色颜色信息，位 8-15 为绿色颜色信息，位 16-23 为蓝色颜色
    信息，位 24-31 必须为零。)

``V4L2_CID_ILLUMINATORS_1 V4L2_CID_ILLUMINATORS_2`` ``(boolean)``
    Switch on or off the illuminator 1 or 2 of the device (usually a
    microscope).
    (打开或关闭设备的照明器 1 或 2 (通常是显微镜)。)

``V4L2_CID_MIN_BUFFERS_FOR_CAPTURE`` ``(integer)``
    This is a read-only control that can be read by the application and
    used as a hint to determine the number of CAPTURE buffers to pass to
    REQBUFS. The value is the minimum number of CAPTURE buffers that is
    necessary for hardware to work. This control is required for stateful
    decoders.
    (这是一个只读控件，应用程序可以读取它，作为确定要传给 REQBUFS 的 CAPTURE
    缓冲区数量的提示。该值是硬件正常工作所需的 CAPTURE 缓冲区最小数量。有状态
    解码器必须提供该控件。)

``V4L2_CID_MIN_BUFFERS_FOR_OUTPUT`` ``(integer)``
    This is a read-only control that can be read by the application and
    used as a hint to determine the number of OUTPUT buffers to pass to
    REQBUFS. The value is the minimum number of OUTPUT buffers that is
    necessary for hardware to work. This control is required for stateful
    encoders.
    (这是一个只读控件，应用程序可以读取它，作为确定要传给 REQBUFS 的 OUTPUT
    缓冲区数量的提示。该值是硬件正常工作所需的 OUTPUT 缓冲区最小数量。有状态
    编码器必须提供该控件。)

.. _v4l2-alpha-component:

``V4L2_CID_ALPHA_COMPONENT`` ``(integer)``
    Sets the alpha color component. When a capture device (or capture
    queue of a mem-to-mem device) produces a frame format that includes
    an alpha component (e.g.
    :ref:`packed RGB image formats <pixfmt-rgb>`) and the alpha value
    is not defined by the device or the mem-to-mem input data this
    control lets you select the alpha component value of all pixels.
    When an output device (or output queue of a mem-to-mem device)
    consumes a frame format that doesn't include an alpha component and
    the device supports alpha channel processing this control lets you
    set the alpha component value of all pixels for further processing
    in the device.
    (设置 alpha 颜色分量。当捕获设备 (或 mem-to-mem 设备的捕获队列) 产生的帧
    格式包含 alpha 分量 (例如 :ref:`packed RGB image formats <pixfmt-rgb>` )
    且 alpha 值未由设备或 mem-to-mem 输入数据定义时，该控件让你选择所有像素的
    alpha 分量值。当输出设备 (或 mem-to-mem 设备的输出队列) 消费的帧格式不包含
    alpha 分量且设备支持 alpha 通道处理时，该控件让你为设备中的后续处理设置
    所有像素的 alpha 分量值。)

``V4L2_CID_LASTP1``
    End of the predefined control IDs (currently
    ``V4L2_CID_ALPHA_COMPONENT`` + 1).
    (预定义控件 ID 的末尾 (当前为 ``V4L2_CID_ALPHA_COMPONENT`` + 1)。)

``V4L2_CID_PRIVATE_BASE``
    ID of the first custom (driver specific) control. Applications
    depending on particular custom controls should check the driver name
    and version, see :ref:`querycap`.
    (第一个自定义 (驱动专用) 控件的 ID。依赖特定自定义控件的应用程序应检查
    驱动程序的名称和版本，参见 :ref:`querycap` 。)

Applications can enumerate the available controls with the
:ref:`VIDIOC_QUERYCTRL` and
:ref:`VIDIOC_QUERYMENU <VIDIOC_QUERYCTRL>` ioctls, get and set a
control value with the :ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` and
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls. Drivers must implement
``VIDIOC_QUERYCTRL``, ``VIDIOC_G_CTRL`` and ``VIDIOC_S_CTRL`` when the
device has one or more controls, ``VIDIOC_QUERYMENU`` when it has one or
more menu type controls.
(应用程序可以用 :ref:`VIDIOC_QUERYCTRL` 和 :ref:`VIDIOC_QUERYMENU <VIDIOC_QUERYCTRL>`
ioctl 枚举可用控件，用 :ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` 和
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl 读取和设置控件值。设备拥有一个或
多个控件时，驱动程序必须实现 ``VIDIOC_QUERYCTRL`` 、 ``VIDIOC_G_CTRL`` 和
``VIDIOC_S_CTRL`` ；拥有一个或多个菜单型控件时，必须实现 ``VIDIOC_QUERYMENU`` 。)


.. _enum_all_controls:

Example: Enumerating all controls(示例：枚举所有控件)
=====================================================

.. code-block:: c

    struct v4l2_queryctrl queryctrl;
    struct v4l2_querymenu querymenu;

    static void enumerate_menu(__u32 id)
    {
	printf("  Menu items:\\n");

	memset(&querymenu, 0, sizeof(querymenu));
	querymenu.id = id;

	for (querymenu.index = queryctrl.minimum;
	     querymenu.index <= queryctrl.maximum;
	     querymenu.index++) {
	    if (0 == ioctl(fd, VIDIOC_QUERYMENU, &querymenu)) {
		printf("  %s\\n", querymenu.name);
	    }
	}
    }

    memset(&queryctrl, 0, sizeof(queryctrl));

    queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
	if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
	    printf("Control %s\\n", queryctrl.name);

	    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
	        enumerate_menu(queryctrl.id);
        }

	queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    if (errno != EINVAL) {
	perror("VIDIOC_QUERYCTRL");
	exit(EXIT_FAILURE);
    }

Example: Enumerating all controls including compound controls(示例：枚举包括复合控件在内的所有控件)
===================================================================================================

.. code-block:: c

    struct v4l2_query_ext_ctrl query_ext_ctrl;

    memset(&query_ext_ctrl, 0, sizeof(query_ext_ctrl));

    query_ext_ctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL | V4L2_CTRL_FLAG_NEXT_COMPOUND;
    while (0 == ioctl(fd, VIDIOC_QUERY_EXT_CTRL, &query_ext_ctrl)) {
	if (!(query_ext_ctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
	    printf("Control %s\\n", query_ext_ctrl.name);

	    if (query_ext_ctrl.type == V4L2_CTRL_TYPE_MENU)
	        enumerate_menu(query_ext_ctrl.id);
        }

	query_ext_ctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL | V4L2_CTRL_FLAG_NEXT_COMPOUND;
    }
    if (errno != EINVAL) {
	perror("VIDIOC_QUERY_EXT_CTRL");
	exit(EXIT_FAILURE);
    }

Example: Enumerating all user controls (old style)(示例：枚举所有用户控件 (旧式))
=================================================================================

.. code-block:: c


    memset(&queryctrl, 0, sizeof(queryctrl));

    for (queryctrl.id = V4L2_CID_BASE;
	 queryctrl.id < V4L2_CID_LASTP1;
	 queryctrl.id++) {
	if (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
	    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
		continue;

	    printf("Control %s\\n", queryctrl.name);

	    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
		enumerate_menu(queryctrl.id);
	} else {
	    if (errno == EINVAL)
		continue;

	    perror("VIDIOC_QUERYCTRL");
	    exit(EXIT_FAILURE);
	}
    }

    for (queryctrl.id = V4L2_CID_PRIVATE_BASE;;
	 queryctrl.id++) {
	if (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
	    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
		continue;

	    printf("Control %s\\n", queryctrl.name);

	    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
		enumerate_menu(queryctrl.id);
	} else {
	    if (errno == EINVAL)
		break;

	    perror("VIDIOC_QUERYCTRL");
	    exit(EXIT_FAILURE);
	}
    }


Example: Changing controls(示例：更改控件)
==========================================

.. code-block:: c

    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;

    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = V4L2_CID_BRIGHTNESS;

    if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
	if (errno != EINVAL) {
	    perror("VIDIOC_QUERYCTRL");
	    exit(EXIT_FAILURE);
	} else {
	    printf("V4L2_CID_BRIGHTNESS is not supported\n");
	}
    } else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
	printf("V4L2_CID_BRIGHTNESS is not supported\n");
    } else {
	memset(&control, 0, sizeof (control));
	control.id = V4L2_CID_BRIGHTNESS;
	control.value = queryctrl.default_value;

	if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)) {
	    perror("VIDIOC_S_CTRL");
	    exit(EXIT_FAILURE);
	}
    }

    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_CONTRAST;

    if (0 == ioctl(fd, VIDIOC_G_CTRL, &control)) {
	control.value += 1;

	/* The driver may clamp the value or return ERANGE, ignored here */

	if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)
	    && errno != ERANGE) {
	    perror("VIDIOC_S_CTRL");
	    exit(EXIT_FAILURE);
	}
    /* Ignore if V4L2_CID_CONTRAST is unsupported */
    } else if (errno != EINVAL) {
	perror("VIDIOC_G_CTRL");
	exit(EXIT_FAILURE);
    }

    control.id = V4L2_CID_AUDIO_MUTE;
    control.value = 1; /* silence */

    /* Errors ignored */
    ioctl(fd, VIDIOC_S_CTRL, &control);

.. [#f1]
   The use of ``V4L2_CID_PRIVATE_BASE`` is problematic because different
   drivers may use the same ``V4L2_CID_PRIVATE_BASE`` ID for different
   controls. This makes it hard to programmatically set such controls
   since the meaning of the control with that ID is driver dependent. In
   order to resolve this drivers use unique IDs and the
   ``V4L2_CID_PRIVATE_BASE`` IDs are mapped to those unique IDs by the
   kernel. Consider these ``V4L2_CID_PRIVATE_BASE`` IDs as aliases to
   the real IDs.
   (使用 ``V4L2_CID_PRIVATE_BASE`` 存在问题，因为不同的驱动程序可能把同一个
   ``V4L2_CID_PRIVATE_BASE`` ID 用在不同的控件上。这使得以编程方式设置这类控件
   变得困难，因为该 ID 所对应控件的含义依赖于具体的驱动程序。为了解决这一问题，
   驱动程序使用唯一的 ID，而 ``V4L2_CID_PRIVATE_BASE`` ID 由内核映射到那些唯一
   ID。可以把这些 ``V4L2_CID_PRIVATE_BASE`` ID 视为真实 ID 的别名。)

   Many applications today still use the ``V4L2_CID_PRIVATE_BASE`` IDs
   instead of using :ref:`VIDIOC_QUERYCTRL` with
   the ``V4L2_CTRL_FLAG_NEXT_CTRL`` flag to enumerate all IDs, so
   support for ``V4L2_CID_PRIVATE_BASE`` is still around.
   (如今许多应用程序仍在使用 ``V4L2_CID_PRIVATE_BASE`` ID，而不是配合
   ``V4L2_CTRL_FLAG_NEXT_CTRL`` 标志使用 :ref:`VIDIOC_QUERYCTRL` 来枚举全部
   ID，因此对 ``V4L2_CID_PRIVATE_BASE`` 的支持依然存在。)
