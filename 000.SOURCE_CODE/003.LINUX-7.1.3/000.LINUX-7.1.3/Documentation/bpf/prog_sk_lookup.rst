.. SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)

=========================================
BPF sk_lookup program(BPF sk_lookup 程序)
=========================================

BPF sk_lookup program type (``BPF_PROG_TYPE_SK_LOOKUP``) introduces programmability
into the socket lookup performed by the transport layer when a packet is to be
delivered locally.
(BPF sk_lookup 程序类型(``BPF_PROG_TYPE_SK_LOOKUP``)在传输层把数据包
本地交付时执行的套接字查找中引入了可编程性。)

When invoked BPF sk_lookup program can select a socket that will receive the
incoming packet by calling the ``bpf_sk_assign()`` BPF helper function.
(被调用时，BPF sk_lookup 程序可以通过调用 ``bpf_sk_assign()`` BPF
辅助函数来选择接收传入数据包的套接字。)

Hooks for a common attach point (``BPF_SK_LOOKUP``) exist for both TCP and UDP.
(TCP 和 UDP 都存在公共挂载点(``BPF_SK_LOOKUP``)的钩子。)

Motivation(动机)
================

BPF sk_lookup program type was introduced to address setup scenarios where
binding sockets to an address with ``bind()`` socket call is impractical, such
as:
(引入 BPF sk_lookup 程序类型是为了应对用 ``bind()`` 套接字调用把套接字
绑定到地址并不切实际的场景，例如：)

1. receiving connections on a range of IP addresses, e.g. 192.0.2.0/24, when
   binding to a wildcard address ``INADRR_ANY`` is not possible due to a port
   conflict,
   (在一个 IP 地址范围内接收连接，例如 192.0.2.0/24，而由于端口冲突又
   无法绑定到通配地址 ``INADRR_ANY``；)
2. receiving connections on all or a wide range of ports, i.e. an L7 proxy use
   case.
   (在全部或很宽的端口范围内接收连接，即 L7 代理用例。)

Such setups would require creating and ``bind()``'ing one socket to each of the
IP address/port in the range, leading to resource consumption and potential
latency spikes during socket lookup.
(这样的设置需要为范围内的每个 IP 地址/端口创建并 ``bind()`` 一个
套接字，导致资源消耗以及套接字查找期间的潜在延迟尖峰。)

Attachment(挂载)
================

BPF sk_lookup program can be attached to a network namespace with
``bpf(BPF_LINK_CREATE, ...)`` syscall using the ``BPF_SK_LOOKUP`` attach type and a
netns FD as attachment ``target_fd``.
(可以使用 ``bpf(BPF_LINK_CREATE, ...)`` 系统调用把 BPF sk_lookup 程序
挂载到网络命名空间，使用 ``BPF_SK_LOOKUP`` 挂载类型和一个 netns FD
作为挂载 ``target_fd``。)

Multiple programs can be attached to one network namespace. Programs will be
invoked in the same order as they were attached.
(多个程序可以挂载到同一个网络命名空间。程序会按照它们挂载的顺序被
调用。)

Hooks(钩子)
===========

The attached BPF sk_lookup programs run whenever the transport layer needs to
find a listening (TCP) or an unconnected (UDP) socket for an incoming packet.
(只要传输层需要为传入数据包找到监听(TCP)或未连接(UDP)的套接字，
挂载的 BPF sk_lookup 程序就会运行。)

Incoming traffic to established (TCP) and connected (UDP) sockets is delivered
as usual without triggering the BPF sk_lookup hook.
(发往已建立(TCP)和已连接(UDP)套接字的流量照常交付，不会触发 BPF
sk_lookup 钩子。)

The attached BPF programs must return with either ``SK_PASS`` or ``SK_DROP``
verdict code. As for other BPF program types that are network filters,
``SK_PASS`` signifies that the socket lookup should continue on to regular
hashtable-based lookup, while ``SK_DROP`` causes the transport layer to drop the
packet.
(挂载的 BPF 程序必须以 ``SK_PASS`` 或 ``SK_DROP`` 判定码返回。与其他
作为网络过滤器的 BPF 程序类型一样，``SK_PASS`` 表示套接字查找应继续
进行常规的基于哈希表的查找，而 ``SK_DROP`` 会使传输层丢弃该数据包。)

A BPF sk_lookup program can also select a socket to receive the packet by
calling ``bpf_sk_assign()`` BPF helper. Typically, the program looks up a socket
in a map holding sockets, such as ``SOCKMAP`` or ``SOCKHASH``, and passes a
``struct bpf_sock *`` to ``bpf_sk_assign()`` helper to record the
selection. Selecting a socket only takes effect if the program has terminated
with ``SK_PASS`` code.
(BPF sk_lookup 程序还可以通过调用 ``bpf_sk_assign()`` BPF 辅助函数来
选择接收数据包的套接字。通常，程序在一个保存套接字的映射(如
``SOCKMAP`` 或 ``SOCKHASH``)中查找套接字，并把 ``struct bpf_sock *``
传给 ``bpf_sk_assign()`` 辅助函数以记录选择。只有程序以 ``SK_PASS``
码终止时，选择套接字才生效。)

When multiple programs are attached, the end result is determined from return
codes of all the programs according to the following rules:
(当挂载了多个程序时，最终结果根据以下规则由所有程序的返回码决定：)

1. If any program returned ``SK_PASS`` and selected a valid socket, the socket
   is used as the result of the socket lookup.
   (如果有任何程序返回 ``SK_PASS`` 且选择了一个有效套接字，则该套接字
   用作套接字查找的结果。)
2. If more than one program returned ``SK_PASS`` and selected a socket, the last
   selection takes effect.
   (如果有多个程序返回 ``SK_PASS`` 且选择了套接字，则最后一次选择
   生效。)
3. If any program returned ``SK_DROP``, and no program returned ``SK_PASS`` and
   selected a socket, socket lookup fails.
   (如果有程序返回 ``SK_DROP``，且没有程序返回 ``SK_PASS`` 并选择
   套接字，则套接字查找失败。)
4. If all programs returned ``SK_PASS`` and none of them selected a socket,
   socket lookup continues on.
   (如果所有程序都返回 ``SK_PASS`` 且没有一个选择套接字，则套接字
   查找继续进行。)

API(API)
========

In its context, an instance of ``struct bpf_sk_lookup``, BPF sk_lookup program
receives information about the packet that triggered the socket lookup. Namely:
(在上下文中，BPF sk_lookup 程序会收到一个 ``struct bpf_sk_lookup``
实例，其中包含触发套接字查找的数据包的信息，即：)

* IP version (``AF_INET`` or ``AF_INET6``),
  (IP 版本(``AF_INET`` 或 ``AF_INET6``)；)
* L4 protocol identifier (``IPPROTO_TCP`` or ``IPPROTO_UDP``),
  (L4 协议标识符(``IPPROTO_TCP`` 或 ``IPPROTO_UDP``)；)
* source and destination IP address,
  (源和目的 IP 地址；)
* source and destination L4 port,
  (源和目的 L4 端口；)
* the socket that has been selected with ``bpf_sk_assign()``.
  (用 ``bpf_sk_assign()`` 选择的套接字。)

Refer to ``struct bpf_sk_lookup`` declaration in ``linux/bpf.h`` user API
header, and `bpf-helpers(7)
<https://man7.org/linux/man-pages/man7/bpf-helpers.7.html>`_ man-page section
for ``bpf_sk_assign()`` for details.
(详情请参阅 ``linux/bpf.h`` 用户 API 头文件中 ``struct bpf_sk_lookup``
的声明，以及手册页中关于 ``bpf_sk_assign()`` 的 bpf-helpers(7) 章节。)

Example(示例)
=============

See ``tools/testing/selftests/bpf/prog_tests/sk_lookup.c`` for the reference
implementation.
(参考实现见 ``tools/testing/selftests/bpf/prog_tests/sk_lookup.c``。)
