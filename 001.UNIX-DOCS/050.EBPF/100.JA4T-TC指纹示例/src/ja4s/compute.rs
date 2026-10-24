//! JA4S 指纹计算与解析
//!
//! JA4S 指纹格式: t{ver}{#exts}{alpn}_{cipher}_{ext_hash}
//!
//! # 算法概要（8 步）
//!
//! 1. Protocol prefix = "t"
//! 2. TLS version mapping (TLS_MAPPER)
//! 3. Extension count: min(x, 99)，两位数字，**含 GREASE**
//! 4. ALPN: 首尾字母数字，无则 "00"
//! 5. Part A = "t" + ver + #exts + alpn
//! 6. Part B = cipher hex lowercase（单个，非列表 hash）
//! 7. Part C = SHA256(扩展 type 原始顺序逗号拼接)[:12]（含 GREASE，不排序）
//! 8. JA4S = PartA + "_" + PartB + "_" + PartC

use sha2::{Digest, Sha256};

use super::constants::{GREASE_VALUES, TLS_MAPPER};
use super::types::{Ja4sData, Ja4sFields, Ja4sFingerprint, Ja4sFieldsDebug};

/// 计算 JA4S 指纹
///
/// JA4S 是 JA4+ 套件中的**服务器响应指纹**。与 JA4 (Client Hello) 配对使用，
/// 实现 TLS 双向指纹验证。爬虫无法控制 Server Hello 的内容，因此 JA4S
/// 形成了一道被动防线。
///
/// 参考 FoxIO-LLC/ja4 实现：
///   - python/ja4.py:170-215 (to_ja4s)
///   - rust/ja4/src/tls.rs:406-479 (ServerStats::into_out)
///
/// # JA4 vs JA4S 关键差异
///
/// | 维度 | JA4 (客户端) | JA4S (服务器) |
/// |------|-------------|----------------|
/// | 密码套件 | 列表排序后 hash | **单个 cipher hex** |
/// | Extension 计数 | 排除 GREASE | **包含 GREASE** |
/// | Extension hash | 排序, 过滤 GREASE/SNI/ALPN | **原始顺序, 含 GREASE** |
pub fn compute_ja4s(data: &Ja4sData) -> Option<Ja4sFingerprint> {
    // Step 1: Protocol prefix (fixed for TCP)
    let protocol = "t";

    // Step 2: TLS version mapping
    // 优先使用 supported_versions 扩展中的最高版本，否则用 handshake_version
    // 当前数据来自 eBPF 采集的 Server Hello，直接使用 tls_version 字段
    let ver = TLS_MAPPER
        .iter()
        .find(|(v, _)| *v == data.tls_version)
        .map(|(_, s)| *s)
        .unwrap_or("00");

    // Step 3: Extension count（含 GREASE，min(x, 99)）
    let ext_count = std::cmp::min(data.extension_count as usize, 99);
    let ext_str = format!("{:02}", ext_count);

    // Step 4: ALPN — 第一个 ALPN 值的首尾字符
    // 参考 FoxIO 实现:
    //   Python: alpn[0] + alpn[-1] (len>2), 非 ASCII → '99'
    //   Rust:   first_last() 取首尾, 非 ASCII → '9'
    let alpn_str = data
        .alpn
        .as_deref()
        .filter(|s| !s.is_empty())
        .map(|s| {
            let chars: Vec<char> = s.chars().collect();
            if chars.len() > 2 {
                let first = if chars[0].is_ascii() { chars[0] } else { '9' };
                let last = if chars[chars.len() - 1].is_ascii() { chars[chars.len() - 1] } else { '9' };
                format!("{}{}", first, last)
            } else {
                s.to_string()
            }
        })
        .filter(|s| !s.is_empty())
        .unwrap_or_else(|| "00".to_string());

    // Step 5: Part A = "t{ver}{#exts}{alpn}"
    let part_a = format!("{}{}{}{}", protocol, ver, ext_str, alpn_str);

    // Step 6: Part B = cipher hex lowercase (无 "0x" 前缀)
    let cipher = format!("{:04x}", data.cipher_suite);

    // Step 7: Part C = extension hash
    // 扩展 type 按原始顺序 → 逗号连接 → SHA256[:12]
    // 包含 GREASE，不排序，不过滤
    let ext_list: Vec<String> = data
        .extension_raw_types
        .iter()
        .map(|t| format!("{:04x}", t))
        .collect();
    let ext_str_raw = ext_list.join(",");

    let ext_hash = if ext_str_raw.is_empty() {
        "000000000000".to_string()
    } else {
        let mut hasher = Sha256::new();
        hasher.update(ext_str_raw.as_bytes());
        let result = hasher.finalize();
        hex::encode(&result[..6]) // 取前 12 个 hex 字符
    };

    // Step 8: JA4S = PartA + "_" + PartB + "_" + PartC
    let raw = format!("{}_{}_{}", part_a, cipher, ext_hash);

    Some(Ja4sFingerprint {
        raw: raw.clone(),
        fields: Ja4sFieldsDebug {
            protocol: protocol.to_string(),
            version: ver.to_string(),
            extension_count: data.extension_count,
            alpn: alpn_str,
            cipher,
            extension_hash: ext_hash,
        },
    })
}

/// 解析 JA4S 指纹字符串为结构化字段
///
/// 格式: "t{ver}{#exts}{alpn}_{cipher}_{ext_hash}"
/// 示例: "t1303h2_1301_abc123def456"
#[allow(dead_code)]
pub fn parse_ja4s(raw: &str) -> Option<Ja4sFields> {
    let parts: Vec<&str> = raw.split('_').collect();
    if parts.len() != 3 {
        return None;
    }

    let part_a = parts[0]; // "t1303h2"
    let part_b = parts[1]; // "1301"
    let part_c = parts[2]; // "abc123def456"

    if part_a.len() < 6 {
        return None;
    }

    let chars: Vec<char> = part_a.chars().collect();
    let protocol = chars[0]; // 't'
    let version = chars[1..3].iter().collect::<String>(); // "13"
    let ext_count = chars[3..5].iter().collect::<String>(); // "03"
    let alpn = chars[5..].iter().collect::<String>(); // "h2" or "00"

    Some(Ja4sFields {
        protocol,
        version,
        extension_count_str: ext_count,
        alpn,
        cipher: part_b.to_string(),
        extension_hash: part_c.to_string(),
    })
}

/// 判断扩展类型是否为 GREASE 值（RFC 8701）
#[allow(dead_code)]
pub fn is_grease(ext_type: u16) -> bool {
    GREASE_VALUES.contains(&ext_type)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn make_data_v1() -> Ja4sData {
        Ja4sData {
            tls_version: 0x0304,
            cipher_suite: 0x1301,
            extension_count: 3,
            alpn: Some("h2".to_string()),
            extension_raw_types: vec![0x002b, 0x0a0a, 0x0033],
        }
    }

    fn make_data_v2() -> Ja4sData {
        Ja4sData {
            tls_version: 0x0303,
            cipher_suite: 0xc02f,
            extension_count: 2,
            alpn: None,
            extension_raw_types: vec![0xff01, 0x002b],
        }
    }

    fn make_data_v3() -> Ja4sData {
        Ja4sData {
            tls_version: 0x0301,
            cipher_suite: 0x0005,
            extension_count: 0,
            alpn: None,
            extension_raw_types: vec![],
        }
    }

    #[test]
    fn test_compute_ja4s_tls13() {
        let fp = compute_ja4s(&make_data_v1()).expect("should compute");
        assert!(fp.raw.starts_with("t1303"));
        assert!(fp.raw.contains("h2"));
        assert!(fp.raw.contains("1301"));
        assert_eq!(fp.raw.split('_').count(), 3);
    }

    #[test]
    fn test_compute_ja4s_no_alpn() {
        let fp = compute_ja4s(&make_data_v2()).expect("should compute");
        assert!(fp.raw.starts_with("t1202"));
        assert!(fp.raw.contains("00"));
        assert!(fp.raw.contains("c02f"));
    }

    #[test]
    fn test_compute_ja4s_empty_extensions() {
        let fp = compute_ja4s(&make_data_v3()).expect("should compute");
        assert!(fp.raw.starts_with("t1000"));
        assert!(fp.raw.ends_with("000000000000"));
    }
}
