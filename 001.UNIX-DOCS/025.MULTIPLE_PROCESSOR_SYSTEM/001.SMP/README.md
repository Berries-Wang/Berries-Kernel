# SMP(对称多处理器.Symmetric Multi-Processor)
> 在Linux内核中，SMP和NUMA是可同时存在的，且是共生的: NUMA 是多个 SMP 节点通过高速互联组成的更大系统 , 见:<br/>- [003.SMP负载均衡](../../../001.UNIX-DOCS/031.进程管理/008.进程调度/003.SMP负载均衡) <br/>- [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#图8.18　调度域和调度组的关系]

> 学习:<br/> - [RISC-V Architecture Programming and Practice: 图11.2 UMA体系结构](../../../007.BOOKs/RISC-V%20Architecture%20Programming%20and%20Practice/RISC-V%20Architecture%20Programming%20and%20Practice-231-464.pdf) <br/> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#8.3　SMP负载均衡 <br/> - [现代操作系统(第5版)#8.1 MULTIPROCESSORS(PDF P559-586)](../../../007.BOOKs/现代操作系统/Modern%20Operating%20Systems%205th%20Edition-PDF-01.pdf)

> 本目录偏概念；完整理论（UMA/NUMA 硬件、多处理器 OS 三种模型、同步、调度）见 [../002.多处理器理论/README.md](../002.多处理器理论/README.md)，内核侧实现见 [../003.Linux-7.1.3实现/README.md](../003.Linux-7.1.3实现/README.md)

|项|说明|备注|
|-|-|-|
|特点|所有CPU共享同一个内存和I/O，并且每个CPU访问内存的速度是一样的|严格说这是 UMA 的特征，SMP 强调的是所有 CPU 地位对等、运行同一份内核|
|与UMA的关系|SMP 通常基于 UMA 架构实现，但两者不等价：双路/多路服务器是对称的（SMP），却是 NUMA|见上方"SMP与NUMA共生"|
|SDTL|Linux内核通过数据结构 `sched_domain_topology_level` 来描述CPU的层次关系，本节将其简称为SDTL|[/include/linux/sched/topology.h:191](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/topology.h)|

---

## 内核如何管理 CPU（Linux 7.1.3）

内核对CPU的管理是通过位图（bitmap）变量来管理的 <sub>[/include/linux/cpumask.h:84](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/cpumask.h)</sub>。经典教材/旧内核描述 possible、present、online、active 这 4 种状态；**7.1.3 的头文件注释已扩展为五段生命周期**（possible→present→enabled→online→active），并另有 `cpu_dying_mask` 标记下线中的 CPU：

| 位图 | 含义 |
|-|-|
| `cpu_possible_mask` | 系统中**可存在**的 CPU（启动时固定，热插拔 ID 全集）|
| `cpu_present_mask` | 已**插入/枚举**的 CPU（热插拔时可动态变化；无热插拔时与 possible 相同）|
| `cpu_enabled_mask` | **允许被上线**的 CPU（7.1.3 在经典四状态之外新增：`register_cpu()` 置位 <sub>[/drivers/base/cpu.c:441](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/drivers/base/cpu.c)</sub>，`unregister_cpu()` 清除）|
| `cpu_online_mask` | **可被调度器使用**的 CPU（present 的动态子集）|
| `cpu_active_mask` | **允许任务迁移/唤醒落地**的 CPU（onlining/offlining 过渡期会先摘除）|
| `cpu_dying_mask` | 正在**下线过程中**的 CPU（cpuhp 置位 <sub>[/kernel/cpu.c:727](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/cpu.c)</sub>）|

在系统启动时（以 arm64 为例），`smp_init_cpus()` 函数会通过ACPI或者DTS表来查询和获取CPU内核的数量（`acpi_disabled` 时走 `of_parse_and_init_cpus()` 设备树路径，否则走 `acpi_parse_and_init_cpus()`），然后对每个 CPU 调用 `smp_cpu_setup()`——其中 `set_cpu_possible(cpu, true)` 把 CPU 设置进 `cpu_possible_bits` 位图，即 `cpu_possible_mask` 变量：

- `smp_init_cpus()` <sub>[/arch/arm64/kernel/smp.c:739](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/smp.c)</sub>，由 `setup_arch()` 调用 <sub>[/arch/arm64/kernel/setup.c:360](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/setup.c)</sub>
- `smp_cpu_setup()` → `set_cpu_possible()` <sub>[/arch/arm64/kernel/smp.c:488](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/smp.c)</sub>
- x86 对应逻辑在 `arch/x86/kernel/smpboot.c` 中完成
- possible→present→enabled→online→active 的状态推进由 CPU hotplug（cpuhp）状态机完成 <sub>[/kernel/cpu.c](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/cpu.c)</sub>

---
