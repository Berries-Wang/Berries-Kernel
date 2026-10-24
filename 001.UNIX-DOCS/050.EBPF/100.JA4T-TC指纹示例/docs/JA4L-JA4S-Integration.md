> 📌 迁移自 `Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/docs/JA4L-JA4S-Integration.md`(2026-10-24);导读见 [000.导读.md](../000.导读.md)。

# JA4L + JA4S 集成文档

> 最后更新: 2026-07-08
> 
> 本文档描述 CYBERCLOAK 斗篷系统中 JA4L（L3/L4 物理层信号）和 JA4S（TLS Server Hello 响应指纹）的集成架构、数据流和部署步骤。

## 一、架构概览

```
┌─────────────────────────────────────────────────────────────────────┐
│                        eBPF 采集层                                   │
│                                                                     │
│  [tcp_options.bpf.c]                    [tls_server_hello.bpf.c]     │
│  TC Ingress (入站)                     TC Egress (出站)              │
│  │  - SYN 包: TCP 指纹 + IP TTL        │  - Server Hello 包:        │
│  │  - SYN-ACK 包: 反向匹配更新 RTT     │    TLS 版本 + 密码套件     │
│  │                                     │    + 扩展列表 + ALPN       │
│  ▼                                     ▼                            │
│  tcp_options_map (LRU Hash)            tls_server_hello_map          │
│  64 bytes/value, 65536 entries         168 bytes/value, 65536 entries│
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      ja4t-daemon (Rust)                              │
│                                                                     │
│  • 轮询 BPF Map (每 5 秒)                                          │
│  • 用户态解析 TCP 选项 (parse_tcp_options)                          │
│  • 用户态计算 JA4S 指纹 (ja4s.rs, SHA256)                          │
│  • Unix Socket (/var/run/ja4t.sock) 响应 nginx 查询                 │
│  • 内存缓存: ConnectionCache + ServerHelloCache (HashMap), 两级查找  │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      Nginx (OpenResty)                               │
│                                                                     │
│  • ssl_client_hello_by_lua_block: 计算 JA4 指纹 (resty.ja4)        │
│  • rewrite_by_lua_block: 计算 JA4H 指纹 (resty.ja4h)               │
│  • access_by_lua_block: 查询 ja4t-daemon (Unix Socket)             │
│  • proxy_set_header 注入 Headers:                                   │
│      X-JA4-Fingerprint, X-JA4H-Fingerprint, X-JA4T-Fingerprint     │
│      X-JA4T-TTL, X-JA4T-RTT           ← JA4L 新增                  │
│      X-JA4S-Fingerprint               ← JA4S 新增 (预留变量)        │
│  • 缓存: ja4t_cache (32m lua_shared_dict), found=true 缓存 600s     │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                       Rust 后端 (Axum)                               │
│                                                                     │
│  验证链 (短路执行 + Phase 2 累积评分, 共23个Handler):                │
│  ── 阶段 1: 不依赖 IP 数据的规则 ──                                  │
│    1. Ja4FingerprintHandler    JA4/JA4H/JA4T + Cookie 变异检测      │
│    2. BrowserFingerprintHandler  浏览器 Canvas/WebGL 指纹            │
│    3. UserAgentBlockHandler     UA 结构检测                          │
│    4. HttpHeaderBlockHandler    Accept/Referer/Origin/Sec-Fetch      │
│    5. Ja4LHandler          ← TTL/RTT/MSS 物理层反欺骗 (无IP依赖)     │
│  ── 阶段 2: IP 相关规则 ──                                          │
│    6. IpInfoBlacklistHandler   IP 字段黑名单                         │
│    7. GoogleBlockHandler       Google IP 范围拦截                    │
│    8. FacebookBlockHandler     Facebook IP 范围拦截                  │
│    9. AmazonBlockHandler       Amazon IP 范围拦截                    │
│   10. WhitelistIpHandler       IP 白名单                             │
│   11. BlacklistIpHandler       IP 黑名单                             │
│   12. AsnHandler               ASN 过滤                              │
│   13. CrawlerHandler           爬虫检测 (含 JA4 身份验证)            │
│   14. ProxyDetectionHandler    代理/VPN 检测                         │
│   15. DatacenterDetectionHandler 数据中心检测 (含 Google Ads 识别)   │
│   16. MobileNetworkDetectionHandler 移动网络检测                     │
│   17. Ja4SHandler          ← TLS Server Hello 指纹 + JA4 配对基线   │
│   18. TimezoneCrossValidationHandler ← 时区交叉验证 (IP时区↔JS时区)  │
│   19. DeviceHandler             设备/OS 过滤                         │
│   20. LanguageHandler           语言过滤 + IP Geo 交叉验证            │
│   21. RegionHandler             地区过滤                             │
│   22. CyberCloakDefaultHandler  兜底放行                             │
│   23. AnomalousTimeBlockHandler 异常时段拦截                         │
│  *  RateLimitHandler (可选)    频率限制                              │
│                                                                     │
│  Phase 2 累积评分: 弱信号处理器通过 ctx.add_score() 累加分数，       │
│  全部执行完毕后若超过阈值(默认40)则拦截。                              │
└─────────────────────────────────────────────────────────────────────┘
```

## 二、数据结构变更

### 2.1 BPF Map Value: tcp_options_info

```
旧 (56 字节):                         新 (64 字节):
offset 0:  window_size (2)            offset 0:  window_size (2)
offset 2:  raw_opts_len (1)           offset 2:  raw_opts_len (1)
offset 3:  _pad (1)  ← 被废弃         offset 3:  ip_ttl (1)      ← 新
offset 4:  raw_opts[40]               offset 4:  raw_opts[40]
offset 44: _pad2[4]                   offset 44: _pad2[4]
offset 48: capture_ns (8)             offset 48: capture_ns (8)
                                      offset 56: synack_ns (8)   ← 新
```

### 2.2 新增 BPF Map: tls_server_hello_map

```
Key: ja4s_connection_key (16 bytes)
  src_ip(4) + dst_ip(4) + src_port(2) + dst_port(2) + protocol(1) + pad(3)

Value: tls_server_hello_info (168 bytes)
  tls_version(2) + cipher_suite(2) + extension_count(1) + alpn_len(1)
  + alpn[32] + ext_raw_len(1) + ext_raw[120] + pad[2] + capture_ns(8)
```

### 2.3 VerificationContext 新增字段

```rust
pub ja4t_ttl: Option<u8>,           // IP TTL 观察值
pub ja4t_rtt_us: Option<u64>,       // TCP RTT (微秒)
pub ja4s_fingerprint: Option<String>, // Server Hello 指纹
```

## 三、JA4L 检测规则

| 规则 | 信号 | 条件 | 分数 | 置信度 |
|------|------|------|------|--------|
| L-1 | TTL vs JA4T OS | JA4T→Windows, TTL init=64 | +25 | High |
| L-2 | TTL vs JA4T OS | JA4T→Linux/Apple, TTL init=128 | +25 | High |
| L-3 | TTL vs UA OS | UA→Windows, TTL init=64 | +20 | High |
| L-4 | TTL vs UA OS | UA→Apple, TTL init=128 | +20 | High |
| L-5 | TTL vs CH | CH→Windows, TTL init=64 | +20 | High |
| L-6 | TTL vs CH | CH→macOS, TTL init=128 | +20 | High |
| L-7 | Hop count | hop_count > 30 | +15 | Medium |
| L-8 | Hop + RTT | ≤ 2 hops, RTT > 50ms | +10 | Medium |
| L-10 | RTT vs Geo | RTT < 20ms but IP claims other continent | +30 | High |
| L-20 | MSS vs IP | Residential IP + VPN MSS | +20 | High |
| L-21 | MSS vs IP | Datacenter IP + VPN MSS | +15 | Medium |
| L-23 | MSS vs IP | Mobile network + Ethernet MSS | +10 | Medium |
| L-12 | Composite | 3+ 独立 L3/L4 信号命中 | ×1.5x | Bonus |

## 四、JA4S 检测规则

| 规则 | 信号 | 条件 | 分数 | 置信度 |
|------|------|------|------|--------|
| S-1 | JA4 + JA4S | JA4→非浏览器 + JA4S ext < 3 | +30 | High |
| S-2 | JA4 vs JA4S ALPN | JA4 声称 HTTP/2 但 JA4S ALPN=00 | +25 | High |
| S-3 | JA4 vs baseline | JA4 为浏览器模式, {JA4,JA4S} 不在已验证基线中 | +25 | Medium |
| S-4 | JA4S ext count | extension_count 超出 [3,8] | +15 | Medium |
| S-5 | JA4S ext count | extension_count = 0 | +10 | Low |

## 五、部署步骤

### 5.1 编译 eBPF 程序

```bash
# 进入 ja4t-daemon 目录
cd 000.CODE_SPACE/003.JA4T_PLUGIN

# 编译 eBPF (需要 Linux + bpf-linker)
cargo build --release
```

### 5.2 部署 ja4t-daemon

```bash
# 启动 daemon (会自动加载 2 个 TC 程序)
./target/release/ja4t-daemon \
    --interface eth0 \
    --socket /var/run/ja4t.sock \
    --poll-interval 5
```

### 5.3 Nginx 配置更新

Nginx 配置已自动包含新增的 header（见 `cc_manage.nginx-v2.0.0.conf`）。
重新加载 Nginx：
```bash
nginx -s reload
```

### 5.4 后端部署

```bash
# 编译后端
cd 000.CODE_SPACE/001.APP_SERVER
cargo build --release
```

> **注意**: JA4S 基线使用内存存储 (LazyLock<RwLock<HashMap>>)，无需额外创建数据库表。
> 基线自动维护: 观测次数 ≥ 10 → 自动验证; 30 天未出现 → 淘汰; 最大 50,000 条记录。

### 5.5 验证

```bash
# 1. 验证 BPF Map 中有数据
bpftool map dump name tcp_options_map | head -5
bpftool map dump name tls_server_hello_map | head -5

# 2. 验证 Nginx 透传
curl -s -o /dev/null -D - https://your-server/ | grep -i ja4

# 3. 检查 debug 计数器
bpftool map dump name debug_counters
bpftool map dump name ja4s_debug_counters
```

## 六、监控指标

| 指标 | 来源 | 说明 |
|------|------|------|
| Handler block rate | HANDLER_BLOCK_STATS | ja4l_handler / ja4s_handler 的拦截率 |
| BPF Map entry count | ja4t-daemon 日志 | 当前活跃的 TCP 连接指纹数 |
| SYN-ACK match rate | debug_counters[9]/[8] | SYN-ACK 反向匹配成功率 |
| Server Hello capture rate | ja4s_debug_counters[4] | 成功写入 tls_server_hello_map 的次数 |
| JA4L score distribution | 应用日志 | 按分数分桶统计 |
| JA4S baseline size | 内存 (RwLock<HashMap>) | 已验证配对条目数，上限 50,000 |

## 七、当前实现状态

> 2026-07-08: daemon 代码已重构为模块化架构 (ja4s/ + ja4t/ 子目录)，总代码量 ~2560 行。

| 组件 | 状态 | 文件 |
|------|------|------|
| eBPF TC Ingress (SYN + TTL + RTT) | ✅ 已实现 | `ebpf/tcp_options.bpf.c` (423行) |
| eBPF TC Egress (Server Hello) | ✅ 已实现 | `ebpf/tls_server_hello.bpf.c` (422行) |
| Daemon TCP 模块 | ✅ 已实现 | `ja4t/` (6 files, 362行) |
| Daemon JA4S 计算模块 | ✅ 已实现 | `ja4s/` (7 files, 752行) |
| Daemon Unix Socket 服务 | ✅ 已实现 | `server.rs` (286行) |
| Daemon 协议定义 | ✅ 已实现 | `protocol.rs` (106行) |
| Daemon CLI | ✅ 已实现 | `cli.rs` (32行) |
| Nginx JA4T/JA4H/JA4/JA4L/JA4S 注入 | ✅ 已实现 | `cc_manage.nginx-v2.0.0.conf` |
| Ja4FingerprintHandler (JA4+JA4H+JA4T) | ✅ 已实现 | `ja4_fingerprint_handler.rs` |
| Ja4LHandler (TTL+RTT+MSS) | ✅ 已实现 | `ja4l_handler.rs` (12条规则) |
| Ja4SHandler (JA4-JA4S 配对) | ✅ 已实现 | `ja4s_handler.rs` + 内存基线 |
| Cookie 一致性检测 | ✅ 已实现 | `cookie_consistency.rs` |
| JA4 解析统一模块 | ✅ 已实现 | `ja4_parse.rs` (共享模块) |
| TimezoneCrossValidationHandler | ✅ 已实现 | `timezone_cross_validation_handler.rs` |
| LanguageHandler Geo 交叉验证 | ✅ 已实现 | `language_handler.rs` (IP Geo↔语言) |

## 八、降级策略

| 故障场景 | 处理方式 |
|---------|---------|
| daemon 不可用 | ja4t/ja4l/ja4s 字段为 None, Handler 返回 None → 跳过 |
| BPF Map 满 (64K LRU 淘汰) | 最老条目被淘汰, 部分连接缺少数据 |
| SYN-ACK 未观测 (synack_ns=0) | RTT 为空, RTT 规则跳过 |
| IP 地理数据不可用 | RTT vs Geo 规则跳过 |
| JA4S 基线为空 (冷启动) | S-3 规则跳过, 仅 S-1/S-2/S-4/S-5 生效 |
