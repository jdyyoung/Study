
20210727：
sed与正则表达式 - Lucky_7 - 博客园 - https://www.cnblogs.com/zd520pyx1314/p/6061339.html
```
#把小于等于5个字母的单词提取出来：
$ sed -n '/^[a-zA-Z]\{1,5\}$/p' 6级单词.txt > less_than_5.txt
#把6个字母的单词提取出来：
$ sed -n '/^[a-zA-Z]\{6\}$/p' 6级单词.txt > only_6.txt
```
---

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

