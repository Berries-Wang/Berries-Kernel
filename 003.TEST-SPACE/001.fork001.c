#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    for (int i = 0; i < 2; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            printf("这是子进程\n");
        }
        else
        {
            printf("这是父进程\n");
        }

        printf("_ %d\n", i);
    }

    wait(NULL);
    wait(NULL);
    return 0;
}