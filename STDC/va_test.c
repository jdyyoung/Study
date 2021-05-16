/*
** 计算指定数量的值的平均数
*/
#include<stdarg.h>
#include<stdio.h>

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

int main(){
	printf("1 2 3 4 5 average = %f \n",average(5,1,2,3,4,5));
	printf("2 3 3 3   average = %f \n",average(4,2,3,3,3));
	return 0;
}