==================================
Guest halt polling(客户机停机轮询)
==================================

The cpuidle_haltpoll driver, with the haltpoll governor, allows
the guest vcpus to poll for a specified amount of time before
halting.
(cpuidle_haltpoll 驱动与 haltpoll governor 一起，允许客户机 vcpu 在停
机之前轮询一段指定的时间。)

This provides the following benefits to host side polling:
(这为主机侧轮询提供了以下好处：)

	1) The POLL flag is set while polling is performed, which allows
	   a remote vCPU to avoid sending an IPI (and the associated
	   cost of handling the IPI) when performing a wakeup.
	   (执行轮询时会置位 POLL 标志，这使远程 vCPU 在执行唤醒时可以避
	   免发送 IPI (以及处理 IPI 的相关开销)。)

	2) The VM-exit cost can be avoided.
	   (可以避免 VM-exit 的开销。)

The downside of guest side polling is that polling is performed
even with other runnable tasks in the host.
(客户机侧轮询的缺点在于：即使主机中还有其他可运行的任务，轮询也会执
行。)

The basic logic as follows: A global value, guest_halt_poll_ns,
is configured by the user, indicating the maximum amount of
time polling is allowed. This value is fixed.
(基本逻辑如下：用户配置一个全局值 guest_halt_poll_ns，它指示允许轮询
的最长时间。该值是固定的。)

Each vcpu has an adjustable guest_halt_poll_ns
("per-cpu guest_halt_poll_ns"), which is adjusted by the algorithm
in response to events (explained below).
(每个 vcpu 都有一个可调节的 guest_halt_poll_ns ("per-cpu
guest_halt_poll_ns")，它由算法根据事件进行调整 (说明见下文)。)

Module Parameters(模块参数)
===========================

The haltpoll governor has 5 tunable module parameters:
(haltpoll governor 有 5 个可调的模块参数：)

1) guest_halt_poll_ns:

Maximum amount of time, in nanoseconds, that polling is
performed before halting.
(停机之前执行轮询的最长时间，以纳秒为单位。)

Default: 200000
(默认值：200000)

2) guest_halt_poll_shrink:

Division factor used to shrink per-cpu guest_halt_poll_ns when
wakeup event occurs after the global guest_halt_poll_ns.
(当唤醒事件发生在全局 guest_halt_poll_ns 之后时，用于收缩 per-cpu
guest_halt_poll_ns 的除法因子。)

Default: 2
(默认值：2)

3) guest_halt_poll_grow:

Multiplication factor used to grow per-cpu guest_halt_poll_ns
when event occurs after per-cpu guest_halt_poll_ns
but before global guest_halt_poll_ns.
(当事件发生在 per-cpu guest_halt_poll_ns 之后但在全局
guest_halt_poll_ns 之前时，用于增大 per-cpu guest_halt_poll_ns 的乘法
因子。)

Default: 2
(默认值：2)

4) guest_halt_poll_grow_start:

The per-cpu guest_halt_poll_ns eventually reaches zero
in case of an idle system. This value sets the initial
per-cpu guest_halt_poll_ns when growing. This can
be increased from 10000, to avoid misses during the initial
growth stage:
(在系统空闲的情况下，per-cpu guest_halt_poll_ns 最终会降到零。该值设
置增长时初始的 per-cpu guest_halt_poll_ns。可以从 10000 开始增大该值，
以避免在初始增长阶段出现未命中：)

10k, 20k, 40k, ... (example assumes guest_halt_poll_grow=2).
(10k、20k、40k、…… (示例假设 guest_halt_poll_grow=2)。)

Default: 50000
(默认值：50000)

5) guest_halt_poll_allow_shrink:

Bool parameter which allows shrinking. Set to N
to avoid it (per-cpu guest_halt_poll_ns will remain
high once achieves global guest_halt_poll_ns value).
(允许收缩的布尔参数。设为 N 可避免收缩 (一旦 per-cpu
guest_halt_poll_ns 达到全局 guest_halt_poll_ns 值，它将保持较高水
平)。)

Default: Y
(默认值：Y)

The module parameters can be set from the sysfs files in::

	/sys/module/haltpoll/parameters/

(模块参数可以通过以下 sysfs 文件设置：)

Further Notes(补充说明)
=======================

- Care should be taken when setting the guest_halt_poll_ns parameter as a
  large value has the potential to drive the cpu usage to 100% on a machine
  which would be almost entirely idle otherwise.
  (设置 guest_halt_poll_ns 参数时应当小心，因为一个较大的值有可能把
  CPU 使用率推到 100%，而该机器本来几乎完全空闲。)
