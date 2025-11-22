#include <stdio.h>
#include <inttypes.h>

#define ULLONG_MAX	(~0ULL)

int main(int argc, char **argv)
{
    unsigned long addr = -5;
    printf("Address: %" PRIXPTR "\n", addr);   // 小写，适合指针/地址
    printf("Address: %" PRIXPTR "\n", ((1ULL << 65)-1)); 
    printf("Address: %" PRIXPTR "\n", (ULLONG_MAX)); 
    return 0;
}