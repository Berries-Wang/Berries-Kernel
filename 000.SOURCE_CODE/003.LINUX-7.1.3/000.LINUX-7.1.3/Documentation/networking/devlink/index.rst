Linux Devlink Documentation
===========================

(Linux devlink 文档)

devlink is an API to expose device information and resources not directly
related to any device class, such as chip-wide/switch-ASIC-wide configuration.
(devlink 是一个 API，用于暴露与任何设备类别没有直接关联的设备和资源信息，
例如芯片级/交换机 ASIC 级配置。)

Locking
(加锁)
-------

Driver facing APIs are currently transitioning to allow more explicit
locking. Drivers can use the existing ``devlink_*`` set of APIs, or
new APIs prefixed by ``devl_*``. The older APIs handle all the locking
in devlink core, but don't allow registration of most sub-objects once
the main devlink object is itself registered. The newer ``devl_*`` APIs assume
the devlink instance lock is already held. Drivers can take the instance
lock by calling ``devl_lock()``. It is also held all callbacks of devlink
netlink commands.
(面向驱动的 API 目前正在过渡，以允许更显式的加锁。驱动可以使用现有的
``devlink_*`` 系列 API，或使用以 ``devl_*`` 为前缀的新 API。较旧的 API
由 devlink 核心处理所有加锁，但在主 devlink 对象本身注册之后，不允许
注册大多数子对象。较新的 ``devl_*`` API 则假定 devlink 实例锁已被持有。
驱动可以通过调用 ``devl_lock()`` 来获取实例锁。在 devlink netlink 命令
的所有回调期间也会持有该锁。)

Drivers are encouraged to use the devlink instance lock for their own needs.
(鼓励驱动为了自身需要使用 devlink 实例锁。)

Drivers need to be cautious when taking devlink instance lock and
taking RTNL lock at the same time. Devlink instance lock needs to be taken
first, only after that RTNL lock could be taken.
(驱动在同时获取 devlink 实例锁和 RTNL 锁时需要谨慎。必须先获取 devlink
实例锁，只有在那之后才能获取 RTNL 锁。)

Nested instances
(嵌套实例)
----------------

Some objects, like linecards or port functions, could have another
devlink instances created underneath. In that case, drivers should make
sure to respect following rules:
(某些对象，例如线卡或端口功能，可能会在其下面创建另一个 devlink 实例。
在这种情况下，驱动应确保遵守以下规则：)

 - Lock ordering should be maintained. If driver needs to take instance
   lock of both nested and parent instances at the same time, devlink
   instance lock of the parent instance should be taken first, only then
   instance lock of the nested instance could be taken.
   (应当保持加锁顺序。如果驱动需要同时获取嵌套实例和父实例的实例锁，
   应先获取父实例的 devlink 实例锁，只有在那之后才能获取嵌套实例的
   实例锁。)
 - Driver should use object-specific helpers to setup the
   nested relationship:
   (驱动应使用对象特定的辅助函数来建立嵌套关系：)

   - ``devl_nested_devlink_set()`` - called to setup devlink -> nested
     devlink relationship (could be user for multiple nested instances.
     (``devl_nested_devlink_set()`` - 用于建立 devlink -> 嵌套 devlink
     关系 (可用于多个嵌套实例。)
   - ``devl_port_fn_devlink_set()`` - called to setup port function ->
     nested devlink relationship.
     (``devl_port_fn_devlink_set()`` - 用于建立端口函数 -> 嵌套 devlink
     关系。)
   - ``devlink_linecard_nested_dl_set()`` - called to setup linecard ->
     nested devlink relationship.
     (``devlink_linecard_nested_dl_set()`` - 用于建立线卡 -> 嵌套
     devlink 关系。)

The nested devlink info is exposed to the userspace over object-specific
attributes of devlink netlink.
(嵌套 devlink 信息通过 devlink netlink 的对象特定属性暴露给用户空间。)

Interface documentation
(接口文档)
-----------------------

The following pages describe various interfaces available through devlink in
general.
(以下页面描述一般可通过 devlink 使用的各种接口。)

.. toctree::
   :maxdepth: 1

   devlink-dpipe
   devlink-eswitch-attr
   devlink-flash
   devlink-health
   devlink-info
   devlink-linecard
   devlink-params
   devlink-port
   devlink-region
   devlink-reload
   devlink-resource
   devlink-selftests
   devlink-trap
   devlink-shared

Driver-specific documentation
(驱动特定文档)
-----------------------------

Each driver that implements ``devlink`` is expected to document what
parameters, info versions, and other features it supports.
(每个实现了 ``devlink`` 的驱动都应当记录它支持的参数、info 版本以及其他
特性。)

.. toctree::
   :maxdepth: 1

   am65-nuss-cpsw-switch
   bnxt
   etas_es58x
   hns3
   i40e
   ice
   ionic
   iosm
   ixgbe
   kvaser_pciefd
   kvaser_usb
   mlx4
   mlx5
   mlxsw
   mv88e6xxx
   netdevsim
   nfp
   octeontx2
   prestera
   qed
   sfc
   stmmac
   ti-cpsw-switch
   zl3073x
