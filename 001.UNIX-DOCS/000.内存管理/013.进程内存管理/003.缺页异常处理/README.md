# 缺页异常处理
> 学习:[007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub](../../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub)#4.7　缺页异常处理

在ARM64架构里把异常分成同步异常和异步异常两种。通常异步异常指的是中断，而同步异常指的是异常

## 寄存器摘要
|寄存器名称|功能|备注|
|-|-|-|
|异常综合信息寄存器（Exception Syndrome Register，ESR）|ARMv8架构中有一个与访问失效相关的寄存器——异常综合信息寄存器（Exception Syndrome Register，ESR）|-|
|-|-|-|
|失效地址寄存器（Fault Address Register，FAR）|ARMv8架构还提供了另外一个寄存器——失效地址寄存器（Fault Address Register，FAR）。这个寄存器保存了发生异常的虚拟地址。|-|
|-|-|-|
|非寄存器!只是记录|还是得硬件支持，如 ESR ISS FAR DFSC ...(均在[007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub](../../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub)#4.7　缺页异常处理中)|-|



## 缺页异常
malloc()和mmap()两个用户态接口函数的内核实现时，我们发现它们只建立了进程地址空间，在用户空间里可以看到虚拟内存，但没有建立虚拟内存和物理内存之间的映射关系。当进程访问这些还没有建立映射关系的虚拟内存时，处理器自动触发一个缺页异常（也称为“缺页中断[4]”），Linux内核必须处理此异常。缺页异常是内存管理中重要的一部分，需要考虑很多的细节，包括匿名页面、KSM页面、页面高速缓存页面、写时复制、私有映射和共享映射等

当处理器有异常发生时，处理器会首先跳转到ARM64的异常向量表中。Linux 5.0内核关于异常向量表的描述在arch/arm64/kernel/entry.S汇编文件中

### 缺页异常修复
```c
// <arch/arm64/mm/fault.c>

static int __kprobes do_page_fault(unsigned long addr, unsigned int esr,
                   struct pt_regs *regs)
```