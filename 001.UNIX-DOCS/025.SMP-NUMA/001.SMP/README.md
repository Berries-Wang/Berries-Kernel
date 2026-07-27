# SMP(对称多处理器.Symmetric Multi-Processor)
> 在Linux内核中，SMP和NUMA是可同时存在的，且是共生的: NUMA 是多个 SMP 节点通过高速互联组成的更大系统 , 见:<br/>- [017.SMP负载均衡.md](../../../001.UNIX-DOCS/031.进程管理/008.进程调度/003.SMP负载均衡) <br/>- [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#图8.18　调度域和调度组的关系]


> 学习:<br/> - [RISC-V Architecture Programming and Practice: 图11.2 UMA体系结构](../../../007.BOOKs/RISC-V%20Architecture%20Programming%20and%20Practice/RISC-V%20Architecture%20Programming%20and%20Practice-231-464.pdf) <br/> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#8.3　SMP负载均衡

|项|说明|备注|
|-|-|-|
|- 特点|- 所有CPU共享同一个内存和I/O,并且每个CPU访问内存的速度是一样的|-|
|-|-|-|
|- 别名|- UMA（Uniform Memory Access，一致性内存访问）|-|
|-|-|-|
|- SDTL|- Linux内核通过数据结构sched_domain_topology_level来描述CPU的层次关系，本节将其简称为SDTL|-|
|-|-|-|


---

内核对CPU的管理是通过位图（bitmap）变量来管理的，并且定义了possible、present、online和active这4种状态。 <sub>[/include/linux/cpumask.h]</sub>

在系统启动时，smp_init_cpus()函数会通过ACPI或者DTS表来查询和获取CPU内核的数量，然后通过smp_cpu_setup()函数设置到cpu_possible_bits位图中，从而设置cpu_possible_mask变量。

---





