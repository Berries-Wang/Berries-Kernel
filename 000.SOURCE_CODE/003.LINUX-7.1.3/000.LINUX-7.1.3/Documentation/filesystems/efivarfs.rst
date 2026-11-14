.. SPDX-License-Identifier: GPL-2.0

=======================================
efivarfs - a (U)EFI variable filesystem
=======================================

(efivarfs - 一个 (U)EFI 变量文件系统)

The efivarfs filesystem was created to address the shortcomings of
using entries in sysfs to maintain EFI variables. The old sysfs EFI
variables code only supported variables of up to 1024 bytes. This
limitation existed in version 0.99 of the EFI specification, but was
removed before any full releases. Since variables can now be larger
than a single page, sysfs isn't the best interface for this.
(创建 efivarfs 文件系统是为了解决使用 sysfs 条目来维护 EFI 变量的缺陷。旧的 sysfs EFI 变量代码只支持最大 1024 字节的变量。这一限制存在于 EFI 规范的 0.99 版中，但在任何完整版本发布之前就被移除了。由于变量现在可以大于单个页，sysfs 并不是实现这一功能的最佳接口。)

Variables can be created, deleted and modified with the efivarfs
filesystem.
(可以使用 efivarfs 文件系统创建、删除和修改变量。)

efivarfs is typically mounted like this::
(efivarfs 通常像这样挂载：)

	mount -t efivarfs none /sys/firmware/efi/efivars

Due to the presence of numerous firmware bugs where removing non-standard
UEFI variables causes the system firmware to fail to POST, efivarfs
files that are not well-known standardized variables are created
as immutable files.  This doesn't prevent removal - "chattr -i" will work -
but it does prevent this kind of failure from being accomplished
accidentally.
(由于存在大量固件缺陷 —— 删除非标准 UEFI 变量会导致系统固件无法完成 POST，efivarfs 中那些并非众所周知的标准变量的文件会被创建为不可变文件。这并不妨碍删除 —— "chattr -i" 可以奏效 —— 但它确实能防止这类故障被意外触发。)

.. warning ::
      When a content of an UEFI variable in /sys/firmware/efi/efivars is
      displayed, for example using "hexdump", pay attention that the first
      4 bytes of the output represent the UEFI variable attributes,
      in little-endian format.
      (当显示 /sys/firmware/efi/efivars 中 UEFI 变量的内容时，例如使用 "hexdump"，注意输出的前 4 字节代表 UEFI 变量属性，采用小端格式。)

      Practically the output of each efivar is composed of:
      (实际上，每个 efivar 的输出由以下部分组成：)

          +-----------------------------------+
          |4_bytes_of_attributes + efivar_data|
          +-----------------------------------+

*See also:*
(*另请参见：*)

- Documentation/admin-guide/acpi/ssdt-overlays.rst
- Documentation/ABI/removed/sysfs-firmware-efi-vars
