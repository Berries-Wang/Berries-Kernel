/* 链接器实验: 定义各种连接属性的符号 (028.编译原理/000.链接器) */
int global_init = 42;      /* .data  已初始化全局(强符号) */
int global_uninit;          /* .bss   未初始化全局(强符号,COMMON) */
__attribute__((weak)) int weak_var = 7;   /* 弱符号 */
static int local_static = 1; /* 局部符号, 不参与链接 */

int my_func(int x)
{
    return x + global_init; /* 引用自身 TU 的全局 → 也要重定位 */
}
