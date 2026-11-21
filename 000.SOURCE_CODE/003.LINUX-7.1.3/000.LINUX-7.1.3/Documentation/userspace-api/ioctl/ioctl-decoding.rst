===============================================
Decoding an IOCTL Magic Number(解码 IOCTL 魔数)
===============================================

To decode a hex IOCTL code:
(解码十六进制 IOCTL 代码的方法如下：)

Most architectures use this generic format, but check
include/ARCH/ioctl.h for specifics, e.g. powerpc
uses 3 bits to encode read/write and 13 bits for size.
(大多数体系结构使用这一通用格式，但具体细节请查看 include/ARCH/ioctl.h，
例如 powerpc 用 3 位编码读/写、用 13 位编码大小。)

 ====== ==================================
 bits   meaning
 ====== ==================================
 31-30	00 - no parameters: uses _IO macro
	10 - read: _IOR
	01 - write: _IOW
	11 - read/write: _IOWR

 29-16	size of arguments

 15-8	ascii character supposedly
	unique to each driver

 7-0	function #
 ====== ==================================


So for example 0x82187201 is a read with arg length of 0x218,
character 'r' function 1. Grepping the source reveals this is::

	#define VFAT_IOCTL_READDIR_BOTH         _IOR('r', 1, struct dirent [2])

(例如，0x82187201 是一次读取操作，参数长度为 0x218，字符为 'r'，功能号为
1。在源码中检索可以发现它就是：)
