# Linux 5.9 ARM64 MMIO 映射与访问全流程分析

> 分析版本：Linux 5.9 (arch/arm64)
> 文档说明：以 ARM64 架构为主线，分析内核如何将设备物理地址映射到内核虚拟地址空间，以及 CPU 如何通过 MMIO 访问设备寄存器。

---

## 1. 整体架构概览

### 1.1 MMIO 的本质

在 ARM64 架构中，CPU 访问两种"内存"有本质区别：

| 类型 | 映射方式 | 页表属性 | 用途 |
|------|---------|---------|------|
| 普通内存 (RAM) | 线性映射 (`__phys_to_virt`) | MT_NORMAL (cacheable) | 内核数据、代码 |
| 设备 MMIO | ioremap 动态映射 | MT_DEVICE_nGnRE (device) | 设备寄存器 |

**核心区别**：普通内存使用线性映射（虚拟地址 = 物理地址 - physvirt_offset），在启动时通过 `map_mem()` 一次性建立；而 MMIO 需要运行时通过 `ioremap()` 在 vmalloc 区域动态建立页表映射，且使用 Device 内存属性（禁用 Cache、禁止指令重排等）。

### 1.2 ARM64 虚拟地址空间布局

```
+-------------------+ 0xFFFFFFFF_FFFFFFFF
|                   |
|   ...             |
|                   |
+-------------------+ VMALLOC_END
|                   |
|   vmalloc/ioremap |   <-- ioremap 返回的虚拟地址在这里
|                   |
+-------------------+ VMALLOC_START = MODULES_END
|                   |
|   MODULES         |
|                   |
+-------------------+ MODULES_VADDR
|                   |
|   ...             |
+-------------------+ PAGE_OFFSET (0xFFFF0000_00000000, 48-bit VA)
|                   |
|   Linear Map      |   <-- 普通内存直接映射（RAM）
|   (直接映射区)     |
+-------------------+ PHYS_OFFSET (物理内存起始物理地址)
```

定义位于 `arch/arm64/include/asm/pgtable.h:23-24`：
```c
#define VMALLOC_START        (MODULES_END)
#define VMALLOC_END          (- PUD_SIZE - VMEMMAP_SIZE - SZ_64K)
```

### 1.3 内存属性 (MAIR)

ARM64 通过 MAIR_EL1 寄存器定义内存属性，位于 `arch/arm64/include/asm/memory.h:197-201`：

```c
#define MT_DEVICE_nGnRnE     0   // Device: 不聚合、不重排、不提前应答
#define MT_DEVICE_nGnRE      1   // Device: 不聚合、不重排、允许提前应答
#define MT_DEVICE_GRE        2   // Device: 聚合、重排、提前应答
#define MT_NORMAL_NC         3   // Normal Non-Cacheable
#define MT_NORMAL            4   // Normal (cacheable)
#define MT_NORMAL_WT         5   // Normal Write-Through
```

MAIR 寄存器初始化位于 `arch/arm64/mm/proc.S:47-54`：
```asm
#define MAIR_EL1_SET
    (MAIR_ATTRIDX(MAIR_ATTR_DEVICE_nGnRnE, MT_DEVICE_nGnRnE) |
     MAIR_ATTRIDX(MAIR_ATTR_DEVICE_nGnRE, MT_DEVICE_nGnRE) |
     MAIR_ATTRIDX(MAIR_ATTR_DEVICE_GRE, MT_DEVICE_GRE) |
     MAIR_ATTRIDX(MAIR_ATTR_NORMAL_NC, MT_NORMAL_NC) |
     MAIR_ATTRIDX(MAIR_ATTR_NORMAL, MT_NORMAL) |
     MAIR_ATTRIDX(MAIR_ATTR_NORMAL_WT, MT_NORMAL_WT))
```

---

## 2. ioremap 核心调用链

### 2.1 入口宏：ioremap()

`arch/arm64/include/asm/io.h:169`：
```c
#define ioremap(addr, size)     __ioremap((addr), (size), __pgprot(PROT_DEVICE_nGnRE))
#define ioremap_wc(addr, size)  __ioremap((addr), (size), __pgprot(PROT_NORMAL_NC))
```

- 默认 `ioremap()` 使用 `PROT_DEVICE_nGnRE`（Device 内存，允许提前应答）
- `ioremap_wc()` 使用 `PROT_NORMAL_NC`（Normal Non-Cacheable）
- PCI 配置空间使用 `PROT_DEVICE_nGnRnE`（`arch/arm64/include/asm/io.h:180`）

### 2.2 页表保护属性定义

`arch/arm64/include/asm/pgtable-prot.h:48-50`：
```c
#define PROT_DEVICE_nGnRnE  (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE | PTE_ATTRINDX(MT_DEVICE_nGnRnE))
#define PROT_DEVICE_nGnRE   (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE | PTE_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_NORMAL_NC      (PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL_NC))
```

其中 `PROT_DEFAULT = (_PROT_DEFAULT | PTE_MAYBE_NG)` (line 45)，`_PROT_DEFAULT = PTE_TYPE_PAGE | PTE_AF | PTE_SHARED` (line 29)。

**PTE 属性位含义**（`arch/arm64/include/asm/pgtable-hwdef.h`）：
- `PTE_VALID` (bit 0): 页表项有效
- `PTE_TYPE_PAGE` (bits 0-1 = 0b11): 页表项类型为 page
- `PTE_ATTRINDX(t)` (bits 2-4): MAIR 属性索引
- `PTE_SHARED` (bits 8-9): 内部共享
- `PTE_AF` (bit 10): Access Flag
- `PTE_NG` (bit 11): Non-Global（不全局可见，用于 ASID）
- `PTE_PXN` (bit 53): Privileged Execute Never（内核态不可执行）
- `PTE_UXN` (bit 54): User Execute Never（用户态不可执行）

### 2.3 __ioremap_caller：ARM64 核心实现

`arch/arm64/mm/ioremap.c:20-62`：

```c
static void __iomem *__ioremap_caller(phys_addr_t phys_addr, size_t size,
                                      pgprot_t prot, void *caller)
{
    unsigned long last_addr;
    unsigned long offset = phys_addr & ~PAGE_MASK;   // line 24: 页内偏移
    int err;
    unsigned long addr;
    struct vm_struct *area;

    // Step 1: 页对齐 (line 33-34)
    phys_addr &= PAGE_MASK;
    size = PAGE_ALIGN(size + offset);

    // Step 2: 边界检查 (line 39-41)
    last_addr = phys_addr + size - 1;
    if (!size || last_addr < phys_addr || (last_addr & ~PHYS_MASK))
        return NULL;

    // Step 3: 禁止映射 RAM (line 46-47)
    if (WARN_ON(pfn_valid(__phys_to_pfn(phys_addr))))
        return NULL;

    // Step 4: 从 vmalloc 区域分配虚拟地址 (line 49)
    area = get_vm_area_caller(size, VM_IOREMAP, caller);
    if (!area)
        return NULL;
    addr = (unsigned long)area->addr;
    area->phys_addr = phys_addr;

    // Step 5: 建立页表映射 (line 55)
    err = ioremap_page_range(addr, addr + size, phys_addr, prot);
    if (err) {
        vunmap((void *)addr);
        return NULL;
    }

    // Step 6: 返回虚拟地址 + 页内偏移 (line 61)
    return (void __iomem *)(offset + addr);
}
```

**关键步骤**：
1. **行 24,33-34**：将物理地址向下对齐到页边界，并保存页内偏移 `offset`
2. **行 46-47**：通过 `pfn_valid()` 确保物理地址不属于 RAM — MMIO 区域不应该在 `mem_map` 中
3. **行 49**：调用 `get_vm_area_caller()` 在 vmalloc 区域查找空闲虚拟地址范围
4. **行 55**：调用 `ioremap_page_range()` 逐级建立页表
5. **行 61**：返回 `offset + addr`，使返回的虚拟地址保留原始物理地址的页内偏移

### 2.4 get_vm_area_caller：虚拟地址分配

`mm/vmalloc.c:2121-2126`：
```c
struct vm_struct *get_vm_area_caller(unsigned long size, unsigned long flags,
                                     const void *caller)
{
    return __get_vm_area_node(size, 1, flags, VMALLOC_START, VMALLOC_END,
                              NUMA_NO_NODE, GFP_KERNEL, caller);
}
```

该函数在 `VMALLOC_START` 到 `VMALLOC_END` 之间查找一个空闲的虚拟地址区域，并用红黑树（vmap_area）管理这些区域。

### 2.5 ioremap_page_range：逐级建立页表

`mm/ioremap.c:222-250`：

```c
int ioremap_page_range(unsigned long addr,
                       unsigned long end, phys_addr_t phys_addr, pgprot_t prot)
{
    pgd_t *pgd;
    unsigned long start;
    unsigned long next;
    int err;
    pgtbl_mod_mask mask = 0;

    might_sleep();
    BUG_ON(addr >= end);

    start = addr;
    pgd = pgd_offset_k(addr);       // line 235: 获取 PGD 项

    do {
        next = pgd_addr_end(addr, end);
        err = ioremap_p4d_range(pgd, addr, next, phys_addr, prot, &mask);
        if (err)
            break;
    } while (pgd++, phys_addr += (next - addr), addr = next, addr != end);

    flush_cache_vmap(start, end);   // line 244: 刷新 cache

    if (mask & ARCH_PAGE_TABLE_SYNC_MASK)
        arch_sync_kernel_mappings(start, end);  // line 246-247: 同步内核映射

    return err;
}
```

**页表层级遍历**：PGD -> P4D -> PUD -> PMD -> PTE

| 层级 | 函数 | 位置 | 尝试 huge 映射 |
|------|------|------|---------------|
| PGD | `ioremap_page_range()` | `mm/ioremap.c:222` | - |
| P4D | `ioremap_p4d_range()` | `mm/ioremap.c:198` | `ioremap_try_huge_p4d()` |
| PUD | `ioremap_pud_range()` | `mm/ioremap.c:152` | `ioremap_try_huge_pud()` |
| PMD | `ioremap_pmd_range()` | `mm/ioremap.c:106` | `ioremap_try_huge_pmd()` |
| PTE | `ioremap_pte_range()` | `mm/ioremap.c:64` | 最终页表项 |

### 2.6 ioremap_pte_range：最终 PTE 项建立

`mm/ioremap.c:64-82`：

```c
static int ioremap_pte_range(pmd_t *pmd, unsigned long addr,
                             unsigned long end, phys_addr_t phys_addr,
                             pgprot_t prot, pgtbl_mod_mask *mask)
{
    pte_t *pte;
    u64 pfn;

    pfn = phys_addr >> PAGE_SHIFT;
    pte = pte_alloc_kernel_track(pmd, addr, mask);  // line 72: 分配 PTE 页表
    if (!pte)
        return -ENOMEM;

    do {
        BUG_ON(!pte_none(*pte));
        set_pte_at(&init_mm, addr, pte, pfn_pte(pfn, prot));  // line 77: 写入 PTE
        pfn++;
    } while (pte++, addr += PAGE_SIZE, addr != end);

    *mask |= PGTBL_PTE_MODIFIED;
    return 0;
}
```

**关键操作**：
- **行 72**：`pte_alloc_kernel_track()` — 如果 PMD 项为空，通过 `__pte_alloc_kernel()` 分配一个物理页作为 PTE 表，并将 PMD 项指向该物理页
- **行 77**：逐页调用 `set_pte_at()` + `pfn_pte()` 构造并写入 PTE

### 2.7 pfn_pte：构造 PTE 值

`arch/arm64/include/asm/pgtable.h:76-77`：

```c
#define pfn_pte(pfn,prot) \
    __pte(__phys_to_pte_val((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot))
```

将物理页帧号 (pfn) 左移 PAGE_SHIFT 得到物理地址，然后与保护属性 `prot`（包含 MAIR index、权限位等）按位或，形成最终的 PTE 硬件编码值。

### 2.8 set_pte_at：写入硬件页表

`arch/arm64/include/asm/pgtable.h:302-311`：

```c
static inline void set_pte_at(struct mm_struct *mm, unsigned long addr,
                              pte_t *ptep, pte_t pte)
{
    if (pte_present(pte) && pte_user_exec(pte) && !pte_special(pte))
        __sync_icache_dcache(pte);

    __check_racy_pte_update(mm, ptep, pte);

    set_pte(ptep, pte);  // line 310
}
```

`set_pte()` 的实际实现（`arch/arm64/include/asm/pgtable.h:233-249`）：

```c
static inline void set_pte(pte_t *ptep, pte_t pte)
{
    WRITE_ONCE(*ptep, pte);

    if (pte_valid_not_user(pte)) {
        dsb(ishst);   // 数据同步屏障（内部共享域存储）
        isb();         // 指令同步屏障
    }
}
```

**关键**：对于内核态（非用户态）的有效 PTE，写入后需要执行 `dsb(ishst)` 和 `isb()` 确保页表更新对 MMU 可见。

### 2.9 flush_cache_vmap

`include/asm-generic/cacheflush.h:93-96`：
```c
#ifndef flush_cache_vmap
static inline void flush_cache_vmap(unsigned long start, unsigned long end)
{
}
#endif
```

ARM64 中这是一个**空函数**。因为 Device 内存的映射本身就不经过 cache，所以不需要显式刷新。这个调用是为那些需要软件管理 cache 一致性的架构准备的。

### 2.10 arch_sync_kernel_mappings

`mm/ioremap.c:246-247`：
```c
if (mask & ARCH_PAGE_TABLE_SYNC_MASK)
    arch_sync_kernel_mappings(start, end);
```

当页表被修改后（PGD/P4D/PUD/PMD/PTE 任一级别被修改），将新页表同步到所有页表副本中（内核页表可能被多个进程共享）。ARM64 通常在上下文切换时自动同步。

---

## 3. iounmap 释放流程

### 3.1 ARM64 iounmap

`arch/arm64/mm/ioremap.c:71-81`：

```c
void iounmap(volatile void __iomem *io_addr)
{
    unsigned long addr = (unsigned long)io_addr & PAGE_MASK;

    if (is_vmalloc_addr((void *)addr))
        vunmap((void *)addr);
}
```

- 行 73：将地址对齐到页边界
- 行 79：检查是否在 vmalloc 地址范围（防止 `ioremap_cache` 重用了 RAM 线性映射时误释放）
- 行 80：调用 `vunmap()` 释放

### 3.2 vunmap 调用链

`mm/vmalloc.c:2363-2371`：
```c
void vunmap(const void *addr)
{
    BUG_ON(in_interrupt());
    might_sleep();
    if (addr)
        __vunmap(addr, 0);  // deallocate_pages=0 (ioremap 没有物理页)
}
```

### 3.3 __vunmap 核心流程

`mm/vmalloc.c:2240-2278`：

```c
static void __vunmap(const void *addr, int deallocate_pages)
{
    struct vm_struct *area;

    area = find_vm_area(addr);   // 在 vmap_area 树中查找
    // ...
    vm_remove_mappings(area, deallocate_pages);  // 核心：释放页表 + 移除 vmap_area
    // ...
    kfree(area);  // 释放 vm_struct
}
```

### 3.4 vm_remove_mappings

`mm/vmalloc.c:2194-2204`：
```c
static void vm_remove_mappings(struct vm_struct *area, int deallocate_pages)
{
    // ...
    remove_vm_area(area->addr);   // 从红黑树删除 vmap_area，调用 free_unmap_vmap_area
    // ... (处理 VM_FLUSH_RESET_PERMS 相关)
}
```

### 3.5 remove_vm_area

`mm/vmalloc.c:2159-2181`：
```c
struct vm_struct *remove_vm_area(const void *addr)
{
    struct vmap_area *va;
    might_sleep();

    spin_lock(&vmap_area_lock);
    va = __find_vmap_area((unsigned long)addr);
    if (va && va->vm) {
        struct vm_struct *vm = va->vm;
        va->vm = NULL;
        spin_unlock(&vmap_area_lock);
        kasan_free_shadow(vm);
        free_unmap_vmap_area(va);  // 释放页表映射 + 回收 vmap_area
        return vm;
    }
    spin_unlock(&vmap_area_lock);
    return NULL;
}
```

---

## 4. MMIO 访问接口

### 4.1 整体架构

| 层次 | 函数 | 说明 |
|------|------|------|
| 原始汇编访问 | `__raw_readl/writel` | 直接 ldr/str 指令 |
| Relaxed 访问 | `readl_relaxed/writel_relaxed` | 不做内存屏障 |
| 严格访问 | `readl/writel` | 带内存屏障 |
| 通用接口 | `ioread32/iowrite32` | 架构无关，内部调用 readl/writel |

### 4.2 __raw_readl / __raw_writel：汇编层

`arch/arm64/include/asm/io.h:36-40, 72-80`：

```c
// 写（str 指令）
static __always_inline void __raw_writel(u32 val, volatile void __iomem *addr)
{
    asm volatile("str %w0, [%1]" : : "rZ" (val), "r" (addr));
}

// 读（ldr 指令，带有 device load acquire workaround）
static __always_inline u32 __raw_readl(const volatile void __iomem *addr)
{
    u32 val;
    asm volatile(ALTERNATIVE("ldr %w0, [%1]",
                             "ldar %w0, [%1]",
                             ARM64_WORKAROUND_DEVICE_LOAD_ACQUIRE)
                 : "=r" (val) : "r" (addr));
    return val;
}
```

- `str %w0, [%1]`：将 32 位寄存器值写入 `addr` 指向的内存地址
- `ldr %w0, [%1]`：从 `addr` 指向的内存地址读取 32 位值
- `ALTERNATIVE`：如果 CPU 有 Device Load Acquire 硬件 bug，则用 `ldar`（load-acquire）替代 `ldr`
- 这里的地址是 ioremap 返回的虚拟地址，但**页表项中标记了 Device 属性（AttrIndx = MT_DEVICE_nGnRE）**，MMU 会将这个访问识别为 MMIO，走 Device 内存的访问路径

### 4.3 readl/writel：带屏障的严格访问

`arch/arm64/include/asm/io.h:119-142`：

```c
// Relaxed 访问（无屏障）
#define readl_relaxed(c)    ({ u32 __r = le32_to_cpu((__force __le32)__raw_readl(c)); __r; })
#define writel_relaxed(v,c) ((void)__raw_writel((__force u32)cpu_to_le32(v),(c)))

// 严格访问（带屏障）
#define readl(c)            ({ u32 __v = readl_relaxed(c); __iormb(__v); __v; })
#define writel(v,c)         ({ __iowmb(); writel_relaxed((v),(c)); })
```

**内存屏障定义**（`arch/arm64/include/asm/io.h:94-112`）：
```c
#define __iormb(v)          ({ unsigned long tmp; dma_rmb(); ... })
#define __iowmb()           dma_wmb()
```

- **readl**：先读 MMIO，再执行 `dma_rmb()`（读内存屏障），确保后续的普通内存读取不会重排到 MMIO 读之前
- **writel**：先执行 `dma_wmb()`（写内存屏障），确保之前的普通内存写入在 MMIO 写之前完成

### 4.4 ioread32 / iowrite32：通用接口

`include/asm-generic/io.h:724-763`：
```c
static inline u32 ioread32(const volatile void __iomem *addr)
{
    return readl(addr);
}

static inline void iowrite32(u32 value, volatile void __iomem *addr)
{
    writel(value, addr);
}
```

---

## 5. 普通内存 vs MMIO 访问对比

### 5.1 普通内存访问路径

```
CPU 执行 ldr/str 指令
    ↓
虚拟地址 (PAGE_OFFSET ~ PAGE_END 范围内)
    ↓
TLB 查找 / 页表遍历
    ↓
PTE 中 AttrIndx = MT_NORMAL (cacheable)
    ↓
MMU 将访问标记为 Normal Memory
    ↓
L1/L2 Cache 查找 (可能命中/未命中)
    ↓
如果 Cache 未命中，访问 DRAM
```

普通内存的线性映射在启动时通过 `map_mem()` ( `arch/arm64/mm/mmu.c` ) 一次性建立，PTE 属性为 `PROT_NORMAL`。

### 5.2 MMIO 访问路径

```
CPU 执行 ldr/str 指令（经过 readl/writel 宏）
    ↓
虚拟地址 (VMALLOC_START ~ VMALLOC_END 范围内)
    ↓
TLB 查找 / 页表遍历
    ↓
PTE 中 AttrIndx = MT_DEVICE_nGnRE (device)
    ↓
MMU 将访问标记为 Device Memory
    ↓
绕过 L1/L2 Cache，直接发到系统总线 (AXI/ACE)
    ↓
设备控制器响应，返回寄存器值
```

Device 内存属性保证：
- **nG (non-Gathering)**：不聚合多个访问
- **nR (non-Reordering)**：不重排访问顺序
- **E (Early Write Acknowledgment)**：允许提前写应答（`nGnRE`），或不允许多个观察者之间的可见性

### 5.3 关键对比表

| 特性 | 普通内存 | MMIO (Device) |
|------|---------|---------------|
| 物理地址范围 | DRAM 地址空间 | 设备 MMIO 地址空间 |
| 映射时机 | 启动时 (`map_mem()`) | 驱动探测时 (`ioremap()`) |
| 虚拟地址区域 | 线性映射区 (PAGE_OFFSET) | vmalloc 区域 |
| 页表属性 | PROT_NORMAL (cacheable) | PROT_DEVICE_nGnRE (device) |
| MAIR 索引 | MT_NORMAL (0xff, cacheable WB) | MT_DEVICE_nGnRE (0x04) |
| Cache 行为 | L1/L2 缓存 | 绕过所有缓存 |
| 访问聚合 | 允许 | 禁止 (non-Gathering) |
| 指令重排 | 允许 | 禁止 (non-Reordering) |
| 推测执行 | 允许 | 禁止（PXN/UXN 位阻止执行） |
| 访问接口 | 直接指针解引用 | readl/writel 宏 |
| 释放方式 | 无需特殊释放 | iounmap() |
