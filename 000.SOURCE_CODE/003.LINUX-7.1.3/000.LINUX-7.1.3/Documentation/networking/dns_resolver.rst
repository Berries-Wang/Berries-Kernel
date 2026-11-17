.. SPDX-License-Identifier: GPL-2.0

===================
DNS Resolver Module
===================

(DNS 解析器模块)

.. Contents:

 - Overview.
 - Compilation.
 - Setting up.
 - Usage.
 - Mechanism.
 - Debugging.


Overview
(概述)
========

The DNS resolver module provides a way for kernel services to make DNS queries
by way of requesting a key of key type dns_resolver.  These queries are
upcalled to userspace through /sbin/request-key.
(DNS 解析器模块为内核服务提供了一种发起 DNS 查询的方式，即请求一个 dns_resolver
类型的密钥。这些查询会通过 /sbin/request-key 上行调用(upcall)到用户空间。)

These routines must be supported by userspace tools dns.upcall, cifs.upcall and
request-key.  It is under development and does not yet provide the full feature
set.  The features it does support include:
(这些例程必须由用户空间工具 dns.upcall、cifs.upcall 和 request-key 提供支持。该模块
仍在开发中，尚未提供完整的特性集。它已支持的特性包括：)

 * Implements the dns_resolver key_type to contact userspace.
   (实现 dns_resolver key_type 以联系用户空间。)

It does not yet support the following AFS features:
(它尚不支持以下 AFS 特性：)

 * DNS query support for AFSDB resource record.
   (对 AFSDB 资源记录的 DNS 查询支持。)

This code is extracted from the CIFS filesystem.
(这段代码提取自 CIFS 文件系统。)


Compilation
(编译)
===========

The module should be enabled by turning on the kernel configuration options::
(应当通过打开以下内核配置选项来启用该模块：)

	CONFIG_DNS_RESOLVER	- tristate "DNS Resolver support"


Setting up
(设置)
==========

To set up this facility, the /etc/request-key.conf file must be altered so that
/sbin/request-key can appropriately direct the upcalls.  For example, to handle
basic dname to IPv4/IPv6 address resolution, the following line should be
added::
(要设置这一设施，必须修改 /etc/request-key.conf 文件，使 /sbin/request-key 能够恰当地
引导上行调用。例如，要处理基本的 dname 到 IPv4/IPv6 地址解析，应添加以下行：)


	#OP	TYPE		DESC	CO-INFO	PROGRAM ARG1 ARG2 ARG3 ...
	#======	============	=======	=======	==========================
	create	dns_resolver  	*	*	/usr/sbin/cifs.upcall %k

To direct a query for query type 'foo', a line of the following should be added
before the more general line given above as the first match is the one taken::
(要将查询引导到查询类型 'foo'，应在上面给出的更一般的行之前添加如下所示的一行，因为
生效的是第一个匹配的行：)

	create	dns_resolver  	foo:*	*	/usr/sbin/dns.foo %k


Usage
(用法)
=====

To make use of this facility, first ``dns_resolver.h`` must be included::
(要使用这一设施，首先必须包含 ``dns_resolver.h``：)

	#include <linux/dns_resolver.h>

Then queries may be made by calling::
(然后就可以通过调用以下函数来发起查询：)

	int dns_query(const char *type, const char *name, size_t namelen,
		     const char *options, char **_result, time_t *_expiry);

This is the basic access function.  It looks for a cached DNS query and if
it doesn't find it, it upcalls to userspace to make a new DNS query, which
may then be cached.  The key description is constructed as a string of the
form::
(这是基本的访问函数。它会查找缓存的 DNS 查询，如果没有找到，就上行调用到用户空间
发起新的 DNS 查询，随后该查询可能会被缓存。密钥描述按如下形式的字符串构造：)

		[<type>:]<name>

where <type> optionally specifies the particular upcall program to invoke,
and thus the type of query, and <name> specifies the string to be looked up.
The default query type is a straight hostname to IP address set lookup.
(其中 <type> 可选地指定要调用的特定上行调用程序，从而指定查询的类型；<name> 指定
要查找的字符串。默认的查询类型是直接的主机名到 IP 地址集合的查找。)

The name parameter is not required to be a NUL-terminated string, and its
length should be given by the namelen argument.
(name 参数不必是以 NUL 结尾的字符串，其长度应由 namelen 参数给出。)

The options parameter may be NULL or it may be a set of options
appropriate to the query type.
(options 参数可以为 NULL，也可以是一组适合该查询类型的选项。)

The return value is a string appropriate to the query type.  For instance,
for the default query type it is just a list of comma-separated IPv4 and
IPv6 addresses.  The caller must free the result.
(返回值是一个适合该查询类型的字符串。例如，对于默认查询类型，它只是一个以逗号分隔的
IPv4 和 IPv6 地址列表。调用者必须释放该结果。)

The length of the result string is returned on success, and a negative
error code is returned otherwise.  -EKEYREJECTED will be returned if the
DNS lookup failed.
(成功时返回结果字符串的长度，否则返回一个负数错误码。如果 DNS 查找失败，将返回
-EKEYREJECTED。)

If _expiry is non-NULL, the expiry time (TTL) of the result will be
returned also.
(如果 _expiry 非 NULL，还会返回结果的过期时间(TTL)。)

The kernel maintains an internal keyring in which it caches looked up keys.
This can be cleared by any process that has the CAP_SYS_ADMIN capability by
the use of KEYCTL_KEYRING_CLEAR on the keyring ID.
(内核维护一个内部密钥环，在其中缓存查找到的密钥。任何拥有 CAP_SYS_ADMIN 能力的
进程都可以通过对该密钥环 ID 使用 KEYCTL_KEYRING_CLEAR 来清空它。)


Reading DNS Keys from Userspace
(从用户空间读取 DNS 密钥)
===============================

Keys of dns_resolver type can be read from userspace using keyctl_read() or
"keyctl read/print/pipe".
(用户空间可以使用 keyctl_read() 或 "keyctl read/print/pipe" 读取 dns_resolver
类型的密钥。)


Mechanism
(机制)
=========

The dns_resolver module registers a key type called "dns_resolver".  Keys of
this type are used to transport and cache DNS lookup results from userspace.
(dns_resolver 模块注册一个名为 "dns_resolver" 的密钥类型。这种类型的密钥用于传输
和缓存来自用户空间的 DNS 查找结果。)

When dns_query() is invoked, it calls request_key() to search the local
keyrings for a cached DNS result.  If that fails to find one, it upcalls to
userspace to get a new result.
(当 dns_query() 被调用时，它会调用 request_key() 在本地密钥环中搜索缓存的 DNS
结果。如果未能找到，它会上行调用到用户空间以获取新结果。)

Upcalls to userspace are made through the request_key() upcall vector, and are
directed by means of configuration lines in /etc/request-key.conf that tell
/sbin/request-key what program to run to instantiate the key.
(到用户空间的上行调用通过 request_key() 上行调用向量进行，并由 /etc/request-key.conf
中的配置行加以引导，这些配置行告诉 /sbin/request-key 运行哪个程序来实例化密钥。)

The upcall handler program is responsible for querying the DNS, processing the
result into a form suitable for passing to the keyctl_instantiate_key()
routine.  This then passes the data to dns_resolver_instantiate() which strips
off and processes any options included in the data, and then attaches the
remainder of the string to the key as its payload.
(上行调用处理程序负责查询 DNS，并把结果处理成适合传递给 keyctl_instantiate_key()
例程的形式。然后该例程把数据传递给 dns_resolver_instantiate()，后者会剥离并处理
数据中包含的所有选项，再把字符串的其余部分作为载荷附加到密钥上。)

The upcall handler program should set the expiry time on the key to that of the
lowest TTL of all the records it has extracted a result from.  This means that
the key will be discarded and recreated when the data it holds has expired.
(上行调用处理程序应把密钥上的过期时间设置为它从中提取结果的所有记录中最低的 TTL。
这意味着当密钥所持有的数据过期时，该密钥将被丢弃并重新创建。)

dns_query() returns a copy of the value attached to the key, or an error if
that is indicated instead.
(dns_query() 返回附加到密钥上的值的一个副本；如果结果标明出错，则返回错误。)

See Documentation/security/keys/request-key.rst for further information about
request-key function.
(有关 request-key 功能的更多信息，请参阅
Documentation/security/keys/request-key.rst。)


Debugging
(调试)
=========

Debugging messages can be turned on dynamically by writing a 1 into the
following file::
(可以通过向以下文件写入 1 来动态开启调试消息：)

	/sys/module/dns_resolver/parameters/debug
