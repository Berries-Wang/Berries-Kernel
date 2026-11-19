========================================================
Summary of `HDIO_` ioctl calls(HDIO 系列 ioctl 调用一览)
========================================================

- Edward A. Falk <efalk@google.com>

November, 2004

This document attempts to describe the ioctl(2) calls supported by
the HD/IDE layer.  These are by-and-large implemented (as of Linux 5.11)
(本文档试图描述 HD/IDE 层支持的 ioctl(2) 调用。截至 Linux 5.11，它们大
体上都实现在 drivers/ata/libata-scsi.c 中。)
drivers/ata/libata-scsi.c.

ioctl values are listed in <linux/hdreg.h>.  As of this writing, they
are as follows:
(ioctl 值列在 <linux/hdreg.h> 中。撰写本文时，它们如下：)

    ioctls that pass argument pointers to user space:
    (把参数指针传给用户空间的 ioctl：)

	=======================	=======================================
	HDIO_GETGEO		get device geometry
	HDIO_GET_32BIT		get current io_32bit setting
	HDIO_GET_IDENTITY	get IDE identification info
	HDIO_DRIVE_TASKFILE	execute raw taskfile
	HDIO_DRIVE_TASK		execute task and special drive command
	HDIO_DRIVE_CMD		execute a special drive command
	=======================	=======================================

    ioctls that pass non-pointer values:
    (传递非指针值的 ioctl：)

	=======================	=======================================
	HDIO_SET_32BIT		change io_32bit flags
	=======================	=======================================


The information that follows was determined from reading kernel source
code.  It is likely that some corrections will be made over time.
(以下信息来自对内核源码的阅读。随着时间的推移，可能会做一些修正。)

------------------------------------------------------------------------------

General:
(一般说明：)

	Unless otherwise specified, all ioctl calls return 0 on success
	and -1 with errno set to an appropriate value on error.
	(除非另有说明，所有 ioctl 调用成功时返回 0，出错时返回 -1 并把 errno
	设置为适当的值。)

	Unless otherwise specified, all ioctl calls return -1 and set
	errno to EFAULT on a failed attempt to copy data to or from user
	address space.
	(除非另有说明，所有 ioctl 调用在向用户地址空间复制数据或从其复制数据
	失败时，返回 -1 并把 errno 设置为 EFAULT。)

	Unless otherwise specified, all data structures and constants
	are defined in <linux/hdreg.h>
	(除非另有说明，所有数据结构与常量都定义在 <linux/hdreg.h> 中。)

------------------------------------------------------------------------------

HDIO_GETGEO
	get device geometry
	(获取设备几何参数。)


	usage::

	  struct hd_geometry geom;

	  ioctl(fd, HDIO_GETGEO, &geom);


	(用法：)

	inputs:(输入：)
		none
		(无。)



	outputs:(输出：)
		hd_geometry structure containing:
		(hd_geometry 结构，包含：)


	    =========	==================================
	    heads	number of heads
	    sectors	number of sectors/track
	    cylinders	number of cylinders, mod 65536
	    start	starting sector of this partition.
	    =========	==================================


	error returns:(错误返回：)
	  - EINVAL

			if the device is not a disk drive or floppy drive,
			or if the user passes a null pointer
			(如果设备不是磁盘驱动器或软盘驱动器，或用户传入空指针，则返回此错。)


	notes:(注意：)
		Not particularly useful with modern disk drives, whose geometry
		is a polite fiction anyway.  Modern drives are addressed
		purely by sector number nowadays (lba addressing), and the
		drive geometry is an abstraction which is actually subject
		to change.  Currently (as of Nov 2004), the geometry values
		are the "bios" values -- presumably the values the drive had
		when Linux first booted.
		(对现代磁盘驱动器来说它没什么用，现代驱动器的几何参数本来就只是个
		"客气的虚构"。如今现代驱动器纯粹按扇区号寻址(lba 寻址)，驱动器几何
		参数是实际上可能变化的抽象。目前(2004 年 11 月)，几何参数值是 "bios"
		值——大概是 Linux 首次启动时驱动器所具有的值。)

		In addition, the cylinders field of the hd_geometry is an
		unsigned short, meaning that on most architectures, this
		ioctl will not return a meaningful value on drives with more
		than 65535 tracks.
		(此外，hd_geometry 的 cylinders 字段是 unsigned short，这意味着在大多
		数架构上，对于磁道数超过 65535 的驱动器，此 ioctl 不会返回有意义的值。)

		The start field is unsigned long, meaning that it will not
		contain a meaningful value for disks over 219 Gb in size.
		(start 字段是 unsigned long，这意味着对于大小超过 219 Gb 的磁盘，它不会
		包含有意义的值。)



HDIO_GET_IDENTITY
	get IDE identification info
	(获取 IDE 识别信息。)


	usage::

	  unsigned char identity[512];

	  ioctl(fd, HDIO_GET_IDENTITY, identity);

	(用法：)

	inputs:(输入：)
		none
		(无。)



	outputs:(输出：)
		ATA drive identity information.  For full description, see
		the IDENTIFY DEVICE and IDENTIFY PACKET DEVICE commands in
		the ATA specification.
		(ATA 驱动器识别信息。完整描述见 ATA 规范中的 IDENTIFY DEVICE 与
		IDENTIFY PACKET DEVICE 命令。)

	error returns:
	  - EINVAL	Called on a partition instead of the whole disk device (在分区上而不是整个磁盘设备上调用时返回。)
	  - ENOMSG	IDENTIFY DEVICE information not available (IDENTIFY DEVICE 信息不可用时返回。)

	notes:(注意：)
		Returns information that was obtained when the drive was
		probed.  Some of this information is subject to change, and
		this ioctl does not re-probe the drive to update the
		information.
		(返回驱动器被探测时获得的信息。其中一些信息可能变化，而这个 ioctl
		不会重新探测驱动器来更新信息。)

		This information is also available from /proc/ide/hdX/identify
		(该信息也可以从 /proc/ide/hdX/identify 获取。)



HDIO_GET_32BIT
	get current io_32bit setting
	(获取当前 io_32bit 设置。)


	usage::

	  long val;

	  ioctl(fd, HDIO_GET_32BIT, &val);

	(用法：)

	inputs:(输入：)
		none
		(无。)



	outputs:(输出：)
		The value of the current io_32bit setting
		(当前 io_32bit 设置的值。)



	notes:(注意：)
		0=16-bit, 1=32-bit, 2,3 = 32bit+sync
		(0=16 位，1=32 位，2、3=32 位+同步。)



HDIO_DRIVE_TASKFILE
	execute raw taskfile
	(执行原始 taskfile。)


	Note:(注意：)
		If you don't have a copy of the ANSI ATA specification
		handy, you should probably ignore this ioctl.
		(注意：如果你手头没有 ANSI ATA 规范的副本，大概应该忽略这个 ioctl。)

	- Execute an ATA disk command directly by writing the "taskfile"
	  registers of the drive.  Requires ADMIN and RAWIO access
	  privileges.
	  (通过直接写入驱动器的 "taskfile" 寄存器来执行 ATA 磁盘命令。需要 ADMIN
	  与 RAWIO 访问权限。)

	usage::

	  struct {

	    ide_task_request_t req_task;
	    u8 outbuf[OUTPUT_SIZE];
	    u8 inbuf[INPUT_SIZE];
	  } task;
	  memset(&task.req_task, 0, sizeof(task.req_task));
	  task.req_task.out_size = sizeof(task.outbuf);
	  task.req_task.in_size = sizeof(task.inbuf);
	  ...
	  ioctl(fd, HDIO_DRIVE_TASKFILE, &task);
	  ...

	(用法：)

	inputs:(输入：)

	  (See below for details on memory area passed to ioctl.)
	  (传入 ioctl 的内存区域的细节见下文。)

	  ============	===================================================
	  io_ports[8]	values to be written to taskfile registers
	  hob_ports[8]	high-order bytes, for extended commands.
	  out_flags	flags indicating which registers are valid
	  in_flags	flags indicating which registers should be returned
	  data_phase	see below
	  req_cmd	command type to be executed
	  out_size	size of output buffer
	  outbuf	buffer of data to be transmitted to disk
	  inbuf		buffer of data to be received from disk (see [1])
	  ============	===================================================

	outputs:(输出：)

	  ===========	====================================================
	  io_ports[]	values returned in the taskfile registers
	  hob_ports[]	high-order bytes, for extended commands.
	  out_flags	flags indicating which registers are valid (see [2])
	  in_flags	flags indicating which registers should be returned
	  outbuf	buffer of data to be transmitted to disk (see [1])
	  inbuf		buffer of data to be received from disk
	  ===========	====================================================

	error returns:(错误返回：)
	  - EACCES	CAP_SYS_ADMIN or CAP_SYS_RAWIO privilege not set. (未设置 CAP_SYS_ADMIN 或 CAP_SYS_RAWIO 特权。)
	  - ENOMSG	Device is not a disk drive. (设备不是磁盘驱动器。)
	  - ENOMEM	Unable to allocate memory for task (无法为 task 分配内存。)
	  - EFAULT	req_cmd == TASKFILE_IN_OUT (not implemented as of 2.6.8) (req_cmd == TASKFILE_IN_OUT(截至 2.6.8 尚未实现)。)
	  - EPERM

			req_cmd == TASKFILE_MULTI_OUT and drive
			multi-count not yet set.
			(req_cmd == TASKFILE_MULTI_OUT 且驱动的 multi-count 尚未设置时返回。)
	  - EIO		Drive failed the command. (驱动器命令失败。)

	notes:(注意：)

	  [1] READ THE FOLLOWING NOTES *CAREFULLY*.  THIS IOCTL IS
	  FULL OF GOTCHAS.  Extreme caution should be used with using
	  this ioctl.  A mistake can easily corrupt data or hang the
	  system.
	  (使用这个 ioctl 时必须极度小心。一个错误就很容易损坏数据或挂起系统。)

	  [2] Both the input and output buffers are copied from the
	  user and written back to the user, even when not used.
	  (输入与输出缓冲区都会从用户复制并写回用户，即使未被使用也是如此。)

	  [3] If one or more bits are set in out_flags and in_flags is
	  zero, the following values are used for in_flags.all and
	  written back into in_flags on completion.
	  (如果 out_flags 中置位了一个或多个位而 in_flags 为零，则完成时会将下
	  列值用作 in_flags.all 并写回 in_flags。)

	   * IDE_TASKFILE_STD_IN_FLAGS | (IDE_HOB_STD_IN_FLAGS << 8)
	     if LBA48 addressing is enabled for the drive
	   * IDE_TASKFILE_STD_IN_FLAGS
	     if CHS/LBA28

	  The association between in_flags.all and each enable
	  bitfield flips depending on endianness; fortunately, TASKFILE
	  only uses inflags.b.data bit and ignores all other bits.
	  The end result is that, on any endian machines, it has no
	  effect other than modifying in_flags on completion.
	  (in_flags.all 与各使能位域之间的关联随字节序而翻转；幸运的是，
	  TASKFILE 只用 inflags.b.data 位而忽略所有其他位。最终结果是，在任何字
	  节序的机器上，除了完成时修改 in_flags 之外没有其他影响。)

	  [4] The default value of SELECT is (0xa0|DEV_bit|LBA_bit)
	  except for four drives per port chipsets.  For four drives
	  per port chipsets, it's (0xa0|DEV_bit|LBA_bit) for the first
	  pair and (0x80|DEV_bit|LBA_bit) for the second pair.
	  (SELECT 的默认值是 (0xa0|DEV_bit|LBA_bit)，每端口四驱动器的芯片组除
	  外。对每端口四驱动器的芯片组，第一对是 (0xa0|DEV_bit|LBA_bit)，第二对
	  是 (0x80|DEV_bit|LBA_bit)。)

	  [5] The argument to the ioctl is a pointer to a region of
	  memory containing a ide_task_request_t structure, followed
	  by an optional buffer of data to be transmitted to the
	  drive, followed by an optional buffer to receive data from
	  the drive.
	  (ioctl 的参数是一个指向内存区域的指针：先是 ide_task_request_t 结
	  构，随后是可选的待发送给驱动器的数据缓冲区，再随后是可选的用于接收驱
	  动器数据的缓冲区。)

	  Command is passed to the disk drive via the ide_task_request_t
	  structure, which contains these fields:
	  (命令通过 ide_task_request_t 结构传给磁盘驱动器，该结构包含以下字
	  段：)

	    ============	===============================================
	    io_ports[8]		values for the taskfile registers
	    hob_ports[8]	high-order bytes, for extended commands
	    out_flags		flags indicating which entries in the
				io_ports[] and hob_ports[] arrays
				contain valid values.  Type ide_reg_valid_t.
	    in_flags		flags indicating which entries in the
				io_ports[] and hob_ports[] arrays
				are expected to contain valid values
				on return.
	    data_phase		See below
	    req_cmd		Command type, see below
	    out_size		output (user->drive) buffer size, bytes
	    in_size		input (drive->user) buffer size, bytes
	    ============	===============================================

	  When out_flags is zero, the following registers are loaded.
	  (当 out_flags 为零时，装入以下寄存器。)

	    ============	===============================================
	    HOB_FEATURE		If the drive supports LBA48
	    HOB_NSECTOR		If the drive supports LBA48
	    HOB_SECTOR		If the drive supports LBA48
	    HOB_LCYL		If the drive supports LBA48
	    HOB_HCYL		If the drive supports LBA48
	    FEATURE
	    NSECTOR
	    SECTOR
	    LCYL
	    HCYL
	    SELECT		First, masked with 0xE0 if LBA48, 0xEF
				otherwise; then, or'ed with the default
				value of SELECT.
	    ============	===============================================

	  If any bit in out_flags is set, the following registers are loaded.
	  (如果 out_flags 的任何位置位，装入以下寄存器。)

	    ============	===============================================
	    HOB_DATA		If out_flags.b.data is set.  HOB_DATA will
				travel on DD8-DD15 on little endian machines
				and on DD0-DD7 on big endian machines.
	    DATA		If out_flags.b.data is set.  DATA will
				travel on DD0-DD7 on little endian machines
				and on DD8-DD15 on big endian machines.
	    HOB_NSECTOR		If out_flags.b.nsector_hob is set
	    HOB_SECTOR		If out_flags.b.sector_hob is set
	    HOB_LCYL		If out_flags.b.lcyl_hob is set
	    HOB_HCYL		If out_flags.b.hcyl_hob is set
	    FEATURE		If out_flags.b.feature is set
	    NSECTOR		If out_flags.b.nsector is set
	    SECTOR		If out_flags.b.sector is set
	    LCYL		If out_flags.b.lcyl is set
	    HCYL		If out_flags.b.hcyl is set
	    SELECT		Or'ed with the default value of SELECT and
				loaded regardless of out_flags.b.select.
	    ============	===============================================

	  Taskfile registers are read back from the drive into
	  {io|hob}_ports[] after the command completes iff one of the
	  following conditions is met; otherwise, the original values
	  will be written back, unchanged.
	  (命令完成之后，当且仅当满足下列条件之一时，taskfile 寄存器才从驱动器
	  读回 {io|hob}_ports[]；否则原值将原样写回。)

	    1. The drive fails the command (EIO).
	    2. One or more than one bits are set in out_flags.
	    3. The requested data_phase is TASKFILE_NO_DATA.

	    ============	===============================================
	    HOB_DATA		If in_flags.b.data is set.  It will contain
				DD8-DD15 on little endian machines and
				DD0-DD7 on big endian machines.
	    DATA		If in_flags.b.data is set.  It will contain
				DD0-DD7 on little endian machines and
				DD8-DD15 on big endian machines.
	    HOB_FEATURE		If the drive supports LBA48
	    HOB_NSECTOR		If the drive supports LBA48
	    HOB_SECTOR		If the drive supports LBA48
	    HOB_LCYL		If the drive supports LBA48
	    HOB_HCYL		If the drive supports LBA48
	    NSECTOR
	    SECTOR
	    LCYL
	    HCYL
	    ============	===============================================

	  The data_phase field describes the data transfer to be
	  performed.  Value is one of:
	  (data_phase 字段描述要执行的数据传输。取以下值之一：)

	    ===================        ========================================
	    TASKFILE_IN
	    TASKFILE_MULTI_IN
	    TASKFILE_OUT
	    TASKFILE_MULTI_OUT
	    TASKFILE_IN_OUT
	    TASKFILE_IN_DMA
	    TASKFILE_IN_DMAQ		== IN_DMA (queueing not supported)
	    TASKFILE_OUT_DMA
	    TASKFILE_OUT_DMAQ		== OUT_DMA (queueing not supported)
	    TASKFILE_P_IN		unimplemented
	    TASKFILE_P_IN_DMA		unimplemented
	    TASKFILE_P_IN_DMAQ		unimplemented
	    TASKFILE_P_OUT		unimplemented
	    TASKFILE_P_OUT_DMA		unimplemented
	    TASKFILE_P_OUT_DMAQ		unimplemented
	    ===================        ========================================

	  The req_cmd field classifies the command type.  It may be
	  one of:
	  (req_cmd 字段对命令类型分类。可以是以下之一：)

	    ========================    =======================================
	    IDE_DRIVE_TASK_NO_DATA
	    IDE_DRIVE_TASK_SET_XFER	unimplemented
	    IDE_DRIVE_TASK_IN
	    IDE_DRIVE_TASK_OUT		unimplemented
	    IDE_DRIVE_TASK_RAW_WRITE
	    ========================    =======================================

	  [6] Do not access {in|out}_flags->all except for resetting
	  all the bits.  Always access individual bit fields.  ->all
	  value will flip depending on endianness.  For the same
	  reason, do not use IDE_{TASKFILE|HOB}_STD_{OUT|IN}_FLAGS
	  constants defined in hdreg.h.
	  (除了整体清零之外，不要访问 {in|out}_flags->all。始终访问单个位域。
	  ->all 的值随字节序而翻转。出于同样原因，不要使用 hdreg.h 中定义的
	  IDE_{TASKFILE|HOB}_STD_{OUT|IN}_FLAGS 常量。)



HDIO_DRIVE_CMD
	execute a special drive command
	(执行特殊驱动器命令。)


	Note:  If you don't have a copy of the ANSI ATA specification
	handy, you should probably ignore this ioctl. (注意：如果你手头没有 ANSI
	ATA 规范的副本，大概应该忽略这个 ioctl。)

	usage::

	  u8 args[4+XFER_SIZE];

	  ...
	  ioctl(fd, HDIO_DRIVE_CMD, args);

	(用法：)

	inputs:(输入：)
	    Commands other than WIN_SMART: (WIN_SMART 以外的命令：)

	    =======     =======
	    args[0]	COMMAND
	    args[1]	NSECTOR
	    args[2]	FEATURE
	    args[3]	NSECTOR
	    =======     =======

	    WIN_SMART:

	    =======     =======
	    args[0]	COMMAND
	    args[1]	SECTOR
	    args[2]	FEATURE
	    args[3]	NSECTOR
	    =======     =======

	outputs:(输出：)
		args[] buffer is filled with register values followed by any
		(args[] 缓冲区先被填入寄存器值，随后是磁盘返回的数据。)


	  data returned by the disk.

	    ========	====================================================
	    args[0]	status
	    args[1]	error
	    args[2]	NSECTOR
	    args[3]	undefined
	    args[4+]	NSECTOR * 512 bytes of data returned by the command.
	    ========	====================================================

	error returns:(错误返回：)
	  - EACCES	Access denied:  requires CAP_SYS_RAWIO (访问被拒绝：需要 CAP_SYS_RAWIO。)
	  - ENOMEM	Unable to allocate memory for task (无法为 task 分配内存。)
	  - EIO		Drive reports error (驱动器报告错误。)

	notes:(注意：)

	  [1] For commands other than WIN_SMART, args[1] should equal
	  args[3].  SECTOR, LCYL and HCYL are undefined.  For
	  WIN_SMART, 0x4f and 0xc2 are loaded into LCYL and HCYL
	  respectively.  In both cases SELECT will contain the default
	  value for the drive.  Please refer to HDIO_DRIVE_TASKFILE
	  notes for the default value of SELECT.
	  (对 WIN_SMART 以外的命令，args[1] 应等于 args[3]。SECTOR、LCYL 与
	  HCYL 未定义。对 WIN_SMART，0x4f 与 0xc2 分别装入 LCYL 与 HCYL。两种情
	  况下 SELECT 都会包含该驱动器的默认值。SELECT 默认值请参见
	  HDIO_DRIVE_TASKFILE 的注意事项。)

	  [2] If NSECTOR value is greater than zero and the drive sets
	  DRQ when interrupting for the command, NSECTOR * 512 bytes
	  are read from the device into the area following NSECTOR.
	  In the above example, the area would be
	  args[4..4+XFER_SIZE].  16bit PIO is used regardless of
	  HDIO_SET_32BIT setting.
	  (如果 NSECTOR 值大于零且驱动器在为该命令中断时置 DRQ，则会从设备读取
	  NSECTOR * 512 字节到 NSECTOR 之后的区域。在上面的例子中，该区域是
	  args[4..4+XFER_SIZE]。无论 HDIO_SET_32BIT 设置如何，都使用 16 位 PIO。)

	  [3] If COMMAND == WIN_SETFEATURES && FEATURE == SETFEATURES_XFER
	  && NSECTOR >= XFER_SW_DMA_0 && the drive supports any DMA
	  mode, IDE driver will try to tune the transfer mode of the
	  drive accordingly.
	  (如果 COMMAND == WIN_SETFEATURES 且 FEATURE == SETFEATURES_XFER 且
	  NSECTOR >= XFER_SW_DMA_0 且驱动器支持任何 DMA 模式，IDE 驱动器会相应地
	  尝试调整驱动器的传输模式。)



HDIO_DRIVE_TASK
	execute task and special drive command
	(执行 task 与特殊驱动器命令。)


	Note:  If you don't have a copy of the ANSI ATA specification
	handy, you should probably ignore this ioctl. (注意：如果你手头没有 ANSI
	ATA 规范的副本，大概应该忽略这个 ioctl。)

	usage::

	  u8 args[7];

	  ...
	  ioctl(fd, HDIO_DRIVE_TASK, args);

	(用法：)

	inputs:(输入：)
	    Taskfile register values: (taskfile 寄存器值：)

	    =======	=======
	    args[0]	COMMAND
	    args[1]	FEATURE
	    args[2]	NSECTOR
	    args[3]	SECTOR
	    args[4]	LCYL
	    args[5]	HCYL
	    args[6]	SELECT
	    =======	=======

	outputs:(输出：)
	    Taskfile register values: (taskfile 寄存器值：)


	    =======	=======
	    args[0]	status
	    args[1]	error
	    args[2]	NSECTOR
	    args[3]	SECTOR
	    args[4]	LCYL
	    args[5]	HCYL
	    args[6]	SELECT
	    =======	=======

	error returns:(错误返回：)
	  - EACCES	Access denied:  requires CAP_SYS_RAWIO (访问被拒绝：需要 CAP_SYS_RAWIO。)
	  - ENOMEM	Unable to allocate memory for task (无法为 task 分配内存。)
	  - ENOMSG	Device is not a disk drive. (设备不是磁盘驱动器。)
	  - EIO		Drive failed the command. (驱动器命令失败。)

	notes:(注意：)

	  [1] DEV bit (0x10) of SELECT register is ignored and the
	  appropriate value for the drive is used.  All other bits
	  are used unaltered.
	  (SELECT 寄存器的 DEV 位 (0x10) 被忽略，使用适合该驱动器的值。所有其他
	  位原样使用。)



HDIO_SET_32BIT
	change io_32bit flags
	(改变 io_32bit 标志。)


	usage::

	  int val;

	  ioctl(fd, HDIO_SET_32BIT, val);

	(用法：)

	inputs:(输入：)
		New value for io_32bit flag
		(io_32bit 标志的新值。)



	outputs:(输出：)
		none
		(无。)



	error return:(错误返回：)
	  - EINVAL	Called on a partition instead of the whole disk device (在分区上而不是整个磁盘设备上调用时返回。)
	  - EACCES	Access denied:  requires CAP_SYS_ADMIN (访问被拒绝：需要 CAP_SYS_ADMIN。)
	  - EINVAL	value out of range [0 3] (值超出范围 [0 3]。)
	  - EBUSY	Controller busy (控制器忙。)
