===================================================================================================================
Everything you never wanted to know about kobjects, ksets, and ktypes(关于 kobject、kset 与 ktype 你不想知道的一切)
===================================================================================================================

:Author: Greg Kroah-Hartman <gregkh@linuxfoundation.org>
         (作者: Greg Kroah-Hartman <gregkh@linuxfoundation.org>)
:Last updated: December 19, 2007
               (最后更新: 2007 年 12 月 19 日)

Based on an original article by Jon Corbet for lwn.net written October 1,
2003 and located at https://lwn.net/Articles/51437/
(本文基于 Jon Corbet 2003 年 10 月 1 日为 lwn.net 撰写的原文，位于 https://lwn.net/Articles/51437/ 。)

Part of the difficulty in understanding the driver model - and the kobject
abstraction upon which it is built - is that there is no obvious starting
place. Dealing with kobjects requires understanding a few different types,
all of which make reference to each other. In an attempt to make things
easier, we'll take a multi-pass approach, starting with vague terms and
adding detail as we go. To that end, here are some quick definitions of
some terms we will be working with.
(理解驱动模型——以及构建其上的 kobject 抽象——的部分困难在于：没有一个显而易见的切入点。与 kobject 打交道需要理解几种不同的类型，而它们又相互引用。为了简化问题，我们将采取多轮渐进的方式：先从模糊的术语开始，随行文逐步补充细节。为此，先给出我们即将使用的几个术语的简要定义。)

 - A kobject is an object of type struct kobject.  Kobjects have a name
   and a reference count.  A kobject also has a parent pointer (allowing
   objects to be arranged into hierarchies), a specific type, and,
   usually, a representation in the sysfs virtual filesystem.
   (kobject 是 struct kobject 类型的对象。kobject 有名字和引用计数。kobject 还有父指针(使对象可以组织成层次结构)、特定类型，通常还有在 sysfs 虚拟文件系统中的表示。)

   Kobjects are generally not interesting on their own; instead, they are
   usually embedded within some other structure which contains the stuff
   the code is really interested in.
   (kobject 单独存在时通常没什么意思；它们通常内嵌在其他结构中，而那个结构才包含代码真正关心的内容。)

   No structure should **EVER** have more than one kobject embedded within it.
   If it does, the reference counting for the object is sure to be messed
   up and incorrect, and your code will be buggy.  So do not do this.
   (任何结构都 **绝不** 应当内嵌多于一个 kobject。如果那样做，对象的引用计数肯定会变得混乱而不正确，你的代码也会出缺陷。所以不要这样做。)

 - A ktype is the type of object that embeds a kobject.  Every structure
   that embeds a kobject needs a corresponding ktype.  The ktype controls
   what happens to the kobject when it is created and destroyed.
   (ktype 是内嵌 kobject 的对象的类型。每个内嵌 kobject 的结构都需要一个对应的 ktype。ktype 控制 kobject 在创建与销毁时发生的事情。)

 - A kset is a group of kobjects.  These kobjects can be of the same ktype
   or belong to different ktypes.  The kset is the basic container type for
   collections of kobjects. Ksets contain their own kobjects, but you can
   safely ignore that implementation detail as the kset core code handles
   this kobject automatically.
   (kset 是一组 kobject。这些 kobject 可以是同一 ktype，也可以属于不同 ktype。kset 是 kobject 集合的基本容器类型。kset 包含自己的 kobject，但你可以安全地忽略这一实现细节，因为 kset 核心代码会自动处理这个 kobject。)

   When you see a sysfs directory full of other directories, generally each
   of those directories corresponds to a kobject in the same kset.
   (当你看到一个装满其他目录的 sysfs 目录时，一般而言，其中每个目录都对应同一个 kset 中的一个 kobject。)

We'll look at how to create and manipulate all of these types. A bottom-up
approach will be taken, so we'll go back to kobjects.
(我们将讨论如何创建与操作所有这些类型。我们将采用自底向上的方式，所以会回到 kobject。)


Embedding kobjects(嵌入 kobject)
================================

It is rare for kernel code to create a standalone kobject, with one major
exception explained below.  Instead, kobjects are used to control access to
a larger, domain-specific object.  To this end, kobjects will be found
embedded in other structures.  If you are used to thinking of things in
object-oriented terms, kobjects can be seen as a top-level, abstract class
from which other classes are derived.  A kobject implements a set of
capabilities which are not particularly useful by themselves, but are
nice to have in other objects.  The C language does not allow for the
direct expression of inheritance, so other techniques - such as structure
embedding - must be used.
(内核代码很少创建独立的 kobject——下文会解释的一个重大例外除外。相反，kobject 被用来控制对一个更大的、特定领域对象的访问。为此，kobject 会被内嵌在其他结构中。如果你习惯以面向对象的术语思考问题，可以把 kobject 看作一个顶层的抽象类，其他类由它派生。kobject 实现了一组能力，这些能力本身并不特别有用，但在其他对象中却是很好的配置。C 语言不允许直接表达继承，因此必须使用其他技术——比如结构体嵌入。)

(As an aside, for those familiar with the kernel linked list implementation,
this is analogous as to how "list_head" structs are rarely useful on
their own, but are invariably found embedded in the larger objects of
interest.)
(顺带一提，对于熟悉内核链表实现的人来说，这类似于 "list_head" 结构的情形：它们单独使用时很少有用，却总是出现在感兴趣的大对象中。)

So, for example, the UIO code in ``drivers/uio/uio.c`` has a structure that
defines the memory region associated with a uio device::

    struct uio_map {
            struct kobject kobj;
            struct uio_mem *mem;
    };

(例如，``drivers/uio/uio.c`` 中的 UIO 代码有一个结构，用来定义与 uio 设备关联的内存区域：)

If you have a struct uio_map structure, finding its embedded kobject is
just a matter of using the kobj member.  Code that works with kobjects will
often have the opposite problem, however: given a struct kobject pointer,
what is the pointer to the containing structure?  You must avoid tricks
(such as assuming that the kobject is at the beginning of the structure)
and, instead, use the container_of() macro, found in ``<linux/container_of.h>``::

    container_of(ptr, type, member)

(如果你手里有一个 struct uio_map 结构，找到其内嵌的 kobject 只是使用 kobj 成员的事。然而，操作 kobject 的代码常常遇到相反的问题：给定一个 struct kobject 指针，指向包含它的结构的指针是什么？你必须避免各种取巧的做法(比如假定 kobject 位于结构的开头)，而应使用 ``<linux/container_of.h>`` 中的 container_of() 宏：)

where:
(其中：)

  * ``ptr`` is the pointer to the embedded kobject,
    (``ptr`` 是指向内嵌 kobject 的指针，)
  * ``type`` is the type of the containing structure, and
    (``type`` 是包含结构的类型，)
  * ``member`` is the name of the structure field to which ``pointer`` points.
    (``member`` 是 ``pointer`` 所指向的结构字段的名字。)

The return value from container_of() is a pointer to the corresponding
container type. So, for example, a pointer ``kp`` to a struct kobject
embedded **within** a struct uio_map could be converted to a pointer to the
**containing** uio_map structure with::

    struct uio_map *u_map = container_of(kp, struct uio_map, kobj);

(container_of() 的返回值是指向相应容器类型的指针。例如，指向内嵌 **在** struct uio_map **之中** 的 struct kobject 的指针 ``kp`` ，可以用它转换为指向 **包含它的** uio_map 结构的指针：)

For convenience, programmers often define a simple macro for **back-casting**
kobject pointers to the containing type.  Exactly this happens in the
earlier ``drivers/uio/uio.c``, as you can see here::

    struct uio_map {
            struct kobject kobj;
            struct uio_mem *mem;
    };

    #define to_map(map) container_of(map, struct uio_map, kobj)

(为方便起见，程序员常常定义一个简单的宏，用于把 kobject 指针 **回溯** (back-cast)到包含它的类型。前面提到的 ``drivers/uio/uio.c`` 中正是这样做的，如下所示：)

where the macro argument "map" is a pointer to the struct kobject in
question.  That macro is subsequently invoked with::

    struct uio_map *map = to_map(kobj);

(其中，宏参数 "map" 是指向相关 struct kobject 的指针。该宏随后会被这样调用：)


Initialization of kobjects(kobject 的初始化)
============================================

Code which creates a kobject must, of course, initialize that object. Some
of the internal fields are setup with a (mandatory) call to kobject_init()::

    void kobject_init(struct kobject *kobj, const struct kobj_type *ktype);

(创建 kobject 的代码当然必须初始化该对象。通过(强制的)kobject_init() 调用来设置其中一些内部字段：)

The ktype is required for a kobject to be created properly, as every kobject
must have an associated kobj_type.  After calling kobject_init(), to
register the kobject with sysfs, the function kobject_add() must be called::

    int kobject_add(struct kobject *kobj, struct kobject *parent,
                    const char *fmt, ...);

(ktype 是正确创建 kobject 所必需的，因为每个 kobject 都必须有一个关联的 kobj_type。调用 kobject_init() 之后，要向 sysfs 注册该 kobject，必须调用 kobject_add() 函数：)

This sets up the parent of the kobject and the name for the kobject
properly.  If the kobject is to be associated with a specific kset,
kobj->kset must be assigned before calling kobject_add().  If a kset is
associated with a kobject, then the parent for the kobject can be set to
NULL in the call to kobject_add() and then the kobject's parent will be the
kset itself.
(这会正确设置 kobject 的父节点与名称。如果要把该 kobject 关联到特定的 kset，必须在调用 kobject_add() 之前为 kobj->kset 赋值。如果一个 kset 与 kobject 关联，那么在调用 kobject_add() 时可以把该 kobject 的 parent 设为 NULL，此时 kobject 的父节点就是这个 kset 本身。)

As the name of the kobject is set when it is added to the kernel, the name
of the kobject should never be manipulated directly.  If you must change
the name of the kobject, call kobject_rename()::

    int kobject_rename(struct kobject *kobj, const char *new_name);

(由于 kobject 的名称是在它加入内核时设置的，因此绝不应当直接操纵 kobject 的名称。如果必须更改 kobject 的名称，请调用 kobject_rename()：)

kobject_rename() does not perform any locking or have a solid notion of
what names are valid so the caller must provide their own sanity checking
and serialization.
(kobject_rename() 不执行任何加锁操作，对什么样的名称合法也没有可靠的概念，因此调用者必须自己提供健全性检查与串行化。)

There is a function called kobject_set_name() but that is legacy cruft and
is being removed.  If your code needs to call this function, it is
incorrect and needs to be fixed.
(有一个名为 kobject_set_name() 的函数，但那属于遗留的累赘，正在被移除。如果你的代码需要调用该函数，那么它是错误的，需要修正。)

To properly access the name of the kobject, use the function
kobject_name()::

    const char *kobject_name(const struct kobject * kobj);

(要正确访问 kobject 的名称，请使用 kobject_name() 函数：)

There is a helper function to both initialize and add the kobject to the
kernel at the same time, called surprisingly enough kobject_init_and_add()::

    int kobject_init_and_add(struct kobject *kobj, const struct kobj_type *ktype,
                             struct kobject *parent, const char *fmt, ...);

(有一个辅助函数可以同时初始化 kobject 并把它加入内核，名字相当直白，就叫 kobject_init_and_add()：)

The arguments are the same as the individual kobject_init() and
kobject_add() functions described above.
(其参数与上文描述的单独函数 kobject_init() 与 kobject_add() 相同。)


Uevents(用户事件)
=================

After a kobject has been registered with the kobject core, you need to
announce to the world that it has been created.  This can be done with a
call to kobject_uevent()::

    int kobject_uevent(struct kobject *kobj, enum kobject_action action);

(kobject 向 kobject 核心注册之后，你需要向外界宣布它已被创建。这可以通过调用 kobject_uevent() 完成：)

Use the **KOBJ_ADD** action for when the kobject is first added to the kernel.
This should be done only after any attributes or children of the kobject
have been initialized properly, as userspace will instantly start to look
for them when this call happens.
(当 kobject 第一次加入内核时，使用 **KOBJ_ADD** 动作。这应当只在该 kobject 的任何属性或子节点都正确初始化之后再做，因为该调用发生时用户空间会立刻开始寻找它们。)

When the kobject is removed from the kernel (details on how to do that are
below), the uevent for **KOBJ_REMOVE** will be automatically created by the
kobject core, so the caller does not have to worry about doing that by
hand.
(当 kobject 从内核中移除时(具体做法见下文)， **KOBJ_REMOVE** 的 uevent 会由 kobject 核心自动创建，因此调用者不必操心手动去做。)


Reference counts(引用计数)
==========================

One of the key functions of a kobject is to serve as a reference counter
for the object in which it is embedded. As long as references to the object
exist, the object (and the code which supports it) must continue to exist.
The low-level functions for manipulating a kobject's reference counts are::

    struct kobject *kobject_get(struct kobject *kobj);
    void kobject_put(struct kobject *kobj);

(kobject 的一个关键功能是充当它所内嵌对象的引用计数器。只要对该对象的引用还存在，对象(以及支持它的代码)就必须继续存在。操作 kobject 引用计数的底层函数是：)

A successful call to kobject_get() will increment the kobject's reference
counter and return the pointer to the kobject.
(成功调用 kobject_get() 会递增 kobject 的引用计数，并返回指向该 kobject 的指针。)

When a reference is released, the call to kobject_put() will decrement the
reference count and, possibly, free the object. Note that kobject_init()
sets the reference count to one, so the code which sets up the kobject will
need to do a kobject_put() eventually to release that reference.
(当引用被释放时，调用 kobject_put() 会递减引用计数，并可能释放对象。注意，kobject_init() 把引用计数设为 1，因此设置 kobject 的代码最终需要调用一次 kobject_put() 来释放该引用。)

Because kobjects are dynamic, they must not be declared statically or on
the stack, but instead, always allocated dynamically.  Future versions of
the kernel will contain a run-time check for kobjects that are created
statically and will warn the developer of this improper usage.
(由于 kobject 是动态的，它们不得声明为静态变量或栈上变量，而应始终动态分配。内核的未来版本将包含针对静态创建的 kobject 的运行时检查，并就这种不当用法向开发者发出警告。)

If all that you want to use a kobject for is to provide a reference counter
for your structure, please use the struct kref instead; a kobject would be
overkill.  For more information on how to use struct kref, please see the
file Documentation/core-api/kref.rst in the Linux kernel source tree.
(如果你使用 kobject 只是为了给结构提供一个引用计数，请改用 struct kref；用 kobject 就大材小用了。有关如何使用 struct kref 的更多信息，请参阅 Linux 内核源码树中的 Documentation/core-api/kref.rst 文件。)


Creating "simple" kobjects(创建 "简单" kobject)
===============================================

Sometimes all that a developer wants is a way to create a simple directory
in the sysfs hierarchy, and not have to mess with the whole complication of
ksets, show and store functions, and other details.  This is the one
exception where a single kobject should be created.  To create such an
entry, use the function::

    struct kobject *kobject_create_and_add(const char *name, struct kobject *parent);

(有时开发者只想要一种在 sysfs 层级中创建简单目录的办法，而不想与 kset、show 与 store 函数以及其他细节的全部复杂性打交道。这是唯一应该创建单个 kobject 的例外情形。要创建这样的条目，使用函数：)

This function will create a kobject and place it in sysfs in the location
underneath the specified parent kobject.  To create simple attributes
associated with this kobject, use::

    int sysfs_create_file(struct kobject *kobj, const struct attribute *attr);

(该函数会创建一个 kobject，并把它放到 sysfs 中指定父 kobject 之下的位置。要创建与该 kobject 关联的简单属性，使用：)

or::

    int sysfs_create_group(struct kobject *kobj, const struct attribute_group *grp);

(或：)

Both types of attributes used here, with a kobject that has been created
with the kobject_create_and_add(), can be of type kobj_attribute, so no
special custom attribute is needed to be created.
(对于用 kobject_create_and_add() 创建的 kobject，这里使用的两种类型的属性都可以是 kobj_attribute 类型，因此不需要创建特殊的自定义属性。)

See the example module, ``samples/kobject/kobject-example.c`` for an
implementation of a simple kobject and attributes.
(示例模块 ``samples/kobject/kobject-example.c`` 给出了一个简单 kobject 与属性的实现，可作参考。)



ktypes and release methods(ktype 与 release 方法)
=================================================

One important thing still missing from the discussion is what happens to a
kobject when its reference count reaches zero. The code which created the
kobject generally does not know when that will happen; if it did, there
would be little point in using a kobject in the first place. Even
predictable object lifecycles become more complicated when sysfs is brought
in as other portions of the kernel can get a reference on any kobject that
is registered in the system.
(讨论中还遗漏一件重要的事：当 kobject 的引用计数到达零时会发生什么。创建 kobject 的代码通常并不知道这会在何时发生；如果知道，那么一开始使用 kobject 就没什么意义了。当 sysfs 加入进来后，即使是可以预测的对象生命周期也会变得更复杂，因为内核的其他部分可以获得系统中任何已注册 kobject 的引用。)

The end result is that a structure protected by a kobject cannot be freed
before its reference count goes to zero. The reference count is not under
the direct control of the code which created the kobject. So that code must
be notified asynchronously whenever the last reference to one of its
kobjects goes away.
(最终结果是：受 kobject 保护的结构在引用计数归零之前不能被释放。引用计数并不受创建 kobject 的代码的直接控制。因此，只要其某个 kobject 的最后一个引用消失，就必须异步通知那段代码。)

Once you registered your kobject via kobject_add(), you must never use
kfree() to free it directly. The only safe way is to use kobject_put(). It
is good practice to always use kobject_put() after kobject_init() to avoid
errors creeping in.
(一旦通过 kobject_add() 注册了 kobject，就绝不能直接用 kfree() 释放它。唯一安全的方式是使用 kobject_put()。在 kobject_init() 之后始终配以 kobject_put() 是良好实践，可以避免错误悄悄潜入。)

This notification is done through a kobject's release() method. Usually
such a method has a form like::

    void my_object_release(struct kobject *kobj)
    {
            struct my_object *mine = container_of(kobj, struct my_object, kobj);

            /* Perform any additional cleanup on this object, then... */
            kfree(mine);
    }

(这一通知通过 kobject 的 release() 方法完成。此类方法通常形如：)

One important point cannot be overstated: every kobject must have a
release() method, and the kobject must persist (in a consistent state)
until that method is called. If these constraints are not met, the code is
flawed. Note that the kernel will warn you if you forget to provide a
release() method.  Do not try to get rid of this warning by providing an
"empty" release function.
(有一点怎么强调都不过分：每个 kobject 都必须有 release() 方法，而且 kobject 必须一直存在(处于一致状态)直到该方法被调用。如果不满足这些约束，代码就是有缺陷的。注意，如果忘记提供 release() 方法，内核会向你发出警告。不要试图通过提供一个 "空" 的 release 函数来摆脱这个警告。)

If all your cleanup function needs to do is call kfree(), then you must
create a wrapper function which uses container_of() to upcast to the correct
type (as shown in the example above) and then calls kfree() on the overall
structure.
(如果你的清理函数要做的只是调用 kfree()，那么必须创建一个包装函数：它使用 container_of() 向上转型到正确的类型(如上面的示例所示)，然后对整个结构调用 kfree()。)

Note, the name of the kobject is available in the release function, but it
must NOT be changed within this callback.  Otherwise there will be a memory
leak in the kobject core, which makes people unhappy.
(注意，在 release 函数中可以获得 kobject 的名称，但绝不能在该回调中更改它。否则 kobject 核心中会出现内存泄漏，这会让人不快。)

Interestingly, the release() method is not stored in the kobject itself;
instead, it is associated with the ktype. So let us introduce struct
kobj_type::

    struct kobj_type {
            void (*release)(struct kobject *kobj);
            const struct sysfs_ops *sysfs_ops;
            const struct attribute_group **default_groups;
            const struct kobj_ns_type_operations *(*child_ns_type)(struct kobject *kobj);
            const void *(*namespace)(struct kobject *kobj);
            void (*get_ownership)(struct kobject *kobj, kuid_t *uid, kgid_t *gid);
    };

(有趣的是，release() 方法并不存储在 kobject 自身之中；而是与 ktype 关联。那么让我们来介绍 struct kobj_type：)

This structure is used to describe a particular type of kobject (or, more
correctly, of containing object). Every kobject needs to have an associated
kobj_type structure; a pointer to that structure must be specified when you
call kobject_init() or kobject_init_and_add().
(该结构用于描述某一特定类型的 kobject(更准确地说，是包含它的对象)。每个 kobject 都需要有一个关联的 kobj_type 结构；调用 kobject_init() 或 kobject_init_and_add() 时必须指定指向该结构的指针。)

The release field in struct kobj_type is, of course, a pointer to the
release() method for this type of kobject. The other two fields (sysfs_ops
and default_groups) control how objects of this type are represented in
sysfs; they are beyond the scope of this document.
(struct kobj_type 中的 release 字段当然是指向这类 kobject 的 release() 方法的指针。其他两个字段(sysfs_ops 与 default_groups)控制这种类型的对象在 sysfs 中如何表示；它们超出了本文档的范围。)

The default_groups pointer is a list of default attributes that will be
automatically created for any kobject that is registered with this ktype.
(default_groups 指针是一个默认属性列表，任何以该 ktype 注册的 kobject 都会自动创建这些属性。)


ksets(kset 集合)
================

A kset is merely a collection of kobjects that want to be associated with
each other.  There is no restriction that they be of the same ktype, but be
very careful if they are not.
(kset 只是一组希望彼此关联的 kobject 的集合。它们并不受限于同一 ktype，但如果不是同一 ktype，就要非常小心。)

A kset serves these functions:
(kset 起到以下作用：)

 - It serves as a bag containing a group of objects. A kset can be used by
   the kernel to track "all block devices" or "all PCI device drivers."
   (它充当装有一组对象的袋子。内核可以用 kset 来跟踪 "所有块设备" 或 "所有 PCI 设备驱动"。)

 - A kset is also a subdirectory in sysfs, where the associated kobjects
   with the kset can show up.  Every kset contains a kobject which can be
   set up to be the parent of other kobjects; the top-level directories of
   the sysfs hierarchy are constructed in this way.
   (kset 也是 sysfs 中的一个子目录，与该 kset 关联的 kobject 可以在其中出现。每个 kset 都包含一个 kobject，它可以被设置为其他 kobject 的父节点；sysfs 层级的顶层目录就是这样构建的。)

 - Ksets can support the "hotplugging" of kobjects and influence how
   uevent events are reported to user space.
   (kset 可以支持 kobject 的 "热插拔"，并影响 uevent 事件如何上报给用户空间。)

In object-oriented terms, "kset" is the top-level container class; ksets
contain their own kobject, but that kobject is managed by the kset code and
should not be manipulated by any other user.
(用面向对象的术语来说，"kset" 是顶层的容器类；kset 包含自己的 kobject，但该 kobject 由 kset 代码管理，不应被任何其他使用者操纵。)

A kset keeps its children in a standard kernel linked list.  Kobjects point
back to their containing kset via their kset field. In almost all cases,
the kobjects belonging to a kset have that kset (or, strictly, its embedded
kobject) in their parent.
(kset 用标准的内核链表保存其子节点。kobject 通过自己的 kset 字段回指包含它的 kset。在几乎所有情况下，属于某个 kset 的 kobject 都把该 kset(严格地说，是其内嵌的 kobject)放在自己的 parent 中。)

As a kset contains a kobject within it, it should always be dynamically
created and never declared statically or on the stack.  To create a new
kset use::

  struct kset *kset_create_and_add(const char *name,
                                   const struct kset_uevent_ops *uevent_ops,
                                   struct kobject *parent_kobj);

(由于 kset 内部包含一个 kobject，它应当始终动态创建，绝不声明为静态变量或栈上变量。要创建新的 kset，使用：)

When you are finished with the kset, call::

  void kset_unregister(struct kset *k);

(用完 kset 之后，调用：)

to destroy it.  This removes the kset from sysfs and decrements its reference
count.  When the reference count goes to zero, the kset will be released.
Because other references to the kset may still exist, the release may happen
after kset_unregister() returns.
(来销毁它。这会把 kset 从 sysfs 中移除并递减其引用计数。当引用计数归零时，kset 会被释放。由于对 kset 的其他引用可能仍然存在，释放可能发生在 kset_unregister() 返回之后。)

An example of using a kset can be seen in the
``samples/kobject/kset-example.c`` file in the kernel tree.
(使用 kset 的示例可以在内核树的 ``samples/kobject/kset-example.c`` 文件中看到。)

If a kset wishes to control the uevent operations of the kobjects
associated with it, it can use the struct kset_uevent_ops to handle it::

  struct kset_uevent_ops {
          int (* const filter)(struct kobject *kobj);
          const char *(* const name)(struct kobject *kobj);
          int (* const uevent)(struct kobject *kobj, struct kobj_uevent_env *env);
  };

(如果 kset 想控制与其关联的 kobject 的 uevent 操作，它可以使用 struct kset_uevent_ops 来处理：)


The filter function allows a kset to prevent a uevent from being emitted to
userspace for a specific kobject.  If the function returns 0, the uevent
will not be emitted.
(filter 函数允许 kset 阻止针对特定 kobject 向用户空间发出 uevent。如果该函数返回 0，则不会发出 uevent。)

The name function will be called to override the default name of the kset
that the uevent sends to userspace.  By default, the name will be the same
as the kset itself, but this function, if present, can override that name.
(name 函数会被调用，用于覆盖 uevent 发送给用户空间时使用的 kset 默认名称。默认情况下，该名称与 kset 本身相同，但如果存在该函数，它可以覆盖这个名称。)

The uevent function will be called when the uevent is about to be sent to
userspace to allow more environment variables to be added to the uevent.
(uevent 函数会在 uevent 即将发送给用户空间时被调用，以便向 uevent 添加更多环境变量。)

One might ask how, exactly, a kobject is added to a kset, given that no
functions which perform that function have been presented.  The answer is
that this task is handled by kobject_add().  When a kobject is passed to
kobject_add(), its kset member should point to the kset to which the
kobject will belong.  kobject_add() will handle the rest.
(有人可能会问：既然前面没有介绍任何执行这项功能的函数，kobject 到底是怎样加入 kset 的？答案是这项任务由 kobject_add() 处理。把 kobject 传给 kobject_add() 时，其 kset 成员应当指向该 kobject 将要归属的 kset。剩下的由 kobject_add() 处理。)

If the kobject belonging to a kset has no parent kobject set, it will be
added to the kset's directory.  Not all members of a kset do necessarily
live in the kset directory.  If an explicit parent kobject is assigned
before the kobject is added, the kobject is registered with the kset, but
added below the parent kobject.
(如果属于某个 kset 的 kobject 没有设置父 kobject，它会被加入该 kset 的目录。并非 kset 的所有成员都必定位于 kset 目录中。如果在添加 kobject 之前显式指定了父 kobject，则该 kobject 会注册到 kset，但被添加在父 kobject 之下。)


Kobject removal(kobject 的移除)
===============================

After a kobject has been registered with the kobject core successfully, it
must be cleaned up when the code is finished with it.  To do that, call
kobject_put().  By doing this, the kobject core will automatically clean up
all of the memory allocated by this kobject.  If a ``KOBJ_ADD`` uevent has been
sent for the object, a corresponding ``KOBJ_REMOVE`` uevent will be sent, and
any other sysfs housekeeping will be handled for the caller properly.
(kobject 向 kobject 核心成功注册之后，当代码不再使用它时必须加以清理。为此调用 kobject_put()。这样一来，kobject 核心会自动清理该 kobject 分配的所有内存。如果已经为该对象发送过 ``KOBJ_ADD`` uevent ，则会发送对应的 ``KOBJ_REMOVE`` uevent ，其余的 sysfs 收尾工作也会替调用者妥善处理。)

If you need to do a two-stage delete of the kobject (say you are not
allowed to sleep when you need to destroy the object), then call
kobject_del() which will unregister the kobject from sysfs.  This makes the
kobject "invisible", but it is not cleaned up, and the reference count of
the object is still the same.  At a later time call kobject_put() to finish
the cleanup of the memory associated with the kobject.
(如果你需要对 kobject 进行两阶段删除(比如说你需要销毁对象时不允许睡眠)，那么调用 kobject_del()，它会把 kobject 从 sysfs 注销。这使 kobject "不可见"，但它并未被清理，对象的引用计数也保持不变。稍后再调用 kobject_put()，完成与该 kobject 关联的内存的清理。)

kobject_del() can be used to drop the reference to the parent object, if
circular references are constructed.  It is valid in some cases, that a
parent objects references a child.  Circular references _must_ be broken
with an explicit call to kobject_del(), so that a release functions will be
called, and the objects in the former circle release each other.
(kobject_del() 可用于丢弃对父对象的引用(如果构建了循环引用的话)。在某些情况下，父对象引用子对象是合法的。循环引用必须通过显式调用 kobject_del() 来打破，这样 release 函数才会被调用，原循环中的对象才能彼此释放。)


Example code to copy from(可复制的示例代码)
===========================================

For a more complete example of using ksets and kobjects properly, see the
example programs ``samples/kobject/{kobject-example.c,kset-example.c}``,
which will be built as loadable modules if you select ``CONFIG_SAMPLE_KOBJECT``.
(有关正确使用 kset 与 kobject 的更完整示例，请参见示例程序 ``samples/kobject/{kobject-example.c,kset-example.c}`` ；如果选择了 ``CONFIG_SAMPLE_KOBJECT`` ，它们会被构建为可加载模块。)
