//! JA4S 指纹计算模块 — TLS Server Hello 响应指纹生成
//!
//! 根据 eBPF 侧采集的 TLS Server Hello 原始数据，在用户态计算 JA4S 指纹。
//! JA4S 是 JA4+ 套件中的**服务器响应指纹**，与 JA4 (Client Hello) 配对使用
//! 实现 TLS 双向指纹验证。
//!
//! # JA4S 指纹格式
//!
//! ```text
//! t{ver}{#exts}{alpn}_{cipher}_{ext_hash}
//!
//! 示例: "t1303h2_1301_abc123def456"
//!       ││││││ │──│ ──────────
//!       ││││││   │     └─ SHA256(扩展type原始顺序)[:12]
//!       ││││││   └─────── 密码套件 hex lowercase (单个)
//!       ││││└┴──────────── ALPN 首尾字母数字 (无则 "00")
//!       │││└────────────── 扩展数量 2 位 (含 GREASE)
//!       │└└─────────────── TLS 版本 ("13", "12" 等)
//!       └──────────────── Protocol prefix 't'
//! ```
//!
//! # 爬虫拦截原理
//!
//! TLS 握手是一个由客户端发起、服务器响应的双向协商过程。爬虫面临的
//! 一个根本性约束是：**爬虫只能控制 Client Hello (JA4)，无法控制 Server Hello
//! (JA4S)**。Server Hello 由服务器 TLS 栈根据客户端参数自动生成。
//!
//! 不同 TLS 实现库（BoringSSL/Chrome, OpenSSL/curl, Go/crypto/tls, Java/JSSE）
//! 即使构造相同的 JA4 指纹，由于内部实现差异（扩展顺序、GREASE 策略、
//! 密码套件优先级），会诱导服务器返回不同的 Server Hello → JA4S 不同。
//!
//! 因此，JA4S 形成了一道**被动防线**：
//!   1. 爬虫伪造 JA4 → 服务器返回与伪造 JA4 匹配的 JA4S
//!   2. 后端比较 {JA4, JA4S} 配对是否在已知浏览器基线中
//!   3. 配对偏离基线 → Client Hello 是手动构造的（非真实浏览器）→ 拦截
//!
//! # 数据流
//!
//! ```text
//! eBPF TC egress (tls_server_hello.bpf.c)
//!   → tls_server_hello_map (BPF LRU Hash Map)
//!     → ja4t-daemon 轮询 + compute_ja4s()（本模块）
//!       → Unix Socket → Nginx → X-JA4S-Fingerprint header
//!         → App Server Ja4SHandler 验证
//! ```
//!
//! 参考：
//!   - FoxIO-LLC/ja4/python/ja4.py:170-215
//!   - FoxIO-LLC/ja4/rust/ja4/src/tls.rs:406-479

// ============================================================================
// 子模块
// ============================================================================

pub mod cache;
mod compute;
mod constants;
pub mod loader;
pub mod poller;
pub mod reader;
pub mod types;

// ============================================================================
// 公共 API 重导出
// ============================================================================

pub use compute::compute_ja4s;
pub use types::Ja4sData;
