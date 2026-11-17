.. SPDX-License-Identifier: GPL-2.0
.. _xfrm_device:

===============================================
XFRM device - offloading the IPsec computations
===============================================

(XFRM 设备——卸载 IPsec 计算)

Shannon Nelson <shannon.nelson@oracle.com>
Leon Romanovsky <leonro@nvidia.com>


Overview
(概述)
========

IPsec is a useful feature for securing network traffic, but the
computational cost is high: a 10Gbps link can easily be brought down
to under 1Gbps, depending on the traffic and link configuration.
Luckily, there are NICs that offer a hardware based IPsec offload which
can radically increase throughput and decrease CPU utilization.  The XFRM
Device interface allows NIC drivers to offer to the stack access to the
hardware offload.
(IPsec 是保护网络流量的有用特性，但计算成本很高：取决于流量和链路配置，一条 10Gbps 链路的吞吐量很容易被拉低
到 1Gbps 以下。幸运的是，有些网卡提供基于硬件的 IPsec 卸载，能大幅提升吞吐量并降低 CPU 占用。XFRM Device 接
口允许网卡驱动向协议栈提供对硬件卸载的访问。)

Right now, there are two types of hardware offload that kernel supports:
(目前，内核支持两种类型的硬件卸载：)

 * IPsec crypto offload:
   (IPsec 加密卸载：)

   * NIC performs encrypt/decrypt
     (网卡执行加密/解密)
   * Kernel does everything else
     (内核完成其余所有工作)

 * IPsec packet offload:
   (IPsec 数据包卸载：)

   * NIC performs encrypt/decrypt
     (网卡执行加密/解密)
   * NIC does encapsulation
     (网卡执行封装)
   * Kernel and NIC have SA and policy in-sync
     (内核与网卡保持 SA 和策略同步)
   * NIC handles the SA and policies states
     (网卡负责维护 SA 和策略的状态)
   * The Kernel talks to the keymanager
     (内核与密钥管理器通信)

Userland access to the offload is typically through a system such as
libreswan or KAME/raccoon, but the iproute2 'ip xfrm' command set can
be handy when experimenting.  An example command might look something
like this for crypto offload::
(对卸载的用户空间访问通常通过 libreswan 或 KAME/raccoon 之类的系统完成，但在试验时 iproute2 的 'ip xfrm'
命令集也很方便。对于加密卸载，一条示例命令大致如下：)

  ip x s add proto esp dst 14.0.0.70 src 14.0.0.52 spi 0x07 mode transport \
     reqid 0x07 replay-window 32 \
     aead 'rfc4106(gcm(aes))' 0x44434241343332312423222114131211f4f3f2f1 128 \
     sel src 14.0.0.52/24 dst 14.0.0.70/24 proto tcp \
     offload dev eth4 dir in

and for packet offload::
(而对于数据包卸载：)

  ip x s add proto esp dst 14.0.0.70 src 14.0.0.52 spi 0x07 mode transport \
     reqid 0x07 replay-window 32 \
     aead 'rfc4106(gcm(aes))' 0x44434241343332312423222114131211f4f3f2f1 128 \
     sel src 14.0.0.52/24 dst 14.0.0.70/24 proto tcp \
     offload packet dev eth4 dir in

  ip x p add src 14.0.0.70 dst 14.0.0.52 offload packet dev eth4 dir in
  tmpl src 14.0.0.70 dst 14.0.0.52 proto esp reqid 10000 mode transport

Yes, that's ugly, but that's what shell scripts and/or libreswan are for.
(是的，这确实很难看，但这正是 shell 脚本和/或 libreswan 的用武之地。)



Callbacks to implement
(需要实现的回调)
======================

::

  /* from include/linux/netdevice.h */
  struct xfrmdev_ops {
        /* Crypto and Packet offload callbacks */
	int	(*xdo_dev_state_add)(struct net_device *dev,
                                     struct xfrm_state *x,
                                     struct netlink_ext_ack *extack);
	void	(*xdo_dev_state_delete)(struct net_device *dev,
                                        struct xfrm_state *x);
	void	(*xdo_dev_state_free)(struct net_device *dev,
                                      struct xfrm_state *x);
	bool	(*xdo_dev_offload_ok) (struct sk_buff *skb,
				       struct xfrm_state *x);
	void    (*xdo_dev_state_advance_esn) (struct xfrm_state *x);
	void    (*xdo_dev_state_update_stats) (struct xfrm_state *x);

        /* Solely packet offload callbacks */
	int	(*xdo_dev_policy_add) (struct xfrm_policy *x, struct netlink_ext_ack *extack);
	void	(*xdo_dev_policy_delete) (struct xfrm_policy *x);
	void	(*xdo_dev_policy_free) (struct xfrm_policy *x);
  };

The NIC driver offering ipsec offload will need to implement callbacks
relevant to supported offload to make the offload available to the network
stack's XFRM subsystem. Additionally, the feature bits NETIF_F_HW_ESP and
NETIF_F_HW_ESP_TX_CSUM will signal the availability of the offload.
(提供 IPsec 卸载的网卡驱动需要实现与其所支持卸载相关的回调，以便将卸载功能提供给网络协议栈的 XFRM 子系统。
此外，特性位 NETIF_F_HW_ESP 和 NETIF_F_HW_ESP_TX_CSUM 用于标识卸载功能的可用性。)



Flow
(流程)
====

At probe time and before the call to register_netdev(), the driver should
set up local data structures and XFRM callbacks, and set the feature bits.
The XFRM code's listener will finish the setup on NETDEV_REGISTER.
(在探测时、调用 register_netdev() 之前，驱动应设置好本地数据结构和 XFRM 回调，并设置特性位。XFRM 代码的侦
听器会在 NETDEV_REGISTER 时完成剩余设置。)

::

		adapter->netdev->xfrmdev_ops = &ixgbe_xfrmdev_ops;
		adapter->netdev->features |= NETIF_F_HW_ESP;
		adapter->netdev->hw_enc_features |= NETIF_F_HW_ESP;

When new SAs are set up with a request for "offload" feature, the
driver's xdo_dev_state_add() will be given the new SA to be offloaded
and an indication of whether it is for Rx or Tx.  The driver should
(当新建 SA 并请求"offload"特性时，驱动的 xdo_dev_state_add() 会收到要卸载的新 SA，以及它是用于 Rx 还是
Tx 的指示。驱动应当：)

	- verify the algorithm is supported for offloads
	  (确认该算法支持卸载)
	- store the SA information (key, salt, target-ip, protocol, etc)
	  (保存 SA 信息（密钥、盐值、目标 IP、协议等）)
	- enable the HW offload of the SA
	  (启用该 SA 的硬件卸载)
	- return status value:
	  (返回状态值：)

		===========   ===================================
		0             success
		              (成功)
		-EOPNETSUPP   offload not supported, try SW IPsec,
                              not applicable for packet offload mode
		              (不支持卸载，请尝试软件 IPsec，不适用于数据包卸载模式)
		other         fail the request
		              (使该请求失败)
		===========   ===================================

The driver can also set an offload_handle in the SA, an opaque void pointer
that can be used to convey context into the fast-path offload requests::
(驱动还可以在 SA 中设置一个 offload_handle，这是一个不透明的 void 指针，可用于把上下文信息传递给快速路径的
卸载请求：)

		xs->xso.offload_handle = context;


When the network stack is preparing an IPsec packet for an SA that has
been setup for offload, it first calls into xdo_dev_offload_ok() with
the skb and the intended offload state to ask the driver if the offload
will serviceable.  This can check the packet information to be sure the
offload can be supported (e.g. IPv4 or IPv6, no IPv4 options, etc) and
return true or false to signify its support. In case driver doesn't implement
this callback, the stack provides reasonable defaults.
(当网络协议栈为一个已设置卸载的 SA 准备 IPsec 数据包时，它首先携带 skb 和预期的卸载状态调用
xdo_dev_offload_ok()，询问驱动该卸载是否可行。驱动可以检查数据包信息，确认卸载能否被支持（例如 IPv4 或
IPv6、无 IPv4 选项等），并返回 true 或 false 表示是否支持。如果驱动未实现此回调，协议栈会提供合理的默认行
为。)

Crypto offload mode:
When ready to send, the driver needs to inspect the Tx packet for the
offload information, including the opaque context, and set up the packet
send accordingly::
(加密卸载模式：
准备好发送时，驱动需要检查 Tx 数据包中的卸载信息（包括不透明上下文），并据此设置数据包的发送：)

		xs = xfrm_input_state(skb);
		context = xs->xso.offload_handle;
		set up HW for send

The stack has already inserted the appropriate IPsec headers in the
packet data, the offload just needs to do the encryption and fix up the
header values.
(协议栈已经把相应的 IPsec 头插入到数据包数据中，卸载只需完成加密并修正头的值即可。)


When a packet is received and the HW has indicated that it offloaded a
decryption, the driver needs to add a reference to the decoded SA into
the packet's skb.  At this point the data should be decrypted but the
IPsec headers are still in the packet data; they are removed later up
the stack in xfrm_input().
(当一个数据包被接收且硬件已表明其卸载了一次解密时，驱动需要把对已解码 SA 的引用添加到该数据包的 skb 中。此时
数据应已解密，但 IPsec 头仍在数据包数据中；它们稍后在协议栈上层由 xfrm_input() 移除。)

1. Find and hold the SA that was used to the Rx skb::
   (1. 找到用于该 Rx skb 的 SA 并持有其引用：)

		/* get spi, protocol, and destination IP from packet headers */
		xs = find xs from (spi, protocol, dest_IP)
		xfrm_state_hold(xs);

2. Store the state information into the skb::
   (2. 把状态信息存入 skb：)

		sp = secpath_set(skb);
		if (!sp) return;
		sp->xvec[sp->len++] = xs;
		sp->olen++;

3. Indicate the success and/or error status of the offload::
   (3. 指示卸载的成功和/或错误状态：)

		xo = xfrm_offload(skb);
		xo->flags = CRYPTO_DONE;
		xo->status = crypto_status;

4. Hand the packet to napi_gro_receive() as usual.
   (4. 像往常一样把数据包交给 napi_gro_receive()。)

In ESN mode, xdo_dev_state_advance_esn() is called from
xfrm_replay_advance_esn() for RX, and xfrm_replay_overflow_offload_esn for TX.
Driver will check packet seq number and update HW ESN state machine if needed.
(在 ESN 模式下，xdo_dev_state_advance_esn() 在 RX 方向由 xfrm_replay_advance_esn() 调用，在 TX 方向由
xfrm_replay_overflow_offload_esn 调用。驱动将检查数据包的序列号，并在需要时更新硬件 ESN 状态机。)

Packet offload mode:
HW adds and deletes XFRM headers. So in RX path, XFRM stack is bypassed if HW
reported success. In TX path, the packet lefts kernel without extra header
and not encrypted, the HW is responsible to perform it.
(数据包卸载模式：
由硬件添加和删除 XFRM 头。因此在 RX 路径中，如果硬件报告成功，则绕过 XFRM 协议栈。在 TX 路径中，数据包离开
内核时不带额外头且未加密，由硬件负责执行这些操作。)

When the SA is removed by the user, the driver's xdo_dev_state_delete()
and xdo_dev_policy_delete() are asked to disable the offload.  Later,
xdo_dev_state_free() and xdo_dev_policy_free() are called from a garbage
collection routine after all reference counts to the state and policy
have been removed and any remaining resources can be cleared for the
offload state.  How these are used by the driver will depend on specific
hardware needs.
(当用户移除 SA 时，会请求驱动的 xdo_dev_state_delete() 和 xdo_dev_policy_delete() 禁用卸载。之后，当指向
该状态和策略的所有引用计数都被移除后，垃圾回收例程会调用 xdo_dev_state_free() 和 xdo_dev_policy_free()，
此时便可以清理卸载状态的剩余资源。驱动如何使用这些回调取决于具体的硬件需求。)

As a netdev is set to DOWN the XFRM stack's netdev listener will call
xdo_dev_state_delete(), xdo_dev_policy_delete(), xdo_dev_state_free() and
xdo_dev_policy_free() on any remaining offloaded states.
(当网络设备被置为 DOWN 时，XFRM 协议栈的 netdev 侦听器会对所有剩余的已卸载状态调用
xdo_dev_state_delete()、xdo_dev_policy_delete()、xdo_dev_state_free() 和 xdo_dev_policy_free()。)

Outcome of HW handling packets, the XFRM core can't count hard, soft limits.
The HW/driver are responsible to perform it and provide accurate data when
xdo_dev_state_update_stats() is called. In case of one of these limits
occuried, the driver needs to call to xfrm_state_check_expire() to make sure
that XFRM performs rekeying sequence.
(由于数据包由硬件处理，XFRM 核心无法统计硬限制和软限制。硬件/驱动负责执行统计，并在 xdo_dev_state_update_stats()
被调用时提供准确数据。当触达其中某个限制时，驱动需要调用 xfrm_state_check_expire()，以确保 XFRM 执行重新密钥
协商 (rekeying) 流程。)
