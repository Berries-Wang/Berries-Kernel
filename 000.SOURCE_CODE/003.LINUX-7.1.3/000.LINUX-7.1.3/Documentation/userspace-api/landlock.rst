.. SPDX-License-Identifier: GPL-2.0
.. Copyright © 2017-2020 Mickaël Salaün <mic@digikod.net>
.. Copyright © 2019-2020 ANSSI
.. Copyright © 2021-2022 Microsoft Corporation

==============================================================
Landlock: unprivileged access control(Landlock 非特权访问控制)
==============================================================

:Author: Mickaël Salaün
(作者: Mickaël Salaün)
:Date: March 2026
(日期: 2026年3月)

The goal of Landlock is to enable restriction of ambient rights (e.g. global
filesystem or network access) for a set of processes.  Because Landlock
is a stackable LSM, it makes it possible to create safe security sandboxes as
new security layers in addition to the existing system-wide access-controls.
This kind of sandbox is expected to help mitigate the security impact of bugs or
unexpected/malicious behaviors in user space applications.  Landlock empowers
any process, including unprivileged ones, to securely restrict themselves.
(Landlock 的目标是为一组进程提供对周边权限 (如全局文件系统或网络访问) 的限制能力。由于 Landlock 是一个可堆叠的 LSM，它可以在现有系统级访问控制之外创建新的安全层，从而构建安全的安全沙箱。这种沙箱有望帮助缓解用户空间应用程序中缺陷或意外/恶意行为的安全影响。Landlock 让任何进程(包括非特权进程)都能安全地限制自身。)

We can quickly make sure that Landlock is enabled in the running system by
looking for "landlock: Up and running" in kernel logs (as root):
``dmesg | grep landlock || journalctl -kb -g landlock`` .
Developers can also easily check for Landlock support with a
:ref:`related system call <landlock_abi_versions>`.
If Landlock is not currently supported, we need to
:ref:`configure the kernel appropriately <kernel_support>`.
(通过在内核日志中查找 "landlock: Up and running" (以 root 身份) 可以快速确认运行中的系统已启用 Landlock： ``dmesg | grep landlock || journalctl -kb -g landlock`` 。开发者还可以通过 :ref:`related system call <landlock_abi_versions>` 轻松检查是否支持 Landlock。如果当前系统不支持 Landlock，则需要 :ref:`configure the kernel appropriately <kernel_support>` 。)

Landlock rules(Landlock 规则)
=============================

A Landlock rule describes an action on an object which the process intends to
perform.  A set of rules is aggregated in a ruleset, which can then restrict
the thread enforcing it, and its future children.
(Landlock 规则描述进程打算对某个对象执行的一个操作。一组规则聚合为一个 ruleset，随后它可以限制实施它的线程及其未来的子进程。)

The two existing types of rules are:
(现有的两类规则是：)

Filesystem rules
(文件系统规则)

    For these rules, the object is a file hierarchy,
    and the related filesystem actions are defined with
    `filesystem access rights`.
    (对于这类规则，对象是一个文件层次结构，相关的文件系统操作由 `filesystem access rights`(文件系统访问权限) 定义。)

Network rules (since ABI v4)
(网络规则 (自 ABI v4 起))

    For these rules, the object is a TCP port,
    and the related actions are defined with `network access rights`.
    (对于这类规则，对象是一个 TCP 端口，相关操作由 `network access rights`(网络访问权限) 定义。)

Defining and enforcing a security policy(定义并实施安全策略)
------------------------------------------------------------

We first need to define the ruleset that will contain our rules.
(我们首先需要定义将容纳我们规则的 ruleset。)

For this example, the ruleset will contain rules that only allow filesystem
read actions and establish a specific TCP connection. Filesystem write
actions and other TCP actions will be denied.
(在本例中，ruleset 将包含只允许文件系统读操作和建立特定 TCP 连接的规则。文件系统写操作和其他 TCP 操作将被拒绝。)

The ruleset then needs to handle both these kinds of actions.  This is
required for backward and forward compatibility (i.e. the kernel and user
space may not know each other's supported restrictions), hence the need
to be explicit about the denied-by-default access rights.
(随后，ruleset 需要处理这两类操作。这是向后与向前兼容所必需的 (即内核与用户空间可能彼此不了解对方支持的限制)，因此需要显式声明默认拒绝的访问权限。)

.. code-block:: c

    struct landlock_ruleset_attr ruleset_attr = {
        .handled_access_fs =
            LANDLOCK_ACCESS_FS_EXECUTE |
            LANDLOCK_ACCESS_FS_WRITE_FILE |
            LANDLOCK_ACCESS_FS_READ_FILE |
            LANDLOCK_ACCESS_FS_READ_DIR |
            LANDLOCK_ACCESS_FS_REMOVE_DIR |
            LANDLOCK_ACCESS_FS_REMOVE_FILE |
            LANDLOCK_ACCESS_FS_MAKE_CHAR |
            LANDLOCK_ACCESS_FS_MAKE_DIR |
            LANDLOCK_ACCESS_FS_MAKE_REG |
            LANDLOCK_ACCESS_FS_MAKE_SOCK |
            LANDLOCK_ACCESS_FS_MAKE_FIFO |
            LANDLOCK_ACCESS_FS_MAKE_BLOCK |
            LANDLOCK_ACCESS_FS_MAKE_SYM |
            LANDLOCK_ACCESS_FS_REFER |
            LANDLOCK_ACCESS_FS_TRUNCATE |
            LANDLOCK_ACCESS_FS_IOCTL_DEV |
            LANDLOCK_ACCESS_FS_RESOLVE_UNIX,
        .handled_access_net =
            LANDLOCK_ACCESS_NET_BIND_TCP |
            LANDLOCK_ACCESS_NET_CONNECT_TCP,
        .scoped =
            LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET |
            LANDLOCK_SCOPE_SIGNAL,
    };

Because we may not know which kernel version an application will be executed
on, it is safer to follow a best-effort security approach.  Indeed, we
should try to protect users as much as possible whatever the kernel they are
using.
(由于我们可能不知道应用程序将在哪个内核版本上执行，因此遵循 best-effort(尽力而为) 的安全方式更为稳妥。事实上，无论用户使用什么内核，我们都应尽可能保护他们。)

To be compatible with older Linux versions, we detect the available Landlock ABI
version, and only use the available subset of access rights:
(为了兼容较旧的 Linux 版本，我们检测可用的 Landlock ABI 版本，并只使用其中可用的访问权限子集：)

.. code-block:: c

    int abi;

    abi = landlock_create_ruleset(NULL, 0, LANDLOCK_CREATE_RULESET_VERSION);
    if (abi < 0) {
        /* Degrades gracefully if Landlock is not handled. */
        perror("The running kernel does not enable to use Landlock");
        return 0;
    }
    switch (abi) {
    case 1:
        /* Removes LANDLOCK_ACCESS_FS_REFER for ABI < 2 */
        ruleset_attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_REFER;
        __attribute__((fallthrough));
    case 2:
        /* Removes LANDLOCK_ACCESS_FS_TRUNCATE for ABI < 3 */
        ruleset_attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_TRUNCATE;
        __attribute__((fallthrough));
    case 3:
        /* Removes network support for ABI < 4 */
        ruleset_attr.handled_access_net &=
            ~(LANDLOCK_ACCESS_NET_BIND_TCP |
              LANDLOCK_ACCESS_NET_CONNECT_TCP);
        __attribute__((fallthrough));
    case 4:
        /* Removes LANDLOCK_ACCESS_FS_IOCTL_DEV for ABI < 5 */
        ruleset_attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_IOCTL_DEV;
        __attribute__((fallthrough));
    case 5:
        /* Removes LANDLOCK_SCOPE_* for ABI < 6 */
        ruleset_attr.scoped &= ~(LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET |
                                 LANDLOCK_SCOPE_SIGNAL);
        __attribute__((fallthrough));
    case 6 ... 8:
        /* Removes LANDLOCK_ACCESS_FS_RESOLVE_UNIX for ABI < 9 */
        ruleset_attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_RESOLVE_UNIX;
    }

This enables the creation of an inclusive ruleset that will contain our rules.
(这样就能创建一个包容性的 ruleset 来容纳我们的规则。)

.. code-block:: c

    int ruleset_fd;

    ruleset_fd = landlock_create_ruleset(&ruleset_attr, sizeof(ruleset_attr), 0);
    if (ruleset_fd < 0) {
        perror("Failed to create a ruleset");
        return 1;
    }

We can now add a new rule to this ruleset thanks to the returned file
descriptor referring to this ruleset.  The rule will allow reading and
executing the file hierarchy ``/usr``.  Without another rule, write actions
would then be denied by the ruleset.  To add ``/usr`` to the ruleset, we open
it with the ``O_PATH`` flag and fill the &struct landlock_path_beneath_attr with
this file descriptor.
(现在，借助返回的、指向该 ruleset 的文件描述符，我们可以向其中添加新规则。这条规则将允许读取和执行文件层次结构 ``/usr`` 。如果没有其他规则，写操作将被该 ruleset 拒绝。要把 ``/usr`` 加入 ruleset，我们用 ``O_PATH`` 标志打开它，并用这个文件描述符填充 &struct landlock_path_beneath_attr。)

.. code-block:: c

    int err;
    struct landlock_path_beneath_attr path_beneath = {
        .allowed_access =
            LANDLOCK_ACCESS_FS_EXECUTE |
            LANDLOCK_ACCESS_FS_READ_FILE |
            LANDLOCK_ACCESS_FS_READ_DIR,
    };

    path_beneath.parent_fd = open("/usr", O_PATH | O_CLOEXEC);
    if (path_beneath.parent_fd < 0) {
        perror("Failed to open file");
        close(ruleset_fd);
        return 1;
    }
    err = landlock_add_rule(ruleset_fd, LANDLOCK_RULE_PATH_BENEATH,
                            &path_beneath, 0);
    close(path_beneath.parent_fd);
    if (err) {
        perror("Failed to update ruleset");
        close(ruleset_fd);
        return 1;
    }

It may also be required to create rules following the same logic as explained
for the ruleset creation, by filtering access rights according to the Landlock
ABI version.  In this example, this is not required because all of the requested
``allowed_access`` rights are already available in ABI 1.
(也可能需要按照创建 ruleset 时说明的相同逻辑来创建规则，即根据 Landlock ABI 版本过滤访问权限。在本例中这一步并非必需，因为所请求的 ``allowed_access`` 权限在 ABI 1 中就已经全部可用。)

For network access-control, we can add a set of rules that allow to use a port
number for a specific action: HTTPS connections.
(对于网络访问控制，我们可以添加一组规则，允许将某个端口号用于特定操作：HTTPS 连接。)

.. code-block:: c

    struct landlock_net_port_attr net_port = {
        .allowed_access = LANDLOCK_ACCESS_NET_CONNECT_TCP,
        .port = 443,
    };

    err = landlock_add_rule(ruleset_fd, LANDLOCK_RULE_NET_PORT,
                            &net_port, 0);

When passing a non-zero ``flags`` argument to ``landlock_restrict_self()``, a
similar backwards compatibility check is needed for the restrict flags
(see sys_landlock_restrict_self() documentation for available flags):
(在向 ``landlock_restrict_self()`` 传递非零 ``flags`` 参数时，需要对 restrict 标志做类似的向后兼容检查 (可用标志见 sys_landlock_restrict_self() 文档)：)

.. code-block:: c

    __u32 restrict_flags =
        LANDLOCK_RESTRICT_SELF_LOG_NEW_EXEC_ON |
        LANDLOCK_RESTRICT_SELF_TSYNC;
    switch (abi) {
    case 1 ... 6:
        /* Removes logging flags for ABI < 7 */
        restrict_flags &= ~(LANDLOCK_RESTRICT_SELF_LOG_SAME_EXEC_OFF |
                            LANDLOCK_RESTRICT_SELF_LOG_NEW_EXEC_ON |
                            LANDLOCK_RESTRICT_SELF_LOG_SUBDOMAINS_OFF);
        __attribute__((fallthrough));
    case 7:
        /*
         * Removes multithreaded enforcement flag for ABI < 8
         *
         * WARNING: Without this flag, calling landlock_restrict_self(2) is
         * only equivalent if the calling process is single-threaded. Below
         * ABI v8 (and as of ABI v8, when not using this flag), a Landlock
         * policy would only be enforced for the calling thread and its
         * children (and not for all threads, including parents and siblings).
         */
        restrict_flags &= ~LANDLOCK_RESTRICT_SELF_TSYNC;
    }

The next step is to restrict the current thread from gaining more privileges
(e.g. through a SUID binary).  We now have a ruleset with the first rule
allowing read and execute access to ``/usr`` while denying all other handled
accesses for the filesystem, and a second rule allowing HTTPS connections.
(下一步是阻止当前线程获得更多特权 (例如通过 SUID 二进制程序)。现在我们拥有的 ruleset 包含两条规则：第一条允许对 ``/usr`` 的读和执行访问，同时拒绝文件系统所有其他被处理 (handled) 的访问；第二条允许 HTTPS 连接。)

.. code-block:: c

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        perror("Failed to restrict privileges");
        close(ruleset_fd);
        return 1;
    }

The current thread is now ready to sandbox itself with the ruleset.
(现在，当前线程已准备好用这个 ruleset 对自身进行沙箱化。)

.. code-block:: c

    if (landlock_restrict_self(ruleset_fd, restrict_flags)) {
        perror("Failed to enforce ruleset");
        close(ruleset_fd);
        return 1;
    }
    close(ruleset_fd);

If the ``landlock_restrict_self`` system call succeeds, the current thread is
now restricted and this policy will be enforced on all its subsequently created
children as well.  Once a thread is landlocked, there is no way to remove its
security policy; only adding more restrictions is allowed.  These threads are
now in a new Landlock domain, which is a merger of their parent one (if any)
with the new ruleset.
(如果 ``landlock_restrict_self`` 系统调用成功，当前线程即被限制，且该策略也会在其后续创建的所有子进程上强制执行。一旦线程被 landlock，就无法移除其安全策略；只允许增加更多限制。这些线程现在处于一个新的 Landlock domain 中，它是父 domain (如果有的话) 与新 ruleset 的合并。)

Full working code can be found in `samples/landlock/sandboxer.c`_.
(完整的可运行代码见 `samples/landlock/sandboxer.c`_ 。)

Good practices(最佳实践)
------------------------

It is recommended to set access rights to file hierarchy leaves as much as
possible.  For instance, it is better to be able to have ``~/doc/`` as a
read-only hierarchy and ``~/tmp/`` as a read-write hierarchy, compared to
``~/`` as a read-only hierarchy and ``~/tmp/`` as a read-write hierarchy.
Following this good practice leads to self-sufficient hierarchies that do not
depend on their location (i.e. parent directories).  This is particularly
relevant when we want to allow linking or renaming.  Indeed, having consistent
access rights per directory enables changing the location of such directories
without relying on the destination directory access rights (except those that
are required for this operation, see ``LANDLOCK_ACCESS_FS_REFER``
documentation).
(建议尽可能把访问权限设置到文件层次结构的叶子节点上。例如，让 ``~/doc/`` 为只读层次结构、 ``~/tmp/`` 为读写层次结构，要优于让 ``~/`` 为只读层次结构、 ``~/tmp/`` 为读写层次结构。遵循这一最佳实践可以形成自给自足的层次结构，不依赖于其位置 (即父目录)。在希望允许链接或重命名时这一点尤其重要。事实上，每个目录拥有一致的访问权限，使得可以在不依赖目标目录访问权限的情况下改变这些目录的位置 (本操作所要求的权限除外，参见 ``LANDLOCK_ACCESS_FS_REFER`` 文档)。)

Having self-sufficient hierarchies also helps to tighten the required access
rights to the minimal set of data.  This also helps avoid sinkhole directories,
i.e. directories where data can be linked to but not linked from.  However,
this depends on data organization, which might not be controlled by developers.
In this case, granting read-write access to ``~/tmp/``, instead of write-only
access, would potentially allow moving ``~/tmp/`` to a non-readable directory
and still keep the ability to list the content of ``~/tmp/``.
(自给自足的层次结构还有助于把所需的访问权限收紧到最小数据集。这也有助于避免"汇点" (sinkhole) 目录，即数据可以链接进去却不能链接出来的目录。不过，这取决于数据的组织方式，而这可能不受开发者控制。在这种情况下，授予 ``~/tmp/`` 读写权限(而不是只写权限)，可能允许把 ``~/tmp/`` 移动到一个不可读的目录，同时仍保留列出 ``~/tmp/`` 内容的能力。)

Layers of file path access rights(文件路径访问权限的分层)
---------------------------------------------------------

Each time a thread enforces a ruleset on itself, it updates its Landlock domain
with a new layer of policy.  This complementary policy is stacked with any
other rulesets potentially already restricting this thread.  A sandboxed thread
can then safely add more constraints to itself with a new enforced ruleset.
(每当一个线程对自身实施一个 ruleset，它就会用新的一层策略更新其 Landlock domain。这一补充策略会与可能已经在限制该线程的其他 ruleset 叠加。因此，被沙箱化的线程可以用新实施的 ruleset 安全地给自己添加更多约束。)

One policy layer grants access to a file path if at least one of its rules
encountered on the path grants the access.  A sandboxed thread can only access
a file path if all its enforced policy layers grant the access as well as all
the other system access controls (e.g. filesystem DAC, other LSM policies,
etc.).
(只要在路径上遇到的某一策略层中至少有一条规则授予访问权，该策略层就授予对该文件路径的访问。被沙箱化的线程只有在所有已实施的策略层都授予访问权、且所有其他系统访问控制 (如文件系统 DAC、其他 LSM 策略等) 也都放行时，才能访问一个文件路径。)

Bind mounts and OverlayFS(绑定挂载与 OverlayFS)
-----------------------------------------------

Landlock enables restricting access to file hierarchies, which means that these
access rights can be propagated with bind mounts (cf.
Documentation/filesystems/sharedsubtree.rst) but not with
Documentation/filesystems/overlayfs.rst.
(Landlock 支持限制对文件层次结构的访问，这意味着这些访问权限可以随绑定挂载传播 (参见 Documentation/filesystems/sharedsubtree.rst)，但不能随 Documentation/filesystems/overlayfs.rst 传播。)

A bind mount mirrors a source file hierarchy to a destination.  The destination
hierarchy is then composed of the exact same files, on which Landlock rules can
be tied, either via the source or the destination path.  These rules restrict
access when they are encountered on a path, which means that they can restrict
access to multiple file hierarchies at the same time, whether these hierarchies
are the result of bind mounts or not.
(绑定挂载把一个源文件层次结构镜像到目标位置。目标层次结构由完全相同的文件组成，Landlock 规则可以通过源路径或目标路径绑定到这些文件上。这些规则在路径上被遇到时就会限制访问，这意味着它们可以同时限制对多个文件层次结构的访问，无论这些层次结构是否是绑定挂载的结果。)

An OverlayFS mount point consists of upper and lower layers.  These layers are
combined in a merge directory, and that merged directory becomes available at
the mount point.  This merge hierarchy may include files from the upper and
lower layers, but modifications performed on the merge hierarchy only reflect
on the upper layer.  From a Landlock policy point of view, all OverlayFS layers
and merge hierarchies are standalone and each contains their own set of files
and directories, which is different from bind mounts.  A policy restricting an
OverlayFS layer will not restrict the resulted merged hierarchy, and vice versa.
Landlock users should then only think about file hierarchies they want to allow
access to, regardless of the underlying filesystem.
(一个 OverlayFS 挂载点由上层 (upper) 和下层 (lower) 组成。这些层合并到一个 merge 目录中，合并后的目录在挂载点上可用。该合并层次结构可能包含来自上下两层的文件，但在合并层次结构上执行的修改只反映到上层。从 Landlock 策略的角度看，所有 OverlayFS 层和合并层次结构都是独立的，各自拥有自己的文件和目录集合，这与绑定挂载不同。限制某个 OverlayFS 层的策略不会限制合并出来的层次结构，反之亦然。因此 Landlock 用户只需考虑希望允许访问哪些文件层次结构，而不必关心底层文件系统。)

Inheritance(继承)
-----------------

Every new thread resulting from a :manpage:`clone(2)` inherits Landlock domain
restrictions from its parent.  This is similar to seccomp inheritance (cf.
Documentation/userspace-api/seccomp_filter.rst) or any other LSM dealing with
task's :manpage:`credentials(7)`.  For instance, one process's thread may apply
Landlock rules to itself, but they will not be automatically applied to other
sibling threads (unlike POSIX thread credential changes, cf.
:manpage:`nptl(7)`).
(由 :manpage:`clone(2)` 产生的每个新线程都从其父线程继承 Landlock domain 限制。这类似于 seccomp 继承 (参见 Documentation/userspace-api/seccomp_filter.rst) 或任何其他处理任务 :manpage:`credentials(7)` 的 LSM。例如，一个进程的线程可以对自己应用 Landlock 规则，但这些规则不会自动应用到其他同辈线程 (这与 POSIX 线程凭证变更不同，参见 :manpage:`nptl(7)` )。)

When a thread sandboxes itself, we have the guarantee that the related security
policy will stay enforced on all this thread's descendants.  This allows
creating standalone and modular security policies per application, which will
automatically be composed between themselves according to their runtime parent
policies.
(当一个线程对自身沙箱化时，我们可以保证相关安全策略在该线程的所有后代上持续生效。这允许为每个应用创建独立、模块化的安全策略，它们会根据运行时的父策略自动彼此组合。)

Ptrace restrictions(Ptrace 限制)
--------------------------------

A sandboxed process has less privileges than a non-sandboxed process and must
then be subject to additional restrictions when manipulating another process.
To be allowed to use :manpage:`ptrace(2)` and related syscalls on a target
process, a sandboxed process should have a superset of the target process's
access rights, which means the tracee must be in a sub-domain of the tracer.
(被沙箱化的进程比未沙箱化的进程特权更少，因此在操作其他进程时必须受到额外限制。要被允许对目标进程使用 :manpage:`ptrace(2)` 及相关系统调用，被沙箱化的进程应拥有目标进程访问权限的超集，也就是说被追踪者 (tracee) 必须处于追踪者 (tracer) 的子 domain 中。)

IPC scoping(IPC 作用域)
-----------------------

Similar to the implicit `Ptrace restrictions(Ptrace 限制)`_, we may want to further restrict
interactions between sandboxes.  Therefore, at ruleset creation time, each
Landlock domain can restrict the scope for certain operations, so that these
operations can only reach out to processes within the same Landlock domain or in
a nested Landlock domain (the "scope").
(类似于隐式的 `Ptrace restrictions(Ptrace 限制)`_ ，我们可能希望进一步限制沙箱之间的交互。因此，在创建 ruleset 时，每个 Landlock domain 可以限制某些操作的作用范围，使这些操作只能触达同一 Landlock domain 内或嵌套 Landlock domain 中的进程 (即 "scope"(作用域) )。)

The operations which can be scoped are:
(可以被限定作用范围的操作有：)

``LANDLOCK_SCOPE_SIGNAL``
    This limits the sending of signals to target processes which run within the
    same or a nested Landlock domain.
    (这限制了向运行在同一或嵌套 Landlock domain 内的目标进程发送信号。)

``LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET``
    This limits the set of abstract :manpage:`unix(7)` sockets to which we can
    :manpage:`connect(2)` to socket addresses which were created by a process in
    the same or a nested Landlock domain.
    (这限制了我们可以 :manpage:`connect(2)` 的抽象 :manpage:`unix(7)` 套接字集合——只能连接由同一或嵌套 Landlock domain 中的进程创建的套接字地址。)

    A :manpage:`sendto(2)` on a non-connected datagram socket is treated as if
    it were doing an implicit :manpage:`connect(2)` and will be blocked if the
    remote end does not stem from the same or a nested Landlock domain.
    (在未连接的数据报套接字上进行 :manpage:`sendto(2)` 会被视作在执行隐式 :manpage:`connect(2)` ，如果远端并非来自同一或嵌套 Landlock domain，则会被阻止。)

    A :manpage:`sendto(2)` on a socket which was previously connected will not
    be restricted.  This works for both datagram and stream sockets.
    (在之前已连接的套接字上进行 :manpage:`sendto(2)` 则不受限制。这对数据报套接字和流套接字都适用。)

IPC scoping does not support exceptions via :manpage:`landlock_add_rule(2)`.
If an operation is scoped within a domain, no rules can be added to allow access
to resources or processes outside of the scope.
(IPC 作用域不支持通过 :manpage:`landlock_add_rule(2)` 设置例外。如果一个操作在某个 domain 内被限定了作用范围，就不能通过添加规则来允许访问该范围之外的资源或进程。)

Truncating files(截断文件)
--------------------------

The operations covered by ``LANDLOCK_ACCESS_FS_WRITE_FILE`` and
``LANDLOCK_ACCESS_FS_TRUNCATE`` both change the contents of a file and sometimes
overlap in non-intuitive ways.  It is strongly recommended to always specify
both of these together (either granting both, or granting none).
(``LANDLOCK_ACCESS_FS_WRITE_FILE`` 和 ``LANDLOCK_ACCESS_FS_TRUNCATE`` 覆盖的操作都会改变文件内容，而且有时会以不直观的方式重叠。强烈建议始终把这两者一起指定 (要么都授予，要么都不授予)。)

A particularly surprising example is :manpage:`creat(2)`.  The name suggests
that this system call requires the rights to create and write files.  However,
it also requires the truncate right if an existing file under the same name is
already present.
(一个特别令人意外的例子是 :manpage:`creat(2)` 。从名字看，这个系统调用似乎只需要创建和写文件的权限。然而，如果同名文件已存在，它还需要截断权限。)

It should also be noted that truncating files does not require the
``LANDLOCK_ACCESS_FS_WRITE_FILE`` right.  Apart from the :manpage:`truncate(2)`
system call, this can also be done through :manpage:`open(2)` with the flags
``O_RDONLY | O_TRUNC``.
(还应注意，截断文件并不需要 ``LANDLOCK_ACCESS_FS_WRITE_FILE`` 权限。除了 :manpage:`truncate(2)` 系统调用之外，也可以通过带 ``O_RDONLY | O_TRUNC`` 标志的 :manpage:`open(2)` 来完成。)

At the same time, on some filesystems, :manpage:`fallocate(2)` offers a way to
shorten file contents with ``FALLOC_FL_COLLAPSE_RANGE`` when the file is opened
for writing, sidestepping the ``LANDLOCK_ACCESS_FS_TRUNCATE`` right.
(与此同时，在某些文件系统上，当文件以写方式打开时， :manpage:`fallocate(2)` 提供了一种用 ``FALLOC_FL_COLLAPSE_RANGE`` 缩短文件内容的方法，从而绕过 ``LANDLOCK_ACCESS_FS_TRUNCATE`` 权限。)

The truncate right is associated with the opened file (see below).
(截断权限与打开的文件相关联 (见下文)。)

Rights associated with file descriptors(与文件描述符关联的权限)
---------------------------------------------------------------

When opening a file, the availability of the ``LANDLOCK_ACCESS_FS_TRUNCATE`` and
``LANDLOCK_ACCESS_FS_IOCTL_DEV`` rights is associated with the newly created
file descriptor and will be used for subsequent truncation and ioctl attempts
using :manpage:`ftruncate(2)` and :manpage:`ioctl(2)`.  The behavior is similar
to opening a file for reading or writing, where permissions are checked during
:manpage:`open(2)`, but not during the subsequent :manpage:`read(2)` and
:manpage:`write(2)` calls.
(打开文件时， ``LANDLOCK_ACCESS_FS_TRUNCATE`` 和 ``LANDLOCK_ACCESS_FS_IOCTL_DEV`` 权限的可用性与新创建的文件描述符关联，后续使用 :manpage:`ftruncate(2)` 和 :manpage:`ioctl(2)` 进行截断和 ioctl 的尝试都将依据它。其行为类似于以读或写方式打开文件：权限在 :manpage:`open(2)` 时检查，而在随后的 :manpage:`read(2)` 和 :manpage:`write(2)` 调用时不再检查。)

As a consequence, it is possible that a process has multiple open file
descriptors referring to the same file, but Landlock enforces different things
when operating with these file descriptors.  This can happen when a Landlock
ruleset gets enforced and the process keeps file descriptors which were opened
both before and after the enforcement.  It is also possible to pass such file
descriptors between processes, keeping their Landlock properties, even when some
of the involved processes do not have an enforced Landlock ruleset.
(因此，一个进程可能持有多个指向同一文件的打开文件描述符，但 Landlock 在通过这些文件描述符操作时会实施不同的限制。当一个 Landlock ruleset 被实施、而该进程同时保留实施前和实施后打开的文件描述符时，就会出现这种情况。这样的文件描述符还可以在进程之间传递并保留其 Landlock 属性，即使其中某些相关进程并没有实施 Landlock ruleset。)

Compatibility(兼容性)
=====================

Backward and forward compatibility(向后与向前兼容)
--------------------------------------------------

Landlock is designed to be compatible with past and future versions of the
kernel.  This is achieved thanks to the system call attributes and the
associated bitflags, particularly the ruleset's ``handled_access_fs``.  Making
handled access rights explicit enables the kernel and user space to have a clear
contract with each other.  This is required to make sure sandboxing will not
get stricter with a system update, which could break applications.
(Landlock 被设计为与过去和未来的内核版本兼容。这得益于系统调用属性及其关联的位标志，尤其是 ruleset 的 ``handled_access_fs`` 。把被处理 (handled) 的访问权限显式声明出来，使内核与用户空间之间拥有清晰的契约。这是确保沙箱不会因系统更新而变得更严格所必需的——变严格可能破坏应用程序。)

Developers can subscribe to the `Landlock mailing list
<https://subspace.kernel.org/lists.linux.dev.html>`_ to knowingly update and
test their applications with the latest available features.  In the interest of
users, and because they may use different kernel versions, it is strongly
encouraged to follow a best-effort security approach by checking the Landlock
ABI version at runtime and only enforcing the supported features.
(开发者可以订阅 `Landlock mailing list
<https://subspace.kernel.org/lists.linux.dev.html>`_ ，以便在了解最新可用特性的情况下更新并测试自己的应用。为了用户利益，由于他们可能使用不同的内核版本，强烈建议遵循 best-effort 安全方式：在运行时检查 Landlock ABI 版本，只强制实施受支持的特性。)

.. _landlock_abi_versions:

Landlock ABI versions(Landlock ABI 版本)
----------------------------------------

The Landlock ABI version can be read with the sys_landlock_create_ruleset()
system call:
(Landlock ABI 版本可以通过 sys_landlock_create_ruleset() 系统调用读取：)

.. code-block:: c

    int abi;

    abi = landlock_create_ruleset(NULL, 0, LANDLOCK_CREATE_RULESET_VERSION);
    if (abi < 0) {
        switch (errno) {
        case ENOSYS:
            printf("Landlock is not supported by the current kernel.\n");
            break;
        case EOPNOTSUPP:
            printf("Landlock is currently disabled.\n");
            break;
        }
        return 0;
    }
    if (abi >= 2) {
        printf("Landlock supports LANDLOCK_ACCESS_FS_REFER.\n");
    }

All Landlock kernel interfaces are supported by the first ABI version unless
explicitly noted in their documentation.
(除非其文档中另有明确说明，第一个 ABI 版本支持所有 Landlock 内核接口。)

Landlock errata(Landlock 勘误)
------------------------------

In addition to ABI versions, Landlock provides an errata mechanism to track
fixes for issues that may affect backwards compatibility or require userspace
awareness.  The errata bitmask can be queried using:
(除 ABI 版本之外，Landlock 还提供 errata(勘误) 机制，用于跟踪可能影响向后兼容性或需要用户空间知晓的问题的修复情况。errata 位掩码可通过以下方式查询：)

.. code-block:: c

    int errata;

    errata = landlock_create_ruleset(NULL, 0, LANDLOCK_CREATE_RULESET_ERRATA);
    if (errata < 0) {
        /* Landlock not available or disabled */
        return 0;
    }

The returned value is a bitmask where each bit represents a specific erratum.
If bit N is set (``errata & (1 << (N - 1))``), then erratum N has been fixed
in the running kernel.
(返回值是一个位掩码，其中每个位代表一个特定的 erratum。如果第 N 位被置位 ( ``errata & (1 << (N - 1))`` )，则说明 erratum N 已在运行中的内核里被修复。)

.. warning::

   **Most applications should NOT check errata.** In 99.9% of cases, checking
   errata is unnecessary, increases code complexity, and can potentially
   decrease protection if misused.  For example, disabling the sandbox when an
   erratum is not fixed could leave the system less secure than using
   Landlock's best-effort protection.  When in doubt, ignore errata.
   (**大多数应用不应检查 errata。** 在 99.9% 的情况下，检查 errata 是不必要的，会增加代码复杂度，而且如果使用不当还可能降低保护。例如，在某个 erratum 尚未修复时禁用沙箱，可能使系统比使用 Landlock 的 best-effort 保护更不安全。如有疑问，请忽略 errata。)

.. kernel-doc:: security/landlock/errata/abi-4.h
    :doc: erratum_1

.. kernel-doc:: security/landlock/errata/abi-6.h
    :doc: erratum_2

.. kernel-doc:: security/landlock/errata/abi-1.h
    :doc: erratum_3

How to check for errata(如何检查 errata)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you determine that your application needs to check for specific errata,
use this pattern:
(如果你确定自己的应用需要检查特定的 errata，请使用如下模式：)

.. code-block:: c

    int errata = landlock_create_ruleset(NULL, 0, LANDLOCK_CREATE_RULESET_ERRATA);
    if (errata >= 0) {
        /* Check for specific erratum (1-indexed) */
        if (errata & (1 << (erratum_number - 1))) {
            /* Erratum N is fixed in this kernel */
        } else {
            /* Erratum N is NOT fixed - consider implications for your use case */
        }
    }

**Important:** Only check errata if your application specifically relies on
behavior that changed due to the fix.  The fixes generally make Landlock less
restrictive or more correct, not more restrictive.
(**重要：** 只有当你的应用明确依赖于因该修复而改变的行为时才检查 errata。这些修复通常使 Landlock 的限制更宽松或更正确，而不是更严格。)

Kernel interface(内核接口)
==========================

Access rights(访问权限)
-----------------------

.. kernel-doc:: include/uapi/linux/landlock.h
    :identifiers: fs_access net_access scope

Creating a new ruleset(创建新 ruleset)
--------------------------------------

.. kernel-doc:: security/landlock/syscalls.c
    :identifiers: sys_landlock_create_ruleset

.. kernel-doc:: include/uapi/linux/landlock.h
    :identifiers: landlock_ruleset_attr

Extending a ruleset(扩展 ruleset)
---------------------------------

.. kernel-doc:: security/landlock/syscalls.c
    :identifiers: sys_landlock_add_rule

.. kernel-doc:: include/uapi/linux/landlock.h
    :identifiers: landlock_rule_type landlock_path_beneath_attr
                  landlock_net_port_attr

Enforcing a ruleset(实施 ruleset)
---------------------------------

.. kernel-doc:: security/landlock/syscalls.c
    :identifiers: sys_landlock_restrict_self

Current limitations(当前限制)
=============================

Filesystem topology modification(文件系统拓扑修改)
--------------------------------------------------

Threads sandboxed with filesystem restrictions cannot modify filesystem
topology, whether via :manpage:`mount(2)` or :manpage:`pivot_root(2)`.
However, :manpage:`chroot(2)` calls are not denied.
(带有文件系统限制沙箱化的线程不能修改文件系统拓扑，无论是通过 :manpage:`mount(2)` 还是 :manpage:`pivot_root(2)` 。不过， :manpage:`chroot(2)` 调用不会被拒绝。)

Special filesystems(特殊文件系统)
---------------------------------

Access to regular files and directories can be restricted by Landlock,
according to the handled accesses of a ruleset.  However, files that do not
come from a user-visible filesystem (e.g. pipe, socket), but can still be
accessed through ``/proc/<pid>/fd/*``, cannot currently be explicitly
restricted.  Likewise, some special kernel filesystems such as nsfs, which can
be accessed through ``/proc/<pid>/ns/*``, cannot currently be explicitly
restricted.  However, thanks to the `ptrace restrictions(Ptrace 限制)`_, access to such
sensitive ``/proc`` files are automatically restricted according to domain
hierarchies.  Future Landlock evolutions could still enable to explicitly
restrict such paths with dedicated ruleset flags.
(普通文件和目录的访问可以由 Landlock 根据 ruleset 所处理 (handled) 的访问来限制。然而，并非来自用户可见文件系统的文件 (如管道、套接字)，虽然仍可以通过 ``/proc/<pid>/fd/*`` 访问，但目前无法被显式限制。同样，一些特殊内核文件系统 (如 nsfs，可通过 ``/proc/<pid>/ns/*`` 访问) 目前也无法被显式限制。不过，得益于 `ptrace restrictions(Ptrace 限制)`_ ，对这类敏感 ``/proc`` 文件的访问已根据 domain 层次自动受限。未来 Landlock 的演进仍可能通过专用的 ruleset 标志来显式限制这类路径。)

Ruleset layers(ruleset 分层)
----------------------------

There is a limit of 16 layers of stacked rulesets.  This can be an issue for a
task willing to enforce a new ruleset in complement to its 16 inherited
rulesets.  Once this limit is reached, sys_landlock_restrict_self() returns
E2BIG.  It is then strongly suggested to carefully build rulesets once in the
life of a thread, especially for applications able to launch other applications
that may also want to sandbox themselves (e.g. shells, container managers,
etc.).
(堆叠的 ruleset 最多 16 层。对于一个想要在继承的 16 个 ruleset 之外再实施一个新 ruleset 的任务来说，这可能是个问题。一旦达到此上限，sys_landlock_restrict_self() 会返回 E2BIG。因此强烈建议在线程的一生中一次性仔细构建好 ruleset，尤其是那些能够启动其他应用、而这些应用也可能想对自身沙箱化的程序 (如 shell、容器管理器等)。)

Memory usage(内存使用)
----------------------

Kernel memory allocated to create rulesets is accounted and can be restricted
by the Documentation/admin-guide/cgroup-v1/memory.rst.
(为创建 ruleset 而分配的内核内存会被统计，并可以由 Documentation/admin-guide/cgroup-v1/memory.rst 加以限制。)

IOCTL support(IOCTL 支持)
-------------------------

The ``LANDLOCK_ACCESS_FS_IOCTL_DEV`` right restricts the use of
:manpage:`ioctl(2)`, but it only applies to *newly opened* device files.  This
means specifically that pre-existing file descriptors like stdin, stdout and
stderr are unaffected.
(``LANDLOCK_ACCESS_FS_IOCTL_DEV`` 权限限制 :manpage:`ioctl(2)` 的使用，但它只适用于 *新打开的* 设备文件。具体而言，这意味着诸如 stdin、stdout 和 stderr 这类预先存在的文件描述符不受影响。)

Users should be aware that TTY devices have traditionally permitted to control
other processes on the same TTY through the ``TIOCSTI`` and ``TIOCLINUX`` IOCTL
commands.  Both of these require ``CAP_SYS_ADMIN`` on modern Linux systems, but
the behavior is configurable for ``TIOCSTI``.
(用户应当知道，TTY 设备历来允许通过 ``TIOCSTI`` 和 ``TIOCLINUX`` IOCTL 命令控制同一 TTY 上的其他进程。在现代 Linux 系统上这两者都需要 ``CAP_SYS_ADMIN`` ，但 ``TIOCSTI`` 的行为是可配置的。)

On older systems, it is therefore recommended to close inherited TTY file
descriptors, or to reopen them from ``/proc/self/fd/*`` without the
``LANDLOCK_ACCESS_FS_IOCTL_DEV`` right, if possible.
(因此在较旧的系统上，建议关闭继承而来的 TTY 文件描述符，或者在可能的情况下，从 ``/proc/self/fd/*`` 重新打开它们且不带 ``LANDLOCK_ACCESS_FS_IOCTL_DEV`` 权限。)

Landlock's IOCTL support is coarse-grained at the moment, but may become more
fine-grained in the future.  Until then, users are advised to establish the
guarantees that they need through the file hierarchy, by only allowing the
``LANDLOCK_ACCESS_FS_IOCTL_DEV`` right on files where it is really required.
(Landlock 的 IOCTL 支持目前是粗粒度的，但未来可能变得更加细粒度。在此之前，建议用户通过文件层次结构来建立所需保证，只在确实需要的文件上允许 ``LANDLOCK_ACCESS_FS_IOCTL_DEV`` 权限。)

Previous limitations(既往限制)
==============================

File renaming and linking (ABI < 2)(文件重命名与链接 (ABI < 2))
---------------------------------------------------------------

Because Landlock targets unprivileged access controls, it needs to properly
handle composition of rules.  Such property also implies rules nesting.
Properly handling multiple layers of rulesets, each one of them able to
restrict access to files, also implies inheritance of the ruleset restrictions
from a parent to its hierarchy.  Because files are identified and restricted by
their hierarchy, moving or linking a file from one directory to another implies
propagation of the hierarchy constraints, or restriction of these actions
according to the potentially lost constraints.  To protect against privilege
escalations through renaming or linking, and for the sake of simplicity,
Landlock previously limited linking and renaming to the same directory.
Starting with the Landlock ABI version 2, it is now possible to securely
control renaming and linking thanks to the new ``LANDLOCK_ACCESS_FS_REFER``
access right.
(由于 Landlock 面向非特权的访问控制，它需要正确处理规则的组合。这一特性也意味着规则可以嵌套。正确处理多层 ruleset——其中每一层都能限制对文件的访问——还意味着 ruleset 限制会从父级继承到其层次结构中。由于文件是按其层次结构来标识和限制的，把文件从一个目录移动或链接到另一个目录意味着层次结构约束的传播，或者根据可能丢失的约束来限制这些操作。为防止通过重命名或链接进行的权限提升，也为了简单起见，Landlock 此前把链接和重命名限制在同一目录内。从 Landlock ABI 版本 2 开始，借助新的 ``LANDLOCK_ACCESS_FS_REFER`` 访问权限，现在已经可以安全地控制重命名和链接。)

File truncation (ABI < 3)(文件截断 (ABI < 3))
---------------------------------------------

File truncation could not be denied before the third Landlock ABI, so it is
always allowed when using a kernel that only supports the first or second ABI.
(在第三个 Landlock ABI 之前无法拒绝文件截断，因此在使用仅支持第一或第二个 ABI 的内核时，截断总是被允许。)

Starting with the Landlock ABI version 3, it is now possible to securely control
truncation thanks to the new ``LANDLOCK_ACCESS_FS_TRUNCATE`` access right.
(从 Landlock ABI 版本 3 开始，借助新的 ``LANDLOCK_ACCESS_FS_TRUNCATE`` 访问权限，现在已经可以安全地控制截断。)

TCP bind and connect (ABI < 4)(TCP 绑定与连接 (ABI < 4))
--------------------------------------------------------

Starting with the Landlock ABI version 4, it is now possible to restrict TCP
bind and connect actions to only a set of allowed ports thanks to the new
``LANDLOCK_ACCESS_NET_BIND_TCP`` and ``LANDLOCK_ACCESS_NET_CONNECT_TCP``
access rights.
(从 Landlock ABI 版本 4 开始，借助新的 ``LANDLOCK_ACCESS_NET_BIND_TCP`` 和 ``LANDLOCK_ACCESS_NET_CONNECT_TCP`` 访问权限，现在已经可以把 TCP 绑定与连接操作限制到一组允许的端口上。)

Device IOCTL (ABI < 5)(设备 IOCTL (ABI < 5))
--------------------------------------------

IOCTL operations could not be denied before the fifth Landlock ABI, so
:manpage:`ioctl(2)` is always allowed when using a kernel that only supports an
earlier ABI.
(在第五个 Landlock ABI 之前无法拒绝 IOCTL 操作，因此在使用仅支持更早 ABI 的内核时， :manpage:`ioctl(2)` 总是被允许。)

Starting with the Landlock ABI version 5, it is possible to restrict the use of
:manpage:`ioctl(2)` on character and block devices using the new
``LANDLOCK_ACCESS_FS_IOCTL_DEV`` right.
(从 Landlock ABI 版本 5 开始，可以使用新的 ``LANDLOCK_ACCESS_FS_IOCTL_DEV`` 权限限制在字符设备和块设备上使用 :manpage:`ioctl(2)` 。)

Abstract UNIX socket (ABI < 6)(抽象 UNIX 套接字 (ABI < 6))
----------------------------------------------------------

Starting with the Landlock ABI version 6, it is possible to restrict
connections to an abstract :manpage:`unix(7)` socket by setting
``LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET`` to the ``scoped`` ruleset attribute.
(从 Landlock ABI 版本 6 开始，可以通过把 ``LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET`` 设置到 ruleset 的 ``scoped`` 属性中，来限制对抽象 :manpage:`unix(7)` 套接字的连接。)

Signal (ABI < 6)(信号 (ABI < 6))
--------------------------------

Starting with the Landlock ABI version 6, it is possible to restrict
:manpage:`signal(7)` sending by setting ``LANDLOCK_SCOPE_SIGNAL`` to the
``scoped`` ruleset attribute.
(从 Landlock ABI 版本 6 开始，可以通过把 ``LANDLOCK_SCOPE_SIGNAL`` 设置到 ruleset 的 ``scoped`` 属性中，来限制 :manpage:`signal(7)` 的发送。)

Logging (ABI < 7)(日志 (ABI < 7))
---------------------------------

Starting with the Landlock ABI version 7, it is possible to control logging of
Landlock audit events with the ``LANDLOCK_RESTRICT_SELF_LOG_SAME_EXEC_OFF``,
``LANDLOCK_RESTRICT_SELF_LOG_NEW_EXEC_ON``, and
``LANDLOCK_RESTRICT_SELF_LOG_SUBDOMAINS_OFF`` flags passed to
sys_landlock_restrict_self().  See Documentation/admin-guide/LSM/landlock.rst
for more details on audit.
(从 Landlock ABI 版本 7 开始，可以通过传递给 sys_landlock_restrict_self() 的 ``LANDLOCK_RESTRICT_SELF_LOG_SAME_EXEC_OFF`` 、 ``LANDLOCK_RESTRICT_SELF_LOG_NEW_EXEC_ON`` 和 ``LANDLOCK_RESTRICT_SELF_LOG_SUBDOMAINS_OFF`` 标志来控制 Landlock 审计事件的记录。审计的更多细节见 Documentation/admin-guide/LSM/landlock.rst。)

Thread synchronization (ABI < 8)(线程同步 (ABI < 8))
----------------------------------------------------

Starting with the Landlock ABI version 8, it is now possible to
enforce Landlock rulesets across all threads of the calling process
using the ``LANDLOCK_RESTRICT_SELF_TSYNC`` flag passed to
sys_landlock_restrict_self().
(从 Landlock ABI 版本 8 开始，可以使用传递给 sys_landlock_restrict_self() 的 ``LANDLOCK_RESTRICT_SELF_TSYNC`` 标志，在调用进程的所有线程上强制实施 Landlock ruleset。)

Pathname UNIX sockets (ABI < 9)(路径名 UNIX 套接字 (ABI < 9))
-------------------------------------------------------------

Starting with the Landlock ABI version 9, it is possible to restrict
connections to pathname UNIX domain sockets (:manpage:`unix(7)`) using
the new ``LANDLOCK_ACCESS_FS_RESOLVE_UNIX`` right.
(从 Landlock ABI 版本 9 开始，可以使用新的 ``LANDLOCK_ACCESS_FS_RESOLVE_UNIX`` 权限来限制对路径名 UNIX 域套接字 (:manpage:`unix(7)`) 的连接。)

.. _kernel_support:

Kernel support(内核支持)
========================

Build time configuration(构建期配置)
------------------------------------

Landlock was first introduced in Linux 5.13 but it must be configured at build
time with ``CONFIG_SECURITY_LANDLOCK=y``.  Landlock must also be enabled at boot
time like other security modules.  The list of security modules enabled by
default is set with ``CONFIG_LSM``.  The kernel configuration should then
contain ``CONFIG_LSM=landlock,[...]`` with ``[...]``  as the list of other
potentially useful security modules for the running system (see the
``CONFIG_LSM`` help).
(Landlock 最早出现在 Linux 5.13 中，但必须在构建时以 ``CONFIG_SECURITY_LANDLOCK=y`` 配置。与其他安全模块一样，Landlock 还必须在启动时启用。默认启用的安全模块列表由 ``CONFIG_LSM`` 设置。内核配置因此应包含 ``CONFIG_LSM=landlock,[...]`` ，其中 ``[...]`` 是对当前系统可能有用的其他安全模块列表 (参见 ``CONFIG_LSM`` 帮助)。)

Boot time configuration(启动期配置)
-----------------------------------

If the running kernel does not have ``landlock`` in ``CONFIG_LSM``, then we can
enable Landlock by adding ``lsm=landlock,[...]`` to
Documentation/admin-guide/kernel-parameters.rst in the boot loader
configuration.
(如果运行中的内核的 ``CONFIG_LSM`` 里没有 ``landlock`` ，我们可以通过在引导加载程序配置中按 Documentation/admin-guide/kernel-parameters.rst 添加 ``lsm=landlock,[...]`` 来启用 Landlock。)

For example, if the current built-in configuration is:
(例如，如果当前的内置配置是：)

.. code-block:: console

    $ zgrep -h "^CONFIG_LSM=" "/boot/config-$(uname -r)" /proc/config.gz 2>/dev/null
    CONFIG_LSM="lockdown,yama,integrity,apparmor"

...and if the cmdline doesn't contain ``landlock`` either:
(……并且 cmdline 里也没有包含 ``landlock`` ：)

.. code-block:: console

    $ sed -n 's/.*\(\<lsm=\S\+\).*/\1/p' /proc/cmdline
    lsm=lockdown,yama,integrity,apparmor

...we should configure the boot loader to set a cmdline extending the ``lsm``
list with the ``landlock,`` prefix::

  lsm=landlock,lockdown,yama,integrity,apparmor

(……我们就应该配置引导加载程序，把 ``lsm`` 列表扩展为带有 ``landlock,`` 前缀的 cmdline：)

After a reboot, we can check that Landlock is up and running by looking at
kernel logs:
(重启之后，我们可以通过查看内核日志来确认 Landlock 已在正常运行：)

.. code-block:: console

    # dmesg | grep landlock || journalctl -kb -g landlock
    [    0.000000] Command line: [...] lsm=landlock,lockdown,yama,integrity,apparmor
    [    0.000000] Kernel command line: [...] lsm=landlock,lockdown,yama,integrity,apparmor
    [    0.000000] LSM: initializing lsm=lockdown,capability,landlock,yama,integrity,apparmor
    [    0.000000] landlock: Up and running.

The kernel may be configured at build time to always load the ``lockdown`` and
``capability`` LSMs.  In that case, these LSMs will appear at the beginning of
the ``LSM: initializing`` log line as well, even if they are not configured in
the boot loader.
(内核可以在构建时被配置为始终加载 ``lockdown`` 和 ``capability`` 这两个 LSM。在这种情况下，即使它们没有在引导加载程序中配置，也会出现在 ``LSM: initializing`` 日志行的开头。)

Network support(网络支持)
-------------------------

To be able to explicitly allow TCP operations (e.g., adding a network rule with
``LANDLOCK_ACCESS_NET_BIND_TCP``), the kernel must support TCP
(``CONFIG_INET=y``).  Otherwise, sys_landlock_add_rule() returns an
``EAFNOSUPPORT`` error, which can safely be ignored because this kind of TCP
operation is already not possible.
(为了能够显式允许 TCP 操作 (例如添加带 ``LANDLOCK_ACCESS_NET_BIND_TCP`` 的网络规则)，内核必须支持 TCP ( ``CONFIG_INET=y`` )。否则，sys_landlock_add_rule() 会返回 ``EAFNOSUPPORT`` 错误，可以安全地忽略它，因为这类 TCP 操作本来就不可能。)

Questions and answers(问答)
===========================

What about user space sandbox managers?(用户空间沙箱管理器怎么办？)
-------------------------------------------------------------------

Using user space processes to enforce restrictions on kernel resources can lead
to race conditions or inconsistent evaluations (i.e. `Incorrect mirroring of
the OS code and state
<https://www.ndss-symposium.org/ndss2003/traps-and-pitfalls-practical-problems-system-call-interposition-based-security-tools/>`_).
(使用用户空间进程来对内核资源实施限制可能导致竞争条件或评估不一致 (即上链接所指的 "对操作系统代码与状态的不正确镜像" 问题)。)

What about namespaces and containers?(命名空间与容器怎么办？)
-------------------------------------------------------------

Namespaces can help create sandboxes but they are not designed for
access-control and then miss useful features for such use case (e.g. no
fine-grained restrictions).  Moreover, their complexity can lead to security
issues, especially when untrusted processes can manipulate them (cf.
`Controlling access to user namespaces <https://lwn.net/Articles/673597/>`_).
(命名空间可以帮助创建沙箱，但它们并不是为访问控制设计的，因而缺少这类用例所需的实用特性 (例如没有细粒度的限制)。此外，它们的复杂性可能带来安全问题，尤其是当不可信进程能够操纵它们时 (参见 `Controlling access to user namespaces <https://lwn.net/Articles/673597/>`_ )。)

How to disable Landlock audit records?(如何禁用 Landlock 审计记录？)
--------------------------------------------------------------------

You might want to put in place filters as explained here:
Documentation/admin-guide/LSM/landlock.rst
(你可能希望按照这里的说明设置过滤器： Documentation/admin-guide/LSM/landlock.rst)

Additional documentation(附加文档)
==================================

* Documentation/admin-guide/LSM/landlock.rst
* Documentation/security/landlock.rst
* https://landlock.io

.. Links
.. _samples/landlock/sandboxer.c:
   https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/samples/landlock/sandboxer.c
