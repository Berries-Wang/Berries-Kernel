.. SPDX-License-Identifier: GPL-2.0

==================
Generic HDLC layer
==================

(通用 HDLC 层)

Krzysztof Halasa <khc@pm.waw.pl>
(Krzysztof Halasa <khc@pm.waw.pl>)


Generic HDLC layer currently supports:
(通用 HDLC 层目前支持：)

1. Frame Relay (ANSI, CCITT, Cisco and no LMI)
   (1. 帧中继 (Frame Relay)（ANSI、CCITT、Cisco 以及无 LMI）)

   - Normal (routed) and Ethernet-bridged (Ethernet device emulation)
     interfaces can share a single PVC.
     (普通（路由）接口和以太网桥接（以太网设备模拟）接口可以共享单个 PVC。)
   - ARP support (no InARP support in the kernel - there is an
     experimental InARP user-space daemon available on:
     http://www.kernel.org/pub/linux/utils/net/hdlc/).
     (支持 ARP（内核不支持 InARP - 有一个实验性的 InARP 用户态守护进程，可从
     http://www.kernel.org/pub/linux/utils/net/hdlc/ 获取）。)

2. raw HDLC - either IP (IPv4) interface or Ethernet device emulation
   (2. raw HDLC - IP (IPv4) 接口或以太网设备模拟)
3. Cisco HDLC
   (3. Cisco HDLC)
4. PPP
   (4. PPP)
5. X.25 (uses X.25 routines).
   (5. X.25（使用 X.25 例程）。)

Generic HDLC is a protocol driver only - it needs a low-level driver
for your particular hardware.
(通用 HDLC 只是一个协议驱动 - 它需要针对你特定硬件的低层驱动。)

Ethernet device emulation (using HDLC or Frame-Relay PVC) is compatible
with IEEE 802.1Q (VLANs) and 802.1D (Ethernet bridging).
(以太网设备模拟（使用 HDLC 或帧中继 PVC）与 IEEE 802.1Q (VLAN) 和 802.1D（以太网桥接）兼容。)


Make sure the hdlc.o and the hardware driver are loaded. It should
create a number of "hdlc" (hdlc0 etc) network devices, one for each
WAN port. You'll need the "sethdlc" utility, get it from:
(确保已加载 hdlc.o 和硬件驱动。它应该会创建若干个 "hdlc"（hdlc0 等）网络设备，每个 WAN 端口一个。你还需要 "sethdlc" 工具，可从以下地址获取：)

	http://www.kernel.org/pub/linux/utils/net/hdlc/

Compile sethdlc.c utility::
(编译 sethdlc.c 工具：)

	gcc -O2 -Wall -o sethdlc sethdlc.c

Make sure you're using a correct version of sethdlc for your kernel.
(确保所用 sethdlc 版本与你的内核匹配。)

Use sethdlc to set physical interface, clock rate, HDLC mode used,
and add any required PVCs if using Frame Relay.
Usually you want something like::
(使用 sethdlc 设置物理接口、时钟速率、所用的 HDLC 模式，如果使用帧中继还须添加所需的 PVC。通常你会想要类似下面的命令：)

	sethdlc hdlc0 clock int rate 128000
	sethdlc hdlc0 cisco interval 10 timeout 25

or::
(或：)

	sethdlc hdlc0 rs232 clock ext
	sethdlc hdlc0 fr lmi ansi
	sethdlc hdlc0 create 99
	ifconfig hdlc0 up
	ifconfig pvc0 localIP pointopoint remoteIP

In Frame Relay mode, ifconfig master hdlc device up (without assigning
any IP address to it) before using pvc devices.
(在帧中继模式下，使用 pvc 设备之前，先用 ifconfig 把主 hdlc 设备置为 up（不要给它分配任何 IP 地址）。)


Setting interface:
(设置接口：)

* v35 | rs232 | x21 | t1 | e1
  (v35 | rs232 | x21 | t1 | e1)

    - sets physical interface for a given port
      if the card has software-selectable interfaces
      (为给定端口设置物理接口（当板卡支持软件选择接口时）)

  loopback
  (loopback)

    - activate hardware loopback (for testing only)
      (激活硬件环回（仅用于测试）)
* clock ext
  (clock ext)

    - both RX clock and TX clock external
      (RX 时钟和 TX 时钟都为外部)
* clock int
  (clock int)

    - both RX clock and TX clock internal
      (RX 时钟和 TX 时钟都为内部)
* clock txint
  (clock txint)

    - RX clock external, TX clock internal
      (RX 时钟为外部，TX 时钟为内部)
* clock txfromrx
  (clock txfromrx)

    - RX clock external, TX clock derived from RX clock
      (RX 时钟为外部，TX 时钟由 RX 时钟派生)
* rate
  (rate)

    - sets clock rate in bps (for "int" or "txint" clock only)
      (设置以 bps 为单位的时钟速率（仅用于 "int" 或 "txint" 时钟）)


Setting protocol:
(设置协议：)

* hdlc - sets raw HDLC (IP-only) mode
  (hdlc - 设置 raw HDLC（仅 IP）模式)

  nrz / nrzi / fm-mark / fm-space / manchester - sets transmission code
  (nrz / nrzi / fm-mark / fm-space / manchester - 设置传输码型)

  no-parity / crc16 / crc16-pr0 (CRC16 with preset zeros) / crc32-itu
  (no-parity / crc16 / crc16-pr0（预置零的 CRC16）/ crc32-itu)

  crc16-itu (CRC16 with ITU-T polynomial) / crc16-itu-pr0 - sets parity
  (crc16-itu（ITU-T 多项式的 CRC16）/ crc16-itu-pr0 - 设置奇偶校验)

* hdlc-eth - Ethernet device emulation using HDLC. Parity and encoding
  as above.
  (hdlc-eth - 使用 HDLC 的以太网设备模拟。奇偶校验和编码同上。)

* cisco - sets Cisco HDLC mode (IP, IPv6 and IPX supported)
  (cisco - 设置 Cisco HDLC 模式（支持 IP、IPv6 和 IPX）)

  interval - time in seconds between keepalive packets
  (interval - keepalive 数据包之间的时间间隔（秒）)

  timeout - time in seconds after last received keepalive packet before
	    we assume the link is down
	    (timeout - 收到最后一个 keepalive 数据包后经过多少秒才认为链路已断开)

* ppp - sets synchronous PPP mode
  (ppp - 设置同步 PPP 模式)

* x25 - sets X.25 mode
  (x25 - 设置 X.25 模式)

* fr - Frame Relay mode
  (fr - 帧中继模式)

  lmi ansi / ccitt / cisco / none - LMI (link management) type
  (lmi ansi / ccitt / cisco / none - LMI（链路管理）类型)

  dce - Frame Relay DCE (network) side LMI instead of default DTE (user).
  (dce - 使用帧中继 DCE（网络）侧的 LMI，而不是默认的 DTE（用户）。)

  It has nothing to do with clocks!
  (这与时钟毫无关系！)

  - t391 - link integrity verification polling timer (in seconds) - user
    (t391 - 链路完整性验证轮询定时器（秒）- 用户侧)
  - t392 - polling verification timer (in seconds) - network
    (t392 - 轮询验证定时器（秒）- 网络侧)
  - n391 - full status polling counter - user
    (n391 - 完整状态轮询计数器 - 用户侧)
  - n392 - error threshold - both user and network
    (n392 - 错误阈值 - 用户侧和网络侧)
  - n393 - monitored events count - both user and network
    (n393 - 监视事件计数 - 用户侧和网络侧)

Frame-Relay only:
(仅限帧中继：)

* create n | delete n - adds / deletes PVC interface with DLCI #n.
  Newly created interface will be named pvc0, pvc1 etc.
  (create n | delete n - 添加/删除 DLCI 编号为 n 的 PVC 接口。新创建的接口将命名为 pvc0、pvc1 等。)

* create ether n | delete ether n - adds a device for Ethernet-bridged
  frames. The device will be named pvceth0, pvceth1 etc.
  (create ether n | delete ether n - 为以太网桥接帧添加一个设备。该设备将命名为 pvceth0、pvceth1 等。)




Board-specific issues
(板卡特定问题)
---------------------

n2.o and c101.o need parameters to work::
(n2.o 和 c101.o 需要参数才能工作：)

	insmod n2 hw=io,irq,ram,ports[:io,irq,...]

example::
(例如：)

	insmod n2 hw=0x300,10,0xD0000,01

or::
(或：)

	insmod c101 hw=irq,ram[:irq,...]

example::
(例如：)

	insmod c101 hw=9,0xdc000

If built into the kernel, these drivers need kernel (command line) parameters::
(如果编入内核，这些驱动需要内核（命令行）参数：)

	n2.hw=io,irq,ram,ports:...

or::
(或：)

	c101.hw=irq,ram:...



If you have a problem with N2, C101 or PLX200SYN card, you can issue the
"private" command to see port's packet descriptor rings (in kernel logs)::
(如果 N2、C101 或 PLX200SYN 卡出现问题，可以使用 "private" 命令查看端口的包描述符环（在内核日志中）：)

	sethdlc hdlc0 private

The hardware driver has to be build with #define DEBUG_RINGS.
Attaching this info to bug reports would be helpful. Anyway, let me know
if you have problems using this.
(硬件驱动必须以 #define DEBUG_RINGS 构建。把此信息附在错误报告上会很有帮助。无论如何，如果使用中遇到问题请告诉我。)

For patches and other info look at:
<http://www.kernel.org/pub/linux/utils/net/hdlc/>.
(补丁和其他信息请查看：
<http://www.kernel.org/pub/linux/utils/net/hdlc/>。)
