//! C 兼容结构体 —— 与 eBPF 侧 tcp_options.h 字段布局完全一致
//!
//! 这些类型用于 BPF Map 的 Key/Value 读写，内存布局必须与内核 eBPF 侧
//! 的 C 结构体严格匹配。所有类型仅包含纯整数，不存在无效位模式，
//! 可安全用于 BPF Map 读写。

/// BPF Map 的 Key —— 连接五元组
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[repr(C)]
pub struct ConnectionTuple {
    pub src_ip: u32,   // 源 IP（网络字节序）
    pub dst_ip: u32,   // 目的 IP（网络字节序）
    pub src_port: u16, // 源端口（网络字节序）
    pub dst_port: u16, // 目的端口（网络字节序）
    pub protocol: u8,  // 协议号，6 = TCP
    pub _pad: [u8; 3], // 对齐填充
}

/// BPF Map 的 Value —— TCP SYN 包指纹原始数据 (64 字节)
///
/// 字段布局与 eBPF 侧 tcp_options.h 完全一致：
///
/// offset 0:  window_size (2 bytes)  TCP 窗口大小
/// offset 2:  raw_opts_len (1 byte)  raw_opts 有效字节数
/// offset 3:  ip_ttl (1 byte)        IP TTL 字段（替代原 _pad）
/// offset 4:  raw_opts[40] (40 bytes) TCP Option 原始字节
/// offset 44: _pad2[4] (4 bytes)     对齐到 8 字节边界
/// offset 48: capture_ns (8 bytes)   SYN 捕获时间戳（纳秒）
/// offset 56: synack_ns (8 bytes)    SYN-ACK 到达时间戳（纳秒），0 表示未捕获
#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub struct TcpOptionsInfo {
    pub window_size: u16,   // offset 0: TCP 窗口大小
    pub raw_opts_len: u8,   // offset 2: raw_opts 有效字节数
    pub ip_ttl: u8,         // offset 3: IP TTL（替代原 _pad）
    pub raw_opts: [u8; 40], // offset 4: 原始 TCP Option 字节
    pub _pad2: [u8; 4],     // offset 44: 对齐 capture_ns 到 8 字节边界
    pub capture_ns: u64,    // offset 48: SYN 捕获时间戳（纳秒）
    pub synack_ns: u64,     // offset 56: SYN-ACK 到达时间戳（纳秒），0=未捕获
}

// 编译时验证结构体大小
const _: () = {
    assert!(std::mem::size_of::<TcpOptionsInfo>() == 64);
};

// 安全性声明：这些类型仅包含纯整数，不存在无效位模式，可安全用于 BPF Map 读写
unsafe impl aya::Pod for ConnectionTuple {}
unsafe impl aya::Pod for TcpOptionsInfo {}
