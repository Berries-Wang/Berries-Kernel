.. SPDX-License-Identifier: BSD-3-Clause

================================================================================
Netlink specification support for raw Netlink families(raw Netlink 族的规范支持)
================================================================================

This document describes the additional properties required by raw Netlink
families such as ``NETLINK_ROUTE`` which use the ``netlink-raw`` protocol
specification.
(本文档描述使用 ``netlink-raw`` 协议规范的 raw Netlink 族(如 ``NETLINK_ROUTE``)所需的附加属性。)

Specification(规范)
===================

The netlink-raw schema extends the :doc:`genetlink-legacy <genetlink-legacy>`
schema with properties that are needed to specify the protocol numbers and
multicast IDs used by raw netlink families. See :ref:`classic_netlink` for more
information. The raw netlink families also make use of type-specific
sub-messages.
(netlink-raw 模式在 :doc:`genetlink-legacy <genetlink-legacy>` 模式的基础上扩展了若干属性，用于指定 raw netlink 族所使用的协议号与多播 ID。更多信息见 :ref:`classic_netlink`。raw netlink 族还会使用类型特定的子消息。)

Globals(全局属性)
-----------------

protonum(协议号)
~~~~~~~~~~~~~~~~

The ``protonum`` property is used to specify the protocol number to use when
opening a netlink socket.
(``protonum`` 属性用于指定打开 netlink 套接字时使用的协议号。)

.. code-block:: yaml

  # SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause)

  name: rt-addr
  protocol: netlink-raw
  protonum: 0             # part of the NETLINK_ROUTE protocol


Multicast group properties(多播组属性)
--------------------------------------

value(值)
~~~~~~~~~

The ``value`` property is used to specify the group ID to use for multicast
group registration.
(``value`` 属性用于指定多播组注册所使用的组 ID。)

.. code-block:: yaml

  mcast-groups:
    list:
      -
        name: rtnlgrp-ipv4-ifaddr
        value: 5
      -
        name: rtnlgrp-ipv6-ifaddr
        value: 9
      -
        name: rtnlgrp-mctp-ifaddr
        value: 34

Sub-messages(子消息)
--------------------

Several raw netlink families such as
:ref:`rt-link<netlink-rt-link>` and
:ref:`tc<netlink-tc>` use attribute nesting as an
abstraction to carry module specific information.
(许多 raw netlink 族(如 :ref:`rt-link<netlink-rt-link>` 与 :ref:`tc<netlink-tc>`)使用属性嵌套作为承载模块特定信息的抽象。)

Conceptually it looks as follows::

    [OUTER NEST OR MESSAGE LEVEL]
      [GENERIC ATTR 1]
      [GENERIC ATTR 2]
      [GENERIC ATTR 3]
      [GENERIC ATTR - wrapper]
        [MODULE SPECIFIC ATTR 1]
        [MODULE SPECIFIC ATTR 2]

(概念上如下所示：)

The ``GENERIC ATTRs`` at the outer level are defined in the core (or rt_link or
core TC), while specific drivers, TC classifiers, qdiscs etc. can carry their
own information wrapped in the ``GENERIC ATTR - wrapper``. Even though the
example above shows attributes nesting inside the wrapper, the modules generally
have full freedom to define the format of the nest. In practice the payload of
the wrapper attr has very similar characteristics to a netlink message. It may
contain a fixed header / structure, netlink attributes, or both. Because of
those shared characteristics we refer to the payload of the wrapper attribute as
a sub-message.
(``GENERIC ATTRs`` 等外层属性定义在核心代码中(或 rt_link、核心 TC 中)，而具体的驱动、TC 分类器、qdisc 等则可以把各自的信息包装在 ``GENERIC ATTR - wrapper`` 中携带。尽管上面的示例展示的是属性嵌套在 wrapper 内部，但各模块通常拥有充分的自由来定义该嵌套(nest)的格式。实践中 wrapper 属性的载荷与 netlink 消息的特性非常相似：它可以包含固定头部/结构体、netlink 属性，或两者兼有。由于这些共同特性，我们把 wrapper 属性的载荷称为子消息(sub-message)。)

A sub-message attribute uses the value of another attribute as a selector key to
choose the right sub-message format. For example if the following attribute has
already been decoded:
(子消息属性使用另一个属性的值作为选择键(selector key)来挑选正确的子消息格式。例如，假设以下属性已被解码：)

.. code-block:: json

  { "kind": "gre" }

and we encounter the following attribute spec:
(并且我们遇到如下属性规范：)

.. code-block:: yaml

  -
    name: data
    type: sub-message
    sub-message: linkinfo-data-msg
    selector: kind

Then we look for a sub-message definition called ``linkinfo-data-msg`` and use
the value of the ``kind`` attribute i.e. ``gre`` as the key to choose the
correct format for the sub-message:
(随后我们查找名为 ``linkinfo-data-msg`` 的子消息定义，并使用 ``kind`` 属性的值(即 ``gre``)作为键来为子消息挑选正确的格式：)

.. code-block:: yaml

  sub-messages:
    name: linkinfo-data-msg
    formats:
      -
        value: bridge
        attribute-set: linkinfo-bridge-attrs
      -
        value: gre
        attribute-set: linkinfo-gre-attrs
      -
        value: geneve
        attribute-set: linkinfo-geneve-attrs

This would decode the attribute value as a sub-message with the attribute-set
called ``linkinfo-gre-attrs`` as the attribute space.
(这会把该属性值解码为一个子消息，并以名为 ``linkinfo-gre-attrs`` 的 attribute-set 作为其属性空间。)

A sub-message can have an optional ``fixed-header`` followed by zero or more
attributes from an ``attribute-set``. For example the following
``tc-options-msg`` sub-message defines message formats that use a mixture of
``fixed-header``, ``attribute-set`` or both together:
(子消息可以带一个可选的 ``fixed-header``，其后跟零个或多个来自 ``attribute-set`` 的属性。例如，下面这个 ``tc-options-msg`` 子消息定义了分别使用 ``fixed-header``、``attribute-set`` 或两者兼用的消息格式：)

.. code-block:: yaml

  sub-messages:
    -
      name: tc-options-msg
      formats:
        -
          value: bfifo
          fixed-header: tc-fifo-qopt
        -
          value: cake
          attribute-set: tc-cake-attrs
        -
          value: netem
          fixed-header: tc-netem-qopt
          attribute-set: tc-netem-attrs

Note that a selector attribute must appear in a netlink message before any
sub-message attributes that depend on it.
(注意：选择器属性必须出现在 netlink 消息中任何依赖它的子消息属性之前。)

If an attribute such as ``kind`` is defined at more than one nest level, then a
sub-message selector will be resolved using the value 'closest' to the selector.
For example, if the same attribute name is defined in a nested ``attribute-set``
alongside a sub-message selector and also in a top level ``attribute-set``, then
the selector will be resolved using the value 'closest' to the selector. If the
value is not present in the message at the same level as defined in the spec
then this is an error.
(如果诸如 ``kind`` 这样的属性在多个嵌套层级上都有定义，子消息选择器将使用距离选择器"最近"的值来解析。例如，若同一属性名既在与子消息选择器相邻的嵌套 ``attribute-set`` 中定义，又在顶层 ``attribute-set`` 中定义，则选择器将使用距离选择器"最近"的值来解析。如果该值未按规范定义的层级出现在消息中，则这是一个错误。)

Nested struct definitions(嵌套结构体定义)
-----------------------------------------

Many raw netlink families such as :ref:`tc<netlink-tc>`
make use of nested struct definitions. The ``netlink-raw`` schema makes it
possible to embed a struct within a struct definition using the ``struct``
property. For example, the following struct definition embeds the
``tc-ratespec`` struct definition for both the ``rate`` and the ``peakrate``
members of ``struct tc-tbf-qopt``.
(许多 raw netlink 族(如 :ref:`tc<netlink-tc>`)会使用嵌套结构体定义。``netlink-raw`` 模式支持使用 ``struct`` 属性把一个结构体嵌入到另一个结构体定义中。例如，下面的结构体定义为 ``struct tc-tbf-qopt`` 的 ``rate`` 与 ``peakrate`` 两个成员都嵌入了 ``tc-ratespec`` 结构体定义。)

.. code-block:: yaml

  -
    name: tc-tbf-qopt
    type: struct
    members:
      -
        name: rate
        type: binary
        struct: tc-ratespec
      -
        name: peakrate
        type: binary
        struct: tc-ratespec
      -
        name: limit
        type: u32
      -
        name: buffer
        type: u32
      -
        name: mtu
        type: u32
