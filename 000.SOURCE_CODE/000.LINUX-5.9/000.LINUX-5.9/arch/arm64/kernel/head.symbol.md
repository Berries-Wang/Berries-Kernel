# 内核符号<sup>arm64</sup>
|符号名称|含义|值|定义|
|-|-|-|-|
|_text|内核镜像起始地址(虚拟地址)||arch/arm64/kernel/vmlinux.lds.S|
|_start|内核入口第一条指令地址(虚拟地址,位于.head.text开头,数值上与_text相等)||arch/arm64/kernel/vmlinux.lds.S|
|_end|内核镜像结束地址(虚拟地址)||arch/arm64/kernel/vmlinux.lds.S|
|__PHYS_OFFSET|内核镜像实际被加载到的物理地址(2MB对齐的加载基址)<sup>注意:不是"物理内存(RAM)的起始地址"! adrp	x0, __PHYS_OFFSET 执行后,x0 = 内核镜像实际加载的物理地址(如RAM起始0x80000000、内核加载于0x80200000时,x0=0x80200000);RAM的起始地址是启动后期从DT/UEFI内存映射得出的 memstart_addr,两者通常不同。另外,该宏的链接期数值 = _text - TEXT_OFFSET(与kimage_vaddr数值相同的虚拟地址),从不被当作立即数使用,只作为ADRP标号,运行期按PC相对寻址解析出实际物理地址。阅读:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy](../../../../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy) kimage_voffset计算规则 即可理解</sup>|#define __PHYS_OFFSET	(KERNEL_START - TEXT_OFFSET)|arch/arm64/kernel/head.S|
|KERNEL_START||#define KERNEL_START		_text|arch/arm64/include/asm/memory.h|
|TEXT_OFFSET|内核镜像相对2MB对齐加载基址的字节偏移<sup>Makefile注释: "The byte offset of the kernel image in RAM from the start of RAM";与KASLR无关,5.9中无条件设置为0</sup>|TEXT_OFFSET := 0x0|arch/arm64/Makefile|
|init_pg_dir|早期启动的临时内核页表基地址<sup>奔跑吧Linux内核（第2版）卷1·基础架构#2．创建内核映像的页表映射;paging_init完成后即被释放(mm/mmu.c: memblock_free(__pa_symbol(init_pg_dir),...)),长期使用的内核PGD是swapper_pg_dir</sup>||arch/arm64/kernel/vmlinux.lds.S|
|init_pg_end|||arch/arm64/kernel/vmlinux.lds.S|
|idmap_pg_dir|恒等映射页表<sup>奔跑吧Linux内核（第2版）卷1·基础架构#2．创建内核映像的页表映射</sup>||arch/arm64/kernel/vmlinux.lds.S|
|__idmap_text_start|.idmap.text段的起始地址<sup>查看000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/.tmp_System.map就可以知道这段区域有什么:汇编函数,如__enable_mmu</sup>||arch/arm64/kernel/vmlinux.lds.S|
|kimage_vaddr|内核镜像的虚拟地址|_text - TEXT_OFFSET<sup>head.S中 SYM_DATA 定义: .quad _text - TEXT_OFFSET;由 vmlinux.lds.S 的 _text = KIMAGE_VADDR + TEXT_OFFSET 可知其链接期数值即 KIMAGE_VADDR。注意:"虚拟地址_text - 物理地址_text" 是 kimage_voffset 的公式,不是本符号的值!阅读:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy](../../../../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy) kimage_voffset计算规则 即可理解</sup>|arch/arm64/kernel/head.S|
|KIMAGE_VADDR|内核镜像的虚拟地址|#define KIMAGE_VADDR		(MODULES_END)<sup>宏展开后,值: 0xFFFF800010000000(依赖VA_BITS=48、未开KASAN的默认配置)</sup>|arch/arm64/include/asm/memory.h|
|kimage_voffset|内核映像虚拟地址和物理地址之间的偏移量|虚拟地址_text - 物理地址_text <sup>即 kimage_vaddr - x0(x0来自 adrp x0, __PHYS_OFFSET 运行期解析出的内核镜像实际加载物理地址),从head.S 的 __primary_switched 分析得来</sup>|变量定义: arch/arm64/mm/mmu.c (u64 kimage_voffset __ro_after_init);写入处: arch/arm64/kernel/head.S|
|swapper_pg_dir|长期使用的内核页表PGD基地址(虚拟地址)<sup>TTBR1 / init_mm.pgd;与init_pg_dir(早期临时页表,启动后释放)相对</sup>||arch/arm64/kernel/vmlinux.lds.S|

---

### 计算 kimage_voffset 
<pre>
kimage_voffset = kimage_vaddr - __PHYS_OFFSET   
               = _text - TEXT_OFFSET - (KERNEL_START - TEXT_OFFSET) 
               = _text - KERNEL_START 
               = _text - _text = 0 ? 
    绝对不可能!!!,但是问题出现在哪里呢?

    (其实拿链接期常量做减法,结果确实是0: __PHYS_OFFSET 的链接期数值 = KERNEL_START - TEXT_OFFSET = _text - TEXT_OFFSET,
     与 kimage_vaddr 的链接期数值完全相同(一个虚拟地址),代数没错;
     错在把 __PHYS_OFFSET 当作数值来用 —— 它几乎从不被当作立即数,唯一用法是作 ADRP 标号,运行期按 PC 相对寻址解析出实际物理地址)

    问题出在 “__PHYS_OFFSET ” 这里,从 <a href="../../../../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy">000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy</a> 中发现,
    
    kimage_voffset = x4 - x0 , 这个x0寄存器存储值时的指令: adrp	x0, __PHYS_OFFSET , 这表示取的是标签 __PHYS_OFFSET 所在物理页的基地址,即内核镜像实际加载的物理地址 (为什么?参考:<a href="../../../../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy">000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy</a> 宏: __primary_switched)
    注意:标签是 __PHYS_OFFSET 而非 _text,两者只因 5.9 中 TEXT_OFFSET=0 才重合;若 TEXT_OFFSET≠0(老内核),
    解析出的是2MB对齐的加载基址而非 _text 所在页 —— 这正是 __PHYS_OFFSET 定义中 "- TEXT_OFFSET" 的意义:由 _text 反推2MB对齐的加载基址
    
    而kimage_vaddr = _text - TEXT_OFFSET (head.S中定义)
    > 这里的 _text 表示虚拟地址 (为什么? 参考:<a href="../../../../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy">000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy</a> 宏: __primary_switched)
    
    所以，kimage_voffset = _text的虚拟地址 - _text的物理地址  = kimage_voffset就是内核起始(第一条指令)虚拟地址相对于内核起始(第一条指令)物理地址的偏移量
    > 所以，kimage_voffset 是一个偏移量，且他不等于0！(KASLR 下运行期虚拟地址同样会被随机化,因此必须在运行期计算)

    _text = . = KIMAGE_VADDR + TEXT_OFFSET  (vmlinux.lds.S)

    更多内容，参考:<a href="../../../../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy">000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy</a> 宏: __primary_switched
</pre>


> 非常重要! 在 ARM64 Linux 内核启动过程中，**内核镜像可以被加载到物理内存中任意 2MB 对齐的位置（KASLR，对齐要求 MIN_KIMG_ALIGN）**，但编译时是基于一个固定的虚拟地址（KIMAGE_VADDR）。
> 注意两点: ① KASLR 下运行期**虚拟地址同样会被随机化**(不只是物理位置)，所以 kimage_voffset 必须在运行期计算; ② TEXT_OFFSET 与 KASLR 无关 —— 5.9 的 arch/arm64/Makefile 中无条件 `TEXT_OFFSET := 0x0`(其注释自述为 "The byte offset of the kernel image in RAM from the start of RAM")，老内核中默认值非 0，属历史遗留的镜像放置约定，并非"为了处理 KASLR 而设置"

---

分析[arch/arm64/kernel/head.S](./head.S.copy)之后，再看如下的图，就很清晰了:
- ![wechat_2025-11-15_164110_629.png](../../../../../../001.UNIX-DOCS/999.IMGS/wechat_2025-11-15_164110_629.png)
- ![wechat_2025-11-15_164234_984.png](../../../../../../001.UNIX-DOCS/999.IMGS/wechat_2025-11-15_164234_984.png)