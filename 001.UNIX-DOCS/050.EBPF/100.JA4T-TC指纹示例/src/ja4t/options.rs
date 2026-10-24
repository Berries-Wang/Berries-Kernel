//! TCP Option 解析器（在用户态运行，由 BPF 侧传来的原始字节驱动）
//!
//! TCP Option 格式（RFC 793 / RFC 7323）：
//!   - Kind=0 (EOL): 单字节，表示选项结束
//!   - Kind=1 (NOP): 单字节，无操作填充
//!   - Kind>=2:     第 1 字节=Kind, 第 2 字节=Length(含自身), 后续=数据

const TCPOPT_EOL: u8 = 0; // End of Option List —— 选项列表结束
const TCPOPT_NOP: u8 = 1; // No Operation —— 填充字节
const TCPOPT_MSS: u8 = 2; // Maximum Segment Size
const TCPOPT_WINDOW_SCALE: u8 = 3; // Window Scale Factor
const TCPOPT_SACK_PERM: u8 = 4; // SACK Permitted
const MAX_TCP_OPTIONS: usize = 12; // 最多提取 12 种 Option（匹配 p0f 行为）

/// 解析后的 TCP Option
pub struct ParsedTcpOptions {
    pub mss: Option<u16>,           // MSS 值（Option kind=2, len=4）
    pub window_scale: Option<u8>,   // 窗口缩放因子（Option kind=3, len=3）
    pub sack_permitted: Option<u8>, // SACK 允许标志（Option kind=4, len=2）
    pub options_order: Vec<u8>,     // Option 类型顺序列表（如 [2,4,8,3]）
}

/// 解析原始 TCP Option 字节
///
/// TCP Option 格式（RFC 793 / RFC 7323）：
///   - Kind=0 (EOL): 单字节，表示选项结束
///   - Kind=1 (NOP): 单字节，无操作填充
///   - Kind>=2:     第 1 字节=Kind, 第 2 字节=Length(含自身), 后续=数据
pub fn parse_tcp_options(raw: &[u8]) -> ParsedTcpOptions {
    let len = raw.len();
    let mut pos: usize = 0;
    let mut mss = None;
    let mut window_scale = None;
    let mut sack_permitted = None;
    let mut order: Vec<u8> = Vec::with_capacity(MAX_TCP_OPTIONS);

    while pos < len && order.len() < MAX_TCP_OPTIONS {
        let kind = raw[pos];

        // EOL: 选项列表结束
        if kind == TCPOPT_EOL {
            break;
        }

        // NOP: 单字节填充，跳过
        if kind == TCPOPT_NOP {
            order.push(kind);
            pos += 1;
            continue;
        }

        order.push(kind);

        // 至少还需要 1 字节读取 Length 字段
        if pos + 1 >= len {
            break;
        }

        let opt_len = raw[pos + 1] as usize;
        // Length 必须 >= 2，且不能越界
        if opt_len < 2 || pos + opt_len > len {
            break;
        }

        match kind {
            // MSS: kind=2, len=4, 2 字节值（网络字节序）
            TCPOPT_MSS if opt_len == 4 && pos + 3 < len => {
                mss = Some(((raw[pos + 2] as u16) << 8) | (raw[pos + 3] as u16));
            }
            // Window Scale: kind=3, len=3, 1 字节值
            TCPOPT_WINDOW_SCALE if opt_len == 3 && pos + 2 < len => {
                window_scale = Some(raw[pos + 2]);
            }
            // SACK Permitted: kind=4, len=2, 无数据
            TCPOPT_SACK_PERM if opt_len == 2 => {
                sack_permitted = Some(1);
            }
            // 其他 Option 类型仅记录到 order 中，不解析具体值
            _ => {}
        }

        pos += opt_len;
    }

    ParsedTcpOptions {
        mss,
        window_scale,
        sack_permitted,
        options_order: order,
    }
}
