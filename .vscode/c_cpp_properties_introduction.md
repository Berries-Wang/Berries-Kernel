# C/C++ Extension Pack 插件的配置


## 关于defines
|项目|说明|备注|
|-|-|-|
|CONFIG_MEMCG|Memory Resource Controller（内存资源控制器）,是 Cgroups (Control Groups) 机制下的一个子系统，专门用于管理、限制和统计一组进程所使用的内存资源|-|
|-|-|-|
|CONFIG_VM_EVENT_COUNTERS|Linux 内核中的一个配置选项，主要用于启用或禁用内核虚拟内存（VM）事件的计数器统计功能|-|
|-|-|-|
|CONFIG_MMU|决定了内核是否支持和使用 MMU（Memory Management Unit，内存管理单元） 硬件|-|
|-|-|-|
|CONFIG_TRANSPARENT_HUGEPAGE|- 启用或禁用 透明大页|-|
|-|-|-|
|CONFIG_ARCH_ENABLE_THP_MIGRATION|一个架构相关的配置选项，它开启了 “透明巨页（THP）迁移”<sup>迁移:<br/>- NUMA 平衡：将内存页从一个 CPU 节点移动到离进程更近的另一个 CPU 节点，以减少访问延迟。<br/>- 内存整理 (Compaction)：为了凑出连续的大块空间，把散落在各处的小页挪到一起。<br/> - 热插拔 (Hotplug)：准备拔出某个内存条前，必须把上面的数据搬走。</sup> 的支持|-|
|-|-|-|
|CONFIG_HIGHPTE|CONFIG_HIGHPTE 是一个专门针对 32位系统（x86-32、ARM32 等） 的内存管理优化选项；作用：允许将页表（PTE）本身存放到高端内存（High Memory）中|-|
|-|-|-|
|CONFIG_RANDOMIZE_BASE|对应KASLR (Kernel Address Space Layout Randomization)，即内核地址空间布局随机化|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|

---

## linux-gcc-arm64 配置值解读
```txt
/**
 * C/C++ Extension Pack 插件的配置
 *
 * linux-gcc-arm64 怎么理解?
 *
 * linux:
 *        - 告诉 VS Code 如何处理文件路径（例如是用 / 还是 \）、如何查找系统头文件，以及如何处理符号链接（Symlinks）
 * gcc: 语法特征（Compiler Syntax）
 *        - 决定了编辑器如何解析代码中的“非标准”部分
 *        - * GNU 扩展： Linux 内核大量使用了 GCC 特有的扩展，比如 __attribute__((aligned(8)))、内联汇编 asm volatile (...)、以及 __builtin_expect（也就是 likely() 和 unlikely() 宏）。
 *        - 预定义宏： 选 gcc 会让 IntelliSense 默认认为定义了 __GNUC__ 等宏
 * arm64: 目标架构（Target Architecture）,代表硬件底层的物理属性
 *        - 字长（Data Width）： 告诉编辑器指针（pointer）占用 8 个字节（64位）
 *        - 架构宏： 自动定义 __aarch64__ 或 __arm64__ 宏。
 *        - 条件编译： 在内核代码中，只有这个字段对上了，__aarch64__, __GNUC__, __LP64__ 块里的代码才会被“激活”（颜色加亮），否则代码会变灰，无法跳转
 */
```

---

### c_cpp_properties.json完整配置
```json
{
  "configurations": [
    {
      "name": "Linux-arm64",
      "defines": ["__KERNEL__", "CONFIG_ARM64", "CONFIG_MEMCG"],
      "intelliSenseMode": "linux-gcc-arm64", // 模拟在 Linux 下使用 GCC 开发/分析 64 位 ARM 程序
      "compilerPath": "/home/wei/apps/001.Arm-ToolChain/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc",
      "includePath": [
        "${workspaceFolder}/**/**/arch/arm64/include",
        "${workspaceFolder}/**/**/arch/arm64/include/generated",
        "${workspaceFolder}/**/**/include",
        "${workspaceFolder}/**/**/include/uapi",
        "${workspaceFolder}/**/**/arch/arm64/include/uapi",
        "${workspaceFolder}/**/**/kernel/locking",
        "${workspaceFolder}/**/**/include/linux"
      ]
    },
    {
      "name": "Mac-arm64",
      "defines": ["__KERNEL__", "CONFIG_ARM64", "CONFIG_MEMCG"],
      "intelliSenseMode": "macos-gcc-arm64", // 在 macOS 下使用 GCC 开发或分析 64 位 ARM 程序
      "compilerPath": "/opt/softwares/arm64-arm-toolchain/arm-gnu-toolchain-14.3.rel1-darwin-arm64-aarch64-none-elf/bin/aarch64-none-elf-gcc",
      "includePath": [
        "${workspaceFolder}/**/**/arch/arm64/include",
        "${workspaceFolder}/**/**/arch/arm64/include/generated",
        "${workspaceFolder}/**/**/include",
        "${workspaceFolder}/**/**/include/uapi",
        "${workspaceFolder}/**/**/arch/arm64/include/uapi",
        "${workspaceFolder}/**/**//kernel/locking",
        "${workspaceFolder}/**/**/include/linux"
      ]
    }
  ],
  "version": 4
}
```
