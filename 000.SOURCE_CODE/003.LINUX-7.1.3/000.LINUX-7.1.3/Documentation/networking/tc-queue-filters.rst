.. SPDX-License-Identifier: GPL-2.0

============================================
TC queue based filtering(基于 TC 队列的过滤)
============================================

TC can be used for directing traffic to either a set of queues or
to a single queue on both the transmit and receive side.
(TC 可用于在发送和接收两个方向上把流量引导到一组队列或单个队列。)

On the transmit side:
(在发送方向上：)

1) TC filter directing traffic to a set of queues is achieved
   using the action skbedit priority for Tx priority selection,
   the priority maps to a traffic class (set of queues) when
   the queue-sets are configured using mqprio.
   (1) 把流量引导到一组队列的 TC 过滤器，通过使用 action skbedit priority 来选择 Tx 优先级实现；当使用 mqprio 配置了队列组 (queue-sets) 时，该优先级会映射到一个流量类 (traffic class,即一组队列)。)

2) TC filter directs traffic to a transmit queue with the action
   skbedit queue_mapping $tx_qid. The action skbedit queue_mapping
   for transmit queue is executed in software only and cannot be
   offloaded.
   (2) 使用 action skbedit queue_mapping $tx_qid 的 TC 过滤器把流量引导到某个发送队列。用于发送队列的 action skbedit queue_mapping 仅在软件中执行，不能被卸载 (offload)。)

Likewise, on the receive side, the two filters for selecting set of
queues and/or a single queue are supported as below:
(同样，在接收方向上，支持如下两种过滤器来选择一组队列和/或单个队列：)

1) TC flower filter directs incoming traffic to a set of queues using
   the 'hw_tc' option.
   hw_tc $TCID - Specify a hardware traffic class to pass matching
   packets on to. TCID is in the range 0 through 15.
   (1) TC flower 过滤器使用 'hw_tc' 选项把传入流量引导到一组队列。
   hw_tc $TCID - 指定一个硬件流量类 (traffic class)，把匹配的数据包传递给它。TCID 的取值范围是 0 到 15。)

2) TC filter with action skbedit queue_mapping $rx_qid selects a
   receive queue. The action skbedit queue_mapping for receive queue
   is supported only in hardware. Multiple filters may compete in
   the hardware for queue selection. In such case, the hardware
   pipeline resolves conflicts based on priority. On Intel E810
   devices, TC filter directing traffic to a queue have higher
   priority over flow director filter assigning a queue. The hash
   filter has lowest priority.
   (2) 带 action skbedit queue_mapping $rx_qid 的 TC 过滤器选择一个接收队列。用于接收队列的 action skbedit queue_mapping 仅在硬件中支持。多个过滤器可能在硬件中竞争队列选择。在这种情况下，硬件流水线会根据优先级解决冲突。在 Intel E810 设备上，把流量引导到某个队列的 TC 过滤器比分配队列的 flow director 过滤器具有更高的优先级。哈希 (hash) 过滤器的优先级最低。)
