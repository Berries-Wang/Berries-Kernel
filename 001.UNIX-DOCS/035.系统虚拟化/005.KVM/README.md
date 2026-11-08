# 005.KVM

> 定位：Linux KVM 在 arm64 上的**源码级**实现分析。上层概念（trap-and-emulate、Type-1/2、HVC/ERET 区别）见 [../README.md](../README.md) 与 [../000.系统虚拟化-学习资料索引.md](../000.系统虚拟化-学习资料索引.md)；源码树 = [000.SOURCE_CODE/003.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kvm)。

## 本目录笔记

| 文档 | 内容 |
| - | - |
| [001.VM-exit与VM-entry指令全链路](001.VM-exit与VM-entry指令全链路.md) | guest→VMM 触发什么（HVC/SMC/WFx/sysreg/MMIO abort + IRQ/FIQ/SError 路由）、VMM→guest 用什么（`eret`）、nVHE 的 `hvc #0` 下潜、VHE 的 E2H 向量表重定向、PSCI 实例走查、异常注入 |
| [002.场景流程图-PSCI-CPU_ON中的hvc与eret](002.场景流程图-PSCI-CPU_ON中的hvc与eret.md) | 一个具体场景（guest 启动副核）的 5 幕流程图：4 次 hvc + 4 次 eret（宿主 3 次下潜 + guest 1 次 PSCI）的每一次触发点/落点/代码标注 |
| [003.架构图-vCPU与物理CPU](003.架构图-vCPU与物理CPU.md) | KVM 分层架构、对象模型（kvm/vcpu/kvm_run/memslots）、vCPU=内存里的 CPU 状态、vCPU↔pCPU 多对多时分复用、vCPU 一生的流程图 |
| [004.架构图-内存-中断-定时器-设备的映射](004.架构图-内存-中断-定时器-设备的映射.md) | CPU 之外的资源映射：stage 2+memslots、vgic 两级模型与 LR 投递、虚拟/物理定时器、设备三种呈现方式（模拟/virtio/直通）+ 联动全链 |

## 一图总览

```
        ioctl(KVM_RUN)                    hvc #0 (仅 nVHE)
 QEMU ────────────────> 宿主内核 EL1 ──────────────> KVM hyp EL2
                          kvm_arch_vcpu_ioctl_run      __kvm_vcpu_run
                          (arm.c:1266 循环)                | eret (entry.S:88)
                                ^                          v
                                | handle_exit           guest EL1
                                | (handle_exit.c:446)   hvc/smc/mrs/wfi/访存abort
                                |          ^ 慢路径 eret      |  IRQ/FIQ/SError
                                |          | (host.S:96)       |  (异常路由,硬件自动,
                                |          |                  v   落 EL2 向量表)
                                |          |          KVM hyp 向量表(hyp-entry.S)
                                |          └◄───────────────────┘
```

> KVM hyp = KVM 中运行在 EL2 的那部分代码（`arch/arm64/kvm/hyp/`：存/恢复寄存器、切 stage 2、开关陷阱、执行进出的 `eret`）；KVM 的其余逻辑（调度/模拟）跑在 EL1 宿主内核。nVHE 下 hyp 是独立的 EL2 小映像，VHE 下宿主内核本体就在 EL2。详见 001 §1.1。

## 建议阅读顺序

1. [../README.md](../README.md)「系统虚拟化的流程」「HVC 和 ERET 指令区别」——概念底座
2. [001.VM-exit与VM-entry指令全链路](001.VM-exit与VM-entry指令全链路.md) §1.1 名词速览 → §2（硬件异常模型）→ §3/§4（KVM 两条链路）
3. [003.架构图-vCPU与物理CPU](003.架构图-vCPU与物理CPU.md)——vCPU 到底是什么、和 pCPU 的多对多关系
4. [002.场景流程图-PSCI-CPU_ON中的hvc与eret](002.场景流程图-PSCI-CPU_ON中的hvc与eret.md)——用具体场景把 4 次 hvc + 4 次 eret 串成时间线
5. [004.架构图-内存-中断-定时器-设备的映射](004.架构图-内存-中断-定时器-设备的映射.md)——CPU 之外的四类资源映射
6. 配合 [Learn the architecture: AArch64 virtualization Guide](../../../007.BOOKs/learn_the_architecture_aarch64_virtualization_guide_102142_0100_06_en.pdf) §4-7（stage 2 / trap / 虚拟异常 / 定时器）
7. 待补：stage 2 页表走读（挂 [../003.内存虚拟化](../003.内存虚拟化/README.md)）、vgic 虚拟中断深入、VHE/nVHE/pKVM 对比
