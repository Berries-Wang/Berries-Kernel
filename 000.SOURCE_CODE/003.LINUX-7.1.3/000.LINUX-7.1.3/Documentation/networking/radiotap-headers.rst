.. SPDX-License-Identifier: GPL-2.0

===========================
How to use radiotap headers
===========================

(如何使用 radiotap 头)

Pointer to the radiotap include file
(radiotap 头文件位置的指引)
------------------------------------

Radiotap headers are variable-length and extensible, you can get most of the
information you need to know on them from::
(radiotap 头是可变长且可扩展的，关于它们你需要了解的大部分信息都可以从以下位置获取：)

    ./include/net/ieee80211_radiotap.h

This document gives an overview and warns on some corner cases.
(本文档给出一个概述，并对一些边角情况提出警告。)


Structure of the header
(头部的结构)
-----------------------

There is a fixed portion at the start which contains a u32 bitmap that defines
if the possible argument associated with that bit is present or not.  So if b0
of the it_present member of ieee80211_radiotap_header is set, it means that
the header for argument index 0 (IEEE80211_RADIOTAP_TSFT) is present in the
argument area.
(开头有一个固定部分，其中包含一个 u32 位图，用于定义与每个位相关联的参数是否存在。
因此，如果 ieee80211_radiotap_header 的 it_present 成员的 b0 被置位，则表示参数
索引 0(IEEE80211_RADIOTAP_TSFT)的头部存在于参数区域中。)

::

   < 8-byte ieee80211_radiotap_header >
   [ <possible argument bitmap extensions ... > ]
   [ <argument> ... ]

At the moment there are only 13 possible argument indexes defined, but in case
we run out of space in the u32 it_present member, it is defined that b31 set
indicates that there is another u32 bitmap following (shown as "possible
argument bitmap extensions..." above), and the start of the arguments is moved
forward 4 bytes each time.
(目前只定义了 13 个可能的参数索引，但如果 u32 it_present 成员的空间不够用，则定义
b31 置位表示后面还跟着另一个 u32 位图(即上面所示的 "possible argument bitmap
extensions...")，并且参数的起始位置每次向后移动 4 字节。)

Note also that the it_len member __le16 is set to the total number of bytes
covered by the ieee80211_radiotap_header and any arguments following.
(另请注意，it_len 成员(__le16)被设置为 ieee80211_radiotap_header 及其后续所有参数
所覆盖的总字节数。)


Requirements for arguments
(参数的要求)
--------------------------

After the fixed part of the header, the arguments follow for each argument
index whose matching bit is set in the it_present member of
ieee80211_radiotap_header.
(在头部的固定部分之后，对于 it_present 成员中匹配位被置位的每个参数索引，其参数依次
排列在 ieee80211_radiotap_header 之后。)

 - the arguments are all stored little-endian!
   (所有参数都以小端序存储！)

 - the argument payload for a given argument index has a fixed size.  So
   IEEE80211_RADIOTAP_TSFT being present always indicates an 8-byte argument is
   present.  See the comments in ./include/net/ieee80211_radiotap.h for a nice
   breakdown of all the argument sizes
   (给定参数索引的参数载荷具有固定的大小。因此 IEEE80211_RADIOTAP_TSFT 存在就总是
   表示存在一个 8 字节的参数。所有参数大小的详细分解参见
   ./include/net/ieee80211_radiotap.h 中的注释)

 - the arguments must be aligned to a boundary of the argument size using
   padding.  So a u16 argument must start on the next u16 boundary if it isn't
   already on one, a u32 must start on the next u32 boundary and so on.
   (必须使用填充将参数对齐到参数大小的边界上。因此，u16 参数如果尚未位于 u16 边界
   上，就必须从下一个 u16 边界开始，u32 必须从下一个 u32 边界开始，依此类推。)

 - "alignment" is relative to the start of the ieee80211_radiotap_header, ie,
   the first byte of the radiotap header.  The absolute alignment of that first
   byte isn't defined.  So even if the whole radiotap header is starting at, eg,
   address 0x00000003, still the first byte of the radiotap header is treated as
   0 for alignment purposes.
   ("对齐"是相对于 ieee80211_radiotap_header 的起始位置而言的，即 radiotap 头的
   第一个字节。该第一个字节的绝对对齐方式没有定义。因此，即使整个 radiotap 头从例如
   地址 0x00000003 处开始，就对齐而言 radiotap 头的第一个字节仍被视为 0。)

 - the above point that there may be no absolute alignment for multibyte
   entities in the fixed radiotap header or the argument region means that you
   have to take special evasive action when trying to access these multibyte
   entities.  Some arches like Blackfin cannot deal with an attempt to
   dereference, eg, a u16 pointer that is pointing to an odd address.  Instead
   you have to use a kernel API get_unaligned() to dereference the pointer,
   which will do it bytewise on the arches that require that.
   (上面提到固定 radiotap 头或参数区域中的多字节实体可能没有绝对对齐，这意味着在尝试
   访问这些多字节实体时必须采取特殊的规避措施。某些架构(如 Blackfin)无法处理对指向
   奇数地址的 u16 指针的解引用尝试。此时必须使用内核 API get_unaligned() 来解引用
   该指针，在需要这样做的架构上它会按字节方式进行。)

 - The arguments for a given argument index can be a compound of multiple types
   together.  For example IEEE80211_RADIOTAP_CHANNEL has an argument payload
   consisting of two u16s of total length 4.  When this happens, the padding
   rule is applied dealing with a u16, NOT dealing with a 4-byte single entity.
   (给定参数索引的参数可以是多种类型的组合。例如 IEEE80211_RADIOTAP_CHANNEL 的参数
   载荷由两个 u16 组成，总长度为 4。发生这种情况时，填充规则按 u16 处理，而不是按
   4 字节的单一实体处理。)


Example valid radiotap header
(有效 radiotap 头示例)
-----------------------------

::

	0x00, 0x00, // <-- radiotap version + pad byte
	0x0b, 0x00, // <- radiotap header length
	0x04, 0x0c, 0x00, 0x00, // <-- bitmap
	0x6c, // <-- rate (in 500kHz units)
	0x0c, //<-- tx power
	0x01 //<-- antenna


Using the Radiotap Parser
(使用 radiotap 解析器)
-------------------------

If you are having to parse a radiotap struct, you can radically simplify the
job by using the radiotap parser that lives in net/wireless/radiotap.c and has
its prototypes available in include/net/cfg80211.h.  You use it like this::
(如果你需要解析 radiotap 结构体，使用位于 net/wireless/radiotap.c 中、其原型可在
include/net/cfg80211.h 中找到的 radiotap 解析器，可以极大地简化这项工作。用法
如下：)

    #include <net/cfg80211.h>

    /* buf points to the start of the radiotap header part */

    int MyFunction(u8 * buf, int buflen)
    {
	    int pkt_rate_100kHz = 0, antenna = 0, pwr = 0;
	    struct ieee80211_radiotap_iterator iterator;
	    int ret = ieee80211_radiotap_iterator_init(&iterator, buf, buflen);

	    while (!ret) {

		    ret = ieee80211_radiotap_iterator_next(&iterator);

		    if (ret)
			    continue;

		    /* see if this argument is something we can use */

		    switch (iterator.this_arg_index) {
		    /*
		    * You must take care when dereferencing iterator.this_arg
		    * for multibyte types... the pointer is not aligned.  Use
		    * get_unaligned((type *)iterator.this_arg) to dereference
		    * iterator.this_arg for type "type" safely on all arches.
		    */
		    case IEEE80211_RADIOTAP_RATE:
			    /* radiotap "rate" u8 is in
			    * 500kbps units, eg, 0x02=1Mbps
			    */
			    pkt_rate_100kHz = (*iterator.this_arg) * 5;
			    break;

		    case IEEE80211_RADIOTAP_ANTENNA:
			    /* radiotap uses 0 for 1st ant */
			    antenna = *iterator.this_arg);
			    break;

		    case IEEE80211_RADIOTAP_DBM_TX_POWER:
			    pwr = *iterator.this_arg;
			    break;

		    default:
			    break;
		    }
	    }  /* while more rt headers */

	    if (ret != -ENOENT)
		    return TXRX_DROP;

	    /* discard the radiotap header part */
	    buf += iterator.max_length;
	    buflen -= iterator.max_length;

	    ...

    }

Andy Green <andy@warmcat.com>
(作者: Andy Green <andy@warmcat.com>)
