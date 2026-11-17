.. SPDX-License-Identifier: GPL-2.0

=========
IP Sysctl
=========

(IP Sysctl)

/proc/sys/net/ipv4/* Variables(/proc/sys/net/ipv4/* 变量)
=========================================================

ip_forward - BOOLEAN
	Forward Packets between interfaces.
	(在接口之间转发数据包。)

	This variable is special, its change resets all configuration
	parameters to their default state (RFC1122 for hosts, RFC1812
	for routers)
	(此变量较为特殊，修改它会将所有配置参数重置为默认状态（主机对应 RFC1122，路由器对应 RFC1812）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

ip_default_ttl - INTEGER
	Default value of TTL field (Time To Live) for outgoing (but not
	forwarded) IP packets. Should be between 1 and 255 inclusive.
	Default: 64 (as recommended by RFC1700)
	(发出的（而非转发的）IP 数据包 TTL 字段（生存时间）的默认值。应在 1 到 255 之间（含）。
	默认：64（RFC1700 推荐值）)

ip_no_pmtu_disc - INTEGER
	Disable Path MTU Discovery. If enabled in mode 1 and a
	fragmentation-required ICMP is received, the PMTU to this
	destination will be set to the smallest of the old MTU to
	this destination and min_pmtu (see below). You will need
	to raise min_pmtu to the smallest interface MTU on your system
	manually if you want to avoid locally generated fragments.
	(禁用路径 MTU 发现。若以模式 1 启用，且收到需要分片的 ICMP 报文，
	则到该目的地的 PMTU 将被设置为原 MTU 与 min_pmtu（见下文）两者中
	较小的一个。若要避免本地产生分片，需要手动把 min_pmtu 提高到系统中
	最小的接口 MTU。)

	In mode 2 incoming Path MTU Discovery messages will be
	discarded. Outgoing frames are handled the same as in mode 1,
	implicitly setting IP_PMTUDISC_DONT on every created socket.
	(模式 2 下，传入的路径 MTU 发现消息将被丢弃。发出的帧按模式 1 的方式
	处理，即在每个新建套接字上隐式设置 IP_PMTUDISC_DONT。)

	Mode 3 is a hardened pmtu discover mode. The kernel will only
	accept fragmentation-needed errors if the underlying protocol
	can verify them besides a plain socket lookup. Current
	protocols for which pmtu events will be honored are TCP and
	SCTP as they verify e.g. the sequence number or the
	association. This mode should not be enabled globally but is
	only intended to secure e.g. name servers in namespaces where
	TCP path mtu must still work but path MTU information of other
	protocols should be discarded. If enabled globally this mode
	could break other protocols.
	(模式 3 是强化的 pmtu 发现模式。除普通套接字查找之外，只有当底层协议
	能够验证这些错误时，内核才会接受 fragmentation-needed 错误。目前会采纳
	pmtu 事件的协议是 TCP 和 SCTP，因为它们会验证例如序列号或关联等信息。
	此模式不应全局启用，它只是用于保护命名空间中例如名字服务器之类的场景：
	其中 TCP 路径 MTU 仍须工作，而其他协议的路径 MTU 信息应被丢弃。若全局
	启用，此模式可能破坏其他协议。)

	Possible values: 0-3
	(可能的取值：0-3)

	Default: FALSE
	(默认：FALSE)

min_pmtu - INTEGER
	default 552 - minimum Path MTU. Unless this is changed manually,
	each cached pmtu will never be lower than this setting.
	(默认 552 —— 最小路径 MTU。除非手动修改，每个缓存的 pmtu 都不会低于此设置。)

ip_forward_use_pmtu - BOOLEAN
	By default we don't trust protocol path MTUs while forwarding
	because they could be easily forged and can lead to unwanted
	fragmentation by the router.
	You only need to enable this if you have user-space software
	which tries to discover path mtus by itself and depends on the
	kernel honoring this information. This is normally not the
	case.
	(默认情况下，转发时不信任协议路径 MTU，因为它们容易被伪造，并可能导致
	路由器进行不必要的分片。只有当用户态软件自行发现路径 MTU 并依赖内核
	采纳该信息时才需要启用此选项。通常情况并非如此。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

fwmark_reflect - BOOLEAN
	Controls the fwmark of kernel-generated IPv4 reply packets that are not
	associated with a socket for example, TCP RSTs or ICMP echo replies).
	If disabled, these packets have a fwmark of zero. If enabled, they have the
	fwmark of the packet they are replying to.
	(控制内核生成的、未关联到套接字的 IPv4 应答数据包（例如 TCP RST 或 ICMP
	回显应答）的 fwmark。若禁用，这些数据包的 fwmark 为零；若启用，则沿用
	其所应答数据包的 fwmark。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

fib_multipath_use_neigh - BOOLEAN
	Use status of existing neighbor entry when determining nexthop for
	multipath routes. If disabled, neighbor information is not used and
	packets could be directed to a failed nexthop. Only valid for kernels
	built with CONFIG_IP_ROUTE_MULTIPATH enabled.
	(在为多路径路由确定下一跳时，使用现有邻居表项的状态。若禁用，则不使用
	邻居信息，数据包可能被引导到已失效的下一跳。仅对编译时启用
	CONFIG_IP_ROUTE_MULTIPATH 的内核有效。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

fib_multipath_hash_policy - INTEGER
	Controls which hash policy to use for multipath routes. Only valid
	for kernels built with CONFIG_IP_ROUTE_MULTIPATH enabled.
	(控制多路径路由使用哪种哈希策略。仅对编译时启用 CONFIG_IP_ROUTE_MULTIPATH
	的内核有效。)

	Default: 0 (Layer 3)
	(默认：0（第 3 层）)

	Possible values:
	(可能的取值：)

	- 0 - Layer 3
	  (0 - 第 3 层)
	- 1 - Layer 4
	  (1 - 第 4 层)
	- 2 - Layer 3 or inner Layer 3 if present
	  (2 - 第 3 层，如存在内层则用内层第 3 层)
	- 3 - Custom multipath hash. Fields used for multipath hash calculation
	  are determined by fib_multipath_hash_fields sysctl
	  (3 - 自定义多路径哈希。用于多路径哈希计算的字段由 fib_multipath_hash_fields sysctl 决定)

fib_multipath_hash_fields - UNSIGNED INTEGER
	When fib_multipath_hash_policy is set to 3 (custom multipath hash), the
	fields used for multipath hash calculation are determined by this
	sysctl.
	(当 fib_multipath_hash_policy 设为 3（自定义多路径哈希）时，用于多路径
	哈希计算的字段由此 sysctl 决定。)

	This value is a bitmask which enables various fields for multipath hash
	calculation.
	(该值是一个位掩码，用于启用参与多路径哈希计算的各个字段。)

	Possible fields are:
	(可能的字段有：)

	====== ============================
	0x0001 Source IP address (源 IP 地址)
	0x0002 Destination IP address (目的 IP 地址)
	0x0004 IP protocol (IP 协议)
	0x0008 Unused (Flow Label) (未使用（流标签）)
	0x0010 Source port (源端口)
	0x0020 Destination port (目的端口)
	0x0040 Inner source IP address (内层源 IP 地址)
	0x0080 Inner destination IP address (内层目的 IP 地址)
	0x0100 Inner IP protocol (内层 IP 协议)
	0x0200 Inner Flow Label (内层流标签)
	0x0400 Inner source port (内层源端口)
	0x0800 Inner destination port (内层目的端口)
	====== ============================

	Default: 0x0007 (source IP, destination IP and IP protocol)
	(默认：0x0007（源 IP、目的 IP 与 IP 协议）)

fib_multipath_hash_seed - UNSIGNED INTEGER
	The seed value used when calculating hash for multipath routes. Applies
	to both IPv4 and IPv6 datapath. Only present for kernels built with
	CONFIG_IP_ROUTE_MULTIPATH enabled.
	(计算多路径路由哈希时所用的种子值。同时适用于 IPv4 与 IPv6 数据路径。
	仅在编译时启用 CONFIG_IP_ROUTE_MULTIPATH 的内核中存在。)

	When set to 0, the seed value used for multipath routing defaults to an
	internal random-generated one.
	(设为 0 时，多路径路由使用的种子值默认为内部随机生成的值。)

	The actual hashing algorithm is not specified -- there is no guarantee
	that a next hop distribution effected by a given seed will keep stable
	across kernel versions.
	(具体的哈希算法并未规定——无法保证由给定种子产生的下一跳分布在
	不同内核版本之间保持稳定。)

	Default: 0 (random)
	(默认：0（随机）)

fib_sync_mem - UNSIGNED INTEGER
	Amount of dirty memory from fib entries that can be backlogged before
	synchronize_rcu is forced.
	(在被迫调用 synchronize_rcu 之前，fib 表项可积压的脏内存量。)

	Default: 512kB   Minimum: 64kB   Maximum: 64MB
	(默认：512kB   最小：64kB   最大：64MB)

ip_forward_update_priority - INTEGER
	Whether to update SKB priority from "TOS" field in IPv4 header after it
	is forwarded. The new SKB priority is mapped from TOS field value
	according to an rt_tos2priority table (see e.g. man tc-prio).
	(转发之后是否根据 IPv4 头部的 "TOS" 字段更新 SKB 优先级。新的 SKB 优先级
	依据 rt_tos2priority 表从 TOS 字段值映射而来（参见 man tc-prio）。)

	Default: 1 (Update priority.)
	(默认：1（更新优先级）。)

	Possible values:
	(可能的取值：)

	- 0 - Do not update priority.
	  (0 - 不更新优先级。)
	- 1 - Update priority.
	  (1 - 更新优先级。)

route/max_size - INTEGER
	Maximum number of routes allowed in the kernel.  Increase
	this when using large numbers of interfaces and/or routes.
	(内核允许的最大路由数量。使用大量接口和/或路由时应增大此值。)

	From linux kernel 3.6 onwards, this is deprecated for ipv4
	as route cache is no longer used.
	(从 linux 内核 3.6 起，该设置对 ipv4 已弃用，因为路由缓存已不再使用。)

	From linux kernel 6.3 onwards, this is deprecated for ipv6
	as garbage collection manages cached route entries.
	(从 linux 内核 6.3 起，该设置对 ipv6 已弃用，因为缓存路由表项由垃圾回收管理。)

neigh/default/gc_thresh1 - INTEGER
	Minimum number of entries to keep.  Garbage collector will not
	purge entries if there are fewer than this number.
	(保留的最少表项数。表项数量少于此值时，垃圾回收器不会清除表项。)

	Default: 128
	(默认：128)

neigh/default/gc_thresh2 - INTEGER
	Threshold when garbage collector becomes more aggressive about
	purging entries. Entries older than 5 seconds will be cleared
	when over this number.
	(垃圾回收器开始更积极清除表项的阈值。表项数量超过此值时，闲置超过
	5 秒的表项将被清除。)

	Default: 512
	(默认：512)

neigh/default/gc_thresh3 - INTEGER
	Maximum number of non-PERMANENT neighbor entries allowed.  Increase
	this when using large numbers of interfaces and when communicating
	with large numbers of directly-connected peers.
	(允许的非 PERMANENT 邻居表项最大数量。使用大量接口，或与大量直连
	对端通信时应增大此值。)

	Default: 1024
	(默认：1024)

neigh/default/gc_interval - INTEGER
	Specifies how often the garbage collector for neighbor entries
	should run. This value applies to the entire table, not
	individual entries. Unused since kernel v2.6.8.
	(指定邻居表项垃圾回收器的运行频率。该值作用于整张表而非单个表项。
	自内核 v2.6.8 起已不再使用。)

	Default: 30 seconds
	(默认：30 秒)

neigh/default/gc_stale_time - INTEGER
	Determines how long a neighbor entry can remain unused before it is
	considered stale and eligible for garbage collection. Entries that have
	not been used for longer than this time will be removed by the garbage
	collector, unless they have active references, are marked as PERMANENT,
	or carry the NTF_EXT_LEARNED or NTF_EXT_VALIDATED flag. Stale entries
	are only removed by the periodic GC when there are at least gc_thresh1
	neighbors in the table.
	(决定邻居表项闲置多久之后被视为过期并可回收。超过此时间未使用的表项
	将被垃圾回收器移除，除非它们仍有活动引用、被标记为 PERMANENT，或带有
	NTF_EXT_LEARNED 或 NTF_EXT_VALIDATED 标志。只有当表中邻居数量至少达到
	gc_thresh1 时，周期性 GC 才会移除过期表项。)

	Default: 60 seconds
	(默认：60 秒)

neigh/default/unres_qlen_bytes - INTEGER
	The maximum number of bytes which may be used by packets
	queued for each	unresolved address by other network layers.
	(added in linux 3.3)
	(其他网络层为每个未解析地址排队的数据包可占用的最大字节数。
	（linux 3.3 中新增）)

	Setting negative value is meaningless and will return error.
	(设置负值没有意义，会返回错误。)

	Default: SK_WMEM_DEFAULT, (same as net.core.wmem_default).
	(默认：SK_WMEM_DEFAULT（与 net.core.wmem_default 相同）。)

		Exact value depends on architecture and kernel options,
		but should be enough to allow queuing 256 packets
		of medium size.
		(确切值取决于体系结构与内核选项，但应足以容纳 256 个中等大小的数据包排队。)

neigh/default/unres_qlen - INTEGER
	The maximum number of packets which may be queued for each
	unresolved address by other network layers.
	(其他网络层可为每个未解析地址排队的数据包最大数量。)

	(deprecated in linux 3.3) : use unres_qlen_bytes instead.
	(（linux 3.3 中已弃用）：请改用 unres_qlen_bytes。)

	Prior to linux 3.3, the default value is 3 which may cause
	unexpected packet loss. The current default value is calculated
	according to default value of unres_qlen_bytes and true size of
	packet.
	(linux 3.3 之前默认值为 3，可能导致意外的丢包。当前默认值根据
	unres_qlen_bytes 的默认值和数据包的实际大小计算得出。)

	Default: 101
	(默认：101)

neigh/default/interval_probe_time_ms - INTEGER
	The probe interval for neighbor entries with NTF_MANAGED flag,
	the min value is 1.
	(带有 NTF_MANAGED 标志的邻居表项的探测间隔，最小值为 1。)

	Default: 5000
	(默认：5000)

mtu_expires - INTEGER
	Time, in seconds, that cached PMTU information is kept.
	(缓存的 PMTU 信息保留的时间，单位为秒。)

min_adv_mss - INTEGER
	The advertised MSS depends on the first hop route MTU, but will
	never be lower than this setting.
	(通告的 MSS 取决于第一跳路由的 MTU，但绝不会低于此设置。)

fib_notify_on_flag_change - INTEGER
        Whether to emit RTM_NEWROUTE notifications whenever RTM_F_OFFLOAD/
        RTM_F_TRAP/RTM_F_OFFLOAD_FAILED flags are changed.
        (每当 RTM_F_OFFLOAD/RTM_F_TRAP/RTM_F_OFFLOAD_FAILED 标志发生变化时，是否发出 RTM_NEWROUTE 通知。)

        After installing a route to the kernel, user space receives an
        acknowledgment, which means the route was installed in the kernel,
        but not necessarily in hardware.
        It is also possible for a route already installed in hardware to change
        its action and therefore its flags. For example, a host route that is
        trapping packets can be "promoted" to perform decapsulation following
        the installation of an IPinIP/VXLAN tunnel.
        The notifications will indicate to user-space the state of the route.
        (向内核安装路由后，用户态会收到一条确认，表明该路由已装入内核，但不一定
        装入了硬件。已装入硬件的路由也可能改变其动作进而改变其标志。例如，在
        安装 IPinIP/VXLAN 隧道之后，原本对数据包进行陷阱捕获的主机路由可以被
        "提升"为执行解封装。这些通知会向用户态指示路由的状态。)

        Default: 0 (Do not emit notifications.)
        (默认：0（不发出通知）。)

        Possible values:
        (可能的取值：)

        - 0 - Do not emit notifications.
          (0 - 不发出通知。)
        - 1 - Emit notifications.
          (1 - 发出通知。)
        - 2 - Emit notifications only for RTM_F_OFFLOAD_FAILED flag change.
          (2 - 仅在 RTM_F_OFFLOAD_FAILED 标志变化时发出通知。)

IP Fragmentation:
(IP 分片：)

ipfrag_high_thresh - LONG INTEGER
	Maximum memory used to reassemble IP fragments.
	(用于重组 IP 分片的最大内存。)

ipfrag_low_thresh - LONG INTEGER
	(Obsolete since linux-4.17)
	Maximum memory used to reassemble IP fragments before the kernel
	begins to remove incomplete fragment queues to free up resources.
	The kernel still accepts new fragments for defragmentation.
	(（linux-4.17 起已过时）
	在内核开始移除不完整的分片队列以释放资源之前，用于重组 IP 分片的最大内存。
	内核仍会接受新的分片进行重组。)

ipfrag_time - INTEGER
	Time in seconds to keep an IP fragment in memory.
	(IP 分片在内存中保留的时间，单位为秒。)

ipfrag_max_dist - INTEGER
	ipfrag_max_dist is a non-negative integer value which defines the
	maximum "disorder" which is allowed among fragments which share a
	common IP source address. Note that reordering of packets is
	not unusual, but if a large number of fragments arrive from a source
	IP address while a particular fragment queue remains incomplete, it
	probably indicates that one or more fragments belonging to that queue
	have been lost. When ipfrag_max_dist is positive, an additional check
	is done on fragments before they are added to a reassembly queue - if
	ipfrag_max_dist (or more) fragments have arrived from a particular IP
	address between additions to any IP fragment queue using that source
	address, it's presumed that one or more fragments in the queue are
	lost. The existing fragment queue will be dropped, and a new one
	started. An ipfrag_max_dist value of zero disables this check.
	(ipfrag_max_dist 是一个非负整数值，定义共享同一 IP 源地址的分片之间允许的
	最大"乱序"程度。数据包乱序并不罕见，但如果大量分片从某个源 IP 地址到达而
	某个特定分片队列仍不完整，则很可能表明该队列的一个或多个分片已经丢失。
	当 ipfrag_max_dist 为正时，分片在加入重组队列之前会进行一项额外检查——
	如果在向使用该源地址的任一 IP 分片队列添加表项的间隔之间，从该 IP 地址
	到达了 ipfrag_max_dist 个（或更多）分片，则假定该队列中有一个或多个分片
	丢失。现有的分片队列将被丢弃，并开始一个新的队列。ipfrag_max_dist 为 0
	时禁用此检查。)

	Using a very small value, e.g. 1 or 2, for ipfrag_max_dist can
	result in unnecessarily dropping fragment queues when normal
	reordering of packets occurs, which could lead to poor application
	performance. Using a very large value, e.g. 50000, increases the
	likelihood of incorrectly reassembling IP fragments that originate
	from different IP datagrams, which could result in data corruption.
	Default: 64
	(为 ipfrag_max_dist 设置非常小的值（如 1 或 2），可能在发生正常的数据包
	乱序时不必要地丢弃分片队列，导致应用性能不佳。设置非常大的值（如 50000），
	则会增加把源自不同 IP 数据报的分片错误重组在一起的可能性，可能导致数据
	损坏。默认：64)

bc_forwarding - INTEGER
	bc_forwarding enables the feature described in rfc1812#section-5.3.5.2
	and rfc2644. It allows the router to forward directed broadcast.
	To enable this feature, the 'all' entry and the input interface entry
	should be set to 1.
	Default: 0
	(bc_forwarding 启用 rfc1812#section-5.3.5.2 与 rfc2644 所描述的特性，
	允许路由器转发定向广播。要启用该特性，'all' 项与入接口项都应设为 1。
	默认：0)

INET peer storage(INET 对端存储)
================================

inet_peer_threshold - INTEGER
	The approximate size of the storage.  Starting from this threshold
	entries will be thrown aggressively.  This threshold also determines
	entries' time-to-live and time intervals between garbage collection
	passes.  More entries, less time-to-live, less GC interval.
	(存储的近似容量。达到该阈值后，表项将被激进地丢弃。该阈值还决定表项的
	生存期以及垃圾回收各轮之间的时间间隔。表项越多，生存期越短，GC 间隔越短。)

inet_peer_minttl - INTEGER
	Minimum time-to-live of entries.  Should be enough to cover fragment
	time-to-live on the reassembling side.  This minimum time-to-live  is
	guaranteed if the pool size is less than inet_peer_threshold.
	Measured in seconds.
	(表项的最小生存期。应足以覆盖重组一侧的分片生存期。当池大小小于
	inet_peer_threshold 时，此最小生存期有保证。以秒计。)

inet_peer_maxttl - INTEGER
	Maximum time-to-live of entries.  Unused entries will expire after
	this period of time if there is no memory pressure on the pool (i.e.
	when the number of entries in the pool is very small).
	Measured in seconds.
	(表项的最大生存期。当池没有内存压力（即池中表项数量很少）时，未使用的
	表项将在此时限之后过期。以秒计。)

TCP variables(TCP 变量)
=======================

somaxconn - INTEGER
	Limit of socket listen() backlog, known in userspace as SOMAXCONN.
	Defaults to 4096. (Was 128 before linux-5.4)
	See also tcp_max_syn_backlog for additional tuning for TCP sockets.
	(套接字 listen() 积压队列（backlog）的上限，在用户态中称为 SOMAXCONN。
	默认 4096。（linux-5.4 之前为 128）
	TCP 套接字的进一步调优另见 tcp_max_syn_backlog。)

tcp_abort_on_overflow - BOOLEAN
	If listening service is too slow to accept new connections,
	reset them. Default state is FALSE. It means that if overflow
	occurred due to a burst, connection will recover. Enable this
	option _only_ if you are really sure that listening daemon
	cannot be tuned to accept connections faster. Enabling this
	option can harm clients of your server.
	(如果监听服务接受新连接的速度太慢，则将这些连接复位。默认状态为 FALSE，
	意味着若溢出由突发流量造成，连接会自行恢复。_只有_在确实无法通过调优
	让监听守护进程更快地接受连接时，才应启用此选项。启用此选项可能损害
	服务器的客户端。)

tcp_adv_win_scale - INTEGER
	Obsolete since linux-6.6
	Count buffering overhead as bytes/2^tcp_adv_win_scale
	(if tcp_adv_win_scale > 0) or bytes-bytes/2^(-tcp_adv_win_scale),
	if it is <= 0.
	(linux-6.6 起已过时。
	缓冲开销按 bytes/2^tcp_adv_win_scale（当 tcp_adv_win_scale > 0 时）或
	bytes-bytes/2^(-tcp_adv_win_scale)（当它 <= 0 时）计算。)

	Possible values are [-31, 31], inclusive.
	(可能的取值范围为 [-31, 31]（含端点）。)

	Default: 1
	(默认：1)

tcp_allowed_congestion_control - STRING
	Show/set the congestion control choices available to non-privileged
	processes. The list is a subset of those listed in
	tcp_available_congestion_control.
	(显示/设置非特权进程可用的拥塞控制算法选项。该列表是
	tcp_available_congestion_control 所列内容的子集。)

	Default is "reno" and the default setting (tcp_congestion_control).
	(默认为 "reno"，即默认设置（tcp_congestion_control）。)

tcp_app_win - INTEGER
	Reserve max(window/2^tcp_app_win, mss) of window for application
	buffer. Value 0 is special, it means that nothing is reserved.
	(为应用缓冲保留 max(window/2^tcp_app_win, mss) 大小的窗口。值 0 比较特殊，
	表示不保留任何空间。)

	Possible values are [0, 31], inclusive.
	(可能的取值范围为 [0, 31]（含端点）。)

	Default: 31
	(默认：31)

tcp_autocorking - BOOLEAN
	Enable TCP auto corking :
	When applications do consecutive small write()/sendmsg() system calls,
	we try to coalesce these small writes as much as possible, to lower
	total amount of sent packets. This is done if at least one prior
	packet for the flow is waiting in Qdisc queues or device transmit
	queue. Applications can still use TCP_CORK for optimal behavior
	when they know how/when to uncork their sockets.
	(启用 TCP 自动软木塞（auto corking）：
	当应用程序连续进行小的 write()/sendmsg() 系统调用时，我们会尝试尽可能
	合并这些小的写入，以减少发送的数据包总量。仅当该流至少有一个先前的
	数据包正在 Qdisc 队列或设备发送队列中等待时才会这样做。当应用知道自己
	应如何/何时"拔掉软木塞"（uncork 套接字）时，仍可使用 TCP_CORK 获得最优行为。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_available_congestion_control - STRING
	Shows the available congestion control choices that are registered.
	More congestion control algorithms may be available as modules,
	but not loaded.
	(显示已注册的可用拥塞控制算法选项。更多拥塞控制算法可能以模块形式提供，
	但尚未加载。)

tcp_base_mss - INTEGER
	The initial value of search_low to be used by the packetization layer
	Path MTU discovery (MTU probing).  If MTU probing is enabled,
	this is the initial MSS used by the connection.
	(分组层路径 MTU 发现（MTU 探测）所用 search_low 的初始值。若启用了
	MTU 探测，这就是连接使用的初始 MSS。)

tcp_mtu_probe_floor - INTEGER
	If MTU probing is enabled this caps the minimum MSS used for search_low
	for the connection.
	(若启用了 MTU 探测，此值为连接 search_low 所用最小 MSS 的上限。)

	Default : 48
	(默认：48)

tcp_min_snd_mss - INTEGER
	TCP SYN and SYNACK messages usually advertise an ADVMSS option,
	as described in RFC 1122 and RFC 6691.
	(TCP 的 SYN 与 SYNACK 报文通常会通告 ADVMSS 选项，如 RFC 1122 与
	RFC 6691 所述。)

	If this ADVMSS option is smaller than tcp_min_snd_mss,
	it is silently capped to tcp_min_snd_mss.
	(若该 ADVMSS 选项小于 tcp_min_snd_mss，则会被静默限制为
	tcp_min_snd_mss。)

	Default : 48 (at least 8 bytes of payload per segment)
	(默认：48（每段至少 8 字节载荷）)

tcp_congestion_control - STRING
	Set the congestion control algorithm to be used for new
	connections. The algorithm "reno" is always available, but
	additional choices may be available based on kernel configuration.
	Default is set as part of kernel configuration.
	For passive connections, the listener congestion control choice
	is inherited.
	(设置新连接使用的拥塞控制算法。算法 "reno" 始终可用，其他选项视内核
	配置而定。默认值在内核配置时设定。对于被动建立的连接，会继承监听方
	的拥塞控制算法选择。)

	[see setsockopt(listenfd, SOL_TCP, TCP_CONGESTION, "name" ...) ]
	(参见 setsockopt(listenfd, SOL_TCP, TCP_CONGESTION, "name" ...) )

tcp_dsack - BOOLEAN
	Allows TCP to send "duplicate" SACKs.
	(允许 TCP 发送"重复"SACK。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_early_retrans - INTEGER
	Tail loss probe (TLP) converts RTOs occurring due to tail
	losses into fast recovery (RFC8985). Note that
	TLP requires RACK to function properly (see tcp_recovery below)
	(尾部丢失探测（TLP）把由尾部丢失引起的 RTO 转换为快速恢复（RFC8985）。
	注意，TLP 需要 RACK 才能正常工作（见下文 tcp_recovery）。)

	Possible values:
	(可能的取值：)

		- 0 disables TLP
		  (0 禁用 TLP)
		- 3 or 4 enables TLP
		  (3 或 4 启用 TLP)

	Default: 3
	(默认：3)

tcp_ecn - INTEGER
	Control use of Explicit Congestion Notification (ECN) by TCP.
	ECN is used only when both ends of the TCP connection indicate support
	for it. This feature is useful in avoiding losses due to congestion by
	allowing supporting routers to signal congestion before having to drop
	packets. A host that supports ECN both sends ECN at the IP layer and
	feeds back ECN at the TCP layer. The highest variant of ECN feedback
	that both peers support is chosen by the ECN negotiation (Accurate ECN,
	ECN, or no ECN).
	(控制 TCP 使用显式拥塞通知（ECN）。只有当 TCP 连接两端都表示支持 ECN 时
	才会使用。该特性让支持的路由器能在不得不丢包之前通告拥塞，从而有助于
	避免因拥塞造成的丢包。支持 ECN 的主机既在 IP 层发送 ECN 标记，又在 TCP 层
	反馈 ECN。ECN 协商会选择两端都支持的最高版本的 ECN 反馈（Accurate ECN、
	ECN 或不使用 ECN）。)

	The highest negotiated variant for incoming connection requests
	and the highest variant requested by outgoing connection
	attempts:
	(对于入连接请求协商出的最高版本，以及出连接尝试请求的最高版本：)

	===== ==================== ====================
	Value Incoming connections Outgoing connections
	===== ==================== ====================
	0     No ECN               No ECN (无 ECN)
	1     ECN                  ECN (ECN)
	2     ECN                  No ECN (无 ECN)
	3     AccECN               AccECN (AccECN)
	4     AccECN               ECN (ECN)
	5     AccECN               No ECN (无 ECN)
	===== ==================== ====================

	Default: 2
	(默认：2)

tcp_ecn_option - INTEGER
	Control Accurate ECN (AccECN) option sending when AccECN has been
	successfully negotiated during handshake. Send logic inhibits
	sending AccECN options regarless of this setting when no AccECN
	option has been seen for the reverse direction.
	(控制在握手期间成功协商 Accurate ECN（AccECN）后 AccECN 选项的发送。
	当反向方向上未见过任何 AccECN 选项时，无论此设置如何，发送逻辑都会
	禁止发送 AccECN 选项。)

	Possible values are:
	(可能的取值为：)

	= ============================================================
	0 Never send AccECN option. This also disables sending AccECN
	  option in SYN/ACK during handshake. (从不发送 AccECN 选项。这也会禁止在握手期间的 SYN/ACK 中发送 AccECN 选项。)
	1 Send AccECN option sparingly according to the minimum option
	  rules outlined in draft-ietf-tcpm-accurate-ecn. (按照 draft-ietf-tcpm-accurate-ecn 中概述的最小选项规则谨慎地发送 AccECN 选项。)
	2 Send AccECN option on every packet whenever it fits into TCP
	  option space except when AccECN fallback is triggered. (只要能放进 TCP 选项空间，就在每个数据包上发送 AccECN 选项，但触发 AccECN 回退时除外。)
	3 Send AccECN option on every packet whenever it fits into TCP
	  option space even when AccECN fallback is triggered. (只要能放进 TCP 选项空间，就在每个数据包上发送 AccECN 选项，即使触发了 AccECN 回退。)
	= ============================================================

	Default: 2
	(默认：2)

tcp_ecn_option_beacon - INTEGER
	Control Accurate ECN (AccECN) option sending frequency per RTT and it
	takes effect only when tcp_ecn_option is set to 2.
	(控制 Accurate ECN（AccECN）选项每个 RTT 的发送频率，仅在
	tcp_ecn_option 设为 2 时生效。)

	Default: 3 (AccECN will be send at least 3 times per RTT)
	(默认：3（AccECN 每个 RTT 至少发送 3 次）)

tcp_ecn_fallback - BOOLEAN
	If the kernel detects that ECN connection misbehaves, enable fall
	back to non-ECN. Currently, this knob implements the fallback
	from RFC3168, section 6.1.1.1., but we reserve that in future,
	additional detection mechanisms could be implemented under this
	knob. The value	is not used, if tcp_ecn or per route (or congestion
	control) ECN settings are disabled.
	(如果内核检测到 ECN 连接行为异常，则启用回退到非 ECN。目前该开关实现
	RFC3168 第 6.1.1.1 节的回退机制，但未来可能在此开关下实现额外的检测
	机制。若 tcp_ecn 或按路由（或按拥塞控制）的 ECN 设置被禁用，则该值
	不起作用。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_fack - BOOLEAN
	This is a legacy option, it has no effect anymore.
	(这是一个遗留选项，已不再有任何作用。)

tcp_fin_timeout - INTEGER
	The length of time an orphaned (no longer referenced by any
	application) connection will remain in the FIN_WAIT_2 state
	before it is aborted at the local end.  While a perfectly
	valid "receive only" state for an un-orphaned connection, an
	orphaned connection in FIN_WAIT_2 state could otherwise wait
	forever for the remote to close its end of the connection.
	(孤立（不再被任何应用引用）的连接在被本地中止之前，停留在 FIN_WAIT_2
	状态的时间。对非孤立的连接而言这是一个完全正常的"仅接收"状态，但处于
	FIN_WAIT_2 状态的孤立连接若不中止，可能会永远等待对端关闭其连接端。)

	Cf. tcp_max_orphans
	(另见 tcp_max_orphans)

	Default: 60 seconds
	(默认：60 秒)

tcp_frto - INTEGER
	Enables Forward RTO-Recovery (F-RTO) defined in RFC5682.
	F-RTO is an enhanced recovery algorithm for TCP retransmission
	timeouts.  It is particularly beneficial in networks where the
	RTT fluctuates (e.g., wireless). F-RTO is sender-side only
	modification. It does not require any support from the peer.
	(启用 RFC5682 定义的 Forward RTO-Recovery（F-RTO）。F-RTO 是针对 TCP
	重传超时的增强恢复算法，在 RTT 波动的网络（如无线网络）中特别有益。
	F-RTO 只涉及发送端的修改，不需要对端的任何支持。)

	By default it's enabled with a non-zero value. 0 disables F-RTO.
	(默认以非零值启用。0 表示禁用 F-RTO。)

tcp_fwmark_accept - BOOLEAN
	If enabled, incoming connections to listening sockets that do not have a
	socket mark will set the mark of the accepting socket to the fwmark of
	the incoming SYN packet. This will cause all packets on that connection
	(starting from the first SYNACK) to be sent with that fwmark. The
	listening socket's mark is unchanged. Listening sockets that already
	have a fwmark set via setsockopt(SOL_SOCKET, SO_MARK, ...) are
	unaffected.
	(若启用，对于没有套接字标记的监听套接字，入连接会把接受套接字的标记设为
	入站 SYN 数据包的 fwmark。这会使该连接上的所有数据包（从第一个 SYNACK
	开始）都携带该 fwmark 发送。监听套接字的标记保持不变。已经通过
	setsockopt(SOL_SOCKET, SO_MARK, ...) 设置了 fwmark 的监听套接字不受影响。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_invalid_ratelimit - INTEGER
	Limit the maximal rate for sending duplicate acknowledgments
	in response to incoming TCP packets that are for an existing
	connection but that are invalid due to any of these reasons:
	(限制针对以下入站 TCP 数据包发送重复确认的最大速率：这些数据包属于
	某个已存在的连接，但因下列原因之一而无效：)

	  (a) out-of-window sequence number,
	      ((a) 序列号超出窗口，)
	  (b) out-of-window acknowledgment number, or
	      ((b) 确认号超出窗口，或)
	  (c) PAWS (Protection Against Wrapped Sequence numbers) check failure
	      ((c) PAWS（防止序列号回绕）检查失败)

	This can help mitigate simple "ack loop" DoS attacks, wherein
	a buggy or malicious middlebox or man-in-the-middle can
	rewrite TCP header fields in manner that causes each endpoint
	to think that the other is sending invalid TCP segments, thus
	causing each side to send an unterminating stream of duplicate
	acknowledgments for invalid segments.
	(这有助于缓解简单的"ack 循环"DoS 攻击：有缺陷或恶意的中间设备或中间人
	改写 TCP 头部字段，使每一端都认为对方在发送无效的 TCP 段，从而导致双方
	针对无效段无休止地发送重复确认。)

	Using 0 disables rate-limiting of dupacks in response to
	invalid segments; otherwise this value specifies the minimal
	space between sending such dupacks, in milliseconds.
	(设为 0 表示禁用对无效段重复确认的限速；否则该值指定发送此类重复确认
	之间的最小间隔，单位为毫秒。)

	Default: 500 (milliseconds).
	(默认：500（毫秒）。)

tcp_keepalive_time - INTEGER
	How often TCP sends out keepalive messages when keepalive is enabled.
	Default: 2hours.
	(启用 keepalive 时 TCP 发送保活消息的频率。
	默认：2 小时。)

tcp_keepalive_probes - INTEGER
	How many keepalive probes TCP sends out, until it decides that the
	connection is broken. Default value: 9.
	(TCP 在判定连接已断开之前发送的保活探测次数。默认值：9。)

tcp_keepalive_intvl - INTEGER
	How frequently the probes are send out. Multiplied by
	tcp_keepalive_probes it is time to kill not responding connection,
	after probes started. Default value: 75sec i.e. connection
	will be aborted after ~11 minutes of retries.
	(探测报文的发送间隔。探测开始后，该值乘以 tcp_keepalive_probes 就是
	终止无响应连接所需的时间。默认值：75 秒，即连接会在约 11 分钟的重试后
	被中止。)

tcp_l3mdev_accept - BOOLEAN
	Enables child sockets to inherit the L3 master device index.
	Enabling this option allows a "global" listen socket to work
	across L3 master domains (e.g., VRFs) with connected sockets
	derived from the listen socket to be bound to the L3 domain in
	which the packets originated. Only valid when the kernel was
	compiled with CONFIG_NET_L3_MASTER_DEV.
	(允许子套接字继承 L3 主设备索引。启用此选项后，"全局"监听套接字可以
	跨 L3 主域（例如 VRF）工作，由监听套接字派生的连接套接字会绑定到数据包
	起源所在的 L3 域。仅在内核编译时启用 CONFIG_NET_L3_MASTER_DEV 时有效。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_low_latency - BOOLEAN
	This is a legacy option, it has no effect anymore.
	(这是一个遗留选项，已不再有任何作用。)

tcp_max_orphans - INTEGER
	Maximal number of TCP sockets not attached to any user file handle,
	held by system.	If this number is exceeded orphaned connections are
	reset immediately and warning is printed. This limit exists
	only to prevent simple DoS attacks, you _must_ not rely on this
	or lower the limit artificially, but rather increase it
	(probably, after increasing installed memory),
	if network conditions require more than default value,
	and tune network services to linger and kill such states
	more aggressively. Let me to remind again: each orphan eats
	up to ~64K of unswappable memory.
	(系统中未被任何用户文件句柄引用的 TCP 套接字的最大数量。超过此数量时，
	孤立连接会被立即复位并打印警告。此限制的存在只是为了防止简单的 DoS
	攻击，_切勿_依赖它或人为调低限制；当网络状况需要超过默认值时，应当增大
	它（可能在增加安装内存之后），并调优网络服务以更积极地清理和终止这类
	状态。再次提醒：每个孤立连接最多占用约 64K 不可换出的内存。)

tcp_max_syn_backlog - INTEGER
	Maximal number of remembered connection requests (SYN_RECV),
	which have not received an acknowledgment from connecting client.
	(已记录但尚未收到连接客户端确认的连接请求（SYN_RECV）的最大数量。)

	This is a per-listener limit.
	(这是一个针对每个监听者的限制。)

	The minimal value is 128 for low memory machines, and it will
	increase in proportion to the memory of machine.
	(低内存机器的最小值为 128，该值会随机器内存按比例增大。)

	If server suffers from overload, try increasing this number.
	(如果服务器出现过载，可尝试增大此数值。)

	Remember to also check /proc/sys/net/core/somaxconn
	A SYN_RECV request socket consumes about 304 bytes of memory.
	(记得同时检查 /proc/sys/net/core/somaxconn。
	一个 SYN_RECV 请求套接字占用约 304 字节内存。)

tcp_max_tw_buckets - INTEGER
	Maximal number of timewait sockets held by system simultaneously.
	If this number is exceeded time-wait socket is immediately destroyed
	and warning is printed. This limit exists only to prevent
	simple DoS attacks, you _must_ not lower the limit artificially,
	but rather increase it (probably, after increasing installed memory),
	if network conditions require more than default value.
	(系统同时持有的 time-wait 套接字的最大数量。超过此数量时，time-wait
	套接字会被立即销毁并打印警告。此限制的存在只是为了防止简单的 DoS 攻击，
	_切勿_人为调低限制；当网络状况需要超过默认值时，应当增大它（可能在
	增加安装内存之后）。)

tcp_mem - vector of 3 INTEGERs: min, pressure, max
	min: below this number of pages TCP is not bothered about its
	memory appetite.
	(min：TCP 占用内存低于该页数时，不必担心内存消耗。)

	pressure: when amount of memory allocated by TCP exceeds this number
	of pages, TCP moderates its memory consumption and enters memory
	pressure mode, which is exited when memory consumption falls
	under "min".
	(pressure：当 TCP 分配的内存超过该页数时，TCP 会节制其内存消耗并进入
	内存压力模式；当内存消耗回落到 "min" 以下时退出该模式。)

	max: number of pages allowed for queueing by all TCP sockets.
	(max：允许所有 TCP 套接字用于排队的页数。)

	Defaults are calculated at boot time from amount of available
	memory.
	(默认值在引导时根据可用内存量计算。)

tcp_min_rtt_wlen - INTEGER
	The window length of the windowed min filter to track the minimum RTT.
	A shorter window lets a flow more quickly pick up new (higher)
	minimum RTT when it is moved to a longer path (e.g., due to traffic
	engineering). A longer window makes the filter more resistant to RTT
	inflations such as transient congestion. The unit is seconds.
	(用于跟踪最小 RTT 的窗口化最小值滤波器的窗口长度。窗口较短时，流被
	迁移到更长路径上（例如由于流量工程）后能更快地采拾新的（更大的）最小
	RTT。窗口较长时，滤波器对瞬时拥塞等造成的 RTT 膨胀更有抵抗力。单位为
	秒。)

	Possible values: 0 - 86400 (1 day)
	(可能的取值：0 - 86400（1 天）)

	Default: 300
	(默认：300)

tcp_moderate_rcvbuf - BOOLEAN
	If enabled, TCP performs receive buffer auto-tuning, attempting to
	automatically size the buffer (no greater than tcp_rmem[2]) to
	match the size required by the path for full throughput.
	(若启用，TCP 会进行接收缓冲自动调优，尝试自动把缓冲大小（不超过
	tcp_rmem[2]）调整到路径达到满吞吐所需的大小。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_rcvbuf_low_rtt - INTEGER
	rcvbuf autotuning can over estimate final socket rcvbuf, which
	can lead to cache trashing for high throughput flows.
	(rcvbuf 自动调优可能高估套接字最终的 rcvbuf，导致高吞吐流产生缓存颠簸。)

	For small RTT flows (below tcp_rcvbuf_low_rtt usecs), we can relax
	rcvbuf growth: Few additional ms to reach the final (and smaller)
	rcvbuf is a good tradeoff.
	(对于小 RTT 流（低于 tcp_rcvbuf_low_rtt 微秒），可以放宽 rcvbuf 的增长：
	多花几毫秒达到最终（更小的）rcvbuf 是一个不错的折中。)

	Default : 1000 (1 ms)
	(默认：1000（1 毫秒）)

tcp_mtu_probing - INTEGER
	Controls TCP Packetization-Layer Path MTU Discovery.  Takes three
	values:
	(控制 TCP 分组层路径 MTU 发现。取三个值：)

	- 0 - Disabled
	  (0 - 禁用)
	- 1 - Disabled by default, enabled when an ICMP black hole detected
	  (1 - 默认禁用，检测到 ICMP 黑洞时启用)
	- 2 - Always enabled, use initial MSS of tcp_base_mss.
	  (2 - 始终启用，使用 tcp_base_mss 的初始 MSS。)

tcp_probe_interval - UNSIGNED INTEGER
	Controls how often to start TCP Packetization-Layer Path MTU
	Discovery reprobe. The default is reprobing every 10 minutes as
	per RFC4821.
	(控制 TCP 分组层路径 MTU 发现重新探测的启动频率。默认按 RFC4821
	每 10 分钟重新探测一次。)

tcp_probe_threshold - INTEGER
	Controls when TCP Packetization-Layer Path MTU Discovery probing
	will stop in respect to the width of search range in bytes. Default
	is 8 bytes.
	(控制 TCP 分组层路径 MTU 发现探测在搜索范围宽度（以字节计）达到多少时
	停止。默认为 8 字节。)

tcp_no_metrics_save - BOOLEAN
	By default, TCP saves various connection metrics in the route cache
	when the connection closes, so that connections established in the
	near future can use these to set initial conditions.  Usually, this
	increases overall performance, but may sometimes cause performance
	degradation.  If enabled, TCP will not cache metrics on closing
	connections.
	(默认情况下，TCP 会在连接关闭时把各种连接度量保存到路由缓存中，以便
	近期建立的连接使用它们设置初始条件。这通常会提升整体性能，但有时也
	可能造成性能下降。若启用，TCP 在连接关闭时不再缓存度量。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_no_ssthresh_metrics_save - BOOLEAN
	Controls whether TCP saves ssthresh metrics in the route cache.
	If enabled, ssthresh metrics are disabled.
	(控制 TCP 是否把 ssthresh 度量保存到路由缓存。若启用，则不保存
	ssthresh 度量。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_orphan_retries - INTEGER
	This value influences the timeout of a locally closed TCP connection,
	when RTO retransmissions remain unacknowledged.
	See tcp_retries2 for more details.
	(当 RTO 重传一直未得到确认时，该值影响本地已关闭 TCP 连接的超时。
	详情见 tcp_retries2。)

	The default value is 8.
	(默认值为 8。)

	If your machine is a loaded WEB server,
	you should think about lowering this value, such sockets
	may consume significant resources. Cf. tcp_max_orphans.
	(如果你的机器是负载很重的 WEB 服务器，应考虑调低此值，这类套接字
	可能消耗大量资源。另见 tcp_max_orphans。)

tcp_recovery - INTEGER
	This value is a bitmap to enable various experimental loss recovery
	features.
	(该值是一个位图，用于启用各种实验性的丢包恢复特性。)

	=========   =============================================================
	RACK: 0x1   enables RACK loss detection, for fast detection of lost
		    retransmissions and tail drops, and resilience to
		    reordering. currently, setting this bit to 0 has no
		    effect, since RACK is the only supported loss detection
		    algorithm. (启用 RACK 丢包检测，用于快速检测丢失的重传和尾部丢弃，并能容忍乱序。目前将该位设为 0 没有任何作用，因为 RACK 是唯一支持的丢包检测算法。)

	RACK: 0x2   makes RACK's reordering window static (min_rtt/4). (使 RACK 的乱序窗口保持静态（min_rtt/4）。)

	RACK: 0x4   disables RACK's DUPACK threshold heuristic (禁用 RACK 的 DUPACK 阈值启发式)
	=========   =============================================================

	Default: 0x1
	(默认：0x1)

tcp_reflect_tos - BOOLEAN
	For listening sockets, reuse the DSCP value of the initial SYN message
	for outgoing packets. This allows to have both directions of a TCP
	stream to use the same DSCP value, assuming DSCP remains unchanged for
	the lifetime of the connection.
	(对监听套接字而言，在发出的数据包上复用初始 SYN 报文的 DSCP 值。这使得
	TCP 流的两个方向可以使用相同的 DSCP 值，前提是 DSCP 在连接的整个生命期
	内保持不变。)

	This options affects both IPv4 and IPv6.
	(此选项同时影响 IPv4 与 IPv6。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_reordering - INTEGER
	Initial reordering level of packets in a TCP stream.
	TCP stack can then dynamically adjust flow reordering level
	between this initial value and tcp_max_reordering
	(TCP 流中数据包的初始乱序级别。之后 TCP 协议栈可以在该初始值与
	tcp_max_reordering 之间动态调整流的乱序级别)

	Default: 3
	(默认：3)

tcp_max_reordering - INTEGER
	Maximal reordering level of packets in a TCP stream.
	300 is a fairly conservative value, but you might increase it
	if paths are using per packet load balancing (like bonding rr mode)
	(TCP 流中数据包的最大乱序级别。300 是相当保守的值；如果路径使用按包
	负载均衡（如 bonding 的 rr 模式），可以适当增大。)

	Default: 300
	(默认：300)

tcp_retrans_collapse - BOOLEAN
	Bug-to-bug compatibility with some broken printers.
	On retransmit try to send bigger packets to work around bugs in
	certain TCP stacks.
	(与某些有缺陷的打印机进行"以错对错"的兼容。在重传时尝试发送更大的
	数据包，以规避某些 TCP 协议栈中的缺陷。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_retries1 - INTEGER
	This value influences the time, after which TCP decides, that
	something is wrong due to unacknowledged RTO retransmissions,
	and reports this suspicion to the network layer.
	See tcp_retries2 for more details.
	(该值影响 TCP 经过多久之后，因 RTO 重传一直未得到确认而判定出现问题，
	并将这一疑虑上报给网络层。详情见 tcp_retries2。)

	RFC 1122 recommends at least 3 retransmissions, which is the
	default.
	(RFC 1122 建议至少重传 3 次，这也是默认值。)

tcp_retries2 - INTEGER
	This value influences the timeout of an alive TCP connection,
	when RTO retransmissions remain unacknowledged.
	Given a value of N, a hypothetical TCP connection following
	exponential backoff with an initial RTO of TCP_RTO_MIN would
	retransmit N times before killing the connection at the (N+1)th RTO.
	(当 RTO 重传一直未得到确认时，该值影响存活 TCP 连接的超时。给定值 N，
	一条按指数退避、初始 RTO 为 TCP_RTO_MIN 的假想 TCP 连接会重传 N 次，
	然后在第 (N+1) 次 RTO 时终止连接。)

	The default value of 15 yields a hypothetical timeout of 924.6
	seconds and is a lower bound for the effective timeout.
	TCP will effectively time out at the first RTO which exceeds the
	hypothetical timeout.
	If tcp_rto_max_ms is decreased, it is recommended to also
	change tcp_retries2.
	(默认值 15 对应的假想超时为 924.6 秒，是实际超时的下界。TCP 实际上会在
	第一个超过假想超时的 RTO 处超时。如果调小了 tcp_rto_max_ms，建议同时
	调整 tcp_retries2。)

	RFC 1122 recommends at least 100 seconds for the timeout,
	which corresponds to a value of at least 8.
	(RFC 1122 建议超时至少为 100 秒，对应的取值至少为 8。)

tcp_rfc1337 - BOOLEAN
	If enabled, the TCP stack behaves conforming to RFC1337. If unset,
	we are not conforming to RFC, but prevent TCP TIME_WAIT
	assassination.
	(若启用，TCP 协议栈的行为遵循 RFC1337。若未启用，虽然不符合 RFC，
	但可以防止 TCP TIME_WAIT 刺杀（assassination）问题。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_rmem - vector of 3 INTEGERs: min, default, max
	min: Minimal size of receive buffer used by TCP sockets.
	It is guaranteed to each TCP socket, even under moderate memory
	pressure.
	(min：TCP 套接字接收缓冲的最小大小。即使在中等的内存压力下，每个
	TCP 套接字也能得到保证。)

	Default: 4K
	(默认：4K)

	default: initial size of receive buffer used by TCP sockets.
	This value overrides net.core.rmem_default used by other protocols.
	Default: 131072 bytes.
	This value results in initial window of 65535.
	(default：TCP 套接字接收缓冲的初始大小。该值覆盖其他协议使用的
	net.core.rmem_default。默认：131072 字节。该值对应 65535 的初始窗口。)

	max: maximal size of receive buffer allowed for automatically
	selected receiver buffers for TCP socket.
	Calling setsockopt() with SO_RCVBUF disables
	automatic tuning of that socket's receive buffer size, in which
	case this value is ignored.
	Default: between 131072 and 32MB, depending on RAM size.
	(max：TCP 套接字自动选择的接收缓冲所允许的最大大小。
	以 SO_RCVBUF 调用 setsockopt() 会禁用该套接字接收缓冲大小的自动调优，
	此时忽略此值。默认：介于 131072 与 32MB 之间，取决于内存大小。)

tcp_sack - BOOLEAN
	Enable select acknowledgments (SACKS).
	(启用选择性确认（SACK）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_comp_sack_rtt_percent - INTEGER
	Percentage of SRTT used for the compressed SACK feature.
	See tcp_comp_sack_nr, tcp_comp_sack_delay_ns, tcp_comp_sack_slack_ns.
	(SACK 压缩特性使用的 SRTT 百分比。另见 tcp_comp_sack_nr、
	tcp_comp_sack_delay_ns、tcp_comp_sack_slack_ns。)

	Possible values : 1 - 1000
	(可能的取值：1 - 1000)

	Default : 33 %
	(默认：33 %)

tcp_comp_sack_delay_ns - LONG INTEGER
	TCP tries to reduce number of SACK sent, using a timer based
	on tcp_comp_sack_rtt_percent of SRTT, capped by this sysctl
	in nano seconds.
	The default is 1ms, based on TSO autosizing period.
	(TCP 尝试减少发送的 SACK 数量：使用一个基于 SRTT 的
	tcp_comp_sack_rtt_percent 的定时器，并以该 sysctl 的纳秒值封顶。
	默认为 1 毫秒，基于 TSO 自动尺寸调整周期。)

	Default : 1,000,000 ns (1 ms)
	(默认：1,000,000 ns（1 毫秒）)

tcp_comp_sack_slack_ns - LONG INTEGER
	This sysctl control the slack used when arming the
	timer used by SACK compression. This gives extra time
	for small RTT flows, and reduces system overhead by allowing
	opportunistic reduction of timer interrupts.
	Too big values might reduce goodput.
	(该 sysctl 控制 SACK 压缩定时器启动时的松弛量。这为小 RTT 流提供了
	额外的时间，并通过允许伺机减少定时器中断来降低系统开销。
	过大的值可能降低有效吞吐。)

	Default : 10,000 ns (10 us)
	(默认：10,000 ns（10 微秒）)

tcp_comp_sack_nr - INTEGER
	Max number of SACK that can be compressed.
	Using 0 disables SACK compression.
	(可压缩的 SACK 最大数量。设为 0 禁用 SACK 压缩。)

	Default : 44
	(默认：44)

tcp_backlog_ack_defer - BOOLEAN
	If enabled, user thread processing socket backlog tries sending
	one ACK for the whole queue. This helps to avoid potential
	long latencies at end of a TCP socket syscall.
	(若启用，处理套接字积压队列的用户线程会尝试为整个队列只发送一个 ACK。
	这有助于避免 TCP 套接字系统调用结束时的潜在长延迟。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_slow_start_after_idle - BOOLEAN
	If enabled, provide RFC2861 behavior and time out the congestion
	window after an idle period.  An idle period is defined at
	the current RTO.  If unset, the congestion window will not
	be timed out after an idle period.
	(若启用，提供 RFC2861 的行为：在空闲期结束后使拥塞窗口超时失效。空闲期
	按当前 RTO 定义。若未启用，拥塞窗口不会在空闲期后超时失效。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_stdurg - BOOLEAN
	Use the Host requirements interpretation of the TCP urgent pointer field.
	Most hosts use the older BSD interpretation, so if enabled,
	Linux might not communicate correctly with them.
	(对 TCP 紧急指针字段采用《主机要求》（Host Requirements）的解释。
	大多数主机使用较旧的 BSD 解释，因此启用后 Linux 可能无法与它们正确
	通信。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_synack_retries - INTEGER
	Number of times SYNACKs for a passive TCP connection attempt will
	be retransmitted. Should not be higher than 255. Default value
	is 5, which corresponds to 31seconds till the last retransmission
	with the current initial RTO of 1second. With this the final timeout
	for a passive TCP connection will happen after 63seconds.
	(被动 TCP 连接尝试的 SYNACK 重传次数。不应高于 255。默认值为 5，在当前
	初始 RTO 为 1 秒的情况下，对应到最后一次重传为止共 31 秒。据此，被动
	TCP 连接的最终超时将发生在 63 秒之后。)

tcp_syncookies - INTEGER
	Only valid when the kernel was compiled with CONFIG_SYN_COOKIES
	Send out syncookies when the syn backlog queue of a socket
	overflows. This is to prevent against the common 'SYN flood attack'
	Default: 1
	(仅在内核编译时启用 CONFIG_SYN_COOKIES 时有效。当套接字的 syn 积压
	队列溢出时发送 syncookies，用于防范常见的 'SYN flood attack'。
	默认：1)

	Note, that syncookies is fallback facility.
	It MUST NOT be used to help highly loaded servers to stand
	against legal connection rate. If you see SYN flood warnings
	in your logs, but investigation	shows that they occur
	because of overload with legal connections, you should tune
	another parameters until this warning disappear.
	See: tcp_max_syn_backlog, tcp_synack_retries, tcp_abort_on_overflow.
	(注意，syncookies 是一种后备手段。它绝不得用于帮助高负载服务器抵御
	合法的连接速率。如果日志中出现 SYN flood 警告，但排查表明它们是因
	合法连接过载而出现，就应调整其他参数直到警告消失。
	参见：tcp_max_syn_backlog、tcp_synack_retries、tcp_abort_on_overflow。)

	syncookies seriously violate TCP protocol, do not allow
	to use TCP extensions, can result in serious degradation
	of some services (f.e. SMTP relaying), visible not by you,
	but your clients and relays, contacting you. While you see
	SYN flood warnings in logs not being really flooded, your server
	is seriously misconfigured.
	(syncookies 严重违反 TCP 协议，不允许使用 TCP 扩展，可能导致某些服务
	（例如 SMTP 中继）严重劣化——受影响的不是你，而是与你通信的客户端和
	中继。当你看到日志中的 SYN flood 警告而实际并未被洪水攻击时，说明
	服务器配置存在严重问题。)

	If you want to test which effects syncookies have to your
	network connections you can set this knob to 2 to enable
	unconditionally generation of syncookies.
	(如果想测试 syncookies 对网络连接的影响，可以把该开关设为 2，
	无条件地生成 syncookies。)

tcp_migrate_req - BOOLEAN
	The incoming connection is tied to a specific listening socket when
	the initial SYN packet is received during the three-way handshake.
	When a listener is closed, in-flight request sockets during the
	handshake and established sockets in the accept queue are aborted.
	(在三次握手期间收到初始 SYN 数据包时，入连接就绑定到特定的监听套接字。
	当监听者被关闭时，握手中尚未完成的请求套接字以及接受队列中已建立的
	套接字都会被中止。)

	If the listener has SO_REUSEPORT enabled, other listeners on the
	same port should have been able to accept such connections. This
	option makes it possible to migrate such child sockets to another
	listener after close() or shutdown().
	(如果监听者启用了 SO_REUSEPORT，同端口上的其他监听者本应能够接受这类
	连接。此选项使得在 close() 或 shutdown() 之后，可以把这类子套接字迁移
	到另一个监听者。)

	The BPF_SK_REUSEPORT_SELECT_OR_MIGRATE type of eBPF program should
	usually be used to define the policy to pick an alive listener.
	Otherwise, the kernel will randomly pick an alive listener only if
	this option is enabled.
	(通常应使用 BPF_SK_REUSEPORT_SELECT_OR_MIGRATE 类型的 eBPF 程序来定义
	挑选存活监听者的策略。否则，只有在该选项启用时，内核才会随机挑选一个
	存活的监听者。)

	Note that migration between listeners with different settings may
	crash applications. Let's say migration happens from listener A to
	B, and only B has TCP_SAVE_SYN enabled. B cannot read SYN data from
	the requests migrated from A. To avoid such a situation, cancel
	migration by returning SK_DROP in the type of eBPF program, or
	disable this option.
	(注意，在设置不同的监听者之间迁移可能导致应用崩溃。假设迁移从监听者 A
	发生到 B，而只有 B 启用了 TCP_SAVE_SYN，那么 B 无法读取从 A 迁移来的
	请求中的 SYN 数据。为避免这种情况，可在该类 eBPF 程序中返回 SK_DROP
	取消迁移，或禁用此选项。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_fastopen - INTEGER
	Enable TCP Fast Open (RFC7413) to send and accept data in the opening
	SYN packet.
	(启用 TCP Fast Open（RFC7413），在开头的 SYN 数据包中发送和接受数据。)

	The client support is enabled by flag 0x1 (on by default). The client
	then must use sendmsg() or sendto() with the MSG_FASTOPEN flag,
	rather than connect() to send data in SYN.
	(客户端支持由标志 0x1 启用（默认开启）。客户端随后必须使用带 MSG_FASTOPEN
	标志的 sendmsg() 或 sendto()，而不是 connect()，才能在 SYN 中发送数据。)

	The server support is enabled by flag 0x2 (off by default). Then
	either enable for all listeners with another flag (0x400) or
	enable individual listeners via TCP_FASTOPEN socket option with
	the option value being the length of the syn-data backlog.
	(服务器支持由标志 0x2 启用（默认关闭）。之后既可以用另一个标志（0x400）
	为所有监听者启用，也可以通过 TCP_FASTOPEN 套接字选项为个别监听者启用，
	选项值为 syn-data 积压队列的长度。)

	The values (bitmap) are
	(取值（位图）如下：)

	=====  ======== ======================================================
	  0x1  (client) enables sending data in the opening SYN on the client. (（客户端）允许客户端在开头的 SYN 中发送数据。)
	  0x2  (server) enables the server support, i.e., allowing data in
			a SYN packet to be accepted and passed to the
			application before 3-way handshake finishes. (（服务器）启用服务器支持，即在三次握手完成之前就允许接受 SYN 数据包中的数据并传递给应用。)
	  0x4  (client) send data in the opening SYN regardless of cookie
			availability and without a cookie option. (（客户端）无论 cookie 是否可用，都在开头的 SYN 中发送数据，且不带 cookie 选项。)
	0x200  (server) accept data-in-SYN w/o any cookie option present. (（服务器）在没有任何 cookie 选项时接受 SYN 携带的数据。)
	0x400  (server) enable all listeners to support Fast Open by
			default without explicit TCP_FASTOPEN socket option. (（服务器）让所有监听者默认支持 Fast Open，而无需显式设置 TCP_FASTOPEN 套接字选项。)
	=====  ======== ======================================================

	Default: 0x1
	(默认：0x1)

	Note that additional client or server features are only
	effective if the basic support (0x1 and 0x2) are enabled respectively.
	(注意，附加的客户端或服务器特性只有在基本支持（0x1 与 0x2）分别启用时
	才有效。)

tcp_fastopen_blackhole_timeout_sec - INTEGER
	Initial time period in second to disable Fastopen on active TCP sockets
	when a TFO firewall blackhole issue happens.
	This time period will grow exponentially when more blackhole issues
	get detected right after Fastopen is re-enabled and will reset to
	initial value when the blackhole issue goes away.
	0 to disable the blackhole detection.
	(当发生 TFO 防火墙黑洞问题时，初始的在多少秒内对活跃 TCP 套接字禁用
	Fastopen。若在重新启用 Fastopen 后立刻又检测到更多黑洞问题，该时长会
	按指数增长；当黑洞问题消失时重置为初始值。设为 0 可禁用黑洞检测。)

	By default, it is set to 0 (feature is disabled).
	(默认为 0（特性禁用）。)

tcp_fastopen_key - list of comma separated 32-digit hexadecimal INTEGERs
	The list consists of a primary key and an optional backup key. The
	primary key is used for both creating and validating cookies, while the
	optional backup key is only used for validating cookies. The purpose of
	the backup key is to maximize TFO validation when keys are rotated.
	(该列表由一个主键和一个可选的备用键组成。主键既用于创建也用于校验
	cookie，可选的备用键只用于校验 cookie。备用键的目的是在轮换密钥时
	尽量保证 TFO 校验成功。)

	A randomly chosen primary key may be configured by the kernel if
	the tcp_fastopen sysctl is set to 0x400 (see above), or if the
	TCP_FASTOPEN setsockopt() optname is set and a key has not been
	previously configured via sysctl. If keys are configured via
	setsockopt() by using the TCP_FASTOPEN_KEY optname, then those
	per-socket keys will be used instead of any keys that are specified via
	sysctl.
	(如果 tcp_fastopen sysctl 设为 0x400（见上文），或者设置了 TCP_FASTOPEN
	setsockopt() optname 且此前未通过 sysctl 配置过密钥，内核可能配置一个
	随机选择的主键。如果通过使用 TCP_FASTOPEN_KEY optname 的 setsockopt()
	配置密钥，则将使用这些每套接字密钥，而不是通过 sysctl 指定的任何密钥。)

	A key is specified as 4 8-digit hexadecimal integers which are separated
	by a '-' as: xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx. Leading zeros may be
	omitted. A primary and a backup key may be specified by separating them
	by a comma. If only one key is specified, it becomes the primary key and
	any previously configured backup keys are removed.
	(密钥由 4 个 8 位十六进制整数表示，以 '-' 分隔，形如
	xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx。前导零可以省略。主键与备用键之间
	用逗号分隔。若只指定一个密钥，它就成为主键，且先前配置的备用键会被
	移除。)

tcp_syn_retries - INTEGER
	Number of times initial SYNs for an active TCP connection attempt
	will be retransmitted. Should not be higher than 127. Default value
	is 6, which corresponds to 67seconds (with tcp_syn_linear_timeouts = 4)
	till the last retransmission with the current initial RTO of 1second.
	With this the final timeout for an active TCP connection attempt
	will happen after 131seconds.
	(主动 TCP 连接尝试的初始 SYN 重传次数。不应高于 127。默认值为 6，在
	当前初始 RTO 为 1 秒、tcp_syn_linear_timeouts = 4 的情况下，对应到最后
	一次重传为止共 67 秒。据此，主动 TCP 连接尝试的最终超时将发生在
	131 秒之后。)

tcp_timestamps - INTEGER
	Enable timestamps as defined in RFC1323.
	(启用 RFC1323 定义的时间戳。)

	- 0: Disabled.
	  (0：禁用。)
	- 1: Enable timestamps as defined in RFC1323 and use random offset for
	  each connection rather than only using the current time.
	  (1：启用 RFC1323 定义的时间戳，并为每个连接使用随机偏移，而不只是使用当前时间。)
	- 2: Like 1, but without random offsets.
	  (2：与 1 类似，但不使用随机偏移。)

	Default: 1
	(默认：1)

tcp_min_tso_segs - INTEGER
	Minimal number of segments per TSO frame.
	(每个 TSO 帧的最小分段数。)

	Since linux-3.12, TCP does an automatic sizing of TSO frames,
	depending on flow rate, instead of filling 64Kbytes packets.
	For specific usages, it's possible to force TCP to build big
	TSO frames. Note that TCP stack might split too big TSO packets
	if available window is too small.
	(自 linux-3.12 起，TCP 根据流速率对 TSO 帧进行自动尺寸调整，而不是填满
	64KB 的数据包。对于特定用途，可以强制 TCP 构建大的 TSO 帧。注意，如果
	可用窗口太小，TCP 协议栈可能会拆分过大的 TSO 数据包。)

	Default: 2
	(默认：2)

tcp_tso_rtt_log - INTEGER
	Adjustment of TSO packet sizes based on min_rtt
	(基于 min_rtt 调整 TSO 数据包大小)

	Starting from linux-5.18, TCP autosizing can be tweaked
	for flows having small RTT.
	(从 linux-5.18 开始，对于 RTT 较小的流可以微调 TCP 自动尺寸调整。)

	Old autosizing was splitting the pacing budget to send 1024 TSO
	per second.
	(旧的自动尺寸调整把 pacing 预算拆分为每秒发送 1024 个 TSO。)

	tso_packet_size = sk->sk_pacing_rate / 1024;

	With the new mechanism, we increase this TSO sizing using:
	(在新机制下，我们用以下方式增大 TSO 尺寸：)

	distance = min_rtt_usec / (2^tcp_tso_rtt_log)
	tso_packet_size += gso_max_size >> distance;

	This means that flows between very close hosts can use bigger
	TSO packets, reducing their cpu costs.
	(这意味着距离非常近的主机之间的流可以使用更大的 TSO 数据包，
	降低它们的 CPU 开销。)

	If you want to use the old autosizing, set this sysctl to 0.
	(如果想使用旧的自动尺寸调整，把该 sysctl 设为 0。)

	Default: 9  (2^9 = 512 usec)
	(默认：9（2^9 = 512 微秒）)

tcp_pacing_ss_ratio - INTEGER
	sk->sk_pacing_rate is set by TCP stack using a ratio applied
	to current rate. (current_rate = cwnd * mss / srtt)
	If TCP is in slow start, tcp_pacing_ss_ratio is applied
	to let TCP probe for bigger speeds, assuming cwnd can be
	doubled every other RTT.
	(TCP 协议栈用一个施加于当前速率的比率来设置 sk->sk_pacing_rate。
	(current_rate = cwnd * mss / srtt) 如果 TCP 处于慢启动阶段，则应用
	tcp_pacing_ss_ratio，让 TCP 探测更高的速率，假设 cwnd 每隔一个 RTT
	可以翻倍。)

	Default: 200
	(默认：200)

tcp_pacing_ca_ratio - INTEGER
	sk->sk_pacing_rate is set by TCP stack using a ratio applied
	to current rate. (current_rate = cwnd * mss / srtt)
	If TCP is in congestion avoidance phase, tcp_pacing_ca_ratio
	is applied to conservatively probe for bigger throughput.
	(TCP 协议栈用一个施加于当前速率的比率来设置 sk->sk_pacing_rate。
	(current_rate = cwnd * mss / srtt) 如果 TCP 处于拥塞避免阶段，则应用
	tcp_pacing_ca_ratio，以保守方式探测更大的吞吐。)

	Default: 120
	(默认：120)

tcp_syn_linear_timeouts - INTEGER
	The number of times for an active TCP connection to retransmit SYNs with
	a linear backoff timeout before defaulting to an exponential backoff
	timeout. This has no effect on SYNACK at the passive TCP side.
	(主动 TCP 连接在回退到指数退避超时之前，以线性退避超时重传 SYN 的次数。
	这对被动 TCP 一侧的 SYNACK 没有影响。)

	With an initial RTO of 1 and tcp_syn_linear_timeouts = 4 we would
	expect SYN RTOs to be: 1, 1, 1, 1, 1, 2, 4, ... (4 linear timeouts,
	and the first exponential backoff using 2^0 * initial_RTO).
	Default: 4
	(初始 RTO 为 1 且 tcp_syn_linear_timeouts = 4 时，SYN RTO 序列为：
	1, 1, 1, 1, 1, 2, 4, ...（4 次线性超时，之后第一次指数退避使用
	2^0 * initial_RTO）。默认：4)

tcp_tso_win_divisor - INTEGER
	This allows control over what percentage of the congestion window
	can be consumed by a single TSO frame.
	The setting of this parameter is a choice between burstiness and
	building larger TSO frames.
	(此参数控制单个 TSO 帧可以占用拥塞窗口的百分比。设置该参数是在突发性
	与构建更大的 TSO 帧之间做取舍。)

	Default: 3
	(默认：3)

tcp_tw_reuse - INTEGER
	Enable reuse of TIME-WAIT sockets for new connections when it is
	safe from protocol viewpoint.
	(在协议角度安全的前提下，允许新连接复用 TIME-WAIT 套接字。)

	- 0 - disable
	  (0 - 禁用)
	- 1 - global enable
	  (1 - 全局启用)
	- 2 - enable for loopback traffic only
	  (2 - 仅对环回流量启用)

	It should not be changed without advice/request of technical
	experts.
	(未经技术专家建议/要求，不应修改此值。)

	Default: 2
	(默认：2)

tcp_tw_reuse_delay - UNSIGNED INTEGER
        The delay in milliseconds before a TIME-WAIT socket can be reused by a
        new connection, if TIME-WAIT socket reuse is enabled. The actual reuse
        threshold is within [N, N+1] range, where N is the requested delay in
        milliseconds, to ensure the delay interval is never shorter than the
        configured value.
        (若启用了 TIME-WAIT 套接字复用，此为 TIME-WAIT 套接字被新连接复用之前
        的延迟，单位为毫秒。实际复用阈值位于 [N, N+1] 区间内，其中 N 是所请求
        的毫秒延迟，以确保延迟间隔不会短于配置值。)

        This setting contains an assumption about the other TCP timestamp clock
        tick interval. It should not be set to a value lower than the peer's
        clock tick for PAWS (Protection Against Wrapped Sequence numbers)
        mechanism work correctly for the reused connection.
        (此设置包含对对端 TCP 时间戳时钟滴答间隔的假设。为使 PAWS（防止序列号
        回绕）机制对复用的连接正常工作，不应把该值设置得低于对端的时钟滴答
        间隔。)

        Default: 1000 (milliseconds)
        (默认：1000（毫秒）)

tcp_window_scaling - BOOLEAN
	Enable window scaling as defined in RFC1323.
	(启用 RFC1323 定义的窗口缩放。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

tcp_shrink_window - BOOLEAN
	This changes how the TCP receive window is calculated.
	(这会改变 TCP 接收窗口的计算方式。)

	RFC 7323, section 2.4, says there are instances when a retracted
	window can be offered, and that TCP implementations MUST ensure
	that they handle a shrinking window, as specified in RFC 1122.
	(RFC 7323 第 2.4 节指出，有些场景下可以提供回缩的窗口，并且 TCP 实现
	必须确保按照 RFC 1122 的规定处理收缩的窗口。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - The window is never shrunk.
	  (0（禁用）——窗口从不收缩。)
	- 1 (enabled)  - The window is shrunk when necessary to remain within
	  the memory limit set by autotuning (sk_rcvbuf).
	  This only occurs if a non-zero receive window
	  scaling factor is also in effect.
	  (1（启用）——必要时收缩窗口以保持在自动调优设置的内存上限（sk_rcvbuf）
	  之内。仅当同时存在非零的接收窗口缩放因子时才会发生。)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_wmem - vector of 3 INTEGERs: min, default, max
	min: Amount of memory reserved for send buffers for TCP sockets.
	Each TCP socket has rights to use it due to fact of its birth.
	(min：为 TCP 套接字发送缓冲保留的内存量。每个 TCP 套接字生来就有权使用它。)

	Default: 4K
	(默认：4K)

	default: initial size of send buffer used by TCP sockets.  This
	value overrides net.core.wmem_default used by other protocols.
	(default：TCP 套接字发送缓冲的初始大小。该值覆盖其他协议使用的
	net.core.wmem_default。)

	It is usually lower than net.core.wmem_default.
	(它通常低于 net.core.wmem_default。)

	Default: 16K
	(默认：16K)

	max: Maximal amount of memory allowed for automatically tuned
	send buffers for TCP sockets. This value does not override
	net.core.wmem_max.  Calling setsockopt() with SO_SNDBUF disables
	automatic tuning of that socket's send buffer size, in which case
	this value is ignored.
	(max：TCP 套接字自动调优的发送缓冲所允许的最大内存量。该值不覆盖
	net.core.wmem_max。以 SO_SNDBUF 调用 setsockopt() 会禁用该套接字发送
	缓冲大小的自动调优，此时忽略此值。)

	Default: between 64K and 4MB, depending on RAM size.
	(默认：介于 64K 与 4MB 之间，取决于内存大小。)

tcp_notsent_lowat - UNSIGNED INTEGER
	A TCP socket can control the amount of unsent bytes in its write queue,
	thanks to TCP_NOTSENT_LOWAT socket option. poll()/select()/epoll()
	reports POLLOUT events if the amount of unsent bytes is below a per
	socket value, and if the write queue is not full. sendmsg() will
	also not add new buffers if the limit is hit.
	(借助 TCP_NOTSENT_LOWAT 套接字选项，TCP 套接字可以控制其写队列中未发送
	字节的量。当未发送字节数低于每套接字设定值且写队列未满时，
	poll()/select()/epoll() 才报告 POLLOUT 事件。达到限额时 sendmsg() 也
	不会再添加新的缓冲。)

	This global variable controls the amount of unsent data for
	sockets not using TCP_NOTSENT_LOWAT. For these sockets, a change
	to the global variable has immediate effect.
	(此全局变量控制未使用 TCP_NOTSENT_LOWAT 的套接字的未发送数据量。
	对这些套接字而言，修改该全局变量会立即生效。)

	Default: UINT_MAX (0xFFFFFFFF)
	(默认：UINT_MAX (0xFFFFFFFF))

tcp_workaround_signed_windows - BOOLEAN
	If enabled, assume no receipt of a window scaling option means the
	remote TCP is broken and treats the window as a signed quantity.
	If disabled, assume the remote TCP is not broken even if we do
	not receive a window scaling option from them.
	(若启用，则假定未收到窗口缩放选项意味着对端 TCP 有缺陷，并把窗口当作
	有符号量处理。若禁用，则即使未从对端收到窗口缩放选项，也假定对端 TCP
	没有缺陷。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_thin_linear_timeouts - BOOLEAN
	Enable dynamic triggering of linear timeouts for thin streams.
	If enabled, a check is performed upon retransmission by timeout to
	determine if the stream is thin (less than 4 packets in flight).
	As long as the stream is found to be thin, up to 6 linear
	timeouts may be performed before exponential backoff mode is
	initiated. This improves retransmission latency for
	non-aggressive thin streams, often found to be time-dependent.
	For more information on thin streams, see
	Documentation/networking/tcp-thin.rst
	(为瘦流（thin streams）启用线性超时的动态触发。若启用，每次因超时而
	重传时会检查该流是否为瘦流（在途数据包少于 4 个）。只要发现流是瘦流的，
	最多可执行 6 次线性超时才进入指数退避模式。这改善了非激进型瘦流的
	重传延迟——这类流往往对时间敏感。关于瘦流的更多信息参见
	Documentation/networking/tcp-thin.rst)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_limit_output_bytes - INTEGER
	Controls TCP Small Queue limit per tcp socket.
	TCP bulk sender tends to increase packets in flight until it
	gets losses notifications. With SNDBUF autotuning, this can
	result in a large amount of packets queued on the local machine
	(e.g.: qdiscs, CPU backlog, or device) hurting latency of other
	flows, for typical pfifo_fast qdiscs.  tcp_limit_output_bytes
	limits the number of bytes on qdisc or device to reduce artificial
	RTT/cwnd and reduce bufferbloat.
	(控制每个 TCP 套接字的 TCP Small Queue 限额。
	TCP 批量发送者倾向于不断增加在途数据包，直到收到丢包通知。在 SNDBUF
	自动调优下，对于典型的 pfifo_fast qdisc，这可能造成本机排队的大量数据包
	（例如 qdisc、CPU 积压或设备），损害其他流的延迟。tcp_limit_output_bytes
	限制 qdisc 或设备上的字节数，以减少人为抬高的 RTT/cwnd 并缓解
	bufferbloat。)

	Default: 4194304 (4 MB)
	(默认：4194304（4 MB）)

tcp_challenge_ack_limit - INTEGER
	Limits number of Challenge ACK sent per second, as recommended
	in RFC 5961 (Improving TCP's Robustness to Blind In-Window Attacks)
	Note that this per netns rate limit can allow some side channel
	attacks and probably should not be enabled.
	TCP stack implements per TCP socket limits anyway.
	Default: INT_MAX (unlimited)
	(限制每秒发送的 Challenge ACK 数量，如 RFC 5961（Improving TCP's
	Robustness to Blind In-Window Attacks）所建议。注意，这种按 netns 的
	限速可能允许某些侧信道攻击，或许不应启用。无论如何，TCP 协议栈本身
	已实现每 TCP 套接字的限制。默认：INT_MAX（不限制）)

tcp_ehash_entries - INTEGER
	Show the number of hash buckets for TCP sockets in the current
	networking namespace.
	(显示当前网络命名空间中 TCP 套接字哈希桶的数量。)

	A negative value means the networking namespace does not own its
	hash buckets and shares the initial networking namespace's one.
	(负值表示该网络命名空间不拥有自己的哈希桶，而是共享初始网络命名空间
	的哈希桶。)

tcp_child_ehash_entries - INTEGER
	Control the number of hash buckets for TCP sockets in the child
	networking namespace, which must be set before clone() or unshare().
	(控制子网络命名空间中 TCP 套接字哈希桶的数量，必须在 clone() 或
	unshare() 之前设置。)

	If the value is not 0, the kernel uses a value rounded up to 2^n
	as the actual hash bucket size.  0 is a special value, meaning
	the child networking namespace will share the initial networking
	namespace's hash buckets.
	(若该值不为 0，内核使用向上取整到 2^n 的值作为实际哈希桶大小。0 是
	特殊值，表示子网络命名空间将共享初始网络命名空间的哈希桶。)

	Note that the child will use the global one in case the kernel
	fails to allocate enough memory.  In addition, the global hash
	buckets are spread over available NUMA nodes, but the allocation
	of the child hash table depends on the current process's NUMA
	policy, which could result in performance differences.
	(注意，若内核无法分配足够内存，子命名空间将使用全局的哈希桶。此外，
	全局哈希桶分布在可用的 NUMA 节点上，而子哈希表的分配取决于当前进程的
	NUMA 策略，这可能造成性能差异。)

	Note also that the default value of tcp_max_tw_buckets and
	tcp_max_syn_backlog depend on the hash bucket size.
	(还要注意，tcp_max_tw_buckets 与 tcp_max_syn_backlog 的默认值取决于
	哈希桶大小。)

	Possible values: 0, 2^n (n: 0 - 24 (16Mi))
	(可能的取值：0、2^n（n：0 - 24（16Mi））)

	Default: 0
	(默认：0)

tcp_plb_enabled - BOOLEAN
	If enabled and the underlying congestion control (e.g. DCTCP) supports
	and enables PLB feature, TCP PLB (Protective Load Balancing) is
	enabled. PLB is described in the following paper:
	https://doi.org/10.1145/3544216.3544226. Based on PLB parameters,
	upon sensing sustained congestion, TCP triggers a change in
	flow label field for outgoing IPv6 packets. A change in flow label
	field potentially changes the path of outgoing packets for switches
	that use ECMP/WCMP for routing.
	(若启用且底层拥塞控制（例如 DCTCP）支持并启用 PLB 特性，则 TCP PLB
	（保护性负载均衡，Protective Load Balancing）被启用。PLB 在以下论文中
	描述：https://doi.org/10.1145/3544216.3544226。基于 PLB 参数，在感知到
	持续拥塞时，TCP 会触发对发出的 IPv6 数据包流标签字段的变更。流标签
	字段的变更可能改变交换机使用 ECMP/WCMP 路由时出站数据包的路径。)

	PLB changes socket txhash which results in a change in IPv6 Flow Label
	field, and currently no-op for IPv4 headers. It is possible
	to apply PLB for IPv4 with other network header fields (e.g. TCP
	or IPv4 options) or using encapsulation where outer header is used
	by switches to determine next hop. In either case, further host
	and switch side changes will be needed.
	(PLB 改变套接字的 txhash，从而改变 IPv6 流标签字段，目前对 IPv4 头部
	不起作用。可以把 PLB 应用于 IPv4 的其他网络头部字段（例如 TCP 或 IPv4
	选项），或使用外层头部供交换机确定下一跳的封装方式。无论哪种情况，
	都需要主机侧和交换机侧的进一步改动。)

	If enabled, PLB assumes that congestion signal (e.g. ECN) is made
	available and used by congestion control module to estimate a
	congestion measure (e.g. ce_ratio). PLB needs a congestion measure to
	make repathing decisions.
	(若启用，PLB 假定拥塞信号（例如 ECN）可用，并被拥塞控制模块用于估算
	拥塞度量（例如 ce_ratio）。PLB 需要拥塞度量来做出重新选路决策。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

tcp_plb_idle_rehash_rounds - INTEGER
	Number of consecutive congested rounds (RTT) seen after which
	a rehash can be performed, given there are no packets in flight.
	This is referred to as M in PLB paper:
	https://doi.org/10.1145/3544216.3544226.
	(在没有任何在途数据包的前提下，观察到连续拥塞轮次（RTT）达到该数量后
	才可以执行 rehash。即 PLB 论文中的 M：
	https://doi.org/10.1145/3544216.3544226。)

	Possible Values: 0 - 31
	(可能的取值：0 - 31)

	Default: 3
	(默认：3)

tcp_plb_rehash_rounds - INTEGER
	Number of consecutive congested rounds (RTT) seen after which
	a forced rehash can be performed. Be careful when setting this
	parameter, as a small value increases the risk of retransmissions.
	This is referred to as N in PLB paper:
	https://doi.org/10.1145/3544216.3544226.
	(观察到连续拥塞轮次（RTT）达到该数量后可以执行强制 rehash。设置此参数
	时要小心，较小的值会增加重传风险。即 PLB 论文中的 N：
	https://doi.org/10.1145/3544216.3544226。)

	Possible Values: 0 - 31
	(可能的取值：0 - 31)

	Default: 12
	(默认：12)

tcp_plb_suspend_rto_sec - INTEGER
	Time, in seconds, to suspend PLB in event of an RTO. In order to avoid
	having PLB repath onto a connectivity "black hole", after an RTO a TCP
	connection suspends PLB repathing for a random duration between 1x and
	2x of this parameter. Randomness is added to avoid concurrent rehashing
	of multiple TCP connections. This should be set corresponding to the
	amount of time it takes to repair a failed link.
	(发生 RTO 时暂停 PLB 的时间，单位为秒。为避免 PLB 重新选路到连通性
	"黑洞"，RTO 之后 TCP 连接会在该参数 1 倍到 2 倍之间的随机时长内暂停
	PLB 重新选路。加入随机性是为了避免多条 TCP 连接同时 rehash。此值应
	设置为与修复失效链路所需时间相当。)

	Possible Values: 0 - 255
	(可能的取值：0 - 255)

	Default: 60
	(默认：60)

tcp_plb_cong_thresh - INTEGER
	Fraction of packets marked with congestion over a round (RTT) to
	tag that round as congested. This is referred to as K in the PLB paper:
	https://doi.org/10.1145/3544216.3544226.
	(一轮（RTT）内被标记为拥塞的数据包比例达到该值时，该轮被标记为拥塞。
	即 PLB 论文中的 K：https://doi.org/10.1145/3544216.3544226。)

	The 0-1 fraction range is mapped to 0-256 range to avoid floating
	point operations. For example, 128 means that if at least 50% of
	the packets in a round were marked as congested then the round
	will be tagged as congested.
	(0-1 的比例范围被映射到 0-256 的范围以避免浮点运算。例如，128 表示
	一轮内至少 50% 的数据包被标记为拥塞时，该轮将被标记为拥塞。)

	Setting threshold to 0 means that PLB repaths every RTT regardless
	of congestion. This is not intended behavior for PLB and should be
	used only for experimentation purpose.
	(把阈值设为 0 意味着 PLB 无论是否拥塞每个 RTT 都重新选路。这不是 PLB
	的预期行为，只应用于实验目的。)

	Possible Values: 0 - 256
	(可能的取值：0 - 256)

	Default: 128
	(默认：128)

tcp_pingpong_thresh - INTEGER
	The number of estimated data replies sent for estimated incoming data
	requests that must happen before TCP considers that a connection is a
	"ping-pong" (request-response) connection for which delayed
	acknowledgments can provide benefits.
	(在 TCP 认定一个连接是"乒乓"（请求-响应）连接——从而延迟确认能带来
	好处——之前，估算的入站数据请求必须已引发相应数量的估算数据回复。)

	This threshold is 1 by default, but some applications may need a higher
	threshold for optimal performance.
	(该阈值默认为 1，但某些应用可能需要更高的阈值才能获得最优性能。)

	Possible Values: 1 - 255
	(可能的取值：1 - 255)

	Default: 1
	(默认：1)

tcp_rto_min_us - INTEGER
	Minimal TCP retransmission timeout (in microseconds). Note that the
	rto_min route option has the highest precedence for configuring this
	setting, followed by the TCP_BPF_RTO_MIN and TCP_RTO_MIN_US socket
	options, followed by this tcp_rto_min_us sysctl.
	(最小 TCP 重传超时（微秒）。注意，配置此设置时 rto_min 路由选项优先级
	最高，其次是 TCP_BPF_RTO_MIN 与 TCP_RTO_MIN_US 套接字选项，最后才是
	这个 tcp_rto_min_us sysctl。)

	The recommended practice is to use a value less or equal to 200000
	microseconds.
	(建议使用的值不大于 200000 微秒。)

	Possible Values: 1 - INT_MAX
	(可能的取值：1 - INT_MAX)

	Default: 200000
	(默认：200000)

tcp_rto_max_ms - INTEGER
	Maximal TCP retransmission timeout (in ms).
	Note that TCP_RTO_MAX_MS socket option has higher precedence.
	(最大 TCP 重传超时（毫秒）。注意，TCP_RTO_MAX_MS 套接字选项优先级更高。)

	When changing tcp_rto_max_ms, it is important to understand
	that tcp_retries2 might need a change.
	(修改 tcp_rto_max_ms 时，重要的是要理解 tcp_retries2 可能也需要调整。)

	Possible Values: 1000 - 120,000
	(可能的取值：1000 - 120,000)

	Default: 120,000
	(默认：120,000)

UDP variables(UDP 变量)
=======================

udp_l3mdev_accept - BOOLEAN
	Enabling this option allows a "global" bound socket to work
	across L3 master domains (e.g., VRFs) with packets capable of
	being received regardless of the L3 domain in which they
	originated. Only valid when the kernel was compiled with
	CONFIG_NET_L3_MASTER_DEV.
	(启用此选项后，"全局"绑定套接字可以跨 L3 主域（例如 VRF）工作，无论
	数据包起源于哪个 L3 域都可以接收。仅在内核编译时启用
	CONFIG_NET_L3_MASTER_DEV 时有效。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

udp_mem - vector of 3 INTEGERs: min, pressure, max
	Number of pages allowed for queueing by all UDP sockets.
	(允许所有 UDP 套接字用于排队的页数。)

	min: Number of pages allowed for queueing by all UDP sockets.
	(min：允许所有 UDP 套接字用于排队的页数。)

	pressure: This value was introduced to follow format of tcp_mem.
	(pressure：引入此值只是为了与 tcp_mem 的格式保持一致。)

	max: This value was introduced to follow format of tcp_mem.
	(max：引入此值只是为了与 tcp_mem 的格式保持一致。)

	Default is calculated at boot time from amount of available memory.
	(默认值在引导时根据可用内存量计算。)

udp_rmem_min - INTEGER
	Minimal size of receive buffer used by UDP sockets in moderation.
	Each UDP socket is able to use the size for receiving data, even if
	total pages of UDP sockets exceed udp_mem pressure. The unit is byte.
	(UDP 套接字适度使用的接收缓冲最小大小。即使所有 UDP 套接字的总页数
	超过 udp_mem 的 pressure 值，每个 UDP 套接字也能使用该大小接收数据。
	单位为字节。)

	Default: 4K
	(默认：4K)

udp_wmem_min - INTEGER
	UDP does not have tx memory accounting and this tunable has no effect.
	(UDP 没有发送内存记账，此可调参数没有任何作用。)

udp_hash_entries - INTEGER
	Show the number of hash buckets for UDP sockets in the current
	networking namespace.
	(显示当前网络命名空间中 UDP 套接字哈希桶的数量。)

	A negative value means the networking namespace does not own its
	hash buckets and shares the initial networking namespace's one.
	(负值表示该网络命名空间不拥有自己的哈希桶，而是共享初始网络命名空间
	的哈希桶。)

udp_child_hash_entries - INTEGER
	Control the number of hash buckets for UDP sockets in the child
	networking namespace, which must be set before clone() or unshare().
	(控制子网络命名空间中 UDP 套接字哈希桶的数量，必须在 clone() 或
	unshare() 之前设置。)

	If the value is not 0, the kernel uses a value rounded up to 2^n
	as the actual hash bucket size.  0 is a special value, meaning
	the child networking namespace will share the initial networking
	namespace's hash buckets.
	(若该值不为 0，内核使用向上取整到 2^n 的值作为实际哈希桶大小。0 是
	特殊值，表示子网络命名空间将共享初始网络命名空间的哈希桶。)

	Note that the child will use the global one in case the kernel
	fails to allocate enough memory.  In addition, the global hash
	buckets are spread over available NUMA nodes, but the allocation
	of the child hash table depends on the current process's NUMA
	policy, which could result in performance differences.
	(注意，若内核无法分配足够内存，子命名空间将使用全局的哈希桶。此外，
	全局哈希桶分布在可用的 NUMA 节点上，而子哈希表的分配取决于当前进程的
	NUMA 策略，这可能造成性能差异。)

	Possible values: 0, 2^n (n: 7 (128) - 16 (64K))
	(可能的取值：0、2^n（n：7（128）- 16（64K））)

	Default: 0
	(默认：0)


RAW variables(RAW 变量)
=======================

raw_l3mdev_accept - BOOLEAN
	Enabling this option allows a "global" bound socket to work
	across L3 master domains (e.g., VRFs) with packets capable of
	being received regardless of the L3 domain in which they
	originated. Only valid when the kernel was compiled with
	CONFIG_NET_L3_MASTER_DEV.
	(启用此选项后，"全局"绑定套接字可以跨 L3 主域（例如 VRF）工作，无论
	数据包起源于哪个 L3 域都可以接收。仅在内核编译时启用
	CONFIG_NET_L3_MASTER_DEV 时有效。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

CIPSOv4 Variables(CIPSOv4 变量)
===============================

cipso_cache_enable - BOOLEAN
	If enabled, enable additions to and lookups from the CIPSO label mapping
	cache.  If disabled, additions are ignored and lookups always result in a
	miss.  However, regardless of the setting the cache is still
	invalidated when required when means you can safely toggle this on and
	off and the cache will always be "safe".
	(若启用，允许向 CIPSO 标签映射缓存添加条目以及从中查找。若禁用，添加
	操作被忽略，查找总是未命中。不过，无论设置如何，缓存仍会在需要时失效，
	这意味着可以放心地开关此选项，缓存始终是"安全"的。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

cipso_cache_bucket_size - INTEGER
	The CIPSO label cache consists of a fixed size hash table with each
	hash bucket containing a number of cache entries.  This variable limits
	the number of entries in each hash bucket; the larger the value is, the
	more CIPSO label mappings that can be cached.  When the number of
	entries in a given hash bucket reaches this limit adding new entries
	causes the oldest entry in the bucket to be removed to make room.
	(CIPSO 标签缓存由一个固定大小的哈希表组成，每个哈希桶包含若干缓存
	条目。此变量限制每个哈希桶中的条目数；值越大，可缓存的 CIPSO 标签
	映射越多。当某个哈希桶的条目数达到该限制时，添加新条目会导致桶中
	最旧的条目被移除以腾出空间。)

	Default: 10
	(默认：10)

cipso_rbm_optfmt - BOOLEAN
	Enable the "Optimized Tag 1 Format" as defined in section 3.4.2.6 of
	the CIPSO draft specification (see Documentation/netlabel for details).
	This means that when set the CIPSO tag will be padded with empty
	categories in order to make the packet data 32-bit aligned.
	(启用 CIPSO 草案规范第 3.4.2.6 节定义的"Optimized Tag 1 Format"
	（详情见 Documentation/netlabel）。即启用后，CIPSO 标签会用空类别填充，
	使数据包数据 32 位对齐。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

cipso_rbm_strictvalid - BOOLEAN
	If enabled, do a very strict check of the CIPSO option when
	ip_options_compile() is called.  If disabled, relax the checks done during
	ip_options_compile().  Either way is "safe" as errors are caught else
	where in the CIPSO processing code but setting this to 0 (False) should
	result in less work (i.e. it should be faster) but could cause problems
	with other implementations that require strict checking.
	(若启用，在调用 ip_options_compile() 时对 CIPSO 选项进行非常严格的检查。
	若禁用，则放宽 ip_options_compile() 期间的检查。两种方式都是"安全"的，
	因为错误会在 CIPSO 处理代码的其他地方被捕获，但设为 0（False）应能减少
	工作量（即应该更快），不过可能导致与要求严格检查的其他实现不兼容。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

IP Variables(IP 变量)
=====================

ip_local_port_range - 2 INTEGERS
	Defines the local port range that is used by TCP and UDP to
	choose the local port. The first number is the first, the
	second the last local port number.
	If possible, it is better these numbers have different parity
	(one even and one odd value).
	Must be greater than or equal to ip_unprivileged_port_start.
	The default values are 32768 and 60999 respectively.
	(定义 TCP 和 UDP 选择本地端口时使用的本地端口范围。第一个数字是起始
	端口，第二个数字是最后一个本地端口号。如果可能，这两个数字最好具有
	不同的奇偶性（一个偶数一个奇数）。必须大于或等于
	ip_unprivileged_port_start。默认值分别为 32768 和 60999。)

ip_local_reserved_ports - list of comma separated ranges
	Specify the ports which are reserved for known third-party
	applications. These ports will not be used by automatic port
	assignments (e.g. when calling connect() or bind() with port
	number 0). Explicit port allocation behavior is unchanged.
	(为已知第三方应用指定保留的端口。这些端口不会被自动端口分配使用
	（例如以端口 0 调用 connect() 或 bind() 时）。显式端口分配的行为
	保持不变。)

	The format used for both input and output is a comma separated
	list of ranges (e.g. "1,2-4,10-10" for ports 1, 2, 3, 4 and
	10). Writing to the file will clear all previously reserved
	ports and update the current list with the one given in the
	input.
	(输入与输出使用的格式都是以逗号分隔的范围列表（例如 "1,2-4,10-10"
	表示端口 1、2、3、4 和 10）。写入该文件会清除所有先前保留的端口，
	并用输入给定的列表更新当前列表。)

	Note that ip_local_port_range and ip_local_reserved_ports
	settings are independent and both are considered by the kernel
	when determining which ports are available for automatic port
	assignments.
	(注意，ip_local_port_range 与 ip_local_reserved_ports 的设置相互独立，
	内核在确定哪些端口可用于自动端口分配时会同时考虑两者。)

	You can reserve ports which are not in the current
	ip_local_port_range, e.g.::
	(也可以保留不在当前 ip_local_port_range 范围内的端口，例如：)

	    $ cat /proc/sys/net/ipv4/ip_local_port_range
	    32000	60999
	    $ cat /proc/sys/net/ipv4/ip_local_reserved_ports
	    8080,9148

	although this is redundant. However such a setting is useful
	if later the port range is changed to a value that will
	include the reserved ports. Also keep in mind, that overlapping
	of these ranges may affect probability of selecting ephemeral
	ports which are right after block of reserved ports.
	(尽管这样做是多余的。但如果之后端口范围被改为会包含这些保留端口的
	值，这样的设置就有用了。另外请记住，这些范围相互重叠可能影响选择
	紧随保留端口块之后的临时端口的概率。)

	Default: Empty
	(默认：空)

ip_local_port_step_width - INTEGER
        Defines the numerical maximum increment between successive port
        allocations within the ephemeral port range when an unavailable port is
        reached. This can be used to mitigate accumulated nodes in port
        distribution when reserved ports have been configured. Please note that
        port collisions may be more frequent in a system with a very high load.
        (定义在临时端口范围内遇到不可用端口时，连续两次端口分配之间允许的
        数字最大增量。在配置了保留端口时，可用于缓解端口分布中节点累积的
        问题。请注意，负载非常高的系统中端口冲突可能更频繁。)

        It is recommended to set this value strictly larger than the largest
        contiguous block of ports configure in ip_local_reserved_ports. For
        large reserved port ranges, setting this to 3x or 4x the size of the
        largest block is advised. Using a value equal or greater than the local
        port range size completely solves the uneven port distribution problem,
        but it can degrade performance under port exhaustion situations.
        (建议把该值设置为严格大于 ip_local_reserved_ports 中配置的最大连续
        端口块。对于较大的保留端口范围，建议设为最大端口块的 3 倍或 4 倍。
        若取值大于等于本地端口范围的大小，可以彻底解决端口分布不均的问题，
        但在端口耗尽情形下可能降低性能。)

        Default: 0 (disabled)
        (默认：0（禁用）)

ip_unprivileged_port_start - INTEGER
	This is a per-namespace sysctl.  It defines the first
	unprivileged port in the network namespace.  Privileged ports
	require root or CAP_NET_BIND_SERVICE in order to bind to them.
	To disable all privileged ports, set this to 0.  They must not
	overlap with the ip_local_port_range.
	(这是一个按命名空间生效的 sysctl。它定义网络命名空间中第一个非特权
	端口。绑定特权端口需要 root 或 CAP_NET_BIND_SERVICE。要禁用所有特权
	端口，把它设为 0。这些特权端口不得与 ip_local_port_range 重叠。)

	Default: 1024
	(默认：1024)

ip_nonlocal_bind - BOOLEAN
	If enabled, allows processes to bind() to non-local IP addresses,
	which can be quite useful - but may break some applications.
	(若启用，允许进程 bind() 到非本地 IP 地址，这可能相当有用——但也可能
	破坏某些应用。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

ip_autobind_reuse - BOOLEAN
	By default, bind() does not select the ports automatically even if
	the new socket and all sockets bound to the port have SO_REUSEADDR.
	ip_autobind_reuse allows bind() to reuse the port and this is useful
	when you use bind()+connect(), but may break some applications.
	The preferred solution is to use IP_BIND_ADDRESS_NO_PORT and this
	option should only be set by experts.
	(默认情况下，即使新套接字和绑定到该端口的所有套接字都有 SO_REUSEADDR，
	bind() 也不会自动选择端口。ip_autobind_reuse 允许 bind() 复用该端口，
	这在同时使用 bind()+connect() 时很有用，但可能破坏某些应用。首选方案
	是使用 IP_BIND_ADDRESS_NO_PORT，此选项只应由专家设置。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

ip_dynaddr - INTEGER
	If set non-zero, enables support for dynamic addresses.
	If set to a non-zero value larger than 1, a kernel log
	message will be printed when dynamic address rewriting
	occurs.
	(设为非零时启用动态地址支持。设为大于 1 的非零值时，发生动态地址
	改写会打印内核日志消息。)

	Default: 0
	(默认：0)

ip_early_demux - BOOLEAN
	Optimize input packet processing down to one demux for
	certain kinds of local sockets.  Currently we only do this
	for established TCP and connected UDP sockets.
	(把某些类型本地套接字的入站数据包处理优化为一次分用（demux）。
	目前只对已建立的 TCP 套接字和已连接的 UDP 套接字这样做。)

	It may add an additional cost for pure routing workloads that
	reduces overall throughput, in such case you should disable it.
	(对于纯路由型负载，它可能增加额外开销并降低总体吞吐，此时应禁用它。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

ping_group_range - 2 INTEGERS
	Restrict ICMP_PROTO datagram sockets to users in the group range.
	The default is "1 0", meaning, that nobody (not even root) may
	create ping sockets.  Setting it to "100 100" would grant permissions
	to the single group. "0 4294967294" would enable it for the world, "100
	4294967294" would enable it for the users, but not daemons.
	(把 ICMP_PROTO 数据报套接字限制给组范围内的用户。默认 "1 0" 表示
	任何人（包括 root）都不能创建 ping 套接字。设为 "100 100" 把权限授予
	单个组。"0 4294967294" 对所有人启用，"100 4294967294" 对用户启用，
	但不包括守护进程。)

tcp_early_demux - BOOLEAN
	Enable early demux for established TCP sockets.
	(为已建立的 TCP 套接字启用早期分用（early demux）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

udp_early_demux - BOOLEAN
	Enable early demux for connected UDP sockets. Disable this if
	your system could experience more unconnected load.
	(为已连接的 UDP 套接字启用早期分用（early demux）。如果系统可能承受
	较多非连接负载，应禁用此项。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

icmp_echo_ignore_all - BOOLEAN
	If enabled, then the kernel will ignore all ICMP ECHO
	requests sent to it.
	(若启用，内核将忽略发给它的所有 ICMP ECHO 请求。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

icmp_echo_enable_probe - BOOLEAN
        If enabled, then the kernel will respond to RFC 8335 PROBE
        requests sent to it.
        (若启用，内核将响应发给它的 RFC 8335 PROBE 请求。)

        Possible values:
        (可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

icmp_echo_ignore_broadcasts - BOOLEAN
	If enabled, then the kernel will ignore all ICMP ECHO and
	TIMESTAMP requests sent to it via broadcast/multicast.
	(若启用，内核将忽略通过广播/组播发给它的所有 ICMP ECHO 与 TIMESTAMP
	请求。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

icmp_ratelimit - INTEGER
	Limit the maximal rates for sending ICMP packets whose type matches
	icmp_ratemask (see below) to specific targets.
	0 to disable any limiting,
	otherwise the minimal space between responses in milliseconds.
	Note that another sysctl, icmp_msgs_per_sec limits the number
	of ICMP packets	sent on all targets.
	(限制向特定目标发送类型匹配 icmp_ratemask（见下文）的 ICMP 数据包的
	最大速率。0 表示禁用所有限速；否则为响应之间的最小间隔，单位为毫秒。
	注意，另一个 sysctl icmp_msgs_per_sec 限制发送到所有目标的 ICMP
	数据包数量。)

	Default: 1000
	(默认：1000)

icmp_msgs_per_sec - INTEGER
	Limit maximal number of ICMP packets sent per second from this host.
	Only messages whose type matches icmp_ratemask (see below) are
	controlled by this limit. For security reasons, the precise count
	of messages per second is randomized.
	(限制本主机每秒发送的 ICMP 数据包最大数量。只有类型匹配
	icmp_ratemask（见下文）的消息受此限制控制。出于安全原因，每秒的精确
	消息数会被随机化。)

	Default: 10000
	(默认：10000)

icmp_msgs_burst - INTEGER
	icmp_msgs_per_sec controls number of ICMP packets sent per second,
	while icmp_msgs_burst controls the token bucket size.
	For security reasons, the precise burst size is randomized.
	(icmp_msgs_per_sec 控制每秒发送的 ICMP 数据包数量，而 icmp_msgs_burst
	控制令牌桶大小。出于安全原因，精确的突发大小会被随机化。)

	Default: 10000
	(默认：10000)

icmp_ratemask - INTEGER
	Mask made of ICMP types for which rates are being limited.
	(由被限速的 ICMP 类型构成的掩码。)

	Significant bits: IHGFEDCBA9876543210
	(有效位：IHGFEDCBA9876543210)

	Default mask:     0000001100000011000 (6168)
	(默认掩码：0000001100000011000 (6168))

	Bit definitions (see include/linux/icmp.h):
	(位定义（见 include/linux/icmp.h）：)

		= =========================
		0 Echo Reply (回显应答)
		3 Destination Unreachable [1]_ (目的地不可达 [1]_)
		4 Source Quench [1]_ (源站抑制 [1]_)
		5 Redirect (重定向)
		8 Echo Request (回显请求)
		B Time Exceeded [1]_ (超时 [1]_)
		C Parameter Problem [1]_ (参数问题 [1]_)
		D Timestamp Request (时间戳请求)
		E Timestamp Reply (时间戳应答)
		F Info Request (信息请求)
		G Info Reply (信息应答)
		H Address Mask Request (地址掩码请求)
		I Address Mask Reply (地址掩码应答)
		= =========================

	.. [1] These are rate limited by default (see default mask above) (这些类型默认被限速（见上面的默认掩码）)

icmp_ignore_bogus_error_responses - BOOLEAN
	Some routers violate RFC1122 by sending bogus responses to broadcast
	frames.  Such violations are normally logged via a kernel warning.
	If enabled, the kernel will not give such warnings, which
	will avoid log file clutter.
	(某些路由器通过向广播帧发送伪造响应来违反 RFC1122。这类违规通常会以
	内核警告的形式记录。若启用，内核将不再发出此类警告，从而避免日志文件
	被垃圾信息充斥。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

icmp_errors_use_inbound_ifaddr - BOOLEAN

	If disabled, icmp error messages are sent with the primary address of
	the exiting interface.
	(若禁用，icmp 错误消息以出接口的主地址发送。)

	If enabled, the message will be sent with the primary address of
	the interface that received the packet that caused the icmp error.
	This is the behaviour many network administrators will expect from
	a router. And it can make debugging complicated network layouts
	much easier.
	(若启用，消息以接收到引发 icmp 错误的数据包的接口的主地址发送。这是
	许多网络管理员期望路由器具备的行为，并且能让复杂网络布局的调试容易
	得多。)

	Note that if no primary address exists for the interface selected,
	then the primary address of the first non-loopback interface that
	has one will be used regardless of this setting.
	(注意，如果所选接口没有主地址，则无论此设置如何，都会使用第一个拥有
	主地址的非环回接口的主地址。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

icmp_errors_extension_mask - UNSIGNED INTEGER
	Bitmask of ICMP extensions to append to ICMPv4 error messages
	("Destination Unreachable", "Time Exceeded" and "Parameter Problem").
	The original datagram is trimmed / padded to 128 bytes in order to be
	compatible with applications that do not comply with RFC 4884.
	(要附加到 ICMPv4 错误消息（"Destination Unreachable"、"Time Exceeded"
	和 "Parameter Problem"）的 ICMP 扩展位掩码。为兼容不符合 RFC 4884 的
	应用，原始数据报会被裁剪/填充到 128 字节。)

	Possible extensions are:
	(可能的扩展有：)

	==== ==============================================================
	0x01 Incoming IP interface information according to RFC 5837.
	     Extension will include the index, IPv4 address (if present),
	     name and MTU of the IP interface that received the datagram
	     which elicited the ICMP error. (按 RFC 5837 提供入站 IP 接口信息。扩展将包含接收到引发该 ICMP 错误的数据报的 IP 接口的索引、IPv4 地址（如存在）、名称和 MTU。)
	==== ==============================================================

	Default: 0x00 (no extensions)
	(默认：0x00（无扩展）)

igmp_max_memberships - INTEGER
	Change the maximum number of multicast groups we can subscribe to.
	Default: 20
	(改变可订阅组播组的最大数量。默认：20)

	Theoretical maximum value is bounded by having to send a membership
	report in a single datagram (i.e. the report can't span multiple
	datagrams, or risk confusing the switch and leaving groups you don't
	intend to).
	(理论上限受限于成员关系报告必须在单个数据报中发送（即报告不能跨越
	多个数据报，否则可能使交换机混乱并误退出本不想退出的组）。)

	The number of supported groups 'M' is bounded by the number of group
	report entries you can fit into a single datagram of 65535 bytes.
	(支持的组数 'M' 受限于单个 65535 字节数据报中能容纳的组报告条目数。)

	M = 65536-sizeof (ip header)/(sizeof(Group record))

	Group records are variable length, with a minimum of 12 bytes.
	So net.ipv4.igmp_max_memberships should not be set higher than:
	(组记录长度可变，最小 12 字节。因此 net.ipv4.igmp_max_memberships
	不应设置得高于：)

	(65536-24) / 12 = 5459

	The value 5459 assumes no IP header options, so in practice
	this number may be lower.
	(值 5459 假定没有 IP 头部选项，因此实际中该数字可能更低。)

igmp_max_msf - INTEGER
	Maximum number of addresses allowed in the source filter list for a
	multicast group.
	(组播组源过滤列表中允许的地址最大数量。)

	Default: 10
	(默认：10)

igmp_qrv - INTEGER
	Controls the IGMP query robustness variable (see RFC2236 8.1).
	(控制 IGMP 查询健壮性变量（见 RFC2236 8.1）。)

	Default: 2 (as specified by RFC2236 8.1)
	(默认：2（RFC2236 8.1 规定）)

	Minimum: 1 (as specified by RFC6636 4.5)
	(最小：1（RFC6636 4.5 规定）)

force_igmp_version - INTEGER
	- 0 - (default) No enforcement of a IGMP version, IGMPv1/v2 fallback
	  allowed. Will back to IGMPv3 mode again if all IGMPv1/v2 Querier
	  Present timer expires.
	  (0 -（默认）不强制使用某个 IGMP 版本，允许回退到 IGMPv1/v2。当所有
	  IGMPv1/v2 Querier Present 定时器到期后，会重新回到 IGMPv3 模式。)
	- 1 - Enforce to use IGMP version 1. Will also reply IGMPv1 report if
	  receive IGMPv2/v3 query.
	  (1 - 强制使用 IGMP 版本 1。收到 IGMPv2/v3 查询时也会回复 IGMPv1 报告。)
	- 2 - Enforce to use IGMP version 2. Will fallback to IGMPv1 if receive
	  IGMPv1 query message. Will reply report if receive IGMPv3 query.
	  (2 - 强制使用 IGMP 版本 2。收到 IGMPv1 查询报文时回退到 IGMPv1。
	  收到 IGMPv3 查询时会回复报告。)
	- 3 - Enforce to use IGMP version 3. The same react with default 0.
	  (3 - 强制使用 IGMP 版本 3。行为与默认值 0 相同。)

	.. note::

	   this is not the same with force_mld_version because IGMPv3 RFC3376
	   Security Considerations does not have clear description that we could
	   ignore other version messages completely as MLDv2 RFC3810. So make
	   this value as default 0 is recommended.
	   (注意：这与 force_mld_version 不同，因为 IGMPv3 RFC3376 的安全考虑
	   一节并没有像 MLDv2 RFC3810 那样明确说明可以完全忽略其他版本的消息，
	   因此建议保持该值默认为 0。)

``conf/interface/*``
	changes special settings per interface (where
	interface" is the name of your network interface)
	(按接口修改特殊设置（其中 interface" 是你的网络接口名）)

``conf/all/*``
	  is special, changes the settings for all interfaces
	  (比较特殊，修改所有接口的设置)

log_martians - BOOLEAN
	Log packets with impossible addresses to kernel log.
	log_martians for the interface will be enabled if at least one of
	conf/{all,interface}/log_martians is set to TRUE,
	it will be disabled otherwise
	(把带有不可能地址的数据包记录到内核日志。只要
	conf/{all,interface}/log_martians 中至少有一个设为 TRUE，
	接口的 log_martians 就会启用；否则禁用。)

accept_redirects - BOOLEAN
	Accept ICMP redirect messages.
	accept_redirects for the interface will be enabled if:
	(接受 ICMP 重定向消息。接口的 accept_redirects 在以下情况下启用：)

	- both conf/{all,interface}/accept_redirects are TRUE in the case
	  forwarding for the interface is enabled
	  (接口转发启用时，conf/{all,interface}/accept_redirects 两者都为 TRUE)

	or
	(或者)

	- at least one of conf/{all,interface}/accept_redirects is TRUE in the
	  case forwarding for the interface is disabled
	  (接口转发禁用时，conf/{all,interface}/accept_redirects 至少一个为 TRUE)

	accept_redirects for the interface will be disabled otherwise
	(否则接口的 accept_redirects 将被禁用)

	default:
	(默认：)

		- TRUE (host)
		  (TRUE（主机）)
		- FALSE (router)
		  (FALSE（路由器）)

forwarding - BOOLEAN
	Enable IP forwarding on this interface.  This controls whether packets
	received _on_ this interface can be forwarded.
	(在此接口上启用 IP 转发。它控制接收_自_该接口的数据包能否被转发。)

mc_forwarding - BOOLEAN
	Do multicast routing. The kernel needs to be compiled with CONFIG_MROUTE
	and a multicast routing daemon is required.
	conf/all/mc_forwarding must also be set to TRUE to enable multicast
	routing	for the interface
	(执行组播路由。内核需要编译 CONFIG_MROUTE，并且需要组播路由守护进程。
	要为该接口启用组播路由，conf/all/mc_forwarding 也必须设为 TRUE)

medium_id - INTEGER
	Integer value used to differentiate the devices by the medium they
	are attached to. Two devices can have different id values when
	the broadcast packets are received only on one of them.
	The default value 0 means that the device is the only interface
	to its medium, value of -1 means that medium is not known.
	(用于按设备所连接的介质区分设备的整数值。当广播数据包只在其一上收到时，
	两个设备可以有不同的 id 值。默认值 0 表示该设备是其介质的唯一接口，
	值 -1 表示介质未知。)

	Currently, it is used to change the proxy_arp behavior:
	the proxy_arp feature is enabled for packets forwarded between
	two devices attached to different media.
	(目前，它用于改变 proxy_arp 的行为：在连接到不同介质的两个设备之间
	转发的数据包启用 proxy_arp 特性。)

proxy_arp - BOOLEAN
	Do proxy arp.
	(执行代理 ARP。)

	proxy_arp for the interface will be enabled if at least one of
	conf/{all,interface}/proxy_arp is set to TRUE,
	it will be disabled otherwise
	(只要 conf/{all,interface}/proxy_arp 中至少有一个设为 TRUE，
	接口的 proxy_arp 就会启用；否则禁用。)

proxy_arp_pvlan - BOOLEAN
	Private VLAN proxy arp.
	(私有 VLAN 代理 ARP。)

	Basically allow proxy arp replies back to the same interface
	(from which the ARP request/solicitation was received).
	(基本上允许代理 ARP 应答回到同一接口（即收到 ARP 请求/请求通告的
	接口）。)

	This is done to support (ethernet) switch features, like RFC
	3069, where the individual ports are NOT allowed to
	communicate with each other, but they are allowed to talk to
	the upstream router.  As described in RFC 3069, it is possible
	to allow these hosts to communicate through the upstream
	router by proxy_arp'ing. Don't need to be used together with
	proxy_arp.
	(这样做是为了支持（以太网）交换机特性，例如 RFC 3069：各端口之间
	不允许互相通信，但允许与上游路由器通话。如 RFC 3069 所述，通过
	proxy_arp 可以让这些主机经由上游路由器通信。不需要与 proxy_arp
	一起使用。)

	This technology is known by different names:
	(这项技术有不同的名称：)

	- In RFC 3069 it is called VLAN Aggregation.
	  (在 RFC 3069 中称为 VLAN Aggregation。)
	- Cisco and Allied Telesyn call it Private VLAN.
	  (Cisco 和 Allied Telesyn 称之为 Private VLAN。)
	- Hewlett-Packard call it Source-Port filtering or port-isolation.
	  (惠普称之为 Source-Port filtering 或 port-isolation。)
	- Ericsson call it MAC-Forced Forwarding (RFC Draft).
	  (爱立信称之为 MAC-Forced Forwarding（RFC 草案）。)

proxy_delay - INTEGER
	Delay proxy response.
	(延迟代理响应。)

	Delay response to a neighbor solicitation when proxy_arp
	or proxy_ndp is enabled. A random value between [0, proxy_delay)
	will be chosen, setting to zero means reply with no delay.
	Value in jiffies. Defaults to 80.
	(启用 proxy_arp 或 proxy_ndp 时，延迟对邻居请求的响应。会选取
	[0, proxy_delay) 之间的随机值，设为零表示立即回复。数值单位为
	jiffies。默认 80。)

shared_media - BOOLEAN
	Send(router) or accept(host) RFC1620 shared media redirects.
	Overrides secure_redirects.
	(发送（路由器）或接受（主机）RFC1620 共享介质重定向。
	覆盖 secure_redirects。)

	shared_media for the interface will be enabled if at least one of
	conf/{all,interface}/shared_media is set to TRUE,
	it will be disabled otherwise
	(只要 conf/{all,interface}/shared_media 中至少有一个设为 TRUE，
	接口的 shared_media 就会启用；否则禁用。)

	default TRUE
	(默认 TRUE)

secure_redirects - BOOLEAN
	Accept ICMP redirect messages only to gateways listed in the
	interface's current gateway list. Even if disabled, RFC1122 redirect
	rules still apply.
	(只接受发往接口当前网关列表中所列网关的 ICMP 重定向消息。即使禁用，
	RFC1122 重定向规则仍然适用。)

	Overridden by shared_media.
	(被 shared_media 覆盖。)

	secure_redirects for the interface will be enabled if at least one of
	conf/{all,interface}/secure_redirects is set to TRUE,
	it will be disabled otherwise
	(只要 conf/{all,interface}/secure_redirects 中至少有一个设为 TRUE，
	接口的 secure_redirects 就会启用；否则禁用。)

	default TRUE
	(默认 TRUE)

send_redirects - BOOLEAN
	Send redirects, if router.
	(如果是路由器，则发送重定向。)

	send_redirects for the interface will be enabled if at least one of
	conf/{all,interface}/send_redirects is set to TRUE,
	it will be disabled otherwise
	(只要 conf/{all,interface}/send_redirects 中至少有一个设为 TRUE，
	接口的 send_redirects 就会启用；否则禁用。)

	Default: TRUE
	(默认：TRUE)

bootp_relay - BOOLEAN
	Accept packets with source address 0.b.c.d destined
	not to this host as local ones. It is supposed, that
	BOOTP relay daemon will catch and forward such packets.
	conf/all/bootp_relay must also be set to TRUE to enable BOOTP relay
	for the interface
	(接受源地址为 0.b.c.d、目的地不是本机的数据包并视为本地数据包。
	预期由 BOOTP 中继守护进程捕获并转发此类数据包。要为接口启用 BOOTP
	中继，conf/all/bootp_relay 也必须设为 TRUE)

	default FALSE
	(默认 FALSE)

	Not Implemented Yet.
	(尚未实现。)

accept_source_route - BOOLEAN
	Accept packets with SRR option.
	conf/all/accept_source_route must also be set to TRUE to accept packets
	with SRR option on the interface
	(接受带 SRR 选项的数据包。要在接口上接受带 SRR 选项的数据包，
	conf/all/accept_source_route 也必须设为 TRUE)

	default
	(默认：)

		- TRUE (router)
		  (TRUE（路由器）)
		- FALSE (host)
		  (FALSE（主机）)

accept_local - BOOLEAN
	Accept packets with local source addresses. In combination with
	suitable routing, this can be used to direct packets between two
	local interfaces over the wire and have them accepted properly.
	default FALSE
	(接受本地源地址的数据包。与合适的路由配合，可用于把两个本地接口之间
	的数据包经由线缆发送并被正确接受。默认 FALSE)

route_localnet - BOOLEAN
	Do not consider loopback addresses as martian source or destination
	while routing. This enables the use of 127/8 for local routing purposes.
	(路由时不要把环回地址视为非法（martian）源地址或目的地址。这使得
	127/8 可以用于本地路由目的。)

	default FALSE
	(默认 FALSE)

rp_filter - INTEGER
	- 0 - No source validation.
	  (0 - 不做源地址校验。)
	- 1 - Strict mode as defined in RFC3704 Strict Reverse Path
	  Each incoming packet is tested against the FIB and if the interface
	  is not the best reverse path the packet check will fail.
	  By default failed packets are discarded.
	  (1 - RFC3704 Strict Reverse Path 定义的严格模式。
	  每个入站数据包都会对照 FIB 进行测试，如果该接口不是最佳反向路径，
	  数据包检查将失败。默认丢弃未通过检查的数据包。)
	- 2 - Loose mode as defined in RFC3704 Loose Reverse Path
	  Each incoming packet's source address is also tested against the FIB
	  and if the source address is not reachable via any interface
	  the packet check will fail.
	  (2 - RFC3704 Loose Reverse Path 定义的宽松模式。
	  每个入站数据包的源地址也会对照 FIB 测试，如果经任何接口都不可达
	  该源地址，数据包检查将失败。)

	Current recommended practice in RFC3704 is to enable strict mode
	to prevent IP spoofing from DDos attacks. If using asymmetric routing
	or other complicated routing, then loose mode is recommended.
	(RFC3704 目前推荐的做法是启用严格模式以防止 DDoS 攻击中的 IP 欺骗。
	如果使用非对称路由或其他复杂路由，则建议使用宽松模式。)

	The max value from conf/{all,interface}/rp_filter is used
	when doing source validation on the {interface}.
	(在 {interface} 上进行源地址校验时，使用 conf/{all,interface}/rp_filter
	中的最大值。)

	Default value is 0. Note that some distributions enable it
	in startup scripts.
	(默认值为 0。注意某些发行版会在启动脚本中启用它。)

src_valid_mark - BOOLEAN
	- 0 - The fwmark of the packet is not included in reverse path
	  route lookup.  This allows for asymmetric routing configurations
	  utilizing the fwmark in only one direction, e.g., transparent
	  proxying.
	  (0 - 数据包的 fwmark 不参与反向路径路由查找。这允许仅在单方向使用
	  fwmark 的非对称路由配置，例如透明代理。)

	- 1 - The fwmark of the packet is included in reverse path route
	  lookup.  This permits rp_filter to function when the fwmark is
	  used for routing traffic in both directions.
	  (1 - 数据包的 fwmark 参与反向路径路由查找。当 fwmark 用于双向流量
	  路由时，rp_filter 仍可正常工作。)

	This setting also affects the utilization of fmwark when
	performing source address selection for ICMP replies, or
	determining addresses stored for the IPOPT_TS_TSANDADDR and
	IPOPT_RR IP options.
	(此设置还影响为 ICMP 应答执行源地址选择，或为 IPOPT_TS_TSANDADDR 与
	IPOPT_RR IP 选项确定所存地址时对 fwmark 的使用。)

	The max value from conf/{all,interface}/src_valid_mark is used.
	(使用 conf/{all,interface}/src_valid_mark 中的最大值。)

	Default value is 0.
	(默认值为 0。)

arp_filter - BOOLEAN
	- 1 - Allows you to have multiple network interfaces on the same
	  subnet, and have the ARPs for each interface be answered
	  based on whether or not the kernel would route a packet from
	  the ARP'd IP out that interface (therefore you must use source
	  based routing for this to work). In other words it allows control
	  of which cards (usually 1) will respond to an arp request.
	  (1 - 允许在同一子网上有多个网络接口，并且每个接口的 ARP 应答与否
	  取决于内核是否会将来自该 ARP IP 的数据包从该接口路由出去（因此必须
	  使用基于源的路由此设置才能生效）。换言之，它可以控制哪块网卡
	  （通常为 1 块）响应 arp 请求。)

	- 0 - (default) The kernel can respond to arp requests with addresses
	  from other interfaces. This may seem wrong but it usually makes
	  sense, because it increases the chance of successful communication.
	  IP addresses are owned by the complete host on Linux, not by
	  particular interfaces. Only for more complex setups like load-
	  balancing, does this behaviour cause problems.
	  (0 -（默认）内核可以用其他接口的地址响应 arp 请求。这看似不对，
	  但通常是有意义的，因为它增加了通信成功的机会。在 Linux 上 IP 地址
	  属于整个主机而非特定接口。只有负载均衡等更复杂的配置中，这种行为
	  才会引发问题。)

	arp_filter for the interface will be enabled if at least one of
	conf/{all,interface}/arp_filter is set to TRUE,
	it will be disabled otherwise
	(只要 conf/{all,interface}/arp_filter 中至少有一个设为 TRUE，
	接口的 arp_filter 就会启用；否则禁用。)

arp_announce - INTEGER
	Define different restriction levels for announcing the local
	source IP address from IP packets in ARP requests sent on
	interface:
	(定义在接口发送的 ARP 请求中通告来自 IP 数据包的本地源 IP 地址时
	的不同限制级别：)

	- 0 - (default) Use any local address, configured on any interface
	  (0 -（默认）使用任意接口上配置的任意本地地址)
	- 1 - Try to avoid local addresses that are not in the target's
	  subnet for this interface. This mode is useful when target
	  hosts reachable via this interface require the source IP
	  address in ARP requests to be part of their logical network
	  configured on the receiving interface. When we generate the
	  request we will check all our subnets that include the
	  target IP and will preserve the source address if it is from
	  such subnet. If there is no such subnet we select source
	  address according to the rules for level 2.
	  (1 - 尽量避免使用不属于该接口上目标子网的本地地址。当经由该接口
	  可达的目标主机要求 ARP 请求中的源 IP 地址属于其在接收接口上配置的
	  逻辑网络时，此模式有用。生成请求时我们会检查所有包含目标 IP 的
	  子网，若源地址来自这样的子网则予以保留。如果不存在这样的子网，
	  则按级别 2 的规则选择源地址。)
	- 2 - Always use the best local address for this target.
	  In this mode we ignore the source address in the IP packet
	  and try to select local address that we prefer for talks with
	  the target host. Such local address is selected by looking
	  for primary IP addresses on all our subnets on the outgoing
	  interface that include the target IP address. If no suitable
	  local address is found we select the first local address
	  we have on the outgoing interface or on all other interfaces,
	  with the hope we will receive reply for our request and
	  even sometimes no matter the source IP address we announce.
	  (2 - 始终使用对该目标而言最佳的本地地址。在此模式下，我们忽略
	  IP 数据包中的源地址，尝试选择我们更愿意用于与目标主机通信的本地
	  地址。选择方式是在出接口上所有包含目标 IP 地址的子网中寻找主 IP
	  地址。如果找不到合适的本地地址，则选择出接口或其他所有接口上的
	  第一个本地地址，希望能收到对我们请求的应答——有时甚至不管我们
	  通告的源 IP 地址是什么。)

	The max value from conf/{all,interface}/arp_announce is used.
	(使用 conf/{all,interface}/arp_announce 中的最大值。)

	Increasing the restriction level gives more chance for
	receiving answer from the resolved target while decreasing
	the level announces more valid sender's information.
	(提高限制级别更有可能从解析出的目标收到应答，而降低级别则通告
	更多有效的发送者信息。)

arp_ignore - INTEGER
	Define different modes for sending replies in response to
	received ARP requests that resolve local target IP addresses:
	(为响应解析本地目标 IP 地址的 ARP 请求定义不同的应答模式：)

	- 0 - (default): reply for any local target IP address, configured
	  on any interface
	  (0 -（默认）：对配置在任意接口上的任意本地目标 IP 地址都应答)
	- 1 - reply only if the target IP address is local address
	  configured on the incoming interface
	  (1 - 仅当目标 IP 地址是配置在入接口上的本地地址时才应答)
	- 2 - reply only if the target IP address is local address
	  configured on the incoming interface and both with the
	  sender's IP address are part from same subnet on this interface
	  (2 - 仅当目标 IP 地址是配置在入接口上的本地地址，且它与发送者的
	  IP 地址同属该接口上的同一子网时才应答)
	- 3 - do not reply for local addresses configured with scope host,
	  only resolutions for global and link addresses are replied
	  (3 - 对配置为 scope host 的本地地址不应答，只应答全局和链路地址
	  的解析)
	- 4-7 - reserved
	  (4-7 - 保留)
	- 8 - do not reply for all local addresses
	  (8 - 对所有本地地址都不应答)

	The max value from conf/{all,interface}/arp_ignore is used
	when ARP request is received on the {interface}
	(在 {interface} 上收到 ARP 请求时，使用 conf/{all,interface}/arp_ignore
	中的最大值)

arp_notify - BOOLEAN
	Define mode for notification of address and device changes.
	(定义地址与设备变更的通知模式。)

	 ==  ==========================================================
	  0  (default): do nothing (（默认）：不做任何事)
	  1  Generate gratuitous arp requests when device is brought up
	     or hardware address changes. (设备启用或硬件地址变化时生成免费 arp 请求。)
	 ==  ==========================================================

arp_accept - INTEGER
	Define behavior for accepting gratuitous ARP (garp) frames from devices
	that are not already present in the ARP table:
	(定义接受来自 ARP 表中尚不存在的设备的免费 ARP（garp）帧时的行为：)

	- 0 - don't create new entries in the ARP table
	  (0 - 不在 ARP 表中创建新表项)
	- 1 - create new entries in the ARP table
	  (1 - 在 ARP 表中创建新表项)
	- 2 - create new entries only if the source IP address is in the same
	  subnet as an address configured on the interface that received the
	  garp message.
	  (2 - 仅当源 IP 地址与接收到 garp 消息的接口上配置的某地址位于同一
	  子网时才创建新表项。)

	Both replies and requests type gratuitous arp will trigger the
	ARP table to be updated, if this setting is on.
	(若此设置开启，应答与请求类型的免费 arp 都会触发 ARP 表更新。)

	If the ARP table already contains the IP address of the
	gratuitous arp frame, the arp table will be updated regardless
	if this setting is on or off.
	(如果 ARP 表已包含该免费 arp 帧的 IP 地址，则无论此设置开或关，
	ARP 表都会被更新。)

arp_evict_nocarrier - BOOLEAN
	Clears the ARP cache on NOCARRIER events. This option is important for
	wireless devices where the ARP cache should not be cleared when roaming
	between access points on the same network. In most cases this should
	remain as the default (1).
	(在 NOCARRIER 事件时清除 ARP 缓存。此选项对无线设备很重要：在同一
	网络的接入点之间漫游时不应清除 ARP 缓存。大多数情况下应保持默认值
	（1）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - Do not clear ARP cache on NOCARRIER events
	  (0（禁用）——NOCARRIER 事件时不清除 ARP 缓存)
	- 1 (enabled)  - Clear the ARP cache on NOCARRIER events
	  (1（启用）——NOCARRIER 事件时清除 ARP 缓存)

	Default: 1 (enabled)
	(默认：1（启用）)

mcast_solicit - INTEGER
	The maximum number of multicast probes in INCOMPLETE state,
	when the associated hardware address is unknown.  Defaults
	to 3.
	(关联硬件地址未知时，INCOMPLETE 状态下组播探测的最大次数。
	默认为 3。)

ucast_solicit - INTEGER
	The maximum number of unicast probes in PROBE state, when
	the hardware address is being reconfirmed.  Defaults to 3.
	(正在重新确认硬件地址时，PROBE 状态下单播探测的最大次数。
	默认为 3。)

app_solicit - INTEGER
	The maximum number of probes to send to the user space ARP daemon
	via netlink before dropping back to multicast probes (see
	mcast_resolicit).  Defaults to 0.
	(在回退到组播探测之前（见 mcast_resolicit），通过 netlink 向用户态
	ARP 守护进程发送探测的最大次数。默认为 0。)

mcast_resolicit - INTEGER
	The maximum number of multicast probes after unicast and
	app probes in PROBE state.  Defaults to 0.
	(PROBE 状态下单播探测和应用探测之后的组播探测最大次数。默认为 0。)

disable_policy - BOOLEAN
	Disable IPSEC policy (SPD) for this interface
	(为该接口禁用 IPSEC 策略（SPD）)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

disable_xfrm - BOOLEAN
	Disable IPSEC encryption on this interface, whatever the policy
	(为该接口禁用 IPSEC 加密，无论策略如何)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

igmpv2_unsolicited_report_interval - INTEGER
	The interval in milliseconds in which the next unsolicited
	IGMPv1 or IGMPv2 report retransmit will take place.
	(下一次主动（unsolicited）IGMPv1 或 IGMPv2 报告重传的间隔，
	单位为毫秒。)

	Default: 10000 (10 seconds)
	(默认：10000（10 秒）)

igmpv3_unsolicited_report_interval - INTEGER
	The interval in milliseconds in which the next unsolicited
	IGMPv3 report retransmit will take place.
	(下一次主动 IGMPv3 报告重传的间隔，单位为毫秒。)

	Default: 1000 (1 seconds)
	(默认：1000（1 秒）)

ignore_routes_with_linkdown - BOOLEAN
        Ignore routes whose link is down when performing a FIB lookup.
        (执行 FIB 查找时忽略链路已断开的路由。)

        Possible values:
        (可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

promote_secondaries - BOOLEAN
	When a primary IP address is removed from this interface
	promote a corresponding secondary IP address instead of
	removing all the corresponding secondary IP addresses.
	(从该接口删除主 IP 地址时，提升（promote）一个对应的辅助 IP 地址，
	而不是删除所有对应的辅助 IP 地址。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

drop_unicast_in_l2_multicast - BOOLEAN
	Drop any unicast IP packets that are received in link-layer
	multicast (or broadcast) frames.
	(丢弃任何在链路层组播（或广播）帧中收到的单播 IP 数据包。)

	This behavior (for multicast) is actually a SHOULD in RFC
	1122, but is disabled by default for compatibility reasons.
	(这种行为（对组播而言）实际上是 RFC 1122 中的 SHOULD，但出于兼容性
	原因默认禁用。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

drop_gratuitous_arp - BOOLEAN
	Drop all gratuitous ARP frames, for example if there's a known
	good ARP proxy on the network and such frames need not be used
	(or in the case of 802.11, must not be used to prevent attacks.)
	(丢弃所有免费 ARP 帧，例如当网络上存在已知良好的 ARP 代理而不需要
	使用这类帧时（或在 802.11 场景下，为防止攻击而必须不使用）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)


tag - INTEGER
	Allows you to write a number, which can be used as required.
	(允许写入一个数字，可按需使用。)

	Default value is 0.
	(默认值为 0。)

xfrm4_gc_thresh - INTEGER
	(Obsolete since linux-4.14)
	The threshold at which we will start garbage collecting for IPv4
	destination cache entries.  At twice this value the system will
	refuse new allocations.
	(（linux-4.14 起已过时）
	开始对 IPv4 目的缓存条目进行垃圾回收的阈值。达到该值的两倍时，
	系统将拒绝新的分配。)

igmp_link_local_mcast_reports - BOOLEAN
	Enable IGMP reports for link local multicast groups in the
	224.0.0.X range.
	(为 224.0.0.X 范围内的链路本地组播组启用 IGMP 报告。)

	Default TRUE
	(默认 TRUE)

Alexey Kuznetsov.
kuznet@ms2.inr.ac.ru

Updated by:
(更新者：)

- Andi Kleen
  ak@muc.de
- Nicolas Delon
  delon.nicolas@wanadoo.fr




/proc/sys/net/ipv6/* Variables(/proc/sys/net/ipv6/* 变量)
=========================================================

IPv6 has no global variables such as tcp_*.  tcp_* settings under ipv4/ also
apply to IPv6 [XXX?].
(IPv6 没有诸如 tcp_* 之类的全局变量。ipv4/ 下的 tcp_* 设置同样
适用于 IPv6 [XXX?]。)

bindv6only - BOOLEAN
	Default value for IPV6_V6ONLY socket option,
	which restricts use of the IPv6 socket to IPv6 communication
	only.
	(IPV6_V6ONLY 套接字选项的默认值，它把 IPv6 套接字的使用限制为
	仅 IPv6 通信。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - enable IPv4-mapped address feature
	  (0（禁用）——启用 IPv4 映射地址特性)
	- 1 (enabled)  - disable IPv4-mapped address feature
	  (1（启用）——禁用 IPv4 映射地址特性)

	Default: 0 (disabled)
	(默认：0（禁用）)

flowlabel_consistency - BOOLEAN
	Protect the consistency (and unicity) of flow label.
	You have to disable it to use IPV6_FL_F_REFLECT flag on the
	flow label manager.
	(保护流标签的一致性（和唯一性）。要在流标签管理器上使用
	IPV6_FL_F_REFLECT 标志，必须禁用它。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

auto_flowlabels - INTEGER
	Automatically generate flow labels based on a flow hash of the
	packet. This allows intermediate devices, such as routers, to
	identify packet flows for mechanisms like Equal Cost Multipath
	Routing (see RFC 6438).
	(基于数据包的流哈希自动生成流标签。这让路由器等中间设备能够识别
	数据包流，以支持等价多路径路由等机制（见 RFC 6438）。)

	=  ===========================================================
	0  automatic flow labels are completely disabled (自动流标签完全禁用)
	1  automatic flow labels are enabled by default, they can be
	   disabled on a per socket basis using the IPV6_AUTOFLOWLABEL
	   socket option (自动流标签默认启用，可通过 IPV6_AUTOFLOWLABEL 套接字选项按套接字禁用)
	2  automatic flow labels are allowed, they may be enabled on a
	   per socket basis using the IPV6_AUTOFLOWLABEL socket option (允许自动流标签，可通过 IPV6_AUTOFLOWLABEL 套接字选项按套接字启用)
	3  automatic flow labels are enabled and enforced, they cannot
	   be disabled by the socket option (自动流标签启用并强制执行，无法通过套接字选项禁用)
	=  ===========================================================

	Default: 1
	(默认：1)

flowlabel_state_ranges - BOOLEAN
	Split the flow label number space into two ranges. 0-0x7FFFF is
	reserved for the IPv6 flow manager facility, 0x80000-0xFFFFF
	is reserved for stateless flow labels as described in RFC6437.
	(把流标签编号空间分成两个范围。0-0x7FFFF 保留给 IPv6 流管理器设施，
	0x80000-0xFFFFF 按 RFC6437 保留给无状态流标签。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)


flowlabel_reflect - INTEGER
	Control flow label reflection. Needed for Path MTU
	Discovery to work with Equal Cost Multipath Routing in anycast
	environments. See RFC 7690 and:
	https://tools.ietf.org/html/draft-wang-6man-flow-label-reflection-01
	(控制流标签反射。任播环境下，路径 MTU 发现要与等价多路径路由配合
	工作需要它。参见 RFC 7690 与：
	https://tools.ietf.org/html/draft-wang-6man-flow-label-reflection-01)

	This is a bitmask.
	(这是一个位掩码。)

	- 1: enabled for established flows
	  (1：对已建立的流启用)

	  Note that this prevents automatic flowlabel changes, as done
	  in "tcp: change IPv6 flow-label upon receiving spurious retransmission"
	  and "tcp: Change txhash on every SYN and RTO retransmit"
	  (注意，这会阻止自动的流标签变更，例如
	  "tcp: change IPv6 flow-label upon receiving spurious retransmission"
	  与 "tcp: Change txhash on every SYN and RTO retransmit" 中所做的)

	- 2: enabled for TCP RESET packets (no active listener)
	  If set, a RST packet sent in response to a SYN packet on a closed
	  port will reflect the incoming flow label.
	  (2：对 TCP RESET 数据包启用（无活动监听者）。
	  设置后，对关闭端口上 SYN 数据包回应的 RST 数据包会反射入站的流标签。)

	- 4: enabled for ICMPv6 echo reply messages.
	  (4：对 ICMPv6 回显应答消息启用。)

	Default: 0
	(默认：0)

fib_multipath_hash_policy - INTEGER
	Controls which hash policy to use for multipath routes.
	(控制多路径路由使用哪种哈希策略。)

	Default: 0 (Layer 3)
	(默认：0（第 3 层）)

	Possible values:
	(可能的取值：)

	- 0 - Layer 3 (source and destination addresses plus flow label)
	  (0 - 第 3 层（源与目的地址加流标签）)
	- 1 - Layer 4 (standard 5-tuple)
	  (1 - 第 4 层（标准五元组）)
	- 2 - Layer 3 or inner Layer 3 if present
	  (2 - 第 3 层，如存在内层则用内层第 3 层)
	- 3 - Custom multipath hash. Fields used for multipath hash calculation
	  are determined by fib_multipath_hash_fields sysctl
	  (3 - 自定义多路径哈希。用于多路径哈希计算的字段由
	  fib_multipath_hash_fields sysctl 决定)

fib_multipath_hash_fields - UNSIGNED INTEGER
	When fib_multipath_hash_policy is set to 3 (custom multipath hash), the
	fields used for multipath hash calculation are determined by this
	sysctl.
	(当 fib_multipath_hash_policy 设为 3（自定义多路径哈希）时，用于
	多路径哈希计算的字段由此 sysctl 决定。)

	This value is a bitmask which enables various fields for multipath hash
	calculation.
	(该值是一个位掩码，用于启用参与多路径哈希计算的各个字段。)

	Possible fields are:
	(可能的字段有：)

	====== ============================
	0x0001 Source IP address (源 IP 地址)
	0x0002 Destination IP address (目的 IP 地址)
	0x0004 IP protocol (IP 协议)
	0x0008 Flow Label (流标签)
	0x0010 Source port (源端口)
	0x0020 Destination port (目的端口)
	0x0040 Inner source IP address (内层源 IP 地址)
	0x0080 Inner destination IP address (内层目的 IP 地址)
	0x0100 Inner IP protocol (内层 IP 协议)
	0x0200 Inner Flow Label (内层流标签)
	0x0400 Inner source port (内层源端口)
	0x0800 Inner destination port (内层目的端口)
	====== ============================

	Default: 0x0007 (source IP, destination IP and IP protocol)
	(默认：0x0007（源 IP、目的 IP 与 IP 协议）)

anycast_src_echo_reply - BOOLEAN
	Controls the use of anycast addresses as source addresses for ICMPv6
	echo reply
	(控制是否使用任播地址作为 ICMPv6 回显应答的源地址)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)


idgen_delay - INTEGER
	Controls the delay in seconds after which time to retry
	privacy stable address generation if a DAD conflict is
	detected.
	(检测到 DAD 冲突时，控制多少秒之后重试隐私稳定地址生成。)

	Default: 1 (as specified in RFC7217)
	(默认：1（RFC7217 规定）)

idgen_retries - INTEGER
	Controls the number of retries to generate a stable privacy
	address if a DAD conflict is detected.
	(检测到 DAD 冲突时，控制生成稳定隐私地址的重试次数。)

	Default: 3 (as specified in RFC7217)
	(默认：3（RFC7217 规定）)

mld_qrv - INTEGER
	Controls the MLD query robustness variable (see RFC3810 9.1).
	(控制 MLD 查询健壮性变量（见 RFC3810 9.1）。)

	Default: 2 (as specified by RFC3810 9.1)
	(默认：2（RFC3810 9.1 规定）)

	Minimum: 1 (as specified by RFC6636 4.5)
	(最小：1（RFC6636 4.5 规定）)

max_dst_opts_number - INTEGER
	Maximum number of non-padding TLVs allowed in a Destination
	options extension header. If this value is less than zero
	then unknown options are disallowed and the number of known
	TLVs allowed is the absolute value of this number.
	(目的地选项扩展头中允许的非填充 TLV 最大数量。若该值小于零，
	则不允许未知选项，且允许的已知 TLV 数量为该数值的绝对值。)

	Default: 8
	(默认：8)

max_hbh_opts_number - INTEGER
	Maximum number of non-padding TLVs allowed in a Hop-by-Hop
	options extension header. If this value is less than zero
	then unknown options are disallowed and the number of known
	TLVs allowed is the absolute value of this number.
	(逐跳选项扩展头中允许的非填充 TLV 最大数量。若该值小于零，
	则不允许未知选项，且允许的已知 TLV 数量为该数值的绝对值。)

	Default: 8
	(默认：8)

max_dst_opts_length - INTEGER
	Maximum length allowed for a Destination options extension
	header.
	(目的地选项扩展头允许的最大长度。)

	Default: INT_MAX (unlimited)
	(默认：INT_MAX（不限制）)

max_hbh_length - INTEGER
	Maximum length allowed for a Hop-by-Hop options extension
	header.
	(逐跳选项扩展头允许的最大长度。)

	Default: INT_MAX (unlimited)
	(默认：INT_MAX（不限制）)

skip_notify_on_dev_down - BOOLEAN
	Controls whether an RTM_DELROUTE message is generated for routes
	removed when a device is taken down or deleted. IPv4 does not
	generate this message; IPv6 does by default. Setting this sysctl
	to true skips the message, making IPv4 and IPv6 on par in relying
	on userspace caches to track link events and evict routes.
	(控制在设备被关闭或删除时，是否为被移除的路由生成 RTM_DELROUTE 消息。
	IPv4 不生成该消息；IPv6 默认生成。把该 sysctl 设为 true 将跳过该消息，
	使 IPv4 与 IPv6 一样依赖用户态缓存来跟踪链路事件并清除路由。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - generate the message
	  (0（禁用）——生成该消息)
	- 1 (enabled)  - skip generating the message
	  (1（启用）——跳过生成该消息)

	Default: 0 (disabled)
	(默认：0（禁用）)

nexthop_compat_mode - BOOLEAN
	New nexthop API provides a means for managing nexthops independent of
	prefixes. Backwards compatibility with old route format is enabled by
	default which means route dumps and notifications contain the new
	nexthop attribute but also the full, expanded nexthop definition.
	Further, updates or deletes of a nexthop configuration generate route
	notifications for each fib entry using the nexthop. Once a system
	understands the new API, this sysctl can be disabled to achieve full
	performance benefits of the new API by disabling the nexthop expansion
	and extraneous notifications.
	(新下一跳 API 提供了独立于前缀管理下一跳的手段。默认启用对旧路由格式
	的向后兼容，这意味着路由转储和通知既包含新的下一跳属性，也包含完整的
	展开式下一跳定义。此外，更新或删除下一跳配置时，会为使用该下一跳的
	每个 fib 表项生成路由通知。一旦系统理解了新 API，就可以禁用该 sysctl，
	通过关闭下一跳展开和多余的通知来获得新 API 的全部性能收益。)

	Note that as a backward-compatible mode, dumping of modern features
	might be incomplete or wrong. For example, resilient groups will not be
	shown as such, but rather as just a list of next hops. Also weights that
	do not fit into 8 bits will show incorrectly.
	(注意，作为向后兼容模式，现代特性的转储可能不完整或有误。例如，弹性
	组（resilient groups）不会按其本来面目显示，而只显示为一个下一跳列表。
	此外，不能放入 8 位的权重会显示错误。)

	Default: true (backward compat mode)
	(默认：true（向后兼容模式）)

fib_notify_on_flag_change - INTEGER
        Whether to emit RTM_NEWROUTE notifications whenever RTM_F_OFFLOAD/
        RTM_F_TRAP/RTM_F_OFFLOAD_FAILED flags are changed.
        (每当 RTM_F_OFFLOAD/RTM_F_TRAP/RTM_F_OFFLOAD_FAILED 标志发生变化时，是否发出 RTM_NEWROUTE 通知。)

        After installing a route to the kernel, user space receives an
        acknowledgment, which means the route was installed in the kernel,
        but not necessarily in hardware.
        It is also possible for a route already installed in hardware to change
        its action and therefore its flags. For example, a host route that is
        trapping packets can be "promoted" to perform decapsulation following
        the installation of an IPinIP/VXLAN tunnel.
        The notifications will indicate to user-space the state of the route.
        (向内核安装路由后，用户态会收到一条确认，表明该路由已装入内核，但不一定
        装入了硬件。已装入硬件的路由也可能改变其动作进而改变其标志。例如，在
        安装 IPinIP/VXLAN 隧道之后，原本对数据包进行陷阱捕获的主机路由可以被
        "提升"为执行解封装。这些通知会向用户态指示路由的状态。)

        Default: 0 (Do not emit notifications.)
        (默认：0（不发出通知）。)

        Possible values:
        (可能的取值：)

        - 0 - Do not emit notifications.
          (0 - 不发出通知。)
        - 1 - Emit notifications.
          (1 - 发出通知。)
        - 2 - Emit notifications only for RTM_F_OFFLOAD_FAILED flag change.
          (2 - 仅在 RTM_F_OFFLOAD_FAILED 标志变化时发出通知。)

ioam6_id - INTEGER
        Define the IOAM id of this node. Uses only 24 bits out of 32 in total.
        (定义本节点的 IOAM id。总共只使用 32 位中的 24 位。)

        Possible value range:
        (可能的取值范围：)

        - Min: 0
          (最小：0)
        - Max: 0xFFFFFF
          (最大：0xFFFFFF)

        Default: 0xFFFFFF
        (默认：0xFFFFFF)

ioam6_id_wide - LONG INTEGER
        Define the wide IOAM id of this node. Uses only 56 bits out of 64 in
        total. Can be different from ioam6_id.
        (定义本节点的宽 IOAM id。总共只使用 64 位中的 56 位。可以与
        ioam6_id 不同。)

        Possible value range:
        (可能的取值范围：)

        - Min: 0
          (最小：0)
        - Max: 0xFFFFFFFFFFFFFF
          (最大：0xFFFFFFFFFFFFFF)

        Default: 0xFFFFFFFFFFFFFF
        (默认：0xFFFFFFFFFFFFFF)

IPv6 Fragmentation:
(IPv6 分片：)

ip6frag_high_thresh - INTEGER
	Maximum memory used to reassemble IPv6 fragments. When
	ip6frag_high_thresh bytes of memory is allocated for this purpose,
	the fragment handler will toss packets until ip6frag_low_thresh
	is reached.
	(用于重组 IPv6 分片的最大内存。当为此目的分配的内存达到
	ip6frag_high_thresh 字节时，分片处理程序将丢弃数据包，直到内存降到
	ip6frag_low_thresh 以下。)

ip6frag_low_thresh - INTEGER
	See ip6frag_high_thresh
	(见 ip6frag_high_thresh)

ip6frag_time - INTEGER
	Time in seconds to keep an IPv6 fragment in memory.
	(IPv6 分片在内存中保留的时间，单位为秒。)

``conf/default/*``:
	Change the interface-specific default settings.
	(修改按接口的默认设置。)

	These settings would be used during creating new interfaces.
	(这些设置将在创建新接口时使用。)


``conf/all/*``:
	Change all the interface-specific settings.
	(修改所有按接口的设置。)

	[XXX:  Other special features than forwarding?]
	([XXX: 转发之外的其他特殊特性？])

conf/all/disable_ipv6 - BOOLEAN
	Changing this value is same as changing ``conf/default/disable_ipv6``
	setting and also all per-interface ``disable_ipv6`` settings to the same
	value.
	(修改该值等同于把 ``conf/default/disable_ipv6`` 设置以及所有按接口的
	``disable_ipv6`` 设置改为同一值。)

	Reading this value does not have any particular meaning. It does not say
	whether IPv6 support is enabled or disabled. Returned value can be 1
	also in the case when some interface has ``disable_ipv6`` set to 0 and
	has configured IPv6 addresses.
	(读取该值没有任何特别含义。它并不说明 IPv6 支持处于启用还是禁用状态。
	即使某些接口 ``disable_ipv6`` 设为 0 且已配置 IPv6 地址，返回值也可能是 1。)

conf/all/forwarding - BOOLEAN
	Enable global IPv6 forwarding between all interfaces.
	(在所有接口之间启用全局 IPv6 转发。)

	IPv4 and IPv6 work differently here; the ``force_forwarding`` flag must
	be used to control which interfaces may forward packets.
	(IPv4 与 IPv6 在此处工作方式不同；必须使用 ``force_forwarding`` 标志来
	控制哪些接口可以转发数据包。)

	This also sets all interfaces' Host/Router setting
	'forwarding' to the specified value.  See below for details.
	(这也会把所有接口的主机/路由器设置 'forwarding' 设为指定值。
	详情见下文。)

	This referred to as global forwarding.
	(这被称为全局转发。)

proxy_ndp - BOOLEAN
	Do proxy ndp.
	(执行代理 NDP。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

force_forwarding - BOOLEAN
	Enable forwarding on this interface only -- regardless of the setting on
	``conf/all/forwarding``. When setting ``conf.all.forwarding`` to 0,
	the ``force_forwarding`` flag will be reset on all interfaces.
	(仅在该接口上启用转发——不管 ``conf/all/forwarding`` 的设置如何。
	当把 ``conf.all.forwarding`` 设为 0 时，所有接口上的
	``force_forwarding`` 标志将被重置。)

fwmark_reflect - BOOLEAN
	Controls the fwmark of kernel-generated IPv6 reply packets that are not
	associated with a socket for example, TCP RSTs or ICMPv6 echo replies).
	If disabled, these packets have a fwmark of zero. If enabled, they have the
	fwmark of the packet they are replying to.
	(控制内核生成的、未关联到套接字的 IPv6 应答数据包（例如 TCP RST 或
	ICMPv6 回显应答）的 fwmark。若禁用，这些数据包的 fwmark 为零；若启用，
	则沿用其所应答数据包的 fwmark。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

``conf/interface/*``:
	Change special settings per interface.
	(按接口修改特殊设置。)

	The functional behaviour for certain settings is different
	depending on whether local forwarding is enabled or not.
	(某些设置的实际行为取决于本地转发是否启用而有所不同。)

accept_ra - INTEGER
	Accept Router Advertisements; autoconfigure using them.
	(接受路由器通告（RA），并用它们进行自动配置。)

	It also determines whether or not to transmit Router
	Solicitations. If and only if the functional setting is to
	accept Router Advertisements, Router Solicitations will be
	transmitted.
	(它还决定是否发送路由器请求（Router Solicitations）。当且仅当实际
	设置是接受路由器通告时，才会发送路由器请求。)

	Possible values are:
	(可能的取值为：)

		==  ===========================================================
		 0  Do not accept Router Advertisements. (不接受路由器通告。)
		 1  Accept Router Advertisements if forwarding is disabled. (转发禁用时接受路由器通告。)
		 2  Overrule forwarding behaviour. Accept Router Advertisements
		    even if forwarding is enabled. (无视转发行为。即使转发已启用也接受路由器通告。)
		==  ===========================================================

	Functional default:
	(实际默认值：)

		- enabled if local forwarding is disabled.
		  (本地转发禁用时启用。)
		- disabled if local forwarding is enabled.
		  (本地转发启用时禁用。)

accept_ra_defrtr - BOOLEAN
	Learn default router in Router Advertisement.
	(从路由器通告中学习默认路由器。)

	Functional default:
	(实际默认值：)

		- enabled if accept_ra is enabled.
		  (accept_ra 启用时启用。)
		- disabled if accept_ra is disabled.
		  (accept_ra 禁用时禁用。)

ra_defrtr_metric - UNSIGNED INTEGER
	Route metric for default route learned in Router Advertisement. This value
	will be assigned as metric for the default route learned via IPv6 Router
	Advertisement. Takes affect only if accept_ra_defrtr is enabled.
	(从路由器通告学到的默认路由的路由度量。该值将作为经由 IPv6 路由器
	通告学到的默认路由的度量。仅在 accept_ra_defrtr 启用时生效。)

	Possible values:
		1 to 0xFFFFFFFF
		(可能的取值：1 到 0xFFFFFFFF)

		Default: IP6_RT_PRIO_USER i.e. 1024.
		(默认：IP6_RT_PRIO_USER，即 1024。)

accept_ra_from_local - BOOLEAN
	Accept RA with source-address that is found on local machine
	if the RA is otherwise proper and able to be accepted.
	(若 RA 其他方面正常且可接受，则接受源地址为本机地址的 RA。)

	Default is to NOT accept these as it may be an un-intended
	network loop.
	(默认不接受这类 RA，因为它们可能是非预期的网络环路。)

	Functional default:
	(实际默认值：)

	   - enabled if accept_ra_from_local is enabled
	     on a specific interface.
	     (accept_ra_from_local 在特定接口上启用时启用。)
	   - disabled if accept_ra_from_local is disabled
	     on a specific interface.
	     (accept_ra_from_local 在特定接口上禁用时禁用。)

accept_ra_min_hop_limit - INTEGER
	Minimum hop limit Information in Router Advertisement.
	(路由器通告中跳数限制信息的下限。)

	Hop limit Information in Router Advertisement less than this
	variable shall be ignored.
	(路由器通告中跳数限制信息小于该变量的将被忽略。)

	Default: 1
	(默认：1)

accept_ra_min_lft - INTEGER
	Minimum acceptable lifetime value in Router Advertisement.
	(路由器通告中可接受的最短生存期值。)

	RA sections with a lifetime less than this value shall be
	ignored. Zero lifetimes stay unaffected.
	(生存期小于该值的 RA 段将被忽略。零生存期不受影响。)

	Default: 0
	(默认：0)

accept_ra_pinfo - BOOLEAN
	Learn Prefix Information in Router Advertisement.
	(从路由器通告中学习前缀信息。)

	Functional default:
	(实际默认值：)

		- enabled if accept_ra is enabled.
		  (accept_ra 启用时启用。)
		- disabled if accept_ra is disabled.
		  (accept_ra 禁用时禁用。)

ra_honor_pio_life - BOOLEAN
	Whether to use RFC4862 Section 5.5.3e to determine the valid
	lifetime of an address matching a prefix sent in a Router
	Advertisement Prefix Information Option.
	(是否使用 RFC4862 第 5.5.3e 节来确定与路由器通告前缀信息选项
	（PIO）中前缀匹配的地址的有效生存期。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - RFC4862 section 5.5.3e is used to determine
	  the valid lifetime of the address.
	  (0（禁用）——使用 RFC4862 第 5.5.3e 节确定地址的有效生存期。)
	- 1 (enabled)  - the PIO valid lifetime will always be honored.
	  (1（启用）——始终采纳 PIO 的有效生存期。)

	Default: 0 (disabled)
	(默认：0（禁用）)

ra_honor_pio_pflag - BOOLEAN
	The Prefix Information Option P-flag indicates the network can
	allocate a unique IPv6 prefix per client using DHCPv6-PD.
	This sysctl can be enabled when a userspace DHCPv6-PD client
	is running to cause the P-flag to take effect: i.e. the
	P-flag suppresses any effects of the A-flag within the same
	PIO. For a given PIO, P=1 and A=1 is treated as A=0.
	(前缀信息选项的 P 标志表示网络可以使用 DHCPv6-PD 为每个客户端分配
	唯一的 IPv6 前缀。当用户态 DHCPv6-PD 客户端运行时可启用此 sysctl
	使 P 标志生效：即 P 标志抑制同一 PIO 中 A 标志的一切影响。对给定的
	PIO，P=1 且 A=1 时按 A=0 处理。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - the P-flag is ignored.
	  (0（禁用）——忽略 P 标志。)
	- 1 (enabled)  - the P-flag will disable SLAAC autoconfiguration
	  for the given Prefix Information Option.
	  (1（启用）——P 标志将禁用给定前缀信息选项的 SLAAC 自动配置。)

	Default: 0 (disabled)
	(默认：0（禁用）)

accept_ra_rt_info_min_plen - INTEGER
	Minimum prefix length of Route Information in RA.
	(RA 中路由信息（Route Information）的最小前缀长度。)

	Route Information w/ prefix smaller than this variable shall
	be ignored.
	(前缀小于该变量的路由信息将被忽略。)

	Functional default:
	(实际默认值：)

		* 0 if accept_ra_rtr_pref is enabled.
		  (accept_ra_rtr_pref 启用时为 0。)
		* -1 if accept_ra_rtr_pref is disabled.
		  (accept_ra_rtr_pref 禁用时为 -1。)

accept_ra_rt_info_max_plen - INTEGER
	Maximum prefix length of Route Information in RA.
	(RA 中路由信息的最大前缀长度。)

	Route Information w/ prefix larger than this variable shall
	be ignored.
	(前缀大于该变量的路由信息将被忽略。)

	Functional default:
	(实际默认值：)

		* 0 if accept_ra_rtr_pref is enabled.
		  (accept_ra_rtr_pref 启用时为 0。)
		* -1 if accept_ra_rtr_pref is disabled.
		  (accept_ra_rtr_pref 禁用时为 -1。)

accept_ra_rtr_pref - BOOLEAN
	Accept Router Preference in RA.
	(接受 RA 中的路由器偏好（Router Preference）。)

	Functional default:
	(实际默认值：)

		- enabled if accept_ra is enabled.
		  (accept_ra 启用时启用。)
		- disabled if accept_ra is disabled.
		  (accept_ra 禁用时禁用。)

accept_ra_mtu - BOOLEAN
	Apply the MTU value specified in RA option 5 (RFC4861). If
	disabled, the MTU specified in the RA will be ignored.
	(应用 RA 选项 5（RFC4861）中指定的 MTU 值。若禁用，RA 中指定的
	MTU 将被忽略。)

	Functional default:
	(实际默认值：)

		- enabled if accept_ra is enabled.
		  (accept_ra 启用时启用。)
		- disabled if accept_ra is disabled.
		  (accept_ra 禁用时禁用。)

accept_redirects - BOOLEAN
	Accept Redirects.
	(接受重定向。)

	Functional default:
	(实际默认值：)

		- enabled if local forwarding is disabled.
		  (本地转发禁用时启用。)
		- disabled if local forwarding is enabled.
		  (本地转发启用时禁用。)

accept_source_route - INTEGER
	Accept source routing (routing extension header).
	(接受源路由（路由扩展头）。)

	- >= 0: Accept only routing header type 2.
	  (>= 0：只接受路由头类型 2。)
	- < 0: Do not accept routing header.
	  (< 0：不接受路由头。)

	Default: 0
	(默认：0)

autoconf - BOOLEAN
	Autoconfigure addresses using Prefix Information in Router
	Advertisements.
	(使用路由器通告中的前缀信息自动配置地址。)

	Functional default:
	(实际默认值：)

		- enabled if accept_ra_pinfo is enabled.
		  (accept_ra_pinfo 启用时启用。)
		- disabled if accept_ra_pinfo is disabled.
		  (accept_ra_pinfo 禁用时禁用。)

dad_transmits - INTEGER
	The amount of Duplicate Address Detection probes to send.
	(要发送的重复地址检测（DAD）探测数量。)

	Default: 1
	(默认：1)

forwarding - INTEGER
	Configure interface-specific Host/Router behaviour.
	(配置按接口的主机/路由器行为。)

	.. note::

	   It is recommended to have the same setting on all
	   interfaces; mixed router/host scenarios are rather uncommon.
	   (注意：建议在所有接口上使用相同设置；路由器/主机混合的场景
	   相当少见。)

	Possible values are:
	(可能的取值为：)

		- 0 Forwarding disabled
		  (0 转发禁用)
		- 1 Forwarding enabled
		  (1 转发启用)

	**FALSE (0)**:
	(**FALSE (0)**：)

	By default, Host behaviour is assumed.  This means:
	(默认视为主机行为。这意味着：)

	1. IsRouter flag is not set in Neighbour Advertisements.
	   (1. 邻居通告（Neighbour Advertisements）中不设置 IsRouter 标志。)
	2. If accept_ra is TRUE (default), transmit Router
	   Solicitations.
	   (2. 若 accept_ra 为 TRUE（默认），发送路由器请求。)
	3. If accept_ra is TRUE (default), accept Router
	   Advertisements (and do autoconfiguration).
	   (3. 若 accept_ra 为 TRUE（默认），接受路由器通告（并进行
	   自动配置）。)
	4. If accept_redirects is TRUE (default), accept Redirects.
	   (4. 若 accept_redirects 为 TRUE（默认），接受重定向。)

	**TRUE (1)**:
	(**TRUE (1)**：)

	If local forwarding is enabled, Router behaviour is assumed.
	This means exactly the reverse from the above:
	(若本地转发已启用，则视为路由器行为。这意味着恰与上述相反：)

	1. IsRouter flag is set in Neighbour Advertisements.
	   (1. 邻居通告中设置 IsRouter 标志。)
	2. Router Solicitations are not sent unless accept_ra is 2.
	   (2. 除非 accept_ra 为 2，否则不发送路由器请求。)
	3. Router Advertisements are ignored unless accept_ra is 2.
	   (3. 除非 accept_ra 为 2，否则忽略路由器通告。)
	4. Redirects are ignored.
	   (4. 忽略重定向。)

	Default: 0 (disabled) if global forwarding is disabled (default),
	otherwise 1 (enabled).
	(若全局转发禁用（默认），默认为 0（禁用）；否则为 1（启用）。)

hop_limit - INTEGER
	Default Hop Limit to set.
	(要设置的默认跳数限制（Hop Limit）。)

	Default: 64
	(默认：64)

mtu - INTEGER
	Default Maximum Transfer Unit
	(默认最大传输单元（MTU）)

	Default: 1280 (IPv6 required minimum)
	(默认：1280（IPv6 要求的最小值）)

ip_nonlocal_bind - BOOLEAN
	If enabled, allows processes to bind() to non-local IPv6 addresses,
	which can be quite useful - but may break some applications.
	(若启用，允许进程 bind() 到非本地 IPv6 地址，这可能相当有用——但也
	可能破坏某些应用。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

router_probe_interval - INTEGER
	Minimum interval (in seconds) between Router Probing described
	in RFC4191.
	(RFC4191 所述路由器探测之间的最小间隔（秒）。)

	Default: 60
	(默认：60)

router_solicitation_delay - INTEGER
	Number of seconds to wait after interface is brought up
	before sending Router Solicitations.
	(接口启用后到发送路由器请求之前等待的秒数。)

	Default: 1
	(默认：1)

router_solicitation_interval - INTEGER
	Number of seconds to wait between Router Solicitations.
	(路由器请求之间的等待秒数。)

	Default: 4
	(默认：4)

router_solicitations - INTEGER
	Number of Router Solicitations to send until assuming no
	routers are present.
	(在假定不存在路由器之前要发送的路由器请求数量。)

	Default: 3
	(默认：3)

use_oif_addrs_only - BOOLEAN
	When enabled, the candidate source addresses for destinations
	routed via this interface are restricted to the set of addresses
	configured on this interface (vis. RFC 6724, section 4).
	(启用后，对经该接口路由的目的地，候选源地址被限制为配置在该接口上
	的地址集合（参见 RFC 6724 第 4 节）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

use_tempaddr - INTEGER
	Preference for Privacy Extensions (RFC3041).
	(隐私扩展（RFC3041）的偏好设置。)

	  * <= 0 : disable Privacy Extensions
	    (<= 0：禁用隐私扩展)
	  * == 1 : enable Privacy Extensions, but prefer public
	    addresses over temporary addresses.
	    (== 1：启用隐私扩展，但公共地址优先于临时地址。)
	  * >  1 : enable Privacy Extensions and prefer temporary
	    addresses over public addresses.
	    (> 1：启用隐私扩展，且临时地址优先于公共地址。)

	Default:
	(默认：)

		* 0 (for most devices)
		  (0（对大多数设备）)
		* -1 (for point-to-point devices and loopback devices)
		  (-1（对点对点设备和环回设备）)

temp_valid_lft - INTEGER
	valid lifetime (in seconds) for temporary addresses. If less than the
	minimum required lifetime (typically 5-7 seconds), temporary addresses
	will not be created.
	(临时地址的有效生存期（秒）。若小于所要求的最小生存期（通常 5-7 秒），
	则不会创建临时地址。)

	Default: 172800 (2 days)
	(默认：172800（2 天）)

temp_prefered_lft - INTEGER
	Preferred lifetime (in seconds) for temporary addresses. If
	temp_prefered_lft is less than the minimum required lifetime (typically
	5-7 seconds), the preferred lifetime is the minimum required. If
	temp_prefered_lft is greater than temp_valid_lft, the preferred lifetime
	is temp_valid_lft.
	(临时地址的首选生存期（秒）。若 temp_prefered_lft 小于所要求的最小
	生存期（通常 5-7 秒），首选生存期即为该最小要求值。若
	temp_prefered_lft 大于 temp_valid_lft，则首选生存期取 temp_valid_lft。)

	Default: 86400 (1 day)
	(默认：86400（1 天）)

keep_addr_on_down - INTEGER
	Keep all IPv6 addresses on an interface down event. If set static
	global addresses with no expiration time are not flushed.
	(接口 down 事件时保留所有 IPv6 地址。设置后，没有过期时间的静态
	全局地址不会被清除。)

	*   >0 : enabled (>0：启用)
	*    0 : system default (0：系统默认)
	*   <0 : disabled (<0：禁用)

	Default: 0 (addresses are removed)
	(默认：0（地址被移除）)

max_desync_factor - INTEGER
	Maximum value for DESYNC_FACTOR, which is a random value
	that ensures that clients don't synchronize with each
	other and generate new addresses at exactly the same time.
	value is in seconds.
	(DESYNC_FACTOR 的最大值。DESYNC_FACTOR 是一个随机值，用于确保客户端
	之间互不同步、不会在同一时刻生成新地址。单位为秒。)

	Default: 600
	(默认：600)

regen_min_advance - INTEGER
	How far in advance (in seconds), at minimum, to create a new temporary
	address before the current one is deprecated. This value is added to
	the amount of time that may be required for duplicate address detection
	to determine when to create a new address. Linux permits setting this
	value to less than the default of 2 seconds, but a value less than 2
	does not conform to RFC 8981.
	(在当前临时地址被弃用之前，至少提前多少秒（以秒计）创建新的临时地址。
	该值会加上重复地址检测可能需要的时间，以决定何时创建新地址。Linux
	允许把该值设为小于默认的 2 秒，但小于 2 的值不符合 RFC 8981。)

	Default: 2
	(默认：2)

regen_max_retry - INTEGER
	Number of attempts before give up attempting to generate
	valid temporary addresses.
	(放弃生成有效临时地址之前尝试的次数。)

	Default: 5
	(默认：5)

max_addresses - INTEGER
	Maximum number of autoconfigured addresses per interface.  Setting
	to zero disables the limitation.  It is not recommended to set this
	value too large (or to zero) because it would be an easy way to
	crash the kernel by allowing too many addresses to be created.
	(每个接口自动配置地址的最大数量。设为 0 将禁用该限制。不建议把此值
	设得过大（或为 0），因为那会是让内核因允许创建过多地址而轻易崩溃的
	途径。)

	Default: 16
	(默认：16)

disable_ipv6 - BOOLEAN
	Disable IPv6 operation.  If accept_dad is set to 2, this value
	will be dynamically set to TRUE if DAD fails for the link-local
	address.
	(禁用 IPv6 操作。若 accept_dad 设为 2，当链路本地地址的 DAD 失败时，
	该值会被动态设为 TRUE。)

	Default: FALSE (enable IPv6 operation)
	(默认：FALSE（启用 IPv6 操作）)

	When this value is changed from 1 to 0 (IPv6 is being enabled),
	it will dynamically create a link-local address on the given
	interface and start Duplicate Address Detection, if necessary.
	(当该值从 1 改为 0（正在启用 IPv6）时，会在给定接口上动态创建
	链路本地地址，并在必要时启动重复地址检测。)

	When this value is changed from 0 to 1 (IPv6 is being disabled),
	it will dynamically delete all addresses and routes on the given
	interface. From now on it will not possible to add addresses/routes
	to the selected interface.
	(当该值从 0 改为 1（正在禁用 IPv6）时，会动态删除给定接口上的所有
	地址和路由。此后将无法再向所选接口添加地址/路由。)

accept_dad - INTEGER
	Whether to accept DAD (Duplicate Address Detection).
	(是否接受 DAD（重复地址检测）。)

	 == ==============================================================
	  0  Disable DAD (禁用 DAD)
	  1  Enable DAD (default) (启用 DAD（默认）)
	  2  Enable DAD, and disable IPv6 operation if MAC-based duplicate
	     link-local address has been found. (启用 DAD；若发现基于 MAC 的重复链路本地地址，则禁用 IPv6 操作。)
	 == ==============================================================

	DAD operation and mode on a given interface will be selected according
	to the maximum value of conf/{all,interface}/accept_dad.
	(给定接口上的 DAD 操作与模式将根据 conf/{all,interface}/accept_dad
	的最大值来选择。)

force_tllao - BOOLEAN
	Enable sending the target link-layer address option even when
	responding to a unicast neighbor solicitation.
	(即使在响应单播邻居请求时也启用发送目标链路层地址选项。)

	Default: FALSE
	(默认：FALSE)

	Quoting from RFC 2461, section 4.4, Target link-layer address:
	(引用 RFC 2461 第 4.4 节"目标链路层地址"：)

	"The option MUST be included for multicast solicitations in order to
	avoid infinite Neighbor Solicitation "recursion" when the peer node
	does not have a cache entry to return a Neighbor Advertisements
	message.  When responding to unicast solicitations, the option can be
	omitted since the sender of the solicitation has the correct link-
	layer address; otherwise it would not have be able to send the unicast
	solicitation in the first place. However, including the link-layer
	address in this case adds little overhead and eliminates a potential
	race condition where the sender deletes the cached link-layer address
	prior to receiving a response to a previous solicitation."
	("组播请求中必须包含该选项，以避免在对端节点没有缓存条目返回邻居
	通告消息时出现无限的邻居请求"递归"。响应单播请求时可以省略该选项，
	因为请求的发送者已经拥有正确的链路层地址，否则它一开始就无法发出
	单播请求。不过，在这种情况下包含链路层地址开销很小，并能消除发送者
	在收到对先前请求的应答之前删除缓存链路层地址这一潜在竞态条件。")

ndisc_notify - BOOLEAN
	Define mode for notification of address and device changes.
	(定义地址与设备变更的通知模式。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - do nothing
	  (0（禁用）——不做任何事)
	- 1 (enabled)  - Generate unsolicited neighbour advertisements when device is brought
	  up or hardware address changes.
	  (1（启用）——设备启用或硬件地址变化时生成主动邻居通告。)

	Default: 0 (disabled)
	(默认：0（禁用）)

ndisc_tclass - INTEGER
	The IPv6 Traffic Class to use by default when sending IPv6 Neighbor
	Discovery (Router Solicitation, Router Advertisement, Neighbor
	Solicitation, Neighbor Advertisement, Redirect) messages.
	These 8 bits can be interpreted as 6 high order bits holding the DSCP
	value and 2 low order bits representing ECN (which you probably want
	to leave cleared).
	(发送 IPv6 邻居发现（路由器请求、路由器通告、邻居请求、邻居通告、
	重定向）消息时默认使用的 IPv6 流量类别。这 8 位可以解释为：高 6 位
	存放 DSCP 值，低 2 位表示 ECN（你可能希望保持为 0）。)

	* 0 - (default)
	  (0 -（默认）)

ndisc_evict_nocarrier - BOOLEAN
	Clears the neighbor discovery table on NOCARRIER events. This option is
	important for wireless devices where the neighbor discovery cache should
	not be cleared when roaming between access points on the same network.
	In most cases this should remain as the default (1).
	(在 NOCARRIER 事件时清除邻居发现表。此选项对无线设备很重要：在同一
	网络的接入点之间漫游时不应清除邻居发现缓存。大多数情况下应保持
	默认值（1）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - Do not clear neighbor discovery cache on NOCARRIER events.
	  (0（禁用）——NOCARRIER 事件时不清除邻居发现缓存。)
	- 1 (enabled)  - Clear neighbor discover cache on NOCARRIER events.
	  (1（启用）——NOCARRIER 事件时清除邻居发现缓存。)

	Default: 1 (enabled)
	(默认：1（启用）)

mldv1_unsolicited_report_interval - INTEGER
	The interval in milliseconds in which the next unsolicited
	MLDv1 report retransmit will take place.
	(下一次主动 MLDv1 报告重传的间隔，单位为毫秒。)

	Default: 10000 (10 seconds)
	(默认：10000（10 秒）)

mldv2_unsolicited_report_interval - INTEGER
	The interval in milliseconds in which the next unsolicited
	MLDv2 report retransmit will take place.
	(下一次主动 MLDv2 报告重传的间隔，单位为毫秒。)

	Default: 1000 (1 second)
	(默认：1000（1 秒）)

force_mld_version - INTEGER
	* 0 - (default) No enforcement of a MLD version, MLDv1 fallback allowed
	  (0 -（默认）不强制使用某个 MLD 版本，允许回退到 MLDv1)
	* 1 - Enforce to use MLD version 1
	  (1 - 强制使用 MLD 版本 1)
	* 2 - Enforce to use MLD version 2
	  (2 - 强制使用 MLD 版本 2)

suppress_frag_ndisc - INTEGER
	Control RFC 6980 (Security Implications of IPv6 Fragmentation
	with IPv6 Neighbor Discovery) behavior:
	(控制 RFC 6980（IPv6 分片与 IPv6 邻居发现的安全隐患）行为：)

	* 1 - (default) discard fragmented neighbor discovery packets
	  (1 -（默认）丢弃分片的邻居发现数据包)
	* 0 - allow fragmented neighbor discovery packets
	  (0 - 允许分片的邻居发现数据包)

optimistic_dad - BOOLEAN
	Whether to perform Optimistic Duplicate Address Detection (RFC 4429).
	(是否执行乐观重复地址检测（RFC 4429）。)

	Optimistic Duplicate Address Detection for the interface will be enabled
	if at least one of conf/{all,interface}/optimistic_dad is set to 1,
	it will be disabled otherwise.
	(只要 conf/{all,interface}/optimistic_dad 中至少有一个设为 1，
	接口的乐观重复地址检测就会启用；否则禁用。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)


use_optimistic - BOOLEAN
	If enabled, do not classify optimistic addresses as deprecated during
	source address selection.  Preferred addresses will still be chosen
	before optimistic addresses, subject to other ranking in the source
	address selection algorithm.
	(若启用，在源地址选择期间不把乐观地址归类为已弃用（deprecated）。
	在源地址选择算法的其他排序规则下，首选地址仍会先于乐观地址被选中。)

	This will be enabled if at least one of
	conf/{all,interface}/use_optimistic is set to 1, disabled otherwise.
	(只要 conf/{all,interface}/use_optimistic 中至少有一个设为 1，
	此项就会启用；否则禁用。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

stable_secret - IPv6 address
	This IPv6 address will be used as a secret to generate IPv6
	addresses for link-local addresses and autoconfigured
	ones. All addresses generated after setting this secret will
	be stable privacy ones by default. This can be changed via the
	addrgenmode ip-link. conf/default/stable_secret is used as the
	secret for the namespace, the interface specific ones can
	overwrite that. Writes to conf/all/stable_secret are refused.
	(此 IPv6 地址将作为生成链路本地地址与自动配置地址时使用的密钥
	（secret）。设置该密钥之后生成的所有地址默认都是稳定隐私地址。
	可以通过 addrgenmode ip-link 更改。conf/default/stable_secret 用作
	该命名空间的密钥，各接口的设置可以覆盖它。对
	conf/all/stable_secret 的写入会被拒绝。)

	It is recommended to generate this secret during installation
	of a system and keep it stable after that.
	(建议在安装系统时生成该密钥，并在之后保持其稳定。)

	By default the stable secret is unset.
	(默认不设置稳定密钥。)

addr_gen_mode - INTEGER
	Defines how link-local and autoconf addresses are generated.
	(定义链路本地地址与 autoconf 地址的生成方式。)

	=  =================================================================
	0  generate address based on EUI64 (default) (基于 EUI64 生成地址（默认）)
	1  do no generate a link-local address, use EUI64 for addresses
	   generated from autoconf (不生成链路本地地址；对 autoconf 生成的地址使用 EUI64)
	2  generate stable privacy addresses, using the secret from
	   stable_secret (RFC7217) (使用 stable_secret 中的密钥生成稳定隐私地址（RFC7217）)
	3  generate stable privacy addresses, using a random secret if unset (若未设置密钥则使用随机密钥生成稳定隐私地址)
	=  =================================================================

drop_unicast_in_l2_multicast - BOOLEAN
	Drop any unicast IPv6 packets that are received in link-layer
	multicast (or broadcast) frames.
	(丢弃任何在链路层组播（或广播）帧中收到的单播 IPv6 数据包。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

drop_unsolicited_na - BOOLEAN
	Drop all unsolicited neighbor advertisements, for example if there's
	a known good NA proxy on the network and such frames need not be used
	(or in the case of 802.11, must not be used to prevent attacks.)
	(丢弃所有主动邻居通告（NA），例如当网络上存在已知良好的 NA 代理而
	不需要使用这类帧时（或在 802.11 场景下，为防止攻击而必须不使用）。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled).
	(默认：0（禁用）。)

accept_untracked_na - INTEGER
	Define behavior for accepting neighbor advertisements from devices that
	are absent in the neighbor cache:
	(定义接受邻居缓存中不存在的设备发来的邻居通告（NA）时的行为：)

	- 0 - (default) Do not accept unsolicited and untracked neighbor
	  advertisements.
	  (0 -（默认）不接受主动且未跟踪的邻居通告。)

	- 1 - Add a new neighbor cache entry in STALE state for routers on
	  receiving a neighbor advertisement (either solicited or unsolicited)
	  with target link-layer address option specified if no neighbor entry
	  is already present for the advertised IPv6 address. Without this knob,
	  NAs received for untracked addresses (absent in neighbor cache) are
	  silently ignored.
	  (1 - 路由器在收到带目标链路层地址选项的邻居通告（无论请求触发还是
	  主动发送）时，若所通告的 IPv6 地址尚无邻居表项，则在 STALE 状态下
	  添加新的邻居缓存表项。没有此开关时，针对未跟踪地址（邻居缓存中
	  不存在）收到的 NA 会被静默忽略。)

	  This is as per router-side behavior documented in RFC9131.
	  (这是 RFC9131 中记录的路由器侧行为。)

	  This has lower precedence than drop_unsolicited_na.
	  (此选项的优先级低于 drop_unsolicited_na。)

	  This will optimize the return path for the initial off-link
	  communication that is initiated by a directly connected host, by
	  ensuring that the first-hop router which turns on this setting doesn't
	  have to buffer the initial return packets to do neighbor-solicitation.
	  The prerequisite is that the host is configured to send unsolicited
	  neighbor advertisements on interface bringup. This setting should be
	  used in conjunction with the ndisc_notify setting on the host to
	  satisfy this prerequisite.
	  (这可以优化直连主机发起的初始跨链路通信的返回路径：确保开启此设置
	  的第一跳路由器不必缓存初始返回数据包来做邻居请求。前提是主机配置为
	  在接口启动时发送主动邻居通告。此设置应与主机上的 ndisc_notify 设置
	  配合使用以满足该前提。)

	- 2 - Extend option (1) to add a new neighbor cache entry only if the
	  source IP address is in the same subnet as an address configured on
	  the interface that received the neighbor advertisement.
	  (2 - 扩展选项（1）：仅当源 IP 地址与接收到邻居通告的接口上配置的
	  某地址位于同一子网时，才添加新的邻居缓存表项。)

enhanced_dad - BOOLEAN
	Include a nonce option in the IPv6 neighbor solicitation messages used for
	duplicate address detection per RFC7527. A received DAD NS will only signal
	a duplicate address if the nonce is different. This avoids any false
	detection of duplicates due to loopback of the NS messages that we send.
	The nonce option will be sent on an interface unless both of
	conf/{all,interface}/enhanced_dad are set to FALSE.
	(按 RFC7527，在用于重复地址检测的 IPv6 邻居请求消息中包含 nonce 选项。
	只有当收到的 DAD NS 中 nonce 不同时，才判定存在重复地址。这避免了因
	我们发送的 NS 消息被环回而导致的误判。除非 conf/{all,interface}/
	enhanced_dad 都设为 FALSE，否则接口上会发送 nonce 选项。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)

``icmp/*``:(``icmp/*``：)
=========================

ratelimit - INTEGER
	Limit the maximal rates for sending ICMPv6 messages to a particular
	peer.
	(限制向特定对端发送 ICMPv6 消息的最大速率。)

	0 to disable any limiting,
	otherwise the space between responses in milliseconds.
	(0 表示禁用所有限速；否则为响应之间的最小间隔，单位为毫秒。)

	Default: 100
	(默认：100)

ratemask - list of comma separated ranges
	For ICMPv6 message types matching the ranges in the ratemask, limit
	the sending of the message according to ratelimit parameter.
	(对类型匹配 ratemask 中范围的 ICMPv6 消息，按 ratelimit 参数限制
	其发送。)

	The format used for both input and output is a comma separated
	list of ranges (e.g. "0-127,129" for ICMPv6 message type 0 to 127 and
	129). Writing to the file will clear all previous ranges of ICMPv6
	message types and update the current list with the input.
	(输入与输出使用的格式都是以逗号分隔的范围列表（例如 "0-127,129"
	表示 ICMPv6 消息类型 0 到 127 以及 129）。写入该文件会清除先前的
	所有 ICMPv6 消息类型范围，并用输入更新当前列表。)

	Refer to: https://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml
	for numerical values of ICMPv6 message types, e.g. echo request is 128
	and echo reply is 129.
	(ICMPv6 消息类型的数值请参考：
	https://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml，
	例如 echo request 是 128，echo reply 是 129。)

	Default: 0-1,3-127 (rate limit ICMPv6 errors except Packet Too Big)
	(默认：0-1,3-127（对除 Packet Too Big 之外的 ICMPv6 错误限速）)

echo_ignore_all - BOOLEAN
	If enabled, then the kernel will ignore all ICMP ECHO
	requests sent to it over the IPv6 protocol.
	(若启用，内核将忽略通过 IPv6 协议发给它的所有 ICMP ECHO 请求。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

echo_ignore_multicast - BOOLEAN
	If enabled, then the kernel will ignore all ICMP ECHO
	requests sent to it over the IPv6 protocol via multicast.
	(若启用，内核将忽略通过 IPv6 协议经组播发给它的所有 ICMP ECHO 请求。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

echo_ignore_anycast - BOOLEAN
	If enabled, then the kernel will ignore all ICMP ECHO
	requests sent to it over the IPv6 protocol destined to anycast address.
	(若启用，内核将忽略通过 IPv6 协议发给它、目的地为任播地址的所有
	ICMP ECHO 请求。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

error_anycast_as_unicast - BOOLEAN
	If enabled, then the kernel will respond with ICMP Errors
	resulting from requests sent to it over the IPv6 protocol destined
	to anycast address essentially treating anycast as unicast.
	(若启用，对于通过 IPv6 协议发给它、目的地为任播地址的请求所引发的
	ICMP 错误，内核将照常回应，本质上把任播当作单播处理。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 0 (disabled)
	(默认：0（禁用）)

errors_extension_mask - UNSIGNED INTEGER
	Bitmask of ICMP extensions to append to ICMPv6 error messages
	("Destination Unreachable" and "Time Exceeded"). The original datagram
	is trimmed / padded to 128 bytes in order to be compatible with
	applications that do not comply with RFC 4884.
	(要附加到 ICMPv6 错误消息（"Destination Unreachable" 与 "Time
	Exceeded"）的 ICMP 扩展位掩码。为兼容不符合 RFC 4884 的应用，原始
	数据报会被裁剪/填充到 128 字节。)

	Possible extensions are:
	(可能的扩展有：)

	==== ==============================================================
	0x01 Incoming IP interface information according to RFC 5837.
	     Extension will include the index, IPv6 address (if present),
	     name and MTU of the IP interface that received the datagram
	     which elicited the ICMP error. (按 RFC 5837 提供入站 IP 接口信息。扩展将包含接收到引发该 ICMP 错误的数据报的 IP 接口的索引、IPv6 地址（如存在）、名称和 MTU。)
	==== ==============================================================

	Default: 0x00 (no extensions)
	(默认：0x00（无扩展）)

xfrm6_gc_thresh - INTEGER
	(Obsolete since linux-4.14)
	The threshold at which we will start garbage collecting for IPv6
	destination cache entries.  At twice this value the system will
	refuse new allocations.
	(（linux-4.14 起已过时）
	开始对 IPv6 目的缓存条目进行垃圾回收的阈值。达到该值的两倍时，
	系统将拒绝新的分配。)


IPv6 Update by:
(IPv6 更新者：)
Pekka Savola <pekkas@netcore.fi>
YOSHIFUJI Hideaki / USAGI Project <yoshfuji@linux-ipv6.org>


/proc/sys/net/bridge/* Variables:(/proc/sys/net/bridge/* 变量：)
================================================================

bridge-nf-call-arptables - BOOLEAN

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable this.
	  (0（禁用）——禁用此功能。)
	- 1 (enabled)  - pass bridged ARP traffic to arptables' FORWARD chain.
	  (1（启用）——把桥接的 ARP 流量交给 arptables 的 FORWARD 链。)

	Default: 1 (enabled)
	(默认：1（启用）)

bridge-nf-call-iptables - BOOLEAN

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable this.
	  (0（禁用）——禁用此功能。)
	- 1 (enabled)  - pass bridged IPv4 traffic to iptables' chains.
	  (1（启用）——把桥接的 IPv4 流量交给 iptables 的链。)

	Default: 1 (enabled)
	(默认：1（启用）)

bridge-nf-call-ip6tables - BOOLEAN

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable this.
	  (0（禁用）——禁用此功能。)
	- 1 (enabled)  - pass bridged IPv6 traffic to ip6tables' chains.
	  (1（启用）——把桥接的 IPv6 流量交给 ip6tables 的链。)

	Default: 1 (enabled)
	(默认：1（启用）)

bridge-nf-filter-vlan-tagged - BOOLEAN

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable this.
	  (0（禁用）——禁用此功能。)
	- 1 (enabled)  - pass bridged vlan-tagged ARP/IP/IPv6 traffic to {arp,ip,ip6}tables
	  (1（启用）——把桥接的带 vlan 标签 ARP/IP/IPv6 流量交给 {arp,ip,ip6}tables)

	Default: 0 (disabled)
	(默认：0（禁用）)

bridge-nf-filter-pppoe-tagged - BOOLEAN

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable this.
	  (0（禁用）——禁用此功能。)
	- 1 (enabled)  - pass bridged pppoe-tagged IP/IPv6 traffic to {ip,ip6}tables.
	  (1（启用）——把桥接的带 pppoe 标签 IP/IPv6 流量交给 {ip,ip6}tables。)

	Default: 0 (disabled)
	(默认：0（禁用）)

bridge-nf-pass-vlan-input-dev - BOOLEAN
	- 1: if bridge-nf-filter-vlan-tagged is enabled, try to find a vlan
	  interface on the bridge and set the netfilter input device to the
	  vlan. This allows use of e.g. "iptables -i br0.1" and makes the
	  REDIRECT target work with vlan-on-top-of-bridge interfaces.  When no
	  matching vlan interface is found, or this switch is off, the input
	  device is set to the bridge interface.
	  (1：若 bridge-nf-filter-vlan-tagged 已启用，尝试在桥上查找 vlan
	  接口并把 netfilter 输入设备设置为该 vlan。这样可以使用例如
	  "iptables -i br0.1"，并让 REDIRECT 目标对桥上叠加的 vlan 接口生效。
	  当找不到匹配的 vlan 接口，或此开关关闭时，输入设备被设置为桥接口。)

	- 0: disable bridge netfilter vlan interface lookup.
	  (0：禁用桥 netfilter 的 vlan 接口查找。)

	Default: 0
	(默认：0)

``proc/sys/net/sctp/*`` Variables:(``proc/sys/net/sctp/*`` 变量：)
==================================================================

addip_enable - BOOLEAN
	Enable or disable extension of  Dynamic Address Reconfiguration
	(ADD-IP) functionality specified in RFC5061.  This extension provides
	the ability to dynamically add and remove new addresses for the SCTP
	associations.
	(启用或禁用 RFC5061 规定的动态地址重配置（ADD-IP）功能扩展。该扩展
	提供为 SCTP 关联（association）动态添加和删除新地址的能力。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable extension.
	  (0（禁用）——禁用扩展。)
	- 1 (enabled)  - enable extension
	  (1（启用）——启用扩展)

	Default: 0 (disabled)
	(默认：0（禁用）)

pf_enable - INTEGER
	Enable or disable pf (pf is short for potentially failed) state. A value
	of pf_retrans > path_max_retrans also disables pf state. That is, one of
	both pf_enable and pf_retrans > path_max_retrans can disable pf state.
	Since pf_retrans and path_max_retrans can be changed by userspace
	application, sometimes user expects to disable pf state by the value of
	pf_retrans > path_max_retrans, but occasionally the value of pf_retrans
	or path_max_retrans is changed by the user application, this pf state is
	enabled. As such, it is necessary to add this to dynamically enable
	and disable pf state. See:
	https://datatracker.ietf.org/doc/draft-ietf-tsvwg-sctp-failover for
	details.
	(启用或禁用 pf（pf 是 potentially failed 的缩写）状态。当
	pf_retrans > path_max_retrans 时也会禁用 pf 状态。也就是说，pf_enable
	与 pf_retrans > path_max_retrans 任意之一都可以禁用 pf 状态。由于
	pf_retrans 与 path_max_retrans 可以被用户态应用修改，有时用户希望通过
	pf_retrans > path_max_retrans 的取值来禁用 pf 状态，但偶尔用户应用
	修改了 pf_retrans 或 path_max_retrans 的值后，pf 状态又被启用了。
	因此，需要增加此项来动态地启用和禁用 pf 状态。详见：
	https://datatracker.ietf.org/doc/draft-ietf-tsvwg-sctp-failover。)

	Possible values:
	(可能的取值：)

	- 1: Enable pf.
	  (1：启用 pf。)
	- 0: Disable pf.
	  (0：禁用 pf。)

	Default: 1
	(默认：1)

pf_expose - INTEGER
	Unset or enable/disable pf (pf is short for potentially failed) state
	exposure.  Applications can control the exposure of the PF path state
	in the SCTP_PEER_ADDR_CHANGE event and access of SCTP_PF-state
	transport info via SCTP_GET_PEER_ADDR_INFO sockopt.
	(取消设置或启用/禁用 pf（pf 是 potentially failed 的缩写）状态的
	暴露。应用可以通过 SCTP_PEER_ADDR_CHANGE 事件控制 PF 路径状态的
	暴露，并通过 SCTP_GET_PEER_ADDR_INFO sockopt 访问 SCTP_PF 状态的
	传输信息。)

	Possible values:
	(可能的取值：)

	- 0: Unset pf state exposure (compatible with old applications). No
	  event will be sent but the transport info can be queried.
	  (0：不设置 pf 状态暴露（兼容旧应用）。不发送事件，但可以查询
	  传输信息。)
	- 1: Disable pf state exposure. No event will be sent and trying to
	  obtain transport info will return -EACCESS.
	  (1：禁用 pf 状态暴露。不发送事件，尝试获取传输信息将返回
	  -EACCESS。)
	- 2: Enable pf state exposure. The event will be sent for a transport
	  becoming SCTP_PF state and transport info can be obtained.
	  (2：启用 pf 状态暴露。传输进入 SCTP_PF 状态时会发送事件，且可以
	  获取传输信息。)

	Default: 0
	(默认：0)

addip_noauth_enable - BOOLEAN
	Dynamic Address Reconfiguration (ADD-IP) requires the use of
	authentication to protect the operations of adding or removing new
	addresses.  This requirement is mandated so that unauthorized hosts
	would not be able to hijack associations.  However, older
	implementations may not have implemented this requirement while
	allowing the ADD-IP extension.  For reasons of interoperability,
	we provide this variable to control the enforcement of the
	authentication requirement.
	(动态地址重配置（ADD-IP）要求使用认证来保护添加或删除新地址的操作。
	强制这一要求是为了让未授权主机无法劫持关联。不过，较旧的实现可能
	没有实现这一要求却允许 ADD-IP 扩展。出于互操作性的考虑，我们提供
	此变量来控制是否强制执行认证要求。)

	== ===============================================================
	1  Allow ADD-IP extension to be used without authentication.  This
	   should only be set in a closed environment for interoperability
	   with older implementations. (允许在不认证的情况下使用 ADD-IP 扩展。只应在封闭环境中为与旧实现互操作而设置。)

	0  Enforce the authentication requirement (强制执行认证要求)
	== ===============================================================

	Default: 0
	(默认：0)

auth_enable - BOOLEAN
	Enable or disable Authenticated Chunks extension.  This extension
	provides the ability to send and receive authenticated chunks and is
	required for secure operation of Dynamic Address Reconfiguration
	(ADD-IP) extension.
	(启用或禁用认证块（Authenticated Chunks）扩展。该扩展提供发送和
	接收经过认证的数据块的能力，是动态地址重配置（ADD-IP）扩展安全
	运行所必需的。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable extension.
	  (0（禁用）——禁用扩展。)
	- 1 (enabled)  - enable extension
	  (1（启用）——启用扩展)

	Default: 0 (disabled)
	(默认：0（禁用）)

prsctp_enable - BOOLEAN
	Enable or disable the Partial Reliability extension (RFC3758) which
	is used to notify peers that a given DATA should no longer be expected.
	(启用或禁用部分可靠性扩展（RFC3758），该扩展用于通知对端不再期待
	某个给定的 DATA。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable extension.
	  (0（禁用）——禁用扩展。)
	- 1 (enabled)  - enable extension
	  (1（启用）——启用扩展)

	Default: 1 (enabled)
	(默认：1（启用）)

max_burst - INTEGER
	The limit of the number of new packets that can be initially sent.  It
	controls how bursty the generated traffic can be.
	(初始可发送的新数据包数量限制。它控制生成流量的突发程度。)

	Default: 4
	(默认：4)

association_max_retrans - INTEGER
	Set the maximum number for retransmissions that an association can
	attempt deciding that the remote end is unreachable.  If this value
	is exceeded, the association is terminated.
	(设置关联在判定远端不可达之前可以尝试的最大重传次数。超过此值时，
	关联被终止。)

	Default: 10
	(默认：10)

max_init_retransmits - INTEGER
	The maximum number of retransmissions of INIT and COOKIE-ECHO chunks
	that an association will attempt before declaring the destination
	unreachable and terminating.
	(关联在宣告目的地不可达并终止之前，对 INIT 与 COOKIE-ECHO 块尝试的
	最大重传次数。)

	Default: 8
	(默认：8)

path_max_retrans - INTEGER
	The maximum number of retransmissions that will be attempted on a given
	path.  Once this threshold is exceeded, the path is considered
	unreachable, and new traffic will use a different path when the
	association is multihomed.
	(在给定路径上尝试的最大重传次数。一旦超过该阈值，该路径被视为
	不可达；当关联为多宿主（multihomed）时，新流量将使用其他路径。)

	Default: 5
	(默认：5)

pf_retrans - INTEGER
	The number of retransmissions that will be attempted on a given path
	before traffic is redirected to an alternate transport (should one
	exist).  Note this is distinct from path_max_retrans, as a path that
	passes the pf_retrans threshold can still be used.  Its only
	deprioritized when a transmission path is selected by the stack.  This
	setting is primarily used to enable fast failover mechanisms without
	having to reduce path_max_retrans to a very low value.  See:
	http://www.ietf.org/id/draft-nishida-tsvwg-sctp-failover-05.txt
	for details.  Note also that a value of pf_retrans > path_max_retrans
	disables this feature. Since both pf_retrans and path_max_retrans can
	be changed by userspace application, a variable pf_enable is used to
	disable pf state.
	(在流量被重定向到备用传输（如果存在）之前，在给定路径上尝试的重传
	次数。注意，它与 path_max_retrans 不同：超过 pf_retrans 阈值的路径
	仍然可用，只是在协议栈选择传输路径时被降级。此设置主要用于启用快速
	故障切换机制，而不必把 path_max_retrans 调到很低的值。详见：
	http://www.ietf.org/id/draft-nishida-tsvwg-sctp-failover-05.txt。
	另请注意，pf_retrans > path_max_retrans 会禁用此特性。由于
	pf_retrans 与 path_max_retrans 都可以被用户态应用修改，因此引入变量
	pf_enable 来禁用 pf 状态。)

	Default: 0
	(默认：0)

ps_retrans - INTEGER
	Primary.Switchover.Max.Retrans (PSMR), it's a tunable parameter coming
	from section-5 "Primary Path Switchover" in rfc7829.  The primary path
	will be changed to another active path when the path error counter on
	the old primary path exceeds PSMR, so that "the SCTP sender is allowed
	to continue data transmission on a new working path even when the old
	primary destination address becomes active again".   Note this feature
	is disabled by initializing 'ps_retrans' per netns as 0xffff by default,
	and its value can't be less than 'pf_retrans' when changing by sysctl.
	(Primary.Switchover.Max.Retrans（PSMR），是来自 rfc7829 第 5 节
	"Primary Path Switchover" 的可调参数。当旧主路径上的路径错误计数器
	超过 PSMR 时，主路径将切换到另一条活动路径，以便"SCTP 发送端即使
	在旧主目的地址恢复活动后，也能继续在新工作路径上传输数据"。注意，
	该特性默认通过把每个 netns 的 'ps_retrans' 初始化为 0xffff 而禁用，
	并且通过 sysctl 修改时其值不能小于 'pf_retrans'。)

	Default: 0xffff
	(默认：0xffff)

rto_initial - INTEGER
	The initial round trip timeout value in milliseconds that will be used
	in calculating round trip times.  This is the initial time interval
	for retransmissions.
	(用于计算往返时间的初始往返超时值，单位为毫秒。这是重传的初始时间
	间隔。)

	Default: 3000
	(默认：3000)

rto_max - INTEGER
	The maximum value (in milliseconds) of the round trip timeout.  This
	is the largest time interval that can elapse between retransmissions.
	(往返超时的最大值（毫秒）。这是重传之间可以经过的最大时间间隔。)

	Default: 60000
	(默认：60000)

rto_min - INTEGER
	The minimum value (in milliseconds) of the round trip timeout.  This
	is the smallest time interval the can elapse between retransmissions.
	(往返超时的最小值（毫秒）。这是重传之间可以经过的最小时间间隔。)

	Default: 1000
	(默认：1000)

hb_interval - INTEGER
	The interval (in milliseconds) between HEARTBEAT chunks.  These chunks
	are sent at the specified interval on idle paths to probe the state of
	a given path between 2 associations.
	(HEARTBEAT 块之间的间隔（毫秒）。这些块按指定间隔在空闲路径上发送，
	用于探测两个关联之间给定路径的状态。)

	Default: 30000
	(默认：30000)

sack_timeout - INTEGER
	The amount of time (in milliseconds) that the implementation will wait
	to send a SACK.
	(实现等待发送 SACK 的时间（毫秒）。)

	Default: 200
	(默认：200)

valid_cookie_life - INTEGER
	The default lifetime of the SCTP cookie (in milliseconds).  The cookie
	is used during association establishment.
	(SCTP cookie 的默认生存期（毫秒）。cookie 在关联建立过程中使用。)

	Default: 60000
	(默认：60000)

cookie_preserve_enable - BOOLEAN
	Enable or disable the ability to extend the lifetime of the SCTP cookie
	that is used during the establishment phase of SCTP association
	(启用或禁用延长 SCTP cookie 生存期的能力，该 cookie 在 SCTP 关联的
	建立阶段使用)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - disable.
	  (0（禁用）——禁用。)
	- 1 (enabled)  - enable cookie lifetime extension.
	  (1（启用）——启用 cookie 生存期延长。)

	Default: 1 (enabled)
	(默认：1（启用）)

cookie_hmac_alg - STRING
	Select the hmac algorithm used when generating the cookie value sent by
	a listening sctp socket to a connecting client in the INIT-ACK chunk.
	Valid values are:
	(选择在 INIT-ACK 块中生成监听 sctp 套接字发给连接客户端的 cookie 值
	时使用的 hmac 算法。有效值为：)

	* sha256
	  (sha256)
	* none
	  (none)

	Default: sha256
	(默认：sha256)

rcvbuf_policy - INTEGER
	Determines if the receive buffer is attributed to the socket or to
	association.   SCTP supports the capability to create multiple
	associations on a single socket.  When using this capability, it is
	possible that a single stalled association that's buffering a lot
	of data may block other associations from delivering their data by
	consuming all of the receive buffer space.  To work around this,
	the rcvbuf_policy could be set to attribute the receiver buffer space
	to each association instead of the socket.  This prevents the described
	blocking.
	(决定接收缓冲归属于套接字还是关联。SCTP 支持在单个套接字上创建多个
	关联。使用该能力时，单个缓冲大量数据的停滞关联可能因占用全部接收
	缓冲空间而阻塞其他关联发送数据。为规避这一问题，可以把 rcvbuf_policy
	设为把接收缓冲空间归属于每个关联而不是套接字。这可以防止上述阻塞。)

	- 1: rcvbuf space is per association
	  (1：rcvbuf 空间按关联计)
	- 0: rcvbuf space is per socket
	  (0：rcvbuf 空间按套接字计)

	Default: 0
	(默认：0)

sndbuf_policy - INTEGER
	Similar to rcvbuf_policy above, this applies to send buffer space.
	(与上面的 rcvbuf_policy 类似，但作用于发送缓冲空间。)

	- 1: Send buffer is tracked per association
	  (1：发送缓冲按关联跟踪)
	- 0: Send buffer is tracked per socket.
	  (0：发送缓冲按套接字跟踪。)

	Default: 0
	(默认：0)

sctp_mem - vector of 3 INTEGERs: min, pressure, max
	Number of pages allowed for queueing by all SCTP sockets.
	(允许所有 SCTP 套接字用于排队的页数。)

	* min: Below this number of pages SCTP is not bothered about its
	  memory usage. When amount of memory allocated by SCTP exceeds
	  this number, SCTP starts to moderate memory usage.
	  (min：低于该页数时，SCTP 不必担心内存使用。当 SCTP 分配的内存
	  超过该数量时，SCTP 开始节制内存使用。)
	* pressure: This value was introduced to follow format of tcp_mem.
	  (pressure：引入此值只是为了与 tcp_mem 的格式保持一致。)
	* max: Maximum number of allowed pages.
	  (max：允许的最大页数。)

	Default is calculated at boot time from amount of available memory.
	(默认值在引导时根据可用内存量计算。)

sctp_rmem - vector of 3 INTEGERs: min, default, max
	Only the first value ("min") is used, "default" and "max" are
	ignored.
	(只使用第一个值（"min"），"default" 与 "max" 被忽略。)

	* min: Minimal size of receive buffer used by SCTP socket.
	  It is guaranteed to each SCTP socket (but not association) even
	  under moderate memory pressure.
	  (min：SCTP 套接字接收缓冲的最小大小。即使在中等的内存压力下，
	  每个 SCTP 套接字（但不是关联）也能得到保证。)

	Default: 4K
	(默认：4K)

sctp_wmem  - vector of 3 INTEGERs: min, default, max
	Only the first value ("min") is used, "default" and "max" are
	ignored.
	(只使用第一个值（"min"），"default" 与 "max" 被忽略。)

	* min: Minimum size of send buffer that can be used by SCTP sockets.
	  It is guaranteed to each SCTP socket (but not association) even
	  under moderate memory pressure.
	  (min：SCTP 套接字可使用的发送缓冲最小大小。即使在中等的内存压力下，
	  每个 SCTP 套接字（但不是关联）也能得到保证。)

	Default: 4K
	(默认：4K)

addr_scope_policy - INTEGER
	Control IPv4 address scoping (see
	https://datatracker.ietf.org/doc/draft-stewart-tsvwg-sctp-ipv4/00/
	for details).
	(控制 IPv4 地址作用域（scope）（详见
	https://datatracker.ietf.org/doc/draft-stewart-tsvwg-sctp-ipv4/00/）。)

	- 0   - Disable IPv4 address scoping
	  (0 - 禁用 IPv4 地址作用域)
	- 1   - Enable IPv4 address scoping
	  (1 - 启用 IPv4 地址作用域)
	- 2   - Follow draft but allow IPv4 private addresses
	  (2 - 遵循草案但允许 IPv4 私有地址)
	- 3   - Follow draft but allow IPv4 link local addresses
	  (3 - 遵循草案但允许 IPv4 链路本地地址)

	Default: 1
	(默认：1)

udp_port - INTEGER
	The listening port for the local UDP tunneling sock. Normally it's
	using the IANA-assigned UDP port number 9899 (sctp-tunneling).
	(本地 UDP 隧道套接字的监听端口。通常使用 IANA 分配的 UDP 端口号
	9899（sctp-tunneling）。)

	This UDP sock is used for processing the incoming UDP-encapsulated
	SCTP packets (from RFC6951), and shared by all applications in the
	same net namespace. This UDP sock will be closed when the value is
	set to 0.
	(该 UDP 套接字用于处理入站的 UDP 封装 SCTP 数据包（来自 RFC6951），
	并由同一网络命名空间中的所有应用共享。该值设为 0 时，此 UDP 套接字
	将被关闭。)

	The value will also be used to set the src port of the UDP header
	for the outgoing UDP-encapsulated SCTP packets. For the dest port,
	please refer to 'encap_port' below.
	(该值还将用于为发出的 UDP 封装 SCTP 数据包设置 UDP 头部的源端口。
	目的端口请参见下文的 'encap_port'。)

	Default: 0
	(默认：0)

encap_port - INTEGER
	The default remote UDP encapsulation port.
	(默认的远端 UDP 封装端口。)

	This value is used to set the dest port of the UDP header for the
	outgoing UDP-encapsulated SCTP packets by default. Users can also
	change the value for each sock/asoc/transport by using setsockopt.
	For further information, please refer to RFC6951.
	(默认情况下，该值用于为发出的 UDP 封装 SCTP 数据包设置 UDP 头部的
	目的端口。用户也可以通过 setsockopt 为每个 sock/asoc/transport
	更改该值。更多信息请参考 RFC6951。)

	Note that when connecting to a remote server, the client should set
	this to the port that the UDP tunneling sock on the peer server is
	listening to and the local UDP tunneling sock on the client also
	must be started. On the server, it would get the encap_port from
	the incoming packet's source port.
	(注意，连接远端服务器时，客户端应把此值设置为对端服务器上 UDP 隧道
	套接字的监听端口，且客户端本地的 UDP 隧道套接字也必须已启动。
	在服务器上，encap_port 会从入站数据包的源端口获得。)

	Default: 0
	(默认：0)

plpmtud_probe_interval - INTEGER
        The time interval (in milliseconds) for the PLPMTUD probe timer,
        which is configured to expire after this period to receive an
        acknowledgment to a probe packet. This is also the time interval
        between the probes for the current pmtu when the probe search
        is done.
        (PLPMTUD 探测定时器的时间间隔（毫秒），定时器在此周期后到期，
        以等待探测数据包的确认。这也是进行探测搜索时针对当前 pmtu 的
        各次探测之间的时间间隔。)

        PLPMTUD will be disabled when 0 is set, and other values for it
        must be >= 5000.
        (设为 0 时禁用 PLPMTUD，其他取值必须 >= 5000。)

	Default: 0
	(默认：0)

reconf_enable - BOOLEAN
        Enable or disable extension of Stream Reconfiguration functionality
        specified in RFC6525. This extension provides the ability to "reset"
        a stream, and it includes the Parameters of "Outgoing/Incoming SSN
        Reset", "SSN/TSN Reset" and "Add Outgoing/Incoming Streams".
        (启用或禁用 RFC6525 规定的流重配置（Stream Reconfiguration）功能
        扩展。该扩展提供"重置"流的能力，包含"Outgoing/Incoming SSN
        Reset"、"SSN/TSN Reset" 与 "Add Outgoing/Incoming Streams"
        等参数。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - Disable extension.
	  (0（禁用）——禁用扩展。)
	- 1 (enabled) - Enable extension.
	  (1（启用）——启用扩展。)

	Default: 0 (disabled)
	(默认：0（禁用）)

intl_enable - BOOLEAN
        Enable or disable extension of User Message Interleaving functionality
        specified in RFC8260. This extension allows the interleaving of user
        messages sent on different streams. With this feature enabled, I-DATA
        chunk will replace DATA chunk to carry user messages if also supported
        by the peer. Note that to use this feature, one needs to set this option
        to 1 and also needs to set socket options SCTP_FRAGMENT_INTERLEAVE to 2
        and SCTP_INTERLEAVING_SUPPORTED to 1.
        (启用或禁用 RFC8260 规定的用户消息交错（User Message Interleaving）
        功能扩展。该扩展允许交织发送不同流上的用户消息。启用该特性后，如果
        对端也支持，I-DATA 块将取代 DATA 块来承载用户消息。注意，要使用
        此特性，需要把该选项设为 1，同时还要把套接字选项
        SCTP_FRAGMENT_INTERLEAVE 设为 2、SCTP_INTERLEAVING_SUPPORTED
        设为 1。)

	Possible values:
	(可能的取值：)

	- 0 (disabled) - Disable extension.
	  (0（禁用）——禁用扩展。)
	- 1 (enabled) - Enable extension.
	  (1（启用）——启用扩展。)

	Default: 0 (disabled)
	(默认：0（禁用）)

ecn_enable - BOOLEAN
        Control use of Explicit Congestion Notification (ECN) by SCTP.
        Like in TCP, ECN is used only when both ends of the SCTP connection
        indicate support for it. This feature is useful in avoiding losses
        due to congestion by allowing supporting routers to signal congestion
        before having to drop packets.
        (控制 SCTP 使用显式拥塞通知（ECN）。与 TCP 一样，只有当 SCTP 连接
        两端都表示支持 ECN 时才会使用。该特性让支持的路由器能在不得不丢包
        之前通告拥塞，从而有助于避免因拥塞造成的丢包。)

        Possible values:
        (可能的取值：)

	- 0 (disabled) - Disable ecn.
	  (0（禁用）——禁用 ecn。)
	- 1 (enabled) - Enable ecn.
	  (1（启用）——启用 ecn。)

	Default: 1 (enabled)
	(默认：1（启用）)

l3mdev_accept - BOOLEAN
	Enabling this option allows a "global" bound socket to work
	across L3 master domains (e.g., VRFs) with packets capable of
	being received regardless of the L3 domain in which they
	originated. Only valid when the kernel was compiled with
	CONFIG_NET_L3_MASTER_DEV.
	(启用此选项后，"全局"绑定套接字可以跨 L3 主域（例如 VRF）工作，无论
	数据包起源于哪个 L3 域都可以接收。仅在内核编译时启用
	CONFIG_NET_L3_MASTER_DEV 时有效。)

	Possible values:
	(可能的取值：)

	- 0 (disabled)
	  (0（禁用）)
	- 1 (enabled)
	  (1（启用）)

	Default: 1 (enabled)
	(默认：1（启用）)


``/proc/sys/net/core/*``(``/proc/sys/net/core/*``)
==================================================

	Please see: Documentation/admin-guide/sysctl/net.rst for descriptions of these entries.
	(这些条目的说明请参见：Documentation/admin-guide/sysctl/net.rst。)


``/proc/sys/net/unix/*``(``/proc/sys/net/unix/*``)
==================================================

max_dgram_qlen - INTEGER
	The maximum length of dgram socket receive queue
	(dgram 套接字接收队列的最大长度)

	Default: 10
	(默认：10)

