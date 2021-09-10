#include<stdio.h>
#include<string.h>


/*
main.23-------------------
func.16-------------------
buf = 0123456789
func.19-------------------
*** stack smashing detected ***: ./a.out terminated
已放弃 (核心已转储)
 * */

void func(){
	char buf[5] = {0};
	printf("%s.%d-------------------\n",__func__,__LINE__);
	strcpy(buf,"0123456789");
	printf("buf = %s\n",buf);
	printf("%s.%d-------------------\n",__func__,__LINE__);
}

int main(){
	printf("%s.%d-------------------\n",__func__,__LINE__);
	func();
	printf("%s.%d-------------------\n",__func__,__LINE__);
	return 0;
}
