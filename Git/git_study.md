

-------------------------------------------------------------------2020-07-26------------------------------------------------------------------------

Git 添加空文件夹的方法 - today4king - 博客园 - https://www.cnblogs.com/jinzhao/archive/2012/03/21/2410156.html

```
在空目录下创建.gitignore文件。
文件内写入如下代码，可以排除空目录下所有文件被跟踪：
# Ignore everything in this directory 
* 
# Except this file !.gitignore 
```



---

git重命名远程分支_huangyimo的专栏-CSDN博客_git 远程分支重命名 - https://blog.csdn.net/huangyimo/article/details/88062016

将远程分支的dev分支重命名为develop分支：

1.先删除远程分支

```
git push --delete origin dev
```

2.重命名本地分支

```
git branch -m dev develop
```

3.重新提交一个远程分支

```
git push origin develop
```

----

git 合并指定文件_si_ke的博客-CSDN博客_git 合并指定文件 - https://blog.csdn.net/si_ke/article/details/86237864

只想将B分支的某个文件test.txt合并到A分支上。

```
git checkout A
git checkout --patch B test.txt第一个命令： 切换到A分支；
```

第二个命令：合并B分支上test.txt文件到A分支上。

此时命令行会显示 Apply this hunk to index and worktree [y,n,q,a,d,/,K,j,J,g,e,?]?
这是什么意思呢？

```
y - 存储这个hunk
n - 不存储这个hunk
q - 离开，不存储这个hunk和其他hunk
a - 存储这个hunk和这个文件后面的hunk
d - 不存储这个hunk和这个文件后面的hunk
g - 选择一个hunk
/ - 通过正则查找hunk
j - 不确定是否存储这个hunk，看下一个不确定的hunk
J - 不确定是否存储这个hunk，看下一个hunk
k - 不确定是否存储这个hunk，看上一个不确定的hunk
K -不确定是否存储这个hunk，看上一个hunk
s - 把当前的hunk分成更小的hunks
e - 手动编辑当前的hunk
? - 输出帮助信息
```

如果A分支上没有test.txt文件，用上面的命令是不行的，这时候可以用：
git checkout B test.txt 把B分支的test.txt文件copy过来

---

```
$ git push origin test:test              // 提交本地test分支作为远程的test分支
```

【Git】rebase 用法小结 - 简书 - https://www.jianshu.com/p/4a8f4af4e803

【Git】rebase 用法小结 - 简书 - https://www.jianshu.com/p/4a8f4af4e803

----

gitk界面中文乱码的问题解决方案:

```
 git config --global gui.encoding utf-8
```

---

在GIT中增加忽略文件和文件夹_weixin_42076409的博客-CSDN博客_git 添加忽略文件夹 - https://blog.csdn.net/weixin_42076409/article/details/80324007

---

Git解决中文乱码问题 - 为牧 - 博客园 - https://www.cnblogs.com/ovim/p/12303180.html

```
Git解决中文乱码问题
git status 乱码
解决方法：
git config --global core.quotepath false

git commit 乱码
解决方法：
git config --global i18n.commitencoding utf-8

git status 乱码
解决方法：
git config --global i18n.logoutputencoding utf-8

注意：如果是Linux系统，需要设置环境变量 export LESSCHARSET=utf-8

解决gitk中文乱码：
git config --global gui.encoding utf-8
```

---

---

2020-12-29:

git bundle 用法：

1.找到最近同一commit 点：

A:

```
commit 5606664d7bc9f840ea4577b8c085b8a4a9308ef0
Author: jdyyoung <347587159@qq.com>
Date:   Wed Dec 9 23:47:42 2020 +0800

    119:20201209

commit c53c0fbf20f2aabb88735194eabe61b23112522f
Author: Young <347587159@qq.com>
Date:   Sat Jun 27 10:12:32 2020 +0800

    20200627-02

```

B:

```
commit c53c0fbf20f2aabb88735194eabe61b23112522f
Author: Young <347587159@qq.com>
Date:   Sat Jun 27 10:12:32 2020 +0800

    20200627-02

commit 3ca0b075e0294fef596a891c550aa51f5d17ec52
Author: jdyyoung <347587159@qq.com>
Date:   Sat Jun 27 10:00:34 2020 +0800

    20200627-01
```

A 与B 相差一个commit:5606664d7bc9f840ea4577b8c085b8a4a9308ef0

相同的点的commit:c53c0fbf20f2aabb88735194eabe61b23112522f



2.在A仓库创建bundle:

```
#把tmp分支commit-id:c53c0fbf 之后的commit 打包成commit.bundle
git bundle create commit.bundle tmp ^c53c0fbf
```

3.拷贝bundle文件到B 仓库

```
cp commit.bundle  ../Study
```

4.B仓库校验同源

注：通常要在B 仓库创建同源（相同commit点）的分支，并在这分支导入bundle

```
#git checkout -b tmp
#git reset --hard c53c0fbf
git bundle verify commit.bundle
```

4.B 仓库导入bundle

```
#从commit.bundle 提取tmp分支(bundle的分支)到tmp_b分支(将被新建的分支，且该分支不能是已存在的分支)
git fetch commit.bundle tmp:tmp_b
```

5.合并tmp分支提交master分支：

```
git cherry-pick tmp 5606664d
```

6.删除tmp分支

```
git branch -D tmp
```



---

出现错误：

```
young@young-desktop:~/Study$ git fetch commit.bundle tmp:tmp 
fatal: 拒绝获取到非纯仓库的当前分支 refs/heads/tmp
```

原因：后面个tmp,该分支已存在

---

---

git reset HEAD^  //保留代码
git reset --hard HEAD^ //不保留代码
commit id（版本号）
用HEAD表示当前版本，也就是最新的提交1094adb...
--hard参数有啥意义:
Git为我们自动创建的第一个分支master，以及指向master的一个指针叫HEAD
HEAD指向的就是当前分支

//=========================================================================================
Git - 打包 - https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E6%89%93%E5%8C%85

git log
git bundle create commits.bundle master ^9a466c5
cp commits.bundle  /xxxxxx
cd /xxxxxx
git bundle verify commits.bundle
git branch temp
$ git fetch ../commits.bundle master:temp
git merge temp
git branch -D temp

---

(1条消息)git 拉取远程分支到本地_carfge的博客-CSDN博客_git拉取远程分支 - https://blog.csdn.net/carfge/article/details/79691360

(2条消息)合并其他分支的一个commit_ejennahuang的博客-CSDN博客_合并其他分支commit - https://blog.csdn.net/ejennahuang/article/details/98640238

```
git push origin --delete paul_pir_test

git branch -a

git cherry-pick 上面复制的那个要合并的commit id  // 提交该commit到当前分支

git push origin --delete slim2-v4054-OTA

git tag -d slim2-v4054-OTA
```

