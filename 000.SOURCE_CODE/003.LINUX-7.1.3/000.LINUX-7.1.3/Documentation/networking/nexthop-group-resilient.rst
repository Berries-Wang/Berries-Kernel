.. SPDX-License-Identifier: GPL-2.0

=========================
Resilient Next-hop Groups
=========================

(弹性下一跳组)

Resilient groups are a type of next-hop group that is aimed at minimizing
disruption in flow routing across changes to the group composition and
weights of constituent next hops.
(弹性组是一种下一跳组，其目标是在组的构成与各成员下一跳的权重发生变化时，尽量减少流量路由所受到的干扰。)

The idea behind resilient hashing groups is best explained in contrast to
the legacy multipath next-hop group, which uses the hash-threshold
algorithm, described in RFC 2992.
(弹性哈希组背后的思想，最好通过与旧式多路径下一跳组相对照来理解，后者使用 RFC 2992 中描述的哈希阈值 (hash-threshold) 算法。)

To select a next hop, hash-threshold algorithm first assigns a range of
hashes to each next hop in the group, and then selects the next hop by
comparing the SKB hash with the individual ranges. When a next hop is
removed from the group, the ranges are recomputed, which leads to
reassignment of parts of hash space from one next hop to another. RFC 2992
illustrates it thus::
(为了选择下一跳，哈希阈值算法首先为组内每个下一跳分配一段哈希值区间，然后通过将 SKB 哈希与各区间比较来选出下一跳。当某个下一跳被从组中移除时，这些区间会被重新计算，导致部分哈希空间从一个下一跳重新分配给另一个下一跳。RFC 2992 对此有如下图示：)

             +-------+-------+-------+-------+-------+
             |   1   |   2   |   3   |   4   |   5   |
             +-------+-+-----+---+---+-----+-+-------+
             |    1    |    2    |    4    |    5    |
             +---------+---------+---------+---------+

              Before and after deletion of next hop 3
	      under the hash-threshold algorithm.

Note how next hop 2 gave up part of the hash space in favor of next hop 1,
and 4 in favor of 5. While there will usually be some overlap between the
previous and the new distribution, some traffic flows change the next hop
that they resolve to.
(注意下一跳 2 是如何让出部分哈希空间给下一跳 1、4 让给 5 的。虽然新旧分布之间通常会有一定重叠，但某些流量流解析到的下一跳会发生改变。)

If a multipath group is used for load-balancing between multiple servers,
this hash space reassignment causes an issue that packets from a single
flow suddenly end up arriving at a server that does not expect them. This
can result in TCP connections being reset.
(如果多路径组被用于在多台服务器之间做负载均衡，这种哈希空间重新分配会带来一个问题：单个流的数据包会突然到达一台并未预期它们的服务器，进而可能导致 TCP 连接被重置。)

If a multipath group is used for load-balancing among available paths to
the same server, the issue is that different latencies and reordering along
the way causes the packets to arrive in the wrong order, resulting in
degraded application performance.
(如果多路径组被用于在同一服务器的多条可用路径之间做负载均衡，问题则在于沿途不同的时延和乱序会使数据包以错误的顺序到达，从而导致应用性能下降。)

To mitigate the above-mentioned flow redirection, resilient next-hop groups
insert another layer of indirection between the hash space and its
constituent next hops: a hash table. The selection algorithm uses SKB hash
to choose a hash table bucket, then reads the next hop that this bucket
contains, and forwards traffic there.
(为了缓解上述流量重定向问题，弹性下一跳组在哈希空间与其成员下一跳之间又引入了一层间接层：哈希表。选择算法使用 SKB 哈希选定一个哈希表桶，然后读取该桶所包含的下一跳，并把流量转发到那里。)

This indirection brings an important feature. In the hash-threshold
algorithm, the range of hashes associated with a next hop must be
continuous. With a hash table, mapping between the hash table buckets and
the individual next hops is arbitrary. Therefore when a next hop is deleted
the buckets that held it are simply reassigned to other next hops::
(这种间接机制带来一个重要特性。在哈希阈值算法中，与某个下一跳关联的哈希区间必须是连续的；而有了哈希表，哈希表桶与各个下一跳之间的映射可以是任意的。因此当某个下一跳被删除时，原先容纳它的桶只需被重新指派给其他下一跳：)

	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |1|1|1|1|2|2|2|2|3|3|3|3|4|4|4|4|5|5|5|5|
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	                     v v v v
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    |1|1|1|1|2|2|2|2|1|2|4|5|4|4|4|4|5|5|5|5|
	    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	    Before and after deletion of next hop 3
	    under the resilient hashing algorithm.

When weights of next hops in a group are altered, it may be possible to
choose a subset of buckets that are currently not used for forwarding
traffic, and use those to satisfy the new next-hop distribution demands,
keeping the "busy" buckets intact. This way, established flows are ideally
kept being forwarded to the same endpoints through the same paths as before
the next-hop group change.
(当组内下一跳的权重被修改时，有可能选取出一部分当前未用于转发流量的桶，用它们来满足新的下一跳分布需求，同时保持"忙碌"的桶原封不动。这样，理想情况下已建立的流仍会像下一跳组变更之前一样，经由相同的路径被转发到相同的端点。)

Algorithm
(算法)
---------

In a nutshell, the algorithm works as follows. Each next hop deserves a
certain number of buckets, according to its weight and the number of
buckets in the hash table. In accordance with the source code, we will call
this number a "wants count" of a next hop. In case of an event that might
cause bucket allocation change, the wants counts for individual next hops
are updated.
(简而言之，该算法的工作方式如下。根据每个下一跳的权重以及哈希表中桶的数量，每个下一跳应获得一定数量的桶。与源代码保持一致，我们把这个数量称为一个下一跳的"期望计数" (wants count)。当发生可能导致桶分配变化的事件时，各个下一跳的期望计数会被更新。)

Next hops that have fewer buckets than their wants count, are called
"underweight". Those that have more are "overweight". If there are no
overweight (and therefore no underweight) next hops in the group, it is
said to be "balanced".
(所拥有的桶数量少于其期望计数的下一跳被称为"欠配" (underweight)；多于期望计数的则被称为"超配" (overweight)。如果组中不存在超配（因此也不存在欠配）的下一跳，则称该组是"平衡的" (balanced)。)

Each bucket maintains a last-used timer. Every time a packet is forwarded
through a bucket, this timer is updated to current jiffies value. One
attribute of a resilient group is then the "idle timer", which is the
amount of time that a bucket must not be hit by traffic in order for it to
be considered "idle". Buckets that are not idle are busy.
(每个桶都维护一个"最近使用"计时器。每当有数据包通过某个桶转发时，该计时器就会被更新为当前的 jiffies 值。弹性组的一个属性是"空闲计时器" (idle timer)，即一个桶必须连续多长时间未被流量命中，才会被视为"空闲" (idle)。不空闲的桶就是忙碌的。)

After assigning wants counts to next hops, an "upkeep" algorithm runs. For
buckets:
(为各下一跳指派期望计数之后，一个"维护" (upkeep) 算法会运行。对于以下桶：)

1) that have no assigned next hop, or
   (尚未指派下一跳的桶，或)
2) whose next hop has been removed, or
   (其下一跳已被移除的桶，或)
3) that are idle and their next hop is overweight,
   (处于空闲状态且其下一跳超配的桶，)

upkeep changes the next hop that the bucket references to one of the
underweight next hops. If, after considering all buckets in this manner,
there are still underweight next hops, another upkeep run is scheduled to a
future time.
(upkeep 算法会将这些桶所引用的下一跳更改为某个欠配的下一跳。如果在按此方式考察了所有桶之后，仍存在欠配的下一跳，则会调度在将来某个时间再运行一次 upkeep。)

There may not be enough "idle" buckets to satisfy the updated wants counts
of all next hops. Another attribute of a resilient group is the "unbalanced
timer". This timer can be set to 0, in which case the table will stay out
of balance until idle buckets do appear, possibly never. If set to a
non-zero value, the value represents the period of time that the table is
permitted to stay out of balance.
(可能没有足够的"空闲"桶来满足所有下一跳更新后的期望计数。弹性组的另一个属性是"失衡计时器" (unbalanced timer)。该计时器可以设为 0，此时表将一直处于失衡状态，直到空闲桶出现为止——这可能永远不会发生。如果设为非零值，该值表示允许表保持失衡状态的时间长度。)

With this in mind, we update the above list of conditions with one more
item. Thus buckets:
(考虑到这一点，我们在上述条件列表中再增加一项。于是，以下桶：)

4) whose next hop is overweight, and the amount of time that the table has
   been out of balance exceeds the unbalanced timer, if that is non-zero,
   (其下一跳超配，且表处于失衡状态的时间已超过失衡计时器（若该计时器非零）的桶，)

\... are migrated as well.
(……也将被迁移。)

Offloading & Driver Feedback
(卸载与驱动反馈)
----------------------------

When offloading resilient groups, the algorithm that distributes buckets
among next hops is still the one in SW. Drivers are notified of updates to
next hop groups in the following three ways:
(在卸载弹性组时，在各下一跳之间分配桶的算法仍由软件（SW）侧负责。驱动会通过以下三种方式收到下一跳组更新的通知：)

- Full group notification with the type
  ``NH_NOTIFIER_INFO_TYPE_RES_TABLE``. This is used just after the group is
  created and buckets populated for the first time.
  (类型为 ``NH_NOTIFIER_INFO_TYPE_RES_TABLE`` 的整组通知。它用在组刚创建、桶首次被填充之后。)

- Single-bucket notifications of the type
  ``NH_NOTIFIER_INFO_TYPE_RES_BUCKET``, which is used for notifications of
  individual migrations within an already-established group.
  (类型为 ``NH_NOTIFIER_INFO_TYPE_RES_BUCKET`` 的单桶通知，用于通知一个已建立组内的个别迁移。)

- Pre-replace notification, ``NEXTHOP_EVENT_RES_TABLE_PRE_REPLACE``. This
  is sent before the group is replaced, and is a way for the driver to veto
  the group before committing anything to the HW.
  (预替换通知 ``NEXTHOP_EVENT_RES_TABLE_PRE_REPLACE``。它在组被替换之前发送，是驱动在向硬件提交任何内容之前否决该组的一种途径。)

Some single-bucket notifications are forced, as indicated by the "force"
flag in the notification. Those are used for the cases where e.g. the next
hop associated with the bucket was removed, and the bucket really must be
migrated.
(某些单桶通知是强制性的，由通知中的 "force" 标志指示。它们用于诸如桶所关联的下一跳已被移除、该桶确实必须被迁移之类的情况。)

Non-forced notifications can be overridden by the driver by returning an
error code. The use case for this is that the driver notifies the HW that a
bucket should be migrated, but the HW discovers that the bucket has in fact
been hit by traffic.
(非强制通知可以被驱动通过返回错误码来否决。其使用场景是：驱动通知硬件某个桶应当被迁移，但硬件发现该桶实际上仍被流量命中。)

A second way for the HW to report that a bucket is busy is through the
``nexthop_res_grp_activity_update()`` API. The buckets identified this way
as busy are treated as if traffic hit them.
(硬件报告某个桶忙碌的第二种方式是通过 ``nexthop_res_grp_activity_update()`` API。以此方式被标识为忙碌的桶将被视为有流量命中。)

Offloaded buckets should be flagged as either "offload" or "trap". This is
done through the ``nexthop_bucket_set_hw_flags()`` API.
(已卸载的桶应被标记为 "offload" 或 "trap"。这通过 ``nexthop_bucket_set_hw_flags()`` API 完成。)

Netlink UAPI
(Netlink UAPI)
------------

Resilient Group Replacement
(弹性组的替换)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Resilient groups are configured using the ``RTM_NEWNEXTHOP`` message in the
same manner as other multipath groups. The following changes apply to the
attributes passed in the netlink message:
(弹性组使用 ``RTM_NEWNEXTHOP`` 消息进行配置，方式与其他多路径组相同。netlink 消息中传递的属性有以下变化：)

  =================== =========================================================
  ``NHA_GROUP_TYPE``  Should be ``NEXTHOP_GRP_TYPE_RES`` for resilient group.
                      (对于弹性组，应将其设为 NEXTHOP_GRP_TYPE_RES。)
  ``NHA_RES_GROUP``   A nest that contains attributes specific to resilient
                      groups.
                      (一个嵌套属性，包含弹性组专有的属性。)
  =================== =========================================================

``NHA_RES_GROUP`` payload:
(``NHA_RES_GROUP`` 载荷：)

  =================================== =========================================
  ``NHA_RES_GROUP_BUCKETS``           Number of buckets in the hash table.
                                      (哈希表中的桶数量。)
  ``NHA_RES_GROUP_IDLE_TIMER``        Idle timer in units of clock_t.
                                      (空闲计时器，以 clock_t 为单位。)
  ``NHA_RES_GROUP_UNBALANCED_TIMER``  Unbalanced timer in units of clock_t.
                                      (失衡计时器，以 clock_t 为单位。)
  =================================== =========================================

Next Hop Get
(下一跳获取)
^^^^^^^^^^^^

Requests to get resilient next-hop groups use the ``RTM_GETNEXTHOP``
message in exactly the same way as other next hop get requests. The
response attributes match the replacement attributes cited above, except
``NHA_RES_GROUP`` payload will include the following attribute:
(获取弹性下一跳组的请求使用 ``RTM_GETNEXTHOP`` 消息，方式与其他下一跳获取请求完全相同。响应属性与上面引用的替换属性一致，只是 ``NHA_RES_GROUP`` 载荷会包含以下属性：)

  =================================== =========================================
  ``NHA_RES_GROUP_UNBALANCED_TIME``   How long has the resilient group been out
                                      of balance, in units of clock_t.
                                      (弹性组处于失衡状态已有多长时间，以 clock_t 为单位。)
  =================================== =========================================

Bucket Get
(桶获取)
^^^^^^^^^^

The message ``RTM_GETNEXTHOPBUCKET`` without the ``NLM_F_DUMP`` flag is
used to request a single bucket. The attributes recognized at get requests
are:
(不带 ``NLM_F_DUMP`` 标志的 ``RTM_GETNEXTHOPBUCKET`` 消息用于请求单个桶。获取请求可识别的属性有：)

  =================== =========================================================
  ``NHA_ID``          ID of the next-hop group that the bucket belongs to.
                      (该桶所属下一跳组的 ID。)
  ``NHA_RES_BUCKET``  A nest that contains attributes specific to bucket.
                      (一个嵌套属性，包含桶专有的属性。)
  =================== =========================================================

``NHA_RES_BUCKET`` payload:
(``NHA_RES_BUCKET`` 载荷：)

  ======================== ====================================================
  ``NHA_RES_BUCKET_INDEX`` Index of bucket in the resilient table.
                           (桶在弹性表中的索引。)
  ======================== ====================================================

Bucket Dumps
(桶转储)
^^^^^^^^^^^^

The message ``RTM_GETNEXTHOPBUCKET`` with the ``NLM_F_DUMP`` flag is used
to request a dump of matching buckets. The attributes recognized at dump
requests are:
(带 ``NLM_F_DUMP`` 标志的 ``RTM_GETNEXTHOPBUCKET`` 消息用于请求转储匹配的桶。转储请求可识别的属性有：)

  =================== =========================================================
  ``NHA_ID``          If specified, limits the dump to just the next-hop group
                      with this ID.
                      (如果指定，则将转储限制为仅针对具有此 ID 的下一跳组。)
  ``NHA_OIF``         If specified, limits the dump to buckets that contain
                      next hops that use the device with this ifindex.
                      (如果指定，则将转储限制为包含使用此 ifindex 设备的下一跳的桶。)
  ``NHA_MASTER``      If specified, limits the dump to buckets that contain
                      next hops that use a device in the VRF with this ifindex.
                      (如果指定，则将转储限制为包含使用此 ifindex 所在 VRF 中设备的下一跳的桶。)
  ``NHA_RES_BUCKET``  A nest that contains attributes specific to bucket.
                      (一个嵌套属性，包含桶专有的属性。)
  =================== =========================================================

``NHA_RES_BUCKET`` payload:
(``NHA_RES_BUCKET`` 载荷：)

  ======================== ====================================================
  ``NHA_RES_BUCKET_NH_ID`` If specified, limits the dump to just the buckets
                           that contain the next hop with this ID.
                           (如果指定，则将转储限制为仅针对包含具有此 ID 的下一跳的桶。)
  ======================== ====================================================

Usage
(用法)
-----

To illustrate the usage, consider the following commands::
(为了说明用法，请看以下命令：)

	# ip nexthop add id 1 via 192.0.2.2 dev eth0
	# ip nexthop add id 2 via 192.0.2.3 dev eth0
	# ip nexthop add id 10 group 1/2 type resilient \
		buckets 8 idle_timer 60 unbalanced_timer 300

The last command creates a resilient next-hop group. It will have 8 buckets
(which is unusually low number, and used here for demonstration purposes
only), each bucket will be considered idle when no traffic hits it for at
least 60 seconds, and if the table remains out of balance for 300 seconds,
it will be forcefully brought into balance.
(最后一条命令创建一个弹性下一跳组。它将拥有 8 个桶（该数值异常偏小，这里仅为演示之用），每个桶在至少 60 秒未被流量命中时将被视为空闲；如果表持续失衡达 300 秒，它将被强制恢复平衡。)

Changing next-hop weights leads to change in bucket allocation::
(更改下一跳的权重会导致桶分配发生变化：)

	# ip nexthop replace id 10 group 1,3/2 type resilient

This can be confirmed by looking at individual buckets::
(这可以通过查看各个桶来确认：)

	# ip nexthop bucket show id 10
	id 10 index 0 idle_time 5.59 nhid 1
	id 10 index 1 idle_time 5.59 nhid 1
	id 10 index 2 idle_time 8.74 nhid 2
	id 10 index 3 idle_time 8.74 nhid 2
	id 10 index 4 idle_time 8.74 nhid 1
	id 10 index 5 idle_time 8.74 nhid 1
	id 10 index 6 idle_time 8.74 nhid 1
	id 10 index 7 idle_time 8.74 nhid 1

Note the two buckets that have a shorter idle time. Those are the ones that
were migrated after the next-hop replace command to satisfy the new demand
that next hop 1 be given 6 buckets instead of 4.
(注意空闲时间较短的那两个桶。它们是在执行下一跳替换命令之后被迁移的，以满足下一跳 1 从 4 个桶改为获得 6 个桶的新需求。)

Netdevsim
(Netdevsim)
---------

The netdevsim driver implements a mock offload of resilient groups, and
exposes debugfs interface that allows marking individual buckets as busy.
For example, the following will mark bucket 23 in next-hop group 10 as
active::
(netdevsim 驱动实现了弹性组的一个模拟卸载，并暴露出 debugfs 接口，允许将个别桶标记为忙碌。例如，以下命令会把下一跳组 10 中的桶 23 标记为活跃：)

	# echo 10 23 > /sys/kernel/debug/netdevsim/netdevsim10/fib/nexthop_bucket_activity

In addition, another debugfs interface can be used to configure that the
next attempt to migrate a bucket should fail::
(此外，还可以使用另一个 debugfs 接口来配置下一次尝试迁移桶时应当失败：)

	# echo 1 > /sys/kernel/debug/netdevsim/netdevsim10/fib/fail_nexthop_bucket_replace

Besides serving as an example, the interfaces that netdevsim exposes are
useful in automated testing, and
``tools/testing/selftests/drivers/net/netdevsim/nexthop.sh`` makes use of
them to test the algorithm.
(除了作为示例之外，netdevsim 暴露的这些接口在自动化测试中也很有用，``tools/testing/selftests/drivers/net/netdevsim/nexthop.sh`` 就利用它们来测试该算法。)
