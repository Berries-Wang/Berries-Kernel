.. _stable_api_nonsense:

=====================================================
The Linux Kernel Driver Interface(Linux 内核驱动接口)
=====================================================

(all of your questions answered and then some)
(回答你的所有问题，甚至更多)

Greg Kroah-Hartman <greg@kroah.com>

This is being written to try to explain why Linux **does not have a binary
kernel interface, nor does it have a stable kernel interface**.
(撰写本文是为了解释为什么 Linux **既没有二进制内核接口，也没有稳定的内核接口**。)

.. note::

  Please realize that this article describes the **in kernel** interfaces, not
  the kernel to userspace interfaces.
  (请注意，本文描述的是 **内核内部** 的接口，而不是内核到用户空间的接口。)

  The kernel to userspace interface is the one that application programs use,
  the syscall interface.  That interface is **very** stable over time, and
  will not break.  I have old programs that were built on a pre 0.9something
  kernel that still work just fine on the latest 2.6 kernel release.
  That interface is the one that users and application programmers can count
  on being stable.
  (内核到用户空间的接口是应用程序所使用的接口，即系统调用接口。该接口随着时间推移是 **非常** 稳定的，不会被破坏。我有一些在 0.9 之前的内核上构建的老程序，它们在最新的 2.6 内核发行版上仍然运行良好。用户和应用程序程序员可以放心依赖这一接口的稳定性。)


Executive Summary(内容提要)
---------------------------
You think you want a stable kernel interface, but you really do not, and
you don't even know it.  What you want is a stable running driver, and
you get that only if your driver is in the main kernel tree.  You also
get lots of other good benefits if your driver is in the main kernel
tree, all of which has made Linux into such a strong, stable, and mature
operating system which is the reason you are using it in the first
place.
(你以为自己想要一个稳定的内核接口，但其实并不需要，而且你自己甚至没有意识到这一点。你真正想要的是一个运行稳定的驱动，而只有把你的驱动放进主内核树才能做到这一点。如果你的驱动进入了主内核树，你还会得到许多其他好处——正是这一切使 Linux 成为一个如此强大、稳定、成熟的操作系统，而这正是你一开始就选择使用它的原因。)


Intro(引言)
-----------

It's only the odd person who wants to write a kernel driver that needs
to worry about the in-kernel interfaces changing.  For the majority of
the world, they neither see this interface, nor do they care about it at
all.
(只有少数想编写内核驱动的人才需要担心内核内部接口的变化。对世界上的绝大多数人来说，他们既看不到这个接口，也根本不关心它。)

First off, I'm not going to address **any** legal issues about closed
source, hidden source, binary blobs, source wrappers, or any other term
that describes kernel drivers that do not have their source code
released under the GPL.  Please consult a lawyer if you have any legal
questions, I'm a programmer and hence, I'm just going to be describing
the technical issues here (not to make light of the legal issues, they
are real, and you do need to be aware of them at all times.)
(首先，我不会讨论 **任何** 关于闭源、隐藏源码、二进制 blob、源码包装器，或任何其他描述未按 GPL 发布源码的内核驱动的术语的法律问题。如果你有任何法律问题，请咨询律师。我是程序员，因此在这里只讨论技术问题(这并不意味着轻视法律问题——它们是真实存在的，你确实需要时刻注意它们)。)

So, there are two main topics here, binary kernel interfaces and stable
kernel source interfaces.  They both depend on each other, but we will
discuss the binary stuff first to get it out of the way.
(所以，这里有两个主要议题：二进制内核接口与稳定的内核源码接口。两者相互依存，但我们会先讨论二进制的问题，把它先解决掉。)


Binary Kernel Interface(二进制内核接口)
---------------------------------------
Assuming that we had a stable kernel source interface for the kernel, a
binary interface would naturally happen too, right?  Wrong.  Please
consider the following facts about the Linux kernel:
(假设我们为内核提供了稳定的内核源码接口，二进制接口自然也会随之稳定，对吗？错了。请考虑以下关于 Linux 内核的事实：)

  - Depending on the version of the C compiler you use, different kernel
    data structures will contain different alignment of structures, and
    possibly include different functions in different ways (putting
    functions inline or not.)  The individual function organization
    isn't that important, but the different data structure padding is
    very important.
    (根据你所使用的 C 编译器版本的不同，内核数据结构会包含不同的结构体对齐方式，并且可能以不同方式包含不同的函数(是否内联函数)。单个函数的组织方式并不那么重要，但不同的数据结构填充(padding)非常重要。)

  - Depending on what kernel build options you select, a wide range of
    different things can be assumed by the kernel:
    (根据你选择的内核构建选项，内核可以做出各种不同的假设：)

      - different structures can contain different fields
        (不同的结构体可以包含不同的字段)
      - Some functions may not be implemented at all, (i.e. some locks
	compile away to nothing for non-SMP builds.)
        (某些函数可能完全没有实现(例如在非 SMP 构建中，某些锁会被编译得无影无踪)。)
      - Memory within the kernel can be aligned in different ways,
	depending on the build options.
        (内核中的内存可以根据构建选项以不同方式对齐。)

  - Linux runs on a wide range of different processor architectures.
    There is no way that binary drivers from one architecture will run
    on another architecture properly.
    (Linux 运行在大量不同的处理器架构上。一个架构的二进制驱动绝无可能在另一个架构上正常运行。)

Now a number of these issues can be addressed by simply compiling your
module for the exact specific kernel configuration, using the same exact
C compiler that the kernel was built with.  This is sufficient if you
want to provide a module for a specific release version of a specific
Linux distribution.  But multiply that single build by the number of
different Linux distributions and the number of different supported
releases of the Linux distribution and you quickly have a nightmare of
different build options on different releases.  Also realize that each
Linux distribution release contains a number of different kernels, all
tuned to different hardware types (different processor types and
different options), so for even a single release you will need to create
multiple versions of your module.
(上面的一部分问题可以通过以下方式解决：针对确切的特定内核配置、使用与构建内核时完全相同的 C 编译器来编译你的模块。如果你想为特定 Linux 发行版的特定发行版本提供一个模块，这就足够了。但把这个单一构建乘以不同 Linux 发行版的数量，再乘以该发行版受支持的不同版本的数量，你很快就会陷入不同版本上不同构建选项的噩梦。还要意识到，每个 Linux 发行版都包含多个不同的内核，分别针对不同的硬件类型(不同的处理器类型与不同的选项)进行了调优，因此哪怕只针对一个发行版本，你也需要创建模块的多个版本。)

Trust me, you will go insane over time if you try to support this kind
of release, I learned this the hard way a long time ago...
(相信我，如果你试图支持这种发布模式，时间久了你会疯掉的——很久以前我就用惨痛的教训学到了这一点……)


Stable Kernel Source Interfaces(稳定的内核源码接口)
---------------------------------------------------

This is a much more "volatile" topic if you talk to people who try to
keep a Linux kernel driver that is not in the main kernel tree up to
date over time.
(如果你与那些试图让不在主内核树中的 Linux 内核驱动长期保持更新的人交谈，这是一个更加"动荡"的话题。)

Linux kernel development is continuous and at a rapid pace, never
stopping to slow down.  As such, the kernel developers find bugs in
current interfaces, or figure out a better way to do things.  If they do
that, they then fix the current interfaces to work better.  When they do
so, function names may change, structures may grow or shrink, and
function parameters may be reworked.  If this happens, all of the
instances of where this interface is used within the kernel are fixed up
at the same time, ensuring that everything continues to work properly.
(Linux 内核的开发是持续且快速的，从不停止或放慢。因此，内核开发者会发现现有接口中的缺陷，或者找到更好的做事方式。如果他们这样做了，就会修正现有接口使其更好地工作。在这样做时，函数名可能改变，结构体可能增大或缩小，函数参数可能被重新设计。一旦发生这种情况，内核中所有使用该接口的地方都会同时被修正，以确保一切继续正常工作。)

As a specific examples of this, the in-kernel USB interfaces have
undergone at least three different reworks over the lifetime of this
subsystem.  These reworks were done to address a number of different
issues:
(作为具体例子，内核内部的 USB 接口在该子系统的生命周期中至少经历了三次不同的重构。这些重构是为了解决一系列不同的问题：)

  - A change from a synchronous model of data streams to an asynchronous
    one.  This reduced the complexity of a number of drivers and
    increased the throughput of all USB drivers such that we are now
    running almost all USB devices at their maximum speed possible.
    (把数据流的同步模型改为异步模型。这降低了若干驱动的复杂度，并提高了所有 USB 驱动的吞吐量，使我们现在几乎能以尽可能达到的最大速度运行所有 USB 设备。)
  - A change was made in the way data packets were allocated from the
    USB core by USB drivers so that all drivers now needed to provide
    more information to the USB core to fix a number of documented
    deadlocks.
    (USB 驱动从 USB 核心分配数据包的方式发生了变化，因此现在所有驱动都需要向 USB 核心提供更多信息，以修复若干已有记录的死锁。)

This is in stark contrast to a number of closed source operating systems
which have had to maintain their older USB interfaces over time.  This
provides the ability for new developers to accidentally use the old
interfaces and do things in improper ways, causing the stability of the
operating system to suffer.
(这与许多闭源操作系统形成了鲜明对比，后者不得不长期维护其旧版 USB 接口。这使得新开发者可能意外地使用旧接口、以不当的方式做事，导致操作系统的稳定性受损。)

In both of these instances, all developers agreed that these were
important changes that needed to be made, and they were made, with
relatively little pain.  If Linux had to ensure that it will preserve a
stable source interface, a new interface would have been created, and
the older, broken one would have had to be maintained over time, leading
to extra work for the USB developers.  Since all Linux USB developers do
their work on their own time, asking programmers to do extra work for no
gain, for free, is not a possibility.
(在这两个实例中，所有开发者都一致认为这些是必须进行的重要改动，而且这些改动也在相对较小的痛苦下完成了。如果 Linux 必须保证维持一个稳定的源码接口，那么就不得不创建一个新接口，同时还要长期维护旧的、有缺陷的接口，从而给 USB 开发者带来额外的工作。既然所有 Linux USB 开发者都是利用自己的业余时间做开发，要求程序员免费为毫无收益的额外工作付出劳动是不可能的。)

Security issues are also very important for Linux.  When a
security issue is found, it is fixed in a very short amount of time.  A
number of times this has caused internal kernel interfaces to be
reworked to prevent the security problem from occurring.  When this
happens, all drivers that use the interfaces were also fixed at the
same time, ensuring that the security problem was fixed and could not
come back at some future time accidentally.  If the internal interfaces
were not allowed to change, fixing this kind of security problem and
insuring that it could not happen again would not be possible.
(安全问题对 Linux 也非常重要。当发现安全问题时，会在极短的时间内修复它。有多次，这导致不得不重构内核内部接口，以防止安全问题再次发生。发生这种情况时，所有使用这些接口的驱动也会同时被修复，确保安全问题得到解决，并且不会在将来某个时候意外地卷土重来。如果不允许内部接口改变，就不可能修复这类安全问题并确保它不再发生。)

Kernel interfaces are cleaned up over time.  If there is no one using a
current interface, it is deleted.  This ensures that the kernel remains
as small as possible, and that all potential interfaces are tested as
well as they can be (unused interfaces are pretty much impossible to
test for validity.)
(内核接口会随着时间不断清理。如果当前没有人使用某个接口，它就会被删除。这确保了内核保持尽可能小，并且所有潜在存在的接口都得到了尽可能充分的测试(未使用的接口几乎不可能被有效测试)。)


What to do(该怎么做)
--------------------

So, if you have a Linux kernel driver that is not in the main kernel
tree, what are you, a developer, supposed to do?  Releasing a binary
driver for every different kernel version for every distribution is a
nightmare, and trying to keep up with an ever changing kernel interface
is also a rough job.
(那么，如果你有一个不在主内核树中的 Linux 内核驱动，作为开发者你该怎么办？为每个发行版的每个不同内核版本发布二进制驱动是一场噩梦，而试图跟上不断变化的内核接口同样是一件苦差事。)

Simple, get your kernel driver into the main kernel tree (remember we are
talking about drivers released under a GPL-compatible license here, if your
code doesn't fall under this category, good luck, you are on your own here,
you leech).  If your driver is in the tree, and a kernel interface changes,
it will be fixed up by the person who did the kernel change in the first
place.  This ensures that your driver is always buildable, and works over
time, with very little effort on your part.
(答案很简单：把你的内核驱动送进主内核树(记住，我们这里讨论的是以 GPL 兼容许可证发布的驱动；如果你的代码不属于这一类，那只能祝你好运、自求多福了，你这个吸血鬼)。如果你的驱动在树中，当某个内核接口发生变化时，做出该内核改动的人会顺手把你的驱动修正。这确保你的驱动始终可以构建、长期可用，而你自己几乎不费吹灰之力。)

The very good side effects of having your driver in the main kernel tree
are:
(把驱动放进主内核树还有以下非常好的附带效果：)

  - The quality of the driver will rise as the maintenance costs (to the
    original developer) will decrease.
    (驱动的质量会提升，因为(对原始开发者而言的)维护成本会下降。)
  - Other developers will add features to your driver.
    (其他开发者会为你的驱动添加特性。)
  - Other people will find and fix bugs in your driver.
    (其他人会找到并修复你的驱动中的缺陷。)
  - Other people will find tuning opportunities in your driver.
    (其他人会在你的驱动中找到调优的机会。)
  - Other people will update the driver for you when external interface
    changes require it.
    (当外部接口的变化需要时，其他人会替你更新驱动。)
  - The driver automatically gets shipped in all Linux distributions
    without having to ask the distros to add it.
    (驱动会自动随所有 Linux 发行版发布，无需请求发行版添加它。)

As Linux supports a larger number of different devices "out of the box"
than any other operating system, and it supports these devices on more
different processor architectures than any other operating system, this
proven type of development model must be doing something right :)
(由于 Linux "开箱即用"支持的不同设备数量比任何其他操作系统都多，并且它支持这些设备的处理器架构种类也比任何其他操作系统都多，这种经过验证的开发模式必定是做对了什么 :))



------

Thanks to Randy Dunlap, Andrew Morton, David Brownell, Hanna Linder,
Robert Love, and Nishanth Aravamudan for their review and comments on
early drafts of this paper.
(感谢 Randy Dunlap、Andrew Morton、David Brownell、Hanna Linder、Robert Love 与 Nishanth Aravamudan 对本文早期草稿的审阅与评论。)
