.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _CEC_ADAP_LOG_ADDRS:
.. _CEC_ADAP_G_LOG_ADDRS:
.. _CEC_ADAP_S_LOG_ADDRS:

********************************************************************************
ioctls CEC_ADAP_G_LOG_ADDRS and CEC_ADAP_S_LOG_ADDRS(获取或设置逻辑地址的 ioctl)
********************************************************************************

Name(名称)
==========

CEC_ADAP_G_LOG_ADDRS, CEC_ADAP_S_LOG_ADDRS - Get or set the logical addresses
(CEC_ADAP_G_LOG_ADDRS、CEC_ADAP_S_LOG_ADDRS - 获取或设置逻辑地址)

Synopsis(概要)
==============

.. c:macro:: CEC_ADAP_G_LOG_ADDRS

``int ioctl(int fd, CEC_ADAP_G_LOG_ADDRS, struct cec_log_addrs *argp)``

.. c:macro:: CEC_ADAP_S_LOG_ADDRS

``int ioctl(int fd, CEC_ADAP_S_LOG_ADDRS, struct cec_log_addrs *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`cec_log_addrs`.
    (指向 struct :c:type:`cec_log_addrs` 的指针。)

Description(描述)
=================

To query the current CEC logical addresses, applications call
:ref:`ioctl CEC_ADAP_G_LOG_ADDRS <CEC_ADAP_G_LOG_ADDRS>` with a pointer to a
struct :c:type:`cec_log_addrs` where the driver stores the logical addresses.
(要查询当前的 CEC 逻辑地址，应用程序调用
:ref:`ioctl CEC_ADAP_G_LOG_ADDRS <CEC_ADAP_G_LOG_ADDRS>` ，并传入一个指
向 struct :c:type:`cec_log_addrs` 的指针，驱动会把逻辑地址存储在其中。)

To set new logical addresses, applications fill in
struct :c:type:`cec_log_addrs` and call :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
with a pointer to this struct. The :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
is only available if ``CEC_CAP_LOG_ADDRS`` is set (the ``ENOTTY`` error code is
returned otherwise). The :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
can only be called by a file descriptor in initiator mode (see :ref:`CEC_S_MODE`), if not
the ``EBUSY`` error code will be returned.
(要设置新的逻辑地址，应用程序填写 struct :c:type:`cec_log_addrs` ，然
后调用 :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` 并传入指
向该结构的指针。:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
只有在设置了 ``CEC_CAP_LOG_ADDRS`` 时才可用 (否则返回 ``ENOTTY`` 错误
码)。:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` 只能由处
于 initiator 模式的文件描述符调用 (见 :ref:`CEC_S_MODE`)，否则将返回
``EBUSY`` 错误码。)

To clear existing logical addresses set ``num_log_addrs`` to 0. All other fields
will be ignored in that case. The adapter will go to the unconfigured state and the
``cec_version``, ``vendor_id`` and ``osd_name`` fields are all reset to their default
values (CEC version 2.0, no vendor ID and an empty OSD name).
(要清除既有的逻辑地址，把 ``num_log_addrs`` 设为 0。这种情况下其余所有
字段都会被忽略。适配器将进入未配置状态，``cec_version`` 、
``vendor_id`` 和 ``osd_name`` 字段全部重置为其默认值 (CEC 2.0 版本、无
厂商 ID、空的 OSD 名称)。)

If the physical address is valid (see :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>`),
then this ioctl will block until all requested logical
addresses have been claimed. If the file descriptor is in non-blocking mode then it will
not wait for the logical addresses to be claimed, instead it just returns 0.
(如果物理地址有效 (见 :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` )，
则此 ioctl 会阻塞，直到所有请求的逻辑地址都被认领。如果文件描述符处于
非阻塞模式，则它不会等待逻辑地址被认领，而是直接返回 0。)

A :ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>` event is sent when the
logical addresses are claimed or cleared.
(当逻辑地址被认领或被清除时，会发送
:ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>` 事件。)

Attempting to call :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` when
logical address types are already defined will return with error ``EBUSY``.
(当逻辑地址类型已经定义时再尝试调用
:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` 将返回错误
``EBUSY``。)

.. c:type:: cec_log_addrs

.. tabularcolumns:: |p{1.0cm}|p{8.0cm}|p{8.0cm}|

.. cssclass:: longtable

.. flat-table:: struct cec_log_addrs
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 16

    * - __u8
      - ``log_addr[CEC_MAX_LOG_ADDRS]``
      - The actual logical addresses that were claimed. This is set by the
	driver. If no logical address could be claimed, then it is set to
	``CEC_LOG_ADDR_INVALID``. If this adapter is Unregistered, then
	``log_addr[0]`` is set to 0xf and all others to
	``CEC_LOG_ADDR_INVALID``.
	(已被认领的实际逻辑地址。由驱动设置。如果无法认领任何逻辑地址，
	则被设置为 CEC_LOG_ADDR_INVALID。如果本适配器为 Unregistered，
	则 log_addr[0] 被设为 0xf，其余全部为 CEC_LOG_ADDR_INVALID。)
    * - __u16
      - ``log_addr_mask``
      - The bitmask of all logical addresses this adapter has claimed. If
	this adapter is Unregistered then ``log_addr_mask`` sets bit 15
	and clears all other bits. If this adapter is not configured at
	all, then ``log_addr_mask`` is set to 0. Set by the driver.
	(本适配器已认领的全部逻辑地址的位掩码。如果本适配器为
	Unregistered，则 log_addr_mask 置位第 15 位并清除其余各位。如果本
	适配器完全未配置，则 log_addr_mask 被设为 0。由驱动设置。)
    * - __u8
      - ``cec_version``
      - The CEC version that this adapter shall use. See
	:ref:`cec-versions`. Used to implement the
	``CEC_MSG_CEC_VERSION`` and ``CEC_MSG_REPORT_FEATURES`` messages.
	Note that :ref:`CEC_OP_CEC_VERSION_1_3A <CEC-OP-CEC-VERSION-1-3A>` is not allowed by the CEC
	framework.
	(本适配器应使用的 CEC 版本。见 :ref:`cec-versions` 。用于实现
	CEC_MSG_CEC_VERSION 和 CEC_MSG_REPORT_FEATURES 消息。注意，CEC 框
	架不允许 :ref:`CEC_OP_CEC_VERSION_1_3A <CEC-OP-CEC-VERSION-1-3A>` 。)
    * - __u8
      - ``num_log_addrs``
      - Number of logical addresses to set up. Must be ≤
	``available_log_addrs`` as returned by
	:ref:`CEC_ADAP_G_CAPS`. All arrays in
	this structure are only filled up to index
	``available_log_addrs``-1. The remaining array elements will be
	ignored. Note that the CEC 2.0 standard allows for a maximum of 2
	logical addresses, although some hardware has support for more.
	``CEC_MAX_LOG_ADDRS`` is 4. The driver will return the actual
	number of logical addresses it could claim, which may be less than
	what was requested. If this field is set to 0, then the CEC
	adapter shall clear all claimed logical addresses and all other
	fields will be ignored.
	(要设置的逻辑地址数量。必须 ≤ :ref:`CEC_ADAP_G_CAPS` 返回的
	available_log_addrs。本结构中的所有数组都只填充到下标
	available_log_addrs-1 为止，其余数组元素将被忽略。注意，CEC 2.0
	标准最多允许 2 个逻辑地址，尽管有些硬件支持更多。
	CEC_MAX_LOG_ADDRS 为 4。驱动将返回它实际能够认领的逻辑地址数量，
	可能少于请求的数量。如果此字段设为 0，则 CEC 适配器应清除所有已
	认领的逻辑地址，且其余各字段都将被忽略。)
    * - __u32
      - ``vendor_id``
      - The vendor ID is a 24-bit number that identifies the specific
	vendor or entity. Based on this ID vendor specific commands may be
	defined. If you do not want a vendor ID then set it to
	``CEC_VENDOR_ID_NONE``.
	(厂商 ID 是一个 24 位数字，标识特定的厂商或实体。基于此 ID 可以
	定义厂商专属的命令。如果不想使用厂商 ID，则将其设为
	CEC_VENDOR_ID_NONE。)
    * - __u32
      - ``flags``
      - Flags. See :ref:`cec-log-addrs-flags` for a list of available flags.
	(标志。可用标志列表见 :ref:`cec-log-addrs-flags` 。)
    * - char
      - ``osd_name[15]``
      - The On-Screen Display name as is returned by the
	``CEC_MSG_SET_OSD_NAME`` message.
	(屏幕显示 (On-Screen Display) 名称，由 CEC_MSG_SET_OSD_NAME 消息
	返回。)
    * - __u8
      - ``primary_device_type[CEC_MAX_LOG_ADDRS]``
      - Primary device type for each logical address. See
	:ref:`cec-prim-dev-types` for possible types.
	(每个逻辑地址的主设备类型。可能的类型见
	:ref:`cec-prim-dev-types` 。)
    * - __u8
      - ``log_addr_type[CEC_MAX_LOG_ADDRS]``
      - Logical address types. See :ref:`cec-log-addr-types` for
	possible types. The driver will update this with the actual
	logical address type that it claimed (e.g. it may have to fallback
	to :ref:`CEC_LOG_ADDR_TYPE_UNREGISTERED <CEC-LOG-ADDR-TYPE-UNREGISTERED>`).
	(逻辑地址类型。可能的类型见 :ref:`cec-log-addr-types` 。驱动会用
	它实际认领的逻辑地址类型更新此字段 (例如它可能不得不回退到
	:ref:`CEC_LOG_ADDR_TYPE_UNREGISTERED <CEC-LOG-ADDR-TYPE-UNREGISTERED>` )。)
    * - __u8
      - ``all_device_types[CEC_MAX_LOG_ADDRS]``
      - CEC 2.0 specific: the bit mask of all device types. See
	:ref:`cec-all-dev-types-flags`. It is used in the CEC 2.0
	``CEC_MSG_REPORT_FEATURES`` message. For CEC 1.4 you can either leave
	this field to 0, or fill it in according to the CEC 2.0 guidelines to
	give the CEC framework more information about the device type, even
	though the framework won't use it directly in the CEC message.
	(CEC 2.0 专属：全部设备类型的位掩码。见
	:ref:`cec-all-dev-types-flags` 。用于 CEC 2.0 的
	CEC_MSG_REPORT_FEATURES 消息。对于 CEC 1.4，你可以把该字段保持为
	0，或按照 CEC 2.0 指南填写，以向 CEC 框架提供更多设备类型信息，
	尽管框架不会在 CEC 消息中直接使用它。)
    * - __u8
      - ``features[CEC_MAX_LOG_ADDRS][12]``
      - Features for each logical address. It is used in the CEC 2.0
	``CEC_MSG_REPORT_FEATURES`` message. The 12 bytes include both the
	RC Profile and the Device Features. For CEC 1.4 you can either leave
        this field to all 0, or fill it in according to the CEC 2.0 guidelines to
        give the CEC framework more information about the device type, even
        though the framework won't use it directly in the CEC message.
	(每个逻辑地址的特性。用于 CEC 2.0 的 CEC_MSG_REPORT_FEATURES 消
	息。这 12 个字节同时包含 RC Profile 和设备特性。对于 CEC 1.4，你
	可以把该字段全部保持为 0，或按照 CEC 2.0 指南填写，以向 CEC 框架
	提供更多设备类型信息，尽管框架不会在 CEC 消息中直接使用它。)

.. tabularcolumns:: |p{7.8cm}|p{1.0cm}|p{8.5cm}|

.. _cec-log-addrs-flags:

.. flat-table:: Flags for struct cec_log_addrs
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * .. _`CEC-LOG-ADDRS-FL-ALLOW-UNREG-FALLBACK`:

      - ``CEC_LOG_ADDRS_FL_ALLOW_UNREG_FALLBACK``
      - 1
      - By default if no logical address of the requested type can be claimed, then
	it will go back to the unconfigured state. If this flag is set, then it will
	fallback to the Unregistered logical address. Note that if the Unregistered
	logical address was explicitly requested, then this flag has no effect.
	(默认情况下，如果无法认领所请求类型的逻辑地址，则会回到未配置状
	态。如果设置了此标志，则会回退到 Unregistered 逻辑地址。注意，如
	果显式请求了 Unregistered 逻辑地址，则此标志不起作用。)
    * .. _`CEC-LOG-ADDRS-FL-ALLOW-RC-PASSTHRU`:

      - ``CEC_LOG_ADDRS_FL_ALLOW_RC_PASSTHRU``
      - 2
      - By default the ``CEC_MSG_USER_CONTROL_PRESSED`` and ``CEC_MSG_USER_CONTROL_RELEASED``
        messages are only passed on to the follower(s), if any. If this flag is set,
	then these messages are also passed on to the remote control input subsystem
	and will appear as keystrokes. This features needs to be enabled explicitly.
	If CEC is used to enter e.g. passwords, then you may not want to enable this
	to avoid trivial snooping of the keystrokes.
	(默认情况下，CEC_MSG_USER_CONTROL_PRESSED 和
	CEC_MSG_USER_CONTROL_RELEASED 消息只传递给跟随者 (如果有)。如果设
	置了此标志，则这些消息还会传递给遥控输入子系统，并表现为按键。此
	功能需要显式启用。如果 CEC 被用来输入例如密码，那么你可能不想启用
	它，以避免按键被轻易窥探。)
    * .. _`CEC-LOG-ADDRS-FL-CDC-ONLY`:

      - ``CEC_LOG_ADDRS_FL_CDC_ONLY``
      - 4
      - If this flag is set, then the device is CDC-Only. CDC-Only CEC devices
	are CEC devices that can only handle CDC messages.
	(如果设置了此标志，则设备为 CDC-Only。CDC-Only CEC 设备是只能处
	理 CDC 消息的 CEC 设备。)

	All other messages are ignored.
	(所有其他消息都会被忽略。)

.. tabularcolumns:: |p{7.8cm}|p{1.0cm}|p{8.5cm}|

.. _cec-versions:

.. flat-table:: CEC Versions
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * .. _`CEC-OP-CEC-VERSION-1-3A`:

      - ``CEC_OP_CEC_VERSION_1_3A``
      - 4
      - CEC version according to the HDMI 1.3a standard.
	(依据 HDMI 1.3a 标准的 CEC 版本。)
    * .. _`CEC-OP-CEC-VERSION-1-4B`:

      - ``CEC_OP_CEC_VERSION_1_4B``
      - 5
      - CEC version according to the HDMI 1.4b standard.
	(依据 HDMI 1.4b 标准的 CEC 版本。)
    * .. _`CEC-OP-CEC-VERSION-2-0`:

      - ``CEC_OP_CEC_VERSION_2_0``
      - 6
      - CEC version according to the HDMI 2.0 standard.
	(依据 HDMI 2.0 标准的 CEC 版本。)

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _cec-prim-dev-types:

.. flat-table:: CEC Primary Device Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * .. _`CEC-OP-PRIM-DEVTYPE-TV`:

      - ``CEC_OP_PRIM_DEVTYPE_TV``
      - 0
      - Use for a TV.
	(用于电视。)
    * .. _`CEC-OP-PRIM-DEVTYPE-RECORD`:

      - ``CEC_OP_PRIM_DEVTYPE_RECORD``
      - 1
      - Use for a recording device.
	(用于录制设备。)
    * .. _`CEC-OP-PRIM-DEVTYPE-TUNER`:

      - ``CEC_OP_PRIM_DEVTYPE_TUNER``
      - 3
      - Use for a device with a tuner.
	(用于带调谐器的设备。)
    * .. _`CEC-OP-PRIM-DEVTYPE-PLAYBACK`:

      - ``CEC_OP_PRIM_DEVTYPE_PLAYBACK``
      - 4
      - Use for a playback device.
	(用于播放设备。)
    * .. _`CEC-OP-PRIM-DEVTYPE-AUDIOSYSTEM`:

      - ``CEC_OP_PRIM_DEVTYPE_AUDIOSYSTEM``
      - 5
      - Use for an audio system (e.g. an audio/video receiver).
	(用于音频系统 (例如音视频接收机)。)
    * .. _`CEC-OP-PRIM-DEVTYPE-SWITCH`:

      - ``CEC_OP_PRIM_DEVTYPE_SWITCH``
      - 6
      - Use for a CEC switch.
	(用于 CEC 交换器。)
    * .. _`CEC-OP-PRIM-DEVTYPE-VIDEOPROC`:

      - ``CEC_OP_PRIM_DEVTYPE_VIDEOPROC``
      - 7
      - Use for a video processor device.
	(用于视频处理设备。)

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _cec-log-addr-types:

.. flat-table:: CEC Logical Address Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 16

    * .. _`CEC-LOG-ADDR-TYPE-TV`:

      - ``CEC_LOG_ADDR_TYPE_TV``
      - 0
      - Use for a TV.
	(用于电视。)
    * .. _`CEC-LOG-ADDR-TYPE-RECORD`:

      - ``CEC_LOG_ADDR_TYPE_RECORD``
      - 1
      - Use for a recording device.
	(用于录制设备。)
    * .. _`CEC-LOG-ADDR-TYPE-TUNER`:

      - ``CEC_LOG_ADDR_TYPE_TUNER``
      - 2
      - Use for a tuner device.
	(用于调谐器设备。)
    * .. _`CEC-LOG-ADDR-TYPE-PLAYBACK`:

      - ``CEC_LOG_ADDR_TYPE_PLAYBACK``
      - 3
      - Use for a playback device.
	(用于播放设备。)
    * .. _`CEC-LOG-ADDR-TYPE-AUDIOSYSTEM`:

      - ``CEC_LOG_ADDR_TYPE_AUDIOSYSTEM``
      - 4
      - Use for an audio system device.
	(用于音频系统设备。)
    * .. _`CEC-LOG-ADDR-TYPE-SPECIFIC`:

      - ``CEC_LOG_ADDR_TYPE_SPECIFIC``
      - 5
      - Use for a second TV or for a video processor device.
	(用于第二台电视或视频处理设备。)
    * .. _`CEC-LOG-ADDR-TYPE-UNREGISTERED`:

      - ``CEC_LOG_ADDR_TYPE_UNREGISTERED``
      - 6
      - Use this if you just want to remain unregistered. Used for pure
	CEC switches or CDC-only devices (CDC: Capability Discovery and
	Control).
	(如果你只想保持未注册状态，就使用这一项。用于纯 CEC 交换器或
	CDC-only 设备 (CDC: 能力发现与控制)。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _cec-all-dev-types-flags:

.. flat-table:: CEC All Device Types Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * .. _`CEC-OP-ALL-DEVTYPE-TV`:

      - ``CEC_OP_ALL_DEVTYPE_TV``
      - 0x80
      - This supports the TV type.
	(支持 TV 类型。)
    * .. _`CEC-OP-ALL-DEVTYPE-RECORD`:

      - ``CEC_OP_ALL_DEVTYPE_RECORD``
      - 0x40
      - This supports the Recording type.
	(支持 Recording 类型。)
    * .. _`CEC-OP-ALL-DEVTYPE-TUNER`:

      - ``CEC_OP_ALL_DEVTYPE_TUNER``
      - 0x20
      - This supports the Tuner type.
	(支持 Tuner 类型。)
    * .. _`CEC-OP-ALL-DEVTYPE-PLAYBACK`:

      - ``CEC_OP_ALL_DEVTYPE_PLAYBACK``
      - 0x10
      - This supports the Playback type.
	(支持 Playback 类型。)
    * .. _`CEC-OP-ALL-DEVTYPE-AUDIOSYSTEM`:

      - ``CEC_OP_ALL_DEVTYPE_AUDIOSYSTEM``
      - 0x08
      - This supports the Audio System type.
	(支持 Audio System 类型。)
    * .. _`CEC-OP-ALL-DEVTYPE-SWITCH`:

      - ``CEC_OP_ALL_DEVTYPE_SWITCH``
      - 0x04
      - This supports the CEC Switch or Video Processing type.
	(支持 CEC Switch 或 Video Processing 类型。)


Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并把 ``errno`` 变量设置为相应的值。通用错
误码见 :ref:`Generic Error Codes <gen-errors>` 一章。)

The :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` can return the following
error codes:
(:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` 可以返回以下错
误码：)

ENOTTY
    The ``CEC_CAP_LOG_ADDRS`` capability wasn't set, so this ioctl is not supported.
    (未设置 ``CEC_CAP_LOG_ADDRS`` 能力，因此不支持此 ioctl。)

EBUSY
    The CEC adapter is currently configuring itself, or it is already configured and
    ``num_log_addrs`` is non-zero, or another filehandle is in exclusive follower or
    initiator mode, or the filehandle is in mode ``CEC_MODE_NO_INITIATOR``.
    (CEC 适配器正在配置自身，或者已配置完成且 ``num_log_addrs`` 非零，
    或者另一个文件句柄处于独占 follower 或 initiator 模式，或者该文件句
    柄处于 ``CEC_MODE_NO_INITIATOR`` 模式。)

EINVAL
    The contents of struct :c:type:`cec_log_addrs` is invalid.
    (struct :c:type:`cec_log_addrs` 的内容无效。)
