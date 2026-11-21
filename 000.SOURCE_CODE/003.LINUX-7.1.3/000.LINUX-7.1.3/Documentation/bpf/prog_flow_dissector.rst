.. SPDX-License-Identifier: GPL-2.0

==================================================
BPF_PROG_TYPE_FLOW_DISSECTOR(BPF 流解析器程序类型)
==================================================

Overview(概述)
==============

Flow dissector is a routine that parses metadata out of the packets. It's
used in the various places in the networking subsystem (RFS, flow hash, etc).
(流解析器 (flow dissector) 是一个从数据包中解析出元数据的例程。它在网络
子系统的多个地方被使用（RFS、流哈希等）。)

BPF flow dissector is an attempt to reimplement C-based flow dissector logic
in BPF to gain all the benefits of BPF verifier (namely, limits on the
number of instructions and tail calls).
(BPF 流解析器试图用 BPF 重新实现基于 C 的流解析器逻辑，以获得 BPF 验证器的
所有好处（即指令数量和尾调用 (tail call) 数量的限制）。)

API(接口)
=========

BPF flow dissector programs operate on an ``__sk_buff``. However, only the
limited set of fields is allowed: ``data``, ``data_end`` and ``flow_keys``.
``flow_keys`` is ``struct bpf_flow_keys`` and contains flow dissector input
and output arguments.
(BPF 流解析器程序在 ``__sk_buff`` 上操作。但是，只允许有限的字段集合：
``data``、``data_end`` 和 ``flow_keys``。``flow_keys`` 是
``struct bpf_flow_keys``，包含流解析器的输入和输出参数。)

The inputs are:
(输入包括：)
  * ``nhoff`` - initial offset of the networking header
    (``nhoff`` - 网络层头的初始偏移)
  * ``thoff`` - initial offset of the transport header, initialized to nhoff
    (``thoff`` - 传输层头的初始偏移，初始化为 nhoff)
  * ``n_proto`` - L3 protocol type, parsed out of L2 header
    (``n_proto`` - L3 协议类型，从 L2 头解析得到)
  * ``flags`` - optional flags
    (``flags`` - 可选标志)

Flow dissector BPF program should fill out the rest of the ``struct
bpf_flow_keys`` fields. Input arguments ``nhoff/thoff/n_proto`` should be
also adjusted accordingly.
(流解析器 BPF 程序应填写 ``struct bpf_flow_keys`` 的其余字段。输入参数
``nhoff/thoff/n_proto`` 也应相应调整。)

The return code of the BPF program is either BPF_OK to indicate successful
dissection, or BPF_DROP to indicate parsing error.
(BPF 程序的返回码要么是 BPF_OK 表示解析成功，要么是 BPF_DROP 表示解析
错误。)

__sk_buff->data(原名)
=====================

In the VLAN-less case, this is what the initial state of the BPF flow
dissector looks like::

  +------+------+------------+-----------+
  | DMAC | SMAC | ETHER_TYPE | L3_HEADER |
  +------+------+------------+-----------+
                              ^
                              |
                              +-- flow dissector starts here

(在没有 VLAN 的情况下，BPF 流解析器的初始状态如下：)


.. code:: c

  skb->data + flow_keys->nhoff point to the first byte of L3_HEADER
  flow_keys->thoff = nhoff
  flow_keys->n_proto = ETHER_TYPE

In case of VLAN, flow dissector can be called with the two different states.
(在 VLAN 的情况下，流解析器可能以两种不同的状态被调用。)

Pre-VLAN parsing::

  +------+------+------+-----+-----------+-----------+
  | DMAC | SMAC | TPID | TCI |ETHER_TYPE | L3_HEADER |
  +------+------+------+-----+-----------+-----------+
                        ^
                        |
                        +-- flow dissector starts here

(VLAN 解析之前：)

.. code:: c

  skb->data + flow_keys->nhoff point the to first byte of TCI
  flow_keys->thoff = nhoff
  flow_keys->n_proto = TPID

Please note that TPID can be 802.1AD and, hence, BPF program would
have to parse VLAN information twice for double tagged packets.
(请注意，TPID 可能是 802.1AD，因此 BPF 程序将不得不对双标签 (double
tagged) 数据包解析两次 VLAN 信息。)


Post-VLAN parsing::

  +------+------+------+-----+-----------+-----------+
  | DMAC | SMAC | TPID | TCI |ETHER_TYPE | L3_HEADER |
  +------+------+------+-----+-----------+-----------+
                                          ^
                                          |
                                          +-- flow dissector starts here

(VLAN 解析之后：)

.. code:: c

  skb->data + flow_keys->nhoff point the to first byte of L3_HEADER
  flow_keys->thoff = nhoff
  flow_keys->n_proto = ETHER_TYPE

In this case VLAN information has been processed before the flow dissector
and BPF flow dissector is not required to handle it.
(在这种情况下，VLAN 信息已在流解析器之前被处理，BPF 流解析器不需要处理
它。)


The takeaway here is as follows: BPF flow dissector program can be called with
the optional VLAN header and should gracefully handle both cases: when single
or double VLAN is present and when it is not present. The same program
can be called for both cases and would have to be written carefully to
handle both cases.
(这里的要点是：BPF 流解析器程序可能带着可选的 VLAN 头被调用，应当优雅地
处理两种情况：存在单 VLAN 或双 VLAN 的情况，以及不存在 VLAN 的情况。同一
个程序会在两种情况下都被调用，因此必须小心编写以处理这两种情况。)


Flags(标志)
===========

``flow_keys->flags`` might contain optional input flags that work as follows:
(``flow_keys->flags`` 可能包含如下的可选输入标志：)

* ``BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG`` - tells BPF flow dissector to
  continue parsing first fragment; the default expected behavior is that
  flow dissector returns as soon as it finds out that the packet is fragmented;
  used by ``eth_get_headlen`` to estimate length of all headers for GRO.
  (``BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG`` - 告诉 BPF 流解析器继续解析
  第一个分片；默认的预期行为是，流解析器一旦发现数据包被分片就立即返回；
  ``eth_get_headlen`` 使用它来为 GRO 估算所有头的总长度。)
* ``BPF_FLOW_DISSECTOR_F_STOP_AT_FLOW_LABEL`` - tells BPF flow dissector to
  stop parsing as soon as it reaches IPv6 flow label; used by
  ``___skb_get_hash`` to get flow hash.
  (``BPF_FLOW_DISSECTOR_F_STOP_AT_FLOW_LABEL`` - 告诉 BPF 流解析器一旦
  到达 IPv6 流标签就停止解析；``___skb_get_hash`` 使用它来获取流哈希。)
* ``BPF_FLOW_DISSECTOR_F_STOP_AT_ENCAP`` - tells BPF flow dissector to stop
  parsing as soon as it reaches encapsulated headers; used by routing
  infrastructure.
  (``BPF_FLOW_DISSECTOR_F_STOP_AT_ENCAP`` - 告诉 BPF 流解析器一旦到达
  封装头就停止解析；路由基础设施使用它。)


Reference Implementation(参考实现)
==================================

See ``tools/testing/selftests/bpf/progs/bpf_flow.c`` for the reference
implementation and ``tools/testing/selftests/bpf/flow_dissector_load.[hc]``
for the loader. bpftool can be used to load BPF flow dissector program as well.
(参考实现参见 ``tools/testing/selftests/bpf/progs/bpf_flow.c``，加载器参见
``tools/testing/selftests/bpf/flow_dissector_load.[hc]``。bpftool 也可以
用来加载 BPF 流解析器程序。)

The reference implementation is organized as follows:
(参考实现的组织结构如下：)
  * ``jmp_table`` map that contains sub-programs for each supported L3 protocol
    (``jmp_table`` map，包含每个受支持 L3 协议的子程序)
  * ``_dissect`` routine - entry point; it does input ``n_proto`` parsing and
    does ``bpf_tail_call`` to the appropriate L3 handler
    (``_dissect`` 例程——入口点；它对输入 ``n_proto`` 进行解析，并通过
    ``bpf_tail_call`` 跳转到相应的 L3 处理程序)

Since BPF at this point doesn't support looping (or any jumping back),
jmp_table is used instead to handle multiple levels of encapsulation (and
IPv6 options).
(由于 BPF 目前不支持循环（或任何向后跳转），因此改用 jmp_table 来处理
多级封装（以及 IPv6 选项）。)


Current Limitations(当前限制)
=============================
BPF flow dissector doesn't support exporting all the metadata that in-kernel
C-based implementation can export. Notable example is single VLAN (802.1Q)
and double VLAN (802.1AD) tags. Please refer to the ``struct bpf_flow_keys``
for a set of information that's currently can be exported from the BPF context.
(BPF 流解析器不支持导出内核内基于 C 的实现所能导出的全部元数据。典型的
例子是单 VLAN (802.1Q) 和双 VLAN (802.1AD) 标签。当前可以从 BPF 上下文
导出的信息集合请参见 ``struct bpf_flow_keys``。)

When BPF flow dissector is attached to the root network namespace (machine-wide
policy), users can't override it in their child network namespaces.
(当 BPF 流解析器挂载到根网络命名空间（机器级策略）时，用户无法在其子网络
命名空间中覆盖它。)
