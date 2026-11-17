.. SPDX-License-Identifier: GPL-2.0

===============
NIC SR-IOV APIs
===============

(NIC SR-IOV API)

Modern NICs are strongly encouraged to focus on implementing the ``switchdev``
model (see :ref:`switchdev`) to configure forwarding and security of SR-IOV
functionality.
(强烈建议现代 NIC 专注于实现 ``switchdev`` 模型（参见 :ref:`switchdev`）来配置
SR-IOV 功能的转发与安全。)

Legacy API(传统 API)
====================

The old SR-IOV API is implemented in ``rtnetlink`` Netlink family as part of
the ``RTM_GETLINK`` and ``RTM_SETLINK`` commands. On the driver side
it consists of a number of ``ndo_set_vf_*`` and ``ndo_get_vf_*`` callbacks.
(旧的 SR-IOV API 在 ``rtnetlink`` Netlink 族中实现，是 ``RTM_GETLINK`` 和
``RTM_SETLINK`` 命令的一部分。在驱动侧，它由若干 ``ndo_set_vf_*`` 和
``ndo_get_vf_*`` 回调组成。)

Since the legacy APIs do not integrate well with the rest of the stack
the API is considered frozen; no new functionality or extensions
will be accepted. New drivers should not implement the uncommon callbacks;
namely the following callbacks are off limits:
(由于传统 API 与协议栈其余部分集成不佳，该 API 被视为已冻结；不再接受任何新
功能或扩展。新驱动不应实现那些不常用的回调，即以下回调禁止使用：)

 - ``ndo_get_vf_port``
 - ``ndo_set_vf_port``
 - ``ndo_set_vf_rss_query_en``
