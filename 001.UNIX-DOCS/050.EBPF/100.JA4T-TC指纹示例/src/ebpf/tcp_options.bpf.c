// SPDX-License-Identifier: GPL-2.0
//
// tcp_options.bpf.c — JA4T TCP 指纹 eBPF TC 分类器
//
// 挂载到网卡 ingress 路径，被动观测每个入站 TCP SYN 包，提取：
//   a. TCP Window Size（窗口大小）
//   b. Raw TCP Options（原始选项字节，解析由用户态完成）
//
// 数据存入 LRU Hash Map（按五元组索引），由用户态 ja4t-daemon 轮询读取，
// 最终通过 Unix Socket 提供给 nginx 使用。

// ============================================================================
// 自包含类型定义 —— 不依赖系统头文件，确保可重复构建
// ============================================================================

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;
typedef unsigned long long __u64;
typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __u64 u64;

// ---- BPF 辅助函数 ID -------------------------------------------------------
#define BPF_FUNC_map_lookup_elem 1   // bpf_map_lookup_elem —— 按 key 查 Map
#define BPF_FUNC_map_update_elem 2   // bpf_map_update_elem —— 更新/插入 Map
#define BPF_FUNC_ktime_get_ns 5      // bpf_ktime_get_ns —— 获取纳秒时间戳
#define BPF_FUNC_skb_load_bytes 26   // bpf_skb_load_bytes —— 从 skb 读取指定偏移的字节

// ---- BPF 辅助函数声明（函数指针方式） ----------------------------------------
static void *(*bpf_map_lookup_elem)(void *map, const void *key) =
    (void *)BPF_FUNC_map_lookup_elem;

static u64 (*bpf_ktime_get_ns)(void) =
    (void *)BPF_FUNC_ktime_get_ns;

static long (*bpf_skb_load_bytes)(const void *ctx, u32 offset,
                                  void *to, u32 len) = (void *)BPF_FUNC_skb_load_bytes;

static long (*bpf_map_update_elem)(void *map, const void *key,
                                   const void *value, u64 flags) = (void *)BPF_FUNC_map_update_elem;

// ---- 编译器属性 --------------------------------------------------------------
#define __always_inline __attribute__((always_inline)) inline

// ---- BPF Map 标志位 ---------------------------------------------------------
#define BPF_F_NO_PREALLOC 1  // LRU Map 不预分配内存
#define BPF_ANY 0            // map_update_elem: 存在则更新，不存在则插入

// ---- BPF Map 类型宏（BTF 风格声明） ------------------------------------------
// SEC(name) 直接使用 __attribute__((section(name), used)) 代替宏
#define __uint(name, val) int (*name)[val]                   // 声明 Map 的整数属性
#define __type(name, val) typeof(val) *name                  // 声明 Map 的 key/value 类型

// ---- 调试计数器索引 ----------------------------------------------------------
#define COUNTER_TOTAL       0  // 总包数
#define COUNTER_IPV4        1  // 识别为 IPv4 的包数
#define COUNTER_TCP         2  // TCP 包数
#define COUNTER_SYN         3  // SYN 包数（仅 SYN=1, ACK=0）
#define COUNTER_MAP_OK      4  // 成功写入 BPF Map 的次数
#define COUNTER_ETHERTYPE   5  // 诊断：首次检测到的 IP 偏移（用于判断链路层类型）
#define COUNTER_PROTO       6  // 诊断：首次看到的 IP 协议字段
#define COUNTER_TCP_START   7  // 诊断：TCP 头部在包中的起始偏移
#define COUNTER_SYNACK      8  // SYN-ACK 包捕获计数
#define COUNTER_SYNACK_OK   9  // SYN-ACK 反向匹配成功的次数
#define DEBUG_COUNTERS      10

// ---- TC 返回码 ---------------------------------------------------------------
#define TC_ACT_OK 0           // 处理完成（通常不用）
#define TC_ACT_SHOT 2         // 丢弃包
#define TC_ACT_UNSPEC -1      // 不做任何处理，透传（观测模式用这个）

// ---- 以太网 / IP / TCP 常量 --------------------------------------------------
#define ETH_P_IP 0x0800       // EtherType: IPv4
#define ETH_HDR_LEN 14        // 标准以太网头长度（不含 VLAN）
#define IPPROTO_TCP 6         // IP 协议号: TCP

// TCP 标志位（TCP 头第 13 字节）
#define TCPHDR_FIN 0x01       // 连接结束
#define TCPHDR_SYN 0x02       // 同步序列号（握手开始）
#define TCPHDR_RST 0x04       // 连接重置
#define TCPHDR_PSH 0x08       // 推送数据
#define TCPHDR_ACK 0x10       // 确认应答
#define TCPHDR_URG 0x20       // 紧急指针有效
#define TCPHDR_ECE 0x40       // ECN Echo
#define TCPHDR_CWR 0x80       // 拥塞窗口减小

#define TCP_HDR_FIXED_LEN 20  // TCP 头固定长度（不含 Option）
#define MAX_RAW_OPTS 40       // 最大原始 Option 字节数

// ---- 共享结构体（必须与 tcp_options.h 及用户态完全一致） -----------------------
struct connection_tuple
{
    u32 src_ip;       // 源 IP（网络字节序）
    u32 dst_ip;       // 目的 IP（网络字节序）
    u16 src_port;     // 源端口（网络字节序）
    u16 dst_port;     // 目的端口（网络字节序）
    u8 protocol;      // 协议号，6 = TCP
    u8 padding[3];    // 对齐填充
};

struct tcp_options_info
{
    u16 window_size;          // offset 0: 窗口大小
    u8 raw_opts_len;          // offset 2: 有效 Option 字节数
    u8 ip_ttl;                // offset 3: IP TTL（取代原 _pad[1]）
    u8 raw_opts[MAX_RAW_OPTS];// offset 4: 原始 Option 字节
    u8 _pad2[4];              // offset 44: 对齐到 8 字节边界
    u64 capture_ns;           // offset 48: SYN 捕获时间戳（纳秒）
    u64 synack_ns;            // offset 56: SYN-ACK 到达时间戳（纳秒），0 表示未捕获
};

// ---- __sk_buff 完整定义（BPF 验证器要求完整定义，不能前向声明） ---------------
struct bpf_flow_keys;
struct bpf_sock;

struct __sk_buff
{
    __u32 len;             // 包总长度
    __u32 pkt_type;        // 包类型
    __u32 mark;            // netfilter mark
    __u32 queue_mapping;   // 队列映射
    __u32 protocol;        // 协议
    __u32 vlan_present;    // VLAN 标记存在
    __u32 vlan_tci;        // VLAN TCI
    __u32 vlan_proto;      // VLAN 协议
    __u32 priority;        // 优先级
    __u32 ingress_ifindex; // 入口接口索引
    __u32 ifindex;         // 接口索引
    __u32 tc_index;        // TC 索引
    __u32 cb[5];           // 回调数据
    __u32 hash;            // 哈希值
    __u32 tc_classid;      // TC classid
    __u32 data;            // 数据起始
    __u32 data_end;        // 数据结束
    __u32 napi_id;         // NAPI ID
    __u32 family;          // 地址族
    __u32 remote_ip4;      // 远端 IPv4
    __u32 local_ip4;       // 本地 IPv4
    __u32 remote_ip6[4];   // 远端 IPv6
    __u32 local_ip6[4];    // 本地 IPv6
    __u32 remote_port;     // 远端端口
    __u32 local_port;      // 本地端口
    __u32 data_meta;       // 数据元
    union {
        struct bpf_flow_keys *flow_keys;
    };
    __u64 tstamp;          // 硬件时间戳
    __u32 wire_len;        // 线缆上的长度
    __u32 gso_segs;        // GSO 分段数
    union {
        struct bpf_sock *sk; // socket 指针
    };
    __u32 gso_size;        // GSO 大小
    __u8 tstamp_type;      // 时间戳类型
    __u64 hwtstamp;        // 硬件时间戳
};

// ============================================================================
// BPF Map 定义
// ============================================================================

// TCP 连接数据 Map —— LRU Hash，按五元组索引，最多 65536 条
struct
{
    __uint(type, 9);         // BPF_MAP_TYPE_LRU_HASH（最近最少使用哈希表）
    __uint(max_entries, 65536);  // 最大条目数
    __type(key, struct connection_tuple);    // Key: 五元组
    __type(value, struct tcp_options_info);  // Value: TCP 指纹数据
} tcp_options_map __attribute__((section(".maps"), used));

// 调试计数器 Map —— 数组类型，8 个 u64，用于诊断包处理流程
struct
{
    __uint(type, 2);         // BPF_MAP_TYPE_ARRAY
    __uint(max_entries, DEBUG_COUNTERS);  // 8 个计数器
    __type(key, u32);        // Key: 计数器索引（0-7）
    __type(value, u64);      // Value: 计数值
} debug_counters __attribute__((section(".maps"), used));

// ============================================================================
// 辅助函数：从字节数组按大端序加载 u16 / u32
// ============================================================================

// 从字节指针 p 读取一个大端序的 u16
static __always_inline u16 load_be16(const u8 *p)
{
    return ((u16)p[0] << 8) | p[1];
}

// 从字节指针 p 读取一个大端序的 u32
static __always_inline u32 load_be32(const u8 *p)
{
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
}

// ---- 调试计数器原子递增 ------------------------------------------------------
// 使用 __sync_fetch_and_add 保证原子性，避免多 CPU 并发计数丢失
static __always_inline void inc_counter(u32 idx)
{
    u64 *val = bpf_map_lookup_elem(&debug_counters, &idx);
    if (val)
        __sync_fetch_and_add(val, 1);
}

// ============================================================================
// TC ingress 处理入口 —— 对每个入站包进行分类处理
//
// 处理两类 TCP 包：
//   1. 纯 SYN (SYN=1, ACK=0)：提取 TCP 指纹 + IP TTL → 写入 Map
//   2. SYN-ACK (SYN=1, ACK=1)：反向查找已有 SYN 条目 → 更新 synack_ns
// ============================================================================

__attribute__((section("classifier"), used))
int ja4t_parse_syn(struct __sk_buff *skb)
{
    struct tcp_options_info info = {0};  // Value: 零初始化
    struct connection_tuple key = {0};   // Key:  零初始化

    // 总包计数
    inc_counter(COUNTER_TOTAL);

    // ========================================================================
    // 第 1 步：P0F 风格的 IP 头定位 —— 在偏移 0..38 扫描 IPv4 版本号
    //
    // 不假设链路层一定是 14 字节以太网头，而是扫描可能的偏移范围，
    // 兼容以下链路层格式：
    //   - 标准以太网（14 字节）
    //   - WiFi 802.11 头（不等长）
    //   - 带填充的 USB WiFi 适配器
    //   - 原始 IP（0 字节 Layer 2）
    //   - VLAN 标记（18 字节）+ 各种偏移
    //   - PPP / 隧道封装
    // ========================================================================
    u32 ip_off = 0;

#pragma clang loop unroll(full)  // 提示编译器完全展开循环（每次迭代步长 2）
    for (u32 off = 0; off < 40; off += 2)
    {
        // 已找到 IP 头位置，跳过后续偏移
        if (ip_off != 0)
            continue;

        // 读取 12 字节探测数据: byte[0] = version/ihl, byte[9] = protocol
        u8 peek[12];
        if (bpf_skb_load_bytes(skb, off, peek, sizeof(peek)) < 0)
            continue;

        // 判定条件: 高半字节 == 4 (IPv4) 且 协议字段 == TCP
        if ((peek[0] >> 4) == 4 && peek[9] == IPPROTO_TCP)
        {
            // 确认是 IPv4+TCP，重新读取完整的 IP 头（20 字节）
            u8 ip_hdr[20];
            if (bpf_skb_load_bytes(skb, off, ip_hdr, sizeof(ip_hdr)) < 0)
                continue;

            // 二次校验：IHL（Internet Header Length）>= 5（即 >= 20 字节）
            u8 ihl = ip_hdr[0] & 0x0f;
            if (ihl < 5)
                continue;

            // ---- 分片包跳过 -------------------------------------------------
            // IP 分片后 TCP 头可能不在本片中，无法可靠解析
            // ip_hdr[6-7] = Flags(3bit) + Fragment Offset(13bit)
            // MF=1 (bit 13) 或 Fragment Offset > 0 表示分片
            u16 frag_off = load_be16(ip_hdr + 6);
            if ((frag_off & 0x3FFF) != 0)
                continue;  // 分片包，跳过

            ip_off = off;

            // 诊断：记录首次检测到的 IP 偏移（用于判断链路层类型）
            {
                u32 idx = COUNTER_ETHERTYPE;
                u64 *v = bpf_map_lookup_elem(&debug_counters, &idx);
                if (v && *v == 0)
                    *v = ip_off;
            }

            inc_counter(COUNTER_IPV4);

            // ---- 提取源 IP 和目的 IP（网络字节序）---------------------------
            key.src_ip = load_be32(ip_hdr + 12);
            key.dst_ip = load_be32(ip_hdr + 16);

            // ---- 提取 IP TTL（IPv4 头第 8 字节）-----------------------------
            // TTL 用于推断客户端 OS 类型和网络跳数：
            //   观察值 ≤ 64   → 初始 TTL=64  (Linux/macOS/iOS/Android)
            //   观察值 65-128 → 初始 TTL=128 (Windows)
            //   观察值 > 128  → 初始 TTL=255 (Network Device)
            //   跳数 = 初始 - 观察
            u8 ttl = ip_hdr[8];

            // 诊断：记录 IP 协议字段
            {
                u32 idx = COUNTER_PROTO;
                u64 *v = bpf_map_lookup_elem(&debug_counters, &idx);
                if (v && *v == 0)
                    *v = ip_hdr[9];
            }

            inc_counter(COUNTER_TCP);

            // IP 头长度 = IHL * 4 字节; TCP 起始位置 = IP 偏移 + IP 头长度
            u8 ip_hdr_len = ihl * 4;
            u32 tcp_start = off + ip_hdr_len;

            // 诊断：记录 TCP 头在包中的起始偏移
            {
                u32 idx = COUNTER_TCP_START;
                u64 *v = bpf_map_lookup_elem(&debug_counters, &idx);
                if (v && *v == 0)
                    *v = tcp_start;
            }

            // ---- 第 2 步：加载 TCP 头（20 字节固定部分）-----------------------
            u8 tcp_buf[20];
            if (bpf_skb_load_bytes(skb, tcp_start, tcp_buf, sizeof(tcp_buf)) < 0)
                continue;

            // ---- 提取源端口和目的端口 -----------------------------------------
            key.src_port = load_be16(tcp_buf + 0);
            key.dst_port = load_be16(tcp_buf + 2);
            key.protocol = IPPROTO_TCP;

            // Data Offset: 第 12 字节的高半字节（以 4 字节为单位）
            u8 data_offset = tcp_buf[12] >> 4;
            if (data_offset < 5)
                continue;

            // TCP 标志位：第 13 字节
            u8 flags = tcp_buf[13];

            // ================================================================
            // 分支 A: 纯 SYN 包 (SYN=1, ACK=0) — 提取 TCP 指纹 + TTL
            // ================================================================
            if ((flags & TCPHDR_SYN) && !(flags & TCPHDR_ACK))
            {
                inc_counter(COUNTER_SYN);

                // ---- 提取 TCP 窗口大小 -----------------------------------
                info.window_size = load_be16(tcp_buf + 14);

                // ---- 记录 IP TTL -----------------------------------------
                info.ip_ttl = ttl;

                // ---- 提取原始 TCP Option 字节（留给用户态解析）-----------
                u32 tcp_hdr_len = (u32)data_offset * 4u;
                if (tcp_hdr_len > TCP_HDR_FIXED_LEN && tcp_hdr_len <= 60)
                {
                    u32 opts_len = tcp_hdr_len - TCP_HDR_FIXED_LEN;
                    if (opts_len > MAX_RAW_OPTS)
                        opts_len = MAX_RAW_OPTS;  // 截断到 40 字节
                    if (opts_len <= MAX_RAW_OPTS)
                    {
                        info.raw_opts_len = (u8)opts_len;
                        bpf_skb_load_bytes(skb, tcp_start + TCP_HDR_FIXED_LEN,
                                           info.raw_opts, opts_len);
                    }
                }

                // ---- 记录捕获时间戳（纳秒）-------------------------------
                info.capture_ns = bpf_ktime_get_ns();
                // synack_ns 保持 0（零初始化），后续 SYN-ACK 到达时更新

                // ---- 写入 BPF LRU Hash Map ------------------------------
                bpf_map_update_elem(&tcp_options_map, &key, &info, BPF_ANY);
                inc_counter(COUNTER_MAP_OK);
            }

            // ================================================================
            // 分支 B: SYN-ACK 包 (SYN=1, ACK=1) — 反向查找并更新 synack_ns
            //
            // TCP 三次握手: client → SYN → server → SYN-ACK → client
            // 入站方向看，SYN 是 client→server，SYN-ACK 是 server→client
            // 但实际上 SYN-ACK 是 server 发给 client 的，在 ingress 方向
            // 看到的是出站包的回环（或另一端）。这里我们用反向 5-tuple 匹配。
            //
            // 反向匹配: SYN 的 {src=client, dst=server}
            //           SYN-ACK 的 {src=server, dst=client}
            // → 交换 src/dst IP 和端口来查找
            // ================================================================
            if ((flags & TCPHDR_SYN) && (flags & TCPHDR_ACK))
            {
                inc_counter(COUNTER_SYNACK);

                // 构建反向 5-tuple: 交换 src/dst
                struct connection_tuple rev_key = {0};
                rev_key.src_ip = key.dst_ip;
                rev_key.dst_ip = key.src_ip;
                rev_key.src_port = key.dst_port;
                rev_key.dst_port = key.src_port;
                rev_key.protocol = IPPROTO_TCP;

                // 在 BPF Map 中查找原始 SYN 条目
                struct tcp_options_info *existing =
                    bpf_map_lookup_elem(&tcp_options_map, &rev_key);

                // synack_ns == 0 的检查确保：
                //   - 只记录第一个 SYN-ACK 的到达时间（最准确的 RTT）
                //   - 防止 TCP 重传 SYN-ACK 覆盖首次测量
                //   - 防止并发 SYN-ACK（多核）导致数据竞争
                if (existing && existing->synack_ns == 0)
                {
                    existing->synack_ns = bpf_ktime_get_ns();
                    inc_counter(COUNTER_SYNACK_OK);
                    // 注意：bpf_map_lookup_elem 返回指向 Map 值内存的指针，
                    // 修改指针内容即直接修改 Map 中的值（LRU Hash 支持），
                    // 无需调用 bpf_map_update_elem。
                }
            }
        }
    }

    // 返回 TC_ACT_UNSPEC —— 不做任何修改，透传所有包（纯观测模式）
    return TC_ACT_UNSPEC;
}

// ============================================================================
// License（内核要求，缺少 GPL 声明会导致 BPF 程序加载失败）
// ============================================================================
char _license[] __attribute__((section("license"), used)) = "GPL";
