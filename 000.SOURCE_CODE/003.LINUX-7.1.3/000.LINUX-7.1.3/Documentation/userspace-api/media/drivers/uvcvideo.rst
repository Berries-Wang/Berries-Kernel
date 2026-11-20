.. SPDX-License-Identifier: GPL-2.0

The Linux USB Video Class (UVC) driver(Linux USB 视频类（UVC）驱动)
===================================================================

This file documents some driver-specific aspects of the UVC driver, such as
driver-specific ioctls and implementation notes.
(本文件记录 UVC 驱动的一些驱动专属内容，例如驱动专属的 ioctl 与实现说明。)

Questions and remarks can be sent to the Linux UVC development mailing list at
linux-media@vger.kernel.org.
(问题与意见可发送至 Linux UVC 开发邮件列表 linux-media@vger.kernel.org。)


Extension Unit (XU) support(扩展单元（XU）支持)
-----------------------------------------------

Introduction(引言)
~~~~~~~~~~~~~~~~~~

The UVC specification allows for vendor-specific extensions through extension
units (XUs). The Linux UVC driver supports extension unit controls (XU controls)
through two separate mechanisms:
(UVC 规范允许厂商通过扩展单元（XU）进行扩展。Linux UVC 驱动通过两种相互独立的机制支持扩展单元控制（XU 控制）：)

  - through mappings of XU controls to V4L2 controls
    (把 XU 控制映射到 V4L2 控制)
  - through a driver-specific ioctl interface
    (通过驱动专属的 ioctl 接口)

The first one allows generic V4L2 applications to use XU controls by mapping
certain XU controls onto V4L2 controls, which then show up during ordinary
control enumeration.
(第一种机制把某些 XU 控制映射到 V4L2 控制上，使通用的 V4L2 应用程序能够使用 XU 控制——这些控制随后会出现在普通的控制枚举之中。)

The second mechanism requires uvcvideo-specific knowledge for the application to
access XU controls but exposes the entire UVC XU concept to user space for
maximum flexibility.
(第二种机制要求应用程序具备 uvcvideo 特有的知识才能访问 XU 控制，但它把整个 UVC XU 概念暴露给用户空间，灵活性最大。)

Both mechanisms complement each other and are described in more detail below.
(两种机制互为补充，下文将更详细地介绍。)


Control mappings(控制映射)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The UVC driver provides an API for user space applications to define so-called
control mappings at runtime. These allow for individual XU controls or byte
ranges thereof to be mapped to new V4L2 controls. Such controls appear and
function exactly like normal V4L2 controls (i.e. the stock controls, such as
brightness, contrast, etc.). However, reading or writing of such a V4L2 controls
triggers a read or write of the associated XU control.
(UVC 驱动提供一个 API，让用户空间应用程序在运行时定义所谓的控制映射（control mappings）。它允许把单个 XU 控制或其字节范围映射为新的 V4L2 控制。这类控制的外观与行为与普通 V4L2 控制完全相同（即亮度、对比度等自带控制）。不过，读写这类 V4L2 控制会触发对关联 XU 控制的读写。)

The ioctl used to create these control mappings is called UVCIOC_CTRL_MAP.
Previous driver versions (before 0.2.0) required another ioctl to be used
beforehand (UVCIOC_CTRL_ADD) to pass XU control information to the UVC driver.
This is no longer necessary as newer uvcvideo versions query the information
directly from the device.
(用于创建这些控制映射的 ioctl 叫作 UVCIOC_CTRL_MAP。以前的驱动版本（0.2.0 之前）要求先使用另一个 ioctl（UVCIOC_CTRL_ADD）把 XU 控制信息传给 UVC 驱动。由于较新的 uvcvideo 版本会直接从设备查询该信息，这已不再必要。)

For details on the UVCIOC_CTRL_MAP ioctl please refer to the section titled
"IOCTL reference" below.
(UVCIOC_CTRL_MAP ioctl 的详细信息请参阅下文"IOCTL 参考"一节。)


3. Driver specific XU control interface(驱动专属 XU 控制接口)

For applications that need to access XU controls directly, e.g. for testing
purposes, firmware upload, or accessing binary controls, a second mechanism to
access XU controls is provided in the form of a driver-specific ioctl, namely
UVCIOC_CTRL_QUERY.
(对于需要直接访问 XU 控制的应用程序——例如出于测试目的、固件上传或访问二进制控制——提供了第二种访问 XU 控制的机制，形式是一个驱动专属的 ioctl：UVCIOC_CTRL_QUERY。)

A call to this ioctl allows applications to send queries to the UVC driver that
directly map to the low-level UVC control requests.
(调用该 ioctl，应用程序即可向 UVC 驱动发送与底层 UVC 控制请求直接对应的查询。)

In order to make such a request the UVC unit ID of the control's extension unit
and the control selector need to be known. This information either needs to be
hardcoded in the application or queried using other ways such as by parsing the
UVC descriptor or, if available, using the media controller API to enumerate a
device's entities.
(要发起这样的请求，需要知道控制所在扩展单元的 UVC 单元 ID 以及控制选择器（control selector）。这些信息要么硬编码在应用程序中，要么通过其他途径查询，例如解析 UVC 描述符，或者（如果可用）使用 media controller API 枚举设备的实体。)

Unless the control size is already known it is necessary to first make a
UVC_GET_LEN requests in order to be able to allocate a sufficiently large buffer
and set the buffer size to the correct value. Similarly, to find out whether
UVC_GET_CUR or UVC_SET_CUR are valid requests for a given control, a
UVC_GET_INFO request should be made. The bits 0 (GET supported) and 1 (SET
supported) of the resulting byte indicate which requests are valid.
(除非已经知道控制的大小，否则必须先发出 UVC_GET_LEN 请求，以便分配足够大的缓冲区并把缓冲区大小设为正确的值。类似地，要弄清 UVC_GET_CUR 或 UVC_SET_CUR 对给定控制是否为有效请求，应发出 UVC_GET_INFO 请求。结果字节的位 0（支持 GET）和位 1（支持 SET）指明哪些请求有效。)

With the addition of the UVCIOC_CTRL_QUERY ioctl the UVCIOC_CTRL_GET and
UVCIOC_CTRL_SET ioctls have become obsolete since their functionality is a
subset of the former ioctl. For the time being they are still supported but
application developers are encouraged to use UVCIOC_CTRL_QUERY instead.
(随着 UVCIOC_CTRL_QUERY ioctl 的加入，UVCIOC_CTRL_GET 与 UVCIOC_CTRL_SET ioctl 已过时，因为它们的功能是前者的子集。目前它们仍受支持，但鼓励应用程序开发者改用 UVCIOC_CTRL_QUERY。)

For details on the UVCIOC_CTRL_QUERY ioctl please refer to the section titled
"IOCTL reference" below.
(UVCIOC_CTRL_QUERY ioctl 的详细信息请参阅下文"IOCTL 参考"一节。)


Security(安全性)
~~~~~~~~~~~~~~~~

The API doesn't currently provide a fine-grained access control facility. The
UVCIOC_CTRL_ADD and UVCIOC_CTRL_MAP ioctls require super user permissions.
(该 API 目前未提供细粒度的访问控制设施。UVCIOC_CTRL_ADD 与 UVCIOC_CTRL_MAP ioctl 需要超级用户权限。)

Suggestions on how to improve this are welcome.
(欢迎就如何改进提出建议。)


Debugging(调试)
~~~~~~~~~~~~~~~

In order to debug problems related to XU controls or controls in general it is
recommended to enable the UVC_TRACE_CONTROL bit in the module parameter 'trace'.
This causes extra output to be written into the system log.
(要调试与 XU 控制或一般控制相关的问题，建议在模块参数 'trace' 中启用 UVC_TRACE_CONTROL 位。这会向系统日志写入额外的输出。)


IOCTL reference(IOCTL 参考)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

UVCIOC_CTRL_MAP - Map a UVC control to a V4L2 control(把 UVC 控制映射到 V4L2 控制)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Argument: struct uvc_xu_control_mapping
(参数：struct uvc_xu_control_mapping)

**Description**:
(**描述：**)

	This ioctl creates a mapping between a UVC control or part of a UVC
	control and a V4L2 control. Once mappings are defined, userspace
	applications can access vendor-defined UVC control through the V4L2
	control API.
	(该 ioctl 在一个 UVC 控制（或 UVC 控制的一部分）与一个 V4L2 控制之间创建映射。一旦定义了映射，用户空间应用程序即可通过 V4L2 控制 API 访问厂商自定义的 UVC 控制。)

	To create a mapping, applications fill the uvc_xu_control_mapping
	structure with information about an existing UVC control defined with
	UVCIOC_CTRL_ADD and a new V4L2 control.
	(要创建映射，应用程序在 uvc_xu_control_mapping 结构中填写两部分信息：通过 UVCIOC_CTRL_ADD 定义的既有 UVC 控制，以及一个新的 V4L2 控制。)

	A UVC control can be mapped to several V4L2 controls. For instance,
	a UVC pan/tilt control could be mapped to separate pan and tilt V4L2
	controls. The UVC control is divided into non overlapping fields using
	the 'size' and 'offset' fields and are then independently mapped to
	V4L2 control.
	(一个 UVC 控制可以映射到多个 V4L2 控制。例如，一个 UVC 平移/俯仰（pan/tilt）控制可以映射为独立的平移与俯仰两个 V4L2 控制。UVC 控制用 'size' 与 'offset' 字段划分成互不重叠的字段，再各自独立映射到 V4L2 控制。)

	For signed integer V4L2 controls the data_type field should be set to
	UVC_CTRL_DATA_TYPE_SIGNED. Other values are currently ignored.
	(对有符号整数类型的 V4L2 控制，data_type 字段应设为 UVC_CTRL_DATA_TYPE_SIGNED。其他值目前会被忽略。)

**Return value**:
(**返回值：**)

	On success 0 is returned. On error -1 is returned and errno is set
	appropriately.
	(成功时返回 0。出错时返回 -1 并相应设置 errno。)

	ENOMEM
		Not enough memory to perform the operation.
		(内存不足，无法执行该操作。)
	EPERM
		Insufficient privileges (super user privileges are required).
		(权限不足（需要超级用户权限）。)
	EINVAL
		No such UVC control.
		(没有这样的 UVC 控制。)
	EOVERFLOW
		The requested offset and size would overflow the UVC control.
		(请求的 offset 与 size 会超出 UVC 控制的范围。)
	EEXIST
		Mapping already exists.
		(映射已存在。)

**Data types**:
(**数据类型：**)

.. code-block:: none

	* struct uvc_xu_control_mapping

	__u32	id		V4L2 control identifier
	__u8	name[32]	V4L2 control name
	__u8	entity[16]	UVC extension unit GUID
	__u8	selector	UVC control selector
	__u8	size		V4L2 control size (in bits)
	__u8	offset		V4L2 control offset (in bits)
	enum v4l2_ctrl_type
		v4l2_type	V4L2 control type
	enum uvc_control_data_type
		data_type	UVC control data type
	struct uvc_menu_info
		*menu_info	Array of menu entries (for menu controls only)
	__u32	menu_count	Number of menu entries (for menu controls only)

	* struct uvc_menu_info

	__u32	value		Menu entry value used by the device
	__u8	name[32]	Menu entry name


	* enum uvc_control_data_type

	UVC_CTRL_DATA_TYPE_RAW		Raw control (byte array)
	UVC_CTRL_DATA_TYPE_SIGNED	Signed integer
	UVC_CTRL_DATA_TYPE_UNSIGNED	Unsigned integer
	UVC_CTRL_DATA_TYPE_BOOLEAN	Boolean
	UVC_CTRL_DATA_TYPE_ENUM		Enumeration
	UVC_CTRL_DATA_TYPE_BITMASK	Bitmask
	UVC_CTRL_DATA_TYPE_RECT		Rectangular area


UVCIOC_CTRL_QUERY - Query a UVC XU control(查询 UVC XU 控制)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Argument: struct uvc_xu_control_query
(参数：struct uvc_xu_control_query)

**Description**:
(**描述：**)

	This ioctl queries a UVC XU control identified by its extension unit ID
	and control selector.
	(该 ioctl 查询由扩展单元 ID 与控制选择器标识的 UVC XU 控制。)

	There are a number of different queries available that closely
	correspond to the low-level control requests described in the UVC
	specification. These requests are:
	(有多种不同的查询可用，它们与 UVC 规范中描述的底层控制请求密切对应。这些请求是：)

	UVC_GET_CUR
		Obtain the current value of the control.
		(获取控制的当前值。)
	UVC_GET_MIN
		Obtain the minimum value of the control.
		(获取控制的最小值。)
	UVC_GET_MAX
		Obtain the maximum value of the control.
		(获取控制的最大值。)
	UVC_GET_DEF
		Obtain the default value of the control.
		(获取控制的默认值。)
	UVC_GET_RES
		Query the resolution of the control, i.e. the step size of the
		allowed control values.
		(查询控制的分辨率，即允许的控制值的步长。)
	UVC_GET_LEN
		Query the size of the control in bytes.
		(查询控制的大小（以字节为单位）。)
	UVC_GET_INFO
		Query the control information bitmap, which indicates whether
		get/set requests are supported.
		(查询控制信息位图，它指明是否支持 get/set 请求。)
	UVC_SET_CUR
		Update the value of the control.
		(更新控制的值。)

	Applications must set the 'size' field to the correct length for the
	control. Exceptions are the UVC_GET_LEN and UVC_GET_INFO queries, for
	which the size must be set to 2 and 1, respectively. The 'data' field
	must point to a valid writable buffer big enough to hold the indicated
	number of data bytes.
	(应用程序必须把 'size' 字段设为该控制的正确长度。例外是 UVC_GET_LEN 与 UVC_GET_INFO 查询，它们的 size 必须分别设为 2 与 1。'data' 字段必须指向一个有效的可写缓冲区，其大小要足以容纳指定数量的数据字节。)

	Data is copied directly from the device without any driver-side
	processing. Applications are responsible for data buffer formatting,
	including little-endian/big-endian conversion. This is particularly
	important for the result of the UVC_GET_LEN requests, which is always
	returned as a little-endian 16-bit integer by the device.
	(数据从设备直接复制，驱动不做任何处理。应用程序负责数据缓冲区的格式化，包括小端/大端转换。这对 UVC_GET_LEN 请求的结果尤其重要——设备总是以小端 16 位整数返回它。)

**Return value**:
(**返回值：**)

	On success 0 is returned. On error -1 is returned and errno is set
	appropriately.
	(成功时返回 0。出错时返回 -1 并相应设置 errno。)

	ENOENT
		The device does not support the given control or the specified
		extension unit could not be found.
		(设备不支持给定的控制，或找不到指定的扩展单元。)
	ENOBUFS
		The specified buffer size is incorrect (too big or too small).
		(指定的缓冲区大小不正确（过大或过小）。)
	EINVAL
		An invalid request code was passed.
		(传入了无效的请求码。)
	EBADRQC
		The given request is not supported by the given control.
		(给定的请求不被给定的控制支持。)
	EFAULT
		The data pointer references an inaccessible memory area.
		(data 指针引用了不可访问的内存区域。)

**Data types**:
(**数据类型：**)

.. code-block:: none

	* struct uvc_xu_control_query

	__u8	unit		Extension unit ID
	__u8	selector	Control selector
	__u8	query		Request code to send to the device
	__u16	size		Control data size (in bytes)
	__u8	*data		Control value


Driver-specific V4L2 controls(驱动专属 V4L2 控制)
-------------------------------------------------

The uvcvideo driver implements the following UVC-specific controls:
(uvcvideo 驱动实现以下 UVC 专属控制：)

``V4L2_CID_UVC_REGION_OF_INTEREST_RECT (struct)``
	This control determines the region of interest (ROI). ROI is a
	rectangular area represented by a struct :c:type:`v4l2_rect`. The
	rectangle is in global sensor coordinates using pixel units. It is
	independent of the field of view, not impacted by any cropping or
	scaling.
	(该控制决定感兴趣区域（ROI）。ROI 是一个由 struct :c:type:`v4l2_rect` 表示的矩形区域。矩形采用全局传感器坐标，以像素为单位。它与视场无关，不受任何剪裁或缩放的影响。)

	Use ``V4L2_CTRL_WHICH_MIN_VAL`` and ``V4L2_CTRL_WHICH_MAX_VAL`` to query
	the range of rectangle sizes.
	(使用 ``V4L2_CTRL_WHICH_MIN_VAL`` 与 ``V4L2_CTRL_WHICH_MAX_VAL`` 查询矩形尺寸的范围。)

	Setting a ROI allows the camera to optimize the capture for the region.
	The value of ``V4L2_CID_REGION_OF_INTEREST_AUTO`` control determines
	the detailed behavior.
	(设置 ROI 可让相机针对该区域优化采集。具体行为由 ``V4L2_CID_REGION_OF_INTEREST_AUTO`` 控制的值决定。)

	An example of use of this control, can be found in the:
	`Chrome OS USB camera HAL.
	<https://chromium.googlesource.com/chromiumos/platform2/+/refs/heads/release-R121-15699.B/camera/hal/usb/>`
	(该控制的使用示例可参见：Chrome OS USB camera HAL。)


``V4L2_CID_UVC_REGION_OF_INTEREST_AUTO (bitmask)``
	This determines which, if any, on-board features should track to the
	Region of Interest specified by the current value of
	``V4L2_CID_UVD__REGION_OF_INTEREST_RECT``.
	(它决定（如果有的话）哪些机载特性应跟踪由 ``V4L2_CID_UVD__REGION_OF_INTEREST_RECT`` 当前值指定的感兴趣区域。)

	Max value is a mask indicating all supported Auto Controls.
	(最大值是一个掩码，指明所有受支持的自动控制。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_EXPOSURE``
      - Setting this bit causes automatic exposure to track the region of
	interest instead of the whole image.
	(设置该位使自动曝光跟踪感兴趣区域而不是整幅图像。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_IRIS``
      - Setting this bit causes automatic iris to track the region of interest
        instead of the whole image.
	(设置该位使自动光圈跟踪感兴趣区域而不是整幅图像。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_WHITE_BALANCE``
      - Setting this bit causes automatic white balance to track the region
	of interest instead of the whole image.
	(设置该位使自动白平衡跟踪感兴趣区域而不是整幅图像。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_FOCUS``
      - Setting this bit causes automatic focus adjustment to track the region
        of interest instead of the whole image.
	(设置该位使自动对焦调整跟踪感兴趣区域而不是整幅图像。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_FACE_DETECT``
      - Setting this bit causes automatic face detection to track the region of
        interest instead of the whole image.
	(设置该位使自动人脸检测跟踪感兴趣区域而不是整幅图像。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_DETECT_AND_TRACK``
      - Setting this bit enables automatic face detection and tracking. The
	current value of ``V4L2_CID_REGION_OF_INTEREST_RECT`` may be updated by
	the driver.
	(设置该位启用自动人脸检测与跟踪。``V4L2_CID_REGION_OF_INTEREST_RECT`` 的当前值可能被驱动更新。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_IMAGE_STABILIZATION``
      - Setting this bit enables automatic image stabilization. The
	current value of ``V4L2_CID_REGION_OF_INTEREST_RECT`` may be updated by
	the driver.
	(设置该位启用自动图像稳定。``V4L2_CID_REGION_OF_INTEREST_RECT`` 的当前值可能被驱动更新。)
    * - ``V4L2_UVC_REGION_OF_INTEREST_AUTO_HIGHER_QUALITY``
      - Setting this bit enables automatically capture the specified region
        with higher quality if possible.
	(设置该位启用：如有可能，以更高质量采集指定区域。)
