======================================================================================
Net DIM - Generic Network Dynamic Interrupt Moderation(Net DIM - 通用网络动态中断调节)
======================================================================================

:Author: Tal Gilboa <talgi@mellanox.com>
(作者: Tal Gilboa <talgi@mellanox.com>)

.. contents:: :depth: 2

Assumptions(前提假设)
=====================

This document assumes the reader has basic knowledge in network drivers
and in general interrupt moderation.
(本文假定读者已具备网络驱动以及一般中断调节方面的基础知识。)


Introduction(简介)
==================

Dynamic Interrupt Moderation (DIM) (in networking) refers to changing the
interrupt moderation configuration of a channel in order to optimize packet
processing. The mechanism includes an algorithm which decides if and how to
change moderation parameters for a channel, usually by performing an analysis on
runtime data sampled from the system. Net DIM is such a mechanism. In each
iteration of the algorithm, it analyses a given sample of the data, compares it
to the previous sample and if required, it can decide to change some of the
interrupt moderation configuration fields. The data sample is composed of data
bandwidth, the number of packets and the number of events. The time between
samples is also measured. Net DIM compares the current and the previous data and
returns an adjusted interrupt moderation configuration object. In some cases,
the algorithm might decide not to change anything. The configuration fields are
the minimum duration (microseconds) allowed between events and the maximum
number of wanted packets per event. The Net DIM algorithm ascribes importance to
increase bandwidth over reducing interrupt rate.
(动态中断调节 (DIM)（在网络领域）是指为优化数据包处理而更改通道的中断调节配置。该机制包含一个算法，由它决定
是否以及如何更改通道的中断调节参数，通常做法是对从系统采样的运行时数据进行分析。Net DIM 就是这样一个机制。
在算法的每次迭代中，它分析给定的一份数据样本，将其与上一个样本进行比较，并在需要时决定更改部分中断调节配置
字段。数据样本由数据带宽、数据包数量和事件数量组成，同时也会测量两次采样之间的时间间隔。Net DIM 将当前数据
与之前的数据进行比较，返回一个经过调整的中断调节配置对象。某些情况下，算法可能决定不做任何更改。配置字段包
括事件之间允许的最短时间间隔（微秒）和每个事件期望的最大数据包数。Net DIM 算法将提升带宽的重要性置于降低中
断速率之上。)


Net DIM Algorithm(Net DIM 算法)
===============================

Each iteration of the Net DIM algorithm follows these steps:
(Net DIM 算法的每次迭代遵循以下步骤：)

#. Calculates new data sample.
   (计算新的数据样本。)
#. Compares it to previous sample.
   (将其与上一个样本进行比较。)
#. Makes a decision - suggests interrupt moderation configuration fields.
   (做出决策——给出建议的中断调节配置字段。)
#. Applies a schedule work function, which applies suggested configuration.
   (调度一个工作函数，由它应用建议的配置。)

The first two steps are straightforward, both the new and the previous data are
supplied by the driver registered to Net DIM. The previous data is the new data
supplied to the previous iteration. The comparison step checks the difference
between the new and previous data and decides on the result of the last step.
A step would result as "better" if bandwidth increases and as "worse" if
bandwidth reduces. If there is no change in bandwidth, the packet rate is
compared in a similar fashion - increase == "better" and decrease == "worse".
In case there is no change in the packet rate as well, the interrupt rate is
compared. Here the algorithm tries to optimize for lower interrupt rate so an
increase in the interrupt rate is considered "worse" and a decrease is
considered "better". Step #2 has an optimization for avoiding false results: it
only considers a difference between samples as valid if it is greater than a
certain percentage. Also, since Net DIM does not measure anything by itself, it
assumes the data provided by the driver is valid.
(前两个步骤很直接，新数据和旧数据都由注册到 Net DIM 的驱动提供。旧数据就是上一次迭代所提供的新数据。比较步
骤检查新数据与旧数据之间的差异，并据此得出最后一步的判定结果。如果带宽增加，则判定结果为"更好" (better)；如
果带宽降低，则为"更差" (worse)。如果带宽没有变化，则以类似方式比较数据包速率——增加即为"更好"，减少即为
"更差"。如果数据包速率也没有变化，则比较中断速率。这里算法以更低的中断速率为优化目标，因此中断速率上升被视
为"更差"，下降被视为"更好"。第 2 步有一项避免错误结果的优化：只有当样本之间的差异超过某个百分比时，才认定
该差异有效。此外，由于 Net DIM 自身不进行任何测量，它假定驱动提供的数据是有效的。)

Step #3 decides on the suggested configuration based on the result from step #2
and the internal state of the algorithm. The states reflect the "direction" of
the algorithm: is it going left (reducing moderation), right (increasing
moderation) or standing still. Another optimization is that if a decision
to stay still is made multiple times, the interval between iterations of the
algorithm would increase in order to reduce calculation overhead. Also, after
"parking" on one of the most left or most right decisions, the algorithm may
decide to verify this decision by taking a step in the other direction. This is
done in order to avoid getting stuck in a "deep sleep" scenario. Once a
decision is made, an interrupt moderation configuration is selected from
the predefined profiles.
(第 3 步基于第 2 步的结果和算法的内部状态来决定建议的配置。这些状态反映了算法的"方向"：是向左（减小调节强
度）、向右（增大调节强度）还是原地不动。另一项优化是：如果多次做出原地不动的决策，算法迭代之间的间隔就会增
大，以减少计算开销。此外，在"停驻" (parking) 到最左或最右的决策之后，算法可能会决定朝相反方向迈出一步来验
证该决策，这是为了避免陷入"深度睡眠"的场景。决策一旦做出，就从预定义的配置档 (profile) 中选中一个中断调节
配置。)

The last step is to notify the registered driver that it should apply the
suggested configuration. This is done by scheduling a work function, defined by
the Net DIM API and provided by the registered driver.
(最后一步是通知已注册的驱动，让它应用建议的配置。这通过调度一个工作函数来完成，该函数由 Net DIM API 定义、
由已注册的驱动提供。)

As you can see, Net DIM itself does not actively interact with the system. It
would have trouble making the correct decisions if the wrong data is supplied to
it and it would be useless if the work function would not apply the suggested
configuration. This does, however, allow the registered driver some room for
manoeuvre as it may provide partial data or ignore the algorithm suggestion
under some conditions.
(可以看到，Net DIM 本身并不主动与系统交互。如果提供给它的数据有误，它将难以做出正确的决策；如果工作函数不
应用建议的配置，它也就毫无用处。不过，这也给已注册的驱动留出了一定的操作空间，因为它可以在某些条件下只提供
部分数据，或者忽略算法的建议。)


Registering a Network Device to DIM(将网络设备注册到 DIM)
=========================================================

Net DIM API exposes the main function net_dim().
This function is the entry point to the Net
DIM algorithm and has to be called every time the driver would like to check if
it should change interrupt moderation parameters. The driver should provide two
data structures: :c:type:`struct dim <dim>` and
:c:type:`struct dim_sample <dim_sample>`. :c:type:`struct dim <dim>`
describes the state of DIM for a specific object (RX queue, TX queue,
other queues, etc.). This includes the current selected profile, previous data
samples, the callback function provided by the driver and more.
:c:type:`struct dim_sample <dim_sample>` describes a data sample,
which will be compared to the data sample stored in :c:type:`struct dim <dim>`
in order to decide on the algorithm's next
step. The sample should include bytes, packets and interrupts, measured by
the driver.
(Net DIM API 暴露了主函数 net_dim()。该函数是 Net DIM 算法的入口，每当驱动想检查是否应更改中断调节参数时，
都必须调用它。驱动需要提供两个数据结构：:c:type:`struct dim <dim>` 和
:c:type:`struct dim_sample <dim_sample>`。:c:type:`struct dim <dim>`
描述某个特定对象（RX 队列、TX 队列、其他队列等）的 DIM 状态，其中包括当前选定的配置档、之前的数据样本、驱
动提供的回调函数等。:c:type:`struct dim_sample <dim_sample>` 描述一份
数据样本，它将与存储在 :c:type:`struct dim <dim>` 中的数据样本进行比较，
以决定算法的下一步动作。样本应包含由驱动测量的字节数、数据包数和中断数。)

In order to use Net DIM from a networking driver, the driver needs to call the
main net_dim() function. The recommended method is to call net_dim() on each
interrupt. Since Net DIM has a built-in moderation and it might decide to skip
iterations under certain conditions, there is no need to moderate the net_dim()
calls as well. As mentioned above, the driver needs to provide an object of type
:c:type:`struct dim <dim>` to the net_dim() function call. It is advised for
each entity using Net DIM to hold a :c:type:`struct dim <dim>` as part of its
data structure and use it as the main Net DIM API object.
The :c:type:`struct dim_sample <dim_sample>` should hold the latest
bytes, packets and interrupts count. No need to perform any calculations, just
include the raw data.
(网络驱动要使用 Net DIM，就需要调用主函数 net_dim()。推荐的做法是在每次中断时调用 net_dim()。由于 Net DIM
内置了调节机制，并且在某些条件下可能决定跳过迭代，因此无需再对 net_dim() 的调用本身做节流。如上所述，驱动
需要在调用 net_dim() 时提供一个 :c:type:`struct dim <dim>` 类型的对象。建议每个使用 Net DIM 的实体都将一个
:c:type:`struct dim <dim>` 作为其数据结构的一部分，并将其作为主 Net DIM API 对象来使用。
:c:type:`struct dim_sample <dim_sample>` 应保存最新的字节数、数据包数和中断数。无需做任何计算，只需填入原
始数据即可。)

The net_dim() call itself does not return anything. Instead Net DIM relies on
the driver to provide a callback function, which is called when the algorithm
decides to make a change in the interrupt moderation parameters. This callback
will be scheduled and run in a separate thread in order not to add overhead to
the data flow. After the work is done, Net DIM algorithm needs to be set to
the proper state in order to move to the next iteration.
(net_dim() 调用本身不返回任何内容。相反，Net DIM 依靠驱动提供一个回调函数，当算法决定更改中断调节参数时会调
用它。该回调会被调度到单独的线程中运行，以免给数据流增加开销。工作完成后，需要将 Net DIM 算法设置为正确的
状态，以便进入下一次迭代。)


Example(示例)
=============

The following code demonstrates how to register a driver to Net DIM. The actual
usage is not complete but it should make the outline of the usage clear.
(下面的代码演示如何将驱动注册到 Net DIM。实际用法并不完整，但应当能清晰地展示用法概貌。)

.. code-block:: c

  #include <linux/dim.h>

  /* Callback for net DIM to schedule on a decision to change moderation */
  void my_driver_do_dim_work(struct work_struct *work)
  {
	/* Get struct dim from struct work_struct */
	struct dim *dim = container_of(work, struct dim,
				       work);
	/* Do interrupt moderation related stuff */
	...

	/* Signal net DIM work is done and it should move to next iteration */
	dim->state = DIM_START_MEASURE;
  }

  /* My driver's interrupt handler */
  int my_driver_handle_interrupt(struct my_driver_entity *my_entity, ...)
  {
	...
	/* A struct to hold current measured data */
	struct dim_sample dim_sample;
	...
	/* Initiate data sample struct with current data */
	dim_update_sample(my_entity->events,
		          my_entity->packets,
		          my_entity->bytes,
		          &dim_sample);
	/* Call net DIM */
	net_dim(&my_entity->dim, &dim_sample);
	...
  }

  /* My entity's initialization function (my_entity was already allocated) */
  int my_driver_init_my_entity(struct my_driver_entity *my_entity, ...)
  {
	...
	/* Initiate struct work_struct with my driver's callback function */
	INIT_WORK(&my_entity->dim.work, my_driver_do_dim_work);
	...
  }


Tuning DIM(调优 DIM)
====================

Net DIM serves a range of network devices and delivers excellent acceleration
benefits. Yet, it has been observed that some preset configurations of DIM may
not align seamlessly with the varying specifications of network devices, and
this discrepancy has been identified as a factor to the suboptimal performance
outcomes of DIM-enabled network devices, related to a mismatch in profiles.
(Net DIM 服务于各种网络设备，并带来出色的加速收益。然而，人们观察到 DIM 的某些预设配置可能与网络设备各异的
规格无法完美契合，这种差异被认为是启用 DIM 的网络设备性能欠佳的一个因素，与配置档不匹配有关。)

To address this issue, Net DIM introduces a per-device control to modify and
access a device's ``rx-profile`` and ``tx-profile`` parameters:
Assume that the target network device is named ethx, and ethx only declares
support for RX profile setting and supports modification of ``usec`` field
and ``pkts`` field (See the data structure:
:c:type:`struct dim_cq_moder <dim_cq_moder>`).
(为了解决这个问题，Net DIM 引入了一个按设备维度的控制项，用于修改和访问设备的 ``rx-profile`` 和
``tx-profile`` 参数。假设目标网络设备名为 ethx，并且 ethx 只声明支持 RX 配置档设置，且支持修改 ``usec`` 字
段和 ``pkts`` 字段（参见数据结构
:c:type:`struct dim_cq_moder <dim_cq_moder>`）。)

You can use ethtool to modify the current RX DIM profile where all
values are 64::
(可以使用 ethtool 将当前所有值均为 64 的 RX DIM 配置档修改为：)

    $ ethtool -C ethx rx-profile 1,1,n_2,2,n_3,n,n_n,4,n_n,n,n

``n`` means do not modify this field, and ``_`` separates structure
elements of the profile array.
(``n`` 表示不修改该字段，``_`` 用于分隔配置档数组的各个结构体元素。)

Querying the current profiles using::
(使用以下命令查询当前配置档：)

    $ ethtool -c ethx
    ...
    rx-profile:
    {.usec =   1, .pkts =   1, .comps = n/a,},
    {.usec =   2, .pkts =   2, .comps = n/a,},
    {.usec =   3, .pkts =  64, .comps = n/a,},
    {.usec =  64, .pkts =   4, .comps = n/a,},
    {.usec =  64, .pkts =  64, .comps = n/a,}
    tx-profile:   n/a

If the network device does not support specific fields of DIM profiles,
the corresponding ``n/a`` will display. If the ``n/a`` field is being
modified, error messages will be reported.
(如果网络设备不支持 DIM 配置档的某些特定字段，则会显示相应的 ``n/a`` 。如果尝试修改 ``n/a`` 字段，将报告错误
消息。)


Dynamic Interrupt Moderation (DIM) library API(动态中断调节 (DIM) 库 API)
=========================================================================

.. kernel-doc:: include/linux/dim.h
    :internal:
