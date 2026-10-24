//! 连接缓存 —— 线程安全，由 BPF Map 轮询任务周期性更新
//!
//! 使用 String Key（如 "1.2.3.4:443->5.6.7.8:8080"）而非原始的 ConnectionTuple，
//! 避免为 C 兼容结构体实现自定义 Hash 的复杂性。

use std::collections::HashMap as StdHashMap;
use std::sync::Arc;
use tokio::sync::RwLock;

use super::types::{ConnectionTuple, TcpOptionsInfo};

/// 内存连接缓存，将 BPF Map 的二进制 Key 转换为格式化字符串 Key
pub struct ConnectionCache {
    entries: StdHashMap<String, TcpOptionsInfo>,
}

impl ConnectionCache {
    pub fn new() -> Self {
        Self {
            entries: StdHashMap::new(),
        }
    }

    /// 用 BPF Map 导出的新数据替换整个缓存
    pub fn update(&mut self, fresh: StdHashMap<ConnectionTuple, TcpOptionsInfo>) {
        for (key, value) in fresh {
            let k = format!(
                "{}.{}.{}.{}:{}->{}.{}.{}.{}:{}",
                (key.src_ip >> 24) as u8,
                (key.src_ip >> 16) as u8,
                (key.src_ip >> 8) as u8,
                key.src_ip as u8,
                key.src_port,
                (key.dst_ip >> 24) as u8,
                (key.dst_ip >> 16) as u8,
                (key.dst_ip >> 8) as u8,
                key.dst_ip as u8,
                key.dst_port,
            );
            self.entries.insert(k, value);
        }
    }

    /// 按五元组查询缓存
    pub fn lookup(
        &self,
        src_ip: u32,
        dst_ip: u32,
        src_port: u16,
        dst_port: u16,
    ) -> Option<&TcpOptionsInfo> {
        let k = format!(
            "{}.{}.{}.{}:{}->{}.{}.{}.{}:{}",
            (src_ip >> 24) as u8,
            (src_ip >> 16) as u8,
            (src_ip >> 8) as u8,
            src_ip as u8,
            src_port,
            (dst_ip >> 24) as u8,
            (dst_ip >> 16) as u8,
            (dst_ip >> 8) as u8,
            dst_ip as u8,
            dst_port,
        );
        self.entries.get(&k)
    }
}

/// 共享缓存类型别名：Arc<RwLock<ConnectionCache>>
pub type SharedCache = Arc<RwLock<ConnectionCache>>;
