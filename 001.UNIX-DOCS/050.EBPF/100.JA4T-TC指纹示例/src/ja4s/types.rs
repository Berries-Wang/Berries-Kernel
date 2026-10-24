//! JA4S 数据类型定义
//!
//! 与 eBPF 侧 tcp_options.h 中 tls_server_hello_info 字段布局对应。

use serde::Serialize;

/// TLS Server Hello 指纹原始数据（用户态，解析后）
///
/// 由 ja4t-daemon 从 eBPF tls_server_hello_map 中读取，
/// 传入 compute_ja4s() 计算 JA4S 指纹。
#[derive(Debug, Clone)]
pub struct Ja4sData {
    pub tls_version: u16,
    pub cipher_suite: u16,
    pub extension_count: u8,
    pub alpn: Option<String>,
    /// 扩展 type 列表（原始顺序，含 GREASE）
    pub extension_raw_types: Vec<u16>,
}

/// JA4S 指纹字段（解析后）
#[allow(dead_code)]
#[derive(Debug, Clone)]
pub struct Ja4sFields {
    pub protocol: char,              // 't'
    pub version: String,             // "13", "12" 等
    pub extension_count_str: String, // 两位数字，含 GREASE
    pub alpn: String,                // 首尾字母数字，无则 "00"
    pub cipher: String,              // 密码套件 hex lowercase
    pub extension_hash: String,      // SHA256[:12]
}

/// 解析后的 JA4S 完整指纹
#[derive(Debug, Clone, Serialize)]
pub struct Ja4sFingerprint {
    /// 完整 JA4S 指纹字符串，如 "t1303h2_1301_abc123def456"
    pub raw: String,
    /// 字段明细（用于调试和日志）
    pub fields: Ja4sFieldsDebug,
}

/// JA4S 指纹字段明细（可序列化，用于日志/调试）
#[derive(Debug, Clone, Serialize)]
pub struct Ja4sFieldsDebug {
    pub protocol: String,
    pub version: String,
    pub extension_count: u8,
    pub alpn: String,
    pub cipher: String,
    pub extension_hash: String,
}

// ============================================================================
// JA4S BPF Map C 兼容结构体（与 eBPF 侧 tcp_options.h 字段布局完全一致）
// ============================================================================

/// JA4S BPF Map 的 Key —— 连接五元组（egress 视角: src=服务器, dst=客户端）
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[repr(C)]
pub struct Ja4sConnectionKey {
    pub src_ip: u32,   // 服务器 IP（网络字节序）
    pub dst_ip: u32,   // 客户端 IP（网络字节序）
    pub src_port: u16, // 服务器端口（通常 443）
    pub dst_port: u16, // 客户端端口
    pub protocol: u8,  // 协议号，6 = TCP
    pub _pad: [u8; 3], // 对齐填充
}

/// JA4S BPF Map 的 Value —— TLS Server Hello 指纹原始数据 (176 字节)
///
/// 字段布局与 eBPF 侧 tcp_options.h 中 tls_server_hello_info 完全一致：
///
/// offset 0:   tls_version (2 bytes)       TLS 版本号
/// offset 2:   cipher_suite (2 bytes)      服务器选中的密码套件
/// offset 4:   extension_count (1 byte)    扩展数量（含 GREASE）
/// offset 5:   alpn_len (1 byte)           ALPN 名称长度
/// offset 6:   alpn[32] (32 bytes)         ALPN 协议名称
/// offset 38:  ext_raw_len (1 byte)        ext_raw 有效字节数
/// offset 39:  ext_raw[120] (120 bytes)    扩展 type 原始字节
/// offset 159: _pad[9] (9 bytes)           对齐
/// offset 168: capture_ns (8 bytes)        捕获时间戳（纳秒）
#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub struct TlsServerHelloInfo {
    pub tls_version: u16,    // offset 0
    pub cipher_suite: u16,   // offset 2
    pub extension_count: u8, // offset 4
    pub alpn_len: u8,        // offset 5
    pub alpn: [u8; 32],      // offset 6
    pub ext_raw_len: u8,     // offset 38
    pub ext_raw: [u8; 120],  // offset 39
    pub _pad: [u8; 9],       // offset 159
    pub capture_ns: u64,     // offset 168
}

// 编译时验证 JA4S 结构体大小
const _: () = {
    assert!(std::mem::size_of::<TlsServerHelloInfo>() == 176);
};

// 安全性声明：这些类型仅包含纯整数，不存在无效位模式
unsafe impl aya::Pod for Ja4sConnectionKey {}
unsafe impl aya::Pod for TlsServerHelloInfo {}
