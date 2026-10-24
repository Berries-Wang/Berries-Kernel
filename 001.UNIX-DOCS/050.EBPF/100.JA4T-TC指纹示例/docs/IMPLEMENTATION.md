> 📌 迁移自 `Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/docs/IMPLEMENTATION.md`(2026-10-24);导读见 [000.导读.md](../000.导读.md)。

# JA4T Daemon 实现说明

> 最后更新: 2026-07-08

## 一、源码架构

```
003.JA4T_PLUGIN/
├── build.rs                      # Cargo build 脚本（编译 eBPF C 程序为 BPF 目标文件）
├── Cargo.toml                    # Rust 项目配置
│
├── src/
│   ├── main.rs                   # (94行) ja4t-daemon 启动入口
│   ├── server.rs                 # (286行) Unix Socket 服务器（tokio per-client）
│   ├── protocol.rs               # (106行) LookupRequest/LookupResponse JSON 协议
│   ├── cli.rs                    # (32行) CLI 参数解析 (clap derive)
│   │
│   ├── ja4s/                     # JA4S 指纹计算模块 (TLS Server Hello)
│   │   ├── mod.rs                # (68行) 模块入口 + 公共 API 重导出
│   │   ├── compute.rs            # (231行) compute_ja4s() 8步算法
│   │   ├── cache.rs              # (116行) ServerHelloCache (Arc<RwLock<HashMap>>)
│   │   ├── constants.rs          # (18行) GREASE 表 (RFC 8701, 16个值)
│   │   ├── types.rs              # (102行) 数据结构 (Ja4sData, Ja4sFields, Ja4sFingerprint)
│   │   ├── reader.rs             # (57行) BPF Map 读取封装
│   │   ├── loader.rs             # (52行) eBPF 加载 + TC Egress 挂载
│   │   └── poller.rs             # (37行) 后台轮询任务
│   │
│   ├── ja4t/                     # JA4T TCP 指纹采集模块
│   │   ├── mod.rs                # (21行) 模块入口
│   │   ├── types.rs              # (49行) BPF Map C 兼容结构体
│   │   ├── options.rs            # (91行) TCP Option 解析器 (p0f 风格)
│   │   ├── reader.rs             # (80行) BPF Map 读取器
│   │   ├── cache.rs              # (70行) ConnectionCache (Arc<RwLock<HashMap>>)
│   │   ├── loader.rs             # (52行) eBPF 加载 + TC Ingress 挂载
│   │   └── poller.rs             # (80行) 后台轮询任务
│   │
│   └── ebpf/                     # eBPF C 程序 (编译为 BPF 目标文件)
│       ├── tcp_options.h         # (73行) BPF Map 共享结构体 C header
│       ├── tcp_options.bpf.c     # (423行) eBPF TC Ingress — SYN 捕获 + SYN-ACK RTT
│       └── tls_server_hello.bpf.c # (422行) eBPF TC Egress — Server Hello TLS 指纹
```

## 二、eBPF 程序 (tcp_options.bpf.c) — 423 行

### 2.1 数据包定位策略（参考 p0f）

**不**依赖 Ethernet EtherType (`0x0800`) 定位 IP 头，而是采用 p0f `find_offset()` 方法：
- 在偏移量 0–38（步长 2）扫描 IP 版本号（首字节高 4 位 == 4）
- 验证偏移 +9 处协议字段 == TCP (6)
- 通过协议字段二次确认，避免误匹配

**为什么不直接用 EtherType**：USB WiFi 驱动可能在标准 Ethernet 帧头之前添加额外字节（radiotap、prism 等），直接扫描 IP 版本号对任意链路层偏移都有效。

### 2.2 处理两类 TCP 包

| 包类型 | 条件 | 操作 |
|--------|------|------|
| 纯 SYN | SYN=1, ACK=0 | 提取 TCP 指纹 + TTL → 写入 `tcp_options_map` |
| SYN-ACK | SYN=1, ACK=1 | 反向 5-tuple 匹配已有 SYN → 更新 `synack_ns` (RTT) |

### 2.3 采集字段

| 字段 | 来源 | 说明 |
|------|------|------|
| window_size | TCP header offset 14, 2 bytes | 窗口大小（JA4T 指纹输入） |
| raw_opts | TCP options（header 20 bytes 之后） | 原始选项字节，最多 40 bytes |
| raw_opts_len | 同上 | raw_opts 有效长度 |
| ip_ttl | IP header offset 8, 1 byte | IP TTL（替代原 `_pad[1]`，OS 推断用） |
| capture_ns | `bpf_ktime_get_ns()` | SYN 捕获时间戳（纳秒） |
| synack_ns | `bpf_ktime_get_ns()` (SYN-ACK 时) | SYN-ACK 到达时间戳（0=未捕获） |

### 2.4 BPF Map 定义

| Map | 类型 | 容量 | Key | Value |
|-----|------|------|-----|-------|
| `tcp_options_map` | LRU Hash | 65,536 | `connection_tuple` (16B) | `tcp_options_info` (64B) |
| `debug_counters` | Array | 10 | u32 index | u64 counter |

### 2.5 调试计数器 (10 个)

| 索引 | 常量 | 含义 |
|------|------|------|
| 0 | COUNTER_TOTAL | 收到数据包总数 |
| 1 | COUNTER_IPV4 | 识别为 IPv4 |
| 2 | COUNTER_TCP | 识别为 TCP |
| 3 | COUNTER_SYN | 识别为纯 SYN |
| 4 | COUNTER_MAP_OK | 成功写入 BPF Map |
| 5 | COUNTER_ETHERTYPE | 检测到的 IP 头偏移（诊断） |
| 6 | COUNTER_PROTO | 首次 IP 协议字节（诊断） |
| 7 | COUNTER_TCP_START | TCP 头在包中的偏移（诊断） |
| 8 | COUNTER_SYNACK | SYN-ACK 包捕获数 |
| 9 | COUNTER_SYNACK_OK | SYN-ACK 反向匹配成功数 |

### 2.6 过滤条件

- 仅 IPv4 + TCP
- 跳过 IP 分片包（`frag_off & 0x3FFF != 0`）
- 数据偏移 >= 5（TCP header >= 20 bytes）
- SYN-ACK 分支：`synack_ns == 0` 检查防止重复更新（并发安全）

### 2.7 TCP Options Value 结构体 (64 bytes)

```
offset 0:  window_size (2 bytes)
offset 2:  raw_opts_len (1 byte)
offset 3:  ip_ttl (1 byte)        ← 替代原 _pad[1]
offset 4:  raw_opts[40] (40 bytes)
offset 44: _pad2[4] (4 bytes)
offset 48: capture_ns (8 bytes)   ← SYN 捕获时间戳
offset 56: synack_ns (8 bytes)    ← SYN-ACK 到达时间戳
────────────────────────────────────
Total: 64 bytes
```

## 三、eBPF 程序 (tls_server_hello.bpf.c) — 422 行

### 3.1 定位

挂载到网卡 **TC Egress** 路径，捕获服务器发往客户端的 TLS Server Hello 包。
与 TC Ingress 的 `tc_options.bpf.c` 互补：ingress 采集 TCP 指纹，egress 采集 TLS 响应指纹。

### 3.2 包识别

| 步骤 | 条件 | 说明 |
|------|------|------|
| 1 | 源端口 = 443 | 服务器 TLS 端口 |
| 2 | TLS Record Type = 0x16 | Handshake |
| 3 | Handshake Type = 0x02 | Server Hello |

### 3.3 TLS 解析结构

```
TLS Record: [Type(1)][Version(2)][Length(2)][Payload...]
Handshake:  [Type(1)][Length(3)][Body...]
ServerHello: [Version(2)][Random(32)][SessionID(v)][CipherSuite(2)]
              [CompressionMethod(1)][Extensions(v)]
Extension:  [Type(2)][Length(2)][Data(v)]
```

### 3.4 采集字段 (tls_server_hello_info)

| 字段 | 大小 | 说明 |
|------|------|------|
| tls_version | 2 bytes | 0x0304=TLS1.3, 0x0303=TLS1.2 |
| cipher_suite | 2 bytes | 服务器选择的密码套件 (单个) |
| extension_count | 1 byte | 含 GREASE |
| alpn_len + alpn | 1 + 32 bytes | 选择的 ALPN 协议名 |
| ext_raw_len + ext_raw | 1 + 120 bytes | 扩展 type 原始字节（原始顺序，含 GREASE） |
| capture_ns | 8 bytes | 捕获时间戳 |
| **Total** | **168 bytes** |

### 3.5 BPF Map

| Map | 容量 | Key | Value |
|-----|------|-----|-------|
| `tls_server_hello_map` | 65,536 | `ja4s_connection_key` (16B) | `tls_server_hello_info` (168B) |

### 3.6 调试计数器 (8 个)

| 索引 | 含义 |
|------|------|
| 0 | 总包数 |
| 1 | TCP 端口 443 包数 |
| 2 | TLS Handshake 记录数 |
| 3 | Server Hello 包数 |
| 4 | 成功写入 Map 数 |
| 5 | 有 ALPN 扩展的 Server Hello 数 |
| 6 | 扩展数溢出次数 |
| 7 | 跳过的分片包数 |

## 四、用户态程序 (main.rs + server.rs + protocol.rs + cli.rs) — 518 行

### 4.1 main.rs (94行) — 启动入口

- 解析 CLI 参数 (clap derive)
- 加载 2 个 eBPF 程序: TC Ingress (ja4t) + TC Egress (ja4s)
- 创建 2 个内存缓存: ConnectionCache + ServerHelloCache
- 启动 2 个后台 poll 任务 (5s 轮询 BPF Map)
- 启动 Unix Socket 服务器 (阻塞主线程)

### 4.2 server.rs (286行) — Unix Socket 服务器

- 监听 `/var/run/ja4t.sock`
- tokio per-client spawn，换行分隔 JSON
- 两级查找: 快速路径 (内存缓存, ~μs) → 回退路径 (BPF 直查, ~100μs)
- 并行查询 JA4S: 反向五元组查询 ServerHelloCache

### 4.3 protocol.rs (106行) — JSON 协议

- LookupRequest: {src_ip, dst_ip, src_port, dst_port}
- LookupResponse: {found, window_size, mss, window_scale, sack_permitted, options_order, capture_age_ms, ip_ttl, rtt_us, ja4s_fingerprint}

### 4.4 TCP 选项解析 (ja4t/options.rs — 91行)

解析逻辑参考 p0f 的 `parse_packet()`：
- **MSS** (kind=2, len=4): 提取 2 字节 MSS 值
- **Window Scale** (kind=3, len=3): 提取 1 字节缩放因子
- **SACK Permitted** (kind=4, len=2): 记录存在
- **Timestamp** (kind=8): 记录到 options_order（不解析值）
- **EOL** (kind=0): 终止解析
- **NOP** (kind=1): 跳过，记录到 order
- 最多解析 12 种 Option（匹配 p0f 行为）

### 4.2 Unix Socket 协议（换行分隔 JSON）

**请求** (Nginx → Daemon):
```json
{"src_ip":"192.168.1.1","dst_ip":"192.168.5.8","src_port":12345,"dst_port":443}
```

**响应** (Daemon → Nginx):
```json
{"found":true,"window_size":65535,"mss":1460,"window_scale":7,
 "sack_permitted":1,"options_order":[2,4,8,3],"capture_age_ms":1234,
 "ip_ttl":52,"rtt_us":8500}
```

### 4.3 两级查找架构

| 路径 | 机制 | 延迟 |
|------|------|------|
| 快速路径 | `ConnectionCache.lookup()` — 内存 HashMap | ~微秒级 |
| 回退路径 | `BpfReader::lookup_direct()` — BPF Map 直查 | ~百微秒级 |

缓存 key 格式: `"IP:Port->IP:Port"` (例: `"192.168.1.1:54321->10.0.0.1:443"`)

### 4.4 RTT 计算

```rust
let rtt_us = if info.synack_ns > 0 && info.synack_ns > info.capture_ns {
    Some((info.synack_ns - info.capture_ns) / 1000) // ns → µs
} else {
    None // SYN-ACK 未观测到
};
```

### 4.5 Aya 生命周期注意事项

- `aya::Bpf` 对象必须在 daemon 整个生命周期中保持存活
- 如果被 drop，其内部的 `SchedClassifierLink` 会自动调用 `detach()` 移除 TC 过滤器
- `main()` 中以 `let (_reader, _bpf) = load_and_attach(...)` 确保生命周期

## 五、JA4S 计算模块 (ja4s/) — 752 行

### 5.1 JA4S 指纹格式

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

### 5.2 JA4 vs JA4S 算法差异

| 维度 | JA4 (客户端) | JA4S (服务器) |
|------|-------------|----------------|
| 密码套件 | 列表排序后 hash | **单个 cipher hex** |
| Extension 计数 | 排除 GREASE | **包含 GREASE** |
| Extension hash | 排序, 排除 SNI+ALPN, 排除 GREASE | **原始顺序, 包含 GREASE** |
| SNI 标记 | d/i | 无 |
| 签名算法 | 包含在 Part C | 无 |

### 5.3 compute_ja4s() 8 步算法

1. Protocol prefix = "t"
2. TLS version mapping (TLS_MAPPER)
3. Extension count: `min(count, 99)` 2 位数（含 GREASE）
4. ALPN: 第一个 ALPN 值的首尾字母数字字符，无则 "00"
5. Part A = `"t" + ver + #exts + alpn`
6. Part B = cipher hex lowercase（无 "0x" 前缀）
7. Part C = SHA256(extension type 原始顺序逗号拼接) [:12]
8. JA4S = PartA + "_" + PartB + "_" + PartC

### 5.4 GREASE 表 (RFC 8701, 16 个值)

```rust
const GREASE_VALUES: [u16; 16] = [
    0x0a0a, 0x1a1a, 0x2a2a, 0x3a3a, 0x4a4a, 0x5a5a, 0x6a6a, 0x7a7a,
    0x8a8a, 0x9a9a, 0xaaaa, 0xbaba, 0xcaca, 0xdada, 0xeaea, 0xfafa,
];
```

## 六、共享头文件 (tcp_options.h) — 73 行

### 6.1 结构体定义

| 结构体 | 大小 | 用途 |
|--------|------|------|
| `connection_tuple` | 16 bytes | TCP 连接 5 元组（BPF Map Key） |
| `tcp_options_info` | 64 bytes | TCP SYN 指纹数据（BPF Map Value） |
| `ja4s_connection_key` | 16 bytes | TLS 连接 5 元组（TLS BPF Map Key） |
| `tls_server_hello_info` | 168 bytes | TLS Server Hello 指纹数据（TLS BPF Map Value） |

### 6.2 常量定义

```c
#define MAX_TCP_OPTIONS 12   // 最多解析 12 种 TCP Option
#define MAX_RAW_OPTS 40       // 原始 Option 字节最大长度
#define MAX_ALPN_LEN 32       // ALPN 协议名最大长度
#define MAX_EXT_RAW_BYTES 120 // 扩展 type 原始字节最大长度
```

## 七、build.rs — 构建脚本

- 使用 `cc` crate 调用 clang 编译 `tcp_options.bpf.c` 和 `tls_server_hello.bpf.c` 为 BPF 目标文件
- 编译参数: `-target bpf -O2 -g -Wall`
- 通过 `EBPF_OBJECT_PATH` 环境变量传递路径给 Rust 代码
- `cargo:rerun-if-changed` 监测源文件变更

### Rust 依赖项 (Cargo.toml)

| 依赖 | 版本 | 用途 |
|------|------|------|
| `aya` | 0.12 | eBPF 程序加载和 Map 操作 |
| `tokio` | 1 (full) | 异步运行时 (Unix Socket + 轮询) |
| `clap` | 4 (derive) | CLI 参数解析 |
| `serde` / `serde_json` | 1 | JSON 序列化 (Socket 协议) |
| `sha2` | 0.10 | SHA256 (JA4S 扩展 hash 计算) |
| `hex` | 0.4 | Hex 编码 (JA4S 密码套件) |
| `log` / `env_logger` | 0.4 / 0.11 | 日志 |

## 八、Nginx 集成与数据流

### 8.1 整体调用链

```
TCP SYN → eBPF TC Ingress (tcp_options.bpf.c)
            │  写入 tcp_options_map
            ▼
          ja4t-daemon (poll 5s)
            │  解析 TCP options + 计算 JA4S
            ▼
          Unix Socket /var/run/ja4t.sock
            │  JSON 查询/响应
            ▼
          Nginx Lua (rewrite_by_lua_block)
            │  注入 Headers:
            │    X-JA4T-Fingerprint
            │    X-JA4T-TTL
            │    X-JA4T-RTT
            │    X-JA4S-Fingerprint
            ▼
          Rust Backend (VerificationContext)
            │  Handlers: Ja4Fingerprint, Ja4L, Ja4S
            ▼
          Decision: ALLOW / BLOCK
```

### 8.2 Nginx 配置要点

- Unix Socket 连接复用: 每个 Nginx worker 通过 `ngx.socket.connect()` 连接 daemon
- 超时策略: `settimeout(50)` — 50ms 超时，超时后 `found=false`，不阻塞请求
- Header 注入: `proxy_set_header` 将 JA4 系列指纹注入上游后端

### 8.3 当前实现状态

| 组件 | 状态 | 说明 |
|------|------|------|
| eBPF TC Ingress (SYN 采集) | ✅ 已实现 | tcp_options.bpf.c，含 TTL + RTT |
| eBPF TC Egress (JA4S 采集) | ✅ 已实现 | tls_server_hello.bpf.c，独立 eBPF 程序 |
| Daemon TCP 模块 | ✅ 已实现 | ja4t/ (6 files, 362行)，内存缓存 + BPF 直查 |
| Daemon JA4S 计算模块 | ✅ 已实现 | ja4s/ (7 files, 752行)，用户态 SHA256 |
| Daemon Socket + 协议 | ✅ 已实现 | server.rs (286行) + protocol.rs (106行) |
| Daemon CLI | ✅ 已实现 | cli.rs (32行) |
| Ja4FingerprintHandler | ✅ 已实现 | JA4+JA4H+JA4T 联合检测 |
| Ja4LHandler (TTL+RTT+MSS) | ✅ 已实现 | L3/L4 物理层反欺骗 (12条规则) |
| Ja4SHandler (配对基线) | ✅ 已实现 | TLS 响应指纹 + JA4 配对验证 |
| Cookie 一致性检测 | ✅ 已实现 | 会话内 Cookie 结构变异检测 |
| JA4 解析统一模块 | ✅ 已实现 | ja4_parse.rs (共享模块) |

## 九、关键运行参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--interface` | `eth0` | 挂载 TC 过滤器的网卡 |
| `--socket` | `/var/run/ja4t.sock` | Unix Domain Socket 路径 |
| `--poll-interval` | `5` | BPF Map 轮询间隔（秒） |
| `--verbose` | false | 详细日志输出 |
| `--bpf-object` | 自动检测 | 编译后的 BPF 目标文件路径 |
