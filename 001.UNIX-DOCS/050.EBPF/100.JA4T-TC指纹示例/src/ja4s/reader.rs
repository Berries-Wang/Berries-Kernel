//! BPF Map 读取器 —— 持有 tls_server_hello_map 和 ja4s_debug_counters 两个 Map 句柄
//!
//! 提供两种读取模式：
//!   1. `dump_all`: 遍历整个 LRU Hash Map（周期性轮询用）
//!   2. `lookup_direct`: 按 Key 直接查询（缓存未命中时的兜底）

use anyhow::{Context, Result};
use std::collections::HashMap as StdHashMap;

use super::types::{Ja4sConnectionKey, TlsServerHelloInfo};

/// Server Hello BPF Map 读取器
pub struct ServerHelloReader {
    map: aya::maps::Map,       // tls_server_hello_map: LRU Hash
    debug_map: aya::maps::Map, // ja4s_debug_counters: Array, 8 x u64
}

impl ServerHelloReader {
    pub fn new(map: aya::maps::Map, debug_map: aya::maps::Map) -> Self {
        Self { map, debug_map }
    }

    /// 导出 Server Hello BPF Map 全部条目
    pub fn dump_all(&self) -> Result<StdHashMap<Ja4sConnectionKey, TlsServerHelloInfo>> {
        let map: aya::maps::HashMap<&aya::maps::MapData, Ja4sConnectionKey, TlsServerHelloInfo> =
            aya::maps::HashMap::try_from(&self.map)?;
        let mut result = StdHashMap::new();
        for item in map.iter() {
            let (key, value) = item.context("读取 JA4S BPF Map 条目失败")?;
            result.insert(key, value);
        }
        Ok(result)
    }

    /// 读取 JA4S 调试计数器
    pub fn dump_debug_counters(&self) -> Result<[u64; 8]> {
        let map: aya::maps::Array<&aya::maps::MapData, u64> =
            aya::maps::Array::try_from(&self.debug_map)?;
        let mut counters = [0u64; 8];
        for i in 0..8u32 {
            if let Ok(val) = map.get(&i, 0) {
                counters[i as usize] = val;
            }
        }
        Ok(counters)
    }

    /// 直接从 BPF Map 查询单个 Key（绕过内存缓存）
    pub fn lookup_direct(&self, key: &Ja4sConnectionKey) -> Result<Option<TlsServerHelloInfo>> {
        let map: aya::maps::HashMap<&aya::maps::MapData, Ja4sConnectionKey, TlsServerHelloInfo> =
            aya::maps::HashMap::try_from(&self.map)?;
        match map.get(key, 0) {
            Ok(value) => Ok(Some(value)),
            Err(_) => Ok(None),
        }
    }
}
