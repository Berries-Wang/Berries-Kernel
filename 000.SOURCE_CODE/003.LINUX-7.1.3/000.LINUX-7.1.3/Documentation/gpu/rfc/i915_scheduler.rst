=======================================================================
I915 GuC Submission/DRM Scheduler Section(I915 GuC 提交/DRM 调度器章节)
=======================================================================

Upstream plan(上游计划)
=======================
For upstream the overall plan for landing GuC submission and integrating the
i915 with the DRM scheduler is:
(关于上游落地 GuC 提交并将 i915 集成到 DRM 调度器的总体计划如下：)

* Merge basic GuC submission
  (合并基础 GuC 提交)
	* Basic submission support for all gen11+ platforms
	  (为所有 gen11+ 平台提供基础提交支持)
	* Not enabled by default on any current platforms but can be enabled via
	  modparam enable_guc
	  (在任何当前平台上默认均未启用，但可以通过模块参数 enable_guc
	  启用)
	* Lots of rework will need to be done to integrate with DRM scheduler so
	  no need to nit pick everything in the code, it just should be
	  functional, no major coding style / layering errors, and not regress
	  execlists
	  (与 DRM 调度器集成还需要大量返工，因此无需对代码吹毛求疵，只要
	  功能正常、没有重大的编码风格/分层错误、且不使 execlists 退化
	  即可)
	* Update IGTs / selftests as needed to work with GuC submission
	  (根据需要更新 IGT/自测试以配合 GuC 提交工作)
	* Enable CI on supported platforms for a baseline
	  (在受支持的平台上启用 CI 以获得基线)
	* Rework / get CI heathly for GuC submission in place as needed
	  (根据需要为 GuC 提交进行返工/使 CI 保持健康)
* Merge new parallel submission uAPI
  (合并新的并行提交 uAPI)
	* Bonding uAPI completely incompatible with GuC submission, plus it has
	  severe design issues in general, which is why we want to retire it no
	  matter what
	  (Bonding uAPI 与 GuC 提交完全不兼容，而且总体上存在严重的设计
	  问题，这正是我们无论如何都要将其退役的原因)
	* New uAPI adds I915_CONTEXT_ENGINES_EXT_PARALLEL context setup step
	  which configures a slot with N contexts
	  (新 uAPI 增加 I915_CONTEXT_ENGINES_EXT_PARALLEL 上下文设置步骤，
	  用于将一个 slot 配置为 N 个上下文)
	* After I915_CONTEXT_ENGINES_EXT_PARALLEL a user can submit N batches to
	  a slot in a single execbuf IOCTL and the batches run on the GPU in
	  parallel
	  (在 I915_CONTEXT_ENGINES_EXT_PARALLEL 之后，用户可以在单个
	  execbuf IOCTL 中向一个 slot 提交 N 个批处理，这些批处理在 GPU 上
	  并行运行)
	* Initially only for GuC submission but execlists can be supported if
	  needed
	  (起初仅支持 GuC 提交，但如有需要也可以支持 execlists)
* Convert the i915 to use the DRM scheduler
  (将 i915 转换为使用 DRM 调度器)
	* GuC submission backend fully integrated with DRM scheduler
	  (GuC 提交后端与 DRM 调度器完全集成)
		* All request queues removed from backend (e.g. all backpressure
		  handled in DRM scheduler)
		  (所有请求队列都从后端移除（例如所有背压都在 DRM 调度器中
		  处理）)
		* Resets / cancels hook in DRM scheduler
		  (复位/取消在 DRM 调度器中挂钩)
		* Watchdog hooks into DRM scheduler
		  (看门狗挂钩到 DRM 调度器)
		* Lots of complexity of the GuC backend can be pulled out once
		  integrated with DRM scheduler (e.g. state machine gets
		  simpler, locking gets simpler, etc...)
		  (与 DRM 调度器集成后，GuC 后端的许多复杂性都可以去除
		  （例如状态机变得更简单、加锁变得更简单等）)
	* Execlists backend will minimum required to hook in the DRM scheduler
	  (execlists 后端只做挂接 DRM 调度器所需的最小工作)
		* Legacy interface
		  (遗留接口)
		* Features like timeslicing / preemption / virtual engines would
		  be difficult to integrate with the DRM scheduler and these
		  features are not required for GuC submission as the GuC does
		  these things for us
		  (时间片/抢占/虚拟引擎之类的特性很难与 DRM 调度器集成，而
		  GuC 提交不需要这些特性，因为 GuC 会替我们完成这些工作)
		* ROI low on fully integrating into DRM scheduler
		  (完全集成到 DRM 调度器的投入产出比很低)
		* Fully integrating would add lots of complexity to DRM
		  scheduler
		  (完全集成会给 DRM 调度器增加大量复杂性)
	* Port i915 priority inheritance / boosting feature in DRM scheduler
	  (把 i915 的优先级继承/提升特性移植到 DRM 调度器)
		* Used for i915 page flip, may be useful to other DRM drivers as
		  well
		  (用于 i915 页面翻转，可能对其他 DRM 驱动也有用)
		* Will be an optional feature in the DRM scheduler
		  (将是 DRM 调度器中的一个可选特性)
	* Remove in-order completion assumptions from DRM scheduler
	  (从 DRM 调度器中移除按序完成假设)
		* Even when using the DRM scheduler the backends will handle
		  preemption, timeslicing, etc... so it is possible for jobs to
		  finish out of order
		  (即使使用 DRM 调度器，后端仍会处理抢占、时间片等，因此
		  任务有可能乱序完成)
	* Pull out i915 priority levels and use DRM priority levels
  (抽出 i915 优先级级别，改用 DRM 优先级级别)
	* Optimize DRM scheduler as needed
  (根据需要优化 DRM 调度器)

TODOs for GuC submission upstream(GuC 提交上游的 TODO)
======================================================

* Need an update to GuC firmware / i915 to enable error state capture
  (需要更新 GuC 固件/i915 以启用错误状态捕获)
* Open source tool to decode GuC logs
  (用于解码 GuC 日志的开源工具)
* Public GuC spec
  (公开的 GuC 规范)

New uAPI for basic GuC submission(基础 GuC 提交的新 uAPI)
=========================================================
No major changes are required to the uAPI for basic GuC submission. The only
change is a new scheduler attribute: I915_SCHEDULER_CAP_STATIC_PRIORITY_MAP.
This attribute indicates the 2k i915 user priority levels are statically mapped
into 3 levels as follows:
(基础 GuC 提交不需要对 uAPI 做重大更改。唯一的更改是新增一个调度器属性：
I915_SCHEDULER_CAP_STATIC_PRIORITY_MAP。该属性表明 2k 个 i915 用户优先级
级别被静态映射为如下 3 个级别：)

* -1k to -1 Low priority
  (-1k 到 -1：低优先级)
* 0 Medium priority
  (0：中优先级)
* 1 to 1k High priority
  (1 到 1k：高优先级)

This is needed because the GuC only has 4 priority bands. The highest priority
band is reserved with the kernel. This aligns with the DRM scheduler priority
levels too.
(之所以需要这样，是因为 GuC 只有 4 个优先级段。最高优先级段保留给内核。
这也与 DRM 调度器的优先级级别保持一致。)

Spec references:(规范参考:)
---------------------------
* https://www.khronos.org/registry/EGL/extensions/IMG/EGL_IMG_context_priority.txt
* https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap5.html#devsandqueues-priority
* https://spec.oneapi.com/level-zero/latest/core/api.html#ze-command-queue-priority-t

New parallel submission uAPI(新的并行提交 uAPI)
===============================================
The existing bonding uAPI is completely broken with GuC submission because
whether a submission is a single context submit or parallel submit isn't known
until execbuf time activated via the I915_SUBMIT_FENCE. To submit multiple
contexts in parallel with the GuC the context must be explicitly registered with
N contexts and all N contexts must be submitted in a single command to the GuC.
The GuC interfaces do not support dynamically changing between N contexts as the
bonding uAPI does. Hence the need for a new parallel submission interface. Also
the legacy bonding uAPI is quite confusing and not intuitive at all. Furthermore
I915_SUBMIT_FENCE is by design a future fence, so not really something we should
continue to support.
(现有的 bonding uAPI 在 GuC 提交下完全失效，因为一次提交是单上下文提交还是
并行提交，要到 execbuf 时刻通过 I915_SUBMIT_FENCE 激活时才能确定。要用 GuC
并行提交多个上下文，必须把上下文显式注册为 N 个上下文，并且所有 N 个上下文
必须在单条命令中提交给 GuC。GuC 接口不支持像 bonding uAPI 那样在 N 个上下文
之间动态切换。因此需要一个新的并行提交接口。此外，遗留的 bonding uAPI 相当
令人困惑，一点也不直观。再者，I915_SUBMIT_FENCE 在设计上就是一个 future
fence，因此确实不是我们应该继续支持的东西。)

The new parallel submission uAPI consists of 3 parts:
(新的并行提交 uAPI 由 3 部分组成：)

* Export engines logical mapping
  (导出引擎逻辑映射)
* A 'set_parallel' extension to configure contexts for parallel
  submission
  (用于配置并行提交上下文的 'set_parallel' 扩展)
* Extend execbuf2 IOCTL to support submitting N BBs in a single IOCTL
  (扩展 execbuf2 IOCTL，以支持在单个 IOCTL 中提交 N 个 BB)

Export engines logical mapping(导出引擎逻辑映射)
------------------------------------------------
Certain use cases require BBs to be placed on engine instances in logical order
(e.g. split-frame on gen11+). The logical mapping of engine instances can change
based on fusing. Rather than making UMDs be aware of fusing, simply expose the
logical mapping with the existing query engine info IOCTL. Also the GuC
submission interface currently only supports submitting multiple contexts to
engines in logical order which is a new requirement compared to execlists.
Lastly, all current platforms have at most 2 engine instances and the logical
order is the same as uAPI order. This will change on platforms with more than 2
engine instances.
(某些用例要求 BB 按逻辑顺序放置到引擎实例上（例如 gen11+ 上的
split-frame）。引擎实例的逻辑映射可能因熔丝 (fusing) 而变化。与其让 UMD
感知熔丝，不如直接通过现有的查询引擎信息 IOCTL 暴露逻辑映射。此外，GuC
提交接口目前只支持按逻辑顺序向引擎提交多个上下文，这是与 execlists 相比的
一个新要求。最后，所有当前平台最多只有 2 个引擎实例，逻辑顺序与 uAPI 顺序
相同。在拥有多于 2 个引擎实例的平台上，这一点将会改变。)

A single bit will be added to drm_i915_engine_info.flags indicating that the
logical instance has been returned and a new field,
drm_i915_engine_info.logical_instance, returns the logical instance.
(将在 drm_i915_engine_info.flags 中增加一个位，表明已返回逻辑实例；并新增
一个字段 drm_i915_engine_info.logical_instance，返回逻辑实例。)

A 'set_parallel' extension to configure contexts for parallel submission(用于配置并行提交上下文的 'set_parallel' 扩展)
----------------------------------------------------------------------------------------------------------------------
The 'set_parallel' extension configures a slot for parallel submission of N BBs.
It is a setup step that must be called before using any of the contexts. See
I915_CONTEXT_ENGINES_EXT_LOAD_BALANCE or I915_CONTEXT_ENGINES_EXT_BOND for
similar existing examples. Once a slot is configured for parallel submission the
execbuf2 IOCTL can be called submitting N BBs in a single IOCTL. Initially only
supports GuC submission. Execlists supports can be added later if needed.
('set_parallel' 扩展将一个 slot 配置为并行提交 N 个 BB。这是一个设置步骤，
必须在使用任何上下文之前调用。类似扩展现有示例参见
I915_CONTEXT_ENGINES_EXT_LOAD_BALANCE 或 I915_CONTEXT_ENGINES_EXT_BOND。
一旦把 slot 配置为并行提交，就可以调用 execbuf2 IOCTL，在单个 IOCTL 中提交
N 个 BB。最初只支持 GuC 提交。如有需要，以后可以添加 execlists 支持。)

Add I915_CONTEXT_ENGINES_EXT_PARALLEL_SUBMIT and
drm_i915_context_engines_parallel_submit to the uAPI to implement this
extension.
(在 uAPI 中增加 I915_CONTEXT_ENGINES_EXT_PARALLEL_SUBMIT 和
drm_i915_context_engines_parallel_submit 来实现此扩展。)

.. c:namespace-push:: rfc

.. kernel-doc:: include/uapi/drm/i915_drm.h
        :functions: i915_context_engines_parallel_submit

.. c:namespace-pop::

Extend execbuf2 IOCTL to support submitting N BBs in a single IOCTL(扩展 execbuf2 IOCTL 以支持在单个 IOCTL 中提交 N 个 BB)
---------------------------------------------------------------------------------------------------------------------------
Contexts that have been configured with the 'set_parallel' extension can only
submit N BBs in a single execbuf2 IOCTL. The BBs are either the last N objects
in the drm_i915_gem_exec_object2 list or the first N if I915_EXEC_BATCH_FIRST is
set. The number of BBs is implicit based on the slot submitted and how it has
been configured by 'set_parallel' or other extensions. No uAPI changes are
required to the execbuf2 IOCTL.
(经 'set_parallel' 扩展配置的上下文只能在单个 execbuf2 IOCTL 中提交 N 个
BB。这些 BB 要么是 drm_i915_gem_exec_object2 列表中的最后 N 个对象，要么
在设置了 I915_EXEC_BATCH_FIRST 时是前 N 个对象。BB 的数量由所提交的 slot
及其被 'set_parallel' 或其他扩展配置的方式隐式决定。execbuf2 IOCTL 无需
任何 uAPI 更改。)
