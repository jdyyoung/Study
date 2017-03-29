http://www.cnblogs.com/xuejie/archive/2013/02/17/2914310.html
2017-1-20
!!!特别注意shell脚本的空格！！！
变量赋值的等号左右没空格，没空格，没空格！
双引号围起来字符串，引号与字符串之间没空格，没空格，没空格！

1. 等号赋值两边不能有空格
2. 命令与选项之间需要空格
3. 管道两边空格可有可无

[  ]:有空格
{ }：有空格


Shell 编程：Bash空格的那点事_linux shell_脚本之家
http://www.jb51.net/article/60328.htm

linux shell的here document用法(cat << EOF)
linux shell的here document用法(cat << EOF) - Linux操作系统：Ubuntu_Centos_Debian - 红黑联盟
http://www.2cto.com/os/201307/231010.html

2017-1-22
//  ;之后可以没空格

关于等号“=”：用来表示变量赋值则等号两边没空格；
用来表示相等时则等号两边有空格

在case ... in
...) do something here ;;
esac
"$1: zip archive"*) unzip "$1" ;;
冒号前没空格，冒号后有空格
右括号之前最好是不要有空格，否则影响运行速度

2017-1-23
特殊变量
$#表示包含参数的个数
$*$* 是以一个单字符串显示所有向脚本传递的参数，与位置变量不同，参数可超过9个
$? 是显示最后命令的退出状态，0表示没有错误，其他表示有错误
$@是当前所有的入参 
$$ 是脚本运行的当前进程ID号
$1
$0: 表示当前脚本的名称

shell 之shift
shell 之let
sed命令
awk命令

[ $# -lt 3 ]
-lt 即 less than,小于
 http://www.dutycode.com/post-46.html

chmod u+x hello
chmod是权限管理命令change the permissions mode of a file的缩写。。
u代表所有者，x代表执行权限。 + 表示增加权限。





