#include <stdio.h>

#define _PAGE_OFFSET(va)	(-((1UL) << (va)))

int main(int argc, char **argv)
{
    printf("%#lX \n", ((((((((-((((1UL))) << ((((48))) - 1))))) + (0x08000000))) + (0x08000000)))));
    printf("%#lX \n", (-((1UL) << (48))));
    printf("%#lX \n", (_PAGE_OFFSET(48)));

    printf("\001" "0" "MODULES_VADDR: 0x%lx\n", (((((-((((1UL))) << ((((48))) - 1))))) + (0x08000000))));
    printf("\001" "0" "MODULES_END: 0x%lx\n", ((((((-((((1UL))) << ((((48))) - 1))))) + (0x08000000))) + (0x08000000)));

    
    return 0;
}