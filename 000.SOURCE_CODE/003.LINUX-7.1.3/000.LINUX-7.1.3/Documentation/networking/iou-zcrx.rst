.. SPDX-License-Identifier: GPL-2.0

==========================================
io_uring zero copy Rx(io_uring 零拷贝接收)
==========================================

Introduction(简介)
==================

io_uring zero copy Rx (ZC Rx) is a feature that removes kernel-to-user copy on
the network receive path, allowing packet data to be received directly into
userspace memory. This feature is different to TCP_ZEROCOPY_RECEIVE in that
there are no strict alignment requirements and no need to mmap()/munmap().
Compared to kernel bypass solutions such as e.g. DPDK, the packet headers are
processed by the kernel TCP stack as normal.
(io_uring 零拷贝接收 (ZC Rx) 是一项消除网络接收路径上内核到用户拷贝的特性，使数据包数据能够直接接收到用户空间内存中。此特性与 TCP_ZEROCOPY_RECEIVE 不同：它没有严格的对齐要求，也不需要 mmap()/munmap()。与 DPDK 之类的内核旁路方案相比，数据包报头仍由内核 TCP 协议栈照常处理。)

NIC HW Requirements(网卡硬件要求)
=================================

Several NIC HW features are required for io_uring ZC Rx to work. For now the
kernel API does not configure the NIC and it must be done by the user.
(io_uring ZC Rx 正常工作需要若干网卡硬件特性。目前内核 API 不会配置网卡，必须由用户自行完成。)

Header/data split(报头/数据分离)
--------------------------------

Required to split packets at the L4 boundary into a header and a payload.
Headers are received into kernel memory as normal and processed by the TCP
stack as normal. Payloads are received into userspace memory directly.
(需要该功能在 L4 边界把数据包拆分为报头与载荷。报头像往常一样接收到内核内存中，并由 TCP 协议栈照常处理。载荷则直接接收到用户空间内存中。)

Flow steering(流导向)
---------------------

Specific HW Rx queues are configured for this feature, but modern NICs
typically distribute flows across all HW Rx queues. Flow steering is required
to ensure that only desired flows are directed towards HW queues that are
configured for io_uring ZC Rx.
(该特性使用特定的硬件接收队列，但现代网卡通常会把流分散到所有硬件接收队列上。需要流导向 (flow steering) 来确保只有期望的流被导向配置用于 io_uring ZC Rx 的硬件队列。)

RSS(RSS)
--------

In addition to flow steering above, RSS is required to steer all other non-zero
copy flows away from queues that are configured for io_uring ZC Rx.
(除上述流导向之外，还需要 RSS 把所有其他非零拷贝流从配置用于 io_uring ZC Rx 的队列上引开。)

Usage(用法)
===========

Setup NIC(设置网卡)
-------------------

Must be done out of band for now.
(目前必须在带外完成。)

Ensure there are at least two queues::
(确保至少有两个队列：)

  ethtool -L eth0 combined 2

Enable header/data split::
(启用报头/数据分离：)

  ethtool -G eth0 tcp-data-split on

Carve out half of the HW Rx queues for zero copy using RSS::
(使用 RSS 划出一半硬件接收队列用于零拷贝：)

  ethtool -X eth0 equal 1

Set up flow steering, bearing in mind that queues are 0-indexed::
(设置流导向，注意队列编号从 0 开始：)

  ethtool -N eth0 flow-type tcp6 ... action 1

Setup io_uring(设置 io_uring)
-----------------------------

This section describes the low level io_uring kernel API. Please refer to
liburing documentation for how to use the higher level API.
(本节描述低层的 io_uring 内核 API。关于如何使用更高层的 API，请参阅 liburing 文档。)

Create an io_uring instance with the following required setup flags::
(使用以下必需的设置标志创建 io_uring 实例：)

  IORING_SETUP_SINGLE_ISSUER
  IORING_SETUP_DEFER_TASKRUN
  IORING_SETUP_CQE32 or IORING_SETUP_CQE_MIXED

Create memory area(创建内存区域)
--------------------------------

Allocate userspace memory area for receiving zero copy data::
(为接收零拷贝数据分配用户空间内存区域：)

  void *area_ptr = mmap(NULL, area_size,
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        0, 0);

Create refill ring(创建补充环形缓冲)
------------------------------------

Allocate memory for a shared ringbuf used for returning consumed buffers::
(为用于归还已消费缓冲区的共享环形缓冲区分配内存：)

  void *ring_ptr = mmap(NULL, ring_size,
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        0, 0);

This refill ring consists of some space for the header, followed by an array of
``struct io_uring_zcrx_rqe``::
(此补充环形缓冲由一段头部空间和紧随其后的 ``struct io_uring_zcrx_rqe`` 数组组成：)

  size_t rq_entries = 4096;
  size_t ring_size = rq_entries * sizeof(struct io_uring_zcrx_rqe) + PAGE_SIZE;
  /* align to page size */
  ring_size = (ring_size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

Register ZC Rx(注册 ZC Rx)
--------------------------

Fill in registration structs::
(填写注册结构体：)

  struct io_uring_zcrx_area_reg area_reg = {
    .addr = (__u64)(unsigned long)area_ptr,
    .len = area_size,
    .flags = 0,
  };

  struct io_uring_region_desc region_reg = {
    .user_addr = (__u64)(unsigned long)ring_ptr,
    .size = ring_size,
    .flags = IORING_MEM_REGION_TYPE_USER,
  };

  struct io_uring_zcrx_ifq_reg reg = {
    .if_idx = if_nametoindex("eth0"),
    /* this is the HW queue with desired flow steered into it */
    .if_rxq = 1,
    .rq_entries = rq_entries,
    .area_ptr = (__u64)(unsigned long)&area_reg,
    .region_ptr = (__u64)(unsigned long)&region_reg,
  };

Register with kernel::
(向内核注册：)

  io_uring_register_ifq(ring, &reg);

Map refill ring(映射补充环形缓冲)
---------------------------------

The kernel fills in fields for the refill ring in the registration ``struct
io_uring_zcrx_ifq_reg``. Map it into userspace::
(内核会在注册结构体 ``struct
io_uring_zcrx_ifq_reg`` 中填写补充环形缓冲的字段。把它映射到用户空间：)

  struct io_uring_zcrx_rq refill_ring;

  refill_ring.khead = (unsigned *)((char *)ring_ptr + reg.offsets.head);
  refill_ring.khead = (unsigned *)((char *)ring_ptr + reg.offsets.tail);
  refill_ring.rqes =
    (struct io_uring_zcrx_rqe *)((char *)ring_ptr + reg.offsets.rqes);
  refill_ring.rq_tail = 0;
  refill_ring.ring_ptr = ring_ptr;

Receiving data(接收数据)
------------------------

Prepare a zero copy recv request::
(准备一个零拷贝接收请求：)

  struct io_uring_sqe *sqe;

  sqe = io_uring_get_sqe(ring);
  io_uring_prep_rw(IORING_OP_RECV_ZC, sqe, fd, NULL, 0, 0);
  sqe->ioprio |= IORING_RECV_MULTISHOT;

Now, submit and wait::
(现在，提交并等待：)

  io_uring_submit_and_wait(ring, 1);

Finally, process completions::
(最后，处理完成事件：)

  struct io_uring_cqe *cqe;
  unsigned int count = 0;
  unsigned int head;

  io_uring_for_each_cqe(ring, head, cqe) {
    struct io_uring_zcrx_cqe *rcqe = (struct io_uring_zcrx_cqe *)(cqe + 1);

    unsigned long mask = (1ULL << IORING_ZCRX_AREA_SHIFT) - 1;
    unsigned char *data = area_ptr + (rcqe->off & mask);
    /* do something with the data */

    count++;
  }
  io_uring_cq_advance(ring, count);

Recycling buffers(回收缓冲区)
-----------------------------

Return buffers back to the kernel to be used again::
(把缓冲区交还内核以便再次使用：)

  struct io_uring_zcrx_rqe *rqe;
  unsigned mask = refill_ring.ring_entries - 1;
  rqe = &refill_ring.rqes[refill_ring.rq_tail & mask];

  unsigned long area_offset = rcqe->off & ~IORING_ZCRX_AREA_MASK;
  rqe->off = area_offset | area_reg.rq_area_token;
  rqe->len = cqe->res;
  IO_URING_WRITE_ONCE(*refill_ring.ktail, ++refill_ring.rq_tail);

Area chunking(区域分块)
-----------------------

zcrx splits the memory area into fixed-length physically contiguous chunks.
This limits the maximum buffer size returned in a single io_uring CQE. Users
can provide a hint to the kernel to use larger chunks by setting the
``rx_buf_len`` field of ``struct io_uring_zcrx_ifq_reg`` to the desired length
during registration. If this field is set to zero, the kernel defaults to
the system page size.
(zcrx 把内存区域划分为固定长度的物理连续块。这限制了单个 io_uring CQE 中返回的最大缓冲区大小。用户可以在注册时把 ``struct io_uring_zcrx_ifq_reg`` 的 ``rx_buf_len`` 字段设置为期望的长度，以此向内核提示使用更大的块。如果此字段设为零，内核默认使用系统页大小。)

To use larger sizes, the memory area must be backed by physically contiguous
ranges whose sizes are multiples of ``rx_buf_len``. It also requires kernel
and hardware support. If registration fails, users are generally expected to
fall back to defaults by setting ``rx_buf_len`` to zero.
(要使用更大的尺寸，内存区域必须由大小为 ``rx_buf_len`` 整数倍的物理连续区间支撑。这还需要内核与硬件的支持。如果注册失败，用户通常应把 ``rx_buf_len`` 设为零，回退到默认值。)

Larger chunks don't give any additional guarantees about buffer sizes returned
in CQEs, and they can vary depending on many factors like traffic pattern,
hardware offload, etc. It doesn't require any application changes beyond zcrx
registration.
(更大的块对 CQE 中返回的缓冲区大小不提供任何额外保证，实际大小可能因流量模式、硬件卸载等许多因素而变化。除 zcrx 注册之外，它不需要应用程序做任何改动。)

Testing(测试)
=============

See ``tools/testing/selftests/drivers/net/hw/iou-zcrx.c``
(参见 tools/testing/selftests/drivers/net/hw/iou-zcrx.c。)
