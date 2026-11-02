/* 链接器实验: 引用另一个 TU 的符号 (028.编译原理/000.链接器) */
extern int global_uninit;
extern int my_func(int);

int main(void)
{
    return my_func(global_uninit);
}
