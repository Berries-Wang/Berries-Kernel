ktime accessors(ktime 访问接口)
===============================

Device drivers can read the current time using ktime_get() and the many
related functions declared in linux/timekeeping.h. As a rule of thumb,
using an accessor with a shorter name is preferred over one with a longer
name if both are equally fit for a particular use case.
(设备驱动可以使用 ktime_get() 以及 linux/timekeeping.h 中声明的众多相关函数读取当前时间。根据经验法则，如果两个访问接口都同样适合某个特定用途，应优先选择名字更短的那个。)

Basic ktime_t based interfaces(基于 ktime_t 的基本接口)
-------------------------------------------------------

The recommended simplest form returns an opaque ktime_t, with variants
that return time for different clock references:
(推荐的最简形式返回一个不透明的 ktime_t，并有针对不同时钟基准返回时间的变体：)


.. c:function:: ktime_t ktime_get( void )

	CLOCK_MONOTONIC

	Useful for reliable timestamps and measuring short time intervals
	accurately. Starts at system boot time but stops during suspend.
	(适合用于可靠的时间戳以及精确测量较短的时间间隔。从系统启动时开始计时，但在挂起期间停止。)

.. c:function:: ktime_t ktime_get_boottime( void )

	CLOCK_BOOTTIME

	Like ktime_get(), but does not stop when suspended. This can be
	used e.g. for key expiration times that need to be synchronized
	with other machines across a suspend operation.
	(与 ktime_get() 类似，但在挂起时不会停止。例如可用于需要在挂起操作前后与其他机器保持同步的密钥过期时间。)

.. c:function:: ktime_t ktime_get_real( void )

	CLOCK_REALTIME

	Returns the time in relative to the UNIX epoch starting in 1970
	using the Coordinated Universal Time (UTC), same as gettimeofday()
	user space. This is used for all timestamps that need to
	persist across a reboot, like inode times, but should be avoided
	for internal uses, since it can jump backwards due to a leap
	second update, NTP adjustment settimeofday() operation from user
	space.
	(返回相对于 1970 年起始的 UNIX 纪元、采用协调世界时 (UTC) 的时间，与用户空间的 gettimeofday() 相同。用于所有需要跨重启持久保存的时间戳(如 inode 时间)，但内部使用应避免，因为闰秒更新、NTP 调整或用户空间的 settimeofday() 操作都可能使它向后跳变。)

.. c:function:: ktime_t ktime_get_clocktai( void )

	 CLOCK_TAI

	Like ktime_get_real(), but uses the International Atomic Time (TAI)
	reference instead of UTC to avoid jumping on leap second updates.
	This is rarely useful in the kernel.
	(与 ktime_get_real() 类似，但使用国际原子时 (TAI) 基准而非 UTC，以避免在闰秒更新时跳变。在内核中很少用到。)

.. c:function:: ktime_t ktime_get_raw( void )

	CLOCK_MONOTONIC_RAW

	Like ktime_get(), but runs at the same rate as the hardware
	clocksource without (NTP) adjustments for clock drift. This is
	also rarely needed in the kernel.
	(与 ktime_get() 类似，但以与硬件时钟源相同的速率运行，不做针对时钟漂移的 (NTP) 调整。在内核中也很少用到。)

nanosecond, timespec64, and second output(纳秒、timespec64 与秒级输出)
----------------------------------------------------------------------

For all of the above, there are variants that return the time in a
different format depending on what is required by the user:
(对上述所有接口，都存在按用户需求以不同格式返回时间的变体：)

.. c:function:: u64 ktime_get_ns( void )
		u64 ktime_get_boottime_ns( void )
		u64 ktime_get_real_ns( void )
		u64 ktime_get_clocktai_ns( void )
		u64 ktime_get_raw_ns( void )

	Same as the plain ktime_get functions, but returning a u64 number
	of nanoseconds in the respective time reference, which may be
	more convenient for some callers.
	(与普通 ktime_get 系列函数相同，但以 u64 返回相应时间基准下的纳秒数，对某些调用者来说可能更方便。)

.. c:function:: void ktime_get_ts64( struct timespec64 * )
		void ktime_get_boottime_ts64( struct timespec64 * )
		void ktime_get_real_ts64( struct timespec64 * )
		void ktime_get_clocktai_ts64( struct timespec64 * )
		void ktime_get_raw_ts64( struct timespec64 * )

	Same above, but returns the time in a 'struct timespec64', split
	into seconds and nanoseconds. This can avoid an extra division
	when printing the time, or when passing it into an external
	interface that expects a 'timespec' or 'timeval' structure.
	(与上面相同，但以 'struct timespec64' 形式返回时间，拆分为秒和纳秒。在打印时间，或把时间传给期望 'timespec' 或 'timeval' 结构的外部接口时，可以省去一次额外的除法。)

.. c:function:: time64_t ktime_get_seconds( void )
		time64_t ktime_get_boottime_seconds( void )
		time64_t ktime_get_real_seconds( void )
		time64_t ktime_get_clocktai_seconds( void )
		time64_t ktime_get_raw_seconds( void )

	Return a coarse-grained version of the time as a scalar
	time64_t. This avoids accessing the clock hardware and rounds
	down the seconds to the full seconds of the last timer tick
	using the respective reference.
	(以标量 time64_t 返回时间的粗粒度版本。这避免访问时钟硬件，并把秒数向下取整到相应时间基准下最后一次定时器节拍的整秒。)

Coarse and fast_ns access(Coarse 与 fast_ns 访问)
-------------------------------------------------

Some additional variants exist for more specialized cases:
(针对更专门的场景，还存在一些额外的变体：)

.. c:function:: ktime_t ktime_get_coarse( void )
		ktime_t ktime_get_coarse_boottime( void )
		ktime_t ktime_get_coarse_real( void )
		ktime_t ktime_get_coarse_clocktai( void )

.. c:function:: u64 ktime_get_coarse_ns( void )
		u64 ktime_get_coarse_boottime_ns( void )
		u64 ktime_get_coarse_real_ns( void )
		u64 ktime_get_coarse_clocktai_ns( void )

.. c:function:: void ktime_get_coarse_ts64( struct timespec64 * )
		void ktime_get_coarse_boottime_ts64( struct timespec64 * )
		void ktime_get_coarse_real_ts64( struct timespec64 * )
		void ktime_get_coarse_clocktai_ts64( struct timespec64 * )

	These are quicker than the non-coarse versions, but less accurate,
	corresponding to CLOCK_MONOTONIC_COARSE and CLOCK_REALTIME_COARSE
	in user space, along with the equivalent boottime/tai/raw
	timebase not available in user space.
	(这些接口比非 coarse 版本更快，但精度更低，对应用户空间中的 CLOCK_MONOTONIC_COARSE 与 CLOCK_REALTIME_COARSE，以及用户空间没有的等价 boottime/tai/raw 时间基准。)

	The time returned here corresponds to the last timer tick, which
	may be as much as 10ms in the past (for CONFIG_HZ=100), same as
	reading the 'jiffies' variable.  These are only useful when called
	in a fast path and one still expects better than second accuracy,
	but can't easily use 'jiffies', e.g. for inode timestamps.
	Skipping the hardware clock access saves around 100 CPU cycles
	on most modern machines with a reliable cycle counter, but
	up to several microseconds on older hardware with an external
	clocksource.
	(这里返回的时间对应最后一次定时器节拍，最多可能是 10ms 之前(CONFIG_HZ=100 时)的，与读取 'jiffies' 变量相同。它们只在快速路径中调用、且仍期望优于秒级精度但又不便使用 'jiffies' 的场景有用，例如 inode 时间戳。跳过硬件时钟访问，在大多数带有可靠周期计数器的现代机器上可节省约 100 个 CPU 周期，但在带外部时钟源的旧硬件上可节省多达数微秒。)

.. c:function:: u64 ktime_get_mono_fast_ns( void )
		u64 ktime_get_raw_fast_ns( void )
		u64 ktime_get_boot_fast_ns( void )
		u64 ktime_get_tai_fast_ns( void )
		u64 ktime_get_real_fast_ns( void )

	These variants are safe to call from any context, including from
	a non-maskable interrupt (NMI) during a timekeeper update, and
	while we are entering suspend with the clocksource powered down.
	This is useful in some tracing or debugging code as well as
	machine check reporting, but most drivers should never call them,
	since the time is allowed to jump under certain conditions.
	(这些变体可以从任何上下文安全调用，包括在 timekeeper 更新期间的非屏蔽中断 (NMI) 中，以及在我们正在进入挂起、时钟源已断电的情况下。这在某些跟踪或调试代码以及机器检查 (machine check) 报告中很有用，但大多数驱动永远不应调用它们，因为在某些条件下时间允许跳变。)

Deprecated time interfaces(已弃用的时间接口)
--------------------------------------------

Older kernels used some other interfaces that are now being phased out
but may appear in third-party drivers being ported here. In particular,
all interfaces returning a 'struct timeval' or 'struct timespec' have
been replaced because the tv_sec member overflows in year 2038 on 32-bit
architectures. These are the recommended replacements:
(较老的内核使用过另一些如今正被逐步淘汰的接口，它们可能出现在正在移植到这里的第三方驱动中。特别是，所有返回 'struct timeval' 或 'struct timespec' 的接口都已被替换，因为其 tv_sec 成员在 32 位架构上会在 2038 年溢出。以下是推荐的替代接口：)

.. c:function:: void ktime_get_ts( struct timespec * )

	Use ktime_get() or ktime_get_ts64() instead.
	(应改用 ktime_get() 或 ktime_get_ts64()。)

.. c:function:: void do_gettimeofday( struct timeval * )
		void getnstimeofday( struct timespec * )
		void getnstimeofday64( struct timespec64 * )
		void ktime_get_real_ts( struct timespec * )

	ktime_get_real_ts64() is a direct replacement, but consider using
	monotonic time (ktime_get_ts64()) and/or a ktime_t based interface
	(ktime_get()/ktime_get_real()).
	(ktime_get_real_ts64() 是直接替代品，但可以考虑使用单调时间 (ktime_get_ts64()) 和/或基于 ktime_t 的接口 (ktime_get()/ktime_get_real())。)

.. c:function:: struct timespec current_kernel_time( void )
		struct timespec64 current_kernel_time64( void )
		struct timespec get_monotonic_coarse( void )
		struct timespec64 get_monotonic_coarse64( void )

	These are replaced by ktime_get_coarse_real_ts64() and
	ktime_get_coarse_ts64(). However, A lot of code that wants
	coarse-grained times can use the simple 'jiffies' instead, while
	some drivers may actually want the higher resolution accessors
	these days.
	(它们已被 ktime_get_coarse_real_ts64() 与 ktime_get_coarse_ts64() 取代。不过，如今许多想要粗粒度时间的代码可以改用简单的 'jiffies'，而某些驱动可能确实需要分辨率更高的访问接口。)

.. c:function:: struct timespec getrawmonotonic( void )
		struct timespec64 getrawmonotonic64( void )
		struct timespec timekeeping_clocktai( void )
		struct timespec64 timekeeping_clocktai64( void )
		struct timespec get_monotonic_boottime( void )
		struct timespec64 get_monotonic_boottime64( void )

	These are replaced by ktime_get_raw()/ktime_get_raw_ts64(),
	ktime_get_clocktai()/ktime_get_clocktai_ts64() as well
	as ktime_get_boottime()/ktime_get_boottime_ts64().
	However, if the particular choice of clock source is not
	important for the user, consider converting to
	ktime_get()/ktime_get_ts64() instead for consistency.
	(它们分别被 ktime_get_raw()/ktime_get_raw_ts64()、ktime_get_clocktai()/ktime_get_clocktai_ts64() 以及 ktime_get_boottime()/ktime_get_boottime_ts64() 取代。不过，如果具体选择哪种时钟源对使用者并不重要，为保持一致性可考虑改用 ktime_get()/ktime_get_ts64()。)
