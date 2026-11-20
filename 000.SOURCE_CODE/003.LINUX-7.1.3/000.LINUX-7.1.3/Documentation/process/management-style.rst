.. _managementstyle:

Linux kernel management style(Linux 内核管理风格)
==================================================

This is a short document describing the preferred (or made up, depending
on who you ask) management style for the linux kernel.  It's meant to
mirror the :ref:`process/coding-style.rst <codingstyle>` document to some
degree, and mainly written to avoid answering [#f1]_  the same (or similar)
questions over and over again.
(这是一篇简短的文档，描述 Linux 内核首选的 (或者说是编造的，取决于你问
谁) 管理风格。它在某种程度上与 coding-style 文档相呼应，主要是为了避免
反复回答相同 (或类似) 的问题。)

Management style is very personal and much harder to quantify than
simple coding style rules, so this document may or may not have anything
to do with reality.  It started as a lark, but that doesn't mean that it
might not actually be true. You'll have to decide for yourself.
(管理风格是非常个人化的东西，比简单的编码风格规则难以量化得多，所以本文
档可能与现实有关，也可能无关。它一开始只是个玩笑，但这并不意味着它就不
是真的。你得自己判断。)

Btw, when talking about "kernel manager", it's all about the technical
lead persons, not the people who do traditional management inside
companies.  If you sign purchase orders or you have any clue about the
budget of your group, you're almost certainly not a kernel manager.
These suggestions may or may not apply to you.
(顺便说一句，谈到 "内核管理者" 时，说的都是技术负责人，而不是在公司里
做传统管理工作的人。如果你签署采购订单，或者对你的团队预算有任何概念，
那你几乎可以肯定不是内核管理者。这些建议可能适用于你，也可能不适用。)

First off, I'd suggest buying "Seven Habits of Highly Effective
People", and NOT read it.  Burn it, it's a great symbolic gesture.
(首先，我建议买一本《高效能人士的七个习惯》(Seven Habits of Highly
Effective People)，然后千万别去读它。把它烧掉，这是一个很棒的象征性
姿态。)

.. [#f1] This document does so not so much by answering the question, but by
  making it painfully obvious to the questioner that we don't have a clue
  to what the answer is.
  (本文档做到这一点，与其说靠回答问题，不如说靠让提问者痛苦地意识到：我们
  根本不知道答案是什么。)

Anyway, here goes:
(闲话少说，开始吧：)

.. _decisions:

1) Decisions(决策)
------------------

Everybody thinks managers make decisions, and that decision-making is
important.  The bigger and more painful the decision, the bigger the
manager must be to make it.  That's very deep and obvious, but it's not
actually true.
(每个人都认为管理者做决策，而且决策很重要。决策越重大、越痛苦，做决策的
管理者就必须越强大。这个说法非常深刻也非常显然，但它实际上并不正确。)

The name of the game is to **avoid** having to make a decision.  In
particular, if somebody tells you "choose (a) or (b), we really need you
to decide on this", you're in trouble as a manager.  The people you
manage had better know the details better than you, so if they come to
you for a technical decision, you're screwed.  You're clearly not
competent to make that decision for them.
(这个游戏的名字叫做 **避免** 不得不做决策。具体来说，如果有人对你说
"选择 (a) 或 (b)，我们真的需要你来定夺"，那你这个管理者就有麻烦了。你
管理的人最好比你更了解细节，所以如果他们来找你做技术决策，你就完蛋了
—— 你显然没有资格替他们做这个决定。)

(Corollary:if the people you manage don't know the details better than
you, you're also screwed, although for a totally different reason.
Namely that you are in the wrong job, and that **they** should be managing
your brilliance instead).
(推论：如果你管理的人并不比你更了解细节，那你同样完蛋，只不过原因完全
不同 —— 那就是你入错了行，应该是 **他们** 来管理你的才华。)

So the name of the game is to **avoid** decisions, at least the big and
painful ones.  Making small and non-consequential decisions is fine, and
makes you look like you know what you're doing, so what a kernel manager
needs to do is to turn the big and painful ones into small things where
nobody really cares.
(所以，这个游戏的名字就是 **避免** 决策，至少避免那些重大而痛苦的决策。
做微小而无足轻重的决策没什么问题，还能让你显得知道自己在做什么；所以
内核管理者需要做的，就是把重大而痛苦的决策变成没什么人真正在乎的小事。)

It helps to realize that the key difference between a big decision and a
small one is whether you can fix your decision afterwards.  Any decision
can be made small by just always making sure that if you were wrong (and
you **will** be wrong), you can always undo the damage later by
backtracking.  Suddenly, you get to be doubly managerial for making
**two** inconsequential decisions - the wrong one **and** the right one.
(认识到重大决策与微小决策之间的关键区别在于你事后能否修正决策，会很有
帮助。只要始终确保当你错了时 (而你 **一定** 会错)，你总是可以在事后通过
回退来弥补损害，任何决策都可以变得微小。突然之间，因为做出了 **两个**
无足轻重的决策 —— 错的那个 **以及** 对的那个 —— 你成了加倍称职的
管理者。)

And people will even see that as true leadership (*cough* bullshit
*cough*).
(而且人们甚至会把这视为真正的领导力 (*咳咳* 狗屁 *咳咳*)。)

Thus the key to avoiding big decisions becomes to just avoiding to do
things that can't be undone.  Don't get ushered into a corner from which
you cannot escape.  A cornered rat may be dangerous - a cornered manager
is just pitiful.
(于是，避免重大决策的关键就变成了避免做无法挽回的事情。不要被逼进无法
逃脱的角落。被逼到角落的老鼠可能很危险 —— 被逼到角落的管理者只是可怜。)

It turns out that since nobody would be stupid enough to ever really let
a kernel manager have huge fiscal responsibility **anyway**, it's usually
fairly easy to backtrack.  Since you're not going to be able to waste
huge amounts of money that you might not be able to repay, the only
thing you can backtrack on is a technical decision, and there
back-tracking is very easy: just tell everybody that you were an
incompetent nincompoop, say you're sorry, and undo all the worthless
work you had people work on for the last year.  Suddenly the decision
you made a year ago wasn't a big decision after all, since it could be
easily undone.
(事实证明，反正没有人会蠢到真的让内核管理者承担巨大的财政责任，所以
回退通常相当容易。既然你不可能挥霍掉自己可能无力偿还的巨额金钱，你唯一
能回退的就是技术决策，而在这方面回退非常容易：只需告诉大家你是个无能的
蠢货，说声抱歉，然后撤销你让人们在过去一年里做的所有毫无价值的工作。
突然之间，你一年前做的决策就不再是重大决策了，因为它可以轻松撤销。)

It turns out that some people have trouble with this approach, for two
reasons:
(事实证明，有些人难以接受这种做法，原因有二：)

 - admitting you were an idiot is harder than it looks.  We all like to
   maintain appearances, and coming out in public to say that you were
   wrong is sometimes very hard indeed.
   (承认自己是白痴比看上去更难。我们都喜欢维持体面，公开站出来说自己
   错了有时确实非常困难。)
 - having somebody tell you that what you worked on for the last year
   wasn't worthwhile after all can be hard on the poor lowly engineers
   too, and while the actual **work** was easy enough to undo by just
   deleting it, you may have irrevocably lost the trust of that
   engineer.  And remember: "irrevocable" was what we tried to avoid in
   the first place, and your decision ended up being a big one after
   all.
   (告诉别人你过去一年的工作终究没有价值，对那些可怜的低阶工程师来说
   也可能难以接受；虽然实际的 **工作** 本身删除掉就能轻松撤销，但你可能
   已经无可挽回地失去了那位工程师的信任。记住："无可挽回" 正是我们
   一开始想要避免的，而你的决策最终还是会变成重大决策。)

Happily, both of these reasons can be mitigated effectively by just
admitting up-front that you don't have a friggin' clue, and telling
people ahead of the fact that your decision is purely preliminary, and
might be the wrong thing.  You should always reserve the right to change
your mind, and make people very **aware** of that.  And it's much easier
to admit that you are stupid when you haven't **yet** done the really
stupid thing.
(令人高兴的是，只要一开始就坦白承认自己根本毫无头绪，提前告诉人们你的
决策纯属初步性质、而且可能是错的，这两个原因都可以得到有效缓解。你应该
始终保留改变主意的权利，并让人们非常清楚地 **意识到** 这一点。在你
**尚未** 做出真正愚蠢的事情之前，承认自己愚蠢要容易得多。)

Then, when it really does turn out to be stupid, people just roll their
eyes and say "Oops, not again".
(然后，当事情真的被证明很愚蠢时，人们只会翻个白眼说 "哎呀，又来了"。)

This preemptive admission of incompetence might also make the people who
actually do the work also think twice about whether it's worth doing or
not.  After all, if **they** aren't certain whether it's a good idea, you
sure as hell shouldn't encourage them by promising them that what they
work on will be included.  Make them at least think twice before they
embark on a big endeavor.
(这种抢先承认自己无能的做法，也可能让真正做工作的人再三思考这件事是否
值得做。毕竟，如果 **他们** 自己都不确定这是不是个好主意，你当然更不应
该向他们承诺他们做的东西一定会被纳入，以此来鼓励他们。至少要让他们在
着手一项重大事业之前三思。)

Remember: they'd better know more about the details than you do, and
they usually already think they have the answer to everything.  The best
thing you can do as a manager is not to instill confidence, but rather a
healthy dose of critical thinking on what they do.
(记住：他们最好比你更了解细节，而且他们通常已经自认为对一切都有了答案。
作为管理者，你能做的最好的事情不是灌输信心，而是对他们所做的事情注入
健康剂量的批判性思考。)

Btw, another way to avoid a decision is to plaintively just whine "can't
we just do both?" and look pitiful.  Trust me, it works.  If it's not
clear which approach is better, they'll eventually figure it out.  The
answer may end up being that both teams get so frustrated by the
situation that they just give up.
(顺便说一句，避免决策的另一种方法是可怜巴巴地哀嚎 "我们不能两个都做
吗？" 并摆出一副可怜相。相信我，这招管用。如果哪种方案更好并不明确，
他们最终会弄清楚的。答案也可能变成：两个团队都被这种局面搞得心灰意冷，
索性放弃了。)

That may sound like a failure, but it's usually a sign that there was
something wrong with both projects, and the reason the people involved
couldn't decide was that they were both wrong.  You end up coming up
smelling like roses, and you avoided yet another decision that you could
have screwed up on.
(这听起来像是失败，但这通常是一个信号，表明两个项目本身都有问题，而当事
人无法决断的原因是他们俩都错了。你最终会落得浑身玫瑰香，并且又避免了一个
你可能搞砸的决策。)


2) People(人员)
---------------

Most people are idiots, and being a manager means you'll have to deal
with it, and perhaps more importantly, that **they** have to deal with
**you**.
(大多数人都是白痴，成为管理者意味着你必须应对这一点，而且更重要的是，
**他们** 也不得不应对 **你**。)

It turns out that while it's easy to undo technical mistakes, it's not
as easy to undo personality disorders.  You just have to live with
theirs - and yours.
(事实证明，虽然技术上的错误容易撤销，人格障碍却没那么容易撤销。你只能
忍受他们的 —— 以及你自己的。)

However, in order to prepare yourself as a kernel manager, it's best to
remember not to burn any bridges, bomb any innocent villagers, or
alienate too many kernel developers. It turns out that alienating people
is fairly easy, and un-alienating them is hard. Thus "alienating"
immediately falls under the heading of "not reversible", and becomes a
no-no according to :ref:`decisions`.
(不过，为了让自己做好成为内核管理者的准备，最好记住不要烧掉任何桥梁、
轰炸任何无辜村民、或疏远太多内核开发者。事实证明，疏远别人相当容易，
而重新拉近关系却很难。于是 "疏远" 立即落入 "不可逆" 的范畴，按照
"决策" 一节的规则成为禁忌。)

There's just a few simple rules here:
(这里只有几条简单的规则：)

 (1) don't call people d*ckheads (at least not in public)
 (不要叫别人 d*ckhead，至少不要在公开场合这么叫)
 (2) learn how to apologize when you forgot rule (1)
 (当你忘了规则 (1) 时，要学会道歉)

The problem with #1 is that it's very easy to do, since you can say
"you're a d*ckhead" in millions of different ways [#f2]_, sometimes without
even realizing it, and almost always with a white-hot conviction that
you are right.
(规则 #1 的问题在于违反它太容易了，因为你可以用上百万种不同的方式说
"你是个 d*ckhead"，有时甚至意识不到自己在说，而且几乎总是带着一种炽热
的确信：你是对的。)

And the more convinced you are that you are right (and let's face it,
you can call just about **anybody** a d*ckhead, and you often **will** be
right), the harder it ends up being to apologize afterwards.
(而你越是确信自己是对的 (面对现实吧，你几乎可以把 **任何** 人叫做
d*ckhead，而且你常常 **确实** 会是对的)，事后道歉就越难。)

To solve this problem, you really only have two options:
(要解决这个问题，你实际上只有两个选择：)

 - get really good at apologies
   (把道歉练得炉火纯青)
 - spread the "love" out so evenly that nobody really ends up feeling
   like they get unfairly targeted.  Make it inventive enough, and they
   might even be amused.
   (把 "爱" 撒得足够均匀，让没有人真正觉得自己被不公平地针对了。做得
   足够有创意的话，他们甚至可能觉得有趣。)

The option of being unfailingly polite really doesn't exist. Nobody will
trust somebody who is so clearly hiding their true character.
(始终保持彬彬有礼的选项其实并不存在。没有人会信任一个如此明显地在掩饰
真实性格的人。)

.. [#f2] Paul Simon sang "Fifty Ways to Leave Your Lover", because quite
  frankly, "A Million Ways to Tell a Developer They're a D*ckhead" doesn't
  scan nearly as well.  But I'm sure he thought about it.
  (Paul Simon 唱过 "Fifty Ways to Leave Your Lover" (离开你的情人的五十
  种方法)，因为坦白说，"A Million Ways to Tell a Developer They're a
  D*ckhead" (告诉开发者他们是 d*ckhead 的一百万种方法) 押韵效果差得多。
  不过我敢肯定他考虑过。)


3) People II - the Good Kind(人员 II - 优秀的那类)
--------------------------------------------------

While it turns out that most people are idiots, the corollary to that is
sadly that you are one too, and that while we can all bask in the secure
knowledge that we're better than the average person (let's face it,
nobody ever believes that they're average or below-average), we should
also admit that we're not the sharpest knife around, and there will be
other people that are less of an idiot than you are.
(事实证明大多数人都是白痴，而其令人遗憾的推论是：你也是其中之一。虽然
我们都可以沉浸在比一般人强的安全感中 (面对现实吧，没有人认为自己是普通
人或低于平均水平)，我们也应该承认自己并非最聪明的人，总有别人没你那么
蠢。)

Some people react badly to smart people.  Others take advantage of them.
(有些人对聪明人反应恶劣。另一些人则利用他们。)

Make sure that you, as a kernel maintainer, are in the second group.
Suck up to them, because they are the people who will make your job
easier. In particular, they'll be able to make your decisions for you,
which is what the game is all about.
(作为内核维护者，要确保你属于第二类。要巴结他们，因为正是这些人会让
你的工作更轻松。特别是，他们能替你做决策 —— 而这正是这个游戏的核心。)

So when you find somebody smarter than you are, just coast along.  Your
management responsibilities largely become ones of saying "Sounds like a
good idea - go wild", or "That sounds good, but what about xxx?".  The
second version in particular is a great way to either learn something
new about "xxx" or seem **extra** managerial by pointing out something the
smarter person hadn't thought about.  In either case, you win.
(所以，当你发现比自己聪明的人时，跟着顺势而为就好。你的管理职责很大
程度上变成了说 "听起来是个好主意 —— 放手去干吧"，或者 "听起来不错，
但 xxx 怎么办？"。尤其是后一种说法，是一种绝佳的方式：要么让你学到关于
"xxx" 的新东西，要么通过指出聪明人没想过的事情而显得 **格外** 有管理
范儿。无论哪种情况，你都是赢家。)

One thing to look out for is to realize that greatness in one area does
not necessarily translate to other areas.  So you might prod people in
specific directions, but let's face it, they might be good at what they
do, and suck at everything else.  The good news is that people tend to
naturally gravitate back to what they are good at, so it's not like you
are doing something irreversible when you **do** prod them in some
direction, just don't push too hard.
(需要警惕的一点是：一个领域的杰出并不必然转化为其他领域的杰出。所以你
可以在特定方向上轻推人们，但面对现实吧，他们可能擅长自己做的事，而对
其他一切都一窍不通。好消息是，人们往往自然会回归自己擅长的领域，所以
当你 **确实** 在某个方向上轻推他们时，你并没有做什么不可逆转的事 ——
只是别推得太狠。)


4) Placing blame(归咎于人)
--------------------------

Things will go wrong, and people want somebody to blame. Tag, you're it.
(事情总会出岔子，而人们想找个可以怪罪的人。贴标签了，就是你。)

It's not actually that hard to accept the blame, especially if people
kind of realize that it wasn't **all** your fault.  Which brings us to the
best way of taking the blame: do it for someone else. You'll feel good
for taking the fall, they'll feel good about not getting blamed, and the
person who lost their whole 36GB porn-collection because of your
incompetence will grudgingly admit that you at least didn't try to weasel
out of it.
(接受指责其实没那么难，尤其是当人们多少意识到这 **并不** 全是你的错时。
这就引出了背锅的最佳方式：替别人背。你因为顶了锅而感觉良好，他们因为没有
被怪罪而感觉良好，而那个因为你无能而丢了整个 36GB 色情收藏的人也会不情
不愿地承认：你至少没有试图推卸责任。)

Then make the developer who really screwed up (if you can find them) know
**in private** that they screwed up.  Not just so they can avoid it in the
future, but so that they know they owe you one.  And, perhaps even more
importantly, they're also likely the person who can fix it.  Because, let's
face it, it sure ain't you.
(然后，让真正搞砸的开发者 (如果你能找到的话) 私下知道是他们搞砸了。这
不仅是为了让他们将来能避免，也是为了让他们知道欠你一次。而且，也许更
重要的是，他们很可能也是能修复问题的人。因为，面对现实吧，那个人肯定
不是你。)

Taking the blame is also why you get to be manager in the first place.
It's part of what makes people trust you, and allow you the potential
glory, because you're the one who gets to say "I screwed up".  And if
you've followed the previous rules, you'll be pretty good at saying that
by now.
(愿意背锅也正是你当初能当上管理者的原因。这是让人们信任你、允许你拥有
潜在荣耀的一部分：因为你是那个可以说 "我搞砸了" 的人。如果你遵循了前面
的规则，到现在你说这句话的功夫应该已经相当纯熟了。)


5) Things to avoid(应当避免的事)
--------------------------------

There's one thing people hate even more than being called "d*ckhead",
and that is being called a "d*ckhead" in a sanctimonious voice.  The
first you can apologize for, the second one you won't really get the
chance.  They likely will no longer be listening even if you otherwise
do a good job.
(有一件事比被叫做 "d*ckhead" 更招人恨，那就是用道貌岸然的语气被称为
"d*ckhead"。前者你还可以道歉，后者你基本没机会了。即使你其他方面做得
很好，他们很可能也根本不会再听了。)

We all think we're better than anybody else, which means that when
somebody else puts on airs, it **really** rubs us the wrong way.  You may
be morally and intellectually superior to everybody around you, but
don't try to make it too obvious unless you really **intend** to irritate
somebody [#f3]_.
(我们都认为自己比任何别人都强，这意味着当别人摆架子时会 **格外** 让我们
反感。你可能在道德和智力上都优于周围的每一个人，但除非你真的 **有意**
要惹恼某人，否则不要让它表现得太明显。)

Similarly, don't be too polite or subtle about things. Politeness easily
ends up going overboard and hiding the problem, and as they say, "On the
internet, nobody can hear you being subtle". Use a big blunt object to
hammer the point in, because you can't really depend on people getting
your point otherwise.
(同样，不要对事情过于礼貌或含蓄。礼貌很容易过火并掩盖问题，而且正如
人们所说："在互联网上，没有人能听出你的含蓄"。用一个大而钝的物体把要点
敲进去，因为否则你真的不能指望别人领会你的意思。)

Some humor can help pad both the bluntness and the moralizing.  Going
overboard to the point of being ridiculous can drive a point home
without making it painful to the recipient, who just thinks you're being
silly.  It can thus help get through the personal mental block we all
have about criticism.
(适度的幽默可以缓解生硬与说教两者的冲击。夸张到荒谬的程度可以把一个
观点讲到家，而不会让接收者感到痛苦 —— 对方只会觉得你在犯傻。这因此有
助于穿透我们所有人心里对批评的那道心理关卡。)

.. [#f3] Hint: internet newsgroups that are not directly related to your work
  are great ways to take out your frustrations at other people. Write
  insulting posts with a sneer just to get into a good flame every once in
  a while, and you'll feel cleansed. Just don't crap too close to home.
  (提示：与你的工作没有直接关系的互联网新闻组，是向别人发泄怨气的好
  去处。偶尔带着讥讽写一些侮辱性的帖子来掀起一场骂战，你会觉得被洗净了。
  只是别把垃圾拉得离家太近。)


6) Why me?(为什么是我)
-----------------------

Since your main responsibility seems to be to take the blame for other
peoples mistakes, and make it painfully obvious to everybody else that
you're incompetent, the obvious question becomes one of why do it in the
first place?
(既然你的主要职责似乎是为别人的错误背锅，并向所有人痛苦地彰显你的无能，
那么显而易见的问题就变成了：当初为什么要干这个？)

First off, while you may or may not get screaming teenage girls (or
boys, let's not be judgmental or sexist here) knocking on your dressing
room door, you **will** get an immense feeling of personal accomplishment
for being "in charge".  Never mind the fact that you're really leading
by trying to keep up with everybody else and running after them as fast
as you can.  Everybody will still think you're the person in charge.
(首先，虽然可能不会有尖叫的少女 (或少男 —— 我们在这里不做评判，也不
搞性别歧视) 敲你的更衣室门，但你 **一定会** 因为 "当家的" 而获得巨大的
个人成就感。别介意你其实是在努力跟上所有人、并以最快速度追赶他们的方式
来领导。大家仍然会认为你是那个当家的人。)

It's a great job if you can hack it.
(如果你能扛得住，这是一份很棒的工作。)
