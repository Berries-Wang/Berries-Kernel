.. _todo:

===================
TODO list(待办清单)
===================

This section contains a list of smaller janitorial tasks in the kernel DRM
graphics subsystem useful as newbie projects. Or for slow rainy days.
(本节列出了内核 DRM 图形子系统中一些较小的事务性任务，适合作为新手练手的项目，或者供下雨天的悠闲时光慢慢打磨。)

Difficulty(难度分级)
--------------------

To make it easier task are categorized into different levels:
(为了便于选择，任务被划分为不同的难度级别：)

Starter: Good tasks to get started with the DRM subsystem.
(Starter：适合入门 DRM 子系统的好任务。)

Intermediate: Tasks which need some experience with working in the DRM
subsystem, or some specific GPU/display graphics knowledge. For debugging issue
it's good to have the relevant hardware (or a virtual driver set up) available
for testing.
(Intermediate：需要一些在 DRM 子系统中工作的经验，或者某些特定的 GPU/显示图形知识的任务。为了调试问题，最好能有相关硬件(或搭建好的虚拟驱动)可供测试。)

Advanced: Tricky tasks that need fairly good understanding of the DRM subsystem
and graphics topics. Generally need the relevant hardware for development and
testing.
(Advanced：需要对 DRM 子系统和图形主题有相当深入理解的棘手任务。通常需要相关硬件来进行开发和测试。)

Expert: Only attempt these if you've successfully completed some tricky
refactorings already and are an expert in the specific area
(Expert：只有在已经成功完成过一些棘手的重构、并且是该特定领域的专家之后，才应尝试这些任务。)

Subsystem-wide refactorings(子系统级重构)
=========================================

Open-code drm_simple_encoder_init()(手工展开 drm_simple_encoder_init())
-----------------------------------------------------------------------

The helper drm_simple_encoder_init() was supposed to simplify encoder
initialization. Instead it only added an intermediate layer between atomic
modesetting and the DRM driver.
(辅助函数 drm_simple_encoder_init() 本应简化 encoder 的初始化，结果却只是在 atomic 模式设置与 DRM 驱动之间又增加了一个中间层。)

The task here is to remove drm_simple_encoder_init(). Search for a driver
that calls drm_simple_encoder_init() and inline the helper. The driver will
also need its own instance of drm_encoder_funcs.
(这里的任务是移除 drm_simple_encoder_init()。找到一个调用了 drm_simple_encoder_init() 的驱动，把该辅助函数手工展开内联进去。驱动还需要拥有自己的 drm_encoder_funcs 实例。)

Contact: Thomas Zimmermann, respective driver maintainer
(联系人：Thomas Zimmermann，相应驱动维护者。)

Level: Easy
(难度：简单。)

Replace struct drm_simple_display_pipe with regular atomic helpers(用常规 atomic 辅助函数替换 struct drm_simple_display_pipe)
-----------------------------------------------------------------------------------------------------------------------------

The data type struct drm_simple_display_pipe and its helpers were supposed
to simplify driver development. Instead they only added an intermediate layer
between atomic modesetting and the DRM driver.
(数据类型 struct drm_simple_display_pipe 及其辅助函数本应简化驱动开发，结果却只是在 atomic 模式设置与 DRM 驱动之间又增加了一个中间层。)

There are still drivers that use drm_simple_display_pipe. The task here is to
convert them to use regular atomic helpers. Search for a driver that calls
drm_simple_display_pipe_init() and inline all helpers from drm_simple_kms_helper.c
into the driver, such that no simple-KMS interfaces are required. Please also
rename all inlined fucntions according to driver conventions.
(目前仍有驱动在使用 drm_simple_display_pipe。这里的任务是把它们转换为使用常规的 atomic 辅助函数。找到调用 drm_simple_display_pipe_init() 的驱动，把 drm_simple_kms_helper.c 中的所有辅助函数内联到驱动中，使其不再需要任何 simple-KMS 接口。同时请按照驱动自身的命名约定重命名所有内联后的函数。)

Contact: Thomas Zimmermann, respective driver maintainer
(联系人：Thomas Zimmermann，相应驱动维护者。)

Level: Easy
(难度：简单。)

Remove custom dumb_map_offset implementations(移除自定义的 dumb_map_offset 实现)
--------------------------------------------------------------------------------

All GEM based drivers should be using drm_gem_create_mmap_offset() instead.
Audit each individual driver, make sure it'll work with the generic
implementation (there's lots of outdated locking leftovers in various
implementations), and then remove it.
(所有基于 GEM 的驱动都应改用 drm_gem_create_mmap_offset()。逐个审查每一份驱动，确认它能与通用实现协同工作(各种实现中残留着大量过时的加锁代码)，然后移除自定义实现。)

Contact: Simona Vetter, respective driver maintainers
(联系人：Simona Vetter，相应驱动维护者。)

Level: Intermediate
(难度：中等。)

Convert existing KMS drivers to atomic modesetting(把现有 KMS 驱动转换为 atomic 模式设置)
-----------------------------------------------------------------------------------------

3.19 has the atomic modeset interfaces and helpers, so drivers can now be
converted over. Modern compositors like Wayland or Surfaceflinger on Android
really want an atomic modeset interface, so this is all about the bright
future.
(3.19 已经提供了 atomic 模式设置接口和辅助函数，因此现在可以把驱动转换过来了。像 Wayland 或 Android 上的 Surfaceflinger 这样的现代合成器非常需要 atomic 模式设置接口，所以这项工作关乎光明的未来。)

There is a conversion guide for atomic [1]_ and all you need is a GPU for a
non-converted driver.  The "Atomic mode setting design overview" series [2]_
[3]_ at LWN.net can also be helpful.
(这里有一份 atomic 转换指南 [1]_，你只需要一块运行着尚未转换驱动的 GPU。"Atomic mode setting design overview" 系列 [2]_ [3]_ (LWN.net 上)也会有帮助。)

As part of this drivers also need to convert to universal plane (which means
exposing primary & cursor as proper plane objects). But that's much easier to
do by directly using the new atomic helper driver callbacks.
(作为转换的一部分，驱动还需要转换为 universal plane(即把 primary 与 cursor 作为正式的 plane 对象暴露出来)。不过直接使用新的 atomic 辅助函数驱动回调来做这件事要容易得多。)

  .. [1] https://blog.ffwll.ch/2014/11/atomic-modeset-support-for-kms-drivers.html
  .. [2] https://lwn.net/Articles/653071/
  .. [3] https://lwn.net/Articles/653466/

Contact: Simona Vetter, respective driver maintainers
(联系人：Simona Vetter，相应驱动维护者。)

Level: Advanced
(难度：高级。)

Clean up the clipped coordination confusion around planes(清理 plane 相关裁剪坐标的混乱)
----------------------------------------------------------------------------------------

We have a helper to get this right with drm_plane_helper_check_update(), but
it's not consistently used. This should be fixed, preferably in the atomic
helpers (and drivers then moved over to clipped coordinates). Probably the
helper should also be moved from drm_plane_helper.c to the atomic helpers, to
avoid confusion - the other helpers in that file are all deprecated legacy
helpers.
(我们有 drm_plane_helper_check_update() 这个辅助函数来把事情做对，但它并未被一致地使用。这个问题应当修复，最好是在 atomic 辅助函数中修复(然后让驱动迁移到裁剪坐标)。也许还应把这个辅助函数从 drm_plane_helper.c 移到 atomic 辅助函数中去，以避免混淆——该文件中的其他辅助函数全都是已废弃的遗留辅助函数。)

Contact: Ville Syrjälä, Simona Vetter, driver maintainers
(联系人：Ville Syrjälä、Simona Vetter，驱动维护者。)

Level: Advanced
(难度：高级。)

Improve plane atomic_check helpers(改进 plane 的 atomic_check 辅助函数)
-----------------------------------------------------------------------

Aside from the clipped coordinates right above there's a few suboptimal things
with the current helpers:
(除了上面提到的裁剪坐标问题之外，当前的辅助函数还有一些不够理想之处：)

- drm_plane_helper_funcs->atomic_check gets called for enabled or disabled
  planes. At best this seems to confuse drivers, worst it means they blow up
  when the plane is disabled without the CRTC. The only special handling is
  resetting values in the plane state structures, which instead should be moved
  into the drm_plane_funcs->atomic_duplicate_state functions.
  (无论 plane 处于启用还是禁用状态，drm_plane_helper_funcs->atomic_check 都会被调用。往好了说这让驱动感到困惑，往坏了说这意味着当 plane 在没有 CRTC 的情况下被禁用时驱动会崩溃。目前唯一的特殊处理是重置 plane 状态结构体中的值，而这本应移到 drm_plane_funcs->atomic_duplicate_state 函数中去。)

- Once that's done, helpers could stop calling ->atomic_check for disabled
  planes.
  (完成之后，辅助函数就可以不再对禁用的 plane 调用 ->atomic_check 了。)

- Then we could go through all the drivers and remove the more-or-less confused
  checks for plane_state->fb and plane_state->crtc.
  (然后我们可以遍历所有驱动，移除那些多少有些混乱的 plane_state->fb 和 plane_state->crtc 检查。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Advanced
(难度：高级。)

Convert early atomic drivers to async commit helpers(把早期 atomic 驱动转换为异步提交辅助函数)
----------------------------------------------------------------------------------------------

For the first year the atomic modeset helpers didn't support asynchronous /
nonblocking commits, and every driver had to hand-roll them. This is fixed
now, but there's still a pile of existing drivers that easily could be
converted over to the new infrastructure.
(在第一年里，atomic 模式设置辅助函数并不支持异步/非阻塞提交，每个驱动都不得不自己手工实现。这个问题现在已经修复，但仍有一批现有驱动可以轻松迁移到新的基础设施上。)

One issue with the helpers is that they require that drivers handle completion
events for atomic commits correctly. But fixing these bugs is good anyway.
(这些辅助函数的一个问题是，它们要求驱动正确处理 atomic 提交的完成事件。不过无论如何，修复这些 bug 总是好事。)

Somewhat related is the legacy_cursor_update hack, which should be replaced with
the new atomic_async_check/commit functionality in the helpers in drivers that
still look at that flag.
(与此多少相关的是 legacy_cursor_update 这个权宜之计，在仍在检查该标志的驱动中，应把它替换为辅助函数中新的 atomic_async_check/commit 功能。)

Contact: Simona Vetter, respective driver maintainers
(联系人：Simona Vetter，相应驱动维护者。)

Level: Advanced
(难度：高级。)

Rename drm_atomic_state(重命名 drm_atomic_state)
------------------------------------------------

The KMS framework uses two slightly different definitions for the ``state``
concept. For a given object (plane, CRTC, encoder, etc., so
``drm_$OBJECT_state``), the state is the entire state of that object. However,
at the device level, ``drm_atomic_state`` refers to a state update for a
limited number of objects.
(KMS 框架对 ``state`` 概念有两个略有不同的定义。对于给定对象(plane、CRTC、encoder 等，即 ``drm_$OBJECT_state``)，state 是该对象的完整状态。然而在设备层面，``drm_atomic_state`` 指的却是对有限数量对象的一次状态更新。)

The state isn't the entire device state, but only the full state of some
objects in that device. This is confusing to newcomers, and
``drm_atomic_state`` should be renamed to something clearer like
``drm_atomic_commit``.
(这里的 state 并不是整个设备的状态，而只是该设备中某些对象的完整状态。这会让新手感到困惑，``drm_atomic_state`` 应重命名为更清晰的名字，比如 ``drm_atomic_commit``。)

In addition to renaming the structure itself, it would also imply renaming some
related functions (``drm_atomic_state_alloc``, ``drm_atomic_state_get``,
``drm_atomic_state_put``, ``drm_atomic_state_init``,
``__drm_atomic_state_free``, etc.).
(除了重命名结构体本身之外，还意味着要重命名一些相关函数(``drm_atomic_state_alloc``、``drm_atomic_state_get``、``drm_atomic_state_put``、``drm_atomic_state_init``、``__drm_atomic_state_free`` 等)。)

Contact: Maxime Ripard <mripard@kernel.org>
(联系人：Maxime Ripard <mripard@kernel.org>。)

Level: Advanced
(难度：高级。)

Fallout from atomic KMS(atomic KMS 的连带收尾)
----------------------------------------------

``drm_atomic_helper.c`` provides a batch of functions which implement legacy
IOCTLs on top of the new atomic driver interface. Which is really nice for
gradual conversion of drivers, but unfortunately the semantic mismatches are
a bit too severe. So there's some follow-up work to adjust the function
interfaces to fix these issues:
(``drm_atomic_helper.c`` 提供了一组函数，在新的 atomic 驱动接口之上实现传统 IOCTL。这对于驱动的渐进式转换非常有用，但遗憾的是语义上的不匹配有些过于严重。因此还需要一些后续工作来调整这些函数接口，以修复这些问题：)

* atomic needs the lock acquire context. At the moment that's passed around
  implicitly with some horrible hacks, and it's also allocate with
  ``GFP_NOFAIL`` behind the scenes. All legacy paths need to start allocating
  the acquire context explicitly on stack and then also pass it down into
  drivers explicitly so that the legacy-on-atomic functions can use them.
  (atomic 需要锁获取上下文。目前它是通过一些可怕的 hack 隐式传递的，而且在幕后还用 ``GFP_NOFAIL`` 来分配。所有传统路径都需要改为在栈上显式分配获取上下文，然后再把它显式地传递给驱动，这样 legacy-on-atomic 函数才能使用它们。)

  Except for some driver code this is done. This task should be finished by
  adding WARN_ON(!drm_drv_uses_atomic_modeset) in drm_modeset_lock_all().
  (除了一些驱动代码外，这件事已经完成。这个任务应通过在 drm_modeset_lock_all() 中添加 WARN_ON(!drm_drv_uses_atomic_modeset) 来收尾。)

* A bunch of the vtable hooks are now in the wrong place: DRM has a split
  between core vfunc tables (named ``drm_foo_funcs``), which are used to
  implement the userspace ABI. And then there's the optional hooks for the
  helper libraries (name ``drm_foo_helper_funcs``), which are purely for
  internal use. Some of these hooks should be move from ``_funcs`` to
  ``_helper_funcs`` since they are not part of the core ABI. There's a
  ``FIXME`` comment in the kerneldoc for each such case in ``drm_crtc.h``.
  (有一批虚函数表钩子现在放错了位置：DRM 把核心 vfunc 表(名为 ``drm_foo_funcs``，用于实现用户空间 ABI)与辅助库的可选钩子(名为 ``drm_foo_helper_funcs``，纯粹供内部使用)分开。其中一些钩子应该从 ``_funcs`` 移到 ``_helper_funcs``，因为它们不属于核心 ABI。``drm_crtc.h`` 的 kerneldoc 中对每种此类情况都有 ``FIXME`` 注释。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Intermediate
(难度：中等。)

Move Buffer Object Locking to dma_resv_lock()(把缓冲对象加锁迁移到 dma_resv_lock())
-----------------------------------------------------------------------------------

Many drivers have their own per-object locking scheme, usually using
mutex_lock(). This causes all kinds of trouble for buffer sharing, since
depending which driver is the exporter and importer, the locking hierarchy is
reversed.
(许多驱动都有自己的按对象加锁方案，通常使用 mutex_lock()。这给缓冲区共享带来了各种麻烦，因为随着哪一方是导出方、哪一方是导入方的不同，加锁层次可能正好颠倒。)

To solve this we need one standard per-object locking mechanism, which is
dma_resv_lock(). This lock needs to be called as the outermost lock, with all
other driver specific per-object locks removed. The problem is that rolling out
the actual change to the locking contract is a flag day, due to struct dma_buf
buffer sharing.
(为了解决这个问题，我们需要一种标准的按对象加锁机制，也就是 dma_resv_lock()。这个锁必须作为最外层锁来调用，同时移除所有其他驱动特定的按对象锁。问题在于，由于 struct dma_buf 缓冲区共享的存在，真正推行加锁契约的变更是一个"一刀切"的大迁移。)

Level: Expert
(难度：专家。)

Convert logging to drm_* functions with drm_device parameter(把日志转换为带 drm_device 参数的 drm_* 函数)
---------------------------------------------------------------------------------------------------------

For drivers which could have multiple instances, it is necessary to
differentiate between which is which in the logs. Since DRM_INFO/WARN/ERROR
don't do this, drivers used dev_info/warn/err to make this differentiation. We
now have drm_* variants of the drm print functions, so we can start to convert
those drivers back to using drm-formatted specific log messages.
(对于可能存在多个实例的驱动，有必要在日志中区分到底是谁在输出。由于 DRM_INFO/WARN/ERROR 做不到这一点，驱动们改用 dev_info/warn/err 来作区分。现在 drm 打印函数已经有了 drm_* 变体，所以我们可以开始把这些驱动转回使用 drm 格式化的专用日志消息。)

Before you start this conversion please contact the relevant maintainers to make
sure your work will be merged - not everyone agrees that the DRM dmesg macros
are better.
(在开始这项转换之前，请先联系相关维护者，确保你的工作会被合并——并不是所有人都认为 DRM 的 dmesg 宏更好。)

Contact: Sean Paul, Maintainer of the driver you plan to convert
(联系人：Sean Paul，你计划转换的驱动的维护者。)

Level: Starter
(难度：入门。)

Convert drivers to use simple modeset suspend/resume(把驱动转换为使用简单的 modeset 挂起/恢复)
----------------------------------------------------------------------------------------------

Most drivers (except i915 and nouveau) that use
drm_atomic_helper_suspend/resume() can probably be converted to use
drm_mode_config_helper_suspend/resume(). Also there's still open-coded version
of the atomic suspend/resume code in older atomic modeset drivers.
(使用 drm_atomic_helper_suspend/resume() 的大多数驱动(i915 和 nouveau 除外)大概都可以转换为使用 drm_mode_config_helper_suspend/resume()。此外，较老的 atomic 模式设置驱动中仍有手工展开的 atomic 挂起/恢复代码。)

Contact: Maintainer of the driver you plan to convert
(联系人：你计划转换的驱动的维护者。)

Level: Intermediate
(难度：中等。)

Reimplement functions in drm_fbdev_fb_ops without fbdev(脱离 fbdev 重新实现 drm_fbdev_fb_ops 中的函数)
------------------------------------------------------------------------------------------------------

A number of callback functions in drm_fbdev_fb_ops could benefit from
being rewritten without dependencies on the fbdev module. Some of the
helpers could further benefit from using struct iosys_map instead of
raw pointers.
(drm_fbdev_fb_ops 中有不少回调函数若能在不依赖 fbdev 模块的情况下重写将会受益。其中一些辅助函数进一步还可以改用 struct iosys_map 而不是裸指针。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>, Simona Vetter
(联系人：Thomas Zimmermann <tzimmermann@suse.de>、Simona Vetter。)

Level: Advanced
(难度：高级。)

Benchmark and optimize blitting and format-conversion function(对位块传输与格式转换函数进行基准测试与优化)
----------------------------------------------------------------------------------------------------------

Drawing to display memory quickly is crucial for many applications'
performance.
(能否快速绘制到显示内存对许多应用的性能至关重要。)

On at least x86-64, sys_imageblit() is significantly slower than
cfb_imageblit(), even though both use the same blitting algorithm and
the latter is written for I/O memory. It turns out that cfb_imageblit()
uses movl instructions, while sys_imageblit apparently does not. This
seems to be a problem with gcc's optimizer. DRM's format-conversion
helpers might be subject to similar issues.
(至少在 x86-64 上，sys_imageblit() 明显慢于 cfb_imageblit()，尽管两者使用相同的位块传输算法，而且后者还是为 I/O 内存编写的。事实证明 cfb_imageblit() 使用了 movl 指令，而 sys_imageblit 显然没有。这似乎是 gcc 优化器的问题。DRM 的格式转换辅助函数可能也存在类似问题。)

Benchmark and optimize fbdev's sys_() helpers and DRM's format-conversion
helpers. In cases that can be further optimized, maybe implement a different
algorithm. For micro-optimizations, use movl/movq instructions explicitly.
That might possibly require architecture-specific helpers (e.g., storel()
storeq()).
(对 fbdev 的 sys_() 辅助函数和 DRM 的格式转换辅助函数进行基准测试和优化。在还能进一步优化的场景中，或许可以换一种算法实现。对于微优化，可显式使用 movl/movq 指令。这可能需要架构特定的辅助函数(例如 storel()、storeq())。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>
(联系人：Thomas Zimmermann <tzimmermann@suse.de>。)

Level: Intermediate
(难度：中等。)

drm_framebuffer_funcs and drm_mode_config_funcs.fb_create cleanup(清理 drm_framebuffer_funcs 与 drm_mode_config_funcs.fb_create)
--------------------------------------------------------------------------------------------------------------------------------

A lot more drivers could be switched over to the drm_gem_framebuffer helpers.
Various hold-ups:
(还有更多驱动可以迁移到 drm_gem_framebuffer 辅助函数上。目前的阻碍有：)

- Need to switch over to the generic dirty tracking code using
  drm_atomic_helper_dirtyfb first (e.g. qxl).
  (需要先迁移到使用 drm_atomic_helper_dirtyfb 的通用脏页跟踪代码(例如 qxl)。)

- Need to switch to drm_fbdev_generic_setup(), otherwise a lot of the custom fb
  setup code can't be deleted.
  (需要迁移到 drm_fbdev_generic_setup()，否则大量自定义 fb 设置代码无法删除。)

- Need to switch to drm_gem_fb_create(), as now drm_gem_fb_create() checks for
  valid formats for atomic drivers.
  (需要迁移到 drm_gem_fb_create()，因为现在 drm_gem_fb_create() 会为 atomic 驱动检查格式的有效性。)

- Many drivers subclass drm_framebuffer, we'd need a embedding compatible
  version of the varios drm_gem_fb_create functions. Maybe called
  drm_gem_fb_create/_with_dirty/_with_funcs as needed.
  (许多驱动会对 drm_framebuffer 做子类化，我们需要一个支持内嵌的、兼容各种变体的 drm_gem_fb_create 函数版本。或许可以按需命名为 drm_gem_fb_create/_with_dirty/_with_funcs。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Intermediate
(难度：中等。)

Generic fbdev defio support(通用 fbdev defio 支持)
--------------------------------------------------

The defio support code in the fbdev core has some very specific requirements,
which means drivers need to have a special framebuffer for fbdev. The main
issue is that it uses some fields in struct page itself, which breaks shmem
gem objects (and other things). To support defio, affected drivers require
the use of a shadow buffer, which may add CPU and memory overhead.
(fbdev 核心中的 defio 支持代码有一些非常特殊的要求，这意味着驱动需要为 fbdev 准备一个特殊的 framebuffer。主要问题在于它会使用 struct page 本身的一些字段，这会破坏 shmem gem 对象(以及其他东西)。为了支持 defio，受影响的驱动需要使用影子缓冲区，这会增加 CPU 和内存开销。)

Possible solution would be to write our own defio mmap code in the drm fbdev
emulation. It would need to fully wrap the existing mmap ops, forwarding
everything after it has done the write-protect/mkwrite trickery:
(一种可能的解决方案是在 drm fbdev 仿真层中编写我们自己的 defio mmap 代码。它需要完整包装现有的 mmap 操作，在完成写保护/mkwrite 的戏法之后把其余一切转发下去：)

- In the drm_fbdev_fb_mmap helper, if we need defio, change the
  default page prots to write-protected with something like this::

      vma->vm_page_prot = pgprot_wrprotect(vma->vm_page_prot);

  (在 drm_fbdev_fb_mmap 辅助函数中，如果需要 defio，就把默认的页保护属性改为写保护，比如这样：)

- Set the mkwrite and fsync callbacks with similar implementions to the core
  fbdev defio stuff. These should all work on plain ptes, they don't actually
  require a struct page.  uff. These should all work on plain ptes, they don't
  actually require a struct page.
  (设置 mkwrite 和 fsync 回调，实现方式与核心 fbdev defio 机制类似。这些都应直接在普通 pte 上工作，实际上并不需要 struct page。唉。它们都应能在普通 pte 上工作，实际上不需要 struct page。)

- Track the dirty pages in a separate structure (bitfield with one bit per page
  should work) to avoid clobbering struct page.
  (在一个独立的结构中跟踪脏页(用每页一位的位域应该就可以了)，以免破坏 struct page。)

Might be good to also have some igt testcases for this.
(最好还能为此准备一些 igt 测试用例。)

Contact: Simona Vetter, Noralf Tronnes
(联系人：Simona Vetter、Noralf Tronnes。)

Level: Advanced
(难度：高级。)

connector register/unregister fixes(connector 注册/注销修复)
------------------------------------------------------------

- For most connectors it's a no-op to call drm_connector_register/unregister
  directly from driver code, drm_dev_register/unregister take care of this
  already. We can remove all of them.
  (对大多数 connector 来说，在驱动代码中直接调用 drm_connector_register/unregister 是空操作，drm_dev_register/unregister 已经处理了这件事。我们可以把它们全部移除。)

- For dp drivers it's a bit more a mess, since we need the connector to be
  registered when calling drm_dp_aux_register. Fix this by instead calling
  drm_dp_aux_init, and moving the actual registering into a late_register
  callback as recommended in the kerneldoc.
  (对 DP 驱动来说情况要乱一些，因为调用 drm_dp_aux_register 时需要 connector 已经注册。按照 kerneldoc 的建议，改为调用 drm_dp_aux_init，并把真正的注册动作移到 late_register 回调中来修复此问题。)

Level: Intermediate
(难度：中等。)

Remove load/unload callbacks(移除 load/unload 回调)
---------------------------------------------------

The load/unload callbacks in struct &drm_driver are very much midlayers, plus
for historical reasons they get the ordering wrong (and we can't fix that)
between setting up the &drm_driver structure and calling drm_dev_register().
(struct &drm_driver 中的 load/unload 回调是彻头彻尾的中间层，而且由于历史原因，它们在设置 &drm_driver 结构与调用 drm_dev_register() 之间的顺序是错的(而我们无法修复)。)

- Rework drivers to no longer use the load/unload callbacks, directly coding the
  load/unload sequence into the driver's probe function.
  (改造各驱动，使其不再使用 load/unload 回调，而是把加载/卸载序列直接编写进驱动的 probe 函数。)

- Once all drivers are converted, remove the load/unload callbacks.
  (等所有驱动都转换完成后，移除 load/unload 回调。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Intermediate
(难度：中等。)

Replace drm_detect_hdmi_monitor() with drm_display_info.is_hdmi(用 drm_display_info.is_hdmi 替换 drm_detect_hdmi_monitor())
---------------------------------------------------------------------------------------------------------------------------

Once EDID is parsed, the monitor HDMI support information is available through
drm_display_info.is_hdmi. Many drivers still call drm_detect_hdmi_monitor() to
retrieve the same information, which is less efficient.
(EDID 解析完成后，显示器是否支持 HDMI 的信息即可通过 drm_display_info.is_hdmi 获得。许多驱动仍在调用 drm_detect_hdmi_monitor() 来获取同样的信息，这样效率较低。)

Audit each individual driver calling drm_detect_hdmi_monitor() and switch to
drm_display_info.is_hdmi if applicable.
(逐个审查调用 drm_detect_hdmi_monitor() 的驱动，可行的话改用 drm_display_info.is_hdmi。)

Contact: Laurent Pinchart, respective driver maintainers
(联系人：Laurent Pinchart，相应驱动维护者。)

Level: Intermediate
(难度：中等。)

Consolidate custom driver modeset properties(整合驱动自定义的 modeset 属性)
---------------------------------------------------------------------------

Before atomic modeset took place, many drivers where creating their own
properties. Among other things, atomic brought the requirement that custom,
driver specific properties should not be used.
(在 atomic 模式设置出现之前，许多驱动都在创建自己的属性。atomic 带来的要求之一就是不应再使用驱动特定的自定义属性。)

For this task, we aim to introduce core helpers or reuse the existing ones
if available:
(针对这个任务，我们的目标是引入核心辅助函数，或在可用时复用现有的辅助函数：)

A quick, unconfirmed, examples list.
(下面是一份快速整理、未经确认的示例清单。)

Introduce core helpers:
(需要引入核心辅助函数的属性：)

- audio (amdgpu, intel, gma500, radeon)
  (音频 (amdgpu、intel、gma500、radeon)。)
- brightness, contrast, etc (armada, nouveau) - overlay only (?)
  (亮度、对比度等 (armada、nouveau)——仅限 overlay (？)。)
- broadcast rgb (gma500, intel)
  (广播 RGB (gma500、intel)。)
- colorkey (armada, nouveau, rcar) - overlay only (?)
  (色键 (armada、nouveau、rcar)——仅限 overlay (？)。)
- dither (amdgpu, nouveau, radeon) - varies across drivers
  (抖动 (amdgpu、nouveau、radeon)——各驱动实现不一。)
- underscan family (amdgpu, radeon, nouveau)
  (欠扫描系列 (amdgpu、radeon、nouveau)。)

Already in core:
(核心中已有：)

- colorspace (sti)
  (色彩空间 (sti)。)
- tv format names, enhancements (gma500, intel)
  (电视格式名称、增强选项 (gma500、intel)。)
- tv overscan, margins, etc. (gma500, intel)
  (电视过扫描、边距等 (gma500、intel)。)
- zorder (omapdrm) - same as zpos (?)
  (z 序 (omapdrm)——与 zpos 相同 (？)。)


Contact: Emil Velikov, respective driver maintainers
(联系人：Emil Velikov，相应驱动维护者。)

Level: Intermediate
(难度：中等。)

Use struct iosys_map throughout codebase(在整个代码库中使用 struct iosys_map)
-----------------------------------------------------------------------------

Pointers to shared device memory are stored in struct iosys_map. Each
instance knows whether it refers to system or I/O memory. Most of the DRM-wide
interface have been converted to use struct iosys_map, but implementations
often still use raw pointers.
(指向共享设备内存的指针保存在 struct iosys_map 中。每个实例都知道自己指向的是系统内存还是 I/O 内存。DRM 范围内的大多数接口都已转换为使用 struct iosys_map，但具体实现往往仍在使用裸指针。)

The task is to use struct iosys_map where it makes sense.
(任务是在合理的地方使用 struct iosys_map。)

* Memory managers should use struct iosys_map for dma-buf-imported buffers.
  (内存管理器应对 dma-buf 导入的缓冲区使用 struct iosys_map。)
* TTM might benefit from using struct iosys_map internally.
  (TTM 或许能从内部使用 struct iosys_map 中受益。)
* Framebuffer copying and blitting helpers should operate on struct iosys_map.
  (framebuffer 拷贝与位块传输辅助函数应在 struct iosys_map 上操作。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>, Christian König, Simona Vetter
(联系人：Thomas Zimmermann <tzimmermann@suse.de>、Christian König、Simona Vetter。)

Level: Intermediate
(难度：中等。)

Review all drivers for setting struct drm_mode_config.{max_width,max_height} correctly(审查所有驱动对 struct drm_mode_config.{max_width,max_height} 的设置)
-----------------------------------------------------------------------------------------------------------------------------------------------------------

The values in struct drm_mode_config.{max_width,max_height} describe the
maximum supported framebuffer size. It's the virtual screen size, but many
drivers treat it like limitations of the physical resolution.
(struct drm_mode_config.{max_width,max_height} 中的值描述的是所支持的最大 framebuffer 尺寸。它指的是虚拟屏幕尺寸，但许多驱动把它当成了物理分辨率的限制。)

The maximum width depends on the hardware's maximum scanline pitch. The
maximum height depends on the amount of addressable video memory. Review all
drivers to initialize the fields to the correct values.
(最大宽度取决于硬件的最大扫描行间距。最大高度取决于可寻址显存的容量。请审查所有驱动，把这些字段初始化为正确的值。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>
(联系人：Thomas Zimmermann <tzimmermann@suse.de>。)

Level: Intermediate
(难度：中等。)

Request memory regions in all fbdev drivers(在所有 fbdev 驱动中申请内存区域)
----------------------------------------------------------------------------

Old/ancient fbdev drivers do not request their memory properly.
Go through these drivers and add code to request the memory regions
that the driver uses. This requires adding calls to request_mem_region(),
pci_request_region() or similar functions. Use helpers for managed cleanup
where possible. Problematic areas include hardware that has exclusive ranges
like VGA. VGA16fb does not request the range as it is expected.
Drivers are pretty bad at doing this and there used to be conflicts among
DRM and fbdev drivers. Still, it's the correct thing to do.
(老旧的 fbdev 驱动没有正确申请自己的内存。请逐个检查这些驱动，添加申请驱动所用内存区域的代码。这需要添加对 request_mem_region()、pci_request_region() 或类似函数的调用，并尽可能使用托管式清理的辅助函数。有问题的区域包括像 VGA 这样拥有独占范围的硬件。VGA16fb 就没有像预期那样申请该范围。驱动们在这一点上普遍做得很差，DRM 驱动与 fbdev 驱动之间曾因此发生冲突。尽管如此，这才是正确的做法。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>
(联系人：Thomas Zimmermann <tzimmermann@suse.de>。)

Level: Starter
(难度：入门。)

Remove driver dependencies on FB_DEVICE(移除驱动对 FB_DEVICE 的依赖)
--------------------------------------------------------------------

A number of fbdev drivers provide attributes via sysfs and therefore depend
on CONFIG_FB_DEVICE to be selected. Review each driver and attempt to make
any dependencies on CONFIG_FB_DEVICE optional. At the minimum, the respective
code in the driver could be conditionalized via ifdef CONFIG_FB_DEVICE. Not
all drivers might be able to drop CONFIG_FB_DEVICE.
(一些 fbdev 驱动通过 sysfs 提供属性，因此依赖选中 CONFIG_FB_DEVICE。请逐个审查驱动，尝试把对 CONFIG_FB_DEVICE 的依赖改为可选。至少可以把驱动中的相关代码用 ifdef CONFIG_FB_DEVICE 条件编译起来。并非所有驱动都能去掉 CONFIG_FB_DEVICE。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>
(联系人：Thomas Zimmermann <tzimmermann@suse.de>。)

Level: Starter
(难度：入门。)

Remove disable/unprepare in remove/shutdown in panel-simple and panel-edp(移除 panel-simple 与 panel-edp 在 remove/shutdown 中的 disable/unprepare)
---------------------------------------------------------------------------------------------------------------------------------------------------

As of commit d2aacaf07395 ("drm/panel: Check for already prepared/enabled in
drm_panel"), we have a check in the drm_panel core to make sure nobody
double-calls prepare/enable/disable/unprepare. Eventually that should probably
be turned into a WARN_ON() or somehow made louder.
(自 commit d2aacaf07395 ("drm/panel: Check for already prepared/enabled in drm_panel") 起，drm_panel 核心中有一个检查，确保没有人重复调用 prepare/enable/disable/unprepare。最终它大概应该变成一个 WARN_ON()，或者以某种方式变得更醒目。)

At the moment, we expect that we may still encounter the warnings in the
drm_panel core when using panel-simple and panel-edp. Since those panel
drivers are used with a lot of different DRM modeset drivers they still
make an extra effort to disable/unprepare the panel themsevles at shutdown
time. Specifically we could still encounter those warnings if the panel
driver gets shutdown() _before_ the DRM modeset driver and the DRM modeset
driver properly calls drm_atomic_helper_shutdown() in its own shutdown()
callback. Warnings could be avoided in such a case by using something like
device links to ensure that the panel gets shutdown() after the DRM modeset
driver.
(目前，使用 panel-simple 和 panel-edp 时我们仍可能遇到 drm_panel 核心中的警告。由于这些 panel 驱动会与许多不同的 DRM 模式设置驱动搭配使用，它们仍在关机时额外多做一步，自行 disable/unprepare 面板。具体来说，如果 panel 驱动先于 DRM 模式设置驱动收到 shutdown()，而 DRM 模式设置驱动又在其自身的 shutdown() 回调中正确调用了 drm_atomic_helper_shutdown()，我们仍可能遇到那些警告。在这种情况下，可以通过使用 device link 之类的机制来确保 panel 在 DRM 模式设置驱动之后才收到 shutdown()，从而避免警告。)

Once all DRM modeset drivers are known to shutdown properly, the extra
calls to disable/unprepare in remove/shutdown in panel-simple and panel-edp
should be removed and this TODO item marked complete.
(一旦确认所有 DRM 模式设置驱动都能正确关机，就应移除 panel-simple 和 panel-edp 在 remove/shutdown 中对 disable/unprepare 的额外调用，并将此 TODO 条目标记为完成。)

Contact: Douglas Anderson <dianders@chromium.org>
(联系人：Douglas Anderson <dianders@chromium.org>。)

Level: Intermediate
(难度：中等。)

Transition away from using deprecated MIPI DSI functions(停止使用已废弃的 MIPI DSI 函数)
----------------------------------------------------------------------------------------

There are many functions defined in ``drm_mipi_dsi.c`` which have been
deprecated. Each deprecated function was deprecated in favor of its `multi`
variant (e.g. `mipi_dsi_generic_write()` and `mipi_dsi_generic_write_multi()`).
The `multi` variant of a function includes improved error handling and logic
which makes it more convenient to make several calls in a row, as most MIPI
drivers do.
(``drm_mipi_dsi.c`` 中定义的许多函数已被废弃。每个被废弃的函数都有其 `multi` 变体取而代之(例如 `mipi_dsi_generic_write()` 与 `mipi_dsi_generic_write_multi()`)。函数的 `multi` 变体包含改进的错误处理和逻辑，使得像大多数 MIPI 驱动那样连续进行多次调用更加方便。)

Drivers should be updated to use undeprecated functions. Once all usages of the
deprecated MIPI DSI functions have been removed, their definitions may be
removed from ``drm_mipi_dsi.c``.
(应更新驱动以使用未废弃的函数。一旦被废弃的 MIPI DSI 函数的所有用法都被移除，它们的定义就可以从 ``drm_mipi_dsi.c`` 中删除。)

Contact: Douglas Anderson <dianders@chromium.org>
(联系人：Douglas Anderson <dianders@chromium.org>。)

Level: Starter
(难度：入门。)

Remove devm_drm_put_bridge()(移除 devm_drm_put_bridge())
--------------------------------------------------------

Due to how the panel bridge handles the drm_bridge object lifetime, special
care must be taken to dispose of the drm_bridge object when the
panel_bridge is removed. This is currently managed using
devm_drm_put_bridge(), but that is an unsafe, temporary workaround. To fix
that, the DRM panel lifetime needs to be reworked. After the rework is
done, remove devm_drm_put_bridge() and the TODO in
drm_panel_bridge_remove().
(由于 panel bridge 处理 drm_bridge 对象生命周期的方式，在移除 panel_bridge 时必须特别小心地处置 drm_bridge 对象。目前这是通过 devm_drm_put_bridge() 管理的，但这是一个不安全的临时变通办法。要修复它，需要重新设计 DRM panel 的生命周期。重构完成后，移除 devm_drm_put_bridge() 以及 drm_panel_bridge_remove() 中的 TODO。)

Contact: Maxime Ripard <mripard@kernel.org>,
         Luca Ceresoli <luca.ceresoli@bootlin.com>

(联系人：Maxime Ripard <mripard@kernel.org>、Luca Ceresoli <luca.ceresoli@bootlin.com>。)

Level: Intermediate
(难度：中等。)

Convert users of of_drm_find_bridge() to of_drm_find_and_get_bridge()(把 of_drm_find_bridge() 的使用者迁移到 of_drm_find_and_get_bridge())
------------------------------------------------------------------------------------------------------------------------------------------

Taking a struct drm_bridge pointer requires getting a reference and putting
it after disposing of the pointer. Most functions returning a struct
drm_bridge pointer already call drm_bridge_get() to increment the refcount
and their users have been updated to call drm_bridge_put() when
appropriate. of_drm_find_bridge() does not get a reference and it has been
deprecated in favor of of_drm_find_and_get_bridge() which does, but some
users still need to be converted.
(获取 struct drm_bridge 指针需要取得引用，并在用完指针后归还。大多数返回 struct drm_bridge 指针的函数已经调用 drm_bridge_get() 来递增引用计数，其调用者也已更新为在适当时机调用 drm_bridge_put()。of_drm_find_bridge() 不获取引用，已被废弃，由会获取引用的 of_drm_find_and_get_bridge() 取代，但仍有一些使用者需要转换。)

Contact: Maxime Ripard <mripard@kernel.org>,
         Luca Ceresoli <luca.ceresoli@bootlin.com>

(联系人：Maxime Ripard <mripard@kernel.org>、Luca Ceresoli <luca.ceresoli@bootlin.com>。)

Level: Intermediate
(难度：中等。)

Core refactorings(核心重构)
===========================

Make panic handling work(让 panic 处理真正可用)
-----------------------------------------------

This is a really varied tasks with lots of little bits and pieces:
(这是一项相当繁杂的任务，包含许多零碎的环节：)

* The panic path can't be tested currently, leading to constant breaking. The
  main issue here is that panics can be triggered from hardirq contexts and
  hence all panic related callback can run in hardirq context. It would be
  awesome if we could test at least the fbdev helper code and driver code by
  e.g. trigger calls through drm debugfs files. hardirq context could be
  achieved by using an IPI to the local processor.
  (目前 panic 路径无法测试，导致它不断被弄坏。这里的主要问题是 panic 可能从 hardirq 上下文触发，因此所有与 panic 相关的回调都可能运行在 hardirq 上下文中。如果我们至少能测试 fbdev 辅助代码和驱动代码，比如通过 drm debugfs 文件触发调用，那就太好了。hardirq 上下文可以通过向本地处理器发送 IPI 来实现。)

* There's a massive confusion of different panic handlers. DRM fbdev emulation
  helpers had their own (long removed), but on top of that the fbcon code itself
  also has one. We need to make sure that they stop fighting over each other.
  This is worked around by checking ``oops_in_progress`` at various entry points
  into the DRM fbdev emulation helpers. A much cleaner approach here would be to
  switch fbcon to the `threaded printk support
  <https://lwn.net/Articles/800946/>`_.
  (各种不同的 panic 处理程序之间存在严重混乱。DRM fbdev 仿真辅助函数曾有自己的一个(早已移除)，但除此之外 fbcon 代码本身也有一个。我们需要确保它们不再互相争斗。目前的变通办法是在进入 DRM fbdev 仿真辅助函数的各个入口点检查 ``oops_in_progress``。这里干净得多的做法是让 fbcon 改用 `threaded printk support <https://lwn.net/Articles/800946/>`_ (线程化 printk 支持)。)

* ``drm_can_sleep()`` is a mess. It hides real bugs in normal operations and
  isn't a full solution for panic paths. We need to make sure that it only
  returns true if there's a panic going on for real, and fix up all the
  fallout.
  (``drm_can_sleep()`` 一团糟。它掩盖了正常操作中的真实 bug，而且也不是 panic 路径的完整解决方案。我们需要确保它只在真正发生 panic 时才返回 true，并处理好由此带来的一切影响。)

* The panic handler must never sleep, which also means it can't ever
  ``mutex_lock()``. Also it can't grab any other lock unconditionally, not
  even spinlocks (because NMI and hardirq can panic too). We need to either
  make sure to not call such paths, or trylock everything. Really tricky.
  (panic 处理程序绝不能睡眠，这也意味着它永远不能 ``mutex_lock()``。它也不能无条件地获取任何其他锁，连自旋锁都不行(因为 NMI 和 hardirq 也可能触发 panic)。我们要么确保不调用这类路径，要么对所有锁都改用 trylock。非常棘手。)

* A clean solution would be an entirely separate panic output support in KMS,
  bypassing the current fbcon support. See `[PATCH v2 0/3] drm: Add panic handling
  <https://lore.kernel.org/dri-devel/20190311174218.51899-1-noralf@tronnes.org/>`_.
  (一个干净的解决方案是在 KMS 中提供完全独立的 panic 输出支持，绕过现有的 fbcon 支持。参见 `[PATCH v2 0/3] drm: Add panic handling <https://lore.kernel.org/dri-devel/20190311174218.51899-1-noralf@tronnes.org/>`_。)

* Encoding the actual oops and preceding dmesg in a QR might help with the
  dread "important stuff scrolled away" problem. See `[RFC][PATCH] Oops messages
  transfer using QR codes
  <https://lore.kernel.org/lkml/1446217392-11981-1-git-send-email-alexandru.murtaza@intel.com/>`_
  for some example code that could be reused.
  (把实际的 oops 及其之前的 dmesg 编码进二维码，或许能缓解那个可怕的"重要信息被滚出屏幕"的问题。可复用的示例代码参见 `[RFC][PATCH] Oops messages transfer using QR codes <https://lore.kernel.org/lkml/1446217392-11981-1-git-send-email-alexandru.murtaza@intel.com/>`_。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Advanced
(难度：高级。)

Clean up the debugfs support(清理 debugfs 支持)
-----------------------------------------------

There's a bunch of issues with it:
(它存在一系列问题：)

- Convert drivers to support the drm_debugfs_add_files() function instead of
  the drm_debugfs_create_files() function.
  (把驱动转换为使用 drm_debugfs_add_files() 函数，而不是 drm_debugfs_create_files() 函数。)

- Improve late-register debugfs by rolling out the same debugfs pre-register
  infrastructure for connector and crtc too. That way, the drivers won't need to
  split their setup code into init and register anymore.
  (通过为 connector 和 crtc 也推广同样的 debugfs 预注册基础设施，来改进延迟注册的 debugfs。这样驱动就不再需要把设置代码拆分成 init 和 register 两段了。)

- We probably want to have some support for debugfs files on crtc/connectors and
  maybe other kms objects directly in core. There's even drm_print support in
  the funcs for these objects to dump kms state, so it's all there. And then the
  ->show() functions should obviously give you a pointer to the right object.
  (我们可能希望核心直接为 crtc/connector 或许还有其他 kms 对象上的 debugfs 文件提供某种支持。这些对象的 funcs 中甚至已经有 drm_print 支持用于转储 kms 状态，所以基础都已具备。届时 ->show() 函数显然应该给你一个指向正确对象的指针。)

- The drm_driver->debugfs_init hooks we have is just an artifact of the old
  midlayered load sequence. DRM debugfs should work more like sysfs, where you
  can create properties/files for an object anytime you want, and the core
  takes care of publishing/unpuplishing all the files at register/unregister
  time. Drivers shouldn't need to worry about these technicalities, and fixing
  this (together with the drm_minor->drm_device move) would allow us to remove
  debugfs_init.
  (现有的 drm_driver->debugfs_init 钩子只是旧的中层化加载序列的残留物。DRM debugfs 应该更像 sysfs 那样工作：你可以随时为对象创建属性/文件，由核心负责在 register/unregister 时发布/取消发布所有文件。驱动不应为这些技术细节操心，修复这个问题(连同 drm_minor 到 drm_device 的迁移)将使我们能够移除 debugfs_init。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Intermediate
(难度：中等。)

Object lifetime fixes(对象生命周期修复)
---------------------------------------

There's two related issues here
(这里有两个相关的问题：)

- Cleanup up the various ->destroy callbacks, which often are all the same
  simple code.
  (清理各种 ->destroy 回调，它们往往都是相同的简单代码。)

- Lots of drivers erroneously allocate DRM modeset objects using devm_kzalloc,
  which results in use-after free issues on driver unload. This can be serious
  trouble even for drivers for hardware integrated on the SoC due to
  EPROBE_DEFERRED backoff.
  (许多驱动错误地使用 devm_kzalloc 分配 DRM 模式设置对象，这会导致驱动卸载时出现释放后使用问题。由于 EPROBE_DEFERRED 回退机制，即使是集成在 SoC 上的硬件驱动，这也可能造成严重麻烦。)

Both these problems can be solved by switching over to drmm_kzalloc(), and the
various convenience wrappers provided, e.g. drmm_crtc_alloc_with_planes(),
drmm_universal_plane_alloc(), ... and so on.
(这两个问题都可以通过改用 drmm_kzalloc() 及其提供的各种便捷封装来解决，例如 drmm_crtc_alloc_with_planes()、drmm_universal_plane_alloc() 等等。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Intermediate
(难度：中等。)

Remove automatic page mapping from dma-buf importing(从 dma-buf 导入中移除自动页映射)
-------------------------------------------------------------------------------------

When importing dma-bufs, the dma-buf and PRIME frameworks automatically map
imported pages into the importer's DMA area. drm_gem_prime_fd_to_handle() and
drm_gem_prime_handle_to_fd() require that importers call dma_buf_attach()
even if they never do actual device DMA, but only CPU access through
dma_buf_vmap(). This is a problem for USB devices, which do not support DMA
operations.
(导入 dma-buf 时，dma-buf 和 PRIME 框架会自动把导入的页映射到导入方的 DMA 区域。drm_gem_prime_fd_to_handle() 和 drm_gem_prime_handle_to_fd() 要求导入方调用 dma_buf_attach()，即使它们从不进行真正的设备 DMA，而只是通过 dma_buf_vmap() 进行 CPU 访问。这对不支持 DMA 操作的 USB 设备来说是个问题。)

To fix the issue, automatic page mappings should be removed from the
buffer-sharing code. Fixing this is a bit more involved, since the import/export
cache is also tied to &drm_gem_object.import_attach. Meanwhile we paper over
this problem for USB devices by fishing out the USB host controller device, as
long as that supports DMA. Otherwise importing can still needlessly fail.
(要修复此问题，应当从缓冲区共享代码中移除自动页映射。修复起来稍微复杂一些，因为导入/导出缓存也与 &drm_gem_object.import_attach 绑定在一起。与此同时，只要 USB 主控制器设备支持 DMA，我们就通过把它揪出来来为 USB 设备掩盖这个问题。否则导入仍可能无谓地失败。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>, Simona Vetter
(联系人：Thomas Zimmermann <tzimmermann@suse.de>、Simona Vetter。)

Level: Advanced
(难度：高级。)

Implement a new DUMB_CREATE2 ioctl(实现新的 DUMB_CREATE2 ioctl)
---------------------------------------------------------------

The current DUMB_CREATE ioctl is not well defined. Instead of a pixel and
framebuffer format, it only accepts a color mode of vague semantics. Assuming
a linear framebuffer, the color mode gives an idea of the supported pixel
format. But userspace effectively has to guess the correct values. It really
only works reliably with framebuffers in XRGB8888. Userspace has begun to
workaround these limitations by computing arbitrary format's buffer sizes and
calculating their sizes in terms of XRGB8888 pixels.
(当前的 DUMB_CREATE ioctl 定义得并不好。它不接受像素格式和 framebuffer 格式，而是只接受一个语义含糊的色深模式。在假设 framebuffer 为线性布局的前提下，色深模式能大致反映出所支持的像素格式。但用户空间实际上不得不去猜测正确的取值。它只在 XRGB8888 格式的 framebuffer 上真正可靠。用户空间已经开始绕过这些限制，方法是计算任意格式的缓冲区大小，并以 XRGB8888 像素为单位折算其尺寸。)

One possible solution is a new ioctl DUMB_CREATE2. It should accept a DRM
format and a format modifier to resolve the color mode's ambiguity. As
framebuffers can be multi-planar, the new ioctl has to return the buffer size,
pitch and GEM handle for each individual color plane.
(一种可能的解决方案是新的 ioctl DUMB_CREATE2。它应接受一个 DRM 格式和一个格式修饰符 (format modifier)，以消除色深模式的歧义。由于 framebuffer 可以是多平面的，新 ioctl 必须为每个颜色平面分别返回缓冲区大小、行间距 (pitch) 和 GEM 句柄。)

In the first step, the new ioctl can be limited to the current features of
the existing DUMB_CREATE. Individual drivers can then be extended to support
multi-planar formats. Rockchip might require this and would be a good candidate.
(第一步，新 ioctl 可以限制在现有 DUMB_CREATE 的功能范围内。之后可以逐个扩展驱动以支持多平面格式。Rockchip 可能需要这个功能，会是一个很好的首选。)

It might also be helpful to userspace to query information about the size of
a potential buffer, if allocated. Userspace would supply geometry and format;
the kernel would return minimal allocation sizes and scanline pitch. There is
interest to allocate that memory from another device and provide it to the
DRM driver (say via dma-buf).
(对用户空间来说，能够查询潜在缓冲区(如果分配的话)大小的信息也可能有帮助。用户空间提供几何信息和格式；内核返回最小分配大小和扫描行间距。有人有兴趣从另一个设备分配这块内存再提供给 DRM 驱动(比如通过 dma-buf)。)

Another requested feature is the ability to allocate a buffer by size, without
format. Accelators use this for their buffer allocation and it could likely be
generalized.
(另一个被要求的功能是按大小而不按格式分配缓冲区的能力。加速器驱动将此用于它们的缓冲区分配，它很可能可以被泛化。)

In addition to the kernel implementation, there must be user-space support
for the new ioctl. There's code in Mesa that might be able to use the new
call.
(除了内核实现之外，新 ioctl 还必须有用户空间支持。Mesa 中已有代码可能用得上这个新调用。)

Contact: Thomas Zimmermann <tzimmermann@suse.de>
(联系人：Thomas Zimmermann <tzimmermann@suse.de>。)

Level: Advanced
(难度：高级。)

Better Testing(更好的测试)
==========================

Add unit tests using the Kernel Unit Testing (KUnit) framework(使用内核单元测试 (KUnit) 框架添加单元测试)
---------------------------------------------------------------------------------------------------------

The `KUnit <https://www.kernel.org/doc/html/latest/dev-tools/kunit/index.html>`_
provides a common framework for unit tests within the Linux kernel. Having a
test suite would allow to identify regressions earlier.
(`KUnit <https://www.kernel.org/doc/html/latest/dev-tools/kunit/index.html>`_ 为 Linux 内核内的单元测试提供了一个通用框架。拥有测试套件有助于更早发现回归。)

A good candidate for the first unit tests are the format-conversion helpers in
``drm_format_helper.c``.
(``drm_format_helper.c`` 中的格式转换辅助函数是首批单元测试的良好候选对象。)

Contact: Javier Martinez Canillas <javierm@redhat.com>
(联系人：Javier Martinez Canillas <javierm@redhat.com>。)

Level: Intermediate
(难度：中等。)

Clean up and document former selftests suites(清理原有 selftests 套件并补充文档)
--------------------------------------------------------------------------------

Some KUnit test suites (drm_buddy, drm_cmdline_parser, drm_damage_helper,
drm_format, drm_framebuffer, drm_dp_mst_helper, drm_mm, drm_plane_helper and
drm_rect) are former selftests suites that have been converted over when KUnit
was first introduced.
(一些 KUnit 测试套件(drm_buddy、drm_cmdline_parser、drm_damage_helper、drm_format、drm_framebuffer、drm_dp_mst_helper、drm_mm、drm_plane_helper 和 drm_rect)是原有的 selftests 套件，在 KUnit 最初引入时被转换了过来。)

These suites were fairly undocumented, and with different goals than what unit
tests can be. Trying to identify what each test in these suites actually test
for, whether that makes sense for a unit test, and either remove it if it
doesn't or document it if it does would be of great help.
(这些套件几乎没有文档，而且其目标与单元测试所能做到的并不相同。设法弄清这些套件中的每个测试实际测试的是什么、这对单元测试是否合理，如果不合理就移除它、合理就为它补充文档，将大有帮助。)

Contact: Maxime Ripard <mripard@kernel.org>
(联系人：Maxime Ripard <mripard@kernel.org>。)

Level: Intermediate
(难度：中等。)

Enable trinity for DRM(为 DRM 启用 trinity)
-------------------------------------------

And fix up the fallout. Should be really interesting ...
(然后处理好带来的各种影响。应该会非常有趣……)

Level: Advanced
(难度：高级。)

Make KMS tests in i-g-t generic(让 i-g-t 的 KMS 测试通用化)
-----------------------------------------------------------

The i915 driver team maintains an extensive testsuite for the i915 DRM driver,
including tons of testcases for corner-cases in the modesetting API. It would
be awesome if those tests (at least the ones not relying on Intel-specific GEM
features) could be made to run on any KMS driver.
(i915 驱动团队为 i915 DRM 驱动维护着一个庞大的测试套件，其中包括大量针对模式设置 API 各种边角情况的测试用例。如果这些测试(至少是不依赖 Intel 特有 GEM 特性的那些)能够在任何 KMS 驱动上运行，那就太棒了。)

Basic work to run i-g-t tests on non-i915 is done, what's now missing is mass-
converting things over. For modeset tests we also first need a bit of
infrastructure to use dumb buffers for untiled buffers, to be able to run all
the non-i915 specific modeset tests.
(在非 i915 上运行 i-g-t 测试的基础工作已经完成，现在缺少的是大规模的迁移改造。对于 modeset 测试，我们首先还需要一点基础设施来把 dumb 缓冲区用作非平铺 (untiled) 缓冲区，以便能运行所有非 i915 特有的 modeset 测试。)

Level: Advanced
(难度：高级。)

Extend virtual test driver (VKMS)(扩展虚拟测试驱动 (VKMS))
----------------------------------------------------------

See the documentation of :ref:`VKMS <vkms>` for more details. This is an ideal
internship task, since it only requires a virtual machine and can be sized to
fit the available time.
(更多细节参见 :ref:`VKMS <vkms>` 的文档。这是一个理想的实习任务，因为它只需要一台虚拟机，而且工作量可以按可用时间灵活安排。)

Level: See details
(难度：详见说明。)

Backlight Refactoring(背光重构)
-------------------------------

Backlight drivers have a triple enable/disable state, which is a bit overkill.
Plan to fix this:
(背光驱动拥有三重的 enable/disable 状态，这有点小题大做。修复计划如下：)

1. Roll out backlight_enable() and backlight_disable() helpers everywhere. This
   has started already.
   (在各处推广使用 backlight_enable() 和 backlight_disable() 辅助函数。这项工作已经开始。)
2. In all, only look at one of the three status bits set by the above helpers.
   (统一为只查看上述辅助函数设置的三个状态位中的一个。)
3. Remove the other two status bits.
   (移除另外两个状态位。)

Contact: Simona Vetter
(联系人：Simona Vetter。)

Level: Intermediate
(难度：中等。)

Driver Specific(驱动特定事项)
=============================

AMD DC Display Driver(AMD DC 显示驱动)
--------------------------------------

AMD DC is the display driver for AMD devices starting with Vega. There has been
a bunch of progress cleaning it up but there's still plenty of work to be done.
(AMD DC 是从 Vega 开始的 AMD 设备的显示驱动。清理工作已经取得不少进展，但仍有很多工作要做。)

See drivers/gpu/drm/amd/display/TODO for tasks.
(任务清单见 drivers/gpu/drm/amd/display/TODO。)

Contact: Harry Wentland, Alex Deucher
(联系人：Harry Wentland、Alex Deucher。)

Bootsplash(启动画面)
====================

There is support in place now for writing internal DRM clients making it
possible to pick up the bootsplash work that was rejected because it was written
for fbdev.
(现在已经具备编写内核内部 DRM 客户端的支持，使得重启当年因基于 fbdev 编写而被拒绝的 bootsplash(启动画面)工作成为可能。)

- [v6,8/8] drm/client: Hack: Add bootsplash example
  https://patchwork.freedesktop.org/patch/306579/

- [RFC PATCH v2 00/13] Kernel based bootsplash
  https://lore.kernel.org/r/20171213194755.3409-1-mstaudt@suse.de

Contact: Sam Ravnborg
(联系人：Sam Ravnborg。)

Level: Advanced
(难度：高级。)

Brightness handling on devices with multiple internal panels(多内置面板设备上的亮度处理)
========================================================================================

On x86/ACPI devices there can be multiple backlight firmware interfaces:
(ACPI) video, vendor specific and others. As well as direct/native (PWM)
register programming by the KMS driver.
(在 x86/ACPI 设备上可能存在多个背光固件接口：ACPI video、厂商特定接口及其他接口，还有 KMS 驱动进行的直接/原生 (PWM) 寄存器编程。)

To deal with this backlight drivers used on x86/ACPI call
acpi_video_get_backlight_type() which has heuristics (+quirks) to select
which backlight interface to use; and backlight drivers which do not match
the returned type will not register themselves, so that only one backlight
device gets registered (in a single GPU setup, see below).
(为了应对这种情况，x86/ACPI 上使用的背光驱动会调用 acpi_video_get_backlight_type()，该函数通过启发式规则(外加 quirks)来选择使用哪个背光接口；与返回类型不匹配的背光驱动将不注册自己，从而确保只注册一个背光设备(在单 GPU 配置下，见下文)。)

At the moment this more or less assumes that there will only
be 1 (internal) panel on a system.
(目前这或多或少假定了系统上只有 1 块(内置)面板。)

On systems with 2 panels this may be a problem, depending on
what interface acpi_video_get_backlight_type() selects:
(在有 2 块面板的系统上，这可能成为问题，取决于 acpi_video_get_backlight_type() 选择了哪个接口：)

1. native: in this case the KMS driver is expected to know which backlight
   device belongs to which output so everything should just work.
   (native：这种情况下应假定 KMS 驱动知道哪个背光设备属于哪个输出，所以一切都应正常工作。)
2. video: this does support controlling multiple backlights, but some work
   will need to be done to get the output <-> backlight device mapping
   (video：它确实支持控制多个背光，但还需要做一些工作来建立输出与背光设备之间的映射。)

The above assumes both panels will require the same backlight interface type.
Things will break on systems with multiple panels where the 2 panels need
a different type of control. E.g. one panel needs ACPI video backlight control,
where as the other is using native backlight control. Currently in this case
only one of the 2 required backlight devices will get registered, based on
the acpi_video_get_backlight_type() return value.
(上面假设两块面板都需要相同类型的背光接口。在多面板系统中，如果两块面板需要不同类型的控制，事情就会出问题。例如一块面板需要 ACPI video 背光控制，而另一块使用原生背光控制。目前在这种情况下，2 个所需的背光设备中只有一个会基于 acpi_video_get_backlight_type() 的返回值被注册。)

If this (theoretical) case ever shows up, then supporting this will need some
work. A possible solution here would be to pass a device and connector-name
to acpi_video_get_backlight_type() so that it can deal with this.
(如果这个(理论上的)情况真的出现，那么支持它还需要一些工作。一种可能的解决方案是向 acpi_video_get_backlight_type() 传递设备和 connector 名称，使其能够处理这种情况。)

Note in a way we already have a case where userspace sees 2 panels,
in dual GPU laptop setups with a mux. On those systems we may see
either 2 native backlight devices; or 2 native backlight devices.
(注意，在带 mux 的双 GPU 笔记本配置中，我们其实已经有了一种用户空间看到 2 块面板的情况。在这类系统上我们可能看到 2 个原生背光设备；或者 2 个原生背光设备。)

Userspace already has code to deal with this by detecting if the related
panel is active (iow which way the mux between the GPU and the panels
points) and then uses that backlight device. Userspace here very much
assumes a single panel though. It picks only 1 of the 2 backlight devices
and then only uses that one.
(用户空间已经有处理这种情况的代码：检测相关面板是否处于活动状态(也就是说 GPU 与面板之间的 mux 指向哪一边)，然后使用那个背光设备。不过这里的用户空间非常强烈地假设只有单块面板：它只从 2 个背光设备中挑 1 个，然后只用那一个。)

Note that all userspace code (that I know off) is currently hardcoded
to assume a single panel.
(注意，据我所知，所有用户空间代码目前都硬编码为假设单块面板。)

Before the recent changes to not register multiple (e.g. video + native)
/sys/class/backlight devices for a single panel (on a single GPU laptop),
userspace would see multiple backlight devices all controlling the same
backlight.
(在最近的修改(不为单块面板(在单 GPU 笔记本上)注册多个(例如 video + native)/sys/class/backlight 设备)之前，用户空间会看到多个背光设备都在控制同一个背光。)

To deal with this userspace had to always picks one preferred device under
/sys/class/backlight and will ignore the others. So to support brightness
control on multiple panels userspace will need to be updated too.
(为了应对这种情况，用户空间不得不总是从 /sys/class/backlight 下挑选一个首选设备并忽略其他设备。因此，要支持多面板上的亮度控制，用户空间也需要更新。)

There are plans to allow brightness control through the KMS API by adding
a "display brightness" property to drm_connector objects for panels. This
solves a number of issues with the /sys/class/backlight API, including not
being able to map a sysfs backlight device to a specific connector. Any
userspace changes to add support for brightness control on devices with
multiple panels really should build on top of this new KMS property.
(已有计划通过为 panel 的 drm_connector 对象添加 "display brightness" 属性来允许通过 KMS API 控制亮度。这解决了 /sys/class/backlight API 的许多问题，包括无法把 sysfs 背光设备映射到特定 connector。用户空间为多面板设备添加亮度控制支持的任何改动，都应当构建在这个新的 KMS 属性之上。)

Contact: Hans de Goede
(联系人：Hans de Goede。)

Level: Advanced
(难度：高级。)

Buffer age or other damage accumulation algorithm for buffer damage(面向缓冲区损坏的 buffer age 或其他损坏累积算法)
===================================================================================================================

Drivers that do per-buffer uploads, need a buffer damage handling (rather than
frame damage like drivers that do per-plane or per-CRTC uploads), but there is
no support to get the buffer age or any other damage accumulation algorithm.
(按缓冲区上传的驱动需要针对缓冲区的损坏处理(而不是像按 plane 或按 CRTC 上传的驱动那样按帧处理)，但目前没有任何支持来获取 buffer age 或实现任何其他损坏累积算法。)

For this reason, the damage helpers just fallback to a full plane update if the
framebuffer attached to a plane has changed since the last page-flip. Drivers
set &drm_plane_state.ignore_damage_clips to true as indication to
drm_atomic_helper_damage_iter_init() and drm_atomic_helper_damage_iter_next()
helpers that the damage clips should be ignored.
(因此，只要 plane 关联的 framebuffer 自上次翻页 (page-flip) 以来发生了变化，损坏辅助函数就直接回退为整 plane 更新。驱动会把 &drm_plane_state.ignore_damage_clips 设为 true，以此告知 drm_atomic_helper_damage_iter_init() 和 drm_atomic_helper_damage_iter_next() 辅助函数应忽略损坏裁剪区域。)

This should be improved to get damage tracking properly working on drivers that
do per-buffer uploads.
(应当改进这一点，使损坏跟踪在按缓冲区上传的驱动上真正可用。)

More information about damage tracking and references to learning materials can
be found in :ref:`damage_tracking_properties`.
(关于损坏跟踪的更多信息及学习资料链接，见 :ref:`damage_tracking_properties`。)

Contact: Javier Martinez Canillas <javierm@redhat.com>
(联系人：Javier Martinez Canillas <javierm@redhat.com>。)

Level: Advanced
(难度：高级。)

Querying errors from drm_syncobj(从 drm_syncobj 查询错误)
=========================================================

The drm_syncobj container can be used by driver independent code to signal
complection of submission.
(drm_syncobj 容器可被驱动无关的代码用来通告提交的完成。)

One minor feature still missing is a generic DRM IOCTL to query the error
status of binary and timeline drm_syncobj.
(目前仍缺少的一个小功能是一个通用 DRM IOCTL，用于查询 binary 和 timeline drm_syncobj 的错误状态。)

This should probably be improved by implementing the necessary kernel interface
and adding support for that in the userspace stack.
(改进方式大概是实现所需的内核接口，并在用户空间栈中添加对它的支持。)

Contact: Christian König
(联系人：Christian König。)

Level: Starter
(难度：入门。)

DRM GPU Scheduler(DRM GPU 调度器)
=================================

Provide a universal successor for drm_sched_resubmit_jobs()(为 drm_sched_resubmit_jobs() 提供通用替代)
------------------------------------------------------------------------------------------------------

drm_sched_resubmit_jobs() is deprecated. Main reason being that it leads to
reinitializing dma_fences. See that function's docu for details. The better
approach for valid resubmissions by amdgpu and Xe is (apparently) to figure out
which job (and, through association: which entity) caused the hang. Then, the
job's buffer data, together with all other jobs' buffer data currently in the
same hardware ring, must be invalidated. This can for example be done by
overwriting it. amdgpu currently determines which jobs are in the ring and need
to be overwritten by keeping copies of the job. Xe obtains that information by
directly accessing drm_sched's pending_list.
(drm_sched_resubmit_jobs() 已被废弃，主要原因是它会导致 dma_fence 被重新初始化。详情见该函数的文档。对 amdgpu 和 Xe 的有效重新提交而言，更好的做法(显然)是找出是哪个 job(进而：哪个 entity)造成了挂起。然后必须使该 job 的缓冲区数据，连同当前位于同一硬件环 (ring) 中的所有其他 job 的缓冲区数据一起失效。例如可以通过覆写来做到这一点。amdgpu 目前通过保留 job 的副本来确定环中有哪些 job 需要覆写；Xe 则通过直接访问 drm_sched 的 pending_list 来获取该信息。)

Tasks:
(任务：)

1. implement scheduler functionality through which the driver can obtain the
   information which *broken* jobs are currently in the hardware ring.
   (实现调度器功能，使驱动能够获取当前位于硬件环中的 *损坏* job 的信息。)
2. Such infrastructure would then typically be used in
   drm_sched_backend_ops.timedout_job(). Document that.
   (这类基础设施之后通常会用在 drm_sched_backend_ops.timedout_job() 中。请为此撰写文档。)
3. Port a driver as first user.
   (移植一个驱动作为首个使用者。)
4. Document the new alternative in the docu of deprecated
   drm_sched_resubmit_jobs().
   (在已废弃的 drm_sched_resubmit_jobs() 的文档中说明新的替代方案。)

Contact: Christian König <christian.koenig@amd.com>
         Philipp Stanner <phasta@kernel.org>

(联系人：Christian König <christian.koenig@amd.com>、Philipp Stanner <phasta@kernel.org>。)

Level: Advanced
(难度：高级。)

Add locking for runqueues(为运行队列添加锁保护)
-----------------------------------------------

There is an old FIXME by Sima in include/drm/gpu_scheduler.h. It details that
struct drm_sched_rq is read at many places without any locks, not even with a
READ_ONCE. At XDC 2025 no one could really tell why that is the case, whether
locks are needed and whether they could be added. (But for real, that should
probably be locked!). Check whether it's possible to add locks everywhere, and
do so if yes.
(include/drm/gpu_scheduler.h 中有 Sima 留下的一个陈年 FIXME。其中详细说明 struct drm_sched_rq 在许多地方被无锁读取，甚至没有用 READ_ONCE。在 XDC 2025 上没人能真正说清为什么会是这样、是否需要加锁以及能否加上。(但说真的，这里恐怕就该加锁！)。请确认是否能在所有地方加上锁，可以的话就去做。)

Contact: Philipp Stanner <phasta@kernel.org>
(联系人：Philipp Stanner <phasta@kernel.org>。)

Level: Intermediate
(难度：中等。)

Outside DRM(DRM 之外)
=====================

Convert fbdev drivers to DRM(把 fbdev 驱动转换为 DRM)
-----------------------------------------------------

There are plenty of fbdev drivers for older hardware. Some hardware has
become obsolete, but some still provides good(-enough) framebuffers. The
drivers that are still useful should be converted to DRM and afterwards
removed from fbdev.
(面向较老硬件的 fbdev 驱动还有很多。其中一些硬件已经过时，但仍有一些能提供良好(或足够好)的 framebuffer。仍然有用的驱动应转换为 DRM，之后再从 fbdev 中移除。)

Very simple fbdev drivers can best be converted by starting with a new
DRM driver. Simple KMS helpers and SHMEM should be able to handle any
existing hardware. The new driver's call-back functions are filled from
existing fbdev code.
(非常简单的 fbdev 驱动最好从一个新的 DRM 驱动入手进行转换。Simple KMS 辅助函数和 SHMEM 应当能应付任何现有硬件。新驱动的回调函数可以从现有 fbdev 代码中填充。)

More complex fbdev drivers can be refactored step-by-step into a DRM
driver with the help of the DRM fbconv helpers [4]_. These helpers provide
the transition layer between the DRM core infrastructure and the fbdev
driver interface. Create a new DRM driver on top of the fbconv helpers,
copy over the fbdev driver, and hook it up to the DRM code. Examples for
several fbdev drivers are available in Thomas Zimmermann's fbconv tree
[4]_, as well as a tutorial of this process [5]_. The result is a primitive
DRM driver that can run X11 and Weston.
(更复杂的 fbdev 驱动可以借助 DRM fbconv 辅助函数 [4]_ 一步步重构为 DRM 驱动。这些辅助函数在 DRM 核心基础设施与 fbdev 驱动接口之间提供过渡层。在 fbconv 辅助函数之上创建一个新的 DRM 驱动，把 fbdev 驱动复制过来，并将其接入 DRM 代码。Thomas Zimmermann 的 fbconv 代码树 [4]_ 中有几个 fbdev 驱动的示例，还有该过程的教程 [5]_。最终得到的是一个能运行 X11 和 Weston 的初级 DRM 驱动。)

 .. [4] https://gitlab.freedesktop.org/tzimmermann/linux/tree/fbconv
 .. [5] https://gitlab.freedesktop.org/tzimmermann/linux/blob/fbconv/drivers/gpu/drm/drm_fbconv_helper.c

Contact: Thomas Zimmermann <tzimmermann@suse.de>
(联系人：Thomas Zimmermann <tzimmermann@suse.de>。)

Level: Advanced
(难度：高级。)
