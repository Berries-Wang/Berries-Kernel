.. SPDX-License-Identifier: GPL-2.0

========================================================
Backporting and conflict resolution(向后移植与冲突解决)
========================================================

:Author: Vegard Nossum <vegard.nossum@oracle.com>
(作者：Vegard Nossum <vegard.nossum@oracle.com>)

.. contents::
    :local:
    :depth: 3
    :backlinks: none

Introduction(引言)
==================

Some developers may never really have to deal with backporting patches,
merging branches, or resolving conflicts in their day-to-day work, so
when a merge conflict does pop up, it can be daunting. Luckily,
resolving conflicts is a skill like any other, and there are many useful
techniques you can use to make the process smoother and increase your
confidence in the result.
(一些开发者在日常工作中可能从未真正需要处理补丁的向后移植、分支合并或冲突解决，因此当合并冲突真的出现时，可能会令人望而生畏。幸运的是，解决冲突和其他技能一样是可以习得的，有许多实用的技巧可以让整个过程更顺畅，并增强你对结果的信心。)

This document aims to be a comprehensive, step-by-step guide to
backporting and conflict resolution.
(本文档旨在成为一份全面的、逐步讲解向后移植与冲突解决的指南。)

Applying the patch to a tree(把补丁应用到树上)
==============================================

Sometimes the patch you are backporting already exists as a git commit,
in which case you just cherry-pick it directly using
``git cherry-pick``. However, if the patch comes from an email, as it
often does for the Linux kernel, you will need to apply it to a tree
using ``git am``.
(有时你要向后移植的补丁已经以 git commit 的形式存在，这种情况下直接用 ``git cherry-pick`` 摘取即可。不过，如果补丁来自电子邮件——对 Linux 内核来说这很常见——你就需要用 ``git am`` 把它应用到树上。)

If you've ever used ``git am``, you probably already know that it is
quite picky about the patch applying perfectly to your source tree. In
fact, you've probably had nightmares about ``.rej`` files and trying to
edit the patch to make it apply.
(如果你用过 ``git am``，大概已经知道它对补丁能否完美地应用到你的源码树上相当挑剔。事实上，你可能已经做过关于 ``.rej`` 文件以及为了使补丁能应用而反复编辑补丁的噩梦。)

It is strongly recommended to instead find an appropriate base version
where the patch applies cleanly and *then* cherry-pick it over to your
destination tree, as this will make git output conflict markers and let
you resolve conflicts with the help of git and any other conflict
resolution tools you might prefer to use. For example, if you want to
apply a patch that just arrived on LKML to an older stable kernel, you
can apply it to the most recent mainline kernel and then cherry-pick it
to your older stable branch.
(强烈建议改为找到一个补丁能够干净应用的合适基础版本，*然后* 再把它摘取到你的目标树上，因为这样 git 会输出冲突标记，让你可以借助 git 以及任何你偏好的其他冲突解决工具来处理冲突。例如，如果你想把刚出现在 LKML 上的一个补丁应用到较旧的稳定内核上，可以把它应用到最新的主线内核上，然后再摘取到较旧的稳定分支。)

It's generally better to use the exact same base as the one the patch
was generated from, but it doesn't really matter that much as long as it
applies cleanly and isn't too far from the original base. The only
problem with applying the patch to the "wrong" base is that it may pull
in more unrelated changes in the context of the diff when cherry-picking
it to the older branch.
(通常最好使用与生成补丁时完全相同的基础版本，但只要补丁能干净应用、且与原始基础版本相差不太远，就没有太大关系。把补丁应用到 "错误" 基础版本的唯一问题在于，摘取到较旧分支时，diff 的上下文中可能带入更多无关的修改。)

A good reason to prefer ``git cherry-pick`` over ``git am`` is that git
knows the precise history of an existing commit, so it will know when
code has moved around and changed the line numbers; this in turn makes
it less likely to apply the patch to the wrong place (which can result
in silent mistakes or messy conflicts).
(相比 ``git am`` 更推荐 ``git cherry-pick`` 的一个好理由是：git 知道既有 commit 的精确历史，因此它能察觉代码位置的移动和行号的变化；这反过来降低了把补丁应用到错误位置的可能性 (应用到错误位置可能导致无声的错误或混乱的冲突)。)

If you are using `b4`_. and you are applying the patch directly from an
email, you can use ``b4 am`` with the options ``-g``/``--guess-base``
and ``-3``/``--prep-3way`` to do some of this automatically (see the
`b4 presentation`_ for more information). However, the rest of this
article will assume that you are doing a plain ``git cherry-pick``.
(如果你正在使用 `b4`_ 并直接从电子邮件应用补丁，可以用 ``b4 am`` 加上 ``-g``/``--guess-base`` 和 ``-3``/``--prep-3way`` 选项来自动完成其中的一部分 (更多信息见 `b4 presentation`_)。不过，本文其余部分将假设你使用的是普通的 ``git cherry-pick``。)

.. _b4: https://people.kernel.org/monsieuricon/introducing-b4-and-patch-attestation
.. _b4 presentation: https://youtu.be/mF10hgVIx9o?t=2996

Once you have the patch in git, you can go ahead and cherry-pick it into
your source tree. Don't forget to cherry-pick with ``-x`` if you want a
written record of where the patch came from!
(一旦补丁进入了 git，就可以继续把它摘取到你的源码树上。如果你想要一份补丁来源的书面记录，别忘了带上 ``-x`` 进行摘取！)

Note that if you are submitting a patch for stable, the format is
slightly different; the first line after the subject line needs to be
either::

    commit <upstream commit> upstream

(注意，如果你要向 stable 提交补丁，格式略有不同；主题行之后的第一行需要是以下两者之一：)

or::

    [ Upstream commit <upstream commit> ]

(或：)

Resolving conflicts(解决冲突)
=============================

Uh-oh; the cherry-pick failed with a vaguely threatening message::

    CONFLICT (content): Merge conflict

(糟糕，摘取失败，并给出了一条隐约带威胁意味的消息：)

What to do now?
(现在该怎么办？)

In general, conflicts appear when the context of the patch (i.e., the
lines being changed and/or the lines surrounding the changes) doesn't
match what's in the tree you are trying to apply the patch *to*.
(一般来说，当补丁的上下文 (即被修改的行和/或修改周围的行) 与你试图把补丁应用 *到* 的树中的内容不匹配时，就会出现冲突。)

For backports, what likely happened was that the branch you are
backporting from contains patches not in the branch you are backporting
to. However, the reverse is also possible. In any case, the result is a
conflict that needs to be resolved.
(对于向后移植而言，很可能发生的情况是：你从中向后移植的分支包含了目标分支所没有的补丁。不过反过来也可能成立。无论如何，结果就是一个需要解决的冲突。)

If your attempted cherry-pick fails with a conflict, git automatically
edits the files to include so-called conflict markers showing you where
the conflict is and how the two branches have diverged. Resolving the
conflict typically means editing the end result in such a way that it
takes into account these other commits.
(如果你的摘取尝试因冲突而失败，git 会自动编辑文件，加入所谓的冲突标记，向你显示冲突在哪里以及两个分支如何分叉。解决冲突通常意味着编辑最终结果，使之把这些其他 commit 考虑在内。)

Resolving the conflict can be done either by hand in a regular text
editor or using a dedicated conflict resolution tool.
(解决冲突既可以手工在普通文本编辑器中进行，也可以使用专门的冲突解决工具。)

Many people prefer to use their regular text editor and edit the
conflict directly, as it may be easier to understand what you're doing
and to control the final result. There are definitely pros and cons to
each method, and sometimes there's value in using both.
(许多人更愿意使用自己惯用的文本编辑器直接编辑冲突，因为这样更容易理解自己在做什么、也更容易控制最终结果。每种方法肯定各有利弊，有时两者结合使用也有价值。)

We will not cover using dedicated merge tools here beyond providing some
pointers to various tools that you could use:
(这里不打算深入介绍专门合并工具的使用，只给出一些你可能用得上的工具的指引：)

-  `Emacs Ediff mode <https://www.emacswiki.org/emacs/EdiffMode>`__
-  `vimdiff/gvimdiff <https://linux.die.net/man/1/vimdiff>`__
-  `KDiff3 <http://kdiff3.sourceforge.net/>`__
-  `TortoiseMerge <https://tortoisesvn.net/TortoiseMerge.html>`__
-  `Meld <https://meldmerge.org/help/>`__
-  `P4Merge <https://www.perforce.com/products/helix-core-apps/merge-diff-tool-p4merge>`__
-  `Beyond Compare <https://www.scootersoftware.com/>`__
-  `IntelliJ <https://www.jetbrains.com/help/idea/resolve-conflicts.html>`__
-  `VSCode <https://code.visualstudio.com/docs/editor/versioncontrol>`__

To configure git to work with these, see ``git mergetool --help`` or
the official `git-mergetool documentation`_.
(要配置 git 使用这些工具，见 ``git mergetool --help`` 或官方的 `git-mergetool documentation`_。)

.. _git-mergetool documentation: https://git-scm.com/docs/git-mergetool

Prerequisite patches(前置补丁)
------------------------------

Most conflicts happen because the branch you are backporting to is
missing some patches compared to the branch you are backporting *from*.
In the more general case (such as merging two independent branches),
development could have happened on either branch, or the branches have
simply diverged -- perhaps your older branch had some other backports
applied to it that themselves needed conflict resolutions, causing a
divergence.
(大多数冲突的发生，是因为你向后移植到的分支相对你向后移植 *来自* 的分支缺少一些补丁。在更一般的情况下 (比如合并两个独立的分支)，两个分支上都可能发生过开发，或者分支干脆已经分叉——也许你的旧分支上应用过其他一些本身就需要解决冲突的向后移植，从而造成了分叉。)

It's important to always identify the commit or commits that caused the
conflict, as otherwise you cannot be confident in the correctness of
your resolution. As an added bonus, especially if the patch is in an
area you're not that familiar with, the changelogs of these commits will
often give you the context to understand the code and potential problems
or pitfalls with your conflict resolution.
(始终找出引发冲突的那个或那些 commit 很重要，否则你无法对解决方案的正确性有信心。另外一个好处是，尤其当补丁位于你不那么熟悉的领域时，这些 commit 的变更日志往往会给你提供理解代码的上下文，以及你的冲突解决可能存在的问题或陷阱。)

git log(git log 命令)
~~~~~~~~~~~~~~~~~~~~~

A good first step is to look at ``git log`` for the file that has the
conflict -- this is usually sufficient when there aren't a lot of
patches to the file, but may get confusing if the file is big and
frequently patched. You should run ``git log`` on the range of commits
between your currently checked-out branch (``HEAD``) and the parent of
the patch you are picking (``<commit>``), i.e.::

    git log HEAD..<commit>^ -- <path>


(一个好的第一步是查看发生冲突的文件的 ``git log``——当该文件的补丁不多时这通常就够了，但如果文件很大、补丁频繁，可能会让人眼花缭乱。你应当在当前检出的分支 (``HEAD``) 与你要摘取的补丁的父提交 (``<commit>``) 之间的 commit 范围上运行 ``git log``，即：)

Even better, if you want to restrict this output to a single function
(because that's where the conflict appears), you can use the following
syntax::

    git log -L:'\<function\>':<path> HEAD..<commit>^


(更好的做法是，如果你想把这个输出限制到单个函数 (因为冲突正是出现在那里)，可以使用如下语法：)

.. note::
     The ``\<`` and ``\>`` around the function name ensure that the
     matches are anchored on a word boundary. This is important, as this
     part is actually a regex and git only follows the first match, so
     if you use ``-L:thread_stack:kernel/fork.c`` it may only give you
     results for the function ``try_release_thread_stack_to_cache`` even
     though there are many other functions in that file containing the
     string ``thread_stack`` in their names.
     (注意：函数名两侧的 ``\<`` 和 ``\>`` 确保匹配锚定在单词边界上。这一点很重要，因为这一部分实际上是一个正则表达式，而 git 只跟随第一个匹配项，所以如果你使用 ``-L:thread_stack:kernel/fork.c``，它可能只会给出函数 ``try_release_thread_stack_to_cache`` 的结果，尽管该文件中还有许多其他函数的名字里包含字符串 ``thread_stack``。)

Another useful option for ``git log`` is ``-G``, which allows you to
filter on certain strings appearing in the diffs of the commits you are
listing::

    git log -G'regex' HEAD..<commit>^ -- <path>


(``git log`` 的另一个有用选项是 ``-G``，它允许你根据出现在所列 commit 的 diff 中的特定字符串进行过滤：)

This can also be a handy way to quickly find when something (e.g. a
function call or a variable) was changed, added, or removed. The search
string is a regular expression, which means you can potentially search
for more specific things like assignments to a specific struct member::

    git log -G'\->index\>.*='


(这也是快速找出某个东西 (比如某个函数调用或变量) 何时被修改、添加或删除的便捷方法。搜索字符串是一个正则表达式，这意味着你有可能搜索更具体的东西，比如对特定结构体成员的赋值：)

git blame(git blame 命令)
~~~~~~~~~~~~~~~~~~~~~~~~~

Another way to find prerequisite commits (albeit only the most recent
one for a given conflict) is to run ``git blame``. In this case, you
need to run it against the parent commit of the patch you are
cherry-picking and the file where the conflict appeared, i.e.::

    git blame <commit>^ -- <path>


(查找前置 commit 的另一种方法 (尽管对给定冲突只能找到最近的一个) 是运行 ``git blame``。在这种情况下，你需要针对你要摘取的补丁的父提交以及出现冲突的文件来运行它，即：)

This command also accepts the ``-L`` argument (for restricting the
output to a single function), but in this case you specify the filename
at the end of the command as usual::

    git blame -L:'\<function\>' <commit>^ -- <path>


(该命令同样接受 ``-L`` 参数 (用于把输出限制到单个函数)，但这种情况要像往常一样在命令末尾指定文件名：)

Navigate to the place where the conflict occurred. The first column of
the blame output is the commit ID of the patch that added a given line
of code.
(定位到冲突发生的地方。blame 输出的第一列是添加给定代码行的补丁的 commit ID。)

It might be a good idea to ``git show`` these commits and see if they
look like they might be the source of the conflict. Sometimes there will
be more than one of these commits, either because multiple commits
changed different lines of the same conflict area *or* because multiple
subsequent patches changed the same line (or lines) multiple times. In
the latter case, you may have to run ``git blame`` again and specify the
older version of the file to look at in order to dig further back in
the history of the file.
(用 ``git show`` 查看这些 commit、看看它们是否可能是冲突的来源，或许是个好主意。有时这样的 commit 不止一个，原因可能是多个 commit 修改了同一冲突区域的不同行，*或者* 多个后续补丁多次修改了同一行 (或几行)。在后一种情况下，你可能需要再次运行 ``git blame`` 并指定要查看的更旧版本的文件，以便向文件历史的更深处挖掘。)

Prerequisite vs. incidental patches(前置补丁与偶然补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Having found the patch that caused the conflict, you need to determine
whether it is a prerequisite for the patch you are backporting or
whether it is just incidental and can be skipped. An incidental patch
would be one that touches the same code as the patch you are
backporting, but does not change the semantics of the code in any
material way. For example, a whitespace cleanup patch is completely
incidental -- likewise, a patch that simply renames a function or a
variable would be incidental as well. On the other hand, if the function
being changed does not even exist in your current branch then this would
not be incidental at all and you need to carefully consider whether the
patch adding the function should be cherry-picked first.
(找到引发冲突的补丁之后，你需要判断它是你正在向后移植的补丁的前置条件，还是仅仅是偶然性的、可以跳过的。偶然补丁指的是与你正在向后移植的补丁触及相同代码、但没有以任何实质性方式改变代码语义的补丁。例如，空白清理补丁完全是偶然性的——同样，仅仅重命名某个函数或变量的补丁也是偶然性的。反过来，如果被修改的函数在你当前分支上根本不存在，那就完全不是偶然性的了，你需要仔细考虑是否应当先摘取添加该函数的补丁。)

If you find that there is a necessary prerequisite patch, then you need
to stop and cherry-pick that instead. If you've already resolved some
conflicts in a different file and don't want to do it again, you can
create a temporary copy of that file.
(如果你发现有必要的补丁，那么你需要停下来，转而先摘取它。如果你已经在另一个文件中解决了一些冲突、不想再来一遍，可以为该文件创建一个临时副本。)

To abort the current cherry-pick, go ahead and run
``git cherry-pick --abort``, then restart the cherry-picking process
with the commit ID of the prerequisite patch instead.
(要中止当前的摘取，执行 ``git cherry-pick --abort``，然后用前置补丁的 commit ID 重新开始摘取过程。)

Understanding conflict markers(理解冲突标记)
--------------------------------------------

Combined diffs(组合差异)
~~~~~~~~~~~~~~~~~~~~~~~~

Let's say you've decided against picking (or reverting) additional
patches and you just want to resolve the conflict. Git will have
inserted conflict markers into your file. Out of the box, this will look
something like::

    <<<<<<< HEAD
    this is what's in your current tree before cherry-picking
    =======
    this is what the patch wants it to be after cherry-picking
    >>>>>>> <commit>... title


(假设你决定不摘取 (或回退) 额外的补丁，只想直接解决冲突。Git 会在你的文件中插入冲突标记。默认情况下，它看起来大致像这样：)

This is what you would see if you opened the file in your editor.
However, if you were to run ``git diff`` without any arguments, the
output would look something like this::

    $ git diff
    [...]
    ++<<<<<<<< HEAD
     +this is what's in your current tree before cherry-picking
    ++========
    + this is what the patch wants it to be after cherry-picking
    ++>>>>>>>> <commit>... title


(这就是你在编辑器中打开文件时会看到的内容。不过，如果你不带任何参数运行 ``git diff``，输出会大致像这样：)

When you are resolving a conflict, the behavior of ``git diff`` differs
from its normal behavior. Notice the two columns of diff markers
instead of the usual one; this is a so-called "`combined diff`_", here
showing the 3-way diff (or diff-of-diffs) between
(在解决冲突时，``git diff`` 的行为与平常不同。注意这里有两列 diff 标记，而不是平常的一列；这就是所谓的 "`combined diff`_"，这里显示的是以下两者之间的 3 路差异 (即差异的差异)：)

#. the current branch (before cherry-picking) and the current working
   directory, and
#. the current branch (before cherry-picking) and the file as it looks
   after the original patch has been applied.
(#. 当前分支 (摘取之前) 与当前工作目录，以及
#. 当前分支 (摘取之前) 与应用原始补丁之后的文件。)

.. _combined diff: https://git-scm.com/docs/diff-format#_combined_diff_format


Better diffs(更好的差异输出)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

3-way combined diffs include all the other changes that happened to the
file between your current branch and the branch you are cherry-picking
from. While this is useful for spotting other changes that you need to
take into account, this also makes the output of ``git diff`` somewhat
intimidating and difficult to read. You may instead prefer to run
``git diff HEAD`` (or ``git diff --ours``) which shows only the diff
between the current branch before cherry-picking and the current working
directory. It looks like this::

    $ git diff HEAD
    [...]
    +<<<<<<<< HEAD
     this is what's in your current tree before cherry-picking
    +========
    +this is what the patch wants it to be after cherry-picking
    +>>>>>>>> <commit>... title


(3 路组合差异会包含文件在你的当前分支与你摘取补丁的分支之间发生的所有其他修改。虽然这有助于发现你需要考虑的其他修改，但这也让 ``git diff`` 的输出有些吓人、难以阅读。你可能更愿意改用 ``git diff HEAD`` (或 ``git diff --ours``)，它只显示摘取之前的当前分支与当前工作目录之间的差异。它看起来像这样：)

As you can see, this reads just like any other diff and makes it clear
which lines are in the current branch and which lines are being added
because they are part of the merge conflict or the patch being
cherry-picked.
(可以看到，这个输出读起来就像任何其他 diff 一样，并且能清楚地区分哪些行属于当前分支、哪些行是因为属于合并冲突或正在摘取的补丁而新增的。)

Merge styles and diff3(合并风格与 diff3)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The default conflict marker style shown above is known as the ``merge``
style. There is also another style available, known as the ``diff3``
style, which looks like this::

    <<<<<<< HEAD
    this is what is in your current tree before cherry-picking
    ||||||| parent of <commit> (title)
    this is what the patch expected to find there
    =======
    this is what the patch wants it to be after being applied
    >>>>>>> <commit> (title)


(上面展示的默认冲突标记风格称为 ``merge`` 风格。还有另一种可用的风格，称为 ``diff3`` 风格，它看起来像这样：)

As you can see, this has 3 parts instead of 2, and includes what git
expected to find there but didn't. It is *highly recommended* to use
this conflict style as it makes it much clearer what the patch actually
changed; i.e., it allows you to compare the before-and-after versions
of the file for the commit you are cherry-picking. This allows you to
make better decisions about how to resolve the conflict.
(可以看到，这里有 3 个部分而不是 2 个，并且包含了 git 预期在那里找到、却没有找到的内容。*强烈建议* 使用这种冲突风格，因为它能更清楚地表明补丁实际上改了什么；也就是说，它让你能够比较你正在摘取的 commit 对应的文件的前后两个版本。这能帮助你更好地决定如何解决冲突。)

To change conflict marker styles, you can use the following command::

    git config merge.conflictStyle diff3


(要更改冲突标记风格，可以使用以下命令：)

There is a third option, ``zdiff3``, introduced in `Git 2.35`_,
which has the same 3 sections as ``diff3``, but where common lines have
been trimmed off, making the conflict area smaller in some cases.
(还有第三个选项 ``zdiff3``，由 `Git 2.35`_ 引入，它与 ``diff3`` 有同样的 3 个部分，但会裁剪掉共同的行，在某些情况下能让冲突区域更小。)

.. _Git 2.35: https://github.blog/2022-01-24-highlights-from-git-2-35/

Iterating on conflict resolutions(迭代解决冲突)
-----------------------------------------------

The first step in any conflict resolution process is to understand the
patch you are backporting. For the Linux kernel this is especially
important, since an incorrect change can lead to the whole system
crashing -- or worse, an undetected security vulnerability.
(任何冲突解决流程的第一步都是理解你正在向后移植的补丁。对 Linux 内核来说这一点尤其重要，因为一个错误的修改可能导致整个系统崩溃——或者更糟，一个未被察觉的安全漏洞。)

Understanding the patch can be easy or difficult depending on the patch
itself, the changelog, and your familiarity with the code being changed.
However, a good question for every change (or every hunk of the patch)
might be: "Why is this hunk in the patch?" The answers to these
questions will inform your conflict resolution.
(理解补丁的难易取决于补丁本身、变更日志以及你对被修改代码的熟悉程度。不过，对每一处修改 (或补丁的每一个 hunk) 都可以问一个好问题："这个 hunk 为什么会出现在补丁里？" 这些问题的答案将指导你的冲突解决。)

Resolution process(解决流程)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sometimes the easiest thing to do is to just remove all but the first
part of the conflict, leaving the file essentially unchanged, and apply
the changes by hand. Perhaps the patch is changing a function call
argument from ``0`` to ``1`` while a conflicting change added an
entirely new (and insignificant) parameter to the end of the parameter
list; in that case, it's easy enough to change the argument from ``0``
to ``1`` by hand and leave the rest of the arguments alone. This
technique of manually applying changes is mostly useful if the conflict
pulled in a lot of unrelated context that you don't really need to care
about.
(有时最简单的做法是把冲突中除第一部分之外的内容全部删掉，让文件基本保持原样，然后手工应用修改。也许补丁只是把某个函数调用参数从 ``0`` 改为 ``1``，而冲突的另一方在参数列表末尾加了一个全新的 (且无关紧要的) 参数；这种情况下，手工把参数从 ``0`` 改成 ``1``、其余参数保持不动就足够了。这种手工应用修改的技巧，主要在冲突带入了大量你其实无需关心的无关上下文时有用。)

For particularly nasty conflicts with many conflict markers, you can use
``git add`` or ``git add -i`` to selectively stage your resolutions to
get them out of the way; this also lets you use ``git diff HEAD`` to
always see what remains to be resolved or ``git diff --cached`` to see
what your patch looks like so far.
(对于带有很多冲突标记、特别棘手的冲突，你可以用 ``git add`` 或 ``git add -i`` 有选择地把已解决的暂存起来，先把它们放到一边；这也让你可以用 ``git diff HEAD`` 随时查看还有哪些待解决，或用 ``git diff --cached`` 查看目前补丁的样子。)

Dealing with file renames(处理文件重命名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

One of the most annoying things that can happen while backporting a
patch is discovering that one of the files being patched has been
renamed, as that typically means git won't even put in conflict markers,
but will just throw up its hands and say (paraphrased): "Unmerged path!
You do the work..."
(向后移植补丁时可能发生的最恼人的事情之一，就是发现被补丁修改的文件之一已经被重命名，因为这通常意味着 git 连冲突标记都不会加，只会摊手表示 (大意)："未合并路径！你自己干吧……")

There are generally a few ways to deal with this. If the patch to the
renamed file is small, like a one-line change, the easiest thing is to
just go ahead and apply the change by hand and be done with it. On the
other hand, if the change is big or complicated, you definitely don't
want to do it by hand.
(处理这种情况一般有几种办法。如果对重命名文件的补丁很小，比如一行修改，最简单的就是直接手工应用修改了事。反之，如果修改很大或很复杂，你肯定不想手工去做。)

As a first pass, you can try something like this, which will lower the
rename detection threshold to 30% (by default, git uses 50%, meaning
that two files need to have at least 50% in common for it to consider
an add-delete pair to be a potential rename)::

  git cherry-pick -strategy=recursive -Xrename-threshold=30


(作为第一步，你可以尝试类似下面这样的命令，它会把重命名检测阈值降到 30% (默认情况下 git 使用 50%，即两个文件至少要有 50% 相同，才会把一对增删视为可能的重命名)：)

Sometimes the right thing to do will be to also backport the patch that
did the rename, but that's definitely not the most common case. Instead,
what you can do is to temporarily rename the file in the branch you're
backporting to (using ``git mv`` and committing the result), restart the
attempt to cherry-pick the patch, rename the file back (``git mv`` and
committing again), and finally squash the result using ``git rebase -i``
(see the `rebase tutorial`_) so it appears as a single commit when you
are done.
(有时正确的做法是把执行重命名的补丁也一并向后移植，但这肯定不是最常见的情况。作为替代，你可以先在向后移植到的分支中临时重命名该文件 (用 ``git mv`` 并提交结果)，重新尝试摘取补丁，再把文件重命名回来 (再次 ``git mv`` 并提交)，最后用 ``git rebase -i`` 把结果压缩 (见 `rebase tutorial`_)，这样完成之后它看起来就是一个单独的 commit。)

.. _rebase tutorial: https://medium.com/@slamflipstrom/a-beginners-guide-to-squashing-commits-with-git-rebase-8185cf6e62ec

Gotchas(易错点)
---------------

Function arguments(函数参数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pay attention to changing function arguments! It's easy to gloss over
details and think that two lines are the same but actually they differ
in some small detail like which variable was passed as an argument
(especially if the two variables are both a single character that look
the same, like i and j).
(注意函数参数的变化！人们很容易忽略细节、以为两行代码相同，实际上它们可能在某个小细节上不同，比如传作参数的是哪个变量 (尤其是当两个变量都是看起来一样的单字符变量时，比如 i 和 j)。)

Error handling(错误处理)
~~~~~~~~~~~~~~~~~~~~~~~~

If you cherry-pick a patch that includes a ``goto`` statement (typically
for error handling), it is absolutely imperative to double check that
the target label is still correct in the branch you are backporting to.
The same goes for added ``return``, ``break``, and ``continue``
statements.
(如果你摘取的补丁包含 ``goto`` 语句 (通常用于错误处理)，那么绝对必须再次确认目标标签在你向后移植到的分支中仍然正确。新增的 ``return``、``break`` 和 ``continue`` 语句同样如此。)

Error handling is typically located at the bottom of the function, so it
may not be part of the conflict even though it could have been changed by
other patches.
(错误处理通常位于函数底部，因此即使它可能已被其他补丁修改，也可能不属于冲突的一部分。)

A good way to ensure that you review the error paths is to always use
``git diff -W`` and ``git show -W`` (AKA ``--function-context``) when
inspecting your changes.  For C code, this will show you the whole
function that's being changed in a patch. One of the things that often
go wrong during backports is that something else in the function changed
on either of the branches that you're backporting from or to. By
including the whole function in the diff you get more context and can
more easily spot problems that might otherwise go unnoticed.
(确保你审查错误路径的一个好办法，是查看修改时始终使用 ``git diff -W`` 和 ``git show -W`` (又称 ``--function-context``)。对 C 代码来说，这会显示补丁所修改的整个函数。向后移植中经常出问题的一件事，就是函数中的其他东西在你向后移植的源分支或目标分支上发生了变化。把整个函数包含进 diff 能给你更多上下文，更容易发现原本可能被忽略的问题。)

Refactored code(重构过的代码)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Something that happens quite often is that code gets refactored by
"factoring out" a common code sequence or pattern into a helper
function. When backporting patches to an area where such a refactoring
has taken place, you effectively need to do the reverse when
backporting: a patch to a single location may need to be applied to
multiple locations in the backported version. (One giveaway for this
scenario is that a function was renamed -- but that's not always the
case.)
(相当常见的一种情况是，代码被重构过，把一段公共的代码序列或模式 "提取" 成了辅助函数。当向后移植补丁到发生过这种重构的区域时，你实际上需要在向后移植时做相反的事情：针对单一位置的补丁，可能需要应用到向后移植后版本中的多个位置。(判断这种场景的一个线索是某个函数被重命名了——但并非总是如此。))

To avoid incomplete backports, it's worth trying to figure out if the
patch fixes a bug that appears in more than one place. One way to do
this would be to use ``git grep``. (This is actually a good idea to do
in general, not just for backports.) If you do find that the same kind
of fix would apply to other places, it's also worth seeing if those
places exist upstream -- if they don't, it's likely the patch may need
to be adjusted. ``git log`` is your friend to figure out what happened
to these areas as ``git blame`` won't show you code that has been
removed.
(为了避免不完整的向后移植，值得试着弄清补丁修复的 bug 是否出现在不止一处。一个办法是使用 ``git grep``。(这其实是一个普遍适用的好习惯，不只是为了向后移植。) 如果你确实发现同样的修复也适用于其他地方，还值得看看这些地方在上游是否存在——如果不存在，补丁很可能需要调整。``git log`` 是弄清这些区域发生了什么的好帮手，因为 ``git blame`` 不会显示已被删除的代码。)

If you do find other instances of the same pattern in the upstream tree
and you're not sure whether it's also a bug, it may be worth asking the
patch author. It's not uncommon to find new bugs during backporting!
(如果你在上游树中确实发现了相同模式的其他实例，又不确定它是否也是 bug，也许值得问问补丁作者。在向后移植过程中发现新 bug 并不罕见！)

Verifying the result(验证结果)
==============================

colordiff(colordiff 工具)
-------------------------

Having committed a conflict-free new patch, you can now compare your
patch to the original patch. It is highly recommended that you use a
tool such as `colordiff`_ that can show two files side by side and color
them according to the changes between them::

    colordiff -yw -W 200 <(git diff -W <upstream commit>^-) <(git diff -W HEAD^-) | less -SR


(提交了无冲突的新补丁之后，现在可以把你的补丁与原始补丁进行比较。强烈建议使用 `colordiff`_ 这样的工具，它能并排显示两个文件，并根据它们之间的变化着色：)

.. _colordiff: https://www.colordiff.org/

Here, ``-y`` means to do a side-by-side comparison; ``-w`` ignores
whitespace, and ``-W 200`` sets the width of the output (as otherwise it
will use 130 by default, which is often a bit too little).
(这里，``-y`` 表示并排比较；``-w`` 忽略空白，``-W 200`` 设置输出的宽度 (否则默认为 130，通常有点太窄)。)

The ``rev^-`` syntax is a handy shorthand for ``rev^..rev``, essentially
giving you just the diff for that single commit; also see
the official `git rev-parse documentation`_.
(``rev^-`` 语法是 ``rev^..rev`` 的便捷简写，实质上只给出那一个 commit 的 diff；另见官方的 `git rev-parse documentation`_。)

.. _git rev-parse documentation: https://git-scm.com/docs/git-rev-parse#_other_rev_parent_shorthand_notations

Again, note the inclusion of ``-W`` for ``git diff``; this ensures that
you will see the full function for any function that has changed.
(再次注意 ``git diff`` 带上了 ``-W``；这确保你能看到任何被修改函数的完整函数体。)

One incredibly important thing that colordiff does is to highlight lines
that are different. For example, if an error-handling ``goto`` has
changed labels between the original and backported patch, colordiff will
show these side-by-side but highlighted in a different color.  Thus, it
is easy to see that the two ``goto`` statements are jumping to different
labels. Likewise, lines that were not modified by either patch but
differ in the context will also be highlighted and thus stand out during
a manual inspection.
(colordiff 做的一件极其重要的事是高亮不同的行。例如，如果错误处理的 ``goto`` 在原始补丁与向后移植补丁之间改了标签，colordiff 会把它们并排显示并以不同颜色高亮。这样，很容易看出两个 ``goto`` 语句跳转到的是不同的标签。同样，未被任何一个补丁修改但在上下文中不同的行也会被高亮，从而在人工检查时凸显出来。)

Of course, this is just a visual inspection; the real test is building
and running the patched kernel (or program).
(当然，这只是目视检查；真正的检验是构建并运行打过补丁的内核 (或程序)。)

Build testing(构建测试)
-----------------------

We won't cover runtime testing here, but it can be a good idea to build
just the files touched by the patch as a quick sanity check. For the
Linux kernel you can build single files like this, assuming you have the
``.config`` and build environment set up correctly::

    make path/to/file.o


(这里不讨论运行时测试，但只构建补丁触及的文件作为一种快速健全性检查是个好主意。对 Linux 内核来说，假设你已正确设置 ``.config`` 和构建环境，可以像这样构建单个文件：)

Note that this won't discover linker errors, so you should still do a
full build after verifying that the single file compiles. By compiling
the single file first you can avoid having to wait for a full build *in
case* there are compiler errors in any of the files you've changed.
(注意这不会发现链接器错误，所以在确认单个文件能编译之后仍应做一次完整构建。先编译单个文件，可以避免 *万一* 你修改的文件中有编译错误时等待完整构建。)

Runtime testing(运行时测试)
---------------------------

Even a successful build or boot test is not necessarily enough to rule
out a missing dependency somewhere. Even though the chances are small,
there could be code changes where two independent changes to the same
file result in no conflicts, no compile-time errors, and runtime errors
only in exceptional cases.
(即使构建或启动测试成功，也不一定足以排除某处缺失的依赖。尽管概率很小，但可能存在这样的代码修改：对同一文件的两个独立修改既不产生冲突、也不产生编译期错误，只在例外情况下产生运行时错误。)

One concrete example of this was a pair of patches to the system call
entry code where the first patch saved/restored a register and a later
patch made use of the same register somewhere in the middle of this
sequence. Since there was no overlap between the changes, one could
cherry-pick the second patch, have no conflicts, and believe that
everything was fine, when in fact the code was now scribbling over an
unsaved register.
(这方面的一个具体例子是对系统调用入口代码的一对补丁：第一个补丁保存/恢复了一个寄存器，而后一个补丁在这个序列中间的某处使用了同一寄存器。由于两个修改之间没有重叠，摘取第二个补丁不会产生任何冲突，让人以为一切正常，而实际上代码此时正在践踏一个未被保存的寄存器。)

Although the vast majority of errors will be caught during compilation
or by superficially exercising the code, the only way to *really* verify
a backport is to review the final patch with the same level of scrutiny
as you would (or should) give to any other patch. Having unit tests and
regression tests or other types of automatic testing can help increase
the confidence in the correctness of a backport.
(尽管绝大多数错误会在编译期间或粗略运行代码时被发现，但 *真正* 验证一个向后移植的唯一方法，是以与你对待 (或应当对待) 任何其他补丁相同级别的严格程度来评审最终补丁。拥有单元测试、回归测试或其他类型的自动化测试有助于提高对向后移植正确性的信心。)

Submitting backports to stable(向 stable 提交向后移植补丁)
==========================================================

As the stable maintainers try to cherry-pick mainline fixes onto their
stable kernels, they may send out emails asking for backports when
encountering conflicts, see e.g.
<https://lore.kernel.org/stable/2023101528-jawed-shelving-071a@gregkh/>.
These emails typically include the exact steps you need to cherry-pick
the patch to the correct tree and submit the patch.
(stable 维护者在尝试把主线修复摘取到他们的稳定内核上时，遇到冲突可能会发邮件征求向后移植，例如 <https://lore.kernel.org/stable/2023101528-jawed-shelving-071a@gregkh/>。这些邮件通常会包含把补丁摘取到正确之树并提交补丁所需的确切步骤。)

One thing to make sure is that your changelog conforms to the expected
format::

  <original patch title>

  [ Upstream commit <mainline rev> ]

  <rest of the original changelog>
  [ <summary of the conflicts and their resolutions> ]
  Signed-off-by: <your name and email>


(需要确保的一点是，你的变更日志符合预期格式：)

The "Upstream commit" line is sometimes slightly different depending on
the stable version. Older version used this format::

  commit <mainline rev> upstream.


("Upstream commit" 行有时会因稳定版本不同而略有差异。较旧的版本使用如下格式：)

It is most common to indicate the kernel version the patch applies to
in the email subject line (using e.g.
``git send-email --subject-prefix='PATCH 6.1.y'``), but you can also put
it in the Signed-off-by:-area or below the ``---`` line.
(最常见的做法是在邮件主题行中标明补丁适用的内核版本 (例如使用 ``git send-email --subject-prefix='PATCH 6.1.y'``)，但也可以把它放在 Signed-off-by: 区域或 ``---`` 行之下。)

The stable maintainers expect separate submissions for each active
stable version, and each submission should also be tested separately.
(stable 维护者期望每个仍在维护的稳定版本单独提交，每次提交也应当分别测试。)

A few final words of advice(最后的几点建议)
===========================================

1) Approach the backporting process with humility.
   (以谦逊的态度对待向后移植过程。)

2) Understand the patch you are backporting; this means reading both
   the changelog and the code.
   (理解你正在向后移植的补丁；这意味着既要读变更日志，也要读代码。)

3) Be honest about your confidence in the result when submitting the
   patch.
   (提交补丁时，诚实地说明你对结果的信心。)

4) Ask relevant maintainers for explicit acks.
   (请相关维护者给出明确的 ack。)

Examples(示例)
==============

The above shows roughly the idealized process of backporting a patch.
For a more concrete example, see this video tutorial where two patches
are backported from mainline to stable:
`Backporting Linux Kernel Patches`_.
(以上大致展示了向后移植一个补丁的理想化流程。更具体的例子见这个视频教程，其中演示了把两个补丁从主线向后移植到 stable：`Backporting Linux Kernel Patches`_。)

.. _Backporting Linux Kernel Patches: https://youtu.be/sBR7R1V2FeA
