//! BPF Map 读取器 —— 持有 tcp_options_map 和 debug_counters 两个 Map 句柄
//!
//! 提供两种读取模式：
//!   1. `dump_all`: 遍历整个 LRU Hash Map（周期性轮询用）
//!   2. `lookup_direct`: 按 Key 直接查询（缓存未命中时的兜底）

use anyhow::{Context, Result};
use std::collections::HashMap as StdHashMap;

use super::types::{ConnectionTuple, TcpOptionsInfo};

/// BPF Map 读取器，持有 tcp_options_map 和 debug_counters 两个 Map 的句柄
pub struct BpfReader {
    map: aya::maps::Map, // tcp_options_map: LRU Hash, key=五元组, value=指纹数据
    debug_map: aya::maps::Map, // debug_counters: Array, 8 个 u64 诊断计数器
}

impl BpfReader {
    pub fn new(map: aya::maps::Map, debug_map: aya::maps::Map) -> Self {
        Self { map, debug_map }
    }

    /// 导出 BPF Map 的全部条目到本地 HashMap
    ///
    /// 遍历整个 LRU Hash Map，将内核中的数据复制到用户态。
    /// 此操作用于周期性轮询，而非每次查询时调用（性能考虑）。
    pub fn dump_all(&self) -> Result<StdHashMap<ConnectionTuple, TcpOptionsInfo>> {
        let map: aya::maps::HashMap<&aya::maps::MapData, ConnectionTuple, TcpOptionsInfo> =
            aya::maps::HashMap::try_from(&self.map)?;
        let mut result = StdHashMap::new();
        for item in map.iter() {
            let (key, value) = item.context("读取 BPF Map 条目失败")?;
            result.insert(key, value);
        }
        Ok(result)
    }

    /// 读取调试计数器
    ///
    /// 返回 (主计数器[5], SYN-ACK计数器[2], 诊断计数器[3])
    /// 主计数器: total, ipv4, tcp, syn, map_ok
    /// SYN-ACK: synack, synack_ok
    /// 诊断计数器: ethertype_offset, protocol_byte, tcp_start_offset
    pub fn dump_debug_counters(&self) -> Result<([u64; 5], [u64; 2], [u64; 3])> {
        let map: aya::maps::Array<&aya::maps::MapData, u64> =
            aya::maps::Array::try_from(&self.debug_map)?;
        let mut counters = [0u64; 5];
        for i in 0..5u32 {
            if let Ok(val) = map.get(&i, 0) {
                counters[i as usize] = val;
            }
        }
        let mut synack = [0u64; 2];
        for i in 0..2u32 {
            if let Ok(val) = map.get(&(8 + i), 0) {
                synack[i as usize] = val;
            }
        }
        let mut diag = [0u64; 3];
        for i in 0..3u32 {
            if let Ok(val) = map.get(&(5 + i), 0) {
                diag[i as usize] = val;
            }
        }
        Ok((counters, synack, diag))
    }

    /// 直接从 BPF Map 查询单个 Key（绕过内存缓存）
    ///
    /// 当内存缓存未命中时，作为后备方案直接调用 bpf_map_lookup_elem。
    /// 这消除了轮询间隔带来的竞态窗口问题。
    pub fn lookup_direct(&self, key: &ConnectionTuple) -> Result<Option<TcpOptionsInfo>> {
        let map: aya::maps::HashMap<&aya::maps::MapData, ConnectionTuple, TcpOptionsInfo> =
            aya::maps::HashMap::try_from(&self.map)?;
        match map.get(key, 0) {
            Ok(value) => Ok(Some(value)),
            Err(_) => Ok(None), // Key 不存在或 Map 错误，统一返回 None
        }
    }
}
