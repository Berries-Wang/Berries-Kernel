.. SPDX-License-Identifier: GPL-2.0

.. _coding_assistants:

+++++++++++++++++++++++++++++++++
AI Coding Assistants(AI 编码助手)
+++++++++++++++++++++++++++++++++

This document provides guidance for AI tools and developers using AI
assistance when contributing to the Linux kernel.
(本文档为向 Linux 内核贡献代码的 AI 工具以及使用 AI 辅助的开发者提供指导。)

AI tools helping with Linux kernel development should follow the standard
kernel development process:
(帮助进行 Linux 内核开发的 AI 工具应遵循标准的内核开发流程：)

* Documentation/process/development-process.rst
* Documentation/process/coding-style.rst
* Documentation/process/submitting-patches.rst

Licensing and Legal Requirements(许可与法律要求)
================================================

All contributions must comply with the kernel's licensing requirements:
(所有贡献必须遵守内核的许可要求：)

* All code must be compatible with GPL-2.0-only
  (所有代码必须与 GPL-2.0-only 兼容。)
* Use appropriate SPDX license identifiers
  (使用恰当的 SPDX 许可标识符。)
* See Documentation/process/license-rules.rst for details
  (详情参见 Documentation/process/license-rules.rst。)

Signed-off-by and Developer Certificate of Origin(Signed-off-by 与开发者来源证明)
=================================================================================

AI agents MUST NOT add Signed-off-by tags. Only humans can legally
certify the Developer Certificate of Origin (DCO). The human submitter
is responsible for:
(AI 代理不得添加 Signed-off-by 标签。只有人类才能在法律上为开发者来源证明
(Developer Certificate of Origin, DCO) 作证。人类提交者负责：)

* Reviewing all AI-generated code
  (审查所有 AI 生成的代码。)
* Ensuring compliance with licensing requirements
  (确保符合许可要求。)
* Adding their own Signed-off-by tag to certify the DCO
  (添加自己的 Signed-off-by 标签以证明 DCO。)
* Taking full responsibility for the contribution
  (对贡献承担全部责任。)

Attribution(署名)
=================

When AI tools contribute to kernel development, proper attribution
helps track the evolving role of AI in the development process.
Contributions should include an Assisted-by tag in the following format::

  Assisted-by: AGENT_NAME:MODEL_VERSION [TOOL1] [TOOL2]

(当 AI 工具参与内核开发时，恰当的署名有助于跟踪 AI 在开发过程中不断演变
的角色。贡献应包含一个如下格式的 Assisted-by 标签：)

Where:
(其中：)

* ``AGENT_NAME`` is the name of the AI tool or framework
  (``AGENT_NAME`` 是 AI 工具或框架的名称。)
* ``MODEL_VERSION`` is the specific model version used
  (``MODEL_VERSION`` 是所使用的具体模型版本。)
* ``[TOOL1] [TOOL2]`` are optional specialized analysis tools used
  (e.g., coccinelle, sparse, smatch, clang-tidy)
  (``[TOOL1] [TOOL2]`` 是可选的专用分析工具 (例如 coccinelle、sparse、
  smatch、clang-tidy)。)

Basic development tools (git, gcc, make, editors) should not be listed.
(基础开发工具 (git、gcc、make、编辑器) 不应列出。)

Example::

  Assisted-by: Claude:claude-3-opus coccinelle sparse

(示例：)
