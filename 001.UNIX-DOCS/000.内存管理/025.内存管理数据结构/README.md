# 内存管理数据结构

```c
// 使用一个全局指针数组来统一管理所有的 NUMA 节点
// arch/arm64/include/asm/mmzone.h

extern struct pglist_data *node_data[];
```