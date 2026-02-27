# mmap系统调用
```txt
 调用路径: 
  mmap() [misc/sys/mman.h](glibc)
                |
                V 
  ksys_mmap_pgoff() [arch/arm64/kernel/sys.c](Kernel): 定义系统调用
```