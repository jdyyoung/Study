

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
```

---



