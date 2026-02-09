# CPU虚拟化
> 学习: [23-虚拟化：CPU虚拟化 [中山大学 操作系统原理]#方法4：硬件虚拟化#P11](../../000.内存管理/998.REFS/000.中山大学-操作系统/14-0526-virt-2.pdf)

## CPU虚拟化实现方式： 方法4：硬件虚拟化
x86_64和ARM都引入了全新的虚拟化特权级

|平台|虚拟化方式|着重阅读资料|
|-|-|-|
|x86_64|- Intel推出了VT-x硬件虚拟化扩展 <br/> - amd: AMD-V|-|
|-|-|-|
|ARM|- ARM引入了EL2</br>|- [23-虚拟化：CPU虚拟化 [中山大学 操作系统原理]#方法4：硬件虚拟化#P11](../../000.内存管理/998.REFS/000.中山大学-操作系统/14-0526-virt-2.pdf) <br/> - [Learn the architecture - AArch64 virtualization](../../../006.REFS/learn_the_architecture_aarch64_virtualization_guide_102142_0100_06_en.pdf)|


### HVC 和 ERET 指令

| 指令 | 全称 | 作用 | 转换方向 |
| --- | --- | --- | --- |
| **HVC** <sup>Hypervisor call to allow OS code to call the Hypervisor. It generates an exception targeting exception level 2 (EL2).(虚拟机监控程序调用，允许作系统代码调用虚拟机管理程序。它生成一个针对异常等级 2（EL2）的异常。)</sup> , <sup>Synchronous exceptions are routed according to the rules associated with the exception-generating instructions SVC, HVC, and SMC. When implemented, other classes of exception can be routed to EL2 (Hypervisor) or EL3 (Secure Monitor). Routing is set independently for IRQs, FIQs, and SErrors. (同步异常的路由遵循异常生成指令 SVC、HVC 和 SMC 相关的规则。实现后，其他异常类别可以路由到 EL2（虚拟机监控器）或 EL3（安全监控器）,IRQ、FIQ 和 SError 的路由独立设置)</sup> | Hypervisor Call | **发起请求**：从 Non-secure EL1（内核态）调用 EL2（虚拟化层）。 | 向上切换 (Low -> High) |
| **ERET** | Exception Return | **异常返回**：从当前异常级别返回到发生异常之前的级别。 | 向下切换 (High -> Low) |

---

## 参考资料
- [Learn the architecture - AArch64 virtualization](https://developer.arm.com/documentation/102142/0100/Stage-2-translation)
- [Learn the architecture - AArch64 virtualization](../../../006.REFS/learn_the_architecture_aarch64_virtualization_guide_102142_0100_06_en.pdf)
- [HVC (A64)](https://developer.arm.com/documentation/dui0801/l/A64-General-Instructions/HVC--A64-?lang=en)
- [Learn the architecture - AArch64 Exception Model](../../../006.REFS/learn_the_architecture_-_aarch64_exception_model_102412_0103_02_en.pdf)<sup>Synchronous exceptions are routed according to the rules associated with the exception-generating instructions SVC, HVC, and SMC. When implemented, other classes of exception can be routed to EL2 (Hypervisor) or EL3 (Secure Monitor). Routing is set independently for IRQs, FIQs, and SErrors. </sup>