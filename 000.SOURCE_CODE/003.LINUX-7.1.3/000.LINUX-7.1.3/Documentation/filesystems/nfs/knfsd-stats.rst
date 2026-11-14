============================
Kernel NFS Server Statistics
============================

(内核 NFS 服务器统计信息)

:Authors: Greg Banks <gnb@sgi.com> - 26 Mar 2009
(作者: Greg Banks <gnb@sgi.com> - 2009年3月26日)

This document describes the format and semantics of the statistics
which the kernel NFS server makes available to userspace.  These
statistics are available in several text form pseudo files, each of
which is described separately below.
(本文档描述内核 NFS 服务器提供给用户空间的统计信息的格式与语义。这些统计信息以几个文本形式的伪文件提供，下文将分别描述。)

In most cases you don't need to know these formats, as the nfsstat(8)
program from the nfs-utils distribution provides a helpful command-line
interface for extracting and printing them.
(在大多数情况下你无需了解这些格式，因为 nfs-utils 发行版中的 nfsstat(8) 程序提供了一个便于提取和打印它们的命令行接口。)

All the files described here are formatted as a sequence of text lines,
separated by newline '\n' characters.  Lines beginning with a hash
'#' character are comments intended for humans and should be ignored
by parsing routines.  All other lines contain a sequence of fields
separated by whitespace.
(这里描述的所有文件都格式化为一系列文本行，以换行符 '\n' 分隔。以井号 '#' 开头的行是面向人类的注释，解析例程应将其忽略。所有其他行包含一系列以空白分隔的字段。)

/proc/fs/nfsd/pool_stats
(/proc/fs/nfsd/pool_stats)
========================

This file is available in kernels from 2.6.30 onwards, if the
/proc/fs/nfsd filesystem is mounted (it almost always should be).
(从 2.6.30 内核起，如果 /proc/fs/nfsd 文件系统已挂载(几乎总是应当挂载)，该文件即可用。)

The first line is a comment which describes the fields present in
all the other lines.  The other lines present the following data as
a sequence of unsigned decimal numeric fields.  One line is shown
for each NFS thread pool.
(第一行是一条注释，描述了其他所有行中出现的字段。其余各行以一系列无符号十进制数字字段的形式呈现以下数据。每个 NFS 线程池显示一行。)

All counters are 64 bits wide and wrap naturally.  There is no way
to zero these counters, instead applications should do their own
rate conversion.
(所有计数器都是 64 位宽并自然回绕。这些计数器无法清零，应用程序应当自行进行速率换算。)

pool
	The id number of the NFS thread pool to which this line applies.
	This number does not change.
	(pool：本行适用的 NFS 线程池的 id 编号。该编号不会改变。)

	Thread pool ids are a contiguous set of small integers starting
	at zero.  The maximum value depends on the thread pool mode, but
	currently cannot be larger than the number of CPUs in the system.
	Note that in the default case there will be a single thread pool
	which contains all the nfsd threads and all the CPUs in the system,
	and thus this file will have a single line with a pool id of "0".
	(线程池 id 是从零开始的一组连续小整数。最大值取决于线程池模式，但目前不会大于系统中的 CPU 数量。注意，在默认情况下将只有一个线程池，它包含所有 nfsd 线程和系统中的全部 CPU，因此该文件将只有一行，其 pool id 为 "0"。)

packets-arrived
	Counts how many NFS packets have arrived.  More precisely, this
	is the number of times that the network stack has notified the
	sunrpc server layer that new data may be available on a transport
	(e.g. an NFS or UDP socket or an NFS/RDMA endpoint).
	(packets-arrived(到达的数据包)：统计有多少 NFS 数据包到达。更精确地说，它是网络栈通知 sunrpc 服务器层"某个传输上可能有新数据可用"的次数(例如 NFS 或 UDP 套接字，或 NFS/RDMA 端点)。)

	Depending on the NFS workload patterns and various network stack
	effects (such as Large Receive Offload) which can combine packets
	on the wire, this may be either more or less than the number
	of NFS calls received (which statistic is available elsewhere).
	However this is a more accurate and less workload-dependent measure
	of how much CPU load is being placed on the sunrpc server layer
	due to NFS network traffic.
	(取决于 NFS 工作负载模式以及各种能够在网络上合并数据包的网络栈效应(例如大型接收卸载(Large Receive Offload))，该值可能多于也可能少于收到的 NFS 调用数(该统计可在其他地方获得)。不过，对于因 NFS 网络流量而施加在 sunrpc 服务器层上的 CPU 负载，这是一个更准确、对工作负载依赖更小的度量。)

sockets-enqueued
	Counts how many times an NFS transport is enqueued to wait for
	an nfsd thread to service it, i.e. no nfsd thread was considered
	available.
	(sockets-enqueued(入队的套接字)：统计 NFS 传输被入队以等待某个 nfsd 线程来服务它的次数，即当时没有被认为可用的 nfsd 线程。)

	The circumstance this statistic tracks indicates that there was NFS
	network-facing work to be done but it couldn't be done immediately,
	thus introducing a small delay in servicing NFS calls.  The ideal
	rate of change for this counter is zero; significantly non-zero
	values may indicate a performance limitation.
	(该统计所跟踪的情况表明，存在需要处理的 NFS 网络侧工作但无法立即完成，从而给 NFS 调用的服务带来少量延迟。该计数器的理想变化速率为零；明显非零的值可能表示存在性能瓶颈。)

	This can happen because there are too few nfsd threads in the thread
	pool for the NFS workload (the workload is thread-limited), in which
	case configuring more nfsd threads will probably improve the
	performance of the NFS workload.
	(发生这种情况可能是因为线程池中的 nfsd 线程数量相对 NFS 工作负载太少(工作负载受线程数限制)，在这种情况下，配置更多 nfsd 线程很可能会改善 NFS 工作负载的性能。)

threads-woken
	Counts how many times an idle nfsd thread is woken to try to
	receive some data from an NFS transport.
	(threads-woken(被唤醒的线程)：统计空闲的 nfsd 线程被唤醒以尝试从 NFS 传输接收一些数据的次数。)

	This statistic tracks the circumstance where incoming
	network-facing NFS work is being handled quickly, which is a good
	thing.  The ideal rate of change for this counter will be close
	to but less than the rate of change of the packets-arrived counter.
	(该统计跟踪的是传入的 NFS 网络侧工作正在被快速处理的情况，这是件好事。该计数器的理想变化速率将接近但小于 packets-arrived 计数器的变化速率。)

threads-timedout
	Counts how many times an nfsd thread triggered an idle timeout,
	i.e. was not woken to handle any incoming network packets for
	some time.
	(threads-timedout(超时的线程)：统计 nfsd 线程触发空闲超时的次数，即有一段时间未被唤醒去处理任何传入的网络数据包。)

	This statistic counts a circumstance where there are more nfsd
	threads configured than can be used by the NFS workload.  This is
	a clue that the number of nfsd threads can be reduced without
	affecting performance.  Unfortunately, it's only a clue and not
	a strong indication, for a couple of reasons:
	(该统计记录的是所配置的 nfsd 线程数量多于 NFS 工作负载所能使用的数量的情况。这是一个线索，表明可以在不影响性能的情况下减少 nfsd 线程的数量。遗憾的是，它只是一个线索而非强有力的迹象，原因有以下几点：)

	 - Currently the rate at which the counter is incremented is quite
	   slow; the idle timeout is 60 minutes.  Unless the NFS workload
	   remains constant for hours at a time, this counter is unlikely
	   to be providing information that is still useful.
	   (目前该计数器递增的速率相当慢；空闲超时为 60 分钟。除非 NFS 工作负载连续数小时保持不变，否则该计数器提供的信息很可能已经不再有用。)

	 - It is usually a wise policy to provide some slack,
	   i.e. configure a few more nfsds than are currently needed,
	   to allow for future spikes in load.
	   (通常明智的做法是留出一些余量，即配置比当前所需略多的 nfsd，以应对未来可能出现的负载高峰。)


Note that incoming packets on NFS transports will be dealt with in
one of three ways.  An nfsd thread can be woken (threads-woken counts
this case), or the transport can be enqueued for later attention
(sockets-enqueued counts this case), or the packet can be temporarily
deferred because the transport is currently being used by an nfsd
thread.  This last case is not very interesting and is not explicitly
counted, but can be inferred from the other counters thus::
(注意，NFS 传输上的传入数据包将以三种方式之一得到处理。可以唤醒一个 nfsd 线程(threads-woken 统计这种情况)，或者该传输可以被入队以待后续处理(sockets-enqueued 统计这种情况)，或者该数据包可能被暂时推迟，因为该传输当前正被某个 nfsd 线程使用。最后一种情况不太值得关注，也没有被显式计数，但可以从其他计数器推断出来，如下所示：)

	packets-deferred = packets-arrived - ( sockets-enqueued + threads-woken )


More
(更多)
====

Descriptions of the other statistics file should go here.
(对其他统计文件的描述应放在这里。)
