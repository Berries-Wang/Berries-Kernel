=========================================================
(How to avoid) Botching up ioctls((如何避免) 搞砸 ioctl)
=========================================================

From: https://blog.ffwll.ch/2013/11/botching-up-ioctls.html

By: Daniel Vetter, Copyright © 2013 Intel Corporation

One clear insight kernel graphics hackers gained in the past few years is that
trying to come up with a unified interface to manage the execution units and
memory on completely different GPUs is a futile effort. So nowadays every
driver has its own set of ioctls to allocate memory and submit work to the GPU.
Which is nice, since there's no more insanity in the form of fake-generic, but
actually only used once interfaces. But the clear downside is that there's much
more potential to screw things up.
(过去几年，内核图形黑客获得的一个清晰认识是：试图为完全不同的 GPU 设计一个统一的管理执行单元与内存的接口是徒劳的。因此如今每个驱动都有自己的一套 ioctl 来分配内存并向 GPU 提交工作。这挺好，因为再也没有那种伪通用、实际只被用一次的疯狂接口了。但明显的坏处是，把事情搞砸的可能性大大增加了。)

To avoid repeating all the same mistakes again I've written up some of the
lessons learned while botching the job for the drm/i915 driver. Most of these
only cover technicalities and not the big-picture issues like what the command
submission ioctl exactly should look like. Learning these lessons is probably
something every GPU driver has to do on its own.
(为了避免重蹈覆辙，我总结了在为 drm/i915 驱动搞砸这个活儿的过程中学到的一些教训。其中大多数只涉及技术细节，而不涉及命令提交 ioctl 究竟应当是什么样这类全局性问题。学习这些教训大概是每个 GPU 驱动都得自己经历的事。)


Prerequisites(前提条件)
-----------------------

First the prerequisites. Without these you have already failed, because you
will need to add a 32-bit compat layer:
(首先是前提条件。不具备这些你就已经失败了，因为你将需要添加 32 位兼容层：)

 * Only use fixed sized integers. To avoid conflicts with typedefs in userspace
   the kernel has special types like __u32, __s64. Use them.
   (只使用定长整数。为避免与用户态中的 typedef 冲突，内核有 __u32、__s64 这类特殊类型。请使用它们。)

 * Align everything to the natural size and use explicit padding. 32-bit
   platforms don't necessarily align 64-bit values to 64-bit boundaries, but
   64-bit platforms do. So we always need padding to the natural size to get
   this right.
   (把一切都对齐到自然大小并使用显式填充。32 位平台不一定把 64 位值对齐到 64 位边界，但 64 位平台会。所以为了做对，我们总是需要填充到自然大小。)

 * Pad the entire struct to a multiple of 64-bits if the structure contains
   64-bit types - the structure size will otherwise differ on 32-bit versus
   64-bit. Having a different structure size hurts when passing arrays of
   structures to the kernel, or if the kernel checks the structure size, which
   e.g. the drm core does.
   (如果结构体包含 64 位类型，则把整个结构体填充到 64 位的倍数——否则结构体大小在 32 位与 64 位上会不同。当向内核传递结构体数组，或内核检查结构体大小时 (例如 drm core 就这么做)，结构体大小不同会带来麻烦。)

 * Pointers are __u64, cast from/to a uintptr_t on the userspace side and
   from/to a void __user * in the kernel. Try really hard not to delay this
   conversion or worse, fiddle the raw __u64 through your code since that
   diminishes the checking tools like sparse can provide. The macro
   u64_to_user_ptr can be used in the kernel to avoid warnings about integers
   and pointers of different sizes.
   (指针用 __u64，在用户态一侧与 uintptr_t 相互转换，在内核中与 void __user * 相互转换。尽力不要推迟这种转换，更不要把裸 __u64 在代码里传来传去，因为那会削弱 sparse 之类的检查工具所能提供的检查。在内核中可以使用宏 u64_to_user_ptr 来避免关于不同大小整数与指针的警告。)


Basics(基础知识)
----------------

With the joys of writing a compat layer avoided we can take a look at the basic
fumbles. Neglecting these will make backward and forward compatibility a real
pain. And since getting things wrong on the first attempt is guaranteed you
will have a second iteration or at least an extension for any given interface.
(避开了编写兼容层的"乐趣"之后，我们来看看基本的失误。忽视这些会让向后与向前兼容变得非常痛苦。而且既然第一次就搞对是注定不可能的，任何给定接口都会有第二次迭代，或至少一次扩展。)

 * Have a clear way for userspace to figure out whether your new ioctl or ioctl
   extension is supported on a given kernel. If you can't rely on old kernels
   rejecting the new flags/modes or ioctls (since doing that was botched in the
   past) then you need a driver feature flag or revision number somewhere.
   (要有清晰的方法让用户态判断给定内核是否支持你的新 ioctl 或 ioctl 扩展。如果不能指望旧内核拒绝新标志/模式或新 ioctl (因为过去这方面已经搞砸过)，那么你需要在某处提供驱动特性标志或版本号。)

 * Have a plan for extending ioctls with new flags or new fields at the end of
   the structure. The drm core checks the passed-in size for each ioctl call
   and zero-extends any mismatches between kernel and userspace. That helps,
   but isn't a complete solution since newer userspace on older kernels won't
   notice that the newly added fields at the end get ignored. So this still
   needs a new driver feature flags.
   (要有在结构体末尾以新标志或新字段扩展 ioctl 的计划。drm core 会检查每次 ioctl 调用传入的大小，并把内核与用户态之间的不匹配做零扩展。这有帮助，但不是完整方案，因为在旧内核上运行的新用户态不会注意到末尾新加的字段被忽略了。所以这仍然需要新的驱动特性标志。)

 * Check all unused fields and flags and all the padding for whether it's 0,
   and reject the ioctl if that's not the case. Otherwise your nice plan for
   future extensions is going right down the gutters since someone will submit
   an ioctl struct with random stack garbage in the yet unused parts. Which
   then bakes in the ABI that those fields can never be used for anything else
   but garbage. This is also the reason why you must explicitly pad all
   structures, even if you never use them in an array - the padding the compiler
   might insert could contain garbage.
   (检查所有未用字段、标志以及所有填充是否为 0，如果不是则拒绝该 ioctl。否则你为未来扩展准备的美好计划会付诸东流，因为有人会提交一个在尚未使用的部分带着随机栈垃圾的 ioctl 结构体。这会把"这些字段永远只能用于垃圾"固化进 ABI。这也是必须显式填充所有结构体的原因，即使你从不在数组中使用它们——编译器可能插入的填充里可能含垃圾。)

 * Have simple testcases for all of the above.
   (为以上所有内容准备简单的测试用例。)


Fun with Error Paths(错误路径的乐趣)
------------------------------------

Nowadays we don't have any excuse left any more for drm drivers being neat
little root exploits. This means we both need full input validation and solid
error handling paths - GPUs will die eventually in the oddmost corner cases
anyway:
(如今，我们已经没有任何借口让 drm 驱动成为精巧的小型 root 提权漏洞。这意味着我们既需要完整的输入校验，也需要坚实的错误处理路径——反正 GPU 最终会在最古怪的角落场景中挂掉：)

 * The ioctl must check for array overflows. Also it needs to check for
   over/underflows and clamping issues of integer values in general. The usual
   example is sprite positioning values fed directly into the hardware with the
   hardware just having 12 bits or so. Works nicely until some odd display
   server doesn't bother with clamping itself and the cursor wraps around the
   screen.
   (ioctl 必须检查数组溢出。一般而言它还需要检查整数值的上/下溢与截断 (clamping) 问题。常见的例子是把精灵 (sprite) 定位值直接喂给只有 12 位左右的硬件。一切正常，直到某个古怪的显示服务器自己不做截断，导致光标绕屏幕回绕。)

 * Have simple testcases for every input validation failure case in your ioctl.
   Check that the error code matches your expectations. And finally make sure
   that you only test for one single error path in each subtest by submitting
   otherwise perfectly valid data. Without this an earlier check might reject
   the ioctl already and shadow the codepath you actually want to test, hiding
   bugs and regressions.
   (为 ioctl 中每个输入校验失败用例准备简单的测试用例。检查错误码是否符合你的预期。最后确保每个子测试通过提交其他方面完全合法的数据、只测试单一错误路径。否则更早的检查可能已经拒绝了 ioctl，遮蔽你真正想测试的代码路径，掩盖缺陷与退化。)

 * Make all your ioctls restartable. First X really loves signals and second
   this will allow you to test 90% of all error handling paths by just
   interrupting your main test suite constantly with signals. Thanks to X's
   love for signal you'll get an excellent base coverage of all your error
   paths pretty much for free for graphics drivers. Also, be consistent with
   how you handle ioctl restarting - e.g. drm has a tiny drmIoctl helper in its
   userspace library. The i915 driver botched this with the set_tiling ioctl,
   now we're stuck forever with some arcane semantics in both the kernel and
   userspace.
   (让所有 ioctl 都可重启。一来 X 真的很爱信号，二来这让你只需用信号不断打断主测试套件，就能测试 90% 的错误处理路径。多亏 X 对信号的热爱，图形驱动几乎免费获得了所有错误路径的出色基础覆盖。另外，处理 ioctl 重启的方式要保持一致——例如 drm 在其用户态库里有个小小的 drmIoctl 辅助函数。i915 驱动用 set_tiling ioctl 把这事搞砸了，现在我们永远被困在内核与用户态中的某些玄学语义里。)

 * If you can't make a given codepath restartable make a stuck task at least
   killable. GPUs just die and your users won't like you more if you hang their
   entire box (by means of an unkillable X process). If the state recovery is
   still too tricky have a timeout or hangcheck safety net as a last-ditch
   effort in case the hardware has gone bananas.
   (如果无法让某个代码路径可重启，至少要让卡住的任务可杀死。GPU 就是会挂，而如果你把用户的整台机器都挂住 (用一个杀不掉的 X 进程)，用户会更讨厌你。如果状态恢复仍然太棘手，就准备一个超时或 hangcheck 安全网作为硬件彻底疯掉时的最后手段。)

 * Have testcases for the really tricky corner cases in your error recovery code
   - it's way too easy to create a deadlock between your hangcheck code and
   waiters.
   (为错误恢复代码中真正棘手的角落用例准备测试用例——在 hangcheck 代码与等待者之间制造死锁实在太容易了。)


Time, Waiting and Missing it(时间、等待与错过)
----------------------------------------------

GPUs do most everything asynchronously, so we have a need to time operations and
wait for outstanding ones. This is really tricky business; at the moment none of
the ioctls supported by the drm/i915 get this fully right, which means there's
still tons more lessons to learn here.
(GPU 的大部分事情都是异步做的，因此我们需要给操作计时并等待未完成的操作。这是非常棘手的事；目前 drm/i915 支持的 ioctl 没有一个完全做对，这意味着这里还有很多教训要吸取。)

 * Use CLOCK_MONOTONIC as your reference time, always. It's what alsa, drm and
   v4l use by default nowadays. But let userspace know which timestamps are
   derived from different clock domains like your main system clock (provided
   by the kernel) or some independent hardware counter somewhere else. Clocks
   will mismatch if you look close enough, but if performance measuring tools
   have this information they can at least compensate. If your userspace can
   get at the raw values of some clocks (e.g. through in-command-stream
   performance counter sampling instructions) consider exposing those also.
   (始终使用 CLOCK_MONOTONIC 作为参考时间。如今 alsa、drm 与 v4l 默认都用它。但要让用户态知道哪些时间戳来自不同的时钟域，例如主系统时钟 (由内核提供) 或别处某个独立硬件计数器。凑得足够近看，时钟总会有偏差，但如果性能测量工具拥有这一信息，它们至少可以补偿。如果你的用户态能拿到某些时钟的原始值 (例如通过命令流内的性能计数器采样指令)，考虑也把它们暴露出来。)

 * Use __s64 seconds plus __u64 nanoseconds to specify time. It's not the most
   convenient time specification, but it's mostly the standard.
   (用 __s64 秒加 __u64 纳秒来表示时间。这不是最方便的时间表示法，但它基本就是标准。)

 * Check that input time values are normalized and reject them if not. Note
   that the kernel native struct ktime has a signed integer for both seconds
   and nanoseconds, so beware here.
   (检查输入的时间值是否已规范化，未规范化则拒绝。注意内核原生的 struct ktime 中秒与纳秒都是有符号整数，所以要当心。)

 * For timeouts, use absolute times. If you're a good fellow and made your
   ioctl restartable relative timeouts tend to be too coarse and can
   indefinitely extend your wait time due to rounding on each restart.
   Especially if your reference clock is something really slow like the display
   frame counter. With a spec lawyer hat on this isn't a bug since timeouts can
   always be extended - but users will surely hate you if their neat animations
   starts to stutter due to this.
   (超时使用绝对时间。如果你是个好同志，把 ioctl 做成了可重启的，相对超时往往太粗糙，且由于每次重启时的舍入，会无限延长你的等待时间。尤其当你的参考时钟是显示帧计数器这种非常慢的东西时。戴上规范律师的帽子说，这不算缺陷，因为超时总是可以延长——但如果用户优雅的动画因此开始卡顿，他们肯定会恨你。)

 * Consider ditching any synchronous wait ioctls with timeouts and just deliver
   an asynchronous event on a pollable file descriptor. It fits much better
   into event driven applications' main loop.
   (考虑抛弃任何带超时的同步等待 ioctl，改为在可轮询的文件描述符上投递异步事件。它更适合事件驱动应用的主循环。)

 * Have testcases for corner-cases, especially whether the return values for
   already-completed events, successful waits and timed-out waits are all sane
   and suiting to your needs.
   (为角落用例准备测试用例，尤其要检查已完成事件、成功等待与超时等待的返回值是否都合理并符合你的需求。)


Leaking Resources, Not(不泄露资源)
----------------------------------

A full-blown drm driver essentially implements a little OS, but specialized to
the given GPU platforms. This means a driver needs to expose tons of handles
for different objects and other resources to userspace. Doing that right
entails its own little set of pitfalls:
(一个完整的 drm 驱动本质上实现了一个小型操作系统，只是针对给定 GPU 平台特化。这意味着驱动需要向用户态暴露大量句柄，用于不同的对象及其他资源。要把这件事做对，自有一套小坑：)

 * Always attach the lifetime of your dynamically created resources to the
   lifetime of a file descriptor. Consider using a 1:1 mapping if your resource
   needs to be shared across processes -  fd-passing over unix domain sockets
   also simplifies lifetime management for userspace.
   (始终把动态创建资源的生命周期挂接到某个文件描述符的生命周期上。如果你的资源需要跨进程共享，考虑使用 1:1 映射——通过 unix 域套接字传递 fd 也简化了用户态的生命周期管理。)

 * Always have O_CLOEXEC support.
   (始终支持 O_CLOEXEC。)

 * Ensure that you have sufficient insulation between different clients. By
   default pick a private per-fd namespace which forces any sharing to be done
   explicitly. Only go with a more global per-device namespace if the objects
   are truly device-unique. One counterexample in the drm modeset interfaces is
   that the per-device modeset objects like connectors share a namespace with
   framebuffer objects, which mostly are not shared at all. A separate
   namespace, private by default, for framebuffers would have been more
   suitable.
   (确保不同客户端之间有足够的隔离。默认选择按 fd 私有的命名空间，强制任何共享都必须显式进行。只有对象真正是设备唯一时才采用更全局的按设备命名空间。drm 模式设置接口里有一个反例：像连接器 (connector) 这类按设备的 modeset 对象与 framebuffer 对象共享命名空间，而后者基本完全不被共享。为 framebuffer 单独设一个默认私有的命名空间会更合适。)

 * Think about uniqueness requirements for userspace handles. E.g. for most drm
   drivers it's a userspace bug to submit the same object twice in the same
   command submission ioctl. But then if objects are shareable userspace needs
   to know whether it has seen an imported object from a different process
   already or not. I haven't tried this myself yet due to lack of a new class
   of objects, but consider using inode numbers on your shared file descriptors
   as unique identifiers - it's how real files are told apart, too.
   Unfortunately this requires a full-blown virtual filesystem in the kernel.
   (考虑用户态句柄的唯一性要求。例如对多数 drm 驱动而言，在同一个命令提交 ioctl 中提交同一对象两次是用户态缺陷。但既然对象可共享，用户态就需要知道自己是否已经见过从其他进程导入的对象。由于缺少一类新对象，我自己还没试过，但考虑用共享文件描述符上的 inode 号作为唯一标识符——真实文件也是这样区分的。遗憾的是这需要在内核中有一个完整的虚拟文件系统。)


Last, but not Least(最后但同样重要)
-----------------------------------

Not every problem needs a new ioctl:
(不是每个问题都需要一个新 ioctl：)

 * Think hard whether you really want a driver-private interface. Of course
   it's much quicker to push a driver-private interface than engaging in
   lengthy discussions for a more generic solution. And occasionally doing a
   private interface to spearhead a new concept is what's required. But in the
   end, once the generic interface comes around you'll end up maintaining two
   interfaces. Indefinitely.
   (认真想想你是否真的想要驱动私有接口。当然，推一个驱动私有接口比为了更通用的方案参与冗长讨论快得多。而且偶尔用一个私有接口来开创新概念也是必要的。但最终，一旦通用接口出现，你就得维护两套接口。无限期地。)

 * Consider other interfaces than ioctls. A sysfs attribute is much better for
   per-device settings, or for child objects with fairly static lifetimes (like
   output connectors in drm with all the detection override attributes). Or
   maybe only your testsuite needs this interface, and then debugfs with its
   disclaimer of not having a stable ABI would be better.
   (考虑 ioctl 之外的接口。对按设备的设置，或对生命周期相当静态的子对象 (例如 drm 中带有全部检测覆盖属性的输出连接器)，sysfs 属性要好得多。或者也许只有你的测试套件需要这个接口，那么用声明不保证稳定 ABI 的 debugfs 会更好。)

Finally, the name of the game is to get it right on the first attempt, since if
your driver proves popular and your hardware platforms long-lived then you'll
be stuck with a given ioctl essentially forever. You can try to deprecate
horrible ioctls on newer iterations of your hardware, but generally it takes
years to accomplish this. And then again years until the last user able to
complain about regressions disappears, too.
(最后，这个游戏的名字叫"一次做对"，因为如果你的驱动足够流行、你的硬件平台足够长寿，那么你基本上会永远被某个给定的 ioctl 套牢。你可以尝试在硬件的新版本上废弃糟糕的 ioctl，但一般这要花好几年才能完成。之后还要再过好几年，直到最后一个能够抱怨退化的用户消失为止。)
