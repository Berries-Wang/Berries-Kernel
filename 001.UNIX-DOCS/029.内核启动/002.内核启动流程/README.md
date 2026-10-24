# 内核启动流程<sup>arm64</sup>· 5.9 vs 7.1.3

> 只分析 **arm64** 平台。两版内核启动流程整理,重点展示**内核符号的变化过程**。
>
> 学习/出处:
> + 本仓库源码:[000.SOURCE_CODE/000.LINUX-5.9](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S)(head.S/head.S.copy/kaslr.c)、[000.SOURCE_CODE/003.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/head.S)(head.S 与 kernel/pi/)
> + 本仓库文档:[head.symbol.md (5.9)](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.symbol.md)、[head.symbol.md (7.1.3)](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/head.symbol.md)(符号定义级分析)、[001.KASLR.md](../001.KASLR.md)(5.9 KASLR链路)、[README.md](../README.md)(5.9 QEMU启动日志实测值)
> + ARM官方资料(本仓库 `006.REFS/` 收录):[AArch64 Memory Management Guide](../../../006.REFS/learn_the_architecture_aarch64_memory_management_guide_101811_0105_00_en.pdf)、[A64 Instruction Set Guide](../../../006.REFS/learn_the_architecture_a64_instruction_set_architecture_guide_102374_0103_02_en.pdf);[ARM ARM DDI0487](../../../007.BOOKs/Arm%20Architecture%20Reference%20Manual%20for%20A-profile%20architecture/README.md)(本仓库 `007.BOOKs/`)
> + 内核自带启动协议文档:两版源码树内 `Documentation/arch/arm64/booting.rst`

## 目录
|版本文档|内容|
|-|-|
|[000.KERNEL-5.9/000.内核启动流程.md](./000.KERNEL-5.9/000.内核启动流程.md)|5.9 三阶段主流程图 + KASLR回跳环 + kimage_voffset诞生(含QEMU实测值) + 符号去向表|
|[000.KERNEL-7.1.3/000.内核启动流程.md](./000.KERNEL-7.1.3/000.内核启动流程.md)|7.1.3 三阶段主流程图 + "直线KASLR"对比 + kimage_voffset诞生 + 新生/继承/删除符号表|

## 同一个三阶段骨架
<pre>
              5.9                                    7.1.3
阶段⓪  MMU=off, PC=物理地址          阶段⓪  MMU=off或on(新), PC=物理地址
        primary_entry                       primary_entry
        __create_page_tables(纯汇编)         __pi_create_init_idmap(C代码)
        ├ idmap_pg_dir: 仅.idmap.text       ├ init_idmap_pg_dir: 整个镜像
        └ init_pg_dir: 内核VA映射            └ (内核VA映射推迟到阶段①的C代码)
─────────────────────────────────────────────────────────────────────
阶段①  MMU=on, PC=仍物理地址          阶段①  MMU=on, PC=仍物理地址
        __primary_switch                    __primary_switch
        __enable_mmu:                       __enable_mmu:
          TTBR0←idmap_pg_dir                  TTBR0←init_idmap_pg_dir
          TTBR1←init_pg_dir(映射已就绪)       TTBR1←reserved_pg_dir(空表占位!)
        __relocate_kernel(按x23)           __pi_early_map_kernel(C, 一次完成):
        blr x8 ←试探调用                       映射/KASLR/重定位/切TTBR1
                                             br x8 ←唯一一次跳转
─────────────────────────────────────────────────────────────────────
阶段②  MMU=on, PC=虚拟地址            阶段②  MMU=on, PC=(可能随机化的)虚拟地址
        __primary_switched(可能两次!)      __primary_switched(只进一次)
        kimage_voffset = kimage_vaddr−x0    kimage_voffset = VA(_text)−PA(_text)
        [KASLR: ret回阶段①重做一遍]         b start_kernel
        b start_kernel
</pre>

## 符号变迁总表(5.9 → 7.1.3)
|5.9符号/机制|7.1.3状态|替代方案|数值变化|
|-|-|-|-|-|
|_head(head.S:65)|✝删除|Image头就在 _text,第一条指令 head.S:61-62|—|
|__PHYS_OFFSET(head.S:39)|✝删除|`adrp x0, KERNEL_START`(head.S:521)运行期取PA;链接期由`_text==KIMAGE_VADDR`断言(lds:413)接管|—|
|kimage_vaddr(head.S:476-477)|✝删除|`adrp x4, _text`直接取;kaslr_offset()内联(memory.h:291-294)|—|
|TEXT_OFFSET(Makefile:136,恒0)|✝删除|构建系统移除;Image头偏移8字段恒0(head.S:63)|—|
|KIMAGE_VADDR|✓保留|memory.h:46 = MODULES_END|0xffff8000_10000000 → **0xffff8000_80000000**(模块区 128MB+128MB BPF → 单一2GB)|
|kimage_voffset|✓保留|mmu.c:54;写入 head.S:443-445(二次进入重算) → head.S:233-235(只算一次)|语义不变:VA(_text)−PA(_text)|
|idmap_pg_dir|✓保留,职责缩小|早期职责让位 init_idmap_pg_dir;由 create_idmap 重建(mmu.c:1393)|—|
|init_pg_dir|✓保留,机制重写|C函数 map_kernel() 按段填充,根表复制进 swapper_pg_dir(map_kernel.c:136)|BSS,不再显式释放|
|swapper_pg_dir|✓保留,接管提前|map_kernel 即复制根表切 TTBR1(map_kernel.c:136-138)|—|
|__create_page_tables(约700行汇编)|✝删除|__pi_create_init_idmap(map_range.c:91)+ __pi_early_map_kernel(map_kernel.c:241)|—|
|el2_setup|改名重构|init_kernel_el(head.S:270)+ finalise_el2(head.S:244)|—|
|x23(KASLR偏移寄存器)|✝"消失"|C变量 kaslr_offset(map_kernel.c:245,281)|低21位吸收物理放置偏差(新语义)|
|KASLR回跳(二次进入__primary_switched)|✝删除|early_map_kernel 内一次完成,纯虚拟KASLR|—|
|(无)|⚑新生|reserved_pg_dir(lds:243,空PGD占位TTBR1/uaccess)|5.9不存在|
|(无)|⚑新生|init_idmap_pg_dir(lds:274,.init.data,映射整个镜像)|—|
|(无)|⚑新生|early_init_stack(lds:361,早期C代码4KB栈)|—|
|(无)|⚑新能力|record_mmu_state(head.S:134):允许开着MMU进内核|—|

## 阶段对照表
|事项|5.9|7.1.3|
|-|-|-|
|入口符号|_head(与_text同址)|_text(无_head)|
|入口形态|仅MMU=off进入|MMU=off或on(record_mmu_state记录)|
|EL处理|el2_setup(回落EL1)|init_kernel_el + finalise_el2(能用VHE则留EL2)|
|初始页表创建|__create_page_tables(纯汇编)|__pi_create_init_idmap(C,map_range.c)|
|初始ID映射范围|仅.idmap.text段(约0x378字节)|整个镜像[_stext,_end]|
|开MMU时TTBR1|init_pg_dir(内核VA映射已就绪)|reserved_pg_dir(空表,映射稍后由C代码建)|
|内核VA映射创建|与ID映射同批建好|early_map_kernel→map_kernel(C,按段分权限)|
|KASLR|kaslr_early_init + 回跳关MMU重建页表 + 二次进入|early_map_kernel内直线完成(纯虚拟)|
|进入虚拟地址|blr试探 + br正式(两次)|br一次|
|kimage_voffset计算|kimage_vaddr(重定位平移后) − x0|VA(_text) − PA(_text)|
|BSS清理|__primary_switched(head.S:448-452)|early_map_kernel(map_kernel.c:252)|
|早期栈|init_thread_union|early_init_stack(专用4KB)|
|线性映射/模块区随机|kaslr_early_init内(kaslr.c:152,186)|移至kaslr_init()(kaslr.c:15,setup.c:287)|
