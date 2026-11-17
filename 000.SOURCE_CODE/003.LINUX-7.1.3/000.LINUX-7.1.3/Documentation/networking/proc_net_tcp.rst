.. SPDX-License-Identifier: GPL-2.0

==================================================================================
The proc/net/tcp and proc/net/tcp6 variables(/proc/net/tcp 和 /proc/net/tcp6 变量)
==================================================================================

This document describes the interfaces /proc/net/tcp and /proc/net/tcp6.
Note that these interfaces are deprecated in favor of tcp_diag.
(本文档描述 /proc/net/tcp 和 /proc/net/tcp6 接口。注意，这两个接口已弃用，建议改用 tcp_diag。)

These /proc interfaces provide information about currently active TCP
connections, and are implemented by tcp4_seq_show() in net/ipv4/tcp_ipv4.c
and tcp6_seq_show() in net/ipv6/tcp_ipv6.c, respectively.
(这些 /proc 接口提供当前处于活动状态的 TCP 连接的信息，分别由 net/ipv4/tcp_ipv4.c 中的 tcp4_seq_show() 和 net/ipv6/tcp_ipv6.c 中的 tcp6_seq_show() 实现。)

It will first list all listening TCP sockets, and next list all established
TCP connections. A typical entry of /proc/net/tcp would look like this (split
up into 3 parts because of the length of the line)::
(它首先列出所有处于监听状态的 TCP 套接字，然后列出所有已建立的 TCP 连接。/proc/net/tcp 的一条典型条目如下所示(由于行太长而拆分成 3 部分)：)

   46: 010310AC:9C4C 030310AC:1770 01
   |      |      |      |      |   |--> connection state
   |      |      |      |      |------> remote TCP port number
   |      |      |      |-------------> remote IPv4 address
   |      |      |--------------------> local TCP port number
   |      |---------------------------> local IPv4 address
   |----------------------------------> number of entry

   00000150:00000000 01:00000019 00000000
      |        |     |     |       |--> number of unrecovered RTO timeouts
      |        |     |     |----------> number of jiffies until timer expires
      |        |     |----------------> timer_active (see below)
      |        |----------------------> receive-queue
      |-------------------------------> transmit-queue

   1000        0 54165785 4 cd1e6040 25 4 27 3 -1
    |          |    |     |    |     |  | |  | |--> slow start size threshold,
    |          |    |     |    |     |  | |  |      or -1 if the threshold
    |          |    |     |    |     |  | |  |      is >= 0xFFFF
    |          |    |     |    |     |  | |  |----> sending congestion window
    |          |    |     |    |     |  | |-------> (ack.quick<<1)|ack.pingpong
    |          |    |     |    |     |  |---------> Predicted tick of soft clock
    |          |    |     |    |     |              (delayed ACK control data)
    |          |    |     |    |     |------------> retransmit timeout
    |          |    |     |    |------------------> location of socket in memory
    |          |    |     |-----------------------> socket reference count
    |          |    |-----------------------------> inode
    |          |----------------------------------> unanswered 0-window probes
    |---------------------------------------------> uid

timer_active:
(timer_active 的取值如下：)

 ==  ================================================================
  0  no timer is pending
     (没有计时器处于待处理状态)
  1  retransmit-timer is pending
     (重传计时器处于待处理状态)
  2  another timer (e.g. delayed ack or keepalive) is pending
     (另一个计时器(例如延迟 ACK 或保活)处于待处理状态)
  3  this is a socket in TIME_WAIT state. Not all fields will contain
     data (or even exist)
     (这是一个处于 TIME_WAIT 状态的套接字。并非所有字段都会包含数据(甚至可能不存在))
  4  zero window probe timer is pending
     (零窗口探测计时器处于待处理状态)
 ==  ================================================================
