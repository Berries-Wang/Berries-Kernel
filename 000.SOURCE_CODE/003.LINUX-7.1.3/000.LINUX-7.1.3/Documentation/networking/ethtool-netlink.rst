=============================
Netlink interface for ethtool
=============================

(ethtool 的 netlink 接口)


Basic information
(基本信息)
=================

Netlink interface for ethtool uses generic netlink family ``ethtool``
(userspace application should use macros ``ETHTOOL_GENL_NAME`` and
``ETHTOOL_GENL_VERSION`` defined in ``<linux/ethtool_netlink.h>`` uapi
header). This family does not use a specific header, all information in
requests and replies is passed using netlink attributes.
(ethtool 的 netlink 接口使用 generic netlink 族 ``ethtool`` (用户空间应用程序应
使用 ``<linux/ethtool_netlink.h>`` uapi 头文件中定义的宏 ``ETHTOOL_GENL_NAME``
和 ``ETHTOOL_GENL_VERSION``)。该族不使用专用报文头，请求和应答中的所有信息都
通过 netlink 属性传递。)

The ethtool netlink interface uses extended ACK for error and warning
reporting, userspace application developers are encouraged to make these
messages available to user in a suitable way.
(ethtool netlink 接口使用扩展 ACK 报告错误和警告，鼓励用户空间应用程序开发者
以合适的方式将这些消息呈现给用户。)

Requests can be divided into three categories: "get" (retrieving information),
"set" (setting parameters) and "action" (invoking an action).
(请求可分为三类："get"(获取信息)、"set"(设置参数)和 "action"(执行动作)。)

All "set" and "action" type requests require admin privileges
(``CAP_NET_ADMIN`` in the namespace). Most "get" type requests are allowed for
anyone but there are exceptions (where the response contains sensitive
information). In some cases, the request as such is allowed for anyone but
unprivileged users have attributes with sensitive information (e.g.
wake-on-lan password) omitted.
(所有 "set" 和 "action" 类型的请求都需要管理员权限(所在命名空间中的
``CAP_NET_ADMIN``)。大多数 "get" 类型的请求对所有人开放，但也有例外(响应中
包含敏感信息的情况)。在某些情况下，请求本身对所有人开放，但非特权用户得到的
属性中会省略敏感信息(例如 wake-on-lan 密码)。)


Conventions
(约定)
===========

Attributes which represent a boolean value usually use NLA_U8 type so that we
can distinguish three states: "on", "off" and "not present" (meaning the
information is not available in "get" requests or value is not to be changed
in "set" requests). For these attributes, the "true" value should be passed as
number 1 but any non-zero value should be understood as "true" by recipient.
In the tables below, "bool" denotes NLA_U8 attributes interpreted in this way.
(表示布尔值的属性通常使用 NLA_U8 类型，以便区分三种状态："on"(开)、"off"(关)
和 "not present"(不存在)——后者表示 "get" 请求中无法提供该信息，或者 "set"
请求中不打算更改该值。对于这类属性，"true" 值应以数字 1 传入，但接收方应把任何
非零值都理解为 "true"。在下表中，"bool" 表示按此方式解释的 NLA_U8 属性。)

In the message structure descriptions below, if an attribute name is suffixed
with "+", parent nest can contain multiple attributes of the same type. This
implements an array of entries.
(在下文的消息结构描述中，若属性名以 "+" 结尾，表示父嵌套属性可包含多个同类型
属性。这实现了条目数组。)

Attributes that need to be filled-in by device drivers and that are dumped to
user space based on whether they are valid or not should not use zero as a
valid value. This avoids the need to explicitly signal the validity of the
attribute in the device driver API.
(需要由设备驱动程序填充、并根据其是否有效来决定是否转储到用户空间的属性，不应
把零用作有效值。这样可以避免在设备驱动 API 中显式标示属性的有效性。)


Request header
(请求头)
==============

Each request or reply message contains a nested attribute with common header.
Structure of this header is
(每个请求或应答消息都包含一个携带公共头的嵌套属性。该头的结构如下：)

  ==============================  ======  =============================
  ``ETHTOOL_A_HEADER_DEV_INDEX``  u32     device ifindex (设备 ifindex)
  ``ETHTOOL_A_HEADER_DEV_NAME``   string  device name (设备名)
  ``ETHTOOL_A_HEADER_FLAGS``      u32     flags common for all requests (所有请求共用的标志)
  ``ETHTOOL_A_HEADER_PHY_INDEX``  u32     phy device index (PHY 设备索引)
  ==============================  ======  =============================

``ETHTOOL_A_HEADER_DEV_INDEX`` and ``ETHTOOL_A_HEADER_DEV_NAME`` identify the
device message relates to. One of them is sufficient in requests, if both are
used, they must identify the same device. Some requests, e.g. global string
sets, do not require device identification. Most ``GET`` requests also allow
dump requests without device identification to query the same information for
all devices providing it (each device in a separate message).
(``ETHTOOL_A_HEADER_DEV_INDEX`` 和 ``ETHTOOL_A_HEADER_DEV_NAME`` 标识消息所针对
的设备。请求中给出其中之一即可；若两者同时使用，则必须指向同一设备。某些请求
(例如全局字符串集)不需要设备标识。大多数 ``GET`` 请求还允许不带设备标识的转储
请求，用于向所有提供该信息的设备查询相同信息(每个设备单独一条消息)。)

``ETHTOOL_A_HEADER_FLAGS`` is a bitmap of request flags common for all request
types. The interpretation of these flags is the same for all request types but
the flags may not apply to requests. Recognized flags are:
(``ETHTOOL_A_HEADER_FLAGS`` 是所有请求类型共用的请求标志位图。这些标志的解释对
所有请求类型都相同，但未必对所有请求都适用。可识别的标志有：)

  =================================  ===================================
  ``ETHTOOL_FLAG_COMPACT_BITSETS``   use compact format bitsets in reply (在应答中使用紧凑格式位集)
  ``ETHTOOL_FLAG_OMIT_REPLY``        omit optional reply (_SET and _ACT) (省略可选应答(_SET 和 _ACT))
  ``ETHTOOL_FLAG_STATS``             include optional device statistics (包含可选的设备统计信息)
  =================================  ===================================

New request flags should follow the general idea that if the flag is not set,
the behaviour is backward compatible, i.e. requests from old clients not aware
of the flag should be interpreted the way the client expects. A client must
not set flags it does not understand.
(新增请求标志应遵循如下总体原则：未设置该标志时行为保持向后兼容，即不知道该
标志的旧客户端发出的请求应按客户端期望的方式解释。客户端不得设置自己不理解的
标志。)

``ETHTOOL_A_HEADER_PHY_INDEX`` identifies the Ethernet PHY the message relates to.
As there are numerous commands that are related to PHY configuration, and because
there may be more than one PHY on the link, the PHY index can be passed in the
request for the commands that needs it. It is, however, not mandatory, and if it
is not passed for commands that target a PHY, the net_device.phydev pointer
is used.
(``ETHTOOL_A_HEADER_PHY_INDEX`` 标识消息所针对的以太网 PHY。由于与 PHY 配置相关
的命令众多，且链路上可能有不止一个 PHY，需要该信息的命令可在请求中传入 PHY 索引。
但这并非强制；对于以 PHY 为目标的命令，若未传入该索引，则使用 net_device.phydev
指针。)

Bit sets
(位集)
========

For short bitmaps of (reasonably) fixed length, standard ``NLA_BITFIELD32``
type is used. For arbitrary length bitmaps, ethtool netlink uses a nested
attribute with contents of one of two forms: compact (two binary bitmaps
representing bit values and mask of affected bits) and bit-by-bit (list of
bits identified by index or name).
(对于长度(基本)固定的短位图，使用标准的 ``NLA_BITFIELD32`` 类型。对于任意长度
的位图，ethtool netlink 使用一个嵌套属性，其内容为两种形式之一：紧凑形式(两个
二进制位图，分别表示位值和受影响位的掩码)和逐位形式(按索引或名称标识的位列表)。)

Verbose (bit-by-bit) bitsets allow sending symbolic names for bits together
with their values which saves a round trip (when the bitset is passed in a
request) or at least a second request (when the bitset is in a reply). This is
useful for one shot applications like traditional ethtool command. On the
other hand, long running applications like ethtool monitor (displaying
notifications) or network management daemons may prefer fetching the names
only once and using compact form to save message size. Notifications from
ethtool netlink interface always use compact form for bitsets.
(详细(逐位)位集允许把位的符号名连同其值一起发送，从而节省一次往返(当位集随
请求传入时)或至少省去第二次请求(当位集出现在应答中时)。这对传统 ethtool 命令
这类一次性应用很有用。另一方面，ethtool monitor(显示通知)或网络管理守护进程
这类长期运行的应用可能更愿意只获取一次名称，并使用紧凑形式来减小消息尺寸。
ethtool netlink 接口发出的通知中的位集总是使用紧凑形式。)

A bitset can represent either a value/mask pair (``ETHTOOL_A_BITSET_NOMASK``
not set) or a single bitmap (``ETHTOOL_A_BITSET_NOMASK`` set). In requests
modifying a bitmap, the former changes the bit set in mask to values set in
value and preserves the rest; the latter sets the bits set in the bitmap and
clears the rest.
(位集既可以表示"值/掩码"对(未设置 ``ETHTOOL_A_BITSET_NOMASK``)，也可以表示
单个位图(设置了 ``ETHTOOL_A_BITSET_NOMASK``)。在修改位图的请求中，前者把掩码中
置位的位改为 value 中设置的值并保留其余位；后者置位位图中列出的位并清除其余位。)

Compact form: nested (bitset) attribute contents:
(紧凑形式：嵌套(bitset)属性的内容：)

  ============================  ======  ============================
  ``ETHTOOL_A_BITSET_NOMASK``   flag    no mask, only a list (无掩码，仅一个列表)
  ``ETHTOOL_A_BITSET_SIZE``     u32     number of significant bits (有效位数)
  ``ETHTOOL_A_BITSET_VALUE``    binary  bitmap of bit values (位值的位图)
  ``ETHTOOL_A_BITSET_MASK``     binary  bitmap of valid bits (有效位的位图)
  ============================  ======  ============================

Value and mask must have length at least ``ETHTOOL_A_BITSET_SIZE`` bits
rounded up to a multiple of 32 bits. They consist of 32-bit words in host byte
order, words ordered from least significant to most significant (i.e. the same
way as bitmaps are passed with ioctl interface).
(value 和 mask 的长度必须至少为 ``ETHTOOL_A_BITSET_SIZE`` 位，并向上取整到 32
位的倍数。它们由按主机字节序的 32 位字组成，字按从最低有效到最高有效排列(即
与通过 ioctl 接口传递位图的方式相同)。)

For compact form, ``ETHTOOL_A_BITSET_SIZE`` and ``ETHTOOL_A_BITSET_VALUE`` are
mandatory. ``ETHTOOL_A_BITSET_MASK`` attribute is mandatory if
``ETHTOOL_A_BITSET_NOMASK`` is not set (bitset represents a value/mask pair);
if ``ETHTOOL_A_BITSET_NOMASK`` is not set, ``ETHTOOL_A_BITSET_MASK`` is not
allowed (bitset represents a single bitmap.
(对于紧凑形式，``ETHTOOL_A_BITSET_SIZE`` 和 ``ETHTOOL_A_BITSET_VALUE`` 是必需的。
若未设置 ``ETHTOOL_A_BITSET_NOMASK`` (位集表示"值/掩码"对)，则
``ETHTOOL_A_BITSET_MASK`` 属性是必需的；若 ``ETHTOOL_A_BITSET_NOMASK`` 未设置，
则不允许使用 ``ETHTOOL_A_BITSET_MASK`` (位集表示单个位图。)

Kernel bit set length may differ from userspace length if older application is
used on newer kernel or vice versa. If userspace bitmap is longer, an error is
issued only if the request actually tries to set values of some bits not
recognized by kernel.
(若在较新内核上运行较旧的应用(或相反)，内核位集长度可能与用户空间长度不同。
若用户空间位图更长，则只有当请求确实试图设置某些内核无法识别的位的值时才报错。)

Bit-by-bit form: nested (bitset) attribute contents:
(逐位形式：嵌套(bitset)属性的内容：)

 +------------------------------------+--------+-----------------------------+
 | ``ETHTOOL_A_BITSET_NOMASK``        | flag   | no mask, only a list        |
 |                                    |        | (无掩码，仅一个列表)        |
 +------------------------------------+--------+-----------------------------+
 | ``ETHTOOL_A_BITSET_SIZE``          | u32    | number of significant bits  |
 |                                    |        | (有效位数)                  |
 +------------------------------------+--------+-----------------------------+
 | ``ETHTOOL_A_BITSET_BITS``          | nested | array of bits (位数组)      |
 +-+----------------------------------+--------+-----------------------------+
 | | ``ETHTOOL_A_BITSET_BITS_BIT+``   | nested | one bit (一个位)            |
 +-+-+--------------------------------+--------+-----------------------------+
 | | | ``ETHTOOL_A_BITSET_BIT_INDEX`` | u32    | bit index (0 for LSB)       |
 | | |                                |        | (位索引，0 为最低位)        |
 +-+-+--------------------------------+--------+-----------------------------+
 | | | ``ETHTOOL_A_BITSET_BIT_NAME``  | string | bit name (位名称)           |
 +-+-+--------------------------------+--------+-----------------------------+
 | | | ``ETHTOOL_A_BITSET_BIT_VALUE`` | flag   | present if bit is set       |
 | | |                                |        | (若该位已置位则存在)        |
 +-+-+--------------------------------+--------+-----------------------------+

For bit-by-bit form, ``ETHTOOL_A_BITSET_SIZE`` is optional, and
``ETHTOOL_A_BITSET_BITS`` is mandatory. ``ETHTOOL_A_BITSET_BITS`` nest can
only contain ``ETHTOOL_A_BITSET_BITS_BIT`` attributes but there can be an
arbitrary number of them.  A bit may be identified by its index or by its
name. When used in requests, listed bits are set to 0 or 1 according to
``ETHTOOL_A_BITSET_BIT_VALUE``, the rest is preserved.

A request fails if index exceeds kernel bit length or if name is not
recognized. If both name and index are set, the request will fail if they
point to different bits.

When ``ETHTOOL_A_BITSET_NOMASK`` flag is present, bitset is interpreted as
a simple bitmap. ``ETHTOOL_A_BITSET_BIT_VALUE`` attributes are not used in
such case. Such bitset represents a bitmap with listed bits set and the rest
zero.

In requests, application can use either form. Form used by kernel in reply is
determined by ``ETHTOOL_FLAG_COMPACT_BITSETS`` flag in flags field of request
header. Semantics of value and mask depends on the attribute.


List of message types
(消息类型列表)
=====================

All constants identifying message types use ``ETHTOOL_CMD_`` prefix and suffix
according to message purpose:
(所有标识消息类型的常量都以 ``ETHTOOL_CMD_`` 为前缀，并按消息用途加后缀：)

  ==============    ======================================
  ``_GET``          userspace request to retrieve data (用户空间获取数据的请求)
  ``_SET``          userspace request to set data (用户空间设置数据的请求)
  ``_ACT``          userspace request to perform an action (用户空间执行动作的请求)
  ``_GET_REPLY``    kernel reply to a ``GET`` request (内核对 ``GET`` 请求的应答)
  ``_SET_REPLY``    kernel reply to a ``SET`` request (内核对 ``SET`` 请求的应答)
  ``_ACT_REPLY``    kernel reply to an ``ACT`` request (内核对 ``ACT`` 请求的应答)
  ``_NTF``          kernel notification (内核通知)
  ==============    ======================================

Userspace to kernel:
(用户空间到内核：)

  ===================================== =================================
  ``ETHTOOL_MSG_STRSET_GET``            get string set (获取字符串集)
  ``ETHTOOL_MSG_LINKINFO_GET``          get link settings (获取链路设置)
  ``ETHTOOL_MSG_LINKINFO_SET``          set link settings (设置链路设置)
  ``ETHTOOL_MSG_LINKMODES_GET``         get link modes info (获取链路模式信息)
  ``ETHTOOL_MSG_LINKMODES_SET``         set link modes info (设置链路模式信息)
  ``ETHTOOL_MSG_LINKSTATE_GET``         get link state (获取链路状态)
  ``ETHTOOL_MSG_DEBUG_GET``             get debugging settings (获取调试设置)
  ``ETHTOOL_MSG_DEBUG_SET``             set debugging settings (设置调试设置)
  ``ETHTOOL_MSG_WOL_GET``               get wake-on-lan settings (获取网络唤醒设置)
  ``ETHTOOL_MSG_WOL_SET``               set wake-on-lan settings (设置网络唤醒设置)
  ``ETHTOOL_MSG_FEATURES_GET``          get device features (获取设备特性)
  ``ETHTOOL_MSG_FEATURES_SET``          set device features (设置设备特性)
  ``ETHTOOL_MSG_PRIVFLAGS_GET``         get private flags (获取私有标志)
  ``ETHTOOL_MSG_PRIVFLAGS_SET``         set private flags (设置私有标志)
  ``ETHTOOL_MSG_RINGS_GET``             get ring sizes (获取环大小)
  ``ETHTOOL_MSG_RINGS_SET``             set ring sizes (设置环大小)
  ``ETHTOOL_MSG_CHANNELS_GET``          get channel counts (获取通道数量)
  ``ETHTOOL_MSG_CHANNELS_SET``          set channel counts (设置通道数量)
  ``ETHTOOL_MSG_COALESCE_GET``          get coalescing parameters (获取合并参数)
  ``ETHTOOL_MSG_COALESCE_SET``          set coalescing parameters (设置合并参数)
  ``ETHTOOL_MSG_PAUSE_GET``             get pause parameters (获取暂停参数)
  ``ETHTOOL_MSG_PAUSE_SET``             set pause parameters (设置暂停参数)
  ``ETHTOOL_MSG_EEE_GET``               get EEE settings (获取 EEE 设置)
  ``ETHTOOL_MSG_EEE_SET``               set EEE settings (设置 EEE 设置)
  ``ETHTOOL_MSG_TSINFO_GET``		get timestamping info (获取时间戳信息)
  ``ETHTOOL_MSG_CABLE_TEST_ACT``        action start cable test (动作：启动线缆测试)
  ``ETHTOOL_MSG_CABLE_TEST_TDR_ACT``    action start raw TDR cable test (动作：启动原始 TDR 线缆测试)
  ``ETHTOOL_MSG_TUNNEL_INFO_GET``       get tunnel offload info (获取隧道卸载信息)
  ``ETHTOOL_MSG_FEC_GET``               get FEC settings (获取 FEC 设置)
  ``ETHTOOL_MSG_FEC_SET``               set FEC settings (设置 FEC 设置)
  ``ETHTOOL_MSG_MODULE_EEPROM_GET``     read SFP module EEPROM (读取 SFP 模块 EEPROM)
  ``ETHTOOL_MSG_STATS_GET``             get standard statistics (获取标准统计信息)
  ``ETHTOOL_MSG_PHC_VCLOCKS_GET``       get PHC virtual clocks info (获取 PHC 虚拟时钟信息)
  ``ETHTOOL_MSG_MODULE_SET``            set transceiver module parameters (设置收发器模块参数)
  ``ETHTOOL_MSG_MODULE_GET``            get transceiver module parameters (获取收发器模块参数)
  ``ETHTOOL_MSG_PSE_SET``               set PSE parameters (设置 PSE 参数)
  ``ETHTOOL_MSG_PSE_GET``               get PSE parameters (获取 PSE 参数)
  ``ETHTOOL_MSG_RSS_GET``               get RSS settings (获取 RSS 设置)
  ``ETHTOOL_MSG_PLCA_GET_CFG``          get PLCA RS parameters (获取 PLCA RS 参数)
  ``ETHTOOL_MSG_PLCA_SET_CFG``          set PLCA RS parameters (设置 PLCA RS 参数)
  ``ETHTOOL_MSG_PLCA_GET_STATUS``       get PLCA RS status (获取 PLCA RS 状态)
  ``ETHTOOL_MSG_MM_GET``                get MAC merge layer state (获取 MAC 合并层状态)
  ``ETHTOOL_MSG_MM_SET``                set MAC merge layer parameters (设置 MAC 合并层参数)
  ``ETHTOOL_MSG_MODULE_FW_FLASH_ACT``   flash transceiver module firmware (烧录收发器模块固件)
  ``ETHTOOL_MSG_PHY_GET``               get Ethernet PHY information (获取以太网 PHY 信息)
  ``ETHTOOL_MSG_TSCONFIG_GET``          get hw timestamping configuration (获取硬件时间戳配置)
  ``ETHTOOL_MSG_TSCONFIG_SET``          set hw timestamping configuration (设置硬件时间戳配置)
  ``ETHTOOL_MSG_RSS_SET``               set RSS settings (设置 RSS 设置)
  ``ETHTOOL_MSG_RSS_CREATE_ACT``        create an additional RSS context (创建附加 RSS 上下文)
  ``ETHTOOL_MSG_RSS_DELETE_ACT``        delete an additional RSS context (删除附加 RSS 上下文)
  ``ETHTOOL_MSG_MSE_GET``               get MSE diagnostic data (获取 MSE 诊断数据)
  ===================================== =================================

Kernel to userspace:
(内核到用户空间：)

  ======================================== =================================
  ``ETHTOOL_MSG_STRSET_GET_REPLY``         string set contents (字符串集内容)
  ``ETHTOOL_MSG_LINKINFO_GET_REPLY``       link settings (链路设置)
  ``ETHTOOL_MSG_LINKINFO_NTF``             link settings notification (链路设置通知)
  ``ETHTOOL_MSG_LINKMODES_GET_REPLY``      link modes info (链路模式信息)
  ``ETHTOOL_MSG_LINKMODES_NTF``            link modes notification (链路模式通知)
  ``ETHTOOL_MSG_LINKSTATE_GET_REPLY``      link state info (链路状态信息)
  ``ETHTOOL_MSG_DEBUG_GET_REPLY``          debugging settings (调试设置)
  ``ETHTOOL_MSG_DEBUG_NTF``                debugging settings notification (调试设置通知)
  ``ETHTOOL_MSG_WOL_GET_REPLY``            wake-on-lan settings (网络唤醒设置)
  ``ETHTOOL_MSG_WOL_NTF``                  wake-on-lan settings notification (网络唤醒设置通知)
  ``ETHTOOL_MSG_FEATURES_GET_REPLY``       device features (设备特性)
  ``ETHTOOL_MSG_FEATURES_SET_REPLY``       optional reply to FEATURES_SET (对 FEATURES_SET 的可选应答)
  ``ETHTOOL_MSG_FEATURES_NTF``             netdev features notification (netdev 特性通知)
  ``ETHTOOL_MSG_PRIVFLAGS_GET_REPLY``      private flags (私有标志)
  ``ETHTOOL_MSG_PRIVFLAGS_NTF``            private flags (私有标志)
  ``ETHTOOL_MSG_RINGS_GET_REPLY``          ring sizes (环大小)
  ``ETHTOOL_MSG_RINGS_NTF``                ring sizes (环大小)
  ``ETHTOOL_MSG_CHANNELS_GET_REPLY``       channel counts (通道数量)
  ``ETHTOOL_MSG_CHANNELS_NTF``             channel counts (通道数量)
  ``ETHTOOL_MSG_COALESCE_GET_REPLY``       coalescing parameters (合并参数)
  ``ETHTOOL_MSG_COALESCE_NTF``             coalescing parameters (合并参数)
  ``ETHTOOL_MSG_PAUSE_GET_REPLY``          pause parameters (暂停参数)
  ``ETHTOOL_MSG_PAUSE_NTF``                pause parameters (暂停参数)
  ``ETHTOOL_MSG_EEE_GET_REPLY``            EEE settings (EEE 设置)
  ``ETHTOOL_MSG_EEE_NTF``                  EEE settings (EEE 设置)
  ``ETHTOOL_MSG_TSINFO_GET_REPLY``         timestamping info (时间戳信息)
  ``ETHTOOL_MSG_CABLE_TEST_NTF``           Cable test results (线缆测试结果)
  ``ETHTOOL_MSG_CABLE_TEST_TDR_NTF``       Cable test TDR results (线缆测试 TDR 结果)
  ``ETHTOOL_MSG_TUNNEL_INFO_GET_REPLY``    tunnel offload info (隧道卸载信息)
  ``ETHTOOL_MSG_FEC_GET_REPLY``            FEC settings (FEC 设置)
  ``ETHTOOL_MSG_FEC_NTF``                  FEC settings (FEC 设置)
  ``ETHTOOL_MSG_MODULE_EEPROM_GET_REPLY``  read SFP module EEPROM (读取 SFP 模块 EEPROM)
  ``ETHTOOL_MSG_STATS_GET_REPLY``          standard statistics (标准统计信息)
  ``ETHTOOL_MSG_PHC_VCLOCKS_GET_REPLY``    PHC virtual clocks info (PHC 虚拟时钟信息)
  ``ETHTOOL_MSG_MODULE_GET_REPLY``         transceiver module parameters (收发器模块参数)
  ``ETHTOOL_MSG_PSE_GET_REPLY``            PSE parameters (PSE 参数)
  ``ETHTOOL_MSG_RSS_GET_REPLY``            RSS settings (RSS 设置)
  ``ETHTOOL_MSG_RSS_NTF``                  RSS settings (RSS 设置)
  ``ETHTOOL_MSG_PLCA_GET_CFG_REPLY``       PLCA RS parameters (PLCA RS 参数)
  ``ETHTOOL_MSG_PLCA_GET_STATUS_REPLY``    PLCA RS status (PLCA RS 状态)
  ``ETHTOOL_MSG_PLCA_NTF``                 PLCA RS parameters (PLCA RS 参数)
  ``ETHTOOL_MSG_MM_GET_REPLY``             MAC merge layer status (MAC 合并层状态)
  ``ETHTOOL_MSG_MODULE_FW_FLASH_NTF``      transceiver module flash updates (收发器模块固件烧录进度更新)
  ``ETHTOOL_MSG_PHY_GET_REPLY``            Ethernet PHY information (以太网 PHY 信息)
  ``ETHTOOL_MSG_PHY_NTF``                  Ethernet PHY information change (以太网 PHY 信息变更)
  ``ETHTOOL_MSG_TSCONFIG_GET_REPLY``       hw timestamping configuration (硬件时间戳配置)
  ``ETHTOOL_MSG_TSCONFIG_SET_REPLY``       new hw timestamping configuration (新的硬件时间戳配置)
  ``ETHTOOL_MSG_PSE_NTF``                  PSE events notification (PSE 事件通知)
  ``ETHTOOL_MSG_RSS_NTF``                  RSS settings notification (RSS 设置通知)
  ``ETHTOOL_MSG_RSS_CREATE_ACT_REPLY``     create an additional RSS context (创建附加 RSS 上下文)
  ``ETHTOOL_MSG_RSS_CREATE_NTF``           additional RSS context created (附加 RSS 上下文已创建)
  ``ETHTOOL_MSG_RSS_DELETE_NTF``           additional RSS context deleted (附加 RSS 上下文已删除)
  ``ETHTOOL_MSG_MSE_GET_REPLY``            MSE diagnostic data (MSE 诊断数据)
  ======================================== =================================

``GET`` requests are sent by userspace applications to retrieve device
information. They usually do not contain any message specific attributes.
Kernel replies with corresponding "GET_REPLY" message. For most types, ``GET``
request with ``NLM_F_DUMP`` and no device identification can be used to query
the information for all devices supporting the request.
(``GET`` 请求由用户空间应用程序发送，用于获取设备信息。它们通常不包含任何消息
特定属性。内核以相应的 "GET_REPLY" 消息应答。对于大多数类型，带 ``NLM_F_DUMP``
且不带设备标识的 ``GET`` 请求可用于向所有支持该请求的设备查询信息。)

If the data can be also modified, corresponding ``SET`` message with the same
layout as corresponding ``GET_REPLY`` is used to request changes. Only
attributes where a change is requested are included in such request (also, not
all attributes may be changed). Replies to most ``SET`` request consist only
of error code and extack; if kernel provides additional data, it is sent in
the form of corresponding ``SET_REPLY`` message which can be suppressed by
setting ``ETHTOOL_FLAG_OMIT_REPLY`` flag in request header.
(如果数据也可以修改，则使用与相应 ``GET_REPLY`` 布局相同的 ``SET`` 消息请求
更改。此类请求只包含请求更改的属性(而且并非所有属性都可更改)。对大多数
``SET`` 请求的应答仅由错误码和 extack 组成；若内核提供附加数据，则以相应
``SET_REPLY`` 消息的形式发送，可通过在请求头中设置 ``ETHTOOL_FLAG_OMIT_REPLY``
标志来抑制该消息。)

Data modification also triggers sending a ``NTF`` message with a notification.
These usually bear only a subset of attributes which was affected by the
change. The same notification is issued if the data is modified using other
means (mostly ioctl ethtool interface). Unlike notifications from ethtool
netlink code which are only sent if something actually changed, notifications
triggered by ioctl interface may be sent even if the request did not actually
change any data.
(数据修改还会触发发送带有通知的 ``NTF`` 消息。这些消息通常只携带受此次更改影响
的属性子集。若数据通过其他途径(主要是 ethtool ioctl 接口)被修改，也会发出同样的
通知。ethtool netlink 代码发出的通知只在确实发生变化时才发送；而由 ioctl 接口
触发的通知即使请求并未真正更改任何数据也可能被发送。)

``ACT`` messages request kernel (driver) to perform a specific action. If some
information is reported by kernel (which can be suppressed by setting
``ETHTOOL_FLAG_OMIT_REPLY`` flag in request header), the reply takes form of
an ``ACT_REPLY`` message. Performing an action also triggers a notification
(``NTF`` message).
(``ACT`` 消息请求内核(驱动程序)执行特定动作。如果内核报告某些信息(可通过在
请求头中设置 ``ETHTOOL_FLAG_OMIT_REPLY`` 标志来抑制)，应答采用 ``ACT_REPLY``
消息的形式。执行动作也会触发一条通知(``NTF`` 消息)。)

Later sections describe the format and semantics of these messages.
(后文各节将描述这些消息的格式和语义。)


STRSET_GET
(STRSET_GET，字符串集获取)
==========

Requests contents of a string set as provided by ioctl commands
``ETHTOOL_GSSET_INFO`` and ``ETHTOOL_GSTRINGS.`` String sets are not user
writeable so that the corresponding ``STRSET_SET`` message is only used in
kernel replies. There are two types of string sets: global (independent of
a device, e.g. device feature names) and device specific (e.g. device private
flags).
(请求获取字符串集的内容，如同 ioctl 命令 ``ETHTOOL_GSSET_INFO`` 和
``ETHTOOL_GSTRINGS.`` 所提供的那样。字符串集不可由用户写入，因此相应的
``STRSET_SET`` 消息只在内核应答中使用。字符串集有两类：全局的(独立于设备，
例如设备特性名)和设备特定的(例如设备私有标志)。)

Request contents:
(请求内容：)

 +---------------------------------------+--------+------------------------+
 | ``ETHTOOL_A_STRSET_HEADER``           | nested | request header (请求头)|
 +---------------------------------------+--------+------------------------+
 | ``ETHTOOL_A_STRSET_STRINGSETS``       | nested | string set to request  |
 |                                       |        | (要请求的字符串集)     |
 +-+-------------------------------------+--------+------------------------+
 | | ``ETHTOOL_A_STRINGSETS_STRINGSET+`` | nested | one string set         |
 | |                                     |        | (一个字符串集)         |
 +-+-+-----------------------------------+--------+------------------------+
 | | | ``ETHTOOL_A_STRINGSET_ID``        | u32    | set id (集合 ID)       |
 +-+-+-----------------------------------+--------+------------------------+

Kernel response contents:
(内核响应内容：)

 +---------------------------------------+--------+-----------------------+
 | ``ETHTOOL_A_STRSET_HEADER``           | nested | reply header (应答头) |
 +---------------------------------------+--------+-----------------------+
 | ``ETHTOOL_A_STRSET_STRINGSETS``       | nested | array of string sets  |
 |                                       |        | (字符串集数组)        |
 +-+-------------------------------------+--------+-----------------------+
 | | ``ETHTOOL_A_STRINGSETS_STRINGSET+`` | nested | one string set        |
 | |                                     |        | (一个字符串集)        |
 +-+-+-----------------------------------+--------+-----------------------+
 | | | ``ETHTOOL_A_STRINGSET_ID``        | u32    | set id (集合 ID)      |
 +-+-+-----------------------------------+--------+-----------------------+
 | | | ``ETHTOOL_A_STRINGSET_COUNT``     | u32    | number of strings     |
 | | |                                   |        | (字符串数量)          |
 +-+-+-----------------------------------+--------+-----------------------+
 | | | ``ETHTOOL_A_STRINGSET_STRINGS``   | nested | array of strings      |
 | | |                                   |        | (字符串数组)          |
 +-+-+-+---------------------------------+--------+-----------------------+
 | | | | ``ETHTOOL_A_STRINGS_STRING+``   | nested | one string            |
 | | | |                                 |        | (一个字符串)          |
 +-+-+-+-+-------------------------------+--------+-----------------------+
 | | | | | ``ETHTOOL_A_STRING_INDEX``    | u32    | string index          |
 | | | | |                               |        | (字符串索引)          |
 +-+-+-+-+-------------------------------+--------+-----------------------+
 | | | | | ``ETHTOOL_A_STRING_VALUE``    | string | string value          |
 | | | | |                               |        | (字符串值)            |
 +-+-+-+-+-------------------------------+--------+-----------------------+
 | ``ETHTOOL_A_STRSET_COUNTS_ONLY``      | flag   | return only counts    |
 |                                       |        | (仅返回计数)          |
 +---------------------------------------+--------+-----------------------+

Device identification in request header is optional. Depending on its presence
a and ``NLM_F_DUMP`` flag, there are three type of ``STRSET_GET`` requests:
(请求头中的设备标识是可选的。根据其是否存在以及 ``NLM_F_DUMP`` 标志，
``STRSET_GET`` 请求有三种类型：)

 - no ``NLM_F_DUMP,`` no device: get "global" stringsets
   (无 ``NLM_F_DUMP,``、无设备：获取"全局"字符串集)
 - no ``NLM_F_DUMP``, with device: get string sets related to the device
   (无 ``NLM_F_DUMP``、有设备：获取与该设备相关的字符串集)
 - ``NLM_F_DUMP``, no device: get device related string sets for all devices
   (``NLM_F_DUMP``、无设备：获取所有设备的设备相关字符串集)

If there is no ``ETHTOOL_A_STRSET_STRINGSETS`` array, all string sets of
requested type are returned, otherwise only those specified in the request.
Flag ``ETHTOOL_A_STRSET_COUNTS_ONLY`` tells kernel to only return string
counts of the sets, not the actual strings.
(若没有 ``ETHTOOL_A_STRSET_STRINGSETS`` 数组，则返回所请求类型的全部字符串集，
否则只返回请求中指定的那些。标志 ``ETHTOOL_A_STRSET_COUNTS_ONLY`` 告诉内核只
返回各集合的字符串数量，而不返回实际字符串。)


LINKINFO_GET
(LINKINFO_GET，链路信息获取)
============

Requests link settings as provided by ``ETHTOOL_GLINKSETTINGS`` except for
link modes and autonegotiation related information. The request does not use
any attributes.
(请求获取 ``ETHTOOL_GLINKSETTINGS`` 所提供的链路设置，但不包括链路模式和自动
协商相关信息。该请求不使用任何属性。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_LINKINFO_HEADER``         nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_LINKINFO_HEADER``         nested  reply header (应答头)
  ``ETHTOOL_A_LINKINFO_PORT``           u8      physical port (物理端口)
  ``ETHTOOL_A_LINKINFO_PHYADDR``        u8      phy MDIO address (PHY MDIO 地址)
  ``ETHTOOL_A_LINKINFO_TP_MDIX``        u8      MDI(-X) status (MDI(-X) 状态)
  ``ETHTOOL_A_LINKINFO_TP_MDIX_CTRL``   u8      MDI(-X) control (MDI(-X) 控制)
  ``ETHTOOL_A_LINKINFO_TRANSCEIVER``    u8      transceiver (收发器)
  ====================================  ======  ==========================

Attributes and their values have the same meaning as matching members of the
corresponding ioctl structures.
(这些属性及其取值的含义与相应 ioctl 结构中的对应成员相同。)

``LINKINFO_GET`` allows dump requests (kernel returns reply message for all
devices supporting the request).
(``LINKINFO_GET`` 允许转储请求(内核为所有支持该请求的设备返回应答消息)。)


LINKINFO_SET
(LINKINFO_SET，链路信息设置)
============

``LINKINFO_SET`` request allows setting some of the attributes reported by
``LINKINFO_GET``.
(``LINKINFO_SET`` 请求允许设置 ``LINKINFO_GET`` 所报告的部分属性。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_LINKINFO_HEADER``         nested  request header (请求头)
  ``ETHTOOL_A_LINKINFO_PORT``           u8      physical port (物理端口)
  ``ETHTOOL_A_LINKINFO_PHYADDR``        u8      phy MDIO address (PHY MDIO 地址)
  ``ETHTOOL_A_LINKINFO_TP_MDIX_CTRL``   u8      MDI(-X) control (MDI(-X) 控制)
  ====================================  ======  ==========================

MDI(-X) status and transceiver cannot be set, request with the corresponding
attributes is rejected.
(MDI(-X) 状态和收发器不可设置，包含相应属性的请求会被拒绝。)


LINKMODES_GET
(LINKMODES_GET，链路模式获取)
=============

Requests link modes (supported, advertised and peer advertised) and related
information (autonegotiation status, link speed and duplex) as provided by
``ETHTOOL_GLINKSETTINGS``. The request does not use any attributes.
(请求获取 ``ETHTOOL_GLINKSETTINGS`` 所提供的链路模式(支持的、通告的与对端通告
的)及相关信息(自动协商状态、链路速率和双工模式)。该请求不使用任何属性。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_LINKMODES_HEADER``        nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ==========================================  ======  ==========================
  ``ETHTOOL_A_LINKMODES_HEADER``              nested  reply header (应答头)
  ``ETHTOOL_A_LINKMODES_AUTONEG``             u8      autonegotiation status (自动协商状态)
  ``ETHTOOL_A_LINKMODES_OURS``                bitset  advertised link modes (本端通告的链路模式)
  ``ETHTOOL_A_LINKMODES_PEER``                bitset  partner link modes (对端链路模式)
  ``ETHTOOL_A_LINKMODES_SPEED``               u32     link speed (Mb/s) (链路速率(Mb/s))
  ``ETHTOOL_A_LINKMODES_DUPLEX``              u8      duplex mode (双工模式)
  ``ETHTOOL_A_LINKMODES_MASTER_SLAVE_CFG``    u8      Master/slave port mode (主/从端口模式)
  ``ETHTOOL_A_LINKMODES_MASTER_SLAVE_STATE``  u8      Master/slave port state (主/从端口状态)
  ``ETHTOOL_A_LINKMODES_RATE_MATCHING``       u8      PHY rate matching (PHY 速率匹配)
  ==========================================  ======  ==========================

For ``ETHTOOL_A_LINKMODES_OURS``, value represents advertised modes and mask
represents supported modes. ``ETHTOOL_A_LINKMODES_PEER`` in the reply is a bit
list.
(对于 ``ETHTOOL_A_LINKMODES_OURS``，value 表示通告的模式，mask 表示支持的模式。
应答中的 ``ETHTOOL_A_LINKMODES_PEER`` 是一个位列表。)

``LINKMODES_GET`` allows dump requests (kernel returns reply messages for all
devices supporting the request).
(``LINKMODES_GET`` 允许转储请求(内核为所有支持该请求的设备返回应答消息)。)


LINKMODES_SET
(LINKMODES_SET，链路模式设置)
=============

Request contents:
(请求内容：)

  ==========================================  ======  ==========================
  ``ETHTOOL_A_LINKMODES_HEADER``              nested  request header (请求头)
  ``ETHTOOL_A_LINKMODES_AUTONEG``             u8      autonegotiation status (自动协商状态)
  ``ETHTOOL_A_LINKMODES_OURS``                bitset  advertised link modes (本端通告的链路模式)
  ``ETHTOOL_A_LINKMODES_PEER``                bitset  partner link modes (对端链路模式)
  ``ETHTOOL_A_LINKMODES_SPEED``               u32     link speed (Mb/s) (链路速率(Mb/s))
  ``ETHTOOL_A_LINKMODES_DUPLEX``              u8      duplex mode (双工模式)
  ``ETHTOOL_A_LINKMODES_MASTER_SLAVE_CFG``    u8      Master/slave port mode (主/从端口模式)
  ``ETHTOOL_A_LINKMODES_RATE_MATCHING``       u8      PHY rate matching (PHY 速率匹配)
  ``ETHTOOL_A_LINKMODES_LANES``               u32     lanes (lane 数)
  ==========================================  ======  ==========================

``ETHTOOL_A_LINKMODES_OURS`` bit set allows setting advertised link modes. If
autonegotiation is on (either set now or kept from before), advertised modes
are not changed (no ``ETHTOOL_A_LINKMODES_OURS`` attribute) and at least one
of speed, duplex and lanes is specified, kernel adjusts advertised modes to all
supported modes matching speed, duplex, lanes or all (whatever is specified).
This autoselection is done on ethtool side with ioctl interface, netlink
interface is supposed to allow requesting changes without knowing what exactly
kernel supports.
(``ETHTOOL_A_LINKMODES_OURS`` 位集允许设置通告的链路模式。若自动协商开启(无论是
现在设置还是此前保持)，且未更改通告模式(没有 ``ETHTOOL_A_LINKMODES_OURS`` 属性)
而指定了速率、双工和 lane 数中的至少一项，内核会把通告模式调整为所有与速率、
双工、lane 数(或全部指定项)相匹配的支持模式。这种自动选择原本由 ethtool 侧通过
ioctl 接口完成；netlink 接口的目标是让用户无需确切知道内核支持什么就能请求
更改。)


LINKSTATE_GET
(LINKSTATE_GET，链路状态获取)
=============

Requests link state information. Link up/down flag (as provided by
``ETHTOOL_GLINK`` ioctl command) is provided. Optionally, extended state might
be provided as well. In general, extended state describes reasons for why a port
is down, or why it operates in some non-obvious mode. This request does not have
any attributes.
(请求获取链路状态信息。提供链路 up/down 标志(由 ``ETHTOOL_GLINK`` ioctl 命令
提供)。此外还可以选择性地提供扩展状态。总体而言，扩展状态描述端口处于 down
状态的原因，或其以某种非显而易见模式运行的原因。该请求没有任何属性。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_LINKSTATE_HEADER``        nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ============================
  ``ETHTOOL_A_LINKSTATE_HEADER``        nested  reply header (应答头)
  ``ETHTOOL_A_LINKSTATE_LINK``          bool    link state (up/down) (链路状态(up/down))
  ``ETHTOOL_A_LINKSTATE_SQI``           u32     Current Signal Quality Index (当前信号质量指数)
  ``ETHTOOL_A_LINKSTATE_SQI_MAX``       u32     Max support SQI value (支持的最大 SQI 值)
  ``ETHTOOL_A_LINKSTATE_EXT_STATE``     u8      link extended state (链路扩展状态)
  ``ETHTOOL_A_LINKSTATE_EXT_SUBSTATE``  u8      link extended substate (链路扩展子状态)
  ``ETHTOOL_A_LINKSTATE_EXT_DOWN_CNT``  u32     count of link down events (链路 down 事件计数)
  ====================================  ======  ============================

For most NIC drivers, the value of ``ETHTOOL_A_LINKSTATE_LINK`` returns
carrier flag provided by ``netif_carrier_ok()`` but there are drivers which
define their own handler.
(对大多数 NIC 驱动而言，``ETHTOOL_A_LINKSTATE_LINK`` 的值返回
``netif_carrier_ok()`` 提供的载波标志，但也有驱动定义了自己的处理逻辑。)

``ETHTOOL_A_LINKSTATE_EXT_STATE`` and ``ETHTOOL_A_LINKSTATE_EXT_SUBSTATE`` are
optional values. ethtool core can provide either both
``ETHTOOL_A_LINKSTATE_EXT_STATE`` and ``ETHTOOL_A_LINKSTATE_EXT_SUBSTATE``,
or only ``ETHTOOL_A_LINKSTATE_EXT_STATE``, or none of them.
(``ETHTOOL_A_LINKSTATE_EXT_STATE`` 和 ``ETHTOOL_A_LINKSTATE_EXT_SUBSTATE`` 是
可选值。ethtool 核心可以同时提供 ``ETHTOOL_A_LINKSTATE_EXT_STATE`` 和
``ETHTOOL_A_LINKSTATE_EXT_SUBSTATE``，也可以只提供
``ETHTOOL_A_LINKSTATE_EXT_STATE``，或两者都不提供。)

``LINKSTATE_GET`` allows dump requests (kernel returns reply messages for all
devices supporting the request).
(``LINKSTATE_GET`` 允许转储请求(内核为所有支持该请求的设备返回应答消息)。)


Link extended states:
(链路扩展状态：)

  ================================================      ============================================
  ``ETHTOOL_LINK_EXT_STATE_AUTONEG``                    States relating to the autonegotiation or
                                                        issues therein (与自动协商及其内部问题有关的状态)

  ``ETHTOOL_LINK_EXT_STATE_LINK_TRAINING_FAILURE``      Failure during link training (链路训练期间发生故障)

  ``ETHTOOL_LINK_EXT_STATE_LINK_LOGICAL_MISMATCH``      Logical mismatch in physical coding sublayer
                                                        or forward error correction sublayer (物理编码子层或前向纠错子层中存在逻辑失配)

  ``ETHTOOL_LINK_EXT_STATE_BAD_SIGNAL_INTEGRITY``       Signal integrity issues (信号完整性问题)

  ``ETHTOOL_LINK_EXT_STATE_NO_CABLE``                   No cable connected (未连接线缆)

  ``ETHTOOL_LINK_EXT_STATE_CABLE_ISSUE``                Failure is related to cable,
                                                        e.g., unsupported cable (故障与线缆有关，例如不支持的线缆)

  ``ETHTOOL_LINK_EXT_STATE_EEPROM_ISSUE``               Failure is related to EEPROM, e.g., failure
                                                        during reading or parsing the data (故障与 EEPROM 有关，例如读取或解析数据时发生故障)

  ``ETHTOOL_LINK_EXT_STATE_CALIBRATION_FAILURE``        Failure during calibration algorithm (校准算法期间发生故障)

  ``ETHTOOL_LINK_EXT_STATE_POWER_BUDGET_EXCEEDED``      The hardware is not able to provide the
                                                        power required from cable or module (硬件无法提供线缆或模块所需的功率)

  ``ETHTOOL_LINK_EXT_STATE_OVERHEAT``                   The module is overheated (模块过热)

  ``ETHTOOL_LINK_EXT_STATE_MODULE``                     Transceiver module issue (收发器模块问题)
  ================================================      ============================================

Link extended substates:
(链路扩展子状态：)

  Autoneg substates:
  (自动协商子状态：)

  ===============================================================   ================================
  ``ETHTOOL_LINK_EXT_SUBSTATE_AN_NO_PARTNER_DETECTED``              Peer side is down (对端处于 down 状态)

  ``ETHTOOL_LINK_EXT_SUBSTATE_AN_ACK_NOT_RECEIVED``                 Ack not received from peer side (未收到对端的确认)

  ``ETHTOOL_LINK_EXT_SUBSTATE_AN_NEXT_PAGE_EXCHANGE_FAILED``        Next page exchange failed (下一页交换失败)

  ``ETHTOOL_LINK_EXT_SUBSTATE_AN_NO_PARTNER_DETECTED_FORCE_MODE``   Peer side is down during force
                                                                    mode or there is no agreement of
                                                                    speed (强制模式下对端处于 down 状态，或双方未就速率达成一致)

  ``ETHTOOL_LINK_EXT_SUBSTATE_AN_FEC_MISMATCH_DURING_OVERRIDE``     Forward error correction modes
                                                                    in both sides are mismatched (双方的前向纠错模式不匹配)

  ``ETHTOOL_LINK_EXT_SUBSTATE_AN_NO_HCD``                           No Highest Common Denominator (没有最高公分母)
  ===============================================================   ================================

  Link training substates:
  (链路训练子状态：)

  ===========================================================================   ====================
  ``ETHTOOL_LINK_EXT_SUBSTATE_LT_KR_FRAME_LOCK_NOT_ACQUIRED``                    Frames were not
                                                                                 recognized, the
                                                                                 lock failed (未能识别帧，锁定失败)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LT_KR_LINK_INHIBIT_TIMEOUT``                       The lock did not
                                                                                 occur before
                                                                                 timeout (超时前未完成锁定)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LT_KR_LINK_PARTNER_DID_NOT_SET_RECEIVER_READY``    Peer side did not
                                                                                 send ready signal
                                                                                 after training
                                                                                 process (训练过程结束后对端未发送就绪信号)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LT_REMOTE_FAULT``                                  Remote side is not
                                                                                 ready yet (远端尚未就绪)
  ===========================================================================   ====================

  Link logical mismatch substates:
  (链路逻辑失配子状态：)

  ================================================================   ===============================
  ``ETHTOOL_LINK_EXT_SUBSTATE_LLM_PCS_DID_NOT_ACQUIRE_BLOCK_LOCK``   Physical coding sublayer was
                                                                     not locked in first phase -
                                                                     block lock (物理编码子层在第一阶段——块锁定——未锁定)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LLM_PCS_DID_NOT_ACQUIRE_AM_LOCK``      Physical coding sublayer was
                                                                     not locked in second phase -
                                                                     alignment markers lock (物理编码子层在第二阶段——对齐标记锁定——未锁定)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LLM_PCS_DID_NOT_GET_ALIGN_STATUS``     Physical coding sublayer did
                                                                     not get align status (物理编码子层未获得对齐状态)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LLM_FC_FEC_IS_NOT_LOCKED``             FC forward error correction is
                                                                     not locked (FC 前向纠错未锁定)

  ``ETHTOOL_LINK_EXT_SUBSTATE_LLM_RS_FEC_IS_NOT_LOCKED``             RS forward error correction is
                                                                     not locked (RS 前向纠错未锁定)
  ================================================================   ===============================

  Bad signal integrity substates:
  (信号完整性不良子状态：)

  =================================================================    =============================
  ``ETHTOOL_LINK_EXT_SUBSTATE_BSI_LARGE_NUMBER_OF_PHYSICAL_ERRORS``    Large number of physical
                                                                       errors (大量物理错误)

  ``ETHTOOL_LINK_EXT_SUBSTATE_BSI_UNSUPPORTED_RATE``                   The system attempted to
                                                                       operate the cable at a rate
                                                                       that is not formally
                                                                       supported, which led to
                                                                       signal integrity issues (系统试图让线缆以非正式支持的速率运行，导致信号完整性问题)

  ``ETHTOOL_LINK_EXT_SUBSTATE_BSI_SERDES_REFERENCE_CLOCK_LOST``        The external clock signal for
                                                                       SerDes is too weak or
                                                                       unavailable. (SerDes 的外部时钟信号太弱或不可用。)

  ``ETHTOOL_LINK_EXT_SUBSTATE_BSI_SERDES_ALOS``                        The received signal for
                                                                       SerDes is too weak because
                                                                       analog loss of signal. (由于模拟信号丢失，SerDes 接收到的信号太弱。)
  =================================================================    =============================

  Cable issue substates:
  (线缆问题子状态：)

  ===================================================   ============================================
  ``ETHTOOL_LINK_EXT_SUBSTATE_CI_UNSUPPORTED_CABLE``    Unsupported cable (不支持的线缆)

  ``ETHTOOL_LINK_EXT_SUBSTATE_CI_CABLE_TEST_FAILURE``   Cable test failure (线缆测试失败)
  ===================================================   ============================================

  Transceiver module issue substates:
  (收发器模块问题子状态：)

  ===================================================   ============================================
  ``ETHTOOL_LINK_EXT_SUBSTATE_MODULE_CMIS_NOT_READY``   The CMIS Module State Machine did not reach
                                                        the ModuleReady state. For example, if the
                                                        module is stuck at ModuleFault state (CMIS 模块状态机未到达 ModuleReady 状态。例如模块卡在 ModuleFault 状态)
  ===================================================   ============================================

DEBUG_GET
(DEBUG_GET，调试设置获取)
=========

Requests debugging settings of a device. At the moment, only message mask is
provided.
(请求获取设备的调试设置。目前只提供消息掩码。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_DEBUG_HEADER``            nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_DEBUG_HEADER``            nested  reply header (应答头)
  ``ETHTOOL_A_DEBUG_MSGMASK``           bitset  message mask (消息掩码)
  ====================================  ======  ==========================

The message mask (``ETHTOOL_A_DEBUG_MSGMASK``) is equal to message level as
provided by ``ETHTOOL_GMSGLVL`` and set by ``ETHTOOL_SMSGLVL`` in ioctl
interface. While it is called message level there for historical reasons, most
drivers and almost all newer drivers use it as a mask of enabled message
classes (represented by ``NETIF_MSG_*`` constants); therefore netlink
interface follows its actual use in practice.
(消息掩码(``ETHTOOL_A_DEBUG_MSGMASK``)等同于 ioctl 接口中由 ``ETHTOOL_GMSGLVL``
提供并由 ``ETHTOOL_SMSGLVL`` 设置的消息级别。虽然由于历史原因在那里它被称为消息
级别，但大多数驱动以及几乎所有较新的驱动都把它用作已启用消息类(由
``NETIF_MSG_*`` 常量表示)的掩码；因此 netlink 接口遵循它在实践中的实际用法。)

``DEBUG_GET`` allows dump requests (kernel returns reply messages for all
devices supporting the request).
(``DEBUG_GET`` 允许转储请求(内核为所有支持该请求的设备返回应答消息)。)


DEBUG_SET
(DEBUG_SET，设置调试设置)
=========

Set or update debugging settings of a device. At the moment, only message mask
is supported.
(设置或更新设备的调试设置。目前只支持消息掩码。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_DEBUG_HEADER``            nested  request header (请求头)
  ``ETHTOOL_A_DEBUG_MSGMASK``           bitset  message mask (消息掩码)
  ====================================  ======  ==========================

``ETHTOOL_A_DEBUG_MSGMASK`` bit set allows setting or modifying mask of
enabled debugging message types for the device.
(``ETHTOOL_A_DEBUG_MSGMASK`` 位集允许设置或修改设备已启用的调试消息类型掩码。)


WOL_GET
(WOL_GET，网络唤醒设置获取)
=======

Query device wake-on-lan settings. Unlike most "GET" type requests,
``ETHTOOL_MSG_WOL_GET`` requires (netns) ``CAP_NET_ADMIN`` privileges as it
(potentially) provides SecureOn(tm) password which is confidential.
(查询设备的网络唤醒设置。与大多数 "GET" 类型请求不同，``ETHTOOL_MSG_WOL_GET``
需要(网络命名空间中的)``CAP_NET_ADMIN`` 特权，因为它(可能)提供机密的
SecureOn(tm) 密码。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_WOL_HEADER``              nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_WOL_HEADER``              nested  reply header (应答头)
  ``ETHTOOL_A_WOL_MODES``               bitset  mask of enabled WoL modes (已启用 WoL 模式的掩码)
  ``ETHTOOL_A_WOL_SOPASS``              binary  SecureOn(tm) password (SecureOn(tm) 密码)
  ====================================  ======  ==========================

In reply, ``ETHTOOL_A_WOL_MODES`` mask consists of modes supported by the
device, value of modes which are enabled. ``ETHTOOL_A_WOL_SOPASS`` is only
included in reply if ``WAKE_MAGICSECURE`` mode is supported.
(在应答中，``ETHTOOL_A_WOL_MODES`` 的掩码为设备支持的模式，值为已启用的模式。
``ETHTOOL_A_WOL_SOPASS`` 仅在支持 ``WAKE_MAGICSECURE`` 模式时才包含在应答中。)


WOL_SET
(WOL_SET，设置网络唤醒设置)
=======

Set or update wake-on-lan settings.
(设置或更新网络唤醒设置。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_WOL_HEADER``              nested  request header (请求头)
  ``ETHTOOL_A_WOL_MODES``               bitset  enabled WoL modes (已启用的 WoL 模式)
  ``ETHTOOL_A_WOL_SOPASS``              binary  SecureOn(tm) password (SecureOn(tm) 密码)
  ====================================  ======  ==========================

``ETHTOOL_A_WOL_SOPASS`` is only allowed for devices supporting
``WAKE_MAGICSECURE`` mode.
(``ETHTOOL_A_WOL_SOPASS`` 仅允许用于支持 ``WAKE_MAGICSECURE`` 模式的设备。)


FEATURES_GET
(FEATURES_GET，特性获取)
============

Gets netdev features like ``ETHTOOL_GFEATURES`` ioctl request.
(获取 netdev 特性，如同 ``ETHTOOL_GFEATURES`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_FEATURES_HEADER``         nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_FEATURES_HEADER``         nested  reply header (应答头)
  ``ETHTOOL_A_FEATURES_HW``             bitset  dev->hw_features
  ``ETHTOOL_A_FEATURES_WANTED``         bitset  dev->wanted_features
  ``ETHTOOL_A_FEATURES_ACTIVE``         bitset  dev->features
  ``ETHTOOL_A_FEATURES_NOCHANGE``       bitset  NETIF_F_NEVER_CHANGE
  ====================================  ======  ==========================

Bitmaps in kernel response have the same meaning as bitmaps used in ioctl
interference but attribute names are different (they are based on
corresponding members of struct net_device). Legacy "flags" are not provided,
if userspace needs them (most likely only ethtool for backward compatibility),
it can calculate their values from related feature bits itself.
ETHA_FEATURES_HW uses mask consisting of all features recognized by kernel (to
provide all names when using verbose bitmap format), the other three use no
mask (simple bit lists).
(内核响应中的位图与 ioctl 接口中使用的位图含义相同，但属性名不同(它们基于
struct net_device 的对应成员)。不提供传统的 "flags"；如果用户空间需要它们
(很可能只有 ethtool 出于向后兼容的原因需要)，可以自行根据相关特性位计算其值。
ETHA_FEATURES_HW 使用由内核识别的全部特性组成的掩码(以便在详细位图格式下提供
所有名称)，其余三个不使用掩码(简单位列表)。)


FEATURES_SET
(FEATURES_SET，特性设置)
============

Request to set netdev features like ``ETHTOOL_SFEATURES`` ioctl request.
(请求设置 netdev 特性，如同 ``ETHTOOL_SFEATURES`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_FEATURES_HEADER``         nested  request header (请求头)
  ``ETHTOOL_A_FEATURES_WANTED``         bitset  requested features (请求的特性)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_FEATURES_HEADER``         nested  reply header (应答头)
  ``ETHTOOL_A_FEATURES_WANTED``         bitset  diff wanted vs. result (请求与结果的差异)
  ``ETHTOOL_A_FEATURES_ACTIVE``         bitset  diff old vs. new active (新旧生效特性的差异)
  ====================================  ======  ==========================

Request contains only one bitset which can be either value/mask pair (request
to change specific feature bits and leave the rest) or only a value (request
to set all features to specified set).
(请求只包含一个位集，它既可以是"值/掩码"对(请求更改特定特性位并保留其余
部分)，也可以只有值(请求把所有特性设置为指定集合)。)

As request is subject to netdev_change_features() sanity checks, optional
kernel reply (can be suppressed by ``ETHTOOL_FLAG_OMIT_REPLY`` flag in request
header) informs client about the actual result. ``ETHTOOL_A_FEATURES_WANTED``
reports the difference between client request and actual result: mask consists
of bits which differ between requested features and result (dev->features
after the operation), value consists of values of these bits in the request
(i.e. negated values from resulting features). ``ETHTOOL_A_FEATURES_ACTIVE``
reports the difference between old and new dev->features: mask consists of
bits which have changed, values are their values in new dev->features (after
the operation).
(由于请求要经过 netdev_change_features() 的合理性检查，可选的内核应答(可通过
在请求头中设置 ``ETHTOOL_FLAG_OMIT_REPLY`` 标志来抑制)会告知客户端实际结果。
``ETHTOOL_A_FEATURES_WANTED`` 报告客户端请求与实际结果之间的差异：掩码由请求的
特性与结果(操作后的 dev->features)之间不同的位组成，值由这些位在请求中的值
组成(即取反后的结果特性值)。``ETHTOOL_A_FEATURES_ACTIVE`` 报告新旧
dev->features 之间的差异：掩码由发生变化的位组成，值是它们在新
dev->features(操作后)中的值。)

``ETHTOOL_MSG_FEATURES_NTF`` notification is sent not only if device features
are modified using ``ETHTOOL_MSG_FEATURES_SET`` request or on of ethtool ioctl
request but also each time features are modified with netdev_update_features()
or netdev_change_features().
(``ETHTOOL_MSG_FEATURES_NTF`` 通知不仅在使用 ``ETHTOOL_MSG_FEATURES_SET`` 请求
或某个 ethtool ioctl 请求修改设备特性时发送，而且每次通过
netdev_update_features() 或 netdev_change_features() 修改特性时也会发送。)


PRIVFLAGS_GET
(PRIVFLAGS_GET，私有标志获取)
=============

Gets private flags like ``ETHTOOL_GPFLAGS`` ioctl request.
(获取私有标志，如同 ``ETHTOOL_GPFLAGS`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_PRIVFLAGS_HEADER``        nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_PRIVFLAGS_HEADER``        nested  reply header (应答头)
  ``ETHTOOL_A_PRIVFLAGS_FLAGS``         bitset  private flags (私有标志)
  ====================================  ======  ==========================

``ETHTOOL_A_PRIVFLAGS_FLAGS`` is a bitset with values of device private flags.
These flags are defined by driver, their number and names (and also meaning)
are device dependent. For compact bitset format, names can be retrieved as
``ETH_SS_PRIV_FLAGS`` string set. If verbose bitset format is requested,
response uses all private flags supported by the device as mask so that client
gets the full information without having to fetch the string set with names.
(``ETHTOOL_A_PRIVFLAGS_FLAGS`` 是一个包含设备私有标志值的位集。这些标志由驱动
定义，其数量和名称(以及含义)取决于设备。对于紧凑位集格式，名称可通过
``ETH_SS_PRIV_FLAGS`` 字符串集获取。如果请求详细位集格式，响应会使用设备支持的
全部私有标志作为掩码，这样客户端无需再获取包含名称的字符串集就能得到完整
信息。)


PRIVFLAGS_SET
(PRIVFLAGS_SET，私有标志设置)
=============

Sets or modifies values of device private flags like ``ETHTOOL_SPFLAGS``
ioctl request.
(设置或修改设备私有标志的值，如同 ``ETHTOOL_SPFLAGS`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_PRIVFLAGS_HEADER``        nested  request header (请求头)
  ``ETHTOOL_A_PRIVFLAGS_FLAGS``         bitset  private flags (私有标志)
  ====================================  ======  ==========================

``ETHTOOL_A_PRIVFLAGS_FLAGS`` can either set the whole set of private flags or
modify only values of some of them.
(``ETHTOOL_A_PRIVFLAGS_FLAGS`` 既可以设置整个私有标志集合，也可以只修改其中
一部分的值。)


RINGS_GET
(RINGS_GET，环参数获取)
=========

Gets ring sizes like ``ETHTOOL_GRINGPARAM`` ioctl request.
(获取环大小，如同 ``ETHTOOL_GRINGPARAM`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_RINGS_HEADER``            nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  =======================================   ======  ===========================
  ``ETHTOOL_A_RINGS_HEADER``                nested  reply header (应答头)
  ``ETHTOOL_A_RINGS_RX_MAX``                u32     max size of RX ring (RX 环最大尺寸)
  ``ETHTOOL_A_RINGS_RX_MINI_MAX``           u32     max size of RX mini ring (RX mini 环最大尺寸)
  ``ETHTOOL_A_RINGS_RX_JUMBO_MAX``          u32     max size of RX jumbo ring (RX jumbo 环最大尺寸)
  ``ETHTOOL_A_RINGS_TX_MAX``                u32     max size of TX ring (TX 环最大尺寸)
  ``ETHTOOL_A_RINGS_RX``                    u32     size of RX ring (RX 环大小)
  ``ETHTOOL_A_RINGS_RX_MINI``               u32     size of RX mini ring (RX mini 环大小)
  ``ETHTOOL_A_RINGS_RX_JUMBO``              u32     size of RX jumbo ring (RX jumbo 环大小)
  ``ETHTOOL_A_RINGS_TX``                    u32     size of TX ring (TX 环大小)
  ``ETHTOOL_A_RINGS_RX_BUF_LEN``            u32     size of buffers on the ring (环上缓冲区大小)
  ``ETHTOOL_A_RINGS_TCP_DATA_SPLIT``        u8      TCP header / data split (TCP 头/数据分离)
  ``ETHTOOL_A_RINGS_CQE_SIZE``              u32     Size of TX/RX CQE (TX/RX CQE 大小)
  ``ETHTOOL_A_RINGS_TX_PUSH``               u8      flag of TX Push mode (TX Push 模式标志)
  ``ETHTOOL_A_RINGS_RX_PUSH``               u8      flag of RX Push mode (RX Push 模式标志)
  ``ETHTOOL_A_RINGS_TX_PUSH_BUF_LEN``       u32     size of TX push buffer (TX push 缓冲区大小)
  ``ETHTOOL_A_RINGS_TX_PUSH_BUF_LEN_MAX``   u32     max size of TX push buffer (TX push 缓冲区最大尺寸)
  ``ETHTOOL_A_RINGS_HDS_THRESH``            u32     threshold of
                                                    header / data split (头/数据分离阈值)
  ``ETHTOOL_A_RINGS_HDS_THRESH_MAX``        u32     max threshold of
                                                    header / data split (头/数据分离最大阈值)
  =======================================   ======  ===========================

``ETHTOOL_A_RINGS_TCP_DATA_SPLIT`` indicates whether the device is usable with
page-flipping TCP zero-copy receive (``getsockopt(TCP_ZEROCOPY_RECEIVE)``).
If enabled the device is configured to place frame headers and data into
separate buffers. The device configuration must make it possible to receive
full memory pages of data, for example because MTU is high enough or through
HW-GRO.
(``ETHTOOL_A_RINGS_TCP_DATA_SPLIT`` 指示设备是否可用于页面翻转的 TCP 零拷贝
接收(``getsockopt(TCP_ZEROCOPY_RECEIVE)``)。若启用，设备被配置为把帧头和数据
放入不同的缓冲区。设备配置必须能够接收整页内存的数据，例如 MTU 足够高，或
通过 HW-GRO。)

``ETHTOOL_A_RINGS_[RX|TX]_PUSH`` flag is used to enable descriptor fast
path to send or receive packets. In ordinary path, driver fills descriptors in DRAM and
notifies NIC hardware. In fast path, driver pushes descriptors to the device
through MMIO writes, thus reducing the latency. However, enabling this feature
may increase the CPU cost. Drivers may enforce additional per-packet
eligibility checks (e.g. on packet size).
(``ETHTOOL_A_RINGS_[RX|TX]_PUSH`` 标志用于启用发送或接收数据包的描述符快速
路径。在普通路径中，驱动在 DRAM 中填充描述符并通知 NIC 硬件。在快速路径中，
驱动通过 MMIO 写入把描述符推送到设备，从而降低延迟。但启用该特性可能增加 CPU
开销。驱动可以实施额外的逐包资格检查(例如基于数据包大小)。)

``ETHTOOL_A_RINGS_TX_PUSH_BUF_LEN`` specifies the maximum number of bytes of a
transmitted packet a driver can push directly to the underlying device
('push' mode). Pushing some of the payload bytes to the device has the
advantages of reducing latency for small packets by avoiding DMA mapping (same
as ``ETHTOOL_A_RINGS_TX_PUSH`` parameter) as well as allowing the underlying
device to process packet headers ahead of fetching its payload.
This can help the device to make fast actions based on the packet's headers.
This is similar to the "tx-copybreak" parameter, which copies the packet to a
preallocated DMA memory area instead of mapping new memory. However,
tx-push-buff parameter copies the packet directly to the device to allow the
device to take faster actions on the packet.
(``ETHTOOL_A_RINGS_TX_PUSH_BUF_LEN`` 指定驱动可以直接推送到底层设备的传输
数据包的最大字节数("push" 模式)。把部分载荷字节推送到设备的优点是：通过避免
DMA 映射来降低小数据包的延迟(与 ``ETHTOOL_A_RINGS_TX_PUSH`` 参数相同)，并允许
底层设备在获取载荷之前先处理包头。这有助于设备基于包头快速采取行动。这类似于
"tx-copybreak" 参数——后者把数据包复制到预分配的 DMA 内存区域而不是映射新
内存；而 tx-push-buff 参数把数据包直接复制到设备，让设备能更快地对数据包采取
行动。)

RINGS_SET
(RINGS_SET，环参数设置)
=========

Sets ring sizes like ``ETHTOOL_SRINGPARAM`` ioctl request.
(设置环大小，如同 ``ETHTOOL_SRINGPARAM`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ===========================
  ``ETHTOOL_A_RINGS_HEADER``            nested  reply header (应答头)
  ``ETHTOOL_A_RINGS_RX``                u32     size of RX ring (RX 环大小)
  ``ETHTOOL_A_RINGS_RX_MINI``           u32     size of RX mini ring (RX mini 环大小)
  ``ETHTOOL_A_RINGS_RX_JUMBO``          u32     size of RX jumbo ring (RX jumbo 环大小)
  ``ETHTOOL_A_RINGS_TX``                u32     size of TX ring (TX 环大小)
  ``ETHTOOL_A_RINGS_RX_BUF_LEN``        u32     size of buffers on the ring (环上缓冲区大小)
  ``ETHTOOL_A_RINGS_TCP_DATA_SPLIT``    u8      TCP header / data split (TCP 头/数据分离)
  ``ETHTOOL_A_RINGS_CQE_SIZE``          u32     Size of TX/RX CQE (TX/RX CQE 大小)
  ``ETHTOOL_A_RINGS_TX_PUSH``           u8      flag of TX Push mode (TX Push 模式标志)
  ``ETHTOOL_A_RINGS_RX_PUSH``           u8      flag of RX Push mode (RX Push 模式标志)
  ``ETHTOOL_A_RINGS_TX_PUSH_BUF_LEN``   u32     size of TX push buffer (TX push 缓冲区大小)
  ``ETHTOOL_A_RINGS_HDS_THRESH``        u32     threshold of header / data split (头/数据分离阈值)
  ====================================  ======  ===========================

Kernel checks that requested ring sizes do not exceed limits reported by
driver. Driver may impose additional constraints and may not support all
attributes.
(内核检查请求的环大小是否超过驱动报告的限制。驱动可以施加额外约束，也可能
不支持所有属性。)


``ETHTOOL_A_RINGS_CQE_SIZE`` specifies the completion queue event size.
Completion queue events (CQE) are the events posted by NIC to indicate the
completion status of a packet when the packet is sent (like send success or
error) or received (like pointers to packet fragments). The CQE size parameter
enables to modify the CQE size other than default size if NIC supports it.
A bigger CQE can have more receive buffer pointers, and in turn the NIC can
transfer a bigger frame from wire. Based on the NIC hardware, the overall
completion queue size can be adjusted in the driver if CQE size is modified.
(``ETHTOOL_A_RINGS_CQE_SIZE`` 指定完成队列事件的大小。完成队列事件(CQE)是 NIC
发布的事件，用于指示数据包发送时(如发送成功或出错)或接收时(如指向数据包分片
的指针)的完成状态。CQE 大小参数允许在 NIC 支持的情况下把 CQE 大小改为非默认
值。更大的 CQE 可以容纳更多接收缓冲区指针，进而让 NIC 能从线路上传输更大的帧。
根据 NIC 硬件，若修改了 CQE 大小，驱动中可以相应调整整个完成队列的大小。)

``ETHTOOL_A_RINGS_HDS_THRESH`` specifies the threshold value of
header / data split feature. If a received packet size is larger than this
threshold value, header and data will be split.
(``ETHTOOL_A_RINGS_HDS_THRESH`` 指定头/数据分离特性的阈值。若接收到的数据包
大小超过该阈值，头和数据将被分离。)

CHANNELS_GET
(CHANNELS_GET，通道数量获取)
============

Gets channel counts like ``ETHTOOL_GCHANNELS`` ioctl request.
(获取通道数量，如同 ``ETHTOOL_GCHANNELS`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_CHANNELS_HEADER``         nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_CHANNELS_HEADER``          nested  reply header (应答头)
  ``ETHTOOL_A_CHANNELS_RX_MAX``          u32     max receive channels (最大接收通道数)
  ``ETHTOOL_A_CHANNELS_TX_MAX``          u32     max transmit channels (最大发送通道数)
  ``ETHTOOL_A_CHANNELS_OTHER_MAX``       u32     max other channels (最大其他通道数)
  ``ETHTOOL_A_CHANNELS_COMBINED_MAX``    u32     max combined channels (最大合并通道数)
  ``ETHTOOL_A_CHANNELS_RX_COUNT``        u32     receive channel count (接收通道数量)
  ``ETHTOOL_A_CHANNELS_TX_COUNT``        u32     transmit channel count (发送通道数量)
  ``ETHTOOL_A_CHANNELS_OTHER_COUNT``     u32     other channel count (其他通道数量)
  ``ETHTOOL_A_CHANNELS_COMBINED_COUNT``  u32     combined channel count (合并通道数量)
  =====================================  ======  ==========================


CHANNELS_SET
(CHANNELS_SET，通道数量设置)
============

Sets channel counts like ``ETHTOOL_SCHANNELS`` ioctl request.
(设置通道数量，如同 ``ETHTOOL_SCHANNELS`` ioctl 请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_CHANNELS_HEADER``          nested  request header (请求头)
  ``ETHTOOL_A_CHANNELS_RX_COUNT``        u32     receive channel count (接收通道数量)
  ``ETHTOOL_A_CHANNELS_TX_COUNT``        u32     transmit channel count (发送通道数量)
  ``ETHTOOL_A_CHANNELS_OTHER_COUNT``     u32     other channel count (其他通道数量)
  ``ETHTOOL_A_CHANNELS_COMBINED_COUNT``  u32     combined channel count (合并通道数量)
  =====================================  ======  ==========================

Kernel checks that requested channel counts do not exceed limits reported by
driver. Driver may impose additional constraints and may not support all
attributes.
(内核检查请求的通道数量是否超过驱动报告的限制。驱动可以施加额外约束，也可能
不支持所有属性。)


COALESCE_GET
(COALESCE_GET，合并参数获取)
============

Gets coalescing parameters like ``ETHTOOL_GCOALESCE`` ioctl request.
(获取合并参数，如同 ``ETHTOOL_GCOALESCE`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_COALESCE_HEADER``         nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ===========================================  ======  =======================
  ``ETHTOOL_A_COALESCE_HEADER``                nested  reply header (应答头)
  ``ETHTOOL_A_COALESCE_RX_USECS``              u32     delay (us), normal Rx (延时(us)，普通 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES``         u32     max packets, normal Rx (最大数据包数，普通 Rx)
  ``ETHTOOL_A_COALESCE_RX_USECS_IRQ``          u32     delay (us), Rx in IRQ (延时(us)，IRQ 中的 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES_IRQ``     u32     max packets, Rx in IRQ (最大数据包数，IRQ 中的 Rx)
  ``ETHTOOL_A_COALESCE_TX_USECS``              u32     delay (us), normal Tx (延时(us)，普通 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES``         u32     max packets, normal Tx (最大数据包数，普通 Tx)
  ``ETHTOOL_A_COALESCE_TX_USECS_IRQ``          u32     delay (us), Tx in IRQ (延时(us)，IRQ 中的 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES_IRQ``     u32     IRQ packets, Tx in IRQ (IRQ 数据包数，IRQ 中的 Tx)
  ``ETHTOOL_A_COALESCE_STATS_BLOCK_USECS``     u32     delay of stats update (统计信息更新延时)
  ``ETHTOOL_A_COALESCE_USE_ADAPTIVE_RX``       bool    adaptive Rx coalesce (自适应 Rx 合并)
  ``ETHTOOL_A_COALESCE_USE_ADAPTIVE_TX``       bool    adaptive Tx coalesce (自适应 Tx 合并)
  ``ETHTOOL_A_COALESCE_PKT_RATE_LOW``          u32     threshold for low rate (低速率阈值)
  ``ETHTOOL_A_COALESCE_RX_USECS_LOW``          u32     delay (us), low Rx (延时(us)，低速率 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES_LOW``     u32     max packets, low Rx (最大数据包数，低速率 Rx)
  ``ETHTOOL_A_COALESCE_TX_USECS_LOW``          u32     delay (us), low Tx (延时(us)，低速率 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES_LOW``     u32     max packets, low Tx (最大数据包数，低速率 Tx)
  ``ETHTOOL_A_COALESCE_PKT_RATE_HIGH``         u32     threshold for high rate (高速率阈值)
  ``ETHTOOL_A_COALESCE_RX_USECS_HIGH``         u32     delay (us), high Rx (延时(us)，高速率 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES_HIGH``    u32     max packets, high Rx (最大数据包数，高速率 Rx)
  ``ETHTOOL_A_COALESCE_TX_USECS_HIGH``         u32     delay (us), high Tx (延时(us)，高速率 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES_HIGH``    u32     max packets, high Tx (最大数据包数，高速率 Tx)
  ``ETHTOOL_A_COALESCE_RATE_SAMPLE_INTERVAL``  u32     rate sampling interval (速率采样间隔)
  ``ETHTOOL_A_COALESCE_USE_CQE_TX``            bool    timer reset mode, Tx (定时器复位模式，Tx)
  ``ETHTOOL_A_COALESCE_USE_CQE_RX``            bool    timer reset mode, Rx (定时器复位模式，Rx)
  ``ETHTOOL_A_COALESCE_TX_AGGR_MAX_BYTES``     u32     max aggr size, Tx (最大聚合大小，Tx)
  ``ETHTOOL_A_COALESCE_TX_AGGR_MAX_FRAMES``    u32     max aggr packets, Tx (最大聚合数据包数，Tx)
  ``ETHTOOL_A_COALESCE_TX_AGGR_TIME_USECS``    u32     time (us), aggr, Tx (时间(us)，聚合，Tx)
  ``ETHTOOL_A_COALESCE_RX_PROFILE``            nested  profile of DIM, Rx (DIM 配置，Rx)
  ``ETHTOOL_A_COALESCE_TX_PROFILE``            nested  profile of DIM, Tx (DIM 配置，Tx)
  ``ETHTOOL_A_COALESCE_RX_CQE_FRAMES``         u32     max packets, Rx CQE (最大数据包数，Rx CQE)
  ``ETHTOOL_A_COALESCE_RX_CQE_NSECS``          u32     delay (ns), Rx CQE (延时(ns)，Rx CQE)
  ===========================================  ======  =======================

Attributes are only included in reply if their value is not zero or the
corresponding bit in ``ethtool_ops::supported_coalesce_params`` is set (i.e.
they are declared as supported by driver).
(只有当属性值非零，或 ``ethtool_ops::supported_coalesce_params`` 中的对应位已
置位(即驱动声明支持)时，该属性才会包含在应答中。)

Timer reset mode (``ETHTOOL_A_COALESCE_USE_CQE_TX`` and
``ETHTOOL_A_COALESCE_USE_CQE_RX``) controls the interaction between packet
arrival and the various time based delay parameters. By default timers are
expected to limit the max delay between any packet arrival/departure and a
corresponding interrupt. In this mode timer should be started by packet
arrival (sometimes delivery of previous interrupt) and reset when interrupt
is delivered.
Setting the appropriate attribute to 1 will enable ``CQE`` mode, where
each packet event resets the timer. In this mode timer is used to force
the interrupt if queue goes idle, while busy queues depend on the packet
limit to trigger interrupts.
(定时器复位模式(``ETHTOOL_A_COALESCE_USE_CQE_TX`` 和
``ETHTOOL_A_COALESCE_USE_CQE_RX``)控制数据包到达与各基于时间的延时参数之间的
相互作用。默认情况下，定时器应限制任意数据包到达/离开与相应中断之间的最大
延时。在该模式下，定时器应由数据包到达启动(有时是上一中断的送达)，并在中断
送达时复位。
把相应属性设为 1 将启用 ``CQE`` 模式：每个数据包事件都会复位定时器。在该模式
下，定时器用于在队列空闲时强制发出中断，而繁忙的队列则依靠数据包数上限来触发
中断。)

Tx aggregation consists of copying frames into a contiguous buffer so that they
can be submitted as a single IO operation. ``ETHTOOL_A_COALESCE_TX_AGGR_MAX_BYTES``
describes the maximum size in bytes for the submitted buffer.
``ETHTOOL_A_COALESCE_TX_AGGR_MAX_FRAMES`` describes the maximum number of frames
that can be aggregated into a single buffer.
``ETHTOOL_A_COALESCE_TX_AGGR_TIME_USECS`` describes the amount of time in usecs,
counted since the first packet arrival in an aggregated block, after which the
block should be sent.
This feature is mainly of interest for specific USB devices which does not cope
well with frequent small-sized URBs transmissions.
(Tx 聚合是指把帧复制到一块连续缓冲区中，使它们能作为单次 IO 操作提交。
``ETHTOOL_A_COALESCE_TX_AGGR_MAX_BYTES`` 描述所提交缓冲区的最大字节数。
``ETHTOOL_A_COALESCE_TX_AGGR_MAX_FRAMES`` 描述可聚合进单个缓冲区的最大帧数。
``ETHTOOL_A_COALESCE_TX_AGGR_TIME_USECS`` 描述以微秒计的时间量，从聚合块中第一个
数据包到达起计算，超过该时间后应发送该块。
该特性主要针对那些难以应对频繁的小尺寸 URB 传输的特定 USB 设备。)

``ETHTOOL_A_COALESCE_RX_PROFILE`` and ``ETHTOOL_A_COALESCE_TX_PROFILE`` refer
to DIM parameters, see `Generic Network Dynamic Interrupt Moderation (Net DIM)
<https://www.kernel.org/doc/Documentation/networking/net_dim.rst>`_.
(``ETHTOOL_A_COALESCE_RX_PROFILE`` 和 ``ETHTOOL_A_COALESCE_TX_PROFILE`` 指 DIM
参数，参见"通用网络动态中断调优(Net DIM)"文档(见上方链接)。)

Rx CQE coalescing allows multiple received packets to be coalesced into a
single Completion Queue Entry (CQE) or descriptor writeback.
``ETHTOOL_A_COALESCE_RX_CQE_FRAMES`` describes the maximum number of
frames that can be coalesced into a CQE or writeback.
``ETHTOOL_A_COALESCE_RX_CQE_NSECS`` describes max time in nanoseconds after
the first packet arrival in a coalesced CQE or writeback to be sent.
(Rx CQE 合并允许把多个收到的数据包合并进单个完成队列条目(CQE)或描述符回写。
``ETHTOOL_A_COALESCE_RX_CQE_FRAMES`` 描述可合并进一个 CQE 或回写的最大帧数。
``ETHTOOL_A_COALESCE_RX_CQE_NSECS`` 描述从合并 CQE 或回写中第一个数据包到达起、
到其被发送为止的最大纳秒时间。)

COALESCE_SET
(COALESCE_SET，合并参数设置)
============

Sets coalescing parameters like ``ETHTOOL_SCOALESCE`` ioctl request.
(设置合并参数，如同 ``ETHTOOL_SCOALESCE`` ioctl 请求。)

Request contents:
(请求内容：)

  ===========================================  ======  =======================
  ``ETHTOOL_A_COALESCE_HEADER``                nested  request header (请求头)
  ``ETHTOOL_A_COALESCE_RX_USECS``              u32     delay (us), normal Rx (延时(us)，普通 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES``         u32     max packets, normal Rx (最大数据包数，普通 Rx)
  ``ETHTOOL_A_COALESCE_RX_USECS_IRQ``          u32     delay (us), Rx in IRQ (延时(us)，IRQ 中的 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES_IRQ``     u32     max packets, Rx in IRQ (最大数据包数，IRQ 中的 Rx)
  ``ETHTOOL_A_COALESCE_TX_USECS``              u32     delay (us), normal Tx (延时(us)，普通 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES``         u32     max packets, normal Tx (最大数据包数，普通 Tx)
  ``ETHTOOL_A_COALESCE_TX_USECS_IRQ``          u32     delay (us), Tx in IRQ (延时(us)，IRQ 中的 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES_IRQ``     u32     IRQ packets, Tx in IRQ (IRQ 数据包数，IRQ 中的 Tx)
  ``ETHTOOL_A_COALESCE_STATS_BLOCK_USECS``     u32     delay of stats update (统计信息更新延时)
  ``ETHTOOL_A_COALESCE_USE_ADAPTIVE_RX``       bool    adaptive Rx coalesce (自适应 Rx 合并)
  ``ETHTOOL_A_COALESCE_USE_ADAPTIVE_TX``       bool    adaptive Tx coalesce (自适应 Tx 合并)
  ``ETHTOOL_A_COALESCE_PKT_RATE_LOW``          u32     threshold for low rate (低速率阈值)
  ``ETHTOOL_A_COALESCE_RX_USECS_LOW``          u32     delay (us), low Rx (延时(us)，低速率 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES_LOW``     u32     max packets, low Rx (最大数据包数，低速率 Rx)
  ``ETHTOOL_A_COALESCE_TX_USECS_LOW``          u32     delay (us), low Tx (延时(us)，低速率 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES_LOW``     u32     max packets, low Tx (最大数据包数，低速率 Tx)
  ``ETHTOOL_A_COALESCE_PKT_RATE_HIGH``         u32     threshold for high rate (高速率阈值)
  ``ETHTOOL_A_COALESCE_RX_USECS_HIGH``         u32     delay (us), high Rx (延时(us)，高速率 Rx)
  ``ETHTOOL_A_COALESCE_RX_MAX_FRAMES_HIGH``    u32     max packets, high Rx (最大数据包数，高速率 Rx)
  ``ETHTOOL_A_COALESCE_TX_USECS_HIGH``         u32     delay (us), high Tx (延时(us)，高速率 Tx)
  ``ETHTOOL_A_COALESCE_TX_MAX_FRAMES_HIGH``    u32     max packets, high Tx (最大数据包数，高速率 Tx)
  ``ETHTOOL_A_COALESCE_RATE_SAMPLE_INTERVAL``  u32     rate sampling interval (速率采样间隔)
  ``ETHTOOL_A_COALESCE_USE_CQE_TX``            bool    timer reset mode, Tx (定时器复位模式，Tx)
  ``ETHTOOL_A_COALESCE_USE_CQE_RX``            bool    timer reset mode, Rx (定时器复位模式，Rx)
  ``ETHTOOL_A_COALESCE_TX_AGGR_MAX_BYTES``     u32     max aggr size, Tx (最大聚合大小，Tx)
  ``ETHTOOL_A_COALESCE_TX_AGGR_MAX_FRAMES``    u32     max aggr packets, Tx (最大聚合数据包数，Tx)
  ``ETHTOOL_A_COALESCE_TX_AGGR_TIME_USECS``    u32     time (us), aggr, Tx (时间(us)，聚合，Tx)
  ``ETHTOOL_A_COALESCE_RX_PROFILE``            nested  profile of DIM, Rx (DIM 配置，Rx)
  ``ETHTOOL_A_COALESCE_TX_PROFILE``            nested  profile of DIM, Tx (DIM 配置，Tx)
  ``ETHTOOL_A_COALESCE_RX_CQE_FRAMES``         u32     max packets, Rx CQE (最大数据包数，Rx CQE)
  ``ETHTOOL_A_COALESCE_RX_CQE_NSECS``          u32     delay (ns), Rx CQE (延时(ns)，Rx CQE)
  ===========================================  ======  =======================

Request is rejected if it attributes declared as unsupported by driver (i.e.
such that the corresponding bit in ``ethtool_ops::supported_coalesce_params``
is not set), regardless of their values. Driver may impose additional
constraints on coalescing parameters and their values.
(若请求包含驱动声明不支持的属性(即 ``ethtool_ops::supported_coalesce_params``
中的对应位未置位)，则无论其值如何都会被拒绝。驱动可以对合并参数及其取值施加
额外约束。)

Compared to requests issued via the ``ioctl()`` netlink version of this request
will try harder to make sure that values specified by the user have been applied
and may call the driver twice.
(与通过 ``ioctl()`` 发出的请求相比，netlink 版本的该请求会更尽力确保用户指定的
值得到应用，并可能调用驱动两次。)


PAUSE_GET
(PAUSE_GET，暂停参数获取)
=========

Gets pause frame settings like ``ETHTOOL_GPAUSEPARAM`` ioctl request.
(获取暂停帧设置，如同 ``ETHTOOL_GPAUSEPARAM`` ioctl 请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_PAUSE_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_PAUSE_STATS_SRC``          u32     source of statistics (统计信息来源)
  =====================================  ======  ==========================

``ETHTOOL_A_PAUSE_STATS_SRC`` is optional. It takes values from:
(``ETHTOOL_A_PAUSE_STATS_SRC`` 是可选的。它取以下值：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_mac_stats_src

If absent from the request, stats will be provided with
an ``ETHTOOL_A_PAUSE_STATS_SRC`` attribute in the response equal to
``ETHTOOL_MAC_STATS_SRC_AGGREGATE``.
(若请求中没有该属性，响应中的 ``ETHTOOL_A_PAUSE_STATS_SRC`` 属性将等于
``ETHTOOL_MAC_STATS_SRC_AGGREGATE``，并据此提供统计信息。)

Kernel response contents:
(内核响应内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_PAUSE_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_PAUSE_AUTONEG``            bool    pause autonegotiation (暂停帧自动协商)
  ``ETHTOOL_A_PAUSE_RX``                 bool    receive pause frames (接收暂停帧)
  ``ETHTOOL_A_PAUSE_TX``                 bool    transmit pause frames (发送暂停帧)
  ``ETHTOOL_A_PAUSE_STATS``              nested  pause statistics (暂停统计信息)
  =====================================  ======  ==========================

``ETHTOOL_A_PAUSE_STATS`` are reported if ``ETHTOOL_FLAG_STATS`` was set
in ``ETHTOOL_A_HEADER_FLAGS``.
It will be empty if driver did not report any statistics. Drivers fill in
the statistics in the following structure:
(若在 ``ETHTOOL_A_HEADER_FLAGS`` 中设置了 ``ETHTOOL_FLAG_STATS``，则报告
``ETHTOOL_A_PAUSE_STATS``。若驱动未报告任何统计信息，它将为空。驱动在以下结构
中填写统计信息：)

.. kernel-doc:: include/linux/ethtool.h
    :identifiers: ethtool_pause_stats

Each member has a corresponding attribute defined.
(每个成员都定义了对应的属性。)

PAUSE_SET
(PAUSE_SET，暂停参数设置)
=========

Sets pause parameters like ``ETHTOOL_GPAUSEPARAM`` ioctl request.
(设置暂停参数，如同 ``ETHTOOL_GPAUSEPARAM`` ioctl 请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_PAUSE_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_PAUSE_AUTONEG``            bool    pause autonegotiation (暂停帧自动协商)
  ``ETHTOOL_A_PAUSE_RX``                 bool    receive pause frames (接收暂停帧)
  ``ETHTOOL_A_PAUSE_TX``                 bool    transmit pause frames (发送暂停帧)
  =====================================  ======  ==========================


EEE_GET
(EEE_GET，EEE 设置获取)
=======

Gets Energy Efficient Ethernet settings like ``ETHTOOL_GEEE`` ioctl request.
(获取高能效以太网(Energy Efficient Ethernet)设置，如同 ``ETHTOOL_GEEE`` ioctl
请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_EEE_HEADER``               nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_EEE_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_EEE_MODES_OURS``           bool    supported/advertised modes (支持/通告的模式)
  ``ETHTOOL_A_EEE_MODES_PEER``           bool    peer advertised link modes (对端通告的链路模式)
  ``ETHTOOL_A_EEE_ACTIVE``               bool    EEE is actively used (EEE 正在被使用)
  ``ETHTOOL_A_EEE_ENABLED``              bool    EEE is enabled (EEE 已启用)
  ``ETHTOOL_A_EEE_TX_LPI_ENABLED``       bool    Tx lpi enabled (Tx lpi 已启用)
  ``ETHTOOL_A_EEE_TX_LPI_TIMER``         u32     Tx lpi timeout (in us) (Tx lpi 超时(单位 us))
  =====================================  ======  ==========================

In ``ETHTOOL_A_EEE_MODES_OURS``, mask consists of link modes for which EEE is
enabled, value of link modes for which EEE is advertised. Link modes for which
peer advertises EEE are listed in ``ETHTOOL_A_EEE_MODES_PEER`` (no mask). The
netlink interface allows reporting EEE status for all link modes but only
first 32 are provided by the ``ethtool_ops`` callback.
(在 ``ETHTOOL_A_EEE_MODES_OURS`` 中，掩码由已为其启用 EEE 的链路模式组成，值
为已通告 EEE 的链路模式。对端通告 EEE 的链路模式列在 ``ETHTOOL_A_EEE_MODES_PEER``
中(无掩码)。netlink 接口允许报告所有链路模式的 EEE 状态，但 ``ethtool_ops``
回调只提供前 32 个。)


EEE_SET
(EEE_SET，设置 EEE 设置)
=======

Sets Energy Efficient Ethernet parameters like ``ETHTOOL_SEEE`` ioctl request.
(设置高能效以太网参数，如同 ``ETHTOOL_SEEE`` ioctl 请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_EEE_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_EEE_MODES_OURS``           bool    advertised modes (通告的模式)
  ``ETHTOOL_A_EEE_ENABLED``              bool    EEE is enabled (EEE 已启用)
  ``ETHTOOL_A_EEE_TX_LPI_ENABLED``       bool    Tx lpi enabled (Tx lpi 已启用)
  ``ETHTOOL_A_EEE_TX_LPI_TIMER``         u32     Tx lpi timeout (in us) (Tx lpi 超时(单位 us))
  =====================================  ======  ==========================

``ETHTOOL_A_EEE_MODES_OURS`` is used to either list link modes to advertise
EEE for (if there is no mask) or specify changes to the list (if there is
a mask). The netlink interface allows reporting EEE status for all link modes
but only first 32 can be set at the moment as that is what the ``ethtool_ops``
callback supports.
(``ETHTOOL_A_EEE_MODES_OURS`` 用于列出要为其通告 EEE 的链路模式(若没有掩码)，
或指定对该列表的更改(若有掩码)。netlink 接口允许报告所有链路模式的 EEE 状态，
但目前只能设置前 32 个，因为 ``ethtool_ops`` 回调仅支持这么多。)


TSINFO_GET
(TSINFO_GET，时间戳信息获取)
==========

Gets timestamping information like ``ETHTOOL_GET_TS_INFO`` ioctl request.
(获取时间戳信息，如同 ``ETHTOOL_GET_TS_INFO`` ioctl 请求。)

Request contents:
(请求内容：)

  ========================================  ======  ============================
  ``ETHTOOL_A_TSINFO_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_TSINFO_HWTSTAMP_PROVIDER``    nested  PTP hw clock provider (PTP 硬件时钟提供者)
  ========================================  ======  ============================

Kernel response contents:
(内核响应内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_TSINFO_HEADER``            nested  request header (请求头)
  ``ETHTOOL_A_TSINFO_TIMESTAMPING``      bitset  SO_TIMESTAMPING flags (SO_TIMESTAMPING 标志)
  ``ETHTOOL_A_TSINFO_TX_TYPES``          bitset  supported Tx types (支持的 Tx 类型)
  ``ETHTOOL_A_TSINFO_RX_FILTERS``        bitset  supported Rx filters (支持的 Rx 过滤器)
  ``ETHTOOL_A_TSINFO_PHC_INDEX``         u32     PTP hw clock index (PTP 硬件时钟索引)
  ``ETHTOOL_A_TSINFO_STATS``             nested  HW timestamping statistics (硬件时间戳统计信息)
  =====================================  ======  ==========================

``ETHTOOL_A_TSINFO_PHC_INDEX`` is absent if there is no associated PHC (there
is no special value for this case). The bitset attributes are omitted if they
would be empty (no bit set).
(若没有关联的 PHC，则不存在 ``ETHTOOL_A_TSINFO_PHC_INDEX`` (这种情况没有特殊
值)。若位集属性为空(没有置位的位)，则会被省略。)

Additional hardware timestamping statistics response contents:
(附加的硬件时间戳统计信息响应内容：)

  ==================================================  ======  =====================
  ``ETHTOOL_A_TS_STAT_TX_PKTS``                       uint    Packets with Tx
                                                              HW timestamps (带 Tx 硬件时间戳的数据包数)
  ``ETHTOOL_A_TS_STAT_TX_LOST``                       uint    Tx HW timestamp
                                                              not arrived count (未到达的 Tx 硬件时间戳计数)
  ``ETHTOOL_A_TS_STAT_TX_ERR``                        uint    HW error request
                                                              Tx timestamp count (硬件错误请求 Tx 时间戳计数)
  ``ETHTOOL_A_TS_STAT_TX_ONESTEP_PKTS_UNCONFIRMED``   uint    Packets with one-step
                                                              HW TX timestamps with
                                                              unconfirmed delivery (带一步式硬件 Tx 时间戳且送达未确认的数据包数)
  ==================================================  ======  =====================

CABLE_TEST
(CABLE_TEST，线缆测试)
==========

Start a cable test.
(启动线缆测试。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_CABLE_TEST_HEADER``       nested  request header (请求头)
  ====================================  ======  ==========================

Notification contents:
(通知内容：)

An Ethernet cable typically contains 1, 2 or 4 pairs. The length of
the pair can only be measured when there is a fault in the pair and
hence a reflection. Information about the fault may not be available,
depending on the specific hardware. Hence the contents of the notify
message are mostly optional. The attributes can be repeated an
arbitrary number of times, in an arbitrary order, for an arbitrary
number of pairs.
(以太网线缆通常包含 1、2 或 4 个线对。只有当线对中存在故障并因此产生反射时，
才能测量线对长度。故障信息可能不可用，取决于具体硬件。因此通知消息的内容大多
是可选的。这些属性可以按任意顺序、对任意数量的线对重复任意次数。)

The example shows the notification sent when the test is completed for
a T2 cable, i.e. two pairs. One pair is OK and hence has no length
information. The second pair has a fault and does have length
information.
(该示例展示了 T2 线缆(即两个线对)测试完成时发送的通知。第一个线对正常，因此
没有长度信息；第二个线对有故障，确实带有长度信息。)

 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_CABLE_TEST_HEADER``             | nested | reply header        |
 |                                             |        | (应答头)            |
 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_CABLE_TEST_STATUS``             | u8     | completed (已完成)  |
 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_CABLE_TEST_NTF_NEST``           | nested | all the results     |
 |                                             |        | (全部结果)          |
 +-+-------------------------------------------+--------+---------------------+
 | | ``ETHTOOL_A_CABLE_NEST_RESULT``           | nested | cable test result   |
 | |                                           |        | (线缆测试结果)      |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_PAIR``        | u8     | pair number         |
 | | |                                         |        | (线对编号)          |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_CODE``        | u8     | result code (结果码)|
 +-+-+-----------------------------------------+--------+---------------------+
 | | ``ETHTOOL_A_CABLE_NEST_RESULT``           | nested | cable test results  |
 | |                                           |        | (线缆测试结果)      |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_PAIR``        | u8     | pair number         |
 | | |                                         |        | (线对编号)          |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_CODE``        | u8     | result code (结果码)|
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_RESULT_SRC``          | u32    | information source  |
 | | |                                         |        | (信息来源)          |
 +-+-+-----------------------------------------+--------+---------------------+
 | | ``ETHTOOL_A_CABLE_NEST_FAULT_LENGTH``     | nested | cable length        |
 | |                                           |        | (线缆长度)          |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_FAULT_LENGTH_PAIR``   | u8     | pair number         |
 | | |                                         |        | (线对编号)          |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_FAULT_LENGTH_CM``     | u32    | length in cm        |
 | | |                                         |        | (长度，cm)          |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_CABLE_FAULT_LENGTH_SRC``    | u32    | information source  |
 | | |                                         |        | (信息来源)          |
 +-+-+-----------------------------------------+--------+---------------------+


CABLE_TEST TDR
(CABLE_TEST TDR，线缆测试 TDR)
==============

Start a cable test and report raw TDR data
(启动线缆测试并报告原始 TDR 数据)

Request contents:
(请求内容：)

 +--------------------------------------------+--------+-----------------------+
 | ``ETHTOOL_A_CABLE_TEST_TDR_HEADER``        | nested | reply header (应答头) |
 +--------------------------------------------+--------+-----------------------+
 | ``ETHTOOL_A_CABLE_TEST_TDR_CFG``           | nested | test configuration    |
 |                                            |        | (测试配置)            |
 +-+------------------------------------------+--------+-----------------------+
 | | ``ETHTOOL_A_CABLE_STEP_FIRST_DISTANCE``  | u32    | first data distance   |
 | |                                          |        | (首个数据距离)        |
 +-+-+----------------------------------------+--------+-----------------------+
 | | ``ETHTOOL_A_CABLE_STEP_LAST_DISTANCE``   | u32    | last data distance    |
 | |                                          |        | (最后数据距离)        |
 +-+-+----------------------------------------+--------+-----------------------+
 | | ``ETHTOOL_A_CABLE_STEP_STEP_DISTANCE``   | u32    | distance of each step |
 | |                                          |        | (每步距离)            |
 +-+-+----------------------------------------+--------+-----------------------+
 | | ``ETHTOOL_A_CABLE_TEST_TDR_CFG_PAIR``    | u8     | pair to test          |
 | |                                          |        | (要测试的线对)        |
 +-+-+----------------------------------------+--------+-----------------------+

The ETHTOOL_A_CABLE_TEST_TDR_CFG is optional, as well as all members
of the nest. All distances are expressed in centimeters. The PHY takes
the distances as a guide, and rounds to the nearest distance it
actually supports. If a pair is passed, only that one pair will be
tested. Otherwise all pairs are tested.
(ETHTOOL_A_CABLE_TEST_TDR_CFG 及该嵌套中的所有成员都是可选的。所有距离都以
厘米表示。PHY 把这些距离当作参考值，并舍入到它实际支持的最近距离。若传入
线对，则只测试该线对；否则测试所有线对。)

Notification contents:
(通知内容：)

Raw TDR data is gathered by sending a pulse down the cable and
recording the amplitude of the reflected pulse for a given distance.
(原始 TDR 数据的采集方式是：沿线缆发送一个脉冲，并记录给定距离处反射脉冲的
幅度。)

It can take a number of seconds to collect TDR data, especial if the
full 100 meters is probed at 1 meter intervals. When the test is
started a notification will be sent containing just
ETHTOOL_A_CABLE_TEST_TDR_STATUS with the value
ETHTOOL_A_CABLE_TEST_NTF_STATUS_STARTED.
(采集 TDR 数据可能需要数秒，尤其是以 1 米间隔探测整个 100 米时。测试启动时会
发送一条仅包含 ETHTOOL_A_CABLE_TEST_TDR_STATUS 的通知，其值为
ETHTOOL_A_CABLE_TEST_NTF_STATUS_STARTED。)

When the test has completed a second notification will be sent
containing ETHTOOL_A_CABLE_TEST_TDR_STATUS with the value
ETHTOOL_A_CABLE_TEST_NTF_STATUS_COMPLETED and the TDR data.
(测试完成后会发送第二条通知，包含值为
ETHTOOL_A_CABLE_TEST_NTF_STATUS_COMPLETED 的 ETHTOOL_A_CABLE_TEST_TDR_STATUS
以及 TDR 数据。)

The message may optionally contain the amplitude of the pulse send
down the cable. This is measured in mV. A reflection should not be
bigger than transmitted pulse.
(该消息可以选择性地包含沿线缆发送的脉冲幅度，以 mV 为单位。反射不应大于发送
的脉冲。)

Before the raw TDR data should be an ETHTOOL_A_CABLE_TDR_NEST_STEP
nest containing information about the distance along the cable for the
first reading, the last reading, and the step between each
reading. Distances are measured in centimeters. These should be the
exact values the PHY used. These may be different to what the user
requested, if the native measurement resolution is greater than 1 cm.
(在原始 TDR 数据之前应有一个 ETHTOOL_A_CABLE_TDR_NEST_STEP 嵌套，包含首次
读数、最后一次读数以及各读数之间步进沿线缆的距离信息。距离以厘米为单位。这些
应是 PHY 使用的精确值。如果原生测量分辨率大于 1 cm，这些值可能与用户请求的
不同。)

For each step along the cable, a ETHTOOL_A_CABLE_TDR_NEST_AMPLITUDE is
used to report the amplitude of the reflection for a given pair.
(对于沿线缆的每个步进，使用一个 ETHTOOL_A_CABLE_TDR_NEST_AMPLITUDE 报告给定
线对的反射幅度。)

 +---------------------------------------------+--------+----------------------+
 | ``ETHTOOL_A_CABLE_TEST_TDR_HEADER``         | nested | reply header (应答头)|
 +---------------------------------------------+--------+----------------------+
 | ``ETHTOOL_A_CABLE_TEST_TDR_STATUS``         | u8     | completed (已完成)   |
 +---------------------------------------------+--------+----------------------+
 | ``ETHTOOL_A_CABLE_TEST_TDR_NTF_NEST``       | nested | all the results      |
 |                                             |        | (全部结果)           |
 +-+-------------------------------------------+--------+----------------------+
 | | ``ETHTOOL_A_CABLE_TDR_NEST_PULSE``        | nested | TX Pulse amplitude   |
 | |                                           |        | (发送脉冲幅度)       |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_PULSE_mV``            | s16    | Pulse amplitude      |
 | | |                                         |        | (脉冲幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | ``ETHTOOL_A_CABLE_NEST_STEP``             | nested | TDR step info        |
 | |                                           |        | (TDR 步进信息)       |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_STEP_FIRST_DISTANCE`` | u32    | First data distance  |
 | | |                                         |        | (首个数据距离)       |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_STEP_LAST_DISTANCE``  | u32    | Last data distance   |
 | | |                                         |        | (最后数据距离)       |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_STEP_STEP_DISTANCE``  | u32    | distance of each step|
 | | |                                         |        | (每步距离)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | ``ETHTOOL_A_CABLE_TDR_NEST_AMPLITUDE``    | nested | Reflection amplitude |
 | |                                           |        | (反射幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_PAIR``        | u8     | pair number          |
 | | |                                         |        | (线对编号)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_AMPLITUDE_mV``        | s16    | Reflection amplitude |
 | | |                                         |        | (反射幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | ``ETHTOOL_A_CABLE_TDR_NEST_AMPLITUDE``    | nested | Reflection amplitude |
 | |                                           |        | (反射幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_PAIR``        | u8     | pair number          |
 | | |                                         |        | (线对编号)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_AMPLITUDE_mV``        | s16    | Reflection amplitude |
 | | |                                         |        | (反射幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | ``ETHTOOL_A_CABLE_TDR_NEST_AMPLITUDE``    | nested | Reflection amplitude |
 | |                                           |        | (反射幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_RESULTS_PAIR``        | u8     | pair number          |
 | | |                                         |        | (线对编号)           |
 +-+-+-----------------------------------------+--------+----------------------+
 | | | ``ETHTOOL_A_CABLE_AMPLITUDE_mV``        | s16    | Reflection amplitude |
 | | |                                         |        | (反射幅度)           |
 +-+-+-----------------------------------------+--------+----------------------+

TUNNEL_INFO
(TUNNEL_INFO，隧道信息)
===========

Gets information about the tunnel state NIC is aware of.
(获取 NIC 已知的隧道状态信息。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_TUNNEL_INFO_HEADER``       nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_TUNNEL_INFO_HEADER``            | nested | reply header        |
 |                                             |        | (应答头)            |
 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_TUNNEL_INFO_UDP_PORTS``         | nested | all UDP port tables |
 |                                             |        | (所有 UDP 端口表)   |
 +-+-------------------------------------------+--------+---------------------+
 | | ``ETHTOOL_A_TUNNEL_UDP_TABLE``            | nested | one UDP port table  |
 | |                                           |        | (一个 UDP 端口表)   |
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_TUNNEL_UDP_TABLE_SIZE``     | u32    | max size of the     |
 | | |                                         |        | table (表的最大尺寸)|
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_TUNNEL_UDP_TABLE_TYPES``    | bitset | tunnel types which  |
 | | |                                         |        | table can hold      |
 | | |                                         |        | (表可容纳的隧道类型)|
 +-+-+-----------------------------------------+--------+---------------------+
 | | | ``ETHTOOL_A_TUNNEL_UDP_TABLE_ENTRY``    | nested | offloaded UDP port  |
 | | |                                         |        | (已卸载的 UDP 端口) |
 +-+-+-+---------------------------------------+--------+---------------------+
 | | | | ``ETHTOOL_A_TUNNEL_UDP_ENTRY_PORT``   | be16   | UDP port (UDP 端口) |
 +-+-+-+---------------------------------------+--------+---------------------+
 | | | | ``ETHTOOL_A_TUNNEL_UDP_ENTRY_TYPE``   | u32    | tunnel type         |
 | | | |                                       |        | (隧道类型)          |
 +-+-+-+---------------------------------------+--------+---------------------+

For UDP tunnel table empty ``ETHTOOL_A_TUNNEL_UDP_TABLE_TYPES`` indicates that
the table contains static entries, hard-coded by the NIC.
(对于 UDP 隧道表，空的 ``ETHTOOL_A_TUNNEL_UDP_TABLE_TYPES`` 表示该表包含由
NIC 硬编码的静态条目。)

FEC_GET
(FEC_GET，FEC 设置获取)
=======

Gets FEC configuration and state like ``ETHTOOL_GFECPARAM`` ioctl request.
(获取 FEC 配置和状态，如同 ``ETHTOOL_GFECPARAM`` ioctl 请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_FEC_HEADER``               nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_FEC_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_FEC_MODES``                bitset  configured modes (已配置的模式)
  ``ETHTOOL_A_FEC_AUTO``                 bool    FEC mode auto selection (FEC 模式自动选择)
  ``ETHTOOL_A_FEC_ACTIVE``               u32     index of active FEC mode (当前生效 FEC 模式的索引)
  ``ETHTOOL_A_FEC_STATS``                nested  FEC statistics (FEC 统计信息)
  =====================================  ======  ==========================

``ETHTOOL_A_FEC_ACTIVE`` is the bit index of the FEC link mode currently
active on the interface. This attribute may not be present if device does
not support FEC.
(``ETHTOOL_A_FEC_ACTIVE`` 是接口上当前生效的 FEC 链路模式的位索引。若设备不
支持 FEC，该属性可能不存在。)

``ETHTOOL_A_FEC_MODES`` and ``ETHTOOL_A_FEC_AUTO`` are only meaningful when
autonegotiation is disabled. If ``ETHTOOL_A_FEC_AUTO`` is non-zero driver will
select the FEC mode automatically based on the parameters of the SFP module.
This is equivalent to the ``ETHTOOL_FEC_AUTO`` bit of the ioctl interface.
``ETHTOOL_A_FEC_MODES`` carry the current FEC configuration using link mode
bits (rather than old ``ETHTOOL_FEC_*`` bits).
(``ETHTOOL_A_FEC_MODES`` 和 ``ETHTOOL_A_FEC_AUTO`` 仅在禁用自动协商时才有
意义。若 ``ETHTOOL_A_FEC_AUTO`` 非零，驱动会根据 SFP 模块的参数自动选择 FEC
模式。这等价于 ioctl 接口的 ``ETHTOOL_FEC_AUTO`` 位。``ETHTOOL_A_FEC_MODES``
使用链路模式位(而不是旧的 ``ETHTOOL_FEC_*`` 位)承载当前 FEC 配置。)

``ETHTOOL_A_FEC_STATS`` are reported if ``ETHTOOL_FLAG_STATS`` was set in
``ETHTOOL_A_HEADER_FLAGS``.
Each attribute carries an array of 64bit statistics. First entry in the array
contains the total number of events on the port, while the following entries
are counters corresponding to lanes/PCS instances. The number of entries in
the array will be:
(若在 ``ETHTOOL_A_HEADER_FLAGS`` 中设置了 ``ETHTOOL_FLAG_STATS``，则报告
``ETHTOOL_A_FEC_STATS``。每个属性承载一个 64 位统计数组。数组第一项包含端口上
事件的总数，后续各项是与 lane/PCS 实例对应的计数器。数组项数为：)

+--------------+---------------------------------------------+
| `0`          | device does not support FEC statistics      |
|              | (设备不支持 FEC 统计)                       |
+--------------+---------------------------------------------+
| `1`          | device does not support per-lane break down |
|              | (设备不支持按线对的细分)                    |
+--------------+---------------------------------------------+
| `1 + #lanes` | device has full support for FEC stats       |
|              | (设备完全支持 FEC 统计)                     |
+--------------+---------------------------------------------+

Drivers fill in the statistics in the following structure:
(驱动在以下结构中填写统计信息：)

.. kernel-doc:: include/linux/ethtool.h
    :identifiers: ethtool_fec_stats

Statistics may have FEC bins histogram attribute ``ETHTOOL_A_FEC_STAT_HIST``
as defined in IEEE 802.3ck-2022 and 802.3df-2024. Nested attributes will have
the range of FEC errors in the bin (inclusive) and the amount of error events
in the bin.
(统计信息可以包含 IEEE 802.3ck-2022 和 802.3df-2024 定义的 FEC 桶直方图属性
``ETHTOOL_A_FEC_STAT_HIST``。嵌套属性将包含桶中 FEC 错误的范围(含边界)以及
桶中错误事件的数量。)

FEC_SET
(FEC_SET，设置 FEC 设置)
=======

Sets FEC parameters like ``ETHTOOL_SFECPARAM`` ioctl request.
(设置 FEC 参数，如同 ``ETHTOOL_SFECPARAM`` ioctl 请求。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_FEC_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_FEC_MODES``                bitset  configured modes (已配置的模式)
  ``ETHTOOL_A_FEC_AUTO``                 bool    FEC mode auto selection (FEC 模式自动选择)
  =====================================  ======  ==========================

``FEC_SET`` is only meaningful when autonegotiation is disabled. Otherwise
FEC mode is selected as part of autonegotiation.
(``FEC_SET`` 仅在禁用自动协商时才有意义。否则，FEC 模式作为自动协商的一部分
来选择。)

``ETHTOOL_A_FEC_MODES`` selects which FEC mode should be used. It's recommended
to set only one bit, if multiple bits are set driver may choose between them
in an implementation specific way.
(``ETHTOOL_A_FEC_MODES`` 选择应使用的 FEC 模式。建议只设置一个位；若设置了
多个位，驱动可能以实现特定的方式在其中选择。)

``ETHTOOL_A_FEC_AUTO`` requests the driver to choose FEC mode based on SFP
module parameters. This does not mean autonegotiation.
(``ETHTOOL_A_FEC_AUTO`` 请求驱动根据 SFP 模块参数选择 FEC 模式。这不代表自动
协商。)

MODULE_EEPROM_GET
(MODULE_EEPROM_GET，模块 EEPROM 读取)
=================

Fetch module EEPROM data dump.
This interface is designed to allow dumps of at most 1/2 page at once. This
means only dumps of 128 (or less) bytes are allowed, without crossing half page
boundary located at offset 128. For pages other than 0 only high 128 bytes are
accessible.
(获取模块 EEPROM 数据转储。该接口设计为一次最多允许转储 1/2 页。这意味着只允许
128(或更少)字节的转储，且不得越过位于偏移 128 处的半页边界。对于 0 以外的页，
只有高 128 字节可访问。)

Request contents:
(请求内容：)

  =======================================  ======  ==========================
  ``ETHTOOL_A_MODULE_EEPROM_HEADER``       nested  request header (请求头)
  ``ETHTOOL_A_MODULE_EEPROM_OFFSET``       u32     offset within a page (页内偏移)
  ``ETHTOOL_A_MODULE_EEPROM_LENGTH``       u32     amount of bytes to read (要读取的字节数)
  ``ETHTOOL_A_MODULE_EEPROM_PAGE``         u8      page number (页号)
  ``ETHTOOL_A_MODULE_EEPROM_BANK``         u8      bank number (bank 号)
  ``ETHTOOL_A_MODULE_EEPROM_I2C_ADDRESS``  u8      page I2C address (页 I2C 地址)
  =======================================  ======  ==========================

If ``ETHTOOL_A_MODULE_EEPROM_BANK`` is not specified, bank 0 is assumed.
(若未指定 ``ETHTOOL_A_MODULE_EEPROM_BANK``，则假定 bank 0。)

Kernel response contents:
(内核响应内容：)

 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_MODULE_EEPROM_HEADER``          | nested | reply header        |
 |                                             |        | (应答头)            |
 +---------------------------------------------+--------+---------------------+
 | ``ETHTOOL_A_MODULE_EEPROM_DATA``            | binary | array of bytes from |
 |                                             |        | module EEPROM       |
 |                                             |        | (EEPROM 字节数组)   |
 +---------------------------------------------+--------+---------------------+

``ETHTOOL_A_MODULE_EEPROM_DATA`` has an attribute length equal to the amount of
bytes driver actually read.
(``ETHTOOL_A_MODULE_EEPROM_DATA`` 的属性长度等于驱动实际读取的字节数。)

STATS_GET
(STATS_GET，统计信息获取)
=========

Get standard statistics for the interface. Note that this is not
a re-implementation of ``ETHTOOL_GSTATS`` which exposed driver-defined
stats.
(获取接口的标准统计信息。注意，这并不是对暴露驱动自定义统计信息的
``ETHTOOL_GSTATS`` 的重新实现。)

Request contents:
(请求内容：)

  =======================================  ======  ==========================
  ``ETHTOOL_A_STATS_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_STATS_SRC``                  u32     source of statistics (统计信息来源)
  ``ETHTOOL_A_STATS_GROUPS``               bitset  requested groups of stats (请求的统计组)
  =======================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

 +-----------------------------------+--------+--------------------------------+
 | ``ETHTOOL_A_STATS_HEADER``        | nested | reply header (应答头)          |
 +-----------------------------------+--------+--------------------------------+
 | ``ETHTOOL_A_STATS_SRC``           | u32    | source of statistics           |
 |                                   |        | (统计信息来源)                 |
 +-----------------------------------+--------+--------------------------------+
 | ``ETHTOOL_A_STATS_GRP``           | nested | one or more group of stats     |
 |                                   |        | (一个或多个统计组)             |
 +-+---------------------------------+--------+--------------------------------+
 | | ``ETHTOOL_A_STATS_GRP_ID``      | u32    | group ID - ``ETHTOOL_STATS_*`` |
 | |                                 |        | (统计组 ID，即 ETHTOOL_STATS_*)|
 +-+---------------------------------+--------+--------------------------------+
 | | ``ETHTOOL_A_STATS_GRP_SS_ID``   | u32    | string set ID for names        |
 | |                                 |        | (名称字符串集 ID)              |
 +-+---------------------------------+--------+--------------------------------+
 | | ``ETHTOOL_A_STATS_GRP_STAT``    | nested | nest containing a statistic    |
 | |                                 |        | (包含一项统计的嵌套属性)       |
 +-+---------------------------------+--------+--------------------------------+
 | | ``ETHTOOL_A_STATS_GRP_HIST_RX`` | nested | histogram statistic (Rx)       |
 | |                                 |        | (直方图统计(Rx))               |
 +-+---------------------------------+--------+--------------------------------+
 | | ``ETHTOOL_A_STATS_GRP_HIST_TX`` | nested | histogram statistic (Tx)       |
 | |                                 |        | (直方图统计(Tx))               |
 +-+---------------------------------+--------+--------------------------------+

Users specify which groups of statistics they are requesting via
the ``ETHTOOL_A_STATS_GROUPS`` bitset. Currently defined values are:
(用户通过 ``ETHTOOL_A_STATS_GROUPS`` 位集指定请求哪些统计组。当前定义的值为：)

 ====================== ======== ===============================================
 ETHTOOL_STATS_ETH_MAC  eth-mac  Basic IEEE 802.3 MAC statistics (30.3.1.1.*) (基本 IEEE 802.3 MAC 统计(30.3.1.1.*))
 ETHTOOL_STATS_ETH_PHY  eth-phy  Basic IEEE 802.3 PHY statistics (30.3.2.1.*) (基本 IEEE 802.3 PHY 统计(30.3.2.1.*))
 ETHTOOL_STATS_ETH_CTRL eth-ctrl Basic IEEE 802.3 MAC Ctrl statistics (30.3.3.*) (基本 IEEE 802.3 MAC 控制统计(30.3.3.*))
 ETHTOOL_STATS_RMON     rmon     RMON (RFC 2819) statistics (RMON(RFC 2819) 统计)
 ETHTOOL_STATS_PHY      phy      Additional PHY statistics, not defined by IEEE (IEEE 未定义的附加 PHY 统计)
 ====================== ======== ===============================================

Each group should have a corresponding ``ETHTOOL_A_STATS_GRP`` in the reply.
``ETHTOOL_A_STATS_GRP_ID`` identifies which group's statistics nest contains.
``ETHTOOL_A_STATS_GRP_SS_ID`` identifies the string set ID for the names of
the statistics in the group, if available.
(每个组在应答中都应有对应的 ``ETHTOOL_A_STATS_GRP``。``ETHTOOL_A_STATS_GRP_ID``
标识统计嵌套所属的组。``ETHTOOL_A_STATS_GRP_SS_ID`` 标识组中统计名称的字符串集
ID(如果有)。)

Statistics are added to the ``ETHTOOL_A_STATS_GRP`` nest under
``ETHTOOL_A_STATS_GRP_STAT``. ``ETHTOOL_A_STATS_GRP_STAT`` should contain
single 8 byte (u64) attribute inside - the type of that attribute is
the statistic ID and the value is the value of the statistic.
Each group has its own interpretation of statistic IDs.
Attribute IDs correspond to strings from the string set identified
by ``ETHTOOL_A_STATS_GRP_SS_ID``. Complex statistics (such as RMON histogram
entries) are also listed inside ``ETHTOOL_A_STATS_GRP`` and do not have
a string defined in the string set.
(统计信息通过 ``ETHTOOL_A_STATS_GRP_STAT`` 添加到 ``ETHTOOL_A_STATS_GRP`` 嵌套
中。``ETHTOOL_A_STATS_GRP_STAT`` 内部应包含单个 8 字节(u64)属性——该属性的
类型即统计 ID，值即统计值。
每组对统计 ID 有自己的解释。
属性 ID 对应于由 ``ETHTOOL_A_STATS_GRP_SS_ID`` 标识的字符串集中的字符串。复杂
统计(如 RMON 直方图条目)也列在 ``ETHTOOL_A_STATS_GRP`` 中，但在字符串集中没有
定义对应字符串。)

RMON "histogram" counters count number of packets within given size range.
Because RFC does not specify the ranges beyond the standard 1518 MTU devices
differ in definition of buckets. For this reason the definition of packet ranges
is left to each driver.
(RMON "直方图"计数器统计给定大小范围内的数据包数量。由于 RFC 未规定超出标准
1518 MTU 的范围，各设备对桶的定义不同。因此数据包范围的定义留给各驱动自行
决定。)

``ETHTOOL_A_STATS_GRP_HIST_RX`` and ``ETHTOOL_A_STATS_GRP_HIST_TX`` nests
contain the following attributes:
(``ETHTOOL_A_STATS_GRP_HIST_RX`` 和 ``ETHTOOL_A_STATS_GRP_HIST_TX`` 嵌套包含
以下属性：)

 ================================= ====== ===================================
 ETHTOOL_A_STATS_RMON_HIST_BKT_LOW u32    low bound of the packet size bucket (数据包大小桶的下界)
 ETHTOOL_A_STATS_RMON_HIST_BKT_HI  u32    high bound of the bucket (桶的上界)
 ETHTOOL_A_STATS_RMON_HIST_VAL     u64    packet counter (数据包计数器)
 ================================= ====== ===================================

Low and high bounds are inclusive, for example:
(下界和上界都是闭区间，例如：)

 ============================= ==== ====
 RFC statistic (RFC 统计项)    low  high (高)
 ============================= ==== ====
 etherStatsPkts64Octets          0    64
 etherStatsPkts512to1023Octets 512  1023
 ============================= ==== ====

``ETHTOOL_A_STATS_SRC`` is optional. Similar to ``PAUSE_GET``, it takes values
from ``enum ethtool_mac_stats_src``. If absent from the request, stats will be
provided with an ``ETHTOOL_A_STATS_SRC`` attribute in the response equal to
``ETHTOOL_MAC_STATS_SRC_AGGREGATE``.
(``ETHTOOL_A_STATS_SRC`` 是可选的。与 ``PAUSE_GET`` 类似，它取
``enum ethtool_mac_stats_src`` 中的值。若请求中没有该属性，响应中的
``ETHTOOL_A_STATS_SRC`` 属性将等于 ``ETHTOOL_MAC_STATS_SRC_AGGREGATE``，并据此
提供统计信息。)

PHC_VCLOCKS_GET
(PHC_VCLOCKS_GET，PHC 虚拟时钟信息获取)
===============

Query device PHC virtual clocks information.
(查询设备的 PHC 虚拟时钟信息。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_PHC_VCLOCKS_HEADER``      nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_PHC_VCLOCKS_HEADER``      nested  reply header (应答头)
  ``ETHTOOL_A_PHC_VCLOCKS_NUM``         u32     PHC virtual clocks number (PHC 虚拟时钟数量)
  ``ETHTOOL_A_PHC_VCLOCKS_INDEX``       s32     PHC index array (PHC 索引数组)
  ====================================  ======  ==========================

MODULE_GET
(MODULE_GET，收发器模块参数获取)
==========

Gets transceiver module parameters.
(获取收发器模块参数。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_MODULE_HEADER``            nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ======================================  ======  ==========================
  ``ETHTOOL_A_MODULE_HEADER``             nested  reply header (应答头)
  ``ETHTOOL_A_MODULE_POWER_MODE_POLICY``  u8      power mode policy (功率模式策略)
  ``ETHTOOL_A_MODULE_POWER_MODE``         u8      operational power mode (运行功率模式)
  ======================================  ======  ==========================

The optional ``ETHTOOL_A_MODULE_POWER_MODE_POLICY`` attribute encodes the
transceiver module power mode policy enforced by the host. The default policy
is driver-dependent, but "auto" is the recommended default and it should be
implemented by new drivers and drivers where conformance to a legacy behavior
is not critical.
(可选的 ``ETHTOOL_A_MODULE_POWER_MODE_POLICY`` 属性编码主机强制执行的收发器
模块功率模式策略。默认策略取决于驱动，但推荐把 "auto" 作为默认值；新驱动以及
不严格要求兼容传统行为的驱动都应实现它。)

The optional ``ETHTHOOL_A_MODULE_POWER_MODE`` attribute encodes the operational
power mode policy of the transceiver module. It is only reported when a module
is plugged-in. Possible values are:
(可选的 ``ETHTHOOL_A_MODULE_POWER_MODE`` 属性编码收发器模块的运行功率模式。
仅在模块已插入时才报告。可能的值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_module_power_mode

MODULE_SET
(MODULE_SET，收发器模块参数设置)
==========

Sets transceiver module parameters.
(设置收发器模块参数。)

Request contents:
(请求内容：)

  ======================================  ======  ==========================
  ``ETHTOOL_A_MODULE_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_MODULE_POWER_MODE_POLICY``  u8      power mode policy (功率模式策略)
  ======================================  ======  ==========================

When set, the optional ``ETHTOOL_A_MODULE_POWER_MODE_POLICY`` attribute is used
to set the transceiver module power policy enforced by the host. Possible
values are:
(设置时，可选的 ``ETHTOOL_A_MODULE_POWER_MODE_POLICY`` 属性用于设置主机强制
执行的收发器模块功率策略。可能的值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_module_power_mode_policy

For SFF-8636 modules, low power mode is forced by the host according to table
6-10 in revision 2.10a of the specification.
(对于 SFF-8636 模块，主机按规范 2.10a 版表 6-10 强制低功率模式。)

For CMIS modules, low power mode is forced by the host according to table 6-12
in revision 5.0 of the specification.
(对于 CMIS 模块，主机按规范 5.0 版表 6-12 强制低功率模式。)

PSE_GET
(PSE_GET，PSE 属性获取)
=======

Gets PSE attributes.
(获取 PSE 属性。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_PSE_HEADER``               nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ==========================================  ======  =============================
  ``ETHTOOL_A_PSE_HEADER``                    nested  reply header (应答头)
  ``ETHTOOL_A_PODL_PSE_ADMIN_STATE``             u32  Operational state of the PoDL
                                                      PSE functions (PoDL PSE 功能的运行状态)
  ``ETHTOOL_A_PODL_PSE_PW_D_STATUS``             u32  power detection status of the
                                                      PoDL PSE. (PoDL PSE 的功率检测状态。)
  ``ETHTOOL_A_C33_PSE_ADMIN_STATE``              u32  Operational state of the PoE
                                                      PSE functions. (PoE PSE 功能的运行状态。)
  ``ETHTOOL_A_C33_PSE_PW_D_STATUS``              u32  power detection status of the
                                                      PoE PSE. (PoE PSE 的功率检测状态。)
  ``ETHTOOL_A_C33_PSE_PW_CLASS``                 u32  power class of the PoE PSE. (PoE PSE 的功率等级。)
  ``ETHTOOL_A_C33_PSE_ACTUAL_PW``                u32  actual power drawn on the
                                                      PoE PSE. (PoE PSE 上实际吸取的功率。)
  ``ETHTOOL_A_C33_PSE_EXT_STATE``                u32  power extended state of the
                                                      PoE PSE. (PoE PSE 的功率扩展状态。)
  ``ETHTOOL_A_C33_PSE_EXT_SUBSTATE``             u32  power extended substatus of
                                                      the PoE PSE. (PoE PSE 的功率扩展子状态。)
  ``ETHTOOL_A_C33_PSE_AVAIL_PW_LIMIT``           u32  currently configured power
                                                      limit of the PoE PSE. (PoE PSE 当前配置的功率限制。)
  ``ETHTOOL_A_C33_PSE_PW_LIMIT_RANGES``       nested  Supported power limit
                                                      configuration ranges. (支持的功率限制配置范围。)
  ``ETHTOOL_A_PSE_PW_D_ID``                      u32  Index of the PSE power domain (PSE 功率域索引)
  ``ETHTOOL_A_PSE_PRIO_MAX``                     u32  Priority maximum configurable
                                                      on the PoE PSE (PoE PSE 上可配置的最大优先级)
  ``ETHTOOL_A_PSE_PRIO``                         u32  Priority of the PoE PSE
                                                      currently configured (PoE PSE 当前配置的优先级)
  ==========================================  ======  =============================

When set, the optional ``ETHTOOL_A_PODL_PSE_ADMIN_STATE`` attribute identifies
the operational state of the PoDL PSE functions.  The operational state of the
PSE function can be changed using the ``ETHTOOL_A_PODL_PSE_ADMIN_CONTROL``
action. This attribute corresponds to ``IEEE 802.3-2018`` 30.15.1.1.2
aPoDLPSEAdminState. Possible values are:
(设置时，可选的 ``ETHTOOL_A_PODL_PSE_ADMIN_STATE`` 属性标识 PoDL PSE 功能的
运行状态。PSE 功能的运行状态可通过 ``ETHTOOL_A_PODL_PSE_ADMIN_CONTROL`` 动作
更改。该属性对应 ``IEEE 802.3-2018`` 30.15.1.1.2 aPoDLPSEAdminState。可能的
值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_podl_pse_admin_state

The same goes for ``ETHTOOL_A_C33_PSE_ADMIN_STATE`` implementing
``IEEE 802.3-2022`` 30.9.1.1.2 aPSEAdminState.
(``ETHTOOL_A_C33_PSE_ADMIN_STATE`` 同理，实现 ``IEEE 802.3-2022`` 30.9.1.1.2
aPSEAdminState。)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_c33_pse_admin_state

When set, the optional ``ETHTOOL_A_PODL_PSE_PW_D_STATUS`` attribute identifies
the power detection status of the PoDL PSE.  The status depend on internal PSE
state machine and automatic PD classification support. This attribute
corresponds to ``IEEE 802.3-2018`` 30.15.1.1.3 aPoDLPSEPowerDetectionStatus.
Possible values are:
(设置时，可选的 ``ETHTOOL_A_PODL_PSE_PW_D_STATUS`` 属性标识 PoDL PSE 的功率
检测状态。该状态取决于内部 PSE 状态机以及自动 PD 分类支持。该属性对应
``IEEE 802.3-2018`` 30.15.1.1.3 aPoDLPSEPowerDetectionStatus。可能的值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_podl_pse_pw_d_status

The same goes for ``ETHTOOL_A_C33_PSE_ADMIN_PW_D_STATUS`` implementing
``IEEE 802.3-2022`` 30.9.1.1.5 aPSEPowerDetectionStatus.
(``ETHTOOL_A_C33_PSE_ADMIN_PW_D_STATUS`` 同理，实现 ``IEEE 802.3-2022``
30.9.1.1.5 aPSEPowerDetectionStatus。)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_c33_pse_pw_d_status

When set, the optional ``ETHTOOL_A_C33_PSE_PW_CLASS`` attribute identifies
the power class of the C33 PSE. It depends on the class negotiated between
the PSE and the PD. This attribute corresponds to ``IEEE 802.3-2022``
30.9.1.1.8 aPSEPowerClassification.
(设置时，可选的 ``ETHTOOL_A_C33_PSE_PW_CLASS`` 属性标识 C33 PSE 的功率等级。
它取决于 PSE 与 PD 之间协商的等级。该属性对应 ``IEEE 802.3-2022`` 30.9.1.1.8
aPSEPowerClassification。)

When set, the optional ``ETHTOOL_A_C33_PSE_ACTUAL_PW`` attribute identifies
the actual power drawn by the C33 PSE. This attribute corresponds to
``IEEE 802.3-2022`` 30.9.1.1.23 aPSEActualPower. Actual power is reported
in mW.
(设置时，可选的 ``ETHTOOL_A_C33_PSE_ACTUAL_PW`` 属性标识 C33 PSE 实际吸取的
功率。该属性对应 ``IEEE 802.3-2022`` 30.9.1.1.23 aPSEActualPower。实际功率以
mW 报告。)

When set, the optional ``ETHTOOL_A_C33_PSE_EXT_STATE`` attribute identifies
the extended error state of the C33 PSE. Possible values are:
(设置时，可选的 ``ETHTOOL_A_C33_PSE_EXT_STATE`` 属性标识 C33 PSE 的扩展错误
状态。可能的值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_c33_pse_ext_state

When set, the optional ``ETHTOOL_A_C33_PSE_EXT_SUBSTATE`` attribute identifies
the extended error state of the C33 PSE. Possible values are:
Possible values are:
(设置时，可选的 ``ETHTOOL_A_C33_PSE_EXT_SUBSTATE`` 属性标识 C33 PSE 的扩展
错误状态。可能的值为：
可能的值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_c33_pse_ext_substate_class_num_events
		  ethtool_c33_pse_ext_substate_error_condition
		  ethtool_c33_pse_ext_substate_mr_pse_enable
		  ethtool_c33_pse_ext_substate_option_detect_ted
		  ethtool_c33_pse_ext_substate_option_vport_lim
		  ethtool_c33_pse_ext_substate_ovld_detected
		  ethtool_c33_pse_ext_substate_pd_dll_power_type
		  ethtool_c33_pse_ext_substate_power_not_available
		  ethtool_c33_pse_ext_substate_short_detected

When set, the optional ``ETHTOOL_A_C33_PSE_AVAIL_PW_LIMIT`` attribute
identifies the C33 PSE power limit in mW.
(设置时，可选的 ``ETHTOOL_A_C33_PSE_AVAIL_PW_LIMIT`` 属性标识以 mW 计的 C33
PSE 功率限制。)

When set the optional ``ETHTOOL_A_C33_PSE_PW_LIMIT_RANGES`` nested attribute
identifies the C33 PSE power limit ranges through
``ETHTOOL_A_C33_PSE_PWR_VAL_LIMIT_RANGE_MIN`` and
``ETHTOOL_A_C33_PSE_PWR_VAL_LIMIT_RANGE_MAX``.
If the controller works with fixed classes, the min and max values will be
equal.
(设置时，可选的 ``ETHTOOL_A_C33_PSE_PW_LIMIT_RANGES`` 嵌套属性通过
``ETHTOOL_A_C33_PSE_PWR_VAL_LIMIT_RANGE_MIN`` 和
``ETHTOOL_A_C33_PSE_PWR_VAL_LIMIT_RANGE_MAX`` 标识 C33 PSE 的功率限制范围。
若控制器使用固定等级，则最小值和最大值相等。)

The ``ETHTOOL_A_PSE_PW_D_ID`` attribute identifies the index of PSE power
domain.
(``ETHTOOL_A_PSE_PW_D_ID`` 属性标识 PSE 功率域的索引。)

When set, the optional ``ETHTOOL_A_PSE_PRIO_MAX`` attribute identifies
the PSE maximum priority value.
When set, the optional ``ETHTOOL_A_PSE_PRIO`` attributes is used to
identifies the currently configured PSE priority.
For a description of PSE priority attributes, see ``PSE_SET``.
(设置时，可选的 ``ETHTOOL_A_PSE_PRIO_MAX`` 属性标识 PSE 最大优先级值。
设置时，可选的 ``ETHTOOL_A_PSE_PRIO`` 属性用于标识当前配置的 PSE 优先级。
PSE 优先级属性的描述见 ``PSE_SET``。)

PSE_SET
(PSE_SET，PSE 参数设置)
=======

Sets PSE parameters.
(设置 PSE 参数。)

Request contents:
(请求内容：)

  ======================================  ======  =============================
  ``ETHTOOL_A_PSE_HEADER``                nested  request header (请求头)
  ``ETHTOOL_A_PODL_PSE_ADMIN_CONTROL``       u32  Control PoDL PSE Admin state (控制 PoDL PSE 管理状态)
  ``ETHTOOL_A_C33_PSE_ADMIN_CONTROL``        u32  Control PSE Admin state (控制 PSE 管理状态)
  ``ETHTOOL_A_C33_PSE_AVAIL_PWR_LIMIT``      u32  Control PoE PSE available
                                                  power limit (控制 PoE PSE 可用功率限制)
  ``ETHTOOL_A_PSE_PRIO``                     u32  Control priority of the
                                                  PoE PSE (控制 PoE PSE 的优先级)
  ======================================  ======  =============================

When set, the optional ``ETHTOOL_A_PODL_PSE_ADMIN_CONTROL`` attribute is used
to control PoDL PSE Admin functions. This option implements
``IEEE 802.3-2018`` 30.15.1.2.1 acPoDLPSEAdminControl. See
``ETHTOOL_A_PODL_PSE_ADMIN_STATE`` for supported values.
(设置时，可选的 ``ETHTOOL_A_PODL_PSE_ADMIN_CONTROL`` 属性用于控制 PoDL PSE
管理功能。该选项实现 ``IEEE 802.3-2018`` 30.15.1.2.1 acPoDLPSEAdminControl。
支持的值见 ``ETHTOOL_A_PODL_PSE_ADMIN_STATE``。)

The same goes for ``ETHTOOL_A_C33_PSE_ADMIN_CONTROL`` implementing
``IEEE 802.3-2022`` 30.9.1.2.1 acPSEAdminControl.
(``ETHTOOL_A_C33_PSE_ADMIN_CONTROL`` 同理，实现 ``IEEE 802.3-2022`` 30.9.1.2.1
acPSEAdminControl。)

When set, the optional ``ETHTOOL_A_C33_PSE_AVAIL_PWR_LIMIT`` attribute is
used to control the available power value limit for C33 PSE in milliwatts.
This attribute corresponds  to the `pse_available_power` variable described in
``IEEE 802.3-2022`` 33.2.4.4 Variables  and `pse_avail_pwr` in 145.2.5.4
Variables, which are described in power classes.
(设置时，可选的 ``ETHTOOL_A_C33_PSE_AVAIL_PWR_LIMIT`` 属性用于以毫瓦为单位
控制 C33 PSE 的可用功率值限制。该属性对应 ``IEEE 802.3-2022`` 33.2.4.4
Variables 中描述的 `pse_available_power` 变量以及 145.2.5.4 Variables 中的
`pse_avail_pwr`，这些都在功率等级中描述。)

It was decided to use milliwatts for this interface to unify it with other
power monitoring interfaces, which also use milliwatts, and to align with
various existing products that document power consumption in watts rather than
classes. If power limit configuration based on classes is needed, the
conversion can be done in user space, for example by ethtool.
(决定在该接口中使用毫瓦，以便与其他同样使用毫瓦的功率监测接口统一，并与各种
以瓦而非等级记载功耗的现有产品保持一致。如果需要基于等级的功率限制配置，可以
在用户空间完成换算，例如由 ethtool 完成。)

When set, the optional ``ETHTOOL_A_PSE_PRIO`` attributes is used to
control the PSE priority. Allowed priority value are between zero and
the value of ``ETHTOOL_A_PSE_PRIO_MAX`` attribute.
(设置时，可选的 ``ETHTOOL_A_PSE_PRIO`` 属性用于控制 PSE 优先级。允许的优先级
值介于 0 与 ``ETHTOOL_A_PSE_PRIO_MAX`` 属性的值之间。)

A lower value indicates a higher priority, meaning that a priority value
of 0 corresponds to the highest port priority.
Port priority serves two functions:
(值越低表示优先级越高，即优先级值 0 对应最高的端口优先级。
端口优先级有两个作用：)

 - Power-up Order: After a reset, ports are powered up in order of their
   priority from highest to lowest. Ports with higher priority
   (lower values) power up first.
   (上电顺序：复位后，端口按优先级从高到低的顺序上电。优先级较高(值较低)的
   端口先上电。)
 - Shutdown Order: When the power budget is exceeded, ports with lower
   priority (higher values) are turned off first.
   (关断顺序：当超出功率预算时，优先级较低(值较高)的端口先被关断。)

PSE_NTF
(PSE_NTF，PSE 事件通知)
=======

Notify PSE events.
(通知 PSE 事件。)

Notification contents:
(通知内容：)

  ===============================  ======  ========================
  ``ETHTOOL_A_PSE_HEADER``         nested  request header (请求头)
  ``ETHTOOL_A_PSE_EVENTS``         bitset  PSE events (PSE 事件)
  ===============================  ======  ========================

When set, the optional ``ETHTOOL_A_PSE_EVENTS`` attribute identifies the
PSE events.
(设置时，可选的 ``ETHTOOL_A_PSE_EVENTS`` 属性标识所发生的 PSE 事件。)

.. kernel-doc:: include/uapi/linux/ethtool_netlink_generated.h
    :identifiers: ethtool_pse_event

RSS_GET
(RSS_GET，RSS 设置获取)
=======

Get indirection table, hash key and hash function info associated with a
RSS context of an interface similar to ``ETHTOOL_GRSSH`` ioctl request.
(获取接口某 RSS 上下文关联的间接表、哈希键和哈希函数信息，类似
``ETHTOOL_GRSSH`` ioctl 请求。)

Request contents:
(请求内容：)

=====================================  ======  ============================
  ``ETHTOOL_A_RSS_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_RSS_CONTEXT``            u32     context number (上下文编号)
  ``ETHTOOL_A_RSS_START_CONTEXT``      u32     start context number (dumps) (起始上下文编号(转储))
=====================================  ======  ============================

``ETHTOOL_A_RSS_CONTEXT`` specifies which RSS context number to query,
if not set context 0 (the main context) is queried. Dumps can be filtered
by device (only listing contexts of a given netdev). Filtering single
context number is not supported but ``ETHTOOL_A_RSS_START_CONTEXT``
can be used to start dumping context from the given number (primarily
used to ignore context 0s and only dump additional contexts).
(``ETHTOOL_A_RSS_CONTEXT`` 指定要查询的 RSS 上下文编号；若未设置，则查询上下文
0(主上下文)。转储可以按设备过滤(只列出给定 netdev 的上下文)。不支持按单个
上下文编号过滤，但可以使用 ``ETHTOOL_A_RSS_START_CONTEXT`` 从给定编号开始转储
上下文(主要用于忽略上下文 0，只转储附加上下文)。)

Kernel response contents:
(内核响应内容：)

=====================================  ======  ===============================
  ``ETHTOOL_A_RSS_HEADER``             nested  reply header (应答头)
  ``ETHTOOL_A_RSS_CONTEXT``            u32     context number (上下文编号)
  ``ETHTOOL_A_RSS_HFUNC``              u32     RSS hash func (RSS 哈希函数)
  ``ETHTOOL_A_RSS_INDIR``              binary  Indir table bytes (间接表字节)
  ``ETHTOOL_A_RSS_HKEY``               binary  Hash key bytes (哈希键字节)
  ``ETHTOOL_A_RSS_INPUT_XFRM``         u32     RSS input data transformation (RSS 输入数据变换)
  ``ETHTOOL_A_RSS_FLOW_HASH``          nested  Header fields included in hash (哈希中包含的报文字段)
=====================================  ======  ===============================

ETHTOOL_A_RSS_HFUNC attribute is bitmap indicating the hash function
being used. Current supported options are toeplitz, xor or crc32.
ETHTOOL_A_RSS_INDIR attribute returns RSS indirection table where each byte
indicates queue number.
ETHTOOL_A_RSS_INPUT_XFRM attribute is a bitmap indicating the type of
transformation applied to the input protocol fields before given to the RSS
hfunc. Current supported options are symmetric-xor and symmetric-or-xor.
ETHTOOL_A_RSS_FLOW_HASH carries per-flow type bitmask of which header
fields are included in the hash calculation.
(ETHTOOL_A_RSS_HFUNC 属性是位图，指示正在使用的哈希函数。当前支持的选项为
toeplitz、xor 或 crc32。
ETHTOOL_A_RSS_INDIR 属性返回 RSS 间接表，其中每个字节指示队列编号。
ETHTOOL_A_RSS_INPUT_XFRM 属性是位图，指示在交给 RSS 哈希函数之前对输入协议
字段施加的变换类型。当前支持的选项为 symmetric-xor 和 symmetric-or-xor。
ETHTOOL_A_RSS_FLOW_HASH 携带按流类型的位掩码，指示哈希计算中包含哪些报文
字段。)

RSS_SET
(RSS_SET，设置 RSS 设置)
=======

Request contents:
(请求内容：)

=====================================  ======  ==============================
  ``ETHTOOL_A_RSS_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_RSS_CONTEXT``            u32     context number (上下文编号)
  ``ETHTOOL_A_RSS_HFUNC``              u32     RSS hash func (RSS 哈希函数)
  ``ETHTOOL_A_RSS_INDIR``              binary  Indir table bytes (间接表字节)
  ``ETHTOOL_A_RSS_HKEY``               binary  Hash key bytes (哈希键字节)
  ``ETHTOOL_A_RSS_INPUT_XFRM``         u32     RSS input data transformation (RSS 输入数据变换)
  ``ETHTOOL_A_RSS_FLOW_HASH``          nested  Header fields included in hash (哈希中包含的报文字段)
=====================================  ======  ==============================

``ETHTOOL_A_RSS_INDIR`` is the minimal RSS table the user expects. Kernel and
the device driver may replicate the table if its smaller than smallest table
size supported by the device. For example if user requests ``[0, 1]`` but the
device needs at least 8 entries - the real table in use will end up being
``[0, 1, 0, 1, 0, 1, 0, 1]``. Most devices require the table size to be power
of 2, so tables which size is not a power of 2 will likely be rejected.
Using table of size 0 will reset the indirection table to the default.
(``ETHTOOL_A_RSS_INDIR`` 是用户期望的最小 RSS 表。若它小于设备支持的最小表
大小，内核和设备驱动可以复制该表。例如用户请求 ``[0, 1]`` 而设备至少需要 8 个
条目——实际使用的表最终会是 ``[0, 1, 0, 1, 0, 1, 0, 1]``。大多数设备要求表
大小为 2 的幂，因此大小不是 2 的幂的表很可能被拒绝。
使用大小为 0 的表会把间接表重置为默认值。)

RSS_CREATE_ACT
(RSS_CREATE_ACT，创建 RSS 上下文)
==============

Request contents:
(请求内容：)

=====================================  ======  ==============================
  ``ETHTOOL_A_RSS_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_RSS_CONTEXT``            u32     context number (上下文编号)
  ``ETHTOOL_A_RSS_HFUNC``              u32     RSS hash func (RSS 哈希函数)
  ``ETHTOOL_A_RSS_INDIR``              binary  Indir table bytes (间接表字节)
  ``ETHTOOL_A_RSS_HKEY``               binary  Hash key bytes (哈希键字节)
  ``ETHTOOL_A_RSS_INPUT_XFRM``         u32     RSS input data transformation (RSS 输入数据变换)
=====================================  ======  ==============================

Kernel response contents:
(内核响应内容：)

=====================================  ======  ==============================
  ``ETHTOOL_A_RSS_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_RSS_CONTEXT``            u32     context number (上下文编号)
=====================================  ======  ==============================

Create an additional RSS context, if ``ETHTOOL_A_RSS_CONTEXT`` is not
specified kernel will allocate one automatically.
(创建附加 RSS 上下文；若未指定 ``ETHTOOL_A_RSS_CONTEXT``，内核会自动分配
一个。)

RSS_DELETE_ACT
(RSS_DELETE_ACT，删除 RSS 上下文)
==============

Request contents:
(请求内容：)

=====================================  ======  ==============================
  ``ETHTOOL_A_RSS_HEADER``             nested  request header (请求头)
  ``ETHTOOL_A_RSS_CONTEXT``            u32     context number (上下文编号)
=====================================  ======  ==============================

Delete an additional RSS context.
(删除附加 RSS 上下文。)

PLCA_GET_CFG
(PLCA_GET_CFG，PLCA RS 参数获取)
============

Gets the IEEE 802.3cg-2019 Clause 148 Physical Layer Collision Avoidance
(PLCA) Reconciliation Sublayer (RS) attributes.
(获取 IEEE 802.3cg-2019 第 148 章物理层冲突避免(PLCA)协调子层(RS)的属性。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_PLCA_HEADER``              nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ======================================  ======  =============================
  ``ETHTOOL_A_PLCA_HEADER``               nested  reply header (应答头)
  ``ETHTOOL_A_PLCA_VERSION``              u16     Supported PLCA management
                                                  interface standard/version (支持的 PLCA 管理接口标准/版本)
  ``ETHTOOL_A_PLCA_ENABLED``              u8      PLCA Admin State (PLCA 管理状态)
  ``ETHTOOL_A_PLCA_NODE_ID``              u32     PLCA unique local node ID (PLCA 唯一本地节点 ID)
  ``ETHTOOL_A_PLCA_NODE_CNT``             u32     Number of PLCA nodes on the
                                                  network, including the
                                                  coordinator (网络上的 PLCA 节点数，含协调器)
  ``ETHTOOL_A_PLCA_TO_TMR``               u32     Transmit Opportunity Timer
                                                  value in bit-times (BT) (发送机会定时器值，单位位时间(BT))
  ``ETHTOOL_A_PLCA_BURST_CNT``            u32     Number of additional packets
                                                  the node is allowed to send
                                                  within a single TO (允许节点在单个 TO 内发送的附加数据包数)
  ``ETHTOOL_A_PLCA_BURST_TMR``            u32     Time to wait for the MAC to
                                                  transmit a new frame before
                                                  terminating the burst (在结束突发前等待 MAC 发送新帧的时间)
  ======================================  ======  =============================

When set, the optional ``ETHTOOL_A_PLCA_VERSION`` attribute indicates which
standard and version the PLCA management interface complies to. When not set,
the interface is vendor-specific and (possibly) supplied by the driver.
The OPEN Alliance SIG specifies a standard register map for 10BASE-T1S PHYs
embedding the PLCA Reconciliation Sublayer. See "10BASE-T1S PLCA Management
Registers" at https://www.opensig.org/about/specifications/.
(设置时，可选的 ``ETHTOOL_A_PLCA_VERSION`` 属性指示 PLCA 管理接口遵循哪个
标准及版本。未设置时，该接口是厂商特定的，并(可能)由驱动提供。OPEN Alliance
SIG 为内嵌 PLCA 协调子层的 10BASE-T1S PHY 规定了标准寄存器映射。参见
https://www.opensig.org/about/specifications/ 上的 "10BASE-T1S PLCA Management
Registers"。)

When set, the optional ``ETHTOOL_A_PLCA_ENABLED`` attribute indicates the
administrative state of the PLCA RS. When not set, the node operates in "plain"
CSMA/CD mode. This option is corresponding to ``IEEE 802.3cg-2019`` 30.16.1.1.1
aPLCAAdminState / 30.16.1.2.1 acPLCAAdminControl.
(设置时，可选的 ``ETHTOOL_A_PLCA_ENABLED`` 属性指示 PLCA RS 的管理状态。未
设置时，节点以"普通"CSMA/CD 模式运行。该选项对应 ``IEEE 802.3cg-2019``
30.16.1.1.1 aPLCAAdminState / 30.16.1.2.1 acPLCAAdminControl。)

When set, the optional ``ETHTOOL_A_PLCA_NODE_ID`` attribute indicates the
configured local node ID of the PHY. This ID determines which transmit
opportunity (TO) is reserved for the node to transmit into. This option is
corresponding to ``IEEE 802.3cg-2019`` 30.16.1.1.4 aPLCALocalNodeID. The valid
range for this attribute is [0 .. 255] where 255 means "not configured".
(设置时，可选的 ``ETHTOOL_A_PLCA_NODE_ID`` 属性指示 PHY 配置的本地节点 ID。
该 ID 决定为节点保留哪个发送机会(TO)用于发送。该选项对应 ``IEEE 802.3cg-2019``
30.16.1.1.4 aPLCALocalNodeID。该属性的有效范围是 [0 .. 255]，其中 255 表示
"未配置"。)

When set, the optional ``ETHTOOL_A_PLCA_NODE_CNT`` attribute indicates the
configured maximum number of PLCA nodes on the mixing-segment. This number
determines the total number of transmit opportunities generated during a
PLCA cycle. This attribute is relevant only for the PLCA coordinator, which is
the node with aPLCALocalNodeID set to 0. Follower nodes ignore this setting.
This option is corresponding to ``IEEE 802.3cg-2019`` 30.16.1.1.3
aPLCANodeCount. The valid range for this attribute is [1 .. 255].
(设置时，可选的 ``ETHTOOL_A_PLCA_NODE_CNT`` 属性指示混合段上配置的最大 PLCA
节点数。该数字决定一个 PLCA 周期内产生的发送机会总数。该属性只与 PLCA 协调器
相关，协调器是 aPLCALocalNodeID 设为 0 的节点。从节点忽略此设置。该选项对应
``IEEE 802.3cg-2019`` 30.16.1.1.3 aPLCANodeCount。该属性的有效范围是
[1 .. 255]。)

When set, the optional ``ETHTOOL_A_PLCA_TO_TMR`` attribute indicates the
configured value of the transmit opportunity timer in bit-times. This value
must be set equal across all nodes sharing the medium for PLCA to work
correctly. This option is corresponding to ``IEEE 802.3cg-2019`` 30.16.1.1.5
aPLCATransmitOpportunityTimer. The valid range for this attribute is
[0 .. 255].
(设置时，可选的 ``ETHTOOL_A_PLCA_TO_TMR`` 属性指示以位时间为单位配置的发送
机会定时器值。为使 PLCA 正常工作，共享介质的全部节点都必须把该值设置为相同。
该选项对应 ``IEEE 802.3cg-2019`` 30.16.1.1.5
aPLCATransmitOpportunityTimer。该属性的有效范围是 [0 .. 255]。)

When set, the optional ``ETHTOOL_A_PLCA_BURST_CNT`` attribute indicates the
configured number of extra packets that the node is allowed to send during a
single transmit opportunity. By default, this attribute is 0, meaning that
the node can only send a single frame per TO. When greater than 0, the PLCA RS
keeps the TO after any transmission, waiting for the MAC to send a new frame
for up to aPLCABurstTimer BTs. This can only happen a number of times per PLCA
cycle up to the value of this parameter. After that, the burst is over and the
normal counting of TOs resumes. This option is corresponding to
``IEEE 802.3cg-2019`` 30.16.1.1.6 aPLCAMaxBurstCount. The valid range for this
attribute is [0 .. 255].
(设置时，可选的 ``ETHTOOL_A_PLCA_BURST_CNT`` 属性指示配置的节点在单个发送
机会内允许发送的额外数据包数。默认情况下该属性为 0，意味着节点每个 TO 只能
发送一帧。当大于 0 时，PLCA RS 在任何一次传输后都保持该 TO，等待 MAC 在最多
aPLCABurstTimer 个位时间内发送新帧。每个 PLCA 周期内这种情况最多只能发生本
参数值指定的次数。此后突发结束，恢复正常 TO 计数。该选项对应
``IEEE 802.3cg-2019`` 30.16.1.1.6 aPLCAMaxBurstCount。该属性的有效范围是
[0 .. 255]。)

When set, the optional ``ETHTOOL_A_PLCA_BURST_TMR`` attribute indicates how
many bit-times the PLCA RS waits for the MAC to initiate a new transmission
when aPLCAMaxBurstCount is greater than 0. If the MAC fails to send a new
frame within this time, the burst ends and the counting of TOs resumes.
Otherwise, the new frame is sent as part of the current burst. This option
is corresponding to ``IEEE 802.3cg-2019`` 30.16.1.1.7 aPLCABurstTimer. The
valid range for this attribute is [0 .. 255]. Although, the value should be
set greater than the Inter-Frame-Gap (IFG) time of the MAC (plus some margin)
for PLCA burst mode to work as intended.
(设置时，可选的 ``ETHTOOL_A_PLCA_BURST_TMR`` 属性指示当 aPLCAMaxBurstCount
大于 0 时，PLCA RS 等待 MAC 发起新传输的位时间数。若 MAC 未能在该时间内发送
新帧，突发结束，恢复 TO 计数；否则新帧作为当前突发的一部分发送。该选项对应
``IEEE 802.3cg-2019`` 30.16.1.1.7 aPLCABurstTimer。该属性的有效范围是
[0 .. 255]。不过，为使 PLCA 突发模式按预期工作，该值应设置为大于 MAC 的帧间隙
(IFG)时间(外加一定余量)。)

PLCA_SET_CFG
(PLCA_SET_CFG，PLCA RS 参数设置)
============

Sets PLCA RS parameters.
(设置 PLCA RS 参数。)

Request contents:
(请求内容：)

  ======================================  ======  =============================
  ``ETHTOOL_A_PLCA_HEADER``               nested  request header (请求头)
  ``ETHTOOL_A_PLCA_ENABLED``              u8      PLCA Admin State (PLCA 管理状态)
  ``ETHTOOL_A_PLCA_NODE_ID``              u8      PLCA unique local node ID (PLCA 唯一本地节点 ID)
  ``ETHTOOL_A_PLCA_NODE_CNT``             u8      Number of PLCA nodes on the
                                                  network, including the
                                                  coordinator (网络上的 PLCA 节点数，含协调器)
  ``ETHTOOL_A_PLCA_TO_TMR``               u8      Transmit Opportunity Timer
                                                  value in bit-times (BT) (发送机会定时器值，单位位时间(BT))
  ``ETHTOOL_A_PLCA_BURST_CNT``            u8      Number of additional packets
                                                  the node is allowed to send
                                                  within a single TO (允许节点在单个 TO 内发送的附加数据包数)
  ``ETHTOOL_A_PLCA_BURST_TMR``            u8      Time to wait for the MAC to
                                                  transmit a new frame before
                                                  terminating the burst (在结束突发前等待 MAC 发送新帧的时间)
  ======================================  ======  =============================

For a description of each attribute, see ``PLCA_GET_CFG``.
(各属性的描述见 ``PLCA_GET_CFG``。)

PLCA_GET_STATUS
(PLCA_GET_STATUS，PLCA RS 状态获取)
===============

Gets PLCA RS status information.
(获取 PLCA RS 状态信息。)

Request contents:
(请求内容：)

  =====================================  ======  ==========================
  ``ETHTOOL_A_PLCA_HEADER``              nested  request header (请求头)
  =====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ======================================  ======  =============================
  ``ETHTOOL_A_PLCA_HEADER``               nested  reply header (应答头)
  ``ETHTOOL_A_PLCA_STATUS``               u8      PLCA RS operational status (PLCA RS 运行状态)
  ======================================  ======  =============================

When set, the ``ETHTOOL_A_PLCA_STATUS`` attribute indicates whether the node is
detecting the presence of the BEACON on the network. This flag is
corresponding to ``IEEE 802.3cg-2019`` 30.16.1.1.2 aPLCAStatus.
(设置时，``ETHTOOL_A_PLCA_STATUS`` 属性指示节点是否正在检测网络上的 BEACON。
该标志对应 ``IEEE 802.3cg-2019`` 30.16.1.1.2 aPLCAStatus。)

MM_GET
(MM_GET，MAC 合并层参数获取)
======

Retrieve 802.3 MAC Merge parameters.
(检索 802.3 MAC 合并(MAC Merge)参数。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_MM_HEADER``               nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  =================================  ======  ===================================
  ``ETHTOOL_A_MM_HEADER``            nested  request header (请求头)
  ``ETHTOOL_A_MM_PMAC_ENABLED``      bool    set if RX of preemptible and SMD-V
                                             frames is enabled (若已启用可抢占帧和 SMD-V 帧的 RX 则设置)
  ``ETHTOOL_A_MM_TX_ENABLED``        bool    set if TX of preemptible frames is
                                             administratively enabled (might be
                                             inactive if verification failed) (若可抢占帧的 TX 被管理性启用则设置(验证失败时可能处于非活动状态))
  ``ETHTOOL_A_MM_TX_ACTIVE``         bool    set if TX of preemptible frames is
                                             operationally enabled (若可抢占帧的 TX 被运行性启用则设置)
  ``ETHTOOL_A_MM_TX_MIN_FRAG_SIZE``  u32     minimum size of transmitted
                                             non-final fragments, in octets (发送的非末尾分片的最小大小，单位八位组)
  ``ETHTOOL_A_MM_RX_MIN_FRAG_SIZE``  u32     minimum size of received non-final
                                             fragments, in octets (接收的非末尾分片的最小大小，单位八位组)
  ``ETHTOOL_A_MM_VERIFY_ENABLED``    bool    set if TX of SMD-V frames is
                                             administratively enabled (若 SMD-V 帧的 TX 被管理性启用则设置)
  ``ETHTOOL_A_MM_VERIFY_STATUS``     u8      state of the verification function (验证功能的状态)
  ``ETHTOOL_A_MM_VERIFY_TIME``       u32     delay between verification attempts (验证尝试之间的延时)
  ``ETHTOOL_A_MM_MAX_VERIFY_TIME```  u32     maximum verification interval
                                             supported by device (设备支持的最大验证间隔)
  ``ETHTOOL_A_MM_STATS``             nested  IEEE 802.3-2018 subclause 30.14.1
                                             oMACMergeEntity statistics counters (IEEE 802.3-2018 第 30.14.1 节 oMACMergeEntity 统计计数器)
  =================================  ======  ===================================

The attributes are populated by the device driver through the following
structure:
(这些属性由设备驱动通过以下结构填充：)

.. kernel-doc:: include/linux/ethtool.h
    :identifiers: ethtool_mm_state

The ``ETHTOOL_A_MM_VERIFY_STATUS`` will report one of the values from
(``ETHTOOL_A_MM_VERIFY_STATUS`` 将报告以下取值之一：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_mm_verify_status

If ``ETHTOOL_A_MM_VERIFY_ENABLED`` was passed as false in the ``MM_SET``
command, ``ETHTOOL_A_MM_VERIFY_STATUS`` will report either
``ETHTOOL_MM_VERIFY_STATUS_INITIAL`` or ``ETHTOOL_MM_VERIFY_STATUS_DISABLED``,
otherwise it should report one of the other states.
(若在 ``MM_SET`` 命令中传入的 ``ETHTOOL_A_MM_VERIFY_ENABLED`` 为 false，
``ETHTOOL_A_MM_VERIFY_STATUS`` 将报告 ``ETHTOOL_MM_VERIFY_STATUS_INITIAL`` 或
``ETHTOOL_MM_VERIFY_STATUS_DISABLED``；否则它应报告其他状态之一。)

It is recommended that drivers start with the pMAC disabled, and enable it upon
user space request. It is also recommended that user space does not depend upon
the default values from ``ETHTOOL_MSG_MM_GET`` requests.
(建议驱动以禁用 pMAC 的状态启动，并在用户空间请求时启用它。同时建议用户空间
不要依赖 ``ETHTOOL_MSG_MM_GET`` 请求返回的默认值。)

``ETHTOOL_A_MM_STATS`` are reported if ``ETHTOOL_FLAG_STATS`` was set in
``ETHTOOL_A_HEADER_FLAGS``. The attribute will be empty if driver did not
report any statistics. Drivers fill in the statistics in the following
structure:
(若在 ``ETHTOOL_A_HEADER_FLAGS`` 中设置了 ``ETHTOOL_FLAG_STATS``，则报告
``ETHTOOL_A_MM_STATS``。若驱动未报告任何统计信息，该属性将为空。驱动在以下
结构中填写统计信息：)

.. kernel-doc:: include/linux/ethtool.h
    :identifiers: ethtool_mm_stats

MM_SET
(MM_SET，MAC 合并层配置修改)
======

Modifies the configuration of the 802.3 MAC Merge layer.
(修改 802.3 MAC 合并层的配置。)

Request contents:
(请求内容：)

  =================================  ======  ==========================
  ``ETHTOOL_A_MM_VERIFY_TIME``       u32     see MM_GET description (见 MM_GET 说明)
  ``ETHTOOL_A_MM_VERIFY_ENABLED``    bool    see MM_GET description (见 MM_GET 说明)
  ``ETHTOOL_A_MM_TX_ENABLED``        bool    see MM_GET description (见 MM_GET 说明)
  ``ETHTOOL_A_MM_PMAC_ENABLED``      bool    see MM_GET description (见 MM_GET 说明)
  ``ETHTOOL_A_MM_TX_MIN_FRAG_SIZE``  u32     see MM_GET description (见 MM_GET 说明)
  =================================  ======  ==========================

The attributes are propagated to the driver through the following structure:
(这些属性通过以下结构传递给驱动：)

.. kernel-doc:: include/linux/ethtool.h
    :identifiers: ethtool_mm_cfg

MODULE_FW_FLASH_ACT
(MODULE_FW_FLASH_ACT，收发器模块固件烧录)
===================

Flashes transceiver module firmware.
(烧录收发器模块固件。)

Request contents:
(请求内容：)

  =======================================  ======  ===========================
  ``ETHTOOL_A_MODULE_FW_FLASH_HEADER``     nested  request header (请求头)
  ``ETHTOOL_A_MODULE_FW_FLASH_FILE_NAME``  string  firmware image file name (固件映像文件名)
  ``ETHTOOL_A_MODULE_FW_FLASH_PASSWORD``   u32     transceiver module password (收发器模块密码)
  =======================================  ======  ===========================

The firmware update process consists of three logical steps:
(固件更新过程由三个逻辑步骤组成：)

1. Downloading a firmware image to the transceiver module and validating it.
   (把固件映像下载到收发器模块并进行验证。)
2. Running the firmware image.
   (运行固件映像。)
3. Committing the firmware image so that it is run upon reset.
   (提交固件映像，使其在复位后运行。)

When flash command is given, those three steps are taken in that order.
(执行 flash 命令时，按此顺序执行这三个步骤。)

This message merely schedules the update process and returns immediately
without blocking. The process then runs asynchronously.
Since it can take several minutes to complete, during the update process
notifications are emitted from the kernel to user space updating it about
the status and progress.
(该消息只是排定更新过程并立即返回，不阻塞。之后该过程异步运行。
由于可能需要几分钟才能完成，更新过程中内核会向用户空间发出通知，告知其状态和
进度。)

The ``ETHTOOL_A_MODULE_FW_FLASH_FILE_NAME`` attribute encodes the firmware
image file name. The firmware image is downloaded to the transceiver module,
validated, run and committed.
(``ETHTOOL_A_MODULE_FW_FLASH_FILE_NAME`` 属性编码固件映像文件名。固件映像会被
下载到收发器模块、验证、运行并提交。)

The optional ``ETHTOOL_A_MODULE_FW_FLASH_PASSWORD`` attribute encodes a password
that might be required as part of the transceiver module firmware update
process.
(可选的 ``ETHTOOL_A_MODULE_FW_FLASH_PASSWORD`` 属性编码收发器模块固件更新过程
中可能需要的密码。)

The firmware update process can take several minutes to complete. Therefore,
during the update process notifications are emitted from the kernel to user
space updating it about the status and progress.
(固件更新过程可能需要几分钟才能完成。因此，更新过程中内核会向用户空间发出
通知，告知其状态和进度。)



Notification contents:
(通知内容：)

 +---------------------------------------------------+--------+----------------+
 | ``ETHTOOL_A_MODULE_FW_FLASH_HEADER``              | nested | reply header   |
 |                                                   |        | (应答头)       |
 +---------------------------------------------------+--------+----------------+
 | ``ETHTOOL_A_MODULE_FW_FLASH_STATUS``              | u32    | status (状态)  |
 +---------------------------------------------------+--------+----------------+
 | ``ETHTOOL_A_MODULE_FW_FLASH_STATUS_MSG``          | string | status message |
 |                                                   |        | (状态消息)     |
 +---------------------------------------------------+--------+----------------+
 | ``ETHTOOL_A_MODULE_FW_FLASH_DONE``                | uint   | progress       |
 |                                                   |        | (已完成量)     |
 +---------------------------------------------------+--------+----------------+
 | ``ETHTOOL_A_MODULE_FW_FLASH_TOTAL``               | uint   | total (总量)   |
 +---------------------------------------------------+--------+----------------+

The ``ETHTOOL_A_MODULE_FW_FLASH_STATUS`` attribute encodes the current status
of the firmware update process. Possible values are:
(``ETHTOOL_A_MODULE_FW_FLASH_STATUS`` 属性编码固件更新过程的当前状态。可能的
值为：)

.. kernel-doc:: include/uapi/linux/ethtool.h
    :identifiers: ethtool_module_fw_flash_status

The ``ETHTOOL_A_MODULE_FW_FLASH_STATUS_MSG`` attribute encodes a status message
string.
(``ETHTOOL_A_MODULE_FW_FLASH_STATUS_MSG`` 属性编码状态消息字符串。)

The ``ETHTOOL_A_MODULE_FW_FLASH_DONE`` and ``ETHTOOL_A_MODULE_FW_FLASH_TOTAL``
attributes encode the completed and total amount of work, respectively.
(``ETHTOOL_A_MODULE_FW_FLASH_DONE`` 和 ``ETHTOOL_A_MODULE_FW_FLASH_TOTAL``
属性分别编码已完成的工作量和总工作量。)

PHY_GET
(PHY_GET，PHY 信息获取)
=======

Retrieve information about a given Ethernet PHY sitting on the link. The DO
operation returns all available information about dev->phydev. User can also
specify a PHY_INDEX, in which case the DO request returns information about that
specific PHY.
(检索链路上给定以太网 PHY 的信息。DO 操作返回关于 dev->phydev 的所有可用
信息。用户也可以指定 PHY_INDEX，此时 DO 请求返回关于该特定 PHY 的信息。)

As there can be more than one PHY, the DUMP operation can be used to list the PHYs
present on a given interface, by passing an interface index or name in
the dump request.
(由于可能有多个 PHY，DUMP 操作可用于列出给定接口上存在的 PHY，方法是在转储
请求中传入接口索引或名称。)

For more information, refer to :ref:`phy_link_topology`
(更多信息请参阅 :ref:`phy_link_topology`。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_PHY_HEADER``              nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ===================================== ======  ===============================
  ``ETHTOOL_A_PHY_HEADER``              nested  request header (请求头)
  ``ETHTOOL_A_PHY_INDEX``               u32     the phy's unique index, that can
                                                be used for phy-specific
                                                requests (PHY 的唯一索引，可用于 PHY 特定请求)
  ``ETHTOOL_A_PHY_DRVNAME``             string  the phy driver name (PHY 驱动名)
  ``ETHTOOL_A_PHY_NAME``                string  the phy device name (PHY 设备名)
  ``ETHTOOL_A_PHY_UPSTREAM_TYPE``       u32     the type of device this phy is
                                                connected to (该 PHY 所连接设备的类型)
  ``ETHTOOL_A_PHY_UPSTREAM_INDEX``      u32     the PHY index of the upstream
                                                PHY (上游 PHY 的 PHY 索引)
  ``ETHTOOL_A_PHY_UPSTREAM_SFP_NAME``   string  if this PHY is connected to
                                                its parent PHY through an SFP
                                                bus, the name of this sfp bus (若该 PHY 通过 SFP 总线连接到其父 PHY，则为该 SFP 总线的名称)
  ``ETHTOOL_A_PHY_DOWNSTREAM_SFP_NAME`` string  if the phy controls an sfp bus,
                                                the name of the sfp bus (若该 PHY 控制一条 SFP 总线，则为该总线的名称)
  ===================================== ======  ===============================

When ``ETHTOOL_A_PHY_UPSTREAM_TYPE`` is PHY_UPSTREAM_PHY, the PHY's parent is
another PHY.
(当 ``ETHTOOL_A_PHY_UPSTREAM_TYPE`` 为 PHY_UPSTREAM_PHY 时，该 PHY 的父设备是
另一个 PHY。)

TSCONFIG_GET
(TSCONFIG_GET，硬件时间戳配置获取)
============

Retrieves the information about the current hardware timestamping source and
configuration.
(检索当前硬件时间戳源和配置的信息。)

It is similar to the deprecated ``SIOCGHWTSTAMP`` ioctl request.
(它类似于已弃用的 ``SIOCGHWTSTAMP`` ioctl 请求。)

Request contents:
(请求内容：)

  ====================================  ======  ==========================
  ``ETHTOOL_A_TSCONFIG_HEADER``         nested  request header (请求头)
  ====================================  ======  ==========================

Kernel response contents:
(内核响应内容：)

  ======================================== ======  ============================
  ``ETHTOOL_A_TSCONFIG_HEADER``            nested  request header (请求头)
  ``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` nested  PTP hw clock provider (PTP 硬件时钟提供者)
  ``ETHTOOL_A_TSCONFIG_TX_TYPES``          bitset  hwtstamp Tx type (hwtstamp Tx 类型)
  ``ETHTOOL_A_TSCONFIG_RX_FILTERS``        bitset  hwtstamp Rx filter (hwtstamp Rx 过滤器)
  ``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS``	   u32     hwtstamp flags (hwtstamp 标志)
  ======================================== ======  ============================

When set the ``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` attribute identifies the
source of the hw timestamping provider. It is composed by
``ETHTOOL_A_TS_HWTSTAMP_PROVIDER_INDEX`` attribute which describe the index of
the PTP device and ``ETHTOOL_A_TS_HWTSTAMP_PROVIDER_QUALIFIER`` which describe
the qualifier of the timestamp.
(设置时，``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` 属性标识硬件时间戳提供者的
来源。它由描述 PTP 设备索引的 ``ETHTOOL_A_TS_HWTSTAMP_PROVIDER_INDEX`` 属性和
描述时间戳限定符的 ``ETHTOOL_A_TS_HWTSTAMP_PROVIDER_QUALIFIER`` 组成。)

When set the ``ETHTOOL_A_TSCONFIG_TX_TYPES``, ``ETHTOOL_A_TSCONFIG_RX_FILTERS``
and the ``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS`` attributes identify the Tx
type, the Rx filter and the flags configured for the current hw timestamping
provider. The attributes are propagated to the driver through the following
structure:
(设置时，``ETHTOOL_A_TSCONFIG_TX_TYPES``、``ETHTOOL_A_TSCONFIG_RX_FILTERS`` 和
``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS`` 属性标识为当前硬件时间戳提供者配置的 Tx
类型、Rx 过滤器和标志。这些属性通过以下结构传递给驱动：)

.. kernel-doc:: include/linux/net_tstamp.h
    :identifiers: kernel_hwtstamp_config

TSCONFIG_SET
(TSCONFIG_SET，硬件时间戳配置设置)
============

Set the information about the current hardware timestamping source and
configuration.
(设置当前硬件时间戳源和配置的信息。)

It is similar to the deprecated ``SIOCSHWTSTAMP`` ioctl request.
(它类似于已弃用的 ``SIOCSHWTSTAMP`` ioctl 请求。)

Request contents:
(请求内容：)

  ======================================== ======  ============================
  ``ETHTOOL_A_TSCONFIG_HEADER``            nested  request header (请求头)
  ``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` nested  PTP hw clock provider (PTP 硬件时钟提供者)
  ``ETHTOOL_A_TSCONFIG_TX_TYPES``          bitset  hwtstamp Tx type (hwtstamp Tx 类型)
  ``ETHTOOL_A_TSCONFIG_RX_FILTERS``        bitset  hwtstamp Rx filter (hwtstamp Rx 过滤器)
  ``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS``	   u32     hwtstamp flags (hwtstamp 标志)
  ======================================== ======  ============================

Kernel response contents:
(内核响应内容：)

  ======================================== ======  ============================
  ``ETHTOOL_A_TSCONFIG_HEADER``            nested  request header (请求头)
  ``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` nested  PTP hw clock provider (PTP 硬件时钟提供者)
  ``ETHTOOL_A_TSCONFIG_TX_TYPES``          bitset  hwtstamp Tx type (hwtstamp Tx 类型)
  ``ETHTOOL_A_TSCONFIG_RX_FILTERS``        bitset  hwtstamp Rx filter (hwtstamp Rx 过滤器)
  ``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS``	   u32     hwtstamp flags (hwtstamp 标志)
  ======================================== ======  ============================

For a description of each attribute, see ``TSCONFIG_GET``.
(各属性的描述见 ``TSCONFIG_GET``。)

MSE_GET
(MSE_GET，MSE 诊断信息获取)
=======

Retrieves detailed Mean Square Error (MSE) diagnostic information from the PHY.
(从 PHY 检索详细的均方误差(Mean Square Error，MSE)诊断信息。)

Request Contents:
(请求内容：)

  ====================================  ======  ============================
  ``ETHTOOL_A_MSE_HEADER``              nested  request header (请求头)
  ====================================  ======  ============================

Kernel Response Contents:
(内核响应内容：)

  ====================================  ======  ================================
  ``ETHTOOL_A_MSE_HEADER``              nested  reply header (应答头)
  ``ETHTOOL_A_MSE_CAPABILITIES``        nested  capability/scale info for MSE
                                                measurements (MSE 测量的能力/标度信息)
  ``ETHTOOL_A_MSE_CHANNEL_A``           nested  snapshot for Channel A (通道 A 的快照)
  ``ETHTOOL_A_MSE_CHANNEL_B``           nested  snapshot for Channel B (通道 B 的快照)
  ``ETHTOOL_A_MSE_CHANNEL_C``           nested  snapshot for Channel C (通道 C 的快照)
  ``ETHTOOL_A_MSE_CHANNEL_D``           nested  snapshot for Channel D (通道 D 的快照)
  ``ETHTOOL_A_MSE_WORST_CHANNEL``       nested  snapshot for worst channel (最差通道的快照)
  ``ETHTOOL_A_MSE_LINK``                nested  snapshot for link-wide aggregate (链路级聚合的快照)
  ====================================  ======  ================================

MSE Capabilities
(MSE 能力)
----------------

This nested attribute reports the capability / scaling properties used to
interpret snapshot values.
(该嵌套属性报告用于解释快照值的能力/缩放属性。)

  ============================================== ======  =========================
  ``ETHTOOL_A_MSE_CAPABILITIES_MAX_AVERAGE_MSE`` uint    max avg_mse scale (最大 avg_mse 标度)
  ``ETHTOOL_A_MSE_CAPABILITIES_MAX_PEAK_MSE``    uint    max peak_mse scale (最大 peak_mse 标度)
  ``ETHTOOL_A_MSE_CAPABILITIES_REFRESH_RATE_PS`` uint    sample rate (picoseconds) (采样率(皮秒))
  ``ETHTOOL_A_MSE_CAPABILITIES_NUM_SYMBOLS``     uint    symbols per HW sample (每个硬件采样的符号数)
  ============================================== ======  =========================

The max-average/peak fields are included only if the corresponding metric
is supported by the PHY. Their absence indicates that the metric is not
available.
(仅当 PHY 支持相应指标时才包含 max-average/peak 字段。它们的缺失表示该指标
不可用。)

See ``struct phy_mse_capability`` kernel documentation in
``include/linux/phy.h``.
(参见 ``include/linux/phy.h`` 中 ``struct phy_mse_capability`` 的内核文档。)

MSE Snapshot
(MSE 快照)
------------

Each per-channel nest contains an atomic snapshot of MSE values for that
selector (channel A/B/C/D, worst channel, or link).
(每个通道嵌套包含该选择器(通道 A/B/C/D、最差通道或链路)的 MSE 值的原子
快照。)

  ==========================================  ======  ===================
  ``ETHTOOL_A_MSE_SNAPSHOT_AVERAGE_MSE``      uint    average MSE value (平均 MSE 值)
  ``ETHTOOL_A_MSE_SNAPSHOT_PEAK_MSE``         uint    current peak MSE (当前峰值 MSE)
  ``ETHTOOL_A_MSE_SNAPSHOT_WORST_PEAK_MSE``   uint    worst-case peak MSE (最坏情况峰值 MSE)
  ==========================================  ======  ===================

Within each channel nest, only the metrics supported by the PHY will be present.
(在每个通道嵌套内，只有 PHY 支持的指标才会出现。)

See ``struct phy_mse_snapshot`` kernel documentation in
``include/linux/phy.h``.
(参见 ``include/linux/phy.h`` 中 ``struct phy_mse_snapshot`` 的内核文档。)

Request translation
(Request translation，请求对照表)
===================

The following table maps ioctl commands to netlink commands providing their
functionality. Entries with "n/a" in right column are commands which do not
have their netlink replacement yet. Entries which "n/a" in the left column
are netlink only.
(下表将 ioctl 命令映射到提供其功能的 netlink 命令。右列为 "n/a" 的条目是尚无
netlink 替代物的命令。左列为 "n/a" 的条目是 netlink 独有。)

  =================================== =====================================
  ioctl command (ioctl 命令)            netlink command (netlink 命令)
  =================================== =====================================
  ``ETHTOOL_GSET``                    ``ETHTOOL_MSG_LINKINFO_GET``
                                      ``ETHTOOL_MSG_LINKMODES_GET``
  ``ETHTOOL_SSET``                    ``ETHTOOL_MSG_LINKINFO_SET``
                                      ``ETHTOOL_MSG_LINKMODES_SET``
  ``ETHTOOL_GDRVINFO``                n/a
  ``ETHTOOL_GREGS``                   n/a
  ``ETHTOOL_GWOL``                    ``ETHTOOL_MSG_WOL_GET``
  ``ETHTOOL_SWOL``                    ``ETHTOOL_MSG_WOL_SET``
  ``ETHTOOL_GMSGLVL``                 ``ETHTOOL_MSG_DEBUG_GET``
  ``ETHTOOL_SMSGLVL``                 ``ETHTOOL_MSG_DEBUG_SET``
  ``ETHTOOL_NWAY_RST``                n/a
  ``ETHTOOL_GLINK``                   ``ETHTOOL_MSG_LINKSTATE_GET``
  ``ETHTOOL_GEEPROM``                 n/a
  ``ETHTOOL_SEEPROM``                 n/a
  ``ETHTOOL_GCOALESCE``               ``ETHTOOL_MSG_COALESCE_GET``
  ``ETHTOOL_SCOALESCE``               ``ETHTOOL_MSG_COALESCE_SET``
  ``ETHTOOL_GRINGPARAM``              ``ETHTOOL_MSG_RINGS_GET``
  ``ETHTOOL_SRINGPARAM``              ``ETHTOOL_MSG_RINGS_SET``
  ``ETHTOOL_GPAUSEPARAM``             ``ETHTOOL_MSG_PAUSE_GET``
  ``ETHTOOL_SPAUSEPARAM``             ``ETHTOOL_MSG_PAUSE_SET``
  ``ETHTOOL_GRXCSUM``                 ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SRXCSUM``                 ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GTXCSUM``                 ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_STXCSUM``                 ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GSG``                     ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SSG``                     ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_TEST``                    n/a
  ``ETHTOOL_GSTRINGS``                ``ETHTOOL_MSG_STRSET_GET``
  ``ETHTOOL_PHYS_ID``                 n/a
  ``ETHTOOL_GSTATS``                  n/a
  ``ETHTOOL_GTSO``                    ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_STSO``                    ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GPERMADDR``               rtnetlink ``RTM_GETLINK``
  ``ETHTOOL_GUFO``                    ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SUFO``                    ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GGSO``                    ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SGSO``                    ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GFLAGS``                  ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SFLAGS``                  ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GPFLAGS``                 ``ETHTOOL_MSG_PRIVFLAGS_GET``
  ``ETHTOOL_SPFLAGS``                 ``ETHTOOL_MSG_PRIVFLAGS_SET``
  ``ETHTOOL_GRXFH``                   ``ETHTOOL_MSG_RSS_GET``
  ``ETHTOOL_SRXFH``                   ``ETHTOOL_MSG_RSS_SET``
  ``ETHTOOL_GGRO``                    ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SGRO``                    ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GRXRINGS``                n/a
  ``ETHTOOL_GRXCLSRLCNT``             n/a
  ``ETHTOOL_GRXCLSRULE``              n/a
  ``ETHTOOL_GRXCLSRLALL``             n/a
  ``ETHTOOL_SRXCLSRLDEL``             n/a
  ``ETHTOOL_SRXCLSRLINS``             n/a
  ``ETHTOOL_FLASHDEV``                n/a
  ``ETHTOOL_RESET``                   n/a
  ``ETHTOOL_SRXNTUPLE``               n/a
  ``ETHTOOL_GRXNTUPLE``               n/a
  ``ETHTOOL_GSSET_INFO``              ``ETHTOOL_MSG_STRSET_GET``
  ``ETHTOOL_GRXFHINDIR``              ``ETHTOOL_MSG_RSS_GET``
  ``ETHTOOL_SRXFHINDIR``              ``ETHTOOL_MSG_RSS_SET``
  ``ETHTOOL_GFEATURES``               ``ETHTOOL_MSG_FEATURES_GET``
  ``ETHTOOL_SFEATURES``               ``ETHTOOL_MSG_FEATURES_SET``
  ``ETHTOOL_GCHANNELS``               ``ETHTOOL_MSG_CHANNELS_GET``
  ``ETHTOOL_SCHANNELS``               ``ETHTOOL_MSG_CHANNELS_SET``
  ``ETHTOOL_SET_DUMP``                n/a
  ``ETHTOOL_GET_DUMP_FLAG``           n/a
  ``ETHTOOL_GET_DUMP_DATA``           n/a
  ``ETHTOOL_GET_TS_INFO``             ``ETHTOOL_MSG_TSINFO_GET``
  ``ETHTOOL_GMODULEINFO``             ``ETHTOOL_MSG_MODULE_EEPROM_GET``
  ``ETHTOOL_GMODULEEEPROM``           ``ETHTOOL_MSG_MODULE_EEPROM_GET``
  ``ETHTOOL_GEEE``                    ``ETHTOOL_MSG_EEE_GET``
  ``ETHTOOL_SEEE``                    ``ETHTOOL_MSG_EEE_SET``
  ``ETHTOOL_GRSSH``                   ``ETHTOOL_MSG_RSS_GET``
  ``ETHTOOL_SRSSH``                   n/a
  ``ETHTOOL_GTUNABLE``                n/a
  ``ETHTOOL_STUNABLE``                n/a
  ``ETHTOOL_GPHYSTATS``               n/a
  ``ETHTOOL_PERQUEUE``                n/a
  ``ETHTOOL_GLINKSETTINGS``           ``ETHTOOL_MSG_LINKINFO_GET``
                                      ``ETHTOOL_MSG_LINKMODES_GET``
  ``ETHTOOL_SLINKSETTINGS``           ``ETHTOOL_MSG_LINKINFO_SET``
                                      ``ETHTOOL_MSG_LINKMODES_SET``
  ``ETHTOOL_PHY_GTUNABLE``            n/a
  ``ETHTOOL_PHY_STUNABLE``            n/a
  ``ETHTOOL_GFECPARAM``               ``ETHTOOL_MSG_FEC_GET``
  ``ETHTOOL_SFECPARAM``               ``ETHTOOL_MSG_FEC_SET``
  n/a                                 ``ETHTOOL_MSG_CABLE_TEST_ACT``
  n/a                                 ``ETHTOOL_MSG_CABLE_TEST_TDR_ACT``
  n/a                                 ``ETHTOOL_MSG_TUNNEL_INFO_GET``
  n/a                                 ``ETHTOOL_MSG_PHC_VCLOCKS_GET``
  n/a                                 ``ETHTOOL_MSG_MODULE_GET``
  n/a                                 ``ETHTOOL_MSG_MODULE_SET``
  n/a                                 ``ETHTOOL_MSG_PLCA_GET_CFG``
  n/a                                 ``ETHTOOL_MSG_PLCA_SET_CFG``
  n/a                                 ``ETHTOOL_MSG_PLCA_GET_STATUS``
  n/a                                 ``ETHTOOL_MSG_MM_GET``
  n/a                                 ``ETHTOOL_MSG_MM_SET``
  n/a                                 ``ETHTOOL_MSG_MODULE_FW_FLASH_ACT``
  n/a                                 ``ETHTOOL_MSG_PHY_GET``
  ``SIOCGHWTSTAMP``                   ``ETHTOOL_MSG_TSCONFIG_GET``
  ``SIOCSHWTSTAMP``                   ``ETHTOOL_MSG_TSCONFIG_SET``
  =================================== =====================================
