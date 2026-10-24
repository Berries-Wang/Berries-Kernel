#ifndef TCP_OPTIONS_H
#define TCP_OPTIONS_H

#include <linux/types.h>

// JA4T TCP 指纹提取 —— eBPF 与用户态共享结构体
// 用于在 eBPF TC 分类器和 ja4t-daemon 之间传递 TCP SYN 包的指纹数据

// 单个 TCP 连接最多允许 12 种 Option 类型（与 p0f 工具保持一致）
#define MAX_TCP_OPTIONS 12
// 原始 TCP Option 字节的最大长度（40 字节足够容纳 10 个 4 字节 MSS Option）
#define MAX_RAW_OPTS 40

/* BPF LRU Hash Map 的 Key —— 连接五元组 */
struct connection_tuple
{
    __u32 src_ip;      // 源 IP（网络字节序）
    __u32 dst_ip;      // 目的 IP（网络字节序）
    __u16 src_port;    // 源端口（网络字节序）
    __u16 dst_port;    // 目的端口（网络字节序）
    __u8 protocol;     // 协议号，6 = TCP
    __u8 padding[3];   // 对齐填充，保证结构体大小为 16 字节
};

/* BPF LRU Hash Map 的 Value —— TCP SYN 包的指纹原始数据 */
// 注意：字段布局（offset）必须与 eBPF 侧和用户态 Rust 侧完全一致
// 解析工作延迟到用户态完成，以简化 eBPF 验证器的工作
struct tcp_options_info
{
    __u16 window_size;            // offset 0: TCP 窗口大小（原始值）
    __u8 raw_opts_len;            // offset 2: raw_opts[] 中的有效字节数
    __u8 ip_ttl;                  // offset 3: IP 头 TTL 字段（替代原来的 _pad[1]）
                                  //   用于推断客户端 OS 类型和网络跳数
    __u8 raw_opts[MAX_RAW_OPTS];  // offset 4: 原始 TCP Option 字节
    __u8 _pad2[4];                // offset 44: 对齐 capture_ns 到 offset 48（8 字节对齐）
    __u64 capture_ns;             // offset 48: SYN 包捕获时间戳（纳秒）
    __u64 synack_ns;              // offset 56: SYN-ACK 包到达时间戳（纳秒），0 表示未捕获
                                  //   RTT = (synack_ns - capture_ns) / 1000 微秒
};                                // 总大小: 64 字节

// JA4S: ALPN 协议名最大长度
#define MAX_ALPN_LEN 32
// JA4S: 扩展 type 原始字节最大长度（每扩展 2 字节 type × 最多 20 个扩展）
#define MAX_EXT_RAW_BYTES 120

/* JA4S: TLS Server Hello 指纹原始数据 */
// 用于在 eBPF TC egress 程序和 ja4t-daemon 之间传递 Server Hello 信息
// JA4S 指纹的 SHA256 计算在用户态完成，以简化 eBPF 验证器的工作
struct tls_server_hello_info
{
    __u16 tls_version;               // offset 0: TLS 版本 (0x0304=TLS1.3, 0x0303=TLS1.2)
    __u16 cipher_suite;              // offset 2: 服务器选择的密码套件 (单个)
    __u8  extension_count;           // offset 4: 扩展数量 (含 GREASE)
    __u8  alpn_len;                  // offset 5: ALPN 协议名长度 (0=无 ALPN)
    __u8  alpn[MAX_ALPN_LEN];        // offset 6: ALPN 协议名 (如 "h2", "http/1.1")
    __u8  ext_raw_len;               // offset 38: ext_raw 中的有效字节数
    __u8  ext_raw[MAX_EXT_RAW_BYTES];// offset 39: 扩展 type 原始字节 (每扩展 2 字节，原始顺序)
    __u8  _pad[9];                   // offset 159: 对齐 capture_ns 到 8 字节边界
    __u64 capture_ns;                // offset 168: 捕获时间戳 (纳秒)
};                                   // 总大小: 176 字节

/* JA4S: 连接五元组 Key（与 connection_tuple 格式相同） */
struct ja4s_connection_key
{
    __u32 src_ip;      // 服务器 IP（网络字节序）
    __u32 dst_ip;      // 客户端 IP（网络字节序）
    __u16 src_port;    // 服务器端口（通常是 443）
    __u16 dst_port;    // 客户端端口
    __u8 protocol;     // 协议号，6 = TCP
    __u8 padding[3];   // 对齐填充，16 字节
};

#endif /* TCP_OPTIONS_H */
