# dmesg 命令手册：用法与排查场景

> 从 [001.内核重启排查](./001.内核重启排查/README.md) 与 [002.高负载D状态排查](./002.高负载D状态排查/README.md) 两个实战项目沉淀的 dmesg 用法汇总；选项以 Ubuntu 24.04 的 util-linux 2.39 为准。
> 一句话：**dmesg = 内核 printk 环形缓冲区的读取器**。它能干什么、不能干什么全由第 1 节的缓冲区性质决定；第 2 节选项速查；第 3 节先选对工具；第 4～6 节按场景给命令。

## 1. 原理：dmesg 读的是什么

- **数据源**：printk 环形缓冲区 `static char __log_buf[__LOG_BUF_LEN]`（[printk.c:459](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/printk/printk.c)），大小 `1 << CONFIG_LOG_BUF_SHIFT`（[printk.c:457](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/printk/printk.c)，默认 17 → 128 KiB，[Kconfig:683](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/init/Kconfig)；可用内核参数 `log_buf_len=` 在启动时调大，[printk.c:1134](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/printk/printk.c)）。**写满后覆盖最旧的记录**。
- **读取路径**：util-linux 版默认读 `/dev/kmsg`（需内核 ≥3.5），`-S` 强制退回 `syslog(2)` 系统调用；BusyBox 版只实现了 `klogctl`（即 syslog(2)）路径（[dmesg.c:52](../000.SOURCE_CODE/001.BUSYBOX/000.BUSYBOX-1.36.1/BusyBox-1.36.1/util-linux/dmesg.c)）。
- **时间戳**：缓冲区里存的是**开机起的单调时钟秒数**；`-T` 用「当前墙上时间 − uptime」换算成可读时间。SUSPEND/RESUME 期间单调时钟不走，换算会偏早（man page 明确警告；`--time-format=iso` 同样受影响）。
- **权限**：`kernel.dmesg_restrict=1` 时非 root 直接报 `dmesg: read kernel buffer failed: Operation not permitted`（开关定义见 [sysctl.c:2245](../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sysctl.c)）。

| 缓冲区性质 | 推论（决定了用法边界） |
|-|-|
| 只在内存里 | **重启即失忆**：重启原因永远不在重启后的 dmesg 里（[001](./001.内核重启排查/README.md) 的核心教训） |
| 环形、写满覆盖 | 高刷屏时旧消息被冲掉 → 现场要么立刻落盘存档，要么靠 journald 持久化 |
| 相对时间戳 | 与应用/审计日志对齐需 `-T` 换算，且注意 suspend 漂移、VM 时钟漂移 |
| systemd 等用户态也往 /dev/kmsg 写 | dmesg 里混有少量用户态消息（`-k` 只看内核的、`-u` 只看用户态的） |

## 2. 选项速查

```bash
# —— 输出格式 ——
dmesg -T                     # ★ 时间戳换成本地墙上时间（默认是 [秒.微秒] 开机相对时间）
dmesg -d                     # 额外显示相邻两条消息的间隔（定位"卡了多久才吐出下一条"）
dmesg -TL                    # -T + 颜色（管道里也要颜色用 --color=always）
dmesg -H                     # 人类友好：颜色+相对时间+分页（人工看现场舒服，脚本里别用）

# —— 过滤 ——
dmesg -l err,warn            # ★ 按级别：emerg/alert/crit/err/warn/notice/info/debug
dmesg -l err+                # err 及更严重（crit/alert/emerg）；也支持 +err 向低级别扩展
dmesg -x                     # 解码出每条消息的 facility/level 可读前缀
dmesg --since "1 hour ago"   # ★ 时间窗过滤（绝对/相对时间均可；老版本无此选项）
dmesg | grep -iE '...'       # 关键词初筛（关键词表见 2.1）

# —— 实时跟踪（故障复现窗口）——
dmesg -w                     # ★ 先回放现有缓冲区，再像 tail -f 一样等新消息（需 /dev/kmsg）
dmesg -W                     # 只等新消息、不回放（"从现在开始"的干净视角）

# —— 离线读文件 ——
dmesg -F vmcore-dmesg.txt    # 读 kdump 落盘的崩溃瞬间快照（syslog 格式，见 001 手册第 5 节）
dmesg -K dump.kmsg           # 读 /dev/kmsg 格式记录（NULL 字节分隔）

# —— 危险区（改状态；排查现场不要用）——
dmesg -c                     # 打印后清空缓冲区 → 毁掉后续排查的证据
dmesg -C                     # 直接清空
dmesg -n 3 / -D / -E         # 改 console 日志级别 / 关 / 开控制台打印 → 影响所有人
dmesg -s 65536               # 走 syslog(2) 路径时的用户态读取缓冲（默认 16392，一般不动）
dmesg -S                     # 强制 syslog(2) 而非 /dev/kmsg（旧行为，一般不用）
```

嵌入式注意：BusyBox 版 dmesg（本仓库 [BusyBox dmesg.c](../000.SOURCE_CODE/001.BUSYBOX/000.BUSYBOX-1.36.1/BusyBox-1.36.1/util-linux/dmesg.c)）只支持 `-c/-n/-s/-r`，**没有** `-T/-l/--since/-w`——在最小系统/initramfs/嵌入式板上别按上表写脚本，时间戳要自己换算。

### 2.1 判读关键词表（两个实战项目沉淀）

| 关键词（dmesg / journalctl -k 里 grep） | 含义 | 出处 |
|-|-|-|
| `Kernel panic` / `Oops` / `BUG:` / `Call Trace` | 内核崩溃 | [001](./001.内核重启排查/README.md) |
| `MCE` / `Hardware Error` / `EDAC` | 硬件/内存错误 | 同上 |
| `Out of memory` / `oom-killer` | 内存耗尽 | 同上 |
| `Power-on or device reset occurred`（virtio 盘） | 经历过掉电/复位周期（无法区分 guest 正常 reboot 与宿主机强制重启） | 同上 |
| EXT4 挂载时**无** `recovery complete` 字样 | 上次大概率是干净关机（软证据） | 同上 |
| `nfs: server ... not responding, still trying` | 远端存储失联 | [002](./002.高负载D状态排查/README.md) |
| `blocked for more than 120 seconds` | hung task：任务在 D 状态卡超时 | 同上 |
| `I/O error, device ...` | 块设备 I/O 错误 | 同上 |
| `reset`（scsi/nvme/controller） | 设备复位重试中 | 同上 |

## 3. 选对工具：dmesg / journalctl -k / 跨重启幸存通道

| 工具 | 数据源 | 时间窗过滤 | 跨重启 | 典型用途 |
|-|-|-|-|-|
| `dmesg` | 内存环形缓冲区（实时） | `--since/--until` | ✗ | 本次启动、当前缓冲区；故障进行时 |
| `journalctl -k` | journald 落盘的内核日志 | ★ `--since/--until` 最强 | ✓（需持久化） | 历史回溯、与变更时间窗对齐定责（002 用法） |
| `journalctl -b -1 -k` | 上一次启动 | ✓ | ✓ | 重启排查（001 用法） |
| `dmesg -F /var/crash/*/vmcore-dmesg.txt` | kdump 崩溃快照 | — | ✓ | 崩溃瞬间的最后输出 |
| `cat /sys/fs/pstore/console-ramoops*` | pstore/ramoops | — | ✓ | 掉电前最后的 printk |

口诀：**正在发生的看 dmesg；已经过去的看 journalctl -k；已经重启的看 journalctl -b -1；已经崩溃的看 vmcore/pstore。** journalctl 的完整用法与场景见 [journalctl命令手册.md](./journalctl命令手册.md)。

## 4. 场景一：开机巡检与机器画像

拿到一台陌生机器（或云主机）先花一分钟摸底——[001](./001.内核重启排查/README.md) 第 1、2 节即用此法从一份 dmesg 里画出机器全貌：

```bash
dmesg -T | head -5                        # ★ 首条 Linux version 行的时间 = 本次开机时刻
dmesg -T | grep -iE 'DMI:|Hypervisor detected'   # 物理机还是 VM、什么平台（KVM/xen/vmware）
dmesg -T | grep -iE 'smp:|Memory:.*available|virtio|nvme|sd [a-z]'   # CPU/内存/磁盘枚举画像
dmesg -l err,warn                         # ★ 本次启动有没有报错
dmesg -T | grep -iE 'pstore'              # 有没有注册 pstore 后端（将来崩溃能否留证据）
```

判读要点：

- 无 err/warn **不等于**健康——不少驱动失败只打 notice 级，过一遍 2.1 关键词表更稳；
- 虚拟机专属软证据（virtio 掉电复位、EXT4 recovery）见 [001 README](./001.内核重启排查/README.md) 第 2 节证据 4、5；
- 开机时刻与 `uptime -s` 互相佐证。

## 5. 场景二：故障进行时（负载异常 / 慢 / 超时）

对应 [002](./002.高负载D状态排查/README.md) 的主线：内核日志负责**定责**（它在等谁），不负责定位（哪个线程卡在哪——那是 `ps -eLo wchan` + `/proc/<TID>/stack` 的事，见 [002 手册](./002.高负载D状态排查/001.排查命令手册.md)第 3、4 节）。

```bash
date -Is && dmesg -Tw                      # ★ 先固定绝对时间锚点，再开实时跟踪（输出建议 tee 到文件）
dmesg -T > dmesg-$(date +%F-%H%M).txt      # ★ 高刷屏时先落盘存档——环形缓冲区会被冲掉
dmesg -T --since "13:40:00"                # 限定告警时间窗（覆盖告警前几分钟）
dmesg -T | grep -inE 'not responding|timed out|I/O error|reset|blocked for more than'
```

要点：

- 内核日志与「健康节点对照 + 变更记录」一起才构成证据链——单凭一条 `nfs: server not responding` 定不了根因（[002 手册](./002.高负载D状态排查/001.排查命令手册.md)第 5 节三旁证）；
- 事后回溯优先 `journalctl -k --since/--until`：墙钟时间直接可过滤，且不受缓冲区覆盖影响。

## 6. 场景三：重启 / 崩溃之后（dmesg 的边界）

[001](./001.内核重启排查/README.md) 的核心结论，务必先记住再动手：

- **这份 dmesg 看不到重启原因**：环形缓冲区随上一个内核消亡，重启后 dmesg 只剩新内核自己的输出——无论上次是正常关机还是内核崩溃。
- 重启后本次 dmesg 的正确用法只有两个：
  1. **证明本次启动干净**（全程无 panic/Oops/MCE → 重启不是本次运行期间的问题导致）；
  2. **软证据**：virtio `Power-on or device reset occurred`、EXT4 无 `recovery complete`。
- 真正要看的是**上一次启动的尾部**：`journalctl -b -1 -n 100`（用户态持久层）；内核侧日志要跨重启，得事前部署（[001 手册](./001.内核重启排查/001.排查命令手册.md)第 7 节）：netconsole（实时吐远端）、kdump（崩溃瞬间 vmcore + `vmcore-dmesg.txt`）、pstore/ramoops（掉电前最后 printk）。

## 7. 常见坑速查

| 症状 | 原因 | 处置 |
|-|-|-|
| `dmesg: read kernel buffer failed: Operation not permitted` | `kernel.dmesg_restrict=1` | 用 sudo；放开 sysctl 前权衡暴露面 |
| 之前明明看到过的消息不见了 | 环形缓冲区被新消息覆盖 / 机器重启过 | 靠 journalctl -k（持久化）；事前 netconsole |
| `-T` 的时间和应用日志对不上 | 单调时钟 suspend 期间不走 / VM 时钟漂移 | 以 journald 接收时间为准；交叉系统对比用 `--time-format=iso` 也要小心 |
| 嵌入式板上 `-T`/`-l` 报 invalid option | BusyBox 版只有 `-c/-n/-s/-r` | 手动换算时间戳；或换 util-linux/dmesg |
| 想要"从现在开始"的日志却把缓冲区清了 | 用了 `-c/-C` | 用 `-W`（只等新消息），永远别在排查现场用 `-c/-C` |
