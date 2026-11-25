# NVIDIA
|技术项|说明|参考|
|-|-|-|
|- Unified and System Memory|- 即CPU 和 GPU各有自己独立的物理内存，但是共用一个虚拟地址空间|- [2.6. Unified and System Memory](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/understanding-memory.html#memory-unified-virtual-address-space) <br/> -  [007.BOOKs/098.NVIDIA/cuda-programming-guide.pdf#2.6. Unified and System Memory](../../007.BOOKs/098.NVIDIA/cuda-programming-guide.pdf)<br/> - 看这个，绝对有收获:[NVIDIA_Grace_Hopper_Architecture_CN.pdf](../../007.BOOKs/098.NVIDIA/NVIDIA_Grace_Hopper_Architecture_CN.pdf)|
|-|-|-|
|- Address Translation Service (ATS) |- ATS 允许完全访问所有 CPU 与 GPU 内存分配；无需迁移——迁移更少。ATS 为整个系统创建单一页表；NVLink-C2C允许在不迁移的情况下访问所有物理内存。|- [Accelerating Scientific Workflows with the  NVIDIA Grace Hopper Platform#P14](../../007.BOOKs/098.NVIDIA/Accelerating_Scientific_Workflows_with_the_NVIDIA_Grace_Hopper_Platform_SC23.pdf)|
|-|-|-|
|- GB10 统一内存架构（Unified Memory Architecture）|- 与 Grace Hopper 思路不同：GB10 是单一物理内存池（128GB LPDDR5x 由 CPU/GPU 共享，GPU 可直接分配 >100GiB），S-die（CPU）与 G-die（GPU）经 C2C + Arm AMBA CHI-G 实现封装内芯片级缓存一致性；无页迁移概念|- [NVIDIA GB10 Superchip Whitepaper#P5](../../007.BOOKs/098.NVIDIA/nvidia-gb10-superchip-whitepaper.pdf) <br/> - [NVIDIA GB10 Superchip Whitepaper#P15](../../007.BOOKs/098.NVIDIA/nvidia-gb10-superchip-whitepaper.pdf)|
|-|-|-|

---

## Accelerators (加速器)
GPU就是图中的加速器: 
![001.UNIX-DOCS/999.IMGS/wechat_2026-11-22_102259_345.png](../../001.UNIX-DOCS/999.IMGS/wechat_2026-11-22_102259_345.png)
> 加速器 [Learn the architecture - Introduction to AMBA®CHI C2C property negotiatio#P21](../../007.BOOKs/learn_the_architecture_introduction_to_amba_chi_c2c_property_negotiation_112071_0100_01_en.pdf)

---

## 统一地址空间实现对比: Grace Hopper vs GB10
两份文档都叫"统一内存/统一地址空间"，也都基于 NVIDIA C2C 互连，但统一的对象层次完全不同。

### Grace Hopper: 物理上分离，逻辑上统一（统一的是虚拟地址空间）
- 两块物理独立的内存: Grace CPU 侧 LPDDR5X（240~480 GB，500 GB/s）+ Hopper GPU 侧 HBM3（96 GB，4000 GB/s），带宽相差 8 倍
- 统一的是虚拟地址空间，靠三个机制实现:
  1. **NVLink-C2C**（900 GB/s，硬件缓存一致性）: 任一处理器都能缓存一致地访问任一物理内存——CPU 把 GPU 数据取入 CPU L3、GPU 经 CPU L3 读 CPU 内存
  2. **ATS 地址翻译服务**: 为全系统创建单一系统页表，CPU 的 `malloc()` 可被翻译映射到 CPU 页或 GPU 页，物理页 first-touch 分配；页迁移后 CPU/GPU 经共享页表自动在新位置访问 X
  3. **自动页迁移**: 系统可自动迁移托管内存与 CPU 分配的内存——数据放在哪一侧性能差异巨大（500 vs 4000 GB/s），按需把页搬到访问者本地
- 本质: **异构双内存池之上的统一虚拟地址空间**——先"免迁移直接访问"，再"迁移换局部性"
- 参考: [SC23 CN#P13-15](../../007.BOOKs/098.NVIDIA/Accelerating_Scientific_Workflows_with_the_NVIDIA_Grace_Hopper_Platform_SC23_CN.pdf)（全局访问/ATS/共享页表）、[SC23 CN#P18-19](../../007.BOOKs/098.NVIDIA/Accelerating_Scientific_Workflows_with_the_NVIDIA_Grace_Hopper_Platform_SC23_CN.pdf)（系统分配器: malloc 直接跑 GPU kernel）

### GB10: 物理上就是一块内存（统一的是物理内存池）
- 单一 128 GB LPDDR5x 由 CPU 和 GPU 共享，GPU 没有专用显存，直接从公共池分配（可分到 >100 GiB）
- S-die（CPU）与 G-die（GPU）2.5D 封装，经 C2C + **Arm AMBA CHI-G** 一致性协议互联，全缓存一致
- CPU/GPU 直接互访、无需拷贝；**不存在"页迁移"概念**——只有一块内存，无处可迁，也就不需要 ATS 单一系统页表那套机制
- 本质: **封装内芯片级一致性共享同一块物理内存**
- 参考: [GB10 Whitepaper#P5](../../007.BOOKs/098.NVIDIA/nvidia-gb10-superchip-whitepaper.pdf)（Two Tiles: C2C + CHI-G）、[GB10 Whitepaper#P15](../../007.BOOKs/098.NVIDIA/nvidia-gb10-superchip-whitepaper.pdf)（Unified Memory Architecture）

### 对比表
| | Grace Hopper | GB10 |
|---|---|---|
| 内存拓扑 | 异构双池（LPDDR5X + HBM3） | 单一同构池（LPDDR5x） |
| 统一的层次 | **虚拟**地址空间（系统级页表 + ATS） | **物理**内存（芯片级共享） |
| 一致性实现 | NVLink-C2C 硬件一致性 + SCF（可扩到 4 芯片一致性/256 GPU） | C2C + AMBA CHI-G（封装内） |
| 数据迁移 | 有（自动页迁移优化局部性） | 无此概念 |
| 位置敏感性 | 高（500 vs 4000 GB/s 分层） | 无（平坦单一池，带宽共享） |

一句话概括: **Grace Hopper 解决的是"两块异构内存如何呈现成一个统一地址空间"（一致性互连 + 单一页表 + 按需迁移）；GB10 解决的是"一块共享内存如何被两颗 die 一致地访问"（封装内芯片级一致性）**。

编程模型上的差异: SC23 文档要花大量篇幅讲 cudaMallocManaged、系统分配器、迁移时机这些"位置优化"话题；GB10 白皮书对此几乎不着墨，因为开发者面对的就是一个平坦的共享内存。