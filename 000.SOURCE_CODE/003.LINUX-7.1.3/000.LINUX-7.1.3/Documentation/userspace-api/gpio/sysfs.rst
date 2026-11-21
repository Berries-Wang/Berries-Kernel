==================================================================
GPIO Sysfs Interface for Userspace(面向用户空间的 GPIO Sysfs 接口)
==================================================================

.. warning::
   This API is obsoleted by the chardev.rst and the ABI documentation has
   been moved to Documentation/ABI/obsolete/sysfs-gpio.
   (此 API 已被 chardev.rst 取代，其 ABI 文档已移至
   Documentation/ABI/obsolete/sysfs-gpio。)

   New developments should use the chardev.rst, and existing developments are
   encouraged to migrate as soon as possible, as this API will be removed
   in the future.
   (新的开发应使用 chardev.rst，并鼓励现有开发尽快迁移，因为此 API 将来会被移除。)

   This interface will continue to be maintained for the migration period,
   but new features will only be added to the new API.
   (在迁移期间，此接口将继续得到维护，但新功能只会添加到新 API 中。)

The obsolete sysfs ABI(已废弃的 sysfs ABI)
------------------------------------------
Platforms which use the "gpiolib" implementors framework may choose to
configure a sysfs user interface to GPIOs. This is different from the
debugfs interface, since it provides control over GPIO direction and
value instead of just showing a gpio state summary. Plus, it could be
present on production systems without debugging support.
(使用 "gpiolib" 实现者框架的平台可以选择配置一个面向 GPIO 的 sysfs 用户接口。这与 debugfs 接口不同，因为它提供对 GPIO 方向和值的控制，而不仅仅是显示 GPIO 状态摘要。此外，它可以存在于没有调试支持的生产系统上。)

Given appropriate hardware documentation for the system, userspace could
know for example that GPIO #23 controls the write protect line used to
protect boot loader segments in flash memory. System upgrade procedures
may need to temporarily remove that protection, first importing a GPIO,
then changing its output state, then updating the code before re-enabling
the write protection. In normal use, GPIO #23 would never be touched,
and the kernel would have no need to know about it.
(有了针对系统的适当硬件文档，用户空间就可以知道例如 GPIO #23 控制用于保护闪存中引导加载程序段的写保护线。系统升级过程可能需要暂时移除该保护：先导入一个 GPIO，然后改变其输出状态，再更新代码，之后重新启用写保护。在正常使用中，GPIO #23 永远不会被触碰，内核也无需知道它。)

Again depending on appropriate hardware documentation, on some systems
userspace GPIO can be used to determine system configuration data that
standard kernels won't know about. And for some tasks, simple userspace
GPIO drivers could be all that the system really needs.
(同样取决于适当的硬件文档，在某些系统上，用户空间 GPIO 可用于确定标准内核不会了解的系统配置数据。而对于某些任务，简单的用户空间 GPIO 驱动可能就是系统真正所需的全部。)

.. note::
   Do NOT abuse sysfs to control hardware that has proper kernel drivers.
   Please read Documentation/driver-api/gpio/drivers-on-gpio.rst
   to avoid reinventing kernel wheels in userspace.
   (不要滥用 sysfs 去控制已经有合适内核驱动的硬件。请阅读
   Documentation/driver-api/gpio/drivers-on-gpio.rst，避免在用户空间重复
   造内核已有的轮子。)

   I MEAN IT. REALLY.
   (我是认真的。真的。)

Paths in Sysfs(Sysfs 中的路径)
------------------------------
There are three kinds of entries in /sys/class/gpio:
(/sys/class/gpio 中有三类条目：)

   -	Control interfaces used to get userspace control over GPIOs;
	(控制接口，用于让用户空间获得对 GPIO 的控制；)

   -	GPIOs themselves; and
	(GPIO 本身；以及)

   -	GPIO controllers ("gpio_chip" instances).
	(GPIO 控制器("gpio_chip" 实例)。)

That's in addition to standard files including the "device" symlink.
(这是除标准文件(包括 "device" 符号链接)之外的补充。)

The control interfaces are write-only:
(控制接口是只写的：)

    /sys/class/gpio/

	"export" ...
		Userspace may ask the kernel to export control of
		a GPIO to userspace by writing its number to this file.
		(用户空间可以通过向该文件写入某个 GPIO 的编号，
		请求内核将该 GPIO 的控制权导出到用户空间。)

		Example:  "echo 19 > export" will create a "gpio19" node
		for GPIO #19, if that's not requested by kernel code.
		(例如： "echo 19 > export" 会为 GPIO #19 创建一个
		"gpio19" 节点——前提是内核代码尚未请求该 GPIO。)

	"unexport" ...
		Reverses the effect of exporting to userspace.
		(逆转导出到用户空间的效果。)

		Example:  "echo 19 > unexport" will remove a "gpio19"
		node exported using the "export" file.
		(例如： "echo 19 > unexport" 会删除之前使用 "export"
		文件导出的 "gpio19" 节点。)

GPIO signals have paths like /sys/class/gpio/gpio42/ (for GPIO #42)
and have the following read/write attributes:
(GPIO 信号具有类似 /sys/class/gpio/gpio42/ 的路径(对于 GPIO #42)，
并具有以下读/写属性：)

    /sys/class/gpio/gpioN/

	"direction" ...
		reads as either "in" or "out". This value may
		normally be written. Writing as "out" defaults to
		initializing the value as low. To ensure glitch free
		operation, values "low" and "high" may be written to
		configure the GPIO as an output with that initial value.
		(读取结果为 "in" 或 "out"。该值通常可以写入。写入
		"out" 默认将值初始化为低电平。为确保无毛刺 (glitch
		free) 操作，可以写入 "low" 和 "high" 值，将 GPIO 配置
		为具有该初始值的输出。)

		Note that this attribute *will not exist* if the kernel
		doesn't support changing the direction of a GPIO, or
		it was exported by kernel code that didn't explicitly
		allow userspace to reconfigure this GPIO's direction.
		(注意，如果内核不支持更改 GPIO 的方向，或者该 GPIO
		是由未明确允许用户空间重新配置其方向的内核代码导出的，
		那么该属性*将不存在*。)

	"value" ...
		reads as either 0 (inactive) or 1 (active). If the GPIO
		is configured as an output, this value may be written;
		any nonzero value is treated as active.
		(读取结果为 0 (inactive，无效) 或 1 (active，有效)。
		如果 GPIO 被配置为输出，则可以写入该值；任何非零值
		都被视为有效。)

		If the pin can be configured as interrupt-generating interrupt
		and if it has been configured to generate interrupts (see the
		description of "edge"), you can poll(2) on that file and
		poll(2) will return whenever the interrupt was triggered. If
		you use poll(2), set the events POLLPRI and POLLERR. If you
		use select(2), set the file descriptor in exceptfds. After
		poll(2) returns, use pread(2) to read the value at offset
		zero. Alternatively, either lseek(2) to the beginning of the
		sysfs file and read the new value or close the file and
		re-open it to read the value.
		(如果该引脚可以配置为产生中断，并且已被配置为产生中断
		(参见 "edge" 的描述)，你可以对该文件使用 poll(2)，每当
		中断触发时 poll(2) 就会返回。如果使用 poll(2)，请设置
		POLLPRI 和 POLLERR 事件。如果使用 select(2)，请将文件
		描述符放入 exceptfds。在 poll(2) 返回后，使用 pread(2)
		在偏移量为零处读取该值。或者，可以将 sysfs 文件
		lseek(2) 到开头并读取新值，或者关闭文件并重新打开它
		来读取该值。)

	"edge" ...
		reads as either "none", "rising", "falling", or
		"both". Write these strings to select the signal edge(s)
		that will make poll(2) on the "value" file return.
		(读取结果为 "none"、"rising"、"falling" 或 "both"。
		写入这些字符串以选择使 "value" 文件上的 poll(2) 返回
		的信号边沿。)

		This file exists only if the pin can be configured as an
		interrupt generating input pin.
		(仅当该引脚可以配置为产生中断的输入引脚时，此文件
		才存在。)

	"active_low" ...
		reads as either 0 (false) or 1 (true). Write
		any nonzero value to invert the value attribute both
		for reading and writing. Existing and subsequent
		poll(2) support configuration via the edge attribute
		for "rising" and "falling" edges will follow this
		setting.
		(读取结果为 0 (false，假) 或 1 (true，真)。写入任何
		非零值将在读取和写入两个方向上都反转 value 属性。
		现有的和后续的对 "rising" 和 "falling" 边沿通过 edge
		属性进行的 poll(2) 支持配置都会遵循此设置。)

GPIO controllers have paths like /sys/class/gpio/gpiochip42/ (for the
controller implementing GPIOs starting at #42) and have the following
read-only attributes:
(GPIO 控制器具有类似 /sys/class/gpio/gpiochip42/ 的路径(对于从
#42 开始实现 GPIO 的控制器)，并具有以下只读属性：)

    /sys/class/gpio/gpiochipN/

	"base" ...
		same as N, the first GPIO managed by this chip
		(与 N 相同，即该芯片管理的第一个 GPIO)

	"label" ...
		provided for diagnostics (not always unique)
		(提供用于诊断(不总是唯一))

	"ngpio" ...
		how many GPIOs this manages (N to N + ngpio - 1)
		(它管理多少个 GPIO(N 到 N + ngpio - 1))

Board documentation should in most cases cover what GPIOs are used for
what purposes. However, those numbers are not always stable; GPIOs on
a daughtercard might be different depending on the base board being used,
or other cards in the stack. In such cases, you may need to use the
gpiochip nodes (possibly in conjunction with schematics) to determine
the correct GPIO number to use for a given signal.
(开发板文档在大多数情况下应涵盖哪些 GPIO 用于哪些用途。然而，
这些编号并不总是稳定的：子卡 (daughtercard) 上的 GPIO 可能因所
使用的基板或堆叠中的其他板卡而不同。在这种情况下，你可能需要
使用 gpiochip 节点(可能结合原理图)来确定给定信号应使用的正确
GPIO 编号。)


Exporting from Kernel code(从内核代码导出)
------------------------------------------
Kernel code can explicitly manage exports of GPIOs which have already been
requested using gpio_request()::

	/* export the GPIO to userspace */
	int gpiod_export(struct gpio_desc *desc, bool direction_may_change);

	/* reverse gpiod_export() */
	void gpiod_unexport(struct gpio_desc *desc);

	/* create a sysfs link to an exported GPIO node */
	int gpiod_export_link(struct device *dev, const char *name,
		      struct gpio_desc *desc);

(内核代码可以显式管理已经通过 gpio_request() 请求的 GPIO 的导出：)

After a kernel driver requests a GPIO, it may only be made available in
the sysfs interface by gpiod_export(). The driver can control whether the
signal direction may change. This helps drivers prevent userspace code
from accidentally clobbering important system state.
(在内核驱动请求一个 GPIO 之后，只能通过 gpiod_export() 将其提供给 sysfs 接口。驱动可以控制信号方向是否可以更改。这有助于驱动防止用户空间代码意外破坏重要的系统状态。)

This explicit exporting can help with debugging (by making some kinds
of experiments easier), or can provide an always-there interface that's
suitable for documenting as part of a board support package.
(这种显式导出有助于调试(让某些类型的实验更容易进行)，或者可以提供一个始终存在的接口，适合作为板级支持包的一部分来编写文档。)

After the GPIO has been exported, gpiod_export_link() allows creating
symlinks from elsewhere in sysfs to the GPIO sysfs node. Drivers can
use this to provide the interface under their own device in sysfs with
a descriptive name.
(在 GPIO 导出之后，gpiod_export_link() 允许从 sysfs 中的其他位置创建指向该 GPIO sysfs 节点的符号链接。驱动可以使用它在自己设备的 sysfs 条目下提供一个具有描述性名称的接口。)
