//! JA4T 模块 — TCP SYN 指纹采集与查询
//!
//! JA4T (TCP Fingerprint) 通过分析 TCP SYN 包的 Option 字段来识别客户端 OS。
//! 不同操作系统的 TCP/IP 协议栈在 TCP Option 类型顺序上存在显著差异，
//! 这些差异由内核实现决定，用户态程序无法修改，因此是可靠的指纹信号。
//!
//! # 子模块
//!
//! - `types`: BPF Map 的 C 兼容结构体（ConnectionTuple, TcpOptionsInfo）
//! - `options`: TCP Option 解析器
//! - `reader`: BPF Map 读取器（BpfReader）
//! - `cache`: 线程安全的内存连接缓存（ConnectionCache）
//! - `poller`: 后台轮询任务
//! - `loader`: eBPF 程序加载与 TC 挂载

pub mod cache;
pub mod loader;
pub mod options;
pub mod poller;
pub mod reader;
pub mod types;
