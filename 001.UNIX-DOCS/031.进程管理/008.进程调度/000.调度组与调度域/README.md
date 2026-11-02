# 调度域与调度组（目录）

> 学习: <br/> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#8.3.2　CPU调度域 / #8.3.3　建立CPU调度域拓扑关系](../../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub) <br/> - [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub#9.1.3　查看调度域信息](../../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%202:%20Debugging%20and%20Case%20Analysis.epub)

本目录是 SMP 负载均衡的"地图"部分：调度器如何用 **调度域（sched_domain）+ 调度组（sched_group）** 两级结构描述 CPU 的缓存/内存拓扑。域与组构建完成后，负载均衡（见 [../003.SMP负载均衡/](../003.SMP负载均衡/)）才有了"先近后远"的决策依据。

## 文档导航

|文档|内核版本|内容概要|适合读者|
|-|-|-|-|
|[001.KERNEL_7.1.3.md](./001.KERNEL_7.1.3.md)|Linux 7.1.3（主产物）|default_topology/SDTL_INIT/SD_* 标志/sd_init 参数走读；arm64（DT/PPTT/capacity-dmips-mhz、Arm AGI CPU（SP113012，Neoverse V3，136 核/双 chiplet，同构服务器；异构 big.LITTLE 对照见附录 A））与 AMD EPYC 9996（8 CCD+2 IOD、MC=CCX、NPS1/NPS4 域树）两大实例；build_sched_groups 环形组链与 NUMA overlap 组；sched_init_smp → build_sched_domains → cpu_attach_domain 全链流程图；sd_degenerate 域退化；debugfs（/sys/kernel/debug/sched/domains）查看；含 6 类 ASCII 图|以新版内核为学习基准的读者|
|[002.EPYC9996-CCX-CCD-DIE-核心层级与调度域.md](./002.EPYC9996-CCX-CCD-DIE-核心层级与调度域.md)|Linux 7.1.3 + 硬件|EPYC 9996 **层级专篇**（含附录 A：Chiplet 芯粒详解——良率数学/AMD 谱系/GMI/UCIe/封装互连）：线程→核→CCX→CCD/DIE→IOD→Socket 每层共享什么资源（L1/L2/L3/Fabric/内存控制器）；"L3 边界=CCX 而非 CCD、CCD 层在调度域中不存在"两大认知点；跨 CCX 读完整路径、CPUID→llc_id→x86_topology 发现链、NPS1/NPS4 域树带调度组环链、逐层迁移代价表、频域(SMU)对照；含 9 幅 ASCII 图|想同时搞清硬件层级与内核映射的读者|
|[003.AGI-SP113012-层级架构-缓存一致性-原子操作.md](./003.AGI-SP113012-层级架构-缓存一致性-原子操作.md)|Linux 7.1.3 + 硬件|Arm AGI CPU **层级+一致性+原子性专篇**：三层战场总图（**核内**私有层次/**跨核心** CHI-E 目录式/**跨 socket** 三谱系：专利 CHI 链路·CXL(CCG)·非一致 NUMA）；CHI 事务/状态速查表、读 miss 含 DMT/DCT 优化、写路径含 WriteUniquePtl 小写优化、HN-I(I/O 一致)与 DVM(TLB 击落)；**两级独占监视器**（核内 2 态状态机+HN-F 全局）+原子三路（LL/SC/LSE/CHI 硬件原子）、32 核争用 offload 图、屏障域表；含 12 幅 ASCII 图|想搞清 arm64 一致性与原子实现路径的读者|
|[000.KERNEL_5.9.md](./000.KERNEL_5.9.md)|Linux 5.9（历史参考，保留原文）|书籍《奔跑吧 Linux 内核》第2版 §8.3.2/§8.3.3 所对应的 5.x 视角：SMT/MC/DIE 三层、AMD Threadripper 9995WX（Zen 5，MC=CCD）层级表|与书对照、了解 5.x 原貌|
|[../000.调度域与调度组.md](../000.调度域与调度组.md)|综述|上级目录的概念性综述（不随版本更新）|快速建立概念|

## 两版内核一句话对比

- 5.9：`SMT → MC → DIE`，标志 `SD_SHARE_PKG_RESOURCES`，`sd_init(tl, cpu_map, child, dflags, cpu)`；
- 7.1.3：`SMT → CLS → MC → PKG`（NUMA 再加 `NODE → NUMA`），标志 `SD_SHARE_LLC/SD_CLUSTER`，异构容量由 `asym_cpu_capacity_classify()` 自动检测，冗余层级由 `sd_degenerate()/sd_parent_degenerate()` 裁剪。

详细差异表见 [001.KERNEL_7.1.3.md](./001.KERNEL_7.1.3.md) 的"与 5.9 的关键差异总览"。

## 相关目录

- 负载均衡算法（域/组的消费方）：[../003.SMP负载均衡/](../003.SMP负载均衡/)
- SMT（SMT 域的硬件背景）：[../008.SMT/](../008.SMT/)
- NUMA 体系结构：[../../../025.MULTIPLE_PROCESSOR_SYSTEM/000.NUMA/](../../../025.MULTIPLE_PROCESSOR_SYSTEM/000.NUMA/)
