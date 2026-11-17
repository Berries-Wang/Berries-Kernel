.. SPDX-License-Identifier: GPL-2.0

=======================================
IOAM6 Sysfs variables(IOAM6 Sysfs 变量)
=======================================


/proc/sys/net/conf/<iface>/ioam6_* variables:(/proc/sys/net/conf/<iface>/ioam6_* 变量)
======================================================================================

ioam6_enabled - BOOL
(ioam6_enabled - BOOL(布尔值))
        Accept (= enabled) or ignore (= disabled) IPv6 IOAM options on ingress
        for this interface.
        (在该接口的入方向上，接受(即启用)还是忽略(即禁用)IPv6 IOAM 选项。)

        * 0 - disabled (default)
          (0 - 禁用(默认))
        * 1 - enabled
          (1 - 启用)

ioam6_id - SHORT INTEGER
(ioam6_id - SHORT INTEGER(短整数))
        Define the IOAM id of this interface.
        (定义该接口的 IOAM id。)

        Default is ~0.
        (默认值为 ~0。)

ioam6_id_wide - INTEGER
(ioam6_id_wide - INTEGER(整数))
        Define the wide IOAM id of this interface.
        (定义该接口的宽 IOAM id。)

        Default is ~0.
        (默认值为 ~0。)
