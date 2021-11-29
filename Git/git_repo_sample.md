### 																					Git Repo Sample

----

1.创建4个本地仓库：1个manifest仓库，和manifest管理的3个仓库，manifest仓库建立manifest.xml 文件，文件目录如下：

```
lindenis@ubuntu:~/git-repo-test1$ tree -L 2
.
├── Hello
│   ├── hello.c
│   ├── Make.mk
│   └── ProjectConfig.mk
├── Manifest
│   └── git-repo-test.xml
├── Nihao
│   └── Nihao.c
└── World
    └── world.c

4 directories, 6 files
lindenis@ubuntu:~/git-repo-test1$
```

2.在gitee 创建4个远程仓库

![](.\md_att\git-manifest.png)

![](.\md_att\git-hello.png)

![](.\md_att\git-nihao.png)



![](.\md_att\git-world.png)

3.把本地的4个仓库提交本地，并关联且提交到远程。

```
mkdir git-repo-test1 git-repo-test2
cd git-repo-test1
mkdir Hello Manifest Nihao World
cd Hello
touch hello.c Make.mk ProjectConfig.mk
git init
git add -A
git commit -m "init Hello"
git remote set-url origin https://gitee.com/jdyyoung/git-repo-hello.git
git push -u origin master

cd ../Nihao
touch Nihao.c
git init
git add -A
git commit -m "init Nihao"
git remote set-url origin https://gitee.com/jdyyoung/git-repo-nihao.git
git push -u origin master

cd ../World
touch world.c
git init
git add -A
git commit -m "init World"
git remote set-url origin https://gitee.com/jdyyoung/git-repo-world.git
git push -u origin master

cd ../Manifest
vim git-repo-test.xml
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
    <remote fetch="https://gitee.com/jdyyoung" name="origin"/>
    <default remote="origin" revision="master"/>
    <project name="git-repo-hello" path="Hello">
        <copyfile dest="Make.mk" src="Make.mk"/>
        <copyfile dest="ProjectConfig.mk" src="ProjectConfig.mk"/>
    </project>
    <project name="git-repo-world" path="World"/>
    <project name="git-repo-nihao" path="Nihao"/>
</manifest>
```

注意：project 里的name的值是创建仓库xxxx.git的xxxx!

```
git add -A
git commit -m "init manifest"
git remote set-url origin https://gitee.com/jdyyoung/manifest.git
git push -u origin master
```

4.验证到另一个文件使用repo命令验证

```
cd ../../git-repo-test2
repo init -u https://gitee.com/jdyyoung/manifest.git -b master -m git-repo-test.xml
repo sync
```

```
lindenis@ubuntu:~/git-repo-test2$ tree -L 2
.
├── Hello
│   ├── hello.c
│   ├── Make.mk
│   └── ProjectConfig.mk
├── Make.mk
├── Nihao
│   └── Nihao.c
├── ProjectConfig.mk
└── World
    └── world.c

3 directories, 7 files
lindenis@ubuntu:~/git-repo-test2$
```

---

参考：怎么针对自己项目工程建立Repo管理多个git仓库? - 知乎 - https://www.zhihu.com/question/41440585?sort=created