.. SPDX-License-Identifier: GPL-2.0

=============
Devlink DPIPE
=============

(Devlink DPIPE)

Background
(背景)
==========

While performing the hardware offloading process, much of the hardware
specifics cannot be presented. These details are useful for debugging, and
``devlink-dpipe`` provides a standardized way to provide visibility into the
offloading process.
(在执行硬件卸载的过程中，许多硬件细节无法呈现。这些细节对调试很有用，而 ``devlink-dpipe`` 提供了一种标准化的方式来查看卸载过程。)

For example, the routing longest prefix match (LPM) algorithm used by the
Linux kernel may differ from the hardware implementation. The pipeline debug
API (DPIPE) is aimed at providing the user visibility into the ASIC's
pipeline in a generic way.
(例如，Linux 内核所使用的路由最长前缀匹配 (LPM) 算法可能与硬件实现不同。管线调试 API (DPIPE) 旨在以通用的方式让用户能够查看 ASIC 的管线。)

The hardware offload process is expected to be done in a way that the user
should not be able to distinguish between the hardware vs. software
implementation. In this process, hardware specifics are neglected. In
reality those details can have lots of meaning and should be exposed in some
standard way.
(硬件卸载过程应当做到让用户无法区分硬件实现与软件实现。在此过程中，硬件细节被忽略了。而实际上这些细节可能意义重大，应当以某种标准方式加以暴露。)

This problem is made even more complex when one wishes to offload the
control path of the whole networking stack to a switch ASIC. Due to
differences in the hardware and software models some processes cannot be
represented correctly.
(当人们希望把整个网络栈的控制路径卸载到交换 ASIC 时，这个问题变得更加复杂。由于硬件模型与软件模型之间的差异，某些过程无法被正确呈现。)

One example is the kernel's LPM algorithm which in many cases differs
greatly to the hardware implementation. The configuration API is the same,
but one cannot rely on the Forward Information Base (FIB) to look like the
Level Path Compression trie (LPC-trie) in hardware.
(一个例子是内核的 LPM 算法，它在很多情况下与硬件实现大不相同。配置 API 是相同的，但不能指望转发信息库 (FIB) 在硬件中呈现为与软件中一样的层级路径压缩 trie (LPC-trie)。)

In many situations trying to analyze systems failure solely based on the
kernel's dump may not be enough. By combining this data with complementary
information about the underlying hardware, this debugging can be made
easier; additionally, the information can be useful when debugging
performance issues.
(在很多情况下，仅凭内核的转储来分析系统故障可能并不足够。把这些数据与底层硬件的补充信息相结合，可以使调试更加容易；此外，这些信息在调试性能问题时也很有用。)

Overview
(概述)
========

The ``devlink-dpipe`` interface closes this gap. The hardware's pipeline is
modeled as a graph of match/action tables. Each table represents a specific
hardware block. This model is not new, first being used by the P4 language.
(``devlink-dpipe`` 接口弥补了这一空白。硬件管线被建模为一张由匹配/动作表构成的图。每个表代表一个特定的硬件模块。这个模型并不新鲜，最早由 P4 语言使用。)

Traditionally it has been used as an alternative model for hardware
configuration, but the ``devlink-dpipe`` interface uses it for visibility
purposes as a standard complementary tool. The system's view from
``devlink-dpipe`` should change according to the changes done by the
standard configuration tools.
(传统上，它被用作硬件配置的另一种模型，但 ``devlink-dpipe`` 接口把它用于可视化目的，作为一项标准的补充工具。``devlink-dpipe`` 所呈现的系统视图应当随标准配置工具所做的更改而相应变化。)

For example, it’s quite common to  implement Access Control Lists (ACL)
using Ternary Content Addressable Memory (TCAM). The TCAM memory can be
divided into TCAM regions. Complex TC filters can have multiple rules with
different priorities and different lookup keys. On the other hand hardware
TCAM regions have a predefined lookup key. Offloading the TC filter rules
using TCAM engine can result in multiple TCAM regions being interconnected
in a chain (which may affect the data path latency). In response to a new TC
filter new tables should be created describing those regions.
(例如，用三态内容寻址存储器 (TCAM) 实现访问控制列表 (ACL) 相当常见。TCAM 存储器可以划分为多个 TCAM 区域 (region)。复杂的 TC 过滤器可以拥有多条规则，具有不同的优先级和不同的查找键。另一方面，硬件 TCAM 区域具有预定义的查找键。使用 TCAM 引擎卸载 TC 过滤器规则可能导致多个 TCAM 区域以链式方式互连（这可能影响数据路径时延）。为了响应新的 TC 过滤器，应当创建描述这些区域的新表。)

Model
(模型)
=====

The ``DPIPE`` model introduces several objects:
(``DPIPE`` 模型引入了几个对象：)

  * headers
    (头)
  * tables
    (表)
  * entries
    (表项)

A ``header`` describes packet formats and provides names for fields within
the packet. A ``table`` describes hardware blocks. An ``entry`` describes
the actual content of a specific table.
(一个 ``header`` 描述数据包格式并为包内字段提供名称。一个 ``table`` 描述硬件模块。一个 ``entry`` 描述某个表的实际内容。)

The hardware pipeline is not port specific, but rather describes the whole
ASIC. Thus it is tied to the top of the ``devlink`` infrastructure.
(硬件管线不是特定于端口的，而是描述整个 ASIC。因此它挂在 ``devlink`` 基础设施的顶层。)

Drivers can register and unregister tables at run time, in order to support
dynamic behavior. This dynamic behavior is mandatory for describing hardware
blocks like TCAM regions which can be allocated and freed dynamically.
(驱动可以在运行时注册和注销表，以支持动态行为。这种动态行为对于描述可以动态分配和释放的硬件模块（如 TCAM 区域）是必需的。)

``devlink-dpipe`` generally is not intended for configuration. The exception
is hardware counting for a specific table.
(``devlink-dpipe`` 一般不用于配置。唯一的例外是针对特定表的硬件计数。)

The following commands are used to obtain the ``dpipe`` objects from
userspace:
(以下命令用于从用户态获取 ``dpipe`` 对象：)

  * ``table_get``: Receive a table's description.
    (``table_get``：接收一个表的描述。)
  * ``headers_get``: Receive a device's supported headers.
    (``headers_get``：接收设备支持的头的列表。)
  * ``entries_get``: Receive a table's current entries.
    (``entries_get``：接收一个表当前的表项。)
  * ``counters_set``: Enable or disable counters on a table.
    (``counters_set``：启用或禁用一个表上的计数器。)

Table
(表)
-----

The driver should implement the following operations for each table:
(驱动应为每个表实现以下操作：)

  * ``matches_dump``: Dump the supported matches.
    (``matches_dump``：转储支持的匹配。)
  * ``actions_dump``: Dump the supported actions.
    (``actions_dump``：转储支持的动作。)
  * ``entries_dump``: Dump the actual content of the table.
    (``entries_dump``：转储表的实际内容。)
  * ``counters_set_update``: Synchronize hardware with counters enabled or
    disabled.
    (``counters_set_update``：在启用或禁用计数器时与硬件同步。)

Header/Field
(头/字段)
------------

In a similar way to P4 headers and fields are used to describe a table's
behavior. There is a slight difference between the standard protocol headers
and specific ASIC metadata. The protocol headers should be declared in the
``devlink`` core API. On the other hand ASIC meta data is driver specific
and should be defined in the driver. Additionally, each driver-specific
devlink documentation file should document the driver-specific ``dpipe``
headers it implements. The headers and fields are identified by enumeration.
(与 P4 类似，头和字段用于描述表的行为。标准协议头与特定 ASIC 元数据之间存在细微差别。协议头应在 ``devlink`` 核心 API 中声明。而 ASIC 元数据是驱动特定的，应在驱动中定义。此外，每个驱动特定的 devlink 文档文件都应记录它所实现的驱动特定 ``dpipe`` 头。头和字段通过枚举值来标识。)

In order to provide further visibility some ASIC metadata fields could be
mapped to kernel objects. For example, internal router interface indexes can
be directly mapped to the net device ifindex. FIB table indexes used by
different Virtual Routing and Forwarding (VRF) tables can be mapped to
internal routing table indexes.
(为了提供进一步的可见性，一些 ASIC 元数据字段可以映射到内核对象。例如，内部路由器接口索引可以直接映射到网络设备的 ifindex。不同虚拟路由转发 (VRF) 表所使用的 FIB 表索引可以映射到内部路由表索引。)

Match
(匹配)
-----

Matches are kept primitive and close to hardware operation. Match types like
LPM are not supported due to the fact that this is exactly a process we wish
to describe in full detail. Example of matches:
(匹配保持原始形态并贴近硬件操作。像 LPM 这样的匹配类型不受支持，因为这恰恰是我们希望以完整细节加以描述的过程。匹配的示例：)

  * ``field_exact``: Exact match on a specific field.
    (``field_exact``：对特定字段进行精确匹配。)
  * ``field_exact_mask``: Exact match on a specific field after masking.
    (``field_exact_mask``：掩码后对特定字段进行精确匹配。)
  * ``field_range``: Match on a specific range.
    (``field_range``：对特定范围进行匹配。)

The id's of the header and the field should be specified in order to
identify the specific field. Furthermore, the header index should be
specified in order to distinguish multiple headers of the same type in a
packet (tunneling).
(应指定头和字段的 id 以标识特定字段。此外，还应指定头索引，以区分同一数据包中同一类型的多个头（隧道场景）。)

Action
(动作)
------

Similar to match, the actions are kept primitive and close to hardware
operation. For example:
(与匹配类似，动作保持原始形态并贴近硬件操作。例如：)

  * ``field_modify``: Modify the field value.
    (``field_modify``：修改字段值。)
  * ``field_inc``: Increment the field value.
    (``field_inc``：递增字段值。)
  * ``push_header``: Add a header.
    (``push_header``：添加一个头。)
  * ``pop_header``: Remove a header.
    (``pop_header``：移除一个头。)

Entry
(表项)
-----

Entries of a specific table can be dumped on demand. Each eentry is
identified with an index and its properties are described by a list of
match/action values and specific counter. By dumping the tables content the
interactions between tables can be resolved.
(特定表的表项可以按需转储。每个 eentry（表项）由一个索引标识，其属性由一组匹配/动作值和特定的计数器描述。通过转储表的内容，可以解析出各表之间的交互。)

Abstraction Example
(抽象示例)
===================

The following is an example of the abstraction model of the L3 part of
Mellanox Spectrum ASIC. The blocks are described in the order they appear in
the pipeline. The table sizes in the following examples are not real
hardware sizes and are provided for demonstration purposes.
(以下是以 Mellanox Spectrum ASIC 的 L3 部分为例的抽象模型。各模块按其在管线中出现的顺序描述。下例中的表大小并非真实的硬件大小，仅用于演示。)

LPM
(LPM)
---

The LPM algorithm can be implemented as a list of hash tables. Each hash
table contains routes with the same prefix length. The root of the list is
/32, and in case of a miss the hardware will continue to the next hash
table. The depth of the search will affect the data path latency.
(LPM 算法可以实现为一系列哈希表。每个哈希表包含具有相同前缀长度的路由。链表的根是 /32，一旦未命中，硬件将继续到下一个哈希表。搜索深度会影响数据路径时延。)

In case of a hit the entry contains information about the next stage of the
pipeline which resolves the MAC address. The next stage can be either local
host table for directly connected routes, or adjacency table for next-hops.
The ``meta.lpm_prefix`` field is used to connect two LPM tables.
(命中时，表项包含关于管线下一阶段（负责解析 MAC 地址）的信息。下一阶段既可以是用于直连路由的本地主机表，也可以是用于下一跳的邻接表。``meta.lpm_prefix`` 字段用于连接两个 LPM 表。)

.. code::

    table lpm_prefix_16 {
      size: 4096,
      counters_enabled: true,
      match: { meta.vr_id: exact,
               ipv4.dst_addr: exact_mask,
               ipv6.dst_addr: exact_mask,
               meta.lpm_prefix: exact },
      action: { meta.adj_index: set,
                meta.adj_group_size: set,
                meta.rif_port: set,
                meta.lpm_prefix: set },
    }

Local Host
(本地主机)
----------

In the case of local routes the LPM lookup already resolves the egress
router interface (RIF), yet the exact MAC address is not known. The local
host table is a hash table combining the output interface id with
destination IP address as a key. The result is the MAC address.
(对于本地路由，LPM 查找已经解析出出口路由器接口 (RIF)，但确切的 MAC 地址尚不可知。本地主机表是一个哈希表，它把输出接口 id 与目的 IP 地址组合起来作为键，结果就是 MAC 地址。)

.. code::

    table local_host {
      size: 4096,
      counters_enabled: true,
      match: { meta.rif_port: exact,
               ipv4.dst_addr: exact},
      action: { ethernet.daddr: set }
    }

Adjacency
(邻接)
---------

In case of remote routes this table does the ECMP. The LPM lookup results in
ECMP group size and index that serves as a global offset into this table.
Concurrently a hash of the packet is generated. Based on the ECMP group size
and the packet's hash a local offset is generated. Multiple LPM entries can
point to the same adjacency group.
(对于远程路由，该表执行 ECMP。LPM 查找的结果是 ECMP 组大小和索引，索引充当进入该表的全局偏移。与此同时会生成数据包的哈希。基于 ECMP 组大小和数据包哈希生成一个局部偏移。多个 LPM 表项可以指向同一个邻接组。)

.. code::

    table adjacency {
      size: 4096,
      counters_enabled: true,
      match: { meta.adj_index: exact,
               meta.adj_group_size: exact,
               meta.packet_hash_index: exact },
      action: { ethernet.daddr: set,
                meta.erif: set }
    }

ERIF
(ERIF)
----

In case the egress RIF and destination MAC have been resolved by previous
tables this table does multiple operations like TTL decrease and MTU check.
Then the decision of forward/drop is taken and the port L3 statistics are
updated based on the packet's type (broadcast, unicast, multicast).
(当出口 RIF 和目的 MAC 已由前面的表解析出来后，该表执行多项操作，例如 TTL 递减和 MTU 检查。然后做出转发/丢弃的决定，并根据数据包类型（广播、单播、组播）更新端口 L3 统计信息。)

.. code::

    table erif {
      size: 800,
      counters_enabled: true,
      match: { meta.rif_port: exact,
               meta.is_l3_unicast: exact,
               meta.is_l3_broadcast: exact,
               meta.is_l3_multicast, exact },
      action: { meta.l3_drop: set,
                meta.l3_forward: set }
    }
