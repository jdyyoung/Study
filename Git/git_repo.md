```
git clone http://mirrors.ustc.edu.cn/aosp/git-repo.git ~/.bin/repo
echo "export PATH=~/.bin/repo:$PATH" >> ~/.bashrc
source ~/.bashrc
```

注:需要python3.6 以上的版本



```
repo init -u https://gitee.com/lindenis/manifest.git -b master -m lindenis-v536.xml
repo sync
repo start lindenis-v536 --all
```

```
repo init -u URL [OPTIONS]
-u：指定一个URL，其连接到一个manifest仓库。
-m：在manifest仓库中选择一个 NAME.xml 文件。
-b：选择一个manifest仓库中一个特殊的分支。
注：
· 如果不提供 -b REVISION 或者 –manifest-branch=REVISION参数，则检出 manifest Git 库的 master 分支。
· 如果不提供 -m NAME.xml 或者 –manifest-name=NAME.xml 参数，则使用缺省值 default.xml
```



怎么针对自己项目工程建立Repo管理多个git仓库? - 知乎 - https://www.zhihu.com/question/41440585?sort=created

作者：Elpie Kay

通常repo通过以下两个命令来下载代码：

```shell
repo init -u git://xxx.xxx.xxx/platform/manifest  -b $branch -m $manifest --reference=$mirror
repo sync
```

repo init命令里的repo是指repo launcher，是个可执行文件。

它会在当前目录下创建.repo文件夹，然后克隆一份repo的源代码到.repo/repo下，里面存放了其他repo子命令的源码。接着将git://[xxx.xxx.xxx/platform/manifest]从中心服务器上clone到.repo里生成manifests和manifests.git。platform/manifest是一个git仓库，用来存放manifest文件。

实际上这里可以用任意仓库，只要-m指定的manifest文件在这个仓库里的分支（通过-b指定）最新版本里能找到。它的每个分支最好有一个名为default.xml的文件，因为-m $manifest这个参数是可以省略的，省略的话就相当于-m default.xml。

之后repo sync会去解析-m指定的manifest，根据里面的配置去clone组成整个项目的各个git。--reference参数是可选的，后面接的是镜像代码仓的路径。镜像代码仓相当于是本地的缓存，用了镜像仓库的话，下载仓库数据时会先去镜像仓库取，缺失的部分才去服务器上下载。

下面举个实际的例子，看下manifest文件是什么样的。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
	<remote fetch="git://http://xxx.xxx.xxx" name="origin" review="http://xxx.xxx.xxx:8080"/>
    <default remote="origin" revision="main_dev"/>
    <project name="platform/mediatek/modem/buildscript" path="buildscript">
        <copyfile dest="m" src="m"/>
        <copyfile dest="Make.bat" src="Make.bat"/>
        <copyfile dest="make.pl" src="make.pl"/>
        <copyfile dest="make.sh" src="make.sh"/>
        <copyfile dest="M.bat" src="M.bat"/>
        <copyfile dest="BuildMMAA.pl" src="BuildMMAA.pl"/>
    </project>
    <project name="platform/mediatek/modem/driver" path="driver"revision="352b1d8fb184a0169c6a17b5f6a858730fc51966"upstream="main_release" />
    <project name="platform/mediatek/modem/make" path="make"revision="main_release" />
    <project name="platform/mediatek/modem/middleware" path="middleware"revision="tag_20160316" />
    <project name="platform/mediatek/modem/modem" path="modem" />
    <project name="platform/mediatek/modem/mtk_rel" path="mtk_rel"/>
    <project name="platform/mediatek/modem/custom" path="custom"/>
    <project name="platform/mediatek/modem/tools" path="tools"/>
    <project name="platform/mediatek/modem/interface" path="interface"/>
    <project name="platform/mediatek/modem/service" path="service"/>
</manifest>
```

先看remote部分，fetch是服务器地址，可以用“..”来代替；name是默认的remote name，就是git push origin里面那个origin；review是gerrit地址（这点是我猜的，不确定）。

然后是default部分，里面定义了默认的remote和revision。project部分不单独指定remote或者revision的话就使用default部分的。


下面是project部分，这部分定义了整包代码由哪些git组成。name是git在服务器上的相对路径，path是把代码下载下来后在本地的相对路径，path是可以省略的，如果省略那么就认为path和name一样，revision是指下载下来的代码要checkout到哪个revision上，这里的revision可以是commit id、branch name、tag name，反正本质上都是commit id。利用branch name的特性，revision写branch name的话，那总可以下载到并且checkout出该branch上最新的代码，default.xml中通常用branch name做revision。而commit id和tag name就是固定的某个commit了。如果revision用commit id的话，那后面必须跟上upstream，upstream的值是个branch name。revision部分如果省略的话，就等于使用default部分定义的revision。project还有个group属性，用于将部分仓库归为一组，方便repo命令批量操作，譬如只下载其中某个或者某些group的仓库。还有copyfile部分，这是用来处理一些无法或者不方便直接归类到某个git中去的文件的，譬如项目代码根目录下的几个零散文件，这几个文件所在目录做成一个git的话，等于把整包代码做成了一个git。虽然git是可以嵌套的，但这样不方便。repo的做法是将这些文件先放到某个已知git中去，本例就是先放到project buildscript里面去。当然，放到某个git里去的动作是由你来做的。然后下载完后再自动将它们copy到应该在的地方，本例就是根目录下。dest是相对于根目录的路径，src是相对于project path的路径。

这样一个manifest就可以定义一个完整的项目代码了。里面的值大致用于以下命令：

```shell
git clone $fetch/$name -b $upstream -- $path
git checkout $revision
cp $path/$src $dest
```


通过自由组合project和revision，就可以做出各种搭配的项目代码了。

常用的几个repo 命令：

```
repo sync -j6 -m $manifest
```

sync时可以重新指定一个manifest，如果这个manifest在.repo/manifests里面，那么只要填它相对于.repo/manifests的路径，否则要填它的全路径。

```
repo manifest -r -o $name
```


根据当前代码生成一份manifest，这通常用于记录当前所有git的快照，这样以后或者别人、编译服务器可以根据这个manifest下载到那份代码。

```
repo forall -p -c '$command'
```


在每个project下执行一次command，-p会打印出当前在哪个project下，-p可选的。command的内容根据需求来写。


repo有个常用的环境变量$REPO_PROJECT，值是当前project的name。

譬如给每个git添加一个remote server1，可以用

```
repo forall -p -c 'git remote add server1 git://[xxx.xxx.xxx/]$REPO_PROJECT'
```

关于**manifest文件的格式**，在repo init命令成功执行后，可以在代码根目录下的**.repo/repo/docs**下看到文档说明。

---

google repo本来是用于管理android系统源码的，这份源码不同的版本一般都由450个左右的git组成。

现在将问题简化一下，假如有一份代码，目录结构如下:

```
myApp
├── Hello
│   ├── hello.java
│   ├── Make.mk
│   └── ProjectConfig.mk
├── Make.mk
├── Nihao
│   └── nihao.java
├── ProjectConfig.mk
└── World
    └── world.java
```

将这份代码拆分成三个git，Hello, World和Nihao。现在要用repo将它们组织起来，组成一份完整的代码myApp。这3个git都有两个branch，master和release。下面是给master用的default.xml。服务器上还有一个git叫platform/manifest，对应的也建了2个branch，分别存放master和release相关的manifest。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
    <remote fetch="git://http://xxx.xxx.xxx" name="origin" review="http://xxx.xxx.xxx:8080"/>
    <default remote="origin" revision="master"/>
    <project name="platform/Hello" path="Hello">
        <copyfile dest="Make.mk" src="Make.mk"/>
        <copyfile dest="ProjectConfig.mk" src="ProjectConfig.mk"/>
    </project>
    <project name="platform/World" path="World"/>
    <project name="platform/Nihao" path="Nihao"/>
</manifest>
```

这份manifest描述了以下信息：这份代码由3个git（里面的3个project）组成；

Hello/底下的2个mk文件在repo sync后会被拷贝到代码根目录下；

这3个git用的都是default revision中描述的分支master。

---

```xml
<?xml version="1.0" encoding="UTF-8"?>
<manifest>
	<remote name="origin" fetch="https://gitee.com/lindenis" />
	<default revision="master" remote="origin" />
	<project path="build" name="lindenis-v536-build"  >
		<copyfile src="top_main.mk" dest="Makefile" />
		<copyfile src="rules.mk" dest="rules.mk" />
	</project>
	<project path="config" name="lindenis-v536-config"  >
		<copyfile src="top_config.in" dest="Config.in" />
	</project>
	<project path="device" name="lindenis-v536-device"  />
	<project path="lichee/brandy" name="lindenis-v536-lichee-brandy"  />
	<project path="lichee/linux-4.9" name="lindenis-v536-lichee-linux-4.9"  />
	<project path="package" name="lindenis-v536-package"  />
	<project path="scripts" name="lindenis-v536-scripts"  />
	<project path="softwinner" name="lindenis-v536-softwinner"  />
	<project path="target" name="lindenis-v536-target"  />
	<project path="toolchain" name="lindenis-v536-toolchain"  />
	<project path="prebuilt" name="lindenis-v536-prebuilt"  />
	<project path="tools" name="lindenis-v536-tools"  />
</manifest>
```



```
lindenis@ubuntu:~/Lindenis-V536-SDK$ ls .repo/
manifests  manifests.git  manifest.xml  project.list  project-objects  projects  repo
lindenis@ubuntu:~/Lindenis-V536-SDK$
```



```
lindenis@ubuntu:~/Lindenis-V536-SDK/.repo$ tree -L 2
.
├── manifests
│   └── lindenis-v536.xml
├── manifests.git
│   ├── branches
│   ├── config
│   ├── description
│   ├── FETCH_HEAD
│   ├── HEAD
│   ├── hooks
│   ├── info
│   ├── logs
│   ├── objects
│   ├── packed-refs
│   ├── refs
│   ├── rr-cache
│   └── svn
├── manifest.xml -> manifests/lindenis-v536.xml
├── project.list
├── project-objects
│   ├── lindenis-v536-build.git
│   ├── lindenis-v536-config.git
│   ├── lindenis-v536-device.git
│   ├── lindenis-v536-lichee-brandy.git
│   ├── lindenis-v536-lichee-linux-4.9.git
│   ├── lindenis-v536-package.git
│   ├── lindenis-v536-prebuilt.git
│   ├── lindenis-v536-scripts.git
│   ├── lindenis-v536-softwinner.git
│   ├── lindenis-v536-target.git
│   ├── lindenis-v536-toolchain.git
│   └── lindenis-v536-tools.git
├── projects
│   ├── build.git
│   ├── config.git
│   ├── device.git
│   ├── lichee
│   ├── package.git
│   ├── prebuilt.git
│   ├── scripts.git
│   ├── softwinner.git
│   ├── target.git
│   ├── toolchain.git
│   └── tools.git
└── repo
    ├── color.py
    ├── command.py
    ├── docs
    ├── editor.py
    ├── error.py
    ├── event_log.py
    ├── git_command.py
    ├── git_config.py
    ├── gitc_utils.py
    ├── git_refs.py
    ├── git_ssh
    ├── hooks
    ├── LICENSE
    ├── main.py
    ├── MANIFEST.in
    ├── manifest_xml.py
    ├── pager.py
    ├── platform_utils.py
    ├── platform_utils_win32.py
    ├── progress.py
    ├── project.py
    ├── pyversion.py
    ├── README.md
    ├── repo
    ├── repo_trace.py
    ├── run_tests
    ├── setup.py
    ├── subcmds
    ├── SUBMITTING_PATCHES.md
    ├── tests
    ├── tox.ini
    └── wrapper.py

```



```
├── build
├── config
├── Config.in
├── dl
├── lichee
├── logs
├── Makefile
├── package
├── prebuilt
├── rules.mk
├── scripts
├── softwinner
├── target
├── toolchain
└── tools
```



---



简单学习：repo入门 - 一叶梧桐 - 博客园 - https://www.cnblogs.com/bill-technology/p/4130851.html

 repo学习总结_张许平的博客-CSDN博客_repo start - https://blog.csdn.net/salmon_zhang/article/details/79180075

Repo的理解及用法小结(1)_煊琦的CSDN小窝-CSDN博客 - https://blog.csdn.net/u012842255/article/details/68941379

repo系列-关于repo的理解-vaqeteart-ChinaUnix博客 - http://blog.chinaunix.net/uid-9525959-id-4534319.html

linux建立多个repo,repo管理多个git仓库_dtffeng的博客-CSDN博客 - https://blog.csdn.net/weixin_30578645/article/details/116900333

Git多项目管理 - 简书 - https://www.jianshu.com/p/284ded3d191b

repo manifest - 简书 - https://www.jianshu.com/p/c6be01f23ebd

-----

```
lindenis@ubuntu:~/git-repo-test2$ repo help --all
usage: repo COMMAND [ARGS]
The complete list of recognized repo commands are:
  abandon        Permanently abandon a development branch
  branch         View current topic branches
  branches       View current topic branches
  checkout       Checkout a branch for development
  cherry-pick    Cherry-pick a change.
  diff           Show changes between commit and working tree
  diffmanifests  Manifest diff utility
  download       Download and checkout a change
  forall         Run a shell command in each project
  gitc-delete    Delete a GITC Client.
  gitc-init      Initialize a GITC Client.
  grep           Print lines matching a pattern
  help           Display detailed help on a command
  info           Get info on the manifest branch, current branch or unmerged branches
  init           Initialize repo in the current directory
  list           List projects and their associated directories
  manifest       Manifest inspection utility
  overview       Display overview of unmerged project branches
  prune          Prune (delete) already merged topics
  rebase         Rebase local branches on upstream branch
  selfupdate     Update repo to the latest version
  smartsync      Update working tree to the latest known good revision
  stage          Stage file(s) for commit
  start          Start a new branch for development
  status         Show the working tree status
  sync           Update working tree to the latest revision
  upload         Upload changes for code review
  version        Display the version of repo
See 'repo help <command>' for more information on a specific command.
lindenis@ubuntu:~/git-repo-test2$
```


repo start <topic_name>
　　开启一个新的主题，其实就是每个Project都新建一个分支。
repo init -u <url> [OPTIONS]
　　在当前目录下初始化repo，会在当前目录生生成一个.repo目录，像Git Project下的.git一样，-u指定url，可以加参数-m指定manifest文件，默认是default.xml，.repo/manifests保存manifest文件。.repo/projects下有所有的project的数据信息，repo是一系列git project的集合，每个git project下的.git目录中的refs等目录都是链接到.repo/manifests下的。
repo manifest
　　可以根据当前各Project的版本信息生成一个manifest文件
repo sync [PROJECT1...PROJECTN]
　　同步Code。
repo status
　　查看本地所有Project的修改，在每个修改的文件前有两个字符，第一个字符表示暂存区的状态。
-
no change
same in HEAD and index
A
added
not in HEAD, in index
M
modified
in HEAD, modified in index
D
deleted
in HEAD, not in index
R
renamed
not in HEAD, path changed in index
C
copied
not in HEAD, copied from another in index
T
mode changed
same content in HEAD and index, mode changed
U
unmerged
conflict between HEAD and index; resolution required
　　每二个字符表示工作区的状态 
letter
meaning
description
-
new/unknown
not in index, in work tree
m
modified
in index, in work tree, modified
d
deleted
in index, not in work tree

repo prune <topic> 
　　删除已经merge的分支
repo abandon <topic>
　　删除分支，无论是否merged
repo branch或repo branches
　　查看所有分支
repo diff
　　查看修改
repo upload
　　上传本地提交至服务器
repo forall [PROJECT_LIST]-c COMMAND
　　对指定的Project列表或所有Project执行命令COMMAND，加上-p参数可打印出Project的路径。
repo forall -c 'git reset --hard HEAD;git clean -df;git rebase --abort'
　　这个命令可以撤销整个工程的本地修改。