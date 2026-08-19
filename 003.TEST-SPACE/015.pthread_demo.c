#include <pthread.h>
#include <stdio.h>

 int balance = 0;

void *my_thread(void *arg)
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < 20000; i++)
    {
        balance++;
    }

    printf("%lu：Balance is %d \n", (unsigned long)tid, balance);
    return NULL;
}

int main(int argc, char **argv)
{

    pthread_t p1, p2, p3;
    pthread_create(&p1, NULL, my_thread, (void *)"A");
    pthread_create(&p2, NULL, my_thread, (void *)"B");
    pthread_create(&p3, NULL, my_thread, (void *)"C");

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    printf("Final Balance is %d \n", balance);
    return 0;
}

// ------------------ 汇编代码如下 -------------------
//	.arch armv8-a
//	.file	"015.pthread_demo.c"
//	.text
//	.global	balance
//	.bss
//	.align	2
//	.type	balance, %object
//	.size	balance, 4
//balance:
//	.zero	4
//	.section	.rodata
//	.align	3
//.LC0:
//	.string	"%lu\357\274\232Balance is %d \n"
//	.text
//	.align	2
//	.global	my_thread
//	.type	my_thread, %function
//my_thread:
//.LFB0:
//	.cfi_startproc
//	stp	x29, x30, [sp, -48]!
//	.cfi_def_cfa_offset 48
//	.cfi_offset 29, -48
//	.cfi_offset 30, -40
//	mov	x29, sp
//	str	x0, [sp, 24]
//	bl	pthread_self
//	str	x0, [sp, 32]
//	str	wzr, [sp, 44]
//	b	.L2
//.L3:
//	adrp	x0, balance           // 计算 balance 地址
//	add	x0, x0, :lo12:balance // 拼出完整地址
//	ldr	w0, [x0]              // 读 balance 到 w0
//	add	w1, w0, 1             // w1 = balance + 1
//	adrp	x0, balance           // 重新计算地址
//	add	x0, x0, :lo12:balance // 拼出完整地址
//	str	w1, [x0]              // 写回 balance
//	ldr	w0, [sp, 44]
//	add	w0, w0, 1
//	str	w0, [sp, 44]
//.L2:
//	ldr	w1, [sp, 44]
//	mov	w0, 19999
//	cmp	w1, w0
//	ble	.L3
//	adrp	x0, balance
//	add	x0, x0, :lo12:balance
//	ldr	w0, [x0]
//	mov	w2, w0
//	ldr	x1, [sp, 32]
//	adrp	x0, .LC0
//	add	x0, x0, :lo12:.LC0
//	bl	printf
//	mov	x0, 0
//	ldp	x29, x30, [sp], 48
//	.cfi_restore 30
//	.cfi_restore 29
//	.cfi_def_cfa_offset 0
//	ret
//	.cfi_endproc
//.LFE0:
//	.size	my_thread, .-my_thread
//	.section	.rodata
//	.align	3
//.LC1:
//	.string	"A"
//	.align	3
//.LC2:
//	.string	"B"
//	.align	3
//.LC3:
//	.string	"C"
//	.align	3
//.LC4:
//	.string	"Final Balance is %d \n"
//	.text
//	.align	2
//	.global	main
//	.type	main, %function
//main:
//.LFB1:
//	.cfi_startproc
//	stp	x29, x30, [sp, -64]!
//	.cfi_def_cfa_offset 64
//	.cfi_offset 29, -64
//	.cfi_offset 30, -56
//	mov	x29, sp
//	str	w0, [sp, 28]
//	str	x1, [sp, 16]
//	add	x4, sp, 56
//	adrp	x0, .LC1
//	add	x3, x0, :lo12:.LC1
//	adrp	x0, my_thread
//	add	x2, x0, :lo12:my_thread
//	mov	x1, 0
//	mov	x0, x4
//	bl	pthread_create
//	add	x4, sp, 48
//	adrp	x0, .LC2
//	add	x3, x0, :lo12:.LC2
//	adrp	x0, my_thread
//	add	x2, x0, :lo12:my_thread
//	mov	x1, 0
//	mov	x0, x4
//	bl	pthread_create
//	add	x4, sp, 40
//	adrp	x0, .LC3
//	add	x3, x0, :lo12:.LC3
//	adrp	x0, my_thread
//	add	x2, x0, :lo12:my_thread
//	mov	x1, 0
//	mov	x0, x4
//	bl	pthread_create
//	ldr	x0, [sp, 56]
//	mov	x1, 0
//	bl	pthread_join
//	ldr	x0, [sp, 48]
//	mov	x1, 0
//	bl	pthread_join
//	ldr	x0, [sp, 40]
//	mov	x1, 0
//	bl	pthread_join
//	adrp	x0, balance
//	add	x0, x0, :lo12:balance
//	ldr	w0, [x0]
//	mov	w1, w0
//	adrp	x0, .LC4
//	add	x0, x0, :lo12:.LC4
//	bl	printf
//	mov	w0, 0
//	ldp	x29, x30, [sp], 64
//	.cfi_restore 30
//	.cfi_restore 29
//	.cfi_def_cfa_offset 0
//	ret
//	.cfi_endproc
//.LFE1:
//	.size	main, .-main
//	.ident	"GCC: (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 14.3.1 20250623"
//	.section	.note.GNU-stack,"",@progbits
//
// ============================================================
// 学习笔记：balance++ 的汇编与内存子系统（TLB / L1 / L2 / L3 / DRAM）
// ============================================================
//
// 【一】汇编 <-> C 代码对应关系
//   volatile int balance = 0;
//   for (int i = 0; i < 20000; i++) { balance++; }
//   .L3 循环体内：
//     adrp x0, balance
//     add  x0, x0, :lo12:balance
//     ldr  w0, [x0]              // 读全局 balance
//     add  w1, w0, 1             // balance + 1
//     adrp x0, balance
//     add  x0, x0, :lo12:balance
//     str  w1, [x0]              // 写回全局 balance   <- 这就是 balance++
//     ldr  w0, [sp, 44]          // 读局部变量 i
//     add  w0, w0, 1             // i + 1
//     str  w0, [sp, 44]          // 写回 i             <- 这是 i++，不是 balance++
//   .L2 循环判断：
//     ldr  w1, [sp, 44]          // 读 i
//     mov  w0, 19999
//     cmp  w1, w0
//     ble  .L3                   // i <= 19999 则继续循环
//   判断依据：
//     - balance 是全局变量(.bss 段)，用 adrp 取地址，地址要经页表翻译；
//       而 [sp,44] 是函数栈帧内的局部槽位，只可能是局部变量 i。
//   adrp 只能得到 4KB 页基址，需要 :lo12: 补低 12 位偏移才拼出完整地址，
//   所以每次访问 balance 都要先算一次地址（两次 adrp/add 的原因）。
//
// 【二】这几条指令里，真正"碰内存"的只有 ldr 和 str；
//   adrp/add 只是 ALU 整数运算，算出虚拟地址(VA)，不走 TLB/缓存。
//
// 【三】ldr（读路径，向下"沉"）
//   VA ->(TLB)-> PA -> L1D -> L2 -> L3 -> DRAM
//   - TLB 是"VA->PA"翻译缓存：命中直接得 PA；
//     未命中则 MMU 硬件遍历页表(ARM64 3~4 级)，结果填回 TLB；
//     本循环 balance 在同一 4KB 页内，首次迭代后 TLB 常驻命中。
//   - L1D(核内私有 32~64KB，命中 ~2~4 周期，多为 VIPT) ->
//     L2(统一缓存 128KB~2MB，~10 周期) ->
//     L3(SoC 级共享，几 MB，~30~50 周期) ->
//     DRAM(~100+ 周期)。
//   - 读按缓存行(cache line，ARM64 = 64B)拉取：冷 miss 时一次
//     把整行从内存搬到 L1D，沿途各级各留一份副本。
//
// 【四】str（写路径，停在最浅处）
//   - 写策略 = write-allocate + write-back：
//     写命中：改 L1D 对应行并打脏位(dirty)，内存仍是旧值；
//     写未命中：先整行读入(RFO, read-for-ownership)再改，同样只标脏。
//   - 写指令先进 store buffer，再提交到 L1D。
//   - 脏行真正回写 DRAM 只在三种情况：
//       a. 该行被替换/驱逐(eviction)，L1D->L2->L3->DRAM 逐级写回；
//       b. 显式缓存维护指令(dc cvau / dc cvac 等)；
//       c. 整体刷新(上下文切换/掉电等)。
//   => 写回是"懒"的：循环结束那一刻 balance 可能还躺在 L1D 脏行里，
//      物理内存的值早已过时。
//
// 【五】多核一致性(MESI/MOESI)
//   L1D 每核私有，各核可有同一地址的副本。一致性协议以 L2/L3 为
//   point of coherence：A 核写入后，B/C 核副本被 invalidate，
//   下次读取 miss 再拉新值。
//   => 这正是本 demo 演示的竞态：ldr 与 str 是两条独立指令，
//      非原子读-改-写，三个线程互相覆盖，最终 balance 到不了 60000。
//
// 【六】volatile 的作用
//   volatile 强制每次 balance++ 都"读内存->加一->写内存"，
//   不许把值长期留在寄存器复用。去掉 volatile，编译器可能优化成
//   "算一次写一次"，竞态被掩盖。故该经典例子必须加 volatile。
//
// 【七】TLB / L1/L2/L3 是硬件操作吗？
//   - 运行时的查找、命中/未命中、替换、回写：100% 硬件，对软件透明。
//   - TLB：硬件查表，但页表由 OS 建立，映射变化时 OS 用 tlbi 失效表项。
//   - 缓存策略：能否缓存、write-back/write-through 由页表属性 + MAIR/TCR
//     寄存器决定，是 OS 建映射时写好的，硬件照执行。
//   - 少数场景软件需显式碰缓存：
//       - DMA 与 CPU 共享数据   -> dc civac(invalidate)；
//       - 自修改代码 / JIT      -> dc cvau + ic ivau；
//       - 多核一致性边界        -> dmb / dsb 屏障。
//
// 【八】一张图（读下沉，写就地）
//   adrp/add  ->  纯 ALU，算出 VA，不走缓存/TLB
//                       |
//   ldr w0,[x0]  ->  VA -> TLB命中 -> PA
//                        |
//                  L1D -> L2 -> L3 -> DRAM   （按 64B 整行填充）
//                       |
//   str w1,[x0]  ->  写入 L1D 对应行，打脏位
//                       |
//            [内存仍为旧值] <- 直到该行被驱逐/显式clean才回写 DRAM
//   逐行解读（竖线 | = 上一步结果传给下一步）：
//     1. adrp/add -> 纯 ALU 拼出 VA，尚未访存，不走缓存/TLB
//     2. ldr      -> 拿 VA 查 TLB（命中）得 PA，交给缓存层级
//     3. 缓存漏斗 -> 按 L1D->L2->L3->DRAM 逐级回退找数据，
//                    哪级有数据哪级停；miss 时按 64B 整行填充，沿途各级各留一份
//     4. str      -> 写 L1D 对应行并打脏位，写路径到此为止（写就地）
//     5. 延迟回写 -> 此刻 DRAM 仍是旧值；脏行在"被驱逐"或"显式 clean"
//                    时才真正回写物理内存
//   读下沉：ldr 一路向下找数据，深度决定延迟（L1D 命中最快，DRAM 最慢）
//   写就地：str 只改最浅的 L1D 并打脏，欠下回写账，驱逐/clean 时才还
//   稳态(第 2~20000 次)：ldr -> TLB命中 -> L1D命中；str -> L1D命中打脏。
//                        全程只碰 L1D / TLB。
//   冷启动(第一次)：TLB miss -> 页表遍历 -> L1D/L2/L3 miss -> DRAM 读整行
//                   -> 逐级填充回 L1D。
//   一句话：读要向下找到数据(深度决定延迟)，写只在最浅处落笔(快但欠账)。