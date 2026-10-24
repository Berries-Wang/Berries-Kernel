# 内核符号<sup>arm64 · LINUX 7.1.3</sup>

> 对照 5.9 版文档:[head.symbol.md (5.9)](../../../../../000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.symbol.md)。
> 5.9 → 7.1.3 早期启动代码经历了大重构(Ard Biesheuvel 的 PI 系列重构):汇编千行的 `__create_page_tables` 消失,页表创建/KASLR/重定位全部改由 C 代码(arch/arm64/kernel/pi/)完成;`_head`、`__PHYS_OFFSET`、`kimage_vaddr`、`TEXT_OFFSET` 等符号被删除。
> 注:7.1.3 源码树无构建产物,本文数值均为按默认配置(VA_BITS=48、4K页、无LPA2/KASAN)从代码推算,非 System.map 实测。

|符号名称|含义|值|定义|
|-|-|-|-|
|_text|内核镜像起始地址(虚拟地址),**同时就是 Image 头/第一条指令所在**(MZ NOP + b primary_entry)<sup>注意:7.1.3 已无 _head 符号!5.9 中 _head 与 _text 同地址;lds 断言 _text == KIMAGE_VADDR([vmlinux.lds.S:413](./vmlinux.lds.S)),故 _text 必然 2MB 对齐。arm64 同样没有 _start 符号(勿与x86/arm32混淆),入口标号是 primary_entry([head.S:85](./head.S))</sup>|_text = KIMAGE_VADDR|[vmlinux.lds.S:189-192](./vmlinux.lds.S) (ENTRY(_text) 在 lds:94)|
|_end|内核镜像结束地址(虚拟地址)<sup>BSS 尾部依次是:init_pg_dir(lds:356)、early_init_stack(lds:361),然后才是 _end</sup>||[vmlinux.lds.S:366](./vmlinux.lds.S)|
|KERNEL_START|_text 的别名宏<sup>__primary_switch 用 `adrp x0, KERNEL_START` 取物理地址([head.S:521](./head.S)),取代了 5.9 的 `adrp x0, __PHYS_OFFSET`;KERNEL_END(_end 别名)在 [memory.h:69](../include/asm/memory.h)</sup>|#define KERNEL_START _text|[memory.h:68](../include/asm/memory.h)|
|KIMAGE_VADDR|内核镜像的虚拟地址|#define KIMAGE_VADDR (MODULES_END)<sup>宏展开后,默认配置(VA_BITS=48)值: 0xffff8000_80000000 —— 7.1.3 的模块区扩大为 2GB(MODULES_VSIZE=SZ_2G,memory.h:49;5.9 是 BPF 128M + modules 128M,故为 0xffff8000_10000000)。断言 _text==KIMAGE_VADDR 在 lds:413</sup>|[memory.h:46](../include/asm/memory.h) (MODULES_END: memory.h:47,MODULES_VADDR: memory.h:48)|
|kimage_voffset|内核映像虚拟地址和物理地址之间的偏移量|VA(_text) − PA(_text)<sup>即 `adrp x4, _text`(PC=虚拟地址)− `adrp x0, KERNEL_START`(PC=物理地址),从 head.S 的 __primary_switched 分析得来(head.S:233-235);kimage_vaddr 符号已删,不再参与。__kimg_to_phys/__phys_to_kimg 换算宏在 memory.h:383/401;运行期位移另有 kaslr_offset() 内联函数 = &_text − KIMAGE_VADDR(memory.h:291-294)</sup>|变量定义: [mmu.c:54](../mm/mmu.c) (u64 kimage_voffset __ro_after_init);唯一写入处: [head.S:233-235](./head.S) (__primary_switched)|
|init_idmap_pg_dir|初始恒等映射(ID map)页表,符号名带 __pi_ 前缀<sup>7.1.3 大改:①不再由汇编 __create_page_tables 创建,改由 C 函数 __pi_create_init_idmap 创建([pi/map_range.c:91](./pi/map_range.c),head.S:92-94 调用);②覆盖范围不再只是 .idmap.text 段(5.9 约0x378字节),而是**整个镜像 [_stext,_end]**(map_range.c:101-106);③位于 .init.data(lds:274,夹在 __init_begin..__init_end 之间),随 free_initmem 释放。__pi_ 前缀来自 pi/Makefile 的 objcopy --prefix-symbols=__pi_</sup>|大小 INIT_IDMAP_DIR_SIZE|[vmlinux.lds.S:274-276](./vmlinux.lds.S) (尺寸 INIT_IDMAP_DIR_SIZE=[kernel-pgtable.h:62](../include/asm/kernel-pgtable.h))|
|idmap_pg_dir|长期使用的恒等映射页表<sup>主核早期用的是 init_idmap_pg_dir;本表由 paging_init→create_idmap 重建([mmu.c:1425](../mm/mmu.c) 调用 [mmu.c:1393](../mm/mmu.c)),只映射 .idmap.text 段(lds:395 仍断言其≤4K,原因在此),供二级 CPU(secondary_startup,head.S:375)与 cpu_resume 等使用</sup>||[vmlinux.lds.S:235](./vmlinux.lds.S) (仅1页根表,子表是 mmu.c:1388 的静态 idmap_ptes)|
|init_pg_dir|早期内核映射(VA映射)页表,符号名带 __pi_ 前缀<sup>与 5.9 同名但机制全变:由 C 函数 map_kernel() 按段填充(_text.._stext.._etext..init段.._data.._end 共6段,[pi/map_kernel.c:85-96](./pi/map_kernel.c)),填好后立即把根表复制进 swapper_pg_dir 并切 TTBR1(map_kernel.c:99→136-138)。7.1.3 不再有 memblock_free 显式释放(5.9 在 paging_init 中释放),它位于 BSS 尾部的镜像保留区 [_text,_end) 内([mm/init.c:290](../mm/init.c))</sup>|大小 INIT_DIR_SIZE|[vmlinux.lds.S:356-358](./vmlinux.lds.S) (尺寸 INIT_DIR_SIZE=[kernel-pgtable.h:58](../include/asm/kernel-pgtable.h),位于BSS)|
|swapper_pg_dir|长期使用的内核页表PGD基地址(TTBR1 / init_mm.pgd)<sup>7.1.3:启动早期即由 map_kernel 把 init_pg_dir 的根表整页复制过来(map_kernel.c:136)并切入 TTBR1(map_kernel.c:138);paging_init 再补全线性映射(mmu.c:1422 map_mem)</sup>||[vmlinux.lds.S:246](./vmlinux.lds.S)|
|reserved_pg_dir|空PGD(全0)<sup>①早期开MMU时给 TTBR1 占位(head.S:509)——此刻内核尚无任何VA映射,TTBR1必须指向合法空表;②此后用于 uaccess 切换(mmu_context.h:41-45)、init_task 的 TTBR0(setup.c:369)、__idmap_cpu_set_reserved_ttbr1(proc.S:180-188)</sup>||[vmlinux.lds.S:243](./vmlinux.lds.S) (1页)|
|tramp_pg_dir|KPTI trampoline 页表(CONFIG_UNMAP_KERNEL_AT_EL0)||[vmlinux.lds.S:239](./vmlinux.lds.S)|
|early_init_stack|早期 C 运行时(pi/ 代码)的 4KB 栈<sup>5.9 没有的新符号;primary_entry(head.S:89-91)与 __primary_switch(head.S:513-515)两次把 SP 设到这里,位于 BSS 尾部</sup>||[vmlinux.lds.S:361-362](./vmlinux.lds.S)|
|__idmap_text_start|.idmap.text 段的起始地址<sup>必须以物理地址(PC)执行的代码住这里:__enable_mmu、__primary_switch 等;段输出在 .rodata.text(lds:227-233),配对符号 __idmap_text_end 在 lds:113;lds:395 断言整段≤4K(后期 idmap_pg_dir 只映射这一段,见上行)</sup>||[vmlinux.lds.S:109-113](./vmlinux.lds.S)|
|MIN_KIMG_ALIGN|镜像物理加载对齐要求|SZ_2M|[boot.h:18](../include/asm/boot.h)|
|memstart_addr / PHYS_OFFSET|RAM 起始物理地址<sup>注意:不是"内核加载基址"!启动后期从DT/UEFI内存映射得出;7.1.3 中取加载基址用 adrp x0, KERNEL_START(head.S:521),与 RAM 起始无关</sup>||[memory.h:284-286](../include/asm/memory.h)|
|kimage_limit|镜像上限(_end + 64KB 再按 2MB 对齐),仅用于 INIT_IDMAP_DIR_SIZE 的页数估算|ALIGN(_end + SZ_64K, SZ_2M)|[image-vars.h:164](./image-vars.h) (被 [kernel-pgtable.h:61](../include/asm/kernel-pgtable.h) 使用)|

### 符号变迁: 5.9 → 7.1.3

|5.9 符号|7.1.3 状态|替代方案|
|-|-|-|
|_head|❌ 已删除|Image 头就在 _text:第一条指令是 efi_signature_nop(head.S:61,UEFI的MZ头,定义在 [efi-header.S:10](./efi-header.S)),第二条 b primary_entry(head.S:62);魔数 ARM64_IMAGE_MAGIC="ARM\x64"([image.h:6](../include/asm/image.h))|
|__PHYS_OFFSET|❌ 已删除|运行期取物理地址:`adrp x0, KERNEL_START`(head.S:521);链接期职责由 `_text == KIMAGE_VADDR` 断言(lds:413)接管|
|kimage_vaddr|❌ 已删除|`adrp x4, _text`(head.S:233)直接取;运行期随机化位移用 kaslr_offset() 内联(memory.h:291-294)|
|TEXT_OFFSET|❌ 已删除|[Makefile](../Makefile) 中彻底无此定义;Image 头偏移8字段("Image load offset from start of RAM")恒为 0(head.S:63);[booting.rst](../../../Documentation/arch/arm64/booting.rst) 仍以"距2MB对齐基址 text_offset 字节"描述协议|
|el2_setup|重构改名|init_kernel_el(head.S:270)+ finalise_el2(head.S:244 调用)|
|__create_page_tables(约700行汇编)|❌ 已删除|C 代码:__pi_create_init_idmap([pi/map_range.c:91](./pi/map_range.c))+ __pi_early_map_kernel → map_kernel([pi/map_kernel.c](./pi/map_kernel.c))|
|kaslr_early_init(汇编调用,二次进入 __primary_switched)|重构|C 版 kaslr_early_init([pi/kaslr_early.c:41](./pi/kaslr_early.c)),在 early_map_kernel 内一次完成,不再二次进入 __primary_switched|

---

### 计算 kimage_voffset (7.1.3)
<pre>
7.1.3 的写法(5.9 里 "kimage_vaddr - __PHYS_OFFSET" 的代数陷阱不复存在,两个旧符号都删了):

    adrp x4, _text          (head.S:233, 此时 PC=虚拟地址 → x4 = VA(_text))
    sub  x4, x4, x0         (head.S:234, x0 来自 __primary_switch 的 adrp x0, KERNEL_START)
    str_l x4, kimage_voffset (head.S:235)

    而 __primary_switch 里 (head.S:521, 此时 MMU 已开但 PC 仍是物理地址, 走 TTBR0 的 init_idmap 恒等映射):
    adrp x0, KERNEL_START    → x0 = _text 所在4K页基 = 内核镜像实际加载的物理地址 PA(_text)

    所以 kimage_voffset = VA(_text) - PA(_text)
                        = 内核第一条指令的虚拟地址相对其物理地址的偏移量, 运行期才算得出来(KASLR 会随机化 VA)

    更多内容,参考: [pi/map_kernel.c](./pi/map_kernel.c) 函数 early_map_kernel(:241) 和 [head.S](./head.S) 函数 __primary_switch(:508)/__primary_switched(:220)
</pre>

### 图解: kimage_voffset 的诞生 (链接期 vs 运行期)
<pre>
链接期(写死在镜像里的数值):
  _text = KIMAGE_VADDR = MODULES_END = _PAGE_END(VA_BITS_MIN) + SZ_2G
       = 0xffff8000_80000000                (lds:189-192; ASSERT _text==KIMAGE_VADDR: lds:413)
  5.9 的 kimage_vaddr / __PHYS_OFFSET 两个符号已删 —— 只剩一个链接期常量 _text

运行期(开机后才确定, 全部发生在 __pi_early_map_kernel, pi/map_kernel.c:241):
  pa_base      = (u64)&_text                 (map_kernel.c:244, 运行于ID映射, 即物理地址)
  kaslr_offset = pa_base % 2MB | seed高各位   (map_kernel.c:245,281; 低21位由物理放置决定,
                                               否则无法用2MB块描述符建早期映射, 见:269注释)
  va_base      = KIMAGE_VADDR + kaslr_offset (map_kernel.c:287) ← 内核映射的VA起点
  x0 = adrp x0, KERNEL_START 的结果 = PA(_text) (head.S:521, PC=物理地址)
  x4 = adrp x4, _text 的结果       = VA(_text) (head.S:233, PC=虚拟地址)
  kimage_voffset = x4 - x0                      (head.S:234)
     例(KASLR关, 加载于0x80200000): 0xffff800080000000 - 0x80200000 = 0xffff7fff_ffe00000

用途: 内核镜像区 VA↔PA 换算   __kimg_to_phys(x) = x - kimage_voffset   (memory.h:383)
</pre>

> 非常重要! 7.1.3 的 KASLR 语义与 5.9 明显不同:
> ① **纯虚拟 KASLR**:内核不再自我物理搬运 —— 物理位置完全由 bootloader/EFI stub 决定(建议 2MB 对齐;EFI stub 可能仅 4K 对齐任意放置),`kaslr_offset` 的低 21 位吸收物理放置偏差 `pa_base % MIN_KIMG_ALIGN`,高位由 FDT `/chosen/kaslr-seed` 或 RNDR 随机([pi/kaslr_early.c:47-61](./pi/kaslr_early.c),随机范围取 vmalloc 中间半区);
> ② 虚拟随机化位移 = `kaslr_offset() = &_text - KIMAGE_VADDR`(memory.h:291-294),RELA/RELR 重定位由 relocate_kernel(kaslr_offset) 应用([pi/relocate.c:15](./pi/relocate.c))——**包括 `__primary_switch` 里 `ldr x8, =__primary_switched` 的字面量**(head.S:520),所以 br x8 一步就跳进(可能已随机化的)虚拟地址;
> ③ 执行路径不再有 5.9 的"二次进入":primary_entry 建初始ID映射 → __primary_switch 开MMU → early_map_kernel 一次性完成 FDT映射/清BSS/特性覆盖/KASLR/内核VA映射/重定位/TTBR1切换 → 单次进入 __primary_switched → start_kernel。

---

### 图解: 早期启动主流程 (含寄存器与MMU状态, 行号均指 head.S)
<pre>
bootloader/EFI stub 跳转到镜像物理加载地址(建议2MB对齐), x0=FDT物理地址
   MMU=off 或 on(x19记录, record_mmu_state:134; EFI stub 可能开着MMU进内核,
                   setup.c:430 会对此告警检查)
   │
   ▼
_text (head.S:57 .head.text 起, =Image头) ── MZ NOP + b primary_entry (:61-62) ──▶
primary_entry (head.S:85, 位于.idmap.text)
   │  bl record_mmu_state (:86)  → x19 = 进入时MMU是否开启
   │  bl preserve_boot_args (:87)→ x21 = FDT; x0..x3 存入 boot_args[4] (:170-183)
   │  SP ← early_init_stack (:89-91, BSS尾部4KB栈, lds:361)
   │  adrp x0, __pi_init_idmap_pg_dir (:92); bl __pi_create_init_idmap (:94, C代码)
   │     └─ 初始ID映射: VA=PA 覆盖整个镜像 [_stext,_end] (pi/map_range.c:101-106)
   │  按x19做cache维护 (:101-118: MMU=off→失效页表cache行 / MMU=on→清洗.idmap.text到PoC)
   │  bl init_kernel_el (:119, 取代5.9 el2_setup) → w0=BOOT_CPU_MODE_EL1/EL2 → x20
   │  bl __cpu_setup (:129, proc.S:483)
   ▼
__primary_switch (head.S:508, 位于.idmap.text!)   PC=物理地址
   │  TTBR1 ← reserved_pg_dir (:509, 空PGD占位——尚无任何内核VA映射)
   │  TTBR0 ← __pi_init_idmap_pg_dir (:510)
   │  bl __enable_mmu (:511, 函数体:459)  MMU开启, ret
   │     —— PC仍是物理地址, 走TTBR0初始ID映射继续执行!
   │  SP ← early_init_stack (:513-515)
   │  x0=x20(boot status), x1=x21(FDT)
   │  bl __pi_early_map_kernel (:518, C代码 pi/map_kernel.c:241)
   │     ├─ map_fdt: FDT映射进ID映射 (map_kernel.c:200,249)
   │     ├─ 清BSS+init页表区 (map_kernel.c:252)
   │     ├─ init_feature_override: 解析 /chosen 的CPU特性覆盖 (map_kernel.c:256)
   │     ├─ [RANDOMIZE_BASE] kaslr_early_init (kaslr_early.c:41)
   │     │     kaslr-seed(FDT)或RNDR → vmalloc中间半区取随机位移 (kaslr_early.c:59-61)
   │     │     kaslr_offset = PA(_text)%2MB | seed&~(2MB-1) (map_kernel.c:245,281) ★纯虚拟KASLR
   │     ├─ va_base = KIMAGE_VADDR + kaslr_offset (map_kernel.c:287)
   │     └─ map_kernel (map_kernel.c:39): 按段建 VA[va_base..]→PA[加载基址..] 于 init_pg_dir (:85-96)
   │           TTBR1 ← init_pg_dir (:99)
   │           [RELOCATABLE] relocate_kernel(kaslr_offset): 应用RELA/RELR (:103, relocate.c:15)
   │              ★包括本函数下面 ldr x8,=__primary_switched 的字面量!
   │           [两遍模式] 文本段先RW执行重定位, 再重映射为ROX (:101-133)
   │           根表整页复制到 swapper_pg_dir, TTBR1 ← swapper_pg_dir (:136-138)
   │  ldr  x8, =__primary_switched (:520) ← 值已被重定位 = 平移后虚拟地址
   │  adrp x0, KERNEL_START (:521)       ←★ PC=物理地址, PC相对寻址 → x0=PA(_text)
   │  br x8 ─────────────────────┐     PC切换到(可能随机化的)高虚拟地址, 走TTBR1
   ▼                            │
__primary_switched (head.S:220)◄┘   MMU=on, PC=虚拟地址, x0=PA(_text)
   │  init_task + init_cpu_task(:221-222, 内核栈/最终栈帧); VBAR_EL1 ← vectors (:224)
   │  __fdt_pointer = x21 (:231)
   │  adrp x4, _text (:233) x4 = VA(_text)
   │  sub  x4, x4, x0 (:234)
   │  str_l x4, kimage_voffset (:235) ★ kimage_voffset = VA(_text)-PA(_text)
   │  set_cpu_boot_mode_flag (:238); [KASAN] kasan_early_init (:241)
   │  finalise_el2 (:244, 能用VHE则留在EL2)
   ▼
bl start_kernel (head.S:246)
</pre>

### 图解: 地址空间布局 (VA_BITS=48、4K页、无KASAN, 行号指 memory.h/pgtable.h)
<pre>
虚拟地址                                          物理地址
0xffff0000_00000000 ┬ PAGE_OFFSET (:44-45)
                    │  线性映射区 (linear map, paging_init 之后的 map_mem)
0xffff8000_00000000 ┼ MODULES_VADDR = _PAGE_END(VA_BITS_MIN) (:48,66)
                    │  modules + BPF JIT 区 (2GB, MODULES_VSIZE=SZ_2G :49)
0xffff8000_80000000 ┼ MODULES_END = KIMAGE_VADDR (:46-47) = _text(链接期, lds:413断言)
                    │  ╔══════════════╗          ╔══════════════╗
                    │  ║   内核镜像    ║ ←块映射→  ║ 内核镜像      ║ ← 建议2MB对齐
                    │  ║ (+kaslr_offset║          ║ 加载基址起    ║   (MIN_KIMG_ALIGN,
                    │  ║  随机上浮)    ║          ╚══════════════╝    boot.h:18)
0xffff8000_8xxxxxxx ┼ _end (lds:366)
                    │  vmalloc 区 (VMALLOC_START=MODULES_END, pgtable.h:23)
                    │  ★KASLR随机区: [KIMAGE_VADDR+range/4, +range/2] 之间
                    │    (range=(VMALLOC_END-KIMAGE_VADDR)/2, kaslr_early.c:59-61)
..............      ┼  vmemmap 区 (VMEMMAP_START:50, 结束于 -1GB 处 VMEMMAP_END:51)
                    │        ▲
                    └────────┴─ kimage_voffset = VA(_text) - PA(_text)
</pre>

---

分析 [arch/arm64/kernel/head.S](./head.S) 与 [arch/arm64/kernel/pi/](./pi/) 之后,再与 5.9 对照([head.symbol.md (5.9)](../../../../../000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.symbol.md)),就很清晰了:
- ![wechat_2025-11-15_164110_629.png](../../../../../../001.UNIX-DOCS/999.IMGS/wechat_2025-11-15_164110_629.png)(5.9 时代流程图,注意 7.1.3 已无 __PHYS_OFFSET/__create_page_tables)
- ![wechat_2025-11-15_164234_984.png](../../../../../../001.UNIX-DOCS/999.IMGS/wechat_2025-11-15_164234_984.png)(同上)
