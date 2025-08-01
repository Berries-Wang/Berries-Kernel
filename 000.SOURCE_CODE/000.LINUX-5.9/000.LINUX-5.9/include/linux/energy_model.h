/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ENERGY_MODEL_H
#define _LINUX_ENERGY_MODEL_H
#include <linux/cpumask.h>
#include <linux/device.h>
#include <linux/jump_label.h>
#include <linux/kobject.h>
#include <linux/rcupdate.h>
#include <linux/sched/cpufreq.h>
#include <linux/sched/topology.h>
#include <linux/types.h>

/**
 * em_perf_state - Performance state of a performance domain (性能域的性能状态)
 * 
 * @frequency:	The frequency in KHz, for consistency with CPUFreq (频率值（单位为KHz），以保持与CPUFreq的一致性)
 * @power:	The power consumed at this level, in milli-watts (by 1 CPU or
 *		by a registered device). It can be a total power: static and
 *		dynamic.(该性能级别下的功耗（单位为毫瓦），可对应单个CPU或已注册设备的功耗值。此数值可以是静态功耗与动态功耗的总和。)
 * @cost:	The cost coefficient associated with this level, used during
 *		energy calculation. Equal to: power * max_frequency / frequency
 *      (该性能级别(该频率下的)对应的能耗计算系数（计算公式为：功耗 * 最大频率 / 当前频率），用于系统能耗评估计算)
 */
struct em_perf_state {
	unsigned long frequency;
	unsigned long power;
	unsigned long cost;
};

/**
 * em_perf_domain - Performance domain(性能域)
 * 
 * @table:		List of performance states, in ascending order  (table指表述CPU频率和功耗之间关系的一个表)
 * @nr_perf_states:	Number of performance states (nr_cap_states表示CPU有多少个频率点或者能力点)
 * @cpus:		Cpumask covering the CPUs of the domain. It's here for performance reasons to avoid potential cache
 *			    misses during energy calculations in the scheduler and simplifies allocating/freeing that memory region.
 *          (该cpumask涵盖了此域（domain）中的所有CPU。此处引入主要是出于性能考量，既可避免调度器在能量计算过程中可能出现的缓存未命中问题，又能简化该内存区域的分配/释放操作。)
 *  (cpus表示这个性能域所包含的CPU位图)
 *
 * In case of CPU device, a "performance domain" represents a group of CPUs
 * whose performance is scaled together. All CPUs of a performance domain
 * must have the same micro-architecture. Performance domains often have
 * a 1-to-1 mapping with CPUFreq policies. In case of other devices the @cpus
 * field is unused.
 * (对于CPU设备而言，"性能域(performance domain)"代表一组需要同步调节性能的CPU集合。同一个性能域中的所有CPU必须采用相同的微架构。性能域通常与CPUFreq策略存在一一对应关系。若为其他类型的设备，则@cpus字段不会被使用)
 * 
 * 注意事项:
 * - 通常在同一个性能域中的CPU必须拥有同样的架构
 * 
 * 
 * em_perf_domain数据结构中的table成员指向一个表，表的每项描述一个动态电压和调频性能操作点（Dynamic Voltage and Frequency Scaling Operating Performance Point，DVFS OPP）。
 * 性能操作点（OPP）指SoC中某个电源域（power domain）的频率和电压的节点，通常指CPU。通常CPU的核心电压和CPU频率存在某种对应的关系，高频率必然需要比较高的CPU内核电压，同时也会带来高功耗。
 * CPUfreq模块驱动通常维护着一个频率和电压的对应表，每个表项就是OPP
 * - OPP用em_perf_state数据结构来描述，它用来描述CPU的频率和功耗之间的关系
 */
struct em_perf_domain {
	struct em_perf_state *table;
	int nr_perf_states;
	unsigned long cpus[];
};

#define em_span_cpus(em) (to_cpumask((em)->cpus))

#ifdef CONFIG_ENERGY_MODEL
#define EM_MAX_POWER 0xFFFF

struct em_data_callback {
	/**
	 * active_power() - Provide power at the next performance state of
	 *		a device
	 * @power	: Active power at the performance state in mW
	 *		(modified)
	 * @freq	: Frequency at the performance state in kHz
	 *		(modified)
	 * @dev		: Device for which we do this operation (can be a CPU)
	 *
	 * active_power() must find the lowest performance state of 'dev' above
	 * 'freq' and update 'power' and 'freq' to the matching active power
	 * and frequency.
	 *
	 * In case of CPUs, the power is the one of a single CPU in the domain,
	 * expressed in milli-watts. It is expected to fit in the
	 * [0, EM_MAX_POWER] range.
	 *
	 * Return 0 on success.
	 */
	int (*active_power)(unsigned long *power, unsigned long *freq,
			    struct device *dev);
};
#define EM_DATA_CB(_active_power_cb) { .active_power = &_active_power_cb }

struct em_perf_domain *em_cpu_get(int cpu);
struct em_perf_domain *em_pd_get(struct device *dev);
int em_dev_register_perf_domain(struct device *dev, unsigned int nr_states,
				struct em_data_callback *cb, cpumask_t *span);
void em_dev_unregister_perf_domain(struct device *dev);

/**
 * em_cpu_energy() - Estimates the energy consumed by the CPUs of a performance domain (估算性能域内所有CPU的能耗)
 * @pd		    : performance domain for which energy has to be estimated
 * @max_util	: highest utilization among CPUs of the domain (max_util表示在这个性能域中所有的CPU里面最高的CPU使用率)
 * @sum_util	: sum of the utilization of all CPUs in the domain (sum_util表示所有CPU的总CPU使用率)
 *
 * This function must be used only for CPU devices. There is no validation,
 * i.e. if the EM is a CPU type and has cpumask allocated. It is called from
 * the scheduler code quite frequently and that is why there is not checks.
 * (该函数仅适用于CPU设备，且不进行任何验证（包括不检查能量模型是否为CPU类型或是否已分配cpumask）。由于该函数在调度代码中频繁调用，故未设置校验环节)
 *
 * Return: the sum of the energy consumed by the CPUs of the domain assuming
 * a capacity state satisfying the max utilization of the domain.
 * (返回值：在满足该性能域最大利用率的前提下，域内所有CPU的能耗总和)
 */
static inline unsigned long em_cpu_energy(struct em_perf_domain *pd,
				unsigned long max_util, unsigned long sum_util)
{
	unsigned long freq, scale_cpu;
	struct em_perf_state *ps;
	int i, cpu;

	/*
	 * In order to predict the performance state, map the utilization of
	 * the most utilized CPU of the performance domain to a requested
	 * frequency, like schedutil.
	 */
	cpu = cpumask_first(to_cpumask(pd->cpus));
	scale_cpu = arch_scale_cpu_capacity(cpu);
	ps = &pd->table[pd->nr_perf_states - 1];
	freq = map_util_freq(max_util, ps->frequency, scale_cpu);

	/*
	 * Find the lowest performance state of the Energy Model above the
	 * requested frequency.
	 */
	for (i = 0; i < pd->nr_perf_states; i++) {
		ps = &pd->table[i];
		if (ps->frequency >= freq)
			break;
	}

	/*
	 * The capacity of a CPU in the domain at the performance state (ps)
	 * can be computed as:
	 *
	 *             ps->freq * scale_cpu
	 *   ps->cap = --------------------                          (1)
	 *                 cpu_max_freq
	 *
	 * So, ignoring the costs of idle states (which are not available in
	 * the EM), the energy consumed by this CPU at that performance state
	 * is estimated as:
	 *
	 *             ps->power * cpu_util
	 *   cpu_nrg = --------------------                          (2)
	 *                   ps->cap
	 *
	 * since 'cpu_util / ps->cap' represents its percentage of busy time.
	 *
	 *   NOTE: Although the result of this computation actually is in
	 *         units of power, it can be manipulated as an energy value
	 *         over a scheduling period, since it is assumed to be
	 *         constant during that interval.
	 *
	 * By injecting (1) in (2), 'cpu_nrg' can be re-expressed as a product
	 * of two terms:
	 *
	 *             ps->power * cpu_max_freq   cpu_util
	 *   cpu_nrg = ------------------------ * ---------          (3)
	 *                    ps->freq            scale_cpu
	 *
	 * The first term is static, and is stored in the em_perf_state struct
	 * as 'ps->cost'.
	 *
	 * Since all CPUs of the domain have the same micro-architecture, they
	 * share the same 'ps->cost', and the same CPU capacity. Hence, the
	 * total energy of the domain (which is the simple sum of the energy of
	 * all of its CPUs) can be factorized as:
	 *
	 *            ps->cost * \Sum cpu_util
	 *   pd_nrg = ------------------------                       (4)
	 *                  scale_cpu
	 */
	return ps->cost * sum_util / scale_cpu;
}

/**
 * em_pd_nr_perf_states() - Get the number of performance states of a perf.
 *				domain
 * @pd		: performance domain for which this must be done
 *
 * Return: the number of performance states in the performance domain table
 */
static inline int em_pd_nr_perf_states(struct em_perf_domain *pd)
{
	return pd->nr_perf_states;
}

#else
struct em_data_callback {};
#define EM_DATA_CB(_active_power_cb) { }

static inline
int em_dev_register_perf_domain(struct device *dev, unsigned int nr_states,
				struct em_data_callback *cb, cpumask_t *span)
{
	return -EINVAL;
}
static inline void em_dev_unregister_perf_domain(struct device *dev)
{
}
static inline struct em_perf_domain *em_cpu_get(int cpu)
{
	return NULL;
}
static inline struct em_perf_domain *em_pd_get(struct device *dev)
{
	return NULL;
}
static inline unsigned long em_cpu_energy(struct em_perf_domain *pd,
			unsigned long max_util, unsigned long sum_util)
{
	return 0;
}
static inline int em_pd_nr_perf_states(struct em_perf_domain *pd)
{
	return 0;
}
#endif

#endif
