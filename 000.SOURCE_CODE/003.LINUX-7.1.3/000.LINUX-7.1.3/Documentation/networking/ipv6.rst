.. SPDX-License-Identifier: GPL-2.0

===============================
IPv6(IPv6，即互联网协议第 6 版)
===============================


Options for the ipv6 module are supplied as parameters at load time.
(ipv6 模块的选项在加载时作为参数提供。)

Module options may be given as command line arguments to the insmod
or modprobe command, but are usually specified in either
``/etc/modules.d/*.conf`` configuration files, or in a distro-specific
configuration file.
(模块选项可以作为 insmod 或 modprobe 命令的命令行参数给出，但通常是在 ``/etc/modules.d/*.conf`` 配置文件或特定发行版的配置文件中指定。)

The available ipv6 module parameters are listed below.  If a parameter
is not specified the default value is used.
(下面列出了可用的 ipv6 模块参数。如果未指定某个参数，则使用默认值。)

The parameters are as follows:
(参数如下：)

disable
(禁用)

	Specifies whether to load the IPv6 module, but disable all
	its functionality.  This might be used when another module
	has a dependency on the IPv6 module being loaded, but no
	IPv6 addresses or operations are desired.
	(指定是否加载 IPv6 模块但禁用其全部功能。当另一个模块依赖于 IPv6 模块被加载、但不需要任何 IPv6 地址或操作时，可能会用到它。)

	The possible values and their effects are:
	(可能的取值及其效果如下：)

	0
		IPv6 is enabled.
		(IPv6 已启用。)

		This is the default value.
		(这是默认值。)

	1
		IPv6 is disabled.
		(IPv6 已禁用。)

		No IPv6 addresses will be added to interfaces, and
		it will not be possible to open an IPv6 socket.
		(接口不会添加任何 IPv6 地址，也无法打开 IPv6 套接字。)

		A reboot is required to enable IPv6.
		(需要重新启动才能启用 IPv6。)

autoconf
(自动配置)

	Specifies whether to enable IPv6 address autoconfiguration
	on all interfaces.  This might be used when one does not wish
	for addresses to be automatically generated from prefixes
	received in Router Advertisements.
	(指定是否在所有接口上启用 IPv6 地址自动配置。当不希望根据路由通告 (Router Advertisement) 中收到的前缀自动生成地址时，可能会用到它。)

	The possible values and their effects are:
	(可能的取值及其效果如下：)

	0
		IPv6 address autoconfiguration is disabled on all interfaces.
		(所有接口上的 IPv6 地址自动配置均已禁用。)

		Only the IPv6 loopback address (::1) and link-local addresses
		will be added to interfaces.
		(接口只会添加 IPv6 环回地址 (::1) 和链路本地地址。)

	1
		IPv6 address autoconfiguration is enabled on all interfaces.
		(所有接口上的 IPv6 地址自动配置均已启用。)

		This is the default value.
		(这是默认值。)

disable_ipv6
(disable_ipv6，在所有接口上禁用 IPv6)

	Specifies whether to disable IPv6 on all interfaces.
	This might be used when no IPv6 addresses are desired.
	(指定是否在所有接口上禁用 IPv6。当不需要任何 IPv6 地址时，可能会用到它。)

	The possible values and their effects are:
	(可能的取值及其效果如下：)

	0
		IPv6 is enabled on all interfaces.
		(所有接口上都已启用 IPv6。)

		This is the default value.
		(这是默认值。)

	1
		IPv6 is disabled on all interfaces.
		(所有接口上都已禁用 IPv6。)

		No IPv6 addresses will be added to interfaces.
		(接口不会添加任何 IPv6 地址。)

