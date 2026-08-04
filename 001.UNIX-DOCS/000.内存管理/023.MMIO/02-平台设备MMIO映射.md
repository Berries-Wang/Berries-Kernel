# 平台设备 MMIO 映射流程 (ARM64)

## 1. 平台设备资源模型

在 ARM64 嵌入式系统中，设备通常通过 Device Tree (设备树) 描述其硬件资源。平台设备 (platform_device) 是 Linux 内核中最重要的设备模型之一。

### 1.1 设备树中 MMIO 资源的描述

```dts
uart0: serial@9000000 {
    compatible = "arm,pl011";
    reg = <0x0 0x09000000 0x0 0x1000>;  // 起始物理地址 0x09000000, 大小 0x1000
    interrupts = <GIC_SPI 1 IRQ_TYPE_LEVEL_HIGH>;
    clocks = <&uart_clk>;
};
```

- `reg` 属性的第一个 `<addr, size>` 对（index 0）即为 MMIO 寄存器区域的物理地址和大小
- 内核在解析设备树时将其转换为 `struct resource`

### 1.2 struct resource 结构

```c
struct resource {
    resource_size_t start;   // 物理起始地址
    resource_size_t end;     // 物理结束地址
    const char *name;        // 资源名称
    unsigned long flags;     // IORESOURCE_MEM / IORESOURCE_IO / IORESOURCE_IRQ
    // ...
};
```

## 2. 平台设备 MMIO 映射完整调用链

### 2.1 场景一：标准平台设备驱动（设备树解析）

```
Device Tree (.dts)
    ↓ (内核解析)
platform_device.resource[] (struct resource)
    ↓ platform_get_resource(pdev, IORESOURCE_MEM, index)
    ↓ devm_ioremap_resource(&pdev->dev, res)
    ↓ __devm_ioremap_resource()
    ↓ __devm_ioremap()
    ↓ ioremap(res->start, size)
    ↓ __ioremap_caller()           [arch/arm64/mm/ioremap.c:20]
    ↓ ioremap_page_range()         [mm/ioremap.c:222]
    ↓ 页表建立 (PGD→P4D→PUD→PMD→PTE)
    ↓
返回 void __iomem * 虚拟地址
```

### 2.2 详细代码路径

#### Step 1: 设备树 → resource

`drivers/of/address.c:865-882` — `of_address_to_resource()`：
```c
int of_address_to_resource(struct device_node *dev, int index,
                           struct resource *r)
{
    const __be32 *addrp;
    u64 size;
    unsigned int flags;
    const char *name = NULL;

    addrp = of_get_address(dev, index, &size, &flags);  // 从 "reg" 属性解析
    if (addrp == NULL)
        return -EINVAL;

    of_property_read_string_index(dev, "reg-names", index, &name);
    return __of_address_to_resource(dev, addrp, size, flags, name, r);
}
```

内核在创建设备时调用此函数，将设备树的 `reg` 属性解析为 `resource.start` 和 `resource.end`。

#### Step 2: 驱动获取 resource

`drivers/base/platform.c:49-61` — `platform_get_resource()`：
```c
struct resource *platform_get_resource(struct platform_device *dev,
                                       unsigned int type, unsigned int num)
{
    u32 i;
    for (i = 0; i < dev->num_resources; i++) {
        struct resource *r = &dev->resource[i];
        if (type == resource_type(r) && num-- == 0)
            return r;
    }
    return NULL;
}
```

驱动通常这样调用：
```c
struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
```

#### Step 3: 便捷封装（推荐使用）

`drivers/base/platform.c:74-84` — `devm_platform_get_and_ioremap_resource()`：
```c
void __iomem *
devm_platform_get_and_ioremap_resource(struct platform_device *pdev,
                                       unsigned int index, struct resource **res)
{
    struct resource *r;
    r = platform_get_resource(pdev, IORESOURCE_MEM, index);
    if (res)
        *res = r;
    return devm_ioremap_resource(&pdev->dev, r);
}
```

更简单的封装（`drivers/base/platform.c:95-99`）：
```c
void __iomem *devm_platform_ioremap_resource(struct platform_device *pdev,
                                             unsigned int index)
{
    return devm_platform_get_and_ioremap_resource(pdev, index, NULL);
}
```

驱动典型用法：
```c
void __iomem *base = devm_platform_ioremap_resource(pdev, 0);
if (IS_ERR(base))
    return PTR_ERR(base);
```

#### Step 4: devm_ioremap_resource — 请求区域 + 映射

`lib/devres.c:173-177`→`lib/devres.c:117-154` — `__devm_ioremap_resource()`：
```c
static void __iomem *
__devm_ioremap_resource(struct device *dev, const struct resource *res,
                        enum devm_ioremap_type type)
{
    resource_size_t size;
    void __iomem *dest_ptr;

    // 1. 验证资源类型必须是 IORESOURCE_MEM
    if (!res || resource_type(res) != IORESOURCE_MEM) {
        dev_err(dev, "invalid resource\n");
        return IOMEM_ERR_PTR(-EINVAL);
    }

    size = resource_size(res);

    // 2. 请求内存区域（标记为已占用，防止其他驱动重复映射）
    if (!devm_request_mem_region(dev, res->start, size, pretty_name)) {
        dev_err(dev, "can't request region for resource %pR\n", res);
        return IOMEM_ERR_PTR(-EBUSY);
    }

    // 3. 调用 ioremap 建立页表映射
    dest_ptr = __devm_ioremap(dev, res->start, size, type);
    if (!dest_ptr) {
        dev_err(dev, "ioremap failed for resource %pR\n", res);
        devm_release_mem_region(dev, res->start, size);
        dest_ptr = IOMEM_ERR_PTR(-ENOMEM);
    }

    return dest_ptr;
}
```

#### Step 5: __devm_ioremap → ioremap

`lib/devres.c:25-54` — `__devm_ioremap()`：
```c
static void __iomem *__devm_ioremap(struct device *dev, resource_size_t offset,
                                    resource_size_t size,
                                    enum devm_ioremap_type type)
{
    void __iomem **ptr, *addr = NULL;

    // 分配 devres 资源（用于驱动卸载时自动 unmapped）
    ptr = devres_alloc(devm_ioremap_release, sizeof(*ptr), GFP_KERNEL);
    if (!ptr)
        return NULL;

    switch (type) {
    case DEVM_IOREMAP:
        addr = ioremap(offset, size);   // 调用 arch ioremap
        break;
    case DEVM_IOREMAP_WC:
        addr = ioremap_wc(offset, size);
        break;
    }

    if (addr) {
        *ptr = addr;
        devres_add(dev, ptr);   // 注册到设备，驱动 detach 时自动 iounmap
    } else
        devres_free(ptr);

    return addr;
}
```

**devres 自动释放**（`lib/devres.c:15-18`）：
```c
void devm_ioremap_release(struct device *dev, void *res)
{
    iounmap(*(void __iomem **)res);
}
```

### 2.3 场景二：of_iomap（直接通过 device_node 映射）

`drivers/of/address.c:891-900`：
```c
void __iomem *of_iomap(struct device_node *np, int index)
{
    struct resource res;

    if (of_address_to_resource(np, index, &res))
        return NULL;

    return ioremap(res.start, resource_size(&res));
}
```

注意：`of_iomap()` **不会调用** `devm_request_mem_region()`，因此不会标记区域为已占用。如果多个驱动试图映射同一区域，不会有冲突检测。

### 2.4 场景三：devm_of_iomap（推荐的 OF iomap）

`lib/devres.c:223-233`：
```c
void __iomem *devm_of_iomap(struct device *dev, struct device_node *node,
                            int index, resource_size_t *size)
{
    struct resource res;

    if (of_address_to_resource(node, index, &res))
        return IOMEM_ERR_PTR(-EINVAL);
    if (size)
        *size = resource_size(&res);
    return devm_ioremap_resource(dev, &res);  // 包含 request_mem_region
}
```

与 `of_iomap()` 的区别：
- 调用 `devm_ioremap_resource()` → 包含 `request_mem_region()` → 有冲突检测
- 驱动 detach 时自动 `iounmap()`

---

## 3. 平台设备 MMIO 生命周期

```
驱动加载 (probe)
    │
    ├── platform_get_resource(pdev, IORESOURCE_MEM, 0)
    │       获取物理地址 0x09000000
    │
    ├── devm_platform_ioremap_resource(pdev, 0)
    │   ├── devm_request_mem_region()          ← 标记区域占用
    │   ├── ioremap(0x09000000, 0x1000)
    │   │   ├── get_vm_area_caller()           ← 分配虚拟地址 (如 0xFFFF001234560000)
    │   │   └── ioremap_page_range()           ← 建立 PTE (Device nGnRE)
    │   └── devres_add()                        ← 注册自动释放
    │
    ├── readl(base + REG_OFFSET)               ← 读取设备寄存器
    ├── writel(value, base + REG_OFFSET)       ← 写入设备寄存器
    │
    └── (... 驱动正常工作 ...)

驱动卸载 (remove / detach)
    │
    └── devres_release_all()                   ← 自动调用
        └── devm_ioremap_release()
            └── iounmap(addr)
                └── vunmap()
                    └── remove_vm_area() + free_unmap_vmap_area()
```
