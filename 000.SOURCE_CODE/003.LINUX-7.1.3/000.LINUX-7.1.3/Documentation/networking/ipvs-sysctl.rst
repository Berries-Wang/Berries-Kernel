.. SPDX-License-Identifier: GPL-2.0

=============================================
IPvs-sysctl(IPVS (IP 虚拟服务器) sysctl 参数)
=============================================

/proc/sys/net/ipv4/vs/* Variables:(/proc/sys/net/ipv4/vs/* 变量：)
==================================================================

am_droprate - INTEGER
(am_droprate - 整数)

	default 10
	(默认值 10)

	It sets the always mode drop rate, which is used in the mode 3
	of the drop_rate defense.
	(它设置 always 模式的丢弃速率，用于 drop_rate 防御的模式 3。)

amemthresh - INTEGER
(amemthresh - 整数)

	default 1024
	(默认值 1024)

	It sets the available memory threshold (in pages), which is
	used in the automatic modes of defense. When there is no
	enough available memory, the respective strategy will be
	enabled and the variable is automatically set to 2, otherwise
	the strategy is disabled and the variable is  set  to 1.
	(它设置可用内存阈值 (以页为单位)，用于各种自动防御模式。当可用内存不足时，相应的策略将被启用，且该变量被自动设置为 2；否则策略被禁用，该变量被设置为 1。)

backup_only - BOOLEAN
(backup_only - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	If set, disable the director function while the server is
	in backup mode to avoid packet loops for DR/TUN methods.
	(如果设置此选项，则在服务器处于备份模式时禁用 director (调度器) 功能，以避免 DR/TUN 方法出现数据包循环。)

conn_lfactor - INTEGER
(conn_lfactor - 整数)

	Possible values: -8 (larger table) .. 8 (smaller table)
	(可能的取值：-8 (更大的表) .. 8 (更小的表))

	Default: -4
	(默认值：-4)

	Controls the sizing of the connection hash table based on the
	load factor (number of connections per table buckets):
	(根据负载因子 (每个表桶的连接数) 控制连接哈希表的大小：)

		2^conn_lfactor = nodes / buckets

	As result, the table grows if load increases and shrinks when
	load decreases in the range of 2^8 - 2^conn_tab_bits (module
	parameter).
	(因此，当负载增加时表会增长，负载减少时表会收缩，范围是 2^8 - 2^conn_tab_bits (模块参数)。)
	The value is a shift count where negative values select
	buckets = (connection hash nodes << -value) while positive
	values select buckets = (connection hash nodes >> value). The
	negative values reduce the collisions and reduce the time for
	lookups but increase the table size. Positive values will
	tolerate load above 100% when using smaller table is
	preferred with the cost of more collisions. If using NAT
	connections consider decreasing the value with one because
	they add two nodes in the hash table.
	(该值是一个移位计数：负值选择 buckets = (connection hash nodes << -value)，正值选择 buckets = (connection hash nodes >> value)。负值可以减少冲突、缩短查找时间，但会增大表的大小。正值可以容忍超过 100% 的负载，适合在希望使用更小的表时以更多冲突为代价。如果使用 NAT 连接，考虑把该值减一，因为它们会在哈希表中添加两个节点。)

	Example:
	(示例：)
	-4: grow if load goes above 6% (buckets = nodes * 16)
	(-4：负载超过 6% 时增长 (buckets = nodes * 16))
	2: grow if load goes above 400% (buckets = nodes / 4)
	(2：负载超过 400% 时增长 (buckets = nodes / 4))

conn_reuse_mode - INTEGER
(conn_reuse_mode - 整数)

	1 - default
	(1 - 默认值)

	Controls how ipvs will deal with connections that are detected
	port reuse. It is a bitmap, with the values being:
	(控制 ipvs 如何处理被检测到端口复用的连接。它是一个位图，取值如下：)

	0: disable any special handling on port reuse. The new
	connection will be delivered to the same real server that was
	servicing the previous connection.
	(0：对端口复用禁用任何特殊处理。新连接将被交付给为前一个连接提供服务的同一台真实服务器。)

	bit 1: enable rescheduling of new connections when it is safe.
	That is, whenever expire_nodest_conn and for TCP sockets, when
	the connection is in TIME_WAIT state (which is only possible if
	you use NAT mode).
	(bit 1：在安全时启用新连接的重新调度。即，只要 expire_nodest_conn 生效，且对 TCP 套接字而言，当连接处于 TIME_WAIT 状态时 (这只有在使用 NAT 模式时才可能)。)

	bit 2: it is bit 1 plus, for TCP connections, when connections
	are in FIN_WAIT state, as this is the last state seen by load
	balancer in Direct Routing mode. This bit helps on adding new
	real servers to a very busy cluster.
	(bit 2：在 bit 1 的基础上，对 TCP 连接，再增加当连接处于 FIN_WAIT 状态时的情形，因为这是负载均衡器在直接路由模式下看到的最后一个状态。此位有助于向非常繁忙的集群添加新的真实服务器。)

conntrack - BOOLEAN
(conntrack - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	If set, maintain connection tracking entries for
	connections handled by IPVS.
	(如果设置，为 IPVS 处理的连接维护连接跟踪表项。)

	This should be enabled if connections handled by IPVS are to be
	also handled by stateful firewall rules. That is, iptables rules
	that make use of connection tracking.  It is a performance
	optimisation to disable this setting otherwise.
	(如果 IPVS 处理的连接还要由有状态防火墙规则处理，则应启用此项。即，那些使用连接跟踪的 iptables 规则。否则，禁用此设置是一种性能优化。)

	Connections handled by the IPVS FTP application module
	will have connection tracking entries regardless of this setting.
	(无论此设置如何，由 IPVS FTP 应用模块处理的连接都会有连接跟踪表项。)

	Only available when IPVS is compiled with CONFIG_IP_VS_NFCT enabled.
	(仅当 IPVS 编译时启用了 CONFIG_IP_VS_NFCT 时可用。)

cache_bypass - BOOLEAN
(cache_bypass - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	If it is enabled, forward packets to the original destination
	directly when no cache server is available and destination
	address is not local (iph->daddr is RTN_UNICAST). It is mostly
	used in transparent web cache cluster.
	(如果启用，当没有可用的缓存服务器且目标地址不是本地地址 (iph->daddr 为 RTN_UNICAST) 时，直接把数据包转发到原始目标。它主要用于透明 Web 缓存集群。)

debug_level - INTEGER
(debug_level - 整数)

	- 0          - transmission error messages (default)
	  (0 - 传输错误消息 (默认))
	- 1          - non-fatal error messages
	  (1 - 非致命错误消息)
	- 2          - configuration
	  (2 - 配置)
	- 3          - destination trash
	  (3 - 目标垃圾箱)
	- 4          - drop entry
	  (4 - 丢弃表项)
	- 5          - service lookup
	  (5 - 服务查找)
	- 6          - scheduling
	  (6 - 调度)
	- 7          - connection new/expire, lookup and synchronization
	  (7 - 连接新建/过期、查找与同步)
	- 8          - state transition
	  (8 - 状态迁移)
	- 9          - binding destination, template checks and applications
	  (9 - 绑定目标、模板检查与应用)
	- 10         - IPVS packet transmission
	  (10 - IPVS 数据包传输)
	- 11         - IPVS packet handling (ip_vs_in/ip_vs_out)
	  (11 - IPVS 数据包处理 (ip_vs_in/ip_vs_out))
	- 12 or more - packet traversal
	  (12 或更高 - 数据包遍历)

	Only available when IPVS is compiled with CONFIG_IP_VS_DEBUG enabled.
	(仅当 IPVS 编译时启用了 CONFIG_IP_VS_DEBUG 时可用。)

	Higher debugging levels include the messages for lower debugging
	levels, so setting debug level 2, includes level 0, 1 and 2
	messages. Thus, logging becomes more and more verbose the higher
	the level.
	(更高的调试级别包含较低调试级别的消息，因此把调试级别设为 2 时，会包含级别 0、1 和 2 的消息。因此，级别越高，日志越详细。)

drop_entry - INTEGER
(drop_entry - 整数)

	- 0  - disabled (default)
	  (0 - 禁用 (默认))

	The drop_entry defense is to randomly drop entries in the
	connection hash table, just in order to collect back some
	memory for new connections. In the current code, the
	drop_entry procedure can be activated every second, then it
	randomly scans 1/32 of the whole and drops entries that are in
	the SYN-RECV/SYNACK state, which should be effective against
	syn-flooding attack.
	(drop_entry 防御是指随机丢弃连接哈希表中的表项，只是为了给新连接回收一些内存。在当前代码中，drop_entry 过程每秒可以激活一次，然后随机扫描整个表的 1/32，并丢弃处于 SYN-RECV/SYNACK 状态的表项，这对抵御 syn-flooding 攻击应该有效。)

	The valid values of drop_entry are from 0 to 3, where 0 means
	that this strategy is always disabled, 1 and 2 mean automatic
	modes (when there is no enough available memory, the strategy
	is enabled and the variable is automatically set to 2,
	otherwise the strategy is disabled and the variable is set to
	1), and 3 means that the strategy is always enabled.
	(drop_entry 的有效取值为 0 到 3：0 表示此策略始终禁用；1 和 2 表示自动模式 (当可用内存不足时，策略被启用且变量被自动设置为 2，否则策略被禁用且变量被设置为 1)；3 表示此策略始终启用。)

drop_packet - INTEGER
(drop_packet - 整数)

	- 0  - disabled (default)
	  (0 - 禁用 (默认))

	The drop_packet defense is designed to drop 1/rate packets
	before forwarding them to real servers. If the rate is 1, then
	drop all the incoming packets.
	(drop_packet 防御的设计是在把数据包转发给真实服务器之前按 1/rate 的比例丢弃数据包。如果 rate 为 1，则丢弃所有进入的数据包。)

	The value definition is the same as that of the drop_entry. In
	the automatic mode, the rate is determined by the follow
	formula: rate = amemthresh / (amemthresh - available_memory)
	when available memory is less than the available memory
	threshold. When the mode 3 is set, the always mode drop rate
	is controlled by the /proc/sys/net/ipv4/vs/am_droprate.
	(取值定义与 drop_entry 相同。在自动模式下，速率由以下公式确定：rate = amemthresh / (amemthresh - available_memory)，在可用内存小于可用内存阈值时生效。当设置为模式 3 时，always 模式的丢弃速率由 /proc/sys/net/ipv4/vs/am_droprate 控制。)

est_cpulist - CPULIST
(est_cpulist - CPU 列表)

	Allowed	CPUs for estimation kthreads
	(允许用于估算内核线程的 CPU)

	Syntax: standard cpulist format
	empty list - stop kthread tasks and estimation
	default - the system's housekeeping CPUs for kthreads
	(语法：标准 cpulist 格式；空列表 - 停止内核线程任务和估算；default - 内核线程使用系统的内务 (housekeeping) CPU)

	Example:
	(示例：)
	"all": all possible CPUs
	("all"：所有可能的 CPU)
	"0-N": all possible CPUs, N denotes last CPU number
	("0-N"：所有可能的 CPU，N 表示最后一个 CPU 的编号)
	"0,1-N:1/2": first and all CPUs with odd number
	("0,1-N:1/2"：第一个 CPU 以及所有奇数编号的 CPU)
	"": empty list
	(""：空列表)

est_nice - INTEGER
(est_nice - 整数)

	default 0
	(默认值 0)
	Valid range: -20 (more favorable) .. 19 (less favorable)
	(有效范围：-20 (更优待) .. 19 (更不优待))

	Niceness value to use for the estimation kthreads (scheduling
	priority)
	(估算内核线程使用的 nice 值 (调度优先级))

expire_nodest_conn - BOOLEAN
(expire_nodest_conn - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	The default value is 0, the load balancer will silently drop
	packets when its destination server is not available. It may
	be useful, when user-space monitoring program deletes the
	destination server (because of server overload or wrong
	detection) and add back the server later, and the connections
	to the server can continue.
	(默认值为 0，当目标服务器不可用时，负载均衡器会静默丢弃数据包。当用户态监控程序删除目标服务器 (因为服务器过载或检测错误) 并稍后重新添加该服务器，且到该服务器的连接需要继续时，这可能有用。)

	If this feature is enabled, the load balancer will expire the
	connection immediately when a packet arrives and its
	destination server is not available, then the client program
	will be notified that the connection is closed. This is
	equivalent to the feature some people requires to flush
	connections when its destination is not available.
	(如果启用此特性，当数据包到达而其目标服务器不可用时，负载均衡器会立即让该连接过期，然后客户端程序将收到连接已关闭的通知。这相当于某些人所需要的特性：在目标不可用时清空 (flush) 连接。)

expire_quiescent_template - BOOLEAN
(expire_quiescent_template - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	When set to a non-zero value, the load balancer will expire
	persistent templates when the destination server is quiescent.
	This may be useful, when a user makes a destination server
	quiescent by setting its weight to 0 and it is desired that
	subsequent otherwise persistent connections are sent to a
	different destination server.  By default new persistent
	connections are allowed to quiescent destination servers.
	(设置为非零值时，当目标服务器处于静默 (quiescent) 状态时，负载均衡器会使持久化模板过期。当用户通过把目标服务器的权重设为 0 使其静默，并希望后续原本会持久化的连接被发送到不同的目标服务器时，这可能有用。默认情况下，允许发往静默目标服务器的新持久化连接。)

	If this feature is enabled, the load balancer will expire the
	persistence template if it is to be used to schedule a new
	connection and the destination server is quiescent.
	(如果启用此特性，当持久化模板将被用于调度新连接而目标服务器处于静默状态时，负载均衡器会使该持久化模板过期。)

ignore_tunneled - BOOLEAN
(ignore_tunneled - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	If set, ipvs will set the ipvs_property on all packets which are of
	unrecognized protocols.  This prevents us from routing tunneled
	protocols like ipip, which is useful to prevent rescheduling
	packets that have been tunneled to the ipvs host (i.e. to prevent
	ipvs routing loops when ipvs is also acting as a real server).
	(如果设置，ipvs 会在所有属于未识别协议的数据包上设置 ipvs_property。这会阻止我们路由 ipip 之类的隧道协议，有助于防止把已被隧道化到 ipvs 主机的数据包重新调度 (即防止 ipvs 在同时充当真实服务器时出现 ipvs 路由循环)。)

nat_icmp_send - BOOLEAN
(nat_icmp_send - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用 (默认))
	- not 0 - enabled
	  (非 0 - 启用)

	It controls sending icmp error messages (ICMP_DEST_UNREACH)
	for VS/NAT when the load balancer receives packets from real
	servers but the connection entries don't exist.
	(它控制 VS/NAT 在负载均衡器收到来自真实服务器的数据包但连接表项不存在时发送 icmp 错误消息 (ICMP_DEST_UNREACH) 的行为。)

pmtu_disc - BOOLEAN
(pmtu_disc - 布尔值)

	- 0 - disabled
	  (0 - 禁用)
	- not 0 - enabled (default)
	  (非 0 - 启用 (默认))

	By default, reject with FRAG_NEEDED all DF packets that exceed
	the PMTU, irrespective of the forwarding method. For TUN method
	the flag can be disabled to fragment such packets.
	(默认情况下，无论使用何种转发方法，对所有超过 PMTU 的 DF 数据包都以 FRAG_NEEDED 拒绝。对于 TUN 方法，可以禁用此标志以对这类数据包进行分片。)

secure_tcp - INTEGER
(secure_tcp - 整数)

	- 0  - disabled (default)
	  (0 - 禁用 (默认))

	The secure_tcp defense is to use a more complicated TCP state
	transition table. For VS/NAT, it also delays entering the
	TCP ESTABLISHED state until the three way handshake is completed.
	(secure_tcp 防御是使用更复杂的 TCP 状态转换表。对 VS/NAT，它还会把进入 TCP ESTABLISHED 状态的时刻推迟到三次握手完成之后。)

	The value definition is the same as that of drop_entry and
	drop_packet.
	(取值定义与 drop_entry 和 drop_packet 相同。)

svc_lfactor - INTEGER
(svc_lfactor - 整数)

	Possible values: -8 (larger table) .. 8 (smaller table)
	(可能的取值：-8 (更大的表) .. 8 (更小的表))

	Default: -3
	(默认值：-3)

	Controls the sizing of the service hash table based on the
	load factor (number of services per table buckets). The table
	will grow and shrink in the range of 2^4 - 2^20.
	See conn_lfactor for explanation.
	(根据负载因子 (每个表桶的服务数) 控制服务哈希表的大小。表会在 2^4 - 2^20 的范围内增长和收缩。解释参见 conn_lfactor。)

sync_threshold - vector of 2 INTEGERs: sync_threshold, sync_period
(sync_threshold - 2 个整数的向量：sync_threshold, sync_period)

	default 3 50
	(默认值 3 50)

	It sets synchronization threshold, which is the minimum number
	of incoming packets that a connection needs to receive before
	the connection will be synchronized. A connection will be
	synchronized, every time the number of its incoming packets
	modulus sync_period equals the threshold. The range of the
	threshold is from 0 to sync_period.
	(它设置同步阈值，即连接被同步之前需要收到的最少入站数据包数。每当连接的入站数据包数对 sync_period 取模等于阈值时，连接就会被同步。阈值的范围是 0 到 sync_period。)

	When sync_period and sync_refresh_period are 0, send sync only
	for state changes or only once when pkts matches sync_threshold
	(当 sync_period 和 sync_refresh_period 为 0 时，仅在状态变化时发送同步，或仅在数据包数匹配 sync_threshold 时发送一次同步)

sync_refresh_period - UNSIGNED INTEGER
(sync_refresh_period - 无符号整数)

	default 0
	(默认值 0)

	In seconds, difference in reported connection timer that triggers
	new sync message. It can be used to avoid sync messages for the
	specified period (or half of the connection timeout if it is lower)
	if connection state is not changed since last sync.
	(以秒为单位，触发新同步消息的所报告连接计时器差异。自上次同步以来连接状态未发生变化时，它可用于在指定周期内 (或取连接超时的一半，以较低者为准) 避免发送同步消息。)

	This is useful for normal connections with high traffic to reduce
	sync rate. Additionally, retry sync_retries times with period of
	sync_refresh_period/8.
	(这对高流量的普通连接很有用，可以降低同步速率。此外，会以 sync_refresh_period/8 为周期重试 sync_retries 次。)

sync_retries - INTEGER
(sync_retries - 整数)

	default 0
	(默认值 0)

	Defines sync retries with period of sync_refresh_period/8. Useful
	to protect against loss of sync messages. The range of the
	sync_retries is from 0 to 3.
	(定义以 sync_refresh_period/8 为周期的同步重试。有助于防止同步消息丢失。sync_retries 的范围是 0 到 3。)

sync_qlen_max - UNSIGNED LONG
(sync_qlen_max - 无符号长整数)


	Hard limit for queued sync messages that are not sent yet. It
	defaults to 1/32 of the memory pages but actually represents
	number of messages. It will protect us from allocating large
	parts of memory when the sending rate is lower than the queuing
	rate.
	(尚未发送的排队同步消息的硬性上限。它默认为内存页数的 1/32，但实际上表示的是消息数量。当发送速率低于排队速率时，它可以防止我们分配大量内存。)

sync_sock_size - INTEGER
(sync_sock_size - 整数)

	default 0
	(默认值 0)

	Configuration of SNDBUF (master) or RCVBUF (slave) socket limit.
	Default value is 0 (preserve system defaults).
	(SNDBUF (master) 或 RCVBUF (slave) 套接字上限的配置。默认值为 0 (保留系统默认值)。)

sync_ports - INTEGER
(sync_ports - 整数)

	default 1
	(默认值 1)

	The number of threads that master and backup servers can use for
	sync traffic. Every thread will use single UDP port, thread 0 will
	use the default port 8848 while last thread will use port
	8848+sync_ports-1.
	(master 和 backup 服务器可用于同步流量的线程数。每个线程使用单个 UDP 端口：线程 0 使用默认端口 8848，最后一个线程使用端口 8848+sync_ports-1。)

snat_reroute - BOOLEAN
(snat_reroute - 布尔值)

	- 0 - disabled
	  (0 - 禁用)
	- not 0 - enabled (default)
	  (非 0 - 启用 (默认))

	If enabled, recalculate the route of SNATed packets from
	realservers so that they are routed as if they originate from the
	director. Otherwise they are routed as if they are forwarded by the
	director.
	(如果启用，则重新计算来自真实服务器的 SNAT 数据包的路由，使它们像是源自 director 一样被路由。否则，它们按"由 director 转发"的方式路由。)

	If policy routing is in effect then it is possible that the route
	of a packet originating from a director is routed differently to a
	packet being forwarded by the director.
	(如果策略路由生效，那么源自 director 的数据包的路由可能与由 director 转发的数据包的路由不同。)

	If policy routing is not in effect then the recalculated route will
	always be the same as the original route so it is an optimisation
	to disable snat_reroute and avoid the recalculation.
	(如果策略路由未生效，那么重新计算出的路由总是与原始路由相同，因此禁用 snat_reroute 以避免重新计算是一种优化。)

sync_persist_mode - INTEGER
(sync_persist_mode - 整数)

	default 0
	(默认值 0)

	Controls the synchronisation of connections when using persistence
	(控制使用持久化时连接的同步)

	0: All types of connections are synchronised
	(0：同步所有类型的连接)

	1: Attempt to reduce the synchronisation traffic depending on
	the connection type. For persistent services avoid synchronisation
	for normal connections, do it only for persistence templates.
	In such case, for TCP and SCTP it may need enabling sloppy_tcp and
	sloppy_sctp flags on backup servers. For non-persistent services
	such optimization is not applied, mode 0 is assumed.
	(1：根据连接类型尝试减少同步流量。对持久化服务，避免对普通连接进行同步，只对持久化模板进行同步。在这种情况下，对 TCP 和 SCTP，可能需要在 backup 服务器上启用 sloppy_tcp 和 sloppy_sctp 标志。对非持久化服务，不应用此类优化，假定使用模式 0。)

sync_version - INTEGER
(sync_version - 整数)

	default 1
	(默认值 1)

	The version of the synchronisation protocol used when sending
	synchronisation messages.
	(发送同步消息时使用的同步协议版本。)

	0 selects the original synchronisation protocol (version 0). This
	should be used when sending synchronisation messages to a legacy
	system that only understands the original synchronisation protocol.
	(0 选择原始同步协议 (版本 0)。当向只理解原始同步协议的旧式系统发送同步消息时，应使用该版本。)

	1 selects the current synchronisation protocol (version 1). This
	should be used where possible.
	(1 选择当前同步协议 (版本 1)。应尽可能使用该版本。)

	Kernels with this sync_version entry are able to receive messages
	of both version 1 and version 2 of the synchronisation protocol.
	(带有此 sync_version 项的内核能够接收同步协议版本 1 和版本 2 的消息。)

run_estimation - BOOLEAN
(run_estimation - 布尔值)

	0 - disabled
	(0 - 禁用)
	not 0 - enabled (default)
	(非 0 - 启用 (默认))

	If disabled, the estimation will be suspended and kthread tasks
	stopped.
	(如果禁用，估算将被挂起，内核线程任务将停止。)

	You can always re-enable estimation by setting this value to 1.
	But be careful, the first estimation after re-enable is not
	accurate.
	(把此值设为 1 总是可以重新启用估算。但要小心，重新启用后的第一次估算并不准确。)
