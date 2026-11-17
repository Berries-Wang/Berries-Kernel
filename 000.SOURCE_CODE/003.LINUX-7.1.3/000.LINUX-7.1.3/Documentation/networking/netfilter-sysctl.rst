.. SPDX-License-Identifier: GPL-2.0

=========================
Netfilter Sysfs variables
=========================

/proc/sys/net/netfilter/* Variables:(/proc/sys/net/netfilter/* 变量：)
======================================================================

nf_log_all_netns - BOOLEAN
(nf_log_all_netns - BOOLEAN（布尔值）)

	- 0 - disabled (default)
	  (0 - 禁用（默认）)
	- not 0 - enabled
	  (非 0 - 启用)

	By default, only init_net namespace can log packets into kernel log
	with LOG target; this aims to prevent containers from flooding host
	kernel log. If enabled, this target also works in other network
	namespaces. This variable is only accessible from init_net.
	(默认情况下，只有 init_net 命名空间可以通过 LOG target 将数据包记录到
	内核日志中；这是为了防止容器刷爆主机内核日志。启用后，该 target 也可以
	在其他网络命名空间中工作。此变量只能从 init_net 访问。)
