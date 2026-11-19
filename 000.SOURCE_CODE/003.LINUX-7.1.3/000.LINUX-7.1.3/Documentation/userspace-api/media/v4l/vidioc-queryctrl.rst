.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_QUERYCTRL:

**************************************************************************************************************************************
ioctls VIDIOC_QUERYCTRL, VIDIOC_QUERY_EXT_CTRL and VIDIOC_QUERYMENU(ioctl VIDIOC_QUERYCTRL、VIDIOC_QUERY_EXT_CTRL 与 VIDIOC_QUERYMENU)
**************************************************************************************************************************************

Name(名称)
==========

VIDIOC_QUERYCTRL - VIDIOC_QUERY_EXT_CTRL - VIDIOC_QUERYMENU - Enumerate controls and menu control items
(枚举控件与菜单控件项。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_QUERY_CTRL

``int ioctl(int fd, int VIDIOC_QUERYCTRL, struct v4l2_queryctrl *argp)``

.. c:macro:: VIDIOC_QUERY_EXT_CTRL

``int ioctl(int fd, VIDIOC_QUERY_EXT_CTRL, struct v4l2_query_ext_ctrl *argp)``

.. c:macro:: VIDIOC_QUERYMENU

``int ioctl(int fd, VIDIOC_QUERYMENU, struct v4l2_querymenu *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_queryctrl`, :c:type:`v4l2_query_ext_ctrl`
    or :c:type:`v4l2_querymenu` (depending on the ioctl).
    (指向 struct v4l2_queryctrl、v4l2_query_ext_ctrl 或 v4l2_querymenu
    （取决于具体 ioctl）的指针。)

Description(描述)
=================

To query the attributes of a control applications set the ``id`` field
of a struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` and call the
``VIDIOC_QUERYCTRL`` ioctl with a pointer to this structure. The driver
fills the rest of the structure or returns an ``EINVAL`` error code when the
``id`` is invalid.
(要查询控件的属性，应用设置 struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` 的 ``id``
字段，然后以指向该结构的指针调用 ``VIDIOC_QUERYCTRL`` ioctl。驱动填充结构的其余
部分；当 ``id`` 无效时返回 ``EINVAL`` 错误码。)

It is possible to enumerate controls by calling ``VIDIOC_QUERYCTRL``
with successive ``id`` values starting from ``V4L2_CID_BASE`` up to and
exclusive ``V4L2_CID_LASTP1``. Drivers may return ``EINVAL`` if a control in
this range is not supported. Further applications can enumerate private
controls, which are not defined in this specification, by starting at
``V4L2_CID_PRIVATE_BASE`` and incrementing ``id`` until the driver
returns ``EINVAL``.
(可以通过从 ``V4L2_CID_BASE`` 开始、到 ``V4L2_CID_LASTP1`` 之前为止，以连续的
``id`` 值调用 ``VIDIOC_QUERYCTRL`` 来枚举控件。如果此范围内的某个控件不受支持，
驱动可能返回 ``EINVAL``。此外，应用还可以从 ``V4L2_CID_PRIVATE_BASE`` 开始、不断
递增 ``id`` 直到驱动返回 ``EINVAL``，来枚举未在本规范中定义的私有控件。)

In both cases, when the driver sets the ``V4L2_CTRL_FLAG_DISABLED`` flag
in the ``flags`` field this control is permanently disabled and should
be ignored by the application. [#f1]_
(在这两种情况下，当驱动在 ``flags`` 字段中设置 ``V4L2_CTRL_FLAG_DISABLED`` 标志
时，该控件被永久禁用，应用应忽略它。)

When the application ORs ``id`` with ``V4L2_CTRL_FLAG_NEXT_CTRL`` the
driver returns the next supported non-compound control, or ``EINVAL`` if
there is none. In addition, the ``V4L2_CTRL_FLAG_NEXT_COMPOUND`` flag
can be specified to enumerate all compound controls (i.e. controls with
type ≥ ``V4L2_CTRL_COMPOUND_TYPES`` and/or array control, in other words
controls that contain more than one value). Specify both
``V4L2_CTRL_FLAG_NEXT_CTRL`` and ``V4L2_CTRL_FLAG_NEXT_COMPOUND`` in
order to enumerate all controls, compound or not. Drivers which do not
support these flags yet always return ``EINVAL``.
(当应用把 ``id`` 与 ``V4L2_CTRL_FLAG_NEXT_CTRL`` 按位或时，驱动返回下一个受支持
的非复合控件；如果没有则返回 ``EINVAL``。此外，可以指定
``V4L2_CTRL_FLAG_NEXT_COMPOUND`` 标志来枚举所有复合控件（即类型 ≥
``V4L2_CTRL_COMPOUND_TYPES`` 的控件和/或数组控件，换言之，包含多于一个值的
控件）。同时指定 ``V4L2_CTRL_FLAG_NEXT_CTRL`` 与 ``V4L2_CTRL_FLAG_NEXT_COMPOUND``
即可枚举所有控件（无论是否复合）。尚不支持这些标志的驱动总是返回 ``EINVAL``。)

The ``VIDIOC_QUERY_EXT_CTRL`` ioctl was introduced in order to better
support controls that can use compound types, and to expose additional
control information that cannot be returned in struct
:ref:`v4l2_queryctrl <v4l2-queryctrl>` since that structure is full.
(引入 ``VIDIOC_QUERY_EXT_CTRL`` ioctl 是为了更好地支持可以使用复合类型的控件，
并暴露 struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` 无法返回的额外控件信息，
因为该结构已经装满。)

``VIDIOC_QUERY_EXT_CTRL`` is used in the same way as
``VIDIOC_QUERYCTRL``, except that the ``reserved`` array must be zeroed
as well.
(``VIDIOC_QUERY_EXT_CTRL`` 的使用方式与 ``VIDIOC_QUERYCTRL`` 相同，只是
``reserved`` 数组也必须清零。)

Additional information is required for menu controls: the names of the
menu items. To query them applications set the ``id`` and ``index``
fields of struct :ref:`v4l2_querymenu <v4l2-querymenu>` and call the
``VIDIOC_QUERYMENU`` ioctl with a pointer to this structure. The driver
fills the rest of the structure or returns an ``EINVAL`` error code when the
``id`` or ``index`` is invalid. Menu items are enumerated by calling
``VIDIOC_QUERYMENU`` with successive ``index`` values from struct
:ref:`v4l2_queryctrl <v4l2-queryctrl>` ``minimum`` to ``maximum``,
inclusive.
(菜单控件还需要额外的信息：菜单项的名称。要查询它们，应用设置 struct
:ref:`v4l2_querymenu <v4l2-querymenu>` 的 ``id`` 与 ``index`` 字段，然后以指向
该结构的指针调用 ``VIDIOC_QUERYMENU`` ioctl。驱动填充结构的其余部分；当 ``id``
或 ``index`` 无效时返回 ``EINVAL`` 错误码。菜单项通过以 struct
:ref:`v4l2_queryctrl <v4l2-queryctrl>` 的 ``minimum`` 到 ``maximum``（含）之间的
连续 ``index`` 值调用 ``VIDIOC_QUERYMENU`` 来枚举。)

.. note::

   It is possible for ``VIDIOC_QUERYMENU`` to return
   an ``EINVAL`` error code for some indices between ``minimum`` and
   ``maximum``. In that case that particular menu item is not supported by
   this driver. Also note that the ``minimum`` value is not necessarily 0.
   (对于 ``minimum`` 与 ``maximum`` 之间的某些索引，``VIDIOC_QUERYMENU`` 可能
   返回 ``EINVAL`` 错误码。这种情况下该特定菜单项不受此驱动支持。另请注意
   ``minimum`` 值不一定为 0。)

See also the examples in :ref:`control`.
(另请参阅 :ref:`control` 中的示例。)

.. tabularcolumns:: |p{1.2cm}|p{3.6cm}|p{12.5cm}|

.. _v4l2-queryctrl:

.. c:struct:: v4l2_queryctrl

.. cssclass:: longtable

.. flat-table:: struct v4l2_queryctrl
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``id``
      - Identifies the control, set by the application. See
	:ref:`control-id` for predefined IDs. When the ID is ORed with
	V4L2_CTRL_FLAG_NEXT_CTRL the driver clears the flag and
	returns the first control with a higher ID. Drivers which do not
	support this flag yet always return an ``EINVAL`` error code. (标识控件，由应用设置。预定义 ID 见 :ref:`control-id`。当 ID 与 V4L2_CTRL_FLAG_NEXT_CTRL 按位或时，驱动会清除该标志并返回第一个具有更大 ID 的控件。尚不支持该标志的驱动总是返回 ``EINVAL`` 错误码。)
    * - __u32
      - ``type``
      - Type of control, see :c:type:`v4l2_ctrl_type`. (控件类型，见 :c:type:`v4l2_ctrl_type`。)
    * - __u8
      - ``name``\ [32]
      - Name of the control, a NUL-terminated ASCII string. This
	information is intended for the user. (控件的名称，以 NUL 结尾的 ASCII 字符串。此信息面向用户。)
    * - __s32
      - ``minimum``
      - Minimum value, inclusive. This field gives a lower bound for the
	control. See enum :c:type:`v4l2_ctrl_type` how
	the minimum value is to be used for each possible control type.
	Note that this a signed 32-bit value. (最小值（含）。该字段为控件给出下界。对于每种可能的控件类型，最小值应如何使用见枚举 :c:type:`v4l2_ctrl_type`。注意这是一个有符号 32 位值。)
    * - __s32
      - ``maximum``
      - Maximum value, inclusive. This field gives an upper bound for the
	control. See enum :c:type:`v4l2_ctrl_type` how
	the maximum value is to be used for each possible control type.
	Note that this a signed 32-bit value. (最大值（含）。该字段为控件给出上界。对于每种可能的控件类型，最大值应如何使用见枚举 :c:type:`v4l2_ctrl_type`。注意这是一个有符号 32 位值。)
    * - __s32
      - ``step``
      - This field gives a step size for the control. See enum
	:c:type:`v4l2_ctrl_type` how the step value is
	to be used for each possible control type. Note that this an
	unsigned 32-bit value.

	Generally drivers should not scale hardware control values. It may
	be necessary for example when the ``name`` or ``id`` imply a
	particular unit and the hardware actually accepts only multiples
	of said unit. If so, drivers must take care values are properly
	rounded when scaling, such that errors will not accumulate on
	repeated read-write cycles.

	This field gives the smallest change of an integer control
	actually affecting hardware. Often the information is needed when
	the user can change controls by keyboard or GUI buttons, rather
	than a slider. When for example a hardware register accepts values
	0-511 and the driver reports 0-65535, step should be 128. (该字段给出控件的步长。对于每种可能的控件类型，步长值应如何使用见枚举 :c:type:`v4l2_ctrl_type`。注意这是一个无符号 64 位值。一般而言，驱动不应缩放硬件控件值。例如当 ``name`` 或 ``id`` 隐含特定单位、而硬件实际只接受该单位的倍数时，可能有必要缩放。如果这样做，驱动在缩放时必须注意正确舍入，使误差不会在反复读写循环中累积。该字段给出整数控件实际影响硬件的最小变化量。当用户通过键盘或 GUI 按钮而不是滑块更改控件时，通常需要此信息。例如，当硬件寄存器接受 0-511 的值而驱动报告 0-65535 时，步长应为 128。)

	Note that although signed, the step value is supposed to be always
	positive. (该字段给出控件的步长。对于每种可能的控件类型，步长值应如何使用见枚举 :c:type:`v4l2_ctrl_type`。注意这是一个无符号 32 位值。一般而言，驱动不应缩放硬件控件值。例如当 ``name`` 或 ``id`` 隐含特定单位、而硬件实际只接受该单位的倍数时，可能有必要缩放。如果这样做，驱动在缩放时必须注意正确舍入，使误差不会在反复读写循环中累积。该字段给出整数控件实际影响硬件的最小变化量。当用户通过键盘或 GUI 按钮而不是滑块更改控件时，通常需要此信息。例如，当硬件寄存器接受 0-511 的值而驱动报告 0-65535 时，步长应为 128。注意虽然有符号，但步长值应当始终为正。)
    * - __s32
      - ``default_value``
      - The default value of a ``V4L2_CTRL_TYPE_INTEGER``, ``_BOOLEAN``,
	``_BITMASK``, ``_MENU`` or ``_INTEGER_MENU`` control. Not valid
	for other types of controls. (``V4L2_CTRL_TYPE_INTEGER``、``_BOOLEAN``、``_BITMASK``、``_MENU`` 或 ``_INTEGER_MENU`` 控件的默认值。对其他类型的控件无效。)

	.. note::

	   Drivers reset controls to their default value only when
	   the driver is first loaded, never afterwards. (仅当驱动首次加载时，驱动才会把控件重置为其默认值，此后从不重置。)
    * - __u32
      - ``flags``
      - Control flags, see :ref:`control-flags`. (控件标志，见 :ref:`control-flags`。)
    * - __u32
      - ``reserved``\ [2]
      - Reserved for future extensions. Drivers must set the array to
	zero. (为将来扩展保留。驱动必须把该数组置零。)


.. tabularcolumns:: |p{1.2cm}|p{5.5cm}|p{10.6cm}|

.. _v4l2-query-ext-ctrl:

.. cssclass:: longtable

.. c:struct:: v4l2_query_ext_ctrl

.. flat-table:: struct v4l2_query_ext_ctrl
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``id``
      - Identifies the control, set by the application. See
	:ref:`control-id` for predefined IDs. When the ID is ORed with
	``V4L2_CTRL_FLAG_NEXT_CTRL`` the driver clears the flag and
	returns the first non-compound control with a higher ID. When the
	ID is ORed with ``V4L2_CTRL_FLAG_NEXT_COMPOUND`` the driver clears
	the flag and returns the first compound control with a higher ID.
	Set both to get the first control (compound or not) with a higher
	ID. (标识控件，由应用设置。预定义 ID 见 :ref:`control-id`。当 ID 与 ``V4L2_CTRL_FLAG_NEXT_CTRL`` 按位或时，驱动清除该标志并返回第一个具有更大 ID 的非复合控件。当 ID 与 ``V4L2_CTRL_FLAG_NEXT_COMPOUND`` 按位或时，驱动清除该标志并返回第一个具有更大 ID 的复合控件。同时设置两者可获得第一个具有更大 ID 的控件（无论是否复合）。)
    * - __u32
      - ``type``
      - Type of control, see :c:type:`v4l2_ctrl_type`. (控件类型，见 :c:type:`v4l2_ctrl_type`。)
    * - char
      - ``name``\ [32]
      - Name of the control, a NUL-terminated ASCII string. This
	information is intended for the user. (控件的名称，以 NUL 结尾的 ASCII 字符串。此信息面向用户。)
    * - __s64
      - ``minimum``
      - Minimum value, inclusive. This field gives a lower bound for the
	control. See enum :c:type:`v4l2_ctrl_type` how
	the minimum value is to be used for each possible control type.
	Note that this a signed 64-bit value. (最小值（含）。该字段为控件给出下界。对于每种可能的控件类型，最小值应如何使用见枚举 :c:type:`v4l2_ctrl_type`。注意这是一个有符号 64 位值。)
    * - __s64
      - ``maximum``
      - Maximum value, inclusive. This field gives an upper bound for the
	control. See enum :c:type:`v4l2_ctrl_type` how
	the maximum value is to be used for each possible control type.
	Note that this a signed 64-bit value. (最大值（含）。该字段为控件给出上界。对于每种可能的控件类型，最大值应如何使用见枚举 :c:type:`v4l2_ctrl_type`。注意这是一个有符号 64 位值。)
    * - __u64
      - ``step``
      - This field gives a step size for the control. See enum
	:c:type:`v4l2_ctrl_type` how the step value is
	to be used for each possible control type. Note that this an
	unsigned 64-bit value.

	Generally drivers should not scale hardware control values. It may
	be necessary for example when the ``name`` or ``id`` imply a
	particular unit and the hardware actually accepts only multiples
	of said unit. If so, drivers must take care values are properly
	rounded when scaling, such that errors will not accumulate on
	repeated read-write cycles.

	This field gives the smallest change of an integer control
	actually affecting hardware. Often the information is needed when
	the user can change controls by keyboard or GUI buttons, rather
	than a slider. When for example a hardware register accepts values
	0-511 and the driver reports 0-65535, step should be 128.
    * - __s64
      - ``default_value``
      - The default value of a ``V4L2_CTRL_TYPE_INTEGER``, ``_INTEGER64``,
	``_BOOLEAN``, ``_BITMASK``, ``_MENU``, ``_INTEGER_MENU``, ``_U8``
	or ``_U16`` control. Not valid for other types of controls. (``V4L2_CTRL_TYPE_INTEGER``、``_INTEGER64``、``_BOOLEAN``、``_BITMASK``、``_MENU``、``_INTEGER_MENU``、``_U8`` 或 ``_U16`` 控件的默认值。对其他类型的控件无效。)

	.. note::

	   Drivers reset controls to their default value only when
	   the driver is first loaded, never afterwards. (仅当驱动首次加载时，驱动才会把控件重置为其默认值，此后从不重置。)
    * - __u32
      - ``flags``
      - Control flags, see :ref:`control-flags`. (控件标志，见 :ref:`control-flags`。)
    * - __u32
      - ``elem_size``
      - The size in bytes of a single element of the array. Given a char
	pointer ``p`` to a 3-dimensional array you can find the position
	of cell ``(z, y, x)`` as follows:
	``p + ((z * dims[1] + y) * dims[0] + x) * elem_size``.
	``elem_size`` is always valid, also when the control isn't an
	array. For string controls ``elem_size`` is equal to
	``maximum + 1``. (数组单个元素的大小（字节）。给定指向三维数组的 char 指针 ``p``，可以用 ``p + ((z * dims[1] + y) * dims[0] + x) * elem_size`` 求出单元格 ``(z, y, x)`` 的位置。``elem_size`` 始终有效，即使控件不是数组。对于字符串控件，``elem_size`` 等于 ``maximum + 1``。)
    * - __u32
      - ``elems``
      - The number of elements in the N-dimensional array. If this control
	is not an array, then ``elems`` is 1. The ``elems`` field can
	never be 0. (N 维数组中的元素数量。如果该控件不是数组，则 ``elems`` 为 1。``elems`` 字段绝不能为 0。)
    * - __u32
      - ``nr_of_dims``
      - The number of dimension in the N-dimensional array. If this
	control is not an array, then this field is 0. (N 维数组的维度数量。如果该控件不是数组，则该字段为 0。)
    * - __u32
      - ``dims[V4L2_CTRL_MAX_DIMS]``
      - The size of each dimension. The first ``nr_of_dims`` elements of
	this array must be non-zero, all remaining elements must be zero. (每个维度的大小。该数组的前 ``nr_of_dims`` 个元素必须非零，其余元素必须为零。)
    * - __u32
      - ``reserved``\ [32]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero. (为将来扩展保留。应用与驱动必须把该数组置零。)


.. tabularcolumns:: |p{1.2cm}|p{3.0cm}|p{13.1cm}|

.. _v4l2-querymenu:

.. c:struct:: v4l2_querymenu

.. flat-table:: struct v4l2_querymenu
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``id``
      - Identifies the control, set by the application from the respective
	struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` ``id``. (标识控件，由应用从相应 struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` 的 ``id`` 设置。)
    * - __u32
      - ``index``
      - Index of the menu item, starting at zero, set by the application. (菜单项的索引，从零开始，由应用设置。)
    * - union {
      - (anonymous) (匿名)
    * - __u8
      - ``name``\ [32]
      - Name of the menu item, a NUL-terminated ASCII string. This
	information is intended for the user. This field is valid for
	``V4L2_CTRL_TYPE_MENU`` type controls. (菜单项的名称，以 NUL 结尾的 ASCII 字符串。此信息面向用户。该字段对 ``V4L2_CTRL_TYPE_MENU`` 类型控件有效。)
    * - __s64
      - ``value``
      - Value of the integer menu item. This field is valid for
	``V4L2_CTRL_TYPE_INTEGER_MENU`` type controls. (整数菜单项的值。该字段对 ``V4L2_CTRL_TYPE_INTEGER_MENU`` 类型控件有效。)
    * - }
      -
    * - __u32
      - ``reserved``
      - Reserved for future extensions. Drivers must set the array to
	zero. (为将来扩展保留。驱动必须把该数组置零。)

.. c:type:: v4l2_ctrl_type

.. raw:: latex

   \footnotesize

.. tabularcolumns:: |p{6.5cm}|p{1.5cm}|p{1.1cm}|p{1.5cm}|p{6.8cm}|

.. cssclass:: longtable

.. flat-table:: enum v4l2_ctrl_type
    :header-rows:  1
    :stub-columns: 0
    :widths:       30 5 5 5 55

    * - Type (类型)
      - ``minimum``
      - ``step``
      - ``maximum``
      - Description (描述)
    * - ``V4L2_CTRL_TYPE_INTEGER``
      - any
      - any
      - any
      - An integer-valued control ranging from minimum to maximum
	inclusive. The step value indicates the increment between values. (一个取值从 minimum 到 maximum（含）的整数控件。步长值表示取值之间的增量。)
    * - ``V4L2_CTRL_TYPE_BOOLEAN``
      - 0
      - 1
      - 1
      - A boolean-valued control. Zero corresponds to "disabled", and one
	means "enabled". (布尔值控件。零对应 "disabled"（禁用），一表示 "enabled"（启用）。)
    * - ``V4L2_CTRL_TYPE_MENU``
      - ≥ 0
      - 1
      - N-1
      - The control has a menu of N choices. The names of the menu items
	can be enumerated with the ``VIDIOC_QUERYMENU`` ioctl. (该控件具有 N 个选项的菜单。菜单项的名称可以用 ``VIDIOC_QUERYMENU`` ioctl 枚举。)
    * - ``V4L2_CTRL_TYPE_INTEGER_MENU``
      - ≥ 0
      - 1
      - N-1
      - The control has a menu of N choices. The values of the menu items
	can be enumerated with the ``VIDIOC_QUERYMENU`` ioctl. This is (该控件具有 N 个选项的菜单。菜单项的值可以用 ``VIDIOC_QUERYMENU`` ioctl 枚举。它与 ``V4L2_CTRL_TYPE_MENU`` 类似，只是菜单项是有符号 64 位整数而不是字符串。)
	similar to ``V4L2_CTRL_TYPE_MENU`` except that instead of strings,
	the menu items are signed 64-bit integers.
    * - ``V4L2_CTRL_TYPE_BITMASK``
      - 0
      - n/a
      - any
      - A bitmask field. The maximum value is the set of bits that can be
	used, all other bits are to be 0. The maximum value is interpreted
	as a __u32, allowing the use of bit 31 in the bitmask. (位掩码字段。最大值是可以使用的位的集合，所有其他位必须为 0。最大值按 __u32 解释，因此可以在位掩码中使用第 31 位。)
    * - ``V4L2_CTRL_TYPE_BUTTON``
      - 0
      - 0
      - 0
      - A control which performs an action when set. Drivers must ignore
	the value passed with ``VIDIOC_S_CTRL`` and return an ``EACCES`` error
	code on a ``VIDIOC_G_CTRL`` attempt. (设置时执行某个动作的控件。驱动必须忽略通过 ``VIDIOC_S_CTRL`` 传入的值，并在 ``VIDIOC_G_CTRL`` 尝试时返回 ``EACCES`` 错误码。)
    * - ``V4L2_CTRL_TYPE_INTEGER64``
      - any
      - any
      - any
      - A 64-bit integer valued control. Minimum, maximum and step size
	cannot be queried using ``VIDIOC_QUERYCTRL``. Only
	``VIDIOC_QUERY_EXT_CTRL`` can retrieve the 64-bit min/max/step
	values, they should be interpreted as n/a when using
	``VIDIOC_QUERYCTRL``. (64 位整数控件。无法用 ``VIDIOC_QUERYCTRL`` 查询 minimum、maximum 与步长。只有 ``VIDIOC_QUERY_EXT_CTRL`` 能取回 64 位的 min/max/step 值；使用 ``VIDIOC_QUERYCTRL`` 时应将其解释为不适用。)
    * - ``V4L2_CTRL_TYPE_STRING``
      - ≥ 0
      - ≥ 1
      - ≥ 0
      - The minimum and maximum string lengths. The step size means that
	the string must be (minimum + N * step) characters long for N ≥ 0.
	These lengths do not include the terminating zero, so in order to
	pass a string of length 8 to
	:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` you need to
	set the ``size`` field of struct
	:c:type:`v4l2_ext_control` to 9. For
	:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` you can set
	the ``size`` field to ``maximum`` + 1. Which character encoding is
	used will depend on the string control itself and should be part
	of the control documentation. (最小与最大字符串长度。步长表示字符串必须长 (minimum + N * step) 个字符（N ≥ 0）。这些长度不包括结尾的零，因此要把长度为 8 的字符串传给 :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`，需要把 struct :c:type:`v4l2_ext_control` 的 ``size`` 字段设为 9。对于 :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`，可以把 ``size`` 字段设为 ``maximum`` + 1。使用何种字符编码取决于字符串控件本身，应当是控件文档的一部分。)
    * - ``V4L2_CTRL_TYPE_CTRL_CLASS``
      - n/a
      - n/a
      - n/a
      - This is not a control. When ``VIDIOC_QUERYCTRL`` is called with a
	control ID equal to a control class code (see :ref:`ctrl-class`)
	+ 1, the ioctl returns the name of the control class and this
	control type. Older drivers which do not support this feature
	return an ``EINVAL`` error code. (这不是一个控件。当 ``VIDIOC_QUERYCTRL`` 以等于控件类代码（见 :ref:`ctrl-class`）+ 1 的控件 ID 被调用时，ioctl 返回控件类的名称与该控件类型。不支持此特性的旧驱动返回 ``EINVAL`` 错误码。)
    * - ``V4L2_CTRL_TYPE_U8``
      - any
      - any
      - any
      - An unsigned 8-bit valued control ranging from minimum to maximum
	inclusive. The step value indicates the increment between values. (一个取值从 minimum 到 maximum（含）的无符号 8 位整数控件。步长值表示取值之间的增量。)
    * - ``V4L2_CTRL_TYPE_U16``
      - any
      - any
      - any
      - An unsigned 16-bit valued control ranging from minimum to maximum
	inclusive. The step value indicates the increment between values. (一个取值从 minimum 到 maximum（含）的无符号 16 位整数控件。步长值表示取值之间的增量。)
    * - ``V4L2_CTRL_TYPE_U32``
      - any
      - any
      - any
      - An unsigned 32-bit valued control ranging from minimum to maximum
	inclusive. The step value indicates the increment between values. (一个取值从 minimum 到 maximum（含）的无符号 32 位整数控件。步长值表示取值之间的增量。)
    * - ``V4L2_CTRL_TYPE_MPEG2_QUANTISATION``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_mpeg2_quantisation`, containing MPEG-2
	quantisation matrices for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_mpeg2_quantisation`，包含用于无状态视频解码器的 MPEG-2 量化矩阵。)
    * - ``V4L2_CTRL_TYPE_MPEG2_SEQUENCE``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_mpeg2_sequence`, containing MPEG-2
	sequence parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_mpeg2_sequence`，包含用于无状态视频解码器的 MPEG-2 序列参数。)
    * - ``V4L2_CTRL_TYPE_MPEG2_PICTURE``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_mpeg2_picture`, containing MPEG-2
	picture parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_mpeg2_picture`，包含用于无状态视频解码器的 MPEG-2 图像参数。)
    * - ``V4L2_CTRL_TYPE_AREA``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_area`, containing the width and the height
        of a rectangular area. Units depend on the use case. (一个 struct :c:type:`v4l2_area`，包含矩形区域的宽度与高度。单位取决于用例。)
    * - ``V4L2_CTRL_TYPE_RECT``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_rect`, containing a rectangle described by
	the position of its top-left corner, the width and the height. Units
	depend on the use case. Support for ``V4L2_CTRL_WHICH_MIN_VAL`` and
	``V4L2_CTRL_WHICH_MAX_VAL`` is optional and depends on the
	``V4L2_CTRL_FLAG_HAS_WHICH_MIN_MAX`` flag. See the documentation of
	the specific control on how to interpret the minimum and maximum values. (一个 struct :c:type:`v4l2_rect`，包含由其左上角位置、宽度与高度描述的矩形。单位取决于用例。对 ``V4L2_CTRL_WHICH_MIN_VAL`` 与 ``V4L2_CTRL_WHICH_MAX_VAL`` 的支持是可选的，取决于 ``V4L2_CTRL_FLAG_HAS_WHICH_MIN_MAX`` 标志。如何解释 minimum 与 maximum 值见具体控件的文档。)
    * - ``V4L2_CTRL_TYPE_H264_SPS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_h264_sps`, containing H264
	sequence parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_h264_sps`，包含用于无状态视频解码器的 H264 序列参数。)
    * - ``V4L2_CTRL_TYPE_H264_PPS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_h264_pps`, containing H264
	picture parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_h264_pps`，包含用于无状态视频解码器的 H264 图像参数。)
    * - ``V4L2_CTRL_TYPE_H264_SCALING_MATRIX``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_h264_scaling_matrix`, containing H264
	scaling matrices for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_h264_scaling_matrix`，包含用于无状态视频解码器的 H264 缩放矩阵。)
    * - ``V4L2_CTRL_TYPE_H264_SLICE_PARAMS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_h264_slice_params`, containing H264
	slice parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_h264_slice_params`，包含用于无状态视频解码器的 H264 条带参数。)
    * - ``V4L2_CTRL_TYPE_H264_DECODE_PARAMS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_h264_decode_params`, containing H264
	decode parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_h264_decode_params`，包含用于无状态视频解码器的 H264 解码参数。)
    * - ``V4L2_CTRL_TYPE_FWHT_PARAMS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_fwht_params`, containing FWHT
	parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_fwht_params`，包含用于无状态视频解码器的 FWHT 参数。)
    * - ``V4L2_CTRL_TYPE_HEVC_SPS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_sps`, containing HEVC Sequence
	Parameter Set for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_sps`，包含用于无状态视频解码器的 HEVC 序列参数集。)
    * - ``V4L2_CTRL_TYPE_HEVC_PPS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_pps`, containing HEVC Picture
	Parameter Set for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_pps`，包含用于无状态视频解码器的 HEVC 图像参数集。)
    * - ``V4L2_CTRL_TYPE_HEVC_SLICE_PARAMS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_slice_params`, containing HEVC
	slice parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_slice_params`，包含用于无状态视频解码器的 HEVC 条带参数。)
    * - ``V4L2_CTRL_TYPE_HEVC_SCALING_MATRIX``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_scaling_matrix`, containing HEVC
	scaling matrix for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_scaling_matrix`，包含用于无状态视频解码器的 HEVC 缩放矩阵。)
    * - ``V4L2_CTRL_TYPE_VP8_FRAME``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_vp8_frame`, containing VP8
	frame parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_vp8_frame`，包含用于无状态视频解码器的 VP8 帧参数。)
    * - ``V4L2_CTRL_TYPE_HEVC_DECODE_PARAMS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_decode_params`, containing HEVC
	decoding parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_decode_params`，包含用于无状态视频解码器的 HEVC 解码参数。)
    * - ``V4L2_CTRL_TYPE_HEVC_EXT_SPS_LT_RPS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_ext_sps_lt_rps`, containing HEVC
	extended Long-Term RPS for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_ext_sps_lt_rps`，包含用于无状态视频解码器的 HEVC 扩展长期 RPS。)
    * - ``V4L2_CTRL_TYPE_HEVC_EXT_SPS_ST_RPS``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_hevc_ext_sps_st_rps`, containing HEVC
	extended Short-Term RPS for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_hevc_ext_sps_st_rps`，包含用于无状态视频解码器的 HEVC 扩展短期 RPS。)
    * - ``V4L2_CTRL_TYPE_VP9_COMPRESSED_HDR``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_vp9_compressed_hdr`, containing VP9
	probabilities updates for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_vp9_compressed_hdr`，包含用于无状态视频解码器的 VP9 概率更新。)
    * - ``V4L2_CTRL_TYPE_VP9_FRAME``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_vp9_frame`, containing VP9
	frame decode parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_vp9_frame`，包含用于无状态视频解码器的 VP9 帧解码参数。)
    * - ``V4L2_CTRL_TYPE_AV1_SEQUENCE``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_av1_sequence`, containing AV1 Sequence OBU
	decoding parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_av1_sequence`，包含用于无状态视频解码器的 AV1 序列 OBU 解码参数。)
    * - ``V4L2_CTRL_TYPE_AV1_TILE_GROUP_ENTRY``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_av1_tile_group_entry`, containing AV1 Tile Group
	OBU decoding parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_av1_tile_group_entry`，包含用于无状态视频解码器的 AV1 Tile Group OBU 解码参数。)
    * - ``V4L2_CTRL_TYPE_AV1_FRAME``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_av1_frame`, containing AV1 Frame/Frame
	Header OBU decoding parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_av1_frame`，包含用于无状态视频解码器的 AV1 帧/帧头 OBU 解码参数。)
    * - ``V4L2_CTRL_TYPE_AV1_FILM_GRAIN``
      - n/a
      - n/a
      - n/a
      - A struct :c:type:`v4l2_ctrl_av1_film_grain`, containing AV1 Film Grain
        parameters for stateless video decoders. (一个 struct :c:type:`v4l2_ctrl_av1_film_grain`，包含用于无状态视频解码器的 AV1 胶片颗粒参数。)

.. raw:: latex

   \normalsize

.. tabularcolumns:: |p{7.3cm}|p{1.8cm}|p{8.2cm}|

.. cssclass:: longtable

.. _control-flags:

.. flat-table:: Control Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_CTRL_FLAG_DISABLED``
      - 0x0001
      - This control is permanently disabled and should be ignored by the
	application. Any attempt to change the control will result in an
	``EINVAL`` error code. (该控件被永久禁用，应被应用忽略。任何更改该控件的尝试都会导致 ``EINVAL`` 错误码。)
    * - ``V4L2_CTRL_FLAG_GRABBED``
      - 0x0002
      - This control is temporarily unchangeable, for example because
	another application took over control of the respective resource.
	Such controls may be displayed specially in a user interface.
	Attempts to change the control may result in an ``EBUSY`` error code. (该控件暂时不可更改，例如因为另一个应用接管了相应资源的控制权。这类控件可以在用户界面中特殊显示。更改该控件的尝试可能导致 ``EBUSY`` 错误码。)
    * - ``V4L2_CTRL_FLAG_READ_ONLY``
      - 0x0004
      - This control is permanently readable only. Any attempt to change
	the control will result in an ``EINVAL`` error code. (该控件永久只读。任何更改该控件的尝试都会导致 ``EINVAL`` 错误码。)
    * - ``V4L2_CTRL_FLAG_UPDATE``
      - 0x0008
      - A hint that changing this control may affect the value of other
	controls within the same control class. Applications should update
	their user interface accordingly. (提示：更改该控件可能影响同一控件类中其他控件的值。应用应相应地更新其用户界面。)
    * - ``V4L2_CTRL_FLAG_INACTIVE``
      - 0x0010
      - This control is not applicable to the current configuration and
	should be displayed accordingly in a user interface. For example
	the flag may be set on a MPEG audio level 2 bitrate control when
	MPEG audio encoding level 1 was selected with another control. (该控件不适用于当前配置，应在用户界面中相应显示。例如，当通过另一个控件选择了 MPEG 音频编码级别 1 时，该标志可能设置在 MPEG 音频级别 2 码率控件上。)
    * - ``V4L2_CTRL_FLAG_SLIDER``
      - 0x0020
      - A hint that this control is best represented as a slider-like
	element in a user interface. (提示：该控件在用户界面中最好用滑块状元素表示。)
    * - ``V4L2_CTRL_FLAG_WRITE_ONLY``
      - 0x0040
      - This control is permanently writable only. Any attempt to read the
	control will result in an ``EACCES`` error code error code. This flag
	is typically present for relative controls or action controls
	where writing a value will cause the device to carry out a given
	action (e. g. motor control) but no meaningful value can be
	returned. (该控件永久只写。任何读取该控件的尝试都会导致 ``EACCES`` 错误码错误码。该标志通常出现在相对控件或动作控件上——写入一个值会使设备执行给定动作（例如电机控制），但无法返回有意义的值。)
    * - ``V4L2_CTRL_FLAG_VOLATILE``
      - 0x0080
      - This control is volatile, which means that the value of the
	control changes continuously. A typical example would be the
	current gain value if the device is in auto-gain mode. In such a
	case the hardware calculates the gain value based on the lighting
	conditions which can change over time. (该控件是易变的（volatile），即控件的值会持续变化。典型例子是设备处于自动增益模式时的当前增益值。这种情况下，硬件会根据可能随时间变化的光照条件计算增益值。)

	.. note::

	   Setting a new value for a volatile control will be ignored
	   unless
	   :ref:`V4L2_CTRL_FLAG_EXECUTE_ON_WRITE <FLAG_EXECUTE_ON_WRITE>`
	   is also set.
	   Setting a new value for a volatile control will *never* trigger a
	   :ref:`V4L2_EVENT_CTRL_CH_VALUE <ctrl-changes-flags>` event. (为易变控件设置新值将被忽略，除非也设置了 :ref:`V4L2_CTRL_FLAG_EXECUTE_ON_WRITE <FLAG_EXECUTE_ON_WRITE>`。为易变控件设置新值 *绝不会* 触发 :ref:`V4L2_EVENT_CTRL_CH_VALUE <ctrl-changes-flags>` 事件。)
    * - ``V4L2_CTRL_FLAG_HAS_PAYLOAD``
      - 0x0100
      - This control has a pointer type, so its value has to be accessed
	using one of the pointer fields of struct
	:c:type:`v4l2_ext_control`. This flag is set
	for controls that are an array, string, or have a compound type.
	In all cases you have to set a pointer to memory containing the
	payload of the control. (该控件具有指针类型，因此其值必须使用 struct :c:type:`v4l2_ext_control` 的某个指针字段来访问。该标志为是数组、字符串或具有复合类型的控件设置。所有情况下都必须设置一个指向包含控件载荷的内存的指针。)
    * .. _FLAG_EXECUTE_ON_WRITE:

      - ``V4L2_CTRL_FLAG_EXECUTE_ON_WRITE``
      - 0x0200
      - The value provided to the control will be propagated to the driver
	even if it remains constant. This is required when the control
	represents an action on the hardware. For example: clearing an
	error flag or triggering the flash. All the controls of the type
	``V4L2_CTRL_TYPE_BUTTON`` have this flag set. (提供给控件的值即使保持不变也会传播给驱动。当控件表示对硬件的一个动作时需要如此。例如：清除错误标志或触发闪光灯。所有 ``V4L2_CTRL_TYPE_BUTTON`` 类型的控件都设置有此标志。)
    * .. _FLAG_MODIFY_LAYOUT:

      - ``V4L2_CTRL_FLAG_MODIFY_LAYOUT``
      - 0x0400
      - Changing this control value may modify the layout of the
        buffer (for video devices) or the media bus format (for sub-devices).

	A typical example would be the ``V4L2_CID_ROTATE`` control.

	Note that typically controls with this flag will also set the
	``V4L2_CTRL_FLAG_GRABBED`` flag when buffers are allocated or
	streaming is in progress since most drivers do not support changing
	the format in that case. (更改此控件的值可能修改缓冲的布局（对视频设备）或 media bus 格式（对子设备）。一个典型例子是 ``V4L2_CID_ROTATE`` 控件。注意，带有此标志的控件通常还会在缓冲已分配或流式传输进行中时设置 ``V4L2_CTRL_FLAG_GRABBED`` 标志，因为大多数驱动不支持在这种情况下更改格式。)
    * - ``V4L2_CTRL_FLAG_DYNAMIC_ARRAY``
      - 0x0800
      - This control is a dynamically sized 1-dimensional array. It
        behaves the same as a regular array, except that the number
	of elements as reported by the ``elems`` field is between 1 and
	``dims[0]``. So setting the control with a differently sized
	array will change the ``elems`` field when the control is
	queried afterwards. (该控件是动态大小的 1 维数组。其行为与常规数组相同，只是 ``elems`` 字段报告的元素数量介于 1 与 ``dims[0]`` 之间。因此，以不同大小的数组设置该控件之后，再查询控件时 ``elems`` 字段会发生变化。)
    * - ``V4L2_CTRL_FLAG_HAS_WHICH_MIN_MAX``
      - 0x1000
      - This control supports getting minimum and maximum values using
        vidioc_g_ext_ctrls with V4L2_CTRL_WHICH_MIN/MAX_VAL. (该控件支持通过 vidioc_g_ext_ctrls 与 V4L2_CTRL_WHICH_MIN/MAX_VAL 获取最小值与最大值。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` ``id`` is
    invalid. The struct :ref:`v4l2_querymenu <v4l2-querymenu>` ``id``
    is invalid or ``index`` is out of range (less than ``minimum`` or
    greater than ``maximum``) or this particular menu item is not
    supported by the driver.    (struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` 的 ``id`` 无效。struct :ref:`v4l2_querymenu <v4l2-querymenu>` 的 ``id`` 无效，或 ``index`` 超出范围（小于 ``minimum`` 或大于 ``maximum``），或该特定菜单项不受驱动支持。)

EACCES
    An attempt was made to read a write-only control.
    (尝试读取一个只写控件。)

.. [#f1]
   ``V4L2_CTRL_FLAG_DISABLED`` was intended for two purposes: Drivers
   can skip predefined controls not supported by the hardware (although
   returning ``EINVAL`` would do as well), or disable predefined and private
   controls after hardware detection without the trouble of reordering
   control arrays and indices (``EINVAL`` cannot be used to skip private
   controls because it would prematurely end the enumeration).   (``V4L2_CTRL_FLAG_DISABLED`` 原本有两个用途：驱动可以跳过硬件不支持的预定义控件（尽管返回 ``EINVAL`` 也一样可行），或者在硬件检测之后禁用预定义与私有控件，而免去重新排序控件数组与索引的麻烦（``EINVAL`` 不能用于跳过私有控件，因为它会过早终止枚举）。)
