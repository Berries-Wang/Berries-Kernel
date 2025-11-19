#include <stdio.h>
#include <inttypes.h>

int main(int argc, char **argv)
{
    unsigned long addr = -5;
    printf("Address: %" PRIXPTR "\n", addr);   // 小写，适合指针/地址
    return 0;
}