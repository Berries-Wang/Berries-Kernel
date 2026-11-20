.. _email_clients:

Email clients info for Linux(Linux 邮件客户端信息)
==================================================

Git(Git 的使用)
---------------

These days most developers use ``git send-email`` instead of regular
email clients.  The man page for this is quite good.  On the receiving
end, maintainers use ``git am`` to apply the patches.
(如今大多数开发者都使用 ``git send-email`` 而不是普通邮件客户端。它的
手册页相当完善。在接收端，维护者使用 ``git am`` 应用补丁。)

If you are new to ``git`` then send your first patch to yourself.  Save it
as raw text including all the headers.  Run ``git am raw_email.txt`` and
then review the changelog with ``git log``.  When that works then send
the patch to the appropriate mailing list(s).
(如果你是 ``git`` 新手，先把你的第一个补丁发给自己。以原始文本连同全部
邮件头保存下来。运行 ``git am raw_email.txt``，然后用 ``git log`` 检查变
更日志。可行之后再把它发给相应的邮件列表。)

General Preferences(一般偏好)
-----------------------------

Patches for the Linux kernel are submitted via email, preferably as
inline text in the body of the email.  Some maintainers accept
attachments, but then the attachments should have content-type
``text/plain``.  However, attachments are generally frowned upon because
it makes quoting portions of the patch more difficult in the patch
review process.
(Linux 内核的补丁通过电子邮件提交，最好以正文内联文本的形式。一些维护
者接受附件，但附件的 content-type 应为 ``text/plain``。不过，附件通常不
受欢迎，因为它使补丁评审过程中引用补丁片段变得更困难。)

It's also strongly recommended that you use plain text in your email body,
for patches and other emails alike. https://useplaintext.email may be useful
for information on how to configure your preferred email client, as well as
listing recommended email clients should you not already have a preference.
(同样强烈建议你在邮件正文中使用纯文本，补丁和其他邮件都一样。
https://useplaintext.email 可能有助于了解如何配置你偏好的邮件客户端；
如果你还没有偏好的话，那里也列出了推荐的邮件客户端。)

Email clients that are used for Linux kernel patches should send the
patch text untouched.  For example, they should not modify or delete tabs
or spaces, even at the beginning or end of lines.
(用于 Linux 内核补丁的邮件客户端应当原封不动地发送补丁文本。例如，不应
修改或删除制表符或空格，即使它们位于行首或行尾。)

Don't send patches with ``format=flowed``.  This can cause unexpected
and unwanted line breaks.
(不要发送带 ``format=flowed`` 的补丁。这会导致意外且不合需要的换行。)

Don't let your email client do automatic word wrapping for you.
This can also corrupt your patch.
(不要让邮件客户端替你自动折行。这同样会损坏你的补丁。)

Email clients should not modify the character set encoding of the text.
Emailed patches should be in ASCII or UTF-8 encoding only.
If you configure your email client to send emails with UTF-8 encoding,
you avoid some possible charset problems.
(邮件客户端不应改动文本的字符集编码。以邮件发送的补丁应只使用 ASCII 或
UTF-8 编码。如果你把邮件客户端配置为以 UTF-8 编码发送邮件，就能避免一
些可能的字符集问题。)

Email clients should generate and maintain "References:" or "In-Reply-To:"
headers so that mail threading is not broken.
(邮件客户端应生成并维护 "References:" 或 "In-Reply-To:" 头，以免邮件线
程被破坏。)

Copy-and-paste (or cut-and-paste) usually does not work for patches
because tabs are converted to spaces.  Using xclipboard, xclip, and/or
xcutsel may work, but it's best to test this for yourself or just avoid
copy-and-paste.
(复制粘贴 (或剪切粘贴) 通常不适用于补丁，因为制表符会被转换成空格。使
用 xclipboard、xclip 和/或 xcutsel 或许可行，但最好亲自测试一下，或者干
脆避免复制粘贴。)

Don't use PGP/GPG signatures in mail that contains patches.
This breaks many scripts that read and apply the patches.
(This should be fixable.)
(不要在包含补丁的邮件中使用 PGP/GPG 签名。这会破坏许多读取并应用补丁
的脚本。(这应当是可以修复的。))

It's a good idea to send a patch to yourself, save the received message,
and successfully apply it with 'patch' before sending patches to Linux
mailing lists.
(在把补丁发送到 Linux 邮件列表之前，最好先把补丁发给自己、保存收到的
消息，并成功地用 'patch' 应用它。)


Some email client (MUA) hints(一些邮件客户端提示)
-------------------------------------------------

Here are some specific MUA configuration hints for editing and sending
patches for the Linux kernel.  These are not meant to be complete
software package configuration summaries.
(以下是一些用于编辑和发送 Linux 内核补丁的特定 MUA 配置提示。它们并不
打算成为完整的软件包配置汇总。)


Legend:
(图例：)

- TUI = text-based user interface
  (- TUI = 基于文本的用户界面)
- GUI = graphical user interface
  (- GUI = 图形用户界面)

Alpine (TUI)(基于文本的用户界面)
********************************

Config options:
(配置选项：)

In the :menuselection:`Sending Preferences` section:
(在 :menuselection:`Sending Preferences` 一节中：)

- :menuselection:`Do Not Send Flowed Text` must be ``enabled``
  (- :menuselection:`Do Not Send Flowed Text` 必须为 ``enabled``)
- :menuselection:`Strip Whitespace Before Sending` must be ``disabled``
  (- :menuselection:`Strip Whitespace Before Sending` 必须为 ``disabled``)

When composing the message, the cursor should be placed where the patch
should appear, and then pressing `CTRL-R` let you specify the patch file
to insert into the message.
(撰写消息时，应把光标放在补丁应当出现的位置，然后按 `CTRL-R` 即可指
定要插入消息的补丁文件。)

Claws Mail (GUI)(图形用户界面)
******************************

Works. Some people use this successfully for patches.
(可用。有些人成功地用它处理补丁。)

To insert a patch use :menuselection:`Message-->Insert File` (`CTRL-I`)
or an external editor.
(要插入补丁，使用 :menuselection:`Message-->Insert File` (`CTRL-I`) 或
外部编辑器。)

If the inserted patch has to be edited in the Claws composition window
"Auto wrapping" in
:menuselection:`Configuration-->Preferences-->Compose-->Wrapping` should be
disabled.
(如果插入的补丁需要在 Claws 撰写窗口中编辑，应禁用
:menuselection:`Configuration-->Preferences-->Compose-->Wrapping` 中
的 "Auto wrapping" 功能。)

Evolution (GUI)(图形用户界面)
*****************************

Some people use this successfully for patches.
(有些人成功地用它处理补丁。)

When composing mail select: Preformat
  from :menuselection:`Format-->Paragraph Style-->Preformatted` (`CTRL-7`)
  or the toolbar
(撰写邮件时选择 Preformat，来自
  :menuselection:`Format-->Paragraph Style-->Preformatted` (`CTRL-7`)
  或使用工具栏)

Then use:
:menuselection:`Insert-->Text File...` (`ALT-N x`)
to insert the patch.
(然后使用 :menuselection:`Insert-->Text File...` (`ALT-N x`)
来插入补丁。)

You can also ``diff -Nru old.c new.c | xclip``, select
:menuselection:`Preformat`, then paste with the middle button.
(也可以 ``diff -Nru old.c new.c | xclip``，选择
:menuselection:`Preformat`，然后用中键粘贴。)

Kmail (GUI)(图形用户界面)
*************************

Some people use Kmail successfully for patches.
(有些人成功地用 Kmail 处理补丁。)

The default setting of not composing in HTML is appropriate; do not
enable it.
(默认不以 HTML 撰写的设置是合适的；不要启用 HTML 撰写。)

When composing an email, under options, uncheck "word wrap". The only
disadvantage is any text you type in the email will not be word-wrapped
so you will have to manually word wrap text before the patch. The easiest
way around this is to compose your email with word wrap enabled, then save
it as a draft. Once you pull it up again from your drafts it is now hard
word-wrapped and you can uncheck "word wrap" without losing the existing
wrapping.
(撰写邮件时，在选项中取消勾选 "word wrap"。唯一的缺点是你输入的邮件正
文不会自动折行，所以你必须在补丁之前手动折行文本。最简单的绕过办法是：
先在启用自动折行的状态下撰写邮件，然后保存为草稿；之后再从草稿中打开
时，文本已被硬折行，此时再取消勾选 "word wrap" 也不会丢失已有的折行。)

At the bottom of your email, put the commonly-used patch delimiter before
inserting your patch:  three hyphens (``---``).
(在邮件末尾、插入补丁之前，先放上常用的补丁分隔符：三个连字符
(``---``)。)

Then from the :menuselection:`Message` menu item, select
:menuselection:`insert file` and choose your patch.
As an added bonus you can customise the message creation toolbar menu
and put the :menuselection:`insert file` icon there.
(然后在 :menuselection:`Message` 菜单项中选择 :menuselection:`insert file`
并选择你的补丁。额外的好处是，你可以自定义撰写工具栏菜单，把
:menuselection:`insert file` 图标放上去。)

Make the composer window wide enough so that no lines wrap. As of
KMail 1.13.5 (KDE 4.5.4), KMail will apply word wrapping when sending
the email if the lines wrap in the composer window. Having word wrapping
disabled in the Options menu isn't enough. Thus, if your patch has very
long lines, you must make the composer window very wide before sending
the email. See: https://bugs.kde.org/show_bug.cgi?id=174034
(把撰写窗口调宽到没有任何行发生折行为止。自 KMail 1.13.5 (KDE 4.5.4)
起，如果行在撰写窗口中发生了折行，KMail 在发送邮件时会应用自动折行。
仅在 Options 菜单中禁用自动折行并不够。因此，如果你的补丁有很长的行，
发送邮件前必须把撰写窗口调得非常宽。参见：
https://bugs.kde.org/show_bug.cgi?id=174034 。)

You can safely GPG sign attachments, but inlined text is preferred for
patches so do not GPG sign them.  Signing patches that have been inserted
as inlined text will make them tricky to extract from their 7-bit encoding.
(你可以放心地对附件做 GPG 签名，但补丁首选内联文本，所以不要对补丁做
GPG 签名。对以内联文本方式插入的补丁签名，会使其难以从 7 位编码中提取
出来。)

If you absolutely must send patches as attachments instead of inlining
them as text, right click on the attachment and select :menuselection:`properties`,
and highlight :menuselection:`Suggest automatic display` to make the attachment
inlined to make it more viewable.
(如果你绝对必须以附件而不是内联文本的方式发送补丁，请右键点击附件并
选择 :menuselection:`properties`，再选中
:menuselection:`Suggest automatic display`，使附件内联以便于查看。)

When saving patches that are sent as inlined text, select the email that
contains the patch from the message list pane, right click and select
:menuselection:`save as`.  You can use the whole email unmodified as a patch
if it was properly composed.  Emails are saved as read-write for user only so
you will have to chmod them to make them group and world readable if you copy
them elsewhere.
(保存以内联文本发送的补丁时，在消息列表面板中选中包含补丁的邮件，右
键点击并选择 :menuselection:`save as`。如果邮件撰写得当，你可以把整个
邮件不加修改地用作补丁。邮件保存后默认仅对用户可读写，因此如果你把它
们复制到别处，就需要 chmod 使其对组和其他人可读。)

Lotus Notes (GUI)(图形用户界面)
*******************************

Run away from it.
(离它远点。)

IBM Verse (Web GUI)(网页图形界面)
*********************************

See Lotus Notes.
(参见 Lotus Notes。)

Mutt (TUI)(基于文本的用户界面)
******************************

Plenty of Linux developers use ``mutt``, so it must work pretty well.
(很多 Linux 开发者都在用 ``mutt``，所以它想必工作得相当好。)

Mutt doesn't come with an editor, so whatever editor you use should be
used in a way that there are no automatic linebreaks.  Most editors have
an :menuselection:`insert file` option that inserts the contents of a file
unaltered.
(Mutt 不自带编辑器，因此无论你使用哪个编辑器，都应以不产生自动换行的
方式使用它。大多数编辑器都有 :menuselection:`insert file` 选项，可以原
样插入文件内容。)

To use ``vim`` with mutt::

  set editor="vi"

(要在 mutt 中使用 ``vim``：)

If using xclip, type the command::

  :set paste

(如果使用 xclip，请在按中键或 shift-insert 之前输入以下命令：)

before middle button or shift-insert or use::

  :r filename

(或者使用：)

if you want to include the patch inline.
(a)ttach works fine without ``set paste``.
(以上 :r filename 用于内联包含补丁的情形。(a)ttach 在没有 ``set paste``
的情况下也能正常工作。)

You can also generate patches with ``git format-patch`` and then use Mutt
to send them::

    $ mutt -H 0001-some-bug-fix.patch

(你也可以用 ``git format-patch`` 生成补丁，然后用 Mutt 发送：)

Config options:
(配置选项：)

It should work with default settings.
However, it's a good idea to set the ``send_charset`` to::

  set send_charset="us-ascii:utf-8"

(默认设置应当就能工作。不过，最好把 ``send_charset`` 设置为：)

Mutt is highly customizable. Here is a minimum configuration to start
using Mutt to send patches through Gmail::

  # .muttrc
  # ================  IMAP  ====================
  set imap_user = 'yourusername@gmail.com'
  set imap_pass = 'yourpassword'
  set spoolfile = imaps://imap.gmail.com/INBOX
  set folder = imaps://imap.gmail.com/
  set record="imaps://imap.gmail.com/[Gmail]/Sent Mail"
  set postponed="imaps://imap.gmail.com/[Gmail]/Drafts"
  set mbox="imaps://imap.gmail.com/[Gmail]/All Mail"

  # ================  SMTP  ====================
  set smtp_url = "smtp://username@smtp.gmail.com:587/"
  set smtp_pass = $imap_pass
  set ssl_force_tls = yes # Require encrypted connection

  # ================  Composition  ====================
  set editor = `echo \$EDITOR`
  set edit_headers = yes  # See the headers when editing
  set charset = UTF-8     # value of $LANG; also fallback for send_charset
  # Sender, email address, and sign-off line must match
  unset use_domain        # because joe@localhost is just embarrassing
  set realname = "YOUR NAME"
  set from = "username@gmail.com"
  set use_from = yes

(Mutt 高度可定制。以下是开始用 Mutt 通过 Gmail 发送补丁所需的最小配置：)

The Mutt docs have lots more information:
(Mutt 的文档中有更多更多信息：)

    https://gitlab.com/muttmua/mutt/-/wikis/UseCases/Gmail

    http://www.mutt.org/doc/manual/

Pine (TUI)(基于文本的用户界面)
******************************

Pine has had some whitespace truncation issues in the past, but these
should all be fixed now.
(Pine 过去有一些空白字符截断问题，但现在应该都已修复。)

Use alpine (pine's successor) if you can.
(如果可以，请使用 alpine (Pine 的后继者)。)

Config options:
(配置选项：)

- ``quell-flowed-text`` is needed for recent versions
  (- 新版本需要 ``quell-flowed-text``)
- the ``no-strip-whitespace-before-send`` option is needed
  (- 需要 ``no-strip-whitespace-before-send`` 选项)


Sylpheed (GUI)(图形用户界面)
****************************

- Works well for inlining text (or using attachments).
  (- 内联文本 (或使用附件) 效果良好。)
- Allows use of an external editor.
  (- 允许使用外部编辑器。)
- Is slow on large folders.
  (- 在大文件夹上速度较慢。)
- Won't do TLS SMTP auth over a non-SSL connection.
  (- 不能在非 SSL 连接上进行 TLS SMTP 认证。)
- Has a helpful ruler bar in the compose window.
  (- 撰写窗口中有一个实用的标尺栏。)
- Adding addresses to address book doesn't understand the display name
  properly.
  (- 向地址簿添加地址时不能正确理解显示名。)

Thunderbird (GUI)(图形用户界面)
*******************************

Thunderbird is an Outlook clone that likes to mangle text, but there are ways
to coerce it into behaving.
(Thunderbird 是一个喜欢破坏文本格式的 Outlook 克隆，不过有办法迫使它
规矩起来。)

After doing the modifications, this includes installing the extensions,
you need to restart Thunderbird.
(完成修改 (包括安装扩展) 之后，你需要重启 Thunderbird。)

- Allow use of an external editor:
  (- 允许使用外部编辑器：)

  The easiest thing to do with Thunderbird and patches is to use extensions
  which open your favorite external editor.
  (对 Thunderbird 和补丁而言，最简单的做法是使用能打开你最喜欢的外部编
  辑器的扩展。)

  Here are some example extensions which are capable of doing this.
  (下面是一些能够做到这一点的示例扩展。)

  - "External Editor Revived"

    https://github.com/Frederick888/external-editor-revived

    https://addons.thunderbird.net/en-GB/thunderbird/addon/external-editor-revived/

    It requires installing a "native messaging host".
    Please read the wiki which can be found here:
    https://github.com/Frederick888/external-editor-revived/wiki
    (它需要安装一个 "native messaging host"。请阅读位于
    https://github.com/Frederick888/external-editor-revived/wiki 的
    wiki。)

  - "External Editor"

    https://github.com/exteditor/exteditor

    To do this, download and install the extension, then open the
    :menuselection:`compose` window, add a button for it using
    :menuselection:`View-->Toolbars-->Customize...`
    then just click on the new button when you wish to use the external editor.
    (为此，先下载并安装该扩展，然后打开 :menuselection:`compose` 窗口，
    通过 :menuselection:`View-->Toolbars-->Customize...` 为它添加一个按
    钮；之后只要想用外部编辑器，点击这个新按钮即可。)

    Please note that "External Editor" requires that your editor must not
    fork, or in other words, the editor must not return before closing.
    You may have to pass additional flags or change the settings of your
    editor. Most notably if you are using gvim then you must pass the -f
    option to gvim by putting ``/usr/bin/gvim --nofork"`` (if the binary is in
    ``/usr/bin``) to the text editor field in :menuselection:`external editor`
    settings. If you are using some other editor then please read its manual
    to find out how to do this.
    (请注意，"External Editor" 要求你的编辑器不得 fork，换句话说，编辑
    器在关闭之前不得返回。你可能需要传递额外的旗标或修改编辑器的设置。
    最值得注意的是，如果你使用 gvim，则必须向 gvim 传入 -f 选项，即在
    :menuselection:`external editor` 设置中的文本编辑器一栏填入
    ``/usr/bin/gvim --nofork"`` (若二进制文件位于 ``/usr/bin`` )。如果你
    使用的是其他编辑器，请阅读其手册了解具体做法。)

To beat some sense out of the internal editor, do this:
(要让内置编辑器稍微讲点道理，请这样做：)

- Edit your Thunderbird config settings so that it won't use ``format=flowed``!
  Go to your main window and find the button for your main dropdown menu.
  :menuselection:`Main Menu-->Preferences-->General-->Config Editor...`
  to bring up the thunderbird's registry editor.
  (编辑 Thunderbird 的配置设置，使其不使用 ``format=flowed`` ！进入主
  窗口，找到主下拉菜单的按钮，进入
  :menuselection:`Main Menu-->Preferences-->General-->Config Editor...`
  以打开 Thunderbird 的注册表编辑器。)

  - Set ``mailnews.send_plaintext_flowed`` to ``false``
    (把 ``mailnews.send_plaintext_flowed`` 设为 ``false``)

  - Set ``mailnews.wraplength`` from ``72`` to ``0`` **or** install the
    "Toggle Line Wrap" extension

    https://github.com/jan-kiszka/togglelinewrap

    https://addons.thunderbird.net/thunderbird/addon/toggle-line-wrap

    to control this registry on the fly.
    (把 ``mailnews.wraplength`` 从 ``72`` 改为 ``0``，或者安装 "Toggle
    Line Wrap" 扩展 (位于
    https://github.com/jan-kiszka/togglelinewrap 和
    https://addons.thunderbird.net/thunderbird/addon/toggle-line-wrap )
    以便动态控制这一注册表项。)

- Don't write HTML messages! Go to the main window
  :menuselection:`Main Menu-->Account Settings-->youracc@server.something-->Composition & Addressing`!
  There you can disable the option "Compose messages in HTML format".
  (不要写 HTML 邮件！进入主窗口
  :menuselection:`Main Menu-->Account Settings-->youracc@server.something-->Composition & Addressing` ！
  在那里可以禁用 "Compose messages in HTML format" 选项。)

- Open messages only as plain text! Go to the main window
  :menuselection:`Main Menu-->View-->Message Body As-->Plain Text`!
  (只以纯文本方式打开邮件！进入主窗口
  :menuselection:`Main Menu-->View-->Message Body As-->Plain Text` ！)

TkRat (GUI)(图形用户界面)
*************************

Works.  Use "Insert file..." or external editor.
(可用。使用 "Insert file..." 或外部编辑器。)

Gmail (Web GUI)(网页图形界面)
*****************************

Does not work for sending patches.
(不能用于发送补丁。)

Gmail web client converts tabs to spaces automatically.
(Gmail 网页客户端会自动把制表符转换为空格。)

At the same time it wraps lines every 78 chars with CRLF style line breaks
although tab2space problem can be solved with external editor.
(同时它每 78 个字符就折行一次，并使用 CRLF 风格的换行符，尽管 tab2space
问题可以通过外部编辑器解决。)

Another problem is that Gmail will base64-encode any message that has a
non-ASCII character. That includes things like European names.
(另一个问题是，Gmail 会对任何含有非 ASCII 字符的邮件进行 base64 编码。
这包括诸如欧洲人名之类的内容。)

HacKerMaiL (TUI)(基于文本的用户界面)
************************************

HacKerMaiL (hkml) is a public-inbox based simple mails management tool that
doesn't require subscription of mailing lists.  It is developed and maintained
by the DAMON maintainer and aims to support simple development workflows for
DAMON and general kernel subsystems.  Refer to the README
(https://github.com/sjp38/hackermail/blob/master/README.md) for details.
(HacKerMaiL (hkml) 是一个基于 public-inbox 的简单邮件管理工具，不需要
订阅邮件列表。它由 DAMON 维护者开发并维护，旨在为 DAMON 和一般内核子系
统提供简单的开发工作流支持。详情参见 README
(https://github.com/sjp38/hackermail/blob/master/README.md)。)
