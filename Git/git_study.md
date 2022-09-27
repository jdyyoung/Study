

解决server certificate verification failed. CAfile: /etc/ssl/certs/ca-certificates.crt CRLfile: none

```
git config --global http.sslverify false
```



 git报错：git@gitee.com: Permission denied (publickey)_soldier_jw的博客-CSDN博客_nodejs - https://blog.csdn.net/soldier_jw/article/details/106183099

`vi config`

```
 # gitee
Host gitee.com
HostName gitee.com
PreferredAuthentications publickey
IdentityFile ~/.ssh/gitee/id_rsa
 
# github
Host github.com
HostName github.com
PreferredAuthentications publickey
IdentityFile ~/.ssh/github/id_rsa
```



---

Git log 高级用法 - 张建斌 - 博客园 - https://www.cnblogs.com/zhangjianbin/p/7778625.html

 git 跨分支搜索代码，确定commit 所属分支_杰_happy的博客-CSDN博客 - https://blog.csdn.net/liujiekkk123/article/details/78246662

```
git 跨分支搜索代码，确定commit 所属分支
首先需要知道常用的搜索分支的 git 命令为何？

git 查看日志搜索代码的语句
+ git log [分支名称] -S “代码关键字” （搜索指定分支的日志）
+ git log -S “代码关键字” 搜索当前分支的日志
+ git log -p “查看日志修改的详细信息”，然后通过 “/代码关键字” 搜索代码关键字，再通过向上查看此次修改的commit id。

有了 commit id 我们就可以通过 id 获取到当前commit 对应的分之名称
git branch –contains [commit id] （通过此命令查看当前id 所属分支名称）

到此为止我们已经可以搜索某个确定的分之的上的代码了，问题是，如果当前本地和线上分支比较多的情况下，一个一个的去 git log 那就有点费事了。这时候我们要用到一个东西 xargs。
这个东西在linux 或者mac 上都有，可能使用起来命令稍有不同，但是功能是一个样的。xargs 用于将 stdin 中的结果用作下一个命令的参数来进行多个命令的联合使用，类似我们常用的管道符 + grep 的用法。具体的使用方法请参考网上资料，不作过多介绍。
这样我们有了一个新思路，把手动去搜索每个分支的这个过程用一个命令来实现，当前如果用shell 也是可以的，只不过麻烦点，没有命令来的快。
示例：git branch -r | grep lj_ | xargs -n 1 -J % git log % -S “abc” >> result.log
这里以 mac 下搜索远程分支代码为例，去搜索关键字为 abc 的代码为示例。这样就可以得到当前代码对应的 commit id，然后再通过 git branch -r –contains [commit id] 即可得到所查代码所在分支。
以上需求常用在团队开发，团队中队员有事临时不再，却又不知道对方代码所在位置的时候，可以通过一些关键字来确定其代码的分支
```



----

----

恢复单个文件到某一个版本

git reset commit_id -- path_to_file
git checkout -- path_to_file

---

git diff 的简单使用（比较版本区别） - 暗恋桃埖源 - 博客园 - https://www.cnblogs.com/taohuaya/p/11107264.html

---

---

2021-06-03:

把U盘当做远程仓库：

*在U盘目录bash：*

```
mkdir 000_buf_repo && cd 000_buf_repo
```

```
young@DESKTOP-6AN11J2 MINGW64 /g/000_buf_repo
$ git init
Initialized empty Git repository in G:/000_buf_repo/.git/
```

*在PC 硬盘目录bash：*

```
young@DESKTOP-6AN11J2 MINGW64 /f
$ mkdir 000_buf_repo && cd 000_buf_repo

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo
$ git init
Initialized empty Git repository in F:/000_buf_repo/.git/

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (master)
$ git remote add origin G:/000_buf_repo/.git

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (master)
$ git remote -v
origin  G:/000_buf_repo/.git (fetch)
origin  G:/000_buf_repo/.git (push)

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (master)
$ cp ../git_test_dir/002_skybell_analyse/*.md .

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (master)
$ git checkout -b dev
Switched to a new branch 'dev'

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (dev)
$ ls
ap_c_analyse.md     skybell_std.md      sta_analyse.md
skybell_analyse.md  skybell-project.md  wifi_c_analyse.md

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (dev)
$ git add -A

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (dev)
$ git commit -m "init HP_2_U"
[dev (root-commit) 6a9a832] init HP_2_U
 6 files changed, 1180 insertions(+)
 create mode 100644 ap_c_analyse.md
 create mode 100644 skybell-project.md
 create mode 100644 skybell_analyse.md
 create mode 100644 skybell_std.md
 create mode 100644 sta_analyse.md
 create mode 100644 wifi_c_analyse.md

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (dev)
$ git push origin dev
Enumerating objects: 8, done.
Counting objects: 100% (8/8), done.
Delta compression using up to 8 threads
Compressing objects: 100% (8/8), done.
Writing objects: 100% (8/8), 11.79 KiB | 135.00 KiB/s, done.
Total 8 (delta 0), reused 0 (delta 0), pack-reused 0
To G:/000_buf_repo/.git
 * [new branch]      dev -> dev

young@DESKTOP-6AN11J2 MINGW64 /f/000_buf_repo (dev)

```

*在U盘的bash:*

```
young@DESKTOP-6AN11J2 MINGW64 /g/000_buf_repo (master)
$ git checkout dev
Switched to branch 'dev'

young@DESKTOP-6AN11J2 MINGW64 /g/000_buf_repo (dev)
$ git branch -a
* dev

young@DESKTOP-6AN11J2 MINGW64 /g/000_buf_repo (dev)
$ ls
ap_c_analyse.md     skybell_std.md      sta_analyse.md
skybell_analyse.md  skybell-project.md  wifi_c_analyse.md

young@DESKTOP-6AN11J2 MINGW64 /g/000_buf_repo (dev)
$

```

**OK！**

----

---



2021-03-26：

git丢弃、删除已经提交的commit_performance-CSDN博客_git 丢弃commit - https://blog.csdn.net/realDonaldTrump/article/details/85050779

```
git删除中间某个commit：
1.git log获取commit信息
2.git rebase -i (commit-id)
commit-id 为要删除的commit的下一个commit号
3.编辑文件，将要删除的commit之前的单词改为drop
4.保存文件退出大功告成
5.git log查看

git push origin master –force
```



----

```
#先删除已关联的名为origin的远程库:
git remote rm origin
#关联GitHub的远程库,远程仓库名github
git remote add github git@github.com:michaelliao/learngit.git
#查看远程库信息
git remote -v

#拉取
git fetch github master
git rebase github/master

git push github master
```

使用 git rebase 让历史变得清晰_张吉的博客-CSDN博客 - https://blog.csdn.net/zjerryj/article/details/77394585

使用Gitee - 廖雪峰的官方网站 - https://www.liaoxuefeng.com/wiki/896043488029600/1163625339727712

Git重命名仓库、修改远程仓库地址、修改仓库配置_Spring的博客-CSDN博客_git仓库改名 - https://blog.csdn.net/u011884440/article/details/71246572?utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~aggregatepage~first_rank_v2~rank_aggregation-3-71246572.pc_agg_rank_aggregation&utm_term=git+%E6%9C%AC%E5%9C%B0%E4%BB%93%E5%BA%93+%E9%87%8D%E5%91%BD%E5%90%8D&spm=1000.2123.3001.4430

2021-0317:

git 拉取远程分支到本地_carfge的博客-CSDN博客_git拉取远程分支 - https://blog.csdn.net/carfge/article/details/79691360

```
git fetch origin dev（dev为远程仓库的分支名）
git checkout -b dev(本地分支名称) origin/dev(远程分支名称)
```



----

2021-03-03:

git仓库拆分 - 勇~勇 - 博客园 - https://www.cnblogs.com/lyongyong/p/11994938.html

```
git仓库拆分
例如：
# 这就是那个大仓库 big-project
$ git clone git@github.com:tom/big-project.git
$ cd big-project

# 把所有 `codes-eiyo` 目录下的相关提交整理为一个新的分支 eiyo
$ git subtree split -P codes-eiyo -b eiyo

# 另建一个新目录并初始化为 git 仓库
$ mkdir ../eiyo
$ cd ../eiyo
$ git init

# 拉取旧仓库的 eiyo 分支到当前的 master 分支
$ git pull ../big-project eiyo
```





---

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

----

2021-01-17：

```
#git查看远程仓库地址命令
rck@rck:/vtcs/d3tech/skybell_app$ git remote -v
origin	git@gitee.com:skybell/d3tech.git (fetch)
origin	git@gitee.com:skybell/d3tech.git (push)
rck@rck:/vtcs/d3tech/skybell_app$
```

```
#git修改远程仓库关联
git remote rm origin
git remote add origin http://192.168.100.235:9797/john/git_test.git
||
git remote set-url origin http://192.168.100.235:9797/john/git_test.git
git修改远程仓库关联 - 自律西红柿 - 博客园 - https://www.cnblogs.com/sugar-tomato/p/9029205.html
```

