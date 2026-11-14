.. SPDX-License-Identifier: GPL-2.0

===============
Shared Subtrees
===============

(共享子树)

.. Contents:
	1) Overview
	2) Features
	3) Setting mount states
	4) Use-case
	5) Detailed semantics
	6) Quiz
	7) FAQ
	8) Implementation


1) Overview
(1) 概述)
-----------

Consider the following situation:
(考虑以下情况：)

A process wants to clone its own namespace, but still wants to access the CD
that got mounted recently.  Shared subtree semantics provide the necessary
mechanism to accomplish the above.
(一个进程想要克隆自己的命名空间，但仍想访问最近挂载的 CD。共享子树语义提供了完成上述目标所需的机制。)

It provides the necessary building blocks for features like per-user-namespace
and versioned filesystem.
(它为诸如 per-user-namespace(每用户命名空间)和 versioned filesystem(版本化文件系统)之类的特性提供了必要的构建块。)

2) Features
(2) 特性)
-----------

Shared subtree provides four different flavors of mounts; struct vfsmount to be
precise:
(共享子树提供了四种不同类型的挂载；准确地说是 struct vfsmount：)


a) A **shared mount** can be replicated to as many mountpoints and all the
   replicas continue to be exactly same.
   (**共享挂载(shared mount)** 可以被复制到任意多个挂载点，并且所有副本保持完全相同。)

   Here is an example:
   (下面是一个例子：)

   Let's say /mnt has a mount that is shared::
   (假设 /mnt 有一个共享的挂载：)

     # mount --make-shared /mnt

   .. note::
      mount(8) command now supports the --make-shared flag,
      so the sample 'smount' program is no longer needed and has been
      removed.
      (mount(8) 命令现在支持 --make-shared 标志，因此示例 'smount' 程序不再需要，已被移除。)

   ::

     # mount --bind /mnt /tmp

   The above command replicates the mount at /mnt to the mountpoint /tmp
   and the contents of both the mounts remain identical.
   (上面的命令把 /mnt 处的挂载复制到挂载点 /tmp，并且两个挂载的内容保持一致。)

   ::

     #ls /mnt
     a b c

     #ls /tmp
     a b c

   Now let's say we mount a device at /tmp/a::
   (现在假设我们在 /tmp/a 挂载一个设备：)

     # mount /dev/sd0  /tmp/a

     # ls /tmp/a
     t1 t2 t3

     # ls /mnt/a
     t1 t2 t3

   Note that the mount has propagated to the mount at /mnt as well.
   (注意，该挂载已经传播到了 /mnt 处的挂载。)

   And the same is true even when /dev/sd0 is mounted on /mnt/a. The
   contents will be visible under /tmp/a too.
   (即使 /dev/sd0 被挂载到 /mnt/a 上也是如此。其内容在 /tmp/a 下同样可见。)


b) A **slave mount** is like a shared mount except that mount and umount events
   only propagate towards it.
   (**slave 挂载(slave mount)** 类似于共享挂载，只是挂载和卸载事件只会朝它传播。)

   All slave mounts have a master mount which is a shared.
   (所有 slave 挂载都有一个作为共享挂载的 master 挂载。)

   Here is an example:
   (下面是一个例子：)

   Let's say /mnt has a mount which is shared::
   (假设 /mnt 有一个共享的挂载：)

     # mount --make-shared /mnt

   Let's bind mount /mnt to /tmp::
   (让我们把 /mnt bind 挂载到 /tmp：)

     # mount --bind /mnt /tmp

   the new mount at /tmp becomes a shared mount and it is a replica of
   the mount at /mnt.
   (/tmp 处的新挂载成为共享挂载，并且它是 /mnt 处挂载的一个副本。)

   Now let's make the mount at /tmp; a slave of /mnt::
   (现在让我们把 /tmp 处的挂载变成 /mnt 的 slave：)

     # mount --make-slave /tmp

   let's mount /dev/sd0 on /mnt/a::
   (让我们把 /dev/sd0 挂载到 /mnt/a 上：)

     # mount /dev/sd0 /mnt/a

     # ls /mnt/a
     t1 t2 t3

     # ls /tmp/a
     t1 t2 t3

   Note the mount event has propagated to the mount at /tmp
   (注意，挂载事件已经传播到了 /tmp 处的挂载)

   However let's see what happens if we mount something on the mount at
   /tmp::
   (不过让我们看看，如果在 /tmp 处的挂载上再挂载东西会发生什么：)

     # mount /dev/sd1 /tmp/b

     # ls /tmp/b
     s1 s2 s3

     # ls /mnt/b

   Note how the mount event has not propagated to the mount at
   /mnt
   (注意，挂载事件没有传播到 /mnt 处的挂载)


c) A **private mount** does not forward or receive propagation.
   (**私有挂载(private mount)** 不转发也不接收传播。)

   This is the mount we are familiar with. Its the default type.
   (这就是我们所熟悉的挂载。它是默认类型。)


d) An **unbindable mount** is, as the name suggests, an unbindable private
   mount.
   (**不可绑定挂载(unbindable mount)** 顾名思义，是一种不可绑定的私有挂载。)

   let's say we have a mount at /mnt and we make it unbindable::
   (假设我们在 /mnt 处有一个挂载，我们把它设为不可绑定：)

     # mount --make-unbindable /mnt

   Let's try to bind mount this mount somewhere else::
   (让我们尝试把这个挂载 bind 挂载到别处：)

     # mount --bind /mnt /tmp mount: wrong fs type, bad option, bad
     superblock on /mnt, or too many mounted file systems

   Binding a unbindable mount is a invalid operation.
   (绑定一个不可绑定挂载是无效操作。)


3) Setting mount states
(3) 设置挂载状态)
-----------------------

The mount command (util-linux package) can be used to set mount
states::
(mount 命令(util-linux 软件包)可用于设置挂载状态：)

    mount --make-shared mountpoint
    mount --make-slave mountpoint
    mount --make-private mountpoint
    mount --make-unbindable mountpoint


4) Use cases
(4) 用例)
------------

A) A process wants to clone its own namespace, but still wants to
   access the CD that got mounted recently.
   (一个进程想要克隆自己的命名空间，但仍想访问最近挂载的 CD。)

   Solution:
   (解决方案：)

   The system administrator can make the mount at /cdrom shared::
   (系统管理员可以把 /cdrom 处的挂载设为共享的：)

     mount --bind /cdrom /cdrom
     mount --make-shared /cdrom

   Now any process that clones off a new namespace will have a
   mount at /cdrom which is a replica of the same mount in the
   parent namespace.
   (现在，任何克隆出新命名空间的进程在 /cdrom 处都会有一个挂载，它是父命名空间中同一挂载的副本。)

   So when a CD is inserted and mounted at /cdrom that mount gets
   propagated to the other mount at /cdrom in all the other clone
   namespaces.
   (因此，当插入 CD 并挂载到 /cdrom 时，该挂载会传播到所有其他克隆命名空间中 /cdrom 处的另一个挂载。)

B) A process wants its mounts invisible to any other process, but
   still be able to see the other system mounts.
   (一个进程想要让自己的挂载对任何其他进程不可见，但仍能看到其他的系统挂载。)

   Solution:
   (解决方案：)

   To begin with, the administrator can mark the entire mount tree
   as shareable::
   (首先，管理员可以把整个挂载树标记为可共享的：)

     mount --make-rshared /

   A new process can clone off a new namespace. And mark some part
   of its namespace as slave::
   (新进程可以克隆出一个新命名空间。并把其命名空间的某一部分标记为 slave：)

     mount --make-rslave /myprivatetree

   Hence forth any mounts within the /myprivatetree done by the
   process will not show up in any other namespace. However mounts
   done in the parent namespace under /myprivatetree still shows
   up in the process's namespace.
   (从此以后，该进程在 /myprivatetree 内所做的任何挂载都不会出现在任何其他命名空间中。但在父命名空间中 /myprivatetree 下所做的挂载仍会显示在该进程的命名空间中。)


Apart from the above semantics this feature provides the
building blocks to solve the following problems:
(除了上述语义之外，该特性还提供了解决以下问题的构建块：)

C)  Per-user namespace
    (每用户命名空间(per-user namespace))

    The above semantics allows a way to share mounts across
    namespaces.  But namespaces are associated with processes. If
    namespaces are made first class objects with user API to
    associate/disassociate a namespace with userid, then each user
    could have his/her own namespace and tailor it to his/her
    requirements. This needs to be supported in PAM.
    (上述语义提供了一种跨命名空间共享挂载的方法。但命名空间是与进程相关联的。如果把命名空间做成一等对象，并提供用户 API 来将命名空间与用户 id 关联/解除关联，那么每个用户都可以拥有自己的命名空间，并根据自己的需求进行定制。这需要在 PAM 中得到支持。)

D)  Versioned files
    (版本化文件(versioned files))

    If the entire mount tree is visible at multiple locations, then
    an underlying versioning file system can return different
    versions of the file depending on the path used to access that
    file.
    (如果整个挂载树在多个位置可见，那么底层的版本化文件系统可以根据访问该文件所用的路径返回该文件的不同版本。)

    An example is::
    (一个例子是：)

       mount --make-shared /
       mount --rbind / /view/v1
       mount --rbind / /view/v2
       mount --rbind / /view/v3
       mount --rbind / /view/v4

    and if /usr has a versioning filesystem mounted, then that
    mount appears at /view/v1/usr, /view/v2/usr, /view/v3/usr and
    /view/v4/usr too
    (而如果 /usr 上挂载了一个版本化文件系统，那么该挂载也会出现在 /view/v1/usr、/view/v2/usr、/view/v3/usr 和 /view/v4/usr 处)

    A user can request v3 version of the file /usr/fs/namespace.c
    by accessing /view/v3/usr/fs/namespace.c . The underlying
    versioning filesystem can then decipher that v3 version of the
    filesystem is being requested and return the corresponding
    inode.
    (用户可以通过访问 /view/v3/usr/fs/namespace.c 来请求文件 /usr/fs/namespace.c 的 v3 版本。底层的版本化文件系统随后可以辨别出正在请求的是文件系统的 v3 版本，并返回相应的 inode。)

5) Detailed semantics
(5) 详细语义)
---------------------
The section below explains the detailed semantics of
bind, rbind, move, mount, umount and clone-namespace operations.
(下面这一节解释 bind、rbind、move、mount、umount 和 clone-namespace 操作的详细语义。)

.. Note::
   the word 'vfsmount' and the noun 'mount' have been used
   to mean the same thing, throughout this document.
   (在本文档中，单词 'vfsmount' 和名词 'mount' 自始至终被用来表示同一事物。)

a) Mount states
(挂载状态)

   A **propagation event** is defined as event generated on a vfsmount
   that leads to mount or unmount actions in other vfsmounts.
   (**传播事件(propagation event)** 定义为在一个 vfsmount 上产生的、导致在其他 vfsmount 中执行挂载或卸载动作的事件。)

   A **peer group** is defined as a group of vfsmounts that propagate
   events to each other.
   (**对等组(peer group)** 定义为一组相互传播事件的 vfsmount。)

   A given mount can be in one of the following states:
   (一个给定的挂载可以处于以下状态之一：)

   (1) Shared mounts
   (共享挂载)

       A **shared mount** is defined as a vfsmount that belongs to a
       peer group.
       (**共享挂载** 定义为属于某个对等组的 vfsmount。)

       For example::
       (例如：)

         mount --make-shared /mnt
         mount --bind /mnt /tmp

       The mount at /mnt and that at /tmp are both shared and belong
       to the same peer group. Anything mounted or unmounted under
       /mnt or /tmp reflect in all the other mounts of its peer
       group.
       (/mnt 处的挂载和 /tmp 处的挂载都是共享的，并且属于同一个对等组。在 /mnt 或 /tmp 下挂载或卸载任何东西都会反映到其对等组的所有其他挂载中。)


   (2) Slave mounts
   (slave 挂载)

       A **slave mount** is defined as a vfsmount that receives
       propagation events and does not forward propagation events.
       (**slave 挂载** 定义为接收传播事件但不转发传播事件的 vfsmount。)

       A slave mount as the name implies has a master mount from which
       mount/unmount events are received. Events do not propagate from
       the slave mount to the master.  Only a shared mount can be made
       a slave by executing the following command::
       (顾名思义，slave 挂载有一个 master 挂载，挂载/卸载事件从它那里接收。事件不会从 slave 挂载传播到 master。只有共享挂载可以通过执行以下命令被设为 slave：)

         mount --make-slave mount

       A shared mount that is made as a slave is no more shared unless
       modified to become shared.
       (被设为 slave 的共享挂载不再是共享的，除非又把它改回共享。)

   (3) Shared and Slave
   (共享且 slave)

       A vfsmount can be both **shared** as well as **slave**.  This state
       indicates that the mount is a slave of some vfsmount, and
       has its own peer group too.  This vfsmount receives propagation
       events from its master vfsmount, and also forwards propagation
       events to its 'peer group' and to its slave vfsmounts.
       (一个 vfsmount 可以既是**共享**的又是 **slave**。这种状态表示该挂载是某个 vfsmount 的 slave，并且也有自己的对等组。这个 vfsmount 从其 master vfsmount 接收传播事件，同时也把传播事件转发给它的"对等组"以及它的 slave vfsmount。)

       Strictly speaking, the vfsmount is shared having its own
       peer group, and this peer-group is a slave of some other
       peer group.
       (严格来说，该 vfsmount 是共享的、拥有自己的对等组，而这个对等组是某个其他对等组的 slave。)

       Only a slave vfsmount can be made as 'shared and slave' by
       either executing the following command::
       (只有 slave vfsmount 可以被设为"共享且 slave"，方法是执行以下命令：)

         mount --make-shared mount

       or by moving the slave vfsmount under a shared vfsmount.
       (或者通过把该 slave vfsmount 移动到一个共享 vfsmount 之下。)

   (4) Private mount
   (私有挂载)

       A **private mount** is defined as vfsmount that does not
       receive or forward any propagation events.
       (**私有挂载** 定义为不接收也不转发任何传播事件的 vfsmount。)

   (5) Unbindable mount
   (不可绑定挂载)

       A **unbindable mount** is defined as vfsmount that does not
       receive or forward any propagation events and cannot
       be bind mounted.
       (**不可绑定挂载** 定义为不接收也不转发任何传播事件、并且不能被 bind 挂载的 vfsmount。)


       State diagram:
       (状态图：)

       The state diagram below explains the state transition of a mount,
       in response to various commands::

            -----------------------------------------------------------------------
            |             |make-shared |  make-slave  | make-private |make-unbindab|
            --------------|------------|--------------|--------------|-------------|
            |shared       |shared      |*slave/private|   private    | unbindable  |
            |             |            |              |              |             |
            |-------------|------------|--------------|--------------|-------------|
            |slave        |shared      | **slave      |    private   | unbindable  |
            |             |and slave   |              |              |             |
            |-------------|------------|--------------|--------------|-------------|
            |shared       |shared      | slave        |    private   | unbindable  |
            |and slave    |and slave   |              |              |             |
            |-------------|------------|--------------|--------------|-------------|
            |private      |shared      |  **private   |    private   | unbindable  |
            |-------------|------------|--------------|--------------|-------------|
            |unbindable   |shared      |**unbindable  |    private   | unbindable  |
            ------------------------------------------------------------------------

       (下面的状态图解释了一个挂载响应各种命令时的状态转换：)

            * if the shared mount is the only mount in its peer group, making it
            slave, makes it private automatically. Note that there is no master to
            which it can be slaved to.

            ** slaving a non-shared mount has no effect on the mount.

       Apart from the commands listed below, the 'move' operation also changes
       the state of a mount depending on type of the destination mount. Its
       explained in section 5d.
       (除了下面列出的命令之外，'move' 操作也会根据目标挂载的类型改变挂载的状态。这在 5d 节中解释。)

b) Bind semantics
(bind 语义)

   Consider the following command::
   (考虑以下命令：)

     mount --bind A/a  B/b

   where 'A' is the source mount, 'a' is the dentry in the mount 'A', 'B'
   is the destination mount and 'b' is the dentry in the destination mount.
   (其中 'A' 是源挂载，'a' 是挂载 'A' 中的 dentry，'B' 是目标挂载，'b' 是目标挂载中的 dentry。)

   The outcome depends on the type of mount of 'A' and 'B'. The table
   below contains quick reference::

            --------------------------------------------------------------------------
            |         BIND MOUNT OPERATION                                           |
            |************************************************************************|
            |source(A)->| shared      |       private  |       slave    | unbindable |
            | dest(B)  |              |                |                |            |
            |   |      |              |                |                |            |
            |   v      |              |                |                |            |
            |************************************************************************|
            |  shared  | shared       |     shared     | shared & slave |  invalid   |
            |          |              |                |                |            |
            |non-shared| shared       |      private   |      slave     |  invalid   |
            **************************************************************************

   (结果取决于 'A' 和 'B' 的挂载类型。下表包含快速参考：)

   Details:
   (细节：)

   1. 'A' is a shared mount and 'B' is a shared mount. A new mount 'C'
      which is clone of 'A', is created. Its root dentry is 'a' . 'C' is
      mounted on mount 'B' at dentry 'b'. Also new mount 'C1', 'C2', 'C3' ...
      are created and mounted at the dentry 'b' on all mounts where 'B'
      propagates to. A new propagation tree containing 'C1',..,'Cn' is
      created. This propagation tree is identical to the propagation tree of
      'B'.  And finally the peer-group of 'C' is merged with the peer group
      of 'A'.
      ('A' 是共享挂载，'B' 是共享挂载。会创建一个作为 'A' 克隆的新挂载 'C'，其根 dentry 为 'a'。'C' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，在 'B' 传播到的所有挂载上，也会创建新挂载 'C1'、'C2'、'C3' …… 并挂载到 dentry 'b' 上。会创建一个包含 'C1'、..、'Cn' 的新传播树。该传播树与 'B' 的传播树相同。最后，'C' 的对等组与 'A' 的对等组合并。)

   2. 'A' is a private mount and 'B' is a shared mount. A new mount 'C'
      which is clone of 'A', is created. Its root dentry is 'a'. 'C' is
      mounted on mount 'B' at dentry 'b'. Also new mount 'C1', 'C2', 'C3' ...
      are created and mounted at the dentry 'b' on all mounts where 'B'
      propagates to. A new propagation tree is set containing all new mounts
      'C', 'C1', .., 'Cn' with exactly the same configuration as the
      propagation tree for 'B'.
      ('A' 是私有挂载，'B' 是共享挂载。会创建一个作为 'A' 克隆的新挂载 'C'，其根 dentry 为 'a'。'C' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，在 'B' 传播到的所有挂载上，也会创建新挂载 'C1'、'C2'、'C3' …… 并挂载到 dentry 'b' 上。会把所有新挂载 'C'、'C1'、..、'Cn' 设置为一个新的传播树，其配置与 'B' 的传播树完全相同。)

   3. 'A' is a slave mount of mount 'Z' and 'B' is a shared mount. A new
      mount 'C' which is clone of 'A', is created. Its root dentry is 'a' .
      'C' is mounted on mount 'B' at dentry 'b'. Also new mounts 'C1', 'C2',
      'C3' ... are created and mounted at the dentry 'b' on all mounts where
      'B' propagates to. A new propagation tree containing the new mounts
      'C','C1',..  'Cn' is created. This propagation tree is identical to the
      propagation tree for 'B'. And finally the mount 'C' and its peer group
      is made the slave of mount 'Z'.  In other words, mount 'C' is in the
      state 'slave and shared'.
      ('A' 是挂载 'Z' 的 slave 挂载，'B' 是共享挂载。会创建一个作为 'A' 克隆的新挂载 'C'，其根 dentry 为 'a'。'C' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，在 'B' 传播到的所有挂载上，也会创建新挂载 'C1'、'C2'、'C3' …… 并挂载到 dentry 'b' 上。会创建一个包含新挂载 'C'、'C1'、..、'Cn' 的新传播树。该传播树与 'B' 的传播树相同。最后，挂载 'C' 及其对等组成为挂载 'Z' 的 slave。换句话说，挂载 'C' 处于"slave 且共享"状态。)

   4. 'A' is a unbindable mount and 'B' is a shared mount. This is a
      invalid operation.
      ('A' 是不可绑定挂载，'B' 是共享挂载。这是无效操作。)

   5. 'A' is a private mount and 'B' is a non-shared(private or slave or
      unbindable) mount. A new mount 'C' which is clone of 'A', is created.
      Its root dentry is 'a'. 'C' is mounted on mount 'B' at dentry 'b'.
      ('A' 是私有挂载，'B' 是非共享(私有、slave 或不可绑定)挂载。会创建一个作为 'A' 克隆的新挂载 'C'，其根 dentry 为 'a'。'C' 被挂载到挂载 'B' 的 dentry 'b' 上。)

   6. 'A' is a shared mount and 'B' is a non-shared mount. A new mount 'C'
      which is a clone of 'A' is created. Its root dentry is 'a'. 'C' is
      mounted on mount 'B' at dentry 'b'.  'C' is made a member of the
      peer-group of 'A'.
      ('A' 是共享挂载，'B' 是非共享挂载。会创建一个作为 'A' 克隆的新挂载 'C'，其根 dentry 为 'a'。'C' 被挂载到挂载 'B' 的 dentry 'b' 上。'C' 被设为 'A' 的对等组的成员。)

   7. 'A' is a slave mount of mount 'Z' and 'B' is a non-shared mount. A
      new mount 'C' which is a clone of 'A' is created. Its root dentry is
      'a'.  'C' is mounted on mount 'B' at dentry 'b'. Also 'C' is set as a
      slave mount of 'Z'. In other words 'A' and 'C' are both slave mounts of
      'Z'.  All mount/unmount events on 'Z' propagates to 'A' and 'C'. But
      mount/unmount on 'A' do not propagate anywhere else. Similarly
      mount/unmount on 'C' do not propagate anywhere else.
      ('A' 是挂载 'Z' 的 slave 挂载，'B' 是非共享挂载。会创建一个作为 'A' 克隆的新挂载 'C'，其根 dentry 为 'a'。'C' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，'C' 被设为 'Z' 的 slave 挂载。换句话说，'A' 和 'C' 都是 'Z' 的 slave 挂载。'Z' 上的所有挂载/卸载事件都会传播到 'A' 和 'C'。但 'A' 上的挂载/卸载不会传播到其他任何地方。类似地，'C' 上的挂载/卸载也不会传播到其他任何地方。)

   8. 'A' is a unbindable mount and 'B' is a non-shared mount. This is a
      invalid operation. A unbindable mount cannot be bind mounted.
      ('A' 是不可绑定挂载，'B' 是非共享挂载。这是无效操作。不可绑定挂载不能被 bind 挂载。)

c) Rbind semantics
(rbind 语义)

   rbind is same as bind. Bind replicates the specified mount.  Rbind
   replicates all the mounts in the tree belonging to the specified mount.
   Rbind mount is bind mount applied to all the mounts in the tree.
   (rbind 与 bind 相同。bind 复制指定的挂载。rbind 复制属于指定挂载的树中的所有挂载。rbind 挂载就是应用到树中所有挂载上的 bind 挂载。)

   If the source tree that is rbind has some unbindable mounts,
   then the subtree under the unbindable mount is pruned in the new
   location.
   (如果被 rbind 的源树中有一些不可绑定挂载，那么该不可绑定挂载之下的子树会在新位置被剪除。)

   eg:
   (例如：)

   let's say we have the following mount tree::
   (假设我们有如下挂载树：)

                A
              /   \
              B   C
             / \ / \
             D E F G

   Let's say all the mount except the mount C in the tree are
   of a type other than unbindable.
   (假设树中除挂载 C 之外的所有挂载都是不可绑定以外的类型。)

   If this tree is rbound to say Z
   (如果把这棵树 rbind 到比如 Z 上)

   We will have the following tree at the new location::

                Z
                |
                A'
               /
              B'                Note how the tree under C is pruned
             / \                in the new location.
            D' E'

   (我们将在新位置得到如下树：)

d) Move semantics
(move 语义)

   Consider the following command::
   (考虑以下命令：)

     mount --move A  B/b

   where 'A' is the source mount, 'B' is the destination mount and 'b' is
   the dentry in the destination mount.
   (其中 'A' 是源挂载，'B' 是目标挂载，'b' 是目标挂载中的 dentry。)

   The outcome depends on the type of the mount of 'A' and 'B'. The table
   below is a quick reference::

            ---------------------------------------------------------------------------
            |                   MOVE MOUNT OPERATION                                 |
            |**************************************************************************
            | source(A)->| shared      |       private  |       slave    | unbindable |
            | dest(B)  |               |                |                |            |
            |   |      |               |                |                |            |
            |   v      |               |                |                |            |
            |**************************************************************************
            |  shared  | shared        |     shared     |shared and slave|  invalid   |
            |          |               |                |                |            |
            |non-shared| shared        |      private   |    slave       | unbindable |
            ***************************************************************************

   (结果取决于 'A' 和 'B' 的挂载类型。下表是快速参考：)

   .. Note:: moving a mount residing under a shared mount is invalid.
   (移动一个位于共享挂载之下的挂载是无效的。)

   Details follow:
   (细节如下：)

   1. 'A' is a shared mount and 'B' is a shared mount.  The mount 'A' is
      mounted on mount 'B' at dentry 'b'.  Also new mounts 'A1', 'A2'...'An'
      are created and mounted at dentry 'b' on all mounts that receive
      propagation from mount 'B'. A new propagation tree is created in the
      exact same configuration as that of 'B'. This new propagation tree
      contains all the new mounts 'A1', 'A2'...  'An'.  And this new
      propagation tree is appended to the already existing propagation tree
      of 'A'.
      ('A' 是共享挂载，'B' 是共享挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，在所有从挂载 'B' 接收传播的挂载上，也会创建新挂载 'A1'、'A2'……'An' 并挂载到 dentry 'b' 上。会创建一个与 'B' 的配置完全相同的新传播树。这个新传播树包含所有新挂载 'A1'、'A2'……'An'。并且这个新传播树被追加到 'A' 已存在的传播树上。)

   2. 'A' is a private mount and 'B' is a shared mount. The mount 'A' is
      mounted on mount 'B' at dentry 'b'. Also new mount 'A1', 'A2'... 'An'
      are created and mounted at dentry 'b' on all mounts that receive
      propagation from mount 'B'. The mount 'A' becomes a shared mount and a
      propagation tree is created which is identical to that of
      'B'. This new propagation tree contains all the new mounts 'A1',
      'A2'...  'An'.
      ('A' 是私有挂载，'B' 是共享挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，在所有从挂载 'B' 接收传播的挂载上，也会创建新挂载 'A1'、'A2'……'An' 并挂载到 dentry 'b' 上。挂载 'A' 变成共享挂载，并会创建一个与 'B' 的传播树相同的传播树。这个新传播树包含所有新挂载 'A1'、'A2'……'An'。)

   3. 'A' is a slave mount of mount 'Z' and 'B' is a shared mount.  The
      mount 'A' is mounted on mount 'B' at dentry 'b'.  Also new mounts 'A1',
      'A2'... 'An' are created and mounted at dentry 'b' on all mounts that
      receive propagation from mount 'B'. A new propagation tree is created
      in the exact same configuration as that of 'B'. This new propagation
      tree contains all the new mounts 'A1', 'A2'...  'An'.  And this new
      propagation tree is appended to the already existing propagation tree of
      'A'.  Mount 'A' continues to be the slave mount of 'Z' but it also
      becomes 'shared'.
      ('A' 是挂载 'Z' 的 slave 挂载，'B' 是共享挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。同时，在所有从挂载 'B' 接收传播的挂载上，也会创建新挂载 'A1'、'A2'……'An' 并挂载到 dentry 'b' 上。会创建一个与 'B' 的配置完全相同的新传播树。这个新传播树包含所有新挂载 'A1'、'A2'……'An'。并且这个新传播树被追加到 'A' 已存在的传播树上。挂载 'A' 继续是 'Z' 的 slave 挂载，但它也变成了"共享"。)

   4. 'A' is a unbindable mount and 'B' is a shared mount. The operation
      is invalid. Because mounting anything on the shared mount 'B' can
      create new mounts that get mounted on the mounts that receive
      propagation from 'B'.  And since the mount 'A' is unbindable, cloning
      it to mount at other mountpoints is not possible.
      ('A' 是不可绑定挂载，'B' 是共享挂载。该操作无效。因为把任何东西挂载到共享挂载 'B' 上，会在从 'B' 接收传播的挂载上创建并挂载新挂载。而由于挂载 'A' 是不可绑定的，无法把它克隆并挂载到其他挂载点上。)

   5. 'A' is a private mount and 'B' is a non-shared(private or slave or
      unbindable) mount. The mount 'A' is mounted on mount 'B' at dentry 'b'.
      ('A' 是私有挂载，'B' 是非共享(私有、slave 或不可绑定)挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。)

   6. 'A' is a shared mount and 'B' is a non-shared mount.  The mount 'A'
      is mounted on mount 'B' at dentry 'b'.  Mount 'A' continues to be a
      shared mount.
      ('A' 是共享挂载，'B' 是非共享挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。挂载 'A' 继续是共享挂载。)

   7. 'A' is a slave mount of mount 'Z' and 'B' is a non-shared mount.
      The mount 'A' is mounted on mount 'B' at dentry 'b'.  Mount 'A'
      continues to be a slave mount of mount 'Z'.
      ('A' 是挂载 'Z' 的 slave 挂载，'B' 是非共享挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。挂载 'A' 继续是挂载 'Z' 的 slave 挂载。)

   8. 'A' is a unbindable mount and 'B' is a non-shared mount. The mount
      'A' is mounted on mount 'B' at dentry 'b'. Mount 'A' continues to be a
      unbindable mount.
      ('A' 是不可绑定挂载，'B' 是非共享挂载。挂载 'A' 被挂载到挂载 'B' 的 dentry 'b' 上。挂载 'A' 继续是不可绑定挂载。)

e) Mount semantics
(mount 语义)

   Consider the following command::
   (考虑以下命令：)

     mount device  B/b

   'B' is the destination mount and 'b' is the dentry in the destination
   mount.
   ('B' 是目标挂载，'b' 是目标挂载中的 dentry。)

   The above operation is the same as bind operation with the exception
   that the source mount is always a private mount.
   (上述操作与 bind 操作相同，唯一的例外是源挂载总是私有挂载。)


f) Unmount semantics
(卸载语义)

   Consider the following command::
   (考虑以下命令：)

     umount A

   where 'A' is a mount mounted on mount 'B' at dentry 'b'.
   (其中 'A' 是挂载到挂载 'B' 的 dentry 'b' 上的一个挂载。)

   If mount 'B' is shared, then all most-recently-mounted mounts at dentry
   'b' on mounts that receive propagation from mount 'B' and does not have
   sub-mounts within them are unmounted.
   (如果挂载 'B' 是共享的，那么在从挂载 'B' 接收传播的挂载上、位于 dentry 'b' 处的所有最近挂载且内部没有子挂载的挂载都会被卸载。)

   Example: Let's say 'B1', 'B2', 'B3' are shared mounts that propagate to
   each other.
   (例子：假设 'B1'、'B2'、'B3' 是相互传播的共享挂载。)

   let's say 'A1', 'A2', 'A3' are first mounted at dentry 'b' on mount
   'B1', 'B2' and 'B3' respectively.
   (假设 'A1'、'A2'、'A3' 先分别被挂载到挂载 'B1'、'B2'、'B3' 的 dentry 'b' 上。)

   let's say 'C1', 'C2', 'C3' are next mounted at the same dentry 'b' on
   mount 'B1', 'B2' and 'B3' respectively.
   (假设 'C1'、'C2'、'C3' 接着被挂载到挂载 'B1'、'B2'、'B3' 的同一 dentry 'b' 上。)

   if 'C1' is unmounted, all the mounts that are most-recently-mounted on
   'B1' and on the mounts that 'B1' propagates-to are unmounted.
   (如果 'C1' 被卸载，那么最近挂载在 'B1' 上以及 'B1' 传播到的挂载上的所有挂载都会被卸载。)

   'B1' propagates to 'B2' and 'B3'. And the most recently mounted mount
   on 'B2' at dentry 'b' is 'C2', and that of mount 'B3' is 'C3'.
   ('B1' 传播到 'B2' 和 'B3'。'B2' 上位于 dentry 'b' 的最近挂载的挂载是 'C2'，挂载 'B3' 上的是 'C3'。)

   So all 'C1', 'C2' and 'C3' should be unmounted.
   (所以 'C1'、'C2' 和 'C3' 都应该被卸载。)

   If any of 'C2' or 'C3' has some child mounts, then that mount is not
   unmounted, but all other mounts are unmounted. However if 'C1' is told
   to be unmounted and 'C1' has some sub-mounts, the umount operation is
   failed entirely.
   (如果 'C2' 或 'C3' 中任何一个有一些子挂载，那么该挂载不会被卸载，但其他所有挂载都会被卸载。然而，如果被告知卸载的 'C1' 有一些子挂载，那么卸载操作将完全失败。)

g) Clone Namespace
(克隆命名空间)

   A cloned namespace contains all the mounts as that of the parent
   namespace.
   (克隆出的命名空间包含与父命名空间相同的所有挂载。)

   Let's say 'A' and 'B' are the corresponding mounts in the parent and the
   child namespace.
   (假设 'A' 和 'B' 是父命名空间和子命名空间中对应的挂载。)

   If 'A' is shared, then 'B' is also shared and 'A' and 'B' propagate to
   each other.
   (如果 'A' 是共享的，那么 'B' 也是共享的，并且 'A' 和 'B' 相互传播。)

   If 'A' is a slave mount of 'Z', then 'B' is also the slave mount of
   'Z'.
   (如果 'A' 是 'Z' 的 slave 挂载，那么 'B' 也是 'Z' 的 slave 挂载。)

   If 'A' is a private mount, then 'B' is a private mount too.
   (如果 'A' 是私有挂载，那么 'B' 也是私有挂载。)

   If 'A' is unbindable mount, then 'B' is a unbindable mount too.
   (如果 'A' 是不可绑定挂载，那么 'B' 也是不可绑定挂载。)


6) Quiz
(6) 测验)
-------

A. What is the result of the following command sequence?
   (以下命令序列的结果是什么？)

   ::

       mount --bind /mnt /mnt
       mount --make-shared /mnt
       mount --bind /mnt /tmp
       mount --move /tmp /mnt/1

   what should be the contents of /mnt /mnt/1 /mnt/1/1 should be?
   Should they all be identical? or should /mnt and /mnt/1 be
   identical only?
   (/mnt、/mnt/1、/mnt/1/1 的内容应该是什么？它们应该全部相同吗？还是只有 /mnt 和 /mnt/1 相同？)


B. What is the result of the following command sequence?
   (以下命令序列的结果是什么？)

   ::

       mount --make-rshared /
       mkdir -p /v/1
       mount --rbind / /v/1

   what should be the content of /v/1/v/1 be?
   (/v/1/v/1 的内容应该是什么？)


C. What is the result of the following command sequence?
   (以下命令序列的结果是什么？)

   ::

       mount --bind /mnt /mnt
       mount --make-shared /mnt
       mkdir -p /mnt/1/2/3 /mnt/1/test
       mount --bind /mnt/1 /tmp
       mount --make-slave /mnt
       mount --make-shared /mnt
       mount --bind /mnt/1/2 /tmp1
       mount --make-slave /mnt

   At this point we have the first mount at /tmp and
   its root dentry is 1. Let's call this mount 'A'
   And then we have a second mount at /tmp1 with root
   dentry 2. Let's call this mount 'B'
   Next we have a third mount at /mnt with root dentry
   mnt. Let's call this mount 'C'
   (此时我们在 /tmp 处有第一个挂载，其根 dentry 是 1。我们把这个挂载称为 'A'。然后在 /tmp1 处有第二个挂载，根 dentry 为 2。我们把这个挂载称为 'B'。接着我们在 /mnt 处有第三个挂载，根 dentry 为 mnt。我们把这个挂载称为 'C'。)

   'B' is the slave of 'A' and 'C' is a slave of 'B'
   A -> B -> C
   ('B' 是 'A' 的 slave，'C' 是 'B' 的 slave。A -> B -> C)

   at this point if we execute the following command::
   (此时如果我们执行以下命令：)

     mount --bind /bin /tmp/test

   The mount is attempted on 'A'
   (该挂载尝试在 'A' 上进行)

   will the mount propagate to 'B' and 'C' ?
   (该挂载会传播到 'B' 和 'C' 吗？)

   what would be the contents of
   /mnt/1/test be?
   (/mnt/1/test 的内容会是什么？)

7) FAQ
(7) 常见问题)
------

1. Why is bind mount needed? How is it different from symbolic links?
   (为什么需要 bind 挂载？它与符号链接有何不同？)

   symbolic links can get stale if the destination mount gets
   unmounted or moved. Bind mounts continue to exist even if the
   other mount is unmounted or moved.
   (如果目标挂载被卸载或移动，符号链接可能会失效。即使另一个挂载被卸载或移动，bind 挂载仍然继续存在。)

2. Why can't the shared subtree be implemented using exportfs?
   (为什么不能用 exportfs 来实现共享子树？)

   exportfs is a heavyweight way of accomplishing part of what
   shared subtree can do. I cannot imagine a way to implement the
   semantics of slave mount using exportfs?
   (exportfs 是完成共享子树所能做的一部分工作的一种重量级方式。我无法想象如何用 exportfs 实现 slave 挂载的语义？)

3. Why is unbindable mount needed?
   (为什么需要不可绑定挂载？)

   Let's say we want to replicate the mount tree at multiple
   locations within the same subtree.
   (假设我们想在同一子树内的多个位置复制挂载树。)

   if one rbind mounts a tree within the same subtree 'n' times
   the number of mounts created is an exponential function of 'n'.
   Having unbindable mount can help prune the unneeded bind
   mounts. Here is an example.
   (如果在同一子树内把一棵树 rbind 挂载 'n' 次，创建的挂载数量将是 'n' 的指数函数。拥有不可绑定挂载有助于剪除不需要的 bind 挂载。下面是一个例子。)

   step 1:
   (步骤 1：)
      let's say the root tree has just two directories with
      one vfsmount::
      (假设根树只有两个目录和一个 vfsmount：)

                                    root
                                   /    \
                                  tmp    usr

      And we want to replicate the tree at multiple
      mountpoints under /root/tmp
      (而我们想在 /root/tmp 下的多个挂载点复制这棵树)

   step 2:
   (步骤 2：)
      ::


                        mount --make-shared /root

                        mkdir -p /tmp/m1

                        mount --rbind /root /tmp/m1

      the new tree now looks like this::
      (现在的新树看起来像这样：)

                                    root
                                   /    \
                                 tmp    usr
                                /
                               m1
                              /  \
                             tmp  usr
                             /
                            m1

      it has two vfsmounts
      (它有两个 vfsmount)

   step 3:
   (步骤 3：)
      ::

                            mkdir -p /tmp/m2
                            mount --rbind /root /tmp/m2

      the new tree now looks like this::
      (现在的新树看起来像这样：)

                                      root
                                     /    \
                                   tmp     usr
                                  /    \
                                m1       m2
                               / \       /  \
                             tmp  usr   tmp  usr
                             / \          /
                            m1  m2      m1
                                / \     /  \
                              tmp usr  tmp   usr
                              /        / \
                             m1       m1  m2
                            /  \
                          tmp   usr
                          /  \
                         m1   m2

                    it has 6 vfsmounts

   step 4:
   (步骤 4：)
      ::

                          mkdir -p /tmp/m3
                          mount --rbind /root /tmp/m3

      I won't draw the tree..but it has 24 vfsmounts
      (我就不画这棵树了……但它有 24 个 vfsmount)


   at step i the number of vfsmounts is V[i] = i*V[i-1].
   This is an exponential function. And this tree has way more
   mounts than what we really needed in the first place.
   (在第 i 步，vfsmount 的数量是 V[i] = i*V[i-1]。这是一个指数函数。而这棵树所拥有的挂载远超我们最初真正需要的数量。)

   One could use a series of umount at each step to prune
   out the unneeded mounts. But there is a better solution.
   Unclonable mounts come in handy here.
   (可以在每一步使用一系列 umount 来剪除不需要的挂载。但有更好的解决方案。不可克隆挂载(unclonable mounts)在这里就派上用场了。)

   step 1:
   (步骤 1：)
      let's say the root tree has just two directories with
      one vfsmount::
      (假设根树只有两个目录和一个 vfsmount：)

                                    root
                                   /    \
                                  tmp    usr

         How do we set up the same tree at multiple locations under
         /root/tmp
         (我们如何在 /root/tmp 下的多个位置设置同样的树)

   step 2:
   (步骤 2：)
      ::


                        mount --bind /root/tmp /root/tmp

                        mount --make-rshared /root
                        mount --make-unbindable /root/tmp

                        mkdir -p /tmp/m1

                        mount --rbind /root /tmp/m1

      the new tree now looks like this::
      (现在的新树看起来像这样：)

                                    root
                                   /    \
                                 tmp    usr
                                /
                               m1
                              /  \
                             tmp  usr

   step 3:
   (步骤 3：)
      ::

                            mkdir -p /tmp/m2
                            mount --rbind /root /tmp/m2

      the new tree now looks like this::
      (现在的新树看起来像这样：)

                                    root
                                   /    \
                                 tmp    usr
                                /   \
                               m1     m2
                              /  \     / \
                             tmp  usr tmp usr

   step 4:
   (步骤 4：)
      ::

                            mkdir -p /tmp/m3
                            mount --rbind /root /tmp/m3

      the new tree now looks like this::
      (现在的新树看起来像这样：)

                                          root
                                      /           \
                                     tmp           usr
                                 /    \    \
                               m1     m2     m3
                              /  \     / \    /  \
                             tmp  usr tmp usr tmp usr

8) Implementation
(8) 实现)
-----------------

A) Datastructure
(数据结构)

   Several new fields are introduced to struct vfsmount:
   (为 struct vfsmount 引入了几个新字段：)

   ->mnt_share
           Links together all the mount to/from which this vfsmount
           send/receives propagation events.
           (把所有与本 vfsmount 相互发送/接收传播事件的挂载链接在一起。)

   ->mnt_slave_list
           Links all the mounts to which this vfsmount propagates
           to.
           (链接本 vfsmount 传播到的所有挂载。)

   ->mnt_slave
           Links together all the slaves that its master vfsmount
           propagates to.
           (把其 master vfsmount 传播到的所有 slave 链接在一起。)

   ->mnt_master
           Points to the master vfsmount from which this vfsmount
           receives propagation.
           (指向本 vfsmount 从其接收传播的 master vfsmount。)

   ->mnt_flags
           Takes two more flags to indicate the propagation status of
           the vfsmount.  MNT_SHARE indicates that the vfsmount is a shared
           vfsmount.  MNT_UNCLONABLE indicates that the vfsmount cannot be
           replicated.
           (增加了两个标志来指示 vfsmount 的传播状态。MNT_SHARE 表示该 vfsmount 是共享 vfsmount。MNT_UNCLONABLE 表示该 vfsmount 不能被复制。)

   All the shared vfsmounts in a peer group form a cyclic list through
   ->mnt_share.
   (对等组中的所有共享 vfsmount 通过 ->mnt_share 形成一个循环链表。)

   All vfsmounts with the same ->mnt_master form on a cyclic list anchored
   in ->mnt_master->mnt_slave_list and going through ->mnt_slave.
   (具有相同 ->mnt_master 的所有 vfsmount 形成一个循环链表，该链表锚定于 ->mnt_master->mnt_slave_list 并经过 ->mnt_slave。)

   ->mnt_master can point to arbitrary (and possibly different) members
   of master peer group.  To find all immediate slaves of a peer group
   you need to go through _all_ ->mnt_slave_list of its members.
   Conceptually it's just a single set - distribution among the
   individual lists does not affect propagation or the way propagation
   tree is modified by operations.
   (->mnt_master 可以指向 master 对等组中任意(且可能不同的)成员。要找到一个对等组的所有直接 slave，需要遍历其成员的 _所有_ ->mnt_slave_list。从概念上讲它只是一个集合——在各链表之间的分布不影响传播，也不影响传播树被操作修改的方式。)

   All vfsmounts in a peer group have the same ->mnt_master.  If it is
   non-NULL, they form a contiguous (ordered) segment of slave list.
   (对等组中的所有 vfsmount 都有相同的 ->mnt_master。如果它非 NULL，它们会构成 slave 链表中一个连续的(有序的)段。)

   A example propagation tree looks as shown in the figure below.
   (一个示例传播树如下图所示。)

   .. note::
      Though it looks like a forest, if we consider all the shared
      mounts as a conceptual entity called 'pnode', it becomes a tree.
      (虽然它看起来像一片森林，但如果我们把所有共享挂载视为一个叫做 'pnode' 的概念实体，它就变成了一棵树。)

   ::


                        A <--> B <--> C <---> D
                       /|\            /|      |\
                      / F G          J K      H I
                     /
                    E<-->K
                        /|\
                       M L N

   In the above figure  A,B,C and D all are shared and propagate to each
   other.   'A' has got 3 slave mounts 'E' 'F' and 'G' 'C' has got 2 slave
   mounts 'J' and 'K'  and  'D' has got two slave mounts 'H' and 'I'.
   'E' is also shared with 'K' and they propagate to each other.  And
   'K' has 3 slaves 'M', 'L' and 'N'
   (在上图中，A、B、C 和 D 都是共享的并且相互传播。'A' 有 3 个 slave 挂载 'E'、'F' 和 'G'，'C' 有 2 个 slave 挂载 'J' 和 'K'，而 'D' 有两个 slave 挂载 'H' 和 'I'。'E' 与 'K' 也是共享的，并且它们相互传播。而 'K' 有 3 个 slave：'M'、'L' 和 'N')

   A's ->mnt_share links with the ->mnt_share of 'B' 'C' and 'D'
   (A 的 ->mnt_share 与 'B'、'C' 和 'D' 的 ->mnt_share 相链接)

   A's ->mnt_slave_list links with ->mnt_slave of 'E', 'K', 'F' and 'G'
   (A 的 ->mnt_slave_list 与 'E'、'K'、'F' 和 'G' 的 ->mnt_slave 相链接)

   E's ->mnt_share links with ->mnt_share of K
   (E 的 ->mnt_share 与 K 的 ->mnt_share 相链接)

   'E', 'K', 'F', 'G' have their ->mnt_master point to struct vfsmount of 'A'
   ('E'、'K'、'F'、'G' 的 ->mnt_master 指向 'A' 的 struct vfsmount)

   'M', 'L', 'N' have their ->mnt_master point to struct vfsmount of 'K'
   ('M'、'L'、'N' 的 ->mnt_master 指向 'K' 的 struct vfsmount)

   K's ->mnt_slave_list links with ->mnt_slave of 'M', 'L' and 'N'
   (K 的 ->mnt_slave_list 与 'M'、'L' 和 'N' 的 ->mnt_slave 相链接)

   C's ->mnt_slave_list links with ->mnt_slave of 'J' and 'K'
   (C 的 ->mnt_slave_list 与 'J' 和 'K' 的 ->mnt_slave 相链接)

   J and K's ->mnt_master points to struct vfsmount of C
   (J 和 K 的 ->mnt_master 指向 C 的 struct vfsmount)

   and finally D's ->mnt_slave_list links with ->mnt_slave of 'H' and 'I'
   (最后，D 的 ->mnt_slave_list 与 'H' 和 'I' 的 ->mnt_slave 相链接)

   'H' and 'I' have their ->mnt_master pointing to struct vfsmount of 'D'.
   ('H' 和 'I' 的 ->mnt_master 指向 'D' 的 struct vfsmount。)


   NOTE: The propagation tree is orthogonal to the mount tree.
   (注意：传播树与挂载树是相互正交的。)

B) Locking:
(加锁：)

   ->mnt_share, ->mnt_slave, ->mnt_slave_list, ->mnt_master are protected
   by namespace_sem (exclusive for modifications, shared for reading).
   (->mnt_share、->mnt_slave、->mnt_slave_list、->mnt_master 由 namespace_sem 保护(修改时独占，读取时共享)。)

   Normally we have ->mnt_flags modifications serialized by vfsmount_lock.
   There are two exceptions: do_add_mount() and clone_mnt().
   The former modifies a vfsmount that has not been visible in any shared
   data structures yet.
   The latter holds namespace_sem and the only references to vfsmount
   are in lists that can't be traversed without namespace_sem.
   (通常，->mnt_flags 的修改由 vfsmount_lock 串行化。有两个例外：do_add_mount() 和 clone_mnt()。前者修改的 vfsmount 尚未在任何共享数据结构中可见。后者持有 namespace_sem，并且对 vfsmount 的唯一引用位于不持有 namespace_sem 就无法遍历的链表中。)

C) Algorithm:
(算法：)

   The crux of the implementation resides in rbind/move operation.
   (实现的关键在于 rbind/move 操作。)

   The overall algorithm breaks the operation into 3 phases: (look at
   attach_recursive_mnt() and propagate_mnt())
   (整体算法把该操作分解为 3 个阶段：(参见 attach_recursive_mnt() 和 propagate_mnt()))

   1. Prepare phase.
      (准备阶段。)

      For each mount in the source tree:
      (对于源树中的每个挂载：)

      a) Create the necessary number of mount trees to
         be attached to each of the mounts that receive
         propagation from the destination mount.
         (创建必要数量的挂载树，以便附加到从目标挂载接收传播的每个挂载上。)
      b) Do not attach any of the trees to its destination.
         However note down its ->mnt_parent and ->mnt_mountpoint
         (不要把任何一棵树附加到其目标。但要记下其 ->mnt_parent 和 ->mnt_mountpoint)
      c) Link all the new mounts to form a propagation tree that
         is identical to the propagation tree of the destination
         mount.
         (把所有新挂载链接起来，形成一个与目标挂载的传播树相同的传播树。)

      If this phase is successful, there should be 'n' new
      propagation trees; where 'n' is the number of mounts in the
      source tree.  Go to the commit phase
      (如果这个阶段成功，应该有 'n' 棵新传播树；其中 'n' 是源树中挂载的数量。转到提交阶段)

      Also there should be 'm' new mount trees, where 'm' is
      the number of mounts to which the destination mount
      propagates to.
      (另外应该有 'm' 棵新挂载树，其中 'm' 是目标挂载传播到的挂载数量。)

      If any memory allocations fail, go to the abort phase.
      (如果任何内存分配失败，转到中止阶段。)

   2. Commit phase.
      (提交阶段。)

      Attach each of the mount trees to their corresponding
      destination mounts.
      (把每棵挂载树附加到其对应的目标挂载上。)

   3. Abort phase.
      (中止阶段。)

      Delete all the newly created trees.
      (删除所有新创建的树。)

   .. Note::
      all the propagation related functionality resides in the file pnode.c
      (所有与传播相关的功能都位于文件 pnode.c 中)


------------------------------------------------------------------------

version 0.1  (created the initial document, Ram Pai linuxram@us.ibm.com)
(version 0.1  (创建了初始文档，Ram Pai linuxram@us.ibm.com))

version 0.2  (Incorporated comments from Al Viro)
(version 0.2  (吸收了 Al Viro 的意见))
