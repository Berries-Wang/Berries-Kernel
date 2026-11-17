.. SPDX-License-Identifier: GPL-2.0

===========================================================================
How to use packet injection with mac80211(如何在 mac80211 中使用数据包注入)
===========================================================================

mac80211 now allows arbitrary packets to be injected down any Monitor Mode
interface from userland.  The packet you inject needs to be composed in the
following format::
(mac80211 现在允许从用户态向任何处于监视模式 (Monitor Mode) 的接口注入
任意数据包。你注入的数据包需要按以下格式构成：)

 [ radiotap header  ]
 [ ieee80211 header ]
 [ payload ]

The radiotap format is discussed in
./Documentation/networking/radiotap-headers.rst.
(radiotap 格式在 ./Documentation/networking/radiotap-headers.rst 中
讨论。)

Despite many radiotap parameters being currently defined, most only make sense
to appear on received packets.  The following information is parsed from the
radiotap headers and used to control injection:
(尽管目前定义了许多 radiotap 参数，但它们大多数只在出现在接收数据包上时
才有意义。以下信息会从 radiotap 头中解析出来，并用于控制注入：)

 * IEEE80211_RADIOTAP_FLAGS

   =========================  ===========================================
   IEEE80211_RADIOTAP_F_FCS   FCS will be removed and recalculated (FCS 将被移除并重新计算)
   IEEE80211_RADIOTAP_F_WEP   frame will be encrypted if key available (如有可用密钥，帧将被加密)
   IEEE80211_RADIOTAP_F_FRAG  frame will be fragmented if longer than the
			      current fragmentation threshold. (若长度超过当前分片阈值，帧将被分片)
   =========================  ===========================================

 * IEEE80211_RADIOTAP_TX_FLAGS

   =============================  ========================================
   IEEE80211_RADIOTAP_F_TX_NOACK  frame should be sent without waiting for
				  an ACK even if it is a unicast frame (即使它是单播帧，发送时也无需等待 ACK)
   =============================  ========================================

 * IEEE80211_RADIOTAP_RATE

   legacy rate for the transmission (only for devices without own rate control)
   (传输的传统 (legacy) 速率 (仅适用于没有自身速率控制的设备))

 * IEEE80211_RADIOTAP_MCS

   HT rate for the transmission (only for devices without own rate control).
   Also some flags are parsed
   (传输的 HT 速率 (仅适用于没有自身速率控制的设备)。
   此外还会解析一些标志)

   ============================  ========================
   IEEE80211_RADIOTAP_MCS_SGI    use short guard interval (使用短保护间隔)
   IEEE80211_RADIOTAP_MCS_BW_40  send in HT40 mode (以 HT40 模式发送)
   ============================  ========================

 * IEEE80211_RADIOTAP_DATA_RETRIES

   number of retries when either IEEE80211_RADIOTAP_RATE or
   IEEE80211_RADIOTAP_MCS was used
   (当使用 IEEE80211_RADIOTAP_RATE 或 IEEE80211_RADIOTAP_MCS 时的
   重试次数)

 * IEEE80211_RADIOTAP_VHT

   VHT mcs and number of streams used in the transmission (only for devices
   without own rate control). Also other fields are parsed
   (传输所用的 VHT mcs 和流数 (仅适用于没有自身速率控制的设备)。
   此外还会解析其他字段)

   flags field
	IEEE80211_RADIOTAP_VHT_FLAG_SGI: use short guard interval
	(使用短保护间隔)

   (flags 字段)

   bandwidth field
	* 1: send using 40MHz channel width
	  (使用 40MHz 信道宽度发送)
	* 4: send using 80MHz channel width
	  (使用 80MHz 信道宽度发送)
	* 11: send using 160MHz channel width
	  (使用 160MHz 信道宽度发送)

   (bandwidth 字段)

The injection code can also skip all other currently defined radiotap fields
facilitating replay of captured radiotap headers directly.
(注入代码也可以跳过当前定义的所有其他 radiotap 字段，从而便于直接重放
捕获的 radiotap 头。)

Here is an example valid radiotap header defining some parameters::
(下面是一个定义了一些参数的有效 radiotap 头示例：)

	0x00, 0x00, // <-- radiotap version
	0x0b, 0x00, // <- radiotap header length
	0x04, 0x0c, 0x00, 0x00, // <-- bitmap
	0x6c, // <-- rate
	0x0c, //<-- tx power
	0x01 //<-- antenna

The ieee80211 header follows immediately afterwards, looking for example like
this::
(ieee80211 头紧随其后，例如形如下面这样：)

	0x08, 0x01, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x13, 0x22, 0x33, 0x44, 0x55, 0x66,
	0x13, 0x22, 0x33, 0x44, 0x55, 0x66,
	0x10, 0x86

Then lastly there is the payload.
(最后是载荷 (payload)。)

After composing the packet contents, it is sent by send()-ing it to a logical
mac80211 interface that is in Monitor mode.  Libpcap can also be used,
(which is easier than doing the work to bind the socket to the right
interface), along the following lines:::
(在构成好数据包内容之后，通过把它 send() 到一个处于监视模式的逻辑
mac80211 接口来发送。也可以使用 Libpcap (这比手动完成把套接字绑定到
正确接口的工作要容易)，大致按照如下方式进行：)

	ppcap = pcap_open_live(szInterfaceName, 800, 1, 20, szErrbuf);
	...
	r = pcap_inject(ppcap, u8aSendBuffer, nLength);

You can also find a link to a complete inject application here:
(你还可以在这里找到一个完整的注入应用程序的链接：)

https://wireless.wiki.kernel.org/en/users/Documentation/packetspammer

Andy Green <andy@warmcat.com>
