#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
int main(int argc, char **argv)
{
    int input, output;
    size_t filesize;
    void *source, *target;
    // 打开文件
    input = open(argv[1], O_RDONLY);
    /**
     * 以读写模式打开/创建目标文件（argv[2]）  O_CREAT: 不存在则创建；O_TRUNC: 存在则清空；0666: 文件权限
     */
    output = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    /**
     * 获取源文件的大小：将文件指针移动到末尾，返回的偏移量即为字节数
     */
    filesize = lseek(input, 0, SEEK_END);
    printf("源文件大小: %u\n", filesize);
    /**
     * 关键步骤：扩展目标文件的大小: 内存映射要求目标文件必须有实际大小，否则写入时会触发 SIGBUS 错误
     */
    if (ftruncate(output, filesize) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    // 映射源文件
    if ((source = mmap(0, filesize, PROT_READ, MAP_SHARED, input, 0)) == (void *)-1)
    {
        fprintf(stderr, "Error mapping input file: %s\n", argv[1]), exit(1);
    }

    // 映射目标文件
    if ((target = mmap(0, filesize, PROT_WRITE, MAP_SHARED, output, 0)) == (void *)-1)
    {
        fprintf(stderr, "Error mapping ouput file: %s\n", argv[2]), exit(1);
    }
    /**
     * 像操作普通内存一样拷贝文件内容: 这一步之后，数据实际上已经从源文件映射区“流动”到了目标文件映射区
     */
    memcpy(target, source, filesize);
    /**
     * man mmap:
     * The  munmap() system call deletes the mappings for the specified address range, and causes further references to addresses within the range to generate invalid memory references.  The region is also
     *  automatically unmapped when the process is terminated.  On the other hand, closing the file descriptor does not unmap the region.
     *
     * munmap() 系统调用用于删除指定地址范围内的映射，并导致随后对该范围内地址的访问产生无效的内存引用（Invalid Memory References）。
     * 当进程终止时，该区域也会被自动取消映射。另一方面，关闭文件描述符（File Descriptor）并不会取消该区域的映射
     *
     * 即: 内存映射（Memory Mapping）一旦建立，它的生命周期就不再依赖于最初创建它时所使用的那个文件描述符了。 它只受 munmap() 或进程退出的控制
     */
    munmap(source, filesize);
    munmap(target, filesize);

    // 关闭文件描述符
    close(input);
    close(output);
    return 0;
}
