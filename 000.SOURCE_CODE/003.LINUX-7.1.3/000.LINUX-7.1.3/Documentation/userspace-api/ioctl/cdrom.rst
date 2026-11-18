==================================================
Summary of CDROM ioctl calls(CDROM ioctl 调用一览)
==================================================

- Edward A. Falk <efalk@google.com>
(作者：Edward A. Falk <efalk@google.com>)

November, 2004
(2004 年 11 月)

This document attempts to describe the ioctl(2) calls supported by
the CDROM layer.  These are by-and-large implemented (as of Linux 2.6)
in drivers/cdrom/cdrom.c and drivers/block/scsi_ioctl.c
(本文档试图描述 CDROM 层所支持的 ioctl(2) 调用。截至 Linux 2.6，它们大体上实现在 drivers/cdrom/cdrom.c 和 drivers/block/scsi_ioctl.c 中。)

ioctl values are listed in <linux/cdrom.h>.  As of this writing, they
are as follows:
(ioctl 的取值列在 <linux/cdrom.h> 中。撰写本文时，它们如下：)

	========================  ===============================================
	CDROMPAUSE		  Pause Audio Operation(暂停音频操作。)
	CDROMRESUME		  Resume paused Audio Operation(恢复被暂停的音频操作。)
	CDROMPLAYMSF		  Play Audio MSF (struct cdrom_msf)(播放音频 MSF (struct cdrom_msf)。)
	CDROMPLAYTRKIND		  Play Audio Track/index (struct cdrom_ti)(播放音频轨道/索引 (struct cdrom_ti)。)
	CDROMREADTOCHDR		  Read TOC header (struct cdrom_tochdr)(读取 TOC 头 (struct cdrom_tochdr)。)
	CDROMREADTOCENTRY	  Read TOC entry (struct cdrom_tocentry)(读取 TOC 表项 (struct cdrom_tocentry)。)
	CDROMSTOP		  Stop the cdrom drive(停止光驱。)
	CDROMSTART		  Start the cdrom drive(启动光驱。)
	CDROMEJECT		  Ejects the cdrom media(弹出光盘介质。)
	CDROMVOLCTRL		  Control output volume (struct cdrom_volctrl)(控制输出音量 (struct cdrom_volctrl)。)
	CDROMSUBCHNL		  Read subchannel data (struct cdrom_subchnl)(读取子通道数据 (struct cdrom_subchnl)。)
	CDROMREADMODE2		  Read CDROM mode 2 data (2336 Bytes)
				  (struct cdrom_read)(读取 CDROM 模式 2 数据 (2336 字节) (struct cdrom_read)。)
	CDROMREADMODE1		  Read CDROM mode 1 data (2048 Bytes)
				  (struct cdrom_read)(读取 CDROM 模式 1 数据 (2048 字节) (struct cdrom_read)。)
	CDROMREADAUDIO		  (struct cdrom_read_audio)(读取音频数据 (struct cdrom_read_audio)。)
	CDROMEJECT_SW		  enable(1)/disable(0) auto-ejecting(启用 (1)/禁用 (0) 自动弹出。)
	CDROMMULTISESSION	  Obtain the start-of-last-session
				  address of multi session disks
				  (struct cdrom_multisession)(获取多区段 (multi session) 光盘最后一次区段的起始地址 (struct cdrom_multisession)。)
	CDROM_GET_MCN		  Obtain the "Universal Product Code"
				  if available (struct cdrom_mcn)(获取"通用产品代码" (Universal Product Code)，如果有的话 (struct cdrom_mcn)。)
	CDROM_GET_UPC		  Deprecated, use CDROM_GET_MCN instead.(已弃用，请改用 CDROM_GET_MCN。)
	CDROMRESET		  hard-reset the drive(硬复位光驱。)
	CDROMVOLREAD		  Get the drive's volume setting
				  (struct cdrom_volctrl)(获取光驱的音量设置 (struct cdrom_volctrl)。)
	CDROMREADRAW		  read data in raw mode (2352 Bytes)
				  (struct cdrom_read)(以原始 (raw) 模式读取数据 (2352 字节) (struct cdrom_read)。)
	CDROMREADCOOKED		  read data in cooked mode(以整理 (cooked) 模式读取数据。)
	CDROMSEEK		  seek msf address(寻道到 MSF 地址。)
	CDROMPLAYBLK		  scsi-cd only, (struct cdrom_blk)(仅限 SCSI 光驱 (struct cdrom_blk)。)
	CDROMREADALL		  read all 2646 bytes(读取全部 2646 字节。)
	CDROMGETSPINDOWN	  return 4-bit spindown value(返回 4 位的降速 (spindown) 值。)
	CDROMSETSPINDOWN	  set 4-bit spindown value(设置 4 位的降速 (spindown) 值。)
	CDROMCLOSETRAY		  pendant of CDROMEJECT(CDROMEJECT 的姊妹命令。)
	CDROM_SET_OPTIONS	  Set behavior options(设置行为选项。)
	CDROM_CLEAR_OPTIONS	  Clear behavior options(清除行为选项。)
	CDROM_SELECT_SPEED	  Set the CD-ROM speed(设置 CD-ROM 速度。)
	CDROM_SELECT_DISC	  Select disc (for juke-boxes)(选择光盘 (用于点唱机式设备)。)
	CDROM_MEDIA_CHANGED	  Check is media changed(检查介质是否更换过。)
	CDROM_TIMED_MEDIA_CHANGE  Check if media changed
				  since given time
				  (struct cdrom_timed_media_change_info)(检查介质自给定时间以来是否更换过 (struct cdrom_timed_media_change_info)。)
	CDROM_DRIVE_STATUS	  Get tray position, etc.(获取托盘位置等信息。)
	CDROM_DISC_STATUS	  Get disc type, etc.(获取光盘类型等信息。)
	CDROM_CHANGER_NSLOTS	  Get number of slots(获取槽位数量。)
	CDROM_LOCKDOOR		  lock or unlock door(锁定或解锁仓门。)
	CDROM_DEBUG		  Turn debug messages on/off(打开/关闭调试消息。)
	CDROM_GET_CAPABILITY	  get capabilities(获取能力。)
	CDROMAUDIOBUFSIZ	  set the audio buffer size(设置音频缓冲区大小。)
	DVD_READ_STRUCT		  Read structure(读取结构。)
	DVD_WRITE_STRUCT	  Write structure(写入结构。)
	DVD_AUTH		  Authentication(认证。)
	CDROM_SEND_PACKET	  send a packet to the drive(向光驱发送一个数据包。)
	CDROM_NEXT_WRITABLE	  get next writable block(获取下一个可写块。)
	CDROM_LAST_WRITTEN	  get last block written on disc(获取光盘上最后写入的块。)
	========================  ===============================================


The information that follows was determined from reading kernel source
code.  It is likely that some corrections will be made over time.

------------------------------------------------------------------------------

General:
(总则：)

	Unless otherwise specified, all ioctl calls return 0 on success
	and -1 with errno set to an appropriate value on error.  (Some
	ioctls return non-negative data values.)
	(除非另有说明，所有 ioctl 调用成功时返回 0，出错时返回 -1 并把 errno 置为适当的值。(有些 ioctl 会返回非负的数据值。))

	Unless otherwise specified, all ioctl calls return -1 and set
	errno to EFAULT on a failed attempt to copy data to or from user
	address space.
	(除非另有说明，所有 ioctl 调用在向用户地址空间复制数据或从其复制数据失败时，返回 -1 并把 errno 置为 EFAULT。)

	Individual drivers may return error codes not listed here.
	(各个驱动可能返回此处未列出的错误码。)

	Unless otherwise specified, all data structures and constants
	are defined in <linux/cdrom.h>
	(除非另有说明，所有数据结构和常量都定义在 <linux/cdrom.h> 中)

------------------------------------------------------------------------------


CDROMPAUSE
	Pause Audio Operation
	(暂停音频操作。)


	usage::

	  ioctl(fd, CDROMPAUSE, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)


CDROMRESUME
	Resume paused Audio Operation
	(恢复被暂停的音频操作。)


	usage::

	  ioctl(fd, CDROMRESUME, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)


CDROMPLAYMSF
	Play Audio MSF
	(播放音频 MSF)

	(struct cdrom_msf)
	((struct cdrom_msf))


	usage::

	  struct cdrom_msf msf;

	  ioctl(fd, CDROMPLAYMSF, &msf);

	(用法：)

	inputs:
		cdrom_msf structure, describing a segment of music to play
		(cdrom_msf 结构体，描述要播放的一段音乐。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)

	notes:
		- MSF stands for minutes-seconds-frames(MSF 代表分-秒-帧。)
		- LBA stands for logical block address(LBA 代表逻辑块地址。)
		- Segment is described as start and end times, where each time
		  is described as minutes:seconds:frames.
		  A frame is 1/75 of a second.
		  (片段由开始和结束时间描述，每个时间以 分:秒:帧 表示。一帧为 1/75 秒。)


CDROMPLAYTRKIND
	Play Audio Track/index
	(播放音频轨道/索引)

	(struct cdrom_ti)
	((struct cdrom_ti))


	usage::

	  struct cdrom_ti ti;

	  ioctl(fd, CDROMPLAYTRKIND, &ti);

	(用法：)

	inputs:
		cdrom_ti structure, describing a segment of music to play
		(cdrom_ti 结构体，描述要播放的一段音乐。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)

	notes:
		- Segment is described as start and end times, where each time
		  is described as a track and an index.
		  (片段由开始和结束时间描述，每个时间以一条轨道和一个索引表示。)



CDROMREADTOCHDR
	Read TOC header
	(读取 TOC 头)

	(struct cdrom_tochdr)
	((struct cdrom_tochdr))


	usage::

	  cdrom_tochdr header;

	  ioctl(fd, CDROMREADTOCHDR, &header);

	(用法：)

	inputs:
		cdrom_tochdr structure
		(cdrom_tochdr 结构体。)


	outputs:
		cdrom_tochdr structure
		(cdrom_tochdr 结构体。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)



CDROMREADTOCENTRY
	Read TOC entry
	(读取 TOC 表项)

	(struct cdrom_tocentry)
	((struct cdrom_tocentry))


	usage::

	  struct cdrom_tocentry entry;

	  ioctl(fd, CDROMREADTOCENTRY, &entry);

	(用法：)

	inputs:
		cdrom_tocentry structure
		(cdrom_tocentry 结构体。)


	outputs:
		cdrom_tocentry structure
		(cdrom_tocentry 结构体。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)
	  - EINVAL	entry.cdte_format not CDROM_MSF or CDROM_LBA(EINVAL：entry.cdte_format 不是 CDROM_MSF 或 CDROM_LBA)
	  - EINVAL	requested track out of bounds(EINVAL：所请求的轨道超出范围)
	  - EIO		I/O error reading TOC(EIO：读取 TOC 时发生 I/O 错误)

	notes:
		- TOC stands for Table Of Contents(TOC 代表目录表。)
		- MSF stands for minutes-seconds-frames(MSF 代表分-秒-帧。)
		- LBA stands for logical block address(LBA 代表逻辑块地址。)



CDROMSTOP
	Stop the cdrom drive
	(停止光驱)


	usage::

	  ioctl(fd, CDROMSTOP, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)

	notes:
	  - Exact interpretation of this ioctl depends on the device,
	    but most seem to spin the drive down.
	    (这个 ioctl 的确切解释取决于设备，但大多数设备似乎会让驱动器降速停转。)


CDROMSTART
	Start the cdrom drive
	(启动光驱)


	usage::

	  ioctl(fd, CDROMSTART, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)

	notes:
	  - Exact interpretation of this ioctl depends on the device,
	    but most seem to spin the drive up and/or close the tray.
	    Other devices ignore the ioctl completely.
	    (这个 ioctl 的确切解释取决于设备，但大多数设备似乎会让驱动器起转和/或关上托盘。其他设备则完全忽略这个 ioctl。)


CDROMEJECT
	- Ejects the cdrom media
	(- 弹出光盘介质)


	usage::

	  ioctl(fd, CDROMEJECT, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error returns:
	  - ENOSYS	cd drive not capable of ejecting(光驱不具备弹出能力)
	  - EBUSY	other processes are accessing drive, or door is locked(其他进程正在访问光驱，或仓门已锁定)

	notes:
		- See CDROM_LOCKDOOR, below.(见下文的 CDROM_LOCKDOOR。)




CDROMCLOSETRAY
	pendant of CDROMEJECT
	(CDROMEJECT 的姊妹命令)


	usage::

	  ioctl(fd, CDROMCLOSETRAY, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error returns:
	  - ENOSYS	cd drive not capable of closing the tray(光驱不具备关闭托盘的能力)
	  - EBUSY	other processes are accessing drive, or door is locked(其他进程正在访问光驱，或仓门已锁定)

	notes:
		- See CDROM_LOCKDOOR, below.(见下文的 CDROM_LOCKDOOR。)




CDROMVOLCTRL
	Control output volume (struct cdrom_volctrl)
	(控制输出音量 (struct cdrom_volctrl))


	usage::

	  struct cdrom_volctrl volume;

	  ioctl(fd, CDROMVOLCTRL, &volume);

	(用法：)

	inputs:
		cdrom_volctrl structure containing volumes for up to 4
		channels.
		(cdrom_volctrl 结构体，包含最多 4 个通道的音量。)

	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)



CDROMVOLREAD
	Get the drive's volume setting
	(获取光驱的音量设置)

	(struct cdrom_volctrl)
	((struct cdrom_volctrl))


	usage::

	  struct cdrom_volctrl volume;

	  ioctl(fd, CDROMVOLREAD, &volume);

	(用法：)

	inputs:
		none
		(无。)


	outputs:
		The current volume settings.
		(当前的音量设置。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)



CDROMSUBCHNL
	Read subchannel data
	(读取子通道数据)

	(struct cdrom_subchnl)
	((struct cdrom_subchnl))


	usage::

	  struct cdrom_subchnl q;

	  ioctl(fd, CDROMSUBCHNL, &q);

	(用法：)

	inputs:
		cdrom_subchnl structure
		(cdrom_subchnl 结构体。)


	outputs:
		cdrom_subchnl structure
		(cdrom_subchnl 结构体。)


	error return:
	  - ENOSYS	cd drive not audio-capable.(光驱不具备音频能力。)
	  - EINVAL	format not CDROM_MSF or CDROM_LBA(EINVAL：格式不是 CDROM_MSF 或 CDROM_LBA)

	notes:
		- Format is converted to CDROM_MSF or CDROM_LBA
		  as per user request on return
		  (返回时，格式会按用户的请求转换为 CDROM_MSF 或 CDROM_LBA。)



CDROMREADRAW
	read data in raw mode (2352 Bytes)
	(以原始 (raw) 模式读取数据 (2352 字节))

	(struct cdrom_read)
	((struct cdrom_read))

	usage::

	  union {

	    struct cdrom_msf msf;		/* input */
	    char buffer[CD_FRAMESIZE_RAW];	/* return */
	  } arg;
	  ioctl(fd, CDROMREADRAW, &arg);

	(用法：)

	inputs:
		cdrom_msf structure indicating an address to read.
		(cdrom_msf 结构体，指示要读取的地址。)

		Only the start values are significant.
		(只有起始值有意义。)

	outputs:
		Data written to address provided by user.
		(数据写入用户提供的地址。)


	error return:
	  - EINVAL	address less than 0, or msf less than 0:2:0(EINVAL：地址小于 0，或 msf 小于 0:2:0)
	  - ENOMEM	out of memory(内存不足)

	notes:
		- As of 2.6.8.1, comments in <linux/cdrom.h> indicate that this
		  ioctl accepts a cdrom_read structure, but actual source code
		  reads a cdrom_msf structure and writes a buffer of data to
		  the same address.
		  (从 2.6.8.1 起，<linux/cdrom.h> 中的注释表明这个 ioctl 接受一个 cdrom_read 结构体，但实际源码读取的是一个 cdrom_msf 结构体，并把数据缓冲区写到同一地址。)

		- MSF values are converted to LBA values via this formula::

		    lba = (((m * CD_SECS) + s) * CD_FRAMES + f) - CD_MSF_OFFSET;

		  (MSF 值通过上面的公式转换为 LBA 值。)




CDROMREADMODE1
	Read CDROM mode 1 data (2048 Bytes)
	(读取 CDROM 模式 1 数据 (2048 字节))

	(struct cdrom_read)
	((struct cdrom_read))

	notes:
		Identical to CDROMREADRAW except that block size is
		CD_FRAMESIZE (2048) bytes
		(与 CDROMREADRAW 相同，只是块大小为 CD_FRAMESIZE (2048) 字节)



CDROMREADMODE2
	Read CDROM mode 2 data (2336 Bytes)
	(读取 CDROM 模式 2 数据 (2336 字节))

	(struct cdrom_read)
	((struct cdrom_read))

	notes:
		Identical to CDROMREADRAW except that block size is
		CD_FRAMESIZE_RAW0 (2336) bytes
		(与 CDROMREADRAW 相同，只是块大小为 CD_FRAMESIZE_RAW0 (2336) 字节)



CDROMREADAUDIO
	(struct cdrom_read_audio)
	(读取音频数据 ((struct cdrom_read_audio)))

	usage::

	  struct cdrom_read_audio ra;

	  ioctl(fd, CDROMREADAUDIO, &ra);

	(用法：)

	inputs:
		cdrom_read_audio structure containing read start
		point and length
		(cdrom_read_audio 结构体，包含读取起始点和长度。)

	outputs:
		audio data, returned to buffer indicated by ra
		(音频数据，返回到 ra 所指示的缓冲区中。)


	error return:
	  - EINVAL	format not CDROM_MSF or CDROM_LBA(EINVAL：格式不是 CDROM_MSF 或 CDROM_LBA)
	  - EINVAL	nframes not in range [1 75](EINVAL：nframes 不在范围 [1 75] 内)
	  - ENXIO	drive has no queue (probably means invalid fd)(ENXIO：光驱没有队列 (可能意味着 fd 无效))
	  - ENOMEM	out of memory(ENOMEM：内存不足)


CDROMEJECT_SW
	enable(1)/disable(0) auto-ejecting
	(启用 (1)/禁用 (0) 自动弹出)


	usage::

	  int val;

	  ioctl(fd, CDROMEJECT_SW, val);

	(用法：)

	inputs:
		Flag specifying auto-eject flag.
		(指定自动弹出标志的旗标值。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	Drive is not capable of ejecting.(光驱不具备弹出能力。)
	  - EBUSY	Door is locked(仓门已锁定)




CDROMMULTISESSION
	Obtain the start-of-last-session address of multi session disks
	(获取多区段光盘最后一次区段的起始地址)

	(struct cdrom_multisession)
	((struct cdrom_multisession))

	usage::

	  struct cdrom_multisession ms_info;

	  ioctl(fd, CDROMMULTISESSION, &ms_info);

	(用法：)

	inputs:
		cdrom_multisession structure containing desired

	  format.
	  (cdrom_multisession 结构体，包含所需的格式。)

	outputs:
		cdrom_multisession structure is filled with last_session
		information.
		(cdrom_multisession 结构体被填入 last_session 信息。)

	error return:
	  - EINVAL	format not CDROM_MSF or CDROM_LBA(EINVAL：格式不是 CDROM_MSF 或 CDROM_LBA)


CDROM_GET_MCN
	Obtain the "Universal Product Code"
	if available
	(获取"通用产品代码" (Universal Product Code)，如果有的话)

	(struct cdrom_mcn)
	((struct cdrom_mcn))


	usage::

	  struct cdrom_mcn mcn;

	  ioctl(fd, CDROM_GET_MCN, &mcn);

	(用法：)

	inputs:
		none
		(无。)


	outputs:
		Universal Product Code
		(通用产品代码。)


	error return:
	  - ENOSYS	Drive is not capable of reading MCN data.(光驱不具备读取 MCN 数据的能力。)

	notes:
		- Source code comments state::

		    The following function is implemented, although very few
		    audio discs give Universal Product Code information, which
		    should just be the Medium Catalog Number on the box.  Note,
		    that the way the code is written on the CD is /not/ uniform
		    across all discs!

		  (源码注释说：上面这个函数已经实现，尽管极少数音频光盘会提供通用产品代码信息——它应该就是包装盒上的媒体目录号 (Medium Catalog Number)。注意，CD 上书写该代码的方式在不同光盘之间并不一致！)




CDROM_GET_UPC
	CDROM_GET_MCN  (deprecated)
	(CDROM_GET_MCN (已弃用))


	Not implemented, as of 2.6.8.1
	(截至 2.6.8.1，尚未实现。)



CDROMRESET
	hard-reset the drive
	(硬复位光驱)


	usage::

	  ioctl(fd, CDROMRESET, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		none
		(无。)


	error return:
	  - EACCES	Access denied:  requires CAP_SYS_ADMIN(访问被拒绝：需要 CAP_SYS_ADMIN)
	  - ENOSYS	Drive is not capable of resetting.(光驱不具备复位能力。)




CDROMREADCOOKED
	read data in cooked mode
	(以整理 (cooked) 模式读取数据)


	usage::

	  u8 buffer[CD_FRAMESIZE]

	  ioctl(fd, CDROMREADCOOKED, buffer);

	(用法：)

	inputs:
		none
		(无。)


	outputs:
		2048 bytes of data, "cooked" mode.
		(2048 字节的数据，"cooked" 模式。)


	notes:
		Not implemented on all drives.
		(并非在所有光驱上都已实现。)





CDROMREADALL
	read all 2646 bytes
	(读取全部 2646 字节)


	Same as CDROMREADCOOKED, but reads 2646 bytes.
	(与 CDROMREADCOOKED 相同，但读取 2646 字节。)



CDROMSEEK
	seek msf address
	(寻道到 MSF 地址)


	usage::

	  struct cdrom_msf msf;

	  ioctl(fd, CDROMSEEK, &msf);

	(用法：)

	inputs:
		MSF address to seek to.
		(要寻道到的 MSF 地址。)


	outputs:
		none
		(无。)




CDROMPLAYBLK
	scsi-cd only
	(仅限 SCSI 光驱)

	(struct cdrom_blk)
	((struct cdrom_blk))


	usage::

	  struct cdrom_blk blk;

	  ioctl(fd, CDROMPLAYBLK, &blk);

	(用法：)

	inputs:
		Region to play
		(要播放的区域。)


	outputs:
		none
		(无。)




CDROMGETSPINDOWN
	Obsolete, was ide-cd only
	(已废弃，原先仅限 ide-cd)


	usage::

	  char spindown;

	  ioctl(fd, CDROMGETSPINDOWN, &spindown);

	(用法：)

	inputs:
		none
		(无。)


	outputs:
		The value of the current 4-bit spindown value.
		(当前 4 位降速 (spindown) 值的取值。)





CDROMSETSPINDOWN
	Obsolete, was ide-cd only
	(已废弃，原先仅限 ide-cd)


	usage::

	  char spindown

	  ioctl(fd, CDROMSETSPINDOWN, &spindown);

	(用法：)

	inputs:
		4-bit value used to control spindown (TODO: more detail here)
		(用于控制降速的 4 位值 (TODO：此处需要更多细节))


	outputs:
		none
		(无。)






CDROM_SET_OPTIONS
	Set behavior options
	(设置行为选项)


	usage::

	  int options;

	  ioctl(fd, CDROM_SET_OPTIONS, options);

	(用法：)

	inputs:
		New values for drive options.  The logical 'or' of:
		(光驱选项的新值。取以下各项的逻辑"或"：)

	    ==============      ==================================
	    CDO_AUTO_CLOSE	close tray on first open(2)(第一次 open(2) 时关闭托盘)
	    CDO_AUTO_EJECT	open tray on last release(最后一次释放时弹出托盘)
	    CDO_USE_FFLAGS	use O_NONBLOCK information on open(在 open 时使用 O_NONBLOCK 信息)
	    CDO_LOCK		lock tray on open files(有打开的文件时锁定托盘)
	    CDO_CHECK_TYPE	check type on open for data(打开数据光盘时检查类型)
	    ==============      ==================================

	outputs:
		Returns the resulting options settings in the
		ioctl return value.  Returns -1 on error.
		(在 ioctl 返回值中返回最终生效的选项设置。出错时返回 -1。)

	error return:
	  - ENOSYS	selected option(s) not supported by drive.(所选选项不被光驱支持。)




CDROM_CLEAR_OPTIONS
	Clear behavior options
	(清除行为选项)


	Same as CDROM_SET_OPTIONS, except that selected options are
	turned off.
	(与 CDROM_SET_OPTIONS 相同，只是所选选项被关闭。)



CDROM_SELECT_SPEED
	Set the CD-ROM speed
	(设置 CD-ROM 速度)


	usage::

	  int speed;

	  ioctl(fd, CDROM_SELECT_SPEED, speed);

	(用法：)

	inputs:
		New drive speed.
		(光驱的新速度。)


	outputs:
		none
		(无。)


	error return:
	  - ENOSYS	speed selection not supported by drive.(光驱不支持速度选择。)



CDROM_SELECT_DISC
	Select disc (for juke-boxes)
	(选择光盘 (用于点唱机式设备))


	usage::

	  int disk;

	  ioctl(fd, CDROM_SELECT_DISC, disk);

	(用法：)

	inputs:
		Disk to load into drive.
		(要装入光驱的盘片编号。)


	outputs:
		none
		(无。)


	error return:
	  - EINVAL	Disk number beyond capacity of drive(EINVAL：盘片编号超出光驱容量)



CDROM_MEDIA_CHANGED
	Check is media changed
	(检查介质是否更换过)


	usage::

	  int slot;

	  ioctl(fd, CDROM_MEDIA_CHANGED, slot);

	(用法：)

	inputs:
		Slot number to be tested, always zero except for jukeboxes.
		(要测试的槽位编号，除点唱机外恒为 0。)

		May also be special values CDSL_NONE or CDSL_CURRENT
		(也可以是特殊值 CDSL_NONE 或 CDSL_CURRENT。)

	outputs:
		Ioctl return value is 0 or 1 depending on whether the media

	  has been changed, or -1 on error.
	  (ioctl 返回值为 0 或 1，取决于介质是否已更换；出错时返回 -1。)

	error returns:
	  - ENOSYS	Drive can't detect media change(光驱无法检测介质更换)
	  - EINVAL	Slot number beyond capacity of drive(EINVAL：槽位编号超出光驱容量)
	  - ENOMEM	Out of memory(ENOMEM：内存不足)



CDROM_DRIVE_STATUS
	Get tray position, etc.
	(获取托盘位置等信息)


	usage::

	  int slot;

	  ioctl(fd, CDROM_DRIVE_STATUS, slot);

	(用法：)

	inputs:
		Slot number to be tested, always zero except for jukeboxes.
		(要测试的槽位编号，除点唱机外恒为 0。)

		May also be special values CDSL_NONE or CDSL_CURRENT
		(也可以是特殊值 CDSL_NONE 或 CDSL_CURRENT。)

	outputs:
		Ioctl return value will be one of the following values

	  from <linux/cdrom.h>:
	  (ioctl 返回值将是 <linux/cdrom.h> 中的以下值之一：)

	    =================== ==========================
	    CDS_NO_INFO		Information not available.(信息不可用。)
	    CDS_NO_DISC
	    CDS_TRAY_OPEN
	    CDS_DRIVE_NOT_READY
	    CDS_DISC_OK
	    -1			error(错误)
	    =================== ==========================

	error returns:
	  - ENOSYS	Drive can't detect drive status(光驱无法检测驱动器状态)
	  - EINVAL	Slot number beyond capacity of drive(EINVAL：槽位编号超出光驱容量)
	  - ENOMEM	Out of memory(ENOMEM：内存不足)




CDROM_DISC_STATUS
	Get disc type, etc.
	(获取光盘类型等信息)


	usage::

	  ioctl(fd, CDROM_DISC_STATUS, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		Ioctl return value will be one of the following values

	  from <linux/cdrom.h>:
	  (ioctl 返回值将是 <linux/cdrom.h> 中的以下值之一：)

	    - CDS_NO_INFO
	    - CDS_AUDIO
	    - CDS_MIXED
	    - CDS_XA_2_2
	    - CDS_XA_2_1
	    - CDS_DATA_1

	error returns:
		none at present
		(目前没有。)

	notes:
	    - Source code comments state::


		Ok, this is where problems start.  The current interface for
		the CDROM_DISC_STATUS ioctl is flawed.  It makes the false
		assumption that CDs are all CDS_DATA_1 or all CDS_AUDIO, etc.
		Unfortunately, while this is often the case, it is also
		very common for CDs to have some tracks with data, and some
		tracks with audio.	Just because I feel like it, I declare
		the following to be the best way to cope.  If the CD has
		ANY data tracks on it, it will be returned as a data CD.
		If it has any XA tracks, I will return it as that.	Now I
		could simplify this interface by combining these returns with
		the above, but this more clearly demonstrates the problem
		with the current interface.  Too bad this wasn't designed
		to use bitmasks...	       -Erik

		Well, now we have the option CDS_MIXED: a mixed-type CD.
		User level programmers might feel the ioctl is not very
		useful.
				---david

	  (源码注释说：好了，问题从这里开始。CDROM_DISC_STATUS ioctl 的当前接口是有缺陷的。它做了一个错误假设，认为光盘要么全是 CDS_DATA_1，要么全是 CDS_AUDIO，等等。不幸的是，虽然情况常常如此，但光盘上有些轨道是数据、有些轨道是音频的情况也很常见。由于本人乐意，我在此宣布以下做法是最佳的应对方式。只要光盘上有任何数据轨道，它就会作为数据光盘返回。如果它有任何 XA 轨道，我就按那个返回。我本可以把这些返回值与上面合并来简化这个接口，但这样更能清楚地展示当前接口的问题。可惜它当初没有设计成用位掩码……——Erik

	  好了，现在我们有了 CDS_MIXED 选项：混合类型的光盘。用户态程序员可能会觉得这个 ioctl 不太有用。
				——david)




CDROM_CHANGER_NSLOTS
	Get number of slots
	(获取槽位数量)


	usage::

	  ioctl(fd, CDROM_CHANGER_NSLOTS, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		The ioctl return value will be the number of slots in a
		CD changer.  Typically 1 for non-multi-disk devices.
		(ioctl 返回值将是光盘点唱机的槽位数量。非多盘设备的典型值为 1。)

	error returns:
		none
		(无。)



CDROM_LOCKDOOR
	lock or unlock door
	(锁定或解锁仓门)


	usage::

	  int lock;

	  ioctl(fd, CDROM_LOCKDOOR, lock);

	(用法：)

	inputs:
		Door lock flag, 1=lock, 0=unlock
		(仓门锁定标志，1=锁定，0=解锁)


	outputs:
		none
		(无。)


	error returns:
	  - EDRIVE_CANT_DO_THIS

				Door lock function not supported.
				(不支持仓门锁定功能。)
	  - EBUSY

				Attempt to unlock when multiple users
				have the drive open and not CAP_SYS_ADMIN
				(在多个用户都打开了该光驱、且不具备 CAP_SYS_ADMIN 时尝试解锁。)

	notes:
		As of 2.6.8.1, the lock flag is a global lock, meaning that
		all CD drives will be locked or unlocked together.  This is
		probably a bug.
		(截至 2.6.8.1，lock 标志是一个全局锁，这意味着所有 CD 光驱会被一起锁定或解锁。这很可能是个 bug。)

		The EDRIVE_CANT_DO_THIS value is defined in <linux/cdrom.h>
		and is currently (2.6.8.1) the same as EOPNOTSUPP
		(EDRIVE_CANT_DO_THIS 的值定义在 <linux/cdrom.h> 中，目前 (2.6.8.1) 与 EOPNOTSUPP 相同)



CDROM_DEBUG
	Turn debug messages on/off
	(打开/关闭调试消息)


	usage::

	  int debug;

	  ioctl(fd, CDROM_DEBUG, debug);

	(用法：)

	inputs:
		Cdrom debug flag, 0=disable, 1=enable
		(光驱调试标志，0=禁用，1=启用)


	outputs:
		The ioctl return value will be the new debug flag.
		(ioctl 返回值将是新的调试标志。)


	error return:
	  - EACCES	Access denied:  requires CAP_SYS_ADMIN(访问被拒绝：需要 CAP_SYS_ADMIN)



CDROM_GET_CAPABILITY
	get capabilities
	(获取能力)


	usage::

	  ioctl(fd, CDROM_GET_CAPABILITY, 0);

	(用法：)


	inputs:
		none
		(无。)


	outputs:
		The ioctl return value is the current device capability
		flags.  See CDC_CLOSE_TRAY, CDC_OPEN_TRAY, etc.
		(ioctl 返回值是当前设备能力标志。参见 CDC_CLOSE_TRAY、CDC_OPEN_TRAY 等。)



CDROMAUDIOBUFSIZ
	set the audio buffer size
	(设置音频缓冲区大小)


	usage::

	  int arg;

	  ioctl(fd, CDROMAUDIOBUFSIZ, val);

	(用法：)

	inputs:
		New audio buffer size
		(新的音频缓冲区大小。)


	outputs:
		The ioctl return value is the new audio buffer size, or -1
		on error.
		(ioctl 返回值是新的音频缓冲区大小，出错时返回 -1。)

	error return:
	  - ENOSYS	Not supported by this driver.(此驱动不支持。)

	notes:
		Not supported by all drivers.
		(并非所有驱动都支持。)




DVD_READ_STRUCT			Read structure
				(读取结构)

	usage::

	  dvd_struct s;

	  ioctl(fd, DVD_READ_STRUCT, &s);

	(用法：)

	inputs:
		dvd_struct structure, containing:
		(dvd_struct 结构体，包含：)

	    =================== ==========================================
	    type		specifies the information desired, one of(指定所需信息，取以下之一：)
				DVD_STRUCT_PHYSICAL, DVD_STRUCT_COPYRIGHT,
				DVD_STRUCT_DISCKEY, DVD_STRUCT_BCA,
				DVD_STRUCT_MANUFACT
	    physical.layer_num	desired layer, indexed from 0(所需层，从 0 开始编号)
	    copyright.layer_num	desired layer, indexed from 0(所需层，从 0 开始编号)
	    disckey.agid
	    =================== ==========================================

	outputs:
		dvd_struct structure, containing:
		(dvd_struct 结构体，包含：)

	    =================== ================================
	    physical		for type == DVD_STRUCT_PHYSICAL(当 type == DVD_STRUCT_PHYSICAL 时)
	    copyright		for type == DVD_STRUCT_COPYRIGHT(当 type == DVD_STRUCT_COPYRIGHT 时)
	    disckey.value	for type == DVD_STRUCT_DISCKEY(当 type == DVD_STRUCT_DISCKEY 时)
	    bca.{len,value}	for type == DVD_STRUCT_BCA(当 type == DVD_STRUCT_BCA 时)
	    manufact.{len,valu}	for type == DVD_STRUCT_MANUFACT(当 type == DVD_STRUCT_MANUFACT 时)
	    =================== ================================

	error returns:
	  - EINVAL	physical.layer_num exceeds number of layers(EINVAL：physical.layer_num 超过层数)
	  - EIO		Received invalid response from drive(EIO：收到来自光驱的无效应答)



DVD_WRITE_STRUCT		Write structure
				(写入结构)

	Not implemented, as of 2.6.8.1
	(截至 2.6.8.1，尚未实现。)



DVD_AUTH			Authentication
				(认证)

	usage::

	  dvd_authinfo ai;

	  ioctl(fd, DVD_AUTH, &ai);

	(用法：)

	inputs:
		dvd_authinfo structure.  See <linux/cdrom.h>
		(dvd_authinfo 结构体。参见 <linux/cdrom.h>。)


	outputs:
		dvd_authinfo structure.
		(dvd_authinfo 结构体。)


	error return:
	  - ENOTTY	ai.type not recognized.(无法识别 ai.type。)



CDROM_SEND_PACKET
	send a packet to the drive
	(向光驱发送一个数据包)


	usage::

	  struct cdrom_generic_command cgc;

	  ioctl(fd, CDROM_SEND_PACKET, &cgc);

	(用法：)

	inputs:
		cdrom_generic_command structure containing the packet to send.
		(cdrom_generic_command 结构体，包含要发送的数据包。)


	outputs:
		none
		(无。)

	  cdrom_generic_command structure containing results.
	  (cdrom_generic_command 结构体，包含结果。)

	error return:
	  - EIO

			command failed.(命令失败。)
	  - EPERM

			Operation not permitted, either because a
			write command was attempted on a drive which
			is opened read-only, or because the command
			requires CAP_SYS_RAWIO
			(操作不被允许，或是因为对以只读方式打开的光驱尝试了写命令，或是因为该命令需要 CAP_SYS_RAWIO。)
	  - EINVAL

			cgc.data_direction not set(未设置 cgc.data_direction。)



CDROM_NEXT_WRITABLE
	get next writable block
	(获取下一个可写块)


	usage::

	  long next;

	  ioctl(fd, CDROM_NEXT_WRITABLE, &next);

	(用法：)

	inputs:
		none
		(无。)


	outputs:
		The next writable block.
		(下一个可写块。)


	notes:
		If the device does not support this ioctl directly, the

	  ioctl will return CDROM_LAST_WRITTEN + 7.
	  (如果设备不直接支持这个 ioctl，该 ioctl 将返回 CDROM_LAST_WRITTEN + 7。)



CDROM_LAST_WRITTEN
	get last block written on disc
	(获取光盘上最后写入的块)


	usage::

	  long last;

	  ioctl(fd, CDROM_LAST_WRITTEN, &last);

	(用法：)

	inputs:
		none
		(无。)


	outputs:
		The last block written on disc
		(光盘上最后写入的块。)


	notes:
		If the device does not support this ioctl directly, the
		result is derived from the disc's table of contents.  If the
		table of contents can't be read, this ioctl returns an
		error.
		(如果设备不直接支持这个 ioctl，结果将从光盘的目录表推导得出。如果目录表无法读取，该 ioctl 返回一个错误。)
