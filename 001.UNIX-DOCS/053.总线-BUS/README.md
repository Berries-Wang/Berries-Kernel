# 总线(BUS)
> 由 [037.内核基础](../037.内核基础/README.md) 拆解而来 <br/>
> 相关: <br/> - [000.内存管理/023.MMIO(内存映射IO)](../000.内存管理/023.MMIO(内存映射IO)/README.md): 地址/数据/控制总线、AXI总线、PCI专用总线 <br/> - [025.SMP-NUMA](../025.SMP-NUMA/000.NUMA/README.md): UPI/QPI 总线跨NUMA节点访问远端内存 <br/> - [051.AI-ASSISANT/001.CPU与DMA双向交互(网络收包事件全流程)](../051.AI-ASSISANT/001.CPU与DMA双向交互(网络收包事件全流程).md): PCI/PCI-X 总线主控(bus master)、总线地址、总线仲裁

|总线类型|说明|备注|
|-|-|-|
|- 硬总线（Hard Bus）|- 在你的电脑主板上，CPU、内存、硬盘是通过物理铜线（总线）连接的。因为有这根线，CPU 访问内存的速度极快且非常稳定。|- I2C等，见 [Firmware-Engineer](https://github.com/Berries-Wang/Firmware-Engineer)|
|-|-|-|
|- 分布式软总线（Soft Bus）|- 在多设备环境下，并没有物理线缆。软总线通过 Wi-Fi、蓝牙、P2P 等无线技术，在软件层模拟出了一根“总线”。|- 详见 [001.分布式软总线.md](./001.分布式软总线.md)|
|-|-|-|

> 待补: Linux驱动模型中的总线（`bus_type`、总线-设备-驱动匹配，PCI/USB/platform/I2C/SPI枚举），与 [052.IO设备/001.设备驱动程序如何装入操作系统.md](../052.IO设备/001.设备驱动程序如何装入操作系统.md) 相关
