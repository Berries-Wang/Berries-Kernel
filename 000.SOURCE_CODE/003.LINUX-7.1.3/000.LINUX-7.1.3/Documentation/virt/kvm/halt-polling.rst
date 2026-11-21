.. SPDX-License-Identifier: GPL-2.0

=============================================
The KVM halt polling system(KVM 停机轮询系统)
=============================================

The KVM halt polling system provides a feature within KVM whereby the latency
of a guest can, under some circumstances, be reduced by polling in the host
for some time period after the guest has elected to no longer run by cedeing.
That is, when a guest vcpu has ceded, or in the case of powerpc when all of the
vcpus of a single vcore have ceded, the host kernel polls for wakeup conditions
before giving up the cpu to the scheduler in order to let something else run.
(KVM 停机轮询系统在 KVM 内提供一项特性：在某些情况下，当客户机通过让出 (cede)
选择不再运行之后，可以在宿主机中轮询一段时间，从而降低客户机的延迟。也就是说，
当客户机的某个 vcpu 已经让出，或者在 powerpc 上单个 vcore 的所有 vcpu 都已
让出时，宿主内核会轮询唤醒条件，之后才把 CPU 交给调度器去运行其他任务。)

Polling provides a latency advantage in cases where the guest can be run again
very quickly by at least saving us a trip through the scheduler, normally on
the order of a few micro-seconds, although performance benefits are workload
dependent. In the event that no wakeup source arrives during the polling
interval or some other task on the runqueue is runnable the scheduler is
invoked. Thus halt polling is especially useful on workloads with very short
wakeup periods where the time spent halt polling is minimised and the time
savings of not invoking the scheduler are distinguishable.
(在客户机可以被很快再次运行的场景下，轮询带来延迟上的优势，至少省去了一次
进入调度器的过程，通常在几微秒的量级，不过性能收益取决于工作负载。如果在
轮询间隔内没有唤醒源到达，或者运行队列上有其他任务可运行，就会调用调度器。
因此，停机轮询对唤醒周期极短的工作负载尤其有用——在这类负载上，停机轮询
耗费的时间被降到最低，而省去调度器调用所节省的时间是可辨别的。)

The generic halt polling code is implemented in:
(通用的停机轮询代码实现于：)

	virt/kvm/kvm_main.c: kvm_vcpu_block()

The powerpc kvm-hv specific case is implemented in:
(powerpc kvm-hv 的特定情况实现于：)

	arch/powerpc/kvm/book3s_hv.c: kvmppc_vcore_blocked()

Halt Polling Interval(停机轮询间隔)
===================================

The maximum time for which to poll before invoking the scheduler, referred to
as the halt polling interval, is increased and decreased based on the perceived
effectiveness of the polling in an attempt to limit pointless polling.
This value is stored in either the vcpu struct:
(在调用调度器之前轮询的最长时间，称为停机轮询间隔，会根据轮询的感知效果
进行增减，以尽量减少无意义的轮询。该值存储在 vcpu 结构体中：)

	kvm_vcpu->halt_poll_ns

or in the case of powerpc kvm-hv, in the vcore struct:
(或者在 powerpc kvm-hv 的情况下，存储在 vcore 结构体中：)

	kvmppc_vcore->halt_poll_ns

Thus this is a per vcpu (or vcore) value.
(因此这是一个 per vcpu（或 vcore）的值。)

During polling if a wakeup source is received within the halt polling interval,
the interval is left unchanged. In the event that a wakeup source isn't
received during the polling interval (and thus schedule is invoked) there are
two options, either the polling interval and total block time[0] were less than
the global max polling interval (see module params below), or the total block
time was greater than the global max polling interval.
(轮询期间，如果在停机轮询间隔内收到唤醒源，则该间隔保持不变。如果在轮询
间隔内未收到唤醒源（因而调用了调度），则有两种情况：要么轮询间隔和总阻塞
时间[0]小于全局最大轮询间隔（参见下面的模块参数），要么总阻塞时间大于全局
最大轮询间隔。)

In the event that both the polling interval and total block time were less than
the global max polling interval then the polling interval can be increased in
the hope that next time during the longer polling interval the wake up source
will be received while the host is polling and the latency benefits will be
received. The polling interval is grown in the function grow_halt_poll_ns() and
is multiplied by the module parameters halt_poll_ns_grow and
halt_poll_ns_grow_start.
(如果轮询间隔和总阻塞时间都小于全局最大轮询间隔，则可以增大轮询间隔，希望
下次在更长的轮询间隔内，宿主机轮询时就能收到唤醒源，从而获得延迟上的好处。
轮询间隔在 grow_halt_poll_ns() 函数中增大，乘以模块参数 halt_poll_ns_grow
和 halt_poll_ns_grow_start。)

In the event that the total block time was greater than the global max polling
interval then the host will never poll for long enough (limited by the global
max) to wakeup during the polling interval so it may as well be shrunk in order
to avoid pointless polling. The polling interval is shrunk in the function
shrink_halt_poll_ns() and is divided by the module parameter
halt_poll_ns_shrink, or set to 0 iff halt_poll_ns_shrink == 0.
(如果总阻塞时间大于全局最大轮询间隔，那么宿主机永远轮询不了足够长的时间
（受全局最大值限制）以致在轮询间隔内等到唤醒，因此不妨将其缩小，以避免
无意义的轮询。轮询间隔在 shrink_halt_poll_ns() 函数中缩小，除以模块参数
halt_poll_ns_shrink；当 halt_poll_ns_shrink == 0 时则置为 0。)

It is worth noting that this adjustment process attempts to hone in on some
steady state polling interval but will only really do a good job for wakeups
which come at an approximately constant rate, otherwise there will be constant
adjustment of the polling interval.
(值得注意的是，这一调整过程试图收敛到某个稳态轮询间隔，但它只对以大致恒定
速率到来的唤醒真正有效，否则轮询间隔会被不停地调整。)

[0] total block time:
		      the time between when the halt polling function is
		      invoked and a wakeup source received (irrespective of
		      whether the scheduler is invoked within that function).
([0] 总阻塞时间：
		      指从调用停机轮询函数到收到唤醒源之间的时间（无论
		      调度器是否在该函数内被调用）。)

Module Parameters(模块参数)
===========================

The kvm module has 4 tunable module parameters to adjust the global max polling
interval, the initial value (to grow from 0), and the rate at which the polling
interval is grown and shrunk. These variables are defined in
include/linux/kvm_host.h and as module parameters in virt/kvm/kvm_main.c, or
arch/powerpc/kvm/book3s_hv.c in the powerpc kvm-hv case.
(kvm 模块有 4 个可调的模块参数，用于调整全局最大轮询间隔、初始值（从 0
增长时的起点）以及轮询间隔增大和缩小的速率。这些变量定义在
include/linux/kvm_host.h 中，并作为模块参数定义在 virt/kvm/kvm_main.c 中，
在 powerpc kvm-hv 的情况下则定义在 arch/powerpc/kvm/book3s_hv.c 中。)

+-----------------------+---------------------------+-------------------------+
|Module Parameter	|   Description		    |	     Default Value    |
|(模块参数)             |(描述)                     |(默认值)                 |
+-----------------------+---------------------------+-------------------------+
|halt_poll_ns		| The global max polling    | KVM_HALT_POLL_NS_DEFAULT|
|			| interval which defines    |			      |
|			| the ceiling value of the  |			      |
|			| polling interval for      | (per arch value)	      |
|			| each vcpu.		    |			      |
|                       |全局最大轮询间隔，定义     |                         |
|                       |每个 vcpu 轮询间隔的上     |                         |
|                       |限值。                     |                         |
+-----------------------+---------------------------+-------------------------+
|halt_poll_ns_grow	| The value by which the    | 2			      |
|			| halt polling interval is  |			      |
|			| multiplied in the	    |			      |
|			| grow_halt_poll_ns()	    |			      |
|			| function.		    |			      |
|                       |grow_halt_poll_ns() 函     |                         |
|                       |数中停机轮询间隔的乘数。   |                         |
+-----------------------+---------------------------+-------------------------+
|halt_poll_ns_grow_start| The initial value to grow | 10000		      |
|			| to from zero in the	    |			      |
|			| grow_halt_poll_ns()	    |			      |
|			| function.		    |			      |
|                       |在 grow_halt_poll_ns()     |                         |
|                       |函数中从零增长到的初始值。 |                         |
+-----------------------+---------------------------+-------------------------+
|halt_poll_ns_shrink	| The value by which the    | 2			      |
|			| halt polling interval is  |			      |
|			| divided in the	    |			      |
|			| shrink_halt_poll_ns()	    |			      |
|			| function.		    |			      |
|                       |shrink_halt_poll_ns()      |                         |
|                       |函数中停机轮询间隔的除数。 |                         |
+-----------------------+---------------------------+-------------------------+

These module parameters can be set from the sysfs files in:
(这些模块参数可以通过以下 sysfs 文件设置：)

	/sys/module/kvm/parameters/

Note: these module parameters are system-wide values and are not able to
      be tuned on a per vm basis.
(注意：这些模块参数是系统级的值，无法按 VM 单独调整。)

Any changes to these parameters will be picked up by new and existing vCPUs the
next time they halt, with the notable exception of VMs using KVM_CAP_HALT_POLL
(see next section).
(这些参数的任何更改都会在新的和已有的 vCPU 下次停机时生效，使用
KVM_CAP_HALT_POLL 的 VM 是明显的例外（参见下一节）。)

KVM_CAP_HALT_POLL(原名)
=======================

KVM_CAP_HALT_POLL is a VM capability that allows userspace to override halt_poll_ns
on a per-VM basis. VMs using KVM_CAP_HALT_POLL ignore halt_poll_ns completely (but
still obey halt_poll_ns_grow, halt_poll_ns_grow_start, and halt_poll_ns_shrink).
(KVM_CAP_HALT_POLL 是一种 VM 能力，允许用户空间按 VM 覆盖 halt_poll_ns。使用
KVM_CAP_HALT_POLL 的 VM 完全忽略 halt_poll_ns（但仍遵循 halt_poll_ns_grow、
halt_poll_ns_grow_start 和 halt_poll_ns_shrink）。)

See Documentation/virt/kvm/api.rst for more information on this capability.
(有关此能力的更多信息，参见 Documentation/virt/kvm/api.rst。)

Further Notes(补充说明)
=======================

- Care should be taken when setting the halt_poll_ns module parameter as a large value
  has the potential to drive the cpu usage to 100% on a machine which would be almost
  entirely idle otherwise. This is because even if a guest has wakeups during which very
  little work is done and which are quite far apart, if the period is shorter than the
  global max polling interval (halt_poll_ns) then the host will always poll for the
  entire block time and thus cpu utilisation will go to 100%.
  (设置 halt_poll_ns 模块参数时要小心，因为较大的值有可能把一台几乎完全空闲
  的机器的 CPU 使用率推到 100%。这是因为，即使客户机的唤醒期间完成的工作
  很少且彼此相隔很远，只要该周期短于全局最大轮询间隔（halt_poll_ns），宿主机
  就总是会在整个阻塞时间内轮询，因此 CPU 利用率会达到 100%。)

- Halt polling essentially presents a trade-off between power usage and latency and
  the module parameters should be used to tune the affinity for this. Idle cpu time is
  essentially converted to host kernel time with the aim of decreasing latency when
  entering the guest.
  (停机轮询本质上呈现的是功耗与延迟之间的权衡，应使用模块参数来调整对此的
  倾向。空闲的 CPU 时间实质上被转换为宿主内核时间，目的是降低进入客户机时
  的延迟。)

- Halt polling will only be conducted by the host when no other tasks are runnable on
  that cpu, otherwise the polling will cease immediately and schedule will be invoked to
  allow that other task to run. Thus this doesn't allow a guest to cause denial of service
  of the cpu.
  (只有当该 CPU 上没有其他可运行任务时，宿主机才会进行停机轮询；否则轮询会
  立即停止，并调用调度器让那个任务运行。因此这不允许客户机对 CPU 造成拒绝
  服务。)
