.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_enum_links:

*********************************************************
ioctl MEDIA_IOC_ENUM_LINKS(枚举给定实体的所有 pad 和链接)
*********************************************************

Name(名称)
==========

MEDIA_IOC_ENUM_LINKS - Enumerate all pads and links for a given entity
(MEDIA_IOC_ENUM_LINKS - 枚举给定实体的所有 pad 和链接)

Synopsis(概要)
==============

.. c:macro:: MEDIA_IOC_ENUM_LINKS

``int ioctl(int fd, MEDIA_IOC_ENUM_LINKS, struct media_links_enum *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`media_links_enum`.
    (指向 struct :c:type:`media_links_enum` 的指针。)

Description(描述)
=================

To enumerate pads and/or links for a given entity, applications set the
entity field of a struct :c:type:`media_links_enum`
structure and initialize the struct
:c:type:`media_pad_desc` and struct
:c:type:`media_link_desc` structure arrays pointed by
the ``pads`` and ``links`` fields. They then call the
MEDIA_IOC_ENUM_LINKS ioctl with a pointer to this structure.
(要枚举给定实体的 pad 和/或链接，应用程序设置 struct
:c:type:`media_links_enum` 结构的 entity 字段，并初始化 ``pads`` 和
``links`` 字段所指向的 struct :c:type:`media_pad_desc` 和 struct
:c:type:`media_link_desc` 结构数组。然后用指向该结构的指针调用
MEDIA_IOC_ENUM_LINKS ioctl。)

If the ``pads`` field is not NULL, the driver fills the ``pads`` array
with information about the entity's pads. The array must have enough
room to store all the entity's pads. The number of pads can be retrieved
with :ref:`MEDIA_IOC_ENUM_ENTITIES`.
(如果 ``pads`` 字段不为 NULL，驱动会在 ``pads`` 数组中填入有关实体 pad
的信息。数组必须有足够的空间来存储实体的所有 pad。pad 的数量可以通过
:ref:`MEDIA_IOC_ENUM_ENTITIES` 获取。)

If the ``links`` field is not NULL, the driver fills the ``links`` array
with information about the entity's outbound links. The array must have
enough room to store all the entity's outbound links. The number of
outbound links can be retrieved with :ref:`MEDIA_IOC_ENUM_ENTITIES`.
(如果 ``links`` 字段不为 NULL，驱动会在 ``links`` 数组中填入有关实体出站
链接的信息。数组必须有足够的空间来存储实体的所有出站链接。出站链接的数量
可以通过 :ref:`MEDIA_IOC_ENUM_ENTITIES` 获取。)

Only forward links that originate at one of the entity's source pads are
returned during the enumeration process.
(在枚举过程中，只返回起始于实体某个源 pad 的前向链接。)

.. c:type:: media_links_enum

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_links_enum
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  __u32
       -  ``entity``
       -  Entity id, set by the application.
	  (实体 id，由应用程序设置。)

    *  -  struct :c:type:`media_pad_desc`
       -  \*\ ``pads``
       -  Pointer to a pads array allocated by the application. Ignored if
	  NULL.
	  (指向由应用程序分配的 pads 数组的指针。如果为 NULL 则忽略。)

    *  -  struct :c:type:`media_link_desc`
       -  \*\ ``links``
       -  Pointer to a links array allocated by the application. Ignored if
	  NULL.
	  (指向由应用程序分配的 links 数组的指针。如果为 NULL 则忽略。)

    *  -  __u32
       -  ``reserved[4]``
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.
	  (为未来扩展保留。驱动和应用程序必须将数组置零。)

.. c:type:: media_pad_desc

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_pad_desc
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  __u32
       -  ``entity``
       -  ID of the entity this pad belongs to.
	  (此 pad 所属实体的 ID。)

    *  -  __u16
       -  ``index``
       -  Pad index, starts at 0.
	  (pad 索引，从 0 开始。)

    *  -  __u32
       -  ``flags``
       -  Pad flags, see :ref:`media-pad-flag` for more details.
	  (pad 标志，更多细节参见 :ref:`media-pad-flag`。)

    *  -  __u32
       -  ``reserved[2]``
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.
	  (为未来扩展保留。驱动和应用程序必须将数组置零。)


.. c:type:: media_link_desc

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_link_desc
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  struct :c:type:`media_pad_desc`
       -  ``source``
       -  Pad at the origin of this link.
	  (此链接起点的 pad。)

    *  -  struct :c:type:`media_pad_desc`
       -  ``sink``
       -  Pad at the target of this link.
	  (此链接目标的 pad。)

    *  -  __u32
       -  ``flags``
       -  Link flags, see :ref:`media-link-flag` for more details.
	  (链接标志，更多细节参见 :ref:`media-link-flag`。)

    *  -  __u32
       -  ``reserved[2]``
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.
	  (为未来扩展保留。驱动和应用程序必须将数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`media_links_enum` ``id``
    references a non-existing entity.
    (struct :c:type:`media_links_enum` 的 ``id`` 引用了一个不存在的
    实体。)
