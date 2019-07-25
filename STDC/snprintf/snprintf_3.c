#include <stdio.h>
#include <strings.h>
#include <string.h>
/*
 *源串大于目标字符串:
 源串为"abcdefghijklmnopq\0"，最多从源串拷贝10个字节(含\0)到
 目标串那么就是拷贝源串的9个字节内容(abcdefghi)再加一个\0到目标串,
 目标串的结果和情形二一样,但是ret返回值变成了17,即strlen("abcdefghijklmnopq")
 */
int main(void){
	char a[10] = {'\0'};
	int i = 0;
	int ret = 0;
	memset(a, 1, sizeof(a));
	for(i = 0; i < 10; i++){
		printf("a[%d] = %d\n",i,a[i]);
	}
	//ret = snprintf(a, 10, "%d", 123);
	//ret = snprintf(a, 10, "%s", "abcdefghi");
	ret = snprintf(a, 10, "%s", "abcdefghijklmnopq");
	printf("ret = %d\n",ret);
	for(i = 0; i < 10; i++)
	{
		printf("a[%d] = %d\n",i,a[i]);
	}
	return 0;
}
