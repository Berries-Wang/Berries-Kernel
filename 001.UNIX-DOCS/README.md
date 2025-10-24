# Linux Kernel
- ARM64处理器有两个页表基地址寄存器，一个是TTBR0，另外一个TTBR1

---

## 链接文件基础知识
- 任何一种可执行程序（不论是ELF还是EXE）都是由代码（.text）段、数据（.data）段、未初始化数据（.bss）段等段（section）组成的。 <sup>[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#2.6.1　链接文件基础知识</sup>

---

## 内核启动流程
> [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图2.18　内核启动流程