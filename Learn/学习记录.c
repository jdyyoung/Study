2017-01-19
用 Vi 编译器底行命令模式实现字符串替换，把当前文件中的“ xiaodai”全部
替换成“ banzhang”。（） :%s/xiaodai/banzhang/g

find /home/tarena/workdir/toolchains/opt/S5PV210-crosstools/4.4.6/arm-concenwit-linux-gnueabi/lib/ \( -name "libc-*"-o -name "libc.so*" \) -exec cp -frd {}  lib/  \;

查找 路径下 所有的名字为（libc-的文件或者libc.so文件) 并执行拷贝动作，把找到的内容拷贝到lib/路径下 
注意：空格、转化符，分号

关于tar 打包压缩、解压命令
tar –jxvf busybox-1.21.1.tar.bz2
tar –xvf Kernel_2.6.35.7_CW210_for_Linux_v1.0.tar.gz
tar -xzvf  .tar.gz
tar -zcvf lastmod.tar.gz


tar命令
[root@Linux ~]# tar [-cxtzjvfpPN] 文件与目录 ....
参数：
-c ：建立一个压缩文件的参数指令(create 的意思)；
-x ：解开一个压缩文件的参数指令！
-t ：查看 tarfile 里面的文件！
特别注意，在参数的下达中， c/x/t 仅能存在一个！不可同时存在！
因为不可能同时压缩与解压缩。
-z ：是否同时具有 gzip 的属性？亦即是否需要用 gzip 压缩？
-j ：是否同时具有 bzip2 的属性？亦即是否需要用 bzip2 压缩？
-v ：压缩的过程中显示文件！这个常用，但不建议用在背景执行过程！
-f ：使用档名，请留意，在 f 之后要立即接档名喔！不要再加参数！
　　　例如使用『 tar -zcvfP tfile sfile』就是错误的写法，要写成
　　　『 tar -zcvPf tfile sfile』才对喔！
-p ：使用原文件的原来属性（属性不会依据使用者而变）
-P ：可以使用绝对路径来压缩！
-N ：比后面接的日期(yyyy/mm/dd)还要新的才会被打包进新建的文件中！
--exclude FILE：在压缩的过程中，不要将 FILE 打包！

在Linux操作系统上编写Shell脚本时候，我们是在变量的前面使用$符号来获取该变量的值，通常在脚本中使用”$param”这种带双引号的格式，但也有出现使用'$param'这种带引号的使用的场景，首先大家看一段例子：
复制代码 代码如下:

[root@linux ~]# name=TekTea
[root@linux ~]# echo $name
TekTea
[root@linux ~]# sayhello=”Hello $name”
[root@linux ~]# echo $sayhello
Hello TekTea
[root@linux ~]# sayhello='Hello $name'
[root@linux ~]# echo $sayhello
Hello $name

发现了吗？没错！使用了单引号的时候，那 $name将失去原有的变量内容，仅为一般字符的显示型态而已！这里必需要特别小心在意！
所以，单引号与双引号的最大不同在于双引号仍然可以保有变量的内容，但单引号内仅能是一般字符，而不会有特殊符号，我们也可以这里理解：'$sayhello' 与 “\$sayhello”是相同的。
在Shell脚本开发过程中，针对变量的单引号(‘)和双引号(“)都是有应用场景的，根据我的经验总结起来就是：
1. 获取变量值的时候就使用双引号(“)，这是基本用法。
2. 如果是在脚本中要编写一个新的脚本，同时这个新的脚本中还需要包含变量，你就可以选择使用单引号(‘)了，特别是生成一个临时的expect脚本时，单引号(‘)就会经常使用到的。

管道（|）
重定向 >      >>
backtick反短斜线

替代命令：sed  和 tr
vi中的替代格式 :%s/ str1/str2/ 或者:s/str1/str2/

流程控制：
1.if
"if" 表达式 如果条件为真则执行then后面的部分：
if ....; then
....
elif ....; then
....
else
....
fi

通常用" [ ] "来表示条件 测试。注意这里的空格很重要。要确保方括号的空格。
[ -f "somefile" ] ：判断是否是一个文件
[ -x "/bin/ls" ] ：判断/bin/ls是否存在并有可执行权限
[ -n "$var" ] ：判断$var变量是否有值
[ "$a" = "$b" ] ：判断$a和$b是否相等


2017-1-20
vi 取消行号  set nonu

 
ssh -T git@github.com host key verification failed. 解决办法
这里会提示输入（yes/no）,不能直接回车，输入yes就好了！哈哈
https://zhidao.baidu.com/question/488954251016978292.html

2017-02-12
怎么查当前测试环境：
包括：系统版本，bash版本，make版本
输入"uname -a ",可显示电脑以及操作系统的相关信息。
输入"cat /proc/version",说明正在运行的内核版本。
输入"cat /etc/issue", 显示的是发行版本信息

查看内核版本及平台 uname -r -m
查看发行版版本 lsb_release -a

查看当前系统支持的Shell，使用cat、head、more等命令查看/etc/shells的内容即可。
cat /etc/shells

显示shell的版本号 ：bash  --version
显示make的版本号：make  --version

.tar.xz
# xz -d ***.tar.xz  //先解压xz 
# tar -xvf  ***.tar //再解压tar 

对内核调试，打印信息的添加与解读，
例如：new high speed USB device using s5p-ehci and address 3
这句是那打出来，追踪




