.. SPDX-License-Identifier: GPL-2.0

.. _pgpguide:

===================================================
Kernel Maintainer PGP guide(Kernel 维护者 PGP 指南)
===================================================

:Author: Konstantin Ryabitsev <konstantin@linuxfoundation.org>
(作者: Konstantin Ryabitsev <konstantin@linuxfoundation.org>)

This document is aimed at Linux kernel developers, and especially at
subsystem maintainers. It contains a subset of information discussed in
the more general "`Protecting Code Integrity`_" guide published by the
Linux Foundation. Please read that document for more in-depth discussion
on some of the topics mentioned in this guide.
(本文档面向 Linux 内核开发者，尤其是子系统维护者。它包含 Linux Foundation 发布的更通用的 "Protecting Code Integrity"（《保护代码完整性》）指南中所讨论信息的一个子集。关于本指南提到的部分主题的更深入讨论，请阅读该文档。)

.. _`Protecting Code Integrity`: https://github.com/lfit/itpol/blob/master/protecting-code-integrity.md

The role of PGP in Linux Kernel development(PGP 在 Linux 内核开发中的角色)
==========================================================================

PGP helps ensure the integrity of the code that is produced by the Linux
kernel development community and, to a lesser degree, establish trusted
communication channels between developers via PGP-signed email exchange.
(PGP 有助于确保 Linux 内核开发社区所产出代码的完整性，并在较小程度上通过 PGP 签名邮件往来在开发者之间建立可信的通信渠道。)

The Linux kernel source code is available in two main formats:
(Linux 内核源代码有两种主要格式：)

- Distributed source repositories (git)
  (分布式源代码仓库（git）。)
- Periodic release snapshots (tarballs)
  (定期发布的快照（tarball）。)

Both git repositories and tarballs carry PGP signatures of the kernel
developers who create official kernel releases. These signatures offer a
cryptographic guarantee that downloadable versions made available via
kernel.org or any other mirrors are identical to what these developers
have on their workstations. To this end:
(git 仓库和 tarball 都带有创建官方内核发布的内核开发者的 PGP 签名。这些签名提供了密码学层面的保证：通过 kernel.org 或任何其他镜像提供的可下载版本与这些开发者工作站上的内容完全一致。为此：

- git repositories provide PGP signatures on all tags
  (git 仓库在所有标签上都提供 PGP 签名。)
- tarballs provide detached PGP signatures with all downloads
  (tarball 为所有下载提供分离式 PGP 签名。)

.. _devs_not_infra:

Trusting the developers, not infrastructure(信任开发者，而非基础设施)
---------------------------------------------------------------------

Ever since the 2011 compromise of core kernel.org systems, the main
operating principle of the Kernel Archives project has been to assume
that any part of the infrastructure can be compromised at any time. For
this reason, the administrators have taken deliberate steps to emphasize
that trust must always be placed with developers and never with the code
hosting infrastructure, regardless of how good the security practices
for the latter may be.
(自 2011 年 kernel.org 核心系统被攻破以来，Kernel Archives 项目的主要运作原则就是假定基础设施的任何部分都可能随时被攻破。因此，管理员们采取了审慎的措施来强调：信任必须始终放在开发者身上，而绝不能放在代码托管基础设施上，无论后者的安全实践做得多么好。)

The above guiding principle is the reason why this guide is needed. We
want to make sure that by placing trust into developers we do not merely
shift the blame for potential future security incidents to someone else.
The goal is to provide a set of guidelines developers can use to create
a secure working environment and safeguard the PGP keys used to
establish the integrity of the Linux kernel itself.
(上述指导原则正是需要本指南的原因。我们希望确保，把信任置于开发者身上并不是单纯把潜在未来安全事件的责任推给其他人。目标是提供一套准则，开发者可以用它来创建安全的工作环境，并保护用于确立 Linux 内核自身完整性的 PGP 密钥。)

.. _pgp_tools:

PGP tools(PGP 工具)
===================

Use GnuPG 2.4 or later(使用 GnuPG 2.4 或更高版本)
-------------------------------------------------

Your distro should already have GnuPG installed by default, you just
need to verify that you are using a reasonably recent version of it.
To check, run::

    $ gpg --version | head -n1

(你的发行版默认应该已经安装了 GnuPG，你只需确认所使用的版本足够新。要检查，请运行以下命令：)

If you have version 2.4 or above, then you are good to go. If you have
an earlier version, then you are using a release of GnuPG that is no
longer maintained and some commands from this guide may not work.
(如果你的版本是 2.4 或更高，那么就可以放心使用了。如果你使用的是更早的版本，那么你正在运行的是已不再维护的 GnuPG 发行版，本指南中的某些命令可能无法工作。)

Configure gpg-agent options(配置 gpg-agent 选项)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The GnuPG agent is a helper tool that will start automatically whenever
you use the ``gpg`` command and run in the background with the purpose
of caching the private key passphrase. There are two options you should
know in order to tweak when the passphrase should be expired from cache:
(GnuPG agent 是一个辅助工具，每当你使用 ``gpg`` 命令时它会自动启动，并在后台运行，用于缓存私钥口令。为了调整口令何时从缓存中过期，你需要了解两个选项：)

- ``default-cache-ttl`` (seconds): If you use the same key again before
  the time-to-live expires, the countdown will reset for another period.
  The default is 600 (10 minutes).
  (``default-cache-ttl`` (秒)：如果在生存期到期之前再次使用同一密钥，倒计时会重置并重新开始一个计时周期。默认值为 600（10 分钟）。)
- ``max-cache-ttl`` (seconds): Regardless of how recently you've used
  the key since initial passphrase entry, if the maximum time-to-live
  countdown expires, you'll have to enter the passphrase again. The
  default is 30 minutes.
  (``max-cache-ttl`` (秒)：无论自首次输入口令以来最近是否用过密钥，只要最大生存期倒计时到期，你就必须再次输入口令。默认值为 30 分钟。)

If you find either of these defaults too short (or too long), you can
edit your ``~/.gnupg/gpg-agent.conf`` file to set your own values::

    # set to 30 minutes for regular ttl, and 2 hours for max ttl
    default-cache-ttl 1800
    max-cache-ttl 7200

(如果你觉得这两个默认值太短（或太长），可以编辑 ``~/.gnupg/gpg-agent.conf`` 文件来设置自己的值：)

.. note::

    It is no longer necessary to start gpg-agent manually at the
    beginning of your shell session. You may want to check your rc files
    to remove anything you had in place for older versions of GnuPG, as
    it may not be doing the right thing any more.
    (已不再需要在 shell 会话开始时手动启动 gpg-agent。你可能需要检查自己的 rc 文件，删除为旧版本 GnuPG 准备的内容，因为它们可能已不再正确。)

.. _protect_your_key:

Protect your PGP key(保护你的 PGP 密钥)
=======================================

This guide assumes that you already have a PGP key that you use for Linux
kernel development purposes. If you do not yet have one, please see the
"`Protecting Code Integrity`_" document mentioned earlier for guidance
on how to create a new one.
(本指南假定你已经拥有一个用于 Linux 内核开发用途的 PGP 密钥。如果你还没有，请参阅前文提到的 "Protecting Code Integrity" 文档，了解如何创建新密钥。)

You should also make a new key if your current one is weaker than 2048
bits (RSA).
(如果你当前密钥的强度低于 2048 位（RSA），也应该生成一个新密钥。)

Understanding PGP Subkeys(理解 PGP 子密钥)
------------------------------------------

A PGP key rarely consists of a single keypair -- usually it is a
collection of independent subkeys that can be used for different
purposes based on their capabilities, assigned at their creation time.
PGP defines four capabilities that a key can have:
(PGP 密钥很少只由单个密钥对组成——它通常是一组相互独立的子密钥，根据创建时分配的能力（capability）用于不同用途。PGP 定义了密钥可以具有的四种能力：)

- **[S]** keys can be used for signing
  (**[S]** 密钥可用于签名。)
- **[E]** keys can be used for encryption
  (**[E]** 密钥可用于加密。)
- **[A]** keys can be used for authentication
  (**[A]** 密钥可用于身份验证。)
- **[C]** keys can be used for certifying other keys
  (**[C]** 密钥可用于为其他密钥签发证书。)

The key with the **[C]** capability is often called the "master" key,
but this terminology is misleading because it implies that the Certify
key can be used in place of any of other subkey on the same chain (like
a physical "master key" can be used to open locks made for other keys).
Since this is not the case, this guide will refer to it as "the Certify
key" to avoid any ambiguity.
(具有 **[C]** 能力的密钥通常被称为"主密钥"（master key），但这一术语有误导性，因为它暗示 Certify 密钥可以代替同一链上的任何其他子密钥使用（就像实体"万能钥匙"可以打开为其他钥匙制作的锁一样）。实际情况并非如此，因此为了避免歧义，本指南将其称为 "Certify 密钥"。)

It is critical to fully understand the following:
(充分理解以下几点至关重要：)

1. All subkeys are fully independent from each other. If you lose a
   private subkey, it cannot be restored or recreated from any other
   private key on your chain.
   (所有子密钥彼此完全独立。如果你丢失了一个私有子密钥，它无法从链上的任何其他私钥恢复或重建。)
2. With the exception of the Certify key, there can be multiple subkeys
   with identical capabilities (e.g. you can have 2 valid encryption
   subkeys, 3 valid signing subkeys, but only one valid certification
   subkey). All subkeys are fully independent -- a message encrypted to
   one **[E]** subkey cannot be decrypted with any other **[E]** subkey
   you may also have.
   (除 Certify 密钥之外，可以存在多个具有相同能力的子密钥（例如，你可以拥有 2 个有效的加密子密钥、3 个有效的签名子密钥，但只能有一个有效的证书签发子密钥）。所有子密钥完全独立——加密给某个 **[E]** 子密钥的消息无法用你可能拥有的任何其他 **[E]** 子密钥解密。)
3. A single subkey may have multiple capabilities (e.g. your **[C]** key
   can also be your **[S]** key).
   (单个子密钥可以具有多种能力（例如，你的 **[C]** 密钥也可以是你的 **[S]** 密钥）。)

The key carrying the **[C]** (certify) capability is the only key that
can be used to indicate relationship with other keys. Only the **[C]**
key can be used to:
(携带 **[C]**（证书签发，certify）能力的密钥是唯一可以用来表明与其他密钥关系的密钥。只有 **[C]** 密钥可以用于：)

- add or revoke other keys (subkeys) with S/E/A capabilities
  (添加或吊销具有 S/E/A 能力的其他密钥（子密钥）。)
- add, change or revoke identities (uids) associated with the key
  (添加、更改或吊销与该密钥关联的身份（uid）。)
- add or change the expiration date on itself or any subkey
  (为自己或任何子密钥添加或更改过期日期。)
- sign other people's keys for web of trust purposes
  (出于信任之网（web of trust）的目的为其他人的密钥签名。)

By default, GnuPG creates the following when generating new keys:
(默认情况下，GnuPG 在生成新密钥时会创建以下内容：)

- One subkey carrying both Certify and Sign capabilities (**[SC]**)
  (一个同时携带证书签发和签名能力的子密钥（**[SC]**）。)
- A separate subkey with the Encryption capability (**[E]**)
  (一个具有加密能力的独立子密钥（**[E]**）。)

If you used the default parameters when generating your key, then that
is what you will have. You can verify by running ``gpg --list-secret-keys``,
for example::

    sec   ed25519 2022-12-20 [SC] [expires: 2024-12-19]
          000000000000000000000000AAAABBBBCCCCDDDD
    uid           [ultimate] Alice Dev <adev@kernel.org>
    ssb   cv25519 2022-12-20 [E] [expires: 2024-12-19]

(如果你在生成密钥时使用了默认参数，那么你得到的就是上述配置。可以通过运行 ``gpg --list-secret-keys`` 来验证，例如：)

The long line under the ``sec`` entry is your key fingerprint --
whenever you see ``[fpr]`` in the examples below, that 40-character
string is what it refers to.
(``sec`` 条目下方的那一长行就是你的密钥指纹——在下面的示例中凡是看到 ``[fpr]`` 的地方，指的都是这个 40 字符的字符串。)

Ensure your passphrase is strong(确保你的口令足够强)
----------------------------------------------------

GnuPG uses passphrases to encrypt your private keys before storing them on
disk. This way, even if your ``.gnupg`` directory is leaked or stolen in
its entirety, the attackers cannot use your private keys without first
obtaining the passphrase to decrypt them.
(GnuPG 使用口令在把私钥存入磁盘之前对其进行加密。这样一来，即使你的 ``.gnupg`` 目录整体泄露或被盗，攻击者也无法在不先获得解密口令的情况下使用你的私钥。)

It is absolutely essential that your private keys are protected by a
strong passphrase. To set it or change it, use::

    $ gpg --change-passphrase [fpr]

(你的私钥必须受到强口令的保护，这一点至关重要。要设置或更改口令，请使用以下命令：)

Create a separate Signing subkey(创建独立的签名子密钥)
------------------------------------------------------

Our goal is to protect your Certify key by moving it to offline media,
so if you only have a combined **[SC]** key, then you should create a
separate signing subkey::

    $ gpg --quick-addkey [fpr] ed25519 sign

(我们的目标是把你的 Certify 密钥转移到离线介质上加以保护，因此如果你只有一把合一的 **[SC]** 密钥，就应该创建一个独立的签名子密钥，请运行以下命令：)

Back up your Certify key for disaster recovery(备份 Certify 密钥以备灾难恢复)
-----------------------------------------------------------------------------

The more signatures you have on your PGP key from other developers, the
more reasons you have to create a backup version that lives on something
other than digital media, for disaster recovery reasons.
(你的 PGP 密钥上来自其他开发者的签名越多，出于灾难恢复的考虑，你就越有理由创建一个保存在数字介质之外的备份版本。)

A good way to create a printable hardcopy of your private key is by
using the ``paperkey`` software written for this very purpose. See ``man
paperkey`` for more details on the output format and its benefits over
other solutions. Paperkey should already be packaged for most
distributions.
(要创建可打印的私钥硬拷贝，一个好办法是使用专为这一目的编写的 ``paperkey`` 软件。输出格式及其相对其他方案的优势等更多细节请参见 ``man paperkey``。大多数发行版应该都已经打包了 Paperkey。)

Run the following command to create a hardcopy backup of your private
key::

    $ gpg --export-secret-key [fpr] | paperkey -o /tmp/key-backup.txt

(运行以下命令创建私钥的硬拷贝备份：)

Print out that file, then take a pen and write your passphrase on the
margin of the paper. **This is strongly recommended** because the key
printout is still encrypted with that passphrase, and if you ever change
it you will not remember what it used to be when you had created the
backup -- *guaranteed*.
(把该文件打印出来，然后用笔把你的口令写在纸张边距上。**强烈建议这样做**，因为密钥打印件仍然使用该口令加密，如果你日后更改了口令，你将不会记得创建备份时用的是哪个口令——这一点完全可以打包票。)

Put the resulting printout and the hand-written passphrase into an envelope
and store in a secure and well-protected place, preferably away from your
home, such as your bank vault.
(把打印件和手写的口令一起放入信封，存放在安全且防护良好的地方，最好是远离你住处的地方，例如银行保险柜。)

.. note::

    The key is still encrypted with your passphrase, so printing out
    even to "cloud-integrated" modern printers should remain a
    relatively safe operation.
    (密钥仍然由你的口令加密，因此即使打印到"云集成"的现代打印机上，也应当是相对安全的操作。)

Back up your whole GnuPG directory(备份整个 GnuPG 目录)
-------------------------------------------------------

.. warning::

    **!!!Do not skip this step!!!**
    (**!!!不要跳过这一步!!!**)

It is important to have a readily available backup of your PGP keys
should you need to recover them. This is different from the
disaster-level preparedness we did with ``paperkey``. You will also rely
on these external copies whenever you need to use your Certify key --
such as when making changes to your own key or signing other people's
keys after conferences and summits.
(拥有一份随时可用的 PGP 密钥备份非常重要，以备你需要恢复密钥时使用。这与我们用 ``paperkey`` 做的灾难级准备不同。每当你需要使用 Certify 密钥时——例如更改自己的密钥，或在会议和峰会之后为其他人的密钥签名——你同样要依赖这些外部副本。)

Start by getting an external media card (preferably two!) that you will
use for backup purposes. You will need to create an encrypted partition
on this device using LUKS -- refer to your distro's documentation on how
to accomplish this.
(首先准备一张用于备份的外部介质卡（最好是两张！）。你需要使用 LUKS 在这个设备上创建一个加密分区——具体做法请参考你的发行版文档。)

For the encryption passphrase, you can use the same one as on your
PGP key.
(加密口令可以使用与你的 PGP 密钥相同的口令。)

Once the encryption process is over, re-insert your device and make sure
it gets properly mounted. Copy your entire ``.gnupg`` directory over to
the encrypted storage::

    $ cp -a ~/.gnupg /media/disk/foo/gnupg-backup

(加密过程完成后，重新插入设备并确保它被正确挂载。把你整个 ``.gnupg`` 目录复制到加密存储上，请运行以下命令：)

You should now test to make sure everything still works::

    $ gpg --homedir=/media/disk/foo/gnupg-backup --list-key [fpr]

(现在你应该测试一下，确保一切仍然正常工作：)

If you don't get any errors, then you should be good to go. Unmount the
device, distinctly label it so you don't overwrite it by accident, and
put in a safe place -- but not too far away, because you'll need to use
it every now and again for things like editing identities, adding or
revoking subkeys, or signing other people's keys.
(如果没有出现任何错误，那么就可以放心使用了。卸载设备，清晰地做好标记以免误覆盖，然后放在安全的地方——但也不要太远，因为你需要不时使用它来做编辑身份、添加或吊销子密钥、为其他人的密钥签名之类的事情。)

Remove the Certify key from your homedir(从主目录中移除 Certify 密钥)
---------------------------------------------------------------------

The files in our home directory are not as well protected as we like to
think.  They can be leaked or stolen via many different means:
(我们主目录中的文件并不像我们以为的那样受到良好保护。它们可能通过许多不同的途径泄露或被盗：)

- by accident when making quick homedir copies to set up a new workstation
  (在为设置新工作站而快速复制主目录时意外泄露。)
- by systems administrator negligence or malice
  (系统管理员的疏忽或恶意行为。)
- via poorly secured backups
  (防护不善的备份。)
- via malware in desktop apps (browsers, pdf viewers, etc)
  (桌面应用（浏览器、pdf 查看器等）中的恶意软件。)
- via coercion when crossing international borders
  (跨越国境时受到胁迫。)

Protecting your key with a good passphrase greatly helps reduce the risk
of any of the above, but passphrases can be discovered via keyloggers,
shoulder-surfing, or any number of other means. For this reason, the
recommended setup is to remove your Certify key from your home directory
and store it on offline storage.
(用一个强口令保护你的密钥能大大降低上述风险，但口令仍可能通过键盘记录器、肩窥（shoulder-surfing）或其他诸多手段被窃取。因此，推荐的做法是把你的 Certify 密钥从主目录中移除，并存放在离线存储上。)

.. warning::

    Please see the previous section and make sure you have backed up
    your GnuPG directory in its entirety. What we are about to do will
    render your key useless if you do not have a usable backup!
    (请参阅上一节，确保你已经完整备份了 GnuPG 目录。如果没有可用的备份，我们接下来要做的事情将使你的密钥彻底失效！)

First, identify the "keygrip" of your Certify key::

    $ gpg --with-keygrip --list-key [fpr]

(首先，找出你的 Certify 密钥的 "keygrip"（密钥握柄）：)

The output will be something like this::

    pub   ed25519 2022-12-20 [SC] [expires: 2022-12-19]
          000000000000000000000000AAAABBBBCCCCDDDD
          Keygrip = 1111000000000000000000000000000000000000
    uid           [ultimate] Alice Dev <adev@kernel.org>
    sub   cv25519 2022-12-20 [E] [expires: 2022-12-19]
          Keygrip = 2222000000000000000000000000000000000000
    sub   ed25519 2022-12-20 [S]
          Keygrip = 3333000000000000000000000000000000000000

(输出将类似如下：)

Find the keygrip entry that is beneath the ``pub`` line (right under the
Certify key fingerprint). This will correspond directly to a file in your
``~/.gnupg`` directory::

    $ cd ~/.gnupg/private-keys-v1.d
    $ ls
    1111000000000000000000000000000000000000.key
    2222000000000000000000000000000000000000.key
    3333000000000000000000000000000000000000.key

(找到 ``pub`` 行下方的 keygrip 条目（就在 Certify 密钥指纹的正下方）。它直接对应你 ``~/.gnupg`` 目录中的一个文件：)

It is sufficient to remove the .key file that corresponds to the Certify
key keygrip::

    $ cd ~/.gnupg/private-keys-v1.d
    $ rm 1111000000000000000000000000000000000000.key

(只需删除与 Certify 密钥 keygrip 对应的 .key 文件即可：)

Now, if you issue the ``--list-secret-keys`` command, it will show that
the Certify key is missing (the ``#`` indicates it is not available)::

    $ gpg --list-secret-keys
    sec#  ed25519 2022-12-20 [SC] [expires: 2024-12-19]
          000000000000000000000000AAAABBBBCCCCDDDD
    uid           [ultimate] Alice Dev <adev@kernel.org>
    ssb   cv25519 2022-12-20 [E] [expires: 2024-12-19]
    ssb   ed25519 2022-12-20 [S]

(现在，如果你执行 ``--list-secret-keys`` 命令，会看到 Certify 密钥已经缺失（``#`` 表示它不可用）：)

You should also remove any ``secring.gpg`` files in the ``~/.gnupg``
directory, which may be left over from previous versions of GnuPG.
(你还应该删除 ``~/.gnupg`` 目录中任何 ``secring.gpg`` 文件，它们可能是旧版本 GnuPG 遗留下来的。)

If you don't have the "private-keys-v1.d" directory(如果你没有 "private-keys-v1.d" 目录)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you do not have a ``~/.gnupg/private-keys-v1.d`` directory, then your
secret keys are still stored in the legacy ``secring.gpg`` file used by
GnuPG v1. Making any changes to your key, such as changing the
passphrase or adding a subkey, should automatically convert the old
``secring.gpg`` format to use ``private-keys-v1.d`` instead.
(如果你没有 ``~/.gnupg/private-keys-v1.d`` 目录，那么你的私钥仍存储在 GnuPG v1 使用的旧式 ``secring.gpg`` 文件中。对密钥做任何更改（例如更改口令或添加子密钥）时，应该会自动把旧的 ``secring.gpg`` 格式转换为改用 ``private-keys-v1.d``。)

Once you get that done, make sure to delete the obsolete ``secring.gpg``
file, which still contains your private keys.
(完成之后，务必删除已过时的 ``secring.gpg`` 文件，其中仍然包含你的私钥。)

.. _smartcards:

Move the subkeys to a dedicated crypto device(将子密钥转移到专用加密设备)
=========================================================================

Even though the Certify key is now safe from being leaked or stolen, the
subkeys are still in your home directory. Anyone who manages to get
their hands on those will be able to decrypt your communication or fake
your signatures (if they know the passphrase). Furthermore, each time a
GnuPG operation is performed, the keys are loaded into system memory and
can be stolen from there by sufficiently advanced malware (think
Meltdown and Spectre).
(尽管 Certify 密钥现在已经不易被泄露或窃取，但子密钥仍然在你的主目录里。任何拿到这些子密钥的人（如果知道口令）都能解密你的通信或伪造你的签名。此外，每次执行 GnuPG 操作时，密钥都会被加载到系统内存中，足够先进的恶意软件（想想 Meltdown 和 Spectre）可以从那里窃取它们。)

A good way to completely protect your keys is to move them to a
specialized hardware device that is capable of smartcard operations.
(完全保护密钥的一个好办法，是把它们转移到支持智能卡操作的专用硬件设备上。)

The benefits of smartcards(智能卡的好处)
----------------------------------------

A smartcard contains a cryptographic chip that is capable of storing
private keys and performing crypto operations directly on the card
itself. Because the key contents never leave the smartcard, the
operating system of the computer into which you plug in the hardware
device is not able to retrieve the private keys themselves. This is very
different from the encrypted media storage device we used earlier for
backup purposes -- while that device is plugged in and mounted, the
operating system is able to access the private key contents.
(智能卡包含一个加密芯片，能够存储私钥并直接在卡上执行加密操作。由于密钥内容从不离开智能卡，插入硬件设备的计算机操作系统无法取回私钥本身。这与我们之前用于备份的加密介质存储设备非常不同——只要那个设备插着并处于挂载状态，操作系统就能够访问其中的私钥内容。)

Using external encrypted media is not a substitute to having a
smartcard-capable device.
(使用外部加密介质并不能替代拥有支持智能卡的设备。)

Available smartcard devices(可用的智能卡设备)
---------------------------------------------

Unless all your laptops and workstations have smartcard readers, the
easiest is to get a specialized USB device that implements smartcard
functionality. There are several options available:
(除非你所有的笔记本电脑和工作站都配有智能卡读卡器，否则最简单的办法是购买一个实现智能卡功能的专用 USB 设备。有几种选择：)

- `Nitrokey Start`_: Open hardware and Free Software, based on FSI
  Japan's `Gnuk`_. One of the cheapest options, but offers fewest
  security features (such as resistance to tampering or some
  side-channel attacks).
  (Nitrokey Start：开放硬件与自由软件，基于 FSI Japan 的 Gnuk。最便宜的选项之一，但安全特性最少（例如防篡改或抵抗某些侧信道攻击的能力）。)
- `Nitrokey 3`_: Similar to the Nitrokey Start, but more
  tamper-resistant and offers more security features and USB
  form-factors. Supports ECC cryptography (ED25519 and NISTP).
  (Nitrokey 3：与 Nitrokey Start 类似，但抗篡改能力更强，提供更多安全特性和 USB 形态。支持 ECC 密码学（ED25519 和 NISTP）。)
- `Yubikey 5`_: proprietary hardware and software, but cheaper than
  Nitrokey with a similar set of features. Supports ECC cryptography
  (ED25519 and NISTP).
  (Yubikey 5：专有硬件和软件，但比 Nitrokey 便宜，功能集相近。支持 ECC 密码学（ED25519 和 NISTP）。)

Your choice will depend on cost, shipping availability in your
geographical region, and open/proprietary hardware considerations.
(你的选择将取决于成本、所在地区的发货情况，以及开放/专有硬件方面的考量。)

.. note::

    If you are listed in an `M:` entry in MAINTAINERS or have an account at
    kernel.org, you `qualify for a free Nitrokey Start`_ courtesy of The Linux
    Foundation.
    (如果你在 MAINTAINERS 中列有 "M:" 条目或拥有 kernel.org 账户，那么你就有资格获得 The Linux Foundation 慷慨赠送的免费 Nitrokey Start。)

.. _`Nitrokey Start`: https://www.nitrokey.com/products/nitrokeys
.. _`Nitrokey 3`: https://www.nitrokey.com/products/nitrokeys
.. _`Yubikey 5`: https://www.yubico.com/products/yubikey-5-overview/
.. _Gnuk: https://www.fsij.org/doc-gnuk/
.. _`qualify for a free Nitrokey Start`: https://www.kernel.org/nitrokey-digital-tokens-for-kernel-developers.html

Configure your smartcard device(配置你的智能卡设备)
---------------------------------------------------

Your smartcard device should Just Work (TM) the moment you plug it into
any modern Linux workstation. You can verify it by running::

    $ gpg --card-status

(你的智能卡设备插入任何现代 Linux 工作站的瞬间就应该"开箱即用"。你可以通过运行以下命令来验证：)

If you see full smartcard details, then you are good to go.
Unfortunately, troubleshooting all possible reasons why things may not
be working for you is way beyond the scope of this guide. If you are
having trouble getting the card to work with GnuPG, please seek help via
usual support channels.
(如果你看到了完整的智能卡详情，那么就可以放心使用了。遗憾的是，排查设备无法工作的所有可能原因远远超出了本指南的范围。如果你在让智能卡与 GnuPG 协作时遇到困难，请通过常规支持渠道寻求帮助。)

To configure your smartcard, you will need to use the GnuPG menu system, as
there are no convenient command-line switches::

    $ gpg --card-edit
    [...omitted...]
    gpg/card> admin
    Admin commands are allowed
    gpg/card> passwd

(要配置你的智能卡，需要使用 GnuPG 的菜单系统，因为没有便捷的命令行开关：)

You should set the user PIN (1), Admin PIN (3), and the Reset Code (4).
Please make sure to record and store these in a safe place -- especially
the Admin PIN and the Reset Code (which allows you to completely wipe
the smartcard). You so rarely need to use the Admin PIN, that you will
inevitably forget what it is if you do not record it.
(你应当设置用户 PIN（1）、管理员 PIN（3）和重置码（4）。请务必记录并妥善保存这些内容——尤其是管理员 PIN 和重置码（重置码允许你完全擦除智能卡）。管理员 PIN 极少用到，如果不记录下来，你迟早会忘记它是什么。)

Getting back to the main card menu, you can also set other values (such
as name, gender, login data, etc), but it's not necessary and will
additionally leak information about your smartcard should you lose it.
(回到主卡片菜单后，你还可以设置其他值（例如姓名、性别、登录数据等），但这并非必需，而且一旦你丢失了智能卡，这些信息还会造成额外泄露。)

.. note::

    Despite having the name "PIN", neither the user PIN nor the admin
    PIN on the card need to be numbers.
    (尽管名字叫 "PIN"，卡上的用户 PIN 和管理员 PIN 都不必是数字。)

.. warning::

    Some devices may require that you move the subkeys onto the device
    before you can change the passphrase. Please check the documentation
    provided by the device manufacturer.
    (某些设备可能要求你先把子密钥转移到设备上，然后才能更改口令。请查阅设备制造商提供的文档。)

Move the subkeys to your smartcard(将子密钥转移到你的智能卡)
------------------------------------------------------------

Exit the card menu (using "q") and save all changes. Next, let's move
your subkeys onto the smartcard. You will need both your PGP key
passphrase and the admin PIN of the card for most operations::

    $ gpg --edit-key [fpr]

    Secret subkeys are available.

    pub  ed25519/AAAABBBBCCCCDDDD
         created: 2022-12-20  expires: 2024-12-19  usage: SC
         trust: ultimate      validity: ultimate
    ssb  cv25519/1111222233334444
         created: 2022-12-20  expires: never       usage: E
    ssb  ed25519/5555666677778888
         created: 2017-12-07  expires: never       usage: S
    [ultimate] (1). Alice Dev <adev@kernel.org>

    gpg>

(退出卡片菜单（使用 "q"）并保存所有更改。接下来，我们把你的子密钥转移到智能卡上。大多数操作都需要用到你的 PGP 密钥口令和卡片的管理员 PIN：)

Using ``--edit-key`` puts us into the menu mode again, and you will
notice that the key listing is a little different. From here on, all
commands are done from inside this menu mode, as indicated by ``gpg>``.
(使用 ``--edit-key`` 会再次进入菜单模式，你会注意到密钥列表有些不同。从现在起，所有命令都在这个菜单模式内完成，以 ``gpg>`` 提示符为标志。)

First, let's select the key we'll be putting onto the card -- you do
this by typing ``key 1`` (it's the first one in the listing, the **[E]**
subkey)::

    gpg> key 1

(首先，选择要放到卡上的密钥——输入 ``key 1``（它是列表中的第一把，即 **[E]** 子密钥）：)

In the output, you should now see ``ssb*`` on the **[E]** key. The ``*``
indicates which key is currently "selected." It works as a *toggle*,
meaning that if you type ``key 1`` again, the ``*`` will disappear and
the key will not be selected any more.
(在输出中，你现在应该会在 **[E]** 密钥上看到 ``ssb*``。``*`` 表示当前"选中"的是哪把密钥。它相当于一个*切换开关*，也就是说，如果你再次输入 ``key 1``，``*`` 就会消失，该密钥也不再处于选中状态。)

Now, let's move that key onto the smartcard::

    gpg> keytocard
    Please select where to store the key:
       (2) Encryption key
    Your selection? 2

(现在，把这把密钥转移到智能卡上：)

Since it's our **[E]** key, it makes sense to put it into the Encryption
slot.  When you submit your selection, you will be prompted first for
your PGP key passphrase, and then for the admin PIN. If the command
returns without an error, your key has been moved.
(由于这是我们的 **[E]** 密钥，把它放进加密（Encryption）槽位是合理的。提交选择后，会先提示输入你的 PGP 密钥口令，然后是管理员 PIN。如果命令没有报错地返回，说明密钥已经转移完成。)

**Important**: Now type ``key 1`` again to unselect the first key, and
``key 2`` to select the **[S]** key::

    gpg> key 1
    gpg> key 2
    gpg> keytocard
    Please select where to store the key:
       (1) Signature key
       (3) Authentication key
    Your selection? 1

(**重要**：现在再次输入 ``key 1`` 取消选择第一把密钥，然后输入 ``key 2`` 选择 **[S]** 密钥：)

You can use the **[S]** key both for Signature and Authentication, but
we want to make sure it's in the Signature slot, so choose (1). Once
again, if your command returns without an error, then the operation was
successful::

    gpg> q
    Save changes? (y/N) y

(**[S]** 密钥既可以用于签名（Signature）也可以用于身份验证（Authentication），但我们要确保它位于签名槽位，所以选择 (1)。同样，如果命令没有报错地返回，说明操作成功：)

Saving the changes will delete the keys you moved to the card from your
home directory (but it's okay, because we have them in our backups
should we need to do this again for a replacement smartcard).
(保存更改会把已经转移到卡上的密钥从你的主目录中删除（不过没关系，备份里还有它们，以便在更换智能卡时再次执行此操作）。)

Verifying that the keys were moved(验证密钥已被转移)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you perform ``--list-secret-keys`` now, you will see a subtle
difference in the output::

    $ gpg --list-secret-keys
    sec#  ed25519 2022-12-20 [SC] [expires: 2024-12-19]
          000000000000000000000000AAAABBBBCCCCDDDD
    uid           [ultimate] Alice Dev <adev@kernel.org>
    ssb>  cv25519 2022-12-20 [E] [expires: 2024-12-19]
    ssb>  ed25519 2022-12-20 [S]

(如果你现在执行 ``--list-secret-keys``，会在输出中看到一个细微的差别：)

The ``>`` in the ``ssb>`` output indicates that the subkey is only
available on the smartcard. If you go back into your secret keys
directory and look at the contents there, you will notice that the
``.key`` files there have been replaced with stubs::

    $ cd ~/.gnupg/private-keys-v1.d
    $ strings *.key | grep 'private-key'

(``ssb>`` 输出中的 ``>`` 表示该子密钥只存在于智能卡上。如果你回到私钥目录查看其中的内容，会注意到那里的 ``.key`` 文件已被替换为占位文件（stub）：)

The output should contain ``shadowed-private-key`` to indicate that
these files are only stubs and the actual content is on the smartcard.
(输出中应包含 ``shadowed-private-key``，表明这些文件只是占位文件，实际内容在智能卡上。)

Verifying that the smartcard is functioning(验证智能卡工作正常)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To verify that the smartcard is working as intended, you can create a
signature::

    $ echo "Hello world" | gpg --clearsign > /tmp/test.asc
    $ gpg --verify /tmp/test.asc

(要验证智能卡是否按预期工作，可以创建一个签名：)

This should ask for your smartcard PIN on your first command, and then
show "Good signature" after you run ``gpg --verify``.
(第一条命令应当会要求你输入智能卡 PIN，然后在你运行 ``gpg --verify`` 后显示 "Good signature"。)

Congratulations, you have successfully made it extremely difficult to
steal your digital developer identity!
(恭喜你，你已经成功地让窃取你的数字开发者身份变得极其困难！)

Other common GnuPG operations(其他常见 GnuPG 操作)
--------------------------------------------------

Here is a quick reference for some common operations you'll need to do
with your PGP key.
(以下是你在使用 PGP 密钥时需要执行的一些常见操作的速查参考。)

Mounting your safe offline storage(挂载你的安全离线存储)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You will need your Certify key for any of the operations below, so you
will first need to mount your backup offline storage and tell GnuPG to
use it::

    $ export GNUPGHOME=/media/disk/foo/gnupg-backup
    $ gpg --list-secret-keys

(下面的任何操作都需要用到你的 Certify 密钥，因此首先需要挂载你的备份离线存储，并告诉 GnuPG 使用它：)

You want to make sure that you see ``sec`` and not ``sec#`` in the
output (the ``#`` means the key is not available and you're still using
your regular home directory location).
(你要确保在输出中看到的是 ``sec`` 而不是 ``sec#``（``#`` 表示密钥不可用，你仍在使用常规的主目录位置）。)

Extending key expiration date(延长密钥过期日期)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Certify key has the default expiration date of 2 years from the date
of creation. This is done both for security reasons and to make obsolete
keys eventually disappear from keyservers.
(Certify 密钥的默认过期日期是自创建之日起 2 年。这样做既有安全方面的考虑，也是为了让过时的密钥最终从密钥服务器上消失。)

To extend the expiration on your key by a year from current date, just
run::

    $ gpg --quick-set-expire [fpr] 1y

(要把密钥的过期时间从当前日期起延长一年，只需运行以下命令：)

You can also use a specific date if that is easier to remember (e.g.
your birthday, January 1st, or Canada Day)::

    $ gpg --quick-set-expire [fpr] 2038-07-01

(如果更容易记住，你也可以使用一个具体的日期（例如你的生日、1 月 1 日或加拿大日）：)

Remember to send the updated key back to keyservers::

    $ gpg --send-key [fpr]

(记得把更新后的密钥发送回密钥服务器：)

Updating your work directory after any changes(在做出任何更改后更新工作目录)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After you make any changes to your key using the offline storage, you will
want to import these changes back into your regular working directory::

    $ gpg --export | gpg --homedir ~/.gnupg --import
    $ unset GNUPGHOME

(使用离线存储对密钥做出任何更改之后，你需要把这些更改导回你的常规工作目录：)

Using gpg-agent over ssh(通过 ssh 使用 gpg-agent)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can forward your gpg-agent over ssh if you need to sign tags or
commits on a remote system. Please refer to the instructions provided
on the GnuPG wiki:
(如果你需要在远程系统上为标签或提交签名，可以通过 ssh 转发你的 gpg-agent。具体说明请参阅 GnuPG wiki 上提供的指引：)

- `Agent Forwarding over SSH`_
  (通过 SSH 转发 agent。)

It works more smoothly if you can modify the sshd server settings on the
remote end.
(如果你能修改远程端的 sshd 服务器设置，它工作起来会更顺畅。)

.. _`Agent Forwarding over SSH`: https://wiki.gnupg.org/AgentForwarding

.. _pgp_with_git:

Using PGP with Git(在 Git 中使用 PGP)
=====================================

One of the core features of Git is its decentralized nature -- once a
repository is cloned to your system, you have full history of the
project, including all of its tags, commits and branches. However, with
hundreds of cloned repositories floating around, how does anyone verify
that their copy of linux.git has not been tampered with by a malicious
third party?
(Git 的核心特性之一是它的去中心化本质——一旦仓库被克隆到你的系统上，你就拥有了项目的完整历史，包括它所有的标签、提交和分支。然而，既然有数百个克隆仓库四处流传，人们如何验证自己手中的 linux.git 副本没有被恶意第三方篡改过？)

Or what happens if malicious code is discovered in the kernel and the
"Author" line in the commit says it was done by you, while you're pretty
sure you had `nothing to do with it`_?
(或者，如果在内核中发现了恶意代码，而提交中的 "Author" 行显示它出自你手，但你非常确定自己与此毫无干系，那会发生什么？)

To address both of these issues, Git introduced PGP integration. Signed
tags prove the repository integrity by assuring that its contents are
exactly the same as on the workstation of the developer who created the
tag, while signed commits make it nearly impossible for someone to
impersonate you without having access to your PGP keys.
(为了解决这两个问题，Git 引入了 PGP 集成。签名标签可以证明仓库的完整性，确保其内容与创建该标签的开发者工作站上的内容完全一致；而签名提交则让攻击者在拿不到你的 PGP 密钥的情况下几乎不可能冒充你。)

.. _`nothing to do with it`: https://github.com/jayphelps/git-blame-someone-else

Configure git to use your PGP key(配置 git 使用你的 PGP 密钥)
-------------------------------------------------------------

If you only have one secret key in your keyring, then you don't really
need to do anything extra, as it becomes your default key.  However, if
you happen to have multiple secret keys, you can tell git which key
should be used (``[fpr]`` is the fingerprint of your key)::

    $ git config --global user.signingKey [fpr]

(如果你的密钥环中只有一把私钥，那么不需要做任何额外的事情，它会自动成为你的默认密钥。不过，如果你恰好拥有多把私钥，可以告诉 git 应该使用哪一把（``[fpr]`` 是你密钥的指纹）：)

How to work with signed tags(如何使用签名标签)
----------------------------------------------

To create a signed tag, pass the ``-s`` switch to the tag command::

    $ git tag -s [tagname]

(要创建签名标签，给 tag 命令传递 ``-s`` 开关：)

Our recommendation is to always sign git tags, as this allows other
developers to ensure that the git repository they are pulling from has
not been maliciously altered.
(我们建议始终为 git 标签签名，因为这可以让其他开发者确保他们正在拉取的 git 仓库没有被恶意篡改。)

How to verify signed tags(如何验证签名标签)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To verify a signed tag, use the ``verify-tag`` command::

    $ git verify-tag [tagname]

(要验证签名标签，使用 ``verify-tag`` 命令：)

If you are pulling a tag from another fork of the project repository,
git should automatically verify the signature at the tip you're pulling
and show you the results during the merge operation::

    $ git pull [url] tags/sometag

(如果你从项目仓库的另一个分支拉取标签，git 应当会自动验证你所拉取顶端提交的签名，并在合并操作期间向你显示结果：)

The merge message will contain something like this::

    Merge tag 'sometag' of [url]

    [Tag message]

    # gpg: Signature made [...]
    # gpg: Good signature from [...]

(合并消息将包含类似如下内容：)

If you are verifying someone else's git tag, you will first need to
import their PGP key. Please refer to the ":ref:`verify_identities`"
section below.
(如果你要验证别人的 git 标签，首先需要导入他们的 PGP 密钥。请参阅下文的 :ref:`verify_identities` 一节。)

Configure git to always sign annotated tags(配置 git 始终为附注标签签名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Chances are, if you're creating an annotated tag, you'll want to sign
it. To force git to always sign annotated tags, you can set a global
configuration option::

    $ git config --global tag.forceSignAnnotated true

(如果你在创建附注标签（annotated tag），通常都会想为它签名。要强制 git 始终为附注标签签名，可以设置一个全局配置选项：)

How to work with signed commits(如何使用签名提交)
-------------------------------------------------

It is also possible to create signed commits, but they have limited
usefulness in Linux kernel development. The kernel contribution workflow
relies on sending in patches, and converting commits to patches does not
preserve git commit signatures. Furthermore, when rebasing your own
repository on a newer upstream, PGP commit signatures will end up
discarded. For this reason, most kernel developers don't bother signing
their commits and will ignore signed commits in any external
repositories that they rely upon in their work.
(也可以创建签名提交，但它们在 Linux 内核开发中的用处有限。内核贡献流程依赖提交补丁，而把提交转换为补丁并不会保留 git 提交签名。此外，当你把自己的仓库变基（rebase）到更新的上游时，PGP 提交签名最终会被丢弃。因此，大多数内核开发者不会费心为自己的提交签名，并且会忽略他们工作中所依赖的任何外部仓库里的签名提交。)

That said, if you have your working git tree publicly available at some
git hosting service (kernel.org, infradead.org, ozlabs.org, or others),
then the recommendation is that you sign all your git commits even if
upstream developers do not directly benefit from this practice.
(话虽如此，如果你的工作 git 树在某个 git 托管服务（kernel.org、infradead.org、ozlabs.org 等）上公开可用，那么建议你为所有 git 提交签名，即使上游开发者并不能直接从中受益。)

We recommend this for the following reasons:
(我们建议这样做的原因如下：)

1. Should there ever be a need to perform code forensics or track code
   provenance, even externally maintained trees carrying PGP commit
   signatures will be valuable for such purposes.
   (万一需要进行代码取证或追溯代码来源，即使是外部维护的、带有 PGP 提交签名的代码树也会对这些用途很有价值。)
2. If you ever need to re-clone your local repository (for example,
   after reinstalling your system), this lets you verify the repository
   integrity before resuming your work.
   (如果你需要重新克隆本地仓库（例如，重装系统之后），这让你能够在恢复工作之前验证仓库的完整性。)
3. If someone needs to cherry-pick your commits, this allows them to
   quickly verify their integrity before applying them.
   (如果有人需要 cherry-pick 你的提交，这让他们能够在应用之前快速验证其完整性。)

Creating signed commits(创建签名提交)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a signed commit, pass the ``-S`` flag to the ``git commit``
command (it's capital ``-S`` due to collision with another flag)::

    $ git commit -S

(要创建签名提交，给 ``git commit`` 命令传递 ``-S`` 标志（因为与另一个标志冲突，所以是大写的 ``-S``）：)

Configure git to always sign commits(配置 git 始终为提交签名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can tell git to always sign commits::

    git config --global commit.gpgSign true

(可以告诉 git 始终为提交签名：)

.. note::

    Make sure you configure ``gpg-agent`` before you turn this on.
    (在启用此选项之前，请确保你已经配置好 ``gpg-agent``。)

.. _verify_identities:

How to work with signed patches(如何使用签名补丁)
-------------------------------------------------

It is possible to use your PGP key to sign patches sent to kernel
developer mailing lists. Since existing email signature mechanisms
(PGP-Mime or PGP-inline) tend to cause problems with regular code
review tasks, you should use the tool kernel.org created for this
purpose that puts cryptographic attestation signatures into message
headers (a-la DKIM):
(可以使用你的 PGP 密钥为发送到内核开发者邮件列表的补丁签名。由于现有的邮件签名机制（PGP-Mime 或 PGP-inline）往往会给常规的代码审查工作带来问题，你应当使用 kernel.org 为此目的创建的工具，它把加密证明签名放入邮件消息头中（类似 DKIM）：)

- `Patatt Patch Attestation`_
  (Patatt 补丁证明。)

.. _`Patatt Patch Attestation`: https://pypi.org/project/patatt/

Installing and configuring patatt(安装和配置 patatt)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::

    If you use B4 to send in your patches, patatt is already installed
    and integrated into your workflow.
    (如果你使用 B4 发送补丁，那么 patatt 已经安装并集成到你的工作流程中。)

Patatt is packaged for many distributions already, so please check there
first. You can also install it from pypi using "``pip install patatt``".
(Patatt 已经为许多发行版打包，因此请先在发行版中查找。你也可以从 pypi 安装它，使用 "``pip install patatt``"。)

If you already have your PGP key configured with git (via the
``user.signingKey`` configuration parameter), then patatt requires no
further configuration. You can start signing your patches by installing
the git-send-email hook in the repository you want::

    patatt install-hook

(如果你已经把 PGP 密钥配置给了 git（通过 ``user.signingKey`` 配置参数），那么 patatt 无需进一步配置。在你想要的仓库中安装 git-send-email 钩子，即可开始为补丁签名：)

Now any patches you send with ``git send-email`` will be automatically
signed with your cryptographic signature.
(现在，你用 ``git send-email`` 发送的任何补丁都会自动附上你的加密签名。)

Checking patatt signatures(检查 patatt 签名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you are using ``b4`` to retrieve and apply patches, then it will
automatically attempt to verify all DKIM and patatt signatures it
encounters, for example::

    $ b4 am 20220720205013.890942-1-broonie@kernel.org
    [...]
    Checking attestation on all messages, may take a moment...
    ---
      ✓ [PATCH v1 1/3] kselftest/arm64: Correct buffer allocation for SVE Z registers
      ✓ [PATCH v1 2/3] arm64/sve: Document our actual ABI for clearing registers on syscall
      ✓ [PATCH v1 3/3] kselftest/arm64: Enforce actual ABI for SVE syscalls
      ---
      ✓ Signed: openpgp/broonie@kernel.org
      ✓ Signed: DKIM/kernel.org

(如果你使用 ``b4`` 获取和应用补丁，它会自动尝试验证遇到的所有 DKIM 和 patatt 签名，例如：)

.. note::

    Patatt and b4 are still in active development and you should check
    the latest documentation for these projects for any new or updated
    features.
    (Patatt 和 b4 仍在积极开发之中，有关任何新增或更新的特性，请查阅这些项目的最新文档。)

.. _kernel_identities:

How to verify kernel developer identities(如何验证内核开发者身份)
=================================================================

Signing tags and commits is straightforward, but how does one go about
verifying that the key used to sign something belongs to the actual
kernel developer and not to a malicious imposter?
(为标签和提交签名很简单，但要如何验证用于签名的密钥确实属于真正的内核开发者，而不是恶意的冒名顶替者呢？)

Configure auto-key-retrieval using WKD and DANE(使用 WKD 和 DANE 配置自动密钥获取)
----------------------------------------------------------------------------------

If you are not already someone with an extensive collection of other
developers' public keys, then you can jumpstart your keyring by relying
on key auto-discovery and auto-retrieval. GnuPG can piggyback on other
delegated trust technologies, namely DNSSEC and TLS, to get you going if
the prospect of starting your own Web of Trust from scratch is too
daunting.
(如果你还没有收集大量其他开发者的公钥，那么可以依靠密钥自动发现与自动获取来快速充实你的密钥环。如果从零开始建立自己的信任之网前景过于令人生畏，GnuPG 可以借助其他委托信任技术（即 DNSSEC 和 TLS）帮你起步。)

Add the following to your ``~/.gnupg/gpg.conf``::

    auto-key-locate wkd,dane,local
    auto-key-retrieve

(把以下内容添加到你的 ``~/.gnupg/gpg.conf``：)

DNS-Based Authentication of Named Entities ("DANE") is a method for
publishing public keys in DNS and securing them using DNSSEC signed
zones. Web Key Directory ("WKD") is the alternative method that uses
https lookups for the same purpose. When using either DANE or WKD for
looking up public keys, GnuPG will validate DNSSEC or TLS certificates,
respectively, before adding auto-retrieved public keys to your local
keyring.
(基于 DNS 的命名实体认证（"DANE"）是一种在 DNS 中发布公钥并使用 DNSSEC 签名区域加以保护的方法。Web 密钥目录（"WKD"）是另一种实现同一目的的方法，它使用 https 查询。使用 DANE 或 WKD 查找公钥时，GnuPG 会分别先验证 DNSSEC 或 TLS 证书，然后再把自动获取的公钥加入你的本地密钥环。)

Kernel.org publishes the WKD for all developers who have kernel.org
accounts. Once you have the above changes in your ``gpg.conf``, you can
auto-retrieve the keys for Linus Torvalds and Greg Kroah-Hartman (if you
don't already have them)::

    $ gpg --locate-keys torvalds@kernel.org gregkh@kernel.org

(Kernel.org 为所有拥有 kernel.org 账户的开发者发布 WKD。在你的 ``gpg.conf`` 中完成上述更改后，你可以自动获取 Linus Torvalds 和 Greg Kroah-Hartman 的密钥（如果你还没有的话）：)

If you have a kernel.org account, then you should `add the kernel.org
UID to your key`_ to make WKD more useful to other kernel developers.
(如果你拥有 kernel.org 账户，那么应当把 kernel.org UID 添加到你的密钥中，让 WKD 对其他内核开发者更加有用。)

.. _`add the kernel.org UID to your key`: https://korg.docs.kernel.org/mail.html#adding-a-kernel-org-uid-to-your-pgp-key

Web of Trust (WOT) vs. Trust on First Use (TOFU)(信任之网 (WOT) 与首次使用信任 (TOFU))
--------------------------------------------------------------------------------------

PGP incorporates a trust delegation mechanism known as the "Web of
Trust." At its core, this is an attempt to replace the need for
centralized Certification Authorities of the HTTPS/TLS world. Instead of
various software makers dictating who should be your trusted certifying
entity, PGP leaves this responsibility to each user.
(PGP 内置了一种被称为"信任之网"（Web of Trust）的信任委托机制。其核心是尝试取代 HTTPS/TLS 世界中对集中式认证机构（CA）的需求。PGP 不是由各家软件厂商来规定谁应当是你信任的签发实体，而是把这一责任留给每个用户。)

Unfortunately, very few people understand how the Web of Trust works.
While it is still an important part of the OpenPGP specification,
recent versions of GnuPG (2.2 and above) have implemented an alternative
mechanism called "Trust on First Use" (TOFU). You can think of TOFU as
"the SSH-like approach to trust." With SSH, the first time you connect
to a remote system, its key fingerprint is recorded and remembered. If
the key changes in the future, the SSH client will alert you and refuse
to connect, forcing you to make a decision on whether you choose to
trust the changed key or not. Similarly, the first time you import
someone's PGP key, it is assumed to be valid. If at any point in the
future GnuPG comes across another key with the same identity, both the
previously imported key and the new key will be marked for verification
and you will need to manually figure out which one to keep.
(遗憾的是，很少有人理解信任之网是如何运作的。虽然它仍然是 OpenPGP 规范的重要组成部分，但较新版本的 GnuPG（2.2 及以上）实现了一种称为"首次使用信任"（TOFU）的替代机制。你可以把 TOFU 理解为"类似 SSH 的信任方式"。在 SSH 中，第一次连接远程系统时，其密钥指纹会被记录并记住。如果日后密钥发生变化，SSH 客户端会发出警告并拒绝连接，迫使你决定是否信任更改后的密钥。类似地，第一次导入某人的 PGP 密钥时，它被默认视为有效。如果将来 GnuPG 遇到另一把具有相同身份的密钥，先前导入的密钥和新密钥都会被标记为待验证，你需要手动弄清楚该保留哪一把。)

We recommend that you use the combined TOFU+PGP trust model (which is
the new default in GnuPG v2). To set it, add (or modify) the
``trust-model`` setting in ``~/.gnupg/gpg.conf``::

    trust-model tofu+pgp

(我们建议使用 TOFU+PGP 组合信任模型（这是 GnuPG v2 的新默认值）。要设置它，请在 ``~/.gnupg/gpg.conf`` 中添加（或修改）``trust-model`` 设置：)

.. _kernel_org_trust_repository:

Using the kernel.org web of trust repository(使用 kernel.org 信任之网仓库)
--------------------------------------------------------------------------

Kernel.org maintains a git repository with developers' public keys as a
replacement for replicating keyserver networks that have gone mostly
dark in the past few years. The full documentation for how to set up
that repository as your source of public keys can be found here:
(Kernel.org 维护着一个包含开发者公钥的 git 仓库，用来取代近几年来大多已经停摆的密钥服务器网络。关于如何把该仓库设置为你的公钥来源的完整文档可以在这里找到：)

- `Kernel developer PGP Keyring`_
  (内核开发者 PGP 密钥环。)

If you are a kernel developer, please consider submitting your key for
inclusion into that keyring.
(如果你是内核开发者，请考虑提交你的密钥，将其纳入该密钥环。)

.. _`Kernel developer PGP Keyring`: https://korg.docs.kernel.org/pgpkeys.html
