# Heterogeneous Memory Management (HMM)


## devmap managed pages
在 Linux 内核中，devmap managed pages 是一种特殊的物理内存页。它们并不是由内核在系统启动时自动发现并管理的“常规内存”（如你的 16GB RAM），而是由设备驱动程序主动“塞”进内核内存管理系统的。简单来说，它们是拥有 struct page 结构的设备地址空间。

## 为什么需要这个?

## 参考资料
- [异构内存管理 (HMM)](https://docs.kernel.org/6.0/translations/zh_CN/mm/hmm.html)
   + [异构内存管理 (HMM)](./999.REFS/Screenshot%202026-01-24%20at%2015-20-01%20异构内存管理%20(HMM)%20—%20The%20Linux%20Kernel%20documentation.png)