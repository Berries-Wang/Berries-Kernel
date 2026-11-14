.. SPDX-License-Identifier: GPL-2.0

=====
spufs
=====

(spufs)

Name
(名称)
====

       spufs - the SPU file system
       (spufs——SPU 文件系统)


Description
(描述)
===========

       The SPU file system is used on PowerPC machines that implement the Cell
       Broadband Engine Architecture in order to access Synergistic  Processor
       Units (SPUs).
       (SPU 文件系统用于实现了 Cell Broadband Engine 架构的 PowerPC 机器上，目的是访问 Synergistic Processor Units(SPU)。)

       The file system provides a name space similar to posix shared memory or
       message queues. Users that have write permissions on  the  file  system
       can use spu_create(2) to establish SPU contexts in the spufs root.
       (该文件系统提供了一个类似于 posix 共享内存或消息队列的命名空间。对文件系统拥有写权限的用户可以使用 spu_create(2) 在 spufs 根目录中建立 SPU 上下文。)

       Every SPU context is represented by a directory containing a predefined
       set of files. These files can be used for manipulating the state of the
       logical SPU. Users can change permissions on those files, but not actu-
       ally add or remove files.
       (每个 SPU 上下文都由一个目录表示，其中包含一组预定义的文件。这些文件可用于操控该逻辑 SPU 的状态。用户可以更改这些文件的权限，但实际上并不能添加或删除文件。)


Mount Options
(挂载选项)
=============

       uid=<uid>
              set the user owning the mount point, the default is 0 (root).
              (设置拥有该挂载点的用户，默认为 0(root)。)

       gid=<gid>
              set the group owning the mount point, the default is 0 (root).
              (设置拥有该挂载点的组，默认为 0(root)。)


Files
(文件)
=====

       The files in spufs mostly follow the standard behavior for regular sys-
       tem  calls like read(2) or write(2), but often support only a subset of
       the operations supported on regular file systems. This list details the
       supported  operations  and  the  deviations  from  the behaviour in the
       respective man pages.
       (spufs 中的文件在大多数情况下遵循 read(2)、write(2) 等常规系统调用的标准行为，但通常只支持常规文件系统所支持操作的一个子集。本列表详细说明了所支持的操作，以及与相应手册页中行为的差异。)

       All files that support the read(2) operation also support readv(2)  and
       all  files  that support the write(2) operation also support writev(2).
       All files support the access(2) and stat(2) family of  operations,  but
       only  the  st_mode,  st_nlink,  st_uid and st_gid fields of struct stat
       contain reliable information.
       (所有支持 read(2) 操作的文件也都支持 readv(2)，所有支持 write(2) 操作的文件也都支持 writev(2)。所有文件都支持 access(2) 和 stat(2) 系列操作，但 struct stat 中只有 st_mode、st_nlink、st_uid 和 st_gid 字段包含可靠的信息。)

       All files support the chmod(2)/fchmod(2) and chown(2)/fchown(2)  opera-
       tions,  but  will  not be able to grant permissions that contradict the
       possible operations, e.g. read access on the wbox file.
       (所有文件都支持 chmod(2)/fchmod(2) 和 chown(2)/fchown(2) 操作，但无法授予与可能进行的操作相矛盾的权限，例如对 wbox 文件的读权限。)

       The current set of files is:
       (当前的文件集合如下：)


   /mem
       the contents of the local storage memory  of  the  SPU.   This  can  be
       accessed  like  a regular shared memory file and contains both code and
       data in the address space of the SPU.  The possible  operations  on  an
       open mem file are:
       (SPU 本地存储内存的内容。可以像常规的共享内存文件一样访问它，其中同时包含 SPU 地址空间中的代码和数据。对一个已打开的 mem 文件可能进行的操作有：)

       read(2), pread(2), write(2), pwrite(2), lseek(2)
              These  operate  as  documented, with the exception that seek(2),
              write(2) and pwrite(2) are not supported beyond the end  of  the
              file. The file size is the size of the local storage of the SPU,
              which normally is 256 kilobytes.
              (它们按文档所述进行操作，例外是不支持越过文件末尾的 seek(2)、write(2) 和 pwrite(2)。文件大小是 SPU 本地存储的大小，通常为 256 千字节。)

       mmap(2)
              Mapping mem into the process address space gives access  to  the
              SPU  local  storage  within  the  process  address  space.  Only
              MAP_SHARED mappings are allowed.
              (把 mem 映射到进程地址空间，即可在进程地址空间内访问 SPU 本地存储。只允许 MAP_SHARED 映射。)


   /mbox
       The first SPU to CPU communication mailbox. This file is read-only  and
       can  be  read  in  units of 32 bits.  The file can only be used in non-
       blocking mode and it even poll() will not block on  it.   The  possible
       operations on an open mbox file are:
       (第一个 SPU 到 CPU 的通信邮箱。该文件是只读的，可以以 32 位为单位读取。该文件只能以非阻塞模式使用，甚至连 poll() 也不会在它上面阻塞。对一个已打开的 mbox 文件可能进行的操作有：)

       read(2)
              If  a  count smaller than four is requested, read returns -1 and
              sets errno to EINVAL.  If there is no data available in the mail
              box,  the  return  value  is set to -1 and errno becomes EAGAIN.
              When data has been read successfully, four bytes are  placed  in
              the data buffer and the value four is returned.
              (如果请求的 count 小于四，read 返回 -1 并把 errno 设置为 EINVAL。如果邮箱中没有可用数据，返回值被设置为 -1，errno 变为 EAGAIN。当数据被成功读取时，四个字节被放入数据缓冲区，并返回值四。)


   /ibox
       The  second  SPU  to CPU communication mailbox. This file is similar to
       the first mailbox file, but can be read in blocking I/O mode,  and  the
       poll  family of system calls can be used to wait for it.  The  possible
       operations on an open ibox file are:
       (第二个 SPU 到 CPU 的通信邮箱。该文件与第一个邮箱文件类似，但可以以阻塞 I/O 模式读取，并且可以使用 poll 系列系统调用来等待它。对一个已打开的 ibox 文件可能进行的操作有：)

       read(2)
              If a count smaller than four is requested, read returns  -1  and
              sets errno to EINVAL.  If there is no data available in the mail
              box and the file descriptor has been opened with O_NONBLOCK, the
              return value is set to -1 and errno becomes EAGAIN.
              (如果请求的 count 小于四，read 返回 -1 并把 errno 设置为 EINVAL。如果邮箱中没有可用数据且文件描述符是以 O_NONBLOCK 打开的，返回值被设置为 -1，errno 变为 EAGAIN。)

              If  there  is  no  data  available  in the mail box and the file
              descriptor has been opened without  O_NONBLOCK,  the  call  will
              block  until  the  SPU  writes to its interrupt mailbox channel.
              When data has been read successfully, four bytes are  placed  in
              the data buffer and the value four is returned.
              (如果邮箱中没有可用数据且文件描述符是在没有 O_NONBLOCK 的情况下打开的，该调用将阻塞，直到 SPU 写入它的中断邮箱通道。当数据被成功读取时，四个字节被放入数据缓冲区，并返回值四。)

       poll(2)
              Poll  on  the  ibox  file returns (POLLIN | POLLRDNORM) whenever
              data is available for reading.
              (只要 ibox 文件上有可供读取的数据，对它的 poll 就返回 (POLLIN | POLLRDNORM)。)


   /wbox
       The CPU to SPU communation mailbox. It is write-only and can be written
       in  units  of  32  bits. If the mailbox is full, write() will block and
       poll can be used to wait for it becoming  empty  again.   The  possible
       operations  on  an open wbox file are: write(2) If a count smaller than
       four is requested, write returns -1 and sets errno to EINVAL.  If there
       is  no space available in the mail box and the file descriptor has been
       opened with O_NONBLOCK, the return value is set to -1 and errno becomes
       EAGAIN.
       (CPU 到 SPU 的通信邮箱。它是只写的，可以以 32 位为单位写入。如果邮箱已满，write() 将阻塞，并且可以用 poll 来等待它再次变空。对一个已打开的 wbox 文件可能进行的操作有：write(2)——如果请求的 count 小于四，write 返回 -1 并把 errno 设置为 EINVAL。如果邮箱中没有可用空间且文件描述符是以 O_NONBLOCK 打开的，返回值被设置为 -1，errno 变为 EAGAIN。)

       If  there is no space available in the mail box and the file descriptor
       has been opened without O_NONBLOCK, the call will block until  the  SPU
       reads  from  its PPE mailbox channel.  When data has been read success-
       fully, four bytes are placed in the data buffer and the value  four  is
       returned.
       (如果邮箱中没有可用空间且文件描述符是在没有 O_NONBLOCK 的情况下打开的，该调用将阻塞，直到 SPU 从它的 PPE 邮箱通道读取。当数据被成功读取时，四个字节被放入数据缓冲区，并返回值四。)

       poll(2)
              Poll  on  the  ibox  file returns (POLLOUT | POLLWRNORM) whenever
              space is available for writing.
              (只要文件上有可供写入的空间，对它的 poll 就返回 (POLLOUT | POLLWRNORM)。)


   /mbox_stat, /ibox_stat, /wbox_stat
       Read-only files that contain the length of the current queue, i.e.  how
       many  words  can  be  read  from  mbox or ibox or how many words can be
       written to wbox without blocking.  The files can be read only in 4-byte
       units  and  return  a  big-endian  binary integer number.  The possible
       operations on an open ``*box_stat`` file are:
       (只读文件，包含当前队列的长度，即在不阻塞的情况下可以从 mbox 或 ibox 读取多少个字，或者可以向 wbox 写入多少个字。这些文件只能以 4 字节为单位读取，并返回一个大端序的二进制整数。对一个已打开的 ``*box_stat`` 文件可能进行的操作有：)

       read(2)
              If a count smaller than four is requested, read returns  -1  and
              sets errno to EINVAL.  Otherwise, a four byte value is placed in
              the data buffer, containing the number of elements that  can  be
              read  from  (for  mbox_stat  and  ibox_stat)  or written to (for
              wbox_stat) the respective mail box without blocking or resulting
              in EAGAIN.
              (如果请求的 count 小于四，read 返回 -1 并把 errno 设置为 EINVAL。否则，一个四字节的值被放入数据缓冲区，其中包含在不阻塞或产生 EAGAIN 的情况下，可以从相应邮箱读取的元素数量(对于 mbox_stat 和 ibox_stat)，或可以向其写入的元素数量(对于 wbox_stat)。)


   /npc, /decr, /decr_status, /spu_tag_mask, /event_mask, /srr0
       Internal  registers  of  the SPU. The representation is an ASCII string
       with the numeric value of the next instruction to  be  executed.  These
       can  be  used in read/write mode for debugging, but normal operation of
       programs should not rely on them because access to any of  them  except
       npc requires an SPU context save and is therefore very inefficient.
       (SPU 的内部寄存器。其表示形式是一个 ASCII 字符串，带有要执行的下一条指令的数值。它们可以以读/写模式用于调试，但程序的正常操作不应依赖它们，因为除 npc 之外对其中任何一个的访问都需要进行 SPU 上下文保存，因此效率非常低。)

       The contents of these files are:
       (这些文件的内容为：)

       =================== ===================================
       npc                 Next Program Counter (下一条程序的计数器)
       decr                SPU Decrementer (SPU 递减计数器)
       decr_status         Decrementer Status (递减计数器状态)
       spu_tag_mask        MFC tag mask for SPU DMA (用于 SPU DMA 的 MFC tag 掩码)
       event_mask          Event mask for SPU interrupts (SPU 中断的事件掩码)
       srr0                Interrupt Return address register (中断返回地址寄存器)
       =================== ===================================


       The   possible   operations   on   an   open  npc,  decr,  decr_status,
       spu_tag_mask, event_mask or srr0 file are:
       (对一个已打开的 npc、decr、decr_status、spu_tag_mask、event_mask 或 srr0 文件可能进行的操作有：)

       read(2)
              When the count supplied to the read call  is  shorter  than  the
              required  length for the pointer value plus a newline character,
              subsequent reads from the same file descriptor  will  result  in
              completing  the string, regardless of changes to the register by
              a running SPU task.  When a complete string has been  read,  all
              subsequent read operations will return zero bytes and a new file
              descriptor needs to be opened to read the value again.
              (当提供给 read 调用的 count 小于指针值加上换行符所需的长度时，后续从同一文件描述符的读取将导致补全该字符串，而不管正在运行的 SPU 任务对寄存器做了什么更改。当完整的字符串被读出后，所有后续的读操作都将返回零字节，需要打开一个新的文件描述符才能再次读取该值。)

       write(2)
              A write operation on the file results in setting the register to
              the  value  given  in  the string. The string is parsed from the
              beginning to the first non-numeric character or the end  of  the
              buffer.  Subsequent writes to the same file descriptor overwrite
              the previous setting.
              (对该文件的写操作会把寄存器设置为字符串中给定的值。字符串从开头解析到第一个非数字字符或缓冲区末尾。后续对同一文件描述符的写入会覆盖之前的设置。)


   /fpcr
       This file gives access to the Floating Point Status and Control  Regis-
       ter as a four byte long file. The operations on the fpcr file are:
       (该文件以一个四字节长的文件的形式提供对浮点状态与控制寄存器(Floating Point Status and Control Register)的访问。对 fpcr 文件的操作有：)

       read(2)
              If  a  count smaller than four is requested, read returns -1 and
              sets errno to EINVAL.  Otherwise, a four byte value is placed in
              the data buffer, containing the current value of the fpcr regis-
              ter.
              (如果请求的 count 小于四，read 返回 -1 并把 errno 设置为 EINVAL。否则，一个四字节的值被放入数据缓冲区，其中包含 fpcr 寄存器的当前值。)

       write(2)
              If a count smaller than four is requested, write returns -1  and
              sets  errno  to  EINVAL.  Otherwise, a four byte value is copied
              from the data buffer, updating the value of the fpcr register.
              (如果请求的 count 小于四，write 返回 -1 并把 errno 设置为 EINVAL。否则，会从数据缓冲区复制一个四字节的值，更新 fpcr 寄存器的值。)


   /signal1, /signal2
       The two signal notification channels of an SPU.  These  are  read-write
       files  that  operate  on  a 32 bit word.  Writing to one of these files
       triggers an interrupt on the SPU.  The  value  written  to  the  signal
       files can be read from the SPU through a channel read or from host user
       space through the file.  After the value has been read by the  SPU,  it
       is  reset  to zero.  The possible operations on an open signal1 or sig-
       nal2 file are:
       (SPU 的两个信号通知通道。它们是对一个 32 位字进行操作的读/写文件。向这些文件之一写入会在 SPU 上触发一次中断。写入信号文件的值可以从 SPU 侧通过通道读取，也可以由宿主用户空间通过文件读取。在该值被 SPU 读取之后，它会被重置为零。对一个已打开的 signal1 或 signal2 文件可能进行的操作有：)

       read(2)
              If a count smaller than four is requested, read returns  -1  and
              sets errno to EINVAL.  Otherwise, a four byte value is placed in
              the data buffer, containing the current value of  the  specified
              signal notification register.
              (如果请求的 count 小于四，read 返回 -1 并把 errno 设置为 EINVAL。否则，一个四字节的值被放入数据缓冲区，其中包含指定的信号通知寄存器的当前值。)

       write(2)
              If  a count smaller than four is requested, write returns -1 and
              sets errno to EINVAL.  Otherwise, a four byte  value  is  copied
              from the data buffer, updating the value of the specified signal
              notification register.  The signal  notification  register  will
              either be replaced with the input data or will be updated to the
              bitwise OR of the old value and the input data, depending on the
              contents  of  the  signal1_type,  or  signal2_type respectively,
              file.
              (如果请求的 count 小于四，write 返回 -1 并把 errno 设置为 EINVAL。否则，会从数据缓冲区复制一个四字节的值，更新指定的信号通知寄存器的值。信号通知寄存器要么被输入数据替换，要么被更新为旧值与输入数据按位或的结果，具体取决于 signal1_type 或 signal2_type 文件的内容。)


   /signal1_type, /signal2_type
       These two files change the behavior of the signal1 and signal2  notifi-
       cation  files.  The  contain  a numerical ASCII string which is read as
       either "1" or "0".  In mode 0 (overwrite), the  hardware  replaces  the
       contents of the signal channel with the data that is written to it.  in
       mode 1 (logical OR), the hardware accumulates the bits that are  subse-
       quently written to it.  The possible operations on an open signal1_type
       or signal2_type file are:
       (这两个文件改变 signal1 和 signal2 通知文件的行为。其中包含一个数字 ASCII 字符串，会被读取为 "1" 或 "0"。在模式 0(覆盖)下，硬件用写入的数据替换信号通道的内容。在模式 1(逻辑或)下，硬件会累积随后写入的位。对一个已打开的 signal1_type 或 signal2_type 文件可能进行的操作有：)

       read(2)
              When the count supplied to the read call  is  shorter  than  the
              required  length  for the digit plus a newline character, subse-
              quent reads from the same file descriptor will  result  in  com-
              pleting  the  string.  When a complete string has been read, all
              subsequent read operations will return zero bytes and a new file
              descriptor needs to be opened to read the value again.
              (当提供给 read 调用的 count 小于该数字加上换行符所需的长度时，后续从同一文件描述符的读取将导致补全该字符串。当完整的字符串被读出后，所有后续的读操作都将返回零字节，需要打开一个新的文件描述符才能再次读取该值。)

       write(2)
              A write operation on the file results in setting the register to
              the value given in the string. The string  is  parsed  from  the
              beginning  to  the first non-numeric character or the end of the
              buffer.  Subsequent writes to the same file descriptor overwrite
              the previous setting.
              (对该文件的写操作会把寄存器设置为字符串中给定的值。字符串从开头解析到第一个非数字字符或缓冲区末尾。后续对同一文件描述符的写入会覆盖之前的设置。)


Examples
(示例)
========
       /etc/fstab entry
              none      /spu      spufs     gid=spu   0    0
       (/etc/fstab 条目)


Authors
(作者)
=======
       Arnd  Bergmann  <arndb@de.ibm.com>,  Mark  Nutter <mnutter@us.ibm.com>,
       Ulrich Weigand <Ulrich.Weigand@de.ibm.com>

See Also
(参见)
========
       capabilities(7), close(2), spu_create(2), spu_run(2), spufs(7)
