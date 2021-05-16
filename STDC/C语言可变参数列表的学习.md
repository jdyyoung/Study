### 												C语言可变参数列表的学习

可变参数列表是通过宏来实现的，这些宏定义于stdarg.h头文件，它是标准库的一部分。这个头文件声明了一个类型va_list 和三个宏：va_start、va_arg 和va_end。

va在这里是variable-argument(可变参数)的意思。

原理：利用函数形参的栈地址指针，所以必须至少有一个参数，为形参的首地址。

可变参数列表在stdarg.h文件下的实现：

```C
typedef char  *va_list; 
#define va_start(ap,v)  ap = (va_list)&v + sizeof(v)//ap:用va_list声明的变量 v:第一个参数
#define va_arg(ap,t)    (((t *)ap)++[0]) //t:数据类型,如char,int,double ...
#define va_end(ap)   
```

(1) va_list：    用于声明变量ap，其实也就是char * ；
(2) va_start：	根据到可变参数表的首地址，将第二参数的地址赋给指针ap，也就是ap指向第二个参数
(3) va_arg：   获取当前ap所指向的可变参数的值，并将指针ap指向下一个可变参数，该宏的第二个参数为数据类型；
(4) va_end：   结束可变参数的获取。

运用：

```C
int func(int fmt,...){
	//声明变量ap
	va_list ap；
	
	//初始化变量ap,开始
	 va_start(ap,fmt);
	 
	 .......
     //获取可变参数值并处理
	 int tmp;
	 while(/*根据fmt 得到的界限*/){
	 	tmp = va_arg(ap,t);
	 	......
	 }
	 
    //结束，释放
	 va_end(ap);
	 return tmp;
}
```

**注意：可变参数的使用时，必须特别注意指针越界访问时可能导致程序崩溃的问题！**

例子：

```C
/*
** 计算指定数量的值的平均数
*/
#include<stdarg.h>
float average(int n_values,...){
	va_list var_arg;
	int count;
	float sum = 0;
	
	/*
	**准备访问可变参数。
	*/
	va_start(var_arg,n_values);
	
	/*
	** 添加取自可变参数列表的值。
	*/
	for( count = 0; count < n_values; count++ ){
		sum += va_arg(var_arg,int);
	}
	
	/*
	** 完成处理可变参数
	*/
	va_end(var_arg);
	
	return sum / n_values;
}
```

**个人总结：可变参数列表第一个形参指定可变参数的界限，可变参数个数的说明；第二点可变形参的类型最好相同，不然需要在第一个参数有格式说明。**

参考资料：

可变参数列表与printf()函数的实现 - chenguan - 博客园 - https://www.cnblogs.com/guanguangreat/p/6119278.html

