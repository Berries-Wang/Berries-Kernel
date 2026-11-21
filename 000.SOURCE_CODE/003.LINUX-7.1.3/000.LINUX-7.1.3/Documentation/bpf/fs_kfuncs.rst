.. SPDX-License-Identifier: GPL-2.0

.. _fs_kfuncs-header-label:

===========================================
BPF filesystem kfuncs(BPF 文件系统 kfuncs)
===========================================

BPF LSM programs need to access filesystem data from LSM hooks. The following
BPF kfuncs can be used to get these data.
(BPF LSM 程序需要从 LSM 钩子访问文件系统数据。以下 BPF kfuncs 可用于获取这些数据。)

 * ``bpf_get_file_xattr()``

 * ``bpf_get_fsverity_digest()``

To avoid recursions, these kfuncs follow the following rules:
(为避免递归，这些 kfuncs 遵循以下规则：)

1. These kfuncs are only permitted from BPF LSM function.
   (这些 kfuncs 仅允许从 BPF LSM 函数中调用。)
2. These kfuncs should not call into other LSM hooks, i.e. security_*(). For
   example, ``bpf_get_file_xattr()`` does not use ``vfs_getxattr()``, because
   the latter calls LSM hook ``security_inode_getxattr``.
   (这些 kfuncs 不应调用其他 LSM 钩子，即 security_*()。例如，``bpf_get_file_xattr()`` 不使用 ``vfs_getxattr()`` ，因为后者会调用 LSM 钩子 ``security_inode_getxattr`` 。)
