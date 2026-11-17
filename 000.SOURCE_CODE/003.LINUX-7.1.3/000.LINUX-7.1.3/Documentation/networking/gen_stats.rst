.. SPDX-License-Identifier: GPL-2.0

===============================================
Generic networking statistics for netlink users
===============================================

(面向 netlink 用户的通用网络统计)

Statistic counters are grouped into structs:
(统计计数器被组织成若干结构体：)

==================== ===================== =====================
Struct (结构体)      TLV type (TLV 类型)   Description (描述)
==================== ===================== =====================
gnet_stats_basic     TCA_STATS_BASIC       Basic statistics (基本统计)
gnet_stats_rate_est  TCA_STATS_RATE_EST    Rate estimator (速率估计器)
gnet_stats_queue     TCA_STATS_QUEUE       Queue statistics (队列统计)
none                 TCA_STATS_APP         Application specific (应用特定)
==================== ===================== =====================


Collecting:
(收集：)
-----------

Declare the statistic structs you need::
(声明你所需的统计结构体：)

	struct mystruct {
		struct gnet_stats_basic	bstats;
		struct gnet_stats_queue	qstats;
		...
	};

Update statistics, in dequeue() methods only, (while owning qdisc->running)::
(在 dequeue() 方法中更新统计信息 (仅在持有 qdisc->running 期间)：)

	mystruct->tstats.packet++;
	mystruct->qstats.backlog += skb->pkt_len;


Export to userspace (Dump):
(导出到用户空间 (Dump)：)
---------------------------

::

    my_dumping_routine(struct sk_buff *skb, ...)
    {
	    struct gnet_dump dump;

	    if (gnet_stats_start_copy(skb, TCA_STATS2, &mystruct->lock, &dump,
				    TCA_PAD) < 0)
		    goto rtattr_failure;

	    if (gnet_stats_copy_basic(&dump, &mystruct->bstats) < 0 ||
		gnet_stats_copy_queue(&dump, &mystruct->qstats) < 0 ||
		    gnet_stats_copy_app(&dump, &xstats, sizeof(xstats)) < 0)
		    goto rtattr_failure;

	    if (gnet_stats_finish_copy(&dump) < 0)
		    goto rtattr_failure;
	    ...
    }

TCA_STATS/TCA_XSTATS backward compatibility:
(TCA_STATS/TCA_XSTATS 向后兼容：)
--------------------------------------------

Prior users of struct tc_stats and xstats can maintain backward
compatibility by calling the compat wrappers to keep providing the
existing TLV types::
(struct tc_stats 和 xstats 的既有使用者可以通过调用兼容性包装器来保持
向后兼容，继续提供现有的 TLV 类型：)

    my_dumping_routine(struct sk_buff *skb, ...)
    {
	if (gnet_stats_start_copy_compat(skb, TCA_STATS2, TCA_STATS,
					TCA_XSTATS, &mystruct->lock, &dump,
					TCA_PAD) < 0)
		    goto rtattr_failure;
	    ...
    }

A struct tc_stats will be filled out during gnet_stats_copy_* calls
and appended to the skb. TCA_XSTATS is provided if gnet_stats_copy_app
was called.
(在调用 gnet_stats_copy_* 期间会填充一个 struct tc_stats 并把它附加到
skb 上。如果调用了 gnet_stats_copy_app，则会提供 TCA_XSTATS。)


Locking:
(加锁：)
--------

Locks are taken before writing and released once all statistics have
been written. Locks are always released in case of an error. You
are responsible for making sure that the lock is initialized.
(在写入之前获取锁，并在所有统计信息写完之后释放。发生错误时总是释放锁。
你需要负责确保该锁已被初始化。)


Rate Estimator:
(速率估计器：)
---------------

0) Prepare an estimator attribute. Most likely this would be in user
   space. The value of this TLV should contain a tc_estimator structure.
   As usual, such a TLV needs to be 32 bit aligned and therefore the
   length needs to be appropriately set, etc. The estimator interval
   and ewma log need to be converted to the appropriate values.
   tc_estimator.c::tc_setup_estimator() is advisable to be used as the
   conversion routine. It does a few clever things. It takes a time
   interval in microsecs, a time constant also in microsecs and a struct
   tc_estimator to  be populated. The returned tc_estimator can be
   transported to the kernel.  Transfer such a structure in a TLV of type
   TCA_RATE to your code in the kernel.
   (0) 准备一个估计器属性。这很可能是在用户空间完成的。该 TLV 的值应当
   包含一个 tc_estimator 结构。和往常一样，这样的 TLV 需要按 32 位对齐，
   因此长度等需要恰当地设置。估计器间隔和 ewma 对数需要转换为合适的值。
   建议使用 tc_estimator.c::tc_setup_estimator() 作为转换例程。它做了
   一些巧妙的事情：它接受以微秒为单位的时间间隔、同样以微秒为单位的时间
   常数以及一个待填充的 struct tc_estimator。返回的 tc_estimator 可以
   被传送到内核。把这样的结构放在类型为 TCA_RATE 的 TLV 中传输给你内核
   中的代码。

In the kernel when setting up:
(在内核中设置时：)

1) make sure you have basic stats and rate stats setup first.
   (1) 确保先设置好基本统计和速率统计。
2) make sure you have initialized stats lock that is used to setup such
   stats.
   (2) 确保已初始化用于设置这些统计信息的统计锁。
3) Now initialize a new estimator::
   (3) 现在初始化一个新的估计器：

    int ret = gen_new_estimator(my_basicstats,my_rate_est_stats,
	mystats_lock, attr_with_tcestimator_struct);

    if ret == 0
	success
    else
	failed

From now on, every time you dump my_rate_est_stats it will contain
up-to-date info.
(从现在起，每次转储 my_rate_est_stats 时，它都会包含最新的信息。)

Once you are done, call gen_kill_estimator(my_basicstats,
my_rate_est_stats) Make sure that my_basicstats and my_rate_est_stats
are still valid (i.e still exist) at the time of making this call.
(完成后，调用 gen_kill_estimator(my_basicstats, my_rate_est_stats)。
请确保在发起该调用时 my_basicstats 和 my_rate_est_stats 仍然有效
(即仍然存在)。)


Authors:
(作者：)
--------
- Thomas Graf <tgraf@suug.ch>
- Jamal Hadi Salim <hadi@cyberus.ca>
