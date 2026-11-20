======================================================================
The object-lifetime debugging infrastructure(对象生命周期调试基础设施)
======================================================================

:Author: Thomas Gleixner
(作者： Thomas Gleixner)

Introduction(引言)
==================

debugobjects is a generic infrastructure to track the life time of
kernel objects and validate the operations on those.
(debugobjects 是一个通用基础设施，用于跟踪内核对象的生命周期并验证对它们的操作。)

debugobjects is useful to check for the following error patterns:
(debugobjects 可用于检查以下错误模式：)

-  Activation of uninitialized objects

-  Initialization of active objects

-  Usage of freed/destroyed objects
(上述三项分别是：激活未初始化的对象；初始化处于活动状态的对象；使用已释放/已销毁的对象。)

debugobjects is not changing the data structure of the real object so it
can be compiled in with a minimal runtime impact and enabled on demand
with a kernel command line option.
(debugobjects 不会改变真实对象的数据结构，因此可以在运行时影响极小的情况下编译进内核，并通过内核命令行选项按需启用。)

Howto use debugobjects(如何使用 debugobjects)
=============================================

A kernel subsystem needs to provide a data structure which describes the
object type and add calls into the debug code at appropriate places. The
data structure to describe the object type needs at minimum the name of
the object type. Optional functions can and should be provided to fixup
detected problems so the kernel can continue to work and the debug
information can be retrieved from a live system instead of hard core
debugging with serial consoles and stack trace transcripts from the
monitor.
(内核子系统需要提供一个描述对象类型的数据结构，并在适当的位置加入对调试代码的调用。描述对象类型的数据结构至少需要对象类型的名称。可以并且应当提供可选的函数来修复检测到的问题，使内核能够继续运行，并可以从运行中的系统获取调试信息，而不是靠串口控制台和监视器上的栈回溯记录进行硬核调试。)

The debug calls provided by debugobjects are:
(debugobjects 提供的调试调用有：)

-  debug_object_init

-  debug_object_init_on_stack

-  debug_object_activate

-  debug_object_deactivate

-  debug_object_destroy

-  debug_object_free

-  debug_object_assert_init
(以上分别是：对象初始化、栈上对象初始化、对象激活、对象去激活、对象销毁、对象释放、断言对象已初始化。)

Each of these functions takes the address of the real object and a
pointer to the object type specific debug description structure.
(每个函数都接受真实对象的地址，以及指向该对象类型专属调试描述结构的指针。)

Each detected error is reported in the statistics and a limited number
of errors are printk'ed including a full stack trace.
(每个检测到的错误都会计入统计信息，并且有限数量的错误会被 printk 出来，附带完整的栈回溯。)

The statistics are available via /sys/kernel/debug/debug_objects/stats.
They provide information about the number of warnings and the number of
successful fixups along with information about the usage of the internal
tracking objects and the state of the internal tracking objects pool.
(统计信息可通过 /sys/kernel/debug/debug_objects/stats 获得。它们提供警告数量与成功修复数量的信息，以及内部跟踪对象的使用情况和内部跟踪对象池的状态。)

Debug functions(调试函数)
=========================

.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_init

This function is called whenever the initialization function of a real
object is called.
(每当真实对象的初始化函数被调用时，就会调用该函数。)

When the real object is already tracked by debugobjects it is checked,
whether the object can be initialized. Initializing is not allowed for
active and destroyed objects. When debugobjects detects an error, then
it calls the fixup_init function of the object type description
structure if provided by the caller. The fixup function can correct the
problem before the real initialization of the object happens. E.g. it
can deactivate an active object in order to prevent damage to the
subsystem.
(当真实对象已被 debugobjects 跟踪时，会检查该对象是否可以初始化。对处于活动状态和已销毁状态的对象不允许初始化。当 debugobjects 检测到错误时，如果调用方提供了对象类型描述结构中的 fixup_init 函数，就会调用它。修复函数可以在对象真正初始化发生之前纠正问题。例如，它可以对活动对象执行去激活，以避免对子系统造成损害。)

When the real object is not yet tracked by debugobjects, debugobjects
allocates a tracker object for the real object and sets the tracker
object state to ODEBUG_STATE_INIT. It verifies that the object is not
on the callers stack. If it is on the callers stack then a limited
number of warnings including a full stack trace is printk'ed. The
calling code must use debug_object_init_on_stack() and remove the
object before leaving the function which allocated it. See next section.
(当真实对象尚未被 debugobjects 跟踪时，debugobjects 为真实对象分配一个跟踪器对象，并把跟踪器对象的状态设为 ODEBUG_STATE_INIT。它会验证该对象不在调用者的栈上。如果对象在调用者的栈上，则会 printk 有限数量的警告并附带完整的栈回溯。调用代码必须使用 debug_object_init_on_stack()，并在离开分配该对象的函数之前移除该对象。参见下一节。)

.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_init_on_stack

This function is called whenever the initialization function of a real
object which resides on the stack is called.
(每当位于栈上的真实对象的初始化函数被调用时，就会调用该函数。)

When the real object is already tracked by debugobjects it is checked,
whether the object can be initialized. Initializing is not allowed for
active and destroyed objects. When debugobjects detects an error, then
it calls the fixup_init function of the object type description
structure if provided by the caller. The fixup function can correct the
problem before the real initialization of the object happens. E.g. it
can deactivate an active object in order to prevent damage to the
subsystem.
(当真实对象已被 debugobjects 跟踪时，会检查该对象是否可以初始化。对处于活动状态和已销毁状态的对象不允许初始化。当 debugobjects 检测到错误时，如果调用方提供了对象类型描述结构中的 fixup_init 函数，就会调用它。修复函数可以在对象真正初始化发生之前纠正问题。例如，它可以对活动对象执行去激活，以避免对子系统造成损害。)

When the real object is not yet tracked by debugobjects debugobjects
allocates a tracker object for the real object and sets the tracker
object state to ODEBUG_STATE_INIT. It verifies that the object is on
the callers stack.
(当真实对象尚未被 debugobjects 跟踪时，debugobjects 为真实对象分配一个跟踪器对象，把跟踪器对象的状态设为 ODEBUG_STATE_INIT，并验证该对象位于调用者的栈上。)

An object which is on the stack must be removed from the tracker by
calling debug_object_free() before the function which allocates the
object returns. Otherwise we keep track of stale objects.
(栈上的对象必须在分配它的函数返回之前，通过调用 debug_object_free() 从跟踪器中移除。否则我们会继续跟踪已过期的对象。)

.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_activate

This function is called whenever the activation function of a real
object is called.
(每当真实对象的激活函数被调用时，就会调用该函数。)

When the real object is already tracked by debugobjects it is checked,
whether the object can be activated. Activating is not allowed for
active and destroyed objects. When debugobjects detects an error, then
it calls the fixup_activate function of the object type description
structure if provided by the caller. The fixup function can correct the
problem before the real activation of the object happens. E.g. it can
deactivate an active object in order to prevent damage to the subsystem.
(当真实对象已被 debugobjects 跟踪时，会检查该对象是否可以激活。对处于活动状态和已销毁状态的对象不允许激活。当 debugobjects 检测到错误时，如果调用方提供了对象类型描述结构中的 fixup_activate 函数，就会调用它。修复函数可以在对象真正激活发生之前纠正问题。例如，它可以对活动对象执行去激活，以避免对子系统造成损害。)

When the real object is not yet tracked by debugobjects then the
fixup_activate function is called if available. This is necessary to
allow the legitimate activation of statically allocated and initialized
objects. The fixup function checks whether the object is valid and calls
the debug_objects_init() function to initialize the tracking of this
object.
(当真实对象尚未被 debugobjects 跟踪时，若 fixup_activate 函数可用则调用它。这样做是为了允许对静态分配并初始化的对象进行合法激活。修复函数检查对象是否有效，并调用 debug_objects_init() 函数来初始化对该对象的跟踪。)

When the activation is legitimate, then the state of the associated
tracker object is set to ODEBUG_STATE_ACTIVE.
(当激活合法时，关联跟踪器对象的状态被设为 ODEBUG_STATE_ACTIVE。)


.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_deactivate

This function is called whenever the deactivation function of a real
object is called.
(每当真实对象的去激活函数被调用时，就会调用该函数。)

When the real object is tracked by debugobjects it is checked, whether
the object can be deactivated. Deactivating is not allowed for untracked
or destroyed objects.
(当真实对象被 debugobjects 跟踪时，会检查该对象是否可以去激活。对未被跟踪或已销毁的对象不允许去激活。)

When the deactivation is legitimate, then the state of the associated
tracker object is set to ODEBUG_STATE_INACTIVE.
(当去激活合法时，关联跟踪器对象的状态被设为 ODEBUG_STATE_INACTIVE。)

.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_destroy

This function is called to mark an object destroyed. This is useful to
prevent the usage of invalid objects, which are still available in
memory: either statically allocated objects or objects which are freed
later.
(调用该函数把对象标记为已销毁。这有助于防止使用仍然存在于内存中的无效对象：包括静态分配的对象和稍后才释放的对象。)

When the real object is tracked by debugobjects it is checked, whether
the object can be destroyed. Destruction is not allowed for active and
destroyed objects. When debugobjects detects an error, then it calls the
fixup_destroy function of the object type description structure if
provided by the caller. The fixup function can correct the problem
before the real destruction of the object happens. E.g. it can
deactivate an active object in order to prevent damage to the subsystem.
(当真实对象被 debugobjects 跟踪时，会检查该对象是否可以销毁。对处于活动状态和已销毁状态的对象不允许销毁。当 debugobjects 检测到错误时，如果调用方提供了对象类型描述结构中的 fixup_destroy 函数，就会调用它。修复函数可以在对象真正销毁发生之前纠正问题。例如，它可以对活动对象执行去激活，以避免对子系统造成损害。)

When the destruction is legitimate, then the state of the associated
tracker object is set to ODEBUG_STATE_DESTROYED.
(当销毁合法时，关联跟踪器对象的状态被设为 ODEBUG_STATE_DESTROYED。)

.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_free

This function is called before an object is freed.
(在对象被释放之前调用该函数。)

When the real object is tracked by debugobjects it is checked, whether
the object can be freed. Free is not allowed for active objects. When
debugobjects detects an error, then it calls the fixup_free function of
the object type description structure if provided by the caller. The
fixup function can correct the problem before the real free of the
object happens. E.g. it can deactivate an active object in order to
prevent damage to the subsystem.
(当真实对象被 debugobjects 跟踪时，会检查该对象是否可以释放。对处于活动状态的对象不允许释放。当 debugobjects 检测到错误时，如果调用方提供了对象类型描述结构中的 fixup_free 函数，就会调用它。修复函数可以在对象真正释放发生之前纠正问题。例如，它可以对活动对象执行去激活，以避免对子系统造成损害。)

Note that debug_object_free removes the object from the tracker. Later
usage of the object is detected by the other debug checks.
(注意，debug_object_free 会把对象从跟踪器中移除。此后对该对象的使用会被其他调试检查检测到。)


.. kernel-doc:: lib/debugobjects.c
   :functions: debug_object_assert_init

This function is called to assert that an object has been initialized.
(调用该函数以断言对象已被初始化。)

When the real object is not tracked by debugobjects, it calls
fixup_assert_init of the object type description structure provided by
the caller, with the hardcoded object state ODEBUG_NOT_AVAILABLE. The
fixup function can correct the problem by calling debug_object_init
and other specific initializing functions.
(当真实对象未被 debugobjects 跟踪时，它会以硬编码的对象状态 ODEBUG_NOT_AVAILABLE 调用调用方提供的对象类型描述结构中的 fixup_assert_init。修复函数可以通过调用 debug_object_init 及其他特定的初始化函数来纠正问题。)

When the real object is already tracked by debugobjects it is ignored.
(当真实对象已被 debugobjects 跟踪时，则被忽略。)

Fixup functions(修复函数)
=========================

Debug object type description structure(调试对象类型描述结构体)
---------------------------------------------------------------

.. kernel-doc:: include/linux/debugobjects.h
   :internal:

fixup_init(初始化修复)
----------------------

This function is called from the debug code whenever a problem in
debug_object_init is detected. The function takes the address of the
object and the state which is currently recorded in the tracker.
(每当在 debug_object_init 中检测到问题时，调试代码就会调用该函数。该函数接受对象的地址以及跟踪器中当前记录的状态。)

Called from debug_object_init when the object state is:
(当对象状态如下时由 debug_object_init 调用：)

-  ODEBUG_STATE_ACTIVE

The function returns true when the fixup was successful, otherwise
false. The return value is used to update the statistics.
(修复成功时该函数返回 true，否则返回 false。返回值用于更新统计信息。)

Note, that the function needs to call the debug_object_init() function
again, after the damage has been repaired in order to keep the state
consistent.
(注意，在损害修复之后，该函数需要再次调用 debug_object_init() 函数，以保持状态一致。)

fixup_activate(激活修复)
------------------------

This function is called from the debug code whenever a problem in
debug_object_activate is detected.
(每当在 debug_object_activate 中检测到问题时，调试代码就会调用该函数。)

Called from debug_object_activate when the object state is:
(当对象状态如下时由 debug_object_activate 调用：)

-  ODEBUG_STATE_NOTAVAILABLE

-  ODEBUG_STATE_ACTIVE

The function returns true when the fixup was successful, otherwise
false. The return value is used to update the statistics.
(修复成功时该函数返回 true，否则返回 false。返回值用于更新统计信息。)

Note that the function needs to call the debug_object_activate()
function again after the damage has been repaired in order to keep the
state consistent.
(注意，在损害修复之后，该函数需要再次调用 debug_object_activate() 函数，以保持状态一致。)

The activation of statically initialized objects is a special case. When
debug_object_activate() has no tracked object for this object address
then fixup_activate() is called with object state
ODEBUG_STATE_NOTAVAILABLE. The fixup function needs to check whether
this is a legitimate case of a statically initialized object or not. In
case it is it calls debug_object_init() and debug_object_activate()
to make the object known to the tracker and marked active. In this case
the function should return false because this is not a real fixup.
(静态初始化对象的激活是一种特殊情况。当 debug_object_activate() 对该对象地址没有跟踪对象时，会以对象状态 ODEBUG_STATE_NOTAVAILABLE 调用 fixup_activate()。修复函数需要检查这是否为静态初始化对象的合法情形。如果是，则调用 debug_object_init() 与 debug_object_activate()，使跟踪器知晓该对象并将其标记为活动。这种情况下该函数应返回 false，因为这并不是一次真正的修复。)

fixup_destroy(销毁修复)
-----------------------

This function is called from the debug code whenever a problem in
debug_object_destroy is detected.
(每当在 debug_object_destroy 中检测到问题时，调试代码就会调用该函数。)

Called from debug_object_destroy when the object state is:
(当对象状态如下时由 debug_object_destroy 调用：)

-  ODEBUG_STATE_ACTIVE

The function returns true when the fixup was successful, otherwise
false. The return value is used to update the statistics.
(修复成功时该函数返回 true，否则返回 false。返回值用于更新统计信息。)

fixup_free(释放修复)
--------------------

This function is called from the debug code whenever a problem in
debug_object_free is detected. Further it can be called from the debug
checks in kfree/vfree, when an active object is detected from the
debug_check_no_obj_freed() sanity checks.
(每当在 debug_object_free 中检测到问题时，调试代码就会调用该函数。此外，当 kfree/vfree 中的调试检查通过 debug_check_no_obj_freed() 健全性检查检测到活动对象时，也会调用该函数。)

Called from debug_object_free() or debug_check_no_obj_freed() when
the object state is:
(当对象状态如下时由 debug_object_free() 或 debug_check_no_obj_freed() 调用：)

-  ODEBUG_STATE_ACTIVE

The function returns true when the fixup was successful, otherwise
false. The return value is used to update the statistics.
(修复成功时该函数返回 true，否则返回 false。返回值用于更新统计信息。)

fixup_assert_init(断言初始化修复)
---------------------------------

This function is called from the debug code whenever a problem in
debug_object_assert_init is detected.
(每当在 debug_object_assert_init 中检测到问题时，调试代码就会调用该函数。)

Called from debug_object_assert_init() with a hardcoded state
ODEBUG_STATE_NOTAVAILABLE when the object is not found in the debug
bucket.
(当在调试桶（debug bucket）中找不到对象时，由 debug_object_assert_init() 以硬编码状态 ODEBUG_STATE_NOTAVAILABLE 调用。)

The function returns true when the fixup was successful, otherwise
false. The return value is used to update the statistics.
(修复成功时该函数返回 true，否则返回 false。返回值用于更新统计信息。)

Note, this function should make sure debug_object_init() is called
before returning.
(注意，该函数应当确保在返回之前已调用 debug_object_init()。)

The handling of statically initialized objects is a special case. The
fixup function should check if this is a legitimate case of a statically
initialized object or not. In this case only debug_object_init()
should be called to make the object known to the tracker. Then the
function should return false because this is not a real fixup.
(对静态初始化对象的处理是一种特殊情况。修复函数应检查这是否为静态初始化对象的合法情形。如果是，则只应调用 debug_object_init() 使跟踪器知晓该对象，然后该函数应返回 false，因为这并不是一次真正的修复。)

Known Bugs And Assumptions(已知缺陷与假设)
==========================================

None (knock on wood).
(无（但愿如此）。)
