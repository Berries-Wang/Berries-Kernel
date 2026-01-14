# mmap系统调用
![mmap立即映射](../../999.IMGS/mmap_20251211080200_89.jpg)

## mmap系统调用定义
```c
//  000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/sys.c

SYSCALL_DEFINE6(mmap, unsigned long, addr, unsigned long, len,
		unsigned long, prot, unsigned long, flags,
		unsigned long, fd, unsigned long, off)
{
	if (offset_in_page(off) != 0)
		return -EINVAL;

	return ksys_mmap_pgoff(addr, len, prot, flags, fd, off >> PAGE_SHIFT);
}
```


## 映射类型
> [007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#4.6　mmap

|映射类型|说明|
|-|-|
|匿名映射|没有映射对应的相关文件，匿名映射的内存区域的内容会初始化为0。|
|-|-|
|文件映射|映射和实际文件相关联，通常把文件内容映射到进程地址空间，这样应用程序就可以像操作进程地址空间一样读写文件|
|-|-|

## 使用示例
### 1. 基于映射的文件复制
- 参考代码: [003.TEST-SPACE/012.File_Copy_Use_mmap.c](../../../../003.TEST-SPACE/012.File_Copy_Use_mmap.c)

## 参考资料
- [007.BOOKs/Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub](../../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub)
- [文件系统II](../../../../001.UNIX-DOCS/033.文件系统/000.REFS/001.文件系统-2.pdf)#P24