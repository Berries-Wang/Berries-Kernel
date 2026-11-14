=========================================
rpcsec_gss support for kernel RPC servers
=========================================

(内核 RPC 服务器的 rpcsec_gss 支持)

This document gives references to the standards and protocols used to
implement RPCGSS authentication in kernel RPC servers such as the NFS
server and the NFS client's NFSv4.0 callback server.  (But note that
NFSv4.1 and higher don't require the client to act as a server for the
purposes of authentication.)
(本文档给出在内核 RPC 服务器(例如 NFS 服务器以及 NFS 客户端的 NFSv4.0 回调服务器)中实现 RPCGSS 认证所使用的标准和协议的参考。(但请注意，NFSv4.1 及更高版本不要求客户端为了认证目的而充当服务器。))

RPCGSS is specified in a few IETF documents:
(RPCGSS 由几份 IETF 文档规定：)

 - RFC2203 v1: https://tools.ietf.org/rfc/rfc2203.txt
 - RFC5403 v2: https://tools.ietf.org/rfc/rfc5403.txt

There is a third version that we don't currently implement:
(还有第三个版本，我们目前尚未实现：)

 - RFC7861 v3: https://tools.ietf.org/rfc/rfc7861.txt

Background
(背景)
==========

The RPCGSS Authentication method describes a way to perform GSSAPI
Authentication for NFS.  Although GSSAPI is itself completely mechanism
agnostic, in many cases only the KRB5 mechanism is supported by NFS
implementations.
(RPCGSS 认证方法描述了为 NFS 执行 GSSAPI 认证的一种方式。尽管 GSSAPI 本身完全与具体机制无关，但在许多情况下，NFS 实现只支持 KRB5 机制。)

The Linux kernel, at the moment, supports only the KRB5 mechanism, and
depends on GSSAPI extensions that are KRB5 specific.
(Linux 内核目前只支持 KRB5 机制，并依赖于 KRB5 特有的 GSSAPI 扩展。)

GSSAPI is a complex library, and implementing it completely in kernel is
unwarranted. However GSSAPI operations are fundamentally separable in 2
parts:
(GSSAPI 是一个复杂的库，在内核中完整实现它并不值得。不过，GSSAPI 操作在根本上可以分为两部分：)

- initial context establishment
  (初始上下文建立。)
- integrity/privacy protection (signing and encrypting of individual
  packets)
  (完整性/隐私保护(对单个数据包进行签名和加密)。)

The former is more complex and policy-independent, but less
performance-sensitive.  The latter is simpler and needs to be very fast.
(前者更复杂且与策略无关，但对性能不那么敏感。后者更简单，且需要非常快。)

Therefore, we perform per-packet integrity and privacy protection in the
kernel, but leave the initial context establishment to userspace.  We
need upcalls to request userspace to perform context establishment.
(因此，我们在内核中执行每数据包的完整性和隐私保护，而把初始上下文建立留给用户空间。我们需要通过上行调用(upcall)请求用户空间执行上下文建立。)

NFS Server Legacy Upcall Mechanism
(NFS 服务器旧式上行调用机制)
==================================

The classic upcall mechanism uses a custom text based upcall mechanism
to talk to a custom daemon called rpc.svcgssd that is provide by the
nfs-utils package.
(经典的上行调用机制使用一种自定义的基于文本的上行调用机制，与一个名为 rpc.svcgssd 的自定义守护进程通信，该守护进程由 nfs-utils 软件包提供。)

This upcall mechanism has 2 limitations:
(该上行调用机制有 2 个局限：)

A) It can handle tokens that are no bigger than 2KiB
(A) 它只能处理不超过 2KiB 的令牌。)

In some Kerberos deployment GSSAPI tokens can be quite big, up and
beyond 64KiB in size due to various authorization extensions attacked to
the Kerberos tickets, that needs to be sent through the GSS layer in
order to perform context establishment.
(在某些 Kerberos 部署中，由于附加在 Kerberos 票据上的各种授权扩展，GSSAPI 令牌可能非常大，达到甚至超过 64KiB，而这些令牌需要通过 GSS 层发送才能执行上下文建立。)

B) It does not properly handle creds where the user is member of more
than a few thousand groups (the current hard limit in the kernel is 65K
groups) due to limitation on the size of the buffer that can be send
back to the kernel (4KiB).
(B) 由于能发回内核的缓冲区大小受限(4KiB)，它无法正确处理用户所属组数超过几千个的 creds(内核中当前的硬性上限为 65K 个组)。)

NFS Server New RPC Upcall Mechanism
(NFS 服务器新式 RPC 上行调用机制)
===================================

The newer upcall mechanism uses RPC over a unix socket to a daemon
called gss-proxy, implemented by a userspace program called Gssproxy.
(较新的上行调用机制使用通过 unix 套接字的 RPC 与一个名为 gss-proxy 的守护进程通信，该守护进程由一个名为 Gssproxy 的用户空间程序实现。)

The gss_proxy RPC protocol is currently documented `here
<https://fedorahosted.org/gss-proxy/wiki/ProtocolDocumentation>`_.
(gss_proxy RPC 协议目前记载于 `这里 <https://fedorahosted.org/gss-proxy/wiki/ProtocolDocumentation>`_。)

This upcall mechanism uses the kernel rpc client and connects to the gssproxy
userspace program over a regular unix socket. The gssproxy protocol does not
suffer from the size limitations of the legacy protocol.
(该上行调用机制使用内核 rpc 客户端，通过普通的 unix 套接字连接到 gssproxy 用户空间程序。gssproxy 协议不受旧式协议大小限制的困扰。)

Negotiating Upcall Mechanisms
(协商上行调用机制)
=============================

To provide backward compatibility, the kernel defaults to using the
legacy mechanism.  To switch to the new mechanism, gss-proxy must bind
to /var/run/gssproxy.sock and then write "1" to
/proc/net/rpc/use-gss-proxy.  If gss-proxy dies, it must repeat both
steps.
(为了提供向后兼容性，内核默认使用旧式机制。要切换到新机制，gss-proxy 必须绑定到 /var/run/gssproxy.sock，然后向 /proc/net/rpc/use-gss-proxy 写入 "1"。如果 gss-proxy 死掉，它必须重复这两个步骤。)

Once the upcall mechanism is chosen, it cannot be changed.  To prevent
locking into the legacy mechanisms, the above steps must be performed
before starting nfsd.  Whoever starts nfsd can guarantee this by reading
from /proc/net/rpc/use-gss-proxy and checking that it contains a
"1"--the read will block until gss-proxy has done its write to the file.
(一旦选定了上行调用机制，就无法再更改。为避免被锁定在旧式机制上，上述步骤必须在启动 nfsd 之前完成。启动 nfsd 的人可以通过读取 /proc/net/rpc/use-gss-proxy 并检查其内容是否包含 "1" 来保证这一点——该读取会一直阻塞，直到 gss-proxy 完成对该文件的写入。)
