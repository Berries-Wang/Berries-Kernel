.. SPDX-License-Identifier: GPL-2.0

=============================================
Open vSwitch datapath developer documentation
=============================================

(Open vSwitch 数据路径开发者文档)

The Open vSwitch kernel module allows flexible userspace control over
flow-level packet processing on selected network devices.  It can be
used to implement a plain Ethernet switch, network device bonding,
VLAN processing, network access control, flow-based network control,
and so on.
(Open vSwitch 内核模块允许用户空间对选定网络设备上的流级数据包处理进行灵活控制。它可用于实现普通以太网交换机、网络设备绑定、VLAN 处理、网络访问控制、基于流的网络控制等。)

The kernel module implements multiple "datapaths" (analogous to
bridges), each of which can have multiple "vports" (analogous to ports
within a bridge).  Each datapath also has associated with it a "flow
table" that userspace populates with "flows" that map from keys based
on packet headers and metadata to sets of actions.  The most common
action forwards the packet to another vport; other actions are also
implemented.
(该内核模块实现多个 "datapath"（数据路径，类似于网桥），每个数据路径可以拥有多个 "vport"（虚拟端口，类似于网桥内的端口）。每个数据路径还关联一个 "flow table"（流表），用户空间在其中填充 "flow"（流），流将基于数据包报头和元数据构成的键映射到一组动作。最常见的动作是将数据包转发到另一个 vport；此外也实现了其他动作。)

When a packet arrives on a vport, the kernel module processes it by
extracting its flow key and looking it up in the flow table.  If there
is a matching flow, it executes the associated actions.  If there is
no match, it queues the packet to userspace for processing (as part of
its processing, userspace will likely set up a flow to handle further
packets of the same type entirely in-kernel).
(当数据包到达某个 vport 时，内核模块会提取其流键并在流表中查找。如果存在匹配的流，则执行相关联的动作。如果没有匹配，则将数据包排入用户空间进行处理（作为处理的一部分，用户空间很可能会设置一个流，使同一类型的后续数据包完全在内核中处理）。)


Flow key compatibility
(流键兼容性)
----------------------

Network protocols evolve over time.  New protocols become important
and existing protocols lose their prominence.  For the Open vSwitch
kernel module to remain relevant, it must be possible for newer
versions to parse additional protocols as part of the flow key.  It
might even be desirable, someday, to drop support for parsing
protocols that have become obsolete.  Therefore, the Netlink interface
to Open vSwitch is designed to allow carefully written userspace
applications to work with any version of the flow key, past or future.
(网络协议会随时间演进。新协议变得重要，已有协议则逐渐失去地位。为了让 Open vSwitch 内核模块保持可用，必须允许较新的版本在流键中解析更多协议。甚至可能有朝一日希望放弃对已过时协议的解析支持。因此，Open vSwitch 的 Netlink 接口被设计为允许编写严谨的用户空间应用程序与过去或未来任何版本的流键协同工作。)

To support this forward and backward compatibility, whenever the
kernel module passes a packet to userspace, it also passes along the
flow key that it parsed from the packet.  Userspace then extracts its
own notion of a flow key from the packet and compares it against the
kernel-provided version:
(为了支持这种向前和向后兼容，每当内核模块把数据包传递给用户空间时，也会一并提供它从数据包中解析出的流键。用户空间随后从数据包中提取自己理解的流键，并与内核提供的版本进行比较：)

    - If userspace's notion of the flow key for the packet matches the
      kernel's, then nothing special is necessary.
      (如果用户空间对该数据包流键的理解与内核一致，则无需任何特殊处理。)

    - If the kernel's flow key includes more fields than the userspace
      version of the flow key, for example if the kernel decoded IPv6
      headers but userspace stopped at the Ethernet type (because it
      does not understand IPv6), then again nothing special is
      necessary.  Userspace can still set up a flow in the usual way,
      as long as it uses the kernel-provided flow key to do it.
      (如果内核的流键比用户空间版本的流键包含更多字段，例如内核解析了 IPv6
      报头而用户空间止步于以太网类型（因为它不理解 IPv6），那么同样无需特殊
      处理。用户空间仍可按常规方式设置流，只要使用内核提供的流键即可。)

    - If the userspace flow key includes more fields than the
      kernel's, for example if userspace decoded an IPv6 header but
      the kernel stopped at the Ethernet type, then userspace can
      forward the packet manually, without setting up a flow in the
      kernel.  This case is bad for performance because every packet
      that the kernel considers part of the flow must go to userspace,
      but the forwarding behavior is correct.  (If userspace can
      determine that the values of the extra fields would not affect
      forwarding behavior, then it could set up a flow anyway.)
      (如果用户空间的流键比内核的流键包含更多字段，例如用户空间解析了 IPv6
      报头而内核止步于以太网类型，那么用户空间可以手动转发数据包，而不在内核
      中设置流。这种情况对性能不利，因为内核认为属于该流的每个数据包都必须送到
      用户空间，但转发行为是正确的。（如果用户空间能够确定这些额外字段的值不会
      影响转发行为，那么它仍然可以设置流。）)

How flow keys evolve over time is important to making this work, so
the following sections go into detail.
(流键如何随时间演进对实现这一点至关重要，因此以下各节将详细说明。)


Flow key format
(流键格式)
---------------

A flow key is passed over a Netlink socket as a sequence of Netlink
attributes.  Some attributes represent packet metadata, defined as any
information about a packet that cannot be extracted from the packet
itself, e.g. the vport on which the packet was received.  Most
attributes, however, are extracted from headers within the packet,
e.g. source and destination addresses from Ethernet, IP, or TCP
headers.
(流键以一系列 Netlink 属性的形式通过 Netlink 套接字传递。某些属性表示数据包元数据，其定义为无法从数据包本身提取的关于数据包的任何信息，例如接收数据包的 vport。不过大多数属性是从数据包内部的报头中提取的，例如来自以太网、IP 或 TCP 报头的源地址和目的地址。)

The <linux/openvswitch.h> header file defines the exact format of the
flow key attributes.  For informal explanatory purposes here, we write
them as comma-separated strings, with parentheses indicating arguments
and nesting.  For example, the following could represent a flow key
corresponding to a TCP packet that arrived on vport 1::
(<linux/openvswitch.h> 头文件定义了流键属性的精确格式。为便于这里的非正式说明，我们将其写作以逗号分隔的字符串，用括号表示参数和嵌套。例如，下面的内容可以表示对应于到达 vport 1 的 TCP 数据包的流键：)

    in_port(1), eth(src=e0:91:f5:21:d0:b2, dst=00:02:e3:0f:80:a4),
    eth_type(0x0800), ipv4(src=172.16.0.20, dst=172.18.0.52, proto=17, tos=0,
    frag=no), tcp(src=49163, dst=80)

Often we ellipsize arguments not important to the discussion, e.g.::
(我们常常省略与讨论无关的参数，例如：)

    in_port(1), eth(...), eth_type(0x0800), ipv4(...), tcp(...)


Wildcarded flow key format
(通配流键格式)
--------------------------

A wildcarded flow is described with two sequences of Netlink attributes
passed over the Netlink socket. A flow key, exactly as described above, and an
optional corresponding flow mask.
(通配流由通过 Netlink 套接字传递的两组 Netlink 属性序列来描述：一个与上文所述完全相同的流键，以及一个可选的对应流掩码。)

A wildcarded flow can represent a group of exact match flows. Each '1' bit
in the mask specifies a exact match with the corresponding bit in the flow key.
A '0' bit specifies a don't care bit, which will match either a '1' or '0' bit
of a incoming packet. Using wildcarded flow can improve the flow set up rate
by reduce the number of new flows need to be processed by the user space program.
(一个通配流可以表示一组精确匹配流。掩码中的每个 '1' 位表示与流键中对应位精确
匹配；'0' 位表示不关心的位，可以匹配到来数据包中的 '1' 或 '0' 位。使用通配流可以
减少需要由用户空间程序处理的新流数量，从而提高流的建立速率。)

Support for the mask Netlink attribute is optional for both the kernel and user
space program. The kernel can ignore the mask attribute, installing an exact
match flow, or reduce the number of don't care bits in the kernel to less than
what was specified by the user space program. In this case, variations in bits
that the kernel does not implement will simply result in additional flow setups.
The kernel module will also work with user space programs that neither support
nor supply flow mask attributes.
(对掩码 Netlink 属性的支持对内核和用户空间程序来说都是可选的。内核可以忽略掩码
属性而安装精确匹配流，也可以把内核中不关心位的数量减少到少于用户空间程序指定的
数量。在这种情况下，内核未实现的位上的变化只会导致额外的流建立。内核模块也可以
与既不支持也不提供流掩码属性的用户空间程序协同工作。)

Since the kernel may ignore or modify wildcard bits, it can be difficult for
the userspace program to know exactly what matches are installed. There are
two possible approaches: reactively install flows as they miss the kernel
flow table (and therefore not attempt to determine wildcard changes at all)
or use the kernel's response messages to determine the installed wildcards.
(由于内核可能忽略或修改通配位，用户空间程序可能难以确切知道安装了哪些匹配。
有两种可行的方法：在流未命中内核流表时被动式地安装流（因此完全不试图确定通配位
的变化），或者利用内核的响应消息来确定已安装的通配位。)

When interacting with userspace, the kernel should maintain the match portion
of the key exactly as originally installed. This will provides a handle to
identify the flow for all future operations. However, when reporting the
mask of an installed flow, the mask should include any restrictions imposed
by the kernel.
(在与用户空间交互时，内核应保持键的匹配部分与最初安装时完全一致。这为后续的
所有操作提供了标识该流的句柄。不过，在报告已安装流的掩码时，掩码应包含内核
施加的所有限制。)

The behavior when using overlapping wildcarded flows is undefined. It is the
responsibility of the user space program to ensure that any incoming packet
can match at most one flow, wildcarded or not. The current implementation
performs best-effort detection of overlapping wildcarded flows and may reject
some but not all of them. However, this behavior may change in future versions.
(使用相互重叠的通配流时的行为是未定义的。确保任何到来数据包最多只能匹配一个流
（无论是否为通配流）是用户空间程序的责任。当前实现会尽力检测相互重叠的通配流，
并可能拒绝其中一部分（而非全部）。不过，这一行为在未来的版本中可能会改变。)


Unique flow identifiers
(唯一流标识符)
-----------------------

An alternative to using the original match portion of a key as the handle for
flow identification is a unique flow identifier, or "UFID". UFIDs are optional
for both the kernel and user space program.
(除了使用键中原有的匹配部分作为流标识的句柄之外，另一种选择是唯一流标识符，
即 "UFID"。UFID 对内核和用户空间程序来说都是可选的。)

User space programs that support UFID are expected to provide it during flow
setup in addition to the flow, then refer to the flow using the UFID for all
future operations. The kernel is not required to index flows by the original
flow key if a UFID is specified.
(支持 UFID 的用户空间程序应在设置流时除流本身之外一并提供 UFID，之后的全部
操作都使用 UFID 来引用该流。如果指定了 UFID，内核不要求按原始流键对流建立
索引。)


Basic rule for evolving flow keys
(流键演进的基本规则)
---------------------------------

Some care is needed to really maintain forward and backward
compatibility for applications that follow the rules listed under
"Flow key compatibility" above.
(要真正为遵循上文 "Flow key compatibility"（流键兼容性）一节所列规则的
应用程序保持向前和向后兼容，还需要一些细心处理。)

The basic rule is obvious::
(基本规则显而易见：)

    ==================================================================
    New network protocol support must only supplement existing flow
    key attributes.  It must not change the meaning of already defined
    flow key attributes.
    ==================================================================

This rule does have less-obvious consequences so it is worth working
through a few examples.  Suppose, for example, that the kernel module
did not already implement VLAN parsing.  Instead, it just interpreted
the 802.1Q TPID (0x8100) as the Ethertype then stopped parsing the
packet.  The flow key for any packet with an 802.1Q header would look
essentially like this, ignoring metadata::
(这条规则确实有一些不那么明显的推论，因此值得通过几个例子来推敲。例如，假设
内核模块尚未实现 VLAN 解析，而是把 802.1Q TPID (0x8100) 当作 Ethertype 然后
停止解析数据包。那么任何带有 802.1Q 报头的数据包的流键（忽略元数据）基本上
会像这样：)

    eth(...), eth_type(0x8100)

Naively, to add VLAN support, it makes sense to add a new "vlan" flow
key attribute to contain the VLAN tag, then continue to decode the
encapsulated headers beyond the VLAN tag using the existing field
definitions.  With this change, a TCP packet in VLAN 10 would have a
flow key much like this::
(按照朴素的想法，要增加 VLAN 支持，似乎应该添加一个新的 "vlan" 流键属性来
包含 VLAN 标签，然后使用现有的字段定义继续解码 VLAN 标签之后的封装报头。
有了这一改动，VLAN 10 中的 TCP 数据包的流键会非常像这样：)

    eth(...), vlan(vid=10, pcp=0), eth_type(0x0800), ip(proto=6, ...), tcp(...)

But this change would negatively affect a userspace application that
has not been updated to understand the new "vlan" flow key attribute.
The application could, following the flow compatibility rules above,
ignore the "vlan" attribute that it does not understand and therefore
assume that the flow contained IP packets.  This is a bad assumption
(the flow only contains IP packets if one parses and skips over the
802.1Q header) and it could cause the application's behavior to change
across kernel versions even though it follows the compatibility rules.
(但这一改动会对尚未更新以理解新 "vlan" 流键属性的用户空间应用程序产生负面
影响。依照上述流兼容规则，该应用程序可能会忽略它不理解的 "vlan" 属性，进而
假定该流包含的是 IP 数据包。这是一个糟糕的假设（只有解析并跳过 802.1Q 报头，
流中包含的才是 IP 数据包），而且即使应用程序遵循兼容规则，也可能导致其行为
在不同内核版本间发生变化。)

The solution is to use a set of nested attributes.  This is, for
example, why 802.1Q support uses nested attributes.  A TCP packet in
VLAN 10 is actually expressed as::
(解决办法是使用一组嵌套属性。例如，802.1Q 支持之所以使用嵌套属性，原因就在
于此。VLAN 10 中的 TCP 数据包实际上表示为：)

    eth(...), eth_type(0x8100), vlan(vid=10, pcp=0), encap(eth_type(0x0800),
    ip(proto=6, ...), tcp(...)))

Notice how the "eth_type", "ip", and "tcp" flow key attributes are
nested inside the "encap" attribute.  Thus, an application that does
not understand the "vlan" key will not see either of those attributes
and therefore will not misinterpret them.  (Also, the outer eth_type
is still 0x8100, not changed to 0x0800.)
(注意 "eth_type"、"ip" 和 "tcp" 流键属性是如何嵌套在 "encap" 属性内部的。
这样，不理解 "vlan" 键的应用程序将看不到这些属性中的任何一个，因此不会误解
它们。（此外，外层 eth_type 仍为 0x8100，不会改为 0x0800。）)

Handling malformed packets
(处理畸形数据包)
--------------------------

Don't drop packets in the kernel for malformed protocol headers, bad
checksums, etc.  This would prevent userspace from implementing a
simple Ethernet switch that forwards every packet.
(不要因为协议报头畸形、校验和错误等原因而在内核中丢弃数据包。否则用户空间
将无法实现一个转发所有数据包的简单以太网交换机。)

Instead, in such a case, include an attribute with "empty" content.
It doesn't matter if the empty content could be valid protocol values,
as long as those values are rarely seen in practice, because userspace
can always forward all packets with those values to userspace and
handle them individually.
(在这种情况下，应当改为包含一个内容为"空"的属性。即使这个空内容可能对应
合法的协议值也没有关系，只要这些值在实践中很少出现即可，因为用户空间总可以
把所有具有这些值的数据包转发到用户空间并逐个处理。)

For example, consider a packet that contains an IP header that
indicates protocol 6 for TCP, but which is truncated just after the IP
header, so that the TCP header is missing.  The flow key for this
packet would include a tcp attribute with all-zero src and dst, like
this::
(例如，考虑一个包含 IP 报头、指示协议 6（TCP），但在 IP 报头之后即被截断
从而缺少 TCP 报头的数据包。该数据包的流键会包含一个 src 和 dst 全为零的
tcp 属性，如下所示：)

    eth(...), eth_type(0x0800), ip(proto=6, ...), tcp(src=0, dst=0)

As another example, consider a packet with an Ethernet type of 0x8100,
indicating that a VLAN TCI should follow, but which is truncated just
after the Ethernet type.  The flow key for this packet would include
an all-zero-bits vlan and an empty encap attribute, like this::
(再举一例，考虑一个以太网类型为 0x8100、表示其后应跟有 VLAN TCI，但在
以太网类型之后即被截断的数据包。该数据包的流键会包含一个全零位的 vlan
属性和一个空的 encap 属性，如下所示：)

    eth(...), eth_type(0x8100), vlan(0), encap()

Unlike a TCP packet with source and destination ports 0, an
all-zero-bits VLAN TCI is not that rare, so the CFI bit (aka
VLAN_TAG_PRESENT inside the kernel) is ordinarily set in a vlan
attribute expressly to allow this situation to be distinguished.
Thus, the flow key in this second example unambiguously indicates a
missing or malformed VLAN TCI.
(与源端口和目的端口为 0 的 TCP 数据包不同，全零位的 VLAN TCI 并不罕见，
因此 vlan 属性中通常特意设置 CFI 位（在内核内称为 VLAN_TAG_PRESENT），
以便能够区分这种情况。因此，第二个例子中的流键明确指示 VLAN TCI 缺失或
畸形。)

Other rules
(其他规则)
-----------

The other rules for flow keys are much less subtle:
(关于流键的其他规则则直白得多：)

    - Duplicate attributes are not allowed at a given nesting level.
      (在给定的嵌套层级中不允许出现重复属性。)

    - Ordering of attributes is not significant.
      (属性的顺序无关紧要。)

    - When the kernel sends a given flow key to userspace, it always
      composes it the same way.  This allows userspace to hash and
      compare entire flow keys that it may not be able to fully
      interpret.
      (当内核向用户空间发送某个给定的流键时，它总是以相同的方式组合该流键。
      这使得用户空间能够对可能无法完全解读的整个流键进行哈希和比较。)
