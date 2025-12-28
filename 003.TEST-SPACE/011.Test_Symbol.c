#include <stdio.h>

int main(int argc, char **argv)
{
    /**
     * ^ 是按位与操作，不是幂运算 ， 要使用幂运算，使用 pow(x, y)(<math.h>)
     */
    printf(" 8 ^ 2 = %d \n", (8 ^ 2));

    return 0;
}