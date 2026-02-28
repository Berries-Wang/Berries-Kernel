# Linux Kernel

## 链接文件基础知识
- 任何一种可执行程序（不论是ELF还是EXE）都是由代码（.text）段、数据（.data）段、未初始化数据（.bss）段等段（section）组成的。 <sup>[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#2.6.1　链接文件基础知识</sup>

---

## 重要，但是容易被忽略的点
|项目|传送门|备注|
|-|-|-|
|- 函数的调用_返回与栈|- [001.函数的调用_返回与栈.md](../001.UNIX-DOCS/037.内核基础/001.程序是怎么运行的/001.函数的调用_返回与栈.md)|- 栈上内存的分配与释放 <br/> - 运行时栈结构  <br/> - 各种寄存器|
|-|-|-|
|-|-|-|
|-|-|-|


## 注意事项
- 关于Linux内核内存布局<sup>**与参考资料有差异**</sup>：
  + 资料[007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub] 描述的是Linux 5.0 内核，而当前版本为5.9,内核布局已经不一样了,参考:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/arm64/memory.rst](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/arm64/memory.rst)

---

## 内核启动流程
> [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图2.18　内核启动流程
   - 关于Linux内核内存布局<sup>**与参考资料有差异**</sup>： 所以，注意甄别
       + 资料[007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub] 描述的是Linux 5.0 内核，而当前版本为5.9,内核布局已经不一样了,参考:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/arm64/memory.rst](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/arm64/memory.rst)
