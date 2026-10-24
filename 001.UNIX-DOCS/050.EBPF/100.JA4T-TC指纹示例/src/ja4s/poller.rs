//! 轮询任务 —— 周期性读取 JA4S Server Hello BPF Map 并更新缓存

use std::sync::Arc;

use super::cache::SharedServerHelloCache;
use super::reader::ServerHelloReader;

/// 后台异步任务：轮询 Server Hello BPF Map，计算 JA4S 指纹，更新缓存
pub async fn poll_server_hello_map(
    reader: Arc<ServerHelloReader>,
    cache: SharedServerHelloCache,
    interval_secs: u64,
) {
    let mut interval = tokio::time::interval(tokio::time::Duration::from_secs(interval_secs));

    loop {
        interval.tick().await;
        match reader.dump_all() {
            Ok(entries) => {
                let count = entries.len();
                // 用新数据替换整个缓存（内部调用 compute_ja4s 计算指纹）
                cache.write().await.update(entries);

                // 打印每条 JA4S 指纹到终端
                let cache_ro = cache.read().await;
                for (key, entry) in cache_ro.iter() {
                    log::info!("JA4S {} -> {}", key, entry.fingerprint);
                }
                drop(cache_ro);

                // 输出调试计数器
                let counters = reader.dump_debug_counters().unwrap_or_default();
                log::info!(
                    "JA4S Map 轮询完成: {} 条 | 计数器: total={} tcp_443={} tls_hs={} sh={} map_ok={} alpn={} ext_overflow={} frag={}",
                    count,
                    counters[0], counters[1], counters[2], counters[3],
                    counters[4], counters[5], counters[6], counters[7]
                );
            }
            Err(e) => {
                log::error!("导出 JA4S BPF Map 失败: {}", e);
            }
        }
    }
}
