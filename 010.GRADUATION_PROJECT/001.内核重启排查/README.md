# 内核重启排查 —— srv1572367 · 2026-10-17 19:54:52 的一次重启

> **事件**：云主机 `srv1572367` 于 2026-10-17（周六）19:54:52 发生一次重启。
> **材料**：重启后约 24.5 小时（10-18 20:19）采集的 `dmesg -T` 全量输出 → [logs/dmesg-srv1572367-20261018.txt](./logs/dmesg-srv1572367-20261018.txt)（743 行，行号即该文件行号）。
> **工具**：排查所用全部命令见 [001.排查命令手册.md](./001.排查命令手册.md)（可复用 runbook）。
> **状态**：阶段结论一 —— dmesg 不足以定位原因，待 `journalctl -b -1` 确认。

## TL;DR

1. **这份 dmesg 看不到重启原因**：`dmesg` 读的是内核日志环形缓冲区，它在内存里、随内核消亡——重启后只剩新内核（19:54:52 起）自己的输出，而重启原因恰恰在**上一次启动日志的末尾**，那里已经丢了。
2. **本次启动完全干净**：硬件枚举 → systemd → graphical.target 全程无 panic／Oops／硬件报错；系统此后安静运行至采集时刻。重启不是本次运行期间的问题导致的。
3. **两条软证据**：① virtio 磁盘上报「经历过断电或复位」；② EXT4 根文件系统挂载时**没有** journal 回放（recovery）迹象——后者提示上次关机大概率走了正常的 shutdown 流程（文件系统被干净卸载），而非强制断电。均待 `journalctl -b -1` 确认。
4. **下一步只有一条关键命令**：`journalctl -b -1 -n 100 --no-pager`（上一次启动的最后 100 行），按「结局判读」表定性。

## 1. 机器与事件

| 项目 | 内容 | 日志依据 |
|-|-|-|
| 主机 | `srv1572367`，KVM 虚拟机（云主机） | 行 668、行 24–25 |
| 系统 | Ubuntu 24.04（systemd 255.4） | 行 665 |
| 内核 | `6.8.0-147-generic`（x86_64，PREEMPT_DYNAMIC） | 行 2 |
| 虚拟硬件 | 2 vCPU／8 GiB RAM／100 GiB virtio-blk（sda）／virtio-net | 行 88、113、565 |
| 宿主 CPU | AMD EPYC 9354P 32-Core Processor | 行 177 |
| 重启时刻 | **2026-10-17 19:54:52**（本次内核首条日志时间戳；RTC 19:54:54 与之吻合） | 行 2、561 |
| dmesg 采集时刻 | 2026-10-18 20:19（即重启后约 24.5 h） | 文件名时间戳 |

## 2. 证据链：dmesg 能确定的事实

| # | 事实 | 日志依据（行号） |
|-|-|-|
| 1 | 本次启动始于 2026-10-17 19:54:52，**重启就发生在这一刻之前** | 行 2 `Linux version ...` |
| 2 | 这是一台 QEMU/KVM 虚拟机，而非物理机 → 「宿主机侧动作」必须纳入嫌疑 | 行 24–25 `DMI: QEMU ...` / `Hypervisor detected: KVM` |
| 3 | 本次启动**全程无任何报错**：无 panic／Oops／MCE／硬件错误，systemd 完整跑到 graphical.target | 通篇（对照排查手册 §3 的崩溃关键词清单） |
| 4 | virtio 磁盘在启动时上报「经历过断电或复位」——VM 经历过一次完整的掉电/复位周期，但**无法区分** guest 内部正常 `reboot` 与宿主机强制重启 | 行 563 `sd 0:0:0:0: Power-on or device reset occurred` |
| 5 | **软证据**：EXT4 根分区挂载时无 journal 回放迹象 → 上次关机时文件系统大概率被干净卸载 → 倾向「经历过正常 shutdown 流程」 | 行 663 `EXT4-fs (sda1): mounted filesystem ... ro`（对比：强制断电后常见 `recovery complete`／jbd2 恢复消息，此处没有） |
| 6 | 持久化 journal 存在（`/var/log/journal`）→ **上一次启动的用户态日志大概率还在磁盘上，可直接查** | 行 722 `/var/log/journal/.../system.journal` |
| 7 | 采集前最后两条内核日志只是 journald 按保留期轮转清理，与重启无关；系统安静运行 | 行 742–743（10-18 05:36 / 06:08） |

## 3. 原理：为什么 dmesg 看不到重启原因

- `dmesg` 输出的是当前内核的 printk 环形缓冲区：一块静态分配在内核镜像里的内存（[printk.c:459](../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/printk/printk.c) `static char __log_buf[__LOG_BUF_LEN]`）。新内核启动时在自己的 `.bss` 里重新初始化它，**上一个内核的 printk 内容不会出现在新内核的 dmesg 里**——无论上次是正常关机还是内核崩溃。
- 想让内核日志**跨重启幸存**，需要专门的机制：pstore（efi-pstore／ramoops，掉电后从 `/sys/fs/pstore` 读回）、netconsole（实时吐到远端主机）、kdump（崩溃瞬间把内存转储成 vmcore）。本机未见这些已产出证据。
- 因此跨重启可查的只剩三层：**用户态持久层**（journald 的 `/var/log/journal`、wtmp 的 `last`）、**崩溃转储层**（`/var/crash`）、**带外层**（云厂商面板/工单/串口控制台历史）。本次排查就是沿这三层展开——见命令手册。

## 4. 结局判读：上一 boot 尾部长什么样，结论就是什么

拿到 `journalctl -b -1 -n 100` 后对照：

| 上一 boot 日志的结尾形态 | 结论 | 进一步动作 |
|-|-|-|
| 一串 systemd `Stopping ...` ＋ `Reached target Reboot/Power-Off` ＋ `reboot: Restarting system` | 有人/有服务**主动** reboot | 查发起者：`journalctl -b -1 -u systemd-logind`、`journalctl -b -1 _COMM=sudo`（手册 §4） |
| `Out of memory`／`Kernel panic`／`Oops`／`BUG:`／`Call Trace`／MCE | **内核崩溃** | 保留现场：`/var/crash`、kdump、pstore（手册 §5）；按调用栈继续追 |
| 日志在正常业务输出中**戛然而止**，无任何关机收尾动作 | guest 没来得及反应 → **宿主机侧断电/强制重启/热迁移**（云主机常见于宿主维护） | `last -x` 里 reboot 无配对 shutdown 记录可佐证；去云厂商控制台查维护公告 |

结合证据 5（EXT4 无 recovery）：若结局属于第一行，与软证据互相印证——一次正常的人工/定时重启；若属于第三行，则软证据被推翻（强制断电下 jbd2 通常需要回放），以 journalctl 实际输出为准。

## 5. 下一步（待执行，拿到输出可继续追加到本目录）

```bash
journalctl --list-boots                 # 确认重启次数与时间区间
journalctl -b -1 -n 100 --no-pager      # ★ 上一 boot 最后 100 行
last -x shutdown reboot | head -20      # shutdown/reboot 记录是否配对
uptime -s                               # 佐证本次开机时刻 ≈ 10-17 19:54:52
ls /var/crash                           # 有无崩溃转储
```

## 附：材料清单

- [logs/dmesg-srv1572367-20261018.txt](./logs/dmesg-srv1572367-20261018.txt) —— 原始 `dmesg -T` 输出（743 行，未改动）
- [001.排查命令手册.md](./001.排查命令手册.md) —— 可复用的重启排查命令 runbook
