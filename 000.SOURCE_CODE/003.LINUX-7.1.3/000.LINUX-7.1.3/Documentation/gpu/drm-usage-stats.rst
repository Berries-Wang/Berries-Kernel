.. _drm-client-usage-stats:

==========================================
DRM client usage stats(DRM 客户端使用统计)
==========================================

DRM drivers can choose to export partly standardised text output via the
`fops->show_fdinfo()` as part of the driver specific file operations registered
in the `struct drm_driver` object registered with the DRM core.
(DRM 驱动可以选择通过 `fops->show_fdinfo()` 导出部分标准化的文本输出，作为向 DRM 核心注册的 `struct drm_driver` 对象中驱动特定文件操作的一部分。)

One purpose of this output is to enable writing as generic as practically
feasible `top(1)` like userspace monitoring tools.
(该输出的目的之一，是让用户空间能够编写尽可能通用的、类似 `top(1)` 的监控工具。)

Given the differences between various DRM drivers the specification of the
output is split between common and driver specific parts. Having said that,
wherever possible effort should still be made to standardise as much as
possible.
(考虑到各 DRM 驱动之间的差异，输出的规范分为通用部分和驱动特定部分。话虽如此，仍应尽可能地努力做到最大程度的标准化。)

File format specification(文件格式规范)
=======================================

- File shall contain one key value pair per one line of text.
  (文件中每一行文本包含一个键值对。)
- Colon character (`:`) must be used to delimit keys and values.
  (必须使用冒号字符(`:`)来分隔键和值。)
- All standardised keys shall be prefixed with `drm-`.
  (所有标准化的键都应以 `drm-` 为前缀。)
- Driver-specific keys shall be prefixed with `driver_name-`, where
  driver_name should ideally be the same as the `name` field in
  `struct drm_driver`, although this is not mandatory.
  (驱动特有的键应以 `driver_name-` 为前缀，其中 driver_name 理想情况下应与 `struct drm_driver` 中的 `name` 字段相同，但这不是强制要求。)
- Whitespace between the delimiter and first non-whitespace character shall be
  ignored when parsing.
  (解析时应忽略分隔符与第一个非空白字符之间的空白。)
- Keys are not allowed to contain whitespace characters.
  (键中不允许包含空白字符。)
- Numerical key value pairs can end with optional unit string.
  (数值型键值对可以以可选的单位字符串结尾。)
- Data type of the value is fixed as defined in the specification.
  (值的数据类型按本规范中的定义固定不变。)

Key types(键类型)
-----------------

1. Mandatory, fully standardised.
2. Optional, fully standardised.
3. Driver specific.
(1. 强制，完全标准化。)
(2. 可选，完全标准化。)
(3. 驱动特有。)

Data types(数据类型)
--------------------

- <uint> - Unsigned integer without defining the maximum value.
  (<uint> - 未定义最大值的无符号整数。)
- <keystr> - String excluding any above defined reserved characters or whitespace.
  (<keystr> - 不含上述任何保留字符或空白的字符串。)
- <valstr> - String.
  (<valstr> - 字符串。)

Mandatory fully standardised keys(强制性完全标准化键)
-----------------------------------------------------

- drm-driver: <valstr>

String shall contain the name this driver registered as via the respective
`struct drm_driver` data structure.
(该字符串应包含此驱动通过相应 `struct drm_driver` 数据结构注册的名称。)

Optional fully standardised keys(可选的完全标准化键)
----------------------------------------------------

Identification(标识类)
^^^^^^^^^^^^^^^^^^^^^^

- drm-pdev: <aaaa:bb.cc.d>

For PCI devices this should contain the PCI slot address of the device in
question.
(对于 PCI 设备，此项应包含相应设备的 PCI 槽位地址。)

- drm-client-id: <uint>

Unique value relating to the open DRM file descriptor used to distinguish
duplicated and shared file descriptors. Conceptually the value should map 1:1
to the in kernel representation of `struct drm_file` instances.
(与已打开的 DRM 文件描述符相关的唯一值，用于区分重复和共享的文件描述符。从概念上讲，该值应与 `struct drm_file` 实例的内核表示一一对应。)

Uniqueness of the value shall be either globally unique, or unique within the
scope of each device, in which case `drm-pdev` shall be present as well.
(该值的唯一性可以是全局唯一，也可以是在每个设备范围内唯一；若是后者，则还应同时提供 `drm-pdev`。)

Userspace should make sure to not double account any usage statistics by using
the above described criteria in order to associate data to individual clients.
(用户空间应利用上述标准将数据关联到各个客户端，确保不重复统计任何使用数据。)

- drm-client-name: <valstr>

String optionally set by userspace using DRM_IOCTL_SET_CLIENT_NAME.
(用户空间可通过 DRM_IOCTL_SET_CLIENT_NAME 可选地设置的字符串。)


Utilization(利用率)
^^^^^^^^^^^^^^^^^^^

- drm-engine-<keystr>: <uint> ns

GPUs usually contain multiple execution engines. Each shall be given a stable
and unique name (keystr), with possible values documented in the driver specific
documentation.
(GPU 通常包含多个执行引擎。每个引擎都应获得一个稳定且唯一的名称(keystr)，可能的取值记录在驱动各自的文档中。)

Value shall be in specified time units which the respective GPU engine spent
busy executing workloads belonging to this client.
(该值应为相应 GPU 引擎忙于执行属于该客户端的工作负载所耗费的时间(按指定时间单位)。)

Values are not required to be constantly monotonic if it makes the driver
implementation easier, but are required to catch up with the previously reported
larger value within a reasonable period. Upon observing a value lower than what
was previously read, userspace is expected to stay with that larger previous
value until a monotonic update is seen.
(如果能让驱动实现更容易，这些值不要求始终单调递增，但必须在合理的时间段内追回先前报告的较大值。当观察到低于先前读取值的情况时，用户空间应继续使用先前较大的值，直到看到单调递增的更新。)

- drm-engine-capacity-<keystr>: <uint>

Engine identifier string must be the same as the one specified in the
drm-engine-<keystr> tag and shall contain a greater than zero number in case the
exported engine corresponds to a group of identical hardware engines.
(引擎标识字符串必须与 drm-engine-<keystr> 标签中指定的相同；当导出的引擎对应于一组相同硬件引擎时，该项应包含一个大于零的数值。)

In the absence of this tag parser shall assume capacity of one. Zero capacity
is not allowed.
(若缺少此标签，解析器应假定容量为 1。不允许容量为零。)

- drm-cycles-<keystr>: <uint>

Engine identifier string must be the same as the one specified in the
drm-engine-<keystr> tag and shall contain the number of busy cycles for the given
engine.
(引擎标识字符串必须与 drm-engine-<keystr> 标签中指定的相同；该项应包含给定引擎的忙周期数。)

Values are not required to be constantly monotonic if it makes the driver
implementation easier, but are required to catch up with the previously reported
larger value within a reasonable period. Upon observing a value lower than what
was previously read, userspace is expected to stay with that larger previous
value until a monotonic update is seen.
(如果能让驱动实现更容易，这些值不要求始终单调递增，但必须在合理的时间段内追回先前报告的较大值。当观察到低于先前读取值的情况时，用户空间应继续使用先前较大的值，直到看到单调递增的更新。)

- drm-total-cycles-<keystr>: <uint>

Engine identifier string must be the same as the one specified in the
drm-cycles-<keystr> tag and shall contain the total number cycles for the given
engine.
(引擎标识字符串必须与 drm-cycles-<keystr> 标签中指定的相同；该项应包含给定引擎的总周期数。)

This is a timestamp in GPU unspecified unit that matches the update rate
of drm-cycles-<keystr>. For drivers that implement this interface, the engine
utilization can be calculated entirely on the GPU clock domain, without
considering the CPU sleep time between 2 samples.
(这是一个以 GPU 未指定单位表示的时间戳，其更新频率与 drm-cycles-<keystr> 相匹配。对于实现此接口的驱动，引擎利用率可以完全在 GPU 时钟域上计算，而无需考虑两次采样之间的 CPU 休眠时间。)

A driver may implement either this key or drm-maxfreq-<keystr>, but not both.
(驱动可以实现此键或 drm-maxfreq-<keystr> 两者之一，但不能同时实现两者。)

- drm-maxfreq-<keystr>: <uint> [Hz|MHz|KHz]

Engine identifier string must be the same as the one specified in the
drm-engine-<keystr> tag and shall contain the maximum frequency for the given
engine.  Taken together with drm-cycles-<keystr>, this can be used to calculate
percentage utilization of the engine, whereas drm-engine-<keystr> only reflects
time active without considering what frequency the engine is operating as a
percentage of its maximum frequency.
(引擎标识字符串必须与 drm-engine-<keystr> 标签中指定的相同；该项应包含给定引擎的最大频率。结合 drm-cycles-<keystr>，可用于计算引擎利用率的百分比；而 drm-engine-<keystr> 只反映活跃时间，未考虑引擎运行频率占其最大频率的百分比。)

A driver may implement either this key or drm-total-cycles-<keystr>, but not
both.
(驱动可以实现此键或 drm-total-cycles-<keystr> 两者之一，但不能同时实现两者。)

Memory(内存)
^^^^^^^^^^^^

Each possible memory type which can be used to store buffer objects by the GPU
in question shall be given a stable and unique name to be used as the "<region>"
string.
(相关 GPU 可用来存储缓冲区对象的每种内存类型，都应获得一个稳定且唯一的名称，用作 "<region>" 字符串。)

The region name "memory" is reserved to refer to normal system memory.
(区域名 "memory" 保留用于指代普通系统内存。)

The value shall reflect the amount of storage currently consumed by the buffer
objects belong to this client, in the respective memory region.
(该值应反映属于该客户端的缓冲区对象当前在相应内存区域中消耗的存储量。)

Default unit shall be bytes with optional unit specifiers of 'KiB' or 'MiB'
indicating kibi- or mebi-bytes.
(默认单位为字节，可选的单位说明符 'KiB' 或 'MiB' 分别表示 kibibyte 或 mebibyte。)

- drm-total-<region>: <uint> [KiB|MiB]

The total size of all requested buffers, including both shared and private
memory. The backing store for the buffers does not need to be currently
instantiated to count under this category. To avoid double-counting, if a buffer
has multiple regions where it can be allocated to, the implementation should
consistently select a single region for accounting purposes.
(所有已请求缓冲区的总大小，包括共享内存和私有内存。缓冲区的后备存储即使当前尚未实例化，也可计入此类别。为避免重复计数，若一个缓冲区有多个可分配区域，实现应始终如一地为记账目的选择单一区域。)

- drm-shared-<region>: <uint> [KiB|MiB]

The total size of buffers that are shared with another file (i.e., have more
than one handle). The same requirement to avoid double-counting that applies to
drm-total-<region> also applies here.
(与其他文件共享(即拥有多于一个句柄)的缓冲区的总大小。适用于 drm-total-<region> 的避免重复计数要求同样适用于此项。)

- drm-resident-<region>: <uint> [KiB|MiB]

The total size of buffers that are resident (i.e., have their backing store
present or instantiated) in the specified region.
(在指定区域内驻留(即后备存储已存在或已实例化)的缓冲区的总大小。)

- drm-memory-<region>: <uint> [KiB|MiB]

This key is deprecated and is only printed by amdgpu; it is an alias for
drm-resident-<region>.
(此键已废弃，目前仅由 amdgpu 打印；它是 drm-resident-<region> 的别名。)

- drm-purgeable-<region>: <uint> [KiB|MiB]

The total size of buffers that are resident and purgeable.
(驻留且可清除(purgeable)的缓冲区的总大小。)

For example, drivers that implement functionality similar to 'madvise' can count
buffers that have instantiated backing stores but have been marked with an
equivalent of MADV_DONTNEED.
(例如，实现了类似 'madvise' 功能的驱动，可以统计那些已实例化后备存储、但已被标记为等效于 MADV_DONTNEED 的缓冲区。)

- drm-active-<region>: <uint> [KiB|MiB]

The total size of buffers that are active on one or more engines.
(在一个或多个引擎上处于活跃状态的缓冲区的总大小。)

One practical example of this could be the presence of unsignaled fences in a
GEM buffer reservation object. Therefore, the active category is a subset of the
resident category.
(一个实际的例子是 GEM 缓冲区预留对象中存在未发信号的 fence。因此，活跃类别是驻留类别的子集。)

Implementation Details(实现细节)
================================

Drivers should use drm_show_fdinfo() in their `struct file_operations`, and
implement &drm_driver.show_fdinfo if they wish to provide any stats which
are not provided by drm_show_fdinfo().  But even driver specific stats should
be documented above and where possible, aligned with other drivers.
(驱动应在其 `struct file_operations` 中使用 drm_show_fdinfo()，如果希望提供 drm_show_fdinfo() 未提供的任何统计信息，则实现 &drm_driver.show_fdinfo。但即便是驱动特有的统计信息，也应在上文加以记录，并在可能的情况下与其他驱动保持一致。)

Driver specific implementations(驱动特定实现)
----------------------------------------------

* :ref:`i915-usage-stats`
* :ref:`panfrost-usage-stats`
* :ref:`panthor-usage-stats`
* :ref:`xe-usage-stats`
