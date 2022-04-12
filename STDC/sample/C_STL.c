#include<stdio.h>                                                                                                                                                      


/*
 typedef struct{

 }STL_int;

结合#define 与 C语言连接符双井号“##”，即可做简单的C++模板
 * */
void set_gpio_1(int state}{
>---printf("%s:state = %d----\n");
}

void set_gpio_2(int state}{
>---printf("%s:state = %d----\n");
}

#define SET_GPIO(num ,state) set_gpio_##num(state)

int main(){
>---SET_GPIO(1,1);
>---SET_GPIO(2,1);
>---return 0;
}
/*
(19条消息) C语言宏定义制作函数模板_锅锅是锅锅的博客-CSDN博客_c宏定义函数 -
 https://blog.csdn.net/u010835747/article/details/119752185
 
 一、介绍
当做协议栈或者批量化函数的时候，内容框架是固定的，如果要写1000个10000个函数确实手敲比较麻烦，一种是用工具批量产生这些函数，另一种就是宏定义，这里给出宏定义解决的方法，利用宏定义制作函数模板

二、原理
1、宏定义中##是一种分隔连接方式，它的作用是先分隔，然后进行强制连接
例如:#define hello(x) hello##x
hello(123) 等效 hello123
#define A1(name, type) type name_##type##type 或
#define A2(name, type) type name####type##type
A1(a1, int); 等效 int name_int_type;
A2(a1, int); 等效 int a1_int_type;
第一个宏定义中，“name"和第一个”“之间，以及第2个”"和第二个"type"之间没有被分隔，所以预处理器会把name##type##type解释成3段：
“name”、“type”、以及“type”，这中间只有“type”是在宏前面出现过的，所以它可以被宏替换。
第二个宏定义中，“name”和第一个“”之间也被分隔了，所以
预处理器会把name##_##type##type解释成4段：“name”、“”、“type”以及“_type”，这其间，就有两个可以被宏替换了。

另外 ##连接的时候会去掉所有的空格

2、宏定义中#是“字符串化”的意思。出现在宏定义中的#是把跟在后面的参数转换成一个字符串
例如：#define M(x) printf(“result = %s”,#x)
M(1) 等效为 printf(“result = %s”,“1”)
#include <stdio.h>

#define create_port(name,content) test##name()\
{printf("hello test1 %s\n", #content);}\
test2##name()\
{printf("hello test2 %s\n", #content);}


create_port(1, guoguo)

int main()
{
	test1();
	test21();
	return 0;
} 
————————————————
版权声明：本文为CSDN博主「锅锅是锅锅」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/u010835747/article/details/119752185

*/
