.. SPDX-License-Identifier: GPL-2.0

===================================================================
Netfilter Conntrack Sysfs variables(Netfilter Conntrack Sysfs 变量)
===================================================================

/proc/sys/net/netfilter/nf_conntrack_* Variables:(/proc/sys/net/netfilter/nf_conntrack_* 变量：)
================================================================================================

nf_conntrack_acct - BOOLEAN
(nf_conntrack_acct - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用（默认）)
	- not 0 - enabled
	  (非 0 - 启用)

	Enable connection tracking flow accounting. 64-bit byte and packet
	counters per flow are added.
	(启用连接跟踪流统计。会为每个流添加 64 位的字节计数器与数据包计数器。)

nf_conntrack_buckets - INTEGER
(nf_conntrack_buckets - 整数)

	Size of hash table. If not specified as parameter during module
	loading, the default size is calculated by dividing total memory
	by 16384 to determine the number of buckets. The hash table will
	never have fewer than 1024 and never more than 262144 buckets.
	This sysctl is only writeable in the initial net namespace.
	(哈希表大小。如果在模块加载时未作为参数指定，默认大小通过把总内存除以 16384 来确定桶的数量。哈希表的桶数不会少于 1024，也不会多于 262144。此 sysctl 仅在初始网络命名空间中可写。)

nf_conntrack_checksum - BOOLEAN
(nf_conntrack_checksum - 布尔值)

	- 0 - disabled
	  (0 - 禁用)
	- not 0 - enabled (default)
	  (非 0 - 启用（默认）)

	Verify checksum of incoming packets. Packets with bad checksums are
	in INVALID state. If this is enabled, such packets will not be
	considered for connection tracking.
	(校验到来数据包的校验和。校验和不正确的数据包处于 INVALID 状态。如果启用此项，这类数据包将不被纳入连接跟踪的考虑。)

nf_conntrack_count - INTEGER (read-only)
(nf_conntrack_count - 整数（只读）)

	Number of currently allocated flow entries.
	(当前已分配的流表项数量。)

nf_conntrack_events - BOOLEAN
(nf_conntrack_events - 布尔值)

	- 0 - disabled
	  (0 - 禁用)
	- 1 - enabled
	  (1 - 启用)
	- 2 - auto (default)
	  (2 - 自动（默认）)

	If this option is enabled, the connection tracking code will
	provide userspace with connection tracking events via ctnetlink.
	The default allocates the extension if a userspace program is
	listening to ctnetlink events.
	(如果启用此选项，连接跟踪代码会通过 ctnetlink 向用户空间提供连接跟踪事件。默认情况下，如果有用户空间程序正在监听 ctnetlink 事件，则会分配该扩展。)

nf_conntrack_expect_max - INTEGER
(nf_conntrack_expect_max - 整数)

	Maximum size of expectation table.  Default value is
	nf_conntrack_buckets / 256. Minimum is 1.
	(期望 (expectation) 表的最大尺寸。默认值为 nf_conntrack_buckets / 256。最小值为 1。)

nf_conntrack_frag6_high_thresh - INTEGER
(nf_conntrack_frag6_high_thresh - 整数)

	default 262144
	(默认 262144)

	Maximum memory used to reassemble IPv6 fragments.  When
	nf_conntrack_frag6_high_thresh bytes of memory is allocated for this
	purpose, the fragment handler will toss packets until
	nf_conntrack_frag6_low_thresh is reached.
	(用于重组 IPv6 分片的最大内存。当为此目的分配的内存达到 nf_conntrack_frag6_high_thresh 字节时，分片处理器将丢弃数据包，直到内存降到 nf_conntrack_frag6_low_thresh 以下。)

nf_conntrack_frag6_low_thresh - INTEGER
(nf_conntrack_frag6_low_thresh - 整数)

	default 196608
	(默认 196608)

	See nf_conntrack_frag6_low_thresh
	(参见 nf_conntrack_frag6_low_thresh。)

nf_conntrack_frag6_timeout - INTEGER (seconds)
(nf_conntrack_frag6_timeout - 整数（秒）)

	default 60
	(默认 60)

	Time to keep an IPv6 fragment in memory.
	(IPv6 分片在内存中保留的时间。)

nf_conntrack_generic_timeout - INTEGER (seconds)
(nf_conntrack_generic_timeout - 整数（秒）)

	default 600
	(默认 600)

	Default for generic timeout.  This refers to layer 4 unknown/unsupported
	protocols.
	(通用超时的默认值。适用于四层未知/不受支持的协议。)

nf_conntrack_icmp_timeout - INTEGER (seconds)
(nf_conntrack_icmp_timeout - 整数（秒）)

	default 30
	(默认 30)

	Default for ICMP timeout.
	(ICMP 超时的默认值。)

nf_conntrack_icmpv6_timeout - INTEGER (seconds)
(nf_conntrack_icmpv6_timeout - 整数（秒）)

	default 30
	(默认 30)

	Default for ICMP6 timeout.
	(ICMP6 超时的默认值。)

nf_conntrack_log_invalid - INTEGER
(nf_conntrack_log_invalid - 整数)

	- 0   - disable (default)
	  (0   - 禁用（默认）)
	- 1   - log ICMP packets
	  (1   - 记录 ICMP 数据包)
	- 6   - log TCP packets
	  (6   - 记录 TCP 数据包)
	- 17  - log UDP packets
	  (17  - 记录 UDP 数据包)
	- 41  - log ICMPv6 packets
	  (41  - 记录 ICMPv6 数据包)
	- 136 - log UDPLITE packets
	  (136 - 记录 UDPLITE 数据包)
	- 255 - log packets of any protocol
	  (255 - 记录任意协议的数据包)

	Log invalid packets of a type specified by value.
	(记录由值所指定类型的无效数据包。)

nf_conntrack_max - INTEGER
(nf_conntrack_max - 整数)

        Maximum number of allowed connection tracking entries. This value is set
        to nf_conntrack_buckets by default.
        Note that connection tracking entries are added to the table twice -- once
        for the original direction and once for the reply direction (i.e., with
        the reversed address). This means that with default settings a maxed-out
        table will have a average hash chain length of 2, not 1.
        (允许的连接跟踪表项的最大数量。此值默认设置为 nf_conntrack_buckets。注意，连接跟踪表项会被加入表中两次——一次用于原始方向，一次用于回复方向（即地址反转后的表项）。这意味着在默认设置下，一个被填满的表其平均哈希链长度为 2 而不是 1。)

nf_conntrack_tcp_be_liberal - BOOLEAN
(nf_conntrack_tcp_be_liberal - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用（默认）)
	- not 0 - enabled
	  (非 0 - 启用)

	Be conservative in what you do, be liberal in what you accept from others.
	If it's non-zero, we mark only out of window RST segments as INVALID.
	(严于律己，宽以待人。如果为非零，则只把窗外 (out of window) 的 RST 段标记为 INVALID。)

nf_conntrack_tcp_ignore_invalid_rst - BOOLEAN
(nf_conntrack_tcp_ignore_invalid_rst - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用（默认）)
	- 1 - enabled
	  (1 - 启用)

	If it's 1, we don't mark out of window RST segments as INVALID.
	(如果为 1，则不把窗外的 RST 段标记为 INVALID。)

nf_conntrack_tcp_loose - BOOLEAN
(nf_conntrack_tcp_loose - 布尔值)

	- 0 - disabled
	  (0 - 禁用)
	- not 0 - enabled (default)
	  (非 0 - 启用（默认）)

	If it is set to zero, we disable picking up already established
	connections.
	(如果设为零，则禁用拾取已建立的连接。)

nf_conntrack_tcp_max_retrans - INTEGER
(nf_conntrack_tcp_max_retrans - 整数)

	default 3
	(默认 3)

	Maximum number of packets that can be retransmitted without
	received an (acceptable) ACK from the destination. If this number
	is reached, a shorter timer will be started.
	(在尚未收到来自目的端的（可接受的）ACK 之前，允许重传的最大数据包数。达到此数量后，将启动一个更短的定时器。)

nf_conntrack_tcp_timeout_close - INTEGER (seconds)
(nf_conntrack_tcp_timeout_close - 整数（秒）)

	default 10
	(默认 10)

nf_conntrack_tcp_timeout_close_wait - INTEGER (seconds)
(nf_conntrack_tcp_timeout_close_wait - 整数（秒）)

	default 60
	(默认 60)

nf_conntrack_tcp_timeout_established - INTEGER (seconds)
(nf_conntrack_tcp_timeout_established - 整数（秒）)

	default 432000 (5 days)
	(默认 432000（5 天）)

nf_conntrack_tcp_timeout_fin_wait - INTEGER (seconds)
(nf_conntrack_tcp_timeout_fin_wait - 整数（秒）)

	default 120
	(默认 120)

nf_conntrack_tcp_timeout_last_ack - INTEGER (seconds)
(nf_conntrack_tcp_timeout_last_ack - 整数（秒）)

	default 30
	(默认 30)

nf_conntrack_tcp_timeout_max_retrans - INTEGER (seconds)
(nf_conntrack_tcp_timeout_max_retrans - 整数（秒）)

	default 300
	(默认 300)

nf_conntrack_tcp_timeout_syn_recv - INTEGER (seconds)
(nf_conntrack_tcp_timeout_syn_recv - 整数（秒）)

	default 60
	(默认 60)

nf_conntrack_tcp_timeout_syn_sent - INTEGER (seconds)
(nf_conntrack_tcp_timeout_syn_sent - 整数（秒）)

	default 120
	(默认 120)

nf_conntrack_tcp_timeout_time_wait - INTEGER (seconds)
(nf_conntrack_tcp_timeout_time_wait - 整数（秒）)

	default 120
	(默认 120)

nf_conntrack_tcp_timeout_unacknowledged - INTEGER (seconds)
(nf_conntrack_tcp_timeout_unacknowledged - 整数（秒）)

	default 300
	(默认 300)

nf_conntrack_timestamp - BOOLEAN
(nf_conntrack_timestamp - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用（默认）)
	- not 0 - enabled
	  (非 0 - 启用)

	Enable connection tracking flow timestamping.
	(启用连接跟踪流时间戳。)

nf_conntrack_sctp_timeout_closed - INTEGER (seconds)
(nf_conntrack_sctp_timeout_closed - 整数（秒）)

	default 10
	(默认 10)

nf_conntrack_sctp_timeout_cookie_wait - INTEGER (seconds)
(nf_conntrack_sctp_timeout_cookie_wait - 整数（秒）)

	default 3
	(默认 3)

nf_conntrack_sctp_timeout_cookie_echoed - INTEGER (seconds)
(nf_conntrack_sctp_timeout_cookie_echoed - 整数（秒）)

	default 3
	(默认 3)

nf_conntrack_sctp_timeout_established - INTEGER (seconds)
(nf_conntrack_sctp_timeout_established - 整数（秒）)

	default 210
	(默认 210)

	Default is set to (hb_interval * path_max_retrans + rto_max)
	(默认值设置为 hb_interval * path_max_retrans + rto_max。)

nf_conntrack_sctp_timeout_shutdown_sent - INTEGER (seconds)
(nf_conntrack_sctp_timeout_shutdown_sent - 整数（秒）)

	default 3
	(默认 3)

nf_conntrack_sctp_timeout_shutdown_recd - INTEGER (seconds)
(nf_conntrack_sctp_timeout_shutdown_recd - 整数（秒）)

	default 3
	(默认 3)

nf_conntrack_sctp_timeout_shutdown_ack_sent - INTEGER (seconds)
(nf_conntrack_sctp_timeout_shutdown_ack_sent - 整数（秒）)

	default 3
	(默认 3)

nf_conntrack_sctp_timeout_heartbeat_sent - INTEGER (seconds)
(nf_conntrack_sctp_timeout_heartbeat_sent - 整数（秒）)

	default 30
	(默认 30)

	This timeout is used to setup conntrack entry on secondary paths.
	Default is set to hb_interval.
	(此超时用于在次路径上建立 conntrack 表项。默认值设置为 hb_interval。)

nf_conntrack_udp_timeout - INTEGER (seconds)
(nf_conntrack_udp_timeout - 整数（秒）)

	default 30
	(默认 30)

nf_conntrack_udp_timeout_stream - INTEGER (seconds)
(nf_conntrack_udp_timeout_stream - 整数（秒）)

	default 120
	(默认 120)

	This extended timeout will be used in case there is an UDP stream
	detected.
	(检测到 UDP 流时将使用此扩展超时。)

nf_conntrack_gre_timeout - INTEGER (seconds)
(nf_conntrack_gre_timeout - 整数（秒）)

	default 30
	(默认 30)

nf_conntrack_gre_timeout_stream - INTEGER (seconds)
(nf_conntrack_gre_timeout_stream - 整数（秒）)

	default 180
	(默认 180)

	This extended timeout will be used in case there is an GRE stream
	detected.
	(检测到 GRE 流时将使用此扩展超时。)

nf_hooks_lwtunnel - BOOLEAN
(nf_hooks_lwtunnel - 布尔值)

	- 0 - disabled (default)
	  (0 - 禁用（默认）)
	- not 0 - enabled
	  (非 0 - 启用)

	If this option is enabled, the lightweight tunnel netfilter hooks are
	enabled. This option cannot be disabled once it is enabled.
	(如果启用此选项，轻量级隧道 (lightweight tunnel) netfilter 钩子将被启用。此选项一旦启用便无法再禁用。)

nf_flowtable_tcp_timeout - INTEGER (seconds)
(nf_flowtable_tcp_timeout - 整数（秒）)

        default 30
        (默认 30)

        Control offload timeout for tcp connections.
        TCP connections may be offloaded from nf conntrack to nf flow table.
        Once aged, the connection is returned to nf conntrack.
        (控制 TCP 连接的卸载超时。TCP 连接可以从 nf conntrack 卸载到 nf flow table。一旦老化到期，连接会被退回 nf conntrack。)

nf_flowtable_udp_timeout - INTEGER (seconds)
(nf_flowtable_udp_timeout - 整数（秒）)

        default 30
        (默认 30)

        Control offload timeout for udp connections.
        UDP connections may be offloaded from nf conntrack to nf flow table.
        Once aged, the connection is returned to nf conntrack.
        (控制 UDP 连接的卸载超时。UDP 连接可以从 nf conntrack 卸载到 nf flow table。一旦老化到期，连接会被退回 nf conntrack。)
