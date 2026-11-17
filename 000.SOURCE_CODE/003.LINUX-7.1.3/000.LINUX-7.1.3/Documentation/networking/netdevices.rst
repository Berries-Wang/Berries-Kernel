.. SPDX-License-Identifier: GPL-2.0

=====================================
Network Devices, the Kernel, and You!
=====================================

(网络设备、内核与你！)


Introduction
(简介)
============

The following is a random collection of documentation regarding
network devices. It is intended for driver developers.
(以下是关于网络设备的一些零散文档，面向驱动开发者。)

struct net_device lifetime rules
(struct net_device 生命周期规则)
================================

Network device structures need to persist even after module is unloaded and
must be allocated with alloc_netdev_mqs() and friends.
If device has registered successfully, it will be freed on last use
by free_netdev(). This is required to handle the pathological case cleanly
(example: ``rmmod mydriver </sys/class/net/myeth/mtu``)
(网络设备结构必须在模块卸载之后仍然存续，并且必须用 alloc_netdev_mqs() 及其
同类函数分配。如果设备注册成功，它将在最后一次使用后被 free_netdev() 释放。
这是为了干净地处理病态情况（例如：
``rmmod mydriver </sys/class/net/myeth/mtu`` ）所必需的。)

alloc_netdev_mqs() / alloc_netdev() reserve extra space for driver
private data which gets freed when the network device is freed. If
separately allocated data is attached to the network device
(netdev_priv()) then it is up to the module exit handler to free that.
(alloc_netdev_mqs() / alloc_netdev() 会为驱动私有数据预留额外空间，这些空间
在网络设备被释放时一并释放。如果单独分配的数据被挂接到网络设备上
（netdev_priv()），那么释放它就是模块退出处理程序的责任。)

There are two groups of APIs for registering struct net_device.
First group can be used in normal contexts where ``rtnl_lock`` is not already
held: register_netdev(), unregister_netdev().
Second group can be used when ``rtnl_lock`` is already held:
register_netdevice(), unregister_netdevice(), free_netdevice().
(注册 struct net_device 的 API 有两组。第一组可在尚未持有 ``rtnl_lock`` 的
普通上下文中使用：register_netdev()、unregister_netdev()。第二组可在已持有
``rtnl_lock`` 时使用：register_netdevice()、unregister_netdevice()、
free_netdevice()。)

Simple drivers
(简单驱动)
--------------

Most drivers (especially device drivers) handle lifetime of struct net_device
in context where ``rtnl_lock`` is not held (e.g. driver probe and remove paths).
(大多数驱动（尤其是设备驱动）在未持有 ``rtnl_lock`` 的上下文中处理 struct
net_device 的生命周期（例如驱动的 probe 与 remove 路径）。)

In that case the struct net_device registration is done using
the register_netdev(), and unregister_netdev() functions:
(在这种情况下，struct net_device 的注册通过 register_netdev() 和
unregister_netdev() 函数完成：)

.. code-block:: c

  int probe()
  {
    struct my_device_priv *priv;
    int err;

    dev = alloc_netdev_mqs(...);
    if (!dev)
      return -ENOMEM;
    priv = netdev_priv(dev);

    /* ... do all device setup before calling register_netdev() ...
     */

    err = register_netdev(dev);
    if (err)
      goto err_undo;

    /* net_device is visible to the user! */

  err_undo:
    /* ... undo the device setup ... */
    free_netdev(dev);
    return err;
  }

  void remove()
  {
    unregister_netdev(dev);
    free_netdev(dev);
  }

Note that after calling register_netdev() the device is visible in the system.
Users can open it and start sending / receiving traffic immediately,
or run any other callback, so all initialization must be done prior to
registration.
(注意，调用 register_netdev() 之后设备就在系统中可见了。用户可以立即打开它并
开始收发流量，或运行任何其他回调，因此所有初始化都必须在注册之前完成。)

unregister_netdev() closes the device and waits for all users to be done
with it. The memory of struct net_device itself may still be referenced
by sysfs but all operations on that device will fail.
(unregister_netdev() 会关闭设备并等待所有用户用完它。struct net_device 本身的
内存可能仍被 sysfs 引用，但对该设备的所有操作都会失败。)

free_netdev() can be called after unregister_netdev() returns or when
register_netdev() failed.
(free_netdev() 可以在 unregister_netdev() 返回之后调用，或在
register_netdev() 失败时调用。)

Device management under RTNL
(RTNL 下的设备管理)
----------------------------

Registering struct net_device while in context which already holds
the ``rtnl_lock`` requires extra care. In those scenarios most drivers
will want to make use of struct net_device's ``needs_free_netdev``
and ``priv_destructor`` members for freeing of state.
(在已持有 ``rtnl_lock`` 的上下文中注册 struct net_device 需要格外小心。在这类
场景下，大多数驱动会希望利用 struct net_device 的 ``needs_free_netdev`` 和
``priv_destructor`` 成员来释放状态。)

Example flow of netdev handling under ``rtnl_lock``:
(``rtnl_lock`` 下 netdev 处理流程示例：)

.. code-block:: c

  static void my_setup(struct net_device *dev)
  {
    dev->needs_free_netdev = true;
  }

  static void my_destructor(struct net_device *dev)
  {
    some_obj_destroy(priv->obj);
    some_uninit(priv);
  }

  int create_link()
  {
    struct my_device_priv *priv;
    int err;

    ASSERT_RTNL();

    dev = alloc_netdev(sizeof(*priv), "net%d", NET_NAME_UNKNOWN, my_setup);
    if (!dev)
      return -ENOMEM;
    priv = netdev_priv(dev);

    /* Implicit constructor */
    err = some_init(priv);
    if (err)
      goto err_free_dev;

    priv->obj = some_obj_create();
    if (!priv->obj) {
      err = -ENOMEM;
      goto err_some_uninit;
    }
    /* End of constructor, set the destructor: */
    dev->priv_destructor = my_destructor;

    err = register_netdevice(dev);
    if (err)
      /* register_netdevice() calls destructor on failure */
      goto err_free_dev;

    /* If anything fails now unregister_netdevice() (or unregister_netdev())
     * will take care of calling my_destructor and free_netdev().
     */

    return 0;

  err_some_uninit:
    some_uninit(priv);
  err_free_dev:
    free_netdev(dev);
    return err;
  }

If struct net_device.priv_destructor is set it will be called by the core
some time after unregister_netdevice(), it will also be called if
register_netdevice() fails. The callback may be invoked with or without
``rtnl_lock`` held.
(如果设置了 struct net_device.priv_destructor，核心会在 unregister_netdevice()
之后的某个时刻调用它；register_netdevice() 失败时也会调用它。该回调可能在
持有或不持有 ``rtnl_lock`` 的情况下被调用。)

There is no explicit constructor callback, driver "constructs" the private
netdev state after allocating it and before registration.
(没有显式的构造函数回调；驱动在分配私有 netdev 状态之后、注册之前"构造"它。)

Setting struct net_device.needs_free_netdev makes core call free_netdevice()
automatically after unregister_netdevice() when all references to the device
are gone. It only takes effect after a successful call to register_netdevice()
so if register_netdevice() fails driver is responsible for calling
free_netdev().
(设置 struct net_device.needs_free_netdev 会让核心在 unregister_netdevice()
之后、当设备的所有引用都消失时自动调用 free_netdevice()。它只在
register_netdevice() 成功调用之后才生效，因此如果 register_netdevice() 失败，
驱动要负责调用 free_netdev()。)

free_netdev() is safe to call on error paths right after unregister_netdevice()
or when register_netdevice() fails. Parts of netdev (de)registration process
happen after ``rtnl_lock`` is released, therefore in those cases free_netdev()
will defer some of the processing until ``rtnl_lock`` is released.
(在错误路径上，可以在 unregister_netdevice() 之后立即安全地调用
free_netdev()，或在 register_netdevice() 失败时调用。netdev 注册/注销过程的
一部分发生在 ``rtnl_lock`` 释放之后，因此在这些情况下 free_netdev() 会把部分
处理推迟到 ``rtnl_lock`` 释放之后。)

Devices spawned from struct rtnl_link_ops should never free the
struct net_device directly.
(由 struct rtnl_link_ops 派生的设备绝不应直接释放 struct net_device。)

.ndo_init and .ndo_uninit
(.ndo_init 与 .ndo_uninit)
~~~~~~~~~~~~~~~~~~~~~~~~~

``.ndo_init`` and ``.ndo_uninit`` callbacks are called during net_device
registration and de-registration, under ``rtnl_lock``. Drivers can use
those e.g. when parts of their init process need to run under ``rtnl_lock``.
(``.ndo_init`` 和 ``.ndo_uninit`` 回调在 net_device 注册与注销期间、于
``rtnl_lock`` 下被调用。当驱动的初始化过程有部分需要在 ``rtnl_lock`` 下运行
时，可以使用它们。)

``.ndo_init`` runs before device is visible in the system, ``.ndo_uninit``
runs during de-registering after device is closed but other subsystems
may still have outstanding references to the netdevice.
(``.ndo_init`` 在设备于系统中可见之前运行；``.ndo_uninit`` 在注销期间、设备
关闭之后运行，但其他子系统可能仍持有对该 netdevice 的未决引用。)

MTU
(MTU)
===

Each network device has a Maximum Transfer Unit. The MTU does not
include any link layer protocol overhead. Upper layer protocols must
not pass a socket buffer (skb) to a device to transmit with more data
than the mtu. The MTU does not include link layer header overhead, so
for example on Ethernet if the standard MTU is 1500 bytes used, the
actual skb will contain up to 1514 bytes because of the Ethernet
header. Devices should allow for the 4 byte VLAN header as well.
(每个网络设备都有一个最大传输单元（MTU）。MTU 不包含任何链路层协议开销。
上层协议传给设备发送的套接字缓冲区（skb），其数据量不得超过 MTU。MTU 不包含
链路层头部开销，例如在以太网上若使用标准 MTU 1500 字节，由于以太网头的存在，
实际 skb 最多可含 1514 字节。设备还应为 4 字节的 VLAN 头留出余量。)

Segmentation Offload (GSO, TSO) is an exception to this rule.  The
upper layer protocol may pass a large socket buffer to the device
transmit routine, and the device will break that up into separate
packets based on the current MTU.
(分段卸载（GSO、TSO）是这条规则的一个例外。上层协议可以把大的套接字缓冲区
交给设备发送例程，设备会按当前 MTU 把它拆分成多个数据包。)

MTU is symmetrical and applies both to receive and transmit. A device
must be able to receive at least the maximum size packet allowed by
the MTU. A network device may use the MTU as mechanism to size receive
buffers, but the device should allow packets with VLAN header. With
standard Ethernet mtu of 1500 bytes, the device should allow up to
1518 byte packets (1500 + 14 header + 4 tag).  The device may either:
drop, truncate, or pass up oversize packets, but dropping oversize
packets is preferred.
(MTU 是对称的，同时适用于接收与发送。设备必须能够接收至少 MTU 所允许的最大
尺寸的数据包。网络设备可以把 MTU 用作设定接收缓冲区大小的机制，但设备应当
允许带 VLAN 头的数据包。对于标准以太网 MTU 1500 字节，设备应允许最多 1518
字节的数据包（1500 + 14 头部 + 4 标签）。设备可以丢弃、截断或上传超长数据
包，但首选丢弃超长数据包。)


struct net_device synchronization rules
(struct net_device 同步规则)
=======================================

ndo_open:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	Context: process
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程。)

ndo_stop:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	Context: process
	Note: netif_running() is guaranteed false
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程。注意：保证 netif_running() 为 false。)

ndo_do_ioctl:
	Synchronization: rtnl_lock() semaphore.
	(同步：rtnl_lock() 信号量。)

	This is only called by network subsystems internally,
	not by user space calling ioctl as it was in before
	linux-5.14.
	(它只由网络子系统在内部调用，而不像 linux-5.14 之前那样由用户空间通过
	ioctl 调用。)

ndo_siocbond:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
        Context: process
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程。)

	Used by the bonding driver for the SIOCBOND family of
	ioctl commands.
	(由 bonding 驱动用于 SIOCBOND 系列 ioctl 命令。)

ndo_siocwandev:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	Context: process
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程。)

	Used by the drivers/net/wan framework to handle
	the SIOCWANDEV ioctl with the if_settings structure.
	(由 drivers/net/wan 框架用于配合 if_settings 结构处理 SIOCWANDEV ioctl。)

ndo_siocdevprivate:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	Context: process
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程。)

	This is used to implement SIOCDEVPRIVATE ioctl helpers.
	These should not be added to new drivers, so don't use.
	(它用于实现 SIOCDEVPRIVATE ioctl 辅助函数。不应在新驱动中添加，请不要
	使用。)

ndo_eth_ioctl:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	Context: process
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程。)

ndo_get_stats:
	Synchronization: RCU (can be called concurrently with the stats
	update path).
	Context: atomic (can't sleep under RCU)
	(同步：RCU（可与统计信息更新路径并发调用）。上下文：原子上下文（在 RCU
	下不能睡眠）。)

ndo_start_xmit:
	Synchronization: __netif_tx_lock spinlock.
	(同步：__netif_tx_lock 自旋锁。)

	When the driver sets dev->lltx this will be
	called without holding netif_tx_lock. In this case the driver
	has to lock by itself when needed.
	The locking there should also properly protect against
	set_rx_mode. WARNING: use of dev->lltx is deprecated.
	Don't use it for new drivers.
	(当驱动设置 dev->lltx 时，此回调会在不持有 netif_tx_lock 的情况下被调用。
	这种情况下，驱动需要在需要时自行加锁。该加锁还应恰当地防范 set_rx_mode。
	警告：dev->lltx 的使用已废弃，请不要在新驱动中使用。)

	Context: Process with BHs disabled or BH (timer),
		 will be called with interrupts disabled by netconsole.

	(上下文：禁用 BH 的进程上下文或 BH（定时器）；由 netconsole 调用时中断
	已被禁用。)

	Return codes:
	(返回码：)

	* NETDEV_TX_OK everything ok.
	  (NETDEV_TX_OK：一切正常。)
	* NETDEV_TX_BUSY Cannot transmit packet, try later
	  Usually a bug, means queue start/stop flow control is broken in
	  the driver. Note: the driver must NOT put the skb in its DMA ring.
	  (NETDEV_TX_BUSY：无法发送数据包，请稍后再试。这通常是 bug，意味着驱动
	  中的队列启停流控已损坏。注意：驱动绝不能把 skb 放入其 DMA 环。)

ndo_tx_timeout:
	Synchronization: netif_tx_lock spinlock; all TX queues frozen.
	Context: BHs disabled
	Notes: netif_queue_stopped() is guaranteed true
	(同步：netif_tx_lock 自旋锁；所有 TX 队列被冻结。上下文：禁用 BH。注意：
	保证 netif_queue_stopped() 为 true。)

ndo_set_rx_mode:
	Synchronization: netif_addr_lock spinlock.
	Context: BHs disabled
	Notes: Deprecated in favor of ndo_set_rx_mode_async which runs
	in process context.
	(同步：netif_addr_lock 自旋锁。上下文：禁用 BH。注意：已废弃，建议改用
	在进程上下文中运行的 ndo_set_rx_mode_async。)

ndo_set_rx_mode_async:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	Context: process (from a work queue)
	Notes: Async version of ndo_set_rx_mode which runs in process
	context. Receives snapshots of the unicast and multicast address lists.
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。上下文：进程（来自工作队列）。注意：ndo_set_rx_mode
	的异步版本，在进程上下文中运行。接收单播与多播地址列表的快照。)

ndo_change_rx_flags:
	Synchronization: rtnl_lock() semaphore. In addition, netdev instance
	lock if the driver implements queue management or shaper API.
	(同步：rtnl_lock() 信号量。此外，如果驱动程序实现了队列管理或整形器 API，
	还需 netdev 实例锁。)

ndo_setup_tc:
	``TC_SETUP_BLOCK`` and ``TC_SETUP_FT`` are running under NFT locks
	(i.e. no ``rtnl_lock`` and no device instance lock). The rest of
	``tc_setup_type`` types run under netdev instance lock if the driver
	implements queue management or shaper API.
	(``TC_SETUP_BLOCK`` 和 ``TC_SETUP_FT`` 在 NFT 锁下运行（即不持有
	``rtnl_lock``，也不持有设备实例锁）。其余 ``tc_setup_type`` 类型在驱动
	实现队列管理或整形器 API 时于 netdev 实例锁下运行。)

Most ndo callbacks not specified in the list above are running
under ``rtnl_lock``. In addition, netdev instance lock is taken as well if
the driver implements queue management or shaper API.
(上面列表未列出的大多数 ndo 回调都在 ``rtnl_lock`` 下运行。此外，如果驱动
实现了队列管理或整形器 API，还会同时获取 netdev 实例锁。)

struct napi_struct synchronization rules
(struct napi_struct 同步规则)
========================================

napi->poll:
	Synchronization:
	(同步：)

		NAPI_STATE_SCHED bit in napi->state.  Device
		driver's ndo_stop method will invoke napi_disable() on
		all NAPI instances which will do a sleeping poll on the
		NAPI_STATE_SCHED napi->state bit, waiting for all pending
		NAPI activity to cease.
		(napi->state 中的 NAPI_STATE_SCHED 位。设备驱动的 ndo_stop 方法会对
		所有 NAPI 实例调用 napi_disable()，后者会对 NAPI_STATE_SCHED
		napi->state 位做休眠轮询，等待所有未决的 NAPI 活动停止。)

	Context:
	(上下文：)

		 softirq
		 will be called with interrupts disabled by netconsole.
		 (softirq（软中断）；由 netconsole 调用时中断已被禁用。)

netdev instance lock
(netdev 实例锁)
====================

Historically, all networking control operations were protected by a single
global lock known as ``rtnl_lock``. There is an ongoing effort to replace this
global lock with separate locks for each network namespace. Additionally,
properties of individual netdev are increasingly protected by per-netdev locks.
(历史上，所有网络控制操作都由一个称为 ``rtnl_lock`` 的全局锁保护。目前有一项
正在进行的工作，要把这个全局锁替换为每个网络命名空间各自独立的锁。此外，单个
netdev 的属性也越来越多地由 per-netdev 锁保护。)

For device drivers that implement shaping or queue management APIs, all control
operations will be performed under the netdev instance lock.
Drivers can also explicitly request instance lock to be held during ops
by setting ``request_ops_lock`` to true. Code comments and docs refer
to drivers which have ops called under the instance lock as "ops locked".
See also the documentation of the ``lock`` member of struct net_device.
(对于实现整形或队列管理 API 的设备驱动，所有控制操作都将在 netdev 实例锁下
执行。驱动还可以通过把 ``request_ops_lock`` 设置为 true 来显式要求在回调执行
期间持有实例锁。代码注释和文档把回调在实例锁下被调用的驱动称为 "ops locked"
（回调加锁型）。另请参阅 struct net_device 的 ``lock`` 成员的文档。)

There is also a case of taking two per-netdev locks in sequence when netdev
queues are leased, that is, the netdev-scope lock is taken for both the
virtual and the physical device. To prevent deadlocks, the virtual device's
lock must always be acquired before the physical device's (see
``netdev_nl_queue_create_doit``).
(还有一种情况：当 netdev 队列被租借时，需要依次获取两个 per-netdev 锁，也就是
说，对虚拟设备和物理设备都要获取 netdev 范围的锁。为避免死锁，必须总是先获取
虚拟设备的锁，再获取物理设备的锁（参见 ``netdev_nl_queue_create_doit``）。)

In the future, there will be an option for individual
drivers to opt out of using ``rtnl_lock`` and instead perform their control
operations directly under the netdev instance lock.
(将来，会提供一种选项，让个别驱动可以选择不再使用 ``rtnl_lock``，而是直接在
netdev 实例锁下执行其控制操作。)

Device drivers are encouraged to rely on the instance lock where possible.
(鼓励设备驱动在可能的情况下依赖实例锁。)

For the (mostly software) drivers that need to interact with the core stack,
there are two sets of interfaces: ``dev_xxx``/``netdev_xxx`` and ``netif_xxx``
(e.g., ``dev_set_mtu`` and ``netif_set_mtu``). The ``dev_xxx``/``netdev_xxx``
functions handle acquiring the instance lock themselves, while the
``netif_xxx`` functions assume that the driver has already acquired
the instance lock.
(对于需要与核心栈交互的（主要是软件）驱动，有两套接口：``dev_xxx``/
``netdev_xxx`` 与 ``netif_xxx``（例如 ``dev_set_mtu`` 与 ``netif_set_mtu``）。
``dev_xxx``/``netdev_xxx`` 函数会自行获取实例锁，而 ``netif_xxx`` 函数则假定
驱动已经获取了实例锁。)

struct net_device_ops
(struct net_device_ops)
---------------------

``ndos`` are called without holding the instance lock for most drivers.
(对于大多数驱动，``ndos`` 在不持有实例锁的情况下被调用。)

"Ops locked" drivers will have most of the ``ndos`` invoked under
the instance lock.
(对于 "ops locked" 驱动，大多数 ``ndos`` 会在实例锁下被调用。)

struct ethtool_ops
(struct ethtool_ops)
------------------

Similarly to ``ndos`` the instance lock is only held for select drivers.
For "ops locked" drivers all ethtool ops without exceptions should
be called under the instance lock.
(与 ``ndos`` 类似，实例锁只对特定的驱动持有。对于 "ops locked" 驱动，所有
ethtool 回调无一例外地都应在实例锁下调用。)

struct netdev_stat_ops
(struct netdev_stat_ops)
----------------------

"qstat" ops are invoked under the instance lock for "ops locked" drivers,
and under rtnl_lock for all other drivers.
(对于 "ops locked" 驱动，"qstat" 回调在实例锁下被调用；对所有其他驱动则在
rtnl_lock 下调用。)

struct net_shaper_ops
(struct net_shaper_ops)
---------------------

All net shaper callbacks are invoked while holding the netdev instance
lock. ``rtnl_lock`` may or may not be held.
(所有 net shaper 回调都在持有 netdev 实例锁的情况下被调用。``rtnl_lock``
可能持有也可能不持有。)

Note that supporting net shapers automatically enables "ops locking".
(注意，支持 net shaper 会自动启用 "ops locking"。)

struct netdev_queue_mgmt_ops
(struct netdev_queue_mgmt_ops)
----------------------------

All queue management callbacks are invoked while holding the netdev instance
lock. ``rtnl_lock`` may or may not be held.
(所有队列管理回调都在持有 netdev 实例锁的情况下被调用。``rtnl_lock`` 可能
持有也可能不持有。)

Note that supporting struct netdev_queue_mgmt_ops automatically enables
"ops locking".
(注意，支持 struct netdev_queue_mgmt_ops 会自动启用 "ops locking"。)

Notifiers and netdev instance lock
(通知器与 netdev 实例锁)
----------------------------------

For device drivers that implement shaping or queue management APIs,
some of the notifiers (``enum netdev_cmd``) are running under the netdev
instance lock.
(对于实现整形或队列管理 API 的设备驱动，某些通知器（``enum netdev_cmd``）在
netdev 实例锁下运行。)

The following netdev notifiers are always run under the instance lock:
(以下 netdev 通知器始终在实例锁下运行：)

* ``NETDEV_XDP_FEAT_CHANGE``

For devices with locked ops, currently only the following notifiers are
running under the lock:
(对于 ops locked 设备，目前只有以下通知器在锁下运行：)

* ``NETDEV_CHANGE``
* ``NETDEV_REGISTER``
* ``NETDEV_UP``

The following notifiers are running without the lock:
(以下通知器在无锁状态下运行：)

* ``NETDEV_UNREGISTER``

There are no clear expectations for the remaining notifiers. Notifiers not on
the list may run with or without the instance lock, potentially even invoking
the same notifier type with and without the lock from different code paths.
The goal is to eventually ensure that all (or most, with a few documented
exceptions) notifiers run under the instance lock. Please extend this
documentation whenever you make explicit assumption about lock being held
from a notifier.
(对其余通知器没有明确的预期。不在上述列表中的通知器可能在持有或不持有实例锁
的情况下运行，甚至可能由不同的代码路径分别在持锁与不持锁的情况下调用同一
类型的通知器。目标是最终确保所有（或大多数，少数有文档记录的例外）通知器都
在实例锁下运行。当你明确假设某个通知器会持锁运行时，请扩充本文档。)

NETDEV_INTERNAL symbol namespace
(NETDEV_INTERNAL 符号命名空间)
================================

Symbols exported as NETDEV_INTERNAL can only be used in networking
core and drivers which exclusively flow via the main networking list and trees.
Note that the inverse is not true, most symbols outside of NETDEV_INTERNAL
are not expected to be used by random code outside netdev either.
Symbols may lack the designation because they predate the namespaces,
or simply due to an oversight.
(以 NETDEV_INTERNAL 导出的符号只能在网络核心以及完全经由主网络邮件列表与
代码树发展的驱动中使用。注意反之并不成立：NETDEV_INTERNAL 之外的大多数符号
同样不期望被 netdev 之外的随意代码使用。符号缺少该标识，可能是因为它们早于
命名空间机制出现，或者纯粹是疏忽。)
