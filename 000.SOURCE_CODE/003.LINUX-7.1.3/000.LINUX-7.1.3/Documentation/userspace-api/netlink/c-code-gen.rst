.. SPDX-License-Identifier: BSD-3-Clause

=========================================================
Netlink spec C code generation(Netlink 规范的 C 代码生成)
=========================================================

This document describes how Netlink specifications are used to render
C code (uAPI, policies etc.). It also defines the additional properties
allowed in older families by the ``genetlink-c`` protocol level,
to control the naming.
(本文描述如何使用 Netlink 规范来渲染 C 代码(uAPI、策略等)。它还定义了
``genetlink-c`` 协议级别允许较旧 family 使用的额外属性，用于控制命名。)

For brevity this document refers to ``name`` properties of various
objects by the object type. For example ``$attr`` is the value
of ``name`` in an attribute, and ``$family`` is the name of the
family (the global ``name`` property).
(为简洁起见，本文用对象类型来指代各对象的 ``name`` 属性。例如 ``$attr``
表示某个属性中 ``name`` 的值，``$family`` 表示 family 的名称(全局
``name`` 属性)。)

The upper case is used to denote literal values, e.g. ``$family-CMD``
means the concatenation of ``$family``, a dash character, and the literal
``CMD``.
(大写用于表示字面值，例如 ``$family-CMD`` 表示 ``$family``、一个连字符
和字面值 ``CMD`` 的拼接。)

The names of ``#defines`` and enum values are always converted to upper case,
and with dashes (``-``) replaced by underscores (``_``).
(``#defines`` 和枚举值的名称总是转换为大写，并把连字符(``-``)替换为
下划线(``_``)。)

If the constructed name is a C keyword, an extra underscore is
appended (``do`` -> ``do_``).
(如果构造出的名称是 C 关键字，则会在末尾追加一个额外的下划线
(``do`` -> ``do_``)。)

Globals(全局属性)
=================

``c-family-name`` controls the name of the ``#define`` for the family
name, default is ``$family-FAMILY-NAME``.
(``c-family-name`` 控制 family 名称 ``#define`` 的名字，默认为
``$family-FAMILY-NAME``。)

``c-version-name`` controls the name of the ``#define`` for the version
of the family, default is ``$family-FAMILY-VERSION``.
(``c-version-name`` 控制 family 版本 ``#define`` 的名字，默认为
``$family-FAMILY-VERSION``。)

``max-by-define`` selects if max values for enums are defined as a
``#define`` rather than inside the enum.
(``max-by-define`` 选择枚举的最大值是定义为 ``#define`` 还是放在枚举
内部。)

Definitions(定义)
=================

Constants(常量)
---------------

Every constant is rendered as a ``#define``.
The name of the constant is ``$family-$constant`` and the value
is rendered as a string or integer according to its type in the spec.
(每个常量都渲染为一个 ``#define``。常量名为 ``$family-$constant``，
其值根据该常量在规范中的类型渲染为字符串或整数。)

Enums and flags(枚举与标志)
---------------------------

Enums are named ``$family-$enum``. The full name can be set directly
or suppressed by specifying the ``enum-name`` property.
Default entry name is ``$family-$enum-$entry``.
If ``name-prefix`` is specified it replaces the ``$family-$enum``
portion of the entry name.
(枚举命名为 ``$family-$enum``。可以通过指定 ``enum-name`` 属性直接
设置完整名称，或将其省略。默认条目名为 ``$family-$enum-$entry``。
如果指定了 ``name-prefix``，它会替换条目名中 ``$family-$enum`` 的
部分。)

Boolean ``render-max`` controls creation of the max values
(which are enabled by default for attribute enums). These max
values are named ``__$pfx-MAX`` and ``$pfx-MAX``. The name
of the first value can be overridden via ``enum-cnt-name`` property.
(布尔值 ``render-max`` 控制是否创建最大值(对属性枚举默认启用)。
这些最大值命名为 ``__$pfx-MAX`` 和 ``$pfx-MAX``。第一个值的名称可以
通过 ``enum-cnt-name`` 属性覆盖。)

Attributes(属性)
================

Each attribute set (excluding fractional sets) is rendered as an enum.
(每个属性集(不含分数集)都渲染为一个枚举。)

Attribute enums are traditionally unnamed in netlink headers.
If naming is desired ``enum-name`` can be used to specify the name.
(按惯例，netlink 头文件中的属性枚举是匿名的。如果希望命名，可以用
``enum-name`` 指定名称。)

The default attribute name prefix is ``$family-A`` if the name of the set
is the same as the name of the family and ``$family-A-$set`` if the names
differ. The prefix can be overridden by the ``name-prefix`` property of a set.
The rest of the section will refer to the prefix as ``$pfx``.
(如果属性集与 family 同名，默认属性名前缀为 ``$family-A``；如果名称
不同，则为 ``$family-A-$set``。该前缀可以被属性集的 ``name-prefix``
属性覆盖。本节其余部分把这个前缀记作 ``$pfx``。)

Attributes are named ``$pfx-$attribute``.
(属性命名为 ``$pfx-$attribute``。)

Attribute enums end with two special values ``__$pfx-MAX`` and ``$pfx-MAX``
which are used for sizing attribute tables.
These two names can be specified directly with the ``attr-cnt-name``
and ``attr-max-name`` properties respectively.
(属性枚举以两个特殊值 ``__$pfx-MAX`` 和 ``$pfx-MAX`` 结尾，它们用于
确定属性表的大小。这两个名称可以分别通过 ``attr-cnt-name`` 和
``attr-max-name`` 属性直接指定。)

If ``max-by-define`` is set to ``true`` at the global level ``attr-max-name``
will be specified as a ``#define`` rather than an enum value.
(如果在全局层把 ``max-by-define`` 设为 ``true``，``attr-max-name``
将以 ``#define`` 而不是枚举值的形式指定。)

Operations(操作)
================

Operations are named ``$family-CMD-$operation``.
If ``name-prefix`` is specified it replaces the ``$family-CMD``
portion of the name.
(操作命名为 ``$family-CMD-$operation``。如果指定了 ``name-prefix``，
它会替换名称中 ``$family-CMD`` 的部分。)

Similarly to attribute enums operation enums end with special count and max
attributes. For operations those attributes can be renamed with
``cmd-cnt-name`` and ``cmd-max-name``. Max will be a define if ``max-by-define``
is ``true``.
(与属性枚举类似，操作枚举也以特殊的计数和最大值结尾。对于操作，这些
名称可以用 ``cmd-cnt-name`` 和 ``cmd-max-name`` 重命名。如果
``max-by-define`` 为 ``true``，最大值将是一个 define。)

Multicast groups(组播组)
========================

Each multicast group gets a define rendered into the kernel uAPI header.
The name of the define is ``$family-MCGRP-$group``, and can be overwritten
with the ``c-define-name`` property.
(每个组播组都会得到一个渲染进内核 uAPI 头文件的 define。该 define 的
名称为 ``$family-MCGRP-$group``，可以用 ``c-define-name`` 属性覆盖。)

Code generation(代码生成)
=========================

uAPI header is assumed to come from ``<linux/$family.h>`` in the default header
search path. It can be changed using the ``uapi-header`` global property.
(uAPI 头文件假定来自默认头文件搜索路径中的 ``<linux/$family.h>``。
可以用 ``uapi-header`` 全局属性修改。)
