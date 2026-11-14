.. SPDX-License-Identifier: GPL-2.0

=======================================
FUSE-over-io-uring design documentation
=======================================

(FUSE-over-io-uring 设计文档)

This documentation covers basic details how the fuse
kernel/userspace communication through io-uring is configured
and works. For generic details about FUSE see fuse.rst.
(本文档涵盖 fuse 内核/用户空间通过 io-uring 进行通信的基本配置和工作原理细节。关于 FUSE 的一般细节请参见 fuse.rst。)

This document also covers the current interface, which is
still in development and might change.
(本文档还涵盖了当前的接口，该接口仍在开发中，可能会发生变化。)

Limitations
(限制)
===========
As of now not all requests types are supported through io-uring, userspace
is required to also handle requests through /dev/fuse after io-uring setup
is complete. Specifically notifications (initiated from the daemon side)
and interrupts.
(截至目前，并非所有请求类型都支持通过 io-uring 处理，在 io-uring 设置完成后，用户空间仍需要通过 /dev/fuse 处理请求，具体来说就是通知(由守护进程侧发起)和中断。)

Fuse io-uring configuration
(Fuse io-uring 配置)
===========================

Fuse kernel requests are queued through the classical /dev/fuse
read/write interface - until io-uring setup is complete.
(在 io-uring 设置完成之前，fuse 内核请求都通过经典的 /dev/fuse 读/写接口排队。)

In order to set up fuse-over-io-uring fuse-server (user-space)
needs to submit SQEs (opcode = IORING_OP_URING_CMD) to the /dev/fuse
connection file descriptor. Initial submit is with the sub command
FUSE_URING_REQ_REGISTER, which will just register entries to be
available in the kernel.
(为了设置 fuse-over-io-uring，fuse 服务器(用户空间)需要向 /dev/fuse 连接文件描述符提交 SQE(opcode = IORING_OP_URING_CMD)。初始提交使用子命令 FUSE_URING_REQ_REGISTER，它只是将条目注册为在内核中可用。)

Once at least one entry per queue is submitted, kernel starts
to enqueue to ring queues.
Note, every CPU core has its own fuse-io-uring queue.
Userspace handles the CQE/fuse-request and submits the result as
subcommand FUSE_URING_REQ_COMMIT_AND_FETCH - kernel completes
the requests and also marks the entry available again. If there are
pending requests waiting the request will be immediately submitted
to the daemon again.
(一旦每个队列至少提交了一个条目，内核就开始向环形队列入队。注意，每个 CPU 核心都有自己的 fuse-io-uring 队列。用户空间处理 CQE/fuse 请求并以子命令 FUSE_URING_REQ_COMMIT_AND_FETCH 提交结果——内核完成请求并将该条目标记为再次可用。如果有等待中的请求，该请求会立即再次提交给守护进程。)

Initial SQE
(初始 SQE)
-----------::

 |                                    |  FUSE filesystem daemon
 |                                    |
 |                                    |  >io_uring_submit()
 |                                    |   IORING_OP_URING_CMD /
 |                                    |   FUSE_URING_CMD_REGISTER
 |                                    |  [wait cqe]
 |                                    |   >io_uring_wait_cqe() or
 |                                    |   >io_uring_submit_and_wait()
 |                                    |
 |  >fuse_uring_cmd()                 |
 |   >fuse_uring_register()           |


Sending requests with CQEs
(通过 CQE 发送请求)
--------------------------::

 |                                           |  FUSE filesystem daemon
 |                                           |  [waiting for CQEs]
 |  "rm /mnt/fuse/file"                      |
 |                                           |
 |  >sys_unlink()                            |
 |    >fuse_unlink()                         |
 |      [allocate request]                   |
 |      >fuse_send_one()                     |
 |        ...                                |
 |       >fuse_uring_queue_fuse_req          |
 |        [queue request on fg queue]        |
 |         >fuse_uring_add_req_to_ring_ent() |
 |         ...                               |
 |          >fuse_uring_copy_to_ring()       |
 |          >io_uring_cmd_done()             |
 |       >request_wait_answer()              |
 |         [sleep on req->waitq]             |
 |                                           |  [receives and handles CQE]
 |                                           |  [submit result and fetch next]
 |                                           |  >io_uring_submit()
 |                                           |   IORING_OP_URING_CMD/
 |                                           |   FUSE_URING_CMD_COMMIT_AND_FETCH
 |  >fuse_uring_cmd()                        |
 |   >fuse_uring_commit_fetch()              |
 |    >fuse_uring_commit()                   |
 |     >fuse_uring_copy_from_ring()          |
 |      [ copy the result to the fuse req]   |
 |     >fuse_uring_req_end()                 |
 |      >fuse_request_end()                  |
 |       [wake up req->waitq]                |
 |    >fuse_uring_next_fuse_req              |
 |       [wait or handle next req]           |
 |                                           |
 |       [req->waitq woken up]               |
 |    <fuse_unlink()                         |
 |  <sys_unlink()                            |



