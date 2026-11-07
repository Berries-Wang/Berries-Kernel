# Linux 内核网络:以 e1000e 为标本的五层解剖<sup>LINUX 7.1.3 · E1000E</sup>

> 学习/出处:
> + 本仓库源码(全系列行号均指此树,已逐条 `grep -n` 实测):[000.SOURCE_CODE/003.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/)(Linux 7.1.3);
> + 驱动标本:[drivers/net/ethernet/intel/e1000e/netdev.c](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/drivers/net/ethernet/intel/e1000e/netdev.c)(约 8000 行,收发主链路全在其中)。

## 为什么用 e1000e 当标本

e1000 家族卡在"抽象文档"与"25G 驱动巨石代码"之间的甜点区:

- **概念完整**:PCI 设备模型、MMIO 寄存器、DMA、中断(INTx/MSI/MSI-X)、NAPI 轮询、环形缓冲区、`sk_buff`、ethtool、电源管理——一块网卡该有的内核机制全部齐备;
- **代码量适中**:e1000e 单文件(netdev.c)即可读通收发全链路,不像 ixgbe 被多队列/MSI-X 复杂度淹没;
- **离云很近**:e1000/e1000e 是 QEMU/KVM 虚拟机默认网卡型号,调试环境随手可得(本目录 [011.调试方案](011.调试方案-e1000e内核网络调试.md) 直接用 QEMU 复现);
- **是后辈的祖先**:igb/igc/ixgbe 与 e1000 架构同源,"环 + 描述符 + 门铃"一通百通。

## 目录

|文档|主题|对应五层|
|-|-|-|
|[000.全景-五层架构与关键数据结构](000.全景-五层架构与关键数据结构.md)|分层总览、对象关系图、收/发两条主链路、执行上下文|全部|
|[001.应用层-socket收发数据](001.应用层-socket收发数据.md)|`socket()/send/recv` 系统调用、socketfs 三层对象|① 应用层|
|[002.协议栈层-IP与协议分发](002.协议栈层-IP与协议分发.md)|`inet_protos`、`ip_rcv`、`ip_local_deliver`、路由、封装/解封装|② 协议栈(IP)|
|[003.协议栈层-TCP数据面](003.协议栈层-TCP数据面.md)|`tcp_sendmsg`/`tcp_v4_rcv`/`tcp_recvmsg`/`tcp_poll` 数据收发|② 协议栈(TCP)|
|[004.网络设备抽象层-net_device](004.网络设备抽象层-net_device.md)|`struct net_device`、`net_device_ops`、NAPI、`register_netdev`|③ 设备抽象层|
|[005.网卡驱动层-e1000e](005.网卡驱动层-e1000e.md)|`e1000e_open/xmit_frame/poll` 回调、描述符环、寄存器读写|④ 驱动层|
|[006.硬件层-MAC-PHY-PCIe](006.硬件层-MAC-PHY-PCIe.md)|MAC 控制器、PHY/MDIO、PCIe BAR/MMIO、DMA 硬件视角|⑤ 硬件层|
|[007.收包全链路-从网线到epoll](007.收包全链路-从网线到epoll.md)|DMA→硬中断→NAPI→sk_buff→协议栈→socket 队列→唤醒|⑥ 收包|
|[008.发包全链路-从sk_buff到网线](008.发包全链路-从sk_buff到网线.md)|sendmsg→TCP→IP→邻居→qdisc→xmit→DMA→完成中断|⑦ 发包|
|[009.epoll-事件驱动网络IO](009.epoll-事件驱动网络IO.md)|`eventpoll` 三件套、回调注册链、LT/ET、惊群|epoll 专题|
|[010.io_uring-异步网络IO](010.io_uring-异步网络IO.md)|`io_send/recv`、multishot、PBUF、NAPI busy-poll、零拷贝|io_uring 专题|
|[011.调试方案-e1000e内核网络调试](011.调试方案-e1000e内核网络调试.md)|QEMU+KGDB+ftrace+bpftrace 全套调试方案(含断点清单)|实战|

同目录相关:[003.TCP-IP(三次握手/四次挥手内核代码)](../003.TCP-IP/000.TCP三次握手与四次挥手-内核代码交互.md)、[002.IO_Uring](../002.IO_Uring/README.md)、[000.网络协议栈-学习资料索引](../000.网络协议栈-学习资料索引.md)、[051.CPU与DMA双向交互(网络收包事件全流程)](../../051.AI-ASSISANT/001.CPU与DMA双向交互%28网络收包事件全流程%29.md)。

## 五层全景一图

每层只和相邻层对话,层间契约分别是:系统调用(`socket` 层 ↔ 应用)、`proto_ops/proto` 结构(socket 层 ↔ 协议栈)、`net_device_ops`(协议栈 ↔ 驱动)、寄存器与 DMA 描述符(驱动 ↔ 硬件):

<pre>
┌─────────────────────────────────────────────────────────────────────────┐
│ ① 应用层       socket()/send()/recv()/epoll_wait/io_uring_enter        │  用户态
├─────────────── 系统调用边界 syscall ────────────────────────────────────┤
│ ② socket 层    net/socket.c  __sys_socket/__sys_sendto/sock_sendmsg    │
│                struct socket(net.h:137) + struct sock(sock.h)          │
├─────────────── proto_ops 回调(inet_stream_ops af_inet.c:1062) ─────────┤
│ ③ 协议栈层     TCP: tcp_sendmsg/tcp_v4_rcv    IP: ip_rcv/ip_queue_xmit │
│                分发表: inet_protos[] (protocol.c:27)  路由: fib/dst     │
├─────────────── net_device_ops 回调(netdevice.h:1436) ──────────────────┤
│ ④ 设备抽象层   struct net_device(netdevice.h:2124) 统一所有网卡      │
│                dev_queue_xmit / netif_receive_skb / NAPI(napi_struct)  │
├─────────────── 驱动实现回调(e1000e netdev.c:7334 e1000e_netdev_ops) ───┤
│ ⑤ 驱动层       e1000e: e1000e_open/e1000_xmit_frame/e1000e_poll        │
│                描述符环 + er32/ew32 读写寄存器(e1000.h:607)             │
├─────────────── MMIO 寄存器 + DMA 描述符 ────────────────────────────────┤
│ ⑥ 硬件层       MAC 控制器(82574/I219) + PHY(物理层芯片)                │
│                挂在 PCIe 总线上,BAR 空间映射寄存器,总线主控 DMA        │
└─────────────────────────────────────────────────────────────────────────┘
</pre>

**驱动的本质工作只有两件**(本文集以 7.1.3 代码逐行兑现):

1. **收包**:硬件 DMA 把帧写入内存 → 驱动构建 `sk_buff` → 交给协议栈(详见 [007](007.收包全链路-从网线到epoll.md));
2. **发包**:协议栈把 `sk_buff` 交给驱动 → 驱动映射 DMA → 通知硬件发送(详见 [008](008.发包全链路-从sk_buff到网线.md))。

## 阅读顺序建议

1. 先读 [000](000.全景-五层架构与关键数据结构.md) 建立骨架;
2. 顺着一条**发包**链路自顶向下读 001 → 003 → 002(封装) → 004 → 005 → 006(门铃寄存器);
3. 再顺着一条**收包**链路自底向上读 007 串起 006→005→004→002→003→001 全部层;
4. 最后读 009/010 两个专题,看"包到了 socket 队列之后"如何唤醒 epoll/io_uring 应用;
5. 对照 [011](011.调试方案-e1000e内核网络调试.md) 在 QEMU 里逐断点复现。

## 审核记录

|日期|事项|
|-|-|
|2026-11-06|建立本目录;全部行号按 7.1.3 树 `grep -n` 实测;e1000e 寄存器偏移以 [regs.h](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/drivers/net/ethernet/intel/e1000e/regs.h) 为准|
|2026-11-07|全量复核:①脚本化 1062 处 file:line 引用(强锚定口径:冒号前符号必须出现在被引行)逐条对 7.1.3 树实测(0 FAIL);②代码引文 903 行逐行对源码比对(0 不匹配);③修正 30 余处凭旧版内核(5.x/6.x)记忆引入的**伪引文**——7.1.3 实际为:`ip_route_input_noref(...,ip4h_dscp(iph),dev)`(ip_input.c:407)、`__inet_lookup_skb(skb,...)` 无 hashinfo 参数(:2112)、`__sys_socket` 经 `__sys_socket_create`+`update_socket_protocol`(:1807)、TCP 唤醒走 `tcp_data_ready`(:5661,定义 :5568)、`tcp_poll` 用 `tcp_stream_is_readable/__sk_stream_is_writeable`(:598/:602)、`th->source = inet->inet_sport` 无 htons(:1624)、queue_xmit 为 `INDIRECT_CALL_INET`(:1715)、`do_epoll_create` 用 `ep_alloc(&ep)`+`FD_PREPARE`(:2182/:2188)、epitem 无 `f` 字段(ffd :144)、eppoll_entry 为 next/base/wait/whead(:108)、`poll_wait` 判 `p->_qproc`(poll.h:44)、e1000e ISR 无 `likely(`(:1801)、`clean_rx_irq` 末参 `work_to_do`(:916)、dma_map 用 `adapter->rx_buffer_len`(:679)、补环经 `adapter->alloc_rx_buf` 函数指针(:1041)、TX 唤醒条件 `TX_WAKE_THRESHOLD`(:1271)、MDIC 读为手写 for 轮询(phy.c:146-161);二轮深检补修:`inet_add_protocol` 无 netns_ok 检查(protocol.c:33 纯 cmpxchg)、`rx_handler` 分派为 `switch(RX_HANDLER_*)`(dev.c:6089)、`update_socket_protocol` 为弱符号(:1795)、e1000e ICR LSC 判定在 :1759;三轮强锚定补修:`er32(ICR)` 在 :1756(非 :1759)、`tcp_stream_alloc_skb` 在 :1257、`tcp_v4_rcv` 二次 pull 后 th/iph 在 :2109/:2110、`__inet_lookup_skb` 在 :2112、图/表格单元格补齐符号名(如 `netdev.c:5942 writel`、`e1000_watchdog_task` 去 e)、`netif_wake_queue` 统一 :1279、`validate_xmit_skb_list` :4078;④相对链接 279 条全通;⑤外部文章出处痕迹已全部移除(用户要求)|
