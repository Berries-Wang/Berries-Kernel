.. SPDX-License-Identifier: GPL-2.0

.. _networking-filter:

=======================================================================================================
Linux Socket Filtering aka Berkeley Packet Filter (BPF)(Linux 套接字过滤，又称伯克利数据包过滤器 (BPF))
=======================================================================================================

Notice(注意事项)
----------------

This file used to document the eBPF format and mechanisms even when not
related to socket filtering.  The ../bpf/index.rst has more details
on eBPF.
(本文件过去曾在与套接字过滤无关的情况下记录 eBPF 格式及其机制。
../bpf/index.rst 中有关于 eBPF 的更多细节。)

Introduction(简介)
------------------

Linux Socket Filtering (LSF) is derived from the Berkeley Packet Filter.
Though there are some distinct differences between the BSD and Linux
Kernel filtering, but when we speak of BPF or LSF in Linux context, we
mean the very same mechanism of filtering in the Linux kernel.
(Linux 套接字过滤 (LSF) 源自伯克利数据包过滤器。尽管 BSD 与 Linux 内核
过滤之间存在一些明显差异，但在 Linux 语境下谈论 BPF 或 LSF 时，我们指的
正是 Linux 内核中同一种过滤机制。)

BPF allows a user-space program to attach a filter onto any socket and
allow or disallow certain types of data to come through the socket. LSF
follows exactly the same filter code structure as BSD's BPF, so referring
to the BSD bpf.4 manpage is very helpful in creating filters.
(BPF 允许用户态程序将过滤器挂接到任意套接字上，从而允许或阻止某些类型的
数据通过该套接字。LSF 使用的过滤代码结构与 BSD 的 BPF 完全相同，因此在
编写过滤器时参考 BSD 的 bpf.4 手册页非常有帮助。)

On Linux, BPF is much simpler than on BSD. One does not have to worry
about devices or anything like that. You simply create your filter code,
send it to the kernel via the SO_ATTACH_FILTER option and if your filter
code passes the kernel check on it, you then immediately begin filtering
data on that socket.
(在 Linux 上，BPF 比 BSD 上简单得多。无需关心设备或类似的东西。你只需
创建自己的过滤代码，通过 SO_ATTACH_FILTER 选项发送给内核，如果你的过滤
代码通过了内核对其的检查，便可立即开始在该套接字上过滤数据。)

You can also detach filters from your socket via the SO_DETACH_FILTER
option. This will probably not be used much since when you close a socket
that has a filter on it the filter is automagically removed. The other
less common case may be adding a different filter on the same socket where
you had another filter that is still running: the kernel takes care of
removing the old one and placing your new one in its place, assuming your
filter has passed the checks, otherwise if it fails the old filter will
remain on that socket.
(你也可以通过 SO_DETACH_FILTER 选项从套接字上卸下过滤器。这一操作可能
不会经常用到，因为当你关闭一个挂有过滤器的套接字时，过滤器会被自动移除。
另一种较少见的情况是：在同一个套接字上原本已有一个仍在运行的过滤器，此时
想换上另一个不同的过滤器——内核会负责移除旧的过滤器并放置你的新过滤器，
前提是它通过了检查；否则若检查失败，旧过滤器将继续保留在该套接字上。)

SO_LOCK_FILTER option allows to lock the filter attached to a socket. Once
set, a filter cannot be removed or changed. This allows one process to
setup a socket, attach a filter, lock it then drop privileges and be
assured that the filter will be kept until the socket is closed.
(SO_LOCK_FILTER 选项允许锁定挂接到套接字上的过滤器。一旦设置，过滤器便
不能被移除或更改。这使得一个进程可以先建立套接字、挂接过滤器并锁定它，
然后丢弃特权，并确信该过滤器会一直保留到套接字关闭为止。)

The biggest user of this construct might be libpcap. Issuing a high-level
filter command like `tcpdump -i em1 port 22` passes through the libpcap
internal compiler that generates a structure that can eventually be loaded
via SO_ATTACH_FILTER to the kernel. `tcpdump -i em1 port 22 -ddd`
displays what is being placed into this structure.
(这一机制最大的使用者可能是 libpcap。执行 `tcpdump -i em1 port 22` 这样
的高层过滤命令时，会经过 libpcap 内部编译器，生成一个最终可通过
SO_ATTACH_FILTER 加载到内核的结构。`tcpdump -i em1 port 22 -ddd` 会显示
放入该结构中的内容。)

Although we were only speaking about sockets here, BPF in Linux is used
in many more places. There's xt_bpf for netfilter, cls_bpf in the kernel
qdisc layer, SECCOMP-BPF (SECure COMPuting [1]_), and lots of other places
such as team driver, PTP code, etc where BPF is being used.
(尽管我们这里只谈论了套接字，但 Linux 中的 BPF 还被用于更多地方。例如
netfilter 的 xt_bpf、内核 qdisc 层的 cls_bpf、SECCOMP-BPF (SECure COMPuting
[1]_)，以及 team 驱动、PTP 代码等许多其他使用 BPF 的场合。)

.. [1] Documentation/userspace-api/seccomp_filter.rst

Original BPF paper:
(原始 BPF 论文：)

Steven McCanne and Van Jacobson. 1993. The BSD packet filter: a new
architecture for user-level packet capture. In Proceedings of the
USENIX Winter 1993 Conference Proceedings on USENIX Winter 1993
Conference Proceedings (USENIX'93). USENIX Association, Berkeley,
CA, USA, 2-2. [http://www.tcpdump.org/papers/bpf-usenix93.pdf]
(Steven McCanne 与 Van Jacobson，1993 年。《BSD 数据包过滤器：一种面向
用户级数据包捕获的新架构》。收录于 USENIX Winter 1993 会议论文集
(USENIX'93)。USENIX Association，Berkeley, CA, USA，2-2。
[http://www.tcpdump.org/papers/bpf-usenix93.pdf])

Structure(结构)
---------------

User space applications include <linux/filter.h> which contains the
following relevant structures::
(用户态应用程序包含 <linux/filter.h>，其中有以下相关结构：)

	struct sock_filter {	/* Filter block */
		__u16	code;   /* Actual filter code */
		__u8	jt;	/* Jump true */
		__u8	jf;	/* Jump false */
		__u32	k;      /* Generic multiuse field */
	};

Such a structure is assembled as an array of 4-tuples, that contains
a code, jt, jf and k value. jt and jf are jump offsets and k a generic
value to be used for a provided code::
(这样的结构被组装成一个由四元组构成的数组，每个四元组包含 code、jt、jf
和 k 值。jt 和 jf 是跳转偏移量，k 是供给定指令使用的通用值。)

	struct sock_fprog {			/* Required for SO_ATTACH_FILTER. */
		unsigned short		   len;	/* Number of filter blocks */
		struct sock_filter __user *filter;
	};

For socket filtering, a pointer to this structure (as shown in
follow-up example) is being passed to the kernel through setsockopt(2).
(对于套接字过滤，指向该结构的指针(如下面示例所示)会通过 setsockopt(2)
传递给内核。)

Example(示例)
-------------

::

    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <linux/if_ether.h>
    /* ... */

    /* From the example above: tcpdump -i em1 port 22 -dd */
    struct sock_filter code[] = {
	    { 0x28,  0,  0, 0x0000000c },
	    { 0x15,  0,  8, 0x000086dd },
	    { 0x30,  0,  0, 0x00000014 },
	    { 0x15,  2,  0, 0x00000084 },
	    { 0x15,  1,  0, 0x00000006 },
	    { 0x15,  0, 17, 0x00000011 },
	    { 0x28,  0,  0, 0x00000036 },
	    { 0x15, 14,  0, 0x00000016 },
	    { 0x28,  0,  0, 0x00000038 },
	    { 0x15, 12, 13, 0x00000016 },
	    { 0x15,  0, 12, 0x00000800 },
	    { 0x30,  0,  0, 0x00000017 },
	    { 0x15,  2,  0, 0x00000084 },
	    { 0x15,  1,  0, 0x00000006 },
	    { 0x15,  0,  8, 0x00000011 },
	    { 0x28,  0,  0, 0x00000014 },
	    { 0x45,  6,  0, 0x00001fff },
	    { 0xb1,  0,  0, 0x0000000e },
	    { 0x48,  0,  0, 0x0000000e },
	    { 0x15,  2,  0, 0x00000016 },
	    { 0x48,  0,  0, 0x00000010 },
	    { 0x15,  0,  1, 0x00000016 },
	    { 0x06,  0,  0, 0x0000ffff },
	    { 0x06,  0,  0, 0x00000000 },
    };

    struct sock_fprog bpf = {
	    .len = ARRAY_SIZE(code),
	    .filter = code,
    };

    sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0)
	    /* ... bail out ... */

    ret = setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
    if (ret < 0)
	    /* ... bail out ... */

    /* ... */
    close(sock);

The above example code attaches a socket filter for a PF_PACKET socket
in order to let all IPv4/IPv6 packets with port 22 pass. The rest will
be dropped for this socket.
(上面的示例代码为 PF_PACKET 套接字挂接了一个套接字过滤器，让所有端口为
22 的 IPv4/IPv6 数据包通过，其余数据包在该套接字上将被丢弃。)

The setsockopt(2) call to SO_DETACH_FILTER doesn't need any arguments
and SO_LOCK_FILTER for preventing the filter to be detached, takes an
integer value with 0 or 1.
(对 SO_DETACH_FILTER 的 setsockopt(2) 调用不需要任何参数；而用于防止
过滤器被卸下的 SO_LOCK_FILTER 则接受一个取值为 0 或 1 的整数。)

Note that socket filters are not restricted to PF_PACKET sockets only,
but can also be used on other socket families.
(注意，套接字过滤器并不局限于 PF_PACKET 套接字，也可以用于其他套接字族。)

Summary of system calls:
(系统调用汇总：)

 * setsockopt(sockfd, SOL_SOCKET, SO_ATTACH_FILTER, &val, sizeof(val));
 * setsockopt(sockfd, SOL_SOCKET, SO_DETACH_FILTER, &val, sizeof(val));
 * setsockopt(sockfd, SOL_SOCKET, SO_LOCK_FILTER,   &val, sizeof(val));

Normally, most use cases for socket filtering on packet sockets will be
covered by libpcap in high-level syntax, so as an application developer
you should stick to that. libpcap wraps its own layer around all that.
(通常，包套接字上套接字过滤的大多数使用场景都可以用 libpcap 的高层语法
来覆盖，因此作为应用开发者，你应当坚持使用它。libpcap 在这一切之上封装
了自己的层。)

Unless i) using/linking to libpcap is not an option, ii) the required BPF
filters use Linux extensions that are not supported by libpcap's compiler,
iii) a filter might be more complex and not cleanly implementable with
libpcap's compiler, or iv) particular filter codes should be optimized
differently than libpcap's internal compiler does; then in such cases
writing such a filter "by hand" can be of an alternative. For example,
xt_bpf and cls_bpf users might have requirements that could result in
more complex filter code, or one that cannot be expressed with libpcap
(e.g. different return codes for various code paths). Moreover, BPF JIT
implementors may wish to manually write test cases and thus need low-level
access to BPF code as well.
(除非出现以下情况：i) 无法使用或链接 libpcap；ii) 所需的 BPF 过滤器使用
了 libpcap 编译器不支持的 Linux 扩展；iii) 过滤器可能较为复杂，无法用
libpcap 编译器干净地实现；或 iv) 特定的过滤代码需要采用与 libpcap 内部
编译器不同的优化方式——否则在上述情况下，"手工"编写这样的过滤器可以
作为一种替代方案。例如，xt_bpf 和 cls_bpf 的用户可能有某些需求，导致
过滤代码更复杂，或是无法用 libpcap 表达(例如对不同的代码路径返回不同的
返回码)。此外，BPF JIT 的实现者可能希望手工编写测试用例，因而也需要对
BPF 代码的低层访问能力。)

BPF engine and instruction set(BPF 引擎与指令集)
------------------------------------------------

Under tools/bpf/ there's a small helper tool called bpf_asm which can
be used to write low-level filters for example scenarios mentioned in the
previous section. Asm-like syntax mentioned here has been implemented in
bpf_asm and will be used for further explanations (instead of dealing with
less readable opcodes directly, principles are the same). The syntax is
closely modelled after Steven McCanne's and Van Jacobson's BPF paper.
(在 tools/bpf/ 目录下有一个名为 bpf_asm 的小型辅助工具，可用于编写上一
节所举示例场景的低层过滤器。这里提到的类汇编语法已在 bpf_asm 中实现，
并将用于后续讲解(不直接处理可读性较差的操作码，但原理相同)。该语法紧密
参照 Steven McCanne 与 Van Jacobson 的 BPF 论文设计。)

The BPF architecture consists of the following basic elements:
(BPF 架构由以下基本元素组成：)

  =======          ====================================================
  Element          Description (描述)
  =======          ====================================================
  A                32 bit wide accumulator (32 位宽的累加器)
  X                32 bit wide X register (32 位宽的 X 寄存器)
  M[]              16 x 32 bit wide misc registers aka "scratch memory
		   store", addressable from 0 to 15 (16 个 32 位宽的杂项
		   寄存器，又称 "暂存内存"，可寻址范围为 0 到 15)
  =======          ====================================================

A program, that is translated by bpf_asm into "opcodes" is an array that
consists of the following elements (as already mentioned)::
(被 bpf_asm 翻译成 "操作码" 的程序是一个由以下元素构成的数组
(如前文所述)：)

  op:16, jt:8, jf:8, k:32

The element op is a 16 bit wide opcode that has a particular instruction
encoded. jt and jf are two 8 bit wide jump targets, one for condition
"jump if true", the other one "jump if false". Eventually, element k
contains a miscellaneous argument that can be interpreted in different
ways depending on the given instruction in op.
(元素 op 是一个 16 位宽的操作码，其中编码了某条特定指令。jt 和 jf 是两个
8 位宽的跳转目标，一个对应条件 "为真则跳转"，另一个对应 "为假则跳转"。
最后，元素 k 包含一个通用参数，根据 op 中给定的指令不同，它可以以不同的
方式来解释。)

The instruction set consists of load, store, branch, alu, miscellaneous
and return instructions that are also represented in bpf_asm syntax. This
table lists all bpf_asm instructions available resp. what their underlying
opcodes as defined in linux/filter.h stand for:
(指令集由加载、存储、分支、ALU、杂项和返回指令组成，这些指令同样以
bpf_asm 语法表示。下表列出了所有可用的 bpf_asm 指令，以及它们在
linux/filter.h 中定义的底层操作码的含义：)

  ===========      ===================  =====================
  Instruction      Addressing mode      Description (描述)
  ===========      ===================  =====================
  ld               1, 2, 3, 4, 12       Load word into A (将字加载到 A)
  ldi              4                    Load word into A (将字加载到 A)
  ldh              1, 2                 Load half-word into A (将半字加载到 A)
  ldb              1, 2                 Load byte into A (将字节加载到 A)
  ldx              3, 4, 5, 12          Load word into X (将字加载到 X)
  ldxi             4                    Load word into X (将字加载到 X)
  ldxb             5                    Load byte into X (将字节加载到 X)

  st               3                    Store A into M[] (将 A 存入 M[])
  stx              3                    Store X into M[] (将 X 存入 M[])

  jmp              6                    Jump to label (跳转到标号)
  ja               6                    Jump to label (跳转到标号)
  jeq              7, 8, 9, 10          Jump on A == <x> (当 A == <x> 时跳转)
  jneq             9, 10                Jump on A != <x> (当 A != <x> 时跳转)
  jne              9, 10                Jump on A != <x> (当 A != <x> 时跳转)
  jlt              9, 10                Jump on A <  <x> (当 A <  <x> 时跳转)
  jle              9, 10                Jump on A <= <x> (当 A <= <x> 时跳转)
  jgt              7, 8, 9, 10          Jump on A >  <x> (当 A >  <x> 时跳转)
  jge              7, 8, 9, 10          Jump on A >= <x> (当 A >= <x> 时跳转)
  jset             7, 8, 9, 10          Jump on A &  <x> (当 A &  <x> 时跳转)

  add              0, 4                 A + <x>
  sub              0, 4                 A - <x>
  mul              0, 4                 A * <x>
  div              0, 4                 A / <x>
  mod              0, 4                 A % <x>
  neg                                   !A
  and              0, 4                 A & <x>
  or               0, 4                 A | <x>
  xor              0, 4                 A ^ <x>
  lsh              0, 4                 A << <x>
  rsh              0, 4                 A >> <x>

  tax                                   Copy A into X (将 A 复制到 X)
  txa                                   Copy X into A (将 X 复制到 A)

  ret              4, 11                Return (返回)
  ===========      ===================  =====================

The next table shows addressing formats from the 2nd column:
(下表展示第 2 列中各寻址模式的格式：)

  ===============  ===================  ===============================================
  Addressing mode  Syntax (语法)        Description (描述)
  ===============  ===================  ===============================================
   0               x/%x                 Register X (寄存器 X)
   1               [k]                  BHW at byte offset k in the packet (数据包中字节偏移 k 处的 BHW)
   2               [x + k]              BHW at the offset X + k in the packet (数据包中偏移 X + k 处的 BHW)
   3               M[k]                 Word at offset k in M[] (M[] 中偏移 k 处的字)
   4               #k                   Literal value stored in k (存储在 k 中的字面值)
   5               4*([k]&0xf)          Lower nibble * 4 at byte offset k in the packet (数据包中字节偏移 k 处的低半字节 * 4)
   6               L                    Jump label L (跳转标号 L)
   7               #k,Lt,Lf             Jump to Lt if true, otherwise jump to Lf (为真则跳转到 Lt，否则跳转到 Lf)
   8               x/%x,Lt,Lf           Jump to Lt if true, otherwise jump to Lf (为真则跳转到 Lt，否则跳转到 Lf)
   9               #k,Lt                Jump to Lt if predicate is true (谓词为真则跳转到 Lt)
  10               x/%x,Lt              Jump to Lt if predicate is true (谓词为真则跳转到 Lt)
  11               a/%a                 Accumulator A (累加器 A)
  12               extension            BPF extension (BPF 扩展)
  ===============  ===================  ===============================================

The Linux kernel also has a couple of BPF extensions that are used along
with the class of load instructions by "overloading" the k argument with
a negative offset + a particular extension offset. The result of such BPF
extensions are loaded into A.
(Linux 内核还有若干 BPF 扩展，它们与加载类指令配合使用，方式是用一个负
偏移量加上特定的扩展偏移量来 "重载" k 参数。这类 BPF 扩展的结果会被加载
到 A 中。)

Possible BPF extensions are shown in the following table:
(可用的 BPF 扩展如下表所示：)

  ===================================   =================================================
  Extension                             Description (描述)
  ===================================   =================================================
  len                                   skb->len
  proto                                 skb->protocol
  type                                  skb->pkt_type
  poff                                  Payload start offset (载荷起始偏移)
  ifidx                                 skb->dev->ifindex
  nla                                   Netlink attribute of type X with offset A (偏移 A 处类型为 X 的 netlink 属性)
  nlan                                  Nested Netlink attribute of type X with offset A (偏移 A 处类型为 X 的嵌套 netlink 属性)
  mark                                  skb->mark
  queue                                 skb->queue_mapping
  hatype                                skb->dev->type
  rxhash                                skb->hash
  cpu                                   raw_smp_processor_id()
  vlan_tci                              skb_vlan_tag_get(skb)
  vlan_avail                            skb_vlan_tag_present(skb)
  vlan_tpid                             skb->vlan_proto
  rand                                  get_random_u32()
  ===================================   =================================================

These extensions can also be prefixed with '#'.
(这些扩展也可以加上 '#' 前缀。)
Examples for low-level BPF:
(低层 BPF 示例：)

**ARP packets**::
(ARP 数据包：)

  ldh [12]
  jne #0x806, drop
  ret #-1
  drop: ret #0

**IPv4 TCP packets**::
(IPv4 TCP 数据包：)

  ldh [12]
  jne #0x800, drop
  ldb [23]
  jneq #6, drop
  ret #-1
  drop: ret #0

**icmp random packet sampling, 1 in 4**::
(icmp 随机抽样，每 4 个抽 1 个：)

  ldh [12]
  jne #0x800, drop
  ldb [23]
  jneq #1, drop
  # get a random uint32 number
  ld rand
  mod #4
  jneq #1, drop
  ret #-1
  drop: ret #0

**SECCOMP filter example**::
(SECCOMP 过滤器示例：)

  ld [4]                  /* offsetof(struct seccomp_data, arch) */
  jne #0xc000003e, bad    /* AUDIT_ARCH_X86_64 */
  ld [0]                  /* offsetof(struct seccomp_data, nr) */
  jeq #15, good           /* __NR_rt_sigreturn */
  jeq #231, good          /* __NR_exit_group */
  jeq #60, good           /* __NR_exit */
  jeq #0, good            /* __NR_read */
  jeq #1, good            /* __NR_write */
  jeq #5, good            /* __NR_fstat */
  jeq #9, good            /* __NR_mmap */
  jeq #14, good           /* __NR_rt_sigprocmask */
  jeq #13, good           /* __NR_rt_sigaction */
  jeq #35, good           /* __NR_nanosleep */
  bad: ret #0             /* SECCOMP_RET_KILL_THREAD */
  good: ret #0x7fff0000   /* SECCOMP_RET_ALLOW */

Examples for low-level BPF extension:
(低层 BPF 扩展示例：)

**Packet for interface index 13**::
(接口索引为 13 的数据包：)

  ld ifidx
  jneq #13, drop
  ret #-1
  drop: ret #0

**(Accelerated) VLAN w/ id 10**::
((加速) VLAN，id 为 10：)

  ld vlan_tci
  jneq #10, drop
  ret #-1
  drop: ret #0

The above example code can be placed into a file (here called "foo"), and
then be passed to the bpf_asm tool for generating opcodes, output that xt_bpf
and cls_bpf understands and can directly be loaded with. Example with above
ARP code::
(上面的示例代码可以放入一个文件(这里称为 "foo")，然后传给 bpf_asm 工具
以生成操作码，xt_bpf 和 cls_bpf 能够理解并直接加载其输出。以上面的 ARP
代码为例：)

    $ ./bpf_asm foo
    4,40 0 0 12,21 0 1 2054,6 0 0 4294967295,6 0 0 0,

In copy and paste C-like output::
(以可直接复制粘贴的类 C 输出表示：)

    $ ./bpf_asm -c foo
    { 0x28,  0,  0, 0x0000000c },
    { 0x15,  0,  1, 0x00000806 },
    { 0x06,  0,  0, 0xffffffff },
    { 0x06,  0,  0, 0000000000 },

In particular, as usage with xt_bpf or cls_bpf can result in more complex BPF
filters that might not be obvious at first, it's good to test filters before
attaching to a live system. For that purpose, there's a small tool called
bpf_dbg under tools/bpf/ in the kernel source directory. This debugger allows
for testing BPF filters against given pcap files, single stepping through the
BPF code on the pcap's packets and to do BPF machine register dumps.
(特别是，由于配合 xt_bpf 或 cls_bpf 使用可能产生初看之下并不直观的复杂
BPF 过滤器，因此在挂接到实际运行的系统之前最好先测试过滤器。为此，内核
源码目录的 tools/bpf/ 下提供了一个名为 bpf_dbg 的小工具。该调试器支持
针对给定的 pcap 文件测试 BPF 过滤器、对 pcap 数据包上的 BPF 代码单步
调试，以及转储 BPF 机器的寄存器。)

Starting bpf_dbg is trivial and just requires issuing::
(启动 bpf_dbg 很简单，只需执行：)

    # ./bpf_dbg

In case input and output do not equal stdin/stdout, bpf_dbg takes an
alternative stdin source as a first argument, and an alternative stdout
sink as a second one, e.g. `./bpf_dbg test_in.txt test_out.txt`.
(如果输入和输出不是 stdin/stdout，bpf_dbg 会把第一个参数作为替代的 stdin
来源，把第二个参数作为替代的 stdout 去向，例如
`./bpf_dbg test_in.txt test_out.txt`。)

Other than that, a particular libreadline configuration can be set via
file "~/.bpf_dbg_init" and the command history is stored in the file
"~/.bpf_dbg_history".
(除此之外，可以通过文件 "~/.bpf_dbg_init" 设置特定的 libreadline 配置，
命令历史则保存在文件 "~/.bpf_dbg_history" 中。)

Interaction in bpf_dbg happens through a shell that also has auto-completion
support (follow-up example commands starting with '>' denote bpf_dbg shell).
The usual workflow would be to ...
(bpf_dbg 中的交互通过一个 shell 进行，该 shell 还支持自动补全(下文示例
中以 '>' 开头的命令表示 bpf_dbg shell)。通常的工作流程是……)

* load bpf 6,40 0 0 12,21 0 3 2048,48 0 0 23,21 0 1 1,6 0 0 65535,6 0 0 0
  Loads a BPF filter from standard output of bpf_asm, or transformed via
  e.g. ``tcpdump -iem1 -ddd port 22 | tr '\n' ','``. Note that for JIT
  debugging (next section), this command creates a temporary socket and
  loads the BPF code into the kernel. Thus, this will also be useful for
  JIT developers.
  (从 bpf_asm 的标准输出加载一个 BPF 过滤器，也可以通过例如
  ``tcpdump -iem1 -ddd port 22 | tr '\n' ','`` 这样的命令转换而来。注意，
  对于 JIT 调试(见下一节)而言，该命令会创建一个临时套接字并把 BPF 代码
  加载进内核，因此这对 JIT 开发者同样有用。)

* load pcap foo.pcap

  Loads standard tcpdump pcap file.
  (加载标准的 tcpdump pcap 文件。)

* run [<n>]

bpf passes:1 fails:9
  Runs through all packets from a pcap to account how many passes and fails
  the filter will generate. A limit of packets to traverse can be given.
  (遍历 pcap 中的所有数据包，统计过滤器将产生多少次通过与失败。可以指定
  要遍历的数据包数量上限。)

* disassemble::
  (反汇编：)

	l0:	ldh [12]
	l1:	jeq #0x800, l2, l5
	l2:	ldb [23]
	l3:	jeq #0x1, l4, l5
	l4:	ret #0xffff
	l5:	ret #0

  Prints out BPF code disassembly.
  (打印出 BPF 代码的反汇编。)

* dump::
  (转储：)

	/* { op, jt, jf, k }, */
	{ 0x28,  0,  0, 0x0000000c },
	{ 0x15,  0,  3, 0x00000800 },
	{ 0x30,  0,  0, 0x00000017 },
	{ 0x15,  0,  1, 0x00000001 },
	{ 0x06,  0,  0, 0x0000ffff },
	{ 0x06,  0,  0, 0000000000 },

  Prints out C-style BPF code dump.
  (打印出 C 风格的 BPF 代码转储。)

* breakpoint 0::
  (断点 0：)

	breakpoint at: l0:	ldh [12]

* breakpoint 1::
  (断点 1：)

	breakpoint at: l1:	jeq #0x800, l2, l5

  ...

  Sets breakpoints at particular BPF instructions. Issuing a `run` command
  will walk through the pcap file continuing from the current packet and
  break when a breakpoint is being hit (another `run` will continue from
  the currently active breakpoint executing next instructions):
  (在特定的 BPF 指令处设置断点。执行 `run` 命令将从当前数据包起继续遍历
  pcap 文件，并在命中断点时中断(再一次执行 `run` 将从当前活动的断点继续，
  执行后续指令)：)

  * run::
    (运行：)

	-- register dump --
	pc:       [0]                       <-- program counter
	code:     [40] jt[0] jf[0] k[12]    <-- plain BPF code of current instruction
	curr:     l0:	ldh [12]              <-- disassembly of current instruction
	A:        [00000000][0]             <-- content of A (hex, decimal)
	X:        [00000000][0]             <-- content of X (hex, decimal)
	M[0,15]:  [00000000][0]             <-- folded content of M (hex, decimal)
	-- packet dump --                   <-- Current packet from pcap (hex)
	len: 42
	    0: 00 19 cb 55 55 a4 00 14 a4 43 78 69 08 06 00 01
	16: 08 00 06 04 00 01 00 14 a4 43 78 69 0a 3b 01 26
	32: 00 00 00 00 00 00 0a 3b 01 01
	(breakpoint)
	>

  * breakpoint::
    (断点：)

	breakpoints: 0 1

    Prints currently set breakpoints.
    (打印当前已设置的断点。)

* step [-<n>, +<n>]

  Performs single stepping through the BPF program from the current pc
  offset. Thus, on each step invocation, above register dump is issued.
  This can go forwards and backwards in time, a plain `step` will break
  on the next BPF instruction, thus +1. (No `run` needs to be issued here.)
  (从当前 pc 偏移处对 BPF 程序进行单步执行。因此，每次调用 step 都会输出
  上述寄存器转储。这个过程既可以向前也可以向后进行，单纯的 `step` 会在
  下一 BPF 指令处中断，即 +1。(此处无需执行 `run`。))

* select <n>

  Selects a given packet from the pcap file to continue from. Thus, on
  the next `run` or `step`, the BPF program is being evaluated against
  the user pre-selected packet. Numbering starts just as in Wireshark
  with index 1.
  (从 pcap 文件中选择给定的数据包作为继续执行的起点。这样，在下一次
  `run` 或 `step` 时，BPF 程序将针对用户预先选定的数据包进行求值。编号
  与 Wireshark 一样从索引 1 开始。)

* quit

  Exits bpf_dbg.
  (退出 bpf_dbg。)

JIT compiler(JIT 编译器)
------------------------

The Linux kernel has a built-in BPF JIT compiler for x86_64, SPARC,
PowerPC, ARM, ARM64, MIPS, RISC-V, s390, and ARC and can be enabled through
CONFIG_BPF_JIT. The JIT compiler is transparently invoked for each
attached filter from user space or for internal kernel users if it has
been previously enabled by root::
(Linux 内核为 x86_64、SPARC、PowerPC、ARM、ARM64、MIPS、RISC-V、s390 和
ARC 内置了 BPF JIT 编译器，可通过 CONFIG_BPF_JIT 启用。如果 root 此前已
将其启用，那么对每个来自用户空间的挂接过滤器或内部内核使用者，JIT 编译器
都会被透明地调用：)

  echo 1 > /proc/sys/net/core/bpf_jit_enable

For JIT developers, doing audits etc, each compile run can output the generated
opcode image into the kernel log via::
(对于 JIT 开发者做审计等工作，每次编译运行都可以通过以下方式将生成的
操作码映像输出到内核日志中：)

  echo 2 > /proc/sys/net/core/bpf_jit_enable

Example output from dmesg::
(dmesg 输出示例：)

    [ 3389.935842] flen=6 proglen=70 pass=3 image=ffffffffa0069c8f
    [ 3389.935847] JIT code: 00000000: 55 48 89 e5 48 83 ec 60 48 89 5d f8 44 8b 4f 68
    [ 3389.935849] JIT code: 00000010: 44 2b 4f 6c 4c 8b 87 d8 00 00 00 be 0c 00 00 00
    [ 3389.935850] JIT code: 00000020: e8 1d 94 ff e0 3d 00 08 00 00 75 16 be 17 00 00
    [ 3389.935851] JIT code: 00000030: 00 e8 28 94 ff e0 83 f8 01 75 07 b8 ff ff 00 00
    [ 3389.935852] JIT code: 00000040: eb 02 31 c0 c9 c3

When CONFIG_BPF_JIT_ALWAYS_ON is enabled, bpf_jit_enable is permanently set to 1 and
setting any other value than that will return in failure. This is even the case for
setting bpf_jit_enable to 2, since dumping the final JIT image into the kernel log
is discouraged and introspection through bpftool (under tools/bpf/bpftool/) is the
generally recommended approach instead.
(启用 CONFIG_BPF_JIT_ALWAYS_ON 后，bpf_jit_enable 将被永久设置为 1，
设置任何其他值都会失败。即使把 bpf_jit_enable 设为 2 也是如此，因为不
鼓励将最终的 JIT 映像转储到内核日志中，一般推荐改用 bpftool(位于
tools/bpf/bpftool/ 下)进行内省。)

In the kernel source tree under tools/bpf/, there's bpf_jit_disasm for
generating disassembly out of the kernel log's hexdump::
(在内核源码树的 tools/bpf/ 目录下，有一个 bpf_jit_disasm 工具，可根据
内核日志中的十六进制转储生成反汇编：)

	# ./bpf_jit_disasm
	70 bytes emitted from JIT compiler (pass:3, flen:6)
	ffffffffa0069c8f + <x>:
	0:	push   %rbp
	1:	mov    %rsp,%rbp
	4:	sub    $0x60,%rsp
	8:	mov    %rbx,-0x8(%rbp)
	c:	mov    0x68(%rdi),%r9d
	10:	sub    0x6c(%rdi),%r9d
	14:	mov    0xd8(%rdi),%r8
	1b:	mov    $0xc,%esi
	20:	callq  0xffffffffe0ff9442
	25:	cmp    $0x800,%eax
	2a:	jne    0x0000000000000042
	2c:	mov    $0x17,%esi
	31:	callq  0xffffffffe0ff945e
	36:	cmp    $0x1,%eax
	39:	jne    0x0000000000000042
	3b:	mov    $0xffff,%eax
	40:	jmp    0x0000000000000044
	42:	xor    %eax,%eax
	44:	leaveq
	45:	retq

	Issuing option `-o` will "annotate" opcodes to resulting assembler
	instructions, which can be very useful for JIT developers:

	# ./bpf_jit_disasm -o
	70 bytes emitted from JIT compiler (pass:3, flen:6)
	ffffffffa0069c8f + <x>:
	0:	push   %rbp
		55
	1:	mov    %rsp,%rbp
		48 89 e5
	4:	sub    $0x60,%rsp
		48 83 ec 60
	8:	mov    %rbx,-0x8(%rbp)
		48 89 5d f8
	c:	mov    0x68(%rdi),%r9d
		44 8b 4f 68
	10:	sub    0x6c(%rdi),%r9d
		44 2b 4f 6c
	14:	mov    0xd8(%rdi),%r8
		4c 8b 87 d8 00 00 00
	1b:	mov    $0xc,%esi
		be 0c 00 00 00
	20:	callq  0xffffffffe0ff9442
		e8 1d 94 ff e0
	25:	cmp    $0x800,%eax
		3d 00 08 00 00
	2a:	jne    0x0000000000000042
		75 16
	2c:	mov    $0x17,%esi
		be 17 00 00 00
	31:	callq  0xffffffffe0ff945e
		e8 28 94 ff e0
	36:	cmp    $0x1,%eax
		83 f8 01
	39:	jne    0x0000000000000042
		75 07
	3b:	mov    $0xffff,%eax
		b8 ff ff 00 00
	40:	jmp    0x0000000000000044
		eb 02
	42:	xor    %eax,%eax
		31 c0
	44:	leaveq
		c9
	45:	retq
		c3

For BPF JIT developers, bpf_jit_disasm, bpf_asm and bpf_dbg provides a useful
toolchain for developing and testing the kernel's JIT compiler.
(对于 BPF JIT 开发者来说，bpf_jit_disasm、bpf_asm 和 bpf_dbg 组成了一套
实用的工具链，可用于开发和测试内核的 JIT 编译器。)

BPF kernel internals(BPF 内核内部机制)
--------------------------------------

Internally, for the kernel interpreter, a different instruction set
format with similar underlying principles from BPF described in previous
paragraphs is being used. However, the instruction set format is modelled
closer to the underlying architecture to mimic native instruction sets, so
that a better performance can be achieved (more details later). This new
ISA is called eBPF.  See the ../bpf/index.rst for details.  (Note: eBPF which
originates from [e]xtended BPF is not the same as BPF extensions! While
eBPF is an ISA, BPF extensions date back to classic BPF's 'overloading'
of BPF_LD | BPF_{B,H,W} | BPF_ABS instruction.)
(在内核内部，对于内核解释器而言，使用的是一种与前面几段所描述的 BPF 原理
相近、但格式不同的指令集。不过，这种指令集格式在建模上更贴近底层架构，
以模仿原生指令集，从而获得更好的性能(详见后文)。这种新 ISA 称为 eBPF，
详情参见 ../bpf/index.rst。(注意：源自 [e]xtended BPF 的 eBPF 与 BPF
扩展 (BPF extensions) 不是一回事！eBPF 是一种 ISA，而 BPF 扩展则要追溯
到经典 BPF 对 BPF_LD | BPF_{B,H,W} | BPF_ABS 指令的 "重载" 机制。))

The new instruction set was originally designed with the possible goal in
mind to write programs in "restricted C" and compile into eBPF with a optional
GCC/LLVM backend, so that it can just-in-time map to modern 64-bit CPUs with
minimal performance overhead over two steps, that is, C -> eBPF -> native code.
(设计这套新指令集的初衷之一，是让人们可以用 "受限 C" 编写程序，再通过
可选的 GCC/LLVM 后端编译成 eBPF，从而经过 C -> eBPF -> 原生代码两个步骤，
以最小的性能开销即时映射到现代 64 位 CPU 上。)

Currently, the new format is being used for running user BPF programs, which
includes seccomp BPF, classic socket filters, cls_bpf traffic classifier,
team driver's classifier for its load-balancing mode, netfilter's xt_bpf
extension, PTP dissector/classifier, and much more. They are all internally
converted by the kernel into the new instruction set representation and run
in the eBPF interpreter. For in-kernel handlers, this all works transparently
by using bpf_prog_create() for setting up the filter, resp.
bpf_prog_destroy() for destroying it. The function
bpf_prog_run(filter, ctx) transparently invokes eBPF interpreter or JITed
code to run the filter. 'filter' is a pointer to struct bpf_prog that we
got from bpf_prog_create(), and 'ctx' the given context (e.g.
skb pointer). All constraints and restrictions from bpf_check_classic() apply
before a conversion to the new layout is being done behind the scenes!
(目前，新格式被用于运行用户 BPF 程序，其中包括 seccomp BPF、经典套接字
过滤器、cls_bpf 流量分类器、team 驱动负载均衡模式的分类器、netfilter 的
xt_bpf 扩展、PTP 解析器/分类器等等。它们都会由内核在内部转换成新指令集
表示，并在 eBPF 解释器中运行。对于内核内部的处理程序来说，这一切都是
透明的：使用 bpf_prog_create() 建立过滤器，相应地用 bpf_prog_destroy()
销毁它。函数 bpf_prog_run(filter, ctx) 会透明地调用 eBPF 解释器或 JIT
编译后的代码来运行过滤器。'filter' 是指向 struct bpf_prog 的指针，来自
bpf_prog_create() 的返回值；'ctx' 是给定的上下文(例如 skb 指针)。在幕后
转换为新布局之前，bpf_check_classic() 的所有约束和限制都同样适用！)

Currently, the classic BPF format is being used for JITing on most
32-bit architectures, whereas x86-64, aarch64, s390x, powerpc64,
sparc64, arm32, riscv64, riscv32, loongarch64, arc perform JIT compilation
from eBPF instruction set.
(目前，大多数 32 位架构上的 JIT 仍使用经典 BPF 格式，而 x86-64、aarch64、
s390x、powerpc64、sparc64、arm32、riscv64、riscv32、loongarch64、arc 则
从 eBPF 指令集执行 JIT 编译。)

Testing(测试)
-------------

Next to the BPF toolchain, the kernel also ships a test module that contains
various test cases for classic and eBPF that can be executed against
the BPF interpreter and JIT compiler. It can be found in lib/test_bpf.c and
enabled via Kconfig::
(除 BPF 工具链之外，内核还附带一个测试模块，其中包含针对经典 BPF 和
eBPF 的各种测试用例，可以在 BPF 解释器和 JIT 编译器上执行。它位于
lib/test_bpf.c，可通过 Kconfig 启用：)

  CONFIG_TEST_BPF=m

After the module has been built and installed, the test suite can be executed
via insmod or modprobe against 'test_bpf' module. Results of the test cases
including timings in nsec can be found in the kernel log (dmesg).
(该模块构建并安装后，可以通过 insmod 或 modprobe 加载 'test_bpf' 模块来
运行测试套件。各测试用例的结果(包括以纳秒计的耗时)可在内核日志 (dmesg)
中找到。)

Misc(其他)
----------

Also trinity, the Linux syscall fuzzer, has built-in support for BPF and
SECCOMP-BPF kernel fuzzing.
(此外，Linux 系统调用模糊测试工具 trinity 也内置了对 BPF 和 SECCOMP-BPF
内核模糊测试的支持。)

Written by(作者)
----------------

The document was written in the hope that it is found useful and in order
to give potential BPF hackers or security auditors a better overview of
the underlying architecture.
(本文档的写作初衷是希望它能对读者有所帮助，并为潜在的 BPF 黑客或安全
审计人员提供对底层架构的更好概览。)

- Jay Schulist <jschlst@samba.org>
- Daniel Borkmann <daniel@iogearbox.net>
- Alexei Starovoitov <ast@kernel.org>
