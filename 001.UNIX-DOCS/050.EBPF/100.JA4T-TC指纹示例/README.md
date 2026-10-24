> 📌 **迁移说明**(2026-10-24):本目录迁移自 `Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN`(原项目保留不动),作为 [050.EBPF](../README.md) 的 **TC 分类器实战示例**。迁移范围 = **完整项目**(eBPF C 程序、Rust daemon、Cargo 构建脚本、文档、运行日志),build.rs 使用 crate 相对路径,`cargo build --release` 可原地构建(需 clang ≥12、Linux ≥5.3)。导读与 050 笔记对照见 [000.导读.md](000.导读.md)。

# JA4T Daemon — eBPF TCP + TLS 指纹采集守护进程

> 最后更新: 2026-07-09
>
> 基于 eBPF TC 分类器从 TCP SYN 和 TLS Server Hello 包中提取 JA4T/JA4S 指纹原始参数，
> 通过 Unix Domain Socket 暴露给 Nginx 读取，为 CYBERCLOAK 斗篷系统提供
> **L3/L4/L6 跨层不可伪造信号**。
>
> **同时采集 JA4T（TCP SYN 指纹）+ JA4S（TLS Server Hello 指纹）**，
> 两个 eBPF 程序独立运行于同一 daemon 进程中，分别挂载 TC Ingress 和 TC Egress。

---

## 一、架构总览

```
┌─────────────────────────────────────────────────────────────────────┐
│                      Linux Kernel Space                              │
│                                                                     │
│  网卡 eth0                                                          │
│    │                                                                │
│    ├─ TC Ingress (入站)                                              │
│    │   └─ tcp_options.bpf.c (423行)                                  │
│    │       ├─ p0f 风格 IP 头扫描 (offset 0..38, step 2)             │
│    │       ├─ SYN 包: window_size + raw_opts[40] + ip_ttl            │
│    │       ├─ SYN-ACK 包: 反向 5-tuple 匹配 → synack_ns (RTT)       │
│    │       └─ 写入 tcp_options_map (LRU Hash, 65,536 条, 64B/value)  │
│    │                                                                │
│    └─ TC Egress (出站)                                               │
│        └─ tls_server_hello.bpf.c (422行)                            │
│            ├─ Server Hello 识别 (src_port=443, TLS 0x16, HS 0x02)    │
│            ├─ 提取: tls_version + cipher_suite + extensions + ALPN   │
│            └─ 写入 tls_server_hello_map (LRU Hash, 65,536 条, 176B)  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                     User Space — ja4t-daemon                         │
│                                                                     │
│  main.rs (94行) — 启动入口，加载 eBPF + 启动 poll 任务 + Socket 服务 │
│    ├─ ja4t/loader.rs: 加载 TC Ingress eBPF 程序                      │
│    ├─ ja4t/poller.rs: 每 5s dump tcp_options_map → ConnectionCache   │
│    ├─ ja4s/loader.rs: 加载 TC Egress eBPF 程序                      │
│    ├─ ja4s/poller.rs: 每 5s dump tls_server_hello_map → ServerHelloCache│
│    └─ server.rs (286行): Unix Socket 服务器 (tokio per-client)       │
│                                                                     │
│  模块化子目录:                                                       │
│    ja4s/ (7 files, 752行) — JA4S TLS Server Hello 指纹计算          │
│      ├─ mod.rs          (68行)  公共 API 重导出                      │
│      ├─ compute.rs      (231行) compute_ja4s() 8步算法               │
│      ├─ cache.rs        (116行) ServerHelloCache 内存缓存            │
│      ├─ constants.rs    (18行)  GREASE 表 (RFC 8701, 16个值)         │
│      ├─ types.rs        (102行) JA4S 数据结构                        │
│      ├─ reader.rs       (57行)  BPF Map 读取器                        │
│      ├─ loader.rs       (52行)  eBPF 程序加载与 TC Egress 挂载       │
│      └─ poller.rs       (37行)  后台轮询 BPF Map                      │
│                                                                     │
│    ja4t/ (6 files, 362行) — JA4T TCP SYN 指纹采集                   │
│      ├─ mod.rs          (21行)  模块入口                              │
│      ├─ types.rs        (49行)  BPF Map C 兼容结构体                  │
│      ├─ options.rs      (91行)  TCP Option 解析器 (p0f 风格)         │
│      ├─ reader.rs       (80行)  BPF Map 读取器                        │
│      ├─ cache.rs        (70行)  ConnectionCache 内存缓存              │
│      ├─ loader.rs       (52行)  eBPF 程序加载与 TC Ingress 挂载       │
│      └─ poller.rs       (80行)  后台轮询任务                           │
│                                                                     │
│  protocol.rs (106行) — LookupRequest/LookupResponse JSON 协议定义     │
│  cli.rs     (32行)  — CLI 参数解析 (clap derive)                     │
│                                                                     │
│  两级查找架构:                                                       │
│    快路径: 内存 HashMap (~微秒) → 回退路径: BPF Map 直查 (~百微秒)   │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      Nginx (OpenResty)                               │
│                                                                     │
│  access_by_lua_block: 查询 /var/run/ja4t.sock → JSON 响应            │
│  proxy_set_header 注入:                                              │
│    X-JA4T-Fingerprint, X-JA4T-TTL, X-JA4T-RTT, X-JA4S-Fingerprint   │
│  缓存: ja4t_cache (32m lua_shared_dict), found=true 缓存 600s        │
└─────────────────────────────────────────────────────────────────────┘
```

### 组件职责

| 组件 | 空间 | 代码位置 | 职责 |
|------|------|---------|------|
| **eBPF TC Ingress** | Kernel | `ebpf/tcp_options.bpf.c` (423行) | 拦截入站 TCP SYN/SYN-ACK，提取窗口大小 + TCP 选项 + IP TTL + RTT |
| **eBPF TC Egress** | Kernel | `ebpf/tls_server_hello.bpf.c` (422行) | 拦截出站 TLS Server Hello，提取版本 + 密码套件 + 扩展 + ALPN |
| **TCP 选项解析** | User | `ja4t/options.rs` (91行) | 纯 Rust p0f 风格解析：MSS / Window Scale / SACK / Option Order |
| **JA4S 计算** | User | `ja4s/compute.rs` (231行) | SHA256 扩展 hash，用户态计算（避免 eBPF 指令限制） |
| **BpfReader** | User | `ja4t/reader.rs` + `ja4s/reader.rs` | 封装 Aya BPF Map 访问：dump_all() 全量 + lookup_direct() 单 key |
| **ConnectionCache** | User | `ja4t/cache.rs` (70行) | 内存 HashMap 缓存 TCP 指纹，poll 任务每 5s 全量刷新 |
| **ServerHelloCache** | User | `ja4s/cache.rs` (116行) | 内存 HashMap 缓存 TLS 指纹，poll 任务每 5s 全量刷新 |
| **Unix Socket Server** | User | `server.rs` (286行) | 监听 `/var/run/ja4t.sock`，tokio per-client spawn，换行分隔 JSON |
| **Cli** | User | `cli.rs` (32行) | CLI 参数解析（interface, socket, poll-interval, bpf-object） |
| **Protocol** | User | `protocol.rs` (106行) | LookupRequest/LookupResponse 结构体定义与 JSON 序列化 |
| **Nginx Lua Client** | Nginx | `cc_manage.nginx-v2.0.0.conf` | cosocket 连接 Unix Socket，构建 JA4T/JA4S 指纹字符串并注入 Header |

---

## 二、文件结构与行数

```
003.JA4T_PLUGIN/
├── build.rs                      # Cargo build 脚本 (编译 2 个 eBPF C 程序)
├── Cargo.toml                    # Rust 项目配置
├── src/
│   ├── main.rs                   # (94行)  daemon 启动入口
│   ├── server.rs                 # (286行) Unix Socket 服务器
│   ├── protocol.rs               # (106行) JSON 协议定义
│   ├── cli.rs                    # (32行)  CLI 参数解析
│   ├── ja4s/                     # JA4S TLS Server Hello 指纹模块
│   │   ├── mod.rs                # (68行)  模块入口 + 公共 API
│   │   ├── compute.rs            # (231行) compute_ja4s() 算法实现
│   │   ├── cache.rs              # (116行) ServerHelloCache 缓存
│   │   ├── constants.rs          # (18行)  GREASE 常量表
│   │   ├── types.rs              # (102行) 数据结构定义
│   │   ├── reader.rs             # (57行)  BPF Map 读取
│   │   ├── loader.rs             # (52行)  eBPF 加载 + TC Egress 挂载
│   │   └── poller.rs             # (37行)  后台轮询任务
│   ├── ja4t/                     # JA4T TCP SYN 指纹模块
│   │   ├── mod.rs                # (21行)  模块入口
│   │   ├── types.rs              # (49行)  BPF Map 结构体
│   │   ├── options.rs            # (91行)  TCP Option 解析器
│   │   ├── reader.rs             # (80行)  BPF Map 读取
│   │   ├── cache.rs              # (70行)  ConnectionCache 缓存
│   │   ├── loader.rs             # (52行)  eBPF 加载 + TC Ingress 挂载
│   │   └── poller.rs             # (80行)  后台轮询任务
│   └── ebpf/                     # eBPF C 程序 (编译为 BPF 目标文件)
│       ├── tcp_options.h         # (73行)  共享结构体 C header (4 结构体)
│       ├── tcp_options.bpf.c     # (423行) eBPF TC Ingress — SYN + SYN-ACK RTT
│       └── tls_server_hello.bpf.c # (422行) eBPF TC Egress — TLS Server Hello
├── docs/                         # 参考文档
│   ├── IMPLEMENTATION.md         # 实现细节说明
│   ├── JA4T_DATA_FLOW.md         # 数据流图 + 时序图
│   └── JA4L-JA4S-Integration.md  # JA4L+JA4S 集成与部署文档
└── README.md
```

**总代码量**: ~2,560 行（不含注释和文档）

---

## 三、编译

### 依赖

| 工具 | 版本 | 说明 |
|------|------|------|
| Rust | 1.70+ | 用户态程序编译 |
| clang | 12+ | eBPF C 程序编译 (`-target bpf`) |
| Linux Kernel | 5.3+ | BPF LRU hash map + bounded loops |

### Rust 依赖 (Cargo.toml)

| 依赖 | 版本 | 用途 |
|------|------|------|
| `aya` | 0.12 | eBPF 程序加载和 Map 操作 |
| `tokio` | 1 (full) | 异步运行时 (Unix Socket + 轮询) |
| `clap` | 4 (derive) | CLI 参数解析 |
| `serde` / `serde_json` | 1 | JSON 序列化 (Socket 协议) |
| `sha2` | 0.10 | SHA256 (JA4S 扩展 hash) |
| `hex` | 0.4 | Hex 编码 (JA4S 密码套件) |
| `log` / `env_logger` | 0.4 / 0.11 | 日志 |
| `anyhow` | 1 | 错误处理 |

### 编译命令

```bash
cd 000.CODE_SPACE/003.JA4T_PLUGIN

# 完整编译 (build.rs 自动编译 2 个 .bpf.c → .bpf.o)
cargo build --release

# 产物:
#   二进制:    target/release/ja4t-daemon
#   BPF 对象:  target/release/build/JA4T_PLUGIN-*/out/tcp_options.bpf.o   (JA4T TCP SYN)
#   BPF 对象:  target/release/build/JA4T_PLUGIN-*/out/tls_server_hello.bpf.o (JA4S TLS)
```

手动编译 eBPF（跳过 build.rs）：

```bash
# TCP SYN fingerprint
clang -target bpf -O2 -g -Wall \
    -I src/ebpf \
    -c src/ebpf/tcp_options.bpf.c \
    -o /opt/ja4t/tcp_options.bpf.o

# TLS Server Hello fingerprint
clang -target bpf -O2 -g -Wall \
    -I src/ebpf \
    -c src/ebpf/tls_server_hello.bpf.c \
    -o /opt/ja4t/tls_server_hello.bpf.o

cargo build --release
```

---

## 四、部署

### 4.1 环境要求

- Linux Kernel ≥ 5.3（BPF LRU hash map + bounded loops）
- `CAP_BPF` + `CAP_NET_ADMIN` 权限（eBPF 加载 + TC 挂载）
- TC (traffic control) clsact qdisc 支持
- Nginx / OpenResty（含 lua-nginx-module）

### 4.2 创建 TC clsact qdisc

在内核调用 TC 分类器之前，必须先挂载 clsact qdisc：

```bash
# 添加 clsact qdisc (只需执行一次，重启后需重新添加)
sudo tc qdisc add dev eth0 clsact

# 验证
sudo tc qdisc show dev eth0
# 输出应包含: qdisc clsact ...
```

> **注意**: 只需一个 clsact qdisc，同时挂载 ingress（JA4T）和 egress（JA4S）两个 BPF 程序。

### 4.3 启动 ja4t-daemon

daemon 自动加载两个 eBPF 程序：
- **TC Ingress** → `tcp_options.bpf.o` (JA4T TCP SYN 指纹)
- **TC Egress** → `tls_server_hello.bpf.o` (JA4S TLS Server Hello 指纹)

```bash
# === 前台运行（调试用）===

# 基础启动（自动加载 2 个 eBPF TC 程序: ingress + egress）
sudo ./ja4t-daemon -i eth0

# 详细日志
sudo ./ja4t-daemon -i eth0 -v

# 自定义参数（分别指定两个 BPF 对象文件）
sudo ./ja4t-daemon \
    -i eth0 \
    --bpf-object /opt/ja4t/tcp_options.bpf.o \
    --bpf-server-hello-object /opt/ja4t/tls_server_hello.bpf.o \
    -s /var/run/ja4t.sock \
    --poll-interval 5

# 省略 --bpf-server-hello-object 时，daemon 自动在 build 输出目录中查找

# 
sudo tc qdisc add dev wlx9c47829dc963 clsact
sudo tc qdisc show dev wlx9c47829dc963
sudo ./target/release/ja4t-daemon   -i wlx9c47829dc963  --bpf-object /home/wei/OPEN_SOURCE/Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/target/release/build/JA4T_PLUGIN-0cde1c0f8780252c/out/tcp_options.bpf.o  --bpf-server-hello-object /home/wei/OPEN_SOURCE/Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/target/release/build/JA4T_PLUGIN-0cde1c0f8780252c/out/tls_server_hello.bpf.o  -s /var/run/ja4t.sock  --poll-interval 5

# === 后台运行（生产环境）===

# 无日志后台运行（避免 nohup.out 磁盘膨胀）
nohup sudo ./ja4t-daemon \
    -i eth0 \
    --bpf-object /opt/ja4t/tcp_options.bpf.o \
    --bpf-server-hello-object /opt/ja4t/tls_server_hello.bpf.o \
    -s /var/run/ja4t.sock \
    --poll-interval 5 \
    > /dev/null 2>&1 &

# 有日志后台运行（需配合 logrotate）
nohup sudo ./ja4t-daemon \
    -i eth0 \
    --bpf-object /opt/ja4t/tcp_options.bpf.o \
    --bpf-server-hello-object /opt/ja4t/tls_server_hello.bpf.o \
    -s /var/run/ja4t.sock \
    --poll-interval 5 \
    > /var/log/ja4t.log 2>&1 &
```

### 4.4 Systemd 服务（推荐）

```ini
# /etc/systemd/system/ja4t-daemon.service
[Unit]
Description=JA4T+JA4S eBPF TCP/TLS Fingerprint Daemon
After=network.target
Before=nginx.service

[Service]
Type=simple
# 自动创建 clsact qdisc（如已存在则跳过）
ExecStartPre=/usr/sbin/tc qdisc add dev eth0 clsact 2>/dev/null || true
ExecStart=/opt/ja4t/ja4t-daemon \
    -i eth0 \
    --bpf-object /opt/ja4t/tcp_options.bpf.o \
    --bpf-server-hello-object /opt/ja4t/tls_server_hello.bpf.o \
    -s /var/run/ja4t.sock \
    --poll-interval 5
Restart=always
RestartSec=5
AmbientCapabilities=CAP_BPF CAP_NET_ADMIN

# 确保 daemon 早于 nginx 启动（减少冷启动 not found）
# nginx.service 中应设置 After=ja4t-daemon.service

[Install]
WantedBy=multi-user.target
```

启用服务：

```bash
sudo systemctl daemon-reload
sudo systemctl enable ja4t-daemon
sudo systemctl start ja4t-daemon
sudo systemctl status ja4t-daemon
```

### 4.5 调优建议

| 参数 | 默认值 | 建议 | 说明 |
|------|--------|------|------|
| `--poll-interval` | `5` (秒) | `1-2` (生产) | 更小的间隔减少 Nginx 查询竞态窗口 |
| `--interface` | `eth0` | 实际网卡名 | 多网卡场景需为每网卡启动独立实例 |
| `--socket` | `/var/run/ja4t.sock` | 保持默认 | 需与 Nginx 配置一致 |
| `--bpf-object` | 自动检测 | 可省略 | JA4T TCP SYN eBPF 目标文件路径 |
| `--bpf-server-hello-object` | 自动检测 | 可省略 | JA4S TLS Server Hello eBPF 目标文件路径 |

**竞态窗口说明**: TCP SYN 到达后 BPF 立即写入 Map，但 daemon 在下一个 poll 周期才读到。HTTP 请求可能在 poll 前到达导致 `not found`。缩小 poll-interval 可大幅降低此窗口。

### 4.6 验证部署

```bash
# 1. 检查 Unix Socket
ls -la /var/run/ja4t.sock

# 2. 检查进程
ps aux | grep ja4t

# 3. 检查 TC filter（应看到 2 个 bpf 程序: ingress JA4T + egress JA4S）
sudo tc filter show dev eth0 ingress
# 输出示例:
#   filter protocol all pref 49152 bpf chain 0
#   filter protocol all pref 49152 bpf chain 0 handle 0x1 ja4t_parse_syn:[...] direct-action

sudo tc filter show dev eth0 egress
# 输出示例:
#   filter protocol all pref 49152 bpf chain 0
#   filter protocol all pref 49152 bpf chain 0 handle 0x1 ja4s_parse_server_hello:[...] direct-action

# 4. 检查 BPF Map（应看到 2 个 map: tcp_options_map + tls_server_hello_map）
sudo bpftool map list | grep -E 'tcp_options|tls_server_hello'

# 5. 查看 Map 条目
sudo bpftool map dump name tcp_options_map | head -10
sudo bpftool map dump name tls_server_hello_map | head -10

# 6. 手动查询测试（同时返回 JA4T 和 JA4S 数据）
echo '{"src_ip":"192.168.1.100","dst_ip":"10.0.0.1","src_port":54321,"dst_port":443}' \
    | nc -U /var/run/ja4t.sock
# 响应包含: found, window_size, mss, ..., ja4s_fingerprint
```

---

## 五、eBPF 实现细节

### 5.1 tcp_options.bpf.c — TC Ingress (423行)

**数据包定位策略（参考 p0f）**:

不依赖 Ethernet EtherType，采用 p0f `find_offset()` 方法：
- 在偏移量 0–38（步长 2）扫描 IP 版本号（首字节高 4 位 == 4）
- 验证偏移 +9 处协议字段 == TCP (6)
- 兼容 USB WiFi 驱动的非标准链路层偏移（radiotap、prism 等）

**处理两类包**:

| 包类型 | 条件 | 操作 |
|--------|------|------|
| 纯 SYN | SYN=1, ACK=0 | 提取 TCP 指纹 + TTL → 写入 `tcp_options_map` |
| SYN-ACK | SYN=1, ACK=1 | 反向 5-tuple 匹配已有 SYN → 更新 `synack_ns` (RTT) |

**采集字段**:

| 字段 | 来源 | 大小 | 说明 |
|------|------|------|------|
| window_size | TCP header | 2B | 窗口大小（JA4T 指纹输入） |
| raw_opts | TCP options | 40B | 原始选项字节 |
| raw_opts_len | TCP options | 1B | 有效长度 |
| ip_ttl | IP header | 1B | IP TTL（OS 推断用） |
| capture_ns | bpf_ktime_get_ns() | 8B | SYN 捕获时间戳 |
| synack_ns | bpf_ktime_get_ns() | 8B | SYN-ACK 到达时间戳（0=未捕获） |

**BPF Map**:

| Map | 类型 | 容量 | Key | Value |
|-----|------|------|-----|-------|
| `tcp_options_map` | LRU Hash | 65,536 | `connection_tuple` (16B) | `tcp_options_info` (64B) |
| `debug_counters` | Array | 10 | u32 index | u64 counter |

**调试计数器**:

| 索引 | 含义 | 索引 | 含义 |
|------|------|------|------|
| 0 | 总包数 | 5 | IP 头偏移（诊断） |
| 1 | IPv4 包 | 6 | 协议字节（诊断） |
| 2 | TCP 包 | 7 | TCP 头偏移（诊断） |
| 3 | 纯 SYN | 8 | SYN-ACK 捕获数 |
| 4 | 写入成功 | 9 | SYN-ACK 匹配成功 |

**过滤条件**: 仅 IPv4 + TCP；跳过 IP 分片包 (`frag_off & 0x3FFF != 0`)；数据偏移 >= 5

### 5.2 tls_server_hello.bpf.c — TC Egress (422行)

挂载到网卡 **TC Egress** 路径，捕获服务器发往客户端的 TLS Server Hello 包。

**包识别**:

| 步骤 | 条件 | 说明 |
|------|------|------|
| 1 | src_port = 443 | 服务器 TLS 端口 |
| 2 | TLS Record Type = 0x16 | Handshake |
| 3 | Handshake Type = 0x02 | Server Hello |

**采集字段 (tls_server_hello_info, 176B)**:

| 字段 | 大小 | 说明 |
|------|------|------|
| tls_version | 2B | 0x0304=TLS1.3, 0x0303=TLS1.2 |
| cipher_suite | 2B | 服务器选择的密码套件 (单个) |
| extension_count | 1B | 含 GREASE |
| alpn_len + alpn | 1+32B | 选择的 ALPN 协议名 |
| ext_raw_len + ext_raw | 1+120B | 扩展 type 原始字节（原始顺序，含 GREASE） |
| capture_ns | 8B | 捕获时间戳 |

**调试计数器 (8 个)**:

| 索引 | 含义 | 索引 | 含义 |
|------|------|------|------|
| 0 | 总包数 | 4 | 成功写入 Map |
| 1 | TCP 443 包 | 5 | 含 ALPN 的 Server Hello |
| 2 | TLS Handshake | 6 | 扩展数溢出 |
| 3 | Server Hello | 7 | 跳过的分片包 |

### 5.3 共享头文件 (tcp_options.h, 73行)

```c
#define MAX_TCP_OPTIONS 12   // 最多解析 12 种 TCP Option
#define MAX_RAW_OPTS 40       // 原始 Option 字节最大长度
#define MAX_ALPN_LEN 32       // ALPN 协议名最大长度
#define MAX_EXT_RAW_BYTES 120 // 扩展 type 原始字节最大长度
```

**结构体布局**:

```
connection_tuple (16B):  src_ip(4) + dst_ip(4) + src_port(2) + dst_port(2)
                          + protocol(1) + pad(3)

tcp_options_info (64B):   window_size(2) + raw_opts_len(1) + ip_ttl(1)
                          + raw_opts[40] + _pad2[4]
                          + capture_ns(8) + synack_ns(8)

ja4s_connection_key(16B): 同 connection_tuple

tls_server_hello_info(176B): tls_version(2) + cipher_suite(2)
                              + extension_count(1) + alpn_len(1)
                              + alpn[32] + ext_raw_len(1) + ext_raw[120]
                              + _pad[9] + capture_ns(8)
```

### 5.4 用户态 TCP 选项解析 (ja4t/options.rs)

参考 p0f 的 `parse_packet()`：

| Kind | 名称 | 长度 | 解析方式 |
|------|------|------|---------|
| 0 | EOL | 1 | 终止解析 |
| 1 | NOP | 1 | 记录到 order, idx+=1 |
| 2 | MSS | 4 | 提取 2 字节 MSS 值 |
| 3 | Window Scale | 3 | 提取 1 字节缩放因子 |
| 4 | SACK Permitted | 2 | 记录存在 (sack=1) |
| 8 | Timestamp | 10 | 记录到 order（不解析值） |
| 其他 | — | var | 记录到 order |

最多解析 12 种 Option（匹配 p0f 行为）。

**平台特征 TCP Option Order**:

| 平台 | Option Order | 说明 |
|------|-------------|------|
| Linux (含 Android) | 2-4-8-1-3 | mss, sack, ts, nop, ws |
| Windows 7+ | 2-1-3-4-8 | mss, nop, ws, sack, ts |
| Apple (macOS/iOS) | 2-1-3-1-1-8-4[-0] | mss, nop, ws, nop, nop, ts, sack[, eol] |

---

## 六、JA4S 计算模块 (ja4s/)

### 6.1 JA4S 指纹格式

```
t{ver}{#exts}{alpn}_{cipher}_{ext_hash}

示例: "t1303h2_1301_abc123def456"
      ││││││ │──│ ──────────
      ││││││   │     └─ SHA256(扩展type原始顺序)[:12]
      ││││││   └─────── 密码套件 hex lowercase (单个)
      ││││└┴──────────── ALPN 首尾字母数字 (无则 "00")
      │││└────────────── 扩展数量 2 位 (含 GREASE)
      │└└─────────────── TLS 版本 ("13", "12" 等)
      └──────────────── Protocol prefix 't'
```

### 6.2 JA4 vs JA4S 算法差异

| 维度 | JA4 (客户端) | JA4S (服务器) |
|------|-------------|----------------|
| 密码套件 | 列表排序后 hash | **单个 cipher hex** |
| Extension 计数 | 排除 GREASE | **包含 GREASE** |
| Extension hash | 排序, 排除 SNI+ALPN, 排除 GREASE | **原始顺序, 包含 GREASE** |
| SNI 标记 | d/i | 无 |
| 签名算法 | 包含在 Part C | 无 |

### 6.3 compute_ja4s() 8 步算法

1. Protocol prefix = "t"
2. TLS version mapping (`TLS_MAPPER`: 0x0304→"13", 0x0303→"12")
3. Extension count: `min(count, 99)` 2 位数（含 GREASE）
4. ALPN: 第一个 ALPN 值的首尾字母数字字符，无则 "00"
5. Part A = `"t" + ver + #exts + alpn`
6. Part B = cipher hex lowercase（无 "0x" 前缀）
7. Part C = SHA256(extension type 原始顺序逗号拼接) [:12]
8. JA4S = PartA + "_" + PartB + "_" + PartC

### 6.4 GREASE 表 (RFC 8701, 16 个值)

```
0x0a0a, 0x1a1a, 0x2a2a, 0x3a3a, 0x4a4a, 0x5a5a, 0x6a6a, 0x7a7a,
0x8a8a, 0x9a9a, 0xaaaa, 0xbaba, 0xcaca, 0xdada, 0xeaea, 0xfafa
```

---

## 七、数据流

### 7.1 端到端流程

```
TCP SYN → eBPF TC Ingress (tcp_options.bpf.c)
            │  写入 tcp_options_map (capture_ns)
            ▼
          eBPF TC Ingress (同一 BPF)
            │  SYN-ACK: 反向匹配 → synack_ns (RTT = synack_ns - capture_ns)
            ▼
          ja4t-daemon Poll Task (每 5s)
            │  dump_all → parse_tcp_options → ConnectionCache
            ▼
          eBPF TC Egress (tls_server_hello.bpf.c)
            │  Server Hello → tls_server_hello_map
            ▼
          ja4t-daemon JA4S Poll Task (每 5s)
            │  dump_all → compute_ja4s → ServerHelloCache
            ▼
          Unix Socket /var/run/ja4t.sock
            │  JSON 查询: {src_ip, dst_ip, src_port, dst_port}
            │  JSON 响应: {found, mss, ws, sack, opts, ip_ttl, rtt_us, ja4s, ...}
            ▼
          Nginx Lua (access_by_lua_block)
            │  注入 Headers:
            │    X-JA4T-Fingerprint, X-JA4T-TTL, X-JA4T-RTT, X-JA4S-Fingerprint
            ▼
          Rust Backend (23+ Handler 验证链)
            │  Ja4FingerprintHandler → Ja4LHandler → Ja4SHandler → ...
            ▼
          Decision: ALLOW / BLOCK
```

### 7.2 两级查找架构

```
Nginx Worker → Unix Socket Server
                  │
                  ├─ 快路径: ConnectionCache.lookup(key) → ~微秒
                  │    (内存 HashMap, poll 任务每 5s 全量刷新)
                  │
                  └─ 回退路径: BpfReader::lookup_direct(key) → ~百微秒
                       (直接调用 bpf_map_lookup_elem 系统调用)
```

缓存 key 格式: `"IP:Port->IP:Port"` (例: `"192.168.1.1:54321->10.0.0.1:443"`)

### 7.3 查询时序

```
场景 A: 缓存命中 (正常路径, ~微秒)
  SYN → BPF Map → Poll(5s) → ConnectionCache → Nginx 查询 → 命中 ✅

场景 B: 缓存未命中 (竞态窗口, ~100微秒)
  SYN → BPF Map → (尚未 poll) → Nginx 查询 → 缓存 miss → BPF 直查 → 命中 ✅

场景 C: 连接不存在
  Nginx 查询 → 缓存 miss → BPF 直查 → miss → {"found": false}
```

---

## 八、JA4+ 信号层次与爬虫检测

### 8.1 信号层次模型

爬虫可以伪造单个信号，但很难同时伪造**跨协议层的多个信号**且保持逻辑一致。

```
L7 应用层  ├── JA4H (HTTP 头)        ← 可伪造，但 Headless 浏览器头特征不同
           ├── Browser Fingerprint    ← Headless 暴露 canvas/WebGL/font 差异
           └── Cookie 一致性          ← 会话内 Cookie 结构异常

L6 表示层  ├── JA4 (TLS Client Hello) ← 可伪造，但参数组合暴露底层 TLS 库
           └── JA4S (TLS Server Hello) ← 服务器动态响应，客户端不能直接控制

L4 传输层  ├── JA4T (TCP SYN)        ← OS 内核 netcode，用户态无法修改
           └── MSS (网络路径特征)      ← 反射 VPN/代理/运营商

L3 网络层  ├── JA4L_b (TTL)           ← 中间网络设备逐跳递减，客户端无法控制
           └── JA4L_C (RTT 延迟)       ← 光速约束，无法伪造更短的距离
```

### 8.2 各信号爬虫检测价值

| 指纹 | 协议层 | 爬虫检测价值 | 伪造难度 | 状态 |
|------|--------|------------|---------|------|
| **JA4T** | L4 TCP | OS/平台识别，TCP 栈与 UA 矛盾则暴露 | **极高** (内核 netcode) | ✅ |
| **JA4L_b (TTL)** | L3 IP | TTL 推断 OS + 跳数，与 JA4T 交叉验证 | **极高** (网络路径) | ✅ |
| **JA4L_C (RTT)** | L3 时序 | 物理距离上界，光速限制 | **极高** (物理定律) | ✅ |
| **JA4S** | L6 TLS | 客户端 JA4 + 服务器 JA4S 配对唯一性 | **高** (服务器动态选择) | ✅ |
| **MSS 语义** | L4 TCP | VPN/代理/运营商识别 | **高** (网络路径) | ✅ |
| **JA4** | L6 TLS | 客户端 TLS 库识别，非浏览器 TLS 直接暴露 | 中 | ✅ |
| **JA4H** | L7 HTTP | HTTP 方法/头/Cookie 结构 | 低 | ✅ |

### 8.3 攻击者伪造能力矩阵

| 攻击者类型 | JA4 | JA4H | JA4T | JA4L | JA4S | BrowserFP |
|-----------|-----|------|------|------|------|-----------|
| curl/wget 脚本 | 易 | 易 | **难** | **难** | **难** | **无** |
| Python requests | 易 | 易 | **难** | **难** | **难** | **无** |
| Go HTTP Client | 易 | 易 | **难** | **难** | **难** | **无** |
| Headless Chrome | **难** | **难** | **难*** | 中 | **难** | 中 |
| HC + Proxy/VPN | **难** | **难** | **难*** | 中 | **难** | 中 |
| Playwright Stealth | **难** | **难** | **难*** | 中 | **难** | **难** |
| Google Ads 审核 | **难** | **难** | **难*** | 中 | **难** | **难** |

\* Headless Chrome 的 TCP 栈为 Linux，如果 UA 声称其他 OS 则矛盾

### 8.4 关键结论

1. **没有单一信号能覆盖所有攻击者** — 脚本类在 L6/L7 暴露，Headless 浏览器在 L3/L4 可能留下痕迹
2. **L3/L4 组合 (JA4T + JA4L + MSS) 对脚本类攻击者覆盖最好**
3. **JA4S 对高级伪装者有独特价值** — 服务器侧信号，客户端不能直接控制
4. **Cookie 稳定性检测不作为独立拦截信号** — 必须与其他信号联合
5. **Google Ads 审核流量特征**: 真实 Chrome + Google ASN + 美国 IP → 不应用激进 L3/L4 规则

---

## 九、JA4T 指纹格式

```
{window_size}_{mss}_{window_scale}_{sack_permitted}_{options_order}

示例: 42340_1460_10_1_2-4-8-1-3
      ───── ──── ── ─ ─────────
        │     │    │  │     │
        │     │    │  │     └─ TCP 选项类型序列 (按 SYN 包中出现的顺序, 用 - 连接)
        │     │    │  └─────── SACK Permitted (0=不支持, 1=支持)
        │     │    └────────── Window Scale 值 (0-14)
        │     └─────────────── MSS 值 (通常 536-1460)
        └───────────────────── TCP 窗口大小 (原始值, 不做 scale 换算)
```

Options 编码: 1=NOP, 2=MSS, 3=Window Scale, 4=SACK Permitted, 5=SACK, 8=Timestamp
> SACK Permitted (kind=4) 出现在 SYN 包中作为能力声明；SACK (kind=5) 只在 ACK 包中出现，不会出现在 JA4T 指纹中。

---

## 十、Unix Socket 协议

### 请求 (Nginx → Daemon)

```json
{"src_ip":"192.168.1.1","dst_ip":"192.168.5.8","src_port":12345,"dst_port":443}
```

- `src_ip`: 客户端 IP（IPv4 点分十进制）
- `dst_ip`: 服务器 IP
- 每行一条 JSON，以换行符 `\n` 结束

### 响应 (Daemon → Nginx)

```json
{
  "found": true,
  "window_size": 65535,
  "mss": 1460,
  "window_scale": 7,
  "sack_permitted": 1,
  "options_order": [2, 4, 8, 3],
  "capture_age_ms": 1234,
  "ip_ttl": 52,
  "rtt_us": 8500,
  "ja4s_fingerprint": "t1303h2_1301_abc123def456"
}
```

| 字段 | 类型 | 说明 |
|------|------|------|
| `found` | bool | 是否找到匹配的连接记录 |
| `window_size` | u16 | TCP 接收窗口大小 (原始值) |
| `mss` | u16 | 最大段大小，未出现时为 0 |
| `window_scale` | u8 | 窗口缩放因子 (0-14)，未出现时为 0 |
| `sack_permitted` | u8 | 是否支持 SACK，1=支持 |
| `options_order` | [u8] | TCP 选项出现顺序 |
| `capture_age_ms` | u64 | 数据距当前的毫秒数 |
| `ip_ttl` | u8\|null | IP TTL 观察值 (1-255) |
| `rtt_us` | u64\|null | TCP RTT 微秒值 (SYN→SYN-ACK) |
| `ja4s_fingerprint` | string\|null | JA4S TLS Server Hello 指纹 |

---

## 十一、Nginx 集成

### 11.1 配置示例

```nginx
http {
    # 共享字典，缓存 JA4T 结果
    lua_shared_dict ja4t_cache 32m;

    server {
        listen 443 ssl;

        # 声明变量
        set $ja4t_fingerprint "";
        set $ja4t_ttl 0;
        set $ja4t_rtt_us 0;
        set $ja4s_fingerprint "";

        access_by_lua_block {
            local cjson = require "cjson"
            local src_ip   = ngx.var.remote_addr
            local dst_ip   = ngx.var.server_addr
            local src_port = ngx.var.remote_port
            local dst_port = ngx.var.server_port

            -- 查本地缓存
            local cache_key = src_ip .. "->" .. dst_ip .. ":" .. dst_port
            local cache = ngx.shared.ja4t_cache
            local cached = cache:get(cache_key)
            local ja4t_data = nil

            if cached then
                ja4t_data = cached
            else
                local sock = ngx.socket.tcp()
                sock:settimeout(100)  -- 100ms 超时

                local ok, err = sock:connect("unix:/var/run/ja4t.sock")
                if ok then
                    local query = cjson.encode({
                        src_ip = src_ip, dst_ip = dst_ip,
                        src_port = tonumber(src_port),
                        dst_port = tonumber(dst_port),
                    })
                    sock:send(query .. "\n")
                    local data, err = sock:receive("*l")
                    if data then
                        local ok, body = pcall(cjson.decode, data)
                        if ok and body.found then
                            cache:set(cache_key, data, 600)
                            ja4t_data = data
                        end
                    end
                    sock:close()
                end
            end

            if ja4t_data then
                local body = cjson.decode(ja4t_data)
                if body.found then
                    -- JA4T 指纹
                    local opts = table.concat(body.options_order, "-")
                    ngx.var.ja4t_fingerprint = string.format(
                        "%d_%d_%d_%d_%s",
                        body.window_size, body.mss,
                        body.window_scale, body.sack_permitted, opts
                    )
                    -- JA4L 信号
                    ngx.var.ja4t_ttl = body.ip_ttl or 0
                    ngx.var.ja4t_rtt_us = body.rtt_us or 0
                    -- JA4S 指纹
                    ngx.var.ja4s_fingerprint = body.ja4s_fingerprint or ""
                end
            end
        }

        location / {
            proxy_set_header X-JA4T-Fingerprint $ja4t_fingerprint;
            proxy_set_header X-JA4T-TTL        $ja4t_ttl;
            proxy_set_header X-JA4T-RTT        $ja4t_rtt_us;
            proxy_set_header X-JA4S-Fingerprint $ja4s_fingerprint;
            proxy_pass http://backend;
        }
    }
}
```

### 11.2 缓存策略

| 缓存层 | 位置 | TTL | Key | 说明 |
|--------|------|-----|-----|------|
| Nginx ja4t_cache | lua_shared_dict 32m | 600s | `src_ip->dst_ip:dst_port` | 仅缓存 found=true |
| Daemon ConnectionCache | 内存 HashMap | 5s 全量替换 | `IP:Port->IP:Port` | BPF Map dump |
| Daemon ServerHelloCache | 内存 HashMap | 5s 全量替换 | 5-tuple key | BPF Map dump |

> **仅缓存 found=true**: 避免 `found=false` 因 poll 竞态被长缓存 600 秒。

### 11.3 后端 Handler 集成

Nginx 注入的 Headers 在后端验证链中被以下 Handler 消费：

| Header | Handler | 检测内容 |
|--------|---------|---------|
| `X-JA4T-Fingerprint` | Ja4FingerprintHandler | JA4+JA4H+JA4T 交叉验证 + Cookie 变异 |
| `X-JA4T-TTL` | Ja4LHandler | TTL OS 推断 + RTT 距离 + MSS 语义 |
| `X-JA4T-RTT` | Ja4LHandler | RTT 光速约束 + Geo 矛盾检测 |
| `X-JA4S-Fingerprint` | Ja4SHandler | JA4-JA4S 配对基线验证 |

---

## 十二、当前实现状态

| 组件 | 状态 | 文件 |
|------|------|------|
| eBPF TC Ingress (SYN + TTL + RTT) | ✅ 已实现 | `tcp_options.bpf.c` (423行) |
| eBPF TC Egress (Server Hello) | ✅ 已实现 | `tls_server_hello.bpf.c` (422行) |
| Daemon TCP 缓存 + Socket | ✅ 已实现 | `ja4t/` + `server.rs` |
| Daemon JA4S 计算模块 | ✅ 已实现 | `ja4s/compute.rs` (231行) |
| Daemon JA4S Map 轮询 | ✅ 已实现 | `ja4s/poller.rs` |
| Daemon CLI + 协议层 | ✅ 已实现 | `cli.rs` + `protocol.rs` |
| Nginx JA4T/JA4H/JA4 注入 | ✅ 已实现 | `cc_manage.nginx-v2.0.0.conf` |
| Nginx JA4L (TTL/RTT) 透传 | ✅ 已实现 | `$ja4t_ttl` / `$ja4t_rtt_us` 变量 |
| Nginx JA4S Fingerprint 透传 | ✅ 已实现 | `$ja4s_fingerprint` 变量 |
| Ja4FingerprintHandler | ✅ 已实现 | `ja4_fingerprint_handler.rs` |
| Ja4LHandler (TTL+RTT+MSS) | ✅ 已实现 | `ja4l_handler.rs` (12条规则) |
| Ja4SHandler (JA4-JA4S 配对) | ✅ 已实现 | `ja4s_handler.rs` (内存 HashMap 基线) |
| Cookie 一致性检测 | ✅ 已实现 | `cookie_consistency.rs` |

> **全部核心组件已实现。** 后端验证链共 23 个 Handler（含新增的 TimezoneCrossValidationHandler），短路执行 + Phase 2 累积评分。

---

## 十三、关键运行参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--interface` | `eth0` | 挂载 TC 过滤器的网卡（同时挂载 ingress + egress） |
| `--socket` | `/var/run/ja4t.sock` | Unix Domain Socket 路径 |
| `--poll-interval` | `5` | BPF Map 轮询间隔（秒），生产建议 1-2 |
| `--verbose` | false | 详细日志输出 |
| `--bpf-object` | 自动检测 | JA4T TCP SYN BPF 目标文件路径 |
| `--bpf-server-hello-object` | 自动检测 | JA4S TLS Server Hello BPF 目标文件路径 |

### 关键指标

| 指标 | 值 |
|------|-----|
| BPF Map 类型 | LRU Hash × 2 (tcp + tls) |
| Map 容量 | 65,536 × 2 |
| tcp_options_info 大小 | 64 bytes |
| tls_server_hello_info 大小 | 176 bytes |
| 轮询周期 | 5 秒 (可配) |
| Nginx 缓存 TTL | 600 秒 (仅 found=true) |
| Nginx 缓存 key | `src_ip->dst_ip:dst_port` |
| Unix Socket 权限 | 0666 |
| Socket 超时 | 100ms (Nginx Lua) |
| 响应延迟 (命中) | ~微秒级 |
| 响应延迟 (未命中) | ~百微秒级 |
| eBPF 扫描范围 | offset 0..38, step 2 (p0f 兼容) |
| TCP 选项最大字节 | 40 |

---

## 十四、性能与资源

| 组件 | 开销 |
|------|------|
| eBPF 每包处理 | < 1µs (对 10Gbps 线速无影响) |
| Daemon 内存: tcp_options_map | ~4 MB (64B × 65K) |
| Daemon 内存: tls_server_hello_map | ~11.4 MB (176B × 65K) |
| Daemon 内存: 缓存 | ~13 MB (ConnectionCache + ServerHelloCache) |
| **Daemon 总内存** | **~28 MB** |

---

## 十五、故障排查

### daemon 启动失败

```bash
uname -r                          # 需要 >= 5.3
ls /sys/kernel/btf/               # 检查 BPF 支持
sudo setcap cap_bpf,cap_net_admin=eip ./ja4t-daemon  # 检查权限
sudo tc qdisc show dev eth0       # 检查 clsact
```

### Socket 无响应

```bash
ls -la /var/run/ja4t.sock         # 检查 socket
ps aux | grep ja4t                # 检查进程
echo '{"src_ip":"10.0.0.1","dst_ip":"10.0.0.2","src_port":8080,"dst_port":80}' \
    | sudo nc -U /var/run/ja4t.sock  # 手动测试
```

### BPF 未捕获数据

```bash
sudo tc filter show dev eth0 ingress    # 检查 ingress filter
sudo tc filter show dev eth0 egress     # 检查 egress filter
sudo bpftool map dump name tcp_options_map | head -10
sudo bpftool map dump name tls_server_hello_map | head -10
sudo bpftool map dump name debug_counters
```

### 为什么 Not Found

**现象**: Nginx 日志显示 `not found`，但 `nc -U` 手动查询能查到。

**原因**: 时序问题。BPF 只捕获 daemon 启动**之后**新建立的 TCP SYN。如果连接在 daemon 启动前就已建立（如 HTTP/2 长连接），BPF map 中无记录。

**解决**: 旧连接断开、新连接建立后自动恢复。

### 提高 JA4T 命中率

**核心手段**: 缩小 `--poll-interval`（默认 5s → 建议 1-2s），缩短 TCP SYN 到达与 daemon poll 之间的竞态窗口。

**辅助手段**: daemon 早于 nginx 启动（systemd `Before=nginx.service`）。

**不可消除的窗口**: TCP 三次握手约 1-2ms，之后 HTTP 请求即到达。即使 poll-interval=100ms，仍有极低概率在窗口内。nginx 侧不缓存 `not found`，下次请求重试。

---

## 十六、降级策略

| 故障场景 | 处理方式 |
|---------|---------|
| daemon 不可用 | JA4T/JA4L/JA4S 字段为 None → 对应 Handler 返回 None (跳过) |
| BPF Map 满 (64K LRU) | 最老条目淘汰，部分连接缺少数据 |
| SYN-ACK 未观测 (synack_ns=0) | RTT 为空 → RTT 规则跳过，TTL 规则正常 |
| IP 地理数据不可用 | RTT vs Geo 规则跳过 |
| JA4S 基线为空 (冷启动) | S-3 规则跳过, S-1/S-2/S-4/S-5 正常 |

**设计原则**: 每个信号独立降级，单点故障不影响其他检测维度。

---

## 十七、相关文档

| 文档 | 路径 | 说明 |
|------|------|------|
| 实现细节说明 | `docs/IMPLEMENTATION.md` | eBPF 程序实现、TCP 选项解析、JA4S 算法详解 |
| 数据流图 | `docs/JA4T_DATA_FLOW.md` | Mermaid 流程图 + 时序图 |
| JA4L+JA4S 集成 | `docs/JA4L-JA4S-Integration.md` | JA4L/JA4S 检测规则 + 部署步骤 |
| 系统架构 | `002.REF-DOCS/03-Architecture/CYBERCLOAK-Architecture.md` | 全系统架构文档 |
| Nginx 配置 | `000.CODE_SPACE/002.APP_DOCS/005.nginx/` | Nginx 配置与集成文档 |
