# 高负载 D 状态排查

## 核心结论

1. **负载高 ≠ CPU 不够**。Load Average 统计的是「运行 + 就绪 + **不可中断睡眠（D 状态）**」三类任务数（[loadavg.c:84](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/loadavg.c) `nr_active += this_rq->nr_uninterruptible`），D 状态任务不占 CPU，所以「load 100 + idle 80%」是合法组合。
2. **`kill -9` 杀不掉是确诊线索，不是玄学**。信号只能唤醒可中断睡眠（[signal.c:769](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/signal.c) `wake_up_state(t, state | TASK_INTERRUPTIBLE)`）；对 D 状态任务，kill -9 只是把信号挂到 pending，任务必须先从内核等待路径返回才有机会处理退出。反复 `kill -9` 没有诊断价值。
3. **排查主线一句话**：先用 `vmstat`（r 低、b 高、id 高）证明负载来自阻塞而非 CPU 短缺，再用线程级 `ps` 拿到 D 状态任务的 `wchan`，沿「wchan → 内核栈 → 设备/文件系统指标 → 内核日志」定位**它在等谁**。
4. **案例根因不在 Linux 也不在 Java**：路由变更 → `nfs: server not responding` → 大量线程 D 在 rpc 等待路径。此时扩容只会把更多实例送进同一条故障路径。

## 1. 现象与反常识点

| 现场 | 下意识误判 | 实际含义 |
|-|-|-|
| Load Average 94（16 核） | CPU 严重过载，赶紧扩容 | 负载里混入了 D 状态任务，本例来自排队等远端存储的线程 |
| CPU Idle ~80%，`r` 不高 | 「负载是误报」 | 排队发生在内核等待路径，不在 CPU 运行队列 |
| `kill -9` 后进程迟迟不退 | 进程僵死 / 要重启机器 | D 状态任务不响应信号是内核设计行为（见 2.2 节） |
| 扩容后故障依旧 | 容量还不够 | 依赖（存储网络）仍异常，新实例同样被阻塞 |

## 2. 内核原理（本仓库 5.9 源码为据）

### 2.1 Load Average 统计口径

- 每个采样周期（5 秒，[loadavg.h:20](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched/loadavg.h) `#define LOAD_FREQ (5*HZ+1)`）把各 CPU 的 `nr_running` 与 `nr_uninterruptible` 相加作为活跃任务数（[loadavg.c:79-84](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/loadavg.c) `calc_load_fold_active()`），再指数滑动平均出 1/5/15 分钟值。文件头注释（行 18–52）明确写了 `nr_active += nr_running + nr_uninterruptible`。
- 推论：① D 状态堆积会**直接**推高负载；② 故障解除后负载按滑动平均慢慢回落，**不能只看某一刻的 load 判断是否恢复**，要看趋势。

### 2.2 为什么 kill -9 杀不掉 D 状态任务

- D 状态即 `TASK_UNINTERRUPTIBLE`（[sched.h:81](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched.h) `#define TASK_UNINTERRUPTIBLE 0x0002`）。
- 发送信号最终走到 [signal.c:759-770](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/signal.c) `signal_wake_up_state()`：设置 `TIF_SIGPENDING` 后调用 `wake_up_state(t, state | TASK_INTERRUPTIBLE)`——唤醒条件只匹配可中断睡眠。纯 `TASK_UNINTERRUPTIBLE` 的任务唤不醒，信号只能挂着，等它自己从等待点返回、退出到可处理信号的位置才会死。
- 特例 `TASK_KILLABLE`（[sched.h:98](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched.h)）＝ `TASK_WAKEKILL | TASK_UNINTERRUPTIBLE`：内核部分路径（NFS 常用）用 killable wait，致命信号可将其唤醒——这解释了为什么有些 D 进程又「能」被杀掉。
- 注意：`ps` 状态列的 `D` 是 **不可中断睡眠**，不等于「磁盘坏」。它可能在等本地盘/云盘 I/O、NFS/Ceph 远端存储、块设备/SCSI/NVMe/多路径故障恢复、文件系统或驱动内部路径，极少数是内核缺陷/硬件异常。

## 3. 排查思路（证据链怎么搭）

> 原则：**高负载只是排队的结果，D 状态才指向等待现场**。先说清「谁在等、等在哪里」，再决定扩容/止损。全程只读采证，命令见 [001.排查命令手册.md](./001.排查命令手册.md) 第 1～5 节。

| 步骤 | 动作 | 关键判读 | 结论分支 |
|-|-|-|-|
| ① 固定现场 | 记录绝对时间、主机、内核版本、核数、工具版本 | 不同内核/sysstat 版本字段有差异 | — |
| ② 负载来源 | `uptime` + `vmstat 1 10` | `r` 低、`b` 高、`id` 高 → 阻塞，不是 CPU 短缺 | `wa` 高只是佐证；**`wa` 低不能排除 I/O**（远端存储/设备超时未必计入 wa） |
| ③ 压力度量 | `/proc/pressure/{cpu,memory,io}`（PSI, 4.20+） | `io` 的 `some/full avg10/60/300`＝停顿时长占比 | 只回答「业务停了多久」，不回答「卡在哪个设备」 |
| ④ 线程级找 D | `ps -eLo state,pid,tid,ppid,wchan:32,comm` 过滤 `D` | **按线程不按进程**（进程级会漏掉多线程程序内部卡住的线程） | 三问：是否集中在同一业务进程？`wchan` 是否相同？是否访问同一设备/文件系统？ |
| ⑤ 等待路径 | `cat /proc/<PID>/task/<TID>/stack` | 内核栈指向具体子系统（如 rpc_wait） | `wchan` 显示 `0`/`-` 可能是权限/配置，不能据此排除阻塞 |
| ⑥ 设备分支 | 本地盘：`lsblk`/`findmnt`/`iostat -xz`；NFS：`findmnt -t nfs`/`nfsstat -m`/`nfsiostat` | `await` 与队列增长＝恶化；`%util`≈100% **不等于**达到吞吐上限（NVMe/阵列并行能力强） | 与健康节点基线对照 |
| ⑦ 日志与变更 | `journalctl -k --since/--until`（窗口覆盖告警前几分钟）搜 `not responding`/`timed out`/`I/O error`/`reset`/`blocked for more than`/`nvme`/`scsi`/`nfs` | 内核日志 + 变更记录 + 健康节点对照，串成完整证据链 | — |

### 案例的证据链（文章脱敏数据）

| # | 证据 | 来源 |
|-|-|-|
| 1 | `load average: 94.31, 61.08, 25.47`，但 vmstat 显示 idle ~80%、`r` 不高、`b` 在 60–75 | `uptime` / `vmstat 1 10` |
| 2 | 内存 PSI 无异常，本地块设备 `iostat` 与健康节点一致 → **排除 CPU、内存、本地盘** | PSI / `iostat -xz` |
| 3 | **71 个 D 状态线程**，同属一组 Java 工作进程，`wchan` 集中在 NFS/RPC 等待路径 | 线程级 `ps -eLo` |
| 4 | 代表性线程内核栈指向远端文件读取等待 | `/proc/<PID>/task/<TID>/stack` |
| 5 | `nfs: server 10.0.18.24 not responding, still trying` | `journalctl -k` |
| 6 | 该挂载点 RPC 重传持续增加、平均往返时间显著高于故障前基线 | `nfsiostat 1 5` |
| 7 | 另一可用区同版本应用节点无 D 堆积、访问同一业务数据正常 → 排除应用自身 | 健康节点对照 |
| 8 | 故障前十分钟改过该可用区到存储网段的路由策略 → **回滚后 NFS 恢复、D 陆续返回、超时停止** | 变更记录 |

根因：**存储网络路径异常导致远端文件访问被阻塞**——不是「Linux 负载太高」，也不是「Java 线程太多」。复盘时保留被证伪的方向（CPU/内存/本地盘为何被排除）与根因同样重要。

## 4. 止损与恢复原则（每一步都要能退回来）

- **摘流量优先于杀进程**：有冗余时先把异常节点摘出流量路径；具体操作取决于负载均衡器/调度平台，执行前确认健康节点接得住。
- **Kubernetes 止损顺序**：确认集群上下文（context）与 PDB（PodDisruptionBudget） → 保存节点与 Pod 清单 → `cordon`（只挡新调度，不驱逐；回退 `uncordon`）→ 需要迁移时 `drain --ignore-daemonsets --dry-run=client` 先预览 → 再 `--timeout=10m` 受限驱逐。**故意不用 `--force`**（可能删掉无控制器接管的 Pod）**和 `--delete-emptydir-data`**（丢本地临时数据）。
- **D 状态会导致 Pod 无法按时终止**：drain 超时就停手重新评估，不要继续扩大破坏范围。已驱逐的 Pod 不会因 `uncordon` 自动搬回，完整回退要结合副本数/流量入口/发布系统。
- **根因修复禁忌**：NFS 故障优先恢复服务端或网络路径，不要对繁忙挂载点强制 umount；不要为了「能杀掉进程」把 `hard` 改 `soft`（长等待变成应用错误，特定场景有数据完整性风险）；无厂商指导不删块设备/重置控制器/分离云盘；重启节点只能是证据保存后的兜底（强制重启不可回滚，可能带来文件系统损坏）。
- **恢复流量从小范围开始**：单节点/小流量验证 `uptime`、`vmstat`、D 计数、PSI io、`iostat`，同时观察业务错误率、延迟分位数、存储重传和内核新日志；异常复现立即回摘并回退上一版配置。

## 5. 与本仓库其他笔记的关联

- [001.内核重启排查](../001.内核重启排查/README.md)：同一方法论——先保证据再动手、`journalctl` 时间窗限定、结局判读表分支推进。
- 内核源码引用：[kernel/sched/loadavg.c](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/loadavg.c)、[kernel/signal.c](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/signal.c)、[include/linux/sched.h](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched.h)、[include/linux/sched/loadavg.h](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched/loadavg.h)。
