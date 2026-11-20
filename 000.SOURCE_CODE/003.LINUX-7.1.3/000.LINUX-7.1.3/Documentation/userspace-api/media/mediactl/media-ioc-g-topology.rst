.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_g_topology:

******************************************************
ioctl MEDIA_IOC_G_TOPOLOGY(ioctl MEDIA_IOC_G_TOPOLOGY)
******************************************************

Name(名称)
==========

MEDIA_IOC_G_TOPOLOGY - Enumerate the graph topology and graph element properties
(MEDIA_IOC_G_TOPOLOGY - 枚举图拓扑与图元素属性)

Synopsis(概要)
==============

.. c:macro:: MEDIA_IOC_G_TOPOLOGY

``int ioctl(int fd, MEDIA_IOC_G_TOPOLOGY, struct media_v2_topology *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 open() 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`media_v2_topology`.
    (指向 struct media_v2_topology 的指针。)

Description(描述)
=================

The typical usage of this ioctl is to call it twice. On the first call,
the structure defined at struct
:c:type:`media_v2_topology` should be zeroed. At
return, if no errors happen, this ioctl will return the
``topology_version`` and the total number of entities, interfaces, pads
and links.
(这个 ioctl 的典型用法是调用两次。第一次调用时，应将 struct
media_v2_topology 中定义的结构清零。返回时，如果没有发生错误，该 ioctl
会返回 ``topology_version`` 以及实体、接口、pad 和链路的总数。)

Before the second call, the userspace should allocate arrays to store
the graph elements that are desired, putting the pointers to them at the
ptr_entities, ptr_interfaces, ptr_links and/or ptr_pads, keeping the
other values untouched.
(在第二次调用之前，用户态应分配好数组来存储所需的图元素，把指向它们的
指针放入 ptr_entities、ptr_interfaces、ptr_links 和/或 ptr_pads，并保持
其他值不变。)

If the ``topology_version`` remains the same, the ioctl should fill the
desired arrays with the media graph elements.
(如果 ``topology_version`` 保持不变，该 ioctl 应当用媒体图元素填充所需的
数组。)

.. tabularcolumns:: |p{1.6cm}|p{3.4cm}|p{12.3cm}|

.. c:type:: media_v2_topology

.. flat-table:: struct media_v2_topology
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 2 8

    *  -  __u64
       -  ``topology_version``
       -  Version of the media graph topology. When the graph is created,
	  this field starts with zero. Every time a graph element is added
	  or removed, this field is incremented. (媒体图拓扑的版本。图被创建时，
	  该字段从零开始。每当图元素被添加或删除，该字段都会递增。)

    *  -  __u32
       -  ``num_entities``
       -  Number of entities in the graph (图中实体 (entity) 的数量)

    *  -  __u32
       -  ``reserved1``
       -  Applications and drivers shall set this to 0. (应用程序和驱动应将
	  此设置为 0。)

    *  -  __u64
       -  ``ptr_entities``
       -  A pointer to a memory area where the entities array will be
	  stored, converted to a 64-bits integer. It can be zero. if zero,
	  the ioctl won't store the entities. It will just update
	  ``num_entities`` (指向将存储实体数组的内存区域的指针，需转换为
	  64 位整数。可以为零。若为零，ioctl 不会存储实体，只会更新
	  num_entities 的值)

    *  -  __u32
       -  ``num_interfaces``
       -  Number of interfaces in the graph (图中接口 (interface) 的数量)

    *  -  __u32
       -  ``reserved2``
       -  Applications and drivers shall set this to 0. (应用程序和驱动应将
	  此设置为 0。)

    *  -  __u64
       -  ``ptr_interfaces``
       -  A pointer to a memory area where the interfaces array will be
	  stored, converted to a 64-bits integer. It can be zero. if zero,
	  the ioctl won't store the interfaces. It will just update
	  ``num_interfaces`` (指向将存储接口数组的内存区域的指针，需转换为
	  64 位整数。可以为零。若为零，ioctl 不会存储接口，只会更新
	  num_interfaces 的值)

    *  -  __u32
       -  ``num_pads``
       -  Total number of pads in the graph (图中 pad 的总数)

    *  -  __u32
       -  ``reserved3``
       -  Applications and drivers shall set this to 0. (应用程序和驱动应将
	  此设置为 0。)

    *  -  __u64
       -  ``ptr_pads``
       -  A pointer to a memory area where the pads array will be stored,
	  converted to a 64-bits integer. It can be zero. if zero, the ioctl
	  won't store the pads. It will just update ``num_pads`` (指向将存储
	  pad 数组的内存区域的指针，需转换为 64 位整数。可以为零。若为零，
	  ioctl 不会存储 pad，只会更新 num_pads 的值)

    *  -  __u32
       -  ``num_links``
       -  Total number of data and interface links in the graph (图中数据链路
	  与接口链路的总数)

    *  -  __u32
       -  ``reserved4``
       -  Applications and drivers shall set this to 0. (应用程序和驱动应将
	  此设置为 0。)

    *  -  __u64
       -  ``ptr_links``
       -  A pointer to a memory area where the links array will be stored,
	  converted to a 64-bits integer. It can be zero. if zero, the ioctl
	  won't store the links. It will just update ``num_links`` (指向将存储
	  链路数组的内存区域的指针，需转换为 64 位整数。可以为零。若为零，
	  ioctl 不会存储链路，只会更新 num_links 的值)

.. tabularcolumns:: |p{1.6cm}|p{3.2cm}|p{12.5cm}|

.. c:type:: media_v2_entity

.. flat-table:: struct media_v2_entity
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 2 8

    *  -  __u32
       -  ``id``
       -  Unique ID for the entity. Do not expect that the ID will
	  always be the same for each instance of the device. In other words,
	  do not hardcode entity IDs in an application. (实体的唯一 ID。不要
	  期望该 ID 在设备的每个实例上都相同。换句话说，不要在应用程序中
	  硬编码实体 ID。)

    *  -  char
       -  ``name``\ [64]
       -  Entity name as an UTF-8 NULL-terminated string. This name must be unique
          within the media topology. (实体名称，为以 NULL 结尾的 UTF-8 字符串。
          该名称在媒体拓扑内必须唯一。)

    *  -  __u32
       -  ``function``
       -  Entity main function, see :ref:`media-entity-functions` for details.
	  (实体主要功能，详见 media-entity-functions 一节。)

    *  -  __u32
       -  ``flags``
       -  Entity flags, see :ref:`media-entity-flag` for details.
	  Only valid if ``MEDIA_V2_ENTITY_HAS_FLAGS(media_version)``
	  returns true. The ``media_version`` is defined in struct
	  :c:type:`media_device_info` and can be retrieved using
	  :ref:`MEDIA_IOC_DEVICE_INFO`. (实体标志，详见 media-entity-flag 一节。
	  仅当 MEDIA_V2_ENTITY_HAS_FLAGS(media_version) 返回真时才有效。
	  media_version 在 struct media_device_info 中定义，可通过
	  MEDIA_IOC_DEVICE_INFO 获取。)

    *  -  __u32
       -  ``reserved``\ [5]
       -  Reserved for future extensions. Drivers and applications must set
	  this array to zero. (预留给未来扩展。驱动和应用程序必须将此数组
	  设置为零。)

.. tabularcolumns:: |p{1.6cm}|p{3.2cm}|p{12.5cm}|

.. c:type:: media_v2_interface

.. flat-table:: struct media_v2_interface
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 2 8

    *  -  __u32
       -  ``id``
       -  Unique ID for the interface. Do not expect that the ID will
	  always be the same for each instance of the device. In other words,
	  do not hardcode interface IDs in an application. (接口的唯一 ID。
	  不要期望该 ID 在设备的每个实例上都相同。换句话说，不要在应用程序
	  中硬编码接口 ID。)

    *  -  __u32
       -  ``intf_type``
       -  Interface type, see :ref:`media-intf-type` for details.
	  (接口类型，详见 media-intf-type 一节。)

    *  -  __u32
       -  ``flags``
       -  Interface flags. Currently unused. (接口标志。目前未使用。)

    *  -  __u32
       -  ``reserved``\ [9]
       -  Reserved for future extensions. Drivers and applications must set
	  this array to zero. (预留给未来扩展。驱动和应用程序必须将此数组
	  设置为零。)

    *  -  struct media_v2_intf_devnode
       -  ``devnode``
       -  Used only for device node interfaces. See
	  :c:type:`media_v2_intf_devnode` for details. (仅用于设备节点接口。
	  详见 media_v2_intf_devnode 一节。)

.. tabularcolumns:: |p{1.6cm}|p{3.2cm}|p{12.5cm}|

.. c:type:: media_v2_intf_devnode

.. flat-table:: struct media_v2_intf_devnode
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 2 8

    *  -  __u32
       -  ``major``
       -  Device node major number. (设备节点的主设备号。)

    *  -  __u32
       -  ``minor``
       -  Device node minor number. (设备节点的次设备号。)

.. tabularcolumns:: |p{1.6cm}|p{3.2cm}|p{12.5cm}|

.. c:type:: media_v2_pad

.. flat-table:: struct media_v2_pad
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 2 8

    *  -  __u32
       -  ``id``
       -  Unique ID for the pad. Do not expect that the ID will
	  always be the same for each instance of the device. In other words,
	  do not hardcode pad IDs in an application. (pad 的唯一 ID。不要期望
	  该 ID 在设备的每个实例上都相同。换句话说，不要在应用程序中硬编码
	  pad ID。)

    *  -  __u32
       -  ``entity_id``
       -  Unique ID for the entity where this pad belongs. (此 pad 所属实体的
	  唯一 ID。)

    *  -  __u32
       -  ``flags``
       -  Pad flags, see :ref:`media-pad-flag` for more details.
	  (pad 标志，更多详情参见 media-pad-flag 一节。)

    *  -  __u32
       -  ``index``
       -  Pad index, starts at 0. Only valid if ``MEDIA_V2_PAD_HAS_INDEX(media_version)``
	  returns true. The ``media_version`` is defined in struct
	  :c:type:`media_device_info` and can be retrieved using
	  :ref:`MEDIA_IOC_DEVICE_INFO`. (pad 索引，从 0 开始。仅当
	  MEDIA_V2_PAD_HAS_INDEX(media_version) 返回真时才有效。media_version
	  在 struct media_device_info 中定义，可通过 MEDIA_IOC_DEVICE_INFO
	  获取。)

    *  -  __u32
       -  ``reserved``\ [4]
       -  Reserved for future extensions. Drivers and applications must set
	  this array to zero. (预留给未来扩展。驱动和应用程序必须将此数组
	  设置为零。)

.. tabularcolumns:: |p{1.6cm}|p{3.2cm}|p{12.5cm}|

.. c:type:: media_v2_link

.. flat-table:: struct media_v2_link
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 2 8

    *  -  __u32
       -  ``id``
       -  Unique ID for the link. Do not expect that the ID will
	  always be the same for each instance of the device. In other words,
	  do not hardcode link IDs in an application. (链路的唯一 ID。不要
	  期望该 ID 在设备的每个实例上都相同。换句话说，不要在应用程序中
	  硬编码链路 ID。)

    *  -  __u32
       -  ``source_id``
       -  On pad to pad links: unique ID for the source pad.
	  (pad 到 pad 链路：源 pad 的唯一 ID。)

	  On interface to entity links: unique ID for the interface.
	  (接口到实体链路：接口的唯一 ID。)

    *  -  __u32
       -  ``sink_id``
       -  On pad to pad links: unique ID for the sink pad.
	  (pad 到 pad 链路：宿 pad 的唯一 ID。)

	  On interface to entity links: unique ID for the entity.
	  (接口到实体链路：实体的唯一 ID。)

    *  -  __u32
       -  ``flags``
       -  Link flags, see :ref:`media-link-flag` for more details.
	  (链路标志，更多详情参见 media-link-flag 一节。)

    *  -  __u32
       -  ``reserved``\ [6]
       -  Reserved for future extensions. Drivers and applications must set
	  this array to zero. (预留给未来扩展。驱动和应用程序必须将此数组
	  设置为零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并相应地设置 ``errno`` 变量。通用错误码在
Generic Error Codes 一章中描述。)

ENOSPC
    This is returned when either one or more of the num_entities,
    num_interfaces, num_links or num_pads are non-zero and are
    smaller than the actual number of elements inside the graph. This
    may happen if the ``topology_version`` changed when compared to the
    last time this ioctl was called. Userspace should usually free the
    area for the pointers, zero the struct elements and call this ioctl
    again.
    (当 num_entities、num_interfaces、num_links 或 num_pads 中有一个或多个
    非零、且小于图内实际元素数量时，会返回此错误。与上次调用该 ioctl 相比
    ``topology_version`` 发生了变化时，就可能出现这种情况。用户态通常应
    释放指针所指向的区域，将结构元素清零，然后再次调用该 ioctl。)
