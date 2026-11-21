.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_enum_entities:

***********************************************
ioctl MEDIA_IOC_ENUM_ENTITIES(枚举实体及其属性)
***********************************************

Name(名称)
==========

MEDIA_IOC_ENUM_ENTITIES - Enumerate entities and their properties
(MEDIA_IOC_ENUM_ENTITIES - 枚举实体及其属性)

Synopsis(概要)
==============

.. c:macro:: MEDIA_IOC_ENUM_ENTITIES

``int ioctl(int fd, MEDIA_IOC_ENUM_ENTITIES, struct media_entity_desc *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`media_entity_desc`.
    (指向 struct :c:type:`media_entity_desc` 的指针。)

Description(描述)
=================

To query the attributes of an entity, applications set the id field of a
struct :c:type:`media_entity_desc` structure and
call the MEDIA_IOC_ENUM_ENTITIES ioctl with a pointer to this
structure. The driver fills the rest of the structure or returns an
EINVAL error code when the id is invalid.
(要查询实体的属性，应用程序设置 struct :c:type:`media_entity_desc` 结构的
id 字段，并用指向该结构的指针调用 MEDIA_IOC_ENUM_ENTITIES ioctl。驱动会
填充结构体的其余部分；当 id 无效时返回 EINVAL 错误码。)

.. _media-ent-id-flag-next:

Entities can be enumerated by or'ing the id with the
``MEDIA_ENT_ID_FLAG_NEXT`` flag. The driver will return information
about the entity with the smallest id strictly larger than the requested
one ('next entity'), or the ``EINVAL`` error code if there is none.
(可以通过把 id 与 ``MEDIA_ENT_ID_FLAG_NEXT`` 标志按位或来枚举实体。驱动
将返回 id 严格大于所请求 id 的最小实体（"下一个实体"）的信息；如果没有
这样的实体，则返回 ``EINVAL`` 错误码。)

Entity IDs can be non-contiguous. Applications must *not* try to
enumerate entities by calling MEDIA_IOC_ENUM_ENTITIES with increasing
id's until they get an error.
(实体 ID 可以是不连续的。应用程序 *绝* 不应该通过用递增的 id 反复调用
MEDIA_IOC_ENUM_ENTITIES 直到出错的方式来枚举实体。)

.. c:type:: media_entity_desc

.. tabularcolumns:: |p{1.5cm}|p{1.7cm}|p{1.6cm}|p{1.5cm}|p{10.6cm}|

.. flat-table:: struct media_entity_desc
    :header-rows:  0
    :stub-columns: 0
    :widths: 2 2 1 8

    *  -  __u32
       -  ``id``
       -
       -  Entity ID, set by the application. When the ID is or'ed with
	  ``MEDIA_ENT_ID_FLAG_NEXT``, the driver clears the flag and returns
	  the first entity with a larger ID. Do not expect that the ID will
	  always be the same for each instance of the device. In other words,
	  do not hardcode entity IDs in an application.
	  (实体 ID，由应用程序设置。当 ID 与 ``MEDIA_ENT_ID_FLAG_NEXT`` 按
	  位或时，驱动会清除该标志并返回 ID 更大的第一个实体。不要指望对于
	  设备的每个实例 ID 都会相同。换句话说，不要在应用程序中硬编码
	  实体 ID。)

    *  -  char
       -  ``name``\ [32]
       -
       -  Entity name as an UTF-8 NULL-terminated string. This name must be unique
          within the media topology.
	  (实体名称，为以 UTF-8 NULL 结尾的字符串。此名称在媒体拓扑内必须
	  唯一。)

    *  -  __u32
       -  ``type``
       -
       -  Entity type, see :ref:`media-entity-functions` for details.
	  (实体类型，详情参见 :ref:`media-entity-functions`。)

    *  -  __u32
       -  ``revision``
       -
       -  Entity revision. Always zero (obsolete)
	  (实体版本号。始终为零（已废弃）)

    *  -  __u32
       -  ``flags``
       -
       -  Entity flags, see :ref:`media-entity-flag` for details.
	  (实体标志，详情参见 :ref:`media-entity-flag`。)

    *  -  __u32
       -  ``group_id``
       -
       -  Entity group ID. Always zero (obsolete)
	  (实体组 ID。始终为零（已废弃）)

    *  -  __u16
       -  ``pads``
       -
       -  Number of pads
	  (pad 的数量)

    *  -  __u16
       -  ``links``
       -
       -  Total number of outbound links. Inbound links are not counted in
	  this field.
	  (出站链接的总数。入站链接不计入此字段。)

    *  -  __u32
       -  ``reserved[4]``
       -
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.
	  (为未来扩展保留。驱动和应用程序必须将数组置零。)

    *  -  union {
       -  (anonymous)

    *  -  struct
       -  ``dev``
       -
       -  Valid for (sub-)devices that create a single device node.
	  (对创建单个设备节点的（子）设备有效。)

    *  -
       -  __u32
       -  ``major``
       -  Device node major number.
	  (设备节点主设备号。)

    *  -
       -  __u32
       -  ``minor``
       -  Device node minor number.
	  (设备节点次设备号。)

    *  -  __u8
       -  ``raw``\ [184]
       -
       -
    *  - }
       -

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`media_entity_desc` ``id``
    references a non-existing entity.
    (struct :c:type:`media_entity_desc` 的 ``id`` 引用了一个不存在的
    实体。)
