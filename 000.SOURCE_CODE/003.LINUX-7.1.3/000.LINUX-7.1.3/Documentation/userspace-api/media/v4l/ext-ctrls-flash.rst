.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _flash-controls:

***************************************
Flash Control Reference(闪光灯控制参考)
***************************************

The V4L2 flash controls are intended to provide generic access to flash
controller devices. Flash controller devices are typically used in
digital cameras.
(V4L2 闪光灯控制旨在提供对闪光灯控制器设备的通用访问。闪光灯控制器设备通常用于数码相机。)

The interface can support both LED and xenon flash devices. As of
writing this, there is no xenon flash driver using this interface.
(该接口可以同时支持 LED 与氙气闪光灯设备。撰写本文时，尚无使用该接口的氙气闪光灯驱动。)


.. _flash-controls-use-cases:

Supported use cases(支持的用例)
===============================


Unsynchronised LED flash (software strobe)(非同步 LED 闪光灯 (软件频闪))
------------------------------------------------------------------------

Unsynchronised LED flash is controlled directly by the host as the
sensor. The flash must be enabled by the host before the exposure of the
image starts and disabled once it ends. The host is fully responsible
for the timing of the flash.
(非同步 LED 闪光灯由宿主直接控制 (如同传感器)。闪光灯必须在图像曝光开始之前由宿主启用，并在曝光结束后禁用。闪光时序完全由宿主负责。)

Example of such device: Nokia N900.
(此类设备的例子：Nokia N900。)


Synchronised LED flash (hardware strobe)(同步 LED 闪光灯 (硬件频闪))
--------------------------------------------------------------------

The synchronised LED flash is pre-programmed by the host (power and
timeout) but controlled by the sensor through a strobe signal from the
sensor to the flash.
(同步 LED 闪光灯由宿主预先编程 (功率与超时)，但由传感器通过从传感器到闪光灯的频闪信号进行控制。)

The sensor controls the flash duration and timing. This information
typically must be made available to the sensor.
(传感器控制闪光持续时间与时序。这一信息通常必须提供给传感器。)


LED flash as torch(用作手电筒的 LED 闪光灯)
-------------------------------------------

LED flash may be used as torch in conjunction with another use case
involving camera or individually.
(LED 闪光灯可以用作手电筒，可与另一个涉及相机的用例配合使用，也可以单独使用。)


.. _flash-control-id:

Flash Control IDs(闪光灯控制 ID)
--------------------------------

``V4L2_CID_FLASH_CLASS (class)``
    The FLASH class descriptor.
    (FLASH 类描述符。)

.. _v4l2-cid-flash-led-mode:

``V4L2_CID_FLASH_LED_MODE (menu)``
    Defines the mode of the flash LED, the high-power white LED attached
    to the flash controller. Setting this control may not be possible in
    presence of some faults. See V4L2_CID_FLASH_FAULT.
    (定义闪光灯 LED (即挂接在闪光灯控制器上的高功率白光 LED) 的模式。在某些故障存在时可能无法设置该控制。见 V4L2_CID_FLASH_FAULT。)


.. tabularcolumns:: |p{5.7cm}|p{11.8cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_FLASH_LED_MODE_NONE``
      - Off.(关闭。)
    * - ``V4L2_FLASH_LED_MODE_FLASH``
      - Flash mode.(闪光模式。)
    * - ``V4L2_FLASH_LED_MODE_TORCH``
      - Torch mode.(手电筒模式。)

        See V4L2_CID_FLASH_TORCH_INTENSITY.
        (见 V4L2_CID_FLASH_TORCH_INTENSITY。)



.. _v4l2-cid-flash-strobe-source:

``V4L2_CID_FLASH_STROBE_SOURCE (menu)``
    Defines the source of the flash LED strobe.
    (定义闪光灯 LED 频闪的来源。)

.. tabularcolumns:: |p{7.5cm}|p{7.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_FLASH_STROBE_SOURCE_SOFTWARE``
      - The flash strobe is triggered by using the
	V4L2_CID_FLASH_STROBE control.
        (闪光灯频闪通过使用 V4L2_CID_FLASH_STROBE 控制来触发。)
    * - ``V4L2_FLASH_STROBE_SOURCE_EXTERNAL``
      - The flash strobe is triggered by an external source. Typically
	this is a sensor, which makes it possible to synchronise the
	flash strobe start to exposure start.
        This method of controlling flash LED strobe has two additional
        prerequisites: the strobe source's :ref:`strobe output
        <v4l2-cid-flash-strobe-oe>` must be enabled (if available)
        and the flash controller's :ref:`flash LED mode
        <v4l2-cid-flash-led-mode>` must be set to
        ``V4L2_FLASH_LED_MODE_FLASH``.
        (闪光灯频闪由外部源触发。通常这是一个传感器，这使得可以把频闪开始同步到曝光开始。
        这种控制闪光灯 LED 频闪的方法有两个额外前提条件：频闪源的 :ref:`strobe output
        <v4l2-cid-flash-strobe-oe>` 必须启用 (如可用)，并且闪光灯控制器的 :ref:`flash LED mode
        <v4l2-cid-flash-led-mode>` 必须设置为
        ``V4L2_FLASH_LED_MODE_FLASH`` 。)



``V4L2_CID_FLASH_STROBE (button)``
    Strobe flash. Valid when V4L2_CID_FLASH_LED_MODE is set to
    V4L2_FLASH_LED_MODE_FLASH and V4L2_CID_FLASH_STROBE_SOURCE
    is set to V4L2_FLASH_STROBE_SOURCE_SOFTWARE. Setting this
    control may not be possible in presence of some faults. See
    V4L2_CID_FLASH_FAULT.
    (触发闪光灯频闪。仅在 V4L2_CID_FLASH_LED_MODE 设为 V4L2_FLASH_LED_MODE_FLASH 且 V4L2_CID_FLASH_STROBE_SOURCE 设为 V4L2_FLASH_STROBE_SOURCE_SOFTWARE 时有效。在某些故障存在时可能无法设置该控制。见 V4L2_CID_FLASH_FAULT。)

``V4L2_CID_FLASH_STROBE_STOP (button)``
    Stop flash strobe immediately.
    (立即停止闪光灯频闪。)

``V4L2_CID_FLASH_STROBE_STATUS (boolean)``
    Strobe status: whether the flash is strobing at the moment or not.
    This is a read-only control.
    (频闪状态：闪光灯此刻是否正在频闪。这是一个只读控制。)

``V4L2_CID_FLASH_TIMEOUT (integer)``
    Hardware timeout for flash. The flash strobe is stopped after this
    period of time has passed from the start of the strobe.
    (闪光灯硬件超时。从频闪开始经过这段时间后，闪光灯频闪将被停止。)

``V4L2_CID_FLASH_INTENSITY (integer)``
    Intensity of the flash strobe when the flash LED is in flash mode
    (V4L2_FLASH_LED_MODE_FLASH). The unit should be milliamps (mA)
    if possible.
    (闪光灯 LED 处于闪光模式 (V4L2_FLASH_LED_MODE_FLASH) 时频闪的强度。如可能，单位应为毫安 (mA)。)

``V4L2_CID_FLASH_TORCH_INTENSITY (integer)``
    Intensity of the flash LED in torch mode
    (V4L2_FLASH_LED_MODE_TORCH). The unit should be milliamps (mA)
    if possible. Setting this control may not be possible in presence of
    some faults. See V4L2_CID_FLASH_FAULT.
    (手电筒模式 (V4L2_FLASH_LED_MODE_TORCH) 下闪光灯 LED 的强度。如可能，单位应为毫安 (mA)。在某些故障存在时可能无法设置该控制。见 V4L2_CID_FLASH_FAULT。)

``V4L2_CID_FLASH_INDICATOR_INTENSITY (integer)``
    Intensity of the indicator LED. The indicator LED may be fully
    independent of the flash LED. The unit should be microamps (uA) if
    possible.
    (指示灯 LED 的强度。指示灯 LED 可以完全独立于闪光灯 LED。如可能，单位应为微安 (uA)。)

``V4L2_CID_FLASH_FAULT (bitmask)``
    Faults related to the flash. The faults tell about specific problems
    in the flash chip itself or the LEDs attached to it. Faults may
    prevent further use of some of the flash controls. In particular,
    V4L2_CID_FLASH_LED_MODE is set to V4L2_FLASH_LED_MODE_NONE
    if the fault affects the flash LED. Exactly which faults have such
    an effect is chip dependent. Reading the faults resets the control
    and returns the chip to a usable state if possible.
    (与闪光灯相关的故障。这些故障说明闪光灯芯片自身或挂接其上的 LED 存在特定问题。故障可能阻止继续使用某些闪光灯控制。特别是，如果故障影响闪光灯 LED，V4L2_CID_FLASH_LED_MODE 会被设为 V4L2_FLASH_LED_MODE_NONE。具体哪些故障有这样的效果取决于芯片。读取故障会复位该控制，并尽可能把芯片恢复到可用状态。)

.. tabularcolumns:: |p{8.4cm}|p{9.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_FLASH_FAULT_OVER_VOLTAGE``
      - Flash controller voltage to the flash LED has exceeded the limit
	specific to the flash controller.
        (闪光灯控制器输出给闪光灯 LED 的电压已超过该闪光灯控制器特定的上限。)
    * - ``V4L2_FLASH_FAULT_TIMEOUT``
      - The flash strobe was still on when the timeout set by the user ---
	V4L2_CID_FLASH_TIMEOUT control --- has expired. Not all flash
	controllers may set this in all such conditions.
        (当用户设置的超时——V4L2_CID_FLASH_TIMEOUT 控制——到期时，闪光灯频闪仍在进行。并非所有闪光灯控制器都会在所有此类条件下设置此标志。)
    * - ``V4L2_FLASH_FAULT_OVER_TEMPERATURE``
      - The flash controller has overheated.
        (闪光灯控制器已过热。)
    * - ``V4L2_FLASH_FAULT_SHORT_CIRCUIT``
      - The short circuit protection of the flash controller has been
	triggered.
        (闪光灯控制器的短路保护已被触发。)
    * - ``V4L2_FLASH_FAULT_OVER_CURRENT``
      - Current in the LED power supply has exceeded the limit specific to
	the flash controller.
        (LED 电源中的电流已超过该闪光灯控制器特定的上限。)
    * - ``V4L2_FLASH_FAULT_INDICATOR``
      - The flash controller has detected a short or open circuit
	condition on the indicator LED.
        (闪光灯控制器在指示灯 LED 上检测到短路或开路情况。)
    * - ``V4L2_FLASH_FAULT_UNDER_VOLTAGE``
      - Flash controller voltage to the flash LED has been below the
	minimum limit specific to the flash controller.
        (闪光灯控制器输出给闪光灯 LED 的电压低于该闪光灯控制器特定的下限。)
    * - ``V4L2_FLASH_FAULT_INPUT_VOLTAGE``
      - The input voltage of the flash controller is below the limit under
	which strobing the flash at full current will not be possible.The
	condition persists until this flag is no longer set.
        (闪光灯控制器的输入电压低于某个限值，低于该限值时将无法以全电流频闪闪光灯。该状况持续到该标志不再置位为止。)
    * - ``V4L2_FLASH_FAULT_LED_OVER_TEMPERATURE``
      - The temperature of the LED has exceeded its allowed upper limit.
        (LED 的温度已超过其允许的上限。)



``V4L2_CID_FLASH_CHARGE (boolean)``
    Enable or disable charging of the xenon flash capacitor.
    (启用或禁用氙气闪光灯电容的充电。)

``V4L2_CID_FLASH_READY (boolean)``
    Is the flash ready to strobe? Xenon flashes require their capacitors
    charged before strobing. LED flashes often require a cooldown period
    after strobe during which another strobe will not be possible. This
    is a read-only control.
    (闪光灯是否准备好频闪？氙气闪光灯在频闪前需要给电容充电。LED 闪光灯在频闪后常需要一段冷却期，期间无法再次频闪。这是一个只读控制。)

.. _v4l2-cid-flash-duration:

``V4L2_CID_FLASH_DURATION (integer)``
    Duration of the flash strobe pulse generated by the strobe source, when
    using external strobe. This control shall be implemented by the device
    generating the hardware flash strobe signal, typically a camera sensor,
    connected to a flash controller.
    (使用外部频闪时，频闪源生成的闪光灯频闪脉冲的持续时间。该控制应由生成硬件闪光灯频闪信号的设备 (通常是连接到闪光灯控制器的相机传感器) 实现。)

    The flash controllers :ref:`strobe source <v4l2-cid-flash-strobe-source>`
    must be configured to ``V4L2_FLASH_STROBE_SOURCE_EXTERNAL`` for this
    mode of operation. For more details please also take a look at the
    documentation there.
    (在这种工作模式下，闪光灯控制器的 :ref:`strobe source <v4l2-cid-flash-strobe-source>`
    必须配置为 ``V4L2_FLASH_STROBE_SOURCE_EXTERNAL`` 。更多细节也请参阅那里的文档。)

    The unit should be microseconds (µs) if possible.
    (如可能，单位应为微秒 (µs)。)

.. _v4l2-cid-flash-strobe-oe:

``V4L2_CID_FLASH_STROBE_OE (boolean)``
    Enables the output of a hardware strobe signal from the strobe source,
    when using external strobe. This control shall be implemented by the device
    generating the hardware flash strobe signal, typically a camera sensor,
    connected to a flash controller.
    (使用外部频闪时，启用频闪源输出硬件频闪信号。该控制应由生成硬件闪光灯频闪信号的设备 (通常是连接到闪光灯控制器的相机传感器) 实现。)

    Provided the signal generating device driver supports it, the length of the
    strobe signal can be configured by adjusting its
    :ref:`flash duration <v4l2-cid-flash-duration>`.
    (前提是信号生成设备驱动支持，可以通过调整其
    :ref:`flash duration <v4l2-cid-flash-duration>` 来配置频闪信号的长度。)

    The flash controllers :ref:`strobe source <v4l2-cid-flash-strobe-source>`
    must be configured to ``V4L2_FLASH_STROBE_SOURCE_EXTERNAL`` for this
    mode of operation. For more details please also take a look at the
    documentation there.
    (在这种工作模式下，闪光灯控制器的 :ref:`strobe source <v4l2-cid-flash-strobe-source>`
    必须配置为 ``V4L2_FLASH_STROBE_SOURCE_EXTERNAL`` 。更多细节也请参阅那里的文档。)
