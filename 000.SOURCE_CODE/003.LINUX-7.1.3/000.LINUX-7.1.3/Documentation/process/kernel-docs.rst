.. _kernel_docs:

Index of Further Kernel Documentation(其他内核文档索引)
========================================================

The need for a document like this one became apparent in the linux-kernel
mailing list as the same questions, asking for pointers to information,
appeared again and again.
(当有人在 linux-kernel 邮件列表上一遍又一遍地问"哪里能找到某某资料"这类相同的问题时，大家意识到需要一份这样的文档。)

Fortunately, as more and more people get to GNU/Linux, more and more get
interested in the Kernel. But reading the sources is not always enough. It
is easy to understand the code, but miss the concepts, the philosophy and
design decisions behind this code.
(幸运的是，随着越来越多的人接触 GNU/Linux，对内核感兴趣的人也越来越多。但只读源码并不总是够的。人们很容易看懂代码，却错过代码背后的概念、哲学与设计决策。)

Unfortunately, not many documents are available for beginners to start.
And, even if they exist, there was no "well-known" place which kept track
of them. These lines try to cover this lack.
(遗憾的是，可供初学者入门的文档并不多。而且即使存在，也没有一个"众所周知的"地方对它们做追踪登记。本文件试图弥补这一缺憾。)

PLEASE, if you know any paper not listed here or write a new document,
include a reference to it here, following the kernel's patch submission
process. Any corrections, ideas or comments are also welcome.
(拜托：如果你知道这里没有列出的任何文献，或者写了新文档，请按内核补丁提交流程在这里添加对它的引用。任何更正、想法或评论也都欢迎。)

All documents are cataloged with the following fields: the document's
"Title", the "Author"/s, the "URL" where they can be found, some "Keywords"
helpful when searching for specific topics, and a brief "Description" of
the Document.
(所有文档都按以下字段编目：文档的 "Title"(标题)、"Author"(作者)、可找到它的 "URL"、"Keywords"(关键词，便于检索特定主题)，以及对文档的简要 "Description"(描述)。)

.. note::

   The documents on each section of this document are ordered by its
   published date, from the newest to the oldest. The maintainer(s) should
   periodically retire resources as they become obsolete or outdated; with
   the exception of foundational books.
   (注意：本文档每一节内的文档按发布日期排序，从新到旧。维护者应定期淘汰已过时或陈旧的资源；基础性书籍除外。)

Docs at the Linux Kernel tree(Linux 内核树中的文档)
---------------------------------------------------

The Sphinx books should be built with ``make {htmldocs | pdfdocs | epubdocs}``.
(Sphinx 书籍应使用 ``make {htmldocs | pdfdocs | epubdocs}`` 构建。)

    * Name: **linux/Documentation**

      :Author: Many.
      :Location: Documentation/
      :Keywords: text files, Sphinx.
      :Description: Documentation that comes with the kernel sources,
        inside the Documentation directory. Some pages from this document
        (including this document itself) have been moved there, and might
        be more up to date than the web version.

      (:Author: 众人。)
      (:Location: Documentation/)
      (:Keywords: 文本文件，Sphinx。)
      (:Description: 随内核源码一起发布、位于 Documentation 目录中的文档。本文档的部分页面(包括本文档本身)已被移到那里，可能比网页版本更新。)

On-line docs(在线文档)
----------------------

    * Title: **Linux Kernel Mailing List Glossary**

      :Author: various
      :URL: https://kernelnewbies.org/KernelGlossary
      :Date: rolling version
      :Keywords: glossary, terms, linux-kernel.
      :Description: From the introduction: "This glossary is intended as
        a brief description of some of the acronyms and terms you may hear
        during discussion of the Linux kernel".

      (标题：《Linux 内核邮件列表术语表》。)
      (:Author: 多人。)
      (:Date: 滚动版本。)
      (:Keywords: 术语表，术语，linux-kernel。)
      (:Description: 引自其导言："本术语表旨在简要描述你在讨论 Linux 内核时可能听到的一些缩略语与术语"。)

    * Title: **The Linux Kernel Module Programming Guide**

      :Author: Peter Jay Salzman, Michael Burian, Ori Pomerantz, Bob Mottram,
        Jim Huang.
      :URL: https://sysprog21.github.io/lkmpg/
      :Date: 2021
      :Keywords: modules, GPL book, /proc, ioctls, system calls,
        interrupt handlers .
      :Description: A very nice GPL book on the topic of modules
        programming. Lots of examples. Currently the new version is being
        actively maintained at https://github.com/sysprog21/lkmpg.

      (标题：《Linux 内核模块编程指南》。)
      (:Date: 2021。)
      (:Keywords: 模块，GPL 书籍，/proc，ioctl，系统调用，中断处理程序。)
      (:Description: 一本关于模块编程主题的非常好的 GPL 书籍，示例很多。目前新版本在 https://github.com/sysprog21/lkmpg 持续活跃维护。)

Published books(已出版书籍)
---------------------------

    * Title: **The Linux Memory Manager**

      :Author: Lorenzo Stoakes
      :Publisher: No Starch Press
      :Date: February 2025
      :Pages: 1300
      :ISBN: 978-1718504462
      :Notes: Memory management. Full draft available as early access for
              pre-order, full release scheduled for Fall 2025. See
              https://nostarch.com/linux-memory-manager for further info.

      (标题：《The Linux Memory Manager(Linux 内存管理器)》。)
      (:Notes: 内存管理。完整草稿已以预售早期访问形式提供，完整版计划于 2025 年秋季发布。更多信息见 https://nostarch.com/linux-memory-manager。)

    * Title: **Practical Linux System Administration: A Guide to Installation, Configuration, and Management, 1st Edition**

      :Author: Kenneth Hess
      :Publisher: O'Reilly Media
      :Date: May, 2023
      :Pages: 246
      :ISBN: 978-1098109035
      :Notes: System administration

      (标题：《Practical Linux System Administration(实用 Linux 系统管理：安装、配置与管理指南)》第 1 版。)
      (:Notes: 系统管理。)

    * Title: **Linux Kernel Debugging: Leverage proven tools and advanced techniques to effectively debug Linux kernels and kernel modules**

      :Author: Kaiwan N Billimoria
      :Publisher: Packt Publishing Ltd
      :Date: August, 2022
      :Pages: 638
      :ISBN: 978-1801075039
      :Notes: Debugging book

      (标题：《Linux Kernel Debugging(Linux 内核调试：利用经过验证的工具与高级技术有效调试 Linux 内核与内核模块)》。)
      (:Notes: 调试类书籍。)

    * Title: **Linux Kernel Programming: A Comprehensive Guide to Kernel Internals, Writing Kernel Modules, and Kernel Synchronization**

      :Author: Kaiwan N Billimoria
      :Publisher: Packt Publishing Ltd
      :Date: March, 2021 (Second Edition published in 2024)
      :Pages: 754
      :ISBN: 978-1789953435 (Second Edition ISBN is 978-1803232225)

      (标题：《Linux Kernel Programming(Linux 内核编程：内核原理、内核模块编写与内核同步综合指南)》。第二版于 2024 年出版。)

    * Title: **Linux Kernel Programming Part 2 - Char Device Drivers and Kernel Synchronization: Create user-kernel interfaces, work with peripheral I/O, and handle hardware interrupts**

      :Author: Kaiwan N Billimoria
      :Publisher: Packt Publishing Ltd
      :Date: March, 2021
      :Pages: 452
      :ISBN: 978-1801079518

      (标题：《Linux Kernel Programming Part 2(Linux 内核编程第 2 部分——字符设备驱动与内核同步：创建用户-内核接口、使用外设 I/O、处理硬件中断)》。)

    * Title: **Linux System Programming: Talking Directly to the Kernel and C Library**

      :Author: Robert Love
      :Publisher: O'Reilly Media
      :Date: June, 2013
      :Pages: 456
      :ISBN: 978-1449339531
      :Notes: Foundational book

      (标题：《Linux System Programming(Linux 系统编程：直接与内核和 C 库对话)》。)
      (:Notes: 基础性书籍。)

    * Title: **Linux Kernel Development, 3rd Edition**

      :Author: Robert Love
      :Publisher: Addison-Wesley
      :Date: July, 2010
      :Pages: 440
      :ISBN: 978-0672329463
      :Notes: Foundational book

      (标题：《Linux Kernel Development(Linux 内核开发)》第 3 版。)
      (:Notes: 基础性书籍。)

.. _ldd3_published:

    * Title: **Linux Device Drivers, 3rd Edition**

      :Authors: Jonathan Corbet, Alessandro Rubini, and Greg Kroah-Hartman
      :Publisher: O'Reilly & Associates
      :Date: 2005
      :Pages: 636
      :ISBN: 0-596-00590-3
      :Notes: Foundational book. Further information in
        http://www.oreilly.com/catalog/linuxdrive3/
        PDF format, URL: https://lwn.net/Kernel/LDD3/

      (标题：《Linux Device Drivers(Linux 设备驱动)》第 3 版。)
      (:Notes: 基础性书籍。更多信息见 http://www.oreilly.com/catalog/linuxdrive3/；PDF 格式见 https://lwn.net/Kernel/LDD3/。)

    * Title: **The Design of the UNIX Operating System**

      :Author: Maurice J. Bach
      :Publisher: Prentice Hall
      :Date: 1986
      :Pages: 471
      :ISBN: 0-13-201757-1
      :Notes: Foundational book

      (标题：《The Design of the UNIX Operating System(UNIX 操作系统设计)》。)
      (:Notes: 基础性书籍。)

Miscellaneous(杂项)
-------------------

    * Name: **Cross-Referencing Linux**

      :URL: https://elixir.bootlin.com/
      :Keywords: Browsing source code.
      :Description: Another web-based Linux kernel source code browser.
        Lots of cross references to variables and functions. You can see
        where they are defined and where they are used.

      (名称：Cross-Referencing Linux(Linux 交叉引用)。)
      (:Keywords: 浏览源码。)
      (:Description: 另一个基于 Web 的 Linux 内核源码浏览器。包含大量对变量与函数的交叉引用，可以查看它们的定义处与使用处。)

    * Name: **Linux Weekly News**

      :URL: https://lwn.net
      :Keywords: latest kernel news.
      :Description: The title says it all. There's a fixed kernel section
        summarizing developers' work, bug fixes, new features and versions
        produced during the week.

      (名称：Linux Weekly News。)
      (:Keywords: 最新内核新闻。)
      (:Description: 标题已说明一切。它有一个固定的内核栏目，汇总一周内开发者的工作、缺陷修复、新特性与新版本。)

    * Name: **The home page of Linux-MM**

      :Author: The Linux-MM team.
      :URL: https://linux-mm.org/
      :Keywords: memory management, Linux-MM, mm patches, TODO, docs,
        mailing list.
      :Description: Site devoted to Linux Memory Management development.
        Memory related patches, HOWTOs, links, mm developers... Don't miss
        it if you are interested in memory management development!

      (名称：Linux-MM 主页。)
      (:Author: Linux-MM 团队。)
      (:Keywords: 内存管理，Linux-MM，mm 补丁，TODO，文档，邮件列表。)
      (:Description: 致力于 Linux 内存管理开发的站点。内存相关补丁、HOWTO、链接、mm 开发者……如果你对内存管理开发感兴趣，千万不要错过！)

    * Name: **Kernel Newbies IRC Channel and Website**

      :URL: https://www.kernelnewbies.org
      :Keywords: IRC, newbies, channel, asking doubts.
      :Description: #kernelnewbies on irc.oftc.net.
        #kernelnewbies is an IRC network dedicated to the 'newbie'
        kernel hacker. The audience mostly consists of people who are
        learning about the kernel, working on kernel projects or
        professional kernel hackers that want to help less seasoned kernel
        people.
        #kernelnewbies is on the OFTC IRC Network.
        Try irc.oftc.net as your server and then /join #kernelnewbies.
        The kernelnewbies website also hosts articles, documents, FAQs...

      (名称：Kernel Newbies IRC 频道与网站。)
      (:Keywords: IRC，新手，频道，提问解惑。)
      (:Description: irc.oftc.net 上的 #kernelnewbies。#kernelnewbies 是一个献给"新手"内核黑客的 IRC 频道。受众主要是正在学习内核、做内核项目的人，以及愿意帮助经验较少的内核人员的职业内核黑客。#kernelnewbies 位于 OFTC IRC 网络。把 irc.oftc.net 设为你的服务器，然后 /join #kernelnewbies。kernelnewbies 网站上还有文章、文档、FAQ 等。)

    * Name: **linux-kernel mailing list archives and search engines**

      :URL: https://subspace.kernel.org
      :URL: https://lore.kernel.org
      :Keywords: linux-kernel, archives, search.
      :Description: Some of the linux-kernel mailing list archivers. If
        you have a better/another one, please let me know.

      (名称：linux-kernel 邮件列表归档与搜索引擎。)
      (:Keywords: linux-kernel，归档，搜索。)
      (:Description: linux-kernel 邮件列表的一些归档。如果你有更好的/别的归档，请告诉我。)

    * Name: **The Linux Foundation YouTube channel**

      :URL: https://www.youtube.com/user/thelinuxfoundation
      :Keywords: linux, videos, linux-foundation, youtube.
      :Description: The Linux Foundation uploads video recordings of their
        collaborative events, Linux conferences including LinuxCon, and
        other original research and content related to Linux and software
        development.

      (名称：Linux Foundation YouTube 频道。)
      (:Keywords: linux，视频，linux-foundation，youtube。)
      (:Description: Linux Foundation 上传其协作活动、Linux 会议(包括 LinuxCon)的视频录像，以及其他与 Linux 和软件开发相关的原创研究内容。)

Rust(Rust 相关文档)
-------------------

    * Title: **Rust for Linux**

      :Author: various
      :URL: https://rust-for-linux.com/
      :Date: rolling version
      :Keywords: glossary, terms, linux-kernel, rust.
      :Description: From the website: "Rust for Linux is the project adding
        support for the Rust language to the Linux kernel. This website is
        intended as a hub of links, documentation and resources related to
        the project".

      (标题：Rust for Linux(Linux 的 Rust 支持)。)
      (:Author: 多人。)
      (:Date: 滚动版本。)
      (:Keywords: 术语表，术语，linux-kernel，rust。)
      (:Description: 引自网站："Rust for Linux 是为 Linux 内核添加 Rust 语言支持的项目。本网站旨在作为与该项目相关的链接、文档与资源的中心"。)

    * Title: **Learn Rust the Dangerous Way**

      :Author: Cliff L. Biffle
      :URL: https://cliffle.com/p/dangerust/
      :Date: Accessed Sep 11 2024
      :Keywords: rust, blog.
      :Description: From the website: "LRtDW is a series of articles
        putting Rust features in context for low-level C programmers who
        maybe don’t have a formal CS background — the sort of people who
        work on firmware, game engines, OS kernels, and the like.
        Basically, people like me.". It illustrates line-by-line
        conversions from C to Rust.

      (标题：《Learn Rust the Dangerous Way(危险地学 Rust)》。)
      (:Date: 访问于 2024 年 9 月 11 日。)
      (:Keywords: rust，博客。)
      (:Description: 引自网站："LRtDW 是一系列文章，为底层 C 程序员——那些可能没有正规计算机科学背景、做固件、游戏引擎、操作系统内核之类工作的人——把 Rust 特性放到上下文中讲解。基本上，就是像我这样的人"。它逐行展示了从 C 到 Rust 的转换。)

    * Title: **The Rust Book**

      :Author: Steve Klabnik and Carol Nichols, with contributions from the
        Rust community
      :URL: https://doc.rust-lang.org/book/
      :Date: Accessed Sep 11 2024
      :Keywords: rust, book.
      :Description: From the website: "This book fully embraces the
        potential of Rust to empower its users. It’s a friendly and
        approachable text intended to help you level up not just your
        knowledge of Rust, but also your reach and confidence as a
        programmer in general. So dive in, get ready to learn—and welcome
        to the Rust community!".

      (标题：《The Rust Book(Rust 程序设计语言)》。)
      (:Date: 访问于 2024 年 9 月 11 日。)
      (:Keywords: rust，书籍。)
      (:Description: 引自网站："本书充分拥抱 Rust 赋能使用者的潜力。它是一本友好、平易近人的教材，旨在不仅提升你对 Rust 的掌握，也提升你作为程序员的视野与信心。所以，投入进来，准备学习——欢迎来到 Rust 社区！"。)

    * Title: **Rust for the Polyglot Programmer**

      :Author: Ian Jackson
      :URL: https://www.chiark.greenend.org.uk/~ianmdlvl/rust-polyglot/index.html
      :Date: December 2022
      :Keywords: rust, blog, tooling.
      :Description: From the website: "There are many guides and
        introductions to Rust. This one is something different: it is
        intended for the experienced programmer who already knows many
        other programming languages. I try to be comprehensive enough to be
        a starting point for any area of Rust, but to avoid going into too
        much detail except where things are not as you might expect. Also
        this guide is not entirely free of opinion, including
        recommendations of libraries (crates), tooling, etc.".

      (标题：《Rust for the Polyglot Programmer(写给多语言程序员的 Rust)》。)
      (:Date: 2022 年 12 月。)
      (:Keywords: rust，博客，工具链。)
      (:Description: 引自网站："关于 Rust 的指南与入门材料很多。这篇有所不同：它面向已经掌握许多其他编程语言的有经验程序员。我尽量写得足够全面，使它成为进入 Rust 任何领域的起点，但除非事情与你的预期不同，否则避免过多细节。此外，本指南并非完全不带观点，其中包括对库 (crate)、工具链等的推荐。")

    * Title: **Fasterthanli.me**

      :Author: Amos Wenger
      :URL: https://fasterthanli.me/
      :Date: Accessed Sep 11 2024
      :Keywords: rust, blog, news.
      :Description: From the website: "I make articles and videos about how
        computers work. My content is long-form, didactic and exploratory
        — and often an excuse to teach Rust!".

      (标题：Fasterthanli.me。)
      (:Date: 访问于 2024 年 9 月 11 日。)
      (:Keywords: rust，博客，新闻。)
      (:Description: 引自网站："我制作关于计算机如何工作的文章与视频。我的内容是长篇的、教学式的、探索式的——而且常常是讲授 Rust 的一个借口！")

    * Title: **Comprehensive Rust**

      :Author: Android team at Google
      :URL: https://google.github.io/comprehensive-rust/
      :Date: Accessed Sep 13 2024
      :Keywords: rust, blog.
      :Description: From the website: "The course covers the full spectrum
        of Rust, from basic syntax to advanced topics like generics and
        error handling".

      (标题：《Comprehensive Rust(Rust 全面教程)》。)
      (:Author: Google 的 Android 团队。)
      (:Date: 访问于 2024 年 9 月 13 日。)
      (:Keywords: rust，博客。)
      (:Description: 引自网站："本课程涵盖 Rust 的全部光谱，从基本语法到泛型与错误处理等高级主题"。)

    * Title: **The Embedded Rust Book**

      :Author: Multiple contributors, mostly Jorge Aparicio
      :URL: https://docs.rust-embedded.org/book/
      :Date: Accessed Sep 13 2024
      :Keywords: rust, blog.
      :Description: From the website: "An introductory book about using
        the Rust Programming Language on "Bare Metal" embedded systems,
        such as Microcontrollers".

      (标题：《The Embedded Rust Book(嵌入式 Rust 手册)》。)
      (:Author: 多位贡献者，主要为 Jorge Aparicio。)
      (:Date: 访问于 2024 年 9 月 13 日。)
      (:Keywords: rust，博客。)
      (:Description: 引自网站："一本介绍在"裸机"嵌入式系统(例如微控制器)上使用 Rust 编程语言的入门书籍"。)

   * Title: **Experiment: Improving the Rust Book**

      :Author: Cognitive Engineering Lab at Brown University
      :URL: https://rust-book.cs.brown.edu/
      :Date: Accessed Sep 22 2024
      :Keywords: rust, blog.
      :Description: From the website: "The goal of this experiment is to
        evaluate and improve the content of the Rust Book to help people
        learn Rust more effectively.".

      (标题：《Experiment: Improving the Rust Book(实验：改进 Rust 手册)》。)
      (:Author: 布朗大学认知工程实验室。)
      (:Date: 访问于 2024 年 9 月 22 日。)
      (:Keywords: rust，博客。)
      (:Description: 引自网站："本实验的目标是评估并改进 Rust Book 的内容，帮助人们更有效地学习 Rust"。)

   * Title: **New Rustacean** (podcast)

      :Author: Chris Krycho
      :URL: https://newrustacean.com/
      :Date: Accessed Sep 22 2024
      :Keywords: rust, podcast.
      :Description: From the website: "This is a podcast about learning
        the programming language Rust—from scratch! Apart from this spiffy
        landing page, all the site content is built with Rust's own
        documentation tools.".

      (标题：《New Rustacean》(播客)。)
      (:Date: 访问于 2024 年 9 月 22 日。)
      (:Keywords: rust，播客。)
      (:Description: 引自网站："这是一个关于从零开始学习 Rust 编程语言的播客！除了这个漂亮的落地页，网站的全部内容都使用 Rust 自己的文档工具构建。")

   * Title: **Opsem-team** (repository)

      :Author: Operational semantics team
      :URL: https://github.com/rust-lang/opsem-team/tree/main
      :Date: Accessed Sep 22 2024
      :Keywords: rust, repository.
      :Description: From the README: "The opsem team is the successor of
        the unsafe-code-guidelines working group and responsible for
        answering many of the difficult questions about the semantics of
        unsafe Rust".

      (标题：Opsem-team(仓库)。)
      (:Author: 操作语义团队。)
      (:Date: 访问于 2024 年 9 月 22 日。)
      (:Keywords: rust，仓库。)
      (:Description: 引自 README："opsem 团队是 unsafe-code-guidelines 工作组的后继者，负责回答许多关于不安全 Rust 语义的难题"。)

    * Title: **You Can't Spell Trust Without Rust**

      :Author: Alexis Beingessner
      :URL: https://repository.library.carleton.ca/downloads/1j92g820w?locale=en
      :Date: 2015
      :Keywords: rust, master, thesis.
      :Description: This thesis focuses on Rust's ownership system, which
        ensures memory safety by controlling data manipulation and
        lifetime, while also highlighting its limitations and comparing it
        to similar systems in Cyclone and C++.

      (标题：《You Can't Spell Trust Without Rust(不写 Rust 就拼不出 Trust)》——一篇硕士论文。)
      (:Date: 2015。)
      (:Keywords: rust，硕士，论文。)
      (:Description: 本论文聚焦 Rust 的所有权系统——它通过控制数据操作与生命周期来确保内存安全，同时也指出其局限，并与 Cyclone 和 C++ 中的类似系统进行比较。)

    * Name: **Linux Plumbers (LPC) 2024 Rust presentations**

      :Title: Rust microconference
      :URL: https://lpc.events/event/18/sessions/186/#20240918
      :Title: Rust for Linux
      :URL: https://lpc.events/event/18/contributions/1912/
      :Title: Journey of a C kernel engineer starting a Rust driver project
      :URL: https://lpc.events/event/18/contributions/1911/
      :Title: Crafting a Linux kernel scheduler that runs in user-space
        using Rust
      :URL: https://lpc.events/event/18/contributions/1723/
      :Title: openHCL: A Linux and Rust based paravisor
      :URL: https://lpc.events/event/18/contributions/1956/
      :Keywords: rust, lpc, presentations.
      :Description: A number of LPC talks related to Rust.

      (名称：Linux Plumbers (LPC) 2024 的 Rust 演讲。)
      (:Keywords: rust，lpc，演讲。)
      (:Description: 与 Rust 相关的一批 LPC 演讲。)

    * Name: **The Rustacean Station Podcast**

      :URL: https://rustacean-station.org/
      :Keywords: rust, podcasts.
      :Description: A community project for creating podcast content for
        the Rust programming language.

      (名称：The Rustacean Station Podcast。)
      (:Keywords: rust，播客。)
      (:Description: 为 Rust 编程语言制作播客内容的社区项目。)

-------

This document was originally based on:

 https://www.dit.upm.es/~jmseyas/linux/kernel/hackers-docs.html

and written by Juan-Mariano de Goyeneche

(本文档最初基于 https://www.dit.upm.es/~jmseyas/linux/kernel/hackers-docs.html，由 Juan-Mariano de Goyeneche 编写。)
