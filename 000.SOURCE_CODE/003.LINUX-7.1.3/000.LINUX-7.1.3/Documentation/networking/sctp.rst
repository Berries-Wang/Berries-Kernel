.. SPDX-License-Identifier: GPL-2.0

=================
Linux Kernel SCTP
=================

(Linux 内核 SCTP)

This is the current BETA release of the Linux Kernel SCTP reference
implementation.
(这是 Linux 内核 SCTP 参考实现的当前 BETA 版本。)

SCTP (Stream Control Transmission Protocol) is a IP based, message oriented,
reliable transport protocol, with congestion control, support for
transparent multi-homing, and multiple ordered streams of messages.
RFC2960 defines the core protocol.  The IETF SIGTRAN working group originally
developed the SCTP protocol and later handed the protocol over to the
Transport Area (TSVWG) working group for the continued evolvement of SCTP as a
general purpose transport.
(SCTP(流控制传输协议，Stream Control Transmission Protocol)是一种基于 IP 的、面向消息的可靠传输协议，具有拥塞控制、透明多归属 (multi-homing) 支持以及多条有序消息流。RFC2960 定义了核心协议。IETF SIGTRAN 工作组最初开发了 SCTP 协议，后来把该协议移交给传输领域 (Transport Area, TSVWG) 工作组，以便将其作为通用传输协议持续演进。)

See the IETF website (http://www.ietf.org) for further documents on SCTP.
See http://www.ietf.org/rfc/rfc2960.txt
(更多关于 SCTP 的文档请参见 IETF 网站 (http://www.ietf.org)。参见 http://www.ietf.org/rfc/rfc2960.txt)

The initial project goal is to create an Linux kernel reference implementation
of SCTP that is RFC 2960 compliant and provides an programming interface
referred to as the  UDP-style API of the Sockets Extensions for SCTP, as
proposed in IETF Internet-Drafts.
(项目的最初目标是创建一个符合 RFC 2960 的 Linux 内核 SCTP 参考实现，并提供一个被称为 SCTP 套接字扩展的 UDP 风格 API 的编程接口，如 IETF Internet-Draft 所提议的那样。)

Caveats
=======
(注意事项)

- lksctp can be built as statically or as a module.  However, be aware that
  module removal of lksctp is not yet a safe activity.
  (lksctp 可以被编译为静态内建，也可以编译为模块。但请注意，卸载 lksctp 模块目前还不是一件安全的事情。)

- There is tentative support for IPv6, but most work has gone towards
  implementation and testing lksctp on IPv4.
  (目前对 IPv6 只有初步支持，大部分工作都投入在 IPv4 上对 lksctp 的实现和测试。)


For more information, please visit the lksctp project website:
(更多信息请访问 lksctp 项目网站：)

   http://www.sf.net/projects/lksctp

Or contact the lksctp developers through the mailing list:
(或通过邮件列表联系 lksctp 开发者：)

   <linux-sctp@vger.kernel.org>
