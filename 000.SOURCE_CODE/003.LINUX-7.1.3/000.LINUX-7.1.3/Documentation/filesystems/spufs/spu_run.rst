.. SPDX-License-Identifier: GPL-2.0

=======
spu_run
=======

(spu_run)


Name
(名称)
====
       spu_run - execute an spu context
       (spu_run - 执行一个 SPU 上下文)


Synopsis
(概要)
========

       ::

	    #include <sys/spu.h>

	    int spu_run(int fd, unsigned int *npc, unsigned int *event);

Description
(描述)
===========
       The  spu_run system call is used on PowerPC machines that implement the
       Cell Broadband Engine Architecture in order to access Synergistic  Pro-
       cessor  Units  (SPUs).  It  uses the fd that was returned from spu_cre-
       ate(2) to address a specific SPU context. When the context gets  sched-
       uled  to a physical SPU, it starts execution at the instruction pointer
       passed in npc.
       (spu_run 系统调用用于实现了 Cell 宽带引擎架构的 PowerPC 机器上，以访问协处理器单元(SPU)。它使用从 spu_create(2) 返回的 fd 来寻址特定的 SPU 上下文。当该上下文被调度到物理 SPU 上时，它会从传入 npc 的指令指针处开始执行。)

       Execution of SPU code happens synchronously, meaning that spu_run  does
       not  return  while the SPU is still running. If there is a need to exe-
       cute SPU code in parallel with other code on either  the  main  CPU  or
       other  SPUs,  you  need to create a new thread of execution first, e.g.
       using the pthread_create(3) call.
       (SPU 代码的执行是同步进行的，也就是说，在 SPU 仍在运行期间 spu_run 不会返回。如果需要将 SPU 代码与主 CPU 或其他 SPU 上的其他代码并行执行，你需要先创建一个新的执行线程，例如使用 pthread_create(3) 调用。)

       When spu_run returns, the current value of the SPU instruction  pointer
       is  written back to npc, so you can call spu_run again without updating
       the pointers.
       (当 spu_run 返回时，SPU 指令指针的当前值会被写回 npc，因此你无需更新指针即可再次调用 spu_run。)

       event can be a NULL pointer or point to an extended  status  code  that
       gets  filled  when spu_run returns. It can be one of the following con-
       stants:
       (event 可以是 NULL 指针，或指向一个在 spu_run 返回时被填充的扩展状态码。它可以是以下常量之一：)

       SPE_EVENT_DMA_ALIGNMENT
              A DMA alignment error
              (DMA 对齐错误)

       SPE_EVENT_SPE_DATA_SEGMENT
              A DMA segmentation error
              (DMA 分段错误)

       SPE_EVENT_SPE_DATA_STORAGE
              A DMA storage error
              (DMA 存储错误)

       If NULL is passed as the event argument, these errors will result in  a
       signal delivered to the calling process.
       (如果 event 参数传入 NULL，这些错误将导致向调用进程递送一个信号。)

Return Value
(返回值)
============
       spu_run  returns the value of the spu_status register or -1 to indicate
       an error and set errno to one of the error  codes  listed  below.   The
       spu_status  register  value  contains  a  bit  mask of status codes and
       optionally a 14 bit code returned from the stop-and-signal  instruction
       on the SPU. The bit masks for the status codes are:
       (spu_run 返回 spu_status 寄存器的值，或返回 -1 以指示错误，并将 errno 设置为下面列出的错误码之一。spu_status 寄存器的值包含状态码的位掩码，以及可选的、来自 SPU 上 stop-and-signal 指令的 14 位代码。状态码的位掩码如下：)

       0x02
	      SPU was stopped by stop-and-signal.
	      (SPU 被 stop-and-signal 停止。)

       0x04
	      SPU was stopped by halt.
	      (SPU 被 halt 停止。)

       0x08
	      SPU is waiting for a channel.
	      (SPU 正在等待一个通道。)

       0x10
	      SPU is in single-step mode.
	      (SPU 处于单步模式。)

       0x20
	      SPU has tried to execute an invalid instruction.
	      (SPU 曾试图执行一条无效指令。)

       0x40
	      SPU has tried to access an invalid channel.
	      (SPU 曾试图访问一个无效通道。)

       0x3fff0000
              The  bits  masked with this value contain the code returned from
              stop-and-signal.
              (以该值掩码出的位包含从 stop-and-signal 返回的代码。)

       There are always one or more of the lower eight bits set  or  an  error
       code is returned from spu_run.
       (低八位中总是至少有一位置位，否则 spu_run 会返回一个错误码。)

Errors
(错误)
======
       EAGAIN or EWOULDBLOCK
              fd is in non-blocking mode and spu_run would block.
              (fd 处于非阻塞模式且 spu_run 将会阻塞。)

       EBADF  fd is not a valid file descriptor.
              (fd 不是有效的文件描述符。)

       EFAULT npc is not a valid pointer or status is neither NULL nor a valid
              pointer.
              (npc 不是有效的指针，或者 status 既不是 NULL 也不是有效的指针。)

       EINTR  A signal occurred while spu_run was in progress.  The npc  value
              has  been updated to the new program counter value if necessary.
              (在 spu_run 进行期间发生了信号。如有必要，npc 值已被更新为新的程序计数器值。)

       EINVAL fd is not a file descriptor returned from spu_create(2).
              (fd 不是从 spu_create(2) 返回的文件描述符。)

       ENOMEM Insufficient memory was available to handle a page fault result-
              ing from an MFC direct memory access.
              (没有足够的内存来处理由 MFC 直接内存访问引起的缺页。)

       ENOSYS the functionality is not provided by the current system, because
              either the hardware does not provide SPUs or the spufs module is
              not loaded.
              (当前系统不提供该功能，因为硬件不提供 SPU，或者 spufs 模块未加载。)


Notes
(注)
=====
       spu_run  is  meant  to  be  used  from  libraries that implement a more
       abstract interface to SPUs, not to be used from  regular  applications.
       See  http://www.bsc.es/projects/deepcomputing/linuxoncell/ for the rec-
       ommended libraries.
       (spu_run 旨在由为 SPU 实现了更抽象接口的库来使用，而不是供常规应用程序使用。推荐的库请参见 http://www.bsc.es/projects/deepcomputing/linuxoncell/。)


Conforming to
(遵循标准)
=============
       This call is Linux specific and only implemented by the ppc64
       architecture. Programs using this system call are not portable.
       (该调用是 Linux 特有的，仅由 ppc64 架构实现。使用该系统调用的程序不具备可移植性。)


Bugs
(缺陷)
====
       The code does not yet fully implement all features lined out here.
       (该代码尚未完全实现这里列出的所有功能。)


Author
(作者)
======
       Arnd Bergmann <arndb@de.ibm.com>

See Also
(另请参阅)
========
       capabilities(7), close(2), spu_create(2), spufs(7)
