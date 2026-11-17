.. SPDX-License-Identifier: GPL-2.0

=====
IPsec
=====

(IPsec)


Here documents known IPsec corner cases which need to be keep in mind when
deploy various IPsec configuration in real world production environment.
(本文记录已知的 IPsec 边界情况 (corner cases)，在真实生产环境中部署各种 IPsec 配置时需要牢记在心。)

1. IPcomp:
   (IPcomp：)
	   Small IP packet won't get compressed at sender, and failed on
	   policy check on receiver.
	   (小的 IP 数据包在发送端不会被压缩，并在接收端的策略检查中失败。)

Quote from RFC3173::
(引用 RFC3173 如下：)

  2.2. Non-Expansion Policy

   If the total size of a compressed payload and the IPComp header, as
   defined in section 3, is not smaller than the size of the original
   payload, the IP datagram MUST be sent in the original non-compressed
   form.  To clarify: If an IP datagram is sent non-compressed, no

   IPComp header is added to the datagram.  This policy ensures saving
   the decompression processing cycles and avoiding incurring IP
   datagram fragmentation when the expanded datagram is larger than the
   MTU.

   Small IP datagrams are likely to expand as a result of compression.
   Therefore, a numeric threshold should be applied before compression,
   where IP datagrams of size smaller than the threshold are sent in the
   original form without attempting compression.  The numeric threshold
   is implementation dependent.

Current IPComp implementation is indeed by the book, while as in practice
when sending non-compressed packet to the peer (whether or not packet len
is smaller than the threshold or the compressed len is larger than original
packet len), the packet is dropped when checking the policy as this packet
matches the selector but not coming from any XFRM layer, i.e., with no
security path. Such naked packet will not eventually make it to upper layer.
The result is much more wired to the user when ping peer with different
payload length.
(当前的 IPComp 实现确实完全按规范行事。但在实际使用中，当向对端发送未压缩的数据包时(无论数据包长度是否小于阈值，还是压缩后长度大于原始数据包长度)，该数据包都会在策略检查时被丢弃：因为它匹配选择器 (selector)，但并非来自任何 XFRM 层，即没有安全路径 (security path)。这种"裸"数据包最终无法到达上层。结果是，当用不同负载长度对对端执行 ping 时，用户会看到更加怪异的行为。)

One workaround is try to set "level use" for each policy if user observed
above scenario. The consequence of doing so is small packet(uncompressed)
will skip policy checking on receiver side.
(一种解决方法是：如果用户观察到上述场景，可尝试为每条策略设置 "level use"。这样做的后果是，小数据包(未压缩的)将跳过接收端的策略检查。)
