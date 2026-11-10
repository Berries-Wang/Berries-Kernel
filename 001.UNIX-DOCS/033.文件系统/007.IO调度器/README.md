# IO调度器
内核中采用的各种用于调度和重排I/O操作的算法，称之为I/O调度器。通常，I/O调度器也称作电梯(elevator<sup>/include/linux/elevator.h</sup>)

> 详细笔记已整理到 [052.INPUT_OUTPUT/008.IO调度器-电梯算法与死线](../../052.INPUT_OUTPUT/008.IO调度器-电梯算法与死线.md)(MOS5 §10.5.4 原文 + Linux 7.1.3 `mq-deadline`/`kyber`/`bfq` 源码对照)。

## 参考资料
- [007.BOOKs/Professional-Linux-Kernel-Architecture.epub#6.5.8 I/O调度](../../../007.BOOKs/Professional-Linux-Kernel-Architecture.epub)