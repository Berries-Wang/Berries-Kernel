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



## D8.2 Translation process
The translation of a VA to a PA begins with reading the translation table base address register, followed by a walk through the translation tables to read the descriptors associated with the mapping.(将虚拟地址转换为物理地址的过程始于读取转换表基址寄存器，随后遍历转换表以读取与映射相关的描述符。)
> 遍历转换表，这个转换表是页表吗?

## D8.2.1 Translation table walk
A translation table walk is the set of translation table lookups that are required to do all of the following:（转换表遍历是指为完成以下所有操作而执行的一系列转换表查询）
- For a single stage translation at stage 1, translate a VA to a PA.
- For two translation stages and stage 1 is disabled, translate an IPA to a PA.
- For a two stage translation, all of the following:
   + For a stage 1 translation, translate a VA to an IPA.
   + For a stage 2 translation, translate an IPA to a PA for each of the stage 1 translation table lookups.（对于阶段 2 转换，其作用是在每次阶段 1 转换表查询后，将中间物理地址转换为物理地址）
   + For a stage 2 translation, translate an IPA to a PA for the stage 1 OA.（对于阶段 2 转换，其作用是将阶段 1 输出地址对应的中间物理地址转换为物理地址。）
   + `这两个 For a stage 2 有什么区别?`

When a translation table walk begins, the initial translation table lookup uses the translation table base address stored in the TTBR_ELx register to locate the translation table(转换表遍历启动时，首次转换表查询将使用 TTBR_ELx 寄存器中存储的转换表基地址来定位转换表)
> 引出 `基地址寄存器`

When a translation table lookup occurs, the descriptor held in the translation table entry is read as one of the following:(当进行转换表查询时，从转换表条目中读取的描述符属于以下类型之一)
- For the VMSAv8-64 translation system, an 8-byte single-copy atomic access.
- For the VMSAv9-128 translation system, a 16-byte single-copy atomic access.


The descriptor type indicates one of the following:（描述符类型用于指示以下内容之一）
- The translation table walk is complete and the translation table entry is the final entry.（转换表遍历已完成，且该转换表条目为最终条目）
- The translation table walk requires an extra lookup at the next higher lookup level.（转换表遍历需要在下一个更高级别的查询层级进行额外查找）
- The descriptor is invalid.

All of the following [descriptor](./000.Translation%20table%20descriptor%20formats.md) types conclude the translation process:(下面所有的描述符类型推断出翻译过程)
- Page descriptor.
- Block descriptor.
- Invalid descriptor.

When an Invalid descriptor is returned during the translation process, a Translation fault at the current lookup level is generated.(当遇到了无效的描述符)

When an extra lookup level is required, the descriptor contains all of the following information:(当请求一个额外?的查询级别，这个描述符号包含以下所有的信息)
- The translation table base address of the next lookup level. (下一个查询级别的翻译表的基地址)
- For stage 1 descriptors, if the Effective value of TCR_ELx.HPDn is 0, then the descriptor has hierarchical access permissions that are applied to the final translation.
- If the translation is made in a Secure translation regime, then a stage 1 descriptor indicates whether the base  address is in the Secure address space or the Non-secure address space, unless a hierarchical control at a previous lookup level has indicated that it is required to be in the Non-secure address space.

For more information, see [Security state of translation table lookups].

For a translation lookup level, the translation table base address is one of the following:<sup>每次查询，基地址怎么来的</sup>
- For the initial lookup level, the aligned value of the appropriate TTBR_ELx.BADDR field.
- For subsequent<sup>随后的，接着的</sup> lookup levels, the next-level translation table base address returned by the previous lookup level.

For the VMSAv9-128 translation system, the Skip level (SKL) field in the descriptors and translation table base address registers permit the translation table walk to skip subsequent lookup levels.

When the last lookup level of a translation stage returns a valid descriptor, it contains all of the following:
- The OA from the translation stage.
- The memory region access permissions.
- The memory region attributes.

For more information, see [Memory region attributes]<sup>重要!</sup> and [Memory access control].

For a translation regime that uses two translation stages, the stage 1 descriptor addresses require stage 2 translation from IPA to PA.(对于使用两段翻译机制的翻译制式，阶段一的描述符需要阶段2来将IPA转换为PA)

When a translation table walk succeeds, all of the following are returned:
- The OA.
- If the translation table walk is made in a Secure translation regime, then the information returned indicates whether the OA is in the Secure IPA or PA space, or the Non-secure IPA or PA space.
- If the translation table walk is made in a Realm translation regime, then the information returned indicates whether the OA is in the Realm PA space, or the Non-secure PA space.
- If the translation table walk is made in Root state, then the OA can be in the Root, Realm, Secure, or Non-secure PA spaces.
- The output memory region attributes.
- The output memory region access permissions.

The following figure is a generalized view of a single address translation stage with three lookup levels.(这得结合[D8.3　Translation table descriptor formats](./001.D8.3%20Translation%20table%20descriptor%20formats.md)&[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure]页表描述符来看了)
![Figure D8-1 Generalized view of a single address translation stage](./../999.IMGS/Screenshot%20from%202025-09-24%2007-56-25.png)

![Figure D8-4 Generalized view of a stage 1 address translation using the 4KB granule](../999.IMGS/Screenshot%20from%202025-09-24%2008-15-19.png)

[维护TLB缓存]: If all of the following are true, then a translation table entry is permitted to be cached in a Translation Lookaside Buffer(TLB):
- The entry is valid.
- Using the entry does not generate a Translation fault, an Address size fault, or an Access flag fault.


## 参考
- [D8.1.2 Translation regimes](../../../../007.BOOKs/Arm®%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/) # D8.1.2 Translation regimes