# MMU

## 名词&概念
### Translation regimes <sup>翻译制式: D8.1.2 Translation regimes</sup>
在ARM架构手册中，翻译制式 是一个核心概念，它指的是一套完整的、用于管理虚拟地址到物理地址转换的系统或上下文。

A translation regime determines how a VA is mapped to a PA. The translation regime is affected by the current Security state, the current Exception level, the enabled Exception levels, HCR_EL2 settings, and implemented features.（转换机制决定了如何将虚拟地址映射到物理地址。该转换机制受当前安全状态、当前异常级别、已启用的异常级别、HCR_EL2寄存器设置及已实现功能的影响）

The implemented Exception levels affect the supported translation regimes and translation stages（已实现的异常级别回影响到支持的转换机制和转换阶段）

For each Security state, configuration of stage 1 and stage 2 translation can produce output addresses only in physical address spaces marked as YES in the following table: （对于每个安全状态，阶段 1 与阶段 2 转换的配置所生成的输出地址，仅能位于下表中标记为“是”的物理地址空间内。） ？ 暂时还不理解

Only the EL1&0 translation regimes support a stage 2 translation.(仅 EL1&0 转换机制支持阶段 2 转换)

If a software agent, such as an operating system, uses or configures stage 1 translations, then that software agent might be unaware of all of the following: (若某软件代理（如操作系统）使用或配置阶段 1 转换，则该代理可能无法感知以下所有情况)
- The stage 2 translation.
- The distinction between IPA and PA.(IPA与PA的区别)

If the Effective value of HCR_EL2.E2H is 0, then only the EL1&0 translation regime, stage 1, can support two VA ranges. （若 HCR_EL2.E2H 的有效值为 0，则仅 EL1&0 转换机制的阶段 1 可支持两个虚拟地址范围）




## 参考
- [D8.1.2 Translation regimes](../../../../007.BOOKs/Arm®%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/) # D8.1.2 Translation regimes