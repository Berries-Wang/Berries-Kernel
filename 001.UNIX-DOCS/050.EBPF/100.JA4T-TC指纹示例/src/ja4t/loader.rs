//! eBPF 加载器 —— 编译/加载 JA4T BPF 程序并挂载到网卡 ingress

use anyhow::{Context, Result};
use std::path::PathBuf;

use super::reader::BpfReader;

/// 加载 BPF 目标文件，将 TC 分类器挂载到网卡 ingress 路径
///
/// 返回 BpfReader（用于读取 Map）和 aya::Bpf（必须保持存活，否则 TC 过滤器会分离）
pub fn load_and_attach(
    bpf_obj_path: Option<&PathBuf>,
    interface: &str,
) -> Result<(BpfReader, aya::Bpf)> {
    use aya::programs::{tc::SchedClassifier, TcAttachType};
    use aya::BpfLoader;

    let mut loader = BpfLoader::new();

    let mut bpf = if let Some(path) = bpf_obj_path {
        log::info!("从文件加载 BPF 目标: {}", path.display());
        loader.load_file(path)?
    } else {
        // 从编译时嵌入的路径加载（由 build.rs 设置 EBPF_OBJECT_PATH 环境变量）
        let obj_path = option_env!("EBPF_OBJECT_PATH").unwrap_or("/nonexistent");
        log::info!("加载嵌入式 BPF 目标: {}", obj_path);
        loader.load_file(obj_path)?
    };

    // 获取 TC 分类器程序
    let prog: &mut SchedClassifier = bpf
        .program_mut("ja4t_parse_syn")
        .context("程序中未找到 'ja4t_parse_syn'")?
        .try_into()
        .context("程序不是 TC 分类器")?;

    // 加载 BPF 程序并通过验证器
    prog.load()?;
    // 挂载到指定网卡的 ingress（入站路径）
    prog.attach(interface, TcAttachType::Ingress)?;
    log::info!("TC 分类器已挂载到 {} (ingress)", interface);

    // 取出 Map 句柄（所有权转移给 BpfReader）
    let map = bpf
        .take_map("tcp_options_map")
        .context("Map 'tcp_options_map' 未在 BPF 目标中找到")?;
    let debug_map = bpf
        .take_map("debug_counters")
        .context("Map 'debug_counters' 未在 BPF 目标中找到")?;

    Ok((BpfReader::new(map, debug_map), bpf))
}
