//! Unix Socket 通信的 JSON 格式定义
//!
//! nginx 通过 Unix Socket 查询 daemon，获取原始 TCP 指纹数据后按以下格式拼接:
//!
//!   JA4T 指纹 = window_size _ mss _ window_scale _ sack_permitted _ options-order
//!
//! 示例: "42340_1460_10_1_2-4-8-1-3"
//!
//!   ┌────────────────┬───────────────┬────────────────────────────────────┐
//!   │ 示例值          │ 字段           │ 说明                               │
//!   ├────────────────┼───────────────┼────────────────────────────────────┤
//!   │ 42340          │ window_size   │ TCP 窗口大小原始值                   │
//!   ├────────────────┼───────────────┼────────────────────────────────────┤
//!   │ 1460           │ mss           │ 最大分段大小 (TCP Option kind=2)     │
//!   ├────────────────┼───────────────┼────────────────────────────────────┤
//!   │ 10             │ window_scale  │ 窗口缩放因子 0-14 (TCP Option kind=3)│
//!   ├────────────────┼───────────────┼────────────────────────────────────┤
//!   │ 1              │ sack_permitted│ SACK 支持标志 (TCP Option kind=4)   │
//!   ├────────────────┼───────────────┼────────────────────────────────────┤
//!   │ 2-4-8-1-3      │ options_order │ Option 类型顺序，"-" 连接            │
//!   └────────────────┴───────────────┴────────────────────────────────────┘
//!
//! TCP Option 类型编码:
//!   0 = EOL (选项结束), 1 = NOP (填充), 2 = MSS, 3 = Window Scale,
//!   4 = SACK Permitted, 5 = SACK (不出现在 SYN 中), 8 = Timestamp
//!
//! 各平台特征 Option Order:
//!   Linux (含Android): 2-4-8-1-3  (mss,sack,ts,nop,ws)
//!   Windows 7+:        2-1-3-4-8  (mss,nop,ws,sack,ts)
//!   Apple (macOS/iOS):  2-1-3-1-1-8-4[-0] (mss,nop,ws,nop,nop,ts,sack[,eol])

use serde::{Deserialize, Serialize};

/// 日志中输出的 SYN 条目（JSON 格式，方便下游解析）
/// 这些字段是 nginx 拼接 JA4T 指纹的原始数据来源
#[derive(Debug, Serialize)]
pub struct SynEntry {
    pub src_ip: String,
    pub src_port: u16,
    pub dst_ip: String,
    pub dst_port: u16,
    pub window_size: u16,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub mss: Option<u16>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub window_scale: Option<u8>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub sack_permitted: Option<u8>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub options_order: Option<Vec<u8>>,
    pub capture_age_ms: u64, // 捕获后经过的毫秒数
    /// IP TTL 观察值（用于 OS 推断和网络跳数分析）
    #[serde(skip_serializing_if = "Option::is_none")]
    pub ip_ttl: Option<u8>,
    /// TCP 往返延迟（微秒），仅在 SYN-ACK 已捕获时有值
    /// 由 (synack_ns - capture_ns) / 1000 计算得出
    #[serde(skip_serializing_if = "Option::is_none")]
    pub rtt_us: Option<u64>,
}

/// nginx 发送的查询请求
#[derive(Debug, Deserialize)]
pub struct LookupRequest {
    pub src_ip: String,
    pub dst_ip: String,
    pub src_port: u16,
    pub dst_port: u16,
}

/// 返回给 nginx 的查询响应
///
/// JSON 示例:
/// ```json
/// {"found":true,"window_size":65535,"mss":1460,"window_scale":7,
///  "sack_permitted":1,"options_order":[2,4,8,3],"capture_age_ms":1234,
///  "ip_ttl":52,"rtt_us":8500}
/// ```
/// - `ip_ttl=52` → 初始 TTL=64 (Linux)，经过 12 跳
/// - `rtt_us=8500` → 8.5ms 往返延迟
#[derive(Debug, Serialize)]
pub struct LookupResponse {
    pub found: bool,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub window_size: Option<u16>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub mss: Option<u16>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub window_scale: Option<u8>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub sack_permitted: Option<u8>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub options_order: Option<Vec<u8>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub capture_age_ms: Option<u64>,
    /// IP TTL 观察值（用于推断客户端 OS 类型和网络跳数）
    #[serde(skip_serializing_if = "Option::is_none")]
    pub ip_ttl: Option<u8>,
    /// TCP 往返延迟（微秒），仅在 SYN-ACK 已捕获时有值
    /// null 表示 SYN-ACK 未观测到
    #[serde(skip_serializing_if = "Option::is_none")]
    pub rtt_us: Option<u64>,
    /// JA4S TLS Server Hello 指纹（仅在 eBPF 捕获到 Server Hello 时有值）
    /// 格式: t{ver}{#exts}{alpn}_{cipher}_{ext_hash}
    #[serde(skip_serializing_if = "Option::is_none")]
    pub ja4s_fingerprint: Option<String>,
}
