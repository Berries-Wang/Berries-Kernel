.. SPDX-License-Identifier: GPL-2.0

=====================
Softnet Driver Issues
=====================

(softnet 驱动相关问题)

Probing guidelines
(探测指南)
==================

Address validation
(地址校验)
------------------

Any hardware layer address you obtain for your device should
be verified.  For example, for ethernet check it with
linux/etherdevice.h:is_valid_ether_addr()
(你为设备获得的任何硬件层地址都应当经过校验。例如，对于以太网，使用
linux/etherdevice.h:is_valid_ether_addr() 来检查)

Close/stop guidelines
(关闭/停止指南)
=====================

Quiescence
(静止状态)
----------

After the ndo_stop routine has been called, the hardware must
not receive or transmit any data.  All in flight packets must
be aborted. If necessary, poll or wait for completion of
any reset commands.
(在 ndo_stop 例程被调用之后，硬件不得接收或发送任何数据。所有在途的
数据包都必须被中止。如有必要，轮询或等待任何复位命令完成。)

Auto-close
(自动关闭)
----------

The ndo_stop routine will be called by unregister_netdevice
if device is still UP.
(如果设备仍处于 UP 状态，unregister_netdevice 会调用 ndo_stop 例程。)

Transmit path guidelines
(发送路径指南)
========================

Stop queues in advance
(提前停止队列)
----------------------

The ndo_start_xmit method must not return NETDEV_TX_BUSY under
any normal circumstances.  It is considered a hard error unless
there is no way your device can tell ahead of time when its
transmit function will become busy.
(在任何正常情况下，ndo_start_xmit 方法都不得返回 NETDEV_TX_BUSY。这被
视为严重错误，除非你的设备完全没有办法提前告知其发送函数何时会变忙。)

Instead it must maintain the queue properly.  For example,
for a driver implementing scatter-gather this means:
(相反，它必须正确地维护队列。例如，对于实现了散射-聚集 (scatter-gather)
的驱动，这意味着：)

.. code-block:: c

	static u32 drv_tx_avail(struct drv_ring *dr)
	{
		u32 used = READ_ONCE(dr->prod) - READ_ONCE(dr->cons);

		return dr->tx_ring_size - (used & bp->tx_ring_mask);
	}

	static netdev_tx_t drv_hard_start_xmit(struct sk_buff *skb,
					       struct net_device *dev)
	{
		struct drv *dp = netdev_priv(dev);
		struct netdev_queue *txq;
		struct drv_ring *dr;
		int idx;

		idx = skb_get_queue_mapping(skb);
		dr = dp->tx_rings[idx];
		txq = netdev_get_tx_queue(dev, idx);

		//...
		/* This should be a very rare race - log it. */
		if (drv_tx_avail(dr) <= skb_shinfo(skb)->nr_frags + 1) {
			netif_stop_queue(dev);
			netdev_warn(dev, "Tx Ring full when queue awake!\n");
			return NETDEV_TX_BUSY;
		}

		//... queue packet to card ...

		netdev_tx_sent_queue(txq, skb->len);

		//... update tx producer index using WRITE_ONCE() ...

		if (!netif_txq_maybe_stop(txq, drv_tx_avail(dr),
					  MAX_SKB_FRAGS + 1, 2 * MAX_SKB_FRAGS))
			dr->stats.stopped++;

		//...
		return NETDEV_TX_OK;
	}

And then at the end of your TX reclamation event handling:
(然后在你的 TX 回收事件处理结束时：)

.. code-block:: c

	//... update tx consumer index using WRITE_ONCE() ...

	netif_txq_completed_wake(txq, cmpl_pkts, cmpl_bytes,
				 drv_tx_avail(dr), 2 * MAX_SKB_FRAGS);

Lockless queue stop / wake helper macros
(无锁队列停止/唤醒辅助宏)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/net/netdev_queues.h
   :doc: Lockless queue stopping / waking helpers.

No exclusive ownership
(无独占所有权)
----------------------

An ndo_start_xmit method must not modify the shared parts of a
cloned SKB.
(ndo_start_xmit 方法不得修改已克隆 SKB 的共享部分。)

Timely completions
(及时完成)
------------------

Do not forget that once you return NETDEV_TX_OK from your
ndo_start_xmit method, it is your driver's responsibility to free
up the SKB and in some finite amount of time.
(不要忘记，一旦你的 ndo_start_xmit 方法返回了 NETDEV_TX_OK，释放 SKB
并且在某个有限的时间内完成，就是你驱动的责任。)

For example, this means that it is not allowed for your TX
mitigation scheme to let TX packets "hang out" in the TX
ring unreclaimed forever if no new TX packets are sent.
This error can deadlock sockets waiting for send buffer room
to be freed up.
(例如，这意味着不允许你的 TX 缓解方案在没有任何新 TX 数据包发送的情况
下，让 TX 数据包永远 "滞留" 在 TX 环中不被回收。这种错误会使正在等待
发送缓冲空间被释放的套接字死锁。)

If you return NETDEV_TX_BUSY from the ndo_start_xmit method, you
must not keep any reference to that SKB and you must not attempt
to free it up.
(如果你从 ndo_start_xmit 方法返回 NETDEV_TX_BUSY，则不得保留对该 SKB
的任何引用，也不得尝试释放它。)
