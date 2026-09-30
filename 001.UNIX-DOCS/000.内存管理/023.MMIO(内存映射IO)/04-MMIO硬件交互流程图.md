# ARM64 MMIO 全流程架构图与硬件交互

## 1. 完整系统架构图：从 CPU 到设备寄存器

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                              ARM64 CPU                                      │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                      Core (e.g., Cortex-A76)                         │    │
│  │  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐       │    │
│  │  │  L1 I$   │    │  L1 D$   │    │  Fetch   │    │  Decode  │       │    │
│  │  │ (48KB)   │    │ (32KB)   │    │   Unit   │    │   Unit   │       │    │
│  │  └────┬─────┘    └────┬─────┘    └────┬─────┘    └────┬─────┘       │    │
│  │       │               │               │               │             │    │
│  │       └───────────────┴───────────────┘               │             │    │
│  │                          │                            │             │    │
│  │              ┌───────────┴───────────┐                │             │    │
│  │              │     L2 Cache (256KB)  │                │             │    │
│  │              └───────────┬───────────┘                │             │    │
│  └──────────────────────────┼────────────────────────────┘             │    │
│                             │                                          │    │
│  ┌──────────────────────────┼──────────────────────────────────────┐   │    │
│  │                    MMU (Memory Management Unit)                   │   │    │
│  │  ┌──────────┐    ┌───────┴──────────┐    ┌──────────────────┐    │   │    │
│  │  │  TLB     │◄───│ Page Table Walk  │◄───│ TTBR0_EL1 (User) │    │   │    │
│  │  │ (TLB I$) │    │   Unit           │    │ TTBR1_EL1 (Kern) │    │   │    │
│  │  └────┬─────┘    └───────┬──────────┘    └──────────────────┘    │   │    │
│  │       │                  │                                       │   │    │
│  │  ┌────┴──────────────────┴──────────────────────────────────┐    │   │    │
│  │  │              MAIR_EL1 (Memory Attribute Register)          │    │   │    │
│  │  │  Attr0(MT_DEVICE_nGnRnE)=0x00  Attr3(MT_NORMAL_NC)=0x44   │    │   │    │
│  │  │  Attr1(MT_DEVICE_nGnRE) =0x04  Attr4(MT_NORMAL)   =0xff   │    │   │    │
│  │  │  Attr2(MT_DEVICE_GRE)   =0x0c  Attr5(MT_NORMAL_WT)=0xbb   │    │   │    │
│  │  └────────────────────────────────────────────────────────────┘    │   │    │
│  └──────────────────────────┬────────────────────────────────────────┘   │    │
│                             │                                            │    │
│  ┌──────────────────────────┴──────────────────────────────────────┐    │    │
│  │                       L3 Cache (可选)                             │    │    │
│  └──────────────────────────┬──────────────────────────────────────┘    │    │
└─────────────────────────────┼──────────────────────────────────────────-┘
                              │
                    ┌─────────┴──────────┐
                    │   Interconnect     │
                    │ (AXI/ACE/CHI Bus)  │
                    └──┬─────────────┬───┘
                       │             │
          ┌────────────┴──┐    ┌─────┴──────────────┐
          │  DRAM         │    │  Device Controller  │
          │  Controller   │    │  (e.g., UART, NIC)  │
          │  ┌──────────┐ │    │  ┌───────────────┐  │
          │  │ DDR PHY  │ │    │  │  MMIO Regs    │  │
          │  └────┬─────┘ │    │  │  0x09000000   │  │
          └───────┼───────┘    │  └───┬───────────┘  │
                  │            └──────┼──────────────┘
          ┌───────┴───────┐          │
          │  DDR SDRAM    │          │ 设备寄存器被映射为
          │  (普通内存)     │          │ 物理地址空间的一部分
          └───────────────┘          │
```

## 2. MMIO 访问完整流程图（从软件到硬件）

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           软件层 (Linux Kernel)                          │
│                                                                         │
│  设备驱动代码:                                                           │
│    val = readl(base + REG_OFFSET);   // 读取设备寄存器                    │
│    writel(val, base + REG_OFFSET);   // 写入设备寄存器                    │
│                                                                         │
│  展开后 (ARM64):                                                         │
│    readl(c):                                                            │
│      1. u32 __r = le32_to_cpu(__raw_readl(c))  // 转小端                 │
│      2. dma_rmb()                               // 读内存屏障             │
│                                                                         │
│    writel(v,c):                                                         │
│      1. dma_wmb()                               // 写内存屏障             │
│      2. __raw_writel(cpu_to_le32(v), c)         // 转小端                │
│                                                                         │
│  __raw_readl / __raw_writel (内联汇编):                                  │
│    ldr %w0, [%1]     // 或 ldarb (带 workaround)                        │
│    str %w0, [%1]                                                        │
│                                                                         │
│  虚拟地址: VA (例如 0xFFFF0012_34560000)                                 │
│  ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ │
│                           硬件层 (ARM Core)                              │
│                                                                         │
│  ┌─ MMU ───────────────────────────────────────────────────────────┐   │
│  │                                                                    │   │
│  │  TLB 查找:  VA → PA                                                │   │
│  │    TLB 命中?                                                       │   │
│  │     是 → 直接使用缓存的 PA + Attr                                   │   │
│  │     否 → Page Table Walk:                                          │   │
│  │       TTBR1_EL1 → PGD → PUD → PMD → PTE                           │   │
│  │                                                                    │   │
│  │  PTE 内容:                                                         │   │
│  │    [物理页帧号(PA高40位)] | [AttrIndx=0b001] | [SH=0b11]           │   │
│  │    | [AF=1] | [PXN=1] | [UXN=1] | [VALID=1] | [TYPE=Page]         │   │
│  │                          │                                        │   │
│  │               MAIR_EL1[1] = 0x04                                   │   │
│  │               (Device, nGnRE)                                      │   │
│  │                                                                    │   │
│  └────────────────────────────────────────────────────────────────────┘   │
│                             │                                            │
│                             │ 物理地址 PA (例如 0x09000000)              │
│                             │ 内存属性: Device nGnRE                     │
│                             ▼                                            │
│  ┌─ L1/L2 Cache ─────────────────────────────────────────────────────┐   │
│  │                                                                     │   │
│  │  检测到 Device 属性 → 跳过所有 Cache!                                │   │
│  │  不查 L1 D$, 不查 L2, 不走 Cache 一致性协议                          │   │
│  │                                                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                             │                                            │
│                             │ 直接发到总线 (non-cacheable)               │
│                             ▼                                            │
└─────────────────────────────────────────────────────────────────────────┘
                              │
                    AXI/ACE 总线事务
                    (Device Memory Type)
                              │
                    ┌─────────┴──────────┐
                    │  Interconnect      │
                    │  (地址解码)          │
                    │  0x09000000 →       │
                    │  UART 控制器         │
                    └─────────┬──────────┘
                              │
┌─────────────────────────────┴─────────────────────────────────────────┐
│                       UART 控制器 (PL011)                               │
│                                                                         │
│  ┌──────────────────────────────────────────┐                          │
│  │         寄存器组 (MMIO)                    │                          │
│  │  ┌──────────────┐  ┌──────────────┐       │                          │
│  │  │ UARTDR (0x00)│  │ UARTFR (0x18)│ ...   │                          │
│  │  │  数据寄存器   │  │  标志寄存器   │       │                          │
│  │  └──────────────┘  └──────────────┘       │                          │
│  │                                           │                          │
│  │  ldr 指令 → 返回 UARTFR 当前值             │                          │
│  │  str 指令 → 将数据写入 UARTDR, 触发发送    │                          │
│  └──────────────────────────────────────────┘                          │
│                                                                         │
│  设备内部逻辑:                                                           │
│    - ldr 到 0x18 (UARTFR): 返回 FIFO 状态, BUSY 标志                    │
│    - str 到 0x00 (UARTDR): 将字节写入发送 FIFO, 启动移位寄存器         │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

## 3. 普通内存 vs MMIO 访问硬件路径对比

```
┌───────────────────────────────────────────────────────────────────────┐
│                    普通内存访问 (Normal Memory)                          │
│                                                                         │
│  Kernel 代码:                                                           │
│    *ptr = 0x42;        // 直接指针解引用, ptr 在线性映射区              │
│                                                                         │
│  ARM 指令:               MMU 处理:           Cache 行为:               │
│    str w0, [x1]    →    查 TLB          →    查 L1 D$                 │
│                          VA → PA             命中 → 写入 Cacheline      │
│                          (PAGE_OFFSET +      未命中 → Cache Fill        │
│                           PHYS_OFFSET)       + Write Allocate            │
│                          AttrIndx=4          最终写回 DDR                │
│                                             (Write-Back)                │
│                                                                         │
│  PTE 属性: PROT_NORMAL (0xff in MAIR → Normal WBWA)                     │
│  访问路径: CPU → L1$ → L2$ → L3$ → DRAM                                 │
│  延迟:    ~1ns (L1 hit) ~10ns (L2 hit) ~80ns (DRAM)                    │
│                                                                         │
├───────────────────────────────────────────────────────────────────────┤
│                      MMIO 访问 (Device Memory)                           │
│                                                                         │
│  Kernel 代码:                                                           │
│    val = readl(base + REG_OFFSET);                                      │
│                                                                         │
│  ARM 指令:               MMU 处理:           Cache 行为:               │
│    ldr w0, [x1]    →    查 TLB          →   跳过所有 Cache!            │
│                          VA → PA            直接发 AXI 总线              │
│                          (vmalloc 区域)      Device Memory 类型          │
│                          AttrIndx=1          读事务                         │
│                                             等待设备响应                 │
│                                                                         │
│  PTE 属性: PROT_DEVICE_nGnRE (0x04 in MAIR → Device nGnRE)              │
│  访问路径: CPU → AXI Bus → 设备控制器                                    │
│  延迟:    不可预测（取决于设备速度和总线仲裁）                             │
│  保证:    不聚合(nG), 不重排(nR), 允许提前应答(E)                        │
└───────────────────────────────────────────────────────────────────────┘
```

## 4. ioremap 映射建立完整流程图

```
┌───────────────────────────────────────────────────────────────────────┐
│                        软件层：ioremap 流程                               │
│                                                                         │
│  驱动: arch_initcall(my_driver_init);                                    │
│                                                                         │
│  [1] 入口宏 (arch/arm64/include/asm/io.h:169)                            │
│    ioremap(0x09000000, 0x1000)                                           │
│      → __ioremap(0x09000000, 0x1000, PROT_DEVICE_nGnRE)                 │
│      → pgprot: PTE_TYPE_PAGE|PTE_AF|PTE_SHARED|NG|PXN|UXN|WRITE|       │
│                ATTRINDX(MT_DEVICE_nGnRE=1)                               │
│                                                                         │
│  [2] __ioremap_caller() (arch/arm64/mm/ioremap.c:20)                     │
│    ├── 页对齐: phys_addr &= PAGE_MASK → 0x09000000                      │
│    ├── pfn_valid() 检查: RAM? → NO → 继续                               │
│    ├── get_vm_area_caller() [mm/vmalloc.c:2121]                          │
│    │     → 在 VMALLOC_START~VMALLOC_END 红黑树中查找空闲区域             │
│    │     → 返回虚拟地址, 例如 0xFFFF0012_34560000                        │
│    └── ioremap_page_range() [mm/ioremap.c:222]                           │
│                                                                         │
│  [3] ioremap_page_range(vaddr, vaddr+0x1000, 0x09000000, prot)          │
│    ┌──────────────────────────────────────────────────────────────┐     │
│    │ 遍历页表层级:                                                  │     │
│    │                                                               │     │
│    │ pgd = pgd_offset_k(vaddr)          [PGD项索引]                │     │
│    │ ioremap_p4d_range()                                           │     │
│    │   p4d = p4d_alloc_track()          [按需分配P4D页表]          │     │
│    │   ioremap_pud_range()                                         │     │
│    │     pud = pud_alloc_track()        [按需分配PUD页表]          │     │
│    │     ioremap_pmd_range()                                       │     │
│    │       pmd = pmd_alloc_track()      [按需分配PMD页表]          │     │
│    │       尝试 huge PMD? (2MB对齐?) → 不满足条件                   │     │
│    │       ioremap_pte_range()                                      │     │
│    │         pte = pte_alloc_kernel_track() [分配PTE页表页]        │     │
│    │         for each 4KB page:                                     │     │
│    │           pfn = 0x09000 (物理地址 >> 12)                      │     │
│    │           pte_val = (pfn << 12) | PROT_DEVICE_nGnRE           │     │
│    │           set_pte_at(&init_mm, addr, ptep, pte)               │     │
│    │             WRITE_ONCE(*ptep, pte)  [写入PTE]                 │     │
│    │             dsb(ishst)              [数据同步屏障]             │     │
│    │             isb()                   [指令同步屏障]             │     │
│    └──────────────────────────────────────────────────────────────┘     │
│                                                                         │
│  [4] 返回 (void __iomem *)(offset + addr)                               │
│      例如: (0x000 + 0xFFFF0012_34560000)                                │
│      = 0xFFFF0012_34560000                                              │
│                                                                         │
├───────────────────────────────────────────────────────────────────────┤
│                        硬件层：页表建立                                    │
│                                                                         │
│  ┌────────────────────────────────────────────────────────────────┐    │
│  │                   ARM64 页表结构 (4KB, 4级)                      │    │
│  │                                                                  │    │
│  │  TTBR1_EL1 ──► PGD (Level 0)                                    │    │
│  │                    │                                             │    │
│  │                    ▼ bits[47:39] 索引                            │    │
│  │                 PUD (Level 1)                                    │    │
│  │                    │                                             │    │
│  │                    ▼ bits[38:30] 索引                            │    │
│  │                 PMD (Level 2)                                    │    │
│  │                    │                                             │    │
│  │                    ▼ bits[29:21] 索引                            │    │
│  │                 PTE (Level 3)                                    │    │
│  │                    │                                             │    │
│  │                    ├── PTE[0]: PA=0x09000000, AttrIndx=1(Device)  │    │
│  │                    ├── PTE[1]: PA=0x09001000, AttrIndx=1(Device)  │    │
│  │                    ├── PTE[2]: PA=0x09002000, AttrIndx=1(Device)  │    │
│  │                    └── PTE[3]: PA=0x09003000, AttrIndx=1(Device)  │    │
│  │                                                                  │    │
│  │  每个 PTE 的硬件编码:                                             │    │
│  │  ┌────────────────────────────────────────────────────────┐     │    │
│  │  │ Output Addr[47:12] │ Attr[1:0]│SH│AF│NG│...│VALID│     │     │    │
│  │  │      0x09000        │    01    │11│1 │1 │   │  1  │     │     │    │
│  │  └────────────────────────────────────────────────────────┘     │    │
│  │                                                                  │    │
│  │  MAIR_EL1[1] = 0x04(Device nGnRE): 不聚合, 不重排, 允许提前应答  │    │
│  └──────────────────────────────────────────────────────────────────┘    │
│                                                                         │
└───────────────────────────────────────────────────────────────────────┘
```

## 5. iounmap 释放流程图

```
┌───────────────────────────────────────────────────────────────────────┐
│                           iounmap 流程                                    │
│                                                                         │
│  驱动:                                                                    │
│    iounmap(base);     // 或者 devm 自动调用                               │
│                                                                         │
│  [1] iounmap() [arch/arm64/mm/ioremap.c:71]                              │
│    ├── addr &= PAGE_MASK                                                │
│    ├── is_vmalloc_addr(addr)? → YES (在 vmalloc 区域)                    │
│    └── vunmap(addr) [mm/vmalloc.c:2363]                                  │
│          └── __vunmap(addr, 0)    // deallocate_pages=0                  │
│                │                                                        │
│  [2] __vunmap() [mm/vmalloc.c:2240]                                     │
│    ├── find_vm_area(addr) → 在 vmap_area 红黑树中查找                   │
│    └── vm_remove_mappings(area, 0)                                       │
│          │                                                               │
│  [3] vm_remove_mappings() [mm/vmalloc.c:2194]                            │
│    └── remove_vm_area(area->addr) [mm/vmalloc.c:2159]                    │
│          ├── __find_vmap_area(addr)   [在 vmap_area 树中查找]            │
│          ├── free_unmap_vmap_area(va) [释放页表 + 回收虚拟地址]          │
│          │     ├── 清理 PTE 页表项 (unmap_kernel_range?)                  │
│          │     ├── 释放中间页表 (如果整页表为空)                           │
│          │     ├── TLB 刷新 (针对该虚拟地址范围)                          │
│          │     └── 将 vmap_area 放回空闲树 (free_vmap_area_rb)           │
│          └── kasan_free_shadow(vm)    [释放 KASAN shadow 映射]           │
│                                                                         │
│  [4] kfree(area)                                                        │
│    └── 释放 vm_struct 内存                                               │
│                                                                         │
├───────────────────────────────────────────────────────────────────────┤
│                          硬件层：页表清理                                  │
│                                                                         │
│  ┌──────────────────────────────────────────────────────────────┐      │
│  │  1. 清除 PTE 项: *ptep = 0                                   │      │
│  │  2. dsb(ishst)          // 确保 PTE 清空可见                  │      │
│  │  3. tlbi vmalle1is      // 刷 TLB (使缓存 VA→PA 映射失效)     │      │
│  │  4. dsb(ish)            // 确保 TLB 刷新完成                   │      │
│  │  5. isb()               // 指令同步                           │      │
│  │                                                               │      │
│  │  之后任何对该 VA 的访问都会触发 Translation Fault              │      │
│  └──────────────────────────────────────────────────────────────┘      │
│                                                                         │
└───────────────────────────────────────────────────────────────────────┘
```

## 6. 关键硬件寄存器一览

| 寄存器 | 作用 | 在 MMIO 中的角色 |
|--------|------|-----------------|
| **TTBR0_EL1** | 用户态页表基地址 | 用户进程页表起始 |
| **TTBR1_EL1** | 内核态页表基地址 | 内核页表起始 (ioremap 的页表在此) |
| **TCR_EL1** | 翻译控制寄存器 | 配置页表粒度 (4K/16K/64K)、VA 位数 |
| **MAIR_EL1** | 内存属性间接寄存器 | 定义 AttrIndx[0..7] 对应的内存类型 |
| **SCTLR_EL1** | 系统控制寄存器 | 使能 MMU (M bit)、I-Cache、D-Cache |
| **TTBR0_EL1** | TLB | 缓存 VA→PA 映射，加速地址翻译 |

### MAIR_EL1 编码详情

`arch/arm64/include/asm/sysreg.h:596-605` → `arch/arm64/mm/proc.S:48-54`

```
MAIR_EL1 (64-bit 寄存器, 每 8 位定义一个内存属性):

Attr7  Attr6  Attr5      Attr4      Attr3      Attr2      Attr1      Attr0
      unused  0xbb       0xff       0x44       0x0c       0x04       0x00
              (WT)       (WBWA)     (NC)       (GRE)      (nGnRE)    (nGnRnE)

Attr0 (MT_DEVICE_nGnRnE, index=0): 0x00 = 0000_0000
  Device memory, non-Gathering, non-Reordering, no Early Ack

Attr1 (MT_DEVICE_nGnRE, index=1): 0x04 = 0000_0100
  Device memory, non-Gathering, non-Reordering, Early Ack

Attr4 (MT_NORMAL, index=4): 0xff = 1111_1111
  Normal memory, Inner Write-Back, Outer Write-Back,
  Read-Allocate, Write-Allocate (full cacheable)
```

## 7. 从 CPU 指令到设备寄存器的端到端时间线

```
时间 →

[CPU Core]
 │  readl(base + 0x18)                    // C 代码
 │  ldr w0, [x1]                          // ARM 指令被发射
 │
 ├─[MMU/TLB]────────────────────
 │   TLB 查找 (L1 TLB: 1 cycle)
 │     Miss → L2 TLB
 │       Hit → PA=0x09000018, Attr=Device nGnRE
 │
 ├─[Cache]──────────────────────
 │   检测到 Device 属性 → Skip Cache
 │
 ├─[AXI Bus]────────────────────
 │   发出 AXI Read 事务:
 │     ARADDR=0x09000018
 │     ARCACHE=Device Memory
 │     ARSIZE=32-bit
 │
 │   等待 ARREADY (总线仲裁, 可能多 cycle)
 │   等待 RVALID  (设备响应延迟)
 │
 ├─[Interconnect]───────────────
 │   地址解码: 0x09000000 → UART 控制器
 │   路由到对应从设备
 │
 ├─[UART Controller]────────────
 │   设备解码: offset 0x18 → UARTFR 寄存器
 │   读取内部状态 (TX FIFO full? RX FIFO empty?)
 │   返回数据: 0x00000090 (TXFF=1, RXFE=1)
 │
 ├─[AXI Bus]────────────────────
 │   Read Response:
 │     RRESP=OKAY, RDATA=0x00000090
 │
 ├─[CPU Core]───────────────────
 │   数据写入寄存器 w0
 │   dma_rmb() 屏障
 │   返回给 C 代码
 │
 │  // 典型总延迟: 几十到几百 ns
 │  // (取决于总线频率、设备速度、桥接延迟)
```

## 8. 相关源码文件索引

| 文件 | 关键内容 | 行号 |
|------|---------|------|
| `arch/arm64/include/asm/io.h` | ioremap/ioremap_wc 宏定义, readl/writel, __raw_readl/writel | 24-180 |
| `arch/arm64/mm/ioremap.c` | __ioremap_caller, __ioremap, iounmap | 20-101 |
| `arch/arm64/include/asm/pgtable-prot.h` | PROT_DEVICE_nGnRE, PROT_NORMAL_NC 属性定义 | 48-52 |
| `arch/arm64/include/asm/pgtable.h` | pfn_pte, set_pte_at, set_pte | 76-311 |
| `arch/arm64/include/asm/pgtable-hwdef.h` | PTE_* 位定义, PMD/PUD/PGD 定义 | 74-275 |
| `arch/arm64/include/asm/memory.h` | MT_DEVICE_*, MT_NORMAL 索引, __phys_to_virt | 197-355 |
| `arch/arm64/include/asm/sysreg.h` | MAIR_ATTR_* 编码, MAIR_ATTRIDX | 596-605 |
| `arch/arm64/mm/proc.S` | MAIR_EL1 初始化 (汇编) | 47-54, 423-424 |
| `mm/ioremap.c` | ioremap_page_range, 各级页表建立 | 64-250 |
| `mm/vmalloc.c` | get_vm_area_caller, vunmap, __vunmap | 2121-2371 |
| `mm/pgalloc-track.h` | pte_alloc_kernel_track, pmd_alloc_track 等 | 6-51 |
| `lib/devres.c` | __devm_ioremap, devm_ioremap_resource, pcim_iomap | 25-395 |
| `lib/pci_iomap.c` | pci_iomap_range, pci_iomap, pci_iomap_wc | 28-137 |
| `drivers/base/platform.c` | platform_get_resource, devm_platform_ioremap_resource | 49-137 |
| `drivers/of/address.c` | of_address_to_resource, of_iomap | 865-900 |
