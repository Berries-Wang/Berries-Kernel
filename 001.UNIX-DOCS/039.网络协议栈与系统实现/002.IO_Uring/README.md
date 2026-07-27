# io_uring

|源码类型|源代码路径|备注|
|-|-|-|
|- 系统调用入口|- 000.LINUX-7.1.3/io_uring/io_uring.c|-|
|-|-|-|
|- 用户API定义|- 000.LINUX-7.1.3/include/uapi/linux/io_uring.h|-|
|-|-|-|
|- 内核内部类型|- 000.LINUX-7.1.3/include/linux/io_uring_types.h|-|
|-|-|-|
|- 操作分发表  |- 000.LINUX-7.1.3/io_uring/opdef.c|-|
|-|-|-|
|- 辅助子系统|- 000.LINUX-7.1.3/io_uring/sqpoll.c | 000.LINUX-7.1.3/io_uring/io-wq.c|-|
|-|-|-|

## 数据结构
### io_uring_sqe <sup>000.LINUX-7.1.3/include/uapi/linux/io_uring.h</sup>
- ![wechat_2026-07-26_170407_318.png](../../../001.UNIX-DOCS/999.IMGS/wechat_2026-07-26_170407_318.png)


### io_uring_cqe <sup>000.LINUX-7.1.3/include/uapi/linux/io_uring.h</sup>




## 核心函数
|函数名|定义|文件路径|备注|
|-|-|-|-|
|- io_uring_setup|- SYSCALL_DEFINE2(io_uring_setup, u32, entries, struct io_uring_params __user *, params)...|- 000.LINUX-7.1.3/io_uring/io_uring.c|-|
|-|-|-|-|
|-io_uring_enter|- SYSCALL_DEFINE6(io_uring_enter, unsigned int, fd, u32, to_submit, u32, min_complete, u32, flags, const void __user *, argp, size_t, argsz)...|- 000.LINUX-7.1.3/io_uring/io_uring.c|-io_uring性能的核心秘密:<br/> 传统IO：提交&获取结果，两次系统调用 ；<br/>io_uring:只需要一次系统调用|
|-|-|-|-|
|-io_submit_sqes|-|-|- 批量提交引擎|
|-|-|-|-|
|- io_issue_sqe|-|-|- io_uring 的多态|
|-|-|-|-|
|- io_uring_register|-|-|-缓存file指针，绕过原子引用计数，提升性能|
|-|-|-|-|

### 处理流程

**调用链路全景** (基于 000.LINUX-7.1.3/io_uring/)

```
用户态                                         内核态
──────────────────────────────────────────────────────────────────────

[1] io_uring_setup()  [io_uring.c:3159]
    │
    └─ io_uring_create()                      [io_uring.c:2987]
         ├─ io_ring_ctx_alloc()               分配 io_ring_ctx 上下文
         ├─ io_allocate_scq_urings()          分配 SQ/CQ 共享内存环形缓冲区
         │    └─ mmap 共享内存：用户态与内核态共享 SQ 和 CQ 环
         ├─ io_sq_offload_create()            创建 SQPOLL 线程（如启用）
         └─ __io_uring_add_tctx_node()        关联当前任务到 io_uring 上下文

[2] io_uring_register()  [register.c:1006]
    │
    └─ __io_uring_register()                 [register.c:739]
         ├─ io_sqe_files_register()           注册文件（固定文件表）
         │    └─ 绕过 fget/fput 原子引用计数，直接通过索引访问
         ├─ io_sqe_buffers_register()         注册缓冲区（固定缓冲区）
         │    └─ 预映射用户态内存，避免每次 IO 都做 get_user_pages
         └─ io_register_personality() / 其他

[3] io_uring_enter()  [io_uring.c:2609]  ★ 核心提交路径
    │
    ├─ [SQPOLL 模式]
    │    ├─ wake_up(&ctx->sq_data->wait)      唤醒内核 SQ 轮询线程
    │    └─ io_sqpoll_wait_sq()               可选等待 SQ 条目被消费
    │
    └─ [非 SQPOLL 模式]  —— 直接提交
         ├─ io_uring_add_tctx_node()           获取/创建 io_uring_task
         ├─ mutex_lock(&ctx->uring_lock)
         └─ io_submit_sqes()                  [io_uring.c:2035]  ★ 批量提交引擎
              │
              ├─ 循环: do { ... } while (--left)
              │    ├─ io_alloc_req()           从缓存分配 io_kiocb 请求对象
              │    ├─ io_get_sqe()             从 SQ 环获取下一个 SQE 条目
              │    └─ io_submit_sqe()           [io_uring.c:1895]
              │         │
              │         ├─ io_init_req()        [io_uring.c:1743]
              │         │    ├─ 解析 SQE: opcode, flags, user_data, fd, buf_index...
              │         │    ├─ 验证 opcode 范围
              │         │    ├─ 处理 IOSQE_IO_LINK 链式请求
              │         │    └─ 初始化 io_kiocb 各字段
              │         │
              │         └─ io_queue_sqe()       [io_uring.c:1655]  ★ 多态分发
              │              │
              │              └─ io_issue_sqe()  [io_uring.c:1423]
              │                   │
              │                   └─ __io_issue_sqe()  [io_uring.c:1390]
              │                        │
              │                        └─ def->issue(req, issue_flags) ← 多态派发
              │                             │
              │                             │  io_issue_defs[] 表 [opdef.c:54]
              │                             │  ├─ IORING_OP_READV   → io_read()
              │                             │  ├─ IORING_OP_WRITEV  → io_write()
              │                             │  ├─ IORING_OP_ACCEPT  → io_accept()
              │                             │  ├─ IORING_OP_SEND   → io_send()
              │                             │  ├─ IORING_OP_RECV   → io_recv()
              │                             │  ├─ IORING_OP_POLL_ADD → io_poll_add()
              │                             │  ├─ IORING_OP_TIMEOUT → io_timeout()
              │                             │  └─ ... (40+ 种 opcode)
              │                             │
              │                             ├─ 同步完成: IOU_COMPLETE
              │                             │    ├─ io_req_complete_defer() 延迟完成
              │                             │    └─ io_req_complete_post()  立即完成
              │                             │         └─ 填充 CQE → 更新 CQ 环
              │                             │              → 唤醒等待 CQ 事件的用户态
              │                             │
              │                             └─ 异步: ret != 0
              │                                  └─ io_queue_async()
              │                                       ├─ io_queue_iowq()  → io-wq 线程池
              │                                       └─ io_arm_poll_handler() → epoll 机制
              │
              └─ 循环结束

[4] 完成事件收割
    │
    ├─ io_uring_enter(IORING_ENTER_GETEVENTS)  获取已完成事件
    │    └─ io_cqring_wait()                   等待 CQ 环中有新事件
    │
    └─ 用户态直接读取 CQ 环（无需系统调用，无锁）
         └─ smp_load_acquire(cq->tail) 获取最新完成事件
```

**SQPOLL 模式详解**

```
用户态                                       内核 SQPOLL 线程
──────────────────────────────────────────────────────────────────────

[初始化] io_uring_setup(IORING_SETUP_SQPOLL)
    │
    └─ io_sq_offload_create()                创建内核线程 io_sq_thread
         │                                     [sqpoll.c:293]
         └─ 内核线程绑定到指定 CPU，持续运行

[提交] 用户态填充 SQE → 更新 SQ 尾指针
    │
    ├─ [默认] SQPOLL 线程主动轮询 SQ 环
    │    └─ __io_sq_thread()                  [sqpoll.c:204]
    │         ├─ io_sqring_entries()          检测新提交的 SQE
    │         ├─ io_submit_sqes()             批量提交（同非 SQPOLL 路径）
    │         └─ io_do_iopoll()               处理 IO 轮询完成
    │
    └─ [SQ 空闲超时] io_sq_thread 进入休眠
         │
         └─ [用户再次提交时] 设置 IORING_SQ_NEED_WAKEUP 标志
              └─ io_uring_enter(IORING_ENTER_SQ_WAKEUP)
                   └─ wake_up(&ctx->sq_data->wait)  唤醒 SQPOLL 线程

[完成] SQPOLL 线程将结果写入 CQ 环
    │
    └─ 用户态直接读取 CQ 环（零系统调用，真正的 0 拷贝完成事件）
```

**传统 IO vs io_uring 对比**

| 对比项 | 传统 IO (read/write) | io_uring (非 SQPOLL) | io_uring (SQPOLL) |
|--------|---------------------|---------------------|-------------------|
| 提交 | 1 次系统调用 | 1 次系统调用 (批量) | 0 次系统调用 |
| 完成 | 1 次系统调用 | 可选 1 次系统调用 | 0 次系统调用 |
| 每 IO 系统调用数 | 2 次 | 0~1 次 | 0 次 |
| 数据拷贝 | 需要 | 注册缓冲区避免 | 注册缓冲区避免 |
| 适用场景 | 通用 | 高吞吐 IO | 超低延迟、高 IOPS |

**关键设计要点**

1. **共享内存环** (`struct io_rings`): 用户态和内核态通过 mmap 共享 SQ (Submission Queue) 和 CQ (Completion Queue) 环形缓冲区，避免数据拷贝
2. **批量提交** (`io_submit_sqes`): 一次系统调用可批量提交多个 SQE，分摊系统调用开销
3. **多态分发** (`io_issue_defs[]`): 通过 opcode 索引 `io_issue_defs` 表，如 `io_issue_defs[IORING_OP_READV].issue = io_read()`，实现统一的 SQE 分发机制
4. **固定文件/缓冲区** (`io_uring_register`): 注册后的文件绕过 `fget/fput` 原子引用计数，注册后的缓冲区预映射避免每 IO 做 `get_user_pages`
5. **无锁收割** (SQPOLL): 用户态直接读取 CQ 环，使用 `smp_load_acquire` / `smp_store_release` 保证内存序，无需系统调用


## 零拷贝基石： 共享内存环机制:用户态和内核态共享内存环形缓冲区
> 阅读:[000.LINUX-7.1.3/include/linux/io_uring_types.h]#struct io_rings{....}
- ![wechat_2026-07-26_175342_061.png](../../../001.UNIX-DOCS/999.IMGS/wechat_2026-07-26_175342_061.png)

---

## 参考资料
- [https://blog.cloudflare.com/missing-manuals-io_uring-worker-pool/](https://blog.cloudflare.com/missing-manuals-io_uring-worker-pool/)
- ![wechat_2026-07-26_180220_630.png](../../../001.UNIX-DOCS/999.IMGS/wechat_2026-07-26_180220_630.png)
