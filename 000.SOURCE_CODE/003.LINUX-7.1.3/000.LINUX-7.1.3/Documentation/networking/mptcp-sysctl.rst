.. SPDX-License-Identifier: GPL-2.0

=====================
MPTCP Sysfs variables
=====================

(MPTCP Sysfs 变量)

/proc/sys/net/mptcp/* Variables
(/proc/sys/net/mptcp/* 变量)
===============================

add_addr_timeout - INTEGER (seconds)
	Set the maximum value of timeout after which an ADD_ADDR control message
	will be resent to an MPTCP peer that has not acknowledged a previous
	ADD_ADDR message. A dynamically estimated retransmission timeout based
	on the estimated connection round-trip-time is used if this value is
	lower than the maximum one.
	(设置超时的最大值，超过该时长后，ADD_ADDR 控制消息将被重新发送给尚未确认先前
	ADD_ADDR 消息的 MPTCP 对端。如果该值低于最大超时值，则使用基于估算的连接往返
	时间的动态估计重传超时。)

	Do not retransmit if set to 0.
	(设置为 0 时不进行重传。)

	The default value matches TCP_RTO_MAX. This is a per-namespace
	sysctl.
	(默认值与 TCP_RTO_MAX 相同。这是一个按命名空间生效的 sysctl。)

	Default: 120
	(默认：120)

allow_join_initial_addr_port - BOOLEAN
	Allow peers to send join requests to the IP address and port number used
	by the initial subflow if the value is 1. This controls a flag that is
	sent to the peer at connection time, and whether such join requests are
	accepted or denied.
	(如果该值为 1，则允许对端向初始子流所使用的 IP 地址和端口号发送加入(join)
	请求。它控制连接时发送给对端的一个标志，以及此类加入请求是被接受还是被拒绝。)

	Joins to addresses advertised with ADD_ADDR are not affected by this
	value.
	(到通过 ADD_ADDR 通告的地址的加入请求不受此值影响。)

	This is a per-namespace sysctl.
	(这是一个按命名空间生效的 sysctl。)

	Default: 1
	(默认：1)

available_path_managers - STRING
	Shows the available path managers choices that are registered. More
	path managers may be available, but not loaded.
	(显示已注册的可用路径管理器选项。可能还有更多路径管理器可用，但尚未加载。)

available_schedulers - STRING
	Shows the available schedulers choices that are registered. More packet
	schedulers may be available, but not loaded.
	(显示已注册的可用调度器选项。可能还有更多数据包调度器可用，但尚未加载。)

blackhole_timeout - INTEGER (seconds)
	Initial time period in second to disable MPTCP on active MPTCP sockets
	when a MPTCP firewall blackhole issue happens. This time period will
	grow exponentially when more blackhole issues get detected right after
	MPTCP is re-enabled and will reset to the initial value when the
	blackhole issue goes away.
	(当发生 MPTCP 防火墙黑洞问题时，在处于活跃状态的 MPTCP 套接字上禁用 MPTCP 的
	初始时长(以秒计)。当 MPTCP 重新启用后紧接着检测到更多黑洞问题时，该时长将呈
	指数增长；当黑洞问题消失时，将重置为初始值。)

	0 to disable the blackhole detection. This is a per-namespace sysctl.
	(0 表示禁用黑洞检测。这是一个按命名空间生效的 sysctl。)

	Default: 3600
	(默认：3600)

checksum_enabled - BOOLEAN
	Control whether DSS checksum can be enabled.
	(控制是否可以启用 DSS 校验和。)

	DSS checksum can be enabled if the value is nonzero. This is a
	per-namespace sysctl.
	(如果该值非零，则可以启用 DSS 校验和。这是一个按命名空间生效的 sysctl。)

	Default: 0
	(默认：0)

close_timeout - INTEGER (seconds)
	Set the make-after-break timeout: in absence of any close or
	shutdown syscall, MPTCP sockets will maintain the status
	unchanged for such time, after the last subflow removal, before
	moving to TCP_CLOSE.
	(设置 make-after-break 超时：在没有任何 close 或 shutdown 系统调用的情况下，
	MPTCP 套接字在最后一个子流被移除之后，会在此时长内保持状态不变，然后才进入
	TCP_CLOSE。)

	The default value matches TCP_TIMEWAIT_LEN. This is a per-namespace
	sysctl.
	(默认值与 TCP_TIMEWAIT_LEN 相同。这是一个按命名空间生效的 sysctl。)

	Default: 60
	(默认：60)

enabled - BOOLEAN
	Control whether MPTCP sockets can be created.
	(控制是否可以创建 MPTCP 套接字。)

	MPTCP sockets can be created if the value is 1. This is a
	per-namespace sysctl.
	(如果该值为 1，则可以创建 MPTCP 套接字。这是一个按命名空间生效的 sysctl。)

	Default: 1 (enabled)
	(默认：1(启用))

path_manager - STRING
	Set the default path manager name to use for each new MPTCP
	socket. In-kernel path management will control subflow
	connections and address advertisements according to
	per-namespace values configured over the MPTCP netlink
	API. Userspace path management puts per-MPTCP-connection subflow
	connection decisions and address advertisements under control of
	a privileged userspace program, at the cost of more netlink
	traffic to propagate all of the related events and commands.
	(设置每个新 MPTCP 套接字使用的默认路径管理器名称。内核内路径管理将根据通过
	MPTCP netlink API 配置的每命名空间值来控制子流连接和地址通告。用户空间路径管理
	则把每个 MPTCP 连接的子流连接决策和地址通告交给一个特权的用户空间程序控制，代价
	是传播所有相关事件和命令需要更多的 netlink 流量。)

	This is a per-namespace sysctl.
	(这是一个按命名空间生效的 sysctl。)

	* "kernel"          - In-kernel path manager (内核内路径管理器)
	* "userspace"       - Userspace path manager (用户空间路径管理器)

	Default: "kernel"
	(默认："kernel")

pm_type - INTEGER
	Set the default path manager type to use for each new MPTCP
	socket. In-kernel path management will control subflow
	connections and address advertisements according to
	per-namespace values configured over the MPTCP netlink
	API. Userspace path management puts per-MPTCP-connection subflow
	connection decisions and address advertisements under control of
	a privileged userspace program, at the cost of more netlink
	traffic to propagate all of the related events and commands.
	(设置每个新 MPTCP 套接字使用的默认路径管理器类型。内核内路径管理将根据通过
	MPTCP netlink API 配置的每命名空间值来控制子流连接和地址通告。用户空间路径管理
	则把每个 MPTCP 连接的子流连接决策和地址通告交给一个特权的用户空间程序控制，代价
	是传播所有相关事件和命令需要更多的 netlink 流量。)

	This is a per-namespace sysctl.
	(这是一个按命名空间生效的 sysctl。)

	Deprecated since v6.15, use path_manager instead.
	(自 v6.15 起已弃用，请改用 path_manager。)

	* 0 - In-kernel path manager (内核内路径管理器)
	* 1 - Userspace path manager (用户空间路径管理器)

	Default: 0
	(默认：0)

scheduler - STRING
	Select the scheduler of your choice.
	(选择你想要的调度器。)

	Support for selection of different schedulers. This is a per-namespace
	sysctl.
	(支持选择不同的调度器。这是一个按命名空间生效的 sysctl。)

	Default: "default"
	(默认："default")

stale_loss_cnt - INTEGER
	The number of MPTCP-level retransmission intervals with no traffic and
	pending outstanding data on a given subflow required to declare it stale.
	The packet scheduler ignores stale subflows.
	A low stale_loss_cnt  value allows for fast active-backup switch-over,
	an high value maximize links utilization on edge scenarios e.g. lossy
	link with high BER or peer pausing the data processing.
	(在给定子流上，声明其过时(stale)所需的、没有流量且仍有未完成数据的 MPTCP 层
	重传间隔的次数。数据包调度器会忽略过时的子流。较低的 stale_loss_cnt 值允许
	快速的主备(active-backup)切换；较高的值则在边缘场景下最大化链路利用率，例如
	高误码率(BER)的易损链路或对端暂停数据处理的情况。)

	This is a per-namespace sysctl.
	(这是一个按命名空间生效的 sysctl。)

	Default: 4
	(默认：4)

syn_retrans_before_tcp_fallback - INTEGER
	The number of SYN + MP_CAPABLE retransmissions before falling back to
	TCP, i.e. dropping the MPTCP options. In other words, if all the packets
	are dropped on the way, there will be:
	(在回退到 TCP(即丢弃 MPTCP 选项)之前 SYN + MP_CAPABLE 重传的次数。换言之，
	如果所有数据包在途中都被丢弃，将会出现：)

	* The initial SYN with MPTCP support (带有 MPTCP 支持的初始 SYN)
	* This number of SYN retransmitted with MPTCP support (此数量的 SYN 以支持
	  MPTCP 的方式重传)
	* The next SYN retransmissions will be without MPTCP support (接下来的 SYN
	  重传将不带 MPTCP 支持)

	0 means the first retransmission will be done without MPTCP options.
	>= 128 means that all SYN retransmissions will keep the MPTCP options. A
	lower number might increase false-positive MPTCP blackholes detections.
	This is a per-namespace sysctl.
	(0 表示第一次重传将不带 MPTCP 选项。>= 128 表示所有 SYN 重传都将保留 MPTCP
	选项。较低的数值可能会增加 MPTCP 黑洞误检的可能性。这是一个按命名空间生效的
	sysctl。)

	Default: 2
	(默认：2)
