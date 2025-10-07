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

[维护TLB缓存]: If all of the following are true, then a translation table entry is permitted<sup>允许；（使）有可能；:permit 的过去式及过去分词</sup> to be cached in a Translation Lookaside Buffer(TLB):
- The entry is valid.
- Using the entry does not generate a Translation fault, an Address size fault, or an Access flag fault.


TLB caching can reduce the number of required translation table lookups.(TLB能减少翻译表的查询次数)


If two address translation stages are used and when a full translation table walk at both stages is required, then all of the following apply:（如果采用两个地址转换阶段，并且在这两个阶段都需要进行完整的转换表遍历，则以下所有情况均适用） ？
- S1 is the number of lookup levels required for a stage 1 translation.
- S2 is the number of lookup levels required for a stage 2 translation.
- The maximum number of translation table lookups required is (S1+1)*(S2+1)-1.

## D8.2.2 Concatenated translation tables-串联转换表
All statements in this section apply only to the VMSAv8-64 translation system.

For the initial lookup of a stage 2 address translation, up to 16 translation tables can be concatenated(在二级地址转换的初始查找过程中，最多可串联 16 个转换表。)

For a stage 2 translation, if the translation table in the initial lookup level would require 16 or fewer entries, then the stage 2 translation can be configured to have all of the following properties:（在二级转换中，若初始查找级别的转换表所需的条目数为 16 个或更少，则可将该二级转换配置为具备以下所有特性：）
- The initial lookup of the stage 2 translation starts at the next lookup level.(第二级转换的初始查找从下一查找级别开始。)
- A number of translation tables corresponding to the original number of translation table entries at the previously initial lookup level are concatenated at that next lookup level.（在下一查找级别，将与初始查找级别原始转换表条目数相对应的若干转换表进行串联）
  + 指的是下一级别查询依赖于上一级的输出吧 ？？？

For the 16KB translation granule, if the Effective value of VTCR_EL2.DS is 0 and a 48-bit address size is translated by the stage 2 translation, then the initial lookup is required to start at level 1 with two concatenated translation tables.（对于16KB转换粒度，若VTCR_EL2.DS的有效值为0且第二阶段转换需处理48位地址空间时，初始查找需从第一层级开始，并使用两个串联的转换表。）？？

For the initial lookup in a stage 2 translation, if concatenated translation tables are used, then all of the following apply:（在第二阶段转换的初始查找中，若采用串联转换表，则以下规则全部适用：）
- Up to four additional IA bits are resolved by the initial lookup.（初始查找最多可解析4个附加IA位）-- IA： Input Address
- For each additional n IA bits resolved by the initial lookup, the number of least significant translation table base address bits held in the TTBR_ELx is reduced by n bits.（初始查找每解析n个附加IA位，TTBR_ELx中保留的转换表基地址最低有效位将减少n位）
- For each additional n IA bits resolved by the initial lookup, 2n concatenated translation tables are required at the initial lookup level.（初始查找每解析n个附加IA位，初始查找层级需使用2ⁿ个串联转换表）


Using a concatenated translation table eliminates(排除) the overhead of an extra(额外的) lookup level.(使用串联转换表可消除额外查找级别的开销。)


If concatenated translation tables are used, then software is required to do all of the following:（若使用级联转换表（Concatenated Translation Tables），则软件需执行以下所有操作：）
- Align the base address of the first translation table to the sum of the size of the memory occupied by the concatenated translation tables.（将第一个转换表的基地址与级联转换表所占用的内存大小之和进行对齐。）
- Program VTTBR_EL2 or VSTTBR_EL2 with the address of the first translation table in the set of concatenated translation tables.（向 VTTBR_EL2 或 VSTTBR_EL2 寄存器写入级联转换表集合中第一个转换表的地址。）
- Program VTCR_EL2 or VSTCR_EL2 with the initial lookup level.（向 VTCR_EL2 或 VSTTCR_EL2 寄存器写入初始查找级别（Initial Lookup Level））-- 级别，即页表的级别数量
- 这段话怎么理解?[001.UNIX-DOCS/023.从问题来探索Linux内核/001.ARM64在Linux内核中的实现/019.MMU(Memory Management Unit)/002.解释说明/000.级联转换表工作流程.md](./002.解释说明/000.级联转换表工作流程.md)




## D8.2.3 Translation table base address register
For a translation stage, the translation table base address register, TTBR_ELx, holds the translation table base address of the initial lookup.（TTBR_ELx基地址寄存器保存的是首次查找的翻译表的基地址）

For a translation stage that supports two VA ranges, two translation table base address registers are required.
- 这个指的是 
   + 0x0000 0000 0000 0000～0x0000 FFFF FFFF FFFF    (用户空间)
   + 0xFFFF 0000 0000 0000～0xFFFF FFFF FFFF FFFF    (内核空间)
- 这两段吗? 是的，这段话怎么理解:
- ARM架构为了高效、安全地同时管理内核空间和用户空间，将虚拟地址空间划分为两个范围。为了实现每个范围都能有独立的映射规则（即独立的页表），硬件上提供了两个基地址寄存器（TTBR0和TTBR1）来分别指向这两个页表。MMU根据虚拟地址的最高位自动选择使用哪个寄存器，从而实现了内核与用户进程的隔离，以及进程间的快速切换。

Software can use TCR_ELx.TnSZ to configure the translation stage IA size to be smaller than the supported size.(软件可以使用 TCR_ELx.TnSZ 来将转换阶段的输入地址大小配置为小于所支持的大小)
- 怎么理解? 
  + 虽然 ARM 处理器硬件能够处理很长的虚拟地址（比如 48 位），但操作系统软件可以通过配置 TCR_ELx 寄存器中的 T0SZ/T1SZ 字段，来告诉 MMU：“我当前使用的虚拟地址空间没用到全部位数，只用了其中的一部分（比如 39 位或 42 位）” ?
-  ARMv8-A  中支持的最大虚拟地址位宽是多少?

If EL2 is disabled or not implemented, then for the stage 1 EL1&0 regime, all of the following apply:(如果 EL2 被禁用或未实现，那么对于阶段 1 的 EL1&0 转换体系（translation regime），以下所有描述均适用：)
- The translation table base address held in the TTBR_ELx register is a PA.
- The translation table base address returned by a Table descriptor is a PA.

If EL2 is enabled, then for all address translation stages other than stage 1 in EL1&0, all of the following apply:
- The translation table base address held in the TTBR_ELx register is a PA.
- The translation table base address returned by a Table descriptor is a PA.


If EL2 is enabled, then for stage 1 address translations in EL1&0, all of the following apply:(如果 EL2 被启用，那么对于 EL1&0 中的阶段 1 地址转换，以下所有描述均适用): 参考: [EL2 is enabled](./002.解释说明/001.EL2%20is%20enabled.md)
- The stage 1 translation table base address held in the TTBR_ELx register is an IPA.(存储在 TTBR_ELx 寄存器中的阶段 1 转换表基地址是一个 IPA)
- The stage 1 translation table base address returned by a Table descriptor is an IPA.(由页表描述符返回的阶段 1 转换表基地址也是一个 IPA)
- Accesses to stage 1 translation tables are subject to a stage 2 translation.(对阶段 1 转换表的访问需要经过阶段 2 转换)
- EL2 是和虚拟化相关的内容了,相关术语：
    + IPA： 中间物理地址。这是 Guest OS（虚拟机）认为自己使用的“物理地址”。然而，它并非真正的硬件物理地址。
    + 阶段 1 转换： 由 Guest OS 的 MMU 和页表管理的转换，负责将 Guest OS 内的虚拟地址 转换为 IPA。
    + 阶段 2 转换： 由 Hypervisor（在 EL2）管理的转换，负责将 Guest OS 产生的 IPA 转换为真正的物理地址。
    + TTBR_ELx 寄存器： Guest OS 内核认为的页表基地址寄存器。
    + 页表描述符： 页表中的一个条目，指向下一级页表或物理页帧。

For an address translation stage, the translation table base address in TTBR_ELx is defined for the supported OA size of that stage.(对于一个地址转换阶段，TTBR_ELx 中的转换表基地址是针对该阶段所支持的输出地址大小定义的。)
- 怎么理解? CPU 设计时规定，当你设置 TTBR_ELx 寄存器来指向一个页表时，你必须确保这个页表的格式和预期能够产生的物理地址范围，与当前转换阶段所配置的物理地址位宽 相匹配
- 如果 MMU 被配置为支持 40 位的物理地址空间（即 OA Size = 40 位），那么存储在 TTBR_ELx 中的页表基地址，以及整个页表的结构，都必须是为生成 40 位物理地址而设计的。你不能在此配置下使用一个只能输出 32 位物理地址的页表结构。

For the VMSAv8-64 translation system, the number of {I}PA bits held in TTBR_ELx is determined by the granule size and OA address size. The bits used for each granule size when using the maximum OA address size of 48 bits or 52 bits are shown in the following table. Software might be required to set one or more of the low-order base address bits to zero to align the table to the table size （对于 VMSAv8-64 转换系统，TTBR_ELx 寄存器中存储的 {中间}物理地址（IPA） 的位数由颗粒大小 和输出地址大小 共同决定。下表（Table D8-10 {I}PA bits held in TTBR_ELx，具体看手册）展示了在使用最大为 48 位或 52 位的输出地址时，每种颗粒大小所使用的地址位。软件可能需要将一个或多个低序位的基本地址位设置为零，以使页表与页表大小对齐。）
- VMSAv8-64 转换系统： 这是 ARMv8-A 架构中用于 64 位虚拟地址空间的虚拟内存系统架构。
- {I}PA： 这里的 {I}PA 表示 IPA 或 PA。
   + 在非虚拟化环境（EL2 禁用）下，它就是 PA。
   + 在虚拟化环境（EL2 启用）下，对于 EL1&0 的阶段 1 转换，它就是 IPA。
- TTBR_ELx 中存储的 {I}PA： 指的是 TTBR_ELx 寄存器中存储的页表基地址本身所代表的地址值。这个地址是一个 {I}PA。
- 颗粒大小： 指页表映射的内存页大小，通常为 4KB、16KB 或 64KB。它决定了页表的结构和层级。
- 输出地址大小： 指该转换阶段输出的地址位宽，即 {I}PA 的位宽（如 48 位）。
- 低序位的基本地址位设置为零： 这意味着页表在内存中的起始地址必须是页表本身大小的整数倍。例如，一个占用 4KB 内存的页表，其基地址必须是 4KB 的倍数（即地址的低 12 位必须为 0）。这是一种对齐要求。

If an address translation stage uses an OA size smaller than the maximum, then the upper bits of the translation table base address in TTBR_ELx corresponding to the upper bits of OA size are required to be set by software to zero.（部分高位要设置为0： 如果一个地址转换阶段使用的输出地址大小小于最大值，那么软件必须将 TTBR_ELx 中转换表基地址的、对应于输出地址大小上限的高位部分设置为零）

If TCR_ELx.TnSZ specifies an IA size that is smaller than the maximum size resolved at the initial lookup level, then more low-order TTBR_ELx bits are used to hold the translation table base address.(如果 TCR_ELx.TnSZ 指定的输入地址大小小于在初始查找级别所解析的最大大小，那么将使用更多的 TTBR_ELx 低序位来存储转换表基地址。)

If an address translation stage uses an OA size smaller than the maximum and if the bits above the configured OA size of the translation table base address in TTBR_ELx are not set to zero, then an Address size fault is generated and reports all of the following:（如果某个地址转换阶段使用的输出地址大小小于最大值（上述的 “更高位”不为 0 ），并且 TTBR_ELx 中转换表基地址的高于已配置 OA 大小的位未被设置为零，则将生成一个地址大小错误，并报告以下所有信息：）
- A translation level 0 lookup as generating the fault, regardless of whether or not the translation stage starts with a level 0 lookup.（将错误报告为由转换等级 0 查找所产生，而无论该转换阶段是否实际从等级 0 查找开始。）
- The translation stage that generated the fault.（报告生成该错误的转换阶段。）
- 这是一个硬件错误检查机制
  ```txt
    触发条件（错误何时发生）：
     软件配置了较小的物理地址空间（例如，OA Size = 40位）。
     但是，软件在设置 TTBR_ELx 寄存器时，写入了超出这个已配置范围的基地址。例如，基地址的第 41 位或更高位不为 0（比如地址是 0x1_0000_8000，而有效的 40 位地址范围是 0x0_0000_0000 到 0x0_FFFF_FFFF）。

    硬件响应（发生什么）：
      MMU 在开始进行地址转换之前，会先检查 TTBR_ELx 中的值是否符合 TCR_ELx 中配置的 OA 大小规则。
      一旦发现不符合（即高位非零），MMU 会立即中止本次地址转换，并产生一个同步异常——即地址大小错误。
  ```

Direct writes to TTBR0_ELx and TTBR1_ELx occur in program order relative to one another, without the need for explicit synchronization. For any one translation, all indirect reads of TTBR0_ELx and TTBR1_ELx that are made as part of the translation observe only one point in that order of direct writes.(对 TTBR0_ELx 和 TTBR1_ELx 的直接写入操作会按照程序顺序相对于彼此执行，无需显式的同步操作。对于任何一次地址转换，作为转换一部分而间接读取 TTBR0_ELx 和 TTBR1_ELx 时，所观察到的都只是那个直接写入顺序中的一个一致的状态点。)
- ARM 架构为保证内存管理单元行为确定性而提供的内存排序保证
  ```txt
     这段话描述了 ARM 架构为保证内存管理单元行为确定性而提供的**内存排序保证**，主要涉及两个方面：
       **1. 写入顺序（软件视角）：**
       *   **"直接写入...按照程序顺序"**：这意味着，如果软件代码中先写 `TTBR1_EL1`，紧接着再写 `TTBR0_EL1`，那么硬件保证 `TTBR1_EL1` 的写入一定在 `TTBR0_EL1` 的写入之前对系统可见。
       *   **"无需显式的同步操作"**：程序员在连续写入这两个寄存器后，不需要插入 `DSB` 这样的内存屏障指令来确保写入顺序。硬件已经为此提供了内在的保证。这简化了操作系统在进行进程切换（更新 TTBR0）或修改内核映射（更新 TTBR1）时的操作。
       
       **2. 读取一致性（硬件/MMU 视角）：**
       *   **"对于任何一次地址转换"**：这是指 MMU 为处理**一条单独的指令**所执行的一次完整的地址翻译过程。
       *   **"间接读取"**：指 MMU 在翻译一个虚拟地址时，需要自动读取 `TTBR0_ELx` 或 `TTBR1_ELx` 以获取页表基地址。这个读取操作是由硬件自动完成的，而非软件的 `LDR` 指令。
       *   **"观察到一个一致的状态点"**：这是最核心的保证。它意味着，在翻译**一个**虚拟地址的过程中，无论这个地址是落在 TTBR0 的范围还是 TTBR1 的范围，MMU 看到的这两个寄存器的值，必定是来自**同一次软件写入序列**中的某个瞬间状态。
  ```

Consistent with the general requirements for direct writes to System registers, direct writes to TTBR_ELx are not required to be observed by indirect reads until completion of a Context synchronization event.（根据对系统寄存器直接写入的通用要求，对 TTBR_ELx 的直接写入，在上下文同步事件完成之前，不要求被间接读取操作所观察到）
```txt
关键术语解释：
    直接写入： 由软件执行的、明确的写入 TTBR_ELx 寄存器的指令（例如 MSR TTBR0_EL1, Xn）。
    间接读取： 指内存管理单元在进行地址转换时，自动读取 TTBR_ELx 寄存器以获取页表基地址的行为。这不是一条软件指令，而是硬件行为。
    上下文同步事件： 这是一类特殊的指令或事件，它们能强制让之前所有未完成的系统寄存器配置对后续所有操作可见。最典型的例子是 ISB 指令。

    这意味着，当软件执行一条写入 TTBR_ELx 的指令后，这个新值可能不会立即被正在进行的或新发起的地址转换操作所使用。MMU 可能在一段时间内仍然使用旧的寄存器值。

    为什么会这样？
      这是为了提升处理器性能。允许硬件延迟应用新的系统寄存器设置，可以避免流水线频繁停顿，使得执行和内存访问单元能够更流畅地工作。

    软件需要做什么？（重要实践）
      因此，在进行了可能影响后续指令执行的关键系统寄存器（尤其是像 TTBR_ELx 这种改变地址空间的寄存器）配置之后，软件必须主动插入一个上下文同步事件（通常是 ISB 指令），来确保修改能够被后续的指令准确地看到。

```

---

## D8.2.4 Selection between TTBR0_ELx and TTBR1_ELx when two VA ranges are supported
If a stage 1 translation regime supports two VA ranges, then the translation regime TTBR_ELx registers point to all of the following address ranges:（如果阶段1转换机制支持两个VA范围，那么该转换机制的TTBR_ELx寄存器将指向以下所有地址范围：）
- For the lower VA range that begins at address 0x0000000000000000, TTBR0_ELx points to the translation table for the initial lookup level.
- For the upper VA range that ends at address 0xFFFFFFFFFFFFFFFF, TTBR1_ELx points to the translation table for the initial lookup level

If a stage 1 translation regime supports two VA ranges, then the TCR_ELx.{T0SZ, T1SZ} fields configure all of the following address range sizes:
- The lower VA range is 0x0000000000000000 to (2^(64-T0SZ) - 1).
- The upper VA range is (2^64 - 2^(64-T1SZ) ) to 0xFFFFFFFFFFFFFFFF.

If a stage 1 translation regime supports two VA ranges, when an accessed address is not in the lower VA range or the upper VA range, a stage 1 level 0 Translation fault is generated.(若阶段一的翻译机制支持两段虚拟地址，当访问地址既不在低地址范围，也不在高地址范围，那么会产生一个阶段一的级别0的翻译错误)

The following figure illustrates<sup>阐明；举例说明（illustrate 的三单形式）</sup> the two address ranges translated by the tables the TTBR_ELx registers point to.

![Screenshot from 2025-09-27 08-56-40.png](../999.IMGS/Screenshot%20from%202025-09-27%2008-56-40.png)
- 结合[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure]#▲图2.9　ARM64在Linux 5.0内核的内存分布 学习，简直一样
  + ![Screenshot from 2025-09-27 09-00-40.png](../999.IMGS/Screenshot%20from%202025-09-27%2009-00-40.png)

If a stage 1 translation regime supports two VA ranges, then all of the following are used to select the TTBR_ELx:(如何选择 TTBR_ELx寄存器)
- If VA bit[55] is zero, then TTBR0_ELx is selected.
- If VA bit[55] is one, then TTBR1_ELx is selected.
- 为什么是 bit[55] , 有些资料里面就是bit[63]<sub>[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure]: 当虚拟地址第63位（简称VA[63]）为1时选择TTBR1；当VA[63]为0时选择TTBR0。</sub> ， 为什么呢?
   + 难道就因为下面这句话? "VMSAv9-128 translation system" ， 因为目前支持48位(T0SZ=16)的虚拟地址!!!
     ```
       个人理解! 使用VA[55] VA[63] 是等价的
     ```

For the VMSAv9-128 translation system, if a stage 1 translation regime supports two VA ranges, then the maximum supported VA width is 55 bits, and the smallest permitted value of the TnSZ field is 9.（对于VMSAv9-128转换系统，如果阶段1转换机制支持两个VA范围，那么支持的最大VA宽度为55位，且TnSZ字段允许的最小值为9。）

### D8.2.4.1 Preventing EL0 access to halves of the address map<sub>禁止EL0（异常级别0，通常指用户态）访问部分地址映射空间</sub>
All statements in this section require implementation of FEAT_E0PD.

The TCR_ELx.{E0PD0, E0PD1} fields can be used to prevent EL0 access to the addresses translated by the corresponding TTBR0_ELx or TTBR1_ELx.

When the TCR_ELx.{E0PD0, E0PD1} fields prevent EL0 access to an address translated by TTBR0_ELx or TTBR1_ELx, then a level 0 Translation fault is generated.（当TCR_ELx的{E0PD0, E0PD1}字段阻止EL0访问由TTBR0_ELx或TTBR1_ELx转换的地址时，将产生0级转换错误。）

When the TCR_ELx.{E0PD0, E0PD1} fields generate a level 0 Translation fault, then all of the following apply:
- The time needed to take the fault should be the same whether or not the address accessed is present in a TLB, to mitigate attacks that use fault timing.（无论访问的地址是否存在于TLB中，触发故障所需的时间都应相同，以防范利用故障时序发起的攻击）
- The fault generated does not affect any micro-architectural state of the PE in a manner that is different if the address accessed is present in a TLB or not, to prevent this information being used to determine the presence of the address in a TLB.（为确保无法通过此类信息判断某地址是否存在于TLB中，所生成的故障对处理单元微架构状态的影响，不会因访问地址是否存在于TLB中而产生差异。）

## D8.2.5 Translation table and translation table lookup properties
Translation table and translation table lookup properties include the table size, table alignment, table endianness, and memory attributes.（翻译表和翻译表查找属性包括表大小、表对齐方式、表字节序以及内存属性。）

### D8.2.5.1 Translation table size
The descriptor size in a translation table entry is one of the following:
- For the VMSAv8-64 translation system, an eight-byte, or 64-bit, object.
- For the VMSAv9-128 translation system, a 16-byte, or 128-bit, object.（对于VMSAv9-128转换系统而言，其操作对象为16字节（即128位）的数据单元）

If n is the number of bits resolved by a lookup level, then the number of translation table entries required at that lookup level is 2^n .
```txt
  就是说用虚拟地址的那些地址位来作为索引，那么条目数量就是2^n个
```

The size of a translation table in bytes is determined by multiplying the number of entries by the descriptor size.(翻译表的大小（以字节为单位）由条目数量乘以描述符大小所决定。)

The maximum number of translation table entries is determined by the translation granule size, which is defined by one of the following:（翻译表条目的最大数量由转换粒度大小决定，该大小由以下之一定义：）
- For a stage 1 translation that supports one VA range, TCR_ELx.TG0.
- For a stage 1 translation that can support two VA ranges, all of the following:
  + For the lower VA range, TCR_ELx.TG0.
  + For the upper VA range, TCR_ELx.TG1.
- For a stage 2 translation in the Non-secure EL1&0 translation regime, VTCR_EL2.TG0.
- For a stage 2 translation in the Secure EL1&0 translation regime, one of the following:
  + If the stage 2 IA is a Non-secure IPA, then VTCR_EL2.TG0.
  + If the stage 2 IA is a Secure IPA, then VSTCR_EL2.TG0.
- For a stage 2 translation in the Realm EL1&0 translation regime, VTCR_EL2.TG0.

### D8.2.5.2 Translation table alignment
A translation table is required to be aligned to one of the following
- For the VMSAv8-64 translation system, if the translation table has fewer than eight entries and an OA size greater than 48 bits is used, then the table is aligned to 64 bytes.（对于VMSAv8-64转换系统，若翻译表的条目数少于八条且采用大于48位的OA（输出地址）宽度，则该表需按64字节对齐。）
- Otherwise, the translation table is aligned to the size of that translation table.(否则，该翻译表需按其自身的大小进行对齐)

Only when all of the following are true is it possible to have fewer than 8 translation table entries:
- The translation table is at the initial lookup level.
- Concatenated translation tables are not used.

If concatenated translation tables are used, then the concatenated translation tables are required to be aligned to the overall size of the memory occupied by the concatenated translation tables.（若采用级联翻译表，则级联翻译表需按其整体所占内存空间的大小进行对齐）
```txt
   需要在代码中求证!!!
```

## 后续内容看文档吧，内容太多了，有需要再继续查看


## Q&A
### 如何禁用和启用MMU






## 参考
- [D8.1.2 Translation regimes](../../../../007.BOOKs/Arm®%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/) # D8.1.2 Translation regimes