问题：

ls -a -lh 看不到占用内存的文件；

df -h 发现tmp目录被占满。

```
最后发现原因：
fopen()打开/tmp下的文件，没有fclose()掉就unlink()删除掉。
```



---

lsof |grep deleted 

linux lsof命令详解 - 星火spark - 博客园 - https://www.cnblogs.com/sparkbj/p/7161669.html

linux lsof命令-80后小菜鸟-51CTO博客 - https://blog.51cto.com/zhangxinqi/2374118

```
cd /tmp
du -sh
df

lsof | grep /tmp
```

(2条消息)追查一次/tmp目录打满的问题_woai110120130的专栏-CSDN博客 - https://blog.csdn.net/woai110120130/article/details/105046679

linux tmp目录满了，却找不到占满磁盘文件的解决方法_海天狼的博客-CSDN博客_tmp目录满 - https://blog.csdn.net/sclfox/article/details/8564454



---

tmpfs 是 Linux/Unix 系统上的一种基于内存的文件系统。

文件存储之-内存文件系统tmpfs - 杨小愚 - 博客园 - https://www.cnblogs.com/yangliheng/p/6187193.html

tmpfs 内存文件系统_阁-CSDN博客_mount -t tmpfs -o size=1024m tmpfs /tmp/memory - https://blog.csdn.net/cherisegege/article/details/79311621



---

tmpfs 是占用内存。
tmpfs一开始使用很小的空间，但随着文件的复制和创建，tmpfs文件系统会分配更多的内存，并按照需求动态地增加文件系统的空间。而且，当tmpfs中的文件被删除时，tmpfs文件系统会动态地减小文件并释放内存资源。