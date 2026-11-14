=====================================
Filesystem-level encryption (fscrypt)
=====================================

(文件系统级加密 (fscrypt))

Introduction
(引言)
============

fscrypt is a library which filesystems can hook into to support
transparent encryption of files and directories.
(fscrypt 是一个库，文件系统可以挂接它以支持对文件和目录的透明加密。)

Note: "fscrypt" in this document refers to the kernel-level portion,
implemented in ``fs/crypto/``, as opposed to the userspace tool
`fscrypt <https://github.com/google/fscrypt>`_.  This document only
covers the kernel-level portion.  For command-line examples of how to
use encryption, see the documentation for the userspace tool `fscrypt
<https://github.com/google/fscrypt>`_.  Also, it is recommended to use
the fscrypt userspace tool, or other existing userspace tools such as
`fscryptctl <https://github.com/google/fscryptctl>`_ or `Android's key
management system
<https://source.android.com/security/encryption/file-based>`_, over
using the kernel's API directly.  Using existing tools reduces the
chance of introducing your own security bugs.  (Nevertheless, for
completeness this documentation covers the kernel's API anyway.)
(注意: 本文档中的 "fscrypt" 指的是内核级部分，实现在 ``fs/crypto/`` 中，
与用户空间工具 `fscrypt <https://github.com/google/fscrypt>`_ 相对。本文档
仅涵盖内核级部分。关于如何使用加密的命令行示例，请参见用户空间工具
`fscrypt <https://github.com/google/fscrypt>`_ 的文档。此外，建议使用
fscrypt 用户空间工具，或其他现有的用户空间工具，例如
`fscryptctl <https://github.com/google/fscryptctl>`_ 或 `Android 的密钥
管理系统
<https://source.android.com/security/encryption/file-based>`_，而不是
直接使用内核的 API。使用现有工具可以减少引入自身安全漏洞的机会。
(尽管如此，为了完整性，本文档仍然涵盖了内核的 API。))

Unlike dm-crypt, fscrypt operates at the filesystem level rather than
at the block device level.  This allows it to encrypt different files
with different keys and to have unencrypted files on the same
filesystem.  This is useful for multi-user systems where each user's
data-at-rest needs to be cryptographically isolated from the others.
However, except for filenames, fscrypt does not encrypt filesystem
metadata.
(与 dm-crypt 不同，fscrypt 在文件系统层面而不是块设备层面运行。这使得它
可以用不同的密钥加密不同的文件，并在同一个文件系统上保存未加密的文件。
这对于多用户系统非常有用，因为在这类系统中，每个用户的静态数据需要与其
他用户在密码学上隔离。但是，除文件名外，fscrypt 不加密文件系统元数据。)

Unlike eCryptfs, which is a stacked filesystem, fscrypt is integrated
directly into supported filesystems --- currently ext4, F2FS, UBIFS,
and CephFS.  This allows encrypted files to be read and written
without caching both the decrypted and encrypted pages in the
pagecache, thereby nearly halving the memory used and bringing it in
line with unencrypted files.  Similarly, half as many dentries and
inodes are needed.  eCryptfs also limits encrypted filenames to 143
bytes, causing application compatibility issues; fscrypt allows the
full 255 bytes (NAME_MAX).  Finally, unlike eCryptfs, the fscrypt API
can be used by unprivileged users, with no need to mount anything.
(与作为堆叠文件系统的 eCryptfs 不同，fscrypt 直接集成到受支持的文件系统
中——目前是 ext4、F2FS、UBIFS 和 CephFS。这使得加密文件的读写无需同时
在页缓存中缓存解密后的页和加密后的页，从而几乎将内存使用量减半，使其与
未加密的文件持平。类似地，所需的 dentry 和 inode 数量也减半。eCryptfs
还将加密文件名限制为 143 字节，导致应用程序兼容性问题；fscrypt 允许完整
的 255 字节 (NAME_MAX)。最后，与 eCryptfs 不同，fscrypt API 可由无特权
用户使用，无需挂载任何东西。)

fscrypt does not support encrypting files in-place.  Instead, it
supports marking an empty directory as encrypted.  Then, after
userspace provides the key, all regular files, directories, and
symbolic links created in that directory tree are transparently
encrypted.
(fscrypt 不支持就地加密文件。相反，它支持将一个空目录标记为加密目录。
然后，在用户空间提供密钥之后，在该目录树中创建的所有普通文件、目录和
符号链接都会被透明地加密。)

Threat model
(威胁模型)
============

Offline attacks
(离线攻击)
---------------

Provided that userspace chooses a strong encryption key, fscrypt
protects the confidentiality of file contents and filenames in the
event of a single point-in-time permanent offline compromise of the
block device content.  fscrypt does not protect the confidentiality of
non-filename metadata, e.g. file sizes, file permissions, file
timestamps, and extended attributes.  Also, the existence and location
of holes (unallocated blocks which logically contain all zeroes) in
files is not protected.
(只要用户空间选择了强加密密钥，fscrypt 就能在块设备内容发生单次时间点
永久性离线泄露的情况下，保护文件内容和文件名的机密性。fscrypt 不保护
非文件名元数据的机密性，例如文件大小、文件权限、文件时间戳和扩展属性。
此外，文件中空洞(逻辑上全为零的未分配块)的存在和位置也不受保护。)

fscrypt is not guaranteed to protect confidentiality or authenticity
if an attacker is able to manipulate the filesystem offline prior to
an authorized user later accessing the filesystem.
(如果攻击者能够在授权用户之后访问文件系统之前离线篡改文件系统，则
fscrypt 不保证保护机密性或真实性。)

Online attacks
(在线攻击)
--------------

fscrypt (and storage encryption in general) can only provide limited
protection against online attacks.  In detail:
(fscrypt(以及一般意义上的存储加密)只能对在线攻击提供有限的保护。详细
说明如下：)

Side-channel attacks
(侧信道攻击)
~~~~~~~~~~~~~~~~~~~~

fscrypt is only resistant to side-channel attacks, such as timing or
electromagnetic attacks, to the extent that the underlying Linux
Cryptographic API algorithms or inline encryption hardware are.  If a
vulnerable algorithm is used, such as a table-based implementation of
AES, it may be possible for an attacker to mount a side channel attack
against the online system.  Side channel attacks may also be mounted
against applications consuming decrypted data.
(fscrypt 对侧信道攻击(例如时序攻击或电磁攻击)的抵抗力，取决于底层 Linux
加密 API 算法或内联加密硬件的抵抗力。如果使用了易受攻击的算法，例如
基于查找表的 AES 实现，攻击者就有可能对在线系统发起侧信道攻击。对消费
解密数据的应用程序，也可能发起侧信道攻击。)

Unauthorized file access
(未授权的文件访问)
~~~~~~~~~~~~~~~~~~~~~~~~

After an encryption key has been added, fscrypt does not hide the
plaintext file contents or filenames from other users on the same
system.  Instead, existing access control mechanisms such as file mode
bits, POSIX ACLs, LSMs, or namespaces should be used for this purpose.
(在加密密钥被添加之后，fscrypt 不会向同一系统上的其他用户隐藏明文文件
内容或文件名。相反，应使用现有的访问控制机制来实现这一目的，例如文件
权限位、POSIX ACL、LSM 或命名空间。)

(For the reasoning behind this, understand that while the key is
added, the confidentiality of the data, from the perspective of the
system itself, is *not* protected by the mathematical properties of
encryption but rather only by the correctness of the kernel.
Therefore, any encryption-specific access control checks would merely
be enforced by kernel *code* and therefore would be largely redundant
with the wide variety of access control mechanisms already available.)
((关于这背后的理由，需要理解的是：在密钥被添加期间，从系统自身的角度
来看，数据的机密性*并非*由加密的数学性质保护，而仅由内核的正确性保护。
因此，任何加密专有的访问控制检查只会由内核*代码*来执行，因此与已有的
多种访问控制机制相比，在很大程度上是冗余的。))

Read-only kernel memory compromise
(只读内核内存被攻破)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Unless `hardware-wrapped keys`_ are used, an attacker who gains the
ability to read from arbitrary kernel memory, e.g. by mounting a
physical attack or by exploiting a kernel security vulnerability, can
compromise all fscrypt keys that are currently in-use.  This also
extends to cold boot attacks; if the system is suddenly powered off,
keys the system was using may remain in memory for a short time.
(除非使用了 `hardware-wrapped keys`_，否则获得了读取任意内核内存能力的
攻击者，例如通过实施物理攻击或利用内核安全漏洞，可以攻破当前正在使用的
所有 fscrypt 密钥。这也延伸到冷启动攻击；如果系统突然断电，系统当时正在
使用的密钥可能会在内存中留存一小段时间。)

However, if hardware-wrapped keys are used, then the fscrypt master
keys and file contents encryption keys (but not other types of fscrypt
subkeys such as filenames encryption keys) are protected from
compromises of arbitrary kernel memory.
(但是，如果使用了硬件封装密钥(hardware-wrapped keys)，那么 fscrypt 主
密钥和文件内容加密密钥(但不包括其他类型的 fscrypt 子密钥，例如文件名
加密密钥)将免受任意内核内存被攻破的影响。)

In addition, fscrypt allows encryption keys to be removed from the
kernel, which may protect them from later compromise.
(此外，fscrypt 允许将加密密钥从内核中移除，这可以保护它们免受之后的
攻破。)

In more detail, the FS_IOC_REMOVE_ENCRYPTION_KEY ioctl (or the
FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS ioctl) can wipe a master
encryption key from kernel memory.  If it does so, it will also try to
evict all cached inodes which had been "unlocked" using the key,
thereby wiping their per-file keys and making them once again appear
"locked", i.e. in ciphertext or encrypted form.
(更详细地说，FS_IOC_REMOVE_ENCRYPTION_KEY ioctl(或
FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS ioctl)可以从内核内存中擦除主
加密密钥。如果这样做，它还会尝试逐出所有曾使用该密钥"解锁"的缓存
inode，从而擦除它们的每文件密钥(per-file key)，使它们再次呈现"锁定"
状态，即以密文或加密形式呈现。)

However, these ioctls have some limitations:
(但是，这些 ioctl 有一些局限性：)

- Per-file keys for in-use files will *not* be removed or wiped.
  Therefore, for maximum effect, userspace should close the relevant
  encrypted files and directories before removing a master key, as
  well as kill any processes whose working directory is in an affected
  encrypted directory.
  (正在使用中的文件的每文件密钥*不会*被移除或擦除。因此，为了获得最大
  效果，用户空间应在移除主密钥之前关闭相关的加密文件和目录，并杀死
  工作目录位于受影响加密目录中的所有进程。)

- The kernel cannot magically wipe copies of the master key(s) that
  userspace might have as well.  Therefore, userspace must wipe all
  copies of the master key(s) it makes as well; normally this should
  be done immediately after FS_IOC_ADD_ENCRYPTION_KEY, without waiting
  for FS_IOC_REMOVE_ENCRYPTION_KEY.  Naturally, the same also applies
  to all higher levels in the key hierarchy.  Userspace should also
  follow other security precautions such as mlock()ing memory
  containing keys to prevent it from being swapped out.
  (内核也无法神奇地擦除用户空间可能持有的主密钥副本。因此，用户空间
  也必须擦除其制作的所有主密钥副本；通常应在 FS_IOC_ADD_ENCRYPTION_KEY
  之后立即执行此操作，而不应等待 FS_IOC_REMOVE_ENCRYPTION_KEY。自然，
  这同样适用于密钥层次结构中所有更高的层级。用户空间还应遵循其他安全
  预防措施，例如对包含密钥的内存调用 mlock()，以防止其被换出。)

- In general, decrypted contents and filenames in the kernel VFS
  caches are freed but not wiped.  Therefore, portions thereof may be
  recoverable from freed memory, even after the corresponding key(s)
  were wiped.  To partially solve this, you can add init_on_free=1 to
  your kernel command line.  However, this has a performance cost.
  (一般来说，内核 VFS 缓存中已解密的内容和文件名会被释放但不会被擦除。
  因此，即使在相应的密钥被擦除之后，其中的一部分仍可能从已释放的内存中
  被恢复。要部分解决此问题，可以在内核命令行中添加 init_on_free=1。
  但是，这会有性能开销。)

- Secret keys might still exist in CPU registers or in other places
  not explicitly considered here.
  (秘密密钥可能仍然存在于 CPU 寄存器中，或存在于此处未明确考虑的其他
  地方。)

Full system compromise
(整个系统被攻破)
~~~~~~~~~~~~~~~~~~~~~~

An attacker who gains "root" access and/or the ability to execute
arbitrary kernel code can freely exfiltrate data that is protected by
any in-use fscrypt keys.  Thus, usually fscrypt provides no meaningful
protection in this scenario.  (Data that is protected by a key that is
absent throughout the entire attack remains protected, modulo the
limitations of key removal mentioned above in the case where the key
was removed prior to the attack.)
(获得 "root" 访问权限和/或执行任意内核代码能力的攻击者，可以随意窃取
受任何正在使用的 fscrypt 密钥保护的数据。因此，在此场景中 fscrypt 通常
无法提供有意义的保护。(在整个攻击过程中密钥始终不存在的受保护数据仍然
受到保护，但如果密钥在攻击之前已被移除，则受上述密钥移除局限性的
制约。))

However, if `hardware-wrapped keys`_ are used, such attackers will be
unable to exfiltrate the master keys or file contents keys in a form
that will be usable after the system is powered off.  This may be
useful if the attacker is significantly time-limited and/or
bandwidth-limited, so they can only exfiltrate some data and need to
rely on a later offline attack to exfiltrate the rest of it.
(但是，如果使用了 `hardware-wrapped keys`_，此类攻击者将无法以系统断电
后仍然可用的形式窃取主密钥或文件内容密钥。如果攻击者受到明显的时间限制
和/或带宽限制，以致只能窃取部分数据、而需要依赖之后的离线攻击来窃取
其余数据，这可能会有用。)

Limitations of v1 policies
(v1 策略的局限性)
~~~~~~~~~~~~~~~~~~~~~~~~~~

v1 encryption policies have some weaknesses with respect to online
attacks:
(v1 加密策略在在线攻击方面存在一些弱点：)

- There is no verification that the provided master key is correct.
  Therefore, a malicious user can temporarily associate the wrong key
  with another user's encrypted files to which they have read-only
  access.  Because of filesystem caching, the wrong key will then be
  used by the other user's accesses to those files, even if the other
  user has the correct key in their own keyring.  This violates the
  meaning of "read-only access".
  (没有对所提供的主密钥是否正确进行验证。因此，恶意用户可以临时将错误
  的密钥与另一用户对其拥有只读访问权限的加密文件相关联。由于文件系统
  缓存的存在，另一用户随后对这些文件的访问将使用错误的密钥，即使该用户
  在自己的密钥环(keyring)中拥有正确的密钥。这违背了"只读访问"的含义。)

- A compromise of a per-file key also compromises the master key from
  which it was derived.
  (每文件密钥被攻破也意味着其派生自的主密钥被攻破。)

- Non-root users cannot securely remove encryption keys.
  (非 root 用户无法安全地移除加密密钥。)

All the above problems are fixed with v2 encryption policies.  For
this reason among others, it is recommended to use v2 encryption
policies on all new encrypted directories.
(以上所有问题都已通过 v2 加密策略修复。基于此原因及其他原因，建议在
所有新加密目录上使用 v2 加密策略。)

Key hierarchy
(密钥层次结构)
=============

Note: this section assumes the use of raw keys rather than
hardware-wrapped keys.  The use of hardware-wrapped keys modifies the
key hierarchy slightly.  For details, see `Hardware-wrapped keys`_.
(注意: 本节假设使用原始密钥(raw key)而非硬件封装密钥。硬件封装密钥的
使用会对密钥层次结构稍作修改。详情请参见 `Hardware-wrapped keys`_。)

Master Keys
(主密钥)
-----------

Each encrypted directory tree is protected by a *master key*.  Master
keys can be up to 64 bytes long, and must be at least as long as the
greater of the security strength of the contents and filenames
encryption modes being used.  For example, if any AES-256 mode is
used, the master key must be at least 256 bits, i.e. 32 bytes.  A
stricter requirement applies if the key is used by a v1 encryption
policy and AES-256-XTS is used; such keys must be 64 bytes.
(每个加密目录树都由一个*主密钥*(master key)保护。主密钥最长可达 64 字节，
并且其长度必须至少等于所用内容和文件名加密模式中安全强度较高者的长度。
例如，如果使用了任何 AES-256 模式，主密钥必须至少为 256 位，即 32 字节。
如果该密钥由 v1 加密策略使用且使用了 AES-256-XTS，则有更严格的要求；
此类密钥必须为 64 字节。)

To "unlock" an encrypted directory tree, userspace must provide the
appropriate master key.  There can be any number of master keys, each
of which protects any number of directory trees on any number of
filesystems.
(要"解锁"一个加密目录树，用户空间必须提供相应的主密钥。主密钥可以有
任意多个，每个主密钥可以保护任意多个文件系统上的任意多个目录树。)

Master keys must be real cryptographic keys, i.e. indistinguishable
from random bytestrings of the same length.  This implies that users
**must not** directly use a password as a master key, zero-pad a
shorter key, or repeat a shorter key.  Security cannot be guaranteed
if userspace makes any such error, as the cryptographic proofs and
analysis would no longer apply.
(主密钥必须是真正的密码学密钥，即与相同长度的随机字节串不可区分。这
意味着用户**不得**直接将口令用作主密钥、对较短的密钥做零填充、或重复
较短的密钥。如果用户空间犯了任何此类错误，则无法保证安全性，因为密码
学证明和分析将不再适用。)

Instead, users should generate master keys either using a
cryptographically secure random number generator, or by using a KDF
(Key Derivation Function).  The kernel does not do any key stretching;
therefore, if userspace derives the key from a low-entropy secret such
as a passphrase, it is critical that a KDF designed for this purpose
be used, such as scrypt, PBKDF2, or Argon2.
(相反，用户应当使用密码学安全的随机数生成器来生成主密钥，或者使用 KDF
(密钥派生函数，Key Derivation Function)。内核不做任何密钥拉伸(key
stretching)；因此，如果用户空间从诸如口令这类低熵秘密派生密钥，至关
重要的是使用为此目的设计的 KDF，例如 scrypt、PBKDF2 或 Argon2。)

Key derivation function
(密钥派生函数)
-----------------------

With one exception, fscrypt never uses the master key(s) for
encryption directly.  Instead, they are only used as input to a KDF
(Key Derivation Function) to derive the actual keys.
(除一个例外情况外，fscrypt 从不直接使用主密钥进行加密。相反，它们仅
用作 KDF(密钥派生函数)的输入来派生实际使用的密钥。)

The KDF used for a particular master key differs depending on whether
the key is used for v1 encryption policies or for v2 encryption
policies.  Users **must not** use the same key for both v1 and v2
encryption policies.  (No real-world attack is currently known on this
specific case of key reuse, but its security cannot be guaranteed
since the cryptographic proofs and analysis would no longer apply.)
(特定主密钥所用的 KDF 取决于该密钥是用于 v1 加密策略还是 v2 加密策略。
用户**不得**将同一个密钥同时用于 v1 和 v2 加密策略。(目前尚不知道针对
这种特定密钥重用情况的真实攻击，但由于密码学证明和分析将不再适用，
其安全性无法得到保证。))

For v1 encryption policies, the KDF only supports deriving per-file
encryption keys.  It works by encrypting the master key with
AES-128-ECB, using the file's 16-byte nonce as the AES key.  The
resulting ciphertext is used as the derived key.  If the ciphertext is
longer than needed, then it is truncated to the needed length.
(对于 v1 加密策略，KDF 仅支持派生每文件加密密钥。它的工作方式是：使用
文件的 16 字节 nonce 作为 AES 密钥，用 AES-128-ECB 加密主密钥。得到的
密文被用作派生的密钥。如果密文长于所需长度，则将其截断到所需长度。)

For v2 encryption policies, the KDF is HKDF-SHA512.  The master key is
passed as the "input keying material", no salt is used, and a distinct
"application-specific information string" is used for each distinct
key to be derived.  For example, when a per-file encryption key is
derived, the application-specific information string is the file's
nonce prefixed with "fscrypt\\0" and a context byte.  Different
context bytes are used for other types of derived keys.
(对于 v2 加密策略，KDF 是 HKDF-SHA512。主密钥作为"输入密钥材料"(input
keying material)传入，不使用盐值(salt)，并且每个要派生的不同密钥使用
不同的"应用特定信息字符串"。例如，在派生每文件加密密钥时，应用特定
信息字符串是文件的 nonce，前缀为 "fscrypt\\0" 和一个上下文字节。其他
类型的派生密钥使用不同的上下文字节。)

HKDF-SHA512 is preferred to the original AES-128-ECB based KDF because
HKDF is more flexible, is nonreversible, and evenly distributes
entropy from the master key.  HKDF is also standardized and widely
used by other software, whereas the AES-128-ECB based KDF is ad-hoc.
(HKDF-SHA512 优于最初基于 AES-128-ECB 的 KDF，因为 HKDF 更灵活、不可
逆转，并且能均匀分布主密钥的熵。HKDF 还是标准化的，并被其他软件广泛
使用，而基于 AES-128-ECB 的 KDF 是临时专用的。)

Per-file encryption keys
(每文件加密密钥)
------------------------

Since each master key can protect many files, it is necessary to
"tweak" the encryption of each file so that the same plaintext in two
files doesn't map to the same ciphertext, or vice versa.  In most
cases, fscrypt does this by deriving per-file keys.  When a new
encrypted inode (regular file, directory, or symlink) is created,
fscrypt randomly generates a 16-byte nonce and stores it in the
inode's encryption xattr.  Then, it uses a KDF (as described in `Key
derivation function`_) to derive the file's key from the master key
and nonce.
(由于每个主密钥可以保护多个文件，因此有必要对每个文件的加密进行"调整"
(tweak)，使得两个文件中的相同明文不会映射到相同的密文，反之亦然。在
大多数情况下，fscrypt 通过派生每文件密钥来做到这一点。当创建一个新的
加密 inode(普通文件、目录或符号链接)时，fscrypt 会随机生成一个 16 字节
的 nonce，并将其存储在 inode 的加密 xattr 中。然后，它使用 KDF(如
`Key derivation function`_ 中所述)从主密钥和 nonce 派生该文件的密钥。)

Key derivation was chosen over key wrapping because wrapped keys would
require larger xattrs which would be less likely to fit in-line in the
filesystem's inode table, and there didn't appear to be any
significant advantages to key wrapping.  In particular, currently
there is no requirement to support unlocking a file with multiple
alternative master keys or to support rotating master keys.  Instead,
the master keys may be wrapped in userspace, e.g. as is done by the
`fscrypt <https://github.com/google/fscrypt>`_ tool.
(之所以选择密钥派生而非密钥封装(key wrapping)，是因为封装密钥需要更大
的 xattr，更难以内联保存在文件系统的 inode 表中，而且密钥封装似乎没有
任何显著优势。特别是，目前没有需求要支持用多个备用主密钥解锁一个文件，
也没有需求要支持轮换主密钥。相反，主密钥可以在用户空间中被封装，例如
`fscrypt <https://github.com/google/fscrypt>`_ 工具就是这样做的。)

DIRECT_KEY policies
(DIRECT_KEY 策略)
-------------------

The Adiantum encryption mode (see `Encryption modes and usage`_) is
suitable for both contents and filenames encryption, and it accepts
long IVs --- long enough to hold both an 8-byte data unit index and a
16-byte per-file nonce.  Also, the overhead of each Adiantum key is
greater than that of an AES-256-XTS key.
(Adiantum 加密模式(参见 `Encryption modes and usage`_)同时适用于内容
加密和文件名加密，并且它接受较长的 IV——长到足以同时容纳一个 8 字节的
数据单元索引和一个 16 字节的每文件 nonce。此外，每个 Adiantum 密钥的
开销大于 AES-256-XTS 密钥的开销。)

Therefore, to improve performance and save memory, for Adiantum a
"direct key" configuration is supported.  When the user has enabled
this by setting FSCRYPT_POLICY_FLAG_DIRECT_KEY in the fscrypt policy,
per-file encryption keys are not used.  Instead, whenever any data
(contents or filenames) is encrypted, the file's 16-byte nonce is
included in the IV.  Moreover:
(因此，为了提高性能并节省内存，Adiantum 支持"直接密钥"(direct key)
配置。当用户通过在 fscrypt 策略中设置 FSCRYPT_POLICY_FLAG_DIRECT_KEY
来启用此配置时，将不使用每文件加密密钥。相反，每当加密任何数据(内容
或文件名)时，文件的 16 字节 nonce 都会包含在 IV 中。此外：)

- For v1 encryption policies, the encryption is done directly with the
  master key.  Because of this, users **must not** use the same master
  key for any other purpose, even for other v1 policies.
  (对于 v1 加密策略，加密直接使用主密钥完成。因此，用户**不得**将同一
  主密钥用于任何其他用途，即使是其他 v1 策略。)

- For v2 encryption policies, the encryption is done with a per-mode
  key derived using the KDF.  Users may use the same master key for
  other v2 encryption policies.
  (对于 v2 加密策略，加密使用通过 KDF 派生的每模式密钥完成。用户可以将
  同一主密钥用于其他 v2 加密策略。)

IV_INO_LBLK_64 policies
(IV_INO_LBLK_64 策略)
-----------------------

When FSCRYPT_POLICY_FLAG_IV_INO_LBLK_64 is set in the fscrypt policy,
the encryption keys are derived from the master key, encryption mode
number, and filesystem UUID.  This normally results in all files
protected by the same master key sharing a single contents encryption
key and a single filenames encryption key.  To still encrypt different
files' data differently, inode numbers are included in the IVs.
Consequently, shrinking the filesystem may not be allowed.
(当在 fscrypt 策略中设置了 FSCRYPT_POLICY_FLAG_IV_INO_LBLK_64 时，加密
密钥将从主密钥、加密模式编号和文件系统 UUID 派生。这通常导致由同一主
密钥保护的所有文件共享单一的内容加密密钥和单一的文件名加密密钥。为了
仍然以不同方式加密不同文件的数据，inode 编号被包含在 IV 中。因此，可能
不允许缩小文件系统。)

This format is optimized for use with inline encryption hardware
compliant with the UFS standard, which supports only 64 IV bits per
I/O request and may have only a small number of keyslots.
(此格式针对符合 UFS 标准的内联加密硬件进行了优化，这种硬件每个 I/O 请求
仅支持 64 个 IV 位，并且可能只有少量的密钥槽(keyslot)。)

IV_INO_LBLK_32 policies
(IV_INO_LBLK_32 策略)
-----------------------

IV_INO_LBLK_32 policies work like IV_INO_LBLK_64, except that for
IV_INO_LBLK_32, the inode number is hashed with SipHash-2-4 (where the
SipHash key is derived from the master key) and added to the file data
unit index mod 2^32 to produce a 32-bit IV.
(IV_INO_LBLK_32 策略的工作方式与 IV_INO_LBLK_64 类似，不同之处在于：对于
IV_INO_LBLK_32，inode 编号先用 SipHash-2-4 进行哈希(其中 SipHash 密钥从
主密钥派生)，然后以模 2^32 加到文件数据单元索引上，产生一个 32 位的 IV。)

This format is optimized for use with inline encryption hardware
compliant with the eMMC v5.2 standard, which supports only 32 IV bits
per I/O request and may have only a small number of keyslots.  This
format results in some level of IV reuse, so it should only be used
when necessary due to hardware limitations.
(此格式针对符合 eMMC v5.2 标准的内联加密硬件进行了优化，这种硬件每个
I/O 请求仅支持 32 个 IV 位，并且可能只有少量的密钥槽。此格式会导致
一定程度的 IV 重用，因此只应在因硬件限制而必要时使用。)

Key identifiers
(密钥标识符)
---------------

For master keys used for v2 encryption policies, a unique 16-byte "key
identifier" is also derived using the KDF.  This value is stored in
the clear, since it is needed to reliably identify the key itself.
(对于用于 v2 加密策略的主密钥，还会使用 KDF 派生一个唯一的 16 字节"密钥
标识符"(key identifier)。该值以明文形式存储，因为需要它来可靠地标识
密钥本身。)

Dirhash keys
(目录哈希密钥)
------------

For directories that are indexed using a secret-keyed dirhash over the
plaintext filenames, the KDF is also used to derive a 128-bit
SipHash-2-4 key per directory in order to hash filenames.  This works
just like deriving a per-file encryption key, except that a different
KDF context is used.  Currently, only casefolded ("case-insensitive")
encrypted directories use this style of hashing.
(对于使用基于明文文件名的带秘密密钥 dirhash 进行索引的目录，KDF 还用于
为每个目录派生一个 128 位的 SipHash-2-4 密钥，以便对文件名进行哈希。
其工作方式与派生每文件加密密钥相同，只是使用了不同的 KDF 上下文。目前，
只有启用了大小写折叠(casefolded，即"大小写不敏感")的加密目录才使用这种
哈希方式。)

Encryption modes and usage
(加密模式与用法)
==========================

fscrypt allows one encryption mode to be specified for file contents
and one encryption mode to be specified for filenames.  Different
directory trees are permitted to use different encryption modes.
(fscrypt 允许为文件内容指定一种加密模式，并为文件名指定一种加密模式。
不同的目录树允许使用不同的加密模式。)

Supported modes
(支持的模式)
---------------

Currently, the following pairs of encryption modes are supported:
(目前，支持以下几对加密模式：)

- AES-256-XTS for contents and AES-256-CBC-CTS for filenames
  (内容使用 AES-256-XTS，文件名使用 AES-256-CBC-CTS)
- AES-256-XTS for contents and AES-256-HCTR2 for filenames
  (内容使用 AES-256-XTS，文件名使用 AES-256-HCTR2)
- Adiantum for both contents and filenames
  (内容和文件名均使用 Adiantum)
- AES-128-CBC-ESSIV for contents and AES-128-CBC-CTS for filenames
  (内容使用 AES-128-CBC-ESSIV，文件名使用 AES-128-CBC-CTS)
- SM4-XTS for contents and SM4-CBC-CTS for filenames
  (内容使用 SM4-XTS，文件名使用 SM4-CBC-CTS)

Note: in the API, "CBC" means CBC-ESSIV, and "CTS" means CBC-CTS.
So, for example, FSCRYPT_MODE_AES_256_CTS means AES-256-CBC-CTS.
(注意: 在 API 中，"CBC" 指 CBC-ESSIV，"CTS" 指 CBC-CTS。因此，例如
FSCRYPT_MODE_AES_256_CTS 表示 AES-256-CBC-CTS。)

Authenticated encryption modes are not currently supported because of
the difficulty of dealing with ciphertext expansion.  Therefore,
contents encryption uses a block cipher in `XTS mode
<https://en.wikipedia.org/wiki/Disk_encryption_theory#XTS>`_ or
`CBC-ESSIV mode
<https://en.wikipedia.org/wiki/Disk_encryption_theory#Encrypted_salt-sector_initialization_vector_(ESSIV)>`_,
or a wide-block cipher.  Filenames encryption uses a
block cipher in `CBC-CTS mode
<https://en.wikipedia.org/wiki/Ciphertext_stealing>`_ or a wide-block
cipher.
(由于处理密文扩展的困难，目前不支持认证加密模式。因此，内容加密使用
`XTS 模式
<https://en.wikipedia.org/wiki/Disk_encryption_theory#XTS>`_ 或
`CBC-ESSIV 模式
<https://en.wikipedia.org/wiki/Disk_encryption_theory#Encrypted_salt-sector_initialization_vector_(ESSIV)>`_
的分组密码，或宽块密码(wide-block cipher)。文件名加密使用 `CBC-CTS 模式
<https://en.wikipedia.org/wiki/Ciphertext_stealing>`_ 的分组密码，或
宽块密码。)

The (AES-256-XTS, AES-256-CBC-CTS) pair is the recommended default.
It is also the only option that is *guaranteed* to always be supported
if the kernel supports fscrypt at all; see `Kernel config options`_.
((AES-256-XTS, AES-256-CBC-CTS) 这一对是推荐的默认选项。它也是唯一
*保证*在内核支持 fscrypt 时始终受支持的选项；参见 `Kernel config
options`_。)

The (AES-256-XTS, AES-256-HCTR2) pair is also a good choice that
upgrades the filenames encryption to use a wide-block cipher.  (A
*wide-block cipher*, also called a tweakable super-pseudorandom
permutation, has the property that changing one bit scrambles the
entire result.)  As described in `Filenames encryption`_, a wide-block
cipher is the ideal mode for the problem domain, though CBC-CTS is the
"least bad" choice among the alternatives.  For more information about
HCTR2, see `the HCTR2 paper <https://eprint.iacr.org/2021/1441.pdf>`_.
((AES-256-XTS, AES-256-HCTR2) 这一对也是一个好选择，它将文件名加密升级
为使用宽块密码。(*宽块密码*(wide-block cipher)，也称为可调超级伪随机
置换(tweakable super-pseudorandom permutation)，具有改变一位就会打乱
整个结果的特性。)如 `Filenames encryption`_ 中所述，宽块密码是该问题
领域的理想模式，尽管 CBC-CTS 是各备选方案中"最不坏"的选择。关于
HCTR2 的更多信息，参见 `the HCTR2 paper
<https://eprint.iacr.org/2021/1441.pdf>`_。)

Adiantum is recommended on systems where AES is too slow due to lack
of hardware acceleration for AES.  Adiantum is a wide-block cipher
that uses XChaCha12 and AES-256 as its underlying components.  Most of
the work is done by XChaCha12, which is much faster than AES when AES
acceleration is unavailable.  For more information about Adiantum, see
`the Adiantum paper <https://eprint.iacr.org/2018/720.pdf>`_.
(在因缺乏 AES 硬件加速而导致 AES 过慢的系统上，推荐使用 Adiantum。
Adiantum 是一种宽块密码，它使用 XChaCha12 和 AES-256 作为其底层组件。
大部分工作由 XChaCha12 完成，当 AES 加速不可用时，XChaCha12 比 AES 快
得多。关于 Adiantum 的更多信息，参见 `the Adiantum paper
<https://eprint.iacr.org/2018/720.pdf>`_。)

The (AES-128-CBC-ESSIV, AES-128-CBC-CTS) pair was added to try to
provide a more efficient option for systems that lack AES instructions
in the CPU but do have a non-inline crypto engine such as CAAM or CESA
that supports AES-CBC (and not AES-XTS).  This is deprecated.  It has
been shown that just doing AES on the CPU is actually faster.
Moreover, Adiantum is faster still and is recommended on such systems.
((AES-128-CBC-ESSIV, AES-128-CBC-CTS) 这一对是为了给那些 CPU 缺少 AES
指令但确实拥有支持 AES-CBC(而非 AES-XTS)的非内联加密引擎(例如 CAAM 或
CESA)的系统提供一个更高效的选项而添加的。这已被弃用。事实证明，直接在
CPU 上执行 AES 实际上更快。而且，Adiantum 仍然更快，在这类系统上推荐
使用它。)

The remaining mode pairs are the "national pride ciphers":
(其余的模式对是"民族自豪密码"(national pride ciphers)：)

- (SM4-XTS, SM4-CBC-CTS)
  ((SM4-XTS, SM4-CBC-CTS))

Generally speaking, these ciphers aren't "bad" per se, but they
receive limited security review compared to the usual choices such as
AES and ChaCha.  They also don't bring much new to the table.  It is
suggested to only use these ciphers where their use is mandated.
(总的来说，这些密码本身并不"差"，但与 AES 和 ChaCha 等常见选择相比，
它们受到的安全审查有限。它们也没有带来多少新东西。建议仅在强制要求
使用它们的地方使用这些密码。)

Kernel config options
(内核配置选项)
---------------------

Enabling fscrypt support (CONFIG_FS_ENCRYPTION) automatically pulls in
only the basic support from the crypto API needed to use AES-256-XTS
and AES-256-CBC-CTS encryption.  For optimal performance, it is
strongly recommended to also enable any available platform-specific
kconfig options that provide acceleration for the algorithm(s) you
wish to use.  Support for any "non-default" encryption modes typically
requires extra kconfig options as well.
(启用 fscrypt 支持(CONFIG_FS_ENCRYPTION)只会自动引入加密 API 中使用
AES-256-XTS 和 AES-256-CBC-CTS 加密所需的基本支持。为了获得最佳性能，
强烈建议同时启用任何可用的、为你想使用的算法提供加速的平台特定 kconfig
选项。对任何"非默认"加密模式的支持通常也需要额外的 kconfig 选项。)

Below, some relevant options are listed by encryption mode.  Note,
acceleration options not listed below may be available for your
platform; refer to the kconfig menus.  File contents encryption can
also be configured to use inline encryption hardware instead of the
kernel crypto API (see `Inline encryption support`_); in that case,
the file contents mode doesn't need to supported in the kernel crypto
API, but the filenames mode still does.
(下面按加密模式列出了一些相关的选项。注意，你的平台可能还有下面未列出
的加速选项；请参考 kconfig 菜单。文件内容加密也可以配置为使用内联加密
硬件而不是内核加密 API(参见 `Inline encryption support`_)；在这种情况下，
文件内容模式不需要在内核加密 API 中受支持，但文件名模式仍然需要。)

- AES-256-XTS and AES-256-CBC-CTS
    - Recommended:
        - arm64: CONFIG_CRYPTO_AES_ARM64_CE_BLK
        - x86: CONFIG_CRYPTO_AES_NI_INTEL

  (推荐(Recommended)：
        - arm64: CONFIG_CRYPTO_AES_ARM64_CE_BLK
        - x86: CONFIG_CRYPTO_AES_NI_INTEL)

- AES-256-HCTR2
    - Mandatory:
        - CONFIG_CRYPTO_HCTR2
    - Recommended:
        - arm64: CONFIG_CRYPTO_AES_ARM64_CE_BLK
        - x86: CONFIG_CRYPTO_AES_NI_INTEL

  (强制(Mandatory)：
        - CONFIG_CRYPTO_HCTR2
    推荐(Recommended)：
        - arm64: CONFIG_CRYPTO_AES_ARM64_CE_BLK
        - x86: CONFIG_CRYPTO_AES_NI_INTEL)

- Adiantum
    - Mandatory:
        - CONFIG_CRYPTO_ADIANTUM

  (强制(Mandatory)：
        - CONFIG_CRYPTO_ADIANTUM)

- AES-128-CBC-ESSIV and AES-128-CBC-CTS:
    - Mandatory:
        - CONFIG_CRYPTO_ESSIV
        - CONFIG_CRYPTO_SHA256 or another SHA-256 implementation
    - Recommended:
        - AES-CBC acceleration

  (强制(Mandatory)：
        - CONFIG_CRYPTO_ESSIV
        - CONFIG_CRYPTO_SHA256 或其他 SHA-256 实现
    推荐(Recommended)：
        - AES-CBC 加速)

Contents encryption
(内容加密)
-------------------

For contents encryption, each file's contents is divided into "data
units".  Each data unit is encrypted independently.  The IV for each
data unit incorporates the zero-based index of the data unit within
the file.  This ensures that each data unit within a file is encrypted
differently, which is essential to prevent leaking information.
(对于内容加密，每个文件的内容被划分为"数据单元"(data unit)。每个数据
单元独立加密。每个数据单元的 IV 包含该数据单元在文件中以零为起点的
索引。这确保了文件内的每个数据单元都以不同的方式加密，这对于防止泄露
信息至关重要。)

Note: the encryption depending on the offset into the file means that
operations like "collapse range" and "insert range" that rearrange the
extent mapping of files are not supported on encrypted files.
(注意: 加密依赖于文件内的偏移量，这意味着像"折叠范围"(collapse range)
和"插入范围"(insert range)这类会重新排列文件区段映射的操作，在加密文件
上不受支持。)

There are two cases for the sizes of the data units:
(数据单元的大小有两种情况：)

* Fixed-size data units.  This is how all filesystems other than UBIFS
  work.  A file's data units are all the same size; the last data unit
  is zero-padded if needed.  By default, the data unit size is equal
  to the filesystem block size.  On some filesystems, users can select
  a sub-block data unit size via the ``log2_data_unit_size`` field of
  the encryption policy; see `FS_IOC_SET_ENCRYPTION_POLICY`_.
  (固定大小的数据单元。这是除 UBIFS 以外所有文件系统的工作方式。文件的
  数据单元大小都相同；最后一个数据单元在需要时做零填充。默认情况下，
  数据单元大小等于文件系统块大小。在某些文件系统上，用户可以通过加密
  策略的 ``log2_data_unit_size`` 字段选择小于块的数据单元大小；参见
  `FS_IOC_SET_ENCRYPTION_POLICY`_。)

* Variable-size data units.  This is what UBIFS does.  Each "UBIFS
  data node" is treated as a crypto data unit.  Each contains variable
  length, possibly compressed data, zero-padded to the next 16-byte
  boundary.  Users cannot select a sub-block data unit size on UBIFS.
  (可变大小的数据单元。这是 UBIFS 的做法。每个"UBIFS 数据节点"都被视为
  一个加密数据单元。每个节点包含可变长度的、可能已压缩的数据，并零填充
  到下一个 16 字节边界。用户不能在 UBIFS 上选择小于块的数据单元大小。)

In the case of compression + encryption, the compressed data is
encrypted.  UBIFS compression works as described above.  f2fs
compression works a bit differently; it compresses a number of
filesystem blocks into a smaller number of filesystem blocks.
Therefore a f2fs-compressed file still uses fixed-size data units, and
it is encrypted in a similar way to a file containing holes.
(在压缩+加密的情况下，被加密的是压缩后的数据。UBIFS 压缩的工作方式如
上所述。f2fs 压缩的工作方式略有不同；它将多个文件系统块压缩为更少数量的
文件系统块。因此，经 f2fs 压缩的文件仍然使用固定大小的数据单元，并且其
加密方式与包含空洞的文件类似。)

As mentioned in `Key hierarchy`_, the default encryption setting uses
per-file keys.  In this case, the IV for each data unit is simply the
index of the data unit in the file.  However, users can select an
encryption setting that does not use per-file keys.  For these, some
kind of file identifier is incorporated into the IVs as follows:
(如 `Key hierarchy`_ 中所述，默认加密设置使用每文件密钥。在这种情况下，
每个数据单元的 IV 就是该数据单元在文件中的索引。但是，用户可以选择不
使用每文件密钥的加密设置。对于这些设置，某种文件标识符会按如下方式
并入 IV：)

- With `DIRECT_KEY policies`_, the data unit index is placed in bits
  0-63 of the IV, and the file's nonce is placed in bits 64-191.
  (使用 `DIRECT_KEY policies`_ 时，数据单元索引被放置在 IV 的位
  0-63 中，文件的 nonce 被放置在位 64-191 中。)

- With `IV_INO_LBLK_64 policies`_, the data unit index is placed in
  bits 0-31 of the IV, and the file's inode number is placed in bits
  32-63.  This setting is only allowed when data unit indices and
  inode numbers fit in 32 bits.
  (使用 `IV_INO_LBLK_64 policies`_ 时，数据单元索引被放置在 IV 的位
  0-31 中，文件的 inode 编号被放置在位 32-63 中。仅当数据单元索引和
  inode 编号能装进 32 位时才允许此设置。)

- With `IV_INO_LBLK_32 policies`_, the file's inode number is hashed
  and added to the data unit index.  The resulting value is truncated
  to 32 bits and placed in bits 0-31 of the IV.  This setting is only
  allowed when data unit indices and inode numbers fit in 32 bits.
  (使用 `IV_INO_LBLK_32 policies`_ 时，文件的 inode 编号先被哈希，然后
  加到数据单元索引上。得到的值被截断为 32 位，并放置在 IV 的位 0-31 中。
  仅当数据单元索引和 inode 编号能装进 32 位时才允许此设置。)

The byte order of the IV is always little endian.
(IV 的字节序始终是小端序(little endian)。)

If the user selects FSCRYPT_MODE_AES_128_CBC for the contents mode, an
ESSIV layer is automatically included.  In this case, before the IV is
passed to AES-128-CBC, it is encrypted with AES-256 where the AES-256
key is the SHA-256 hash of the file's contents encryption key.
(如果用户为内容模式选择了 FSCRYPT_MODE_AES_128_CBC，则会自动包含一个
ESSIV 层。在这种情况下，在 IV 被传给 AES-128-CBC 之前，它会先用 AES-256
加密，其中 AES-256 密钥是文件内容加密密钥的 SHA-256 哈希值。)

Filenames encryption
(文件名加密)
--------------------

For filenames, each full filename is encrypted at once.  Because of
the requirements to retain support for efficient directory lookups and
filenames of up to 255 bytes, the same IV is used for every filename
in a directory.
(对于文件名，每个完整的文件名被一次性加密。由于需要继续支持高效的目录
查找以及最长 255 字节的文件名，目录中的每个文件名都使用相同的 IV。)

However, each encrypted directory still uses a unique key, or
alternatively has the file's nonce (for `DIRECT_KEY policies`_) or
inode number (for `IV_INO_LBLK_64 policies`_) included in the IVs.
Thus, IV reuse is limited to within a single directory.
(但是，每个加密目录仍然使用唯一的密钥，或者在 IV 中包含文件的 nonce
(对于 `DIRECT_KEY policies`_)或 inode 编号(对于 `IV_INO_LBLK_64
policies`_)。因此，IV 重用被限制在单个目录之内。)

With CBC-CTS, the IV reuse means that when the plaintext filenames share a
common prefix at least as long as the cipher block size (16 bytes for AES), the
corresponding encrypted filenames will also share a common prefix.  This is
undesirable.  Adiantum and HCTR2 do not have this weakness, as they are
wide-block encryption modes.
(对于 CBC-CTS，IV 重用意味着：当明文文件名共享一个至少与密码块大小一样
长(AES 为 16 字节)的公共前缀时，对应的加密文件名也将共享一个公共前缀。
这是不理想的。Adiantum 和 HCTR2 没有这个弱点，因为它们是宽块加密模式。)

All supported filenames encryption modes accept any plaintext length
>= 16 bytes; cipher block alignment is not required.  However,
filenames shorter than 16 bytes are NUL-padded to 16 bytes before
being encrypted.  In addition, to reduce leakage of filename lengths
via their ciphertexts, all filenames are NUL-padded to the next 4, 8,
16, or 32-byte boundary (configurable).  32 is recommended since this
provides the best confidentiality, at the cost of making directory
entries consume slightly more space.  Note that since NUL (``\0``) is
not otherwise a valid character in filenames, the padding will never
produce duplicate plaintexts.
(所有受支持的文件名加密模式都接受任何 >= 16 字节的明文长度；不需要密码
块对齐。但是，短于 16 字节的文件名在加密之前会被 NUL 填充到 16 字节。
此外，为了减少文件名长度通过其密文泄露，所有文件名都会被 NUL 填充到
下一个 4、8、16 或 32 字节边界(可配置)。推荐使用 32，因为它提供了最佳
的机密性，代价是目录项会消耗略多的空间。注意，由于 NUL(``\0``)在文件名
中本来就不是有效字符，因此填充永远不会产生重复的明文。)

Symbolic link targets are considered a type of filename and are
encrypted in the same way as filenames in directory entries, except
that IV reuse is not a problem as each symlink has its own inode.
(符号链接的目标被视为一种文件名，并以与目录项中文件名相同的方式加密，
只是 IV 重用不是问题，因为每个符号链接都有自己的 inode。)

User API
(用户 API)
========

Setting an encryption policy
(设置加密策略)
----------------------------

FS_IOC_SET_ENCRYPTION_POLICY
(FS_IOC_SET_ENCRYPTION_POLICY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The FS_IOC_SET_ENCRYPTION_POLICY ioctl sets an encryption policy on an
empty directory or verifies that a directory or regular file already
has the specified encryption policy.  It takes in a pointer to
struct fscrypt_policy_v1 or struct fscrypt_policy_v2, defined as
follows::
(FS_IOC_SET_ENCRYPTION_POLICY ioctl 在空目录上设置加密策略，或验证目录
或普通文件是否已具有指定的加密策略。它接受一个指向 struct
fscrypt_policy_v1 或 struct fscrypt_policy_v2 的指针，其定义如下：)

    #define FSCRYPT_POLICY_V1               0
    #define FSCRYPT_KEY_DESCRIPTOR_SIZE     8
    struct fscrypt_policy_v1 {
            __u8 version;
            __u8 contents_encryption_mode;
            __u8 filenames_encryption_mode;
            __u8 flags;
            __u8 master_key_descriptor[FSCRYPT_KEY_DESCRIPTOR_SIZE];
    };
    #define fscrypt_policy  fscrypt_policy_v1

    #define FSCRYPT_POLICY_V2               2
    #define FSCRYPT_KEY_IDENTIFIER_SIZE     16
    struct fscrypt_policy_v2 {
            __u8 version;
            __u8 contents_encryption_mode;
            __u8 filenames_encryption_mode;
            __u8 flags;
            __u8 log2_data_unit_size;
            __u8 __reserved[3];
            __u8 master_key_identifier[FSCRYPT_KEY_IDENTIFIER_SIZE];
    };

This structure must be initialized as follows:
(该结构必须按如下方式初始化：)

- ``version`` must be FSCRYPT_POLICY_V1 (0) if
  struct fscrypt_policy_v1 is used or FSCRYPT_POLICY_V2 (2) if
  struct fscrypt_policy_v2 is used. (Note: we refer to the original
  policy version as "v1", though its version code is really 0.)
  For new encrypted directories, use v2 policies.
  (``version``：如果使用 struct fscrypt_policy_v1，则必须是
  FSCRYPT_POLICY_V1 (0)；如果使用 struct fscrypt_policy_v2，则必须是
  FSCRYPT_POLICY_V2 (2)。(注意: 我们将最初的策略版本称为 "v1"，尽管其
  版本号实际上是 0。)对于新的加密目录，请使用 v2 策略。)

- ``contents_encryption_mode`` and ``filenames_encryption_mode`` must
  be set to constants from ``<linux/fscrypt.h>`` which identify the
  encryption modes to use.  If unsure, use FSCRYPT_MODE_AES_256_XTS
  (1) for ``contents_encryption_mode`` and FSCRYPT_MODE_AES_256_CTS
  (4) for ``filenames_encryption_mode``.  For details, see `Encryption
  modes and usage`_.
  (``contents_encryption_mode`` 和 ``filenames_encryption_mode``：必须
  设置为 ``<linux/fscrypt.h>`` 中标识要使用的加密模式的常量。如果不确定，
  ``contents_encryption_mode`` 使用 FSCRYPT_MODE_AES_256_XTS (1)，
  ``filenames_encryption_mode`` 使用 FSCRYPT_MODE_AES_256_CTS (4)。
  详情参见 `Encryption modes and usage`_。)

  v1 encryption policies only support three combinations of modes:
  (FSCRYPT_MODE_AES_256_XTS, FSCRYPT_MODE_AES_256_CTS),
  (FSCRYPT_MODE_AES_128_CBC, FSCRYPT_MODE_AES_128_CTS), and
  (FSCRYPT_MODE_ADIANTUM, FSCRYPT_MODE_ADIANTUM).  v2 policies support
  all combinations documented in `Supported modes`_.
  (v1 加密策略仅支持三种模式组合：
  (FSCRYPT_MODE_AES_256_XTS, FSCRYPT_MODE_AES_256_CTS)、
  (FSCRYPT_MODE_AES_128_CBC, FSCRYPT_MODE_AES_128_CTS) 和
  (FSCRYPT_MODE_ADIANTUM, FSCRYPT_MODE_ADIANTUM)。v2 策略支持
  `Supported modes`_ 中记录的所有组合。)

- ``flags`` contains optional flags from ``<linux/fscrypt.h>``:
  (``flags``：包含来自 ``<linux/fscrypt.h>`` 的可选标志：)

  - FSCRYPT_POLICY_FLAGS_PAD_*: The amount of NUL padding to use when
    encrypting filenames.  If unsure, use FSCRYPT_POLICY_FLAGS_PAD_32
    (0x3).
    (FSCRYPT_POLICY_FLAGS_PAD_*：加密文件名时使用的 NUL 填充值。如果
    不确定，使用 FSCRYPT_POLICY_FLAGS_PAD_32 (0x3)。)
  - FSCRYPT_POLICY_FLAG_DIRECT_KEY: See `DIRECT_KEY policies`_.
    (FSCRYPT_POLICY_FLAG_DIRECT_KEY：参见 `DIRECT_KEY policies`_。)
  - FSCRYPT_POLICY_FLAG_IV_INO_LBLK_64: See `IV_INO_LBLK_64
    policies`_.
    (FSCRYPT_POLICY_FLAG_IV_INO_LBLK_64：参见 `IV_INO_LBLK_64
    policies`_。)
  - FSCRYPT_POLICY_FLAG_IV_INO_LBLK_32: See `IV_INO_LBLK_32
    policies`_.
    (FSCRYPT_POLICY_FLAG_IV_INO_LBLK_32：参见 `IV_INO_LBLK_32
    policies`_。)

  v1 encryption policies only support the PAD_* and DIRECT_KEY flags.
  The other flags are only supported by v2 encryption policies.
  (v1 加密策略仅支持 PAD_* 和 DIRECT_KEY 标志。其他标志仅受 v2 加密
  策略支持。)

  The DIRECT_KEY, IV_INO_LBLK_64, and IV_INO_LBLK_32 flags are
  mutually exclusive.
  (DIRECT_KEY、IV_INO_LBLK_64 和 IV_INO_LBLK_32 标志是互斥的。)

- ``log2_data_unit_size`` is the log2 of the data unit size in bytes,
  or 0 to select the default data unit size.  The data unit size is
  the granularity of file contents encryption.  For example, setting
  ``log2_data_unit_size`` to 12 causes file contents be passed to the
  underlying encryption algorithm (such as AES-256-XTS) in 4096-byte
  data units, each with its own IV.
  (``log2_data_unit_size``：数据单元大小(以字节为单位)的 log2 值；为 0
  表示选择默认的数据单元大小。数据单元大小是文件内容加密的粒度。例如，
  将 ``log2_data_unit_size`` 设置为 12 会使文件内容以 4096 字节的数据
  单元传给底层加密算法(例如 AES-256-XTS)，每个数据单元都有自己的 IV。)

  Not all filesystems support setting ``log2_data_unit_size``.  ext4
  and f2fs support it since Linux v6.7.  On filesystems that support
  it, the supported nonzero values are 9 through the log2 of the
  filesystem block size, inclusively.  The default value of 0 selects
  the filesystem block size.
  (并非所有文件系统都支持设置 ``log2_data_unit_size``。ext4 和 f2fs 自
  Linux v6.7 起支持它。在支持它的文件系统上，受支持的非零值是从 9 到
  文件系统块大小的 log2 值(含两端)。默认值 0 选择文件系统块大小。)

  The main use case for ``log2_data_unit_size`` is for selecting a
  data unit size smaller than the filesystem block size for
  compatibility with inline encryption hardware that only supports
  smaller data unit sizes.  ``/sys/block/$disk/queue/crypto/`` may be
  useful for checking which data unit sizes are supported by a
  particular system's inline encryption hardware.
  (``log2_data_unit_size`` 的主要用例是选择小于文件系统块大小的数据单元
  大小，以兼容只支持更小数据单元大小的内联加密硬件。
  ``/sys/block/$disk/queue/crypto/`` 可用于查看特定系统的内联加密硬件
  支持哪些数据单元大小。)

  Leave this field zeroed unless you are certain you need it.  Using
  an unnecessarily small data unit size reduces performance.
  (除非确定需要，否则请将此字段保持为零。使用不必要地小的数据单元大小
  会降低性能。)

- For v2 encryption policies, ``__reserved`` must be zeroed.
  (对于 v2 加密策略，``__reserved`` 必须清零。)

- For v1 encryption policies, ``master_key_descriptor`` specifies how
  to find the master key in a keyring; see `Adding keys`_.  It is up
  to userspace to choose a unique ``master_key_descriptor`` for each
  master key.  The e4crypt and fscrypt tools use the first 8 bytes of
  ``SHA-512(SHA-512(master_key))``, but this particular scheme is not
  required.  Also, the master key need not be in the keyring yet when
  FS_IOC_SET_ENCRYPTION_POLICY is executed.  However, it must be added
  before any files can be created in the encrypted directory.
  (对于 v1 加密策略，``master_key_descriptor`` 指定如何在密钥环中找到
  主密钥；参见 `Adding keys`_。由用户空间为每个主密钥选择唯一的
  ``master_key_descriptor``。e4crypt 和 fscrypt 工具使用
  ``SHA-512(SHA-512(master_key))`` 的前 8 个字节，但并不要求使用这个
  特定方案。此外，在执行 FS_IOC_SET_ENCRYPTION_POLICY 时，主密钥还不必
  已经在密钥环中。但是，在可以在加密目录中创建任何文件之前，它必须已被
  添加。)

  For v2 encryption policies, ``master_key_descriptor`` has been
  replaced with ``master_key_identifier``, which is longer and cannot
  be arbitrarily chosen.  Instead, the key must first be added using
  `FS_IOC_ADD_ENCRYPTION_KEY`_.  Then, the ``key_spec.u.identifier``
  the kernel returned in the struct fscrypt_add_key_arg must
  be used as the ``master_key_identifier`` in
  struct fscrypt_policy_v2.
  (对于 v2 加密策略，``master_key_descriptor`` 已被
  ``master_key_identifier`` 取代，后者更长且不能任意选择。相反，必须先
  使用 `FS_IOC_ADD_ENCRYPTION_KEY`_ 添加密钥。然后，必须将内核在 struct
  fscrypt_add_key_arg 中返回的 ``key_spec.u.identifier`` 用作 struct
  fscrypt_policy_v2 中的 ``master_key_identifier``。)

If the file is not yet encrypted, then FS_IOC_SET_ENCRYPTION_POLICY
verifies that the file is an empty directory.  If so, the specified
encryption policy is assigned to the directory, turning it into an
encrypted directory.  After that, and after providing the
corresponding master key as described in `Adding keys`_, all regular
files, directories (recursively), and symlinks created in the
directory will be encrypted, inheriting the same encryption policy.
The filenames in the directory's entries will be encrypted as well.
(如果文件尚未加密，FS_IOC_SET_ENCRYPTION_POLICY 会验证该文件是一个空
目录。如果是，指定的加密策略将被赋予该目录，将其变成加密目录。之后，
在按照 `Adding keys`_ 中所述提供相应的主密钥之后，在该目录中创建的所有
普通文件、目录(递归地)和符号链接都将被加密，并继承相同的加密策略。
目录各项中的文件名也将被加密。)

Alternatively, if the file is already encrypted, then
FS_IOC_SET_ENCRYPTION_POLICY validates that the specified encryption
policy exactly matches the actual one.  If they match, then the ioctl
returns 0.  Otherwise, it fails with EEXIST.  This works on both
regular files and directories, including nonempty directories.
(或者，如果文件已经加密，FS_IOC_SET_ENCRYPTION_POLICY 会验证指定的加密
策略与实际策略完全匹配。如果匹配，ioctl 返回 0。否则，它以 EEXIST 失败。
这对普通文件和目录都有效，包括非空目录。)

When a v2 encryption policy is assigned to a directory, it is also
required that either the specified key has been added by the current
user or that the caller has CAP_FOWNER in the initial user namespace.
(This is needed to prevent a user from encrypting their data with
another user's key.)  The key must remain added while
FS_IOC_SET_ENCRYPTION_POLICY is executing.  However, if the new
encrypted directory does not need to be accessed immediately, then the
key can be removed right away afterwards.
(当 v2 加密策略被赋予一个目录时，还要求：指定的密钥已被当前用户添加，
或者调用者在初始用户命名空间中拥有 CAP_FOWNER。(这是为了防止用户使用
其他用户的密钥加密自己的数据。)在 FS_IOC_SET_ENCRYPTION_POLICY 执行
期间，密钥必须保持已添加状态。但是，如果新的加密目录不需要立即访问，
则之后可以立即移除密钥。)

Note that the ext4 filesystem does not allow the root directory to be
encrypted, even if it is empty.  Users who want to encrypt an entire
filesystem with one key should consider using dm-crypt instead.
(注意，ext4 文件系统不允许加密根目录，即使它是空的。想要用一个密钥加密
整个文件系统的用户应考虑改用 dm-crypt。)

FS_IOC_SET_ENCRYPTION_POLICY can fail with the following errors:
(FS_IOC_SET_ENCRYPTION_POLICY 可能因以下错误而失败：)

- ``EACCES``: the file is not owned by the process's uid, nor does the
  process have the CAP_FOWNER capability in a namespace with the file
  owner's uid mapped
  (``EACCES``：该文件不属于进程的 uid，而且进程在映射了文件所有者 uid 的
  命名空间中也不具备 CAP_FOWNER 能力)
- ``EEXIST``: the file is already encrypted with an encryption policy
  different from the one specified
  (``EEXIST``：该文件已经用不同于所指定策略的加密策略加密)
- ``EINVAL``: an invalid encryption policy was specified (invalid
  version, mode(s), or flags; or reserved bits were set); or a v1
  encryption policy was specified but the directory has the casefold
  flag enabled (casefolding is incompatible with v1 policies).
  (``EINVAL``：指定了无效的加密策略(无效的版本、模式或标志；或设置了
  保留位)；或者指定了 v1 加密策略但目录启用了大小写折叠标志(大小写折叠
  与 v1 策略不兼容)。)
- ``ENOKEY``: a v2 encryption policy was specified, but the key with
  the specified ``master_key_identifier`` has not been added, nor does
  the process have the CAP_FOWNER capability in the initial user
  namespace
  (``ENOKEY``：指定了 v2 加密策略，但具有所指定
  ``master_key_identifier`` 的密钥尚未添加，而且进程在初始用户命名空间
  中也不具备 CAP_FOWNER 能力)
- ``ENOTDIR``: the file is unencrypted and is a regular file, not a
  directory
  (``ENOTDIR``：该文件未加密，且是普通文件而不是目录)
- ``ENOTEMPTY``: the file is unencrypted and is a nonempty directory
  (``ENOTEMPTY``：该文件未加密，且是非空目录)
- ``ENOTTY``: this type of filesystem does not implement encryption
  (``ENOTTY``：此类型的文件系统未实现加密)
- ``EOPNOTSUPP``: the kernel was not configured with encryption
  support for filesystems, or the filesystem superblock has not
  had encryption enabled on it.  (For example, to use encryption on an
  ext4 filesystem, CONFIG_FS_ENCRYPTION must be enabled in the
  kernel config, and the superblock must have had the "encrypt"
  feature flag enabled using ``tune2fs -O encrypt`` or ``mkfs.ext4 -O
  encrypt``.)
  (``EOPNOTSUPP``：内核配置时未启用文件系统加密支持，或文件系统超级块
  尚未启用加密。(例如，要在 ext4 文件系统上使用加密，必须在内核配置中
  启用 CONFIG_FS_ENCRYPTION，并且超级块必须已通过 ``tune2fs -O
  encrypt`` 或 ``mkfs.ext4 -O encrypt`` 启用 "encrypt" 特性标志。))
- ``EPERM``: this directory may not be encrypted, e.g. because it is
  the root directory of an ext4 filesystem
  (``EPERM``：此目录不允许被加密，例如因为它是 ext4 文件系统的根目录)
- ``EROFS``: the filesystem is readonly
  (``EROFS``：文件系统是只读的)

Getting an encryption policy
(获取加密策略)
----------------------------

Two ioctls are available to get a file's encryption policy:
(有两个 ioctl 可用于获取文件的加密策略：)

- `FS_IOC_GET_ENCRYPTION_POLICY_EX`_
- `FS_IOC_GET_ENCRYPTION_POLICY`_

The extended (_EX) version of the ioctl is more general and is
recommended to use when possible.  However, on older kernels only the
original ioctl is available.  Applications should try the extended
version, and if it fails with ENOTTY fall back to the original
version.
(扩展(_EX)版本的 ioctl 更通用，建议在可能的情况下使用它。但是，在较旧
的内核上只有最初的 ioctl 可用。应用程序应先尝试扩展版本，如果它以
ENOTTY 失败，则回退到最初的版本。)

FS_IOC_GET_ENCRYPTION_POLICY_EX
(FS_IOC_GET_ENCRYPTION_POLICY_EX)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The FS_IOC_GET_ENCRYPTION_POLICY_EX ioctl retrieves the encryption
policy, if any, for a directory or regular file.  No additional
permissions are required beyond the ability to open the file.  It
takes in a pointer to struct fscrypt_get_policy_ex_arg,
defined as follows::
(FS_IOC_GET_ENCRYPTION_POLICY_EX ioctl 检索目录或普通文件的加密策略(如果
有的话)。除了打开文件的能力之外，不需要额外的权限。它接受一个指向
struct fscrypt_get_policy_ex_arg 的指针，其定义如下：)

    struct fscrypt_get_policy_ex_arg {
            __u64 policy_size; /* input/output */
            union {
                    __u8 version;
                    struct fscrypt_policy_v1 v1;
                    struct fscrypt_policy_v2 v2;
            } policy; /* output */
    };

The caller must initialize ``policy_size`` to the size available for
the policy struct, i.e. ``sizeof(arg.policy)``.
(调用者必须将 ``policy_size`` 初始化为可用于该策略结构的大小，即
``sizeof(arg.policy)``。)

On success, the policy struct is returned in ``policy``, and its
actual size is returned in ``policy_size``.  ``policy.version`` should
be checked to determine the version of policy returned.  Note that the
version code for the "v1" policy is actually 0 (FSCRYPT_POLICY_V1).
(成功时，策略结构在 ``policy`` 中返回，其实际大小在 ``policy_size`` 中
返回。应检查 ``policy.version`` 来确定返回的策略版本。注意，"v1" 策略
的版本号实际上是 0 (FSCRYPT_POLICY_V1)。)

FS_IOC_GET_ENCRYPTION_POLICY_EX can fail with the following errors:
(FS_IOC_GET_ENCRYPTION_POLICY_EX 可能因以下错误而失败：)

- ``EINVAL``: the file is encrypted, but it uses an unrecognized
  encryption policy version
  (``EINVAL``：文件已加密，但它使用了无法识别的加密策略版本)
- ``ENODATA``: the file is not encrypted
  (``ENODATA``：文件未加密)
- ``ENOTTY``: this type of filesystem does not implement encryption,
  or this kernel is too old to support FS_IOC_GET_ENCRYPTION_POLICY_EX
  (try FS_IOC_GET_ENCRYPTION_POLICY instead)
  (``ENOTTY``：此类型的文件系统未实现加密，或此内核太旧而不支持
  FS_IOC_GET_ENCRYPTION_POLICY_EX(请改用 FS_IOC_GET_ENCRYPTION_POLICY))
- ``EOPNOTSUPP``: the kernel was not configured with encryption
  support for this filesystem, or the filesystem superblock has not
  had encryption enabled on it
  (``EOPNOTSUPP``：内核配置时未启用对此文件系统的加密支持，或文件系统
  超级块尚未在其上启用加密)
- ``EOVERFLOW``: the file is encrypted and uses a recognized
  encryption policy version, but the policy struct does not fit into
  the provided buffer
  (``EOVERFLOW``：文件已加密并使用了可识别的加密策略版本，但策略结构
  放不进所提供的缓冲中)

Note: if you only need to know whether a file is encrypted or not, on
most filesystems it is also possible to use the FS_IOC_GETFLAGS ioctl
and check for FS_ENCRYPT_FL, or to use the statx() system call and
check for STATX_ATTR_ENCRYPTED in stx_attributes.
(注意: 如果只需要知道文件是否加密，在大多数文件系统上也可以使用
FS_IOC_GETFLAGS ioctl 并检查 FS_ENCRYPT_FL，或者使用 statx() 系统调用
并检查 stx_attributes 中的 STATX_ATTR_ENCRYPTED。)

FS_IOC_GET_ENCRYPTION_POLICY
(FS_IOC_GET_ENCRYPTION_POLICY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The FS_IOC_GET_ENCRYPTION_POLICY ioctl can also retrieve the
encryption policy, if any, for a directory or regular file.  However,
unlike `FS_IOC_GET_ENCRYPTION_POLICY_EX`_,
FS_IOC_GET_ENCRYPTION_POLICY only supports the original policy
version.  It takes in a pointer directly to struct fscrypt_policy_v1
rather than struct fscrypt_get_policy_ex_arg.
(FS_IOC_GET_ENCRYPTION_POLICY ioctl 也可以检索目录或普通文件的加密策略
(如果有的话)。但是，与 `FS_IOC_GET_ENCRYPTION_POLICY_EX`_ 不同，
FS_IOC_GET_ENCRYPTION_POLICY 只支持最初的策略版本。它接受一个直接指向
struct fscrypt_policy_v1 的指针，而不是 struct
fscrypt_get_policy_ex_arg。)

The error codes for FS_IOC_GET_ENCRYPTION_POLICY are the same as those
for FS_IOC_GET_ENCRYPTION_POLICY_EX, except that
FS_IOC_GET_ENCRYPTION_POLICY also returns ``EINVAL`` if the file is
encrypted using a newer encryption policy version.
(FS_IOC_GET_ENCRYPTION_POLICY 的错误码与 FS_IOC_GET_ENCRYPTION_POLICY_EX
的相同，不同之处在于：如果文件是使用较新的加密策略版本加密的，
FS_IOC_GET_ENCRYPTION_POLICY 还会返回 ``EINVAL``。)

Getting the per-filesystem salt
(获取每文件系统的盐值)
-------------------------------

Some filesystems, such as ext4 and F2FS, also support the deprecated
ioctl FS_IOC_GET_ENCRYPTION_PWSALT.  This ioctl retrieves a randomly
generated 16-byte value stored in the filesystem superblock.  This
value is intended to used as a salt when deriving an encryption key
from a passphrase or other low-entropy user credential.
(一些文件系统，例如 ext4 和 F2FS，还支持已弃用的 ioctl
FS_IOC_GET_ENCRYPTION_PWSALT。此 ioctl 检索存储在文件系统超级块中的
一个随机生成的 16 字节值。当从口令或其他低熵用户凭据派生加密密钥时，
该值旨在用作盐值(salt)。)

FS_IOC_GET_ENCRYPTION_PWSALT is deprecated.  Instead, prefer to
generate and manage any needed salt(s) in userspace.
(FS_IOC_GET_ENCRYPTION_PWSALT 已被弃用。相反，建议在用户空间中生成并
管理所需的盐值。)

Getting a file's encryption nonce
(获取文件的加密 nonce)
---------------------------------

Since Linux v5.7, the ioctl FS_IOC_GET_ENCRYPTION_NONCE is supported.
On encrypted files and directories it gets the inode's 16-byte nonce.
On unencrypted files and directories, it fails with ENODATA.
(自 Linux v5.7 起，支持 ioctl FS_IOC_GET_ENCRYPTION_NONCE。在加密的文件
和目录上，它获取 inode 的 16 字节 nonce。在未加密的文件和目录上，它以
ENODATA 失败。)

This ioctl can be useful for automated tests which verify that the
encryption is being done correctly.  It is not needed for normal use
of fscrypt.
(此 ioctl 对于验证加密是否正确完成的自动化测试很有用。正常使用 fscrypt
并不需要它。)

Adding keys
(添加密钥)
-----------

FS_IOC_ADD_ENCRYPTION_KEY
(FS_IOC_ADD_ENCRYPTION_KEY)
~~~~~~~~~~~~~~~~~~~~~~~~~

The FS_IOC_ADD_ENCRYPTION_KEY ioctl adds a master encryption key to
the filesystem, making all files on the filesystem which were
encrypted using that key appear "unlocked", i.e. in plaintext form.
It can be executed on any file or directory on the target filesystem,
but using the filesystem's root directory is recommended.  It takes in
a pointer to struct fscrypt_add_key_arg, defined as follows::
(FS_IOC_ADD_ENCRYPTION_KEY ioctl 将一个主加密密钥添加到文件系统，使该
文件系统上所有使用该密钥加密的文件呈现"已解锁"状态，即以明文形式呈现。
它可以在目标文件系统上的任何文件或目录上执行，但建议使用文件系统的根
目录。它接受一个指向 struct fscrypt_add_key_arg 的指针，其定义如下：)

    struct fscrypt_add_key_arg {
            struct fscrypt_key_specifier key_spec;
            __u32 raw_size;
            __u32 key_id;
    #define FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED 0x00000001
            __u32 flags;
            __u32 __reserved[7];
            __u8 raw[];
    };

    #define FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR        1
    #define FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER        2

    struct fscrypt_key_specifier {
            __u32 type;     /* one of FSCRYPT_KEY_SPEC_TYPE_* */
            __u32 __reserved;
            union {
                    __u8 __reserved[32]; /* reserve some extra space */
                    __u8 descriptor[FSCRYPT_KEY_DESCRIPTOR_SIZE];
                    __u8 identifier[FSCRYPT_KEY_IDENTIFIER_SIZE];
            } u;
    };

    struct fscrypt_provisioning_key_payload {
            __u32 type;
            __u32 flags;
            __u8 raw[];
    };

struct fscrypt_add_key_arg must be zeroed, then initialized
as follows:
(struct fscrypt_add_key_arg 必须先清零，然后按如下方式初始化：)

- If the key is being added for use by v1 encryption policies, then
  ``key_spec.type`` must contain FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR, and
  ``key_spec.u.descriptor`` must contain the descriptor of the key
  being added, corresponding to the value in the
  ``master_key_descriptor`` field of struct fscrypt_policy_v1.
  To add this type of key, the calling process must have the
  CAP_SYS_ADMIN capability in the initial user namespace.
  (如果添加密钥是为了供 v1 加密策略使用，则 ``key_spec.type`` 必须包含
  FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR，并且 ``key_spec.u.descriptor`` 必须
  包含所添加密钥的描述符(descriptor)，对应 struct fscrypt_policy_v1 的
  ``master_key_descriptor`` 字段中的值。要添加这种类型的密钥，调用进程
  必须在初始用户命名空间中拥有 CAP_SYS_ADMIN 能力。)

  Alternatively, if the key is being added for use by v2 encryption
  policies, then ``key_spec.type`` must contain
  FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER, and ``key_spec.u.identifier`` is
  an *output* field which the kernel fills in with a cryptographic
  hash of the key.  To add this type of key, the calling process does
  not need any privileges.  However, the number of keys that can be
  added is limited by the user's quota for the keyrings service (see
  ``Documentation/security/keys/core.rst``).
  (或者，如果添加密钥是为了供 v2 加密策略使用，则 ``key_spec.type`` 必须
  包含 FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER，并且 ``key_spec.u.identifier``
  是一个*输出*字段，内核会用该密钥的密码学哈希值填充它。要添加这种类型
  的密钥，调用进程不需要任何特权。但是，可以添加的密钥数量受用户在
  keyrings 服务上的配额限制(参见
  ``Documentation/security/keys/core.rst``)。)

- ``raw_size`` must be the size of the ``raw`` key provided, in bytes.
  Alternatively, if ``key_id`` is nonzero, this field must be 0, since
  in that case the size is implied by the specified Linux keyring key.
  (``raw_size``：必须是所提供的 ``raw`` 密钥的大小，以字节为单位。或者，
  如果 ``key_id`` 非零，则此字段必须为 0，因为在这种情况下，大小由所
  指定的 Linux 密钥环密钥隐含。)

- ``key_id`` is 0 if the key is given directly in the ``raw`` field.
  Otherwise ``key_id`` is the ID of a Linux keyring key of type
  "fscrypt-provisioning" whose payload is struct
  fscrypt_provisioning_key_payload whose ``raw`` field contains the
  key, whose ``type`` field matches ``key_spec.type``, and whose
  ``flags`` field matches ``flags``.  Since ``raw`` is
  variable-length, the total size of this key's payload must be
  ``sizeof(struct fscrypt_provisioning_key_payload)`` plus the number
  of key bytes.  The process must have Search permission on this key.
  (``key_id``：如果密钥直接在 ``raw`` 字段中给出，则为 0。否则
  ``key_id`` 是类型为 "fscrypt-provisioning" 的 Linux 密钥环密钥的 ID，
  其载荷(payload)是 struct fscrypt_provisioning_key_payload，其 ``raw``
  字段包含该密钥，其 ``type`` 字段匹配 ``key_spec.type``，并且其
  ``flags`` 字段匹配 ``flags``。由于 ``raw`` 是变长的，此密钥载荷的总
  大小必须是 ``sizeof(struct fscrypt_provisioning_key_payload)`` 加上
  密钥的字节数。进程必须对该密钥拥有 Search 权限。)

  Most users should leave this 0 and specify the key directly.  The
  support for specifying a Linux keyring key is intended mainly to
  allow re-adding keys after a filesystem is unmounted and re-mounted,
  without having to store the keys in userspace memory.
  (大多数用户应将其保留为 0 并直接指定密钥。支持指定 Linux 密钥环密钥
  主要是为了在文件系统被卸载并重新挂载之后能够重新添加密钥，而无需将
  密钥存储在用户空间内存中。)

- ``flags`` contains optional flags from ``<linux/fscrypt.h>``:
  (``flags``：包含来自 ``<linux/fscrypt.h>`` 的可选标志：)

  - FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED: This denotes that the key is a
    hardware-wrapped key.  See `Hardware-wrapped keys`_.  This flag
    can't be used if FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR is used.
    (FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED：表示该密钥是硬件封装密钥。参见
    `Hardware-wrapped keys`_。如果使用了 FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR
    则不能使用此标志。)

- ``raw`` is a variable-length field which must contain the actual
  key, ``raw_size`` bytes long.  Alternatively, if ``key_id`` is
  nonzero, then this field is unused.  Note that despite being named
  ``raw``, if FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED is specified then it
  will contain a wrapped key, not a raw key.
  (``raw``：一个变长字段，必须包含实际密钥，长度为 ``raw_size`` 字节。
  或者，如果 ``key_id`` 非零，则此字段不被使用。注意，尽管名为 ``raw``，
  如果指定了 FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED，则它将包含一个封装密钥，
  而不是原始密钥。)

For v2 policy keys, the kernel keeps track of which user (identified
by effective user ID) added the key, and only allows the key to be
removed by that user --- or by "root", if they use
`FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS`_.
(对于 v2 策略密钥，内核会记录是哪个用户(以有效用户 ID 标识)添加了密钥，
并且只允许该用户移除密钥——或者如果 "root" 使用
`FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS`_，则由 "root" 移除。)

However, if another user has added the key, it may be desirable to
prevent that other user from unexpectedly removing it.  Therefore,
FS_IOC_ADD_ENCRYPTION_KEY may also be used to add a v2 policy key
*again*, even if it's already added by other user(s).  In this case,
FS_IOC_ADD_ENCRYPTION_KEY will just install a claim to the key for the
current user, rather than actually add the key again (but the key must
still be provided, as a proof of knowledge).
(但是，如果另一个用户已添加该密钥，可能希望防止该用户意外地移除它。
因此，FS_IOC_ADD_ENCRYPTION_KEY 也可以用来*再次*添加 v2 策略密钥，即使
它已被其他用户添加。在这种情况下，FS_IOC_ADD_ENCRYPTION_KEY 只会为当前
用户安装对该密钥的一个声明(claim)，而不是真正再次添加密钥(但仍必须提供
该密钥，作为知晓密钥的证明)。)

FS_IOC_ADD_ENCRYPTION_KEY returns 0 if either the key or a claim to
the key was either added or already exists.
(如果密钥或对密钥的声明被添加或已存在，FS_IOC_ADD_ENCRYPTION_KEY 返回
0。)

FS_IOC_ADD_ENCRYPTION_KEY can fail with the following errors:
(FS_IOC_ADD_ENCRYPTION_KEY 可能因以下错误而失败：)

- ``EACCES``: FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR was specified, but the
  caller does not have the CAP_SYS_ADMIN capability in the initial
  user namespace; or the key was specified by Linux key ID but the
  process lacks Search permission on the key.
  (``EACCES``：指定了 FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR，但调用者在初始
  用户命名空间中不具备 CAP_SYS_ADMIN 能力；或者密钥是通过 Linux 密钥 ID
  指定的，但进程对该密钥缺少 Search 权限。)
- ``EBADMSG``: invalid hardware-wrapped key
  (``EBADMSG``：无效的硬件封装密钥)
- ``EDQUOT``: the key quota for this user would be exceeded by adding
  the key
  (``EDQUOT``：添加该密钥将超过此用户的密钥配额)
- ``EINVAL``: invalid key size or key specifier type, or reserved bits
  were set
  (``EINVAL``：无效的密钥大小或密钥说明符类型，或设置了保留位)
- ``EKEYREJECTED``: the key was specified by Linux key ID, but the key
  has the wrong type
  (``EKEYREJECTED``：密钥是通过 Linux 密钥 ID 指定的，但该密钥的类型
  错误)
- ``ENOKEY``: the key was specified by Linux key ID, but no key exists
  with that ID
  (``ENOKEY``：密钥是通过 Linux 密钥 ID 指定的，但不存在具有该 ID 的
  密钥)
- ``ENOTTY``: this type of filesystem does not implement encryption
  (``ENOTTY``：此类型的文件系统未实现加密)
- ``EOPNOTSUPP``: the kernel was not configured with encryption
  support for this filesystem, or the filesystem superblock has not
  had encryption enabled on it; or a hardware wrapped key was specified
  but the filesystem does not support inline encryption or the hardware
  does not support hardware-wrapped keys
  (``EOPNOTSUPP``：内核配置时未启用对此文件系统的加密支持，或文件系统
  超级块尚未在其上启用加密；或者指定了硬件封装密钥，但文件系统不支持
  内联加密，或硬件不支持硬件封装密钥)

Legacy method
(传统方法)
~~~~~~~~~~~~~

For v1 encryption policies, a master encryption key can also be
provided by adding it to a process-subscribed keyring, e.g. to a
session keyring, or to a user keyring if the user keyring is linked
into the session keyring.
(对于 v1 加密策略，也可以通过将主加密密钥添加到进程订阅的密钥环中来
提供它，例如添加到会话密钥环(session keyring)，或者如果用户密钥环
(user keyring)已链接到会话密钥环，则添加到用户密钥环。)

This method is deprecated (and not supported for v2 encryption
policies) for several reasons.  First, it cannot be used in
combination with FS_IOC_REMOVE_ENCRYPTION_KEY (see `Removing keys`_),
so for removing a key a workaround such as keyctl_unlink() in
combination with ``sync; echo 2 > /proc/sys/vm/drop_caches`` would
have to be used.  Second, it doesn't match the fact that the
locked/unlocked status of encrypted files (i.e. whether they appear to
be in plaintext form or in ciphertext form) is global.  This mismatch
has caused much confusion as well as real problems when processes
running under different UIDs, such as a ``sudo`` command, need to
access encrypted files.
(出于几个原因，此方法已被弃用(并且不受 v2 加密策略支持)。首先，它不能
与 FS_IOC_REMOVE_ENCRYPTION_KEY 结合使用(参见 `Removing keys`_)，因此
要移除密钥，必须使用诸如 keyctl_unlink() 结合
``sync; echo 2 > /proc/sys/vm/drop_caches`` 之类的变通方法。其次，它与
以下事实不匹配：加密文件的锁定/解锁状态(即它们呈现为明文形式还是密文
形式)是全局的。当在不同 UID 下运行的进程(例如 ``sudo`` 命令)需要访问
加密文件时，这种不匹配已经引起了许多混淆以及实际问题。)

Nevertheless, to add a key to one of the process-subscribed keyrings,
the add_key() system call can be used (see:
``Documentation/security/keys/core.rst``).  The key type must be
"logon"; keys of this type are kept in kernel memory and cannot be
read back by userspace.  The key description must be "fscrypt:"
followed by the 16-character lower case hex representation of the
``master_key_descriptor`` that was set in the encryption policy.  The
key payload must conform to the following structure::
(尽管如此，要将密钥添加到进程订阅的密钥环之一，可以使用 add_key()
系统调用(参见：``Documentation/security/keys/core.rst``)。密钥类型必须
是 "logon"；这种类型的密钥保存在内核内存中，用户空间无法读回。密钥
描述必须是 "fscrypt:" 后跟加密策略中设置的
``master_key_descriptor`` 的 16 字符小写十六进制表示。密钥载荷必须
符合以下结构：)

    #define FSCRYPT_MAX_KEY_SIZE            64

    struct fscrypt_key {
            __u32 mode;
            __u8 raw[FSCRYPT_MAX_KEY_SIZE];
            __u32 size;
    };

``mode`` is ignored; just set it to 0.  The actual key is provided in
``raw`` with ``size`` indicating its size in bytes.  That is, the
bytes ``raw[0..size-1]`` (inclusive) are the actual key.
(``mode`` 被忽略；只需将其设置为 0。实际密钥在 ``raw`` 中提供，
``size`` 指示其大小(以字节为单位)。也就是说，字节
``raw[0..size-1]``(含两端)就是实际密钥。)

The key description prefix "fscrypt:" may alternatively be replaced
with a filesystem-specific prefix such as "ext4:".  However, the
filesystem-specific prefixes are deprecated and should not be used in
new programs.
(密钥描述前缀 "fscrypt:" 也可以替换为文件系统特定的前缀，例如
"ext4:"。但是，文件系统特定的前缀已被弃用，不应在新程序中使用。)

Removing keys
(移除密钥)
-------------

Two ioctls are available for removing a key that was added by
`FS_IOC_ADD_ENCRYPTION_KEY`_:
(有两个 ioctl 可用于移除由 `FS_IOC_ADD_ENCRYPTION_KEY`_ 添加的密钥：)

- `FS_IOC_REMOVE_ENCRYPTION_KEY`_
- `FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS`_

These two ioctls differ only in cases where v2 policy keys are added
or removed by non-root users.
(仅在 v2 策略密钥由非 root 用户添加或移除的情况下，这两个 ioctl 才有
区别。)

These ioctls don't work on keys that were added via the legacy
process-subscribed keyrings mechanism.
(这些 ioctl 对通过传统进程订阅密钥环机制添加的密钥不起作用。)

Before using these ioctls, read the `Online attacks`_ section for a
discussion of the security goals and limitations of these ioctls.
(在使用这些 ioctl 之前，请阅读 `Online attacks`_ 一节，以了解这些
ioctl 的安全目标和局限性。)

FS_IOC_REMOVE_ENCRYPTION_KEY
(FS_IOC_REMOVE_ENCRYPTION_KEY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The FS_IOC_REMOVE_ENCRYPTION_KEY ioctl removes a claim to a master
encryption key from the filesystem, and possibly removes the key
itself.  It can be executed on any file or directory on the target
filesystem, but using the filesystem's root directory is recommended.
It takes in a pointer to struct fscrypt_remove_key_arg, defined
as follows::
(FS_IOC_REMOVE_ENCRYPTION_KEY ioctl 从文件系统中移除对主加密密钥的
一个声明，并可能移除密钥本身。它可以在目标文件系统上的任何文件或目录上
执行，但建议使用文件系统的根目录。它接受一个指向 struct
fscrypt_remove_key_arg 的指针，其定义如下：)

    struct fscrypt_remove_key_arg {
            struct fscrypt_key_specifier key_spec;
    #define FSCRYPT_KEY_REMOVAL_STATUS_FLAG_FILES_BUSY      0x00000001
    #define FSCRYPT_KEY_REMOVAL_STATUS_FLAG_OTHER_USERS     0x00000002
            __u32 removal_status_flags;     /* output */
            __u32 __reserved[5];
    };

This structure must be zeroed, then initialized as follows:
(该结构必须先清零，然后按如下方式初始化：)

- The key to remove is specified by ``key_spec``:
  (要移除的密钥由 ``key_spec`` 指定：)

    - To remove a key used by v1 encryption policies, set
      ``key_spec.type`` to FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR and fill
      in ``key_spec.u.descriptor``.  To remove this type of key, the
      calling process must have the CAP_SYS_ADMIN capability in the
      initial user namespace.
      (要移除 v1 加密策略使用的密钥，将 ``key_spec.type`` 设置为
      FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR，并填充
      ``key_spec.u.descriptor``。要移除这种类型的密钥，调用进程必须在
      初始用户命名空间中拥有 CAP_SYS_ADMIN 能力。)

    - To remove a key used by v2 encryption policies, set
      ``key_spec.type`` to FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER and fill
      in ``key_spec.u.identifier``.
      (要移除 v2 加密策略使用的密钥，将 ``key_spec.type`` 设置为
      FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER，并填充
      ``key_spec.u.identifier``。)

For v2 policy keys, this ioctl is usable by non-root users.  However,
to make this possible, it actually just removes the current user's
claim to the key, undoing a single call to FS_IOC_ADD_ENCRYPTION_KEY.
Only after all claims are removed is the key really removed.
(对于 v2 策略密钥，非 root 用户也可以使用此 ioctl。但是，为了做到这一
点，它实际上只是移除当前用户对该密钥的声明，撤销一次对
FS_IOC_ADD_ENCRYPTION_KEY 的调用。只有当所有声明都被移除后，密钥才会
被真正移除。)

For example, if FS_IOC_ADD_ENCRYPTION_KEY was called with uid 1000,
then the key will be "claimed" by uid 1000, and
FS_IOC_REMOVE_ENCRYPTION_KEY will only succeed as uid 1000.  Or, if
both uids 1000 and 2000 added the key, then for each uid
FS_IOC_REMOVE_ENCRYPTION_KEY will only remove their own claim.  Only
once *both* are removed is the key really removed.  (Think of it like
unlinking a file that may have hard links.)
(例如，如果 FS_IOC_ADD_ENCRYPTION_KEY 是以 uid 1000 调用的，那么该密钥
将被 uid 1000 "声明"，并且 FS_IOC_REMOVE_ENCRYPTION_KEY 只有以 uid
1000 身份才能成功。或者，如果 uid 1000 和 uid 2000 都添加了该密钥，那么
对每个 uid 而言，FS_IOC_REMOVE_ENCRYPTION_KEY 只会移除其自己的声明。
只有当*两者*都被移除后，密钥才会被真正移除。(可以把它想象成对一个可能
有硬链接的文件执行 unlink。))

If FS_IOC_REMOVE_ENCRYPTION_KEY really removes the key, it will also
try to "lock" all files that had been unlocked with the key.  It won't
lock files that are still in-use, so this ioctl is expected to be used
in cooperation with userspace ensuring that none of the files are
still open.  However, if necessary, this ioctl can be executed again
later to retry locking any remaining files.
(如果 FS_IOC_REMOVE_ENCRYPTION_KEY 真正移除了密钥，它还会尝试"锁定"
所有曾用该密钥解锁的文件。它不会锁定仍在使用中的文件，因此预期应与
用户空间配合使用，确保没有文件仍处于打开状态。但是，如有必要，此 ioctl
可以在之后再次执行，以重试锁定任何剩余的文件。)

FS_IOC_REMOVE_ENCRYPTION_KEY returns 0 if either the key was removed
(but may still have files remaining to be locked), the user's claim to
the key was removed, or the key was already removed but had files
remaining to be the locked so the ioctl retried locking them.  In any
of these cases, ``removal_status_flags`` is filled in with the
following informational status flags:
(如果密钥被移除(但可能仍有文件待锁定)、用户对密钥的声明被移除、或密钥
已被移除但仍有文件待锁定因而 ioctl 重试锁定它们，
FS_IOC_REMOVE_ENCRYPTION_KEY 返回 0。在任何一种情况下，
``removal_status_flags`` 都会填充以下信息性状态标志：)

- ``FSCRYPT_KEY_REMOVAL_STATUS_FLAG_FILES_BUSY``: set if some file(s)
  are still in-use.  Not guaranteed to be set in the case where only
  the user's claim to the key was removed.
  (``FSCRYPT_KEY_REMOVAL_STATUS_FLAG_FILES_BUSY``：如果某些文件仍在使用
  中则被设置。在仅移除了用户对密钥的声明的情况下，不保证被设置。)
- ``FSCRYPT_KEY_REMOVAL_STATUS_FLAG_OTHER_USERS``: set if only the
  user's claim to the key was removed, not the key itself
  (``FSCRYPT_KEY_REMOVAL_STATUS_FLAG_OTHER_USERS``：如果仅移除了用户对
  密钥的声明而未移除密钥本身，则被设置)

FS_IOC_REMOVE_ENCRYPTION_KEY can fail with the following errors:
(FS_IOC_REMOVE_ENCRYPTION_KEY 可能因以下错误而失败：)

- ``EACCES``: The FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR key specifier type
  was specified, but the caller does not have the CAP_SYS_ADMIN
  capability in the initial user namespace
  (``EACCES``：指定了 FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR 密钥说明符类型，
  但调用者在初始用户命名空间中不具备 CAP_SYS_ADMIN 能力)
- ``EINVAL``: invalid key specifier type, or reserved bits were set
  (``EINVAL``：无效的密钥说明符类型，或设置了保留位)
- ``ENOKEY``: the key object was not found at all, i.e. it was never
  added in the first place or was already fully removed including all
  files locked; or, the user does not have a claim to the key (but
  someone else does).
  (``ENOKEY``：根本找不到密钥对象，即它最初从未被添加过，或已经被完全
  移除(包括所有文件已锁定)；或者，该用户对密钥没有声明(但其他用户
  有)。)
- ``ENOTTY``: this type of filesystem does not implement encryption
  (``ENOTTY``：此类型的文件系统未实现加密)
- ``EOPNOTSUPP``: the kernel was not configured with encryption
  support for this filesystem, or the filesystem superblock has not
  had encryption enabled on it
  (``EOPNOTSUPP``：内核配置时未启用对此文件系统的加密支持，或文件系统
  超级块尚未在其上启用加密)

FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS
(FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS is exactly the same as
`FS_IOC_REMOVE_ENCRYPTION_KEY`_, except that for v2 policy keys, the
ALL_USERS version of the ioctl will remove all users' claims to the
key, not just the current user's.  I.e., the key itself will always be
removed, no matter how many users have added it.  This difference is
only meaningful if non-root users are adding and removing keys.
(FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS 与 `FS_IOC_REMOVE_ENCRYPTION_KEY`_
完全相同，不同之处在于：对于 v2 策略密钥，ALL_USERS 版本的 ioctl 会
移除所有用户对该密钥的声明，而不仅仅是当前用户的。也就是说，无论有多少
用户添加了该密钥，密钥本身总是会被移除。只有在非 root 用户添加和移除
密钥的情况下，这一区别才有意义。)

Because of this, FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS also requires
"root", namely the CAP_SYS_ADMIN capability in the initial user
namespace.  Otherwise it will fail with EACCES.
(因此，FS_IOC_REMOVE_ENCRYPTION_KEY_ALL_USERS 还要求 "root" 身份，即在
初始用户命名空间中拥有 CAP_SYS_ADMIN 能力。否则它将以 EACCES 失败。)

Getting key status
(获取密钥状态)
------------------

FS_IOC_GET_ENCRYPTION_KEY_STATUS
(FS_IOC_GET_ENCRYPTION_KEY_STATUS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The FS_IOC_GET_ENCRYPTION_KEY_STATUS ioctl retrieves the status of a
master encryption key.  It can be executed on any file or directory on
the target filesystem, but using the filesystem's root directory is
recommended.  It takes in a pointer to
struct fscrypt_get_key_status_arg, defined as follows::
(FS_IOC_GET_ENCRYPTION_KEY_STATUS ioctl 检索主加密密钥的状态。它可以在
目标文件系统上的任何文件或目录上执行，但建议使用文件系统的根目录。它
接受一个指向 struct fscrypt_get_key_status_arg 的指针，其定义如下：)

    struct fscrypt_get_key_status_arg {
            /* input */
            struct fscrypt_key_specifier key_spec;
            __u32 __reserved[6];

            /* output */
    #define FSCRYPT_KEY_STATUS_ABSENT               1
    #define FSCRYPT_KEY_STATUS_PRESENT              2
    #define FSCRYPT_KEY_STATUS_INCOMPLETELY_REMOVED 3
            __u32 status;
    #define FSCRYPT_KEY_STATUS_FLAG_ADDED_BY_SELF   0x00000001
            __u32 status_flags;
            __u32 user_count;
            __u32 __out_reserved[13];
    };

The caller must zero all input fields, then fill in ``key_spec``:
(调用者必须将所有输入字段清零，然后填充 ``key_spec``：)

    - To get the status of a key for v1 encryption policies, set
      ``key_spec.type`` to FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR and fill
      in ``key_spec.u.descriptor``.
      (要获取 v1 加密策略所用密钥的状态，将 ``key_spec.type`` 设置为
      FSCRYPT_KEY_SPEC_TYPE_DESCRIPTOR，并填充
      ``key_spec.u.descriptor``。)

    - To get the status of a key for v2 encryption policies, set
      ``key_spec.type`` to FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER and fill
      in ``key_spec.u.identifier``.
      (要获取 v2 加密策略所用密钥的状态，将 ``key_spec.type`` 设置为
      FSCRYPT_KEY_SPEC_TYPE_IDENTIFIER，并填充
      ``key_spec.u.identifier``。)

On success, 0 is returned and the kernel fills in the output fields:
(成功时返回 0，内核填充输出字段：)

- ``status`` indicates whether the key is absent, present, or
  incompletely removed.  Incompletely removed means that removal has
  been initiated, but some files are still in use; i.e.,
  `FS_IOC_REMOVE_ENCRYPTION_KEY`_ returned 0 but set the informational
  status flag FSCRYPT_KEY_REMOVAL_STATUS_FLAG_FILES_BUSY.
  (``status``：指示密钥是不存在、存在还是未完全移除。未完全移除意味着
  移除已经开始，但仍有一些文件在使用中；也就是说，
  `FS_IOC_REMOVE_ENCRYPTION_KEY`_ 返回了 0，但设置了信息性状态标志
  FSCRYPT_KEY_REMOVAL_STATUS_FLAG_FILES_BUSY。)

- ``status_flags`` can contain the following flags:
  (``status_flags``：可以包含以下标志：)

    - ``FSCRYPT_KEY_STATUS_FLAG_ADDED_BY_SELF`` indicates that the key
      has added by the current user.  This is only set for keys
      identified by ``identifier`` rather than by ``descriptor``.
      (``FSCRYPT_KEY_STATUS_FLAG_ADDED_BY_SELF``：指示该密钥已被当前
      用户添加。仅对以 ``identifier`` 而不是 ``descriptor`` 标识的密钥
      设置。)

- ``user_count`` specifies the number of users who have added the key.
  This is only set for keys identified by ``identifier`` rather than
  by ``descriptor``.
  (``user_count``：指定已添加该密钥的用户数量。仅对以 ``identifier``
  而不是 ``descriptor`` 标识的密钥设置。)

FS_IOC_GET_ENCRYPTION_KEY_STATUS can fail with the following errors:
(FS_IOC_GET_ENCRYPTION_KEY_STATUS 可能因以下错误而失败：)

- ``EINVAL``: invalid key specifier type, or reserved bits were set
  (``EINVAL``：无效的密钥说明符类型，或设置了保留位)
- ``ENOTTY``: this type of filesystem does not implement encryption
  (``ENOTTY``：此类型的文件系统未实现加密)
- ``EOPNOTSUPP``: the kernel was not configured with encryption
  support for this filesystem, or the filesystem superblock has not
  had encryption enabled on it
  (``EOPNOTSUPP``：内核配置时未启用对此文件系统的加密支持，或文件系统
  超级块尚未在其上启用加密)

Among other use cases, FS_IOC_GET_ENCRYPTION_KEY_STATUS can be useful
for determining whether the key for a given encrypted directory needs
to be added before prompting the user for the passphrase needed to
derive the key.
(在其他用例中，FS_IOC_GET_ENCRYPTION_KEY_STATUS 可用于确定是否需要在
提示用户输入派生密钥所需的口令之前，为给定的加密目录添加密钥。)

FS_IOC_GET_ENCRYPTION_KEY_STATUS can only get the status of keys in
the filesystem-level keyring, i.e. the keyring managed by
`FS_IOC_ADD_ENCRYPTION_KEY`_ and `FS_IOC_REMOVE_ENCRYPTION_KEY`_.  It
cannot get the status of a key that has only been added for use by v1
encryption policies using the legacy mechanism involving
process-subscribed keyrings.
(FS_IOC_GET_ENCRYPTION_KEY_STATUS 只能获取文件系统级密钥环中的密钥状态，
即由 `FS_IOC_ADD_ENCRYPTION_KEY`_ 和 `FS_IOC_REMOVE_ENCRYPTION_KEY`_
管理的密钥环。它无法获取仅通过涉及进程订阅密钥环的传统机制添加、供 v1
加密策略使用的密钥的状态。)

Access semantics
(访问语义)
================

With the key
(有密钥时)
------------

With the encryption key, encrypted regular files, directories, and
symlinks behave very similarly to their unencrypted counterparts ---
after all, the encryption is intended to be transparent.  However,
astute users may notice some differences in behavior:
(拥有加密密钥时，加密的普通文件、目录和符号链接的行为与其未加密的对应
物非常相似——毕竟，加密的目标是透明的。但是，敏锐的用户可能会注意到
一些行为差异：)

- Unencrypted files, or files encrypted with a different encryption
  policy (i.e. different key, modes, or flags), cannot be renamed or
  linked into an encrypted directory; see `Encryption policy
  enforcement`_.  Attempts to do so will fail with EXDEV.  However,
  encrypted files can be renamed within an encrypted directory, or
  into an unencrypted directory.
  (未加密的文件，或使用不同加密策略(即不同的密钥、模式或标志)加密的
  文件，不能被重命名或链接到加密目录中；参见 `Encryption policy
  enforcement`_。尝试这样做将以 EXDEV 失败。但是，加密文件可以在加密
  目录内重命名，或重命名到未加密目录中。)

  Note: "moving" an unencrypted file into an encrypted directory, e.g.
  with the `mv` program, is implemented in userspace by a copy
  followed by a delete.  Be aware that the original unencrypted data
  may remain recoverable from free space on the disk; prefer to keep
  all files encrypted from the very beginning.  The `shred` program
  may be used to overwrite the source files but isn't guaranteed to be
  effective on all filesystems and storage devices.
  (注意: 将未加密文件"移动"到加密目录中，例如使用 `mv` 程序，是在用户
  空间中通过先复制后删除实现的。请注意，原始未加密数据可能仍可从磁盘的
  空闲空间中恢复；最好从一开始就保持所有文件处于加密状态。`shred` 程序
  可用于覆写源文件，但不保证在所有文件系统和存储设备上都有效。)

- Direct I/O is supported on encrypted files only under some
  circumstances.  For details, see `Direct I/O support`_.
  (只有在某些情况下，加密文件才支持直接 I/O(Direct I/O)。详情参见
  `Direct I/O support`_。)

- The fallocate operations FALLOC_FL_COLLAPSE_RANGE and
  FALLOC_FL_INSERT_RANGE are not supported on encrypted files and will
  fail with EOPNOTSUPP.
  (fallocate 操作 FALLOC_FL_COLLAPSE_RANGE 和 FALLOC_FL_INSERT_RANGE
  在加密文件上不受支持，将以 EOPNOTSUPP 失败。)

- Online defragmentation of encrypted files is not supported.  The
  EXT4_IOC_MOVE_EXT and F2FS_IOC_MOVE_RANGE ioctls will fail with
  EOPNOTSUPP.
  (不支持对加密文件进行在线碎片整理。EXT4_IOC_MOVE_EXT 和
  F2FS_IOC_MOVE_RANGE ioctl 将以 EOPNOTSUPP 失败。)

- The ext4 filesystem does not support data journaling with encrypted
  regular files.  It will fall back to ordered data mode instead.
  (ext4 文件系统不支持对加密的普通文件使用数据日志(data journaling)。
  它将回退到有序数据模式(ordered data mode)。)

- DAX (Direct Access) is not supported on encrypted files.
  (加密文件不支持 DAX(Direct Access)。)

- The maximum length of an encrypted symlink is 2 bytes shorter than
  the maximum length of an unencrypted symlink.  For example, on an
  EXT4 filesystem with a 4K block size, unencrypted symlinks can be up
  to 4095 bytes long, while encrypted symlinks can only be up to 4093
  bytes long (both lengths excluding the terminating null).
  (加密符号链接的最大长度比未加密符号链接的最大长度短 2 字节。例如，
  在块大小为 4K 的 EXT4 文件系统上，未加密符号链接最长可达 4095 字节，
  而加密符号链接最长只能为 4093 字节(两种长度都不包括结尾的 null)。)

Note that mmap *is* supported.  This is possible because the pagecache
for an encrypted file contains the plaintext, not the ciphertext.
(注意，mmap *是*受支持的。这是可能的，因为加密文件的页缓存中包含的是
明文，而不是密文。)

Without the key
(无密钥时)
---------------

Some filesystem operations may be performed on encrypted regular
files, directories, and symlinks even before their encryption key has
been added, or after their encryption key has been removed:
(即使加密密钥尚未添加、或加密密钥已被移除，仍可以对加密的普通文件、
目录和符号链接执行一些文件系统操作：)

- File metadata may be read, e.g. using stat().
  (可以读取文件元数据，例如使用 stat()。)

- Directories may be listed, in which case the filenames will be
  listed in an encoded form derived from their ciphertext.  The
  current encoding algorithm is described in `Filename hashing and
  encoding`_.  The algorithm is subject to change, but it is
  guaranteed that the presented filenames will be no longer than
  NAME_MAX bytes, will not contain the ``/`` or ``\0`` characters, and
  will uniquely identify directory entries.
  (可以列出目录，此时文件名将以从其密文派生的编码形式列出。当前的编码
  算法在 `Filename hashing and encoding`_ 中描述。该算法可能会更改，但
  保证所呈现的文件名不会超过 NAME_MAX 字节，不会包含 ``/`` 或 ``\0``
  字符，并且能唯一标识目录项。)

  The ``.`` and ``..`` directory entries are special.  They are always
  present and are not encrypted or encoded.
  (``.`` 和 ``..`` 目录项是特殊的。它们始终存在，不会被加密或编码。)

- Files may be deleted.  That is, nondirectory files may be deleted
  with unlink() as usual, and empty directories may be deleted with
  rmdir() as usual.  Therefore, ``rm`` and ``rm -r`` will work as
  expected.
  (可以删除文件。也就是说，非目录文件可以像平常一样用 unlink() 删除，
  空目录可以像平常一样用 rmdir() 删除。因此，``rm`` 和 ``rm -r`` 将
  按预期工作。)

- Symlink targets may be read and followed, but they will be presented
  in encrypted form, similar to filenames in directories.  Hence, they
  are unlikely to point to anywhere useful.
  (符号链接目标可以被读取和跟随，但它们将以加密形式呈现，类似于目录中
  的文件名。因此，它们不太可能指向任何有用的地方。)

Without the key, regular files cannot be opened or truncated.
Attempts to do so will fail with ENOKEY.  This implies that any
regular file operations that require a file descriptor, such as
read(), write(), mmap(), fallocate(), and ioctl(), are also forbidden.
(没有密钥时，普通文件不能被打开或截断。尝试这样做将以 ENOKEY 失败。
这意味着任何需要文件描述符的普通文件操作，例如 read()、write()、
mmap()、fallocate() 和 ioctl()，也都被禁止。)

Also without the key, files of any type (including directories) cannot
be created or linked into an encrypted directory, nor can a name in an
encrypted directory be the source or target of a rename, nor can an
O_TMPFILE temporary file be created in an encrypted directory.  All
such operations will fail with ENOKEY.
(同样，没有密钥时，任何类型的文件(包括目录)都不能被创建或链接到加密
目录中，加密目录中的名称也不能作为重命名的源或目标，也不能在加密目录中
创建 O_TMPFILE 临时文件。所有此类操作都将以 ENOKEY 失败。)

It is not currently possible to backup and restore encrypted files
without the encryption key.  This would require special APIs which
have not yet been implemented.
(目前，在没有加密密钥的情况下无法备份和恢复加密文件。这需要尚未实现的
特殊 API。)

Encryption policy enforcement
(加密策略的执行)
=============================

After an encryption policy has been set on a directory, all regular
files, directories, and symbolic links created in that directory
(recursively) will inherit that encryption policy.  Special files ---
that is, named pipes, device nodes, and UNIX domain sockets --- will
not be encrypted.
(在目录上设置加密策略之后，在该目录中创建的所有普通文件、目录和符号
链接(递归地)都将继承该加密策略。特殊文件——即命名管道(named pipe)、
设备节点(device node)和 UNIX 域套接字(UNIX domain socket)——不会被
加密。)

Except for those special files, it is forbidden to have unencrypted
files, or files encrypted with a different encryption policy, in an
encrypted directory tree.  Attempts to link or rename such a file into
an encrypted directory will fail with EXDEV.  This is also enforced
during ->lookup() to provide limited protection against offline
attacks that try to disable or downgrade encryption in known locations
where applications may later write sensitive data.  It is recommended
that systems implementing a form of "verified boot" take advantage of
this by validating all top-level encryption policies prior to access.
(除这些特殊文件外，禁止在加密目录树中存在未加密的文件，或使用不同加密
策略加密的文件。将此类文件链接或重命名到加密目录的尝试将以 EXDEV 失败。
这在 ->lookup() 期间也会被执行，以针对那些试图在已知位置(应用程序之后
可能向其中写入敏感数据的位置)禁用或降级加密的离线攻击提供有限的保护。
建议实现某种"验证启动"(verified boot)的系统利用这一点，在访问之前
验证所有顶层加密策略。)

Inline encryption support
(内联加密支持)
=========================

Many newer systems (especially mobile SoCs) have *inline encryption
hardware* that can encrypt/decrypt data while it is on its way to/from
the storage device.  Linux supports inline encryption through a set of
extensions to the block layer called *blk-crypto*.  blk-crypto allows
filesystems to attach encryption contexts to bios (I/O requests) to
specify how the data will be encrypted or decrypted in-line.  For more
information about blk-crypto, see
:ref:`Documentation/block/inline-encryption.rst <inline_encryption>`.
(许多较新的系统(尤其是移动 SoC)拥有*内联加密硬件*(inline encryption
hardware)，可以在数据往来于存储设备的途中对其进行加密/解密。Linux 通过
块层(block layer)的一组称为 *blk-crypto* 的扩展来支持内联加密。
blk-crypto 允许文件系统将加密上下文附加到 bio(I/O 请求)上，以指定数据
将如何被内联加密或解密。关于 blk-crypto 的更多信息，参见
:ref:`Documentation/block/inline-encryption.rst <inline_encryption>`。)

On supported filesystems (currently ext4 and f2fs), fscrypt can use
blk-crypto instead of the kernel crypto API to encrypt/decrypt file
contents.  To enable this, set CONFIG_FS_ENCRYPTION_INLINE_CRYPT=y in
the kernel configuration, and specify the "inlinecrypt" mount option
when mounting the filesystem.
(在受支持的文件系统上(目前是 ext4 和 f2fs)，fscrypt 可以使用
blk-crypto 代替内核加密 API 来加密/解密文件内容。要启用此功能，请在
内核配置中设置 CONFIG_FS_ENCRYPTION_INLINE_CRYPT=y，并在挂载文件系统时
指定 "inlinecrypt" 挂载选项。)

Note that the "inlinecrypt" mount option just specifies to use inline
encryption when possible; it doesn't force its use.  fscrypt will
still fall back to using the kernel crypto API on files where the
inline encryption hardware doesn't have the needed crypto capabilities
(e.g. support for the needed encryption algorithm and data unit size)
and where blk-crypto-fallback is unusable.  (For blk-crypto-fallback
to be usable, it must be enabled in the kernel configuration with
CONFIG_BLK_INLINE_ENCRYPTION_FALLBACK=y, and the file must be
protected by a raw key rather than a hardware-wrapped key.)
(注意，"inlinecrypt" 挂载选项只是指定在可能时使用内联加密；它并不强制
使用内联加密。在内联加密硬件不具备所需加密能力(例如支持所需的加密算法
和数据单元大小)、且 blk-crypto-fallback 不可用的文件上，fscrypt 仍会
回退到使用内核加密 API。(要使 blk-crypto-fallback 可用，必须在内核配置
中通过 CONFIG_BLK_INLINE_ENCRYPTION_FALLBACK=y 启用它，并且文件必须
受原始密钥(raw key)而非硬件封装密钥保护。))

Currently fscrypt always uses the filesystem block size (which is
usually 4096 bytes) as the data unit size.  Therefore, it can only use
inline encryption hardware that supports that data unit size.
(目前，fscrypt 总是使用文件系统块大小(通常为 4096 字节)作为数据单元
大小。因此，它只能使用支持该数据单元大小的内联加密硬件。)

Inline encryption doesn't affect the ciphertext or other aspects of
the on-disk format, so users may freely switch back and forth between
using "inlinecrypt" and not using "inlinecrypt".  An exception is that
files that are protected by a hardware-wrapped key can only be
encrypted/decrypted by the inline encryption hardware and therefore
can only be accessed when the "inlinecrypt" mount option is used.  For
more information about hardware-wrapped keys, see below.
(内联加密不影响密文或磁盘格式的其他方面，因此用户可以在使用
"inlinecrypt" 和不使用 "inlinecrypt" 之间自由切换。一个例外是：受硬件
封装密钥保护的文件只能由内联加密硬件加密/解密，因此只能在使用
"inlinecrypt" 挂载选项时访问。关于硬件封装密钥的更多信息，参见下文。)

Hardware-wrapped keys
(硬件封装密钥)
---------------------

fscrypt supports using *hardware-wrapped keys* when the inline
encryption hardware supports it.  Such keys are only present in kernel
memory in wrapped (encrypted) form; they can only be unwrapped
(decrypted) by the inline encryption hardware and are temporally bound
to the current boot.  This prevents the keys from being compromised if
kernel memory is leaked.  This is done without limiting the number of
keys that can be used and while still allowing the execution of
cryptographic tasks that are tied to the same key but can't use inline
encryption hardware, e.g. filenames encryption.
(当内联加密硬件支持时，fscrypt 支持使用*硬件封装密钥*(hardware-wrapped
keys)。这类密钥在内核内存中只以封装(加密)形式存在；它们只能由内联加密
硬件解封(解密)，并且在时间上绑定到当前这次启动。这可以防止密钥在内核
内存泄露时被攻破。这是在不限制可使用密钥数量的情况下完成的，同时仍然
允许执行与同一密钥绑定但无法使用内联加密硬件的密码学任务，例如文件名
加密。)

Note that hardware-wrapped keys aren't specific to fscrypt; they are a
block layer feature (part of *blk-crypto*).  For more details about
hardware-wrapped keys, see the block layer documentation at
:ref:`Documentation/block/inline-encryption.rst
<hardware_wrapped_keys>`.  The rest of this section just focuses on
the details of how fscrypt can use hardware-wrapped keys.
(注意，硬件封装密钥并非 fscrypt 特有；它们是块层的一项功能(*blk-crypto*
的一部分)。关于硬件封装密钥的更多细节，参见块层文档
:ref:`Documentation/block/inline-encryption.rst
<hardware_wrapped_keys>`。本节其余部分只关注 fscrypt 如何使用硬件封装
密钥的细节。)

fscrypt supports hardware-wrapped keys by allowing the fscrypt master
keys to be hardware-wrapped keys as an alternative to raw keys.  To
add a hardware-wrapped key with `FS_IOC_ADD_ENCRYPTION_KEY`_,
userspace must specify FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED in the
``flags`` field of struct fscrypt_add_key_arg and also in the
``flags`` field of struct fscrypt_provisioning_key_payload when
applicable.  The key must be in ephemerally-wrapped form, not
long-term wrapped form.
(fscrypt 通过允许 fscrypt 主密钥作为原始密钥的替代而采用硬件封装密钥的
形式，来支持硬件封装密钥。要通过 `FS_IOC_ADD_ENCRYPTION_KEY`_ 添加
硬件封装密钥，用户空间必须在 struct fscrypt_add_key_arg 的 ``flags``
字段中指定 FSCRYPT_ADD_KEY_FLAG_HW_WRAPPED，并且在适用的情况下，还要在
struct fscrypt_provisioning_key_payload 的 ``flags`` 字段中指定。该
密钥必须处于临时封装(ephemerally-wrapped)形式，而不是长期封装
(long-term wrapped)形式。)

Some limitations apply.  First, files protected by a hardware-wrapped
key are tied to the system's inline encryption hardware.  Therefore
they can only be accessed when the "inlinecrypt" mount option is used,
and they can't be included in portable filesystem images.  Second,
currently the hardware-wrapped key support is only compatible with
`IV_INO_LBLK_64 policies`_ and `IV_INO_LBLK_32 policies`_, as it
assumes that there is just one file contents encryption key per
fscrypt master key rather than one per file.  Future work may address
this limitation by passing per-file nonces down the storage stack to
allow the hardware to derive per-file keys.
(存在一些限制。首先，受硬件封装密钥保护的文件与系统的内联加密硬件绑定。
因此，只能在使用 "inlinecrypt" 挂载选项时访问它们，并且它们不能包含在
可移植的文件系统映像中。其次，目前硬件封装密钥支持仅与
`IV_INO_LBLK_64 policies`_ 和 `IV_INO_LBLK_32 policies`_ 兼容，因为它
假设每个 fscrypt 主密钥只有一个文件内容加密密钥，而不是每个文件一个。
未来的工作可以通过将每文件 nonce 沿存储栈向下传递、让硬件派生每文件
密钥来解决这一限制。)

Implementation-wise, to encrypt/decrypt the contents of files that are
protected by a hardware-wrapped key, fscrypt uses blk-crypto,
attaching the hardware-wrapped key to the bio crypt contexts.  As is
the case with raw keys, the block layer will program the key into a
keyslot when it isn't already in one.  However, when programming a
hardware-wrapped key, the hardware doesn't program the given key
directly into a keyslot but rather unwraps it (using the hardware's
ephemeral wrapping key) and derives the inline encryption key from it.
The inline encryption key is the key that actually gets programmed
into a keyslot, and it is never exposed to software.
(在实现层面，为了加密/解密受硬件封装密钥保护的文件的内容，fscrypt 使用
blk-crypto，将硬件封装密钥附加到 bio crypt 上下文中。与原始密钥的情况
一样，当密钥尚未在密钥槽中时，块层会将密钥编程到密钥槽中。但是，在编程
硬件封装密钥时，硬件不会将给定的密钥直接编程到密钥槽中，而是先将其解封
(使用硬件的临时封装密钥)，并从中派生内联加密密钥。内联加密密钥才是实际
被编程到密钥槽中的密钥，并且它永远不会暴露给软件。)

However, fscrypt doesn't just do file contents encryption; it also
uses its master keys to derive filenames encryption keys, key
identifiers, and sometimes some more obscure types of subkeys such as
dirhash keys.  So even with file contents encryption out of the
picture, fscrypt still needs a raw key to work with.  To get such a
key from a hardware-wrapped key, fscrypt asks the inline encryption
hardware to derive a cryptographically isolated "software secret" from
the hardware-wrapped key.  fscrypt uses this "software secret" to key
its KDF to derive all subkeys other than file contents keys.
(但是，fscrypt 不只做文件内容加密；它还使用其主密钥派生文件名加密密钥、
密钥标识符，有时还派生一些更冷门的子密钥类型，例如目录哈希密钥(dirhash
key)。因此，即使文件内容加密不在考虑范围内，fscrypt 仍然需要一个原始
密钥来工作。为了从硬件封装密钥获得这样的密钥，fscrypt 请求内联加密硬件
从硬件封装密钥派生一个密码学上隔离的"软件秘密"(software secret)。
fscrypt 使用这个"软件秘密"为其 KDF 提供密钥，以派生除文件内容密钥之外的
所有子密钥。)

Note that this implies that the hardware-wrapped key feature only
protects the file contents encryption keys.  It doesn't protect other
fscrypt subkeys such as filenames encryption keys.
(注意，这意味着硬件封装密钥功能只保护文件内容加密密钥。它不保护其他
fscrypt 子密钥，例如文件名加密密钥。)

Direct I/O support
(直接 I/O 支持)
==================

For direct I/O on an encrypted file to work, the following conditions
must be met (in addition to the conditions for direct I/O on an
unencrypted file):
(要使加密文件上的直接 I/O 能够工作，必须满足以下条件(除未加密文件的直接
I/O 条件之外)：)

* The file must be using inline encryption.  Usually this means that
  the filesystem must be mounted with ``-o inlinecrypt`` and inline
  encryption hardware must be present.  However, a software fallback
  is also available.  For details, see `Inline encryption support`_.
  (文件必须正在使用内联加密。通常这意味着文件系统必须以 ``-o
  inlinecrypt`` 挂载，并且内联加密硬件必须存在。不过，也可以使用软件
  回退。详情参见 `Inline encryption support`_。)

* The I/O request must be fully aligned to the filesystem block size.
  This means that the file position the I/O is targeting, the lengths
  of all I/O segments, and the memory addresses of all I/O buffers
  must be multiples of this value.  Note that the filesystem block
  size may be greater than the logical block size of the block device.
  (I/O 请求必须与文件系统块大小完全对齐。这意味着 I/O 所针对的文件
  位置、所有 I/O 段的长度、以及所有 I/O 缓冲的内存地址都必须是该值的
  倍数。注意，文件系统块大小可能大于块设备的逻辑块大小。)

If either of the above conditions is not met, then direct I/O on the
encrypted file will fall back to buffered I/O.
(如果不满足上述任一条件，加密文件上的直接 I/O 将回退到缓冲 I/O
(buffered I/O)。)

Implementation details
(实现细节)
======================

Encryption context
(加密上下文)
------------------

An encryption policy is represented on-disk by
struct fscrypt_context_v1 or struct fscrypt_context_v2.  It is up to
individual filesystems to decide where to store it, but normally it
would be stored in a hidden extended attribute.  It should *not* be
exposed by the xattr-related system calls such as getxattr() and
setxattr() because of the special semantics of the encryption xattr.
(In particular, there would be much confusion if an encryption policy
were to be added to or removed from anything other than an empty
directory.)  These structs are defined as follows::
(加密策略在磁盘上由 struct fscrypt_context_v1 或 struct
fscrypt_context_v2 表示。由各个文件系统自行决定将其存储在哪里，但通常
它会存储在一个隐藏的扩展属性中。由于加密 xattr 的特殊语义，它*不应*
通过 xattr 相关的系统调用(例如 getxattr() 和 setxattr())暴露。
(特别是，如果向空目录以外的任何东西添加或从中移除加密策略，将会引起
很多混乱。)这些结构体的定义如下：)

    #define FSCRYPT_FILE_NONCE_SIZE 16

    #define FSCRYPT_KEY_DESCRIPTOR_SIZE  8
    struct fscrypt_context_v1 {
            u8 version;
            u8 contents_encryption_mode;
            u8 filenames_encryption_mode;
            u8 flags;
            u8 master_key_descriptor[FSCRYPT_KEY_DESCRIPTOR_SIZE];
            u8 nonce[FSCRYPT_FILE_NONCE_SIZE];
    };

    #define FSCRYPT_KEY_IDENTIFIER_SIZE  16
    struct fscrypt_context_v2 {
            u8 version;
            u8 contents_encryption_mode;
            u8 filenames_encryption_mode;
            u8 flags;
            u8 log2_data_unit_size;
            u8 __reserved[3];
            u8 master_key_identifier[FSCRYPT_KEY_IDENTIFIER_SIZE];
            u8 nonce[FSCRYPT_FILE_NONCE_SIZE];
    };

The context structs contain the same information as the corresponding
policy structs (see `Setting an encryption policy`_), except that the
context structs also contain a nonce.  The nonce is randomly generated
by the kernel and is used as KDF input or as a tweak to cause
different files to be encrypted differently; see `Per-file encryption
keys`_ and `DIRECT_KEY policies`_.
(上下文结构体包含与相应策略结构体相同的信息(参见 `Setting an encryption
policy`_)，只是上下文结构体还包含一个 nonce。该 nonce 由内核随机生成，
用作 KDF 输入，或用作调整值(tweak)以使不同文件以不同方式加密；参见
`Per-file encryption keys`_ 和 `DIRECT_KEY policies`_。)

Data path changes
(数据路径的变化)
-----------------

When inline encryption is used, filesystems just need to associate
encryption contexts with bios to specify how the block layer or the
inline encryption hardware will encrypt/decrypt the file contents.
(使用内联加密时，文件系统只需要将加密上下文与 bio 关联，以指定块层或
内联加密硬件将如何加密/解密文件内容。)

When inline encryption isn't used, filesystems must encrypt/decrypt
the file contents themselves, as described below:
(不使用内联加密时，文件系统必须自己加密/解密文件内容，如下所述：)

For the read path (->read_folio()) of regular files, filesystems can
read the ciphertext into the page cache and decrypt it in-place.  The
folio lock must be held until decryption has finished, to prevent the
folio from becoming visible to userspace prematurely.
(对于普通文件的读路径(->read_folio())，文件系统可以将密文读入页缓存并
就地解密。必须持有 folio 锁直到解密完成，以防止 folio 过早地对用户空间
可见。)

For the write path (->writepages()) of regular files, filesystems
cannot encrypt data in-place in the page cache, since the cached
plaintext must be preserved.  Instead, filesystems must encrypt into a
temporary buffer or "bounce page", then write out the temporary
buffer.  Some filesystems, such as UBIFS, already use temporary
buffers regardless of encryption.  Other filesystems, such as ext4 and
F2FS, have to allocate bounce pages specially for encryption.
(对于普通文件的写路径(->writepages())，文件系统不能在页缓存中就地加密
数据，因为必须保留缓存的明文。相反，文件系统必须加密到一个临时缓冲或
"弹跳页"(bounce page)中，然后写出该临时缓冲。一些文件系统，例如 UBIFS，
无论是否加密都已经在使用临时缓冲。其他文件系统，例如 ext4 和 F2FS，
必须专门为加密分配弹跳页。)

Filename hashing and encoding
(文件名哈希与编码)
-----------------------------

Modern filesystems accelerate directory lookups by using indexed
directories.  An indexed directory is organized as a tree keyed by
filename hashes.  When a ->lookup() is requested, the filesystem
normally hashes the filename being looked up so that it can quickly
find the corresponding directory entry, if any.
(现代文件系统通过使用索引目录来加速目录查找。索引目录被组织成以文件名
哈希为键的树。当请求 ->lookup() 时，文件系统通常会对要查找的文件名进行
哈希，以便快速找到对应的目录项(如果存在)。)

With encryption, lookups must be supported and efficient both with and
without the encryption key.  Clearly, it would not work to hash the
plaintext filenames, since the plaintext filenames are unavailable
without the key.  (Hashing the plaintext filenames would also make it
impossible for the filesystem's fsck tool to optimize encrypted
directories.)  Instead, filesystems hash the ciphertext filenames,
i.e. the bytes actually stored on-disk in the directory entries.  When
asked to do a ->lookup() with the key, the filesystem just encrypts
the user-supplied name to get the ciphertext.
(使用加密时，无论有无加密密钥，都必须支持查找并保持高效。显然，对明文
文件名进行哈希是行不通的，因为没有密钥时明文文件名是不可用的。(对明文
文件名进行哈希还会使文件系统的 fsck 工具无法优化加密目录。)相反，
文件系统对密文文件名进行哈希，即目录项中实际存储在磁盘上的字节。当被
要求用密钥执行 ->lookup() 时，文件系统只需加密用户提供的名称即可得到
密文。)

Lookups without the key are more complicated.  The raw ciphertext may
contain the ``\0`` and ``/`` characters, which are illegal in
filenames.  Therefore, readdir() must base64url-encode the ciphertext
for presentation.  For most filenames, this works fine; on ->lookup(),
the filesystem just base64url-decodes the user-supplied name to get
back to the raw ciphertext.
(没有密钥时的查找更为复杂。原始密文可能包含 ``\0`` 和 ``/`` 字符，而这
些字符在文件名中是非法的。因此，readdir() 必须对密文进行 base64url 编码
以便呈现。对于大多数文件名，这工作良好；在 ->lookup() 时，文件系统只需
对用户提供的名称进行 base64url 解码，即可回到原始密文。)

However, for very long filenames, base64url encoding would cause the
filename length to exceed NAME_MAX.  To prevent this, readdir()
actually presents long filenames in an abbreviated form which encodes
a strong "hash" of the ciphertext filename, along with the optional
filesystem-specific hash(es) needed for directory lookups.  This
allows the filesystem to still, with a high degree of confidence, map
the filename given in ->lookup() back to a particular directory entry
that was previously listed by readdir().  See
struct fscrypt_nokey_name in the source for more details.
(但是，对于非常长的文件名，base64url 编码会导致文件名长度超过
NAME_MAX。为防止这种情况，readdir() 实际上以缩写形式呈现长文件名，该
形式对密文文件名编码了一个强"哈希"，以及目录查找所需的可选的文件系统
特定哈希。这使文件系统仍然能够以很高的置信度，将在 ->lookup() 中给出的
文件名映射回之前由 readdir() 列出的特定目录项。更多细节请参见源码中的
struct fscrypt_nokey_name。)

Note that the precise way that filenames are presented to userspace
without the key is subject to change in the future.  It is only meant
as a way to temporarily present valid filenames so that commands like
``rm -r`` work as expected on encrypted directories.
(注意，没有密钥时向用户空间呈现文件名的确切方式将来可能会改变。它只是
作为一种临时呈现有效文件名的方式，以便像 ``rm -r`` 这样的命令在加密
目录上按预期工作。)

Tests
(测试)
=====

To test fscrypt, use xfstests, which is Linux's de facto standard
filesystem test suite.  First, run all the tests in the "encrypt"
group on the relevant filesystem(s).  One can also run the tests
with the 'inlinecrypt' mount option to test the implementation for
inline encryption support.  For example, to test ext4 and
f2fs encryption using `kvm-xfstests
<https://github.com/tytso/xfstests-bld/blob/master/Documentation/kvm-quickstart.md>`_::
(要测试 fscrypt，请使用 xfstests，它是 Linux 事实上的标准文件系统测试
套件。首先，在相关文件系统上运行 "encrypt" 组中的所有测试。还可以使用
'inlinecrypt' 挂载选项运行这些测试，以测试实现对内联加密的支持。例如，
使用 `kvm-xfstests
<https://github.com/tytso/xfstests-bld/blob/master/Documentation/kvm-quickstart.md>`_
测试 ext4 和 f2fs 加密：)

    kvm-xfstests -c ext4,f2fs -g encrypt
    kvm-xfstests -c ext4,f2fs -g encrypt -m inlinecrypt

UBIFS encryption can also be tested this way, but it should be done in
a separate command, and it takes some time for kvm-xfstests to set up
emulated UBI volumes::
(UBIFS 加密也可以用这种方式测试，但应该在单独的命令中完成，并且
kvm-xfstests 设置模拟的 UBI 卷需要一些时间：)

    kvm-xfstests -c ubifs -g encrypt

No tests should fail.  However, tests that use non-default encryption
modes (e.g. generic/549 and generic/550) will be skipped if the needed
algorithms were not built into the kernel's crypto API.  Also, tests
that access the raw block device (e.g. generic/399, generic/548,
generic/549, generic/550) will be skipped on UBIFS.
(不应该有任何测试失败。但是，如果所需的算法未编入内核的加密 API，使用
非默认加密模式的测试(例如 generic/549 和 generic/550)将被跳过。此外，
访问原始块设备的测试(例如 generic/399、generic/548、generic/549、
generic/550)在 UBIFS 上将被跳过。)

Besides running the "encrypt" group tests, for ext4 and f2fs it's also
possible to run most xfstests with the "test_dummy_encryption" mount
option.  This option causes all new files to be automatically
encrypted with a dummy key, without having to make any API calls.
This tests the encrypted I/O paths more thoroughly.  To do this with
kvm-xfstests, use the "encrypt" filesystem configuration::
(除了运行 "encrypt" 组测试之外，对于 ext4 和 f2fs，还可以使用
"test_dummy_encryption" 挂载选项运行大多数 xfstests 测试。此选项使所有
新文件自动用一个哑密钥(dummy key)加密，而无需进行任何 API 调用。这可以
更彻底地测试加密的 I/O 路径。要用 kvm-xfstests 做到这一点，请使用
"encrypt" 文件系统配置：)

    kvm-xfstests -c ext4/encrypt,f2fs/encrypt -g auto
    kvm-xfstests -c ext4/encrypt,f2fs/encrypt -g auto -m inlinecrypt

Because this runs many more tests than "-g encrypt" does, it takes
much longer to run; so also consider using `gce-xfstests
<https://github.com/tytso/xfstests-bld/blob/master/Documentation/gce-xfstests.md>`_
instead of kvm-xfstests::
(由于这比 "-g encrypt" 运行多得多的测试，运行时间要长得多；因此也可以
考虑使用 `gce-xfstests
<https://github.com/tytso/xfstests-bld/blob/master/Documentation/gce-xfstests.md>`_
代替 kvm-xfstests：)

    gce-xfstests -c ext4/encrypt,f2fs/encrypt -g auto
    gce-xfstests -c ext4/encrypt,f2fs/encrypt -g auto -m inlinecrypt
