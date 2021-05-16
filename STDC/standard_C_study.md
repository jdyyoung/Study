20210906：

C语言再学习 -- 关键字volatile_不积跬步，无以至千里-CSDN博客_c语言 volatile - https://blog.csdn.net/qq_29350001/article/details/54024070

---

---

2021-0515：

字符串删掉某个字符:

c语言删除某个字符串中的某一个字符_Janik的博客-CSDN博客 - https://blog.csdn.net/qq_39769995/article/details/83211154

```c
void delete_char(char str[],char target){
	int i,j;
	for(i=j=0;str[i]!='\0';i++){
		if(str[i]!=target){
			str[j++]=str[i];
		}
	}
	str[j]='\0';
}
```



2021-02-26:

可变参数列表与printf()函数的实现 - chenguan - 博客园 - https://www.cnblogs.com/guanguangreat/p/6119278.html
对C的printf函数的可变长参数实现的分析_c/c++_shexujia的专栏-CSDN博客 - https://blog.csdn.net/shexujia/article/details/19125981
C语言中的可变参数函数 三个点“…” - sticker - 博客园 - https://www.cnblogs.com/sticker/p/9988237.html
C语言 ## __VA_ARGS__ 宏 - AlexShi - 博客园 - https://www.cnblogs.com/alexshi/archive/2012/03/09/2388453.html
宏定义中的##操作符和... and _ _VA_ARGS_ _ - DoubleLi - 博客园 - https://www.cnblogs.com/lidabo/p/4570949.html
va在这里是variable-argument(可变参数)的意思。
原理：利用函数形参的栈地址指针，所以必须至少有一个参数，为形参的首地址

先看看stdarg.h文件下这三个宏的实现：
第一种：
typedef char  *va_list;
#define va_start(ap,v)   ap = (va_list)&v + sizeof(v)  //v:第一个形参
#define va_arg(ap,t)      (((t *)ap)++[0])   //++:注意指针越界访问问题，t类型

#define va_end(ap)

(1) va_list：    定义一个va_list型的变量ap，也就是char *；
(2) va_start：	根据到可变参数表的首地址，将第二参数的地址赋给指针ap，也就是ap指向第二个参数
(3) va_arg：   获取当前ap所指向的可变参数的值，并将指针ap指向下一个可变参数。注意，该宏的第二个参数为类型；
(4) va_end：   结束可变参数的获取。

可变参数列表与printf()函数的实现 - chenguan - 博客园 - https://www.cnblogs.com/guanguangreat/p/6119278.html

：需要注意边界终止问题

1.以va_list类型声明一个变量ap

2.初始化指针变量ap 指向第二形参，va_start(ap,v) ；

3.可用va_arg(ap,t) ：返回指针变量ap 所指向位置的值，并修改指针变量ap，ap 重新指向下一个形参， t 是指数据类型，如char,int,short。。。

4.va_end(ap)：   结束可变参数的获取

**注意：第一个形参指定可变参数的界限，可变参数的限定；第二点可变形参的类型最好相同，不然需要在第一个参数有格式说明。**

---
>>>>>>> Stashed changes

如何在引号里面引用宏_水晶鞋-CSDN博客_sas引号里调用宏变量 - https://blog.csdn.net/mos2046/article/details/25741223

```
#define SKYBELL_FWVER	8850
#define __SYKBELL_VERSION(ver) "SKYBELL_APP_VER:"#ver""
#define SYKBELL_VERSION(ver)  __SYKBELL_VERSION(ver)
const char* SKYBELL_FWVER_STR=SYKBELL_VERSION(SKYBELL_FWVER);
```



判断一个unsigned char 相加是否溢出 的好思想:

```C
 while(i != 0xEC00){
    
        if(i%2 == 0){
            savel = l;    
            l = (l+a[i]);
        }
        else{
            if(savel > l){
                h = (h+a[i])+1;
            }
            else{
                h = (h+a[i]);
            }
        }
        i++;
    }
```

---

临时变量，类型转换的bug:

![1609761529577](./md_att/9B2A30A4-F01C-4223-850C-7A01DDCC621B.png)

---

常见一些变量名前缀加上psz什么意思?比如char*pszBuffer,这是什么意思？_百度知道 - https://zhidao.baidu.com/question/503195581.html



(1条消息) strcasecmp函数_闫钰晨的博客-CSDN博客_strcasecmp函数 - https://blog.csdn.net/yyc794990923/article/details/76268076

---

strdup



Linux 常用C函数（中文版） - http://net.pku.edu.cn/~yhf/linux_c/



[From My Companion]memmem_weixin_34008933的博客-CSDN博客 - https://blog.csdn.net/weixin_34008933/article/details/90684451

