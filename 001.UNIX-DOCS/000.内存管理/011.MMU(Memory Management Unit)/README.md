# 011.MMU(Memory Management Unit)

## ARMv8-A 架构中，有效的 TTBR 寄存器
| 异常级别 | 角色 | 存在的寄存器 | 用途 |
| :--- | :--- | :--- | :--- |
| EL3 | 安全监视器 | `TTBR0_EL3` | 仅用于 EL3 的安全世界地址转换。 |
| EL2 | 虚拟化层 (Hypervisor) | `TTBR0_EL2`, `TTBR1_EL2` | 用于管理虚拟机（Guest OS）的物理地址到真实物理地址的转换（Stage-2 转换）。 |
| EL1 | 操作系统内核 | `TTBR0_EL1`, `TTBR1_EL1` | 这是最关键的层级。<br>• `TTBR0_EL1`: 通常指向用户空间（EL0）的页表。<br>• `TTBR1_EL1`: 通常指向内核空间（EL1）的页表。 |
| EL0 | 应用程序 | 无 | 依赖 EL1 的寄存器进行地址翻译。 |

