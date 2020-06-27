/*
Linux如何调试内存泄漏？超牛干货奉献给你（代码全）
https://mp.weixin.qq.com/s/8HcPYzmaLxIAXIRHkrU8MA
加参数：-fsanitize=address -g
例如：g++ -fsanitize=address -g test_leak.cc && ./a.out
sanitize 审查
2020.05.30
只有运行到才会报出错误
*/
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

void func1() {malloc(7);}
void func2() {malloc(5);}

int main(){
	//func1();
	srand((unsigned)time(NULL));
	int val =rand();
	printf("random value is %d\n",val);
	if(val % 2 == 0){
		func1();
	}
	//else{
	//	func2();
	//}
	//func2();
	return 0;
}
