.. SPDX-License-Identifier: BSD-3-Clause

======================================================================================================
Netlink specification support for legacy Generic Netlink families(对旧式 Generic Netlink 族的规范支持)
======================================================================================================

This document describes the many additional quirks and properties
required to describe older Generic Netlink families which form
the ``genetlink-legacy`` protocol level.
(本文档描述构成 ``genetlink-legacy`` 协议层级的较旧 Generic Netlink 族
所需的众多附加怪癖 (quirk) 与属性。)

Specification(规范)
===================

Globals(全局属性)
-----------------

Attributes listed directly at the root level of the spec file.
(直接列在规范文件根级别的属性。)

version(原名)
~~~~~~~~~~~~~

Generic Netlink family version, default is 1.
(Generic Netlink 族版本，默认为 1。)

``version`` has historically been used to introduce family changes
which may break backwards compatibility. Since compatibility breaking changes
are generally not allowed ``version`` is very rarely used.
(历史上 ``version`` 被用来引入可能破坏向后兼容性的族变更。由于破坏兼容性
的变更一般是不允许的，``version`` 很少被使用。)

Attribute type nests(属性类型嵌套)
----------------------------------

New Netlink families should use ``multi-attr`` to define arrays.
Older families (e.g. ``genetlink`` control family) attempted to
define array types reusing attribute type to carry information.
(新的 netlink 族应当使用 ``multi-attr`` 来定义数组。较旧的族 (例如
``genetlink`` 控制族) 则试图复用属性类型来携带信息，以定义数组类型。)

For reference the ``multi-attr`` array may look like this::

  [ARRAY-ATTR]
    [INDEX (optionally)]
    [MEMBER1]
    [MEMBER2]
  [SOME-OTHER-ATTR]
  [ARRAY-ATTR]
    [INDEX (optionally)]
    [MEMBER1]
    [MEMBER2]

(作为参考，``multi-attr`` 数组可能长这样：)

where ``ARRAY-ATTR`` is the array entry type.
(其中 ``ARRAY-ATTR`` 是数组条目的类型。)

indexed-array(原名)
~~~~~~~~~~~~~~~~~~~

``indexed-array`` wraps the entire array in an extra attribute (hence
limiting its size to 64kB). The ``ENTRY`` nests are special and have the
index of the entry as their type instead of normal attribute type.
( ``indexed-array`` 把整个数组包裹在一个额外的属性中 (因此将其大小限制在
64kB)。``ENTRY`` 嵌套比较特殊，它们用条目的索引作为自己的类型，而不是
普通的属性类型。)

``indexed-array`` wraps the entire array in an extra attribute (hence
limiting its size to 64kB). The ``ENTRY`` nests are special and have the
index of the entry as their type instead of normal attribute type.

A ``sub-type`` is needed to describe what type in the ``ENTRY``. A ``nest``
``sub-type`` means there are nest arrays in the ``ENTRY``, with the structure
looks like::

  [SOME-OTHER-ATTR]
  [ARRAY-ATTR]
    [ENTRY]
      [MEMBER1]
      [MEMBER2]
    [ENTRY]
      [MEMBER1]
      [MEMBER2]

(需要一个 ``sub-type`` 来描述 ``ENTRY`` 中是什么类型。``nest`` 类型的
``sub-type`` 意味着 ``ENTRY`` 中有嵌套数组，结构如下：)

Other ``sub-type`` like ``u32`` means there is only one member as described
in ``sub-type`` in the ``ENTRY``. The structure looks like::

  [SOME-OTHER-ATTR]
  [ARRAY-ATTR]
    [ENTRY u32]
    [ENTRY u32]

(其他 ``sub-type`` (如 ``u32``) 意味着 ``ENTRY`` 中只有一个由 ``sub-type``
所描述类型的成员。结构如下：)

type-value(原名)
~~~~~~~~~~~~~~~~

``type-value`` is a construct which uses attribute types to carry
information about a single object (often used when array is dumped
entry-by-entry).
( ``type-value`` 是一种利用属性类型来携带单个对象信息的构造 (常用于逐条目
转储数组时)。)

``type-value`` can have multiple levels of nesting, for example
genetlink's policy dumps create the following structures::

  [POLICY-IDX]
    [ATTR-IDX]
      [POLICY-INFO-ATTR1]
      [POLICY-INFO-ATTR2]

( ``type-value`` 可以有多层嵌套，例如 genetlink 的策略转储会创建以下
结构：)

Where the first level of nest has the policy index as it's attribute
type, it contains a single nest which has the attribute index as its
type. Inside the attr-index nest are the policy attributes. Modern
Netlink families should have instead defined this as a flat structure,
the nesting serves no good purpose here.
(第一层嵌套用策略索引作为其属性类型；它包含一个单独的嵌套，该嵌套用属性
索引作为其类型。attr-index 嵌套的内部是策略属性。现代的 netlink 族本应
把它定义为扁平结构，这里的嵌套毫无益处。)

Operations(操作)
================

Enum (message ID) model(枚举 (消息 ID) 模型)
--------------------------------------------

unified(原名)
~~~~~~~~~~~~~

Modern families use the ``unified`` message ID model, which uses
a single enumeration for all messages within family. Requests and
responses share the same message ID. Notifications have separate
IDs from the same space. For example given the following list
of operations:
(现代族使用 ``unified`` 消息 ID 模型，它为族内所有消息使用单一的枚举。
请求与响应共享同一个消息 ID。通知则从同一空间中取单独的 ID。例如给定
以下操作列表：)

.. code-block:: yaml

  -
    name: a
    value: 1
    do: ...
  -
    name: b
    do: ...
  -
    name: c
    value: 4
    notify: a
  -
    name: d
    do: ...

Requests and responses for operation ``a`` will have the ID of 1,
the requests and responses of ``b`` - 2 (since there is no explicit
``value`` it's previous operation ``+ 1``). Notification ``c`` will
use the ID of 4, operation ``d`` 5 etc.
(操作 ``a`` 的请求与响应将使用 ID 1；``b`` 的请求与响应使用 2 (因为没有
显式 ``value``，取前一操作 ``+ 1``)。通知 ``c`` 将使用 ID 4，操作 ``d``
使用 5，依此类推。)

directional(原名)
~~~~~~~~~~~~~~~~~

The ``directional`` model splits the ID assignment by the direction of
the message. Messages from and to the kernel can't be confused with
each other so this conserves the ID space (at the cost of making
the programming more cumbersome).
( ``directional`` 模型按消息的方向拆分 ID 分配。来自内核与发往内核的消息
不会彼此混淆，因此这种方式节省了 ID 空间 (代价是编程更加繁琐)。)

In this case ``value`` attribute should be specified in the ``request``
``reply`` sections of the operations (if an operation has both ``do``
and ``dump`` the IDs are shared, ``value`` should be set in ``do``).
For notifications the ``value`` is provided at the op level but it
only allocates a ``reply`` (i.e. a "from-kernel" ID). Let's look
at an example:
(在这种情况下，应当在操作的 ``request``/``reply`` 小节中指定 ``value``
属性 (如果操作同时具有 ``do`` 与 ``dump``，则 ID 是共享的，``value`` 应
设置在 ``do`` 中)。对于通知，``value`` 在操作级别提供，但它只分配一个
``reply`` (即 "from-kernel" ID)。让我们看一个例子：)

.. code-block:: yaml

  -
    name: a
    do:
      request:
        value: 2
        attributes: ...
      reply:
        value: 1
        attributes: ...
  -
    name: b
    notify: a
  -
    name: c
    notify: a
    value: 7
  -
    name: d
    do: ...

In this case ``a`` will use 2 when sending the message to the kernel
and expects message with ID 1 in response. Notification ``b`` allocates
a "from-kernel" ID which is 2. ``c`` allocates "from-kernel" ID of 7.
If operation ``d`` does not set ``values`` explicitly in the spec
it will be allocated 3 for the request (``a`` is the previous operation
with a request section and the value of 2) and 8 for response (``c`` is
the previous operation in the "from-kernel" direction).
(在这种情况下，``a`` 向内核发送消息时使用 2，并期望收到 ID 为 1 的响应
消息。通知 ``b`` 分配 "from-kernel" ID，即 2。``c`` 分配 "from-kernel"
ID 7。如果操作 ``d`` 在规范中没有显式设置 ``values``，它将被分配请求用
3 (``a`` 是前一个带有 request 小节的操作，值为 2)、响应用 8 (``c`` 是
"from-kernel" 方向上前一个操作)。)

Other quirks(其他特殊之处)
==========================

Structures(结构体)
------------------

Legacy families can define C structures both to be used as the contents of
an attribute and as a fixed message header. Structures are defined in
``definitions``  and referenced in operations or attributes.
(旧式族可以定义 C 结构体，既用作属性的内容，也用作固定的消息头。结构体
定义在 ``definitions`` 中，并在操作或属性中被引用。)

members(原名)
~~~~~~~~~~~~~

 - ``name`` - The attribute name of the struct member
   ( ``name`` - 结构体成员的属性名)
 - ``type`` - One of the scalar types ``u8``, ``u16``, ``u32``, ``u64``, ``s8``,
   ``s16``, ``s32``, ``s64``, ``string``, ``binary`` or ``bitfield32``.
   ( ``type`` - 标量类型之一：``u8``、``u16``、``u32``、``u64``、``s8``、
   ``s16``、``s32``、``s64``、``string``、``binary`` 或 ``bitfield32``。)
 - ``byte-order`` - ``big-endian`` or ``little-endian``
   ( ``byte-order`` - ``big-endian`` 或 ``little-endian``)
 - ``doc``, ``enum``, ``enum-as-flags``, ``display-hint`` - Same as for
   :ref:`attribute definitions <attribute_properties>`
   ( ``doc``、``enum``、``enum-as-flags``、``display-hint`` - 与属性定义
   (attribute definitions) 一节相同)

Note that structures defined in YAML are implicitly packed according to C
conventions. For example, the following struct is 4 bytes, not 6 bytes:
(注意，YAML 中定义的结构体按照 C 惯例隐式打包。例如，下面的结构体是
4 字节而不是 6 字节：)

.. code-block:: c

  struct {
          u8 a;
          u16 b;
          u8 c;
  }

Any padding must be explicitly added and C-like languages should infer the
need for explicit padding from whether the members are naturally aligned.
(任何填充都必须显式添加，类 C 语言应当根据成员是否自然对齐来推断是否需要
显式填充。)

Here is the struct definition from above, declared in YAML:
(上面这个结构体的 YAML 声明如下：)

.. code-block:: yaml

  definitions:
    -
      name: message-header
      type: struct
      members:
        -
          name: a
          type: u8
        -
          name: b
          type: u16
        -
          name: c
          type: u8

Fixed Headers(固定头)
~~~~~~~~~~~~~~~~~~~~~

Fixed message headers can be added to operations using ``fixed-header``.
The default ``fixed-header`` can be set in ``operations`` and it can be set
or overridden for each operation.
(可以使用 ``fixed-header`` 为操作添加固定的消息头。默认 ``fixed-header``
可以在 ``operations`` 中设置，也可以为每个操作单独设置或覆盖。)

.. code-block:: yaml

  operations:
    fixed-header: message-header
    list:
      -
        name: get
        fixed-header: custom-header
        attribute-set: message-attrs

Attributes(属性)
~~~~~~~~~~~~~~~~

A ``binary`` attribute can be interpreted as a C structure using a
``struct`` property with the name of the structure definition. The
``struct`` property implies ``sub-type: struct`` so it is not necessary to
specify a sub-type.
( ``binary`` 属性可以使用 ``struct`` 属性 (取结构体定义的名称) 被解释为
C 结构体。``struct`` 属性隐含 ``sub-type: struct``，因此无需再指定
sub-type。)

.. code-block:: yaml

  attribute-sets:
    -
      name: stats-attrs
      attributes:
        -
          name: stats
          type: binary
          struct: vport-stats

C Arrays(C 数组)
----------------

Legacy families also use ``binary`` attributes to encapsulate C arrays. The
``sub-type`` is used to identify the type of scalar to extract.
(旧式族也使用 ``binary`` 属性来封装 C 数组。``sub-type`` 用于标识要提取的
标量类型。)

.. code-block:: yaml

  attributes:
    -
      name: ports
      type: binary
      sub-type: u32

Multi-message DO(多消息 DO)
---------------------------

New Netlink families should never respond to a DO operation with multiple
replies, with ``NLM_F_MULTI`` set. Use a filtered dump instead.
(新的 netlink 族绝不应该在设置了 ``NLM_F_MULTI`` 的情况下，用多个回复来
响应一个 DO 操作。应改用过滤的转储。)

At the spec level we can define a ``dumps`` property for the ``do``,
perhaps with values of ``combine`` and ``multi-object`` depending
on how the parsing should be implemented (parse into a single reply
vs list of objects i.e. pretty much a dump).
(在规范层面，我们可以为 ``do`` 定义一个 ``dumps`` 属性，取值或许可以是
``combine`` 和 ``multi-object``，取决于解析应如何实现 (解析成单个回复，
还是对象列表 —— 后者基本上就是一个转储)。)
