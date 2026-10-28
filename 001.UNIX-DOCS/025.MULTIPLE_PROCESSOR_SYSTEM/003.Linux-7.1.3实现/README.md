# Linux 7.1.3 中的 SMP/NUMA 实现（理论 ↔ 源码对照）

> 理论出处：[现代操作系统 第5版 §8.1](../../../007.BOOKs/现代操作系统/Modern%20Operating%20Systems%205th%20Edition-PDF-01.pdf)，总结见 [../002.多处理器理论/README.md](../002.多处理器理论/README.md)。
> 源码树：[000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3)。本文所有 `文件:行号` 均已在该树中核对。

## 0. 总览：书中模型 → Linux 选择

| 书中理论（§8.1）| Linux 的选择 |
|-|-|
| OS 类型：每 CPU 独立 OS / 主从 / **SMP** | **SMP 模型**：一份内核映像，任意 CPU 都能陷入内核执行；历史上曾有大内核锁（BKL，2.6.39 移除），现在是细粒度锁（书中"独立临界区 + 各自 mutex"的成熟形态）|
| 缓存一致：侦听 / 目录 | 全是**硬件实现**（总线侦听、UPI/xGMI 内的目录协议），内核只是拓扑信息的**消费者** |
| 同步：总线锁 TSL、先读后锁、退避、MCS 队列 | 原子指令（x86 `LOCK` 前缀、arm64 LSE）+ **qspinlock（MCS 思想的排队自旋锁）** + **osq 乐观自旋（先自旋后睡眠，即书中 spin vs switch 的工程答案）** |
| 调度：分时 / 亲和性 / 空间共享 / 组调度 | **每 CPU 运行队列 + 调度域层次（SMT/MC/PKG/NUMA）+ 负载均衡 + 亲和性掩码**；无 gang scheduling，但有面向安全的 core scheduling |
| NUMA：本地快远程慢 | 节点位图 + `pglist_data` + 内存策略 + **NUMA balancing 自动迁移** |

---

## 1. CPU 的发现与管理（cpumask 四状态）

书中说"调度有两个维度：跑哪个线程、放在哪个 CPU"，Linux 对第二个维度的第一步就是把 CPU 管起来：

- 状态位图及语义（7.1.3 为五段生命周期 possible→present→enabled→online→active，另有 `cpu_dying_mask`；经典资料常说的"四状态"少了 enabled）：<sub>[/include/linux/cpumask.h:84](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/cpumask.h)</sub>
- **arm64 枚举流程**：`setup_arch()` <sub>[/arch/arm64/kernel/setup.c:360](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/setup.c)</sub> → `smp_init_cpus()` <sub>[/arch/arm64/kernel/smp.c:739](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/smp.c)</sub>
  - 设备树路径 `of_parse_and_init_cpus()` / ACPI 路径 `acpi_parse_and_init_cpus()`（对应书中"通过 ACPI 或 DTS 获取 CPU 数量"）
  - 逐 CPU 调 `smp_cpu_setup()` <sub>[/arch/arm64/kernel/smp.c:488](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/smp.c)</sub> → `set_cpu_possible(cpu, true)` 置入 `cpu_possible_mask`
- x86 对应逻辑在 `arch/x86/kernel/smpboot.c`（MADT 枚举 + 二级 CPU 上线）；possible→present→online→active 由 CPU 热插拔（cpuhp）状态机推进 <sub>[/kernel/cpu.c](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/cpu.c)</sub>

## 2. 调度域：Linux 版的"两级调度 + 亲和性调度"

书中"两级调度（顶层分配线程到 CPU、底层各自调度、空闲可偷线程）"和"亲和性调度（尽量回到热缓存所在 CPU）"，在 Linux 中由**调度域（sched_domain）**体系落实：

- 拓扑层级数据结构 `sched_domain_topology_level`（即 SDTL）：<sub>[/include/linux/sched/topology.h:191](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/topology.h)</sub>
- 默认拓扑（从 cache/LLC 共享关系推导，而非硬编码 CPU 档位）：<sub>[/kernel/sched/topology.c:1803](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/topology.c)</sub>
  `default_topology[]` 依次注册 **SMT**(超线程,:1805) → **CLS**(cluster,:1809) → **MC**(LLC 共,:1813) → **PKG**(物理封装,:1815)；NUMA 系统再追加 **NODE**(:2132) 与更多 **NUMA** 层级(:2138)。
  - SMT/MC 层 = 书中"共享缓存尽量靠近调度"的体现；NUMA 层 = 书中"NUMA 架构下访问近端内存"的调度侧应对。
- 每 CPU 一个运行队列 `struct rq`（`runqueues` 为 per-CPU 变量）：<sub>[/kernel/sched/sched.h:1131](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/sched.h)</sub>、<sub>[:1386](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/sched.h)</sub> —— 正是书中"每个 CPU 自己的就绪表"；空闲 CPU 在调度域内**拉取/偷取**任务 = 负载均衡（详见 [031.进程管理/008.进程调度/003.SMP负载均衡](../../031.进程管理/008.进程调度/003.SMP负载均衡)）。
- **亲和性调度**：任务携带 `cpus_mask` 掩码，调度器只在掩码内选 CPU <sub>[/include/linux/sched.h:927](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched.h)</sub>；用户态用 `sched_setaffinity(2)` 修改 <sub>[/kernel/sched/syscalls.c:1262](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/syscalls.c)</sub>。
- **空间共享的类比**：cpuset/cgroup v2 `cpuset.cpus` 把 CPU 划成互不重叠的分区给不同组使用，即书中"把 CPU 静态分区给一组线程"的通用化；cgroup 也可以动态调整（对应"中央服务器管理并行度"）。
- **组调度**：内核 CFS 没有书中所述 gang scheduling（同起同止时间片）；用户视角的"组"是 cgroup 组调度（按组公平），见 [031.进程管理/008.进程调度/003.组调度.md](../../031.进程管理/008.进程调度/003.组调度.md)。

## 3. 多处理器同步：从 TSL 到 qspinlock

书中 8.1.3 的演进路线，与 Linux 锁实现的对应关系：

| 书中概念 | Linux 实现 |
|-|-|
| 总线锁定的 TSL/原子"读+置1" | 原子指令：x86 `LOCK CMPXCHG/XCHG`（旧总线锁语义进化为缓存行锁），arm64 LSE 原子指令（`LDXR/STXR` 独占对 / `SWPAL` 等）|
| Mellor-Crummey–Scott 私有锁队列（每个等待者自旋自己的变量，图 8-11）| `kernel/locking/mcs_spinlock.h:57` 的 `mcs_spin_lock()`——教科书方案在内核中的直接实现 |
| 高效自旋锁成品 | **qspinlock** <sub>[/kernel/locking/qspinlock.c](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/locking/qspinlock.c)</sub>：锁字 + MCS 队列（pending 位先乐观自旋、再入队）；x86 与 arm64 均默认启用（两架构都 `select ARCH_USE_QUEUED_SPINLOCKS`）|
| 先读探测再上锁（减少缓存行乒乓）| qspinlock 的 pending 乐观路径、以及 `atomic_read` 先行探测等技巧 |
| spin vs switch（先自旋一段时间、不划算再睡眠）| **osq（optimistic spin queue）** <sub>[/kernel/locking/osq_lock.c](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/locking/osq_lock.c)</sub> + mutex/rwsem 的乐观自旋（`osq_lock` 失败/超时才真正睡眠，对应书中"记录上次自旋时长做预测"的工程化版本）|
| MONITOR/MWAIT 省电等待 | x86 有 `monitor/mwait` 支持的优化等待（如 mwaitx/umwait 相关路径），arm64 对应 WFE |

## 4. NUMA 的内存视角

Linux 对"Node = Socket + 本地内存"的软件抽象（与硬件目录一致性协议无关）：

- **节点状态位图** `enum node_states`（N_POSSIBLE/N_ONLINE/N_NORMAL_MEMORY/N_CPU...）：<sub>[/include/linux/nodemask.h:384](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/nodemask.h)</sub>——与 cpumask 四状态同构。
- **节点描述符** `pglist_data`（zonelist、`node_mem_map` 等）：<sub>[/include/linux/mmzone.h:1473](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/mmzone.h)</sub>，每个节点独立管理自己的页面。
- **CPU↔Node 映射**：`numa_node_id()` / `cpu_to_node()` <sub>[/include/linux/topology.h:84](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/topology.h)</sub>；arm64 启动时 `numa_store_cpu_info()` / `numa_add_cpu()` 建立 <sub>[/arch/arm64/kernel/smp.c:783](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/smp.c)</sub>，ACPI SRAT/slit 解析在 <sub>[/arch/arm64/kernel/acpi_numa.c](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/acpi_numa.c)</sub>。
- **内存策略（手动）**：`mm/mempolicy.c`——`set_mempolicy(2)`/`mbind(2)` 的 MPOL_BIND / MPOL_INTERLEAVE / MPOL_PREFERRED，numactl 命令即基于此（书中"把数据放在靠近使用它的 CPU"的显式手段）。
- **NUMA balancing（自动，CONFIG_NUMA_BALANCING）**：通过页错误采样统计任务的"理想节点"，自动把任务和内存迁回本地：
  - `task_numa_migrate()` <sub>[/kernel/sched/fair.c:2683](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/fair.c)</sub>、`task_numa_group()` <sub>[:3198](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/fair.c)</sub>、`init_numa_balancing()` <sub>[:3741](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/fair.c)</sub>
  - 开关：`/proc/sys/kernel/numa_balancing`（sysctl 注册在 <sub>[/kernel/sched/core.c:4667](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/core.c)</sub>）
- 分配器侧的本地倾斜：页面分配从本节点 zone 开始（`alloc_pages` 按 zonelist 顺序，节点内优先）。

## 5. 安全与 SMT：core scheduling

书中 8.1.4 末尾"面向安全的调度"（不同安全域的线程不共核、宁可空着超线程），Linux 的实现是 **core scheduling**（`CONFIG_SCHED_CORE`）：

- 代码：<sub>[/kernel/sched/core.c:195](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/core.c)</sub> 起（`sched_core_*` 系列，cookie 机制让"同 cookie"的任务才能同核并跑）
- 用户接口：`prctl(PR_SCHED_CORE, ...)`；容器/VM 场景用 cookie 隔离，等价于书中 Hyper-V 核调度器"不同 VM 不共核"的思路
- 相关讨论见 [031.进程管理/008.进程调度/008.SMT](../../031.进程管理/008.进程调度/008.SMT)

## 6. 快速索引（本目录相关源码文件）

| 主题 | 文件 |
|-|-|
| CPU 位图 | `include/linux/cpumask.h`、`kernel/cpu.c` |
| SMP 启动（arm64）| `arch/arm64/kernel/smp.c`、`setup.c`、`psci.c` |
| 拓扑/调度域 | `include/linux/sched/topology.h`、`kernel/sched/topology.c`、`drivers/base/arch_topology.c:933 init_cpu_topology()` |
| 运行队列/负载均衡 | `kernel/sched/sched.h`（rq）、`kernel/sched/fair.c` |
| 亲和性 | `kernel/sched/syscalls.c:1262`、`include/linux/sched.h:927` |
| 自旋锁 | `kernel/locking/qspinlock.c`、`mcs_spinlock.h`、`osq_lock.c` |
| NUMA 内存 | `include/linux/nodemask.h`、`include/linux/mmzone.h`、`mm/mempolicy.c`、`mm/memory_hotplug.c` |
| NUMA balancing | `kernel/sched/fair.c` |
| core scheduling | `kernel/sched/core.c`（CONFIG_SCHED_CORE）|
