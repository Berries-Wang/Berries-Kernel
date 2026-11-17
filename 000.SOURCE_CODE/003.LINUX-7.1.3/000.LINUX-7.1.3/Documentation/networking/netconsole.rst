.. SPDX-License-Identifier: GPL-2.0

===================================
Netconsole(网络控制台 (Netconsole))
===================================


started by Ingo Molnar <mingo@redhat.com>, 2001.09.17
(由 Ingo Molnar <mingo@redhat.com> 于 2001.09.17 发起)

2.6 port and netpoll api by Matt Mackall <mpm@selenic.com>, Sep 9 2003
(2.6 移植与 netpoll api 由 Matt Mackall <mpm@selenic.com> 于 2003 年 9 月 9 日完成)

IPv6 support by Cong Wang <xiyou.wangcong@gmail.com>, Jan 1 2013
(IPv6 支持由 Cong Wang <xiyou.wangcong@gmail.com> 于 2013 年 1 月 1 日添加)

Extended console support by Tejun Heo <tj@kernel.org>, May 1 2015
(扩展控制台支持由 Tejun Heo <tj@kernel.org> 于 2015 年 5 月 1 日添加)

Release prepend support by Breno Leitao <leitao@debian.org>, Jul 7 2023
(版本号前缀支持由 Breno Leitao <leitao@debian.org> 于 2023 年 7 月 7 日添加)

Userdata append support by Matthew Wood <thepacketgeek@gmail.com>, Jan 22 2024
(用户数据追加支持由 Matthew Wood <thepacketgeek@gmail.com> 于 2024 年 1 月 22 日添加)

Sysdata append support by Breno Leitao <leitao@debian.org>, Jan 15 2025
(系统数据追加支持由 Breno Leitao <leitao@debian.org> 于 2025 年 1 月 15 日添加)

Introduction:(简介：)
=====================

This module logs kernel printk messages over UDP allowing debugging of
problem where disk logging fails and serial consoles are impractical.
(本模块通过 UDP 记录内核 printk 消息，可用于在磁盘日志失效、串行控制台又
不切实际的情况下调试问题。)

It can be used either built-in or as a module. As a built-in,
netconsole initializes immediately after NIC cards and will bring up
the specified interface as soon as possible. While this doesn't allow
capture of early kernel panics, it does capture most of the boot
process.
(它既可内建使用，也可作为模块使用。作为内建功能时，netconsole 会在网卡
初始化之后立即初始化，并尽快拉起指定的接口。虽然这无法捕获早期的内核
崩溃 (panic)，但可以捕获启动过程的大部分内容。)

Sender and receiver configuration:(发送端与接收端配置：)
========================================================

It takes a string configuration parameter "netconsole" in the
following format::
(它接受一个字符串形式的配置参数 "netconsole"，格式如下：)

 netconsole=[+][r][src-port]@[src-ip]/[<dev>],[tgt-port]@<tgt-ip>/[tgt-macaddr]

   where
	+             if present, enable extended console support
	r             if present, prepend kernel version (release) to the message
	src-port      source for UDP packets (defaults to 6665)
	src-ip        source IP to use (interface address)
	dev           network interface name (eth0) or MAC address
	tgt-port      port for logging agent (6666)
	tgt-ip        IP address for logging agent
	tgt-macaddr   ethernet MAC address for logging agent (broadcast)

Examples::
(示例：)

 linux netconsole=4444@10.0.0.1/eth1,9353@10.0.0.2/12:34:56:78:9a:bc

or::
(或者：)

 insmod netconsole netconsole=@/,@10.0.0.2/

or using IPv6::
(或者使用 IPv6：)

 insmod netconsole netconsole=@/,@fd00:1:2:3::1/

or using a MAC address to select the egress interface::
(或者使用 MAC 地址来选择出口接口：)

   linux netconsole=4444@10.0.0.1/22:33:44:55:66:77,9353@10.0.0.2/12:34:56:78:9a:bc

It also supports logging to multiple remote agents by specifying
parameters for the multiple agents separated by semicolons and the
complete string enclosed in "quotes", thusly::
(它还支持记录到多个远程代理：用分号分隔多个代理各自的参数，并将整个字符串
用 "引号" 括起来，如下所示：)

 modprobe netconsole netconsole="@/,@10.0.0.2/;@/eth1,6892@10.0.0.3/"

Built-in netconsole starts immediately after the TCP stack is
initialized and attempts to bring up the supplied dev at the supplied
address.
(内建的 netconsole 在 TCP 协议栈初始化完成后立即启动，并尝试在提供的地址上
拉起所提供的设备。)

The remote host has several options to receive the kernel messages,
for example:
(远程主机有几种接收内核消息的方式，例如：)

1) syslogd
   (1) syslogd

2) netcat
   (2) netcat

   On distributions using a BSD-based netcat version (e.g. Fedora,
   openSUSE and Ubuntu) the listening port must be specified without
   the -p switch::
   (在使用基于 BSD 的 netcat 版本的发行版 (例如 Fedora、openSUSE 和
   Ubuntu) 上，监听端口必须不带 -p 开关指定：)

	nc -u -l -p <port>' / 'nc -u -l <port>

   or::
   (或者：)

	netcat -u -l -p <port>' / 'netcat -u -l <port>

3) socat
   (3) socat

::

   socat udp-recv:<port> -

Dynamic reconfiguration:(动态重配置：)
======================================

Dynamic reconfigurability is a useful addition to netconsole that enables
remote logging targets to be dynamically added, removed, or have their
parameters reconfigured at runtime from a configfs-based userspace interface.
(动态可重配置性是 netconsole 的一个实用增强，它使远程日志目标可以在运行时
通过基于 configfs 的用户空间接口动态添加、删除或重新配置参数。)

To include this feature, select CONFIG_NETCONSOLE_DYNAMIC when building the
netconsole module (or kernel, if netconsole is built-in).
(要包含此特性，在构建 netconsole 模块 (如果是内建的则是内核) 时选择
CONFIG_NETCONSOLE_DYNAMIC。)

Some examples follow (where configfs is mounted at the /sys/kernel/config
mountpoint).
(下面给出一些示例 (假设 configfs 挂载在 /sys/kernel/config 挂载点)。)

To add a remote logging target (target names can be arbitrary)::
(要添加一个远程日志目标 (目标名可以是任意名称)：)

 cd /sys/kernel/config/netconsole/
 mkdir target1

Note that newly created targets have default parameter values (as mentioned
above) and are disabled by default -- they must first be enabled by writing
"1" to the "enabled" attribute (usually after setting parameters accordingly)
as described below.
(注意，新建的目标具有默认参数值 (如上所述)，且默认处于禁用状态 --
必须先向 "enabled" 属性写入 "1" 来启用它们 (通常在相应地设置好参数之后)，
如下所述。)

To remove a target::
(要删除一个目标：)

 rmdir /sys/kernel/config/netconsole/othertarget/

The interface exposes these parameters of a netconsole target to userspace:
(该接口向用户空间暴露 netconsole 目标的以下参数：)

	=============== =================================       ============
	enabled		Is this target currently enabled?	(read-write)
	                (该目标当前是否已启用？)
	extended	Extended mode enabled			(read-write)
	                (是否启用扩展模式)
	release		Prepend kernel release to message	(read-write)
	                (在消息前加上内核版本号)
	dev_name	Local network interface name		(read-write)
	                (本地网络接口名)
	local_port	Source UDP port to use			(read-write)
	                (要使用的源 UDP 端口)
	remote_port	Remote agent's UDP port			(read-write)
	                (远程代理的 UDP 端口)
	local_ip	Source IP address to use		(read-write)
	                (要使用的源 IP 地址)
	remote_ip	Remote agent's IP address		(read-write)
	                (远程代理的 IP 地址)
	local_mac	Local interface's MAC address		(read-only)
	                (本地接口的 MAC 地址)
	remote_mac	Remote agent's MAC address		(read-write)
	                (远程代理的 MAC 地址)
	transmit_errors	Number of packet send errors		(read-only)
	                (数据包发送错误的次数)
	=============== =================================       ============

The "enabled" attribute is also used to control whether the parameters of
a target can be updated or not -- you can modify the parameters of only
disabled targets (i.e. if "enabled" is 0).
("enabled" 属性还用于控制目标的参数是否可以更新 -- 你只能修改处于禁用状态
的目标的参数 (即 "enabled" 为 0 时)。)

To update a target's parameters::
(要更新目标的参数：)

 cat enabled				# check if enabled is 1
 echo 0 > enabled			# disable the target (if required)
 echo eth2 > dev_name			# set local interface
 echo 10.0.0.4 > remote_ip		# update some parameter
 echo cb:a9:87:65:43:21 > remote_mac	# update more parameters
 echo 1 > enabled			# enable target again

You can also update the local interface dynamically. This is especially
useful if you want to use interfaces that have newly come up (and may not
have existed when netconsole was loaded / initialized).
(你还可以动态更新本地接口。如果你想使用新出现的接口 (在 netconsole 被加载
/ 初始化时可能还不存在)，这一点尤其有用。)

Netconsole targets defined at boot time (or module load time) with the
`netconsole=` param are assigned the name `cmdline<index>`.  For example, the
first target in the parameter is named `cmdline0`.  You can control and modify
these targets by creating configfs directories with the matching name.
(通过 `netconsole=` 参数在启动时 (或模块加载时) 定义的 netconsole 目标会被
命名为 `cmdline<index>`。例如，参数中的第一个目标被命名为 `cmdline0`。
你可以通过创建同名的 configfs 目录来控制和修改这些目标。)

Let's suppose you have two netconsole targets defined at boot time::
(假设你在启动时定义了两个 netconsole 目标：)

 netconsole=4444@10.0.0.1/eth1,9353@10.0.0.2/12:34:56:78:9a:bc;4444@10.0.0.1/eth1,9353@10.0.0.3/12:34:56:78:9a:bc

You can modify these targets in runtime by creating the following targets::
(你可以通过创建以下目标，在运行时修改这些目标：)

 mkdir cmdline0
 cat cmdline0/remote_ip
 10.0.0.2

 mkdir cmdline1
 cat cmdline1/remote_ip
 10.0.0.3

Append User Data(追加用户数据)
------------------------------

Custom user data can be appended to the end of messages with netconsole
dynamic configuration enabled. User data entries can be modified without
changing the "enabled" attribute of a target.
(在启用 netconsole 动态配置的情况下，可以将自定义用户数据追加到消息末尾。
用户数据条目可以在不改变目标 "enabled" 属性的情况下进行修改。)

Directories (keys) under `userdata` are limited to 53 character length, and
data in `userdata/<key>/value` are limited to 200 bytes::
(`userdata` 下的目录 (键) 长度限制为 53 个字符，`userdata/<key>/value` 中的
数据限制为 200 字节：)

 cd /sys/kernel/config/netconsole && mkdir cmdline0
 cd cmdline0
 mkdir userdata/foo
 echo bar > userdata/foo/value
 mkdir userdata/qux
 echo baz > userdata/qux/value

Messages will now include this additional user data::
(现在，消息将包含这些额外的用户数据：)

 echo "This is a message" > /dev/kmsg

Sends::
(发送出：)

 12,607,22085407756,-;This is a message
  foo=bar
  qux=baz

Preview the userdata that will be appended with::
(预览将要追加的用户数据可以使用：)

 cd /sys/kernel/config/netconsole/cmdline0/userdata
 for f in `ls userdata`; do echo $f=$(cat userdata/$f/value); done

If a `userdata` entry is created but no data is written to the `value` file,
the entry will be omitted from netconsole messages::
(如果创建了一个 `userdata` 条目但没有向 `value` 文件写入数据，netconsole
消息中将省略该条目：)

 cd /sys/kernel/config/netconsole && mkdir cmdline0
 cd cmdline0
 mkdir userdata/foo
 echo bar > userdata/foo/value
 mkdir userdata/qux

The `qux` key is omitted since it has no value::
(`qux` 键因为没有值而被省略：)

 echo "This is a message" > /dev/kmsg
 12,607,22085407756,-;This is a message
  foo=bar

Delete `userdata` entries with `rmdir`::
(使用 `rmdir` 删除 `userdata` 条目：)

 rmdir /sys/kernel/config/netconsole/cmdline0/userdata/qux

.. warning::
   When writing strings to user data values, input is broken up per line in
   configfs store calls and this can cause confusing behavior::
   (警告：向用户数据值写入字符串时，输入会在 configfs store 调用中按行拆分，
   这可能导致令人困惑的行为：)

     mkdir userdata/testing
     printf "val1\nval2" > userdata/testing/value
     # userdata store value is called twice, first with "val1\n" then "val2"
     # so "val2" is stored, being the last value stored
     cat userdata/testing/value
     val2

   It is recommended to not write user data values with newlines.
   (建议不要写入带有换行符的用户数据值。)

Task name auto population in userdata(在 userdata 中自动填充任务名)
-------------------------------------------------------------------

Inside the netconsole configfs hierarchy, there is a file called
`taskname_enabled` under the `userdata` directory. This file is used to enable
or disable the automatic task name population feature. This feature
automatically populates the current task name that is scheduled in the CPU
sneding the message.
(在 netconsole 的 configfs 层次结构中，`userdata` 目录下有一个名为
`taskname_enabled` 的文件。该文件用于启用或禁用自动填充任务名特性。该特性
会自动填充在发送该消息的 CPU 上调度的当前任务名。)

To enable task name auto-population::
(要启用任务名自动填充：)

  echo 1 > /sys/kernel/config/netconsole/target1/userdata/taskname_enabled

When this option is enabled, the netconsole messages will include an additional
line in the userdata field with the format `taskname=<task name>`. This allows
the receiver of the netconsole messages to easily find which application was
currently scheduled when that message was generated, providing extra context
for kernel messages and helping to categorize them.
(启用该选项后，netconsole 消息将在 userdata 字段中包含一行额外内容，格式为
`taskname=<task name>`。这样，netconsole 消息的接收方可以方便地找到该消息
生成时当前调度的是哪个应用程序，为内核消息提供额外上下文，并有助于对其进行
归类。)

Example::
(示例：)

  echo "This is a message" > /dev/kmsg
  12,607,22085407756,-;This is a message
   taskname=echo

In this example, the message was generated while "echo" was the current
scheduled process.
(在这个例子中，消息生成时当前调度的进程是 "echo"。)

Kernel release auto population in userdata(在 userdata 中自动填充内核版本号)
----------------------------------------------------------------------------

Within the netconsole configfs hierarchy, there is a file named `release_enabled`
located in the `userdata` directory. This file controls the kernel release
(version) auto-population feature, which appends the kernel release information
to userdata dictionary in every message sent.
(在 netconsole 的 configfs 层次结构中，`userdata` 目录下有一个名为
`release_enabled` 的文件。该文件控制内核版本号 (release) 自动填充特性，
它会在每条发送的消息中把内核版本信息追加到 userdata 字典。)

To enable the release auto-population::
(要启用版本号自动填充：)

  echo 1 > /sys/kernel/config/netconsole/target1/userdata/release_enabled

Example::
(示例：)

  echo "This is a message" > /dev/kmsg
  12,607,22085407756,-;This is a message
   release=6.14.0-rc6-01219-g3c027fbd941d

.. note::

   This feature provides the same data as the "release prepend" feature.
   However, in this case, the release information is appended to the userdata
   dictionary rather than being included in the message header.
   (注意：该特性提供的数据与 "release prepend" 特性相同。不过在这种情况下，
   版本信息是追加到 userdata 字典中，而不是包含在消息头中。)


CPU number auto population in userdata(在 userdata 中自动填充 CPU 编号)
-----------------------------------------------------------------------

Inside the netconsole configfs hierarchy, there is a file called
`cpu_nr` under the `userdata` directory. This file is used to enable or disable
the automatic CPU number population feature. This feature automatically
populates the CPU number that is sending the message.
(在 netconsole 的 configfs 层次结构中，`userdata` 目录下有一个名为
`cpu_nr` 的文件。该文件用于启用或禁用自动填充 CPU 编号特性。该特性会自动
填充发送该消息的 CPU 编号。)

To enable the CPU number auto-population::
(要启用 CPU 编号自动填充：)

  echo 1 > /sys/kernel/config/netconsole/target1/userdata/cpu_nr

When this option is enabled, the netconsole messages will include an additional
line in the userdata field with the format `cpu=<cpu_number>`. This allows the
receiver of the netconsole messages to easily differentiate and demultiplex
messages originating from different CPUs, which is particularly useful when
dealing with parallel log output.
(启用该选项后，netconsole 消息将在 userdata 字段中包含一行额外内容，格式为
`cpu=<cpu_number>`。这样，netconsole 消息的接收方可以轻松区分来自不同 CPU
的消息并将其分流，这在处理并行的日志输出时特别有用。)

Example::
(示例：)

  echo "This is a message" > /dev/kmsg
  12,607,22085407756,-;This is a message
   cpu=42

In this example, the message was sent by CPU 42.
(在这个例子中，消息由 CPU 42 发送。)

.. note::

   If the user has set a conflicting `cpu` key in the userdata dictionary,
   both keys will be reported, with the kernel-populated entry appearing after
   the user one. For example::
   (注意：如果用户在 userdata 字典中设置了一个冲突的 `cpu` 键，两个键都会被
   报告，内核填充的条目会出现在用户条目之后。例如：)

     # User-defined CPU entry
     mkdir -p /sys/kernel/config/netconsole/target1/userdata/cpu
     echo "1" > /sys/kernel/config/netconsole/target1/userdata/cpu/value

   Output might look like::
   (输出可能类似于：)

     12,607,22085407756,-;This is a message
      cpu=1
      cpu=42    # kernel-populated value


Message ID auto population in userdata(在 userdata 中自动填充消息 ID)
---------------------------------------------------------------------

Within the netconsole configfs hierarchy, there is a file named `msgid_enabled`
located in the `userdata` directory. This file controls the message ID
auto-population feature, which assigns a numeric id to each message sent to a
given target and appends the ID to userdata dictionary in every message sent.
(在 netconsole 的 configfs 层次结构中，`userdata` 目录下有一个名为
`msgid_enabled` 的文件。该文件控制消息 ID 自动填充特性，它为发送到给定目标
的每条消息分配一个数字 ID，并在每条发送的消息中把该 ID 追加到 userdata
字典。)

The message ID is generated using a per-target 32 bit counter that is
incremented for every message sent to the target. Note that this counter will
eventually wrap around after reaching uint32_t max value, so the message ID is
not globally unique over time. However, it can still be used by the target to
detect if messages were dropped before reaching the target by identifying gaps
in the sequence of IDs.
(消息 ID 使用一个每目标 32 位计数器生成，每发送一条到该目标的消息计数器就
递增一次。注意，该计数器在达到 uint32_t 最大值后最终会回绕，因此消息 ID
在时间上并非全局唯一。不过，目标仍然可以通过识别 ID 序列中的空缺，用它来
检测消息是否在到达目标之前被丢弃。)

It is important to distinguish message IDs from the message <sequnum> field.
Some kernel messages may never reach netconsole (for example, due to printk
rate limiting). Thus, a gap in <sequnum> cannot be solely relied upon to
indicate that a message was dropped during transmission, as it may never have
been sent via netconsole. The message ID, on the other hand, is only assigned
to messages that are actually transmitted via netconsole.
(重要的是要区分消息 ID 与消息的 <sequnum> 字段。某些内核消息可能永远到不了
netconsole (例如由于 printk 速率限制)。因此，<sequnum> 中的空缺不能单独作为
消息在传输过程中被丢弃的依据，因为它可能根本没有通过 netconsole 发送。而
消息 ID 只会分配给真正通过 netconsole 传输的消息。)

Example::
(示例：)

  echo "This is message #1" > /dev/kmsg
  echo "This is message #2" > /dev/kmsg
  13,434,54928466,-;This is message #1
   msgid=1
  13,435,54934019,-;This is message #2
   msgid=2


Extended console:(扩展控制台：)
===============================

If '+' is prefixed to the configuration line or "extended" config file
is set to 1, extended console support is enabled. An example boot
param follows::
(如果在配置行前加上 '+'，或将 "extended" 配置文件设为 1，就会启用扩展
控制台支持。启动参数示例如下：)

 linux netconsole=+4444@10.0.0.1/eth1,9353@10.0.0.2/12:34:56:78:9a:bc

Log messages are transmitted with extended metadata header in the
following format which is the same as /dev/kmsg::
(日志消息传输时带有扩展元数据头，格式与 /dev/kmsg 相同，如下所示：)

 <level>,<sequnum>,<timestamp>,<contflag>;<message text>

If 'r' (release) feature is enabled, the kernel release version is
prepended to the start of the message. Example::
(如果启用了 'r' (release) 特性，内核版本号会被加在消息的开头。示例：)

 6.4.0,6,444,501151268,-;netconsole: network logging started

Non printable characters in <message text> are escaped using "\xff"
notation. If the message contains optional dictionary, verbatim
newline is used as the delimiter.
(<message text> 中的不可打印字符使用 "\xff" 记法转义。如果消息包含可选的
字典 (dictionary)，则使用原样的换行符作为分隔符。)

If a message doesn't fit in certain number of bytes (currently 1000),
the message is split into multiple fragments by netconsole. These
fragments are transmitted with "ncfrag" header field added::
(如果消息超过一定的字节数 (当前为 1000)，netconsole 会将其拆分为多个分片。
这些分片在传输时带有附加的 "ncfrag" 头字段：)

 ncfrag=<byte-offset>/<total-bytes>

For example, assuming a lot smaller chunk size, a message "the first
chunk, the 2nd chunk." may be split as follows::
(例如，假设分块大小要小得多，一条消息 "the first chunk, the 2nd chunk."
可能会被拆分如下：)

 6,416,1758426,-,ncfrag=0/31;the first chunk,
 6,416,1758426,-,ncfrag=16/31; the 2nd chunk.

Miscellaneous notes:(杂项说明：)
================================

.. Warning::

   the default target ethernet setting uses the broadcast
   ethernet address to send packets, which can cause increased load on
   other systems on the same ethernet segment.
   (警告：目标默认的以太网设置使用广播以太网地址发送数据包，这可能导致
   同一以太网段上其他系统的负载增加。)

.. Tip::

   some LAN switches may be configured to suppress ethernet broadcasts
   so it is advised to explicitly specify the remote agents' MAC addresses
   from the config parameters passed to netconsole.
   (提示：某些局域网交换机可能被配置为抑制以太网广播，因此建议在传给
   netconsole 的配置参数中显式指定远程代理的 MAC 地址。)

.. Tip::

   to find out the MAC address of, say, 10.0.0.2, you may try using::
   (提示：要查明某个地址 (例如 10.0.0.2) 的 MAC 地址，可以尝试使用：)

	ping -c 1 10.0.0.2 ; /sbin/arp -n | grep 10.0.0.2

.. Tip::

   in case the remote logging agent is on a separate LAN subnet than
   the sender, it is suggested to try specifying the MAC address of the
   default gateway (you may use /sbin/route -n to find it out) as the
   remote MAC address instead.
   (提示：如果远程日志代理与发送端不在同一局域网子网内，建议改为尝试指定
   默认网关的 MAC 地址 (可以用 /sbin/route -n 查到) 作为远程 MAC 地址。)

.. note::

   the network device (eth1 in the above case) can run any kind
   of other network traffic, netconsole is not intrusive. Netconsole
   might cause slight delays in other traffic if the volume of kernel
   messages is high, but should have no other impact.
   (注意：该网络设备 (上例中的 eth1) 可以承载任何其他类型的网络流量，
   netconsole 不具侵入性。如果内核消息量很大，netconsole 可能导致其他流量
   出现轻微延迟，但不应该有其他影响。)

.. note::

   if you find that the remote logging agent is not receiving or
   printing all messages from the sender, it is likely that you have set
   the "console_loglevel" parameter (on the sender) to only send high
   priority messages to the console. You can change this at runtime using::
   (注意：如果你发现远程日志代理没有收到或没有打印出发送端的全部消息，
   很可能是你把 "console_loglevel" 参数 (在发送端上) 设置为只向控制台发送
   高优先级消息。你可以在运行时使用以下命令更改：)

	dmesg -n 8

   or by specifying "debug" on the kernel command line at boot, to send
   all kernel messages to the console. A specific value for this parameter
   can also be set using the "loglevel" kernel boot option. See the
   dmesg(8) man page and Documentation/admin-guide/kernel-parameters.rst
   for details.
   (或者在启动时在内核命令行上指定 "debug"，以把所有内核消息发送到控制台。
   也可以使用 "loglevel" 内核启动选项为该参数设置特定值。详见 dmesg(8)
   手册页和 Documentation/admin-guide/kernel-parameters.rst。)

Netconsole was designed to be as instantaneous as possible, to
enable the logging of even the most critical kernel bugs. It works
from IRQ contexts as well, and does not enable interrupts while
sending packets. Due to these unique needs, configuration cannot
be more automatic, and some fundamental limitations will remain:
only IP networks, UDP packets and ethernet devices are supported.
(Netconsole 的设计目标是尽可能即时地记录哪怕是最严重的内核 bug。它也可以
在 IRQ 上下文中工作，并且在发送数据包时不会启用中断。由于这些独特的需求，
配置无法做到更加自动化，并且会保留一些根本性的限制：只支持 IP 网络、UDP
数据包和以太网设备。)
