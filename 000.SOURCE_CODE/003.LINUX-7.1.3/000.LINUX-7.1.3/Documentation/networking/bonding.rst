.. SPDX-License-Identifier: GPL-2.0

===============================================================
Linux Ethernet Bonding Driver HOWTO(Linux 以太网绑定驱动 HOWTO)
===============================================================

Latest update: 27 April 2011
(最近更新：2011 年 4 月 27 日)

Initial release: Thomas Davis <tadavis at lbl.gov>
(最初发布：Thomas Davis <tadavis at lbl.gov>)

Corrections, HA extensions: 2000/10/03-15:
(勘误与高可用性扩展：2000/10/03-15：)

  - Willy Tarreau <willy at meta-x.org>
  - Constantine Gavrilov <const-g at xpert.com>
  - Chad N. Tindel <ctindel at ieee dot org>
  - Janice Girouard <girouard at us dot ibm dot com>
  - Jay Vosburgh <fubar at us dot ibm dot com>

Reorganized and updated Feb 2005 by Jay Vosburgh
(2005 年 2 月由 Jay Vosburgh 重新整理并更新)
Added Sysfs information: 2006/04/24
(新增 Sysfs 信息：2006/04/24)

  - Mitch Williams <mitch.a.williams at intel.com>

Introduction(引言)
==================

The Linux bonding driver provides a method for aggregating
multiple network interfaces into a single logical "bonded" interface.
The behavior of the bonded interfaces depends upon the mode; generally
speaking, modes provide either hot standby or load balancing services.
Additionally, link integrity monitoring may be performed.
(Linux 绑定驱动提供了一种将多个网络接口聚合为单个逻辑 "bonded" 接口的方法。绑定接口的行为取决于所用模式；大体而言，各模式提供热备份或负载均衡服务。此外，还可以执行链路完整性监控。)

The bonding driver originally came from Donald Becker's
beowulf patches for kernel 2.0. It has changed quite a bit since, and
the original tools from extreme-linux and beowulf sites will not work
with this version of the driver.
(绑定驱动最初源自 Donald Becker 针对 2.0 内核的 beowulf 补丁。此后它已发生了很大变化，来自 extreme-linux 和 beowulf 站点的原始工具已无法与当前版本的驱动配合使用。)

For new versions of the driver, updated userspace tools, and
who to ask for help, please follow the links at the end of this file.
(关于驱动的新版本、更新的用户态工具以及求助渠道，请参阅本文件末尾的链接。)

.. Table of Contents

   1. Bonding Driver Installation

   2. Bonding Driver Options

   3. Configuring Bonding Devices
   3.1	Configuration with Sysconfig Support
   3.1.1		Using DHCP with Sysconfig
   3.1.2		Configuring Multiple Bonds with Sysconfig
   3.2	Configuration with Initscripts Support
   3.2.1		Using DHCP with Initscripts
   3.2.2		Configuring Multiple Bonds with Initscripts
   3.3	Configuring Bonding Manually with Ifenslave
   3.3.1		Configuring Multiple Bonds Manually
   3.4	Configuring Bonding Manually via Sysfs
   3.5	Configuration with Interfaces Support
   3.6	Overriding Configuration for Special Cases
   3.7 Configuring LACP for 802.3ad mode in a more secure way

   4. Querying Bonding Configuration
   4.1	Bonding Configuration
   4.2	Network Configuration

   5. Switch Configuration

   6. 802.1q VLAN Support

   7. Link Monitoring
   7.1	ARP Monitor Operation
   7.2	Configuring Multiple ARP Targets
   7.3	MII Monitor Operation

   8. Potential Trouble Sources
   8.1	Adventures in Routing
   8.2	Ethernet Device Renaming
   8.3	Painfully Slow Or No Failed Link Detection By Miimon

   9. SNMP agents

   10. Promiscuous mode

   11. Configuring Bonding for High Availability
   11.1	High Availability in a Single Switch Topology
   11.2	High Availability in a Multiple Switch Topology
   11.2.1		HA Bonding Mode Selection for Multiple Switch Topology
   11.2.2		HA Link Monitoring for Multiple Switch Topology

   12. Configuring Bonding for Maximum Throughput
   12.1	Maximum Throughput in a Single Switch Topology
   12.1.1		MT Bonding Mode Selection for Single Switch Topology
   12.1.2		MT Link Monitoring for Single Switch Topology
   12.2	Maximum Throughput in a Multiple Switch Topology
   12.2.1		MT Bonding Mode Selection for Multiple Switch Topology
   12.2.2		MT Link Monitoring for Multiple Switch Topology

   13. Switch Behavior Issues
   13.1	Link Establishment and Failover Delays
   13.2	Duplicated Incoming Packets

   14. Hardware Specific Considerations
   14.1	IBM BladeCenter

   15. Frequently Asked Questions

   16. Resources and Links


1. Bonding Driver Installation(1. 绑定驱动安装)
===============================================

Most popular distro kernels ship with the bonding driver
already available as a module. If your distro does not, or you
have need to compile bonding from source (e.g., configuring and
installing a mainline kernel from kernel.org), you'll need to perform
the following steps:
(大多数流行的发行版内核都已将绑定驱动以模块形式提供。如果你的发行版没有提供，或者你需要从源码编译 bonding（例如配置并安装来自 kernel.org 的主线内核），则需要执行以下步骤：)

1.1 Configure and build the kernel with bonding(1.1 配置并构建支持 bonding 的内核)
----------------------------------------------------------------------------------

The current version of the bonding driver is available in the
drivers/net/bonding subdirectory of the most recent kernel source
(which is available on http://kernel.org).  Most users "rolling their
own" will want to use the most recent kernel from kernel.org.
(当前版本的绑定驱动位于最新内核源码的 drivers/net/bonding 子目录中（可从 http://kernel.org 获取）。大多数自行编译内核的用户会希望使用来自 kernel.org 的最新内核。)

Configure kernel with "make menuconfig" (or "make xconfig" or
"make config"), then select "Bonding driver support" in the "Network
device support" section.  It is recommended that you configure the
driver as module since it is currently the only way to pass parameters
to the driver or configure more than one bonding device.
(使用 "make menuconfig"（或 "make xconfig" 或 "make config"）配置内核，然后在 "Network device support" 一节中选择 "Bonding driver support"。建议将该驱动配置为模块，因为这是目前向驱动传递参数或配置多个 bonding 设备的唯一途径。)

Build and install the new kernel and modules.
(构建并安装新内核及各模块。)

1.2 Bonding Control Utility(1.2 bonding 控制工具)
-------------------------------------------------

It is recommended to configure bonding via iproute2 (netlink)
or sysfs, the old ifenslave control utility is obsolete.
(建议通过 iproute2（netlink）或 sysfs 来配置 bonding，旧的 ifenslave 控制工具已废弃。)

2. Bonding Driver Options(2. 绑定驱动选项)
==========================================

Options for the bonding driver are supplied as parameters to the
bonding module at load time, or are specified via sysfs.
(绑定驱动的选项在加载时作为参数提供给 bonding 模块，或通过 sysfs 指定。)

Module options may be given as command line arguments to the
insmod or modprobe command, but are usually specified in either the
``/etc/modprobe.d/*.conf`` configuration files, or in a distro-specific
configuration file (some of which are detailed in the next section).
(模块选项可以作为命令行参数提供给 insmod 或 modprobe 命令，但通常是在 ``/etc/modprobe.d/*.conf`` 配置文件中指定，或在发行版特有的配置文件中指定（其中一些将在下一节详细介绍）。)

Details on bonding support for sysfs is provided in the
"Configuring Bonding Manually via Sysfs" section, below.
(关于 sysfs 对 bonding 支持的详细信息，请参阅下文 "Configuring Bonding Manually via Sysfs" 一节。)

The available bonding driver parameters are listed below. If a
parameter is not specified the default value is used.  When initially
configuring a bond, it is recommended "tail -f /var/log/messages" be
run in a separate window to watch for bonding driver error messages.
(下面列出了可用的绑定驱动参数。若未指定某个参数，则使用默认值。初次配置 bond 时，建议在单独的窗口中运行 "tail -f /var/log/messages"，以便观察绑定驱动的错误消息。)

It is critical that either the miimon or arp_interval and
arp_ip_target parameters be specified, otherwise serious network
degradation will occur during link failures.  Very few devices do not
support at least miimon, so there is really no reason not to use it.
(关键在于必须指定 miimon，或指定 arp_interval 与 arp_ip_target 参数，否则在链路故障期间会出现严重的网络性能劣化。几乎没有设备不支持 miimon，因此实在没有理由不使用它。)

Options with textual values will accept either the text name
or, for backwards compatibility, the option value.  E.g.,
"mode=802.3ad" and "mode=4" set the same mode.
(接受文本取值的选项既可以接受文本名称，为向后兼容也可以接受选项数值。例如，"mode=802.3ad" 与 "mode=4" 设置的是同一种模式。)

The parameters are as follows:
(参数如下：)

active_slave
(活动从设备)

	Specifies the new active slave for modes that support it
	(active-backup, balance-alb and balance-tlb).  Possible values
	are the name of any currently enslaved interface, or an empty
	string.  If a name is given, the slave and its link must be up in order
	to be selected as the new active slave.  If an empty string is
	specified, the current active slave is cleared, and a new active
	slave is selected automatically.
	(为支持该功能的模式（active-backup、balance-alb 和 balance-tlb）指定新的活动从设备。可能的取值为当前任一已绑定接口的名称，或空字符串。若给定名称，则该从设备及其链路必须处于 up 状态才能被选为新的活动从设备。若指定空字符串，则清除当前活动从设备，并自动选择新的活动从设备。)

	Note that this is only available through the sysfs interface. No module
	parameter by this name exists.
	(注意，该选项只能通过 sysfs 接口使用，并不存在同名的模块参数。)

	The normal value of this option is the name of the currently
	active slave, or the empty string if there is no active slave or
	the current mode does not use an active slave.
	(该选项的正常取值为当前活动从设备的名称；如果没有活动从设备，或当前模式不使用活动从设备，则为空字符串。)

ad_actor_sys_prio
(AD 系统优先级)

	In an AD system, this specifies the system priority. The allowed range
	is 1 - 65535. If the value is not specified, it takes 65535 as the
	default value.
	(在 AD 系统中，该参数指定系统优先级。允许的范围是 1 - 65535。若未指定该值，则默认取 65535。)

	This parameter has effect only in 802.3ad mode and is available through
	SysFs interface.
	(该参数仅在 802.3ad 模式下生效，并可通过 SysFs 接口设置。)

actor_port_prio
(AD actor 端口优先级)

	In an AD system, this specifies the port priority. The allowed range
	is 1 - 65535. If the value is not specified, it takes 255 as the
	default value.
	(在 AD 系统中，该参数指定端口优先级。允许的范围是 1 - 65535。若未指定该值，则默认取 255。)

	This parameter has effect only in 802.3ad mode and is available through
	netlink interface.
	(该参数仅在 802.3ad 模式下生效，并可通过 netlink 接口设置。)

ad_actor_system
(AD actor 系统地址)

	In an AD system, this specifies the mac-address for the actor in
	protocol packet exchanges (LACPDUs). The value cannot be a multicast
	address. If the all-zeroes MAC is specified, bonding will internally
	use the MAC of the bond itself. It is preferred to have the
	local-admin bit set for this mac but driver does not enforce it. If
	the value is not given then system defaults to using the masters'
	mac address as actors' system address.
	(在 AD 系统中，该参数指定 actor 在协议报文交换（LACPDUs）中使用的 MAC 地址。该值不能是组播地址。若指定全零 MAC，bonding 将在内部改用 bond 自身的 MAC。该 MAC 最好设置 local-admin 位，但驱动并不强制要求。若未指定该值，系统默认使用主设备（master）的 MAC 地址作为 actor 的系统地址。)

	This parameter has effect only in 802.3ad mode and is available through
	SysFs interface.
	(该参数仅在 802.3ad 模式下生效，并可通过 SysFs 接口设置。)

ad_select
(聚合器选择逻辑)

	Specifies the 802.3ad aggregation selection logic to use.  The
	possible values and their effects are:
	(指定要使用的 802.3ad 聚合选择逻辑。可能的取值及其效果如下：)

	stable or 0
	(stable 或 0)

		The active aggregator is chosen by largest aggregate
		bandwidth.
		(活动聚合器按最大的聚合带宽来选择。)

		Reselection of the active aggregator occurs only when all
		slaves of the active aggregator are down or the active
		aggregator has no slaves.
		(只有当活动聚合器的所有从设备都 down，或活动聚合器没有从设备时，才会重新选择活动聚合器。)

		This is the default value.
		(这是默认值。)

	bandwidth or 1
	(bandwidth 或 1)

		The active aggregator is chosen by largest aggregate
		bandwidth.  Reselection occurs if:
		(活动聚合器按最大的聚合带宽来选择。出现以下情况时会重新选择：)

		- A slave is added to or removed from the bond
		  (向 bond 添加或从 bond 移除某个从设备)

		- Any slave's link state changes
		  (任一从设备的链路状态发生变化)

		- Any slave's 802.3ad association state changes
		  (任一从设备的 802.3ad 关联状态发生变化)

		- The bond's administrative state changes to up
		  (bond 的管理状态变为 up)

	count or 2
	(count 或 2)

		The active aggregator is chosen by the largest number of
		ports (slaves).  Reselection occurs as described under the
		"bandwidth" setting, above.
		(活动聚合器按端口（从设备）数量的最多者来选择。重新选择的时机与上述 "bandwidth" 设置中描述的相同。)

	actor_port_prio or 3
	(actor_port_prio 或 3)

		The active aggregator is chosen by the highest total sum of
		actor port priorities across its active ports. Note this
		priority is actor_port_prio, not per port prio, which is
		used for primary reselect.
		(活动聚合器按其活动端口上 actor 端口优先级总和的最高者来选择。注意此处的优先级是 actor_port_prio，而不是用于 primary 重新选择的每端口优先级。)

	The bandwidth, count and actor_port_prio selection policies permit
	failover of 802.3ad aggregations when partial failure of the active
	aggregator occurs. This keeps the aggregator with the highest
	availability (either in bandwidth, number of ports, or total value
	of port priorities) active at all times.
	(bandwidth、count 和 actor_port_prio 这几种选择策略允许在活动聚合器发生部分故障时对 802.3ad 聚合进行故障转移。这使可用性最高（以带宽、端口数或端口优先级总和衡量）的聚合器始终保持活动状态。)

	This option was added in bonding version 3.4.0.
	(该选项自 bonding 版本 3.4.0 起引入。)

ad_user_port_key
(AD 用户端口键)

	In an AD system, the port-key has three parts as shown below -
	(在 AD 系统中，端口键 (port-key) 由如下三个部分组成 -)

	   =====  ============
	   Bits   Use
	   =====  ============
	   00     Duplex
	   01-05  Speed
	   06-15  User-defined
	   =====  ============

	This defines the upper 10 bits of the port key. The values can be
	from 0 - 1023. If not given, the system defaults to 0.
	(该参数定义端口键的高 10 位。取值范围是 0 - 1023。若未指定，系统默认取 0。)

	This parameter has effect only in 802.3ad mode and is available through
	SysFs interface.
	(该参数仅在 802.3ad 模式下生效，并可通过 SysFs 接口设置。)

all_slaves_active
(传递非活动端口上的重复帧)

	Specifies that duplicate frames (received on inactive ports) should be
	dropped (0) or delivered (1).
	(指定丢弃 (0) 还是照常上送 (1) 在非活动端口上收到的重复帧。)

	Normally, bonding will drop duplicate frames (received on inactive
	ports), which is desirable for most users. But there are some times
	it is nice to allow duplicate frames to be delivered.
	(通常，bonding 会丢弃在非活动端口上收到的重复帧，这对大多数用户来说是期望的行为。但某些情况下，允许上送重复帧会更有用。)

	The default value is 0 (drop duplicate frames received on inactive
	ports).
	(默认值为 0（丢弃在非活动端口上收到的重复帧）。)

arp_interval
(ARP 链路监控间隔)

	Specifies the ARP link monitoring frequency in milliseconds.
	(以毫秒为单位指定 ARP 链路监控的频率。)

	The ARP monitor works by periodically checking the slave
	devices to determine whether they have sent or received
	traffic recently (the precise criteria depends upon the
	bonding mode, and the state of the slave).  Regular traffic is
	generated via ARP probes issued for the addresses specified by
	the arp_ip_target option.
	(ARP 监控的工作方式是定期检查从设备，判断它们近期是否发送或接收过流量（具体的判定标准取决于 bonding 模式以及从设备的状态）。常规流量通过向 arp_ip_target 选项指定的地址发送 ARP 探测来产生。)

	This behavior can be modified by the arp_validate option,
	below.
	(该行为可通过下文的 arp_validate 选项加以修改。)

	If ARP monitoring is used in an etherchannel compatible mode
	(modes 0 and 2), the switch should be configured in a mode
	that evenly distributes packets across all links. If the
	switch is configured to distribute the packets in an XOR
	fashion, all replies from the ARP targets will be received on
	the same link which could cause the other team members to
	fail.  ARP monitoring should not be used in conjunction with
	miimon.  A value of 0 disables ARP monitoring.  The default
	value is 0.
	(如果在 etherchannel 兼容模式（模式 0 和 2）下使用 ARP 监控，交换机应配置为将数据包均匀分布到所有链路的模式。若交换机配置为以 XOR 方式分发数据包，则来自 ARP 目标的全部应答都会到达同一条链路，这可能导致其他组成员失效。ARP 监控不应与 miimon 同时使用。取值 0 表示禁用 ARP 监控。默认值为 0。)

arp_ip_target
(ARP 监控目标 IP 地址)

	Specifies the IP addresses to use as ARP monitoring peers when
	arp_interval is > 0.  These are the targets of the ARP request
	sent to determine the health of the link to the targets.
	Specify these values in ddd.ddd.ddd.ddd format.  Multiple IP
	addresses must be separated by a comma.  At least one IP
	address must be given for ARP monitoring to function.  The
	maximum number of targets that can be specified is 16.  The
	default value is no IP addresses.
	(当 arp_interval 大于 0 时，指定用作 ARP 监控对端的 IP 地址。它们是 ARP 请求的目标，用于确定到这些目标的链路是否健康。取值需以 ddd.ddd.ddd.ddd 格式给出。多个 IP 地址之间必须用逗号分隔。要使 ARP 监控生效，必须至少提供一个 IP 地址。最多可指定 16 个目标。默认值为未配置任何 IP 地址。)

ns_ip6_target
(IPv6 监控目标地址)

	Specifies the IPv6 addresses to use as IPv6 monitoring peers when
	arp_interval is > 0.  These are the targets of the NS request
	sent to determine the health of the link to the targets.
	Specify these values in ffff:ffff::ffff:ffff format.  Multiple IPv6
	addresses must be separated by a comma.  At least one IPv6
	address must be given for NS/NA monitoring to function.  The
	maximum number of targets that can be specified is 16.  The
	default value is no IPv6 addresses.
	(当 arp_interval 大于 0 时，指定用作 IPv6 监控对端的 IPv6 地址。它们是 NS 请求的目标，用于确定到这些目标的链路是否健康。取值需以 ffff:ffff::ffff:ffff 格式给出。多个 IPv6 地址之间必须用逗号分隔。要使 NS/NA 监控生效，必须至少提供一个 IPv6 地址。最多可指定 16 个目标。默认值为未配置任何 IPv6 地址。)

arp_validate
(ARP 探测/应答校验与过滤)

	Specifies whether or not ARP probes and replies should be
	validated in any mode that supports arp monitoring, or whether
	non-ARP traffic should be filtered (disregarded) for link
	monitoring purposes.
	(指定在支持 ARP 监控的模式下是否校验 ARP 探测与应答，或者是否出于链路监控目的而过滤（忽略）非 ARP 流量。)

	Possible values are:
	(可能的取值有：)

	none or 0
	(none 或 0)

		No validation or filtering is performed.
		(不执行任何校验或过滤。)

	active or 1
	(active 或 1)

		Validation is performed only for the active slave.
		(仅对活动从设备执行校验。)

	backup or 2
	(backup 或 2)

		Validation is performed only for backup slaves.
		(仅对备份从设备执行校验。)

	all or 3
	(all 或 3)

		Validation is performed for all slaves.
		(对所有从设备执行校验。)

	filter or 4
	(filter 或 4)

		Filtering is applied to all slaves. No validation is
		performed.
		(对所有从设备应用过滤，不执行校验。)

	filter_active or 5
	(filter_active 或 5)

		Filtering is applied to all slaves, validation is performed
		only for the active slave.
		(对所有从设备应用过滤，仅对活动从设备执行校验。)

	filter_backup or 6
	(filter_backup 或 6)

		Filtering is applied to all slaves, validation is performed
		only for backup slaves.
		(对所有从设备应用过滤，仅对备份从设备执行校验。)

	Validation:
	(校验：)

	Enabling validation causes the ARP monitor to examine the incoming
	ARP requests and replies, and only consider a slave to be up if it
	is receiving the appropriate ARP traffic.
	(启用校验后，ARP 监控会检查收到的 ARP 请求与应答，只有当从设备正在接收恰当的 ARP 流量时才认为它处于 up 状态。)

	For an active slave, the validation checks ARP replies to confirm
	that they were generated by an arp_ip_target.  Since backup slaves
	do not typically receive these replies, the validation performed
	for backup slaves is on the broadcast ARP request sent out via the
	active slave.  It is possible that some switch or network
	configurations may result in situations wherein the backup slaves
	do not receive the ARP requests; in such a situation, validation
	of backup slaves must be disabled.
	(对于活动从设备，校验会检查 ARP 应答，以确认它们由某个 arp_ip_target 产生。由于备份从设备通常接收不到这些应答，对备份从设备执行的校验针对的是经由活动从设备发出的广播 ARP 请求。某些交换机或网络配置可能导致备份从设备收不到 ARP 请求；在这种情况下，必须禁用对备份从设备的校验。)

	The validation of ARP requests on backup slaves is mainly helping
	bonding to decide which slaves are more likely to work in case of
	the active slave failure, it doesn't really guarantee that the
	backup slave will work if it's selected as the next active slave.
	(对备份从设备上 ARP 请求的校验，主要是帮助 bonding 判断在活动从设备失效时哪些从设备更可能正常工作，它并不能真正保证被选为下一个活动从设备的备份从设备一定可用。)

	Validation is useful in network configurations in which multiple
	bonding hosts are concurrently issuing ARPs to one or more targets
	beyond a common switch.  Should the link between the switch and
	target fail (but not the switch itself), the probe traffic
	generated by the multiple bonding instances will fool the standard
	ARP monitor into considering the links as still up.  Use of
	validation can resolve this, as the ARP monitor will only consider
	ARP requests and replies associated with its own instance of
	bonding.
	(在多台 bonding 主机经由同一交换机同时向一个或多个目标发送 ARP 的网络配置中，校验非常有用。如果交换机与目标之间的链路发生故障（交换机本身未故障），多个 bonding 实例产生的探测流量会欺骗标准 ARP 监控，使其认为链路仍然正常。使用校验可以解决这一问题，因为 ARP 监控只会考虑与其自身 bonding 实例相关的 ARP 请求与应答。)

	Filtering:
	(过滤：)

	Enabling filtering causes the ARP monitor to only use incoming ARP
	packets for link availability purposes.  Arriving packets that are
	not ARPs are delivered normally, but do not count when determining
	if a slave is available.
	(启用过滤后，ARP 监控在判断链路可用性时只使用收到的 ARP 数据包。到达的非 ARP 数据包会被正常上送，但在判断从设备是否可用时不会被计入。)

	Filtering operates by only considering the reception of ARP
	packets (any ARP packet, regardless of source or destination) when
	determining if a slave has received traffic for link availability
	purposes.
	(过滤的工作方式是：在为判断链路可用性而确定某从设备是否收到过流量时，只考虑 ARP 数据包的接收（无论来源或目的地为何的任何 ARP 数据包）。)

	Filtering is useful in network configurations in which significant
	levels of third party broadcast traffic would fool the standard
	ARP monitor into considering the links as still up.  Use of
	filtering can resolve this, as only ARP traffic is considered for
	link availability purposes.
	(在存在大量第三方广播流量的网络配置中，这些流量会欺骗标准 ARP 监控，使其认为链路仍然正常，此时过滤非常有用。使用过滤可以解决这一问题，因为判断链路可用性时只考虑 ARP 流量。)

	This option was added in bonding version 3.1.0.
	(该选项自 bonding 版本 3.1.0 起引入。)

arp_all_targets
(ARP 目标可达数量要求)

	Specifies the quantity of arp_ip_targets that must be reachable
	in order for the ARP monitor to consider a slave as being up.
	This option affects only active-backup mode for slaves with
	arp_validation enabled.
	(指定 ARP 监控判定从设备为 up 时必须可达的 arp_ip_targets 数量。该选项只影响启用了 arp_validation 的 active-backup 模式下的从设备。)

	Possible values are:
	(可能的取值有：)

	any or 0
	(any 或 0)

		consider the slave up only when any of the arp_ip_targets
		is reachable
		(只要 arp_ip_targets 中任意一个可达，即认为该从设备处于 up 状态)

	all or 1
	(all 或 1)

		consider the slave up only when all of the arp_ip_targets
		are reachable
		(只有当 arp_ip_targets 全部可达时，才认为该从设备处于 up 状态)

arp_missed_max
(ARP 监控失败次数上限)

	Specifies the number of arp_interval monitor checks that must
	fail in order for an interface to be marked down by the ARP monitor.
	(指定接口被 ARP 监控标记为 down 之前必须连续失败的 arp_interval 监控检查次数。)

	In order to provide orderly failover semantics, backup interfaces
	are permitted an extra monitor check (i.e., they must fail
	arp_missed_max + 1 times before being marked down).
	(为了提供有序的故障转移语义，备份接口允许多做一次监控检查（即它们必须失败 arp_missed_max + 1 次后才会被标记为 down）。)

	The default value is 2, and the allowable range is 1 - 255.
	(默认值为 2，允许的范围是 1 - 255。)

coupled_control
(LACP MUX 独立控制状态机)

    Specifies whether the LACP state machine's MUX in the 802.3ad mode
    should have separate Collecting and Distributing states.
    (指定 802.3ad 模式下 LACP 状态机的 MUX 是否应具有独立的 Collecting（收集）与 Distributing（分发）状态。)

    This is by implementing the independent control state machine per
    IEEE 802.1AX-2008 5.4.15 in addition to the existing coupled control
    state machine.
    (其实现方式是：在既有耦合控制状态机之外，再按 IEEE 802.1AX-2008 5.4.15 实现独立控制状态机。)

    The default value is 1. This setting does not separate the Collecting
    and Distributing states, maintaining the bond in coupled control.
    (默认值为 1。该设置不分离 Collecting 与 Distributing 状态，使 bond 保持耦合控制。)

downdelay
(禁用从设备前的延迟)

	Specifies the time, in milliseconds, to wait before disabling
	a slave after a link failure has been detected.  This option
	is only valid for the miimon link monitor.  The downdelay
	value should be a multiple of the miimon value; if not, it
	will be rounded down to the nearest multiple.  The default
	value is 0.
	(以毫秒为单位，指定检测到链路故障后禁用从设备前的等待时间。该选项仅对 miimon 链路监控有效。downdelay 的值应为 miimon 值的整数倍；若不是，则向下取整到最近的倍数。默认值为 0。)

fail_over_mac
(MAC 地址故障转移策略)

	Specifies whether active-backup mode should set all slaves to
	the same MAC address at enslavement (the traditional
	behavior), or, when enabled, perform special handling of the
	bond's MAC address in accordance with the selected policy.
	(指定 active-backup 模式是在绑定时将所有从设备设置为相同的 MAC 地址（传统行为），还是在启用后按所选策略对 bond 的 MAC 地址做特殊处理。)

	Possible values are:
	(可能的取值有：)

	none or 0
	(none 或 0)

		This setting disables fail_over_mac, and causes
		bonding to set all slaves of an active-backup bond to
		the same MAC address at enslavement time.  This is the
		default.
		(该设置禁用 fail_over_mac，使 bonding 在绑定时将 active-backup bond 的所有从设备设置为相同的 MAC 地址。这是默认值。)

	active or 1
	(active 或 1)

		The "active" fail_over_mac policy indicates that the
		MAC address of the bond should always be the MAC
		address of the currently active slave.  The MAC
		address of the slaves is not changed; instead, the MAC
		address of the bond changes during a failover.
		("active" fail_over_mac 策略表示 bond 的 MAC 地址应始终等于当前活动从设备的 MAC 地址。从设备的 MAC 地址不改变；改变的是 bond 的 MAC 地址（在故障转移时）。)

		This policy is useful for devices that cannot ever
		alter their MAC address, or for devices that refuse
		incoming broadcasts with their own source MAC (which
		interferes with the ARP monitor).
		(该策略适用于永远无法更改自身 MAC 地址的设备，或拒绝接收以其自身源 MAC 为源地址的入站广播的设备（这种行为会干扰 ARP 监控）。)

		The down side of this policy is that every device on
		the network must be updated via gratuitous ARP,
		vs. just updating a switch or set of switches (which
		often takes place for any traffic, not just ARP
		traffic, if the switch snoops incoming traffic to
		update its tables) for the traditional method.  If the
		gratuitous ARP is lost, communication may be
		disrupted.
		(该策略的缺点是：网络上的每台设备都必须通过免费 ARP (gratuitous ARP) 来更新，而传统方法只需更新一台或一组交换机（如果交换机会侦听入站流量以更新自身表项，那么任何流量——而不仅仅是 ARP 流量——通常都能触发更新）。如果免费 ARP 丢失，通信可能被中断。)

		When this policy is used in conjunction with the mii
		monitor, devices which assert link up prior to being
		able to actually transmit and receive are particularly
		susceptible to loss of the gratuitous ARP, and an
		appropriate updelay setting may be required.
		(当该策略与 mii 监控配合使用时，那些在实际收发能力就绪之前就上报链路 up 的设备特别容易丢失免费 ARP，此时可能需要设置合适的 updelay。)

	follow or 2
	(follow 或 2)

		The "follow" fail_over_mac policy causes the MAC
		address of the bond to be selected normally (normally
		the MAC address of the first slave added to the bond).
		However, the second and subsequent slaves are not set
		to this MAC address while they are in a backup role; a
		slave is programmed with the bond's MAC address at
		failover time (and the formerly active slave receives
		the newly active slave's MAC address).
		("follow" fail_over_mac 策略使 bond 的 MAC 地址按正常方式选定（通常是加入 bond 的第一个从设备的 MAC 地址）。但在处于备份角色期间，第二及后续从设备不会被设置为该 MAC 地址；只有发生故障转移时，从设备才会被写入 bond 的 MAC 地址（而原先的活动从设备则改用新活动从设备的 MAC 地址）。)

		This policy is useful for multiport devices that
		either become confused or incur a performance penalty
		when multiple ports are programmed with the same MAC
		address.
		(该策略适用于那种在多个端口被写入同一 MAC 地址时会混乱或性能受损的多端口设备。)


	The default policy is none, unless the first slave cannot
	change its MAC address, in which case the active policy is
	selected by default.
	(默认策略为 none，除非第一个从设备无法更改其 MAC 地址，此时默认选用 active 策略。)

	This option may be modified via sysfs only when no slaves are
	present in the bond.
	(仅当 bond 中不存在从设备时，才能通过 sysfs 修改该选项。)

	This option was added in bonding version 3.2.0.  The "follow"
	policy was added in bonding version 3.3.0.
	(该选项自 bonding 版本 3.2.0 起引入。"follow" 策略自版本 3.3.0 起引入。)

lacp_active
(周期性发送 LACPDU)

	Option specifying whether to send LACPDU frames periodically.
	(指定是否周期性发送 LACPDU 帧的选项。)

	off or 0
	(off 或 0)

		LACPDU frames acts as "speak when spoken to".
		(LACPDU 帧的行为是"被问才答"，即仅在被询问时才发送。)

	on or 1
	(on 或 1)

		LACPDU frames are sent along the configured links
		periodically. See lacp_rate for more details.
		(LACPDU 帧沿已配置的链路周期性发送。详情参见 lacp_rate。)

	The default is on.
	(默认为 on。)

lacp_rate
(LACPDU 发送速率)

	Option specifying the rate in which we'll ask our link partner
	to transmit LACPDU packets in 802.3ad mode.  Possible values
	are:
	(在 802.3ad 模式下，指定请求链路伙伴发送 LACPDU 数据包速率的选项。可能的取值有：)

	slow or 0
	(slow 或 0)

		Request partner to transmit LACPDUs every 30 seconds
		(请求伙伴每 30 秒发送一次 LACPDUs)

	fast or 1
	(fast 或 1)

		Request partner to transmit LACPDUs every 1 second
		(请求伙伴每 1 秒发送一次 LACPDUs)

	The default is slow.
	(默认为 slow。)

broadcast_neighbor
(向所有活动从设备广播 ARP/ND)

	Option specifying whether to broadcast ARP/ND packets to all
	active slaves.  This option has no effect in modes other than
	802.3ad mode.  The default is off (0).
	(指定是否向所有活动从设备广播 ARP/ND 数据包的选项。该选项在 802.3ad 以外的模式下无效。默认为 off (0)。)

max_bonds
(创建的 bonding 设备数量)

	Specifies the number of bonding devices to create for this
	instance of the bonding driver.  E.g., if max_bonds is 3, and
	the bonding driver is not already loaded, then bond0, bond1
	and bond2 will be created.  The default value is 1.  Specifying
	a value of 0 will load bonding, but will not create any devices.
	(指定为本 bonding 驱动实例创建的 bonding 设备数量。例如，若 max_bonds 为 3 且 bonding 驱动尚未加载，则会创建 bond0、bond1 和 bond2。默认值为 1。指定 0 会加载 bonding，但不创建任何设备。)

miimon
(MII 链路监控间隔)

	Specifies the MII link monitoring frequency in milliseconds.
	This determines how often the link state of each slave is
	inspected for link failures.  A value of zero disables MII
	link monitoring.  A value of 100 is a good starting point.
	(以毫秒为单位指定 MII 链路监控的频率。它决定检查各从设备链路是否故障的频率。取值 0 表示禁用 MII 链路监控。100 是一个不错的起始值。)

	The default value is 100 if arp_interval is not set.
	(若未设置 arp_interval，默认值为 100。)

min_links
(宣告载波所需的最小链路数)

	Specifies the minimum number of links that must be active before
	asserting carrier. It is similar to the Cisco EtherChannel min-links
	feature. This allows setting the minimum number of member ports that
	must be up (link-up state) before marking the bond device as up
	(carrier on). This is useful for situations where higher level services
	such as clustering want to ensure a minimum number of low bandwidth
	links are active before switchover. This option only affect 802.3ad
	mode.
	(指定在宣告载波 (carrier) 之前必须处于活动状态的最小链路数。它类似于 Cisco EtherChannel 的 min-links 特性。通过它可设置在将 bond 设备标记为 up（载波开启）之前必须处于 up（链路 up 状态）的成员端口的最小数量。这对于集群等上层服务希望在切换前确保最低数量低带宽链路可用的场景很有用。该选项仅影响 802.3ad 模式。)

	The default value is 0. This will cause carrier to be asserted (for
	802.3ad mode) whenever there is an active aggregator, regardless of the
	number of available links in that aggregator. Note that, because an
	aggregator cannot be active without at least one available link,
	setting this option to 0 or to 1 has the exact same effect.
	(默认值为 0。这意味着在 802.3ad 模式下，只要存在活动聚合器，就会宣告载波，而不论该聚合器中有多少可用链路。注意，由于聚合器没有至少一条可用链路就不可能处于活动状态，将该选项设为 0 或 1 的效果完全相同。)

mode
(绑定策略/模式)

	Specifies one of the bonding policies. The default is
	balance-rr (round robin).  Possible values are:
	(指定一种绑定策略。默认为 balance-rr（轮询）。可能的取值有：)

	balance-rr or 0
	(balance-rr 或 0)

		Round-robin policy: Transmit packets in sequential
		order from the first available slave through the
		last.  This mode provides load balancing and fault
		tolerance.
		(轮询策略：按顺序从第一个可用从设备到最后一个从设备依次发送数据包。该模式提供负载均衡与容错能力。)

	active-backup or 1
	(active-backup 或 1)

		Active-backup policy: Only one slave in the bond is
		active.  A different slave becomes active if, and only
		if, the active slave fails.  The bond's MAC address is
		externally visible on only one port (network adapter)
		to avoid confusing the switch.
		(主备策略：bond 中只有一个从设备处于活动状态。当且仅当活动从设备失效时，另一个从设备才会变为活动。bond 的 MAC 地址在外部只出现在一个端口（网络适配器）上，以避免混淆交换机。)

		In bonding version 2.6.2 or later, when a failover
		occurs in active-backup mode, bonding will issue one
		or more gratuitous ARPs on the newly active slave.
		One gratuitous ARP is issued for the bonding master
		interface and each VLAN interfaces configured above
		it, provided that the interface has at least one IP
		address configured.  Gratuitous ARPs issued for VLAN
		interfaces are tagged with the appropriate VLAN id.
		(在 bonding 2.6.2 及更高版本中，当 active-backup 模式发生故障转移时，bonding 会在新的活动从设备上发送一个或多个免费 ARP。bonding 主接口及其上配置的每个 VLAN 接口各发送一个免费 ARP，前提是该接口至少配置了一个 IP 地址。为 VLAN 接口发送的免费 ARP 会打上相应的 VLAN id 标签。)

		This mode provides fault tolerance.  The primary
		option, documented below, affects the behavior of this
		mode.
		(该模式提供容错能力。下文记录的 primary 选项会影响该模式的行为。)

	balance-xor or 2
	(balance-xor 或 2)

		XOR policy: Transmit based on the selected transmit
		hash policy.  The default policy is a simple [(source
		MAC address XOR'd with destination MAC address XOR
		packet type ID) modulo slave count].  Alternate transmit
		policies may be	selected via the xmit_hash_policy option,
		described below.
		(XOR 策略：基于所选的发送哈希策略进行发送。默认策略是简单的 [(源 MAC 地址 XOR 目的 MAC 地址 XOR 数据包类型 ID) 对从设备数量取模]。可以通过下文描述的 xmit_hash_policy 选项选用其他发送策略。)

		This mode provides load balancing and fault tolerance.
		(该模式提供负载均衡与容错能力。)

	broadcast or 3
	(broadcast 或 3)

		Broadcast policy: transmits everything on all slave
		interfaces.  This mode provides fault tolerance.
		(广播策略：在所有从设备接口上发送全部数据。该模式提供容错能力。)

	802.3ad or 4
	(802.3ad 或 4)

		IEEE 802.3ad Dynamic link aggregation.  Creates
		aggregation groups that share the same speed and
		duplex settings.  Utilizes all slaves in the active
		aggregator according to the 802.3ad specification.
		(IEEE 802.3ad 动态链路聚合。创建共享相同速率与双工设置的聚合组。按照 802.3ad 规范使用活动聚合器中的所有从设备。)

		Slave selection for outgoing traffic is done according
		to the transmit hash policy, which may be changed from
		the default simple XOR policy via the xmit_hash_policy
		option, documented below.  Note that not all transmit
		policies may be 802.3ad compliant, particularly in
		regards to the packet mis-ordering requirements of
		section 43.2.4 of the 802.3ad standard.  Differing
		peer implementations will have varying tolerances for
		noncompliance.
		(出站流量的从设备选择依据发送哈希策略进行，可通过下文记录的 xmit_hash_policy 选项将默认的简单 XOR 策略改为其他策略。注意，并非所有发送策略都符合 802.3ad 规范，尤其是在数据包乱序方面需满足 802.3ad 标准 43.2.4 节的要求。不同的对端实现对不合规情况的容忍度各不相同。)

		Prerequisites:
		(前提条件：)

		1. Ethtool support in the base drivers for retrieving
		the speed and duplex of each slave.
		(1. 底层驱动需支持 Ethtool，以获取每个从设备的速率与双工模式。)

		2. A switch that supports IEEE 802.3ad Dynamic link
		aggregation.
		(2. 交换机需支持 IEEE 802.3ad 动态链路聚合。)

		Most switches will require some type of configuration
		to enable 802.3ad mode.
		(大多数交换机都需要进行某种配置才能启用 802.3ad 模式。)

	balance-tlb or 5
	(balance-tlb 或 5)

		Adaptive transmit load balancing: channel bonding that
		does not require any special switch support.
		(自适应发送负载均衡：一种不需要任何特殊交换机支持的通道绑定。)

		In tlb_dynamic_lb=1 mode; the outgoing traffic is
		distributed according to the current load (computed
		relative to the speed) on each slave.
		(在 tlb_dynamic_lb=1 模式下，出站流量按每个从设备当前负载（相对速率计算）进行分发。)

		In tlb_dynamic_lb=0 mode; the load balancing based on
		current load is disabled and the load is distributed
		only using the hash distribution.
		(在 tlb_dynamic_lb=0 模式下，基于当前负载的负载均衡被禁用，流量仅通过哈希分发方式分配。)

		Incoming traffic is received by the current slave.
		If the receiving slave fails, another slave takes over
		the MAC address of the failed receiving slave.
		(入站流量由当前从设备接收。如果接收从设备失效，另一个从设备会接管失效接收从设备的 MAC 地址。)

		Prerequisite:
		(前提条件：)

		Ethtool support in the base drivers for retrieving the
		speed of each slave.
		(底层驱动需支持 Ethtool，以获取每个从设备的速率。)

	balance-alb or 6
	(balance-alb 或 6)

		Adaptive load balancing: includes balance-tlb plus
		receive load balancing (rlb) for IPV4 traffic, and
		does not require any special switch support.  The
		receive load balancing is achieved by ARP negotiation.
		The bonding driver intercepts the ARP Replies sent by
		the local system on their way out and overwrites the
		source hardware address with the unique hardware
		address of one of the slaves in the bond such that
		different peers use different hardware addresses for
		the server.
		(自适应负载均衡：包含 balance-tlb 以及针对 IPv4 流量的接收负载均衡 (rlb)，不需要任何特殊交换机支持。接收负载均衡通过 ARP 协商实现。bonding 驱动会在本地系统发出的 ARP 应答离开本机时将其拦截，并把源硬件地址改写为 bond 中某个从设备的唯一硬件地址，使不同的对端使用不同的硬件地址来访问本服务器。)

		Receive traffic from connections created by the server
		is also balanced.  When the local system sends an ARP
		Request the bonding driver copies and saves the peer's
		IP information from the ARP packet.  When the ARP
		Reply arrives from the peer, its hardware address is
		retrieved and the bonding driver initiates an ARP
		reply to this peer assigning it to one of the slaves
		in the bond.  A problematic outcome of using ARP
		negotiation for balancing is that each time that an
		ARP request is broadcast it uses the hardware address
		of the bond.  Hence, peers learn the hardware address
		of the bond and the balancing of receive traffic
		collapses to the current slave.  This is handled by
		sending updates (ARP Replies) to all the peers with
		their individually assigned hardware address such that
		the traffic is redistributed.  Receive traffic is also
		redistributed when a new slave is added to the bond
		and when an inactive slave is re-activated.  The
		receive load is distributed sequentially (round robin)
		among the group of highest speed slaves in the bond.
		(由服务器发起的连接的接收流量也会被均衡。当本地系统发送 ARP 请求时，bonding 驱动会从 ARP 数据包中复制并保存对端的 IP 信息。当来自对端的 ARP 应答到达时，会获取其硬件地址，并由 bonding 驱动向该对端发起一个 ARP 应答，将其分配给 bond 中的某个从设备。使用 ARP 协商做均衡存在一个问题：每次广播 ARP 请求时使用的都是 bond 的硬件地址。因此对端会学到 bond 的硬件地址，接收流量的均衡就塌缩到当前从设备上。解决办法是向所有对端发送更新（ARP 应答），附上各自被分配的硬件地址，从而重新分发流量。当新从设备加入 bond 或非活动从设备被重新激活时，接收流量也会重新分发。接收负载按顺序（轮询）分布在 bond 中速率最高的一组从设备上。)

		When a link is reconnected or a new slave joins the
		bond the receive traffic is redistributed among all
		active slaves in the bond by initiating ARP Replies
		with the selected MAC address to each of the
		clients. The updelay parameter (detailed below) must
		be set to a value equal or greater than the switch's
		forwarding delay so that the ARP Replies sent to the
		peers will not be blocked by the switch.
		(当链路重新连接或新从设备加入 bond 时，通过向每个客户端发起带有选定 MAC 地址的 ARP 应答，将接收流量重新分发到 bond 中所有活动从设备上。updelay 参数（下文详述）必须设置为大于或等于交换机转发延迟的值，这样发送给对端的 ARP 应答才不会被交换机阻塞。)

		Prerequisites:
		(前提条件：)

		1. Ethtool support in the base drivers for retrieving
		the speed of each slave.
		(1. 底层驱动需支持 Ethtool，以获取每个从设备的速率。)

		2. Base driver support for setting the hardware
		address of a device while it is open.  This is
		required so that there will always be one slave in the
		team using the bond hardware address (the
		curr_active_slave) while having a unique hardware
		address for each slave in the bond.  If the
		curr_active_slave fails its hardware address is
		swapped with the new curr_active_slave that was
		chosen.
		(2. 底层驱动需支持在设备处于打开状态时设置其硬件地址。这样做是为了保证组中始终有一个从设备使用 bond 的硬件地址（即 curr_active_slave），同时 bond 中每个从设备又拥有唯一的硬件地址。如果 curr_active_slave 失效，其硬件地址会与选出的新 curr_active_slave 交换。)

num_grat_arp,
num_unsol_na
(故障转移后的对端通知数量)

	Specify the number of peer notifications (gratuitous ARPs and
	unsolicited IPv6 Neighbor Advertisements) to be issued after a
	failover event.  As soon as the link is up on the new slave
	(possibly immediately) a peer notification is sent on the
	bonding device and each VLAN sub-device. This is repeated at
	the rate specified by peer_notif_delay if the number is
	greater than 1.
	(指定故障转移事件发生后要发出的对端通知（免费 ARP 和主动 IPv6 邻居通告）的数量。一旦新从设备上的链路变为 up（可能是立即），就会在 bonding 设备及每个 VLAN 子设备上发送对端通知。若该数量大于 1，则按 peer_notif_delay 指定的间隔重复发送。)

	The valid range is 0 - 255; the default value is 1.  These options
	affect the active-backup or 802.3ad (broadcast_neighbor enabled) mode.
	These options were added for bonding versions 3.3.0 and 3.4.0
	respectively.
	(有效范围是 0 - 255，默认值为 1。这两个选项影响 active-backup 模式或 802.3ad（启用 broadcast_neighbor）模式。它们分别自 bonding 版本 3.3.0 和 3.4.0 起引入。)

	From Linux 3.0 and bonding version 3.7.1, these notifications
	are generated by the ipv4 and ipv6 code and the numbers of
	repetitions cannot be set independently.
	(自 Linux 3.0 与 bonding 版本 3.7.1 起，这些通知由 ipv4 和 ipv6 代码生成，重复次数无法独立设置。)

packets_per_slave
(每个从设备发送的数据包数)

	Specify the number of packets to transmit through a slave before
	moving to the next one. When set to 0 then a slave is chosen at
	random.
	(指定在切换到下一个从设备之前经由一个从设备发送的数据包数量。设为 0 时随机选择从设备。)

	The valid range is 0 - 65535; the default value is 1. This option
	has effect only in balance-rr mode.
	(有效范围是 0 - 65535，默认值为 1。该选项仅在 balance-rr 模式下生效。)

peer_notif_delay
(对端通知间隔)

	Specify the delay, in milliseconds, between each peer
	notification (gratuitous ARP and unsolicited IPv6 Neighbor
	Advertisement) when they are issued after a failover event.
	This delay should be a multiple of the MII link monitor interval
	(miimon).
	(指定故障转移事件后发出的各对端通知（免费 ARP 和主动 IPv6 邻居通告）之间的延迟，以毫秒为单位。该延迟应为 MII 链路监控间隔 (miimon) 的整数倍。)

	The valid range is 0 - 300000. The default value is 0, which means
	to match the value of the MII link monitor interval.
	(有效范围是 0 - 300000。默认值为 0，表示与 MII 链路监控间隔的值保持一致。)

prio
(从设备优先级)

	Slave priority. A higher number means higher priority.
	The primary slave has the highest priority. This option also
	follows the primary_reselect rules.
	(从设备优先级。数值越大优先级越高。主从设备 (primary slave) 具有最高优先级。该选项同样遵循 primary_reselect 规则。)

	This option could only be configured via netlink, and is only valid
	for active-backup(1), balance-tlb (5) and balance-alb (6) mode.
	The valid value range is a signed 32 bit integer.
	(该选项只能通过 netlink 配置，并且仅对 active-backup (1)、balance-tlb (5) 和 balance-alb (6) 模式有效。有效取值范围为带符号 32 位整数。)

	The default value is 0.
	(默认值为 0。)

primary
(主从设备)

	A string (eth0, eth2, etc) specifying which slave is the
	primary device.  The specified device will always be the
	active slave while it is available.  Only when the primary is
	off-line will alternate devices be used.  This is useful when
	one slave is preferred over another, e.g., when one slave has
	higher throughput than another.
	(一个字符串（eth0、eth2 等），指定哪个从设备是主设备。只要指定的设备可用，它就始终是活动从设备。只有主设备离线时才会改用其他设备。当某个从设备比其他从设备更受青睐时（例如某个从设备的吞吐量更高），该选项很有用。)

	The primary option is only valid for active-backup(1),
	balance-tlb (5) and balance-alb (6) mode.
	(primary 选项仅对 active-backup (1)、balance-tlb (5) 和 balance-alb (6) 模式有效。)

primary_reselect
(主从设备重新选择策略)

	Specifies the reselection policy for the primary slave.  This
	affects how the primary slave is chosen to become the active slave
	when failure of the active slave or recovery of the primary slave
	occurs.  This option is designed to prevent flip-flopping between
	the primary slave and other slaves.  Possible values are:
	(指定主从设备的重新选择策略。它影响在活动从设备失效或主从设备恢复时，如何选择主从设备成为活动从设备。该选项用于防止主从设备与其他从设备之间来回切换。可能的取值有：)

	always or 0 (default)
	(always 或 0（默认）)

		The primary slave becomes the active slave whenever it
		comes back up.
		(只要主从设备恢复 up，它就成为活动从设备。)

	better or 1
	(better 或 1)

		The primary slave becomes the active slave when it comes
		back up, if the speed and duplex of the primary slave is
		better than the speed and duplex of the current active
		slave.
		(主从设备恢复 up 时，若其速率与双工模式优于当前活动从设备，则成为活动从设备。)

	failure or 2
	(failure 或 2)

		The primary slave becomes the active slave only if the
		current active slave fails and the primary slave is up.
		(仅当当前活动从设备失效且主从设备处于 up 状态时，主从设备才成为活动从设备。)

	The primary_reselect setting is ignored in two cases:
	(在以下两种情况下，primary_reselect 设置会被忽略：)

		If no slaves are active, the first slave to recover is
		made the active slave.
		(如果没有活动的从设备，第一个恢复的从设备将成为活动从设备。)

		When initially enslaved, the primary slave is always made
		the active slave.
		(初次绑定时，主从设备总是被设为活动从设备。)

	Changing the primary_reselect policy via sysfs will cause an
	immediate selection of the best active slave according to the new
	policy.  This may or may not result in a change of the active
	slave, depending upon the circumstances.
	(通过 sysfs 更改 primary_reselect 策略会立即按新策略选出最佳活动从设备。这是否会改变活动从设备取决于具体情况。)

	This option was added for bonding version 3.6.0.
	(该选项自 bonding 版本 3.6.0 起引入。)

tlb_dynamic_lb
(tlb/alb 动态流迁移开关)

	Specifies if dynamic shuffling of flows is enabled in tlb
	or alb mode. The value has no effect on any other modes.
	(指定在 tlb 或 alb 模式下是否启用流的动态迁移。该值对其他任何模式都没有影响。)

	The default behavior of tlb mode is to shuffle active flows across
	slaves based on the load in that interval. This gives nice lb
	characteristics but can cause packet reordering. If re-ordering is
	a concern use this variable to disable flow shuffling and rely on
	load balancing provided solely by the hash distribution.
	xmit-hash-policy can be used to select the appropriate hashing for
	the setup.
	(tlb 模式的默认行为是基于该时间间隔内的负载在从设备之间迁移活动流。这带来了良好的负载均衡特性，但可能导致数据包乱序。如果担心乱序问题，可用此变量禁用流迁移，只依赖哈希分发提供的负载均衡。可使用 xmit-hash-policy 为具体环境选择合适的哈希方式。)

	The sysfs entry can be used to change the setting per bond device
	and the initial value is derived from the module parameter. The
	sysfs entry is allowed to be changed only if the bond device is
	down.
	(可通过对应的 sysfs 条目按 bond 设备更改该设置，其初始值来自模块参数。仅当 bond 设备处于 down 状态时才允许更改 sysfs 条目。)

	The default value is "1" that enables flow shuffling while value "0"
	disables it. This option was added in bonding driver 3.7.1
	(默认值为 "1"，表示启用流迁移；值 "0" 则禁用。该选项自 bonding 驱动 3.7.1 起引入。)


updelay
(启用从设备前的延迟)

	Specifies the time, in milliseconds, to wait before enabling a
	slave after a link recovery has been detected.  This option is
	only valid for the miimon link monitor.  The updelay value
	should be a multiple of the miimon value; if not, it will be
	rounded down to the nearest multiple.  The default value is 0.
	(以毫秒为单位，指定检测到链路恢复后启用从设备前的等待时间。该选项仅对 miimon 链路监控有效。updelay 的值应为 miimon 值的整数倍；若不是，则向下取整到最近的倍数。默认值为 0。)

use_carrier
(已废弃的链路状态检测方式选项)

	Obsolete option that previously selected between MII /
	ETHTOOL ioctls and netif_carrier_ok() to determine link
	state.
	(已废弃的选项，过去用于在 MII/ETHTOOL ioctl 与 netif_carrier_ok() 之间选择确定链路状态的方式。)

	All link state checks are now done with netif_carrier_ok().
	(现在所有链路状态检查都通过 netif_carrier_ok() 完成。)

	For backwards compatibility, this option's value may be inspected
	or set.  The only valid setting is 1.
	(为了向后兼容，仍可查看或设置该选项的值。唯一有效的设置值是 1。)

xmit_hash_policy
(发送哈希策略)

	Selects the transmit hash policy to use for slave selection in
	balance-xor, 802.3ad, and tlb modes.  Possible values are:
	(选择在 balance-xor、802.3ad 和 tlb 模式下用于从设备选择的发送哈希策略。可能的取值有：)

	layer2
	(layer2)

		Uses XOR of hardware MAC addresses and packet type ID
		field to generate the hash. The formula is
		(使用硬件 MAC 地址与数据包类型 ID 字段的异或 (XOR) 来生成哈希。公式为：)

		hash = source MAC[5] XOR destination MAC[5] XOR packet type ID
		slave number = hash modulo slave count

		This algorithm will place all traffic to a particular
		network peer on the same slave.
		(该算法会把发往特定网络对端的全部流量放到同一个从设备上。)

		This algorithm is 802.3ad compliant.
		(该算法符合 802.3ad 规范。)

	layer2+3
	(layer2+3)

		This policy uses a combination of layer2 and layer3
		protocol information to generate the hash.
		(该策略组合使用二层与三层协议信息来生成哈希。)

		Uses XOR of hardware MAC addresses and IP addresses to
		generate the hash.  The formula is
		(使用硬件 MAC 地址与 IP 地址的异或来生成哈希。公式为：)

		hash = source MAC[5] XOR destination MAC[5] XOR packet type ID
		hash = hash XOR source IP XOR destination IP
		hash = hash XOR (hash RSHIFT 16)
		hash = hash XOR (hash RSHIFT 8)
		And then hash is reduced modulo slave count.

		If the protocol is IPv6 then the source and destination
		addresses are first hashed using ipv6_addr_hash.
		(如果协议是 IPv6，则源地址与目的地址先用 ipv6_addr_hash 做哈希处理。)

		This algorithm will place all traffic to a particular
		network peer on the same slave.  For non-IP traffic,
		the formula is the same as for the layer2 transmit
		hash policy.
		(该算法会把发往特定网络对端的全部流量放到同一个从设备上。对于非 IP 流量，公式与 layer2 发送哈希策略相同。)

		This policy is intended to provide a more balanced
		distribution of traffic than layer2 alone, especially
		in environments where a layer3 gateway device is
		required to reach most destinations.
		(该策略旨在比单独使用 layer2 提供更均衡的流量分布，尤其是在需要借助三层网关设备才能到达大多数目的地的环境中。)

		This algorithm is 802.3ad compliant.
		(该算法符合 802.3ad 规范。)

	layer3+4
	(layer3+4)

		This policy uses upper layer protocol information,
		when available, to generate the hash.  This allows for
		traffic to a particular network peer to span multiple
		slaves, although a single connection will not span
		multiple slaves.
		(该策略在可用时使用上层协议信息来生成哈希。这使得发往特定网络对端的流量可以分布在多个从设备上，但单条连接不会跨越多个从设备。)

		The formula for unfragmented TCP and UDP packets is
		(对于未分片的 TCP 与 UDP 数据包，公式为：)

		hash = source port, destination port (as in the header)
		hash = hash XOR source IP XOR destination IP
		hash = hash XOR (hash RSHIFT 16)
		hash = hash XOR (hash RSHIFT 8)
		hash = hash RSHIFT 1
		And then hash is reduced modulo slave count.

		If the protocol is IPv6 then the source and destination
		addresses are first hashed using ipv6_addr_hash.
		(如果协议是 IPv6，则源地址与目的地址先用 ipv6_addr_hash 做哈希处理。)

		For fragmented TCP or UDP packets and all other IPv4 and
		IPv6 protocol traffic, the source and destination port
		information is omitted.  For non-IP traffic, the
		formula is the same as for the layer2 transmit hash
		policy.
		(对于已分片的 TCP 或 UDP 数据包以及其他所有 IPv4、IPv6 协议流量，源端口与目的端口信息会被省略。对于非 IP 流量，公式与 layer2 发送哈希策略相同。)

		This algorithm is not fully 802.3ad compliant.  A
		single TCP or UDP conversation containing both
		fragmented and unfragmented packets will see packets
		striped across two interfaces.  This may result in out
		of order delivery.  Most traffic types will not meet
		this criteria, as TCP rarely fragments traffic, and
		most UDP traffic is not involved in extended
		conversations.  Other implementations of 802.3ad may
		or may not tolerate this noncompliance.
		(该算法并非完全符合 802.3ad 规范。一条同时包含分片与非分片数据包的 TCP 或 UDP 会话，其数据包会被条带化分布到两个接口上，这可能导致乱序送达。大多数流量类型不会满足这一条件，因为 TCP 很少对流量分片，而大多数 UDP 流量也不涉及长时间的持续会话。其他 802.3ad 实现对这种不合规情况的容忍程度不一。)

	encap2+3
	(encap2+3)

		This policy uses the same formula as layer2+3 but it
		relies on skb_flow_dissect to obtain the header fields
		which might result in the use of inner headers if an
		encapsulation protocol is used. For example this will
		improve the performance for tunnel users because the
		packets will be distributed according to the encapsulated
		flows.
		(该策略使用与 layer2+3 相同的公式，但依靠 skb_flow_dissect 获取报文字段；在使用封装协议时可能改用内层报文头。例如，这将提升隧道使用者的性能，因为数据包会按封装后的流进行分发。)

	encap3+4
	(encap3+4)

		This policy uses the same formula as layer3+4 but it
		relies on skb_flow_dissect to obtain the header fields
		which might result in the use of inner headers if an
		encapsulation protocol is used. For example this will
		improve the performance for tunnel users because the
		packets will be distributed according to the encapsulated
		flows.
		(该策略使用与 layer3+4 相同的公式，但依靠 skb_flow_dissect 获取报文字段；在使用封装协议时可能改用内层报文头。例如，这将提升隧道使用者的性能，因为数据包会按封装后的流进行分发。)

	vlan+srcmac
	(vlan+srcmac)

		This policy uses a very rudimentary vlan ID and source mac
		hash to load-balance traffic per-vlan, with failover
		should one leg fail. The intended use case is for a bond
		shared by multiple virtual machines, all configured to
		use their own vlan, to give lacp-like functionality
		without requiring lacp-capable switching hardware.
		(该策略使用非常简单的 vlan ID 与源 MAC 哈希，按 VLAN 对流量做负载均衡，并在其中一条链路失效时进行故障转移。预期的使用场景是：多个虚拟机共享一个 bond，每个虚拟机都配置使用自己的 VLAN，从而在不需要支持 LACP 的交换机硬件的情况下提供类似 LACP 的功能。)

		The formula for the hash is simply
		(哈希公式很简单：)

		hash = (vlan ID) XOR (source MAC vendor) XOR (source MAC dev)

	The default value is layer2.  This option was added in bonding
	version 2.6.3.  In earlier versions of bonding, this parameter
	does not exist, and the layer2 policy is the only policy.  The
	layer2+3 value was added for bonding version 3.2.2.
	(默认值为 layer2。该选项自 bonding 版本 2.6.3 起引入。在更早版本的 bonding 中不存在该参数，layer2 是唯一的策略。layer2+3 取值自 bonding 版本 3.2.2 起引入。)

resend_igmp
(IGMP 成员关系报告重发次数)

	Specifies the number of IGMP membership reports to be issued after
	a failover event. One membership report is issued immediately after
	the failover, subsequent packets are sent in each 200ms interval.
	(指定故障转移事件后要发出的 IGMP 成员关系报告数量。故障转移后立即发出一份成员关系报告，后续数据包以每 200ms 一份的间隔发送。)

	The valid range is 0 - 255; the default value is 1. A value of 0
	prevents the IGMP membership report from being issued in response
	to the failover event.
	(有效范围是 0 - 255，默认值为 1。取值 0 表示不在故障转移事件后发出 IGMP 成员关系报告。)

	This option is useful for bonding modes balance-rr (0), active-backup
	(1), balance-tlb (5) and balance-alb (6), in which a failover can
	switch the IGMP traffic from one slave to another.  Therefore a fresh
	IGMP report must be issued to cause the switch to forward the incoming
	IGMP traffic over the newly selected slave.
	(该选项对 balance-rr (0)、active-backup (1)、balance-tlb (5) 和 balance-alb (6) 这些 bonding 模式很有用，因为这些模式下故障转移可能把 IGMP 流量从一个从设备切换到另一个。因此必须重新发出一份 IGMP 报告，促使交换机把入站 IGMP 流量转发到新选定的从设备上。)

	This option was added for bonding version 3.7.0.
	(该选项自 bonding 版本 3.7.0 起引入。)

lp_interval
(学习包发送间隔)

	Specifies the number of seconds between instances where the bonding
	driver sends learning packets to each slaves peer switch.
	(指定 bonding 驱动向每个从设备的对端交换机发送学习包的间隔秒数。)

	The valid range is 1 - 0x7fffffff; the default value is 1. This Option
	has effect only in balance-tlb and balance-alb modes.
	(有效范围是 1 - 0x7fffffff，默认值为 1。该选项仅在 balance-tlb 和 balance-alb 模式下生效。)

3. Configuring Bonding Devices(3. 配置 bonding 设备)
====================================================

You can configure bonding using either your distro's network
initialization scripts, or manually using either iproute2 or the
sysfs interface.  Distros generally use one of three packages for the
network initialization scripts: initscripts, sysconfig or interfaces.
Recent versions of these packages have support for bonding, while older
versions do not.
(你可以使用发行版的网络初始化脚本配置 bonding，也可以用 iproute2 或 sysfs 接口手动配置。发行版的网络初始化脚本通常使用三种软件包之一：initscripts、sysconfig 或 interfaces。这些软件包的较新版本支持 bonding，旧版本则不支持。)

We will first describe the options for configuring bonding for
distros using versions of initscripts, sysconfig and interfaces with full
or partial support for bonding, then provide information on enabling
bonding without support from the network initialization scripts (i.e.,
older versions of initscripts or sysconfig).
(我们先介绍在使用对 bonding 提供完整或部分支持的 initscripts、sysconfig 和 interfaces 版本的发行版上配置 bonding 的选项，然后介绍在没有网络初始化脚本支持的情况下（即旧版 initscripts 或 sysconfig）启用 bonding 的方法。)

If you're unsure whether your distro uses sysconfig,
initscripts or interfaces, or don't know if it's new enough, have no fear.
Determining this is fairly straightforward.
(如果你不确定发行版使用的是 sysconfig、initscripts 还是 interfaces，也不知道其版本是否足够新，不必担心。判别方法相当简单。)

First, look for a file called interfaces in /etc/network directory.
If this file is present in your system, then your system use interfaces. See
Configuration with Interfaces Support.
(首先，查看 /etc/network 目录下是否存在名为 interfaces 的文件。如果系统中存在该文件，那么你的系统使用 interfaces，请参阅 "Configuration with Interfaces Support" 一节。)

Else, issue the command::
(否则，执行以下命令：)

	$ rpm -qf /sbin/ifup

It will respond with a line of text starting with either
"initscripts" or "sysconfig," followed by some numbers.  This is the
package that provides your network initialization scripts.
(它会输出一行文本，以 "initscripts" 或 "sysconfig" 开头，后面跟一些数字。这就是提供网络初始化脚本的软件包。)

Next, to determine if your installation supports bonding,
issue the command::
(接下来，要确定你的系统是否支持 bonding，执行以下命令：)

    $ grep ifenslave /sbin/ifup

If this returns any matches, then your initscripts or
sysconfig has support for bonding.
(如果返回任何匹配项，说明你的 initscripts 或 sysconfig 支持 bonding。)

3.1 Configuration with Sysconfig Support(3.1 使用 sysconfig 支持进行配置)
-------------------------------------------------------------------------

This section applies to distros using a version of sysconfig
with bonding support, for example, SuSE Linux Enterprise Server 9.
(本节适用于使用带 bonding 支持的 sysconfig 版本的发行版，例如 SuSE Linux Enterprise Server 9。)

SuSE SLES 9's networking configuration system does support
bonding, however, at this writing, the YaST system configuration
front end does not provide any means to work with bonding devices.
Bonding devices can be managed by hand, however, as follows.
(SuSE SLES 9 的网络配置系统确实支持 bonding，但撰写本文时，YaST 系统配置前端尚未提供管理 bonding 设备的任何手段。不过，可以按如下方式手工管理 bonding 设备。)

First, if they have not already been configured, configure the
slave devices.  On SLES 9, this is most easily done by running the
yast2 sysconfig configuration utility.  The goal is for to create an
ifcfg-id file for each slave device.  The simplest way to accomplish
this is to configure the devices for DHCP (this is only to get the
file ifcfg-id file created; see below for some issues with DHCP).  The
name of the configuration file for each device will be of the form::
(首先，如果从设备尚未配置，先配置各从设备。在 SLES 9 上，最简单的方法是运行 yast2 sysconfig 配置工具。目标是为每个从设备创建一个 ifcfg-id 文件。最简单的做法是将这些设备配置为 DHCP（这只是为了生成 ifcfg-id 文件；使用 DHCP 存在的一些问题见下文）。每个设备的配置文件名形如：)

    ifcfg-id-xx:xx:xx:xx:xx:xx

Where the "xx" portion will be replaced with the digits from
the device's permanent MAC address.
(其中 "xx" 部分将被替换为设备永久 MAC 地址中的数字。)

Once the set of ifcfg-id-xx:xx:xx:xx:xx:xx files has been
created, it is necessary to edit the configuration files for the slave
devices (the MAC addresses correspond to those of the slave devices).
Before editing, the file will contain multiple lines, and will look
something like this::
(创建好这组 ifcfg-id-xx:xx:xx:xx:xx:xx 文件后，需要编辑各从设备的配置文件（其中的 MAC 地址与从设备一一对应）。编辑之前，文件内容有多行，大致如下：)

	BOOTPROTO='dhcp'
	STARTMODE='on'
	USERCTL='no'
	UNIQUE='XNzu.WeZGOGF+4wE'
	_nm_name='bus-pci-0001:61:01.0'

Change the BOOTPROTO and STARTMODE lines to the following::
(把 BOOTPROTO 和 STARTMODE 两行改成如下内容：)

	BOOTPROTO='none'
	STARTMODE='off'

Do not alter the UNIQUE or _nm_name lines.  Remove any other
lines (USERCTL, etc).
(不要改动 UNIQUE 或 _nm_name 行。删除其余所有行（USERCTL 等）。)

Once the ifcfg-id-xx:xx:xx:xx:xx:xx files have been modified,
it's time to create the configuration file for the bonding device
itself.  This file is named ifcfg-bondX, where X is the number of the
bonding device to create, starting at 0.  The first such file is
ifcfg-bond0, the second is ifcfg-bond1, and so on.  The sysconfig
network configuration system will correctly start multiple instances
of bonding.
(修改完 ifcfg-id-xx:xx:xx:xx:xx:xx 文件后，接下来创建 bonding 设备自身的配置文件。该文件名为 ifcfg-bondX，其中 X 是要创建的 bonding 设备编号，从 0 开始。第一个文件是 ifcfg-bond0，第二个是 ifcfg-bond1，依此类推。sysconfig 网络配置系统能够正确启动多个 bonding 实例。)

The contents of the ifcfg-bondX file is as follows::
(ifcfg-bondX 文件的内容如下：)

	BOOTPROTO="static"
	BROADCAST="10.0.2.255"
	IPADDR="10.0.2.10"
	NETMASK="255.255.0.0"
	NETWORK="10.0.2.0"
	REMOTE_IPADDR=""
	STARTMODE="onboot"
	BONDING_MASTER="yes"
	BONDING_MODULE_OPTS="mode=active-backup miimon=100"
	BONDING_SLAVE0="eth0"
	BONDING_SLAVE1="bus-pci-0000:06:08.1"

Replace the sample BROADCAST, IPADDR, NETMASK and NETWORK
values with the appropriate values for your network.
(将示例中的 BROADCAST、IPADDR、NETMASK 和 NETWORK 值替换为你所在网络的适当值。)

The STARTMODE specifies when the device is brought online.
The possible values are:
(STARTMODE 指定设备何时上线。可能的取值有：)

	======== ======================================================
	onboot	 The device is started at boot time.  If you're not
		 sure, this is probably what you want.
		 (设备在系统引导时启动。如果不确定，这很可能就是你要的选项。)

	manual	 The device is started only when ifup is called
		 manually.  Bonding devices may be configured this
		 way if you do not wish them to start automatically
		 at boot for some reason.
		 (设备仅在手工调用 ifup 时启动。如果出于某些原因不希望 bonding 设备在引导时自动启动，可以按这种方式配置。)

	hotplug  The device is started by a hotplug event.  This is not
		 a valid choice for a bonding device.
		 (设备由热插拔事件启动。对 bonding 设备来说这不是有效选项。)

	off or   The device configuration is ignored. (设备配置被忽略。)
	ignore
	======== ======================================================

The line BONDING_MASTER='yes' indicates that the device is a
bonding master device.  The only useful value is "yes."
(BONDING_MASTER='yes' 一行表明该设备是 bonding 主设备。唯一有用的取值是 "yes"。)

The contents of BONDING_MODULE_OPTS are supplied to the
instance of the bonding module for this device.  Specify the options
for the bonding mode, link monitoring, and so on here.  Do not include
the max_bonds bonding parameter; this will confuse the configuration
system if you have multiple bonding devices.
(BONDING_MODULE_OPTS 的内容会传递给该设备对应的 bonding 模块实例。请在此指定 bonding 模式、链路监控等选项。不要包含 max_bonds 这个 bonding 参数；如果配置了多个 bonding 设备，它会使配置系统混乱。)

Finally, supply one BONDING_SLAVEn="slave device" for each
slave.  where "n" is an increasing value, one for each slave.  The
"slave device" is either an interface name, e.g., "eth0", or a device
specifier for the network device.  The interface name is easier to
find, but the ethN names are subject to change at boot time if, e.g.,
a device early in the sequence has failed.  The device specifiers
(bus-pci-0000:06:08.1 in the example above) specify the physical
network device, and will not change unless the device's bus location
changes (for example, it is moved from one PCI slot to another).  The
example above uses one of each type for demonstration purposes; most
configurations will choose one or the other for all slave devices.
(最后，为每个从设备提供一个 BONDING_SLAVEn="slave device"。其中 "n" 是递增的编号，每个从设备一个。"slave device" 可以是接口名（如 "eth0"），也可以是网络设备的设备标识符。接口名更容易查找，但如果序列中较早的设备发生故障，ethN 名称在引导时可能发生变化。设备标识符（如上例中的 bus-pci-0000:06:08.1）指定的是物理网络设备，除非设备的总线位置改变（例如从一个 PCI 插槽移到另一个），否则不会变化。上例为了演示使用了两种类型各一个；大多数配置会为所有从设备统一选用其中一种。)

When all configuration files have been modified or created,
networking must be restarted for the configuration changes to take
effect.  This can be accomplished via the following::
(所有配置文件修改或创建完毕后，必须重启网络才能使配置更改生效。可以通过以下方式完成：)

	# /etc/init.d/network restart

Note that the network control script (/sbin/ifdown) will
remove the bonding module as part of the network shutdown processing,
so it is not necessary to remove the module by hand if, e.g., the
module parameters have changed.
(注意，网络控制脚本 (/sbin/ifdown) 在关闭网络的过程中会移除 bonding 模块，因此例如模块参数发生变化时，无需手工移除该模块。)

Also, at this writing, YaST/YaST2 will not manage bonding
devices (they do not show bonding interfaces on its list of network
devices).  It is necessary to edit the configuration file by hand to
change the bonding configuration.
(另外，撰写本文时，YaST/YaST2 尚不能管理 bonding 设备（其网络设备列表中不显示 bonding 接口）。要修改 bonding 配置，必须手工编辑配置文件。)

Additional general options and details of the ifcfg file
format can be found in an example ifcfg template file::
(更多通用选项以及 ifcfg 文件格式的细节，可参见 ifcfg 模板示例文件：)

	/etc/sysconfig/network/ifcfg.template

Note that the template does not document the various ``BONDING_*``
settings described above, but does describe many of the other options.
(注意，该模板并未记录上述各种 ``BONDING_*`` 设置，但描述了许多其他选项。)

3.1.1 Using DHCP with Sysconfig(3.1.1 在 sysconfig 中使用 DHCP)
---------------------------------------------------------------

Under sysconfig, configuring a device with BOOTPROTO='dhcp'
will cause it to query DHCP for its IP address information.  At this
writing, this does not function for bonding devices; the scripts
attempt to obtain the device address from DHCP prior to adding any of
the slave devices.  Without active slaves, the DHCP requests are not
sent to the network.
(在 sysconfig 下，将设备配置为 BOOTPROTO='dhcp' 会使它向 DHCP 查询自己的 IP 地址信息。撰写本文时，这对 bonding 设备不起作用：脚本会在添加任何从设备之前先尝试从 DHCP 获取设备地址。没有活动从设备时，DHCP 请求不会发往网络。)

3.1.2 Configuring Multiple Bonds with Sysconfig(3.1.2 使用 sysconfig 配置多个 bond)
-----------------------------------------------------------------------------------

The sysconfig network initialization system is capable of
handling multiple bonding devices.  All that is necessary is for each
bonding instance to have an appropriately configured ifcfg-bondX file
(as described above).  Do not specify the "max_bonds" parameter to any
instance of bonding, as this will confuse sysconfig.  If you require
multiple bonding devices with identical parameters, create multiple
ifcfg-bondX files.
(sysconfig 网络初始化系统能够处理多个 bonding 设备。只需让每个 bonding 实例都有一个配置正确的 ifcfg-bondX 文件（如上所述）即可。不要给任何 bonding 实例指定 "max_bonds" 参数，否则会使 sysconfig 混乱。如果需要多个参数完全相同的 bonding 设备，创建多个 ifcfg-bondX 文件即可。)

Because the sysconfig scripts supply the bonding module
options in the ifcfg-bondX file, it is not necessary to add them to
the system ``/etc/modules.d/*.conf`` configuration files.
(由于 sysconfig 脚本已在 ifcfg-bondX 文件中提供 bonding 模块选项，因此无需再把它们添加到系统的 ``/etc/modules.d/*.conf`` 配置文件中。)

3.2 Configuration with Initscripts Support(3.2 使用 initscripts 支持进行配置)
-----------------------------------------------------------------------------

This section applies to distros using a recent version of
initscripts with bonding support, for example, Red Hat Enterprise Linux
version 3 or later, Fedora, etc.  On these systems, the network
initialization scripts have knowledge of bonding, and can be configured to
control bonding devices.  Note that older versions of the initscripts
package have lower levels of support for bonding; this will be noted where
applicable.
(本节适用于使用带 bonding 支持的新版 initscripts 的发行版，例如 Red Hat Enterprise Linux 3 或更高版本、Fedora 等。在这些系统上，网络初始化脚本了解 bonding，可以配置为控制 bonding 设备。注意，旧版 initscripts 软件包对 bonding 的支持程度较低；适用的地方会予以说明。)

These distros will not automatically load the network adapter
driver unless the ethX device is configured with an IP address.
Because of this constraint, users must manually configure a
network-script file for all physical adapters that will be members of
a bondX link.  Network script files are located in the directory:
(除非 ethX 设备配置了 IP 地址，否则这些发行版不会自动加载网络适配器驱动。受此限制，用户必须为将成为 bondX 链路成员的所有物理适配器手工配置网络脚本文件。网络脚本文件位于以下目录：)

/etc/sysconfig/network-scripts

The file name must be prefixed with "ifcfg-eth" and suffixed
with the adapter's physical adapter number.  For example, the script
for eth0 would be named /etc/sysconfig/network-scripts/ifcfg-eth0.
Place the following text in the file::
(文件名必须以 "ifcfg-eth" 开头，以适配器的物理适配器编号结尾。例如，eth0 的脚本应命名为 /etc/sysconfig/network-scripts/ifcfg-eth0。在文件中放入以下文本：)

	DEVICE=eth0
	USERCTL=no
	ONBOOT=yes
	MASTER=bond0
	SLAVE=yes
	BOOTPROTO=none

The DEVICE= line will be different for every ethX device and
must correspond with the name of the file, i.e., ifcfg-eth1 must have
a device line of DEVICE=eth1.  The setting of the MASTER= line will
also depend on the final bonding interface name chosen for your bond.
As with other network devices, these typically start at 0, and go up
one for each device, i.e., the first bonding instance is bond0, the
second is bond1, and so on.
(每个 ethX 设备的 DEVICE= 行各不相同，且必须与文件名对应，即 ifcfg-eth1 必须有 DEVICE=eth1 设备行。MASTER= 行的设置也取决于你为 bond 选定的最终 bonding 接口名。与其他网络设备一样，这些编号通常从 0 开始，每个设备递增 1，即第一个 bonding 实例是 bond0，第二个是 bond1，依此类推。)

Next, create a bond network script.  The file name for this
script will be /etc/sysconfig/network-scripts/ifcfg-bondX where X is
the number of the bond.  For bond0 the file is named "ifcfg-bond0",
for bond1 it is named "ifcfg-bond1", and so on.  Within that file,
place the following text::
(接着，创建 bond 的网络脚本。该脚本的文件名将是 /etc/sysconfig/network-scripts/ifcfg-bondX，其中 X 是 bond 的编号。bond0 对应的文件名为 "ifcfg-bond0"，bond1 为 "ifcfg-bond1"，依此类推。在该文件中放入以下文本：)

	DEVICE=bond0
	IPADDR=192.168.1.1
	NETMASK=255.255.255.0
	NETWORK=192.168.1.0
	BROADCAST=192.168.1.255
	ONBOOT=yes
	BOOTPROTO=none
	USERCTL=no

Be sure to change the networking specific lines (IPADDR,
NETMASK, NETWORK and BROADCAST) to match your network configuration.
(务必将网络相关的各行（IPADDR、NETMASK、NETWORK 和 BROADCAST）改成与你所在网络配置相匹配的值。)

For later versions of initscripts, such as that found with Fedora
7 (or later) and Red Hat Enterprise Linux version 5 (or later), it is possible,
and, indeed, preferable, to specify the bonding options in the ifcfg-bond0
file, e.g. a line of the format::
(对于较新的 initscripts 版本，例如 Fedora 7（或更高）以及 Red Hat Enterprise Linux 5（或更高）所带的版本，可以在 ifcfg-bond0 文件中指定 bonding 选项，而且这样做更可取，例如如下格式的一行：)

  BONDING_OPTS="mode=active-backup arp_interval=60 arp_ip_target=192.168.1.254"

will configure the bond with the specified options.  The options
specified in BONDING_OPTS are identical to the bonding module parameters
except for the arp_ip_target field when using versions of initscripts older
than and 8.57 (Fedora 8) and 8.45.19 (Red Hat Enterprise Linux 5.2).  When
using older versions each target should be included as a separate option and
should be preceded by a '+' to indicate it should be added to the list of
queried targets, e.g.,::
(该行会用指定的选项配置 bond。BONDING_OPTS 中指定的选项与 bonding 模块参数完全相同，但使用早于 8.57 (Fedora 8) 和 8.45.19 (Red Hat Enterprise Linux 5.2) 版本的 initscripts 时 arp_ip_target 字段除外。使用旧版本时，每个目标都应作为单独的选项包含，且应在前面加一个 '+'，表示要将其加入查询目标列表，例如：)

    arp_ip_target=+192.168.1.1 arp_ip_target=+192.168.1.2

is the proper syntax to specify multiple targets.  When specifying
options via BONDING_OPTS, it is not necessary to edit
``/etc/modprobe.d/*.conf``.
(这是指定多个目标的正确语法。通过 BONDING_OPTS 指定选项时，无需编辑 ``/etc/modprobe.d/*.conf``。)

For even older versions of initscripts that do not support
BONDING_OPTS, it is necessary to edit /etc/modprobe.d/*.conf, depending upon
your distro) to load the bonding module with your desired options when the
bond0 interface is brought up.  The following lines in /etc/modprobe.d/*.conf
will load the bonding module, and select its options:
(对于不支持 BONDING_OPTS 的更旧版本 initscripts，需要编辑 /etc/modprobe.d/*.conf（视发行版而定），以便在 bond0 接口启动时以所需选项加载 bonding 模块。/etc/modprobe.d/*.conf 中的以下几行会加载 bonding 模块并选定其选项：)

	alias bond0 bonding
	options bond0 mode=balance-alb miimon=100

Replace the sample parameters with the appropriate set of
options for your configuration.
(将示例参数替换为适合你配置的选项组合。)

Finally run "/etc/rc.d/init.d/network restart" as root.  This
will restart the networking subsystem and your bond link should be now
up and running.
(最后以 root 身份运行 "/etc/rc.d/init.d/network restart"。这将重启网络子系统，你的 bond 链路现在应该已经 up 并正常运行。)

3.2.1 Using DHCP with Initscripts(3.2.1 在 initscripts 中使用 DHCP)
-------------------------------------------------------------------

Recent versions of initscripts (the versions supplied with Fedora
Core 3 and Red Hat Enterprise Linux 4, or later versions, are reported to
work) have support for assigning IP information to bonding devices via
DHCP.
(较新版本的 initscripts（据报告，Fedora Core 3 和 Red Hat Enterprise Linux 4 所带版本或更高版本可用）支持通过 DHCP 为 bonding 设备分配 IP 信息。)

To configure bonding for DHCP, configure it as described
above, except replace the line "BOOTPROTO=none" with "BOOTPROTO=dhcp"
and add a line consisting of "TYPE=Bonding".  Note that the TYPE value
is case sensitive.
(要把 bonding 配置为使用 DHCP，按上文所述配置，只是把 "BOOTPROTO=none" 一行换成 "BOOTPROTO=dhcp"，并添加一行 "TYPE=Bonding"。注意 TYPE 的值区分大小写。)

3.2.2 Configuring Multiple Bonds with Initscripts(3.2.2 使用 initscripts 配置多个 bond)
---------------------------------------------------------------------------------------

Initscripts packages that are included with Fedora 7 and Red Hat
Enterprise Linux 5 support multiple bonding interfaces by simply
specifying the appropriate BONDING_OPTS= in ifcfg-bondX where X is the
number of the bond.  This support requires sysfs support in the kernel,
and a bonding driver of version 3.0.0 or later.  Other configurations may
not support this method for specifying multiple bonding interfaces; for
those instances, see the "Configuring Multiple Bonds Manually" section,
below.
(Fedora 7 和 Red Hat Enterprise Linux 5 所带的 initscripts 软件包支持多个 bonding 接口，只需在 ifcfg-bondX（X 为 bond 编号）中指定相应的 BONDING_OPTS= 即可。该支持要求内核具备 sysfs 支持，且 bonding 驱动版本为 3.0.0 或更高。其他配置可能不支持这种指定多个 bonding 接口的方法；这类情况请参阅下文 "Configuring Multiple Bonds Manually" 一节。)

3.3 Configuring Bonding Manually with iproute2(3.3 使用 iproute2 手动配置 bonding)
----------------------------------------------------------------------------------

This section applies to distros whose network initialization
scripts (the sysconfig or initscripts package) do not have specific
knowledge of bonding.  One such distro is SuSE Linux Enterprise Server
version 8.
(本节适用于其网络初始化脚本（sysconfig 或 initscripts 软件包）不了解 bonding 的发行版。SuSE Linux Enterprise Server 8 就是这样一种发行版。)

The general method for these systems is to place the bonding
module parameters into a config file in /etc/modprobe.d/ (as
appropriate for the installed distro), then add modprobe and/or
`ip link` commands to the system's global init script.  The name of
the global init script differs; for sysconfig, it is
/etc/init.d/boot.local and for initscripts it is /etc/rc.d/rc.local.
(这类系统的一般做法是把 bonding 模块参数写入 /etc/modprobe.d/ 下的一个配置文件（视所装发行版而定），然后把 modprobe 和/或 `ip link` 命令加入系统的全局 init 脚本。全局 init 脚本的名称各不相同：sysconfig 是 /etc/init.d/boot.local，initscripts 是 /etc/rc.d/rc.local。)

For example, if you wanted to make a simple bond of two e100
devices (presumed to be eth0 and eth1), and have it persist across
reboots, edit the appropriate file (/etc/init.d/boot.local or
/etc/rc.d/rc.local), and add the following::
(例如，如果想用两个 e100 设备（假定是 eth0 和 eth1）建立一个简单 bond，并让它在重启后依然生效，可以编辑相应的文件（/etc/init.d/boot.local 或 /etc/rc.d/rc.local），并添加以下内容：)

	modprobe bonding mode=balance-alb miimon=100
	modprobe e100
	ifconfig bond0 192.168.1.1 netmask 255.255.255.0 up
	ip link set eth0 master bond0
	ip link set eth1 master bond0

Replace the example bonding module parameters and bond0
network configuration (IP address, netmask, etc) with the appropriate
values for your configuration.
(将示例中的 bonding 模块参数和 bond0 网络配置（IP 地址、子网掩码等）替换为适合你环境的值。)

Unfortunately, this method will not provide support for the
ifup and ifdown scripts on the bond devices.  To reload the bonding
configuration, it is necessary to run the initialization script, e.g.,::
(遗憾的是，这种方法不支持在 bond 设备上使用 ifup 和 ifdown 脚本。要重新加载 bonding 配置，必须运行初始化脚本，例如：)

	# /etc/init.d/boot.local

or::
(或者：)

	# /etc/rc.d/rc.local

It may be desirable in such a case to create a separate script
which only initializes the bonding configuration, then call that
separate script from within boot.local.  This allows for bonding to be
enabled without re-running the entire global init script.
(在这种情况下，更可取的做法是创建一个只初始化 bonding 配置的独立脚本，然后在 boot.local 中调用该脚本。这样无需重新运行整个全局 init 脚本即可启用 bonding。)

To shut down the bonding devices, it is necessary to first
mark the bonding device itself as being down, then remove the
appropriate device driver modules.  For our example above, you can do
the following::
(要关闭 bonding 设备，必须先把 bonding 设备本身标记为 down，然后移除相应的设备驱动模块。对于上面的示例，可以执行以下操作：)

	# ifconfig bond0 down
	# rmmod bonding
	# rmmod e100

Again, for convenience, it may be desirable to create a script
with these commands.
(同样，为了方便，也可以把这些命令做成一个脚本。)


3.3.1 Configuring Multiple Bonds Manually(3.3.1 手动配置多个 bond)
------------------------------------------------------------------

This section contains information on configuring multiple
bonding devices with differing options for those systems whose network
initialization scripts lack support for configuring multiple bonds.
(本节介绍在网络初始化脚本不支持配置多个 bond 的系统上，如何为多个 bonding 设备配置不同选项。)

If you require multiple bonding devices, but all with the same
options, you may wish to use the "max_bonds" module parameter,
documented above.
(如果你需要多个 bonding 设备，但它们都使用相同选项，可以使用上文记录的 "max_bonds" 模块参数。)

To create multiple bonding devices with differing options, it is
preferable to use bonding parameters exported by sysfs, documented in the
section below.
(要创建使用不同选项的多个 bonding 设备，更可取的做法是使用 sysfs 导出的 bonding 参数，参见下一节的说明。)

For versions of bonding without sysfs support, the only means to
provide multiple instances of bonding with differing options is to load
the bonding driver multiple times.  Note that current versions of the
sysconfig network initialization scripts handle this automatically; if
your distro uses these scripts, no special action is needed.  See the
section Configuring Bonding Devices, above, if you're not sure about your
network initialization scripts.
(对于不支持 sysfs 的 bonding 版本，让多个 bonding 实例使用不同选项的唯一办法是多次加载 bonding 驱动。注意，当前版本的 sysconfig 网络初始化脚本会自动处理此事；如果你的发行版使用这些脚本，则无需特殊操作。如果不确定你的网络初始化脚本的情况，请参阅上文 "Configuring Bonding Devices" 一节。)

To load multiple instances of the module, it is necessary to
specify a different name for each instance (the module loading system
requires that every loaded module, even multiple instances of the same
module, have a unique name).  This is accomplished by supplying multiple
sets of bonding options in ``/etc/modprobe.d/*.conf``, for example::
(要加载模块的多个实例，必须为每个实例指定不同的名称（模块加载系统要求每个已加载的模块，即使是同一模块的多个实例，都拥有唯一的名称）。这可以通过在 ``/etc/modprobe.d/*.conf`` 中提供多组 bonding 选项来实现，例如：)

	alias bond0 bonding
	options bond0 -o bond0 mode=balance-rr miimon=100

	alias bond1 bonding
	options bond1 -o bond1 mode=balance-alb miimon=50

will load the bonding module two times.  The first instance is
named "bond0" and creates the bond0 device in balance-rr mode with an
miimon of 100.  The second instance is named "bond1" and creates the
bond1 device in balance-alb mode with an miimon of 50.
(上面会加载 bonding 模块两次。第一个实例名为 "bond0"，以 balance-rr 模式、miimon=100 创建 bond0 设备。第二个实例名为 "bond1"，以 balance-alb 模式、miimon=50 创建 bond1 设备。)

In some circumstances (typically with older distributions),
the above does not work, and the second bonding instance never sees
its options.  In that case, the second options line can be substituted
as follows::
(在某些情况下（通常是较老的发行版），上述方法不奏效，第二个 bonding 实例看不到自己的选项。此时可以把第二个选项行替换为如下形式：)

	install bond1 /sbin/modprobe --ignore-install bonding -o bond1 \
				     mode=balance-alb miimon=50

This may be repeated any number of times, specifying a new and
unique name in place of bond1 for each subsequent instance.
(此法可以重复任意多次，只要为后续每个实例指定一个替代 bond1 的新且唯一的名称。)

It has been observed that some Red Hat supplied kernels are unable
to rename modules at load time (the "-o bond1" part).  Attempts to pass
that option to modprobe will produce an "Operation not permitted" error.
This has been reported on some Fedora Core kernels, and has been seen on
RHEL 4 as well.  On kernels exhibiting this problem, it will be impossible
to configure multiple bonds with differing parameters (as they are older
kernels, and also lack sysfs support).
(据观察，一些 Red Hat 提供的内核无法在加载时重命名模块（即 "-o bond1" 部分）。尝试向 modprobe 传递该选项会产生 "Operation not permitted" 错误。这一问题在某些 Fedora Core 内核上有报告，RHEL 4 上也出现过。在存在此问题的内核上，将无法配置参数各不相同的多个 bond（因为这些内核较老，而且缺少 sysfs 支持）。)

3.4 Configuring Bonding Manually via Sysfs(3.4 通过 sysfs 手动配置 bonding)
---------------------------------------------------------------------------

Starting with version 3.0.0, Channel Bonding may be configured
via the sysfs interface.  This interface allows dynamic configuration
of all bonds in the system without unloading the module.  It also
allows for adding and removing bonds at runtime.  Ifenslave is no
longer required, though it is still supported.
(从 3.0.0 版本开始，可以通过 sysfs 接口配置通道绑定 (Channel Bonding)。该接口允许在不卸载模块的情况下动态配置系统中的所有 bond，还允许在运行时添加和删除 bond。不再需要 ifenslave，不过它仍然受支持。)

Use of the sysfs interface allows you to use multiple bonds
with different configurations without having to reload the module.
It also allows you to use multiple, differently configured bonds when
bonding is compiled into the kernel.
(使用 sysfs 接口，可以在不重新加载模块的情况下使用多个配置各异的 bond。当 bonding 编译进内核时，也可以使用多个不同配置的 bond。)

You must have the sysfs filesystem mounted to configure
bonding this way.  The examples in this document assume that you
are using the standard mount point for sysfs, e.g. /sys.  If your
sysfs filesystem is mounted elsewhere, you will need to adjust the
example paths accordingly.
(要用这种方式配置 bonding，必须已挂载 sysfs 文件系统。本文档的示例假定你使用 sysfs 的标准挂载点，例如 /sys。如果你的 sysfs 文件系统挂载在其他位置，需要相应调整示例中的路径。)

Creating and Destroying Bonds(创建和删除 bond)
----------------------------------------------
To add a new bond foo::
(要添加一个名为 foo 的新 bond：)

	# echo +foo > /sys/class/net/bonding_masters

To remove an existing bond bar::
(要删除一个已存在的 bond bar：)

	# echo -bar > /sys/class/net/bonding_masters

To show all existing bonds::
(要显示所有已存在的 bond：)

	# cat /sys/class/net/bonding_masters

.. note::

   due to 4K size limitation of sysfs files, this list may be
   truncated if you have more than a few hundred bonds.  This is unlikely
   to occur under normal operating conditions.
   (注意：由于 sysfs 文件有 4K 的大小限制，如果 bond 数量超过几百个，此列表可能被截断。正常运行条件下一般不会出现这种情况。)

Adding and Removing Slaves(添加和移除从设备)
--------------------------------------------
Interfaces may be enslaved to a bond using the file
/sys/class/net/<bond>/bonding/slaves.  The semantics for this file
are the same as for the bonding_masters file.
(可以使用文件 /sys/class/net/<bond>/bonding/slaves 将接口绑定到 bond。该文件的语义与 bonding_masters 文件相同。)

To enslave interface eth0 to bond bond0::
(要把接口 eth0 绑定到 bond bond0：)

	# ifconfig bond0 up
	# echo +eth0 > /sys/class/net/bond0/bonding/slaves

To free slave eth0 from bond bond0::
(要把从设备 eth0 从 bond bond0 中解除：)

	# echo -eth0 > /sys/class/net/bond0/bonding/slaves

When an interface is enslaved to a bond, symlinks between the
two are created in the sysfs filesystem.  In this case, you would get
/sys/class/net/bond0/slave_eth0 pointing to /sys/class/net/eth0, and
/sys/class/net/eth0/master pointing to /sys/class/net/bond0.
(当接口被绑定到某个 bond 时，sysfs 文件系统中会在两者之间创建符号链接。此时会得到指向 /sys/class/net/eth0 的 /sys/class/net/bond0/slave_eth0，以及指向 /sys/class/net/bond0 的 /sys/class/net/eth0/master。)

This means that you can tell quickly whether or not an
interface is enslaved by looking for the master symlink.  Thus:
# echo -eth0 > /sys/class/net/eth0/master/bonding/slaves
will free eth0 from whatever bond it is enslaved to, regardless of
the name of the bond interface.
(这意味着，只需查看 master 符号链接是否存在，就能快速判断某个接口是否已被绑定。因此：# echo -eth0 > /sys/class/net/eth0/master/bonding/slaves 可以把 eth0 从它所绑定的任何 bond 中解除，无论该 bond 接口叫什么名字。)

Changing a Bond's Configuration(修改 bond 的配置)
-------------------------------------------------
Each bond may be configured individually by manipulating the
files located in /sys/class/net/<bond name>/bonding
(每个 bond 都可以通过操作 /sys/class/net/<bond name>/bonding 目录下的文件单独配置。)

The names of these files correspond directly with the command-
line parameters described elsewhere in this file, and, with the
exception of arp_ip_target, they accept the same values.  To see the
current setting, simply cat the appropriate file.
(这些文件的名称与本文件其他地方描述的命令行参数一一对应，并且除 arp_ip_target 外接受相同的取值。要查看当前设置，只需 cat 相应的文件即可。)

A few examples will be given here; for specific usage
guidelines for each parameter, see the appropriate section in this
document.
(这里给出几个示例；每个参数的具体使用指南请参见本文档的相应章节。)

To configure bond0 for balance-alb mode::
(要把 bond0 配置为 balance-alb 模式：)

	# ifconfig bond0 down
	# echo 6 > /sys/class/net/bond0/bonding/mode
	- or -
	# echo balance-alb > /sys/class/net/bond0/bonding/mode

.. note::

   The bond interface must be down before the mode can be changed.
   (注意：必须先将 bond 接口置为 down，才能更改模式。)

To enable MII monitoring on bond0 with a 1 second interval::
(要在 bond0 上启用间隔为 1 秒的 MII 监控：)

	# echo 1000 > /sys/class/net/bond0/bonding/miimon

.. note::

   If ARP monitoring is enabled, it will disabled when MII
   monitoring is enabled, and vice-versa.
   (注意：如果启用了 ARP 监控，启用 MII 监控时会将其禁用，反之亦然。)

To add ARP targets::
(要添加 ARP 目标：)

	# echo +192.168.0.100 > /sys/class/net/bond0/bonding/arp_ip_target
	# echo +192.168.0.101 > /sys/class/net/bond0/bonding/arp_ip_target

.. note::

   up to 16 target addresses may be specified.
   (注意：最多可以指定 16 个目标地址。)

To remove an ARP target::
(要移除一个 ARP 目标：)

	# echo -192.168.0.100 > /sys/class/net/bond0/bonding/arp_ip_target

To configure the interval between learning packet transmits::
(要配置学习包发送的间隔：)

	# echo 12 > /sys/class/net/bond0/bonding/lp_interval

.. note::

   the lp_interval is the number of seconds between instances where
   the bonding driver sends learning packets to each slaves peer switch.  The
   default interval is 1 second.
   (注意：lp_interval 是 bonding 驱动向每个从设备的对端交换机发送学习包的间隔秒数。默认间隔为 1 秒。)

Example Configuration(配置示例)
-------------------------------
We begin with the same example that is shown in section 3.3,
executed with sysfs, and without using ifenslave.
(我们从 3.3 节所示的同一样例开始，改用 sysfs 执行，且不使用 ifenslave。)

To make a simple bond of two e100 devices (presumed to be eth0
and eth1), and have it persist across reboots, edit the appropriate
file (/etc/init.d/boot.local or /etc/rc.d/rc.local), and add the
following::
(要用两个 e100 设备（假定是 eth0 和 eth1）建立一个简单 bond，并让它在重启后依然生效，可以编辑相应的文件（/etc/init.d/boot.local 或 /etc/rc.d/rc.local），并添加以下内容：)

	modprobe bonding
	modprobe e100
	echo balance-alb > /sys/class/net/bond0/bonding/mode
	ifconfig bond0 192.168.1.1 netmask 255.255.255.0 up
	echo 100 > /sys/class/net/bond0/bonding/miimon
	echo +eth0 > /sys/class/net/bond0/bonding/slaves
	echo +eth1 > /sys/class/net/bond0/bonding/slaves

To add a second bond, with two e1000 interfaces in
active-backup mode, using ARP monitoring, add the following lines to
your init script::
(要添加第二个 bond，它由两个 e1000 接口组成、使用 active-backup 模式和 ARP 监控，请在你的 init 脚本中添加以下各行：)

	modprobe e1000
	echo +bond1 > /sys/class/net/bonding_masters
	echo active-backup > /sys/class/net/bond1/bonding/mode
	ifconfig bond1 192.168.2.1 netmask 255.255.255.0 up
	echo +192.168.2.100 /sys/class/net/bond1/bonding/arp_ip_target
	echo 2000 > /sys/class/net/bond1/bonding/arp_interval
	echo +eth2 > /sys/class/net/bond1/bonding/slaves
	echo +eth3 > /sys/class/net/bond1/bonding/slaves

3.5 Configuration with Interfaces Support(3.5 使用 interfaces 支持进行配置)
---------------------------------------------------------------------------

This section applies to distros which use /etc/network/interfaces file
to describe network interface configuration, most notably Debian and its
derivatives.
(本节适用于使用 /etc/network/interfaces 文件描述网络接口配置的发行版，最有代表性的是 Debian 及其衍生版。)

The ifup and ifdown commands on Debian don't support bonding out of
the box. The ifenslave-2.6 package should be installed to provide bonding
support.  Once installed, this package will provide ``bond-*`` options
to be used into /etc/network/interfaces.
(Debian 的 ifup 和 ifdown 命令开箱并不支持 bonding。应安装 ifenslave-2.6 软件包来提供 bonding 支持。安装后，该软件包会提供用于 /etc/network/interfaces 的 ``bond-*`` 选项。)

Note that ifenslave-2.6 package will load the bonding module and use
the ifenslave command when appropriate.
(注意，ifenslave-2.6 软件包会加载 bonding 模块，并在适当时机使用 ifenslave 命令。)

Example Configurations(配置示例)
--------------------------------

In /etc/network/interfaces, the following stanza will configure bond0, in
active-backup mode, with eth0 and eth1 as slaves::
(在 /etc/network/interfaces 中，以下配置段会以 active-backup 模式配置 bond0，并以 eth0 和 eth1 作为从设备：)

	auto bond0
	iface bond0 inet dhcp
		bond-slaves eth0 eth1
		bond-mode active-backup
		bond-miimon 100
		bond-primary eth0 eth1

If the above configuration doesn't work, you might have a system using
upstart for system startup. This is most notably true for recent
Ubuntu versions. The following stanza in /etc/network/interfaces will
produce the same result on those systems::
(如果上述配置不起作用，你的系统可能使用 upstart 进行系统启动，较新的 Ubuntu 版本尤其如此。在那些系统上，/etc/network/interfaces 中的以下配置段会产生相同的结果：)

	auto bond0
	iface bond0 inet dhcp
		bond-slaves none
		bond-mode active-backup
		bond-miimon 100

	auto eth0
	iface eth0 inet manual
		bond-master bond0
		bond-primary eth0 eth1

	auto eth1
	iface eth1 inet manual
		bond-master bond0
		bond-primary eth0 eth1

For a full list of ``bond-*`` supported options in /etc/network/interfaces and
some more advanced examples tailored to you particular distros, see the files in
/usr/share/doc/ifenslave-2.6.
(关于 /etc/network/interfaces 中支持的 ``bond-*`` 选项的完整列表，以及针对你所用发行版定制的一些更高级的示例，请参阅 /usr/share/doc/ifenslave-2.6 中的文件。)

3.6 Overriding Configuration for Special Cases(3.6 特殊场景下的配置覆盖)
------------------------------------------------------------------------

When using the bonding driver, the physical port which transmits a frame is
typically selected by the bonding driver, and is not relevant to the user or
system administrator.  The output port is simply selected using the policies of
the selected bonding mode.  On occasion however, it is helpful to direct certain
classes of traffic to certain physical interfaces on output to implement
slightly more complex policies.  For example, to reach a web server over a
bonded interface in which eth0 connects to a private network, while eth1
connects via a public network, it may be desirous to bias the bond to send said
traffic over eth0 first, using eth1 only as a fall back, while all other traffic
can safely be sent over either interface.  Such configurations may be achieved
using the traffic control utilities inherent in linux.
(使用 bonding 驱动时，发送帧所用的物理端口通常由 bonding 驱动选择，与用户或系统管理员无关。输出端口只是按所选 bonding 模式的策略来选择。不过，有时为了实现稍复杂一些的策略，希望在发送时把某类流量引导到特定物理接口上。例如，经由绑定接口访问 Web 服务器时，若 eth0 连接私有网络而 eth1 通过公共网络连接，可能希望让 bond 优先经 eth0 发送上述流量，仅把 eth1 作为后备，而其他所有流量可以安全地经由任一接口发送。这类配置可以利用 Linux 内置的流量控制 (traffic control) 工具实现。)

By default the bonding driver is multiqueue aware and 16 queues are created
when the driver initializes (see Documentation/networking/multiqueue.rst
for details).  If more or less queues are desired the module parameter
tx_queues can be used to change this value.  There is no sysfs parameter
available as the allocation is done at module init time.
(默认情况下，bonding 驱动具备多队列感知能力，驱动初始化时会创建 16 个队列（详见 Documentation/networking/multiqueue.rst）。如果需要更多或更少的队列，可以用模块参数 tx_queues 更改该值。由于队列分配在模块初始化时完成，因此没有对应的 sysfs 参数可用。)

The output of the file /proc/net/bonding/bondX has changed so the output Queue
ID is now printed for each slave::
(/proc/net/bonding/bondX 文件的输出已有变化，现在会为每个从设备打印输出队列 ID：)

	Bonding Mode: fault-tolerance (active-backup)
	Primary Slave: None
	Currently Active Slave: eth0
	MII Status: up
	MII Polling Interval (ms): 0
	Up Delay (ms): 0
	Down Delay (ms): 0

	Slave Interface: eth0
	MII Status: up
	Link Failure Count: 0
	Permanent HW addr: 00:1a:a0:12:8f:cb
	Slave queue ID: 0

	Slave Interface: eth1
	MII Status: up
	Link Failure Count: 0
	Permanent HW addr: 00:1a:a0:12:8f:cc
	Slave queue ID: 2

The queue_id for a slave can be set using the command::
(从设备的 queue_id 可以用以下命令设置：)

	# echo "eth1:2" > /sys/class/net/bond0/bonding/queue_id

Any interface that needs a queue_id set should set it with multiple calls
like the one above until proper priorities are set for all interfaces.  On
distributions that allow configuration via initscripts, multiple 'queue_id'
arguments can be added to BONDING_OPTS to set all needed slave queues.
(任何需要设置 queue_id 的接口都应像上面那样多次调用，直到为所有接口设置好合适的优先级。在允许通过 initscripts 配置的发行版上，可以在 BONDING_OPTS 中加入多个 'queue_id' 参数来设置所有需要的从设备队列。)

These queue id's can be used in conjunction with the tc utility to configure
a multiqueue qdisc and filters to bias certain traffic to transmit on certain
slave devices.  For instance, say we wanted, in the above configuration to
force all traffic bound to 192.168.1.100 to use eth1 in the bond as its output
device. The following commands would accomplish this::
(这些队列 id 可以与 tc 工具配合使用，配置多队列 qdisc 和过滤器，使特定流量偏向在某些从设备上发送。例如，假设在上面的配置中，我们想强制所有目的地为 192.168.1.100 的流量使用 bond 中的 eth1 作为输出设备，以下命令可以实现：)

	# tc qdisc add dev bond0 handle 1 root multiq

	# tc filter add dev bond0 protocol ip parent 1: prio 1 u32 match ip \
		dst 192.168.1.100 action skbedit queue_mapping 2

These commands tell the kernel to attach a multiqueue queue discipline to the
bond0 interface and filter traffic enqueued to it, such that packets with a dst
ip of 192.168.1.100 have their output queue mapping value overwritten to 2.
This value is then passed into the driver, causing the normal output path
selection policy to be overridden, selecting instead qid 2, which maps to eth1.
(这些命令告诉内核在 bond0 接口上附加一个多队列排队规则 (qdisc)，并对入队的流量进行过滤，使目的 IP 为 192.168.1.100 的数据包的输出队列映射值被改写为 2。该值随后传入驱动，导致正常的输出路径选择策略被覆盖，改为选择 qid 2，即映射到 eth1。)

Note that qid values begin at 1.  Qid 0 is reserved to initiate to the driver
that normal output policy selection should take place.  One benefit to simply
leaving the qid for a slave to 0 is the multiqueue awareness in the bonding
driver that is now present.  This awareness allows tc filters to be placed on
slave devices as well as bond devices and the bonding driver will simply act as
a pass-through for selecting output queues on the slave device rather than
output port selection.
(注意，qid 值从 1 开始。qid 0 是保留值，用于告知驱动应执行正常的输出策略选择。把某从设备的 qid 简单地保持为 0 的一个好处，是利用 bonding 驱动现在具备的多队列感知能力。这种感知能力允许把 tc 过滤器同时放在从设备和 bond 设备上，此时 bonding 驱动只是作为从设备上输出队列选择的透传层，而不是进行输出端口选择。)

This feature first appeared in bonding driver version 3.7.0 and support for
output slave selection was limited to round-robin and active-backup modes.
(该特性最早出现于 bonding 驱动 3.7.0 版本，且当时对输出从设备选择的支持仅限于 round-robin 和 active-backup 模式。)

3.7 Configuring LACP for 802.3ad mode in a more secure way(3.7 以更安全的方式为 802.3ad 模式配置 LACP)
------------------------------------------------------------------------------------------------------

When using 802.3ad bonding mode, the Actor (host) and Partner (switch)
exchange LACPDUs.  These LACPDUs cannot be sniffed, because they are
destined to link local mac addresses (which switches/bridges are not
supposed to forward).  However, most of the values are easily predictable
or are simply the machine's MAC address (which is trivially known to all
other hosts in the same L2).  This implies that other machines in the L2
domain can spoof LACPDU packets from other hosts to the switch and potentially
cause mayhem by joining (from the point of view of the switch) another
machine's aggregate, thus receiving a portion of that hosts incoming
traffic and / or spoofing traffic from that machine themselves (potentially
even successfully terminating some portion of flows). Though this is not
a likely scenario, one could avoid this possibility by simply configuring
few bonding parameters:
(使用 802.3ad bonding 模式时，Actor（主机）与 Partner（交换机）交换 LACPDUs。这些 LACPDUs 无法被嗅探，因为它们发往链路本地 MAC 地址（交换机/网桥不应转发这类地址）。然而，其中大部分值都容易预测，或者干脆就是本机的 MAC 地址（同一 L2 内的所有其他主机天然都知道它）。这意味着同一 L2 域内的其他机器可以伪造来自其他主机的 LACPDU 数据包发给交换机，并可能（从交换机的视角）加入另一台机器的聚合，从而接收该主机入站流量的一部分，以及/或者冒充该机器发送流量（甚至可能成功终止部分流）。虽然这不太可能发生，但只需配置几个 bonding 参数即可避免这种可能性：)

   (a) ad_actor_system : You can set a random mac-address that can be used for
       these LACPDU exchanges. The value can not be either NULL or Multicast.
       Also it's preferable to set the local-admin bit. Following shell code
       generates a random mac-address as described above::
       ((a) ad_actor_system：可以设置一个用于这些 LACPDU 交换的随机 MAC 地址。该值不能为 NULL 或组播地址。另外最好设置 local-admin 位。下面的 shell 代码按上述要求生成一个随机 MAC 地址：)

	      # sys_mac_addr=$(printf '%02x:%02x:%02x:%02x:%02x:%02x' \
				       $(( (RANDOM & 0xFE) | 0x02 )) \
				       $(( RANDOM & 0xFF )) \
				       $(( RANDOM & 0xFF )) \
				       $(( RANDOM & 0xFF )) \
				       $(( RANDOM & 0xFF )) \
				       $(( RANDOM & 0xFF )))
	      # echo $sys_mac_addr > /sys/class/net/bond0/bonding/ad_actor_system

   (b) ad_actor_sys_prio : Randomize the system priority. The default value
       is 65535, but system can take the value from 1 - 65535. Following shell
       code generates random priority and sets it::
       ((b) ad_actor_sys_prio：将系统优先级随机化。默认值为 65535，但系统可以取 1 - 65535 之间的值。下面的 shell 代码生成随机优先级并进行设置：)

	    # sys_prio=$(( 1 + RANDOM + RANDOM ))
	    # echo $sys_prio > /sys/class/net/bond0/bonding/ad_actor_sys_prio

   (c) ad_user_port_key : Use the user portion of the port-key. The default
       keeps this empty. These are the upper 10 bits of the port-key and value
       ranges from 0 - 1023. Following shell code generates these 10 bits and
       sets it::
       ((c) ad_user_port_key：使用端口键 (port-key) 的用户部分。默认情况下这部分为空。它们是端口键的高 10 位，取值范围 0 - 1023。下面的 shell 代码生成这 10 位并进行设置：)

	    # usr_port_key=$(( RANDOM & 0x3FF ))
	    # echo $usr_port_key > /sys/class/net/bond0/bonding/ad_user_port_key


4 Querying Bonding Configuration(4. 查询 bonding 配置)
======================================================

4.1 Bonding Configuration(4.1 bonding 配置)
-------------------------------------------

Each bonding device has a read-only file residing in the
/proc/net/bonding directory.  The file contents include information
about the bonding configuration, options and state of each slave.
(每个 bonding 设备在 /proc/net/bonding 目录下都有一个只读文件。文件内容包括 bonding 配置、选项以及每个从设备的状态信息。)

For example, the contents of /proc/net/bonding/bond0 after the
driver is loaded with parameters of mode=0 and miimon=1000 is
generally as follows::
(例如，以 mode=0 和 miimon=1000 参数加载驱动后，/proc/net/bonding/bond0 的内容通常如下：)

	Ethernet Channel Bonding Driver: 2.6.1 (October 29, 2004)
	Bonding Mode: load balancing (round-robin)
	Currently Active Slave: eth0
	MII Status: up
	MII Polling Interval (ms): 1000
	Up Delay (ms): 0
	Down Delay (ms): 0

	Slave Interface: eth1
	MII Status: up
	Link Failure Count: 1

	Slave Interface: eth0
	MII Status: up
	Link Failure Count: 1

The precise format and contents will change depending upon the
bonding configuration, state, and version of the bonding driver.
(确切的格式和内容会随 bonding 配置、状态以及 bonding 驱动版本的不同而变化。)

4.2 Network configuration(4.2 网络配置)
---------------------------------------

The network configuration can be inspected using the ifconfig
command.  Bonding devices will have the MASTER flag set; Bonding slave
devices will have the SLAVE flag set.  The ifconfig output does not
contain information on which slaves are associated with which masters.
(可以使用 ifconfig 命令查看网络配置。bonding 设备会置有 MASTER 标志；bonding 从设备会置有 SLAVE 标志。ifconfig 的输出中不包含哪个从设备属于哪个主设备的信息。)

In the example below, the bond0 interface is the master
(MASTER) while eth0 and eth1 are slaves (SLAVE). Notice all slaves of
bond0 have the same MAC address (HWaddr) as bond0 for all modes except
TLB and ALB that require a unique MAC address for each slave::
(在下面的示例中，bond0 接口是主设备 (MASTER)，eth0 和 eth1 是从设备 (SLAVE)。注意，除要求每个从设备拥有唯一 MAC 地址的 TLB 和 ALB 模式外，其余所有模式下 bond0 的所有从设备都与 bond0 具有相同的 MAC 地址 (HWaddr)：)

  # /sbin/ifconfig
  bond0     Link encap:Ethernet  HWaddr 00:C0:F0:1F:37:B4
	    inet addr:XXX.XXX.XXX.YYY  Bcast:XXX.XXX.XXX.255  Mask:255.255.252.0
	    UP BROADCAST RUNNING MASTER MULTICAST  MTU:1500  Metric:1
	    RX packets:7224794 errors:0 dropped:0 overruns:0 frame:0
	    TX packets:3286647 errors:1 dropped:0 overruns:1 carrier:0
	    collisions:0 txqueuelen:0

  eth0      Link encap:Ethernet  HWaddr 00:C0:F0:1F:37:B4
	    UP BROADCAST RUNNING SLAVE MULTICAST  MTU:1500  Metric:1
	    RX packets:3573025 errors:0 dropped:0 overruns:0 frame:0
	    TX packets:1643167 errors:1 dropped:0 overruns:1 carrier:0
	    collisions:0 txqueuelen:100
	    Interrupt:10 Base address:0x1080

  eth1      Link encap:Ethernet  HWaddr 00:C0:F0:1F:37:B4
	    UP BROADCAST RUNNING SLAVE MULTICAST  MTU:1500  Metric:1
	    RX packets:3651769 errors:0 dropped:0 overruns:0 frame:0
	    TX packets:1643480 errors:0 dropped:0 overruns:0 carrier:0
	    collisions:0 txqueuelen:100
	    Interrupt:9 Base address:0x1400

5. Switch Configuration(5. 交换机配置)
======================================

For this section, "switch" refers to whatever system the
bonded devices are directly connected to (i.e., where the other end of
the cable plugs into).  This may be an actual dedicated switch device,
or it may be another regular system (e.g., another computer running
Linux),
(在本节中，"switch"（交换机）指 bond 设备直接连接的任何系统（即网线另一端插入的设备）。它可能是一台真正的专用交换机，也可能是另一台普通系统（例如另一台运行 Linux 的计算机）。)

The active-backup, balance-tlb and balance-alb modes do not
require any specific configuration of the switch.
(active-backup、balance-tlb 和 balance-alb 模式不需要对交换机做任何特殊配置。)

The 802.3ad mode requires that the switch have the appropriate
ports configured as an 802.3ad aggregation.  The precise method used
to configure this varies from switch to switch, but, for example, a
Cisco 3550 series switch requires that the appropriate ports first be
grouped together in a single etherchannel instance, then that
etherchannel is set to mode "lacp" to enable 802.3ad (instead of
standard EtherChannel).
(802.3ad 模式要求交换机将相应端口配置为一个 802.3ad 聚合。具体配置方法因交换机而异，例如 Cisco 3550 系列交换机要求先把相应端口归入同一个 etherchannel 实例，然后将该 etherchannel 设置为 "lacp" 模式以启用 802.3ad（而不是标准 EtherChannel）。)

The balance-rr, balance-xor and broadcast modes generally
require that the switch have the appropriate ports grouped together.
The nomenclature for such a group differs between switches, it may be
called an "etherchannel" (as in the Cisco example, above), a "trunk
group" or some other similar variation.  For these modes, each switch
will also have its own configuration options for the switch's transmit
policy to the bond.  Typical choices include XOR of either the MAC or
IP addresses.  The transmit policy of the two peers does not need to
match.  For these three modes, the bonding mode really selects a
transmit policy for an EtherChannel group; all three will interoperate
with another EtherChannel group.
(balance-rr、balance-xor 和 broadcast 模式通常要求交换机将相应端口归为一组。这种组的名称因交换机而异，可能叫 "etherchannel"（如上面的 Cisco 示例）、"trunk group" 或其他类似的说法。对于这些模式，各交换机还有自己的配置选项来设定交换机发往 bond 的发送策略。典型选择包括对 MAC 地址或 IP 地址做 XOR。两个对端的发送策略不必一致。对这三种模式而言，bonding 模式实际上是在为 EtherChannel 组选择一种发送策略；这三种模式都能与另一个 EtherChannel 组互操作。)


6. 802.1q VLAN Support(6. 802.1q VLAN 支持)
===========================================

It is possible to configure VLAN devices over a bond interface
using the 8021q driver.  However, only packets coming from the 8021q
driver and passing through bonding will be tagged by default.  Self
generated packets, for example, bonding's learning packets or ARP
packets generated by either ALB mode or the ARP monitor mechanism, are
tagged internally by bonding itself.  As a result, bonding must
"learn" the VLAN IDs configured above it, and use those IDs to tag
self generated packets.
(可以使用 8021q 驱动在 bond 接口之上配置 VLAN 设备。不过，默认情况下只有来自 8021q 驱动并经过 bonding 的数据包才会被打上 VLAN 标签。bond 自身生成的数据包——例如 bonding 的学习包，或由 ALB 模式或 ARP 监控机制产生的 ARP 数据包——由 bonding 在内部自行打标签。因此，bonding 必须"学习"配置在其上的 VLAN ID，并用这些 ID 为自身生成的数据包打标签。)

For reasons of simplicity, and to support the use of adapters
that can do VLAN hardware acceleration offloading, the bonding
interface declares itself as fully hardware offloading capable, it gets
the add_vid/kill_vid notifications to gather the necessary
information, and it propagates those actions to the slaves.  In case
of mixed adapter types, hardware accelerated tagged packets that
should go through an adapter that is not offloading capable are
"un-accelerated" by the bonding driver so the VLAN tag sits in the
regular location.
(为了简化设计，并支持具备 VLAN 硬件加速卸载能力的适配器，bonding 接口将自身声明为完全具备硬件卸载能力，通过 add_vid/kill_vid 通知收集必要的信息，并将这些动作传播到从设备。在混合适配器类型的情况下，本应经过不支持卸载的适配器发送的硬件加速打标签数据包，会被 bonding 驱动"去加速"，使 VLAN 标签回到常规位置。)

VLAN interfaces *must* be added on top of a bonding interface
only after enslaving at least one slave.  The bonding interface has a
hardware address of 00:00:00:00:00:00 until the first slave is added.
If the VLAN interface is created prior to the first enslavement, it
would pick up the all-zeroes hardware address.  Once the first slave
is attached to the bond, the bond device itself will pick up the
slave's hardware address, which is then available for the VLAN device.
(VLAN 接口 *必须* 在绑定至少一个从设备之后才能添加到 bonding 接口之上。在添加第一个从设备之前，bonding 接口的硬件地址是 00:00:00:00:00:00。如果在第一次绑定之前就创建 VLAN 接口，它会继承全零硬件地址。一旦第一个从设备连接到 bond，bond 设备本身就会采用该从设备的硬件地址，随后 VLAN 设备即可使用该地址。)

Also, be aware that a similar problem can occur if all slaves
are released from a bond that still has one or more VLAN interfaces on
top of it.  When a new slave is added, the bonding interface will
obtain its hardware address from the first slave, which might not
match the hardware address of the VLAN interfaces (which was
ultimately copied from an earlier slave).
(另外要注意，如果 bond 之上仍有一个或多个 VLAN 接口，而所有从设备都被解除绑定，也会出现类似问题。添加新从设备时，bonding 接口会从第一个从设备获取硬件地址，这可能与 VLAN 接口的硬件地址（它最终复制自更早的某个从设备）不一致。)

There are two methods to ensure that the VLAN device operates
with the correct hardware address if all slaves are removed from a
bond interface:
(如果所有从设备都从 bond 接口上移除，有两种方法可确保 VLAN 设备使用正确的硬件地址运行：)

1. Remove all VLAN interfaces then recreate them
(1. 删除所有 VLAN 接口，然后重新创建)

2. Set the bonding interface's hardware address so that it
matches the hardware address of the VLAN interfaces.
(2. 设置 bonding 接口的硬件地址，使其与 VLAN 接口的硬件地址一致。)

Note that changing a VLAN interface's HW address would set the
underlying device -- i.e. the bonding interface -- to promiscuous
mode, which might not be what you want.
(注意，更改 VLAN 接口的硬件地址会把底层设备（即 bonding 接口）置于混杂模式，这可能不是你想要的效果。)


7. Link Monitoring(7. 链路监控)
===============================

The bonding driver at present supports two schemes for
monitoring a slave device's link state: the ARP monitor and the MII
monitor.
(bonding 驱动目前支持两种监控从设备链路状态的方案：ARP 监控和 MII 监控。)

At the present time, due to implementation restrictions in the
bonding driver itself, it is not possible to enable both ARP and MII
monitoring simultaneously.
(目前，由于 bonding 驱动自身的实现限制，无法同时启用 ARP 和 MII 监控。)

7.1 ARP Monitor Operation(7.1 ARP 监控的运作)
---------------------------------------------

The ARP monitor operates as its name suggests: it sends ARP
queries to one or more designated peer systems on the network, and
uses the response as an indication that the link is operating.  This
gives some assurance that traffic is actually flowing to and from one
or more peers on the local network.
(ARP 监控如其名称所示：它向网络上一个或多个指定的对端系统发送 ARP 查询，并以响应作为链路正常工作的标志。这在一定程度上保证了流量确实在本地网络上的一个或多个对端之间往返。)

7.2 Configuring Multiple ARP Targets(7.2 配置多个 ARP 目标)
-----------------------------------------------------------

While ARP monitoring can be done with just one target, it can
be useful in a High Availability setup to have several targets to
monitor.  In the case of just one target, the target itself may go
down or have a problem making it unresponsive to ARP requests.  Having
an additional target (or several) increases the reliability of the ARP
monitoring.
(虽然只用一个目标也能做 ARP 监控，但在高可用 (High Availability) 环境中配置多个监控目标会更有用。只有一个目标时，该目标自身可能宕机或出现问题，从而无法响应 ARP 请求。多配置一个（或几个）目标可以提高 ARP 监控的可靠性。)

Multiple ARP targets must be separated by commas as follows::
(多个 ARP 目标必须用逗号分隔，如下所示：)

 # example options for ARP monitoring with three targets
 alias bond0 bonding
 options bond0 arp_interval=60 arp_ip_target=192.168.0.1,192.168.0.3,192.168.0.9

For just a single target the options would resemble::
(若只有一个目标，选项则类似如下：)

    # example options for ARP monitoring with one target
    alias bond0 bonding
    options bond0 arp_interval=60 arp_ip_target=192.168.0.100


7.3 MII Monitor Operation(7.3 MII 监控的运作)
---------------------------------------------

The MII monitor monitors only the carrier state of the local
network interface.  It accomplishes this in one of three ways: by
depending upon the device driver to maintain its carrier state, by
querying the device's MII registers, or by making an ethtool query to
the device.
(MII 监控只监控本地网络接口的载波状态。它通过三种方式之一实现：依赖设备驱动维护其载波状态、查询设备的 MII 寄存器，或者向设备发起 ethtool 查询。)

The MII monitor relies on the driver for carrier state information (via
the netif_carrier subsystem).
(MII 监控依靠驱动（通过 netif_carrier 子系统）获取载波状态信息。)

8. Potential Sources of Trouble(8. 潜在问题来源)
================================================

8.1 Adventures in Routing(8.1 路由的玄机)
-----------------------------------------

When bonding is configured, it is important that the slave
devices not have routes that supersede routes of the master (or,
generally, not have routes at all).  For example, suppose the bonding
device bond0 has two slaves, eth0 and eth1, and the routing table is
as follows::
(配置 bonding 时，重要的是从设备不要拥有会取代主设备路由的路由（一般来说，最好完全没有路由）。例如，假设 bonding 设备 bond0 有两个从设备 eth0 和 eth1，路由表如下：)

  Kernel IP routing table
  Destination     Gateway         Genmask         Flags   MSS Window  irtt Iface
  10.0.0.0        0.0.0.0         255.255.0.0     U        40 0          0 eth0
  10.0.0.0        0.0.0.0         255.255.0.0     U        40 0          0 eth1
  10.0.0.0        0.0.0.0         255.255.0.0     U        40 0          0 bond0
  127.0.0.0       0.0.0.0         255.0.0.0       U        40 0          0 lo

This routing configuration will likely still update the
receive/transmit times in the driver (needed by the ARP monitor), but
may bypass the bonding driver (because outgoing traffic to, in this
case, another host on network 10 would use eth0 or eth1 before bond0).
(这种路由配置很可能仍会更新驱动中的收发时间（ARP 监控需要），但可能会绕过 bonding 驱动（因为在本例中，发往网络 10 上其他主机的出站流量会先匹配 eth0 或 eth1，而不是 bond0）。)

The ARP monitor (and ARP itself) may become confused by this
configuration, because ARP requests (generated by the ARP monitor)
will be sent on one interface (bond0), but the corresponding reply
will arrive on a different interface (eth0).  This reply looks to ARP
as an unsolicited ARP reply (because ARP matches replies on an
interface basis), and is discarded.  The MII monitor is not affected
by the state of the routing table.
(ARP 监控（以及 ARP 本身）可能被这种配置搞糊涂：ARP 请求（由 ARP 监控产生）从一个接口 (bond0) 发出，但对应的应答却从另一个接口 (eth0) 到达。这个应答在 ARP 看来是一份未经请求的 ARP 应答（因为 ARP 按接口匹配应答），于是被丢弃。MII 监控不受路由表状态的影响。)

The solution here is simply to ensure that slaves do not have
routes of their own, and if for some reason they must, those routes do
not supersede routes of their master.  This should generally be the
case, but unusual configurations or errant manual or automatic static
route additions may cause trouble.
(解决办法很简单：确保从设备没有自己的路由；如果出于某种原因必须有路由，也要保证这些路由不会取代其主设备的路由。通常情况本该如此，但异常配置或误加的手工/自动静态路由可能引发麻烦。)

8.2 Ethernet Device Renaming(8.2 以太网设备重命名)
--------------------------------------------------

On systems with network configuration scripts that do not
associate physical devices directly with network interface names (so
that the same physical device always has the same "ethX" name), it may
be necessary to add some special logic to config files in
/etc/modprobe.d/.
(在网络配置脚本不把物理设备与网络接口名直接关联的系统中（即同一物理设备不能保证总是同一个 "ethX" 名称），可能需要在 /etc/modprobe.d/ 的配置文件中加入一些特殊逻辑。)

For example, given a modules.conf containing the following::
(例如，假设 modules.conf 包含如下内容：)

	alias bond0 bonding
	options bond0 mode=some-mode miimon=50
	alias eth0 tg3
	alias eth1 tg3
	alias eth2 e1000
	alias eth3 e1000

If neither eth0 and eth1 are slaves to bond0, then when the
bond0 interface comes up, the devices may end up reordered.  This
happens because bonding is loaded first, then its slave device's
drivers are loaded next.  Since no other drivers have been loaded,
when the e1000 driver loads, it will receive eth0 and eth1 for its
devices, but the bonding configuration tries to enslave eth2 and eth3
(which may later be assigned to the tg3 devices).
(如果 eth0 和 eth1 都不是 bond0 的从设备，那么当 bond0 接口启动时，设备最终可能被重新排序。这是因为先加载 bonding，然后才加载其从设备的驱动。由于此前没有加载其他驱动，e1000 驱动加载时会获得 eth0 和 eth1 作为其设备，而 bonding 配置却试图绑定 eth2 和 eth3（它们稍后可能被分配给 tg3 设备）。)

Adding the following::
(加上以下内容：)

	add above bonding e1000 tg3

causes modprobe to load e1000 then tg3, in that order, when
bonding is loaded.  This command is fully documented in the
modules.conf manual page.
(会使 modprobe 在加载 bonding 时按先 e1000 后 tg3 的顺序加载。该命令在 modules.conf 手册页中有完整说明。)

On systems utilizing modprobe an equivalent problem can occur.
In this case, the following can be added to config files in
/etc/modprobe.d/ as::
(在使用 modprobe 的系统上也会出现等价的问题。此时可以在 /etc/modprobe.d/ 的配置文件中加入如下内容：)

	softdep bonding pre: tg3 e1000

This will load tg3 and e1000 modules before loading the bonding one.
Full documentation on this can be found in the modprobe.d and modprobe
manual pages.
(这会先加载 tg3 和 e1000 模块，再加载 bonding 模块。完整说明可参见 modprobe.d 和 modprobe 手册页。)

9. SNMP agents(9. SNMP 代理)
============================

If running SNMP agents, the bonding driver should be loaded
before any network drivers participating in a bond.  This requirement
is due to the interface index (ipAdEntIfIndex) being associated to
the first interface found with a given IP address.  That is, there is
only one ipAdEntIfIndex for each IP address.  For example, if eth0 and
eth1 are slaves of bond0 and the driver for eth0 is loaded before the
bonding driver, the interface for the IP address will be associated
with the eth0 interface.  This configuration is shown below, the IP
address 192.168.1.1 has an interface index of 2 which indexes to eth0
in the ifDescr table (ifDescr.2).
(如果运行 SNMP 代理，bonding 驱动应在参与 bond 的任何网络驱动之前加载。这一要求是因为接口索引 (ipAdEntIfIndex) 会关联到按给定 IP 地址找到的第一个接口。也就是说，每个 IP 地址只有一个 ipAdEntIfIndex。例如，如果 eth0 和 eth1 都是 bond0 的从设备，而 eth0 的驱动先于 bonding 驱动加载，那么该 IP 地址对应的接口就会关联到 eth0 接口。这种配置如下所示，IP 地址 192.168.1.1 的接口索引为 2，对应 ifDescr 表中的 eth0 (ifDescr.2)。)

::
(上述配置对应的输出如下：)

     interfaces.ifTable.ifEntry.ifDescr.1 = lo
     interfaces.ifTable.ifEntry.ifDescr.2 = eth0
     interfaces.ifTable.ifEntry.ifDescr.3 = eth1
     interfaces.ifTable.ifEntry.ifDescr.4 = eth2
     interfaces.ifTable.ifEntry.ifDescr.5 = eth3
     interfaces.ifTable.ifEntry.ifDescr.6 = bond0
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.10.10.10.10 = 5
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.192.168.1.1 = 2
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.10.74.20.94 = 4
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.127.0.0.1 = 1

This problem is avoided by loading the bonding driver before
any network drivers participating in a bond.  Below is an example of
loading the bonding driver first, the IP address 192.168.1.1 is
correctly associated with ifDescr.2.
(在参与 bond 的任何网络驱动之前先加载 bonding 驱动即可避免这个问题。下面是先加载 bonding 驱动的示例，IP 地址 192.168.1.1 正确关联到 ifDescr.2。)

     interfaces.ifTable.ifEntry.ifDescr.1 = lo
     interfaces.ifTable.ifEntry.ifDescr.2 = bond0
     interfaces.ifTable.ifEntry.ifDescr.3 = eth0
     interfaces.ifTable.ifEntry.ifDescr.4 = eth1
     interfaces.ifTable.ifEntry.ifDescr.5 = eth2
     interfaces.ifTable.ifEntry.ifDescr.6 = eth3
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.10.10.10.10 = 6
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.192.168.1.1 = 2
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.10.74.20.94 = 5
     ip.ipAddrTable.ipAddrEntry.ipAdEntIfIndex.127.0.0.1 = 1

While some distributions may not report the interface name in
ifDescr, the association between the IP address and IfIndex remains
and SNMP functions such as Interface_Scan_Next will report that
association.
(虽然某些发行版可能不在 ifDescr 中报告接口名称，但 IP 地址与 IfIndex 之间的关联依然存在，Interface_Scan_Next 等 SNMP 函数会报告这一关联。)

10. Promiscuous mode(10. 混杂模式)
==================================

When running network monitoring tools, e.g., tcpdump, it is
common to enable promiscuous mode on the device, so that all traffic
is seen (instead of seeing only traffic destined for the local host).
The bonding driver handles promiscuous mode changes to the bonding
master device (e.g., bond0), and propagates the setting to the slave
devices.
(运行网络监控工具（如 tcpdump）时，通常会在设备上启用混杂模式，以便看到所有流量（而不是只看到目的地为本地的流量）。bonding 驱动会处理 bonding 主设备（如 bond0）上的混杂模式变更，并将该设置传播到从设备。)

For the balance-rr, balance-xor, broadcast, and 802.3ad modes,
the promiscuous mode setting is propagated to all slaves.
(对于 balance-rr、balance-xor、broadcast 和 802.3ad 模式，混杂模式设置会传播到所有从设备。)

For the active-backup, balance-tlb and balance-alb modes, the
promiscuous mode setting is propagated only to the active slave.
(对于 active-backup、balance-tlb 和 balance-alb 模式，混杂模式设置只传播到活动从设备。)

For balance-tlb mode, the active slave is the slave currently
receiving inbound traffic.
(对于 balance-tlb 模式，活动从设备是当前接收入站流量的从设备。)

For balance-alb mode, the active slave is the slave used as a
"primary."  This slave is used for mode-specific control traffic, for
sending to peers that are unassigned or if the load is unbalanced.
(对于 balance-alb 模式，活动从设备是被用作 "primary" 的从设备。该从设备用于模式专属的控制流量，以及向未分配的对端发送流量或在负载不均衡时发送流量。)

For the active-backup, balance-tlb and balance-alb modes, when
the active slave changes (e.g., due to a link failure), the
promiscuous setting will be propagated to the new active slave.
(对于 active-backup、balance-tlb 和 balance-alb 模式，当活动从设备发生变化时（例如因链路故障），混杂模式设置会传播到新的活动从设备。)

11. Configuring Bonding for High Availability(11. 为高可用性配置 bonding)
=========================================================================

High Availability refers to configurations that provide
maximum network availability by having redundant or backup devices,
links or switches between the host and the rest of the world.  The
goal is to provide the maximum availability of network connectivity
(i.e., the network always works), even though other configurations
could provide higher throughput.
(高可用性 (High Availability) 指通过在主机与外部世界之间部署冗余或备份设备、链路或交换机来提供最大化网络可用性的配置。其目标是提供最大的网络连通性可用性（即网络始终可用），尽管其他配置可能提供更高的吞吐量。)

11.1 High Availability in a Single Switch Topology(11.1 单交换机拓扑中的高可用性)
---------------------------------------------------------------------------------

If two hosts (or a host and a single switch) are directly
connected via multiple physical links, then there is no availability
penalty to optimizing for maximum bandwidth.  In this case, there is
only one switch (or peer), so if it fails, there is no alternative
access to fail over to.  Additionally, the bonding load balance modes
support link monitoring of their members, so if individual links fail,
the load will be rebalanced across the remaining devices.
(如果两台主机（或一台主机与一台交换机）通过多条物理链路直接相连，那么针对最大带宽进行优化不会损失可用性。这种情况下只有一个交换机（或对端），它一旦失效，就没有可供切换的其他访问途径。此外，bonding 的负载均衡模式支持对其成员链路的监控，因此如果个别链路失效，负载会在其余设备之间重新均衡。)

See Section 12, "Configuring Bonding for Maximum Throughput"
for information on configuring bonding with one peer device.
(关于与单个对端设备配置 bonding 的信息，请参阅第 12 节 "Configuring Bonding for Maximum Throughput"。)

11.2 High Availability in a Multiple Switch Topology(11.2 多交换机拓扑中的高可用性)
-----------------------------------------------------------------------------------

With multiple switches, the configuration of bonding and the
network changes dramatically.  In multiple switch topologies, there is
a trade off between network availability and usable bandwidth.
(有了多台交换机，bonding 与网络的配置会发生显著变化。在多交换机拓扑中，需要在网络可用性与可用带宽之间进行权衡。)

Below is a sample network, configured to maximize the
availability of the network::
(下面是一个为最大化网络可用性而配置的示例网络：)

		|                                     |
		|port3                           port3|
	  +-----+----+                          +-----+----+
	  |          |port2       ISL      port2|          |
	  | switch A +--------------------------+ switch B |
	  |          |                          |          |
	  +-----+----+                          +-----++---+
		|port1                           port1|
		|             +-------+               |
		+-------------+ host1 +---------------+
			 eth0 +-------+ eth1

In this configuration, there is a link between the two
switches (ISL, or inter switch link), and multiple ports connecting to
the outside world ("port3" on each switch).  There is no technical
reason that this could not be extended to a third switch.
(在这种配置中，两台交换机之间有一条链路（ISL，即交换机间链路），并且有多条连接外部世界的端口（每台交换机上的 "port3"）。没有技术上的理由阻止将其扩展到第三台交换机。)

11.2.1 HA Bonding Mode Selection for Multiple Switch Topology(11.2.1 多交换机拓扑下 HA 的 bonding 模式选择)
-----------------------------------------------------------------------------------------------------------

In a topology such as the example above, the active-backup and
broadcast modes are the only useful bonding modes when optimizing for
availability; the other modes require all links to terminate on the
same peer for them to behave rationally.
(在如上例所示的拓扑中，以可用性为优化目标时，active-backup 和 broadcast 是仅有的两种有用的 bonding 模式；其他模式要求所有链路都终止于同一对端才能正常工作。)

active-backup:
	This is generally the preferred mode, particularly if
	the switches have an ISL and play together well.  If the
	network configuration is such that one switch is specifically
	a backup switch (e.g., has lower capacity, higher cost, etc),
	then the primary option can be used to ensure that the
	preferred link is always used when it is available.
	(这通常是首选模式，尤其当交换机之间有 ISL 且配合良好时。如果网络配置中一台交换机专门充当备份交换机（例如容量较低、成本较高等），那么可以使用 primary 选项确保在首选链路可用时总是使用它。)

broadcast:
	This mode is really a special purpose mode, and is suitable
	only for very specific needs.  For example, if the two
	switches are not connected (no ISL), and the networks beyond
	them are totally independent.  In this case, if it is
	necessary for some specific one-way traffic to reach both
	independent networks, then the broadcast mode may be suitable.
	(该模式实际上是特殊用途模式，仅适合非常特殊的需求。例如，两台交换机之间没有连接（无 ISL），它们之外的网络彼此完全独立。在这种情况下，如果需要让某些特定的单向流量同时到达两个独立网络，那么 broadcast 模式可能合适。)

11.2.2 HA Link Monitoring Selection for Multiple Switch Topology(11.2.2 多交换机拓扑下 HA 的链路监控选择)
---------------------------------------------------------------------------------------------------------

The choice of link monitoring ultimately depends upon your
switch.  If the switch can reliably fail ports in response to other
failures, then either the MII or ARP monitors should work.  For
example, in the above example, if the "port3" link fails at the remote
end, the MII monitor has no direct means to detect this.  The ARP
monitor could be configured with a target at the remote end of port3,
thus detecting that failure without switch support.
(链路监控方式的选择最终取决于你的交换机。如果交换机能可靠地随其他端口故障而将端口置为 down，那么 MII 或 ARP 监控都应可行。例如在上面的示例中，如果 "port3" 链路在远端发生故障，MII 监控没有直接手段检测到；而 ARP 监控可以把目标配置在 port3 的远端，从而在无交换机支持的情况下检测到该故障。)

In general, however, in a multiple switch topology, the ARP
monitor can provide a higher level of reliability in detecting end to
end connectivity failures (which may be caused by the failure of any
individual component to pass traffic for any reason).  Additionally,
the ARP monitor should be configured with multiple targets (at least
one for each switch in the network).  This will ensure that,
regardless of which switch is active, the ARP monitor has a suitable
target to query.
(不过总的来说，在多交换机拓扑中，ARP 监控在检测端到端连通性故障方面能提供更高的可靠性（这类故障可能由任何组件因任何原因无法转发流量引起）。此外，ARP 监控应配置多个目标（网络中每台交换机至少一个）。这可以确保无论哪台交换机处于活动状态，ARP 监控都有合适的目标可查询。)

Note, also, that of late many switches now support a functionality
generally referred to as "trunk failover."  This is a feature of the
switch that causes the link state of a particular switch port to be set
down (or up) when the state of another switch port goes down (or up).
Its purpose is to propagate link failures from logically "exterior" ports
to the logically "interior" ports that bonding is able to monitor via
miimon.  Availability and configuration for trunk failover varies by
switch, but this can be a viable alternative to the ARP monitor when using
suitable switches.
(另外要注意，近来许多交换机已支持一种通常称为 "trunk failover"（干道故障转移）的功能。这是交换机的一项特性：当另一个交换机端口的状态变为 down（或 up）时，特定交换机端口的链路状态也会被置为 down（或 up）。其目的是将链路故障从逻辑上的"外部"端口传播到 bonding 能通过 miimon 监控的逻辑"内部"端口。trunk failover 的可用性与配置方法因交换机而异，但在使用合适的交换机时，它可以作为 ARP 监控的可行替代方案。)

12. Configuring Bonding for Maximum Throughput(12. 为最大吞吐量配置 bonding)
============================================================================

12.1 Maximizing Throughput in a Single Switch Topology(12.1 单交换机拓扑中的吞吐量最大化)
-----------------------------------------------------------------------------------------

In a single switch configuration, the best method to maximize
throughput depends upon the application and network environment.  The
various load balancing modes each have strengths and weaknesses in
different environments, as detailed below.
(在单交换机配置中，最大化吞吐量的最佳方法取决于应用和网络环境。各种负载均衡模式在不同环境中各有优劣，详见下文。)

For this discussion, we will break down the topologies into
two categories.  Depending upon the destination of most traffic, we
categorize them into either "gatewayed" or "local" configurations.
(在本节的讨论中，我们把拓扑分为两类。根据大多数流量的目的地，将其归入 "gatewayed"（经网关）或 "local"（本地）配置。)

In a gatewayed configuration, the "switch" is acting primarily
as a router, and the majority of traffic passes through this router to
other networks.  An example would be the following::
(在 gatewayed 配置中，"switch"（交换机）主要充当路由器，大部分流量经由该路由器去往其他网络。示例如下：)


     +----------+                     +----------+
     |          |eth0            port1|          | to other networks
     | Host A   +---------------------+ router   +------------------->
     |          +---------------------+          | Hosts B and C are out
     |          |eth1            port2|          | here somewhere
     +----------+                     +----------+

The router may be a dedicated router device, or another host
acting as a gateway.  For our discussion, the important point is that
the majority of traffic from Host A will pass through the router to
some other network before reaching its final destination.
(路由器可以是专用路由器设备，也可以是充当网关的另一台主机。对我们的讨论而言，重点在于来自主机 A 的大部分流量都会先经由路由器到达某个其他网络，然后才到达最终目的地。)

In a gatewayed network configuration, although Host A may
communicate with many other systems, all of its traffic will be sent
and received via one other peer on the local network, the router.
(在 gatewayed 网络配置中，尽管主机 A 可能与许多其他系统通信，其所有流量都会经由本地网络上的另一个对端——路由器——收发。)

Note that the case of two systems connected directly via
multiple physical links is, for purposes of configuring bonding, the
same as a gatewayed configuration.  In that case, it happens that all
traffic is destined for the "gateway" itself, not some other network
beyond the gateway.
(注意，就配置 bonding 而言，两台系统通过多条物理链路直连的情形与 gatewayed 配置相同。这种情况下，只是所有流量恰好都发往"网关"本身，而不是网关之外的某个网络。)

In a local configuration, the "switch" is acting primarily as
a switch, and the majority of traffic passes through this switch to
reach other stations on the same network.  An example would be the
following::
(在 local 配置中，"switch" 主要充当交换机，大部分流量经由该交换机到达同一网络上的其他站点。示例如下：)

    +----------+            +----------+       +--------+
    |          |eth0   port1|          +-------+ Host B |
    |  Host A  +------------+  switch  |port3  +--------+
    |          +------------+          |                  +--------+
    |          |eth1   port2|          +------------------+ Host C |
    +----------+            +----------+port4             +--------+


Again, the switch may be a dedicated switch device, or another
host acting as a gateway.  For our discussion, the important point is
that the majority of traffic from Host A is destined for other hosts
on the same local network (Hosts B and C in the above example).
(同样，交换机可以是专用交换设备，也可以是充当网关的另一台主机。对我们的讨论而言，重点在于来自主机 A 的大部分流量都去往同一本地网络上的其他主机（上例中的主机 B 和主机 C）。)

In summary, in a gatewayed configuration, traffic to and from
the bonded device will be to the same MAC level peer on the network
(the gateway itself, i.e., the router), regardless of its final
destination.  In a local configuration, traffic flows directly to and
from the final destinations, thus, each destination (Host B, Host C)
will be addressed directly by their individual MAC addresses.
(总而言之，在 gatewayed 配置中，进出绑定设备的流量在网络上始终面向同一个 MAC 层对端（网关本身，即路由器），而与其最终目的地无关。在 local 配置中，流量直接流向最终目的地并从中返回，因此每个目的地（主机 B、主机 C）都会由其各自的 MAC 地址直接寻址。)

This distinction between a gatewayed and a local network
configuration is important because many of the load balancing modes
available use the MAC addresses of the local network source and
destination to make load balancing decisions.  The behavior of each
mode is described below.
(gatewayed 与 local 网络配置之间的这一区别很重要，因为许多可用的负载均衡模式会利用本地网络源与目的地的 MAC 地址来做负载均衡决策。每种模式的行为将在下文描述。)


12.1.1 MT Bonding Mode Selection for Single Switch Topology(12.1.1 单交换机拓扑下的 MT bonding 模式选择)
--------------------------------------------------------------------------------------------------------

This configuration is the easiest to set up and to understand,
although you will have to decide which bonding mode best suits your
needs.  The trade offs for each mode are detailed below:
(这种配置最容易搭建和理解，不过你需要决定哪种 bonding 模式最符合你的需求。每种模式的权衡详见下文：)

balance-rr:
	This mode is the only mode that will permit a single
	TCP/IP connection to stripe traffic across multiple
	interfaces. It is therefore the only mode that will allow a
	single TCP/IP stream to utilize more than one interface's
	worth of throughput.  This comes at a cost, however: the
	striping generally results in peer systems receiving packets out
	of order, causing TCP/IP's congestion control system to kick
	in, often by retransmitting segments.
	(该模式是唯一允许单条 TCP/IP 连接将流量条带化分布到多个接口上的模式。因此它也是唯一能让单条 TCP/IP 流利用超过一个接口吞吐量的模式。但这有代价：条带化通常导致对端系统乱序收到数据包，触发 TCP/IP 的拥塞控制系统介入，往往以重传报文段的方式进行。)

	It is possible to adjust TCP/IP's congestion limits by
	altering the net.ipv4.tcp_reordering sysctl parameter.  The
	usual default value is 3. But keep in mind TCP stack is able
	to automatically increase this when it detects reorders.
	(可以通过修改 net.ipv4.tcp_reordering sysctl 参数来调整 TCP/IP 的乱序容忍上限。通常默认值为 3。但请记住，TCP 协议栈检测到乱序时会自动增大该值。)

	Note that the fraction of packets that will be delivered out of
	order is highly variable, and is unlikely to be zero.  The level
	of reordering depends upon a variety of factors, including the
	networking interfaces, the switch, and the topology of the
	configuration.  Speaking in general terms, higher speed network
	cards produce more reordering (due to factors such as packet
	coalescing), and a "many to many" topology will reorder at a
	higher rate than a "many slow to one fast" configuration.
	(注意，乱序送达的数据包比例变化很大，且不太可能为零。乱序程度取决于多种因素，包括网络接口、交换机以及配置的拓扑。大体而言，速率更高的网卡会产生更多乱序（原因包括数据包合并等），而"多对多"拓扑的乱序率会高于"多慢对一快"的配置。)

	Many switches do not support any modes that stripe traffic
	(instead choosing a port based upon IP or MAC level addresses);
	for those devices, traffic for a particular connection flowing
	through the switch to a balance-rr bond will not utilize greater
	than one interface's worth of bandwidth.
	(许多交换机不支持任何条带化流量的模式（而是根据 IP 或 MAC 层地址选择端口）；对于这类设备，经交换机流向 balance-rr bond 的某条连接的流量，所能利用的带宽不会超过一个接口的量。)

	If you are utilizing protocols other than TCP/IP, UDP for
	example, and your application can tolerate out of order
	delivery, then this mode can allow for single stream datagram
	performance that scales near linearly as interfaces are added
	to the bond.
	(如果你使用的是 TCP/IP 以外的协议（例如 UDP），且你的应用能够容忍乱序送达，那么随着接口加入 bond，该模式可以提供接近线性扩展的单流数据报性能。)

	This mode requires the switch to have the appropriate ports
	configured for "etherchannel" or "trunking."
	(该模式要求交换机将相应端口配置为 "etherchannel" 或 "trunking"。)

active-backup:
	There is not much advantage in this network topology to
	the active-backup mode, as the inactive backup devices are all
	connected to the same peer as the primary.  In this case, a
	load balancing mode (with link monitoring) will provide the
	same level of network availability, but with increased
	available bandwidth.  On the plus side, active-backup mode
	does not require any configuration of the switch, so it may
	have value if the hardware available does not support any of
	the load balance modes.
	(在这种网络拓扑中，active-backup 模式没有太大优势，因为非活动的备份设备都和主设备连接到同一个对端。这种情况下，负载均衡模式（带链路监控）能提供同等的网络可用性，同时增加可用带宽。好处是 active-backup 模式不需要对交换机做任何配置，因此如果现有硬件不支持任何负载均衡模式，它仍有价值。)

balance-xor:
	This mode will limit traffic such that packets destined
	for specific peers will always be sent over the same
	interface.  Since the destination is determined by the MAC
	addresses involved, this mode works best in a "local" network
	configuration (as described above), with destinations all on
	the same local network.  This mode is likely to be suboptimal
	if all your traffic is passed through a single router (i.e., a
	"gatewayed" network configuration, as described above).
	(该模式会限制流量，使发往特定对端的数据包总是经由同一接口发送。由于目的地由所涉及的 MAC 地址决定，该模式最适合 "local" 网络配置（如上所述），即所有目的地都在同一本地网络上。如果你的所有流量都经由单一路由器转发（即上文所述的 "gatewayed" 网络配置），该模式的表现可能欠佳。)

	As with balance-rr, the switch ports need to be configured for
	"etherchannel" or "trunking."
	(与 balance-rr 一样，交换机端口也需要配置为 "etherchannel" 或 "trunking"。)

broadcast:
	Like active-backup, there is not much advantage to this
	mode in this type of network topology.
	(与 active-backup 一样，在这种类型的网络拓扑中该模式没有太大优势。)

802.3ad:
	This mode can be a good choice for this type of network
	topology.  The 802.3ad mode is an IEEE standard, so all peers
	that implement 802.3ad should interoperate well.  The 802.3ad
	protocol includes automatic configuration of the aggregates,
	so minimal manual configuration of the switch is needed
	(typically only to designate that some set of devices is
	available for 802.3ad).  The 802.3ad standard also mandates
	that frames be delivered in order (within certain limits), so
	in general single connections will not see misordering of
	packets.  The 802.3ad mode does have some drawbacks: the
	standard mandates that all devices in the aggregate operate at
	the same speed and duplex.  Also, as with all bonding load
	balance modes other than balance-rr, no single connection will
	be able to utilize more than a single interface's worth of
	bandwidth.
	(对于这种类型的网络拓扑，该模式可以是不错的选择。802.3ad 模式是 IEEE 标准，因此所有实现 802.3ad 的对端都应能良好互操作。802.3ad 协议包含聚合的自动配置，因此交换机只需最少的手工配置（通常只需指定某些设备可用于 802.3ad）。802.3ad 标准还强制要求帧按序送达（在一定限度内），因此一般单条连接不会看到数据包乱序。802.3ad 模式也有一些缺点：标准强制要求聚合中的所有设备以相同的速率和双工模式运行。另外，与除 balance-rr 之外的所有 bonding 负载均衡模式一样，单条连接无法利用超过单个接口的带宽。)

	Additionally, the linux bonding 802.3ad implementation
	distributes traffic by peer (using an XOR of MAC addresses
	and packet type ID), so in a "gatewayed" configuration, all
	outgoing traffic will generally use the same device.  Incoming
	traffic may also end up on a single device, but that is
	dependent upon the balancing policy of the peer's 802.3ad
	implementation.  In a "local" configuration, traffic will be
	distributed across the devices in the bond.
	(此外，Linux bonding 的 802.3ad 实现按对端分发流量（使用 MAC 地址与数据包类型 ID 的 XOR），因此在 "gatewayed" 配置中，所有出站流量通常都会使用同一设备。入站流量也可能落在单个设备上，但这取决于对端 802.3ad 实现的均衡策略。在 "local" 配置中，流量会分布在 bond 中的各个设备上。)

	Finally, the 802.3ad mode mandates the use of the MII monitor,
	therefore, the ARP monitor is not available in this mode.
	(最后，802.3ad 模式强制使用 MII 监控，因此该模式下不能使用 ARP 监控。)

balance-tlb:
	The balance-tlb mode balances outgoing traffic by peer.
	Since the balancing is done according to MAC address, in a
	"gatewayed" configuration (as described above), this mode will
	send all traffic across a single device.  However, in a
	"local" network configuration, this mode balances multiple
	local network peers across devices in a vaguely intelligent
	manner (not a simple XOR as in balance-xor or 802.3ad mode),
	so that mathematically unlucky MAC addresses (i.e., ones that
	XOR to the same value) will not all "bunch up" on a single
	interface.
	(balance-tlb 模式按对端均衡出站流量。由于均衡依据 MAC 地址进行，在 "gatewayed" 配置（如上所述）中，该模式会把所有流量都经由单个设备发送。而在 "local" 网络配置中，该模式会以某种粗略智能的方式（不同于 balance-xor 或 802.3ad 模式中的简单 XOR）把多个本地网络对端均衡到各设备上，使数学上"不走运"的 MAC 地址（即 XOR 结果相同的地址）不会全都"挤"在同一个接口上。)

	Unlike 802.3ad, interfaces may be of differing speeds, and no
	special switch configuration is required.  On the down side,
	in this mode all incoming traffic arrives over a single
	interface, this mode requires certain ethtool support in the
	network device driver of the slave interfaces, and the ARP
	monitor is not available.
	(与 802.3ad 不同，各接口速率可以不同，也不需要特殊的交换机配置。不利之处在于：该模式下所有入站流量都从单一接口到达，要求从设备的网络设备驱动提供某些 ethtool 支持，并且不能使用 ARP 监控。)

balance-alb:
	This mode is everything that balance-tlb is, and more.
	It has all of the features (and restrictions) of balance-tlb,
	and will also balance incoming traffic from local network
	peers (as described in the Bonding Module Options section,
	above).
	(该模式涵盖 balance-tlb 的一切并更进一步。它具备 balance-tlb 的全部特性（和限制），还能均衡来自本地网络对端的入站流量（见上文 "Bonding Module Options" 一节的描述）。)

	The only additional down side to this mode is that the network
	device driver must support changing the hardware address while
	the device is open.
	(该模式唯一的额外不利之处是，网络设备驱动必须支持在设备处于打开状态时更改硬件地址。)

12.1.2 MT Link Monitoring for Single Switch Topology(12.1.2 单交换机拓扑下的 MT 链路监控)
-----------------------------------------------------------------------------------------

The choice of link monitoring may largely depend upon which
mode you choose to use.  The more advanced load balancing modes do not
support the use of the ARP monitor, and are thus restricted to using
the MII monitor (which does not provide as high a level of end to end
assurance as the ARP monitor).
(链路监控的选择在很大程度上取决于你选用哪种模式。较高级的负载均衡模式不支持 ARP 监控，因此只能使用 MII 监控（它提供的端到端保障水平不如 ARP 监控）。)

12.2 Maximum Throughput in a Multiple Switch Topology(12.2 多交换机拓扑中的最大吞吐量)
--------------------------------------------------------------------------------------

Multiple switches may be utilized to optimize for throughput
when they are configured in parallel as part of an isolated network
between two or more systems, for example::

		       +-----------+
		       |  Host A   |
		       +-+---+---+-+
			 |   |   |
		+--------+   |   +---------+
		|            |             |
	 +------+---+  +-----+----+  +-----+----+
	 | Switch A |  | Switch B |  | Switch C |
	 +------+---+  +-----+----+  +-----+----+
		|            |             |
		+--------+   |   +---------+
			 |   |   |
		       +-+---+---+-+
		       |  Host B   |
		       +-----------+

(当多台交换机并行配置、构成两台或更多系统之间的隔离网络时，可以用它们来优化吞吐量，例如：)

In this configuration, the switches are isolated from one
another.  One reason to employ a topology such as this is for an
isolated network with many hosts (a cluster configured for high
performance, for example), using multiple smaller switches can be more
cost effective than a single larger switch, e.g., on a network with 24
hosts, three 24 port switches can be significantly less expensive than
a single 72 port switch.
(在这种配置中，各交换机彼此隔离。采用这种拓扑的一个原因是：对于拥有大量主机的隔离网络（例如为高性能配置的集群），使用多台较小的交换机可能比一台大交换机更划算。例如，在一个有 24 台主机的网络上，三台 24 口交换机的成本可能明显低于一台 72 口交换机。)

If access beyond the network is required, an individual host
can be equipped with an additional network device connected to an
external network; this host then additionally acts as a gateway.
(如果需要访问该网络之外，可以给某台主机加装一个连接外部网络的额外网络设备；这台主机便同时充当网关。)

12.2.1 MT Bonding Mode Selection for Multiple Switch Topology(12.2.1 多交换机拓扑下的 MT bonding 模式选择)
----------------------------------------------------------------------------------------------------------

In actual practice, the bonding mode typically employed in
configurations of this type is balance-rr.  Historically, in this
network configuration, the usual caveats about out of order packet
delivery are mitigated by the use of network adapters that do not do
any kind of packet coalescing (via the use of NAPI, or because the
device itself does not generate interrupts until some number of
packets has arrived).  When employed in this fashion, the balance-rr
mode allows individual connections between two hosts to effectively
utilize greater than one interface's bandwidth.
(在实际实践中，这类配置通常采用 balance-rr。历史上，在这种网络配置中，关于数据包乱序送达的常见顾虑，可以通过使用不做任何数据包合并的网络适配器来缓解（借助 NAPI，或因为设备本身在累积一定数量的数据包之前不产生中断）。以这种方式使用时，balance-rr 模式能让两台主机之间的单条连接有效利用超过一个接口的带宽。)

12.2.2 MT Link Monitoring for Multiple Switch Topology(12.2.2 多交换机拓扑下的 MT 链路监控)
-------------------------------------------------------------------------------------------

Again, in actual practice, the MII monitor is most often used
in this configuration, as performance is given preference over
availability.  The ARP monitor will function in this topology, but its
advantages over the MII monitor are mitigated by the volume of probes
needed as the number of systems involved grows (remember that each
host in the network is configured with bonding).
(同样，在实际实践中，这种配置最常使用 MII 监控，因为性能优先于可用性。ARP 监控在这种拓扑中也能工作，但随着涉及系统的数量增长，所需探测的流量规模会削弱它相对 MII 监控的优势（记住网络中每台主机都配置了 bonding）。)

13. Switch Behavior Issues(13. 交换机行为问题)
==============================================

13.1 Link Establishment and Failover Delays(13.1 链路建立与故障转移延迟)
------------------------------------------------------------------------

Some switches exhibit undesirable behavior with regard to the
timing of link up and down reporting by the switch.
(一些交换机在链路 up/down 状态上报的时机方面表现不佳。)

First, when a link comes up, some switches may indicate that
the link is up (carrier available), but not pass traffic over the
interface for some period of time.  This delay is typically due to
some type of autonegotiation or routing protocol, but may also occur
during switch initialization (e.g., during recovery after a switch
failure).  If you find this to be a problem, specify an appropriate
value to the updelay bonding module option to delay the use of the
relevant interface(s).
(首先，当链路刚建立时，某些交换机可能报告链路已 up（载波可用），但在一段时间内不通过该接口转发流量。这种延迟通常由某种自动协商或路由协议引起，也可能发生在交换机初始化期间（例如交换机故障后的恢复过程中）。如果你发现这是个问题，可为 updelay bonding 模块选项指定一个合适的值，以延迟启用相关接口。)

Second, some switches may "bounce" the link state one or more
times while a link is changing state.  This occurs most commonly while
the switch is initializing.  Again, an appropriate updelay value may
help.
(其次，某些交换机在链路状态变化过程中可能使链路状态"抖动"一次或多次。这种情况最常发生在交换机初始化时。同样，设置合适的 updelay 值可能有所帮助。)

Note that when a bonding interface has no active links, the
driver will immediately reuse the first link that goes up, even if the
updelay parameter has been specified (the updelay is ignored in this
case).  If there are slave interfaces waiting for the updelay timeout
to expire, the interface that first went into that state will be
immediately reused.  This reduces down time of the network if the
value of updelay has been overestimated, and since this occurs only in
cases with no connectivity, there is no additional penalty for
ignoring the updelay.
(注意，当 bonding 接口没有任何活动链路时，驱动会立即复用第一条变为 up 的链路，即使已指定 updelay 参数（此时 updelay 被忽略）。如果有从设备接口正在等待 updelay 超时到期，则最先进入该状态的接口会被立即复用。如果 updelay 的值被设得过大，这能减少网络的停机时间；而且由于这种情况只发生在完全无连通性的时候，忽略 updelay 不会带来额外代价。)

In addition to the concerns about switch timings, if your
switches take a long time to go into backup mode, it may be desirable
to not activate a backup interface immediately after a link goes down.
Failover may be delayed via the downdelay bonding module option.
(除了交换机时序方面的顾虑，如果你的交换机进入备份模式需要较长时间，可能希望不要在链路断开后立即激活备份接口。可以通过 downdelay bonding 模块选项延迟故障转移。)

13.2 Duplicated Incoming Packets(13.2 重复的入站数据包)
-------------------------------------------------------

NOTE: Starting with version 3.0.2, the bonding driver has logic to
suppress duplicate packets, which should largely eliminate this problem.
The following description is kept for reference.
(注意：从 3.0.2 版本开始，bonding 驱动加入了抑制重复数据包的逻辑，应能在很大程度上消除此问题。以下描述仅作参考保留。)

It is not uncommon to observe a short burst of duplicated
traffic when the bonding device is first used, or after it has been
idle for some period of time.  This is most easily observed by issuing
a "ping" to some other host on the network, and noticing that the
output from ping flags duplicates (typically one per slave).
(在首次使用 bonding 设备时，或其空闲一段时间之后，观察到一小段重复流量并不罕见。最容易的观察方法是向网络上另一台主机发起 "ping"，并注意 ping 输出中标记的重复包（通常每个从设备一个）。)

For example, on a bond in active-backup mode with five slaves
all connected to one switch, the output may appear as follows::
(例如，在一个 active-backup 模式、五个从设备全部连接到同一台交换机的 bond 上，输出可能如下：)

	# ping -n 10.0.4.2
	PING 10.0.4.2 (10.0.4.2) from 10.0.3.10 : 56(84) bytes of data.
	64 bytes from 10.0.4.2: icmp_seq=1 ttl=64 time=13.7 ms
	64 bytes from 10.0.4.2: icmp_seq=1 ttl=64 time=13.8 ms (DUP!)
	64 bytes from 10.0.4.2: icmp_seq=1 ttl=64 time=13.8 ms (DUP!)
	64 bytes from 10.0.4.2: icmp_seq=1 ttl=64 time=13.8 ms (DUP!)
	64 bytes from 10.0.4.2: icmp_seq=1 ttl=64 time=13.8 ms (DUP!)
	64 bytes from 10.0.4.2: icmp_seq=2 ttl=64 time=0.216 ms
	64 bytes from 10.0.4.2: icmp_seq=3 ttl=64 time=0.267 ms
	64 bytes from 10.0.4.2: icmp_seq=4 ttl=64 time=0.222 ms

This is not due to an error in the bonding driver, rather, it
is a side effect of how many switches update their MAC forwarding
tables.  Initially, the switch does not associate the MAC address in
the packet with a particular switch port, and so it may send the
traffic to all ports until its MAC forwarding table is updated.  Since
the interfaces attached to the bond may occupy multiple ports on a
single switch, when the switch (temporarily) floods the traffic to all
ports, the bond device receives multiple copies of the same packet
(one per slave device).
(这并非 bonding 驱动的错误，而是许多交换机更新其 MAC 转发表的方式带来的副作用。起初，交换机并未把数据包中的 MAC 地址与特定交换机端口关联起来，因此在 MAC 转发表更新之前，它可能把流量发往所有端口。由于连接到 bond 的接口可能占用同一台交换机上的多个端口，当交换机（临时）将流量泛洪到所有端口时，bond 设备会收到同一数据包的多个副本（每个从设备一份）。)

The duplicated packet behavior is switch dependent, some
switches exhibit this, and some do not.  On switches that display this
behavior, it can be induced by clearing the MAC forwarding table (on
most Cisco switches, the privileged command "clear mac address-table
dynamic" will accomplish this).
(重复数据包行为与交换机有关，有些交换机会出现，有些不会。在表现出此行为的交换机上，可以通过清空 MAC 转发表来诱发（在大多数 Cisco 交换机上，特权命令 "clear mac address-table dynamic" 即可完成）。)

14. Hardware Specific Considerations(14. 硬件相关注意事项)
==========================================================

This section contains additional information for configuring
bonding on specific hardware platforms, or for interfacing bonding
with particular switches or other devices.
(本节包含在特定硬件平台上配置 bonding，或让 bonding 与特定交换机及其他设备配合使用的补充信息。)

14.1 IBM BladeCenter(14.1 IBM BladeCenter)
------------------------------------------

This applies to the JS20 and similar systems.
(这适用于 JS20 及类似系统。)

On the JS20 blades, the bonding driver supports only
balance-rr, active-backup, balance-tlb and balance-alb modes.  This is
largely due to the network topology inside the BladeCenter, detailed
below.
(在 JS20 刀片服务器上，bonding 驱动仅支持 balance-rr、active-backup、balance-tlb 和 balance-alb 模式。这主要归因于 BladeCenter 内部的网络拓扑，详见下文。)

JS20 network adapter information(JS20 网络适配器信息)
-----------------------------------------------------

All JS20s come with two Broadcom Gigabit Ethernet ports
integrated on the planar (that's "motherboard" in IBM-speak).  In the
BladeCenter chassis, the eth0 port of all JS20 blades is hard wired to
I/O Module #1; similarly, all eth1 ports are wired to I/O Module #2.
An add-on Broadcom daughter card can be installed on a JS20 to provide
two more Gigabit Ethernet ports.  These ports, eth2 and eth3, are
wired to I/O Modules 3 and 4, respectively.
(所有 JS20 都在主板上（IBM 的说法叫 "planar"）集成了两个 Broadcom 千兆以太网端口。在 BladeCenter 机箱内，所有 JS20 刀片的 eth0 端口都硬连线到 I/O 模块 1；同样，所有 eth1 端口连线到 I/O 模块 2。可以在 JS20 上安装 Broadcom 子卡以提供另外两个千兆以太网端口。这两个端口 eth2 和 eth3 分别连线到 I/O 模块 3 和 4。)

Each I/O Module may contain either a switch or a passthrough
module (which allows ports to be directly connected to an external
switch).  Some bonding modes require a specific BladeCenter internal
network topology in order to function; these are detailed below.
(每个 I/O 模块可以包含交换机或直通模块（直通模块允许端口直接连接到外部交换机）。某些 bonding 模式需要特定的 BladeCenter 内部网络拓扑才能工作；详见下文。)

Additional BladeCenter-specific networking information can be
found in two IBM Redbooks (www.ibm.com/redbooks):
(更多 BladeCenter 特有的网络信息可参见两本 IBM 红皮书 (www.ibm.com/redbooks)：)

- "IBM eServer BladeCenter Networking Options"
- "IBM eServer BladeCenter Layer 2-7 Network Switching"

BladeCenter networking configuration(BladeCenter 网络配置)
----------------------------------------------------------

Because a BladeCenter can be configured in a very large number
of ways, this discussion will be confined to describing basic
configurations.
(由于 BladeCenter 的配置方式非常多，本节讨论将仅限于描述基本配置。)

Normally, Ethernet Switch Modules (ESMs) are used in I/O
modules 1 and 2.  In this configuration, the eth0 and eth1 ports of a
JS20 will be connected to different internal switches (in the
respective I/O modules).
(通常，以太网交换机模块 (ESM) 用在 I/O 模块 1 和 2 中。在这种配置中，JS20 的 eth0 和 eth1 端口会连接到不同的内部交换机（位于各自的 I/O 模块中）。)

A passthrough module (OPM or CPM, optical or copper,
passthrough module) connects the I/O module directly to an external
switch.  By using PMs in I/O module #1 and #2, the eth0 and eth1
interfaces of a JS20 can be redirected to the outside world and
connected to a common external switch.
(直通模块（OPM 或 CPM，光口或铜口直通模块）将 I/O 模块直接连接到外部交换机。通过在 I/O 模块 1 和 2 中使用直通模块 (PM)，JS20 的 eth0 和 eth1 接口可以被引到外部，并连接到同一台外部交换机。)

Depending upon the mix of ESMs and PMs, the network will
appear to bonding as either a single switch topology (all PMs) or as a
multiple switch topology (one or more ESMs, zero or more PMs).  It is
also possible to connect ESMs together, resulting in a configuration
much like the example in "High Availability in a Multiple Switch
Topology," above.
(根据 ESM 与 PM 的组合情况，网络在 bonding 看来要么是单交换机拓扑（全部为 PM），要么是多交换机拓扑（一个或多个 ESM，零个或多个 PM）。也可以把多台 ESM 相互连接，得到与上文 "High Availability in a Multiple Switch Topology" 示例非常相似的配置。)

Requirements for specific modes(特定模式的要求)
-----------------------------------------------

The balance-rr mode requires the use of passthrough modules
for devices in the bond, all connected to an common external switch.
That switch must be configured for "etherchannel" or "trunking" on the
appropriate ports, as is usual for balance-rr.
(balance-rr 模式要求 bond 中的设备使用直通模块，且全部连接到同一台外部交换机。该交换机的相应端口必须配置为 "etherchannel" 或 "trunking"，这是 balance-rr 的常规要求。)

The balance-alb and balance-tlb modes will function with
either switch modules or passthrough modules (or a mix).  The only
specific requirement for these modes is that all network interfaces
must be able to reach all destinations for traffic sent over the
bonding device (i.e., the network must converge at some point outside
the BladeCenter).
(balance-alb 和 balance-tlb 模式既可配合交换机模块工作，也可配合直通模块工作（或混合使用）。这些模式唯一的具体要求是：所有网络接口都必须能到达经由 bonding 设备发送的流量的所有目的地（即网络必须在 BladeCenter 之外的某处汇聚）。)

The active-backup mode has no additional requirements.
(active-backup 模式没有额外要求。)

Link monitoring issues(链路监控问题)
------------------------------------

When an Ethernet Switch Module is in place, only the ARP
monitor will reliably detect link loss to an external switch.  This is
nothing unusual, but examination of the BladeCenter cabinet would
suggest that the "external" network ports are the ethernet ports for
the system, when it fact there is a switch between these "external"
ports and the devices on the JS20 system itself.  The MII monitor is
only able to detect link failures between the ESM and the JS20 system.
(当使用以太网交换机模块时，只有 ARP 监控能可靠检测到与外部交换机之间的链路丢失。这没什么特别奇怪的，但查看 BladeCenter 机柜会让人觉得"外部"网络端口就是系统的以太网端口，而事实上这些"外部"端口与 JS20 系统自身设备之间还存在一台交换机。MII 监控只能检测 ESM 与 JS20 系统之间的链路故障。)

When a passthrough module is in place, the MII monitor does
detect failures to the "external" port, which is then directly
connected to the JS20 system.
(当使用直通模块时，MII 监控确实能检测到"外部"端口的故障，因为此时该端口直接连接到 JS20 系统。)

Other concerns(其他注意事项)
----------------------------

The Serial Over LAN (SoL) link is established over the primary
ethernet (eth0) only, therefore, any loss of link to eth0 will result
in losing your SoL connection.  It will not fail over with other
network traffic, as the SoL system is beyond the control of the
bonding driver.
(Serial Over LAN (SoL) 链路仅通过主以太网口 (eth0) 建立，因此任何到 eth0 的链路丢失都会导致 SoL 连接丢失。它不会随其他网络流量一起故障转移，因为 SoL 系统不在 bonding 驱动的控制范围内。)

It may be desirable to disable spanning tree on the switch
(either the internal Ethernet Switch Module, or an external switch) to
avoid fail-over delay issues when using bonding.
(可能最好在交换机（无论是内部以太网交换机模块还是外部交换机）上禁用生成树协议，以避免使用 bonding 时的故障转移延迟问题。)


15. Frequently Asked Questions(15. 常见问题)
============================================

1.  Is it SMP safe?(1. 它是 SMP 安全的吗？)
-------------------------------------------

Yes. The old 2.0.xx channel bonding patch was not SMP safe.
The new driver was designed to be SMP safe from the start.
(是的。旧的 2.0.xx 通道绑定补丁不是 SMP 安全的。新驱动从设计之初就考虑了 SMP 安全性。)

2.  What type of cards will work with it?(2. 什么类型的网卡可以配合它使用？)
----------------------------------------------------------------------------

Any Ethernet type cards (you can even mix cards - a Intel
EtherExpress PRO/100 and a 3com 3c905b, for example).  For most modes,
devices need not be of the same speed.
(任何以太网类型的网卡都可以（甚至可以混用不同网卡——例如 Intel EtherExpress PRO/100 与 3com 3c905b）。对大多数模式而言，各设备不必速率相同。)

Starting with version 3.2.1, bonding also supports Infiniband
slaves in active-backup mode.
(从 3.2.1 版本开始，bonding 在 active-backup 模式下还支持 Infiniband 从设备。)

3.  How many bonding devices can I have?(3. 我可以有多少个 bonding 设备？)
--------------------------------------------------------------------------

There is no limit.
(没有限制。)

4.  How many slaves can a bonding device have?(4. 一个 bonding 设备可以有多少个从设备？)
----------------------------------------------------------------------------------------

This is limited only by the number of network interfaces Linux
supports and/or the number of network cards you can place in your
system.
(这仅受 Linux 支持的网络接口数量和/或你能装进系统的网卡数量限制。)

5.  What happens when a slave link dies?(5. 从设备链路失效时会发生什么？)
-------------------------------------------------------------------------

If link monitoring is enabled, then the failing device will be
disabled.  The active-backup mode will fail over to a backup link, and
other modes will ignore the failed link.  The link will continue to be
monitored, and should it recover, it will rejoin the bond (in whatever
manner is appropriate for the mode). See the sections on High
Availability and the documentation for each mode for additional
information.
(如果启用了链路监控，失效的设备会被禁用。active-backup 模式会切换到备份链路，其他模式会忽略失效的链路。该链路会继续被监控，一旦恢复，它会重新加入 bond（以适合该模式的方式）。更多信息请参阅高可用性相关章节以及各模式的文档。)

Link monitoring can be enabled via either the miimon or
arp_interval parameters (described in the module parameters section,
above).  In general, miimon monitors the carrier state as sensed by
the underlying network device, and the arp monitor (arp_interval)
monitors connectivity to another host on the local network.
(链路监控可以通过 miimon 或 arp_interval 参数启用（见上文模块参数一节）。一般而言，miimon 监控底层网络设备感知到的载波状态，而 arp 监控 (arp_interval) 监控与本地网络上另一台主机的连通性。)

If no link monitoring is configured, the bonding driver will
be unable to detect link failures, and will assume that all links are
always available.  This will likely result in lost packets, and a
resulting degradation of performance.  The precise performance loss
depends upon the bonding mode and network configuration.
(如果未配置任何链路监控，bonding 驱动将无法检测链路故障，并会假设所有链路始终可用。这很可能导致丢包，进而造成性能下降。具体的性能损失取决于 bonding 模式和网络配置。)

6.  Can bonding be used for High Availability?(6. bonding 能用于高可用性吗？)
-----------------------------------------------------------------------------

Yes.  See the section on High Availability for details.
(可以。详情请参阅高可用性一节。)

7.  Which switches/systems does it work with?(7. 它可以与哪些交换机/系统配合工作？)
-----------------------------------------------------------------------------------

The full answer to this depends upon the desired mode.
(完整的答案取决于你所需要的模式。)

In the basic balance modes (balance-rr and balance-xor), it
works with any system that supports etherchannel (also called
trunking).  Most managed switches currently available have such
support, and many unmanaged switches as well.
(在基本均衡模式（balance-rr 和 balance-xor）下，它可以与任何支持 etherchannel（也称为 trunking）的系统配合工作。目前市面上的大多数网管型交换机都支持这一功能，许多非网管型交换机同样支持。)

The advanced balance modes (balance-tlb and balance-alb) do
not have special switch requirements, but do need device drivers that
support specific features (described in the appropriate section under
module parameters, above).
(高级均衡模式（balance-tlb 和 balance-alb）对交换机没有特殊要求，但需要设备驱动支持特定特性（见上文模块参数下的相应小节）。)

In 802.3ad mode, it works with systems that support IEEE
802.3ad Dynamic Link Aggregation.  Most managed and many unmanaged
switches currently available support 802.3ad.
(在 802.3ad 模式下，它可以与支持 IEEE 802.3ad 动态链路聚合的系统配合工作。目前市面上的大多数网管型交换机和许多非网管型交换机都支持 802.3ad。)

The active-backup mode should work with any Layer-II switch.
(active-backup 模式应当能与任何二层交换机配合工作。)

8.  Where does a bonding device get its MAC address from?(8. bonding 设备的 MAC 地址从哪里来？)
-----------------------------------------------------------------------------------------------

When using slave devices that have fixed MAC addresses, or when
the fail_over_mac option is enabled, the bonding device's MAC address is
the MAC address of the active slave.
(当使用具有固定 MAC 地址的从设备，或启用了 fail_over_mac 选项时，bonding 设备的 MAC 地址就是活动从设备的 MAC 地址。)

For other configurations, if not explicitly configured (with
ifconfig or ip link), the MAC address of the bonding device is taken from
its first slave device.  This MAC address is then passed to all following
slaves and remains persistent (even if the first slave is removed) until
the bonding device is brought down or reconfigured.
(对于其他配置，如果没有显式配置（用 ifconfig 或 ip link），bonding 设备的 MAC 地址取自其第一个从设备。该 MAC 地址随后会传递给之后的所有从设备，并保持不变（即使第一个从设备被移除），直到 bonding 设备被关闭或重新配置。)

If you wish to change the MAC address, you can set it with
ifconfig or ip link::
(如果想更改 MAC 地址，可以用 ifconfig 或 ip link 设置：)

	# ifconfig bond0 hw ether 00:11:22:33:44:55

	# ip link set bond0 address 66:77:88:99:aa:bb

The MAC address can be also changed by bringing down/up the
device and then changing its slaves (or their order)::
(也可以通过先关闭/再打开设备，然后更改其从设备（或其顺序）来改变 MAC 地址：)

	# ifconfig bond0 down ; modprobe -r bonding
	# ifconfig bond0 .... up
	# ifenslave bond0 eth...

This method will automatically take the address from the next
slave that is added.
(这种方法会自动从下一个加入的从设备获取地址。)

To restore your slaves' MAC addresses, you need to detach them
from the bond (``ifenslave -d bond0 eth0``). The bonding driver will
then restore the MAC addresses that the slaves had before they were
enslaved.
(要恢复从设备原有的 MAC 地址，需要把它们从 bond 上解除 (``ifenslave -d bond0 eth0``)。此后 bonding 驱动会把各从设备恢复为绑定之前的 MAC 地址。)

9.  What bonding modes support native XDP?(9. 哪些 bonding 模式支持原生 XDP？)
------------------------------------------------------------------------------

  * balance-rr (0)
  * active-backup (1)
  * balance-xor (2)
  * 802.3ad (4)

Note that the vlan+srcmac hash policy does not support native XDP.
For other bonding modes, the XDP program must be loaded with generic mode.
(注意，vlan+srcmac 哈希策略不支持原生 XDP。对于其他 bonding 模式，XDP 程序必须以 generic 模式加载。)

16. Resources and Links(16. 资源与链接)
=======================================

The latest version of the bonding driver can be found in the latest
version of the linux kernel, found on http://kernel.org
(bonding 驱动的最新版本可在最新版 Linux 内核中找到，见 http://kernel.org。)

The latest version of this document can be found in the latest kernel
source (named Documentation/networking/bonding.rst).
(本文档的最新版本可在最新内核源码中找到（名为 Documentation/networking/bonding.rst）。)

Discussions regarding the development of the bonding driver take place
on the main Linux network mailing list, hosted at vger.kernel.org. The list
address is:
(关于 bonding 驱动开发的讨论在托管于 vger.kernel.org 的 Linux 主网络邮件列表上进行。列表地址为：)

netdev@vger.kernel.org

The administrative interface (to subscribe or unsubscribe) can
be found at:
(管理界面（用于订阅或退订）位于：)

http://vger.kernel.org/vger-lists.html#netdev
