# ARM64 MMIO 映射与访问 — 文档索引

## 文档列表

| 文档 | 内容 |
|------|------|
| [01-MMIO核心调用链.md](01-MMIO核心调用链.md) | ioremap/iounmap 完整代码路径、页表建立、MAIR 属性、readl/writel 实现 |
| [02-平台设备MMIO映射.md](02-平台设备MMIO映射.md) | 平台设备(platform_device)从 Device Tree 到 ioremap 的完整流程 |
| [03-PCI设备MMIO映射.md](03-PCI设备MMIO映射.md) | PCI 设备 BAR 映射、pci_iomap/pcim_iomap 调用链 |
| [04-MMIO硬件交互流程图.md](04-MMIO硬件交互流程图.md) | CPU→MMU→Cache→总线→设备 完整硬件交互流程图与时间线 |

## 核心要点速查

### ioremap 核心路径

```
ioremap(phys, size)
  → __ioremap(phys, size, PROT_DEVICE_nGnRE)        [arch/arm64/include/asm/io.h:169]
    → __ioremap_caller(phys, size, prot, caller)     [arch/arm64/mm/ioremap.c:20]
      → get_vm_area_caller(size, VM_IOREMAP, ...)   [mm/vmalloc.c:2121]
      → ioremap_page_range(vaddr, vend, phys, prot)  [mm/ioremap.c:222]
        → ioremap_p4d_range → pud → pmd → pte
          → pfn_pte(pfn, prot) | set_pte_at()        [mm/ioremap.c:76-77]
      → return (void __iomem *)(offset + vaddr)
```

### 普通内存 vs MMIO 的关键差异

| 维度 | 普通内存 | MMIO |
|------|---------|------|
| 虚拟地址来源 | 线性映射 `PAGE_OFFSET + PA - PHYS_OFFSET` | vmalloc 区域动态分配 |
| 映射建立 | 启动时 `map_mem()` 一次性 | 驱动加载时 `ioremap()` 按需 |
| MAIR 属性 | 0xff (WBWA, cacheable) | 0x04 (Device nGnRE, non-cacheable) |
| PTE AttrIndx | 4 (MT_NORMAL) | 1 (MT_DEVICE_nGnRE) |
| Cache 行为 | 经过 L1/L2/L3 | 绕过所有 Cache |
| 页表层级 | 通常用 block/section 映射 (PMD/PUD) | 通常用 PTE 级别逐页映射 |
| 访问接口 | 直接指针解引用 | readl/writel (含内存屏障) |

### 平台设备 MMIO 推荐用法

```c
// 推荐：一行搞定
void __iomem *base = devm_platform_ioremap_resource(pdev, 0);
if (IS_ERR(base))
    return PTR_ERR(base);
```

### PCI 设备 MMIO 推荐用法

```c
// 推荐：自动管理
void __iomem *base = pcim_iomap(pdev, BAR0, 0);
if (!base)
    return -ENOMEM;
```

### MMIO 访问

```c
// 读取状态寄存器 (有读屏障)
u32 status = readl(base + STATUS_REG_OFFSET);

// 写入命令寄存器 (有写屏障)
writel(CMD_START, base + CMD_REG_OFFSET);

// 不需要屏障时使用 relaxed 版本
u32 val = readl_relaxed(base + DATA_REG_OFFSET);
```

## ARM64 关键内存属性速查

| 属性 | MAIR 编码 | 用途 |
|------|----------|------|
| `PROT_DEVICE_nGnRnE` | AttrIndx=0, MAIR=0x00 | PCI 配置空间 |
| `PROT_DEVICE_nGnRE` | AttrIndx=1, MAIR=0x04 | 普通 MMIO (默认 ioremap) |
| `PROT_NORMAL_NC` | AttrIndx=3, MAIR=0x44 | ioremap_wc, 显存 |
| `PROT_NORMAL` | AttrIndx=4, MAIR=0xff | 普通内存, 线性映射 |
