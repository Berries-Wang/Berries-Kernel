//! CLI 参数定义

use clap::Parser;
use std::path::PathBuf;

#[derive(Parser, Debug)]
#[command(name = "ja4t-daemon")]
pub struct Cli {
    /// 要挂载 TC 分类器的网络接口
    #[arg(short, long, default_value = "eth0")]
    pub interface: String,

    /// Unix Domain Socket 路径（nginx 通过此路径查询）
    #[arg(short, long, default_value = "/var/run/ja4t.sock")]
    pub socket: PathBuf,

    /// BPF Map 轮询间隔（秒）
    #[arg(long, default_value = "5")]
    pub poll_interval: u64,

    /// 详细日志输出
    #[arg(short, long)]
    pub verbose: bool,

    /// 编译后的 BPF 目标文件路径（默认自动检测）
    #[arg(long)]
    pub bpf_object: Option<PathBuf>,

    /// TLS Server Hello JA4S BPF 目标文件路径（默认自动检测）
    #[arg(long)]
    pub bpf_server_hello_object: Option<PathBuf>,
}
