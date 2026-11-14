# 基座-ARM
> 基于ARM学习Linux内核

## 本目录笔记
|文档|内容|
|-|-|
|[001.Arm-CSS-计算子系统](001.Arm-CSS-计算子系统.md)|CSS（Compute Subsystems）定义与交付边界、产品/场景对照、MPAM Hardware Guide Figure 4-2（N2 CSS）逐项解读、CSS 在 SoC 中的位置|
|[002.协议指令集与系统IP分类总览](002.协议指令集与系统IP分类总览.md)|互连/协议/指令扩展对象分类总表（23 行 24 对象，SVE/SVE2 同行）、易混概念对照（SCU/DSU、CCI/CI/CMN、NI/NoC、CHI/CHI C2C/UCIe）、系统关系图、代际时间轴、7.1.3 内核驱动触点|
|[003.AMBA-CHI-C2C架构-Figure1-3](003.AMBA-CHI-C2C架构-Figure1-3.md)|CHI C2C 组件边界、Protocol/Packetization/Data-link/PHY 四层职责、跨芯粒通信路径、ASCII 重绘|
|[004.全一致性加速器-Figure5-2-应用场景](004.全一致性加速器-Figure5-2-应用场景.md)|Figure 5-2 重绘、"fully coherent" 覆盖范围、CPU+加速器工作负载案例（GH200/MI300A/POWER9 OpenCAPI 等）、场景流程图|
|[005.Arm-AGI-CPU架构核验](005.Arm-AGI-CPU架构核验.md)|双芯粒架构图 11 项逐项核验（有依据/需修正/未确认）、修订后架构图、协议分层判断、硬件拓扑小结（含证据级别）|

## 建议阅读顺序
1. [002 分类总览](002.协议指令集与系统IP分类总览.md)（先建立"什么是什么"的框架）→ 2. [001 CSS](001.Arm-CSS-计算子系统.md)（平台级视角）→ 3. [003 CHI C2C 架构](003.AMBA-CHI-C2C架构-Figure1-3.md)（跨芯粒互连）→ 4. [004 一致性加速器应用](004.全一致性加速器-Figure5-2-应用场景.md)（工作负载落点）→ 5. [005 Arm AGI CPU 核验](005.Arm-AGI-CPU架构核验.md)（具体芯片收口，含内核笔记可引用的拓扑小结）

## 关联目录
- [003.高速缓存](../003.高速缓存/README.md)：[010.L2-Cache](../003.高速缓存/010.L2-Cache/README.md)——其 [001.MOS5-Fig1-8-L2拓扑与Linux调度](../003.高速缓存/010.L2-Cache/001.MOS5-Fig1-8-L2拓扑与Linux调度.md) 做"硬件拓扑 → cacheinfo → 调度域"对照，硬件侧引用本目录 [005](005.Arm-AGI-CPU架构核验.md)
- [053.总线-BUS](../053.总线-BUS/000.总线BUS-学习资料索引.md)：互连/总线资料总索引（已加 [002 分类总览](002.协议指令集与系统IP分类总览.md) 入口）
- AMBA 规范与各 IP TRM 全文：[007.BOOKs](../../007.BOOKs/README.md)（IHI0022L AXI / IHI0024E APB / IHI0033C AHB / IHI0050H CHI / IHI0098B CHI C2C、DSU-120/CMN-700/CCI-550/V3/A725/A520 TRM 等）
