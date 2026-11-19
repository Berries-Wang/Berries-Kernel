.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _camera-controls:

**************************************
Camera Control Reference(相机控制参考)
**************************************

The Camera class includes controls for mechanical (or equivalent
digital) features of a device such as controllable lenses or sensors.
(Camera 类包含设备的机械（或等效的数字）特性控制项，例如可控镜头或
传感器。)


.. _camera-control-id:

Camera Control IDs(相机控制 ID)
===============================

``V4L2_CID_CAMERA_CLASS (class)``
    The Camera class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (Camera 类描述符。对此控制项调用 :ref:`VIDIOC_QUERYCTRL` 将返回该控制类
    的描述。)

.. _v4l2-exposure-auto-type:

``V4L2_CID_EXPOSURE_AUTO``
    (enum)

enum v4l2_exposure_auto_type -
    Enables automatic adjustments of the exposure time and/or iris
    aperture. The effect of manual changes of the exposure time or iris
    aperture while these features are enabled is undefined, drivers
    should ignore such requests. Possible values are:
    (启用曝光时间和/或光圈大小的自动调整。在这些特性启用期间手动更改曝光
    时间或光圈的效果未定义，驱动应忽略此类请求。可能的取值有：)


.. tabularcolumns:: |p{7.1cm}|p{10.4cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_EXPOSURE_AUTO``
      - Automatic exposure time, automatic iris aperture.
	(自动曝光时间，自动光圈。)
    * - ``V4L2_EXPOSURE_MANUAL``
      - Manual exposure time, manual iris.
	(手动曝光时间，手动光圈。)
    * - ``V4L2_EXPOSURE_SHUTTER_PRIORITY``
      - Manual exposure time, auto iris.
	(手动曝光时间，自动光圈。)
    * - ``V4L2_EXPOSURE_APERTURE_PRIORITY``
      - Auto exposure time, manual iris.
	(自动曝光时间，手动光圈。)



``V4L2_CID_EXPOSURE_ABSOLUTE (integer)``
    Determines the exposure time of the camera sensor. The exposure time
    is limited by the frame interval. Drivers should interpret the
    values as 100 µs units, where the value 1 stands for 1/10000th of a
    second, 10000 for 1 second and 100000 for 10 seconds.
    (确定相机传感器的曝光时间。曝光时间受帧间隔限制。驱动应把取值解释为
    100 µs 单位：值 1 表示 1/10000 秒，10000 表示 1 秒，100000 表示 10
    秒。)

``V4L2_CID_EXPOSURE_AUTO_PRIORITY (boolean)``
    When ``V4L2_CID_EXPOSURE_AUTO`` is set to ``AUTO`` or
    ``APERTURE_PRIORITY``, this control determines if the device may
    dynamically vary the frame rate. By default this feature is disabled
    (0) and the frame rate must remain constant.
    (当 ``V4L2_CID_EXPOSURE_AUTO`` 设置为 ``AUTO`` 或 ``APERTURE_PRIORITY``
    时，此控制项决定设备是否可以动态改变帧率。默认禁用该特性 (0)，帧率必须
    保持恒定。)

``V4L2_CID_AUTO_EXPOSURE_BIAS (integer menu)``
    Determines the automatic exposure compensation, it is effective only
    when ``V4L2_CID_EXPOSURE_AUTO`` control is set to ``AUTO``,
    ``SHUTTER_PRIORITY`` or ``APERTURE_PRIORITY``. It is expressed in
    terms of EV, drivers should interpret the values as 0.001 EV units,
    where the value 1000 stands for +1 EV.
    (确定自动曝光补偿；仅当 ``V4L2_CID_EXPOSURE_AUTO`` 控制项设置为
    ``AUTO``、``SHUTTER_PRIORITY`` 或 ``APERTURE_PRIORITY`` 时有效。以 EV
    表示，驱动应把取值解释为 0.001 EV 单位：值 1000 表示 +1 EV。)

    Increasing the exposure compensation value is equivalent to
    decreasing the exposure value (EV) and will increase the amount of
    light at the image sensor. The camera performs the exposure
    compensation by adjusting absolute exposure time and/or aperture.
    (增大曝光补偿值等同于减小曝光值 (EV)，将增加图像传感器处的光量。相机
    通过调整绝对曝光时间和/或光圈来执行曝光补偿。)

.. _v4l2-exposure-metering:

``V4L2_CID_EXPOSURE_METERING``
    (enum)

enum v4l2_exposure_metering -
    Determines how the camera measures the amount of light available for
    the frame exposure. Possible values are:
    (确定相机如何测量可用于帧曝光的光量。可能的取值有：)

.. tabularcolumns:: |p{8.7cm}|p{8.7cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_EXPOSURE_METERING_AVERAGE``
      - Use the light information coming from the entire frame and average
	giving no weighting to any particular portion of the metered area.
	(使用来自整个帧的光信息并取平均，不对测光区域的任何特定部分加权。)
    * - ``V4L2_EXPOSURE_METERING_CENTER_WEIGHTED``
      - Average the light information coming from the entire frame giving
	priority to the center of the metered area.
	(对来自整个帧的光信息取平均，并优先考虑测光区域的中心。)
    * - ``V4L2_EXPOSURE_METERING_SPOT``
      - Measure only very small area at the center of the frame.
	(只测量帧中心非常小的区域。)
    * - ``V4L2_EXPOSURE_METERING_MATRIX``
      - A multi-zone metering. The light intensity is measured in several
	points of the frame and the results are combined. The algorithm of
	the zones selection and their significance in calculating the
	final value is device dependent.
	(多区域测光。在帧的多个点测量光强并合并结果。区域选择算法及其在计算
	最终值时的重要性取决于设备。)



``V4L2_CID_PAN_RELATIVE (integer)``
    This control turns the camera horizontally by the specified amount.
    The unit is undefined. A positive value moves the camera to the
    right (clockwise when viewed from above), a negative value to the
    left. A value of zero does not cause motion. This is a write-only
    control.
    (此控制项使相机水平转动指定量。单位未定义。正值使相机向右移动（从上方
    看为顺时针），负值向左。值为零不产生运动。这是一个只写控制项。)

``V4L2_CID_TILT_RELATIVE (integer)``
    This control turns the camera vertically by the specified amount.
    The unit is undefined. A positive value moves the camera up, a
    negative value down. A value of zero does not cause motion. This is
    a write-only control.
    (此控制项使相机垂直转动指定量。单位未定义。正值使相机向上移动，负值
    向下。值为零不产生运动。这是一个只写控制项。)

``V4L2_CID_PAN_RESET (button)``
    When this control is set, the camera moves horizontally to the
    default position.
    (设置此控制项时，相机水平移动到默认位置。)

``V4L2_CID_TILT_RESET (button)``
    When this control is set, the camera moves vertically to the default
    position.
    (设置此控制项时，相机垂直移动到默认位置。)

``V4L2_CID_PAN_ABSOLUTE (integer)``
    This control turns the camera horizontally to the specified
    position. Positive values move the camera to the right (clockwise
    when viewed from above), negative values to the left. Drivers should
    interpret the values as arc seconds, with valid values between -180
    * 3600 and +180 * 3600 inclusive.
    (此控制项使相机水平转动到指定位置。正值使相机向右移动（从上方看为
    顺时针），负值向左。驱动应把取值解释为角秒，有效值介于 -180 * 3600 与
    +180 * 3600 之间（含边界）。)

``V4L2_CID_TILT_ABSOLUTE (integer)``
    This control turns the camera vertically to the specified position.
    Positive values move the camera up, negative values down. Drivers
    should interpret the values as arc seconds, with valid values
    between -180 * 3600 and +180 * 3600 inclusive.
    (此控制项使相机垂直转动到指定位置。正值使相机向上移动，负值向下。
    驱动应把取值解释为角秒，有效值介于 -180 * 3600 与 +180 * 3600 之间
    （含边界）。)

``V4L2_CID_FOCUS_ABSOLUTE (integer)``
    This control sets the focal point of the camera to the specified
    position. The unit is undefined. Positive values set the focus
    closer to the camera, negative values towards infinity.
    (此控制项把相机焦点设置到指定位置。单位未定义。正值把焦点设置得离相机
    更近，负值朝向无穷远。)

``V4L2_CID_FOCUS_RELATIVE (integer)``
    This control moves the focal point of the camera by the specified
    amount. The unit is undefined. Positive values move the focus closer
    to the camera, negative values towards infinity. This is a
    write-only control.
    (此控制项按指定量移动相机焦点。单位未定义。正值使焦点离相机更近，负值
    朝向无穷远。这是一个只写控制项。)

``V4L2_CID_FOCUS_AUTO (boolean)``
    Enables continuous automatic focus adjustments. The effect of manual
    focus adjustments while this feature is enabled is undefined,
    drivers should ignore such requests.
    (启用连续的自动对焦调整。在该特性启用期间手动对焦的效果未定义，驱动应
    忽略此类请求。)

``V4L2_CID_AUTO_FOCUS_START (button)``
    Starts single auto focus process. The effect of setting this control
    when ``V4L2_CID_FOCUS_AUTO`` is set to ``TRUE`` (1) is undefined,
    drivers should ignore such requests.
    (启动单次自动对焦过程。当 ``V4L2_CID_FOCUS_AUTO`` 设置为 ``TRUE`` (1)
    时设置此控制项的效果未定义，驱动应忽略此类请求。)

``V4L2_CID_AUTO_FOCUS_STOP (button)``
    Aborts automatic focusing started with ``V4L2_CID_AUTO_FOCUS_START``
    control. It is effective only when the continuous autofocus is
    disabled, that is when ``V4L2_CID_FOCUS_AUTO`` control is set to
    ``FALSE`` (0).
    (中止由 ``V4L2_CID_AUTO_FOCUS_START`` 控制项启动的自动对焦。仅当连续
    自动对焦被禁用时才有效，即 ``V4L2_CID_FOCUS_AUTO`` 控制项设置为
    ``FALSE`` (0) 时。)

.. _v4l2-auto-focus-status:

``V4L2_CID_AUTO_FOCUS_STATUS (bitmask)``
    The automatic focus status. This is a read-only control.
    (自动对焦状态。这是一个只读控制项。)

    Setting ``V4L2_LOCK_FOCUS`` lock bit of the ``V4L2_CID_3A_LOCK``
    control may stop updates of the ``V4L2_CID_AUTO_FOCUS_STATUS``
    control value.
    (设置 ``V4L2_CID_3A_LOCK`` 控制项的 ``V4L2_LOCK_FOCUS`` 锁定位可能会
    停止 ``V4L2_CID_AUTO_FOCUS_STATUS`` 控制项值的更新。)

.. tabularcolumns:: |p{6.8cm}|p{10.7cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_AUTO_FOCUS_STATUS_IDLE``
      - Automatic focus is not active.
	(自动对焦未激活。)
    * - ``V4L2_AUTO_FOCUS_STATUS_BUSY``
      - Automatic focusing is in progress.
	(自动对焦正在进行。)
    * - ``V4L2_AUTO_FOCUS_STATUS_REACHED``
      - Focus has been reached.
	(已达到焦点。)
    * - ``V4L2_AUTO_FOCUS_STATUS_FAILED``
      - Automatic focus has failed, the driver will not transition from
	this state until another action is performed by an application.
	(自动对焦失败；在应用程序执行另一个动作之前，驱动不会离开此状态。)



.. _v4l2-auto-focus-range:

``V4L2_CID_AUTO_FOCUS_RANGE``
    (enum)

enum v4l2_auto_focus_range -
    Determines auto focus distance range for which lens may be adjusted.
    (确定镜头可以调整的自动对焦距离范围。)

.. tabularcolumns:: |p{6.9cm}|p{10.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_AUTO_FOCUS_RANGE_AUTO``
      - The camera automatically selects the focus range.
	(相机自动选择对焦范围。)
    * - ``V4L2_AUTO_FOCUS_RANGE_NORMAL``
      - Normal distance range, limited for best automatic focus
	performance.
	(正常距离范围，为获得最佳自动对焦性能而受限。)
    * - ``V4L2_AUTO_FOCUS_RANGE_MACRO``
      - Macro (close-up) auto focus. The camera will use its minimum
	possible distance for auto focus.
	(微距（近摄）自动对焦。相机将使用其可能的最小距离进行自动对焦。)
    * - ``V4L2_AUTO_FOCUS_RANGE_INFINITY``
      - The lens is set to focus on an object at infinite distance.
	(镜头被设置为对焦于无限远处的物体。)



``V4L2_CID_ZOOM_ABSOLUTE (integer)``
    Specify the objective lens focal length as an absolute value. The
    zoom unit is driver-specific and its value should be a positive
    integer.
    (以绝对值指定物镜焦距。变焦单位由驱动决定，其值应为正整数。)

``V4L2_CID_ZOOM_RELATIVE (integer)``
    Specify the objective lens focal length relatively to the current
    value. Positive values move the zoom lens group towards the
    telephoto direction, negative values towards the wide-angle
    direction. The zoom unit is driver-specific. This is a write-only
    control.
    (相对于当前值指定物镜焦距。正值使变焦镜头组朝长焦方向移动，负值朝广角
    方向移动。变焦单位由驱动决定。这是一个只写控制项。)

``V4L2_CID_ZOOM_CONTINUOUS (integer)``
    Move the objective lens group at the specified speed until it
    reaches physical device limits or until an explicit request to stop
    the movement. A positive value moves the zoom lens group towards the
    telephoto direction. A value of zero stops the zoom lens group
    movement. A negative value moves the zoom lens group towards the
    wide-angle direction. The zoom speed unit is driver-specific.
    (以指定速度移动物镜组，直到达到设备物理限制或收到明确的停止请求。正值
    使变焦镜头组朝长焦方向移动。值为零停止变焦镜头组运动。负值使变焦镜头
    组朝广角方向移动。变焦速度单位由驱动决定。)

``V4L2_CID_IRIS_ABSOLUTE (integer)``
    This control sets the camera's aperture to the specified value. The
    unit is undefined. Larger values open the iris wider, smaller values
    close it.
    (此控制项把相机光圈设置到指定值。单位未定义。较大的值把光圈开得更大，
    较小的值把光圈收得更小。)

``V4L2_CID_IRIS_RELATIVE (integer)``
    This control modifies the camera's aperture by the specified amount.
    The unit is undefined. Positive values open the iris one step
    further, negative values close it one step further. This is a
    write-only control.
    (此控制项按指定量修改相机光圈。单位未定义。正值把光圈再开大一档，负值
    把光圈再收小一档。这是一个只写控制项。)

``V4L2_CID_PRIVACY (boolean)``
    Prevent video from being acquired by the camera. When this control
    is set to ``TRUE`` (1), no image can be captured by the camera.
    Common means to enforce privacy are mechanical obturation of the
    sensor and firmware image processing, but the device is not
    restricted to these methods. Devices that implement the privacy
    control must support read access and may support write access.
    (阻止相机采集视频。当此控制项设置为 ``TRUE`` (1) 时，相机无法采集任何
    图像。常见的隐私保护手段包括对传感器的机械遮挡和固件图像处理，但设备
    不限于这些方法。实现隐私控制项的设备必须支持读取访问，并可以支持写入
    访问。)

``V4L2_CID_BAND_STOP_FILTER (integer)``
    Switch the band-stop filter of a camera sensor on or off, or specify
    its strength. Such band-stop filters can be used, for example, to
    filter out the fluorescent light component.
    (打开或关闭相机传感器的带阻滤波器，或指定其强度。此类带阻滤波器例如可
    用于滤除荧光灯成分。)

.. _v4l2-auto-n-preset-white-balance:

``V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE``
    (enum)

enum v4l2_auto_n_preset_white_balance -
    Sets white balance to automatic, manual or a preset. The presets
    determine color temperature of the light as a hint to the camera for
    white balance adjustments resulting in most accurate color
    representation. The following white balance presets are listed in
    order of increasing color temperature.
    (把白平衡设置为自动、手动或某个预设。预设确定光线的色温，作为相机进行
    白平衡调整、从而获得最准确色彩表现的提示。以下白平衡预设按色温递增的
    顺序列出。)

.. tabularcolumns:: |p{7.4cm}|p{10.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_WHITE_BALANCE_MANUAL``
      - Manual white balance.
	(手动白平衡。)
    * - ``V4L2_WHITE_BALANCE_AUTO``
      - Automatic white balance adjustments.
	(自动白平衡调整。)
    * - ``V4L2_WHITE_BALANCE_INCANDESCENT``
      - White balance setting for incandescent (tungsten) lighting. It
	generally cools down the colors and corresponds approximately to
	2500...3500 K color temperature range.
	(针对白炽（钨丝）照明的白平衡设置。它通常使色彩偏冷，大致对应
	2500...3500 K 色温范围。)
    * - ``V4L2_WHITE_BALANCE_FLUORESCENT``
      - White balance preset for fluorescent lighting. It corresponds
	approximately to 4000...5000 K color temperature.
	(针对荧光灯照明的白平衡预设。大致对应 4000...5000 K 色温。)
    * - ``V4L2_WHITE_BALANCE_FLUORESCENT_H``
      - With this setting the camera will compensate for fluorescent H
	lighting.
	(此设置下相机会补偿荧光 H 照明。)
    * - ``V4L2_WHITE_BALANCE_HORIZON``
      - White balance setting for horizon daylight. It corresponds
	approximately to 5000 K color temperature.
	(针对地平线日光的白平衡设置。大致对应 5000 K 色温。)
    * - ``V4L2_WHITE_BALANCE_DAYLIGHT``
      - White balance preset for daylight (with clear sky). It corresponds
	approximately to 5000...6500 K color temperature.
	(针对日光（晴天）的白平衡预设。大致对应 5000...6500 K 色温。)
    * - ``V4L2_WHITE_BALANCE_FLASH``
      - With this setting the camera will compensate for the flash light.
	It slightly warms up the colors and corresponds roughly to
	5000...5500 K color temperature.
	(此设置下相机会补偿闪光灯。它使色彩略微偏暖，大致对应 5000...5500 K
	色温。)
    * - ``V4L2_WHITE_BALANCE_CLOUDY``
      - White balance preset for moderately overcast sky. This option
	corresponds approximately to 6500...8000 K color temperature
	range.
	(针对中度阴天的白平衡预设。该选项大致对应 6500...8000 K 色温
	范围。)
    * - ``V4L2_WHITE_BALANCE_SHADE``
      - White balance preset for shade or heavily overcast sky. It
	corresponds approximately to 9000...10000 K color temperature.
	(针对阴影或浓阴天的白平衡预设。大致对应 9000...10000 K 色温。)



.. _v4l2-wide-dynamic-range:

``V4L2_CID_WIDE_DYNAMIC_RANGE (boolean)``
    Enables or disables the camera's wide dynamic range feature. This
    feature allows to obtain clear images in situations where intensity
    of the illumination varies significantly throughout the scene, i.e.
    there are simultaneously very dark and very bright areas. It is most
    commonly realized in cameras by combining two subsequent frames with
    different exposure times.  [#f1]_
    (启用或禁用相机的宽动态范围特性。该特性使得在照明强度在整个场景中变化
    剧烈（即同时存在非常暗和非常亮的区域）的情况下也能获得清晰的图像。在
    相机中它最常通过合并两个不同曝光时间的相邻帧来实现。)

.. _v4l2-image-stabilization:

``V4L2_CID_IMAGE_STABILIZATION (boolean)``
    Enables or disables image stabilization.
    (启用或禁用图像防抖。)

``V4L2_CID_ISO_SENSITIVITY (integer menu)``
    Determines ISO equivalent of an image sensor indicating the sensor's
    sensitivity to light. The numbers are expressed in arithmetic scale,
    as per :ref:`iso12232` standard, where doubling the sensor
    sensitivity is represented by doubling the numerical ISO value.
    Applications should interpret the values as standard ISO values
    multiplied by 1000, e.g. control value 800 stands for ISO 0.8.
    Drivers will usually support only a subset of standard ISO values.
    The effect of setting this control while the
    ``V4L2_CID_ISO_SENSITIVITY_AUTO`` control is set to a value other
    than ``V4L2_CID_ISO_SENSITIVITY_MANUAL`` is undefined, drivers
    should ignore such requests.
    (确定图像传感器的 ISO 等价值，表示传感器对光的敏感度。数值按算术刻度
    表示（依照 :ref:`iso12232` 标准），传感器灵敏度加倍对应 ISO 数值加倍。
    应用程序应把取值解释为标准 ISO 值乘以 1000，例如控制值 800 表示
    ISO 0.8。驱动通常只支持标准 ISO 值的一个子集。当
    ``V4L2_CID_ISO_SENSITIVITY_AUTO`` 控制项设置为
    ``V4L2_CID_ISO_SENSITIVITY_MANUAL`` 以外的值时设置此控制项的效果未
    定义，驱动应忽略此类请求。)

.. _v4l2-iso-sensitivity-auto-type:

``V4L2_CID_ISO_SENSITIVITY_AUTO``
    (enum)

enum v4l2_iso_sensitivity_type -
    Enables or disables automatic ISO sensitivity adjustments.
    (启用或禁用自动 ISO 感光度调整。)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_CID_ISO_SENSITIVITY_MANUAL``
      - Manual ISO sensitivity.
	(手动 ISO 感光度。)
    * - ``V4L2_CID_ISO_SENSITIVITY_AUTO``
      - Automatic ISO sensitivity adjustments.
	(自动 ISO 感光度调整。)



.. _v4l2-scene-mode:

``V4L2_CID_SCENE_MODE``
    (enum)

enum v4l2_scene_mode -
    This control allows to select scene programs as the camera automatic
    modes optimized for common shooting scenes. Within these modes the
    camera determines best exposure, aperture, focusing, light metering,
    white balance and equivalent sensitivity. The controls of those
    parameters are influenced by the scene mode control. An exact
    behavior in each mode is subject to the camera specification.
    (此控制项允许选择场景程序，即针对常见拍摄场景优化的相机自动模式。在这些
    模式下，相机会确定最佳曝光、光圈、对焦、测光、白平衡和等效感光度。这些
    参数的控制项受场景模式控制项影响。每种模式的确切行为取决于相机规格。)

    When the scene mode feature is not used, this control should be set
    to ``V4L2_SCENE_MODE_NONE`` to make sure the other possibly related
    controls are accessible. The following scene programs are defined:
    (不使用场景模式特性时，应把此控制项设置为 ``V4L2_SCENE_MODE_NONE``，
    以确保其他可能相关的控制项可访问。定义了以下场景程序：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{5.9cm}|p{11.6cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_SCENE_MODE_NONE``
      - The scene mode feature is disabled.
	(场景模式特性被禁用。)
    * - ``V4L2_SCENE_MODE_BACKLIGHT``
      - Backlight. Compensates for dark shadows when light is coming from
	behind a subject, also by automatically turning on the flash.
	(逆光。当光线来自被摄主体背后时补偿暗阴影，也会自动打开闪光灯。)
    * - ``V4L2_SCENE_MODE_BEACH_SNOW``
      - Beach and snow. This mode compensates for all-white or bright
	scenes, which tend to look gray and low contrast, when camera's
	automatic exposure is based on an average scene brightness. To
	compensate, this mode automatically slightly overexposes the
	frames. The white balance may also be adjusted to compensate for
	the fact that reflected snow looks bluish rather than white.
	(海滩与雪景。当相机的自动曝光基于场景平均亮度时，全白或明亮场景往往
	显得灰暗且对比度低，此模式对此进行补偿。为补偿这一点，该模式会自动
	轻微过度曝光各帧。白平衡也可能被调整，以补偿反光的雪看起来偏蓝而非
	白色这一事实。)
    * - ``V4L2_SCENE_MODE_CANDLELIGHT``
      - Candle light. The camera generally raises the ISO sensitivity and
	lowers the shutter speed. This mode compensates for relatively
	close subject in the scene. The flash is disabled in order to
	preserve the ambiance of the light.
	(烛光。相机通常会提高 ISO 感光度并降低快门速度。此模式针对场景中相对
	较近的主体进行补偿。闪光灯被禁用，以保留光线的氛围。)
    * - ``V4L2_SCENE_MODE_DAWN_DUSK``
      - Dawn and dusk. Preserves the colors seen in low natural light
	before dusk and after down. The camera may turn off the flash, and
	automatically focus at infinity. It will usually boost saturation
	and lower the shutter speed.
	(黎明与黄昏。保留黄昏之前与之后低自然光下看到的色彩。相机可能会关闭
	闪光灯并自动对焦于无穷远。它通常会提高饱和度并降低快门速度。)
    * - ``V4L2_SCENE_MODE_FALL_COLORS``
      - Fall colors. Increases saturation and adjusts white balance for
	color enhancement. Pictures of autumn leaves get saturated reds
	and yellows.
	(秋色。提高饱和度并调整白平衡以增强色彩。秋叶照片会呈现饱和的红色与
	黄色。)
    * - ``V4L2_SCENE_MODE_FIREWORKS``
      - Fireworks. Long exposure times are used to capture the expanding
	burst of light from a firework. The camera may invoke image
	stabilization.
	(烟花。使用长时间曝光来捕捉烟花不断扩散的光芒。相机可能启用图像
	防抖。)
    * - ``V4L2_SCENE_MODE_LANDSCAPE``
      - Landscape. The camera may choose a small aperture to provide deep
	depth of field and long exposure duration to help capture detail
	in dim light conditions. The focus is fixed at infinity. Suitable
	for distant and wide scenery.
	(风景。相机可能选择小光圈以提供大景深，并使用较长的曝光时间来帮助在
	昏暗光线下捕捉细节。对焦固定于无穷远。适合遥远而宽广的景色。)
    * - ``V4L2_SCENE_MODE_NIGHT``
      - Night, also known as Night Landscape. Designed for low light
	conditions, it preserves detail in the dark areas without blowing
	out bright objects. The camera generally sets itself to a
	medium-to-high ISO sensitivity, with a relatively long exposure
	time, and turns flash off. As such, there will be increased image
	noise and the possibility of blurred image.
	(夜景，也称为夜间风景。为低光条件设计，在不过曝明亮物体的前提下保留
	暗区细节。相机通常把自身设置为中到高 ISO 感光度、相对较长的曝光时间，
	并关闭闪光灯。因此，图像噪点会增加，并且图像可能模糊。)
    * - ``V4L2_SCENE_MODE_PARTY_INDOOR``
      - Party and indoor. Designed to capture indoor scenes that are lit
	by indoor background lighting as well as the flash. The camera
	usually increases ISO sensitivity, and adjusts exposure for the
	low light conditions.
	(聚会与室内。为拍摄由室内背景照明与闪光灯共同照亮的室内场景而设计。
	相机通常会提高 ISO 感光度，并针对低光条件调整曝光。)
    * - ``V4L2_SCENE_MODE_PORTRAIT``
      - Portrait. The camera adjusts the aperture so that the depth of
	field is reduced, which helps to isolate the subject against a
	smooth background. Most cameras recognize the presence of faces in
	the scene and focus on them. The color hue is adjusted to enhance
	skin tones. The intensity of the flash is often reduced.
	(人像。相机调整光圈使景深变浅，这有助于把主体从平滑背景中分离出来。
	大多数相机能识别场景中的人脸并对焦于其上。色调被调整以增强肤色。
	闪光灯强度通常会降低。)
    * - ``V4L2_SCENE_MODE_SPORTS``
      - Sports. Significantly increases ISO and uses a fast shutter speed
	to freeze motion of rapidly-moving subjects. Increased image noise
	may be seen in this mode.
	(运动。显著提高 ISO 并使用高速快门来凝固快速运动主体的动作。此模式下
	图像噪点可能增加。)
    * - ``V4L2_SCENE_MODE_SUNSET``
      - Sunset. Preserves deep hues seen in sunsets and sunrises. It bumps
	up the saturation.
	(日落。保留日落与日出时看到的深色调。它会提升饱和度。)
    * - ``V4L2_SCENE_MODE_TEXT``
      - Text. It applies extra contrast and sharpness, it is typically a
	black-and-white mode optimized for readability. Automatic focus
	may be switched to close-up mode and this setting may also involve
	some lens-distortion correction.
	(文本。应用额外的对比度与锐度，通常是针对可读性优化的黑白模式。
	自动对焦可能切换到近摄模式，此设置还可能涉及一些镜头畸变校正。)

.. raw:: latex

    \normalsize


``V4L2_CID_3A_LOCK (bitmask)``
    This control locks or unlocks the automatic focus, exposure and
    white balance. The automatic adjustments can be paused independently
    by setting the corresponding lock bit to 1. The camera then retains
    the settings until the lock bit is cleared. The following lock bits
    are defined:
    (此控制项锁定或解锁自动对焦、曝光和白平衡。可以把对应的锁定位设置为 1
    来独立暂停自动调整。此后相机会保留这些设置，直到锁定位被清除。定义了
    以下锁定位：)

    When a given algorithm is not enabled, drivers should ignore
    requests to lock it and should return no error. An example might be
    an application setting bit ``V4L2_LOCK_WHITE_BALANCE`` when the
    ``V4L2_CID_AUTO_WHITE_BALANCE`` control is set to ``FALSE``. The
    value of this control may be changed by exposure, white balance or
    focus controls.
    (当某个算法未启用时，驱动应忽略锁定它的请求且不返回错误。例如应用程序
    在 ``V4L2_CID_AUTO_WHITE_BALANCE`` 控制项设置为 ``FALSE`` 时设置
    ``V4L2_LOCK_WHITE_BALANCE`` 位。此控制项的值可能被曝光、白平衡或对焦
    控制项改变。)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_LOCK_EXPOSURE``
      - Automatic exposure adjustments lock.
	(自动曝光调整锁定。)
    * - ``V4L2_LOCK_WHITE_BALANCE``
      - Automatic white balance adjustments lock.
	(自动白平衡调整锁定。)
    * - ``V4L2_LOCK_FOCUS``
      - Automatic focus lock.
	(自动对焦锁定。)



``V4L2_CID_PAN_SPEED (integer)``
    This control turns the camera horizontally at the specific speed.
    The unit is undefined. A positive value moves the camera to the
    right (clockwise when viewed from above), a negative value to the
    left. A value of zero stops the motion if one is in progress and has
    no effect otherwise.
    (此控制项使相机以指定速度水平转动。单位未定义。正值使相机向右移动
    （从上方看为顺时针），负值向左。值为零时如果运动正在进行则停止运动，
    否则无效果。)

``V4L2_CID_TILT_SPEED (integer)``
    This control turns the camera vertically at the specified speed. The
    unit is undefined. A positive value moves the camera up, a negative
    value down. A value of zero stops the motion if one is in progress
    and has no effect otherwise.
    (此控制项使相机以指定速度垂直转动。单位未定义。正值使相机向上移动，
    负值向下。值为零时如果运动正在进行则停止运动，否则无效果。)

.. _v4l2-camera-sensor-orientation:

``V4L2_CID_CAMERA_ORIENTATION (menu)``
    This read-only control describes the camera orientation by reporting its
    mounting position on the device where the camera is installed. The control
    value is constant and not modifiable by software. This control is
    particularly meaningful for devices which have a well defined orientation,
    such as phones, laptops and portable devices since the control is expressed
    as a position relative to the device's intended usage orientation. For
    example, a camera installed on the user-facing side of a phone, a tablet or
    a laptop device is said to be have ``V4L2_CAMERA_ORIENTATION_FRONT``
    orientation, while a camera installed on the opposite side of the front one
    is said to be have ``V4L2_CAMERA_ORIENTATION_BACK`` orientation. Camera
    sensors not directly attached to the device, or attached in a way that
    allows them to move freely, such as webcams and digital cameras, are said to
    have the ``V4L2_CAMERA_ORIENTATION_EXTERNAL`` orientation.
    (此只读控制项通过报告相机在安装设备上的安装位置来描述相机朝向。控制项值
    是常量，不能被软件修改。此控制项对具有明确朝向的设备尤其有意义，例如
    手机、笔记本电脑和便携设备，因为该控制项以相对于设备预期使用朝向的
    位置来表示。例如，安装在手机、平板或笔记本电脑面向用户一侧的相机称为
    具有 ``V4L2_CAMERA_ORIENTATION_FRONT`` 朝向，而安装在与前侧相对一侧的
    相机称为具有 ``V4L2_CAMERA_ORIENTATION_BACK`` 朝向。未直接附着在设备上、
    或以可自由移动的方式附着的相机传感器（例如网络摄像头和数码相机）称为
    具有 ``V4L2_CAMERA_ORIENTATION_EXTERNAL`` 朝向。)


.. tabularcolumns:: |p{7.7cm}|p{9.8cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_CAMERA_ORIENTATION_FRONT``
      - The camera is oriented towards the user facing side of the device.
	(相机朝向设备面向用户的一侧。)
    * - ``V4L2_CAMERA_ORIENTATION_BACK``
      - The camera is oriented towards the back facing side of the device.
	(相机朝向设备的背面。)
    * - ``V4L2_CAMERA_ORIENTATION_EXTERNAL``
      - The camera is not directly attached to the device and is freely movable.
	(相机未直接附着在设备上，可以自由移动。)


.. _v4l2-camera-sensor-rotation:

``V4L2_CID_CAMERA_SENSOR_ROTATION (integer)``
    This read-only control describes the rotation correction in degrees in the
    counter-clockwise direction to be applied to the captured images once
    captured to memory to compensate for the camera sensor mounting rotation.
    (此只读控制项描述对捕获到内存的图像施加的逆时针方向旋转校正角度（以度
    为单位），用于补偿相机传感器的安装旋转。)

    For a precise definition of the sensor mounting rotation refer to the
    extensive description of the 'rotation' properties in the device tree
    bindings file 'video-interfaces.txt'.
    (关于传感器安装旋转的精确定义，请参阅设备树绑定文件
    'video-interfaces.txt' 中对 'rotation' 属性的详细描述。)

    A few examples are below reported, using a shark swimming from left to
    right in front of the user as the example scene to capture. ::

                 0               X-axis
               0 +------------------------------------->
                 !
                 !
                 !
                 !           |\____)\___
                 !           ) _____  __`<
                 !           |/     )/
                 !
                 !
                 !
                 V
               Y-axis

    (下面给出几个例子，以一条在用户面前从左向右游动的鲨鱼作为要捕捉的示例
    场景。)

    Example one - Webcam

    Assuming you can bring your laptop with you while swimming with sharks,
    the camera module of the laptop is installed on the user facing part of a
    laptop screen casing, and is typically used for video calls. The captured
    images are meant to be displayed in landscape mode (width > height) on the
    laptop screen.
    (例子一——网络摄像头。假设你可以带着笔记本电脑去和鲨鱼一起游泳：笔记本
    电脑的摄像头模块安装在笔记本屏幕外壳面向用户的一侧，通常用于视频通话。
    捕获的图像打算在笔记本屏幕上以横向模式（宽 > 高）显示。)

    The camera is typically mounted upside-down to compensate the lens optical
    inversion effect. In this case the value of the
    V4L2_CID_CAMERA_SENSOR_ROTATION control is 0, no rotation is required to
    display images correctly to the user.
    (相机通常倒置安装，以补偿镜头的光学反转效应。在这种情况下，
    V4L2_CID_CAMERA_SENSOR_ROTATION 控制项的值为 0，无需旋转即可向用户正确
    显示图像。)

    If the camera sensor is not mounted upside-down it is required to compensate
    the lens optical inversion effect and the value of the
    V4L2_CID_CAMERA_SENSOR_ROTATION control is 180 degrees, as images will
    result rotated when captured to memory. ::

                 +--------------------------------------+
                 !                                      !
                 !                                      !
                 !                                      !
                 !              __/(_____/|             !
                 !            >.___  ____ (             !
                 !                 \(    \|             !
                 !                                      !
                 !                                      !
                 !                                      !
                 +--------------------------------------+

    (如果相机传感器没有倒置安装，则需要补偿镜头的光学反转效应，
    V4L2_CID_CAMERA_SENSOR_ROTATION 控制项的值为 180 度，因为图像捕获到内存
    时会是旋转的。)

    A software rotation correction of 180 degrees has to be applied to correctly
    display the image on the user screen. ::

                 +--------------------------------------+
                 !                                      !
                 !                                      !
                 !                                      !
                 !             |\____)\___              !
                 !             ) _____  __`<            !
                 !             |/     )/                !
                 !                                      !
                 !                                      !
                 !                                      !
                 +--------------------------------------+

    (需要在软件上施加 180 度旋转校正，才能在用户屏幕上正确显示该图像。)

    Example two - Phone camera

    It is more handy to go and swim with sharks with only your mobile phone
    with you and take pictures with the camera that is installed on the back
    side of the device, facing away from the user. The captured images are meant
    to be displayed in portrait mode (height > width) to match the device screen
    orientation and the device usage orientation used when taking the picture.
    (例子二——手机相机。只带手机去和鲨鱼游泳、并用安装在设备背面（背对
    用户）的相机拍照会更方便。捕获的图像打算以纵向模式（高 > 宽）显示，以
    匹配设备屏幕朝向以及拍照时设备的使用朝向。)

    The camera sensor is typically mounted with its pixel array longer side
    aligned to the device longer side, upside-down mounted to compensate for
    the lens optical inversion effect.
    (相机传感器通常以像素阵列长边与设备长边对齐的方式安装，并倒置安装以
    补偿镜头的光学反转效应。)

    The images once captured to memory will be rotated and the value of the
    V4L2_CID_CAMERA_SENSOR_ROTATION will report a 90 degree rotation. ::


                 +-------------------------------------+
                 |                 _ _                 |
                 |                \   /                |
                 |                 | |                 |
                 |                 | |                 |
                 |                 |  >                |
                 |                <  |                 |
                 |                 | |                 |
                 |                   .                 |
                 |                  V                  |
                 +-------------------------------------+

    A correction of 90 degrees in counter-clockwise direction has to be
    applied to correctly display the image in portrait mode on the device
    screen. ::

                          +--------------------+
                          |                    |
                          |                    |
                          |                    |
                          |                    |
                          |                    |
                          |                    |
                          |   |\____)\___      |
                          |   ) _____  __`<    |
                          |   |/     )/        |
                          |                    |
                          |                    |
                          |                    |
                          |                    |
                          |                    |
                          +--------------------+

    (需要施加 90 度逆时针方向的校正，才能在设备屏幕上以纵向模式正确显示该
    图像。)


.. [#f1]
   This control may be changed to a menu control in the future, if more
   options are required.
   (如果将来需要更多选项，此控制项可能会改为菜单 (menu) 控制项。)

``V4L2_CID_HDR_SENSOR_MODE (menu)``
    Change the sensor HDR mode. A HDR picture is obtained by merging two
    captures of the same scene using two different exposure periods. HDR mode
    describes the way these two captures are merged in the sensor.
    (更改传感器 HDR 模式。HDR 图片通过使用两个不同曝光周期合并同一场景的两次
    捕获得到。HDR 模式描述这两次捕获在传感器中的合并方式。)

    As modes differ for each sensor, menu items are not standardized by this
    control and are left to the programmer.
    (由于各传感器的模式不同，此控制项不标准化菜单项，而由程序员自行决定。)
