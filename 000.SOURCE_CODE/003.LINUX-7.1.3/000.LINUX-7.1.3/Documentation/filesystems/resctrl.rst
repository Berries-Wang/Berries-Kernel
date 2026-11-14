.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=====================================================
User Interface for Resource Control feature (resctrl)
=====================================================

(资源控制功能 (resctrl) 的用户接口)

:Copyright: |copy| 2016 Intel Corporation
:Authors: - Fenghua Yu <fenghua.yu@intel.com>
          - Tony Luck <tony.luck@intel.com>
          - Vikas Shivappa <vikas.shivappa@intel.com>
(:版权: |copy| 2016 Intel 公司)
(:作者: Fenghua Yu <fenghua.yu@intel.com>、Tony Luck <tony.luck@intel.com>、Vikas Shivappa <vikas.shivappa@intel.com>)


Intel refers to this feature as Intel Resource Director Technology(Intel(R) RDT).
AMD refers to this feature as AMD Platform Quality of Service(AMD QoS).
(英特尔将此特性称为 Intel Resource Director Technology(Intel(R) RDT)。AMD 将此特性称为 AMD Platform Quality of Service(AMD QoS)。)

This feature is enabled by the CONFIG_X86_CPU_RESCTRL and the x86 /proc/cpuinfo
flag bits:
(此特性由 CONFIG_X86_CPU_RESCTRL 以及 x86 /proc/cpuinfo 中的下列标志位启用：)

=============================================================== ================================
RDT (Resource Director Technology) Allocation			"rdt_a" (RDT (资源导向技术) 分配)
CAT (Cache Allocation Technology)				"cat_l3", "cat_l2" (CAT (缓存分配技术))
CDP (Code and Data Prioritization)				"cdp_l3", "cdp_l2" (CDP (代码与数据优先级))
CQM (Cache QoS Monitoring)					"cqm_llc", "cqm_occup_llc" (CQM (缓存 QoS 监控))
MBM (Memory Bandwidth Monitoring)				"cqm_mbm_total", "cqm_mbm_local" (MBM (内存带宽监控))
MBA (Memory Bandwidth Allocation)				"mba" (MBA (内存带宽分配))
SMBA (Slow Memory Bandwidth Allocation)				"" (SMBA (慢速内存带宽分配))
BMEC (Bandwidth Monitoring Event Configuration)			"" (BMEC (带宽监控事件配置))
ABMC (Assignable Bandwidth Monitoring Counters)			"" (ABMC (可分配带宽监控计数器))
SDCIAE (Smart Data Cache Injection Allocation Enforcement)	"" (SDCIAE (智能数据缓存注入分配强制))
=============================================================== ================================

Historically, new features were made visible by default in /proc/cpuinfo. This
resulted in the feature flags becoming hard to parse by humans. Adding a new
flag to /proc/cpuinfo should be avoided if user space can obtain information
about the feature from resctrl's info directory.
(历史上，新特性默认在 /proc/cpuinfo 中可见。这导致特性标志难以被人工解析。如果用户空间可以从 resctrl 的 info 目录获取该特性的信息，就应避免向 /proc/cpuinfo 添加新标志。)

To use the feature mount the file system::
(要使用该特性，先挂载该文件系统：)

 # mount -t resctrl resctrl [-o cdp[,cdpl2][,mba_MBps][,debug]] /sys/fs/resctrl

mount options are:
(挂载选项如下：)

"cdp":
	Enable code/data prioritization in L3 cache allocations.
	(在 L3 缓存分配中启用代码/数据优先级。)
"cdpl2":
	Enable code/data prioritization in L2 cache allocations.
	(在 L2 缓存分配中启用代码/数据优先级。)
"mba_MBps":
	Enable the MBA Software Controller(mba_sc) to specify MBA
	bandwidth in MiBps
	(启用 MBA 软件控制器(mba_sc)，以 MiBps 为单位指定 MBA 带宽。)
"debug":
	Make debug files accessible. Available debug files are annotated with
	"Available only with debug option".
	(使调试文件可访问。可用的调试文件都标注有 "Available only with debug option"。)

L2 and L3 CDP are controlled separately.
(L2 与 L3 的 CDP 分别控制。)

RDT features are orthogonal. A particular system may support only
monitoring, only control, or both monitoring and control.  Cache
pseudo-locking is a unique way of using cache control to "pin" or
"lock" data in the cache. Details can be found in
"Cache Pseudo-Locking".
(RDT 各特性相互正交。特定系统可能仅支持监控、仅支持控制，或同时支持监控与控制。缓存伪锁定是一种独特的缓存控制用法，可将数据"钉住"或"锁定"在缓存中。详情参见 "Cache Pseudo-Locking"。)


The mount succeeds if either of allocation or monitoring is present, but
only those files and directories supported by the system will be created.
For more details on the behavior of the interface during monitoring
and allocation, see the "Resource alloc and monitor groups" section.
(只要分配或监控二者有其一部存在，挂载就会成功，但只会创建系统支持的文件和目录。关于监控与分配期间接口行为的更多细节，参见 "Resource alloc and monitor groups" 一节。)

Info directory
(信息目录)
==============

The 'info' directory contains information about the enabled
resources. Each resource has its own subdirectory. The subdirectory
names reflect the resource names.
('info' 目录包含有关已启用资源的信息。每个资源都有自己的子目录。子目录名称反映资源名称。)

Most of the files in the resource's subdirectory are read-only, and
describe properties of the resource. Resources that support global
configuration options also include writable files that can be used
to modify those settings.
(资源子目录中的大多数文件是只读的，用于描述资源的属性。支持全局配置选项的资源还包含可用于修改这些设置的可写文件。)

Each subdirectory contains the following files with respect to
allocation:
(就分配而言，每个子目录包含以下文件：)

Cache resource(L3/L2)  subdirectory contains the following files
related to allocation:
(缓存资源(L3/L2)子目录包含以下与分配相关的文件：)

"num_closids":
		The number of CLOSIDs which are valid for this
		resource. The kernel uses the smallest number of
		CLOSIDs of all enabled resources as limit.
		(对该资源有效的 CLOSID 数量。内核以所有已启用资源中最小的 CLOSID 数量为上限。)
"cbm_mask":
		The bitmask which is valid for this resource.
		This mask is equivalent to 100%.
		(对该资源有效的位掩码。该掩码等同于 100%。)
"min_cbm_bits":
		The minimum number of consecutive bits which
		must be set when writing a mask.
		(写入掩码时必须置位的最小连续位数。)

"shareable_bits":
		Bitmask of shareable resource with other executing entities
		(e.g. I/O). Applies to all instances of this resource. User
		can use this when setting up exclusive cache partitions.
		Note that some platforms support devices that have their
		own settings for cache use which can over-ride these bits.
		(与其他执行实体(例如 I/O)可共享资源的位掩码。适用于该资源的所有实例。用户在设置独占缓存分区时可以使用它。注意，某些平台支持的设备对缓存使用有自己的设置，可以覆盖这些位。)

		When "io_alloc" is enabled, a portion of each cache instance can
		be configured for shared use between hardware and software.
		"bit_usage" should be used to see which portions of each cache
		instance is configured for hardware use via "io_alloc" feature
		because every cache instance can have its "io_alloc" bitmask
		configured independently via "io_alloc_cbm".
		(当 "io_alloc" 启用时，每个缓存实例的一部分可以配置为硬件与软件共享使用。应使用 "bit_usage" 查看每个缓存实例的哪些部分通过 "io_alloc" 特性配置为硬件使用，因为每个缓存实例都可以通过 "io_alloc_cbm" 独立配置其 "io_alloc" 位掩码。)

"bit_usage":
		Annotated capacity bitmasks showing how all
		instances of the resource are used. The legend is:
		(带注释的容量位掩码，显示该资源所有实例的使用情况。图例如下：)

			"0":
			      Corresponding region is unused. When the system's
			      resources have been allocated and a "0" is found
			      in "bit_usage" it is a sign that resources are
			      wasted.
			      (对应区域未被使用。当系统资源已被分配而在 "bit_usage" 中发现 "0" 时，说明资源被浪费了。)

			"H":
			      Corresponding region is used by hardware only
			      but available for software use. If a resource
			      has bits set in "shareable_bits" or "io_alloc_cbm"
			      but not all of these bits appear in the resource
			      groups' schemata then the bits appearing in
			      "shareable_bits" or "io_alloc_cbm" but no
			      resource group will be marked as "H".
			      (对应区域仅由硬件使用，但可供软件使用。如果某资源在 "shareable_bits" 或 "io_alloc_cbm" 中有置位的位，但这些位并未全部出现在资源组的方案中，那么出现在 "shareable_bits" 或 "io_alloc_cbm" 中却没有任何资源组使用的位将被标记为 "H"。)
			"X":
			      Corresponding region is available for sharing and
			      used by hardware and software. These are the bits
			      that appear in "shareable_bits" or "io_alloc_cbm"
			      as well as a resource group's allocation.
			      (对应区域可供共享，由硬件和软件共同使用。这些是既出现在 "shareable_bits" 或 "io_alloc_cbm" 中、也出现在某个资源组分配中的位。)
			"S":
			      Corresponding region is used by software
			      and available for sharing.
			      (对应区域由软件使用，并可供共享。)
			"E":
			      Corresponding region is used exclusively by
			      one resource group. No sharing allowed.
			      (对应区域由一个资源组独占使用。不允许共享。)
			"P":
			      Corresponding region is pseudo-locked. No
			      sharing allowed.
			      (对应区域为伪锁定。不允许共享。)
"sparse_masks":
		Indicates if non-contiguous 1s value in CBM is supported.
		(指示 CBM 中是否支持非连续的 1 值。)

			"0":
			      Only contiguous 1s value in CBM is supported.
			      (仅支持 CBM 中连续的 1 值。)
			"1":
			      Non-contiguous 1s value in CBM is supported.
			      (支持 CBM 中非连续的 1 值。)

"io_alloc":
		"io_alloc" enables system software to configure the portion of
		the cache allocated for I/O traffic. File may only exist if the
		system supports this feature on some of its cache resources.
		("io_alloc" 使系统软件能够配置分配给 I/O 流量的那部分缓存。仅当系统在其部分缓存资源上支持该特性时，文件才可能存在。)

			"disabled":
			      Resource supports "io_alloc" but the feature is disabled.
			      Portions of cache used for allocation of I/O traffic cannot
			      be configured.
			      (资源支持 "io_alloc" 但该特性被禁用。用于分配 I/O 流量的缓存部分无法配置。)
			"enabled":
			      Portions of cache used for allocation of I/O traffic
			      can be configured using "io_alloc_cbm".
			      (用于分配 I/O 流量的缓存部分可以通过 "io_alloc_cbm" 进行配置。)
			"not supported":
			      Support not available for this resource.
			      (该资源不支持此功能。)

		The feature can be modified by writing to the interface, for example:
		(可以通过写入该接口来修改此特性，例如：)

		To enable::
		(启用：)

			# echo 1 > /sys/fs/resctrl/info/L3/io_alloc

		To disable::
		(禁用：)

			# echo 0 > /sys/fs/resctrl/info/L3/io_alloc

		The underlying implementation may reduce resources available to
		general (CPU) cache allocation. See architecture specific notes
		below. Depending on usage requirements the feature can be enabled
		or disabled.
		(底层实现可能减少可用于通用(CPU)缓存分配的资源。参见下文的架构特定说明。根据使用需求，该特性可以启用或禁用。)

		On AMD systems, io_alloc feature is supported by the L3 Smart
		Data Cache Injection Allocation Enforcement (SDCIAE). The CLOSID for
		io_alloc is the highest CLOSID supported by the resource. When
		io_alloc is enabled, the highest CLOSID is dedicated to io_alloc and
		no longer available for general (CPU) cache allocation. When CDP is
		enabled, io_alloc routes I/O traffic using the highest CLOSID allocated
		for the instruction cache (CDP_CODE), making this CLOSID no longer
		available for general (CPU) cache allocation for both the CDP_CODE
		and CDP_DATA resources.
		(在 AMD 系统上，io_alloc 特性由 L3 智能数据缓存注入分配强制(SDCIAE)支持。io_alloc 的 CLOSID 是该资源支持的最高 CLOSID。当 io_alloc 启用时，最高的 CLOSID 专用于 io_alloc，不再可用于通用(CPU)缓存分配。当启用 CDP 时，io_alloc 使用分配给指令缓存(CDP_CODE)的最高 CLOSID 来路由 I/O 流量，使该 CLOSID 对 CDP_CODE 和 CDP_DATA 两种资源都不再可用于通用(CPU)缓存分配。)

"io_alloc_cbm":
		Capacity bitmasks that describe the portions of cache instances to
		which I/O traffic from supported I/O devices are routed when "io_alloc"
		is enabled.
		(容量位掩码，描述当 "io_alloc" 启用时，来自受支持 I/O 设备的 I/O 流量被路由到的缓存实例部分。)

		CBMs are displayed in the following format:
		(CBM 以下列格式显示：)

			<cache_id0>=<cbm>;<cache_id1>=<cbm>;...

		Example::
		(示例：)

			# cat /sys/fs/resctrl/info/L3/io_alloc_cbm
			0=ffff;1=ffff

		CBMs can be configured by writing to the interface.
		(CBM 可以通过写入该接口进行配置。)

		Example::
		(示例：)

			# echo 1=ff > /sys/fs/resctrl/info/L3/io_alloc_cbm
			# cat /sys/fs/resctrl/info/L3/io_alloc_cbm
			0=ffff;1=00ff

			# echo "0=ff;1=f" > /sys/fs/resctrl/info/L3/io_alloc_cbm
			# cat /sys/fs/resctrl/info/L3/io_alloc_cbm
			0=00ff;1=000f

		An ID of "*" configures all domains with the provided CBM.
		(ID 为 "*" 时，用提供的 CBM 配置所有域。)

		Example on a system that does not require a minimum number of consecutive bits in the mask::
		(在不要求掩码具有最小连续位数的系统上的示例：)

			# echo "*=0" > /sys/fs/resctrl/info/L3/io_alloc_cbm
			# cat /sys/fs/resctrl/info/L3/io_alloc_cbm
			0=0;1=0

		When CDP is enabled "io_alloc_cbm" associated with the CDP_DATA and CDP_CODE
		resources may reflect the same values. For example, values read from and
		written to /sys/fs/resctrl/info/L3DATA/io_alloc_cbm may be reflected by
		/sys/fs/resctrl/info/L3CODE/io_alloc_cbm and vice versa.
		(启用 CDP 时，与 CDP_DATA 和 CDP_CODE 资源关联的 "io_alloc_cbm" 可能反映相同的值。例如，从 /sys/fs/resctrl/info/L3DATA/io_alloc_cbm 读出和写入的值可能由 /sys/fs/resctrl/info/L3CODE/io_alloc_cbm 反映，反之亦然。)

Memory bandwidth(MB) subdirectory contains the following files
with respect to allocation:
(内存带宽(MB)子目录包含以下与分配相关的文件：)

"min_bandwidth":
		The minimum memory bandwidth percentage which
		user can request.
		(用户可以请求的最小内存带宽百分比。)

"bandwidth_gran":
		The granularity in which the memory bandwidth
		percentage is allocated. The allocated
		b/w percentage is rounded off to the next
		control step available on the hardware. The
		available bandwidth control steps are:
		min_bandwidth + N * bandwidth_gran.
		(内存带宽百分比分配的粒度。分配的带宽百分比会舍入到硬件上可用的下一个控制步。可用的带宽控制步为：min_bandwidth + N * bandwidth_gran。)

"delay_linear":
		Indicates if the delay scale is linear or
		non-linear. This field is purely informational
		only.
		(指示延迟比例是线性还是非线性。该字段仅供参考。)

"thread_throttle_mode":
		Indicator on Intel systems of how tasks running on threads
		of a physical core are throttled in cases where they
		request different memory bandwidth percentages:
		(在 Intel 系统上指示：当物理核心的线程请求不同的内存带宽百分比时，运行在这些线程上的任务如何被节流：)

		"max":
			the smallest percentage is applied
			to all threads
			(将最小百分比应用到所有线程)
		"per-thread":
			bandwidth percentages are directly applied to
			the threads running on the core
			(带宽百分比直接应用到核心上运行的线程)

If L3 monitoring is available there will be an "L3_MON" directory
with the following files:
(如果 L3 监控可用，将有一个 "L3_MON" 目录，包含以下文件：)

"num_rmids":
		The number of RMIDs supported by hardware for
		L3 monitoring events.
		(硬件为 L3 监控事件支持的 RMID 数量。)

"mon_features":
		Lists the monitoring events if
		monitoring is enabled for the resource.
		Example::
		(列出该资源启用监控时的监控事件。示例：)

			# cat /sys/fs/resctrl/info/L3_MON/mon_features
			llc_occupancy
			mbm_total_bytes
			mbm_local_bytes

		If the system supports Bandwidth Monitoring Event
		Configuration (BMEC), then the bandwidth events will
		be configurable. The output will be::
		(如果系统支持带宽监控事件配置(BMEC)，则带宽事件将可配置。输出如下：)

			# cat /sys/fs/resctrl/info/L3_MON/mon_features
			llc_occupancy
			mbm_total_bytes
			mbm_total_bytes_config
			mbm_local_bytes
			mbm_local_bytes_config

"mbm_total_bytes_config", "mbm_local_bytes_config":
	Read/write files containing the configuration for the mbm_total_bytes
	and mbm_local_bytes events, respectively, when the Bandwidth
	Monitoring Event Configuration (BMEC) feature is supported.
	The event configuration settings are domain specific and affect
	all the CPUs in the domain. When either event configuration is
	changed, the bandwidth counters for all RMIDs of both events
	(mbm_total_bytes as well as mbm_local_bytes) are cleared for that
	domain. The next read for every RMID will report "Unavailable"
	and subsequent reads will report the valid value.
	(读/写文件，分别在支持带宽监控事件配置(BMEC)特性时包含 mbm_total_bytes 和 mbm_local_bytes 事件的配置。事件配置设置是域特定的，影响域中的所有 CPU。当任一事件配置更改时，该域中两个事件(mbm_total_bytes 和 mbm_local_bytes)的所有 RMID 的带宽计数器都会被清除。每个 RMID 的下一次读取将报告 "Unavailable"，随后的读取将报告有效值。)

	Following are the types of events supported:
	(以下是支持的事件类型：)

	====    ========================================================
	Bits    Description
	====    ========================================================
	6       Dirty Victims from the QOS domain to all types of memory (从 QOS 域到所有类型内存的脏牺牲者)
	5       Reads to slow memory in the non-local NUMA domain (对非本地 NUMA 域慢速内存的读取)
	4       Reads to slow memory in the local NUMA domain (对本地 NUMA 域慢速内存的读取)
	3       Non-temporal writes to non-local NUMA domain (对非本地 NUMA 域的非临时写入)
	2       Non-temporal writes to local NUMA domain (对本地 NUMA 域的非临时写入)
	1       Reads to memory in the non-local NUMA domain (对非本地 NUMA 域内存的读取)
	0       Reads to memory in the local NUMA domain (对本地 NUMA 域内存的读取)
	====    ========================================================

	By default, the mbm_total_bytes configuration is set to 0x7f to count
	all the event types and the mbm_local_bytes configuration is set to
	0x15 to count all the local memory events.
	(默认情况下，mbm_total_bytes 配置设为 0x7f 以统计所有事件类型，mbm_local_bytes 配置设为 0x15 以统计所有本地内存事件。)

	Examples:
	(示例：)

	* To view the current configuration::
	  (查看当前配置：)
	  ::

	    # cat /sys/fs/resctrl/info/L3_MON/mbm_total_bytes_config
	    0=0x7f;1=0x7f;2=0x7f;3=0x7f

	    # cat /sys/fs/resctrl/info/L3_MON/mbm_local_bytes_config
	    0=0x15;1=0x15;3=0x15;4=0x15

	* To change the mbm_total_bytes to count only reads on domain 0,
	  the bits 0, 1, 4 and 5 needs to be set, which is 110011b in binary
	  (in hexadecimal 0x33):
	  (要将 mbm_total_bytes 更改为仅统计域 0 上的读取，需要置位 0、1、4 和 5，即二进制 110011b(十六进制 0x33)：)
	  ::

	    # echo  "0=0x33" > /sys/fs/resctrl/info/L3_MON/mbm_total_bytes_config

	    # cat /sys/fs/resctrl/info/L3_MON/mbm_total_bytes_config
	    0=0x33;1=0x7f;2=0x7f;3=0x7f

	* To change the mbm_local_bytes to count all the slow memory reads on
	  domain 0 and 1, the bits 4 and 5 needs to be set, which is 110000b
	  in binary (in hexadecimal 0x30):
	  (要将 mbm_local_bytes 更改为统计域 0 和 1 上的所有慢速内存读取，需要置位 4 和 5，即二进制 110000b(十六进制 0x30)：)
	  ::

	    # echo  "0=0x30;1=0x30" > /sys/fs/resctrl/info/L3_MON/mbm_local_bytes_config

	    # cat /sys/fs/resctrl/info/L3_MON/mbm_local_bytes_config
	    0=0x30;1=0x30;3=0x15;4=0x15

"mbm_assign_mode":
	The supported counter assignment modes. The enclosed brackets indicate which mode
	is enabled. The MBM events associated with counters may reset when "mbm_assign_mode"
	is changed.
	(支持的计数器分配模式。括号指示当前启用的模式。当 "mbm_assign_mode" 更改时，与计数器关联的 MBM 事件可能重置。)
	::

	  # cat /sys/fs/resctrl/info/L3_MON/mbm_assign_mode
	  [mbm_event]
	  default

	"mbm_event":

	mbm_event mode allows users to assign a hardware counter to an RMID, event
	pair and monitor the bandwidth usage as long as it is assigned. The hardware
	continues to track the assigned counter until it is explicitly unassigned by
	the user. Each event within a resctrl group can be assigned independently.
	(mbm_event 模式允许用户将硬件计数器分配给 RMID 与事件对，并在分配期间监控带宽使用。硬件会持续跟踪已分配的计数器，直到用户显式取消分配。resctrl 组内的每个事件都可以独立分配。)

	In this mode, a monitoring event can only accumulate data while it is backed
	by a hardware counter. Use "mbm_L3_assignments" found in each CTRL_MON and MON
	group to specify which of the events should have a counter assigned. The number
	of counters available is described in the "num_mbm_cntrs" file. Changing the
	mode may cause all counters on the resource to reset.
	(在此模式下，监控事件只有在有硬件计数器支持时才能累积数据。使用每个 CTRL_MON 组和 MON 组中的 "mbm_L3_assignments" 来指定哪些事件应分配计数器。可用计数器的数量在 "num_mbm_cntrs" 文件中描述。更改模式可能导致资源上的所有计数器重置。)

	Moving to mbm_event counter assignment mode requires users to assign the counters
	to the events. Otherwise, the MBM event counters will return 'Unassigned' when read.
	(切换到 mbm_event 计数器分配模式需要用户为事件分配计数器。否则，MBM 事件计数器在读取时将返回 'Unassigned'。)

	The mode is beneficial for AMD platforms that support more CTRL_MON
	and MON groups than available hardware counters. By default, this
	feature is enabled on AMD platforms with the ABMC (Assignable Bandwidth
	Monitoring Counters) capability, ensuring counters remain assigned even
	when the corresponding RMID is not actively used by any processor.
	(该模式对支持的 CTRL_MON 和 MON 组数量多于可用硬件计数器的 AMD 平台有益。默认情况下，在具有 ABMC(可分配带宽监控计数器)能力的 AMD 平台上该特性是启用的，即使相应的 RMID 未被任何处理器主动使用，计数器也保持分配状态。)

	"default":

	In default mode, resctrl assumes there is a hardware counter for each
	event within every CTRL_MON and MON group. On AMD platforms, it is
	recommended to use the mbm_event mode, if supported, to prevent reset of MBM
	events between reads resulting from hardware re-allocating counters. This can
	result in misleading values or display "Unavailable" if no counter is assigned
	to the event.
	(在 default 模式下，resctrl 假定每个 CTRL_MON 和 MON 组内的每个事件都有一个硬件计数器。在 AMD 平台上，如果支持，建议使用 mbm_event 模式，以防止因硬件重新分配计数器导致读取之间 MBM 事件被重置。如果没有为事件分配计数器，这可能导致误导性的值或显示 "Unavailable"。)

	* To enable "mbm_event" counter assignment mode:
	  (启用 "mbm_event" 计数器分配模式：)
	  ::

	    # echo "mbm_event" > /sys/fs/resctrl/info/L3_MON/mbm_assign_mode

	* To enable "default" monitoring mode:
	  (启用 "default" 监控模式：)
	  ::

	    # echo "default" > /sys/fs/resctrl/info/L3_MON/mbm_assign_mode

"num_mbm_cntrs":
	The maximum number of counters (total of available and assigned counters) in
	each domain when the system supports mbm_event mode.
	(当系统支持 mbm_event 模式时，每个域中计数器的最大数量(可用计数器与已分配计数器之和)。)

	For example, on a system with maximum of 32 memory bandwidth monitoring
	counters in each of its L3 domains:
	(例如，在一个每个 L3 域最多有 32 个内存带宽监控计数器的系统上：)
	::

	  # cat /sys/fs/resctrl/info/L3_MON/num_mbm_cntrs
	  0=32;1=32

"available_mbm_cntrs":
	The number of counters available for assignment in each domain when mbm_event
	mode is enabled on the system.
	(当系统启用 mbm_event 模式时，每个域中可供分配的计数器数量。)

	For example, on a system with 30 available [hardware] assignable counters
	in each of its L3 domains:
	(例如，在一个每个 L3 域有 30 个可用[硬件]可分配计数器的系统上：)
	::

	  # cat /sys/fs/resctrl/info/L3_MON/available_mbm_cntrs
	  0=30;1=30

"event_configs":
	Directory that exists when "mbm_event" counter assignment mode is supported.
	Contains a sub-directory for each MBM event that can be assigned to a counter.
	(当支持 "mbm_event" 计数器分配模式时存在的目录。包含每个可分配给计数器的 MBM 事件的子目录。)

	Two MBM events are supported by default: mbm_local_bytes and mbm_total_bytes.
	Each MBM event's sub-directory contains a file named "event_filter" that is
	used to view and modify which memory transactions the MBM event is configured
	with. The file is accessible only when "mbm_event" counter assignment mode is
	enabled.
	(默认支持两个 MBM 事件：mbm_local_bytes 和 mbm_total_bytes。每个 MBM 事件的子目录包含一个名为 "event_filter" 的文件，用于查看和修改 MBM 事件配置为统计哪些内存事务。仅当启用 "mbm_event" 计数器分配模式时才可访问该文件。)

	List of memory transaction types supported:
	(支持的内存事务类型列表：)

	==========================  ========================================================
	Name			    Description
	==========================  ========================================================
	dirty_victim_writes_all     Dirty Victims from the QOS domain to all types of memory (从 QOS 域到所有类型内存的脏牺牲者)
	remote_reads_slow_memory    Reads to slow memory in the non-local NUMA domain (对非本地 NUMA 域慢速内存的读取)
	local_reads_slow_memory     Reads to slow memory in the local NUMA domain (对本地 NUMA 域慢速内存的读取)
	remote_non_temporal_writes  Non-temporal writes to non-local NUMA domain (对非本地 NUMA 域的非临时写入)
	local_non_temporal_writes   Non-temporal writes to local NUMA domain (对本地 NUMA 域的非临时写入)
	remote_reads                Reads to memory in the non-local NUMA domain (对非本地 NUMA 域内存的读取)
	local_reads                 Reads to memory in the local NUMA domain (对本地 NUMA 域内存的读取)
	==========================  ========================================================

	For example::
	(例如：)

	  # cat /sys/fs/resctrl/info/L3_MON/event_configs/mbm_total_bytes/event_filter
	  local_reads,remote_reads,local_non_temporal_writes,remote_non_temporal_writes,
	  local_reads_slow_memory,remote_reads_slow_memory,dirty_victim_writes_all

	  # cat /sys/fs/resctrl/info/L3_MON/event_configs/mbm_local_bytes/event_filter
	  local_reads,local_non_temporal_writes,local_reads_slow_memory

	Modify the event configuration by writing to the "event_filter" file within
	the "event_configs" directory. The read/write "event_filter" file contains the
	configuration of the event that reflects which memory transactions are counted by it.
	(通过写入 "event_configs" 目录中的 "event_filter" 文件来修改事件配置。读/写的 "event_filter" 文件包含该事件的配置，反映它统计哪些内存事务。)

	For example::
	(例如：)

	  # echo "local_reads, local_non_temporal_writes" >
	    /sys/fs/resctrl/info/L3_MON/event_configs/mbm_total_bytes/event_filter

	  # cat /sys/fs/resctrl/info/L3_MON/event_configs/mbm_total_bytes/event_filter
	   local_reads,local_non_temporal_writes

"mbm_assign_on_mkdir":
	Exists when "mbm_event" counter assignment mode is supported. Accessible
	only when "mbm_event" counter assignment mode is enabled.
	(当支持 "mbm_event" 计数器分配模式时存在。仅当启用 "mbm_event" 计数器分配模式时可访问。)

	Determines if a counter will automatically be assigned to an RMID, MBM event
	pair when its associated monitor group is created via mkdir. Enabled by default
	on boot, also when switched from "default" mode to "mbm_event" counter assignment
	mode. Users can disable this capability by writing to the interface.
	(决定当通过 mkdir 创建关联的监控组时，是否自动为 RMID 与 MBM 事件对分配计数器。系统启动时默认启用，从 "default" 模式切换到 "mbm_event" 计数器分配模式时也是如此。用户可以通过写入该接口禁用此功能。)

	"0":
		Auto assignment is disabled.
		(自动分配已禁用。)
	"1":
		Auto assignment is enabled.
		(自动分配已启用。)

	Example::
	(示例：)

	  # echo 0 > /sys/fs/resctrl/info/L3_MON/mbm_assign_on_mkdir
	  # cat /sys/fs/resctrl/info/L3_MON/mbm_assign_on_mkdir
	  0

"max_threshold_occupancy":
		Read/write file provides the largest value (in
		bytes) at which a previously used LLC_occupancy
		counter can be considered for reuse.
		(读/写文件，提供一个最大值(以字节为单位)，当占用低于该值时，先前使用的 LLC_occupancy 计数器可被考虑重用。)

If telemetry monitoring is available there will be a "PERF_PKG_MON" directory
with the following files:
(如果遥测监控可用，将有一个 "PERF_PKG_MON" 目录，包含以下文件：)

"num_rmids":
		The number of RMIDs for telemetry monitoring events.
		(用于遥测监控事件的 RMID 数量。)

		On Intel resctrl will not enable telemetry events if the number of
		RMIDs that can be tracked concurrently is lower than the total number
		of RMIDs supported. Telemetry events can be force-enabled with the
		"rdt=" kernel parameter, but this may reduce the number of
		monitoring groups that can be created.
		(在 Intel 上，如果可并发跟踪的 RMID 数量低于支持的 RMID 总数，resctrl 将不启用遥测事件。可以使用 "rdt=" 内核参数强制启用遥测事件，但这可能会减少可创建的监控组数量。)

"mon_features":
		Lists the telemetry monitoring events that are enabled on this system.
		(列出此系统上启用的遥测监控事件。)

The upper bound for how many "CTRL_MON" + "MON" can be created
is the smaller of the L3_MON and PERF_PKG_MON "num_rmids" values.
(可创建的 "CTRL_MON" + "MON" 数量上限是 L3_MON 与 PERF_PKG_MON 的 "num_rmids" 值中较小的一个。)

Finally, in the top level of the "info" directory there is a file
named "last_cmd_status". This is reset with every "command" issued
via the file system (making new directories or writing to any of the
control files). If the command was successful, it will read as "ok".
If the command failed, it will provide more information that can be
conveyed in the error returns from file operations. E.g.
(最后，在 "info" 目录的顶层有一个名为 "last_cmd_status" 的文件。每次通过文件系统发出"命令"(创建新目录或写入任何控制文件)时它都会重置。如果命令成功，它将显示为 "ok"。如果命令失败，它会提供比文件操作错误返回所能表达的更多信息。例如：)
::

	# echo L3:0=f7 > schemata
	bash: echo: write error: Invalid argument
	# cat info/last_cmd_status
	mask f7 has non-consecutive 1-bits

Resource alloc and monitor groups
(资源分配与监控组)
=================================

Resource groups are represented as directories in the resctrl file
system.  The default group is the root directory which, immediately
after mounting, owns all the tasks and cpus in the system and can make
full use of all resources.
(资源组表示为 resctrl 文件系统中的目录。默认组是根目录，挂载后它立即拥有系统中的所有任务和 CPU，并可以充分利用所有资源。)

On a system with RDT control features additional directories can be
created in the root directory that specify different amounts of each
resource (see "schemata" below). The root and these additional top level
directories are referred to as "CTRL_MON" groups below.
(在具有 RDT 控制特性的系统上，可以在根目录中创建额外目录，为每种资源指定不同的数量(见下文 "schemata")。根目录和这些额外的顶层目录在下文中称为 "CTRL_MON" 组。)

On a system with RDT monitoring the root directory and other top level
directories contain a directory named "mon_groups" in which additional
directories can be created to monitor subsets of tasks in the CTRL_MON
group that is their ancestor. These are called "MON" groups in the rest
of this document.
(在具有 RDT 监控的系统上，根目录和其他顶层目录包含一个名为 "mon_groups" 的目录，可以在其中创建额外目录，用于监控其祖先 CTRL_MON 组中任务的子集。本文档其余部分将这些目录称为 "MON" 组。)

Removing a directory will move all tasks and cpus owned by the group it
represents to the parent. Removing one of the created CTRL_MON groups
will automatically remove all MON groups below it.
(删除目录会将其所代表的组拥有的所有任务和 CPU 移到父组。删除已创建的 CTRL_MON 组之一将自动删除其下所有 MON 组。)

Moving MON group directories to a new parent CTRL_MON group is supported
for the purpose of changing the resource allocations of a MON group
without impacting its monitoring data or assigned tasks. This operation
is not allowed for MON groups which monitor CPUs. No other move
operation is currently allowed other than simply renaming a CTRL_MON or
MON group.
(支持将 MON 组目录移动到新的父 CTRL_MON 组，目的是在不影响其监控数据或已分配任务的情况下更改 MON 组的资源分配。对于监控 CPU 的 MON 组，不允许此操作。目前除简单地重命名 CTRL_MON 或 MON 组外，不允许其他移动操作。)

All groups contain the following files:
(所有组都包含以下文件：)

"tasks":
	Reading this file shows the list of all tasks that belong to
	this group. Writing a task id to the file will add a task to the
	group. Multiple tasks can be added by separating the task ids
	with commas. Tasks will be assigned sequentially. Multiple
	failures are not supported. A single failure encountered while
	attempting to assign a task will cause the operation to abort and
	already added tasks before the failure will remain in the group.
	Failures will be logged to /sys/fs/resctrl/info/last_cmd_status.
	(读取此文件显示属于该组的所有任务的列表。向该文件写入任务 ID 会将任务添加到组中。可以通过用逗号分隔任务 ID 来添加多个任务。任务将被按顺序分配。不支持多次失败。尝试分配任务时遇到单个失败将导致操作中止，失败前已添加的任务将保留在组中。失败将记录到 /sys/fs/resctrl/info/last_cmd_status。)

	If the group is a CTRL_MON group the task is removed from
	whichever previous CTRL_MON group owned the task and also from
	any MON group that owned the task. If the group is a MON group,
	then the task must already belong to the CTRL_MON parent of this
	group. The task is removed from any previous MON group.
	(如果该组是 CTRL_MON 组，任务将从先前拥有该任务的 CTRL_MON 组以及拥有该任务的任何 MON 组中移除。如果该组是 MON 组，则任务必须已属于该组的 CTRL_MON 父组。任务将从任何先前的 MON 组中移除。)


"cpus":
	Reading this file shows a bitmask of the logical CPUs owned by
	this group. Writing a mask to this file will add and remove
	CPUs to/from this group. As with the tasks file a hierarchy is
	maintained where MON groups may only include CPUs owned by the
	parent CTRL_MON group.
	When the resource group is in pseudo-locked mode this file will
	only be readable, reflecting the CPUs associated with the
	pseudo-locked region.
	(读取此文件显示该组拥有的逻辑 CPU 的位掩码。向此文件写入掩码将在该组中添加和移除 CPU。与 tasks 文件一样维护着一个层次结构，MON 组只能包含其父 CTRL_MON 组拥有的 CPU。当资源组处于伪锁定模式时，此文件将只可读，反映与伪锁定区域关联的 CPU。)


"cpus_list":
	Just like "cpus", only using ranges of CPUs instead of bitmasks.
	(与 "cpus" 类似，只是使用 CPU 范围而不是位掩码。)


When control is enabled all CTRL_MON groups will also contain:
(启用控制时，所有 CTRL_MON 组还将包含：)

"schemata":
	A list of all the resources available to this group.
	Each resource has its own line and format - see below for details.
	(此组可用的所有资源的列表。每个资源有自己的行和格式——详情见下文。)

"size":
	Mirrors the display of the "schemata" file to display the size in
	bytes of each allocation instead of the bits representing the
	allocation.
	(镜像 "schemata" 文件的显示，以字节为单位显示每个分配的大小，而不是表示分配的位。)

"mode":
	The "mode" of the resource group dictates the sharing of its
	allocations. A "shareable" resource group allows sharing of its
	allocations while an "exclusive" resource group does not. A
	cache pseudo-locked region is created by first writing
	"pseudo-locksetup" to the "mode" file before writing the cache
	pseudo-locked region's schemata to the resource group's "schemata"
	file. On successful pseudo-locked region creation the mode will
	automatically change to "pseudo-locked".
	(资源组的 "mode" 决定其分配的共享方式。"shareable" 资源组允许共享其分配，而 "exclusive" 资源组不允许。缓存伪锁定区域的创建方式是：先将 "pseudo-locksetup" 写入 "mode" 文件，再将缓存伪锁定区域的方案写入该资源组的 "schemata" 文件。伪锁定区域创建成功后，模式将自动更改为 "pseudo-locked"。)

"ctrl_hw_id":
	Available only with debug option. The identifier used by hardware
	for the control group. On x86 this is the CLOSID.
	(仅在 debug 选项下可用。硬件用于控制组的标识符。在 x86 上这是 CLOSID。)

When monitoring is enabled all MON groups will also contain:
(启用监控时，所有 MON 组还将包含：)

"mon_data":
	This contains directories for each monitor domain.
	(其中包含每个监控域的目录。)

	If L3 monitoring is enabled, there will be a "mon_L3_XX" directory for
	each instance of an L3 cache. Each directory contains files for the enabled
	L3 events (e.g. "llc_occupancy", "mbm_total_bytes", and "mbm_local_bytes").
	(如果启用了 L3 监控，每个 L3 缓存实例都会有一个 "mon_L3_XX" 目录。每个目录包含已启用 L3 事件的文件(例如 "llc_occupancy"、"mbm_total_bytes" 和 "mbm_local_bytes")。)

	If telemetry monitoring is enabled, there will be a "mon_PERF_PKG_YY"
	directory for each physical processor package. Each directory contains
	files for the enabled telemetry events (e.g. "core_energy". "activity",
	"uops_retired", etc.)
	(如果启用了遥测监控，每个物理处理器封装都会有一个 "mon_PERF_PKG_YY" 目录。每个目录包含已启用遥测事件的文件(例如 "core_energy"、"activity"、"uops_retired" 等)。)

	The info/`*`/mon_features files provide the full list of enabled
	event/file names.
	(info/`*`/mon_features 文件提供已启用事件/文件名的完整列表。)

	"core energy" reports a floating point number for the energy (in Joules)
	consumed by cores (registers, arithmetic units, TLB and L1/L2 caches)
	during execution of instructions summed across all logical CPUs on a
	package for the current monitoring group.
	("core energy" 报告一个浮点数，表示当前监控组的封装上所有逻辑 CPU 在执行指令期间由核心(寄存器、运算单元、TLB 和 L1/L2 缓存)消耗的能量(以焦耳为单位)的总和。)

	"activity" also reports a floating point value (in Farads).  This provides
	an estimate of work done independent of the frequency that the CPUs used
	for execution.
	("activity" 也报告一个浮点值(以法拉为单位)。它提供与 CPU 执行所用频率无关的工作量估计。)

	Note that "core energy" and "activity" only measure energy/activity in the
	"core" of the CPU (arithmetic units, TLB, L1 and L2 caches, etc.). They
	do not include L3 cache, memory, I/O devices etc.
	(注意，"core energy" 和 "activity" 仅测量 CPU "核心"(运算单元、TLB、L1 和 L2 缓存等)中的能量/活动。它们不包括 L3 缓存、内存、I/O 设备等。)

	All other events report decimal integer values.
	(所有其他事件报告十进制整数值。)

	In a MON group these files provide a read out of the current value of
	the event for all tasks in the group. In CTRL_MON groups these files
	provide the sum for all tasks in the CTRL_MON group and all tasks in
	MON groups. Please see example section for more details on usage.
	(在 MON 组中，这些文件提供该组中所有任务的当前事件值读数。在 CTRL_MON 组中，这些文件提供 CTRL_MON 组中所有任务以及 MON 组中所有任务的总和。用法详情请参见示例一节。)

	On systems with Sub-NUMA Cluster (SNC) enabled there are extra
	directories for each node (located within the "mon_L3_XX" directory
	for the L3 cache they occupy). These are named "mon_sub_L3_YY"
	where "YY" is the node number.
	(在启用了子 NUMA 集群(SNC)的系统上，每个节点都有额外的目录(位于它们所占用的 L3 缓存的 "mon_L3_XX" 目录中)。这些目录命名为 "mon_sub_L3_YY"，其中 "YY" 是节点号。)

	When the 'mbm_event' counter assignment mode is enabled, reading
	an MBM event of a MON group returns 'Unassigned' if no hardware
	counter is assigned to it. For CTRL_MON groups, 'Unassigned' is
	returned if the MBM event does not have an assigned counter in the
	CTRL_MON group nor in any of its associated MON groups.
	(当启用 'mbm_event' 计数器分配模式时，读取 MON 组的 MBM 事件时，如果没有为其分配硬件计数器，将返回 'Unassigned'。对于 CTRL_MON 组，如果该 MBM 事件在 CTRL_MON 组及其任何关联的 MON 组中都没有已分配的计数器，则返回 'Unassigned'。)

"mon_hw_id":
	Available only with debug option. The identifier used by hardware
	for the monitor group. On x86 this is the RMID.
	(仅在 debug 选项下可用。硬件用于监控组的标识符。在 x86 上这是 RMID。)

When monitoring is enabled all MON groups may also contain:
(启用监控时，所有 MON 组还可能包含：)

"mbm_L3_assignments":
	Exists when "mbm_event" counter assignment mode is supported and lists the
	counter assignment states of the group.
	(当支持 "mbm_event" 计数器分配模式时存在，列出该组的计数器分配状态。)

	The assignment list is displayed in the following format:

	<Event>:<Domain ID>=<Assignment state>;<Domain ID>=<Assignment state>
	(分配列表以下列格式显示：)

	Event: A valid MBM event in the
	       /sys/fs/resctrl/info/L3_MON/event_configs directory.
	(Event: /sys/fs/resctrl/info/L3_MON/event_configs 目录中的有效 MBM 事件。)

	Domain ID: A valid domain ID. When writing, '*' applies the changes
		   to all the domains.
	(Domain ID: 有效的域 ID。写入时，'*' 将更改应用到所有域。)

	Assignment states:
	(分配状态：)

	_ : No counter assigned.
	(_ : 未分配计数器。)

	e : Counter assigned exclusively.
	(e : 计数器被独占分配。)

	Example:
	(示例：)

	To display the counter assignment states for the default group.
	(显示默认组的计数器分配状态。)
	::

	 # cd /sys/fs/resctrl
	 # cat /sys/fs/resctrl/mbm_L3_assignments
	   mbm_total_bytes:0=e;1=e
	   mbm_local_bytes:0=e;1=e

	Assignments can be modified by writing to the interface.
	(可以通过写入该接口修改分配。)

	Examples:
	(示例：)

	To unassign the counter associated with the mbm_total_bytes event on domain 0:
	(取消域 0 上与 mbm_total_bytes 事件关联的计数器的分配：)
	::

	 # echo "mbm_total_bytes:0=_" > /sys/fs/resctrl/mbm_L3_assignments
	 # cat /sys/fs/resctrl/mbm_L3_assignments
	   mbm_total_bytes:0=_;1=e
	   mbm_local_bytes:0=e;1=e

	To unassign the counter associated with the mbm_total_bytes event on all the domains:
	(取消所有域上与 mbm_total_bytes 事件关联的计数器的分配：)
	::

	 # echo "mbm_total_bytes:*=_" > /sys/fs/resctrl/mbm_L3_assignments
	 # cat /sys/fs/resctrl/mbm_L3_assignments
	   mbm_total_bytes:0=_;1=_
	   mbm_local_bytes:0=e;1=e

	To assign a counter associated with the mbm_total_bytes event on all domains in
	exclusive mode:
	(以独占模式为所有域分配与 mbm_total_bytes 事件关联的计数器：)
	::

	 # echo "mbm_total_bytes:*=e" > /sys/fs/resctrl/mbm_L3_assignments
	 # cat /sys/fs/resctrl/mbm_L3_assignments
	   mbm_total_bytes:0=e;1=e
	   mbm_local_bytes:0=e;1=e

When the "mba_MBps" mount option is used all CTRL_MON groups will also contain:
(使用 "mba_MBps" 挂载选项时，所有 CTRL_MON 组还将包含：)

"mba_MBps_event":
	Reading this file shows which memory bandwidth event is used
	as input to the software feedback loop that keeps memory bandwidth
	below the value specified in the schemata file. Writing the
	name of one of the supported memory bandwidth events found in
	/sys/fs/resctrl/info/L3_MON/mon_features changes the input
	event.
	(读取此文件显示哪个内存带宽事件被用作软件反馈环路的输入，该环路使内存带宽保持在方案文件中指定的值以下。写入 /sys/fs/resctrl/info/L3_MON/mon_features 中所列受支持内存带宽事件之一的名称即可更改输入事件。)

Resource allocation rules
(资源分配规则)
-------------------------

When a task is running the following rules define which resources are
available to it:
(当任务运行时，以下规则定义哪些资源可供其使用：)

1) If the task is a member of a non-default group, then the schemata
   for that group is used.
   (1. 如果任务是非默认组的成员，则使用该组的方案。)

2) Else if the task belongs to the default group, but is running on a
   CPU that is assigned to some specific group, then the schemata for the
   CPU's group is used.
   (2. 否则，如果任务属于默认组，但运行在分配给某个特定组的 CPU 上，则使用该 CPU 所属组的方案。)

3) Otherwise the schemata for the default group is used.
   (3. 否则使用默认组的方案。)

Resource monitoring rules
(资源监控规则)
-------------------------
1) If a task is a member of a MON group, or non-default CTRL_MON group
   then RDT events for the task will be reported in that group.
   (1. 如果任务是 MON 组或非默认 CTRL_MON 组的成员，则该任务的 RDT 事件将报告在该组中。)

2) If a task is a member of the default CTRL_MON group, but is running
   on a CPU that is assigned to some specific group, then the RDT events
   for the task will be reported in that group.
   (2. 如果任务属于默认 CTRL_MON 组，但运行在分配给某个特定组的 CPU 上，则该任务的 RDT 事件将报告在该组中。)

3) Otherwise RDT events for the task will be reported in the root level
   "mon_data" group.
   (3. 否则，该任务的 RDT 事件将报告在根级别的 "mon_data" 组中。)


Notes on cache occupancy monitoring and control
(关于缓存占用监控与控制的说明)
===============================================
When moving a task from one group to another you should remember that
this only affects *new* cache allocations by the task. E.g. you may have
a task in a monitor group showing 3 MB of cache occupancy. If you move
to a new group and immediately check the occupancy of the old and new
groups you will likely see that the old group is still showing 3 MB and
the new group zero. When the task accesses locations still in cache from
before the move, the h/w does not update any counters. On a busy system
you will likely see the occupancy in the old group go down as cache lines
are evicted and re-used while the occupancy in the new group rises as
the task accesses memory and loads into the cache are counted based on
membership in the new group.
(将任务从一个组移动到另一个组时，请记住这仅影响任务*新的*缓存分配。例如，某个监控组中的任务可能显示 3 MB 的缓存占用。如果你将其移到新组并立即检查旧组和新组的占用，你可能会看到旧组仍显示 3 MB 而新组为零。当任务访问移动前仍在缓存中的位置时，硬件不会更新任何计数器。在繁忙的系统上，随着缓存行被逐出和重用，你可能会看到旧组的占用下降，而随着任务访问内存、加载进缓存的操作按新组中的成员身份计数，新组的占用上升。)

The same applies to cache allocation control. Moving a task to a group
with a smaller cache partition will not evict any cache lines. The
process may continue to use them from the old partition.
(这同样适用于缓存分配控制。将任务移动到具有更小缓存分区的组不会逐出任何缓存行。进程可能继续从旧分区使用它们。)

Hardware uses CLOSid(Class of service ID) and an RMID(Resource monitoring ID)
to identify a control group and a monitoring group respectively. Each of
the resource groups are mapped to these IDs based on the kind of group. The
number of CLOSid and RMID are limited by the hardware and hence the creation of
a "CTRL_MON" directory may fail if we run out of either CLOSID or RMID
and creation of "MON" group may fail if we run out of RMIDs.
(硬件使用 CLOSid(服务类 ID)和 RMID(资源监控 ID)分别标识控制组和监控组。每个资源组根据组的类型映射到这些 ID。CLOSid 和 RMID 的数量受硬件限制，因此如果 CLOSID 或 RMID 耗尽，创建 "CTRL_MON" 目录可能失败；如果 RMID 耗尽，创建 "MON" 组可能失败。)

max_threshold_occupancy - generic concepts
(max_threshold_occupancy - 一般概念)
------------------------------------------

Note that an RMID once freed may not be immediately available for use as
the RMID is still tagged the cache lines of the previous user of RMID.
Hence such RMIDs are placed on limbo list and checked back if the cache
occupancy has gone down. If there is a time when system has a lot of
limbo RMIDs but which are not ready to be used, user may see an -EBUSY
during mkdir.
(注意，RMID 一旦被释放可能不会立即可用，因为 RMID 仍然标记着其上一个使用者的缓存行。因此这类 RMID 会被放入 limbo 列表，并在缓存占用下降后再检查。如果系统在某个时刻有大量尚未就绪可用的 limbo RMID，用户在 mkdir 期间可能会看到 -EBUSY。)

max_threshold_occupancy is a user configurable value to determine the
occupancy at which an RMID can be freed.
(max_threshold_occupancy 是用户可配置的值，用于确定可以释放 RMID 的占用阈值。)

The mon_llc_occupancy_limbo tracepoint gives the precise occupancy in bytes
for a subset of RMID that are not immediately available for allocation.
This can't be relied on to produce output every second, it may be necessary
to attempt to create an empty monitor group to force an update. Output may
only be produced if creation of a control or monitor group fails.
(mon_llc_occupancy_limbo 跟踪点给出尚未立即可供分配的一部分 RMID 的精确占用(以字节为单位)。不能指望它每秒都产生输出，可能需要尝试创建一个空的监控组来强制更新。只有在控制组或监控组创建失败时才可能产生输出。)

Schemata files - general concepts
(方案文件 - 一般概念)
---------------------------------
Each line in the file describes one resource. The line starts with
the name of the resource, followed by specific values to be applied
in each of the instances of that resource on the system.
(文件中的每一行描述一个资源。行以资源名称开头，后跟要应用到系统上该资源每个实例的具体值。)

Cache IDs
(缓存 ID)
---------
On current generation systems there is one L3 cache per socket and L2
caches are generally just shared by the hyperthreads on a core, but this
isn't an architectural requirement. We could have multiple separate L3
caches on a socket, multiple cores could share an L2 cache. So instead
of using "socket" or "core" to define the set of logical cpus sharing
a resource we use a "Cache ID". At a given cache level this will be a
unique number across the whole system (but it isn't guaranteed to be a
contiguous sequence, there may be gaps).  To find the ID for each logical
CPU look in /sys/devices/system/cpu/cpu*/cache/index*/id
(在当前一代系统上，每个插槽有一个 L3 缓存，L2 缓存通常仅由一个核心上的超线程共享，但这不是架构要求。一个插槽上可能有多个独立的 L3 缓存，多个核心可能共享一个 L2 缓存。因此我们不使用"插槽"或"核心"来定义共享某资源的逻辑 CPU 集合，而是使用"缓存 ID"。在给定的缓存级别上，它是整个系统中唯一的编号(但不保证是连续序列，可能有间隙)。每个逻辑 CPU 的 ID 可在 /sys/devices/system/cpu/cpu*/cache/index*/id 中查找。)

Cache Bit Masks (CBM)
(缓存位掩码 (CBM))
---------------------
For cache resources we describe the portion of the cache that is available
for allocation using a bitmask. The maximum value of the mask is defined
by each cpu model (and may be different for different cache levels). It
is found using CPUID, but is also provided in the "info" directory of
the resctrl file system in "info/{resource}/cbm_mask". Some Intel hardware
requires that these masks have all the '1' bits in a contiguous block. So
0x3, 0x6 and 0xC are legal 4-bit masks with two bits set, but 0x5, 0x9
and 0xA are not. Check /sys/fs/resctrl/info/{resource}/sparse_masks
if non-contiguous 1s value is supported. On a system with a 20-bit mask
each bit represents 5% of the capacity of the cache. You could partition
the cache into four equal parts with masks: 0x1f, 0x3e0, 0x7c00, 0xf8000.
(对于缓存资源，我们使用位掩码描述可供分配的缓存部分。掩码的最大值由每个 CPU 型号定义(不同缓存级别可能不同)。它可以通过 CPUID 查到，也在 resctrl 文件系统的 "info" 目录中 "info/{resource}/cbm_mask" 提供。某些 Intel 硬件要求这些掩码的所有 '1' 位位于一个连续块中。因此 0x3、0x6 和 0xC 是置两位的合法 4 位掩码，而 0x5、0x9 和 0xA 则不是。是否支持非连续的 1 值请查看 /sys/fs/resctrl/info/{resource}/sparse_masks。在使用 20 位掩码的系统上，每位代表缓存容量的 5%。你可以用掩码 0x1f、0x3e0、0x7c00、0xf8000 将缓存划分为四个相等的部分。)

Notes on Sub-NUMA Cluster mode
(关于子 NUMA 集群模式的说明)
==============================
When SNC mode is enabled, Linux may load balance tasks between Sub-NUMA
nodes much more readily than between regular NUMA nodes since the CPUs
on Sub-NUMA nodes share the same L3 cache and the system may report
the NUMA distance between Sub-NUMA nodes with a lower value than used
for regular NUMA nodes.
(启用 SNC 模式后，Linux 可能比在常规 NUMA 节点之间更容易地在子 NUMA 节点之间进行任务负载均衡，因为子 NUMA 节点上的 CPU 共享同一个 L3 缓存，并且系统报告的子 NUMA 节点之间的 NUMA 距离可能低于常规 NUMA 节点之间使用的值。)

The top-level monitoring files in each "mon_L3_XX" directory provide
the sum of data across all SNC nodes sharing an L3 cache instance.
Users who bind tasks to the CPUs of a specific Sub-NUMA node can read
the "llc_occupancy", "mbm_total_bytes", and "mbm_local_bytes" in the
"mon_sub_L3_YY" directories to get node local data.
(每个 "mon_L3_XX" 目录中的顶层监控文件提供共享一个 L3 缓存实例的所有 SNC 节点的数据总和。将任务绑定到特定子 NUMA 节点的 CPU 的用户可以读取 "mon_sub_L3_YY" 目录中的 "llc_occupancy"、"mbm_total_bytes" 和 "mbm_local_bytes" 来获取节点本地数据。)

Memory bandwidth allocation is still performed at the L3 cache
level. I.e. throttling controls are applied to all SNC nodes.
(内存带宽分配仍然在 L3 缓存级别执行。即节流控制应用于所有 SNC 节点。)

L3 cache allocation bitmaps also apply to all SNC nodes. But note that
the amount of L3 cache represented by each bit is divided by the number
of SNC nodes per L3 cache. E.g. with a 100MB cache on a system with 10-bit
allocation masks each bit normally represents 10MB. With SNC mode enabled
with two SNC nodes per L3 cache, each bit only represents 5MB.
(L3 缓存分配位图也适用于所有 SNC 节点。但请注意，每位所代表的 L3 缓存数量要除以每个 L3 缓存的 SNC 节点数。例如，在具有 10 位分配掩码的系统上，100MB 缓存的每位通常代表 10MB。启用 SNC 模式且每个 L3 缓存有两个 SNC 节点时，每位仅代表 5MB。)

Memory bandwidth Allocation and monitoring
(内存带宽分配与监控)
==========================================

For Memory bandwidth resource, by default the user controls the resource
by indicating the percentage of total memory bandwidth.
(对于内存带宽资源，默认情况下用户通过指定总内存带宽的百分比来控制该资源。)

The minimum bandwidth percentage value for each cpu model is predefined
and can be looked up through "info/MB/min_bandwidth". The bandwidth
granularity that is allocated is also dependent on the cpu model and can
be looked up at "info/MB/bandwidth_gran". The available bandwidth
control steps are: min_bw + N * bw_gran. Intermediate values are rounded
to the next control step available on the hardware.
(每个 CPU 型号的最小带宽百分比是预定义的，可通过 "info/MB/min_bandwidth" 查询。所分配带宽的粒度也取决于 CPU 型号，可在 "info/MB/bandwidth_gran" 查询。可用的带宽控制步为：min_bw + N * bw_gran。中间值会舍入到硬件上可用的下一个控制步。)

The bandwidth throttling is a core specific mechanism on some of Intel
SKUs. Using a high bandwidth and a low bandwidth setting on two threads
sharing a core may result in both threads being throttled to use the
low bandwidth (see "thread_throttle_mode").
(在某些 Intel SKU 上，带宽节流是核心特定的机制。在共享一个核心的两个线程上使用高带宽和低带宽设置，可能导致两个线程都被节流为使用低带宽(见 "thread_throttle_mode")。)

The fact that Memory bandwidth allocation(MBA) may be a core
specific mechanism where as memory bandwidth monitoring(MBM) is done at
the package level may lead to confusion when users try to apply control
via the MBA and then monitor the bandwidth to see if the controls are
effective. Below are such scenarios:
(内存带宽分配(MBA)可能是核心特定的机制，而内存带宽监控(MBM)在封装级别进行，这一事实可能导致用户在尝试通过 MBA 应用控制然后监控带宽以查看控制是否有效时产生困惑。以下是此类场景：)

1. User may *not* see increase in actual bandwidth when percentage
   values are increased:
   (1. 当百分比值提高时，用户可能*不会*看到实际带宽的增加：)

This can occur when aggregate L2 external bandwidth is more than L3
external bandwidth. Consider an SKL SKU with 24 cores on a package and
where L2 external  is 10GBps (hence aggregate L2 external bandwidth is
240GBps) and L3 external bandwidth is 100GBps. Now a workload with '20
threads, having 50% bandwidth, each consuming 5GBps' consumes the max L3
bandwidth of 100GBps although the percentage value specified is only 50%
<< 100%. Hence increasing the bandwidth percentage will not yield any
more bandwidth. This is because although the L2 external bandwidth still
has capacity, the L3 external bandwidth is fully used. Also note that
this would be dependent on number of cores the benchmark is run on.
(当 L2 外部带宽总和大于 L3 外部带宽时可能发生这种情况。考虑一个封装上有 24 个核心的 SKL SKU，L2 外部带宽为 10GBps(因此 L2 外部带宽总和为 240GBps)，L3 外部带宽为 100GBps。一个有 '20 个线程、50% 带宽、每个消耗 5GBps' 的工作负载会消耗掉 100GBps 的最大 L3 带宽，尽管指定的百分比值仅为 50% << 100%。因此提高带宽百分比不会带来更多带宽。这是因为尽管 L2 外部带宽仍有余量，但 L3 外部带宽已被完全占用。另请注意，这取决于基准测试运行所在的核心数量。)

2. Same bandwidth percentage may mean different actual bandwidth
   depending on # of threads:
   (2. 相同的带宽百分比可能因线程数量不同而意味着不同的实际带宽：)

For the same SKU in #1, a 'single thread, with 10% bandwidth' and '4
thread, with 10% bandwidth' can consume up to 10GBps and 40GBps although
they have same percentage bandwidth of 10%. This is simply because as
threads start using more cores in an rdtgroup, the actual bandwidth may
increase or vary although user specified bandwidth percentage is same.
(对于 #1 中相同的 SKU，'单线程、10% 带宽'和 '4 线程、10% 带宽'可以分别消耗高达 10GBps 和 40GBps，尽管它们的百分比带宽同为 10%。这仅仅是因为随着线程开始使用 rdtgroup 中更多的核心，尽管用户指定的带宽百分比相同，实际带宽可能增加或变化。)

In order to mitigate this and make the interface more user friendly,
resctrl added support for specifying the bandwidth in MiBps as well.  The
kernel underneath would use a software feedback mechanism or a "Software
Controller(mba_sc)" which reads the actual bandwidth using MBM counters
and adjust the memory bandwidth percentages to ensure::
(为了缓解这个问题并使接口更加用户友好，resctrl 还增加了以 MiBps 为单位指定带宽的支持。底层内核将使用软件反馈机制或"软件控制器(mba_sc)"，它使用 MBM 计数器读取实际带宽并调整内存带宽百分比以确保：)

	"actual bandwidth < user specified bandwidth".

By default, the schemata would take the bandwidth percentage values
where as user can switch to the "MBA software controller" mode using
a mount option 'mba_MBps'. The schemata format is specified in the below
sections.
(默认情况下，方案采用带宽百分比；而用户可以使用挂载选项 'mba_MBps' 切换到 "MBA 软件控制器"模式。方案格式在下文各节中说明。)

L3 schemata file details (code and data prioritization disabled)
(L3 方案文件详情 (代码与数据优先化已禁用))
----------------------------------------------------------------
With CDP disabled the L3 schemata format is::
(禁用 CDP 时，L3 方案格式为：)

	L3:<cache_id0>=<cbm>;<cache_id1>=<cbm>;...

L3 schemata file details (CDP enabled via mount option to resctrl)
(L3 方案文件详情 (通过 resctrl 挂载选项启用 CDP))
------------------------------------------------------------------
When CDP is enabled L3 control is split into two separate resources
so you can specify independent masks for code and data like this::
(启用 CDP 后，L3 控制被拆分为两个独立的资源，因此可以像这样为代码和数据指定独立的掩码：)

	L3DATA:<cache_id0>=<cbm>;<cache_id1>=<cbm>;...
	L3CODE:<cache_id0>=<cbm>;<cache_id1>=<cbm>;...

L2 schemata file details
(L2 方案文件详情)
------------------------
CDP is supported at L2 using the 'cdpl2' mount option. The schemata
format is either::
(在 L2 上使用 'cdpl2' 挂载选项支持 CDP。方案格式为以下之一：)

	L2:<cache_id0>=<cbm>;<cache_id1>=<cbm>;...

or
(或)

	L2DATA:<cache_id0>=<cbm>;<cache_id1>=<cbm>;...
	L2CODE:<cache_id0>=<cbm>;<cache_id1>=<cbm>;...


Memory bandwidth Allocation (default mode)
(内存带宽分配 (默认模式))
------------------------------------------

Memory b/w domain is L3 cache.
(内存带宽域是 L3 缓存。)
::

	MB:<cache_id0>=bandwidth0;<cache_id1>=bandwidth1;...

Memory bandwidth Allocation specified in MiBps
(以 MiBps 指定的内存带宽分配)
----------------------------------------------

Memory bandwidth domain is L3 cache.
(内存带宽域是 L3 缓存。)
::

	MB:<cache_id0>=bw_MiBps0;<cache_id1>=bw_MiBps1;...

Slow Memory Bandwidth Allocation (SMBA)
(慢速内存带宽分配 (SMBA))
---------------------------------------
AMD hardware supports Slow Memory Bandwidth Allocation (SMBA).
CXL.memory is the only supported "slow" memory device. With the
support of SMBA, the hardware enables bandwidth allocation on
the slow memory devices. If there are multiple such devices in
the system, the throttling logic groups all the slow sources
together and applies the limit on them as a whole.
(AMD 硬件支持慢速内存带宽分配(SMBA)。CXL.memory 是唯一受支持的"慢速"内存设备。借助 SMBA 支持，硬件可以在慢速内存设备上进行带宽分配。如果系统中有多个此类设备，节流逻辑会将所有慢速源归为一组，并对其整体施加限制。)

The presence of SMBA (with CXL.memory) is independent of slow memory
devices presence. If there are no such devices on the system, then
configuring SMBA will have no impact on the performance of the system.
(SMBA(含 CXL.memory)的存在与慢速内存设备的存在无关。如果系统上没有此类设备，配置 SMBA 将对系统性能没有影响。)

The bandwidth domain for slow memory is L3 cache. Its schemata file
is formatted as:
(慢速内存的带宽域是 L3 缓存。其方案文件格式如下：)
::

	SMBA:<cache_id0>=bandwidth0;<cache_id1>=bandwidth1;...

Reading/writing the schemata file
(读写方案文件)
---------------------------------
Reading the schemata file will show the state of all resources
on all domains. When writing you only need to specify those values
which you wish to change.  E.g.
(读取方案文件将显示所有域上所有资源的状态。写入时只需指定想要更改的值。例如：)
::

  # cat schemata
  L3DATA:0=fffff;1=fffff;2=fffff;3=fffff
  L3CODE:0=fffff;1=fffff;2=fffff;3=fffff
  # echo "L3DATA:2=3c0;" > schemata
  # cat schemata
  L3DATA:0=fffff;1=fffff;2=3c0;3=fffff
  L3CODE:0=fffff;1=fffff;2=fffff;3=fffff

Reading/writing the schemata file (on AMD systems)
(读写方案文件 (在 AMD 系统上))
--------------------------------------------------
Reading the schemata file will show the current bandwidth limit on all
domains. The allocated resources are in multiples of one eighth GB/s.
When writing to the file, you need to specify what cache id you wish to
configure the bandwidth limit.
(读取方案文件将显示所有域上的当前带宽限制。分配的资源以八分之一 GB/s 的倍数表示。写入该文件时，需要指定想要为其配置带宽限制的缓存 ID。)

For example, to allocate 2GB/s limit on the first cache id:
(例如，在第一个缓存 ID 上分配 2GB/s 限制：)

::

  # cat schemata
    MB:0=2048;1=2048;2=2048;3=2048
    L3:0=ffff;1=ffff;2=ffff;3=ffff

  # echo "MB:1=16" > schemata
  # cat schemata
    MB:0=2048;1=  16;2=2048;3=2048
    L3:0=ffff;1=ffff;2=ffff;3=ffff

Reading/writing the schemata file (on AMD systems) with SMBA feature
(读写方案文件 (在 AMD 系统上，含 SMBA 特性))
--------------------------------------------------------------------
Reading and writing the schemata file is the same as without SMBA in
above section.
(方案文件的读写与上一节中没有 SMBA 时相同。)

For example, to allocate 8GB/s limit on the first cache id:
(例如，在第一个缓存 ID 上分配 8GB/s 限制：)

::

  # cat schemata
    SMBA:0=2048;1=2048;2=2048;3=2048
      MB:0=2048;1=2048;2=2048;3=2048
      L3:0=ffff;1=ffff;2=ffff;3=ffff

  # echo "SMBA:1=64" > schemata
  # cat schemata
    SMBA:0=2048;1=  64;2=2048;3=2048
      MB:0=2048;1=2048;2=2048;3=2048
      L3:0=ffff;1=ffff;2=ffff;3=ffff

Cache Pseudo-Locking
(缓存伪锁定)
====================
CAT enables a user to specify the amount of cache space that an
application can fill. Cache pseudo-locking builds on the fact that a
CPU can still read and write data pre-allocated outside its current
allocated area on a cache hit. With cache pseudo-locking, data can be
preloaded into a reserved portion of cache that no application can
fill, and from that point on will only serve cache hits. The cache
pseudo-locked memory is made accessible to user space where an
application can map it into its virtual address space and thus have
a region of memory with reduced average read latency.
(CAT 使用户能够指定应用程序可以填充的缓存空间大小。缓存伪锁定基于这样一个事实：在缓存命中时，CPU 仍然可以读取和写入预分配在其当前分配区域之外的数据。通过缓存伪锁定，数据可以预加载到一个任何应用程序都无法填充的保留缓存部分中，并且从那时起只会提供缓存命中。缓存伪锁定内存被开放给用户空间，应用程序可以将其映射到自己的虚拟地址空间，从而拥有一段平均读取延迟降低的内存区域。)

The creation of a cache pseudo-locked region is triggered by a request
from the user to do so that is accompanied by a schemata of the region
to be pseudo-locked. The cache pseudo-locked region is created as follows:
(缓存伪锁定区域的创建由用户的请求触发，请求附带要伪锁定区域的方案。缓存伪锁定区域按如下方式创建：)

- Create a CAT allocation CLOSNEW with a CBM matching the schemata
  from the user of the cache region that will contain the pseudo-locked
  memory. This region must not overlap with any current CAT allocation/CLOS
  on the system and no future overlap with this cache region is allowed
  while the pseudo-locked region exists.
  (创建一个 CAT 分配 CLOSNEW，其 CBM 与用户为将包含伪锁定内存的缓存区域提供的方案相匹配。该区域不得与系统上任何当前的 CAT 分配/CLOS 重叠，并且在伪锁定区域存在期间，不允许将来与此缓存区域重叠。)
- Create a contiguous region of memory of the same size as the cache
  region.
  (创建一个与该缓存区域大小相同的连续内存区域。)
- Flush the cache, disable hardware prefetchers, disable preemption.
  (刷新缓存，禁用硬件预取器，禁用抢占。)
- Make CLOSNEW the active CLOS and touch the allocated memory to load
  it into the cache.
  (使 CLOSNEW 成为活动的 CLOS，并触碰已分配的内存以将其加载到缓存中。)
- Set the previous CLOS as active.
  (将之前的 CLOS 设为活动。)
- At this point the closid CLOSNEW can be released - the cache
  pseudo-locked region is protected as long as its CBM does not appear in
  any CAT allocation. Even though the cache pseudo-locked region will from
  this point on not appear in any CBM of any CLOS an application running with
  any CLOS will be able to access the memory in the pseudo-locked region since
  the region continues to serve cache hits.
  (此时可以释放 closid CLOSNEW——只要其 CBM 不出现在任何 CAT 分配中，缓存伪锁定区域就受到保护。尽管从这一点起缓存伪锁定区域不会再出现在任何 CLOS 的任何 CBM 中，但以任何 CLOS 运行的应用程序仍能访问伪锁定区域中的内存，因为该区域会继续提供缓存命中。)
- The contiguous region of memory loaded into the cache is exposed to
  user-space as a character device.
  (加载到缓存中的连续内存区域作为字符设备暴露给用户空间。)

Cache pseudo-locking increases the probability that data will remain
in the cache via carefully configuring the CAT feature and controlling
application behavior. There is no guarantee that data is placed in
cache. Instructions like INVD, WBINVD, CLFLUSH, etc. can still evict
“locked” data from cache. Power management C-states may shrink or
power off cache. Deeper C-states will automatically be restricted on
pseudo-locked region creation.
(缓存伪锁定通过仔细配置 CAT 特性和控制应用程序行为来提高数据留在缓存中的概率。无法保证数据一定放入缓存。INVD、WBINVD、CLFLUSH 等指令仍可能将"锁定的"数据从缓存中逐出。电源管理 C 状态可能缩小或关闭缓存。在伪锁定区域创建时，更深的 C 状态将被自动限制。)

It is required that an application using a pseudo-locked region runs
with affinity to the cores (or a subset of the cores) associated
with the cache on which the pseudo-locked region resides. A sanity check
within the code will not allow an application to map pseudo-locked memory
unless it runs with affinity to cores associated with the cache on which the
pseudo-locked region resides. The sanity check is only done during the
initial mmap() handling, there is no enforcement afterwards and the
application self needs to ensure it remains affine to the correct cores.
(要求使用伪锁定区域的应用程序以与伪锁定区域所在缓存关联的核心(或其子集)的亲和性运行。代码中的完整性检查不允许应用程序映射伪锁定内存，除非它以与伪锁定区域所在缓存关联的核心的亲和性运行。完整性检查仅在初始 mmap() 处理期间进行，之后没有强制措施，应用程序自身需要确保保持对正确核心的亲和。)

Pseudo-locking is accomplished in two stages:
(伪锁定分两个阶段完成：)

1) During the first stage the system administrator allocates a portion
   of cache that should be dedicated to pseudo-locking. At this time an
   equivalent portion of memory is allocated, loaded into allocated
   cache portion, and exposed as a character device.
   (1. 在第一阶段，系统管理员分配一部分应专用于伪锁定的缓存。此时会分配等量的内存，将其加载到已分配的缓存部分，并作为字符设备暴露出来。)
2) During the second stage a user-space application maps (mmap()) the
   pseudo-locked memory into its address space.
   (2. 在第二阶段，用户空间应用程序将伪锁定内存映射(mmap())到其地址空间中。)

Cache Pseudo-Locking Interface
(缓存伪锁定接口)
------------------------------
A pseudo-locked region is created using the resctrl interface as follows:
(使用 resctrl 接口按如下方式创建伪锁定区域：)

1) Create a new resource group by creating a new directory in /sys/fs/resctrl.
   (1. 通过在 /sys/fs/resctrl 中创建新目录来创建新的资源组。)
2) Change the new resource group's mode to "pseudo-locksetup" by writing
   "pseudo-locksetup" to the "mode" file.
   (2. 通过将 "pseudo-locksetup" 写入 "mode" 文件，将新资源组的模式更改为 "pseudo-locksetup"。)
3) Write the schemata of the pseudo-locked region to the "schemata" file. All
   bits within the schemata should be "unused" according to the "bit_usage"
   file.
   (3. 将伪锁定区域的方案写入 "schemata" 文件。根据 "bit_usage" 文件，方案中的所有位都应为 "unused"。)

On successful pseudo-locked region creation the "mode" file will contain
"pseudo-locked" and a new character device with the same name as the resource
group will exist in /dev/pseudo_lock. This character device can be mmap()'ed
by user space in order to obtain access to the pseudo-locked memory region.
(伪锁定区域创建成功后，"mode" 文件将包含 "pseudo-locked"，并且 /dev/pseudo_lock 中将存在一个与资源组同名的新字符设备。用户空间可以 mmap() 该字符设备，以获得对伪锁定内存区域的访问。)

An example of cache pseudo-locked region creation and usage can be found below.
(下面可以找到缓存伪锁定区域创建和使用的示例。)

Cache Pseudo-Locking Debugging Interface
(缓存伪锁定调试接口)
----------------------------------------
The pseudo-locking debugging interface is enabled by default (if
CONFIG_DEBUG_FS is enabled) and can be found in /sys/kernel/debug/resctrl.
(伪锁定调试接口默认启用(如果 CONFIG_DEBUG_FS 已启用)，位于 /sys/kernel/debug/resctrl。)

There is no explicit way for the kernel to test if a provided memory
location is present in the cache. The pseudo-locking debugging interface uses
the tracing infrastructure to provide two ways to measure cache residency of
the pseudo-locked region:
(内核没有显式的方法来测试提供的内存位置是否存在于缓存中。伪锁定调试接口使用跟踪基础设施提供两种测量伪锁定区域缓存驻留情况的方式：)

1) Memory access latency using the pseudo_lock_mem_latency tracepoint. Data
   from these measurements are best visualized using a hist trigger (see
   example below). In this test the pseudo-locked region is traversed at
   a stride of 32 bytes while hardware prefetchers and preemption
   are disabled. This also provides a substitute visualization of cache
   hits and misses.
   (1. 使用 pseudo_lock_mem_latency 跟踪点的内存访问延迟。这些测量数据最适合使用 hist 触发器进行可视化(见下面的示例)。在此测试中，伪锁定区域以 32 字节的步长被遍历，同时禁用硬件预取器和抢占。这也提供了缓存命中和未命中的替代可视化。)
2) Cache hit and miss measurements using model specific precision counters if
   available. Depending on the levels of cache on the system the pseudo_lock_l2
   and pseudo_lock_l3 tracepoints are available.
   (2. 使用型号特定的精度计数器(如果可用)测量缓存命中和未命中。根据系统上的缓存级别，可以使用 pseudo_lock_l2 和 pseudo_lock_l3 跟踪点。)

When a pseudo-locked region is created a new debugfs directory is created for
it in debugfs as /sys/kernel/debug/resctrl/<newdir>. A single
write-only file, pseudo_lock_measure, is present in this directory. The
measurement of the pseudo-locked region depends on the number written to this
debugfs file:
(创建伪锁定区域时，会在 debugfs 中为其创建一个新目录 /sys/kernel/debug/resctrl/<newdir>。该目录中有一个只写文件 pseudo_lock_measure。伪锁定区域的测量取决于写入此 debugfs 文件的数字：)

1:
     writing "1" to the pseudo_lock_measure file will trigger the latency
     measurement captured in the pseudo_lock_mem_latency tracepoint. See
     example below.
     (向 pseudo_lock_measure 文件写入 "1" 将触发 pseudo_lock_mem_latency 跟踪点捕获的延迟测量。见下面的示例。)
2:
     writing "2" to the pseudo_lock_measure file will trigger the L2 cache
     residency (cache hits and misses) measurement captured in the
     pseudo_lock_l2 tracepoint. See example below.
     (向 pseudo_lock_measure 文件写入 "2" 将触发 pseudo_lock_l2 跟踪点捕获的 L2 缓存驻留(缓存命中和未命中)测量。见下面的示例。)
3:
     writing "3" to the pseudo_lock_measure file will trigger the L3 cache
     residency (cache hits and misses) measurement captured in the
     pseudo_lock_l3 tracepoint.
     (向 pseudo_lock_measure 文件写入 "3" 将触发 pseudo_lock_l3 跟踪点捕获的 L3 缓存驻留(缓存命中和未命中)测量。)

All measurements are recorded with the tracing infrastructure. This requires
the relevant tracepoints to be enabled before the measurement is triggered.
(所有测量都通过跟踪基础设施记录。这要求在触发测量之前启用相关跟踪点。)

Example of latency debugging interface
(延迟调试接口示例)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In this example a pseudo-locked region named "newlock" was created. Here is
how we can measure the latency in cycles of reading from this region and
visualize this data with a histogram that is available if CONFIG_HIST_TRIGGERS
is set::
(在此示例中，创建了一个名为 "newlock" 的伪锁定区域。下面展示如何测量从该区域读取的延迟(以周期为单位)，并在设置了 CONFIG_HIST_TRIGGERS 时用直方图将这些数据可视化：)

  # :> /sys/kernel/tracing/trace
  # echo 'hist:keys=latency' > /sys/kernel/tracing/events/resctrl/pseudo_lock_mem_latency/trigger
  # echo 1 > /sys/kernel/tracing/events/resctrl/pseudo_lock_mem_latency/enable
  # echo 1 > /sys/kernel/debug/resctrl/newlock/pseudo_lock_measure
  # echo 0 > /sys/kernel/tracing/events/resctrl/pseudo_lock_mem_latency/enable
  # cat /sys/kernel/tracing/events/resctrl/pseudo_lock_mem_latency/hist

  # event histogram
  #
  # trigger info: hist:keys=latency:vals=hitcount:sort=hitcount:size=2048 [active]
  #

  { latency:        456 } hitcount:          1
  { latency:         50 } hitcount:         83
  { latency:         36 } hitcount:         96
  { latency:         44 } hitcount:        174
  { latency:         48 } hitcount:        195
  { latency:         46 } hitcount:        262
  { latency:         42 } hitcount:        693
  { latency:         40 } hitcount:       3204
  { latency:         38 } hitcount:       3484

  Totals:
      Hits: 8192
      Entries: 9
    Dropped: 0

Example of cache hits/misses debugging
(缓存命中/未命中调试示例)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In this example a pseudo-locked region named "newlock" was created on the L2
cache of a platform. Here is how we can obtain details of the cache hits
and misses using the platform's precision counters.
(在此示例中，在一个平台的 L2 缓存上创建了一个名为 "newlock" 的伪锁定区域。下面展示如何使用平台的精度计数器获取缓存命中和未命中的详细信息。)
::

  # :> /sys/kernel/tracing/trace
  # echo 1 > /sys/kernel/tracing/events/resctrl/pseudo_lock_l2/enable
  # echo 2 > /sys/kernel/debug/resctrl/newlock/pseudo_lock_measure
  # echo 0 > /sys/kernel/tracing/events/resctrl/pseudo_lock_l2/enable
  # cat /sys/kernel/tracing/trace

  # tracer: nop
  #
  #                              _-----=> irqs-off
  #                             / _----=> need-resched
  #                            | / _---=> hardirq/softirq
  #                            || / _--=> preempt-depth
  #                            ||| /     delay
  #           TASK-PID   CPU#  ||||    TIMESTAMP  FUNCTION
  #              | |       |   ||||       |         |
  pseudo_lock_mea-1672  [002] ....  3132.860500: pseudo_lock_l2: hits=4097 miss=0


Examples for RDT allocation usage
(RDT 分配使用示例)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1) Example 1
   (1. 示例 1)

On a two socket machine (one L3 cache per socket) with just four bits
for cache bit masks, minimum b/w of 10% with a memory bandwidth
granularity of 10%.
(在一台双插槽机器(每个插槽一个 L3 缓存)上，缓存位掩码只有 4 位，最小带宽为 10%，内存带宽粒度为 10%。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl
  # mkdir p0 p1
  # echo "L3:0=3;1=c\nMB:0=50;1=50" > /sys/fs/resctrl/p0/schemata
  # echo "L3:0=3;1=3\nMB:0=50;1=50" > /sys/fs/resctrl/p1/schemata

The default resource group is unmodified, so we have access to all parts
of all caches (its schemata file reads "L3:0=f;1=f").
(默认资源组未被修改，因此我们可以访问所有缓存的全部部分(其 schemata 文件内容为 "L3:0=f;1=f")。)

Tasks that are under the control of group "p0" may only allocate from the
"lower" 50% on cache ID 0, and the "upper" 50% of cache ID 1.
Tasks in group "p1" use the "lower" 50% of cache on both sockets.
(受组 "p0" 控制的任务只能从缓存 ID 0 的"较低"50% 和缓存 ID 1 的"较高"50% 中分配。组 "p1" 中的任务在两个插槽上都使用"较低"的 50% 缓存。)

Similarly, tasks that are under the control of group "p0" may use a
maximum memory b/w of 50% on socket0 and 50% on socket 1.
Tasks in group "p1" may also use 50% memory b/w on both sockets.
Note that unlike cache masks, memory b/w cannot specify whether these
allocations can overlap or not. The allocations specifies the maximum
b/w that the group may be able to use and the system admin can configure
the b/w accordingly.
(类似地，受组 "p0" 控制的任务在插槽 0 上最多可使用 50% 的内存带宽，在插槽 1 上也是 50%。组 "p1" 中的任务在两个插槽上同样可以使用 50% 的内存带宽。注意，与缓存掩码不同，内存带宽无法指定这些分配是否可以重叠。分配指定的是该组可能能够使用的最大带宽，系统管理员可以相应地配置带宽。)

If resctrl is using the software controller (mba_sc) then user can enter the
max b/w in MB rather than the percentage values.
(如果 resctrl 使用软件控制器(mba_sc)，则用户可以输入以 MB 为单位的最大带宽而不是百分比值。)
::

  # echo "L3:0=3;1=c\nMB:0=1024;1=500" > /sys/fs/resctrl/p0/schemata
  # echo "L3:0=3;1=3\nMB:0=1024;1=500" > /sys/fs/resctrl/p1/schemata

In the above example the tasks in "p1" and "p0" on socket 0 would use a max b/w
of 1024MB where as on socket 1 they would use 500MB.
(在上面的示例中，插槽 0 上 "p1" 和 "p0" 中的任务将使用最大 1024MB 的带宽，而在插槽 1 上它们将使用 500MB。)

2) Example 2
   (2. 示例 2)

Again two sockets, but this time with a more realistic 20-bit mask.
(同样是两个插槽，但这次使用更现实的 20 位掩码。)

Two real time tasks pid=1234 running on processor 0 and pid=5678 running on
processor 1 on socket 0 on a 2-socket and dual core machine. To avoid noisy
neighbors, each of the two real-time tasks exclusively occupies one quarter
of L3 cache on socket 0.
(两个实时任务：pid=1234 运行在处理器 0 上，pid=5678 运行在 2 插槽双核机器插槽 0 的处理器 1 上。为避免噪声邻居，两个实时任务各自独占插槽 0 上 L3 缓存的四分之一。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl

First we reset the schemata for the default group so that the "upper"
50% of the L3 cache on socket 0 and 50% of memory b/w cannot be used by
ordinary tasks::
(首先，我们重置默认组的方案，使插槽 0 上 L3 缓存的"较高"50% 和 50% 的内存带宽不能被普通任务使用：)

  # echo "L3:0=3ff;1=fffff\nMB:0=50;1=100" > schemata

Next we make a resource group for our first real time task and give
it access to the "top" 25% of the cache on socket 0.
(接下来，我们为第一个实时任务创建一个资源组，并让它访问插槽 0 上缓存的"顶部"25%。)
::

  # mkdir p0
  # echo "L3:0=f8000;1=fffff" > p0/schemata

Finally we move our first real time task into this resource group. We
also use taskset(1) to ensure the task always runs on a dedicated CPU
on socket 0. Most uses of resource groups will also constrain which
processors tasks run on.
(最后，我们将第一个实时任务移入此资源组。我们还使用 taskset(1) 确保任务始终运行在插槽 0 上的专用 CPU 上。资源组的大多数用法还会约束任务运行在哪些处理器上。)
::

  # echo 1234 > p0/tasks
  # taskset -cp 1 1234

Ditto for the second real time task (with the remaining 25% of cache)::
(第二个实时任务同样处理(使用剩余的 25% 缓存)：)

  # mkdir p1
  # echo "L3:0=7c00;1=fffff" > p1/schemata
  # echo 5678 > p1/tasks
  # taskset -cp 2 5678

For the same 2 socket system with memory b/w resource and CAT L3 the
schemata would look like(Assume min_bandwidth 10 and bandwidth_gran is
10):
(对于相同的带内存带宽资源和 CAT L3 的 2 插槽系统，方案如下(假设 min_bandwidth 为 10，bandwidth_gran 为 10)：)

For our first real time task this would request 20% memory b/w on socket 0.
(对于我们的第一个实时任务，这将请求插槽 0 上 20% 的内存带宽。)
::

  # echo -e "L3:0=f8000;1=fffff\nMB:0=20;1=100" > p0/schemata

For our second real time task this would request an other 20% memory b/w
on socket 0.
(对于我们的第二个实时任务，这将请求插槽 0 上另外 20% 的内存带宽。)
::

  # echo -e "L3:0=f8000;1=fffff\nMB:0=20;1=100" > p0/schemata

3) Example 3
   (3. 示例 3)

A single socket system which has real-time tasks running on core 4-7 and
non real-time workload assigned to core 0-3. The real-time tasks share text
and data, so a per task association is not required and due to interaction
with the kernel it's desired that the kernel on these cores shares L3 with
the tasks.
(单插槽系统，实时任务运行在核心 4-7 上，非实时工作负载分配在核心 0-3 上。实时任务共享文本和数据，因此不需要按任务关联；并且由于与内核的交互，希望这些核心上的内核与任务共享 L3。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl

First we reset the schemata for the default group so that the "upper"
50% of the L3 cache on socket 0, and 50% of memory bandwidth on socket 0
cannot be used by ordinary tasks::
(首先，我们重置默认组的方案，使插槽 0 上 L3 缓存的"较高"50% 和插槽 0 上 50% 的内存带宽不能被普通任务使用：)

  # echo "L3:0=3ff\nMB:0=50" > schemata

Next we make a resource group for our real time cores and give it access
to the "top" 50% of the cache on socket 0 and 50% of memory bandwidth on
socket 0.
(接下来，我们为实时核心创建一个资源组，并让它访问插槽 0 上缓存的"顶部"50% 和插槽 0 上 50% 的内存带宽。)
::

  # mkdir p0
  # echo "L3:0=ffc00\nMB:0=50" > p0/schemata

Finally we move core 4-7 over to the new group and make sure that the
kernel and the tasks running there get 50% of the cache. They should
also get 50% of memory bandwidth assuming that the cores 4-7 are SMT
siblings and only the real time threads are scheduled on the cores 4-7.
(最后，我们将核心 4-7 移到新组，并确保在那里运行的内核和任务获得 50% 的缓存。假设核心 4-7 是 SMT 兄弟线程且只有实时线程调度在核心 4-7 上，它们还应获得 50% 的内存带宽。)
::

  # echo F0 > p0/cpus

4) Example 4
   (4. 示例 4)

The resource groups in previous examples were all in the default "shareable"
mode allowing sharing of their cache allocations. If one resource group
configures a cache allocation then nothing prevents another resource group
to overlap with that allocation.
(前面示例中的资源组都处于默认的 "shareable" 模式，允许共享其缓存分配。如果一个资源组配置了缓存分配，则没有任何东西阻止另一个资源组与该分配重叠。)

In this example a new exclusive resource group will be created on a L2 CAT
system with two L2 cache instances that can be configured with an 8-bit
capacity bitmask. The new exclusive resource group will be configured to use
25% of each cache instance.
(在此示例中，将在具有两个可配置 8 位容量位掩码的 L2 缓存实例的 L2 CAT 系统上创建一个新的独占资源组。新的独占资源组将配置为使用每个缓存实例的 25%。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl/
  # cd /sys/fs/resctrl

First, we observe that the default group is configured to allocate to all L2
cache::
(首先，我们观察到默认组被配置为分配所有 L2 缓存：)

  # cat schemata
  L2:0=ff;1=ff

We could attempt to create the new resource group at this point, but it will
fail because of the overlap with the schemata of the default group::
(此时我们可以尝试创建新的资源组，但由于与默认组的方案重叠，它将失败：)

  # mkdir p0
  # echo 'L2:0=0x3;1=0x3' > p0/schemata
  # cat p0/mode
  shareable
  # echo exclusive > p0/mode
  -sh: echo: write error: Invalid argument
  # cat info/last_cmd_status
  schemata overlaps

To ensure that there is no overlap with another resource group the default
resource group's schemata has to change, making it possible for the new
resource group to become exclusive.
(为确保与其他资源组没有重叠，必须更改默认资源组的方案，使新资源组有可能成为独占。)
::

  # echo 'L2:0=0xfc;1=0xfc' > schemata
  # echo exclusive > p0/mode
  # grep . p0/*
  p0/cpus:0
  p0/mode:exclusive
  p0/schemata:L2:0=03;1=03
  p0/size:L2:0=262144;1=262144

A new resource group will on creation not overlap with an exclusive resource
group::
(新资源组在创建时不会与独占资源组重叠：)

  # mkdir p1
  # grep . p1/*
  p1/cpus:0
  p1/mode:shareable
  p1/schemata:L2:0=fc;1=fc
  p1/size:L2:0=786432;1=786432

The bit_usage will reflect how the cache is used::
(bit_usage 将反映缓存的使用情况：)

  # cat info/L2/bit_usage
  0=SSSSSSEE;1=SSSSSSEE

A resource group cannot be forced to overlap with an exclusive resource group::
(不能强制资源组与独占资源组重叠：)

  # echo 'L2:0=0x1;1=0x1' > p1/schemata
  -sh: echo: write error: Invalid argument
  # cat info/last_cmd_status
  overlaps with exclusive group

Example of Cache Pseudo-Locking
(缓存伪锁定示例)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Lock portion of L2 cache from cache id 1 using CBM 0x3. Pseudo-locked
region is exposed at /dev/pseudo_lock/newlock that can be provided to
application for argument to mmap().
(使用 CBM 0x3 从缓存 ID 1 锁定 L2 缓存的一部分。伪锁定区域暴露在 /dev/pseudo_lock/newlock，可以作为参数提供给应用程序用于 mmap()。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl/
  # cd /sys/fs/resctrl

Ensure that there are bits available that can be pseudo-locked, since only
unused bits can be pseudo-locked the bits to be pseudo-locked needs to be
removed from the default resource group's schemata::
(确保有可用于伪锁定的位。由于只有未使用的位才能被伪锁定，需要从默认资源组的方案中移除要伪锁定的位：)

  # cat info/L2/bit_usage
  0=SSSSSSSS;1=SSSSSSSS
  # echo 'L2:1=0xfc' > schemata
  # cat info/L2/bit_usage
  0=SSSSSSSS;1=SSSSSS00

Create a new resource group that will be associated with the pseudo-locked
region, indicate that it will be used for a pseudo-locked region, and
configure the requested pseudo-locked region capacity bitmask::
(创建一个将与伪锁定区域关联的新资源组，表明它将用于伪锁定区域，并配置所请求的伪锁定区域容量位掩码：)

  # mkdir newlock
  # echo pseudo-locksetup > newlock/mode
  # echo 'L2:1=0x3' > newlock/schemata

On success the resource group's mode will change to pseudo-locked, the
bit_usage will reflect the pseudo-locked region, and the character device
exposing the pseudo-locked region will exist::
(成功后，资源组的模式将更改为伪锁定，bit_usage 将反映伪锁定区域，并且暴露伪锁定区域的字符设备将存在：)

  # cat newlock/mode
  pseudo-locked
  # cat info/L2/bit_usage
  0=SSSSSSSS;1=SSSSSSPP
  # ls -l /dev/pseudo_lock/newlock
  crw------- 1 root root 243, 0 Apr  3 05:01 /dev/pseudo_lock/newlock

::

  /*
  * Example code to access one page of pseudo-locked cache region
  * from user space.
  */
  #define _GNU_SOURCE
  #include <fcntl.h>
  #include <sched.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/mman.h>

  /*
  * It is required that the application runs with affinity to only
  * cores associated with the pseudo-locked region. Here the cpu
  * is hardcoded for convenience of example.
  */
  static int cpuid = 2;

  int main(int argc, char *argv[])
  {
    cpu_set_t cpuset;
    long page_size;
    void *mapping;
    int dev_fd;
    int ret;

    page_size = sysconf(_SC_PAGESIZE);

    CPU_ZERO(&cpuset);
    CPU_SET(cpuid, &cpuset);
    ret = sched_setaffinity(0, sizeof(cpuset), &cpuset);
    if (ret < 0) {
      perror("sched_setaffinity");
      exit(EXIT_FAILURE);
    }

    dev_fd = open("/dev/pseudo_lock/newlock", O_RDWR);
    if (dev_fd < 0) {
      perror("open");
      exit(EXIT_FAILURE);
    }

    mapping = mmap(0, page_size, PROT_READ | PROT_WRITE, MAP_SHARED,
            dev_fd, 0);
    if (mapping == MAP_FAILED) {
      perror("mmap");
      close(dev_fd);
      exit(EXIT_FAILURE);
    }

    /* Application interacts with pseudo-locked memory @mapping */

    ret = munmap(mapping, page_size);
    if (ret < 0) {
      perror("munmap");
      close(dev_fd);
      exit(EXIT_FAILURE);
    }

    close(dev_fd);
    exit(EXIT_SUCCESS);
  }

Locking between applications
(应用程序之间的锁定)
----------------------------

Certain operations on the resctrl filesystem, composed of read/writes
to/from multiple files, must be atomic.
(resctrl 文件系统上的某些操作由对多个文件的读/写组成，必须是原子的。)

As an example, the allocation of an exclusive reservation of L3 cache
involves:
(例如，分配 L3 缓存的独占保留涉及：)

  1. Read the cbmmasks from each directory or the per-resource "bit_usage"
     (1. 从每个目录或每个资源的 "bit_usage" 读取 cbmmasks)
  2. Find a contiguous set of bits in the global CBM bitmask that is clear
     in any of the directory cbmmasks
     (2. 在全局 CBM 位掩码中找到一组在任何目录 cbmmasks 中均为空闲的连续位)
  3. Create a new directory
     (3. 创建一个新目录)
  4. Set the bits found in step 2 to the new directory "schemata" file
     (4. 将在步骤 2 中找到的位设置到新目录的 "schemata" 文件中)

If two applications attempt to allocate space concurrently then they can
end up allocating the same bits so the reservations are shared instead of
exclusive.
(如果两个应用程序并发尝试分配空间，它们可能最终分配到相同的位，这样保留就变成共享而不是独占。)

To coordinate atomic operations on the resctrlfs and to avoid the problem
above, the following locking procedure is recommended:
(为了协调 resctrlfs 上的原子操作并避免上述问题，建议使用以下锁定过程：)

Locking is based on flock, which is available in libc and also as a shell
script command
(锁定基于 flock，它在 libc 中可用，也可作为 shell 脚本命令使用。)

Write lock:
(写锁：)

 A) Take flock(LOCK_EX) on /sys/fs/resctrl
    (A. 对 /sys/fs/resctrl 获取 flock(LOCK_EX))
 B) Read/write the directory structure.
    (B. 读/写目录结构。)
 C) funlock
    (C. funlock)

Read lock:
(读锁：)

 A) Take flock(LOCK_SH) on /sys/fs/resctrl
    (A. 对 /sys/fs/resctrl 获取 flock(LOCK_SH))
 B) If success read the directory structure.
    (B. 如果成功，读取目录结构。)
 C) funlock
    (C. funlock)

Example with bash::
(bash 示例：)

  # Atomically read directory structure
  $ flock -s /sys/fs/resctrl/ find /sys/fs/resctrl

  # Read directory contents and create new subdirectory

  $ cat create-dir.sh
  find /sys/fs/resctrl/ > output.txt
  mask = function-of(output.txt)
  mkdir /sys/fs/resctrl/newres/
  echo mask > /sys/fs/resctrl/newres/schemata

  $ flock /sys/fs/resctrl/ ./create-dir.sh

Example with C::
(C 示例：)

  /*
  * Example code do take advisory locks
  * before accessing resctrl filesystem
  */
  #include <sys/file.h>
  #include <stdlib.h>

  void resctrl_take_shared_lock(int fd)
  {
    int ret;

    /* take shared lock on resctrl filesystem */
    ret = flock(fd, LOCK_SH);
    if (ret) {
      perror("flock");
      exit(-1);
    }
  }

  void resctrl_take_exclusive_lock(int fd)
  {
    int ret;

    /* release lock on resctrl filesystem */
    ret = flock(fd, LOCK_EX);
    if (ret) {
      perror("flock");
      exit(-1);
    }
  }

  void resctrl_release_lock(int fd)
  {
    int ret;

    /* take shared lock on resctrl filesystem */
    ret = flock(fd, LOCK_UN);
    if (ret) {
      perror("flock");
      exit(-1);
    }
  }

  void main(void)
  {
    int fd, ret;

    fd = open("/sys/fs/resctrl", O_DIRECTORY);
    if (fd == -1) {
      perror("open");
      exit(-1);
    }
    resctrl_take_shared_lock(fd);
    /* code to read directory contents */
    resctrl_release_lock(fd);

    resctrl_take_exclusive_lock(fd);
    /* code to read and write directory contents */
    resctrl_release_lock(fd);
  }

Examples for RDT Monitoring along with allocation usage
(RDT 监控与分配配合使用的示例)
=======================================================
Reading monitored data
(读取监控数据)
----------------------
Reading an event file (for ex: mon_data/mon_L3_00/llc_occupancy) would
show the current snapshot of LLC occupancy of the corresponding MON
group or CTRL_MON group.
(读取事件文件(例如 mon_data/mon_L3_00/llc_occupancy)将显示相应 MON 组或 CTRL_MON 组的 LLC 占用当前快照。)


Example 1 (Monitor CTRL_MON group and subset of tasks in CTRL_MON group)
(示例 1 (监控 CTRL_MON 组及 CTRL_MON 组中任务的子集))
------------------------------------------------------------------------
On a two socket machine (one L3 cache per socket) with just four bits
for cache bit masks::
(在一台双插槽机器(每个插槽一个 L3 缓存)上，缓存位掩码只有 4 位：)

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl
  # mkdir p0 p1
  # echo "L3:0=3;1=c" > /sys/fs/resctrl/p0/schemata
  # echo "L3:0=3;1=3" > /sys/fs/resctrl/p1/schemata
  # echo 5678 > p1/tasks
  # echo 5679 > p1/tasks

The default resource group is unmodified, so we have access to all parts
of all caches (its schemata file reads "L3:0=f;1=f").
(默认资源组未被修改，因此我们可以访问所有缓存的全部部分(其 schemata 文件内容为 "L3:0=f;1=f")。)

Tasks that are under the control of group "p0" may only allocate from the
"lower" 50% on cache ID 0, and the "upper" 50% of cache ID 1.
Tasks in group "p1" use the "lower" 50% of cache on both sockets.
(受组 "p0" 控制的任务只能从缓存 ID 0 的"较低"50% 和缓存 ID 1 的"较高"50% 中分配。组 "p1" 中的任务在两个插槽上都使用"较低"的 50% 缓存。)

Create monitor groups and assign a subset of tasks to each monitor group.
(创建监控组并为每个监控组分配一个任务子集。)
::

  # cd /sys/fs/resctrl/p1/mon_groups
  # mkdir m11 m12
  # echo 5678 > m11/tasks
  # echo 5679 > m12/tasks

fetch data (data shown in bytes)
(获取数据(数据以字节显示))
::

  # cat m11/mon_data/mon_L3_00/llc_occupancy
  16234000
  # cat m11/mon_data/mon_L3_01/llc_occupancy
  14789000
  # cat m12/mon_data/mon_L3_00/llc_occupancy
  16789000

The parent ctrl_mon group shows the aggregated data.
(父 ctrl_mon 组显示聚合后的数据。)
::

  # cat /sys/fs/resctrl/p1/mon_data/mon_l3_00/llc_occupancy
  31234000

Example 2 (Monitor a task from its creation)
(示例 2 (从创建起监控任务))
--------------------------------------------
On a two socket machine (one L3 cache per socket)::
(在一台双插槽机器(每个插槽一个 L3 缓存)上：)

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl
  # mkdir p0 p1

An RMID is allocated to the group once its created and hence the <cmd>
below is monitored from its creation.
(组一旦创建就会分配一个 RMID，因此下面的 <cmd> 从创建起就被监控。)
::

  # echo $$ > /sys/fs/resctrl/p1/tasks
  # <cmd>

Fetch the data::
(获取数据：)

  # cat /sys/fs/resctrl/p1/mon_data/mon_l3_00/llc_occupancy
  31789000

Example 3 (Monitor without CAT support or before creating CAT groups)
(示例 3 (无 CAT 支持时或创建 CAT 组之前进行监控))
---------------------------------------------------------------------

Assume a system like HSW has only CQM and no CAT support. In this case
the resctrl will still mount but cannot create CTRL_MON directories.
But user can create different MON groups within the root group thereby
able to monitor all tasks including kernel threads.
(假设像 HSW 这样的系统只有 CQM 而没有 CAT 支持。在这种情况下，resctrl 仍可挂载，但无法创建 CTRL_MON 目录。但用户可以在根组内创建不同的 MON 组，从而能够监控包括内核线程在内的所有任务。)

This can also be used to profile jobs cache size footprint before being
able to allocate them to different allocation groups.
(这也可用于在将作业分配到不同分配组之前，分析作业的缓存大小占用情况。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl
  # mkdir mon_groups/m01
  # mkdir mon_groups/m02

  # echo 3478 > /sys/fs/resctrl/mon_groups/m01/tasks
  # echo 2467 > /sys/fs/resctrl/mon_groups/m02/tasks

Monitor the groups separately and also get per domain data. From the
below its apparent that the tasks are mostly doing work on
domain(socket) 0.
(分别监控这些组并获得每个域的数据。从下面可以明显看出，这些任务主要在域(插槽)0 上工作。)
::

  # cat /sys/fs/resctrl/mon_groups/m01/mon_L3_00/llc_occupancy
  31234000
  # cat /sys/fs/resctrl/mon_groups/m01/mon_L3_01/llc_occupancy
  34555
  # cat /sys/fs/resctrl/mon_groups/m02/mon_L3_00/llc_occupancy
  31234000
  # cat /sys/fs/resctrl/mon_groups/m02/mon_L3_01/llc_occupancy
  32789


Example 4 (Monitor real time tasks)
(示例 4 (监控实时任务))
-----------------------------------

A single socket system which has real time tasks running on cores 4-7
and non real time tasks on other cpus. We want to monitor the cache
occupancy of the real time threads on these cores.
(单插槽系统，实时任务运行在核心 4-7 上，非实时任务运行在其他 CPU 上。我们想监控这些核心上实时线程的缓存占用。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl
  # cd /sys/fs/resctrl
  # mkdir p1

Move the cpus 4-7 over to p1::
(将 CPU 4-7 移到 p1：)

  # echo f0 > p1/cpus

View the llc occupancy snapshot::
(查看 LLC 占用快照：)

  # cat /sys/fs/resctrl/p1/mon_data/mon_L3_00/llc_occupancy
  11234000


Examples on working with mbm_assign_mode
(使用 mbm_assign_mode 的示例)
========================================

a. Check if MBM counter assignment mode is supported.
   (a. 检查是否支持 MBM 计数器分配模式。)
::

  # mount -t resctrl resctrl /sys/fs/resctrl/

  # cat /sys/fs/resctrl/info/L3_MON/mbm_assign_mode
  [mbm_event]
  default

The "mbm_event" mode is detected and enabled.
(检测到并启用了 "mbm_event" 模式。)

b. Check how many assignable counters are supported.
   (b. 检查支持多少个可分配计数器。)
::

  # cat /sys/fs/resctrl/info/L3_MON/num_mbm_cntrs
  0=32;1=32

c. Check how many assignable counters are available for assignment in each domain.
   (c. 检查每个域中有多少个可分配计数器可供分配。)
::

  # cat /sys/fs/resctrl/info/L3_MON/available_mbm_cntrs
  0=30;1=30

d. To list the default group's assign states.
   (d. 列出默认组的分配状态。)
::

  # cat /sys/fs/resctrl/mbm_L3_assignments
  mbm_total_bytes:0=e;1=e
  mbm_local_bytes:0=e;1=e

e.  To unassign the counter associated with the mbm_total_bytes event on domain 0.
    (e. 取消域 0 上与 mbm_total_bytes 事件关联的计数器的分配。)
::

  # echo "mbm_total_bytes:0=_" > /sys/fs/resctrl/mbm_L3_assignments
  # cat /sys/fs/resctrl/mbm_L3_assignments
  mbm_total_bytes:0=_;1=e
  mbm_local_bytes:0=e;1=e

f. To unassign the counter associated with the mbm_total_bytes event on all domains.
   (f. 取消所有域上与 mbm_total_bytes 事件关联的计数器的分配。)
::

  # echo "mbm_total_bytes:*=_" > /sys/fs/resctrl/mbm_L3_assignments
  # cat /sys/fs/resctrl/mbm_L3_assignment
  mbm_total_bytes:0=_;1=_
  mbm_local_bytes:0=e;1=e

g. To assign a counter associated with the mbm_total_bytes event on all domains in
exclusive mode.
   (g. 以独占模式为所有域分配与 mbm_total_bytes 事件关联的计数器。)
::

  # echo "mbm_total_bytes:*=e" > /sys/fs/resctrl/mbm_L3_assignments
  # cat /sys/fs/resctrl/mbm_L3_assignments
  mbm_total_bytes:0=e;1=e
  mbm_local_bytes:0=e;1=e

h. Read the events mbm_total_bytes and mbm_local_bytes of the default group. There is
no change in reading the events with the assignment.
   (h. 读取默认组的 mbm_total_bytes 和 mbm_local_bytes 事件。分配后读取这些事件没有变化。)
::

  # cat /sys/fs/resctrl/mon_data/mon_L3_00/mbm_total_bytes
  779247936
  # cat /sys/fs/resctrl/mon_data/mon_L3_01/mbm_total_bytes
  562324232
  # cat /sys/fs/resctrl/mon_data/mon_L3_00/mbm_local_bytes
  212122123
  # cat /sys/fs/resctrl/mon_data/mon_L3_01/mbm_local_bytes
  121212144

i. Check the event configurations.
   (i. 检查事件配置。)
::

  # cat /sys/fs/resctrl/info/L3_MON/event_configs/mbm_total_bytes/event_filter
  local_reads,remote_reads,local_non_temporal_writes,remote_non_temporal_writes,
  local_reads_slow_memory,remote_reads_slow_memory,dirty_victim_writes_all

  # cat /sys/fs/resctrl/info/L3_MON/event_configs/mbm_local_bytes/event_filter
  local_reads,local_non_temporal_writes,local_reads_slow_memory

j. Change the event configuration for mbm_local_bytes.
   (j. 更改 mbm_local_bytes 的事件配置。)
::

  # echo "local_reads, local_non_temporal_writes, local_reads_slow_memory, remote_reads" >
  /sys/fs/resctrl/info/L3_MON/event_configs/mbm_local_bytes/event_filter

  # cat /sys/fs/resctrl/info/L3_MON/event_configs/mbm_local_bytes/event_filter
  local_reads,local_non_temporal_writes,local_reads_slow_memory,remote_reads

k. Now read the local events again. The first read may come back with "Unavailable"
status. The subsequent read of mbm_local_bytes will display the current value.
   (k. 现在再次读取本地事件。第一次读取可能返回 "Unavailable" 状态。随后读取 mbm_local_bytes 将显示当前值。)
::

  # cat /sys/fs/resctrl/mon_data/mon_L3_00/mbm_local_bytes
  Unavailable
  # cat /sys/fs/resctrl/mon_data/mon_L3_00/mbm_local_bytes
  2252323
  # cat /sys/fs/resctrl/mon_data/mon_L3_01/mbm_local_bytes
  Unavailable
  # cat /sys/fs/resctrl/mon_data/mon_L3_01/mbm_local_bytes
  1566565

l. Users have the option to go back to 'default' mbm_assign_mode if required. This can be
done using the following command. Note that switching the mbm_assign_mode may reset all
the MBM counters (and thus all MBM events) of all the resctrl groups.
   (l. 如有需要，用户可以选择回到 'default' mbm_assign_mode。可以使用以下命令完成。注意，切换 mbm_assign_mode 可能会重置所有 resctrl 组的所有 MBM 计数器(进而所有 MBM 事件)。)
::

  # echo "default" > /sys/fs/resctrl/info/L3_MON/mbm_assign_mode
  # cat /sys/fs/resctrl/info/L3_MON/mbm_assign_mode
  mbm_event
  [default]

m. Unmount the resctrl filesystem.
   (m. 卸载 resctrl 文件系统。)
::

  # umount /sys/fs/resctrl/

Intel RDT Errata
(Intel RDT 勘误)
================

Intel MBM Counters May Report System Memory Bandwidth Incorrectly
(Intel MBM 计数器可能不正确地报告系统内存带宽)
-----------------------------------------------------------------

Errata SKX99 for Skylake server and BDF102 for Broadwell server.
(Skylake 服务器的勘误 SKX99 与 Broadwell 服务器的勘误 BDF102。)

Problem: Intel Memory Bandwidth Monitoring (MBM) counters track metrics
according to the assigned Resource Monitor ID (RMID) for that logical
core. The IA32_QM_CTR register (MSR 0xC8E), used to report these
metrics, may report incorrect system bandwidth for certain RMID values.
(问题：Intel 内存带宽监控(MBM)计数器根据分配给该逻辑核心的资源监控 ID(RMID)来跟踪指标。用于报告这些指标的 IA32_QM_CTR 寄存器(MSR 0xC8E)可能对某些 RMID 值报告不正确的系统带宽。)

Implication: Due to the errata, system memory bandwidth may not match
what is reported.
(影响：由于该勘误，系统内存带宽可能与报告的值不一致。)

Workaround: MBM total and local readings are corrected according to the
following correction factor table:
(解决方法：MBM total 和 local 读数根据以下修正系数表进行修正：)

+---------------+---------------+---------------+-----------------+
|core count (核心数)	|rmid count (RMID 数)	|rmid threshold (RMID 阈值)	|correction factor (修正系数)|
+---------------+---------------+---------------+-----------------+
|1		|8		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|2		|16		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|3		|24		|15		|0.969650	  |
+---------------+---------------+---------------+-----------------+
|4		|32		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|6		|48		|31		|0.969650	  |
+---------------+---------------+---------------+-----------------+
|7		|56		|47		|1.142857	  |
+---------------+---------------+---------------+-----------------+
|8		|64		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|9		|72		|63		|1.185115	  |
+---------------+---------------+---------------+-----------------+
|10		|80		|63		|1.066553	  |
+---------------+---------------+---------------+-----------------+
|11		|88		|79		|1.454545	  |
+---------------+---------------+---------------+-----------------+
|12		|96		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|13		|104		|95		|1.230769	  |
+---------------+---------------+---------------+-----------------+
|14		|112		|95		|1.142857	  |
+---------------+---------------+---------------+-----------------+
|15		|120		|95		|1.066667	  |
+---------------+---------------+---------------+-----------------+
|16		|128		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|17		|136		|127		|1.254863	  |
+---------------+---------------+---------------+-----------------+
|18		|144		|127		|1.185255	  |
+---------------+---------------+---------------+-----------------+
|19		|152		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|20		|160		|127		|1.066667	  |
+---------------+---------------+---------------+-----------------+
|21		|168		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|22		|176		|159		|1.454334	  |
+---------------+---------------+---------------+-----------------+
|23		|184		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|24		|192		|127		|0.969744	  |
+---------------+---------------+---------------+-----------------+
|25		|200		|191		|1.280246	  |
+---------------+---------------+---------------+-----------------+
|26		|208		|191		|1.230921	  |
+---------------+---------------+---------------+-----------------+
|27		|216		|0		|1.000000	  |
+---------------+---------------+---------------+-----------------+
|28		|224		|191		|1.143118	  |
+---------------+---------------+---------------+-----------------+

If rmid > rmid threshold, MBM total and local values should be multiplied
by the correction factor.
(如果 rmid > rmid 阈值，MBM total 和 local 值应乘以修正系数。)

See:
(参见：)

1. Erratum SKX99 in Intel Xeon Processor Scalable Family Specification Update:
http://web.archive.org/web/20200716124958/https://www.intel.com/content/www/us/en/processors/xeon/scalable/xeon-scalable-spec-update.html
   (1. Intel Xeon 可扩展处理器家族规范更新中的勘误 SKX99：)

2. Erratum BDF102 in Intel Xeon E5-2600 v4 Processor Product Family Specification Update:
http://web.archive.org/web/20191125200531/https://www.intel.com/content/dam/www/public/us/en/documents/specification-updates/xeon-e5-v4-spec-update.pdf
   (2. Intel Xeon E5-2600 v4 处理器产品家族规范更新中的勘误 BDF102：)

3. The errata in Intel Resource Director Technology (Intel RDT) on 2nd Generation Intel Xeon Scalable Processors Reference Manual:
https://software.intel.com/content/www/us/en/develop/articles/intel-resource-director-technology-rdt-reference-manual.html
   (3. 第二代 Intel Xeon 可扩展处理器参考手册中 Intel Resource Director Technology (Intel RDT) 的勘误：)

for further information.
(以了解更多信息。)
