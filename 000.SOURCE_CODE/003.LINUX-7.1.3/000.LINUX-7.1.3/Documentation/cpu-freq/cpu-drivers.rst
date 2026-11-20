.. SPDX-License-Identifier: GPL-2.0

====================================================================================
How to Implement a new CPUFreq Processor Driver(如何实现一个新的 CPUFreq 处理器驱动)
====================================================================================

Authors:
(作者：)


	- Dominik Brodowski  <linux@brodo.de>
	- Rafael J. Wysocki <rafael.j.wysocki@intel.com>
	- Viresh Kumar <viresh.kumar@linaro.org>

.. Contents

   1.   What To Do?
   1.1  Initialization
   1.2  Per-CPU Initialization
   1.3  verify
   1.4  target/target_index or setpolicy?
   1.5  target/target_index
   1.6  setpolicy
   1.7  get_intermediate and target_intermediate
   2.   Frequency Table Helpers



1. What To Do?(1. 要做什么？)
=============================

So, you just got a brand-new CPU / chipset with datasheets and want to
add cpufreq support for this CPU / chipset? Great. Here are some hints
on what is necessary:
(那么，你刚拿到一款全新的 CPU / 芯片组及其数据手册，想为这个 CPU /
芯片组添加 cpufreq 支持？很好。以下是一些关于需要做什么的提示：)


1.1 Initialization(1.1 初始化)
------------------------------

First of all, in an __initcall level 7 (module_init()) or later
function check whether this kernel runs on the right CPU and the right
chipset. If so, register a struct cpufreq_driver with the CPUfreq core
using cpufreq_register_driver()
(首先，在一个 __initcall 级别 7 (module_init()) 或更晚的函数中，检查本
内核是否运行在正确的 CPU 和芯片组上。如果是，使用 cpufreq_register_driver()
向 CPUfreq 核心注册一个 struct cpufreq_driver。)

What shall this struct cpufreq_driver contain?
(这个 struct cpufreq_driver 应当包含什么？)

 .name - The name of this driver.
 ( .name - 此驱动的名称。)

 .init - A pointer to the per-policy initialization function.
 ( .init - 指向每策略 (per-policy) 初始化函数的指针。)

 .verify - A pointer to a "verification" function.
 ( .verify - 指向一个 "验证" 函数的指针。)

 .setpolicy _or_ .fast_switch _or_ .target _or_ .target_index - See
 below on the differences.
 ( .setpolicy _或_ .fast_switch _或_ .target _或_ .target_index -
 它们之间的差别见下文。)

And optionally
(以及可选的：)

 .flags - Hints for the cpufreq core.
 ( .flags - 给 cpufreq 核心的提示。)

 .driver_data - cpufreq driver specific data.
 ( .driver_data - cpufreq 驱动特有的数据。)

 .get_intermediate and target_intermediate - Used to switch to stable
 frequency while changing CPU frequency.
 ( .get_intermediate 和 target_intermediate - 用于在改变 CPU 频率时
 切换到稳定频率。)

 .get - Returns current frequency of the CPU.
 ( .get - 返回 CPU 的当前频率。)

 .bios_limit - Returns HW/BIOS max frequency limitations for the CPU.
 ( .bios_limit - 返回该 CPU 的 HW/BIOS 最大频率限制。)

 .exit - A pointer to a per-policy cleanup function called during
 CPU_POST_DEAD phase of cpu hotplug process.
 ( .exit - 指向每策略清理函数的指针，在 cpu 热插拔过程的 CPU_POST_DEAD
 阶段被调用。)

 .suspend - A pointer to a per-policy suspend function which is called
 with interrupts disabled and _after_ the governor is stopped for the
 policy.
 ( .suspend - 指向每策略挂起函数的指针，它在关中断的情况下、且在该策略
 的 governor 停止 _之后_ 被调用。)

 .resume - A pointer to a per-policy resume function which is called
 with interrupts disabled and _before_ the governor is started again.
 ( .resume - 指向每策略恢复函数的指针，它在关中断的情况下、且在 governor
 再次启动 _之前_ 被调用。)

 .ready - A pointer to a per-policy ready function which is called after
 the policy is fully initialized.
 ( .ready - 指向每策略就绪函数的指针，在策略完全初始化之后被调用。)

 .attr - A pointer to a NULL-terminated list of "struct freq_attr" which
 allow to export values to sysfs.
 ( .attr - 指向以 NULL 结尾的 "struct freq_attr" 列表的指针，用于向
 sysfs 导出值。)

 .boost_enabled - If set, boost frequencies are enabled.
 ( .boost_enabled - 如果设置，则启用加速 (boost) 频率。)

 .set_boost - A pointer to a per-policy function to enable/disable boost
 frequencies.
 ( .set_boost - 指向用于启用/禁用加速频率的每策略函数的指针。)


1.2 Per-CPU Initialization(1.2 每 CPU 初始化)
---------------------------------------------

Whenever a new CPU is registered with the device model, or after the
cpufreq driver registers itself, the per-policy initialization function
cpufreq_driver.init is called if no cpufreq policy existed for the CPU.
Note that the .init() and .exit() routines are called only once for the
policy and not for each CPU managed by the policy. It takes a ``struct
cpufreq_policy *policy`` as argument. What to do now?
(每当一个新 CPU 注册到设备模型时，或在 cpufreq 驱动注册自身之后，如果该
CPU 尚不存在 cpufreq 策略，每策略初始化函数 cpufreq_driver.init 就会被
调用。注意，.init() 与 .exit() 例程对每个策略只调用一次，而不是对该策略
管理的每个 CPU 都调用。它接受一个 ``struct cpufreq_policy *policy`` 作为
参数。现在要做什么？)

If necessary, activate the CPUfreq support on your CPU.
(如有必要，在你的 CPU 上激活 CPUfreq 支持。)

Then, the driver must fill in the following values:
(然后，驱动必须填写以下值：)

+-----------------------------------+--------------------------------------+
|policy->cpuinfo.min_freq _and_	    |					   |
|policy->cpuinfo.max_freq	    | the minimum and maximum frequency	   |
|				    | (in kHz) which is supported by	   |
|				    | this CPU				   |
|				    | (该 CPU 支持的最小/最大频率)         |
+-----------------------------------+--------------------------------------+
|policy->cpuinfo.transition_latency | the time it takes on this CPU to	   |
|				    | switch between two frequencies in	   |
|				    | nanoseconds                          |
|				    | (在该 CPU 上切换两个频率所需的时间， |
|				    | 单位为纳秒)                          |
+-----------------------------------+--------------------------------------+
|policy->cur			    | The current operating frequency of   |
|				    | this CPU (if appropriate)		   |
|				    | (该 CPU 当前的运行频率 (如适用))     |
+-----------------------------------+--------------------------------------+
|policy->min,			    |					   |
|policy->max,			    |					   |
|policy->policy and, if necessary,  |					   |
|policy->governor		    | must contain the "default policy" for|
|				    | this CPU. A few moments later,       |
|				    | cpufreq_driver.verify and either     |
|				    | cpufreq_driver.setpolicy or          |
|				    | cpufreq_driver.target/target_index is|
|				    | called with these values.		   |
|				    | (必须包含此 CPU 的 "默认策略"。      |
|				    | 稍后片刻，cpufreq_driver.verify 与   |
|				    | cpufreq_driver.setpolicy 或          |
|				    | cpufreq_driver.target/target_index   |
|				    | 之一会以这些值被调用。)              |
+-----------------------------------+--------------------------------------+
|policy->cpus			    | Update this with the masks of the	   |
|				    | (online + offline) CPUs that do DVFS |
|				    | along with this CPU (i.e.  that share|
|				    | clock/voltage rails with it).	   |
|				    | (把与此 CPU 一起进行 DVFS 的         |
|				    | (在线 + 离线) CPU 的掩码更新到这里   |
|				    | (即与其共享时钟/电压轨的 CPU)。)     |
+-----------------------------------+--------------------------------------+

For setting some of these values (cpuinfo.min[max]_freq, policy->min[max]), the
frequency table helpers might be helpful. See the section 2 for more information
on them.
(要设置其中的一些值 (cpuinfo.min[max]_freq、policy->min[max])，频率表
辅助函数可能会有帮助。更多信息参见第 2 节。)


1.3 verify(原名)
----------------

When the user decides a new policy (consisting of
"policy,governor,min,max") shall be set, this policy must be validated
so that incompatible values can be corrected. For verifying these
values cpufreq_verify_within_limits(``struct cpufreq_policy *policy``,
``unsigned int min_freq``, ``unsigned int max_freq``) function might be helpful.
See section 2 for details on frequency table helpers.
(当用户决定要设置一个新策略 (由 "policy,governor,min,max" 组成) 时，必须
验证该策略，以便纠正不兼容的值。为了验证这些值，
cpufreq_verify_within_limits(``struct cpufreq_policy *policy``,
``unsigned int min_freq``, ``unsigned int max_freq``) 函数可能会有帮助。
频率表辅助函数的细节参见第 2 节。)

You need to make sure that at least one valid frequency (or operating
range) is within policy->min and policy->max. If necessary, increase
policy->max first, and only if this is no solution, decrease policy->min.
(你需要确保在 policy->min 与 policy->max 之间至少有一个有效频率 (或运行
范围)。如有必要，先增大 policy->max，只有当这仍然无济于事时才减小
policy->min。)


1.4 target or target_index or setpolicy or fast_switch?(1.4 target 或 target_index 或 setpolicy 或 fast_switch？)
-------------------------------------------------------------------------------------------------------------------

Most cpufreq drivers or even most cpu frequency scaling algorithms
only allow the CPU frequency to be set to predefined fixed values. For
these, you use the ->target(), ->target_index() or ->fast_switch()
callbacks.
(大多数 cpufreq 驱动，甚至大多数 CPU 频率调节算法，都只允许把 CPU 频率
设置为预定义的固定值。对于这些，你应当使用 ->target()、
->target_index() 或 ->fast_switch() 回调。)

Some cpufreq capable processors switch the frequency between certain
limits on their own. These shall use the ->setpolicy() callback.
(一些具备 cpufreq 能力的处理器会自己在某些限制之间切换频率。它们应当
使用 ->setpolicy() 回调。)


1.5. target/target_index(1.5. target/target_index)
--------------------------------------------------

The target_index call has two arguments: ``struct cpufreq_policy *policy``,
and ``unsigned int`` index (into the exposed frequency table).
(target_index 调用有两个参数：``struct cpufreq_policy *policy``，以及
``unsigned int`` index (指向暴露的频率表的下标)。)

The CPUfreq driver must set the new frequency when called here. The
actual frequency must be determined by freq_table[index].frequency.
(CPUfreq 驱动在此处被调用时必须设置新频率。实际频率必须由
freq_table[index].frequency 决定。)

It should always restore to earlier frequency (i.e. policy->restore_freq) in
case of errors, even if we switched to intermediate frequency earlier.
(出错时，它应当总是恢复到先前的频率 (即 policy->restore_freq)，即使我们
之前切换到了中间频率。)

Deprecated(已弃用)
------------------
The target call has three arguments: ``struct cpufreq_policy *policy``,
unsigned int target_frequency, unsigned int relation.
(target 调用有三个参数：``struct cpufreq_policy *policy``、
unsigned int target_frequency、unsigned int relation。)

The CPUfreq driver must set the new frequency when called here. The
actual frequency must be determined using the following rules:
(CPUfreq 驱动在此处被调用时必须设置新频率。实际频率必须使用以下规则
确定：)

- keep close to "target_freq"
  (接近 "target_freq")
- policy->min <= new_freq <= policy->max (THIS MUST BE VALID!!!)
  (policy->min <= new_freq <= policy->max (这必须有效！！！))
- if relation==CPUFREQ_REL_L, try to select a new_freq higher than or equal
  target_freq. ("L for lowest, but no lower than")
  (如果 relation==CPUFREQ_REL_L，尝试选择大于等于 target_freq 的
  new_freq。("L 代表 lowest，但不低于"))
- if relation==CPUFREQ_REL_H, try to select a new_freq lower than or equal
  target_freq. ("H for highest, but no higher than")
  (如果 relation==CPUFREQ_REL_H，尝试选择小于等于 target_freq 的
  new_freq。("H 代表 highest，但不高于"))

Here again the frequency table helper might assist you - see section 2
for details.
(在这里，频率表辅助函数同样可以帮你 —— 细节参见第 2 节。)

1.6. fast_switch(1.6. fast_switch)
----------------------------------

This function is used for frequency switching from scheduler's context.
Not all drivers are expected to implement it, as sleeping from within
this callback isn't allowed. This callback must be highly optimized to
do switching as fast as possible.
(该函数用于从调度器上下文中切换频率。并不是所有驱动都要实现它，因为不
允许在这个回调内部睡眠。此回调必须高度优化，以尽可能快地完成切换。)

This function has two arguments: ``struct cpufreq_policy *policy`` and
``unsigned int target_frequency``.
(该函数有两个参数：``struct cpufreq_policy *policy`` 与
``unsigned int target_frequency``。)


1.7 setpolicy(1.7 setpolicy)
----------------------------

The setpolicy call only takes a ``struct cpufreq_policy *policy`` as
argument. You need to set the lower limit of the in-processor or
in-chipset dynamic frequency switching to policy->min, the upper limit
to policy->max, and -if supported- select a performance-oriented
setting when policy->policy is CPUFREQ_POLICY_PERFORMANCE, and a
powersaving-oriented setting when CPUFREQ_POLICY_POWERSAVE. Also check
the reference implementation in drivers/cpufreq/longrun.c
(setpolicy 调用只接受一个 ``struct cpufreq_policy *policy`` 作为参数。
你需要把处理器内部或芯片组内部动态频率切换的下限设置为 policy->min，
上限设置为 policy->max，并且 (如果支持的话) 在 policy->policy 为
CPUFREQ_POLICY_PERFORMANCE 时选择面向性能的设置，在
CPUFREQ_POLICY_POWERSAVE 时选择面向省电的设置。另请参考
drivers/cpufreq/longrun.c 中的参考实现。)

1.8 get_intermediate and target_intermediate(1.8 get_intermediate 与 target_intermediate)
-----------------------------------------------------------------------------------------

Only for drivers with target_index() and CPUFREQ_ASYNC_NOTIFICATION unset.
(仅适用于实现了 target_index() 且未设置 CPUFREQ_ASYNC_NOTIFICATION 的
驱动。)

get_intermediate should return a stable intermediate frequency platform wants to
switch to, and target_intermediate() should set CPU to that frequency, before
jumping to the frequency corresponding to 'index'. Core will take care of
sending notifications and driver doesn't have to handle them in
target_intermediate() or target_index().
(get_intermediate 应当返回平台想要切换到的稳定中间频率，而
target_intermediate() 应当在跳转到与 'index' 对应的频率之前，把 CPU 设置
为该频率。核心会负责发送通知，驱动不必在 target_intermediate() 或
target_index() 中处理它们。)

Drivers can return '0' from get_intermediate() in case they don't wish to switch
to intermediate frequency for some target frequency. In that case core will
directly call ->target_index().
(如果驱动不想为某个目标频率切换到中间频率，可以从 get_intermediate()
返回 '0'。在这种情况下，核心会直接调用 ->target_index()。)

NOTE: ->target_index() should restore to policy->restore_freq in case of
failures as core would send notifications for that.
(注意：失败时 ->target_index() 应当恢复到 policy->restore_freq，因为核心
会为此发送通知。)


2. Frequency Table Helpers(2. 频率表辅助函数)
=============================================

As most cpufreq processors only allow for being set to a few specific
frequencies, a "frequency table" with some functions might assist in
some work of the processor driver. Such a "frequency table" consists of
an array of struct cpufreq_frequency_table entries, with driver specific
values in "driver_data", the corresponding frequency in "frequency" and
flags set. At the end of the table, you need to add a
cpufreq_frequency_table entry with frequency set to CPUFREQ_TABLE_END.
And if you want to skip one entry in the table, set the frequency to
CPUFREQ_ENTRY_INVALID. The entries don't need to be in sorted in any
particular order, but if they are cpufreq core will do DVFS a bit
quickly for them as search for best match is faster.
(由于大多数支持 cpufreq 的处理器只允许被设置到少数几个特定频率，一个带
有一些函数的 "频率表" 可以协助处理器驱动的部分工作。这样的 "频率表" 由
一个 struct cpufreq_frequency_table 条目数组组成，其中 "driver_data" 为
驱动特有的值，"frequency" 为对应频率，并设置好标志。在表的末尾，你需要
添加一个 frequency 设置为 CPUFREQ_TABLE_END 的 cpufreq_frequency_table
条目。如果你想跳过表中的某个条目，把其 frequency 设置为
CPUFREQ_ENTRY_INVALID。这些条目不需要按任何特定顺序排序，但如果有序，
cpufreq 核心会为它们稍快地执行 DVFS，因为最佳匹配的搜索更快。)

The cpufreq table is verified automatically by the core if the policy contains a
valid pointer in its policy->freq_table field.
(如果策略的 policy->freq_table 字段包含一个有效指针，核心会自动验证
cpufreq 表。)

cpufreq_frequency_table_verify() assures that at least one valid
frequency is within policy->min and policy->max, and all other criteria
are met. This is helpful for the ->verify call.
(cpufreq_frequency_table_verify() 确保 policy->min 与 policy->max 之间
至少有一个有效频率，并且满足所有其他判据。这对 ->verify 调用很有
帮助。)

cpufreq_frequency_table_target() is the corresponding frequency table
helper for the ->target stage. Just pass the values to this function,
and this function returns the of the frequency table entry which
contains the frequency the CPU shall be set to.
(cpufreq_frequency_table_target() 是 ->target 阶段对应的频率表辅助
函数。只需把值传给该函数，它就会返回 CPU 应当被设置到的频率所在的那个
频率表条目。)

The following macros can be used as iterators over cpufreq_frequency_table:
(以下宏可以用作对 cpufreq_frequency_table 的迭代器：)

cpufreq_for_each_entry(pos, table) - iterates over all entries of frequency
table.
(cpufreq_for_each_entry(pos, table) - 迭代频率表的所有条目。)

cpufreq_for_each_valid_entry(pos, table) - iterates over all entries,
excluding CPUFREQ_ENTRY_INVALID frequencies.
Use arguments "pos" - a ``cpufreq_frequency_table *`` as a loop cursor and
"table" - the ``cpufreq_frequency_table *`` you want to iterate over.
(cpufreq_for_each_valid_entry(pos, table) - 迭代所有条目，排除
CPUFREQ_ENTRY_INVALID 频率。使用参数 "pos" (一个 ``cpufreq_frequency_table
*``，作为循环游标) 与 "table" (你想要迭代的 ``cpufreq_frequency_table
*``)。)

For example::

	struct cpufreq_frequency_table *pos, *driver_freq_table;

	cpufreq_for_each_entry(pos, driver_freq_table) {
		/* Do something with pos */
		pos->frequency = ...
	}

(例如：)

If you need to work with the position of pos within driver_freq_table,
do not subtract the pointers, as it is quite costly. Instead, use the
macros cpufreq_for_each_entry_idx() and cpufreq_for_each_valid_entry_idx().
(如果你需要处理 pos 在 driver_freq_table 中的位置，不要用指针相减，因为
代价相当高。应改用宏 cpufreq_for_each_entry_idx() 与
cpufreq_for_each_valid_entry_idx()。)
