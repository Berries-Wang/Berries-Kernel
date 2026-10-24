//! 轮询任务 —— 周期性读取 JA4T BPF Map 并更新内存缓存

use std::sync::Arc;

use super::cache::SharedCache;
use super::options::parse_tcp_options;
use super::reader::BpfReader;
use crate::protocol::SynEntry;

/// 后台异步任务：按固定间隔导出 BPF Map 全部条目，更新缓存，输出日志
pub async fn poll_bpf_map(reader: Arc<BpfReader>, cache: SharedCache, interval_secs: u64) {
    let mut interval = tokio::time::interval(tokio::time::Duration::from_secs(interval_secs));

    loop {
        interval.tick().await;
        match reader.dump_all() {
            Ok(entries) => {
                let count = entries.len();
                // 逐条输出捕获到的 SYN 条目（JSON 格式日志，方便下游系统解析）
                for (key, info) in &entries {
                    let parsed = parse_tcp_options(&info.raw_opts[..info.raw_opts_len as usize]);
                    let syn = SynEntry {
                        src_ip: std::net::Ipv4Addr::from(key.src_ip).to_string(),
                        src_port: key.src_port,
                        dst_ip: std::net::Ipv4Addr::from(key.dst_ip).to_string(),
                        dst_port: key.dst_port,
                        window_size: info.window_size,
                        mss: parsed.mss,
                        window_scale: parsed.window_scale,
                        sack_permitted: parsed.sack_permitted,
                        options_order: if parsed.options_order.is_empty() {
                            None
                        } else {
                            Some(parsed.options_order)
                        },
                        capture_age_ms: {
                            // 计算自捕获以来经过的毫秒数
                            let now_ns = std::time::SystemTime::now()
                                .duration_since(std::time::UNIX_EPOCH)
                                .unwrap_or_default()
                                .as_nanos() as u64;
                            if info.capture_ns > 0 && now_ns > info.capture_ns {
                                (now_ns - info.capture_ns) / 1_000_000
                            } else {
                                0
                            }
                        },
                        // TTL: 仅在非零时输出（0 表示旧版本 eBPF 未采集）
                        ip_ttl: if info.ip_ttl > 0 {
                            Some(info.ip_ttl)
                        } else {
                            None
                        },
                        // RTT: (synack_ns - capture_ns) / 1000 得到微秒
                        rtt_us: if info.synack_ns > 0 && info.synack_ns > info.capture_ns {
                            Some((info.synack_ns - info.capture_ns) / 1000)
                        } else {
                            None
                        },
                    };
                    log::info!("SYN {}", serde_json::to_string(&syn).unwrap_or_default());
                }
                // 用新数据替换整个缓存
                cache.write().await.update(entries);
                // 输出调试计数器
                let (counters, synack, diag) = reader.dump_debug_counters().unwrap_or_default();
                log::info!(
                    "BPF Map 轮询完成: {} 条 | 计数器: total={} ipv4={} tcp={} syn={} map_ok={} | SYN-ACK: seen={} matched={} | 诊断: ethertype=0x{:04x} proto={} tcp_start={}",
                    count,
                    counters[0], counters[1], counters[2], counters[3], counters[4],
                    synack[0], synack[1],
                    diag[0], diag[1], diag[2]
                );
            }
            Err(e) => {
                log::error!("导出 BPF Map 失败: {}", e);
            }
        }
    }
}
