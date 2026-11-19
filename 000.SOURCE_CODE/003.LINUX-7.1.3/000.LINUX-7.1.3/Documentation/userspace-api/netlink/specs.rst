.. SPDX-License-Identifier: BSD-3-Clause

======================================================================
Netlink protocol specifications (in YAML)(Netlink 协议规范(YAML 格式))
======================================================================

Netlink protocol specifications are complete, machine readable descriptions of
Netlink protocols written in YAML. The goal of the specifications is to allow
separating Netlink parsing from user space logic and minimize the amount of
hand written Netlink code for each new family, command, attribute.
Netlink specs should be complete and not depend on any other spec
or C header file, making it easy to use in languages which can't include
kernel headers directly.
(Netlink 协议规范是以 YAML 编写的、完整且机器可读的 Netlink 协议描述。规范的目标是让 Netlink 解析与用户空间逻辑分离，并为每个新的 family、命令和属性尽量减少手写的 Netlink 代码。Netlink 规范应当完备，不依赖任何其他规范或 C 头文件，从而便于在无法直接包含内核头文件的语言中使用。)

Internally kernel uses the YAML specs to generate:
(内核内部使用这些 YAML 规范来生成：)

 - the C uAPI header
   (C uAPI 头文件)
 - documentation of the protocol as a ReST file - see :ref:`Documentation/netlink/specs/index.rst <specs>`
   (以 ReST 文件形式呈现的协议文档——见 :ref:`Documentation/netlink/specs/index.rst <specs>`。)
 - policy tables for input attribute validation
   (用于输入属性校验的策略表)
 - operation tables
   (操作表)

YAML specifications can be found under ``Documentation/netlink/specs/``
(YAML 规范可以在 ``Documentation/netlink/specs/`` 目录下找到。)

This document describes details of the schema.
See :doc:`intro-specs` for a practical starting guide.
(本文档描述该 schema 的细节。实用的入门指南见 :doc:`intro-specs`。)

All specs must be licensed under
``((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause)``
to allow for easy adoption in user space code.
(所有规范都必须以 ``((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause)`` 授权许可，以便用户空间代码方便地采用。)

Compatibility levels(兼容级别)
==============================

There are four schema levels for Netlink specs, from the simplest used
by new families to the most complex covering all the quirks of the old ones.
Each next level inherits the attributes of the previous level, meaning that
user capable of parsing more complex ``genetlink`` schemas is also compatible
with simpler ones. The levels are:
(Netlink 规范有四个 schema 级别，从新 family 使用的最简单级别，到覆盖老 family 各种怪癖的最复杂级别。每个更高级别都继承前一级别的属性，这意味着能够解析更复杂 ``genetlink`` schema 的用户也同样兼容更简单的 schema。这些级别是：)

 - ``genetlink`` - most streamlined, should be used by all new families
   (``genetlink`` —— 最精简，所有新 family 都应使用它。)
 - ``genetlink-c`` - superset of ``genetlink`` with extra attributes allowing
   customization of define and enum type and value names; this schema should
   be equivalent to ``genetlink`` for all implementations which don't interact
   directly with C uAPI headers
   (``genetlink-c`` —— ``genetlink`` 的超集，通过额外的属性允许自定义 define 与 enum 的类型名和值名；对于所有不直接与 C uAPI 头文件交互的实现而言，该 schema 应与 ``genetlink`` 等价。)
 - ``genetlink-legacy`` - Generic Netlink catch all schema supporting quirks of
   all old genetlink families, strange attribute formats, binary structures etc.
   (``genetlink-legacy`` —— Generic Netlink 兜底 schema，支持所有旧 genetlink family 的怪癖、奇怪的属性格式、二进制结构等。)
 - ``netlink-raw`` - catch all schema supporting pre-Generic Netlink protocols
   such as ``NETLINK_ROUTE``
   (``netlink-raw`` —— 支持 Generic Netlink 出现之前的协议的兜底 schema，例如 ``NETLINK_ROUTE``。)

The definition of the schemas (in ``jsonschema``) can be found
under ``Documentation/netlink/``.
(这些 schema 的定义(以 ``jsonschema`` 编写)可以在 ``Documentation/netlink/`` 目录下找到。)

Schema structure(Schema 模式结构)
=================================

YAML schema has the following conceptual sections:
(YAML schema 包含以下概念性的部分：)

 - globals
   (全局属性)
 - definitions
   (定义)
 - attributes
   (属性)
 - operations
   (操作)
 - multicast groups
   (多播组)

Most properties in the schema accept (or in fact require) a ``doc``
sub-property documenting the defined object.
(schema 中的大多数属性都接受(实际上是要求)一个 ``doc`` 子属性，用于说明所定义的对象。)

The following sections describe the properties of the most modern ``genetlink``
schema. See the documentation of :doc:`genetlink-c <c-code-gen>`
for information on how C names are derived from name properties.
(以下各节描述最现代的 ``genetlink`` schema 的属性。有关 C 名称如何从 name 属性派生，见 :doc:`genetlink-c <c-code-gen>` 的文档。)

See also :ref:`Documentation/core-api/netlink.rst <kernel_netlink>` for
information on the Netlink specification properties that are only relevant to
the kernel space and not part of the user space API.
(另见 :ref:`Documentation/core-api/netlink.rst <kernel_netlink>`，了解仅与内核空间相关、不属于用户空间 API 的那些 Netlink 规范属性。)

genetlink(genetlink 级)
=======================

Globals(全局属性)
-----------------

Attributes listed directly at the root level of the spec file.
(直接列在规范文件根级别的属性。)

name(名称)
~~~~~~~~~~

Name of the family. Name identifies the family in a unique way, since
the Family IDs are allocated dynamically.
(family 的名称。由于 family ID 是动态分配的，名称以唯一的方式标识 family。)

protocol(协议)
~~~~~~~~~~~~~~

The schema level, default is ``genetlink``, which is the only value
allowed for new ``genetlink`` families.
(schema 级别，默认为 ``genetlink``，这也是新 ``genetlink`` family 唯一允许的取值。)

definitions(定义)
-----------------

Array of type and constant definitions.
(类型与常量定义的数组。)

name(名称)
~~~~~~~~~~

Name of the type / constant.
(类型/常量的名称。)

type(类型)
~~~~~~~~~~

One of the following types:
(以下类型之一：)

 - const - a single, standalone constant
   (const —— 单个、独立的常量。)
 - enum - defines an integer enumeration, with values for each entry
   incrementing by 1, (e.g. 0, 1, 2, 3)
   (enum —— 定义整数枚举，每个条目的值依次递增 1(例如 0、1、2、3)。)
 - flags - defines an integer enumeration, with values for each entry
   occupying a bit, starting from bit 0, (e.g. 1, 2, 4, 8)
   (flags —— 定义整数枚举，每个条目的值各占一位，从第 0 位开始(例如 1、2、4、8)。)

value(取值)
~~~~~~~~~~~

The value for the ``const``.
(``const`` 的取值。)

value-start(起始值)
~~~~~~~~~~~~~~~~~~~

The first value for ``enum`` and ``flags``, allows overriding the default
start value of ``0`` (for ``enum``) and starting bit (for ``flags``).
For ``flags`` ``value-start`` selects the starting bit, not the shifted value.
(``enum`` 和 ``flags`` 的第一个取值，允许覆盖默认的起始值 ``0`` (对 ``enum``)与起始位(对 ``flags``)。对 ``flags`` 而言，``value-start`` 指定的是起始位，而非移位后的值。)

Sparse enumerations are not supported.
(不支持稀疏枚举。)

entries(条目)
~~~~~~~~~~~~~

Array of names of the entries for ``enum`` and ``flags``.
(``enum`` 和 ``flags`` 条目名称的数组。)

header(头文件)
~~~~~~~~~~~~~~

For C-compatible languages, header which already defines this value.
In case the definition is shared by multiple families (e.g. ``IFNAMSIZ``)
code generators for C-compatible languages may prefer to add an appropriate
include instead of rendering a new definition.
(对于兼容 C 的语言，指已经定义了该值的头文件。当该定义被多个 family 共享时(例如 ``IFNAMSIZ``)，兼容 C 的语言的代码生成器可能更倾向于添加合适的 include，而不是渲染一个新的定义。)

attribute-sets(属性集)
----------------------

This property contains information about netlink attributes of the family.
All families have at least one attribute set, most have multiple.
``attribute-sets`` is an array, with each entry describing a single set.
(该属性包含 family 的 netlink 属性信息。所有 family 都至少有一个属性集，大多数有多个。``attribute-sets`` 是一个数组，每个条目描述一个属性集。)

Note that the spec is "flattened" and is not meant to visually resemble
the format of the netlink messages (unlike certain ad-hoc documentation
formats seen in kernel comments). In the spec subordinate attribute sets
are not defined inline as a nest, but defined in a separate attribute set
referred to with a ``nested-attributes`` property of the container.
(注意，规范是"扁平化"的，并不追求在视觉上与 netlink 消息的格式相似(不同于内核注释中见到的某些临时性质的文档格式)。在规范中，从属的属性集不是以嵌套方式内联定义的，而是定义在单独的属性集中，由容器的 ``nested-attributes`` 属性来引用。)

Spec may also contain fractional sets - sets which contain a ``subset-of``
property. Such sets describe a section of a full set, allowing narrowing down
which attributes are allowed in a nest or refining the validation criteria.
Fractional sets can only be used in nests. They are not rendered to the uAPI
in any fashion.
(规范还可以包含分数集(fractional sets)，即含有 ``subset-of`` 属性的集合。这类集合描述完整集合的一部分，允许缩小嵌套中允许的属性范围，或细化校验标准。分数集只能在嵌套中使用，不会以任何形式渲染到 uAPI。)

name(名称)
~~~~~~~~~~

Uniquely identifies the attribute set, operations and nested attributes
refer to the sets by the ``name``.
(唯一地标识该属性集；operations 与 nested attributes 通过 ``name`` 引用各个集合。)

subset-of(子集)
~~~~~~~~~~~~~~~

Re-defines a portion of another set (a fractional set).
Allows narrowing down fields and changing validation criteria
or even types of attributes depending on the nest in which they
are contained. The ``value`` of each attribute in the fractional
set is implicitly the same as in the main set.
(重新定义另一个集合的一部分(即分数集)。允许收窄字段范围，并可根据属性所在的嵌套改变校验标准甚至属性类型。分数集中每个属性的 ``value`` 隐式地与主集合中的相同。)

attributes(属性)
~~~~~~~~~~~~~~~~

List of attributes in the set.
(该集合中属性的列表。)

.. _attribute_properties:

Attribute properties(属性字段)
------------------------------

name(名称)
~~~~~~~~~~

Identifies the attribute, unique within the set.
(标识该属性，在集合内唯一。)

type(类型)
~~~~~~~~~~

Netlink attribute type, see :ref:`attr_types`.
(netlink 属性类型，见 :ref:`attr_types`。)

.. _assign_val:

value(取值)
~~~~~~~~~~~

Numerical attribute ID, used in serialized Netlink messages.
The ``value`` property can be skipped, in which case the attribute ID
will be the value of the previous attribute plus one (recursively)
and ``1`` for the first attribute in the attribute set.
(数值型属性 ID，用于序列化的 Netlink 消息中。可以省略 ``value`` 属性，此时属性 ID 将是前一个属性的值加 1(递归套用)，而属性集中第一个属性的 ID 为 ``1``。)

Attributes (and operations) use ``1`` as the default value for the first
entry (unlike enums in definitions which start from ``0``) because
entry ``0`` is almost always reserved as undefined. Spec can explicitly
set value to ``0`` if needed.
(属性(以及操作)使用 ``1`` 作为第一个条目的默认值(不同于 definitions 中从 ``0`` 开始的枚举)，因为条目 ``0`` 几乎总是保留为未定义。如有需要，规范可以显式将值设为 ``0``。)

Note that the ``value`` of an attribute is defined only in its main set
(not in subsets).
(注意，属性的 ``value`` 只在其主集合中定义(不在子集中)。)

enum(枚举)
~~~~~~~~~~

For integer types specifies that values in the attribute belong
to an ``enum`` or ``flags`` from the ``definitions`` section.
(对整数类型，指明属性中的取值属于 ``definitions`` 节中的某个 ``enum`` 或 ``flags``。)

enum-as-flags(按标志使用枚举)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Treat ``enum`` as ``flags`` regardless of its type in ``definitions``.
When both ``enum`` and ``flags`` forms are needed ``definitions`` should
contain an ``enum`` and attributes which need the ``flags`` form should
use this attribute.
(无论 ``enum`` 在 ``definitions`` 中是什么类型，都将其按 ``flags`` 对待。当 ``enum`` 与 ``flags`` 两种形式都需要时，``definitions`` 应包含一个 ``enum``，而需要 ``flags`` 形式的属性应使用本属性。)

nested-attributes(嵌套属性集)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Identifies the attribute space for attributes nested within given attribute.
Only valid for complex attributes which may have sub-attributes.
(标识嵌套在给定属性内的属性所属的属性空间。仅对可能拥有子属性的复杂属性有效。)

multi-attr (arrays)(multi-attr(数组))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Boolean property signifying that the attribute may be present multiple times.
Allowing an attribute to repeat is the recommended way of implementing arrays
(no extra nesting).
(布尔属性，表示该属性可以出现多次。允许属性重复是实现数组的推荐方式(无需额外的嵌套)。)

byte-order(字节序)
~~~~~~~~~~~~~~~~~~

For integer types specifies attribute byte order - ``little-endian``
or ``big-endian``.
(对整数类型，指明属性的字节序——``little-endian`` 或 ``big-endian``。)

checks(校验)
~~~~~~~~~~~~

Input validation constraints used by the kernel. User space should query
the policy of the running kernel using Generic Netlink introspection,
rather than depend on what is specified in the spec file.
(内核使用的输入校验约束。用户空间应通过 Generic Netlink 自省机制查询正在运行的内核的策略，而不是依赖规范文件中写明的内容。)

The validation policy in the kernel is formed by combining the type
definition (``type`` and ``nested-attributes``) and the ``checks``.
(内核中的校验策略由类型定义(``type`` 与 ``nested-attributes``)和 ``checks`` 组合而成。)

sub-type(子类型)
~~~~~~~~~~~~~~~~

Legacy families have special ways of expressing arrays. ``sub-type`` can be
used to define the type of array members in case array members are not
fully defined as attributes (in a bona fide attribute space). For instance
a C array of u32 values can be specified with ``type: binary`` and
``sub-type: u32``. Binary types and legacy array formats are described in
more detail in :doc:`genetlink-legacy`.
(遗留 family 有其表达数组的特殊方式。当数组成员没有以属性的形式(在真正的属性空间中)完整定义时，可用 ``sub-type`` 来定义数组成员的类型。例如，u32 值的 C 数组可以用 ``type: binary`` 加 ``sub-type: u32`` 来指定。二进制类型与遗留数组格式在 :doc:`genetlink-legacy` 中有更详细的描述。)

display-hint(显示提示)
~~~~~~~~~~~~~~~~~~~~~~

Optional format indicator that is intended only for choosing the right
formatting mechanism when displaying values of this type. Currently supported
hints are ``hex``, ``mac``, ``fddi``, ``ipv4``, ``ipv6`` and ``uuid``.
(可选的格式指示，仅用于在显示该类型的值时选择合适的格式化机制。目前支持的提示有 ``hex``、``mac``、``fddi``、``ipv4``、``ipv6`` 和 ``uuid``。)

operations(操作)
----------------

This section describes messages passed between the kernel and the user space.
There are three types of entries in this section - operations, notifications
and events.
(本节描述内核与用户空间之间传递的消息。本节有三类条目——operations(操作)、notifications(通知)与 events(事件)。)

Operations describe the most common request - response communication. User
sends a request and kernel replies. Each operation may contain any combination
of the two modes familiar to netlink users - ``do`` and ``dump``.
``do`` and ``dump`` in turn contain a combination of ``request`` and
``response`` properties. If no explicit message with attributes is passed
in a given direction (e.g. a ``dump`` which does not accept filter, or a ``do``
of a SET operation to which the kernel responds with just the netlink error
code) ``request`` or ``response`` section can be skipped.
``request`` and ``response`` sections list the attributes allowed in a message.
The list contains only the names of attributes from a set referred
to by the ``attribute-set`` property.
(operations 描述最常见的请求—响应式通信：用户发送请求，内核回复。每个操作可以包含 netlink 用户熟悉的两种模式的任意组合——``do`` 与 ``dump``。``do`` 与 ``dump`` 又各自包含 ``request`` 与 ``response`` 属性的组合。如果某个方向上没有显式传入带属性的消息(例如不接受过滤条件的 ``dump``，或 SET 操作的 ``do``——内核仅以 netlink 错误码作为响应)，则可以省略 ``request`` 或 ``response`` 节。``request`` 与 ``response`` 节列出消息中允许的属性；该列表只包含 ``attribute-set`` 属性所引用集合中属性的名字。)

Notifications and events both refer to the asynchronous messages sent by
the kernel to members of a multicast group. The difference between the
two is that a notification shares its contents with a GET operation
(the name of the GET operation is specified in the ``notify`` property).
This arrangement is commonly used for notifications about
objects where the notification carries the full object definition.
(notifications 与 events 都指内核发送给多播组成员的异步消息。两者的区别在于：notification 与某个 GET 操作共享其内容(GET 操作的名字在 ``notify`` 属性中指定)。这种安排常用于关于对象的通知，此时通知携带完整的对象定义。)

Events are more focused and carry only a subset of information rather than full
object state (a made up example would be a link state change event with just
the interface name and the new link state). Events contain the ``event``
property. Events are considered less idiomatic for netlink and notifications
should be preferred.
(events 更加聚焦，只携带信息的一个子集而非完整的对象状态(一个虚构的例子是只带接口名和新链路状态的链路状态变化事件)。events 含有 ``event`` 属性。一般认为 events 不太符合 netlink 的惯常用法，应优先使用 notifications。)

list(列表)
~~~~~~~~~~

The only property of ``operations`` for ``genetlink``, holds the list of
operations, notifications etc.
(``genetlink`` 中 ``operations`` 的唯一属性，保存 operations、notifications 等的列表。)

Operation properties(操作字段)
------------------------------

name(名称)
~~~~~~~~~~

Identifies the operation.
(标识该操作。)

value(取值)
~~~~~~~~~~~

Numerical message ID, used in serialized Netlink messages.
The same enumeration rules are applied as to
:ref:`attribute values<assign_val>`.
(数值型消息 ID，用于序列化的 Netlink 消息中。枚举规则与 :ref:`attribute values<assign_val>` 相同。)

attribute-set(属性集)
~~~~~~~~~~~~~~~~~~~~~

Specifies the attribute set contained within the message.
(指明消息中包含的属性集。)

do(do 操作)
~~~~~~~~~~~

Specification for the ``doit`` request. Should contain ``request``, ``reply``
or both of these properties, each holding a :ref:`attr_list`.
(``doit`` 请求的规范。应包含 ``request``、``reply`` 或两者兼有，各自持有一个 :ref:`attr_list`。)

dump(dump 操作)
~~~~~~~~~~~~~~~

Specification for the ``dumpit`` request. Should contain ``request``, ``reply``
or both of these properties, each holding a :ref:`attr_list`.
(``dumpit`` 请求的规范。应包含 ``request``、``reply`` 或两者兼有，各自持有一个 :ref:`attr_list`。)

notify(通知)
~~~~~~~~~~~~

Designates the message as a notification. Contains the name of the operation
(possibly the same as the operation holding this property) which shares
the contents with the notification (``do``).
(将消息指定为通知。包含与该通知(``do``)共享内容的操作的名字(可能与持有本属性的操作相同)。)

event(事件)
~~~~~~~~~~~

Specification of attributes in the event, holds a :ref:`attr_list`.
``event`` property is mutually exclusive with ``notify``.
(事件中属性的规范，持有一个 :ref:`attr_list`。``event`` 属性与 ``notify`` 互斥。)

mcgrp(多播组)
~~~~~~~~~~~~~

Used with ``event`` and ``notify``, specifies which multicast group
message belongs to.
(与 ``event`` 和 ``notify`` 配合使用，指明消息属于哪个多播组。)

.. _attr_list:

Message attribute list(消息属性列表)
------------------------------------

``request``, ``reply`` and ``event`` properties have a single ``attributes``
property which holds the list of attribute names.
(``request``、``reply`` 与 ``event`` 属性都有一个单独的 ``attributes`` 属性，保存属性名的列表。)

Messages can also define ``pre`` and ``post`` properties which will be rendered
as ``pre_doit`` and ``post_doit`` calls in the kernel (these properties should
be ignored by user space).
(消息还可以定义 ``pre`` 与 ``post`` 属性，它们在内核中会被渲染为 ``pre_doit`` 与 ``post_doit`` 调用(用户空间应忽略这些属性)。)

mcast-groups(多播组)
--------------------

This section lists the multicast groups of the family.
(本节列出 family 的多播组。)

list(列表)
~~~~~~~~~~

The only property of ``mcast-groups`` for ``genetlink``, holds the list
of groups.
(``genetlink`` 中 ``mcast-groups`` 的唯一属性，保存组的列表。)

Multicast group properties(多播组字段)
--------------------------------------

name(名称)
~~~~~~~~~~

Uniquely identifies the multicast group in the family. Similarly to
Family ID, Multicast Group ID needs to be resolved at runtime, based
on the name.
(在 family 内唯一地标识该多播组。与 family ID 类似，多播组 ID 也需要在运行时根据名称来解析。)

.. _attr_types:

Attribute types(属性类型)
=========================

This section describes the attribute types supported by the ``genetlink``
compatibility level. Refer to documentation of different levels for additional
attribute types.
(本节描述 ``genetlink`` 兼容级别支持的属性类型。其他属性类型请参考各个级别的文档。)

Common integer types(常用整数类型)
----------------------------------

``sint`` and ``uint`` represent signed and unsigned 64 bit integers.
If the value can fit on 32 bits only 32 bits are carried in netlink
messages, otherwise full 64 bits are carried. Note that the payload
is only aligned to 4B, so the full 64 bit value may be unaligned!
(``sint`` 与 ``uint`` 表示有符号与无符号的 64 位整数。如果值能放进 32 位，netlink 消息中就只携带 32 位；否则携带完整的 64 位。注意，载荷只按 4 字节对齐，因此完整的 64 位值可能是未对齐的！)

Common integer types should be preferred over fix-width types in majority
of cases.
(在大多数情况下，应优先使用常用整数类型而非定宽类型。)

Fix-width integer types(定宽整数类型)
-------------------------------------

Fixed-width integer types include:
``u8``, ``u16``, ``u32``, ``u64``, ``s8``, ``s16``, ``s32``, ``s64``.
(定宽整数类型包括：``u8``、``u16``、``u32``、``u64``、``s8``、``s16``、``s32``、``s64``。)

Note that types smaller than 32 bit should be avoided as using them
does not save any memory in Netlink messages (due to alignment).
See :ref:`pad_type` for padding of 64 bit attributes.
(注意，应避免使用小于 32 位的类型，因为(由于对齐的缘故)它们在 Netlink 消息中并不能节省任何内存。64 位属性的填充见 :ref:`pad_type`。)

The payload of the attribute is the integer in host order unless ``byte-order``
specifies otherwise.
(除非 ``byte-order`` 另有指定，属性的载荷就是按主机字节序表示的整数。)

64 bit values are usually aligned by the kernel but it is recommended
that the user space is able to deal with unaligned values.
(64 位值通常由内核对齐，但建议用户空间能够处理未对齐的值。)

.. _pad_type:

pad(填充)
~~~~~~~~~

Special attribute type used for padding attributes which require alignment
bigger than standard 4B alignment required by netlink (e.g. 64 bit integers).
There can only be a single attribute of the ``pad`` type in any attribute set
and it should be automatically used for padding when needed.
(特殊的属性类型，用于为需要比 netlink 要求的标准 4 字节对齐更大对齐的属性提供填充(例如 64 位整数)。任何属性集中只能有一个 ``pad`` 类型的属性，需要时应自动用它进行填充。)

flag(标志)
----------

Attribute with no payload, its presence is the entire information.
(不带载荷的属性，其存在本身就是全部信息。)

binary(二进制)
--------------

Raw binary data attribute, the contents are opaque to generic code.
(原始二进制数据属性，其内容对通用代码而言是不透明的。)

string(字符串)
--------------

Character string. Unless ``checks`` has ``unterminated-ok`` set to ``true``
the string is required to be null terminated.
``max-len`` in ``checks`` indicates the longest possible string,
if not present the length of the string is unbounded.
(字符串。除非 ``checks`` 中将 ``unterminated-ok`` 设为 ``true``，否则字符串必须以 null 结尾。``checks`` 中的 ``max-len`` 表示字符串的最大可能长度；若无该设置，字符串长度不受限制。)

Note that ``max-len`` does not count the terminating character.
(注意，``max-len`` 不计入结尾的终止字符。)

nest(嵌套)
----------

Attribute containing other (nested) attributes.
``nested-attributes`` specifies which attribute set is used inside.
(包含其他(嵌套)属性的属性。``nested-attributes`` 指明内部使用的属性集。)
