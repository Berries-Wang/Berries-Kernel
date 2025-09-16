#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  void *initial_break = sbrk(0);
  printf("Initial break: %p\n", initial_break);

  void *ope_brk = (char *)initial_break + 4096;
  void *p = brk(ope_brk); // 直接仅申请4KB内存
  void *new_break = sbrk(0);
  printf("After malloc(4096), break: %p\n", new_break);
  printf("Delta: %ld KB\n", ((char *)new_break - (char *)initial_break)/1024);

  free(p);
  return 0;
}

// wei@Berries-Wang:~/OPEN_SOURCE/Berries-Kernel/003.TEST-SPACE$ gcc 003.sbrk002.c  -o app
// 003.sbrk002.c: In function ‘main’:
// 003.sbrk002.c:12:13: warning: initialization of ‘void *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
//    12 |   void *p = brk(ope_brk); // 直接仅申请4KB内存
//       |             ^~~
// wei@Berries-Wang:~/OPEN_SOURCE/Berries-Kernel/003.TEST-SPACE$ ./app
// Initial break: 0x561b15455000
// After malloc(4096), break: 0x561b15456000
// Delta: 4 KB