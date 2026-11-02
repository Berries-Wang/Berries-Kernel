# 文档写作约定

## CPU 平台举例规范

整理内核笔记时，平台举例固定包含以下**三部分**，缺一不可——三者覆盖的场景互不可替代：

| # | 平台 | 覆盖场景（不可漏） | 关键规格速查 |
|---|------|------------------|-------------|
| 1 | **AMD EPYC 9996**（x86_64） | 同构 SMT 众核、大规模 NUMA（NPS1/2/4）、CCX/LLC 边界、amd-pstate(EPP/CPPC) 调频域、256 核隔离与部署 | 第 6 代 EPYC 9006 系列旗舰，代号 "Venice"，Zen 6c，256C/512T，8 CCD + 2 IOD，16 CCX × 64MB L3 = 1GB，基频 2.55GHz，TDP 600W |
| 2 | **Arm AGI CPU**（arm64，同构服务器） | arm64 主实例：ACPI/PPTT 拓扑、CPPC capacity、SLC=LLC、CMN 互连、DSU-120 Direct connect、同构无 EAS 的"有 EM 也未必有 EAS"场景 | SP113012（一律用核数最多的旗舰档；家族：SP113012=136C / SP113012S=128C / SP113012A=64C），Neoverse V3 核心，136 核/双 chiplet，Armv9.2-A，每核私有 L2 2MB/3MB，无 SMT |
| 3 | **Cortex-A725（大核）/ Cortex-A520（小核）**（arm64，异构 big.LITTLE） | **异构场景必须补上，不能漏**：capacity-dmips-mhz 归一化（1024/252）、SD_ASYM_CPUCAPACITY(_FULL)、EAS 启用与能量比较、性能域(PD)划分、CLS 域成活/退化（A520 对共享 L2 vs A725 私有 L2）——这些只有异构才有意义 | A725：乱序，每核私有 L2（8 路，128KB~1MB）；A520：顺序，每 complex（一对核）共享 L2（128~512KB）；同经 DSU-120 共享簇级 L3 |

## 组织方式

- **正文主实例**：EPYC 9996（x86_64）与 Arm AGI CPU（arm64）；
- **异构部分**：Cortex-A725/A520 内容一般放在**附录**（标题如"附录：Cortex-A725 + A520 异构教学对照"），但**必须写**——凡主题涉及容量异构、EAS、大小核功耗取舍、cluster 域时，附录即为正文教学数字（如 cap=1024/252、PD 表、能量估算）的出处，不可省略。

## 内核源码版本

- 笔记若需结合 Linux 内核源码，**统一使用 [000.SOURCE_CODE/003.LINUX-7.1.3](000.SOURCE_CODE/003.LINUX-7.1.3)（Linux 7.1.3）** 这套代码；
- 代码引用规范：路径写成 `kernel/sched/xxx.c`，行号必须用 `grep -n` 实测（```c 代码块首行注释如 `// kernel/sched/topology.c:1803`），不得凭记忆编写代码；
- 历史对照：`000.SOURCE_CODE/000.LINUX-5.9` 仅作版本差异对比（如 "5.9 vs 7.1.3" 对照表）使用，主基准始终是 7.1.3。

## 原子指令写作规范

- **正文以 LSE 为主**：后续写原子指令相关笔记（示例代码、执行流程、争用分析）时，主路径一律用 **LSE 原子指令**（`LDADD`/`LDCLR`/`LDSET`/`LDEOR`/`SWP`/`CAS`…，含 `A/L/AL` 内存序后缀）；
- **LL/SC 写到附录**：LL/SC（`LDXR/STXR`）相关内容——独占监视器两级状态机、配对限制、ERG 粒度、cmpwait/WFE 等待（LSE 无等待类指令）、非标准 RMW、128 位原子（FEAT_LSE128 之前）、Armv8.0 基线兼容——统一放到**附录**（标题如"附录：LL/SC（LDXR/STXR）与独占监视器"），与上面"异构部分放附录"同一组织方式；正文只在需要对照时一句话带过并链接附录。附录里**不得写成"LL/SC 已淘汰"**，须注明其保留地；
- **术语**：RMW 首次出现时展开为"RMW（Read-Modify-Write，读-改-写）"，定义见 [001.UNIX-DOCS/008.同步原语/001.原子指令/README.md](001.UNIX-DOCS/008.同步原语/001.原子指令/README.md)；
- **对照表复用**：两路线全区别总表见 [004.LLSC与LSE的区别.md](001.UNIX-DOCS/008.同步原语/001.原子指令/004.LLSC与LSE的区别.md)，LL/SC→LSE 逐条替换对照与四块保留地见 [003.LLSC实现原理.md §8](001.UNIX-DOCS/008.同步原语/001.原子指令/003.LLSC实现原理.md)——新笔记链接这两处，不重复抄表。

## 参考资料（TRM）

- Neoverse V3（AGI CPU 核心）：[arm_neoverse_v3_core_technical_reference_manual_107734_0002_06_en.pdf](007.BOOKs/arm_neoverse_v3_core_technical_reference_manual_107734_0002_06_en.pdf)；互连/SLC 另见 [arm_neoverse_cmn_700_trm_102308_0307_08_en.pdf](007.BOOKs/arm_neoverse_cmn_700_trm_102308_0307_08_en.pdf)
- Cortex-A725：[arm_cortex_a725_core_trm_107652_0001_04_en.pdf](007.BOOKs/arm_cortex_a725_core_trm_107652_0001_04_en.pdf)；Cortex-A520：[arm_cortex_a520_core_trm_102517_0001_04_en.pdf](007.BOOKs/arm_cortex_a520_core_trm_102517_0001_04_en.pdf)
