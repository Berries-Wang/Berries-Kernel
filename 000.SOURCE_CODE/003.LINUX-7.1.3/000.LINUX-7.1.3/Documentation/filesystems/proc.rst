.. SPDX-License-Identifier: GPL-2.0

====================
The /proc Filesystem
====================

(/proc 文件系统)

=====================  =======================================  ================
/proc/sys              Terrehon Bowden <terrehon@pacbell.net>,  October 7 1999
                       Bodo Bauer <bb@ricochet.net>
2.4.x update	       Jorge Nerin <comandante@zaralinux.com>   November 14 2000
move /proc/sys	       Shen Feng <shen@cn.fujitsu.com>	        April 1 2009
fixes/update part 1.1  Stefani Seibold <stefani@seibold.net>    June 9 2009
=====================  =======================================  ================



.. Table of Contents

  0     Preface
        (0  前言)
  0.1	Introduction/Credits
        (0.1  引言/致谢)
  0.2	Legal Stuff
        (0.2  法律事项)

  1	Collecting System Information
        (1  收集系统信息)
  1.1	Process-Specific Subdirectories
        (1.1  进程相关的子目录)
  1.2	Kernel data
        (1.2  内核数据)
  1.3	IDE devices in /proc/ide
        (1.3  /proc/ide 中的 IDE 设备)
  1.4	Networking info in /proc/net
        (1.4  /proc/net 中的网络信息)
  1.5	SCSI info
        (1.5  SCSI 信息)
  1.6	Parallel port info in /proc/parport
        (1.6  /proc/parport 中的并行端口信息)
  1.7	TTY info in /proc/tty
        (1.7  /proc/tty 中的 TTY 信息)
  1.8	Miscellaneous kernel statistics in /proc/stat
        (1.8  /proc/stat 中的杂项内核统计信息)
  1.9	Ext4 file system parameters
        (1.9  Ext4 文件系统参数)

  2	Modifying System Parameters
        (2  修改系统参数)

  3	Per-Process Parameters
        (3  进程相关参数)
  3.1	/proc/<pid>/oom_adj & /proc/<pid>/oom_score_adj - Adjust the oom-killer
								score
        (3.1  /proc/<pid>/oom_adj 与 /proc/<pid>/oom_score_adj - 调整 oom-killer 分数)
  3.2	/proc/<pid>/oom_score - Display current oom-killer score
        (3.2  /proc/<pid>/oom_score - 显示当前的 oom-killer 分数)
  3.3	/proc/<pid>/io - Display the IO accounting fields
        (3.3  /proc/<pid>/io - 显示 IO 统计字段)
  3.4	/proc/<pid>/coredump_filter - Core dump filtering settings
        (3.4  /proc/<pid>/coredump_filter - 核心转储过滤设置)
  3.5	/proc/<pid>/mountinfo - Information about mounts
        (3.5  /proc/<pid>/mountinfo - 挂载信息)
  3.6	/proc/<pid>/comm  & /proc/<pid>/task/<tid>/comm
        (3.6  /proc/<pid>/comm 与 /proc/<pid>/task/<tid>/comm)
  3.7   /proc/<pid>/task/<tid>/children - Information about task children
        (3.7  /proc/<pid>/task/<tid>/children - 任务子进程的信息)
  3.8   /proc/<pid>/fdinfo/<fd> - Information about opened file
        (3.8  /proc/<pid>/fdinfo/<fd> - 已打开文件的信息)
  3.9   /proc/<pid>/map_files - Information about memory mapped files
        (3.9  /proc/<pid>/map_files - 内存映射文件的信息)
  3.10  /proc/<pid>/timerslack_ns - Task timerslack value
        (3.10  /proc/<pid>/timerslack_ns - 任务的 timerslack 值)
  3.11	/proc/<pid>/patch_state - Livepatch patch operation state
        (3.11  /proc/<pid>/patch_state - Livepatch 补丁操作状态)
  3.12	/proc/<pid>/arch_status - Task architecture specific information
        (3.12  /proc/<pid>/arch_status - 任务架构特定的信息)
  3.13  /proc/<pid>/fd - List of symlinks to open files
        (3.13  /proc/<pid>/fd - 打开文件的符号链接列表)
  3.14  /proc/<pid>/ksm_stat - Information about the process's ksm status.
        (3.14  /proc/<pid>/ksm_stat - 进程 ksm 状态的信息。)

  4	Configuring procfs
        (4  配置 procfs)
  4.1	Mount options
        (4.1  挂载选项)

  5	Filesystem behavior
        (5  文件系统行为)

Preface
(前言)
=======

0.1 Introduction/Credits
(0.1 引言/致谢)
------------------------

We'd like  to  thank Alan Cox, Rik van Riel, and Alexey Kuznetsov and a lot of
other people for help compiling this documentation. We'd also like to extend a
special thank  you to Andi Kleen for documentation, which we relied on heavily
to create  this  document,  as well as the additional information he provided.
Thanks to  everybody  else  who contributed source or docs to the Linux kernel
and helped create a great piece of software... :)
(我们要感谢 Alan Cox、Rik van Riel、Alexey Kuznetsov 以及许多其他为编写本文档提供帮助的人。我们还要特别感谢 Andi Kleen 提供的文档，我们编写本文档时大量依赖这些文档，以及他提供的额外信息。感谢所有其他为 Linux 内核贡献源代码或文档、帮助创造出这一伟大软件的人们……:) )

The   latest   version    of   this   document   is    available   online   at
https://www.kernel.org/doc/html/latest/filesystems/proc.html
(本文档的最新版本可在线获取：
https://www.kernel.org/doc/html/latest/filesystems/proc.html)

0.2 Legal Stuff
(0.2 法律事项)
---------------

We don't  guarantee  the  correctness  of this document, and if you come to us
complaining about  how  you  screwed  up  your  system  because  of  incorrect
documentation, we won't feel responsible...
(我们不保证本文档的正确性，如果你因为不正确的文档搞砸了自己的系统而来向我们抱怨，我们不会感到负责任……)

Chapter 1: Collecting System Information
(第 1 章：收集系统信息)
========================================

In This Chapter
(本章内容)
---------------
* Investigating  the  properties  of  the  pseudo  file  system  /proc and its
  ability to provide information on the running Linux system
  (研究伪文件系统 /proc 的属性及其提供运行中 Linux 系统信息的能力)
* Examining /proc's structure
  (考察 /proc 的结构)
* Uncovering  various  information  about the kernel and the processes running
  on the system
  (发掘关于内核以及系统上运行进程的各类信息)

------------------------------------------------------------------------------

The proc  file  system acts as an interface to internal data structures in the
kernel. It  can  be  used to obtain information about the system and to change
certain kernel parameters at runtime (sysctl).
(proc 文件系统充当内核内部数据结构的接口。它可用于获取关于系统的信息，并在运行时更改某些内核参数(sysctl)。)

First, we'll  take  a  look  at the read-only parts of /proc. In Chapter 2, we
show you how you can use /proc/sys to change settings.
(首先，我们来看 /proc 的只读部分。在第 2 章中，我们将向你展示如何使用 /proc/sys 更改设置。)

1.1 Process-Specific Subdirectories
(1.1 进程相关的子目录)
-----------------------------------

The directory  /proc  contains  (among other things) one subdirectory for each
process running on the system, which is named after the process ID (PID).
(目录 /proc 中(除其他内容外)为系统上运行的每个进程包含一个子目录，该子目录以进程 ID(PID)命名。)

The link  'self'  points to  the process reading the file system. Each process
subdirectory has the entries listed in Table 1-1.
(链接 'self' 指向正在读取该文件系统的进程。每个进程子目录都有表 1-1 中列出的条目。)

A process can read its own information from /proc/PID/* with no extra
permissions. When reading /proc/PID/* information for other processes, reading
process is required to have either CAP_SYS_PTRACE capability with
PTRACE_MODE_READ access permissions, or, alternatively, CAP_PERFMON
capability. This applies to all read-only information like `maps`, `environ`,
`pagemap`, etc. The only exception is `mem` file due to its read-write nature,
which requires CAP_SYS_PTRACE capabilities with more elevated
PTRACE_MODE_ATTACH permissions; CAP_PERFMON capability does not grant access
to /proc/PID/mem for other processes.
(进程无需额外权限即可从 /proc/PID/* 读取自身的信息。当读取其他进程的 /proc/PID/* 信息时，读取进程需要拥有带 PTRACE_MODE_READ 访问权限的 CAP_SYS_PTRACE 能力，或者替代地拥有 CAP_PERFMON 能力。这适用于所有只读信息，如 `maps`、`environ`、`pagemap` 等。唯一的例外是 `mem` 文件，由于其读写的性质，它需要具有更高 PTRACE_MODE_ATTACH 权限的 CAP_SYS_PTRACE 能力；CAP_PERFMON 能力不允许访问其他进程的 /proc/PID/mem。)

Note that an open file descriptor to /proc/<pid> or to any of its
contained files or subdirectories does not prevent <pid> being reused
for some other process in the event that <pid> exits. Operations on
open /proc/<pid> file descriptors corresponding to dead processes
never act on any new process that the kernel may, through chance, have
also assigned the process ID <pid>. Instead, operations on these FDs
usually fail with ESRCH.
(注意，指向 /proc/<pid> 或其包含的任何文件或子目录的已打开文件描述符，并不能阻止在 <pid> 退出时该 <pid> 被其他某个进程重用。对与已死亡进程对应的已打开 /proc/<pid> 文件描述符的操作，绝不会作用于内核可能碰巧又分配了同一进程 ID <pid> 的任何新进程。相反，对这些文件描述符的操作通常会以 ESRCH 失败。)

.. table:: Table 1-1: Process specific entries in /proc
          (表 1-1: /proc 中进程相关的条目)

 =============  ===============================================================
 File		Content (内容)
 =============  ===============================================================
 clear_refs	Clears page referenced bits shown in smaps output (清除 smaps 输出中显示的页引用位)
 cmdline	Command line arguments (命令行参数)
 cpu		Current and last cpu in which it was executed	(2.4)(smp) (当前及最近一次执行所在的 cpu (2.4)(smp))
 cwd		Link to the current working directory (指向当前工作目录的链接)
 environ	Values of environment variables (环境变量的值)
 exe		Link to the executable of this process (指向本进程可执行文件的链接)
 fd		Directory, which contains all file descriptors (包含所有文件描述符的目录)
 maps		Memory maps to executables and library files	(2.4) (可执行文件与库文件的内存映射 (2.4))
 mem		Memory held by this process (本进程持有的内存)
 root		Link to the root directory of this process (指向本进程根目录的链接)
 stat		Process status (进程状态)
 statm		Process memory status information (进程内存状态信息)
 status		Process status in human readable form (人类可读形式的进程状态)
 wchan		Present with CONFIG_KALLSYMS=y: it shows the kernel function
		symbol the task is blocked in - or "0" if not blocked. (在 CONFIG_KALLSYMS=y 时存在：显示任务阻塞所在的内核函数符号，若未阻塞则为 "0"。)
 pagemap	Page table (页表)
 stack		Report full stack trace, enable via CONFIG_STACKTRACE (报告完整的栈回溯，需通过 CONFIG_STACKTRACE 启用)
 smaps		An extension based on maps, showing the memory consumption of
		each mapping and flags associated with it (基于 maps 的扩展，显示每个映射的内存消耗及相关标志)
 smaps_rollup	Accumulated smaps stats for all mappings of the process.  This
		can be derived from smaps, but is faster and more convenient (该进程所有映射的 smaps 统计汇总。可以从 smaps 推导得出，但更快也更方便)
 numa_maps	An extension based on maps, showing the memory locality and
		binding policy as well as mem usage (in pages) of each mapping. (基于 maps 的扩展，显示每个映射的内存本地性与绑定策略以及内存使用量(以页为单位))
 =============  ===============================================================

For example, to get the status information of a process, all you have to do is
read the file /proc/PID/status::
(例如，要获取某个进程的状态信息，只需读取文件 /proc/PID/status：)

  >cat /proc/self/status
  Name:   cat
  State:  R (running)
  Tgid:   5452
  Pid:    5452
  PPid:   743
  TracerPid:      0						(2.4)
  Uid:    501     501     501     501
  Gid:    100     100     100     100
  FDSize: 256
  Groups: 100 14 16
  Kthread:    0
  VmPeak:     5004 kB
  VmSize:     5004 kB
  VmLck:         0 kB
  VmHWM:       476 kB
  VmRSS:       476 kB
  RssAnon:             352 kB
  RssFile:             120 kB
  RssShmem:              4 kB
  VmData:      156 kB
  VmStk:        88 kB
  VmExe:        68 kB
  VmLib:      1412 kB
  VmPTE:        20 kb
  VmSwap:        0 kB
  HugetlbPages:          0 kB
  CoreDumping:    0
  THP_enabled:	  1
  Threads:        1
  SigQ:   0/28578
  SigPnd: 0000000000000000
  ShdPnd: 0000000000000000
  SigBlk: 0000000000000000
  SigIgn: 0000000000000000
  SigCgt: 0000000000000000
  CapInh: 00000000fffffeff
  CapPrm: 0000000000000000
  CapEff: 0000000000000000
  CapBnd: ffffffffffffffff
  CapAmb: 0000000000000000
  NoNewPrivs:     0
  Seccomp:        0
  Speculation_Store_Bypass:       thread vulnerable
  SpeculationIndirectBranch:      conditional enabled
  voluntary_ctxt_switches:        0
  nonvoluntary_ctxt_switches:     1

This shows you nearly the same information you would get if you viewed it with
the ps  command.  In  fact,  ps  uses  the  proc  file  system  to  obtain its
information.  But you get a more detailed  view of the  process by reading the
file /proc/PID/status. It fields are described in table 1-2.
(这显示的信息与你使用 ps 命令查看时得到的几乎相同。实际上，ps 就是使用 proc 文件系统来获取其信息的。但通过读取文件 /proc/PID/status，你可以看到进程更详细的视图。它的字段在表 1-2 中描述。)

The  statm  file  contains  more  detailed  information about the process
memory usage. Its seven fields are explained in Table 1-3.  The stat file
contains detailed information about the process itself.  Its fields are
explained in Table 1-4.
(statm 文件包含关于进程内存使用的更详细信息。它的七个字段在表 1-3 中解释。stat 文件包含关于进程本身的详细信息。它的字段在表 1-4 中解释。)

(for SMP CONFIG users)
((针对 SMP CONFIG 用户))

For making accounting scalable, RSS related information are handled in an
asynchronous manner and the value may not be very precise. To see a precise
snapshot of a moment, you can see /proc/<pid>/smaps file and scan page table.
It's slow but very precise.
(为了使统计具有可伸缩性，与 RSS 相关的信息以异步方式处理，其值可能不十分精确。要查看某一时刻的精确快照，可以查看 /proc/<pid>/smaps 文件并扫描页表。它很慢但非常精确。)

.. table:: Table 1-2: Contents of the status fields (as of 4.19)
          (表 1-2: status 字段的内容(截至 4.19))

 ==========================  ===================================================
 Field                       Content (字段  内容)
 ==========================  ===================================================
 Name                        filename of the executable (可执行文件的文件名)
 Umask                       file mode creation mask (文件模式创建掩码)
 State                       state (R is running, S is sleeping, D is sleeping
                             in an uninterruptible wait, Z is zombie,
			     T is traced or stopped) (状态(R 为运行，S 为睡眠，D 为不可中断等待中的睡眠，Z 为僵尸，T 为被跟踪或停止))
 Tgid                        thread group ID (线程组 ID)
 Ngid                        NUMA group ID (0 if none) (NUMA 组 ID(若无则为 0))
 Pid                         process id (进程 ID)
 PPid                        process id of the parent process (父进程的进程 ID)
 TracerPid                   PID of process tracing this process (0 if not, or
                             the tracer is outside of the current pid namespace) (跟踪本进程的进程 PID(若没有，或跟踪者位于当前 pid 命名空间之外，则为 0))
 Uid                         Real, effective, saved set, and  file system UIDs (真实、有效、保存集和文件系统 UID)
 Gid                         Real, effective, saved set, and  file system GIDs (真实、有效、保存集和文件系统 GID)
 FDSize                      number of file descriptor slots currently allocated (当前已分配的文件描述符槽位数)
 Groups                      supplementary group list (补充组列表)
 NStgid                      descendant namespace thread group ID hierarchy (后代命名空间线程组 ID 层次结构)
 NSpid                       descendant namespace process ID hierarchy (后代命名空间进程 ID 层次结构)
 NSpgid                      descendant namespace process group ID hierarchy (后代命名空间进程组 ID 层次结构)
 NSsid                       descendant namespace session ID hierarchy (后代命名空间会话 ID 层次结构)
 Kthread                     kernel thread flag, 1 is yes, 0 is no (内核线程标志，1 为是，0 为否)
 VmPeak                      peak virtual memory size (虚拟内存大小峰值)
 VmSize                      total program size (程序总大小)
 VmLck                       locked memory size (已锁定的内存大小)
 VmPin                       pinned memory size (已固定(pin)的内存大小)
 VmHWM                       peak resident set size ("high water mark") (常驻集大小峰值("高水位"))
 VmRSS                       size of memory portions. It contains the three
                             following parts
                             (VmRSS = RssAnon + RssFile + RssShmem) (内存驻留部分的大小。它包含以下三个部分 (VmRSS = RssAnon + RssShmem))
 RssAnon                     size of resident anonymous memory (常驻匿名内存的大小)
 RssFile                     size of resident file mappings (常驻文件映射的大小)
 RssShmem                    size of resident shmem memory (includes SysV shm,
                             mapping of tmpfs and shared anonymous mappings) (常驻 shmem 内存的大小(包括 SysV shm、tmpfs 的映射和共享匿名映射))
 VmData                      size of private data segments (私有数据段的大小)
 VmStk                       size of stack segments (栈段的大小)
 VmExe                       size of text segment (文本段的大小)
 VmLib                       size of shared library code (共享库代码的大小)
 VmPTE                       size of page table entries (页表项的大小)
 VmSwap                      amount of swap used by anonymous private data
                             (shmem swap usage is not included) (匿名私有数据使用的交换空间量(不包括 shmem 的交换使用量))
 HugetlbPages                size of hugetlb memory portions (hugetlb 内存部分的大小)
 CoreDumping                 process's memory is currently being dumped
                             (killing the process may lead to a corrupted core) (进程的内存当前正在被转储(杀死该进程可能导致核心文件损坏))
 THP_enabled                 process is allowed to use THP (returns 0 when
                             PR_SET_THP_DISABLE is set on the process to disable
                             THP completely, not just partially) (进程被允许使用 THP(当在进程上设置了 PR_SET_THP_DISABLE 以完全(而非仅部分)禁用 THP 时返回 0))
 Threads                     number of threads (线程数)
 SigQ                        number of signals queued/max. number for queue (已排队信号数/队列最大数量)
 SigPnd                      bitmap of pending signals for the thread (线程待处理信号的位图)
 ShdPnd                      bitmap of shared pending signals for the process (进程共享待处理信号的位图)
 SigBlk                      bitmap of blocked signals (被阻塞信号的位图)
 SigIgn                      bitmap of ignored signals (被忽略信号的位图)
 SigCgt                      bitmap of caught signals (被捕获信号的位图)
 CapInh                      bitmap of inheritable capabilities (可继承能力的位图)
 CapPrm                      bitmap of permitted capabilities (被允许能力的位图)
 CapEff                      bitmap of effective capabilities (有效能力的位图)
 CapBnd                      bitmap of capabilities bounding set (能力边界集的位图)
 CapAmb                      bitmap of ambient capabilities (环境(ambient)能力的位图)
 NoNewPrivs                  no_new_privs, like prctl(PR_GET_NO_NEW_PRIV, ...) (no_new_privs，类似 prctl(PR_GET_NO_NEW_PRIV, ...))
 Seccomp                     seccomp mode, like prctl(PR_GET_SECCOMP, ...) (seccomp 模式，类似 prctl(PR_GET_SECCOMP, ...))
 Speculation_Store_Bypass    speculative store bypass mitigation status (推测性存储旁路缓解状态)
 SpeculationIndirectBranch   indirect branch speculation mode (间接分支推测模式)
 Cpus_allowed                mask of CPUs on which this process may run (本进程可在其上运行的 CPU 掩码)
 Cpus_allowed_list           Same as previous, but in "list format" (同上一个，但为"列表格式")
 Mems_allowed                mask of memory nodes allowed to this process (本进程可用的内存节点掩码)
 Mems_allowed_list           Same as previous, but in "list format" (同上一个，但为"列表格式")
 voluntary_ctxt_switches     number of voluntary context switches (自愿上下文切换次数)
 nonvoluntary_ctxt_switches  number of non voluntary context switches (非自愿上下文切换次数)
 ==========================  ===================================================


.. table:: Table 1-3: Contents of the statm fields (as of 2.6.8-rc3)
          (表 1-3: statm 字段的内容(截至 2.6.8-rc3))

 ======== ===============================	==============================
 Field    Content (字段  内容)
 ======== ===============================	==============================
 size     total program size (pages)		(same as VmSize in status) (总程序大小(页数)(与 status 中的 VmSize 相同))
 resident size of memory portions (pages)	(same as VmRSS in status) (内存驻留部分的大小(页数)(与 status 中的 VmRSS 相同))
 shared   number of pages that are shared	(i.e. backed by a file, same
						as RssFile+RssShmem in status) (被共享的页数(即由文件支撑的，同 status 中的 RssFile+RssShmem))
 trs      number of pages that are 'code'	(not including libs; broken,
						includes data segment) ('代码'页数(不包括库；已损坏，包含数据段))
 lrs      number of pages of library		(always 0 on 2.6) (库的页数(在 2.6 上始终为 0))
 drs      number of pages of data/stack		(including libs; broken,
						includes library text) (数据/栈的页数(包括库；已损坏，包含库文本))
 dt       number of dirty pages			(always 0 on 2.6) (脏页数(在 2.6 上始终为 0))
 ======== ===============================	==============================


.. table:: Table 1-4: Contents of the stat fields (as of 2.6.30-rc7)
          (表 1-4: stat 字段的内容(截至 2.6.30-rc7))

  ============= ===============================================================
  Field         Content (字段  内容)
  ============= ===============================================================
  pid           process id (进程 ID)
  tcomm         filename of the executable (可执行文件的文件名)
  state         state (R is running, S is sleeping, D is sleeping in an
                uninterruptible wait, Z is zombie, T is traced or stopped) (状态(R 为运行，S 为睡眠，D 为不可中断等待中的睡眠，Z 为僵尸，T 为被跟踪或停止))
  ppid          process id of the parent process (父进程的进程 ID)
  pgrp          pgrp of the process (进程的进程组)
  sid           session id (会话 ID)
  tty_nr        tty the process uses (进程使用的 tty)
  tty_pgrp      pgrp of the tty (tty 的进程组)
  flags         task flags (任务标志)
  min_flt       number of minor faults (次缺页(fault)次数)
  cmin_flt      number of minor faults with child's (连同子进程的次缺页次数)
  maj_flt       number of major faults (主缺页次数)
  cmaj_flt      number of major faults with child's (连同子进程的主缺页次数)
  utime         user mode jiffies (用户态 jiffies)
  stime         kernel mode jiffies (内核态 jiffies)
  cutime        user mode jiffies with child's (连同子进程的用户态 jiffies)
  cstime        kernel mode jiffies with child's (连同子进程的内核态 jiffies)
  priority      priority level (优先级)
  nice          nice level (nice 值)
  num_threads   number of threads (线程数)
  it_real_value	(obsolete, always 0) (已废弃，始终为 0)
  start_time    time the process started after system boot (进程在系统启动后的启动时间)
  vsize         virtual memory size (虚拟内存大小)
  rss           resident set memory size (常驻集内存大小)
  rsslim        current limit in bytes on the rss (rss 的当前字节限制)
  start_code    address above which program text can run (程序文本可运行的地址上界)
  end_code      address below which program text can run (程序文本可运行的地址下界)
  start_stack   address of the start of the main process stack (主进程栈的起始地址)
  esp           current value of ESP (ESP 的当前值)
  eip           current value of EIP (EIP 的当前值)
  pending       bitmap of pending signals (待处理信号的位图)
  blocked       bitmap of blocked signals (被阻塞信号的位图)
  sigign        bitmap of ignored signals (被忽略信号的位图)
  sigcatch      bitmap of caught signals (被捕获信号的位图)
  0		(place holder, used to be the wchan address,
		use /proc/PID/wchan instead) (占位符，曾用于 wchan 地址，请改用 /proc/PID/wchan)
  0             (place holder) (占位符)
  0             (place holder) (占位符)
  exit_signal   signal to send to parent thread on exit (退出时发送给父线程的信号)
  task_cpu      which CPU the task is scheduled on (任务被调度到哪个 CPU 上)
  rt_priority   realtime priority (实时优先级)
  policy        scheduling policy (man sched_setscheduler) (调度策略(man sched_setscheduler))
  blkio_ticks   time spent waiting for block IO (等待块 IO 所花费的时间)
  gtime         guest time of the task in jiffies (任务的客户机(guest)时间，以 jiffies 计)
  cgtime        guest time of the task children in jiffies (任务子进程的客户机时间，以 jiffies 计)
  start_data    address above which program data+bss is placed (程序 data+bss 存放的地址上界)
  end_data      address below which program data+bss is placed (程序 data+bss 存放的地址下界)
  start_brk     address above which program heap can be expanded with brk() (可用 brk() 扩展程序堆的地址上界)
  arg_start     address above which program command line is placed (程序命令行存放的地址上界)
  arg_end       address below which program command line is placed (程序命令行存放的地址下界)
  env_start     address above which program environment is placed (程序环境变量存放的地址上界)
  env_end       address below which program environment is placed (程序环境变量存放的地址下界)
  exit_code     the thread's exit_code in the form reported by the waitpid
		system call (线程的 exit_code，以 waitpid 系统调用报告的形式给出)
  ============= ===============================================================

The /proc/PID/maps file contains the currently mapped memory regions and
their access permissions.
(/proc/PID/maps 文件包含当前已映射的内存区域及其访问权限。)

The format is::
(格式如下：)

    address           perms offset  dev   inode      pathname

    08048000-08049000 r-xp 00000000 03:00 8312       /opt/test
    08049000-0804a000 rw-p 00001000 03:00 8312       /opt/test
    0804a000-0806b000 rw-p 00000000 00:00 0          [heap]
    a7cb1000-a7cb2000 ---p 00000000 00:00 0
    a7cb2000-a7eb2000 rw-p 00000000 00:00 0
    a7eb2000-a7eb3000 ---p 00000000 00:00 0
    a7eb3000-a7ed5000 rw-p 00000000 00:00 0
    a7ed5000-a8008000 r-xp 00000000 03:00 4222       /lib/libc.so.6
    a8008000-a800a000 r--p 00133000 03:00 4222       /lib/libc.so.6
    a800a000-a800b000 rw-p 00135000 03:00 4222       /lib/libc.so.6
    a800b000-a800e000 rw-p 00000000 00:00 0
    a800e000-a8022000 r-xp 00000000 03:00 14462      /lib/libpthread.so.0
    a8022000-a8023000 r--p 00013000 03:00 14462      /lib/libpthread.so.0
    a8023000-a8024000 rw-p 00014000 03:00 14462      /lib/libpthread.so.0
    a8024000-a8027000 rw-p 00000000 00:00 0
    a8027000-a8043000 r-xp 00000000 03:00 8317       /lib/ld-linux.so.2
    a8043000-a8044000 r--p 0001b000 03:00 8317       /lib/ld-linux.so.2
    a8044000-a8045000 rw-p 0001c000 03:00 8317       /lib/ld-linux.so.2
    aff35000-aff4a000 rw-p 00000000 00:00 0          [stack]
    ffffe000-fffff000 r-xp 00000000 00:00 0          [vdso]

where "address" is the address space in the process that it occupies, "perms"
is a set of permissions::
(其中 "address" 是它在进程中所占用的地址空间，"perms" 是一组权限：)

 r = read
 w = write
 x = execute
 s = shared
 p = private (copy on write)

"offset" is the offset into the mapping, "dev" is the device (major:minor), and
"inode" is the inode  on that device.  0 indicates that  no inode is associated
with the memory region, as the case would be with BSS (uninitialized data).
The "pathname" shows the name associated file for this mapping.  If the mapping
is not associated with a file:
("offset" 是该映射内的偏移量，"dev" 是设备(主:次)，"inode" 是该设备上的 inode。0 表示该内存区域没有关联的 inode，BSS(未初始化数据)就是这种情况。"pathname" 显示与此映射关联的文件名。如果该映射未与文件关联：)

 ===================        ===========================================
 [heap]                     the heap of the program (程序的堆)
 [stack]                    the stack of the main process (主进程的栈)
 [vdso]                     the "virtual dynamic shared object",
                            the kernel system call handler ("虚拟动态共享对象"，内核系统调用处理程序)
 [anon:<name>]              a private anonymous mapping that has been
                            named by userspace (已由用户空间命名的私有匿名映射)
 [anon_shmem:<name>]        an anonymous shared memory mapping that has
                            been named by userspace (已由用户空间命名的匿名共享内存映射)
 ===================        ===========================================

 or if empty, the mapping is anonymous.
 (若为空，则该映射是匿名的。)

Starting with 6.11 kernel, /proc/PID/maps provides an alternative
ioctl()-based API that gives ability to flexibly and efficiently query and
filter individual VMAs. This interface is binary and is meant for more
efficient and easy programmatic use. `struct procmap_query`, defined in
linux/fs.h UAPI header, serves as an input/output argument to the
`PROCMAP_QUERY` ioctl() command. See comments in linus/fs.h UAPI header for
details on query semantics, supported flags, data returned, and general API
usage information.
(从 6.11 内核开始，/proc/PID/maps 提供了一种基于 ioctl() 的替代 API，能够灵活而高效地查询和过滤单个 VMA。该接口是二进制的，旨在实现更高效、更便捷的编程式使用。定义在 linux/fs.h UAPI 头文件中的 `struct procmap_query` 用作 `PROCMAP_QUERY` ioctl() 命令的输入/输出参数。关于查询语义、支持的标志、返回的数据以及 API 的一般使用信息，请参见 linus/fs.h UAPI 头文件中的注释。)

The /proc/PID/smaps is an extension based on maps, showing the memory
consumption for each of the process's mappings. For each mapping (aka Virtual
Memory Area, or VMA) there is a series of lines such as the following::
(/proc/PID/smaps 是基于 maps 的扩展，显示进程每个映射的内存消耗。对于每个映射(又称虚拟内存区域，即 VMA)，都有一系列如下所示的行：)

    08048000-080bc000 r-xp 00000000 03:02 13130      /bin/bash

    Size:               1084 kB
    KernelPageSize:        4 kB
    MMUPageSize:           4 kB
    Rss:                 892 kB
    Pss:                 374 kB
    Pss_Dirty:             0 kB
    Shared_Clean:        892 kB
    Shared_Dirty:          0 kB
    Private_Clean:         0 kB
    Private_Dirty:         0 kB
    Referenced:          892 kB
    Anonymous:             0 kB
    KSM:                   0 kB
    LazyFree:              0 kB
    AnonHugePages:         0 kB
    FilePmdMapped:         0 kB
    ShmemPmdMapped:        0 kB
    Shared_Hugetlb:        0 kB
    Private_Hugetlb:       0 kB
    Swap:                  0 kB
    SwapPss:               0 kB
    Locked:                0 kB
    THPeligible:           0
    VmFlags: rd ex mr mw me dw

The first of these lines shows the same information as is displayed for
the mapping in /proc/PID/maps.  Following lines show the size of the
mapping (size); the smallest possible page size allocated when backing a
VMA (KernelPageSize), which is the granularity in which VMA modifications
can be performed; the smallest possible page size that could be used by the
MMU (MMUPageSize) when backing a VMA; the amount of the mapping that is
currently resident in RAM (RSS); the process's proportional share of this
mapping (PSS); and the number of clean and dirty shared and private pages
in the mapping.
(其中第一行显示的信息与 /proc/PID/maps 中该映射显示的信息相同。随后的各行显示映射的大小(size)；支撑 VMA 时所分配的最小可能页大小(KernelPageSize)，它是执行 VMA 修改的粒度；支撑 VMA 时 MMU 可能使用的最小页大小(MMUPageSize)；该映射当前驻留在 RAM 中的量(RSS)；进程对此映射的比例份额(PSS)；以及映射中干净和脏的共享页与私有页的数量。)

"KernelPageSize" always corresponds to "MMUPageSize", except when a larger
kernel page size is emulated on a system with a smaller page size used by the
MMU, which is the case for some PPC64 setups with hugetlb.  Furthermore,
"KernelPageSize" and "MMUPageSize" always correspond to the smallest
possible granularity (fallback) that can be encountered in a VMA throughout
its lifetime.  These values are not affected by Transparent Huge Pages
being in effect, or any usage of larger MMU page sizes (either through
architectural huge-page mappings or other explicit/implicit coalescing of
virtual ranges performed by the MMU).  "AnonHugePages", "ShmemPmdMapped" and
"FilePmdMapped" provide insight into the usage of PMD-level architectural
huge-page mappings.
("KernelPageSize" 始终与 "MMUPageSize" 对应，除非在 MMU 使用较小页大小的系统上模拟较大的内核页大小，某些采用 hugetlb 的 PPC64 配置就是这种情况。此外，"KernelPageSize" 和 "MMUPageSize" 始终对应于 VMA 在其整个生命周期内可能遇到的最小粒度(回退粒度)。这些值不受透明巨页生效与否的影响，也不受任何较大 MMU 页大小使用的影响(无论是通过架构巨页映射，还是由 MMU 执行的其他对虚拟范围的显式/隐式合并)。"AnonHugePages"、"ShmemPmdMapped" 和 "FilePmdMapped" 揭示了 PMD 级架构巨页映射的使用情况。)

The "proportional set size" (PSS) of a process is the count of pages it has
in memory, where each page is divided by the number of processes sharing it.
So if a process has 1000 pages all to itself, and 1000 shared with one other
process, its PSS will be 1500.  "Pss_Dirty" is the portion of PSS which
consists of dirty pages.  ("Pss_Clean" is not included, but it can be
calculated by subtracting "Pss_Dirty" from "Pss".)
(进程的"比例集大小"(PSS)是它在内存中拥有的页数，其中每一页除以共享它的进程数。因此，如果一个进程独占 1000 页，并与另一个进程共享 1000 页，那么它的 PSS 将是 1500。"Pss_Dirty" 是 PSS 中由脏页构成的部分。("Pss_Clean" 未包含在内，但可以用 "Pss" 减去 "Pss_Dirty" 计算得出。))

Traditionally, a page is accounted as "private" if it is mapped exactly once,
and a page is accounted as "shared" when mapped multiple times, even when
mapped in the same process multiple times. Note that this accounting is
independent of MAP_SHARED.
(传统上，如果一页恰好被映射一次，则该页被计为"私有"；当被映射多次时，则计为"共享"，即使是在同一进程中被多次映射。注意，这种统计方式与 MAP_SHARED 无关。)

In some kernel configurations, the semantics of pages part of a larger
allocation (e.g., THP) can differ: a page is accounted as "private" if all
pages part of the corresponding large allocation are *certainly* mapped in the
same process, even if the page is mapped multiple times in that process. A
page is accounted as "shared" if any page page of the larger allocation
is *maybe* mapped in a different process. In some cases, a large allocation
might be treated as "maybe mapped by multiple processes" even though this
is no longer the case.
(在某些内核配置中，属于较大分配(例如 THP)的页的语义可能有所不同：如果对应大分配的所有页*确定*都映射在同一进程中，即使该页在该进程中被映射多次，该页也计为"私有"。如果大分配的任何一页*可能*被映射到不同的进程中，该页就计为"共享"。在某些情况下，即使大分配已不再被多个进程映射，它也可能被当作"可能被多个进程映射"来对待。)

Some kernel configurations do not track the precise number of times a page part
of a larger allocation is mapped. In this case, when calculating the PSS, the
average number of mappings per page in this larger allocation might be used
as an approximation for the number of mappings of a page. The PSS calculation
will be imprecise in this case.
(某些内核配置不会精确跟踪属于较大分配的页被映射的次数。在这种情况下，计算 PSS 时，可能会用该较大分配中每页的平均映射次数来近似一页的映射次数。此时 PSS 的计算将是不精确的。)

"Referenced" indicates the amount of memory currently marked as referenced or
accessed.
("Referenced" 表示当前被标记为已引用或已访问的内存量。)

"Anonymous" shows the amount of memory that does not belong to any file.  Even
a mapping associated with a file may contain anonymous pages: when MAP_PRIVATE
and a page is modified, the file page is replaced by a private anonymous copy.
("Anonymous" 显示不属于任何文件的内存量。即使是与文件关联的映射也可能包含匿名页：当使用 MAP_PRIVATE 且某页被修改时，文件页会被一个私有的匿名副本替换。)

"KSM" reports how many of the pages are KSM pages. Note that KSM-placed zeropages
are not included, only actual KSM pages.
("KSM" 报告其中有多少页是 KSM 页。注意，不包括由 KSM 放置的零页(zeropage)，只包括实际的 KSM 页。)

"LazyFree" shows the amount of memory which is marked by madvise(MADV_FREE).
The memory isn't freed immediately with madvise(). It's freed in memory
pressure if the memory is clean. Please note that the printed value might
be lower than the real value due to optimizations used in the current
implementation. If this is not desirable please file a bug report.
("LazyFree" 显示由 madvise(MADV_FREE) 标记的内存量。这些内存不会在调用 madvise() 时立即释放。当内存紧张且该内存是干净的时候，它才会被释放。请注意，由于当前实现所使用的优化，打印的值可能低于真实值。如果这不合需要，请提交错误报告。)

"AnonHugePages", "ShmemPmdMapped" and "FilePmdMapped" show the amount of
memory backed by Transparent Huge Pages that are currently mapped by
architectural huge-page mappings at the PMD level. "AnonHugePages"
corresponds to memory that does not belong to a file, "ShmemPmdMapped" to
shared memory (shmem/tmpfs) and "FilePmdMapped" to file-backed memory
(excluding shmem/tmpfs).
("AnonHugePages"、"ShmemPmdMapped" 和 "FilePmdMapped" 显示当前由 PMD 级架构巨页映射的、由透明巨页支撑的内存量。"AnonHugePages" 对应不属于文件的内存，"ShmemPmdMapped" 对应共享内存(shmem/tmpfs)，"FilePmdMapped" 对应文件支撑的内存(不包括 shmem/tmpfs)。)

There are no dedicated entries for Transparent Huge Pages (or similar concepts)
that are not mapped by architectural huge-page mappings at the PMD level.
(对于未由 PMD 级架构巨页映射的透明巨页(或类似概念)，没有专门的条目。)

"Shared_Hugetlb" and "Private_Hugetlb" show the amounts of memory backed by
hugetlbfs page which is *not* counted in "RSS" or "PSS" field for historical
reasons. And these are not included in {Shared,Private}_{Clean,Dirty} field.
("Shared_Hugetlb" 和 "Private_Hugetlb" 显示由 hugetlbfs 页支撑的内存量，出于历史原因，这些内存*不*计入 "RSS" 或 "PSS" 字段。而且它们也不包括在 {Shared,Private}_{Clean,Dirty} 字段中。)

"Swap" shows how much would-be-anonymous memory is also used, but out on swap.
("Swap" 显示还有多少本应匿名的内存被使用，但被换出到了交换空间。)

For shmem mappings, "Swap" includes also the size of the mapped (and not
replaced by copy-on-write) part of the underlying shmem object out on swap.
"SwapPss" shows proportional swap share of this mapping. Unlike "Swap", this
does not take into account swapped out page of underlying shmem objects.
"Locked" indicates whether the mapping is locked in memory or not.
(对于 shmem 映射，"Swap" 还包括底层 shmem 对象被换出的、已映射(且未被写时复制替换)部分的大小。"SwapPss" 显示此映射按比例分摊的交换空间份额。与 "Swap" 不同，它不考虑底层 shmem 对象被换出的页。"Locked" 表示该映射是否被锁定在内存中。)

"THPeligible" indicates whether the mapping is eligible for allocating
naturally aligned THP pages of any currently enabled size. 1 if true, 0
otherwise.
("THPeligible" 表示该映射是否有资格分配任意当前已启用大小的自然对齐 THP 页。为真则为 1，否则为 0。)

If both the kernel and the CPU support protection keys (pkeys),
"ProtectionKey" indicates the memory protection key associated with the
virtual memory area.
(如果内核和 CPU 都支持保护键(pkeys)，"ProtectionKey" 表示与该虚拟内存区域关联的内存保护键。)

"VmFlags" field deserves a separate description. This member represents the
kernel flags associated with the particular virtual memory area in two letter
encoded manner. The codes are the following:
("VmFlags" 字段值得单独说明。该成员以两个字母编码的方式表示与特定虚拟内存区域关联的内核标志。这些代码如下：)

    ==    =============================================================
    rd    readable (可读)
    wr    writeable (可写)
    ex    executable (可执行)
    sh    shared (共享)
    mr    may read (可能读取)
    mw    may write (可能写入)
    me    may execute (可能执行)
    ms    may share (可能共享)
    gd    stack segment growns down (栈段向下增长)
    pf    pure PFN range (纯 PFN 范围)
    lo    pages are locked in memory (页被锁定在内存中)
    io    memory mapped I/O area (内存映射 I/O 区域)
    sr    sequential read advise provided (提供了顺序读取建议)
    rr    random read advise provided (提供了随机读取建议)
    dc    do not copy area on fork (fork 时不复制该区域)
    de    do not expand area on remapping (重映射时不扩展该区域)
    ac    area is accountable (该区域是可统计的)
    nr    swap space is not reserved for the area (未为该区域保留交换空间)
    ht    area uses huge tlb pages (该区域使用巨页(huge tlb)页)
    sf    synchronous page fault (同步缺页)
    ar    architecture specific flag (架构特定标志)
    wf    wipe on fork (fork 时擦除)
    dd    do not include area into core dump (不把该区域包含进核心转储)
    sd    soft dirty flag (软脏(soft dirty)标志)
    mm    mixed map area (混合映射区域)
    hg    huge page advise flag (巨页建议标志)
    nh    no huge page advise flag (无巨页建议标志)
    mg    mergeable advise flag (可合并建议标志)
    bt    arm64 BTI guarded page (arm64 BTI 受保护页)
    mt    arm64 MTE allocation tags are enabled (arm64 MTE 分配标签已启用)
    um    userfaultfd missing tracking (userfaultfd 缺页跟踪)
    uw    userfaultfd wr-protect tracking (userfaultfd 写保护跟踪)
    ui    userfaultfd minor fault (userfaultfd 次缺页)
    ss    shadow/guarded control stack page (影子/受保护控制栈页)
    sl    sealed (已封印(sealed))
    lf    lock on fault pages (缺页时锁定的页)
    dp    always lazily freeable mapping (始终可延迟释放的映射)
    gu    maybe contains guard regions (if not set, definitely doesn't) (可能包含保护区域(若未设置则肯定不包含))
    ==    =============================================================

Note that there is no guarantee that every flag and associated mnemonic will
be present in all further kernel releases. Things get changed, the flags may
be vanished or the reverse -- new added. Interpretation of their meaning
might change in future as well. So each consumer of these flags has to
follow each specific kernel version for the exact semantic.
(注意，无法保证每个标志及其关联的助记符在以后的所有内核版本中都存在。情况会发生变化，标志可能消失，也可能相反——新增标志。对其含义的解释将来也可能改变。因此，这些标志的每个使用者都必须针对每个具体的内核版本来确定确切的语义。)

This file is only present if the CONFIG_MMU kernel configuration option is
enabled.
(只有启用了 CONFIG_MMU 内核配置选项时，此文件才存在。)

Note: reading /proc/PID/maps or /proc/PID/smaps is inherently racy (consistent
output can be achieved only in the single read call).
(注意：读取 /proc/PID/maps 或 /proc/PID/smaps 本质上存在竞态(只有在单次 read 调用中才能获得一致的输出)。)

This typically manifests when doing partial reads of these files while the
memory map is being modified.  Despite the races, we do provide the following
guarantees:
(这通常在进行部分读取这些文件的同时内存映射正被修改时表现出来。尽管存在竞态，我们确实提供以下保证：)

1) The mapped addresses never go backwards, which implies no two
   regions will ever overlap.
   (已映射的地址从不回退，这意味着任何两个区域都不会重叠。)
2) If there is something at a given vaddr during the entirety of the
   life of the smaps/maps walk, there will be some output for it.
   (如果在 smaps/maps 遍历的整个生命周期内某个给定的虚拟地址上一直有内容，那么就会有针对它的某些输出。)

The /proc/PID/smaps_rollup file includes the same fields as /proc/PID/smaps,
but their values are the sums of the corresponding values for all mappings of
the process.  Additionally, it contains these fields:
(/proc/PID/smaps_rollup 文件包含与 /proc/PID/smaps 相同的字段，但其值是该进程所有映射的对应值之和。此外，它还包含以下字段：)

- Pss_Anon
- Pss_File
- Pss_Shmem

They represent the proportional shares of anonymous, file, and shmem pages, as
described for smaps above.  These fields are omitted in smaps since each
mapping identifies the type (anon, file, or shmem) of all pages it contains.
Thus all information in smaps_rollup can be derived from smaps, but at a
significantly higher cost.
(它们分别表示匿名页、文件页和 shmem 页的比例份额，如上文针对 smaps 所述。这些字段在 smaps 中被省略，因为每个映射都标明了它包含的所有页的类型(匿名、文件或 shmem)。因此，smaps_rollup 中的所有信息都可以从 smaps 推导得出，但代价要高得多。)

The /proc/PID/clear_refs is used to reset the PG_Referenced and ACCESSED/YOUNG
bits on both physical and virtual pages associated with a process, and the
soft-dirty bit on pte (see Documentation/admin-guide/mm/soft-dirty.rst
for details).
(/proc/PID/clear_refs 用于重置与进程关联的物理页和虚拟页上的 PG_Referenced 与 ACCESSED/YOUNG 位，以及 pte 上的软脏位(详情参见 Documentation/admin-guide/mm/soft-dirty.rst)。)
To clear the bits for all the pages associated with the process::
(要清除与该进程关联的所有页的这些位：)

    > echo 1 > /proc/PID/clear_refs

To clear the bits for the anonymous pages associated with the process::
(要清除与该进程关联的匿名页的这些位：)

    > echo 2 > /proc/PID/clear_refs

To clear the bits for the file mapped pages associated with the process::
(要清除与该进程关联的文件映射页的这些位：)

    > echo 3 > /proc/PID/clear_refs

To clear the soft-dirty bit::
(要清除软脏位：)

    > echo 4 > /proc/PID/clear_refs

To reset the peak resident set size ("high water mark") to the process's
current value::
(要将常驻集大小峰值("高水位")重置为该进程的当前值：)

    > echo 5 > /proc/PID/clear_refs

Any other value written to /proc/PID/clear_refs will have no effect.
(写入 /proc/PID/clear_refs 的任何其他值都不会有效果。)

The /proc/pid/pagemap gives the PFN, which can be used to find the pageflags
using /proc/kpageflags and number of times a page is mapped using
/proc/kpagecount. For detailed explanation, see
Documentation/admin-guide/mm/pagemap.rst.
(/proc/pid/pagemap 给出 PFN，PFN 可用于通过 /proc/kpageflags 查找页标志(pageflags)，并通过 /proc/kpagecount 查找一页被映射的次数。详细说明参见 Documentation/admin-guide/mm/pagemap.rst。)

The /proc/pid/numa_maps is an extension based on maps, showing the memory
locality and binding policy, as well as the memory usage (in pages) of
each mapping. The output follows a general format where mapping details get
summarized separated by blank spaces, one mapping per each file line::
(/proc/pid/numa_maps 是基于 maps 的扩展，显示内存本地性和绑定策略，以及每个映射的内存使用量(以页为单位)。输出遵循一种通用格式：映射细节以空格分隔汇总，每个文件行对应一个映射：)

    address   policy    mapping details

    00400000 default file=/usr/local/bin/app mapped=1 active=0 N3=1 kernelpagesize_kB=4
    00600000 default file=/usr/local/bin/app anon=1 dirty=1 N3=1 kernelpagesize_kB=4
    3206000000 default file=/lib64/ld-2.12.so mapped=26 mapmax=6 N0=24 N3=2 kernelpagesize_kB=4
    320621f000 default file=/lib64/ld-2.12.so anon=1 dirty=1 N3=1 kernelpagesize_kB=4
    3206220000 default file=/lib64/ld-2.12.so anon=1 dirty=1 N3=1 kernelpagesize_kB=4
    3206221000 default anon=1 dirty=1 N3=1 kernelpagesize_kB=4
    3206800000 default file=/lib64/libc-2.12.so mapped=59 mapmax=21 active=55 N0=41 N3=18 kernelpagesize_kB=4
    320698b000 default file=/lib64/libc-2.12.so
    3206b8a000 default file=/lib64/libc-2.12.so anon=2 dirty=2 N3=2 kernelpagesize_kB=4
    3206b8e000 default file=/lib64/libc-2.12.so anon=1 dirty=1 N3=1 kernelpagesize_kB=4
    3206b8f000 default anon=3 dirty=3 active=1 N3=3 kernelpagesize_kB=4
    7f4dc10a2000 default anon=3 dirty=3 N3=3 kernelpagesize_kB=4
    7f4dc10b4000 default anon=2 dirty=2 active=1 N3=2 kernelpagesize_kB=4
    7f4dc1200000 default file=/anon_hugepage\040(deleted) huge anon=1 dirty=1 N3=1 kernelpagesize_kB=2048
    7fff335f0000 default stack anon=3 dirty=3 N3=3 kernelpagesize_kB=4
    7fff3369d000 default mapped=1 mapmax=35 active=0 N3=1 kernelpagesize_kB=4

Where:
(其中：)

"address" is the starting address for the mapping;
("address" 是该映射的起始地址；)

"policy" reports the NUMA memory policy set for the mapping (see Documentation/admin-guide/mm/numa_memory_policy.rst);
("policy" 报告为该映射设置的 NUMA 内存策略(参见 Documentation/admin-guide/mm/numa_memory_policy.rst)；)

"mapping details" summarizes mapping data such as mapping type, page usage counters,
node locality page counters (N0 == node0, N1 == node1, ...) and the kernel page
size, in KB, that is backing the mapping up.
("mapping details" 汇总映射数据，例如映射类型、页使用计数器、节点本地性页计数器(N0 == node0，N1 == node1，……)以及支撑该映射的内核页大小(以 KB 计)。)

Note that some kernel configurations do not track the precise number of times
a page part of a larger allocation (e.g., THP) is mapped. In these
configurations, "mapmax" might corresponds to the average number of mappings
per page in such a larger allocation instead.
(注意，某些内核配置不会精确跟踪属于较大分配(例如 THP)的页被映射的次数。在这些配置中，"mapmax" 可能对应的是这种较大分配中每页的平均映射次数。)

1.2 Kernel data
(1.2 内核数据)
---------------

Similar to  the  process entries, the kernel data files give information about
the running kernel. The files used to obtain this information are contained in
/proc and  are  listed  in Table 1-5. Not all of these will be present in your
system. It  depends  on the kernel configuration and the loaded modules, which
files are there, and which are missing.
(与进程条目类似，内核数据文件给出关于正在运行的内核的信息。用于获取这些信息的文件包含在 /proc 中，并列于表 1-5。这些文件并非全部都会出现在你的系统中。哪些文件存在、哪些缺失，取决于内核配置和已加载的模块。)

.. table:: Table 1-5: Kernel info in /proc
          (表 1-5: /proc 中的内核信息)

 ============ ===============================================================
 File         Content (文件  内容)
 ============ ===============================================================
 allocinfo    Memory allocations profiling information (内存分配分析信息)
 apm          Advanced power management info (高级电源管理信息)
 bootconfig   Kernel command line obtained from boot config,
 	      and, if there were kernel parameters from the
	      boot loader, a "# Parameters from bootloader:"
	      line followed by a line containing those
	      parameters prefixed by "# ".			(5.5) (从 boot config 获取的内核命令行；如果引导加载程序曾提供内核参数，则还有一行 "# Parameters from bootloader:"，随后一行是以 "# " 为前缀的这些参数 (5.5))
 buddyinfo    Kernel memory allocator information (see text)	(2.5) (内核内存分配器信息(见正文) (2.5))
 bus          Directory containing bus specific information (包含总线特定信息的目录)
 cmdline      Kernel command line, both from bootloader and embedded
              in the kernel image (内核命令行，既来自引导加载程序，也内嵌于内核映像)
 cpuinfo      Info about the CPU (关于 CPU 的信息)
 devices      Available devices (block and character) (可用设备(块设备与字符设备))
 dma          Used DMA channels (已使用的 DMA 通道)
 filesystems  Supported filesystems (受支持的文件系统)
 driver       Various drivers grouped here, currently rtc	(2.4) (各种驱动程序归于此处，目前为 rtc (2.4))
 execdomains  Execdomains, related to security			(2.4) (执行域，与安全相关 (2.4))
 fb 	      Frame Buffer devices				(2.4) (帧缓冲(Frame Buffer)设备 (2.4))
 fs 	      File system parameters, currently nfs/exports	(2.4) (文件系统参数，目前为 nfs/exports (2.4))
 ide          Directory containing info about the IDE subsystem (包含 IDE 子系统信息的目录)
 interrupts   Interrupt usage (中断使用情况)
 iomem 	      Memory map					(2.4) (内存映射 (2.4))
 ioports      I/O port usage (I/O 端口使用情况)
 irq 	      Masks for irq to cpu affinity			(2.4)(smp?) (irq 到 CPU 亲和性的掩码 (2.4)(smp?))
 isapnp       ISA PnP (Plug&Play) Info				(2.4) (ISA PnP(即插即用)信息 (2.4))
 kcore        Kernel core image (can be ELF or A.OUT(deprecated in 2.4)) (内核核心映像(可以是 ELF 或 A.OUT(在 2.4 中已废弃)))
 kmsg         Kernel messages (内核消息)
 ksyms        Kernel symbol table (内核符号表)
 loadavg      Load average of last 1, 5 & 15 minutes;
                number of processes currently runnable (running or on ready queue);
                total number of processes in system;
                last pid created.
                All fields are separated by one space except "number of
                processes currently runnable" and "total number of processes
                in system", which are separated by a slash ('/'). Example:
                0.61 0.61 0.55 3/828 22084 (最近 1、5 和 15 分钟的平均负载；当前可运行的进程数(正在运行或位于就绪队列中)；系统内进程总数；最后创建的 pid。除"当前可运行进程数"与"系统内进程总数"之间以斜杠('/')分隔外，所有字段均以一个空格分隔。示例：0.61 0.61 0.55 3/828 22084)
 locks        Kernel locks (内核锁)
 meminfo      Memory info (内存信息)
 misc         Miscellaneous (杂项)
 modules      List of loaded modules (已加载模块的列表)
 mounts       Mounted filesystems (已挂载的文件系统)
 net          Networking info (see text) (网络信息(见正文))
 pagetypeinfo Additional page allocator information (see text)  (2.5) (额外的页分配器信息(见正文)  (2.5))
 partitions   Table of partitions known to the system (系统已知的分区表)
 pci 	      Deprecated info of PCI bus (new way -> /proc/bus/pci/,
              decoupled by lspci				(2.4) (PCI 总线的已废弃信息(新方式 -> /proc/bus/pci/，由 lspci 解耦 (2.4))
 rtc          Real time clock (实时时钟)
 scsi         SCSI info (see text) (SCSI 信息(见正文))
 slabinfo     Slab pool info (Slab 池信息)
 softirqs     softirq usage (软中断(softirq)使用情况)
 stat         Overall statistics (总体统计信息)
 swaps        Swap space utilization (交换空间利用情况)
 sys          See chapter 2 (见第 2 章)
 sysvipc      Info of SysVIPC Resources (msg, sem, shm)		(2.4) (SysVIPC 资源信息(msg、sem、shm) (2.4))
 tty 	      Info of tty drivers (tty 驱动信息)
 uptime       Wall clock since boot, combined idle time of all cpus (开机以来的墙上时钟时间，所有 cpu 的空闲时间总和)
 version      Kernel version (内核版本)
 video 	      bttv info of video resources			(2.4) (视频资源的 bttv 信息 (2.4))
 vmallocinfo  Show vmalloced areas (显示 vmalloc 分配的区域)
 ============ ===============================================================

You can,  for  example,  check  which interrupts are currently in use and what
they are used for by looking in the file /proc/interrupts::
(例如，你可以通过查看文件 /proc/interrupts 来检查当前有哪些中断正在使用以及它们的用途：)

  > cat /proc/interrupts
             CPU0
    0:    8728810          XT-PIC  timer
    1:        895          XT-PIC  keyboard
    2:          0          XT-PIC  cascade
    3:     531695          XT-PIC  aha152x
    4:    2014133          XT-PIC  serial
    5:      44401          XT-PIC  pcnet_cs
    8:          2          XT-PIC  rtc
   11:          8          XT-PIC  i82365
   12:     182918          XT-PIC  PS/2 Mouse
   13:          1          XT-PIC  fpu
   14:    1232265          XT-PIC  ide0
   15:          7          XT-PIC  ide1
  NMI:          0

In 2.4.* a couple of lines where added to this file LOC & ERR (this time is the
output of a SMP machine)::
(在 2.4.* 中，该文件增加了 LOC 和 ERR 两行(这次是一台 SMP 机器的输出)：)

  > cat /proc/interrupts

             CPU0       CPU1
    0:    1243498    1214548    IO-APIC-edge  timer
    1:       8949       8958    IO-APIC-edge  keyboard
    2:          0          0          XT-PIC  cascade
    5:      11286      10161    IO-APIC-edge  soundblaster
    8:          1          0    IO-APIC-edge  rtc
    9:      27422      27407    IO-APIC-edge  3c503
   12:     113645     113873    IO-APIC-edge  PS/2 Mouse
   13:          0          0          XT-PIC  fpu
   14:      22491      24012    IO-APIC-edge  ide0
   15:       2183       2415    IO-APIC-edge  ide1
   17:      30564      30414   IO-APIC-level  eth0
   18:        177        164   IO-APIC-level  bttv
  NMI:    2457961    2457959
  LOC:    2457882    2457881
  ERR:       2155

NMI is incremented in this case because every timer interrupt generates a NMI
(Non Maskable Interrupt) which is used by the NMI Watchdog to detect lockups.
(在这种情况下 NMI 会递增，因为每个时钟中断都会产生一个 NMI(不可屏蔽中断)，NMI 看门狗用它来检测锁定(lockup)。)

LOC is the local interrupt counter of the internal APIC of every CPU.
(LOC 是每个 CPU 内部 APIC 的本地中断计数器。)

ERR is incremented in the case of errors in the IO-APIC bus (the bus that
connects the CPUs in a SMP system. This means that an error has been detected,
the IO-APIC automatically retry the transmission, so it should not be a big
problem, but you should read the SMP-FAQ.
(在 IO-APIC 总线(SMP 系统中连接各 CPU 的总线)发生错误时 ERR 会递增。这意味着检测到了错误，IO-APIC 会自动重试传输，所以它应该不是什么大问题，但你应该阅读 SMP-FAQ。)

In 2.6.2* /proc/interrupts was expanded again.  This time the goal was for
/proc/interrupts to display every IRQ vector in use by the system, not
just those considered 'most important'.  The new vectors are:
(在 2.6.2* 中，/proc/interrupts 再次被扩展。这一次的目标是让 /proc/interrupts 显示系统正在使用的每个 IRQ 向量，而不仅仅是那些被认为"最重要"的。新增的向量有：)

THR
  interrupt raised when a machine check threshold counter
  (typically counting ECC corrected errors of memory or cache) exceeds
  a configurable threshold.  Only available on some systems.
  (当机器检查阈值计数器(通常统计内存或缓存的 ECC 纠正错误)超过可配置阈值时引发的中断。仅在某些系统上可用。)

TRM
  a thermal event interrupt occurs when a temperature threshold
  has been exceeded for the CPU.  This interrupt may also be generated
  when the temperature drops back to normal.
  (当 CPU 超过温度阈值时会发生热事件中断。当温度降回正常时也可能产生该中断。)

SPU
  a spurious interrupt is some interrupt that was raised then lowered
  by some IO device before it could be fully processed by the APIC.  Hence
  the APIC sees the interrupt but does not know what device it came from.
  For this case the APIC will generate the interrupt with a IRQ vector
  of 0xff. This might also be generated by chipset bugs.
  (虚假中断(spurious interrupt)是某个 IO 设备在 APIC 能够完整处理之前先拉高又拉低的某种中断。因此 APIC 看到了该中断，但不知道它来自哪个设备。对于这种情况，APIC 会以 0xff 的 IRQ 向量产生该中断。这也可能由芯片组缺陷引起。)

RES, CAL, TLB
  rescheduling, call and TLB flush interrupts are
  sent from one CPU to another per the needs of the OS.  Typically,
  their statistics are used by kernel developers and interested users to
  determine the occurrence of interrupts of the given type.
  (重新调度、调用函数和 TLB 刷新中断根据操作系统的需要从一个 CPU 发送到另一个 CPU。通常，内核开发者和感兴趣的用户使用它们的统计信息来确定给定类型中断的发生情况。)

The above IRQ vectors are displayed only when relevant.  For example,
the threshold vector does not exist on x86_64 platforms.  Others are
suppressed when the system is a uniprocessor.  As of this writing, only
i386 and x86_64 platforms support the new IRQ vector displays.
(上述 IRQ 向量仅在相关时才显示。例如，阈值向量在 x86_64 平台上不存在。当系统是单处理器时，其他一些向量也会被抑制。截至撰写本文时，只有 i386 和 x86_64 平台支持新的 IRQ 向量显示。)

Of some interest is the introduction of the /proc/irq directory to 2.4.
It could be used to set IRQ to CPU affinity. This means that you can "hook" an
IRQ to only one CPU, or to exclude a CPU of handling IRQs. The contents of the
irq subdir is one subdir for each IRQ, and default_smp_affinity.
(值得关注的是 2.4 中引入的 /proc/irq 目录。它可用于设置 IRQ 到 CPU 的亲和性。这意味着你可以把一个 IRQ"挂钩"到仅一个 CPU，或者排除某个 CPU 处理 IRQ。irq 子目录的内容是每个 IRQ 一个子目录，以及 default_smp_affinity。)

For example::
(例如：)

  > ls /proc/irq/
  0  10  12  14  16  18  2  4  6  8  default_smp_affinity
  1  11  13  15  17  19  3  5  7  9
  > ls /proc/irq/0/
  smp_affinity

smp_affinity is a bitmask, in which you can specify which CPUs can handle the
IRQ. You can set it by doing::
(smp_affinity 是一个位掩码，你可以在其中指定哪些 CPU 可以处理该 IRQ。你可以这样设置它：)

  > echo 1 > /proc/irq/10/smp_affinity

This means that only the first CPU will handle the IRQ, but you can also echo
5 which means that only the first and third CPU can handle the IRQ.
(这意味着只有第一个 CPU 会处理该 IRQ，但你也可以 echo 5，这意味着只有第一个和第三个 CPU 可以处理该 IRQ。)

The contents of each smp_affinity file is the same by default::
(每个 smp_affinity 文件的内容默认都是相同的：)

  > cat /proc/irq/0/smp_affinity
  ffffffff

There is an alternate interface, smp_affinity_list which allows specifying
a CPU range instead of a bitmask::
(还有一个替代接口 smp_affinity_list，它允许指定 CPU 范围而不是位掩码：)

  > cat /proc/irq/0/smp_affinity_list
  1024-1031

The default_smp_affinity mask applies to all non-active IRQs, which are the
IRQs which have not yet been allocated/activated, and hence which lack a
/proc/irq/[0-9]* directory.
(default_smp_affinity 掩码适用于所有非活动 IRQ，即尚未被分配/激活、因而缺少 /proc/irq/[0-9]* 目录的 IRQ。)

The node file on an SMP system shows the node to which the device using the IRQ
reports itself as being attached. This hardware locality information does not
include information about any possible driver locality preference.
(SMP 系统上的 node 文件显示使用该 IRQ 的设备自报所连接到的节点。这一硬件本地性信息不包含任何可能的驱动程序本地性偏好信息。)

The way IRQs are routed is handled by the IO-APIC, and it's Round Robin
between all the CPUs which are allowed to handle it. As usual the kernel has
more info than you and does a better job than you, so the defaults are the
best choice for almost everyone.  [Note this applies only to those IO-APIC's
that support "Round Robin" interrupt distribution.]

There are  three  more  important subdirectories in /proc: net, scsi, and sys.
The general  rule  is  that  the  contents,  or  even  the  existence of these
directories, depend  on your kernel configuration. If SCSI is not enabled, the
directory scsi  may  not  exist. The same is true with the net, which is there
only when networking support is present in the running kernel.
(/proc 中还有三个更重要的子目录：net、scsi 和 sys。一般规则是，这些目录的内容甚至其存在与否都取决于你的内核配置。如果未启用 SCSI，目录 scsi 可能不存在。net 也是如此，只有当正在运行的内核中存在网络支持时它才存在。)

The slabinfo  file  gives  information  about  memory usage at the slab level.
Linux uses  slab  pools for memory management above page level in version 2.2.
Commonly used  objects  have  their  own  slab  pool (such as network buffers,
directory cache, and so on).
(slabinfo 文件给出 slab 层面的内存使用信息。在 2.2 版本中，Linux 在页级别之上使用 slab 池进行内存管理。常用对象都有自己的 slab 池(例如网络缓冲、目录缓存等)。)

::

    > cat /proc/buddyinfo

    Node 0, zone      DMA      0      4      5      4      4      3 ...
    Node 0, zone   Normal      1      0      0      1    101      8 ...
    Node 0, zone  HighMem      2      0      0      1      1      0 ...

External fragmentation is a problem under some workloads, and buddyinfo is a
useful tool for helping diagnose these problems.  Buddyinfo will give you a
clue as to how big an area you can safely allocate, or why a previous
allocation failed.
(外部碎片在某些工作负载下是一个问题，而 buddyinfo 是帮助诊断这些问题的有用工具。Buddyinfo 会给你一个线索，告诉你你可以安全分配多大的区域，或者之前的分配为什么失败。)

Each column represents the number of pages of a certain order which are
available.  In this case, there are 0 chunks of 2^0*PAGE_SIZE available in
ZONE_DMA, 4 chunks of 2^1*PAGE_SIZE in ZONE_DMA, 101 chunks of 2^4*PAGE_SIZE
available in ZONE_NORMAL, etc...
(每一列表示某个阶(order)的可用页的数量。在此例中，ZONE_DMA 中有 0 个 2^0*PAGE_SIZE 大小的块，ZONE_DMA 中有 4 个 2^1*PAGE_SIZE 的块，ZONE_NORMAL 中有 101 个 2^4*PAGE_SIZE 的块，等等……)

More information relevant to external fragmentation can be found in
pagetypeinfo::
(与外部碎片相关的更多信息可在 pagetypeinfo 中找到：)

    > cat /proc/pagetypeinfo
    Page block order: 9
    Pages per block:  512

    Free pages count per migrate type at order       0      1      2      3      4      5      6      7      8      9     10
    Node    0, zone      DMA, type    Unmovable      0      0      0      1      1      1      1      1      1      1      0
    Node    0, zone      DMA, type  Reclaimable      0      0      0      0      0      0      0      0      0      0      0
    Node    0, zone      DMA, type      Movable      1      1      2      1      2      1      1      0      1      0      2
    Node    0, zone      DMA, type      Reserve      0      0      0      0      0      0      0      0      0      1      0
    Node    0, zone      DMA, type      Isolate      0      0      0      0      0      0      0      0      0      0      0
    Node    0, zone    DMA32, type    Unmovable    103     54     77      1      1      1     11      8      7      1      9
    Node    0, zone    DMA32, type  Reclaimable      0      0      2      1      0      0      0      0      1      0      0
    Node    0, zone    DMA32, type      Movable    169    152    113     91     77     54     39     13      6      1    452
    Node    0, zone    DMA32, type      Reserve      1      2      2      2      2      0      1      1      1      1      0
    Node    0, zone    DMA32, type      Isolate      0      0      0      0      0      0      0      0      0      0      0

    Number of blocks type     Unmovable  Reclaimable      Movable      Reserve      Isolate
    Node 0, zone      DMA            2            0            5            1            0
    Node 0, zone    DMA32           41            6          967            2            0

Fragmentation avoidance in the kernel works by grouping pages of different
migrate types into the same contiguous regions of memory called page blocks.
A page block is typically the size of the default hugepage size, e.g. 2MB on
X86-64. By keeping pages grouped based on their ability to move, the kernel
can reclaim pages within a page block to satisfy a high-order allocation.
(内核中的防碎片化机制通过将不同迁移类型的页分组到称为页块(page block)的相同连续内存区域来工作。一个页块的大小通常就是默认巨页的大小，例如在 X86-64 上为 2MB。通过根据页的移动能力将其分组，内核可以回收一个页块内的页以满足高阶分配。)

The pagetypinfo begins with information on the size of a page block. It
then gives the same type of information as buddyinfo except broken down
by migrate-type and finishes with details on how many page blocks of each
type exist.
(pagetypinfo 开头是关于页块大小的信息。随后它给出与 buddyinfo 相同类型的信息，只是按迁移类型进行了细分，最后详细说明每种类型的页块各有多少个。)

If min_free_kbytes has been tuned correctly (recommendations made by hugeadm
from libhugetlbfs https://github.com/libhugetlbfs/libhugetlbfs/), one can
make an estimate of the likely number of huge pages that can be allocated
at a given point in time. All the "Movable" blocks should be allocatable
unless memory has been mlock()'d. Some of the Reclaimable blocks should
also be allocatable although a lot of filesystem metadata may have to be
reclaimed to achieve this.
(如果 min_free_kbytes 已被正确调整(由 libhugetlbfs 的 hugeadm 给出建议，https://github.com/libhugetlbfs/libhugetlbfs/)，就可以估计在给定时间点可能可分配的巨页数量。除非内存已被 mlock()，所有 "Movable" 块都应该是可分配的。一些 Reclaimable 块也应该是可分配的，尽管可能必须回收大量文件系统元数据才能做到这一点。)


allocinfo
~~~~~~~~~

Provides information about memory allocations at all locations in the code
base. Each allocation in the code is identified by its source file, line
number, module (if originates from a loadable module) and the function calling
the allocation. The number of bytes allocated and number of calls at each
location are reported. The first line indicates the version of the file, the
second line is the header listing fields in the file.
(提供代码库中所有位置的内存分配信息。代码中的每个分配都通过其源文件、行号、模块(如果来源于可加载模块)以及调用该分配的函数来标识。报告每个位置分配的字节数和调用次数。第一行指示该文件的版本，第二行是列出文件中各字段的表头。)
If file version is 2.0 or higher then each line may contain additional
<key>:<value> pairs representing extra information about the call site.
For example if the counters are not accurate, the line will be appended with
"accurate:no" pair.
(如果文件版本为 2.0 或更高，则每一行可能包含表示调用点额外信息的附加 <key>:<value> 对。例如，如果计数器不准确，该行将附加 "accurate:no" 对。)

Supported markers in v2:
accurate:no
(v2 中支持的标记：
accurate:no)

              Absolute values of the counters in this line are not accurate
              because of the failure to allocate memory to track some of the
              allocations made at this location.  Deltas in these counters are
              accurate, therefore counters can be used to track allocation size
              and count changes.
              (此行中计数器的绝对值并不准确，因为未能分配内存来跟踪在此位置进行的一些分配。这些计数器的增量是准确的，因此计数器可用于跟踪分配大小和计数的变化。)

Example output.
(输出示例。)

::

    > tail -n +3 /proc/allocinfo | sort -rn
   127664128    31168 mm/page_ext.c:270 func:alloc_page_ext
    56373248     4737 mm/slub.c:2259 func:alloc_slab_page
    14880768     3633 mm/readahead.c:247 func:page_cache_ra_unbounded
    14417920     3520 mm/mm_init.c:2530 func:alloc_large_system_hash
    13377536      234 block/blk-mq.c:3421 func:blk_mq_alloc_rqs
    11718656     2861 mm/filemap.c:1919 func:__filemap_get_folio
     9192960     2800 kernel/fork.c:307 func:alloc_thread_stack_node
     4206592        4 net/netfilter/nf_conntrack_core.c:2567 func:nf_ct_alloc_hashtable
     4136960     1010 drivers/staging/ctagmod/ctagmod.c:20 [ctagmod] func:ctagmod_start
     3940352      962 mm/memory.c:4214 func:alloc_anon_folio
     2894464    22613 fs/kernfs/dir.c:615 func:__kernfs_new_node
     ...


meminfo
~~~~~~~

Provides information about distribution and utilization of memory.  This
varies by architecture and compile options.  Some of the counters reported
here overlap.  The memory reported by the non overlapping counters may not
add up to the overall memory usage and the difference for some workloads
can be substantial.  In many cases there are other means to find out
additional memory using subsystem specific interfaces, for instance
/proc/net/sockstat for TCP memory allocations.
(提供关于内存分布和使用情况的信息。这因架构和编译选项而异。这里报告的一些计数器相互重叠。非重叠计数器报告的内存加起来可能不等于总体内存使用量，对于某些工作负载，差异可能相当大。在许多情况下，还有其他方法可以借助子系统特定的接口来查明额外的内存，例如用于 TCP 内存分配的 /proc/net/sockstat。)

Example output. You may not have all of these fields.
(输出示例。你可能没有所有这些字段。)

::

    > cat /proc/meminfo

    MemTotal:       32858820 kB
    MemFree:        21001236 kB
    MemAvailable:   27214312 kB
    Buffers:          581092 kB
    Cached:          5587612 kB
    SwapCached:            0 kB
    Active:          3237152 kB
    Inactive:        7586256 kB
    Active(anon):      94064 kB
    Inactive(anon):  4570616 kB
    Active(file):    3143088 kB
    Inactive(file):  3015640 kB
    Unevictable:           0 kB
    Mlocked:               0 kB
    SwapTotal:             0 kB
    SwapFree:              0 kB
    Zswap:              1904 kB
    Zswapped:           7792 kB
    Dirty:                12 kB
    Writeback:             0 kB
    AnonPages:       4654780 kB
    Mapped:           266244 kB
    Shmem:              9976 kB
    KReclaimable:     517708 kB
    Slab:             660044 kB
    SReclaimable:     517708 kB
    SUnreclaim:       142336 kB
    KernelStack:       11168 kB
    PageTables:        20540 kB
    SecPageTables:         0 kB
    NFS_Unstable:          0 kB
    Bounce:                0 kB
    WritebackTmp:          0 kB
    CommitLimit:    16429408 kB
    Committed_AS:    7715148 kB
    VmallocTotal:   34359738367 kB
    VmallocUsed:       40444 kB
    VmallocChunk:          0 kB
    Percpu:            29312 kB
    EarlyMemtestBad:       0 kB
    HardwareCorrupted:     0 kB
    AnonHugePages:   4149248 kB
    ShmemHugePages:        0 kB
    ShmemPmdMapped:        0 kB
    FileHugePages:         0 kB
    FilePmdMapped:         0 kB
    CmaTotal:              0 kB
    CmaFree:               0 kB
    Unaccepted:            0 kB
    Balloon:               0 kB
    GPUActive:             0 kB
    GPUReclaim:            0 kB
    HugePages_Total:       0
    HugePages_Free:        0
    HugePages_Rsvd:        0
    HugePages_Surp:        0
    Hugepagesize:       2048 kB
    Hugetlb:               0 kB
    DirectMap4k:      401152 kB
    DirectMap2M:    10008576 kB
    DirectMap1G:    24117248 kB

MemTotal
              Total usable RAM (i.e. physical RAM minus a few reserved
              bits and the kernel binary code)
              (总可用 RAM(即物理 RAM 减去少量保留位和内核二进制代码))
MemFree
              Total free RAM. On highmem systems, the sum of LowFree+HighFree
              (总空闲 RAM。在高端内存(highmem)系统上为 LowFree+HighFree 之和)
MemAvailable
              An estimate of how much memory is available for starting new
              applications, without swapping. Calculated from MemFree,
              SReclaimable, the size of the file LRU lists, and the low
              watermarks in each zone.
              The estimate takes into account that the system needs some
              page cache to function well, and that not all reclaimable
              slab will be reclaimable, due to items being in use. The
              impact of those factors will vary from system to system.
              (对无需交换即可用于启动新应用程序的内存量的估计。由 MemFree、SReclaimable、文件 LRU 列表的大小以及每个 zone 的低水位计算得出。
              该估计考虑了系统需要一些页缓存才能良好运行，以及由于条目正在使用中，并非所有可回收 slab 都能被回收。这些因素的影响因系统而异。)
Buffers
              Relatively temporary storage for raw disk blocks
              shouldn't get tremendously large (20MB or so)
              (用于原始磁盘块的相对临时存储，不应变得非常大(20MB 左右))
Cached
              In-memory cache for files read from the disk (the
              pagecache) as well as tmpfs & shmem.
              Doesn't include SwapCached.
              (从磁盘读取的文件(页缓存)以及 tmpfs 和 shmem 的内存缓存。
              不包括 SwapCached。)
SwapCached
              Memory that once was swapped out, is swapped back in but
              still also is in the swapfile (if memory is needed it
              doesn't need to be swapped out AGAIN because it is already
              in the swapfile. This saves I/O)
              (曾经被换出、又被换入但仍在交换文件中的内存(如果需要内存，它不必再次换出，因为它已经在交换文件中。这节省了 I/O))
Active
              Memory that has been used more recently and usually not
              reclaimed unless absolutely necessary.
              (最近被使用过的内存，通常除非绝对必要，否则不会被回收。)
Inactive
              Memory which has been less recently used.  It is more
              eligible to be reclaimed for other purposes
              (最近较少被使用的内存。它更有资格被回收用于其他用途)
Unevictable
              Memory allocated for userspace which cannot be reclaimed, such
              as mlocked pages, ramfs backing pages, secret memfd pages etc.
              (为用户空间分配的、无法回收的内存，例如 mlocked 页、ramfs 支撑页、secret memfd 页等。)
Mlocked
              Memory locked with mlock().
              (用 mlock() 锁定的内存。)
HighTotal, HighFree
              Highmem is all memory above ~860MB of physical memory.
              Highmem areas are for use by userspace programs, or
              for the pagecache.  The kernel must use tricks to access
              this memory, making it slower to access than lowmem.
              (高端内存(highmem)是物理内存中约 860MB 以上的全部内存。
              高端内存区域供用户空间程序或页缓存使用。内核必须使用特殊技巧才能访问这些内存，使其访问速度比低端内存(lowmem)慢。)
LowTotal, LowFree
              Lowmem is memory which can be used for everything that
              highmem can be used for, but it is also available for the
              kernel's use for its own data structures.  Among many
              other things, it is where everything from the Slab is
              allocated.  Bad things happen when you're out of lowmem.
              (低端内存(lowmem)是既可用于高端内存所能用于的一切、也可供内核用于其自身数据结构的内存。其中，Slab 的一切分配都来自这里。低端内存耗尽时会发生糟糕的事情。)
SwapTotal
              total amount of swap space available (可用交换空间的总量)
SwapFree
              Memory which has been evicted from RAM, and is temporarily
              on the disk
              (已被从 RAM 中逐出、暂时位于磁盘上的内存)
Zswap
              Memory consumed by the zswap backend (compressed size)
              (zswap 后端消耗的内存(压缩后大小))
Zswapped
              Amount of anonymous memory stored in zswap (original size)
              (存储在 zswap 中的匿名内存量(原始大小))
Dirty
              Memory which is waiting to get written back to the disk
              (正在等待写回磁盘的内存)
Writeback
              Memory which is actively being written back to the disk
              (正在被主动写回磁盘的内存)
AnonPages
              Non-file backed pages mapped into userspace page tables. Note that
              some kernel configurations might consider all pages part of a
              larger allocation (e.g., THP) as "mapped", as soon as a single
              page is mapped.
              (映射到用户空间页表中的非文件支撑页。注意，某些内核配置可能会在单个页被映射时，就把属于较大分配(例如 THP)的所有页都视为"已映射"。)
Mapped
              files which have been mmapped, such as libraries. Note that some
              kernel configurations might consider all pages part of a larger
              allocation (e.g., THP) as "mapped", as soon as a single page is
              mapped.
              (已被 mmap 的文件，例如库。注意，某些内核配置可能会在单个页被映射时，就把属于较大分配(例如 THP)的所有页都视为"已映射"。)
Shmem
              Total memory used by shared memory (shmem) and tmpfs
              (共享内存(shmem)和 tmpfs 使用的总内存)
KReclaimable
              Kernel allocations that the kernel will attempt to reclaim
              under memory pressure. Includes SReclaimable (below), and other
              direct allocations with a shrinker.
              (内核在内存压力下将尝试回收的内核分配。包括 SReclaimable(见下文)以及其他带有 shrinker 的直接分配。)
Slab
              in-kernel data structures cache
              (内核内数据结构缓存)
SReclaimable
              Part of Slab, that might be reclaimed, such as caches
              (Slab 中可能被回收的部分，例如各种缓存)
SUnreclaim
              Part of Slab, that cannot be reclaimed on memory pressure
              (Slab 中在内存压力下无法回收的部分)
KernelStack
              Memory consumed by the kernel stacks of all tasks
              (所有任务的内核栈消耗的内存)
PageTables
              Memory consumed by userspace page tables
              (用户空间页表消耗的内存)
SecPageTables
              Memory consumed by secondary page tables, this currently includes
              KVM mmu and IOMMU allocations on x86 and arm64.
              (次级页表消耗的内存，目前包括 x86 和 arm64 上的 KVM mmu 和 IOMMU 分配。)
NFS_Unstable
              Always zero. Previously counted pages which had been written to
              the server, but has not been committed to stable storage.
              (始终为零。以前统计的是已写入服务器但尚未提交到稳定存储的页。)
Bounce
              Always zero. Previously memory used for block device
              "bounce buffers".
              (始终为零。以前是块设备"反弹缓冲(bounce buffers)"所用的内存。)
WritebackTmp
              Always zero. Previously memory used by FUSE for temporary
              writeback buffers.
              (始终为零。以前是 FUSE 用于临时回写缓冲的内存。)
CommitLimit
              Based on the overcommit ratio ('vm.overcommit_ratio'),
              this is the total amount of  memory currently available to
              be allocated on the system. This limit is only adhered to
              if strict overcommit accounting is enabled (mode 2 in
              'vm.overcommit_memory').
              (基于超额分配比率('vm.overcommit_ratio')，这是系统上当前可供分配的内存总量。只有启用了严格超额分配统计('vm.overcommit_memory' 中的模式 2)时，才会遵守此限制。)

              The CommitLimit is calculated with the following formula::
              (CommitLimit 按以下公式计算：)

                CommitLimit = ([total RAM pages] - [total huge TLB pages]) *
                               overcommit_ratio / 100 + [total swap pages]

              For example, on a system with 1G of physical RAM and 7G
              of swap with a `vm.overcommit_ratio` of 30 it would
              yield a CommitLimit of 7.3G.
              (例如，在一台拥有 1G 物理 RAM 和 7G 交换空间、`vm.overcommit_ratio` 为 30 的系统上，得到的 CommitLimit 为 7.3G。)

              For more details, see the memory overcommit documentation
              in mm/overcommit-accounting.
              (更多细节请参见 mm/overcommit-accounting 中的内存超额分配文档。)
Committed_AS
              The amount of memory presently allocated on the system.
              The committed memory is a sum of all of the memory which
              has been allocated by processes, even if it has not been
              "used" by them as of yet. A process which malloc()'s 1G
              of memory, but only touches 300M of it will show up as
              using 1G. This 1G is memory which has been "committed" to
              by the VM and can be used at any time by the allocating
              application. With strict overcommit enabled on the system
              (mode 2 in 'vm.overcommit_memory'), allocations which would
              exceed the CommitLimit (detailed above) will not be permitted.
              This is useful if one needs to guarantee that processes will
              not fail due to lack of memory once that memory has been
              successfully allocated.
              (系统当前已分配的内存量。已提交(committed)内存是进程已分配的全部内存之和，即使这些内存尚未被它们"使用"。一个 malloc() 了 1G 内存、但只实际使用其中 300M 的进程会显示为使用 1G。这 1G 是虚拟内存子系统(VM)已"承诺"的内存，分配它的应用程序可以随时使用。在系统上启用严格超额分配('vm.overcommit_memory' 中的模式 2)时，将不允许超过 CommitLimit(上文详述)的分配。如果需要保证进程在内存成功分配之后不会因内存不足而失败，这会很有用。)
VmallocTotal
              total size of vmalloc virtual address space
              (vmalloc 虚拟地址空间的总大小)
VmallocUsed
              amount of vmalloc area which is used
              (已使用的 vmalloc 区域大小)
VmallocChunk
              largest contiguous block of vmalloc area which is free
              (空闲的最大连续 vmalloc 区域块)
Percpu
              Memory allocated to the percpu allocator used to back percpu
              allocations. This stat excludes the cost of metadata.
              (分配给 percpu 分配器、用于支撑 percpu 分配的内存。此统计不包括元数据的开销。)
EarlyMemtestBad
              The amount of RAM/memory in kB, that was identified as corrupted
              by early memtest. If memtest was not run, this field will not
              be displayed at all. Size is never rounded down to 0 kB.
              That means if 0 kB is reported, you can safely assume
              there was at least one pass of memtest and none of the passes
              found a single faulty byte of RAM.
              (早期内存测试(early memtest)识别为已损坏的 RAM/内存量，以 kB 计。如果未运行 memtest，此字段将完全不显示。大小从不向下取整到 0 kB。这意味着如果报告为 0 kB，你可以放心地认为至少运行了一轮 memtest，并且没有任何一轮发现哪怕一个有故障的 RAM 字节。)
HardwareCorrupted
              The amount of RAM/memory in KB, the kernel identifies as
              corrupted.
              (内核识别为已损坏的 RAM/内存量，以 KB 计。)
AnonHugePages
              Non-file backed huge pages mapped into userspace page tables
              (映射到用户空间页表中的非文件支撑巨页)
ShmemHugePages
              Memory used by shared memory (shmem) and tmpfs allocated
              with huge pages
              (以巨页分配的共享内存(shmem)和 tmpfs 所使用的内存)
ShmemPmdMapped
              Shared memory mapped into userspace with huge pages
              (以巨页映射到用户空间的共享内存)
FileHugePages
              Memory used for filesystem data (page cache) allocated
              with huge pages
              (以巨页分配的、用于文件系统数据(页缓存)的内存)
FilePmdMapped
              Page cache mapped into userspace with huge pages
              (以巨页映射到用户空间的页缓存)
CmaTotal
              Memory reserved for the Contiguous Memory Allocator (CMA)
              (为连续内存分配器(CMA)保留的内存)
CmaFree
              Free remaining memory in the CMA reserves
              (CMA 保留区中剩余的空闲内存)
Unaccepted
              Memory that has not been accepted by the guest
              (尚未被客户机(guest)接受(accept)的内存)
Balloon
              Memory returned to Host by VM Balloon Drivers
              (由虚拟机气球(balloon)驱动程序返还给宿主机的内存)
GPUActive
              System memory allocated to active GPU objects
              (分配给活动 GPU 对象的系统内存)
GPUReclaim
              System memory stored in GPU pools for reuse. This memory is not
              counted in GPUActive. It is shrinker reclaimable memory kept in a reuse
              pool because it has non-standard page table attributes, like WC or UC.
              (存储在 GPU 池中以便重用的系统内存。此内存不计入 GPUActive。它是保存在重用池中的、可由 shrinker 回收的内存，因为它具有非标准的页表属性，例如 WC 或 UC。)
HugePages_Total, HugePages_Free, HugePages_Rsvd, HugePages_Surp, Hugepagesize, Hugetlb
              See Documentation/admin-guide/mm/hugetlbpage.rst.
              (参见 Documentation/admin-guide/mm/hugetlbpage.rst。)
DirectMap4k, DirectMap2M, DirectMap1G
              Breakdown of page table sizes used in the kernel's
              identity mapping of RAM
              (内核对 RAM 的恒等映射所使用页表大小的细分)

vmallocinfo
~~~~~~~~~~~

Provides information about vmalloced/vmaped areas. One line per area,
containing the virtual address range of the area, size in bytes,
caller information of the creator, and optional information depending
on the kind of area:
(提供关于 vmalloc/vmap 分配区域的信息。每个区域一行，包含该区域的虚拟地址范围、以字节计的大小、创建者的调用者信息，以及取决于区域类别的可选信息：)

 ==========  ===================================================
 pages=nr    number of pages (页数)
 phys=addr   if a physical address was specified (如果指定了物理地址)
 ioremap     I/O mapping (ioremap() and friends) (I/O 映射(ioremap() 等))
 vmalloc     vmalloc() area (vmalloc() 区域)
 vmap        vmap()ed pages (经 vmap() 映射的页)
 user        VM_USERMAP area (VM_USERMAP 区域)
 vpages      buffer for pages pointers was vmalloced (huge area) (页指针缓冲区由 vmalloc 分配(巨大区域))
 N<node>=nr  (Only on NUMA kernels)
             Number of pages allocated on memory node <node> (仅在 NUMA 内核上) 在内存节点 <node> 上分配的页数
 ==========  ===================================================

::

    > cat /proc/vmallocinfo
    0xffffc20000000000-0xffffc20000201000 2101248 alloc_large_system_hash+0x204 ...
    /0x2c0 pages=512 vmalloc N0=128 N1=128 N2=128 N3=128
    0xffffc20000201000-0xffffc20000302000 1052672 alloc_large_system_hash+0x204 ...
    /0x2c0 pages=256 vmalloc N0=64 N1=64 N2=64 N3=64
    0xffffc20000302000-0xffffc20000304000    8192 acpi_tb_verify_table+0x21/0x4f...
    phys=7fee8000 ioremap
    0xffffc20000304000-0xffffc20000307000   12288 acpi_tb_verify_table+0x21/0x4f...
    phys=7fee7000 ioremap
    0xffffc2000031d000-0xffffc2000031f000    8192 init_vdso_vars+0x112/0x210
    0xffffc2000031f000-0xffffc2000032b000   49152 cramfs_uncompress_init+0x2e ...
    /0x80 pages=11 vmalloc N0=3 N1=3 N2=2 N3=3
    0xffffc2000033a000-0xffffc2000033d000   12288 sys_swapon+0x640/0xac0      ...
    pages=2 vmalloc N1=2
    0xffffc20000347000-0xffffc2000034c000   20480 xt_alloc_table_info+0xfe ...
    /0x130 [x_tables] pages=4 vmalloc N0=4
    0xffffffffa0000000-0xffffffffa000f000   61440 sys_init_module+0xc27/0x1d00 ...
    pages=14 vmalloc N2=14
    0xffffffffa000f000-0xffffffffa0014000   20480 sys_init_module+0xc27/0x1d00 ...
    pages=4 vmalloc N1=4
    0xffffffffa0014000-0xffffffffa0017000   12288 sys_init_module+0xc27/0x1d00 ...
    pages=2 vmalloc N1=2
    0xffffffffa0017000-0xffffffffa0022000   45056 sys_init_module+0xc27/0x1d00 ...
    pages=10 vmalloc N0=10


softirqs
~~~~~~~~

Provides counts of softirq handlers serviced since boot time, for each CPU.
(提供自系统启动以来每个 CPU 处理的软中断(softirq)处理程序的计数。)

::

    > cat /proc/softirqs
		  CPU0       CPU1       CPU2       CPU3
	HI:          0          0          0          0
    TIMER:       27166      27120      27097      27034
    NET_TX:          0          0          0         17
    NET_RX:         42          0          0         39
    BLOCK:           0          0        107       1121
    TASKLET:         0          0          0        290
    SCHED:       27035      26983      26971      26746
    HRTIMER:         0          0          0          0
	RCU:      1678       1769       2178       2250

1.3 Networking info in /proc/net
(1.3 /proc/net 中的网络信息)
--------------------------------

The subdirectory  /proc/net  follows  the  usual  pattern. Table 1-8 shows the
additional values  you  get  for  IP  version 6 if you configure the kernel to
support this. Table 1-9 lists the files and their meaning.
(子目录 /proc/net 遵循通常的模式。如果你将内核配置为支持 IP 版本 6，表 1-8 显示你会得到的额外条目。表 1-9 列出了这些文件及其含义。)


.. table:: Table 1-8: IPv6 info in /proc/net
          (表 1-8: /proc/net 中的 IPv6 信息)

 ========== =====================================================
 File       Content (文件  内容)
 ========== =====================================================
 udp6       UDP sockets (IPv6) (UDP 套接字(IPv6))
 tcp6       TCP sockets (IPv6) (TCP 套接字(IPv6))
 raw6       Raw device statistics (IPv6) (原始设备统计信息(IPv6))
 igmp6      IP multicast addresses, which this host joined (IPv6) (此主机已加入的 IP 多播地址(IPv6))
 if_inet6   List of IPv6 interface addresses (IPv6 接口地址列表)
 ipv6_route Kernel routing table for IPv6 (IPv6 的内核路由表)
 rt6_stats  Global IPv6 routing tables statistics (全局 IPv6 路由表统计信息)
 sockstat6  Socket statistics (IPv6) (套接字统计信息(IPv6))
 snmp6      Snmp data (IPv6) (Snmp 数据(IPv6))
 ========== =====================================================

.. table:: Table 1-9: Network info in /proc/net
          (表 1-9: /proc/net 中的网络信息)

 ============= ================================================================
 File          Content (文件  内容)
 ============= ================================================================
 arp           Kernel  ARP table (内核 ARP 表)
 dev           network devices with statistics (带统计信息的网络设备)
 dev_mcast     the Layer2 multicast groups a device is listening too
               (interface index, label, number of references, number of bound
               addresses). (设备正在监听的二层多播组(接口索引、标签、引用数、已绑定地址数)。)
 dev_stat      network device status (网络设备状态)
 ip_fwchains   Firewall chain linkage (防火墙链链接)
 ip_fwnames    Firewall chain names (防火墙链名称)
 ip_masq       Directory containing the masquerading tables (包含伪装(masquerading)表的目录)
 ip_masquerade Major masquerading table (主要伪装表)
 netstat       Network statistics (网络统计信息)
 raw           raw device statistics (原始设备统计信息)
 route         Kernel routing table (内核路由表)
 rpc           Directory containing rpc info (包含 rpc 信息的目录)
 rt_cache      Routing cache (路由缓存)
 snmp          SNMP data (SNMP 数据)
 sockstat      Socket statistics (套接字统计信息)
 softnet_stat  Per-CPU incoming packets queues statistics of online CPUs (在线 CPU 的每 CPU 入站数据包队列统计信息)
 tcp           TCP  sockets (TCP 套接字)
 udp           UDP sockets (UDP 套接字)
 unix          UNIX domain sockets (UNIX 域套接字)
 wireless      Wireless interface data (Wavelan etc) (无线接口数据(Wavelan 等))
 igmp          IP multicast addresses, which this host joined (此主机已加入的 IP 多播地址)
 psched        Global packet scheduler parameters. (全局包调度器参数。)
 netlink       List of PF_NETLINK sockets (PF_NETLINK 套接字列表)
 ip_mr_vifs    List of multicast virtual interfaces (多播虚拟接口列表)
 ip_mr_cache   List of multicast routing cache (多播路由缓存列表)
 ============= ================================================================

You can  use  this  information  to see which network devices are available in
your system and how much traffic was routed over those devices::
(你可以使用这些信息查看系统中哪些网络设备可用，以及有多少流量经由这些设备路由：)

  > cat /proc/net/dev
  Inter-|Receive                                                   |[...
   face |bytes    packets errs drop fifo frame compressed multicast|[...
      lo:  908188   5596     0    0    0     0          0         0 [...
    ppp0:15475140  20721   410    0    0   410          0         0 [...
    eth0:  614530   7085     0    0    0     0          0         1 [...

  ...] Transmit
  ...] bytes    packets errs drop fifo colls carrier compressed
  ...]  908188     5596    0    0    0     0       0          0
  ...] 1375103    17405    0    0    0     0       0          0
  ...] 1703981     5535    0    0    0     3       0          0

In addition, each Channel Bond interface has its own directory.  For
example, the bond0 device will have a directory called /proc/net/bond0/.
It will contain information that is specific to that bond, such as the
current slaves of the bond, the link status of the slaves, and how
many times the slaves link has failed.
(此外，每个 Channel Bond(绑定)接口都有自己的目录。例如，bond0 设备会有一个名为 /proc/net/bond0/ 的目录。它将包含特定于该绑定的信息，例如该绑定当前的 slave(从属接口)、各 slave 的链路状态，以及 slave 链路失败的次数。)

1.4 SCSI info
(1.4 SCSI 信息)
-------------

If you have a SCSI or ATA host adapter in your system, you'll find a
subdirectory named after the driver for this adapter in /proc/scsi.
You'll also see a list of all recognized SCSI devices in /proc/scsi::
(如果你的系统中装有 SCSI 或 ATA 主机适配器，你会在 /proc/scsi 中找到一个以该适配器的驱动程序命名的子目录。你还会在 /proc/scsi 中看到所有已识别 SCSI 设备的列表：)

  >cat /proc/scsi/scsi
  Attached devices:
  Host: scsi0 Channel: 00 Id: 00 Lun: 00
    Vendor: IBM      Model: DGHS09U          Rev: 03E0
    Type:   Direct-Access                    ANSI SCSI revision: 03
  Host: scsi0 Channel: 00 Id: 06 Lun: 00
    Vendor: PIONEER  Model: CD-ROM DR-U06S   Rev: 1.04
    Type:   CD-ROM                           ANSI SCSI revision: 02


The directory  named  after  the driver has one file for each adapter found in
the system.  These  files  contain information about the controller, including
the used  IRQ  and  the  IO  address range. The amount of information shown is
dependent on  the adapter you use. The example shows the output for an Adaptec
AHA-2940 SCSI adapter::
(以该驱动程序命名的目录中，系统里找到的每个适配器都有一个对应的文件。这些文件包含关于控制器的信息，包括所使用的 IRQ 和 IO 地址范围。所显示信息的多少取决于你使用的适配器。示例显示的是 Adaptec AHA-2940 SCSI 适配器的输出：)

  > cat /proc/scsi/aic7xxx/0

  Adaptec AIC7xxx driver version: 5.1.19/3.2.4
  Compile Options:
    TCQ Enabled By Default : Disabled
    AIC7XXX_PROC_STATS     : Disabled
    AIC7XXX_RESET_DELAY    : 5
  Adapter Configuration:
             SCSI Adapter: Adaptec AHA-294X Ultra SCSI host adapter
                             Ultra Wide Controller
      PCI MMAPed I/O Base: 0xeb001000
   Adapter SEEPROM Config: SEEPROM found and used.
        Adaptec SCSI BIOS: Enabled
                      IRQ: 10
                     SCBs: Active 0, Max Active 2,
                           Allocated 15, HW 16, Page 255
               Interrupts: 160328
        BIOS Control Word: 0x18b6
     Adapter Control Word: 0x005b
     Extended Translation: Enabled
  Disconnect Enable Flags: 0xffff
       Ultra Enable Flags: 0x0001
   Tag Queue Enable Flags: 0x0000
  Ordered Queue Tag Flags: 0x0000
  Default Tag Queue Depth: 8
      Tagged Queue By Device array for aic7xxx host instance 0:
        {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}
      Actual queue depth per device for aic7xxx host instance 0:
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
  Statistics:
  (scsi0:0:0:0)
    Device using Wide/Sync transfers at 40.0 MByte/sec, offset 8
    Transinfo settings: current(12/8/1/0), goal(12/8/1/0), user(12/15/1/0)
    Total transfers 160151 (74577 reads and 85574 writes)
  (scsi0:0:6:0)
    Device using Narrow/Sync transfers at 5.0 MByte/sec, offset 15
    Transinfo settings: current(50/15/0/0), goal(50/15/0/0), user(50/15/0/0)
    Total transfers 0 (0 reads and 0 writes)


1.5 Parallel port info in /proc/parport
(1.5 /proc/parport 中的并行端口信息)
---------------------------------------

The directory  /proc/parport  contains information about the parallel ports of
your system.  It  has  one  subdirectory  for  each port, named after the port
number (0,1,2,...).
(目录 /proc/parport 包含你系统并行端口的信息。它为每个端口准备了一个子目录，以端口号(0,1,2,...)命名。)

These directories contain the four files shown in Table 1-10.
(这些目录包含表 1-10 所示的四个文件。)


.. table:: Table 1-10: Files in /proc/parport
          (表 1-10: /proc/parport 中的文件)

 ========= ====================================================================
 File      Content (文件  内容)
 ========= ====================================================================
 autoprobe Any IEEE-1284 device ID information that has been acquired. (已获取的任何 IEEE-1284 设备 ID 信息。)
 devices   list of the device drivers using that port. A + will appear by the
           name of the device currently using the port (it might not appear
           against any). (正在使用该端口的设备驱动程序列表。当前正在使用该端口的设备名称旁会显示一个 +(也可能一个都不显示)。)
 hardware  Parallel port's base address, IRQ line and DMA channel. (并行端口的基地址、IRQ 线和 DMA 通道。)
 irq       IRQ that parport is using for that port. This is in a separate
           file to allow you to alter it by writing a new value in (IRQ
           number or none). (parport 用于该端口的 IRQ。它位于单独的文件中，以便你可以通过写入新值(IRQ 号或 none)来修改它。)
 ========= ====================================================================

1.6 TTY info in /proc/tty
(1.6 /proc/tty 中的 TTY 信息)
-------------------------

Information about  the  available  and actually used tty's can be found in the
directory /proc/tty. You'll find  entries  for drivers and line disciplines in
this directory, as shown in Table 1-11.
(关于可用及实际使用的 tty 的信息可以在目录 /proc/tty 中找到。你会在该目录中找到驱动程序和线路规程(line discipline)的条目，如表 1-11 所示。)


.. table:: Table 1-11: Files in /proc/tty
          (表 1-11: /proc/tty 中的文件)

 ============= ==============================================
 File          Content (文件  内容)
 ============= ==============================================
 drivers       list of drivers and their usage (驱动程序列表及其用途)
 ldiscs        registered line disciplines (已注册的线路规程)
 driver/serial usage statistic and status of single tty lines (单个 tty 线路的使用统计和状态)
 ============= ==============================================

To see  which  tty's  are  currently in use, you can simply look into the file
/proc/tty/drivers::
(要查看当前哪些 tty 正在使用，你只需查看文件 /proc/tty/drivers：)

  > cat /proc/tty/drivers
  pty_slave            /dev/pts      136   0-255 pty:slave
  pty_master           /dev/ptm      128   0-255 pty:master
  pty_slave            /dev/ttyp       3   0-255 pty:slave
  pty_master           /dev/pty        2   0-255 pty:master
  serial               /dev/cua        5   64-67 serial:callout
  serial               /dev/ttyS       4   64-67 serial
  /dev/tty0            /dev/tty0       4       0 system:vtmaster
  /dev/ptmx            /dev/ptmx       5       2 system
  /dev/console         /dev/console    5       1 system:console
  /dev/tty             /dev/tty        5       0 system:/dev/tty
  unknown              /dev/tty        4    1-63 console


1.7 Miscellaneous kernel statistics in /proc/stat
(1.7 /proc/stat 中的杂项内核统计信息)
-------------------------------------------------

Various pieces   of  information about  kernel activity  are  available in the
/proc/stat file.  All  of  the numbers reported  in  this file are  aggregates
since the system first booted.  For a quick look, simply cat the file::
(/proc/stat 文件中提供了关于内核活动的各类信息。该文件中报告的所有数字都是自系统首次启动以来的累计值。要快速浏览，直接 cat 该文件即可：)

  > cat /proc/stat
  cpu  237902850 368826709 106375398 1873517540 1135548 0 14507935 0 0 0
  cpu0 60045249 91891769 26331539 468411416 495718 0 5739640 0 0 0
  cpu1 59746288 91759249 26609887 468860630 312281 0 4384817 0 0 0
  cpu2 59489247 92985423 26904446 467808813 171668 0 2268998 0 0 0
  cpu3 58622065 92190267 26529524 468436680 155879 0 2114478 0 0 0
  intr 8688370575 8 3373 0 0 0 0 0 0 1 40791 0 0 353317 0 0 0 0 224789828 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 190974333 41958554 123983334 43 0 224593 0 0 0 <more 0's deleted>
  ctxt 22848221062
  btime 1605316999
  processes 746787147
  procs_running 2
  procs_blocked 0
  softirq 12121874454 100099120 3938138295 127375644 2795979 187870761 0 173808342 3072582055 52608 224184354

The very first  "cpu" line aggregates the  numbers in all  of the other "cpuN"
lines.  These numbers identify the amount of time the CPU has spent performing
different kinds of work.  Time units are in USER_HZ (typically hundredths of a
second).  The meanings of the columns are as follows, from left to right:
(最前面的 "cpu" 行汇总了所有其他 "cpuN" 行中的数字。这些数字标识 CPU 花费在不同种类工作上的时间量。时间单位为 USER_HZ(通常为百分之一秒)。各列的含义从左到右如下：)

- user: normal processes executing in user mode
  (user：在用户态执行的普通进程)
- nice: niced processes executing in user mode
  (nice：在用户态执行的、已调整优先级(niced)的进程)
- system: processes executing in kernel mode
  (system：在内核态执行的进程)
- idle: twiddling thumbs
  (idle：闲着无事可做)
- iowait: In a word, iowait stands for waiting for I/O to complete. But there
  are several problems:
  (iowait：简而言之，iowait 表示等待 I/O 完成。但存在几个问题：)

  1. CPU will not wait for I/O to complete, iowait is the time that a task is
     waiting for I/O to complete. When CPU goes into idle state for
     outstanding task I/O, another task will be scheduled on this CPU.
     (CPU 不会等待 I/O 完成，iowait 是任务等待 I/O 完成的时间。当 CPU 因未完成的任务 I/O 而进入空闲状态时，另一个任务会被调度到该 CPU 上。)
  2. In a multi-core CPU, the task waiting for I/O to complete is not running
     on any CPU, so the iowait of each CPU is difficult to calculate.
     (在多核 CPU 中，等待 I/O 完成的任务并没有运行在任何 CPU 上，因此每个 CPU 的 iowait 都难以计算。)
  3. The value of iowait field in /proc/stat will decrease in certain
     conditions.
     (在某些条件下，/proc/stat 中 iowait 字段的值会减少。)

  So, the iowait is not reliable by reading from /proc/stat.
  (因此，从 /proc/stat 读到的 iowait 并不可靠。)
- irq: servicing interrupts
  (irq：处理中断)
- softirq: servicing softirqs
  (softirq：处理软中断)
- steal: involuntary wait
  (steal：非自愿等待)
- guest: running a normal guest
  (guest：运行普通客户机)
- guest_nice: running a niced guest
  (guest_nice：运行已调整优先级(niced)的客户机)

The "intr" line gives counts of interrupts  serviced since boot time, for each
of the  possible system interrupts.   The first  column  is the  total of  all
interrupts serviced  including  unnumbered  architecture specific  interrupts;
each  subsequent column is the  total for that particular numbered interrupt.
Unnumbered interrupts are not shown, only summed into the total.
("intr" 行给出自启动以来处理的各类可能系统中断的计数。第一列是处理的全部中断的总数，包括未编号的架构特定中断；其后的每一列是该特定编号中断的总数。未编号的中断不会单独显示，只计入总数。)

The "ctxt" line gives the total number of context switches across all CPUs.
("ctxt" 行给出所有 CPU 的上下文切换总次数。)

The "btime" line gives  the time at which the  system booted, in seconds since
the Unix epoch.
("btime" 行给出系统启动的时间，以自 Unix 纪元以来的秒数表示。)

The "processes" line gives the number  of processes and threads created, which
includes (but  is not limited  to) those  created by  calls to the  fork() and
clone() system calls.
("processes" 行给出已创建的进程和线程的数量，包括(但不限于)通过调用 fork() 和 clone() 系统调用创建的那些。)

The "procs_running" line gives the total number of threads that are
running or ready to run (i.e., the total number of runnable threads).
("procs_running" 行给出正在运行或准备运行的线程总数(即可运行线程的总数)。)

The   "procs_blocked" line gives  the  number of  processes currently blocked,
waiting for I/O to complete.
("procs_blocked" 行给出当前被阻塞、正在等待 I/O 完成的进程数。)

The "softirq" line gives counts of softirqs serviced since boot time, for each
of the possible system softirqs. The first column is the total of all
softirqs serviced; each subsequent column is the total for that particular
softirq.
("softirq" 行给出自启动以来处理的各类可能系统软中断的计数。第一列是处理的全部软中断的总数；其后的每一列是该特定软中断的总数。)


1.8 Ext4 file system parameters
(1.8 Ext4 文件系统参数)
-------------------------------

Information about mounted ext4 file systems can be found in
/proc/fs/ext4.  Each mounted filesystem will have a directory in
/proc/fs/ext4 based on its device name (i.e., /proc/fs/ext4/hdc or
/proc/fs/ext4/sda9 or /proc/fs/ext4/dm-0).   The files in each per-device
directory are shown in Table 1-12, below.
(关于已挂载 ext4 文件系统的信息可以在 /proc/fs/ext4 中找到。每个已挂载的文件系统都会在 /proc/fs/ext4 中有一个以其设备名命名的目录(即 /proc/fs/ext4/hdc 或 /proc/fs/ext4/sda9 或 /proc/fs/ext4/dm-0)。每个设备目录中的文件如下面的表 1-12 所示。)

.. table:: Table 1-12: Files in /proc/fs/ext4/<devname>
          (表 1-12: /proc/fs/ext4/<devname> 中的文件)

 ==============  ==========================================================
 File            Content
 mb_groups       details of multiblock allocator buddy cache of free blocks (空闲块多块分配器伙伴缓存(buddy cache)的详细信息)
 ==============  ==========================================================

1.9 /proc/consoles
(1.9 /proc/consoles)
-------------------
Shows registered system console lines.
(显示已注册的系统控制台行。)

To see which character device lines are currently used for the system console
/dev/console, you may simply look into the file /proc/consoles::
(要查看当前哪些字符设备行被用作系统控制台 /dev/console，你只需查看文件 /proc/consoles：)

  > cat /proc/consoles
  tty0                 -WU (ECp)       4:7
  ttyS0                -W- (Ep)        4:64

The columns are:
(各列含义如下：)

+--------------------+-------------------------------------------------------+
| device             | name of the device (设备名称)                         |
+====================+=======================================================+
| operations         | * R = can do read operations (可进行读操作)           |
|                    | * W = can do write operations (可进行写操作)          |
|                    | * U = can do unblank (可执行 unblank 操作)            |
+--------------------+-------------------------------------------------------+
| flags              | * E = it is enabled (它已启用)                        |
|                    | * C = it is preferred console (它是首选控制台)        |
|                    | * B = it is primary boot console (它是主引导控制台)   |
|                    | * p = it is used for printk buffer                    |
|                    |                (它用于 printk 缓冲)                   |
|                    | * b = it is not a TTY but a Braille device            |
|                    |                (它不是 TTY 而是盲文设备)              |
|                    | * a = it is safe to use when cpu is offline           |
|                    |           (cpu 离线时使用它是安全的)                  |
+--------------------+-------------------------------------------------------+
| major:minor        | major and minor number of the device separated by a   |
|                    | colon (设备的主设备号和次设备号，以冒号分隔)          |
+--------------------+-------------------------------------------------------+

Summary
(小结)
-------

The /proc file system serves information about the running system. It not only
allows access to process data but also allows you to request the kernel status
by reading files in the hierarchy.
(/proc 文件系统提供关于正在运行的系统的信息。它不仅允许访问进程数据，还允许你通过读取该层次结构中的文件来查询内核状态。)

The directory  structure  of /proc reflects the types of information and makes
it easy, if not obvious, where to look for specific data.
(/proc 的目录结构反映了信息的类型，即使不能让人一目了然，也能让人容易知道去哪里查找特定的数据。)

Chapter 2: Modifying System Parameters
(第 2 章：修改系统参数)
======================================

In This Chapter
(本章内容)
---------------

* Modifying kernel parameters by writing into files found in /proc/sys
  (通过写入 /proc/sys 下的文件来修改内核参数)
* Exploring the files which modify certain parameters
  (探究修改某些参数的那些文件)
* Review of the /proc/sys file tree
  (概览 /proc/sys 文件树)

------------------------------------------------------------------------------

A very  interesting part of /proc is the directory /proc/sys. This is not only
a source  of  information,  it also allows you to change parameters within the
kernel. Be  very  careful  when attempting this. You can optimize your system,
but you  can  also  cause  it  to  crash.  Never  alter kernel parameters on a
production system.  Set  up  a  development machine and test to make sure that
everything works  the  way  you want it to. You may have no alternative but to
reboot the machine once an error has been made.
(/proc 中非常有趣的一部分是目录 /proc/sys。它不仅是信息的来源，还允许你更改内核中的参数。尝试这样做时要非常小心。你可以优化你的系统，但也可能使它崩溃。绝不要在生产系统上更改内核参数。请搭建一台开发机并进行测试，确保一切都按你想要的方式工作。一旦出错，你可能除了重启机器之外别无选择。)

To change  a  value,  simply  echo  the new value into the file.
You need to be root to do this. You  can  create  your  own  boot script
to perform this every time your system boots.
(要更改一个值，只需将新值 echo 到文件中即可。执行此操作你需要是 root。你可以创建自己的启动脚本，在系统每次启动时执行此操作。)

The files  in /proc/sys can be used to fine tune and monitor miscellaneous and
general things  in  the operation of the Linux kernel. Since some of the files
can inadvertently  disrupt  your  system,  it  is  advisable  to  read  both
documentation and  source  before actually making adjustments. In any case, be
very careful  when  writing  to  any  of these files. The entries in /proc may
change slightly between the 2.1.* and the 2.2 kernel, so if there is any doubt
review the kernel documentation in the directory linux/Documentation.
This chapter  is  heavily  based  on the documentation included in the pre 2.2
kernels, and became part of it in version 2.2.1 of the Linux kernel.
(/proc/sys 中的文件可用于对 Linux 内核运行中的各类一般性事务进行微调和监控。由于其中一些文件可能会在无意中破坏你的系统，建议在实际调整之前先阅读文档和源代码。无论如何，向这些文件中的任何一个写入时都要非常小心。/proc 中的条目在 2.1.* 和 2.2 内核之间可能略有变化，因此如有疑问，请查阅 linux/Documentation 目录中的内核文档。本章大量基于 2.2 之前的内核所附带的文档，并在 Linux 内核 2.2.1 版本中成为其一部分。)

Please see: Documentation/admin-guide/sysctl/ directory for descriptions of
these entries.
(这些条目的说明请参见 Documentation/admin-guide/sysctl/ 目录。)

Summary
(小结)
-------

Certain aspects  of  kernel  behavior  can be modified at runtime, without the
need to  recompile  the kernel, or even to reboot the system. The files in the
/proc/sys tree  can  not only be read, but also modified. You can use the echo
command to write value into these files, thereby changing the default settings
of the kernel.
(内核行为的某些方面可以在运行时修改，无需重新编译内核，甚至无需重启系统。/proc/sys 树中的文件不仅可以读取，还可以修改。你可以使用 echo 命令向这些文件写入值，从而更改内核的默认设置。)


Chapter 3: Per-process Parameters
(第 3 章：进程相关参数)
=================================

3.1 /proc/<pid>/oom_adj & /proc/<pid>/oom_score_adj- Adjust the oom-killer score
(3.1 /proc/<pid>/oom_adj 与 /proc/<pid>/oom_score_adj - 调整 oom-killer 分数)
--------------------------------------------------------------------------------

These files can be used to adjust the badness heuristic used to select which
process gets killed in out of memory (oom) conditions.
(这些文件可用于调整"恶劣程度"(badness)启发式算法，该算法用于选择在内存不足(oom)情况下杀死哪个进程。)

The badness heuristic assigns a value to each candidate task ranging from 0
(never kill) to 1000 (always kill) to determine which process is targeted.  The
units are roughly a proportion along that range of allowed memory the process
may allocate from based on an estimation of its current memory and swap use.
For example, if a task is using all allowed memory, its badness score will be
1000.  If it is using half of its allowed memory, its score will be 500.
(恶劣程度启发式算法为每个候选任务分配一个从 0(绝不杀死)到 1000(总是杀死)的值，以确定以哪个进程为目标。该值的单位大致是进程可分配的允许内存在这个范围上的比例，它基于对该进程当前内存和交换空间使用量的估计。例如，如果某任务正在使用其全部允许内存，它的恶劣程度分数将是 1000。如果它使用了允许内存的一半，它的分数将是 500。)

The amount of "allowed" memory depends on the context in which the oom killer
was called.  If it is due to the memory assigned to the allocating task's cpuset
being exhausted, the allowed memory represents the set of mems assigned to that
cpuset.  If it is due to a mempolicy's node(s) being exhausted, the allowed
memory represents the set of mempolicy nodes.  If it is due to a memory
limit (or swap limit) being reached, the allowed memory is that configured
limit.  Finally, if it is due to the entire system being out of memory, the
allowed memory represents all allocatable resources.
("允许"内存的多少取决于调用 oom killer 时的上下文。如果是由于分配任务所在 cpuset 的内存耗尽，允许内存表示分配给该 cpuset 的那组内存节点(mems)。如果是由于 mempolicy 的节点耗尽，允许内存表示该 mempolicy 的节点集合。如果是达到了内存限制(或交换空间限制)，允许内存就是所配置的限制值。最后，如果是整个系统内存不足，允许内存表示所有可分配的资源。)

The value of /proc/<pid>/oom_score_adj is added to the badness score before it
is used to determine which task to kill.  Acceptable values range from -1000
(OOM_SCORE_ADJ_MIN) to +1000 (OOM_SCORE_ADJ_MAX).  This allows userspace to
polarize the preference for oom killing either by always preferring a certain
task or completely disabling it.  The lowest possible value, -1000, is
equivalent to disabling oom killing entirely for that task since it will always
report a badness score of 0.
(/proc/<pid>/oom_score_adj 的值会在恶劣程度分数被用于确定杀死哪个任务之前加到该分数上。可接受的取值范围从 -1000(OOM_SCORE_ADJ_MIN)到 +1000(OOM_SCORE_ADJ_MAX)。这使得用户空间可以把 oom 杀死的偏好极化——要么总是优先某个任务，要么完全禁用它。最低的可能值 -1000 等价于对该任务完全禁用 oom 杀死，因为它总是报告 0 的恶劣程度分数。)

Consequently, it is very simple for userspace to define the amount of memory to
consider for each task.  Setting a /proc/<pid>/oom_score_adj value of +500, for
example, is roughly equivalent to allowing the remainder of tasks sharing the
same system, cpuset, mempolicy, or memory controller resources to use at least
50% more memory.  A value of -500, on the other hand, would be roughly
equivalent to discounting 50% of the task's allowed memory from being considered
as scoring against the task.
(因此，用户空间可以非常简单地定义每个任务要考虑的内存量。例如，将 /proc/<pid>/oom_score_adj 设置为 +500，大致等价于允许共享同一系统、cpuset、mempolicy 或内存控制器资源的其余任务多使用至少 50% 的内存。另一方面，-500 的值大致等价于在计算针对该任务的分数时，将其允许内存打 50% 的折扣。)

For backwards compatibility with previous kernels, /proc/<pid>/oom_adj may also
be used to tune the badness score.  Its acceptable values range from -16
(OOM_ADJUST_MIN) to +15 (OOM_ADJUST_MAX) and a special value of -17
(OOM_DISABLE) to disable oom killing entirely for that task.  Its value is
scaled linearly with /proc/<pid>/oom_score_adj.
(为了向后兼容以前的内核，/proc/<pid>/oom_adj 也可用于调整恶劣程度分数。它的可接受取值范围从 -16(OOM_ADJUST_MIN)到 +15(OOM_ADJUST_MAX)，以及一个特殊值 -17(OOM_DISABLE)用于对该任务完全禁用 oom 杀死。它的值与 /proc/<pid>/oom_score_adj 线性对应。)

The value of /proc/<pid>/oom_score_adj may be reduced no lower than the last
value set by a CAP_SYS_RESOURCE process. To reduce the value any lower
requires CAP_SYS_RESOURCE.
(/proc/<pid>/oom_score_adj 的值可以降低，但不能低于由具有 CAP_SYS_RESOURCE 的进程最后一次设置的值。要把该值降得更低则需要 CAP_SYS_RESOURCE。)


3.2 /proc/<pid>/oom_score - Display current oom-killer score
(3.2 /proc/<pid>/oom_score - 显示当前 oom-killer 分数)
-------------------------------------------------------------

This file can be used to check the current score used by the oom-killer for
any given <pid>. Use it together with /proc/<pid>/oom_score_adj to tune which
process should be killed in an out-of-memory situation.
(该文件可用于检查 oom-killer 对任意给定 <pid> 当前使用的分数。将它与 /proc/<pid>/oom_score_adj 一起使用，可以调整在内存不足情况下应该杀死哪个进程。)

Please note that the exported value includes oom_score_adj so it is
effectively in range [0,2000].
(请注意，导出的值包含 oom_score_adj，因此它实际处于 [0,2000] 范围内。)


3.3  /proc/<pid>/io - Display the IO accounting fields
(3.3 /proc/<pid>/io - 显示 IO 统计字段)
-------------------------------------------------------

This file contains IO statistics for each running process.
(该文件包含每个正在运行的进程的 IO 统计信息。)

Example
(示例)
~~~~~~~

::

    test:/tmp # dd if=/dev/zero of=/tmp/test.dat &
    [1] 3828

    test:/tmp # cat /proc/3828/io
    rchar: 323934931
    wchar: 323929600
    syscr: 632687
    syscw: 632675
    read_bytes: 0
    write_bytes: 323932160
    cancelled_write_bytes: 0


Description
(描述)
~~~~~~~~~~~

rchar
^^^^^

I/O counter: chars read
The number of bytes which this task has caused to be read from storage. This
is simply the sum of bytes which this process passed to read() and pread().
It includes things like tty IO and it is unaffected by whether or not actual
physical disk IO was required (the read might have been satisfied from
pagecache).
(I/O 计数器：读取的字符数
该任务导致从存储读取的字节数。它就是该进程传递给 read() 和 pread() 的字节数之和。它包括诸如 tty IO 之类的情形，并且不受是否实际需要物理磁盘 IO 的影响(读取可能已由页缓存满足)。)


wchar
^^^^^

I/O counter: chars written
The number of bytes which this task has caused, or shall cause to be written
to disk. Similar caveats apply here as with rchar.
(I/O 计数器：写入的字符数
该任务已经或将要导致写入磁盘的字节数。此处的注意事项与 rchar 相同。)


syscr
^^^^^

I/O counter: read syscalls
Attempt to count the number of read I/O operations, i.e. syscalls like read()
and pread().
(I/O 计数器：读系统调用
尝试统计读 I/O 操作的次数，即诸如 read() 和 pread() 之类的系统调用。)


syscw
^^^^^

I/O counter: write syscalls
Attempt to count the number of write I/O operations, i.e. syscalls like
write() and pwrite().
(I/O 计数器：写系统调用
尝试统计写 I/O 操作的次数，即诸如 write() 和 pwrite() 之类的系统调用。)


read_bytes
^^^^^^^^^^

I/O counter: bytes read
Attempt to count the number of bytes which this process really did cause to
be fetched from the storage layer. Done at the submit_bio() level, so it is
accurate for block-backed filesystems. <please add status regarding NFS and
CIFS at a later time>
(I/O 计数器：读取的字节数
尝试统计该进程确实导致从存储层获取的字节数。它在 submit_bio() 层面完成，因此对块设备支撑的文件系统是准确的。<请稍后补充关于 NFS 和 CIFS 的状态说明>)


write_bytes
^^^^^^^^^^^

I/O counter: bytes written
Attempt to count the number of bytes which this process caused to be sent to
the storage layer. This is done at page-dirtying time.
(I/O 计数器：写入的字节数
尝试统计该进程导致发送到存储层的字节数。它在页变脏(page-dirtying)时完成。)


cancelled_write_bytes
^^^^^^^^^^^^^^^^^^^^^

The big inaccuracy here is truncate. If a process writes 1MB to a file and
then deletes the file, it will in fact perform no writeout. But it will have
been accounted as having caused 1MB of write.
In other words: The number of bytes which this process caused to not happen,
by truncating pagecache. A task can cause "negative" IO too. If this task
truncates some dirty pagecache, some IO which another task has been accounted
for (in its write_bytes) will not be happening. We _could_ just subtract that
from the truncating task's write_bytes, but there is information loss in doing
that.
(这里最大的不准确之处是截断(truncate)。如果一个进程向文件写入 1MB 然后删除该文件，它实际上不会执行任何回写。但它已经被统计为造成了 1MB 的写入。
换句话说：该进程通过截断页缓存而使本来要发生的写入没有发生的字节数。任务也可能造成"负"IO。如果该任务截断了一些脏页缓存，那么另一个任务已被计入(其 write_bytes 中)的一些 IO 将不会发生。我们_可以_直接从执行截断的任务的 write_bytes 中减去它，但这样做会丢失信息。)


.. Note::

   At its current implementation state, this is a bit racy on 32-bit machines:
   if process A reads process B's /proc/pid/io while process B is updating one
   of those 64-bit counters, process A could see an intermediate result.
   (在当前的实现状态下，这在 32 位机器上存在一点竞态：如果进程 A 在进程 B 正在更新某个 64 位计数器时读取进程 B 的 /proc/pid/io，进程 A 可能会看到中间结果。)


More information about this can be found within the taskstats documentation in
Documentation/accounting.
(关于此内容的更多信息可以在 Documentation/accounting 中的 taskstats 文档里找到。)

3.4 /proc/<pid>/coredump_filter - Core dump filtering settings
(3.4 /proc/<pid>/coredump_filter - 核心转储过滤设置)
---------------------------------------------------------------
When a process is dumped, all anonymous memory is written to a core file as
long as the size of the core file isn't limited. But sometimes we don't want
to dump some memory segments, for example, huge shared memory or DAX.
Conversely, sometimes we want to save file-backed memory segments into a core
file, not only the individual files.
(当进程被转储时，只要核心文件的大小不受限制，所有匿名内存都会被写入核心文件。但有时我们不想转储某些内存段，例如巨型共享内存或 DAX。反过来，有时我们想把文件支撑的内存段也保存进核心文件，而不仅是单个的文件。)

/proc/<pid>/coredump_filter allows you to customize which memory segments
will be dumped when the <pid> process is dumped. coredump_filter is a bitmask
of memory types. If a bit of the bitmask is set, memory segments of the
corresponding memory type are dumped, otherwise they are not dumped.
(/proc/<pid>/coredump_filter 允许你自定义当 <pid> 进程被转储时哪些内存段会被转储。coredump_filter 是一个内存类型位掩码。如果位掩码中的某一位被置位，则对应内存类型的内存段会被转储，否则不转储。)

The following 9 memory types are supported:
(支持以下 9 种内存类型：)

  - (bit 0) anonymous private memory
    ((位 0)匿名私有内存)
  - (bit 1) anonymous shared memory
    ((位 1)匿名共享内存)
  - (bit 2) file-backed private memory
    ((位 2)文件支撑的私有内存)
  - (bit 3) file-backed shared memory
    ((位 3)文件支撑的共享内存)
  - (bit 4) ELF header pages in file-backed private memory areas (it is
    effective only if the bit 2 is cleared)
    ((位 4)文件支撑私有内存区域中的 ELF 头页(仅当位 2 被清除时才有效))
  - (bit 5) hugetlb private memory
    ((位 5)hugetlb 私有内存)
  - (bit 6) hugetlb shared memory
    ((位 6)hugetlb 共享内存)
  - (bit 7) DAX private memory
    ((位 7)DAX 私有内存)
  - (bit 8) DAX shared memory
    ((位 8)DAX 共享内存)

  Note that MMIO pages such as frame buffer are never dumped and vDSO pages
  are always dumped regardless of the bitmask status.
  (注意，诸如帧缓冲(frame buffer)之类的 MMIO 页永远不会被转储，而无论位掩码状态如何，vDSO 页总是会被转储。)

  Note that bits 0-4 don't affect hugetlb or DAX memory. hugetlb memory is
  only affected by bit 5-6, and DAX is only affected by bits 7-8.
  (注意，位 0-4 不影响 hugetlb 或 DAX 内存。hugetlb 内存只受位 5-6 影响，DAX 只受位 7-8 影响。)

The default value of coredump_filter is 0x33; this means all anonymous memory
segments, ELF header pages and hugetlb private memory are dumped.
(coredump_filter 的默认值为 0x33；这意味着所有匿名内存段、ELF 头页和 hugetlb 私有内存都会被转储。)

If you don't want to dump all shared memory segments attached to pid 1234,
write 0x31 to the process's proc file::
(如果你不想转储连接到 pid 1234 的所有共享内存段，向该进程的 proc 文件写入 0x31：)

  $ echo 0x31 > /proc/1234/coredump_filter

When a new process is created, the process inherits the bitmask status from its
parent. It is useful to set up coredump_filter before the program runs.
For example::
(当创建新进程时，该进程会从其父进程继承位掩码状态。在程序运行之前设置好 coredump_filter 会很有用。
例如：)

  $ echo 0x7 > /proc/self/coredump_filter
  $ ./some_program

3.5	/proc/<pid>/mountinfo - Information about mounts
(3.5 /proc/<pid>/mountinfo - 挂载信息)
--------------------------------------------------------

This file contains lines of the form::
(该文件包含如下形式的行：)

    36 35 98:0 /mnt1 /mnt2 rw,noatime master:1 - ext3 /dev/root rw,errors=continue
    (1)(2)(3)   (4)   (5)      (6)     (n…m) (m+1)(m+2) (m+3)         (m+4)

    (1)   mount ID:        unique identifier of the mount (may be reused after umount)
    (2)   parent ID:       ID of parent (or of self for the top of the mount tree)
    (3)   major:minor:     value of st_dev for files on filesystem
    (4)   root:            root of the mount within the filesystem
    (5)   mount point:     mount point relative to the process's root
    (6)   mount options:   per mount options
    (n…m) optional fields: zero or more fields of the form "tag[:value]"
    (m+1) separator:       marks the end of the optional fields
    (m+2) filesystem type: name of filesystem of the form "type[.subtype]"
    (m+3) mount source:    filesystem specific information or "none"
    (m+4) super options:   per super block options

Parsers should ignore all unrecognised optional fields.  Currently the
possible optional fields are:
(解析器应忽略所有无法识别的可选字段。当前可能的可选字段为：)

================  ==============================================================
shared:X          mount is shared in peer group X (挂载在对等组 X 中共享)
master:X          mount is slave to peer group X (挂载是对等组 X 的从属(slave))
propagate_from:X  mount is slave and receives propagation from peer group X [#]_ (挂载是从属并从对等组 X 接收传播 [#]_)
unbindable        mount is unbindable (挂载不可绑定(unbindable))
================  ==============================================================

.. [#] X is the closest dominant peer group under the process's root.  If
       X is the immediate master of the mount, or if there's no dominant peer
       group under the same root, then only the "master:X" field is present
       and not the "propagate_from:X" field.
       (X 是进程根目录下最近的主对等组。如果 X 是该挂载的直接主对等组，或者同一根目录下没有主对等组，则只存在 "master:X" 字段而不存在 "propagate_from:X" 字段。)

For more information on mount propagation see:

  Documentation/filesystems/sharedsubtree.rst
(有关挂载传播的更多信息，请参见：
  Documentation/filesystems/sharedsubtree.rst)


3.6	/proc/<pid>/comm  & /proc/<pid>/task/<tid>/comm
(3.6 /proc/<pid>/comm 与 /proc/<pid>/task/<tid>/comm)
--------------------------------------------------------
These files provide a method to access a task's comm value. It also allows for
a task to set its own or one of its thread siblings comm value. The comm value
is limited in size compared to the cmdline value, so writing anything longer
then the kernel's TASK_COMM_LEN (currently 16 chars, including the NUL
terminator) will result in a truncated comm value.
(这些文件提供访问任务 comm 值的方法。它还允许任务设置自身或其某个线程兄弟的 comm 值。与 cmdline 值相比，comm 值的大小受限，因此写入任何超过内核 TASK_COMM_LEN(当前为 16 个字符，包括 NUL 终止符)的内容都会导致 comm 值被截断。)


3.7	/proc/<pid>/task/<tid>/children - Information about task children
(3.7 /proc/<pid>/task/<tid>/children - 任务子进程的信息)
-------------------------------------------------------------------------
This file provides a fast way to retrieve first level children pids
of a task pointed by <pid>/<tid> pair. The format is a space separated
stream of pids.
(该文件提供了一种快速检索 <pid>/<tid> 对所指向任务的第一级子进程 pid 的方法。格式是一个以空格分隔的 pid 流。)

Note the "first level" here -- if a child has its own children they will
not be listed here; one needs to read /proc/<children-pid>/task/<tid>/children
to obtain the descendants.
(注意这里的"第一级"——如果某个子进程还有它自己的子进程，它们不会列在这里；要获取后代，需要读取 /proc/<children-pid>/task/<tid>/children。)

Since this interface is intended to be fast and cheap it doesn't
guarantee to provide precise results and some children might be
skipped, especially if they've exited right after we printed their
pids, so one needs to either stop or freeze processes being inspected
if precise results are needed.
(由于此接口旨在快速且开销低，它不保证提供精确的结果，某些子进程可能会被跳过，尤其是当它们在我们打印其 pid 之后立即退出时。因此，如果需要精确的结果，需要停止或冻结被检查的进程。)


3.8	/proc/<pid>/fdinfo/<fd> - Information about opened file
(3.8 /proc/<pid>/fdinfo/<fd> - 已打开文件的信息)
---------------------------------------------------------------
This file provides information associated with an opened file. The regular
files have at least four fields -- 'pos', 'flags', 'mnt_id' and 'ino'.
The 'pos' represents the current offset of the opened file in decimal
form [see lseek(2) for details], 'flags' denotes the octal O_xxx mask the
file has been created with [see open(2) for details] and 'mnt_id' represents
mount ID of the file system containing the opened file [see 3.5
/proc/<pid>/mountinfo for details]. 'ino' represents the inode number of
the file.
(该文件提供与已打开文件关联的信息。常规文件至少有四个字段——'pos'、'flags'、'mnt_id' 和 'ino'。'pos' 表示已打开文件当前的偏移量，以十进制形式表示[详情参见 lseek(2)]；'flags' 表示创建该文件时所用的八进制 O_xxx 掩码[详情参见 open(2)]；'mnt_id' 表示包含该已打开文件的文件系统的挂载 ID[详情参见 3.5 /proc/<pid>/mountinfo]。'ino' 表示该文件的 inode 编号。)

A typical output is::

	pos:	0
	flags:	0100002
	mnt_id:	19
	ino:	63107

All locks associated with a file descriptor are shown in its fdinfo too::
(与文件描述符关联的所有锁也会显示在其 fdinfo 中：)

    lock:       1: FLOCK  ADVISORY  WRITE 359 00:13:11691 0 EOF

The files such as eventfd, fsnotify, signalfd, epoll among the regular pos/flags
pair provide additional information particular to the objects they represent.
(诸如 eventfd、fsnotify、signalfd、epoll 之类的文件，在常规的 pos/flags 对之外，还会提供它们所代表对象特有的附加信息。)

Eventfd files
(Eventfd 文件)
~~~~~~~~~~~~~

::

	pos:	0
	flags:	04002
	mnt_id:	9
	ino:	63107
	eventfd-count:	5a

where 'eventfd-count' is hex value of a counter.
(其中 'eventfd-count' 是计数器的十六进制值。)

Signalfd files
(Signalfd 文件)
~~~~~~~~~~~~~~

::

	pos:	0
	flags:	04002
	mnt_id:	9
	ino:	63107
	sigmask:	0000000000000200

where 'sigmask' is hex value of the signal mask associated
with a file.
(其中 'sigmask' 是与文件关联的信号掩码的十六进制值。)

Epoll files
(Epoll 文件)
~~~~~~~~~~~

::

	pos:	0
	flags:	02
	mnt_id:	9
	ino:	63107
	tfd:        5 events:       1d data: ffffffffffffffff pos:0 ino:61af sdev:7

where 'tfd' is a target file descriptor number in decimal form,
'events' is events mask being watched and the 'data' is data
associated with a target [see epoll(7) for more details].
(其中 'tfd' 是目标文件描述符编号，以十进制形式表示；'events' 是被监视的事件掩码；'data' 是与目标关联的数据[更多详情参见 epoll(7)]。)

The 'pos' is current offset of the target file in decimal form
[see lseek(2)], 'ino' and 'sdev' are inode and device numbers
where target file resides, all in hex format.
('pos' 是目标文件当前的偏移量，以十进制形式表示[参见 lseek(2)]；'ino' 和 'sdev' 是目标文件所在处的 inode 和设备编号，均以十六进制格式表示。)

Fsnotify files
(Fsnotify 文件)
~~~~~~~~~~~~~~
For inotify files the format is the following::
(对于 inotify 文件，格式如下：)

	pos:	0
	flags:	02000000
	mnt_id:	9
	ino:	63107
	inotify wd:3 ino:9e7e sdev:800013 mask:800afce ignored_mask:0 fhandle-bytes:8 fhandle-type:1 f_handle:7e9e0000640d1b6d

where 'wd' is a watch descriptor in decimal form, i.e. a target file
descriptor number, 'ino' and 'sdev' are inode and device where the
target file resides and the 'mask' is the mask of events, all in hex
form [see inotify(7) for more details].
(其中 'wd' 是以十进制形式表示的监视描述符，即目标文件描述符编号；'ino' 和 'sdev' 是目标文件所在处的 inode 和设备；'mask' 是事件掩码，均以十六进制形式表示[更多详情参见 inotify(7)]。)

If the kernel was built with exportfs support, the path to the target
file is encoded as a file handle.  The file handle is provided by three
fields 'fhandle-bytes', 'fhandle-type' and 'f_handle', all in hex
format.
(如果内核构建时启用了 exportfs 支持，则目标文件的路径会被编码为文件句柄(file handle)。文件句柄由 'fhandle-bytes'、'fhandle-type' 和 'f_handle' 三个字段提供，均以十六进制格式表示。)

If the kernel is built without exportfs support the file handle won't be
printed out.
(如果内核构建时未启用 exportfs 支持，则不会打印文件句柄。)

If there is no inotify mark attached yet the 'inotify' line will be omitted.
(如果尚未附加任何 inotify 标记，则 'inotify' 行将被省略。)

For fanotify files the format is::
(对于 fanotify 文件，格式为：)

	pos:	0
	flags:	02
	mnt_id:	9
	ino:	63107
	fanotify flags:10 event-flags:0
	fanotify mnt_id:12 mflags:40 mask:38 ignored_mask:40000003
	fanotify ino:4f969 sdev:800013 mflags:0 mask:3b ignored_mask:40000000 fhandle-bytes:8 fhandle-type:1 f_handle:69f90400c275b5b4

where fanotify 'flags' and 'event-flags' are values used in fanotify_init
call, 'mnt_id' is the mount point identifier, 'mflags' is the value of
flags associated with mark which are tracked separately from events
mask. 'ino' and 'sdev' are target inode and device, 'mask' is the events
mask and 'ignored_mask' is the mask of events which are to be ignored.
All are in hex format. Incorporation of 'mflags', 'mask' and 'ignored_mask'
provide information about flags and mask used in fanotify_mark
call [see fsnotify manpage for details].
(其中 fanotify 的 'flags' 和 'event-flags' 是 fanotify_init 调用中使用的值；'mnt_id' 是挂载点标识符；'mflags' 是与标记(mark)关联的标志的值，它们与事件掩码分开跟踪。'ino' 和 'sdev' 是目标 inode 和设备；'mask' 是事件掩码；'ignored_mask' 是要忽略的事件的掩码。均以十六进制格式表示。'mflags'、'mask' 和 'ignored_mask' 的组合提供了 fanotify_mark 调用中使用的标志和掩码的信息[详情参见 fsnotify manpage]。)

While the first three lines are mandatory and always printed, the rest is
optional and may be omitted if no marks created yet.
(前三行是必需的并且总是会打印，其余部分是可选的，如果尚未创建任何标记则可能被省略。)

Timerfd files
(Timerfd 文件)
~~~~~~~~~~~~~

::

	pos:	0
	flags:	02
	mnt_id:	9
	ino:	63107
	clockid: 0
	ticks: 0
	settime flags: 01
	it_value: (0, 49406829)
	it_interval: (1, 0)

where 'clockid' is the clock type and 'ticks' is the number of the timer expirations
that have occurred [see timerfd_create(2) for details]. 'settime flags' are
flags in octal form been used to setup the timer [see timerfd_settime(2) for
details]. 'it_value' is remaining time until the timer expiration.
'it_interval' is the interval for the timer. Note the timer might be set up
with TIMER_ABSTIME option which will be shown in 'settime flags', but 'it_value'
still exhibits timer's remaining time.
(其中 'clockid' 是时钟类型；'ticks' 是已经发生的定时器到期次数[详情参见 timerfd_create(2)]。'settime flags' 是设置定时器时使用的八进制形式的标志[详情参见 timerfd_settime(2)]。'it_value' 是距离定时器到期的剩余时间。'it_interval' 是定时器的间隔。注意，定时器可能以 TIMER_ABSTIME 选项设置，这会显示在 'settime flags' 中，但 'it_value' 仍显示定时器的剩余时间。)

DMA Buffer files
(DMA 缓冲文件)
~~~~~~~~~~~~~~~~

::

	pos:	0
	flags:	04002
	mnt_id:	9
	ino:	63107
	size:   32768
	count:  2
	exp_name:  system-heap

where 'size' is the size of the DMA buffer in bytes. 'count' is the file count of
the DMA buffer file. 'exp_name' is the name of the DMA buffer exporter.
(其中 'size' 是 DMA 缓冲的大小，以字节计。'count' 是 DMA 缓冲文件的文件计数。'exp_name' 是 DMA 缓冲导出器(exporter)的名称。)

VFIO Device files
(VFIO 设备文件)
~~~~~~~~~~~~~~~~~

::

	pos:    0
	flags:  02000002
	mnt_id: 17
	ino:    5122
	vfio-device-syspath: /sys/devices/pci0000:e0/0000:e0:01.1/0000:e1:00.0/0000:e2:05.0/0000:e8:00.0

where 'vfio-device-syspath' is the sysfs path corresponding to the VFIO device
file.
(其中 'vfio-device-syspath' 是与该 VFIO 设备文件对应的 sysfs 路径。)

3.9	/proc/<pid>/map_files - Information about memory mapped files
(3.9 /proc/<pid>/map_files - 内存映射文件的信息)
---------------------------------------------------------------------
This directory contains symbolic links which represent memory mapped files
the process is maintaining.  Example output::
(该目录包含代表进程正在维护的内存映射文件的符号链接。输出示例：)

     | lr-------- 1 root root 64 Jan 27 11:24 333c600000-333c620000 -> /usr/lib64/ld-2.18.so
     | lr-------- 1 root root 64 Jan 27 11:24 333c81f000-333c820000 -> /usr/lib64/ld-2.18.so
     | lr-------- 1 root root 64 Jan 27 11:24 333c820000-333c821000 -> /usr/lib64/ld-2.18.so
     | ...
     | lr-------- 1 root root 64 Jan 27 11:24 35d0421000-35d0422000 -> /usr/lib64/libselinux.so.1
     | lr-------- 1 root root 64 Jan 27 11:24 400000-41a000 -> /usr/bin/ls

The name of a link represents the virtual memory bounds of a mapping, i.e.
vm_area_struct::vm_start-vm_area_struct::vm_end.
(链接的名称表示一个映射的虚拟内存边界，即 vm_area_struct::vm_start-vm_area_struct::vm_end。)

The main purpose of the map_files is to retrieve a set of memory mapped
files in a fast way instead of parsing /proc/<pid>/maps or
/proc/<pid>/smaps, both of which contain many more records.  At the same
time one can open(2) mappings from the listings of two processes and
comparing their inode numbers to figure out which anonymous memory areas
are actually shared.
(map_files 的主要目的是以快速的方式检索一组内存映射文件，而不是解析 /proc/<pid>/maps 或 /proc/<pid>/smaps，后两者包含多得多的记录。同时，可以从两个进程的列表中 open(2) 各自的映射，并比较它们的 inode 编号，从而弄清哪些匿名内存区域是实际共享的。)

3.10	/proc/<pid>/timerslack_ns - Task timerslack value
(3.10 /proc/<pid>/timerslack_ns - 任务的 timerslack 值)
---------------------------------------------------------
This file provides the value of the task's timerslack value in nanoseconds.
This value specifies an amount of time that normal timers may be deferred
in order to coalesce timers and avoid unnecessary wakeups.
(该文件以纳秒为单位提供任务的 timerslack 值。该值指定普通定时器可以被推迟的时间量，以便合并定时器并避免不必要的唤醒。)

This allows a task's interactivity vs power consumption tradeoff to be
adjusted.
(这使得可以调整任务在交互性与功耗之间的权衡。)

Writing 0 to the file will set the task's timerslack to the default value.
(向该文件写入 0 会将任务的 timerslack 设置为默认值。)

Valid values are from 0 - ULLONG_MAX
(有效值为 0 - ULLONG_MAX)

An application setting the value must have PTRACE_MODE_ATTACH_FSCREDS level
permissions on the task specified to change its timerslack_ns value.
(设置该值的应用程序必须对指定任务具有 PTRACE_MODE_ATTACH_FSCREDS 级别的权限，才能更改其 timerslack_ns 值。)

3.11	/proc/<pid>/patch_state - Livepatch patch operation state
(3.11 /proc/<pid>/patch_state - Livepatch 补丁操作状态)
-----------------------------------------------------------------
When CONFIG_LIVEPATCH is enabled, this file displays the value of the
patch state for the task.
(当启用 CONFIG_LIVEPATCH 时，该文件显示任务的补丁状态值。)

A value of '-1' indicates that no patch is in transition.
(值 '-1' 表示没有补丁处于过渡状态。)

A value of '0' indicates that a patch is in transition and the task is
unpatched.  If the patch is being enabled, then the task hasn't been
patched yet.  If the patch is being disabled, then the task has already
been unpatched.
(值 '0' 表示有补丁处于过渡状态且任务未打补丁。如果该补丁正在被启用，则该任务尚未被打上补丁。如果该补丁正在被禁用，则该任务已经被取消补丁。)

A value of '1' indicates that a patch is in transition and the task is
patched.  If the patch is being enabled, then the task has already been
patched.  If the patch is being disabled, then the task hasn't been
unpatched yet.
(值 '1' 表示有补丁处于过渡状态且任务已打补丁。如果该补丁正在被启用，则该任务已经被打上补丁。如果该补丁正在被禁用，则该任务尚未被取消补丁。)

3.12 /proc/<pid>/arch_status - task architecture specific status
(3.12 /proc/<pid>/arch_status - 任务架构特定的状态)
-------------------------------------------------------------------
When CONFIG_PROC_PID_ARCH_STATUS is enabled, this file displays the
architecture specific status of the task.
(当启用 CONFIG_PROC_PID_ARCH_STATUS 时，该文件显示任务架构特定的状态。)

Example
(示例)
~~~~~~~

::

 $ cat /proc/6753/arch_status
 AVX512_elapsed_ms:      8

Description
(描述)
~~~~~~~~~~~

x86 specific entries
(x86 特定条目)
~~~~~~~~~~~~~~~~~~~~~

AVX512_elapsed_ms
^^^^^^^^^^^^^^^^^^

  If AVX512 is supported on the machine, this entry shows the milliseconds
  elapsed since the last time AVX512 usage was recorded. The recording
  happens on a best effort basis when a task is scheduled out. This means
  that the value depends on two factors:
  (如果机器支持 AVX512，此条目显示自上次记录 AVX512 使用情况以来经过的毫秒数。记录在任务被调度出去时以尽力而为的方式进行。这意味着该值取决于两个因素：)

    1) The time which the task spent on the CPU without being scheduled
       out. With CPU isolation and a single runnable task this can take
       several seconds.
       (任务在 CPU 上未被调度出去所度过的时间。在 CPU 隔离且只有一个可运行任务的情况下，这可能长达数秒。)

    2) The time since the task was scheduled out last. Depending on the
       reason for being scheduled out (time slice exhausted, syscall ...)
       this can be arbitrary long time.
       (自任务上次被调度出去以来经过的时间。根据被调度出去的原因(时间片用尽、系统调用……)，这可能任意长。)

  As a consequence the value cannot be considered precise and authoritative
  information. The application which uses this information has to be aware
  of the overall scenario on the system in order to determine whether a
  task is a real AVX512 user or not. Precise information can be obtained
  with performance counters.
  (因此，该值不能被视为精确且权威的信息。使用此信息的应用程序必须了解系统上的整体场景，才能判断某个任务是否是真正的 AVX512 使用者。精确的信息可以通过性能计数器获得。)

  A special value of '-1' indicates that no AVX512 usage was recorded, thus
  the task is unlikely an AVX512 user, but depends on the workload and the
  scheduling scenario, it also could be a false negative mentioned above.
  (特殊值 '-1' 表示未记录到 AVX512 使用情况，因此该任务不太可能是 AVX512 使用者；但取决于工作负载和调度场景，它也可能是上文提到的假阴性(false negative)。)

3.13 /proc/<pid>/fd - List of symlinks to open files
(3.13 /proc/<pid>/fd - 打开文件的符号链接列表)
-------------------------------------------------------
This directory contains symbolic links which represent open files
the process is maintaining.  Example output::
(该目录包含代表进程正在维护的打开文件的符号链接。输出示例：)

  lr-x------ 1 root root 64 Sep 20 17:53 0 -> /dev/null
  l-wx------ 1 root root 64 Sep 20 17:53 1 -> /dev/null
  lrwx------ 1 root root 64 Sep 20 17:53 10 -> 'socket:[12539]'
  lrwx------ 1 root root 64 Sep 20 17:53 11 -> 'socket:[12540]'
  lrwx------ 1 root root 64 Sep 20 17:53 12 -> 'socket:[12542]'

The number of open files for the process is stored in 'size' member
of stat() output for /proc/<pid>/fd for fast access.
(该进程打开文件的数量存储在 /proc/<pid>/fd 的 stat() 输出的 'size' 成员中，以便快速访问。)
-------------------------------------------------------

3.14 /proc/<pid>/ksm_stat - Information about the process's ksm status
(3.14 /proc/<pid>/ksm_stat - 进程 ksm 状态的信息)
----------------------------------------------------------------------
When CONFIG_KSM is enabled, each process has this file which displays
the information of ksm merging status.
(当启用 CONFIG_KSM 时，每个进程都有此文件，它显示 ksm 合并状态的信息。)

Example
(示例)
~~~~~~~

::

    / # cat /proc/self/ksm_stat
    ksm_rmap_items 0
    ksm_zero_pages 0
    ksm_merging_pages 0
    ksm_process_profit 0
    ksm_merge_any: no
    ksm_mergeable: no

Description
(描述)
~~~~~~~~~~~

ksm_rmap_items
^^^^^^^^^^^^^^

The number of ksm_rmap_item structures in use.  The structure
ksm_rmap_item stores the reverse mapping information for virtual
addresses.  KSM will generate a ksm_rmap_item for each ksm-scanned page of
the process.
(正在使用的 ksm_rmap_item 结构的数量。结构 ksm_rmap_item 存储虚拟地址的反向映射信息。KSM 会为该进程的每个经 ksm 扫描的页生成一个 ksm_rmap_item。)

ksm_zero_pages
^^^^^^^^^^^^^^

When /sys/kernel/mm/ksm/use_zero_pages is enabled, it represent how many
empty pages are merged with kernel zero pages by KSM.
(当启用 /sys/kernel/mm/ksm/use_zero_pages 时，它表示 KSM 将多少空页与内核零页(zero page)合并。)

ksm_merging_pages
^^^^^^^^^^^^^^^^^

It represents how many pages of this process are involved in KSM merging
(not including ksm_zero_pages). It is the same with what
/proc/<pid>/ksm_merging_pages shows.
(它表示该进程有多少页参与了 KSM 合并(不包括 ksm_zero_pages)。它与 /proc/<pid>/ksm_merging_pages 所显示的一致。)

ksm_process_profit
^^^^^^^^^^^^^^^^^^

The profit that KSM brings (Saved bytes). KSM can save memory by merging
identical pages, but also can consume additional memory, because it needs
to generate a number of rmap_items to save each scanned page's brief rmap
information. Some of these pages may be merged, but some may not be abled
to be merged after being checked several times, which are unprofitable
memory consumed.
(KSM 带来的收益(节省的字节数)。KSM 可以通过合并相同的页来节省内存，但也可能消耗额外的内存，因为它需要生成若干 rmap_item 来保存每个被扫描页的简要 rmap 信息。这些页中的一些可能被合并，但另一些经过多次检查后仍无法合并，这些就是无收益的内存消耗。)

ksm_merge_any
^^^^^^^^^^^^^

It specifies whether the process's 'mm is added by prctl() into the
candidate list of KSM or not, and if KSM scanning is fully enabled at
process level.
(它指定进程的 'mm 是否已通过 prctl() 加入 KSM 的候选列表，以及 KSM 扫描是否在进程级别完全启用。)

ksm_mergeable
^^^^^^^^^^^^^

It specifies whether any VMAs of the process''s mms are currently
applicable to KSM.
(它指定该进程各 mm 的任何 VMA 当前是否适用于 KSM。)

More information about KSM can be found in
Documentation/admin-guide/mm/ksm.rst.
(关于 KSM 的更多信息可在 Documentation/admin-guide/mm/ksm.rst 中找到。)


Chapter 4: Configuring procfs
(第 4 章：配置 procfs)
=============================

4.1	Mount options
(4.1 挂载选项)
---------------------

The following mount options are supported:
(支持以下挂载选项：)

	=========	========================================================
	hidepid=	Set /proc/<pid>/ access mode. (设置 /proc/<pid>/ 的访问模式。)
	gid=		Set the group authorized to learn processes information. (设置有权了解进程信息的组。)
	subset=		Show only the specified subset of procfs. (只显示 procfs 的指定子集。)
	pidns=		Specify a the namespace used by this procfs. (指定此 procfs 使用的命名空间。)
	=========	========================================================

hidepid=off or hidepid=0 means classic mode - everybody may access all
/proc/<pid>/ directories (default).
(hidepid=off 或 hidepid=0 表示经典模式——任何人都可以访问所有 /proc/<pid>/ 目录(默认)。)

hidepid=noaccess or hidepid=1 means users may not access any /proc/<pid>/
directories but their own.  Sensitive files like cmdline, sched*, status are now
protected against other users.  This makes it impossible to learn whether any
user runs specific program (given the program doesn't reveal itself by its
behaviour).  As an additional bonus, as /proc/<pid>/cmdline is unaccessible for
other users, poorly written programs passing sensitive information via program
arguments are now protected against local eavesdroppers.
(hidepid=noaccess 或 hidepid=1 表示用户不能访问除自己的以外的任何 /proc/<pid>/ 目录。像 cmdline、sched*、status 这样的敏感文件现在受到保护，其他用户无法访问。这使得无法得知任何用户是否在运行特定程序(前提是该程序不会通过其行为暴露自己)。另外一个额外好处是，由于 /proc/<pid>/cmdline 对其他用户不可访问，通过程序参数传递敏感信息的编写不良的程序现在也受到了保护，不会被本地窃听者窥探。)

hidepid=invisible or hidepid=2 means hidepid=1 plus all /proc/<pid>/ will be
fully invisible to other users.  It doesn't mean that it hides a fact whether a
process with a specific pid value exists (it can be learned by other means, e.g.
by "kill -0 $PID"), but it hides process's uid and gid, which may be learned by
stat()'ing /proc/<pid>/ otherwise.  It greatly complicates an intruder's task of
gathering information about running processes, whether some daemon runs with
elevated privileges, whether other user runs some sensitive program, whether
other users run any program at all, etc.
(hidepid=invisible 或 hidepid=2 表示 hidepid=1 之外，所有 /proc/<pid>/ 对其他用户将完全不可见。这并不意味着它隐藏了具有特定 pid 值的进程是否存在这一事实(这可以通过其他途径得知，例如 "kill -0 $PID")，但它隐藏了进程的 uid 和 gid，否则这些信息可以通过对 /proc/<pid>/ 进行 stat() 而得知。它极大地增加了入侵者收集运行中进程信息的难度，例如某个守护进程是否以提升的权限运行、其他用户是否在运行某些敏感程序、其他用户是否在运行任何程序，等等。)

hidepid=ptraceable or hidepid=4 means that procfs should only contain
/proc/<pid>/ directories that the caller can ptrace.
(hidepid=ptraceable 或 hidepid=4 表示 procfs 应只包含调用者可以 ptrace 的 /proc/<pid>/ 目录。)

gid= defines a group authorized to learn processes information otherwise
prohibited by hidepid=.  If you use some daemon like identd which needs to learn
information about processes information, just add identd to this group.
(gid= 定义一个被授权了解进程信息的组，否则这些信息会被 hidepid= 禁止访问。如果你使用像 identd 这样需要了解进程信息的守护进程，只需将 identd 加入这个组即可。)

subset=pid hides all top level files and directories in the procfs that
are not related to tasks.
(subset=pid 隐藏 procfs 中所有与任务无关的顶层文件和目录。)

pidns= specifies a pid namespace (either as a string path to something like
`/proc/$pid/ns/pid`, or a file descriptor when using `FSCONFIG_SET_FD`) that
will be used by the procfs instance when translating pids. By default, procfs
will use the calling process's active pid namespace. Note that the pid
namespace of an existing procfs instance cannot be modified (attempting to do
so will give an `-EBUSY` error).
(pidns= 指定一个 pid 命名空间(可以是诸如 `/proc/$pid/ns/pid` 之类的字符串路径，或者在使用 `FSCONFIG_SET_FD` 时的文件描述符)，procfs 实例在转换 pid 时将使用它。默认情况下，procfs 将使用调用进程当前活动的 pid 命名空间。注意，已存在的 procfs 实例的 pid 命名空间不能被修改(尝试这样做会得到 `-EBUSY` 错误)。)

Chapter 5: Filesystem behavior
(第 5 章：文件系统行为)
==============================

Originally, before the advent of pid namespace, procfs was a global file
system. It means that there was only one procfs instance in the system.
(最初，在 pid 命名空间出现之前，procfs 是一个全局文件系统。这意味着系统中只有一个 procfs 实例。)

When pid namespace was added, a separate procfs instance was mounted in
each pid namespace. So, procfs mount options are global among all
mountpoints within the same namespace::
(当加入 pid 命名空间后，每个 pid 命名空间中都挂载了一个独立的 procfs 实例。因此，在同一命名空间内的所有挂载点上，procfs 的挂载选项是全局的：)

	# grep ^proc /proc/mounts
	proc /proc proc rw,relatime,hidepid=2 0 0

	# strace -e mount mount -o hidepid=1 -t proc proc /tmp/proc
	mount("proc", "/tmp/proc", "proc", 0, "hidepid=1") = 0
	+++ exited with 0 +++

	# grep ^proc /proc/mounts
	proc /proc proc rw,relatime,hidepid=2 0 0
	proc /tmp/proc proc rw,relatime,hidepid=2 0 0

and only after remounting procfs mount options will change at all
mountpoints::
(而且只有在重新挂载(remount)之后，procfs 的挂载选项才会在所有挂载点上改变：)

	# mount -o remount,hidepid=1 -t proc proc /tmp/proc

	# grep ^proc /proc/mounts
	proc /proc proc rw,relatime,hidepid=1 0 0
	proc /tmp/proc proc rw,relatime,hidepid=1 0 0

This behavior is different from the behavior of other filesystems.
(这种行为与其他文件系统的行为不同。)

The new procfs behavior is more like other filesystems. Each procfs mount
creates a new procfs instance. Mount options affect own procfs instance.
It means that it became possible to have several procfs instances
displaying tasks with different filtering options in one pid namespace::
(新的 procfs 行为更像其他文件系统了。每次挂载 procfs 都会创建一个新的 procfs 实例。挂载选项只影响各自的 procfs 实例。这意味着在一个 pid 命名空间中，可以有几个 procfs 实例以不同的过滤选项显示任务：)

	# mount -o hidepid=invisible -t proc proc /proc
	# mount -o hidepid=noaccess -t proc proc /tmp/proc
	# grep ^proc /proc/mounts
	proc /proc proc rw,relatime,hidepid=invisible 0 0
	proc /tmp/proc proc rw,relatime,hidepid=noaccess 0 0
