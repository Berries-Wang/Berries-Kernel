# Linux-5.9
本内核文件基于 GCC O2 级别优化编译（调试打印值会出现  'optimize out'） , 目前无法全局处理，使能如下方式对单个函数进行 O0 (即不进行优化) 级别的优化。

---

## 注意事项
- 关于Linux内核内存布局<sup>**与参考资料有差异**</sup>： 所以，注意甄别
  + 资料[007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub] 描述的是Linux 5.0 内核，而当前版本为5.9,内核布局已经不一样了,参考:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/arm64/memory.rst](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/Documentation/arm64/memory.rst)

---

## 内核入口
> 传送门:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/init/main.c#start_kernel](../000.LINUX-5.9/000.LINUX-5.9/init/main.c)

---

## 内核启动
- [000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.S.copy)
  - 得参考:
     + [000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.symbol.md](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/head.symbol.md)
	 + [000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/vmlinux.lds.S](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/vmlinux.lds.S.copy)

---

## 设备属性的传递
在ARM64 Linux中，各种设备的相关属性描述可以采用DTS方式或者BIOS方式来呈现。设备树源（Device Tree Source，DTS）最早是由PowerPC等架构使用的扁平化设备树（Flattened Device Tree，FDT）转变过来的，ARM Linux社区自2011年开始全面支持DTS <sup>[奔跑吧Linux内核（第2版） 卷1·基础架构](../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub)#3.3.3　内存大小</sup>

即: 设备信息通过 BIOS/DTS 传递给内核

---


## optimized out 解决方案
### 单个函数优化
```c
// 添加编译优化修饰: __attribute__((optimize("O0"))) 
__attribute__((optimize("O0")))  static __latent_entropy struct task_struct  *copy_process(
					struct pid *pid,
					int trace,
					int node,
					struct kernel_clone_args *args)
```
##### 修改之前效果
![001-optimize-O2.png](./IMGS/001-optimize-O2.png)

##### 修改之后效果
![001-optimize-O0.png](./IMGS/000-optimize-O0.png)

---

### 全局修改
- [kernel hacking: GCC optimization for debug experience (-Og)](https://lwn.net/Articles/753201/)目前尚未研究出补丁应用方法

