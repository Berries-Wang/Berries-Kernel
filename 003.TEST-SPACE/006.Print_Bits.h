#include <stdio.h>
#include <limits.h> 
void print_bits(int num)
{
    // 计算int类型的位数（通常是32位）
    int total_bits = sizeof(int) * CHAR_BIT;

    // 创建一个掩码，初始时最高位为1，其余为0
    unsigned int mask = 1 << (total_bits - 1);

    printf("二进制表示: ");

    for (int i = 0; i < total_bits; i++)
    {
        // 检查当前位是否为1
        if (num & mask)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }

        // 每8位加一个空格，便于阅读
        if ((i + 1) % 8 == 0)
        {
            printf(" ");
        }

        // 右移掩码
        mask >>= 1;
    }
    printf("\n");
}