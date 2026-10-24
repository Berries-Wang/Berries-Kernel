//! eBPF 加载器 —— 编译/加载 JA4S BPF 程序并挂载到网卡 egress

use anyhow::{Context, Result};
use std::path::PathBuf;

use super::reader::ServerHelloReader;

/// 加载 JA4S Server Hello BPF 目标文件，将 TC 分类器挂载到网卡 egress 路径
///
/// Server Hello 是服务器发送的响应（egress 方向），因此挂载到 egress。
/// 返回 ServerHelloReader（用于读取 Map）和 aya::Bpf（必须保持存活）。
pub fn load_and_attach_server_hello(
    bpf_obj_path: Option<&PathBuf>,
    interface: &str,
) -> Result<(ServerHelloReader, aya::Bpf)> {
    use aya::programs::{tc::SchedClassifier, TcAttachType};
    use aya::BpfLoader;

    let mut loader = BpfLoader::new();

    let mut bpf = if let Some(path) = bpf_obj_path {
        log::info!("从文件加载 JA4S BPF 目标: {}", path.display());
        loader.load_file(path)?
    } else {
        let obj_path = option_env!("EBPF_SERVER_HELLO_PATH").unwrap_or("/nonexistent");
        log::info!("加载嵌入式 JA4S BPF 目标: {}", obj_path);
        loader.load_file(obj_path)?
    };

    // 获取 JA4S TC 分类器程序
    let prog: &mut SchedClassifier = bpf
        .program_mut("ja4s_parse_server_hello")
        .context("程序中未找到 'ja4s_parse_server_hello'")?
        .try_into()
        .context("程序不是 TC 分类器")?;

    // 加载 BPF 程序并通过验证器
    prog.load()?;
    // 挂载到指定网卡的 egress（出站路径，捕获 Server Hello）
    prog.attach(interface, TcAttachType::Egress)?;
    log::info!("JA4S TC 分类器已挂载到 {} (egress)", interface);

    // 取出 Map 句柄
    let map = bpf
        .take_map("tls_server_hello_map")
        .context("Map 'tls_server_hello_map' 未在 BPF 目标中找到")?;
    let debug_map = bpf
        .take_map("ja4s_debug_counters")
        .context("Map 'ja4s_debug_counters' 未在 BPF 目标中找到")?;

    Ok((ServerHelloReader::new(map, debug_map), bpf))
}
