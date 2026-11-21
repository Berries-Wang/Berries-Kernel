===============================
Symbol Namespaces(符号命名空间)
===============================

The following document describes how to use Symbol Namespaces to structure the
export surface of in-kernel symbols exported through the family of
EXPORT_SYMBOL() macros.
(以下文档描述如何使用符号命名空间 (Symbol Namespaces) 来组织通过 EXPORT_SYMBOL() 宏族导出的内核内符号的导出面。)

Introduction(简介)
==================

Symbol Namespaces have been introduced as a means to structure the export
surface of the in-kernel API. It allows subsystem maintainers to partition
their exported symbols into separate namespaces. That is useful for
documentation purposes (think of the SUBSYSTEM_DEBUG namespace) as well as for
limiting the availability of a set of symbols for use in other parts of the
kernel. As of today, modules that make use of symbols exported into namespaces,
are required to import the namespace. Otherwise the kernel will, depending on
its configuration, reject loading the module or warn about a missing import.
(符号命名空间被引入作为一种组织内核内 API 导出面的手段。它允许子系统维护者将其导出的符号划分到独立的命名空间中。这对文档目的(想想 SUBSYSTEM_DEBUG 命名空间)以及限制某组符号在内核其他部分中的可用性都很有用。如今，使用命名空间中导出符号的模块都必须导入该命名空间。否则，内核将根据其配置拒绝加载该模块，或对缺失的导入发出警告。)

Additionally, it is possible to put symbols into a module namespace, strictly
limiting which modules are allowed to use these symbols.
(此外，还可以把符号放入模块命名空间，严格限制哪些模块允许使用这些符号。)

How to define Symbol Namespaces(如何定义符号命名空间)
=====================================================

Symbols can be exported into namespace using different methods. All of them are
changing the way EXPORT_SYMBOL and friends are instrumented to create ksymtab
entries.
(可以使用不同的方法将符号导出到命名空间中。它们都改变了 EXPORT_SYMBOL 及其同族宏生成 ksymtab 条目的插桩方式。)

Using the EXPORT_SYMBOL macros(使用 EXPORT_SYMBOL 宏)
-----------------------------------------------------

In addition to the macros EXPORT_SYMBOL() and EXPORT_SYMBOL_GPL(), that allow
exporting of kernel symbols to the kernel symbol table, variants of these are
available to export symbols into a certain namespace: EXPORT_SYMBOL_NS() and
EXPORT_SYMBOL_NS_GPL(). They take one additional argument: the namespace as a
string constant. Note that this string must not contain whitespaces.
E.g. to export the symbol ``usb_stor_suspend`` into the
namespace ``USB_STORAGE``, use::

	EXPORT_SYMBOL_NS(usb_stor_suspend, "USB_STORAGE");

(除了允许将内核符号导出到内核符号表的 EXPORT_SYMBOL() 和 EXPORT_SYMBOL_GPL() 宏之外，还有可将符号导出到特定命名空间的变体：EXPORT_SYMBOL_NS() 和 EXPORT_SYMBOL_NS_GPL()。它们接受一个额外参数：作为字符串常量的命名空间。注意，该字符串不得包含空白字符。例如，要把符号 ``usb_stor_suspend`` 导出到命名空间 ``USB_STORAGE`` 中，使用：)

The corresponding ksymtab entry struct ``kernel_symbol`` will have the member
``namespace`` set accordingly. A symbol that is exported without a namespace will
refer to ``NULL``. There is no default namespace if none is defined. ``modpost``
and kernel/module/main.c make use the namespace at build time or module load
time, respectively.
(对应的 ksymtab 条目 struct ``kernel_symbol`` 的 ``namespace`` 成员将相应地被设置。未带命名空间导出的符号将引用 ``NULL`` 。如果没有定义，就没有默认命名空间。 ``modpost`` 和 kernel/module/main.c 分别在构建时或模块加载时使用该命名空间。)

Using the DEFAULT_SYMBOL_NAMESPACE define(使用 DEFAULT_SYMBOL_NAMESPACE 定义)
-----------------------------------------------------------------------------

Defining namespaces for all symbols of a subsystem can be very verbose and may
become hard to maintain. Therefore a default define (DEFAULT_SYMBOL_NAMESPACE)
is been provided, that, if set, will become the default for all EXPORT_SYMBOL()
and EXPORT_SYMBOL_GPL() macro expansions that do not specify a namespace.
(为此提供了一个默认定义 (DEFAULT_SYMBOL_NAMESPACE)，如果设置了它，它将成为所有未指定命名空间的 EXPORT_SYMBOL() 和 EXPORT_SYMBOL_GPL() 宏展开的默认值。)

There are multiple ways of specifying this define and it depends on the
subsystem and the maintainer's preference, which one to use. The first option
is to define the default namespace in the ``Makefile`` of the subsystem. E.g. to
export all symbols defined in usb-common into the namespace USB_COMMON, add a
line like this to drivers/usb/common/Makefile::

	ccflags-y += -DDEFAULT_SYMBOL_NAMESPACE='"USB_COMMON"'

(指定这个定义有多种方式，具体使用哪种取决于子系统和维护者的偏好。第一种选项是在子系统的 ``Makefile`` 中定义默认命名空间。例如，要把 usb-common 中定义的所有符号导出到命名空间 USB_COMMON，在 drivers/usb/common/Makefile 中添加如下一行：)

That will affect all EXPORT_SYMBOL() and EXPORT_SYMBOL_GPL() statements. A
symbol exported with EXPORT_SYMBOL_NS() while this definition is present, will
still be exported into the namespace that is passed as the namespace argument
as this argument has preference over a default symbol namespace.
(这会影响所有 EXPORT_SYMBOL() 和 EXPORT_SYMBOL_GPL() 语句。在该定义存在的情况下，用 EXPORT_SYMBOL_NS() 导出的符号仍会被导出到作为命名空间参数传递的命名空间中，因为该参数优先于默认符号命名空间。)

A second option to define the default namespace is directly in the compilation
unit as preprocessor statement. The above example would then read::

	#define DEFAULT_SYMBOL_NAMESPACE "USB_COMMON"

(第二种定义默认命名空间的选项是在编译单元中直接以预处理语句的形式定义。上面的例子于是会写成：)

within the corresponding compilation unit before the #include for
<linux/export.h>. Typically it's placed before the first #include statement.
(即在相应编译单元中、针对 <linux/export.h> 的 #include 之前。通常它放在第一条 #include 语句之前。)

Using the EXPORT_SYMBOL_FOR_MODULES() macro(使用 EXPORT_SYMBOL_FOR_MODULES() 宏)
--------------------------------------------------------------------------------

Symbols exported using this macro are put into a module namespace. This
namespace cannot be imported. These exports are GPL-only as they are only
intended for in-tree modules.
(使用此宏导出的符号被放入模块命名空间。该命名空间不能被导入。这些导出仅限 GPL，因为它们只面向内核内 (in-tree) 模块。)

The macro takes a comma separated list of module names, allowing only those
modules to access this symbol. Simple tail-globs are supported.
(此宏接受一个以逗号分隔的模块名列表，只允许这些模块访问该符号。支持简单的尾部通配符 (tail-globs)。)

For example::

  EXPORT_SYMBOL_FOR_MODULES(preempt_notifier_inc, "kvm,kvm-*")

(例如：)

will limit usage of this symbol to modules whose name matches the given
patterns.
(这会把该符号的使用限制在名称匹配给定模式的模块上。)

How to use Symbols exported in Namespaces(如何使用在命名空间中导出的符号)
-------------------------------------------------------------------------

In order to use symbols that are exported into namespaces, kernel modules need
to explicitly import these namespaces. Otherwise the kernel might reject to
load the module. The module code is required to use the macro MODULE_IMPORT_NS
for the namespaces it uses symbols from. E.g. a module using the
usb_stor_suspend symbol from above, needs to import the namespace USB_STORAGE
using a statement like::

	MODULE_IMPORT_NS("USB_STORAGE");

(要使用导出到命名空间中的符号，内核模块需要显式导入这些命名空间。否则内核可能会拒绝加载该模块。模块代码必须使用宏 MODULE_IMPORT_NS 来导入它使用了符号的命名空间。例如，使用上文 usb_stor_suspend 符号的模块，需要用类似如下语句导入命名空间 USB_STORAGE：)

This will create a ``modinfo`` tag in the module for each imported namespace.
This has the side effect, that the imported namespaces of a module can be
inspected with modinfo::

	$ modinfo drivers/usb/storage/ums-karma.ko
	[...]
	import_ns:      USB_STORAGE
	[...]

(这会在模块中为每个导入的命名空间创建一个 ``modinfo`` 标签。其副作用是，模块导入的命名空间可以用 modinfo 查看：)

For modules that are currently loaded, imported namespaces are also available
via sysfs::

	$ cat /sys/module/ums_karma/import_ns
	USB_STORAGE

(对于当前已加载的模块，导入的命名空间也可以通过 sysfs 获得：)

It is advisable to add the MODULE_IMPORT_NS() statement close to other module
metadata definitions like MODULE_AUTHOR() or MODULE_LICENSE().
(建议把 MODULE_IMPORT_NS() 语句放在其他模块元数据定义(如 MODULE_AUTHOR() 或 MODULE_LICENSE())附近。)

Loading Modules that use namespaced Symbols(加载使用命名空间符号的模块)
=======================================================================

At module loading time (e.g. ``insmod``), the kernel will check each symbol
referenced from the module for its availability and whether the namespace it
might be exported to has been imported by the module. The default behaviour of
the kernel is to reject loading modules that don't specify sufficient imports.
An error will be logged and loading will be failed with EINVAL. In order to
allow loading of modules that don't satisfy this precondition, a configuration
option is available: Setting MODULE_ALLOW_MISSING_NAMESPACE_IMPORTS=y will
enable loading regardless, but will emit a warning.
(在模块加载时(例如 ``insmod``)，内核会检查模块引用的每个符号的可用性，以及该符号可能导出到的命名空间是否已被模块导入。内核的默认行为是拒绝加载未指定足够导入的模块。会记录一个错误，加载将以 EINVAL 失败。为了允许加载不满足这一前提条件的模块，有一个配置选项可用：设置 MODULE_ALLOW_MISSING_NAMESPACE_IMPORTS=y 将无条件允许加载，但会发出警告。)

Automatically creating MODULE_IMPORT_NS statements(自动创建 MODULE_IMPORT_NS 语句)
==================================================================================

Missing namespaces imports can easily be detected at build time. In fact,
modpost will emit a warning if a module uses a symbol from a namespace
without importing it.
MODULE_IMPORT_NS() statements will usually be added at a definite location
(along with other module meta data). To make the life of module authors (and
subsystem maintainers) easier, a script and make target is available to fixup
missing imports. Fixing missing imports can be done with::

	$ make nsdeps

(缺失的命名空间导入很容易在构建时检测到。事实上，如果模块使用了某个命名空间中的符号却没有导入它，modpost 会发出警告。 MODULE_IMPORT_NS() 语句通常会被添加到一个确定的位置(与其他模块元数据一起)。为了让模块作者(和子系统维护者)的生活更轻松，提供了一个脚本和 make 目标来修复缺失的导入。修复缺失的导入可以这样做：)

A typical scenario for module authors would be::

	- write code that depends on a symbol from a not imported namespace
	- ``make``
	- notice the warning of modpost telling about a missing import
	- run ``make nsdeps`` to add the import to the correct code location

(对模块作者而言，一个典型的场景会是：编写依赖某个未导入命名空间中符号的代码；运行 ``make`` ；注意到 modpost 关于缺失导入的警告；然后运行 ``make nsdeps`` 把导入添加到正确的代码位置。)

For subsystem maintainers introducing a namespace, the steps are very similar.
Again, ``make nsdeps`` will eventually add the missing namespace imports for
in-tree modules::

	- move or add symbols to a namespace (e.g. with EXPORT_SYMBOL_NS())
	- ``make`` (preferably with an allmodconfig to cover all in-kernel
	  modules)
	- notice the warning of modpost telling about a missing import
	- run ``make nsdeps`` to add the import to the correct code location

(对于引入命名空间的子系统维护者来说，步骤非常相似：将符号移动或添加到某个命名空间中(例如用 EXPORT_SYMBOL_NS())；运行 ``make`` (最好使用 allmodconfig 以覆盖所有内核内模块)；注意到 modpost 关于缺失导入的警告；然后运行 ``make nsdeps`` 把导入添加到正确的代码位置。同样， ``make nsdeps`` 最终会为内核内模块添加缺失的命名空间导入。)

You can also run nsdeps for external module builds. A typical usage is::

	$ make -C <path_to_kernel_src> M=$PWD nsdeps

(也可以为外部模块构建运行 nsdeps。典型用法是：)

Note: it will happily generate an import statement for the module namespace;
which will not work and generates build and runtime failures.
(注意：它也会很乐意地为模块命名空间生成导入语句；这不会起作用，并会导致构建和运行时失败。)
