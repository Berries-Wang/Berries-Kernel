:orphan:

=======================================
MSM Crash Dump Format(MSM 崩溃转储格式)
=======================================

Following a GPU hang the MSM driver outputs debugging information via
/sys/kernel/dri/X/show or via devcoredump (/sys/class/devcoredump/dcdX/data).
This document describes how the output is formatted.
(GPU 挂起(hang)之后，MSM 驱动会通过 /sys/kernel/dri/X/show 或
devcoredump(/sys/class/devcoredump/dcdX/data)输出调试信息。本文描述
输出的格式。)

Each entry is in the form key: value. Sections headers will not have a value
and all the contents of a section will be indented two spaces from the header.
Each section might have multiple array entries the start of which is designated
by a (-).
(每个条目都是 key: value 的形式。节标题没有值，节的所有内容相对标题
缩进两个空格。每个节可能有多个数组条目，其开始由 (-) 标记。)

Mappings(映射)
--------------

kernel
	The kernel version that generated the dump (UTS_RELEASE).
        (生成转储的内核版本(UTS_RELEASE)。)

module
	The module that generated the crashdump.
        (生成崩溃转储的模块。)

time
	The kernel time at crash formatted as seconds.microseconds.
        (崩溃时的内核时间，格式为 秒.微秒。)

comm
	Comm string for the binary that generated the fault.
        (产生故障的二进制文件的 comm 字符串。)

cmdline
	Command line for the binary that generated the fault.
        (产生故障的二进制文件的命令行。)

revision
	ID of the GPU that generated the crash formatted as
	core.major.minor.patchlevel separated by dots.
        (产生崩溃的 GPU 的 ID，格式为以点分隔的
        core.major.minor.patchlevel。)

rbbm-status
	The current value of RBBM_STATUS which shows what top level GPU
	components are in use at the time of crash.
        (RBBM_STATUS 的当前值，显示崩溃时正在使用哪些顶层 GPU 组件。)

ringbuffer
	Section containing the contents of each ringbuffer. Each ringbuffer is
	identified with an id number.
        (包含每个 ringbuffer 内容的节。每个 ringbuffer 由一个 id 号
        标识。)

	id
		Ringbuffer ID (0 based index).  Each ringbuffer in the section
		will have its own unique id.
                (Ringbuffer ID(从 0 开始的索引)。该节中的每个
                ringbuffer 都有其自己的唯一 id。)
	iova
		GPU address of the ringbuffer.
                (ringbuffer 的 GPU 地址。)

	last-fence
		The last fence that was issued on the ringbuffer
                (在 ringbuffer 上发出的最后一个 fence。)

	retired-fence
		The last fence retired on the ringbuffer.
                (ringbuffer 上最后一个已退休(retired)的 fence。)

	rptr
		The current read pointer (rptr) for the ringbuffer.
                (ringbuffer 的当前读指针(rptr)。)

	wptr
		The current write pointer (wptr) for the ringbuffer.
                (ringbuffer 的当前写指针(wptr)。)

	size
		Maximum size of the ringbuffer programmed in the hardware.
                (在硬件中编程的 ringbuffer 最大大小。)

	data
		The contents of the ring encoded as ascii85.  Only the used
		portions of the ring will be printed.
                (以 ascii85 编码的 ring 内容。只打印 ring 的已使用
                部分。)

bo
	List of buffers from the hanging submission if available.
	Each buffer object will have a uinque iova.
        (如果可用，来自挂起提交的缓冲区列表。每个缓冲区对象都会有
        一个唯一的 iova。)

	iova
		GPU address of the buffer object.
                (缓冲区对象的 GPU 地址。)

	size
		Allocated size of the buffer object.
                (缓冲区对象分配的大小。)

	data
		The contents of the buffer object encoded with ascii85.  Only
		Trailing zeros at the end of the buffer will be skipped.
                (以 ascii85 编码的缓冲区对象内容。只有缓冲区末尾的
                尾随零会被跳过。)

registers
	Set of registers values. Each entry is on its own line enclosed
	by brackets { }.
        (一组寄存器值。每个条目单独占一行，由花括号 { } 包围。)

	offset
		Byte offset of the register from the start of the
		GPU memory region.
                (寄存器相对 GPU 内存区域起始处的字节偏移。)

	value
		Hexadecimal value of the register.
                (寄存器的十六进制值。)

registers-hlsq
		(5xx only) Register values from the HLSQ aperture.
		Same format as the register section.
                ((仅 5xx)HLSQ 孔径的寄存器值。格式与 registers 节相同。)
