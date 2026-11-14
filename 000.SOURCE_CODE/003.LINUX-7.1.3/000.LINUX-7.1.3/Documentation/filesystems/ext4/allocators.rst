.. SPDX-License-Identifier: GPL-2.0

Block and Inode Allocation Policy
(块与 inode 分配策略)
---------------------------------

ext4 recognizes (better than ext3, anyway) that data locality is
generally a desirably quality of a filesystem. On a spinning disk,
keeping related blocks near each other reduces the amount of movement
that the head actuator and disk must perform to access a data block,
thus speeding up disk IO. On an SSD there of course are no moving parts,
but locality can increase the size of each transfer request while
reducing the total number of requests. This locality may also have the
effect of concentrating writes on a single erase block, which can speed
up file rewrites significantly. Therefore, it is useful to reduce
fragmentation whenever possible.
(ext4 认识到(反正比 ext3 认识得更清楚)数据局部性通常是文件系统的一个理想特性。在机械硬盘上，让相关的块彼此靠近可以减少磁头执行机构和磁盘为访问数据块而必须执行的移动量，从而加快磁盘 IO。在 SSD 上当然没有活动部件，但局部性可以增大每个传输请求的大小，同时减少请求的总数。这种局部性还可能使写入集中在单个擦除块上，从而显著加快文件重写的速度。因此，尽可能地减少碎片是有益的。)

The first tool that ext4 uses to combat fragmentation is the multi-block
allocator. When a file is first created, the block allocator
speculatively allocates 8KiB of disk space to the file on the assumption
that the space will get written soon. When the file is closed, the
unused speculative allocations are of course freed, but if the
speculation is correct (typically the case for full writes of small
files) then the file data gets written out in a single multi-block
extent. A second related trick that ext4 uses is delayed allocation.
Under this scheme, when a file needs more blocks to absorb file writes,
the filesystem defers deciding the exact placement on the disk until all
the dirty buffers are being written out to disk. By not committing to a
particular placement until it's absolutely necessary (the commit timeout
is hit, or sync() is called, or the kernel runs out of memory), the hope
is that the filesystem can make better location decisions.
(ext4 用来对抗碎片化的第一个工具是多块分配器。当一个文件首次创建时，块分配器假定该空间很快就会被写入，从而试探性地为该文件分配 8KiB 的磁盘空间。当文件关闭时，未使用的试探性分配当然会被释放，但如果猜测是正确的(对于小文件的完整写入通常如此)，文件数据就会以单个多块 extent 的形式写出。ext4 使用的第二个相关技巧是延迟分配。在这一方案下，当文件需要更多块来容纳文件写入时，文件系统会推迟决定在磁盘上的确切位置，直到所有脏缓冲被写出到磁盘。通过在绝对必要之前(提交超时到期、调用 sync() 或内核内存耗尽)不承诺特定的位置，希望文件系统能够做出更好的位置决策。)

The third trick that ext4 (and ext3) uses is that it tries to keep a
file's data blocks in the same block group as its inode. This cuts down
on the seek penalty when the filesystem first has to read a file's inode
to learn where the file's data blocks live and then seek over to the
file's data blocks to begin I/O operations.
(ext4(和 ext3)使用的第三个技巧是尽量将文件的数据块保存在与其 inode 相同的块组中。这可以减少寻道开销 —— 文件系统必须先读取文件的 inode 以了解文件数据块的位置，然后再寻道到文件数据块处开始 I/O 操作。)

The fourth trick is that all the inodes in a directory are placed in the
same block group as the directory, when feasible. The working assumption
here is that all the files in a directory might be related, therefore it
is useful to try to keep them all together.
(第四个技巧是，在可行的情况下，将目录中的所有 inode 放在与该目录相同的块组中。这里的工作假设是同一目录中的所有文件可能是相关的，因此尽量将它们保存在一起是有益的。)

The fifth trick is that the disk volume is cut up into 128MB block
groups; these mini-containers are used as outlined above to try to
maintain data locality. However, there is a deliberate quirk -- when a
directory is created in the root directory, the inode allocator scans
the block groups and puts that directory into the least heavily loaded
block group that it can find. This encourages directories to spread out
over a disk; as the top-level directory/file blobs fill up one block
group, the allocators simply move on to the next block group. Allegedly
this scheme evens out the loading on the block groups, though the author
suspects that the directories which are so unlucky as to land towards
the end of a spinning drive get a raw deal performance-wise.
(第五个技巧是将磁盘卷切分成 128MB 的块组；如上所述，这些小型容器被用来尽力维持数据局部性。不过，这里有一个故意为之的特殊处理 —— 当在根目录中创建目录时，inode 分配器会扫描各块组，并将该目录放入它能找到的负载最轻的块组。这促使目录在磁盘上分散开来；随着顶层目录/文件块填满一个块组，分配器简单地转向下一个块组。据称这一方案能均衡各块组的负载，不过作者怀疑那些不幸落在机械硬盘末尾附近的目录在性能上吃了亏。)

Of course if all of these mechanisms fail, one can always use e4defrag
to defragment files.
(当然，如果所有这些机制都失效了，总可以使用 e4defrag 来整理文件碎片。)
