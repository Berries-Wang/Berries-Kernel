/*****************************************************************************
 * code_io_uring_server.c — 基于 io_uring 的 TCP 聊天室服务器
 *
 * 功能：
 *   多个客户端连接服务器，任一客户端发送消息，服务器将消息广播给所有
 *   其他在线客户端（类似于简易聊天室）。
 *
 * 编译与运行：
 *   gcc code_io_uring_server.c -luring -o server
 *   ./server
 *
 * 客户端配合：
 *   gcc code_unix_client.c -pthread -o client
 *   ./client a  (启动客户端 a)
 *   ./client b  (启动客户端 b)
 *   ./client c  (启动客户端 c)
 *
 * 技术要点：
 *   1. io_uring 是 Linux 5.1+ 引入的异步 I/O 框架，通过两个 ring buffer
 *      (SQ: Submission Queue / CQ: Completion Queue) 实现用户态与内核
 *      之间的高性能异步通信。
 *   2. liburing 是 io_uring 的用户态封装库，提供了更友好的 API。
 *   3. 本程序使用 io_uring 的异步 accept 和 read 操作，避免了传统
 *      epoll/select 的 "就绪通知 + 同步读写" 模式，改为 "提交请求 →
 *      内核完成后通知" 的异步模式。
 *
 * 架构说明：
 *   - 使用一个 io_uring 实例管理所有监听套接字和已连接套接字的操作。
 *   - 通过 conn_info 结构体区分请求类型（accept / read），在完成事件
 *     回调中根据类型做不同处理。
 *   - 每次 accept 或 read 完成后，立即重新提交下一次请求，形成持续
 *     的异步链。
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <liburing.h>

#define MAXLINE 4096      /* 单条消息最大长度 */
#define PROGRAM_PORT 9999 /* 服务器监听端口 */
#define MAXCLIENTS 512    /* 最大同时在线客户端数 */
#define ENTRIES_LEN (MAXCLIENTS + 1)  /* io_uring 队列深度，+1 是为 listenfd 预留 */

/**
 * conn_info — 每个 io_uring 请求的附带上下文
 *
 * 通过 io_uring_sqe_set_data() 绑定到提交的请求，在完成事件(CQE)中
 * 通过 io_uring_cqe_get_data() 取回，用于判断请求类型并携带数据缓冲区。
 *
 * @fd:   套接字描述符（listenfd 或已连接 socket）
 * @type: 请求类型，0=accept（监听套接字）, 1=read（已连接套接字）
 * @buf:  数据缓冲区指针，type=1 时有效，用于存放读取到的数据
 */
struct conn_info {
    int fd;
    int type; /* 0: listenfd, 1: connected fd */
    char *buf;
};

/* 客户端连接池 — 记录所有已连接的客户端 fd，用于广播 */
static int client_fds[MAXCLIENTS];
static int client_count = 0;

/**
 * add_client — 将新客户端 fd 加入连接池
 */
static void add_client(int fd) {
    if (client_count < MAXCLIENTS) {
        client_fds[client_count++] = fd;
    }
}

/**
 * remove_client — 将断开连接的客户端 fd 从连接池移除
 *
 * 采用"末尾元素覆盖待删除元素"的方式，保持数组紧凑。
 */
static void remove_client(int fd) {
    for (int i = 0; i < client_count; i++) {
        if (client_fds[i] == fd) {
            client_fds[i] = client_fds[--client_count];
            break;
        }
    }
}

int main(int argc, char **argv) {
    struct io_uring ring;   /* io_uring 实例 */
    struct sockaddr_in servaddr;
    int ret;

    /*------------------------------------------------------------------------
     * 第一步：初始化 io_uring
     *
     * io_uring_queue_init(entries, ring, flags)
     *   - entries: 队列深度（SQ 和 CQ 的条目数），决定内核可同时处理的
     *              未完成请求数量
     *   - ring:    输出的 io_uring 实例
     *   - flags:   0 表示默认行为（IORING_SETUP_IOPOLL 等高级选项见内核文档）
     *   - 返回:    0 成功，负数表示错误码（-ENOMEM 等）
     *------------------------------------------------------------------------*/
    ret = io_uring_queue_init(ENTRIES_LEN, &ring, 0);
    if (ret < 0) {
        printf("io_uring_queue_init failed: %d\n", ret);
        return ret;
    }

    /*------------------------------------------------------------------------
     * 第二步：创建并配置监听套接字
     *------------------------------------------------------------------------*/
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PROGRAM_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 128);

    /*------------------------------------------------------------------------
     * 第三步：提交第一个异步 accept 请求
     *
     * io_uring 的工作流程：
     *   1. 从 SQ（Submission Queue）获取一个空闲的 SQE 条目
     *   2. 用 io_uring_prep_* 系列函数填充 SQE（指定操作类型、fd、缓冲区等）
     *   3. 用 io_uring_sqe_set_data() 绑定自定义上下文（conn_info）
     *   4. 调用 io_uring_submit() 将批量的 SQE 提交给内核
     *   5. 内核异步处理完成后，将结果写入 CQ（Completion Queue）
     *   6. 用户态通过 io_uring_wait_cqe() 等待并取出 CQE
     *   7. 处理完成后调用 io_uring_cqe_seen() 标记 CQE 已消费
     *
     * 异步 accept 的要点：
     *   - 与传统的 accept() 不同，这里不会阻塞等待连接到来
     *   - 提交请求后立即返回，内核在连接到达时自动完成 accept
     *   - accept 完成后，新连接的 fd 存放在 cqe->res 中
     *   - 需要为 accept 分配 sockaddr_in 和 socklen_t 的堆内存，
     *     内核会填充这些值，必须保证在 accept 完成前内存有效
     *------------------------------------------------------------------------*/
    struct sockaddr_in *cliaddr = malloc(sizeof(struct sockaddr_in));
    socklen_t *clilen = malloc(sizeof(socklen_t));
    *clilen = sizeof(struct sockaddr_in);

    struct conn_info *listen_info = malloc(sizeof(struct conn_info));
    listen_info->fd = listenfd;
    listen_info->type = 0;  /* type=0 表示这是一个 accept 请求 */
    listen_info->buf = NULL;

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, listenfd, (struct sockaddr *)cliaddr, clilen, 0);
    io_uring_sqe_set_data(sqe, listen_info);
    io_uring_submit(&ring);

    printf("io_uring chat server started on port %d\n", PROGRAM_PORT);

    /*------------------------------------------------------------------------
     * 第四步：主事件循环
     *
     * 每次循环：
     *   1. io_uring_wait_cqe() — 阻塞等待内核完成一个异步操作
     *   2. 根据 conn_info->type 判断是 accept 完成还是 read 完成
     *   3. 处理完成后，立即重新提交下一次请求（accept 或 read 的异步链）
     *   4. io_uring_cqe_seen() — 标记 CQE 已消费，内核可回收该条目
     *------------------------------------------------------------------------*/
    for (;;) {
        struct io_uring_cqe *cqe;

        /*
         * io_uring_wait_cqe(ring, cqe_ptr)
         *   - 阻塞直到有至少一个完成事件到达
         *   - 返回 0 成功，cqe 指向完成队列中的条目
         *   - cqe->res: 操作结果，正数表示成功（如读取的字节数），
         *               负数表示错误码（如 -EAGAIN）
         *   - cqe->user_data: 提交时通过 io_uring_sqe_set_data() 设置的值
         */
        ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            printf("io_uring_wait_cqe failed: %d\n", ret);
            continue;
        }

        /* 取出之前绑定的 conn_info，判断这是哪个操作完成了 */
        struct conn_info *info = (struct conn_info *)io_uring_cqe_get_data(cqe);

        /* 操作失败处理 */
        if (cqe->res < 0) {
            printf("operation error on fd %d: %s\n", info->fd, strerror(-cqe->res));
            if (info->buf)
                free(info->buf);
            free(info);
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        /*=====================================================================
         * 分支 A：accept 操作完成（type == 0）
         *
         * 当有新的客户端发起 TCP 连接时，内核完成 accept 操作，
         * cqe->res 中存放新连接的套接字描述符。
         *
         * 处理步骤：
         *   1. 获取新连接的 fd 并加入客户端池
         *   2. 释放上一次 accept 使用的地址缓冲区，重新分配供下一次使用
         *   3. 提交新的 accept 请求（继续监听新连接）
         *   4. 为新连接提交 read 请求（开始接收该客户端的数据）
         *=====================================================================*/
        if (info->type == 0) {
            int connfd = cqe->res;
            printf("新客户端接入, fd:%d (当前在线:%d)\n", connfd, client_count + 1);
            add_client(connfd);

            /* 重新分配 accept 地址缓冲区（上一个已被内核使用完） */
            free(cliaddr);
            free(clilen);

            cliaddr = malloc(sizeof(struct sockaddr_in));
            clilen = malloc(sizeof(socklen_t));
            *clilen = sizeof(struct sockaddr_in);

            /* 提交下一次 accept 请求 */
            struct conn_info *new_listen_info = malloc(sizeof(struct conn_info));
            new_listen_info->fd = listenfd;
            new_listen_info->type = 0;
            new_listen_info->buf = NULL;

            struct io_uring_sqe *accept_sqe = io_uring_get_sqe(&ring);
            io_uring_prep_accept(accept_sqe, listenfd,
                                 (struct sockaddr *)cliaddr, clilen, 0);
            io_uring_sqe_set_data(accept_sqe, new_listen_info);

            /* 为新连接提交第一个 read 请求 */
            struct conn_info *conn_info_ptr = malloc(sizeof(struct conn_info));
            conn_info_ptr->fd = connfd;
            conn_info_ptr->type = 1;  /* type=1 表示这是一个 read 请求 */
            conn_info_ptr->buf = malloc(MAXLINE);
            memset(conn_info_ptr->buf, 0, MAXLINE);

            struct io_uring_sqe *read_sqe = io_uring_get_sqe(&ring);
            io_uring_prep_read(read_sqe, connfd, conn_info_ptr->buf, MAXLINE, 0);
            io_uring_sqe_set_data(read_sqe, conn_info_ptr);

            /* 批量提交上面两个 SQE（accept + read）到内核 */
            io_uring_submit(&ring);

        /*=====================================================================
         * 分支 B：read 操作完成（type == 1）
         *
         * 当某个客户端发送数据时，内核完成 read 操作，
         * cqe->res 中存放实际读取的字节数。
         *
         * 处理步骤：
         *   1. 如果 bytes_read == 0，表示客户端关闭连接，清理资源
         *   2. 否则将消息广播给所有其他客户端（遍历 client_fds）
         *   3. 重新提交 read 请求（继续监听该客户端的数据）
         *=====================================================================*/
        } else {
            int bytes_read = cqe->res;
            int sockfd = info->fd;

            /* bytes_read == 0 表示对端关闭了连接（FIN 收到） */
            if (bytes_read == 0) {
                printf("[%d] 客户端断开 (当前在线:%d)\n", sockfd, client_count - 1);
                close(sockfd);
                remove_client(sockfd);
                free(info->buf);
                free(info);

            } else {
                printf("收到消息[fd=%d]: %s", sockfd, info->buf);

                /*
                 * 广播：将消息发送给所有其他在线客户端
                 *
                 * 注意：这里使用了同步的 write() 写操作，对于高并发场景
                 * 可以改用异步 write 来进一步提升性能。在这个示例中，
                 * 同步 write 已足够演示广播逻辑。
                 */
                for (int i = 0; i < client_count; i++) {
                    if (client_fds[i] != sockfd) {
                        int n = write(client_fds[i], info->buf, bytes_read);
                        if (n < 0) {
                            printf("write to fd %d failed\n", client_fds[i]);
                        }
                    }
                }

                /* 重新提交 read 请求，继续监听该客户端的数据 */
                memset(info->buf, 0, MAXLINE);
                struct io_uring_sqe *read_sqe = io_uring_get_sqe(&ring);
                io_uring_prep_read(read_sqe, sockfd, info->buf, MAXLINE, 0); /* 填充 SQE：准备从 sockfd 异步读取数据 */
                io_uring_sqe_set_data(read_sqe, info);                      /* 绑定上下文，CQE 回调时区分是哪个连接的 read */
                io_uring_submit(&ring);                                     /* 提交 SQE 到内核，触发异步读操作 */
            }
        }

        /*
         * io_uring_cqe_seen(ring, cqe)
         *   - 通知内核我们已经消费了这个 CQE，内核可以回收该条目
         *   - 如果不调用，CQ 队列会被占满，内核无法提交新的完成事件
         */
        io_uring_cqe_seen(&ring, cqe);
    }

    /* 清理（实际不会执行到这里，因为主循环是无限循环） */
    io_uring_queue_exit(&ring);
    close(listenfd);
    return 0;
}