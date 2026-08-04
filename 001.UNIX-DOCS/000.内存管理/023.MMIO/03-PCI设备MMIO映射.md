# PCI 设备 MMIO 映射流程 (ARM64)

## 1. PCI 设备 BAR 模型

PCI/PCIe 设备通过 BAR (Base Address Register) 暴露其 MMIO 寄存器区域。每个 PCI 设备最多有 6 个 BAR (BAR0-BAR5)。

### 1.1 PCI BAR 的两种类型

| BAR 类型 | IORESOURCE 标志 | 映射方式 |
|----------|----------------|----------|
| Memory BAR | IORESOURCE_MEM | ioremap |
| I/O BAR | IORESOURCE_IO | __pci_ioport_map (访问 I/O 端口空间) |

### 1.2 BAR 配置过程

1. 上电后，BIOS/UEFI 或内核为每个 BAR 分配物理地址空间
2. 内核 PCI 子系统读取 BAR 配置寄存器，填入 `pci_dev.resource[]` 数组
3. 驱动通过 `pci_resource_start(dev, bar)` 等 API 获取已分配的物理地址

## 2. PCI MMIO 映射完整调用链

### 2.1 标准 pci_iomap 路径

```
PCI 设备
    ↓ BIOS/内核 PCI 枚举
pci_dev.resource[bar] (struct resource)
    ↓ pci_resource_start(dev, bar)  获取物理地址
    ↓ pci_iomap_range(dev, bar, 0, 0)
    ↓ ioremap(start, len)            [调用 ARM64 ioremap 宏]
    ↓ __ioremap(PROT_DEVICE_nGnRE)   [arch/arm64/mm/ioremap.c:64]
    ↓ 页表建立
    ↓
返回 void __iomem *
```

### 2.2 pci_iomap_range 详细实现

`lib/pci_iomap.c:28-49`：

```c
void __iomem *pci_iomap_range(struct pci_dev *dev,
                              int bar,
                              unsigned long offset,
                              unsigned long maxlen)
{
    resource_size_t start = pci_resource_start(dev, bar);  // 行 33: BAR 物理起始地址
    resource_size_t len = pci_resource_len(dev, bar);      // 行 34: BAR 大小
    unsigned long flags = pci_resource_flags(dev, bar);    // 行 35: IORESOURCE_MEM/IO

    // 边界检查
    if (len <= offset || !start)
        return NULL;

    len -= offset;
    start += offset;
    if (maxlen && len > maxlen)
        len = maxlen;

    // 根据 BAR 类型选择映射方式
    if (flags & IORESOURCE_IO)
        return __pci_ioport_map(dev, start, len);  // 行 44: I/O 端口空间
    if (flags & IORESOURCE_MEM)
        return ioremap(start, len);                // 行 46: MMIO → ioremap

    return NULL;
}
EXPORT_SYMBOL(pci_iomap_range);
```

**关键点**：
- 行 33-35：通过 `pci_resource_start/len/flags` 从 `pci_dev` 中获取 BAR 信息
- 行 46：对于 Memory BAR，直接调用 `ioremap(start, len)` → 进入 ARM64 `__ioremap_caller()`

### 2.3 pci_iomap 简化封装

`lib/pci_iomap.c:111-115`：
```c
void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long maxlen)
{
    return pci_iomap_range(dev, bar, 0, maxlen);
}
EXPORT_SYMBOL(pci_iomap);
```

### 2.4 pci_iomap_wc (Write-Combining)

`lib/pci_iomap.c:68-94`：
```c
void __iomem *pci_iomap_wc_range(struct pci_dev *dev,
                                 int bar,
                                 unsigned long offset,
                                 unsigned long maxlen)
{
    resource_size_t start = pci_resource_start(dev, bar);
    resource_size_t len = pci_resource_len(dev, bar);
    unsigned long flags = pci_resource_flags(dev, bar);

    if (flags & IORESOURCE_IO)
        return NULL;                    // I/O BAR 不支持 WC

    // ... 边界检查 ...

    if (flags & IORESOURCE_MEM)
        return ioremap_wc(start, len);  // 使用 Normal Non-Cacheable 属性

    return NULL;
}
```

`ioremap_wc` 定义在 `arch/arm64/include/asm/io.h:170`：
```c
#define ioremap_wc(addr, size)  __ioremap((addr), (size), __pgprot(PROT_NORMAL_NC))
```

WC (Write-Combining) 使用 `PROT_NORMAL_NC` (Normal Non-Cacheable) 属性，允许写入合并，适合显存等场景。

## 3. Managed PCI iomap (pcim_*)

### 3.1 pcim_iomap：自动管理的 PCI I/O 映射

`lib/devres.c:356-370`：

```c
void __iomem *pcim_iomap(struct pci_dev *pdev, int bar, unsigned long maxlen)
{
    void __iomem **tbl;

    BUG_ON(bar >= PCIM_IOMAP_MAX);  // 最多 6 个 BAR

    // 获取或创建 iomap 管理表
    tbl = (void __iomem **)pcim_iomap_table(pdev);
    if (!tbl || tbl[bar])    // 不允许多次映射同一个 BAR
        return NULL;

    // 调用 pci_iomap → ioremap
    tbl[bar] = pci_iomap(pdev, bar, maxlen);
    return tbl[bar];
}
EXPORT_SYMBOL(pcim_iomap);
```

### 3.2 pcim_iomap_table：获取管理表

`lib/devres.c:331-345`：

```c
void __iomem * const *pcim_iomap_table(struct pci_dev *pdev)
{
    struct pcim_iomap_devres *dr, *new_dr;

    // 首次调用时，从 devres 查找是否已有 table
    dr = devres_find(&pdev->dev, pcim_iomap_release, NULL, NULL);
    if (dr)
        return dr->table;

    // 不存在则分配新的 table
    new_dr = devres_alloc(pcim_iomap_release, sizeof(*new_dr), GFP_KERNEL);
    if (!new_dr)
        return NULL;
    dr = devres_get(&pdev->dev, new_dr, NULL, NULL);
    return dr->table;
}
```

### 3.3 pcim_iounmap：释放单个 BAR

`lib/devres.c:376-395`：

```c
void pcim_iounmap(struct pci_dev *pdev, void __iomem *addr)
{
    void __iomem **tbl;
    int i;

    pci_iounmap(pdev, addr);  // 调用 iounmap 释放

    tbl = (void __iomem **)pcim_iomap_table(pdev);
    for (i = 0; i < PCIM_IOMAP_MAX; i++)
        if (tbl[i] == addr) {
            tbl[i] = NULL;
            return;
        }
}
```

### 3.4 驱动 detach 时自动释放

`lib/devres.c:306-317`：
```c
static void pcim_iomap_release(struct device *dev, void *res)
{
    struct pcim_iomap_devres *this = res;
    int i;

    for (i = 0; i < PCIM_IOMAP_MAX; i++)
        if (this->table[i])
            pci_iounmap(dev, this->table[i]);
}
```

## 4. PCI 配置空间映射

PCI 配置空间需要特殊的设备内存属性。ARM64 为其定义了专用接口：

`arch/arm64/include/asm/io.h:180`：
```c
#define pci_remap_cfgspace(addr, size) \
    __ioremap((addr), (size), __pgprot(PROT_DEVICE_nGnRnE))
```

使用 `PROT_DEVICE_nGnRnE` 而非 `PROT_DEVICE_nGnRE`，因为 PCI 规范禁止配置空间写操作为 posted write。

## 5. PCI 设备 MMIO 生命周期

```
PCI 设备枚举
    │
    ├── BIOS/UEFI 或内核枚举 PCI 拓扑
    ├── 读取 BAR 寄存器，分配物理地址
    ├── pci_dev.resource[0..5] 填入 resource 信息
    │
    ├── 内核对 BAR 进行二次地址翻译（如果需要）
    │
    └── 驱动加载 (probe)

驱动 probe:
    ├── pci_enable_device(pdev)                    ← 启用 PCI 设备
    │
    ├── pcim_iomap(pdev, BAR0, 0)                  ← 推荐：自动管理的映射
    │   ├── pcim_iomap_table(pdev)                 ← 获取管理表
    │   ├── pci_iomap(pdev, BAR0, 0)
    │   │   ├── pci_resource_start(pdev, BAR0)     ← 获取 BAR0 物理地址
    │   │   ├── pci_resource_flags() == IORESOURCE_MEM?
    │   │   └── ioremap(start, len)                ← 建立 Device nGnRE 映射
    │   └── tbl[BAR0] = 返回值
    │
    ├── readl(base + OFFSET) / writel(val, base + OFFSET)  ← 访问寄存器
    │
    └── (... 正常工作 ...)

驱动卸载:
    └── pcim_iomap_release()                       ← 自动调用
        └── for each BAR:
            └── pci_iounmap()
                └── iounmap()
```

## 6. PCI vs 平台设备映射对比

| 特性 | PCI 设备 | 平台设备 |
|------|---------|---------|
| 物理地址来源 | BAR 寄存器 (BIOS/PCI 枚举分配) | Device Tree `reg` 属性 |
| resource 填充 | PCI 子系统 (`drivers/pci/probe.c`) | OF 子系统 (`drivers/of/address.c`) |
| 常用映射 API | `pci_iomap()` / `pcim_iomap()` | `devm_platform_ioremap_resource()` |
| 区域请求 | PCI 自动管理 | `devm_request_mem_region()` |
| I/O 端口支持 | 是 (`IORESOURCE_IO` → `__pci_ioport_map`) | 否 (通常只有 IORESOURCE_MEM) |
| 自动释放 | pcim_* 系列支持 | devm_* 系列支持 |
| 配置空间 | `pci_remap_cfgspace()` (PROT_DEVICE_nGnRnE) | 不需要 |
