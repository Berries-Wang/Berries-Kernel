.. SPDX-License-Identifier: GPL-2.0

======================================================================================================
General description of the CPUFreq core and CPUFreq notifiers(CPUFreq 核心与 CPUFreq 通知器的一般描述)
======================================================================================================

Authors: (作者：)
	- Dominik Brodowski  <linux@brodo.de>
	- David Kimdon <dwhedon@debian.org>
	- Rafael J. Wysocki <rafael.j.wysocki@intel.com>
	- Viresh Kumar <viresh.kumar@linaro.org>

.. Contents:

   1.  CPUFreq core and interfaces
   2.  CPUFreq notifiers
   3.  CPUFreq Table Generation with Operating Performance Point (OPP)

1. General Information(1. 一般信息)
===================================

The CPUFreq core code is located in drivers/cpufreq/cpufreq.c. This
cpufreq code offers a standardized interface for the CPUFreq
architecture drivers (those pieces of code that do actual
frequency transitions), as well as to "notifiers". These are device
drivers or other part of the kernel that need to be informed of
policy changes (ex. thermal modules like ACPI) or of all
frequency changes (ex. timing code) or even need to force certain
speed limits (like LCD drivers on ARM architecture). Additionally, the
kernel "constant" loops_per_jiffy is updated on frequency changes
here.
(CPUFreq 核心代码位于 drivers/cpufreq/cpufreq.c。这段 cpufreq 代码为 CPUFreq 架构驱动（执行实际频率转换的那部分代码）以及"通知器"提供了标准化接口。这些是需要在策略更改（例如 ACPI 之类的热管理模块）、所有频率更改（例如计时代码）时得到通知，甚至需要强制某些速度限制（例如 ARM 架构上的 LCD 驱动程序）的设备驱动程序或内核的其他部分。此外，内核"常量" loops_per_jiffy 会在此处随频率更改而更新。)

Reference counting of the cpufreq policies is done by cpufreq_cpu_get
and cpufreq_cpu_put, which make sure that the cpufreq driver is
correctly registered with the core, and will not be unloaded until
cpufreq_put_cpu is called. That also ensures that the respective cpufreq
policy doesn't get freed while being used.
(cpufreq 策略的引用计数由 cpufreq_cpu_get 和 cpufreq_cpu_put 完成，它们确保 cpufreq 驱动程序向核心正确注册，并且在调用 cpufreq_put_cpu 之前不会被卸载。这也确保相应的 cpufreq 策略在使用期间不会被释放。)

2. CPUFreq notifiers(2. CPUFreq 通知器)
=======================================

CPUFreq notifiers conform to the standard kernel notifier interface.
See linux/include/linux/notifier.h for details on notifiers.
(CPUFreq 通知器遵循标准的内核通知器接口。通知器的详细信息参见 linux/include/linux/notifier.h。)

There are two different CPUFreq notifiers - policy notifiers and
transition notifiers.
(有两种不同的 CPUFreq 通知器——策略通知器和频率切换通知器。)


2.1 CPUFreq policy notifiers(2.1 CPUFreq 策略通知器)
----------------------------------------------------

These are notified when a new policy is created or removed.
(当创建或移除新策略时会通知它们。)

The phase is specified in the second argument to the notifier.  The phase is
CPUFREQ_CREATE_POLICY when the policy is first created and it is
CPUFREQ_REMOVE_POLICY when the policy is removed.
(阶段在通知器的第二个参数中指定。策略首次创建时阶段为 CPUFREQ_CREATE_POLICY，策略被移除时为 CPUFREQ_REMOVE_POLICY。)

The third argument, a ``void *pointer``, points to a struct cpufreq_policy
consisting of several values, including min, max (the lower and upper
frequencies (in kHz) of the new policy).
(第三个参数是一个 ``void *pointer`` ，指向一个 struct cpufreq_policy，其中包含若干值，包括 min、max（新策略的下限和上限频率（以 kHz 为单位））。)


2.2 CPUFreq transition notifiers(2.2 CPUFreq 频率切换通知器)
------------------------------------------------------------

These are notified twice for each online CPU in the policy, when the
CPUfreq driver switches the CPU core frequency and this change has no
any external implications.
(当 CPUfreq 驱动程序切换 CPU 核心频率且此更改没有任何外部影响时，策略中的每个在线 CPU 都会收到两次通知。)

The second argument specifies the phase - CPUFREQ_PRECHANGE or
CPUFREQ_POSTCHANGE.
(第二个参数指定阶段——CPUFREQ_PRECHANGE 或 CPUFREQ_POSTCHANGE。)

The third argument is a struct cpufreq_freqs with the following
values:
(第三个参数是一个 struct cpufreq_freqs，其包含以下值：)

======	======================================
policy	a pointer to the struct cpufreq_policy
old	old frequency
new	new frequency
flags	flags of the cpufreq driver
======	======================================

(表内字段：policy 为指向 struct cpufreq_policy 的指针，old 为旧频率，new 为新频率，flags 为 cpufreq 驱动程序的标志。)

3. CPUFreq Table Generation with Operating Performance Point (OPP)(3. 使用运行性能点（OPP）生成 CPUFreq 表)
===========================================================================================================
For details about OPP, see Documentation/power/opp.rst
(有关 OPP 的详细信息，参见 Documentation/power/opp.rst)

dev_pm_opp_init_cpufreq_table -
	This function provides a ready to use conversion routine to translate
	the OPP layer's internal information about the available frequencies
	into a format readily providable to cpufreq.
        (此函数提供一个可直接使用的转换例程，把 OPP 层关于可用频率的内部信息转换为可轻松提供给 cpufreq 的格式。)

	.. Warning::

	   Do not use this function in interrupt context.
	   (不要在中断上下文中使用此函数。)

	Example::

	 soc_pm_init()
	 {
		/* Do things */
		r = dev_pm_opp_init_cpufreq_table(dev, &freq_table);
		if (!r)
			policy->freq_table = freq_table;
		/* Do other things */
	 }

	(示例：)

	.. note::

	   This function is available only if CONFIG_CPU_FREQ is enabled in
	   addition to CONFIG_PM_OPP.
	   (仅当 CONFIG_CPU_FREQ 与 CONFIG_PM_OPP 一起启用时，此函数才可用。)

dev_pm_opp_free_cpufreq_table
	Free up the table allocated by dev_pm_opp_init_cpufreq_table
        (释放由 dev_pm_opp_init_cpufreq_table 分配的表)
