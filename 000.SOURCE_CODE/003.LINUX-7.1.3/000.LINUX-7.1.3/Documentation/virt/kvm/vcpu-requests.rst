.. SPDX-License-Identifier: GPL-2.0

================================
KVM VCPU Requests(KVM VCPU 请求)
================================

Overview(概述)
==============

KVM supports an internal API enabling threads to request a VCPU thread to
perform some activity.  For example, a thread may request a VCPU to flush
its TLB with a VCPU request.  The API consists of the following functions::

  /* Check if any requests are pending for VCPU @vcpu. */
  bool kvm_request_pending(struct kvm_vcpu *vcpu);

  /* Check if VCPU @vcpu has request @req pending. */
  bool kvm_test_request(int req, struct kvm_vcpu *vcpu);

  /* Clear request @req for VCPU @vcpu. */
  void kvm_clear_request(int req, struct kvm_vcpu *vcpu);

  /*
   * Check if VCPU @vcpu has request @req pending. When the request is
   * pending it will be cleared and a memory barrier, which pairs with
   * another in kvm_make_request(), will be issued.
   */
  bool kvm_check_request(int req, struct kvm_vcpu *vcpu);

  /*
   * Make request @req of VCPU @vcpu. Issues a memory barrier, which pairs
   * with another in kvm_check_request(), prior to setting the request.
   */
  void kvm_make_request(int req, struct kvm_vcpu *vcpu);

  /* Make request @req of all VCPUs of the VM with struct kvm @kvm. */
  bool kvm_make_all_cpus_request(struct kvm *kvm, unsigned int req);

(KVM 支持一套内部 API，使线程能够请求 VCPU 线程执行某些活动。例如，线程
可以通过一个 VCPU 请求要求 VCPU 刷新其 TLB。该 API 由以下函数组成：)

Typically a requester wants the VCPU to perform the activity as soon
as possible after making the request.  This means most requests
(kvm_make_request() calls) are followed by a call to kvm_vcpu_kick(),
and kvm_make_all_cpus_request() has the kicking of all VCPUs built
into it.
(通常，请求方希望 VCPU 在提出请求后尽快执行该活动。这意味着大多数请求
(kvm_make_request() 调用) 之后都会跟随一次 kvm_vcpu_kick() 调用，而
kvm_make_all_cpus_request() 则内置了对所有 VCPU 的踢 (kick) 操作。)

VCPU Kicks(VCPU 踢 (kick) 操作)
-------------------------------

The goal of a VCPU kick is to bring a VCPU thread out of guest mode in
order to perform some KVM maintenance.  To do so, an IPI is sent, forcing
a guest mode exit.  However, a VCPU thread may not be in guest mode at the
time of the kick.  Therefore, depending on the mode and state of the VCPU
thread, there are two other actions a kick may take.  All three actions
are listed below:
(VCPU kick 的目标是让 VCPU 线程退出客户机模式 (guest mode)，以便执行某些
KVM 维护工作。为此会发送一个 IPI，强制退出客户机模式。然而，kick 发生的
时刻 VCPU 线程可能并不处于客户机模式。因此，根据 VCPU 线程的模式和状态，
kick 还可能采取另外两种动作。这三种动作如下所列：)

1) Send an IPI.  This forces a guest mode exit.
   (发送一个 IPI。这会强制退出客户机模式。)
2) Waking a sleeping VCPU.  Sleeping VCPUs are VCPU threads outside guest
   mode that wait on waitqueues.  Waking them removes the threads from
   the waitqueues, allowing the threads to run again.  This behavior
   may be suppressed, see KVM_REQUEST_NO_WAKEUP below.
   (唤醒休眠的 VCPU。休眠的 VCPU 是指处于客户机模式之外、在等待队列
   (waitqueue) 上等待的 VCPU 线程。唤醒它们会把线程从等待队列中移出，使
   线程能够再次运行。该行为可以被抑制，参见下文的 KVM_REQUEST_NO_WAKEUP。)
3) Nothing.  When the VCPU is not in guest mode and the VCPU thread is not
   sleeping, then there is nothing to do.
   (什么都不做。当 VCPU 不在客户机模式且 VCPU 线程也未休眠时，就没有任何
   事情要做。)

VCPU Mode(VCPU 模式)
--------------------

VCPUs have a mode state, ``vcpu->mode``, that is used to track whether the
guest is running in guest mode or not, as well as some specific
outside guest mode states.  The architecture may use ``vcpu->mode`` to
ensure VCPU requests are seen by VCPUs (see "Ensuring Requests Are Seen"),
as well as to avoid sending unnecessary IPIs (see "IPI Reduction"), and
even to ensure IPI acknowledgements are waited upon (see "Waiting for
Acknowledgements").  The following modes are defined:
(VCPU 有一个模式状态 ``vcpu->mode``，用于跟踪客户机是否运行在客户机模式，
以及某些特定的客户机模式之外的状态。架构可以利用 ``vcpu->mode`` 来确保
VCPU 请求能被 VCPU 看到 (参见下文 "Ensuring Requests Are Seen" 一节)，也
可以用来避免发送不必要的 IPI (参见 "IPI Reduction" 一节)，甚至用来确保
IPI 应答被等待 (参见 "Waiting for Acknowledgements" 一节)。定义了以下
几种模式：)

OUTSIDE_GUEST_MODE

  The VCPU thread is outside guest mode.
  (VCPU 线程处于客户机模式之外。)

IN_GUEST_MODE

  The VCPU thread is in guest mode.
  (VCPU 线程处于客户机模式。)

EXITING_GUEST_MODE

  The VCPU thread is transitioning from IN_GUEST_MODE to
  OUTSIDE_GUEST_MODE.
  (VCPU 线程正在从 IN_GUEST_MODE 转换到 OUTSIDE_GUEST_MODE。)

READING_SHADOW_PAGE_TABLES

  The VCPU thread is outside guest mode, but it wants the sender of
  certain VCPU requests, namely KVM_REQ_TLB_FLUSH, to wait until the VCPU
  thread is done reading the page tables.
  (VCPU 线程处于客户机模式之外，但它希望某些 VCPU 请求 (即
  KVM_REQ_TLB_FLUSH) 的发送方等待 VCPU 线程读完页表。)

VCPU Request Internals(VCPU 请求内部机制)
=========================================

VCPU requests are simply bit indices of the ``vcpu->requests`` bitmap.
This means general bitops, like those documented in [atomic-ops]_ could
also be used, e.g. ::

  clear_bit(KVM_REQ_UNBLOCK & KVM_REQUEST_MASK, &vcpu->requests);

(VCPU 请求只是 ``vcpu->requests`` 位图中的位索引。这意味着也可以使用通用
的位操作，例如：)

However, VCPU request users should refrain from doing so, as it would
break the abstraction.  The first 8 bits are reserved for architecture
independent requests; all additional bits are available for architecture
dependent requests.
(不过，VCPU 请求的使用者应避免这样做，因为这会破坏抽象。前 8 位保留给
架构无关请求；其余所有位供架构相关请求使用。)

Architecture Independent Requests(架构无关请求)
-----------------------------------------------

KVM_REQ_TLB_FLUSH

  KVM's common MMU notifier may need to flush all of a guest's TLB
  entries, calling kvm_flush_remote_tlbs() to do so.  Architectures that
  choose to use the common kvm_flush_remote_tlbs() implementation will
  need to handle this VCPU request.
  (KVM 的通用 MMU 通知器可能需要刷新客户机的全部 TLB 表项，为此会调用
  kvm_flush_remote_tlbs()。选择使用通用 kvm_flush_remote_tlbs() 实现的
  架构需要处理这个 VCPU 请求。)

KVM_REQ_VM_DEAD

  This request informs all VCPUs that the VM is dead and unusable, e.g. due to
  fatal error or because the VM's state has been intentionally destroyed.
  (该请求告知所有 VCPU：VM 已经死亡且不可用，例如由于致命错误，或因为
  VM 的状态已被有意销毁。)

KVM_REQ_UNBLOCK

  This request informs the vCPU to exit kvm_vcpu_block.  It is used for
  example from timer handlers that run on the host on behalf of a vCPU,
  or in order to update the interrupt routing and ensure that assigned
  devices will wake up the vCPU.
  (该请求告知 vCPU 退出 kvm_vcpu_block。例如，代表 vCPU 在宿主机上运行的
  定时器处理函数会使用它，或用于更新中断路由并确保已分配的设备能够唤醒
  vCPU。)

KVM_REQ_OUTSIDE_GUEST_MODE

  This "request" ensures the target vCPU has exited guest mode prior to the
  sender of the request continuing on.  No action needs be taken by the target,
  and so no request is actually logged for the target.  This request is similar
  to a "kick", but unlike a kick it guarantees the vCPU has actually exited
  guest mode.  A kick only guarantees the vCPU will exit at some point in the
  future, e.g. a previous kick may have started the process, but there's no
  guarantee the to-be-kicked vCPU has fully exited guest mode.
  (这个 "请求" 确保目标 vCPU 在请求发送方继续执行之前已经退出客户机模式。
  目标无需采取任何动作，因此实际上不会为目标记录任何请求。该请求类似于
  "kick"，但与 kick 不同的是，它保证 vCPU 已经真正退出客户机模式。kick
  只保证 vCPU 会在将来某个时刻退出，例如前一个 kick 可能已经启动了这个
  过程，但无法保证被 kick 的 vCPU 已完全退出客户机模式。)

KVM_REQUEST_MASK(原名)
-----------------------

VCPU requests should be masked by KVM_REQUEST_MASK before using them with
bitops.  This is because only the lower 8 bits are used to represent the
request's number.  The upper bits are used as flags.  Currently only two
flags are defined.
(VCPU 请求在与位操作一起使用之前，应当先用 KVM_REQUEST_MASK 做掩码。
这是因为只有低 8 位用于表示请求编号，高位用作标志。目前只定义了两个
标志。)

VCPU Request Flags(VCPU 请求标志)
---------------------------------

KVM_REQUEST_NO_WAKEUP

  This flag is applied to requests that only need immediate attention
  from VCPUs running in guest mode.  That is, sleeping VCPUs do not need
  to be awakened for these requests.  Sleeping VCPUs will handle the
  requests when they are awakened later for some other reason.
  (该标志应用于只需要处于客户机模式运行的 VCPU 立即关注的请求。也就是
  说，对这些请求无需唤醒休眠的 VCPU。休眠的 VCPU 稍后因其他原因被唤醒
  时会处理这些请求。)

KVM_REQUEST_WAIT

  When requests with this flag are made with kvm_make_all_cpus_request(),
  then the caller will wait for each VCPU to acknowledge its IPI before
  proceeding.  This flag only applies to VCPUs that would receive IPIs.
  If, for example, the VCPU is sleeping, so no IPI is necessary, then
  the requesting thread does not wait.  This means that this flag may be
  safely combined with KVM_REQUEST_NO_WAKEUP.  See "Waiting for
  Acknowledgements" for more information about requests with
  KVM_REQUEST_WAIT.
  (当带有该标志的请求通过 kvm_make_all_cpus_request() 发出时，调用者在
  继续执行之前会等待每个 VCPU 对其 IPI 的应答。该标志只适用于会接收
  IPI 的 VCPU。例如，如果 VCPU 正在休眠，因而不需要 IPI，那么请求线程
  就不等待。这意味着该标志可以与 KVM_REQUEST_NO_WAKEUP 安全地组合使用。
  关于带有 KVM_REQUEST_WAIT 的请求的更多信息，参见 "Waiting for
  Acknowledgements" 一节。)

VCPU Requests with Associated State(带关联状态的 VCPU 请求)
===========================================================

Requesters that want the receiving VCPU to handle new state need to ensure
the newly written state is observable to the receiving VCPU thread's CPU
by the time it observes the request.  This means a write memory barrier
must be inserted after writing the new state and before setting the VCPU
request bit.  Additionally, on the receiving VCPU thread's side, a
corresponding read barrier must be inserted after reading the request bit
and before proceeding to read the new state associated with it.  See
scenario 3, Message and Flag, of [lwn-mb]_ and the kernel documentation
[memory-barriers]_.
(希望接收方 VCPU 处理新状态的请求方，需要确保在接收方 VCPU 线程的 CPU
观察到该请求时，新写入的状态对其是可观察的。这意味着必须在写入新状态
之后、设置 VCPU 请求位之前插入一个写内存屏障。此外，在接收方 VCPU 线程
一侧，必须在读取请求位之后、开始读取与之关联的新状态之前插入相应的读
屏障。参见相关文章的场景 3 (Message and Flag) 以及内核内存屏障文档。)

The pair of functions, kvm_check_request() and kvm_make_request(), provide
the memory barriers, allowing this requirement to be handled internally by
the API.
(函数对 kvm_check_request() 与 kvm_make_request() 提供了这些内存屏障，
使这一要求可以在 API 内部得到处理。)

Ensuring Requests Are Seen(确保请求被看到)
==========================================

When making requests to VCPUs, we want to avoid the receiving VCPU
executing in guest mode for an arbitrary long time without handling the
request.  We can be sure this won't happen as long as we ensure the VCPU
thread checks kvm_request_pending() before entering guest mode and that a
kick will send an IPI to force an exit from guest mode when necessary.
Extra care must be taken to cover the period after the VCPU thread's last
kvm_request_pending() check and before it has entered guest mode, as kick
IPIs will only trigger guest mode exits for VCPU threads that are in guest
mode or at least have already disabled interrupts in order to prepare to
enter guest mode.  This means that an optimized implementation (see "IPI
Reduction") must be certain when it's safe to not send the IPI.  One
solution, which all architectures except s390 apply, is to:
(当向 VCPU 发出请求时，我们要避免接收方 VCPU 在客户机模式下不加限定地
长时间运行而不处理请求。只要我们确保 VCPU 线程在进入客户机模式之前检查
kvm_request_pending()，并且 kick 会在必要时发送 IPI 强制退出客户机模式，
就可以确信这不会发生。需要特别小心地覆盖 VCPU 线程最后一次
kvm_request_pending() 检查之后、进入客户机模式之前的这段时间，因为 kick
IPI 只会对处于客户机模式、或至少已经为进入客户机模式做准备而关中断的
VCPU 线程触发客户机模式退出。这意味着一个优化的实现 (参见 "IPI
Reduction" 一节) 必须确定何时不发送 IPI 是安全的。除 s390 外的所有架构
都采用的一个解决方案是：)

- set ``vcpu->mode`` to IN_GUEST_MODE between disabling the interrupts and
  the last kvm_request_pending() check;
  (在关中断与最后一次 kvm_request_pending() 检查之间把 ``vcpu->mode``
  设置为 IN_GUEST_MODE；)
- enable interrupts atomically when entering the guest.
  (进入客户机时以原子方式开中断。)

This solution also requires memory barriers to be placed carefully in both
the requesting thread and the receiving VCPU.  With the memory barriers we
can exclude the possibility of a VCPU thread observing
!kvm_request_pending() on its last check and then not receiving an IPI for
the next request made of it, even if the request is made immediately after
the check.  This is done by way of the Dekker memory barrier pattern
(scenario 10 of [lwn-mb]_).  As the Dekker pattern requires two variables,
this solution pairs ``vcpu->mode`` with ``vcpu->requests``.  Substituting
them into the pattern gives::

  CPU1                                    CPU2
  =================                       =================
  local_irq_disable();
  WRITE_ONCE(vcpu->mode, IN_GUEST_MODE);  kvm_make_request(REQ, vcpu);
  smp_mb();                               smp_mb();
  if (kvm_request_pending(vcpu)) {        if (READ_ONCE(vcpu->mode) ==
                                              IN_GUEST_MODE) {
      ...abort guest entry...                 ...send IPI...
  }                                       }

(这个解决方案还要求在请求线程与接收 VCPU 两侧都仔细放置内存屏障。借助
内存屏障，我们可以排除这样的可能性：VCPU 线程在最后一次检查时观察到
!kvm_request_pending()，随后却没有收到针对下一个请求的 IPI，即使该请求
在检查之后立即发出。这通过 Dekker 内存屏障模式 (场景 10) 实现。由于
Dekker 模式需要两个变量，该方案将 ``vcpu->mode`` 与 ``vcpu->requests``
配对。把它们代入该模式即得：)

As stated above, the IPI is only useful for VCPU threads in guest mode or
that have already disabled interrupts.  This is why this specific case of
the Dekker pattern has been extended to disable interrupts before setting
``vcpu->mode`` to IN_GUEST_MODE.  WRITE_ONCE() and READ_ONCE() are used to
pedantically implement the memory barrier pattern, guaranteeing the
compiler doesn't interfere with ``vcpu->mode``'s carefully planned
accesses.
(如上所述，IPI 只对处于客户机模式、或已经关中断的 VCPU 线程有用。这就是
为什么 Dekker 模式的这个具体用例被扩展为在把 ``vcpu->mode`` 设置为
IN_GUEST_MODE 之前先关中断。WRITE_ONCE() 与 READ_ONCE() 被用来严格地实现
该内存屏障模式，保证编译器不会干扰 ``vcpu->mode`` 经过精心规划的访问。)

IPI Reduction(IPI 减少)
-----------------------

As only one IPI is needed to get a VCPU to check for any/all requests,
then they may be coalesced.  This is easily done by having the first IPI
sending kick also change the VCPU mode to something !IN_GUEST_MODE.  The
transitional state, EXITING_GUEST_MODE, is used for this purpose.
(既然只需要一个 IPI 就能让 VCPU 检查任意/全部请求，那么这些请求就可以
合并。做法很简单：让第一个发送 IPI 的 kick 同时把 VCPU 模式改为某个
!IN_GUEST_MODE 的值。过渡状态 EXITING_GUEST_MODE 就是用于此目的的。)

Waiting for Acknowledgements(等待确认)
--------------------------------------

Some requests, those with the KVM_REQUEST_WAIT flag set, require IPIs to
be sent, and the acknowledgements to be waited upon, even when the target
VCPU threads are in modes other than IN_GUEST_MODE.  For example, one case
is when a target VCPU thread is in READING_SHADOW_PAGE_TABLES mode, which
is set after disabling interrupts.  To support these cases, the
KVM_REQUEST_WAIT flag changes the condition for sending an IPI from
checking that the VCPU is IN_GUEST_MODE to checking that it is not
OUTSIDE_GUEST_MODE.
(某些请求，即设置了 KVM_REQUEST_WAIT 标志的请求，即使目标 VCPU 线程处于
IN_GUEST_MODE 以外的模式，也需要发送 IPI 并等待其应答。例如，当目标 VCPU
线程处于 READING_SHADOW_PAGE_TABLES 模式 (在关中断之后设置) 时就是这样
一种情况。为支持这些情况，KVM_REQUEST_WAIT 标志把发送 IPI 的条件从检查
VCPU 是否处于 IN_GUEST_MODE 改为检查它是否不处于 OUTSIDE_GUEST_MODE。)

Request-less VCPU Kicks(无请求的 VCPU kick)
-------------------------------------------

As the determination of whether or not to send an IPI depends on the
two-variable Dekker memory barrier pattern, then it's clear that
request-less VCPU kicks are almost never correct.  Without the assurance
that a non-IPI generating kick will still result in an action by the
receiving VCPU, as the final kvm_request_pending() check does for
request-accompanying kicks, then the kick may not do anything useful at
all.  If, for instance, a request-less kick was made to a VCPU that was
just about to set its mode to IN_GUEST_MODE, meaning no IPI is sent, then
the VCPU thread may continue its entry without actually having done
whatever it was the kick was meant to initiate.
(由于是否发送 IPI 的判定依赖于双变量的 Dekker 内存屏障模式，因此很显然，
无请求的 VCPU kick 几乎从来都是不正确的。对于伴随请求的 kick，最后一次
kvm_request_pending() 检查保证了即使 kick 没有产生 IPI，接收方 VCPU 仍会
执行某个动作；缺少这种保证时，kick 可能根本做不了任何有用的事情。例如，
如果对一个即将把其模式设置为 IN_GUEST_MODE 的 VCPU (意味着不会发送 IPI)
发出无请求 kick，那么 VCPU 线程可能继续进入客户机，而实际上并没有做 kick
本意要引发的事情。)

One exception is x86's posted interrupt mechanism.  In this case, however,
even the request-less VCPU kick is coupled with the same
local_irq_disable() + smp_mb() pattern described above; the ON bit
(Outstanding Notification) in the posted interrupt descriptor takes the
role of ``vcpu->requests``.  When sending a posted interrupt, PIR.ON is
set before reading ``vcpu->mode``; dually, in the VCPU thread,
vmx_sync_pir_to_irr() reads PIR after setting ``vcpu->mode`` to
IN_GUEST_MODE.
(一个例外是 x86 的 posted interrupt 机制。不过在这种情况下，即使是无请求
的 VCPU kick 也与上文描述的相同 local_irq_disable() + smp_mb() 模式耦合；
posted interrupt descriptor 中的 ON 位 (Outstanding Notification) 扮演了
``vcpu->requests`` 的角色。发送 posted interrupt 时，PIR.ON 在读取
``vcpu->mode`` 之前设置；对偶地，在 VCPU 线程中，vmx_sync_pir_to_irr()
在把 ``vcpu->mode`` 设置为 IN_GUEST_MODE 之后读取 PIR。)

Additional Considerations(其他注意事项)
=======================================

Sleeping VCPUs(休眠的 VCPU)
---------------------------

VCPU threads may need to consider requests before and/or after calling
functions that may put them to sleep, e.g. kvm_vcpu_block().  Whether they
do or not, and, if they do, which requests need consideration, is
architecture dependent.  kvm_vcpu_block() calls kvm_arch_vcpu_runnable()
to check if it should awaken.  One reason to do so is to provide
architectures a function where requests may be checked if necessary.
(VCPU 线程在调用可能使其休眠的函数 (例如 kvm_vcpu_block()) 之前和/或之后，
可能需要考虑请求。是否需要考虑，以及如果需要则考虑哪些请求，取决于架构。
kvm_vcpu_block() 会调用 kvm_arch_vcpu_runnable() 来检查自己是否应当被
唤醒。这样做的一个原因是为架构提供一个可在必要时检查请求的函数。)

References(参考资料)
====================

.. [atomic-ops] Documentation/atomic_bitops.txt and Documentation/atomic_t.txt
.. [memory-barriers] Documentation/memory-barriers.txt
.. [lwn-mb] https://lwn.net/Articles/573436/
