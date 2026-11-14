.. SPDX-License-Identifier: GPL-2.0

==========================
KSMBD - SMB3 Kernel Server
==========================

(KSMBD - SMB3 内核服务器)

KSMBD is a linux kernel server which implements SMB3 protocol in kernel space
for sharing files over network.
(KSMBD 是一个在内核空间实现 SMB3 协议、用于通过网络共享文件的 Linux 内核服务器。)

KSMBD architecture
(KSMBD 架构)
==================

The subset of performance related operations belong in kernelspace and
the other subset which belong to operations which are not really related with
performance in userspace. So, DCE/RPC management that has historically resulted
into a number of buffer overflow issues and dangerous security bugs and user
account management are implemented in user space as ksmbd.mountd.
File operations that are related with performance (open/read/write/close etc.)
in kernel space (ksmbd). This also allows for easier integration with VFS
interface for all file operations.
(与性能相关的操作子集放在内核空间中，而另一部分与性能没有太大关系的操作子集则放在用户空间中。因此，历史上曾导致大量缓冲区溢出问题和危险安全漏洞的 DCE/RPC 管理，以及用户账户管理，都在用户空间中实现为 ksmbd.mountd。与性能相关的文件操作(打开/读/写/关闭等)则在内核空间(ksmbd)中处理。这也使得所有文件操作都更容易与 VFS 接口集成。)

ksmbd (kernel daemon)
(ksmbd(内核守护进程))
---------------------

When the server daemon is started, It starts up a forker thread
(ksmbd/interface name) at initialization time and open a dedicated port 445
for listening to SMB requests. Whenever new clients make a request, the Forker
thread will accept the client connection and fork a new thread for a dedicated
communication channel between the client and the server. It allows for parallel
processing of SMB requests(commands) from clients as well as allowing for new
clients to make new connections. Each instance is named ksmbd/1~n(port number)
to indicate connected clients. Depending on the SMB request types, each new
thread can decide to pass through the commands to the user space (ksmbd.mountd),
currently DCE/RPC commands are identified to be handled through the user space.
To further utilize the linux kernel, it has been chosen to process the commands
as workitems and to be executed in the handlers of the ksmbd-io kworker threads.
It allows for multiplexing of the handlers as the kernel takes care of initiating
extra worker threads if the load is increased and vice versa, if the load is
decreased it destroys the extra worker threads. So, after the connection is
established with the client. Dedicated ksmbd/1..n(port number) takes complete
ownership of receiving/parsing of SMB commands. Each received command is worked
in parallel i.e., there can be multiple client commands which are worked in
parallel. After receiving each command a separated kernel workitem is prepared
for each command which is further queued to be handled by ksmbd-io kworkers.
So, each SMB workitem is queued to the kworkers. This allows the benefit of load
sharing to be managed optimally by the default kernel and optimizing client
performance by handling client commands in parallel.
(当服务器守护进程启动时，它会在初始化时启动一个 forker 线程(ksmbd/接口名)，并打开专用端口 445 来监听 SMB 请求。每当新客户端发出请求时，Forker 线程都会接受该客户端连接，并为客户端与服务器之间的专用通信通道 fork 一个新线程。这允许并行处理来自客户端的 SMB 请求(命令)，同时也允许新客户端建立新的连接。每个实例都被命名为 ksmbd/1~n(端口号)，以指示已连接的客户端。根据 SMB 请求类型，每个新线程可以决定是否将命令传递给用户空间(ksmbd.mountd)处理，目前 DCE/RPC 命令被确定通过用户空间处理。为了进一步利用 Linux 内核，我们选择将这些命令作为工作项(workitem)来处理，并在 ksmbd-io kworker 线程的处理程序中执行。这允许对处理程序进行多路复用：当负载增加时，内核会负责启动额外的工作线程；反之，当负载减少时，它会销毁多余的工作线程。因此，在与客户端建立连接之后，专用的 ksmbd/1..n(端口号)会完全接管 SMB 命令的接收/解析。每个收到的命令都并行处理，也就是说，可以有多个客户端命令并行地得到处理。收到每个命令后，会为每个命令准备一个单独的内核工作项，随后将其排队，由 ksmbd-io kworker 处理。这样，每个 SMB 工作项都会排入 kworker 队列。这使得默认内核能够以最优方式管理负载分担带来的好处，并通过并行处理客户端命令来优化客户端性能。)

ksmbd.mountd (user space daemon)
(ksmbd.mountd(用户空间守护进程))
--------------------------------

ksmbd.mountd is a userspace process to, transfer the user account and password that
are registered using ksmbd.adduser (part of utils for user space). Further it
allows sharing information parameters that are parsed from smb.conf to ksmbd in
kernel. For the execution part it has a daemon which is continuously running
and connected to the kernel interface using netlink socket, it waits for the
requests (dcerpc and share/user info). It handles RPC calls (at a minimum few
dozen) that are most important for file server from NetShareEnum and
NetServerGetInfo. Complete DCE/RPC response is prepared from the user space
and passed over to the associated kernel thread for the client.
(ksmbd.mountd 是一个用户空间进程，用于传输使用 ksmbd.adduser(用户空间工具的一部分)注册的用户账户和密码。此外，它允许将从 smb.conf 解析出的共享信息参数传递给内核中的 ksmbd。在执行部分，它有一个持续运行的守护进程，通过 netlink 套接字连接到内核接口，等待请求(dcerpc 以及共享/用户信息)。它处理对文件服务器最重要的 RPC 调用(至少几十个)，来自 NetShareEnum 和 NetServerGetInfo。完整的 DCE/RPC 响应由用户空间准备，然后传递给与该客户端关联的内核线程。)


KSMBD Feature Status
(KSMBD 特性状态)
====================

============================== =================================================
Feature name                   Status (特性名称                状态)
============================== =================================================
Dialects                       Supported. SMB2.1 SMB3.0, SMB3.1.1 dialects
                               (intentionally excludes security vulnerable SMB1
                               dialect). (支持。SMB2.1、SMB3.0、SMB3.1.1 方言(有意排除了存在安全漏洞的 SMB1 方言)。)
Auto Negotiation               Supported. (支持。)
Compound Request               Supported. (支持。)
Oplock Cache Mechanism         Supported. (支持。)
SMB2 leases(v1 lease)          Supported. (支持。)
Directory leases(v2 lease)     Supported. (支持。)
Multi-credits                  Supported. (支持。)
NTLM/NTLMv2                    Supported. (支持。)
HMAC-SHA256 Signing            Supported. (支持。)
Secure negotiate               Supported. (支持。)
Signing Update                 Supported. (支持。)
Pre-authentication integrity   Supported. (支持。)
SMB3 encryption(CCM, GCM)      Supported. (CCM/GCM128 and CCM/GCM256 supported) (支持。(支持 CCM/GCM128 和 CCM/GCM256))
SMB direct(RDMA)               Supported. (支持。)
SMB3 Multi-channel             Partially Supported. Planned to implement
                               replay/retry mechanisms for future. (部分支持。计划在未来实现重放/重试机制。)
Receive Side Scaling mode      Supported. (支持。)
SMB3.1.1 POSIX extension       Supported. (支持。)
ACLs                           Partially Supported. only DACLs available, SACLs
                               (auditing) is planned for the future. For
                               ownership (SIDs) ksmbd generates random subauth
                               values(then store it to disk) and use uid/gid
                               get from inode as RID for local domain SID.
                               The current acl implementation is limited to
                               standalone server, not a domain member.
                               Integration with Samba tools is being worked on
                               to allow future support for running as a domain
                               member. (部分支持。仅提供 DACL，SACL(审计)计划在未来实现。对于所有权(SID)，ksmbd 会生成随机的 subauth 值(然后将其存储到磁盘)，并使用从 inode 获取的 uid/gid 作为本地域 SID 的 RID。当前的 acl 实现仅限于独立服务器，而不是域成员。与 Samba 工具的集成工作正在进行中，以便未来支持作为域成员运行。)
Kerberos                       Supported. (支持。)
Durable handle v1,v2           Planned for future. (计划在未来实现。)
Persistent handle              Planned for future. (计划在未来实现。)
SMB2 notify                    Planned for future. (计划在未来实现。)
Sparse file support            Supported. (支持。)
DCE/RPC support                Partially Supported. a few calls(NetShareEnumAll,
                               NetServerGetInfo, SAMR, LSARPC) that are needed
                               for file server handled via netlink interface
                               from ksmbd.mountd. Additional integration with
                               Samba tools and libraries via upcall is being
                               investigated to allow support for additional
                               DCE/RPC management calls (and future support
                               for Witness protocol e.g.) (部分支持。文件服务器所需的少量调用(NetShareEnumAll、NetServerGetInfo、SAMR、LSARPC)通过 netlink 接口由 ksmbd.mountd 处理。正在研究通过 upcall 与 Samba 工具和库做进一步集成，以支持更多的 DCE/RPC 管理调用(以及未来对 Witness 协议等的支持)。)
ksmbd/nfsd interoperability    Planned for future. The features that ksmbd
                               support are Leases, Notify, ACLs and Share modes. (计划在未来实现。ksmbd 支持的特性是 Leases、Notify、ACLs 和 Share modes。)
SMB3.1.1 Compression           Planned for future. (计划在未来实现。)
SMB3.1.1 over QUIC             Planned for future. (计划在未来实现。)
Signing/Encryption over RDMA   Planned for future. (计划在未来实现。)
SMB3.1.1 GMAC signing support  Planned for future. (计划在未来实现。)
============================== =================================================


How to run
(如何运行)
==========

1. Download ksmbd-tools(https://github.com/cifsd-team/ksmbd-tools/releases) and
   compile them.
   (下载 ksmbd-tools(https://github.com/cifsd-team/ksmbd-tools/releases)并编译它们。)

   - Refer to README(https://github.com/cifsd-team/ksmbd-tools/blob/master/README.md)
     to know how to use ksmbd.mountd/adduser/addshare/control utils
     (参考 README(https://github.com/cifsd-team/ksmbd-tools/blob/master/README.md)以了解如何使用 ksmbd.mountd/adduser/addshare/control 工具)

     $ ./autogen.sh
     $ ./configure --with-rundir=/run
     $ make && sudo make install

2. Create /usr/local/etc/ksmbd/ksmbd.conf file, add SMB share in ksmbd.conf file.
   (创建 /usr/local/etc/ksmbd/ksmbd.conf 文件，并在 ksmbd.conf 文件中添加 SMB 共享。)

   - Refer to ksmbd.conf.example in ksmbd-utils, See ksmbd.conf manpage
     for details to configure shares.
     (参考 ksmbd-utils 中的 ksmbd.conf.example；配置共享的详细信息请参见 ksmbd.conf 手册页。)

        $ man ksmbd.conf

3. Create user/password for SMB share.
   (为 SMB 共享创建用户/密码。)

   - See ksmbd.adduser manpage.
     (参见 ksmbd.adduser 手册页。)

     $ man ksmbd.adduser
     $ sudo ksmbd.adduser -a <Enter USERNAME for SMB share access>

4. Insert the ksmbd.ko module after you build your kernel. No need to load the module
   if ksmbd is built into the kernel.
   (在构建好内核之后插入 ksmbd.ko 模块。如果 ksmbd 已内建于内核中，则无需加载该模块。)

   - Set ksmbd in menuconfig(e.g. $ make menuconfig)
     (在 menuconfig 中设置 ksmbd(例如 $ make menuconfig))
       [*] Network File Systems  --->
           <M> SMB3 server support (EXPERIMENTAL)

	$ sudo modprobe ksmbd.ko

5. Start ksmbd user space daemon
   (启动 ksmbd 用户空间守护进程)

	$ sudo ksmbd.mountd

6. Access share from Windows or Linux using SMB3 client (cifs.ko or smbclient of samba)
   (使用 SMB3 客户端(cifs.ko 或 samba 的 smbclient)从 Windows 或 Linux 访问共享)

Shutdown KSMBD
(关闭 KSMBD)
==============

1. kill user and kernel space daemon
   (终止用户空间和内核空间的守护进程)
	# sudo ksmbd.control -s

How to turn debug print on
(如何打开调试打印)
==========================

Each layer
/sys/class/ksmbd-control/debug
(每一层通过 /sys/class/ksmbd-control/debug 控制)

1. Enable all component prints
   (启用所有组件的打印)
	# sudo ksmbd.control -d "all"

2. Enable one of the components (smb, auth, vfs, oplock, ipc, conn, rdma)
   (启用其中一个组件(smb、auth、vfs、oplock、ipc、conn、rdma))
	# sudo ksmbd.control -d "smb"

3. Show what prints are enabled.
   (显示当前启用了哪些打印。)
	# cat /sys/class/ksmbd-control/debug
	  [smb] auth vfs oplock ipc conn [rdma]

4. Disable prints:
   (禁用打印：)
	If you try the selected component once more, It is disabled without brackets.
	(如果再次对所选组件执行一次，它就会被禁用，显示时不再带方括号。)
