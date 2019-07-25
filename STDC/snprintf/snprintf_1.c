#include <stdio.h>
#include <strings.h>
#include <string.h>
/*
 *【情形一】:源串小于目标字符串

 实际上源串为:"123\0"，所以只将字符'1'，'2'，'3', '\0'拷到了目标串,返回值为源串的strlen为3
 * */
int main(void){
	char a[10] = {'\0'};
	int i = 0;
	int ret = 0;
	memset(a, 1, sizeof(a));
	for(i = 0; i < 10; i++){
		printf("a[%d] = %d\n",i,a[i]);
	}
	ret = snprintf(a, 10, "%d", 123);
	printf("ret = %d\n",ret);
	for(i = 0; i < 10; i++)
	{
		printf("a[%d] = %d\n",i,a[i]);
	}
	return 0;
}
