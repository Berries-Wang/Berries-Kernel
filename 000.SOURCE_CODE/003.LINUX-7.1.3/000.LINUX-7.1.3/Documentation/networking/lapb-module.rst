.. SPDX-License-Identifier: GPL-2.0

===============================
The Linux LAPB Module Interface
===============================

(Linux LAPB 模块接口)

Version 1.3
(版本 1.3)

Jonathan Naylor 29.12.96
(Jonathan Naylor，1996 年 12 月 29 日)

Changed (Henner Eisen, 2000-10-29): int return value for data_indication()
(变更（Henner Eisen，2000-10-29）：data_indication() 的返回值改为 int。)

The LAPB module will be a separately compiled module for use by any parts of
the Linux operating system that require a LAPB service. This document
defines the interfaces to, and the services provided by this module. The
term module in this context does not imply that the LAPB module is a
separately loadable module, although it may be. The term module is used in
its more standard meaning.
(LAPB 模块将是一个单独编译的模块，供 Linux 操作系统中任何需要 LAPB 服务
的部分使用。本文档定义该模块的接口及其提供的服务。此处的"模块"一词并不
意味着 LAPB 模块是可单独加载的模块——尽管它可能是；这里"模块"一词取其
更通用的含义。)

The interface to the LAPB module consists of functions to the module,
callbacks from the module to indicate important state changes, and
structures for getting and setting information about the module.
(与 LAPB 模块的接口包括：调用模块的函数、模块用于指示重要状态变化的
回调，以及获取和设置模块信息的结构体。)

Structures
(结构体)
----------

Probably the most important structure is the skbuff structure for holding
received and transmitted data, however it is beyond the scope of this
document.
(最重要的结构体大概是用于存放接收与发送数据的 skbuff 结构体，但它超出
了本文档的范围。)

The two LAPB specific structures are the LAPB initialisation structure and
the LAPB parameter structure. These will be defined in a standard header
file, <linux/lapb.h>. The header file <net/lapb.h> is internal to the LAPB
module and is not for use.
(两个 LAPB 专属结构体是 LAPB 初始化结构体和 LAPB 参数结构体。它们将定义
在标准头文件 <linux/lapb.h> 中。头文件 <net/lapb.h> 是 LAPB 模块内部的
文件，不供外部使用。)

LAPB Initialisation Structure
(LAPB 初始化结构体)
-----------------------------

This structure is used only once, in the call to lapb_register (see below).
It contains information about the device driver that requires the services
of the LAPB module::
(该结构体只使用一次，即在调用 lapb_register 时（见下文）。它包含需要
LAPB 模块服务的设备驱动程序的信息：)

	struct lapb_register_struct {
		void (*connect_confirmation)(int token, int reason);
		void (*connect_indication)(int token, int reason);
		void (*disconnect_confirmation)(int token, int reason);
		void (*disconnect_indication)(int token, int reason);
		int  (*data_indication)(int token, struct sk_buff *skb);
		void (*data_transmit)(int token, struct sk_buff *skb);
	};

Each member of this structure corresponds to a function in the device driver
that is called when a particular event in the LAPB module occurs. These will
be described in detail below. If a callback is not required (!!) then a NULL
may be substituted.
(该结构体的每个成员都对应设备驱动程序中的一个函数，当 LAPB 模块中发生
特定事件时会调用它。下文将详细介绍这些函数。如果不需要某个回调（!!），
可以用 NULL 代替。)


LAPB Parameter Structure
(LAPB 参数结构体)
------------------------

This structure is used with the lapb_getparms and lapb_setparms functions
(see below). They are used to allow the device driver to get and set the
operational parameters of the LAPB implementation for a given connection::
(该结构体与 lapb_getparms 和 lapb_setparms 函数一起使用（见下文）。它们
用于让设备驱动程序获取和设置给定连接的 LAPB 实现运行参数：)

	struct lapb_parms_struct {
		unsigned int t1;
		unsigned int t1timer;
		unsigned int t2;
		unsigned int t2timer;
		unsigned int n2;
		unsigned int n2count;
		unsigned int window;
		unsigned int state;
		unsigned int mode;
	};

T1 and T2 are protocol timing parameters and are given in units of 100ms. N2
is the maximum number of tries on the link before it is declared a failure.
The window size is the maximum number of outstanding data packets allowed to
be unacknowledged by the remote end, the value of the window is between 1
and 7 for a standard LAPB link, and between 1 and 127 for an extended LAPB
link.
(T1 和 T2 是协议计时参数，以 100 毫秒为单位。N2 是链路被判定为失败之前
的最大尝试次数。窗口大小是允许远端未确认的未决数据包的最大数量：标准
LAPB 链路的窗口值介于 1 到 7 之间，扩展 LAPB 链路则介于 1 到 127 之间。)

The mode variable is a bit field used for setting (at present) three values.
The bit fields have the following meanings:
(mode 变量是一个位域，用于设置（目前）三个值。各字段的含义如下：)

======  =================================================
Bit	Meaning (含义)
======  =================================================
0	LAPB operation (0=LAPB_STANDARD 1=LAPB_EXTENDED). (LAPB 操作：0=LAPB_STANDARD，1=LAPB_EXTENDED)
1	[SM]LP operation (0=LAPB_SLP 1=LAPB=MLP). ([SM]LP 操作：0=LAPB_SLP，1=LAPB=MLP)
2	DTE/DCE operation (0=LAPB_DTE 1=LAPB_DCE) (DTE/DCE 操作：0=LAPB_DTE，1=LAPB_DCE)
3-31	Reserved, must be 0. (保留，必须为 0。)
======  =================================================

Extended LAPB operation indicates the use of extended sequence numbers and
consequently larger window sizes, the default is standard LAPB operation.
MLP operation is the same as SLP operation except that the addresses used by
LAPB are different to indicate the mode of operation, the default is Single
Link Procedure. The difference between DCE and DTE operation is (i) the
addresses used for commands and responses, and (ii) when the DCE is not
connected, it sends DM without polls set, every T1. The upper case constant
names will be defined in the public LAPB header file.
(扩展 LAPB 操作表示使用扩展序列号，因而窗口尺寸更大；默认为标准 LAPB
操作。MLP 操作与 SLP 操作相同，只是 LAPB 使用的地址不同，以指示操作
模式；默认为单链路规程（Single Link Procedure）。DCE 与 DTE 操作的区别
在于：(i) 命令与响应所用的地址不同；(ii) 当 DCE 未连接时，它每隔 T1
发送一个未置轮询位的 DM。大写常量名将在公共 LAPB 头文件中定义。)


Functions
(函数)
---------

The LAPB module provides a number of function entry points.
(LAPB 模块提供若干函数入口点。)

::

    int lapb_register(void *token, struct lapb_register_struct);

This must be called before the LAPB module may be used. If the call is
successful then LAPB_OK is returned. The token must be a unique identifier
generated by the device driver to allow for the unique identification of the
instance of the LAPB link. It is returned by the LAPB module in all of the
callbacks, and is used by the device driver in all calls to the LAPB module.
For multiple LAPB links in a single device driver, multiple calls to
lapb_register must be made. The format of the lapb_register_struct is given
above. The return values are:
(在使用 LAPB 模块之前必须调用此函数。如果调用成功，则返回 LAPB_OK。
token 必须是设备驱动程序生成的唯一标识符，以便唯一标识该 LAPB 链路
实例。LAPB 模块会在所有回调中返回该 token，设备驱动程序在所有对 LAPB
模块的调用中也会使用它。若单个设备驱动程序中有多个 LAPB 链路，则必须
多次调用 lapb_register。lapb_register_struct 的格式已在上面给出。返回值
如下：)

=============		=============================
LAPB_OK			LAPB registered successfully. (LAPB 注册成功。)
LAPB_BADTOKEN		Token is already registered. (该 token 已被注册。)
LAPB_NOMEM		Out of memory (内存不足)
=============		=============================

::

    int lapb_unregister(void *token);

This releases all the resources associated with a LAPB link. Any current
LAPB link will be abandoned without further messages being passed. After
this call, the value of token is no longer valid for any calls to the LAPB
function. The valid return values are:
(该函数释放与某条 LAPB 链路相关的所有资源。任何当前存在的 LAPB 链路
都将被放弃，不再传递任何消息。此调用之后，token 值对 LAPB 函数的任何
调用都不再有效。有效返回值如下：)

=============		===============================
LAPB_OK			LAPB unregistered successfully. (LAPB 注销成功。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
=============		===============================

::

    int lapb_getparms(void *token, struct lapb_parms_struct *parms);

This allows the device driver to get the values of the current LAPB
variables, the lapb_parms_struct is described above. The valid return values
are:
(该函数允许设备驱动程序获取当前 LAPB 变量的值，lapb_parms_struct 已在
上面描述。有效返回值如下：)

=============		=============================
LAPB_OK			LAPB getparms was successful. (LAPB getparms 成功。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
=============		=============================

::

    int lapb_setparms(void *token, struct lapb_parms_struct *parms);

This allows the device driver to set the values of the current LAPB
variables, the lapb_parms_struct is described above. The values of t1timer,
t2timer and n2count are ignored, likewise changing the mode bits when
connected will be ignored. An error implies that none of the values have
been changed. The valid return values are:
(该函数允许设备驱动程序设置当前 LAPB 变量的值，lapb_parms_struct 已在
上面描述。t1timer、t2timer 和 n2count 的值会被忽略；同样，在已连接的
情况下更改模式位也会被忽略。发生错误意味着没有任何值被更改。有效返回
值如下：)

=============		=================================================
LAPB_OK			LAPB getparms was successful. (LAPB getparms 成功。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
LAPB_INVALUE		One of the values was out of its allowable range. (某个值超出了其允许的范围。)
=============		=================================================

::

    int lapb_connect_request(void *token);

Initiate a connect using the current parameter settings. The valid return
values are:
(使用当前参数设置发起一次连接。有效返回值如下：)

==============		=================================
LAPB_OK			LAPB is starting to connect. (LAPB 正在开始连接。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
LAPB_CONNECTED		LAPB module is already connected. (LAPB 模块已处于连接状态。)
==============		=================================

::

    int lapb_disconnect_request(void *token);

Initiate a disconnect. The valid return values are:
(发起一次断开连接。有效返回值如下：)

=================	===============================
LAPB_OK			LAPB is starting to disconnect. (LAPB 正在开始断开连接。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
LAPB_NOTCONNECTED	LAPB module is not connected. (LAPB 模块未处于连接状态。)
=================	===============================

::

    int lapb_data_request(void *token, struct sk_buff *skb);

Queue data with the LAPB module for transmitting over the link. If the call
is successful then the skbuff is owned by the LAPB module and may not be
used by the device driver again. The valid return values are:
(把要经链路发送的数据排入 LAPB 模块的队列。如果调用成功，该 skbuff 就
归 LAPB 模块所有，设备驱动程序不得再次使用。有效返回值如下：)

=================	=============================
LAPB_OK			LAPB has accepted the data. (LAPB 已接受该数据。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
LAPB_NOTCONNECTED	LAPB module is not connected. (LAPB 模块未处于连接状态。)
=================	=============================

::

    int lapb_data_received(void *token, struct sk_buff *skb);

Queue data with the LAPB module which has been received from the device. It
is expected that the data passed to the LAPB module has skb->data pointing
to the beginning of the LAPB data. If the call is successful then the skbuff
is owned by the LAPB module and may not be used by the device driver again.
The valid return values are:
(把从设备接收到的数据排入 LAPB 模块的队列。传入 LAPB 模块的数据应当使
skb->data 指向 LAPB 数据的起始位置。如果调用成功，该 skbuff 就归 LAPB
模块所有，设备驱动程序不得再次使用。有效返回值如下：)

=============		===========================
LAPB_OK			LAPB has accepted the data. (LAPB 已接受该数据。)
LAPB_BADTOKEN		Invalid/unknown LAPB token. (LAPB token 无效或未知。)
=============		===========================

Callbacks
(回调)
---------

These callbacks are functions provided by the device driver for the LAPB
module to call when an event occurs. They are registered with the LAPB
module with lapb_register (see above) in the structure lapb_register_struct
(see above).
(这些回调是设备驱动程序提供的函数，供 LAPB 模块在事件发生时调用。它们
通过 lapb_register（见上文）在 lapb_register_struct 结构体（见上文）中向
LAPB 模块注册。)

::

    void (*connect_confirmation)(void *token, int reason);

This is called by the LAPB module when a connection is established after
being requested by a call to lapb_connect_request (see above). The reason is
always LAPB_OK.
(在通过调用 lapb_connect_request（见上文）请求连接并成功建立后，LAPB
模块会调用此函数。reason 始终为 LAPB_OK。)

::

    void (*connect_indication)(void *token, int reason);

This is called by the LAPB module when the link is established by the remote
system. The value of reason is always LAPB_OK.
(当链路由远端系统建立时，LAPB 模块会调用此函数。reason 的值始终为
LAPB_OK。)

::

    void (*disconnect_confirmation)(void *token, int reason);

This is called by the LAPB module when an event occurs after the device
driver has called lapb_disconnect_request (see above). The reason indicates
what has happened. In all cases the LAPB link can be regarded as being
terminated. The values for reason are:
(在设备驱动程序调用 lapb_disconnect_request（见上文）之后发生事件时，
LAPB 模块会调用此函数。reason 指示发生了什么。无论何种情况，都可以认为
该 LAPB 链路已经终止。reason 的取值如下：)

=================	====================================================
LAPB_OK			The LAPB link was terminated normally. (LAPB 链路已正常终止。)
LAPB_NOTCONNECTED	The remote system was not connected. (远端系统未连接。)
LAPB_TIMEDOUT		No response was received in N2 tries from the remote
			system. (尝试 N2 次后仍未收到远端系统的响应。)
=================	====================================================

::

    void (*disconnect_indication)(void *token, int reason);

This is called by the LAPB module when the link is terminated by the remote
system or another event has occurred to terminate the link. This may be
returned in response to a lapb_connect_request (see above) if the remote
system refused the request. The values for reason are:
(当链路被远端系统终止、或发生其他导致链路终止的事件时，LAPB 模块会调用
此函数。如果远端系统拒绝了请求，它也可能作为 lapb_connect_request（见
上文）的响应被回调。reason 的取值如下：)

=================	====================================================
LAPB_OK			The LAPB link was terminated normally by the remote
			system. (LAPB 链路已被远端系统正常终止。)
LAPB_REFUSED		The remote system refused the connect request. (远端系统拒绝了连接请求。)
LAPB_NOTCONNECTED	The remote system was not connected. (远端系统未连接。)
LAPB_TIMEDOUT		No response was received in N2 tries from the remote
			system. (尝试 N2 次后仍未收到远端系统的响应。)
=================	====================================================

::

    int (*data_indication)(void *token, struct sk_buff *skb);

This is called by the LAPB module when data has been received from the
remote system that should be passed onto the next layer in the protocol
stack. The skbuff becomes the property of the device driver and the LAPB
module will not perform any more actions on it. The skb->data pointer will
be pointing to the first byte of data after the LAPB header.
(当收到来自远端系统、需要向上传递到协议栈下一层的数据时，LAPB 模块会
调用此函数。该 skbuff 随即归设备驱动程序所有，LAPB 模块不会再对它执行
任何操作。skb->data 指针将指向 LAPB 头之后的第一个数据字节。)

This method should return NET_RX_DROP (as defined in the header
file include/linux/netdevice.h) if and only if the frame was dropped
before it could be delivered to the upper layer.
(当且仅当该帧在未能送达上层之前被丢弃时，本方法才应返回 NET_RX_DROP
（定义于头文件 include/linux/netdevice.h 中）。)

::

    void (*data_transmit)(void *token, struct sk_buff *skb);

This is called by the LAPB module when data is to be transmitted to the
remote system by the device driver. The skbuff becomes the property of the
device driver and the LAPB module will not perform any more actions on it.
The skb->data pointer will be pointing to the first byte of the LAPB header.
(当设备驱动程序要向远端系统发送数据时，LAPB 模块会调用此函数。该 skbuff
随即归设备驱动程序所有，LAPB 模块不会再对它执行任何操作。skb->data
指针将指向 LAPB 头的第一个字节。)
