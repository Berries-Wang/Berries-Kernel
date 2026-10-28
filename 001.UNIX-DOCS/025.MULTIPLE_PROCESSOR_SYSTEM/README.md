# MULTIPLE PROCESSOR SYSTEMS（多处理器系统）：SMP / NUMA / 多计算机 / 分布式

> 原目录名 025.SMP-NUMA，因覆盖现代操作系统第 8 章全章（§8.1 多处理器 / §8.2 多计算机 / §8.3 分布式系统）而更名为 025.MULTIPLE_PROCESSOR_SYSTEM。
> 本目录把**理论**（现代操作系统 第5版 §8.1–8.3）、**硬件实物**（UPI/xGMI、服务器主板框图）和**内核实现**（Linux 7.1.3 源码）放在一处对照学习。

| 子目录 | 内容 | 主要来源 |
|-|-|-|
| [000.NUMA](./000.NUMA/README.md) | NUMA 概念、UPI/xGMI 互连、主板框图（MZ73-LM2）、lscpu 实例、内核节点管理入口 | 至强/EPYC 白皮书、奔跑吧Linux内核 |
| [001.SMP](./001.SMP/README.md) | SMP 概念、SMP 与 UMA/NUMA 的关系、cpumask 状态位图（7.1.3 五段生命周期）与 SMP 启动枚举 | Linux 7.1.3 源码 |
| [002.多处理器理论](./002.多处理器理论/README.md) | §8.1 全节总结：UMA 三种互连、NUMA 目录协议、多核/众核/GPU、OS 三种模型、TSL/自旋锁、四类调度；配书中插图 8-2–8-15 | [Modern Operating Systems 5th Edition #8.1](../../007.BOOKs/现代操作系统/Modern%20Operating%20Systems%205th%20Edition-PDF-01.pdf)（PDF P559–586）|
| [003.Linux-7.1.3实现](./003.Linux-7.1.3实现/README.md) | §8.1 理论 ↔ 源码对照：cpumask、调度域(SDTL)、qspinlock/MCS/osq、NUMA balancing、core scheduling，均带已核对的 `文件:行号` | [Linux 7.1.3 源码树](../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3) |
| [004.多计算机理论](./004.多计算机理论/README.md) | §8.2 集群：互连拓扑/交换/网卡与复制、消息传递（阻塞与非阻塞）、RPC、DSM、调度与负载均衡，附内核对照（sendfile 零拷贝、RPS/XPS、pin 页、RDMA、sunrpc）；配书中插图 8-16–8-25 | [Modern OS 5e #8.2](../../007.BOOKs/现代操作系统/Modern%20Operating%20Systems%205th%20Edition-PDF-01.pdf)（PDF P586–608）|
| [005.分布式系统理论](./005.分布式系统理论/README.md) | §8.3 网络硬件与协议、中间件四种范式（Web / 分布式文件系统 / CORBA / Linda 与发布订阅），附内核对照（NFS/9p/AFS/Ceph/SMB、TCP 重传退避）；配书中插图 8-26–8-37 | 同上（PDF P608–627）|

**建议阅读顺序**：002（多处理器理论）→ 000/001（硬件与概念补充）→ 003（内核落地）→ 004（多计算机）→ 005（分布式系统）。

**相关目录**：[031.进程管理/008.进程调度/003.SMP负载均衡](../031.进程管理/008.进程调度/003.SMP负载均衡)、[031.进程管理/008.进程调度/003.组调度.md](../031.进程管理/008.进程调度/003.组调度.md)、[031.进程管理/008.进程调度/008.SMT](../031.进程管理/008.进程调度/008.SMT)、[039.网络协议栈与系统实现](../039.网络协议栈与系统实现)、[033.文件系统](../033.文件系统)
