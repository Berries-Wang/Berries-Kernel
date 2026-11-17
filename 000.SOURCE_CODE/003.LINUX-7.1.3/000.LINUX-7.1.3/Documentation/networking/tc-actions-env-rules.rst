.. SPDX-License-Identifier: GPL-2.0

================================
TC Actions - Environmental Rules
================================

(TC Actions - 环境规则)


The "environmental" rules for authors of any new tc actions are:
(针对任何新 tc action 的作者，"环境"规则如下：)

1) If you stealeth or borroweth any packet thou shalt be branching
   from the righteous path and thou shalt cloneth.
   (1) 如果你窃取或借用了任何数据包，你就偏离了正道，应当克隆它。)

   For example if your action queues a packet to be processed later,
   or intentionally branches by redirecting a packet, then you need to
   clone the packet.
   (例如，如果你的 action 将数据包排队等待稍后处理，或者通过重定向数据包而有意产生分支，那么你需要克隆该数据包。)

2) If you munge any packet thou shalt call pskb_expand_head in the case
   someone else is referencing the skb. After that you "own" the skb.
   (2) 如果你修改任何数据包，在还有其他人引用该 skb 的情况下，你必须调用 pskb_expand_head。之后你就"拥有"了这个 skb。)

3) Dropping packets you don't own is a no-no. You simply return
   TC_ACT_SHOT to the caller and they will drop it.
   (3) 丢弃不属于你的数据包是大忌。你只需向调用者返回 TC_ACT_SHOT，由他们来丢弃它。)

The "environmental" rules for callers of actions (qdiscs etc) are:
(针对 action 的调用者(qdisc 等)，"环境"规则如下：)

#) Thou art responsible for freeing anything returned as being
   TC_ACT_SHOT/STOLEN/QUEUED. If none of TC_ACT_SHOT/STOLEN/QUEUED is
   returned, then all is great and you don't need to do anything.
   (你负责释放所有以 TC_ACT_SHOT/STOLEN/QUEUED 形式返回的内容。如果没有返回 TC_ACT_SHOT/STOLEN/QUEUED 中的任何一种，那么一切安好，你无需做任何事情。)

Post on netdev if something is unclear.
(如果有不清楚的地方，请在 netdev 邮件列表上发帖询问。)
