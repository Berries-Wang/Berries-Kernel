.. SPDX-License-Identifier: GPL-2.0
.. Copyright (C) 2022 Casey Schaufler <casey@schaufler-ca.com>
.. Copyright (C) 2022 Intel Corporation

======================================
Linux Security Modules(Linux 安全模块)
======================================

:Author: Casey Schaufler
(作者：Casey Schaufler)
:Date: July 2023
(日期：2023 年 7 月)

Linux security modules (LSM) provide a mechanism to implement
additional access controls to the Linux security policies.
(Linux 安全模块 (LSM) 提供一种为 Linux 安全策略实现额外访问控制的机
制。)

The various security modules may support any of these attributes:
(各种安全模块可能支持以下任意属性：)

``LSM_ATTR_CURRENT`` is the current, active security context of the
process.
The proc filesystem provides this value in ``/proc/self/attr/current``.
This is supported by the SELinux, Smack and AppArmor security modules.
Smack also provides this value in ``/proc/self/attr/smack/current``.
AppArmor also provides this value in ``/proc/self/attr/apparmor/current``.
( ``LSM_ATTR_CURRENT`` 是进程当前活动的安全上下文。proc 文件系统在
``/proc/self/attr/current`` 中提供该值。SELinux、Smack 与 AppArmor 安
全模块支持它。Smack 还在 ``/proc/self/attr/smack/current`` 中提供该
值。AppArmor 还在 ``/proc/self/attr/apparmor/current`` 中提供该值。)

``LSM_ATTR_EXEC`` is the security context of the process at the time the
current image was executed.
The proc filesystem provides this value in ``/proc/self/attr/exec``.
This is supported by the SELinux and AppArmor security modules.
AppArmor also provides this value in ``/proc/self/attr/apparmor/exec``.
( ``LSM_ATTR_EXEC`` 是执行当前镜像时的进程安全上下文。proc 文件系统
在 ``/proc/self/attr/exec`` 中提供该值。SELinux 与 AppArmor 安全模块
支持它。AppArmor 还在 ``/proc/self/attr/apparmor/exec`` 中提供该值。)

``LSM_ATTR_FSCREATE`` is the security context of the process used when
creating file system objects.
The proc filesystem provides this value in ``/proc/self/attr/fscreate``.
This is supported by the SELinux security module.
( ``LSM_ATTR_FSCREATE`` 是进程创建文件系统对象时使用的安全上下文。
proc 文件系统在 ``/proc/self/attr/fscreate`` 中提供该值。SELinux 安全
模块支持它。)

``LSM_ATTR_KEYCREATE`` is the security context of the process used when
creating key objects.
The proc filesystem provides this value in ``/proc/self/attr/keycreate``.
This is supported by the SELinux security module.
( ``LSM_ATTR_KEYCREATE`` 是进程创建密钥对象时使用的安全上下文。proc
文件系统在 ``/proc/self/attr/keycreate`` 中提供该值。SELinux 安全模块
支持它。)

``LSM_ATTR_PREV`` is the security context of the process at the time the
current security context was set.
The proc filesystem provides this value in ``/proc/self/attr/prev``.
This is supported by the SELinux and AppArmor security modules.
AppArmor also provides this value in ``/proc/self/attr/apparmor/prev``.
( ``LSM_ATTR_PREV`` 是设置当前安全上下文之时的进程安全上下文。proc
文件系统在 ``/proc/self/attr/prev`` 中提供该值。SELinux 与 AppArmor
安全模块支持它。AppArmor 还在 ``/proc/self/attr/apparmor/prev`` 中提
供该值。)

``LSM_ATTR_SOCKCREATE`` is the security context of the process used when
creating socket objects.
The proc filesystem provides this value in ``/proc/self/attr/sockcreate``.
This is supported by the SELinux security module.
( ``LSM_ATTR_SOCKCREATE`` 是进程创建套接字对象时使用的安全上下文。
proc 文件系统在 ``/proc/self/attr/sockcreate`` 中提供该值。SELinux 安
全模块支持它。)

Kernel interface(内核接口)
==========================

Set a security attribute of the current process(设置当前进程的安全属性)
-----------------------------------------------------------------------

.. kernel-doc:: security/lsm_syscalls.c
    :identifiers: sys_lsm_set_self_attr

Get the specified security attributes of the current process(获取当前进程的指定安全属性)
----------------------------------------------------------------------------------------

.. kernel-doc:: security/lsm_syscalls.c
    :identifiers: sys_lsm_get_self_attr

.. kernel-doc:: security/lsm_syscalls.c
    :identifiers: sys_lsm_list_modules

Additional documentation(更多文档)
==================================

* Documentation/security/lsm.rst
* Documentation/security/lsm-development.rst
