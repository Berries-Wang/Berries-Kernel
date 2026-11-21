.. raw:: latex

	\renewcommand\thesection*
	\renewcommand\thesubsection*

.. _process_index:

=================================================================
Working with the kernel development community(与内核开发社区共事)
=================================================================

So you want to be a Linux kernel developer?  Welcome!  While there is a lot
to be learned about the kernel in a technical sense, it is also important
to learn about how our community works.  Reading these documents will make
it much easier for you to get your changes merged with a minimum of
trouble.
(想成为 Linux 内核开发者吗？欢迎！虽然在技术层面关于内核有很多东西要学，但了解我们社区如何运作也很重要。阅读这些文档将使你的更改更容易在最少麻烦的情况下被合并。)

-----------------------------------------------------------------
An introduction to how kernel development works(内核开发方式简介)
-----------------------------------------------------------------

Read these documents first: an understanding of the material here will ease
your entry into the kernel community.
(请先阅读这些文档：理解这里的内容会让你更容易进入内核社区。)

.. toctree::
   :maxdepth: 1

   howto
   development-process
   submitting-patches
   submit-checklist

Tools and technical guides for kernel developers(面向内核开发者的工具与技术指南)
--------------------------------------------------------------------------------

This is a collection of material that kernel developers should be familiar
with.
(这是一份内核开发者应当熟悉的资料集。)

.. toctree::
   :maxdepth: 1

   changes
   programming-language
   coding-style
   maintainer-pgp-guide
   email-clients
   applying-patches
   backporting
   adding-syscalls
   volatile-considered-harmful
   botching-up-ioctls

Policy guides and developer statements(政策指南与开发者声明)
------------------------------------------------------------

These are the rules that we try to live by in the kernel community (and
beyond).
(这些是我们在内核社区（及更广范围）中努力遵守的规则。)

.. toctree::
   :maxdepth: 1

   license-rules
   code-of-conduct
   code-of-conduct-interpretation
   contribution-maturity-model
   kernel-enforcement-statement
   kernel-driver-statement
   stable-api-nonsense
   stable-kernel-rules
   management-style
   researcher-guidelines
   generated-content
   coding-assistants
   conclave

Dealing with bugs(处理缺陷)
---------------------------

Bugs are a fact of life; it is important that we handle them properly. The
documents below provide general advice about debugging and describe our
policies around the handling of a couple of special classes of bugs:
regressions and security problems.
(缺陷是生活的一部分；妥善处理它们很重要。下面的文档提供关于调试的一般性建议，并描述我们围绕处理两类特殊缺陷（回归和安全问题）的政策。)

.. toctree::
   :maxdepth: 1

   debugging/index
   handling-regressions
   security-bugs
   threat-model
   cve
   embargoed-hardware-issues

Maintainer information(维护者信息)
----------------------------------

How to find the people who will accept your patches.
(如何找到会接受你的补丁的人。)

.. toctree::
   :maxdepth: 1

   maintainer-handbooks
   maintainers

Other material(其他资料)
------------------------

Here are some other guides to the community that are of interest to most
developers:
(以下是一些大多数开发者都感兴趣的社区其他指南：)

.. toctree::
   :maxdepth: 1

   kernel-docs
   deprecated
