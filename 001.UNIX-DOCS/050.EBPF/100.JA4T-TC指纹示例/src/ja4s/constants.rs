//! JA4S 常量定义

/// GREASE 值列表（RFC 8701，16 个）
///
/// 浏览器在 Client Hello 中随机插入这些值以检测中间件兼容性问题，
/// 服务器在 Server Hello 中也可能包含 GREASE 扩展。
pub const GREASE_VALUES: [u16; 16] = [
    0x0a0a, 0x1a1a, 0x2a2a, 0x3a3a, 0x4a4a, 0x5a5a, 0x6a6a, 0x7a7a, 0x8a8a, 0x9a9a, 0xaaaa, 0xbaba,
    0xcaca, 0xdada, 0xeaea, 0xfafa,
];

/// TLS 版本映射表（与 JA4 共用）
pub const TLS_MAPPER: &[(u16, &str)] = &[
    (0x0301, "10"), // TLS 1.0
    (0x0302, "11"), // TLS 1.1
    (0x0303, "12"), // TLS 1.2
    (0x0304, "13"), // TLS 1.3
];
