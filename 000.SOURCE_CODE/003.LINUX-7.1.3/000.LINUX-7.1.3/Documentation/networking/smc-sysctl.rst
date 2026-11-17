.. SPDX-License-Identifier: GPL-2.0

==========
SMC Sysctl
==========

(SMC Sysctl)

/proc/sys/net/smc/* Variables(/proc/sys/net/smc/* 变量)
=======================================================

autocorking_size - INTEGER
	Setting SMC auto corking size:
	SMC auto corking is like TCP auto corking from the application's
	perspective of view. When applications do consecutive small
	write()/sendmsg() system calls, we try to coalesce these small writes
	as much as possible, to lower total amount of CDC and RDMA Write been
	sent.
	autocorking_size limits the maximum corked bytes that can be sent to
	the under device in 1 single sending. If set to 0, the SMC auto corking
	is disabled.
	Applications can still use TCP_CORK for optimal behavior when they
	know how/when to uncork their sockets.
	(设置 SMC 自动软木塞(auto corking)大小：从应用程序的角度看，SMC 自动软木塞
	与 TCP 自动软木塞类似。当应用程序进行连续的小型 write()/sendmsg() 系统调用
	时，我们会尝试尽可能多地合并这些小写入，以降低所发送的 CDC 与 RDMA Write 的
	总量。autocorking_size 限制单次发送可以发送到底层设备的最大软木塞字节数。如
	果设置为 0，则禁用 SMC 自动软木塞。当应用程序知道如何/何时打开其套接字的软
	木塞时，仍然可以使用 TCP_CORK 来获得最佳行为。)

	Default: 64K
	(默认：64K)

smcr_buf_type - INTEGER
	Controls which type of sndbufs and RMBs to use in later newly created
	SMC-R link group. Only for SMC-R.
	(控制在之后新建的 SMC-R 链路组中使用哪种类型的 sndbuf 和 RMB。仅适用于
	SMC-R。)

	Default: 0 (physically contiguous sndbufs and RMBs)
	(默认：0(物理连续的 sndbuf 和 RMB))

	Possible values:
	(可能的取值：)

	- 0 - Use physically contiguous buffers (使用物理连续的缓冲区)
	- 1 - Use virtually contiguous buffers (使用虚拟连续的缓冲区)
	- 2 - Mixed use of the two types. Try physically contiguous buffers first.
	  If not available, use virtually contiguous buffers then.
	  (混合使用两种类型。先尝试物理连续的缓冲区，如果不可用，再使用虚拟连续的
	  缓冲区。)

smcr_testlink_time - INTEGER
	How frequently SMC-R link sends out TEST_LINK LLC messages to confirm
	viability, after the last activity of connections on it. Value 0 means
	disabling TEST_LINK.
	(在链路上连接的最后一次活动之后，SMC-R 链路以多高的频率发送 TEST_LINK LLC
	消息以确认链路可用性。值 0 表示禁用 TEST_LINK。)

	Default: 30 seconds.
	(默认：30 秒。)

wmem - INTEGER
	Initial size of send buffer used by SMC sockets.
	(SMC 套接字使用的发送缓冲区的初始大小。)

	The minimum value is 16KiB and there is no hard limit for max value, but
	only allowed 512KiB for SMC-R and 1MiB for SMC-D.
	(最小值为 16KiB，最大值没有硬性限制，但 SMC-R 只允许 512KiB，SMC-D 只允许
	1MiB。)

	Default: 64KiB
	(默认：64KiB)

rmem - INTEGER
	Initial size of receive buffer (RMB) used by SMC sockets.
	(SMC 套接字使用的接收缓冲区(RMB)的初始大小。)

	The minimum value is 16KiB and there is no hard limit for max value, but
	only allowed 512KiB for SMC-R and 1MiB for SMC-D.
	(最小值为 16KiB，最大值没有硬性限制，但 SMC-R 只允许 512KiB，SMC-D 只允许
	1MiB。)

	Default: 64KiB
	(默认：64KiB)

smcr_max_links_per_lgr - INTEGER
	Controls the max number of links can be added to a SMC-R link group. Notice that
	the actual number of the links added to a SMC-R link group depends on the number
	of RDMA devices exist in the system. The acceptable value ranges from 1 to 2. Only
	for SMC-R v2.1 and later.
	(控制可以添加到一个 SMC-R 链路组的链路的最大数量。注意，实际添加到 SMC-R 链
	路组的链路数量取决于系统中存在的 RDMA 设备数量。可接受的取值范围是 1 到 2。
	仅适用于 SMC-R v2.1 及更高版本。)

	Default: 2
	(默认：2)

smcr_max_conns_per_lgr - INTEGER
	Controls the max number of connections can be added to a SMC-R link group. The
	acceptable value ranges from 16 to 255. Only for SMC-R v2.1 and later.
	(控制可以添加到一个 SMC-R 链路组的连接的最大数量。可接受的取值范围是 16 到
	255。仅适用于 SMC-R v2.1 及更高版本。)

	Default: 255
	(默认：255)

smcr_max_send_wr - INTEGER
	So-called work request buffers are SMCR link (and RDMA queue pair) level
	resources necessary for performing RDMA operations. Since up to 255
	connections can share a link group and thus also a link and the number
	of the work request buffers is decided when the link is allocated,
	depending on the workload it can be a bottleneck in a sense that threads
	have to wait for work request buffers to become available. Before the
	introduction of this control the maximal number of work request buffers
	available on the send path used to be hard coded to 16. With this control
	it becomes configurable. The acceptable range is between 2 and 2048.
	(所谓的工作请求缓冲区(work request buffer)是 SMCR 链路(以及 RDMA 队列对)
	级别的资源，是执行 RDMA 操作所必需的。由于最多 255 个连接可以共享一个链路组，
	因而也共享一条链路，而工作请求缓冲区的数量在链路分配时就已确定，所以根据工作
	负载的不同，它可能在某种意义上成为瓶颈：线程必须等待工作请求缓冲区变为可用。
	在引入此控制之前，发送路径上可用的工作请求缓冲区的最大数量被硬编码为 16。有
	了此控制，它就变为可配置的。可接受的范围是 2 到 2048。)

	Please be aware that all the buffers need to be allocated as a physically
	continuous array in which each element is a single buffer and has the size
	of SMC_WR_BUF_SIZE (48) bytes. If the allocation fails, we keep retrying
	with half of the buffer count until it is ether successful or (unlikely)
	we dip below the old hard coded value which is 16 where we give up much
	like before having this control.
	(请注意，所有缓冲区都需要分配为一个物理连续的数组，其中每个元素是单个缓冲区，
	大小为 SMC_WR_BUF_SIZE (48) 字节。如果分配失败，我们会用一半的缓冲区数量不断
	重试，直到成功，或者(不太可能)降到旧的硬编码值 16 以下——此时我们会放弃，
	就像没有此控制时一样。)

	Default: 16
	(默认：16)

smcr_max_recv_wr - INTEGER
	So-called work request buffers are SMCR link (and RDMA queue pair) level
	resources necessary for performing RDMA operations. Since up to 255
	connections can share a link group and thus also a link and the number
	of the work request buffers is decided when the link is allocated,
	depending on the workload it can be a bottleneck in a sense that threads
	have to wait for work request buffers to become available. Before the
	introduction of this control the maximal number of work request buffers
	available on the receive path used to be hard coded to 16. With this control
	it becomes configurable. The acceptable range is between 2 and 2048.
	(所谓的工作请求缓冲区(work request buffer)是 SMCR 链路(以及 RDMA 队列对)
	级别的资源，是执行 RDMA 操作所必需的。由于最多 255 个连接可以共享一个链路组，
	因而也共享一条链路，而工作请求缓冲区的数量在链路分配时就已确定，所以根据工作
	负载的不同，它可能在某种意义上成为瓶颈：线程必须等待工作请求缓冲区变为可用。
	在引入此控制之前，接收路径上可用的工作请求缓冲区的最大数量被硬编码为 16。有
	了此控制，它就变为可配置的。可接受的范围是 2 到 2048。)

	Please be aware that all the buffers need to be allocated as a physically
	continuous array in which each element is a single buffer and has the size
	of SMC_WR_BUF_SIZE (48) bytes. If the allocation fails, we keep retrying
	with half of the buffer count until it is ether successful or (unlikely)
	we dip below the old hard coded value which is 16 where we give up much
	like before having this control.
	(请注意，所有缓冲区都需要分配为一个物理连续的数组，其中每个元素是单个缓冲区，
	大小为 SMC_WR_BUF_SIZE (48) 字节。如果分配失败，我们会用一半的缓冲区数量不断
	重试，直到成功，或者(不太可能)降到旧的硬编码值 16 以下——此时我们会放弃，
	就像没有此控制时一样。)

	Default: 48
	(默认：48)

limit_smc_hs - INTEGER
	Whether to limit SMC handshake for newly created sockets.
	(是否限制新建套接字的 SMC 握手。)

	When enabled, SMC listen path applies handshake limitation based on
	handshake worker congestion and queued SMC handshake load.
	(启用后，SMC 监听路径会基于握手工作线程的拥塞情况和排队的 SMC 握手负载来
	应用握手限制。)

	Possible values:
	(可能的取值：)

	- 0 - Disable handshake limitation (禁用握手限制)
	- 1 - Enable handshake limitation (启用握手限制)

	Default: 0 (disable)
	(默认：0(禁用))

hs_ctrl - STRING
	Select the SMC handshake control profile by name.
	(按名称选择 SMC 握手控制配置档(profile)。)

	This string refers to the name of a user-implemented
	BPF struct_ops instance of type smc_hs_ctrl.
	(该字符串引用用户实现的 smc_hs_ctrl 类型 BPF struct_ops 实例的名称。)

	The selected profile controls whether SMC options are advertised
	during TCP SYN/SYN-ACK handshake.
	(所选配置档控制在 TCP SYN/SYN-ACK 握手期间是否通告 SMC 选项。)

	Only available when CONFIG_SMC_HS_CTRL_BPF is enabled.
	Write an empty string to clear the current profile.
	(仅在 CONFIG_SMC_HS_CTRL_BPF 启用时可用。写入空字符串可清除当前配置档。)

	Default: empty string
	(默认：空字符串)
