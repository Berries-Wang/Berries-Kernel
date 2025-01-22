#include <stdio.h>
#include <sys/time.h>

int main() {
  struct timeval tv;
  if (gettimeofday(&tv, NULL) == 0) {
    printf("Seconds: %ld\n", tv.tv_sec);
    printf("Microseconds: %ld\n", tv.tv_usec);
  } else {
    perror("gettimeofday");
  }
  return 0;
}