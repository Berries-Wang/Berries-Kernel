# 内核符号<sup>arm64</sup>
|符号名称|含义|值|定义|
|-|-|-|-|
|_text|内核镜像起始地址||arch/arm64/kernel/vmlinux.lds.S|
|__PHYS_OFFSET|物理内存的起始地址|#define __PHYS_OFFSET	(KERNEL_START - TEXT_OFFSET)|arch/arm64/kernel/head.S|
|KERNEL_START||#define KERNEL_START		_text|arch/arm64/include/asm/memory.h|
|TEXT_OFFSET||TEXT_OFFSET := 0x0|arch/arm64/Makefile|
|init_pg_dir|内核页表基地址<sup>奔跑吧Linux内核（第2版）卷1·基础架构#2．创建内核映像的页表映射</sup>||arch/arm64/kernel/vmlinux.lds.S|
|init_pg_end|||arch/arm64/kernel/vmlinux.lds.S|
|idmap_pg_dir|恒等映射页表<sup>奔跑吧Linux内核（第2版）卷1·基础架构#2．创建内核映像的页表映射</sup>||arch/arm64/kernel/vmlinux.lds.S|
|__idmap_text_start|.idmap.text段的起始地址<sup>查看000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/.tmp_System.map就可以知道这段区域有什么:汇编函数,如__enable_mmu</sup>||arch/arm64/kernel/vmlinux.lds.S|
|kimage_vaddr|内核镜像的虚拟地址|_text - TEXT_OFFSET|通过 vmlinux.lds.S发现,_text 与 (KIMAGE_VADDR + TEXT_OFFSET) 相等|
|KIMAGE_VADDR|内核镜像的虚拟地址|#define KIMAGE_VADDR		(MODULES_END)<sup>宏展开后,值: 0xFFFF800010000000</sup>|arch/arm64/include/asm/memory.h|
|kimage_voffset|内核映像虚拟地址和物理地址之间的偏移量|kimage_vaddr - __PHYS_OFFSET <sup>从head.S 的 __primary_switched 分析得了</sup>|arch/arm64/kernel/head.S|

分析[arch/arm64/kernel/head.S](./head.S.copy)之后，在看如下的图，就很清晰了:
- ![wechat_2025-11-15_164110_629.png](../../../../../../001.UNIX-DOCS/999.IMGS/wechat_2025-11-15_164110_629.png)
- ![wechat_2025-11-15_164234_984.png](../../../../../../001.UNIX-DOCS/999.IMGS/wechat_2025-11-15_164234_984.png)