.. SPDX-License-Identifier: GPL-2.0

===============================
Device Memory TCP(设备内存 TCP)
===============================


Intro(简介)
===========

Device memory TCP (devmem TCP) enables receiving data directly into device
memory (dmabuf). The feature is currently implemented for TCP sockets.
(设备内存 TCP (devmem TCP) 支持将数据直接接收到设备内存 (dmabuf) 中。该特性
目前针对 TCP 套接字实现。)


Opportunity(机遇)
-----------------

A large number of data transfers have device memory as the source and/or
destination. Accelerators drastically increased the prevalence of such
transfers.  Some examples include:
(大量数据传输以设备内存作为源和/或目的。加速器极大地提高了这类传输的普遍
程度。例如：)

- Distributed training, where ML accelerators, such as GPUs on different hosts,
  exchange data.
  (分布式训练，其中位于不同主机上的 GPU 等 ML 加速器之间相互交换数据。)

- Distributed raw block storage applications transfer large amounts of data with
  remote SSDs. Much of this data does not require host processing.
  (分布式裸块存储应用与远程 SSD 之间传输大量数据。其中许多数据并不需要主机
  处理。)

Typically the Device-to-Device data transfers in the network are implemented as
the following low-level operations: Device-to-Host copy, Host-to-Host network
transfer, and Host-to-Device copy.
(通常，网络中的设备到设备 (Device-to-Device) 数据传输由以下底层操作实现：
设备到主机复制、主机到主机网络传输，以及主机到设备复制。)

The flow involving host copies is suboptimal, especially for bulk data transfers,
and can put significant strains on system resources such as host memory
bandwidth and PCIe bandwidth.
(涉及主机复制的流程并不理想，对大块数据传输而言尤其如此，并且会给主机内存
带宽、PCIe 带宽等系统资源带来很大压力。)

Devmem TCP optimizes this use case by implementing socket APIs that enable
the user to receive incoming network packets directly into device memory.
(Devmem TCP 通过实现让用户能够把传入的网络数据包直接接收到设备内存中的套接字
API，来优化这一使用场景。)

Packet payloads go directly from the NIC to device memory.
(数据包载荷直接从 NIC 进入设备内存。)

Packet headers go to host memory and are processed by the TCP/IP stack
normally. The NIC must support header split to achieve this.
(数据包头部进入主机内存，并由 TCP/IP 协议栈按正常方式处理。为此 NIC 必须支持
头部拆分 (header split)。)

Advantages:
(优点：)

- Alleviate host memory bandwidth pressure, compared to existing
  network-transfer + device-copy semantics.
  (与现有的网络传输 + 设备复制方式相比，减轻主机内存带宽压力。)

- Alleviate PCIe bandwidth pressure, by limiting data transfer to the lowest
  level of the PCIe tree, compared to the traditional path which sends data
  through the root complex.
  (通过将数据传输限制在 PCIe 树的最低层级，减轻 PCIe 带宽压力；而传统路径要
  经过根复合体 (root complex) 发送数据。)


More Info(更多信息)
-------------------

  slides, video
  (幻灯片与视频)

    https://netdevconf.org/0x17/sessions/talk/device-memory-tcp.html

  patchset
  (补丁集)

    [PATCH net-next v24 00/13] Device Memory TCP
    https://lore.kernel.org/netdev/20240831004313.3713467-1-almasrymina@google.com/


RX Interface(RX 接口)
=====================


Example(示例)
-------------

./tools/testing/selftests/drivers/net/hw/ncdevmem:do_server shows an example of
setting up the RX path of this API.
(./tools/testing/selftests/drivers/net/hw/ncdevmem:do_server 展示了设置该 API
RX 路径的示例。)


NIC Setup(NIC 设置)
-------------------

Header split, flow steering, & RSS are required features for devmem TCP.
(头部拆分 (header split)、流 steering 与 RSS 是 devmem TCP 所需的特性。)

Header split is used to split incoming packets into a header buffer in host
memory, and a payload buffer in device memory.
(头部拆分用于把传入的数据包拆分为主机内存中的头部缓冲区，以及设备内存中的
载荷缓冲区。)

Flow steering & RSS are used to ensure that only flows targeting devmem land on
an RX queue bound to devmem.
(流 steering 与 RSS 用于确保只有以 devmem 为目标的流才会落到绑定至 devmem 的
RX 队列上。)

Enable header split & flow steering::
(启用头部拆分与流 steering：)

	# enable header split
	ethtool -G eth1 tcp-data-split on


	# enable flow steering
	ethtool -K eth1 ntuple on

Configure RSS to steer all traffic away from the target RX queue (queue 15 in
this example)::
(配置 RSS，把所有流量都从目标 RX 队列 (此例中为队列 15) 引开：)

	ethtool --set-rxfh-indir eth1 equal 15


The user must bind a dmabuf to any number of RX queues on a given NIC using
the netlink API::
(用户必须使用 netlink API 将 dmabuf 绑定到指定 NIC 上的任意数量的 RX 队列：)

	/* Bind dmabuf to NIC RX queue 15 */
	struct netdev_queue *queues;
	queues = malloc(sizeof(*queues) * 1);

	queues[0]._present.type = 1;
	queues[0]._present.idx = 1;
	queues[0].type = NETDEV_RX_QUEUE_TYPE_RX;
	queues[0].idx = 15;

	*ys = ynl_sock_create(&ynl_netdev_family, &yerr);

	req = netdev_bind_rx_req_alloc();
	netdev_bind_rx_req_set_ifindex(req, 1 /* ifindex */);
	netdev_bind_rx_req_set_dmabuf_fd(req, dmabuf_fd);
	__netdev_bind_rx_req_set_queues(req, queues, n_queue_index);

	rsp = netdev_bind_rx(*ys, req);

	dmabuf_id = rsp->dmabuf_id;


The netlink API returns a dmabuf_id: a unique ID that refers to this dmabuf
that has been bound.
(netlink API 返回一个 dmabuf_id：即指向这个已绑定 dmabuf 的唯一 ID。)

The user can unbind the dmabuf from the netdevice by closing the netlink socket
that established the binding. We do this so that the binding is automatically
unbound even if the userspace process crashes.
(用户可以通过关闭建立绑定的 netlink 套接字来解除 dmabuf 与网络设备的绑定。
这样做的目的是，即使用户态进程崩溃，绑定也会自动解除。)

Note that any reasonably well-behaved dmabuf from any exporter should work with
devmem TCP, even if the dmabuf is not actually backed by devmem. An example of
this is udmabuf, which wraps user memory (non-devmem) in a dmabuf.
(注意，来自任何导出者的任何行为规范的 dmabuf 都应当可用于 devmem TCP，即使该
dmabuf 实际上并非由设备内存支持。udmabuf 就是一例，它把用户内存 (非 devmem)
包装在 dmabuf 中。)


Socket Setup(套接字设置)
------------------------

The socket must be flow steered to the dmabuf bound RX queue::
(必须通过流 steering 把套接字导向绑定 dmabuf 的 RX 队列：)

	ethtool -N eth1 flow-type tcp4 ... queue 15


Receiving data(接收数据)
------------------------

The user application must signal to the kernel that it is capable of receiving
devmem data by passing the MSG_SOCK_DEVMEM flag to recvmsg::
(用户应用必须通过向 recvmsg 传入 MSG_SOCK_DEVMEM 标志，向内核表明自己能够
接收 devmem 数据：)

	ret = recvmsg(fd, &msg, MSG_SOCK_DEVMEM);

Applications that do not specify the MSG_SOCK_DEVMEM flag will receive an EFAULT
on devmem data.
(未指定 MSG_SOCK_DEVMEM 标志的应用在遇到 devmem 数据时会得到 EFAULT。)

Devmem data is received directly into the dmabuf bound to the NIC in 'NIC
Setup', and the kernel signals such to the user via the SCM_DEVMEM_* cmsgs::
(Devmem 数据直接接收到 "NIC 设置" 一节中绑定到 NIC 的 dmabuf 里，内核通过
SCM_DEVMEM_* cmsg 把这一情况告知用户：)

		for (cm = CMSG_FIRSTHDR(&msg); cm; cm = CMSG_NXTHDR(&msg, cm)) {
			if (cm->cmsg_level != SOL_SOCKET ||
				(cm->cmsg_type != SCM_DEVMEM_DMABUF &&
				 cm->cmsg_type != SCM_DEVMEM_LINEAR))
				continue;

			dmabuf_cmsg = (struct dmabuf_cmsg *)CMSG_DATA(cm);

			if (cm->cmsg_type == SCM_DEVMEM_DMABUF) {
				/* Frag landed in dmabuf.
				 *
				 * dmabuf_cmsg->dmabuf_id is the dmabuf the
				 * frag landed on.
				 *
				 * dmabuf_cmsg->frag_offset is the offset into
				 * the dmabuf where the frag starts.
				 *
				 * dmabuf_cmsg->frag_size is the size of the
				 * frag.
				 *
				 * dmabuf_cmsg->frag_token is a token used to
				 * refer to this frag for later freeing.
				 */

				struct dmabuf_token token;
				token.token_start = dmabuf_cmsg->frag_token;
				token.token_count = 1;
				continue;
			}

			if (cm->cmsg_type == SCM_DEVMEM_LINEAR)
				/* Frag landed in linear buffer.
				 *
				 * dmabuf_cmsg->frag_size is the size of the
				 * frag.
				 */
				continue;

		}

Applications may receive 2 cmsgs:
(应用可能收到 2 个 cmsg：)

- SCM_DEVMEM_DMABUF: this indicates the fragment landed in the dmabuf indicated
  by dmabuf_id.
  (SCM_DEVMEM_DMABUF：表示该分片落在了 dmabuf_id 所指示的 dmabuf 中。)

- SCM_DEVMEM_LINEAR: this indicates the fragment landed in the linear buffer.
  This typically happens when the NIC is unable to split the packet at the
  header boundary, such that part (or all) of the payload landed in host
  memory.
  (SCM_DEVMEM_LINEAR：表示该分片落在了线性缓冲区中。这通常发生在 NIC 无法在
  头部边界处拆分数据包时，导致部分 (或全部) 载荷落在主机内存中。)

Applications may receive no SO_DEVMEM_* cmsgs. That indicates non-devmem,
regular TCP data that landed on an RX queue not bound to a dmabuf.
(应用也可能收不到任何 SO_DEVMEM_* cmsg。这表示落在未绑定 dmabuf 的 RX 队列上
的是非 devmem 的普通 TCP 数据。)


Freeing frags(释放分片)
-----------------------

Frags received via SCM_DEVMEM_DMABUF are pinned by the kernel while the user
processes the frag. The user must return the frag to the kernel via
SO_DEVMEM_DONTNEED::
(在用户处理分片期间，通过 SCM_DEVMEM_DMABUF 收到的分片由内核固定 (pin)。用户
必须通过 SO_DEVMEM_DONTNEED 把分片归还给内核：)

	ret = setsockopt(client_fd, SOL_SOCKET, SO_DEVMEM_DONTNEED, &token,
			 sizeof(token));

The user must ensure the tokens are returned to the kernel in a timely manner.
Failure to do so will exhaust the limited dmabuf that is bound to the RX queue
and will lead to packet drops.
(用户必须确保及时把 token 归还给内核。否则会耗尽绑定到 RX 队列的有限 dmabuf，
进而导致丢包。)

The user must pass no more than 128 tokens, with no more than 1024 total frags
among the token->token_count across all the tokens. If the user provides more
than 1024 frags, the kernel will free up to 1024 frags and return early.
(用户最多只能传入 128 个 token，且所有 token 的 token->token_count 加起来的
分片总数不得超过 1024。如果用户提供的分片超过 1024 个，内核最多释放 1024 个
分片后提前返回。)

The kernel returns the number of actual frags freed. The number of frags freed
can be less than the tokens provided by the user in case of:
(内核返回实际释放的分片数量。在以下情况下，实际释放的分片数量可能小于用户
提供的 token 数：)

(a) an internal kernel leak bug.
(a) 内核内部存在泄漏 bug。
(b) the user passed more than 1024 frags.
(b) 用户传入的分片超过 1024 个。

TX Interface(TX 接口)
=====================


Example(示例)
-------------

./tools/testing/selftests/drivers/net/hw/ncdevmem:do_client shows an example of
setting up the TX path of this API.
(./tools/testing/selftests/drivers/net/hw/ncdevmem:do_client 展示了设置该 API
TX 路径的示例。)


NIC Setup(NIC 设置)
-------------------

The user must bind a TX dmabuf to a given NIC using the netlink API::
(用户必须使用 netlink API 将 TX dmabuf 绑定到指定 NIC：)

        struct netdev_bind_tx_req *req = NULL;
        struct netdev_bind_tx_rsp *rsp = NULL;
        struct ynl_error yerr;

        *ys = ynl_sock_create(&ynl_netdev_family, &yerr);

        req = netdev_bind_tx_req_alloc();
        netdev_bind_tx_req_set_ifindex(req, ifindex);
        netdev_bind_tx_req_set_fd(req, dmabuf_fd);

        rsp = netdev_bind_tx(*ys, req);

        tx_dmabuf_id = rsp->id;


The netlink API returns a dmabuf_id: a unique ID that refers to this dmabuf
that has been bound.
(netlink API 返回一个 dmabuf_id：即指向这个已绑定 dmabuf 的唯一 ID。)

The user can unbind the dmabuf from the netdevice by closing the netlink socket
that established the binding. We do this so that the binding is automatically
unbound even if the userspace process crashes.
(用户可以通过关闭建立绑定的 netlink 套接字来解除 dmabuf 与网络设备的绑定。
这样做的目的是，即使用户态进程崩溃，绑定也会自动解除。)

Note that any reasonably well-behaved dmabuf from any exporter should work with
devmem TCP, even if the dmabuf is not actually backed by devmem. An example of
this is udmabuf, which wraps user memory (non-devmem) in a dmabuf.
(注意，来自任何导出者的任何行为规范的 dmabuf 都应当可用于 devmem TCP，即使该
dmabuf 实际上并非由设备内存支持。udmabuf 就是一例，它把用户内存 (非 devmem)
包装在 dmabuf 中。)

Socket Setup(套接字设置)
------------------------

The user application must use MSG_ZEROCOPY flag when sending devmem TCP. Devmem
cannot be copied by the kernel, so the semantics of the devmem TX are similar
to the semantics of MSG_ZEROCOPY::
(用户应用在发送 devmem TCP 数据时必须使用 MSG_ZEROCOPY 标志。内核无法复制
devmem，因此 devmem TX 的语义与 MSG_ZEROCOPY 的语义类似：)

	setsockopt(socket_fd, SOL_SOCKET, SO_ZEROCOPY, &opt, sizeof(opt));

It is also recommended that the user binds the TX socket to the same interface
the dma-buf has been bound to via SO_BINDTODEVICE::
(另外还建议用户通过 SO_BINDTODEVICE 把 TX 套接字绑定到 dma-buf 所绑定的同一
接口上：)

	setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen(ifname) + 1);


Sending data(发送数据)
----------------------

Devmem data is sent using the SCM_DEVMEM_DMABUF cmsg.
(Devmem 数据通过 SCM_DEVMEM_DMABUF cmsg 发送。)

The user should create a msghdr where,
(用户应创建一个 msghdr，其中：)

* iov_base is set to the offset into the dmabuf to start sending from
  (iov_base 设置为 dmabuf 中开始发送处的偏移量)
* iov_len is set to the number of bytes to be sent from the dmabuf
  (iov_len 设置为要从 dmabuf 发送的字节数)

The user passes the dma-buf id to send from via the dmabuf_tx_cmsg.dmabuf_id.
(用户通过 dmabuf_tx_cmsg.dmabuf_id 传递要从中发送的 dma-buf id。)

The example below sends 1024 bytes from offset 100 into the dmabuf, and 2048
from offset 2000 into the dmabuf. The dmabuf to send from is tx_dmabuf_id::
(下面的示例从 dmabuf 中偏移 100 处发送 1024 字节，从偏移 2000 处发送 2048 字
节。发送所用的 dmabuf 为 tx_dmabuf_id：)

       char ctrl_data[CMSG_SPACE(sizeof(struct dmabuf_tx_cmsg))];
       struct dmabuf_tx_cmsg ddmabuf;
       struct msghdr msg = {};
       struct cmsghdr *cmsg;
       struct iovec iov[2];

       iov[0].iov_base = (void*)100;
       iov[0].iov_len = 1024;
       iov[1].iov_base = (void*)2000;
       iov[1].iov_len = 2048;

       msg.msg_iov = iov;
       msg.msg_iovlen = 2;

       msg.msg_control = ctrl_data;
       msg.msg_controllen = sizeof(ctrl_data);

       cmsg = CMSG_FIRSTHDR(&msg);
       cmsg->cmsg_level = SOL_SOCKET;
       cmsg->cmsg_type = SCM_DEVMEM_DMABUF;
       cmsg->cmsg_len = CMSG_LEN(sizeof(struct dmabuf_tx_cmsg));

       ddmabuf.dmabuf_id = tx_dmabuf_id;

       *((struct dmabuf_tx_cmsg *)CMSG_DATA(cmsg)) = ddmabuf;

       sendmsg(socket_fd, &msg, MSG_ZEROCOPY);


Reusing TX dmabufs(复用 TX dmabuf)
----------------------------------

Similar to MSG_ZEROCOPY with regular memory, the user should not modify the
contents of the dma-buf while a send operation is in progress. This is because
the kernel does not keep a copy of the dmabuf contents. Instead, the kernel
will pin and send data from the buffer available to the userspace.
(与对常规内存使用 MSG_ZEROCOPY 类似，在发送操作进行期间，用户不应修改
dma-buf 的内容。这是因为内核不会保留 dmabuf 内容的副本，而是直接从用户态可
见的缓冲区中固定 (pin) 并发送数据。)

Just as in MSG_ZEROCOPY, the kernel notifies the userspace of send completions
using MSG_ERRQUEUE::
(与 MSG_ZEROCOPY 一样，内核使用 MSG_ERRQUEUE 通知用户态发送已完成：)

        int64_t tstop = gettimeofday_ms() + waittime_ms;
        char control[CMSG_SPACE(100)] = {};
        struct sock_extended_err *serr;
        struct msghdr msg = {};
        struct cmsghdr *cm;
        int retries = 10;
        __u32 hi, lo;

        msg.msg_control = control;
        msg.msg_controllen = sizeof(control);

        while (gettimeofday_ms() < tstop) {
                if (!do_poll(fd)) continue;

                ret = recvmsg(fd, &msg, MSG_ERRQUEUE);

                for (cm = CMSG_FIRSTHDR(&msg); cm; cm = CMSG_NXTHDR(&msg, cm)) {
                        serr = (void *)CMSG_DATA(cm);

                        hi = serr->ee_data;
                        lo = serr->ee_info;

                        fprintf(stdout, "tx complete [%d,%d]\n", lo, hi);
                }
        }

After the associated sendmsg has been completed, the dmabuf can be reused by
the userspace.
(在相关 sendmsg 完成之后，用户态即可复用该 dmabuf。)


Implementation & Caveats(实现与注意事项)
========================================

Unreadable skbs(不可读的 skb)
-----------------------------

Devmem payloads are inaccessible to the kernel processing the packets. This
results in a few quirks for payloads of devmem skbs:
(处理这些数据包的内核无法访问 devmem 载荷。这给 devmem skb 的载荷带来了一些
特殊限制：)

- Loopback is not functional. Loopback relies on copying the payload, which is
  not possible with devmem skbs.
  (环回 (loopback) 功能不可用。环回依赖于复制载荷，而这对 devmem skb 来说是
  不可能的。)

- Software checksum calculation fails.
  (软件校验和计算会失败。)

- TCP Dump and bpf can't access devmem packet payloads.
  (TCP Dump 与 bpf 无法访问 devmem 数据包的载荷。)


Testing(测试)
=============

More realistic example code can be found in the kernel source under
``tools/testing/selftests/drivers/net/hw/ncdevmem.c``
(更贴近实际用法的示例代码可在内核源码的
``tools/testing/selftests/drivers/net/hw/ncdevmem.c`` 中找到。)

ncdevmem is a devmem TCP netcat. It works very similarly to netcat, but
receives data directly into a udmabuf.
(ncdevmem 是一个 devmem TCP 版的 netcat。它的工作方式与 netcat 非常相似，但
直接把数据接收到 udmabuf 中。)

To run ncdevmem, you need to run it on a server on the machine under test, and
you need to run netcat on a peer to provide the TX data.
(要运行 ncdevmem，需要在被测机器上以服务器方式运行它，并在对端运行 netcat
来提供 TX 数据。)

ncdevmem has a validation mode as well that expects a repeating pattern of
incoming data and validates it as such. For example, you can launch
ncdevmem on the server by::
(ncdevmem 还有一个校验模式，它期望传入的数据为重复模式并据此进行校验。例如，
可以在服务器上用以下命令启动 ncdevmem：)

	ncdevmem -s <server IP> -c <client IP> -f <ifname> -l -p 5201 -v 7

On client side, use regular netcat to send TX data to ncdevmem process
on the server::
(在客户端，使用常规 netcat 向服务器上的 ncdevmem 进程发送 TX 数据：)

	yes $(echo -e \\x01\\x02\\x03\\x04\\x05\\x06) | \
		tr \\n \\0 | head -c 5G | nc <server IP> 5201 -p 5201
