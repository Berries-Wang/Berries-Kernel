==================================
VDUSE - "vDPA Device in Userspace"
==================================

vDPA (virtio data path acceleration) device is a device that uses a
datapath which complies with the virtio specifications with vendor
specific control path. vDPA devices can be both physically located on
the hardware or emulated by software. VDUSE is a framework that makes it
possible to implement software-emulated vDPA devices in userspace. And
to make the device emulation more secure, the emulated vDPA device's
control path is handled in the kernel and only the data path is
implemented in the userspace.
(vDPA (virtio 数据路径加速) 设备是一种使用符合 virtio 规范的数据路径、
并带有厂商特定控制路径的设备。vDPA 设备既可以物理地位于硬件上，也可以
由软件模拟。VDUSE 是一个框架，它使得在用户空间实现软件模拟的 vDPA 设备
成为可能。为了让设备模拟更安全，被模拟的 vDPA 设备的控制路径在内核中
处理，只有数据路径在用户空间实现。)

Note that only virtio block device is supported by VDUSE framework now,
which can reduce security risks when the userspace process that implements
the data path is run by an unprivileged user. The support for other device
types can be added after the security issue of corresponding device driver
is clarified or fixed in the future.
(注意，VDUSE 框架现在只支持 virtio 块设备，当实现数据路径的用户空间进程
由非特权用户运行时，这可以降低安全风险。对其他设备类型的支持，可以在
将来相应设备驱动的安全问题被澄清或修复之后再添加。)

Create/Destroy VDUSE devices(创建/销毁 VDUSE 设备)
--------------------------------------------------

VDUSE devices are created as follows:
(VDUSE 设备按如下方式创建：)

1. Create a new VDUSE instance with ioctl(VDUSE_CREATE_DEV) on
   /dev/vduse/control.
   (在 /dev/vduse/control 上用 ioctl(VDUSE_CREATE_DEV) 创建一个新的
   VDUSE 实例。)

2. Setup each virtqueue with ioctl(VDUSE_VQ_SETUP) on /dev/vduse/$NAME.
   (在 /dev/vduse/$NAME 上用 ioctl(VDUSE_VQ_SETUP) 设置每个 virtqueue。)

3. Begin processing VDUSE messages from /dev/vduse/$NAME. The first
   messages will arrive while attaching the VDUSE instance to vDPA bus.
   (开始处理来自 /dev/vduse/$NAME 的 VDUSE 消息。在把 VDUSE 实例附加到
   vDPA 总线时，最初的消息就会到达。)

4. Send the VDPA_CMD_DEV_NEW netlink message to attach the VDUSE
   instance to vDPA bus.
   (发送 VDPA_CMD_DEV_NEW netlink 消息，把 VDUSE 实例附加到 vDPA 总线。)

VDUSE devices are destroyed as follows:
(VDUSE 设备按如下方式销毁：)

1. Send the VDPA_CMD_DEV_DEL netlink message to detach the VDUSE
   instance from vDPA bus.
   (发送 VDPA_CMD_DEV_DEL netlink 消息，把 VDUSE 实例从 vDPA 总线分离。)

2. Close the file descriptor referring to /dev/vduse/$NAME.
   (关闭引用 /dev/vduse/$NAME 的文件描述符。)

3. Destroy the VDUSE instance with ioctl(VDUSE_DESTROY_DEV) on
   /dev/vduse/control.
   (在 /dev/vduse/control 上用 ioctl(VDUSE_DESTROY_DEV) 销毁该 VDUSE
   实例。)

The netlink messages can be sent via vdpa tool in iproute2 or use the
below sample codes:
(这些 netlink 消息可以通过 iproute2 中的 vdpa 工具发送，或者使用下面的
示例代码：)

.. code-block:: c

	static int netlink_add_vduse(const char *name, enum vdpa_command cmd)
	{
		struct nl_sock *nlsock;
		struct nl_msg *msg;
		int famid;

		nlsock = nl_socket_alloc();
		if (!nlsock)
			return -ENOMEM;

		if (genl_connect(nlsock))
			goto free_sock;

		famid = genl_ctrl_resolve(nlsock, VDPA_GENL_NAME);
		if (famid < 0)
			goto close_sock;

		msg = nlmsg_alloc();
		if (!msg)
			goto close_sock;

		if (!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, famid, 0, 0, cmd, 0))
			goto nla_put_failure;

		NLA_PUT_STRING(msg, VDPA_ATTR_DEV_NAME, name);
		if (cmd == VDPA_CMD_DEV_NEW)
			NLA_PUT_STRING(msg, VDPA_ATTR_MGMTDEV_DEV_NAME, "vduse");

		if (nl_send_sync(nlsock, msg))
			goto close_sock;

		nl_close(nlsock);
		nl_socket_free(nlsock);

		return 0;
	nla_put_failure:
		nlmsg_free(msg);
	close_sock:
		nl_close(nlsock);
	free_sock:
		nl_socket_free(nlsock);
		return -1;
	}

How VDUSE works(VDUSE 的工作原理)
---------------------------------

As mentioned above, a VDUSE device is created by ioctl(VDUSE_CREATE_DEV) on
/dev/vduse/control. With this ioctl, userspace can specify some basic configuration
such as device name (uniquely identify a VDUSE device), virtio features, virtio
configuration space, the number of virtqueues and so on for this emulated device.
Then a char device interface (/dev/vduse/$NAME) is exported to userspace for device
emulation. Userspace can use the VDUSE_VQ_SETUP ioctl on /dev/vduse/$NAME to
add per-virtqueue configuration such as the max size of virtqueue to the device.
(如上所述，VDUSE 设备通过在 /dev/vduse/control 上执行
ioctl(VDUSE_CREATE_DEV) 来创建。通过该 ioctl，用户空间可以为这个模拟设备
指定一些基本配置，例如设备名 (唯一标识一个 VDUSE 设备)、virtio 特性、
virtio 配置空间、virtqueue 的数量等等。然后，一个字符设备接口
(/dev/vduse/$NAME) 被导出给用户空间用于设备模拟。用户空间可以在
/dev/vduse/$NAME 上使用 VDUSE_VQ_SETUP ioctl，向设备添加每个 virtqueue 的
配置，例如 virtqueue 的最大尺寸。)

After the initialization, the VDUSE device can be attached to vDPA bus via
the VDPA_CMD_DEV_NEW netlink message. Userspace needs to read()/write() on
/dev/vduse/$NAME to receive/reply some control messages from/to VDUSE kernel
module as follows:
(初始化之后，可以通过 VDPA_CMD_DEV_NEW netlink 消息把 VDUSE 设备附加到
vDPA 总线。用户空间需要在 /dev/vduse/$NAME 上调用 read()/write()，以从
VDUSE 内核模块接收一些控制消息、并向其回复，如下所示：)

.. code-block:: c

	static int vduse_message_handler(int dev_fd)
	{
		int len;
		struct vduse_dev_request req;
		struct vduse_dev_response resp;

		len = read(dev_fd, &req, sizeof(req));
		if (len != sizeof(req))
			return -1;

		resp.request_id = req.request_id;

		switch (req.type) {

		/* handle different types of messages */

		}

		len = write(dev_fd, &resp, sizeof(resp));
		if (len != sizeof(resp))
			return -1;

		return 0;
	}

There are now three types of messages introduced by VDUSE framework:
(VDUSE 框架现在引入了三种类型的消息：)

- VDUSE_GET_VQ_STATE: Get the state for virtqueue, userspace should return
  avail index for split virtqueue or the device/driver ring wrap counters and
  the avail and used index for packed virtqueue.
  (VDUSE_GET_VQ_STATE：获取 virtqueue 的状态，用户空间应当返回拆分式
  virtqueue 的 avail 索引，或打包式 virtqueue 的设备/驱动环绕计数器以及
  avail 与 used 索引。)

- VDUSE_SET_STATUS: Set the device status, userspace should follow
  the virtio spec: https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.html
  to process this message. For example, fail to set the FEATURES_OK device
  status bit if the device can not accept the negotiated virtio features
  get from the VDUSE_DEV_GET_FEATURES ioctl.
  (VDUSE_SET_STATUS：设置设备状态，用户空间应当遵循 virtio 规范
  (https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.html) 来
  处理该消息。例如，如果设备不能接受通过 VDUSE_DEV_GET_FEATURES ioctl
  获取的协商好的 virtio 特性，就应当不设置 FEATURES_OK 设备状态位。)

- VDUSE_UPDATE_IOTLB: Notify userspace to update the memory mapping for specified
  IOVA range, userspace should firstly remove the old mapping, then setup the new
  mapping via the VDUSE_IOTLB_GET_FD ioctl.
  (VDUSE_UPDATE_IOTLB：通知用户空间更新指定 IOVA 范围的内存映射，用户
  空间应当先移除旧映射，然后通过 VDUSE_IOTLB_GET_FD ioctl 建立新映射。)

After DRIVER_OK status bit is set via the VDUSE_SET_STATUS message, userspace is
able to start the dataplane processing as follows:
(在通过 VDUSE_SET_STATUS 消息设置 DRIVER_OK 状态位之后，用户空间就可以
开始按如下方式处理数据路径：)

1. Get the specified virtqueue's information with the VDUSE_VQ_GET_INFO ioctl,
   including the size, the IOVAs of descriptor table, available ring and used ring,
   the state and the ready status.
   (用 VDUSE_VQ_GET_INFO ioctl 获取指定 virtqueue 的信息，包括大小、
   描述符表、available ring 与 used ring 的 IOVA、状态以及就绪状态。)

2. Pass the above IOVAs to the VDUSE_IOTLB_GET_FD ioctl so that those IOVA regions
   can be mapped into userspace. Some sample codes is shown below:
   (把上述 IOVA 传递给 VDUSE_IOTLB_GET_FD ioctl，以便把这些 IOVA 区域
   映射到用户空间。部分示例代码如下所示：)

.. code-block:: c

	static int perm_to_prot(uint8_t perm)
	{
		int prot = 0;

		switch (perm) {
		case VDUSE_ACCESS_WO:
			prot |= PROT_WRITE;
			break;
		case VDUSE_ACCESS_RO:
			prot |= PROT_READ;
			break;
		case VDUSE_ACCESS_RW:
			prot |= PROT_READ | PROT_WRITE;
			break;
		}

		return prot;
	}

	static void *iova_to_va(int dev_fd, uint64_t iova, uint64_t *len)
	{
		int fd;
		void *addr;
		size_t size;
		struct vduse_iotlb_entry entry;

		entry.start = iova;
		entry.last = iova;

		/*
		 * Find the first IOVA region that overlaps with the specified
		 * range [start, last] and return the corresponding file descriptor.
		 */
		fd = ioctl(dev_fd, VDUSE_IOTLB_GET_FD, &entry);
		if (fd < 0)
			return NULL;

		size = entry.last - entry.start + 1;
		*len = entry.last - iova + 1;
		addr = mmap(0, size, perm_to_prot(entry.perm), MAP_SHARED,
			    fd, entry.offset);
		close(fd);
		if (addr == MAP_FAILED)
			return NULL;

		/*
		 * Using some data structures such as linked list to store
		 * the iotlb mapping. The munmap(2) should be called for the
		 * cached mapping when the corresponding VDUSE_UPDATE_IOTLB
		 * message is received or the device is reset.
		 */

		return addr + iova - entry.start;
	}

3. Setup the kick eventfd for the specified virtqueues with the VDUSE_VQ_SETUP_KICKFD
   ioctl. The kick eventfd is used by VDUSE kernel module to notify userspace to
   consume the available ring. This is optional since userspace can choose to poll the
   available ring instead.
   (用 VDUSE_VQ_SETUP_KICKFD ioctl 为指定的 virtqueue 设置 kick eventfd。
   kick eventfd 由 VDUSE 内核模块用来通知用户空间消费 available ring。
   这是可选的，因为用户空间可以选择改为轮询 available ring。)

4. Listen to the kick eventfd (optional) and consume the available ring. The buffer
   described by the descriptors in the descriptor table should be also mapped into
   userspace via the VDUSE_IOTLB_GET_FD ioctl before accessing.
   (监听 kick eventfd (可选) 并消费 available ring。在访问之前，描述符表中
   描述符所描述的缓冲区也应当通过 VDUSE_IOTLB_GET_FD ioctl 映射到用户
   空间。)

5. Inject an interrupt for specific virtqueue with the VDUSE_INJECT_VQ_IRQ ioctl
   after the used ring is filled.
   (在 used ring 被填满之后，用 VDUSE_INJECT_VQ_IRQ ioctl 为特定的
   virtqueue 注入一个中断。)

Enabling ASID (API version 1)(启用 ASID (API 版本 1))
-----------------------------------------------------

VDUSE supports per-address-space identifiers (ASIDs) starting with API
version 1. Set it up with ioctl(VDUSE_SET_API_VERSION) on `/dev/vduse/control`
and pass `VDUSE_API_VERSION_1` before creating a new VDUSE instance with
ioctl(VDUSE_CREATE_DEV).
(VDUSE 从 API 版本 1 开始支持每地址空间标识符 (ASID)。在用
ioctl(VDUSE_CREATE_DEV) 创建新的 VDUSE 实例之前，先在
`/dev/vduse/control` 上用 ioctl(VDUSE_SET_API_VERSION) 进行设置，并传入
`VDUSE_API_VERSION_1`。)

Afterwards, you can use the member asid of ioctl(VDUSE_VQ_SETUP) argument to
select the address space of the IOTLB you are querying.  The driver could
change the address space of any virtqueue group by using the
VDUSE_SET_VQ_GROUP_ASID VDUSE message type, and the VDUSE instance needs to
reply with VDUSE_REQ_RESULT_OK if it was possible to change it.
(之后，你可以使用 ioctl(VDUSE_VQ_SETUP) 参数中的 asid 成员来选择你要
查询的 IOTLB 的地址空间。驱动可以通过 VDUSE_SET_VQ_GROUP_ASID VDUSE
消息类型改变任何 virtqueue 组的地址空间，如果可以更改，VDUSE 实例需要
用 VDUSE_REQ_RESULT_OK 回复。)

Similarly, you can use ioctl(VDUSE_IOTLB_GET_FD2) to obtain the file descriptor
describing an IOVA region of a specific ASID. Example usage:
(类似地，你可以使用 ioctl(VDUSE_IOTLB_GET_FD2) 来获取描述特定 ASID 的
某个 IOVA 区域的文件描述符。用法示例：)

.. code-block:: c

	static void *iova_to_va(int dev_fd, uint32_t asid, uint64_t iova,
	                        uint64_t *len)
	{
		int fd;
		void *addr;
		size_t size;
		struct vduse_iotlb_entry_v2 entry = { 0 };

		entry.v1.start = iova;
		entry.v1.last = iova;
		entry.asid = asid;

		fd = ioctl(dev_fd, VDUSE_IOTLB_GET_FD2, &entry);
		if (fd < 0)
			return NULL;

		size = entry.v1.last - entry.v1.start + 1;
		*len = entry.v1.last - iova + 1;
		addr = mmap(0, size, perm_to_prot(entry.v1.perm), MAP_SHARED,
			    fd, entry.v1.offset);
		close(fd);
		if (addr == MAP_FAILED)
			return NULL;

		/*
		 * Using some data structures such as linked list to store
		 * the iotlb mapping. The munmap(2) should be called for the
		 * cached mapping when the corresponding VDUSE_UPDATE_IOTLB
		 * message is received or the device is reset.
		 */

		return addr + iova - entry.v1.start;
	}

For more details on the uAPI, please see include/uapi/linux/vduse.h.
(关于 uAPI 的更多细节，请参见 include/uapi/linux/vduse.h。)
