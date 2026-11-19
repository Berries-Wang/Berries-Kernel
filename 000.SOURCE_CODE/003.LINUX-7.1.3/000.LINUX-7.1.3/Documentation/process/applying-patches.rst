.. _applying_patches:

Applying Patches To The Linux Kernel(将补丁应用到 Linux 内核)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Original by:
	Jesper Juhl, August 2005
	(原作者：Jesper Juhl，2005 年 8 月)

.. note::

   This document is obsolete.  In most cases, rather than using ``patch``
   manually, you'll almost certainly want to look at using Git instead.
   (本文档已过时。在大多数情况下，与其手动使用 ``patch`` ，你几乎肯定应该考虑改用 Git。)

A frequently asked question on the Linux Kernel Mailing List is how to apply
a patch to the kernel or, more specifically, what base kernel a patch for
one of the many trees/branches should be applied to. Hopefully this document
will explain this to you.
(Linux 内核邮件列表上一个常见问题是：如何把补丁应用到内核上，或者更具体地说，针对众多树/分支之一的补丁应该以哪个基础内核为基准来应用。希望本文档能为你解答这个问题。)

In addition to explaining how to apply and revert patches, a brief
description of the different kernel trees (and examples of how to apply
their specific patches) is also provided.
(除了讲解如何应用与回退补丁之外，本文档还简要介绍了不同的内核树(以及应用其专属补丁的示例)。)


What is a patch?(什么是补丁？)
==============================

A patch is a small text document containing a delta of changes between two
different versions of a source tree. Patches are created with the ``diff``
program.
(补丁是一个小型文本文件，记录了源码树两个不同版本之间的变更增量。补丁用 ``diff`` 程序生成。)

To correctly apply a patch you need to know what base it was generated from
and what new version the patch will change the source tree into. These
should both be present in the patch file metadata or be possible to deduce
from the filename.
(要正确应用补丁，你需要知道它是基于什么生成的，以及它会把源码树变成哪个新版本。这两者都应能在补丁文件的元数据中找到，或能从文件名推断出来。)


How do I apply or revert a patch?(如何应用或回退补丁？)
=======================================================

You apply a patch with the ``patch`` program. The patch program reads a diff
(or patch) file and makes the changes to the source tree described in it.
(你使用 ``patch`` 程序来应用补丁。patch 程序读取 diff(或 patch)文件，并按其中描述对源码树做出更改。)

Patches for the Linux kernel are generated relative to the parent directory
holding the kernel source dir.
(Linux 内核的补丁是相对于存放内核源码目录的上一级目录生成的。)

This means that paths to files inside the patch file contain the name of the
kernel source directories it was generated against (or some other directory
names like "a/" and "b/").
(这意味着补丁文件内的文件路径会包含它生成时所针对的内核源码目录的名字(或某些其他目录名，如 "a/" 和 "b/")。)

Since this is unlikely to match the name of the kernel source dir on your
local machine (but is often useful info to see what version an otherwise
unlabeled patch was generated against) you should change into your kernel
source directory and then strip the first element of the path from filenames
in the patch file when applying it (the ``-p1`` argument to ``patch`` does
this).
(由于这很可能与你本地机器上内核源码目录的名字不一致(不过它常常是有用信息，可以看出一个没有标注版本的补丁是基于哪个版本生成的)，你应当先进入你的内核源码目录，再在应用补丁时剥去补丁文件中文件路径的第一级路径元素(``patch`` 的 ``-p1`` 参数就是做这件事的)。)

To revert a previously applied patch, use the -R argument to patch.
So, if you applied a patch like this::

	patch -p1 < ../patch-x.y.z

(要回退先前应用过的补丁，给 patch 加 -R 参数即可。也就是说，如果你像这样应用了补丁：)

You can revert (undo) it like this::

	patch -R -p1 < ../patch-x.y.z

(就可以像这样回退(撤销)它：)


How do I feed a patch/diff file to ``patch``?(如何把 patch/diff 文件喂给 ``patch``？)
=====================================================================================

This (as usual with Linux and other UNIX like operating systems) can be
done in several different ways.
(这(与 Linux 及其他类 UNIX 操作系统的惯例一样)有几种不同的做法。)

In all the examples below I feed the file (in uncompressed form) to patch
via stdin using the following syntax::

	patch -p1 < path/to/patch-x.y.z

(在下面所有示例中，我都用以下语法把(未压缩的)文件经标准输入喂给 patch：)

If you just want to be able to follow the examples below and don't want to
know of more than one way to use patch, then you can stop reading this
section here.
(如果你只是想能跟上下面的示例，而不想了解不止一种使用 patch 的方法，那么这一节读到这里就可以停下了。)

Patch can also get the name of the file to use via the -i argument, like
this::

	patch -p1 -i path/to/patch-x.y.z

(patch 也可以通过 -i 参数获得要使用的文件名，如下：)

If your patch file is compressed with gzip or xz and you don't want to
uncompress it before applying it, then you can feed it to patch like this
instead::

	xzcat path/to/patch-x.y.z.xz | patch -p1
	bzcat path/to/patch-x.y.z.gz | patch -p1

(如果你的补丁文件是用 gzip 或 xz 压缩的，而你又不想在应用前先解压，那么可以这样把它喂给 patch：)

If you wish to uncompress the patch file by hand first before applying it
(what I assume you've done in the examples below), then you simply run
gunzip or xz on the file -- like this::

	gunzip patch-x.y.z.gz
	xz -d patch-x.y.z.xz

(如果你想在应用前先手动解压补丁文件(下面的示例都假设你已经这么做了)，那么只需对文件运行 gunzip 或 xz——如下：)

Which will leave you with a plain text patch-x.y.z file that you can feed to
patch via stdin or the ``-i`` argument, as you prefer.
(这样你就会得到一个纯文本的 patch-x.y.z 文件，可以按你的喜好经标准输入或用 ``-i`` 参数喂给 patch。)

A few other nice arguments for patch are ``-s`` which causes patch to be silent
except for errors which is nice to prevent errors from scrolling out of the
screen too fast, and ``--dry-run`` which causes patch to just print a listing of
what would happen, but doesn't actually make any changes. Finally ``--verbose``
tells patch to print more information about the work being done.
(patch 还有几个好用的参数：``-s`` 让 patch 除错误信息外保持安静，这有助于错误信息不至于滚出屏幕太快；``--dry-run`` 让 patch 只打印将要发生什么而不真正做任何更改；最后的 ``--verbose`` 让 patch 打印更多关于正在进行的工作的信息。)


Common errors when patching(打补丁时的常见错误)
===============================================

When patch applies a patch file it attempts to verify the sanity of the
file in different ways.
(patch 应用补丁文件时，会尝试用不同方式检查该文件的健全性。)

Checking that the file looks like a valid patch file and checking the code
around the bits being modified matches the context provided in the patch are
just two of the basic sanity checks patch does.
(检查文件看起来是否是有效的补丁文件，以及检查被修改位置周围的代码是否与补丁提供的上下文匹配，只是 patch 所做的基础健全性检查中的两项。)

If patch encounters something that doesn't look quite right it has two
options. It can either refuse to apply the changes and abort or it can try
to find a way to make the patch apply with a few minor changes.
(如果 patch 遇到看起来不太对劲的东西，它有两个选择：要么拒绝应用更改并中止，要么设法通过少量微调让补丁应用成功。)

One example of something that's not 'quite right' that patch will attempt to
fix up is if all the context matches, the lines being changed match, but the
line numbers are different. This can happen, for example, if the patch makes
a change in the middle of the file but for some reasons a few lines have
been added or removed near the beginning of the file. In that case
everything looks good it has just moved up or down a bit, and patch will
usually adjust the line numbers and apply the patch.
(patch 会尝试修好的"不太对劲"的一个例子是：上下文全部匹配、被更改的行也匹配，只是行号不同。例如，补丁修改文件中部，而由于某些原因文件开头附近被增删了几行，就会出现这种情况。此时一切都正常，只是位置上下移动了一点，patch 通常会调整行号并应用补丁。)

Whenever patch applies a patch that it had to modify a bit to make it fit
it'll tell you about it by saying the patch applied with **fuzz**.
You should be wary of such changes since even though patch probably got it
right it doesn't /always/ get it right, and the result will sometimes be
wrong.
(每当 patch 应用了一个需要稍作修改才能套上的补丁，它都会告诉你：该补丁以 **fuzz** (模糊匹配)方式应用了。你应当警惕这类更改——尽管 patch 很可能改对了，但它并不总是改对，结果有时会是错的。)

When patch encounters a change that it can't fix up with fuzz it rejects it
outright and leaves a file with a ``.rej`` extension (a reject file). You can
read this file to see exactly what change couldn't be applied, so you can
go fix it up by hand if you wish.
(当 patch 遇到无法用 fuzz 修好的更改时，它会直接拒绝，并留下一个以 ``.rej`` 为扩展名的文件(拒绝文件)。你可以阅读该文件，确切了解哪些更改没能应用，从而可以按自己的意愿手工修好它们。)

If you don't have any third-party patches applied to your kernel source, but
only patches from kernel.org and you apply the patches in the correct order,
and have made no modifications yourself to the source files, then you should
never see a fuzz or reject message from patch. If you do see such messages
anyway, then there's a high risk that either your local source tree or the
patch file is corrupted in some way. In that case you should probably try
re-downloading the patch and if things are still not OK then you'd be advised
to start with a fresh tree downloaded in full from kernel.org.
(如果你的内核源码上没有应用任何第三方补丁，只有来自 kernel.org 的补丁，而且你按正确顺序应用补丁、也未曾自己修改过源文件，那么你理应永远不会看到 patch 的 fuzz 或 reject 消息。如果你还是看到了这类消息，那么很可能你的本地源码树或补丁文件在某种程度上已损坏。此时你或许应当尝试重新下载补丁；如果问题依旧，建议你从 kernel.org 重新完整下载一棵全新的树。)

Let's look a bit more at some of the messages patch can produce.
(我们再仔细看看 patch 可能产生的一些消息。)

If patch stops and presents a ``File to patch:`` prompt, then patch could not
find a file to be patched. Most likely you forgot to specify -p1 or you are
in the wrong directory. Less often, you'll find patches that need to be
applied with ``-p0`` instead of ``-p1`` (reading the patch file should reveal if
this is the case -- if so, then this is an error by the person who created
the patch but is not fatal).
(如果 patch 停下来并显示 ``File to patch:`` 提示符，说明 patch 找不到要打补丁的文件。最可能是你忘了指定 -p1，或者你不在正确的目录里。较少见的情况是，有些补丁需要用 ``-p0`` 而非 ``-p1`` 来应用(阅读补丁文件应当能看出是否属于这种情况——若是，则这是补丁创建者的失误，但并非致命)。)

If you get ``Hunk #2 succeeded at 1887 with fuzz 2 (offset 7 lines).`` or a
message similar to that, then it means that patch had to adjust the location
of the change (in this example it needed to move 7 lines from where it
expected to make the change to make it fit).
(如果你得到 ``Hunk #2 succeeded at 1887 with fuzz 2 (offset 7 lines).`` 或类似消息，说明 patch 不得不调整更改的位置(本例中它需要把更改位置从预期处移动 7 行才能套上)。)

The resulting file may or may not be OK, depending on the reason the file
was different than expected.
(结果文件是否正常，取决于文件与预期不同的原因。)

This often happens if you try to apply a patch that was generated against a
different kernel version than the one you are trying to patch.
(如果你试图应用一个基于与你正在打补丁的内核版本不同的版本所生成的补丁，这种情况经常发生。)

If you get a message like ``Hunk #3 FAILED at 2387.``, then it means that the
patch could not be applied correctly and the patch program was unable to
fuzz its way through. This will generate a ``.rej`` file with the change that
caused the patch to fail and also a ``.orig`` file showing you the original
content that couldn't be changed.
(如果你得到类似 ``Hunk #3 FAILED at 2387.`` 的消息，说明补丁无法正确应用，patch 程序也无法靠 fuzz 硬套过去。这会生成一个 ``.rej`` 文件，内含导致补丁失败的更改，以及一个 ``.orig`` 文件，显示无法被更改的原始内容。)

If you get ``Reversed (or previously applied) patch detected!  Assume -R? [n]``
then patch detected that the change contained in the patch seems to have
already been made.
(如果你得到 ``Reversed (or previously applied) patch detected!  Assume -R? [n]``，说明 patch 检测到补丁所包含的更改似乎已经做过了。)

If you actually did apply this patch previously and you just re-applied it
in error, then just say [n]o and abort this patch. If you applied this patch
previously and actually intended to revert it, but forgot to specify -R,
then you can say [**y**]es here to make patch revert it for you.
(如果你之前确实应用过这个补丁，只是误操作又应用了一次，那么回答 [n]o 中止即可。如果你之前应用过这个补丁，而本意是想回退它却忘了指定 -R，那么这里可以回答 [**y**]es，让 patch 替你回退。)

This can also happen if the creator of the patch reversed the source and
destination directories when creating the patch, and in that case reverting
the patch will in fact apply it.
(如果补丁创建者在创建补丁时把源目录与目标目录弄反了，也会出现这种情况；此时回退补丁实际上就是在应用它。)

A message similar to ``patch: **** unexpected end of file in patch`` or
``patch unexpectedly ends in middle of line`` means that patch could make no
sense of the file you fed to it. Either your download is broken, you tried to
feed patch a compressed patch file without uncompressing it first, or the patch
file that you are using has been mangled by a mail client or mail transfer
agent along the way somewhere, e.g., by splitting a long line into two lines.
Often these warnings can easily be fixed by joining (concatenating) the
two lines that had been split.
(类似 ``patch: **** unexpected end of file in patch`` 或 ``patch unexpectedly ends in middle of line`` 的消息意味着 patch 完全无法理解你喂给它的文件。要么是你的下载损坏了，要么你试图把压缩的补丁文件未经解压就喂给 patch，要么你使用的补丁文件在途中被邮件客户端或邮件传输代理弄乱了——例如把一行长行拆成了两行。这类警告通常很容易修复：把被拆开的两行重新拼接(合并)起来即可。)

As I already mentioned above, these errors should never happen if you apply
a patch from kernel.org to the correct version of an unmodified source tree.
So if you get these errors with kernel.org patches then you should probably
assume that either your patch file or your tree is broken and I'd advise you
to start over with a fresh download of a full kernel tree and the patch you
wish to apply.
(正如上文已经提到的，如果你把来自 kernel.org 的补丁应用到正确版本的未修改源码树上，这些错误本不应发生。因此，如果你在使用 kernel.org 补丁时遇到这些错误，那么你大概应该假定补丁文件或你的树已损坏；我建议你重新开始——完整地重新下载一棵内核树以及你想应用的补丁。)


Are there any alternatives to ``patch``?(有没有 ``patch`` 的替代品？)
=====================================================================


Yes there are alternatives.
(有替代品。)

You can use the ``interdiff`` program (http://cyberelk.net/tim/patchutils/) to
generate a patch representing the differences between two patches and then
apply the result.
(你可以使用 ``interdiff`` 程序(http://cyberelk.net/tim/patchutils/)生成一个表示两个补丁之间差异的补丁，然后应用这个结果。)

This will let you move from something like 5.7.2 to 5.7.3 in a single
step. The -z flag to interdiff will even let you feed it patches in gzip or
bzip2 compressed form directly without the use of zcat or bzcat or manual
decompression.
(这可以让你一步从诸如 5.7.2 升级到 5.7.3。interdiff 的 -z 选项甚至让你直接以 gzip 或 bzip2 压缩形式喂给它补丁，无需使用 zcat 或 bzcat，也无需手动解压。)

Here's how you'd go from 5.7.2 to 5.7.3 in a single step::

	interdiff -z ../patch-5.7.2.gz ../patch-5.7.3.gz | patch -p1

(下面是如何一步从 5.7.2 升级到 5.7.3：)

Although interdiff may save you a step or two you are generally advised to
do the additional steps since interdiff can get things wrong in some cases.
(虽然 interdiff 可以为你省去一两步，但一般仍建议你多做那几步，因为 interdiff 在某些情况下会出错。)

Another alternative is ``ketchup``, which is a python script for automatic
downloading and applying of patches (https://www.selenic.com/ketchup/).
(另一个替代品是 ``ketchup``，这是一个自动下载并应用补丁的 Python 脚本(https://www.selenic.com/ketchup/)。)

Other nice tools are diffstat, which shows a summary of changes made by a
patch; lsdiff, which displays a short listing of affected files in a patch
file, along with (optionally) the line numbers of the start of each patch;
and grepdiff, which displays a list of the files modified by a patch where
the patch contains a given regular expression.
(其他好用的工具还有：diffstat，显示某个补丁所做更改的摘要；lsdiff，列出补丁文件中受影响的文件的简表，还可以(可选)显示每个补丁块起始处的行号；grepdiff，当补丁包含给定正则表达式时，显示被该补丁修改的文件列表。)


Where can I download the patches?(在哪里可以下载补丁？)
=======================================================

The patches are available at https://kernel.org/
Most recent patches are linked from the front page, but they also have
specific homes.
(补丁可在 https://kernel.org/ 获取。最新的补丁从首页链接，但它们也有各自的存放位置。)

The 5.x.y (-stable) and 5.x patches live at
(5.x.y(-stable)与 5.x 补丁位于)

	https://www.kernel.org/pub/linux/kernel/v5.x/

The 5.x.y incremental patches live at
(5.x.y 增量补丁位于)

	https://www.kernel.org/pub/linux/kernel/v5.x/incr/

The -rc patches are not stored on the webserver but are generated on
demand from git tags such as
(-rc 补丁不存放在 Web 服务器上，而是按需从 git 标签生成，例如)

	https://git.kernel.org/torvalds/p/v5.1-rc1/v5.0

The stable -rc patches live at
(stable 的 -rc 补丁位于)

	https://www.kernel.org/pub/linux/kernel/v5.x/stable-review/


The 5.x kernels(5.x 内核)
=========================

These are the base stable releases released by Linus. The highest numbered
release is the most recent.
(这些是 Linus 发布的基础稳定版本。编号最高的版本是最近的。)

If regressions or other serious flaws are found, then a -stable fix patch
will be released (see below) on top of this base. Once a new 5.x base
kernel is released, a patch is made available that is a delta between the
previous 5.x kernel and the new one.
(如果发现回归或其他严重缺陷，将在此基础之上发布 -stable 修复补丁(见下文)。一旦发布新的 5.x 基础内核，就会提供一个表示上一个 5.x 内核与新内核之间差异的补丁。)

To apply a patch moving from 5.6 to 5.7, you'd do the following (note
that such patches do **NOT** apply on top of 5.x.y kernels but on top of the
base 5.x kernel -- if you need to move from 5.x.y to 5.x+1 you need to
first revert the 5.x.y patch).
(要应用从 5.6 升级到 5.7 的补丁，你需要执行以下操作(注意：这类补丁 **不能** 应用在 5.x.y 内核之上，而只能应用在基础 5.x 内核之上——如果你需要从 5.x.y 升级到 5.x+1，必须先回退 5.x.y 补丁)。)

Here are some examples::

	# moving from 5.6 to 5.7

	$ cd ~/linux-5.6		# change to kernel source dir
	$ patch -p1 < ../patch-5.7	# apply the 5.7 patch
	$ cd ..
	$ mv linux-5.6 linux-5.7	# rename source dir

	# moving from 5.6.1 to 5.7

	$ cd ~/linux-5.6.1		# change to kernel source dir
	$ patch -p1 -R < ../patch-5.6.1	# revert the 5.6.1 patch
					# source dir is now 5.6
	$ patch -p1 < ../patch-5.7	# apply new 5.7 patch
	$ cd ..
	$ mv linux-5.6.1 linux-5.7	# rename source dir

(下面是一些示例：)


The 5.x.y kernels(5.x.y 内核)
=============================

Kernels with 3-digit versions are -stable kernels. They contain small(ish)
critical fixes for security problems or significant regressions discovered
in a given 5.x kernel.
(版本号为三位的内核是 -stable 内核。它们包含针对某个给定 5.x 内核中发现的安全问题或重大回归的小型(大致如此)关键修复。)

This is the recommended branch for users who want the most recent stable
kernel and are not interested in helping test development/experimental
versions.
(对于想要最新稳定内核、而对协助测试开发/实验版本不感兴趣的用户，这是推荐的分支。)

If no 5.x.y kernel is available, then the highest numbered 5.x kernel is
the current stable kernel.
(如果没有可用的 5.x.y 内核，那么编号最高的 5.x 内核就是当前的稳定内核。)

The -stable team provides normal as well as incremental patches. Below is
how to apply these patches.
(-stable 团队既提供普通补丁也提供增量补丁。下面介绍如何应用这些补丁。)

Normal patches(普通补丁)
~~~~~~~~~~~~~~~~~~~~~~~~

These patches are not incremental, meaning that for example the 5.7.3
patch does not apply on top of the 5.7.2 kernel source, but rather on top
of the base 5.7 kernel source.
(这类补丁不是增量的，也就是说，例如 5.7.3 补丁不能应用在 5.7.2 内核源码之上，而是应用在基础 5.7 内核源码之上。)

So, in order to apply the 5.7.3 patch to your existing 5.7.2 kernel
source you have to first back out the 5.7.2 patch (so you are left with a
base 5.7 kernel source) and then apply the new 5.7.3 patch.
(因此，要把 5.7.3 补丁应用到现有的 5.7.2 内核源码上，你必须先撤销 5.7.2 补丁(这样你就回到了基础 5.7 内核源码)，然后再应用新的 5.7.3 补丁。)

Here's a small example::

	$ cd ~/linux-5.7.2		# change to the kernel source dir
	$ patch -p1 -R < ../patch-5.7.2	# revert the 5.7.2 patch
	$ patch -p1 < ../patch-5.7.3	# apply the new 5.7.3 patch
	$ cd ..
	$ mv linux-5.7.2 linux-5.7.3	# rename the kernel source dir

(下面是一个小示例：)

Incremental patches(增量补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Incremental patches are different: instead of being applied on top
of base 5.x kernel, they are applied on top of previous stable kernel
(5.x.y-1).
(增量补丁不同：它们不是应用在基础 5.x 内核之上，而是应用在上一个稳定内核(5.x.y-1)之上。)

Here's the example to apply these::

	$ cd ~/linux-5.7.2		# change to the kernel source dir
	$ patch -p1 < ../patch-5.7.2-3	# apply the new 5.7.3 patch
	$ cd ..
	$ mv linux-5.7.2 linux-5.7.3	# rename the kernel source dir

(下面是应用这类补丁的示例：)


The -rc kernels(-rc 内核)
=========================

These are release-candidate kernels. These are development kernels released
by Linus whenever he deems the current git (the kernel's source management
tool) tree to be in a reasonably sane state adequate for testing.
(这些是发布候选(release-candidate)内核。每当 Linus 认为当前 git(内核的源码管理工具)树处于足以供测试的、相当健全的状态时，就会发布这样的开发内核。)

These kernels are not stable and you should expect occasional breakage if
you intend to run them. This is however the most stable of the main
development branches and is also what will eventually turn into the next
stable kernel, so it is important that it be tested by as many people as
possible.
(这些内核并不稳定，如果你打算运行它们，应当预期偶发的故障。不过，它们是各主要开发分支中最稳定的一个，也终将变成下一个稳定内核，因此让尽可能多的人来测试它非常重要。)

This is a good branch to run for people who want to help out testing
development kernels but do not want to run some of the really experimental
stuff (such people should see the sections about -next and -mm kernels below).
(对于想协助测试开发内核、但又不想运行那些真正实验性东西的人，这是一个适合运行的分支(这类人应参阅下文关于 -next 与 -mm 内核的小节)。)

The -rc patches are not incremental, they apply to a base 5.x kernel, just
like the 5.x.y patches described above. The kernel version before the -rcN
suffix denotes the version of the kernel that this -rc kernel will eventually
turn into.
(-rc 补丁不是增量的，它们应用于基础 5.x 内核，与上文描述的 5.x.y 补丁一样。-rcN 后缀之前的内核版本号表示该 -rc 内核最终将变成的内核版本。)

So, 5.8-rc5 means that this is the fifth release candidate for the 5.8
kernel and the patch should be applied on top of the 5.7 kernel source.
(因此，5.8-rc5 表示这是 5.8 内核的第五个发布候选版，该补丁应应用在 5.7 内核源码之上。)

Here are 3 examples of how to apply these patches::

	# first an example of moving from 5.7 to 5.8-rc3

	$ cd ~/linux-5.7			# change to the 5.7 source dir
	$ patch -p1 < ../patch-5.8-rc3		# apply the 5.8-rc3 patch
	$ cd ..
	$ mv linux-5.7 linux-5.8-rc3		# rename the source dir

	# now let's move from 5.8-rc3 to 5.8-rc5

	$ cd ~/linux-5.8-rc3			# change to the 5.8-rc3 dir
	$ patch -p1 -R < ../patch-5.8-rc3	# revert the 5.8-rc3 patch
	$ patch -p1 < ../patch-5.8-rc5		# apply the new 5.8-rc5 patch
	$ cd ..
	$ mv linux-5.8-rc3 linux-5.8-rc5	# rename the source dir

	# finally let's try and move from 5.7.3 to 5.8-rc5

	$ cd ~/linux-5.7.3			# change to the kernel source dir
	$ patch -p1 -R < ../patch-5.7.3		# revert the 5.7.3 patch
	$ patch -p1 < ../patch-5.8-rc5		# apply new 5.8-rc5 patch
	$ cd ..
	$ mv linux-5.7.3 linux-5.8-rc5		# rename the kernel source dir

(下面是应用这类补丁的 3 个示例：)


The -mm patches and the linux-next tree(-mm 补丁与 linux-next 树)
=================================================================

The -mm patches are experimental patches released by Andrew Morton.
(-mm 补丁是由 Andrew Morton 发布的实验性补丁。)

In the past, -mm tree were used to also test subsystem patches, but this
function is now done via the
`linux-next` (https://www.kernel.org/doc/man-pages/linux-next.html)
tree. The Subsystem maintainers push their patches first to linux-next,
and, during the merge window, sends them directly to Linus.
(过去，-mm 树也被用来测试子系统补丁，但这一功能现在由 `linux-next` (https://www.kernel.org/doc/man-pages/linux-next.html) 树承担。子系统的维护者先把补丁推送到 linux-next，然后在合并窗口期间把它们直接发送给 Linus。)

The -mm patches serve as a sort of proving ground for new features and other
experimental patches that aren't merged via a subsystem tree.
Once such patches has proved its worth in -mm for a while Andrew pushes
it on to Linus for inclusion in mainline.
(-mm 补丁相当于一块试验场，用于那些未经子系统树合并的新特性及其他实验性补丁。一旦这类补丁在 -mm 中验证了一段时间并证明其价值，Andrew 就会把它推给 Linus，以纳入主线。)

The linux-next tree is daily updated, and includes the -mm patches.
Both are in constant flux and contains many experimental features, a
lot of debugging patches not appropriate for mainline etc., and is the most
experimental of the branches described in this document.
(linux-next 树每天更新，并包含 -mm 补丁。两者都处于不断变动之中，包含许多实验性特性、大量不适合进入主线的调试补丁等，是本文档所述各分支中最具实验性的。)

These patches are not appropriate for use on systems that are supposed to be
stable and they are more risky to run than any of the other branches (make
sure you have up-to-date backups -- that goes for any experimental kernel but
even more so for -mm patches or using a Kernel from the linux-next tree).
(这些补丁不适合用于本应保持稳定的系统，而且运行它们比其他任何分支都更有风险(务必保有最新的备份——这对任何实验性内核都适用，对 -mm 补丁或使用来自 linux-next 树的内核更是如此)。)

Testing of -mm patches and linux-next is greatly appreciated since the whole
point of those are to weed out regressions, crashes, data corruption bugs,
build breakage (and any other bug in general) before changes are merged into
the more stable mainline Linus tree.
(对 -mm 补丁和 linux-next 的测试将不胜感激，因为它们存在的全部意义就在于：在更改被合并进更稳定的主线 Linus 树之前，清除回归、崩溃、数据损坏缺陷、构建损坏(以及一般意义上的任何其他缺陷)。)

But testers of -mm and linux-next should be aware that breakages are
more common than in any other tree.
(但 -mm 与 linux-next 的测试者应当意识到：损坏的发生比其他任何树都更频繁。)


This concludes this list of explanations of the various kernel trees.
I hope you are now clear on how to apply the various patches and help testing
the kernel.
(以上就是关于各种内核树的说明清单。希望你现在已清楚如何应用各种补丁并协助测试内核。)

Thank you's to Randy Dunlap, Rolf Eike Beer, Linus Torvalds, Bodo Eggert,
Johannes Stezenbach, Grant Coady, Pavel Machek and others that I may have
forgotten for their reviews and contributions to this document.
(感谢 Randy Dunlap、Rolf Eike Beer、Linus Torvalds、Bodo Eggert、Johannes Stezenbach、Grant Coady、Pavel Machek 以及其他我可能遗漏的人对本文件的审阅与贡献。)
