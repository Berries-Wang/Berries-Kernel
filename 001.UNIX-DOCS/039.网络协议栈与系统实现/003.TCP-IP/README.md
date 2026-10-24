# TCP-IP

> 源码树:[000.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3)(本文档系列行号均指此树)

## 目录

| 文档 | 主题 | 关键源文件 |
|-|-|-|
| [000.TCP三次握手与四次挥手-内核代码交互.md](./000.TCP三次握手与四次挥手-内核代码交互.md) | 连接建立/关闭全链路:三种套接字形态、半连接/全连接队列、SYN cookie、TIME_WAIT、状态机 | tcp_input.c, tcp_output.c, tcp_minisocks.c |

## 规划中的后续

- 数据传输:`tcp_rcv_established` 快/慢路径、SACK、乱序重排
- 拥塞控制:`tcp_cong.c` + BBR/CUBIC
- 定时器家族:`tcp_timer.c`(RTO/持久/保活/DELAYED ACK)
