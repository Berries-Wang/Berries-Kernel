:orphan:

.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

===================================================================
mac80211_hwsim - software simulator of 802.11 radio(s) for mac80211
===================================================================

(mac80211_hwsim - mac80211 的 802.11 无线电软件模拟器)

:Copyright: |copy| 2008, Jouni Malinen <j@w1.fi>
            (版权所有 |copy| 2008, Jouni Malinen <j@w1.fi>)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
(本程序是自由软件；您可以遵照自由软件基金会发布的 GNU 通用公共许可证第 2 版的条款对其进行再分发和/或修改。)


Introduction
============
(介绍)

mac80211_hwsim is a Linux kernel module that can be used to simulate
arbitrary number of IEEE 802.11 radios for mac80211. It can be used to
test most of the mac80211 functionality and user space tools (e.g.,
hostapd and wpa_supplicant) in a way that matches very closely with
the normal case of using real WLAN hardware. From the mac80211 view
point, mac80211_hwsim is yet another hardware driver, i.e., no changes
to mac80211 are needed to use this testing tool.
(mac80211_hwsim 是一个 Linux 内核模块，可用于为 mac80211 模拟任意数量的 IEEE 802.11 无线电。它可以用来测试 mac80211 的大部分功能以及用户空间工具(例如 hostapd 和 wpa_supplicant)，其方式与使用真实 WLAN 硬件的常规场景非常接近。从 mac80211 的角度来看，mac80211_hwsim 只是又一个硬件驱动，即使用这个测试工具无需对 mac80211 做任何修改。)

The main goal for mac80211_hwsim is to make it easier for developers
to test their code and work with new features to mac80211, hostapd,
and wpa_supplicant. The simulated radios do not have the limitations
of real hardware, so it is easy to generate an arbitrary test setup
and always reproduce the same setup for future tests. In addition,
since all radio operation is simulated, any channel can be used in
tests regardless of regulatory rules.
(mac80211_hwsim 的主要目标是让开发人员更容易测试自己的代码并开发 mac80211、hostapd 和 wpa_supplicant 的新特性。模拟的无线电没有真实硬件的限制，因此很容易生成任意的测试环境，并始终为后续测试重现相同的设置。此外，由于所有无线电操作都是模拟的，测试中可以使用任何信道，而不受法规规则的约束。)

mac80211_hwsim kernel module has a parameter 'radios' that can be used
to select how many radios are simulated (default 2). This allows
configuration of both very simply setups (e.g., just a single access
point and a station) or large scale tests (multiple access points with
hundreds of stations).
(mac80211_hwsim 内核模块有一个参数 'radios'，可用于选择模拟多少个无线电(默认为 2)。这既可以配置非常简单的环境(例如只有一个接入点和一个站点)，也可以进行大规模测试(带数百个站点的多个接入点)。)

mac80211_hwsim works by tracking the current channel of each virtual
radio and copying all transmitted frames to all other radios that are
currently enabled and on the same channel as the transmitting
radio. Software encryption in mac80211 is used so that the frames are
actually encrypted over the virtual air interface to allow more
complete testing of encryption.
(mac80211_hwsim 的工作方式是：跟踪每个虚拟无线电的当前信道，并把所有发送的帧复制到当前已启用且与发送方无线电处于同一信道的所有其他无线电上。mac80211 中的软件加密被用来在虚拟空口上真正加密帧，以便对加密进行更完整的测试。)

A global monitoring netdev, hwsim#, is created independent of
mac80211. This interface can be used to monitor all transmitted frames
regardless of channel.
(一个全局监控网络设备 hwsim# 的创建独立于 mac80211。该接口可用于监视所有发送的帧，而不论其所在信道为何。)


Simple example
==============
(简单示例)

This example shows how to use mac80211_hwsim to simulate two radios:
one to act as an access point and the other as a station that
associates with the AP. hostapd and wpa_supplicant are used to take
care of WPA2-PSK authentication. In addition, hostapd is also
processing access point side of association.
(本示例展示如何使用 mac80211_hwsim 模拟两个无线电：一个用作接入点，另一个用作与该接入点关联的站点。hostapd 和 wpa_supplicant 负责 WPA2-PSK 认证。此外，hostapd 还处理关联的接入点一侧。)

::


    # Build mac80211_hwsim as part of kernel configuration

    # Load the module
    modprobe mac80211_hwsim

    # Run hostapd (AP) for wlan0
    hostapd hostapd.conf

    # Run wpa_supplicant (station) for wlan1
    wpa_supplicant -Dnl80211 -iwlan1 -c wpa_supplicant.conf


More test cases are available in hostap.git:
git://w1.fi/srv/git/hostap.git and mac80211_hwsim/tests subdirectory
(http://w1.fi/gitweb/gitweb.cgi?p=hostap.git;a=tree;f=mac80211_hwsim/tests)
(hostap.git 的 git://w1.fi/srv/git/hostap.git 以及 mac80211_hwsim/tests 子目录 (http://w1.fi/gitweb/gitweb.cgi?p=hostap.git;a=tree;f=mac80211_hwsim/tests) 中提供了更多测试用例)
