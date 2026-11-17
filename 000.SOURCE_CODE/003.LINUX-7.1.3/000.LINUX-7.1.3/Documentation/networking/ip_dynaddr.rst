.. SPDX-License-Identifier: GPL-2.0

===============================================================
IP dynamic address hack-port v0.03(IP 动态地址 hack-port v0.03)
===============================================================

This stuff allows diald ONESHOT connections to get established by
dynamically changing packet source address (and socket's if local procs).
It is implemented for TCP diald-box connections(1) and IP_MASQuerading(2).
(这套机制允许 diald ONESHOT 连接通过动态修改数据包源地址(对本地进程还包括套接字地址)来建立。它是为 TCP diald-box 连接(1)和 IP_MASQuerading(2)而实现的。)

If enabled\ [#]_ and forwarding interface has changed:
(如果已启用该功能且转发接口发生了变化：)

  1)  Socket (and packet) source address is rewritten ON RETRANSMISSIONS
      while in SYN_SENT state (diald-box processes).
      (套接字(和数据包)的源地址会在处于 SYN_SENT 状态时的重传过程中被重写(diald-box 场景)。)
  2)  Out-bounded MASQueraded source address changes ON OUTPUT (when
      internal host does retransmission) until a packet from outside is
      received by the tunnel.
      (向外发出的伪装 (MASQUERADE) 源地址会在输出时被修改(当内部主机进行重传时)，直到隧道收到来自外部的一个数据包为止。)

This is specially helpful for auto dialup links (diald), where the
``actual`` outgoing address is unknown at the moment the link is
going up. So, the *same* (local AND masqueraded) connections requests that
bring the link up will be able to get established.
(这对自动拨号链路 (diald) 特别有用，因为在链路即将建立的那一刻，"实际"的出站地址是未知的。这样，引发链路建立的那条"相同"的连接请求(本地连接与伪装连接)就能够成功建立。)

.. [#] At boot, by default no address rewriting is attempted.
  (在系统启动时，默认不尝试任何地址重写。)

  To enable::
  (启用：)

     # echo 1 > /proc/sys/net/ipv4/ip_dynaddr

  To enable verbose mode::
  (启用详细模式：)

    # echo 2 > /proc/sys/net/ipv4/ip_dynaddr

  To disable (default)::
  (禁用(默认)：)

     # echo 0 > /proc/sys/net/ipv4/ip_dynaddr

Enjoy!
(祝你使用愉快！)

Juanjo  <jjciarla@raiz.uncu.edu.ar>
