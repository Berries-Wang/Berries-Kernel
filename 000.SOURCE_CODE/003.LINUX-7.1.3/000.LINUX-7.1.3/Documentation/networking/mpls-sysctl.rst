.. SPDX-License-Identifier: GPL-2.0

====================
MPLS Sysfs variables
====================

(MPLS Sysfs 变量)

/proc/sys/net/mpls/* Variables:
===============================

(/proc/sys/net/mpls/* 变量)

platform_labels - INTEGER
(platform_labels - INTEGER(整数))
	Number of entries in the platform label table.  It is not
	possible to configure forwarding for label values equal to or
	greater than the number of platform labels.
	(平台标签表中的条目数量。无法为大于或等于平台标签数量的标签值配置转发。)

	A dense utilization of the entries in the platform label table
	is possible and expected as the platform labels are locally
	allocated.
	(由于平台标签是本地分配的，平台标签表条目可以被密集使用，这也是预期行为。)

	If the number of platform label table entries is set to 0 no
	label will be recognized by the kernel and mpls forwarding
	will be disabled.
	(如果平台标签表条目数量被设置为 0，内核将不会识别任何标签，且 MPLS 转发将被禁用。)

	Reducing this value will remove all label routing entries that
	no longer fit in the table.
	(减小该值将删除所有无法再放入表中的标签路由条目。)

	Possible values: 0 - 1048575
	(可能取值：0 - 1048575)

	Default: 0
	(默认值：0)

ip_ttl_propagate - BOOL
(ip_ttl_propagate - BOOL(布尔值))
	Control whether TTL is propagated from the IPv4/IPv6 header to
	the MPLS header on imposing labels and propagated from the
	MPLS header to the IPv4/IPv6 header on popping the last label.
	(控制压入标签时是否把 TTL 从 IPv4/IPv6 头传播到 MPLS 头，以及弹出最后一个标签时是否把 TTL 从 MPLS 头传播回 IPv4/IPv6 头。)

	If disabled, the MPLS transport network will appear as a
	single hop to transit traffic.
	(如果禁用，对过境流量而言，MPLS 传输网络将表现为单跳。)

	* 0 - disabled / RFC 3443 [Short] Pipe Model
	  (0 - 禁用 / RFC 3443 [Short] Pipe 模型)
	* 1 - enabled / RFC 3443 Uniform Model (default)
	  (1 - 启用 / RFC 3443 Uniform 模型(默认))

default_ttl - INTEGER
(default_ttl - INTEGER(整数))
	Default TTL value to use for MPLS packets where it cannot be
	propagated from an IP header, either because one isn't present
	or ip_ttl_propagate has been disabled.
	(当无法从 IP 头传播 TTL 时——或者因为 IP 头不存在，或者因为 ip_ttl_propagate 已被禁用——MPLS 数据包所使用的默认 TTL 值。)

	Possible values: 1 - 255
	(可能取值：1 - 255)

	Default: 255
	(默认值：255)

conf/<interface>/input - BOOL
(conf/<interface>/input - BOOL(布尔值))
	Control whether packets can be input on this interface.
	(控制是否可以在该接口上输入数据包。)

	If disabled, packets will be discarded without further
	processing.
	(如果禁用，数据包将被丢弃，不再做进一步处理。)

	* 0 - disabled (default)
	  (0 - 禁用(默认))
	* not 0 - enabled
	  (非 0 - 启用)
