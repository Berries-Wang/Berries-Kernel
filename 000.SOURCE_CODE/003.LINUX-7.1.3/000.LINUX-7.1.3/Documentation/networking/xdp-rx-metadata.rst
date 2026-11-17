.. SPDX-License-Identifier: GPL-2.0

==============================
XDP RX Metadata(XDP RX 元数据)
==============================

This document describes how an eXpress Data Path (XDP) program can access
hardware metadata related to a packet using a set of helper functions,
and how it can pass that metadata on to other consumers.
(本文档描述 eXpress Data Path (XDP) 程序如何使用一组辅助函数访问与数据包相关的硬件元
数据，以及它如何将这些元数据传递给其他使用者。)

General Design(总体设计)
========================

XDP has access to a set of kfuncs to manipulate the metadata in an XDP frame.
Every device driver that wishes to expose additional packet metadata can
implement these kfuncs. The set of kfuncs is declared in ``include/net/xdp.h``
via ``XDP_METADATA_KFUNC_xxx``.
(XDP 可以访问一组用于操作 XDP 帧中元数据的 kfunc。每个希望暴露额外数据包元数据的设备
驱动程序都可以实现这些 kfunc。这组 kfunc 在 ``include/net/xdp.h`` 中通过
``XDP_METADATA_KFUNC_xxx`` 声明。)

Currently, the following kfuncs are supported. In the future, as more
metadata is supported, this set will grow:
(目前支持以下 kfunc。将来随着更多元数据得到支持，这一集合还会增长：)

.. kernel-doc:: net/core/xdp.c
   :identifiers: bpf_xdp_metadata_rx_timestamp

.. kernel-doc:: net/core/xdp.c
   :identifiers: bpf_xdp_metadata_rx_hash

.. kernel-doc:: net/core/xdp.c
   :identifiers: bpf_xdp_metadata_rx_vlan_tag

An XDP program can use these kfuncs to read the metadata into stack
variables for its own consumption. Or, to pass the metadata on to other
consumers, an XDP program can store it into the metadata area carried
ahead of the packet. Not all packets will necessary have the requested
metadata available in which case the driver returns ``-ENODATA``.
(XDP 程序可以使用这些 kfunc 将元数据读取到栈变量中供自己使用。或者，为了将元数据传递
给其他使用者，XDP 程序可以将其存储到数据包前方携带的元数据区域中。并非所有数据包都
一定有所请求的元数据可用，在这种情况下驱动程序返回 ``-ENODATA``。)

Not all kfuncs have to be implemented by the device driver; when not
implemented, the default ones that return ``-EOPNOTSUPP`` will be used
to indicate the device driver have not implemented this kfunc.
(并非所有 kfunc 都必须由设备驱动程序实现；未实现时，将使用返回 ``-EOPNOTSUPP`` 的
默认实现，以表明设备驱动程序尚未实现该 kfunc。)


Within an XDP frame, the metadata layout (accessed via ``xdp_buff``) is
as follows::

  +----------+-----------------+------+
  | headroom | custom metadata | data |
  +----------+-----------------+------+
             ^                 ^
             |                 |
   xdp_buff->data_meta   xdp_buff->data

(在 XDP 帧内，元数据布局(通过 ``xdp_buff`` 访问)如下：)

An XDP program can store individual metadata items into this ``data_meta``
area in whichever format it chooses. Later consumers of the metadata
will have to agree on the format by some out of band contract (like for
the AF_XDP use case, see below).
(XDP 程序可以以自己选择的任意格式将各个元数据条目存储到这个 ``data_meta`` 区域中。
元数据的后续使用者必须通过某种带外约定就格式达成一致(例如 AF_XDP 用例，见下文)。)

AF_XDP(AF_XDP)
==============

:doc:`af_xdp` use-case implies that there is a contract between the BPF
program that redirects XDP frames into the ``AF_XDP`` socket (``XSK``) and
the final consumer. Thus the BPF program manually allocates a fixed number of
bytes out of metadata via ``bpf_xdp_adjust_meta`` and calls a subset
of kfuncs to populate it. The userspace ``XSK`` consumer computes
``xsk_umem__get_data() - METADATA_SIZE`` to locate that metadata.
Note, ``xsk_umem__get_data`` is defined in ``libxdp`` and
``METADATA_SIZE`` is an application-specific constant (``AF_XDP`` receive
descriptor does _not_ explicitly carry the size of the metadata).
(:doc:`af_xdp` 用例意味着，在将 XDP 帧重定向到 ``AF_XDP`` 套接字(``XSK``)的 BPF
程序与最终使用者之间存在一个约定。因此，BPF 程序通过 ``bpf_xdp_adjust_meta`` 手动
从元数据中分配固定数量的字节，并调用一部分 kfunc 来填充它。用户空间 ``XSK`` 使用者
计算 ``xsk_umem__get_data() - METADATA_SIZE`` 来定位该元数据。注意，
``xsk_umem__get_data`` 定义在 ``libxdp`` 中，而 ``METADATA_SIZE`` 是应用程序特定的
常量(``AF_XDP`` 接收描述符并不会显式携带元数据的大小)。)

Here is the ``AF_XDP`` consumer layout (note missing ``data_meta`` pointer)::

  +----------+-----------------+------+
  | headroom | custom metadata | data |
  +----------+-----------------+------+
                               ^
                               |
                        rx_desc->address

(下面是 ``AF_XDP`` 使用者的布局(注意缺少 ``data_meta`` 指针)：)

XDP_PASS(XDP_PASS)
==================

This is the path where the packets processed by the XDP program are passed
into the kernel. The kernel creates the ``skb`` out of the ``xdp_buff``
contents. Currently, every driver has custom kernel code to parse
the descriptors and populate ``skb`` metadata when doing this ``xdp_buff->skb``
conversion, and the XDP metadata is not used by the kernel when building
``skbs``. However, TC-BPF programs can access the XDP metadata area using
the ``data_meta`` pointer.
(这是被 XDP 程序处理过的数据包被传入内核的路径。内核使用 ``xdp_buff`` 的内容创建
``skb``。目前，每个驱动程序都有自定义的内核代码来解析描述符，并在进行
``xdp_buff->skb`` 转换时填充 ``skb`` 元数据，内核在构建 ``skbs`` 时并不使用 XDP
元数据。不过，TC-BPF 程序可以使用 ``data_meta`` 指针访问 XDP 元数据区域。)

In the future, we'd like to support a case where an XDP program
can override some of the metadata used for building ``skbs``.
(将来，我们希望支持这样一种场景：XDP 程序可以覆盖用于构建 ``skbs`` 的部分元数据。)

bpf_redirect_map(bpf_redirect_map)
==================================

``bpf_redirect_map`` can redirect the frame to a different device.
Some devices (like virtual ethernet links) support running a second XDP
program after the redirect. However, the final consumer doesn't have
access to the original hardware descriptor and can't access any of
the original metadata. The same applies to XDP programs installed
into devmaps and cpumaps.
(``bpf_redirect_map`` 可以将帧重定向到另一个设备。某些设备(例如虚拟以太网链路)支持
在重定向之后运行第二个 XDP 程序。然而，最终使用者无法访问原始硬件描述符，也就无法
访问任何原始元数据。对于安装到 devmap 和 cpumap 中的 XDP 程序，情况同样如此。)

This means that for redirected packets only custom metadata is
currently supported, which has to be prepared by the initial XDP program
before redirect. If the frame is eventually passed to the kernel, the
``skb`` created from such a frame won't have any hardware metadata populated
in its ``skb``. If such a packet is later redirected into an ``XSK``,
that will also only have access to the custom metadata.
(这意味着对于被重定向的数据包，目前仅支持自定义元数据，它必须由最初的 XDP 程序在
重定向之前准备好。如果该帧最终被传递给内核，由这样的帧创建的 ``skb`` 将不会填充任何
硬件元数据。如果这样的数据包稍后被重定向到 ``XSK``，同样也只能访问自定义元数据。)

bpf_tail_call(bpf_tail_call)
============================

Adding programs that access metadata kfuncs to the ``BPF_MAP_TYPE_PROG_ARRAY``
is currently not supported.
(目前不支持将访问元数据 kfunc 的程序添加到 ``BPF_MAP_TYPE_PROG_ARRAY`` 中。)

Supported Devices(受支持的设备)
===============================

It is possible to query which kfunc the particular netdev implements via
netlink. See ``xdp-rx-metadata-features`` attribute set in
``Documentation/netlink/specs/netdev.yaml``.
(可以通过 netlink 查询特定 netdev 实现了哪些 kfunc。参见
``Documentation/netlink/specs/netdev.yaml`` 中的 ``xdp-rx-metadata-features``
属性集。)

Driver Implementation(驱动程序实现)
===================================

Certain devices may prepend metadata to received packets. However, as of now,
``AF_XDP`` lacks the ability to communicate the size of the ``data_meta`` area
to the consumer. Therefore, it is the responsibility of the driver to copy any
device-reserved metadata out from the metadata area and ensure that
``xdp_buff->data_meta`` is pointing to ``xdp_buff->data`` before presenting the
frame to the XDP program. This is necessary so that, after the XDP program
adjusts the metadata area, the consumer can reliably retrieve the metadata
address using ``METADATA_SIZE`` offset.
(某些设备可能会在接收到的数据包前面附加元数据。然而，就目前而言，``AF_XDP`` 缺少向
使用者告知 ``data_meta`` 区域大小的能力。因此，驱动程序有责任把所有设备保留的元数据
从元数据区域中复制出来，并确保 ``xdp_buff->data_meta`` 指向 ``xdp_buff->data``，
然后再将该帧呈现给 XDP 程序。这样做是必要的，以便在 XDP 程序调整元数据区域之后，
使用者能够使用 ``METADATA_SIZE`` 偏移量可靠地取回元数据地址。)

The following diagram shows how custom metadata is positioned relative to the
packet data and how pointers are adjusted for metadata access::

              |<-- bpf_xdp_adjust_meta(xdp_buff, -METADATA_SIZE) --|
  new xdp_buff->data_meta                              old xdp_buff->data_meta
              |                                                    |
              |                                            xdp_buff->data
              |                                                    |
   +----------+----------------------------------------------------+------+
   | headroom |                  custom metadata                   | data |
   +----------+----------------------------------------------------+------+
              |                                                    |
              |                                            xdp_desc->addr
              |<------ xsk_umem__get_data() - METADATA_SIZE -------|

(下图展示了自定义元数据相对于数据包数据的位置，以及为访问元数据而进行的指针调整：)

``bpf_xdp_adjust_meta`` ensures that ``METADATA_SIZE`` is aligned to 4 bytes,
does not exceed 252 bytes, and leaves sufficient space for building the
xdp_frame. If these conditions are not met, it returns a negative error. In this
case, the BPF program should not proceed to populate data into the ``data_meta``
area.
(``bpf_xdp_adjust_meta`` 确保 ``METADATA_SIZE`` 按 4 字节对齐、不超过 252 字节，并为
构建 xdp_frame 留出足够的空间。如果不满足这些条件，它会返回一个负数错误。在这种情况下，
BPF 程序不应继续向 ``data_meta`` 区域填充数据。)

Example(示例)
=============

See ``tools/testing/selftests/bpf/progs/xdp_metadata.c`` and
``tools/testing/selftests/bpf/prog_tests/xdp_metadata.c`` for an example of
BPF program that handles XDP metadata.
(处理 XDP 元数据的 BPF 程序示例参见
``tools/testing/selftests/bpf/progs/xdp_metadata.c`` 和
``tools/testing/selftests/bpf/prog_tests/xdp_metadata.c``。)
