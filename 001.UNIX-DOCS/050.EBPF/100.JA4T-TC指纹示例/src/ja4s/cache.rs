//! Server Hello 缓存 —— 线程安全，由 Server Hello Map 轮询任务周期性更新
//!
//! 缓存的 Key 是 egress 视角的五元组字符串（server→client），
//! Value 是已计算好的 JA4S 指纹 + 捕获时间戳。

use std::collections::HashMap as StdHashMap;
use std::sync::Arc;
use tokio::sync::RwLock;

use super::compute::compute_ja4s;
use super::types::{Ja4sConnectionKey, TlsServerHelloInfo};

/// 缓存条目：已计算好的 JA4S 指纹
pub struct Ja4sCacheEntry {
    pub fingerprint: String,
}

/// JA4S Server Hello 指纹缓存
///
/// Key: "server_ip:server_port->client_ip:client_port"（egress 视角）
/// Value: 计算好的 JA4S 指纹字符串 + 捕获时间
pub struct ServerHelloCache {
    entries: StdHashMap<String, Ja4sCacheEntry>,
}

impl ServerHelloCache {
    pub fn new() -> Self {
        Self {
            entries: StdHashMap::new(),
        }
    }

    /// 用 eBPF Map 导出的新数据替换整个缓存，同时计算 JA4S 指纹
    pub fn update(&mut self, fresh: StdHashMap<Ja4sConnectionKey, TlsServerHelloInfo>) {
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

            // 将 eBPF 原始数据转换为 ja4s::Ja4sData 并计算 JA4S 指纹
            let ja4s_data = super::types::Ja4sData {
                tls_version: value.tls_version,
                cipher_suite: value.cipher_suite,
                extension_count: value.extension_count,
                alpn: if value.alpn_len > 0 {
                    let len = std::cmp::min(value.alpn_len as usize, 32);
                    String::from_utf8_lossy(&value.alpn[..len])
                        .trim_end_matches('\0')
                        .to_string()
                        .into()
                } else {
                    None
                },
                extension_raw_types: {
                    let count = std::cmp::min(value.ext_raw_len as usize, 120) / 2;
                    (0..count)
                        .map(|i| {
                            let idx = i * 2;
                            ((value.ext_raw[idx] as u16) << 8) | (value.ext_raw[idx + 1] as u16)
                        })
                        .collect()
                },
            };

            let fingerprint = compute_ja4s(&ja4s_data)
                .map(|fp| fp.raw)
                .unwrap_or_else(|| "unknown".to_string());

            self.entries.insert(
                k,
                Ja4sCacheEntry {
                    fingerprint,
                },
            );
        }
    }

    /// 按五元组查询缓存（egress 视角：server→client）
    pub fn lookup(
        &self,
        server_ip: u32,
        client_ip: u32,
        server_port: u16,
        client_port: u16,
    ) -> Option<&Ja4sCacheEntry> {
        let k = format!(
            "{}.{}.{}.{}:{}->{}.{}.{}.{}:{}",
            (server_ip >> 24) as u8,
            (server_ip >> 16) as u8,
            (server_ip >> 8) as u8,
            server_ip as u8,
            server_port,
            (client_ip >> 24) as u8,
            (client_ip >> 16) as u8,
            (client_ip >> 8) as u8,
            client_ip as u8,
            client_port,
        );
        self.entries.get(&k)
    }
/// 获取缓存中所有条目（用于终端打印）
    pub fn iter(&self) -> impl Iterator<Item = (&str, &Ja4sCacheEntry)> {
        self.entries.iter().map(|(k, v)| (k.as_str(), v))
    }
}

/// 共享 JA4S 缓存类型别名
pub type SharedServerHelloCache = Arc<RwLock<ServerHelloCache>>;
