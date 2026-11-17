.. SPDX-License-Identifier: GPL-2.0

=========
XFRM sync
=========

(XFRM 同步)

The sync patches work is based on initial patches from
Krisztian <hidden@balabit.hu> and others and additional patches
from Jamal <hadi@cyberus.ca>.
(同步补丁的工作基于 Krisztian <hidden@balabit.hu> 及其他人最初的补丁，以及 Jamal <hadi@cyberus.ca> 的额外
补丁。)

The end goal for syncing is to be able to insert attributes + generate
events so that the SA can be safely moved from one machine to another
for HA purposes.
The idea is to synchronize the SA so that the takeover machine can do
the processing of the SA as accurate as possible if it has access to it.
(同步的最终目标是能够插入属性并生成事件，使 SA 能够出于高可用 (HA) 的目的安全地从一台机器迁移到另一台机器。
其思想是同步 SA，使得接管机器在拥有该 SA 时，能够尽可能精确地对其进行处理。)

We already have the ability to generate SA add/del/upd events.
These patches add ability to sync and have accurate lifetime byte (to
ensure proper decay of SAs) and replay counters to avoid replay attacks
with as minimal loss at failover time.
This way a backup stays as closely up-to-date as an active member.
(我们已经能够生成 SA 添加/删除/更新事件。这些补丁增加了同步能力，并引入精确的生命期字节数（以保证 SA 的正常衰
减）和重放计数器，以防范重放攻击，同时把故障切换时的损失降到最小。这样，备份成员就能与活动成员保持近乎同等的
最新状态。)

Because the above items change for every packet the SA receives,
it is possible for a lot of the events to be generated.
For this reason, we also add a nagle-like algorithm to restrict
the events. i.e we are going to set thresholds to say "let me
know if the replay sequence threshold is reached or 10 secs have passed"
These thresholds are set system-wide via sysctls or can be updated
per SA.
(由于上述条目会随 SA 收到的每个数据包而变化，因此可能生成大量事件。为此，我们还增加了一个类 nagle 算法来限制
事件的产生。也就是说，我们会设置阈值，例如"当重放序列达到阈值或已过 10 秒时通知我"。这些阈值通过 sysctl 进
行系统级设置，也可以按 SA 单独更新。)

The identified items that need to be synchronized are:
(已确定的需要同步的条目有：)

- the lifetime byte counter
  (生命期字节计数器)

note that: lifetime time limit is not important if you assume the failover
machine is known ahead of time since the decay of the time countdown
is not driven by packet arrival.
(注意：如果假定故障切换机器是事先已知的，那么生命期时间限制并不重要，因为时间倒计时的衰减不是由数据包到达驱动
的。)

- the replay sequence for both inbound and outbound
  (入方向和出方向的重放序列)

1) Message Structure
(1) 消息结构)
--------------------

nlmsghdr:aevent_id:optional-TLVs.
(消息结构为 nlmsghdr:aevent_id:可选 TLV。)

The netlink message types are:
(netlink 消息类型为：)

XFRM_MSG_NEWAE and XFRM_MSG_GETAE.
(XFRM_MSG_NEWAE 和 XFRM_MSG_GETAE。)

A XFRM_MSG_GETAE does not have TLVs.
(XFRM_MSG_GETAE 不携带 TLV。)

A XFRM_MSG_NEWAE will have at least two TLVs (as is
discussed further below).
(XFRM_MSG_NEWAE 至少带有两个 TLV（下文将进一步讨论）。)

aevent_id structure looks like::
(aevent_id 结构如下：)

   struct xfrm_aevent_id {
	     struct xfrm_usersa_id           sa_id;
	     xfrm_address_t                  saddr;
	     __u32                           flags;
	     __u32                           reqid;
   };

The unique SA is identified by the combination of xfrm_usersa_id,
reqid and saddr.
(唯一的 SA 由 xfrm_usersa_id、reqid 和 saddr 的组合来标识。)

flags are used to indicate different things. The possible
flags are::
(flags 用于指示不同的内容。可能的标志有：)

	XFRM_AE_RTHR=1, /* replay threshold*/
	XFRM_AE_RVAL=2, /* replay value */
	XFRM_AE_LVAL=4, /* lifetime value */
	XFRM_AE_ETHR=8, /* expiry timer threshold */
	XFRM_AE_CR=16, /* Event cause is replay update */
	XFRM_AE_CE=32, /* Event cause is timer expiry */
	XFRM_AE_CU=64, /* Event cause is policy update */

How these flags are used is dependent on the direction of the
message (kernel<->user) as well the cause (config, query or event).
This is described below in the different messages.
(这些标志的使用方式取决于消息的方向（kernel<->user）以及起因（配置、查询或事件）。这将在下文的不同消息中分
别描述。)

The pid will be set appropriately in netlink to recognize direction
(0 to the kernel and pid = processid that created the event
when going from kernel to user space)
(netlink 中的 pid 会被适当地设置以识别方向（发往内核时为 0；从内核发往用户空间时，pid 为创建该事件的进程
ID）)

A program needs to subscribe to multicast group XFRMNLGRP_AEVENTS
to get notified of these events.
(程序需要订阅多播组 XFRMNLGRP_AEVENTS 才能获得这些事件的通知。)

2) TLVS reflect the different parameters
(2) TLV 反映不同的参数)
----------------------------------------

a) byte value (XFRMA_LTIME_VAL)
   (a) 字节值 (XFRMA_LTIME_VAL))

   This TLV carries the running/current counter for byte lifetime since
   last event.
   (该 TLV 携带自上一个事件以来的字节数生命期的运行中/当前计数。)

b) replay value (XFRMA_REPLAY_VAL)
   (b) 重放值 (XFRMA_REPLAY_VAL))

   This TLV carries the running/current counter for replay sequence since
   last event.
   (该 TLV 携带自上一个事件以来的重放序列的运行中/当前计数。)

c) replay threshold (XFRMA_REPLAY_THRESH)
   (c) 重放阈值 (XFRMA_REPLAY_THRESH))

   This TLV carries the threshold being used by the kernel to trigger events
   when the replay sequence is exceeded.
   (该 TLV 携带内核所使用的阈值，当超过重放序列时用于触发事件。)

d) expiry timer (XFRMA_ETIMER_THRESH)
   (d) 过期定时器 (XFRMA_ETIMER_THRESH))

   This is a timer value in milliseconds which is used as the nagle
   value to rate limit the events.
   (这是一个以毫秒为单位的定时器值，用作类 nagle 值来对事件进行速率限制。)

3) Default configurations for the parameters
(3) 参数的默认配置)
--------------------------------------------

By default these events should be turned off unless there is
at least one listener registered to listen to the multicast
group XFRMNLGRP_AEVENTS.
(默认情况下，这些事件应当关闭，除非至少有一个监听者已注册监听多播组 XFRMNLGRP_AEVENTS。)

Programs installing SAs will need to specify the two thresholds, however,
in order to not change existing applications such as racoon
we also provide default threshold values for these different parameters
in case they are not specified.
(安装 SA 的程序需要指定这两个阈值；不过，为了不打乱 racoon 等现有应用，我们也为这些不同参数提供了默认阈值，
以备未指定时使用。)

the two sysctls/proc entries are:
(两个 sysctl/proc 条目为：)

a) /proc/sys/net/core/sysctl_xfrm_aevent_etime
   (a) /proc/sys/net/core/sysctl_xfrm_aevent_etime))

   Used to provide default values for the XFRMA_ETIMER_THRESH in incremental
   units of time of 100ms. The default is 10 (1 second)
   (用于以 100ms 为增量时间单位为 XFRMA_ETIMER_THRESH 提供默认值。默认值为 10（即 1 秒）)

b) /proc/sys/net/core/sysctl_xfrm_aevent_rseqth
   (b) /proc/sys/net/core/sysctl_xfrm_aevent_rseqth))

   Used to provide default values for XFRMA_REPLAY_THRESH parameter
   in incremental packet count. The default is two packets.
   (用于以增量数据包计数为 XFRMA_REPLAY_THRESH 参数提供默认值。默认值为两个数据包。)

4) Message types
(4) 消息类型)
----------------

a) XFRM_MSG_GETAE issued by user-->kernel.
   XFRM_MSG_GETAE does not carry any TLVs.
   (a) XFRM_MSG_GETAE 由用户发给内核。
   XFRM_MSG_GETAE 不携带任何 TLV。

   The response is a XFRM_MSG_NEWAE which is formatted based on what
   XFRM_MSG_GETAE queried for.
   (响应是一条 XFRM_MSG_NEWAE，其格式取决于 XFRM_MSG_GETAE 所查询的内容。)

   The response will always have XFRMA_LTIME_VAL and XFRMA_REPLAY_VAL TLVs.
   (响应总是带有 XFRMA_LTIME_VAL 和 XFRMA_REPLAY_VAL 这两个 TLV。)

     * if XFRM_AE_RTHR flag is set, then XFRMA_REPLAY_THRESH is also retrieved
       (如果设置了 XFRM_AE_RTHR 标志，则还会检索 XFRMA_REPLAY_THRESH)
     * if XFRM_AE_ETHR flag is set, then XFRMA_ETIMER_THRESH is also retrieved
       (如果设置了 XFRM_AE_ETHR 标志，则还会检索 XFRMA_ETIMER_THRESH)

b) XFRM_MSG_NEWAE is issued by either user space to configure
   or kernel to announce events or respond to a XFRM_MSG_GETAE.
   (b) XFRM_MSG_NEWAE 可由用户空间发出用于配置，也可由内核发出用于通告事件或响应 XFRM_MSG_GETAE。

   i) user --> kernel to configure a specific SA.
      (i) 用户 --> 内核，用于配置特定的 SA。

      any of the values or threshold parameters can be updated by passing the
      appropriate TLV.
      (通过传递相应的 TLV，可以更新任何值或阈值参数。)

      A response is issued back to the sender in user space to indicate success
      or failure.
      (会向用户空间中的发送者回发一个响应，以指示成功或失败。)

      In the case of success, additionally an event with
      XFRM_MSG_NEWAE is also issued to any listeners as described in iii).
      (在成功的情况下，还会像 iii) 所描述的那样，向所有监听者额外发出一条带有
      XFRM_MSG_NEWAE 的事件。)

   ii) kernel->user direction as a response to XFRM_MSG_GETAE
       (ii) 内核 -> 用户方向，作为对 XFRM_MSG_GETAE 的响应

       The response will always have XFRMA_LTIME_VAL and XFRMA_REPLAY_VAL TLVs.
       (响应总是带有 XFRMA_LTIME_VAL 和 XFRMA_REPLAY_VAL 这两个 TLV。)

       The threshold TLVs will be included if explicitly requested in
       the XFRM_MSG_GETAE message.
       (如果在 XFRM_MSG_GETAE 消息中显式请求，则会包含阈值 TLV。)

   iii) kernel->user to report as event if someone sets any values or
        thresholds for an SA using XFRM_MSG_NEWAE (as described in #i above).
        In such a case XFRM_AE_CU flag is set to inform the user that
        the change happened as a result of an update.
        The message will always have XFRMA_LTIME_VAL and XFRMA_REPLAY_VAL TLVs.
       (iii) 内核 -> 用户：当有人使用 XFRM_MSG_NEWAE 为某个 SA 设置任何值或阈值时（如上文 i) 所述），以此
        作为事件进行报告。在这种情况下会设置 XFRM_AE_CU 标志，通知用户该变更是由更新引起的。
        该消息总是带有 XFRMA_LTIME_VAL 和 XFRMA_REPLAY_VAL 这两个 TLV。)

   iv) kernel->user to report event when replay threshold or a timeout
       is exceeded.
       (iv) 内核 -> 用户：当超过重放阈值或发生超时时报出事件。

In such a case either XFRM_AE_CR (replay exceeded) or XFRM_AE_CE (timeout
happened) is set to inform the user what happened.
Note the two flags are mutually exclusive.
The message will always have XFRMA_LTIME_VAL and XFRMA_REPLAY_VAL TLVs.
(在这种情况下，会设置 XFRM_AE_CR（超过重放）或 XFRM_AE_CE（发生超时）来通知用户发生了什么。
注意这两个标志是互斥的。
该消息总是带有 XFRMA_LTIME_VAL 和 XFRMA_REPLAY_VAL 这两个 TLV。)

5) Exceptions to threshold settings
(5) 阈值设置的例外情况)
-----------------------------------

If you have an SA that is getting hit by traffic in bursts such that
there is a period where the timer threshold expires with no packets
seen, then an odd behavior is seen as follows:
The first packet arrival after a timer expiry will trigger a timeout
event; i.e we don't wait for a timeout period or a packet threshold
to be reached. This is done for simplicity and efficiency reasons.
(如果某个 SA 受到突发流量的冲击，以至于在一段时间内定时器阈值到期时没有看到任何数据包，那么会出现如下的一种
奇怪行为：定时器到期后到达的第一个数据包将触发超时事件；也就是说，我们不会等待超时周期或数据包阈值达到。这是
出于简单性和效率的考虑。)

-JHS
