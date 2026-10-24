//! Unix Socket 服务器 —— 处理 nginx 查询请求
//!
//! 查询流程（两级查找 + JA4S 并行查询）：
//! 1. 快速路径：查询内存缓存（ConnectionCache），命中则直接返回
//! 2. 兜底路径：缓存未命中时，直接调用 BPF Map lookup 查询内核 Map，
//!    消除轮询间隔导致的竞态窗口
//! 3. JA4S 查询：以反向五元组（server→client）查询 ServerHelloCache，
//!    注入 ja4s_fingerprint 字段

use anyhow::{Context, Result};
use std::os::unix::fs::PermissionsExt;
use std::path::PathBuf;
use std::sync::Arc;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader};
use tokio::net::UnixListener;

use crate::ja4s;
use crate::ja4s::cache::SharedServerHelloCache;
use crate::ja4s::reader::ServerHelloReader;
use crate::ja4s::types::Ja4sConnectionKey;
use crate::ja4t::cache::SharedCache;
use crate::ja4t::options::parse_tcp_options;
use crate::ja4t::reader::BpfReader;
use crate::ja4t::types::{ConnectionTuple, TcpOptionsInfo};
use crate::protocol::{LookupRequest, LookupResponse};

// ============================================================================
// 工具函数
// ============================================================================

/// 将点分十进制 IPv4 字符串转换为 u32（网络字节序）
fn parse_ipv4(s: &str) -> Result<u32> {
    let octets: Vec<&str> = s.split('.').collect();
    anyhow::ensure!(octets.len() == 4, "无效的 IPv4 地址: {}", s);
    let mut ip: u32 = 0;
    for (i, octet) in octets.iter().enumerate() {
        let v: u8 = octet.parse().context("无效字节")?;
        ip |= (v as u32) << (24 - 8 * i);
    }
    Ok(ip)
}

/// 根据 TCP 指纹信息构建查询响应
fn build_response(info: &TcpOptionsInfo) -> LookupResponse {
    let now_ns = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap_or_default()
        .as_nanos() as u64;
    let age_ms = if info.capture_ns > 0 && now_ns > info.capture_ns {
        (now_ns - info.capture_ns) / 1_000_000
    } else {
        0
    };
    // RTT 计算: (SYN-ACK 时间戳 - SYN 时间戳) / 1000 = 微秒
    // 仅在 SYN-ACK 已捕获且时间戳有效时返回
    let rtt_us = if info.synack_ns > 0 && info.synack_ns > info.capture_ns {
        Some((info.synack_ns - info.capture_ns) / 1000)
    } else {
        None
    };
    let parsed = parse_tcp_options(&info.raw_opts[..info.raw_opts_len as usize]);
    LookupResponse {
        found: true,
        window_size: Some(info.window_size),
        mss: parsed.mss,
        window_scale: parsed.window_scale,
        sack_permitted: parsed.sack_permitted,
        options_order: if parsed.options_order.is_empty() {
            None
        } else {
            Some(parsed.options_order)
        },
        capture_age_ms: Some(age_ms),
        ip_ttl: if info.ip_ttl > 0 {
            Some(info.ip_ttl)
        } else {
            None
        },
        rtt_us,
        ja4s_fingerprint: None, // JA4S 由调用方在 handle_client 中补充
    }
}

/// 构建"未找到"响应
fn not_found_response() -> LookupResponse {
    LookupResponse {
        found: false,
        window_size: None,
        mss: None,
        window_scale: None,
        sack_permitted: None,
        options_order: None,
        capture_age_ms: None,
        ip_ttl: None,
        rtt_us: None,
        ja4s_fingerprint: None,
    }
}

// ============================================================================
// 客户端处理器
// ============================================================================

/// 将 TlsServerHelloInfo 转换为 ja4s::Ja4sData 并计算指纹
fn compute_ja4s_from_info(info: &crate::ja4s::types::TlsServerHelloInfo) -> Option<String> {
    let ja4s_data = ja4s::Ja4sData {
        tls_version: info.tls_version,
        cipher_suite: info.cipher_suite,
        extension_count: info.extension_count,
        alpn: if info.alpn_len > 0 {
            let len = std::cmp::min(info.alpn_len as usize, 32);
            String::from_utf8_lossy(&info.alpn[..len])
                .trim_end_matches('\0')
                .to_string()
                .into()
        } else {
            None
        },
        extension_raw_types: {
            let count = std::cmp::min(info.ext_raw_len as usize, 120) / 2;
            (0..count)
                .map(|i| {
                    let idx = i * 2;
                    ((info.ext_raw[idx] as u16) << 8) | (info.ext_raw[idx + 1] as u16)
                })
                .collect()
        },
    };
    ja4s::compute_ja4s(&ja4s_data).map(|fp| fp.raw)
}

/// 处理单个 nginx 客户端的查询请求
///
/// 查询流程（两级查找 + JA4S 并行查询）：
/// 1. 快速路径：查询内存缓存（ConnectionCache），命中则直接返回
/// 2. 兜底路径：缓存未命中时，直接调用 BPF Map lookup 查询内核 Map，
///    消除轮询间隔导致的竞态窗口
/// 3. JA4S 查询：以反向五元组（server→client）查询 ServerHelloCache，
///    注入 ja4s_fingerprint 字段
pub async fn handle_client(
    stream: tokio::net::UnixStream,
    cache: SharedCache,
    bpf_reader: Arc<BpfReader>,
    server_hello_cache: SharedServerHelloCache,
    server_hello_reader: Arc<ServerHelloReader>,
) -> Result<()> {
    // 将 stream 拆分为读写两半（tokio 的 split 无需 Arc/Mutex）
    let (reader_half, mut writer_half) = stream.into_split();
    let mut reader = BufReader::new(reader_half);
    let mut line = String::new();

    // 读取一行 JSON 请求（nginx 发送的换行分隔 JSON）
    reader
        .read_line(&mut line)
        .await
        .context("socket 读取错误")?;

    let mut resp = match serde_json::from_str::<LookupRequest>(line.trim()) {
        Ok(req) => {
            // 解析源和目的 IP
            let Ok(src_ip) = parse_ipv4(&req.src_ip) else {
                return Ok(());
            };
            let Ok(dst_ip) = parse_ipv4(&req.dst_ip) else {
                return Ok(());
            };

            // 第 1 步：尝试从内存缓存查询（快速路径）
            let cache = cache.read().await;
            if let Some(info) = cache.lookup(src_ip, dst_ip, req.src_port, req.dst_port) {
                build_response(info)
            } else {
                // 第 2 步：缓存未命中 —— 兜底查询 BPF Map
                // 先释放缓存读锁，避免阻塞轮询任务的写操作
                drop(cache);
                let key = ConnectionTuple {
                    src_ip,
                    dst_ip,
                    src_port: req.src_port,
                    dst_port: req.dst_port,
                    protocol: 6, // TCP
                    _pad: [0; 3],
                };
                match bpf_reader.lookup_direct(&key) {
                    Ok(Some(info)) => build_response(&info),
                    Ok(None) => not_found_response(),
                    Err(e) => {
                        log::info!("BPF 直接查询错误: {}", e);
                        not_found_response()
                    }
                }
            }
        }
        Err(_) => not_found_response(),
    };

    // ================================================================
    // 第 3 步：JA4S Server Hello 指纹查询（独立于 JA4T SYN 查询）
    //
    // nginx 请求的五元组是 client→server（ingress 视角），
    // JA4S Map Key 是 server→client（egress 视角），因此需要反转。
    // ================================================================
    if let Ok(req) = serde_json::from_str::<LookupRequest>(line.trim()) {
        if let (Ok(server_ip), Ok(client_ip)) = (parse_ipv4(&req.dst_ip), parse_ipv4(&req.src_ip)) {
            // JA4S 快速路径：查询内存缓存
            let sh_cache = server_hello_cache.read().await;
            if let Some(entry) = sh_cache.lookup(server_ip, client_ip, req.dst_port, req.src_port) {
                resp.ja4s_fingerprint = Some(entry.fingerprint.clone());
            } else {
                // JA4S 兜底路径：直接查询 BPF Map
                drop(sh_cache);
                let ja4s_key = Ja4sConnectionKey {
                    src_ip: server_ip,
                    dst_ip: client_ip,
                    src_port: req.dst_port,
                    dst_port: req.src_port,
                    protocol: 6,
                    _pad: [0; 3],
                };
                if let Ok(Some(info)) = server_hello_reader.lookup_direct(&ja4s_key) {
                    resp.ja4s_fingerprint = compute_ja4s_from_info(&info);
                }
            }
        }
    }

    // 返回 JSON 响应（换行分隔）
    let mut json = serde_json::to_string(&resp)?;
    json.push('\n');
    writer_half
        .write_all(json.as_bytes())
        .await
        .context("socket 写入错误")?;

    Ok(())
}

// ============================================================================
// Socket 服务器
// ============================================================================

/// 启动 Unix Domain Socket 服务器
///
/// 绑定到指定路径，监听 nginx 连接，为每个客户端 spawn 一个异步任务处理查询
pub async fn run_unix_socket(
    socket_path: PathBuf,
    cache: SharedCache,
    bpf_reader: Arc<BpfReader>,
    server_hello_cache: SharedServerHelloCache,
    server_hello_reader: Arc<ServerHelloReader>,
) -> Result<()> {
    // 清理残留的旧 socket 文件
    if socket_path.exists() {
        std::fs::remove_file(&socket_path).context("删除旧 socket 文件失败")?;
    }
    if let Some(parent) = socket_path.parent() {
        std::fs::create_dir_all(parent).ok();
    }

    let listener = UnixListener::bind(&socket_path).context("绑定 Unix Socket 失败")?;
    // 设置 666 权限，允许 nginx worker 进程读写
    std::fs::set_permissions(&socket_path, std::fs::Permissions::from_mode(0o666))
        .context("设置 socket 权限失败")?;

    log::info!("监听 {}", socket_path.display());

    loop {
        match listener.accept().await {
            Ok((stream, _peer_addr)) => {
                let cache = cache.clone();
                let bpf_reader = bpf_reader.clone();
                let sh_cache = server_hello_cache.clone();
                let sh_reader = server_hello_reader.clone();
                tokio::spawn(async move {
                    if let Err(e) =
                        handle_client(stream, cache, bpf_reader, sh_cache, sh_reader).await
                    {
                        log::info!("客户端处理错误: {}", e);
                    }
                });
            }
            Err(e) => {
                log::error!("Accept 错误: {}", e);
            }
        }
    }
}
