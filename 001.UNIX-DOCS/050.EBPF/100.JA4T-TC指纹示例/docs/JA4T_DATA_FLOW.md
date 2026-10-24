> 📌 迁移自 `Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/docs/JA4T_DATA_FLOW.md`(2026-10-24);导读见 [000.导读.md](../000.导读.md)。

# JA4T Plugin 数据流程图

> 最后更新: 2026-07-07
> 
> 本文档描述 CYBERCLOAK JA4T/JA4S 系统的端到端数据流。
> 从网络数据包到最终验证决策，跨越 5 层处理。

## 一、端到端数据流 (Packet → Fingerprint → Decision)

```mermaid
flowchart TD
    subgraph L1[第一层: 数据采集 - eBPF Kernel]
        A1[网卡 eth0] --> A2{TC Hook}
        A2 -->|ingress| A3[eBPF: ja4t_parse_syn<br/>tcp_options.bpf.c]
        A2 -->|egress| A3S[eBPF: ja4s_parse_server_hello<br/>tls_server_hello.bpf.c]
        A3 --> A4{p0f-style Offset 扫描<br/>0..38 step 2}
        A4 -->|匹配 IPv4+TCP| A5{协议过滤}
        A5 -->|SYN=1 ACK=0| A6[提取数据: window_size + raw_opts + ip_ttl]
        A5 -->|SYN=1 ACK=1| A7[反向5-tuple匹配 → 更新 synack_ns]
        A6 --> A8[(tcp_options_map<br/>LRU Hash 65K)]
        A7 --> A8
        A3S --> A9[TLS Record解析<br/>Server Hello → cipher + ALPN + extensions]
        A9 --> A10[(tls_server_hello_map<br/>LRU Hash 65K)]
    end

    subgraph L2[第二层: 数据处理 - ja4t-daemon User Space]
        B1[Poll Task 5s] --> B2[BpfReader::dump_all]
        B2 --> A8
        B2 --> A10
        A8 --> B3[parse_tcp_options<br/>MSS/WindowScale/SACK/Order]
        A10 --> B4[compute_ja4s<br/>TLS版本/密码套件/扩展hash]
        B3 --> B5[ConnectionCache<br/>内存 HashMap]
        B4 --> B6[ServerHelloCache<br/>内存 HashMap]
    end

    subgraph L3[第三层: 查服务 - Unix Socket IPC]
        C1[NGINX Worker] --> C2[构建 LookupRequest JSON<br/>src_ip, dst_ip, src_port, dst_port]
        C2 --> C3[/var/run/ja4t.sock]
        C3 --> C4{快路径: 内存缓存}
        C4 -->|命中| C5[构建 LookupResponse]
        C4 -->|未命中| C6[回退: BPF Map 直查]
        C6 --> C5
        C5 --> C7[JSON响应: found, mss, ws, sack, opts, ip_ttl, rtt_us, ja4s]
    end

    subgraph L4[第四层: 指纹构建与传递 - NGINX]
        D1[接收响应] --> D2[缓存 found:true 600s]
        D2 --> D3[构建 JA4T 指纹:<br/>window_mss_scale_sack_opts]
        D1 --> D4[提取 ip_ttl / rtt_us / ja4s]
        D3 --> D5[proxy_set_header<br/>X-JA4T-Fingerprint]
        D4 --> D6[proxy_set_header<br/>X-JA4T-TTL<br/>X-JA4T-RTT<br/>X-JA4S-Fingerprint]
    end

    subgraph L5[第五层: 验证决策 - App Server (22 Handlers)]
        E1[接收 Headers] --> E2[VerificationContext 构建]
        E2 --> E3[验证链短路执行]
        E3 --> E4[Phase 1: 无IP依赖规则]
        E4 --> E4A[Ja4FingerprintHandler<br/>JA4+JA4H+JA4T + Cookie变异]
        E4A --> E4B[BrowserFingerprintHandler]
        E4B --> E4C[UserAgentBlockHandler]
        E4C --> E4D[HttpHeaderBlockHandler]
        E4D --> E4E[Ja4LHandler<br/>TTL OS推断 + RTT距离 + MSS语义]
        E4E --> E5[Phase 2: IP相关规则]
        E5 --> E5A[IpInfo/Google/FB/Amazon Block]
        E5A --> E5B[IP White/Blacklist + ASN]
        E5B --> E5C[Crawler + Proxy + Datacenter + Mobile]
        E5C --> E5D[Ja4SHandler<br/>TLS响应指纹 + JA4配对基线]
        E5D --> E5E[Device/Language/Region + Default]
        E5E --> E6{首个 BLOCK 命中?}
        E6 -->|是| E7[BLOCK]
        E6 -->|否| E8[ALLOW]
    end

    L1 --> L2
    L2 --> L3
    L3 --> L4
    L4 --> L5
```

## 二、eBPF 数据采集详细流程

### 2.1 纯 SYN 处理流程

```mermaid
flowchart TD
    START([inbound SYN packet]) --> INC_TOTAL["debug_counters[0]++"]
    INC_TOTAL --> SCAN["offset 0..38 step 2: 扫描 IPv4 header"]
    SCAN -->|找到| CHECK{"IHL >= 5 AND protocol=6?"}
    SCAN -->|未找到| DROP
    CHECK -->|否| DROP
    CHECK -->|是| FRAG{"frag_off & 0x3FFF != 0?"}
    FRAG -->|是| DROP["跳过分片包"]
    FRAG -->|否| EXTRACT["提取: src_ip, dst_ip, ip_ttl"]
    EXTRACT --> TCP_PARSE["读取 TCP header 20 bytes"]
    TCP_PARSE --> SYN_CHECK{"SYN=1 AND ACK=0?"}
    SYN_CHECK -->|SYN=1, ACK=0| EXTRACT_DATA["提取: window_size, raw_opts[40]"]
    SYN_CHECK -->|SYN=1, ACK=1| SYNACK["构建反向5-tuple → 查找已有SYN条目"]
    EXTRACT_DATA --> TIMESTAMP["capture_ns = bpf_ktime_get_ns()"]
    TIMESTAMP --> WRITE_MAP["写入 tcp_options_map"]
    SYNACK -->|找到| UPDATE_RTT["existing->synack_ns = bpf_ktime_get_ns()<br/>(仅首次, synack_ns==0检查)"]
    SYNACK -->|未找到| DROP
    WRITE_MAP --> PASSTHROUGH[return TC_ACT_UNSPEC]
    UPDATE_RTT --> PASSTHROUGH
    DROP --> PASSTHROUGH
    PASSTHROUGH --> END([packet continues])
```

### 2.2 TLS Server Hello 处理流程

```mermaid
flowchart TD
    START([egress packet]) --> PORT_CHECK{"src_port == 443?"}
    PORT_CHECK -->|否| DROP
    PORT_CHECK -->|是| TLS_CHECK{"TLS Record Type == 0x16?"}
    TLS_CHECK -->|否| DROP
    TLS_CHECK -->|是| HS_CHECK{"Handshake Type == 0x02 (Server Hello)?"}
    HS_CHECK -->|否| DROP
    HS_CHECK -->|是| PARSE["解析 ServerHello body"]
    PARSE --> SKIP["跳过 Version(2)+Random(32)+SessionID(v)"]
    SKIP --> CS["读取 CipherSuite(2)"]
    CS --> CM["读取 CompressionMethod(1)"]
    CM --> EXT_LOOP{"遍历 Extensions"}
    EXT_LOOP -->|每个ext| EXT_TYPE["记录 type 到 ext_raw<br/>(原始顺序, 含GREASE)"]
    EXT_LOOP -->|type=0x0010| ALPN["提取 ALPN 协议名"]
    EXT_LOOP -->|循环结束| TIMESTAMP["capture_ns"]
    TIMESTAMP --> WRITE_MAP["写入 tls_server_hello_map"]
    WRITE_MAP --> PASSTHROUGH[return TC_ACT_UNSPEC]
    DROP --> PASSTHROUGH
    PASSTHROUGH --> END([packet continues])
```

## 三、TCP 选项解析流程

```mermaid
flowchart TD
    INPUT["input: raw_opts[40], raw_opts_len"] --> INIT["idx=0, mss=None, ws=None, sack=None, order=[]"]
    INIT --> LOOP{"idx < raw_opts_len<br/>AND order.len() < 12?"}
    LOOP -->|否| RETURN["return ParsedTcpOptions"]
    LOOP -->|是| KIND["kind = raw_opts[idx]"]

    KIND -->|"0 (EOL)"| EOL["终止解析"]
    KIND -->|"1 (NOP)"| NOP["记录到 order, idx+=1"]
    KIND -->|"2 (MSS) len=4"| MSS["mss = be16(raw_opts[idx+2..+4])<br/>记录到 order, idx+=4"]
    KIND -->|"3 (Window Scale) len=3"| WS["ws = raw_opts[idx+2]<br/>记录到 order, idx+=3"]
    KIND -->|"4 (SACK Perm) len=2"| SACK["sack=1, 记录到 order, idx+=2"]
    KIND -->|"8 (Timestamp)"| TS["记录到 order, idx+=len"]
    KIND -->|其他| OTHER["记录到 order, idx+=len"]

    EOL --> LOOP
    NOP --> LOOP
    MSS --> LOOP
    WS --> LOOP
    SACK --> LOOP
    TS --> LOOP
    OTHER --> LOOP
```

### 平台特征 TCP Option Order

| 平台 | Option Order | 说明 |
|------|-------------|------|
| Linux (含 Android) | 2-4-8-1-3 | mss, sack, ts, nop, ws |
| Windows 7+ | 2-1-3-4-8 | mss, nop, ws, sack, ts |
| Apple (macOS/iOS) | 2-1-3-1-1-8-4[-0] | mss, nop, ws, nop, nop, ts, sack[, eol] |

## 四、查时序 (Cache Hit vs Miss)

```mermaid
sequenceDiagram
    participant PKT as TCP SYN Packet
    participant BPF as BPF Map (Kernel)
    participant POLL as Poll Task (5s)
    participant CACHE as ConnectionCache (Memory)
    participant NGX as NGINX Worker
    participant SOCK as Unix Socket Server

    Note over PKT,SOCK: 场景A: 缓存命中 (正常路径, ~μs)

    PKT->>BPF: SYN 到达, 写入 BPF Map
    POLL->>BPF: dump_all() (5s周期)
    BPF-->>POLL: 返回所有条目
    POLL->>CACHE: 全量替换缓存
    NGX->>SOCK: LookupRequest JSON
    SOCK->>CACHE: lookup(key)
    CACHE-->>SOCK: ✅ 命中
    SOCK-->>NGX: LookupResponse {found:true, ...}

    Note over PKT,SOCK: 场景B: 缓存未命中 (竞态窗口, ~100μs)

    PKT->>BPF: SYN 到达 (刚过 poll)
    NGX->>SOCK: LookupRequest JSON
    SOCK->>CACHE: lookup(key) ❌ 未命中
    SOCK->>BPF: lookup_direct(key)
    BPF-->>SOCK: ✅ 命中
    SOCK-->>NGX: LookupResponse {found:true, ...}

    Note over PKT,SOCK: 场景C: 连接不存在

    NGX->>SOCK: LookupRequest JSON
    SOCK->>CACHE: lookup(key) ❌
    SOCK->>BPF: lookup_direct(key) ❌
    SOCK-->>NGX: LookupResponse {found:false}
```

## 五、跨层指纹交叉验证矩阵

> 以下矩阵对应后端验证链中的实际 Handler。所有信号已实现并通过 Nginx headers 注入。

```mermaid
flowchart LR
    subgraph SIGNALS[多层信号采集 — 全部已实现]
        JA4T[JA4T — TCP Stack<br/>eBPF SYN 采集 ✅]
        JA4[JA4 — TLS ClientHello<br/>Nginx ssl_client_hello_by_lua ✅]
        JA4H[JA4H — HTTP Headers<br/>Nginx rewrite_by_lua ✅]
        JA4L[JA4L — TTL + RTT<br/>eBPF SYN + SYN-ACK ✅]
        JA4S[JA4S — TLS ServerHello<br/>eBPF Egress 采集 ✅]
        MSS[MSS — 网络路径特征<br/>JA4T 指纹中提取 ✅]
        CK[Cookie — 会话一致性<br/>跨请求对比 ✅]
        UA[User-Agent<br/>HTTP Header]
        CH[Sec-CH-UA-Platform<br/>Client Hints]
        FP[Browser Fingerprint<br/>JS SDK]
    end

    subgraph CROSS[交叉验证矩阵 — 对应 Handler]
        V1{JA4 ↔ JA4T} -->|"TLS库 vs TCP栈"| H1[Ja4FingerprintHandler]
        V2{JA4H ↔ JA4T} -->|"HTTP指纹 vs TCP栈"| H1
        V3{JA4 ↔ JA4S} -->|"ClientHello vs ServerHello"| H2[Ja4SHandler]
        V4{JA4L ↔ JA4T} -->|"TTL OS vs TCP OS"| H3[Ja4LHandler L-1/L-2]
        V5{JA4L ↔ IP Geo} -->|"RTT vs 物理距离"| H3[Ja4LHandler L-9/L-10]
        V6{UA ↔ JA4T} -->|"声称OS vs 探测OS"| H1
        V7{CH ↔ JA4T} -->|"ClientHints vs TCP"| H1
        V8{MSS ↔ IP Type} -->|"VPN/代理特征"| H3[Ja4LHandler L-20~L-24]
        V9{Cookie ↔ Session} -->|"会话一致性"| H1[Cookie 变异检测]
    end

    JA4T --> V1 & V2 & V4 & V6 & V7
    JA4 --> V1 & V3
    JA4H --> V2
    JA4L --> V4 & V5
    JA4S --> V3
    MSS --> V8
    CK --> V9
    UA --> V6
    CH --> V7
    FP --> H1
```

## 六、关键指标

| 指标 | 值 | 说明 |
|------|-----|------|
| BPF Map 类型 | LRU Hash × 2 | tcp_options_map + tls_server_hello_map |
| BPF Map 容量 | 65,536 × 2 | 最多同时追踪 65K TCP 连接 + 65K TLS 会话 |
| tcp_options_info 大小 | 64 bytes | 含 TTL + SYN-ACK 时间戳 (synack_ns) |
| tls_server_hello_info 大小 | 168 bytes | 含密码套件 + 扩展 + ALPN |
| 轮询周期 | 5 秒 (可配) | 用户态从 BPF Map 拉取数据的频率 |
| nginx 缓存 TTL | 600 秒 | 仅缓存 found=true 的结果 |
| 缓存 key | `src_ip->dst_ip:dst_port` | 不含 src_port (同一客户端 TCP 指纹稳定) |
| Unix Socket 权限 | 0666 | 允许所有 nginx worker 进程访问 |
| Unix Socket 超时 | 100ms | Nginx Lua settimeout |
| eBPF 扫描范围 | offset 0..38, step 2 | p0f 兼容的链路层头部扫描，最多 20 次探测 |
| TCP 选项最大字节 | 40 | 足以容纳常见 TCP 选项组合 |
| 响应延迟 (命中) | ~微秒级 | 纯内存 HashMap 查询 |
| 响应延迟 (未命中) | ~百微秒级 | 一次 bpf_map_lookup_elem 系统调用 |
| JA4S 指纹计算 | 用户态完成 (SHA256) | 避免 eBPF 中进行 SHA256 (指令数超限) |
| JA4S 基线存储 | 内存 HashMap | LazyLock<RwLock<HashMap>>, 上限 50,000 条, 30天 TTL |
| Cookie 会话缓存 | 内存 HashMap | Mutex<HashMap>, 上限 10,000 条, 30min TTL |
| 后端 Handler 总数 | 22 个 | 短路执行，首个命中立即返回 |
