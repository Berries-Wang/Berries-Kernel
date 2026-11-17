.. SPDX-License-Identifier: GPL-2.0

==================================
IP-Aliasing(IP 别名 (IP-Aliasing))
==================================

IP-aliases are an obsolete way to manage multiple IP-addresses/masks
per interface. Newer tools such as iproute2 support multiple
address/prefixes per interface, but aliases are still supported
for backwards compatibility.
(IP 别名 (IP-aliases) 是一种已过时的、用于管理每个接口多个 IP 地址/掩码的方式。较新的工具(如 iproute2)支持每个接口配置多个地址/前缀，但为了向后兼容，别名仍然受支持。)

An alias is formed by adding a colon and a string when running ifconfig.
This string is usually numeric, but this is not a must.
(别名是通过在运行 ifconfig 时添加一个冒号和一个字符串来构成的。该字符串通常是数字，但这并非必须。)


Alias creation(创建别名)
========================

Alias creation is done by 'magic' interface naming: eg. to create a
200.1.1.1 alias for eth0 ...
::
(别名创建通过"magic"式的接口命名来完成：例如，要为 eth0 创建一个 200.1.1.1 的别名……)

  # ifconfig eth0:0 200.1.1.1  etc,etc....
	~~ -> request alias #0 creation (if not yet exists) for eth0

The corresponding route is also set up by this command.  Please note:
The route always points to the base interface.
(相应的路由也会由该命令一并建立。请注意：路由始终指向基础接口。)


Alias deletion(删除别名)
========================

The alias is removed by shutting the alias down::
(通过关闭别名即可将其删除：)

  # ifconfig eth0:0 down
	~~~~~~~~~~ -> will delete alias


Alias (re-)configuring(别名的(重新)配置)
========================================

Aliases are not real devices, but programs should be able to configure
and refer to them as usual (ifconfig, route, etc).
(别名并不是真实的设备，但程序应当能够像往常一样配置和引用它们(ifconfig、route 等)。)


Relationship with main device(与主设备的关系)
=============================================

If the base device is shut down the added aliases will be deleted too.
(如果基础设备被关闭，已添加的别名也会被一并删除。)
