.. SPDX-License-Identifier: BSD-3-Clause

.. _kernel_netlink:

==================================================================
Netlink notes for kernel developers(面向内核开发者的 Netlink 说明)
==================================================================

General guidance(通用指南)
==========================

Attribute enums(属性枚举)
-------------------------

Older families often define "null" attributes and commands with value
of ``0`` and named ``unspec``. This is supported (``type: unused``)
but should be avoided in new families. The ``unspec`` enum values are
not used in practice, so just set the value of the first attribute to ``1``.
(较旧的 family 常常定义值为 ``0``、名为 ``unspec`` 的"空"属性和命令。
这是受支持的(``type: unused``)，但在新 family 中应避免这种做法。
``unspec`` 枚举值在实践中并不使用，所以直接把第一个属性的值设为
``1`` 即可。)

Message enums(消息枚举)
-----------------------

Use the same command IDs for requests and replies. This makes it easier
to match them up, and we have plenty of ID space.
(对请求和应答使用相同的命令 ID。这样更容易把它们匹配起来，而且 ID
空间非常充裕。)

Use separate command IDs for notifications. This makes it easier to
sort the notifications from replies (and present them to the user
application via a different API than replies).
(为通知使用单独的命令 ID。这样更容易把通知从应答中区分出来(并能
通过与应答不同的 API 呈现给用户应用)。)

Answer requests(应答请求)
-------------------------

Older families do not reply to all of the commands, especially NEW / ADD
commands. User only gets information whether the operation succeeded or
not via the ACK. Try to find useful data to return. Once the command is
added whether it replies with a full message or only an ACK is uAPI and
cannot be changed. It's better to err on the side of replying.
(较旧的 family 并不对所有命令都应答，尤其是 NEW / ADD 命令。用户只能
通过 ACK 得知操作是否成功。尽量找到有用的数据来返回。一旦命令加入，
它是以完整消息应答还是只回 ACK 就属于 uAPI，无法更改。宁可多应答，
也不要漏应答。)

Specifically NEW and ADD commands should reply with information identifying
the created object such as the allocated object's ID (without having to
resort to using ``NLM_F_ECHO``).
(具体来说，NEW 和 ADD 命令应以能标识所创建对象的信息来应答，例如所
分配对象的 ID(而不必借助 ``NLM_F_ECHO``)。)

NLM_F_ECHO(NLM_F_ECHO)
----------------------

Make sure to pass the request info to genl_notify() to allow ``NLM_F_ECHO``
to take effect.  This is useful for programs that need precise feedback
from the kernel (for example for logging purposes).
(确保把请求信息传给 genl_notify()，以使 ``NLM_F_ECHO`` 生效。这对
需要从内核获得精确反馈的程序(例如用于日志记录)很有用。)

Support dump consistency(支持 dump 一致性)
------------------------------------------

If iterating over objects during dump may skip over objects or repeat
them - make sure to report dump inconsistency with ``NLM_F_DUMP_INTR``.
This is usually implemented by maintaining a generation id for the
structure and recording it in the ``seq`` member of struct netlink_callback.
(如果在 dump 期间遍历对象可能跳过或重复某些对象——务必用
``NLM_F_DUMP_INTR`` 报告 dump 不一致。这通常通过为该结构维护一个代际
编号(generation id)，并把它记录在 struct netlink_callback 的 ``seq``
成员中来实现。)

Netlink specification(Netlink 规范)
===================================

Documentation of the Netlink specification parts which are only relevant
to the kernel space.
(Netlink 规范中仅与内核空间相关部分的文档。)

Globals(全局属性)
-----------------

kernel-policy(kernel-policy)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Defines whether the kernel validation policy is ``global`` i.e. the same for all
operations of the family, defined for each operation individually - ``per-op``,
or separately for each operation and operation type (do vs dump) - ``split``.
New families should use ``per-op`` (default) to be able to narrow down the
attributes accepted by a specific command.
(定义内核校验策略是 ``global`` (即对 family 的所有操作都相同)、为每个
操作单独定义——``per-op``，还是为每个操作及操作类型(do 与 dump)分别
定义——``split`` 。新 family 应使用 ``per-op`` (默认值)，以便收窄特定
命令所接受的属性。)

checks(检查项)
--------------

Documentation for the ``checks`` sub-sections of attribute specs.
(属性 spec 中 ``checks`` 子节的文档。)

unterminated-ok(允许未终止)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Accept strings without the null-termination (for legacy families only).
Switches from the ``NLA_NUL_STRING`` to ``NLA_STRING`` policy type.
(接受不以 null 结尾的字符串(仅适用于较旧的 family)。把策略类型从
``NLA_NUL_STRING`` 切换为 ``NLA_STRING``。)

max-len(max-len)
~~~~~~~~~~~~~~~~

Defines max length for a binary or string attribute (corresponding
to the ``len`` member of struct nla_policy). For string attributes terminating
null character is not counted towards ``max-len``.
(为二进制或字符串属性定义最大长度(对应 struct nla_policy 的 ``len``
成员)。对字符串属性，结尾的 null 字符不计入 ``max-len``。)

The field may either be a literal integer value or a name of a defined
constant. String types may reduce the constant by one
(i.e. specify ``max-len: CONST - 1``) to reserve space for the terminating
character so implementations should recognize such pattern.
(该字段可以是一个字面整数值，也可以是某个已定义常量的名称。字符串
类型可以把常量减一(即指定 ``max-len: CONST - 1``)来为结尾字符保留
空间，因此实现应当能够识别这种模式。)

min-len(min-len)
~~~~~~~~~~~~~~~~

Similar to ``max-len`` but defines minimum length.
(与 ``max-len`` 类似，但定义的是最小长度。)
