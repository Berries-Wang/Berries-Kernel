//! JA4T Daemon — 基于 eBPF 的 TCP 指纹采集守护进程
//!
//! 加载两个 eBPF TC 分类器：
//!   1. ja4t_parse_syn (ingress): 解析入站 TCP SYN 包，提取 JA4T 指纹数据
//!      a. TCP Window Size（窗口大小）
//!      b. TCP Options Order（选项顺序列表）
//!      c. MSS Value（最大分段大小）
//!      d. Window Scale（窗口缩放因子）
//!      e. IP TTL（网络跳数推断）
//!      f. SYN-ACK RTT（往返延迟）
//!
//!   2. ja4s_parse_server_hello (egress): 解析出站 TLS Server Hello 包，提取 JA4S 指纹数据
//!      a. TLS Version（服务器 TLS 版本）
//!      b. Cipher Suite（服务器选中的密码套件）
//!      c. Extension Count（扩展数量，含 GREASE）
//!      d. ALPN（应用层协议协商结果）
//!      e. Extension Raw Types（扩展类型原始列表）
//!
//! 数据存入 BPF LRU Hash Map（内核侧），由本守护进程周期性轮询读取到内存缓存，
//! JA4S 指纹在用户态通过 ja4s 模块计算（SHA256 哈希），
//! 通过 Unix Domain Socket 统一提供给 nginx 查询使用。
//!
//! # 爬虫拦截原理
//!
//! JA4T 和 JA4S 分别在 TCP 层和 TLS 层捕捉客户端不可伪造的信号：
//!   - JA4T (TCP指纹): OS 内核决定的 TCP 选项顺序，用户态无法修改
//!   - JA4S (ServerHello): 服务器对 Client Hello 的响应，爬虫无法控制
//!
//! 双指纹联合使用可实现 {JA4, JA4S, JA4T} 三维交叉验证，
//! 任何一维偏离已知基线即触发拦截。

mod cli;
mod ja4s; // JA4S 指纹计算模块 (TLS Server Hello)
mod ja4t; // JA4T TCP 指纹采集模块
mod protocol;
mod server;

use anyhow::Result;
use clap::Parser;
use std::sync::Arc;
use tokio::sync::RwLock;

use crate::cli::Cli;
use crate::ja4s::cache::{ServerHelloCache, SharedServerHelloCache};
use crate::ja4s::loader::load_and_attach_server_hello;
use crate::ja4s::poller::poll_server_hello_map;
use crate::ja4t::cache::{ConnectionCache, SharedCache};
use crate::ja4t::loader::load_and_attach;
use crate::ja4t::poller::poll_bpf_map;
use crate::server::run_unix_socket;

#[tokio::main]
async fn main() -> Result<()> {
    let cli = Cli::parse();

    let log_level = if cli.verbose { "debug" } else { "info" };
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or(log_level)).init();

    log::info!("JA4T Daemon 启动中...");

    // 加载 eBPF 程序并挂载到网卡
    let (reader, _bpf) = load_and_attach(cli.bpf_object.as_ref(), &cli.interface)?;
    // _bpf 必须保持存活 —— 丢弃它会导致 TC 过滤器分离
    // BpfReader 用 Arc 包装，在 poll 任务和所有 socket handler 之间共享
    let reader = Arc::new(reader);
    let cache: SharedCache = Arc::new(RwLock::new(ConnectionCache::new()));

    // 启动后台轮询任务：定期从 BPF Map 导出数据并更新缓存
    let poll_cache = cache.clone();
    let poll_reader = reader.clone();
    let poll_interval = cli.poll_interval;
    tokio::spawn(async move {
        poll_bpf_map(poll_reader, poll_cache, poll_interval).await;
    });

    // 加载 JA4S Server Hello eBPF 程序并挂载到 egress
    let (sh_reader, _sh_bpf) =
        load_and_attach_server_hello(cli.bpf_server_hello_object.as_ref(), &cli.interface)?;
    let sh_reader = Arc::new(sh_reader);
    let sh_cache: SharedServerHelloCache = Arc::new(RwLock::new(ServerHelloCache::new()));

    // 启动 JA4S Server Hello 后台轮询任务
    let sh_poll_cache = sh_cache.clone();
    let sh_poll_reader = sh_reader.clone();
    let sh_poll_interval = cli.poll_interval;
    tokio::spawn(async move {
        poll_server_hello_map(sh_poll_reader, sh_poll_cache, sh_poll_interval).await;
    });

    // 启动 Unix Socket 服务器（阻塞当前线程）
    run_unix_socket(cli.socket, cache, reader, sh_cache, sh_reader).await?;

    Ok(())
}
