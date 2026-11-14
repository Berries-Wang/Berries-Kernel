.. SPDX-License-Identifier: GPL-2.0

======================
The seq_file Interface
======================

(seq_file 接口)

	Copyright 2003 Jonathan Corbet <corbet@lwn.net>
	(版权所有 2003 Jonathan Corbet <corbet@lwn.net>)

	This file is originally from the LWN.net Driver Porting series at
	https://lwn.net/Articles/driver-porting/
	(本文最初来自 LWN.net 驱动移植系列：https://lwn.net/Articles/driver-porting/)


There are numerous ways for a device driver (or other kernel component) to
provide information to the user or system administrator.  One useful
technique is the creation of virtual files, in debugfs, /proc or elsewhere.
Virtual files can provide human-readable output that is easy to get at
without any special utility programs; they can also make life easier for
script writers. It is not surprising that the use of virtual files has
grown over the years.
(设备驱动程序(或其他内核组件)向用户或系统管理员提供信息的方式有很多。一种有用的技术是创建虚拟文件，放在 debugfs、/proc 或其他地方。虚拟文件可以提供人类可读的输出，无需任何特殊的实用程序即可轻松获取；它们还能让脚本编写者的工作更加轻松。毫不奇怪，虚拟文件的使用多年来一直在增长。)

Creating those files correctly has always been a bit of a challenge,
however. It is not that hard to make a virtual file which returns a
string. But life gets trickier if the output is long - anything greater
than an application is likely to read in a single operation.  Handling
multiple reads (and seeks) requires careful attention to the reader's
position within the virtual file - that position is, likely as not, in the
middle of a line of output. The kernel has traditionally had a number of
implementations that got this wrong.
(然而，正确创建这些文件一直是一件有点挑战性的事。创建一个返回字符串的虚拟文件并不难。但如果输出很长——超过应用程序一次操作可能读取的量——情况就变得更棘手了。处理多次读取(以及定位)需要仔细关注读取者在虚拟文件中的位置——那个位置很可能位于一行输出的中间。内核传统上有许多实现都把这一点弄错了。)

The kernel now contains a set of functions (implemented by Alexander Viro)
which are designed to make it easy for virtual file creators to get it
right.
(内核现在包含一组函数(由 Alexander Viro 实现)，旨在让虚拟文件的创建者能够轻松地把它做对。)

The seq_file interface is available via <linux/seq_file.h>. There are
three aspects to seq_file:
(seq_file 接口可通过 <linux/seq_file.h> 使用。seq_file 有三个方面：)

     * An iterator interface which lets a virtual file implementation
       step through the objects it is presenting.
       (一个迭代器接口，让虚拟文件的实现可以逐步遍历它所呈现的对象。)

     * Some utility functions for formatting objects for output without
       needing to worry about things like output buffers.
       (一些实用函数，用于将对象格式化以便输出，而无需担心输出缓冲区之类的问题。)

     * A set of canned file_operations which implement most operations on
       the virtual file.
       (一组现成的 file_operations，实现了虚拟文件上的大多数操作。)

We'll look at the seq_file interface via an extremely simple example: a
loadable module which creates a file called /proc/sequence. The file, when
read, simply produces a set of increasing integer values, one per line. The
sequence will continue until the user loses patience and finds something
better to do. The file is seekable, in that one can do something like the
following::
(我们将通过一个极其简单的示例来了解 seq_file 接口：一个可加载模块，它创建一个名为 /proc/sequence 的文件。该文件被读取时只是产生一组递增的整数值，每行一个。这个序列会一直持续，直到用户失去耐心并去找更好的事情做。该文件是可定位的，例如可以像下面这样操作：)

    dd if=/proc/sequence of=out1 count=1
    dd if=/proc/sequence skip=1 of=out2 count=1

Then concatenate the output files out1 and out2 and get the right
result. Yes, it is a thoroughly useless module, but the point is to show
how the mechanism works without getting lost in other details.  (Those
wanting to see the full source for this module can find it at
https://lwn.net/Articles/22359/).
(然后把输出文件 out1 和 out2 拼接起来就能得到正确的结果。是的，这是一个完全无用的模块，但重点是在不陷入其他细节的情况下展示这个机制是如何工作的。(想查看该模块完整源码的人可以在 https://lwn.net/Articles/22359/ 找到它。)

Deprecated create_proc_entry
(已弃用的 create_proc_entry)
============================

Note that the above article uses create_proc_entry which was removed in
kernel 3.10. Current versions require the following update::
(请注意，上面那篇文章使用了 create_proc_entry，它在内核 3.10 中已被移除。当前版本需要做如下更新：)

    -	entry = create_proc_entry("sequence", 0, NULL);
    -	if (entry)
    -		entry->proc_fops = &ct_file_ops;
    +	entry = proc_create("sequence", 0, NULL, &ct_file_ops);

The iterator interface
(迭代器接口)
======================

Modules implementing a virtual file with seq_file must implement an
iterator object that allows stepping through the data of interest
during a "session" (roughly one read() system call).  If the iterator
is able to move to a specific position - like the file they implement,
though with freedom to map the position number to a sequence location
in whatever way is convenient - the iterator need only exist
transiently during a session.  If the iterator cannot easily find a
numerical position but works well with a first/next interface, the
iterator can be stored in the private data area and continue from one
session to the next.
(使用 seq_file 实现虚拟文件的模块必须实现一个迭代器对象，以便在一次"会话"(大致对应一次 read() 系统调用)期间逐步遍历所关注的数据。如果迭代器能够移动到某个特定位置——就像它所实现的文件那样，只是可以自由地以任何方便的方式把位置编号映射到序列中的某个位置——那么迭代器只需在会话期间临时存在即可。如果迭代器无法轻易找到数值位置，但能与 first/next 式接口良好配合，那么迭代器可以保存在私有数据区中，从一个会话延续到下一个会话。)

A seq_file implementation that is formatting firewall rules from a
table, for example, could provide a simple iterator that interprets
position N as the Nth rule in the chain.  A seq_file implementation
that presents the content of a, potentially volatile, linked list
might record a pointer into that list, providing that can be done
without risk of the current location being removed.
(例如，对来自某个表的防火墙规则进行格式化的 seq_file 实现，可以提供一个简单的迭代器，把位置 N 解释为链中的第 N 条规则。而呈现一个可能易变的链表内容的 seq_file 实现，可能会记录一个指向该链表的指针，前提是这样做不会冒当前位置被移除的风险。)

Positioning can thus be done in whatever way makes the most sense for
the generator of the data, which need not be aware of how a position
translates to an offset in the virtual file. The one obvious exception
is that a position of zero should indicate the beginning of the file.
(因此，定位可以采用对数据生成者最有意义的任何方式进行，数据生成者无需知道位置如何转换成虚拟文件中的偏移量。一个明显的例外是，位置为零应表示文件的开头。)

The /proc/sequence iterator just uses the count of the next number it
will output as its position.
(/proc/sequence 迭代器只是把它将要输出的下一个数字的计数用作其位置。)

Four functions must be implemented to make the iterator work. The
first, called start(), starts a session and takes a position as an
argument, returning an iterator which will start reading at that
position.  The pos passed to start() will always be either zero, or
the most recent pos used in the previous session.
(要让迭代器工作起来，必须实现四个函数。第一个叫做 start()，它启动一个会话，接受一个位置作为参数，返回一个将从该位置开始读取的迭代器。传给 start() 的 pos 总是零，或者是上一个会话中最近使用的 pos。)

For our simple sequence example,
the start() function looks like::
(对于我们这个简单的 sequence 示例，start() 函数如下所示：)

	static void *ct_seq_start(struct seq_file *s, loff_t *pos)
	{
	        loff_t *spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
	        if (! spos)
	                return NULL;
	        *spos = *pos;
	        return spos;
	}

The entire data structure for this iterator is a single loff_t value
holding the current position. There is no upper bound for the sequence
iterator, but that will not be the case for most other seq_file
implementations; in most cases the start() function should check for a
"past end of file" condition and return NULL if need be.
(这个迭代器的全部数据结构就是一个保存当前位置的 loff_t 值。sequence 迭代器没有上限，但对大多数其他 seq_file 实现来说情况并非如此；在大多数情况下，start() 函数应当检查"越过文件末尾"的情况，并在需要时返回 NULL。)

For more complicated applications, the private field of the seq_file
structure can be used to hold state from session to session.  There is
also a special value which can be returned by the start() function
called SEQ_START_TOKEN; it can be used if you wish to instruct your
show() function (described below) to print a header at the top of the
output. SEQ_START_TOKEN should only be used if the offset is zero,
however.  SEQ_START_TOKEN has no special meaning to the core seq_file
code.  It is provided as a convenience for a start() function to
communicate with the next() and show() functions.
(对于更复杂的应用，seq_file 结构体的 private 字段可用来在会话之间保存状态。start() 函数还可以返回一个名为 SEQ_START_TOKEN 的特殊值；如果你想指示你的 show() 函数(下文描述)在输出的顶部打印一个表头，就可以使用它。不过，SEQ_START_TOKEN 只应在偏移量为零时使用。SEQ_START_TOKEN 对 seq_file 核心代码没有任何特殊含义。它是为方便 start() 函数与 next() 和 show() 函数通信而提供的。)

The next function to implement is called, amazingly, next(); its job is to
move the iterator forward to the next position in the sequence.  The
example module can simply increment the position by one; more useful
modules will do what is needed to step through some data structure. The
next() function returns a new iterator, or NULL if the sequence is
complete. Here's the example version::
(下一个要实现的函数，令人惊讶地，叫做 next()；它的工作是把迭代器向前移动到序列中的下一个位置。示例模块可以简单地把位置加一；更有用的模块则会执行必要的操作来逐步遍历某个数据结构。next() 函数返回一个新的迭代器，如果序列已经结束则返回 NULL。下面是示例版本：)

	static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
	{
	        loff_t *spos = v;
	        *pos = ++*spos;
	        return spos;
	}

The next() function should set ``*pos`` to a value that start() can use
to find the new location in the sequence.  When the iterator is being
stored in the private data area, rather than being reinitialized on each
start(), it might seem sufficient to simply set ``*pos`` to any non-zero
value (zero always tells start() to restart the sequence).  This is not
sufficient due to historical problems.
(next() 函数应当把 ``*pos`` 设置为 start() 可以用来找到序列中新位置的值。当迭代器保存在私有数据区中、而不是在每次 start() 时重新初始化的情况下，仅仅把 ``*pos`` 设置为任意非零值(零总是告诉 start() 重新开始序列)似乎就足够了。但由于历史上的问题，这样做并不足够。)

Historically, many next() functions have *not* updated ``*pos`` at
end-of-file.  If the value is then used by start() to initialise the
iterator, this can result in corner cases where the last entry in the
sequence is reported twice in the file.  In order to discourage this bug
from being resurrected, the core seq_file code now produces a warning if
a next() function does not change the value of ``*pos``.  Consequently a
next() function *must* change the value of ``*pos``, and of course must
set it to a non-zero value.
(历史上，许多 next() 函数在文件末尾处*没有*更新 ``*pos``。如果这个值随后被 start() 用来初始化迭代器，就可能产生序列中最后一个条目在文件中被报告两次这样的边缘情况。为了避免这个缺陷死灰复燃，现在如果 next() 函数没有改变 ``*pos`` 的值，seq_file 核心代码会发出警告。因此，next() 函数*必须*改变 ``*pos`` 的值，当然也必须把它设置为非零值。)

The stop() function closes a session; its job, of course, is to clean
up. If dynamic memory is allocated for the iterator, stop() is the
place to free it; if a lock was taken by start(), stop() must release
that lock.  The value that ``*pos`` was set to by the last next() call
before stop() is remembered, and used for the first start() call of
the next session unless lseek() has been called on the file; in that
case next start() will be asked to start at position zero::
(stop() 函数关闭一个会话；当然，它的任务是做清理。如果为迭代器分配了动态内存，stop() 就是释放它的地方；如果 start() 拿到了锁，stop() 必须释放那把锁。stop() 之前最后一次 next() 调用设置给 ``*pos`` 的值会被记住，并用于下一个会话的第一次 start() 调用，除非对该文件调用了 lseek()；那种情况下，下一次 start() 将被要求从位置零开始：)

	static void ct_seq_stop(struct seq_file *s, void *v)
	{
	        kfree(v);
	}

Finally, the show() function should format the object currently pointed to
by the iterator for output.  The example module's show() function is::
(最后，show() 函数应当把迭代器当前指向的对象格式化以便输出。示例模块的 show() 函数是：)

	static int ct_seq_show(struct seq_file *s, void *v)
	{
	        loff_t *spos = v;
	        seq_printf(s, "%lld\n", (long long)*spos);
	        return 0;
	}

If all is well, the show() function should return zero.  A negative error
code in the usual manner indicates that something went wrong; it will be
passed back to user space.  This function can also return SEQ_SKIP, which
causes the current item to be skipped; if the show() function has already
generated output before returning SEQ_SKIP, that output will be dropped.
(如果一切正常，show() 函数应当返回零。以通常的方式返回一个负的错误码表示出了问题；它会被传回用户空间。这个函数也可以返回 SEQ_SKIP，这会导致当前条目被跳过；如果 show() 函数在返回 SEQ_SKIP 之前已经产生了输出，那些输出将被丢弃。)

We will look at seq_printf() in a moment. But first, the definition of the
seq_file iterator is finished by creating a seq_operations structure with
the four functions we have just defined::
(我们稍后会看 seq_printf()。但首先，通过用我们刚刚定义的四个函数创建一个 seq_operations 结构体，seq_file 迭代器的定义就完成了：)

	static const struct seq_operations ct_seq_ops = {
	        .start = ct_seq_start,
	        .next  = ct_seq_next,
	        .stop  = ct_seq_stop,
	        .show  = ct_seq_show
	};

This structure will be needed to tie our iterator to the /proc file in
a little bit.
(稍后就需要用这个结构体把我们的迭代器绑定到 /proc 文件上。)

It's worth noting that the iterator value returned by start() and
manipulated by the other functions is considered to be completely opaque by
the seq_file code. It can thus be anything that is useful in stepping
through the data to be output. Counters can be useful, but it could also be
a direct pointer into an array or linked list. Anything goes, as long as
the programmer is aware that things can happen between calls to the
iterator function. However, the seq_file code (by design) will not sleep
between the calls to start() and stop(), so holding a lock during that time
is a reasonable thing to do. The seq_file code will also avoid taking any
other locks while the iterator is active.
(值得注意的是，start() 返回并由其他函数操作的迭代器值被 seq_file 代码视为完全不透明。因此，它可以是任何对逐步遍历待输出数据有用的东西。计数器可能有用，但它也可以是指向数组或链表的直接指针。什么都行，只要程序员意识到在迭代器函数的各次调用之间可能会发生一些事情。不过，seq_file 代码(出于设计)在调用 start() 和 stop() 之间不会睡眠，因此在这段时间内持有锁是合理的做法。当迭代器处于活动状态时，seq_file 代码也会避免获取任何其他锁。)

The iterator value returned by start() or next() is guaranteed to be
passed to a subsequent next() or stop() call.  This allows resources
such as locks that were taken to be reliably released.  There is *no*
guarantee that the iterator will be passed to show(), though in practice
it often will be.
(start() 或 next() 返回的迭代器值保证会被传递给后续的某个 next() 或 stop() 调用。这使得已获取的锁等资源能够被可靠地释放。迭代器*没有*保证会被传递给 show()，尽管在实践中通常会是。)


Formatted output
(格式化输出)
================

The seq_file code manages positioning within the output created by the
iterator and getting it into the user's buffer. But, for that to work, that
output must be passed to the seq_file code. Some utility functions have
been defined which make this task easy.
(seq_file 代码负责管理迭代器所产生输出中的定位，并把输出送入用户的缓冲区。但要使其工作，输出必须被传递给 seq_file 代码。为此定义了一些实用函数，使这项任务变得容易。)

Most code will simply use seq_printf(), which works pretty much like
printk(), but which requires the seq_file pointer as an argument.
(大多数代码会直接使用 seq_printf()，它的工作方式与 printk() 非常相似，但需要把 seq_file 指针作为参数传入。)

For straight character output, the following functions may be used::
(对于纯字符输出，可以使用以下函数：)

	seq_putc(struct seq_file *m, char c);
	seq_puts(struct seq_file *m, const char *s);
	seq_escape(struct seq_file *m, const char *s, const char *esc);

The first two output a single character and a string, just like one would
expect. seq_escape() is like seq_puts(), except that any character in s
which is in the string esc will be represented in octal form in the output.
(前两个函数输出单个字符和一个字符串，正如人们所期望的那样。seq_escape() 类似于 seq_puts()，只是 s 中任何出现在字符串 esc 中的字符都会以八进制形式在输出中表示。)

There are also a pair of functions for printing filenames::
(还有一对用于打印文件名的函数：)

	int seq_path(struct seq_file *m, const struct path *path,
		     const char *esc);
	int seq_path_root(struct seq_file *m, const struct path *path,
			  const struct path *root, const char *esc)

Here, path indicates the file of interest, and esc is a set of characters
which should be escaped in the output.  A call to seq_path() will output
the path relative to the current process's filesystem root.  If a different
root is desired, it can be used with seq_path_root().  If it turns out that
path cannot be reached from root, seq_path_root() returns SEQ_SKIP.
(这里，path 指示所关注的文件，esc 是一组应在输出中被转义的字符。调用 seq_path() 将输出相对于当前进程文件系统根目录的路径。如果想要不同的根，可以改用 seq_path_root()。如果发现从 root 无法到达 path，seq_path_root() 会返回 SEQ_SKIP。)

A function producing complicated output may want to check::
(产生复杂输出的函数可能想要检查：)

	bool seq_has_overflowed(struct seq_file *m);

and avoid further seq_<output> calls if true is returned.
(如果返回 true，则避免继续调用 seq_<output>。)

A true return from seq_has_overflowed means that the seq_file buffer will
be discarded and the seq_show function will attempt to allocate a larger
buffer and retry printing.
(seq_has_overflowed 返回 true 意味着 seq_file 缓冲区将被丢弃，seq_show 函数会尝试分配一个更大的缓冲区并重新尝试打印。)


Making it all work
(让这一切运作起来)
==================

So far, we have a nice set of functions which can produce output within the
seq_file system, but we have not yet turned them into a file that a user
can see. Creating a file within the kernel requires, of course, the
creation of a set of file_operations which implement the operations on that
file. The seq_file interface provides a set of canned operations which do
most of the work. The virtual file author still must implement the open()
method, however, to hook everything up. The open function is often a single
line, as in the example module::
(到目前为止，我们已经有了一组能在 seq_file 系统中产生输出的不错的函数，但还没有把它们变成用户可以看到的文件。当然，在内核中创建文件需要创建一组 file_operations 来实现针对该文件的操作。seq_file 接口提供了一组现成的操作，可以完成大部分工作。不过，虚拟文件的作者仍然必须实现 open() 方法来把一切连接起来。open 函数通常只有一行，如示例模块中所示：)

	static int ct_open(struct inode *inode, struct file *file)
	{
		return seq_open(file, &ct_seq_ops);
	}

Here, the call to seq_open() takes the seq_operations structure we created
before, and gets set up to iterate through the virtual file.
(这里，对 seq_open() 的调用接受我们之前创建的 seq_operations 结构体，并完成设置以遍历虚拟文件。)

On a successful open, seq_open() stores the struct seq_file pointer in
file->private_data. If you have an application where the same iterator can
be used for more than one file, you can store an arbitrary pointer in the
private field of the seq_file structure; that value can then be retrieved
by the iterator functions.
(成功打开后，seq_open() 会把 struct seq_file 指针存储在 file->private_data 中。如果你的应用中同一个迭代器可用于多个文件，你可以在 seq_file 结构体的 private 字段中存储一个任意指针；之后迭代器函数就可以取回该值。)

There is also a wrapper function to seq_open() called seq_open_private(). It
kmallocs a zero filled block of memory and stores a pointer to it in the
private field of the seq_file structure, returning 0 on success. The
block size is specified in a third parameter to the function, e.g.::
(还有一个 seq_open() 的封装函数，叫做 seq_open_private()。它用 kmalloc 分配一块填零的内存块，并把指向它的指针存储在 seq_file 结构体的 private 字段中，成功时返回 0。块大小通过该函数的第三个参数指定，例如：)

	static int ct_open(struct inode *inode, struct file *file)
	{
		return seq_open_private(file, &ct_seq_ops,
					sizeof(struct mystruct));
	}

There is also a variant function, __seq_open_private(), which is functionally
identical except that, if successful, it returns the pointer to the allocated
memory block, allowing further initialisation e.g.::
(还有一个变体函数 __seq_open_private()，它在功能上完全相同，只是如果成功，它会返回指向已分配内存块的指针，从而允许进行进一步的初始化，例如：)

	static int ct_open(struct inode *inode, struct file *file)
	{
		struct mystruct *p =
			__seq_open_private(file, &ct_seq_ops, sizeof(*p));

		if (!p)
			return -ENOMEM;

		p->foo = bar; /* initialize my stuff */
			...
		p->baz = true;

		return 0;
	}

A corresponding close function, seq_release_private() is available which
frees the memory allocated in the corresponding open.
(还有一个对应的关闭函数 seq_release_private() 可用，它释放相应 open 中分配的内存。)

The other operations of interest - read(), llseek(), and release() - are
all implemented by the seq_file code itself. So a virtual file's
file_operations structure will look like::
(其他值得关注的操作——read()、llseek() 和 release()——都由 seq_file 代码自身实现。因此，虚拟文件的 file_operations 结构体将如下所示：)

	static const struct file_operations ct_file_ops = {
	        .owner   = THIS_MODULE,
	        .open    = ct_open,
	        .read    = seq_read,
	        .llseek  = seq_lseek,
	        .release = seq_release
	};

There is also a seq_release_private() which passes the contents of the
seq_file private field to kfree() before releasing the structure.
(还有一个 seq_release_private()，它在释放结构体之前先把 seq_file private 字段的内容传给 kfree()。)

The final step is the creation of the /proc file itself. In the example
code, that is done in the initialization code in the usual way::
(最后一步是创建 /proc 文件本身。在示例代码中，这是在初始化代码里以通常的方式完成的：)

	static int ct_init(void)
	{
	        struct proc_dir_entry *entry;

	        proc_create("sequence", 0, NULL, &ct_file_ops);
	        return 0;
	}

	module_init(ct_init);

And that is pretty much it.
(基本就是这样了。)


seq_list
(seq_list(序列链表))
========

If your file will be iterating through a linked list, you may find these
routines useful::
(如果你的文件要遍历一个链表，你可能会发现这些例程很有用：)

	struct list_head *seq_list_start(struct list_head *head,
	       		 		 loff_t pos);
	struct list_head *seq_list_start_head(struct list_head *head,
			 		      loff_t pos);
	struct list_head *seq_list_next(void *v, struct list_head *head,
					loff_t *ppos);

These helpers will interpret pos as a position within the list and iterate
accordingly.  Your start() and next() functions need only invoke the
``seq_list_*`` helpers with a pointer to the appropriate list_head structure.
(这些辅助函数会把 pos 解释为链表中的位置并据此进行迭代。你的 start() 和 next() 函数只需用指向相应 list_head 结构体的指针调用 ``seq_list_*`` 辅助函数即可。)


The extra-simple version
(额外简单的版本)
========================

For extremely simple virtual files, there is an even easier interface.  A
module can define only the show() function, which should create all the
output that the virtual file will contain. The file's open() method then
calls::
(对于极其简单的虚拟文件，还有一个更加容易的接口。模块可以只定义 show() 函数，它应当生成虚拟文件将包含的全部输出。然后文件的 open() 方法调用：)

	int single_open(struct file *file,
	                int (*show)(struct seq_file *m, void *p),
	                void *data);

When output time comes, the show() function will be called once. The data
value given to single_open() can be found in the private field of the
seq_file structure. When using single_open(), the programmer should use
single_release() instead of seq_release() in the file_operations structure
to avoid a memory leak.
(到了输出的时候，show() 函数会被调用一次。传给 single_open() 的 data 值可以在 seq_file 结构体的 private 字段中找到。使用 single_open() 时，程序员应当在 file_operations 结构体中使用 single_release() 而不是 seq_release()，以避免内存泄漏。)
