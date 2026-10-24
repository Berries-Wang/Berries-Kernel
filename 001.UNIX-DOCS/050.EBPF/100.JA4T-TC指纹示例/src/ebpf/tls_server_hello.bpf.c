// SPDX-License-Identifier: GPL-2.0
//
// tls_server_hello.bpf.c — TLS Server Hello 指纹 eBPF TC 分类器
//
// 挂载到网卡 egress 路径，捕获服务器发出的 TLS Server Hello 包，提取：
//   a. TLS 版本（ServerHello.version）
//   b. 服务器选择的密码套件（单个 cipher）
//   c. 服务器选择的 ALPN 协议
//   d. 服务器返回的扩展列表（原始顺序，含 GREASE）
//
// 数据存入 LRU Hash Map，由 ja4t-daemon 轮询后在用户态计算 JA4S 指纹。
//
// JA4S 指纹格式: t{ver}{#exts}{alpn}_{cipher}_{ext_hash}
//
// 与 JA4 (Client Hello) 的关键差异：
//   - 密码套件：只取单个选中的 cipher（非列表 hash）
//   - 扩展计数：包含 GREASE（非排除）
//   - 扩展 hash：原始顺序、包含 GREASE（非排序、非过滤）

// ============================================================================
// 自包含类型定义（与 tcp_options.bpf.c 相同，确保独立编译）
// ============================================================================

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;
typedef unsigned long long __u64;
typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __u64 u64;

// ---- BPF 辅助函数 -----------------------------------------------------------
#define BPF_FUNC_map_lookup_elem 1
#define BPF_FUNC_map_update_elem 2
#define BPF_FUNC_ktime_get_ns 5
#define BPF_FUNC_skb_load_bytes 26

static void *(*bpf_map_lookup_elem)(void *map, const void *key) =
    (void *)BPF_FUNC_map_lookup_elem;
static long (*bpf_map_update_elem)(void *map, const void *key,
    const void *value, u64 flags) = (void *)BPF_FUNC_map_update_elem;
static u64 (*bpf_ktime_get_ns)(void) = (void *)BPF_FUNC_ktime_get_ns;
static long (*bpf_skb_load_bytes)(const void *ctx, u32 offset,
    void *to, u32 len) = (void *)BPF_FUNC_skb_load_bytes;

// ---- 编译器属性 --------------------------------------------------------------
#define __always_inline __attribute__((always_inline)) inline
// 直接使用 __attribute__((section(name), used)) 代替 SEC 宏
#define __uint(name, val) int (*name)[val]
#define __type(name, val) typeof(val) *name
#define BPF_ANY 0
#define BPF_F_NO_PREALLOC 1

// ---- 常量 -------------------------------------------------------------------
#define IPPROTO_TCP 6
#define TCPHDR_SYN 0x02
#define TCPHDR_ACK 0x10
#define TCPHDR_PSH 0x08

// TLS Record 类型常量
#define TLS_RECORD_HANDSHAKE 0x16
#define TLS_HANDSHAKE_SERVER_HELLO 0x02

// Server Hello 解析常量
#define MAX_EXTENSIONS 20      // Server Hello 中最多解析 20 个扩展
#define MAX_ALPN_LEN 32        // ALPN 协议名最大长度
#define MAX_EXT_RAW_BYTES 120  // 扩展 type 原始字节最大长度

// ---- 调试计数器 --------------------------------------------------------------
#define COUNTER_TOTAL          0  // 总包数
#define COUNTER_TCP_443        1  // TCP dst/src port 443 包数
#define COUNTER_TLS_HS         2  // TLS Handshake 记录数
#define COUNTER_SERVER_HELLO   3  // Server Hello 包数
#define COUNTER_MAP_OK         4  // 成功写入 BPF Map 的次数
#define COUNTER_ALPN_FOUND     5  // 有 ALPN 扩展的 Server Hello 数
#define COUNTER_EXT_OVERFLOW   6  // 扩展数超过 MAX_EXTENSIONS 的次数
#define COUNTER_FRAG_SKIP      7  // 跳过的分片包数
#define DEBUG_COUNTERS         8

// ---- TC 返回码 ---------------------------------------------------------------
#define TC_ACT_OK 0
#define TC_ACT_SHOT 2
#define TC_ACT_UNSPEC -1

// ---- 共享结构体（与 daemon 侧 ja4s.rs 保持一致） -----------------------------

/// 连接五元组 Key（与 tcp_options_map 的 Key 格式相同，方便关联）
struct ja4s_connection_key
{
    u32 src_ip;       // 服务器 IP（网络字节序）
    u32 dst_ip;       // 客户端 IP（网络字节序）
    u16 src_port;     // 服务器端口（通常是 443）
    u16 dst_port;     // 客户端端口
    u8 protocol;      // 6 = TCP
    u8 padding[3];    // 对齐到 16 字节
};

/// TLS Server Hello 指纹原始数据
///
/// 用户态 ja4t-daemon 轮询此 Map 后在 Rust 中计算 JA4S 指纹，
/// 避免在 eBPF 中进行 SHA256 计算（SHA256 指令数远超 eBPF 限制）。
struct tls_server_hello_info
{
    u16 tls_version;           // TLS 版本（0x0304=TLS1.3, 0x0303=TLS1.2）
    u16 cipher_suite;          // 服务器选择的密码套件（单个）
    u8  extension_count;       // 扩展数量（含 GREASE）
    u8  alpn_len;              // ALPN 协议名长度（0 表示无 ALPN 扩展）
    u8  alpn[MAX_ALPN_LEN];    // ALPN 协议名（如 "h2", "http/1.1"）
    u8  ext_raw_len;           // ext_raw 中的有效字节数
    u8  ext_raw[MAX_EXT_RAW_BYTES]; // 扩展 type 原始字节（每扩展 2 字节 type，原始顺序）
    u8  _pad[9];               // 对齐 capture_ns 到 8 字节边界（offset 168）
    u64 capture_ns;            // 捕获时间戳
}; // 总大小: 2+2+1+1+32+1+120+9+8 = 176 字节

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

// TLS Server Hello 数据 Map
struct
{
    __uint(type, 9);         // BPF_MAP_TYPE_LRU_HASH
    __uint(max_entries, 65536);
    __type(key, struct ja4s_connection_key);
    __type(value, struct tls_server_hello_info);
} tls_server_hello_map __attribute__((section(".maps"), used));

// 调试计数器
struct
{
    __uint(type, 2);         // BPF_MAP_TYPE_ARRAY
    __uint(max_entries, DEBUG_COUNTERS);
    __type(key, u32);
    __type(value, u64);
} ja4s_debug_counters __attribute__((section(".maps"), used));

// ============================================================================
// 辅助函数
// ============================================================================

static __always_inline u16 load_be16(const u8 *p)
{
    return ((u16)p[0] << 8) | p[1];
}

static __always_inline u32 load_be32(const u8 *p)
{
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
}

static __always_inline void inc_counter(u32 idx)
{
    u64 *val = bpf_map_lookup_elem(&ja4s_debug_counters, &idx);
    if (val)
        __sync_fetch_and_add(val, 1);
}

// ============================================================================
// TC egress 处理入口 — 捕获服务器发出的 TLS Server Hello 包
//
// 挂载到服务器网卡的 egress 路径，观测服务器发往客户端的包。
// 与 tc_options ingress 程序互补：
//   - ingress: 捕获 client → server 的 SYN（TCP 指纹 + TTL）
//   - egress:  捕获 server → client 的 Server Hello（TLS 响应指纹）
// ============================================================================

__attribute__((section("classifier"), used))
int ja4s_parse_server_hello(struct __sk_buff *skb)
{
    struct tls_server_hello_info info = {0};
    struct ja4s_connection_key key = {0};

    // 显式初始化数组中每个元素 — 帮助 BPF verifier 追踪所有字节的写入
    // clang BPF 后端可能将 __builtin_memset 或 ={0} 优化为 verifier 无法追踪的形式
#pragma clang loop unroll(full)
    for (int _i = 0; _i < MAX_ALPN_LEN; _i++) info.alpn[_i] = 0;
#pragma clang loop unroll(full)
    for (int _i = 0; _i < MAX_EXT_RAW_BYTES; _i++) info.ext_raw[_i] = 0;
#pragma clang loop unroll(full)
    for (int _i = 0; _i < 9; _i++) info._pad[_i] = 0;
#pragma clang loop unroll(full)
    for (int _i = 0; _i < 3; _i++) key.padding[_i] = 0;

    inc_counter(COUNTER_TOTAL);

    // ========================================================================
    // 第 1 步：P0F 风格 IP 头定位（与 ingress 程序相同策略）
    // ========================================================================
    u32 ip_off = 0;

#pragma clang loop unroll(full)
    for (u32 off = 0; off < 40; off += 2)
    {
        if (ip_off != 0)
            continue;

        u8 peek[12];
        if (bpf_skb_load_bytes(skb, off, peek, sizeof(peek)) < 0)
            continue;

        if ((peek[0] >> 4) == 4 && peek[9] == IPPROTO_TCP)
        {
            u8 ip_hdr[20];
            if (bpf_skb_load_bytes(skb, off, ip_hdr, sizeof(ip_hdr)) < 0)
                continue;

            u8 ihl = ip_hdr[0] & 0x0f;
            if (ihl < 5)
                continue;

            // 跳过 IP 分片包
            u16 frag_off = load_be16(ip_hdr + 6);
            if ((frag_off & 0x3FFF) != 0) {
                inc_counter(COUNTER_FRAG_SKIP);
                continue;
            }

            ip_off = off;
            key.src_ip = load_be32(ip_hdr + 12);  // 服务器 IP
            key.dst_ip = load_be32(ip_hdr + 16);  // 客户端 IP

            u8 ip_hdr_len = ihl * 4;
            u32 tcp_start = off + ip_hdr_len;

            u8 tcp_buf[20];
            if (bpf_skb_load_bytes(skb, tcp_start, tcp_buf, sizeof(tcp_buf)) < 0)
                continue;

            key.src_port = load_be16(tcp_buf + 0);  // 服务器端口
            key.dst_port = load_be16(tcp_buf + 2);  // 客户端端口
            key.protocol = IPPROTO_TCP;

            u8 data_offset = tcp_buf[12] >> 4;
            if (data_offset < 5)
                continue;

            // 过滤：只处理从服务器 443 端口发出的包
            if (key.src_port != 443 && key.src_port != 0x01bb)  // 443 = 0x01BB
                continue;

            inc_counter(COUNTER_TCP_443);

            // ================================================================
            // 第 2 步：检查是否有 TCP payload → TLS 记录
            // ================================================================
            u32 tcp_hdr_len = (u32)data_offset * 4u;
            u32 tcp_payload_start = tcp_start + tcp_hdr_len;
            u32 ip_total_len = load_be16(ip_hdr + 2);
            u32 ip_payload_len = ip_total_len - (u32)ip_hdr_len;

            // TCP payload 长度 = IP payload - TCP header
            if (ip_payload_len <= tcp_hdr_len)
                continue;  // 无 payload（纯 ACK 或无数据包）

            // ================================================================
            // 第 3 步：读取并验证 TLS Record 头部（5 字节）
            // ================================================================
            u8 tls_hdr[5];
            if (bpf_skb_load_bytes(skb, tcp_payload_start, tls_hdr, 5) < 0)
                continue;

            // TLS Record Type = 0x16 (Handshake)
            if (tls_hdr[0] != TLS_RECORD_HANDSHAKE)
                continue;

            inc_counter(COUNTER_TLS_HS);

            // ================================================================
            // 第 4 步：读取 Handshake 头部，验证 Server Hello 类型
            //
            // TLS Record: [Type(1)][Version(2)][Length(2)][Payload...]
            // Handshake:   [Type(1)][Length(3)][Body...]
            //               ^offset 5 from TLS record start
            // ================================================================
            u32 hs_start = tcp_payload_start + 5;  // TLS record payload = handshake

            u8 hs_type;
            if (bpf_skb_load_bytes(skb, hs_start, &hs_type, 1) < 0)
                continue;

            // Handshake Type = 0x02 (Server Hello)
            if (hs_type != TLS_HANDSHAKE_SERVER_HELLO)
                continue;

            inc_counter(COUNTER_SERVER_HELLO);

            // ================================================================
            // 第 5 步：解析 Server Hello body
            //
            // ServerHello {
            //   ProtocolVersion(2) + Random(32) + SessionID(variable) +
            //   CipherSuite(2) + CompressionMethod(1) + Extensions(variable)
            // }
            //
            // Body 起始于 hs_start + 1 + 3 = hs_start + 4
            // (handshake type 1 byte + length 3 bytes)
            // ================================================================
            u32 sh_body_start = hs_start + 4;

            // 读取 ProtocolVersion（2 bytes）+ Random（32 bytes）
            u8 sh_ver[2];
            if (bpf_skb_load_bytes(skb, sh_body_start, sh_ver, 2) < 0)
                continue;
            info.tls_version = load_be16(sh_ver);

            // 跳过 Random(32) → offset = sh_body_start + 2 + 32 = sh_body_start + 34
            u32 after_random = sh_body_start + 34;

            // 读取 SessionID Length（1 byte）
            u8 sid_len;
            if (bpf_skb_load_bytes(skb, after_random, &sid_len, 1) < 0)
                continue;

            // CipherSuite offset = after_random + 1 + sid_len
            u32 cs_offset = after_random + 1 + (u32)sid_len;
            u8 cs[2];
            if (bpf_skb_load_bytes(skb, cs_offset, cs, 2) < 0)
                continue;
            info.cipher_suite = load_be16(cs);

            // CompressionMethod offset = cs_offset + 2
            u32 cm_offset = cs_offset + 2;
            u8 comp_method;
            if (bpf_skb_load_bytes(skb, cm_offset, &comp_method, 1) < 0)
                continue;

            // Extensions Length offset = cm_offset + 1
            u32 ext_len_offset = cm_offset + 1;
            u8 ext_len_buf[2];
            if (bpf_skb_load_bytes(skb, ext_len_offset, ext_len_buf, 2) < 0)
                continue;
            u16 extensions_total_len = load_be16(ext_len_buf);

            // ================================================================
            // 第 6 步：解析每个 Extension（限制循环次数）
            //
            // Extension {
            //   ExtensionType (2) + ExtensionDataLength (2) + Data (variable)
            // }
            // ================================================================
            u32 ext_cur = ext_len_offset + 2;  // 第一个 extension 起始
            u32 ext_end = ext_cur + extensions_total_len;
            u8 ext_count = 0;
            info.ext_raw_len = 0;

// 不使用 #pragma unroll(full) — clang 14 BPF 后端无法完全展开带条件 break 的循环
            for (int i = 0; i < MAX_EXTENSIONS && ext_cur + 4 <= ext_end; i++)
            {
                u8 ext_hdr[4];
                if (bpf_skb_load_bytes(skb, ext_cur, ext_hdr, 4) < 0)
                    break;

                u16 ext_type = load_be16(ext_hdr);
                u16 ext_data_len = load_be16(ext_hdr + 2);

                // 记录扩展 type 到 ext_raw（原始顺序，含 GREASE）
                if (info.ext_raw_len + 2 <= MAX_EXT_RAW_BYTES) {
                    info.ext_raw[info.ext_raw_len] = ext_hdr[0];
                    info.ext_raw[info.ext_raw_len + 1] = ext_hdr[1];
                    info.ext_raw_len += 2;
                }

                ext_count++;

                // ALPN 扩展 (type 0x0010)：提取选中的 ALPN 协议名
                //
                // ALPN Extension {
                //   ExtensionType (2) = 0x0010
                //   ExtensionDataLength (2)
                //   ALPNProtocolNameListLength (2)
                //   ProtocolName (1+len)... (Server Hello 中通常只有一个)
                // }
                if (ext_type == 0x0010 && ext_data_len >= 4)
                {
                    // 跳过 Extension Headers (4 bytes) → ALPN Protocol Name List Length (2)
                    u32 alpn_list_len_offset = ext_cur + 4;
                    u8 alpn_list_len_buf[2];
                    if (bpf_skb_load_bytes(skb, alpn_list_len_offset, alpn_list_len_buf, 2) == 0)
                    {
                        // 第一个 Protocol Name: 1 byte length + data
                        u32 proto_name_offset = alpn_list_len_offset + 2 + 1;  // skip list_length(2) + proto_len(1)
                        u8 proto_len;
                        if (bpf_skb_load_bytes(skb, alpn_list_len_offset + 2, &proto_len, 1) == 0)
                        {
                            if (proto_len > 0 && proto_len <= MAX_ALPN_LEN)
                            {
                                // 读取协议名 (e.g., "h2", "http/1.1")
                                // 使用固定长度 MAX_ALPN_LEN 绕过 BPF 验证器对可变长度读取的限制
                                // 超出 proto_len 的字节将被后续 alpn_len 截断
                                if (bpf_skb_load_bytes(skb, proto_name_offset, info.alpn, MAX_ALPN_LEN) == 0)
                                {
                                    info.alpn_len = proto_len;
                                    inc_counter(COUNTER_ALPN_FOUND);
                                }
                            }
                        }
                    }
                }

                // 移动到下一个 Extension
                ext_cur += 4 + (u32)ext_data_len;
            }

            if (ext_count >= MAX_EXTENSIONS)
            {
                inc_counter(COUNTER_EXT_OVERFLOW);
            }

            info.extension_count = ext_count;
            info.capture_ns = bpf_ktime_get_ns();

            // ================================================================
            // 第 7 步：写入 BPF Map
            // ================================================================
            bpf_map_update_elem(&tls_server_hello_map, &key, &info, BPF_ANY);
            inc_counter(COUNTER_MAP_OK);
        }
    }

    return TC_ACT_UNSPEC;  // 纯观测模式，不修改包
}

// ============================================================================
// License
// ============================================================================
char _license[] __attribute__((section("license"), used)) = "GPL";
