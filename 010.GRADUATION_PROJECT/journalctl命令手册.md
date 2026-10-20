# journalctl 命令手册：用法与排查场景

> 从 [001.内核重启排查](./001.内核重启排查/README.md) 与 [002.高负载D状态排查](./002.高负载D状态排查/README.md) 两个实战项目沉淀；选项依据 systemd 262，完整中文翻译见 [journalctl.man.中文版.md](./journalctl.man.中文版.md)，dmesg 侧见 [dmesg命令手册.md](./dmesg命令手册.md)。
> 一句话：**journalctl = 查询 journald 收集（并可落盘）的结构化日志**——内核 printk 只是它的来源之一。dmesg 手册第 1 节的四条边界，在 journalctl 这里分别有解法（见下表）。

## 1. 原理：journalctl 读的是什么

- **数据源**：systemd-journald 把三类输入统一收成带字段（`FIELD=VALUE`）的结构化条目——内核环形缓冲区（kmsg，即 dmesg 看到的那份）、用户进程的 syslog()/stdout/stderr、以及单元元数据（`_SYSTEMD_UNIT`、`_PID`、`_BOOT_ID`…）。
- **存放位置**：`/run/log/journal/`（易失，重启丢）与 `/var/log/journal/`（持久）。Ubuntu 的 journald.conf 默认 `Storage=auto`——**有 `/var/log/journal` 目录就持久化**。这是 `-b -1` 能查到上一次启动的前提（[001 手册](./001.内核重启排查/001.排查命令手册.md)第 2 节的补救项）。
- **权限**：普通用户只能看自己的用户 journal；`systemd-journal`/`adm`/`wheel` 组成员可读全部系统日志——不在组里的用户查系统日志要 sudo。
- **保留策略**：按容量/时间受限清理（`SystemMaxUse=` 等与 `--vacuum-*`），所以它没有 dmesg「环形覆盖」的突然丢失问题，但也不是无限保留。

| dmesg 的边界（dmesg 手册第 1 节） | journalctl 的对应解法 |
|-|-|
| 重启即失忆 | 持久化后 `-b -1` 直接看**上一次启动**（001 的关键一步） |
| 环形写满覆盖、旧消息被冲掉 | 落盘＋保留策略，按 `--since` 回溯历史 |
| 相对时间戳，对齐外部日志难 | `--since/--until` 直接吃墙钟时间（`2026-10-17 19:00:00`、`1 hour ago`、`today`） |
| 只覆盖内核 printk | 内核＋用户态全在一条时间线里，`-k`/`-u`/`-t` 分流（跨子系统排障才排得上完整证据链） |

代价：journal 里内核消息是 journald **收走后落盘**的副本——极端场景（journald 自己没起来、 panic 瞬间没落盘）会缺尾部；这正是 [001 手册](./001.内核重启排查/001.排查命令手册.md)第 7 节 netconsole/kdump/pstore 仍要部署的原因。

## 2. 选项速查（按使用频率分组）

```bash
# —— 启动与时间（跨重启回溯）——
journalctl --list-boots                # ★ 启动历史表：编号/BOOT_ID/起止时间（负数往回数）
journalctl -b -1 -n 100 --no-pager     # ★★ 上一次启动的最后 100 行（001 最关键的一条）
journalctl -b -1 -e                    # 等效跳末尾：-e 隐含 -n 1000，但显式 -b -1 优先生效
journalctl -b -1 -p err -n 50          # 只看上一次启动的 err 级及以上
journalctl --since "2026-10-08 13:40" --until "2026-10-08 14:20"
                                       # ★ 墙钟时间窗（也吃 "1 hour ago"/"today"/"yesterday"）
# —— 过滤 ——
journalctl -k                          # ★ 只看内核（_TRANSPORT=kernel；隐含 -b=0）
journalctl -p err                      # 级别：emerg(0)..debug(7)；单级别=该级及更重要；err..alert 区间
journalctl -u nginx.service            # ★ 按单元（自动附带该单元 coredump/systemd 相关消息；可多次）
journalctl -t sshd                     # 按 syslog 标识（可执行文件名）
journalctl -g 'not responding'         # PCRE2 正则搜 MESSAGE=（模式全小写=忽略大小写）
journalctl /usr/bin/dbus-daemon        # 可执行文件路径 → _EXE= 匹配
journalctl _PID=28097                  # 字段匹配：不同字段=AND，同字段=OR，独立单词 + =组间 OR
# —— 输出 ——
journalctl --no-pager                  # ★ 脚本/重定向必带（默认进 less 会卡住）
journalctl -o cat                      # 只留消息正文（配 -u 看服务日志最干净）
journalctl -o verbose                  # 显示条目全部字段（看 _BOOT_ID/_TRANSPORT 等元数据）
journalctl -o short-iso / -o short-monotonic / --utc    # 时区/ISO/单调时钟三种时间视角
journalctl -r                          # 反序：最新在前
journalctl -q                          # 去掉 "-- Journal begins/-- Reboot --" 等信息行
# —— 实时 ——
journalctl -f -u apache                # 类 tail -f，只跟踪该服务
journalctl -kf                         # 实时看内核消息（复现窗口用）
# —— 维护命令（诊断/治理，非查询）——
journalctl --disk-usage                # journal 磁盘占用
journalctl --vacuum-size=500M          # 清理已归档文件至 500M 以下（--vacuum-time=2week 同理）
journalctl --rotate                    # 轮转活跃文件（可与 vacuum 合并一条命令）
journalctl --sync / --flush            # 强制落盘 / 把 /run 的日志冲到 /var
journalctl --verify / --header         # 校验一致性 / 看 journal 文件头（排查乱序时间戳）
```

完整选项（含 `-I/--invocation=`、FSS 密封、`--namespace=` 等）见[中文 man 页](./journalctl.man.中文版.md)。

## 3. 判读

- 关键词表直接沿用 [dmesg 手册 2.1](./dmesg命令手册.md)：`panic/Oops/Call Trace/MCE/nfs not responding/blocked for more than...` 一套关键词两个工具通用；journalctl 侧加 `grep -iE` 前记得先 `--since` 限定时间窗，避免全量扫历史（[002 手册](./002.高负载D状态排查/001.排查命令手册.md)第 0 节铁律）。
- journalctl 特有输出：`-- Reboot --` 分隔线＝启动边界；`-- Journal begins at ...`＝journal 保留的最早时刻（再早的没了）。
- 输出到终端按优先级着色：红=ERROR 及以上、黄=WARNING、高亮=NOTICE、灰=DEBUG——`-p err` 之外肉眼可再校一遍。

## 4. 场景一：重启排查（来自 001）

```bash
journalctl --list-boots                       # ① 有几次启动、各自时间区间
journalctl -b -1 -n 100 --no-pager            # ② ★ 临终日志：上一次启动最后 100 行
journalctl -b -1 -e                           #    等效：分页器里直接跳末尾
journalctl -b -1 -p err -n 50                 #    只看 err 级以上
journalctl -b -1 --no-pager | grep -inE 'panic|oops|bug:|call trace|mce|hardware error|out of memory|general protection'
```

结局判读（正常重启 / 内核崩溃 / 戛然而止三分支）与「找 reboot 发起者」的命令（`-u systemd-logind`、`_COMM=sudo`、`-u unattended-upgrades`）见 [001 README](./001.内核重启排查/README.md) 第 4 节与 [001 手册](./001.内核重启排查/001.排查命令手册.md)第 3、4 节。注意 `_COMM=sudo` 就是字段匹配语法的实战用法。

## 5. 场景二：时间窗定责（来自 002）

```bash
journalctl -k --since "2026-10-08 13:40:00" --until "2026-10-08 14:20:00" --no-pager
                                              # ★ -k 只看内核 + 墙钟时间窗覆盖告警前几分钟
journalctl -k --since "13:40" | grep -iE 'not responding|timed out|I/O error|reset|blocked for more than'
```

要点：内核日志只回答「内核看到了什么」，D 状态定位仍靠 `ps -eLo wchan` + `/proc/<TID>/stack`（[002 手册](./002.高负载D状态排查/001.排查命令手册.md)第 3、4 节）；证据链要配健康节点对照与变更记录（同手册第 5 节）。

## 6. 场景三：日常与服务级排查

```bash
journalctl -u myapp -f                        # 跟踪单个服务的实时日志（部署验证）
journalctl -u myapp -b                        # 本次开机以来该服务的全部日志
journalctl -p err --since today               # 今天的所有 err 级以上（晨检）
journalctl -g 'TODO|FIXME' -t myapp           # 正则在 MESSAGE= 上过滤
journalctl -u myapp --since "1 hour ago" -o cat --no-pager > app.log
                                              # 导出成纯文本给工单/他人分析
journalctl --user -u pipewire.service         # 用户级服务（需日志持久化才可用）
journalctl -b -1 -k                           # 上一次启动的内核日志（dmesg 做不到的）
```

磁盘治理：`--disk-usage` 看占用 → `--vacuum-size=`/`--vacuum-time=` 清理（只作用于**已归档**文件；配 `--rotate` 先归档再清理效果最大）。长期上限应配 `journald.conf` 的 `SystemMaxUse=` 而非手动 vacuum。

## 7. 常见坑

| 症状 | 原因 | 处置 |
|-|-|-|
| `--list-boots` 只有一条、`-b -1` 报无匹配 | journal 未持久化（无 `/var/log/journal`） | `/etc/systemd/journald.conf` 设 `Storage=persistent` 后 `systemctl restart systemd-journald`；本次已丢的日志补不回来 |
| 普通用户看不到系统日志、输出为空 | 不在 `systemd-journal`/`adm`/`wheel` 组 | `sudo`；或 `usermod -aG systemd-journal` 后重新登录 |
| 脚本里 `journalctl` 卡住不动 | 默认进 `less` 分页 | 一律加 `--no-pager`（或 `PAGER=cat`） |
| `-e` 之后「只有」1000 行 | `-e` 隐含 `--lines=1000` | 显式给 `-n`；001 手册里用 `-b -1 -e` 时显式 `-b -1` 已生效 |
| `-k` 想看上次的内核日志却是本次 | `-k` 隐含 `--boot=0` | 显式 `journalctl -k -b -1` |
| `-g 'Error'` 匹配不到小写 error | 模式含大写=区分大小写 | 用全小写模式（自动忽略大小写）或 `--case-sensitive=false` |
| 时间戳与外部系统对不齐 | 默认本地时区 | `--utc` 或 `-o short-iso`；跨机对比统一用 ISO |
| journal 里没有 panic 尾部 | journald 来不及落盘 | 这不是 journalctl 的锅——部署 kdump/netconsole（001 手册第 7 节） |
