.. _codingstyle:

Linux kernel coding style(Linux 内核编码风格)
=============================================

This is a short document describing the preferred coding style for the
linux kernel.  Coding style is very personal, and I won't **force** my
views on anybody, but this is what goes for anything that I have to be
able to maintain, and I'd prefer it for most other things too.  Please
at least consider the points made here.
(这是一份简短的文档，描述 Linux 内核首选的编码风格。编码风格是非常个人化的东西，我不会把自己的观点强加给任何人，但凡是需要由我维护的代码，都会遵循这里所说的规则，而且我希望在大多数其他场合也是如此。请至少考虑一下这里提出的要点。)

First off, I'd suggest printing out a copy of the GNU coding standards,
and NOT read it.  Burn them, it's a great symbolic gesture.
(首先，我建议你打印一份 GNU 编码标准出来——然后不要去读它。把它烧掉，这是一个很有象征意义的举动。)

Anyway, here goes:
(闲话少说，开始吧：)


1) Indentation(缩进)
--------------------

Tabs are 8 characters, and thus indentations are also 8 characters.
There are heretic movements that try to make indentations 4 (or even 2!)
characters deep, and that is akin to trying to define the value of PI to
be 3.
(制表符是 8 个字符宽，因此缩进也是 8 个字符。有些异端运动试图把缩进改成 4 甚至 2 个字符深，这就好比试图把 PI 的值定义成 3 一样。)

Rationale: The whole idea behind indentation is to clearly define where
a block of control starts and ends.  Especially when you've been looking
at your screen for 20 straight hours, you'll find it a lot easier to see
how the indentation works if you have large indentations.
(理由：缩进的全部意义在于清楚地界定一个控制块的开始和结束。尤其是当你已经连续盯着屏幕看了 20 个小时之后，你会发现只要缩进够大，就更容易看清缩进是如何起作用的。)

Now, some people will claim that having 8-character indentations makes
the code move too far to the right, and makes it hard to read on a
80-character terminal screen.  The answer to that is that if you need
more than 3 levels of indentation, you're screwed anyway, and should fix
your program.
(现在，有些人会声称 8 字符缩进会让代码向右移得太远，在 80 列的终端屏幕上难以阅读。对此的回答是：如果你需要超过 3 层的缩进，不管怎样你都已经被搞砸了，应该去修你的程序。)

In short, 8-char indents make things easier to read, and have the added
benefit of warning you when you're nesting your functions too deep.
Heed that warning.
(简言之，8 字符缩进让代码更易读，还附带着一个好处：当你把函数嵌套得太深时，它会向你发出警告。要听从这个警告。)

The preferred way to ease multiple indentation levels in a switch statement is
to align the ``switch`` and its subordinate ``case`` labels in the same column
instead of ``double-indenting`` the ``case`` labels.  E.g.:
(在 switch 语句中，缓解多层缩进的首选方式，是把 ``switch`` 及其下属的 ``case`` 标签对齐在同一列上，而不是对 ``case`` 标签进行二次缩进 (double-indenting)。例如：)

.. code-block:: c

	switch (suffix) {
	case 'G':
	case 'g':
		mem <<= 30;
		break;
	case 'M':
	case 'm':
		mem <<= 20;
		break;
	case 'K':
	case 'k':
		mem <<= 10;
		fallthrough;
	default:
		break;
	}

Don't put multiple statements on a single line unless you have
something to hide:
(不要把多条语句放在同一行上，除非你有见不得人的东西：)

.. code-block:: c

	if (condition) do_this;
	  do_something_everytime;

Don't use commas to avoid using braces:
(不要用逗号来逃避使用花括号：)

.. code-block:: c

	if (condition)
		do_this(), do_that();

Always use braces for multiple statements:
(多条语句时一律使用花括号：)

.. code-block:: c

	if (condition) {
		do_this();
		do_that();
	}

Don't put multiple assignments on a single line either.  Kernel coding style
is super simple.  Avoid tricky expressions.
(也不要把多个赋值语句放在同一行上。内核编码风格极其简单，要避免玩弄花哨的表达式。)


Outside of comments, documentation and except in Kconfig, spaces are never
used for indentation, and the above example is deliberately broken.
(在注释和文档之外，除 Kconfig 外，绝不用空格做缩进；上面那个例子是故意写错的。)

Get a decent editor and don't leave whitespace at the end of lines.
(用一个像样的编辑器，不要在行尾留下空白字符。)


2) Breaking long lines and strings(长行与长字符串的折行)
--------------------------------------------------------

Coding style is all about readability and maintainability using commonly
available tools.
(编码风格的核心，就是使用通用的工具来保证可读性和可维护性。)

The preferred limit on the length of a single line is 80 columns.
(单行长度首选的上限是 80 列。)

Statements longer than 80 columns should be broken into sensible chunks,
unless exceeding 80 columns significantly increases readability and does
not hide information.
(超过 80 列的语句应当拆分成合理的片段，除非超过 80 列能显著提升可读性并且不隐藏信息。)

Descendants are always substantially shorter than the parent and
are placed substantially to the right.  A very commonly used style
is to align descendants to a function open parenthesis.
(折行后的子行总是比父行短得多，并且向右挪动了相当的距离。一个非常常用的做法，是把子行对齐到函数的左圆括号处。)

These same rules are applied to function headers with a long argument list.
(对于带长参数列表的函数头，也同样适用这些规则。)

However, never break user-visible strings such as printk messages because
that breaks the ability to grep for them.
(不过，绝不要折行 printk 消息这类用户可见的字符串，因为那样会破坏 grep 查找它们的能力。)


3) Placing Braces and Spaces(括号与空格的摆放)
----------------------------------------------

The other issue that always comes up in C styling is the placement of
braces.  Unlike the indent size, there are few technical reasons to
choose one placement strategy over the other, but the preferred way, as
shown to us by the prophets Kernighan and Ritchie, is to put the opening
brace last on the line, and put the closing brace first, thusly:
(C 风格里另一个总是被提起的问题，是花括号的摆放。与缩进大小不同，选择哪种摆放策略并没有多少技术上的理由；但正如先知 Kernighan 和 Ritchie 向我们展示的那样，首选的方式是把开括号放在行尾，而把闭括号放在行首，就像这样：)

.. code-block:: c

	if (x is true) {
		we do y
	}

This applies to all non-function statement blocks (if, switch, for,
while, do).  E.g.:
(这适用于所有非函数语句块 (if、switch、for、while、do)。例如：)

.. code-block:: c

	switch (action) {
	case KOBJ_ADD:
		return "add";
	case KOBJ_REMOVE:
		return "remove";
	case KOBJ_CHANGE:
		return "change";
	default:
		return NULL;
	}

However, there is one special case, namely functions: they have the
opening brace at the beginning of the next line, thus:
(不过，有一个特例，即函数：函数的开括号放在下一行的行首，就像这样：)

.. code-block:: c

	int function(int x)
	{
		body of function
	}

Heretic people all over the world have claimed that this inconsistency
is ...  well ...  inconsistent, but all right-thinking people know that
(a) K&R are **right** and (b) K&R are right.  Besides, functions are
special anyway (you can't nest them in C).
(全世界的异端都声称这种不一致……嗯……很不一致，但所有思想正确的人都知道：(a) K&R 是对的，(b) K&R 是对的。再说，函数本来就很特殊 (在 C 里你无法嵌套它们)。)

Note that the closing brace is empty on a line of its own, **except** in
the cases where it is followed by a continuation of the same statement,
ie a ``while`` in a do-statement or an ``else`` in an if-statement, like
this:
(注意，闭括号独占一行且为空，除非它后面跟的是同一语句的延续，比如 do 语句里的 ``while``，或者 if 语句里的 ``else``，就像这样：)

.. code-block:: c

	do {
		body of do-loop
	} while (condition);

and

.. code-block:: c

	if (x == y) {
		..
	} else if (x > y) {
		...
	} else {
		....
	}

Rationale: K&R.
(理由：K&R。)

Also, note that this brace-placement also minimizes the number of empty
(or almost empty) lines, without any loss of readability.  Thus, as the
supply of new-lines on your screen is not a renewable resource (think
25-line terminal screens here), you have more empty lines to put
comments on.
(另外要注意，这种花括号摆放方式还能把空行 (或近乎空行) 的数量减到最少，而不损失任何可读性。这样一来，既然屏幕上空行供应不是可再生资源 (想想 25 行的终端屏幕)，你就有了更多可以用来放注释的空行。)

Do not unnecessarily use braces where a single statement will do.
(如果单个语句就够用，就不要不必要地使用花括号。)

.. code-block:: c

	if (condition)
		action();

and

.. code-block:: c

	if (condition)
		do_this();
	else
		do_that();

This does not apply if only one branch of a conditional statement is a single
statement; in the latter case use braces in both branches:
(如果条件语句只有一个分支是单条语句，这条规则就不适用了；在后一种情况下，两个分支都要使用花括号：)

.. code-block:: c

	if (condition) {
		do_this();
		do_that();
	} else {
		otherwise();
	}

Also, use braces when a loop contains more than a single simple statement:
(同样，当循环体包含不止一条简单语句时，也要使用花括号：)

.. code-block:: c

	while (condition) {
		if (test)
			do_something();
	}

3.1) Spaces(空格)
*****************

Linux kernel style for use of spaces depends (mostly) on
function-versus-keyword usage.  Use a space after (most) keywords.  The
notable exceptions are sizeof, typeof, alignof, and __attribute__, which look
somewhat like functions (and are usually used with parentheses in Linux,
although they are not required in the language, as in: ``sizeof info`` after
``struct fileinfo info;`` is declared).
(Linux 内核风格对空格的使用 (大体上) 取决于"函数还是关键字"。在 (大多数) 关键字之后使用一个空格。著名的例外是 sizeof、typeof、alignof 和 __attribute__，它们看起来有点像函数 (在 Linux 里通常也带圆括号使用，尽管语言本身并不要求，例如：在声明 ``struct fileinfo info;`` 之后写 ``sizeof info``)。)

So use a space after these keywords::

	if, switch, case, for, do, while

(所以，请在这些关键字之后加一个空格：)

but not with sizeof, typeof, alignof, or __attribute__.  E.g.,

.. code-block:: c


	s = sizeof(struct file);

Do not add spaces around (inside) parenthesized expressions.  This example is
**bad**:

.. code-block:: c


	s = sizeof( struct file );

When declaring pointer data or a function that returns a pointer type, the
preferred use of ``*`` is adjacent to the data name or function name and not
adjacent to the type name.  Examples:

.. code-block:: c


	char *linux_banner;
	unsigned long long memparse(char *ptr, char **retptr);
	char *match_strdup(substring_t *s);

Use one space around (on each side of) most binary and ternary operators,
such as any of these::

	=  +  -  <  >  *  /  %  |  &  ^  <=  >=  ==  !=  ?  :

(大多数二元和三元运算符的两侧 (左右各一个) 要使用空格，比如下面这些：)

but no space after unary operators::

	&  *  +  -  ~  !  sizeof  typeof  alignof  __attribute__  defined

(但一元运算符之后不加空格：)

no space before the postfix increment & decrement unary operators::

	++  --

(后缀自增和自减一元运算符之前不加空格：)

no space after the prefix increment & decrement unary operators::

	++  --

(前缀自增和自减一元运算符之后不加空格：)

and no space around the ``.`` and ``->`` structure member operators.
(同时，``.`` 和 ``->`` 结构体成员运算符周围也不加空格。)

Do not leave trailing whitespace at the ends of lines.  Some editors with
``smart`` indentation will insert whitespace at the beginning of new lines as
appropriate, so you can start typing the next line of code right away.
However, some such editors do not remove the whitespace if you end up not
putting a line of code there, such as if you leave a blank line.  As a result,
you end up with lines containing trailing whitespace.
(不要在行尾留下多余的空白字符。有些带"智能"缩进功能的编辑器会适时在新行开头插入空白，好让你能立刻开始输入下一行代码。但其中一些编辑器在你最终没有在那一行输入代码时 (比如你留了一个空行)，并不会把这些空白删掉。结果就是，你会得到一些含有行尾空白的行。)

Git will warn you about patches that introduce trailing whitespace, and can
optionally strip the trailing whitespace for you; however, if applying a series
of patches, this may make later patches in the series fail by changing their
context lines.
(Git 会对引入行尾空白的补丁提出警告，并可以选择性地帮你去掉行尾空白；不过，如果应用的是一系列补丁，这种做法会改变后面补丁的上下文行，导致它们应用失败。)


4) Naming(命名)
---------------

C is a Spartan language, and your naming conventions should follow suit.
Unlike Modula-2 and Pascal programmers, C programmers do not use cute
names like ThisVariableIsATemporaryCounter. A C programmer would call that
variable ``tmp``, which is much easier to write, and not the least more
difficult to understand.
(C 是一门斯巴达式的语言，你的命名习惯也应如此。与 Modula-2 和 Pascal 程序员不同，C 程序员不会使用 ThisVariableIsATemporaryCounter 这样花哨的名字。C 程序员会把那个变量叫做 ``tmp``，这样写起来容易得多，而且一点都不难理解。)

HOWEVER, while mixed-case names are frowned upon, descriptive names for
global variables are a must.  To call a global function ``foo`` is a
shooting offense.
(不过，虽然混合大小写的名字让人皱眉，但全局变量必须使用具有描述性的名字。把一个全局函数叫做 ``foo`` 是一种该枪毙的罪行。)

GLOBAL variables (to be used only if you **really** need them) need to
have descriptive names, as do global functions.  If you have a function
that counts the number of active users, you should call that
``count_active_users()`` or similar, you should **not** call it ``cntusr()``.
(全局变量 (只有在确实需要时才使用) 需要有描述性的名字，全局函数也一样。如果你有一个统计活跃用户数量的函数，应该叫它 ``count_active_users()`` 之类的名字，而不是 ``cntusr()``。)

Encoding the type of a function into the name (so-called Hungarian
notation) is asinine - the compiler knows the types anyway and can check
those, and it only confuses the programmer.
(把类型信息编进函数名里 (即所谓的匈牙利命名法) 是愚蠢的做法——反正编译器知道这些类型，而且能检查它们，这种做法只会让程序员糊涂。)

LOCAL variable names should be short, and to the point.  If you have
some random integer loop counter, it should probably be called ``i``.
Calling it ``loop_counter`` is non-productive, if there is no chance of it
being mis-understood.  Similarly, ``tmp`` can be just about any type of
variable that is used to hold a temporary value.
(局部变量名应当简短扼要。如果你有一个随随便便的整数循环计数器，大概应该叫它 ``i``。叫它 ``loop_counter`` 毫无益处，前提是它不会被误解。同样，``tmp`` 可以是几乎任何用来存放临时值的变量。)

If you are afraid to mix up your local variable names, you have another
problem, which is called the function-growth-hormone-imbalance syndrome.
See chapter 6 (Functions).
(如果你害怕把自己的局部变量名搞混，那你就有另外一种毛病，叫做函数生长激素失衡综合征。参见第 6 章 (函数)。)

For symbol names and documentation, avoid introducing new usage of
'master / slave' (or 'slave' independent of 'master') and 'blacklist /
whitelist'.
(在符号名和文档中，避免引入 'master / slave' (或者脱离 'master' 单独使用的 'slave') 以及 'blacklist / whitelist' 的新用法。)

Recommended replacements for 'master / slave' are:
('master / slave' 的推荐替代词如下：)
    '{primary,main} / {secondary,replica,subordinate}'
    '{initiator,requester} / {target,responder}'
    '{controller,host} / {device,worker,proxy}'
    'leader / follower'
    'director / performer'

Recommended replacements for 'blacklist/whitelist' are:
('blacklist/whitelist' 的推荐替代词如下：)
    'denylist / allowlist'
    'blocklist / passlist'

Exceptions for introducing new usage is to maintain a userspace ABI/API,
or when updating code for an existing (as of 2020) hardware or protocol
specification that mandates those terms. For new specifications
translate specification usage of the terminology to the kernel coding
standard where possible.
(引入新用法的例外情形是：为了维护用户空间 ABI/API，或者在更新一个现有 (截至 2020 年) 硬件或协议规范的代码时，该规范强制要求这些术语。对于新规范，应尽可能把规范中的术语用法转译为内核编码标准所要求的用法。)

5) Typedefs(类型定义)
---------------------

Please don't use things like ``vps_t``.
It's a **mistake** to use typedef for structures and pointers. When you see a
(请不要使用 ``vps_t`` 这类东西。对结构体和指针使用 typedef 是一个错误。当你看到：)

.. code-block:: c


	vps_t a;

in the source, what does it mean?
(在源码中出现时，它是什么意思？)
In contrast, if it says
(相反，如果它写的是：)

.. code-block:: c

	struct virtual_container *a;

you can actually tell what ``a`` is.
(你就能真正知道 ``a`` 是什么。)

Lots of people think that typedefs ``help readability``. Not so. They are
useful only for:
(很多人认为 typedef 能"提高可读性"。并非如此。它们只在以下情形才有用：)

 (a) totally opaque objects (where the typedef is actively used to **hide**
     what the object is).
     (a) 完全不透明的对象 (此时 typedef 被主动用来隐藏对象到底是什么)。

     Example: ``pte_t`` etc. opaque objects that you can only access using
     the proper accessor functions.
     (例如：``pte_t`` 等不透明对象，你只能通过正确的访问器函数来访问它们。)

     .. note::

       Opaqueness and ``accessor functions`` are not good in themselves.
       The reason we have them for things like pte_t etc. is that there
       really is absolutely **zero** portably accessible information there.
       (不透明性和"访问器函数"本身并不是什么好事。之所以对 pte_t 这类东西采用它们，是因为那里确实绝对没有任何可移植的、可访问的信息。)

 (b) Clear integer types, where the abstraction **helps** avoid confusion
     whether it is ``int`` or ``long``.
     (b) 清晰的整数类型，这里的抽象有助于避免搞不清它究竟是 ``int`` 还是 ``long``。

     u8/u16/u32 are perfectly fine typedefs, although they fit into
     category (d) better than here.
     (u8/u16/u32 是完全没问题的 typedef，尽管它们更适合归入类别 (d) 而不是这里。)

     .. note::

       Again - there needs to be a **reason** for this. If something is
       ``unsigned long``, then there's no reason to do
       (再说一遍——这需要有理由。如果某个东西是 ``unsigned long``，那就没有理由写成：)

	typedef unsigned long myflags_t;

     but if there is a clear reason for why it under certain circumstances
     might be an ``unsigned int`` and under other configurations might be
     ``unsigned long``, then by all means go ahead and use a typedef.
     (但如果有一个明确的理由说明它在某些情况下可能是 ``unsigned int``、而在其他配置下可能是 ``unsigned long``，那么就尽管放心大胆地使用 typedef。)

 (c) when you use sparse to literally create a **new** type for
     type-checking.
     (c) 当你确实用 sparse 字面上创建一个新类型来做类型检查时。

 (d) New types which are identical to standard C99 types, in certain
     exceptional circumstances.
     (d) 在某些特定情况下，与标准 C99 类型完全相同的新类型。

     Although it would only take a short amount of time for the eyes and
     brain to become accustomed to the standard types like ``uint32_t``,
     some people object to their use anyway.
     (虽然让眼睛和大脑习惯 ``uint32_t`` 这类标准类型只需要一小段时间，但有些人还是反对使用它们。)

     Therefore, the Linux-specific ``u8/u16/u32/u64`` types and their
     signed equivalents which are identical to standard types are
     permitted -- although they are not mandatory in new code of your
     own.
     (因此，与标准类型相同的 Linux 专有 ``u8/u16/u32/u64`` 类型及其有符号等价类型是允许使用的——尽管在你自己的新代码中它们并非强制。)

     When editing existing code which already uses one or the other set
     of types, you should conform to the existing choices in that code.
     (在编辑已经使用了其中某一组类型的现有代码时，你应当遵循该代码中现有的选择。)

 (e) Types safe for use in userspace.
     (e) 可安全地在用户空间使用的类型。

     In certain structures which are visible to userspace, we cannot
     require C99 types and cannot use the ``u32`` form above. Thus, we
     use __u32 and similar types in all structures which are shared
     with userspace.
     (在某些对用户空间可见的结构体中，我们不能要求使用 C99 类型，也不能使用上面的 ``u32`` 形式。因此，在所有与用户空间共享的结构体中，我们使用 __u32 及类似的类型。)

Maybe there are other cases too, but the rule should basically be to NEVER
EVER use a typedef unless you can clearly match one of those rules.
(也许还有其他情况，但基本规则应当是：除非你能明确对应上述规则之一，否则永远都不要使用 typedef。)

In general, a pointer, or a struct that has elements that can reasonably
be directly accessed should **never** be a typedef.
(一般来说，指针，或者其成员可以被合理直接访问的结构体，永远都不应该被定义成 typedef。)


6) Functions(函数)
------------------

Functions should be short and sweet, and do just one thing.  They should
fit on one or two screenfuls of text (the ISO/ANSI screen size is 80x24,
as we all know), and do one thing and do that well.
(函数应当短小精悍，而且只做一件事。它们应当一到两屏就能放下 (众所周知，ISO/ANSI 屏幕大小是 80x24)，只做一件事，而且把这件事做好。)

The maximum length of a function is inversely proportional to the
complexity and indentation level of that function.  So, if you have a
conceptually simple function that is just one long (but simple)
case-statement, where you have to do lots of small things for a lot of
different cases, it's OK to have a longer function.
(函数的最大长度与其复杂度和缩进层级成反比。所以，如果你有一个概念上很简单的函数，只是一个很长 (但简单) 的 case 语句，需要针对很多不同的情况做许多小事，那么函数长一些也没关系。)

However, if you have a complex function, and you suspect that a
less-than-gifted first-year high-school student might not even
understand what the function is all about, you should adhere to the
maximum limits all the more closely.  Use helper functions with
descriptive names (you can ask the compiler to in-line them if you think
it's performance-critical, and it will probably do a better job of it
than you would have done).
(不过，如果你有一个复杂的函数，而且你怀疑一个资质平平的高一学生可能都看不懂这个函数在干什么，那你就更应该严格遵守上述上限。应当使用具有描述性名字的辅助函数 (如果你认为这是性能关键路径，可以请编译器把它们内联，而且它很可能做得比你自己做还要好)。)

Another measure of the function is the number of local variables.  They
shouldn't exceed 5-10, or you're doing something wrong.  Re-think the
function, and split it into smaller pieces.  A human brain can
generally easily keep track of about 7 different things, anything more
and it gets confused.  You know you're brilliant, but maybe you'd like
to understand what you did 2 weeks from now.
(衡量函数的另一个尺度是局部变量的数量。它们不应超过 5-10 个，否则你就是在做错事。重新审视这个函数，把它拆分成更小的片段。人脑一般可以轻松跟踪大约 7 件不同的事情，再多就会糊涂。你自知聪明过人，但也许你愿意在两周之后还能看懂自己当时干了什么。)

In source files, separate functions with one blank line.  If the function is
exported, the **EXPORT** macro for it should follow immediately after the
closing function brace line.  E.g.:
(在源文件中，用一个空行分隔函数。如果函数是导出的，它的 EXPORT 宏应当紧跟在函数结束花括号那一行之后。例如：)

.. code-block:: c

	int system_is_up(void)
	{
		return system_state == SYSTEM_RUNNING;
	}
	EXPORT_SYMBOL(system_is_up);

6.1) Function prototypes(函数原型)
**********************************

In function prototypes, include parameter names with their data types.
Although this is not required by the C language, it is preferred in Linux
because it is a simple way to add valuable information for the reader.
(在函数原型中，把参数名连同其数据类型一起写出来。虽然 C 语言并不要求这样做，但 Linux 里首选这样做，因为这是一种为读者提供有价值信息的简单方式。)

Do not use the ``extern`` keyword with function declarations as this makes
lines longer and isn't strictly necessary.
(不要在函数声明中使用 ``extern`` 关键字，因为这会让行变长，而且并非严格必要。)

When writing function prototypes, please keep the `order of elements regular
<https://lore.kernel.org/mm-commits/CAHk-=wiOCLRny5aifWNhr621kYrJwhfURsa0vFPeUEm8mF0ufg@mail.gmail.com/>`_.
For example, using this function declaration example::

 __init void * __must_check action(enum magic value, size_t size, u8 count,
				   char *fmt, ...) __printf(4, 5) __malloc;

(书写函数原型时，请保持元素顺序的规整性。例如，使用下面这个函数声明的例子：)

The preferred order of elements for a function prototype is:
(函数原型中元素的首选顺序是：)

- storage class (below, ``static __always_inline``, noting that ``__always_inline``
  is technically an attribute but is treated like ``inline``)
  (存储类 (下面是 ``static __always_inline``，注意 ``__always_inline`` 严格来说是一个属性，但被当作 ``inline`` 对待))
- storage class attributes (here, ``__init`` -- i.e. section declarations, but also
  things like ``__cold``)
  (存储类属性 (这里是 ``__init``——即段声明，但也包括 ``__cold`` 这类东西))
- return type (here, ``void *``)
  (返回类型 (这里是 ``void *``))
- return type attributes (here, ``__must_check``)
  (返回类型属性 (这里是 ``__must_check``))
- function name (here, ``action``)
  (函数名 (这里是 ``action``))
- function parameters (here, ``(enum magic value, size_t size, u8 count, char *fmt, ...)``,
  noting that parameter names should always be included)
  (函数参数 (这里是 ``(enum magic value, size_t size, u8 count, char *fmt, ...)``，注意参数名始终要写上))
- function parameter attributes (here, ``__printf(4, 5)``)
  (函数参数属性 (这里是 ``__printf(4, 5)``))
- function behavior attributes (here, ``__malloc``)
  (函数行为属性 (这里是 ``__malloc``))

Note that for a function **definition** (i.e. the actual function body),
the compiler does not allow function parameter attributes after the
function parameters. In these cases, they should go after the storage
class attributes (e.g. note the changed position of ``__printf(4, 5)``
below, compared to the **declaration** example above)::

 static __always_inline __init __printf(4, 5) void * __must_check action(enum magic value,
		size_t size, u8 count, char *fmt, ...) __malloc
 {
	...
 }

(注意，对于函数定义 (即实际的函数体)，编译器不允许把函数参数属性放在函数参数之后。在这些情况下，应把它们放在存储类属性之后 (例如，注意下面 ``__printf(4, 5)`` 的位置与上面的声明示例相比发生了变化)：)

7) Centralized exiting of functions(函数的集中退出)
---------------------------------------------------

Albeit deprecated by some people, the equivalent of the goto statement is
used frequently by compilers in form of the unconditional jump instruction.
(尽管被一些人诟病，goto 语句的等价物——无条件跳转指令——还是被编译器频繁使用。)

The goto statement comes in handy when a function exits from multiple
locations and some common work such as cleanup has to be done.  If there is no
cleanup needed then just return directly.
(当一个函数要从多个位置退出、并且必须做一些诸如清理之类的共同工作时，goto 语句就派上用场了。如果不需要清理，那就直接返回。)

Choose label names which say what the goto does or why the goto exists.  An
example of a good name could be ``out_free_buffer:`` if the goto frees ``buffer``.
Avoid using GW-BASIC names like ``err1:`` and ``err2:``, as you would have to
renumber them if you ever add or remove exit paths, and they make correctness
difficult to verify anyway.
(选择能够说明 goto 做了什么、或者 goto 为什么存在的标签名。如果 goto 用于释放 ``buffer``，那么一个好名字的例子可以是 ``out_free_buffer:``。避免使用 ``err1:`` 和 ``err2:`` 这种 GW-BASIC 风格的名字，因为一旦增删退出路径你就得给它们重新编号，而且它们无论如何都让正确性难以验证。)

The rationale for using gotos is:
(使用 goto 的理由是：)

- unconditional statements are easier to understand and follow
  (无条件语句更容易理解和跟进)
- nesting is reduced
  (减少嵌套)
- errors by not updating individual exit points when making
  modifications are prevented
  (避免修改时忘记更新某个单独退出点而出错)
- saves the compiler work to optimize redundant code away ;)
  (为编译器省去优化掉冗余代码的工作 ;)

.. code-block:: c

	int fun(int a)
	{
		int result = 0;
		char *buffer;

		buffer = kmalloc(SIZE, GFP_KERNEL);
		if (!buffer)
			return -ENOMEM;

		if (condition1) {
			while (loop1) {
				...
			}
			result = 1;
			goto out_free_buffer;
		}
		...
	out_free_buffer:
		kfree(buffer);
		return result;
	}

A common type of bug to be aware of is ``one err bugs`` which look like this:
(需要注意的一类常见 bug 是 ``one err bugs``（单一错误标签 bug），它看起来像这样：)

.. code-block:: c

	err:
		kfree(foo->bar);
		kfree(foo);
		return ret;

The bug in this code is that on some exit paths ``foo`` is NULL.  Normally the
fix for this is to split it up into two error labels ``err_free_bar:`` and
``err_free_foo:``:
(这段代码里的 bug 是：在某些退出路径上 ``foo`` 是 NULL。通常的修复方法，是把它拆分成两个错误标签 ``err_free_bar:`` 和 ``err_free_foo:``：)

.. code-block:: c

	err_free_bar:
		kfree(foo->bar);
	err_free_foo:
		kfree(foo);
		return ret;

Ideally you should simulate errors to test all exit paths.
(理想情况下，你应当模拟错误来测试所有退出路径。)


8) Commenting(注释)
-------------------

Comments are good, but there is also a danger of over-commenting.  NEVER
try to explain HOW your code works in a comment: it's much better to
write the code so that the **working** is obvious, and it's a waste of
time to explain badly written code.
(注释是好的，但也有注释过度的危险。永远不要试图在注释里解释你的代码是如何工作的：更好的做法是把代码写得让工作原理一目了然，而去解释写得很糟糕的代码纯属浪费时间。)

Generally, you want your comments to tell WHAT your code does, not HOW.
Also, try to avoid putting comments inside a function body: if the
function is so complex that you need to separately comment parts of it,
you should probably go back to chapter 6 for a while.  You can make
small comments to note or warn about something particularly clever (or
ugly), but try to avoid excess.  Instead, put the comments at the head
of the function, telling people what it does, and possibly WHY it does
it.
(一般而言，你希望注释说明你的代码做什么，而不是怎么做。另外，尽量避免在函数体内放注释：如果函数复杂到需要对其中的部分单独注释，那你大概应该回到第 6 章再修炼一阵子。你可以用小注释来标记或警告某些特别巧妙 (或特别丑陋) 的东西，但尽量避免过度。正确的做法是把注释放在函数头部，告诉人们它做什么，以及可能的话它为什么这么做。)

When commenting the kernel API functions, please use the kernel-doc format.
See the files at :ref:`Documentation/doc-guide/ <doc_guide>` and
``tools/docs/kernel-doc`` for details. Note that the danger of over-commenting
applies to kernel-doc comments all the same. Do not add boilerplate
kernel-doc which simply reiterates what's obvious from the signature
of the function.
(为内核 API 函数写注释时，请使用 kernel-doc 格式。详情参见 :ref:`Documentation/doc-guide/ <doc_guide>` 下的文件和 ``tools/docs/kernel-doc``。注意，注释过度的危险同样适用于 kernel-doc 注释。不要添加只是复述函数签名里显而易见之处的模板化 kernel-doc。)

The preferred style for long (multi-line) comments is:
(较长 (多行) 注释的首选风格是：)

.. code-block:: c

	/*
	 * This is the preferred style for multi-line
	 * comments in the Linux kernel source code.
	 * Please use it consistently.
	 *
	 * Description:  A column of asterisks on the left side,
	 * with beginning and ending almost-blank lines.
	 */

It's also important to comment data, whether they are basic types or derived
types.  To this end, use just one data declaration per line (no commas for
multiple data declarations).  This leaves you room for a small comment on each
item, explaining its use.
(为数据写注释同样重要，无论是基本类型还是派生类型。为此，每行只写一个数据声明 (多个数据声明之间不要用逗号)。这样你就为每个条目留出了写一条小注释来说明其用途的余地。)


9) You've made a mess of it(已经把事情搞砸了怎么办)
---------------------------------------------------

That's OK, we all do.  You've probably been told by your long-time Unix
user helper that ``GNU emacs`` automatically formats the C sources for
you, and you've noticed that yes, it does do that, but the defaults it
uses are less than desirable (in fact, they are worse than random
typing - an infinite number of monkeys typing into GNU emacs would never
make a good program).
(没关系，我们都会。你那位资深 Unix 用户朋友大概告诉过你，``GNU emacs`` 会自动帮你格式化 C 源码，而你也注意到，是的，它确实会这么做，但它用的默认设置并不理想 (实际上，它们比乱敲键盘还糟糕——让无穷多只猴子往 GNU emacs 里乱敲，也敲不出一个好程序来。)

So, you can either get rid of GNU emacs, or change it to use saner
values.  To do the latter, you can stick the following in your .emacs file:
(所以，你要么扔掉 GNU emacs，要么把它改成使用更理智的设置值。若选择后者，可以把下面这些内容放进你的 .emacs 文件：)

.. code-block:: elisp

  (defun c-lineup-arglist-tabs-only (ignored)
    "Line up argument lists by tabs, not spaces"
    (let* ((anchor (c-langelem-pos c-syntactic-element))
           (column (c-langelem-2nd-pos c-syntactic-element))
           (offset (- (1+ column) anchor))
           (steps (floor offset c-basic-offset)))
      (* (max steps 1)
         c-basic-offset)))

  (dir-locals-set-class-variables
   'linux-kernel
   '((c-mode . (
          (c-basic-offset . 8)
          (c-label-minimum-indentation . 0)
          (c-offsets-alist . (
                  (arglist-close         . c-lineup-arglist-tabs-only)
                  (arglist-cont-nonempty .
                      (c-lineup-gcc-asm-reg c-lineup-arglist-tabs-only))
                  (arglist-intro         . +)
                  (brace-list-intro      . +)
                  (c                     . c-lineup-C-comments)
                  (case-label            . 0)
                  (comment-intro         . c-lineup-comment)
                  (cpp-define-intro      . +)
                  (cpp-macro             . -1000)
                  (cpp-macro-cont        . +)
                  (defun-block-intro     . +)
                  (else-clause           . 0)
                  (func-decl-cont        . +)
                  (inclass               . +)
                  (inher-cont            . c-lineup-multi-inher)
                  (knr-argdecl-intro     . 0)
                  (label                 . -1000)
                  (statement             . 0)
                  (statement-block-intro . +)
                  (statement-case-intro  . +)
                  (statement-cont        . +)
                  (substatement          . +)
                  ))
          (indent-tabs-mode . t)
          (show-trailing-whitespace . t)
          ))))

  (dir-locals-set-directory-class
   (expand-file-name "~/src/linux-trees")
   'linux-kernel)

This will make emacs go better with the kernel coding style for C
files below ``~/src/linux-trees``.
(这会让 emacs 在处理 ``~/src/linux-trees`` 以下的 C 文件时，更好地配合内核编码风格。)

But even if you fail in getting emacs to do sane formatting, not
everything is lost: use ``indent``.
(不过，就算你没能让 emacs 做出理智的格式化，也并非万事皆休：用 ``indent``。)

Now, again, GNU indent has the same brain-dead settings that GNU emacs
has, which is why you need to give it a few command line options.
However, that's not too bad, because even the makers of GNU indent
recognize the authority of K&R (the GNU people aren't evil, they are
just severely misguided in this matter), so you just give indent the
options ``-kr -i8`` (stands for ``K&R, 8 character indents``), or use
``scripts/Lindent``, which indents in the latest style.
(再说一次，GNU indent 和 GNU emacs 一样有着脑残的默认设置，这就是为什么你需要给它几个命令行选项。不过这还不算太糟，因为连 GNU indent 的作者们都承认 K&R 的权威 (GNU 的人并不邪恶，他们只是在这件事上被严重误导了)，所以你只要给 indent 加上 ``-kr -i8`` 选项 (代表 ``K&R, 8 character indents``)，或者使用 ``scripts/Lindent``，它会按最新风格缩进。)

``indent`` has a lot of options, and especially when it comes to comment
re-formatting you may want to take a look at the man page.  But
remember: ``indent`` is not a fix for bad programming.
(``indent`` 有很多选项，尤其是涉及注释重排时，你可能想看看它的 man 手册页。但请记住：``indent`` 治不了糟糕的编程。)

Note that you can also use the ``clang-format`` tool to help you with
these rules, to quickly re-format parts of your code automatically,
and to review full files in order to spot coding style mistakes,
typos and possible improvements. It is also handy for sorting ``#includes``,
for aligning variables/macros, for reflowing text and other similar tasks.
See the file :ref:`Documentation/dev-tools/clang-format.rst <clangformat>`
for more details.
(注意，你也可以使用 ``clang-format`` 工具来帮你执行这些规则：快速自动重排代码的某些部分，或者检查整个文件，以便发现编码风格错误、拼写错误和可能的改进之处。它在给 ``#includes`` 排序、对齐变量/宏、重排文本以及其他类似任务上也很趁手。详见文件 :ref:`Documentation/dev-tools/clang-format.rst <clangformat>`。)

Some basic editor settings, such as indentation and line endings, will be
set automatically if you are using an editor that is compatible with
EditorConfig. See the official EditorConfig website for more information:
https://editorconfig.org/
(一些基本的编辑器设置，比如缩进和行尾符，会在你使用兼容 EditorConfig 的编辑器时自动设置好。更多信息请参见 EditorConfig 官方网站：https://editorconfig.org/)

10) Kconfig configuration files(Kconfig 配置文件)
-------------------------------------------------

For all of the Kconfig* configuration files throughout the source tree,
the indentation is somewhat different.  Lines under a ``config`` definition
are indented with one tab, while help text is indented an additional two
spaces.  Example::
(对于整个源码树中所有 Kconfig* 配置文件，缩进方式有些不同。``config`` 定义之下的行用一个制表符缩进，而帮助文本则在此基础上再缩进两个空格。示例：)

  config AUDIT
	bool "Auditing support"
	depends on NET
	help
	  Enable auditing infrastructure that can be used with another
	  kernel subsystem, such as SELinux (which requires this for
	  logging of avc messages output).  Does not do system-call
	  auditing without CONFIG_AUDITSYSCALL.

Seriously dangerous features (such as write support for certain
filesystems) should advertise this prominently in their prompt string::
(具有严重危险性的特性 (比如某些文件系统的写支持)，应当在它们的提示字符串中醒目地声明这一点：)

  config ADFS_FS_RW
	bool "ADFS write support (DANGEROUS)"
	depends on ADFS_FS
	...

For full documentation on the configuration files, see the file
Documentation/kbuild/kconfig-language.rst.
(关于配置文件的完整文档，请参见文件 Documentation/kbuild/kconfig-language.rst。)


11) Data structures(数据结构)
-----------------------------

Data structures that have visibility outside the single-threaded
environment they are created and destroyed in should always have
reference counts.  In the kernel, garbage collection doesn't exist (and
outside the kernel garbage collection is slow and inefficient), which
means that you absolutely **have** to reference count all your uses.
(凡在创建和销毁它们的单线程环境之外具有可见性的数据结构，都应当有引用计数。在内核里，垃圾回收并不存在 (在内核之外，垃圾回收又慢又低效)，这意味着你绝对必须对你的一切使用都做引用计数。)

Reference counting means that you can avoid locking, and allows multiple
users to have access to the data structure in parallel - and not having
to worry about the structure suddenly going away from under them just
because they slept or did something else for a while.
(引用计数意味着你可以避免加锁，并且允许多个用户并行访问该数据结构——而不必担心结构会在他们睡觉或暂时去做别的事情时突然从脚下消失。)

Note that locking is **not** a replacement for reference counting.
Locking is used to keep data structures coherent, while reference
counting is a memory management technique.  Usually both are needed, and
they are not to be confused with each other.
(注意，加锁不是引用计数的替代品。加锁用于保持数据结构的一致性，而引用计数是一种内存管理技术。通常两者都需要，不要把它们混为一谈。)

Many data structures can indeed have two levels of reference counting,
when there are users of different ``classes``.  The subclass count counts
the number of subclass users, and decrements the global count just once
when the subclass count goes to zero.
(确实，当存在不同"类别"的用户时，很多数据结构可以有两级引用计数。子类计数统计子类用户的数量，并且在子类计数归零时只把全局计数减一。)

Examples of this kind of ``multi-level-reference-counting`` can be found in
memory management (``struct mm_struct``: mm_users and mm_count), and in
filesystem code (``struct super_block``: s_count and s_active).
(这种"多级引用计数"的例子可以在内存管理 (``struct mm_struct``：mm_users 和 mm_count) 和文件系统代码 (``struct super_block``：s_count 和 s_active) 中找到。)

Remember: if another thread can find your data structure, and you don't
have a reference count on it, you almost certainly have a bug.
(记住：如果另一个线程能够找到你的数据结构，而你没有对它做引用计数，那你几乎可以肯定有一个 bug。)


12) Macros, Enums and RTL(宏、枚举和 RTL)
-----------------------------------------

Names of macros defining constants and labels in enums are capitalized.
(定义常量的宏以及枚举中的标签名都采用大写。)

.. code-block:: c

	#define CONSTANT 0x12345

Enums are preferred when defining several related constants.
(定义若干相关常量时，首选枚举。)

CAPITALIZED macro names are appreciated but macros resembling functions
may be named in lower case.
(全大写的宏名受人欢迎，但形似函数的宏可以用小写命名。)

Generally, inline functions are preferable to macros resembling functions.
(一般而言，内联函数比形似函数的宏更可取。)

Macros with multiple statements should be enclosed in a do - while block:
(含有多条语句的宏应当用 do - while 块包裹起来：)

.. code-block:: c

	#define macrofun(a, b, c)			\
		do {					\
			if (a == 5)			\
				do_this(b, c);		\
		} while (0)

Function-like macros with unused parameters should be replaced by static
inline functions to avoid the issue of unused variables:
(带未使用参数的形似函数的宏，应当替换成 static inline 函数，以避免出现未使用变量的问题：)

.. code-block:: c

	static inline void fun(struct foo *foo)
	{
	}

Due to historical practices, many files still employ the "cast to (void)"
approach to evaluate parameters. However, this method is not advisable.
Inline functions address the issue of "expression with side effects
evaluated more than once", circumvent unused-variable problems, and
are generally better documented than macros for some reason.
(由于历史习惯，很多文件仍然采用"强制转换成 (void)"的做法来使用参数。然而，这种方法并不可取。内联函数解决了"带副作用的表达式被求值多次"的问题，避开了未使用变量问题，而且出于某种原因，它们的文档一般也比宏写得好。)

.. code-block:: c

	/*
	 * Avoid doing this whenever possible and instead opt for static
	 * inline functions
	 */
	#define macrofun(foo) do { (void) (foo); } while (0)

Things to avoid when using macros:
(使用宏时要避免的事情：)

1) macros that affect control flow:
(1) 影响控制流的宏：)

.. code-block:: c

	#define FOO(x)					\
		do {					\
			if (blah(x) < 0)		\
				return -EBUGGERED;	\
		} while (0)

is a **very** bad idea.  It looks like a function call but exits the ``calling``
function; don't break the internal parsers of those who will read the code.
(这是一个非常糟糕的主意。它看起来像一个函数调用，却从调用者函数中退出；不要破坏读代码者头脑中的内部解析器。)

2) macros that depend on having a local variable with a magic name:
(2) 依赖于带魔名的局部变量的宏：)

.. code-block:: c

	#define FOO(val) bar(index, val)

might look like a good thing, but it's confusing as hell when one reads the
code and it's prone to breakage from seemingly innocent changes.
(它可能看起来是个好东西，但读代码时会把人搞得晕头转向，而且看似无害的改动就容易把它弄坏。)

3) macros with arguments that are used as l-values: FOO(x) = y; will
bite you if somebody e.g. turns FOO into an inline function.
(3) 参数被当作左值使用的宏：FOO(x) = y; 这种写法，一旦有人比如把 FOO 改成了内联函数，就会反咬你一口。

4) forgetting about precedence: macros defining constants using expressions
must enclose the expression in parentheses. Beware of similar issues with
macros using parameters.
(4) 忘记优先级：用表达式定义常量的宏，必须把表达式用圆括号包起来。使用参数的宏也要当心类似问题。)

.. code-block:: c

	#define CONSTANT 0x4000
	#define CONSTEXP (CONSTANT | 3)

5) namespace collisions when defining local variables in macros resembling
functions:
(5) 在形似函数的宏里定义局部变量时的命名空间冲突：)

.. code-block:: c

	#define FOO(x)				\
	({					\
		typeof(x) ret;			\
		ret = calc_ret(x);		\
		(ret);				\
	})

ret is a common name for a local variable - __foo_ret is less likely
to collide with an existing variable.
(ret 是局部变量的常见名字——__foo_ret 与现有变量发生冲突的可能性就小得多。)

The cpp manual deals with macros exhaustively. The gcc internals manual also
covers RTL which is used frequently with assembly language in the kernel.
(cpp 手册对宏有详尽的论述。gcc 内部手册也涵盖 RTL，它在内核中经常与汇编语言一起使用。)


13) Printing kernel messages(打印内核消息)
------------------------------------------

Kernel developers like to be seen as literate. Do mind the spelling
of kernel messages to make a good impression. Do not use incorrect
contractions like ``dont``; use ``do not`` or ``don't`` instead. Make the
messages concise, clear, and unambiguous.
(内核开发者喜欢被人视为有文化的人。请留意内核消息的拼写，给人留下好印象。不要使用 ``dont`` 这类不正确的缩写；要用 ``do not`` 或 ``don't``。让消息简洁、清晰、无歧义。)

Kernel messages do not have to be terminated with a period.
(内核消息不必以句号结尾。)

Printing numbers in parentheses (%d) adds no value and should be avoided.
(把数字打印在圆括号里 (%d) 没有任何价值，应当避免。)

There are a number of driver model diagnostic macros in <linux/dev_printk.h>
which you should use to make sure messages are matched to the right device
and driver, and are tagged with the right level:  dev_err(), dev_warn(),
dev_info(), and so forth.  For messages that aren't associated with a
particular device, <linux/printk.h> defines pr_notice(), pr_info(),
pr_warn(), pr_err(), etc. When drivers are working properly they are quiet,
so prefer to use dev_dbg/pr_debug unless something is wrong.
(<linux/dev_printk.h> 中有若干驱动模型诊断宏，你应当使用它们，以确保消息能匹配到正确的设备和驱动，并被标上正确的级别：dev_err()、dev_warn()、dev_info() 等等。对于与特定设备无关的消息，<linux/printk.h> 定义了 pr_notice()、pr_info()、pr_warn()、pr_err() 等。驱动工作正常时它们是安静的，所以除非出了问题，否则请优先使用 dev_dbg/pr_debug。)

Coming up with good debugging messages can be quite a challenge; and once
you have them, they can be a huge help for remote troubleshooting.  However
debug message printing is handled differently than printing other non-debug
messages.  While the other pr_XXX() functions print unconditionally,
pr_debug() does not; it is compiled out by default, unless either DEBUG is
defined or CONFIG_DYNAMIC_DEBUG is set.  That is true for dev_dbg() also,
and a related convention uses VERBOSE_DEBUG to add dev_vdbg() messages to
the ones already enabled by DEBUG.
(想出好的调试消息相当有挑战；而一旦你有了它们，它们对远程排障的帮助会非常大。不过，调试消息的打印与其他非调试消息的打印处理方式不同。其他 pr_XXX() 函数无条件打印，而 pr_debug() 不然；它默认在编译时被剔除，除非定义了 DEBUG 或者设置了 CONFIG_DYNAMIC_DEBUG。dev_dbg() 也是如此，还有一个相关的约定是用 VERBOSE_DEBUG 把 dev_vdbg() 消息加到 DEBUG 已经启用的那批消息里。)

Many subsystems have Kconfig debug options to turn on -DDEBUG in the
corresponding Makefile; in other cases specific files #define DEBUG.  And
when a debug message should be unconditionally printed, such as if it is
already inside a debug-related #ifdef section, printk(KERN_DEBUG ...) can be
used.
(很多子系统都有 Kconfig 调试选项，用来在相应的 Makefile 中打开 -DDEBUG；在其他情况下，特定文件会 #define DEBUG。而当一条调试消息应当无条件打印时，比如它已经位于某个与调试相关的 #ifdef 区段之内，就可以使用 printk(KERN_DEBUG ...)。)


14) Allocating memory(内存分配)
-------------------------------

The kernel provides the following general purpose memory allocators:
kmalloc(), kzalloc(), kmalloc_array(), kcalloc(), vmalloc(), and
vzalloc().  Please refer to the API documentation for further information
about them.  :ref:`Documentation/core-api/memory-allocation.rst
<memory_allocation>`
(内核提供以下通用内存分配器：kmalloc()、kzalloc()、kmalloc_array()、kcalloc()、vmalloc() 和 vzalloc()。有关它们的更多信息请参阅 API 文档：:ref:`Documentation/core-api/memory-allocation.rst
<memory_allocation>`)

The preferred form for passing a size of a struct is the following:
(传递结构体大小的首选形式如下：)

.. code-block:: c

	p = kmalloc(sizeof(*p), ...);

The alternative form where struct name is spelled out hurts readability and
introduces an opportunity for a bug when the pointer variable type is changed
but the corresponding sizeof that is passed to a memory allocator is not.
(把结构体名字明确写出来的另一种形式会损害可读性，而且当指针变量的类型改变了、但传给内存分配器的相应 sizeof 却没有跟着改时，会埋下 bug 的隐患。)

Casting the return value which is a void pointer is redundant. The conversion
from void pointer to any other pointer type is guaranteed by the C programming
language.
(对返回的 void 指针做强制类型转换是多余的。从 void 指针到任何其他指针类型的转换由 C 语言本身保证。)

The preferred form for allocating an array is the following:
(分配数组的首选形式如下：)

.. code-block:: c

	p = kmalloc_array(n, sizeof(...), ...);

The preferred form for allocating a zeroed array is the following:
(分配清零数组的首选形式如下：)

.. code-block:: c

	p = kcalloc(n, sizeof(...), ...);

Both forms check for overflow on the allocation size n * sizeof(...),
and return NULL if that occurred.
(这两种形式都会检查分配大小 n * sizeof(...) 是否溢出，若发生溢出则返回 NULL。)

These generic allocation functions all emit a stack dump on failure when used
without __GFP_NOWARN so there is no use in emitting an additional failure
message when NULL is returned.
(在不带 __GFP_NOWARN 使用时，这些通用分配函数在失败时都会输出栈回溯，所以在返回 NULL 之后再额外打印一条失败消息毫无用处。)

15) The inline disease(inline 病)
---------------------------------

There appears to be a common misperception that gcc has a magic "make me
faster" speedup option called ``inline``. While the use of inlines can be
appropriate (for example as a means of replacing macros, see Chapter 12), it
very often is not. Abundant use of the inline keyword leads to a much bigger
kernel, which in turn slows the system as a whole down, due to a bigger
icache footprint for the CPU and simply because there is less memory
available for the pagecache. Just think about it; a pagecache miss causes a
disk seek, which easily takes 5 milliseconds. There are a LOT of cpu cycles
that can go into these 5 milliseconds.
(人们似乎普遍有一种误解，认为 gcc 有一个叫 ``inline`` 的神奇"让我更快"加速选项。虽然使用内联函数有时是合适的 (比如作为替换宏的手段，见第 12 章)，但很多时候并不合适。滥用 inline 关键字会导致内核体积大增，进而拖慢整个系统：一方面 CPU 的 icache 占用更大，另一方面留给 pagecache 的内存更少了。想想看：一次 pagecache 未命中会引发一次磁盘寻道，轻松耗去 5 毫秒。这 5 毫秒里可以塞进去非常多的 CPU 周期。)

A reasonable rule of thumb is to not put inline at functions that have more
than 3 lines of code in them. An exception to this rule are the cases where
a parameter is known to be a compile time constant, and as a result of this
constantness you *know* the compiler will be able to optimize most of your
function away at compile time. For a good example of this later case, see
the kmalloc() inline function.
(一个合理的经验法则是：不要给超过 3 行代码的函数加 inline。这条规则的例外是：参数已知是编译期常量，而且正因为这种常量性，你*知道*编译器将能在编译期把你的函数大部分优化掉。对于后一种情况，可以看看 kmalloc() 内联函数这个好例子。)

Often people argue that adding inline to functions that are static and used
only once is always a win since there is no space tradeoff. While this is
technically correct, gcc is capable of inlining these automatically without
help, and the maintenance issue of removing the inline when a second user
appears outweighs the potential value of the hint that tells gcc to do
something it would have done anyway.
(人们常常争辩说，给 static 且只被使用一次的函数加 inline 总是有利无害，因为不存在空间的权衡。虽然这在技术上没错，但 gcc 无需帮助就能自动内联这些函数；而当出现第二个使用者时，你就得记得去掉 inline，这个维护负担超过了那个提示的潜在价值——它不过是让 gcc 去做一件它本来就会做的事。)


16) Function return values and names(函数返回值与命名)
------------------------------------------------------

Functions can return values of many different kinds, and one of the
most common is a value indicating whether the function succeeded or
failed.  Such a value can be represented as an error-code integer
(-Exxx = failure, 0 = success) or a ``succeeded`` boolean (0 = failure,
non-zero = success).
(函数可以返回许多不同种类的值，其中最常见的一种是指示函数成功还是失败的值。这样的值可以表示为错误码整数 (-Exxx = 失败，0 = 成功)，或者表示为"成功"布尔值 (0 = 失败，非零 = 成功)。)

Mixing up these two sorts of representations is a fertile source of
difficult-to-find bugs.  If the C language included a strong distinction
between integers and booleans then the compiler would find these mistakes
for us... but it doesn't.  To help prevent such bugs, always follow this
convention::
(把这两种表示方式搞混，是难查 bug 的丰饶温床。如果 C 语言对整数和布尔值有严格的区分，编译器本可以替我们找出这些错误……但它没有。为帮助预防此类 bug，请始终遵循以下约定：)

	If the name of a function is an action or an imperative command,
	the function should return an error-code integer.  If the name
	is a predicate, the function should return a "succeeded" boolean.

For example, ``add work`` is a command, and the add_work() function returns 0
for success or -EBUSY for failure.  In the same way, ``PCI device present`` is
a predicate, and the pci_dev_present() function returns 1 if it succeeds in
finding a matching device or 0 if it doesn't.
(例如，``add work`` 是一条命令，所以 add_work() 函数成功时返回 0，失败时返回 -EBUSY。同样，``PCI device present`` 是一个谓词，所以 pci_dev_present() 函数在成功找到匹配设备时返回 1，否则返回 0。)

All EXPORTed functions must respect this convention, and so should all
public functions.  Private (static) functions need not, but it is
recommended that they do.
(所有 EXPORT 出去的函数都必须遵守这一约定，所有公开函数也应当如此。私有 (static) 函数不作强制要求，但同样建议遵守。)

Functions whose return value is the actual result of a computation, rather
than an indication of whether the computation succeeded, are not subject to
this rule.  Generally they indicate failure by returning some out-of-range
result.  Typical examples would be functions that return pointers; they use
NULL or the ERR_PTR mechanism to report failure.
(返回值是计算的实际结果、而非计算是否成功之指示的函数，不受这条规则约束。它们一般通过返回某个超出范围的值来指示失败。典型的例子是返回指针的函数；它们用 NULL 或 ERR_PTR 机制来报告失败。)


17) Using bool(使用 bool)
-------------------------

The Linux kernel bool type is an alias for the C99 _Bool type. bool values can
only evaluate to 0 or 1, and implicit or explicit conversion to bool
automatically converts the value to true or false. When using bool types the
!! construction is not needed, which eliminates a class of bugs.
(Linux 内核的 bool 类型是 C99 _Bool 类型的别名。bool 值只能取 0 或 1，到 bool 的隐式或显式转换都会自动把值转换为 true 或 false。使用 bool 类型时不需要 !! 这种构造，从而消灭了一类 bug。)

When working with bool values the true and false definitions should be used
instead of 1 and 0.
(处理 bool 值时，应当使用 true 和 false 定义，而不是 1 和 0。)

bool function return types and stack variables are always fine to use whenever
appropriate. Use of bool is encouraged to improve readability and is often a
better option than 'int' for storing boolean values.
(只要合适，bool 函数返回类型和栈变量总是可以放心使用的。鼓励使用 bool 来提高可读性，对于存储布尔值来说，它往往比 'int' 是更好的选择。)

Do not use bool if cache line layout or size of the value matters, as its size
and alignment varies based on the compiled architecture. Structures that are
optimized for alignment and size should not use bool.
(如果缓存行布局或值的大小很重要，就不要使用 bool，因为它的大小和对齐随编译目标架构而变化。为对齐和大小做了优化的结构体不应使用 bool。)

If a structure has many true/false values, consider consolidating them into a
bitfield with 1 bit members, or using an appropriate fixed width type, such as
u8.
(如果一个结构体有很多 true/false 值，考虑把它们合并成 1 位成员的位域，或者使用合适的定宽类型，比如 u8。)

Similarly for function arguments, many true/false values can be consolidated
into a single bitwise 'flags' argument and 'flags' can often be a more
readable alternative if the call-sites have naked true/false constants.
(同理，对于函数参数，许多 true/false 值可以合并成一个按位操作的 'flags' 参数；如果调用点有裸的 true/false 常量，'flags' 往往是可读性更好的替代方案。)

Otherwise limited use of bool in structures and arguments can improve
readability.
(此外，在结构和参数中有限地使用 bool 可以提高可读性。)

18) Don't re-invent the kernel macros(不要重复发明内核宏)
---------------------------------------------------------

There are many header files in include/linux/ that contain a number of macros that
you should use, rather than explicitly coding some variant of them yourself.
For example, if you need to calculate the length of an array, take advantage
of the macro
(include/linux/ 中有许多头文件，里面包含了不少你应当直接使用的宏，而不是自己显式去编写它们的某种变体。例如，如果你需要计算数组的长度，请利用这个宏：)

.. code-block:: c

	#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

which is defined in array_size.h.
(它定义在 array_size.h 中。)

Similarly, if you need to calculate the size of some structure member, use
(同样，如果你需要计算某个结构体成员的大小，请使用：)

.. code-block:: c

	#define sizeof_field(t, f) (sizeof(((t*)0)->f))

which is defined in stddef.h.
(它定义在 stddef.h 中。)

There are also min() and max() macros defined in minmax.h that do strict type checking
if you need them. Feel free to peruse the header files to see what else is already
defined that you shouldn't reproduce in your code.
(minmax.h 中还定义了 min() 和 max() 宏，它们做严格的类型检查，需要时可以使用。尽管去翻阅这些头文件，看看还有哪些已经定义好的东西，免得在你的代码里重造轮子。)


19) Editor modelines and other cruft(编辑器模式行与其他杂物)
------------------------------------------------------------

Some editors can interpret configuration information embedded in source files,
indicated with special markers.  For example, emacs interprets lines marked
like this:
(有些编辑器可以解读嵌入在源文件里的配置信息，这些信息用特殊标记表示。例如，emacs 会解读下面这样标记的行：)

.. code-block:: c

	-*- mode: c -*-

Or like this:
(或者像这样：)

.. code-block:: c

	/*
	Local Variables:
	compile-command: "gcc -DMAGIC_DEBUG_FLAG foo.c"
	End:
	*/

Vim interprets markers that look like this:
(Vim 会解读看起来像这样的标记：)

.. code-block:: c

	/* vim:set sw=8 noet */

Do not include any of these in source files.  People have their own personal
editor configurations, and your source files should not override them.  This
includes markers for indentation and mode configuration.  People may use their
own custom mode, or may have some other magic method for making indentation
work correctly.
(不要在源文件里包含任何这类东西。人们都有自己个人的编辑器配置，你的源文件不应覆盖它们。这包括用于缩进和模式配置的标记。人们可能使用自己定制的模式，也可能有其他让缩进正确工作的魔法方法。)


20) Inline assembly(内联汇编)
-----------------------------

In architecture-specific code, you may need to use inline assembly to interface
with CPU or platform functionality.  Don't hesitate to do so when necessary.
However, don't use inline assembly gratuitously when C can do the job.  You can
and should poke hardware from C when possible.
(在架构专属代码中，你可能需要使用内联汇编来对接 CPU 或平台功能。必要时不要犹豫。不过，在 C 能胜任的场合，不要滥用内联汇编。只要可能，你能够、也应当用 C 去操作硬件。)

Consider writing simple helper functions that wrap common bits of inline
assembly, rather than repeatedly writing them with slight variations.  Remember
that inline assembly can use C parameters.
(考虑编写简单的辅助函数来封装常用的内联汇编片段，而不是带着细微差异一遍遍地重写。记住，内联汇编可以使用 C 参数。)

Large, non-trivial assembly functions should go in .S files, with corresponding
C prototypes defined in C header files.  The C prototypes for assembly
functions should use ``asmlinkage``.
(较大、不平凡的汇编函数应当放进 .S 文件，并在 C 头文件里定义相应的 C 原型。汇编函数的 C 原型应当使用 ``asmlinkage``。)

You may need to mark your asm statement as volatile, to prevent GCC from
removing it if GCC doesn't notice any side effects.  You don't always need to
do so, though, and doing so unnecessarily can limit optimization.
(你可能需要把 asm 语句标记为 volatile，以防 GCC 在没有注意到任何副作用时把它删掉。不过你并不总是需要这样做，而且不必要地这样做会限制优化。)

When writing a single inline assembly statement containing multiple
instructions, put each instruction on a separate line in a separate quoted
string, and end each string except the last with ``\n\t`` to properly indent
the next instruction in the assembly output:
(编写包含多条指令的单条内联汇编语句时，把每条指令放在单独一行的单独带引号字符串里，并且除最后一个字符串外，每个字符串都以 ``\n\t`` 结尾，以便在汇编输出中正确缩进下一条指令：)

.. code-block:: c

	asm ("magic %reg1, #42\n\t"
	     "more_magic %reg2, %reg3"
	     : /* outputs */ : /* inputs */ : /* clobbers */);


21) Conditional Compilation(条件编译)
-------------------------------------

Wherever possible, don't use preprocessor conditionals (#if, #ifdef) in .c
files; doing so makes code harder to read and logic harder to follow.  Instead,
use such conditionals in a header file defining functions for use in those .c
files, providing no-op stub versions in the #else case, and then call those
functions unconditionally from .c files.  The compiler will avoid generating
any code for the stub calls, producing identical results, but the logic will
remain easy to follow.
(只要可能，就不要在 .c 文件里使用预处理器条件 (#if、#ifdef)；这样做会让代码更难读、逻辑更难跟上。相反，应当把这类条件放在头文件中，在头文件里为那些 .c 文件定义要使用的函数，并在 #else 分支提供无操作 (no-op) 的桩版本，然后从 .c 文件里无条件地调用这些函数。编译器会避免为桩调用生成任何代码，最终结果相同，但逻辑依然清晰易读。)

Prefer to compile out entire functions, rather than portions of functions or
portions of expressions.  Rather than putting an ifdef in an expression, factor
out part or all of the expression into a separate helper function and apply the
conditional to that function.
(尽量把整个函数编译剔除掉，而不是函数的一部分或表达式的一部分。与其在表达式里放 ifdef，不如把表达式的一部分或全部提取成独立的辅助函数，然后把条件施加到那个函数上。)

If you have a function or variable which may potentially go unused in a
particular configuration, and the compiler would warn about its definition
going unused, mark the definition as __maybe_unused rather than wrapping it in
a preprocessor conditional.  (However, if a function or variable *always* goes
unused, delete it.)
(如果某个函数或变量在特定配置下可能不被使用、而编译器会就其未使用的定义发出警告，那么把该定义标记为 __maybe_unused，而不是用预处理器条件包住它。 (不过，如果一个函数或变量*始终*不被使用，就删掉它。))

Within code, where possible, use the IS_ENABLED macro to convert a Kconfig
symbol into a C boolean expression, and use it in a normal C conditional:
(在代码中，只要可能，就使用 IS_ENABLED 宏把 Kconfig 符号转换成 C 布尔表达式，并在普通的 C 条件判断中使用它：)

.. code-block:: c

	if (IS_ENABLED(CONFIG_SOMETHING)) {
		...
	}

The compiler will constant-fold the conditional away, and include or exclude
the block of code just as with an #ifdef, so this will not add any runtime
overhead.  However, this approach still allows the C compiler to see the code
inside the block, and check it for correctness (syntax, types, symbol
references, etc).  Thus, you still have to use an #ifdef if the code inside the
block references symbols that will not exist if the condition is not met.
(编译器会把这个条件常量折叠掉，像 #ifdef 一样包含或排除这段代码块，因此这不会带来任何运行时开销。不过，这种方式仍能让 C 编译器看到块内的代码，并检查其正确性 (语法、类型、符号引用等)。因此，如果块内代码引用的符号在条件不满足时将不存在，你仍然得使用 #ifdef。)

At the end of any non-trivial #if or #ifdef block (more than a few lines),
place a comment after the #endif on the same line, noting the conditional
expression used.  For instance:
(在任何不平凡的 #if 或 #ifdef 块 (超过几行的) 结束处，在 #endif 同一行后面加一条注释，注明所用的条件表达式。例如：)

.. code-block:: c

	#ifdef CONFIG_SOMETHING
	...
	#endif /* CONFIG_SOMETHING */


22) Do not crash the kernel(不要让内核崩溃)
-------------------------------------------

In general, the decision to crash the kernel belongs to the user, rather
than to the kernel developer.
(一般来说，是否让内核崩溃的决定属于用户，而不属于内核开发者。)

Avoid panic()(避免 panic())
***************************

panic() should be used with care and primarily only during system boot.
panic() is, for example, acceptable when running out of memory during boot and
not being able to continue.
(panic() 应当谨慎使用，并且主要只在系统引导阶段使用。例如，引导期间内存耗尽且无法继续时，使用 panic() 是可以接受的。)

Use WARN() rather than BUG()(用 WARN() 而不是 BUG())
****************************************************

Do not add new code that uses any of the BUG() variants, such as BUG(),
BUG_ON(), or VM_BUG_ON(). Instead, use a WARN*() variant, preferably
WARN_ON_ONCE(), and possibly with recovery code. Recovery code is not
required if there is no reasonable way to at least partially recover.
(不要添加使用任何 BUG() 变体 (如 BUG()、BUG_ON() 或 VM_BUG_ON()) 的新代码。应当改用 WARN*() 变体，最好是 WARN_ON_ONCE()，并且可以带上恢复代码。如果没有任何合理的办法至少部分恢复，则不要求有恢复代码。)

"I'm too lazy to do error handling" is not an excuse for using BUG(). Major
internal corruptions with no way of continuing may still use BUG(), but need
good justification.
("我懒得做错误处理"不是使用 BUG() 的借口。无法继续运行的重大内部损坏仍然可以使用 BUG()，但需要有充分的理由。)

Use WARN_ON_ONCE() rather than WARN() or WARN_ON()(用 WARN_ON_ONCE() 而不是 WARN() 或 WARN_ON())
************************************************************************************************

WARN_ON_ONCE() is generally preferred over WARN() or WARN_ON(), because it
is common for a given warning condition, if it occurs at all, to occur
multiple times. This can fill up and wrap the kernel log, and can even slow
the system enough that the excessive logging turns into its own, additional
problem.
(通常首选 WARN_ON_ONCE() 而不是 WARN() 或 WARN_ON()，因为某个给定的警告条件一旦发生，往往会发生多次。这会填满并回绕内核日志，甚至可能把系统拖慢到使过量的日志输出本身成为又一个新问题。)

Do not WARN lightly(不要轻易 WARN)
**********************************

WARN*() is intended for unexpected, this-should-never-happen situations.
WARN*() macros are not to be used for anything that is expected to happen
during normal operation. These are not pre- or post-condition asserts, for
example. Again: WARN*() must not be used for a condition that is expected
to trigger easily, for example, by user space actions. pr_warn_once() is a
possible alternative, if you need to notify the user of a problem.
(WARN*() 旨在用于"这本不该发生"的意外情形。WARN*() 宏不应用于正常操作期间预期会发生的任何事情。例如，它们不是前置或后置条件断言。再强调一遍：WARN*() 绝不能用于预期容易触发的条件，比如由用户空间动作触发的条件。如果你需要把问题通知给用户，pr_warn_once() 是一个可能的替代。)

Do not worry about panic_on_warn users(不要为 panic_on_warn 用户担心)
**********************************************************************

A few more words about panic_on_warn: Remember that ``panic_on_warn`` is an
available kernel option, and that many users set this option. This is why
there is a "Do not WARN lightly" writeup, above. However, the existence of
panic_on_warn users is not a valid reason to avoid the judicious use
WARN*(). That is because, whoever enables panic_on_warn has explicitly
asked the kernel to crash if a WARN*() fires, and such users must be
prepared to deal with the consequences of a system that is somewhat more
likely to crash.
(关于 panic_on_warn 再多说几句：记住 ``panic_on_warn`` 是一个可用的内核选项，而且很多用户都设置了这个选项。这就是为什么前面有"不要轻易 WARN"的论述。但是，panic_on_warn 用户的存在并不是避免审慎使用 WARN*() 的正当理由。这是因为，凡是启用 panic_on_warn 的人，都已经明确要求内核在 WARN*() 触发时崩溃，这类用户必须准备好承受一个崩溃可能性略高的系统所带来的后果。)

Use BUILD_BUG_ON() for compile-time assertions(用 BUILD_BUG_ON() 做编译期断言)
******************************************************************************

The use of BUILD_BUG_ON() is acceptable and encouraged, because it is a
compile-time assertion that has no effect at runtime.
(使用 BUILD_BUG_ON() 是可以接受并受到鼓励的，因为它是一个在运行时不产生任何影响的编译期断言。)

Appendix I) References(附录 I：参考资料)
----------------------------------------

The C Programming Language, Second Edition
by Brian W. Kernighan and Dennis M. Ritchie.
Prentice Hall, Inc., 1988.
ISBN 0-13-110362-8 (paperback), 0-13-110370-9 (hardback).
(The C Programming Language, Second Edition，Brian W. Kernighan 和 Dennis M. Ritchie 著，Prentice Hall, Inc.，1988 年。ISBN 0-13-110362-8 (平装)，0-13-110370-9 (精装)。)

The Practice of Programming
by Brian W. Kernighan and Rob Pike.
Addison-Wesley, Inc., 1999.
ISBN 0-201-61586-X.
(The Practice of Programming，Brian W. Kernighan 和 Rob Pike 著，Addison-Wesley, Inc.，1999 年。ISBN 0-201-61586-X。)

GNU manuals - where in compliance with K&R and this text - for cpp, gcc,
gcc internals and indent, all available from https://www.gnu.org/manual/
(与 K&R 及本文一致的 GNU 手册——cpp、gcc、gcc internals 和 indent——都可以从 https://www.gnu.org/manual/ 获取。)

WG14 is the international standardization working group for the programming
language C, URL: http://www.open-std.org/JTC1/SC22/WG14/
(WG14 是 C 编程语言的国际化标准工作组，URL：http://www.open-std.org/JTC1/SC22/WG14/)

Kernel CodingStyle, by greg@kroah.com at OLS 2002:
http://www.kroah.com/linux/talks/ols_2002_kernel_codingstyle_talk/html/
(Kernel CodingStyle，greg@kroah.com 在 OLS 2002 上的演讲：http://www.kroah.com/linux/talks/ols_2002_kernel_codingstyle_talk/html/)
