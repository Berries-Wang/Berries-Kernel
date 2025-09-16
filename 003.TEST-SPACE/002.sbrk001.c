#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  void *initial_break = sbrk(0);
  printf("Initial break: %p\n", initial_break);

  /**
   *  glibc 的 malloc 不会为每次小分配调用 sbrk
   * 内存池（Heap Arena）机制：
   * glibc 的 malloc 不会每次分配小块内存（如 1024 字节）都直接调用 sbrk，而是会先向操作系统申请一块较大的内存（称为 heap arena），然后从中分割出用户需要的大小。
   * 
   * 预分配策略：
   * 即使调用 malloc(1024)，glibc 可能会一次性通过 sbrk 扩展堆（例如 4 KB 或更大），以减少频繁系统调用的开销。
   */
  void *p = malloc(1024); // glibc 可能触发 sbrk
  void *new_break = sbrk(0);
  printf("After malloc(1024), break: %p\n", new_break);
  printf("Delta: %ld KB\n", ((char *)new_break - (char *)initial_break)/1024);

  free(p);
  return 0;
}

// wei@Berries-Wang:~/OPEN_SOURCE/Berries-Kernel/003.TEST-SPACE$ gcc 002.sbrk001.c  -o app
// wei@Berries-Wang:~/OPEN_SOURCE/Berries-Kernel/003.TEST-SPACE$ ./app 
// Initial break: 0x55cdc50ff000
// After malloc(1024), break: 0x55cdc5120000
// Delta: 132 KB