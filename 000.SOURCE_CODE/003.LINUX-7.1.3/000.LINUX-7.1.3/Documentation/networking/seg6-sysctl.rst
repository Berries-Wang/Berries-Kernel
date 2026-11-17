.. SPDX-License-Identifier: GPL-2.0

====================
Seg6 Sysfs variables
====================

(Seg6 Sysfs 变量)


/proc/sys/net/conf/<iface>/seg6_* variables:
============================================

(/proc/sys/net/conf/<iface>/seg6_* 变量)

seg6_enabled - BOOL
(seg6_enabled - BOOL(布尔值))
	Accept or drop SR-enabled IPv6 packets on this interface.
	(在该接口上接受还是丢弃支持 SR 的 IPv6 数据包。)

	Relevant packets are those with SRH present and DA = local.
	(相关数据包是指带有 SRH 且目的地址 (DA) 等于本地地址的数据包。)

	* 0 - disabled (default)
	  (0 - 禁用(默认))
	* not 0 - enabled
	  (非 0 - 启用)

seg6_require_hmac - INTEGER
(seg6_require_hmac - INTEGER(整数))
	Define HMAC policy for ingress SR-enabled packets on this interface.
	(为该接口上入方向的支持 SR 的数据包定义 HMAC 策略。)

	* -1 - Ignore HMAC field
	  (-1 - 忽略 HMAC 字段)
	* 0 - Accept SR packets without HMAC, validate SR packets with HMAC
	  (0 - 接受不带 HMAC 的 SR 数据包，对带 HMAC 的 SR 数据包进行验证)
	* 1 - Drop SR packets without HMAC, validate SR packets with HMAC
	  (1 - 丢弃不带 HMAC 的 SR 数据包，对带 HMAC 的 SR 数据包进行验证)

	Default is 0.
	(默认值为 0。)

/proc/sys/net/ipv6/seg6_* variables:
====================================

(/proc/sys/net/ipv6/seg6_* 变量)

seg6_flowlabel - INTEGER
(seg6_flowlabel - INTEGER(整数))
	Controls the behaviour of computing the flowlabel of outer
	IPv6 header in case of SR T.encaps
	(在 SR T.encaps 的情况下，控制计算外层 IPv6 头 flowlabel 的行为)

	 == =======================================================
	 -1  set flowlabel to zero.
	     (将 flowlabel 设为零。)
	  0  copy flowlabel from Inner packet in case of Inner IPv6
	     (Set flowlabel to 0 in case IPv4/L2)
	     (内层为 IPv6 时从内层数据包复制 flowlabel；内层为 IPv4/L2 时将 flowlabel 设为 0)
	  1  Compute the flowlabel using seg6_make_flowlabel()
	     (使用 seg6_make_flowlabel() 计算 flowlabel。)
	 == =======================================================

	Default is 0.
	(默认值为 0。)
