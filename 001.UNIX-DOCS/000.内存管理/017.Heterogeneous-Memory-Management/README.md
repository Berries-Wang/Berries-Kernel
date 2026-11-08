# Heterogeneous Memory Management (HMM)

> 异构内存管理: 让 GPU/加速器 **透明地共享进程的虚拟地址空间**, 并让**显存成为内核可管理的内存**。
> 源码基线: Linux v6.8 (本机 `/usr/src/linux-headers-6.8.0-138-generic` 一致)
> 官方文档: [Documentation/mm/hmm.rst](https://docs.kernel.org/v6.8/mm/hmm.html)

---

## 1. 一句话定义

HMM 解决两件事:

```
┌───────────────────────────────────────────────────────────────────────────┐
│ ① 地址空间镜像 (Address space mirroring)                                  │
│    把 CPU 页表"复制"到设备页表(GMMU), 让任意有效的 CPU 指针               │
│    对 GPU 同样有效, 并通过 mmu_notifier 保持两边同步                      │
│    → 核心 API: hmm_range_fault()                                          │
│                                                                           │
│ ② 设备内存托管 (devmap managed pages / ZONE_DEVICE)                       │
│    给显存(VRAM)每一页配一个 struct page, 让内核既有机制                   │
│    (迁移/换页/rmap/memcg) 能管理"CPU 摸不到"的内存                        │
│    → 核心 API: memremap_pages() + migrate_vma_*()                         │
└───────────────────────────────────────────────────────────────────────────┘
```

## 2. 为什么需要它 (30 秒版)

传统 GPU 编程是 **split address space (分裂地址空间)**: `malloc` 的内存和 `cudaMalloc` 的内存是两个世界, 指针不能互用, 复杂数据结构(链表/树)必须整体拷贝并重写所有指针.

```
     split address space (传统)              shared address space (HMM)
┌────────────┐                  ┌────────────┐   ┌────────────┐              ┌────────────┐
│ CPU: VA    │ ═══════════════► │ GPU: VA    │   │ CPU: VA    │  同一份 VA   │ GPU: VA    │
│ malloc     │  手动拷贝+指针   │ 设备分配器 │   │ malloc     │ ──页表镜像─► │ GMMU       │
└────────────┘   重映射(易错)   └────────────┘   └────────────┘  +自动迁移   └────────────┘
  两个独立世界, 库之间无法传递指针               任意指针 CPU/GPU 通用, 数据按需搬家
```

## 3. 学习路线 (按序阅读)

| 文件 | 内容 | 回答的问题 |
|-|-|-|
| [000.场景导入-一个页面的一生.md](./000.场景导入-一个页面的一生.md) | **从这里开始**: 一个页面在 DDR↔VRAM 间搬家的完整故事 | HMM 整体在干什么? |
| [001.硬件背景-GPU内存体系结构.md](./001.硬件背景-GPU内存体系结构.md) | 显卡硬件: VRAM/BAR/PCIe/NVLink/ATS/GMMU/C2C | 硬件为什么逼出这样的设计? |
| [002.HMM核心数据结构与API.md](./002.HMM核心数据结构与API.md) | `include/linux/hmm.h` 全源码 + `struct hmm_range` | 驱动和内核的"合同"长什么样? |
| [003.hmm_range_fault-页表镜像源码分析.md](./003.hmm_range_fault-页表镜像源码分析.md) | `mm/hmm.c` 全源码 + 缺页/镜像流程图 | GPU 缺页时内核做了什么? |
| [004.migrate_vma-双向迁移源码分析.md](./004.migrate_vma-双向迁移源码分析.md) | `mm/migrate_device.c` 源码 + 迁移时序图 | 数据怎么在 DDR/VRAM 间搬家? |
| [005.ZONE_DEVICE与devmap.md](./005.ZONE_DEVICE与devmap.md) | `mm/memremap.c` + `dev_pagemap`: 显存如何拥有 struct page | devmap managed pages 是什么? |
| [006.真实驱动实践-Nouveau-AMD-NVIDIA.md](./006.真实驱动实践-Nouveau-AMD-NVIDIA.md) | nouveau(SVM)/amdkfd(HMM)/nvidia-uvm 三家对照 | 真实显卡驱动怎么用 HMM? |

## 4. 全景图: HMM 在内核中的位置

```
 用户态:  CUDA / ROCm / oneAPI 程序
────────────────────────────────────────────────────────────────────────
 驱动层:  nvidia-uvm      amdkfd (ROCm)      nouveau (社区)
             │                │                  │
             └────────┬───────┴──────────────────┘
                      ▼
 核心 mm 层 (本文档的主角):
 ┌───────────────────────────────────────────────────────────────┐
 │ mm/hmm.c            页表镜像: hmm_range_fault()               │
 │ mm/migrate_device.c 双向迁移: migrate_vma_setup/pages/finalize│
 │ mm/memremap.c       设备页托管: memremap_pages()/ZONE_DEVICE  │
 │ mm/mmu_notifier.c   页表变化广播: invalidate 回调             │
 └───────────────────────────────────────────────────────────────┘
                      │
                      ▼
 基础设施: CPU 页表(pgtable) / 反向映射(rmap) / swap swapops / memcg
────────────────────────────────────────────────────────────────────────
 硬件:  CPU MMU + DDR   ←─ PCIe/NVLink/C2C ─→   GMMU + VRAM (+ IOMMU/ATS)
```

## 5. 原目录旧内容说明

旧 README 中的 "devmap managed pages" 主题已扩充为 [005.ZONE_DEVICE与devmap.md](./005.ZONE_DEVICE与devmap.md)。

## 参考资料
- [内核官方文档: HMM (v6.8)](https://docs.kernel.org/v6.8/mm/hmm.html)
  + [异构内存管理 (HMM) 中文文档截图](<./999.REFS/Screenshot 2026-01-24 at 15-20-01 异构内存管理 (HMM) — The Linux Kernel documentation.png>)
- 相关目录: [019.IOMMU](../019.IOMMU/README.md) (设备侧地址翻译) / [011.MMU](../011.MMU%28Memory%20Management%20Unit%29) / [022.页面回收](../022.页面回收%28page%20reclaim%29) / [005.内存分配](../005.内存分配)
- 源码 (v6.8): [mm/hmm.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/mm/hmm.c?h=v6.8) / [mm/migrate_device.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/mm/migrate_device.c?h=v6.8) / [mm/memremap.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/mm/memremap.c?h=v6.8)
- 驱动: [nouveau_svm.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/gpu/drm/nouveau/nouveau_svm.c?h=v6.8) / [kfd_migrate.c](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/gpu/drm/amd/amdkfd/kfd_migrate.c?h=v6.8) / 本机 `/usr/src/nvidia-580.178.04/nvidia-uvm/`
