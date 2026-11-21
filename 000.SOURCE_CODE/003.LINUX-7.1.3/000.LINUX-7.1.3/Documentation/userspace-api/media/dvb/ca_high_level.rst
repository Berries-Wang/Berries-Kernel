.. SPDX-License-Identifier: GPL-2.0

The High level CI API(高级 CI API)
==================================

.. note::

   This documentation is outdated.
   (本文档已过时。)

This document describes the high level CI API as in accordance to the
Linux DVB API.
(本文档描述符合 Linux DVB API 的高级 CI API。)


With the High Level CI approach any new card with almost any random
architecture can be implemented with this style, the definitions
inside the switch statement can be easily adapted for any card, thereby
eliminating the need for any additional ioctls.
(采用高级 CI（High Level CI）方式，几乎任何具有任意架构的新卡都可以用这种
风格实现，switch 语句内的定义可以轻松适配任何卡，从而消除了对任何额外
ioctl 的需求。)

The disadvantage is that the driver/hardware has to manage the rest. For
the application programmer it would be as simple as sending/receiving an
array to/from the CI ioctls as defined in the Linux DVB API. No changes
have been made in the API to accommodate this feature.
(缺点是其余工作必须由驱动/硬件来管理。对应用程序员来说，这就像按照 Linux
DVB API 中的定义向 CI ioctl 发送/从其接收一个数组那样简单。API 没有为容纳
这一特性做任何更改。)


Why the need for another CI interface?(为什么还需要另一个 CI 接口？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is one of the most commonly asked question. Well a nice question.
Strictly speaking this is not a new interface.
(这是最常被问到的问题之一。嗯，这个问题问得好。严格来说，这并不是一个
新接口。)

The CI interface is defined in the DVB API in ca.h as:
(CI 接口在 DVB API 的 ca.h 中定义如下：)

.. code-block:: c

	typedef struct ca_slot_info {
		int num;               /* slot number */

		int type;              /* CA interface this slot supports */
	#define CA_CI            1     /* CI high level interface */
	#define CA_CI_LINK       2     /* CI link layer level interface */
	#define CA_CI_PHYS       4     /* CI physical layer level interface */
	#define CA_DESCR         8     /* built-in descrambler */
	#define CA_SC          128     /* simple smart card interface */

		unsigned int flags;
	#define CA_CI_MODULE_PRESENT 1 /* module (or card) inserted */
	#define CA_CI_MODULE_READY   2
	} ca_slot_info_t;

This CI interface follows the CI high level interface, which is not
implemented by most applications. Hence this area is revisited.
(此 CI 接口遵循 CI 高级接口，但大多数应用程序并未实现该接口。因此这一领域
被重新审视。)

This CI interface is quite different in the case that it tries to
accommodate all other CI based devices, that fall into the other categories.
(此 CI 接口的独特之处在于，它试图兼容属于其他类别的所有其他基于 CI 的
设备。)

This means that this CI interface handles the EN50221 style tags in the
Application layer only and no session management is taken care of by the
application. The driver/hardware will take care of all that.
(这意味着此 CI 接口只在应用层处理 EN50221 风格的标签，会话管理不需要应用
程序操心，驱动/硬件会负责处理这一切。)

This interface is purely an EN50221 interface exchanging APDU's. This
means that no session management, link layer or a transport layer do
exist in this case in the application to driver communication. It is
as simple as that. The driver/hardware has to take care of that.
(此接口是纯粹用于交换 APDU 的 EN50221 接口。这意味着在这种应用与驱动的
通信中，不存在会话管理、链路层或传输层。就这么简单。这些都必须由驱动/
硬件来负责。)

With this High Level CI interface, the interface can be defined with the
regular ioctls.
(使用这种高级 CI 接口，接口可以用常规的 ioctl 来定义。)

All these ioctls are also valid for the High level CI interface
(所有这些 ioctl 对高级 CI 接口同样有效)

#define CA_RESET          _IO('o', 128)
#define CA_GET_CAP        _IOR('o', 129, ca_caps_t)
#define CA_GET_SLOT_INFO  _IOR('o', 130, ca_slot_info_t)
#define CA_GET_DESCR_INFO _IOR('o', 131, ca_descr_info_t)
#define CA_GET_MSG        _IOR('o', 132, ca_msg_t)
#define CA_SEND_MSG       _IOW('o', 133, ca_msg_t)
#define CA_SET_DESCR      _IOW('o', 134, ca_descr_t)


On querying the device, the device yields information thus:
(查询设备时，设备给出如下信息：)

.. code-block:: none

	CA_GET_SLOT_INFO
	----------------------------
	Command = [info]
	APP: Number=[1]
	APP: Type=[1]
	APP: flags=[1]
	APP: CI High level interface
	APP: CA/CI Module Present

	CA_GET_CAP
	----------------------------
	Command = [caps]
	APP: Slots=[1]
	APP: Type=[1]
	APP: Descrambler keys=[16]
	APP: Type=[1]

	CA_SEND_MSG
	----------------------------
	Descriptors(Program Level)=[ 09 06 06 04 05 50 ff f1]
	Found CA descriptor @ program level

	(20) ES type=[2] ES pid=[201]  ES length =[0 (0x0)]
	(25) ES type=[4] ES pid=[301]  ES length =[0 (0x0)]
	ca_message length is 25 (0x19) bytes
	EN50221 CA MSG=[ 9f 80 32 19 03 01 2d d1 f0 08 01 09 06 06 04 05 50 ff f1 02 e0 c9 00 00 04 e1 2d 00 00]


Not all ioctl's are implemented in the driver from the API, the other
features of the hardware that cannot be implemented by the API are achieved
using the CA_GET_MSG and CA_SEND_MSG ioctls. An EN50221 style wrapper is
used to exchange the data to maintain compatibility with other hardware.
(并非 API 中的所有 ioctl 都在驱动中实现，硬件中无法由 API 实现的其他功能
通过 CA_GET_MSG 和 CA_SEND_MSG ioctl 实现。使用 EN50221 风格的封装来交换
数据，以保持与其他硬件的兼容性。)

.. code-block:: c

	/* a message to/from a CI-CAM */
	typedef struct ca_msg {
		unsigned int index;
		unsigned int type;
		unsigned int length;
		unsigned char msg[256];
	} ca_msg_t;


The flow of data can be described thus,
(数据的流向可以描述如下，)

.. code-block:: none

	App (User)
	-----
	parse
	  |
	  |
	  v
	en50221 APDU (package)
   --------------------------------------
   |	  |				| High Level CI driver
   |	  |				|
   |	  v				|
   |	en50221 APDU (unpackage)	|
   |	  |				|
   |	  |				|
   |	  v				|
   |	sanity checks			|
   |	  |				|
   |	  |				|
   |	  v				|
   |	do (H/W dep)			|
   --------------------------------------
	  |    Hardware
	  |
	  v

The High Level CI interface uses the EN50221 DVB standard, following a
standard ensures futureproofness.
(高级 CI 接口使用 EN50221 DVB 标准，遵循标准可以确保面向未来。)
