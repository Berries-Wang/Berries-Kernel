.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _mmap:

***************************************************
Streaming I/O (Memory Mapping)(流式 I/O (内存映射))
***************************************************

Input and output devices support this I/O method when the
``V4L2_CAP_STREAMING`` flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl is set. There are two
streaming methods, to determine if the memory mapping flavor is
supported applications must call the :ref:`VIDIOC_REQBUFS` ioctl
with the memory type set to ``V4L2_MEMORY_MMAP``.
(当 VIDIOC_QUERYCAP ioctl 返回的 struct v4l2_capability 的
``capabilities`` 字段中设置了 ``V4L2_CAP_STREAMING`` 标志时，输入和输出
设备支持这种 I/O 方法。流式方法有两种；要确定是否支持内存映射这一变体，
应用程序必须调用 VIDIOC_REQBUFS ioctl，并把内存类型设置为
``V4L2_MEMORY_MMAP``。)

Streaming is an I/O method where only pointers to buffers are exchanged
between application and driver, the data itself is not copied. Memory
mapping is primarily intended to map buffers in device memory into the
application's address space. Device memory can be for example the video
memory on a graphics card with a video capture add-on. However, being
the most efficient I/O method available for a long time, many other
drivers support streaming as well, allocating buffers in DMA-able main
memory.
(流式是一种 I/O 方法，其中应用程序与驱动之间只交换指向缓冲区的指针，数据
本身不被复制。内存映射的主要目的是把设备内存中的缓冲区映射到应用程序的
地址空间。设备内存例如可以是带有视频采集附加卡的显卡上的显存。不过，由于
它是长期以来可用的最高效 I/O 方法，许多其他驱动也支持流式，在可 DMA 的
主内存中分配缓冲区。)

A driver can support many sets of buffers. Each set is identified by a
unique buffer type value. The sets are independent and each set can hold
a different type of data. To access different sets at the same time
different file descriptors must be used. [#f1]_
(一个驱动可以支持多组缓冲区。每一组由一个唯一的缓冲区类型值标识。各组
相互独立，每组可以容纳不同类型的数据。要同时访问不同的组，必须使用不同
的文件描述符。)

To allocate device buffers applications call the
:ref:`VIDIOC_REQBUFS` ioctl with the desired number
of buffers and buffer type, for example ``V4L2_BUF_TYPE_VIDEO_CAPTURE``.
This ioctl can also be used to change the number of buffers or to free
the allocated memory, provided none of the buffers are still mapped.
(要分配设备缓冲区，应用程序调用 VIDIOC_REQBUFS ioctl，指定期望的缓冲区
数量与缓冲区类型，例如 ``V4L2_BUF_TYPE_VIDEO_CAPTURE``。该 ioctl 也可以
用于更改缓冲区数量或释放已分配的内存，前提是没有缓冲区仍处于映射
状态。)

Before applications can access the buffers they must map them into their
address space with the :c:func:`mmap()` function. The
location of the buffers in device memory can be determined with the
:ref:`VIDIOC_QUERYBUF` ioctl. In the single-planar
API case, the ``m.offset`` and ``length`` returned in a struct
:c:type:`v4l2_buffer` are passed as sixth and second
parameter to the :c:func:`mmap()` function. When using the
multi-planar API, struct :c:type:`v4l2_buffer` contains an
array of struct :c:type:`v4l2_plane` structures, each
containing its own ``m.offset`` and ``length``. When using the
multi-planar API, every plane of every buffer has to be mapped
separately, so the number of calls to :c:func:`mmap()` should
be equal to number of buffers times number of planes in each buffer. The
offset and length values must not be modified. Remember, the buffers are
allocated in physical memory, as opposed to virtual memory, which can be
swapped out to disk. Applications should free the buffers as soon as
possible with the :c:func:`munmap()` function.
(在应用程序能够访问缓冲区之前，必须用 mmap() 函数把它们映射到自己的
地址空间。缓冲区在设备内存中的位置可以用 VIDIOC_QUERYBUF ioctl 确定。
在单平面 API 的情况下，struct v4l2_buffer 中返回的 ``m.offset`` 与
``length`` 作为第六个和第二个参数传给 mmap() 函数。使用多平面 API 时，
struct v4l2_buffer 包含一个 struct v4l2_plane 结构数组，每个结构都有
自己的 ``m.offset`` 与 ``length``。使用多平面 API 时，每个缓冲区的每个
平面都必须单独映射，因此对 mmap() 的调用次数应当等于缓冲区数量乘以每个
缓冲区中的平面数。offset 与 length 的值不得修改。记住，缓冲区是在物理
内存中分配的，与可以换出到磁盘的虚拟内存相反。应用程序应当尽快用
munmap() 函数释放这些缓冲区。)

Example: Mapping buffers in the single-planar API(示例：在单平面 API 中映射缓冲区)
==================================================================================

.. code-block:: c

    struct v4l2_requestbuffers reqbuf;
    struct {
	void *start;
	size_t length;
    } *buffers;
    unsigned int i;

    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = 20;

    if (-1 == ioctl (fd, VIDIOC_REQBUFS, &reqbuf)) {
	if (errno == EINVAL)
	    printf("Video capturing or mmap-streaming is not supported\\n");
	else
	    perror("VIDIOC_REQBUFS");

	exit(EXIT_FAILURE);
    }

    /* We want at least five buffers. */

    if (reqbuf.count < 5) {
	/* You may need to free the buffers here. */
	printf("Not enough buffer memory\\n");
	exit(EXIT_FAILURE);
    }

    buffers = calloc(reqbuf.count, sizeof(*buffers));
    assert(buffers != NULL);

    for (i = 0; i < reqbuf.count; i++) {
	struct v4l2_buffer buffer;

	memset(&buffer, 0, sizeof(buffer));
	buffer.type = reqbuf.type;
	buffer.memory = V4L2_MEMORY_MMAP;
	buffer.index = i;

	if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buffer)) {
	    perror("VIDIOC_QUERYBUF");
	    exit(EXIT_FAILURE);
	}

	buffers[i].length = buffer.length; /* remember for munmap() */

	buffers[i].start = mmap(NULL, buffer.length,
		    PROT_READ | PROT_WRITE, /* recommended */
		    MAP_SHARED,             /* recommended */
		    fd, buffer.m.offset);

	if (MAP_FAILED == buffers[i].start) {
	    /* If you do not exit here you should unmap() and free()
	       the buffers mapped so far. */
	    perror("mmap");
	    exit(EXIT_FAILURE);
	}
    }

    /* Cleanup. */

    for (i = 0; i < reqbuf.count; i++)
	munmap(buffers[i].start, buffers[i].length);

Example: Mapping buffers in the multi-planar API(示例：在多平面 API 中映射缓冲区)
==================================================================================

.. code-block:: c

    struct v4l2_requestbuffers reqbuf;
    /* Our current format uses 3 planes per buffer */
    #define FMT_NUM_PLANES = 3

    struct {
	void *start[FMT_NUM_PLANES];
	size_t length[FMT_NUM_PLANES];
    } *buffers;
    unsigned int i, j;

    memset(&reqbuf, 0, sizeof(reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = 20;

    if (ioctl(fd, VIDIOC_REQBUFS, &reqbuf) < 0) {
	if (errno == EINVAL)
	    printf("Video capturing or mmap-streaming is not supported\\n");
	else
	    perror("VIDIOC_REQBUFS");

	exit(EXIT_FAILURE);
    }

    /* We want at least five buffers. */

    if (reqbuf.count < 5) {
	/* You may need to free the buffers here. */
	printf("Not enough buffer memory\\n");
	exit(EXIT_FAILURE);
    }

    buffers = calloc(reqbuf.count, sizeof(*buffers));
    assert(buffers != NULL);

    for (i = 0; i < reqbuf.count; i++) {
	struct v4l2_buffer buffer;
	struct v4l2_plane planes[FMT_NUM_PLANES];

	memset(&buffer, 0, sizeof(buffer));
	buffer.type = reqbuf.type;
	buffer.memory = V4L2_MEMORY_MMAP;
	buffer.index = i;
	/* length in struct v4l2_buffer in multi-planar API stores the size
	 * of planes array. */
	buffer.length = FMT_NUM_PLANES;
	buffer.m.planes = planes;

	if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0) {
	    perror("VIDIOC_QUERYBUF");
	    exit(EXIT_FAILURE);
	}

	/* Every plane has to be mapped separately */
	for (j = 0; j < FMT_NUM_PLANES; j++) {
	    buffers[i].length[j] = buffer.m.planes[j].length; /* remember for munmap() */

	    buffers[i].start[j] = mmap(NULL, buffer.m.planes[j].length,
		     PROT_READ | PROT_WRITE, /* recommended */
		     MAP_SHARED,             /* recommended */
		     fd, buffer.m.planes[j].m.mem_offset);

	    if (MAP_FAILED == buffers[i].start[j]) {
		/* If you do not exit here you should unmap() and free()
		   the buffers and planes mapped so far. */
		perror("mmap");
		exit(EXIT_FAILURE);
	    }
	}
    }

    /* Cleanup. */

    for (i = 0; i < reqbuf.count; i++)
	for (j = 0; j < FMT_NUM_PLANES; j++)
	    munmap(buffers[i].start[j], buffers[i].length[j]);

Conceptually streaming drivers maintain two buffer queues, an incoming
and an outgoing queue. They separate the synchronous capture or output
operation locked to a video clock from the application which is subject
to random disk or network delays and preemption by other processes,
thereby reducing the probability of data loss. The queues are organized
as FIFOs, buffers will be output in the order enqueued in the incoming
FIFO, and were captured in the order dequeued from the outgoing FIFO.
(从概念上讲，流式驱动维护两个缓冲区队列：一个入队队列和一个出队队列。
它们把与视频时钟锁定的同步采集或输出操作，与受随机磁盘或网络延迟以及
其他进程抢占影响的应用程序分离开来，从而降低数据丢失的概率。队列按 FIFO
方式组织：缓冲区将按其在入队 FIFO 中入队的顺序被输出，并按其在出队 FIFO
中被出队的顺序被采集。)

The driver may require a minimum number of buffers enqueued at all times
to function, apart of this no limit exists on the number of buffers
applications can enqueue in advance, or dequeue and process. They can
also enqueue in a different order than buffers have been dequeued, and
the driver can *fill* enqueued *empty* buffers in any order.  [#f2]_ The
index number of a buffer (struct :c:type:`v4l2_buffer`
``index``) plays no role here, it only identifies the buffer.
(驱动可能要求始终至少有一定数量的缓冲区处于入队状态才能工作；除此之外，
对应用程序可以预先入队、或出队并处理的缓冲区数量没有限制。应用程序也
可以按照与缓冲区出队时不同的顺序入队，而驱动可以按任意顺序 *填充* 已
入队的 *空* 缓冲区。缓冲区的索引号 (struct v4l2_buffer 的 ``index``)
在这里不起作用，它只用于标识缓冲区。)

Initially all mapped buffers are in dequeued state, inaccessible by the
driver. For capturing applications it is customary to first enqueue all
mapped buffers, then to start capturing and enter the read loop. Here
the application waits until a filled buffer can be dequeued, and
re-enqueues the buffer when the data is no longer needed. Output
applications fill and enqueue buffers, when enough buffers are stacked
up the output is started with :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>`.
In the write loop, when the application runs out of free buffers, it
must wait until an empty buffer can be dequeued and reused.
(最初，所有已映射的缓冲区都处于出队状态，驱动无法访问它们。对采集应用
程序来说，习惯做法是先把所有已映射的缓冲区入队，然后开始采集并进入读取
循环。在这里，应用程序一直等待，直到可以出队一个已填充的缓冲区，并在
数据不再需要时把该缓冲区重新入队。输出应用程序填充缓冲区并使其入队；
当足够多的缓冲区堆积起来时，用 VIDIOC_STREAMON 启动输出。在写循环中，
当应用程序用完了空闲缓冲区时，它必须等待，直到可以出队一个空缓冲区并
重新使用它。)

To enqueue and dequeue a buffer applications use the
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` and :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`
ioctl. The status of a buffer being mapped, enqueued, full or empty can
be determined at any time using the :ref:`VIDIOC_QUERYBUF` ioctl. Two
methods exist to suspend execution of the application until one or more
buffers can be dequeued.  By default :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`
blocks when no buffer is in the outgoing queue. When the ``O_NONBLOCK``
flag was given to the :c:func:`open()` function,
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` returns immediately with an ``EAGAIN``
error code when no buffer is available. The :c:func:`select()`
or :c:func:`poll()` functions are always available.
(要使缓冲区入队和出队，应用程序使用 VIDIOC_QBUF 与 VIDIOC_DQBUF ioctl。
缓冲区被映射、入队、满或空的状态，随时可以用 VIDIOC_QUERYBUF ioctl
确定。有两种方法可以挂起应用程序的执行，直到一个或多个缓冲区可以出队。
默认情况下，当出队队列中没有缓冲区时，VIDIOC_DQBUF 会阻塞。当
``O_NONBLOCK`` 标志被传给 open() 函数时，当没有可用缓冲区时，
VIDIOC_DQBUF 会立即返回 ``EAGAIN`` 错误码。select() 或 poll() 函数始终
可用。)

To start and stop capturing or output applications call the
:ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` and :ref:`VIDIOC_STREAMOFF
<VIDIOC_STREAMON>` ioctl.
(要开始和停止采集或输出，应用程序调用 VIDIOC_STREAMON 与
VIDIOC_STREAMOFF ioctl。)

.. note:::ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>`
   removes all buffers from both queues as a side effect. Since there is
   no notion of doing anything "now" on a multitasking system, if an
   application needs to synchronize with another event it should examine
   the struct ::c:type:`v4l2_buffer` ``timestamp`` of captured
   or outputted buffers.
   (注意：VIDIOC_STREAMOFF 会作为一种副作用，把两个队列中的所有缓冲区
   都移除。由于在多任务系统上没有 "现在" 做某事的概念，如果应用程序需要
   与另一个事件同步，它应当检查被采集或被输出的缓冲区的 struct
   v4l2_buffer 的 ``timestamp``。)

Drivers implementing memory mapping I/O must support the
:ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>`, :ref:`VIDIOC_QUERYBUF
<VIDIOC_QUERYBUF>`, :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`, :ref:`VIDIOC_DQBUF
<VIDIOC_QBUF>`, :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>`
and :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctls, the :ref:`mmap()
<func-mmap>`, :c:func:`munmap()`, :ref:`select()
<func-select>` and :c:func:`poll()` function. [#f3]_
(实现内存映射 I/O 的驱动必须支持 VIDIOC_REQBUFS、VIDIOC_QUERYBUF、
VIDIOC_QBUF、VIDIOC_DQBUF、VIDIOC_STREAMON 与 VIDIOC_STREAMOFF ioctl，
以及 mmap()、munmap()、select() 与 poll() 函数。)

[capture example]

.. [#f1]
   One could use one file descriptor and set the buffer type field
   accordingly when calling :ref:`VIDIOC_QBUF` etc.,
   but it makes the :c:func:`select()` function ambiguous. We also
   like the clean approach of one file descriptor per logical stream.
   Video overlay for example is also a logical stream, although the CPU
   is not needed for continuous operation.
   (可以只使用一个文件描述符，并在调用 VIDIOC_QBUF 等时相应地设置缓冲区
   类型字段，但这会使 select() 函数产生歧义。我们也喜欢每个逻辑流一个
   文件描述符的干净做法。例如，视频叠加 (video overlay) 也是一个逻辑流，
   尽管连续操作并不需要 CPU。)

.. [#f2]
   Random enqueue order permits applications processing images out of
   order (such as video codecs) to return buffers earlier, reducing the
   probability of data loss. Random fill order allows drivers to reuse
   buffers on a LIFO-basis, taking advantage of caches holding
   scatter-gather lists and the like.
   (随机的入队顺序允许乱序处理图像的应用程序 (例如视频编解码器) 更早地
   归还缓冲区，降低数据丢失的概率。随机的填充顺序允许驱动以 LIFO 方式
   重用缓冲区，利用缓存中保存的散布-聚集 (scatter-gather) 列表等。)

.. [#f3]
   At the driver level :c:func:`select()` and :c:func:`poll()` are
   the same, and :c:func:`select()` is too important to be optional.
   The rest should be evident.
   (在驱动层面，select() 与 poll() 是相同的，而 select() 太重要了，不能
   是可选的。其余的应当不言自明。)
