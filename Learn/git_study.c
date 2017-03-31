2017-01-23
初始化一个Git仓库，使用git init命令。

添加文件到Git仓库，分两步：

第一步，使用命令git add <file>，注意，可反复多次使用，添加多个文件；
//出现个警告：http://blog.csdn.net/unityoxb/article/details/20768687

第二步，使用命令git commit，完成。


要随时掌握工作区的状态，使用git status命令。

如果git status告诉你有文件被修改过，用git diff可以查看修改内容。


HEAD指向的版本就是当前版本，因此，Git允许我们在版本的历史之间穿梭，使用命令git reset --hard commit_id。
在Git中，用HEAD表示当前版本，上一个版本就是HEAD^，上上一个版本就是HEAD^^，当然往上100个版本写100个^比较容易数不过来，所以写成HEAD~100

穿梭前，用git log可以查看提交历史，以便确定要回退到哪个版本。

要重返未来，用git reflog查看命令历史，以便确定要回到未来的哪个版本

版本库  工作区  暂存区（stage）  master  HEAD

理解了Git是如何跟踪修改的，每次修改，如果不add到暂存区，那就不会加入到commit中。

2013-01-24
分支管理
新功能  50%   进度
1、创建与合并分支
分支线，时间线，主分支master    HEAD指针
提交点commit  
创建并切换到创建的分支：git checkout -b dev
创建分支：git branch  dev
切换分支：git checkout  dev
查看当前分支：git branch
*  dev  *表示当前分支

然后添加提交：
git add readme.txt 
git commit -m "branch test"

合并到master分支：
git merge dev
Fast-forward信息，Git告诉我们，这次合并是“快进模式”

合并完成后删除dev分支：
git branch -d dev

2、解决冲突
选择master分支重新打开，修改提交，就能解决？？
还是可以切换其他分支？
当Git无法自动合并分支时，就必须首先解决冲突。解决冲突后，再提交，合并完成。
用git log --graph命令可以看到分支合并图。

 git log --graph --pretty=oneline --abbrev-commit

3、分支管理策略
git merge --no-ff -m "merge with no-ff" dev
--no-ff参数，表示禁用Fast forward
合并要创建一个新的commit，所以加上-m参数，把commit描述写进去

4、Bug分支
Git提供了一个stash功能，可以把当前工作现场“储藏”起来，等以后恢复现场后继续工作
git stash


首先确定要在哪个分支上修复bug，假定需要在master分支上修复，就从master创建临时分支
git checkout master
git checkout -b issue-101

git add readme.txt 
git commit -m "fix bug 101"

git checkout master
git branch -d issue-101

git checkout dev

查看工作现场
git stash list

git stash apply恢复，但是恢复后，stash内容并不删除，你需要用git stash drop来删除；

另一种方式是用git stash pop，恢复的同时把stash内容也删了


开发一个新feature，最好新建一个分支；

如果要丢弃一个没有被合并过的分支，可以通过git branch -D <name>强行删除。
