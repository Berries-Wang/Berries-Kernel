# 文件系统(File System.FS)
先学习  <br/>[17-文件系统：基于inode的文件系统 [中山大学 操作系统原理]](./000.REFS/000.文件系统-1.pdf) <br/> [18-文件系统：使用文件系统 [中山大学 操作系统原理]](./000.REFS/001.文件系统-2.pdf)  <br/> [深入理解Linux内核:虚拟文件系统](../../007.BOOKs/Professional-Linux-Kernel-Architecture.epub)  <br/> [文件系统崩溃一致性](../000.内存管理/998.REFS/000.中山大学-操作系统/11-0508-fs-crash-1.pdf)  <br/> [文件系统崩溃一致性II](../000.内存管理/998.REFS/000.中山大学-操作系统/12-0512-fs-crash-2.pdf)

---

## 简介
Linux 文件系统是操作系统管理和存储文件的一套机制。但 Linux 的精妙之处在于，它不仅用文件系统来存照片和文档，还用它来**“管理内核状态”**。

在 Linux 中，有一句名言：“万物皆文件”（Everything is a file）。
- 常规文件系统（如 Ext4, XFS）
- 虚拟文件系统（如 procfs, sysfs, cgroupfs）：这些文件并不存在于硬盘上，而是内核在内存中虚拟出来的。当你读取这些“文件”时，你其实是在读取内核的状态；当你修改这些“文件”时，你其实是在直接调整内核的参数。


---

## 查看文件的文件系统类型
```bash
# 1. df -T <文件路径>
wei@Berries:~/VirtualBox_VMs$ df -T /sys/fs/cgroup/
Filesystem     Type    1K-blocks  Used Available Use% Mounted on
cgroup2        cgroup2         0     0         0    - /sys/fs/cgroup


# 2. state -f <文件路径>
wei@Berries:~/VirtualBox_VMs$ stat -f /sys/fs/cgroup/
  File: "/sys/fs/cgroup/"
    ID: 218518cfb40c0f19 Namelen: 255     Type: cgroup2fs
Block size: 4096       Fundamental block size: 4096
Blocks: Total: 0          Free: 0          Available: 0
Inodes: Total: 0          Free: 0

```

---

## 参考代码
|名称|类型|地址|说明|
|-|-|-|-|
|cgroup|vfs(虚拟文件系统)|000.LINUX-5.9/kernel/cgroup/|-|
|-|-|-|-|
|Ext4|-|-|- 在学习资料中有体现|
|-|-|-|-|
|-|-|-|-|




## 参考资料
- [17-文件系统：基于inode的文件系统 [中山大学 操作系统原理]](./000.REFS/000.文件系统-1.pdf)
- [18-文件系统：使用文件系统 [中山大学 操作系统原理]](./000.REFS/001.文件系统-2.pdf)
- [深入理解Linux内核:虚拟文件系统](../../007.BOOKs/Professional-Linux-Kernel-Architecture.epub)
- [文件系统崩溃一致性](../000.内存管理/998.REFS/000.中山大学-操作系统/11-0508-fs-crash-1.pdf) 
- [文件系统崩溃一致性II](../000.内存管理/998.REFS/000.中山大学-操作系统/12-0512-fs-crash-2.pdf)