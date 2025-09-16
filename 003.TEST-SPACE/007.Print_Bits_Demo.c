#include <stdio.h>

#include "./006.Print_Bits.h"

#define RWSEM_FLAG_HANDOFF (1UL << 2)

int main(int argc, char **argv)
{
    long a = 1;
    long new = a;
    new &= ~RWSEM_FLAG_HANDOFF;

    print_bits_long(new);
    print_bits_long(~RWSEM_FLAG_HANDOFF);
    printf("new = %ld\n", new);

    return 0;
}