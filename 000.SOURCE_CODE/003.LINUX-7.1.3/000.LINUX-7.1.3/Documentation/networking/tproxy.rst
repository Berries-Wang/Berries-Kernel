.. SPDX-License-Identifier: GPL-2.0

=========================
Transparent proxy support
=========================

(透明代理支持)

This feature adds Linux 2.2-like transparent proxy support to current kernels.
To use it, enable the socket match and the TPROXY target in your kernel config.
You will need policy routing too, so be sure to enable that as well.
(该特性为当前内核添加了类似 Linux 2.2 的透明代理支持。要使用它，请在内核
配置中启用 socket 匹配与 TPROXY 目标。你还需要策略路由，因此也请务必启用
它。)

From Linux 4.18 transparent proxy support is also available in nf_tables.
(从 Linux 4.18 起，nf_tables 中也提供了透明代理支持。)

1. Making non-local sockets work
(1. 让非本地套接字工作)
================================

The idea is that you identify packets with destination address matching a local
socket on your box, set the packet mark to a certain value::
(其思路是：识别出目的地址与你机器上某个本地套接字匹配的数据包，并把
数据包标记设置为特定值：)

    # iptables -t mangle -N DIVERT
    # iptables -t mangle -A PREROUTING -p tcp -m socket --transparent -j DIVERT
    # iptables -t mangle -A DIVERT -j MARK --set-mark 1
    # iptables -t mangle -A DIVERT -j ACCEPT

Alternatively you can do this in nft with the following commands::
(或者，你可以在 nft 中用以下命令完成同样的事情：)

    # nft add table filter
    # nft add chain filter divert "{ type filter hook prerouting priority -150; }"
    # nft add rule filter divert meta l4proto tcp socket transparent 1 meta mark set 1 accept

And then match on that value using policy routing to have those packets
delivered locally::
(然后使用策略路由匹配该值，使这些数据包被本地交付：)

    # ip rule add fwmark 1 lookup 100
    # ip route add local 0.0.0.0/0 dev lo table 100

Because of certain restrictions in the IPv4 routing output code you'll have to
modify your application to allow it to send datagrams _from_ non-local IP
addresses. All you have to do is enable the (SOL_IP, IP_TRANSPARENT) socket
option before calling bind::
(由于 IPv4 路由输出代码中的某些限制，你必须修改你的应用程序，使其允许
从非本地 IP 地址发送数据报。你要做的只是在调用 bind 之前启用
(SOL_IP, IP_TRANSPARENT) 套接字选项：)

    fd = socket(AF_INET, SOCK_STREAM, 0);
    /* - 8< -*/
    int value = 1;
    setsockopt(fd, SOL_IP, IP_TRANSPARENT, &value, sizeof(value));
    /* - 8< -*/
    name.sin_family = AF_INET;
    name.sin_port = htons(0xCAFE);
    name.sin_addr.s_addr = htonl(0xDEADBEEF);
    bind(fd, &name, sizeof(name));

A trivial patch for netcat is available here:
http://people.netfilter.org/hidden/tproxy/netcat-ip_transparent-support.patch
(netcat 的一个简单补丁可在这里获取：
http://people.netfilter.org/hidden/tproxy/netcat-ip_transparent-support.patch)


2. Redirecting traffic
(2. 重定向流量)
======================

Transparent proxying often involves "intercepting" traffic on a router. This is
usually done with the iptables REDIRECT target; however, there are serious
limitations of that method. One of the major issues is that it actually
modifies the packets to change the destination address -- which might not be
acceptable in certain situations. (Think of proxying UDP for example: you won't
be able to find out the original destination address. Even in case of TCP
getting the original destination address is racy.)
(透明代理通常涉及在路由器上 "拦截" 流量。这通常用 iptables REDIRECT 目标
完成；然而该方法有严重的局限。主要问题之一是它实际上会修改数据包以改变
目的地址——在某些情况下这可能不可接受。(以代理 UDP 为例：你将无法得知
原始目的地址。即使是 TCP，获取原始目的地址也存在竞态问题。))

The 'TPROXY' target provides similar functionality without relying on NAT. Simply
add rules like this to the iptables ruleset above::
('TPROXY' 目标提供了类似的功能，而不依赖 NAT。只需把如下规则添加到上面的
iptables 规则集中：)

    # iptables -t mangle -A PREROUTING -p tcp --dport 80 -j TPROXY \
      --tproxy-mark 0x1/0x1 --on-port 50080

Or the following rule to nft::
(或者向 nft 添加以下规则：)

    # nft add rule filter divert tcp dport 80 tproxy to :50080 meta mark set 1 accept

Note that for this to work you'll have to modify the proxy to enable (SOL_IP,
IP_TRANSPARENT) for the listening socket.
(注意，要使其正常工作，你必须修改代理，为监听套接字启用
(SOL_IP, IP_TRANSPARENT)。)

As an example implementation, tcprdr is available here:
https://git.breakpoint.cc/cgit/fw/tcprdr.git/
This tool is written by Florian Westphal and it was used for testing during the
nf_tables implementation.
(作为一个示例实现，tcprdr 可在这里获取：
https://git.breakpoint.cc/cgit/fw/tcprdr.git/
该工具由 Florian Westphal 编写，在 nf_tables 实现期间曾被用于测试。)

3. Iptables and nf_tables extensions
(3. Iptables 与 nf_tables 扩展)
====================================

To use tproxy you'll need to have the following modules compiled for iptables:
(要使用 tproxy，你需要为 iptables 编译以下模块：)

 - NETFILTER_XT_MATCH_SOCKET
 - NETFILTER_XT_TARGET_TPROXY

Or the floowing modules for nf_tables:
(或者为 nf_tables 编译以下模块：)

 - NFT_SOCKET
 - NFT_TPROXY

4. Application support
(4. 应用程序支持)
======================

4.1. Squid
----------

Squid 3.HEAD has support built-in. To use it, pass
'--enable-linux-netfilter' to configure and set the 'tproxy' option on
the HTTP listener you redirect traffic to with the TPROXY iptables
target.
(Squid 3.HEAD 已内置支持。要使用它，向 configure 传递
'--enable-linux-netfilter'，并在你用 TPROXY iptables 目标把流量重定向到
的那个 HTTP 监听器上设置 'tproxy' 选项。)

For more information please consult the following page on the Squid
wiki: http://wiki.squid-cache.org/Features/Tproxy4
(更多信息请参阅 Squid wiki 上的以下页面：
http://wiki.squid-cache.org/Features/Tproxy4)
