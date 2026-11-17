.. SPDX-License-Identifier: GPL-2.0

===============================================
prestera devlink support(prestera devlink 支持)
===============================================

This document describes the devlink features implemented by the ``prestera``
device driver.
(本文档描述 ``prestera`` 设备驱动程序所实现的 devlink 功能。)

Driver-specific Traps(驱动特定陷阱(trap))
=========================================

.. list-table:: List of Driver-specific Traps Registered by ``prestera``
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
.. list-table:: List of Driver-specific Traps Registered by ``prestera``
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``arp_bc``
     - ``trap``
     - Traps ARP broadcast packets (both requests/responses)
       (捕获 ARP 广播数据包(包括请求/响应))
   * - ``is_is``
     - ``trap``
     - Traps IS-IS packets
       (捕获 IS-IS 数据包)
   * - ``ospf``
     - ``trap``
     - Traps OSPF packets
       (捕获 OSPF 数据包)
   * - ``ip_bc_mac``
     - ``trap``
     - Traps IPv4 packets with broadcast DA Mac address
       (捕获带有广播目的 MAC 地址的 IPv4 数据包)
   * - ``stp``
     - ``trap``
     - Traps STP BPDU
       (捕获 STP BPDU)
   * - ``lacp``
     - ``trap``
     - Traps LACP packets
       (捕获 LACP 数据包)
   * - ``lldp``
     - ``trap``
     - Traps LLDP packets
       (捕获 LLDP 数据包)
   * - ``router_mc``
     - ``trap``
     - Traps multicast packets
       (捕获组播数据包)
   * - ``vrrp``
     - ``trap``
     - Traps VRRP packets
       (捕获 VRRP 数据包)
   * - ``dhcp``
     - ``trap``
     - Traps DHCP packets
       (捕获 DHCP 数据包)
   * - ``mtu_error``
     - ``trap``
     - Traps (exception) packets that exceeded port's MTU
       (捕获(异常)超过端口 MTU 的数据包)
   * - ``mac_to_me``
     - ``trap``
     -  Traps packets with switch-port's DA Mac address
        (捕获带有交换机端口目的 MAC 地址的数据包)
   * - ``ttl_error``
     - ``trap``
     - Traps (exception) IPv4 packets whose TTL exceeded
       (捕获(异常)TTL 超限的 IPv4 数据包)
   * - ``ipv4_options``
     - ``trap``
     - Traps (exception) packets due to the malformed IPV4 header options
       (由于 IPV4 头选项格式错误而捕获(异常)数据包)
   * - ``ip_default_route``
     - ``trap``
     - Traps packets that have no specific IP interface (IP to me) and no forwarding prefix
       (捕获没有特定 IP 接口(IP to me)也没有转发前缀的数据包)
   * - ``local_route``
     - ``trap``
     - Traps packets that have been send to one of switch IP interfaces addresses
       (捕获已被发送到交换机某个 IP 接口地址的数据包)
   * - ``ipv4_icmp_redirect``
     - ``trap``
     - Traps (exception) IPV4 ICMP redirect packets
       (捕获(异常)IPV4 ICMP 重定向数据包)
   * - ``arp_response``
     - ``trap``
     - Traps ARP replies packets that have switch-port's DA Mac address
       (捕获带有交换机端口目的 MAC 地址的 ARP 应答数据包)
   * - ``acl_code_0``
     - ``trap``
     - Traps packets that have ACL priority set to 0 (tc pref 0)
       (捕获 ACL 优先级设置为 0(tc pref 0)的数据包)
   * - ``acl_code_1``
     - ``trap``
     - Traps packets that have ACL priority set to 1 (tc pref 1)
       (捕获 ACL 优先级设置为 1(tc pref 1)的数据包)
   * - ``acl_code_2``
     - ``trap``
     - Traps packets that have ACL priority set to 2 (tc pref 2)
       (捕获 ACL 优先级设置为 2(tc pref 2)的数据包)
   * - ``acl_code_3``
     - ``trap``
     - Traps packets that have ACL priority set to 3 (tc pref 3)
       (捕获 ACL 优先级设置为 3(tc pref 3)的数据包)
   * - ``acl_code_4``
     - ``trap``
     - Traps packets that have ACL priority set to 4 (tc pref 4)
       (捕获 ACL 优先级设置为 4(tc pref 4)的数据包)
   * - ``acl_code_5``
     - ``trap``
     - Traps packets that have ACL priority set to 5 (tc pref 5)
       (捕获 ACL 优先级设置为 5(tc pref 5)的数据包)
   * - ``acl_code_6``
     - ``trap``
     - Traps packets that have ACL priority set to 6 (tc pref 6)
       (捕获 ACL 优先级设置为 6(tc pref 6)的数据包)
   * - ``acl_code_7``
     - ``trap``
     - Traps packets that have ACL priority set to 7 (tc pref 7)
       (捕获 ACL 优先级设置为 7(tc pref 7)的数据包)
   * - ``ipv4_bgp``
     - ``trap``
     - Traps IPv4 BGP packets
       (捕获 IPv4 BGP 数据包)
   * - ``ssh``
     - ``trap``
     - Traps SSH packets
       (捕获 SSH 数据包)
   * - ``telnet``
     - ``trap``
     - Traps Telnet packets
       (捕获 Telnet 数据包)
   * - ``icmp``
     - ``trap``
     - Traps ICMP packets
       (捕获 ICMP 数据包)
   * - ``rxdma_drop``
     - ``drop``
     - Drops packets (RxDMA) due to the lack of ingress buffers etc.
       (由于入站缓冲区不足等原因丢弃数据包(RxDMA))
   * - ``port_no_vlan``
     - ``drop``
     - Drops packets due to faulty-configured network or due to internal bug (config issue).
       (由于网络配置错误或内部缺陷(配置问题)而丢弃数据包。)
   * - ``local_port``
     - ``drop``
     - Drops packets whose decision (FDB entry) is to bridge packet back to the incoming port/trunk.
       (丢弃其决策(FDB 表项)是把数据包桥接回入站端口/trunk 的数据包。)
   * - ``invalid_sa``
     - ``drop``
     - Drops packets with multicast source MAC address.
       (丢弃带有组播源 MAC 地址的数据包。)
   * - ``illegal_ip_addr``
     - ``drop``
     - Drops packets with illegal SIP/DIP multicast/unicast addresses.
       (丢弃带有非法 SIP/DIP 组播/单播地址的数据包。)
   * - ``illegal_ipv4_hdr``
     - ``drop``
     - Drops packets with illegal IPV4 header.
       (丢弃带有非法 IPV4 头的数据包。)
   * - ``ip_uc_dip_da_mismatch``
     - ``drop``
     - Drops packets with destination MAC being unicast, but destination IP address being multicast.
       (丢弃目的 MAC 为单播但目的 IP 地址为组播的数据包。)
   * - ``ip_sip_is_zero``
     - ``drop``
     - Drops packets with zero (0) IPV4 source address.
       (丢弃源 IPV4 地址为零(0)的数据包。)
   * - ``met_red``
     - ``drop``
     - Drops non-conforming packets (dropped by Ingress policer, metering drop), e.g. packet rate exceeded configured bandwidth.
       (丢弃不符合约定的数据包(由入站策略器丢弃，计量丢弃)，例如数据包速率超过
       配置的带宽。)
