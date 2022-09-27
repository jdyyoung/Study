







#2022-09-19

Shell获取字符串长度的多种方法总结 - 51core - 博客园 - https://www.cnblogs.com/51core/articles/15661987.html

---

#2022-02-17 

shell 脚本中 echo 显示内容带颜色显示，格式如下：

`echo -e "\033[字背景颜色;字体颜色;字体属性m 需要输出的内容 \033[0m"`
\033 转义起始符，定义一个转义序列，可以使用 \e 或 \E 代替。
[ 表示开始定义颜色。
字背景颜色 范围 40-47 。
字体颜色 范围 30-37 。
m 转义终止符，表示颜色定义完毕。
再次使用 \033[ ，表示再次开启颜色定义，0 表示颜色定义结束，所以 \033[0m 的作用是恢复之前的配色方案。
字体颜色
字体颜色：30-37

默认=0
黑色=30
红色=31
绿色=32
黄色=33
蓝色=34
紫色=35
天蓝色（青色）=36
白色=37

---
```
# echo -e "\e[30m 黑色 \e[0m"
# echo -e "\e[31m 红色 \e[0m"
# echo -e "\e[32m 绿色 \e[0m"
# echo -e "\e[33m 黄色 \e[0m"
# echo -e "\e[34m 蓝色 \e[0m"
# echo -e "\e[35m 紫色 \e[0m"
# echo -e "\e[36m 青色 \e[0m"
# echo -e "\e[37m 白色 \e[0m"
```

---

#2022-01-13

正则表达式/(^\s*)|(\s*$)/g意思
包含以空格、回车符等字符开头 或者 空格、回车符等字符结尾 的字符串，可过滤出所有空格、回车符的字符。

```shell
#选出开头为CC 的行，CC前面可以有包含以空格、回车符等字符开头
grep -E '^\s*CC' ${FILE_IN}
```

grep -E 选项可以用来扩展选项为正则表达式。

grep -v、-e、-E_shadow_zed的博客-CSDN博客_grep-e - https://blog.csdn.net/shadow_zed/article/details/93888685

#20211221

```
1. $$
Shell本身的PID（ProcessID）
2. $!
Shell最后运行的后台Process的PID
3. $?
最后运行的命令的结束代码（返回值）
4. $-
使用Set命令设定的Flag一览
5. $*
所有参数列表。如"$*"用「"」括起来的情况、以"$1 $2 … $n"的形式输出所有参数。
6. $@
所有参数列表。如"$@"用「"」括起来的情况、以"$1" "$2" … "$n" 的形式输出所有参数。
7. $#
添加到Shell的参数个数
8. $0
Shell本身的文件名
9.$1～$n
添加到Shell的各参数值。$1是第1参数、$2是第2参数…。
```

Shell脚本中$0、$?、$!、$、$*、$#、$@ - 张大猛 - 博客园 - https://www.cnblogs.com/zhangjiansheng/p/8318042.html

#20210817

shell中的字母大小写转换_zzxuu的博客-CSDN博客_shell 大写转小写 - https://blog.csdn.net/enenand/article/details/78550678

#20210811

Shell脚本8种字符串截取方法总结 - 杨哥哥 - 博客园 - https://www.cnblogs.com/youngerger/p/8433118.html

```
Linux 的字符串截取很有用。有八种方法。
假设有变量 var=http://www.aaa.com/123.htm.
1. # 号截取，删除左边字符，保留右边字符。
echo ${var#*//}
其中 var 是变量名，# 号是运算符，*// 表示从左边开始删除第一个 // 号及左边的所有字符
即删除 http://
结果是 ：www.aaa.com/123.htm

2. ## 号截取，删除左边字符，保留右边字符。
echo ${var##*/}
##*/ 表示从左边开始删除最后（最右边）一个 / 号及左边的所有字符
即删除 http://www.aaa.com/
结果是 123.htm

3. %号截取，删除右边字符，保留左边字符
echo ${var%/*}
%/* 表示从右边开始，删除第一个 / 号及右边的字符
结果是：http://www.aaa.com

4. %% 号截取，删除右边字符，保留左边字符
echo ${var%%/*}
%%/* 表示从右边开始，删除最后（最左边）一个 / 号及右边的字符
结果是：http:

5. 从左边第几个字符开始，及字符的个数
echo ${var:0:5}
其中的 0 表示左边第一个字符开始，5 表示字符的总个数。
结果是：http:

6. 从左边第几个字符开始，一直到结束。
echo ${var:7}
其中的 7 表示左边第8个字符开始，一直到结束。
结果是 ：www.aaa.com/123.htm

7. 从右边第几个字符开始，及字符的个数
echo ${var:0-7:3}
其中的 0-7 表示右边算起第七个字符开始，3 表示字符的个数。
结果是：123

8. 从右边第几个字符开始，一直到结束。
echo ${var:0-7}
表示从右边第七个字符开始，一直到结束。
结果是：123.htm

注：（左边的第一个字符是用 0 表示，右边的第一个字符用 0-1 表示）
```

#20210714:

```shell
#一行命令实现删除当前路径下大于2个文件时最老的文件
[ `ls | wc -l` -gt  2 ] && rm `ls -rt | head -1`
```



---

```
#提取第二列
cat tmp.file | awk '{print $2}' > tmp1.ok

#所有行开头增加{
sed -i 's/^/{/g' countryMap.txt
#所有行尾增加}
sed -i 's/$/}/g' countryMap.txt 
```



---

shell指令的学习：

找出当前路径下所有空文件夹：

```
find . -type d -empty
```

---

Linux patch命令 | 菜鸟教程 - https://www.runoob.com/linux/linux-comm-patch.html

把audio_fw.patch 打到源文件sound/soc/vatics/giu-acdc.c

```
patch -p0 sound/soc/vatics/giu-acdc.c audio_fw.patch
```

---

在Shell里面判断字符串是否为空_gujing001的专栏-CSDN博客_shell 判断字符串是否为空 - https://blog.csdn.net/gujing001/article/details/7110268

详解shell中的几种标准输出重定向方式_yuki5233的博客-CSDN博客_标准输出重定向 - https://blog.csdn.net/yuki5233/article/details/85091314

(2条消息) Shell中ASCII值和字符之间的转换_Justlinux2010的专栏-CSDN博客 - https://blog.csdn.net/justlinux2010/article/details/8019990

(2条消息) linux shell中实现字符串反转的几种简单方法_bitcarmanlee的博客-CSDN博客 - https://blog.csdn.net/bitcarmanlee/article/details/78919114

---

```shell
foo=string

for (( i=0; i<${#foo}; i++ )); do

  echo "${foo:$i:1}"

done

${#foo}扩展到的长度foo。${foo:$i:1}从$i长度1的位置开始扩展到子字符串。
```



```
7040:~$ echo "123456" | rev
654321
```



```
sed  指令用双引号“”替换单引号，可引入变量，如果变量值有特殊字符如反斜线，分隔符可用@，
在用openssl 生成密码可现
```

(2条消息) sed: bad option in substitution expression_飞呀飞呀菲-CSDN博客 - https://blog.csdn.net/qq_36288184/article/details/82418649

在linux中install命令和cp命令的区别_bugouyonggan的专栏-CSDN博客 - https://blog.csdn.net/bugouyonggan/article/details/11962201?utm_medium=distribute.pc_aggpage_search_result.none-task-blog-2~all~first_rank_v2~rank_v25-6-11962201.nonecase&utm_term=cp%E5%91%BD%E4%BB%A4%20makefile%E4%B8%AD%E5%A6%82%E4%BD%95%E4%BD%BF%E7%94%A8

linux shell脚本检测目录下的文件是否有更新_diyun的博客-CSDN博客 - https://blog.csdn.net/mao_hui_fei/article/details/104294315

shell命令 - zip/unzip_linux从业者-CSDN博客 - https://blog.csdn.net/cc289123557/article/details/78719800

```
stat file
```

